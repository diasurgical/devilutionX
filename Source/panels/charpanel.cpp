#include "panels/charpanel.hpp"

#include <fmt/format.h>
#include <string>

#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"

#include "control.h"
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
	Displacement labelOffset; // label's offset (end of the label vs the beginning of the stat box)
	int labelLength;          // max label's length - used for line wrapping
	int labelSpacing;
	int statSpacing;
	bool centered;
	/**
	 * Toggles whether the box should be using the 27px version or 26px.
	 * Must be set to true for stat boxes or they don't line up with the "spend stat" button
	 */
	bool high;
	std::function<StyledText()> statDisplayFunc; // function responsible for displaying stat
};

Player *MyPlayer = &Players[MyPlayerId];

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
	if (MyPlayer->InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW && MyPlayer->_pClass != HeroClass::Rogue) {
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
	{ "", { 13, 14 }, 134, { 0, 0 }, 0, 0, 1, false, false,
	    []() { return StyledText { UiFlags::ColorSilver, MyPlayer->_pName }; } },
	{ N_("Level"), { 57, 52 }, 45, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return StyledText { UiFlags::ColorSilver, fmt::format("{:d}", MyPlayer->_pLevel) }; } },

	{ N_("Base"), { 88, 118 }, 33, { 39, 0 }, 0, 0, 0, false, false,
	    nullptr },
	{ N_("Now"), { 135, 118 }, 33, { 39, 0 }, 0, 0, 0, false, false,
	    nullptr },

	{ N_("Strength"), { 88, 137 }, 33, { -3, 0 }, 0, 0, 1, false, true,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Strength), fmt::format("{:d}", MyPlayer->_pBaseStr) }; } },
	{ N_("Magic"), { 88, 165 }, 33, { -3, 0 }, 0, 0, 1, false, true,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Magic), fmt::format("{:d}", MyPlayer->_pBaseMag) }; } },
	{ N_("Dexterity"), { 88, 193 }, 33, { -3, 0 }, 0, 0, 1, false, true,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", MyPlayer->_pBaseDex) }; } },
	{ N_("Vitality"), { 88, 221 }, 33, { -3, 0 }, 0, 0, 1, false, true,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", MyPlayer->_pBaseVit) }; } },

	{ "", { 135, 137 }, 33, { 0, 0 }, 0, 0, 1, false, true,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Strength), fmt::format("{:d}", MyPlayer->_pStrength) }; } },
	{ "", { 135, 165 }, 33, { 0, 0 }, 0, 0, 1, false, true,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Magic), fmt::format("{:d}", MyPlayer->_pMagic) }; } },
	{ "", { 135, 193 }, 33, { 0, 0 }, 0, 0, 1, false, true,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", MyPlayer->_pDexterity) }; } },
	{ "", { 135, 221 }, 33, { 0, 0 }, 0, 0, 1, false, true,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", MyPlayer->_pVitality) }; } },

	{ N_("Points to distribute"), { 88, 250 }, 33, { -3, -5 }, 120, 0, 1, false, false,
	    []() {
	        MyPlayer->_pStatPts = std::min(CalcStatDiff(*MyPlayer), MyPlayer->_pStatPts);
	        return StyledText { UiFlags::ColorRed, (MyPlayer->_pStatPts > 0 ? fmt::format("{:d}", MyPlayer->_pStatPts) : "") };
	    } },

	{ N_("Life"), { 88, 287 }, 33, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return StyledText { GetMaxHealthColor(), fmt::format("{:d}", MyPlayer->_pMaxHP >> 6) }; } },

	{ "", { 135, 287 }, 33, { 0, 0 }, 0, 0, 1, false, false,
	    []() { return StyledText { (MyPlayer->_pHitPoints != MyPlayer->_pMaxHP ? UiFlags::ColorRed : GetMaxHealthColor()), fmt::format("{:d}", MyPlayer->_pHitPoints >> 6) }; } },

	{ N_("Mana"), { 88, 315 }, 33, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return StyledText { GetMaxManaColor(), fmt::format("{:d}", MyPlayer->_pMaxMana >> 6) }; } },

	{ "", { 135, 315 }, 33, { 0, 0 }, 0, 0, 1, false, false,
	    []() { return StyledText { (MyPlayer->_pMana != MyPlayer->_pMaxMana ? UiFlags::ColorRed : GetMaxManaColor()), fmt::format("{:d}", MyPlayer->_pMana >> 6) }; } },

	{ "", { 161, 14 }, 134, { 0, 0 }, 0, 0, 1, false, false,
	    []() { return StyledText { UiFlags::ColorSilver, _(ClassStrTbl[static_cast<std::size_t>(MyPlayer->_pClass)]) }; } },

	{ N_("Experience"), { 208, 52 }, 87, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return StyledText { UiFlags::ColorSilver, fmt::format("{:d}", MyPlayer->_pExperience) }; } },

	{ N_("Next level"), { 208, 80 }, 87, { -3, 0 }, 0, 0, 1, false, false,
	    []() {
	        if (MyPlayer->_pLevel == MAXCHARLEVEL - 1) {
		        return StyledText { UiFlags::ColorGold, _("None") };
	        } else {
		        return StyledText { UiFlags::ColorSilver, fmt::format("{:d}", MyPlayer->_pNextExper) };
	        }
	    } },

	{ N_("Gold"), { 208, 129 }, 87, { 0, -20 }, 0, 0, 1, true, false,
	    []() { return StyledText { UiFlags::ColorSilver, fmt::format("{:d}", MyPlayer->_pGold) }; } },

	{ N_("Armor class"), { 250, 166 }, 45, { -3, -5 }, 55, 0, 1, false, false,
	    []() { return StyledText { GetValueColor(MyPlayer->_pIBonusAC), fmt::format("{:d}", MyPlayer->GetArmor()) }; } },

	{ N_("To hit"), { 250, 194 }, 45, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return StyledText { GetValueColor(MyPlayer->_pIBonusToHit), fmt::format("{:d}%", (MyPlayer->InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW ? MyPlayer->GetRangedToHit() : MyPlayer->GetMeleeToHit())) }; } },

	{ N_("Damage"), { 250, 222 }, 45, { -3, 0 }, 0, 0, 0, false, false,
	    []() {
	        std::pair<int, int> dmg = GetDamage();
	        return StyledText { GetValueColor(MyPlayer->_pIBonusDam), fmt::format("{:d}-{:d}", dmg.first, dmg.second) };
	    } },

	{ N_("Resist magic"), { 250, 259 }, 45, { -3, -5 }, 46, 1, 1, false, false,
	    []() { return GetResistInfo(MyPlayer->_pMagResist); } },

	{ N_("Resist fire"), { 250, 287 }, 45, { -3, -5 }, 46, 1, 1, false, false,
	    []() { return GetResistInfo(MyPlayer->_pFireResist); } },

	{ N_("Resist lightning"), { 250, 315 }, 45, { -3, -5 }, 76, 0, 1, false, false,
	    []() { return GetResistInfo(MyPlayer->_pLghtResist); } },

};

