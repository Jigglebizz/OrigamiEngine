#include "Origami/pch.h"
#include "Origami/Actor/ActorSystem.h"

#include "Origami/Actor/Actor.h"
#include "Origami/Util/Log.h"

//---------------------------------------------------------------------------------
Actor::ActorCon Actor::g_ActorCon;

//---------------------------------------------------------------------------------
void Actor::Init()
{
  ActorCon* con = &g_ActorCon;

  con->m_ActorInfoCount = 0;
  MemZero( &con->m_ActorInfos, sizeof( con->m_ActorInfos ) );

  con->m_ComponentHeap.InitWithBacking( con->m_ComponentHeapBacking, sizeof( con->m_ComponentHeapBacking ), "Component Heap" );
  Log::LogInfo("Component heap contains: \n");

  char heap_bytes_str[ sizeof( con->m_ComponentHeap ) * 4 ];
  uint32_t offset = 0;
  heap_bytes_str[ offset++ ] = '\n';

  for ( uint32_t i_byte = 0; i_byte < sizeof( con->m_ComponentHeap ); ++i_byte )
  {
    char current_char = *((char*)&con->m_ComponentHeap + i_byte);

    snprintf( heap_bytes_str + offset, sizeof( heap_bytes_str ), " %02X", current_char );
    offset += 3;

    if ( ( i_byte + 1 ) % 8 == 0 && i_byte != sizeof( con->m_ComponentHeap) -1 )
    {
      heap_bytes_str[ offset ] = '\n';
      offset++;
    }
  }

  Log::LogInfo( "%s", heap_bytes_str );
}

//---------------------------------------------------------------------------------
void Actor::Destroy()
{
  g_ActorCon.m_ComponentHeap.Destroy();
}

//---------------------------------------------------------------------------------
void Actor::UpdateFirst( float dt )
{
  for ( uint32_t i_actor = 0; i_actor < g_ActorCon.m_ActorInfoCount; ++i_actor )
  {
    g_ActorCon.m_ActorInfos[ i_actor ].m_Actor->UpdateFirst( dt );
  }
}

//---------------------------------------------------------------------------------
void Actor::UpdateMiddle( float dt )
{
  for ( uint32_t i_actor = 0; i_actor < g_ActorCon.m_ActorInfoCount; ++i_actor )
  {
    g_ActorCon.m_ActorInfos[ i_actor ].m_Actor->UpdateMiddle( dt );
  }
}

//---------------------------------------------------------------------------------
void Actor::UpdateLast( float dt )
{
  for ( uint32_t i_actor = 0; i_actor < g_ActorCon.m_ActorInfoCount; ++i_actor )
  {
    g_ActorCon.m_ActorInfos[ i_actor ].m_Actor->UpdateLast( dt );
  }
}

//---------------------------------------------------------------------------------
void Actor::AddActor( ActorBase* actor )
{
  ActorCon* con = &g_ActorCon;

  ASSERT_MSG( con->m_ActorInfoCount < kMaxActors, "Attempting to add too many actors!" );

  ActorInfo* actor_info = &con->m_ActorInfos[ con->m_ActorInfoCount++ ];
  actor_info->m_Actor = actor;
  actor_info->m_ActorId = actor->GetId();

  QuickSort64( con->m_ActorInfos, sizeof( ActorInfo ), con->m_ActorInfoCount );

  actor_info->m_Actor->Init(); // TODO: move to system's loop?
}