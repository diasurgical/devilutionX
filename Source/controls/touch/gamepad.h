#pragma once

#if defined(VIRTUAL_GAMEPAD) && !defined(USE_SDL1)

#include "controls/controller_buttons.h"
#include "engine/circle.hpp"
#include "engine/point.hpp"

namespace devilution {

struct VirtualDirectionPad {
	Circle area;
	Point position;
	bool isUpPressed;
	bool isDownPressed;
	bool isLeftPressed;
	bool isRightPressed;

	VirtualDirectionPad()
	    : area({ { 0, 0 }, 0 })
	    , position({ 0, 0 })
	    , isUpPressed(false)
	    , isDownPressed(false)
	    , isLeftPressed(false)
	    , isRightPressed(false)
	{
	}

	void UpdatePosition(Point touchCoordinates);
};

struct VirtualPadButton {
	Circle area;
	bool isHeld;
	bool didStateChange;

	VirtualPadButton()
	    : area({ { 0, 0 }, 0 })
	    , isHeld(false)
	    , didStateChange(false)
	{
	}
};

struct VirtualGamepad {
	VirtualDirectionPad directionPad;

	VirtualPadButton primaryActionButton;
	VirtualPadButton secondaryActionButton;
	VirtualPadButton spellActionButton;
	VirtualPadButton cancelButton;

	VirtualPadButton healthButton;
	VirtualPadButton manaButton;

	VirtualGamepad()
	{
	}
};

void InitializeVirtualGamepad();

extern VirtualGamepad VirtualGamepadState;

} // namespace devilution

#endif
