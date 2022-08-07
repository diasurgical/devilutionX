#pragma once
/**
 * @file clx_sprite.hpp
 *
 * @brief CLX format sprites.
 *
 * CLX is a format used for DevilutionX graphics at runtime.
 *
 * It is identical to CL2, except we use the frame header to store the frame's width and height.
 *
 * CLX frame header (10 bytes, same as CL2):
 *
 *      Bytes |   Type   | Value
 *     :-----:|:--------:|------------------------------------
 *      0..2  | uint16_t | offset to data start (same as CL2)
 *      2..4  | uint16_t | width
 *      4..6  | uint16_t | height
 *      6..10 |    -     | unused
 *
 * The CLX format is otherwise identical to CL2.
 *
 * Since the header is identical to CL2, CL2 can be converted to CLX without reallocation.
 *
 * CL2 reference: https://github.com/savagesteel/d1-file-formats/blob/master/PC-Mac/CL2.md#2-file-structure
 */

#include <cstddef>
#include <cstdint>

#include <iterator>
#include <memory>

#include "appfat.h"
#include "utils/endian.hpp"
#include "utils/intrusive_optional.hpp"

namespace devilution {

class OptionalClxSprite;

/**
 * @brief A single CLX sprite.
 */
class ClxSprite {
	static constexpr uint32_t HeaderSize = 10;

public:
	explicit constexpr ClxSprite(const uint8_t *data, uint32_t dataSize)
	    : data_(data)
	    , pixel_data_size_(dataSize - HeaderSize)
	{
		assert(data != nullptr);
	}

	[[nodiscard]] constexpr uint16_t width() const
	{
		return LoadLE16(&data_[2]);
	}

	[[nodiscard]] constexpr uint16_t height() const
	{
		return LoadLE16(&data_[4]);
	}

	/**
	 * @brief The raw pixel data (CL2 frame data).
	 *
	 * Format: https://github.com/savagesteel/d1-file-formats/blob/master/PC-Mac/CL2.md#42-cl2-frame-data
	 */
	[[nodiscard]] constexpr const uint8_t *pixelData() const
	{
		return &data_[HeaderSize];
	}

	[[nodiscard]] constexpr uint32_t pixelDataSize() const
	{
		return pixel_data_size_;
	}

	constexpr bool operator==(const ClxSprite &other) const
	{
		return data_ == other.data_;
	}

	constexpr bool operator!=(const ClxSprite &other) const
	{
		return !(*this == other);
	}

private:
	// For OptionalClxSprite.
	constexpr ClxSprite()
	    : data_(nullptr)
	    , pixel_data_size_(0)
	{
	}

	const uint8_t *data_;
	uint32_t pixel_data_size_;

	friend class OptionalClxSprite;
};

class OwnedClxSpriteList;
class OptionalClxSpriteList;
class ClxSpriteListIterator;

/**
 * @brief A list of `ClxSprite`s.
 */
class ClxSpriteList {
public:
	explicit constexpr ClxSpriteList(const uint8_t *data)
	    : data_(data)
	{
		assert(data != nullptr);
	}

	ClxSpriteList(const OwnedClxSpriteList &owned);

	[[nodiscard]] OwnedClxSpriteList clone() const;

	[[nodiscard]] constexpr uint32_t numSprites() const
	{
		return LoadLE32(data_);
	}

	[[nodiscard]] constexpr ClxSprite operator[](size_t spriteIndex) const
	{
		assert(spriteIndex < numSprites());
		const uint32_t begin = spriteOffset(spriteIndex);
		const uint32_t end = spriteOffset(spriteIndex + 1);
		return ClxSprite { &data_[begin], end - begin };
	}

	[[nodiscard]] constexpr uint32_t spriteOffset(size_t spriteIndex) const
	{
		return LoadLE32(&data_[4 + spriteIndex * 4]);
	}

	/** @brief The offset to the next sprite sheet, or file size if this is the last sprite sheet. */
	[[nodiscard]] constexpr uint32_t nextSpriteSheetOffsetOrFileSize() const
	{
		return LoadLE32(&data_[4 + numSprites() * 4]);
	}

	[[nodiscard]] constexpr const uint8_t *data() const
	{
		return data_;
	}

	[[nodiscard]] constexpr ClxSpriteListIterator begin() const;
	[[nodiscard]] constexpr ClxSpriteListIterator end() const;

private:
	// For OptionalClxSpriteList.
	constexpr ClxSpriteList()
	    : data_(nullptr)
	{
	}

