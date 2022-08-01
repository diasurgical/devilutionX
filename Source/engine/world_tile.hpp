#pragma once

#include "engine/point.hpp"

namespace devilution {

using WorldTileCoord = uint8_t;
using WorldTilePosition = PointOf<WorldTileCoord>;

using WorldTileOffset = int8_t;
using WorldTileDisplacement = DisplacementOf<WorldTileOffset>;

} // namespace devilution

namespace std {

/**
 * @brief Allows using WorldTilePosition as a map key for contexts where we want to lookup an entity by physical dungeon location
 */
template <>
struct hash<devilution::WorldTilePosition> {
	size_t operator()(const devilution::WorldTilePosition &position) const noexcept
	{
		return static_cast<size_t>(position.x) << 8 | position.y;
	}
};

} // namespace std
