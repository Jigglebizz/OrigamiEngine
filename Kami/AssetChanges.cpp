#include "Origami/pch.h"
#include "Kami/AssetChanges.h"

#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Concurrency/Mutex.h"
#include "Origami/Game/GlobalSettings.h"

//---------------------------------------------------------------------------------
HashMap< AssetId, AssetChanges::AssetChangeInfo > s_Unrequested;
Mutex                                             s_UnrequestedMutex;

HashMap< AssetId, AssetChanges::AssetChangeInfo > s_Requested;
Mutex                                             s_RequestedMutex;

//---------------------------------------------------------------------------------
void AssetChanges::Init()
{
  uint32_t asset_capacity = g_GlobalSettings.GetU32( Crc32( "Asset Capacity" ) );
  s_Unrequested.InitOnHeap( &g_DynamicHeap, asset_capacity );
  s_UnrequestedMutex.Init( "Kami Unrequested Asset Changes" );

  s_Requested.InitOnHeap( &g_DynamicHeap, asset_capacity );
  s_RequestedMutex.Init( "Kami Reqested Asset Changes" );
}

//---------------------------------------------------------------------------------
void AssetChanges::Destroy()
{
  s_UnrequestedMutex.Destroy();
  s_Unrequested.Destroy();
  s_RequestedMutex.Destroy();
  s_Requested.Destroy();
}

//---------------------------------------------------------------------------------
void AssetChanges::AddAssetChangeInfo( const AssetChangeInfo* info )
{
  ScopedLock asset_change_lock( &s_UnrequestedMutex );

  ASSERT_MSG( s_Unrequested.IsFull() == false, "Reached asset capacity. Please increase budget in engine.set" );

  char full_asset_path[ Filesystem::kMaxPathLen ];
  snprintf( full_asset_path, sizeof( full_asset_path ), "%s%s", Filesystem::GetAssetsSourcePath(), info->m_Name );

  AssetChangeInfo new_change;
  memcpy_s( &new_change, sizeof ( new_change ), info, sizeof( *info ) );
  s_Unrequested.Insert( new_change.m_AssetId, new_change );

  //BuilderCommon::AssetCommonData asset_data;
  //BuilderCommon::ParseAsset( full_asset_path, &asset_data );

  //new_change->m_DependentsCount = asset_data.m_BuildDependentsCount;
  //memcpy_s( new_change->m_Dependents, sizeof( new_change->m_Dependents ), asset_data.m_BuildDependents,   sizeof( asset_data.m_BuildDependents ) );

  //new_change->m_DependencyCount = asset_data.m_LoadDependenciesCount;
  //memcpy_s( new_change->m_Dependencies, sizeof( new_change->m_Dependencies ), asset_data.m_LoadDependencies, sizeof( asset_data.m_LoadDependencies ) );
}

//---------------------------------------------------------------------------------
uint32_t AssetChanges::GetUnreqestedCount()
{
  ScopedLock asset_change_lock( &s_UnrequestedMutex );
  return s_Unrequested.GetCount();
}

//---------------------------------------------------------------------------------
const AssetChanges::AssetChangeInfo* AssetChanges::GetInfoForAssetId( AssetId id )
{
  ScopedLock asset_change_lock( &s_UnrequestedMutex );

  if ( AssetChanges::AssetChangeInfo* info = s_Unrequested.At( id ) )
  {
    return info;
  }
  return s_Requested.At(id);
}

//---------------------------------------------------------------------------------
const AssetChanges::AssetChangeInfo* AssetChanges::GetNextUnrequestedAsset()
{
  ScopedLock asset_change_lock( &s_UnrequestedMutex );

  ASSERT_MSG( s_Unrequested.IsEmpty() == false, "No Unrequested assets to get" );

  return s_Unrequested.At( *s_Unrequested.GetFirstKey() );
}

//---------------------------------------------------------------------------------
void AssetChanges::RequestAsset( AssetId asset_id )
{
  AssetChangeInfo* info = nullptr;

  {
    ScopedLock unrequested_lock( &s_UnrequestedMutex );

    if ( s_Unrequested.Contains( asset_id ) )
    {
      info = s_Unrequested.At( asset_id );
      s_Unrequested.Remove( asset_id );
    }
  }

  {
    ScopedLock requested_lock( &s_RequestedMutex );

    if ( s_Requested.Contains( asset_id ) == false )
    {
      if ( info != nullptr )
      {
        s_Requested.Insert( asset_id, *info );
      }
      else
      {
        Log::LogError( "Requested unknown Asset ID %lu\n", asset_id.ToU32() );
      }
    }
  }
}