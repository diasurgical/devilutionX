#pragma once

#include <SDL.h>

#include "controller_buttons.h"

namespace devilution {

struct ControllerButtonEvent {
	ControllerButton button;
	bool up;
};

// Must be called exactly once at the start of each SDL input event.
void UnlockControllerState(const SDL_Event &event);

ControllerButtonEvent ToControllerButtonEvent(const SDL_Event &event);

bool IsControllerButtonPressed(ControllerButton button);

bool HandleControllerAddedOrRemovedEvent(const SDL_Event &event);

} // namespace devilution
