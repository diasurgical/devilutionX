#pragma once

#include <cmath>
#include <type_traits>
#ifdef BUILD_TESTING
#include <ostream>
#endif

#include "engine/direction.hpp"
#include "engine/displacement.hpp"
#include "utils/stdcompat/abs.hpp"
#include "utils/stdcompat/algorithm.hpp"

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
	constexpr PointOf(PointOf<PointCoordT> other)
	    : x(other.x)
	    , y(other.y)
	{
	}

	constexpr PointOf(CoordT x, CoordT y)
	    : x(x)
	    , y(y)
	{
	}

	template <typename PointCoordT>
	constexpr bool operator==(const PointOf<PointCoordT> &other) const
	{
		return x == other.x && y == other.y;
	}

	template <typename PointCoordT>
	constexpr bool operator!=(const PointOf<PointCoordT> &other) const
	{
		return !(*this == other);
	}

	template <typename DisplacementDeltaT = int>
	constexpr PointOf<CoordT> &operator+=(const DisplacementOf<DisplacementDeltaT> &displacement)
	{
		x += displacement.deltaX;
		y += displacement.deltaY;
		return *this;
	}

	constexpr PointOf<CoordT> &operator+=(Direction direction)
	{
		return (*this) += DisplacementOf<typename std::make_signed<CoordT>::type>(direction);
	}

	template <typename DisplacementDeltaT = int>
	constexpr PointOf<CoordT> &operator-=(const DisplacementOf<DisplacementDeltaT> &displacement)
	{
		x -= displacement.deltaX;
		y -= displacement.deltaY;
		return *this;
	}

	constexpr PointOf<CoordT> &operator*=(const float factor)
	{
		x = static_cast<int>(x * factor);
		y = static_cast<int>(y * factor);
		return *this;
	}

	constexpr PointOf<CoordT> &operator*=(const int factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	constexpr PointOf<CoordT> operator-() const
	{
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
		auto minMax = std::minmax(offset.deltaX, offset.deltaY);
		int min = minMax.first;
		int max = minMax.second;

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
	constexpr int ManhattanDistance(PointOf<PointCoordT> other) const
	{
		const Displacement offset = abs(Point(*this) - Point(other));

		return offset.deltaX + offset.deltaY;
	}

	template <typename PointCoordT>
	constexpr int WalkingDistance(PointOf<PointCoordT> other) const
	{
		const Displacement offset = abs(Point(*this) - Point(other));

		return std::max<int>(offset.deltaX, offset.deltaY);
	}

	/**
	 * @brief Converts a coordinate in megatiles to the northmost of the 4 corresponding world tiles
	 */
	constexpr PointOf<CoordT> megaToWorld() const
	{
		return { 16 + 2 * x, 16 + 2 * y };
	}

	/**
	 * @brief Converts a coordinate in world tiles back to the corresponding megatile
	 */
	constexpr PointOf<CoordT> worldToMega() const
	{
		return { (x - 16) / 2, (y - 16) / 2 };
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
constexpr PointOf<PointCoordT> operator+(PointOf<PointCoordT> a, DisplacementOf<DisplacementDeltaT> displacement)
{
	a += displacement;
	return a;
}

template <typename PointCoordT>
constexpr PointOf<PointCoordT> operator+(PointOf<PointCoordT> a, Direction direction)
{
	a += direction;
	return a;
}

template <typename PointCoordT, typename OtherPointCoordT>
constexpr DisplacementOf<PointCoordT> operator-(PointOf<PointCoordT> a, PointOf<OtherPointCoordT> b)
{
	return { static_cast<PointCoordT>(a.x - b.x), static_cast<PointCoordT>(a.y - b.y) };
}

template <typename PointCoordT, typename DisplacementDeltaT>
constexpr PointOf<PointCoordT> operator-(PointOf<PointCoordT> a, DisplacementOf<DisplacementDeltaT> displacement)
{
	a -= displacement;
	return a;
}

template <typename PointCoordT>
constexpr PointOf<PointCoordT> operator*(PointOf<PointCoordT> a, const float factor)
{
	a *= factor;
	return a;
}

template <typename PointCoordT>
constexpr PointOf<PointCoordT> operator*(PointOf<PointCoordT> a, const int factor)
{
	a *= factor;
	return a;
}

template <typename PointCoordT>
constexpr PointOf<PointCoordT> abs(PointOf<PointCoordT> a)
{
	return { abs(a.x), abs(a.y) };
}

} // namespace devilution
