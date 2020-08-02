#include "controls/modifier_hints.h"

#include <cstddef>

#include "all.h"
#include "controls/controller.h"
#include "controls/game_controls.h"

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

void DrawCircleMenuHint(const CircleMenuHint &hint, int x, int y)
{
	const int kLineHeight = 25;
	PrintGameStr(x + hint.x_mid - hint.top_w / 2, y, hint.top, CircleMenuHintTextColor(IsTopActive(hint)));
	y += kLineHeight;

	PrintGameStr(x, y, hint.left, CircleMenuHintTextColor(IsLeftActive(hint)));
	PrintGameStr(x + hint.left_w + 5 * SpaceWidth(), y, hint.right, CircleMenuHintTextColor(IsRightActive(hint)));
	y += kLineHeight;

	PrintGameStr(x + hint.x_mid - hint.bottom_w / 2, y, hint.bottom, CircleMenuHintTextColor(IsBottomActive(hint)));
}

const int kCircleMarginX = 16;
const int kCirclesTop = PANEL_TOP - 76;

void DrawStartModifierMenu()
{
	if (!start_modifier_active)
		return;
	static const CircleMenuHint kDpad(/*is_dpad=*/true, /*top=*/"Menu", /*right=*/"Inv", /*bottom=*/"Map", /*left=*/"Char");
	static const CircleMenuHint kButtons(/*is_dpad=*/false, /*top=*/"", /*right=*/"", /*bottom=*/"Spells", /*left=*/"Quests");
	DrawCircleMenuHint(kDpad, kCircleMarginX, kCirclesTop);
	DrawCircleMenuHint(kButtons, SCREEN_WIDTH - kButtons.width() - kCircleMarginX, kCirclesTop);
}

void DrawSelectModifierMenu()
{
	if (!select_modifier_active)
		return;
	static const CircleMenuHint kSpells(/*is_dpad=*/false, "F6", "F8", "F7", "F5");
	DrawCircleMenuHint(kSpells, SCREEN_WIDTH - kSpells.width() - kCircleMarginX, kCirclesTop);
}

} // namespace

void DrawControllerModifierHints()
{
	DrawStartModifierMenu();
	DrawSelectModifierMenu();
}

} // namespace dvl
