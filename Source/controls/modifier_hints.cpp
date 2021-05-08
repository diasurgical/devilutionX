#include "controls/modifier_hints.h"

#include <cstddef>

#include "control.h"
#include "controls/controller.h"
#include "controls/game_controls.h"
#include "engine/render/text_render.hpp"
#include "options.h"
#include "utils/language.h"

namespace devilution {

namespace {

struct CircleMenuHint {
	CircleMenuHint(bool isDpad, const char *top, const char *right, const char *bottom, const char *left)
	    : is_dpad(isDpad)
	    , top(top)
	    , right(right)
	    , bottom(bottom)
	    , left(left)
	{
	}
	bool is_dpad;

	const char *top;
	const char *right;
	const char *bottom;
	const char *left;
};

bool IsTopActive(const CircleMenuHint &hint)
{
	if (hint.is_dpad)
		return IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_UP);
	return IsControllerButtonPressed(ControllerButton_BUTTON_Y);
}

bool IsRightActive(const CircleMenuHint &hint)
{
	if (hint.is_dpad)
		return IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_RIGHT);
	return IsControllerButtonPressed(ControllerButton_BUTTON_B);
}

bool IsBottomActive(const CircleMenuHint &hint)
{
	if (hint.is_dpad)
		return IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_DOWN);
	return IsControllerButtonPressed(ControllerButton_BUTTON_A);
}

bool IsLeftActive(const CircleMenuHint &hint)
{
	if (hint.is_dpad)
		return IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_LEFT);
	return IsControllerButtonPressed(ControllerButton_BUTTON_X);
}

uint16_t CircleMenuHintTextColor(bool active)
{
	return active ? UIS_BLUE : UIS_GOLD;
}

const int CircleSpacing = 40;
const int CircleMarginX = 16 + CircleSpacing * 2;
const int CirclesTop = 16 + CircleSpacing * 2;

void DrawCircleMenuHint(const CelOutputBuffer &out, const CircleMenuHint &hint, int x, int y)
{
	DrawString(out, hint.top, { x, y, CircleSpacing, 0 }, CircleMenuHintTextColor(IsTopActive(hint)) | UIS_CENTER);
	y += CircleSpacing;

	DrawString(out, hint.left, { x + CircleSpacing, y, CircleSpacing, 0 }, CircleMenuHintTextColor(IsLeftActive(hint)) | UIS_CENTER);
	DrawString(out, hint.right, { x - CircleSpacing, y, CircleSpacing, 0 }, CircleMenuHintTextColor(IsRightActive(hint)) | UIS_CENTER);
	y += CircleSpacing;

	DrawString(out, hint.bottom, { x, y, CircleSpacing, 0 }, CircleMenuHintTextColor(IsBottomActive(hint)) | UIS_CENTER);
}

void DrawStartModifierMenu(const CelOutputBuffer &out)
{
	if (!start_modifier_active)
		return;
	static const CircleMenuHint dPad(/*is_dpad=*/true, /*top=*/_("Menu"), /*right=*/_("Inv"), /*bottom=*/_("Map"), /*left=*/_("Char"));
	static const CircleMenuHint buttons(/*is_dpad=*/false, /*top=*/"", /*right=*/"", /*bottom=*/_("Spells"), /*left=*/_("Quests"));
	DrawCircleMenuHint(out, dPad, PANEL_LEFT + CircleMarginX, PANEL_TOP - CirclesTop);
	DrawCircleMenuHint(out, buttons, PANEL_LEFT + PANEL_WIDTH - CircleMarginX, PANEL_TOP - CirclesTop);
}

void DrawSelectModifierMenu(const CelOutputBuffer &out)
{
	if (!select_modifier_active)
		return;
	if (sgOptions.Controller.bDpadHotkeys) {
		static const CircleMenuHint dPad(/*is_dpad=*/true, /*top=*/"F6", /*right=*/"F8", /*bottom=*/"F7", /*left=*/"F5");
		DrawCircleMenuHint(out, dPad, PANEL_LEFT + CircleMarginX, PANEL_TOP - CirclesTop);
	}
	static const CircleMenuHint spells(/*is_dpad=*/false, "F6", "F8", "F7", "F5");
	DrawCircleMenuHint(out, spells, PANEL_LEFT + PANEL_WIDTH - CircleMarginX, PANEL_TOP - CirclesTop);
}

} // namespace

void DrawControllerModifierHints(const CelOutputBuffer &out)
{
	DrawStartModifierMenu(out);
	DrawSelectModifierMenu(out);
}

} // namespace devilution
