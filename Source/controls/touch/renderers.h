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

typedef std::function<void(Art &art, SDL_Rect *src, SDL_Rect *dst)> RenderFunction;

class VirtualDirectionPadRenderer {
public:
	VirtualDirectionPadRenderer(VirtualDirectionPad *virtualDirectionPad)
	    : virtualDirectionPad(virtualDirectionPad)
	{
	}

	void LoadArt(SDL_Renderer *renderer);
	void Render(RenderFunction renderFunction);
	void UnloadArt();

private:
	VirtualDirectionPad *virtualDirectionPad;
	Art padArt;
	Art knobArt;

	void RenderPad(RenderFunction renderFunction);
	void RenderKnob(RenderFunction renderFunction);
};

class VirtualPadButtonRenderer {
public:
	VirtualPadButtonRenderer(VirtualPadButton *virtualPadButton)
	    : virtualPadButton(virtualPadButton)
	{
	}

	void Render(RenderFunction renderFunction, Art &buttonArt);

protected:
	VirtualPadButton *virtualPadButton;

	virtual VirtualGamepadButtonType GetButtonType() = 0;
};

class PrimaryActionButtonRenderer : public VirtualPadButtonRenderer {
public:
	PrimaryActionButtonRenderer(VirtualPadButton *primaryActionButton)
	    : VirtualPadButtonRenderer(primaryActionButton)
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
	SecondaryActionButtonRenderer(VirtualPadButton *secondaryActionButton)
	    : VirtualPadButtonRenderer(secondaryActionButton)
	{
	}

private:
	VirtualGamepadButtonType GetButtonType();
};

class SpellActionButtonRenderer : public VirtualPadButtonRenderer {
public:
	SpellActionButtonRenderer(VirtualPadButton *spellActionButton)
	    : VirtualPadButtonRenderer(spellActionButton)
	{
	}

private:
	VirtualGamepadButtonType GetButtonType();
};

class CancelButtonRenderer : public VirtualPadButtonRenderer {
public:
	CancelButtonRenderer(VirtualPadButton *cancelButton)
	    : VirtualPadButtonRenderer(cancelButton)
	{
	}

private:
	VirtualGamepadButtonType GetButtonType();
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

	void LoadArt(SDL_Renderer *renderer);
	void Render(RenderFunction renderFunction);
	void UnloadArt();

private:
	VirtualDirectionPadRenderer directionPadRenderer;
	PrimaryActionButtonRenderer primaryActionButtonRenderer;
	SecondaryActionButtonRenderer secondaryActionButtonRenderer;
	SpellActionButtonRenderer spellActionButtonRenderer;
	CancelButtonRenderer cancelButtonRenderer;
	Art buttonArt;
};

void InitVirtualGamepadGFX(SDL_Renderer *renderer);
void RenderVirtualGamepad(SDL_Renderer *renderer);
void RenderVirtualGamepad(SDL_Surface *surface);
void FreeVirtualGamepadGFX();

} // namespace devilution

#endif
