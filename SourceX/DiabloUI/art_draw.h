#pragma once

#include "all.h"

#include "../SourceX/DiabloUI/art.h"

namespace dvl {

void DrawArt(Sint16 screenX, Sint16 screenY, Art *art, int nFrame = 0, Uint16 srcW = 0, Uint16 srcH = 0);

void DrawArt(CelOutputBuffer out, Sint16 screenX, Sint16 screenY, Art *art, int nFrame = 0, Uint16 srcW = 0, Uint16 srcH = 0);

void DrawAnimatedArt(Art *art, int screenX, int screenY);

int GetAnimationFrame(int frames, int fps = 60);

} // namespace dvl
