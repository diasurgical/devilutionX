#pragma once

#if defined(VIRTUAL_GAMEPAD) && !defined(USE_SDL1)

#include "controls/touch/gamepad.h"
#include "engine/surface.hpp"
#include "utils/png.h"
#include "utils/sdl_ptrs.h"

namespace devilution {

enum VirtualGamepadButtonType {
	GAMEPAD_ATTACK,
	GAMEPAD_ATTACKDOWN,
	GAMEPAD_TALK,
	GAMEPAD_TALKDOWN,
	GAMEPAD_ITEM,
	GAMEPAD_ITEMDOWN,
	GAMEPAD_OBJECT,
	GAMEPAD_OBJECTDOWN,
	GAMEPAD_CASTSPELL,
	GAMEPAD_CASTSPELLDOWN,
	GAMEPAD_BACK,
	GAMEPAD_BACKDOWN,
	GAMEPAD_BLANK,
	GAMEPAD_BLANKDOWN,
};

class VirtualDirectionPadRenderer {
public:
	VirtualDirectionPadRenderer(VirtualDirectionPad *virtualDirectionPad)
	    : virtualDirectionPad(virtualDirectionPad)
	{
	}

	void LoadArt();
	void Render(const Surface &out);
	void UnloadArt();

private:
	VirtualDirectionPad *virtualDirectionPad;
	SDLSurfaceUniquePtr padSurface;
	SDLSurfaceUniquePtr knobSurface;

	void RenderPad(const Surface &out);
	void RenderKnob(const Surface &out);
};

class VirtualPadButtonRenderer {
public:
	VirtualPadButtonRenderer(VirtualPadButton *virtualPadButton, Art *buttonArt)
	    : virtualPadButton(virtualPadButton)
	    , buttonArt(buttonArt)
	{
	}

	void Render(const Surface &out);

protected:
	VirtualPadButton *virtualPadButton;

	virtual VirtualGamepadButtonType GetButtonType() = 0;

private:
	Art *buttonArt;
};

class PrimaryActionButtonRenderer : public VirtualPadButtonRenderer {
public:
	PrimaryActionButtonRenderer(VirtualPadButton *primaryActionButton, Art *buttonArt)
	    : VirtualPadButtonRenderer(primaryActionButton, buttonArt)
	{
	}

private:
	VirtualGamepadButtonType GetButtonType();
	VirtualGamepadButtonType GetTownButtonType();
	VirtualGamepadButtonType GetDungeonButtonType();
	VirtualGamepadButtonType GetInventoryButtonType();
};

class SecondaryActionButtonRenderer : public VirtualPadButtonRenderer {
public:
	SecondaryActionButtonRenderer(VirtualPadButton *secondaryActionButton, Art *buttonArt)
	    : VirtualPadButtonRenderer(secondaryActionButton, buttonArt)
	{
	}

private:
	VirtualGamepadButtonType GetButtonType();
};

class SpellActionButtonRenderer : public VirtualPadButtonRenderer {
public:
	SpellActionButtonRenderer(VirtualPadButton *spellActionButton, Art *buttonArt)
	    : VirtualPadButtonRenderer(spellActionButton, buttonArt)
	{
	}

private:
	VirtualGamepadButtonType GetButtonType();
};

class CancelButtonRenderer : public VirtualPadButtonRenderer {
public:
	CancelButtonRenderer(VirtualPadButton *cancelButton, Art *buttonArt)
	    : VirtualPadButtonRenderer(cancelButton, buttonArt)
	{
	}

private:
	VirtualGamepadButtonType GetButtonType();
};

class VirtualGamepadRenderer {
public:
	VirtualGamepadRenderer(VirtualGamepad *virtualGamepad)
	    : directionPadRenderer(&virtualGamepad->directionPad)
	    , primaryActionButtonRenderer(&virtualGamepad->primaryActionButton, &buttonArt)
	    , secondaryActionButtonRenderer(&virtualGamepad->secondaryActionButton, &buttonArt)
	    , spellActionButtonRenderer(&virtualGamepad->spellActionButton, &buttonArt)
	    , cancelButtonRenderer(&virtualGamepad->cancelButton, &buttonArt)
	{
	}

	void LoadArt();
	void Render(const Surface &out);
	void UnloadArt();

private:
	VirtualDirectionPadRenderer directionPadRenderer;
	PrimaryActionButtonRenderer primaryActionButtonRenderer;
	SecondaryActionButtonRenderer secondaryActionButtonRenderer;
	SpellActionButtonRenderer spellActionButtonRenderer;
	CancelButtonRenderer cancelButtonRenderer;
	Art buttonArt;
};

void DrawVirtualGamepad(const Surface &out);

void InitVirtualGamepadGFX();
void FreeVirtualGamepadGFX();

} // namespace devilution

#endif
