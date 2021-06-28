#pragma once

#include <cmath>

#include "engine/direction.hpp"
#include "engine/size.hpp"
#include "utils/stdcompat/abs.hpp"
#include "utils/stdcompat/algorithm.hpp"

namespace devilution {

struct Point {
	int x;
	int y;

	static constexpr Point fromDirection(Direction direction)
	{
		switch (direction) {
		case DIR_S:
			return { 1, 1 };
		case DIR_SW:
			return { 0, 1 };
		case DIR_W:
			return { -1, 1 };
		case DIR_NW:
			return { -1, 0 };
		case DIR_N:
			return { -1, -1 };
		case DIR_NE:
			return { 0, -1 };
		case DIR_E:
			return { 1, -1 };
		case DIR_SE:
			return { 1, 0 };
		default:
			return { 0, 0 };
		}
	};

	constexpr bool operator==(const Point &other) const
	{
		return x == other.x && y == other.y;
	}

	constexpr bool operator!=(const Point &other) const
	{
		return !(*this == other);
	}

	constexpr Point &operator+=(const Point &other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	constexpr Point &operator+=(Direction direction)
	{
		return (*this) += Point::fromDirection(direction);
	}

	constexpr Point &operator+=(const Size &size)
	{
		x += size.width;
		y += size.height;
		return *this;
	}

	constexpr Point &operator-=(const Point &other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	constexpr Point &operator-=(const Size &size)
	{
		x -= size.width;
		y -= size.height;
		return *this;
	}

	constexpr Point &operator*=(const float factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	constexpr Point &operator*=(const int factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	constexpr friend Point operator+(Point a, const Point &b)
	{
		a += b;
		return a;
	}

	constexpr friend Point operator+(Point a, Direction direction)
	{
		a += direction;
		return a;
	}

	constexpr friend Point operator+(Point a, Size size)
	{
		a += size;
		return a;
	}

	constexpr friend Point operator-(Point a, const Point &b)
	{
		a -= b;
		return a;
	}

	constexpr friend Point operator-(const Point &a)
	{
		return { -a.x, -a.y };
	}

	constexpr friend Point operator-(Point a, Size size)
	{
		a -= size;
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
		Point offset = abs(other - *this);
		auto minMax = std::minmax(offset.x, offset.y);
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
		auto vector = *this - other; //No need to call abs() as we square the values anyway

		// Casting multiplication operands to a wide type to address overflow warnings
		return static_cast<int>(std::sqrt(static_cast<int64_t>(vector.x) * vector.x + static_cast<int64_t>(vector.y) * vector.y));
	}

	constexpr friend Point abs(Point a)
	{
		return { abs(a.x), abs(a.y) };
	}

	constexpr int ManhattanDistance(Point other) const
	{
		Point offset = abs(*this - other);

		return offset.x + offset.y;
	}

	constexpr int WalkingDistance(Point other) const
	{
		Point offset = abs(*this - other);

		return std::max<int>(offset.x, offset.y);
	}
};

} // namespace devilution
