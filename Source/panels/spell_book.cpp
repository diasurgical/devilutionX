#include "panels/spell_book.hpp"

#include <fmt/format.h>

#include "control.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/rectangle.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "init.h"
#include "missiles.h"
#include "panels/spell_icons.hpp"
#include "panels/ui_panels.hpp"
#include "player.h"
#include "spelldat.h"
#include "utils/language.h"
#include "utils/stdcompat/optional.hpp"

#define SPLICONLAST (gbIsHellfire ? 52 : 43)

namespace devilution {
namespace {

std::optional<CelSprite> pSBkBtnCel;
std::optional<CelSprite> pSBkIconCels;
std::optional<CelSprite> pSpellBkCel;

/** Maps from spellbook page number and position to spell_id. */
spell_id SpellPages[6][7] = {
	{ SPL_NULL, SPL_FIREBOLT, SPL_CBOLT, SPL_HBOLT, SPL_HEAL, SPL_HEALOTHER, SPL_FLAME },
	{ SPL_RESURRECT, SPL_FIREWALL, SPL_TELEKINESIS, SPL_LIGHTNING, SPL_TOWN, SPL_FLASH, SPL_STONE },
	{ SPL_RNDTELEPORT, SPL_MANASHIELD, SPL_ELEMENT, SPL_FIREBALL, SPL_WAVE, SPL_CHAIN, SPL_GUARDIAN },
	{ SPL_NOVA, SPL_GOLEM, SPL_TELEPORT, SPL_APOCA, SPL_BONESPIRIT, SPL_FLARE, SPL_ETHEREALIZE },
	{ SPL_LIGHTWALL, SPL_IMMOLAT, SPL_WARP, SPL_REFLECT, SPL_BERSERK, SPL_FIRERING, SPL_SEARCH },
	{ SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID }
};

constexpr int SpellBookDescriptionWidth = 250;
constexpr int SpellBookDescriptionHeight = 43;
constexpr int SpellBookDescriptionPaddingLeft = 2;
constexpr int SpellBookDescriptionPaddingRight = 2;

void PrintSBookStr(const Surface &out, Point position, string_view text, UiFlags flags = UiFlags::None)
{
	DrawString(out, text,
	    { GetPanelPosition(UiPanels::Spell, { SPLICONLENGTH + SpellBookDescriptionPaddingLeft + position.x, position.y }),
	        { SpellBookDescriptionWidth - SpellBookDescriptionPaddingLeft - SpellBookDescriptionPaddingRight, 0 } },
	    UiFlags::ColorWhite | flags);
}

spell_type GetSBookTrans(spell_id ii, bool townok)
{
	auto &player = Players[MyPlayerId];
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
		if (CheckSpell(MyPlayerId, ii, st, true) != SpellCheckResult::Success) {
			st = RSPLTYPE_INVALID;
		}
		if ((char)(player._pSplLvl[ii] + player._pISplLvlAdd) <= 0) {
			st = RSPLTYPE_INVALID;
		}
	}
	if (townok && currlevel == 0 && st != RSPLTYPE_INVALID && !spelldata[ii].sTownSpell) {
		st = RSPLTYPE_INVALID;
	}

	return st;
}

} // namespace

