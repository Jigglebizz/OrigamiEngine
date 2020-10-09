#include "Origami/pch.h"
#include "BaseCharacter.h"

void BaseCharacter::Init()
{
  Render::SpriteComponentInitProperties sprite_init_props;
  MemZero( &sprite_init_props, sizeof( sprite_init_props ) );
  strcpy_s( sprite_init_props.m_TextureAssetPath, "megaman.png" );
  sprite_init_props.m_FrameSize.x = 50;
  sprite_init_props.m_FrameSize.y = 49;
  sprite_init_props.m_FrameStride = 8;
  sprite_init_props.m_Layer = 0;

  m_SpriteComponent.Init( this, &sprite_init_props );
}

void BaseCharacter::Destroy()
{
  m_SpriteComponent.Destroy();
}

void BaseCharacter::UpdateFirst( float dt )
{
  m_SpriteComponent.UpdateFirst( dt );
}