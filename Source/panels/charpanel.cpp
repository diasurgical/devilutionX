#include "panels/charpanel.hpp"

#include <cstdint>

#include <algorithm>
#include <string>

#include <fmt/format.h>
#include <function_ref.hpp>

#include "control.h"
#include "engine/load_clx.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/render/text_render.hpp"
#include "missiles.h"
#include "panels/ui_panels.hpp"
#include "player.h"
#include "playerdat.hpp"
#include "utils/display.h"
#include "utils/format_int.hpp"
#include "utils/language.h"
#include "utils/str_cat.hpp"
#include "utils/surface_to_clx.hpp"

namespace devilution {

OptionalOwnedClxSpriteList pChrButtons;

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
	int labelLength;                                               // max label's length - used for line wrapping
	std::optional<tl::function_ref<StyledText()>> statDisplayFunc; // function responsible for displaying stat
};

UiFlags GetBaseStatColor(CharacterAttribute attr)
{
	UiFlags style = UiFlags::ColorWhite;
	if (InspectPlayer->GetBaseAttributeValue(attr) == InspectPlayer->GetMaximumAttributeValue(attr))
		style = UiFlags::ColorWhitegold;
	return style;
}

UiFlags GetCurrentStatColor(CharacterAttribute attr)
{
	UiFlags style = UiFlags::ColorWhite;
	int current = InspectPlayer->GetCurrentAttributeValue(attr);
	int base = InspectPlayer->GetBaseAttributeValue(attr);
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
	return InspectPlayer->_pMaxMana > InspectPlayer->_pMaxManaBase ? UiFlags::ColorBlue : UiFlags::ColorWhite;
}

UiFlags GetMaxHealthColor()
{
	return InspectPlayer->_pMaxHP > InspectPlayer->_pMaxHPBase ? UiFlags::ColorBlue : UiFlags::ColorWhite;
}

std::pair<int, int> GetDamage()
{
	int damageMod = InspectPlayer->_pIBonusDamMod;
	if (InspectPlayer->InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Bow && InspectPlayer->_pClass != HeroClass::Rogue) {
		damageMod += InspectPlayer->_pDamageMod / 2;
	} else {
		damageMod += InspectPlayer->_pDamageMod;
	}
	int mindam = InspectPlayer->_pIMinDam + InspectPlayer->_pIBonusDam * InspectPlayer->_pIMinDam / 100 + damageMod;
	int maxdam = InspectPlayer->_pIMaxDam + InspectPlayer->_pIBonusDam * InspectPlayer->_pIMaxDam / 100 + damageMod;
	return { mindam, maxdam };
}

std::pair<int, int> GetSpellDamage()
{
	int min;
	int max;

	GetDamageAmt(InspectPlayer->_pRSpell, &min, &max);

	return { min, max };
}

UiFlags GetSpellTextColor()
{
	UiFlags color;

	switch (InspectPlayer->_pRSpell) {
	case SpellID::Firebolt:
	case SpellID::FireWall:
	case SpellID::Fireball:
	case SpellID::Guardian:
	case SpellID::FlameWave:
	case SpellID::Inferno:
	case SpellID::Elemental:
	case SpellID::Immolation:
	case SpellID::RingOfFire:
	case SpellID::RuneOfFire:
	case SpellID::RuneOfImmolation:
		color = UiFlags::ColorOrange;
		break;
	case SpellID::Lightning:
	case SpellID::ChainLightning:
	case SpellID::Nova:
	case SpellID::ChargedBolt:
	case SpellID::LightningWall:
	case SpellID::RuneOfLight:
	case SpellID::RuneOfNova:
		color = UiFlags::ColorYellow;
		break;
	case SpellID::Healing:
	case SpellID::Flash:
	case SpellID::HealOther:
	case SpellID::BloodStar:
	case SpellID::BoneSpirit:
		color = UiFlags::ColorBlue;
		break;
	case SpellID::DoomSerpents:
	case SpellID::Apocalypse:
		color = UiFlags::ColorRed;
		break;
	default:
		color = UiFlags::ColorWhite;
		break;
	}

	return color;
}

