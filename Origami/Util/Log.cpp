#include "Origami/pch.h"
#include "Log.h"

#include "Origami/Concurrency/Thread.h"

//---------------------------------------------------------------------------------
Log::LogCallbackFunction s_CallbackFunction;

//---------------------------------------------------------------------------------
void Log::RegisterCallback( Log::LogCallbackFunction callback )
{
  s_CallbackFunction = callback;
}

//---------------------------------------------------------------------------------
void Log::LogInfo( const char* fmt, ... )
{
#if ( BUILD < BUILD_FINAL )
  if ( s_CallbackFunction != nullptr )
  {
    va_list args;
    va_start( args, fmt );
    s_CallbackFunction( kLogLevelInfo, fmt, args );
    va_end( args );
  }
#endif
}

//---------------------------------------------------------------------------------
void Log::LogError( const char* fmt, ... )
{
#if ( BUILD < BUILD_FINAL )
  if ( s_CallbackFunction != nullptr )
  {
    va_list args;
    va_start( args, fmt );
    s_CallbackFunction( kLogLevelError, fmt, args );
    va_end( args );
  }
#endif
}