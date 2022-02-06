#pragma once

#include <memory>
#include <utility>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

class OwnedCelSprite;

/**
 * Stores a CEL or CL2 sprite and its width(s).
 * Does not own the data.
 */
class CelSprite {
public:
	CelSprite(const byte *data, int width)
	    : data_ptr_(data)
	    , width_(width)
	{
	}

	CelSprite(const byte *data, const int *widths)
	    : data_ptr_(data)
	    , widths_(widths)
	{
	}

	explicit CelSprite(const OwnedCelSprite &owned);

	CelSprite(const CelSprite &) = default;
	CelSprite(CelSprite &&) noexcept = default;
	CelSprite &operator=(const CelSprite &) = default;
	CelSprite &operator=(CelSprite &&) noexcept = default;

	[[nodiscard]] const byte *Data() const
	{
		return data_ptr_;
	}

	[[nodiscard]] int Width(std::size_t frame = 1) const
	{
		return widths_ == nullptr ? width_ : widths_[frame];
	}

	[[nodiscard]] bool operator==(CelSprite other) const
	{
		return data_ptr_ == other.data_ptr_;
	}
	[[nodiscard]] bool operator!=(CelSprite other) const
	{
		return data_ptr_ != other.data_ptr_;
	}

private:
	const byte *data_ptr_;
	int width_ = 0;
	const int *widths_ = nullptr; // unowned
};

/**
 * Stores a CEL or CL2 sprite and its width(s).
 * Owns the data.
 */
class OwnedCelSprite : public CelSprite {
public:
	OwnedCelSprite(std::unique_ptr<byte[]> data, int width)
	    : CelSprite(data.get(), width)
	    , data_(std::move(data))
	{
	}

	OwnedCelSprite(std::unique_ptr<byte[]> data, const int *widths)
	    : CelSprite(data.get(), widths)
	    , data_(std::move(data))
	{
	}

	OwnedCelSprite(OwnedCelSprite &&) noexcept = default;
	OwnedCelSprite &operator=(OwnedCelSprite &&) noexcept = default;

	[[nodiscard]] CelSprite Unowned() const
	{
		return CelSprite(*this);
	}

private:
	std::unique_ptr<byte[]> data_;
};

inline CelSprite::CelSprite(const OwnedCelSprite &owned)
    : CelSprite(static_cast<const CelSprite &>(owned))
{
}

} // namespace devilution
