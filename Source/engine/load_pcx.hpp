#pragma once

#include <cstdint>

#include <SDL.h>

#include "engine/cel_sprite.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

/**
 * @brief Loads a PCX file as a CL2 sprite sheet.
 *
 * @param filename
 * @param numFramesOrFrameHeight Pass a positive value with the number of frames, or the frame height as a negative value.
 * @param transparentColor
 * @param outPalette
 * @return std::optional<OwnedCelSpriteSheetWithFrameHeight>
 */
std::optional<OwnedCelSpriteSheetWithFrameHeight> LoadPcxSpriteSheetAsCl2(const char *filename, int numFramesOrFrameHeight, std::optional<uint8_t> transparentColor = std::nullopt, SDL_Color *outPalette = nullptr);

std::optional<OwnedCelSpriteWithFrameHeight> LoadPcxAsCl2(const char *filename, std::optional<uint8_t> transparentColor = std::nullopt, SDL_Color *outPalette = nullptr);

} // namespace devilution
