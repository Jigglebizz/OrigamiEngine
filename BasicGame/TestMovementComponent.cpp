#include "Origami/pch.h"
#include "TestMovementComponent.h"

#include "Origami/Actor/Actor.h"

void TestMovementComponent::UpdateMiddle( float dt )
{
  Vec2 pos = GetActor()->GetPosition();
  pos.x += dt * 0.1f;
  pos.y += dt * 0.12f;

  if ( pos.x > 400 )
  {
    pos.x -= 400;
  }

  if ( pos.y > 300 )
  {
    pos.y -= 300;
  }

  GetActor()->SetPosition( &pos );
}