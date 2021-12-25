/**
 * @file text_render.hpp
 *
 * Text rendering.
 */
#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <SDL.h>

#include "DiabloUI/ui_flags.hpp"
#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "engine/rectangle.hpp"
#include "utils/stdcompat/optional.hpp"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

enum GameFontTables : uint8_t {
	GameFont12,
	GameFont24,
	GameFont30,
	GameFont42,
	GameFont46,
	FontSizeDialog,
};

enum text_color : uint8_t {
	ColorUiGold,
	ColorUiSilver,
	ColorUiGoldDark,
	ColorUiSilverDark,

	ColorDialogWhite,
	ColorDialogYellow,

	ColorGold,
	ColorBlack,

	ColorWhite,
	ColorWhitegold,
	ColorRed,
	ColorBlue,

	ColorButtonface,
	ColorButtonpushed,
};

/**
 * @brief A format argument for `DrawStringWithColors`.
 */
class DrawStringFormatArg {
public:
	enum class Type {
		StringView,
		Int
	};

	DrawStringFormatArg(string_view value, UiFlags flags)
	    : type_(Type::StringView)
	    , string_view_value_(value)
	    , flags_(flags)
	{
	}

	DrawStringFormatArg(int value, UiFlags flags)
	    : type_(Type::Int)
	    , int_value_(value)
	    , flags_(flags)
	{
	}

	Type GetType() const
	{
		return type_;
	}

	string_view GetFormatted() const
	{
		if (type_ == Type::StringView)
			return string_view_value_;
		return formatted_;
	}

	void SetFormatted(std::string &&value)
	{
		formatted_ = std::move(value);
	}

	bool HasFormatted() const
	{
		return type_ == Type::StringView || !formatted_.empty();
	}

	int GetIntValue() const
	{
		return int_value_;
	}

	UiFlags GetFlags() const
	{
		return flags_;
	}

private:
	Type type_;
	union {
		string_view string_view_value_;
		int int_value_;
	};

	UiFlags flags_;
	std::string formatted_;
};

extern std::optional<CelSprite> pSPentSpn2Cels;

void UnloadFonts(GameFontTables size, text_color color);

/**
 * @brief Calculate pixel width of first line of text, respecting kerning
 * @param text Text to check, will read until first eol or terminator
 * @param size Font size to use
 * @param spacing Extra spacing to add per character
 * @param charactersInLine Receives characters read until newline or terminator
 * @return Line width in pixels
 */
int GetLineWidth(string_view text, GameFontTables size = GameFont12, int spacing = 1, int *charactersInLine = nullptr);

/**
 * @brief Calculate pixel width of first line of text, respecting kerning
 * @param fmt An fmt::format string.
 * @param args Format arguments.
 * @param size Font size to use
 * @param spacing Extra spacing to add per character
 * @param charactersInLine Receives characters read until newline or terminator
 * @return Line width in pixels
 */
int GetLineWidth(string_view fmt, DrawStringFormatArg *args, std::size_t argsLen, GameFontTables size, int spacing, int *charactersInLine = nullptr);

int GetLineHeight(string_view text, GameFontTables fontIndex);

[[nodiscard]] std::string WordWrapString(string_view text, size_t width, GameFontTables size = GameFont12, int spacing = 1);

/**
 * @brief Draws a line of text within a clipping rectangle (positioned relative to the origin of the output buffer).
 *
 * Specifying a small width (0 to less than two characters wide) should be avoided as this causes issues when laying
 * out the text. To wrap based on available space use the overload taking a Point. If the rect passed through has 0
 * height then the clipping area is extended to the bottom edge of the output buffer. If the clipping rectangle
 * dimensions extend past the edge of the output buffer text wrapping will be calculated using those dimensions (as if
 * the text was being rendered off screen). The text will not actually be drawn beyond the bounds of the output
 * buffer, this is purely to allow for clipping without wrapping.
 *
 * @param out The screen buffer to draw on.
 * @param text String to be drawn.
 * @param rect Clipping region relative to the output buffer describing where to draw the text and when to wrap long lines.
 * @param flags A combination of UiFlags to describe font size, color, alignment, etc. See ui_items.h for available options
 * @param spacing Additional space to add between characters.
 *                This value may be adjusted if the flag UIS_FIT_SPACING is passed in the flags parameter.
 * @param lineHeight Allows overriding the default line height, useful for multi-line strings.
 * @return The number of bytes rendered, including characters "drawn" outside the buffer.
 */
uint32_t DrawString(const Surface &out, string_view text, const Rectangle &rect, UiFlags flags = UiFlags::None, int spacing = 1, int lineHeight = -1);

/**
 * @brief Draws a line of text at the given position relative to the origin of the output buffer.
 *
 * This method is provided as a convenience to pass through to DrawString(..., Rectangle, ...) when no explicit
 * clipping/wrapping is requested. Note that this will still wrap the rendered string if it would end up being drawn
 * beyond the right edge of the output buffer and clip it if it would extend beyond the bottom edge of the buffer.
 *
 * @param out The screen buffer to draw on.
 * @param text String to be drawn.
 * @param position Location of the top left corner of the string relative to the top left corner of the output buffer.
 * @param flags A combination of UiFlags to describe font size, color, alignment, etc. See ui_items.h for available options
 * @param spacing Additional space to add between characters.
 *                This value may be adjusted if the flag UIS_FIT_SPACING is passed in the flags parameter.
 * @param lineHeight Allows overriding the default line height, useful for multi-line strings.
 */
inline void DrawString(const Surface &out, string_view text, const Point &position, UiFlags flags = UiFlags::None, int spacing = 1, int lineHeight = -1)
{
	DrawString(out, text, { position, { out.w() - position.x, 0 } }, flags, spacing, lineHeight);
}

/**
 * @brief Draws a line of text with different colors for certain parts of the text.
 *
 * @example DrawStringWithColors(out, "Press {} to start", {{"Ⓧ", UiFlags::ColorBlue}}, UiFlags::ColorWhite)
 *
 * @param out Output buffer to draw the text on.
 * @param fmt An fmt::format string.
 * @param args Format arguments.
 * @param position Location of the top left corner of the string relative to the top left corner of the output buffer.
 * @param flags A combination of UiFlags to describe font size, color, alignment, etc. See ui_items.h for available options
 * @param spacing Additional space to add between characters.
 *                This value may be adjusted if the flag UIS_FIT_SPACING is passed in the flags parameter.
 * @param lineHeight Allows overriding the default line height, useful for multi-line strings.
 */
void DrawStringWithColors(const Surface &out, string_view fmt, DrawStringFormatArg *args, std::size_t argsLen, const Rectangle &rect, UiFlags flags = UiFlags::None, int spacing = 1, int lineHeight = -1);

inline void DrawStringWithColors(const Surface &out, string_view fmt, std::vector<DrawStringFormatArg> args, const Rectangle &rect, UiFlags flags = UiFlags::None, int spacing = 1, int lineHeight = -1)
{
	return DrawStringWithColors(out, fmt, args.data(), args.size(), rect, flags, spacing, lineHeight);
}

uint8_t PentSpn2Spin();
void UnloadFonts();

} // namespace devilution
