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

struct colorAndText {
	UiFlags color;
	std::string text;
};

struct panelEntry {
	std::string label;
	Displacement position;
	int length;
	Displacement labelOffset; // label's offset (end of the label vs the beginning of the stat box)
	int labelLength;          // max label's length - used for line wrapping
	int labelSpacing;
	int statSpacing;
	bool centered;
	// must be set to true for stat boxes or they don't line up with the "spend stat" button
	bool high;                                     // if the box should be using the 27px version or 26px
	std::function<colorAndText()> statDisplayFunc; // function responsible for displaying stat
};

auto &myPlayer = Players[MyPlayerId];

UiFlags GetBaseStatColor(CharacterAttribute attr)
{
	UiFlags style = UiFlags::ColorSilver;
	if (myPlayer.GetBaseAttributeValue(attr) == myPlayer.GetMaximumAttributeValue(attr))
		style = UiFlags::ColorGold;
	return style;
}

UiFlags GetCurrentStatColor(CharacterAttribute attr)
{
	UiFlags style = UiFlags::ColorSilver;
	int current = myPlayer.GetCurrentAttributeValue(attr);
	int base = myPlayer.GetBaseAttributeValue(attr);
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
	return myPlayer._pMaxMana > myPlayer._pMaxManaBase ? UiFlags::ColorBlue : UiFlags::ColorSilver;
}

std::pair<int, int> GetDamage()
{
	int mindam = myPlayer._pIMinDam;
	mindam += myPlayer._pIBonusDam * mindam / 100;
	mindam += myPlayer._pIBonusDamMod;
	if (myPlayer.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW) {
		if (myPlayer._pClass == HeroClass::Rogue)
			mindam += myPlayer._pDamageMod;
		else
			mindam += myPlayer._pDamageMod / 2;
	} else {
		mindam += myPlayer._pDamageMod;
	}
	int maxdam = myPlayer._pIMaxDam;
	maxdam += myPlayer._pIBonusDam * maxdam / 100;
	maxdam += myPlayer._pIBonusDamMod;
	if (myPlayer.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW) {
		if (myPlayer._pClass == HeroClass::Rogue)
			maxdam += myPlayer._pDamageMod;
		else
			maxdam += myPlayer._pDamageMod / 2;
	} else {
		maxdam += myPlayer._pDamageMod;
	}
	return { mindam, maxdam };
}

colorAndText GetResistInfo(int8_t resist)
{
	UiFlags color = UiFlags::ColorBlue;
	if (resist == 0)
		color = UiFlags::ColorSilver;
	else if (resist < 0)
		color = UiFlags::ColorRed;
	else if (resist >= MAXRESIST)
		color = UiFlags::ColorGold;

	return {
		color, (resist >= MAXRESIST ? _("MAX") : fmt::format("{:d}%", resist))
	};
}

