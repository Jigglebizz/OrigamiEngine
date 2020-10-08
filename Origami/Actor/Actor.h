#pragma once

#include "Origami/Math/Primitives.h"

//---------------------------------------------------------------------------------
class ActorBase
{
protected:
  //---------------------------------------------------------------------------------
  Vec2 m_Position;
public:
  //---------------------------------------------------------------------------------
  virtual void Init();
  virtual void UpdateFirst ( float dt );
  virtual void UpdateMiddle( float dt );
  virtual void UpdateLast  ( float dt );
  virtual void Destroy();

  inline  Vec2 GetPosition() const;
};

Vec2 ActorBase::GetPosition() const { return m_Position; }