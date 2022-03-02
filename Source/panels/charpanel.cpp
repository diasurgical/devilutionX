#include "panels/charpanel.hpp"

#include <string>

#include <fmt/format.h>

#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"
#include "control.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "panels/ui_panels.hpp"
#include "player.h"
#include "utils/display.h"
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
	int spacing = 1;
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
	UiFlags style = UiFlags::ColorWhite;
	if (MyPlayer->GetBaseAttributeValue(attr) == MyPlayer->GetMaximumAttributeValue(attr))
		style = UiFlags::ColorWhitegold;
	return style;
}

UiFlags GetCurrentStatColor(CharacterAttribute attr)
{
	UiFlags style = UiFlags::ColorWhite;
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
	UiFlags style = UiFlags::ColorWhite;
	if (value > 0)
		style = (flip ? UiFlags::ColorRed : UiFlags::ColorBlue);
	if (value < 0)
		style = (flip ? UiFlags::ColorBlue : UiFlags::ColorRed);
	return style;
}

UiFlags GetMaxManaColor()
{
	return MyPlayer->_pMaxMana > MyPlayer->_pMaxManaBase ? UiFlags::ColorBlue : UiFlags::ColorWhite;
}

UiFlags GetMaxHealthColor()
{
	return MyPlayer->_pMaxHP > MyPlayer->_pMaxHPBase ? UiFlags::ColorBlue : UiFlags::ColorWhite;
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
		style = UiFlags::ColorWhite;
	else if (resist < 0)
		style = UiFlags::ColorRed;
	else if (resist >= MAXRESIST)
		style = UiFlags::ColorWhitegold;

	return { style, fmt::format("{:d}%", resist) };
}

constexpr int LeftColumnLabelX = 88;
constexpr int TopRightLabelX = 211;
constexpr int RightColumnLabelX = 253;

constexpr int LeftColumnLabelWidth = 76;
constexpr int RightColumnLabelWidth = 68;

// Indices in `panelEntries`.
constexpr unsigned AttributeHeaderEntryIndices[2] = { 5, 6 };
constexpr unsigned GoldHeaderEntryIndex = 16;

PanelEntry panelEntries[] = {
	{ "", { 9, 14 }, 150, 0,
	    []() { return StyledText { UiFlags::ColorWhite, MyPlayer->_pName }; } },
	{ "", { 161, 14 }, 149, 0,
	    []() { return StyledText { UiFlags::ColorWhite, _(ClassStrTbl[static_cast<std::size_t>(MyPlayer->_pClass)]) }; } },

	{ N_("Level"), { 57, 52 }, 57, 45,
	    []() { return StyledText { UiFlags::ColorWhite, fmt::format("{:d}", MyPlayer->_pLevel) }; } },
	{ N_("Experience"), { TopRightLabelX, 52 }, 99, 91,
	    []() { return StyledText { UiFlags::ColorWhite, fmt::format("{:d}", MyPlayer->_pExperience) }; } },
	{ N_("Next level"), { TopRightLabelX, 80 }, 99, 198,
	    []() {
	        if (MyPlayer->_pLevel == MAXCHARLEVEL) {
		        return StyledText { UiFlags::ColorWhitegold, _("None") };
	        }
	        return StyledText { UiFlags::ColorWhite, fmt::format("{:d}", MyPlayer->_pNextExper) };
	    } },

	{ N_("Base"), { LeftColumnLabelX, /* set dynamically */ 0 }, 0, 44 },
	{ N_("Now"), { 135, /* set dynamically */ 0 }, 0, 44 },
	{ N_("Strength"), { LeftColumnLabelX, 135 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Strength), fmt::format("{:d}", MyPlayer->_pBaseStr) }; } },
	{ "", { 135, 135 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Strength), fmt::format("{:d}", MyPlayer->_pStrength) }; } },
	{ N_("Magic"), { LeftColumnLabelX, 163 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Magic), fmt::format("{:d}", MyPlayer->_pBaseMag) }; } },
	{ "", { 135, 163 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Magic), fmt::format("{:d}", MyPlayer->_pMagic) }; } },
	{ N_("Dexterity"), { LeftColumnLabelX, 191 }, 45, LeftColumnLabelWidth, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", MyPlayer->_pBaseDex) }; } },
	{ "", { 135, 191 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Dexterity), fmt::format("{:d}", MyPlayer->_pDexterity) }; } },
	{ N_("Vitality"), { LeftColumnLabelX, 219 }, 45, LeftColumnLabelWidth, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", MyPlayer->_pBaseVit) }; } },
	{ "", { 135, 219 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Vitality), fmt::format("{:d}", MyPlayer->_pVitality) }; } },
	{ N_("Points to distribute"), { LeftColumnLabelX, 248 }, 45, LeftColumnLabelWidth,
	    []() {
	        MyPlayer->_pStatPts = std::min(CalcStatDiff(*MyPlayer), MyPlayer->_pStatPts);
	        return StyledText { UiFlags::ColorRed, (MyPlayer->_pStatPts > 0 ? fmt::format("{:d}", MyPlayer->_pStatPts) : "") };
	    } },

	{ N_("Gold"), { TopRightLabelX, /* set dynamically */ 0 }, 0, 98 },
	{ "", { TopRightLabelX, 127 }, 99, 0,
	    []() { return StyledText { UiFlags::ColorWhite, fmt::format("{:d}", MyPlayer->_pGold) }; } },

	{ N_("Armor class"), { RightColumnLabelX, 163 }, 57, RightColumnLabelWidth,
	    []() { return StyledText { GetValueColor(MyPlayer->_pIBonusAC), fmt::format("{:d}", MyPlayer->GetArmor()) }; } },
	{ N_("To hit"), { RightColumnLabelX, 191 }, 57, RightColumnLabelWidth,
	    []() { return StyledText { GetValueColor(MyPlayer->_pIBonusToHit), fmt::format("{:d}%", (MyPlayer->InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Bow ? MyPlayer->GetRangedToHit() : MyPlayer->GetMeleeToHit())) }; } },
	{ N_("Damage"), { RightColumnLabelX, 219 }, 57, RightColumnLabelWidth,
	    []() {
	        std::pair<int, int> dmg = GetDamage();
	        int spacing = ((dmg.first >= 100) ? -1 : 1);
	        return StyledText { GetValueColor(MyPlayer->_pIBonusDam), fmt::format("{:d}-{:d}", dmg.first, dmg.second), spacing };
	    } },

	{ N_("Life"), { LeftColumnLabelX, 284 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetMaxHealthColor(), fmt::format("{:d}", MyPlayer->_pMaxHP >> 6) }; } },
	{ "", { 135, 284 }, 45, 0,
	    []() { return StyledText { (MyPlayer->_pHitPoints != MyPlayer->_pMaxHP ? UiFlags::ColorRed : GetMaxHealthColor()), fmt::format("{:d}", MyPlayer->_pHitPoints >> 6) }; } },
	{ N_("Mana"), { LeftColumnLabelX, 312 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetMaxManaColor(), fmt::format("{:d}", MyPlayer->_pMaxMana >> 6) }; } },
	{ "", { 135, 312 }, 45, 0,
	    []() { return StyledText { (MyPlayer->_pMana != MyPlayer->_pMaxMana ? UiFlags::ColorRed : GetMaxManaColor()), fmt::format("{:d}", MyPlayer->_pMana >> 6) }; } },

	{ N_("Resist magic"), { RightColumnLabelX, 256 }, 57, RightColumnLabelWidth,
	    []() { return GetResistInfo(MyPlayer->_pMagResist); } },
	{ N_("Resist fire"), { RightColumnLabelX, 284 }, 57, RightColumnLabelWidth,
	    []() { return GetResistInfo(MyPlayer->_pFireResist); } },
	{ N_("Resist lightning"), { RightColumnLabelX, 313 }, 57, RightColumnLabelWidth,
	    []() { return GetResistInfo(MyPlayer->_pLghtResist); } },
};

