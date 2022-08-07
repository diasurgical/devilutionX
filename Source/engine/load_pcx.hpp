#pragma once

#include <cstdint>

#include <SDL.h>

#include "engine/clx_sprite.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

/**
 * @brief Loads a PCX file as a CLX sprite list.
 *
 * @param filename
 * @param numFramesOrFrameHeight Pass a positive value with the number of frames, or the frame height as a negative value.
 * @param transparentColor
 * @param outPalette
 * @return OptionalOwnedClxSpriteList
 */
OptionalOwnedClxSpriteList LoadPcxSpriteList(const char *filename, int numFramesOrFrameHeight, std::optional<uint8_t> transparentColor = std::nullopt, SDL_Color *outPalette = nullptr);

/**
 * @brief Loads a PCX file as a CLX sprite list with a single sprite.
 *
 * @param filename
 * @param transparentColor
 * @param outPalette
 * @return OptionalOwnedClxSpriteList
 */
inline OptionalOwnedClxSpriteList LoadPcx(const char *filename, std::optional<uint8_t> transparentColor = std::nullopt, SDL_Color *outPalette = nullptr)
{
	return LoadPcxSpriteList(filename, 1, transparentColor, outPalette);
}

} // namespace devilution
