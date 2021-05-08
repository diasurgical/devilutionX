#include "controls/modifier_hints.h"

#include <cstddef>

#include "control.h"
#include "controls/controller.h"
#include "controls/game_controls.h"
#include "options.h"
#include "utils/language.h"

namespace devilution {

namespace {

int CalculateTextWidth(const char *s)
{
	int l = 0;
	while (*s != '\0') {
		l += fontkern[GameFontSmall][fontframe[GameFontSmall][gbFontTransTbl[static_cast<unsigned char>(*s++)]]] + 1;
	}
	return l;
}

int SpaceWidth()
{
	static const int spaceWidth = CalculateTextWidth(" ");
	return spaceWidth;
}

struct CircleMenuHint {
	CircleMenuHint(bool isDpad, const char *top, const char *right, const char *bottom, const char *left)
	    : is_dpad(isDpad)
	    , top(top)
	    , top_w(CalculateTextWidth(top))
	    , right(right)
	    , right_w(CalculateTextWidth(right))
	    , bottom(bottom)
	    , bottom_w(CalculateTextWidth(bottom))
	    , left(left)
	    , left_w(CalculateTextWidth(left))
	    , x_mid(left_w + SpaceWidth() * 2.5)
	{
	}

	int Width() const
	{
		return 2 * x_mid;
	}

	bool is_dpad;

	const char *top;
	int top_w;
	const char *right;
	int right_w;
	const char *bottom;
	int bottom_w;
	const char *left;
	int left_w;

	int x_mid;
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

text_color CircleMenuHintTextColor(bool active)
{
	return active ? COL_BLUE : COL_GOLD;
}

void DrawCircleMenuHint(const CelOutputBuffer &out, const CircleMenuHint &hint, int x, int y)
{
	const int lineHeight = 25;
	PrintGameStr(out, x + hint.x_mid - hint.top_w / 2, y, hint.top, CircleMenuHintTextColor(IsTopActive(hint)));
	y += lineHeight;

	PrintGameStr(out, x, y, hint.left, CircleMenuHintTextColor(IsLeftActive(hint)));
	PrintGameStr(out, x + hint.left_w + 5 * SpaceWidth(), y, hint.right, CircleMenuHintTextColor(IsRightActive(hint)));
	y += lineHeight;

	PrintGameStr(out, x + hint.x_mid - hint.bottom_w / 2, y, hint.bottom, CircleMenuHintTextColor(IsBottomActive(hint)));
}

const int CircleMarginX = 16;
const int CirclesTop = 76;

void DrawStartModifierMenu(const CelOutputBuffer &out)
{
	if (!start_modifier_active)
		return;
	static const CircleMenuHint dPad(/*is_dpad=*/true, /*top=*/_("Menu"), /*right=*/_("Inv"), /*bottom=*/_("Map"), /*left=*/_("Char"));
	static const CircleMenuHint buttons(/*is_dpad=*/false, /*top=*/"", /*right=*/"", /*bottom=*/_("Spells"), /*left=*/_("Quests"));
	DrawCircleMenuHint(out, dPad, PANEL_LEFT + CircleMarginX, PANEL_TOP - CirclesTop);
	DrawCircleMenuHint(out, buttons, PANEL_LEFT + PANEL_WIDTH - buttons.Width() - CircleMarginX, PANEL_TOP - CirclesTop);
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
	DrawCircleMenuHint(out, spells, PANEL_LEFT + PANEL_WIDTH - spells.Width() - CircleMarginX, PANEL_TOP - CirclesTop);
}

} // namespace

void DrawControllerModifierHints(const CelOutputBuffer &out)
{
	DrawStartModifierMenu(out);
	DrawSelectModifierMenu(out);
}

} // namespace devilution
