#pragma once

#include <cstdint>

#include <SDL.h>

#include "engine/pcx_sprite.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

std::optional<OwnedPcxSprite> LoadPcxAsset(const char *path, SDL_Color *outPalette = nullptr);
std::optional<OwnedPcxSpriteSheet> LoadPcxSpriteSheetAsset(const char *path, uint16_t numFrames, SDL_Color *outPalette = nullptr);

} // namespace devilution
