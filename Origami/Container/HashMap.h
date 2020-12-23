#pragma once

DISABLE_OPTS

//---------------------------------------------------------------------------------
template< typename K, typename V >
class HashMap
{
private:
  struct Element
  {
    Element* m_Next;
    K m_Key;
    V m_Value;
  };

  struct BucketHead
  {
    Element* elements;
  };
  
  uint8_t*      m_Backing;
  size_t        m_BackingSize;
  uint32_t      m_NumBuckets;
  MemAllocHeap* m_OwningHeap;

  BucketHead*   m_BucketsBase;
  Element*      m_FreeElementHead;

  uint32_t    HashFunction( K input ) const;

public:
  HashMap() : m_Backing( nullptr ) {}

  static constexpr uint32_t kDefaultBucketNum = 16;

         void     ENGINE_API InitWithBacking        ( void* backing, size_t size, uint32_t num_buckets = kDefaultBucketNum );
         void     ENGINE_API InitOnHeap             ( MemAllocHeap* heap, uint32_t desired_capacity, uint32_t num_buckets = kDefaultBucketNum );
         void     ENGINE_API Destroy                ( );
         
         void     ENGINE_API Insert                 ( const K& key, const V& value );
         V*       ENGINE_API At                     ( const K& key );
         void     ENGINE_API Remove                 ( const K& key );
         uint32_t ENGINE_API GetCapacity            ( ) const;
  static size_t   ENGINE_API GetRequiredBackingSize ( uint32_t capacity, uint32_t num_buckets = kDefaultBucketNum );

};

//---------------------------------------------------------------------------------
template< typename K, typename V >
uint32_t HashMap<K, V>::GetCapacity ( ) const
{
  if ( m_Backing == nullptr )
  {
    return 0;
  }

  uint32_t buckets_size = m_NumBuckets * sizeof ( BucketHead );
  return (uint32_t)(( m_BackingSize - buckets_size ) / sizeof( Element ));
}

//---------------------------------------------------------------------------------
template< typename K, typename V >
size_t HashMap< K, V >::GetRequiredBackingSize( uint32_t capacity, uint32_t num_buckets )
{
  size_t buckets_size = num_buckets * sizeof ( BucketHead );
  return buckets_size + ( capacity * sizeof( Element ) );
}

//---------------------------------------------------------------------------------
template< typename K, typename V >
void HashMap< K, V >::InitWithBacking( void* backing, size_t size, uint32_t num_buckets )
{
  size_t buckets_size = num_buckets * sizeof ( BucketHead );
  ASSERT_MSG( size >= buckets_size + sizeof( Element ), "Hashmap is not large enough to store any elements!" );

  m_Backing     = (uint8_t*)backing;
  m_BackingSize = size;

  m_BucketsBase = (BucketHead*)m_Backing;
  m_NumBuckets  = num_buckets;
  MemZero( m_BucketsBase, sizeof( BucketHead ) * m_NumBuckets );

  Element* elements_base = ( Element* )( m_Backing + sizeof( BucketHead ) * m_NumBuckets );
  uint32_t num_elements = GetCapacity();
  for ( uint32_t i_element = 0; i_element < num_elements; ++i_element )
  {
    MemZero( &elements_base[ i_element ], sizeof ( Element ) );
    elements_base[ i_element ].m_Next = &elements_base[ i_element + 1 ];
  }

  elements_base[ num_elements - 1 ].m_Next = nullptr;

  m_FreeElementHead = elements_base;
  m_OwningHeap      = nullptr;
}

//---------------------------------------------------------------------------------
template< typename K, typename V >
void HashMap<K, V>::InitOnHeap( MemAllocHeap* heap, uint32_t desired_capacity, uint32_t num_buckets)
{
  size_t required_size = GetRequiredBackingSize( desired_capacity, num_buckets );
  m_Backing = (uint8_t*)heap->Alloc( required_size );
  ASSERT_MSG( m_Backing != nullptr, "Not enough space in heap for hashmap!" );

  InitWithBacking( m_Backing, required_size, num_buckets );

  m_OwningHeap = heap;
}

//---------------------------------------------------------------------------------
template< typename K, typename V >
void HashMap<K, V>::Destroy()
{
  if ( m_OwningHeap != nullptr )
  {
    m_OwningHeap->Free( m_Backing );
  }
}


//---------------------------------------------------------------------------------
template< typename K, typename V >
void HashMap< K, V >::Insert( const K& key, const V& value )
{
  ASSERT_MSG( m_Backing != nullptr, "HashMap not initialized!" );

  Element* new_elem = m_FreeElementHead;
  ASSERT_MSG(new_elem != nullptr, "Hashmap is out of space!");

  m_FreeElementHead = m_FreeElementHead->m_Next;
  new_elem->m_Next = nullptr;

  new_elem->m_Key = key;
  new_elem->m_Value = value;


  uint32_t i_bucket = HashFunction( key );
  Element* list_elem = m_BucketsBase[ i_bucket ].elements;
  if ( list_elem == nullptr )
  {
    m_BucketsBase[ i_bucket ].elements = new_elem;
  }
  else
  {
    while ( list_elem->m_Next != nullptr )
    {
      list_elem = list_elem->m_Next;
      ASSERT_MSG( list_elem->m_Key != key, "Duplicated key inserted!" );
    }
    list_elem->m_Next = new_elem;
  }
}

//---------------------------------------------------------------------------------
template< typename K, typename V>
V* HashMap<K, V>::At( const K& key )
{
  if ( m_Backing == nullptr )
  {
    return nullptr;
  }

  uint32_t i_bucket = HashFunction( key );
  Element* list_elem = m_BucketsBase[ i_bucket ].elements;
  while ( list_elem != nullptr )
  {
    if ( list_elem->m_Key == key )
    {
      return &list_elem->m_Value;
    }

    list_elem = list_elem->m_Next;
  }

  return nullptr;
}

//---------------------------------------------------------------------------------
template< typename K, typename V >
void HashMap<K, V>::Remove(const K& key)
{
  ASSERT_MSG(m_Backing != nullptr, "HashMap not initialized!");

  uint32_t i_bucket = HashFunction( key );
  Element* list_elem = m_BucketsBase[ i_bucket ].elements;

  if ( list_elem->m_Key == key )
  {
    if ( list_elem->m_Next != nullptr )
    {
      m_BucketsBase[ i_bucket ].elements = list_elem->m_Next;
    }

    list_elem->m_Next = m_FreeElementHead;
    m_FreeElementHead = list_elem;
    return;
  }
  
  Element* prev_elem = list_elem;
  list_elem          = list_elem->m_Next;
  while ( list_elem != nullptr && list_elem->m_Key != key )
  {
    if ( list_elem->m_Key == key )
    {
      prev_elem->m_Next = list_elem->m_Next;
      list_elem->m_Next = m_FreeElementHead;
      m_FreeElementHead = list_elem;
      return;
    }

    prev_elem = list_elem;
    list_elem = list_elem->m_Next;
  }

  ASSERT_ALWAYS( "Attempted to remove element not in HashMap" );
}

//---------------------------------------------------------------------------------
template< typename K, typename V >
uint32_t HashMap< K, V >::HashFunction( K input ) const
{
  return input % m_NumBuckets;
}