#include "Origami/pch.h"

#include <stdio.h>
#include <chrono>

#include "Origami/Util/Log.h"
#include "Origami/Render/Render.h"
#include "Origami/Asset/AssetLoader.h"

#include "BaseCharacter.h"

//---------------------------------------------------------------------------------
static float constexpr kFpsTarget = 24.f;
static float constexpr kFrameTime = 1000.f / kFpsTarget;

BaseCharacter s_BaseChar;
Vec2          s_Pos;

//---------------------------------------------------------------------------------
void LogFunction( uint8_t flags, const char* fmt, va_list args )
{
  UNREFERENCED_PARAMETER( flags );
  vprintf( fmt, args );
}

//---------------------------------------------------------------------------------
void Init()
{
  Log::RegisterCallback(LogFunction);
  Render::Init( "Origami Test" );

  s_BaseChar.Init();
  s_Pos = { 0,0 };
}

//---------------------------------------------------------------------------------
void UpdateFirst( float dt )
{
  UNREFERENCED_PARAMETER( dt );
  Render::Draw();

  s_BaseChar.UpdateFirst( dt );
}

//---------------------------------------------------------------------------------
void UpdateMiddle( float dt )
{
  UNREFERENCED_PARAMETER( dt );
  s_BaseChar.UpdateMiddle( dt );

  s_BaseChar.SetPosition( &s_Pos );
  s_Pos.x += dt * 0.1f;
  s_Pos.y += dt * 0.12f;
  if (s_Pos.x > 400)
  {
    s_Pos.x -= 400;
  }
  if (s_Pos.y > 300)
  {
    s_Pos.y -= 300;
  }
}

//---------------------------------------------------------------------------------
void UpdateLast( float dt )
{
  UNREFERENCED_PARAMETER( dt );
  s_BaseChar.UpdateLast( dt );
}

//---------------------------------------------------------------------------------
void Destroy()
{
  Render::Destroy();
}

//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  UNREFERENCED_PARAMETER( argc );
  UNREFERENCED_PARAMETER( argv );

  using namespace std::chrono;

  Init();

  SDL_Event evt;

  float frame_accum_dt = 0;
  steady_clock::time_point time = steady_clock::now();
  while (1)
  {
    SDL_PollEvent( &evt );

    float dt = (float)duration_cast<nanoseconds>( steady_clock::now() - time ).count() / 1000000.f;
    frame_accum_dt += dt;

    if ( frame_accum_dt >= kFrameTime )
    {
      UpdateFirst  ( frame_accum_dt );
      UpdateMiddle ( frame_accum_dt );
      UpdateLast   ( frame_accum_dt );

      frame_accum_dt -= kFrameTime;
    }

    time = steady_clock::now();
  }

  Destroy();
  return 0;
}