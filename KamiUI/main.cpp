#include "Origami/pch.h"

#include "Origami/Util/Log.h"
#include "Origami/Game/GlobalSettings.h"

#include "Origami/Game/Game.h"
#include "Origami/Input/Input.h"
#include "Origami/Render/Render.h"
#include "Origami/Render/imgui/imgui.h"

#include <chrono>


static float constexpr kFpsTarget = 60.f;
static float constexpr kFrameTime = 1000.f / kFpsTarget;

//---------------------------------------------------------------------------------
static void LogFunction( uint8_t flags, const char* fmt, va_list args )
{
  UNREFFED_PARAMETER( flags );
  Log::Timestamp ts = Log::MsToTimestamp( g_Time );
  printf("[%d:%02d:%02d:%03d]", ts.h, ts.m, ts.s, ts.ms );
  vprintf( fmt, args );
}

//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  Log::RegisterCallback( LogFunction );
  g_GlobalSettings.Init( GlobalSettings::kProjectTypeKami );
  Render::Init( "Kami" );
  Input::Init();

  using namespace std::chrono;
  steady_clock::time_point prev_time = steady_clock::now();
  float dt_accum = 0.f;
  while ( g_GameShouldRun )
  {
    steady_clock::time_point current_time = steady_clock::now();
    float dt = (float)duration_cast<nanoseconds>( current_time - prev_time ).count() / 1'000'000.f;
    Input::EventPump( dt );
    dt_accum += dt;
    g_Time += dt;

    if ( dt_accum >= kFrameTime )
    {
      ImGui::NewFrame();

      Render::Draw();
      
      dt_accum -= kFrameTime;
    }
    prev_time = current_time;
  }

  Render::Destroy();
  g_GlobalSettings.Destroy();
  
  return 0;
}