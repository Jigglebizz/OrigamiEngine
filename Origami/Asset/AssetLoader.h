#pragma once

namespace AssetLoader
{
  constexpr uint32_t kMaxAssetNameLen = 64;
  constexpr uint32_t kMaxPathLen = 128;

  struct BasicAsset
  {
    char     m_Name[ kMaxAssetNameLen ];
    uint64_t m_DataLength;
    char*    m_Data;
  };
  
  //---------------------------------------------------------------------------------
  const char* GetAssetsSourcePath();
  const char* GetAssetsBuiltPath();
 
  //---------------------------------------------------------------------------------
  char* Load( const char* asset_path );
  void  Free( void* asset );
}