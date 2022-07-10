#pragma once

#include "engine/point.hpp"

namespace devilution {

using WorldTileCoord = uint8_t;
using WorldTilePosition = PointOf<WorldTileCoord>;

using WorldTileOffset = int8_t;
using WorldTileDisplacement = DisplacementOf<WorldTileOffset>;

} // namespace devilution
