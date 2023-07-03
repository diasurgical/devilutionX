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
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

enum ItemStatType {
	DAM,
	AC,
	DUR
};

enum ItemRequirementType {
	STR,
	DEX,
	MAG
};

std::map<ItemStatType, UiFlags> GetItemBonusColors(const Item &item)
{
	std::map<ItemStatType, UiFlags> itemBonusColors;

	itemBonusColors[DAM] = UiFlags::ColorWhite;
	itemBonusColors[AC] = UiFlags::ColorWhite;
	itemBonusColors[DUR] = UiFlags::ColorWhite;

	ItemData itemData = AllItemsList[item.IDidx];

	// Here we set the color for the numbers that follow Armor:, Damage:, and Durability:
	if (item._iMagical == ITEM_QUALITY_UNIQUE) {
		const UniqueItem &uitem = UniqueItems[item._iUid];
		assert(uitem.UINumPL <= sizeof(uitem.powers) / sizeof(*uitem.powers));
		for (const auto &power : uitem.powers) {
			switch (power.type) {
			case IPL_ACP:
				itemBonusColors[AC] = UiFlags::ColorBlue;
				break;
			case IPL_ACP_CURSE:
				itemBonusColors[AC] = UiFlags::ColorRed;
				break;
			case IPL_SETAC:
				if (item._iAC > itemData.iMaxAC)
					itemBonusColors[AC] = UiFlags::ColorBlue;
				else if (item._iAC < itemData.iMinAC)
					itemBonusColors[AC] = UiFlags::ColorRed;
				else
					itemBonusColors[AC] = UiFlags::ColorWhite;
				break;
			case IPL_DAMMOD:
			case IPL_DAMP:
			case IPL_TOHIT_DAMP:
				itemBonusColors[DAM] = UiFlags::ColorBlue;
				break;
			case IPL_DAMP_CURSE:
			case IPL_TOHIT_DAMP_CURSE:
				itemBonusColors[DAM] = UiFlags::ColorRed;
				break;
			case IPL_SETDAM:
				if ((item._iMinDam > itemData.iMinDam) || (item._iMaxDam > itemData.iMaxDam))
					itemBonusColors[DAM] = UiFlags::ColorBlue;
				else if ((item._iMinDam < itemData.iMinDam) || (item._iMaxDam < itemData.iMaxDam))
					itemBonusColors[DAM] = UiFlags::ColorRed;
				break;
			case IPL_DUR:
				itemBonusColors[DUR] = UiFlags::ColorBlue;
				break;
			case IPL_DUR_CURSE:
				itemBonusColors[DUR] = UiFlags::ColorRed;
				break;
			case IPL_SETDUR:
				if (item._iMaxDur > itemData.iDurability)
					itemBonusColors[DUR] = UiFlags::ColorBlue;
				else if (item._iMaxDur < itemData.iDurability)
					itemBonusColors[DUR] = UiFlags::ColorRed;
				break;
			default:
				break;
			}
		}
	} else if (item._iMagical == ITEM_QUALITY_MAGIC) {
		switch (item._iPrePower) {
		case IPL_ACP:
			itemBonusColors[AC] = UiFlags::ColorBlue;
			break;
		case IPL_DAMP:
		case IPL_TOHIT_DAMP:
			itemBonusColors[DAM] = UiFlags::ColorBlue;
			break;
		case IPL_ACP_CURSE:
			itemBonusColors[AC] = UiFlags::ColorRed;
			break;
		case IPL_DAMP_CURSE:
		case IPL_TOHIT_DAMP_CURSE:
			itemBonusColors[DAM] = UiFlags::ColorRed;
			break;
		default:
			break;
		}

		switch (item._iSufPower) {
		case IPL_DAMMOD:
			itemBonusColors[DAM] = UiFlags::ColorBlue;
			break;
		case IPL_DUR:
			itemBonusColors[DUR] = UiFlags::ColorBlue;
			break;
		case IPL_DUR_CURSE:
			itemBonusColors[DUR] = UiFlags::ColorRed;
			break;
		default:
			break;
		}
	}

	return itemBonusColors;
}

