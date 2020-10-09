#pragma once

#include "Origami/Asset/AssetLoader.h"
#include "Origami/Math/Primitives.h"
#include "Origami/Render/Render.h"

#include "Origami/Actor/BaseComponent.h"

namespace Render
{
  //---------------------------------------------------------------------------------
  struct SpriteComponentInitProperties
  {
    char     m_TextureAssetPath[ AssetLoader::kMaxAssetNameLen ];
    int      m_Layer;
    Vec2Int  m_FrameSize;
    uint8_t  m_FrameStride;
  };

  //---------------------------------------------------------------------------------
  class SpriteComponent : public BaseComponent
  {
  public:
    void Init( ActorBase* actor, SpriteComponentInitProperties* init_props );
    void Destroy();

    void UpdateLast( float dt );

  private:
    Render::TexHandle m_TexHandle;
    uint32_t          m_FrameIdx;
    Vec2Int           m_FrameSize;
    uint8_t           m_FrameStride;

    RectInt           MakeImageRect() const;
  };
}