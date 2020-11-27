#pragma once

#include <synchapi.h>

class Mutex;

//---------------------------------------------------------------------------------
class ScopedLock
{
  Mutex* m_Mutex;

public:
  ScopedLock( Mutex* mutex );
  ~ScopedLock();
};

//---------------------------------------------------------------------------------
class Mutex
{
  friend class ScopedLock;

  HANDLE m_MutexHandle;
  char   m_OwnerName[ 32 ];
public:
  void Init( const char* owner = "<UNKNOWN>" );
  void Destroy();
};

//---------------------------------------------------------------------------------
class ReadWriteMutex
{
  friend class ScopedReadLock;
  friend class ScopedWriteLock;

  SRWLOCK m_SrwLock;

public:
  void Init( );
  void Destroy();
};

//---------------------------------------------------------------------------------
class ScopedReadLock
{
  ReadWriteMutex* m_Mutex;

public:
  ScopedReadLock( ReadWriteMutex* mutex );
  ~ScopedReadLock();
};

//---------------------------------------------------------------------------------
class ScopedWriteLock
{
  ReadWriteMutex* m_Mutex;

public:
  ScopedWriteLock( ReadWriteMutex* mutex );
  ~ScopedWriteLock();
};