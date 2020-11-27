#include "Origami/pch.h"
#include "Origami/Concurrency/Mutex.h"

#include "Origami/Util/Log.h"

#include <synchapi.h>

//---------------------------------------------------------------------------------
void Mutex::Init( const char* owner )
{
  m_MutexHandle = CreateMutex( NULL, FALSE, NULL );
  if ( m_MutexHandle == NULL )
  {
    Log::LogError( "Could not create mutex for %s! %d\n", owner, GetLastError() );
    return;
  }

  strcpy_s( m_OwnerName, owner );
}

//---------------------------------------------------------------------------------
void Mutex::Destroy()
{
  CloseHandle( m_MutexHandle );
  m_MutexHandle = NULL;
}

//---------------------------------------------------------------------------------
ScopedLock::ScopedLock( Mutex* mutex )
{
  m_Mutex = mutex;
  DWORD wait_result = WaitForSingleObject( m_Mutex->m_MutexHandle, INFINITE );
  if ( wait_result != WAIT_OBJECT_0 )
  {
    Log::LogError( "Could not lock mutex %s! Wait result: %d\n", mutex->m_OwnerName, wait_result );
  }
}

//---------------------------------------------------------------------------------
ScopedLock::~ScopedLock()
{
  if ( ReleaseMutex( m_Mutex->m_MutexHandle ) == FALSE )
  {
    Log::LogError( "Could not release mutex %s! %d\n", m_Mutex->m_OwnerName, GetLastError() );
  };
}


//---------------------------------------------------------------------------------
void ReadWriteMutex::Init( )
{
  InitializeSRWLock( &m_SrwLock );
}

//---------------------------------------------------------------------------------
void ReadWriteMutex::Destroy()
{
}

//---------------------------------------------------------------------------------
ScopedReadLock::ScopedReadLock( ReadWriteMutex* mutex )
{
  m_Mutex = mutex;
  AcquireSRWLockShared( &m_Mutex->m_SrwLock );

}

//---------------------------------------------------------------------------------
ScopedReadLock::~ScopedReadLock()
{
  ReleaseSRWLockShared( &m_Mutex->m_SrwLock );
}

//---------------------------------------------------------------------------------
ScopedWriteLock::ScopedWriteLock( ReadWriteMutex* mutex )
{
  m_Mutex = mutex;
  AcquireSRWLockExclusive( &m_Mutex->m_SrwLock );
}

//---------------------------------------------------------------------------------
ScopedWriteLock::~ScopedWriteLock()
{
  ReleaseSRWLockExclusive( &m_Mutex->m_SrwLock );
}