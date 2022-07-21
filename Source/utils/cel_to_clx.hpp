#pragma once

#include <cstddef>
#include <cstdint>

#include "engine/clx_sprite.hpp"
#include "utils/pointer_value_union.hpp"

namespace devilution {

OwnedClxSpriteListOrSheet CelToClx(const uint8_t *data, size_t size, PointerOrValue<uint16_t> widthOrWidths);

} // namespace devilution
