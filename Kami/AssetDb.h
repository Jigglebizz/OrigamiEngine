#pragma once

#include "Origami/Asset/Asset.h"
#include "Origami/Filesystem/Filesystem.h"

//---------------------------------------------------------------------------------
struct AssetDbEntry
{
  AssetId  m_AssetId;
  uint32_t m_VersionHash;
};

//---------------------------------------------------------------------------------
class AssetDb
{
  uint64_t      m_EntriesCount;
  uint64_t      m_EntriesCapacity;
  AssetDbEntry* m_Entries;
  char          m_FilePath[ Filesystem::kMaxPathLen ];

public:
  enum LoadStatus : uint8_t
  {
    kLoadStatusOk           = 0x00,
    kLoadStatusDoesNotExist = 0x01,
    kLoadStatusFileProblem  = 0x02
  };

  void        Init          ();
  void        Destroy       ();
              
  void        UpdateEntries ( AssetId* ids, uint32_t* versions, uint64_t len );
  uint32_t    GetVersionFor ( AssetId id ) const;
              
  LoadStatus  LoadFromDisk  ( );
  void        SaveToDisk    ( );

  const char* GetFilePath   ( ) const;
};