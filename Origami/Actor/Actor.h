#pragma once

#include "Origami/Math/Primitives.h"
#include "Origami/Actor/BaseComponent.h"

#include "Origami/Util/Search.h"
#include "Origami/Util/Sort.h"

//---------------------------------------------------------------------------------
class ActorBase
{
private:
  struct ComponentInfo
  {
    uint64_t       m_ComponentTypeId;
    BaseComponent* m_Component;
  };

  static constexpr uint32_t kMaxComponents = 32;
  static constexpr uint32_t kMaxNameLen    = 64;
  static           uint64_t s_IdCounter;

  uint32_t      m_ComponentCount;
  ComponentInfo m_Components[ kMaxComponents ];

  char          m_Name[ kMaxNameLen ];
  uint64_t      m_Id;
protected:
  //---------------------------------------------------------------------------------
  Vec2          m_Position;

  //---------------------------------------------------------------------------------
          void     BaseInit    ( const char* name );
          void     BaseDestroy ();
public:
  //---------------------------------------------------------------------------------
  const   char*    GetName     () const;
  inline  uint64_t GetId       () const;

  virtual void     Init        ( );
          void     UpdateFirst ( float dt );
          void     UpdateMiddle( float dt );
          void     UpdateLast  ( float dt );
  virtual void     Destroy     ();
                   
  inline  Vec2     GetPosition () const;
  inline  void     SetPosition ( const Vec2* pos );

  template <class ComponentClass>
  void ENGINE_API AddComponent( void* init_params = nullptr );
};

//---------------------------------------------------------------------------------
Vec2 ActorBase::GetPosition() const
{
  return m_Position;
}

//---------------------------------------------------------------------------------
void ActorBase::SetPosition( const Vec2* pos )
{
  m_Position = *pos;
}

//---------------------------------------------------------------------------------
uint64_t ActorBase::GetId() const
{
  return m_Id;
}

//---------------------------------------------------------------------------------
template <class ComponentClass>
void ActorBase::AddComponent( void* init_params )
{
  // TODO: Create on component heap
  ASSERT_MSG( m_ComponentCount < kMaxComponents, "Attempting to add too many components to actor!" );
  uint64_t id = ComponentClass::GetId();

  size_t existing_component = BinarySearch64( id, &m_Components, sizeof( ComponentInfo ), m_ComponentCount );
  ASSERT_MSG( existing_component == (uint64_t)-1, "Attempting to add duplicate component to actor!" );

  ComponentInfo* new_info = &m_Components[ m_ComponentCount++ ];
  new_info->m_ComponentTypeId = id;

  // TODO: create from component heap
  new_info->m_Component = new ComponentClass();
  new_info->m_Component->Init( this, init_params );

  QuickSort64( m_Components, sizeof( ComponentInfo ), m_ComponentCount );
}