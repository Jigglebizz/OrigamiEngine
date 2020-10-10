#pragma once

#include "Origami/Actor/Actor.h"
#include "Origami/Render/SpriteComponent.h"

class BaseCharacter : public ActorBase
{
  Render::SpriteComponent m_SpriteComponent;

public:
  void Init() override;
  void UpdateLast( float dt );
  void Destroy() override;
};