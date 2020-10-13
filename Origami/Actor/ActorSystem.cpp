#include "Origami/pch.h"
#include "Origami/Actor/ActorSystem.h"

//---------------------------------------------------------------------------------
void Actor::Init()
{
  s_ActorInfoCount = 0;
  MemZero( &s_ActorInfos, sizeof( s_ActorInfos ) );
}

//---------------------------------------------------------------------------------
void Actor::UpdateFirst( float dt )
{
  for ( uint32_t i_actor = 0; i_actor < s_ActorInfoCount; ++i_actor )
  {
    s_ActorInfos[ i_actor ].m_Actor->UpdateFirst( dt );
  }
}

//---------------------------------------------------------------------------------
void Actor::UpdateMiddle( float dt )
{
  for ( uint32_t i_actor = 0; i_actor < s_ActorInfoCount; ++i_actor )
  {
    s_ActorInfos[ i_actor ].m_Actor->UpdateMiddle( dt );
  }
}

//---------------------------------------------------------------------------------
void Actor::UpdateLast( float dt )
{
  for ( uint32_t i_actor = 0; i_actor < s_ActorInfoCount; ++i_actor )
  {
    s_ActorInfos[ i_actor ].m_Actor->UpdateLast( dt );
  }
}

//---------------------------------------------------------------------------------
void Actor::AddActor( ActorBase* actor )
{
  ASSERT_MSG( s_ActorInfoCount < kMaxActors, "Attempting to add too many actors!" );

  ActorInfo* actor_info = &s_ActorInfos[ s_ActorInfoCount++ ];
  actor_info->m_Actor = actor;
  actor_info->m_ActorId = actor->GetId();

  QuickSort64( s_ActorInfos, sizeof( ActorInfo ), s_ActorInfoCount );

  actor_info->m_Actor->Init(); // TODO: move to system's loop?
}