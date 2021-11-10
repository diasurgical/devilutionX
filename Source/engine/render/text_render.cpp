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
#include "utils/utf8.hpp"

namespace devilution {

namespace {

constexpr char32_t ZWSP = U'\u200B'; // Zero-width space

std::unordered_map<uint32_t, Art> Fonts;
std::unordered_map<uint32_t, std::array<uint8_t, 256>> FontKerns;
std::array<int, 6> FontSizes = { 12, 24, 30, 42, 46, 22 };
std::array<uint8_t, 6> FontFullwidth = { 16, 21, 29, 41, 43, 16 };
std::array<int, 6> LineHeights = { 12, 26, 38, 42, 50, 22 };
std::array<int, 6> BaseLineOffset = { -3, -2, -3, -6, -7, 3 };

std::array<const char *, 14> ColorTranlations = {
	"fonts\\goldui.trn",
	"fonts\\grayui.trn",
	"fonts\\golduis.trn",
	"fonts\\grayuis.trn",

	nullptr,
	"fonts\\yellowdialog.trn",

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
	else if (HasAnyOf(flags, UiFlags::FontSizeDialog))
		return FontSizeDialog;

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
	else if (HasAnyOf(flags, UiFlags::ColorDialogWhite))
		return ColorDialogWhite;
	else if (HasAnyOf(flags, UiFlags::ColorDialogYellow))
		return ColorDialogYellow;
	else if (HasAnyOf(flags, UiFlags::ColorButtonface))
		return ColorButtonface;
	else if (HasAnyOf(flags, UiFlags::ColorButtonpushed))
		return ColorButtonpushed;

	return ColorWhitegold;
}

bool IsFullWidth(uint16_t row)
{
	if (row >= 0x4e && row <= 0x9f)
		return true; // CJK Unified Ideographs
	if (row >= 0xac && row <= 0xd7)
		return true; // Hangul Syllables
	return false;
}

std::array<uint8_t, 256> *LoadFontKerning(GameFontTables size, uint16_t row)
{
	uint32_t fontId = (size << 16) | row;

	auto hotKerning = FontKerns.find(fontId);
	if (hotKerning != FontKerns.end()) {
		return &hotKerning->second;
	}

	char path[32];
	sprintf(path, "fonts\\%i-%02x.bin", FontSizes[size], row);

	auto *kerning = &FontKerns[fontId];

	if (IsFullWidth(row)) {
		kerning->fill(FontFullwidth[size]);
	} else {
		SDL_RWops *handle = OpenAsset(path);
		if (handle != nullptr) {
			SDL_RWread(handle, kerning, 256, 1);
			SDL_RWclose(handle);
		} else {
			LogError("Missing font kerning: {}", path);
			kerning->fill(FontFullwidth[size]);
		}
	}

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
	sprintf(path, "fonts\\%i-%02x.pcx", FontSizes[size], row);

	auto *font = &Fonts[fontId];

	if (ColorTranlations[color] != nullptr) {
		std::array<uint8_t, 256> colorMapping;
		LoadFileInMem(ColorTranlations[color], colorMapping);
		LoadMaskedArt(path, font, 256, 1, &colorMapping);
	} else {
		LoadMaskedArt(path, font, 256, 1);
	}
	if (font->surface == nullptr) {
		LogError("Missing font: {}", path);
	}

	return font;
}

bool IsWhitespace(char32_t c)
{
	return IsAnyOf(c, U' ', U'　', ZWSP);
}

bool IsFullWidthPunct(char32_t c)
{
	return IsAnyOf(c, U'，', U'、', U'。', U'？', U'！');
}

bool IsBreakAllowed(char32_t codepoint, char32_t nextCodepoint)
{
	return IsFullWidthPunct(codepoint) && !IsFullWidthPunct(nextCodepoint);
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

	uint32_t codepoints = 0;
	uint32_t currentUnicodeRow = 0;
	std::array<uint8_t, 256> *kerning = nullptr;
	char32_t next;
	while (!text.empty()) {
		next = ConsumeFirstUtf8CodePoint(&text);
		if (next == Utf8DecodeError)
			break;
		if (next == ZWSP)
			continue;

		if (next == '\n')
			break;

		uint8_t frame = next & 0xFF;
		uint32_t unicodeRow = next >> 8;
		if (unicodeRow != currentUnicodeRow || kerning == nullptr) {
			kerning = LoadFontKerning(size, unicodeRow);
			currentUnicodeRow = unicodeRow;
		}
		lineWidth += (*kerning)[frame] + spacing;
		codepoints++;
	}
	if (charactersInLine != nullptr)
		*charactersInLine = codepoints;

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

std::string WordWrapString(string_view text, size_t width, GameFontTables size, int spacing)
{
	std::string output;
	if (text.empty() || text[0] == '\0')
		return output;

	output.reserve(text.size());
	const char *begin = text.data();
	const char *processedEnd = text.data();
	int lastBreakablePos = -1;
	int lastBreakableLen;
	bool lastBreakableKeep = false;
	uint32_t currentUnicodeRow = 0;
	size_t lineWidth = 0;
	std::array<uint8_t, 256> *kerning = nullptr;

	char32_t codepoint = U'\0'; // the current codepoint
	char32_t nextCodepoint;     // the next codepoint
	uint8_t nextCodepointLen;
	string_view remaining = text;
	nextCodepoint = DecodeFirstUtf8CodePoint(remaining, &nextCodepointLen);
	do {
		codepoint = nextCodepoint;
		const uint8_t codepointLen = nextCodepointLen;
		if (codepoint == Utf8DecodeError)
			break;
		remaining.remove_prefix(codepointLen);
		nextCodepoint = !remaining.empty() ? DecodeFirstUtf8CodePoint(remaining, &nextCodepointLen) : U'\0';

		if (codepoint == U'\n') { // Existing line break, scan next line
			lastBreakablePos = -1;
			lineWidth = 0;
			output.append(processedEnd, remaining.data());
			processedEnd = remaining.data();
			continue;
		}

		if (codepoint != ZWSP) {
			uint8_t frame = codepoint & 0xFF;
			uint32_t unicodeRow = codepoint >> 8;
			if (unicodeRow != currentUnicodeRow || kerning == nullptr) {
				kerning = LoadFontKerning(size, unicodeRow);
				currentUnicodeRow = unicodeRow;
			}
			lineWidth += (*kerning)[frame] + spacing;
		}

		const bool isWhitespace = IsWhitespace(codepoint);
		if (isWhitespace || IsBreakAllowed(codepoint, nextCodepoint)) {
			lastBreakablePos = static_cast<int>(remaining.data() - begin - codepointLen);
			lastBreakableLen = codepointLen;
			lastBreakableKeep = !isWhitespace;
			continue;
		}

		if (lineWidth - spacing <= width) {
			continue; // String is still within the limit, continue to the next symbol
		}

		if (lastBreakablePos == -1) { // Single word longer than width
			continue;
		}

		// Break line and continue to next line
		const char *end = &text[lastBreakablePos];
		if (lastBreakableKeep) {
			end += lastBreakableLen;
		}
		output.append(processedEnd, end);
		output += '\n';

		// Restart from the beginning of the new line.
		remaining = text.substr(lastBreakablePos + lastBreakableLen);
		processedEnd = remaining.data();
		lastBreakablePos = -1;
		lineWidth = 0;
		nextCodepoint = !remaining.empty() ? DecodeFirstUtf8CodePoint(remaining, &nextCodepointLen) : U'\0';
	} while (!remaining.empty() && remaining[0] != '\0');
	output.append(processedEnd, remaining.data());
	return output;
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
	const int bottomMargin = rect.size.height != 0 ? std::min(rect.position.y + rect.size.height, out.h()) : out.h();

	if (lineHeight == -1)
		lineHeight = LineHeights[size];

	if (HasAnyOf(flags, UiFlags::VerticalCenter)) {
		int textHeight = (std::count(text.cbegin(), text.cend(), '\n') + 1) * lineHeight;
		characterPosition.y += (rect.size.height - textHeight) / 2;
	}

	characterPosition.y += BaseLineOffset[size];

	Art *font = nullptr;
	std::array<uint8_t, 256> *kerning = nullptr;

	char32_t next;
	uint32_t currentUnicodeRow = 0;
	string_view remaining = text;
	while (!remaining.empty() && remaining[0] != '\0') {
		next = ConsumeFirstUtf8CodePoint(&remaining);
		if (next == Utf8DecodeError)
			break;
		if (next == ZWSP)
			continue;

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
				if (!remaining.empty())
					lineWidth += spacing + GetLineWidth(remaining, size, spacing);
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

	return text.data() - remaining.data();
}

uint8_t PentSpn2Spin()
{
	return (SDL_GetTicks() / 50) % 8 + 1;
}

} // namespace devilution
