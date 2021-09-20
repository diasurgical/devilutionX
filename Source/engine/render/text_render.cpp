/**
 * @file text_render.cpp
 *
 * Text rendering.
 */
#include "text_render.hpp"

#include <array>
#include <unordered_map>
#include <utility>

#include "DiabloUI/art_draw.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/ui_item.h"
#include "cel_render.hpp"
#include "engine.h"
#include "engine/load_cel.hpp"
#include "engine/load_file.hpp"
#include "engine/point.hpp"
#include "palette.h"
#include "utils/display.h"
#include "utils/sdl_compat.h"
#include "utils/utf8.h"

namespace devilution {

namespace {

std::unordered_map<uint32_t, Art> Fonts;
std::unordered_map<uint32_t, std::array<uint8_t, 256>> FontKerns;
std::array<int, 5> FontSizes = { 12, 24, 30, 42, 46 };
std::array<int, 5> LineHeights = { 12, 26, 38, 42, 50 };
std::array<int, 5> BaseLineOffset = { -3, -2, -3, -6, -7 };

std::array<const char *, 12> ColorTranlations = {
	"fonts\\goldui.trn",
	"fonts\\grayui.trn",
	"fonts\\golduis.trn",
	"fonts\\grayuis.trn",

	nullptr,
	"fonts\\black.trn",

	"fonts\\white.trn",
	"fonts\\whitegold.trn",
	"fonts\\red.trn",
	"fonts\\blue.trn",

	"fonts\\buttonface.trn",
	"fonts\\buttonpushed.trn",
};

GameFontTables GetSizeFromFlags(UiFlags flags)
{
	if (HasAnyOf(flags, UiFlags::FontSize24))
		return GameFont24;
	else if (HasAnyOf(flags, UiFlags::FontSize30))
		return GameFont30;
	else if (HasAnyOf(flags, UiFlags::FontSize42))
		return GameFont42;
	else if (HasAnyOf(flags, UiFlags::FontSize46))
		return GameFont46;

	return GameFont12;
}

text_color GetColorFromFlags(UiFlags flags)
{
	if (HasAnyOf(flags, UiFlags::ColorWhite))
		return ColorWhite;
	else if (HasAnyOf(flags, UiFlags::ColorBlue))
		return ColorBlue;
	else if (HasAnyOf(flags, UiFlags::ColorRed))
		return ColorRed;
	else if (HasAnyOf(flags, UiFlags::ColorBlack))
		return ColorBlack;
	else if (HasAnyOf(flags, UiFlags::ColorGold))
		return ColorGold;
	else if (HasAnyOf(flags, UiFlags::ColorUiGold))
		return ColorUiGold;
	else if (HasAnyOf(flags, UiFlags::ColorUiSilver))
		return ColorUiSilver;
	else if (HasAnyOf(flags, UiFlags::ColorUiGoldDark))
		return ColorUiGoldDark;
	else if (HasAnyOf(flags, UiFlags::ColorUiSilverDark))
		return ColorUiSilverDark;
	else if (HasAnyOf(flags, UiFlags::ColorButtonface))
		return ColorButtonface;
	else if (HasAnyOf(flags, UiFlags::ColorButtonpushed))
		return ColorButtonpushed;

	return ColorWhitegold;
}

std::array<uint8_t, 256> *LoadFontKerning(GameFontTables size, uint16_t row)
{
	uint32_t fontId = (size << 16) | row;

	auto hotKerning = FontKerns.find(fontId);
	if (hotKerning != FontKerns.end()) {
		return &hotKerning->second;
	}

	char path[32];
	sprintf(path, "fonts\\%i-%02d.bin", FontSizes[size], row);

	auto *kerning = &FontKerns[fontId];

	LoadFileInMem(path, kerning);

	return kerning;
}

Art *LoadFont(GameFontTables size, text_color color, uint16_t row)
{
	uint32_t fontId = (color << 24) | (size << 16) | row;

	auto hotFont = Fonts.find(fontId);
	if (hotFont != Fonts.end()) {
		return &hotFont->second;
	}

	char path[32];
	sprintf(path, "fonts\\%i-%02d.pcx", FontSizes[size], row);

	auto *font = &Fonts[fontId];

	if (ColorTranlations[color] != nullptr) {
		std::array<uint8_t, 256> colorMapping;
		LoadFileInMem(ColorTranlations[color], colorMapping);
		LoadMaskedArt(path, font, 256, 1, &colorMapping);
	} else {
		LoadMaskedArt(path, font, 256, 1);
	}

	return font;
}

} // namespace

void UnloadFonts(GameFontTables size, text_color color)
{
	uint32_t fontStyle = (color << 24) | (size << 16);

	for (auto font = Fonts.begin(); font != Fonts.end();) {
		if ((font->first & 0xFFFF0000) == fontStyle) {
			font = Fonts.erase(font);
		} else {
			font++;
		}
	}
}

void UnloadFonts()
{
	Fonts.clear();
	FontKerns.clear();
}

int GetLineWidth(string_view text, GameFontTables size, int spacing, int *charactersInLine)
{
	int lineWidth = 0;

	std::string textBuffer(text);
	textBuffer.resize(textBuffer.size() + 4); // Buffer must be padded before calling utf8_decode()
	const char *textData = textBuffer.data();

	size_t i = 0;
	uint32_t currentUnicodeRow = 0;
	std::array<uint8_t, 256> *kerning = nullptr;
	uint32_t next;
	int error;
	for (; *textData != '\0'; i++) {
		textData = utf8_decode(textData, &next, &error);
		if (error)
			next = '?';

		if (next == '\n')
			break;

		uint8_t frame = next & 0xFF;
		uint32_t unicodeRow = next >> 8;
		if (unicodeRow != currentUnicodeRow || kerning == nullptr) {
			kerning = LoadFontKerning(size, unicodeRow);
			if (kerning == nullptr) {
				continue;
			}
			currentUnicodeRow = unicodeRow;
		}
		lineWidth += (*kerning)[frame] + spacing;
		i++;
	}

	if (charactersInLine != nullptr)
		*charactersInLine = i;

	return lineWidth != 0 ? (lineWidth - spacing) : 0;
}

int AdjustSpacingToFitHorizontally(int &lineWidth, int maxSpacing, int charactersInLine, int availableWidth)
{
	if (lineWidth <= availableWidth || charactersInLine < 2)
		return maxSpacing;

	const int overhang = lineWidth - availableWidth;
	const int spacingRedux = (overhang + charactersInLine - 2) / (charactersInLine - 1);
	lineWidth -= spacingRedux * (charactersInLine - 1);
	return maxSpacing - spacingRedux;
}

void WordWrapString(char *text, size_t width, GameFontTables size, int spacing)
{
	int lastKnownSpaceAt = -1;
	size_t lineWidth = 0;

	std::string textBuffer(text);
	textBuffer.resize(textBuffer.size() + 4); // Buffer must be padded before calling utf8_decode()
	const char *textData = textBuffer.data();

	uint32_t currentUnicodeRow = 0;
	std::array<uint8_t, 256> *kerning = nullptr;
	uint32_t next;
	int error;
	while (*textData != '\0') {
		textData = utf8_decode(textData, &next, &error);
		if (error)
			next = '?';

		if (next == '\n') { // Existing line break, scan next line
			lastKnownSpaceAt = -1;
			lineWidth = 0;
			continue;
		}

		uint8_t frame = next & 0xFF;
		uint32_t unicodeRow = next >> 8;
		if (unicodeRow != currentUnicodeRow || kerning == nullptr) {
			kerning = LoadFontKerning(size, unicodeRow);
			if (kerning == nullptr) {
				continue;
			}
			currentUnicodeRow = unicodeRow;
		}
		lineWidth += (*kerning)[frame] + spacing;

		if (next == ' ') {
			lastKnownSpaceAt = textData - textBuffer.data() - 1;
			continue;
		}

		if (lineWidth - spacing <= width) {
			continue; // String is still within the limit, continue to the next symbol
		}

		if (lastKnownSpaceAt == -1) { // Single word longer than width
			continue;
		}

		// Break line and continue to next line
		text[lastKnownSpaceAt] = '\n';
		textData = &textBuffer.data()[lastKnownSpaceAt + 1];
		lastKnownSpaceAt = -1;
		lineWidth = 0;
	}
}

/**
 * @todo replace Rectangle with cropped Surface
 */
uint32_t DrawString(const Surface &out, string_view text, const Rectangle &rect, UiFlags flags, int spacing, int lineHeight)
{
	GameFontTables size = GetSizeFromFlags(flags);
	text_color color = GetColorFromFlags(flags);

	int charactersInLine = 0;
	int lineWidth = 0;
	if (HasAnyOf(flags, (UiFlags::AlignCenter | UiFlags::AlignRight | UiFlags::KerningFitSpacing)))
		lineWidth = GetLineWidth(text, size, spacing, &charactersInLine);

	int maxSpacing = spacing;
	if (HasAnyOf(flags, UiFlags::KerningFitSpacing))
		spacing = AdjustSpacingToFitHorizontally(lineWidth, maxSpacing, charactersInLine, rect.size.width);

	Point characterPosition = rect.position;
	if (HasAnyOf(flags, UiFlags::AlignCenter))
		characterPosition.x += (rect.size.width - lineWidth) / 2;
	else if (HasAnyOf(flags, UiFlags::AlignRight))
		characterPosition.x += rect.size.width - lineWidth;

	int rightMargin = rect.position.x + rect.size.width;
	int bottomMargin = rect.size.height != 0 ? rect.position.y + rect.size.height : out.h();

	if (lineHeight == -1)
		lineHeight = LineHeights[size];

	if (HasAnyOf(flags, UiFlags::VerticalCenter)) {
		int textHeight = (std::count(text.cbegin(), text.cend(), '\n') + 1) * lineHeight;
		characterPosition.y += (rect.size.height - textHeight) / 2;
	}

	characterPosition.y += BaseLineOffset[size];

	Art *font = nullptr;
	std::array<uint8_t, 256> *kerning = nullptr;

	std::string textBuffer(text);
	textBuffer.resize(textBuffer.size() + 4); // Buffer must be padded before calling utf8_decode()
	const char *textData = textBuffer.data();
	const char *previousPosition = textData;

	uint32_t next;
	uint32_t currentUnicodeRow = 0;
	int error;
	for (; *textData != '\0'; previousPosition = textData) {
		textData = utf8_decode(textData, &next, &error);
		if (error)
			next = '?';

		uint32_t unicodeRow = next >> 8;
		if (unicodeRow != currentUnicodeRow || font == nullptr) {
			kerning = LoadFontKerning(size, unicodeRow);
			font = LoadFont(size, color, unicodeRow);
			currentUnicodeRow = unicodeRow;
		}

		uint8_t frame = next & 0xFF;
		if (next == '\n' || characterPosition.x > rightMargin) {
			if (characterPosition.y + lineHeight >= bottomMargin)
				break;
			characterPosition.x = rect.position.x;
			characterPosition.y += lineHeight;

			if (HasAnyOf(flags, (UiFlags::AlignCenter | UiFlags::AlignRight))) {
				lineWidth = (*kerning)[frame];
				if (*textData != '\0')
					lineWidth += spacing + GetLineWidth(textData, size, spacing);
			}

			if (HasAnyOf(flags, UiFlags::AlignCenter))
				characterPosition.x += (rect.size.width - lineWidth) / 2;
			else if (HasAnyOf(flags, UiFlags::AlignRight))
				characterPosition.x += rect.size.width - lineWidth;

			if (next == '\n')
				continue;
		}

		DrawArt(out, characterPosition, font, frame);
		characterPosition.x += (*kerning)[frame] + spacing;
	}

	if (HasAnyOf(flags, UiFlags::PentaCursor)) {
		CelDrawTo(out, characterPosition + Displacement { 0, lineHeight - BaseLineOffset[size] }, *pSPentSpn2Cels, PentSpn2Spin());
	} else if (HasAnyOf(flags, UiFlags::TextCursor) && GetAnimationFrame(2, 500) != 0) {
		DrawArt(out, characterPosition, LoadFont(size, color, 0), '|');
	}

	return previousPosition - textBuffer.data();
}

uint8_t PentSpn2Spin()
{
	return (SDL_GetTicks() / 50) % 8 + 1;
}

} // namespace devilution
