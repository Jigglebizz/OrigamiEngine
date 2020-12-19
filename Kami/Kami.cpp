#include "Origami/pch.h"
#include "Kami/Kami.h"

#include <fileapi.h>
#include <string> // for stoul

#include "Origami/Concurrency/Thread.h"
#include "Origami/Util/Sort.h"
#include "Origami/Util/Search.h"
#include "Origami/Game/GlobalSettings.h"
#include "Origami/Arch/ArchWin.h"
#include "Origami/Util/Log.h"

#include "Kami/AssetDb.h"
#include "Kami/AssetChanges.h"
#include "Kami/AssetServer.h"


//---------------------------------------------------------------------------------
static constexpr uint32_t kMaxBuildersCount = 32;
static constexpr uint32_t kBuiltVersionNew    = 0;

//---------------------------------------------------------------------------------
uint32_t          g_BuilderCount = 0;
Kami::BuilderInfo g_BuilderInfos[ kMaxBuildersCount ];

uint32_t          g_NumRunningBuilders;
uint32_t          g_NumCores;
                  
char              g_SourcePath     [ Filesystem::kMaxPathLen ];
char              g_BuildersDirPath[ Filesystem::kMaxPathLen ];
                  
AssetDb           g_AssetDb;
Thread            g_FsWatchThread;


