#include "Origami/pch.h"
#include "Kami/AssetChanges.h"

#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Concurrency/Mutex.h"

static constexpr uint32_t kMaxAssetCount = 256 * 1024;

//---------------------------------------------------------------------------------
AssetChanges::AssetChangeInfo* s_AssetChanges;
uint32_t                       s_AssetChangesCapacity;
uint32_t                       s_AssetChangesCount;
Mutex                          s_AssetChangesMutex;
uint8_t                        s_AssetChangesBitsetBacking[ kMaxAssetCount / 8 ];
Bitset                         s_AssetChangesBitset;

//---------------------------------------------------------------------------------
void AssetChanges::Init()
{
  s_AssetChangesCapacity = 1024;
  s_AssetChangesCount = 0;
  s_AssetChanges = (AssetChangeInfo*)g_DynamicHeap.Alloc( s_AssetChangesCapacity * sizeof( AssetChangeInfo ) );

  s_AssetChangesBitset.InitWithBacking( s_AssetChangesBitsetBacking, kMaxAssetCount );
  s_AssetChangesMutex.Init( "Kami Asset Changes" );
}

//---------------------------------------------------------------------------------
void AssetChanges::Destroy()
{
  s_AssetChangesMutex.Destroy();
  g_DynamicHeap.Free( s_AssetChanges );
}

//---------------------------------------------------------------------------------
void AssetChanges::AddAssetChangeInfo( const AssetChangeInfo* info )
{
  uint32_t new_idx = s_AssetChangesBitset.FirstUnsetBit();

  ASSERT_MSG( new_idx != -1, "Reached maximum asset count" );

  ScopedLock asset_change_lock( &s_AssetChangesMutex );

  if ( new_idx > s_AssetChangesCapacity )
  {
    s_AssetChangesCapacity += 1024;
    s_AssetChanges = ( AssetChangeInfo* )g_DynamicHeap.Realloc( s_AssetChanges, s_AssetChangesCapacity * sizeof( AssetChangeInfo ) );
  }

  char full_asset_path[ Filesystem::kMaxPathLen ];
  snprintf( full_asset_path, sizeof( full_asset_path ), "%s%s", Filesystem::GetAssetsSourcePath(), info->m_Name );

  AssetChangeInfo* new_change = &s_AssetChanges[ new_idx ];
  memcpy_s( new_change, sizeof ( *s_AssetChanges ), info, sizeof( *info ) );

  BuilderCommon::AssetCommonData asset_data;
  BuilderCommon::ParseAsset( full_asset_path, &asset_data );

  new_change->m_DependentsCount = asset_data.m_BuildDependentsCount;
  memcpy_s( new_change->m_Dependents, sizeof( new_change->m_Dependents ), asset_data.m_BuildDependents,   sizeof( asset_data.m_BuildDependents )   );

  new_change->m_DependencyCount = asset_data.m_LoadDependenciesCount;
  memcpy_s( new_change->m_Dependencies, sizeof( new_change->m_Dependencies ), asset_data.m_LoadDependencies, sizeof( asset_data.m_LoadDependencies ) );

  s_AssetChangesCount++;
}

//---------------------------------------------------------------------------------
uint32_t AssetChanges::GetChangeCount()
{
  return s_AssetChangesCount;
}

//---------------------------------------------------------------------------------
const AssetChanges::AssetChangeInfo* AssetChanges::GetInfoForAssetId( AssetId id )
{
  uint32_t current_bit = s_AssetChangesBitset.GetNextSetBit(  );
  while ( current_bit != -1 )
  {
    AssetChangeInfo* info = &s_AssetChanges[ current_bit ];
    if ( info->m_AssetId == id )
    {
      return info;
    }
    current_bit = s_AssetChangesBitset.GetNextSetBit( current_bit );
  }

  return nullptr;
}

//---------------------------------------------------------------------------------
const AssetChanges::AssetChangeInfo* AssetChanges::GetNextInfo( )
{
  uint32_t next_bit = s_AssetChangesBitset.GetNextSetBit();
  if ( next_bit != -1 )
  {
    return &s_AssetChanges[ next_bit ];
  }

  return nullptr;
}

//---------------------------------------------------------------------------------
void AssetChanges::RemoveInfo( const AssetChangeInfo* info )
{
  uint32_t change_idx = (uint32_t)INDEX_OF( s_AssetChanges, info );
  ASSERT_MSG( change_idx < s_AssetChangesCount, "Info is not in asset change list" );

  s_AssetChangesBitset.Unset( change_idx );
  s_AssetChangesCount--;
}