#include "controls/modifier_hints.h"

#include <cstddef>

#include "all.h"
#include "controls/controller.h"
#include "controls/game_controls.h"
#include "options.h"

namespace dvl {

namespace {

int CalculateTextWidth(const char *s)
{
	int l = 0;
	while (*s) {
		l += fontkern[fontframe[gbFontTransTbl[static_cast<unsigned char>(*s++)]]] + 1;
	}
	return l;
}

int SpaceWidth()
{
	static const int kSpaceWidth = CalculateTextWidth(" ");
	return kSpaceWidth;
}

struct CircleMenuHint {
	CircleMenuHint(bool is_dpad, const char *top, const char *right, const char *bottom, const char *left)
	    : is_dpad(is_dpad)
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

	int width() const
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

void DrawCircleMenuHint(CelOutputBuffer out, const CircleMenuHint &hint, int x, int y)
{
	const int kLineHeight = 25;
	PrintGameStr(out, x + hint.x_mid - hint.top_w / 2, y, hint.top, CircleMenuHintTextColor(IsTopActive(hint)));
	y += kLineHeight;

	PrintGameStr(out, x, y, hint.left, CircleMenuHintTextColor(IsLeftActive(hint)));
	PrintGameStr(out, x + hint.left_w + 5 * SpaceWidth(), y, hint.right, CircleMenuHintTextColor(IsRightActive(hint)));
	y += kLineHeight;

	PrintGameStr(out, x + hint.x_mid - hint.bottom_w / 2, y, hint.bottom, CircleMenuHintTextColor(IsBottomActive(hint)));
}

const int kCircleMarginX = 16;
const int kCirclesTop = 76;

void DrawStartModifierMenu(CelOutputBuffer out)
{
	if (!start_modifier_active)
		return;
	static const CircleMenuHint kDpad(/*is_dpad=*/true, /*top=*/"Menu", /*right=*/"Inv", /*bottom=*/"Map", /*left=*/"Char");
	static const CircleMenuHint kButtons(/*is_dpad=*/false, /*top=*/"", /*right=*/"", /*bottom=*/"Spells", /*left=*/"Quests");
	DrawCircleMenuHint(out, kDpad, PANEL_LEFT + kCircleMarginX, PANEL_TOP - kCirclesTop);
	DrawCircleMenuHint(out, kButtons, PANEL_LEFT + PANEL_WIDTH - kButtons.width() - kCircleMarginX, PANEL_TOP - kCirclesTop);
}

void DrawSelectModifierMenu(CelOutputBuffer out)
{
	if (!select_modifier_active)
		return;
	if (sgOptions.Controller.bDpadHotkeys) {
		static const CircleMenuHint kDpad(/*is_dpad=*/true, /*top=*/"F6", /*right=*/"F8", /*bottom=*/"F7", /*left=*/"F5");
		DrawCircleMenuHint(out, kDpad, PANEL_LEFT + kCircleMarginX, PANEL_TOP - kCirclesTop);
	}
	static const CircleMenuHint kSpells(/*is_dpad=*/false, "F6", "F8", "F7", "F5");
	DrawCircleMenuHint(out, kSpells, PANEL_LEFT + PANEL_WIDTH - kSpells.width() - kCircleMarginX, PANEL_TOP - kCirclesTop);
}

} // namespace

void DrawControllerModifierHints(CelOutputBuffer out)
{
	DrawStartModifierMenu(out);
	DrawSelectModifierMenu(out);
}

} // namespace dvl
