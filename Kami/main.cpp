#include "Origami/pch.h"

#include <fileapi.h>
#include <string> // for stoul

#include "Origami/Concurrency/Thread.h"
#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Util/Sort.h"
#include "Origami/Util/Search.h"
#include "Origami/Game/GlobalSettings.h"
#include "Origami/Util/Log.h"

#include "BuilderCommon/BuilderCommon.h"

#include "Kami/AssetDb.h"


//---------------------------------------------------------------------------------
static constexpr uint32_t kMaxBuildersCount        = 32;
static constexpr uint8_t  kMaxExtensionLen         = 16;
static constexpr uint32_t kMaxAssetCount           = 256 * 1024;

static constexpr uint32_t kBuiltVersionNew    = 0;

//---------------------------------------------------------------------------------
char g_SourcePath      [ Filesystem::kMaxPathLen ];
char g_BuildersDirPath [ Filesystem::kMaxPathLen ];

//---------------------------------------------------------------------------------
AssetDb g_AssetDb;
Thread  g_AssetDbPersistenceThread;

//---------------------------------------------------------------------------------
struct BuilderInfo
{
  uint32_t m_ExtensionHash;
  uint32_t m_Version;
  char     m_Path      [ Filesystem::kMaxPathLen ];
  char     m_Extension [ kMaxExtensionLen ];
};

//---------------------------------------------------------------------------------
uint32_t    g_BuilderCount = 0;
BuilderInfo g_BuilderInfos[ kMaxBuildersCount ];

//---------------------------------------------------------------------------------
void LoadBuilderInfos()
{
  char builders_glob[ Filesystem::kMaxPathLen ];
  snprintf( builders_glob, sizeof( builders_glob ), "%s\\*.exe", g_BuildersDirPath );

  WIN32_FIND_DATA find_file_data;
  HANDLE hFind = FindFirstFile( builders_glob, &find_file_data );
  if ( hFind == INVALID_HANDLE_VALUE )
  {
    printf( "FindFirstFile failed %d. Are there any builders that match %s?\n", GetLastError(), builders_glob );
    return;
  }

  char builder_info_output  [ 128 ];
  char builder_info_command [ Filesystem::kMaxPathLen ];
  do
  {
    MemZero( builder_info_output, sizeof( builder_info_output ) );
    BuilderInfo* info = &g_BuilderInfos[ g_BuilderCount++ ];

    snprintf( info->m_Path,         sizeof( info->m_Path ),         "%s\\%s",      g_BuildersDirPath, find_file_data.cFileName );
    snprintf( builder_info_command, sizeof( builder_info_command ), "%s -version", info->m_Path );
    Filesystem::RunCommand( builder_info_command, builder_info_output, sizeof( builder_info_output ) );

    info->m_Version = std::stoul( builder_info_output, nullptr, 0x10 );

    snprintf( builder_info_command, sizeof( builder_info_command ), "%s -extension", info->m_Path );
    Filesystem::RunCommand( builder_info_command, info->m_Extension, sizeof( info->m_Extension ) );

    RemoveTrailingWhitespace( info->m_Extension, StrLen( info->m_Extension ) );

    info->m_ExtensionHash = Crc32( info->m_Extension );

  } while ( FindNextFile( hFind, &find_file_data ) != 0 );

  QuickSort32( &g_BuilderInfos, sizeof( BuilderInfo ), g_BuilderCount );
  
  for ( uint32_t i_sep = 0; i_sep < 44; ++i_sep )
  {
    putchar('=');
  }
  putchar( '\n' );
  printf( "  Builder Info\n" );
  for (uint32_t i_sep = 0; i_sep < 44; ++i_sep)
  {
    putchar('=');
  }
  putchar('\n');
  for ( uint32_t i_builder = 0; i_builder < g_BuilderCount; ++i_builder )
  {
    BuilderInfo* info = &g_BuilderInfos[ i_builder ];
    printf("ext: .%-17s version: %#08x\n", info->m_Extension, info->m_Version );
  }
  putchar( '\n' );
}

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

AssetChangeInfo* g_AssetChanges;
uint32_t         g_AssetChangesCapacity;
uint32_t         g_AssetChangesCount;

uint8_t          g_AssetChangesBitsetBacking[ kMaxAssetCount / 8 ];
Bitset           g_AssetChangesBitset;

void InitAssetChangesList()
{
  g_AssetChangesCapacity     = 1024;
  g_AssetChangesCount        = 0;
  g_AssetChanges             = (AssetChangeInfo*)g_DynamicHeap.Alloc( g_AssetChangesCapacity * sizeof( AssetChangeInfo ) );

  g_AssetChangesBitset.InitWithBacking( g_AssetChangesBitsetBacking, kMaxAssetCount );
}

