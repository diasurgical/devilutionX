#pragma once

#include <memory>
#include <utility>

#include "utils/pointer_value_union.hpp"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

class OwnedCelSprite;

/**
 * Stores a CEL or CL2 sprite and its width(s).
 * Does not own the data.
 */
class CelSprite {
public:
	CelSprite(const byte *data, uint16_t width)
	    : data_ptr_(data)
	    , width_(width)
	{
	}

	CelSprite(const byte *data, const uint16_t *widths)
	    : data_ptr_(data)
	    , width_(widths)
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

	[[nodiscard]] uint16_t Width(std::size_t frame = 0) const
	{
		return width_.HoldsPointer() ? width_.AsPointer()[frame] : width_.AsValue();
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
	PointerOrValue<uint16_t> width_;
};

/**
 * Stores a CEL or CL2 sprite and its width(s).
 * Owns the data.
 */
class OwnedCelSprite : public CelSprite {
public:
	OwnedCelSprite(std::unique_ptr<byte[]> data, uint16_t width)
	    : CelSprite(data.get(), width)
	    , data_(std::move(data))
	{
	}

	OwnedCelSprite(std::unique_ptr<byte[]> data, const uint16_t *widths)
	    : CelSprite(data.get(), widths)
	    , data_(std::move(data))
	{
	}

	OwnedCelSprite(OwnedCelSprite &&) noexcept = default;
	OwnedCelSprite &operator=(OwnedCelSprite &&) noexcept = default;

	[[nodiscard]] byte *MutableData()
	{
		return data_.get();
	}

private:
	std::unique_ptr<byte[]> data_;
};

inline CelSprite::CelSprite(const OwnedCelSprite &owned)
    : CelSprite(static_cast<const CelSprite &>(owned))
{
}

struct OwnedCelSpriteWithFrameHeight {
	OwnedCelSprite sprite;
	unsigned frameHeight;
};

} // namespace devilution
