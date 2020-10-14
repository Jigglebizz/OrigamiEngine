#pragma once

#if BUILD < BUILD_FINAL
#undef NDEBUG
#endif

#include <assert.h>

static float g_Time = 0.f;

namespace Game
{
  //---------------------------------------------------------------------------------
  void ENGINE_API Init( const char* title, const char* window_icon = nullptr );
  void ENGINE_API Run();
}