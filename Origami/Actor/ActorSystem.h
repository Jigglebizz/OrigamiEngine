#pragma once

#include "Origami/Actor/Actor.h"

namespace Actor
{
  //---------------------------------------------------------------------------------
  void            Init();
  void            UpdateFirst  ( float dt );
  void            UpdateMiddle ( float dt );
  void            UpdateLast   ( float dt );

  void ENGINE_API AddActor     ( ActorBase* actor );

  //---------------------------------------------------------------------------------
  static constexpr uint32_t kMaxActors = 1024;

  struct ActorInfo
  {
    uint64_t   m_ActorId;
    ActorBase* m_Actor;
  };

  static uint32_t     s_ActorInfoCount;
  static ActorInfo    s_ActorInfos[ kMaxActors ];
}