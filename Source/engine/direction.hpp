#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "utils/attributes.h"

namespace devilution {

enum class Direction : std::uint8_t {
	South,
	SouthWest,
	West,
	NorthWest,
	North,
	NorthEast,
	East,
	SouthEast,
	NoDirection
};

/** Maps from direction to a left turn from the direction. */
DVL_ALWAYS_INLINE constexpr Direction Left(Direction facing)
{
	// Direction left[8] = { Direction::SouthEast, Direction::South, Direction::SouthWest, Direction::West, Direction::NorthWest, Direction::North, Direction::NorthEast, Direction::East };
	return static_cast<Direction>((static_cast<std::underlying_type_t<Direction>>(facing) + 7) % 8);
}

/** Maps from direction to a right turn from the direction. */
DVL_ALWAYS_INLINE constexpr Direction Right(Direction facing)
{
	// Direction right[8] = { Direction::SouthWest, Direction::West, Direction::NorthWest, Direction::North, Direction::NorthEast, Direction::East, Direction::SouthEast, Direction::South };
	return static_cast<Direction>((static_cast<std::underlying_type_t<Direction>>(facing) + 1) % 8);
}

/** Maps from direction to the opposite direction. */
DVL_ALWAYS_INLINE constexpr Direction Opposite(Direction facing)
{
	// Direction opposite[8] = { Direction::North, Direction::NorthEast, Direction::East, Direction::SouthEast, Direction::South, Direction::SouthWest, Direction::West, Direction::NorthWest };
	return static_cast<Direction>((static_cast<std::underlying_type_t<Direction>>(facing) + 4) % 8);
}

std::string_view DirectionToString(Direction direction);

} // namespace devilution
