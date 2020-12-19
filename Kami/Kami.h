#pragma once

#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Asset/AssetVersions.h"

namespace Kami
{
  static constexpr uint8_t  kMaxExtensionLen = 16;

  //---------------------------------------------------------------------------------
  struct BuilderInfo
  {
    uint32_t      m_ExtensionHash;
    uint32_t      m_VersionHash;
    char          m_VersionDesc[ AssetVersion::kMaxDescriptionLen ];
    char          m_Path       [ Filesystem::kMaxPathLen ];
    char          m_Extension  [ kMaxExtensionLen ];
  };

  //---------------------------------------------------------------------------------
  int  Init();
  void Destroy();
  void Execute();

  const BuilderInfo* GetBuilderInfos( uint32_t* count );
}