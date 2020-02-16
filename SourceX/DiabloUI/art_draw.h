#pragma once

#include "all.h"

#include "DiabloUI/art.h"

namespace dvl {

void DrawArt(int screenX, int screenY, Art *art, int nFrame = 0,
    decltype(SDL_Rect().w) srcW = 0, decltype(SDL_Rect().h) srcH = 0);

void DrawAnimatedArt(Art *art, int screenX, int screenY);

int GetAnimationFrame(int frames, int fps = 60);

} // namespace dvl
