#include "Origami/pch.h"

#include "Origami/Game/Game.h"
#include "Origami/Actor/ActorSystem.h"


#include "BaseCharacter.h"


//---------------------------------------------------------------------------------
BaseCharacter s_BaseChar;

//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  UNREFERENCED_PARAMETER( argc );
  UNREFERENCED_PARAMETER( argv );

  Game::Init( "Origami test" );

  Actor::AddActor( &s_BaseChar );

  Game::Run();

  return 0;
}