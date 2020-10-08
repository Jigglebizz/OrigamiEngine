#include "Origami/pch.h"
#include "Memory.h"

//---------------------------------------------------------------------------------
void MemZero( void* dst, size_t n )
{
  memset( dst, 0, n );
}

//---------------------------------------------------------------------------------
// Bitset
//---------------------------------------------------------------------------------
void Bitset::InitWithBacking( void* backing, uint32_t num_bits )
{
  m_Set      = (uint8_t*)backing;
  m_NumBytes = num_bits >> 0x03;

  if ( num_bits & 0x7 )
  {
    m_NumBytes++;
  }

  MemZero( backing, m_NumBytes );
}

//---------------------------------------------------------------------------------
bool Bitset::IsSet( uint32_t index ) const
{
  return m_Set[ index >> 0x3 ] & ( 0x01 << ( index & 0x7 ) );
}

//---------------------------------------------------------------------------------
void Bitset::Set( uint32_t index, bool enabled )
{
  uint32_t idx = index >> 0x3;
  uint8_t  bit = ( 0x01 << ( index & 0x7 ) );
  if ( enabled )
  {
    m_Set[ idx ] |= bit;
  }
  else
  {
    m_Set[ idx ] &= ~bit;
  }
}

//---------------------------------------------------------------------------------
// Brian Kernighan’s Algorithm
// https://www.geeksforgeeks.org/count-set-bits-in-an-integer/
uint32_t CountSetBits( uint8_t byte )
{
  uint32_t count = 0;
  while ( byte )
  {
    byte &= byte - 1;
    count++;
  }

  return count;
}

//---------------------------------------------------------------------------------
uint32_t Bitset::GetCountSet( ) const
{
  uint32_t count = 0;
  for ( uint32_t i_byte = 0; i_byte < m_NumBytes; ++i_byte )
  {
    count += CountSetBits( m_Set[ i_byte ] );
  }
  return count;
}

//---------------------------------------------------------------------------------
uint32_t Bitset::FirstUnsetBit( ) const
{
  uint32_t count = 0;
  for ( uint32_t i_byte = 0; i_byte < m_NumBytes; ++i_byte )
  {
    for ( uint8_t i_bit = 0; i_bit < 8; ++i_bit )
    {
      if ( ( m_Set[ i_byte ] & ( 0x01 << i_bit ) ) == 0 )
      {
        return count;
      }
      count++;
    }
  }

  return (uint32_t)-1;
}

//---------------------------------------------------------------------------------
uint32_t Bitset::GetNextSetBit( uint32_t current ) const
{
  uint32_t count = (current == -1) ? 0 : current >> 3 << 3; // start at byte
  for (uint32_t i_byte = count >> 3; i_byte < m_NumBytes; ++i_byte)
  {
    for ( uint8_t i_bit = 0; i_bit < 8; ++i_bit )
    {
      if ( current == -1 || i_byte * 8 + i_bit > current )
      {
        if ( m_Set[ i_byte ] & ( 0x01 << i_bit ) )
        {
          return count;
        }
      }
      count++;
    }
  }

  return (uint32_t)-1;
}