Art PanelBoxLeft;
Art PanelBoxMiddle;
Art PanelBoxRight;
Art PanelFull;

constexpr int PanelFieldHeight = 24;
constexpr int PanelFieldPaddingTop = 3;
constexpr int PanelFieldPaddingBottom = 3;
constexpr int PanelFieldInnerHeight = PanelFieldHeight - PanelFieldPaddingTop - PanelFieldPaddingBottom;

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
	if (entry.label.empty())
		return;

	constexpr int Spacing = 0;
	const std::string &textStr = LanguageTranslate(entry.label.c_str());
	string_view text;
	std::string wrapped;
	if (entry.labelLength > 0) {
		wrapped = WordWrapString(textStr, entry.labelLength, GameFont12, Spacing);
		text = wrapped;
	} else {
		text = textStr;
	}

	UiFlags style = UiFlags::VerticalCenter;

	Point labelPosition = entry.position;

	if (entry.length == 0) {
		style |= UiFlags::AlignCenter;
	} else {
		style |= UiFlags::AlignRight;
		labelPosition += Displacement { -entry.labelLength - (IsSmallFontTall() ? 2 : 3), 0 };
	}

	DrawString(out, text, { labelPosition + Displacement { -2, 2 }, { entry.labelLength, PanelFieldHeight } }, style | UiFlags::ColorBlack, Spacing);
	DrawString(out, text, { labelPosition, { entry.labelLength, PanelFieldHeight } }, style | UiFlags::ColorWhite, Spacing);
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

	const bool isSmallFontTall = IsSmallFontTall();
	const int attributeHeadersY = isSmallFontTall ? 112 : 114;
	for (unsigned i : AttributeHeaderEntryIndices) {
		panelEntries[i].position.y = attributeHeadersY;
	}
	panelEntries[GoldHeaderEntryIndex].position.y = isSmallFontTall ? 105 : 106;

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
			    tmp.text,
			    { entry.position + Displacement { pos.x, pos.y + PanelFieldPaddingTop }, { entry.length, PanelFieldInnerHeight } },
			    UiFlags::AlignCenter | UiFlags::VerticalCenter | tmp.style, tmp.spacing);
		}
	}
	DrawStatButtons(out);
}

} // namespace devilution
