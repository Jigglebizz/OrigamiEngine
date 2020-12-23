#include "Origami/pch.h"

#include "Origami/Util/Log.h"
#include "Origami/Game/GlobalSettings.h"

#include "Origami/Game/Game.h"
#include "Origami/Input/Input.h"
#include "Origami/Render/Render.h"
#include "Origami/Render/imgui/imgui.h"
#include "Origami/Filesystem/Filesystem.h"

#include "Kami/Kami.h"

#include <chrono>



static float constexpr kFpsTarget = 60.f;
static float constexpr kFrameTime = 1000.f / kFpsTarget;

static char  s_LogBuffer[ 4 MB ];
Log::LogRing s_LogRing;

//---------------------------------------------------------------------------------
static void LogFunction( uint8_t flags, const char* fmt, va_list args )
{
  UNREFFED_PARAMETER( flags );

  char timestamp[15];

  Log::Timestamp ts = Log::MsToTimestamp( g_Time );
  snprintf( timestamp, sizeof(timestamp), "[%d:%02d:%02d:%03d]", ts.h, ts.m, ts.s, ts.ms );

  s_LogRing.Append( timestamp );
  
  char log_line[ 128 ];
  vsnprintf( log_line, sizeof( log_line ), fmt, args );
  
  s_LogRing.Append( log_line );
}

//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  UNREFFED_PARAMETER( argc );
  UNREFFED_PARAMETER( argv );

  s_LogRing.InitWithBacking( &s_LogBuffer, sizeof( s_LogBuffer ) );
  Log::RegisterCallback( LogFunction );

  int ret = Kami::Init();
  if ( ret )
  {
    return ret;
  }

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

    Kami::Execute();

    if ( dt_accum >= kFrameTime )
    {
      ImGui::NewFrame();

      ImVec2 pos = { 0, 0 };
      ImGui::SetNextWindowPos( pos );

      Vec2Int ogmi_size = Render::GetWindowSize();
      ImVec2  size      = { (float)ogmi_size.x, (float)ogmi_size.y };
      ImGui::SetNextWindowSize( size );
      ImGui::Begin( "Kami", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );//, ImGuiWindowFlags_NoInputs );

        ImGui::BeginTabBar( "##MainNavTabs" );

        if ( ImGui::BeginTabItem( "Settings" ) )
        {
          ImGui::Text( "Settings" );

          ImGui::EndTabItem();
        }

        if ( ImGui::BeginTabItem( "Builders" ) )
        {
          uint32_t builder_count;
          const Kami::BuilderInfo* builder_infos = Kami::GetBuilderInfos( &builder_count );

          if ( ImGui::BeginTable( "##builders_table", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable ) )
          {
            ImGui::TableSetupColumn( "Asset Type",     ImGuiTableColumnFlags_DefaultSort, ImGuiTableColumnFlags_WidthAutoResize );
            ImGui::TableSetupColumn( "Executable",     ImGuiTableColumnFlags_WidthAutoResize );
            ImGui::TableSetupColumn( "Version Hash",   ImGuiTableColumnFlags_WidthAutoResize );
            ImGui::TableSetupColumn( "Version String", ImGuiTableColumnFlags_WidthStretch );

            ImGui::TableHeadersRow();

            for ( uint32_t i_builder = 0; i_builder < builder_count; i_builder++ )
            {
              const Kami::BuilderInfo* builder = &builder_infos[ i_builder ];

              ImGui::TableNextRow();
              ImGui::TableNextColumn();
              ImGui::Text( builder->m_Extension );
              ImGui::TableNextColumn();
              ImGui::Text( Filesystem::GetFilename( builder->m_Path ) );
              ImGui::TableNextColumn();
              ImGui::Text( "%ul", builder->m_VersionHash );
              ImGui::TableNextColumn();
              ImGui::Text( builder->m_VersionDesc );
            }
            ImGui::EndTable();
          }

          ImGui::EndTabItem();
        }

        if ( ImGui::BeginTabItem( "Log" ) )
        {
          ImGui::TextUnformatted( s_LogRing.GetLogA() );
          if ( const char* log_b = s_LogRing.GetLogB() )
          {
            ImGui::TextUnformatted( log_b );
          }
          ImGui::SetScrollHereY( 1.0f );

          ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
      ImGui::End();

      Render::Draw();
      
      dt_accum -= kFrameTime;
    }
    prev_time = current_time;
  }

  Render::Destroy();
  Kami::Destroy();
  s_LogRing.Destroy();

  return 0;
}