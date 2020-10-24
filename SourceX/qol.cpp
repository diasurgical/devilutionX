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

void DrawMonsterHealthBar()
{
	if (!drawHPBar)
		return;
	if (currlevel == 0)
		return;
	if (pcursmonst == -1)
		return;
	MonsterStruct *mon = &monster[pcursmonst];
	bool specialMonster = mon->_uniqtype != 0;
	int currentLife = mon->_mhitpoints;
	int maxLife = mon->_mmaxhp;

	if (currentLife > maxLife)
		maxLife = currentLife;

	float FilledPercent = (float)currentLife / (float)maxLife;
	const int yPos = 180;
	const int width = 250;
	const int xPos = (SCREEN_WIDTH) / 2 - BORDER_LEFT;
	const int height = 25;
	const int xOffset = 0;
	const int yOffset = 1;
	int borderWidth = 2;
	if (specialMonster)
		borderWidth = 2;
	int borderColors[] = { 242 /*undead*/, 232 /*demon*/, 182 /*beast*/ };
	int borderColor = borderColors[(BYTE)mon->MData->mMonstClass]; //200; // pure golden, unique item style
	int filledColor = 142;                                          // optimum balance in bright red between dark and light
	bool fillCorners = true;
	int square = 10;
	char *immuText = "IMMU: ";
	char *resText = "RES: ";
	char *vulnText = ":VULN";
	int resSize = 3;
	int resistColors[] = { 148, 140, 129 }; // { 170,140,129,148,242 };// {168, 216, 200, 242, 142 }; // arcane // fire // lightning // acid
	WORD immunes[] = { IMUNE_MAGIC, IMUNE_FIRE, IMUNE_LIGHTNING };
	WORD resists[] = { RESIST_MAGIC, RESIST_FIRE, RESIST_LIGHTNING };
	WORD mres = mon->mMagicRes;

	int resOffset = 0 + GetTextWidth(resText);
	for (int k = 0; k < resSize; ++k) {
		if (!(mres & resists[k]))
			continue;
		for (int j = 0; j < square; j++) {
			for (int i = 0; i < square; i++) {
				ENG_set_pixel(xPos + i + resOffset, yPos + height + j + yOffset + borderWidth + 2, resistColors[k]);
			}
		}
		resOffset += square + 2;
	}

	int vulOffset = width - square - GetTextWidth(vulnText) - 4;
	for (int k = 0; k < resSize; ++k) {
		if (mres & resists[k] || mres & immunes[k])
			continue;
		for (int j = 0; j < square; j++) {
			for (int i = 0; i < square; i++) {
				ENG_set_pixel(xPos + i + vulOffset, yPos + height + j + yOffset + borderWidth + 2, resistColors[k]);
			}
		}
		vulOffset -= square + 2;
	}

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < (width * FilledPercent); i++) {
			int tmpColor = filledColor;
			ENG_set_pixel(xPos + i, yPos + j, tmpColor);
		}
	}

	for (int j = 0; j < borderWidth; j++) {
		for (int i = -xOffset - (fillCorners ? borderWidth : 0); i < width + xOffset + (fillCorners ? borderWidth : 0); i++) {
			ENG_set_pixel(xPos + i, yPos + j - yOffset - borderWidth, borderColor);
		}
		for (int i = -xOffset; i < width + xOffset + (fillCorners ? borderWidth : 0); i++) {
			ENG_set_pixel(xPos + i, yPos + j + yOffset + height, borderColor);
		}
	}

	for (int j = -yOffset; j < height + yOffset + (fillCorners ? borderWidth : 0); j++) {
		for (int i = 0; i < borderWidth; i++) {
			ENG_set_pixel(xPos + i - xOffset - borderWidth, yPos + j, borderColor);
			ENG_set_pixel(xPos + i + xOffset + width, yPos + j, borderColor);
		}
	}

	bool drawImmu = false;
	int immuOffset = 0 + GetTextWidth(immuText) - 5;
	for (int k = 0; k < resSize; ++k) {
		if (mres & immunes[k]) {
			drawImmu = true;
			for (int j = 0; j < square; j++) {
				for (int i = 0; i < square; i++) {
					ENG_set_pixel(xPos + i + immuOffset, yPos + height + j + yOffset + borderWidth + 2 - 15, resistColors[k]);
				}
			}
			immuOffset += square + 2;
		}
	}

	int newX = xPos + BORDER_LEFT;
	//int newY = yPos + height - 3;

	char text[166];
	strcpy(text, mon->mName);
	if (mon->leader > 0)
		strcat(text, " (minion)");

	int namecolor = COL_WHITE;
	if (specialMonster)
		namecolor = COL_GOLD;
	PrintGameStr(newX - GetTextWidth(text) / 2, 30, text, namecolor);
	PrintGameStr(newX - GetTextWidth("/") / 2, 43, "/", COL_WHITE);

	sprintf(text, "%d", (maxLife >> 6));
	PrintGameStr(newX + GetTextWidth("/"), 43, text, COL_WHITE);

	sprintf(text, "%d", (currentLife >> 6));
	PrintGameStr(newX - GetTextWidth(text) - GetTextWidth("/"), 43, text, COL_WHITE);

	PrintGameStr(newX - width / 2, 59, resText, COL_GOLD);

	sprintf(text, "kills: %d", monstkills[mon->MType->mtype]);
	PrintGameStr(newX - GetTextWidth("kills:") / 2 - 30, 59, text, COL_WHITE);

	if (drawImmu)
		PrintGameStr(newX - width / 2, 46, immuText, COL_GOLD);

	PrintGameStr(newX + width / 2 - GetTextWidth(vulnText), 59, vulnText, COL_RED);
}

