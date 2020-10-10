#pragma once

#include "Origami/Actor/BaseComponent.h"

namespace Anim
{
  struct AnimComponent : public BaseComponent
  {
    struct AnimComponentInitProperties
    {

    };

    void Init( ActorBase* actor, AnimComponentInitProperties* init_props );
    void UpdateMiddle( float dt );
  };
}