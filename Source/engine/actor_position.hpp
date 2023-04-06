#pragma once

#include "engine/animationinfo.h"
#include "engine/point.hpp"
#include "engine/world_tile.hpp"

namespace devilution {

struct ActorPosition {
	WorldTilePosition tile;
	/** Future tile position. Set at start of walking animation. */
	WorldTilePosition future;
	/** Tile position of player. Set via network on player input. */
	WorldTilePosition last;
	/** Most recent position in dPlayer. */
	WorldTilePosition old;
	/** Used for referring to position of player when finishing moving one tile (also used to define target coordinates for spells and ranged attacks) */
	WorldTilePosition temp;

	/** @brief Calculates the offset for the walking animation. */
	DisplacementOf<int8_t> CalculateWalkingOffset(Direction dir, const AnimationInfo &animInfo, bool pendingProcessAnimation = false) const;
	/** @brief Calculates the offset for the walking animation. */
	DisplacementOf<int16_t> CalculateWalkingOffsetShifted4(Direction dir, const AnimationInfo &animInfo, bool pendingProcessAnimation = false) const;
	/** @brief Calculates the offset for the walking animation. */
	DisplacementOf<int16_t> CalculateWalkingOffsetShifted8(Direction dir, const AnimationInfo &animInfo, bool pendingProcessAnimation = false) const;
	/** @brief Returns Pixel velocity while walking. */
	DisplacementOf<int16_t> GetWalkingVelocityShifted4(Direction dir, const AnimationInfo &animInfo) const;
	/** @brief Returns Pixel velocity while walking. */
	DisplacementOf<int16_t> GetWalkingVelocityShifted8(Direction dir, const AnimationInfo &animInfo) const;
};

} // namespace devilution
