#include "Origami/pch.h"

#include <rapidjson.h>
#include <fileapi.h>

#include "Origami/Asset/AssetLoader.h"

int main( int argc, char* argv[] )
{
  UNREFERENCED_PARAMETER( argc );
  UNREFERENCED_PARAMETER( argv );

  const char* source_path = AssetLoader::GetAssetsSourcePath();
  const char* built_path  = AssetLoader::GetAssetsBuiltPath();

  UNREFERENCED_PARAMETER( source_path );
  UNREFERENCED_PARAMETER( built_path );

  // scan for changes

  // create change notification handle

  while ( 1 )
  {
    
  }
}