Art PanelParts[6];
Art PanelFull;

void DrawPanelFieldLow(const Surface &out, Point pos, int len)
{
	DrawArt(out, pos, &PanelParts[0]);
	pos.x += PanelParts[0].w();
	DrawArt(out, pos, &PanelParts[1], 0, len);
	pos.x += len;
	DrawArt(out, pos, &PanelParts[2]);
}

void DrawPanelFieldHigh(const Surface &out, Point pos, int len)
{
	DrawArt(out, pos, &PanelParts[3]);
	pos.x += PanelParts[3].w();
	DrawArt(out, pos, &PanelParts[4], 0, len);
	pos.x += len;
	DrawArt(out, pos, &PanelParts[5]);
}

void DrawShadowString(const Surface &out, const PanelEntry &entry)
{
	if (entry.label == "")
		return;

	std::string text_tmp = _(entry.label.c_str());
	char buffer[32];
	int spacing = entry.labelSpacing;
	strcpy(buffer, text_tmp.c_str());
	if (entry.labelLength > 0)
		WordWrapGameString(buffer, entry.labelLength, GameFontSmall, spacing);
	std::string text(buffer);
	int width = GetLineWidth(text, GameFontSmall, spacing);
	Point finalPos = { entry.position + Displacement { 0, 17 } + entry.labelOffset };
	if (entry.centered)
		width = entry.length;
	else
		finalPos.x -= width;

	UiFlags style = UiFlags::AlignRight;
	if (entry.centered) {
		style = UiFlags::AlignCenter;
		finalPos += Displacement { 7, 0 }; // left border
	}
	DrawString(out, text, { finalPos + Displacement { -2, 2 }, { width, 0 } }, style | UiFlags::ColorBlack, spacing, 10);
	DrawString(out, text, { finalPos, { width, 0 } }, style | UiFlags::ColorSilver, spacing, 10);
}

