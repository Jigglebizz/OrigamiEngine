#pragma once

namespace Log
{
  //---------------------------------------------------------------------------------
  enum LogConstants : uint8_t
  {
    kLogLevelInfo  = 0x01,
    kLogLevelError = 0x02
  };

  //---------------------------------------------------------------------------------
  typedef void(*LogCallbackFunction)( uint8_t flags, const char* fmt, va_list args );

  //---------------------------------------------------------------------------------
  void ENGINE_API RegisterCallback( LogCallbackFunction callback );
  void ENGINE_API LogInfo( const char* fmt, ... );
  void ENGINE_API LogError( const char* fmt, ...);

  //---------------------------------------------------------------------------------
  struct Timestamp
  {
    int h, m, s, ms;
  };
  Timestamp ENGINE_API MsToTimestamp( float ms );

  //---------------------------------------------------------------------------------
  class LogRing
  {
  private:
    uint32_t m_WriteHead;
    uint32_t m_ReadHead;
    uint32_t m_BufferLoopIdx;
    uint32_t m_BufferSize;
    bool     m_HasLooped;
    char*    m_Buffer;

  public:
    void InitWithBacking ( void* backing, uint32_t size );
    void Destroy         ( );

    void Append          ( const char* string );

    const char* GetLogA  ( ) const;
    const char* GetLogB  ( ) const;
  };
}