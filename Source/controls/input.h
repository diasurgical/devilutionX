#pragma once

#include <SDL.h>

#include "controls/controller.h"

namespace devilution {

inline int PollEvent(SDL_Event *event)
{
	int result = SDL_PollEvent(event);
	if (result != 0)
		UnlockControllerState(*event);
	return result;
}

} // namespace devilution
