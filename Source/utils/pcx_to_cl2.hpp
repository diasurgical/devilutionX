#pragma once

#include <cstdint>

#include <SDL.h>

#include "engine/cel_sprite.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

/**
 * @brief Loads a PCX file as a CL2 sprite.
 *
 * @param handle A non-null SDL_RWops handle. Closed by this function.
 * @param numFramesOrFrameHeight Pass a positive value with the number of frames, or the frame height as a negative value.
 * @param transparentColorIndex The PCX palette index of the transparent color.
 */
std::optional<OwnedCelSpriteWithFrameHeight> PcxToCl2(SDL_RWops *handle, int numFramesOrFrameHeight = 1, std::optional<uint8_t> transparentColor = std::nullopt, SDL_Color *outPalette = nullptr);

} // namespace devilution
