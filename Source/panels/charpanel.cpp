#include "panels/charpanel.hpp"

#include <fmt/format.h>
#include <string>

#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"

#include "control.h"
#include "player.h"
#include "utils/display.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "utils/language.h"

namespace devilution {

std::optional<CelSprite> pChrButtons;

/** Map of hero class names */
const char *const ClassStrTbl[] = {
	N_("Warrior"),
	N_("Rogue"),
	N_("Sorcerer"),
	N_("Monk"),
	N_("Bard"),
	N_("Barbarian"),
};

namespace {

struct StyledText {
	UiFlags style;
	std::string text;
};

struct PanelEntry {
	std::string label;
	Point position;
	int length;
	int labelLength;                                       // max label's length - used for line wrapping
	std::function<StyledText()> statDisplayFunc = nullptr; // function responsible for displaying stat
};

UiFlags GetBaseStatColor(CharacterAttribute attr)
{
	UiFlags style = UiFlags::ColorSilver;
	if (MyPlayer->GetBaseAttributeValue(attr) == MyPlayer->GetMaximumAttributeValue(attr))
		style = UiFlags::ColorGold;
	return style;
}

UiFlags GetCurrentStatColor(CharacterAttribute attr)
{
	UiFlags style = UiFlags::ColorSilver;
	int current = MyPlayer->GetCurrentAttributeValue(attr);
	int base = MyPlayer->GetBaseAttributeValue(attr);
	if (current > base)
		style = UiFlags::ColorBlue;
	if (current < base)
		style = UiFlags::ColorRed;
	return style;
}

UiFlags GetValueColor(int value, bool flip = false)
{
	UiFlags style = UiFlags::ColorSilver;
	if (value > 0)
		style = (flip ? UiFlags::ColorRed : UiFlags::ColorBlue);
	if (value < 0)
		style = (flip ? UiFlags::ColorBlue : UiFlags::ColorRed);
	return style;
}

UiFlags GetMaxManaColor()
{
	return MyPlayer->_pMaxMana > MyPlayer->_pMaxManaBase ? UiFlags::ColorBlue : UiFlags::ColorSilver;
}

UiFlags GetMaxHealthColor()
{
	return MyPlayer->_pMaxHP > MyPlayer->_pMaxHPBase ? UiFlags::ColorBlue : UiFlags::ColorSilver;
}

std::pair<int, int> GetDamage()
{
	int damageMod = MyPlayer->_pIBonusDamMod;
	if (MyPlayer->InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Bow && MyPlayer->_pClass != HeroClass::Rogue) {
		damageMod += MyPlayer->_pDamageMod / 2;
	} else {
		damageMod += MyPlayer->_pDamageMod;
	}
	int mindam = MyPlayer->_pIMinDam + MyPlayer->_pIBonusDam * MyPlayer->_pIMinDam / 100 + damageMod;
	int maxdam = MyPlayer->_pIMaxDam + MyPlayer->_pIBonusDam * MyPlayer->_pIMaxDam / 100 + damageMod;
	return { mindam, maxdam };
}

StyledText GetResistInfo(int8_t resist)
{
	UiFlags style = UiFlags::ColorBlue;
	if (resist == 0)
		style = UiFlags::ColorSilver;
	else if (resist < 0)
		style = UiFlags::ColorRed;
	else if (resist >= MAXRESIST)
		style = UiFlags::ColorGold;

	return {
		style, (resist >= MAXRESIST ? _("MAX") : fmt::format("{:d}%", resist))
	};
}

PanelEntry panelEntries[] = {
	{ "", { 9, 14 }, 150, 0,
	    []() { return StyledText { UiFlags::ColorSilver, MyPlayer->_pName }; } },
	{ "", { 161, 14 }, 149, 0,
	    []() { return StyledText { UiFlags::ColorSilver, _(ClassStrTbl[static_cast<std::size_t>(MyPlayer->_pClass)]) }; } },

	{ N_("Level"), { 57, 52 }, 57, 45,
	    []() { return StyledText { UiFlags::ColorSilver, fmt::format("{:d}", MyPlayer->_pLevel) }; } },
	{ N_("Experience"), { 211, 52 }, 99, 91,
	    []() { return StyledText { UiFlags::ColorSilver, fmt::format("{:d}", MyPlayer->_pExperience) }; } },
	{ N_("Next level"), { 211, 80 }, 99, 198,
	    []() {
	        if (MyPlayer->_pLevel == MAXCHARLEVEL - 1) {
		        return StyledText { UiFlags::ColorGold, _("None") };
	        } else {
		        return StyledText { UiFlags::ColorSilver, fmt::format("{:d}", MyPlayer->_pNextExper) };
	        }
	    } },

	{ N_("Base"), { 88, 115 }, 0, 44 },
	{ N_("Now"), { 135, 115 }, 0, 44 },
	{ N_("Strength"), { 88, 135 }, 45, 76,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Strength), fmt::format("{:d}", MyPlayer->_pBaseStr) }; } },
	{ "", { 135, 135 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Strength), fmt::format("{:d}", MyPlayer->_pStrength) }; } },
	{ N_("Magic"), { 88, 163 }, 45, 76,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Magic), fmt::format("{:d}", MyPlayer->_pBaseMag) }; } },
	{ "", { 135, 163 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Magic), fmt::format("{:d}", MyPlayer->_pMagic) }; } },
	{ N_("Dexterity"), { 88, 191 }, 45, 76, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", MyPlayer->_pBaseDex) }; } },
	{ "", { 135, 191 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", MyPlayer->_pDexterity) }; } },
	{ N_("Vitality"), { 88, 219 }, 45, 76, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", MyPlayer->_pBaseVit) }; } },
	{ "", { 135, 219 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", MyPlayer->_pVitality) }; } },
	{ N_("Points to distribute"), { 88, 248 }, 45, 76,
	    []() {
	        MyPlayer->_pStatPts = std::min(CalcStatDiff(*MyPlayer), MyPlayer->_pStatPts);
	        return StyledText { UiFlags::ColorRed, (MyPlayer->_pStatPts > 0 ? fmt::format("{:d}", MyPlayer->_pStatPts) : "") };
	    } },

	{ N_("Gold"), { 211, 107 }, 0, 98 },
	{ "", { 211, 127 }, 99, 0,
	    []() { return StyledText { UiFlags::ColorSilver, fmt::format("{:d}", MyPlayer->_pGold) }; } },

	{ N_("Armor class"), { 253, 163 }, 57, 67,
	    []() { return StyledText { GetValueColor(MyPlayer->_pIBonusAC), fmt::format("{:d}", MyPlayer->GetArmor()) }; } },
	{ N_("To hit"), { 253, 191 }, 57, 67,
	    []() { return StyledText { GetValueColor(MyPlayer->_pIBonusToHit), fmt::format("{:d}%", (MyPlayer->InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Bow ? MyPlayer->GetRangedToHit() : MyPlayer->GetMeleeToHit())) }; } },
	{ N_("Damage"), { 253, 219 }, 57, 67,
	    []() {
	        std::pair<int, int> dmg = GetDamage();
	        return StyledText { GetValueColor(MyPlayer->_pIBonusDam), fmt::format("{:d}-{:d}", dmg.first, dmg.second) };
	    } },

	{ N_("Life"), { 88, 284 }, 45, 76,
	    []() { return StyledText { GetMaxHealthColor(), fmt::format("{:d}", MyPlayer->_pMaxHP >> 6) }; } },
	{ "", { 135, 284 }, 45, 0,
	    []() { return StyledText { (MyPlayer->_pHitPoints != MyPlayer->_pMaxHP ? UiFlags::ColorRed : GetMaxHealthColor()), fmt::format("{:d}", MyPlayer->_pHitPoints >> 6) }; } },
	{ N_("Mana"), { 88, 312 }, 45, 76,
	    []() { return StyledText { GetMaxManaColor(), fmt::format("{:d}", MyPlayer->_pMaxMana >> 6) }; } },
	{ "", { 135, 312 }, 45, 0,
	    []() { return StyledText { (MyPlayer->_pMana != MyPlayer->_pMaxMana ? UiFlags::ColorRed : GetMaxManaColor()), fmt::format("{:d}", MyPlayer->_pMana >> 6) }; } },

	{ N_("Resist magic"), { 253, 256 }, 57, 67,
	    []() { return GetResistInfo(MyPlayer->_pMagResist); } },
	{ N_("Resist fire"), { 253, 284 }, 57, 67,
	    []() { return GetResistInfo(MyPlayer->_pFireResist); } },
	{ N_("Resist lightning"), { 253, 313 }, 57, 67,
	    []() { return GetResistInfo(MyPlayer->_pLghtResist); } },
};

Art PanelBoxLeft;
Art PanelBoxMiddle;
Art PanelBoxRight;
Art PanelFull;

void DrawPanelField(const Surface &out, Point pos, int len)
{
	DrawArt(out, pos, &PanelBoxLeft);
	pos.x += PanelBoxLeft.w();
	len -= PanelBoxLeft.w() + PanelBoxRight.w();
	DrawArt(out, pos, &PanelBoxMiddle, 0, len);
	pos.x += len;
	DrawArt(out, pos, &PanelBoxRight);
}

void DrawShadowString(const Surface &out, const PanelEntry &entry)
{
	if (entry.label == "")
		return;

	std::string text_tmp = _(entry.label.c_str());
	char buffer[32];
	int spacing = 0;
	strcpy(buffer, text_tmp.c_str());
	if (entry.labelLength > 0)
		WordWrapString(buffer, entry.labelLength, GameFont12, spacing);
	std::string text(buffer);

	UiFlags style = UiFlags::VerticalCenter;

	Point labelPosition = entry.position;

	if (entry.length == 0) {
		style |= UiFlags::AlignCenter;
	} else {
		style |= UiFlags::AlignRight;
		labelPosition += Displacement { -entry.labelLength - 3, 0 };
	}

	DrawString(out, text, { labelPosition + Displacement { -2, 2 }, { entry.labelLength, 20 } }, style | UiFlags::ColorBlack, spacing, 10);
	DrawString(out, text, { labelPosition, { entry.labelLength, 20 } }, style | UiFlags::ColorSilver, spacing, 10);
}

void DrawStatButtons(const Surface &out)
{
	if (MyPlayer->_pStatPts > 0) {
		if (MyPlayer->_pBaseStr < MyPlayer->GetMaximumAttributeValue(CharacterAttribute::Strength))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 157 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Strength)] ? 3 : 2);
		if (MyPlayer->_pBaseMag < MyPlayer->GetMaximumAttributeValue(CharacterAttribute::Magic))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 185 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Magic)] ? 5 : 4);
		if (MyPlayer->_pBaseDex < MyPlayer->GetMaximumAttributeValue(CharacterAttribute::Dexterity))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 214 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Dexterity)] ? 7 : 6);
		if (MyPlayer->_pBaseVit < MyPlayer->GetMaximumAttributeValue(CharacterAttribute::Vitality))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 242 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Vitality)] ? 9 : 8);
	}
}

} // namespace

