#pragma once

#include "Origami/Actor/BaseComponent.h"

DECLARE_COMPONENT_START( TestMovementComponent )
public:
  void UpdateMiddle( float dt ) override;
DECLARE_COMPONENT_END