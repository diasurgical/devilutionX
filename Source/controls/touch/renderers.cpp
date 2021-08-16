#include "control.h"
#include "controls/plrctrls.h"
#include "controls/touch/renderers.h"
#include "cursor.h"
#include "doom.h"
#include "engine.h"
#include "gendung.h"
#include "inv.h"
#include "minitext.h"
#include "stores.h"
#include "towners.h"

namespace devilution {

namespace {

VirtualGamepadRenderer Renderer(&VirtualGamepadState);

} // namespace

void DrawVirtualGamepad(const Surface &out)
{
	Renderer.Render(out);
}

void VirtualGamepadRenderer::Render(const Surface &out)
{
	directionPadRenderer.Render(out);
	primaryActionButtonRenderer.Render(out);
	secondaryActionButtonRenderer.Render(out);
	spellActionButtonRenderer.Render(out);
	cancelButtonRenderer.Render(out);
}

void VirtualDirectionPadRenderer::Render(const Surface &out)
{
	RenderPad(out);
	RenderKnob(out);
}

void VirtualDirectionPadRenderer::RenderPad(const Surface &out)
{
	if (padSurface == nullptr)
		padSurface.reset(LoadPNG("ui_art\\directions.png"));

	auto center = virtualDirectionPad->area.position;
	auto radius = virtualDirectionPad->area.radius;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;
	SDL_Rect rect { x, y, width, height };
	SDL_BlitScaled(padSurface.get(), nullptr, out.surface, &rect);
}

void VirtualDirectionPadRenderer::RenderKnob(const Surface &out)
{
	if (knobSurface == nullptr)
		knobSurface.reset(LoadPNG("ui_art\\directions2.png"));

	auto center = virtualDirectionPad->position;
	auto radius = virtualDirectionPad->area.radius / 3;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;
	SDL_Rect rect { x, y, width, height };
	SDL_BlitScaled(knobSurface.get(), nullptr, out.surface, &rect);
}

void VirtualPadButtonRenderer::Render(const Surface &out)
{
	auto center = virtualPadButton->area.position;
	auto radius = virtualPadButton->area.radius;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;

	SDL_Surface *surface = GetButtonSurface();
	SDL_Rect rect { x, y, width, height };
	SDL_BlitScaled(surface, nullptr, out.surface, &rect);
}

SDL_Surface *VirtualPadButtonRenderer::GetAttackSurface()
{
	if (attackSurface == nullptr)
		attackSurface.reset(LoadPNG("ui_art\\attack.png"));
	if (pressedAttackSurface == nullptr)
		pressedAttackSurface.reset(LoadPNG("ui_art\\attackp.png"));
	return virtualPadButton->isHeld ? pressedAttackSurface.get() : attackSurface.get();
}

SDL_Surface *VirtualPadButtonRenderer::GetTalkSurface()
{
	if (talkSurface == nullptr)
		talkSurface.reset(LoadPNG("ui_art\\talk.png"));
	if (pressedTalkSurface == nullptr)
		pressedTalkSurface.reset(LoadPNG("ui_art\\talkp.png"));
	return virtualPadButton->isHeld ? pressedTalkSurface.get() : talkSurface.get();
}

SDL_Surface *VirtualPadButtonRenderer::GetItemSurface()
{
	if (itemSurface == nullptr)
		itemSurface.reset(LoadPNG("ui_art\\pickitem.png"));
	if (pressedItemSurface == nullptr)
		pressedItemSurface.reset(LoadPNG("ui_art\\pickitemp.png"));
	return virtualPadButton->isHeld ? pressedItemSurface.get() : itemSurface.get();
}

SDL_Surface *VirtualPadButtonRenderer::GetObjectSurface()
{
	if (objectSurface == nullptr)
		objectSurface.reset(LoadPNG("ui_art\\object.png"));
	if (pressedObjectSurface == nullptr)
		pressedObjectSurface.reset(LoadPNG("ui_art\\objectp.png"));
	return virtualPadButton->isHeld ? pressedObjectSurface.get() : objectSurface.get();
}

SDL_Surface *VirtualPadButtonRenderer::GetCastSurface()
{
	if (castSurface == nullptr)
		castSurface.reset(LoadPNG("ui_art\\castspell.png"));
	if (pressedCastSurface == nullptr)
		pressedCastSurface.reset(LoadPNG("ui_art\\castspellp.png"));
	return virtualPadButton->isHeld ? pressedCastSurface.get() : castSurface.get();
}

SDL_Surface *VirtualPadButtonRenderer::GetCancelSurface()
{
	if (cancelSurface == nullptr)
		cancelSurface.reset(LoadPNG("ui_art\\back.png"));
	if (pressedCancelSurface == nullptr)
		pressedCancelSurface.reset(LoadPNG("ui_art\\backp.png"));
	return virtualPadButton->isHeld ? pressedCancelSurface.get() : cancelSurface.get();
}

SDL_Surface *VirtualPadButtonRenderer::GetBlankSurface()
{
	if (blankSurface == nullptr)
		blankSurface.reset(LoadPNG("ui_art\\noaction.png"));
	if (pressedBlankSurface == nullptr)
		pressedBlankSurface.reset(LoadPNG("ui_art\\noactionp.png"));
	return virtualPadButton->isHeld ? pressedBlankSurface.get() : blankSurface.get();
}

SDL_Surface *PrimaryActionButtonRenderer::GetButtonSurface()
{
	// NEED: Confirm surface
	if (qtextflag)
		return GetTalkSurface();
	if (invflag)
		return GetInventoryButtonSurface();
	if (leveltype == DTYPE_TOWN)
		return GetTownButtonSurface();
	return GetDungeonButtonSurface();
}

SDL_Surface *PrimaryActionButtonRenderer::GetTownButtonSurface()
{
	if (stextflag != STORE_NONE || pcursmonst != -1)
		return GetTalkSurface();
	return GetBlankSurface();
}

SDL_Surface *PrimaryActionButtonRenderer::GetDungeonButtonSurface()
{
	if (pcursmonst != -1) {
		const auto &monster = Monsters[pcursmonst];
		if (M_Talker(monster) || monster.mtalkmsg != TEXT_NONE)
			return GetTalkSurface();
	}
	return GetAttackSurface();
}

SDL_Surface *PrimaryActionButtonRenderer::GetInventoryButtonSurface()
{
	if (pcursinvitem != -1 || pcurs > CURSOR_HAND)
		return GetItemSurface();
	return GetBlankSurface();
}

SDL_Surface *SecondaryActionButtonRenderer::GetButtonSurface()
{
	// NEED: Stairs surface
	if (InGameMenu() || QuestLogIsOpen || sbookflag)
		return GetBlankSurface();
	if (pcursobj != -1)
		return GetObjectSurface();
	if (pcursitem != -1)
		return GetItemSurface();
	return GetBlankSurface();
}

SDL_Surface *SpellActionButtonRenderer::GetButtonSurface()
{
	if (!InGameMenu() && !QuestLogIsOpen && !sbookflag)
		return GetCastSurface();
	return GetBlankSurface();
}

SDL_Surface *CancelButtonRenderer::GetButtonSurface()
{
	if (InGameMenu())
		return GetCancelSurface();
	if (DoomFlag || invflag || sbookflag || QuestLogIsOpen || chrflag)
		return GetCancelSurface();
	return GetBlankSurface();
}

} // namespace devilution
