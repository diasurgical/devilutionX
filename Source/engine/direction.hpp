#pragma once

#include <cstdint>

namespace devilution {

enum Direction : std::uint8_t {
	DIR_S,
	DIR_SW,
	DIR_W,
	DIR_NW,
	DIR_N,
	DIR_NE,
	DIR_E,
	DIR_SE,
};

} // namespace devilution
