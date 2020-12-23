#include "Origami/pch.h"
#include "Kami/AssetChanges.h"

#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Concurrency/Mutex.h"
#include "Origami/Game/GlobalSettings.h"

DISABLE_OPTS

//---------------------------------------------------------------------------------
AssetChanges::AssetChangeInfo* s_Unrequested;
uint32_t                       s_UnrequestedCapacity;
uint32_t                       s_UnrequestedCount;
Mutex                          s_UnrequestedMutex;
Bitset                         s_UnrequestedBitset;

AssetChanges::AssetChangeInfo* s_Requested;
uint32_t                       s_RequestedCapacity;
uint32_t                       s_RequestedCount;
Mutex                          s_RequestedMutex;
Bitset                         s_RequestedBitset;

//---------------------------------------------------------------------------------
void AssetChanges::Init()
{
  s_UnrequestedCapacity = g_GlobalSettings.GetU32( Crc32( "Asset Capacity" ) );

  s_UnrequestedCount    = 0;
  s_Unrequested         = (AssetChangeInfo*)g_DynamicHeap.Alloc( s_UnrequestedCapacity * sizeof( AssetChangeInfo ) );
  s_UnrequestedBitset.InitFromDynamicHeap( s_UnrequestedCapacity );
  s_UnrequestedMutex.Init( "Kami Asset Changes" );

  s_RequestedCapacity = g_GlobalSettings.GetU32( Crc32( "Asset Capacity" ) );
  s_RequestedCount    = 0;
  s_Requested         = (AssetChangeInfo*)g_DynamicHeap.Alloc( s_RequestedCapacity * sizeof( AssetChangeInfo ) );
}

//---------------------------------------------------------------------------------
void AssetChanges::Destroy()
{
  s_UnrequestedMutex.Destroy();
  g_DynamicHeap.Free( s_Unrequested );
}

//---------------------------------------------------------------------------------
void AssetChanges::AddAssetChangeInfo( const AssetChangeInfo* info )
{
  uint32_t new_idx = s_UnrequestedBitset.FirstUnsetBit();

  ASSERT_MSG( new_idx != -1, "Reached maximum asset count" );

  ScopedLock asset_change_lock( &s_UnrequestedMutex );

  if ( new_idx > s_UnrequestedCapacity )
  {
    s_UnrequestedCapacity += 1024;
    s_Unrequested = ( AssetChangeInfo* )g_DynamicHeap.Realloc( s_Unrequested, s_UnrequestedCapacity * sizeof( AssetChangeInfo ) );
  }

  char full_asset_path[ Filesystem::kMaxPathLen ];
  snprintf( full_asset_path, sizeof( full_asset_path ), "%s%s", Filesystem::GetAssetsSourcePath(), info->m_Name );

  AssetChangeInfo* new_change = &s_Unrequested[ new_idx ];
  memcpy_s( new_change, sizeof ( *s_Unrequested ), info, sizeof( *info ) );

  BuilderCommon::AssetCommonData asset_data;
  BuilderCommon::ParseAsset( full_asset_path, &asset_data );

  //new_change->m_DependentsCount = asset_data.m_BuildDependentsCount;
  //memcpy_s( new_change->m_Dependents, sizeof( new_change->m_Dependents ), asset_data.m_BuildDependents,   sizeof( asset_data.m_BuildDependents )   );

  //new_change->m_DependencyCount = asset_data.m_LoadDependenciesCount;
  //memcpy_s( new_change->m_Dependencies, sizeof( new_change->m_Dependencies ), asset_data.m_LoadDependencies, sizeof( asset_data.m_LoadDependencies ) );

  s_UnrequestedCount++;
}

//---------------------------------------------------------------------------------
uint32_t AssetChanges::GetChangeCount()
{
  return s_UnrequestedCount;
}

//---------------------------------------------------------------------------------
const AssetChanges::AssetChangeInfo* AssetChanges::GetInfoForAssetId( AssetId id )
{
  uint32_t current_bit = s_UnrequestedBitset.GetNextSetBit(  );
  while ( current_bit != -1 )
  {
    AssetChangeInfo* info = &s_Unrequested[ current_bit ];
    if ( info->m_AssetId == id )
    {
      return info;
    }
    current_bit = s_UnrequestedBitset.GetNextSetBit( current_bit );
  }

  return nullptr;
}

//---------------------------------------------------------------------------------
const AssetChanges::AssetChangeInfo* AssetChanges::GetNextInfo( )
{
  uint32_t next_bit = s_UnrequestedBitset.GetNextSetBit();
  if ( next_bit != -1 )
  {
    return &s_Unrequested[ next_bit ];
  }

  return nullptr;
}

//---------------------------------------------------------------------------------
void AssetChanges::RemoveInfo( const AssetChangeInfo* info )
{
  uint32_t change_idx = (uint32_t)INDEX_OF( s_Unrequested, info );
  ASSERT_MSG( change_idx < s_UnrequestedCount, "Info is not in asset change list" );

  s_UnrequestedBitset.Unset( change_idx );
  s_UnrequestedCount--;
}