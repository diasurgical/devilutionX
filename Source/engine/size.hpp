#pragma once

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
};

} // namespace devilution
