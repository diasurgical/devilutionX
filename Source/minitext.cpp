/**
 * @file minitext.cpp
 *
 * Implementation of scrolling dialog text.
 */
#include <string>
#include <vector>

#include "control.h"
#include "dx.h"
#include "engine.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "textdat.h"
#include "utils/language.h"
#include "utils/stdcompat/optional.hpp"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

/** Specify if the quest dialog window is being shown */
bool qtextflag;

namespace {

/** Vertical speed of the scrolling text in ms/px */
int qtextSpd;
/** Start time of scrolling */
Uint32 ScrollStart;
/** Graphics for the window border */
std::optional<CelSprite> pTextBoxCels;

/** Pixels for a line of text and the empty space under it. */
const int LineHeight = 38;

std::vector<std::string> TextLines;

void LoadText(const char *text)
{
	TextLines.clear();

	char tempstr[1536]; // Longest test is about 768 chars * 2 for unicode
	strcpy(tempstr, text);

	WordWrapGameString(tempstr, 543, GameFontMed, 2);
	const string_view paragraphs = tempstr;

	size_t previous = 0;
	while (true) {
		size_t next = paragraphs.find('\n', previous);
		TextLines.emplace_back(paragraphs.substr(previous, next));
		if (next == std::string::npos)
			break;
		previous = next + 1;
	}
}

/**
 * @brief Calculate the speed the current text should scroll to match the given audio
 * @param nSFX The index of the sound in the sgSFX table
 * @return ms/px
 */
int CalculateTextSpeed(int nSFX)
{
	const int numLines = TextLines.size();

#ifndef NOSOUND
	Uint32 sfxFrames = GetSFXLength(nSFX);
	assert(sfxFrames != 0);
#else
	// Sound is disabled -- estimate length from the number of lines.
	Uint32 SfxFrames = numLines * 3000;
#endif

	int textHeight = LineHeight * numLines;
	textHeight += LineHeight * 5; // adjust so when speaker is done two line are left

	return sfxFrames / textHeight;
}

int CalculateTextPosition()
{
	Uint32 currTime = SDL_GetTicks();

	int y = (currTime - ScrollStart) / qtextSpd - 260;

	int textHeight = LineHeight * TextLines.size();
	if (y >= textHeight)
		qtextflag = false;

	return y;
}

/**
 * @brief Draw the current text in the quest dialog window
 */
void DrawQTextContent(const CelOutputBuffer &out)
{
	int y = CalculateTextPosition();

	const int sx = PANEL_X + 48;
	const int sy = LineHeight / 2 - (y % LineHeight);

	const unsigned int skipLines = y / LineHeight;

	for (int i = 0; i < 8; i++) {
		const int lineNumber = skipLines + i;
		if (lineNumber < 0 || lineNumber >= (int)TextLines.size()) {
			continue;
		}

		const char *line = TextLines[lineNumber].c_str();
		if (line[0] == '\0') {
			continue;
		}

		DrawString(out, line, { sx, sy + i * LineHeight, 543, LineHeight }, UIS_MED, 2);
	}
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
		LoadText(_(alltext[m].txtstr));
		qtextflag = true;
		qtextSpd = CalculateTextSpeed(alltext[m].sfxnr);
		ScrollStart = SDL_GetTicks();
	}
	PlaySFX(alltext[m].sfxnr);
}

void DrawQTextBack(const CelOutputBuffer &out)
{
	CelDrawTo(out, PANEL_X + 24, 327 + UI_OFFSET_Y, *pTextBoxCels, 1);
	DrawHalfTransparentRectTo(out, PANEL_X + 27, UI_OFFSET_Y + 28, 585, 297, 0);
}

void DrawQText(const CelOutputBuffer &out)
{
	DrawQTextBack(out);
	DrawQTextContent(out.subregionY(UI_OFFSET_Y + 49, 260));
}

} // namespace devilution
