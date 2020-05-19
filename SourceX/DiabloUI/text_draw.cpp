#include "DiabloUI/text_draw.h"

#include "DiabloUI/art_draw.h"
#include "DiabloUI/fonts.h"
#include "DiabloUI/text.h"
#include "DiabloUI/ui_item.h"
#include "DiabloUI/ttf_render_wrapped.h"
#include "display.h"

namespace dvl {

extern SDL_Surface *pal_surface;

namespace {

TextAlignment XAlignmentFromFlags(int flags)
{
	if (flags & UIS_CENTER)
		return TextAlignment_CENTER;
	if (flags & UIS_RIGHT)
		return TextAlignment_END;
	return TextAlignment_BEGIN;
}

int AlignXOffset(int flags, const SDL_Rect &dest, int w)
{
	if (flags & UIS_CENTER)
		return (dest.w - w) / 2;
	if (flags & UIS_RIGHT)
		return dest.w - w;
	return 0;
}

} // namespace

void DrawTTF(const char *text, const SDL_Rect &rectIn, int flags,
    const SDL_Color &text_color, const SDL_Color &shadow_color,
    TtfSurfaceCache **render_cache)
{
	SDL_Rect rect(rectIn);
	if (font == NULL || text == NULL || *text == '\0')
		return;
	if (*render_cache == NULL) {
		*render_cache = new TtfSurfaceCache();
		const TextAlignment x_align = XAlignmentFromFlags(flags);
		(*render_cache)->text = RenderUTF8_Solid_Wrapped(font, text, text_color, rect.w, x_align);
		ScaleSurfaceToOutput(&(*render_cache)->text);
		(*render_cache)->shadow = RenderUTF8_Solid_Wrapped(font, text, shadow_color, rect.w, x_align);
		ScaleSurfaceToOutput(&(*render_cache)->shadow);
	}
	SDL_Surface *text_surface = (*render_cache)->text;
	SDL_Surface *shadow_surface = (*render_cache)->shadow;
	if (text_surface == NULL)
		return;

	SDL_Rect dest_rect = rect;
	ScaleOutputRect(&dest_rect);
	dest_rect.x += AlignXOffset(flags, dest_rect, text_surface->w);
	dest_rect.y += (flags & UIS_VCENTER) ? (dest_rect.h - text_surface->h) / 2 : 0;

	SDL_Rect shadow_rect = dest_rect;
	++shadow_rect.x;
	++shadow_rect.y;
	if (SDL_BlitSurface(shadow_surface, NULL, GetOutputSurface(), &shadow_rect) < 0)
		ErrSdl();
	if (SDL_BlitSurface(text_surface, NULL, GetOutputSurface(), &dest_rect) < 0)
		ErrSdl();
}

void DrawArtStr(const char *text, const SDL_Rect &rect, int flags, bool drawTextCursor)
{
	_artFontTables size = AFT_SMALL;
	_artFontColors color = flags & UIS_GOLD ? AFC_GOLD : AFC_SILVER;

	if (flags & UIS_MED)
		size = AFT_MED;
	else if (flags & UIS_BIG)
		size = AFT_BIG;
	else if (flags & UIS_HUGE)
		size = AFT_HUGE;

	const int x = rect.x + AlignXOffset(flags, rect, GetArtStrWidth(text, size));
	const int y = rect.y + ((flags & UIS_VCENTER) ? (rect.h - ArtFonts[size][color].h()) / 2 : 0);

	int sx = x, sy = y;
	for (size_t i = 0, n = strlen(text); i < n; i++) {
		if (text[i] == '\n') {
			sx = x;
			sy += ArtFonts[size][color].h();
			continue;
		}
		BYTE w = FontTables[size][*(BYTE *)&text[i] + 2] ? FontTables[size][*(BYTE *)&text[i] + 2] : FontTables[size][0];
		DrawArt(sx, sy, &ArtFonts[size][color], *(BYTE *)&text[i], w);
		sx += w;
	}
	if (drawTextCursor && GetAnimationFrame(2, 500)) {
		DrawArt(sx, sy, &ArtFonts[size][color], '|');
	}
}

} // namespace dvl
