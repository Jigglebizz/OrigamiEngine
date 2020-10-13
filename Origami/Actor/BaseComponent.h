#pragma once

class ActorBase;

class BaseComponent
{
protected:
  void BaseInit( ActorBase* actor );
public:
  using ComponentId = uint32_t;

  virtual void ENGINE_API Init( ActorBase* actor, void* init_params = nullptr );
  virtual void ENGINE_API UpdateFirst  ( float dt );
  virtual void ENGINE_API UpdateMiddle ( float dt );
  virtual void ENGINE_API UpdateLast   ( float dt );

  ActorBase* GetActor();
private:
  ActorBase* m_Actor;
};

#define DECLARE_COMPONENT_START( Name )     \
class Name : public BaseComponent           \
{                                           \
public:                                     \
  static constexpr const char* GetName()    \
  {                                         \
    return #Name;                           \
  }                                         \
  static constexpr ComponentId GetId()      \
  {                                         \
    return (ComponentId)Crc32( GetName() ); \
  }                                         \
private:

#define DECLARE_COMPONENT_END \
};