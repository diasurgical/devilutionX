#pragma once

#include <SDL_ttf.h>
#include <stdint.h>
#include <string_view>
namespace devilution {

enum TextAlignment : uint8_t {
	TextAlignment_BEGIN,
	TextAlignment_CENTER,
	TextAlignment_END,
};

constexpr std::string_view toString(TextAlignment value)
{
	switch(value) {
	case TextAlignment_BEGIN:
		return "Begin";
	case TextAlignment_CENTER:
		return "Center";
	case TextAlignment_END:
		return "End";
	}
}

/**
 * Renders UTF-8, wrapping lines to avoid exceeding wrapLength, and aligning
 * according to the `x_align` argument.
 *
 * This method is slow. Caching the result is recommended.
 */
SDL_Surface *RenderUTF8_Solid_Wrapped(
    TTF_Font *font, const char *text, SDL_Color fg, Uint32 wrapLength, const int x_align = TextAlignment_BEGIN);

} // namespace devilution
