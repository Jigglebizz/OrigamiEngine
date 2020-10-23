#pragma once

#include "Origami/Asset/Asset.h"
#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Concurrency/Mutex.h"

//---------------------------------------------------------------------------------
struct AssetDbEntry
{
  AssetId  m_AssetId;
  uint32_t m_VersionHash;
};

//---------------------------------------------------------------------------------
class AssetDb
{
  uint32_t      m_EntriesCount;
  uint32_t      m_EntriesCapacity;
  AssetDbEntry* m_Entries;
  char          m_FilePath[ Filesystem::kMaxPathLen ];
  mutable Mutex m_Mutex;

public:
  enum LoadStatus : uint8_t
  {
    kLoadStatusOk           = 0x00,
    kLoadStatusDoesNotExist = 0x01,
    kLoadStatusFileProblem  = 0x02
  };

         void        Init          ();
         void        Destroy       ();
                     
         void        UpdateEntries ( AssetId* ids, uint32_t* versions, uint32_t len );
         uint32_t    GetVersionFor ( AssetId id ) const;
  inline uint32_t    GetNumEntries ()             const;
  inline uint32_t    GetCapacity   ()             const;
              
         LoadStatus  LoadFromDisk  ( );
         void        SaveToDisk    ( );
         
         const char* GetFilePath   ( ) const;
         void        GetOutOfDateAssetIds ( const uint32_t* current_versions, uint32_t len_current_versions, AssetId* ids, uint32_t* num_ids );
};


//---------------------------------------------------------------------------------
uint32_t AssetDb::GetNumEntries() const
{
  return m_EntriesCount;
}


//---------------------------------------------------------------------------------
uint32_t AssetDb::GetCapacity() const
{
  return m_EntriesCapacity;
}