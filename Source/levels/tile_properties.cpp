#include "levels/tile_properties.hpp"

#include "engine/direction.hpp"
#include "engine/path.h"
#include "engine/point.hpp"
#include "gendung.h"
#include "objects.h"

namespace devilution {

bool IsTileNotSolid(Point position)
{
	if (!InDungeonBounds(position)) {
		return false;
	}

	return !TileHasAny(position, TileProperties::Solid);
}

bool IsTileSolid(Point position)
{
	if (!InDungeonBounds(position)) {
		return false;
	}

	return TileHasAny(position, TileProperties::Solid);
}

bool IsTileWalkable(Point position, bool ignoreDoors)
{
	Object *object = FindObjectAtPosition(position);
	if (object != nullptr) {
		if (ignoreDoors && object->isDoor()) {
			return true;
		}
		if (object->_oSolidFlag) {
			return false;
		}
	}

	return IsTileNotSolid(position);
}

bool IsTileOccupied(Point position)
{
	if (!InDungeonBounds(position)) {
		return true; // OOB positions are considered occupied.
	}

	if (IsTileSolid(position)) {
		return true;
	}
	if (dMonster[position.x][position.y] != 0) {
		return true;
	}
	if (dPlayer[position.x][position.y] != 0) {
		return true;
	}
	if (IsObjectAtPosition(position)) {
		return true;
	}

	return false;
}

bool CanStep(Point startPosition, Point destinationPosition)
{
	// These checks are written as if working backwards from the destination to the source, given
	// both tiles are expected to be adjacent this doesn't matter beyond being a bit confusing
	bool rv = true;
	switch (GetPathDirection(startPosition, destinationPosition)) {
	case 5: // Stepping north
		rv = IsTileNotSolid(destinationPosition + Direction::SouthWest) && IsTileNotSolid(destinationPosition + Direction::SouthEast);
		break;
	case 6: // Stepping east
		rv = IsTileNotSolid(destinationPosition + Direction::SouthWest) && IsTileNotSolid(destinationPosition + Direction::NorthWest);
		break;
	case 7: // Stepping south
		rv = IsTileNotSolid(destinationPosition + Direction::NorthEast) && IsTileNotSolid(destinationPosition + Direction::NorthWest);
		break;
	case 8: // Stepping west
		rv = IsTileNotSolid(destinationPosition + Direction::SouthEast) && IsTileNotSolid(destinationPosition + Direction::NorthEast);
		break;
	}
	return rv;
}

} // namespace devilution
