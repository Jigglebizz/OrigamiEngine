#include "Origami/pch.h"

#include <stdio.h>

#include "Origami/Util/Log.h"
#include "Origami/Render/Render.h"
#include "Origami/Asset/AssetLoader.h"

void LogFunction( uint8_t flags, const char* fmt, va_list args )
{
  UNREFERENCED_PARAMETER( flags );
  vprintf( fmt, args );
}

int main( int argc, char* argv[] )
{
  Log::RegisterCallback( LogFunction );

  Render::Init();

  char* test_asset_data = AssetLoader::Load( "test.bmp" );
  Render::TexHandle test_asset_handle = Render::AllocTex( test_asset_data );

  char* test_sprite_data = AssetLoader::Load( "smallsprite.png" );
  Render::TexHandle sprite_handle = Render::AllocTex( test_sprite_data );

  Render::FreeTex ( test_asset_handle );

  test_asset_data = AssetLoader::Load("test.bmp");
  Render::AllocTex( test_asset_data   );

  Render::FreeTex  ( sprite_handle );

  test_sprite_data = AssetLoader::Load("smallsprite.png");
  Render::AllocTex ( test_sprite_data );

  while (1)
  {
    Render::Draw();
    SDL_Delay( 1000 );
  }

  Render::Destroy();
  return 0;
}