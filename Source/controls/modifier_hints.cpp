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

int SpaceWidth()
{
	static const int Result = GetLineWidth(" ");
	return Result;
}

/** The number of spaces between left and right hints. */
constexpr int MidSpaces = 5;

/** Vertical distance between text lines. */
constexpr int LineHeight = 25;

/** Horizontal margin of the hints circle from panel edge. */
constexpr int CircleMarginX = 16;

/** Distance between the panel top and the circle top. */
constexpr int CircleTop = 76;

struct CircleMenuHint {
	CircleMenuHint(bool isDpad, const char *top, const char *right, const char *bottom, const char *left)
	    : isDpad(isDpad)
	    , top(top)
	    , topW(GetLineWidth(top))
	    , right(right)
	    , rightW(GetLineWidth(right))
	    , bottom(bottom)
	    , bottomW(GetLineWidth(bottom))
	    , left(left)
	    , leftW(GetLineWidth(left))
	    , xMid(leftW + SpaceWidth() * MidSpaces / 2)
	{
	}

	[[nodiscard]] int Width() const
	{
		return 2 * xMid;
	}

	bool isDpad;

	const char *top;
	int topW;
	const char *right;
	int rightW;
	const char *bottom;
	int bottomW;
	const char *left;
	int leftW;

	int xMid;
};

bool IsTopActive(const CircleMenuHint &hint)
{
	if (hint.isDpad)
		return IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_UP);
	return IsControllerButtonPressed(ControllerButton_BUTTON_Y);
}

bool IsRightActive(const CircleMenuHint &hint)
{
	if (hint.isDpad)
		return IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_RIGHT);
	return IsControllerButtonPressed(ControllerButton_BUTTON_B);
}

bool IsBottomActive(const CircleMenuHint &hint)
{
	if (hint.isDpad)
		return IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_DOWN);
	return IsControllerButtonPressed(ControllerButton_BUTTON_A);
}

bool IsLeftActive(const CircleMenuHint &hint)
{
	if (hint.isDpad)
		return IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_LEFT);
	return IsControllerButtonPressed(ControllerButton_BUTTON_X);
}

uint16_t CircleMenuHintTextColor(bool active)
{
	return active ? UIS_BLUE : UIS_GOLD;
}

void DrawCircleMenuHint(const CelOutputBuffer &out, const CircleMenuHint &hint, int x, int y)
{
	DrawString(out, hint.top, { x + hint.xMid - hint.topW / 2, y, 0, 0 }, CircleMenuHintTextColor(IsTopActive(hint)));
	y += LineHeight;

	DrawString(out, hint.left, { x, y, 0, 0 }, CircleMenuHintTextColor(IsLeftActive(hint)));
	DrawString(out, hint.right, { x + hint.leftW + MidSpaces * SpaceWidth(), y, 0, 0 }, CircleMenuHintTextColor(IsRightActive(hint)));
	y += LineHeight;

	DrawString(out, hint.bottom, { x + hint.xMid - hint.bottomW / 2, y, 0, 0 }, CircleMenuHintTextColor(IsBottomActive(hint)));
}

void DrawStartModifierMenu(const CelOutputBuffer &out)
{
	if (!start_modifier_active)
		return;
	static const CircleMenuHint DPad(/*isDpad=*/true, /*top=*/_("Menu"), /*right=*/_("Inv"), /*bottom=*/_("Map"), /*left=*/_("Char"));
	static const CircleMenuHint Buttons(/*isDpad=*/false, /*top=*/"", /*right=*/"", /*bottom=*/_("Spells"), /*left=*/_("Quests"));
	DrawCircleMenuHint(out, DPad, PANEL_LEFT + CircleMarginX, PANEL_TOP - CircleTop);
	DrawCircleMenuHint(out, Buttons, PANEL_LEFT + PANEL_WIDTH - Buttons.Width() - CircleMarginX, PANEL_TOP - CircleTop);
}

void DrawSelectModifierMenu(const CelOutputBuffer &out)
{
	if (!select_modifier_active)
		return;
	if (sgOptions.Controller.bDpadHotkeys) {
		static const CircleMenuHint DPad(/*isDpad=*/true, /*top=*/"F6", /*right=*/"F8", /*bottom=*/"F7", /*left=*/"F5");
		DrawCircleMenuHint(out, DPad, PANEL_LEFT + CircleMarginX, PANEL_TOP - CircleTop);
	}
	static const CircleMenuHint Spells(/*isDpad=*/false, "F6", "F8", "F7", "F5");
	DrawCircleMenuHint(out, Spells, PANEL_LEFT + PANEL_WIDTH - Spells.Width() - CircleMarginX, PANEL_TOP - CircleTop);
}

} // namespace

void DrawControllerModifierHints(const CelOutputBuffer &out)
{
	DrawStartModifierMenu(out);
	DrawSelectModifierMenu(out);
}

} // namespace devilution
