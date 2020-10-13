#pragma once

static float g_Time = 0.f;

namespace Game
{
  //---------------------------------------------------------------------------------
  void ENGINE_API Init( const char* title );
  void ENGINE_API Run();
}