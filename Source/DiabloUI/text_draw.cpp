#include "DiabloUI/text_draw.h"

#include "DiabloUI/art_draw.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/fonts.h"
#include "DiabloUI/text.h"
#include "DiabloUI/ttf_render_wrapped.h"
#include "DiabloUI/ui_item.h"
#include "utils/display.h"

namespace devilution {

namespace {

TextAlignment XAlignmentFromFlags(int flags)
{
	if ((flags & UIS_CENTER) != 0)
		return TextAlignment_CENTER;
	if ((flags & UIS_RIGHT) != 0)
		return TextAlignment_END;
	return TextAlignment_BEGIN;
}

int AlignXOffset(int flags, const SDL_Rect &dest, int w)
{
	if ((flags & UIS_CENTER) != 0)
		return (dest.w - w) / 2;
	if ((flags & UIS_RIGHT) != 0)
		return dest.w - w;
	return 0;
}

} // namespace

void DrawTTF(const char *text, const SDL_Rect &rectIn, int flags,
    const SDL_Color &textColor, const SDL_Color &shadowColor,
    TtfSurfaceCache &renderCache)
{
	SDL_Rect rect(rectIn);
	if (font == nullptr || text == nullptr || *text == '\0')
		return;

	const auto xAlign = XAlignmentFromFlags(flags);
	if (renderCache.text == nullptr)
		renderCache.text = ScaleSurfaceToOutput(SDLSurfaceUniquePtr { RenderUTF8_Solid_Wrapped(font, text, textColor, rect.w, xAlign) });
	if (renderCache.shadow == nullptr)
		renderCache.shadow = ScaleSurfaceToOutput(SDLSurfaceUniquePtr { RenderUTF8_Solid_Wrapped(font, text, shadowColor, rect.w, xAlign) });

	SDL_Surface *textSurface = renderCache.text.get();
	SDL_Surface *shadowSurface = renderCache.shadow.get();
	if (textSurface == nullptr)
		return;

	SDL_Rect destRect = rect;
	ScaleOutputRect(&destRect);
	destRect.x += AlignXOffset(flags, destRect, textSurface->w);
	destRect.y += (flags & UIS_VCENTER) != 0 ? (destRect.h - textSurface->h) / 2 : 0;

	SDL_Rect shadowRect = destRect;
	++shadowRect.x;
	++shadowRect.y;
	if (SDL_BlitSurface(shadowSurface, nullptr, DiabloUiSurface(), &shadowRect) < 0)
		ErrSdl();
	if (SDL_BlitSurface(textSurface, nullptr, DiabloUiSurface(), &destRect) < 0)
		ErrSdl();
}

void DrawArtStr(const char *text, const SDL_Rect &rect, int flags, bool drawTextCursor)
{
	_artFontTables size = AFT_SMALL;
	_artFontColors color = (flags & UIS_GOLD) != 0 ? AFC_GOLD : AFC_SILVER;

	if ((flags & UIS_MED) != 0)
		size = AFT_MED;
	else if ((flags & UIS_BIG) != 0)
		size = AFT_BIG;
	else if ((flags & UIS_HUGE) != 0)
		size = AFT_HUGE;

	const int x = rect.x + AlignXOffset(flags, rect, GetArtStrWidth(text, size));
	const int y = rect.y + ((flags & UIS_VCENTER) != 0 ? (rect.h - ArtFonts[size][color].h()) / 2 : 0);

	int sx = x;
	int sy = y;
	for (size_t i = 0, n = strlen(text); i < n; i++) {
		if (text[i] == '\n') {
			sx = x;
			sy += ArtFonts[size][color].h();
			continue;
		}
		uint8_t w = FontTables[size][static_cast<uint8_t>(text[i]) + 2];
		w = (w != 0) ? w : FontTables[size][0];
		DrawArt({ sx, sy }, &ArtFonts[size][color], static_cast<uint8_t>(text[i]), w);
		sx += w;
	}
	if (drawTextCursor && GetAnimationFrame(2, 500) != 0) {
		DrawArt({ sx, sy }, &ArtFonts[size][color], '|');
	}
}

} // namespace devilution
