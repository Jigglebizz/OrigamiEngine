#pragma once

#if BUILD < BUILD_FINAL
#undef NDEBUG
#endif

#include <assert.h>

static          float g_Time          = 0.f;
static volatile bool  g_GameShouldRun = true; // volatile was required to get around 'unreachable code' warning

namespace Game
{
  //---------------------------------------------------------------------------------
  void ENGINE_API Init( const char* title, const char* window_icon = nullptr );
  void ENGINE_API Run();
}