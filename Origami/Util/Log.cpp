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
#else
  UNREFERENCED_PARAMETER( fmt );
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
#else
  UNREFERENCED_PARAMETER( fmt );
#endif
}

//---------------------------------------------------------------------------------
Log::Timestamp Log::MsToTimestamp( float ms )
{
  Timestamp ts;
  ts.ms = (int)ms % 1000;
  int total_s = (int)ms / 1000;
  ts.s = total_s % 60;
  ts.m = total_s / 60;
  ts.h = total_s / 60 / 60;

  return ts;
}