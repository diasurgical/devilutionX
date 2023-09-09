#pragma once

#include "utils/attributes.h"

#ifdef BUILD_TESTING
#include <ostream>
#endif

namespace devilution {

template <typename SizeT>
struct SizeOf {
	SizeT width;
	SizeT height;

	SizeOf() = default;

	DVL_ALWAYS_INLINE constexpr SizeOf(SizeT width, SizeT height)
	    : width(width)
	    , height(height)
	{
	}

	DVL_ALWAYS_INLINE explicit constexpr SizeOf(SizeT size)
	    : width(size)
	    , height(size)
	{
	}

	DVL_ALWAYS_INLINE bool operator==(const SizeOf<SizeT> &other) const
	{
		return width == other.width && height == other.height;
	}

	DVL_ALWAYS_INLINE bool operator!=(const SizeOf<SizeT> &other) const
	{
		return !(*this == other);
	}

	DVL_ALWAYS_INLINE constexpr SizeOf<SizeT> &operator+=(SizeT factor)
	{
		width += factor;
		height += factor;
		return *this;
	}

	DVL_ALWAYS_INLINE constexpr SizeOf<SizeT> &operator-=(SizeT factor)
	{
		return *this += -factor;
	}

	DVL_ALWAYS_INLINE constexpr SizeOf<SizeT> &operator*=(SizeT factor)
	{
		width *= factor;
		height *= factor;
		return *this;
	}

	DVL_ALWAYS_INLINE constexpr SizeOf<SizeT> &operator*=(float factor)
	{
		width = static_cast<SizeT>(width * factor);
		height = static_cast<SizeT>(height * factor);
		return *this;
	}

	DVL_ALWAYS_INLINE constexpr SizeOf<SizeT> &operator/=(SizeT factor)
	{
		width /= factor;
		height /= factor;
		return *this;
	}

	DVL_ALWAYS_INLINE constexpr friend SizeOf<SizeT> operator+(SizeOf<SizeT> a, SizeT factor)
	{
		a += factor;
		return a;
	}

	DVL_ALWAYS_INLINE constexpr friend SizeOf<SizeT> operator-(SizeOf<SizeT> a, SizeT factor)
	{
		a -= factor;
		return a;
	}

	DVL_ALWAYS_INLINE constexpr friend SizeOf<SizeT> operator*(SizeOf<SizeT> a, SizeT factor)
	{
		a *= factor;
		return a;
	}

	DVL_ALWAYS_INLINE constexpr friend SizeOf<SizeT> operator/(SizeOf<SizeT> a, SizeT factor)
	{
		a /= factor;
		return a;
	}

#ifdef BUILD_TESTING
	/**
	 * @brief Format sizes nicely in test failure messages
	 * @param stream output stream, expected to have overloads for int and char*
	 * @param size Object to display
	 * @return the stream, to allow chaining
	 */
	friend std::ostream &operator<<(std::ostream &stream, const SizeOf<SizeT> &size)
	{
		return stream << "(width: " << size.width << ", height: " << size.height << ")";
	}
#endif
};

using Size = SizeOf<int>;

} // namespace devilution