StyledText GetResistInfo(int8_t resist)
{
	UiFlags style = UiFlags::ColorBlue;
	if (resist == 0)
		style = UiFlags::ColorWhite;
	else if (resist < 0)
		style = UiFlags::ColorRed;
	else if (resist >= MaxResistance)
		style = UiFlags::ColorWhitegold;

	return { style, StrCat(resist, "%") };
}

constexpr int LeftColumnLabelX = 63;
constexpr int CenterColumnLabelX = LeftColumnLabelX + 47;
constexpr int TopLeftLabelX = 9;
constexpr int TopCenterLabelX = 110;
constexpr int TopRightLabelX = 211;
constexpr int RightColumnLabelX = 253;

constexpr int LeftColumnLabelWidth = 76;
constexpr int RightColumnLabelWidth = 68;

// Indices in `panelEntries`.
constexpr unsigned ExperienceHeaderEntryIndices[2] = { 2, 4 };
constexpr unsigned AttributeHeaderEntryIndices[2] = { 6, 7 };
constexpr unsigned GoldHeaderEntryIndex = 16;

PanelEntry panelEntries[] = {
	{ "", { 9, 14 }, 150, 0,
	    []() { return StyledText { UiFlags::ColorWhite, InspectPlayer->_pName }; } },
	{ "", { 161, 14 }, 149, 0,
	    []() { return StyledText { UiFlags::ColorWhite, StrCat("Lvl ", InspectPlayer->_pLevel, " ", std::string(_(PlayersData[static_cast<std::size_t>(InspectPlayer->_pClass)].className))) }; } },

	{ N_("Experience"), { TopCenterLabelX, /* set dynamically */ 0 }, 0, 99, {} },
	{ "", { TopCenterLabelX, 62 }, 99, 0,
	    []() {
		int spacing = ((InspectPlayer->_pExperience >= 1000000000) ? 0 : 1);
		return StyledText { UiFlags::ColorWhite, FormatInteger(InspectPlayer->_pExperience), spacing }; } },
	{ N_("Next level"), { TopRightLabelX, /* set dynamically */ 0 }, 0, 99, {} },
	{ "", { TopRightLabelX, 62 }, 99, 0,
	    []() {
		if (InspectPlayer->_pLevel == MaxCharacterLevel) {
			return StyledText { UiFlags::ColorWhitegold, std::string(_("None")) };
		}
		int spacing = ((InspectPlayer->_pNextExper >= 1000000000) ? 0 : 1);
		return StyledText { UiFlags::ColorWhite, FormatInteger(InspectPlayer->_pNextExper), spacing }; } },

	{ N_("Base"), { LeftColumnLabelX, /* set dynamically */ 0 }, 0, 44, {} },
	{ N_("Now"), { CenterColumnLabelX, /* set dynamically */ 0 }, 0, 44, {} },
	{ N_("Str"), { LeftColumnLabelX, 107 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Strength), StrCat(InspectPlayer->_pBaseStr) }; } },
	{ "", { CenterColumnLabelX, 107 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Strength), StrCat(InspectPlayer->_pStrength) }; } },
	{ N_("Mag"), { LeftColumnLabelX, 135 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetBaseStatColor(CharacterAttribute::Magic), StrCat(InspectPlayer->_pBaseMag) }; } },
	{ "", { CenterColumnLabelX, 135 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Magic), StrCat(InspectPlayer->_pMagic) }; } },
	{ N_("Dex"), { LeftColumnLabelX, 163 }, 45, LeftColumnLabelWidth, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Dexterity), StrCat(InspectPlayer->_pBaseDex) }; } },
	{ "", { CenterColumnLabelX, 163 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Dexterity), StrCat(InspectPlayer->_pDexterity) }; } },
	{ N_("Vit"), { LeftColumnLabelX, 191 }, 45, LeftColumnLabelWidth, []() { return StyledText { GetBaseStatColor(CharacterAttribute::Vitality), StrCat(InspectPlayer->_pBaseVit) }; } },
	{ "", { CenterColumnLabelX, 191 }, 45, 0,
	    []() { return StyledText { GetCurrentStatColor(CharacterAttribute::Vitality), StrCat(InspectPlayer->_pVitality) }; } },

	{ N_("Gold"), { TopLeftLabelX, /* set dynamically */ 0 }, 0, 98, {} },
	{ "", { TopLeftLabelX, 62 }, 99, 0,
	    []() { return StyledText { UiFlags::ColorWhite, FormatInteger(InspectPlayer->_pGold) }; } },

	{ N_("Armor class"), { RightColumnLabelX, 107 }, 57, RightColumnLabelWidth,
	    []() { return StyledText { GetValueColor(InspectPlayer->_pIBonusAC), StrCat(InspectPlayer->GetArmor() + InspectPlayer->_pLevel * 2) }; } },
	{ N_("Chance to hit"), { RightColumnLabelX, 135 }, 57, RightColumnLabelWidth,
	    []() { return StyledText { GetValueColor(InspectPlayer->_pIBonusToHit), StrCat(InspectPlayer->InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Bow ? InspectPlayer->GetRangedToHit() : InspectPlayer->GetMeleeToHit(), "%") }; } },
	{ N_("Damage"), { RightColumnLabelX, 163 }, 57, RightColumnLabelWidth,
	    []() {
	        std::pair<int, int> dmg = GetDamage();
	        int spacing = ((dmg.first >= 100) ? -1 : 1);
	        return StyledText { GetValueColor(InspectPlayer->_pIBonusDam), StrCat(dmg.first, "-", dmg.second), spacing };
	    } },

	{ N_("Life"), { LeftColumnLabelX, 256 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetMaxHealthColor(), StrCat(InspectPlayer->_pMaxHP >> 6) }; } },
	{ "", { CenterColumnLabelX, 256 }, 45, 0,
	    []() { return StyledText { (InspectPlayer->_pHitPoints != InspectPlayer->_pMaxHP ? UiFlags::ColorRed : GetMaxHealthColor()), StrCat(InspectPlayer->_pHitPoints >> 6) }; } },
	{ N_("Mana"), { LeftColumnLabelX, 284 }, 45, LeftColumnLabelWidth,
	    []() { return StyledText { GetMaxManaColor(), StrCat(InspectPlayer->_pMaxMana >> 6) }; } },
	{ "", { CenterColumnLabelX, 284 }, 45, 0,
	    []() { return StyledText { (InspectPlayer->_pMana != InspectPlayer->_pMaxMana ? UiFlags::ColorRed : GetMaxManaColor()), StrCat(InspectPlayer->_pMana >> 6) }; } },

	{ N_("Resist magic"), { RightColumnLabelX, 256 }, 57, RightColumnLabelWidth,
	    []() { return GetResistInfo(InspectPlayer->_pMagResist); } },
	{ N_("Resist fire"), { RightColumnLabelX, 284 }, 57, RightColumnLabelWidth,
	    []() { return GetResistInfo(InspectPlayer->_pFireResist); } },
	{ N_("Resist lightning"), { RightColumnLabelX, 313 }, 57, RightColumnLabelWidth,
	    []() { return GetResistInfo(InspectPlayer->_pLghtResist); } },
};

