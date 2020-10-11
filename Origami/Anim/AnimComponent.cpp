#include "Origami/pch.h"
#include "Origami/Anim/AnimComponent.h"

//---------------------------------------------------------------------------------
void Anim::AnimComponent::Init( ActorBase* actor, AnimComponentInitProperties* init_props )
{
  UNREFERENCED_PARAMETER( init_props );

  BaseComponent::Init( actor );
}

//---------------------------------------------------------------------------------
void Anim::AnimComponent::UpdateMiddle( float dt )
{
  UNREFERENCED_PARAMETER( dt );
}