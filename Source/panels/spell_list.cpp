#include "panels/spell_list.hpp"

#include <fmt/format.h>

#include "control.h"
#include "controls/keymapper.hpp"
#include "engine.h"
#include "engine/render/text_render.hpp"
#include "inv_iterators.hpp"
#include "palette.h"
#include "panels/spell_icons.hpp"
#include "player.h"
#include "spells.h"
#include "utils/language.h"

#define SPLROWICONLS 10

namespace devilution {

extern std::array<Keymapper::ActionIndex, 4> quickSpellActionIndexes;

namespace {

void PrintSBookSpellType(const Surface &out, Point position, const std::string &text, uint8_t rectColorIndex)
{
	Point rect { position };
	rect += Displacement { 0, -SPLICONLENGTH + 1 };

	// Top
	DrawHorizontalLine(out, rect, SPLICONLENGTH, rectColorIndex);
	DrawHorizontalLine(out, rect + Displacement { 0, 1 }, SPLICONLENGTH, rectColorIndex);

	// Bottom
	DrawHorizontalLine(out, rect + Displacement { 0, SPLICONLENGTH - 2 }, SPLICONLENGTH, rectColorIndex);
	DrawHorizontalLine(out, rect + Displacement { 0, SPLICONLENGTH - 1 }, SPLICONLENGTH, rectColorIndex);

	// Left Side
	DrawVerticalLine(out, rect, SPLICONLENGTH, rectColorIndex);
	DrawVerticalLine(out, rect + Displacement { 1, 0 }, SPLICONLENGTH, rectColorIndex);

	// Right Side
	DrawVerticalLine(out, rect + Displacement { SPLICONLENGTH - 2, 0 }, SPLICONLENGTH, rectColorIndex);
	DrawVerticalLine(out, rect + Displacement { SPLICONLENGTH - 1, 0 }, SPLICONLENGTH, rectColorIndex);

	// Align the spell type text with bottom of spell icon
	position += Displacement { SPLICONLENGTH / 2 - GetLineWidth(text.c_str()) / 2, (IsSmallFontTall() ? -19 : -15) };

	// Draw a drop shadow below and to the left of the text
	DrawString(out, text, position + Displacement { -1, 1 }, UiFlags::ColorBlack);
	DrawString(out, text, position + Displacement { -1, -1 }, UiFlags::ColorBlack);
	DrawString(out, text, position + Displacement { 1, -1 }, UiFlags::ColorBlack);
	// Then draw the text over the top
	DrawString(out, text, position, UiFlags::ColorWhite);
}

void PrintSBookHotkey(const Surface &out, Point position, const std::string &text)
{
	// Align the hot key text with the top-right corner of the spell icon
	position += Displacement { SPLICONLENGTH - (GetLineWidth(text.c_str()) + 5), 5 - SPLICONLENGTH };

	// Draw a drop shadow below and to the left of the text
	DrawString(out, text, position + Displacement { -1, 1 }, UiFlags::ColorBlack);
	// Then draw the text over the top
	DrawString(out, text, position, UiFlags::ColorWhite);
}

bool GetSpellListSelection(spell_id &pSpell, spell_type &pSplType)
{
	pSpell = spell_id::SPL_INVALID;
	pSplType = spell_type::RSPLTYPE_INVALID;
	auto &myPlayer = Players[MyPlayerId];

	for (auto &spellListItem : GetSpellListItems()) {
		if (spellListItem.isSelected) {
			pSpell = spellListItem.id;
			pSplType = spellListItem.type;
			if (myPlayer._pClass == HeroClass::Monk && spellListItem.id == SPL_SEARCH)
				pSplType = RSPLTYPE_SKILL;
			return true;
		}
	}

	return false;
}

std::optional<std::string> GetHotkeyName(spell_id spellId, spell_type spellType)
{
	auto &myPlayer = Players[MyPlayerId];
	for (int t = 0; t < 4; t++) {
		if (myPlayer._pSplHotKey[t] != spellId || myPlayer._pSplTHotKey[t] != spellType)
			continue;
		return keymapper.KeyNameForAction(quickSpellActionIndexes[t]);
	}
	return {};
}

} // namespace

void DrawSpell(const Surface &out)
{
	auto &myPlayer = Players[MyPlayerId];
	spell_id spl = myPlayer._pRSpell;
	spell_type st = myPlayer._pRSplType;

	// BUGFIX: Move the next line into the if statement to avoid OOB (SPL_INVALID is -1) (fixed)
	if (st == RSPLTYPE_SPELL && spl != SPL_INVALID) {
		int tlvl = myPlayer._pISplLvlAdd + myPlayer._pSplLvl[spl];
		if (CheckSpell(MyPlayerId, spl, st, true) != SpellCheckResult::Success)
			st = RSPLTYPE_INVALID;
		if (tlvl <= 0)
			st = RSPLTYPE_INVALID;
	}
	if (currlevel == 0 && st != RSPLTYPE_INVALID && !spelldata[spl].sTownSpell)
		st = RSPLTYPE_INVALID;
	SetSpellTrans(st);
	const int nCel = (spl != SPL_INVALID) ? SpellITbl[spl] : 27;
	const Point position { PANEL_X + 565, PANEL_Y + 119 };
	DrawSpellCel(out, position, nCel);

	std::optional<std::string> hotkeyName = GetHotkeyName(spl, myPlayer._pRSplType);
	if (hotkeyName)
		PrintSBookHotkey(out, position, *hotkeyName);
}

void DrawSpellList(const Surface &out)
{
	infostr[0] = '\0';
	ClearPanel();

	auto &myPlayer = Players[MyPlayerId];

	for (auto &spellListItem : GetSpellListItems()) {
		const spell_id spellId = spellListItem.id;
		spell_type transType = spellListItem.type;
		int spellLevel = 0;
		const SpellData &spellDataItem = spelldata[static_cast<size_t>(spellListItem.id)];
		if (currlevel == 0 && !spellDataItem.sTownSpell) {
			transType = RSPLTYPE_INVALID;
		}
		if (spellListItem.type == RSPLTYPE_SPELL) {
			spellLevel = std::max(myPlayer._pISplLvlAdd + myPlayer._pSplLvl[spellListItem.id], 0);
			if (spellLevel == 0)
				transType = RSPLTYPE_INVALID;
		}

		SetSpellTrans(transType);
		DrawSpellCel(out, spellListItem.location, SpellITbl[static_cast<size_t>(spellId)]);

		std::optional<std::string> hotkeyName = GetHotkeyName(spellId, spellListItem.type);

		if (hotkeyName)
			PrintSBookHotkey(out, spellListItem.location, *hotkeyName);

		if (!spellListItem.isSelected)
			continue;

		uint8_t spellColor = PAL16_GRAY + 5;

		switch (spellListItem.type) {
		case RSPLTYPE_SKILL:
			spellColor = PAL16_YELLOW - 46;
			PrintSBookSpellType(out, spellListItem.location, _("Skill"), spellColor);
			strcpy(infostr, fmt::format(_("{:s} Skill"), pgettext("spell", spellDataItem.sSkillText)).c_str());
			break;
		case RSPLTYPE_SPELL:
			if (myPlayer.plrlevel != 0) {
				spellColor = PAL16_BLUE + 5;
			}
			PrintSBookSpellType(out, spellListItem.location, _("Spell"), spellColor);
			strcpy(infostr, fmt::format(_("{:s} Spell"), pgettext("spell", spellDataItem.sNameText)).c_str());
			if (spellId == SPL_HBOLT) {
				strcpy(tempstr, _("Damages undead only"));
				AddPanelString(tempstr);
			}
			if (spellLevel == 0)
				strcpy(tempstr, _("Spell Level 0 - Unusable"));
			else
				strcpy(tempstr, fmt::format(_("Spell Level {:d}"), spellLevel).c_str());
			AddPanelString(tempstr);
			break;
		case RSPLTYPE_SCROLL: {
			if (myPlayer.plrlevel != 0) {
				spellColor = PAL16_RED - 59;
			}
			PrintSBookSpellType(out, spellListItem.location, _("Scroll"), spellColor);
			strcpy(infostr, fmt::format(_("Scroll of {:s}"), pgettext("spell", spellDataItem.sNameText)).c_str());
			const InventoryAndBeltPlayerItemsRange items { myPlayer };
			const int scrollCount = std::count_if(items.begin(), items.end(), [spellId](const Item &item) {
				return item.IsScrollOf(spellId);
			});
			strcpy(tempstr, fmt::format(ngettext("{:d} Scroll", "{:d} Scrolls", scrollCount), scrollCount).c_str());
			AddPanelString(tempstr);
		} break;
		case RSPLTYPE_CHARGES: {
			if (myPlayer.plrlevel != 0) {
				spellColor = PAL16_ORANGE + 5;
			}
			PrintSBookSpellType(out, spellListItem.location, _("Staff"), spellColor);
			strcpy(infostr, fmt::format(_("Staff of {:s}"), pgettext("spell", spellDataItem.sNameText)).c_str());
			int charges = myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges;
			strcpy(tempstr, fmt::format(ngettext("{:d} Charge", "{:d} Charges", charges), charges).c_str());
			AddPanelString(tempstr);
		} break;
		case RSPLTYPE_INVALID:
			break;
		}
		if (hotkeyName) {
			strcpy(tempstr, fmt::format(_("Spell Hotkey {:s}"), *hotkeyName).c_str());
			AddPanelString(tempstr);
		}
	}
}

std::vector<SpellListItem> GetSpellListItems()
{
	std::vector<SpellListItem> spellListItems;

	uint64_t mask;

	int x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
	int y = PANEL_Y - 17;

	for (int i = RSPLTYPE_SKILL; i < RSPLTYPE_INVALID; i++) {
		auto &myPlayer = Players[MyPlayerId];
		switch ((spell_type)i) {
		case RSPLTYPE_SKILL:
			mask = myPlayer._pAblSpells;
			break;
		case RSPLTYPE_SPELL:
			mask = myPlayer._pMemSpells;
			break;
		case RSPLTYPE_SCROLL:
			mask = myPlayer._pScrlSpells;
			break;
		case RSPLTYPE_CHARGES:
			mask = myPlayer._pISpells;
			break;
		case RSPLTYPE_INVALID:
			break;
		}
		int8_t j = SPL_FIREBOLT;
		for (uint64_t spl = 1; j < MAX_SPELLS; spl <<= 1, j++) {
			if ((mask & spl) == 0)
				continue;
			int lx = x;
			int ly = y - SPLICONLENGTH;
			bool isSelected = (MousePosition.x >= lx && MousePosition.x < lx + SPLICONLENGTH && MousePosition.y >= ly && MousePosition.y < ly + SPLICONLENGTH);
			spellListItems.emplace_back(SpellListItem { { x, y }, (spell_type)i, (spell_id)j, isSelected });
			x -= SPLICONLENGTH;
			if (x == PANEL_X + 12 - SPLICONLENGTH) {
				x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
				y -= SPLICONLENGTH;
			}
		}
		if (mask != 0 && x != PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS)
			x -= SPLICONLENGTH;
		if (x == PANEL_X + 12 - SPLICONLENGTH) {
			x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
			y -= SPLICONLENGTH;
		}
	}

	return spellListItems;
}

void SetSpell()
{
	spell_id pSpell;
	spell_type pSplType;

	spselflag = false;
	if (!GetSpellListSelection(pSpell, pSplType)) {
		return;
	}

	ClearPanel();

	auto &myPlayer = Players[MyPlayerId];
	myPlayer._pRSpell = pSpell;
	myPlayer._pRSplType = pSplType;

	force_redraw = 255;
}

void SetSpeedSpell(int slot)
{
	spell_id pSpell;
	spell_type pSplType;

	if (!GetSpellListSelection(pSpell, pSplType)) {
		return;
	}
	auto &myPlayer = Players[MyPlayerId];
	for (int i = 0; i < 4; ++i) {
		if (myPlayer._pSplHotKey[i] == pSpell && myPlayer._pSplTHotKey[i] == pSplType)
			myPlayer._pSplHotKey[i] = SPL_INVALID;
	}
	myPlayer._pSplHotKey[slot] = pSpell;
	myPlayer._pSplTHotKey[slot] = pSplType;
}

void ToggleSpell(int slot)
{
	uint64_t spells;

	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pSplHotKey[slot] == SPL_INVALID) {
		return;
	}

