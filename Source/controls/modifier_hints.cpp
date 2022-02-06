#include "controls/modifier_hints.h"

#include <cstddef>

#include "DiabloUI/ui_flags.hpp"
#include "control.h"
#include "controls/controller.h"
#include "controls/game_controls.h"
#include "engine/load_cel.hpp"
#include "engine/render/text_render.hpp"
#include "options.h"
#include "panels/spell_icons.hpp"
#include "utils/language.h"

namespace devilution {

extern std::optional<OwnedCelSprite> pSBkIconCels;

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
constexpr int CircleTop = 101;

/** Spell icon side size. */
constexpr int IconSize = 37;

/** Spell icon text right margin. */
constexpr int IconSizeTextMarginRight = 3;

/** Spell icon text top margin. */
constexpr int IconSizeTextMarginTop = 2;

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

UiFlags CircleMenuHintTextColor(bool active)
{
	return active ? UiFlags::ColorBlue : UiFlags::ColorWhitegold;
}

UiFlags CircleSpellMenuHintTextColor(bool active)
{
	return active ? UiFlags::ColorBlue : UiFlags::ColorWhite;
}

/**
 * @brief Draws hint text for a four button layout with the top/left edge of the bounding box at the position given by origin.
 * @param out The output buffer to draw on.
 * @param hint Struct describing the text to draw and the dimensions of the layout.
 * @param origin Top left corner of the layout (relative to the output buffer).
 */
void DrawCircleMenuHint(const Surface &out, const CircleMenuHint &hint, const Point &origin)
{
	DrawString(out, hint.top, origin + Displacement { hint.xMid - hint.topW / 2, 0 }, CircleMenuHintTextColor(IsTopActive(hint)));

	DrawString(out, hint.left, origin + Displacement { 0, LineHeight }, CircleMenuHintTextColor(IsLeftActive(hint)));
	DrawString(out, hint.right, origin + Displacement { hint.leftW + MidSpaces * SpaceWidth(), LineHeight }, CircleMenuHintTextColor(IsRightActive(hint)));

	DrawString(out, hint.bottom, origin + Displacement { hint.xMid - hint.bottomW / 2, LineHeight * 2 }, CircleMenuHintTextColor(IsBottomActive(hint)));
}

/**
 * @brief Draws hint text for a four button layout with the top/left edge of the bounding box at the position given by origin plus the icon for the spell mapped to that entry.
 * @param out The output buffer to draw on.
 * @param hint Struct describing the text to draw and the dimensions of the layout.
 * @param origin Top left corner of the layout (relative to the output buffer).
 */
void DrawSpellsCircleMenuHint(const Surface &out, const CircleMenuHint &hint, const Point &origin)
{
	const auto &myPlayer = Players[MyPlayerId];
	Point positions[4] = {
		origin + Displacement { 0, LineHeight },
		origin + Displacement { IconSize, LineHeight - IconSize },
		origin + Displacement { IconSize, LineHeight + IconSize },
		origin + Displacement { IconSize * 2, LineHeight }
	};
	Point textPositions[4] = {
		positions[0] + Displacement { IconSize - hint.leftW - IconSizeTextMarginRight, IconSizeTextMarginTop - IconSize },
		positions[1] + Displacement { IconSize - hint.topW - IconSizeTextMarginRight, IconSizeTextMarginTop - IconSize },
		positions[2] + Displacement { IconSize - hint.bottomW - IconSizeTextMarginRight, IconSizeTextMarginTop - IconSize },
		positions[3] + Displacement { IconSize - hint.rightW - IconSizeTextMarginRight, IconSizeTextMarginTop - IconSize }
	};
	const char *texts[4] = { hint.left, hint.top, hint.bottom, hint.right };
	bool isActive[4] = { IsLeftActive(hint), IsTopActive(hint), IsBottomActive(hint), IsRightActive(hint) };
	uint64_t spells = myPlayer._pAblSpells | myPlayer._pMemSpells | myPlayer._pScrlSpells | myPlayer._pISpells;
	spell_id splId;
	spell_type splType;
	Point textPosition;

	for (int slot = 0; slot < 4; ++slot) {
		splId = myPlayer._pSplHotKey[slot];

		if (splId != SPL_INVALID && (spells & GetSpellBitmask(splId)) != 0)
			splType = (currlevel == 0 && !spelldata[splId].sTownSpell) ? RSPLTYPE_INVALID : myPlayer._pSplTHotKey[slot];
		else {
			splType = RSPLTYPE_INVALID;
			splId = SPL_NULL;
		}

		SetSpellTrans(splType);
		DrawSpellCel(out, positions[slot], *pSBkIconCels, SpellITbl[splId]);
		textPosition = textPositions[slot];
		// Drop shadow
		DrawString(out, texts[slot], textPosition + Displacement { -1, 1 }, UiFlags::ColorBlack);
		DrawString(out, texts[slot], textPosition, CircleSpellMenuHintTextColor(isActive[slot]));
	}
}

void DrawStartModifierMenu(const Surface &out)
{
	if (!start_modifier_active)
		return;
	static const CircleMenuHint DPad(/*isDpad=*/true, /*top=*/_("Menu"), /*right=*/_("Inv"), /*bottom=*/_("Map"), /*left=*/_("Char"));
	static const CircleMenuHint Buttons(/*isDpad=*/false, /*top=*/"", /*right=*/"", /*bottom=*/_("Spells"), /*left=*/_("Quests"));
	DrawCircleMenuHint(out, DPad, { PANEL_LEFT + CircleMarginX, PANEL_TOP - CircleTop });
	DrawCircleMenuHint(out, Buttons, { PANEL_LEFT + PANEL_WIDTH - Buttons.Width() - CircleMarginX, PANEL_TOP - CircleTop });
}

void DrawSelectModifierMenu(const Surface &out)
{
	if (!select_modifier_active)
		return;

	if (sgOptions.Controller.bDpadHotkeys) {
		static const CircleMenuHint DPad(/*isDpad=*/true, /*top=*/"F6", /*right=*/"F8", /*bottom=*/"F7", /*left=*/"F5");
		DrawSpellsCircleMenuHint(out, DPad, { PANEL_LEFT + CircleMarginX, PANEL_TOP - CircleTop });
	}
	static const CircleMenuHint Spells(/*isDpad=*/false, "F6", "F8", "F7", "F5");
	DrawSpellsCircleMenuHint(out, Spells, { PANEL_LEFT + PANEL_WIDTH - IconSize * 3 - CircleMarginX, PANEL_TOP - CircleTop });
}

} // namespace

void DrawControllerModifierHints(const Surface &out)
{
	DrawStartModifierMenu(out);
	DrawSelectModifierMenu(out);
}

} // namespace devilution
