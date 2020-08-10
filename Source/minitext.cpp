/**
 * @file minitext.cpp
 *
 * Implementation of scrolling dialog text.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int qtexty;
char *qtextptr;
int qtextSpd;
BOOLEAN qtextflag;
int scrolltexty;
int sgLastScroll;
BYTE *pMedTextCels;
BYTE *pTextBoxCels;

/** Maps from font index to medtexts.cel frame number. */
const BYTE mfontframe[127] = {
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
	24, 25, 26, 48, 0, 49, 0
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

/**
 * Text scroll speeds. Positive numbers will delay scrolling 1 out of n frames,
 * negative numbers will scroll 1+(-n) pixels.
 */
int qscroll_spd_tbl[9] = { 2, 4, 6, 8, 0, -1, -2, -3, -4 };

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

void InitQTextMsg(int m)
{
	if (alltext[m].scrlltxt) {
		questlog = FALSE;
		qtextptr = alltext[m].txtstr;
		qtextflag = TRUE;
		qtexty = 500;
		qtextSpd = qscroll_spd_tbl[alltext[m].txtspd - 1];
		if (qtextSpd <= 0)
			scrolltexty = 50 / -(qtextSpd - 1);
		else
			scrolltexty = ((qtextSpd + 1) * 50) / qtextSpd;
		qtextSpd = SDL_GetTicks();
	}
	PlaySFX(alltext[m].sfxnr);
}

void DrawQTextBack()
{
	CelDraw(PANEL_X + 24, (SCREEN_Y + 327 + MAINMENU_BACKGROUND_Y), pTextBoxCels, 1, 591);  //487  0.683
	trans_rect(PANEL_LEFT + 27, (MAINMENU_BACKGROUND_Y + 28), 585, 297);
}

void PrintQTextChr(int sx, int sy, BYTE *pCelBuff, int nCel)
{
	BYTE *pStart, *pEnd;

	/// ASSERT: assert(gpBuffer);
	pStart = gpBufStart;
	gpBufStart = &gpBuffer[BUFFER_WIDTH * (49 + SCREEN_Y)];
	pEnd = gpBufEnd;
	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (309 + SCREEN_Y)];
	CelDraw(sx, sy, pCelBuff, nCel, 22);

	gpBufStart = pStart;
	gpBufEnd = pEnd;
}

void DrawQText()
{
	int i, l, w, tx, ty;
	BYTE c;
	char *p, *pnl, *s;
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
		ty += 38;
		if (ty > 501) {
			doneflag = TRUE;
		}
	}

	for (currTime = SDL_GetTicks(); qtextSpd + scrolltexty < currTime; qtextSpd += scrolltexty) {
		qtexty--;
		if (qtexty <= 209) {
			qtexty += 38;
			qtextptr = pnl;
			if (*pnl == '|') {
				qtextflag = FALSE;
			}
			break;
		}
	}
}

DEVILUTION_END_NAMESPACE
