/**
 * @file minitext.cpp
 *
 * Implementation of scrolling dialog text.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

/** Specify if the quest dialog window is being shown */
bool qtextflag;

namespace {

/** Current y position of text in px */
int qtexty;
/** Pointer to the current text being displayed */
const char *qtextptr;
/** Vertical speed of the scrolling text in ms/px */
int qtextSpd;
/** Time of last rendering of the text */
Uint32 sgLastScroll;
/** Graphics for the medium size font */
Uint8 *pMedTextCels;
/** Graphics for the window border */
Uint8 *pTextBoxCels;

/** Maps from font index to medtexts.cel frame number. */
const Uint8 mfontframe[128] = {
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
const Uint8 mfontkern[56] = {
	5, 15, 10, 13, 14, 10, 9, 13, 11, 5,
	5, 11, 10, 16, 13, 16, 10, 15, 12, 10,
	14, 17, 17, 22, 17, 16, 11, 5, 11, 11,
	11, 10, 11, 11, 11, 11, 15, 5, 10, 18,
	15, 8, 6, 6, 7, 10, 9, 6, 10, 10,
	5, 5, 5, 5, 11, 12
};

/** Pixels for a line of text and the empty space under it. */
const int lineHeight = 38;

/**
 * @brief Build a single line of text from the given text stream
 * @param text The original text
 * @param line The buffer to insert the line in to
 * @return Indicate that the end of the text was reached
 */
bool BuildLine(const char *text, char line[128])
{
	int lineWidth = 0;
	int l = 0;

	while (*text != '\n' && *text != '|' && lineWidth < 543) {
		Uint8 c = gbFontTransTbl[(Uint8)*text];
		text++;
		if (c != '\0') {
			line[l] = c;
			lineWidth += mfontkern[mfontframe[c]] + 2;
		} else {
			l--;
		}
		l++;
	}
	line[l] = '\0';
	if (*text == '|') {
		line[l] = '\0';
		return true;
	}

	if (*text != '\n') {
		while (line[l] != ' ' && l > 0) {
			line[l] = '\0';
			l--;
		}
	}

	return false;
}

/**
 * @brief Calculate the number of line required by the given text
 * @return Number of lines
 */
int GetLinesInText(const char *text)
{
	char line[128];
	int lines = 0;

	bool doneflag = false;
	while (!doneflag) {
		doneflag = BuildLine(text, line);
		text += strlen(line);
		if (*text == '\n')
			text++;
		lines++;
	}

	return lines;
}

/**
 * @brief Calculate the speed the current text should scroll to match the given audio
 * @param nSFX The index of the sound in the sgSFX table
 * @return ms/px
 */
int CalcTextSpeed(int nSFX)
{
	Uint32 SfxFrames, TextHeight;

	SfxFrames = GetSFXLength(nSFX);
	assert(SfxFrames != 0);

	TextHeight = lineHeight * GetLinesInText(qtextptr);
	TextHeight += lineHeight * 5; // adjust so when speaker is done two line are left

	return SfxFrames / TextHeight;
}

/**
 * @brief Print a character
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 */
void PrintQTextChr(int sx, int sy, Uint8 *pCelBuff, int nCel)
{
	Uint8 *pStart, *pEnd;

	assert(gpBuffer);
	pStart = gpBufStart;
	gpBufStart = &gpBuffer[BUFFER_WIDTH * (49 + SCREEN_Y + UI_OFFSET_Y)];
	pEnd = gpBufEnd;
	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (309 + SCREEN_Y + UI_OFFSET_Y)];
	CelDraw(sx, sy, pCelBuff, nCel, 22);

	gpBufStart = pStart;
	gpBufEnd = pEnd;
}

/**
 * @brief Draw the current text in the quest dialog window
 * @return the start of the text currently being rendered
 */
void ScrollQTextContent(const char *pnl)
{
	for (Uint32 currTime = SDL_GetTicks(); sgLastScroll + qtextSpd < currTime; sgLastScroll += qtextSpd) {
		qtexty--;
		if (qtexty <= 49 + SCREEN_Y + UI_OFFSET_Y) {
			qtexty += 38;
			qtextptr = pnl;
			if (*pnl == '|') {
				qtextflag = false;
			}
			break;
		}
	}
}

/**
 * @brief Draw the current text in the quest dialog window
 */
void DrawQTextContent()
{
	const char *text, *pnl;
	char line[128];

	text = qtextptr;
	pnl = nullptr;
	int tx = 48 + PANEL_X;
	int ty = qtexty;

	bool doneflag = false;
	while (!doneflag) {
		doneflag = BuildLine(text, line);
		for (int i = 0; line[i]; i++) {
			text++;
			Uint8 c = mfontframe[gbFontTransTbl[(Uint8)line[i]]];
			if (*text == '\n') {
				text++;
			}
			if (c != 0) {
				PrintQTextChr(tx, ty, pMedTextCels, c);
			}
			tx += mfontkern[c] + 2;
		}
		if (pnl == nullptr) {
			pnl = text;
		}
		tx = 48 + PANEL_X;
		ty += lineHeight;
		if (ty > 341 + SCREEN_Y + UI_OFFSET_Y) {
			doneflag = true;
		}
	}

	ScrollQTextContent(pnl);
}

} // namespace

/**
 * @brief Free the resouces used by the quest dialog window
 */
void FreeQuestText()
{
	MemFreeDbg(pMedTextCels);
	MemFreeDbg(pTextBoxCels);
}

/**
 * @brief Load the resouces used by the quest dialog window, and initialize it's state
 */
void InitQuestText()
{
	pMedTextCels = LoadFileInMem("Data\\MedTextS.CEL", nullptr);
	pTextBoxCels = LoadFileInMem("Data\\TextBox.CEL", nullptr);
	qtextflag = false;
}

/**
 * @brief Start the given naration
 * @param m Index of narration from the alltext table
 */
void InitQTextMsg(int m)
{
	if (alltext[m].scrlltxt) {
		questlog = false;
		qtextptr = alltext[m].txtstr;
		qtextflag = true;
		qtexty = 340 + SCREEN_Y + UI_OFFSET_Y;
		qtextSpd = CalcTextSpeed(alltext[m].sfxnr);
		sgLastScroll = SDL_GetTicks();
	}
	PlaySFX(alltext[m].sfxnr);
}

/**
 * @brief Draw the quest dialog window decoration and background
 */
void DrawQTextBack()
{
	CelDraw(PANEL_X + 24, SCREEN_Y + 327 + UI_OFFSET_Y, pTextBoxCels, 1, 591);
	trans_rect(PANEL_LEFT + 27, UI_OFFSET_Y + 28, 585, 297);
}

/**
 * @brief Draw the quest dialog window decoration and background
 */
void DrawQText()
{
	DrawQTextBack();
	DrawQTextContent();
}

DEVILUTION_END_NAMESPACE