std::map<ItemRequirementType, UiFlags> GetItemRequirementColors(const Item &item)
{
	Player &myPlayer = *MyPlayer;

	std::map<ItemRequirementType, UiFlags> itemRequirementColors;

	itemRequirementColors[STR] = UiFlags::ColorWhite;
	itemRequirementColors[DEX] = UiFlags::ColorWhite;
	itemRequirementColors[MAG] = UiFlags::ColorWhite;

	// Set color to red for requirement strings if player doesn't have enough
	if (item._iMinStr > myPlayer._pStrength)
		itemRequirementColors[STR] = UiFlags::ColorRed;
	if (item._iMinDex > myPlayer._pDexterity)
		itemRequirementColors[DEX] = UiFlags::ColorRed;
	if (item._iMinMag > myPlayer._pMagic)
		itemRequirementColors[MAG] = UiFlags::ColorRed;

	return itemRequirementColors;
}

void DrawFloatingInfoBox(const Surface &out, Point position)
{
	if (InfoString.empty())
		return;

	Player &myPlayer = *MyPlayer;

	// CONSTRUCT A VECTOR THAT CONTAINS ALL ITEM TEXT ALONG WITH COLORS

	Item &item = GetInventoryItem(myPlayer, pcursinvitem);

	std::map<ItemStatType, UiFlags> itemBonusColors = GetItemBonusColors(item);

	std::map<ItemRequirementType, UiFlags> itemRequirementColors = GetItemRequirementColors(item);

	// Add the lines vector contents into the linesWithColor vector, which contains both the strings and the colors that they will be
	std::vector<DrawStringFormatArg> linesWithColor;

	int16_t modifiedVals[3] = {
		item._iMinDam,
		item._iMaxDam,
		item._iAC,
	};

	if (item._iMagical == ITEM_QUALITY_UNIQUE) {
		const UniqueItem &uitem = UniqueItems[item._iUid];
		assert(uitem.UINumPL <= sizeof(uitem.powers) / sizeof(*uitem.powers));
		for (const auto &power : uitem.powers) {
			switch (power.type) {
			case IPL_ACP:
			case IPL_ACP_CURSE:
				modifiedVals[MIV_AC] *= item._iPLAC;
				modifiedVals[MIV_AC] /= 100;
				modifiedVals[MIV_AC] += item._iAC;
				break;
			case IPL_DAMP:
			case IPL_DAMP_CURSE:
			case IPL_TOHIT_DAMP:
			case IPL_TOHIT_DAMP_CURSE:
				modifiedVals[MIV_MINDAM] *= item._iPLDam;
				modifiedVals[MIV_MINDAM] /= 100;
				modifiedVals[MIV_MINDAM] += item._iMinDam;

				modifiedVals[MIV_MAXDAM] *= item._iPLDam;
				modifiedVals[MIV_MAXDAM] /= 100;
				modifiedVals[MIV_MAXDAM] += item._iMaxDam;
				break;
			case IPL_DAMMOD:
				modifiedVals[MIV_MINDAM] += item._iPLDamMod;
				modifiedVals[MIV_MAXDAM] += item._iPLDamMod;
				break;
			default:
				break;
			}
		}
	} else if (item._iMagical == ITEM_QUALITY_MAGIC) {
		switch (item._iPrePower) {
		case IPL_ACP:
		case IPL_ACP_CURSE:
			modifiedVals[MIV_AC] *= item._iPLAC;
			modifiedVals[MIV_AC] /= 100;
			modifiedVals[MIV_AC] += item._iAC;
			break;
		case IPL_DAMP:
		case IPL_DAMP_CURSE:
		case IPL_TOHIT_DAMP:
		case IPL_TOHIT_DAMP_CURSE:
			modifiedVals[MIV_MINDAM] *= item._iPLDam;
			modifiedVals[MIV_MINDAM] /= 100;
			modifiedVals[MIV_MINDAM] += item._iMinDam;

			modifiedVals[MIV_MAXDAM] *= item._iPLDam;
			modifiedVals[MIV_MAXDAM] /= 100;
			modifiedVals[MIV_MAXDAM] += item._iMaxDam;
			break;
		default:
			break;
		}

		switch (item._iSufPower) {
		case IPL_DAMMOD:
			modifiedVals[MIV_MINDAM] += item._iPLDamMod;
			modifiedVals[MIV_MAXDAM] += item._iPLDamMod;
			break;
		default:
			break;
		}
	}

	// Add Item Name
	if (item._iIdentified && item._iMagical != ITEM_QUALITY_NORMAL) {
		linesWithColor.emplace_back(item._iIName, item.getTextColor());
	}
	linesWithColor.emplace_back(item._iName, item.getTextColor());

	// Add Item Damage
	std::string formattedDam;
	if (item._iMinDam > 0 && item._iMaxDam > 0) {
		linesWithColor.emplace_back(_("Damage:"), UiFlags::ColorWhite);
		if (item._iMinDam == item._iMaxDam) {
			formattedDam = fmt::format(fmt::runtime(_("{:d}")), modifiedVals[MIV_MINDAM]);
			linesWithColor.emplace_back(formattedDam, itemBonusColors[DAM]);
		} else {
			formattedDam = fmt::format(fmt::runtime(_("{:d} to {:d}")), modifiedVals[MIV_MINDAM], modifiedVals[MIV_MAXDAM]);
			linesWithColor.emplace_back(formattedDam, itemBonusColors[DAM]);
		}
	}

	// Add Item Armor
	if (item._iAC > 0) {
		linesWithColor.emplace_back(_("Armor:"), UiFlags::ColorWhite);
		linesWithColor.emplace_back((_("{:d}"), modifiedVals[MIV_AC]), itemBonusColors[AC]);
	}

	// Add Item Durability
	std::string formattedDur;
	if (item._iMaxDur != DUR_INDESTRUCTIBLE && (item._iClass == ICLASS_WEAPON || item._iClass == ICLASS_ARMOR)) {
		linesWithColor.emplace_back(_("Durability:"), UiFlags::ColorWhite);
		formattedDur = fmt::format(fmt::runtime(_("{:d} of {:d}")), item._iDurability, item._iMaxDur);
		linesWithColor.emplace_back(formattedDur, itemBonusColors[DUR]);
	}

	// Add Item Requirements
	if (item._iMinStr > 0) {
		linesWithColor.emplace_back(_("Required Strength:"), itemRequirementColors[STR]);
		linesWithColor.emplace_back((_("{:d}"), item._iMinStr), itemRequirementColors[STR]);
	}
	if (item._iMinDex > 0) {
		linesWithColor.emplace_back(_("Required Dexterity:"), itemRequirementColors[DEX]);
		linesWithColor.emplace_back((_("{:d}"), item._iMinDex), itemRequirementColors[DEX]);
	}
	if (item._iMinMag > 0) {
		linesWithColor.emplace_back(_("Required Magic:"), itemRequirementColors[MAG]);
		linesWithColor.emplace_back((_("{:d}"), item._iMinMag), itemRequirementColors[MAG]);
	}

	// Add Item Identification Status
	if (!item._iIdentified && item._iMagical) {
		linesWithColor.emplace_back(_("Not Identified"), UiFlags::ColorWhite);
	}

	// Add Item Charges
	std::string formattedCharges;
	if (item._iMaxCharges > 0) {
		const char *spellName = GetSpellData(item._iSpell).sNameText;
		formattedCharges = fmt::format("{:s} ({:d}/{:d} Charges)", spellName, item._iCharges, item._iMaxCharges);
		linesWithColor.emplace_back(formattedCharges, UiFlags::ColorBlue);
	}

	// Add Item Bonuses
	StringOrView prePower;
	if (item._iPrePower != -1 && item._iIdentified) {
		prePower = PrintItemPower(item._iPrePower, item);
		linesWithColor.emplace_back(prePower, UiFlags::ColorBlue);
	}

	StringOrView sufPower;
	if (item._iSufPower != -1 && item._iIdentified) {
		sufPower = PrintItemPower(item._iSufPower, item);
		linesWithColor.emplace_back(sufPower, UiFlags::ColorBlue);
	}

	/* if (item._iMagical == ITEM_QUALITY_UNIQUE) {
		const UniqueItem &uitem = UniqueItems[item._iUid];
		assert(uitem.UINumPL <= sizeof(uitem.powers) / sizeof(*uitem.powers));
		for (const auto &power : uitem.powers) {
			if (power.type == IPL_INVALID || power.type == IPL_INVCURS)
				break;
			linesWithColor.emplace_back(PrintItemPower(power.type, item), UiFlags::ColorBlue);
		}
	}*/

	// CONSTRUCT STRING AS A BASE FOR UTILIZING LINESWITHCOLOR DATA
	// linesBase is used to place the {} and newlines to grab the actual string data
	// Lines that contain a colon may have a different color for the second part, so two {}'s are added
	std::string linesBase;
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
		}

		previousLineHadColon = currentLineHasColon;
	}

	// CONSTRUCT AND DRAW TRANSPARENT BOX

	Rectangle infoBoxRect {
		{ 0, 0 },
		{ 0, 0 }
	};

	int totalLinesCount = 0;
	for (char c : linesBase) {
		if (c == '\n') {
			totalLinesCount++;
		}
	}
	totalLinesCount++;

	const int spacing = 4;
	// GetLineHeight always returns 12 in this function
	const int lineHeight = 12 + spacing;
	const Size infoBoxPadding = { 16, 16 };

	int maxWidth = 0;
	for (const auto &arg : linesWithColor) {
		int lineWidth = GetLineWidth(arg.GetFormatted(), GameFontTables::GameFont12, 2);
		maxWidth = std::max(maxWidth, lineWidth);
	}

	int infoBoxWidth = maxWidth + infoBoxPadding.width;

	Size infoBoxSize {
		// width of the longest line of text + horizontal padding
		infoBoxWidth,
		// the height of the number of lines with spacing + vertical padding
		(totalLinesCount * lineHeight) + infoBoxPadding.height
	};

	if (pcursinvitem != -1) {
		// Get which item the cursor is currently over
		Item &item = GetInventoryItem(myPlayer, pcursinvitem);
		// Get how many slots the item occupies horizontally and vertically
		Size itemSize = GetInventorySize(item);
		// Padding between inventory slots
		const int invSlotPadding = 1;

		infoBoxRect = {
			{ // Render box centered horiztonally
			    position.x + ((itemSize.width * (INV_SLOT_SIZE_PX + invSlotPadding)) / 2) - (infoBoxSize.width / 2),
			    // Render box on top of item
			    position.y - (itemSize.height * (INV_SLOT_SIZE_PX + invSlotPadding)) - infoBoxSize.height },
			{ infoBoxSize.width,
			    infoBoxSize.height }
		};
		// Prevent top screen clipping by displaying the box on the bottom of the item, rather than the top
		if (infoBoxRect.position.y < 0)
			infoBoxRect.position.y += (itemSize.height * (INV_SLOT_SIZE_PX + invSlotPadding)) + infoBoxSize.height;
	}

	// Prevent top screen clipping for non inventory/stash items
	if (infoBoxRect.position.y < 0)
		infoBoxRect.position.y = 0;
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

	// FORMAT AND DISPLAY ITEM TEXT
	if (pcursinvitem != -1) {
		string_view linesBaseView(linesBase);

		DrawStringWithColors(
		    out,
		    linesBaseView,
		    linesWithColor,
		    infoBoxRect,
		    UiFlags::AlignCenter,
		    2,
		    lineHeight);
	}
}

