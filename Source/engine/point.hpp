#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>
#ifdef BUILD_TESTING
#include <ostream>
#endif

#include "engine/direction.hpp"
#include "engine/displacement.hpp"
#include "utils/attributes.h"

namespace devilution {

template <typename CoordT>
struct PointOf;

using Point = PointOf<int>;

template <typename PointCoordT, typename OtherPointCoordT>
constexpr DisplacementOf<PointCoordT> operator-(PointOf<PointCoordT> a, PointOf<OtherPointCoordT> b);

template <typename CoordT>
struct PointOf {
	CoordT x;
	CoordT y;

	PointOf() = default;

	template <typename PointCoordT>
	DVL_ALWAYS_INLINE constexpr PointOf(PointOf<PointCoordT> other)
	    : x(other.x)
	    , y(other.y)
	{
	}

	DVL_ALWAYS_INLINE constexpr PointOf(CoordT x, CoordT y)
	    : x(x)
	    , y(y)
	{
	}

	template <typename PointCoordT>
	DVL_ALWAYS_INLINE explicit constexpr PointOf(DisplacementOf<PointCoordT> other)
	    : x(other.deltaX)
	    , y(other.deltaY)
	{
	}

	template <typename PointCoordT>
	DVL_ALWAYS_INLINE constexpr bool operator==(const PointOf<PointCoordT> &other) const
	{
		return x == other.x && y == other.y;
	}

	template <typename PointCoordT>
	DVL_ALWAYS_INLINE constexpr bool operator!=(const PointOf<PointCoordT> &other) const
	{
		return !(*this == other);
	}

	template <typename DisplacementDeltaT = int>
	DVL_ALWAYS_INLINE constexpr PointOf<CoordT> &operator+=(const DisplacementOf<DisplacementDeltaT> &displacement)
	{
		x += displacement.deltaX;
		y += displacement.deltaY;
		return *this;
	}

	DVL_ALWAYS_INLINE constexpr PointOf<CoordT> &operator+=(Direction direction)
	{
		return (*this) += DisplacementOf<typename std::make_signed<CoordT>::type>(direction);
	}

	template <typename DisplacementDeltaT = int>
	DVL_ALWAYS_INLINE constexpr PointOf<CoordT> &operator-=(const DisplacementOf<DisplacementDeltaT> &displacement)
	{
		x -= displacement.deltaX;
		y -= displacement.deltaY;
		return *this;
	}

	DVL_ALWAYS_INLINE constexpr PointOf<CoordT> &operator*=(const float factor)
	{
		x = static_cast<int>(x * factor);
		y = static_cast<int>(y * factor);
		return *this;
	}

