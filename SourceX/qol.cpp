/**
 * @file qol.cpp
 *
 * Quality of life features
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

int GetTextWidth(char *s)
{
	int l = 0;
	while (*s) {
		l += fontkern[fontframe[gbFontTransTbl[(BYTE)*s++]]] + 1;
	}
	return l;
}

int GetConfigIntValue(const char *valuename, int base)
{
	if (!SRegLoadValue("devilutionx", valuename, 0, &base)) {
		SRegSaveValue("devilutionx", valuename, 0, base);
	}
	return base;
}

int highlightItemsMode = 0;
// 0 = disabled
// 1 = highlight when alt pressed
// 2 = hide when alt pressed
// 3 = always highlight
bool altPressed = false;
bool drawXPBar = false;
bool drawHPBar = false;

inline void FastDrawHorizLine(int x, int y, int width, BYTE col)
{
	memset(&gpBuffer[SCREENXY(x, y)], col, width);
}

inline void FastDrawVertLine(int x, int y, int height, BYTE col)
{
	BYTE *p = &gpBuffer[SCREENXY(x, y)];
	for (int j = 0; j < height; j++) {
		*p = col;
		p += BUFFER_WIDTH;
	}
}

inline void FillRect(int x, int y, int width, int height, BYTE col)
{
	for (int j = 0; j < height; j++) {
		FastDrawHorizLine(x, y + j, width, col);
	}
}

inline void FillSquare(int x, int y, int size, BYTE col)
{
	FillRect(x, y, size, size, col);
}

void DrawMonsterHealthBar()
{
	if (!drawHPBar)
		return;
	if (currlevel == 0)
		return;
	if (pcursmonst == -1)
		return;

	int width = 250;
	int height = 25;
	int x = 0;          // x offset from the center of the screen
	int y = 20;         // y position
	int xOffset = 0;    // empty space between left/right borders and health bar
	int yOffset = 1;    // empty space between top/bottom borders and health bar
	int borderSize = 2; // size of the border around health bar
	BYTE borderColors[] = { 242 /*undead*/, 232 /*demon*/, 182 /*beast*/ };
	BYTE filledColor = 142;                                             // filled health bar color
	bool fillCorners = false;                                           // true to fill border corners, false to cut them off
	int square = 10;                                                    // resistance / immunity / vulnerability square size
	char *immuText = "IMMU: ", *resText = "RES: ", *vulnText = ":VULN"; // text displayed for immunities / resistances / vulnerabilities
	int resSize = 3;                                                    // how many damage types
	BYTE resistColors[] = { 148, 140, 129 };                            // colors for these damage types
	WORD immunes[] = { IMMUNE_MAGIC, IMMUNE_FIRE, IMMUNE_LIGHTNING };   // immunity flags for damage types
	WORD resists[] = { RESIST_MAGIC, RESIST_FIRE, RESIST_LIGHTNING };   // resistance flags for damage types

	MonsterStruct *mon = &monster[pcursmonst];
	BYTE borderColor = borderColors[(BYTE)mon->MData->mMonstClass];
	WORD mres = mon->mMagicRes;
	bool drawImmu = false;
	int xPos = (SCREEN_WIDTH - width) / 2 + x;
	int xPos2 = xPos + width / 2;
	int yPos = y;
	int immuOffset = GetTextWidth(immuText) - 5;
	int resOffset = GetTextWidth(resText);
	int vulOffset = width - square - GetTextWidth(vulnText) - 4;
	int corners = (fillCorners ? borderSize : 0);
	int currentLife = mon->_mhitpoints, maxLife = mon->_mmaxhp;
	if (currentLife > maxLife)
		maxLife = currentLife;
	if (mon->_uniqtype != 0)
		borderSize <<= 1;

	FillRect(xPos, yPos, (width * currentLife) / maxLife, height, filledColor);
	for (int j = 0; j < borderSize; j++) {
		FastDrawHorizLine(xPos - xOffset - corners, yPos - borderSize - yOffset + j, (xOffset + corners) * 2 + width, borderColor);
		FastDrawHorizLine(xPos - xOffset, yPos + height + yOffset + j, width + corners + xOffset * 2, borderColor);
	}
	for (int j = -yOffset; j < yOffset + height + corners; ++j) {
		FastDrawHorizLine(xPos - xOffset - borderSize, yPos + j, borderSize, borderColor);
		FastDrawHorizLine(xPos + xOffset + width, yPos + j, borderSize, borderColor);
	}
	for (int k = 0; k < resSize; ++k) {
		if (mres & immunes[k]) {
			drawImmu = true;
			FillSquare(xPos + immuOffset, yPos + height - square, square, resistColors[k]);
			immuOffset += square + 2;
		} else if ((mres & resists[k])) {
			FillSquare(xPos + resOffset, yPos + yOffset + height + borderSize + 2, square, resistColors[k]);
			resOffset += square + 2;
		} else {
			FillSquare(xPos + vulOffset, yPos + yOffset + height + borderSize + 2, square, resistColors[k]);
			vulOffset -= square + 2;
		}
	}

	char text[64];
	strcpy(text, mon->mName);
	if (mon->leader > 0)
		strcat(text, " (minion)");
	PrintGameStr(xPos2 - GetTextWidth(text) / 2, yPos + 10, text, (mon->_uniqtype != 0 ? COL_GOLD : COL_WHITE));

	sprintf(text, "%d", (maxLife >> 6));
	PrintGameStr(xPos2 + GetTextWidth("/"), yPos + 23, text, COL_WHITE);

	sprintf(text, "%d", (currentLife >> 6));
	PrintGameStr(xPos2 - GetTextWidth(text) - GetTextWidth("/"), yPos + 23, text, COL_WHITE);

	PrintGameStr(xPos2 - GetTextWidth("/") / 2, yPos + 23, "/", COL_WHITE);

	sprintf(text, "kills: %d", monstkills[mon->MType->mtype]);
	PrintGameStr(xPos2 - GetTextWidth("kills:") / 2 - 30, yPos + yOffset + height + borderSize + 12, text, COL_WHITE);

	if (drawImmu)
		PrintGameStr(xPos2 - width / 2, yPos + height, immuText, COL_GOLD);

	PrintGameStr(xPos2 - width / 2, yPos + yOffset + height + borderSize + 12, resText, COL_GOLD);
	PrintGameStr(xPos2 + width / 2 - GetTextWidth(vulnText), yPos + yOffset + height + borderSize + 12, vulnText, COL_RED);
}

