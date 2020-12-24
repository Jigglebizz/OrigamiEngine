#pragma once

#include "Origami/Memory/tlsf.h"


//---------------------------------------------------------------------------------
namespace Memory
{
  extern void* g_GlobalBacking;

  void InitGlobalBacking( size_t size );
  void DestroyGlobalBacking();
}

//---------------------------------------------------------------------------------
void ENGINE_API MemZero( void* dst, size_t n );


//---------------------------------------------------------------------------------
// Bitset
//---------------------------------------------------------------------------------
class MemAllocHeap;

class Bitset
{
private:
  uint8_t*      m_Set;
  uint32_t      m_NumBytes;
  MemAllocHeap* m_OwningHeap;

public:
  Bitset() : m_OwningHeap( nullptr ) {}

  static uint32_t       ENGINE_API GetNumberOfBytesForElements ( uint32_t num_elements );

         void           ENGINE_API InitFromHeap                ( MemAllocHeap* heap, uint32_t num_bits );
         void           ENGINE_API InitWithBacking             ( void* backing, uint32_t num_bits );
         void*          ENGINE_API GetBackingBase              ( );
         void           ENGINE_API Destroy                     ( );
                                                               
         bool           ENGINE_API IsSet                       ( uint32_t index                      ) const;
         void           ENGINE_API Set                         ( uint32_t index, bool enabled = true );
         void           ENGINE_API Unset                       ( uint32_t index                      );
                                                               
         uint32_t       ENGINE_API GetCountSet                 ( ) const;
         uint32_t       ENGINE_API FirstUnsetBit               ( ) const;
         uint32_t       ENGINE_API GetNextSetBit               ( uint32_t current = -1 ) const;
};

//---------------------------------------------------------------------------------
inline void Bitset::Unset( uint32_t index )
{
  Set( index, false );
}


//---------------------------------------------------------------------------------
// MemAllocHeap
//---------------------------------------------------------------------------------
struct HeapTemplate
{
  uint32_t     m_NameHash;
  const char*  m_Name;
  size_t       m_Size;
};

struct HeapAuditInfo
{
  size_t size;
  size_t align_size;
  size_t block_size_min;
  size_t block_size_max;
  size_t bookkeeping_mem;
};

//---------------------------------------------------------------------------------
class MemAllocHeap
{
private:
  void* m_OwnedData;
  tlsf_t m_Tlsf;

  static constexpr uint32_t kMaxHeapNameSize = 32;
  char   m_HeapName[ kMaxHeapNameSize ];
public:


  void          ENGINE_API InitWithBacking ( void* data, size_t size, const char* name );
  void          ENGINE_API InitFromTemplate( const HeapTemplate* heap_template );
  void          ENGINE_API Destroy         ();

  void*         ENGINE_API Alloc           ( size_t size );
  void*         ENGINE_API MemAlign        ( size_t align, size_t bytes );
  void*         ENGINE_API Realloc         ( void* ptr, size_t size );
  void          ENGINE_API Free            ( void* ptr );

  const char*   ENGINE_API GetName() const;

  HeapAuditInfo ENGINE_API Audit   ();
};

//---------------------------------------------------------------------------------
extern MemAllocHeap g_DynamicHeap;