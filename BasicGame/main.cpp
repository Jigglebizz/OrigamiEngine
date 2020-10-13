#include "Origami/pch.h"

#include "Origami/Game/Game.h"
#include "Origami/Actor/ActorSystem.h"


#include "BaseCharacter.h"


//---------------------------------------------------------------------------------

BaseCharacter s_BaseChar;
Vec2          s_Pos;



//---------------------------------------------------------------------------------
//void Init()
//{
//
//
//  s_BaseChar.Init();
//  s_Pos = { 0,0 };
//}

//---------------------------------------------------------------------------------
//void UpdateFirst( float dt )
//{
//  s_BaseChar.UpdateFirst( dt );
//}
//
////---------------------------------------------------------------------------------
//void UpdateMiddle( float dt )
//{
//  UNREFERENCED_PARAMETER( dt );
//  s_BaseChar.UpdateMiddle( dt );
//
//  s_BaseChar.SetPosition( &s_Pos );
//  s_Pos.x += dt * 0.1f;
//  s_Pos.y += dt * 0.12f;
//  if (s_Pos.x > 400)
//  {
//    s_Pos.x -= 400;
//  }
//  if (s_Pos.y > 300)
//  {
//    s_Pos.y -= 300;
//  }
//}
//
////---------------------------------------------------------------------------------
//void UpdateLast( float dt )
//{
//  UNREFERENCED_PARAMETER( dt );
//  s_BaseChar.UpdateLast( dt );
//}


//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  UNREFERENCED_PARAMETER( argc );
  UNREFERENCED_PARAMETER( argv );

  Game::Init( "Origami test" );

  Actor::AddActor( &s_BaseChar );

  Game::Run();

  /*using namespace std::chrono;

  Init();

  float frame_accum_dt = 0;
  steady_clock::time_point time = steady_clock::now();
  while (1)
  {
    Input::EventPump();
    steady_clock::time_point new_time = steady_clock::now();
    float dt = (float)duration_cast<nanoseconds>( new_time - time ).count() / 1000000.f;
    time = new_time;
    frame_accum_dt += dt;
    g_Time += dt;


    if ( frame_accum_dt >= kFrameTime )
    {
      UpdateFirst  ( frame_accum_dt );
      UpdateMiddle ( frame_accum_dt );
      UpdateLast   ( frame_accum_dt );

      frame_accum_dt -= kFrameTime;
    }

  }

  Destroy();
  return 0;*/
}