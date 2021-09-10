#pragma once

#include <cstdint>
#include <memory>

#include <SDL_ttf.h>

#include "DiabloUI/art.h"

namespace devilution {

extern TTF_Font *font;

void LoadTtfFont();
void UnloadTtfFont();
void FontsCleanup();

} // namespace devilution
