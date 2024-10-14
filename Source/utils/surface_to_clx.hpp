#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "engine/clx_sprite.hpp"
#include "engine/surface.hpp"

namespace devilution {

/**
 * @brief Converts a Surface to a CLX sprite list.
 *
 * @param surface The source surface.
 * @param numFrames The number of vertically stacked frames in the surface.
 * @param transparentColor The PCX palette index of the transparent color.
 */
OwnedClxSpriteList SurfaceToClx(
#ifdef DEVILUTIONX_RESOURCE_TRACKING_ENABLED
    std::string &&name, std::string &&trnName,
#endif
    const Surface &surface, unsigned numFrames = 1,
    std::optional<uint8_t> transparentColor = std::nullopt);

} // namespace devilution
