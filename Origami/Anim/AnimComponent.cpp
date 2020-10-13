#include "Origami/pch.h"
#include "Origami/Anim/AnimComponent.h"

//---------------------------------------------------------------------------------
void Anim::AnimComponent::Init( ActorBase* actor, void* init_props )
{
  UNREFERENCED_PARAMETER( init_props );

  BaseInit( actor );
}

//---------------------------------------------------------------------------------
void Anim::AnimComponent::UpdateMiddle( float dt )
{
  UNREFERENCED_PARAMETER( dt );
}