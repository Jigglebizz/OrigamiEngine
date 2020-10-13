#pragma once

#include "Origami/Actor/BaseComponent.h"

namespace Anim
{
  DECLARE_COMPONENT_START( AnimComponent )

    void Init          ( ActorBase* actor, void* init_props ) override;
    void UpdateMiddle  ( float dt )                           override;
  DECLARE_COMPONENT_END
}