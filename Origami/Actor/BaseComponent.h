#pragma once

#include "Origami/Actor/Actor.h"

class BaseComponent
{
public:
  void       Init( ActorBase* actor );
  ActorBase* GetActor();

private:
  ActorBase* m_Actor;
};