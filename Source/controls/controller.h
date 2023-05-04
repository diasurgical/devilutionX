#pragma once

#include <SDL.h>

#include "controller_buttons.h"
#include "utils/static_vector.hpp"

namespace devilution {

struct ControllerButtonEvent {
	ControllerButtonEvent(ControllerButton button, bool up)
	    : button(button)
	    , up(up)
	{
	}

	ControllerButton button;
	bool up;
};

// Must be called exactly once at the start of each SDL input event.
void UnlockControllerState(const SDL_Event &event);

StaticVector<ControllerButtonEvent, 4> ToControllerButtonEvents(const SDL_Event &event);

bool IsControllerButtonPressed(ControllerButton button);
bool IsControllerButtonComboPressed(ControllerButtonCombo combo);

bool HandleControllerAddedOrRemovedEvent(const SDL_Event &event);

} // namespace devilution
