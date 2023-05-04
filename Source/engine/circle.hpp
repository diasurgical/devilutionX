#pragma once

#include "engine/displacement.hpp"
#include "engine/point.hpp"

namespace devilution {

struct Circle {
	Point position;
	int radius;

	constexpr bool contains(Point point) const
	{
		Displacement diff = point - position;
		int x = diff.deltaX;
		int y = diff.deltaY;
		return x * x + y * y < radius * radius;
	}
};

} // namespace devilution
