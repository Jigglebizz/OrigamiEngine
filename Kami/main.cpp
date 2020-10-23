#include "Origami/pch.h"

#include <rapidjson.h>
#include <fileapi.h>
#include <string> // for stoul

#include "Origami/Concurrency/Thread.h"
#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Util/Sort.h"

#include "Kami/AssetDb.h"

//---------------------------------------------------------------------------------
static constexpr uint32_t kMaxBuildersCount = 32;
static constexpr uint8_t  kMaxExtensionLen  = 16;

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
  uint32_t m_ExtensionHash;
  uint32_t m_BuiltVersion;
  AssetId  m_AssetId;
  char*    m_Name;
};

//static constexpr uint32_t kMaxAssetChanges = 10 * 1024 * 1024; // 10 million seems like plenty for now. 240 megs out the gate for potential asset changes might be a lot tho

AssetChangeInfo* g_AssetChanges;
uint32_t         g_AssetChangesCapacity;
uint32_t         g_AssetChangeCount;

//---------------------------------------------------------------------------------
void GetOutOfDateBuiltAssets()
{
  // create list of assets that are out of date
  uint32_t asset_db_size = g_AssetDb.GetCapacity();
  g_AssetChangesCapacity = ( asset_db_size > 0 ) ? asset_db_size : 1024;

  g_AssetChanges         = (AssetChangeInfo*)malloc( g_AssetChangesCapacity * sizeof( AssetChangeInfo ) );
  g_AssetChangeCount     = 0;

  // scan db for builder version differences
  uint32_t  num_changed_ids   = g_AssetChangesCapacity;
  AssetId*  changed_asset_ids = (AssetId*)malloc( num_changed_ids * sizeof( AssetId ) );

  uint32_t* current_asset_versions = (uint32_t*)malloc( g_BuilderCount * sizeof ( uint32_t ) );
  if ( current_asset_versions == nullptr )
  {
    printf( "Could not allocate memory for current_asset_versions!\n");
    return;
  }

  for ( uint32_t i_builder = 0; i_builder < g_BuilderCount; ++i_builder )
  {
    current_asset_versions[ i_builder ] = g_BuilderInfos[ i_builder ].m_Version;
  }
  QuickSort32( current_asset_versions, sizeof(uint32_t), g_BuilderCount );

  g_AssetDb.GetOutOfDateAssetIds( current_asset_versions, g_BuilderCount, changed_asset_ids, &num_changed_ids );
}

//---------------------------------------------------------------------------------
void ScanFilesystemForNewAssets()
{
  uint32_t* asset_extensions = (uint32_t*)malloc( sizeof(uint32_t) * g_BuilderCount );


  Filesystem::DoForEachFileInDirectory( Filesystem::GetAssetsSourcePath(), [ asset_extensions ]( const Filesystem::FileCallbackParams* file_params ) {
    
    

    printf( "%s %s %s\n", file_params->m_AbsolutePath, file_params->m_RelativePath, file_params->m_Extension );
    UNREFFED_PARAMETER( file_params );


  }, true);
}

//---------------------------------------------------------------------------------
//void AssetDbPersistenceThreadFunc( Thread* thread )
//{
//  UNREFFED_PARAMETER( thread );
//
//  Sleep( 10'000 );
//  g_AssetDb.SaveToDisk();
//}

//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  UNREFFED_PARAMETER( argc );
  UNREFFED_PARAMETER( argv );

  if ( Filesystem::FileExists( Filesystem::GetAssetsBuiltPath()) == false )
  {
    Filesystem::CreateDir( Filesystem::GetAssetsBuiltPath() );
  }

  snprintf( g_BuildersDirPath, sizeof( g_BuildersDirPath ), "%s\\%s\\%s\\Builders", Filesystem::GetOutputPath(), BUILD_PLATFORM, BUILD_CONFIG );
  LoadBuilderInfos();
  g_AssetDb.Init();
  AssetDb::LoadStatus db_status = g_AssetDb.LoadFromDisk();

  switch ( db_status )
  {
  case AssetDb::kLoadStatusDoesNotExist:
  {
    printf( "Asset db does not exist. Creating at %s\n", g_AssetDb.GetFilePath() );
    g_AssetDb.SaveToDisk();
  }
  break;
  case AssetDb::kLoadStatusOk:
  {
    printf( "Asset DB Loaded. Scanning for changes since last boot\n" );
    GetOutOfDateBuiltAssets();
    ScanFilesystemForNewAssets();
  }
  break;
  case AssetDb::kLoadStatusFileProblem:
  default:
  {
    printf( "Problem with Asset DB. Could not run Kami!\n" );
    return 1;
  }
  }

  // scan filesystem for file changes and new files


  // Spawn asset db persistence thread
  // TODO: not sure if this is a great way to do this. Probably better to make it more predictable.
  //       or to use a real database...
  // TODO: decided this isn't really helpful. It's just going to randomly pre-empt my processing.
  //       for now, going to do something more deterministic, ie manually calling save
  //g_AssetDbPersistenceThread.Start( &AssetDbPersistenceThreadFunc );


  // create change notification handle


  // char builder_command[ Filesystem::kMaxPathLen ];
  // snprintf( builder_command, sizeof( builder_command ), "%s\\ActorBuilder.exe -source actor/TestActor.actor", g_BuildersDirPath );

  //g_AssetDbPersistenceThread.RequestStop();
  //while ( g_AssetDbPersistenceThread.Joinable() == false )
  //{
  //  g_AssetDbPersistenceThread.Join();
  //}

  Sleep( 20'000 );

  return 0;
}