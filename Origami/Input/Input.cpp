#include "Origami/pch.h"
#include "Origami/Input/Input.h"

#include "SDL.h"
#include "Origami/Util/Log.h"
#include "Origami/Game/Game.h"

//---------------------------------------------------------------------------------
void Input::Init()
{
  InputCon* con = &s_InputCon;

  con->m_ButtonEventCount = 0;
  MemZero( &con->m_ButtonEventCount, sizeof ( con->m_ButtonEventCount ) );
}

//---------------------------------------------------------------------------------
void Input::EventPump()
{
  InputCon* con = &s_InputCon;

  SDL_Event sdl_evt;
  SDL_PollEvent( &sdl_evt );
  
  switch( sdl_evt.type )
  {
  case SDL_KEYDOWN:
  case SDL_KEYUP:
  {
    ButtonEvent* evt = &con->m_ButtonEventQueue[ con->m_ButtonEventCount++ ];
    ASSERT_MSG( con->m_ButtonEventCount == InputCon::kMaxButtonEventsPerFrame, "Exceeded max button events per frame" );

    evt->m_Flags = ButtonEvent::kFlagsSourceKeyboard | ( sdl_evt.type == SDL_KEYDOWN ? ButtonEvent::kFlagsStateDown : ButtonEvent::kFlagsStateUp );
    evt->m_PlayerId = 0;
    evt->m_ButtonId = sdl_evt.key.keysym.sym;
  }
  break;
  case SDL_QUIT:
    g_GameShouldRun = false;
  default:
    break;
  }
}

//---------------------------------------------------------------------------------
void Input::Update()
{
  InputCon* con = &s_InputCon;

  for ( uint32_t i_button_evt = 0; i_button_evt < con->m_ButtonEventCount; ++i_button_evt )
  {
    ButtonEvent* evt = &con->m_ButtonEventQueue[ i_button_evt ];
    Log::LogInfo( "Button event: %d was %x\n", evt->m_ButtonId, evt->m_Flags );
  }
  con->m_ButtonEventCount = 0;

}