#pragma once

#ifdef BUILD_TESTING
#include <ostream>
#endif

namespace devilution {

struct Size {
	int width;
	int height;

	Size() = default;

	constexpr Size(int width, int height)
	    : width(width)
	    , height(height)
	{
	}

	explicit constexpr Size(int size)
	    : width(size)
	    , height(size)
	{
	}

	bool operator==(const Size &other) const
	{
		return width == other.width && height == other.height;
	}

	bool operator!=(const Size &other) const
	{
		return !(*this == other);
	}

	constexpr Size &operator+=(const int factor)
	{
		width += factor;
		height += factor;
		return *this;
	}

	constexpr Size &operator-=(const int factor)
	{
		return *this += -factor;
	}

	constexpr Size &operator*=(const int factor)
	{
		width *= factor;
		height *= factor;
		return *this;
	}

	constexpr Size &operator*=(const float factor)
	{
		width = static_cast<int>(width * factor);
		height = static_cast<int>(height * factor);
		return *this;
	}

	constexpr Size &operator/=(const int factor)
	{
		width /= factor;
		height /= factor;
		return *this;
	}

	constexpr friend Size operator+(Size a, const int factor)
	{
		a += factor;
		return a;
	}

	constexpr friend Size operator-(Size a, const int factor)
	{
		a -= factor;
		return a;
	}

	constexpr friend Size operator*(Size a, const int factor)
	{
		a *= factor;
		return a;
	}

	constexpr friend Size operator/(Size a, const int factor)
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
	friend std::ostream &operator<<(std::ostream &stream, const Size &size)
	{
		return stream << "(width: " << size.width << ", height: " << size.height << ")";
	}
#endif
};

} // namespace devilution
