#pragma once

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
