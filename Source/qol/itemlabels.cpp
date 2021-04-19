/**
* @file itemlabels.cpp
*
* Adds Item Labels QoL feature
*/

#include "common.h"
#include "control.h"
#include "itemlabels.h"
#include "inv.h"
#include "render.h"
#include "gmenu.h"
#include "cursor.h"

#include <unordered_set>
#include <vector>
#include <string>

namespace devilution {

namespace {

struct itemLabelQueue {
	int itemID, row, col, x, y, width, height;
	text_color color;
	std::string text;
};

std::vector<itemLabelQueue> labelQ;
int drawMinX;
int drawMaxX;
bool altPressed = false;
int generatingLabelsState = 0; // 0 - waiting for the game to start, 1 = started generating, 2 = finished generating
bool isLabelHighlighted = false;
int labelCenterOffsets[ITEMTYPES];
bool invertHighlightToggle = false;

} // namespace

void ToggleItemLabelHighlight()
{
	invertHighlightToggle = !invertHighlightToggle;
}

void AltPressed(bool pressed)
{
	altPressed = pressed;
}

bool IsGeneratingItemLabels()
{
	return generatingLabelsState == 1;
}

bool IsItemLabelHighlighted()
{
	return isLabelHighlighted;
}

void UpdateItemLabelOffsets(const CelOutputBuffer &out, BYTE *dst, int width)
{
	int xval = (dst - out.begin()) % out.pitch();
	if (xval < drawMinX)
		drawMinX = xval;
	xval += width;
	if (xval > drawMaxX)
		drawMaxX = xval;
}

void GenerateItemLabelOffsets(const CelOutputBuffer &out)
{
	if (generatingLabelsState == 2)
		return;
	generatingLabelsState = 1;
	int itemTypes = gbIsHellfire ? ITEMTYPES : 35;
	for (int i = 0; i < itemTypes; i++) {
		drawMinX = gnScreenWidth;
		drawMaxX = 0;
		CelClippedDrawTo(out, out.pitch() / 2 - 16, 351, itemanims[i], ItemAnimLs[i], 96);
		labelCenterOffsets[i] = drawMinX - out.pitch() / 2 + (drawMaxX - drawMinX) / 2;
	}
	generatingLabelsState = 2;
}

void AddItemToLabelQueue(int x, int y, int id)
{
	if (altPressed == invertHighlightToggle)
		return;
	ItemStruct *it = &items[id];

	char textOnGround[64];
	if (it->_itype == ITYPE_GOLD)
		sprintf(textOnGround, "%i gold", it->_ivalue);
	else
		sprintf(textOnGround, "%s", it->_iIdentified ? it->_iIName : it->_iName);

	int nameWidth = GetTextWidth((char *)textOnGround);
	x += labelCenterOffsets[ItemCAnimTbl[it->_iCurs]];
	y -= TILE_HEIGHT;
	if (!zoomflag) {
		x <<= 1;
		y <<= 1;
	}
	x -= nameWidth >> 1;
	text_color clr = COL_WHITE;
	if (it->_iMagical == ITEM_QUALITY_MAGIC)
		clr = COL_BLUE;
	if (it->_iMagical == ITEM_QUALITY_UNIQUE)
		clr = COL_GOLD;
	labelQ.push_back(itemLabelQueue({ id, it->_ix, it->_iy, x, y, nameWidth, 13, clr, textOnGround }));
}

void DrawItemNameLabels(const CelOutputBuffer &out)
{
	isLabelHighlighted = false;
	const int borderX = 5;
	for (unsigned int i = 0; i < labelQ.size(); ++i) {
		std::unordered_set<int> backtrace;

		bool canShow;
		do {
			canShow = true;
			for (unsigned int j = 0; j < i; ++j) {
				if (abs(labelQ[j].y - labelQ[i].y) < labelQ[i].height + 2) {
					int newpos = labelQ[j].x;
					if (labelQ[j].x >= labelQ[i].x && labelQ[j].x - labelQ[i].x < labelQ[i].width + borderX) {
						newpos -= labelQ[i].width + borderX;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = labelQ[j].x + labelQ[j].width + borderX;
					} else if (labelQ[j].x < labelQ[i].x && labelQ[i].x - labelQ[j].x < labelQ[j].width + borderX) {
						newpos += labelQ[j].width + borderX;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = labelQ[j].x - labelQ[i].width - borderX;
					} else
						continue;
					canShow = false;
					labelQ[i].x = newpos;
					backtrace.insert(newpos);
				}
			}
		} while (!canShow);
	}

	for (const itemLabelQueue &t : labelQ) {

		if (t.x < 0 || t.x >= gnScreenWidth || t.y < 0 || t.y >= gnScreenHeight)
			continue;

		if (MouseX >= t.x && MouseX <= t.x + t.width && MouseY >= t.y - t.height && MouseY <= t.y) {
			if ((invflag || sbookflag) && MouseX > RIGHT_PANEL && MouseY <= SPANEL_HEIGHT) {
			} else if ((chrflag || questlog) && MouseX < SPANEL_WIDTH && MouseY <= SPANEL_HEIGHT) {
			} else if (MouseY >= PANEL_TOP && MouseX >= PANEL_LEFT && MouseX <= PANEL_LEFT + PANEL_WIDTH) {
			} else if (gmenu_is_active() || PauseMode != 0 || deathflag) {
			} else {
				isLabelHighlighted = true;
				cursmx = t.row;
				cursmy = t.col;
				pcursitem = t.itemID;
			}
		}
		int bgcolor = 0;
		if (pcursitem == t.itemID)
			bgcolor = 134;
		FillRect(out, t.x, t.y - t.height, t.width + 1, t.height, bgcolor);
		PrintGameStr(out, t.x, t.y - 1, t.text.c_str(), t.color);
	}
	labelQ.clear();
}

} // namespace devilution
