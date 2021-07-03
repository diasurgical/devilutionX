#pragma once

#include "DiabloUI/art.h"
#include "engine.h"

namespace devilution {

void DrawArt(Sint16 screenX, Sint16 screenY, Art *art, int nFrame = 0, Uint16 srcW = 0, Uint16 srcH = 0);

void DrawArt(const Surface &out, Sint16 screenX, Sint16 screenY, Art *art, int nFrame = 0, Uint16 srcW = 0, Uint16 srcH = 0);

void DrawAnimatedArt(Art *art, int screenX, int screenY);

int GetAnimationFrame(int frames, int fps = 60);

} // namespace devilution
