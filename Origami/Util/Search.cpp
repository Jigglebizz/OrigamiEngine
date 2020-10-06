#include "Origami/pch.h"
#include "Search.h"

size_t BinarySearch32( uint32_t value, const void* buf, size_t stride, size_t len )
{
  const uint8_t* u8_buf = (const uint8_t*)buf;

  size_t end            = len;
  size_t beginning      = 0;

  size_t search_idx     = len >> 1;
  uint32_t idx_value    = *(uint32_t*)&u8_buf[ search_idx * stride ];

  while ( idx_value != value )
  {
    if ( end == beginning )
    {
      return -1;
    }
    
    if ( value < idx_value )
    {
      end = search_idx;
    }
    else if ( value > idx_value )
    {
      beginning = search_idx + 1;
    }

    search_idx = ( ( end - beginning ) >> 1 ) + beginning;
    idx_value  = *(uint32_t*)&u8_buf[ search_idx * stride];
  }

  return search_idx;
}