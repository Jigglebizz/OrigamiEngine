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
    va_list args;
    va_start( args, fmt );
    if (s_CallbackFunction != nullptr)
    {
      s_CallbackFunction( kLogLevelInfo, fmt, args );
    }
    else
    {
      vprintf( fmt, args );
    }
    va_end( args );
#else
  UNREFFED_PARAMETER( fmt );
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
  UNREFFED_PARAMETER( fmt );
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

//---------------------------------------------------------------------------------
void Log::LogRing::InitWithBacking( void* backing, uint32_t size )
{
  m_Buffer = (char*)backing;
  m_BufferSize = size;
  MemZero( m_Buffer, m_BufferSize );

  m_WriteHead = 0;
  m_ReadHead  = 0;

  m_HasLooped     = false;
  m_BufferLoopIdx = m_BufferSize - 1;
}

//---------------------------------------------------------------------------------
void Log::LogRing::Destroy()
{

}

//---------------------------------------------------------------------------------
void Log::LogRing::Append( const char* string )
{
  uint32_t string_len = StrLen(string);

  if ( m_WriteHead + string_len < m_BufferLoopIdx )
  {
    memcpy( &m_Buffer[ m_WriteHead ], string, string_len );
    m_WriteHead += string_len;
  }
  else
  {
    m_HasLooped = true;

    uint32_t initial_consume_len = m_BufferLoopIdx - m_WriteHead;
    memcpy( &m_Buffer[ m_WriteHead ], string, initial_consume_len );

    m_WriteHead = string_len - initial_consume_len;
    memcpy( &m_Buffer[ 0 ], &string[ initial_consume_len ], m_WriteHead );
  }

  if ( m_HasLooped )
  {
    m_ReadHead = ( m_WriteHead != m_BufferLoopIdx - 1 ) ? m_WriteHead + 1 : 0;
    if ( m_ReadHead != 0 )
    {
      m_Buffer[ m_ReadHead - 1 ] = '\0';
    }
  }
}

//---------------------------------------------------------------------------------
// There are two parts to a full log. Sometimes log B can be nullptr,
// in which case it can be ignored. Normal usage to print the full log 
// should be:
//
// printf( ring.GetLogA() );
// if ( const char* log_b = ring.GetLogB() )
// {
//   printf( log_b );
// }
const char* Log::LogRing::GetLogA( ) const
{
  return &m_Buffer[ m_ReadHead ];
}

//---------------------------------------------------------------------------------
// There are two parts to a full log. Sometimes log B can be nullptr,
// in which case it can be ignored. Normal usage to print the full log 
// should be:
//
// printf( ring.GetLogA() );
// if ( const char* log_b = ring.GetLogB() )
// {
//   printf( log_b );
// }
const char* Log::LogRing::GetLogB() const
{
  if ( m_ReadHead != 0 && m_HasLooped == true )
  {
    return &m_Buffer[ 0 ];
  }
  
  return nullptr;
}