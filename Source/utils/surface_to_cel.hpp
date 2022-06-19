#pragma once

#include "engine/cel_sprite.hpp"
#include "engine/surface.hpp"

namespace devilution {

OwnedCelSpriteWithFrameHeight SurfaceToCel(const Surface &surface, unsigned numFrames, bool generateFrameHeaders,
    uint8_t transparentColorIndex = 1);

} // namespace devilution
