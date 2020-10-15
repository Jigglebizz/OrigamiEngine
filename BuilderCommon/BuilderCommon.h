#pragma once

#include "Origami/Asset/Asset.h"
#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Asset/AssetVersions.h"

//---------------------------------------------------------------------------------
template< class Builder>
int RunBuilder( int argc, char* argv[] )
{
  char source_asset_path[ Filesystem::kMaxPathLen ];
  MemZero( source_asset_path, sizeof( source_asset_path ) );

  Builder builder;

  int i_arg = 0;
  while ( i_arg < argc )
  {
    char* current_arg = argv[ i_arg ];
    if ( strcmp( current_arg , "-version" ) == 0 || strcmp( current_arg, "-v" ) == 0 )
    {
      printf( "%#08x\n", builder.GetAssetVersion() );
      return 0;
    }

    if ( strcmp( current_arg, "-extension") == 0 )
    {
      printf( "%s\n", builder.GetAssetExtension() );
      return 0;
    }

    if ( strcmp( current_arg, "-source" ) == 0 )
    {
      strcpy_s( source_asset_path, argv[ ++i_arg ] );
    }
    i_arg++;
  }

  bool stat = source_asset_path[0] != '\0';
  if ( stat == false )
  {
    printf( "source path not provided!\n" );
    return 1;
  }

  MemZero ( &builder, sizeof( Builder ) );
  strcpy_s( builder.m_AssetName, source_asset_path );
  builder.m_AssetId = AssetId::FromAssetPath( builder.m_AssetName );

  MemZero ( builder.m_AssetSourcePath, sizeof( builder.m_AssetSourcePath ) );
  snprintf( builder.m_AssetSourcePath, sizeof( builder.m_AssetSourcePath ), "%s\\%s", Filesystem::GetAssetsSourcePath(), builder.m_AssetName );

  MemZero ( builder.m_AssetBuiltPath, sizeof( builder.m_AssetBuiltPath ) );
  snprintf( builder.m_AssetBuiltPath, sizeof( builder.m_AssetBuiltPath ), "%s\\%#08x.built", Filesystem::GetAssetsBuiltPath(), builder.m_AssetId.ToU32() );

  if ( Filesystem::FileExists( builder.m_AssetSourcePath ) == false )
  {
    printf( "Source asset does not exist\n" );
    return 1;
  }

  printf( "Writing built file to: %s\n", builder.m_AssetBuiltPath );


  return builder.Build();
}

//---------------------------------------------------------------------------------
class BuilderBase
{
public:
  //---------------------------------------------------------------------------------
  AssetId m_AssetId;
  char    m_AssetName      [ Filesystem::kMaxAssetNameLen ];
  char    m_AssetSourcePath[ Filesystem::kMaxPathLen      ];
  char    m_AssetBuiltPath [ Filesystem::kMaxPathLen      ];

  //---------------------------------------------------------------------------------
  virtual int          Build()                   = 0;
  virtual AssetVersion GetAssetVersion()   const = 0;
  virtual const char*  GetAssetExtension() const = 0;
};