void InitSpellBook()
{
	pSpellBkCel = LoadCel("Data\\SpellBk.CEL", SPANEL_WIDTH);

	if (gbIsHellfire) {
		static const int SBkBtnHellfireWidths[] = { 0, 61, 61, 61, 61, 61, 76 };
		pSBkBtnCel = LoadCel("Data\\SpellBkB.CEL", SBkBtnHellfireWidths);
	} else {
		pSBkBtnCel = LoadCel("Data\\SpellBkB.CEL", 76);
	}
	pSBkIconCels = LoadCel("Data\\SpellI2.CEL", 37);

	Player &player = Players[MyPlayerId];
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
	CelDrawTo(out, GetPanelPosition(UiPanels::Spell, { 0, 351 }), *pSpellBkCel, 1);
	if (gbIsHellfire && sbooktab < 5) {
		CelDrawTo(out, GetPanelPosition(UiPanels::Spell, { 61 * sbooktab + 7, 348 }), *pSBkBtnCel, sbooktab + 1);
	} else {
		// BUGFIX: rendering of page 3 and page 4 buttons are both off-by-one pixel (fixed).
		int sx = 76 * sbooktab + 7;
		if (sbooktab == 2 || sbooktab == 3) {
			sx++;
		}
		CelDrawTo(out, GetPanelPosition(UiPanels::Spell, { sx, 348 }), *pSBkBtnCel, sbooktab + 1);
	}
	auto &player = Players[MyPlayerId];
	uint64_t spl = player._pMemSpells | player._pISpells | player._pAblSpells;

	const int lineHeight = 18;

	int yp = 12;
	const int textPaddingTop = 7;
	for (int i = 1; i < 8; i++) {
		spell_id sn = SpellPages[sbooktab][i - 1];
		if (sn != SPL_INVALID && (spl & GetSpellBitmask(sn)) != 0) {
			spell_type st = GetSBookTrans(sn, true);
			SetSpellTrans(st);
			const Point spellCellPosition = GetPanelPosition(UiPanels::Spell, { 11, yp + SpellBookDescriptionHeight });
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
				PrintSBookStr(out, line1, fmt::format(ngettext("Staff ({:d} charge)", "Staff ({:d} charges)", charges), charges));
			} break;
			default: {
				int mana = GetManaAmount(player, sn) >> 6;
				if (sn != SPL_BONESPIRIT) {
					int min;
					int max;
					GetDamageAmt(sn, &min, &max);
					if (min != -1) {
						if (sn == SPL_HEAL || sn == SPL_HEALOTHER) {
							PrintSBookStr(out, line1, fmt::format(_(/* TRANSLATORS: UI constrains, keep short please.*/ "Heals: {:d} - {:d}"), min, max), UiFlags::AlignRight);
						} else {
							PrintSBookStr(out, line1, fmt::format(_(/* TRANSLATORS: UI constrains, keep short please.*/ "Damage: {:d} - {:d}"), min, max), UiFlags::AlignRight);
						}
					}
				} else {
					PrintSBookStr(out, line1, _(/* TRANSLATORS: UI constrains, keep short please.*/ "Dmg: 1/3 target hp"), UiFlags::AlignRight);
				}
				PrintSBookStr(out, line1, fmt::format(pgettext(/* TRANSLATORS: UI constrains, keep short please.*/ "spellbook", "Mana: {:d}"), mana));
				int lvl = std::max(player._pSplLvl[sn] + player._pISplLvlAdd, 0);
				if (lvl == 0) {
					PrintSBookStr(out, line0, _("Level 0 - Unusable"), UiFlags::AlignRight);
				} else {
					PrintSBookStr(out, line0, fmt::format(pgettext(/* TRANSLATORS: UI constrains, keep short please.*/ "spellbook", "Level {:d}"), lvl), UiFlags::AlignRight);
				}
			} break;
			}
		}
		yp += SpellBookDescriptionHeight;
	}
}

void CheckSBook()
{
	Rectangle iconArea = { GetPanelPosition(UiPanels::Spell, { 11, 18 }), { 48 - 11, 314 - 18 } };
	Rectangle tabArea = { GetPanelPosition(UiPanels::Spell, { 7, 320 }), { 311 - 7, 349 - 320 } };
	if (iconArea.Contains(MousePosition)) {
		spell_id sn = SpellPages[sbooktab][(MousePosition.y - GetRightPanel().position.y - 18) / 43];
		auto &player = Players[MyPlayerId];
		uint64_t spl = player._pMemSpells | player._pISpells | player._pAblSpells;
		if (sn != SPL_INVALID && (spl & GetSpellBitmask(sn)) != 0) {
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
	}
	if (tabArea.Contains(MousePosition)) {
		sbooktab = (MousePosition.x - (GetRightPanel().position.x + 7)) / (gbIsHellfire ? 61 : 76);
	}
}

} // namespace devilution
