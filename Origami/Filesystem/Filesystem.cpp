#include "Origami/pch.h"
#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Util/Log.h"

#include <direct.h>
#include <sys/stat.h>
#include <fstream>

#ifndef SOLUTION_NAME
#define SOLUTION_NAME ""
#endif

//---------------------------------------------------------------------------------
const char* GetSolutionBasePath()
{
  const char PATH_SEP = '\\';

  static char base_path[ Filesystem::kMaxPathLen ];

  if ( *base_path == 0 )
  {
    char* cwd_path = _getcwd( NULL, Filesystem::kMaxPathLen );

    if ( cwd_path )
    {
      const char* solution_dir_name = SOLUTION_NAME;
      char* solution_folder_start   = strstr( cwd_path, solution_dir_name );
      
      if ( solution_folder_start == nullptr )
      {
        Log::LogError( "Broken folder structure!" );
        return "";
      }

      solution_folder_start[ StrLen( solution_dir_name ) + 1 ] = 0;

      snprintf( base_path, Filesystem::kMaxPathLen, "%s", cwd_path );
      free( cwd_path );
    }
  }
  return base_path;
}

//---------------------------------------------------------------------------------
const char* GetAssetsBasePath()
{
  static char base_path[ Filesystem::kMaxPathLen ];

  if ( *base_path == 0 )
  {
    snprintf( base_path, Filesystem::kMaxPathLen, "%s%s", GetSolutionBasePath(), "Assets" );
  }

  return base_path;
}

//---------------------------------------------------------------------------------
const char* Filesystem::GetAssetsSourcePath()
{
  static char source_path[ kMaxPathLen ];
  if ( *source_path == 0)
  {
    snprintf( source_path, kMaxPathLen, "%s%s", GetAssetsBasePath(), "\\source" );
  }
  return source_path;
}

//---------------------------------------------------------------------------------
const char* Filesystem::GetAssetsBuiltPath()
{
  static char built_path[ kMaxPathLen ];
  if ( *built_path == 0)
  {
    snprintf( built_path, kMaxPathLen, "%s%s", GetAssetsBasePath(), "\\built");
  }
  return built_path;
}

//---------------------------------------------------------------------------------
const char* Filesystem::GetOutputPath()
{
  static char output_path[ kMaxPathLen ];
  if ( *output_path == 0 )
  {
    snprintf( output_path, kMaxPathLen, "%s%s", GetSolutionBasePath(), "Output" );
  }
  return output_path;
}

//---------------------------------------------------------------------------------
const char* Filesystem::GetExtension( const char* path )
{
  return strchr( path , '.') + 1;
}

//---------------------------------------------------------------------------------
const char* Filesystem::GetFilename( const char* full_path )
{
  return strrchr( full_path, '\\' ) + 1;
}

//---------------------------------------------------------------------------------
const bool Filesystem::FileExists( const char* name )
{
  struct stat buffer;
  return ( stat( name, &buffer ) == 0 );
}

//---------------------------------------------------------------------------------
void Filesystem::CreateDir( const char* name )
{
  _mkdir( name );
}

//---------------------------------------------------------------------------------
uint64_t Filesystem::GetFileSize( const char* file_path )
{
  HANDLE file_handle = CreateFile( file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );
  if ( file_handle == INVALID_HANDLE_VALUE )
  {
    return kInvalidFilesize;
  }

  uint64_t size;
  if ( GetFileSizeEx( file_handle, (PLARGE_INTEGER)&size ) )
  {
    return size;
  }

  return kInvalidFilesize;
}

//---------------------------------------------------------------------------------
VOID CALLBACK FileIoCompletionRoutine(
  __in  DWORD dwErrorCode,
  __in  DWORD dwNumberOfBytesTransfered,
  __in  LPOVERLAPPED lpOverlapped )
 {
  UNREFFED_PARAMETER( dwErrorCode );
  UNREFFED_PARAMETER( dwNumberOfBytesTransfered );
  UNREFFED_PARAMETER( lpOverlapped );
 }

