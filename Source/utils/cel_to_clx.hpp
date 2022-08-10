#pragma once

#include <cstddef>
#include <cstdint>

#include <SDL.h>

#include "engine/clx_sprite.hpp"
#include "utils/pointer_value_union.hpp"

namespace devilution {

OwnedClxSpriteListOrSheet CelToClx(SDL_RWops *rwops, PointerOrValue<uint16_t> widthOrWidths);

} // namespace devilution
