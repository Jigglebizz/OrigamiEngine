#pragma once

class ActorBase;

class BaseComponent
{
protected:
  void BaseInit( ActorBase* actor );
public:
  virtual void ENGINE_API Init( ActorBase* actor, void* init_params = nullptr );
  virtual void ENGINE_API UpdateFirst  ( float dt );
  virtual void ENGINE_API UpdateMiddle ( float dt );
  virtual void ENGINE_API UpdateLast   ( float dt );

  ActorBase* GetActor();
private:
  ActorBase* m_Actor;
};

#define DECLARE_COMPONENT_START( Name ) \
class Name : public BaseComponent       \
{                                       \
public:                                 \
  static const char* GetName()          \
  {                                     \
    return "Name";                      \
  }                                     \
  static uint64_t GetId()               \
  {                                     \
    return (uint64_t)&GetId;            \
  }                                     \
private:

#define DECLARE_COMPONENT_END \
};