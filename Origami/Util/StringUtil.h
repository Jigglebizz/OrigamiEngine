#pragma once

//---------------------------------------------------------------------------------
static constexpr unsigned int StrLen(const char* str)
{
  return *str ? 1 + StrLen( str + 1) : 0;  
}