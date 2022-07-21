#include "itemlabels.h"

#include <string>
#include <unordered_set>
#include <vector>

#include <fmt/format.h>

#include "control.h"
#include "cursor.h"
#include "engine/point.hpp"
#include "engine/render/clx_render.hpp"
#include "gmenu.h"
#include "inv.h"
#include "itemlabels.h"
#include "options.h"
#include "qol/stash.h"
#include "utils/format_int.hpp"
#include "utils/language.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

namespace {

struct ItemLabel {
	int id, width;
	Point pos;
	std::string text;
};

std::vector<ItemLabel> labelQueue;

bool altPressed = false;
bool isLabelHighlighted = false;
std::array<std::optional<int>, ITEMTYPES> labelCenterOffsets;

const int BorderX = 4;               // minimal horizontal space between labels
const int BorderY = 2;               // minimal vertical space between labels
const int MarginX = 2;               // horizontal margins between text and edges of the label
const int MarginY = 1;               // vertical margins between text and edges of the label
const int Height = 11 + MarginY * 2; // going above 13 scatters labels of items that are next to each other

} // namespace

void ToggleItemLabelHighlight()
{
	sgOptions.Gameplay.showItemLabels.SetValue(!*sgOptions.Gameplay.showItemLabels);
}

void AltPressed(bool pressed)
{
	altPressed = pressed;
}

bool IsItemLabelHighlighted()
{
	return isLabelHighlighted;
}

bool IsHighlightingLabelsEnabled()
{
	return altPressed != *sgOptions.Gameplay.showItemLabels;
}

void AddItemToLabelQueue(int id, int x, int y)
{
	if (!IsHighlightingLabelsEnabled())
		return;
	Item &item = Items[id];

	std::string textOnGround;
	if (item._itype == ItemType::Gold) {
		textOnGround = fmt::format(fmt::runtime(_("{:s} gold")), FormatInteger(item._ivalue));
	} else {
		textOnGround = item._iIdentified ? item._iIName : item._iName;
	}

	int nameWidth = GetLineWidth(textOnGround);
	nameWidth += MarginX * 2;
	int index = ItemCAnimTbl[item._iCurs];
	if (!labelCenterOffsets[index]) {
		std::pair<int, int> itemBounds = ClxMeasureSolidHorizontalBounds((*item.AnimInfo.sprites)[item.AnimInfo.currentFrame]);
		labelCenterOffsets[index].emplace((itemBounds.first + itemBounds.second) / 2);
	}

	x += *labelCenterOffsets[index];
	y -= TILE_HEIGHT;
	if (*sgOptions.Graphics.zoom) {
		x *= 2;
		y *= 2;
	}
	x -= nameWidth / 2;
	labelQueue.push_back(ItemLabel { id, nameWidth, { x, y - Height }, textOnGround });
}

bool IsMouseOverGameArea()
{
	if ((IsRightPanelOpen()) && GetRightPanel().contains(MousePosition))
		return false;
	if ((IsLeftPanelOpen()) && GetLeftPanel().contains(MousePosition))
		return false;
	if (GetMainPanel().contains(MousePosition))
		return false;

	return true;
}

void FillRect(const Surface &out, int x, int y, int width, int height, Uint8 col)
{
	for (int j = 0; j < height; j++) {
		DrawHorizontalLine(out, { x, y + j }, width, col);
	}
}

void DrawItemNameLabels(const Surface &out)
{
	isLabelHighlighted = false;

	for (unsigned int i = 0; i < labelQueue.size(); ++i) {
		std::unordered_set<int> backtrace;

		bool canShow;
		do {
			canShow = true;
			for (unsigned int j = 0; j < i; ++j) {
				ItemLabel &a = labelQueue[i];
				ItemLabel &b = labelQueue[j];
				if (abs(b.pos.y - a.pos.y) < Height + BorderY) {
					int widthA = a.width + BorderX + MarginX * 2;
					int widthB = b.width + BorderX + MarginX * 2;
					int newpos = b.pos.x;
					if (b.pos.x >= a.pos.x && b.pos.x - a.pos.x < widthA) {
						newpos -= widthA;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = b.pos.x + widthB;
					} else if (b.pos.x < a.pos.x && a.pos.x - b.pos.x < widthB) {
						newpos += widthB;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = b.pos.x - widthA;
					} else
						continue;
					canShow = false;
					a.pos.x = newpos;
					backtrace.insert(newpos);
				}
			}
		} while (!canShow);
	}

	for (const ItemLabel &label : labelQueue) {
		Item &item = Items[label.id];

		if (MousePosition.x >= label.pos.x && MousePosition.x < label.pos.x + label.width && MousePosition.y >= label.pos.y + MarginY && MousePosition.y < label.pos.y + MarginY + Height) {
			if (!gmenu_is_active() && PauseMode == 0 && !MyPlayerIsDead && IsMouseOverGameArea() && LastMouseButtonAction == MouseActionType::None) {
				isLabelHighlighted = true;
				cursPosition = item.position;
				pcursitem = label.id;
			}
		}
		if (pcursitem == label.id)
			FillRect(out, label.pos.x, label.pos.y + MarginY, label.width, Height, PAL8_BLUE + 6);
		else
			DrawHalfTransparentRectTo(out, label.pos.x, label.pos.y + MarginY, label.width, Height);
		DrawString(out, label.text, { { label.pos.x + MarginX, label.pos.y }, { label.width, Height } }, item.getTextColor());
	}
	labelQueue.clear();
}

} // namespace devilution
