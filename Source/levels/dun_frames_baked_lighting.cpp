#include "levels/dun_frames_baked_lighting.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

#include "levels/dun_tile.hpp"
#include "lighting.h"
#include "utils/endian.hpp"

#if DEVILUTIONX_BAKED_LIGHT_DUNGEON_FRAMES_RAM_SIZE > 0
namespace devilution {
namespace {

std::unique_ptr<uint8_t[]> BakedLightData;
size_t BakedLightDataSize;

// Offsets into either original or baked light data.
using BakedLightDataRef = const uint8_t *;
std::unique_ptr<BakedLightDataRef[]> BakedLightDataRefs;

/**
 * @brief Applies a color map to a dungeon tile.
 *
 * @param src Dungeon CEL data.
 * @param tileType Dungeon CEL type.
 * @param tbl Palette mapping.
 * @param dst Output dungeon CEL data.
 */
void DunTileColorMap(std::span<const uint8_t> src, TileType tileType,
    const uint8_t *tbl, uint8_t *dst)
{
	if (tileType == TileType::TransparentSquare) {
		const uint8_t *srcPtr = src.data();
		for (auto i = 0; i < DunFrameHeight; ++i) {
			for (uint_fast8_t drawWidth = DunFrameWidth; drawWidth > 0;) {
				const uint8_t control = *srcPtr++;
				*dst++ = control;
				auto v = static_cast<int8_t>(control);
				if (v > 0) {
					drawWidth -= v;
					while (v-- > 0) {
						*dst++ = tbl[*srcPtr++];
					}
				} else {
					drawWidth += v;
				}
			}
		}
	} else {
		for (const uint8_t pix : src) {
			*dst++ = tbl[pix];
		}
	}
}

uint32_t GetFrameOffset(uint32_t frameIndex)
{
	return LoadLE32(&pDungeonCels[(frameIndex + 1) * 4]);
}

} // namespace

void BuildBakedLightData(size_t tileCount, size_t blocks)
{
	struct FrameInfo {
		uint16_t index;
		uint16_t size;
		uint16_t frequency = 0;
		TileType tileType;

		size_t ramUsage() const
		{
			return size * (LightsMax - 1);
		}

		bool operator<(const FrameInfo &other) const
		{
			return frequency < other.frequency;
		}
	};

	FreeBakedLightData();
	const size_t numFrames = LoadLE32(&pDungeonCels[0]);
	std::unique_ptr<FrameInfo[]> levelFrameInfo = std::make_unique<FrameInfo[]>(numFrames);

	// Populate `frequency` and `tileType` for each frame:
	for (size_t i = 0; i < tileCount / blocks; i++) {
		for (size_t block = 0; block < blocks; block++) {
			const LevelCelBlock &celBlock = DPieceMicros[i].mt[block];
			if (!celBlock.hasValue())
				continue;
			FrameInfo &info = levelFrameInfo[celBlock.frame() - 1];
			++info.frequency;
			info.tileType = celBlock.type();
		}
	}

	// Populate `index` and `size` for each frame:
	BakedLightDataRefs = std::unique_ptr<BakedLightDataRef[]>(new BakedLightDataRef[numFrames]);
	uint32_t frameBegin = GetFrameOffset(0);
	for (size_t i = 0; i < numFrames; ++i) {
		const uint32_t frameEnd = GetFrameOffset(i + 1);
		FrameInfo &info = levelFrameInfo[i];
		info.index = i;
		info.size = frameEnd - frameBegin;
		BakedLightDataRefs[i] = reinterpret_cast<const uint8_t *>(&pDungeonCels[frameBegin]);
		frameBegin = frameEnd;
	}

	// Figure out which frames to bake:
	// Take as many most frequent frames as we can fit in the RAM budget.
	std::make_heap(levelFrameInfo.get(), levelFrameInfo.get() + numFrames);
	size_t numToBake = 0;
	size_t remainingRam = DEVILUTIONX_BAKED_LIGHT_DUNGEON_FRAMES_RAM_SIZE;
	while (numToBake < numFrames) {
		const FrameInfo &info = levelFrameInfo[0];
		const size_t frameRam = info.ramUsage();
		if (frameRam > remainingRam)
			break;
		remainingRam -= frameRam;
		std::pop_heap(levelFrameInfo.get(), levelFrameInfo.get() + numFrames - numToBake);
		++numToBake;
	}
	const size_t bakedRamSize = DEVILUTIONX_BAKED_LIGHT_DUNGEON_FRAMES_RAM_SIZE - remainingRam;
	const std::span<const FrameInfo> framesToBake(levelFrameInfo.get() + (numFrames - numToBake), numToBake);

	// Bake the frames:
	uint32_t offset = 0;
	BakedLightData = std::unique_ptr<uint8_t[]>(new uint8_t[bakedRamSize]);
	BakedLightDataSize = bakedRamSize;

	for (const FrameInfo &info : framesToBake) {
		BakedLightDataRefs[info.index] = &BakedLightData[offset];
		const std::span<const uint8_t> src(
		    reinterpret_cast<const uint8_t *>(&pDungeonCels[GetFrameOffset(info.index)]),
		    info.size);
		// We do not need to bake light table 0 (fully lit) and LightsMax (fully dark).
		for (size_t lightTableIndex = 1; lightTableIndex < LightsMax; ++lightTableIndex) {
			DunTileColorMap(src, info.tileType, LightTables[lightTableIndex].data(), &BakedLightData[offset]);
			offset += info.size;
		}
	}
}

[[nodiscard]] const GetDungeonCelResult GetMaybeBakedDungeonCel(uint16_t celFrame, uint8_t lightTableIndex)
{
	const uint8_t *ptr = BakedLightDataRefs[celFrame];
	if (ptr >= BakedLightData.get() && ptr < BakedLightData.get() + BakedLightDataSize) {
		return { ptr + (lightTableIndex - 1) * (GetFrameOffset(celFrame + 1) - GetFrameOffset(celFrame)), true };
	}
	return { ptr, false };
}

void FreeBakedLightData()
{
	BakedLightDataRefs = nullptr;
	BakedLightData = nullptr;
}

} // namespace devilution
#endif
