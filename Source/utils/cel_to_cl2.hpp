#pragma once

#include <cstddef>
#include <cstdint>

#include "engine/cel_sprite.hpp"
#include "utils/pointer_value_union.hpp"

namespace devilution {

OwnedCelSprite CelToCl2(const uint8_t *data, size_t size, PointerOrValue<uint16_t> widthOrWidths);

} // namespace devilution