	switch (myPlayer._pSplTHotKey[slot]) {
	case RSPLTYPE_SKILL:
		spells = myPlayer._pAblSpells;
		break;
	case RSPLTYPE_SPELL:
		spells = myPlayer._pMemSpells;
		break;
	case RSPLTYPE_SCROLL:
		spells = myPlayer._pScrlSpells;
		break;
	case RSPLTYPE_CHARGES:
		spells = myPlayer._pISpells;
		break;
	case RSPLTYPE_INVALID:
		return;
	}

	if ((spells & GetSpellBitmask(myPlayer._pSplHotKey[slot])) != 0) {
		myPlayer._pRSpell = myPlayer._pSplHotKey[slot];
		myPlayer._pRSplType = myPlayer._pSplTHotKey[slot];
		force_redraw = 255;
	}
}

void DoSpeedBook()
{
	spselflag = true;
	int xo = PANEL_X + 12 + SPLICONLENGTH * 10;
	int yo = PANEL_Y - 17;
	int x = xo + SPLICONLENGTH / 2;
	int y = yo - SPLICONLENGTH / 2;

	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pRSpell != SPL_INVALID) {
		for (int i = RSPLTYPE_SKILL; i <= RSPLTYPE_CHARGES; i++) {
			uint64_t spells;
			switch (i) {
			case RSPLTYPE_SKILL:
				spells = myPlayer._pAblSpells;
				break;
			case RSPLTYPE_SPELL:
				spells = myPlayer._pMemSpells;
				break;
			case RSPLTYPE_SCROLL:
				spells = myPlayer._pScrlSpells;
				break;
			case RSPLTYPE_CHARGES:
				spells = myPlayer._pISpells;
				break;
			}
			uint64_t spell = 1;
			for (int j = 1; j < MAX_SPELLS; j++) {
				if ((spell & spells) != 0) {
					if (j == myPlayer._pRSpell && i == myPlayer._pRSplType) {
						x = xo + SPLICONLENGTH / 2;
						y = yo - SPLICONLENGTH / 2;
					}
					xo -= SPLICONLENGTH;
					if (xo == PANEL_X + 12 - SPLICONLENGTH) {
						xo = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
						yo -= SPLICONLENGTH;
					}
				}
				spell <<= 1ULL;
			}
			if (spells != 0 && xo != PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS)
				xo -= SPLICONLENGTH;
			if (xo == PANEL_X + 12 - SPLICONLENGTH) {
				xo = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
				yo -= SPLICONLENGTH;
			}
		}
	}

	SetCursorPos({ x, y });
}

} // namespace devilution
