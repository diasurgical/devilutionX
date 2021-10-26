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

class VirtualButtonEventHandler {
public:
	VirtualButtonEventHandler(VirtualButton *virtualButton)
	    : virtualButton(virtualButton)
	    , activeFinger(0)
	    , isActive(false)
	{
	}

	bool Handle(const SDL_Event &event);

private:
	VirtualButton *virtualButton;
	SDL_FingerID activeFinger;
	bool isActive;

	bool HandleFingerDown(const SDL_TouchFingerEvent &event);
	bool HandleFingerUp(const SDL_TouchFingerEvent &event);
	bool HandleFingerMotion(const SDL_TouchFingerEvent &event);
};

class VirtualGamepadEventHandler {
public:
	VirtualGamepadEventHandler(VirtualGamepad *virtualGamepad)
	    : charMenuButtonEventHandler(&virtualGamepad->menuPanel.charButton)
	    , questsMenuButtonEventHandler(&virtualGamepad->menuPanel.questsButton)
	    , inventoryMenuButtonEventHandler(&virtualGamepad->menuPanel.inventoryButton)
	    , mapMenuButtonEventHandler(&virtualGamepad->menuPanel.mapButton)
	    , directionPadEventHandler(&virtualGamepad->directionPad)
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
	VirtualButtonEventHandler charMenuButtonEventHandler;
	VirtualButtonEventHandler questsMenuButtonEventHandler;
	VirtualButtonEventHandler inventoryMenuButtonEventHandler;
	VirtualButtonEventHandler mapMenuButtonEventHandler;

	VirtualDirectionPadEventHandler directionPadEventHandler;

	VirtualButtonEventHandler primaryActionButtonEventHandler;
	VirtualButtonEventHandler secondaryActionButtonEventHandler;
	VirtualButtonEventHandler spellActionButtonEventHandler;
	VirtualButtonEventHandler cancelButtonEventHandler;

	VirtualButtonEventHandler healthButtonEventHandler;
	VirtualButtonEventHandler manaButtonEventHandler;
};

void HandleTouchEvent(const SDL_Event &event);

} // namespace devilution

#endif