void DrawXPBar()
{
	if (!drawXPBar)
		return;
	int barWidth = 306;
	int barHeight = 5;
	int yPos = SCREEN_HEIGHT - 9;                 // y position of xp bar
	int xPos = (SCREEN_WIDTH - barWidth) / 2 + 5; // x position of xp bar
	int dividerHeight = 3;
	int numDividers = 10;
	int barColor = 200; /*242white, 142red, 200yellow, 182blue*/
	int emptyBarColor = 0;
	int frameColor = 242;
	bool space = true; // add 1 pixel separator on top/bottom of the bar

	PrintGameStr(xPos - 22, yPos + 6, "XP", COL_WHITE);
	int charLevel = plr[myplr]._pLevel;
	if (charLevel != MAXCHARLEVEL - 1) {
		int curXp = ExpLvlsTbl[charLevel];
		int prevXp = ExpLvlsTbl[charLevel - 1];
		int prevXpDelta = curXp - prevXp;
		int prevXpDelta_1 = plr[myplr]._pExperience - prevXp;
		if (plr[myplr]._pExperience >= prevXp) {
			int visibleBar = barWidth * (unsigned __int64)prevXpDelta_1 / prevXpDelta;
			FillRect(xPos, yPos, barWidth, barHeight, emptyBarColor);
			FillRect(xPos, yPos + (space ? 1 : 0), visibleBar, barHeight - (space ? 2 : 0), barColor);
			FastDrawHorizLine(xPos - 1, yPos - 1, barWidth + 2, frameColor);
			FastDrawHorizLine(xPos - 1, yPos + barHeight, barWidth + 2, frameColor);
			FastDrawVertLine(xPos - 1, yPos - 1, barHeight + 2, frameColor);
			FastDrawVertLine(xPos + barWidth, yPos - 1, barHeight + 2, frameColor);
			for (int i = 1; i < numDividers; i++)
				FastDrawVertLine(xPos - 1 + (barWidth * i / numDividers), yPos - dividerHeight - 1, barHeight + dividerHeight * 2 + 2, frameColor);
		}
	}
}

