#pragma once

#include <stdint.h>
#include "Origami/Util/Hash.h"

using AssetVersion = uint32_t;

constexpr AssetVersion kAssetVersionActor = Crc32( "1.0 - First actor version" );