//---------------------------------------------------------------------------------
void Filesystem::ReadFile( const char* file_path, char* out_data, size_t* data_size )
{
  static constexpr uint32_t kReadBufferSize = 32;

  HANDLE file_handle;
  OVERLAPPED ol = { 0 };

  file_handle = CreateFile( file_path, 
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                            NULL );

  if ( file_handle == INVALID_HANDLE_VALUE )
  {
    Log::LogError( "Could not open file %s\n", file_path );
    *data_size = 0;
    return;
  }

  if ( ReadFileEx( file_handle, out_data, (DWORD)*data_size, &ol, FileIoCompletionRoutine ) == false )
  {
    Log::LogError( "Could not read file %s\n", file_path );
    *data_size = 0;
    return;
  }

  uint64_t size;
  if ( GetFileSizeEx( file_handle, (PLARGE_INTEGER)&size ) && size <= *data_size )
  {
    *data_size = size;
  }
}

//---------------------------------------------------------------------------------
int Filesystem::RunCommand( const char* command, char* output_buf, size_t output_buf_size )
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
struct DirInfo
{
  HANDLE hDir;
  TCHAR lpszDirName[MAX_PATH];
};

//---------------------------------------------------------------------------------
void Filesystem::WatchDirectoryForChangesThreadFunction( Thread* thread, void* params )
{
  const WatchDirectoryForChangesParams* watch_params = (WatchDirectoryForChangesParams*)params;
  const char*                           directory    = watch_params->m_Directory;
        WatchDirectoryCallback          callback     = watch_params->m_Callback;

  char buf[2048];
  DWORD nRet;
  BOOL result = TRUE;
  char filename[ kMaxPathLen ];
  DirInfo dir_info;
  dir_info.hDir = CreateFile( directory, GENERIC_READ | FILE_LIST_DIRECTORY,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                         NULL );
  if ( dir_info.hDir == INVALID_HANDLE_VALUE )
  {
    printf( "Cannot open folder %s\n", directory );
    thread->RequestStop();
    return;
  }

  lstrcpy( dir_info.lpszDirName, directory );
  OVERLAPPED polling_overlap;

  FILE_NOTIFY_INFORMATION* pNotify;
  int offset;
  polling_overlap.OffsetHigh = 0;
  polling_overlap.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

  if ( polling_overlap.hEvent == NULL )
  {
    printf( "Could not create file change event: %d\n", GetLastError() );
    thread->RequestStop();
    return;
  }

  while ( result )
  {
    result = ReadDirectoryChangesW(
      dir_info.hDir,
      &buf,
      sizeof( buf ),
      TRUE,
      FILE_NOTIFY_CHANGE_FILE_NAME |
      FILE_NOTIFY_CHANGE_DIR_NAME |
      FILE_NOTIFY_CHANGE_SIZE,
      &nRet,
      &polling_overlap,
      NULL
    );

    WaitForSingleObject( polling_overlap.hEvent, INFINITE );
    offset = 0;
    do
    {
      pNotify = (FILE_NOTIFY_INFORMATION*)( (char*)buf + offset );
      strcpy_s( filename, "" );
      WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), NULL, NULL);
      filename[ pNotify->FileNameLength / 2 ] = '\0';

      WatchDirectoryChangeType change_type = kWatchDirectoryChangeTypeUnknown;
      switch ( pNotify->Action )
      {
      case FILE_ACTION_ADDED:
        change_type = kWatchDirectoryChangeTypeAdded;
        break;
      case FILE_ACTION_REMOVED:
        change_type = kWatchDirectoryChangeTypeRemoved;
        break;
      case FILE_ACTION_MODIFIED:
        change_type = kWatchDirectoryChangeTypeModified;
        break;
      case FILE_ACTION_RENAMED_OLD_NAME:
        change_type = kWatchDirectoryChangeTypeRenamedOld;
        break;
      case FILE_ACTION_RENAMED_NEW_NAME:
        change_type = kWatchDirectoryChangeTypeRenamedNew;
        break;
      default:
        break;
      }

      callback( filename, change_type );

      offset += pNotify->NextEntryOffset;
    }
    while ( pNotify->NextEntryOffset );
    Sleep(1);
  }

  CloseHandle( dir_info.hDir );
  g_DynamicHeap.Free( params );
}

//---------------------------------------------------------------------------------
void ENGINE_API Filesystem::WatchDirectoryForChanges( const char* directory, Thread* thread, WatchDirectoryCallback callback )
{
  WatchDirectoryForChangesParams* params = (WatchDirectoryForChangesParams*)g_DynamicHeap.Alloc( sizeof( WatchDirectoryForChangesParams ) );
  strcpy_s( params->m_Directory, directory );
  params->m_Callback = callback;
  thread->Start( &WatchDirectoryForChangesThreadFunction, (void*)params );
}