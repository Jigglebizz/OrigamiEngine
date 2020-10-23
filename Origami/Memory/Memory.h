#pragma once

#include "Origami/Memory/tlsf.h"


//---------------------------------------------------------------------------------
void ENGINE_API MemZero( void* dst, size_t n );


//---------------------------------------------------------------------------------
// Bitset
//---------------------------------------------------------------------------------
class Bitset
{
private:
  uint8_t* m_Set;
  uint32_t m_NumBytes;

public:


         void           ENGINE_API InitWithBacking    ( void* backing, uint32_t num_bits );
                        
         bool           ENGINE_API IsSet              ( uint32_t index                      ) const;
         void           ENGINE_API Set                ( uint32_t index, bool enabled = true );
  inline void           ENGINE_API Unset              ( uint32_t index                      );
                        
         uint32_t       ENGINE_API GetCountSet        ( ) const;
         uint32_t       ENGINE_API FirstUnsetBit      ( ) const;
         uint32_t       ENGINE_API GetNextSetBit      ( uint32_t current = -1 ) const;
};

//---------------------------------------------------------------------------------
void Bitset::Unset( uint32_t index )
{
  Set( index, false );
}


//---------------------------------------------------------------------------------
// MemAllocHeap
//---------------------------------------------------------------------------------
struct HeapAuditInfo
{
  size_t size;
  size_t align_size;
  size_t block_size_min;
  size_t block_size_max;
  size_t bookkeeping_mem;
};

class MemAllocHeap
{
private:
  static constexpr uint32_t kMaxHeapNameSize = 32;

  const char m_HeapName[ kMaxHeapNameSize ];
  tlsf_t     m_Tlsf;
public:
  void          ENGINE_API InitWithBacking ( void* data, size_t size, char* name );
  void          ENGINE_API Destroy         ();

  void*         ENGINE_API Alloc           ( size_t size );
  void*         ENGINE_API MemAlign        ( size_t align, size_t bytes );
  void*         ENGINE_API Realloc         ( void* ptr, size_t size );
  void          ENGINE_API Free            ( void* ptr );

  HeapAuditInfo ENGINE_API Audit   ();
};