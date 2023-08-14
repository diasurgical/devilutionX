#include "engine/direction.hpp"

namespace devilution {

std::string_view DirectionToString(Direction direction)
{
	switch (direction) {
	case Direction::South:
		return "South";
	case Direction::SouthWest:
		return "SouthWest";
	case Direction::West:
		return "West";
	case Direction::NorthWest:
		return "NorthWest";
	case Direction::North:
		return "North";
	case Direction::NorthEast:
		return "NorthEast";
	case Direction::East:
		return "East";
	case Direction::SouthEast:
		return "SouthEast";
	case Direction::NoDirection:
		return "";
	}
	return "Invalid";
}

} // namespace devilution
