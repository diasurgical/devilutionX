#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "engine/clx_sprite.hpp"
#include "utils/pointer_value_union.hpp"

namespace devilution {

OwnedClxSpriteListOrSheet CelToClx(std::string_view name, std::string_view trnName, const uint8_t *data, size_t size, PointerOrValue<uint16_t> widthOrWidths);

} // namespace devilution
