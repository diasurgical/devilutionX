#pragma once

#ifdef __vita__

#include <SDL.h>

#include "engine/point.hpp"

namespace devilution {

void HandleTouchEvent(SDL_Event *event, Point mousePosition);
void FinishSimulatedMouseClicks(Point mousePosition);

} // namespace devilution

#endif
