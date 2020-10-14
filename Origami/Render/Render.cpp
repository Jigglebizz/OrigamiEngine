#include "Origami/pch.h"
#include "Render.h"

#include "Origami/Asset/AssetLoader.h"
#include "Origami/Util/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//---------------------------------------------------------------------------------
void Render::InitRenderCon()
{
  RenderCon* con = &s_RenderCon;
  MemZero( con, sizeof( *con ) );

  for ( uint16_t i_layer = 0; i_layer < kMaxLayers; ++i_layer )
  {
    con->m_LayerHeads[ i_layer ] = kLayerEnd;
    con->m_LayerTails[ i_layer ] = kLayerEnd;
  }

  for ( uint16_t i_tex = 0; i_tex < kMaxTextures; ++i_tex )
  {
    con->m_Textures[ i_tex ].Init();
    if (i_tex != kMaxTextures - 1)
    {
      con->m_Textures[ i_tex ].m_Next = i_tex + 1;
    }
  }
}

//---------------------------------------------------------------------------------
void Render::TextureInfo::Init()
{
  m_SdlTex = nullptr;
  m_Asset  = nullptr;
  MemZero( &m_ImageRect,  sizeof( m_ImageRect ) );
  MemZero( &m_Position,   sizeof( m_Position  ) );
  m_Scale.x = 1.f;
  m_Scale.y = 1.f;
  m_Next   = kTexturesEnd;
  m_Prev   = kTexturesEnd;
}

//---------------------------------------------------------------------------------
void Render::TextureInfo::Free()
{
  if ( m_SdlTex )
  {
    SDL_DestroyTexture( m_SdlTex );
  }

  if ( m_Asset )
  {
    AssetLoader::Free( (char*)m_Asset );
  }
}

//---------------------------------------------------------------------------------
void Render::Init( const char* window_title, const char* window_icon )
{
  InitRenderCon();

  // Init SDL
  if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER ) )
  {
    Log::LogError("Failed to init SDL! %s\n", SDL_GetError() );
    return;
  }

  RenderCon* con = &s_RenderCon;

  con->m_Window = SDL_CreateWindow( window_title, 100, 100, 640, 480, SDL_WINDOW_SHOWN );
  if ( con->m_Window == nullptr )
  {
    Log::LogError("SDL_CreateWindow Error: %s\n", SDL_GetError() );
    Destroy();
    return;
  }

  con->m_Renderer = SDL_CreateRenderer( con->m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
  if ( con->m_Renderer == nullptr )
  {
    Log::LogError( "SDL_CreateRenderer Error: %s\n", SDL_GetError() );
    Destroy();
    return;
  }

  // Load window icon
  static constexpr char default_icon[] = "icon\\OrigamiIcon64.png";
  char icon_full_path[ Filesystem::kMaxPathLen ];
  snprintf( icon_full_path, Filesystem::kMaxPathLen, "%s\\%s", Filesystem::GetAssetsSourcePath(), ( window_icon ) ? window_icon : default_icon );

  int w, h, format;
  unsigned char* pixel_data = stbi_load( icon_full_path, &w, &h, &format, STBI_rgb_alpha );

  if ( pixel_data != nullptr )
  {
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormatFrom( (void*)pixel_data, w, h, 32, 4 * w, SDL_PIXELFORMAT_RGBA32 );
    if ( surf != nullptr )
    {
      SDL_SetWindowIcon( con->m_Window, surf );
    }
    else
    {
      Log::LogError( "Creating surface failed for icon %s: %s", icon_full_path, SDL_GetError() );
    }
    SDL_FreeSurface( surf );
  }
  else
  {
    Log::LogError( "Loading window icon %s failed: %s\n", icon_full_path, stbi_failure_reason() );
    return;
  }

  stbi_image_free( pixel_data );
}

//---------------------------------------------------------------------------------
void Render::Destroy()
{
  RenderCon* con = &s_RenderCon;

  for ( uint16_t i_tex = 0; i_tex < kMaxTextures; i_tex++ )
  {
    con->m_Textures[i_tex].Free();
  }

  if ( con->m_Renderer )
  {
    SDL_DestroyRenderer ( con->m_Renderer );
  }

  if ( con->m_Window )
  {
    SDL_DestroyWindow   ( con->m_Window );
  }
  SDL_Quit();
}



