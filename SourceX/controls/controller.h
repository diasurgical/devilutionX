#pragma once

#include "controls/controller_buttons.h"

namespace dvl {

struct ControllerButtonEvent {

	ControllerButtonEvent()
	{
		button = ControllerButton_NONE;
		up = false;
	}

	ControllerButtonEvent(ControllerButton pButton, bool bUp)
	{
		button = pButton;
		up = bUp;
	}

	ControllerButton button;
	bool up;
};

ControllerButtonEvent ToControllerButtonEvent(const SDL_Event &event);

bool IsControllerButtonPressed(ControllerButton button);

void InitController();

} // namespace dvl
