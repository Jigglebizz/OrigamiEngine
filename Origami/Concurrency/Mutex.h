#pragma once

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