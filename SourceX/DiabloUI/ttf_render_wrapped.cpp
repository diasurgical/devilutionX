#include "DiabloUI/ttf_render_wrapped.h"

#include <cstddef>

namespace dvl {

namespace {

SDL_bool CharacterIsDelimiter(char c, const char *delimiters)
{
	while (*delimiters) {
		if (c == *delimiters)
			return SDL_TRUE;
		++delimiters;
	}
	return SDL_FALSE;
}

} // namespace

// Based on SDL 2.0.12 TTF_RenderUTF8_Blended_Wrapped
SDL_Surface *RenderUTF8_Solid_Wrapped(TTF_Font *font, const char *text, SDL_Color fg, Uint32 wrapLength)
{
	int width, height;
	SDL_Surface *textbuf;
	const int lineSpace = 2;
	char *str, **strLines;

	/* Get the dimensions of the text surface */
	if ((TTF_SizeUTF8(font, text, &width, &height) < 0) || !width) {
		TTF_SetError("Text has zero width");
		return nullptr;
	}

	std::size_t numLines = 1;
	str = nullptr;
	strLines = nullptr;
	if (wrapLength > 0 && *text) {
		const char *wrapDelims = " \t\r\n";
		int w, h;
		char *spot, *tok, *next_tok, *end;
		char delim;
		size_t str_len = SDL_strlen(text);

		numLines = 0;

		str = SDL_stack_alloc(char, str_len + 1);
		if (str == nullptr) {
			TTF_SetError("Out of memory");
			return nullptr;
		}

		SDL_strlcpy(str, text, str_len + 1);
		tok = str;
		end = str + str_len;
		do {
			strLines = (char **)SDL_realloc(strLines, (numLines + 1) * sizeof(*strLines));
			if (!strLines) {
				TTF_SetError("Out of memory");
				return nullptr;
			}
			strLines[numLines++] = tok;

			/* Look for the end of the line */
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
			next_tok = spot;

			/* Get the longest string that will fit in the desired space */
			for (;;) {
				/* Strip trailing whitespace */
				while (spot > tok && CharacterIsDelimiter(spot[-1], wrapDelims)) {
					--spot;
				}
				if (spot == tok) {
					if (CharacterIsDelimiter(*spot, wrapDelims)) {
						*spot = '\0';
					}
					break;
				}
				delim = *spot;
				*spot = '\0';

				TTF_SizeUTF8(font, tok, &w, &h);
				if ((Uint32)w <= wrapLength) {
					break;
				} else {
					/* Back up and try again... */
					*spot = delim;
				}

				while (spot > tok && !CharacterIsDelimiter(spot[-1], wrapDelims)) {
					--spot;
				}
				if (spot > tok) {
					next_tok = spot;
				}
			}
			tok = next_tok;
		} while (tok < end);
	}

	if (!strLines) {
		return TTF_RenderUTF8_Solid(font, text, fg);
	}

	/* Create the target surface */
	textbuf = SDL_CreateRGBSurface(SDL_SWSURFACE, (numLines > 1) ? wrapLength : width, height * numLines + (lineSpace * (numLines - 1)), 8, 0, 0, 0, 0);
	if (textbuf == nullptr) {
		if (strLines) {
			SDL_free(strLines);
			SDL_stack_free(str);
		}
		return nullptr;
	}

	/* Fill the palette with the foreground color */
	SDL_Palette *palette = textbuf->format->palette;
	palette->colors[0].r = 255 - fg.r;
	palette->colors[0].g = 255 - fg.g;
	palette->colors[0].b = 255 - fg.b;
	palette->colors[1].r = fg.r;
	palette->colors[1].g = fg.g;
	palette->colors[1].b = fg.b;
#ifdef USE_SDL1
	SDL_SetColorKey(textbuf, SDL_SRCCOLORKEY, 0);
#else
	SDL_SetColorKey(textbuf, SDL_TRUE, 0);
#endif

	SDL_Rect dest = { 0, 0, 0, 0 };
	for (std::size_t line = 0; line < numLines; line++) {
		text = strLines[line];
		SDL_Surface *tmp = TTF_RenderUTF8_Solid(font, text, fg);
		dest.w = static_cast<decltype(SDL_Rect().w)>(tmp->w);
		dest.h = static_cast<decltype(SDL_Rect().h)>(tmp->h);
		SDL_BlitSurface(tmp, nullptr, textbuf, &dest);
		dest.y += tmp->h;
		SDL_FreeSurface(tmp);
	}
	SDL_free(strLines);
	SDL_stack_free(str);
	return textbuf;
}

} // namespace dvl
