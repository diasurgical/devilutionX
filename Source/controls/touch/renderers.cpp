#include "control.h"
#include "controls/plrctrls.h"
#include "controls/touch/renderers.h"
#include "cursor.h"
#include "doom.h"
#include "engine.h"
#include "gendung.h"
#include "init.h"
#include "inv.h"
#include "minitext.h"
#include "stores.h"
#include "towners.h"

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

	const int Frames = 14;
	buttonArt.surface.reset(LoadPNG("ui_art\\button.png"));
	if (buttonArt.surface == nullptr)
		return;

	buttonArt.logical_width = buttonArt.surface->w;
	buttonArt.frame_height = buttonArt.surface->h / Frames;
	buttonArt.frames = Frames;

	if (renderer != nullptr) {
		buttonArt.texture.reset(SDL_CreateTextureFromSurface(renderer, buttonArt.surface.get()));
		buttonArt.surface = nullptr;
	}
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

void VirtualGamepadRenderer::UnloadArt()
{
	directionPadRenderer.UnloadArt();
	buttonArt.Unload();
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
