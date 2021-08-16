#pragma once

#if defined(VIRTUAL_GAMEPAD) && !defined(USE_SDL1)

#include "controls/touch/gamepad.h"
#include "engine/surface.hpp"
#include "utils/png.h"
#include "utils/sdl_ptrs.h"

namespace devilution {

class VirtualDirectionPadRenderer {
public:
	VirtualDirectionPadRenderer(VirtualDirectionPad *virtualDirectionPad)
	    : virtualDirectionPad(virtualDirectionPad)
	    , padSurface(nullptr)
	    , knobSurface(nullptr)
	{
	}

	void Render(const Surface &out);

private:
	VirtualDirectionPad *virtualDirectionPad;
	SDLSurfaceUniquePtr padSurface;
	SDLSurfaceUniquePtr knobSurface;

	void RenderPad(const Surface &out);
	void RenderKnob(const Surface &out);
};

class VirtualPadButtonRenderer {
public:
	VirtualPadButtonRenderer(VirtualPadButton *virtualPadButton)
	    : virtualPadButton(virtualPadButton)
	    , attackSurface(nullptr)
	    , pressedAttackSurface(nullptr)
	    , talkSurface(nullptr)
	    , pressedTalkSurface(nullptr)
	    , itemSurface(nullptr)
	    , pressedItemSurface(nullptr)
	    , objectSurface(nullptr)
	    , pressedObjectSurface(nullptr)
	    , castSurface(nullptr)
	    , pressedCastSurface(nullptr)
	    , cancelSurface(nullptr)
	    , pressedCancelSurface(nullptr)
	    , blankSurface(nullptr)
	    , pressedBlankSurface(nullptr)
	{
	}

	void Render(const Surface &out);

protected:
	VirtualPadButton *virtualPadButton;

	virtual SDL_Surface *GetButtonSurface() = 0;
	SDL_Surface *GetAttackSurface();
	SDL_Surface *GetTalkSurface();
	SDL_Surface *GetItemSurface();
	SDL_Surface *GetObjectSurface();
	SDL_Surface *GetCastSurface();
	SDL_Surface *GetCancelSurface();
	SDL_Surface *GetBlankSurface();

private:
	SDLSurfaceUniquePtr attackSurface;
	SDLSurfaceUniquePtr pressedAttackSurface;
	SDLSurfaceUniquePtr talkSurface;
	SDLSurfaceUniquePtr pressedTalkSurface;
	SDLSurfaceUniquePtr itemSurface;
	SDLSurfaceUniquePtr pressedItemSurface;
	SDLSurfaceUniquePtr objectSurface;
	SDLSurfaceUniquePtr pressedObjectSurface;
	SDLSurfaceUniquePtr castSurface;
	SDLSurfaceUniquePtr pressedCastSurface;
	SDLSurfaceUniquePtr cancelSurface;
	SDLSurfaceUniquePtr pressedCancelSurface;
	SDLSurfaceUniquePtr blankSurface;
	SDLSurfaceUniquePtr pressedBlankSurface;
};

class PrimaryActionButtonRenderer : public VirtualPadButtonRenderer {
public:
	PrimaryActionButtonRenderer(VirtualPadButton *primaryActionButton)
	    : VirtualPadButtonRenderer(primaryActionButton)
	{
	}

private:
	SDL_Surface *GetButtonSurface();
	SDL_Surface *GetTownButtonSurface();
	SDL_Surface *GetDungeonButtonSurface();
	SDL_Surface *GetInventoryButtonSurface();
};

class SecondaryActionButtonRenderer : public VirtualPadButtonRenderer {
public:
	SecondaryActionButtonRenderer(VirtualPadButton *secondaryActionButton)
	    : VirtualPadButtonRenderer(secondaryActionButton)
	{
	}

private:
	SDL_Surface *GetButtonSurface();
};

class SpellActionButtonRenderer : public VirtualPadButtonRenderer {
public:
	SpellActionButtonRenderer(VirtualPadButton *spellActionButton)
	    : VirtualPadButtonRenderer(spellActionButton)
	{
	}

private:
	SDL_Surface *GetButtonSurface();
};

class CancelButtonRenderer : public VirtualPadButtonRenderer {
public:
	CancelButtonRenderer(VirtualPadButton *cancelButton)
	    : VirtualPadButtonRenderer(cancelButton)
	{
	}

private:
	SDL_Surface *GetButtonSurface();
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
	PrimaryActionButtonRenderer primaryActionButtonRenderer;
	SecondaryActionButtonRenderer secondaryActionButtonRenderer;
	SpellActionButtonRenderer spellActionButtonRenderer;
	CancelButtonRenderer cancelButtonRenderer;
};

void DrawVirtualGamepad(const Surface &out);

} // namespace devilution

#endif
