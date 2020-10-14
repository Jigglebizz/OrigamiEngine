#include "Origami/pch.h"
#include "Origami/Game/Game.h"

#include <stdio.h>
#include <chrono>

#include "Origami/Util/Log.h"
#include "Origami/Anim/Anim.h"
#include "Origami/Input/Input.h"
#include "Origami/Render/Render.h"
#include "Origami/Asset/AssetLoader.h"
#include "Origami/Actor/ActorSystem.h"

//---------------------------------------------------------------------------------
static float constexpr kFpsTarget = 24.f;
static float constexpr kFrameTime = 1000.f / kFpsTarget;


//---------------------------------------------------------------------------------
void LogFunction( uint8_t flags, const char* fmt, va_list args )
{
  UNREFERENCED_PARAMETER( flags );
  Log::Timestamp ts = Log::MsToTimestamp( g_Time );
  printf("[%d:%02d:%02d:%03d]", ts.h, ts.m, ts.s, ts.ms );
  vprintf( fmt, args );
}

//---------------------------------------------------------------------------------
void Game::Init( const char* title, const char* window_icon )
{
  Log::RegisterCallback(LogFunction);
  Actor::Init();
  Render::Init( title, window_icon );
  Anim::Init();
  Input::Init();
}

//---------------------------------------------------------------------------------
void UpdateFirst( float dt )
{
  Render::Draw(); // TODO: decouple this, put in its own thread
  Input::Update();
  Anim::Update();
  Actor::UpdateFirst( dt );
}

//---------------------------------------------------------------------------------
void UpdateMiddle( float dt )
{
  Actor::UpdateMiddle( dt );
}

//---------------------------------------------------------------------------------
void UpdateLast( float dt )
{
  Actor::UpdateLast( dt );
}

//---------------------------------------------------------------------------------
void Destroy()
{
  Anim::Destroy();
  Render::Destroy();
}

//---------------------------------------------------------------------------------
void Game::Run()
{
  using namespace std::chrono;
  steady_clock::time_point prev_time = steady_clock::now();
  float dt_accum = 0.f;
  while ( g_GameShouldRun )
  {
    Input::EventPump();
    steady_clock::time_point current_time = steady_clock::now();
    float dt = (float)duration_cast<nanoseconds>( current_time - prev_time ).count() / 1'000'000.f;
    dt_accum += dt;
    g_Time   += dt;

    if ( dt_accum >= kFrameTime )
    {
      UpdateFirst  ( dt_accum );
      UpdateMiddle ( dt_accum );
      UpdateLast   ( dt_accum );

      dt_accum -= kFrameTime;
    }

    prev_time = current_time;
  }

  Destroy();
}