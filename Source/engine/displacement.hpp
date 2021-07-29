#pragma once

#include <cmath>

#include "direction.hpp"
#include "size.hpp"
#include "utils/stdcompat/abs.hpp"

namespace devilution {

struct Displacement {
	int deltaX;
	int deltaY;

	Displacement() = default;

	constexpr Displacement(int deltaX, int deltaY)
	    : deltaX(deltaX)
	    , deltaY(deltaY)
	{
	}

	explicit constexpr Displacement(const Size &size)
	    : deltaX(size.width)
	    , deltaY(size.height)
	{
	}

	explicit constexpr Displacement(Direction direction)
	    : Displacement(fromDirection(direction))
	{
	}

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
		deltaX = static_cast<int>(deltaX * factor);
		deltaY = static_cast<int>(deltaY * factor);
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

	/**
	 * @brief Returns a new Displacement object in screen coordinates.
	 *
	 * Transforming from world space to screen space involves a rotation of -135° and scaling to fit within a 64x32 pixel tile (since Diablo uses isometric projection)
	 * 32 and 16 are used as the x/y scaling factors being half the relevant max dimension, the rotation matrix is [[-, +], [-, -]] as sin(-135°) = cos(-135°) = ~-0.7.
	 *
	 * [-32,  32] [dx] = [-32dx +  32dy] = [  32dy - 32dx ] = [ 32(dy - dx)]
	 * [-16, -16] [dy] = [-16dx + -16dy] = [-(16dy + 16dx)] = [-16(dy + dx)]
	 *
	 * @return A representation of the original displacement in screen coordinates.
	 */
	constexpr Displacement WorldToScreen() const
	{
		return { (deltaY - deltaX) * 32, (deltaY + deltaX) * -16 };
	}

private:
	static constexpr Displacement fromDirection(Direction direction)
	{
		switch (direction) {
		case Direction::South:
			return { 1, 1 };
		case Direction::SouthWest:
			return { 0, 1 };
		case Direction::West:
			return { -1, 1 };
		case Direction::NorthWest:
			return { -1, 0 };
		case Direction::North:
			return { -1, -1 };
		case Direction::NorthEast:
			return { 0, -1 };
		case Direction::East:
			return { 1, -1 };
		case Direction::SouthEast:
			return { 1, 0 };
		default:
			return { 0, 0 };
		}
	};
};

} // namespace devilution
