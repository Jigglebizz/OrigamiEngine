#include "Origami/pch.h"
#include "Thread.h"

//---------------------------------------------------------------------------------
Thread::Thread( )
  : m_Id( kInvalidId )
  , m_SystemThreadHandle( nullptr )
  , m_Flags( 0x00 )
  , m_Function( nullptr )
{
}

//---------------------------------------------------------------------------------
static DWORD WINAPI StaticThreadExecute( void* param )
{
  Thread* This = (Thread*)param;
  This->ThreadExecute();
  return 0;
}

//---------------------------------------------------------------------------------
void Thread::Start( ThreadFunction function )
{
  if ( function != kLastFunction )
  {
    m_Function = function;
  }

  assert( function && "Function cannot be nullptr" );

  m_SystemThreadHandle = CreateThread(
    NULL,
    0,   // Stack size?
    StaticThreadExecute,
    (void*)this,
    NULL,
    (LPDWORD)&m_Id
  );

  assert( m_SystemThreadHandle != nullptr && "Failed to create thread!" );

  if ( m_Flags & kIsAudioThread )
  {
    assert( SetThreadPriority( m_SystemThreadHandle, THREAD_PRIORITY_TIME_CRITICAL ) && "Could not set audio thread to high priority!");
  }
}

//---------------------------------------------------------------------------------
void Thread::ThreadExecute()
{
  m_Flags |= kAlive;
  while ( ( m_Flags & kRequestStop ) == 0 || ( m_Flags & kActiveRequest ) )
  {
    m_Function( this );
  }
  m_Flags &= ~( kAlive | kRequestStop );
}

//---------------------------------------------------------------------------------
void Thread::RequestStop()
{
  m_Flags |= kRequestStop;
}

//---------------------------------------------------------------------------------
const bool Thread::Stopped() const
{
  return ( m_Flags & kAlive ) == 0;
}

//---------------------------------------------------------------------------------
void Thread::Join()
{
  assert( Joinable() && "Thread must be joined first" );
  CloseHandle( m_SystemThreadHandle );
}

//---------------------------------------------------------------------------------
const bool Thread::Joinable() const
{
  return ( m_Flags & ( kAlive | kRequestStop ) ) == 0;
}

//---------------------------------------------------------------------------------
uint32_t Thread::GetNumLogicalCores()
{
  static uint32_t num_cores = 0;

  if ( num_cores == 0 )
  {
    SYSTEM_INFO sys_info;
    GetSystemInfo( &sys_info );
    num_cores = sys_info.dwNumberOfProcessors;
  }

  return num_cores;
}