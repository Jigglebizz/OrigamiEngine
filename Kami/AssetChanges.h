#pragma once

#include "BuilderCommon/BuilderCommon.h"
#include "Origami/Asset/AssetVersions.h"


namespace AssetChanges
{
  //---------------------------------------------------------------------------------
  struct AssetChangeInfo
  {
    AssetId  m_AssetId;
    uint32_t m_ExtensionHash;
    uint32_t m_BuiltVersion;
    char     m_Name        [ Filesystem::kMaxAssetNameLen            ];

    uint32_t m_DependencyCount;
    AssetId  m_Dependencies[ BuilderCommon::kMaxAssetDependencyCount ];

    uint32_t m_DependentsCount;
    AssetId  m_Dependents  [ BuilderCommon::kMaxAssetDependencyCount ];
  };

  void     Init();
  void     Destroy();
  void     AddAssetChangeInfo( const AssetChangeInfo* info );
  uint32_t GetChangeCount();
}