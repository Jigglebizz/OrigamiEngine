#include "Origami/pch.h"

#include "Origami/Asset/Asset.h"
#include "Origami/Filesystem/Filesystem.h"
#include "BuilderCommon/BuilderCommon.h"

class ActorBuilder : public BuilderBase
{
public:
  int Build() override;
};

int ActorBuilder::Build()
{
  return 1;
}

int main( int argc, char* argv[] )
{
  char source_asset_path[ Filesystem::kMaxPathLen ];
  MemZero( source_asset_path, sizeof( source_asset_path ) );

  int i_arg = 0;
  while ( i_arg < argc )
  {
    char* current_arg = argv[ i_arg ];
    if (strcmp(current_arg, "-source") == 0 )
    {
      strcpy_s( source_asset_path, argv[ ++i_arg ] );
    }
    i_arg++;
  }

  bool stat = source_asset_path[0] != '\0';
  if ( stat == false )
  {
    printf( "source path not provided!\n" );
    return 1;
  }

  return Build<ActorBuilder>( source_asset_path );
}