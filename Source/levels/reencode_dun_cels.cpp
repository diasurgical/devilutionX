#include "levels/reencode_dun_cels.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <span>
#include <utility>

#include <SDL_endian.h>

#include "levels/dun_tile.hpp"
#include "utils/attributes.h"
#include "utils/endian.hpp"
#include "utils/format_int.hpp"
#include "utils/log.hpp"

namespace devilution {
namespace {

constexpr size_t LowerTriangleBloat = 16;
constexpr size_t TriangleBloat = 32;

DVL_ALWAYS_INLINE void ReencodeDungeonCelsLeftTriangleLower(uint8_t *&dst, const uint8_t *&src)
{
	unsigned width = 0;
	for (unsigned i = 0; i < 8; ++i) {
		src += 2; // Skips the two zero bytes (aka bloat).
		width += 2;
		std::memcpy(dst, src, width);
		src += width;
		dst += width;
		width += 2;
		std::memcpy(dst, src, width);
		src += width;
		dst += width;
	}
}

DVL_ALWAYS_INLINE void ReencodeDungeonCelsLeftTriangle(uint8_t *&dst, const uint8_t *&src)
{
	ReencodeDungeonCelsLeftTriangleLower(dst, src);
	unsigned width = DunFrameWidth;
	for (unsigned i = 0; i < 7; ++i) {
		src += 2; // Skips the two zero bytes (aka bloat).
		width -= 2;
		std::memcpy(dst, src, width);
		src += width;
		dst += width;
		width -= 2;
		std::memcpy(dst, src, width);
		src += width;
		dst += width;
	}
	src += 2; // Skips the two zero bytes (aka bloat).
	width -= 2;
	std::memcpy(dst, src, width);
	dst += width;
}

DVL_ALWAYS_INLINE void ReencodeDungeonCelsRightTriangleLower(uint8_t *&dst, const uint8_t *&src)
{
	unsigned width = 0;
	for (unsigned i = 0; i < 8; ++i) {
		width += 2;
		std::memcpy(dst, src, width);
		src += width + 2; // Skips the two zero bytes (aka bloat).
		dst += width;
		width += 2;
		std::memcpy(dst, src, width);
		src += width;
		dst += width;
	}
}

DVL_ALWAYS_INLINE void ReencodeDungeonCelsRightTriangle(uint8_t *&dst, const uint8_t *&src)
{
	ReencodeDungeonCelsRightTriangleLower(dst, src);
	unsigned width = DunFrameWidth;
	for (unsigned i = 0; i < 7; ++i) {
		width -= 2;
		std::memcpy(dst, src, width);
		src += width + 2; // Skips the two zero bytes (aka bloat).
		dst += width;
		width -= 2;
		std::memcpy(dst, src, width);
		src += width;
		dst += width;
	}
	width -= 2;
	std::memcpy(dst, src, width);
	dst += width;
}

DVL_ALWAYS_INLINE void ReencodeDungeonCelsLeftTrapezoid(uint8_t *&dst, const uint8_t *&src)
{
	ReencodeDungeonCelsLeftTriangleLower(dst, src);
	std::memcpy(dst, src, DunFrameWidth * 16);
	dst += DunFrameWidth * 16;
}

DVL_ALWAYS_INLINE void ReencodeDungeonCelsRightTrapezoid(uint8_t *&dst, const uint8_t *&src)
{
	ReencodeDungeonCelsRightTriangleLower(dst, src);
	std::memcpy(dst, src, DunFrameWidth * 16);
	dst += DunFrameWidth * 16;
}

size_t GetReencodedSize(const uint8_t *dungeonCels, std::span<std::pair<uint16_t, TileType>> frames)
{
	size_t result = (2 + frames.size()) * 4;
	const auto *srcOffsets = reinterpret_cast<const uint32_t *>(dungeonCels);
	for (const auto &[frame, type] : frames) {
		size_t frameSize;
		switch (type) {
		case TileType::TransparentSquare: {
			const uint32_t srcFrameBegin = SDL_SwapLE32(srcOffsets[frame]);
			const uint32_t srcFrameEnd = SDL_SwapLE32(srcOffsets[frame + 1]);
			frameSize = srcFrameEnd - srcFrameBegin;
		} break;
		case TileType::Square: {
			frameSize = DunFrameWidth * DunFrameHeight;
		} break;
		case TileType::LeftTriangle:
		case TileType::RightTriangle:
			frameSize = 544 - TriangleBloat;
			break;
		case TileType::LeftTrapezoid:
		case TileType::RightTrapezoid:
			frameSize = 800 - LowerTriangleBloat;
			break;
		}
		result += frameSize;
	}
	return result;
}

} // namespace

void ReencodeDungeonCels(std::unique_ptr<std::byte[]> &dungeonCels, std::span<std::pair<uint16_t, TileType>> frames)
{
	const auto *srcData = reinterpret_cast<const uint8_t *>(dungeonCels.get());
	const auto *srcOffsets = reinterpret_cast<const uint32_t *>(srcData);

	LogVerbose("Re-encoding dungeon CELs: {} frames, {} bytes",
	    FormatInteger(SDL_SwapLE32(srcOffsets[0])),
	    FormatInteger(SDL_SwapLE32(srcOffsets[SDL_SwapLE32(srcOffsets[0]) + 1])));

	const size_t outSize = GetReencodedSize(srcData, frames);
	std::unique_ptr<std::byte[]> result { new std::byte[outSize] };
	auto *const resultPtr = reinterpret_cast<uint8_t *>(result.get());
	WriteLE32(resultPtr, frames.size());
	uint8_t *out = resultPtr + (2 + frames.size()) * 4; // number of frames, frame offsets, file size
	for (const auto &[frame, type] : frames) {
		WriteLE32(&resultPtr[static_cast<size_t>(frame * 4)], out - resultPtr);
		const uint32_t srcFrameBegin = SDL_SwapLE32(srcOffsets[frame]);
		const uint8_t *src = &srcData[srcFrameBegin];
		switch (type) {
		case TileType::TransparentSquare: {
			const uint32_t srcFrameEnd = SDL_SwapLE32(srcOffsets[frame + 1]);
			const uint32_t size = srcFrameEnd - srcFrameBegin;
			std::memcpy(out, src, size);
			out += size;
		} break;
		case TileType::Square:
			std::memcpy(out, src, DunFrameWidth * DunFrameHeight);
			out += DunFrameWidth * DunFrameHeight;
			break;
		case TileType::LeftTriangle:
			ReencodeDungeonCelsLeftTriangle(out, src);
			break;
		case TileType::RightTriangle:
			ReencodeDungeonCelsRightTriangle(out, src);
			break;
		case TileType::LeftTrapezoid:
			ReencodeDungeonCelsLeftTrapezoid(out, src);
			break;
		case TileType::RightTrapezoid:
			ReencodeDungeonCelsRightTrapezoid(out, src);
			break;
		}
	}
	WriteLE32(&resultPtr[(1 + frames.size()) * 4], outSize);

	const auto *dstOffsets = reinterpret_cast<const uint32_t *>(resultPtr);
	LogVerbose(" Re-encoded dungeon CELs: {} frames, {} bytes",
	    FormatInteger(SDL_SwapLE32(dstOffsets[0])),
	    FormatInteger(SDL_SwapLE32(dstOffsets[SDL_SwapLE32(dstOffsets[0]) + 1])));

	dungeonCels = std::move(result);
}

} // namespace devilution