void DrawXPBar()
{
	if (!drawXPBar)
		return;
	int barSize = 306; // *ScreenWidth / 640;
	int offset = 3;
	int barRows = 3; // *ScreenHeight / 480;
	int dividerHeight = 3;
	int numDividers = 10;
	int barColor = 242; /*242white, 142red, 200yellow, 182blue*/
	int emptyBarColor = 0;
	int frameColor = 242;
	int yPos = BORDER_TOP + SCREEN_HEIGHT - 8;

	PrintGameStr(145 + (SCREEN_WIDTH - 640) / 2, SCREEN_HEIGHT - 4, "XP", COL_WHITE);
	int charLevel = plr[myplr]._pLevel;
	if (charLevel != MAXCHARLEVEL - 1) {
		int curXp = ExpLvlsTbl[charLevel];
		int prevXp = ExpLvlsTbl[charLevel - 1];
		int prevXpDelta = curXp - prevXp;
		int prevXpDelta_1 = plr[myplr]._pExperience - prevXp;
		if (plr[myplr]._pExperience >= prevXp) {
			int visibleBar = barSize * (unsigned __int64)prevXpDelta_1 / prevXpDelta;

			for (int i = 0; i < visibleBar; ++i) {
				for (int j = 0; j < barRows; ++j) {
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + i + offset, yPos - barRows / 2 + j, barColor);
				}
			}

			for (int i = visibleBar; i < barSize; ++i) {
				for (int j = 0; j < barRows; ++j) {
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + i + offset, yPos - barRows / 2 + j, emptyBarColor);
				}
			}
			//draw frame
			//horizontal
			for (int i = -1; i <= barSize; ++i) {
				ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + i + offset, yPos - barRows / 2 - 1, frameColor);
				ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + i + offset, yPos + barRows / 2 + 1, frameColor);
			}
			//vertical
			for (int i = -dividerHeight; i < barRows + dividerHeight; ++i) {
				if (i >= 0 && i < barRows) {
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 - 1 + offset, yPos - barRows / 2 + i, frameColor);
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 + barSize + offset, yPos - barRows / 2 + i, frameColor);
				}
				for (int j = 1; j < numDividers; ++j) {
					ENG_set_pixel((BUFFER_WIDTH - barSize) / 2 - 1 + offset + (barSize * j / numDividers), yPos - barRows / 2 + i, frameColor);
				}
			}
			//draw frame
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
	int distToCenterX = abs(SCREEN_WIDTH / 2 - x);
	int distToCenterY = abs(SCREEN_HEIGHT / 2 - y);
	if (x <= SCREEN_WIDTH / 2) {
		x = SCREEN_WIDTH / 2 - distToCenterX * 2;
	} else {
		x = SCREEN_WIDTH / 2 + distToCenterX * 2;
	}

	if (y <= SCREEN_HEIGHT / 2) {
		y = SCREEN_HEIGHT / 2 - distToCenterY * 2;
	} else {
		y = SCREEN_HEIGHT / 2 + distToCenterY * 2;
	}
	x += SCREEN_WIDTH / 2 - 20;
	y += SCREEN_HEIGHT / 2 - 175;
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

	int centerXOffset = GetTextWidth((char *)textOnGround);

	adjustCoordsToZoom(x, y);
	x -= centerXOffset / 2 + 20;
	y -= 193;
	char clr = COL_WHITE;
	if (it->_iMagical == ITEM_QUALITY_MAGIC)
		clr = COL_BLUE;
	if (it->_iMagical == ITEM_QUALITY_UNIQUE)
		clr = COL_GOLD;
	drawQ.push_back(drawingQueue(x, y, GetTextWidth((char *)textOnGround), 13, it->_ix, it->_iy, id, clr, textOnGround));
}