void LoadCharPanel()
{
	LoadArt("data\\charbg.pcx", &PanelFull);
	UpdatePalette(&PanelFull); // PanelFull is being used as a render target
	LoadArt("data\\boxleftend.pcx", &PanelBoxLeft);
	LoadArt("data\\boxmiddle.pcx", &PanelBoxMiddle);
	LoadArt("data\\boxrightend.pcx", &PanelBoxRight);

	const Surface out(PanelFull.surface.get());

	for (auto &entry : panelEntries) {
		if (entry.statDisplayFunc != nullptr) {
			DrawPanelField(out, entry.position, entry.length);
		}
		DrawShadowString(out, entry);
	}

	PanelBoxLeft.Unload();
	PanelBoxMiddle.Unload();
	PanelBoxRight.Unload();
}

void FreeCharPanel()
{
	PanelFull.Unload();
}

void DrawChr(const Surface &out)
{
	Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });
	DrawArt(out, pos, &PanelFull);
	for (auto &entry : panelEntries) {
		if (entry.statDisplayFunc != nullptr) {
			StyledText tmp = entry.statDisplayFunc();
			DrawString(
			    out,
			    tmp.text.c_str(),
			    { entry.position + Displacement { pos.x, pos.y }, { entry.length, 27 } },
			    UiFlags::AlignCenter | UiFlags::VerticalCenter | tmp.style);
		}
	}
	DrawStatButtons(out);
}

} // namespace devilution
