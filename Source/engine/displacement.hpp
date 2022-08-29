#pragma once

#include <cmath>
#ifdef BUILD_TESTING
#include <ostream>
#endif

#include "direction.hpp"
#include "size.hpp"
#include "utils/stdcompat/abs.hpp"

namespace devilution {

template <typename DeltaT>
struct DisplacementOf;

using Displacement = DisplacementOf<int>;

template <typename DeltaT>
struct DisplacementOf {
	DeltaT deltaX;
	DeltaT deltaY;

	DisplacementOf() = default;

	template <typename DisplacementDeltaT>
	constexpr DisplacementOf(DisplacementOf<DisplacementDeltaT> other)
	    : deltaX(other.deltaX)
	    , deltaY(other.deltaY)
	{
	}

	constexpr DisplacementOf(DeltaT deltaX, DeltaT deltaY)
	    : deltaX(deltaX)
	    , deltaY(deltaY)
	{
	}

	explicit constexpr DisplacementOf(DeltaT delta)
	    : deltaX(delta)
	    , deltaY(delta)
	{
	}

	explicit constexpr DisplacementOf(const Size &size)
	    : deltaX(size.width)
	    , deltaY(size.height)
	{
	}

	explicit constexpr DisplacementOf(Direction direction)
	    : DisplacementOf(fromDirection(direction))
	{
	}

	template <typename DisplacementDeltaT>
	constexpr bool operator==(const DisplacementOf<DisplacementDeltaT> &other) const
	{
		return deltaX == other.deltaX && deltaY == other.deltaY;
	}

	template <typename DisplacementDeltaT>
	constexpr bool operator!=(const DisplacementOf<DisplacementDeltaT> &other) const
	{
		return !(*this == other);
	}

	template <typename DisplacementDeltaT = DeltaT>
	constexpr DisplacementOf<DeltaT> &operator+=(DisplacementOf<DisplacementDeltaT> displacement)
	{
		deltaX += displacement.deltaX;
		deltaY += displacement.deltaY;
		return *this;
	}

	template <typename DisplacementDeltaT = DeltaT>
	constexpr DisplacementOf<DeltaT> &operator-=(DisplacementOf<DisplacementDeltaT> displacement)
	{
		deltaX -= displacement.deltaX;
		deltaY -= displacement.deltaY;
		return *this;
	}

	constexpr DisplacementOf<DeltaT> &operator*=(const int factor)
	{
		deltaX *= factor;
		deltaY *= factor;
		return *this;
	}

	constexpr DisplacementOf<DeltaT> &operator*=(const float factor)
	{
		deltaX = static_cast<DeltaT>(deltaX * factor);
		deltaY = static_cast<DeltaT>(deltaY * factor);
		return *this;
	}

	template <typename DeltaU>
	constexpr DisplacementOf<DeltaT> &operator*=(const DisplacementOf<DeltaU> factor)
	{
		deltaX = static_cast<DeltaT>(deltaX * factor.deltaX);
		deltaY = static_cast<DeltaT>(deltaY * factor.deltaY);
		return *this;
	}

	constexpr DisplacementOf<DeltaT> &operator/=(const int factor)
	{
		deltaX /= factor;
		deltaY /= factor;
		return *this;
	}

