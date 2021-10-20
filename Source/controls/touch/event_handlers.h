#pragma once

#if defined(VIRTUAL_GAMEPAD) && !defined(USE_SDL1)

#include <SDL.h>

#include "controls/touch/gamepad.h"

namespace devilution {

class VirtualDirectionPadEventHandler {
public:
	VirtualDirectionPadEventHandler(VirtualDirectionPad *virtualDirectionPad)
	    : virtualDirectionPad(virtualDirectionPad)
	    , activeFinger(0)
	    , isActive(false)
	{
	}

	bool Handle(const SDL_Event &event);

private:
	VirtualDirectionPad *virtualDirectionPad;
	SDL_FingerID activeFinger;
	bool isActive;

	bool HandleFingerDown(const SDL_TouchFingerEvent &event);
	bool HandleFingerUp(const SDL_TouchFingerEvent &event);
	bool HandleFingerMotion(const SDL_TouchFingerEvent &event);
};

class VirtualPadButtonEventHandler {
public:
	VirtualPadButtonEventHandler(VirtualPadButton *virtualPadButton)
	    : virtualPadButton(virtualPadButton)
	    , activeFinger(0)
	    , isActive(false)
	{
	}

	bool Handle(const SDL_Event &event);

private:
	VirtualPadButton *virtualPadButton;
	SDL_FingerID activeFinger;
	bool isActive;

	bool HandleFingerDown(const SDL_TouchFingerEvent &event);
	bool HandleFingerUp(const SDL_TouchFingerEvent &event);
	bool HandleFingerMotion(const SDL_TouchFingerEvent &event);
};

class VirtualGamepadEventHandler {
public:
	VirtualGamepadEventHandler(VirtualGamepad *virtualGamepad)
	    : directionPadEventHandler(&virtualGamepad->directionPad)
	    , primaryActionButtonEventHandler(&virtualGamepad->primaryActionButton)
	    , secondaryActionButtonEventHandler(&virtualGamepad->secondaryActionButton)
	    , spellActionButtonEventHandler(&virtualGamepad->spellActionButton)
	    , cancelButtonEventHandler(&virtualGamepad->cancelButton)
	    , healthButtonEventHandler(&virtualGamepad->healthButton)
	    , manaButtonEventHandler(&virtualGamepad->manaButton)
	{
	}

	bool Handle(const SDL_Event &event);

private:
	VirtualDirectionPadEventHandler directionPadEventHandler;

	VirtualPadButtonEventHandler primaryActionButtonEventHandler;
	VirtualPadButtonEventHandler secondaryActionButtonEventHandler;
	VirtualPadButtonEventHandler spellActionButtonEventHandler;
	VirtualPadButtonEventHandler cancelButtonEventHandler;

	VirtualPadButtonEventHandler healthButtonEventHandler;
	VirtualPadButtonEventHandler manaButtonEventHandler;
};

void HandleTouchEvent(const SDL_Event &event);

} // namespace devilution

#endif
