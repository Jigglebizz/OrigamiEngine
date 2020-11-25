#pragma once

#include <stdint.h>
#include "Origami/Util/Hash.h"

struct AssetVersion
{
  static constexpr uint32_t kMaxDescriptionLen = 64;

  const char* m_Description;
  uint32_t    m_Hash;

  constexpr AssetVersion( const char* description )
    : m_Description( description )
    , m_Hash       ( Crc32( description ) )
  {
    ASSERT_MSG( StrLen( description ) < ( kMaxDescriptionLen - 1 ), "Asset version description is too long" );
  }
};

constexpr AssetVersion kAssetVersionActor( "1.0 - First actor version" );
