#pragma once

class Thread;
typedef void(*ThreadFunction)( Thread*, void* );

//---------------------------------------------------------------------------------
class Thread
{
private:
  static constexpr uint32_t kInvalidId = 0xffff'ffff;

  enum Flags : uint8_t
  {
    kAlive         = 0x01,
    kRequestStop   = 0x02,
    kActiveRequest = 0x04,
    kIsAudioThread = 0x10,
  };
  
  uint32_t          m_Id;
  HANDLE            m_SystemThreadHandle;
  volatile uint8_t  m_Flags;
  ThreadFunction    m_Function;
  void*             m_FunctionParams;

  friend static DWORD WINAPI StaticThreadExecute(void* param);
  void ThreadExecute( );
public:
  static constexpr ThreadFunction kLastFunction = nullptr;

  Thread( );

         void    Start           ( ThreadFunction func = kLastFunction, void* params = nullptr );
         void    RequestStop     ( );
  const  bool    Stopped         ( ) const;
         void    Join            ( );
  const  bool    Joinable        ( ) const;
  const  bool    StopRequested   ( ) const;
                 
  inline void    EnsureRequests  ( );
  inline void    ReceiptRequests ( );
                 
         void    SetIsAudioThread( bool enabled = true ) { m_Flags = ( enabled ) ? m_Flags | kIsAudioThread : m_Flags & ~kIsAudioThread; }

  //---------------------------------------------------------------------------------
  static uint32_t GetNumLogicalCores();
};

//---------------------------------------------------------------------------------
void Thread::EnsureRequests()
{
  m_Flags |= kActiveRequest;
}

//---------------------------------------------------------------------------------
// For use internal to the thread
void Thread::ReceiptRequests()
{
  m_Flags &= kActiveRequest;
}