class drawingQueue {
public:
	int ItemID;
	int Row;
	int Col;
	int x;
	int y;
	int width;
	int height;
	int color;
	char text[64];
	drawingQueue(int x2, int y2, int width2, int height2, int Row2, int Col2, int ItemID2, int q2, char *text2)
	{
		x = x2;
		y = y2;
		Row = Row2;
		Col = Col2;
		ItemID = ItemID2;
		width = width2;
		height = height2;
		color = q2;
		strcpy(text, text2);
	}
};

std::vector<drawingQueue> drawQ;

void adjustCoordsToZoom(int &x, int &y)
{
	if (zoomflag)
		return;
	x = 2 * x;
	y = 2 * y;
	x += 60;
}

void AddItemToDrawQueue(int x, int y, int id)
{
	if (highlightItemsMode == 0 || (highlightItemsMode == 1 && !altPressed) || (highlightItemsMode == 2 && altPressed))
		return;
	ItemStruct *it = &item[id];

	char textOnGround[64];
	if (it->_itype == ITYPE_GOLD) {
		sprintf(textOnGround, "%i gold", it->_ivalue);
	} else {
		sprintf(textOnGround, "%s", it->_iIdentified ? it->_iIName : it->_iName);
	}

	int nameWidth = GetTextWidth((char *)textOnGround);
	x -= SCREEN_X;
	y -= SCREEN_Y;
	y -= 30;
	x -= nameWidth / 2 - 40;
	adjustCoordsToZoom(x, y);
	char clr = COL_WHITE;
	if (it->_iMagical == ITEM_QUALITY_MAGIC)
		clr = COL_BLUE;
	if (it->_iMagical == ITEM_QUALITY_UNIQUE)
		clr = COL_GOLD;
	drawQ.push_back(drawingQueue(x, y, nameWidth, 13, it->_ix, it->_iy, id, clr, textOnGround));
}

void HighlightItemsNameOnMap()
{
	if (highlightItemsMode == 0 || (highlightItemsMode == 1 && !altPressed) || (highlightItemsMode == 2 && altPressed))
		return;
	const int borderX = 5;
	for (unsigned int i = 0; i < drawQ.size(); ++i) {
		std::map<int, bool> backtrace;

		bool canShow;
		do {
			canShow = true;
			for (unsigned int j = 0; j < i; ++j) {
				if (abs(drawQ[j].y - drawQ[i].y) < drawQ[i].height + 2) {
					int newpos = drawQ[j].x;
					if (drawQ[j].x >= drawQ[i].x && drawQ[j].x - drawQ[i].x < drawQ[i].width + borderX) {
						newpos -= drawQ[i].width + borderX;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = drawQ[j].x + drawQ[j].width + borderX;
					} else if (drawQ[j].x < drawQ[i].x && drawQ[i].x - drawQ[j].x < drawQ[j].width + borderX) {
						newpos += drawQ[j].width + borderX;
						if (backtrace.find(newpos) != backtrace.end())
							newpos = drawQ[j].x - drawQ[i].width - borderX;
					} else
						continue;
					canShow = false;
					drawQ[i].x = newpos;
					backtrace[newpos] = true;
				}
			}
		} while (!canShow);
	}

	for (unsigned int i = 0; i < drawQ.size(); ++i) {
		drawingQueue t = drawQ[i];

		if (t.x < 0 || t.x >= SCREEN_WIDTH || t.y < 0 || t.y >= SCREEN_HEIGHT) {
			continue;
		}

		if (MouseX >= t.x && MouseX <= t.x + t.width && MouseY >= t.y - t.height && MouseY <= t.y) {
			if ((invflag || sbookflag) && MouseX > RIGHT_PANEL && MouseY <= SPANEL_HEIGHT) {
			} else if ((chrflag || questlog) && MouseX < SPANEL_WIDTH && MouseY <= SPANEL_HEIGHT) {
			} else if (MouseY >= PANEL_TOP && MouseX >= PANEL_LEFT && MouseX <= PANEL_LEFT + PANEL_WIDTH) {
			} else {
				cursmx = t.Row;
				cursmy = t.Col;
				pcursitem = t.ItemID;
			}
		}
		int bgcolor = 0;
		if (pcursitem == t.ItemID)
			bgcolor = 134;
		FillRect(t.x, t.y - t.height, t.width + 1, t.height, bgcolor);
		PrintGameStr(t.x, t.y - 1, t.text, t.color);
	}
	drawQ.clear();
}

