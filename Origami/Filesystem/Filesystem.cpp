#include "Origami/pch.h"
#include "Origami/Filesystem/Filesystem.h"

#include <direct.h>

//---------------------------------------------------------------------------------
const char* GetAssetsBasePath()
{
#ifdef _WIN32
  const char PATH_SEP = '\\';
#else
  const char PATH_SEP = '/';
#endif

  static char base_path[ Filesystem::kMaxPathLen ];

  if ( *base_path == 0 )
  {
    char* cwd_path = _getcwd( NULL, Filesystem::kMaxPathLen );

    if ( cwd_path )
    {
      const char* project_dir_name = "Origami";
      char* project_folder_start   = strstr( cwd_path, project_dir_name );
      
      if ( project_folder_start == nullptr )
      {
        Log::LogError( "Broken folder structure!" );
        return "";
      }

      project_folder_start[ StrLen( project_dir_name ) + 1 ] = 0;

      snprintf( base_path, Filesystem::kMaxPathLen, "%s%s", cwd_path, "Assets" );
      free( cwd_path );
    }
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