void LoadCharPanel()
{
	LoadArt("data\\charbg.pcx", &PanelFull);
	LoadArt("data\\boxleftend26.pcx", &PanelParts[0]);
	LoadArt("data\\boxmiddle26.pcx", &PanelParts[1]);
	LoadArt("data\\boxrightend26.pcx", &PanelParts[2]);
	LoadArt("data\\boxleftend27.pcx", &PanelParts[3]);
	LoadArt("data\\boxmiddle27.pcx", &PanelParts[4]);
	LoadArt("data\\boxrightend27.pcx", &PanelParts[5]);

	const Surface out(PanelFull.surface.get());

	for (auto &entry : panelEntries) {
		if (entry.statDisplayFunc != nullptr) {
			if (entry.high)
				DrawPanelFieldHigh(out, entry.position, entry.length);
			else
				DrawPanelFieldLow(out, entry.position, entry.length);
		}
		DrawShadowString(out, entry);
	}

	for (auto &gfx : PanelParts) {
		gfx.Unload();
	}
}

bool CharPanelLoaded = false;

void DrawStatButtons(const Surface &out)
{
	if (MyPlayer->_pStatPts > 0) {
		if (MyPlayer->_pBaseStr < MyPlayer->GetMaximumAttributeValue(CharacterAttribute::Strength))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 159 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Strength)] ? 3 : 2);
		if (MyPlayer->_pBaseMag < MyPlayer->GetMaximumAttributeValue(CharacterAttribute::Magic))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 187 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Magic)] ? 5 : 4);
		if (MyPlayer->_pBaseDex < MyPlayer->GetMaximumAttributeValue(CharacterAttribute::Dexterity))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 216 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Dexterity)] ? 7 : 6);
		if (MyPlayer->_pBaseVit < MyPlayer->GetMaximumAttributeValue(CharacterAttribute::Vitality))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 244 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Vitality)] ? 9 : 8);
	}
}

} // namespace

void DrawChr(const Surface &out)
{
	if (!CharPanelLoaded) {
		LoadCharPanel();
		CharPanelLoaded = true;
	}
	Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });
	DrawArt(out, pos, &PanelFull);
	for (auto &entry : panelEntries) {
		if (entry.statDisplayFunc != nullptr) {
			StyledText tmp = entry.statDisplayFunc();
			Displacement displacement = Displacement { pos.x + 7, pos.y + 17 };
			if (entry.high)
				displacement += { 0, 1 };
			DrawString(out, tmp.text.c_str(), { entry.position + displacement, { entry.length, 0 } }, UiFlags::AlignCenter | tmp.style, entry.statSpacing);
		}
	}
	DrawStatButtons(out);
}

} // namespace devilution
