#pragma once

#include <memory>
#include <utility>

#include "appfat.h"
#include "utils/pointer_value_union.hpp"
#include "utils/stdcompat/cstddef.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

class OwnedCelSprite;
class OptionalCelSprite;

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

	CelSprite(const byte *data, PointerOrValue<uint16_t> widths)
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

	[[nodiscard]] PointerOrValue<uint16_t> widthOrWidths() const
	{
		return width_;
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
	// for OptionalCelSprite
	CelSprite()
	    : data_ptr_(nullptr)
	    , width_(nullptr)
	{
	}

	const byte *data_ptr_;
	PointerOrValue<uint16_t> width_;

	friend class OptionalCelSprite;
};

/**
 * @brief Equivalent to `std::optional<CelSprite>` but smaller.
 */
class OptionalCelSprite {
public:
	OptionalCelSprite() = default;

	OptionalCelSprite(CelSprite sprite)
	    : sprite_(sprite)
	{
	}

	explicit OptionalCelSprite(const OwnedCelSprite &owned);

	OptionalCelSprite(std::nullopt_t)
	    : OptionalCelSprite()
	{
	}

	template <typename... Args>
	CelSprite &emplace(Args &&...args)
	{
		sprite_ = CelSprite(std::forward<Args>(args)...);
		return sprite_;
	}

	OptionalCelSprite &operator=(CelSprite sprite)
	{
		sprite_ = sprite;
		return *this;
	}

	OptionalCelSprite &operator=(std::nullopt_t)
	{
		sprite_ = {};
		return *this;
	}

	CelSprite operator*() const
	{
		assert(sprite_.data_ptr_ != nullptr);
		return sprite_;
	}

	CelSprite *operator->()
	{
		assert(sprite_.data_ptr_ != nullptr);
		return &sprite_;
	}

	const CelSprite *operator->() const
	{
		assert(sprite_.data_ptr_ != nullptr);
		return &sprite_;
	}

	operator bool() const
	{
		return sprite_.data_ptr_ != nullptr;
	}

private:
	CelSprite sprite_;
};

class OptionalOwnedCelSprite;

/**
 * Stores a CEL or CL2 sprite and its width(s).
 * Owns the data.
 */
class OwnedCelSprite {
public:
	OwnedCelSprite(std::unique_ptr<byte[]> data, uint16_t width)
	    : data_(std::move(data))
	    , width_(width)
	{
	}

	OwnedCelSprite(std::unique_ptr<byte[]> data, const uint16_t *widths)
	    : data_(std::move(data))
	    , width_(widths)
	{
	}

	OwnedCelSprite(std::unique_ptr<byte[]> data, PointerOrValue<uint16_t> widths)
	    : data_(std::move(data))
	    , width_(widths)
	{
	}

	OwnedCelSprite(OwnedCelSprite &&) noexcept = default;
	OwnedCelSprite &operator=(OwnedCelSprite &&) noexcept = default;

	[[nodiscard]] byte *MutableData()
	{
		return data_.get();
	}

	std::unique_ptr<byte[]> data() &&
	{
		return std::move(data_);
	}

private:
	// for OptionalOwnedCelSprite.
	OwnedCelSprite()
	    : data_(nullptr)
	    , width_(nullptr)
	{
	}

	std::unique_ptr<byte[]> data_;
	PointerOrValue<uint16_t> width_;

	friend class CelSprite;
	friend class OptionalOwnedCelSprite;
};

/**
 * @brief Equivalent to `std::optional<OwnedCelSprite>` but smaller.
 */
class OptionalOwnedCelSprite {
public:
	OptionalOwnedCelSprite() = default;

	OptionalOwnedCelSprite(OwnedCelSprite &&sprite)
	    : sprite_(std::move(sprite))
	{
	}

	OptionalOwnedCelSprite(std::nullopt_t)
	    : OptionalOwnedCelSprite()
	{
	}

	template <typename... Args>
	OwnedCelSprite &emplace(Args &&...args)
	{
		sprite_ = OwnedCelSprite(std::forward<Args>(args)...);
		return sprite_;
	}

	OptionalOwnedCelSprite &operator=(OwnedCelSprite &&sprite)
	{
		sprite_ = std::move(sprite);
		return *this;
	}

	OptionalOwnedCelSprite &operator=(std::nullopt_t)
	{
		sprite_ = {};
		return *this;
	}

	OwnedCelSprite &operator*()
	{
		assert(sprite_.data_ != nullptr);
		return sprite_;
	}

	const OwnedCelSprite &operator*() const
	{
		assert(sprite_.data_ != nullptr);
		return sprite_;
	}

	OwnedCelSprite *operator->()
	{
		assert(sprite_.data_ != nullptr);
		return &sprite_;
	}

	const OwnedCelSprite *operator->() const
	{
		assert(sprite_.data_ != nullptr);
		return &sprite_;
	}

	operator bool() const
	{
		return sprite_.data_ != nullptr;
	}

private:
	OwnedCelSprite sprite_;
};

inline CelSprite::CelSprite(const OwnedCelSprite &owned)
    : CelSprite(owned.data_.get(), owned.width_)
{
}

inline OptionalCelSprite::OptionalCelSprite(const OwnedCelSprite &owned)
{
	sprite_ = CelSprite { owned };
}

struct CelSpriteWithFrameHeight {
	CelSprite sprite;
	unsigned frameHeight;
};

struct OwnedCelSpriteWithFrameHeight {
	OwnedCelSprite sprite;
	unsigned frameHeight;
};

} // namespace devilution