	const uint8_t *data_;

	friend class OptionalClxSpriteList;
};

class ClxSpriteListIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using difference_type = int;
	using value_type = ClxSprite;
	using pointer = void;
	using reference = value_type &;

	constexpr ClxSpriteListIterator(ClxSpriteList list, size_t index)
	    : list_(list)
	    , index_(index)
	{
	}

	constexpr ClxSprite operator*()
	{
		return list_[index_];
	}

	constexpr ClxSpriteListIterator &operator++()
	{
		++index_;
		return *this;
	}

	constexpr ClxSpriteListIterator operator++(int)
	{
		auto copy = *this;
		++(*this);
		return copy;
	}

	constexpr bool operator==(const ClxSpriteListIterator &other) const
	{
		return index_ == other.index_;
	}

	constexpr bool operator!=(const ClxSpriteListIterator &other) const
	{
		return !(*this == other);
	}

private:
	ClxSpriteList list_;
	size_t index_;
};

inline constexpr ClxSpriteListIterator ClxSpriteList::begin() const
{
	return { *this, 0 };
}

inline constexpr ClxSpriteListIterator ClxSpriteList::end() const
{
	return { *this, numSprites() };
}

class OwnedClxSpriteSheet;
class OptionalClxSpriteSheet;
class ClxSpriteSheetIterator;

/**
 * @brief A sprite sheet is a list of `ClxSpriteList`s.
 */
class ClxSpriteSheet {
public:
	explicit constexpr ClxSpriteSheet(const uint8_t *data, uint16_t numLists)
	    : data_(data)
	    , num_lists_(numLists)
	{
		assert(data != nullptr);
		assert(num_lists_ > 0);
	}

	ClxSpriteSheet(const OwnedClxSpriteSheet &owned);

	[[nodiscard]] constexpr uint16_t numLists() const
	{
		return num_lists_;
	}

	[[nodiscard]] constexpr ClxSpriteList operator[](size_t sheetIndex) const
	{
		return ClxSpriteList { &data_[sheetOffset(sheetIndex)] };
	}

	[[nodiscard]] constexpr uint32_t sheetOffset(size_t sheetIndex) const
	{
		assert(sheetIndex < num_lists_);
		return LoadLE32(&data_[4 * sheetIndex]);
	}

	[[nodiscard]] constexpr const uint8_t *data() const
	{
		return data_;
	}

	[[nodiscard]] constexpr ClxSpriteSheetIterator begin() const;
	[[nodiscard]] constexpr ClxSpriteSheetIterator end() const;

private:
	// For OptionalClxSpriteSheet.
	constexpr ClxSpriteSheet()
	    : data_(nullptr)
	    , num_lists_(0)
	{
	}

	const uint8_t *data_;
	uint16_t num_lists_;

	friend class OptionalClxSpriteSheet;
};

class ClxSpriteSheetIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using difference_type = int;
	using value_type = ClxSpriteList;
	using pointer = void;
	using reference = value_type &;

	constexpr ClxSpriteSheetIterator(ClxSpriteSheet sheet, size_t index)
	    : sheet_(sheet)
	    , index_(index)
	{
	}

	constexpr ClxSpriteList operator*()
	{
		return sheet_[index_];
	}

	constexpr ClxSpriteSheetIterator &operator++()
	{
		++index_;
		return *this;
	}

	constexpr ClxSpriteSheetIterator operator++(int)
	{
		auto copy = *this;
		++(*this);
		return copy;
	}

	constexpr bool operator==(const ClxSpriteSheetIterator &other) const
	{
		return index_ == other.index_;
	}

	constexpr bool operator!=(const ClxSpriteSheetIterator &other) const
	{
		return !(*this == other);
	}

private:
	ClxSpriteSheet sheet_;
	size_t index_;
};

inline constexpr ClxSpriteSheetIterator ClxSpriteSheet::begin() const
{
	return { *this, 0 };
}

inline constexpr ClxSpriteSheetIterator ClxSpriteSheet::end() const
{
	return { *this, num_lists_ };
}

class OptionalOwnedClxSpriteList;
class OwnedClxSpriteListOrSheet;

/**
 * @brief Implicitly convertible to `ClxSpriteList` and owns its data.
 */
