#include "Origami/pch.h"

#include "Origami/Asset/Asset.h"
#include "Origami/Filesystem/Filesystem.h"

int main( int argc, char* argv[] )
{
  char source_asset_path[ Filesystem::kMaxPathLen ];
  MemZero( source_asset_path, sizeof( source_asset_path ) );
  AssetId asset_id = AssetId::kInvalidAssetId;

  int i_arg = 0;
  while ( i_arg < argc )
  {
    char* current_arg = argv[ i_arg ];
    if (strcmp(current_arg, "-source") == 0 )
    {
      snprintf( source_asset_path, sizeof( source_asset_path ), "%s\\%s", Filesystem::GetAssetsSourcePath(), argv[ ++i_arg ] );
      asset_id = AssetId::FromAssetPath( argv[ i_arg ] );
    }
    i_arg++;
  }

  bool stat = source_asset_path[0] != '\0';
  if ( stat == false )
  {
    printf( "source path not provided!\n" );
    return 1;
  }

  stat &= Filesystem::FileExists( source_asset_path );

  if ( stat == false )
  {
    printf( "Source path does not exist!\n" );
    return 1;
  }

  char built_asset_path[Filesystem::kMaxPathLen];
  MemZero( built_asset_path, sizeof( built_asset_path ) );

  snprintf( built_asset_path, sizeof( built_asset_path), "%s\\%#08x.built", Filesystem::GetAssetsBuiltPath(), asset_id.ToU32() );

  printf( "Writing built file to: %s\n", built_asset_path );



  return 0;
}