#pragma once

#include <cstdint>

#include "engine/animationinfo.h"
#include "engine/point.hpp"
#include "engine/world_tile.hpp"

namespace devilution {

class ActorPosition {
public: // TODO: Make private when all direct usage of members variables are removed.
	WorldTilePosition tile;
	/** Future tile position. Set at start of walking animation. */
	WorldTilePosition future;
	/** Tile position of player. Set via network on player input. */
	WorldTilePosition last;
	/** Most recent position in dPlayer. */
	WorldTilePosition old;
	/** Used for referring to position of player when finishing moving one tile (also used to define target coordinates for spells and ranged attacks) */
	WorldTilePosition temp;

	// TODO: Set public here when all direct usage of members variables are removed.
	WorldTilePosition getTile() const { return tile; }
	void setTile(const WorldTilePosition &pos) { tile = pos; }

	WorldTilePosition getFuture() const { return future; }
	void setFuture(const WorldTilePosition &pos) { future = pos; }

	WorldTilePosition getLast() const { return last; }
	void setLast(const WorldTilePosition &pos) { last = pos; }

	WorldTilePosition getOld() const { return old; }
	void setOld(const WorldTilePosition &pos) { old = pos; }

	WorldTilePosition getTemp() const { return temp; }
	void setTemp(const WorldTilePosition &pos) { temp = pos; }

	/** @brief Calculates the offset for the walking animation. */
	DisplacementOf<int8_t> CalculateWalkingOffset(Direction dir, const AnimationInfo &animInfo) const;
	/** @brief Calculates the offset for the walking animation. */
	DisplacementOf<int16_t> CalculateWalkingOffsetShifted4(Direction dir, const AnimationInfo &animInfo) const;
	/** @brief Calculates the offset for the walking animation. */
	DisplacementOf<int16_t> CalculateWalkingOffsetShifted8(Direction dir, const AnimationInfo &animInfo) const;
	/** @brief Returns Pixel velocity while walking. */
	DisplacementOf<int16_t> GetWalkingVelocityShifted4(Direction dir, const AnimationInfo &animInfo) const;
	/** @brief Returns Pixel velocity while walking. */
	DisplacementOf<int16_t> GetWalkingVelocityShifted8(Direction dir, const AnimationInfo &animInfo) const;
};

} // namespace devilution
