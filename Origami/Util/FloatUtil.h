#pragma once

static constexpr float g_Pi      = 3.141592f;
static constexpr float g_TwoPi   = 6.283185f;
static constexpr float g_InvPi   = 0.3183099f;
static constexpr float g_PiOver2 = 1.570797f;
static constexpr float g_PiOver4 = 0.785398f;


static void NormalizeFloat( float& f )
{
  f = ( f > 1.0f ) ? 1.0f :
      ( f < 0.0f ) ? 0.0f :
      f;
}