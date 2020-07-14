#pragma once

#include "controls/controller_buttons.h"

namespace dvl {

struct ControllerButtonEvent {
	ControllerButton button;
	bool up;
};

ControllerButtonEvent ToControllerButtonEvent(const SDL_Event &event);

bool IsControllerButtonPressed(ControllerButton button);

void InitController();

} // namespace dvl
