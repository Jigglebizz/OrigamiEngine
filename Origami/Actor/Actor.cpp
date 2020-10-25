#include "Origami/pch.h"
#include "Actor.h"

//---------------------------------------------------------------------------------
uint64_t ActorBase::s_IdCounter = 0;

//---------------------------------------------------------------------------------
void ActorBase::BaseInit( const char* name )
{
  m_ComponentCount = 0;
  MemZero( &m_Components, sizeof( m_Components ) );
  strncpy_s( m_Name, name, kMaxNameLen );
  m_Id = s_IdCounter++;
}

void ActorBase::BaseDestroy( )
{
  for ( uint32_t i_component = 0; i_component < m_ComponentCount; ++i_component )
  {
    Actor::g_ActorCon.m_ComponentHeap.Free( m_Components[ i_component ].m_Component );
  }
}

//---------------------------------------------------------------------------------
const char* ActorBase::GetName() const
{
  return m_Name;
}

//---------------------------------------------------------------------------------
void ActorBase::Init( )
{
  BaseInit( "<UNKNOWN>" );
}

//---------------------------------------------------------------------------------
void ActorBase::UpdateFirst( float dt )
{
  for ( uint32_t i_component = 0; i_component < m_ComponentCount; ++i_component )
  {
    m_Components[ i_component ].m_Component->UpdateFirst( dt );
  }
}

//---------------------------------------------------------------------------------
void ActorBase::UpdateMiddle( float dt )
{
  for ( uint32_t i_component = 0; i_component < m_ComponentCount; ++i_component )
  {
    m_Components[ i_component ].m_Component->UpdateMiddle( dt );
  }
}

//---------------------------------------------------------------------------------
void ActorBase::UpdateLast( float dt )
{
  for ( uint32_t i_component = 0; i_component < m_ComponentCount; ++i_component )
  {
    m_Components[ i_component ].m_Component->UpdateLast( dt );
  }
}

//---------------------------------------------------------------------------------
void ActorBase::Destroy()
{
  BaseDestroy();
}