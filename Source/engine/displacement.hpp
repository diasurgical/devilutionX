#pragma once

#include <cmath>

#include "direction.hpp"
#include "utils/stdcompat/abs.hpp"

namespace devilution {

struct Displacement {
	int deltaX;
	int deltaY;

	constexpr bool operator==(const Displacement &other) const
	{
		return deltaX == other.deltaX && deltaY == other.deltaY;
	}

	constexpr bool operator!=(const Displacement &other) const
	{
		return !(*this == other);
	}

	constexpr Displacement &operator+=(const Displacement &displacement)
	{
		deltaX += displacement.deltaX;
		deltaY += displacement.deltaY;
		return *this;
	}

	constexpr Displacement &operator-=(const Displacement &displacement)
	{
		deltaX -= displacement.deltaX;
		deltaY -= displacement.deltaY;
		return *this;
	}

	constexpr Displacement &operator*=(const int factor)
	{
		deltaX *= factor;
		deltaY *= factor;
		return *this;
	}

	constexpr Displacement &operator*=(const float factor)
	{
		deltaX *= factor;
		deltaY *= factor;
		return *this;
	}

	constexpr friend Displacement operator+(Displacement a, Displacement b)
	{
		a += b;
		return a;
	}

	constexpr friend Displacement operator-(Displacement a, Displacement b)
	{
		a -= b;
		return a;
	}

	constexpr friend Displacement operator*(Displacement a, const int factor)
	{
		a *= factor;
		return a;
	}

	constexpr friend Displacement operator*(Displacement a, const float factor)
	{
		a *= factor;
		return a;
	}

	constexpr friend Displacement operator-(const Displacement &a)
	{
		return { -a.deltaX, -a.deltaY };
	}

	constexpr friend Displacement abs(Displacement a)
	{
		return { abs(a.deltaX), abs(a.deltaY) };
	}

	static constexpr Displacement fromDirection(Direction direction)
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
};

} // namespace devilution
