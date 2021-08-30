#pragma once

namespace devilution {

struct Size {
	int width;
	int height;

	bool operator==(const Size &other) const
	{
		return width == other.width && height == other.height;
	}

	bool operator!=(const Size &other) const
	{
		return !(*this == other);
	}

	constexpr Size &operator/=(const int factor)
	{
		width /= factor;
		height /= factor;
		return *this;
	}

	constexpr friend Size operator/(Size a, const int factor)
	{
		a /= factor;
		return a;
	}
};

} // namespace devilution