	constexpr DisplacementOf<DeltaT> &operator/=(const float factor)
	{
		deltaX = static_cast<DeltaT>(deltaX / factor);
		deltaY = static_cast<DeltaT>(deltaY / factor);
		return *this;
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
	constexpr DisplacementOf<DeltaT> worldToScreen() const
	{
		static_assert(std::is_signed<DeltaT>::value, "DeltaT must be signed for transformations involving a rotation");
		return { (deltaY - deltaX) * 32, (deltaY + deltaX) * -16 };
	}

	/**
	 * @brief Returns a new Displacement object in world coordinates.
	 *
	 * This is an inverse matrix of the worldToScreen transformation.
	 *
	 * @return A representation of the original displacement in world coordinates.
	 */
	constexpr DisplacementOf<DeltaT> screenToWorld() const
	{
		static_assert(std::is_signed<DeltaT>::value, "DeltaT must be signed for transformations involving a rotation");
		return { (2 * deltaY + deltaX) / -64, (2 * deltaY - deltaX) / -64 };
	}

	/**
	 * @brief Missiles flip the axes for some reason -_-
	 * @return negated world displacement, for use with missile movement routines.
	 */
	constexpr DisplacementOf<DeltaT> screenToMissile() const
	{
		return -screenToWorld();
	}

	constexpr DisplacementOf<DeltaT> screenToLight() const
	{
		static_assert(std::is_signed<DeltaT>::value, "DeltaT must be signed for transformations involving a rotation");
		return { (2 * deltaY + deltaX) / 8, (2 * deltaY - deltaX) / 8 };
	}

	/**
	 * @brief Returns a 16 bit fixed point normalised displacement in isometric projection
	 *
	 * This will return a displacement of the form (-1.0 to 1.0, -0.5 to 0.5), to get a full tile offset you can multiply by 16
	 */
	[[nodiscard]] Displacement worldToNormalScreen() const
	{
		static_assert(std::is_signed<DeltaT>::value, "DeltaT must be signed for transformations involving a rotation");
		// Most transformations between world and screen space take shortcuts when scaling to simplify the math. This
		//  routine is typically used with missiles where we want a normal vector that can be multiplied with a target
		//  velocity (given in pixels). We could normalize the vector first but then we'd need to scale it during
		//  rotation from world to screen space. To save performing unnecessary divisions we rotate first without
		//  correcting the scaling. This gives a vector in elevation projection aligned with screen space.
		DisplacementOf<DeltaT> rotated { deltaY - deltaX, -(deltaY + deltaX) };
		// then normalize this vector
		Displacement rotatedAndNormalized = rotated.normalized();
		// and finally scale the y axis to bring it to isometric projection
		return { rotatedAndNormalized.deltaX, rotatedAndNormalized.deltaY / 2 };
	}

	/**
	 * @brief Calculates a 16 bit fixed point normalized displacement (having magnitude of ~1.0) from the current Displacement
	 */
	[[nodiscard]] Displacement normalized() const;

	[[nodiscard]] constexpr DisplacementOf<DeltaT> Rotate(int quadrants) const
	{
		static_assert(std::is_signed<DeltaT>::value, "DeltaT must be signed for Rotate");
		constexpr DeltaT Sines[] = { 0, 1, 0, -1 };

		quadrants = (quadrants % 4 + 4) % 4;

		DeltaT sine = Sines[quadrants];
		DeltaT cosine = Sines[(quadrants + 1) % 4];

		return DisplacementOf { deltaX * cosine - deltaY * sine, deltaX * sine + deltaY * cosine };
	}

	[[nodiscard]] constexpr DisplacementOf<DeltaT> flipX() const
	{
		static_assert(std::is_signed<DeltaT>::value, "DeltaT must be signed for flipX");
		return { static_cast<DeltaT>(-deltaX), deltaY };
	}

	[[nodiscard]] constexpr DisplacementOf<DeltaT> flipY() const
	{
		static_assert(std::is_signed<DeltaT>::value, "DeltaT must be signed for flipY");
		return { deltaX, static_cast<DeltaT>(-deltaY) };
	}

	[[nodiscard]] constexpr DisplacementOf<DeltaT> flipXY() const
	{
		static_assert(std::is_signed<DeltaT>::value, "DeltaT must be signed for flipXY");
		return { static_cast<DeltaT>(-deltaX), static_cast<DeltaT>(-deltaY) };
	}

private:
	static constexpr DisplacementOf<DeltaT> fromDirection(Direction direction)
	{
		static_assert(std::is_signed<DeltaT>::value, "DeltaT must be signed for conversion from Direction");
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
		case Direction::NoDirection:
			return { 0, 0 };
		default:
			return { 0, 0 };
		}
	};
};

#ifdef BUILD_TESTING
/**
 * @brief Format displacements nicely in test failure messages
 * @param stream output stream, expected to have overloads for int and char*
 * @param offset Object to display
 * @return the stream, to allow chaining
 */
template <typename DisplacementDeltaT>
std::ostream &operator<<(std::ostream &stream, const DisplacementOf<DisplacementDeltaT> &offset)
{
	return stream << "(x: " << offset.deltaX << ", y: " << offset.deltaY << ")";
}
#endif

template <typename DisplacementDeltaT, typename OtherDisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> operator+(DisplacementOf<DisplacementDeltaT> a, DisplacementOf<OtherDisplacementDeltaT> b)
{
	a += b;
	return a;
}

template <typename DisplacementDeltaT, typename OtherDisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> operator-(DisplacementOf<DisplacementDeltaT> a, DisplacementOf<OtherDisplacementDeltaT> b)
{
	a -= b;
	return a;
}

template <typename DisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> operator*(DisplacementOf<DisplacementDeltaT> a, const int factor)
{
	a *= factor;
	return a;
}

template <typename DisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> operator*(DisplacementOf<DisplacementDeltaT> a, const float factor)
{
	a *= factor;
	return a;
}

template <typename DisplacementDeltaT, typename DisplacementDeltaU>
constexpr DisplacementOf<DisplacementDeltaT> operator*(DisplacementOf<DisplacementDeltaT> a, const DisplacementOf<DisplacementDeltaU> factor)
{
	a *= factor;
	return a;
}

template <typename DisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> operator/(DisplacementOf<DisplacementDeltaT> a, const int factor)
{
	a /= factor;
	return a;
}

template <typename DisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> operator/(DisplacementOf<DisplacementDeltaT> a, const float factor)
{
	a /= factor;
	return a;
}

template <typename DisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> operator-(DisplacementOf<DisplacementDeltaT> a)
{
	return { -a.deltaX, -a.deltaY };
}

template <typename DisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> operator<<(DisplacementOf<DisplacementDeltaT> a, unsigned factor)
{
	return { a.deltaX << factor, a.deltaY << factor };
}

template <typename DisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> operator>>(DisplacementOf<DisplacementDeltaT> a, unsigned factor)
{
	return { a.deltaX >> factor, a.deltaY >> factor };
}

template <typename DisplacementDeltaT>
constexpr DisplacementOf<DisplacementDeltaT> abs(DisplacementOf<DisplacementDeltaT> a)
{
	return { abs(a.deltaX), abs(a.deltaY) };
}

template <typename DeltaT>
Displacement DisplacementOf<DeltaT>::normalized() const
{
	const float magnitude = this->magnitude();
	Displacement normalDisplacement = Displacement(*this) << 16u;
	normalDisplacement /= magnitude;
	return normalDisplacement;
}

} // namespace devilution
