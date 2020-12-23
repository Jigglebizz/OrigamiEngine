#include "Origami/pch.h"
#include "Origami/Memory/Memory.h"

#include "Origami/Util/Log.h"

//#define MEMORY_LOGGING

//---------------------------------------------------------------------------------
void* Memory::g_GlobalBacking;
MemAllocHeap g_DynamicHeap;

//---------------------------------------------------------------------------------
void Memory::InitGlobalBacking( size_t size )
{
  g_GlobalBacking = malloc( size );
  ASSERT_MSG( g_GlobalBacking != nullptr, "Could not allocate memory." );
  g_DynamicHeap.InitWithBacking( g_GlobalBacking, size, "Dynamic Heap" );
}

void Memory::DestroyGlobalBacking()
{
  g_DynamicHeap.Destroy();
  free( g_GlobalBacking );
}

//---------------------------------------------------------------------------------
void MemZero( void* dst, size_t n )
{
  memset( dst, 0, n );
}

//---------------------------------------------------------------------------------
// Bitset
//---------------------------------------------------------------------------------
uint32_t Bitset::GetNumberOfBytesForElements(uint32_t num_elements)
{
  uint32_t bytes = num_elements >> 3;
  if ( num_elements & 0x7 )
  {
    bytes++;
  }
  return bytes;
}

//---------------------------------------------------------------------------------
void Bitset::InitFromDynamicHeap( uint32_t num_bits )
{
  m_NumBytes = GetNumberOfBytesForElements( num_bits );

  m_Set = (uint8_t*)g_DynamicHeap.Alloc( m_NumBytes );
  if ( m_Set == nullptr )
  {
    Log::LogError( "Could not allocate memory for bitset!" );
    return;
  }

  MemZero( m_Set, m_NumBytes );
  m_OwnsBacking = true;
}

//---------------------------------------------------------------------------------
void Bitset::InitWithBacking( void* backing, uint32_t num_bits )
{
  m_Set      = (uint8_t*)backing;
  m_NumBytes = GetNumberOfBytesForElements( num_bits );

  MemZero( backing, m_NumBytes );
  m_OwnsBacking = false;
}

//---------------------------------------------------------------------------------
void Bitset::Destroy()
{
  if ( m_OwnsBacking && m_Set )
  {
    g_DynamicHeap.Free( m_Set );
  }
}

//---------------------------------------------------------------------------------
void* Bitset::GetBackingBase()
{
  return m_Set;
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
    if ( m_Set[ i_byte ] == -1 )
    {
      count += 8;
      continue;
    }
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
    if ( m_Set[ i_byte ] == 0 )
    {
      count += 8;
      continue;
    }
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

//---------------------------------------------------------------------------------
void MemAllocHeap::InitWithBacking( void* data, size_t size, const char* name )
{
  ASSERT_MSG( StrLen( name ) <= kMaxHeapNameSize, "Heap name is too long" );
  strcpy_s( m_HeapName, name );

  m_Tlsf = tlsf_create_with_pool( data, size );
  m_OwnedData = nullptr;

#ifdef MEMORY_LOGGING
  Log::LogInfo( "Created TLSF heap %s of size %llu. Data is at %#08x\n", name, size, data );
#endif
}

//---------------------------------------------------------------------------------
void MemAllocHeap::InitFromTemplate( const HeapTemplate* heap_template )
{
  ASSERT_MSG( StrLen( heap_template->m_Name ) <= kMaxHeapNameSize, "Heap name is too long" );
  strcpy_s( m_HeapName, heap_template->m_Name );

  m_OwnedData = g_DynamicHeap.Alloc( heap_template->m_Size );
  m_Tlsf = tlsf_create_with_pool( m_OwnedData, heap_template->m_Size );

#ifdef MEMORY_LOGGING
  Log::LogInfo( "Created TLSF heap %s of size %llu. Data is at %#08x\n", m_Name, heap_template->m_Size, m_OwnedData );
#endif
}

//---------------------------------------------------------------------------------
void MemAllocHeap::Destroy()
{
  tlsf_destroy( m_Tlsf );

  if ( m_OwnedData != nullptr )
  {
    g_DynamicHeap.Free( m_OwnedData );
  }

#ifdef MEMORY_LOGGING
  Log::LogInfo("Destroyed TLSF heap %s\n", m_HeapName );
#endif
}

//---------------------------------------------------------------------------------
void* MemAllocHeap::Alloc( size_t size )
{
  return tlsf_malloc( m_Tlsf, size );
}

//---------------------------------------------------------------------------------
void* MemAllocHeap::MemAlign( size_t align, size_t bytes )
{
  return tlsf_memalign( m_Tlsf, align, bytes );
}

//---------------------------------------------------------------------------------
void* MemAllocHeap::Realloc( void* ptr, size_t size )
{
return tlsf_realloc( m_Tlsf, ptr, size );
}

//---------------------------------------------------------------------------------
void MemAllocHeap::Free( void* ptr )
{
  tlsf_free( m_Tlsf, ptr );
}

//---------------------------------------------------------------------------------
HeapAuditInfo MemAllocHeap::Audit()
{
  HeapAuditInfo info;
  info.size = tlsf_size();

  return info;
}