namespace {

} // namespace

void PrintFloatingItemDur(const Item &item)
{
	if (HeadlessMode)
		return;

	if (IsAnyOf(item._itype, ItemType::Ring, ItemType::Amulet))
		AddPanelString(_("Not Identified"));

	if (IsAnyOf(item._iClass, ICLASS_WEAPON, ICLASS_ARMOR)) {
		if (item._iClass == ICLASS_WEAPON) {
			if (item._iMinDam == item._iMaxDam) {
				AddPanelString(fmt::format(fmt::runtime(_("Damage: {:d}")), item._iMinDam));
			} else {
				AddPanelString(fmt::format(fmt::runtime(_("Damage: {:d} to {:d}")), item._iMinDam, item._iMaxDam));
			}
		}

		if (item._iClass == ICLASS_ARMOR) {
			AddPanelString(fmt::format(fmt::runtime(_("Armor: {:d}")), item._iAC));
		}

		if (item._iMaxDur != DUR_INDESTRUCTIBLE) {
			AddPanelString(fmt::format(fmt::runtime(_("Durability: {:d} of {:d}")), item._iDurability, item._iMaxDur));
		}

		if (item._iMagical != ITEM_QUALITY_NORMAL)
			AddPanelString(_("Not Identified"));
	}

	if (item._iMiscId == IMISC_STAFF && item._iMaxCharges != 0) {
		const char *spellName = GetSpellData(item._iSpell).sNameText;
		AddPanelString(fmt::format(fmt::runtime(_("{:s} ({:d}/{:d} Charges)")), spellName, item._iCharges, item._iMaxCharges));
	}
}

} // namespace devilution