void diablo_parse_config()
{
	drawHPBar = GetConfigIntValue("monster health bar", 0) != 0;
	drawXPBar = GetConfigIntValue("xp bar", 0) != 0;
	highlightItemsMode = GetConfigIntValue("highlight items", 0);
}

void SaveHotkeys()
{
	BYTE *oldtbuff = tbuff;
	DWORD dwLen = codec_get_encoded_len(4 * (sizeof(int) + sizeof(char)));
	BYTE *SaveBuff = DiabloAllocPtr(dwLen);
	tbuff = SaveBuff;

	CopyInts(&plr[myplr]._pSplHotKey, 4, tbuff);
	CopyBytes(&plr[myplr]._pSplTHotKey, 4, tbuff);

	dwLen = codec_get_encoded_len(tbuff - SaveBuff);
	pfile_write_save_file("hotkeys", SaveBuff, tbuff - SaveBuff, dwLen);
	mem_free_dbg(SaveBuff);
	tbuff = oldtbuff;
}

void LoadHotkeys()
{
	BYTE *oldtbuff = tbuff;
	DWORD dwLen;
	BYTE *LoadBuff = pfile_read("hotkeys", &dwLen);
	if (LoadBuff != NULL) {
		tbuff = LoadBuff;

		CopyInts(tbuff, 4, &plr[myplr]._pSplHotKey);
		CopyBytes(tbuff, 4, &plr[myplr]._pSplTHotKey);

		mem_free_dbg(LoadBuff);
		tbuff = oldtbuff;
	}
}

void RepeatClicks()
{
	switch (sgbMouseDown) {
	case 1: {
		if ((SDL_GetModState() & KMOD_SHIFT) && currlevel != 0) {
			if (plr[myplr]._pwtype == WT_RANGED) {
				NetSendCmdLoc(TRUE, CMD_RATTACKXY, cursmx, cursmy);
			} else {
				NetSendCmdLoc(TRUE, CMD_SATTACKXY, cursmx, cursmy);
			}
		} else {
			NetSendCmdLoc(TRUE, CMD_WALKXY, cursmx, cursmy);
		}
		break;
	}
	case 2: {
		/*
		repeated casting disabled for spells that change cursor and ones that wouldn't benefit from casting them more than 1 time
		it has to be done here, otherwise there is a delay between casting a spell and changing the cursor, during which more casts get queued
		*/
		int spl = plr[myplr]._pRSpell;
		if (spl != SPL_TELEKINESIS && spl != SPL_RESURRECT && spl != SPL_HEALOTHER && spl != SPL_IDENTIFY && spl != SPL_RECHARGE && spl != SPL_DISARM && spl != SPL_REPAIR && spl != SPL_GOLEM && spl != SPL_INFRA && spl != SPL_TOWN)
			CheckPlrSpell();
		break;
	}
	}
}

DEVILUTION_END_NAMESPACE
