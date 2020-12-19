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

// Log is in a ring buffer
// TODO: Implement as its own class. This might be reuseable
static uint32_t           s_LogHead;
static uint32_t           s_LogReadHead;
static bool               s_LogHasLooped;
static char               s_LogBuffer[ 4 MB ];
static uint32_t constexpr kLogLoopIdx = sizeof(s_LogBuffer) - 1;

//---------------------------------------------------------------------------------
//static void InitFrontLoadingLogRing()
//{
//  s_LogBuffer[ kLogLoopIdx ] = '\0';
//  s_LogHead = kLogLoopIdx - 1;
//  s_LogHasLooped = false;
//}
//
////---------------------------------------------------------------------------------
//// TODO: This is probably still useful but it's not needed here
//static void LogRingAppendFront( const char* string )
//{
//  uint32_t string_len = StrLen( string );
//
//  if( string_len > s_LogHead )
//  {
//    s_LogHasLooped = true;
//
//    memcpy( s_LogBuffer, &string[ string_len - s_LogHead ], s_LogHead );
//    uint32_t remaining_len = string_len - s_LogHead;
//    s_LogHead = kLogLoopIdx - remaining_len;
//    
//    memcpy( &s_LogBuffer[ s_LogHead ], string, remaining_len );
//  }
//  else
//  {
//    s_LogHead -= string_len;
//    memcpy( &s_LogBuffer[ s_LogHead ], string, string_len );
//  }
//
//  if ( s_LogHead != 0 )
//  {
//    s_LogBuffer[ s_LogHead - 1 ] = '\0';
//  }
//}

//---------------------------------------------------------------------------------
static void InitBackLoadingLogRing()
{
  MemZero( s_LogBuffer, sizeof( s_LogBuffer ) );
  s_LogHead                  = 0;
  s_LogReadHead              = 0;
  s_LogHasLooped             = false;
}

//---------------------------------------------------------------------------------
static void LogRingAppendEnd(const char* string)
{
  uint32_t string_len = StrLen(string);

  if ( s_LogHead + string_len < kLogLoopIdx )
  {
    memcpy(&s_LogBuffer[s_LogHead], string, string_len);
    s_LogHead += string_len;
  }
  else
  {
    s_LogHasLooped = true;

    uint32_t initial_consume_len = kLogLoopIdx - s_LogHead;
    memcpy( &s_LogBuffer[ s_LogHead ], string, initial_consume_len );

    s_LogHead = string_len - initial_consume_len;
    memcpy( &s_LogBuffer[ 0 ], &string[ initial_consume_len ], s_LogHead );
  }

  if ( s_LogHasLooped )
  {
    s_LogReadHead = ( s_LogHead != kLogLoopIdx - 1 ) ? s_LogHead + 1 : 0;
    if ( s_LogReadHead != 0 )
    {
      s_LogBuffer[ s_LogReadHead - 1 ] = '\0';
    }
  }
}

//---------------------------------------------------------------------------------
static void LogFunction( uint8_t flags, const char* fmt, va_list args )
{
  UNREFFED_PARAMETER( flags );

  char timestamp[15];

  Log::Timestamp ts = Log::MsToTimestamp( g_Time );
  snprintf( timestamp, sizeof(timestamp), "[%d:%02d:%02d:%03d]", ts.h, ts.m, ts.s, ts.ms );

  LogRingAppendEnd( timestamp );
  
  char log_line[ 128 ];
  vsnprintf( log_line, sizeof( log_line ), fmt, args );
  
  LogRingAppendEnd( log_line );
}

//---------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  UNREFFED_PARAMETER( argc );
  UNREFFED_PARAMETER( argv );

  InitBackLoadingLogRing();
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
          UNREFFED_PARAMETER( builder_infos );

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
          ImGui::TextUnformatted( &s_LogBuffer[ s_LogReadHead ] );
          if ( s_LogHead != 0 && s_LogHasLooped )
          {
            ImGui::TextUnformatted( &s_LogBuffer[0] );
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
  
  return 0;
}