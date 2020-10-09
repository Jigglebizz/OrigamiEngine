#include "Origami/pch.h"
#include "Origami/Actor/BaseComponent.h"

//---------------------------------------------------------------------------------
void BaseComponent::Init( ActorBase* actor )
{
  m_Actor = actor;
}

//---------------------------------------------------------------------------------
ActorBase* BaseComponent::GetActor()
{
  return m_Actor;
}