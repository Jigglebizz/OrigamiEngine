#include "Origami/pch.h"
#include "AssetDb.h"

#include "Origami/Util/Sort.h"
#include "Origami/Util/Search.h"

static const     char*    kAssetDbFilename    = "AssetDb.db";
static constexpr uint16_t kCapacityIncrements = 1024;

//---------------------------------------------------------------------------------
void AssetDb::Init()
{
  m_Mutex.Init( "Asset DB" );
  snprintf( m_FilePath, sizeof( m_FilePath ), "%s\\%s", Filesystem::GetAssetsBuiltPath(), kAssetDbFilename );
  m_EntriesCapacity = kCapacityIncrements;
  m_EntriesCount    = 0;
  m_Entries = (AssetDbEntry*)malloc( m_EntriesCapacity );
}

//---------------------------------------------------------------------------------
void AssetDb::Destroy()
{
  free( m_Entries );
  m_Mutex.Destroy();
}

//---------------------------------------------------------------------------------
const char* AssetDb::GetFilePath() const
{
  return m_FilePath;
}

//---------------------------------------------------------------------------------
AssetDb::LoadStatus AssetDb::LoadFromDisk()
{
  if ( Filesystem::FileExists( m_FilePath ) )
  {
    FILE* db_file;
    errno_t err = fopen_s( &db_file, m_FilePath, "r" );
    if ( err == 0 && db_file != nullptr )
    {
      ScopedLock lock( &m_Mutex );

      fread( &m_EntriesCount, sizeof( m_EntriesCount ), 1, db_file );

      m_EntriesCapacity = ( ( m_EntriesCount / kCapacityIncrements) + 1 ) * kCapacityIncrements;

      free( m_Entries );

      m_Entries = (AssetDbEntry*)malloc( m_EntriesCapacity );
      return kLoadStatusOk;
    }
    printf( "Error! could not open db file: %d", err );
    return kLoadStatusFileProblem;
  }

  return kLoadStatusDoesNotExist;
}

//---------------------------------------------------------------------------------
void AssetDb::SaveToDisk( )
{
  FILE* db_file;
  errno_t err = fopen_s( &db_file, m_FilePath, "w" );
  if ( err == 0 && db_file != nullptr )
  {
    ScopedLock lock( &m_Mutex );
    fwrite( &m_EntriesCount, sizeof( m_EntriesCount ), 1,              db_file );
    fwrite( m_Entries,       sizeof( AssetDbEntry   ), m_EntriesCount, db_file );
    fclose( db_file );
  }
  else
  {
    printf( "Error saving asset db to disk! %d", err );
  }
}

//---------------------------------------------------------------------------------
void AssetDb::UpdateEntries( AssetId* ids, uint32_t* versions, uint64_t len )
{
  ScopedLock lock( &m_Mutex );

  uint64_t current_count = m_EntriesCount;
  bool     need_sort     = false;

  for ( uint64_t i_entry = 0; i_entry < len; ++i_entry )
  {
    size_t found_idx = BinarySearch32( ids[ i_entry ].ToU32(), m_Entries, sizeof( AssetDbEntry ), current_count );
    if ( found_idx != (size_t)-1 )
    {
      m_Entries[ found_idx ].m_VersionHash = versions[ i_entry ];
    }
    else
    {
      if ( m_EntriesCount == m_EntriesCapacity )
      {
        AssetDbEntry* new_entries = (AssetDbEntry*)realloc( m_Entries, m_EntriesCapacity + kCapacityIncrements );
        if ( new_entries == nullptr )
        {
          printf("Error! could not realloc asset db!\n");
          break;
        }
        
        m_Entries = new_entries;
        m_EntriesCapacity += kCapacityIncrements;
      }

      AssetDbEntry* new_entry = &m_Entries[ m_EntriesCount++ ];
      new_entry->m_AssetId    = ids       [ i_entry ];
      new_entry->m_VersionHash = versions [ i_entry ];

      need_sort = true;
    }
  }

  if ( need_sort )
  {
    QuickSort32( &m_Entries, sizeof( AssetDbEntry), m_EntriesCount );
  }
}

//---------------------------------------------------------------------------------
uint32_t AssetDb::GetVersionFor( AssetId id ) const
{
  ScopedLock lock( &m_Mutex );
  size_t found_idx = BinarySearch32( id.ToU32(), m_Entries, sizeof( AssetDbEntry ), m_EntriesCount );
  if ( found_idx != (size_t)-1 )
  {
    return m_Entries[ found_idx ].m_VersionHash;
  }
  return (uint32_t)-1;
}