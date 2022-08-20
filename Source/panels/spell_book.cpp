#include "panels/spell_book.hpp"

#include <fmt/format.h>

#include "control.h"
#include "engine/clx_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/rectangle.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/render/text_render.hpp"
#include "init.h"
#include "missiles.h"
#include "panels/spell_icons.hpp"
#include "panels/ui_panels.hpp"
#include "player.h"
#include "spelldat.h"
#include "utils/language.h"
#include "utils/stdcompat/optional.hpp"

#define SPLICONLAST (gbIsHellfire ? 51 : 42)

namespace devilution {

OptionalOwnedClxSpriteList pSBkIconCels;

namespace {

OptionalOwnedClxSpriteList pSBkBtnCel;
OptionalOwnedClxSpriteList pSpellBkCel;

/** Maps from spellbook page number and position to spell_id. */
spell_id SpellPages[6][7] = {
	{ SPL_NULL, SPL_FIREBOLT, SPL_CBOLT, SPL_HBOLT, SPL_HEAL, SPL_HEALOTHER, SPL_FLAME },
	{ SPL_RESURRECT, SPL_FIREWALL, SPL_TELEKINESIS, SPL_LIGHTNING, SPL_TOWN, SPL_FLASH, SPL_STONE },
	{ SPL_RNDTELEPORT, SPL_MANASHIELD, SPL_ELEMENT, SPL_FIREBALL, SPL_WAVE, SPL_CHAIN, SPL_GUARDIAN },
	{ SPL_NOVA, SPL_GOLEM, SPL_TELEPORT, SPL_APOCA, SPL_BONESPIRIT, SPL_FLARE, SPL_ETHEREALIZE },
	{ SPL_LIGHTWALL, SPL_IMMOLAT, SPL_WARP, SPL_REFLECT, SPL_BERSERK, SPL_FIRERING, SPL_SEARCH },
	{ SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID }
};

constexpr Size SpellBookDescription { 250, 43 };
constexpr int SpellBookDescriptionPaddingHorizontal = 2;

void PrintSBookStr(const Surface &out, Point position, string_view text, UiFlags flags = UiFlags::None)
{
	DrawString(out, text,
	    Rectangle(GetPanelPosition(UiPanels::Spell, position + Displacement { SPLICONLENGTH, 0 }),
	        SpellBookDescription)
	        .inset({ SpellBookDescriptionPaddingHorizontal, 0 }),
	    UiFlags::ColorWhite | flags);
}

spell_type GetSBookTrans(spell_id ii, bool townok)
{
	Player &player = *MyPlayer;
	if ((player._pClass == HeroClass::Monk) && (ii == SPL_SEARCH))
		return RSPLTYPE_SKILL;
	spell_type st = RSPLTYPE_SPELL;
	if ((player._pISpells & GetSpellBitmask(ii)) != 0) {
		st = RSPLTYPE_CHARGES;
	}
	if ((player._pAblSpells & GetSpellBitmask(ii)) != 0) {
		st = RSPLTYPE_SKILL;
	}
	if (st == RSPLTYPE_SPELL) {
		if (CheckSpell(*MyPlayer, ii, st, true) != SpellCheckResult::Success) {
			st = RSPLTYPE_INVALID;
		}
		if (player.GetSpellLevel(ii) == 0) {
			st = RSPLTYPE_INVALID;
		}
	}
	if (townok && leveltype == DTYPE_TOWN && st != RSPLTYPE_INVALID && !spelldata[ii].sTownSpell) {
		st = RSPLTYPE_INVALID;
	}

	return st;
}

} // namespace

void InitSpellBook()
{
	pSpellBkCel = LoadCel("data\\spellbk.cel", static_cast<uint16_t>(SidePanelSize.width));
	pSBkBtnCel = LoadCel("data\\spellbkb.cel", gbIsHellfire ? 61 : 76);
	pSBkIconCels = LoadCel("data\\spelli2.cel", 37);

	Player &player = *MyPlayer;
	if (player._pClass == HeroClass::Warrior) {
		SpellPages[0][0] = SPL_REPAIR;
	} else if (player._pClass == HeroClass::Rogue) {
		SpellPages[0][0] = SPL_DISARM;
	} else if (player._pClass == HeroClass::Sorcerer) {
		SpellPages[0][0] = SPL_RECHARGE;
	} else if (player._pClass == HeroClass::Monk) {
		SpellPages[0][0] = SPL_SEARCH;
	} else if (player._pClass == HeroClass::Bard) {
		SpellPages[0][0] = SPL_IDENTIFY;
	} else if (player._pClass == HeroClass::Barbarian) {
		SpellPages[0][0] = SPL_BLODBOIL;
	}
}

void FreeSpellBook()
{
	pSpellBkCel = std::nullopt;
	pSBkBtnCel = std::nullopt;
	pSBkIconCels = std::nullopt;
}

void DrawSpellBook(const Surface &out)
{
	ClxDraw(out, GetPanelPosition(UiPanels::Spell, { 0, 351 }), (*pSpellBkCel)[0]);
	if (gbIsHellfire && sbooktab < 5) {
		ClxDraw(out, GetPanelPosition(UiPanels::Spell, { 61 * sbooktab + 7, 348 }), (*pSBkBtnCel)[sbooktab]);
	} else {
		// BUGFIX: rendering of page 3 and page 4 buttons are both off-by-one pixel (fixed).
		int sx = 76 * sbooktab + 7;
		if (sbooktab == 2 || sbooktab == 3) {
			sx++;
		}
		ClxDraw(out, GetPanelPosition(UiPanels::Spell, { sx, 348 }), (*pSBkBtnCel)[sbooktab]);
	}
	Player &player = *MyPlayer;
	uint64_t spl = player._pMemSpells | player._pISpells | player._pAblSpells;

	const int lineHeight = 18;

	int yp = 12;
	const int textPaddingTop = 7;
	for (int i = 1; i < 8; i++) {
		spell_id sn = SpellPages[sbooktab][i - 1];
		if (IsValidSpell(sn) && (spl & GetSpellBitmask(sn)) != 0) {
			spell_type st = GetSBookTrans(sn, true);
			SetSpellTrans(st);
			const Point spellCellPosition = GetPanelPosition(UiPanels::Spell, { 11, yp + SpellBookDescription.height });
			DrawSpellCel(out, spellCellPosition, *pSBkIconCels, SpellITbl[sn]);
			if (sn == player._pRSpell && st == player._pRSplType) {
				SetSpellTrans(RSPLTYPE_SKILL);
				DrawSpellCel(out, spellCellPosition, *pSBkIconCels, SPLICONLAST);
			}

			const Point line0 { 0, yp + textPaddingTop };
			const Point line1 { 0, yp + textPaddingTop + lineHeight };
			PrintSBookStr(out, line0, pgettext("spell", spelldata[sn].sNameText));
			switch (GetSBookTrans(sn, false)) {
			case RSPLTYPE_SKILL:
				PrintSBookStr(out, line1, _("Skill"));
				break;
			case RSPLTYPE_CHARGES: {
				int charges = player.InvBody[INVLOC_HAND_LEFT]._iCharges;
				PrintSBookStr(out, line1, fmt::format(fmt::runtime(ngettext("Staff ({:d} charge)", "Staff ({:d} charges)", charges)), charges));
			} break;
			default: {
				int mana = GetManaAmount(player, sn) >> 6;
				int lvl = player.GetSpellLevel(sn);
				PrintSBookStr(out, line0, fmt::format(fmt::runtime(pgettext(/* TRANSLATORS: UI constraints, keep short please.*/ "spellbook", "Level {:d}")), lvl), UiFlags::AlignRight);
				if (lvl == 0) {
					PrintSBookStr(out, line1, _("Unusable"), UiFlags::AlignRight);
				} else {
					if (sn != SPL_BONESPIRIT) {
						int min;
						int max;
						GetDamageAmt(sn, &min, &max);
						if (min != -1) {
							if (sn == SPL_HEAL || sn == SPL_HEALOTHER) {
								PrintSBookStr(out, line1, fmt::format(fmt::runtime(_(/* TRANSLATORS: UI constraints, keep short please.*/ "Heals: {:d} - {:d}")), min, max), UiFlags::AlignRight);
							} else {
								PrintSBookStr(out, line1, fmt::format(fmt::runtime(_(/* TRANSLATORS: UI constraints, keep short please.*/ "Damage: {:d} - {:d}")), min, max), UiFlags::AlignRight);
							}
						}
					} else {
						PrintSBookStr(out, line1, _(/* TRANSLATORS: UI constraints, keep short please.*/ "Dmg: 1/3 target hp"), UiFlags::AlignRight);
					}
					PrintSBookStr(out, line1, fmt::format(fmt::runtime(pgettext(/* TRANSLATORS: UI constraints, keep short please.*/ "spellbook", "Mana: {:d}")), mana));
				}
			} break;
			}
		}
		yp += SpellBookDescription.height;
	}
}

void CheckSBook()
{
	// Icons are drawn in a column near the left side of the panel and aligned with the spell book description entries
	// Spell icons/buttons are 37x38 pixels, laid out from 11,18 with a 5 pixel margin between each icon. This is close
	// enough to the height of the space given to spell descriptions that we can reuse that value and subtract the
	// padding from the end of the area.
	Rectangle iconArea = { GetPanelPosition(UiPanels::Spell, { 11, 18 }), Size { 37, SpellBookDescription.height * 7 - 5 } };
	if (iconArea.contains(MousePosition)) {
		spell_id sn = SpellPages[sbooktab][(MousePosition.y - iconArea.position.y) / SpellBookDescription.height];
		Player &player = *MyPlayer;
		uint64_t spl = player._pMemSpells | player._pISpells | player._pAblSpells;
		if (IsValidSpell(sn) && (spl & GetSpellBitmask(sn)) != 0) {
			spell_type st = RSPLTYPE_SPELL;
			if ((player._pISpells & GetSpellBitmask(sn)) != 0) {
				st = RSPLTYPE_CHARGES;
			}
			if ((player._pAblSpells & GetSpellBitmask(sn)) != 0) {
				st = RSPLTYPE_SKILL;
			}
			player._pRSpell = sn;
			player._pRSplType = st;
			force_redraw = 255;
		}
		return;
	}

	// The width of the panel excluding the border is 305 pixels. This does not cleanly divide by 4 meaning Diablo tabs
	// end up with an extra pixel somewhere around the buttons. Vanilla Diablo had the buttons left-aligned, devilutionX
	// instead justifies the buttons and puts the gap between buttons 2/3. See DrawSpellBook
	const int TabWidth = gbIsHellfire ? 61 : 76;
	// Tabs are drawn in a row near the bottom of the panel
	Rectangle tabArea = { GetPanelPosition(UiPanels::Spell, { 7, 320 }), Size { 305, 29 } };
	if (tabArea.contains(MousePosition)) {
		int hitColumn = MousePosition.x - tabArea.position.x;
		// Clicking on the gutter currently activates tab 3. Could make it do nothing by checking for == here and return early.
		if (!gbIsHellfire && hitColumn > TabWidth * 2) {
			// Subtract 1 pixel to account for the gutter between buttons 2/3
			hitColumn--;
		}
		sbooktab = hitColumn / TabWidth;
	}
}

} // namespace devilution
