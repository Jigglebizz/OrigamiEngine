#include "Origami/pch.h"
#include "Kami/AssetServer.h"
#include "Origami/Concurrency/Thread.h"
#include "Origami/Util/Log.h"

#include <ws2tcpip.h>

//---------------------------------------------------------------------------------
AssetServer::ServerStatus s_Status;
Thread                    s_Thread;
SOCKET                    s_ListenSocket;
SOCKET                    s_ClientSockets[ AssetServer::kMaxClientConnections ];

//---------------------------------------------------------------------------------
void SocketThreadFunction( Thread* thread, void* params )
{
  UNREFFED_PARAMETER( thread );
  UNREFFED_PARAMETER( params );
  // Service client connection requests



  // Service client comm

  //if ( ( s = socket(AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
  //{
  //  Log::LogInfo( "" )
  //}

}

//---------------------------------------------------------------------------------
void AssetServer::Init()
{
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

  s_Thread.Start( &SocketThreadFunction, nullptr );
}

//---------------------------------------------------------------------------------
void AssetServer::Destroy()
{
  s_Thread.RequestStop();
  while ( s_Thread.Joinable() == false );
  s_Thread.Join();

  closesocket( s_ListenSocket );
  WSACleanup();
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