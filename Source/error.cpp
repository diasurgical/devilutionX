/**
 * @file error.cpp
 *
 * Implementation of in-game message functions.
 */

#include <deque>

#include "error.h"

#include "DiabloUI/ui_flags.hpp"
#include "control.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "stores.h"
#include "utils/language.h"

namespace devilution {

namespace {

std::deque<std::string> DiabloMessages;
std::vector<std::string> TextLines;
uint32_t msgdelay;
int ErrorWindowHeight = 54;
const int LineHeight = 12;
const int LineWidth = 418;

void InitNextLines()
{
	msgdelay = SDL_GetTicks();
	auto message = DiabloMessages.front();

	TextLines.clear();

	char tempstr[1536]; // Longest test is about 768 chars * 2 for unicode
	strcpy(tempstr, message.data());

	const std::string paragraphs = WordWrapString(tempstr, LineWidth, GameFont12, 1);

	size_t previous = 0;
	while (true) {
		size_t next = paragraphs.find('\n', previous);
		TextLines.emplace_back(paragraphs.substr(previous, next - previous));
		if (next == std::string::npos)
			break;
		previous = next + 1;
	}

	ErrorWindowHeight = std::max(54, static_cast<int>((TextLines.size() * LineHeight) + 42));
}

} // namespace

/** Maps from error_id to error message. */
const char *const MsgStrings[] = {
	"",
	N_("No automap available in town"),
	N_("No multiplayer functions in demo"),
	N_("Direct Sound Creation Failed"),
	N_("Not available in shareware version"),
	N_("Not enough space to save"),
	N_("No Pause in town"),
	N_("Copying to a hard disk is recommended"),
	N_("Multiplayer sync problem"),
	N_("No pause in multiplayer"),
	N_("Loading..."),
	N_("Saving..."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Some are weakened as one grows strong"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "New strength is forged through destruction"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Those who defend seldom attack"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "The sword of justice is swift and sharp"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "While the spirit is vigilant the body thrives"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "The powers of mana refocused renews"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Time cannot diminish the power of steel"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Magic is not always what it seems to be"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "What once was opened now is closed"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Intensity comes at the cost of wisdom"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Arcane power brings destruction"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "That which cannot be held cannot be harmed"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Crimson and Azure become as the sun"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Knowledge and wisdom at the cost of self"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Drink and be refreshed"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Wherever you go, there you are"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Energy comes at the cost of wisdom"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Riches abound when least expected"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Where avarice fails, patience gains reward"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Blessed by a benevolent companion!"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "The hands of men may be guided by fate"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Strength is bolstered by heavenly faith"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "The essence of life flows from within"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "The way is made clear when viewed from above"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Salvation comes at the cost of wisdom"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Mysteries are revealed in the light of reason"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Those who are last may yet be first"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Generosity brings its own rewards"),
	N_("You must be at least level 8 to use this."),
	N_("You must be at least level 13 to use this."),
	N_("You must be at least level 17 to use this."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Arcane knowledge gained!"),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "That which does not kill you..."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Knowledge is power."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Give and you shall receive."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Some experience is gained by touch."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "There's no place like home."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "Spiritual energy is restored."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "You feel more agile."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "You feel stronger."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "You feel wiser."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "You feel refreshed."),
	N_(/* TRANSLATORS: Shrine Text. Keep atmospheric. :) */ "That which can break will."),
};

void InitDiabloMsg(diablo_message e)
{
	std::string msg = _(MsgStrings[e]);
	InitDiabloMsg(msg);
}

void InitDiabloMsg(std::string msg)
{
	if (DiabloMessages.size() >= MAX_SEND_STR_LEN)
		return;

	if (std::find(DiabloMessages.begin(), DiabloMessages.end(), msg) != DiabloMessages.end())
		return;

	DiabloMessages.push_back(msg);
	if (DiabloMessages.size() == 1)
		InitNextLines();
}

bool IsDiabloMsgAvailable()
{
	return !DiabloMessages.empty();
}

void CancelCurrentDiabloMsg()
{
	msgdelay = 0;
}

void ClrDiabloMsg()
{
	DiabloMessages.clear();
}

void DrawDiabloMsg(const Surface &out)
{
	int dialogStartY = ((gnScreenHeight - PANEL_HEIGHT) / 2) - (ErrorWindowHeight / 2) + 9;

	CelDrawTo(out, { PANEL_X + 101, dialogStartY }, *pSTextSlidCels, 1);
	CelDrawTo(out, { PANEL_X + 527, dialogStartY }, *pSTextSlidCels, 4);
	CelDrawTo(out, { PANEL_X + 101, dialogStartY + ErrorWindowHeight - 6 }, *pSTextSlidCels, 2);
	CelDrawTo(out, { PANEL_X + 527, dialogStartY + ErrorWindowHeight - 6 }, *pSTextSlidCels, 3);

	int sx = PANEL_X + 109;
	for (int i = 0; i < 35; i++) {
		CelDrawTo(out, { sx, dialogStartY }, *pSTextSlidCels, 5);
		CelDrawTo(out, { sx, dialogStartY + ErrorWindowHeight - 6 }, *pSTextSlidCels, 7);
		sx += 12;
	}
	int drawnYborder = 12;
	while ((drawnYborder + 12) < ErrorWindowHeight) {
		CelDrawTo(out, { PANEL_X + 101, dialogStartY + drawnYborder }, *pSTextSlidCels, 6);
		CelDrawTo(out, { PANEL_X + 527, dialogStartY + drawnYborder }, *pSTextSlidCels, 8);
		drawnYborder += 12;
	}

	DrawHalfTransparentRectTo(out, PANEL_X + 104, dialogStartY - 8, 432, ErrorWindowHeight);

	auto message = DiabloMessages.front();
	int lineNumber = 0;
	for (auto &line : TextLines) {
		DrawString(out, line.c_str(), { { PANEL_X + 109, dialogStartY + 12 + lineNumber * LineHeight }, { LineWidth, LineHeight } }, UiFlags::AlignCenter, 1, LineHeight);
		lineNumber += 1;
	}

	if (msgdelay > 0 && msgdelay <= SDL_GetTicks() - 3500) {
		msgdelay = 0;
	}
	if (msgdelay == 0) {
		DiabloMessages.pop_front();
		if (!DiabloMessages.empty())
			InitNextLines();
	}
}

} // namespace devilution
