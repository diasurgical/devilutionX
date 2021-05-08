/**
 * @file minitext.cpp
 *
 * Implementation of scrolling dialog text.
 */

#include "control.h"
#include "dx.h"
#include "engine.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "textdat.h"
#include "utils/language.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

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
/** Graphics for the window border */
std::optional<CelSprite> pTextBoxCels;

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
		uint8_t c = gbFontTransTbl[(uint8_t)*text];
		text++;
		if (c != '\0') {
			line[l] = c;
			lineWidth += fontkern[GameFontMed][fontframe[GameFontMed][c]] + 2;
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
	int TextHeight;
	Uint32 SfxFrames;

	const int numLines = GetLinesInText(qtextptr);

#ifndef NOSOUND
	SfxFrames = GetSFXLength(nSFX);
	assert(SfxFrames != 0);
#else
	// Sound is disabled -- estimate length from the number of lines.
	SfxFrames = numLines * 3000;
#endif

	TextHeight = lineHeight * numLines;
	TextHeight += lineHeight * 5; // adjust so when speaker is done two line are left

	return SfxFrames / TextHeight;
}

/**
 * @brief Print a character
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param cel CEL sprite
 * @param nCel CEL frame number
 */
void PrintQTextChr(int sx, int sy, const CelSprite &cel, int nCel)
{
	const int start_y = 49 + UI_OFFSET_Y;
	const CelOutputBuffer &buf = GlobalBackBuffer().subregionY(start_y, 260);
	CelDrawTo(buf, sx, sy - start_y, cel, nCel);
}

/**
 * @brief Draw the current text in the quest dialog window
 * @return the start of the text currently being rendered
 */
void ScrollQTextContent(const char *pnl)
{
	for (Uint32 currTime = SDL_GetTicks(); sgLastScroll + qtextSpd < currTime; sgLastScroll += qtextSpd) {
		qtexty--;
		if (qtexty <= 49 + UI_OFFSET_Y) {
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
static void DrawQTextContent()
{
	// TODO: Draw to the given `out` buffer.
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
			uint8_t c = fontframe[GameFontMed][gbFontTransTbl[(uint8_t)line[i]]];
			if (*text == '\n') {
				text++;
			}
			if (c != 0) {
				PrintQTextChr(tx, ty, *pMedTextCels, c);
			}
			tx += fontkern[GameFontMed][c] + 2;
		}
		if (pnl == nullptr) {
			pnl = text;
		}
		tx = 48 + PANEL_X;
		ty += lineHeight;
		if (ty > 341 + UI_OFFSET_Y) {
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
	pTextBoxCels = std::nullopt;
}

/**
 * @brief Load the resouces used by the quest dialog window, and initialize it's state
 */
void InitQuestText()
{
	pTextBoxCels = LoadCel("Data\\TextBox.CEL", 591);
	qtextflag = false;
}

/**
 * @brief Start the given naration
 * @param m Index of narration from the alltext table
 */
void InitQTextMsg(_speech_id m)
{
	if (alltext[m].scrlltxt) {
		questlog = false;
		qtextptr = _(alltext[m].txtstr);
		qtextflag = true;
		qtexty = 340 + UI_OFFSET_Y;
		qtextSpd = CalcTextSpeed(alltext[m].sfxnr);
		sgLastScroll = SDL_GetTicks();
	}
	PlaySFX(alltext[m].sfxnr);
}

void DrawQTextBack(const CelOutputBuffer &out)
{
	CelDrawTo(out, PANEL_X + 24, 327 + UI_OFFSET_Y, *pTextBoxCels, 1);
	DrawHalfTransparentRectTo(out, PANEL_X + 27, UI_OFFSET_Y + 28, 585, 297);
}

void DrawQText(const CelOutputBuffer &out)
{
	DrawQTextBack(out);
	DrawQTextContent();
}

} // namespace devilution
