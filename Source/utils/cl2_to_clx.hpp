#pragma once

#include <cstddef>
#include <cstdint>

#include <memory>

#include "engine/clx_sprite.hpp"
#include "utils/pointer_value_union.hpp"

namespace devilution {

/**
 * @brief Converts CL2 to CLX in-place.
 *
 * @return uint16_t The number of lists in a sheet if it is a sheet, 0 otherwise.
 */
uint16_t Cl2ToClx(uint8_t *data, size_t size, PointerOrValue<uint16_t> widthOrWidths);

inline OwnedClxSpriteListOrSheet Cl2ToClx(std::unique_ptr<uint8_t[]> &&data, size_t size, PointerOrValue<uint16_t> widthOrWidths)
{
	const uint16_t numLists = Cl2ToClx(data.get(), size, widthOrWidths);
	return OwnedClxSpriteListOrSheet { std::move(data), numLists };
}

} // namespace devilution
