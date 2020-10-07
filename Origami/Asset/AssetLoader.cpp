#include "Origami/pch.h"
#include "AssetLoader.h"

#include <fstream>
#include <direct.h>

#include "Origami/Util/StringUtil.h"
#include "Origami/Util/Log.h"

constexpr uint32_t kMaxPathLen = 128;

DISABLE_OPTS

//---------------------------------------------------------------------------------
char* AssetLoader::Load( const char* asset_name )
{
  char full_path[ kMaxPathLen ];
  snprintf( full_path, kMaxPathLen, "%s%s%s", AssetLoader::GetAssetsSourcePath(), "\\", asset_name );

  std::ifstream file( full_path, std::ios::binary | std::ios::in | std::ios::ate );
  if ( file.good() == false )
  {
    Log::LogError("Could not open file %s\n", full_path );
    return nullptr;
  }

  uint64_t asset_size = file.tellg();
  file.seekg( 0, std::ios::beg );

  BasicAsset* asset = (BasicAsset*)malloc( sizeof( BasicAsset ) + asset_size );

  if ( asset == nullptr )
  {
    Log::LogError( "No memory for asset %s\n", asset_name );
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
  free( asset );
}

//---------------------------------------------------------------------------------
const char* GetAssetsBasePath()
{
#ifdef _WIN32
  const char PATH_SEP = '\\';
#else
  const char PATH_SEP = '/';
#endif

  static char base_path[ kMaxPathLen ];

  if ( *base_path == 0 )
  {
    char* cwd_path = _getcwd( NULL, kMaxPathLen );
    //snprintf(base_path, kMaxPathLen, "%s", .c_str() );

    const char* project_dir_name = "Origami";
    char* project_folder_start   = strstr( cwd_path, project_dir_name );

    if ( project_folder_start == nullptr )
    {
      Log::LogError( "Broken folder structure!" );
      return "";
    }

    project_folder_start[ StrLen( project_dir_name ) + 1 ] = 0;

    // strcpy_s( base_path, base_path );

    snprintf( base_path, kMaxPathLen, "%s%s", cwd_path, "Assets" );
  }
  return base_path;
}

//---------------------------------------------------------------------------------
const char* AssetLoader::GetAssetsSourcePath()
{
  static char source_path[ kMaxPathLen ];
  if ( *source_path == 0)
  {
    snprintf( source_path, kMaxPathLen, "%s%s", GetAssetsBasePath(), "\\source" );
  }
  return source_path;
}

//---------------------------------------------------------------------------------
const char* AssetLoader::GetAssetsBuiltPath()
{
  static char built_path[ kMaxPathLen ];
  if ( *built_path == 0)
  {
    snprintf( built_path, kMaxPathLen, "%s%s", GetAssetsBasePath(), "\\built");
  }
  return built_path;
}