//---------------------------------------------------------------------------------
Render::TexHandle Render::AllocTex( void* data_header, uint32_t layer )
{
  RenderCon* con = &s_RenderCon;
  TextureInfo* tex_info = &con->m_Textures[con->m_TextureHead];

  TextureAsset* tex_asset = (TextureAsset*)data_header;
  int width, height;

  constexpr int sdl_format = STBI_rgb_alpha;
  int orig_format;
  unsigned char* pixel_data = stbi_load_from_memory( (uint8_t*)tex_asset->m_Data, (int)tex_asset->m_DataLength, &width, &height, &orig_format, sdl_format );

  if ( pixel_data == nullptr )
  {
    Log::LogError( "Converting image %s failed: %s\n", tex_asset->m_Name, stbi_failure_reason() );
    return kInvalidTexHandle;
  }

  SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormatFrom( (void*)pixel_data, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32 );
  if ( surf == nullptr )
  {
    Log::LogError( "Creating surface failed for %s: %s", tex_asset->m_Name, SDL_GetError() );
    return kInvalidTexHandle;
  }

  SDL_Texture* sdl_tex = SDL_CreateTextureFromSurface( con->m_Renderer, surf );
  SDL_FreeSurface( surf );
  stbi_image_free( pixel_data );
  if ( sdl_tex == nullptr )
  {
    Log::LogError( "Creating texture from surface %s error: %s", tex_asset->m_Name, SDL_GetError() );
    return kInvalidTexHandle;
  }

  tex_info->m_Asset        = tex_asset;
  tex_info->m_SdlTex       = sdl_tex;
  tex_info->m_Layer        = layer;
  tex_info->m_ImageRect.x  = 0;
  tex_info->m_ImageRect.y  = 0;
  tex_info->m_ImageRect.w  = width;
  tex_info->m_ImageRect.h  = height;

  con->m_TextureCount++;
  TexHandle handle = con->m_TextureHead;

  tex_info->m_Prev             = con->m_LayerTails[ layer ];
  if ( con->m_LayerHeads[ layer ] == kLayerEnd )
  {
    con->m_LayerHeads[ layer ] = con->m_TextureHead;
  }
  else
  {
    con->m_Textures[ tex_info->m_Prev ].m_Next = con->m_TextureHead;
  }
  con->m_LayerTails[ layer ]   = con->m_TextureHead;
  con->m_TextureHead           = tex_info->m_Next;
  tex_info->m_Next             = kLayerEnd;

  return handle;
}

//---------------------------------------------------------------------------------
void Render::FreeTex( TexHandle tex_handle )
{
  RenderCon* con = &s_RenderCon;

  TextureInfo* tex = &con->m_Textures[ tex_handle ];
  if ( tex->m_Prev != kLayerEnd )
  {
    TextureInfo* prev_tex = &con->m_Textures[ tex->m_Prev ];
    prev_tex->m_Next = tex->m_Next;
  }

  if ( tex->m_Next != kLayerEnd )
  {
    TextureInfo* next_tex = &con->m_Textures[ tex->m_Next ];
    next_tex->m_Prev = tex->m_Prev;
  }

  if ( con->m_LayerHeads[ tex->m_Layer ] == tex_handle )
  {
    con->m_LayerHeads[ tex->m_Layer ] = tex->m_Next;
  }

  if ( con->m_LayerTails[ tex->m_Layer ] == tex_handle )
  {
    con->m_LayerTails[ tex->m_Layer ] = tex->m_Next;
  }

  SDL_DestroyTexture ( tex->m_SdlTex );
  AssetLoader::Free  ( tex->m_Asset  );

  tex->Init();

  tex->m_Next = con->m_TextureHead;
  con->m_TextureHead = (uint16_t)tex_handle;
}

//---------------------------------------------------------------------------------
void Render::SetTexPosition( TexHandle tex_handle, Vec2 pos )
{
  RenderCon*   con = &s_RenderCon;
  TextureInfo* tex = &con->m_Textures[ tex_handle ];
  tex->m_Position  = pos;
}

//---------------------------------------------------------------------------------
void Render::SetTexImageRect( TexHandle tex_handle, RectInt rect )
{
  RenderCon*   con = &s_RenderCon;
  TextureInfo* tex = &con->m_Textures[ tex_handle ];
  tex->m_ImageRect = rect;
}

//---------------------------------------------------------------------------------
void Render::SetTexScale( TexHandle tex_handle, Vec2 scale )
{
  RenderCon*   con = &s_RenderCon;
  TextureInfo* tex = &con->m_Textures[ tex_handle ];
  tex->m_Scale     = scale;
}

//---------------------------------------------------------------------------------
void  Render::SetTexScale( TexHandle tex_handle, float scale )
{
  SetTexScale( tex_handle, { scale, scale });
}

//---------------------------------------------------------------------------------
void Render::Draw()
{
  RenderCon* con = &s_RenderCon;

  SDL_RenderClear( con->m_Renderer );
  for ( int32_t i_layer = kMaxLayers - 1; i_layer >= 0; --i_layer )
  {
    uint16_t layer_tex_idx = con->m_LayerHeads[ i_layer ];
    while ( layer_tex_idx != kLayerEnd )
    {
      TextureInfo* tex_info = &con->m_Textures[ layer_tex_idx ];
      SDL_FRect screen_rect;
      screen_rect.x = tex_info->m_Position.x;
      screen_rect.y = tex_info->m_Position.y;
      screen_rect.w = tex_info->m_ImageRect.w * tex_info->m_Scale.x;
      screen_rect.h = tex_info->m_ImageRect.h * tex_info->m_Scale.y;
      
      SDL_RenderCopyF( con->m_Renderer, tex_info->m_SdlTex, (SDL_Rect*)&tex_info->m_ImageRect, &screen_rect );

      layer_tex_idx = tex_info->m_Next;
    }
  }

  SDL_RenderPresent( con->m_Renderer );
}