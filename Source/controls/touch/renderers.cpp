#include "control.h"
#include "controls/touch/renderers.h"
#include "cursor.h"
#include "doom.h"
#include "engine.h"
#include "engine/render/cel_render.hpp"
#include "gendung.h"
#include "init.h"
#include "inv.h"
#include "minitext.h"
#include "stores.h"
#include "towners.h"
#include "utils/sdl_compat.h"
#include "utils/sdl_wrap.h"

namespace devilution {

namespace {

VirtualGamepadRenderer Renderer(&VirtualGamepadState);

VirtualGamepadButtonType GetAttackButtonType(bool isPressed)
{
	return isPressed ? GAMEPAD_ATTACKDOWN : GAMEPAD_ATTACK;
}

VirtualGamepadButtonType GetTalkButtonType(bool isPressed)
{
	return isPressed ? GAMEPAD_TALKDOWN : GAMEPAD_TALK;
}

VirtualGamepadButtonType GetItemButtonType(bool isPressed)
{
	return isPressed ? GAMEPAD_ITEMDOWN : GAMEPAD_ITEM;
}

VirtualGamepadButtonType GetObjectButtonType(bool isPressed)
{
	return isPressed ? GAMEPAD_OBJECTDOWN : GAMEPAD_OBJECT;
}

VirtualGamepadButtonType GetCastButtonType(bool isPressed)
{
	return isPressed ? GAMEPAD_CASTSPELLDOWN : GAMEPAD_CASTSPELL;
}

VirtualGamepadButtonType GetCancelButtonType(bool isPressed)
{
	return isPressed ? GAMEPAD_BACKDOWN : GAMEPAD_BACK;
}

VirtualGamepadButtonType GetBlankButtonType(bool isPressed)
{
	return isPressed ? GAMEPAD_BLANKDOWN : GAMEPAD_BLANK;
}

void LoadButtonArt(Art *buttonArt, SDL_Renderer *renderer)
{
	const int Frames = 14;
	buttonArt->surface.reset(LoadPNG("ui_art\\button.png"));
	if (buttonArt->surface == nullptr)
		return;

	buttonArt->logical_width = buttonArt->surface->w;
	buttonArt->frame_height = buttonArt->surface->h / Frames;
	buttonArt->frames = Frames;

	if (renderer != nullptr) {
		buttonArt->texture.reset(SDL_CreateTextureFromSurface(renderer, buttonArt->surface.get()));
		buttonArt->surface = nullptr;
	}
}

void LoadPotionArt(Art *potionArt, SDL_Renderer *renderer)
{
	item_cursor_graphic potionGraphics[] {
		ICURS_POTION_OF_HEALING,
		ICURS_POTION_OF_MANA,
		ICURS_POTION_OF_REJUVENATION,
		ICURS_POTION_OF_FULL_HEALING,
		ICURS_POTION_OF_FULL_MANA,
		ICURS_POTION_OF_FULL_REJUVENATION,
		ICURS_SCROLL_OF
	};

	int potionFrame = CURSOR_FIRSTITEM + ICURS_POTION_OF_HEALING;
	Size potionSize = GetInvItemSize(potionFrame);

	auto surface = SDLWrap::CreateRGBSurfaceWithFormat(
	    /*flags=*/0,
	    /*width=*/potionSize.width,
	    /*height=*/potionSize.height * sizeof(potionGraphics),
	    /*depth=*/8,
	    SDL_PIXELFORMAT_INDEX8);

	auto palette = SDLWrap::AllocPalette();
	if (SDLC_SetSurfaceAndPaletteColors(surface.get(), palette.get(), orig_palette, 0, 256) < 0)
		ErrSdl();

	Uint32 bgColor = SDL_MapRGB(surface->format, orig_palette[1].r, orig_palette[1].g, orig_palette[1].b);
	if (SDL_FillRect(surface.get(), nullptr, bgColor) < 0)
		ErrSdl();
	if (SDL_SetColorKey(surface.get(), SDL_TRUE, bgColor) < 0)
		ErrSdl();

	Point position { 0, 0 };
	for (item_cursor_graphic graphic : potionGraphics) {
		const int frame = CURSOR_FIRSTITEM + graphic;
		const CelSprite &potionSprite = GetInvItemSprite(frame);
		position.y += potionSize.height;
		CelClippedDrawTo(Surface(surface.get()), position, potionSprite, frame);
	}

	potionArt->logical_width = potionSize.width;
	potionArt->frame_height = potionSize.height;
	potionArt->frames = sizeof(potionGraphics);

	if (renderer == nullptr) {
		potionArt->surface.reset(SDL_ConvertSurfaceFormat(surface.get(), SDL_PIXELFORMAT_ARGB8888, 0));
	} else {
		potionArt->texture.reset(SDL_CreateTextureFromSurface(renderer, surface.get()));
		potionArt->surface = nullptr;
	}
}

} // namespace

void RenderVirtualGamepad(SDL_Renderer *renderer)
{
	RenderFunction renderFunction = [&](Art &art, SDL_Rect *src, SDL_Rect *dst) {
		if (art.texture == nullptr)
			return;

		if (SDL_RenderCopy(renderer, art.texture.get(), src, dst) <= -1)
			ErrSdl();
	};

	Renderer.Render(renderFunction);
}

void RenderVirtualGamepad(SDL_Surface *surface)
{
	RenderFunction renderFunction = [&](Art &art, SDL_Rect *src, SDL_Rect *dst) {
		if (art.surface == nullptr)
			return;

		if (SDL_BlitScaled(art.surface.get(), src, surface, dst) <= -1)
			ErrSdl();
	};

	Renderer.Render(renderFunction);
}

void VirtualGamepadRenderer::LoadArt(SDL_Renderer *renderer)
{
	directionPadRenderer.LoadArt(renderer);
	LoadButtonArt(&buttonArt, renderer);
	LoadPotionArt(&potionArt, renderer);
}

void VirtualDirectionPadRenderer::LoadArt(SDL_Renderer *renderer)
{
	padArt.surface.reset(LoadPNG("ui_art\\directions.png"));
	knobArt.surface.reset(LoadPNG("ui_art\\directions2.png"));

	if (renderer != nullptr) {
		padArt.texture.reset(SDL_CreateTextureFromSurface(renderer, padArt.surface.get()));
		padArt.surface = nullptr;

		knobArt.texture.reset(SDL_CreateTextureFromSurface(renderer, knobArt.surface.get()));
		knobArt.surface = nullptr;
	}
}

void VirtualGamepadRenderer::Render(RenderFunction renderFunction)
{
	if (CurrentProc == DisableInputWndProc)
		return;

	directionPadRenderer.Render(renderFunction);

	primaryActionButtonRenderer.Render(renderFunction, buttonArt);
	secondaryActionButtonRenderer.Render(renderFunction, buttonArt);
	spellActionButtonRenderer.Render(renderFunction, buttonArt);
	cancelButtonRenderer.Render(renderFunction, buttonArt);
	healthButtonRenderer.Render(renderFunction, buttonArt);
	manaButtonRenderer.Render(renderFunction, buttonArt);

	healthButtonRenderer.RenderPotion(renderFunction, potionArt);
	manaButtonRenderer.RenderPotion(renderFunction, potionArt);
}

void VirtualDirectionPadRenderer::Render(RenderFunction renderFunction)
{
	RenderPad(renderFunction);
	RenderKnob(renderFunction);
}

void VirtualDirectionPadRenderer::RenderPad(RenderFunction renderFunction)
{
	auto center = virtualDirectionPad->area.position;
	auto radius = virtualDirectionPad->area.radius;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;
	SDL_Rect rect { x, y, width, height };
	renderFunction(padArt, nullptr, &rect);
}

void VirtualDirectionPadRenderer::RenderKnob(RenderFunction renderFunction)
{
	auto center = virtualDirectionPad->position;
	auto radius = virtualDirectionPad->area.radius / 3;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;
	SDL_Rect rect { x, y, width, height };
	renderFunction(knobArt, nullptr, &rect);
}

void VirtualPadButtonRenderer::Render(RenderFunction renderFunction, Art &buttonArt)
{
	VirtualGamepadButtonType buttonType = GetButtonType();
	int frame = buttonType;
	int offset = buttonArt.h() * frame;

	auto center = virtualPadButton->area.position;
	auto radius = virtualPadButton->area.radius;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;

	SDL_Rect src { 0, offset, buttonArt.w(), buttonArt.h() };
	SDL_Rect dst { x, y, width, height };
	renderFunction(buttonArt, &src, &dst);
}

void PotionButtonRenderer::RenderPotion(RenderFunction renderFunction, Art &potionArt)
{
	VirtualGamepadPotionType potionType = GetPotionType();
	int frame = potionType;
	int offset = potionArt.h() * frame;

	auto center = virtualPadButton->area.position;
	auto radius = virtualPadButton->area.radius * 8 / 10;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;

	SDL_Rect src { 0, offset, potionArt.w(), potionArt.h() };
	SDL_Rect dst { x, y, width, height };
	renderFunction(potionArt, &src, &dst);
}

VirtualGamepadPotionType PotionButtonRenderer::GetPotionType()
{
	for (int i = 0; i < MAXBELTITEMS; i++) {
		auto &myPlayer = Players[MyPlayerId];
		const int id = AllItemsList[myPlayer.SpdList[i].IDidx].iMiscId;
		const int spellId = AllItemsList[myPlayer.SpdList[i].IDidx].iSpell;

		if (myPlayer.SpdList[i].isEmpty())
			continue;

		if (potionType == BLT_HEALING) {
			if (id == IMISC_HEAL)
				return GAMEPAD_HEALING;
			if (id == IMISC_FULLHEAL)
				return GAMEPAD_FULL_HEALING;
			if (id == IMISC_SCROLL && spellId == SPL_HEAL)
				return GAMEPAD_SCROLL_OF_HEALING;
		}

		if (potionType == BLT_MANA) {
			if (id == IMISC_MANA)
				return GAMEPAD_MANA;
			if (id == IMISC_FULLMANA)
				return GAMEPAD_FULL_MANA;
		}

		if (id == IMISC_REJUV)
			return GAMEPAD_REJUVENATION;
		if (id == IMISC_FULLREJUV)
			return GAMEPAD_FULL_REJUVENATION;
	}
}

VirtualGamepadButtonType PrimaryActionButtonRenderer::GetButtonType()
{
	// NEED: Confirm surface
	if (qtextflag)
		return GetTalkButtonType(virtualPadButton->isHeld);
	if (invflag)
		return GetInventoryButtonType();
	if (leveltype == DTYPE_TOWN)
		return GetTownButtonType();
	return GetDungeonButtonType();
}

VirtualGamepadButtonType PrimaryActionButtonRenderer::GetTownButtonType()
{
	if (stextflag != STORE_NONE || pcursmonst != -1)
		return GetTalkButtonType(virtualPadButton->isHeld);
	return GetBlankButtonType(virtualPadButton->isHeld);
}

VirtualGamepadButtonType PrimaryActionButtonRenderer::GetDungeonButtonType()
{
	if (pcursmonst != -1) {
		const auto &monster = Monsters[pcursmonst];
		if (M_Talker(monster) || monster.mtalkmsg != TEXT_NONE)
			return GetTalkButtonType(virtualPadButton->isHeld);
	}
	return GetAttackButtonType(virtualPadButton->isHeld);
}

VirtualGamepadButtonType PrimaryActionButtonRenderer::GetInventoryButtonType()
{
	if (pcursinvitem != -1 || pcurs > CURSOR_HAND)
		return GetItemButtonType(virtualPadButton->isHeld);
	return GetBlankButtonType(virtualPadButton->isHeld);
}

VirtualGamepadButtonType SecondaryActionButtonRenderer::GetButtonType()
{
	// NEED: Stairs surface
	if (InGameMenu() || QuestLogIsOpen || sbookflag)
		return GetBlankButtonType(virtualPadButton->isHeld);
	if (pcursobj != -1)
		return GetObjectButtonType(virtualPadButton->isHeld);
	if (pcursitem != -1)
		return GetItemButtonType(virtualPadButton->isHeld);
	return GetBlankButtonType(virtualPadButton->isHeld);
}

VirtualGamepadButtonType SpellActionButtonRenderer::GetButtonType()
{
	if (!InGameMenu() && !QuestLogIsOpen && !sbookflag)
		return GetCastButtonType(virtualPadButton->isHeld);
	return GetBlankButtonType(virtualPadButton->isHeld);
}

VirtualGamepadButtonType CancelButtonRenderer::GetButtonType()
{
	if (InGameMenu())
		return GetCancelButtonType(virtualPadButton->isHeld);
	if (DoomFlag || invflag || sbookflag || QuestLogIsOpen || chrflag)
		return GetCancelButtonType(virtualPadButton->isHeld);
	return GetBlankButtonType(virtualPadButton->isHeld);
}

VirtualGamepadButtonType PotionButtonRenderer::GetButtonType()
{
	return GetBlankButtonType(virtualPadButton->isHeld);
}

void VirtualGamepadRenderer::UnloadArt()
{
	directionPadRenderer.UnloadArt();
	buttonArt.Unload();
	potionArt.Unload();
}

void VirtualDirectionPadRenderer::UnloadArt()
{
	padArt.Unload();
	knobArt.Unload();
}

void InitVirtualGamepadGFX(SDL_Renderer *renderer)
{
	Renderer.LoadArt(renderer);
}

void FreeVirtualGamepadGFX()
{
	Renderer.UnloadArt();
}

} // namespace devilution
