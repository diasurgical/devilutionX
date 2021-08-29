#include "panels/charpanel.hpp"

#include <fmt/format.h>
#include <string>

#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"

#include "control.h"
#include "engine/render/text_render.hpp"
#include "utils/language.h"

namespace devilution {

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
	Displacement labelOffset;                      // label's offset (end of the label vs the beginning of the stat box)
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

panelEntry panelEntries[] = {
	{ "", { 13, 14 }, 134, { 0, 0 },
	    []() { return colorAndText { UiFlags::ColorSilver, myPlayer._pName }; } },
	{ "level", { 57, 52 }, 45, { 0, 0 },
	    []() { return colorAndText { UiFlags::ColorSilver, fmt::format("{:d}", myPlayer._pLevel) }; } },

	{ "base", { 88, 118 }, 33, { 39, 0 },
	    nullptr },
	{ "now", { 135, 118 }, 33, { 39, 0 },
	    nullptr },

	{ "strength", { 88, 138 }, 33, { 0, 0 },
	    []() { return colorAndText { GetBaseStatColor(CharacterAttribute::Strength), fmt::format("{:d}", myPlayer._pBaseStr) }; } },
	{ "magic", { 88, 166 }, 33, { 0, 0 },
	    []() { return colorAndText { GetBaseStatColor(CharacterAttribute::Magic), fmt::format("{:d}", myPlayer._pBaseMag) }; } },
	{ "dexterity", { 88, 194 }, 33, { 0, 0 },
	    []() { return colorAndText { GetBaseStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", myPlayer._pBaseDex) }; } },
	{ "vitality", { 88, 222 }, 33, { 0, 0 },
	    []() { return colorAndText { GetBaseStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", myPlayer._pBaseVit) }; } },

	{ "", { 135, 138 }, 33, { 0, 0 },
	    []() { return colorAndText { GetCurrentStatColor(CharacterAttribute::Strength), fmt::format("{:d}", myPlayer._pStrength) }; } },
	{ "", { 135, 166 }, 33, { 0, 0 },
	    []() { return colorAndText { GetCurrentStatColor(CharacterAttribute::Magic), fmt::format("{:d}", myPlayer._pMagic) }; } },
	{ "", { 135, 194 }, 33, { 0, 0 },
	    []() { return colorAndText { GetCurrentStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", myPlayer._pDexterity) }; } },
	{ "", { 135, 222 }, 33, { 0, 0 },
	    []() { return colorAndText { GetCurrentStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", myPlayer._pVitality) }; } },

	//{ "points to distribute", { 88, 250 }, 33, { 0, 0 },
	//    []() { return colorAndText { GetCurrentStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", myPlayer._pVitality) }; } },

	{ "life", { 88, 287 }, 33, { -5, 0 },
	    []() { return colorAndText { (myPlayer._pMaxHP > myPlayer._pMaxHPBase ? UiFlags::ColorBlue : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pMaxHP >> 6) }; } },

	{ "", { 135, 287 }, 33, { 0, 0 },
	    []() { return colorAndText { (myPlayer._pHitPoints != myPlayer._pMaxHP ? UiFlags::ColorRed : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pHitPoints >> 6) }; } },

	{ "mana", { 88, 315 }, 33, { -5, 0 },
	    []() { return colorAndText { (myPlayer._pMaxMana > myPlayer._pMaxManaBase ? UiFlags::ColorBlue : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pMaxMana >> 6) }; } },

	{ "", { 135, 315 }, 33, { 0, 0 },
	    []() { return colorAndText { (myPlayer._pMana != myPlayer._pMaxMana ? UiFlags::ColorRed : UiFlags::ColorSilver), fmt::format("{:d}", myPlayer._pMana >> 6) }; } },

	{ "", { 161, 14 }, 134, { 0, 0 },
	    []() { return colorAndText { UiFlags::ColorSilver, _(ClassStrTbl[static_cast<std::size_t>(myPlayer._pClass)]) }; } },

	{ "experience", { 208, 52 }, 87, { 0, 0 },
	    []() { return colorAndText { UiFlags::ColorSilver, fmt::format("{:d}", myPlayer._pExperience) }; } },

	{ "next level", { 208, 80 }, 87, { 0, 0 },
	    []() {
	        if (myPlayer._pLevel == MAXCHARLEVEL - 1) {
		        return colorAndText { UiFlags::ColorGold, _("None") };
	        } else {
		        return colorAndText { UiFlags::ColorSilver, fmt::format("{:d}", myPlayer._pNextExper) };
	        }
	    } },

	{ "centered:gold", { 208, 129 }, 87, { 0, -20 },
	    []() { return colorAndText { UiFlags::ColorSilver, fmt::format("{:d}", myPlayer._pGold) }; } },

	{ "armor class", { 250, 166 }, 45, { 0, 0 },
	    []() { return colorAndText { GetValueColor(myPlayer._pIBonusAC), fmt::format("{:d}", myPlayer.GetArmor()) }; } },

	{ "to hit", { 250, 194 }, 45, { 0, 0 },
	    []() { return colorAndText { GetValueColor(myPlayer._pIBonusToHit), fmt::format("{:d}%", (myPlayer.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW ? myPlayer.GetRangedToHit() : myPlayer.GetMeleeToHit())) }; } },

	{ "damage", { 250, 222 }, 45, { 0, 0 },
	    []() {
	        std::pair<int, int> dmg = GetDamage();
	        return colorAndText { GetValueColor(myPlayer._pIBonusDam), fmt::format("{:d}-{:d}", dmg.first, dmg.second) };
	    } },
	/*
	DrawThingy(out, { pos.x + 250, pos.y + 166 }, 45); // armor
	DrawThingy(out, { pos.x + 250, pos.y + 194 }, 45); // to hit
	DrawThingy(out, { pos.x + 250, pos.y + 222 }, 45); // damage
	DrawThingy(out, { pos.x + 250, pos.y + 259 }, 45); // resist magic
	DrawThingy(out, { pos.x + 250, pos.y + 287 }, 45); // resist fire
	DrawThingy(out, { pos.x + 250, pos.y + 315 }, 45); // resist lightning
		*/
};

Art PanelParts[3];
Art PanelFull;

void DrawThingy(const Surface &out, Point pos, int len)
{
	DrawArt(out, pos.x, pos.y, &PanelParts[0]);
	pos.x += PanelParts[0].w();
	DrawArt(out, pos.x, pos.y, &PanelParts[1], 0, len);
	pos.x += len;
	DrawArt(out, pos.x, pos.y, &PanelParts[2]);
}

void DrawShadowString(const Surface &out, Point pos, panelEntry &entry)
{
	if (entry.label == "")
		return;

	bool centered = false;
	std::string text = entry.label;
	int width = GetLineWidth(text);
	if (text.find("centered:") != std::string::npos) {
		centered = true;
		text = text.substr(9);
		width = entry.length;
	}

	if (!centered)
		pos.x -= width;
	Point finalPos = { pos + entry.position + Displacement { 0, 17 } + entry.labelOffset };
	UiFlags style = UiFlags::AlignRight;
	if (centered) {
		style = UiFlags::AlignCenter;
		finalPos += Displacement { 7, 0 }; // left border
	}

	DrawString(out, text, { finalPos + Displacement { -2, 2 }, { width, 12 } }, style | UiFlags::ColorBlack);
	DrawString(out, text, { finalPos + Displacement { -1, 2 }, { width, 12 } }, style | UiFlags::ColorBlack);
	DrawString(out, text, { finalPos, { width, 12 } }, style | UiFlags::ColorSilver);
}

void LoadCharPanel()
{
	LoadArt("debugart\\charbg.pcx", &PanelFull);
	LoadArt("debugart\\boxleftend.pcx", &PanelParts[0]);
	LoadArt("debugart\\boxmiddle.pcx", &PanelParts[1]);
	LoadArt("debugart\\boxrightend.pcx", &PanelParts[2]);

	Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });
	const Surface out(PanelFull.surface.get());

	for (auto &entry : panelEntries) {
		if (entry.statDisplayFunc != nullptr) {
			DrawThingy(out, pos + entry.position, entry.length);
		}
		DrawShadowString(out, pos, entry);
	}

	//DrawThingy(out, { pos.x + 13, pos.y + 14 }, 134); //name

	//DrawThingy(out, { pos.x + 57, pos.y + 52 }, 45); // level
	//DrawShadowText(out, "LEVEL", { pos.x + 54, pos.y + 69 });
	/*
	DrawThingy(out, { pos.x + 88, pos.y + 138 }, 33); //str base
	DrawThingy(out, { pos.x + 88, pos.y + 166 }, 33); // magic base
	DrawThingy(out, { pos.x + 88, pos.y + 194 }, 33); //dex base
	DrawThingy(out, { pos.x + 88, pos.y + 222 }, 33); //vita base
	DrawThingy(out, { pos.x + 88, pos.y + 250 }, 33); //points to distribute
	DrawThingy(out, { pos.x + 88, pos.y + 287 }, 33); //curr life
	DrawThingy(out, { pos.x + 88, pos.y + 315 }, 33); //curr mana

	DrawThingy(out, { pos.x + 135, pos.y + 138 }, 33); // str curr
	DrawThingy(out, { pos.x + 135, pos.y + 166 }, 33); // magic curr
	DrawThingy(out, { pos.x + 135, pos.y + 194 }, 33); // dex curr
	DrawThingy(out, { pos.x + 135, pos.y + 222 }, 33); // vita curr
	DrawThingy(out, { pos.x + 135, pos.y + 287 }, 33); //max life
	DrawThingy(out, { pos.x + 135, pos.y + 315 }, 33); //max mana

	DrawThingy(out, { pos.x + 161, pos.y + 14 }, 134); // class

	DrawThingy(out, { pos.x + 208, pos.y + 52 }, 87);  // exp
	DrawThingy(out, { pos.x + 208, pos.y + 80 }, 87);  // next level
	DrawThingy(out, { pos.x + 208, pos.y + 129 }, 87); // gold

	DrawThingy(out, { pos.x + 250, pos.y + 166 }, 45); // armor
	DrawThingy(out, { pos.x + 250, pos.y + 194 }, 45); // to hit
	DrawThingy(out, { pos.x + 250, pos.y + 222 }, 45); // damage
	DrawThingy(out, { pos.x + 250, pos.y + 259 }, 45); // resist magic
	DrawThingy(out, { pos.x + 250, pos.y + 287 }, 45); // resist fire
	DrawThingy(out, { pos.x + 250, pos.y + 315 }, 45); // resist lightning
	*/
	for (auto &gfx : PanelParts) {
		gfx.Unload();
	}
}

bool CharPanelLoaded = false;

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
			DrawString(out, tmp.text.c_str(), { pos + entry.position + Displacement { 7, 17 }, { entry.length, 12 } }, UiFlags::AlignCenter | tmp.color);
		}
	}
}

} // namespace devilution