	DVL_ALWAYS_INLINE constexpr PointOf<CoordT> &operator*=(const int factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	DVL_ALWAYS_INLINE constexpr PointOf<CoordT> &operator/=(const int factor)
	{
		x /= factor;
		y /= factor;
		return *this;
	}

	DVL_ALWAYS_INLINE constexpr PointOf<CoordT> operator-() const
	{
		static_assert(std::is_signed<CoordT>::value, "CoordT must be signed");
		return { -x, -y };
	}

	/**
	 * @brief Fast approximate distance between two points, using only integer arithmetic, with less than ~5% error
	 * @param other Pointer to which we want the distance
	 * @return Magnitude of vector this -> other
	 */

	template <typename PointCoordT>
	constexpr int ApproxDistance(PointOf<PointCoordT> other) const
	{
		const Displacement offset = abs(Point(*this) - Point(other));
		const auto [min, max] = std::minmax(offset.deltaX, offset.deltaY);

		int approx = max * 1007 + min * 441;
		if (max < (min * 16))
			approx -= max * 40;

		return (approx + 512) / 1024;
	}

	/**
	 * @brief Calculates the exact distance between two points (as accurate as the closest integer representation)
	 *
	 * In practice it is likely that ApproxDistance gives the same result, especially for nearby points.
	 * @param other Point to which we want the distance
	 * @return Exact magnitude of vector this -> other
	 */
	template <typename PointCoordT>
	int ExactDistance(PointOf<PointCoordT> other) const
	{
		const Displacement vector = Point(*this) - Point(other); // No need to call abs() as we square the values anyway

		// Casting multiplication operands to a wide type to address overflow warnings
		return static_cast<int>(std::sqrt(static_cast<int64_t>(vector.deltaX) * vector.deltaX + static_cast<int64_t>(vector.deltaY) * vector.deltaY));
	}

	template <typename PointCoordT>
	DVL_ALWAYS_INLINE constexpr int ManhattanDistance(PointOf<PointCoordT> other) const
	{
		return std::abs(static_cast<int>(x) - static_cast<int>(other.x))
		    + std::abs(static_cast<int>(y) - static_cast<int>(other.y));
	}

	template <typename PointCoordT>
	DVL_ALWAYS_INLINE constexpr int WalkingDistance(PointOf<PointCoordT> other) const
	{
		return std::max<int>(
		    std::abs(static_cast<int>(x) - static_cast<int>(other.x)),
		    std::abs(static_cast<int>(y) - static_cast<int>(other.y)));
	}

	/**
	 * @brief Converts a coordinate in megatiles to the northmost of the 4 corresponding world tiles
	 */
	DVL_ALWAYS_INLINE constexpr PointOf<CoordT> megaToWorld() const
	{
		return { static_cast<CoordT>(16 + 2 * x), static_cast<CoordT>(16 + 2 * y) };
	}

	/**
	 * @brief Converts a coordinate in world tiles back to the corresponding megatile
	 */
	DVL_ALWAYS_INLINE constexpr PointOf<CoordT> worldToMega() const
	{
		return { static_cast<CoordT>((x - 16) / 2), static_cast<CoordT>((y - 16) / 2) };
	}
};

#ifdef BUILD_TESTING
/**
 * @brief Format points nicely in test failure messages
 * @param stream output stream, expected to have overloads for int and char*
 * @param point Object to display
 * @return the stream, to allow chaining
 */
template <typename PointCoordT>
std::ostream &operator<<(std::ostream &stream, const PointOf<PointCoordT> &point)
{
	return stream << "(x: " << point.x << ", y: " << point.y << ")";
}
#endif

template <typename PointCoordT, typename DisplacementDeltaT>
DVL_ALWAYS_INLINE constexpr PointOf<PointCoordT> operator+(PointOf<PointCoordT> a, DisplacementOf<DisplacementDeltaT> displacement)
{
	a += displacement;
	return a;
}

template <typename PointCoordT>
DVL_ALWAYS_INLINE constexpr PointOf<PointCoordT> operator+(PointOf<PointCoordT> a, Direction direction)
{
	a += direction;
	return a;
}

template <typename PointCoordT, typename OtherPointCoordT>
DVL_ALWAYS_INLINE constexpr DisplacementOf<PointCoordT> operator-(PointOf<PointCoordT> a, PointOf<OtherPointCoordT> b)
{
	static_assert(std::is_signed<PointCoordT>::value == std::is_signed<OtherPointCoordT>::value, "points must have the same signedness");
	return { static_cast<PointCoordT>(a.x - b.x), static_cast<PointCoordT>(a.y - b.y) };
}

template <typename PointCoordT, typename DisplacementDeltaT>
DVL_ALWAYS_INLINE constexpr PointOf<PointCoordT> operator-(PointOf<PointCoordT> a, DisplacementOf<DisplacementDeltaT> displacement)
{
	a -= displacement;
	return a;
}

template <typename PointCoordT>
DVL_ALWAYS_INLINE constexpr PointOf<PointCoordT> operator*(PointOf<PointCoordT> a, const float factor)
{
	a *= factor;
	return a;
}

template <typename PointCoordT>
DVL_ALWAYS_INLINE constexpr PointOf<PointCoordT> operator*(PointOf<PointCoordT> a, const int factor)
{
	a *= factor;
	return a;
}

template <typename PointCoordT>
DVL_ALWAYS_INLINE constexpr PointOf<PointCoordT> abs(PointOf<PointCoordT> a)
{
	return { std::abs(a.x), std::abs(a.y) };
}

/**
 * @brief Calculate the best fit direction between two points
 * @param start Tile coordinate
 * @param destination Tile coordinate
 * @return A value from the direction enum
 */
inline Direction GetDirection(Point start, Point destination)
{
	Direction md;

	int mx = destination.x - start.x;
	int my = destination.y - start.y;
	if (mx >= 0) {
		if (my >= 0) {
			if (5 * mx <= (my * 2)) // mx/my <= 0.4, approximation of tan(22.5)
				return Direction::SouthWest;
			md = Direction::South;
		} else {
			my = -my;
			if (5 * mx <= (my * 2))
				return Direction::NorthEast;
			md = Direction::East;
		}
		if (5 * my <= (mx * 2)) // my/mx <= 0.4
			md = Direction::SouthEast;
	} else {
		mx = -mx;
		if (my >= 0) {
			if (5 * mx <= (my * 2))
				return Direction::SouthWest;
			md = Direction::West;
		} else {
			my = -my;
			if (5 * mx <= (my * 2))
				return Direction::NorthEast;
			md = Direction::North;
		}
		if (5 * my <= (mx * 2))
			md = Direction::NorthWest;
	}
	return md;
}

} // namespace devilution
