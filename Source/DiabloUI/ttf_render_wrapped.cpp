#include "DiabloUI/ttf_render_wrapped.h"

#include <cstddef>
#include <cstring>
#include <vector>
#include <algorithm>

#include <SDL.h>

#include "utils/log.hpp"
#include "utils/sdl_compat.h"
#include "utils/sdl_wrap.h"
#include "utils/ttf_wrap.h"

namespace devilution {

namespace {

bool CharacterIsDelimiter(char c)
{
	constexpr char Delimiters[] = { ' ', '\t', '\r', '\n' };

	return std::find(std::begin(Delimiters), std::end(Delimiters), c) != std::end(Delimiters);
}

} // namespace

// Based on SDL 2.0.12 TTF_RenderUTF8_Blended_Wrapped
SDLSurfaceUniquePtr RenderUTF8_Solid_Wrapped(TTF_Font *font, const char *text, SDL_Color fg, Uint32 wrapLength, const int xAlign)
{
	int width = 0;
	int height = 0;
	const int lineSpace = 2;

	/* Get the dimensions of the text surface */
	if (TTF_SizeUTF8(font, text, &width, &height) < 0 || width == 0) {
		TTF_SetError("Text has zero width");
		return {};
	}

	std::unique_ptr<char[]> str;
	std::vector<char *> strLines;
	if (wrapLength > 0 && *text != '\0') {
		const std::size_t strLen = std::strlen(text);

		str.reset(new char[strLen + 1]);

		std::memcpy(str.get(), text, strLen + 1);
		char *tok = str.get();
		char *end = str.get() + strLen;
		do {
			strLines.push_back(tok);

			/* Look for the end of the line */
			char *spot;
			if ((spot = SDL_strchr(tok, '\r')) != nullptr || (spot = SDL_strchr(tok, '\n')) != nullptr) {
				if (*spot == '\r') {
					++spot;
				}
				if (*spot == '\n') {
					++spot;
				}
			} else {
				spot = end;
			}
			char *nextTok = spot;

			/* Get the longest string that will fit in the desired space */
			for (;;) {
				/* Strip trailing whitespace */
				while (spot > tok && CharacterIsDelimiter(spot[-1])) {
					--spot;
				}
				if (spot == tok) {
					if (CharacterIsDelimiter(*spot)) {
						*spot = '\0';
					}
					break;
				}
				char delim = *spot;
				*spot = '\0';

				int w = 0;
				int h = 0;
				TTF_SizeUTF8(font, tok, &w, &h);
				if ((Uint32)w <= wrapLength) {
					break;
				}
				/* Back up and try again... */
				*spot = delim;

				while (spot > tok && !CharacterIsDelimiter(spot[-1])) {
					--spot;
				}
				if (spot > tok) {
					nextTok = spot;
				}
			}
			tok = nextTok;
		} while (tok < end);
	}

	if (strLines.empty())
		return TTFWrap::RenderText_Solid(font, text, fg);

	/* Create the target surface */
	auto textbuf = SDLWrap::CreateRGBSurface(SDL_SWSURFACE, (strLines.size() > 1) ? wrapLength : width, height * strLines.size() + (lineSpace * (strLines.size() - 1)), 8, 0, 0, 0, 0);

	/* Fill the palette with the foreground color */
	SDL_Palette *palette = textbuf->format->palette;
	palette->colors[0].r = 255 - fg.r;
	palette->colors[0].g = 255 - fg.g;
	palette->colors[0].b = 255 - fg.b;
	palette->colors[1].r = fg.r;
	palette->colors[1].g = fg.g;
	palette->colors[1].b = fg.b;
	SDLC_SetColorKey(textbuf.get(), 0);

	// Reduced space between lines to roughly match Diablo.
	const int lineskip = TTF_FontLineSkip(font) * 7 / 10; // avoids forced int > float > int conversion
	SDL_Rect dest = { 0, 0, 0, 0 };
	for (auto text : strLines) {
		if (*text == '\0') {
			dest.y += lineskip;
			continue;
		}
		SDLSurfaceUniquePtr tmp = TTFWrap::RenderText_Solid(font, text, fg);

		dest.w = static_cast<Uint16>(tmp->w);
		dest.h = static_cast<Uint16>(tmp->h);

		switch (xAlign) {
		case TextAlignment_END:
			dest.x = textbuf->w - tmp->w;
			break;
		case TextAlignment_CENTER:
			dest.x = (textbuf->w - tmp->w) / 2;
			break;
		case TextAlignment_BEGIN:
			dest.x = 0;
			break;
		}
		SDL_BlitSurface(tmp.get(), nullptr, textbuf.get(), &dest);
		dest.y += lineskip;
	}
	return textbuf;
}

} // namespace devilution
