#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <memory>
#include <vector>

#include "engine/clx_sprite.hpp"
#include "utils/pointer_value_union.hpp"

namespace devilution {

/**
 * @brief Converts CL2 to CLX in-place.
 *
 * @return uint16_t The number of lists in a sheet if it is a sheet, 0 otherwise.
 */
uint16_t Cl2ToClx(const uint8_t *data, size_t size,
    PointerOrValue<uint16_t> widthOrWidths, std::vector<uint8_t> &clxData);

inline OwnedClxSpriteListOrSheet Cl2ToClx(std::unique_ptr<uint8_t[]> &&data, size_t size, PointerOrValue<uint16_t> widthOrWidths)
{
	std::vector<uint8_t> clxData;
	const uint16_t numLists = Cl2ToClx(data.get(), size, widthOrWidths, clxData);
	data = nullptr;
	data = std::unique_ptr<uint8_t[]>(new uint8_t[clxData.size()]);
	memcpy(&data[0], clxData.data(), clxData.size());
	return OwnedClxSpriteListOrSheet { std::move(data), numLists };
}

} // namespace devilution
