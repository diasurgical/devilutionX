#pragma once

#include <cstdint>
#include <memory>
#include <utility>

#include "utils/stdcompat/optional.hpp"

namespace devilution {

class OwnedPcxSprite;
class OwnedPcxSpriteSheet;

/**
 * @brief An 8-bit PCX sprite.
 */
class PcxSprite {
public:
	PcxSprite(const uint8_t *data, uint16_t width, uint16_t height, std::optional<uint8_t> transparentColor = std::nullopt)
	    : data_(data)
	    , width_(width)
	    , height_(height)
	    , transparent_color_(transparentColor)
	{
	}

	explicit PcxSprite(const OwnedPcxSprite &owned);

	[[nodiscard]] const uint8_t *data() const
	{
		return data_;
	}

	[[nodiscard]] uint16_t width() const
	{
		return width_;
	}

	[[nodiscard]] uint16_t height() const
	{
		return height_;
	}

	[[nodiscard]] std::optional<uint8_t> transparentColor() const
	{
		return transparent_color_;
	}

private:
	const uint8_t *data_;
	uint16_t width_;
	uint16_t height_;
	std::optional<uint8_t> transparent_color_;
};

class OwnedPcxSprite {
public:
	OwnedPcxSprite(std::unique_ptr<uint8_t[]> &&data, uint16_t width, uint16_t height, std::optional<uint8_t> transparentColor = std::nullopt)
	    : data_(std::move(data))
	    , width_(width)
	    , height_(height)
	    , transparent_color_(transparentColor)
	{
	}

	OwnedPcxSprite(OwnedPcxSprite &&) noexcept = default;
	OwnedPcxSprite &operator=(OwnedPcxSprite &&) noexcept = default;

private:
	std::unique_ptr<uint8_t[]> data_;
	uint16_t width_;
	uint16_t height_;
	std::optional<uint8_t> transparent_color_;

	friend class PcxSprite;
	friend class OwnedPcxSpriteSheet;
};

inline PcxSprite::PcxSprite(const OwnedPcxSprite &owned)
    : PcxSprite(owned.data_.get(), owned.width_, owned.height_, owned.transparent_color_)
{
}

/**
 * @brief An 8-bit PCX sprite sheet consisting of vertically stacked frames.
 */
class PcxSpriteSheet {
public:
	PcxSpriteSheet(const uint8_t *data, const uint32_t *frameOffsets, uint16_t numFrames, uint16_t width, uint16_t frameHeight, std::optional<uint8_t> transparentColor = std::nullopt)
	    : data_(data)
	    , frame_offsets_(frameOffsets)
	    , num_frames_(numFrames)
	    , width_(width)
	    , frame_height_(frameHeight)
	    , transparent_color_(transparentColor)
	{
	}

	explicit PcxSpriteSheet(const OwnedPcxSpriteSheet &owned);

	[[nodiscard]] PcxSprite sprite(uint16_t frame) const
	{
		return PcxSprite { data_ + frame_offsets_[frame], width_, frame_height_, transparent_color_ };
	}

	[[nodiscard]] uint16_t numFrames() const
	{
		return num_frames_;
	}

	[[nodiscard]] uint16_t width() const
	{
		return width_;
	}

	[[nodiscard]] uint16_t frameHeight() const
	{
		return frame_height_;
	}

private:
	const uint8_t *data_;
	const uint32_t *frame_offsets_;
	uint16_t num_frames_;
	uint16_t width_;
	uint16_t frame_height_;
	std::optional<uint8_t> transparent_color_;
};

class OwnedPcxSpriteSheet {
public:
	OwnedPcxSpriteSheet(std::unique_ptr<uint8_t[]> &&data, std::unique_ptr<uint32_t[]> &&frameOffsets, uint16_t numFrames, uint16_t width, uint16_t frameHeight, std::optional<uint8_t> transparentColor = std::nullopt)
	    : data_(std::move(data))
	    , frame_offsets_(std::move(frameOffsets))
	    , num_frames_(numFrames)
	    , width_(width)
	    , frame_height_(frameHeight)
	    , transparent_color_(transparentColor)
	{
	}

	OwnedPcxSpriteSheet(OwnedPcxSprite &&sprite, std::unique_ptr<uint32_t[]> &&frameOffsets, uint16_t numFrames)
	    : OwnedPcxSpriteSheet(std::move(sprite.data_), std::move(frameOffsets), numFrames, sprite.width_, sprite.height_ / numFrames, sprite.transparent_color_)
	{
	}

	OwnedPcxSpriteSheet(OwnedPcxSpriteSheet &&) noexcept = default;
	OwnedPcxSpriteSheet &operator=(OwnedPcxSpriteSheet &&) noexcept = default;

private:
	std::unique_ptr<uint8_t[]> data_;
	std::unique_ptr<uint32_t[]> frame_offsets_;
	uint16_t num_frames_;
	uint16_t width_;
	uint16_t frame_height_;
	std::optional<uint8_t> transparent_color_;

	friend class PcxSpriteSheet;
};

inline PcxSpriteSheet::PcxSpriteSheet(const OwnedPcxSpriteSheet &owned)
    : PcxSpriteSheet(owned.data_.get(), owned.frame_offsets_.get(), owned.num_frames_, owned.width_, owned.frame_height_, owned.transparent_color_)
{
}

} // namespace devilution
