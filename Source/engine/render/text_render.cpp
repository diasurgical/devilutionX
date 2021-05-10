/**
 * @file text_render.cpp
 *
 * Text rendering.
 */

#include "engine.h"
#include "text_render.hpp"
#include "cel_render.hpp"
#include "palette.h"
#include "DiabloUI/ui_item.h"

namespace devilution {

/**
 * Maps ASCII character code to font index, as used by the
 * small, medium and large sized fonts; which corresponds to smaltext.cel,
 * medtexts.cel and bigtgold.cel respectively.
 */
const uint8_t gbFontTransTbl[256] = {
	// clang-format off
	'\0', 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	' ',  '!',  '\"', '#',  '$',  '%',  '&',  '\'', '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
	'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
	'@',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
	'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '[',  '\\', ']',  '^',  '_',
	'`',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
	'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '{',  '|',  '}',  '~',  0x01,
	'C',  'u',  'e',  'a',  'a',  'a',  'a',  'c',  'e',  'e',  'e',  'i',  'i',  'i',  'A',  'A',
	'E',  'a',  'A',  'o',  'o',  'o',  'u',  'u',  'y',  'O',  'U',  'c',  'L',  'Y',  'P',  'f',
	'a',  'i',  'o',  'u',  'n',  'N',  'a',  'o',  '?',  0x01, 0x01, 0x01, 0x01, '!',  '<',  '>',
	'o',  '+',  '2',  '3',  '\'', 'u',  'P',  '.',  ',',  '1',  '0',  '>',  0x01, 0x01, 0x01, '?',
	'A',  'A',  'A',  'A',  'A',  'A',  'A',  'C',  'E',  'E',  'E',  'E',  'I',  'I',  'I',  'I',
	'D',  'N',  'O',  'O',  'O',  'O',  'O',  'X',  '0',  'U',  'U',  'U',  'U',  'Y',  'b',  'B',
	'a',  'a',  'a',  'a',  'a',  'a',  'a',  'c',  'e',  'e',  'e',  'e',  'i',  'i',  'i',  'i',
	'o',  'n',  'o',  'o',  'o',  'o',  'o',  '/',  '0',  'u',  'u',  'u',  'u',  'y',  'b',  'y',
	// clang-format on
};

/** Maps from font index to cel frame number. */
const uint8_t fontframe[3][128] = {
	{
	    // clang-format off
	     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	     0, 54, 44, 57, 58, 56, 55, 47, 40, 41, 59, 39, 50, 37, 51, 52,
	    36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 48, 49, 60, 38, 61, 53,
	    62,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42, 63, 43, 64, 65,
	     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 40, 66, 41, 67,  0,
	    // clang-format on
	},
	{
	    // clang-format off
	     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	     0, 37, 49, 38,  0, 39, 40, 47, 42, 43, 41, 45, 52, 44, 53, 55,
	    36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 51, 50, 48, 46, 49, 54,
	     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42,  0, 43,  0,  0,
	     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 48,  0, 49,  0,  0,
	    // clang-format on
	},
	{
	    // clang-format off
	     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	     0, 37, 49, 38,  0, 39, 40, 47, 42, 43, 41, 45, 52, 44, 53, 55,
	    36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 51, 50,  0, 46,  0, 54,
	     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42,  0, 43,  0,  0,
	     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 20,  0, 21,  0,  0,
	    // clang-format on
	},
};

/**
 * Maps from cel frame number to character width. Note, the character width
 * may be distinct from the frame width, which is the same for every cel frame.
 */
const uint8_t fontkern[3][68] = {
	{
	    // clang-format off
		 8, 10,  7,  9,  8,  7,  6,  8,  8,  3,
		 3,  8,  6, 11,  9, 10,  6,  9,  9,  6,
		 9, 11, 10, 13, 10, 11,  7,  5,  7,  7,
		 8,  7,  7,  7,  7,  7, 10,  4,  5,  6,
		 3,  3,  4,  3,  6,  6,  3,  3,  3,  3,
		 3,  2,  7,  6,  3, 10, 10,  6,  6,  7,
		 4,  4,  9,  6,  6, 12,  3,  7
	    // clang-format on
	},
	{
	    // clang-format off
		 5, 15, 10, 13, 14, 10,  9, 13, 11,  5,
		 5, 11, 10, 16, 13, 16, 10, 15, 12, 10,
		14, 17, 17, 22, 17, 16, 11,  5, 11, 11,
		11, 10, 11, 11, 11, 11, 15,  5, 10, 18,
		15,  8,  6,  6,  7, 10,  9,  6, 10, 10,
		 5,  5,  5,  5, 11, 12
	    // clang-format on
	},
	{
	    // clang-format off
		18, 33, 21, 26, 28, 19, 19, 26, 25, 11,
		12, 25, 19, 34, 28, 32, 20, 32, 28, 20,
		28, 36, 35, 46, 33, 33, 24, 11, 23, 22,
		22, 21, 22, 21, 21, 21, 32, 10, 20, 36,
		31, 17, 13, 12, 13, 18, 16, 11, 20, 21,
		11, 10, 12, 11, 21, 23
	    // clang-format on
	}
};

int LineHeights[3] = { 17, 43, 50 };

std::optional<CelSprite> pPanelText;
/** Graphics for the medium size font */
std::optional<CelSprite> pMedTextCels;
std::optional<CelSprite> BigTGold_cel;

std::optional<CelSprite> pSPentSpn2Cels;

uint8_t fontColorTableGold[256];
uint8_t fontColorTableBlue[256];
uint8_t fontColorTableRed[256];

void InitText()
{
	pPanelText = LoadCel("CtrlPan\\SmalText.CEL", 13);
	pMedTextCels = LoadCel("Data\\MedTextS.CEL", 22);
	BigTGold_cel = LoadCel("Data\\BigTGold.CEL", 46);

	pSPentSpn2Cels = LoadCel("Data\\PentSpn2.CEL", 12);

	for (int i = 0; i < 256; i++) {
		uint8_t pix = i;
		if (pix >= PAL16_GRAY + 14)
			pix = PAL16_BLUE + 15;
		else if (pix >= PAL16_GRAY)
			pix -= PAL16_GRAY - (PAL16_BLUE + 2);
		fontColorTableBlue[i] = pix;
	}

	for (int i = 0; i < 256; i++) {
		uint8_t pix = i;
		if (pix >= PAL16_GRAY)
			pix -= PAL16_GRAY - PAL16_RED;
		fontColorTableRed[i] = pix;
	}

	for (int i = 0; i < 256; i++) {
		uint8_t pix = i;
		if (pix >= PAL16_GRAY + 14)
			pix = PAL16_YELLOW + 15;
		else if (pix >= PAL16_GRAY)
			pix -= PAL16_GRAY - (PAL16_YELLOW + 2);
		fontColorTableGold[i] = pix;
	}
}

void FreeText()
{
	pPanelText = std::nullopt;
	pMedTextCels = std::nullopt;
	BigTGold_cel = std::nullopt;

	pSPentSpn2Cels = std::nullopt;
}

void PrintChar(const CelOutputBuffer &out, int sx, int sy, int nCel, text_color col)
{
	switch (col) {
	case COL_WHITE:
		CelDrawTo(out, sx, sy, *pPanelText, nCel);
		return;
	case COL_BLUE:
		CelDrawLightTo(out, sx, sy, *pPanelText, nCel, fontColorTableBlue);
		break;
	case COL_RED:
		CelDrawLightTo(out, sx, sy, *pPanelText, nCel, fontColorTableRed);
		break;
	case COL_GOLD:
		CelDrawLightTo(out, sx, sy, *pPanelText, nCel, fontColorTableGold);
		break;
	case COL_BLACK:
		light_table_index = 15;
		CelDrawLightTo(out, sx, sy, *pPanelText, nCel, nullptr);
		return;
	}
}

int GetLineWidth(const char *text, GameFontTables size)
{
	int lineWidth = 0;

	size_t textLength = strlen(text);
	for (unsigned i = 0; i < textLength; i++) {
		if (text[i] == '\n')
			break;

		uint8_t frame = fontframe[size][gbFontTransTbl[static_cast<uint8_t>(text[i])]];
		lineWidth += fontkern[size][frame] + 1;
	}

	return lineWidth != 0 ? (lineWidth - 1) : 0;
}

void WordWrapGameString(char *text, size_t width, size_t size)
{
	const size_t textLength = strlen(text);
	size_t lineStart = 0;
	size_t lineWidth = 0;
	for (unsigned i = 0; i < textLength; i++) {
		if (text[i] == '\n') { // Existing line break, scan next line
			lineStart = i + 1;
			lineWidth = 0;
			continue;
		}

		uint8_t frame = fontframe[size][gbFontTransTbl[static_cast<uint8_t>(text[i])]];
		lineWidth += fontkern[size][frame] + 1;

		if (lineWidth - 1 <= width) {
			continue; // String is still within the limit, continue to the next line
		}

		size_t j; // Backtrack to the previous space
		for (j = i; j >= lineStart; j--) {
			if (text[j] == ' ') {
				break;
			}
		}

		if (j == lineStart) { // Single word longer then width
			if (i == textLength)
				break;
			j = i;
		}

		// Break line and continue to next line
		i = j;
		text[i] = '\n';
		lineStart = i + 1;
		lineWidth = 0;
	}
}

/**
 * @todo replace SDL_Rect with croped CelOutputBuffer
 */
void DrawString(const CelOutputBuffer &out, const char *text, const SDL_Rect &rect, uint16_t flags, bool drawTextCursor)
{
	GameFontTables size = GameFontSmall;
	if ((flags & UIS_MED) != 0)
		size = GameFontMed;
	else if ((flags & UIS_HUGE) != 0)
		size = GameFontBig;

	text_color color = COL_GOLD;
	if ((flags & UIS_SILVER) != 0)
		color = COL_WHITE;
	else if ((flags & UIS_BLUE) != 0)
		color = COL_BLUE;
	else if ((flags & UIS_RED) != 0)
		color = COL_RED;
	else if ((flags & UIS_BLACK) != 0)
		color = COL_BLACK;

	const int w = rect.w != 0 ? rect.w : out.w() - rect.x;
	const int h = rect.h != 0 ? rect.h : out.h() - rect.x;

	int sx = rect.x;
	if ((flags & UIS_CENTER) != 0)
		sx += (w - GetLineWidth(text, size)) / 2;
	int sy = rect.y;

	int rightMargin = rect.x + w;
	int bottomMargin = rect.y + h;

	const size_t textLength = strlen(text);
	for (unsigned i = 0; i < textLength; i++) {
		uint8_t frame = fontframe[size][gbFontTransTbl[static_cast<uint8_t>(text[i])]];
		int symbolWidth = fontkern[size][frame] + 1;
		if (text[i] == '\n' || sx + symbolWidth - 1 > rightMargin) {
			sx = rect.x;
			if ((flags & UIS_CENTER) != 0)
				sx += (w - GetLineWidth(&text[i + 1], size)) / 2;
			sy += LineHeights[size];
			if (sy > bottomMargin)
				return;
		}
		if (frame != 0) {
			PrintChar(out, sx, sy, frame, color);
		}
		if (text[i] != '\n')
			sx += symbolWidth;
	}
	if (drawTextCursor) {
		CelDrawTo(out, sx, sy, *pSPentSpn2Cels, PentSpn2Spin());
	}
}

int PentSpn2Spin()
{
	return (SDL_GetTicks() / 50) % 8 + 1;
}

} // namespace devilution
