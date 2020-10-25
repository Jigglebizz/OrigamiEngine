#pragma once

class ActorBase;

namespace Actor
{
  //---------------------------------------------------------------------------------
  static constexpr uint32_t kComponentHeapSize = 32 * 1024 * 1024; // 32 MB

  //---------------------------------------------------------------------------------
  void            Init();
  void            Destroy();
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

  struct ActorCon
  {
    char         m_ComponentHeapBacking[ kComponentHeapSize ];
    MemAllocHeap m_ComponentHeap;

    uint32_t     m_ActorInfoCount;
    ActorInfo    m_ActorInfos[ kMaxActors ];
  };

  static ActorCon g_ActorCon;
}