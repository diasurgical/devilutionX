#pragma once

#if defined(VIRTUAL_GAMEPAD) && !defined(USE_SDL1)

#include "controls/touch/gamepad.h"
#include "engine/surface.hpp"

namespace devilution {

class VirtualDirectionPadRenderer {
public:
	VirtualDirectionPadRenderer(VirtualDirectionPad *virtualDirectionPad)
	    : virtualDirectionPad(virtualDirectionPad)
	{
	}

	void Render(const Surface &out);

private:
	VirtualDirectionPad *virtualDirectionPad;
};

class VirtualPadButtonRenderer {
public:
	VirtualPadButtonRenderer(VirtualPadButton *virtualPadButton)
	    : virtualPadButton(virtualPadButton)
	{
	}

	void Render(const Surface &out);

private:
	VirtualPadButton *virtualPadButton;
};

class VirtualGamepadRenderer {
public:
	VirtualGamepadRenderer(VirtualGamepad *virtualGamepad)
	    : directionPadRenderer(&virtualGamepad->directionPad)
	    , primaryActionButtonRenderer(&virtualGamepad->primaryActionButton)
	    , secondaryActionButtonRenderer(&virtualGamepad->secondaryActionButton)
	    , spellActionButtonRenderer(&virtualGamepad->spellActionButton)
	    , cancelButtonRenderer(&virtualGamepad->cancelButton)
	{
	}

	void Render(const Surface &out);

private:
	VirtualDirectionPadRenderer directionPadRenderer;
	VirtualPadButtonRenderer primaryActionButtonRenderer;
	VirtualPadButtonRenderer secondaryActionButtonRenderer;
	VirtualPadButtonRenderer spellActionButtonRenderer;
	VirtualPadButtonRenderer cancelButtonRenderer;
};

void DrawVirtualGamepad(const Surface &out);

} // namespace devilution

#endif
