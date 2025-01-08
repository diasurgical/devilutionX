#pragma once

#include <cstdint>

#include "utils/enum_traits.h"

#define TILE_WIDTH 64
#define TILE_HEIGHT 32

namespace devilution {

/**
 * Level tile type.
 *
 * The tile type determines data encoding and the shape.
 *
 * Each tile type has its own encoding but they all encode data in the order
 * of bottom-to-top (bottom row first).
 */
enum class TileType : uint8_t {
	/**
	 * 🮆 A 32x32 square. Stored as an array of pixels.
	 */
	Square,

	/**
	 * 🮆 A 32x32 square with transparency. RLE encoded.
	 *
	 * Each run starts with an int8_t value.
	 * If positive, it is followed by this many pixels.
	 * If negative, it indicates `-value` fully transparent pixels, which are omitted.
	 *
	 * Runs do not cross row boundaries.
	 */
	TransparentSquare,

	/**
	 *🭮 Left-pointing 32x31 triangle. Encoded as 31 varying-width rows with 2 padding bytes before every even row.
	 * We remove the padding bytes in `ReencodeDungeonCels`.
	 *
	 * The smallest rows (bottom and top) are 2px wide, the largest row is 32px wide (middle row).
	 *
	 * Encoding:
	 * for i in [0, 30]:
	 * - 2 unused bytes if i is even
	 * - row (only the pixels within the triangle)
	 */
	LeftTriangle,

	/**
	 * 🭬Right-pointing 32x31 triangle.  Encoded as 31 varying-width rows with 2 padding bytes after every even row.
	 * We remove the padding bytes in `ReencodeDungeonCels`.
	 *
	 * The smallest rows (bottom and top) are 2px wide, the largest row is 32px wide (middle row).
	 *
	 * Encoding:
	 * for i in [0, 30]:
	 * - row (only the pixels within the triangle)
	 * - 2 unused bytes if i is even
	 */
	RightTriangle,

	/**
	 * 🭓 Left-pointing 32x32 trapezoid: a 32x16 rectangle and the 16x16 bottom part of `LeftTriangle`.
	 *
	 * Begins with triangle part, which uses the `LeftTriangle` encoding,
	 * and is followed by a flat array of pixels for the top rectangle part.
	 */
	LeftTrapezoid,

	/**
	 * 🭞 Right-pointing 32x32 trapezoid: 32x16 rectangle and the 16x16 bottom part of `RightTriangle`.
	 *
	 * Begins with the triangle part, which uses the `RightTriangle` encoding,
	 * and is followed by a flat array of pixels for the top rectangle part.
	 */
	RightTrapezoid,
};

/**
 * Specifies the current MIN block of the level CEL file, as used during rendering of the level tiles.
 */
struct LevelCelBlock {
	uint16_t data;

	[[nodiscard]] bool hasValue() const
	{
		return data != 0;
	}

	[[nodiscard]] TileType type() const
	{
		return static_cast<TileType>((data & 0x7000) >> 12);
	}

	/**
	 * @brief Returns the 1-based index of the frame in `pDungeonCels`.
	 */
	[[nodiscard]] uint16_t frame() const
	{
		return data & 0xFFF;
	}
};

enum class TileProperties : uint8_t {
	// clang-format off
	None             = 0,
	Solid            = 1 << 0,
	BlockLight       = 1 << 1,
	BlockMissile     = 1 << 2,
	Transparent      = 1 << 3,
	TransparentLeft  = 1 << 4,
	TransparentRight = 1 << 5,
	Trap             = 1 << 7,
	// clang-format on
};
use_enum_as_flags(TileProperties);

struct MICROS {
	LevelCelBlock mt[16];
};

/** Width of a tile rendering primitive. */
constexpr int_fast16_t DunFrameWidth = TILE_WIDTH / 2;

/** Height of a tile rendering primitive (except triangles). */
constexpr int_fast16_t DunFrameHeight = TILE_HEIGHT;

constexpr int_fast16_t DunFrameTriangleHeight = 31;

constexpr size_t ReencodedTriangleFrameSize = 544 - 32;
constexpr size_t ReencodedTrapezoidFrameSize = 800 - 16;

/**
 * @return Returns the center of the sprite relative to the center of the tile.
 */
constexpr int CalculateSpriteTileCenterX(int width)
{
	return (width - TILE_WIDTH) / 2;
}

} // namespace devilution
