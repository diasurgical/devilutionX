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

void DrawVirtualGamepad(const Surface &out)
{
	Renderer.Render(out);
}

void VirtualGamepadRenderer::LoadArt()
{
	directionPadRenderer.LoadArt();

	const int Frames = 14;
	buttonArt.surface.reset(LoadPNG("ui_art\\button.png"));
	buttonArt.logical_width = buttonArt.surface->w;
	buttonArt.frame_height = buttonArt.surface->h / Frames;
	buttonArt.frames = Frames;
}

void VirtualDirectionPadRenderer::LoadArt()
{
	padSurface.reset(LoadPNG("ui_art\\directions.png"));
	knobSurface.reset(LoadPNG("ui_art\\directions2.png"));
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
		return;

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
		return;

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
	if (buttonArt->surface == nullptr)
		return;

	VirtualGamepadButtonType buttonType = GetButtonType();
	int frame = buttonType;
	int offset = buttonArt->h() * frame;

	auto center = virtualPadButton->area.position;
	auto radius = virtualPadButton->area.radius;
	int diameter = 2 * radius;

	int x = center.x - radius;
	int y = center.y - radius;
	int width = diameter;
	int height = diameter;

	SDL_Rect src { 0, offset, buttonArt->w(), buttonArt->h() };
	SDL_Rect dst { x, y, width, height };
	SDL_BlitScaled(buttonArt->surface.get(), &src, out.surface, &dst);
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
	buttonArt.surface = nullptr;
}

void VirtualDirectionPadRenderer::UnloadArt()
{
	padSurface = nullptr;
	knobSurface = nullptr;
}

void InitVirtualGamepadGFX()
{
	Renderer.LoadArt();
}

void FreeVirtualGamepadGFX()
{
	Renderer.UnloadArt();
}

} // namespace devilution
