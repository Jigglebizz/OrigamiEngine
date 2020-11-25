#pragma once

//---------------------------------------------------------------------------------
static constexpr unsigned int StrLen(const char* str)
{
  return *str ? 1 + StrLen( str + 1) : 0;  
}

//---------------------------------------------------------------------------------
inline void ENGINE_API StrCpy( char* dst, const char* src );

//---------------------------------------------------------------------------------
static constexpr void RemoveTrailingWhitespace( char* str, size_t len )
{
  size_t cursor = len - 1;
  while ( str[ cursor ] < '!' )
  {
    str[ cursor-- ] = 0;
  }
}

//---------------------------------------------------------------------------------
void StrCpy( char* dst, const char* src )
{
  memcpy( dst, src, (size_t)StrLen( src ) + 1 );
}