void DrawBackground(int xPos, int yPos, int width, int height, int borderX, int borderY, BYTE backgroundColor, BYTE borderColor)
{
	xPos += BORDER_LEFT;
	yPos += BORDER_TOP;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (x < borderX || x + borderX >= width || y < borderY || y + borderY >= height)
				continue;
			int val = ((yPos - height) + y) * BUFFER_WIDTH + xPos + x;
			gpBuffer[val] = backgroundColor;
		}
	}
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

		int sx = t.x;
		int sy = t.y;

		int sx2 = sx;
		int sy2 = sy + 1;

		if (sx < 0 || sx >= SCREEN_WIDTH || sy < 0 || sy >= SCREEN_HEIGHT) {
			continue;
		}
		if (sx2 < 0 || sx2 >= SCREEN_WIDTH || sy2 < 0 || sy2 >= SCREEN_HEIGHT) {
			continue;
		}

		int bgcolor = 0;
		int CursorX = MouseX;
		int CursorY = MouseY + t.height;

		if (CursorX >= sx && CursorX <= sx + t.width + 1 && CursorY >= sy && CursorY <= sy + t.height) {
			if ((invflag || sbookflag) && MouseX > RIGHT_PANEL && MouseY <= SPANEL_HEIGHT) {
			} else if ((chrflag || questlog) && MouseX < SPANEL_WIDTH && MouseY <= SPANEL_HEIGHT) {
			} else if (MouseY >= PANEL_TOP && MouseX >= PANEL_LEFT && MouseX <= PANEL_LEFT + PANEL_WIDTH) {
			} else {
				cursmx = t.Row;
				cursmy = t.Col;
				pcursitem = t.ItemID;
			}
		}
		if (pcursitem == t.ItemID)
			bgcolor = 134;
		DrawBackground(sx2, sy2, t.width + 1, t.height, 0, 0, bgcolor, bgcolor);
		PrintGameStr(sx, sy, t.text, t.color);
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
	DWORD dwLen = codec_get_encoded_len(4 * (sizeof(int) + sizeof(char)));
	BYTE *SaveBuff = DiabloAllocPtr(dwLen);
	tbuff = SaveBuff;

	for (int t = 0; t < 4; t++) {
		CopyInt(&plr[myplr]._pSplHotKey[t], tbuff);
		CopyChar(&plr[myplr]._pSplTHotKey[t], tbuff);
	}
	
	dwLen = codec_get_encoded_len(tbuff - SaveBuff);
	pfile_write_save_file("hotkeys", SaveBuff, tbuff - SaveBuff, dwLen);
	mem_free_dbg(SaveBuff);
}

void LoadHotkeys()
{
	DWORD dwLen;
	BYTE *LoadBuff;
	LoadBuff = pfile_read("hotkeys", &dwLen);
	if (LoadBuff != NULL) {
		tbuff = LoadBuff;
		for (int t = 0; t < 4; t++) {
			CopyInt(tbuff, &plr[myplr]._pSplHotKey[t]);
			CopyChar(tbuff, &plr[myplr]._pSplTHotKey[t]);
		}
		mem_free_dbg(LoadBuff);
	}
}

void RepeatClicks()
{
	switch (sgbMouseDown) {
	case 1: {
		if ((SDL_GetModState() & KMOD_SHIFT)) {
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
		if (spl != SPL_TELEKINESIS &&
			spl != SPL_RESURRECT &&
			spl != SPL_HEALOTHER &&
			spl != SPL_IDENTIFY &&
			spl != SPL_RECHARGE &&
			spl != SPL_DISARM &&
			spl != SPL_REPAIR &&
			spl != SPL_GOLEM &&
			spl != SPL_INFRA &&
			spl != SPL_TOWN
			) 
			CheckPlrSpell();
		break;
	}
	}
}

DEVILUTION_END_NAMESPACE
