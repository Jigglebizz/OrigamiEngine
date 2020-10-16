#include "Origami/pch.h"

#include <rapidjson.h>
#include <fileapi.h>
#include <string> // for stoul

#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Util/Sort.h"

//---------------------------------------------------------------------------------
static constexpr uint32_t kMaxBuildersCount = 32;
static constexpr uint8_t  kMaxExtensionLen  = 16;

//---------------------------------------------------------------------------------
char g_SourcePath      [ Filesystem::kMaxPathLen ];
char g_BuildersDirPath [ Filesystem::kMaxPathLen ];

int RunCommand( const char* command, char* output_buf, size_t output_buf_size )
{
  static constexpr size_t tmp_buf_size = 128;
  char tmp_buf[ tmp_buf_size ];
  FILE* exe_pipe = _popen( command, "rt" );

  size_t output_buf_cursor = 0;
  while ( fgets( tmp_buf, tmp_buf_size, exe_pipe ) )
  {
    size_t len_tmp_buf = StrLen( tmp_buf );
    if ( output_buf_cursor + len_tmp_buf < output_buf_size )
    {
      snprintf( &output_buf[ output_buf_cursor ], output_buf_size - output_buf_cursor, "%s", tmp_buf );
    }
    output_buf_cursor += len_tmp_buf;
  }

  if ( feof( exe_pipe ) )
  {
    return _pclose( exe_pipe );
  }

  printf("\nError: Failed to read the pipe to the end.\n");
  return 2;
}

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
    RunCommand( builder_info_command, builder_info_output, sizeof( builder_info_output ) );

    info->m_Version = std::stoul( builder_info_output, nullptr, 0x10 );

    snprintf( builder_info_command, sizeof( builder_info_command ), "%s -extension", info->m_Path );
    RunCommand( builder_info_command, info->m_Extension, sizeof( info->m_Extension ) );

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
    printf("ext: .%-17s version: %#08x", info->m_Extension, info->m_Version );
  }

}

//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  UNREFERENCED_PARAMETER( argc );
  UNREFERENCED_PARAMETER( argv );

  snprintf( g_BuildersDirPath, sizeof( g_BuildersDirPath ), "%s\\%s\\%s\\Builders", Filesystem::GetOutputPath(), BUILD_PLATFORM, BUILD_CONFIG );

  LoadBuilderInfos();

  // scan for changes since last opened



  // create change notification handle


  char builder_command[ Filesystem::kMaxPathLen ];
  snprintf( builder_command, sizeof( builder_command ), "%s\\ActorBuilder.exe -source actor/TestActor.actor", g_BuildersDirPath );

  return 0;
}