/**
 * @file floatinginfobox.cpp
 *
 * Adds floating info box QoL feature
 */

#include "qol/floatinginfobox.hpp"

#include "itemlabels.h"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "control.h"
#include "controls/plrctrls.h"
#include "cursor.h"
#include "engine/point.hpp"
#include "engine/render/clx_render.hpp"
#include "gmenu.h"
#include "inv.h"
#include "options.h"
#include "qol/stash.h"
#include "utils/format_int.hpp"
#include "utils/language.h"

namespace devilution {

namespace {

} // namespace

enum class ItemStatType {
	Damage,
	Armor,
	Durability
};

UiFlags GetItemBonusColors(const ItemStatType stat, const Item &item)
{
	ItemData iData = AllItemsList[item.IDidx];
	UiFlags color = UiFlags::ColorWhite;

	if (item._iMagical == ITEM_QUALITY_UNIQUE) {
		const UniqueItem &uitem = UniqueItems[item._iUid];
		assert(uitem.UINumPL <= sizeof(uitem.powers) / sizeof(*uitem.powers));
		for (const auto &power : uitem.powers) {
			switch (power.type) {
			case IPL_ACP:
				if (stat == ItemStatType::Armor)
					color = UiFlags::ColorBlue;
				break;
			case IPL_ACP_CURSE:
				if (stat == ItemStatType::Armor)
					color = UiFlags::ColorRed;
				break;
			case IPL_SETAC:
			case IPL_AC_CURSE:
				if (stat == ItemStatType::Armor) {
					if (item._iAC > iData.iMaxAC)
						color = UiFlags::ColorBlue;
					else if (item._iAC < iData.iMinAC)
						color = UiFlags::ColorRed;
				}
				break;
			case IPL_DAMMOD:
			case IPL_DAMP:
			case IPL_TOHIT_DAMP:
				if (stat == ItemStatType::Damage)
					color = UiFlags::ColorBlue;
				break;
			case IPL_DAMP_CURSE:
			case IPL_TOHIT_DAMP_CURSE:
				if (stat == ItemStatType::Damage)
					color = UiFlags::ColorRed;
				break;
			case IPL_SETDAM:
				if (stat == ItemStatType::Damage) {
					if ((item._iMinDam > iData.iMinDam) || (item._iMaxDam > iData.iMaxDam))
						color = UiFlags::ColorBlue;
					else if ((item._iMinDam < iData.iMinDam) || (item._iMaxDam < iData.iMaxDam))
						color = UiFlags::ColorRed;
				}
				break;
			case IPL_DUR:
				if (stat == ItemStatType::Durability)
					color = UiFlags::ColorBlue;
				break;
			case IPL_DUR_CURSE:
				if (stat == ItemStatType::Durability)
					color = UiFlags::ColorRed;
				break;
			case IPL_SETDUR:
				if (stat == ItemStatType::Durability) {
					if (item._iMaxDur > iData.iDurability)
						color = UiFlags::ColorBlue;
					else if (item._iMaxDur < iData.iDurability)
						color = UiFlags::ColorRed;
				}
				break;
			default:
				break;
			}
		}
	} else if (item._iMagical == ITEM_QUALITY_MAGIC) {
		switch (item._iPrePower) {
		case IPL_ACP:
			if (stat == ItemStatType::Armor)
				color = UiFlags::ColorBlue;
			break;
		case IPL_ACP_CURSE:
			if (stat == ItemStatType::Armor)
				color = UiFlags::ColorRed;
			break;
		case IPL_DAMP:
		case IPL_TOHIT_DAMP:
			if (stat == ItemStatType::Damage)
				color = UiFlags::ColorBlue;
			break;
		case IPL_DAMP_CURSE:
		case IPL_TOHIT_DAMP_CURSE:
			if (stat == ItemStatType::Damage)
				color = UiFlags::ColorRed;
			break;
		default:
			break;
		}
		switch (item._iSufPower) {
		case IPL_DUR:
			if (stat == ItemStatType::Durability)
				color = UiFlags::ColorBlue;
			break;
		case IPL_DUR_CURSE:
			if (stat == ItemStatType::Durability)
				color = UiFlags::ColorRed;
			break;
		default:
			break;
		}
	}

	return color;
}

enum class ItemRequirementType {
	Strength,
	Dexterity,
	Magic
};

UiFlags GetItemRequirementColors(const ItemRequirementType stat, const Item &item)
{
	const Player &myPlayer = *MyPlayer;
	const UiFlags colorMinRequirementMet = UiFlags::ColorWhite;
	const UiFlags colorMinRequirementNotMet = UiFlags::ColorRed;

	UiFlags color = colorMinRequirementMet;

	if (stat == ItemRequirementType::Strength) {
		if (item._iMinStr > myPlayer._pStrength)
			color = colorMinRequirementNotMet;
	} else if (stat == ItemRequirementType::Dexterity) {
		if (item._iMinDex > myPlayer._pDexterity)
			color = colorMinRequirementNotMet;
	} else if (stat == ItemRequirementType::Magic) {
		if (item._iMinMag > myPlayer._pMagic)
			color = colorMinRequirementNotMet;
	}

	return color;
}

enum class ItemStatModifier {
	MinDamage,
	MaxDamage,
	Armor
};

void CalculateArmorPercentMod(const Item &item, int16_t &modifiedArmor)
{
	modifiedArmor = modifiedArmor * (item._iPLAC + 100) / 100;
}

void CalculateDamPercentMod(const Item &item, int16_t &modifiedDam)
{
	modifiedDam = modifiedDam * (item._iPLDam + 100) / 100;
}

void CalculateDamMod(const Item &item, int16_t &modifiedDam)
{
	modifiedDam = modifiedDam + item._iPLDamMod;
}

int16_t CalculateModifiedStatValue(const ItemStatModifier stat, const Item &item)
{
	assert(IsAnyOf(stat, ItemStatModifier::MinDamage, ItemStatModifier::MaxDamage, ItemStatModifier::Armor));

	int16_t modifiedMinDam = item._iMinDam;
	int16_t modifiedMaxDam = item._iMaxDam;
	int16_t modifiedArmor = item._iAC;

	if (item._iMagical == ITEM_QUALITY_UNIQUE) {
		const UniqueItem &uitem = UniqueItems[item._iUid];
		assert(uitem.UINumPL <= sizeof(uitem.powers) / sizeof(*uitem.powers));

		for (const auto &power : uitem.powers) {
			switch (power.type) {
			case IPL_ACP:
			case IPL_ACP_CURSE:
				if (stat == ItemStatModifier::Armor) {
					CalculateArmorPercentMod(item, modifiedArmor);
				}
				break;
			case IPL_DAMP:
			case IPL_DAMP_CURSE:
			case IPL_TOHIT_DAMP:
			case IPL_TOHIT_DAMP_CURSE:
				if (stat == ItemStatModifier::MinDamage) {
					CalculateDamPercentMod(item, modifiedMinDam);
				} else if (stat == ItemStatModifier::MaxDamage) {
					CalculateDamPercentMod(item, modifiedMaxDam);
				}
				break;
			case IPL_DAMMOD:
				if (stat == ItemStatModifier::MinDamage) {
					CalculateDamMod(item, modifiedMinDam);
				} else if (stat == ItemStatModifier::MaxDamage) {
					CalculateDamMod(item, modifiedMaxDam);
				}
				break;
			default:
				break;
			}
		}
	} else if (item._iMagical == ITEM_QUALITY_MAGIC) {
		switch (item._iPrePower) {
		case IPL_ACP:
		case IPL_ACP_CURSE:
			if (stat == ItemStatModifier::Armor) {
				CalculateArmorPercentMod(item, modifiedArmor);
			}
			break;
		case IPL_DAMP:
		case IPL_DAMP_CURSE:
		case IPL_TOHIT_DAMP:
		case IPL_TOHIT_DAMP_CURSE:
			if (stat == ItemStatModifier::MinDamage) {
				CalculateDamPercentMod(item, modifiedMinDam);
			} else if (stat == ItemStatModifier::MaxDamage) {
				CalculateDamPercentMod(item, modifiedMaxDam);
			}
			break;
		default:
			break;
		}

		switch (item._iSufPower) {
		case IPL_DAMMOD:
			if (stat == ItemStatModifier::MinDamage)
				CalculateDamPercentMod(item, modifiedMinDam);
			if (stat == ItemStatModifier::MaxDamage)
				CalculateDamPercentMod(item, modifiedMaxDam);
			break;
		default:
			break;
		}
	}

	switch (stat) {
	case ItemStatModifier::MinDamage:
		return modifiedMinDam;
	case ItemStatModifier::MaxDamage:
		return modifiedMaxDam;
	case ItemStatModifier::Armor:
		return modifiedArmor;
	}
}

int CountSetBits(int value)
{
	int count = 0;
	while (value != 0) {
		if (value & 1)
			count++;
		value >>= 1;
	}
	return count;
}

void DrawFloatingItemInfoBox(const Surface &out, Point position)
{
	Player &myPlayer = *MyPlayer;

	// Add the lines vector contents into the linesWithColor vector, which contains both the strings and the colors that they will be
	std::vector<DrawStringFormatArg> linesWithColor;

	// Get which item the cursor is currently over
	Item &item = pcursinvitem != -1 ? GetInventoryItem(myPlayer, pcursinvitem) : Stash.stashList[pcursstashitem];

	// Add Item Name

	std::string formattedGold;
	if (item._iClass == ICLASS_GOLD) {
		int nGold = item._ivalue;
		formattedGold = fmt::format(fmt::runtime(ngettext("{:s} gold piece", "{:s} gold pieces", nGold)), FormatInteger(nGold));
		linesWithColor.emplace_back(formattedGold, UiFlags::ColorWhite);
	} else {
		if (item._iIdentified && item._iMagical != ITEM_QUALITY_NORMAL) {
			linesWithColor.emplace_back(item._iIName, item.getTextColor());
		}
		linesWithColor.emplace_back(item._iName, item.getTextColor());
	}

	// Add Item Damage
	std::string formattedDam;
	if (item._iClass == ICLASS_WEAPON || item._iMinDam != 0 || item._iMaxDam != 0) {
		int16_t minDam = CalculateModifiedStatValue(ItemStatModifier::MinDamage, item);
		int16_t maxDam = CalculateModifiedStatValue(ItemStatModifier::MaxDamage, item);
		UiFlags damColor = GetItemBonusColors(ItemStatType::Damage, item);
		linesWithColor.emplace_back(_("Damage:"), UiFlags::ColorWhite);

		if (item._iMinDam == item._iMaxDam) {
			formattedDam = fmt::format(fmt::runtime(_("{:d}")), minDam);
			linesWithColor.emplace_back(formattedDam, damColor);
		} else {
			formattedDam = fmt::format(fmt::runtime(_("{:d} to {:d}")), minDam, maxDam);
			linesWithColor.emplace_back(formattedDam, damColor);
		}
	}

	// Add Item Armor
	std::string formattedArmor;
	if (item._iClass == ICLASS_ARMOR || item._iAC != 0) {
		int16_t armor = CalculateModifiedStatValue(ItemStatModifier::Armor, item);
		UiFlags armorColor = GetItemBonusColors(ItemStatType::Armor, item);
		linesWithColor.emplace_back(_("Armor:"), UiFlags::ColorWhite);
		formattedArmor = fmt::format(fmt::runtime(_("{:d}")), armor);
		linesWithColor.emplace_back(formattedArmor, armorColor);
	}

	// Add Item Durability
	std::string formattedDur;
	if (item._iMaxDur != DUR_INDESTRUCTIBLE && (item._iClass == ICLASS_WEAPON || item._iClass == ICLASS_ARMOR)) {
		UiFlags durColor = GetItemBonusColors(ItemStatType::Durability, item);
		linesWithColor.emplace_back(_("Durability:"), UiFlags::ColorWhite);
		formattedDur = fmt::format(fmt::runtime(_("{:d} of {:d}")), item._iDurability, item._iMaxDur);
		linesWithColor.emplace_back(formattedDur, durColor);
	}

	// Add Item Requirements
	if (item._iMinStr > 0) {
		UiFlags strReqCol = GetItemRequirementColors(ItemRequirementType::Strength, item);
		linesWithColor.emplace_back(_("Required Strength:"), strReqCol);
		linesWithColor.emplace_back((_("{:d}"), item._iMinStr), strReqCol);
	}
	if (item._iMinDex > 0) {
		UiFlags dexReqCol = GetItemRequirementColors(ItemRequirementType::Dexterity, item);
		linesWithColor.emplace_back(_("Required Dexterity:"), dexReqCol);
		linesWithColor.emplace_back((_("{:d}"), item._iMinDex), dexReqCol);
	}
	if (item._iMinMag > 0) {
		UiFlags magReqCol = GetItemRequirementColors(ItemRequirementType::Magic, item);
		linesWithColor.emplace_back(_("Required Magic:"), magReqCol);
		linesWithColor.emplace_back((_("{:d}"), item._iMinMag), magReqCol);
	}

	// Add Item Identification Status
	if (!item._iIdentified && item._iMagical) {
		linesWithColor.emplace_back(_("Not Identified"), UiFlags::ColorRed);
	}

	// Add Item Charges
	std::string formattedCharges;
	if (item._iMaxCharges > 0) {
		const std::string_view spellName = GetSpellData(item._iSpell).sNameText;
		formattedCharges = fmt::format("{:s} ({:d}/{:d} Charges)", spellName, item._iCharges, item._iMaxCharges);
		linesWithColor.emplace_back(formattedCharges, UiFlags::ColorBlue);
	}

	// Add Magical Item Prefix Power
	StringOrView prePower;
	if (item._iPrePower != -1 && item._iIdentified) {
		prePower = PrintItemPower(item._iPrePower, item);
		linesWithColor.emplace_back(prePower, UiFlags::ColorBlue);
	}

	// Add Magical Item Suffix Power
	StringOrView sufPower;
	if (item._iSufPower != -1 && item._iIdentified) {
		sufPower = PrintItemPower(item._iSufPower, item);
		linesWithColor.emplace_back(sufPower, UiFlags::ColorBlue);
	}

	// Add Unique Item Powers
	StringOrView uPower1;
	StringOrView uPower2;
	StringOrView uPower3;
	StringOrView uPower4;
	StringOrView uPower5;
	StringOrView uPower6;
	if (item._iMagical == ITEM_QUALITY_UNIQUE) {
		const UniqueItem &uitem = UniqueItems[item._iUid];
		assert(uitem.UINumPL <= sizeof(uitem.powers) / sizeof(*uitem.powers));
		for (int i = 0; i < uitem.UINumPL; i++) {
			if (IsAnyOf(uitem.powers[i].type, IPL_INVALID, IPL_INVCURS)) {
				continue;
			}
			switch (i) {
			case 0:
				uPower1 = PrintItemPower(uitem.powers[i].type, item);
				linesWithColor.emplace_back(uPower1, UiFlags::ColorBlue);
				break;
			case 1:
				uPower2 = PrintItemPower(uitem.powers[i].type, item);
				linesWithColor.emplace_back(uPower2, UiFlags::ColorBlue);
				break;
			case 2:
				uPower3 = PrintItemPower(uitem.powers[i].type, item);
				linesWithColor.emplace_back(uPower3, UiFlags::ColorBlue);
				break;
			case 3:
				uPower4 = PrintItemPower(uitem.powers[i].type, item);
				linesWithColor.emplace_back(uPower4, UiFlags::ColorBlue);
				break;
			case 4:
				uPower5 = PrintItemPower(uitem.powers[i].type, item);
				linesWithColor.emplace_back(uPower5, UiFlags::ColorBlue);
				break;
			case 5:
				uPower6 = PrintItemPower(uitem.powers[i].type, item);
				linesWithColor.emplace_back(uPower6, UiFlags::ColorBlue);
				break;
			}
		}
	}

	// Add Item Value
	std::string formattedValue;
	if (IsAnyOf(item._iClass, ICLASS_ARMOR, ICLASS_WEAPON, ICLASS_MISC)) {
		int32_t value = item._iIdentified ? item._iIvalue : item._ivalue;
		linesWithColor.emplace_back(_("Value:"), UiFlags::ColorWhite);
		formattedValue = fmt::format(fmt::runtime(_("{:s}")), FormatInteger(value));
		linesWithColor.emplace_back(formattedValue, UiFlags::ColorWhite);
	}

	// CONSTRUCT STRING AS A BASE FOR UTILIZING LINESWITHCOLOR DATA
	// linesBase is used to place the {} and newlines to grab the actual string data
	// Lines that contain a colon may have a different color for the second part, so two {}'s are added
	std::string linesBase;
	int totalLinesCount = 0;
	bool previousLineHadColon = false;

	for (const auto &arg : linesWithColor) {
		const auto &formatted = arg.GetFormatted();
		bool currentLineHasColon = !formatted.empty() && formatted.back() == ':';

		if (!previousLineHadColon && (!currentLineHasColon || (!linesBase.empty() && currentLineHasColon))) {
			if (currentLineHasColon) {
				linesBase.append("{} {}");
			} else {
				linesBase.append("{}");
			}
			if (&arg != &linesWithColor.back()) {
				linesBase.append("\n");
			}
			totalLinesCount++;
		}

		previousLineHadColon = currentLineHasColon;
	}

	// Adjust the total line count if the last line had a colon
	if (previousLineHadColon) {
		totalLinesCount--;
	}

	// CONSTRUCT AND DRAW TRANSPARENT BOX

	Rectangle infoBoxRect {
		{ 0, 0 },
		{ 0, 0 }
	};

	const int spacing = 4;
	// GetLineHeight always returns 12 in this function
	const int lineHeight = 12 + spacing;
	const Size infoBoxPadding = { 16, 16 };

	int maxWidth = 0;
	size_t numLines = linesWithColor.size();

	for (size_t i = 0; i < numLines; i++) {
		const auto &arg = linesWithColor[i];
		int lineWidth = GetLineWidth(arg.GetFormatted(), GameFontTables::GameFont12, 2);

		if (!arg.GetFormatted().empty() && arg.GetFormatted().back() == ':' && i < numLines - 1) {
			const auto &nextArg = linesWithColor[i + 1];
			std::string combinedText = std::string(arg.GetFormatted()) + " " + std::string(nextArg.GetFormatted());
			// The addition of the infobox padding is a hack; I literally have no clue how to make this work without it.
			lineWidth = GetLineWidth(combinedText, GameFontTables::GameFont12, 2) + infoBoxPadding.width;
		}

		maxWidth = std::max(maxWidth, lineWidth);
	}

	int infoBoxWidth = maxWidth + infoBoxPadding.width;

	Size infoBoxSize {
		// width of the longest line of text + horizontal padding
		infoBoxWidth,
		// the height of the number of lines with spacing + vertical padding
		(totalLinesCount * lineHeight) + infoBoxPadding.height
	};

	// Get how many slots the item occupies horizontally and vertically
	Size itemSize = GetInventorySize(item);
	// Padding between inventory slots
	const int invSlotPadding = 1;

	// Define the size and location of the Info Box
	infoBoxRect = {
		{ // Render box centered horiztonally
		    position.x + ((itemSize.width * (INV_SLOT_SIZE_PX + invSlotPadding)) / 2) - (infoBoxSize.width / 2),
		    // Render box on top of item
		    position.y - (itemSize.height * (INV_SLOT_SIZE_PX + invSlotPadding)) - infoBoxSize.height },
		{ infoBoxSize.width,
		    infoBoxSize.height }
	};

	// Prevent top screen clipping for items
	if (infoBoxRect.position.y < 0 && (pcursinvitem != -1 || pcursstashitem != StashStruct::EmptyCell)) {
		// Display box below item instead of above
		infoBoxRect.position.y += (itemSize.height * (INV_SLOT_SIZE_PX + invSlotPadding)) + infoBoxSize.height;
	}

	/* PREVENT CLIPPING */
	// Prevent top screen clipping
	if (infoBoxRect.position.y < 0) {
		infoBoxRect.position.y = 0;
	}
	// Prevent right screen clipping
	if ((infoBoxRect.position.x + infoBoxRect.size.width) > GetScreenWidth())
		infoBoxRect.position.x -= (infoBoxRect.position.x + infoBoxRect.size.width) - GetScreenWidth();
	// Prevent left screen clipping
	if (infoBoxRect.position.x < 0)
		infoBoxRect.position.x = 0;

	// Draw the transparent box
	DrawHalfTransparentRectTo(out, infoBoxRect.position.x, infoBoxRect.position.y, infoBoxRect.size.width, infoBoxRect.size.height);
	DrawHalfTransparentRectTo(out, infoBoxRect.position.x, infoBoxRect.position.y, infoBoxRect.size.width, infoBoxRect.size.height);

	// Adjusting the line height to add spacing between lines
	// will also add additional space beneath the last line
	// which throws off the vertical centering
	infoBoxRect.position.y += infoBoxPadding.height / 2;
	infoBoxRect.position.y += spacing / 2;

	// FORMAT AND DISPLAY ITEM TEXT OVER TRANSPARENT BOX
	std::string_view linesBaseView(linesBase);

	DrawStringWithColors(
	    out,
	    linesBaseView,
	    linesWithColor,
	    infoBoxRect,
	    { .flags = UiFlags::AlignCenter, .lineHeight = lineHeight });
}

} // namespace devilution
