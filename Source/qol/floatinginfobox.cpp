#include "itemlabels.h"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "control.h"
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

void DrawFloatingInfoBox(const Surface &out, Point position)
{
	if (InfoString.empty())
		return;

	Player &myPlayer = *MyPlayer;

	const int space[] = { 18, 12, 6, 3, 0 };

	Rectangle infoBoxRect {
		{ 0, 0 },
		{ 0, 0 }
	};

	const int newLineCount = std::count(InfoString.str().begin(), InfoString.str().end(), '\n');
	const int spaceIndex = std::min(4, newLineCount);
	const int spacing = space[spaceIndex];
	// GetLineHeight always returns 12 in this function
	const int lineHeight = GetLineHeight(InfoString, GameFontTables::GameFont12) + spacing;
	const Size infoBoxPadding = { 48, 24 };

	Size infoBoxSize {
		// width of the longest line of text + horizontal padding
		GetMaxLineWidth(InfoString) + infoBoxPadding.width,
		// the height of the number of lines with spacing + vertical padding
		(newLineCount * lineHeight) + infoBoxPadding.height
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
	infoBoxRect.position.y += spacing / 2;

	//DrawString(out, InfoString, infoBoxRect, InfoColor | UiFlags::AlignCenter | UiFlags::VerticalCenter, 2, lineHeight);

	//std::string infoString(InfoString.str());
	//SDL_Log("%s", infoString.c_str());

	if (pcursinvitem != -1) {
		// Lines from InfoString are added into the lines vector with a newline at the end of each line, except the last line
		// Lines that contain a ":" are split into two parts, so we are able to apply a different color to each part
		// Lines that don't contain a ":" contain a single color for the entire line
		std::vector<std::string> lines;
		auto start = InfoString.str().begin();
		while (start != InfoString.str().end()) {
			auto end = std::find(start, InfoString.str().end(), '\n');
			std::string line(start, end);

			auto colon_pos = line.find(':');
			if (colon_pos != std::string::npos) {
				// Split the line based on the colon
				std::string part1 = line.substr(0, colon_pos + 1);
				std::string part2 = line.substr(colon_pos + 1);

				lines.push_back(part1);
				lines.push_back(part2);
			} else {
				lines.push_back(line);
			}

			if (end == InfoString.str().end()) {
				break;
			}
			start = end + 1;
		}

		Item &item = GetInventoryItem(myPlayer, pcursinvitem);
		ItemData iData = AllItemsList[item.IDidx];

		UiFlags armorBonus = UiFlags::ColorWhite;
		UiFlags damageBonus = UiFlags::ColorWhite;
		UiFlags durBonus = UiFlags::ColorWhite;

		// Here we set the color for the numbers that follow Armor:, Damage:, and Durability:
		if (item._iMagical == ITEM_QUALITY_UNIQUE) {
			const UniqueItem &uitem = UniqueItems[item._iUid];
			assert(uitem.UINumPL <= sizeof(uitem.powers) / sizeof(*uitem.powers));
			for (const auto &power : uitem.powers) {
				switch (power.type) {
				case IPL_ACP:
					armorBonus = UiFlags::ColorBlue;
					break;
				case IPL_ACP_CURSE:
					armorBonus = UiFlags::ColorRed;
					break;
				case IPL_SETAC:
					if (item._iAC > iData.iMaxAC)
						armorBonus = UiFlags::ColorBlue;
					else if (item._iAC < iData.iMinAC)
						armorBonus = UiFlags::ColorRed;
					else
						armorBonus = UiFlags::ColorWhite;
					break;
				case IPL_DAMMOD:
				case IPL_DAMP:
				case IPL_TOHIT_DAMP:
					damageBonus = UiFlags::ColorBlue;
					break;
				case IPL_DAMP_CURSE:
				case IPL_TOHIT_DAMP_CURSE:
					damageBonus = UiFlags::ColorRed;
					break;
				case IPL_SETDAM:
					if ((item._iMinDam > iData.iMinDam) || (item._iMaxDam > iData.iMaxDam))
						damageBonus = UiFlags::ColorBlue;
					else if ((item._iMinDam < iData.iMinDam) || (item._iMaxDam < iData.iMaxDam))
						damageBonus = UiFlags::ColorRed;
					break;
				case IPL_DUR:
					durBonus = UiFlags::ColorBlue;
					break;
				case IPL_DUR_CURSE:
					durBonus = UiFlags::ColorRed;
					break;
				case IPL_SETDUR:
					if (item._iMaxDur > iData.iDurability)
						durBonus = UiFlags::ColorBlue;
					else if (item._iMaxDur < iData.iDurability)
						durBonus = UiFlags::ColorRed;
					break;
				default:
					break;
				}
			}
		} else if (item._iMagical == ITEM_QUALITY_MAGIC) {
			switch (item._iPrePower) {
			case IPL_ACP:
				armorBonus = UiFlags::ColorBlue;
				break;
			case IPL_DAMP:
			case IPL_TOHIT_DAMP:
				damageBonus = UiFlags::ColorBlue;
				break;
			case IPL_ACP_CURSE:
				armorBonus = UiFlags::ColorRed;
				break;
			case IPL_DAMP_CURSE:
			case IPL_TOHIT_DAMP_CURSE:
				damageBonus = UiFlags::ColorRed;
				break;
			default:
				break;
			}

			switch (item._iSufPower) {
			case IPL_DAMMOD:
				damageBonus = UiFlags::ColorBlue;
				break;
			case IPL_DUR:
				durBonus = UiFlags::ColorBlue;
				break;
			case IPL_DUR_CURSE:
				durBonus = UiFlags::ColorRed;
				break;
			default:
				break;
			}
		}

		UiFlags reqStr = UiFlags::ColorWhite;
		UiFlags reqDex = UiFlags::ColorWhite;
		UiFlags reqMag = UiFlags::ColorWhite;

		// Set color to red for requirement strings if player doesn't have enough
		if (item._iMinStr > myPlayer._pStrength)
			reqStr = UiFlags::ColorRed;
		if (item._iMinDex > myPlayer._pDexterity)
			reqDex = UiFlags::ColorRed;
		if (item._iMinMag > myPlayer._pMagic)
			reqMag = UiFlags::ColorRed;

		// Add the lines vector contents into the linesWithColor vector, which contains both the strings and the colors that they will be
		std::vector<DrawStringFormatArg> linesWithColor;
		UiFlags color = UiFlags::ColorWhite;
		for (size_t i = 0; i < lines.size(); i++) {
			const auto &line = lines[i];
			// Item name and base item name should match the item quality color, normally found in InfoColor
			// Note: Split lines are both considered separate lines, even though in game they appear on the same line visually
			if (line.find(item._iIName) != std::string::npos || line.find(item._iName) != std::string::npos) {
				color = InfoColor;
			} else if (line.find("Armor:") != std::string::npos || line.find("Damage:") != std::string::npos || line.find("Durability:") != std::string::npos) {
				color = UiFlags::ColorWhite;
			} else if ((i > 0 && lines[i - 1].find("Armor:") != std::string::npos)) {
				color = armorBonus;
			} else if ((i > 0 && lines[i - 1].find("Damage:") != std::string::npos)) {
				color = damageBonus;
			} else if ((i > 0 && lines[i - 1].find("Durability:") != std::string::npos)) {
				color = durBonus;
			} else if ((line.find("Required Strength:") != std::string::npos) || (i > 0 && lines[i - 1].find("Required Strength:") != std::string::npos)) {
				color = reqStr;
			} else if ((line.find("Required Dexterity:") != std::string::npos) || (i > 0 && lines[i - 1].find("Required Dexterity:") != std::string::npos)) {
				color = reqDex;
			} else if ((line.find("Required Magic:") != std::string::npos) || (i > 0 && lines[i - 1].find("Required Magic:") != std::string::npos)) {
				color = reqMag;
			} else if (item._iMagical != ITEM_QUALITY_NORMAL) {
				// All remaining lines for non-normal items will be powers, so set all to blue
				color = UiFlags::ColorBlue;
			}

			linesWithColor.emplace_back(line, color);
		}

		// linesBase is used to place the {} and newlines to grab the actual string data
		// Lines that contain a colon may have a different color for the second part, so two {}'s are added
		std::string linesBase;
		bool previousLineHadColon = false;
		for (size_t i = 0; i < lines.size(); i++) {
			bool currentLineHasColon = (lines[i].find(":") != std::string::npos);

			if (!previousLineHadColon && (!currentLineHasColon || (i > 0 && currentLineHasColon))) {
				if (currentLineHasColon) {
					linesBase.append("{}{}");
				} else {
					linesBase.append("{}");
				}
				if (i != lines.size() - 1) {
					linesBase.append("\n");
				}
			}

			previousLineHadColon = currentLineHasColon;
		}

		std::string_view linesBaseView(linesBase.data(), linesBase.length());

		DrawStringWithColors(
		    out,
		    linesBase,
		    linesWithColor,
		    infoBoxRect,
		    UiFlags::AlignCenter | UiFlags::VerticalCenter,
		    2,
		    lineHeight);
	}
}

namespace {

} // namespace
} // namespace devilution
