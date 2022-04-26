#pragma once

#include <cstdint>

#include "engine/cel_sprite.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

std::optional<OwnedCelSpriteWithFrameHeight> LoadPcxAssetAsCel(const char *filename, unsigned numFrames, bool generateFrameHeaders = false, uint8_t transparentColorIndex = 1);

} // namespace devilution
