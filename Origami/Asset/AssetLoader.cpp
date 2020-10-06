#include "Origami/pch.h"
#include "AssetLoader.h"

#include "SDL.h"
#include <fstream>

#include "Origami/Util/StringUtil.h"
#include "Origami/Util/Log.h"

char* AssetLoader::Load( const char* asset_name )
{
  char full_path[128];
  snprintf( full_path, 128, "%s%s%s", AssetLoader::GetAssetPath(), "\\", asset_name );

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

void  AssetLoader::Free( void* asset )
{
  free( asset );
}

const char* AssetLoader::GetAssetPath()
{
#ifdef _WIN32
  const char PATH_SEP = '\\';
#else
  const char PATH_SEP = '/';
#endif

  static char base_res[ 256 ];
  char* base_path = SDL_GetBasePath();
  if ( StrLen( base_path ) != 0)
  {
    const char* project_dir_name = "Origami";
    char* project_folder_start   = strstr( base_path, project_dir_name );

    if ( project_folder_start == nullptr )
    {
      Log::LogError( "Broken folder structure!" );
      return "";
    }

    project_folder_start[ StrLen( project_dir_name) + 1 ] = 0;

    strcpy_s( base_res, base_path );
    SDL_free( base_path );

    snprintf( base_res, 256, "%s%s", base_res, "Assets" );

    return base_res;
  }

  Log::LogError( "Error getting resource path: %s\n", SDL_GetError() );
  return "";

}