#include "Origami/pch.h"
#include "Origami/Asset/AssetLoader.h"

#include <fstream>

#include "Origami/Util/StringUtil.h"
#include "Origami/Util/Log.h"


//---------------------------------------------------------------------------------
void AssetLoader::Init()
{
  m_AssetHeap.InitWithBacking( m_AssetHeapBacking, sizeof( m_AssetHeapBacking ), "Asset Heap" );
}

//---------------------------------------------------------------------------------
void AssetLoader::Destroy()
{
  m_AssetHeap.Destroy();
}

//---------------------------------------------------------------------------------
char* AssetLoader::Load( const char* asset_name )
{
  char full_path[ Filesystem::kMaxPathLen ];
  snprintf( full_path, Filesystem::kMaxPathLen, "%s%s%s", Filesystem::GetAssetsSourcePath(), "\\", asset_name );

  std::ifstream file( full_path, std::ios::binary | std::ios::in | std::ios::ate );
  if ( file.good() == false )
  {
    Log::LogError("Could not open file %s\n", full_path );
    return nullptr;
  }

  uint64_t asset_size = file.tellg();
  file.seekg( 0, std::ios::beg );

  BasicAsset* asset = (BasicAsset*)m_AssetHeap.Alloc( sizeof( BasicAsset ) + asset_size );

  if ( asset == nullptr )
  {
    Log::LogError( "No memory for asset %s\n", asset_name );
    return nullptr;
  }

  strcpy_s( asset->m_Name, asset_name );
  asset->m_DataLength = asset_size;
  asset->m_Data       = (char*)asset + sizeof( *asset );
  file.read( asset->m_Data, asset->m_DataLength);

  return (char*)asset;
}

//---------------------------------------------------------------------------------
void AssetLoader::Free( void* asset )
{
  m_AssetHeap.Free( asset );
}