//---------------------------------------------------------------------------------
void AddAssetChangeInfo( const AssetChangeInfo* info )
{
  uint32_t new_idx = g_AssetChangesBitset.FirstUnsetBit();

  ASSERT_MSG( new_idx != -1, "Reached maximum asset count" );

  if ( new_idx > g_AssetChangesCapacity )
  {
    g_AssetChangesCapacity += 1024;
    g_AssetChanges = ( AssetChangeInfo* )g_DynamicHeap.Realloc( g_AssetChanges, g_AssetChangesCapacity * sizeof( AssetChangeInfo ) );
  }

  char full_asset_path[ Filesystem::kMaxPathLen ];
  snprintf( full_asset_path, sizeof( full_asset_path ), "%s%s", Filesystem::GetAssetsSourcePath(), info->m_Name );

  AssetChangeInfo* new_change = &g_AssetChanges[new_idx];
  memcpy_s( new_change, sizeof ( *g_AssetChanges ), info, sizeof( *info ) );

  BuilderCommon::AssetCommonData asset_data;
  BuilderCommon::ParseAsset( full_asset_path, &asset_data );

  new_change->m_DependentsCount = asset_data.m_BuildDependentsCount;
  memcpy_s( new_change->m_Dependents,   sizeof( new_change->m_Dependents ),   asset_data.m_BuildDependents,   sizeof( asset_data.m_BuildDependents )   );

  new_change->m_DependencyCount = asset_data.m_LoadDependenciesCount;
  memcpy_s( new_change->m_Dependencies, sizeof( new_change->m_Dependencies ), asset_data.m_LoadDependencies, sizeof( asset_data.m_LoadDependencies ) );

  g_AssetChangesCount++;
}

//---------------------------------------------------------------------------------
void ScanFilesystemForChangedAssets()
{
  uint32_t* asset_extensions = (uint32_t*)g_DynamicHeap.Alloc( sizeof(uint32_t) * g_BuilderCount );

  Filesystem::DoForEachFileInDirectory( Filesystem::GetAssetsSourcePath(), [ asset_extensions ]( const Filesystem::FileCallbackParams* file_params ) {
    
    AssetId asset_id  = AssetId::FromAssetPath( file_params->m_RelativePath );
    uint32_t ext_hash = Crc32( file_params->m_Extension );

    size_t newest_version_idx = BinarySearch32( ext_hash, g_BuilderInfos, sizeof( g_BuilderInfos[0] ), g_BuilderCount );
    if ( newest_version_idx == -1 )
    {
      // No builder for extension
      return;
    }

    if ( g_AssetDb.Contains( asset_id ) == false )
    {
      AssetChangeInfo new_info;
      new_info.m_AssetId       = asset_id;
      new_info.m_BuiltVersion  = kBuiltVersionNew;
      new_info.m_ExtensionHash = ext_hash;
      strcpy_s( new_info.m_Name, file_params->m_RelativePath );

      AddAssetChangeInfo( &new_info );
    }
    else
    {
      // check if asset is out of date
      BuilderInfo* builder         = &g_BuilderInfos[ newest_version_idx ];
      uint32_t     current_version = g_AssetDb.GetVersionFor( asset_id );

      if ( current_version != builder->m_Version )
      {
        AssetChangeInfo new_info;
        new_info.m_AssetId       = asset_id;
        new_info.m_BuiltVersion  = current_version;
        new_info.m_ExtensionHash = ext_hash;
        strcpy_s( new_info.m_Name, file_params->m_RelativePath );

        AddAssetChangeInfo( &new_info );
      }
    }
  }, true);
}

const char* change_names[] =
{
  "Unknown",
  "Added",
  "Removed",
  "Modified",
  "Renamed Old",
  "Renamed New"
};

//---------------------------------------------------------------------------------
void FileChangedCallback( const char* filename, Filesystem::WatchDirectoryChangeType change_type )
{
  printf( "file %s: %s\n", change_names[change_type], filename );
}

//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  UNREFFED_PARAMETER( argc );
  UNREFFED_PARAMETER( argv );

  if ( Filesystem::FileExists( Filesystem::GetAssetsBuiltPath()) == false )
  {
    Filesystem::CreateDir( Filesystem::GetAssetsBuiltPath() );
  }

  g_GlobalSettings.Init( GlobalSettings::kProjectTypeKami );

  snprintf( g_BuildersDirPath, sizeof( g_BuildersDirPath ), "%s\\%s\\%s\\Builders", Filesystem::GetOutputPath(), BUILD_PLATFORM, BUILD_CONFIG );
  LoadBuilderInfos();
  g_AssetDb.Init();
  AssetDb::LoadStatus db_status = g_AssetDb.LoadFromDisk();

  if ( db_status != AssetDb::kLoadStatusOk && db_status != AssetDb::kLoadStatusDoesNotExist )
  {
    printf( "Problem with Asset DB. Could not run Kami!\n" );
    return 1;
  }

  if ( db_status  == AssetDb::kLoadStatusDoesNotExist )
  {
    printf( "Asset db does not exist. Creating at %s\n", g_AssetDb.GetFilePath() );
    g_AssetDb.SaveToDisk();
  }

  InitAssetChangesList();

  // scan filesystem for file changes and new files
  printf( "Asset DB Loaded. Scanning for changes since last boot\n" );
  ScanFilesystemForChangedAssets();
  printf( "Found %lu assets that need to be built\n", g_AssetChangesCount );
  
  // create change notification handle
  Thread fs_watch_thread;
  Filesystem::WatchDirectoryForChanges( Filesystem::GetAssetsSourcePath(), &fs_watch_thread, &FileChangedCallback );

  // start main loop
  {
    // spawn loop
  }


  // char builder_command[ Filesystem::kMaxPathLen ];
  // snprintf( builder_command, sizeof( builder_command ), "%s\\ActorBuilder.exe -source actor/TestActor.actor", g_BuildersDirPath );

  //g_AssetDbPersistenceThread.RequestStop();
  //while ( g_AssetDbPersistenceThread.Joinable() == false )
  //{
  //  g_AssetDbPersistenceThread.Join();
  //}

  Sleep( 200'000 );

  BuilderCommon::Destroy();
  g_GlobalSettings.Destroy();
  return 0;
}