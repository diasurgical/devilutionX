/**
 * @file text_render.cpp
 *
 * Text rendering.
 */
#include "text_render.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>

#include <fmt/core.h>

#include "DiabloUI/diabloui.h"
#include "DiabloUI/ui_item.h"
#include "engine.h"
#include "engine/load_cel.hpp"
#include "engine/load_clx.hpp"
#include "engine/load_file.hpp"
#include "engine/load_pcx.hpp"
#include "engine/palette.h"
#include "engine/point.hpp"
#include "engine/render/clx_render.hpp"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/sdl_compat.h"
#include "utils/stdcompat/optional.hpp"
#include "utils/utf8.hpp"

namespace devilution {

OptionalOwnedClxSpriteList pSPentSpn2Cels;

namespace {

constexpr char32_t ZWSP = U'\u200B'; // Zero-width space

std::unordered_map<uint32_t, OptionalOwnedClxSpriteList> Fonts;

std::array<int, 6> FontSizes = { 12, 24, 30, 42, 46, 22 };
constexpr std::array<int, 6> LineHeights = { 12, 26, 38, 42, 50, 22 };
constexpr int SmallFontTallLineHeight = 16;
std::array<int, 6> BaseLineOffset = { -3, -2, -3, -6, -7, 3 };

std::array<const char *, 15> ColorTranslations = {
	"fonts\\goldui.trn",
	"fonts\\grayui.trn",
	"fonts\\golduis.trn",
	"fonts\\grayuis.trn",

	nullptr,
	"fonts\\yellow.trn",

	nullptr,
	"fonts\\black.trn",

	"fonts\\white.trn",
	"fonts\\whitegold.trn",
	"fonts\\red.trn",
	"fonts\\blue.trn",
	"fonts\\orange.trn",

	"fonts\\buttonface.trn",
	"fonts\\buttonpushed.trn",
};

std::array<std::optional<std::array<uint8_t, 256>>, 15> ColorTranslationsData;

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
	else if (HasAnyOf(flags, UiFlags::ColorOrange))
		return ColorOrange;
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
	else if (HasAnyOf(flags, UiFlags::ColorYellow))
		return ColorYellow;
	else if (HasAnyOf(flags, UiFlags::ColorButtonface))
		return ColorButtonface;
	else if (HasAnyOf(flags, UiFlags::ColorButtonpushed))
		return ColorButtonpushed;

	return ColorWhitegold;
}

uint16_t GetUnicodeRow(char32_t codePoint)
{
	return static_cast<uint32_t>(codePoint) >> 8;
}

bool IsCJK(uint16_t row)
{
	return row >= 0x30 && row <= 0x9f;
}

bool IsHangul(uint16_t row)
{
	return row >= 0xac && row <= 0xd7;
}

bool IsSmallFontTallRow(uint16_t row)
{
	return IsCJK(row) || IsHangul(row);
}

void GetFontPath(GameFontTables size, uint16_t row, string_view ext, char *out)
{
	*fmt::format_to(out, R"(fonts\{}-{:02x}{})", FontSizes[size], row, ext) = '\0';
}

uint32_t GetFontId(GameFontTables size, uint16_t row)
{
	return (size << 16) | row;
}

OptionalClxSpriteList LoadFont(GameFontTables size, text_color color, uint16_t row)
{
	if (ColorTranslations[color] != nullptr && !ColorTranslationsData[color]) {
		ColorTranslationsData[color].emplace();
		LoadFileInMem(ColorTranslations[color], *ColorTranslationsData[color]);
	}

	const uint32_t fontId = GetFontId(size, row);
	auto hotFont = Fonts.find(fontId);
	if (hotFont != Fonts.end()) {
		return OptionalClxSpriteList(*hotFont->second);
	}

	char path[32];
	GetFontPath(size, row, ".clx", &path[0]);

	OptionalOwnedClxSpriteList &font = Fonts[fontId];
	font = LoadOptionalClx(path);
	if (!font) {
		// Could be an old devilutionx.mpq or fonts.mpq with PCX instead of CLX.
		//
		// We'll show an error elsewhere (in `CheckArchivesUpToDate`) and we need to load
		// the font files to display it.
		char pcxPath[32];
		GetFontPath(size, row, "", &pcxPath[0]);
		font = LoadPcxSpriteList(pcxPath, /*numFramesOrFrameHeight=*/256, /*transparentColor=*/1);
	}

	if (!font) {
		LogError("Error loading font: {}", path);
	}

	return OptionalClxSpriteList(*font);
}

class CurrentFont {
public:
	OptionalClxSpriteList sprite;

