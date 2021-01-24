/**
 * @file minitext.cpp
 *
 * Implementation of scrolling dialog text.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int qtexty;
const char *qtextptr;
int qtextSpd;
BOOLEAN qtextflag;
DWORD scrolltexty;
DWORD sgLastScroll;
BYTE *pMedTextCels;
BYTE *pTextBoxCels;

/** Maps from font index to medtexts.cel frame number. */
const BYTE mfontframe[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 37, 49, 38, 0, 39, 40, 47,
	42, 43, 41, 45, 52, 44, 53, 55, 36, 27,
	28, 29, 30, 31, 32, 33, 34, 35, 51, 50,
	48, 46, 49, 54, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 42, 0, 43, 0, 0, 0, 1, 2, 3,
	4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
	14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
	24, 25, 26, 48, 0, 49, 0, 0
};
/**
 * Maps from medtexts.cel frame number to character width. Note, the
 * character width may be distinct from the frame width, which is 22 for every
 * medtexts.cel frame.
 */
const BYTE mfontkern[56] = {
	5, 15, 10, 13, 14, 10, 9, 13, 11, 5,
	5, 11, 10, 16, 13, 16, 10, 15, 12, 10,
	14, 17, 17, 22, 17, 16, 11, 5, 11, 11,
	11, 10, 11, 11, 11, 11, 15, 5, 10, 18,
	15, 8, 6, 6, 7, 10, 9, 6, 10, 10,
	5, 5, 5, 5, 11, 12
};

/* data */

/** pixels for a line of text and the empty space under it */
#define MQTEXTNL 38

void FreeQuestText()
{
	MemFreeDbg(pMedTextCels);
	MemFreeDbg(pTextBoxCels);
}

void InitQuestText()
{
	pMedTextCels = LoadFileInMem("Data\\MedTextS.CEL", NULL);
	pTextBoxCels = LoadFileInMem("Data\\TextBox.CEL", NULL);
	qtextflag = FALSE;
}

int GetWrap(const char *p)
{
	char tempstr[128];
	int l, i, w;
	BOOL doneflag;
	BYTE c;

	int lines = 0;

	doneflag = FALSE;
	while (!doneflag) {
		w = 0;
		const char *s = p;
		l = 0;
		while (*s != '\n' && *s != '|' && w < 543) {
			c = gbFontTransTbl[(BYTE)*s];
			s++;
			if (c != '\0') {
				tempstr[l] = c;
				w += mfontkern[mfontframe[c]] + 2;
			} else {
				l--;
			}
			l++;
		}
		tempstr[l] = '\0';
		if (*s == '|') {
			tempstr[l] = '\0';
			doneflag = TRUE;
		} else if (*s != '\n') {
			while (tempstr[l] != ' ' && l > 0) {
				tempstr[l] = '\0';
				l--;
			}
		}
		for (i = 0; tempstr[i]; i++) {
			p++;
			if (*p == '\n') {
				p++;
			}
		}
		lines++;
	}

	return lines;
}

void CalcTextSpeed(int n)
{
	DWORD SfxFrames, TextHeight;

	SfxFrames = GetSFXLength(n) * 20 / 1000;
	assert(SfxFrames != 0);

	TextHeight = GetWrap(qtextptr);
	TextHeight *= MQTEXTNL;
	TextHeight += MQTEXTNL * 4; // adjust so when speaker is done one line is left

	qtextSpd = (TextHeight << 16) / SfxFrames;
}

void InitQTextMsg(int m)
{
	if (alltext[m].scrlltxt) {
		questlog = FALSE;
		qtextptr = alltext[m].txtstr;
		qtextflag = TRUE;
		qtexty = 340 + SCREEN_Y + UI_OFFSET_Y;
		scrolltexty = qtexty << 16;
		CalcTextSpeed(alltext[m].sfxnr);
		sgLastScroll = SDL_GetTicks();
	}
	PlaySFX(alltext[m].sfxnr);
}

void DrawQTextBack()
{
	CelDraw(PANEL_X + 24, SCREEN_Y + 327 + UI_OFFSET_Y, pTextBoxCels, 1, 591);
	trans_rect(PANEL_LEFT + 27, UI_OFFSET_Y + 28, 585, 297);
}

void PrintQTextChr(int sx, int sy, BYTE *pCelBuff, int nCel)
{
	BYTE *pStart, *pEnd;

	/// ASSERT: assert(gpBuffer);
	pStart = gpBufStart;
	gpBufStart = &gpBuffer[BUFFER_WIDTH * (49 + SCREEN_Y + UI_OFFSET_Y)];
	pEnd = gpBufEnd;
	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (309 + SCREEN_Y + UI_OFFSET_Y)];
	CelDraw(sx, sy, pCelBuff, nCel, 22);

	gpBufStart = pStart;
	gpBufEnd = pEnd;
}

void DrawQText()
{
	int i, l, w, tx, ty;
	BYTE c;
	const char *p, *pnl, *s;
	char tempstr[128];
	BOOL doneflag;
	DWORD currTime;

	DrawQTextBack();

	p = qtextptr;
	pnl = NULL;
	tx = 48 + PANEL_X;
	ty = qtexty;

	doneflag = FALSE;
	while (!doneflag) {
		w = 0;
		s = p;
		l = 0;
		while (*s != '\n' && *s != '|' && w < 543) {
			c = gbFontTransTbl[(BYTE)*s];
			s++;
			if (c != '\0') {
				tempstr[l] = c;
				w += mfontkern[mfontframe[c]] + 2;
			} else {
				l--;
			}
			l++;
		}
		tempstr[l] = '\0';
		if (*s == '|') {
			tempstr[l] = '\0';
			doneflag = TRUE;
		} else if (*s != '\n') {
			while (tempstr[l] != ' ' && l > 0) {
				tempstr[l] = '\0';
				l--;
			}
		}
		for (i = 0; tempstr[i]; i++) {
			p++;
			c = mfontframe[gbFontTransTbl[(BYTE)tempstr[i]]];
			if (*p == '\n') {
				p++;
			}
			if (c != 0) {
				PrintQTextChr(tx, ty, pMedTextCels, c);
			}
			tx += mfontkern[c] + 2;
		}
		if (pnl == NULL) {
			pnl = p;
		}
		tx = 48 + PANEL_X;
		ty += MQTEXTNL;
		if (ty > 341 + SCREEN_Y + UI_OFFSET_Y) {
			doneflag = TRUE;
		}
	}

	currTime = SDL_GetTicks();

	double qtextDelay = (currTime - sgLastScroll) / (1000.0 / 20);
	scrolltexty -= (DWORD)(qtextSpd * qtextDelay);
	qtexty = scrolltexty >> 16;
	sgLastScroll = currTime;

	if (qtexty <= 49 + SCREEN_Y + UI_OFFSET_Y) {
		scrolltexty += MQTEXTNL << 16;
		qtexty = scrolltexty >> 16;
		qtextptr = pnl;
		if (*qtextptr == '|') {
			qtextflag = FALSE;
		}
	}
}

DEVILUTION_END_NAMESPACE
