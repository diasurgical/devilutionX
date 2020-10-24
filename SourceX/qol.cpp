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

DEVILUTION_END_NAMESPACE
