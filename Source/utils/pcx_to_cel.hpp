#pragma once

#include <cstdint>

#include <SDL.h>

#include "engine/cel_sprite.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

/** @brief Loads a PCX file as CEL.
 *
 * Assumes that the PCX file does not have a palette.
 *
 * @param handle A non-null SDL_RWops handle. Closed by this function.
 * @param numFrames The number of vertically stacked frames in the PCX file.
 * @param generateFrameHeaders Whether to generate frame headers in the CEL sprite.
 * @param transparentColorIndex The PCX palette index of the transparent color.
 */
std::optional<OwnedCelSpriteWithFrameHeight> LoadPcxAsCel(SDL_RWops *handle, unsigned numFrames, bool generateFrameHeaders,
    uint8_t transparentColorIndex = 1);

} // namespace devilution
