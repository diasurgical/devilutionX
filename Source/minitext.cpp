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
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
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

	char tempstr[2560];
	strcpy(tempstr, text);

	const std::string paragraphs = WordWrapString(tempstr, 543, GameFont30);

	size_t previous = 0;
	while (true) {
		size_t next = paragraphs.find('\n', previous);
		TextLines.emplace_back(paragraphs.substr(previous, next - previous));
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
uint32_t CalculateTextSpeed(int nSFX)
{
	const int numLines = TextLines.size();

#ifndef NOSOUND
	Uint32 sfxFrames = GetSFXLength(nSFX);
#else
	// Sound is disabled -- estimate length from the number of lines.
	Uint32 sfxFrames = numLines * 3000;
#endif
	assert(sfxFrames != 0);

	uint32_t textHeight = LineHeight * numLines;
	textHeight += LineHeight * 5; // adjust so when speaker is done two line are left
	assert(textHeight != 0);

	return sfxFrames / textHeight;
}

int CalculateTextPosition()
{
	uint32_t currTime = SDL_GetTicks();

	int y = (currTime - ScrollStart) / qtextSpd - 260;

	int textHeight = LineHeight * TextLines.size();
	if (y >= textHeight)
		qtextflag = false;

	return y;
}

/**
 * @brief Draw the current text in the quest dialog window
 */
void DrawQTextContent(const Surface &out)
{
	int y = CalculateTextPosition();

	const int sx = PANEL_X + 48;
	const int sy = 0 - (y % LineHeight);

	const unsigned int skipLines = y / LineHeight;

	for (int i = 0; i < 8; i++) {
		const unsigned int lineNumber = skipLines + i;
		if (lineNumber >= TextLines.size()) {
			continue;
		}

		const std::string &line = TextLines[lineNumber];
		if (line.empty()) {
			continue;
		}

		DrawString(out, line, { { sx, sy + i * LineHeight }, { 543, LineHeight } }, UiFlags::FontSize30 | UiFlags::ColorGold);
	}
}

} // namespace

void FreeQuestText()
{
	pTextBoxCels = std::nullopt;
}

void InitQuestText()
{
	pTextBoxCels = LoadCel("Data\\TextBox.CEL", 591);
	qtextflag = false;
}

void InitQTextMsg(_speech_id m)
{
	if (Speeches[m].scrlltxt) {
		QuestLogIsOpen = false;
		LoadText(_(Speeches[m].txtstr));
		qtextflag = true;
		qtextSpd = CalculateTextSpeed(Speeches[m].sfxnr);
		ScrollStart = SDL_GetTicks();
	}
	PlaySFX(Speeches[m].sfxnr);
}

void DrawQTextBack(const Surface &out)
{
	CelDrawTo(out, { PANEL_X + 24, 327 + UI_OFFSET_Y }, *pTextBoxCels, 1);
	DrawHalfTransparentRectTo(out, PANEL_X + 27, UI_OFFSET_Y + 28, 585, 297);
}

void DrawQText(const Surface &out)
{
	DrawQTextBack(out);
	DrawQTextContent(out.subregionY(UI_OFFSET_Y + 49, 260));
}

} // namespace devilution
