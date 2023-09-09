#pragma once

#include "engine/point.hpp"
#include "engine/size.hpp"
#include "utils/attributes.h"

namespace devilution {

template <typename CoordT, typename SizeT = CoordT>
struct RectangleOf {
	PointOf<CoordT> position;
	SizeOf<SizeT> size;

	RectangleOf() = default;

	DVL_ALWAYS_INLINE constexpr RectangleOf(PointOf<CoordT> position, SizeOf<SizeT> size)
	    : position(position)
	    , size(size)
	{
	}

	/**
	 * @brief Constructs a rectangle centered on the given point and including all tiles within the given radius.
	 *
	 * The resulting rectangle will be square with an odd size equal to 2*radius + 1.
	 *
	 * @param center center point of the target rectangle
	 * @param radius a non-negative value indicating how many tiles to include around the center
	 */
	DVL_ALWAYS_INLINE explicit constexpr RectangleOf(PointOf<CoordT> center, SizeT radius)
	    : position(center - DisplacementOf<SizeT> { radius })
	    , size(static_cast<SizeT>(2 * radius + 1))
	{
	}

	/**
	 * @brief Whether this rectangle contains the given point.
	 * Works correctly even if the point uses a different underlying numeric type
	 */
	template <typename PointCoordT>
	DVL_ALWAYS_INLINE constexpr bool contains(PointOf<PointCoordT> point) const
	{
		return contains(point.x, point.y);
	}

	template <typename T>
	constexpr bool contains(T x, T y) const
	{
		return x >= this->position.x
		    && x < (this->position.x + this->size.width)
		    && y >= this->position.y
		    && y < (this->position.y + this->size.height);
	}

	/**
	 * @brief Computes the center of this rectangle in integer coordinates. Values are truncated towards zero.
	 */
	constexpr PointOf<CoordT> Center() const
	{
		return position + DisplacementOf<SizeT>(size / 2);
	}

	/**
	 * @brief Returns a rectangle with all sides shrunk according to the given displacement
	 *
	 * Effectively moves the left/right edges in by deltaX, and the top/bottom edges in by deltaY
	 */
	constexpr RectangleOf<CoordT, SizeT> inset(DisplacementOf<SizeT> factor) const
	{
		return {
			position + factor,
			SizeOf<SizeT>(size.width - factor.deltaX * 2, size.height - factor.deltaY * 2)
		};
	}
};

using Rectangle = RectangleOf<int, int>;

} // namespace devilution
