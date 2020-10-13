#pragma once

#include "SDL.h"
#include "Origami/Math/Primitives.h"

namespace Render
{
  //---------------------------------------------------------------------------------
  void ENGINE_API Init( const char* window_title );
  void ENGINE_API Destroy();
  void ENGINE_API Draw();

  using TexHandle = uint64_t;
  static constexpr TexHandle kInvalidTexHandle = (TexHandle)-1;

  //---------------------------------------------------------------------------------
  TexHandle ENGINE_API AllocTex        ( void* data_header, uint32_t layer = 0);
  void      ENGINE_API FreeTex         ( TexHandle handle );

  void      ENGINE_API SetTexPosition  ( TexHandle tex_handle, Vec2 pos     );
  void      ENGINE_API SetTexImageRect ( TexHandle tex_handle, RectInt rect );
  void      ENGINE_API SetTexScale     ( TexHandle tex_handle, Vec2 scale   );
  void      ENGINE_API SetTexScale     ( TexHandle tex_handle, float scale  );

  //---------------------------------------------------------------------------------
  struct TextureAsset
  {
    char     m_Name[ 64 ];
    uint64_t m_DataLength;
    char*    m_Data;
  };

  //---------------------------------------------------------------------------------
  static constexpr uint16_t kMaxTextures = 1024;
  static constexpr uint16_t kMaxLayers   = 32;

  struct TextureInfo
  {
    SDL_Texture*  m_SdlTex;
    TextureAsset* m_Asset;
    int           m_Layer;
    RectInt       m_ImageRect;
    Vec2          m_Position;
    Vec2          m_Scale;

    // Layers are arranged as linked lists
    uint16_t m_Next;
    uint16_t m_Prev;

    void Init();
    void Free();
  };

  //---------------------------------------------------------------------------------
  enum TextureConstants : uint16_t
  {
    kLayerHead   = 0xffff,
    kLayerEnd    = 0xffff,
    kTexturesEnd = 0xffff,
  };
  
  struct RenderCon
  {
    SDL_Window*   m_Window;
    SDL_Renderer* m_Renderer;

    uint16_t      m_TextureCount;
    uint16_t      m_TextureHead;
    uint16_t      m_LayerHeads [ kMaxLayers ];
    uint16_t      m_LayerTails [ kMaxLayers ];
    TextureInfo   m_Textures   [ kMaxTextures ];
  };

  //---------------------------------------------------------------------------------
  void InitRenderCon();

  static RenderCon s_RenderCon;
}