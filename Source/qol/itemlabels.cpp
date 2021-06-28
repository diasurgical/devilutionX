#include "itemlabels.h"

#include <string>
#include <unordered_set>
#include <vector>

#include "common.h"
#include "control.h"
#include "cursor.h"
#include "engine/point.hpp"
#include "engine/render/cel_render.hpp"
#include "gmenu.h"
#include "inv.h"
#include "itemlabels.h"
#include "utils/language.h"

namespace devilution {

namespace {

struct itemLabel {
	int id, width;
	Point pos;
	std::string text;
};

std::vector<itemLabel> labelQueue;

bool altPressed = false;
bool isLabelHighlighted = false;
std::array<std::optional<int>, ITEMTYPES> labelCenterOffsets;
bool invertHighlightToggle = false;

const int borderX = 4;               // minimal horizontal space between labels
const int borderY = 2;               // minimal vertical space between labels
const int marginX = 2;               // horizontal margins between text and edges of the label
const int marginY = 1;               // vertical margins between text and edges of the label
const int height = 11 + marginY * 2; // going above 13 scatters labels of items that are next to each other

} // namespace

void ToggleItemLabelHighlight()
{
	invertHighlightToggle = !invertHighlightToggle;
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
	return altPressed != invertHighlightToggle;
}

void AddItemToLabelQueue(int id, int x, int y)
{
	if (!IsHighlightingLabelsEnabled())
		return;
	ItemStruct *it = &items[id];

	const char *textOnGround;
	if (it->_itype == ITYPE_GOLD) {
		std::sprintf(tempstr, _("%i gold"), it->_ivalue);
		textOnGround = tempstr;
	} else {
		textOnGround = it->_iIdentified ? it->_iIName : it->_iName;
	}

	int nameWidth = GetLineWidth(textOnGround);
	nameWidth += marginX * 2;
	int index = ItemCAnimTbl[it->_iCurs];
	if (!labelCenterOffsets[index]) {
		std::pair<int, int> itemBounds = MeasureSolidHorizontalBounds(*it->AnimInfo.pCelSprite, it->AnimInfo.CurrentFrame);
		labelCenterOffsets[index].emplace((itemBounds.first + itemBounds.second) / 2);
	}

	x += *labelCenterOffsets[index];
	y -= TILE_HEIGHT;
	if (!zoomflag) {
		x *= 2;
		y *= 2;
	}
	x -= nameWidth / 2;
	labelQueue.push_back(itemLabel { id, nameWidth, { x, y }, textOnGround });
}

bool IsMouseOverGameArea()
{
	if ((invflag || sbookflag) && MousePosition.x > RIGHT_PANEL && MousePosition.y <= SPANEL_HEIGHT)
		return false;
	if ((chrflag || questlog) && MousePosition.x < SPANEL_WIDTH && MousePosition.y <= SPANEL_HEIGHT)
		return false;
	if (MousePosition.y >= PANEL_TOP && MousePosition.x >= PANEL_LEFT && MousePosition.x <= PANEL_LEFT + PANEL_WIDTH)
		return false;

	return true;
}

void FillRect(const CelOutputBuffer &out, int x, int y, int width, int height, Uint8 col)
{
	for (int j = 0; j < height; j++) {
		DrawHorizontalLine(out, { x, y + j }, width, col);
	}
}

void DrawItemNameLabels(const CelOutputBuffer &out)
{
	isLabelHighlighted = false;

	for (unsigned int i = 0; i < labelQueue.size(); ++i) {
		std::unordered_set<int> backtrace;

		bool canShow;
		do {
			canShow = true;
			for (unsigned int j = 0; j < i; ++j) {
				itemLabel &a = labelQueue[i];
				itemLabel &b = labelQueue[j];
				if (abs(b.pos.y - a.pos.y) < height + borderY) {
					int widthA = a.width + borderX + marginX * 2;
					int widthB = b.width + borderX + marginX * 2;
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

	for (const itemLabel &label : labelQueue) {
		ItemStruct &itm = items[label.id];

		if (MousePosition.x >= label.pos.x && MousePosition.x < label.pos.x + label.width && MousePosition.y >= label.pos.y - height + marginY && MousePosition.y < label.pos.y + marginY) {
			if (!gmenu_is_active() && PauseMode == 0 && !deathflag && IsMouseOverGameArea()) {
				isLabelHighlighted = true;
				cursPosition = itm.position;
				pcursitem = label.id;
			}
		}
		if (pcursitem == label.id)
			FillRect(out, label.pos.x, label.pos.y - height + marginY, label.width, height, PAL8_BLUE + 6);
		else
			DrawHalfTransparentRectTo(out, label.pos.x, label.pos.y - height + marginY, label.width, height);
		DrawString(out, label.text.c_str(), { label.pos.x + marginX, label.pos.y, label.width, height }, itm.getTextColor());
	}
	labelQueue.clear();
}

} // namespace devilution