PanelEntry pointsToDistributeEntry[] = {
	{ N_("Stat points remaining"), { LeftColumnLabelX + 47, 220 }, 45, LeftColumnLabelWidth + 47,
	    []() {
	        InspectPlayer->_pStatPts = std::min(CalcStatDiff(*InspectPlayer), InspectPlayer->_pStatPts);
	        return StyledText { UiFlags::ColorRed, (InspectPlayer->_pStatPts > 0 ? StrCat(InspectPlayer->_pStatPts) : "") };
	    } }
};

PanelEntry spellDamageEntry[] = {
	{ N_("Spell Damage"), { RightColumnLabelX, 191 }, 57, RightColumnLabelWidth,
	    []() {
	        std::pair<int, int> dmg = GetSpellDamage();
	        int spacing = ((dmg.first >= 100) ? -1 : 1);
	        return StyledText { GetSpellTextColor(), StrCat(dmg.first, "-", dmg.second), spacing };
	    } },
};

OptionalOwnedClxSpriteList Panel;

constexpr int PanelFieldHeight = 24;
constexpr int PanelFieldPaddingTop = 3;
constexpr int PanelFieldPaddingBottom = 3;
constexpr int PanelFieldInnerHeight = PanelFieldHeight - PanelFieldPaddingTop - PanelFieldPaddingBottom;

