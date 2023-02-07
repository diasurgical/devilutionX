#include "panels/spell_book.hpp"

#include <fmt/format.h>

#include "control.h"
#include "engine/backbuffer_state.hpp"
#include "engine/clx_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/load_clx.hpp"
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
#include "utils/str_cat.hpp"
#include <vector>

#define SPLICONLAST (gbIsHellfire ? 51 : 42)

namespace devilution {

namespace {

OptionalOwnedClxSpriteList pSBkBtnCel;
OptionalOwnedClxSpriteList pSpellBkCel;

/** Maps from spellbook page number and position to SpellID. */
SpellID SpellPages[6][7] = {
	{ SpellID::Null, SpellID::Firebolt, SpellID::ChargedBolt, SpellID::HolyBolt, SpellID::Healing, SpellID::HealOther, SpellID::Inferno },
	{ SpellID::Resurrect, SpellID::FireWall, SpellID::Telekinesis, SpellID::Lightning, SpellID::TownPortal, SpellID::Flash, SpellID::StoneCurse },
	{ SpellID::Phasing, SpellID::ManaShield, SpellID::Elemental, SpellID::Fireball, SpellID::FlameWave, SpellID::ChainLightning, SpellID::Guardian },
	{ SpellID::Nova, SpellID::Golem, SpellID::Teleport, SpellID::Apocalypse, SpellID::BoneSpirit, SpellID::BloodStar, SpellID::Etherealize },
	{ SpellID::LightningWall, SpellID::Immolation, SpellID::Warp, SpellID::Reflect, SpellID::Berserk, SpellID::RingOfFire, SpellID::Search },
	{ SpellID::Invalid, SpellID::Invalid, SpellID::Invalid, SpellID::Invalid, SpellID::Invalid, SpellID::Invalid, SpellID::Invalid }
};

constexpr Size SpellBookDescription { 250, 43 };
constexpr int SpellBookDescriptionPaddingHorizontal = 2;

void PrintSBookStr(const Surface &out, Point position, string_view text, UiFlags flags = UiFlags::None)
{
	DrawString(out, text,
	    Rectangle(GetPanelPosition(UiPanels::Spell, position + Displacement { SPLICONLENGTH, 0 }),
	        SpellBookDescription)
	        .inset({ SpellBookDescriptionPaddingHorizontal, 0 }),
	    flags);
}

SpellType GetSBookTrans(SpellID ii, bool townok)
{
	Player &player = *MyPlayer;
	if ((player._pClass == HeroClass::Monk) && (ii == SpellID::Search))
		return SpellType::Skill;
	SpellType st = SpellType::Spell;
	if ((player._pISpells & GetSpellBitmask(ii)) != 0) {
		st = SpellType::Charges;
	}
	if ((player._pAblSpells & GetSpellBitmask(ii)) != 0) {
		st = SpellType::Skill;
	}
	if (st == SpellType::Spell) {
		if (CheckSpell(*MyPlayer, ii, st, true) != SpellCheckResult::Success) {
			st = SpellType::Invalid;
		}
		if (player.GetSpellLevel(ii) == 0) {
			st = SpellType::Invalid;
		}
	}
	if (townok && leveltype == DTYPE_TOWN && st != SpellType::Invalid && !GetSpellData(ii).sTownSpell) {
		st = SpellType::Invalid;
	}

	return st;
}

} // namespace

void InitSpellBook()
{
	pSpellBkCel = LoadCel("data\\spellbk", static_cast<uint16_t>(SidePanelSize.width));
	pSBkBtnCel = LoadCel("data\\spellbkb", gbIsHellfire ? 61 : 76);
	LoadSmallSpellIcons();

	Player &player = *MyPlayer;
	if (player._pClass == HeroClass::Warrior) {
		SpellPages[0][0] = SpellID::ItemRepair;
	} else if (player._pClass == HeroClass::Rogue) {
		SpellPages[0][0] = SpellID::TrapDisarm;
	} else if (player._pClass == HeroClass::Sorcerer) {
		SpellPages[0][0] = SpellID::StaffRecharge;
	} else if (player._pClass == HeroClass::Monk) {
		SpellPages[0][0] = SpellID::Search;
	} else if (player._pClass == HeroClass::Bard) {
		SpellPages[0][0] = SpellID::Identify;
	} else if (player._pClass == HeroClass::Barbarian) {
		SpellPages[0][0] = SpellID::Rage;
	}
}

void FreeSpellBook()
{
	FreeSmallSpellIcons();
	pSBkBtnCel = std::nullopt;
	pSpellBkCel = std::nullopt;
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
		SpellID sn = SpellPages[sbooktab][i - 1];

		if (IsValidSpell(sn) && (spl & GetSpellBitmask(sn)) != 0) {
			SpellType st = GetSBookTrans(sn, true);
			SetSpellTrans(st);
			const Point spellCellPosition = GetPanelPosition(UiPanels::Spell, { 11, yp + SpellBookDescription.height });
			DrawSmallSpellIcon(out, spellCellPosition, sn);

			int8_t baseLvl = player._pSplLvl[static_cast<size_t>(sn)];
			int8_t splLvlAdd = player._pISplLvlAdd;
			UiFlags splLvlColor = baseLvl == MaxSpellLevel ? UiFlags::ColorGold : UiFlags::ColorWhite;
			UiFlags splLvlAddColor = UiFlags::ColorWhite;

			if (splLvlAdd > 0)
				splLvlAddColor = UiFlags::ColorBlue;
			else if (splLvlAdd < 0)
				splLvlAddColor = UiFlags::ColorRed;

			if (sn == player._pRSpell && st == player._pRSplType) {
				SetSpellTrans(SpellType::Skill);
				DrawSmallSpellIconBorder(out, spellCellPosition);
			}

			const Point line0 { 0, yp + textPaddingTop };
			const Point line1 { 0, yp + textPaddingTop + lineHeight };

			int charges = player.InvBody[INVLOC_HAND_LEFT]._iCharges;
			int mana = GetManaAmount(player, sn) >> 6;
			int8_t lvl = player.GetSpellLevel(sn);
			int min, max;
			GetDamageAmt(sn, &min, &max);

			PrintSBookStr(out, line0, pgettext("spell", GetSpellData(sn).sNameText), UiFlags::ColorWhite);

			switch (GetSBookTrans(sn, false)) {
			case SpellType::Skill:
				PrintSBookStr(out, line1, _("Skill"), UiFlags::ColorWhite);
				break;
			case SpellType::Charges:
				PrintSBookStr(out, line1, fmt::format(fmt::runtime(ngettext("{:d} charge", "{:d} charges", charges)), charges), UiFlags::ColorWhite);
				break;
			case SpellType::Spell:
				PrintSBookStr(out, line1, fmt::format(fmt::runtime(pgettext("spellbook", "Mana: {:d}")), mana), UiFlags::ColorWhite);
				if (lvl == 0) {
					PrintSBookStr(out, line1, _("Unusable"), UiFlags::AlignRight | UiFlags::ColorRed);
					break;
				}
				switch (sn) {
				case SpellID::BoneSpirit:
					PrintSBookStr(out, line1, _("Dmg: 1/3 target hp"), UiFlags::AlignRight | UiFlags::ColorWhite);
					break;
				case SpellID::ManaShield:
					PrintSBookStr(out, line1, fmt::format(fmt::runtime(_("Dmg Reduction: {:d}%")), lvl > 0 ? (100 / player.GetManaShieldDamageReduction()) : 0), UiFlags::AlignRight | UiFlags::ColorWhite);
					break;
				case SpellID::Healing:
				case SpellID::HealOther:

					PrintSBookStr(out, line1, fmt::format(fmt::runtime(_("Heals: {:d} - {:d}")), min, max), UiFlags::AlignRight | UiFlags::ColorWhite);
					break;
				default:
					GetDamageAmt(sn, &min, &max);
					if (min != -1) {
						PrintSBookStr(out, line1, fmt::format(fmt::runtime(_("Damage: {:d} - {:d}")), min, max), UiFlags::AlignRight | UiFlags::ColorWhite);
					}
					break;
				}
				break;
			}

			if (IsNoneOf(sn, SpellID::ItemRepair, SpellID::StaffRecharge, SpellID::TrapDisarm, SpellID::Identify, SpellID::Rage)) {
				if (splLvlAdd != 0) {
					DrawStringWithColors(out, _("Level: {} {}"), std::vector<DrawStringFormatArg> { { baseLvl, splLvlColor }, { StrCat("(", lvl, ")"), splLvlAddColor } }, Rectangle(GetPanelPosition(UiPanels::Spell, line0 + Displacement { SPLICONLENGTH, 0 }), SpellBookDescription).inset({ SpellBookDescriptionPaddingHorizontal, 0 }), UiFlags::AlignRight | UiFlags::ColorWhite, 1, 0);
				} else {
					DrawStringWithColors(out, _("Level: {}"), std::vector<DrawStringFormatArg> { { baseLvl, splLvlColor } }, Rectangle(GetPanelPosition(UiPanels::Spell, line0 + Displacement { SPLICONLENGTH, 0 }), SpellBookDescription).inset({ SpellBookDescriptionPaddingHorizontal, 0 }), UiFlags::AlignRight | UiFlags::ColorWhite, 1, 0);
				}
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
		SpellID sn = SpellPages[sbooktab][(MousePosition.y - iconArea.position.y) / SpellBookDescription.height];
		Player &player = *MyPlayer;
		uint64_t spl = player._pMemSpells | player._pISpells | player._pAblSpells;
		if (IsValidSpell(sn) && (spl & GetSpellBitmask(sn)) != 0) {
			SpellType st = SpellType::Spell;
			if ((player._pISpells & GetSpellBitmask(sn)) != 0) {
				st = SpellType::Charges;
			}
			if ((player._pAblSpells & GetSpellBitmask(sn)) != 0) {
				st = SpellType::Skill;
			}
			player._pRSpell = sn;
			player._pRSplType = st;
			RedrawEverything();
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
