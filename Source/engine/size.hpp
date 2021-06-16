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
};

} // namespace devilution
