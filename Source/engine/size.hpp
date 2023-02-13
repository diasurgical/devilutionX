#pragma once

#ifdef BUILD_TESTING
#include <ostream>
#endif

namespace devilution {

template <typename SizeT>
struct SizeOf {
	SizeT width;
	SizeT height;

	SizeOf() = default;

	constexpr SizeOf(SizeT width, SizeT height)
	    : width(width)
	    , height(height)
	{
	}

	explicit constexpr SizeOf(SizeT size)
	    : width(size)
	    , height(size)
	{
	}

	bool operator==(const SizeOf<SizeT> &other) const
	{
		return width == other.width && height == other.height;
	}

	bool operator!=(const SizeOf<SizeT> &other) const
	{
		return !(*this == other);
	}

	constexpr SizeOf<SizeT> &operator+=(SizeT factor)
	{
		width += factor;
		height += factor;
		return *this;
	}

	constexpr SizeOf<SizeT> &operator-=(SizeT factor)
	{
		return *this += -factor;
	}

	constexpr SizeOf<SizeT> &operator*=(SizeT factor)
	{
		width *= factor;
		height *= factor;
		return *this;
	}

	constexpr SizeOf<SizeT> &operator*=(float factor)
	{
		width = static_cast<SizeT>(width * factor);
		height = static_cast<SizeT>(height * factor);
		return *this;
	}

	constexpr SizeOf<SizeT> &operator/=(SizeT factor)
	{
		width /= factor;
		height /= factor;
		return *this;
	}

	constexpr friend SizeOf<SizeT> operator+(SizeOf<SizeT> a, SizeT factor)
	{
		a += factor;
		return a;
	}

	constexpr friend SizeOf<SizeT> operator-(SizeOf<SizeT> a, SizeT factor)
	{
		a -= factor;
		return a;
	}

	constexpr friend SizeOf<SizeT> operator*(SizeOf<SizeT> a, SizeT factor)
	{
		a *= factor;
		return a;
	}

	constexpr friend SizeOf<SizeT> operator/(SizeOf<SizeT> a, SizeT factor)
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
