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

namespace devilution {

namespace {

std::unordered_map<uint32_t, std::unique_ptr<Art>> Fonts;

std::array<std::array<uint8_t, 256>, 5> FontKerns;
std::array<int, 5> FontSizes = { 12, 24, 30, 42, 46 };
std::array<int, 5> LineHeights = { 12, 26, 38, 42, 50 };
std::array<int, 5> BaseLineOffset = { -3, -2, -3, -6, -7 };

} // namespace

void LoadFont(GameFontTables size, text_color color, const char *translationFile)
{
	auto font = std::make_unique<Art>();

	char path[32];
	sprintf(path, "fonts\\%i-00.pcx", FontSizes[size]);

	if (translationFile != nullptr) {
		std::array<uint8_t, 256> colorMapping;
		LoadFileInMem(translationFile, colorMapping);
		LoadMaskedArt(path, font.get(), 256, 1, &colorMapping);
	} else {
		LoadMaskedArt(path, font.get(), 256, 1);
	}

	uint32_t fontId = (color << 24) | (size << 16);
	Fonts.insert(make_pair(fontId, move(font)));

	sprintf(path, "fonts\\%i-00.bin", FontSizes[size]);
	LoadFileInMem(path, FontKerns[size]);
}

void UnloadFonts()
{
	Fonts.clear();
}

int GetLineWidth(string_view text, GameFontTables size, int spacing, int *charactersInLine)
{
	int lineWidth = 0;

	size_t i = 0;
	for (; i < text.length(); i++) {
		if (text[i] == '\n')
			break;

		uint8_t frame = text[i] & 0xFF;
		lineWidth += FontKerns[size][frame] + spacing;
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
	const size_t textLength = strlen(text);
	size_t lineStart = 0;
	size_t lineWidth = 0;
	for (unsigned i = 0; i < textLength; i++) {
		if (text[i] == '\n') { // Existing line break, scan next line
			lineStart = i + 1;
			lineWidth = 0;
			continue;
		}

		uint8_t frame = text[i] & 0xFF;
		lineWidth += FontKerns[size][frame] + spacing;

		if (lineWidth - spacing <= width) {
			continue; // String is still within the limit, continue to the next line
		}

		size_t j; // Backtrack to the previous space
		for (j = i; j >= lineStart; j--) {
			if (text[j] == ' ') {
				break;
			}
		}

		if (j == lineStart) { // Single word longer than width
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
 * @todo replace Rectangle with cropped Surface
 */
uint32_t DrawString(const Surface &out, string_view text, const Rectangle &rect, UiFlags flags, int spacing, int lineHeight)
{
	GameFontTables size = GameFont12;
	if (HasAnyOf(flags, UiFlags::FontSize24))
		size = GameFont24;
	else if (HasAnyOf(flags, UiFlags::FontSize30))
		size = GameFont30;
	else if (HasAnyOf(flags, UiFlags::FontSize42))
		size = GameFont42;
	else if (HasAnyOf(flags, UiFlags::FontSize46))
		size = GameFont46;

	text_color color = ColorGold;
	if (HasAnyOf(flags, UiFlags::ColorSilver))
		color = ColorSilver;
	else if (HasAnyOf(flags, UiFlags::ColorBlue))
		color = ColorBlue;
	else if (HasAnyOf(flags, UiFlags::ColorRed))
		color = ColorRed;
	else if (HasAnyOf(flags, UiFlags::ColorBlack))
		color = ColorBlack;

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

	int bottomMargin = rect.size.height != 0 ? rect.position.y + rect.size.height : out.h();

	if (lineHeight == -1)
		lineHeight = LineHeights[size];

	if (HasAnyOf(flags, UiFlags::VerticalCenter)) {
		int textHeight = (std::count(text.cbegin(), text.cend(), '\n') + 1) * lineHeight;
		characterPosition.y += (rect.size.height - textHeight) / 2;
	}

	characterPosition.y += BaseLineOffset[size];

	uint32_t fontId = (color << 24) | (size << 16);
	auto font = Fonts.find(fontId);
	if (font == Fonts.end()) {
		Log("Font: size {} and color {} not loaded ", size, color);
		return 0;
	}

	const auto &activeFont = font->second;

	uint32_t i = 0;
	for (; i < text.length(); i++) {
		uint8_t frame = text[i] & 0xFF;
		if (text[i] == '\n') {
			if (characterPosition.y + lineHeight >= bottomMargin)
				break;
			characterPosition.y += lineHeight;

			if (HasAnyOf(flags, (UiFlags::AlignCenter | UiFlags::AlignRight | UiFlags::KerningFitSpacing)))
				lineWidth = GetLineWidth(&text[i + 1], size, spacing, &charactersInLine);

			if (HasAnyOf(flags, UiFlags::KerningFitSpacing))
				spacing = AdjustSpacingToFitHorizontally(lineWidth, maxSpacing, charactersInLine, rect.size.width);

			characterPosition.x = rect.position.x;
			if (HasAnyOf(flags, UiFlags::AlignCenter))
				characterPosition.x += (rect.size.width - lineWidth) / 2;
			else if (HasAnyOf(flags, UiFlags::AlignRight))
				characterPosition.x += rect.size.width - lineWidth;
		}
		DrawArt(out, characterPosition, activeFont.get(), frame);
		if (text[i] != '\n')
			characterPosition.x += FontKerns[size][frame] + spacing;
	}
	if (HasAnyOf(flags, UiFlags::PentaCursor)) {
		CelDrawTo(out, characterPosition + Displacement { 0, lineHeight - BaseLineOffset[size] }, *pSPentSpn2Cels, PentSpn2Spin());
	} else if (HasAnyOf(flags, UiFlags::TextCursor) && GetAnimationFrame(2, 500) != 0) {
		DrawArt(out, characterPosition, activeFont.get(), '|');
	}

	return i;
}

uint8_t PentSpn2Spin()
{
	return (SDL_GetTicks() / 50) % 8 + 1;
}

} // namespace devilution
