#include "Origami/pch.h"
#include "Origami/Actor/BaseComponent.h"

//---------------------------------------------------------------------------------
void BaseComponent::BaseInit( ActorBase* actor )
{
  m_Actor = actor;
}

//---------------------------------------------------------------------------------
void BaseComponent::Init( ActorBase* actor, void* init_params )
{
  UNREFERENCED_PARAMETER( init_params );
  BaseInit( actor );
}

//---------------------------------------------------------------------------------
ActorBase* BaseComponent::GetActor()
{
  return m_Actor;
}

//---------------------------------------------------------------------------------
void BaseComponent::UpdateFirst( float dt )
{
  UNREFERENCED_PARAMETER( dt );
}

//---------------------------------------------------------------------------------
void BaseComponent::UpdateMiddle( float dt )
{
  UNREFERENCED_PARAMETER( dt );
}

//---------------------------------------------------------------------------------
void BaseComponent::UpdateLast( float dt )
{
  UNREFERENCED_PARAMETER( dt );
}