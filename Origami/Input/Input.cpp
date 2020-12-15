#include "Origami/pch.h"
#include "Origami/Input/Input.h"

#include "SDL.h"
#include "Origami/Util/Log.h"
#include "Origami/Game/Game.h"

#include "Origami/Render/Render.h"
#include "Origami/Render/imgui/imgui.h"

//---------------------------------------------------------------------------------
void Input::Init()
{
  InputCon* con = &s_InputCon;

  con->m_ButtonEventCount = 0;
  MemZero( &con->m_ButtonEventCount, sizeof ( con->m_ButtonEventCount ) );
}

//---------------------------------------------------------------------------------
void Input::EventPump( float dt )
{
  InputCon* con = &s_InputCon;
  ImGuiIO& io = ImGui::GetIO();

  SDL_Event sdl_evt;
  while ( SDL_PollEvent( &sdl_evt ) )
  {
    switch( sdl_evt.type )
    {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
      ButtonEvent* evt = nullptr;
      bool add_new = true;

      for ( uint32_t i_evt = 0; i_evt < con->m_ButtonEventCount; ++i_evt )
      {
        if ( con->m_ButtonEventQueue[ i_evt ].m_ButtonId == (uint32_t)sdl_evt.key.keysym.sym )
        {
          evt = &con->m_ButtonEventQueue[ i_evt ];
          add_new = false;
        }
      }

      if ( add_new )
      {
        evt = &con->m_ButtonEventQueue[ con->m_ButtonEventCount++ ];
        ASSERT_MSG( con->m_ButtonEventCount != InputCon::kMaxButtonEventsPerFrame, "Exceeded max button events per frame" );
      }

      evt->m_Flags = ButtonEvent::kFlagsSourceKeyboard | ( sdl_evt.type == SDL_KEYDOWN ? ButtonEvent::kFlagsStateDown : ButtonEvent::kFlagsStateUp );
      evt->m_PlayerId = 0;
      evt->m_ButtonId = sdl_evt.key.keysym.sym;
    }
    break;
    case SDL_WINDOWEVENT:
    {
      if ( sdl_evt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
      {
        io.DisplaySize.x = static_cast<float>( sdl_evt.window.data1 );
        io.DisplaySize.y = static_cast<float>( sdl_evt.window.data2 );
        Render::ResizeWindow( sdl_evt.window.data1, sdl_evt.window.data2 );
      }
    }
    break;
    case SDL_MOUSEWHEEL:
    {
      io.MouseWheel = static_cast<float>( sdl_evt.wheel.y );
    }
    break;
    case SDL_QUIT:
      g_GameShouldRun = false;
    default:
      break;
    }
  }

  int mouseX, mouseY;
  const int buttons = SDL_GetMouseState( &mouseX, &mouseY );
  io.DeltaTime      = dt;
  io.MousePos       = ImVec2( static_cast< float >( mouseX ), static_cast< float >( mouseY ) );
  io.MouseDown[ 0 ] = buttons & SDL_BUTTON(  SDL_BUTTON_LEFT );
  io.MouseDown[ 1 ] = buttons & SDL_BUTTON( SDL_BUTTON_RIGHT );
}

//---------------------------------------------------------------------------------
void Input::Update()
{
  InputCon* con = &s_InputCon;

  for ( uint32_t i_button_evt = 0; i_button_evt < con->m_ButtonEventCount; ++i_button_evt )
  {
    ButtonEvent* evt = &con->m_ButtonEventQueue[ i_button_evt ];
    UNREFFED_PARAMETER( evt );
    Log::LogInfo( "Button event: %d was %x\n", evt->m_ButtonId, evt->m_Flags );
  }
  con->m_ButtonEventCount = 0;

}