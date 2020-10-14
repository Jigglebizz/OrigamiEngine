#pragma once

#include "Origami/Asset/Asset.h"
#include "Origami/Filesystem/Filesystem.h"

//---------------------------------------------------------------------------------
template< class Builder>
int Build( const char* source_asset_path )
{
  Builder builder;
  MemZero ( &builder, sizeof( Builder ) );
  strcpy_s( builder.m_AssetName, source_asset_path );
  builder.m_AssetId = AssetId::FromAssetPath( builder.m_AssetName );

  MemZero ( builder.m_AssetSourcePath, sizeof( builder.m_AssetSourcePath ) );
  snprintf( builder.m_AssetSourcePath, sizeof(builder.m_AssetSourcePath), "%s\\%s", Filesystem::GetAssetsSourcePath(), builder.m_AssetName );

  MemZero ( builder.m_AssetBuiltPath, sizeof( builder.m_AssetBuiltPath ) );
  snprintf( builder.m_AssetBuiltPath, sizeof( builder.m_AssetBuiltPath ), "%s\\%#08x.built", Filesystem::GetAssetsBuiltPath(), builder.m_AssetId.ToU32() );

  if ( Filesystem::FileExists( builder.m_AssetSourcePath ) == false )
  {
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
  virtual int Build() = 0;
};