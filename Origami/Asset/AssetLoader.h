#pragma once

#include "Origami/Filesystem/Filesystem.h"

namespace AssetLoader
{
  static constexpr uint32_t kAssetHeapSize = 512 * 1024 * 1024; // 1GB of loaded assets

  static char         m_AssetHeapBacking[ kAssetHeapSize ];
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