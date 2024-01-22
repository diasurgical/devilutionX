#pragma once

#include <cstdint>

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
	 *
	 * The smallest rows (bottom and top) are 2px wide, the largest row is 16px wide (middle row).
	 *
	 * Encoding:
	 * for i in [0, 30]:
	 * - 2 unused bytes if i is even
	 * - row (only the pixels within the triangle)
	 */
	LeftTriangle,

	/**
	 * 🭬Right-pointing 32x31 triangle.  Encoded as 31 varying-width rows with 2 padding bytes after every even row.
	 *
	 * The smallest rows (bottom and top) are 2px wide, the largest row is 16px wide (middle row).
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

/** Width of a tile rendering primitive. */
constexpr int_fast16_t DunFrameWidth = TILE_WIDTH / 2;

/** Height of a tile rendering primitive (except triangles). */
constexpr int_fast16_t DunFrameHeight = TILE_HEIGHT;

} // namespace devilution
