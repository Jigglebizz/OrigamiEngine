#pragma once

#include "Origami/Asset/Asset.h"

namespace AssetServer
{
  enum ServerStatus : uint8_t
  {
    kStatusDead         = 0x00,
    kStatusInitialized  = 0x01,

  };

  static constexpr uint16_t kTcpPort              = 2255;
  static constexpr uint8_t  kMaxClientConnections = 4;

  void         ENGINE_API Init();
  void         ENGINE_API Destroy();
  AssetId      ENGINE_API GetNextRequest();
  ServerStatus ENGINE_API GetStatus();
};