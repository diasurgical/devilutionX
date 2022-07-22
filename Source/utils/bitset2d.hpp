#pragma once

#include <bitset>
#include <cstddef>

namespace devilution {

/**
 * @brief A 2D variant of std::bitset.
 *
 * For function documentation, see `std::bitset`
 *
 * @tparam Width
 * @tparam Height
 */
template <size_t Width, size_t Height>
class Bitset2d {
public:
	bool test(size_t x, size_t y) const
	{
		return data_.test(index(x, y));
	}

	void set(size_t x, size_t y, bool value = true)
	{
		data_.set(index(x, y), value);
	}

	void reset(size_t x, size_t y)
	{
		data_.reset(index(x, y));
	}

	void reset()
	{
		data_.reset();
	}

	[[nodiscard]] size_t count() const
	{
		return data_.count();
	}

private:
	static size_t index(size_t x, size_t y)
	{
		return y * Width + x;
	}

	std::bitset<Width * Height> data_;
};

} // namespace devilution