void DrawPanelField(const Surface &out, Point pos, int len, ClxSprite left, ClxSprite middle, ClxSprite right)
{
	RenderClxSprite(out, left, pos);
	pos.x += left.width();
	len -= left.width() + right.width();
	RenderClxSprite(out.subregion(pos.x, pos.y, len, middle.height()), middle, Point { 0, 0 });
	pos.x += len;
	RenderClxSprite(out, right, pos);
}

void DrawShadowString(const Surface &out, const PanelEntry &entry)
{
	if (entry.label.empty())
		return;

	constexpr int Spacing = 0;
	const string_view textStr = LanguageTranslate(entry.label);
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

	// If the text is less tall then the field, we center it vertically relative to the field.
	// Otherwise, we draw from the top of the field.
	const int textHeight = (std::count(wrapped.begin(), wrapped.end(), '\n') + 1) * GetLineHeight(wrapped, GameFont12);
	const int labelHeight = std::max(PanelFieldHeight, textHeight);

	DrawString(out, text, { labelPosition + Displacement { -2, 2 }, { entry.labelLength, labelHeight } }, style | UiFlags::ColorBlack, Spacing);
	DrawString(out, text, { labelPosition, { entry.labelLength, labelHeight } }, style | UiFlags::ColorWhite, Spacing);
}

void DrawStatButtons(const Surface &out)
{
	if (InspectPlayer->_pStatPts > 0 && !IsInspectingPlayer()) {
		auto &entry = pointsToDistributeEntry[0];
		if (entry.statDisplayFunc) {
			OwnedClxSpriteList boxLeft = LoadClx("data\\boxleftend.clx");
			OwnedClxSpriteList boxMiddle = LoadClx("data\\boxmiddle.clx");
			OwnedClxSpriteList boxRight = LoadClx("data\\boxrightend.clx");
			Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });
			StyledText tmp = (*entry.statDisplayFunc)();

			DrawPanelField(out, entry.position, entry.length, boxLeft[0], boxMiddle[0], boxRight[0]);

			DrawString(
			    out,
			    tmp.text,
			    { entry.position + Displacement { pos.x, pos.y + PanelFieldPaddingTop }, { entry.length, PanelFieldInnerHeight } },
			    UiFlags::AlignCenter | UiFlags::VerticalCenter | tmp.style, tmp.spacing);
		}
		DrawShadowString(out, entry);

		if (InspectPlayer->_pBaseStr < InspectPlayer->GetMaximumAttributeValue(CharacterAttribute::Strength))
			ClxDraw(out, GetPanelPosition(UiPanels::Character, { 113, 129 }), (*pChrButtons)[chrbtn[static_cast<size_t>(CharacterAttribute::Strength)] ? 2 : 1]);
		if (InspectPlayer->_pBaseMag < InspectPlayer->GetMaximumAttributeValue(CharacterAttribute::Magic))
			ClxDraw(out, GetPanelPosition(UiPanels::Character, { 113, 157 }), (*pChrButtons)[chrbtn[static_cast<size_t>(CharacterAttribute::Magic)] ? 4 : 3]);
		if (InspectPlayer->_pBaseDex < InspectPlayer->GetMaximumAttributeValue(CharacterAttribute::Dexterity))
			ClxDraw(out, GetPanelPosition(UiPanels::Character, { 113, 185 }), (*pChrButtons)[chrbtn[static_cast<size_t>(CharacterAttribute::Dexterity)] ? 6 : 5]);
		if (InspectPlayer->_pBaseVit < InspectPlayer->GetMaximumAttributeValue(CharacterAttribute::Vitality))
			ClxDraw(out, GetPanelPosition(UiPanels::Character, { 113, 214 }), (*pChrButtons)[chrbtn[static_cast<size_t>(CharacterAttribute::Vitality)] ? 8 : 7]);
	}
}

} // namespace

