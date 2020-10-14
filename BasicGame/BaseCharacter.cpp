#include "Origami/pch.h"
#include "BaseCharacter.h"

#include "TestMovementComponent.h"

void BaseCharacter::Init()
{
  Render::SpriteComponentInitProperties sprite_init_props;
  MemZero( &sprite_init_props, sizeof( sprite_init_props ) );
  strcpy_s( sprite_init_props.m_TextureAssetPath, "texture\\megaman.png" );
  sprite_init_props.m_FrameSize.x = 50;
  sprite_init_props.m_FrameSize.y = 49;
  sprite_init_props.m_FrameStride = 8;
  sprite_init_props.m_Layer = 0;

  AddComponent<Render::SpriteComponent>( &sprite_init_props );
  AddComponent<TestMovementComponent>();
  //AddComponent<TestMovementComponent>();
}