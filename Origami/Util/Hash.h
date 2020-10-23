#pragma once

#include "StringUtil.h"

namespace crc
{
  //---------------------------------------------------------------------------------
  static constexpr auto crc_table_idx(uint32_t i)
  {
    uint32_t polynomial = 0xEDB88320;

    uint32_t c = i;
    for (size_t j = 0; j < 8; j++)
    {
      if (c & 1) {
        c = polynomial ^ (c >> 1);
      }
      else {
        c >>= 1;
      }
    }
    return c;
  }

  //---------------------------------------------------------------------------------
  // Trying to avoid using templates in this project. This is here just to do some
  // compile-time generation of the crc LUT
  template< int N>
  struct crc_table
  {
    uint32_t arr[N];

    //---------------------------------------------------------------------------------
    constexpr crc_table()
      : arr()
    {
      for (uint32_t i_tab = 0; i_tab < N; ++i_tab)
      {
        arr[i_tab] = crc::crc_table_idx(i_tab);
      }
    }

    //---------------------------------------------------------------------------------
    constexpr uint32_t operator[](unsigned idx) const
    {
      return arr[idx];
    }
  };
}

//---------------------------------------------------------------------------------
constexpr inline uint32_t Crc32( uint32_t initial, const char* buf, size_t len )
{
  constexpr auto table = crc::crc_table< 256 >();

  uint32_t c = initial ^ 0xFFFFFFFF;
  const char* u = buf;
  for (size_t i = 0; i < len; ++i)
  {
    c = table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
  }
  return c ^ 0xFFFFFFFF;
}

//---------------------------------------------------------------------------------
constexpr inline uint32_t ENGINE_API Crc32( const char* string )
{
  return Crc32( 0, string, StrLen(string) );
}