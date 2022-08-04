#pragma once

#include <cstdint>

#include "engine/clx_sprite.hpp"
#include "engine/surface.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

/**
 * @brief Converts a Surface to a CLX sprite list.
 *
 * @param surface The source surface.
 * @param numFrames The number of vertically stacked frames in the surface.
 * @param transparentColor The PCX palette index of the transparent color.
 */
OwnedClxSpriteList SurfaceToClx(const Surface &surface, unsigned numFrames = 1,
    std::optional<uint8_t> transparentColor = std::nullopt);

} // namespace devilution
