#pragma once

namespace AssetLoader
{
  struct BasicAsset
  {
    char     m_Name[ 64 ];
    uint64_t m_DataLength;
    char*    m_Data;
  };
  
  //---------------------------------------------------------------------------------
  const char* GetAssetPath();
 
  //---------------------------------------------------------------------------------
  char* Load( const char* asset_path );
  void  Free( void* asset );
}