class OwnedClxSpriteList {
public:
	explicit OwnedClxSpriteList(std::unique_ptr<uint8_t[]> &&data)
	    : data_(std::move(data))
	{
		assert(data_ != nullptr);
	}

	OwnedClxSpriteList(OwnedClxSpriteList &&) noexcept = default;
	OwnedClxSpriteList &operator=(OwnedClxSpriteList &&) noexcept = default;

	[[nodiscard]] OwnedClxSpriteList clone() const
	{
		return ClxSpriteList { *this }.clone();
	}

	[[nodiscard]] ClxSprite operator[](size_t spriteIndex) const
	{
		return ClxSpriteList { *this }[spriteIndex];
	}

private:
	// For OptionalOwnedClxSpriteList.
	OwnedClxSpriteList() = default;

	std::unique_ptr<uint8_t[]> data_;

	friend class ClxSpriteList; // for implicit conversion
	friend class OptionalOwnedClxSpriteList;
	friend class OwnedClxSpriteListOrSheet;
};

inline ClxSpriteList::ClxSpriteList(const OwnedClxSpriteList &owned)
    : data_(owned.data_.get())
{
}

inline OwnedClxSpriteList ClxSpriteList::clone() const
{
	const size_t dataSize = nextSpriteSheetOffsetOrFileSize();
	std::unique_ptr<uint8_t[]> data { new uint8_t[dataSize] };
	memcpy(data.get(), data_, dataSize);
	return OwnedClxSpriteList { std::move(data) };
}

/**
 * @brief Implicitly convertible to `ClxSpriteSheet` and owns its data.
 */
class OwnedClxSpriteSheet {
public:
	OwnedClxSpriteSheet(std::unique_ptr<uint8_t[]> &&data, uint16_t numLists)
	    : data_(std::move(data))
	    , num_lists_(numLists)
	{
		assert(data_ != nullptr);
		assert(numLists > 0);
	}

	OwnedClxSpriteSheet(OwnedClxSpriteSheet &&) noexcept = default;
	OwnedClxSpriteSheet &operator=(OwnedClxSpriteSheet &&) noexcept = default;

	[[nodiscard]] ClxSpriteList operator[](size_t sheetIndex) const
	{
		return ClxSpriteSheet { *this }[sheetIndex];
	}

	[[nodiscard]] ClxSpriteSheetIterator begin() const
	{
		return ClxSpriteSheet { *this }.begin();
	}

	[[nodiscard]] ClxSpriteSheetIterator end() const
	{
		return ClxSpriteSheet { *this }.end();
	}

private:
	// For OptionalOwnedClxSpriteList.
	OwnedClxSpriteSheet()
	    : data_(nullptr)
	    , num_lists_(0)
	{
	}

	std::unique_ptr<uint8_t[]> data_;
	uint16_t num_lists_;

	friend class ClxSpriteSheet; // for implicit conversion.
	friend class OptionalOwnedClxSpriteSheet;
	friend class OwnedClxSpriteListOrSheet;
};

inline ClxSpriteSheet::ClxSpriteSheet(const OwnedClxSpriteSheet &owned)
    : data_(owned.data_.get())
    , num_lists_(owned.num_lists_)
{
}

class OwnedClxSpriteListOrSheet;
class OptionalClxSpriteListOrSheet;

/**
 * @brief A CLX sprite list or a sprite sheet (list of lists).
 */
class ClxSpriteListOrSheet {
public:
	constexpr ClxSpriteListOrSheet(const uint8_t *data, uint16_t numLists)
	    : data_(data)
	    , num_lists_(numLists)
	{
	}

	ClxSpriteListOrSheet(const OwnedClxSpriteListOrSheet &listOrSheet);

	[[nodiscard]] constexpr ClxSpriteList list() const
	{
		assert(num_lists_ == 0);
		return ClxSpriteList { data_ };
	}

	[[nodiscard]] constexpr ClxSpriteSheet sheet() const
	{
		assert(num_lists_ != 0);
		return ClxSpriteSheet { data_, num_lists_ };
	}

	[[nodiscard]] constexpr bool isSheet() const
	{
		return num_lists_ != 0;
	}

private:
	const uint8_t *data_;
	uint16_t num_lists_;

	// For OptionalClxSpriteListOrSheet.
	constexpr ClxSpriteListOrSheet()
	    : data_(nullptr)
	    , num_lists_(0)
	{
	}

	friend class OptionalClxSpriteListOrSheet;
};

class OptionalOwnedClxSpriteListOrSheet;

