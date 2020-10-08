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
  Render::Init();

  s_BaseChar.Init();
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

  //char* test_asset_data = AssetLoader::Load( "test.bmp" );
  //Render::TexHandle test_asset_handle = Render::AllocTex( test_asset_data );

  //char* test_sprite_data = AssetLoader::Load( "smallsprite.png" );
  //Render::TexHandle sprite_handle = Render::AllocTex( test_sprite_data );

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