#pragma once

namespace Input
{
  void Init();
  void EventPump();
  void Update();

  struct ButtonEvent
  {
    enum Flags : uint8_t
    {
      kFlagsSourceMouse    = 0x01,
      kFlagsSourceJoy      = 0x02,
      kFlagsSourceGamepad  = 0x04,
      kFlagsSourceKeyboard = 0x08,

      kFlagsStateDown      = 0x10,
      kFlagsStateUp        = 0x20,
    };

    uint32_t m_PlayerId;
    uint32_t m_ButtonId;
    uint8_t  m_Flags;
  };

  struct InputCon
  {
    static constexpr uint8_t kMaxButtonEventsPerFrame = 8; // TODO: Measure what's reasonable here

    uint8_t     m_ButtonEventCount;
    ButtonEvent m_ButtonEventQueue[ kMaxButtonEventsPerFrame ];
  };

  static InputCon s_InputCon;
}