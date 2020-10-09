#include "Origami/pch.h"
#include "Origami/Render/SpriteComponent.h"

//---------------------------------------------------------------------------------
void Render::SpriteComponent::Init( ActorBase* actor, SpriteComponentInitProperties* init_props )
{
  BaseComponent::Init( actor );

  m_TexHandle = 0;
  m_FrameIdx  = 0;

  m_FrameSize   = init_props->m_FrameSize;
  m_FrameStride = init_props->m_FrameStride;

  char* sprite_data = AssetLoader::Load( init_props->m_TextureAssetPath );
  ASSERT_MSG( sprite_data, "Sprite data was not loaded" );

  m_TexHandle = Render::AllocTex( sprite_data, init_props->m_Layer );

  ASSERT_MSG( m_TexHandle != kInvalidTexHandle, "Sprite got inalid tex handle" );
  Render::SetTexImageRect( m_TexHandle, MakeImageRect() );
}

//---------------------------------------------------------------------------------
void Render::SpriteComponent::Destroy()
{
  if ( m_TexHandle != kInvalidTexHandle )
  {
    Render::FreeTex( m_TexHandle );
  }
}

//---------------------------------------------------------------------------------
void Render::SpriteComponent::UpdateLast( float dt )
{
  UNREFERENCED_PARAMETER( dt );
  Render::SetTexImageRect( m_TexHandle, MakeImageRect() );
  Render::SetTexPosition(  m_TexHandle, GetActor()->GetPosition() );
}

//---------------------------------------------------------------------------------
RectInt Render::SpriteComponent::MakeImageRect() const
{
  RectInt rect;

  rect.w = m_FrameSize.x;
  rect.h = m_FrameSize.y;

  rect.x = ( m_FrameIdx % m_FrameStride ) * m_FrameSize.x;
  rect.y = ( m_FrameIdx / m_FrameStride ) * m_FrameSize.y;

  return rect;
}