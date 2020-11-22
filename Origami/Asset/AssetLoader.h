#pragma once

#include "Origami/Filesystem/Filesystem.h"

namespace AssetLoader
{
  static MemAllocHeap m_AssetHeap;

  struct BasicAsset
  {
    char     m_Name[ Filesystem::kMaxAssetNameLen ];
    uint64_t m_DataLength;
    char*    m_Data;
  };
 
  //---------------------------------------------------------------------------------
  void  Init();
  void  Destroy();

  char* Load( const char* asset_path );
  void  Free( void* asset );
}