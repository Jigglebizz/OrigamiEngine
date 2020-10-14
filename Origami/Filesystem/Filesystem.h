#pragma once

namespace Filesystem
{
  constexpr uint32_t kMaxAssetNameLen = 64;
  constexpr uint32_t kMaxPathLen = 128;

  //---------------------------------------------------------------------------------
  const char* GetAssetsSourcePath();
  const char* GetAssetsBuiltPath();

  const bool ENGINE_API FileExists( const char* path );
}