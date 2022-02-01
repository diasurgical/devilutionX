#pragma once

#include <SDL.h>

#include "controller_buttons.h"

namespace devilution {

enum ControllerButtonState : uint8_t {
	ControllerButtonState_RELEASED = SDL_RELEASED,
	ControllerButtonState_PRESSED = SDL_PRESSED,
	ControllerButtonState_HELD = SDL_PRESSED + SDL_RELEASED + 1 // for making it sure that the value is different
};

struct ControllerButtonEvent {
	ControllerButton button;
	bool up;
	uint8_t state;
};

// Must be called exactly once at the start of each SDL input event.
void UnlockControllerState(const SDL_Event &event);

ControllerButtonEvent ToControllerButtonEvent(const SDL_Event &event);

bool IsControllerButtonPressed(ControllerButton button);

bool HandleControllerAddedOrRemovedEvent(const SDL_Event &event);

void UnlockHeldControllerButtonEvents(const SDL_Event &event);

int PollActionButtonPressed(SDL_Event *event);

} // namespace devilution
