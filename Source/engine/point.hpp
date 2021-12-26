#pragma once

#include <cmath>
#ifdef BUILD_TESTING
#include <ostream>
#endif

#include "engine/direction.hpp"
#include "engine/displacement.hpp"
#include "utils/stdcompat/abs.hpp"
#include "utils/stdcompat/algorithm.hpp"

namespace devilution {

struct Point {
	int x;
	int y;

	constexpr bool operator==(const Point &other) const
	{
		return x == other.x && y == other.y;
	}

	constexpr bool operator!=(const Point &other) const
	{
		return !(*this == other);
	}

	constexpr Point &operator+=(const Displacement &displacement)
	{
		x += displacement.deltaX;
		y += displacement.deltaY;
		return *this;
	}

	constexpr Point &operator+=(Direction direction)
	{
		return (*this) += Displacement(direction);
	}

	constexpr Point &operator-=(const Displacement &displacement)
	{
		x -= displacement.deltaX;
		y -= displacement.deltaY;
		return *this;
	}

	constexpr Point &operator*=(const float factor)
	{
		x = static_cast<int>(x * factor);
		y = static_cast<int>(y * factor);
		return *this;
	}

	constexpr Point &operator*=(const int factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	constexpr friend Point operator+(Point a, Displacement displacement)
	{
		a += displacement;
		return a;
	}

	constexpr friend Point operator+(Point a, Direction direction)
	{
		a += direction;
		return a;
	}

	constexpr friend Displacement operator-(Point a, const Point &b)
	{
		return { a.x - b.x, a.y - b.y };
	}

	constexpr friend Point operator-(const Point &a)
	{
		return { -a.x, -a.y };
	}

	constexpr friend Point operator-(Point a, Displacement displacement)
	{
		a -= displacement;
		return a;
	}

	constexpr friend Point operator*(Point a, const float factor)
	{
		a *= factor;
		return a;
	}

	constexpr friend Point operator*(Point a, const int factor)
	{
		a *= factor;
		return a;
	}

	/**
	 * @brief Fast approximate distance between two points, using only integer arithmetic, with less than ~5% error
	 * @param other Pointer to which we want the distance
	 * @return Magnitude of vector this -> other
	 */

	constexpr int ApproxDistance(Point other) const
	{
		Displacement offset = abs(other - *this);
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
	int ExactDistance(Point other) const
	{
		auto vector = *this - other; // No need to call abs() as we square the values anyway

		// Casting multiplication operands to a wide type to address overflow warnings
		return static_cast<int>(std::sqrt(static_cast<int64_t>(vector.deltaX) * vector.deltaX + static_cast<int64_t>(vector.deltaY) * vector.deltaY));
	}

	constexpr friend Point abs(Point a)
	{
		return { abs(a.x), abs(a.y) };
	}

	constexpr int ManhattanDistance(Point other) const
	{
		Displacement offset = abs(*this - other);

		return offset.deltaX + offset.deltaY;
	}

	constexpr int WalkingDistance(Point other) const
	{
		Displacement offset = abs(*this - other);

		return std::max<int>(offset.deltaX, offset.deltaY);
	}

#ifdef BUILD_TESTING
	/**
	 * @brief Format points nicely in test failure messages
	 * @param stream output stream, expected to have overloads for int and char*
	 * @param point Object to display
	 * @return the stream, to allow chaining
	 */
	friend std::ostream &operator<<(std::ostream &stream, const Point &point)
	{
		return stream << "(x: " << point.x << ", y: " << point.y << ")";
	}
#endif
};

} // namespace devilution
