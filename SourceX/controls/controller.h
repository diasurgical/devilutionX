#pragma once

#include "controls/controller_buttons.h"

namespace dvl {

struct ControllerButtonEvent {
	
	ControllerButtonEvent()
	{
		button = ControllerButtonNS::NONE;
		up = false;
	}

	ControllerButtonEvent(ControllerButtonNS::ControllerButton pButton, bool bUp)
	{
		button = pButton;
		up = bUp;
	}

	ControllerButtonNS::ControllerButton button;
	bool up;
};

ControllerButtonEvent ToControllerButtonEvent(const SDL_Event &event);

bool IsControllerButtonPressed(ControllerButtonNS::ControllerButton button);

void InitController();

} // namespace dvl