/**
 * @brief A CLX sprite list or a sprite sheet (list of lists).
 */
class OwnedClxSpriteListOrSheet {
public:
	explicit OwnedClxSpriteListOrSheet(std::unique_ptr<uint8_t[]> &&data, uint16_t numLists = 0)
	    : data_(std::move(data))
	    , num_lists_(numLists)
	{
	}

	explicit OwnedClxSpriteListOrSheet(OwnedClxSpriteSheet &&sheet)
	    : data_(std::move(sheet.data_))
	    , num_lists_(sheet.num_lists_)
	{
	}

	explicit OwnedClxSpriteListOrSheet(OwnedClxSpriteList &&list)
	    : data_(std::move(list.data_))
	    , num_lists_(0)
	{
	}

	[[nodiscard]] ClxSpriteList list() const &
	{
		assert(num_lists_ == 0);
		return ClxSpriteList { data_.get() };
	}

	[[nodiscard]] OwnedClxSpriteList list() &&
	{
		assert(num_lists_ == 0);
		return OwnedClxSpriteList { std::move(data_) };
	}

	[[nodiscard]] ClxSpriteSheet sheet() const &
	{
		assert(num_lists_ != 0);
		return ClxSpriteSheet { data_.get(), num_lists_ };
	}

	[[nodiscard]] OwnedClxSpriteSheet sheet() &&
	{
		assert(num_lists_ != 0);
		return OwnedClxSpriteSheet { std::move(data_), num_lists_ };
	}

	[[nodiscard]] bool isSheet() const
	{
		return num_lists_ != 0;
	}

private:
	std::unique_ptr<uint8_t[]> data_;
	uint16_t num_lists_;

	// For OptionalOwnedClxSpriteListOrSheet.
	OwnedClxSpriteListOrSheet()
	    : data_(nullptr)
	    , num_lists_(0)
	{
	}

	friend class ClxSpriteListOrSheet;
	friend class OptionalOwnedClxSpriteListOrSheet;
};

inline ClxSpriteListOrSheet::ClxSpriteListOrSheet(const OwnedClxSpriteListOrSheet &listOrSheet)
    : data_(listOrSheet.data_.get())
    , num_lists_(listOrSheet.num_lists_)
{
}

/**
 * @brief Equivalent to `std::optional<ClxSprite>` but smaller.
 */
class OptionalClxSprite {
	DEFINE_CONSTEXPR_INTRUSIVE_OPTIONAL(OptionalClxSprite, ClxSprite, data_, nullptr)
};

/**
 * @brief Equivalent to `std::optional<ClxSpriteList>` but smaller.
 */
class OptionalClxSpriteList {
	DEFINE_CONSTEXPR_INTRUSIVE_OPTIONAL(OptionalClxSpriteList, ClxSpriteList, data_, nullptr)
};

/**
 * @brief Equivalent to `std::optional<ClxSpriteSheet>` but smaller.
 */
class OptionalClxSpriteSheet {
	DEFINE_CONSTEXPR_INTRUSIVE_OPTIONAL(OptionalClxSpriteSheet, ClxSpriteSheet, data_, nullptr)
};

/**
 * @brief Equivalent to `std::optional<ClxSpriteListOrSheet>` but smaller.
 */
class OptionalClxSpriteListOrSheet {
public:
	DEFINE_INTRUSIVE_OPTIONAL(OptionalClxSpriteListOrSheet, ClxSpriteListOrSheet, data_, nullptr);
};

/**
 * @brief Equivalent to `std::optional<OwnedClxSpriteList>` but smaller.
 */
class OptionalOwnedClxSpriteList {
public:
	DEFINE_INTRUSIVE_OPTIONAL(OptionalOwnedClxSpriteList, OwnedClxSpriteList, data_, nullptr)
};

/**
 * @brief Equivalent to `std::optional<OwnedClxSpriteSheet>` but smaller.
 */
class OptionalOwnedClxSpriteSheet {
public:
	DEFINE_INTRUSIVE_OPTIONAL(OptionalOwnedClxSpriteSheet, OwnedClxSpriteSheet, data_, nullptr)
};

class OptionalOwnedClxSpriteListOrSheet {
public:
	DEFINE_INTRUSIVE_OPTIONAL(OptionalOwnedClxSpriteListOrSheet, OwnedClxSpriteListOrSheet, data_, nullptr);
};

} // namespace devilution