//---------------------------------------------------------------------------------
static void ScanFilesystemForChangedAssets()
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
      AssetChanges::AssetChangeInfo new_info;
      new_info.m_AssetId       = asset_id;
      new_info.m_BuiltVersion  = kBuiltVersionNew;
      new_info.m_ExtensionHash = ext_hash;
      strcpy_s( new_info.m_Name, file_params->m_RelativePath );

      AssetChanges::AddAssetChangeInfo( &new_info );
    }
    else
    {
      // check if asset is out of date
      Kami::BuilderInfo* builder         = &g_BuilderInfos[ newest_version_idx ];
      uint32_t           current_version = g_AssetDb.GetVersionFor( asset_id );

      if ( current_version != builder->m_VersionHash )
      {
        AssetChanges::AssetChangeInfo new_info;
        new_info.m_AssetId       = asset_id;
        new_info.m_BuiltVersion  = current_version;
        new_info.m_ExtensionHash = ext_hash;
        strcpy_s( new_info.m_Name, file_params->m_RelativePath );

        AssetChanges::AddAssetChangeInfo( &new_info );
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
static void FileChangedCallback( const char* filename, Filesystem::WatchDirectoryChangeType change_type )
{
  const char* extension   = Filesystem::GetExtension( filename );
  uint32_t    ext_hash    = Crc32( extension );
  size_t      builder_idx = BinarySearch32( ext_hash, g_BuilderInfos, sizeof( g_BuilderInfos[0] ), g_BuilderCount );

  if ( builder_idx == -1 )
  {
    return;
  }

  AssetId asset_id = AssetId::FromAssetPath( filename );
  if ( change_type == Filesystem::kWatchDirectoryChangeTypeAdded || change_type == Filesystem::kWatchDirectoryChangeTypeModified || change_type == Filesystem::kWatchDirectoryChangeTypeRenamedNew )
  {
    AssetChanges::AssetChangeInfo new_info;
    new_info.m_AssetId = asset_id;
    new_info.m_BuiltVersion = kBuiltVersionNew;
    new_info.m_ExtensionHash = ext_hash;
    strcpy_s( new_info.m_Name, filename );

    AssetChanges::AddAssetChangeInfo(&new_info);
    Log::LogInfo( "Added %s : 0x%8x to asset build list\n", filename, asset_id.ToU32() );
  }

  //printf( "file %s: %s\n", change_names[change_type], filename );
}

//---------------------------------------------------------------------------------
static void LoadBuilderInfos()
{
  char builders_glob[ Filesystem::kMaxPathLen ];
  snprintf( builders_glob, sizeof( builders_glob ), "%s\\*.exe", g_BuildersDirPath );

  WIN32_FIND_DATA find_file_data;
  HANDLE hFind = FindFirstFile( builders_glob, &find_file_data );
  if ( hFind == INVALID_HANDLE_VALUE )
  {
    Log::LogError( "FindFirstFile failed %d. Are there any builders that match %s?\n", GetLastError(), builders_glob );
    return;
  }

  char builder_info_output  [ 128 ];
  char builder_info_command [ Filesystem::kMaxPathLen ];
  do
  {
    MemZero( builder_info_output, sizeof( builder_info_output ) );
    Kami::BuilderInfo* info = &g_BuilderInfos[ g_BuilderCount++ ];

    snprintf( info->m_Path,         sizeof( info->m_Path ),         "%s\\%s",      g_BuildersDirPath, find_file_data.cFileName );
    snprintf( builder_info_command, sizeof( builder_info_command ), "%s -version", info->m_Path );
    Filesystem::RunCommand( builder_info_command, builder_info_output, sizeof( builder_info_output ) );

    const char* ver_hash = builder_info_output;
    char*       end_hash = (char*)strchr( ver_hash, ' ');
    *end_hash = '\0';

    const char* ver_description = end_hash + 3;

    info->m_VersionHash        = std::stoul( ver_hash, nullptr, 0x10 );
    StrCpy( (char*)info->m_VersionDesc, ver_description );

    snprintf( builder_info_command, sizeof( builder_info_command ), "%s -extension", info->m_Path );
    Filesystem::RunCommand( builder_info_command, info->m_Extension, sizeof( info->m_Extension ) );

    RemoveTrailingWhitespace( info->m_Extension, StrLen( info->m_Extension ) );

    info->m_ExtensionHash = Crc32( info->m_Extension );

  } while ( FindNextFile( hFind, &find_file_data ) != 0 );

  QuickSort32( &g_BuilderInfos, sizeof( Kami::BuilderInfo ), g_BuilderCount );
  
  Log::LogInfo( "========================================\n" );
  Log::LogInfo( "\n" );
  Log::LogInfo( "  Builder Info\n" );
  Log::LogInfo( "========================================\n" );

  for ( uint32_t i_builder = 0; i_builder < g_BuilderCount; ++i_builder )
  {
    Kami::BuilderInfo* info = &g_BuilderInfos[ i_builder ];
    Log::LogInfo("ext: .%-17s version: %#08x : %s\n", info->m_Extension, info->m_VersionHash, info->m_VersionDesc );
  }
  Log::LogInfo("\n");
}

//---------------------------------------------------------------------------------
static int LoadConfig()
{
  LoadBuilderInfos();
  AssetDb::LoadStatus db_status = g_AssetDb.LoadFromDisk();

  if ( db_status != AssetDb::kLoadStatusOk && db_status != AssetDb::kLoadStatusDoesNotExist )
  {
    Log::LogError( "Problem with Asset DB. Could not run Kami!\n" );
    return 1;
  }

  if ( db_status  == AssetDb::kLoadStatusDoesNotExist )
  {
    Log::LogError( "Asset db does not exist. Creating at %s\n", g_AssetDb.GetFilePath() );
    g_AssetDb.SaveToDisk();
  }

  Log::LogInfo( "Asset DB Loaded.\n"); 

  return 0;
}

//---------------------------------------------------------------------------------
int Kami::Init()
{
  if ( Filesystem::FileExists( Filesystem::GetAssetsBuiltPath()) == false )
  {
    Filesystem::CreateDir( Filesystem::GetAssetsBuiltPath() );
  }

  g_GlobalSettings.Init( GlobalSettings::kProjectTypeKami );
  g_NumCores           = Arch::GetNumberOfCores();
  g_NumRunningBuilders = 0;

  snprintf( g_BuildersDirPath, sizeof( g_BuildersDirPath ), "%s\\%s\\%s\\Builders", Filesystem::GetOutputPath(), BUILD_PLATFORM, BUILD_CONFIG );
  g_AssetDb.Init();
  AssetChanges::Init();

  int ret = LoadConfig();
  if ( ret )
  {
    return ret;
  }

  // scan filesystem for file changes and new files
  Log::LogInfo( "Scanning for changes since last boot\n" );
  ScanFilesystemForChangedAssets();
  Log::LogInfo( "Found %lu assets that need to be built\n", AssetChanges::GetChangeCount() );
  
  // create change notification handle
  Filesystem::WatchDirectoryForChanges( Filesystem::GetAssetsSourcePath(), &g_FsWatchThread, &FileChangedCallback );

  // Start server
  AssetServer::Init( 4 );

  return 0;
}

//---------------------------------------------------------------------------------
void Kami::Execute()
{
  Sleep(1);
}

//---------------------------------------------------------------------------------
void Kami::Destroy()
{
  AssetServer::Destroy();

  g_FsWatchThread.RequestStop();
  while ( g_FsWatchThread.Joinable() == false );
  g_FsWatchThread.Join();

  AssetChanges::Destroy();
  g_AssetDb.Destroy();
  g_GlobalSettings.Destroy();
}

//---------------------------------------------------------------------------------
const Kami::BuilderInfo* Kami::GetBuilderInfos( uint32_t* count )
{
  *count = g_BuilderCount;
  return g_BuilderInfos;
}

// TODO: Keep track of built assets to delete
// TODO: Create thread(s) to consume asset change requests, and run builders
// TODO: Build dependencies
// TODO: Serve assets