void LoadCharPanel()
{
	OptionalOwnedClxSpriteList background = LoadClx("data\\charbg.clx");
	OwnedSurface out((*background)[0].width(), (*background)[0].height());
	RenderClxSprite(out, (*background)[0], { 0, 0 });
	background = std::nullopt;

	{
		OwnedClxSpriteList boxLeft = LoadClx("data\\boxleftend.clx");
		OwnedClxSpriteList boxMiddle = LoadClx("data\\boxmiddle.clx");
		OwnedClxSpriteList boxRight = LoadClx("data\\boxrightend.clx");

		const bool isSmallFontTall = IsSmallFontTall();
		const int attributeHeadersY = isSmallFontTall ? 84 : 86;
		for (unsigned i : AttributeHeaderEntryIndices) {
			panelEntries[i].position.y = attributeHeadersY;
		}
		const int experienceHeadersY = isSmallFontTall ? 40 : 41;
		for (unsigned i : ExperienceHeaderEntryIndices) {
			panelEntries[i].position.y = experienceHeadersY;
		}
		panelEntries[GoldHeaderEntryIndex].position.y = isSmallFontTall ? 40 : 41;

		for (auto &entry : panelEntries) {
			if (entry.statDisplayFunc) {
				DrawPanelField(out, entry.position, entry.length, boxLeft[0], boxMiddle[0], boxRight[0]);
			}
			DrawShadowString(out, entry);
		}
	}

	Panel = SurfaceToClx(out);
}

void FreeCharPanel()
{
	Panel = std::nullopt;
}

void DrawChr(const Surface &out)
{
	Point pos = GetPanelPosition(UiPanels::Character, { 0, 0 });
	RenderClxSprite(out, (*Panel)[0], pos);

	for (auto &entry : panelEntries) {
		if (entry.statDisplayFunc) {
			StyledText tmp = (*entry.statDisplayFunc)();
			DrawString(
			    out,
			    tmp.text,
			    { entry.position + Displacement { pos.x, pos.y + PanelFieldPaddingTop }, { entry.length, PanelFieldInnerHeight } },
			    UiFlags::AlignCenter | UiFlags::VerticalCenter | tmp.style, tmp.spacing);
		}
	}

	OwnedClxSpriteList boxLeft = LoadClx("data\\boxleftend.clx");
	OwnedClxSpriteList boxMiddle = LoadClx("data\\boxmiddle.clx");
	OwnedClxSpriteList boxRight = LoadClx("data\\boxrightend.clx");

	if (IsNoneOf(InspectPlayer->_pRSpell, SpellID::Invalid, SpellID::Null)) {
		auto &entry = spellDamageEntry[0];

		if (entry.statDisplayFunc) {

			StyledText tmp = (*entry.statDisplayFunc)();

			DrawPanelField(out, entry.position, entry.length, boxLeft[0], boxMiddle[0], boxRight[0]);

			std::pair<int, int> damage = GetSpellDamage();
			if (damage.first != -1 && damage.second != -1 && IsNoneOf(InspectPlayer->_pRSpell, SpellID::Jester, SpellID::Mana, SpellID::Magi)) {
				DrawString(
				    out,
				    tmp.text,
				    { entry.position + Displacement { pos.x, pos.y + PanelFieldPaddingTop }, { entry.length, PanelFieldInnerHeight } },
				    UiFlags::AlignCenter | UiFlags::VerticalCenter | tmp.style, tmp.spacing);
			}
		}

		entry.label = GetSpellData(InspectPlayer->_pRSpell).sNameText;
		DrawShadowString(out, entry);
	}

	DrawStatButtons(out);
}

} // namespace devilution
