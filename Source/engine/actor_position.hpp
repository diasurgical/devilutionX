#pragma once

#include "engine/point.hpp"

namespace devilution {

struct ActorPosition {
	Point tile;
	/** Future tile position. Set at start of walking animation. */
	Point future;
	/** Tile position of player. Set via network on player input. */
	Point last;
	/** Most recent position in dPlayer. */
	Point old;
	/** Pixel offset from tile. */
	Point offset;
	/** Same as offset but contains the value in a higher range */
	Point offset2;
	/** Pixel velocity while walking. Indirectly applied to offset via _pvar6/7 */
	Point velocity;
	/** Used for referring to position of player when finishing moving one tile (also used to define target coordinates for spells and ranged attacks) */
	Point temp;
};

} // namespace devilution
