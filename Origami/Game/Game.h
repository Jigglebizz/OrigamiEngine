#pragma once

#if BUILD < BUILD_FINAL
#undef NDEBUG
#endif

#include <assert.h>

extern float g_Time;
extern bool  g_GameShouldRun;

namespace Game
{
  //---------------------------------------------------------------------------------
  void ENGINE_API Init( const char* title, const char* window_icon = nullptr );
  void ENGINE_API Run();
}