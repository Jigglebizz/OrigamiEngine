#pragma once

#include "Origami/Actor/Actor.h"

class Command
{
public:
  virtual ~Command() {}
  virtual void execute( ActorBase* actor ) = 0;
};