	bool load(GameFontTables size, text_color color, char32_t next)
	{
		const uint32_t unicodeRow = GetUnicodeRow(next);
		if (unicodeRow == currentUnicodeRow_ && hasAttemptedLoad_) {
			return true;
		}

		sprite = LoadFont(size, color, unicodeRow);
		hasAttemptedLoad_ = true;
		currentUnicodeRow_ = unicodeRow;

		return sprite;
	}

	void clear()
	{
		hasAttemptedLoad_ = false;
	}

private:
	bool hasAttemptedLoad_ = false;
	uint32_t currentUnicodeRow_ = 0;
};

void DrawFont(const Surface &out, Point position, const ClxSpriteList font, text_color color, int frame, bool outline)
{
	ClxSprite glyph = font[frame];
	if (outline) {
		ClxDrawOutlineSkipColorZero(out, 0, { position.x, position.y + glyph.height() - 1 }, glyph);
	}
	if (ColorTranslationsData[color]) {
		RenderClxSpriteWithTRN(out, glyph, position, ColorTranslationsData[color]->data());
	} else {
		RenderClxSprite(out, glyph, position);
	}
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

std::size_t CountNewlines(string_view fmt, const DrawStringFormatArg *args, std::size_t argsLen)
{
	std::size_t result = std::count(fmt.begin(), fmt.end(), '\n');
	for (std::size_t i = 0; i < argsLen; ++i) {
		if (args[i].GetType() == DrawStringFormatArg::Type::StringView)
			result += std::count(args[i].GetFormatted().begin(), args[i].GetFormatted().end(), '\n');
	}
	return result;
}

class FmtArgParser {
public:
	FmtArgParser(string_view fmt,
	    DrawStringFormatArg *args,
	    size_t len,
	    size_t offset = 0)
	    : fmt_(fmt)
	    , args_(args)
	    , len_(len)
	    , next_(offset)
	{
	}

	std::optional<std::size_t> operator()(string_view &rest)
	{
		std::optional<std::size_t> result;
		if (rest[0] != '{')
			return result;

		std::size_t closingBracePos = rest.find('}', 1);
		if (closingBracePos == string_view::npos) {
			LogError("Unclosed format argument: {}", fmt_);
			return result;
		}

		std::size_t fmtLen;
		bool positional;
		if (closingBracePos == 2 && rest[1] >= '0' && rest[1] <= '9') {
			result = rest[1] - '0';
			fmtLen = 3;
			positional = true;
		} else {
			result = next_++;
			fmtLen = closingBracePos + 1;
			positional = false;
		}
		if (!result) {
			LogError("Unsupported format argument: {}", rest);
		} else if (*result >= len_) {
			LogError("Not enough format arguments, {} given for: {}", len_, fmt_);
			result = std::nullopt;
		} else {
			if (!args_[*result].HasFormatted()) {
				const auto fmtStr = positional ? "{}" : fmt::string_view(rest.data(), fmtLen);
				args_[*result].SetFormatted(fmt::format(fmt::runtime(fmtStr), args_[*result].GetIntValue()));
			}
			rest.remove_prefix(fmtLen);
		}
		return result;
	}

	size_t offset() const
	{
		return next_;
	}

private:
	string_view fmt_;
	DrawStringFormatArg *args_;
	std::size_t len_;
	std::size_t next_;
};

bool ContainsSmallFontTallCodepoints(string_view text)
{
	while (!text.empty()) {
		const char32_t next = ConsumeFirstUtf8CodePoint(&text);
		if (next == Utf8DecodeError)
			break;
		if (next == ZWSP)
			continue;
		if (IsSmallFontTallRow(GetUnicodeRow(next)))
			return true;
	}
	return false;
}

int GetLineHeight(string_view fmt, DrawStringFormatArg *args, std::size_t argsLen, GameFontTables fontIndex)
{
	constexpr std::array<int, 6> LineHeights = { 12, 26, 38, 42, 50, 22 };
	if (fontIndex == GameFont12 && IsSmallFontTall()) {
		char32_t prev = U'\0';
		char32_t next;
		FmtArgParser fmtArgParser { fmt, args, argsLen };
		string_view rest = fmt;
		while (!rest.empty()) {
			if ((prev == U'{' || prev == U'}') && static_cast<char>(prev) == rest[0]) {
				rest.remove_prefix(1);
				continue;
			}
			const std::optional<std::size_t> fmtArgPos = fmtArgParser(rest);
			if (fmtArgPos) {
				if (ContainsSmallFontTallCodepoints(args[*fmtArgPos].GetFormatted()))
					return SmallFontTallLineHeight;
				prev = U'\0';
				continue;
			}

			next = ConsumeFirstUtf8CodePoint(&rest);
			if (next == Utf8DecodeError)
				break;
			if (next == ZWSP) {
				prev = next;
				continue;
			}
			if (IsSmallFontTallRow(GetUnicodeRow(next)))
				return SmallFontTallLineHeight;
		}
	}
	return LineHeights[fontIndex];
}

Surface ClipSurface(const Surface &out, Rectangle rect)
{
	if (rect.size.height == 0) {
		return out.subregion(0, 0, std::min(rect.position.x + rect.size.width, out.w()), out.h());
	}
	return out.subregion(0, 0,
	    std::min(rect.position.x + rect.size.width, out.w()),
	    std::min(rect.position.y + rect.size.height, out.h()));
}

void MaybeWrap(Point &characterPosition, int characterWidth, int rightMargin, int initialX, int lineHeight)
{
	if (characterPosition.x + characterWidth > rightMargin) {
		characterPosition.x = initialX;
		characterPosition.y += lineHeight;
	}
}

int GetLineStartX(UiFlags flags, const Rectangle &rect, int lineWidth)
{
	if (HasAnyOf(flags, UiFlags::AlignCenter))
		return rect.position.x + (rect.size.width - lineWidth) / 2;
	if (HasAnyOf(flags, UiFlags::AlignRight))
		return rect.position.x + rect.size.width - lineWidth;
	return rect.position.x;
}

uint32_t DoDrawString(const Surface &out, string_view text, Rectangle rect, Point &characterPosition,
    int spacing, int lineHeight, int lineWidth, int rightMargin, int bottomMargin,
    UiFlags flags, GameFontTables size, text_color color, bool outline)
{
	CurrentFont currentFont;

	char32_t next;
	string_view remaining = text;
	size_t cpLen;
	for (; !remaining.empty() && remaining[0] != '\0'
	     && (next = DecodeFirstUtf8CodePoint(remaining, &cpLen)) != Utf8DecodeError;
	     remaining.remove_prefix(cpLen)) {
		if (next == ZWSP)
			continue;

		if (!currentFont.load(size, color, next)) {
			next = U'?';
			if (!currentFont.load(size, color, next)) {
				app_fatal("Missing fonts");
			}
		}

		const uint8_t frame = next & 0xFF;
		const uint16_t width = (*currentFont.sprite)[frame].width();
		if (next == U'\n' || characterPosition.x + width > rightMargin) {
			const int nextLineY = characterPosition.y + lineHeight;
			if (nextLineY >= bottomMargin)
				break;
			characterPosition.y = nextLineY;

			if (HasAnyOf(flags, (UiFlags::AlignCenter | UiFlags::AlignRight))) {
				lineWidth = width;
				if (remaining.size() > cpLen)
					lineWidth += spacing + GetLineWidth(remaining.substr(cpLen), size, spacing);
			}
			characterPosition.x = GetLineStartX(flags, rect, lineWidth);

			if (next == U'\n')
				continue;
		}

		DrawFont(out, characterPosition, *currentFont.sprite, color, frame, outline);
		characterPosition.x += width + spacing;
	}
	return remaining.data() - text.data();
}

} // namespace

void LoadSmallSelectionSpinner()
{
	pSPentSpn2Cels = LoadCel("data\\pentspn2", 12);
}

void UnloadFonts()
{
	Fonts.clear();
}

int GetLineWidth(string_view text, GameFontTables size, int spacing, int *charactersInLine)
{
	int lineWidth = 0;
	CurrentFont currentFont;
	uint32_t codepoints = 0;
	char32_t next;
	while (!text.empty()) {
		next = ConsumeFirstUtf8CodePoint(&text);
		if (next == Utf8DecodeError)
			break;
		if (next == ZWSP)
			continue;

		if (next == U'\n')
			break;

		if (!currentFont.load(size, text_color::ColorDialogWhite, next)) {
			next = U'?';
			if (!currentFont.load(size, text_color::ColorDialogWhite, next)) {
				app_fatal("Missing fonts");
			}
		}

		const uint8_t frame = next & 0xFF;
		lineWidth += (*currentFont.sprite)[frame].width() + spacing;
		++codepoints;
	}
	if (charactersInLine != nullptr)
		*charactersInLine = codepoints;

	return lineWidth != 0 ? (lineWidth - spacing) : 0;
}

int GetLineWidth(string_view fmt, DrawStringFormatArg *args, std::size_t argsLen, size_t argsOffset, GameFontTables size, int spacing, int *charactersInLine)
{
	int lineWidth = 0;
	CurrentFont currentFont;

	uint32_t codepoints = 0;
	char32_t prev = U'\0';
	char32_t next;

	FmtArgParser fmtArgParser { fmt, args, argsLen, argsOffset };
	string_view rest = fmt;
	while (!rest.empty()) {
		if ((prev == U'{' || prev == U'}') && static_cast<char>(prev) == rest[0]) {
			rest.remove_prefix(1);
			continue;
		}
		const std::optional<std::size_t> fmtArgPos = fmtArgParser(rest);
		if (fmtArgPos) {
			int argCodePoints;
			lineWidth += GetLineWidth(args[*fmtArgPos].GetFormatted(), size, spacing, &argCodePoints);
			codepoints += argCodePoints;
			prev = U'\0';
			continue;
		}

		next = ConsumeFirstUtf8CodePoint(&rest);
		if (next == Utf8DecodeError)
			break;
		if (next == ZWSP) {
			prev = next;
			continue;
		}
		if (next == U'\n')
			break;

		if (!currentFont.load(size, text_color::ColorDialogWhite, next)) {
			next = U'?';
			if (!currentFont.load(size, text_color::ColorDialogWhite, next)) {
				app_fatal("Missing fonts");
			}
		}

		const uint8_t frame = next & 0xFF;
		lineWidth += (*currentFont.sprite)[frame].width() + spacing;
		codepoints++;
		prev = next;
	}
	if (charactersInLine != nullptr)
		*charactersInLine = codepoints;

	return lineWidth != 0 ? (lineWidth - spacing) : 0;
}

int GetLineHeight(string_view text, GameFontTables fontIndex)
{
	if (fontIndex == GameFont12 && IsSmallFontTall() && ContainsSmallFontTallCodepoints(text)) {
		return SmallFontTallLineHeight;
	}
	return LineHeights[fontIndex];
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

std::string WordWrapString(string_view text, unsigned width, GameFontTables size, int spacing)
{
	std::string output;
	if (text.empty() || text[0] == '\0')
		return output;

	output.reserve(text.size());
	const char *begin = text.data();
	const char *processedEnd = text.data();
	string_view::size_type lastBreakablePos = string_view::npos;
	std::size_t lastBreakableLen;
	bool lastBreakableKeep = false;
	unsigned lineWidth = 0;
	CurrentFont currentFont;

	char32_t codepoint = U'\0'; // the current codepoint
	char32_t nextCodepoint;     // the next codepoint
	std::size_t nextCodepointLen;
	string_view remaining = text;
	nextCodepoint = DecodeFirstUtf8CodePoint(remaining, &nextCodepointLen);
	do {
		codepoint = nextCodepoint;
		const std::size_t codepointLen = nextCodepointLen;
		if (codepoint == Utf8DecodeError)
			break;
		remaining.remove_prefix(codepointLen);
		nextCodepoint = !remaining.empty() ? DecodeFirstUtf8CodePoint(remaining, &nextCodepointLen) : U'\0';

		if (codepoint == U'\n') { // Existing line break, scan next line
			lastBreakablePos = string_view::npos;
			lineWidth = 0;
			output.append(processedEnd, remaining.data());
			processedEnd = remaining.data();
			continue;
		}

		if (codepoint != ZWSP) {
			const uint8_t frame = codepoint & 0xFF;
			if (!currentFont.load(size, text_color::ColorDialogWhite, codepoint)) {
				codepoint = U'?';
				if (!currentFont.load(size, text_color::ColorDialogWhite, codepoint)) {
					app_fatal("Missing fonts");
				}
			}

			lineWidth += (*currentFont.sprite)[frame].width() + spacing;
		}

		const bool isWhitespace = IsWhitespace(codepoint);
		if (isWhitespace || IsBreakAllowed(codepoint, nextCodepoint)) {
			lastBreakablePos = remaining.data() - begin - codepointLen;
			lastBreakableLen = codepointLen;
			lastBreakableKeep = !isWhitespace;
			continue;
		}

		if (lineWidth - spacing <= width) {
			continue; // String is still within the limit, continue to the next symbol
		}

		if (lastBreakablePos == string_view::npos) { // Single word longer than width
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
		lastBreakablePos = string_view::npos;
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

	Point characterPosition { GetLineStartX(flags, rect, lineWidth), rect.position.y };
	const int initialX = characterPosition.x;

	const int rightMargin = rect.position.x + rect.size.width;
	const int bottomMargin = rect.size.height != 0 ? std::min(rect.position.y + rect.size.height + BaseLineOffset[size], out.h()) : out.h();

	if (lineHeight == -1)
		lineHeight = GetLineHeight(text, size);

	if (HasAnyOf(flags, UiFlags::VerticalCenter)) {
		int textHeight = (std::count(text.cbegin(), text.cend(), '\n') + 1) * lineHeight;
		characterPosition.y += std::max(0, (rect.size.height - textHeight) / 2);
	}

	characterPosition.y += BaseLineOffset[size];

	const bool outlined = HasAnyOf(flags, UiFlags::Outlined);

	const Surface clippedOut = ClipSurface(out, rect);

	const uint32_t bytesDrawn = DoDrawString(clippedOut, text, rect, characterPosition, spacing, lineHeight, lineWidth, rightMargin, bottomMargin, flags, size, color, outlined);

	if (HasAnyOf(flags, UiFlags::PentaCursor)) {
		const ClxSprite sprite = (*pSPentSpn2Cels)[PentSpn2Spin()];
		MaybeWrap(characterPosition, sprite.width(), rightMargin, initialX, lineHeight);
		ClxDraw(clippedOut, characterPosition + Displacement { 0, lineHeight - BaseLineOffset[size] }, sprite);
	} else if (HasAnyOf(flags, UiFlags::TextCursor) && GetAnimationFrame(2, 500) != 0) {
		MaybeWrap(characterPosition, 2, rightMargin, initialX, lineHeight);
		OptionalClxSpriteList baseFont = LoadFont(size, color, 0);
		if (baseFont)
			DrawFont(clippedOut, characterPosition, *baseFont, color, '|', outlined);
	}

	return bytesDrawn;
}

void DrawStringWithColors(const Surface &out, string_view fmt, DrawStringFormatArg *args, std::size_t argsLen, const Rectangle &rect, UiFlags flags, int spacing, int lineHeight)
{
	GameFontTables size = GetSizeFromFlags(flags);
	text_color color = GetColorFromFlags(flags);

	int charactersInLine = 0;
	int lineWidth = 0;
	if (HasAnyOf(flags, (UiFlags::AlignCenter | UiFlags::AlignRight | UiFlags::KerningFitSpacing)))
		lineWidth = GetLineWidth(fmt, args, argsLen, 0, size, spacing, &charactersInLine);

	int maxSpacing = spacing;
	if (HasAnyOf(flags, UiFlags::KerningFitSpacing))
		spacing = AdjustSpacingToFitHorizontally(lineWidth, maxSpacing, charactersInLine, rect.size.width);

	Point characterPosition { GetLineStartX(flags, rect, lineWidth), rect.position.y };
	const int initialX = characterPosition.x;

	const int rightMargin = rect.position.x + rect.size.width;
	const int bottomMargin = rect.size.height != 0 ? std::min(rect.position.y + rect.size.height + BaseLineOffset[size], out.h()) : out.h();

	if (lineHeight == -1)
		lineHeight = GetLineHeight(fmt, args, argsLen, size);

	if (HasAnyOf(flags, UiFlags::VerticalCenter)) {
		int textHeight = (CountNewlines(fmt, args, argsLen) + 1) * lineHeight;
		characterPosition.y += std::max(0, (rect.size.height - textHeight) / 2);
	}

	characterPosition.y += BaseLineOffset[size];

	const bool outlined = HasAnyOf(flags, UiFlags::Outlined);

	const Surface clippedOut = ClipSurface(out, rect);

	CurrentFont currentFont;

	char32_t prev = U'\0';
	char32_t next;
	string_view remaining = fmt;
	FmtArgParser fmtArgParser { fmt, args, argsLen };
	size_t cpLen;
	for (; !remaining.empty() && remaining[0] != '\0'
	     && (next = DecodeFirstUtf8CodePoint(remaining, &cpLen)) != Utf8DecodeError;
	     remaining.remove_prefix(cpLen), prev = next) {
		if (((prev == U'{' || prev == U'}') && prev == next)
		    || next == ZWSP)
			continue;

		const std::optional<std::size_t> fmtArgPos = fmtArgParser(remaining);
		if (fmtArgPos) {
			DoDrawString(clippedOut, args[*fmtArgPos].GetFormatted(), rect, characterPosition, spacing, lineHeight, lineWidth, rightMargin, bottomMargin, flags, size,
			    GetColorFromFlags(args[*fmtArgPos].GetFlags()), outlined);
			// `fmtArgParser` has already consumed `remaining`. Ensure the loop doesn't consume any more.
			cpLen = 0;
			// The loop assigns `prev = next`. We want `prev` to be `\0` after this.
			next = U'\0';
			currentFont.clear();
			continue;
		}

		if (!currentFont.load(size, color, next)) {
			next = U'?';
			if (!currentFont.load(size, color, next)) {
				app_fatal("Missing fonts");
			}
		}

		const uint8_t frame = next & 0xFF;
		const uint16_t width = (*currentFont.sprite)[frame].width();
		if (next == U'\n' || characterPosition.x + width > rightMargin) {
			const int nextLineY = characterPosition.y + lineHeight;
			if (nextLineY >= bottomMargin)
				break;
			characterPosition.y = nextLineY;

			if (HasAnyOf(flags, (UiFlags::AlignCenter | UiFlags::AlignRight))) {
				lineWidth = width;
				if (remaining.size() > cpLen)
					lineWidth += spacing + GetLineWidth(remaining.substr(cpLen), args, argsLen, fmtArgParser.offset(), size, spacing);
			}
			characterPosition.x = GetLineStartX(flags, rect, lineWidth);

			if (next == U'\n')
				continue;
		}

		DrawFont(clippedOut, characterPosition, *currentFont.sprite, color, frame, outlined);
		characterPosition.x += width + spacing;
	}

	if (HasAnyOf(flags, UiFlags::PentaCursor)) {
		const ClxSprite sprite = (*pSPentSpn2Cels)[PentSpn2Spin()];
		MaybeWrap(characterPosition, sprite.width(), rightMargin, initialX, lineHeight);
		ClxDraw(clippedOut, characterPosition + Displacement { 0, lineHeight - BaseLineOffset[size] }, sprite);
	} else if (HasAnyOf(flags, UiFlags::TextCursor) && GetAnimationFrame(2, 500) != 0) {
		MaybeWrap(characterPosition, 2, rightMargin, initialX, lineHeight);
		OptionalClxSpriteList baseFont = LoadFont(size, color, 0);
		if (baseFont)
			DrawFont(clippedOut, characterPosition, *baseFont, color, '|', outlined);
	}
}

uint8_t PentSpn2Spin()
{
	return (SDL_GetTicks() / 50) % 8;
}

} // namespace devilution
