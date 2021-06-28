#pragma once

#include "engine/point.hpp"
#include "engine/size.hpp"

namespace devilution {

struct Rectangle {
	Point position;
	Size size;

	constexpr bool Contains(Point point) const
	{
		return point.x >= this->position.x
		    && point.x <= (this->position.x + this->size.width)
		    && point.y >= this->position.y
		    && point.y <= (this->position.y + this->size.height);
	}

	/**
	 * @brief Computes the center of this rectangle in integer coordinates. Values are truncated towards zero.
	 */
	constexpr Point Center() const
	{
		return position + (size / 2);
	}
};

} // namespace devilution
