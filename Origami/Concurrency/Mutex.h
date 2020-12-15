#pragma once

#include <synchapi.h>

class Mutex;

//---------------------------------------------------------------------------------
class ScopedLock
{
  Mutex* m_Mutex;

public:
  ENGINE_API ScopedLock( Mutex* mutex );
  ENGINE_API ~ScopedLock();
};

//---------------------------------------------------------------------------------
class Mutex
{
  friend class ScopedLock;

  HANDLE m_MutexHandle;
  char   m_OwnerName[ 32 ];
public:
  void ENGINE_API Init( const char* owner = "<UNKNOWN>" );
  void ENGINE_API Destroy();
};

//---------------------------------------------------------------------------------
class ReadWriteMutex
{
  friend class ScopedReadLock;
  friend class ScopedWriteLock;

  SRWLOCK m_SrwLock;

public:
  void ENGINE_API Init( );
  void ENGINE_API Destroy();
};

//---------------------------------------------------------------------------------
class ScopedReadLock
{
  ReadWriteMutex* m_Mutex;

public:
  ENGINE_API ScopedReadLock( ReadWriteMutex* mutex );
  ENGINE_API ~ScopedReadLock();
};

//---------------------------------------------------------------------------------
class ScopedWriteLock
{
  ReadWriteMutex* m_Mutex;

public:
  ENGINE_API ScopedWriteLock( ReadWriteMutex* mutex );
  ENGINE_API ~ScopedWriteLock();
};