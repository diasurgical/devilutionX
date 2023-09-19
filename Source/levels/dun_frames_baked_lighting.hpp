#pragma once

#include <cstddef>
#include <cstdint>

#include "levels/dun_tile.hpp"

namespace devilution {

#if DEVILUTIONX_BAKED_LIGHT_DUNGEON_FRAMES_RAM_SIZE > 0
struct GetDungeonCelResult {
	const uint8_t *ptr;
	bool baked;
};
[[nodiscard]] const GetDungeonCelResult GetMaybeBakedDungeonCel(uint16_t celFrame, uint8_t lightTableIndex);
void BuildBakedLightData(size_t tileCount, size_t blocks);
void FreeBakedLightData();
#else
inline void BuildBakedLightData(size_t tileCount, size_t blocks)
{
}
inline void FreeBakedLightData()
{
}
#endif

} // namespace devilution
