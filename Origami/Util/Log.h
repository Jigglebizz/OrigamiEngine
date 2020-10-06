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
}