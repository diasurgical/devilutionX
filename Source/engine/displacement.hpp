#pragma once

#include <cmath>
#ifdef BUILD_TESTING
#include <ostream>
#endif

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

	explicit constexpr Displacement(int delta)
	    : deltaX(delta)
	    , deltaY(delta)
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

	constexpr Displacement &operator/=(const int factor)
	{
		deltaX /= factor;
		deltaY /= factor;
		return *this;
	}

	constexpr Displacement &operator/=(const float factor)
	{
		deltaX = static_cast<int>(deltaX / factor);
		deltaY = static_cast<int>(deltaY / factor);
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

	constexpr friend Displacement operator/(Displacement a, const int factor)
	{
		a /= factor;
		return a;
	}

	constexpr friend Displacement operator/(Displacement a, const float factor)
	{
		a /= factor;
		return a;
	}

	constexpr friend Displacement operator-(const Displacement &a)
	{
		return { -a.deltaX, -a.deltaY };
	}

	constexpr friend Displacement operator<<(Displacement a, unsigned factor)
	{
		return { a.deltaX << factor, a.deltaY << factor };
	}

	constexpr friend Displacement operator>>(Displacement a, unsigned factor)
	{
		return { a.deltaX >> factor, a.deltaY >> factor };
	}

	constexpr friend Displacement abs(Displacement a)
	{
		return { abs(a.deltaX), abs(a.deltaY) };
	}

	float magnitude() const
	{
		return static_cast<float>(hypot(deltaX, deltaY));
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
	constexpr Displacement worldToScreen() const
	{
		return { (deltaY - deltaX) * 32, (deltaY + deltaX) * -16 };
	}

	/**
	 * @brief Returns a new Displacement object in world coordinates.
	 *
	 * This is an inverse matrix of the worldToScreen transformation.
	 *
	 * @return A representation of the original displacement in world coordinates.
	 */
	constexpr Displacement screenToWorld() const
	{
		return { (2 * deltaY + deltaX) / -64, (2 * deltaY - deltaX) / -64 };
	}

	/**
	 * @brief Missiles flip the axes for some reason -_-
	 * @return negated world displacement, for use with missile movement routines.
	 */
	constexpr Displacement screenToMissile() const
	{
		return -screenToWorld();
	}

	constexpr Displacement screenToLight() const
	{
		return { (2 * deltaY + deltaX) / 8, (2 * deltaY - deltaX) / 8 };
	}

	/**
	 * @brief Returns a 16 bit fixed point normalised displacement in isometric projection
	 *
	 * This will return a displacement of the form (-1.0 to 1.0, -0.5 to 0.5), to get a full tile offset you can multiply by 16
	 */
	Displacement worldToNormalScreen() const
	{
		// Most transformations between world and screen space take shortcuts when scaling to simplify the math. This
		//  routine is typically used with missiles where we want a normal vector that can be multiplied with a target
		//  velocity (given in pixels). We could normalize the vector first but then we'd need to scale it during
		//  rotation from world to screen space. To save performing unnecessary divisions we rotate first without
		//  correcting the scaling. This gives a vector in elevation projection aligned with screen space.
		Displacement rotated { (deltaY - deltaX), -(deltaY + deltaX) };
		// then normalize this vector
		Displacement rotatedAndNormalized = rotated.normalized();
		// and finally scale the y axis to bring it to isometric projection
		return { rotatedAndNormalized.deltaX, rotatedAndNormalized.deltaY / 2 };
	}

	/**
	 * @brief Calculates a 16 bit fixed point normalized displacement (having magnitude of ~1.0) from the current Displacement
	 */
	Displacement normalized() const
	{
		float magnitude = this->magnitude();
		Displacement normalDisplacement = *this << 16;
		normalDisplacement /= magnitude;
		return normalDisplacement;
	}

	constexpr Displacement Rotate(int quadrants)
	{
		constexpr int Sines[] = { 0, 1, 0, -1 };

		quadrants = (quadrants % 4 + 4) % 4;

		int sine = Sines[quadrants];
		int cosine = Sines[(quadrants + 1) % 4];

		return Displacement { deltaX * cosine - deltaY * sine, deltaX * sine + deltaY * cosine };
	}

#ifdef BUILD_TESTING
	/**
	 * @brief Format displacements nicely in test failure messages
	 * @param stream output stream, expected to have overloads for int and char*
	 * @param offset Object to display
	 * @return the stream, to allow chaining
	 */
	friend std::ostream &operator<<(std::ostream &stream, const Displacement &offset)
	{
		return stream << "(x: " << offset.deltaX << ", y: " << offset.deltaY << ")";
	}
#endif

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