panelEntry panelEntries[] = {
	{ "", { 13, 14 }, 134, { 0, 0 }, 0, 0, 1, false, false,
	    []() { return colorAndText { UiFlags::ColorSilver, myPlayer._pName }; } },
	{ "level", { 57, 52 }, 45, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return colorAndText { UiFlags::ColorSilver, fmt::format("{:d}", myPlayer._pLevel) }; } },

	{ "base", { 88, 118 }, 33, { 39, 0 }, 0, 0, 0, false, false,
	    nullptr },
	{ "now", { 135, 118 }, 33, { 39, 0 }, 0, 0, 0, false, false,
	    nullptr },

	{ "strength", { 88, 137 }, 33, { -3, 0 }, 0, 0, 1, false, true,
	    []() { return colorAndText { GetBaseStatColor(CharacterAttribute::Strength), fmt::format("{:d}", myPlayer._pBaseStr) }; } },
	{ "magic", { 88, 165 }, 33, { -3, 0 }, 0, 0, 1, false, true,
	    []() { return colorAndText { GetBaseStatColor(CharacterAttribute::Magic), fmt::format("{:d}", myPlayer._pBaseMag) }; } },
	{ "dexterity", { 88, 193 }, 33, { -3, 0 }, 0, 0, 1, false, true,
	    []() { return colorAndText { GetBaseStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", myPlayer._pBaseDex) }; } },
	{ "vitality", { 88, 221 }, 33, { -3, 0 }, 0, 0, 1, false, true,
	    []() { return colorAndText { GetBaseStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", myPlayer._pBaseVit) }; } },

	{ "", { 135, 137 }, 33, { 0, 0 }, 0, 0, 1, false, true,
	    []() { return colorAndText { GetCurrentStatColor(CharacterAttribute::Strength), fmt::format("{:d}", myPlayer._pStrength) }; } },
	{ "", { 135, 165 }, 33, { 0, 0 }, 0, 0, 1, false, true,
	    []() { return colorAndText { GetCurrentStatColor(CharacterAttribute::Magic), fmt::format("{:d}", myPlayer._pMagic) }; } },
	{ "", { 135, 193 }, 33, { 0, 0 }, 0, 0, 1, false, true,
	    []() { return colorAndText { GetCurrentStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", myPlayer._pDexterity) }; } },
	{ "", { 135, 221 }, 33, { 0, 0 }, 0, 0, 1, false, true,
	    []() { return colorAndText { GetCurrentStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", myPlayer._pVitality) }; } },

	{ "points to distribute", { 88, 250 }, 33, { -3, -5 }, 120, 0, 1, false, false,
	    []() {
	        myPlayer._pStatPts = std::min(CalcStatDiff(myPlayer), myPlayer._pStatPts);
	        return colorAndText { UiFlags::ColorRed, (myPlayer._pStatPts > 0 ? fmt::format("{:d}", myPlayer._pStatPts) : "") };
	    } },

	{ "life", { 88, 287 }, 33, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return colorAndText { (myPlayer._pMaxHP > myPlayer._pMaxHPBase ? UiFlags::ColorBlue : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pMaxHP >> 6) }; } },

	{ "", { 135, 287 }, 33, { 0, 0 }, 0, 0, 1, false, false,
	    []() { return colorAndText { (myPlayer._pHitPoints != myPlayer._pMaxHP ? UiFlags::ColorRed : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pHitPoints >> 6) }; } },

	{ "mana", { 88, 315 }, 33, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return colorAndText { GetMaxManaColor(), fmt::format("{:d}", myPlayer._pMaxMana >> 6) }; } },

	{ "", { 135, 315 }, 33, { 0, 0 }, 0, 0, 1, false, false,
	    []() { return colorAndText { (myPlayer._pMana != myPlayer._pMaxMana ? UiFlags::ColorRed : GetMaxManaColor()), fmt::format("{:d}", myPlayer._pMana >> 6) }; } },

	{ "", { 161, 14 }, 134, { 0, 0 }, 0, 0, 1, false, false,
	    []() { return colorAndText { UiFlags::ColorSilver, _(ClassStrTbl[static_cast<std::size_t>(myPlayer._pClass)]) }; } },

	{ "experience", { 208, 52 }, 87, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return colorAndText { UiFlags::ColorSilver, fmt::format("{:d}", myPlayer._pExperience) }; } },

	{ "next level", { 208, 80 }, 87, { -3, 0 }, 0, 0, 1, false, false,
	    []() {
	        if (myPlayer._pLevel == MAXCHARLEVEL - 1) {
		        return colorAndText { UiFlags::ColorGold, _("None") };
	        } else {
		        return colorAndText { UiFlags::ColorSilver, fmt::format("{:d}", myPlayer._pNextExper) };
	        }
	    } },

	{ "gold", { 208, 129 }, 87, { 0, -20 }, 0, 0, 1, true, false,
	    []() { return colorAndText { UiFlags::ColorSilver, fmt::format("{:d}", myPlayer._pGold) }; } },

	{ "armor class", { 250, 166 }, 45, { -3, -5 }, 55, 0, 1, false, false,
	    []() { return colorAndText { GetValueColor(myPlayer._pIBonusAC), fmt::format("{:d}", myPlayer.GetArmor()) }; } },

	{ "to hit", { 250, 194 }, 45, { -3, 0 }, 0, 0, 1, false, false,
	    []() { return colorAndText { GetValueColor(myPlayer._pIBonusToHit), fmt::format("{:d}%", (myPlayer.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW ? myPlayer.GetRangedToHit() : myPlayer.GetMeleeToHit())) }; } },

	{ "damage", { 250, 222 }, 45, { -3, 0 }, 0, 0, 0, false, false,
	    []() {
	        std::pair<int, int> dmg = GetDamage();
	        return colorAndText { GetValueColor(myPlayer._pIBonusDam), fmt::format("{:d}-{:d}", dmg.first, dmg.second) };
	    } },

	{ "resist magic", { 250, 259 }, 45, { -3, -5 }, 46, 1, 1, false, false,
	    []() { return GetResistInfo(myPlayer._pMagResist); } },

	{ "resist fire", { 250, 287 }, 45, { -3, -5 }, 46, 1, 1, false, false,
	    []() { return GetResistInfo(myPlayer._pFireResist); } },

	{ "resist lightning", { 250, 315 }, 45, { -3, -5 }, 76, 0, 1, false, false,
	    []() { return GetResistInfo(myPlayer._pLghtResist); } },

};

Art PanelParts[6];
Art PanelFull;

void DrawPanelFieldLow(const Surface &out, Point pos, int len)
{
	DrawArt(out, pos.x, pos.y, &PanelParts[0]);
	pos.x += PanelParts[0].w();
	DrawArt(out, pos.x, pos.y, &PanelParts[1], 0, len);
	pos.x += len;
	DrawArt(out, pos.x, pos.y, &PanelParts[2]);
}

void DrawPanelFieldHigh(const Surface &out, Point pos, int len)
{
	DrawArt(out, pos.x, pos.y, &PanelParts[3]);
	pos.x += PanelParts[3].w();
	DrawArt(out, pos.x, pos.y, &PanelParts[4], 0, len);
	pos.x += len;
	DrawArt(out, pos.x, pos.y, &PanelParts[5]);
}

void DrawShadowString(const Surface &out, Point pos, panelEntry &entry)
{
	if (entry.label == "")
		return;

	std::string text_tmp = entry.label;
	char buffer[32];
	int spacing = entry.labelSpacing;
	strcpy(buffer, text_tmp.c_str());
	if (entry.labelLength > 0)
		WordWrapGameString(buffer, entry.labelLength, GameFontSmall, spacing);
	std::string text(buffer);
	int width = GetLineWidth(text, GameFontSmall, spacing);
	if (entry.centered)
		width = entry.length;
	else
		pos.x -= width;

	Point finalPos = { pos + entry.position + Displacement { 0, 17 } + entry.labelOffset };
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

	Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });
	const Surface out(PanelFull.surface.get());

	for (auto &entry : panelEntries) {
		if (entry.statDisplayFunc != nullptr) {
			if (entry.high)
				DrawPanelFieldHigh(out, pos + entry.position, entry.length);
			else
				DrawPanelFieldLow(out, pos + entry.position, entry.length);
		}
		DrawShadowString(out, pos, entry);
	}

	for (auto &gfx : PanelParts) {
		gfx.Unload();
	}
}

bool CharPanelLoaded = false;

void DrawStatButtons(const Surface &out)
{
	if (myPlayer._pStatPts > 0) {
		if (myPlayer._pBaseStr < myPlayer.GetMaximumAttributeValue(CharacterAttribute::Strength))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 159 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Strength)] ? 3 : 2);
		if (myPlayer._pBaseMag < myPlayer.GetMaximumAttributeValue(CharacterAttribute::Magic))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 187 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Magic)] ? 5 : 4);
		if (myPlayer._pBaseDex < myPlayer.GetMaximumAttributeValue(CharacterAttribute::Dexterity))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 216 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Dexterity)] ? 7 : 6);
		if (myPlayer._pBaseVit < myPlayer.GetMaximumAttributeValue(CharacterAttribute::Vitality))
			CelDrawTo(out, GetPanelPosition(UiPanels::Character, { 137, 244 }), *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Vitality)] ? 9 : 8);
	}
}

void DrawPcxChr(const Surface &out)
{
	if (!CharPanelLoaded) {
		LoadCharPanel();
		CharPanelLoaded = true;
	}
	Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });
	DrawArt(out, pos.x, pos.y, &PanelFull);
	for (auto &entry : panelEntries) {
		if (entry.statDisplayFunc != nullptr) {
			colorAndText tmp = entry.statDisplayFunc();
			Displacement displacement = Displacement {7, 17};
			if (entry.high)
				displacement += { 0, 1 };
			DrawString(out, tmp.text.c_str(), { pos + entry.position + displacement, { entry.length, 12 } }, UiFlags::AlignCenter | tmp.color, entry.statSpacing);
		}
	}
	DrawStatButtons(out);
}

} // namespace devilution
