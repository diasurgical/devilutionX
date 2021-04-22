/**
* @file itemlabels.cpp
*
* Adds Item Labels QoL feature
*/

#include "common.h"
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

struct itemLabel {
	int itemID, row, col, x, y, width, height;
	text_color color;
	std::string text;
};

enum class State {
	NOT_STARTED = 0,
	STARTED,
	FINISHED,
};

State generatingLabelsState;
std::vector<itemLabel> labelQueue;

int drawMinX;
int drawMaxX;
bool altPressed = false;
bool isLabelHighlighted = false;
int labelCenterOffsets[ITEMTYPES];
bool invertHighlightToggle = false;

} // namespace

extern BYTE ItemAnimLs[];
extern BYTE *itemanims[ITEMTYPES];

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
	return generatingLabelsState == State::STARTED;
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
	if (generatingLabelsState == State::FINISHED)
		return;
	generatingLabelsState = State::STARTED;
	int itemTypes = gbIsHellfire ? ITEMTYPES : 35;
	for (int i = 0; i < itemTypes; i++) {
		drawMinX = gnScreenWidth;
		drawMaxX = 0;
		CelClippedDrawTo(out, out.pitch() / 2 - 16, 351, itemanims[i], ItemAnimLs[i], 96);
		labelCenterOffsets[i] = drawMinX - out.pitch() / 2 + (drawMaxX - drawMinX) / 2;
	}
	generatingLabelsState = State::FINISHED;
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
	labelQueue.push_back(itemLabel{ id, it->_ix, it->_iy, x, y, nameWidth, 13, GetItemTextColor(*it, false), textOnGround });
}

void DrawItemNameLabels(const CelOutputBuffer &out)
{
	isLabelHighlighted = false;
	const int borderX = 5;
	for (unsigned int i = 0; i < labelQueue.size(); ++i) {
		std::unordered_set<int> backtrace;

		bool canShow;
		do {
			canShow = true;
			for (unsigned int j = 0; j < i; ++j) {
				itemLabel &a = labelQueue[i], &b = labelQueue[j];
				if (abs(b.y - a.y) < a.height + 2) {
					int newpos = b.x;
					if (b.x >= a.x && b.x - a.x < a.width + borderX) {
						newpos -= a.width + borderX;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = b.x + b.width + borderX;
					} else if (b.x < a.x && a.x - b.x < b.width + borderX) {
						newpos += b.width + borderX;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = b.x - a.width - borderX;
					} else
						continue;
					canShow = false;
					a.x = newpos;
					backtrace.insert(newpos);
				}
			}
		} while (!canShow);
	}

	for (const itemLabel &t : labelQueue) {

		if(t.x < 0 || t.x >= out.w() || t.y < 0 || t.y >= out.h())
			continue;

		if (MouseX >= t.x && MouseX <= t.x + t.width && MouseY >= t.y - t.height && MouseY <= t.y) {
			if (!gmenu_is_active() && PauseMode == 0 && !deathflag && IsMouseOverGameArea()) {
				isLabelHighlighted = true;
				cursmx = t.row;
				cursmy = t.col;
				pcursitem = t.itemID;
			}
		}
		if (pcursitem == t.itemID) 
			FillRect(out, t.x, t.y - t.height, t.width + 1, t.height, PAL8_BLUE + 6);
		else
			DrawHalfTransparentRectTo(out, t.x, t.y - t.height, t.width + 1, t.height);
		PrintGameStr(out, t.x, t.y - 1, t.text.c_str(), t.color);
	}
	labelQueue.clear();
}

} // namespace devilution
