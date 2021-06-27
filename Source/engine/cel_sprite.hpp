#pragma once

#include <memory>
#include <utility>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

/**
 * Stores a CEL or CL2 sprite and its width(s).
 *
 * The data may be unowned.
 * Eventually we'd like to remove the unowned version.
 */
class CelSprite {
public:
	CelSprite(std::unique_ptr<byte[]> data, int width)
	    : data_(std::move(data))
	    , data_ptr_(data_.get())
	    , width_(width)
	{
	}

	CelSprite(std::unique_ptr<byte[]> data, const int *widths)
	    : data_(std::move(data))
	    , data_ptr_(data_.get())
	    , widths_(widths)
	{
	}

	/**
	 * Constructs an unowned sprite.
	 * Ideally we'd like to remove all uses of this constructor.
	 */
	CelSprite(const byte *data, int width)
	    : data_ptr_(data)
	    , width_(width)
	{
	}

	CelSprite(CelSprite &&) noexcept = default;
	CelSprite &operator=(CelSprite &&) noexcept = default;

	[[nodiscard]] const byte *Data() const
	{
		return data_ptr_;
	}

	[[nodiscard]] int Width(std::size_t frame = 1) const
	{
		return widths_ == nullptr ? width_ : widths_[frame];
	}

private:
	std::unique_ptr<byte[]> data_;
	const byte *data_ptr_;
	int width_ = 0;
	const int *widths_ = nullptr; // unowned
};

} // namespace devilution
