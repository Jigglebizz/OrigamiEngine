#include "Origami/pch.h"
#include "Sort.h"

static constexpr size_t kMaxObjectSizeInBytes = 1024; // 1K

//---------------------------------------------------------------------------------
void Swap( uint8_t* A, uint8_t* B, size_t width )
{
  assert( width < kMaxObjectSizeInBytes && "Attempt to quicksort an object that is too large" );

  // TODO: Use some scratch alloc for this
  uint8_t temp[ kMaxObjectSizeInBytes ];
  memcpy_s( &temp, width, A,     width );
  memcpy_s( A,     width, B,     width );
  memcpy_s( B,     width, &temp, width );
}

//---------------------------------------------------------------------------------
template< typename UIntT >
size_t QuicksortPivot( void* buf, size_t stride, size_t len )
{
  uint8_t* u8_buf   = (uint8_t*)buf;
  size_t   low_idx  = 0;
  UIntT    pivot    = *(UIntT*)&u8_buf[ (len - 1) * stride  ];

  for ( size_t upper_idx = 0; upper_idx < len - 1; ++upper_idx)
  {
    if ( *(UIntT*)&u8_buf[ upper_idx * stride ] < pivot )
    {
      Swap( &u8_buf[ low_idx * stride ], &u8_buf[ upper_idx * stride ], stride );
      low_idx++;
    }
  }
  Swap( &u8_buf[ low_idx * stride ], &u8_buf[ (len - 1) * stride ], stride );

  return low_idx;
}

//---------------------------------------------------------------------------------
void QuickSort32(void* buf, size_t stride, size_t len)
{
  ASSERT_MSG( stride >= sizeof(uint32_t), "Cannot sort u32s if elements are smaller than u32" );

  if ( len > 0 )
  {
    size_t pivot_point = QuicksortPivot<uint32_t>(buf, stride, len);
    QuickSort32( buf,                                                             stride, pivot_point           );
    QuickSort32( (void*)( (uint8_t*)buf + ( ( pivot_point + 1 ) * ( stride ) ) ), stride, len - pivot_point - 1 );
  }
}

//---------------------------------------------------------------------------------
void QuickSort64( void* buf, size_t stride, size_t len )
{
  ASSERT_MSG( stride >= sizeof( uint64_t ), "Cannot sort u64s if elements are smaller than u64" );

  if ( len > 0 )
  {
    size_t pivot_point = QuicksortPivot<uint64_t>(buf, stride, len);
    QuickSort64( buf, stride, pivot_point );
    QuickSort64( (void*)( (uint8_t*)buf + ( ( pivot_point + 1 ) * ( stride ) ) ), stride, len - pivot_point - 1 );
  }
}