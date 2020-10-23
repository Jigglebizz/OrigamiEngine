#pragma once

#include <vector>
#include <string>

namespace Filesystem
{
  //---------------------------------------------------------------------------------
  constexpr uint32_t kMaxAssetNameLen = 64;
  constexpr uint32_t kMaxPathLen = 128;

  //---------------------------------------------------------------------------------
  struct FileCallbackParams
  {
    const char* m_AbsolutePath;
    const char* m_RelativePath;
    const char* m_Extension;
  };

  //---------------------------------------------------------------------------------
  template< typename FileCb >
  void DoForEachFileInDirectoryImpl(const char* base_path, const char* current_path, FileCb callback, bool recursive)
  {
    char search_str[ Filesystem::kMaxPathLen ];
    snprintf( search_str, sizeof( search_str ), "%s\\%s\\*", base_path, current_path );

    WIN32_FIND_DATA file;
    HANDLE search_handle = FindFirstFile( search_str, &file );
    if ( search_handle != INVALID_HANDLE_VALUE )
    {
      std::vector< std::string > directories; // @TODO: Fix this, lazy ass
      char file_path[ Filesystem::kMaxPathLen ];

      do
      {
        if ( file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
          if ( strcmp( file.cFileName, "." ) == 0 || strcmp( file.cFileName, "..") == 0 )
          {
            continue;
          }

          directories.push_back( file.cFileName );
        }
        else
        {

          snprintf( file_path, sizeof( file_path ), "%s%s%s\\%s", base_path, *current_path ? "\\" : "", current_path, file.cFileName );
          Filesystem::FileCallbackParams cb_params;
          cb_params.m_AbsolutePath = file_path;
          cb_params.m_RelativePath = cb_params.m_AbsolutePath + StrLen( base_path );
          cb_params.m_Extension    = strchr( cb_params.m_RelativePath, '.' );
          callback( &cb_params );
        }
      }
      while ( FindNextFile( search_handle, & file ) );

      FindClose( search_handle );

      if ( recursive )
      {
        for ( auto it_dir = directories.begin(); it_dir != directories.end(); ++it_dir )
        {
        
          if ( StrLen( current_path ) == 0 )
          {
            strcpy_s( file_path, it_dir->c_str() );
          }
          else
          {
            snprintf( file_path, sizeof( file_path ), "%s\\%s", current_path, it_dir->c_str() );
          }
          DoForEachFileInDirectoryImpl( base_path, file_path, callback, true );
        }
      }
    }
  }

  //---------------------------------------------------------------------------------
  const char* ENGINE_API GetAssetsSourcePath ();
  const char* ENGINE_API GetAssetsBuiltPath  ();
  const char* ENGINE_API GetOutputPath       ();

  const bool  ENGINE_API FileExists          ( const char* path );
  void        ENGINE_API CreateDir           ( const char* path );
  int         ENGINE_API RunCommand          ( const char* command, char* output_buf, size_t output_buf_size );

  //---------------------------------------------------------------------------------
  template< typename FileCallback >
  void ENGINE_API DoForEachFileInDirectory( const char* dir_path, FileCallback callback, bool recursive = false)
  {
    DoForEachFileInDirectoryImpl( dir_path, "", callback, recursive );
  }
}