#include "Origami/pch.h"
#include "Kami/AssetServer.h"
#include "Origami/Concurrency/Thread.h"
#include "Origami/Concurrency/Mutex.h"
#include "Origami/Util/Log.h"

#include <ws2tcpip.h>

//---------------------------------------------------------------------------------
AssetServer::ServerStatus s_Status;
Thread                    s_RequestThread;
Thread                    s_CommThread;

ReadWriteMutex            s_ListenerSocketMutex;
SOCKET                    s_ListenSocket;

ReadWriteMutex            s_ClientSocketsMutex;
uint32_t                  s_NumMaxClientConnections;
uint32_t                  s_NumConnectedClients;
Bitset                    s_ClientSocketBitset;
SOCKET*                   s_ClientSockets;

//---------------------------------------------------------------------------------
void ConnectionRequestThreadFunction( Thread* thread, void* params )
{
  UNREFFED_PARAMETER( thread );
  UNREFFED_PARAMETER( params );

  if ( s_NumConnectedClients != s_NumMaxClientConnections )
  {

    SOCKET new_client;

    {
      ScopedReadLock listener_lock( &s_ListenerSocketMutex );
      new_client = accept( s_ListenSocket, NULL, NULL );
      if ( new_client == INVALID_SOCKET )
      {
        Log::LogError( "Accept incoming cconnection failed with %#08x\n", WSAGetLastError() );
        return;
      }
    }

    ScopedWriteLock clients_lock( &s_ClientSocketsMutex );

    uint32_t client_id = s_ClientSocketBitset.FirstUnsetBit();
    s_ClientSockets[ client_id ] = new_client;
    s_NumConnectedClients++;
  }
}

//---------------------------------------------------------------------------------
void CommunicationThreadFunction( Thread* thread, void* params )
{
  UNREFFED_PARAMETER( thread );
  UNREFFED_PARAMETER( params );

  uint32_t client_id;
  {
    ScopedReadLock read_lock( &s_ClientSocketsMutex );
    client_id = s_ClientSocketBitset.GetNextSetBit();
  }

  while ( client_id != -1 )
  {
    SOCKET client;

    {
      ScopedReadLock read_lock( &s_ClientSocketsMutex );
      client    = s_ClientSockets[ client_id ];
      client_id = s_ClientSocketBitset.GetNextSetBit( client_id );
    }


  }
}

//---------------------------------------------------------------------------------
void AssetServer::Init( uint32_t max_connections )
{
  s_ListenerSocketMutex.Init();
  s_ClientSocketsMutex.Init();

  s_NumMaxClientConnections   = max_connections;
  s_NumConnectedClients       = 0;
  s_ClientSockets             = (SOCKET*)g_DynamicHeap.Alloc( sizeof( SOCKET ) * max_connections );
  void* client_bitset_backing =          g_DynamicHeap.Alloc( (size_t)( max_connections >> 3 ) + 1 );

  s_ClientSocketBitset.InitWithBacking( client_bitset_backing, max_connections );

  WSADATA wsa;
  int rc;

  rc = WSAStartup(MAKEWORD(2, 2), &wsa);
  if ( rc != 0 )
  {
    Log::LogError( "Failed to initialize server: %#08x\n", rc );
    return;
  }

  struct addrinfo hints;
  MemZero( &hints, sizeof( hints ) );
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags    = AI_PASSIVE;

  static char port_str[ 6 ];
  snprintf( port_str, sizeof( port_str ), "%d", AssetServer::kTcpPort );
  struct addrinfo* addr_result = nullptr;

  rc = getaddrinfo( NULL, port_str, &hints, &addr_result );
  if ( rc != 0 )
  {
    Log::LogError( "getaddrinfo failed with error %#08x\n", rc );
    WSACleanup();
    return;
  }

  s_ListenSocket = INVALID_SOCKET;
  s_ListenSocket = socket( addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol );
  if ( s_ListenSocket == INVALID_SOCKET )
  {
    Log::LogError("socket failed with error: %#08x\n", WSAGetLastError() );
    freeaddrinfo( addr_result );
    closesocket( s_ListenSocket );
    WSACleanup();
    return;
  }

  rc = bind( s_ListenSocket, addr_result->ai_addr, (int)addr_result->ai_addrlen );
  if ( rc == SOCKET_ERROR )
  {
    Log::LogError( "bind failed with error: %#08x\n", WSAGetLastError() );
    freeaddrinfo( addr_result );
    closesocket( s_ListenSocket );
    WSACleanup();
    return;
  }

  freeaddrinfo( addr_result );

  rc = listen( s_ListenSocket, SOMAXCONN );
  if ( rc == SOCKET_ERROR )
  {
    Log::LogError( "listen failed with error: %#08x\n", WSAGetLastError() );
    closesocket( s_ListenSocket );
    WSACleanup();
    return;
  }

  s_Status = kStatusInitialized;

  s_CommThread.Start   ( &CommunicationThreadFunction,     nullptr );
  s_RequestThread.Start( &ConnectionRequestThreadFunction, nullptr );
}

//---------------------------------------------------------------------------------
void AssetServer::Destroy()
{
  s_RequestThread.RequestStop();
  s_CommThread.RequestStop();

  while ( s_RequestThread.Joinable() == false );
  s_RequestThread.Join();

  while ( s_CommThread.Joinable() == false );
  s_CommThread.Join();

  closesocket( s_ListenSocket );
  WSACleanup();

  g_DynamicHeap.Free( s_ClientSocketBitset.GetBackingBase() );
  g_DynamicHeap.Free( s_ClientSockets );

  s_ListenerSocketMutex.Destroy();
  s_ClientSocketsMutex.Destroy();

  s_Status = kStatusDead;
}

//---------------------------------------------------------------------------------
AssetId AssetServer::GetNextRequest()
{
  return AssetId::kInvalidAssetId;
}

//---------------------------------------------------------------------------------
AssetServer::ServerStatus AssetServer::GetStatus()
{
  return s_Status;
}