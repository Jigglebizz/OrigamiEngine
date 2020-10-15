#include "Origami/pch.h"

#include "Origami/Asset/Asset.h"
#include "Origami/Filesystem/Filesystem.h"
#include "BuilderCommon/BuilderCommon.h"

//---------------------------------------------------------------------------------
class ActorBuilder : public BuilderBase
{
public:
  int          Build            ()       override;
  AssetVersion GetAssetVersion  () const override;
  const char*  GetAssetExtension() const override;
};

//---------------------------------------------------------------------------------
int ActorBuilder::Build()
{
  return 0;
}

//---------------------------------------------------------------------------------
AssetVersion ActorBuilder::GetAssetVersion() const
{
  return kAssetVersionActor;
}

//---------------------------------------------------------------------------------
const char* ActorBuilder::GetAssetExtension() const
{
  return "actor";
}

//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  return RunBuilder<ActorBuilder>( argc, argv );
}