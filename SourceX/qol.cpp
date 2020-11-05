/**
 * @file qol.cpp
 *
 * Quality of life features
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

int GetTextWidth(const char *s)
{
	int l = 0;
	while (*s) {
		l += fontkern[fontframe[gbFontTransTbl[(BYTE)*s++]]] + 1;
	}
	return l;
}

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
	if (!sgOptions.bEnemyHealthBar)
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
	BYTE filledColor = 142;                                                   // filled health bar color
	bool fillCorners = false;                                                 // true to fill border corners, false to cut them off
	int square = 10;                                                          // resistance / immunity / vulnerability square size
	const char *immuText = "IMMU: ", *resText = "RES: ", *vulnText = ":VULN"; // text displayed for immunities / resistances / vulnerabilities
	int resSize = 3;                                                          // how many damage types
	BYTE resistColors[] = { 148, 140, 129 };                                  // colors for these damage types
	WORD immunes[] = { IMMUNE_MAGIC, IMMUNE_FIRE, IMMUNE_LIGHTNING };         // immunity flags for damage types
	WORD resists[] = { RESIST_MAGIC, RESIST_FIRE, RESIST_LIGHTNING };         // resistance flags for damage types

	MonsterStruct *mon = &monster[pcursmonst];
	BYTE borderColor = borderColors[(BYTE)mon->MData->mMonstClass];
	WORD mres = mon->mMagicRes;
	bool drawImmu = false;
	int xPos = (gnScreenWidth - width) / 2 + x;
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
	CelOutputBuffer out = GlobalBackBuffer();
	PrintGameStr(out, xPos2 - GetTextWidth(text) / 2, yPos + 10, text, (mon->_uniqtype != 0 ? COL_GOLD : COL_WHITE));

	sprintf(text, "%d", (maxLife >> 6));
	PrintGameStr(out, xPos2 + GetTextWidth("/"), yPos + 23, text, COL_WHITE);

	sprintf(text, "%d", (currentLife >> 6));
	PrintGameStr(out, xPos2 - GetTextWidth(text) - GetTextWidth("/"), yPos + 23, text, COL_WHITE);

	PrintGameStr(out, xPos2 - GetTextWidth("/") / 2, yPos + 23, "/", COL_WHITE);

	sprintf(text, "kills: %d", monstkills[mon->MType->mtype]);
	PrintGameStr(out, xPos2 - GetTextWidth("kills:") / 2 - 30, yPos + yOffset + height + borderSize + 12, text, COL_WHITE);

	if (drawImmu)
		PrintGameStr(out, xPos2 - width / 2, yPos + height, immuText, COL_GOLD);

	PrintGameStr(out, xPos2 - width / 2, yPos + yOffset + height + borderSize + 12, resText, COL_GOLD);
	PrintGameStr(out, xPos2 + width / 2 - GetTextWidth(vulnText), yPos + yOffset + height + borderSize + 12, vulnText, COL_RED);
}

void DrawXPBar()
{
	if (!sgOptions.bExperienceBar)
		return;

	int barWidth = 306;
	int barHeight = 5;
	int yPos = gnScreenHeight - 9;                 // y position of xp bar
	int xPos = (gnScreenWidth - barWidth) / 2 + 5; // x position of xp bar
	int dividerHeight = 3;
	int numDividers = 10;
	int barColor = 198;
	int emptyBarColor = 0;
	int frameColor = 245;
	bool space = true; // add 1 pixel separator on top/bottom of the bar

	CelOutputBuffer out = GlobalBackBuffer();
	PrintGameStr(out, xPos - 22, yPos + 6, "XP", COL_WHITE);
	int charLevel = plr[myplr]._pLevel;
	if (charLevel == MAXCHARLEVEL - 1)
		return;

	int curXp = ExpLvlsTbl[charLevel];
	int prevXp = ExpLvlsTbl[charLevel - 1];
	int prevXpDelta = curXp - prevXp;
	int prevXpDelta_1 = plr[myplr]._pExperience - prevXp;
	if (plr[myplr]._pExperience < prevXp)
		return;

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

void AutoGoldPickup(int pnum)
{
	if (!sgOptions.bAutoGoldPickup)
		return;
	if (pnum != myplr)
		return;
	if (leveltype == DTYPE_TOWN)
		return;

	for (int dir = 0; dir < 8; dir++) {
		int x = plr[pnum]._px + pathxdir[dir];
		int y = plr[pnum]._py + pathydir[dir];
		if (dItem[x][y] != 0) {
			int itemIndex = dItem[x][y] - 1;
			if (item[itemIndex]._itype == ITYPE_GOLD) {
				NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
				item[itemIndex]._iRequest = TRUE;
			}
		}
	}
}

DEVILUTION_END_NAMESPACE
