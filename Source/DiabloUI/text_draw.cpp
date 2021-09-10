#include "DiabloUI/text_draw.h"

#include "DiabloUI/art_draw.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/fonts.h"
#include "DiabloUI/ttf_render_wrapped.h"
#include "DiabloUI/ui_item.h"
#include "utils/display.h"

namespace devilution {

namespace {

TextAlignment XAlignmentFromFlags(UiFlags flags)
{
	if (HasAnyOf(flags, UiFlags::AlignCenter))
		return TextAlignment_CENTER;
	if (HasAnyOf(flags, UiFlags::AlignRight))
		return TextAlignment_END;
	return TextAlignment_BEGIN;
}

int AlignXOffset(UiFlags flags, const SDL_Rect &dest, int w)
{
	if (HasAnyOf(flags, UiFlags::AlignCenter))
		return (dest.w - w) / 2;
	if (HasAnyOf(flags, UiFlags::AlignRight))
		return dest.w - w;
	return 0;
}

} // namespace

void DrawTTF(const char *text, const SDL_Rect &rectIn, UiFlags flags,
    const SDL_Color &textColor, const SDL_Color &shadowColor,
    TtfSurfaceCache &renderCache)
{
	SDL_Rect rect(rectIn);
	if (font == nullptr || text == nullptr || *text == '\0')
		return;

	const auto xAlign = XAlignmentFromFlags(flags);
	if (renderCache.text == nullptr)
		renderCache.text = ScaleSurfaceToOutput(RenderUTF8_Solid_Wrapped(font, text, textColor, rect.w, xAlign));
	if (renderCache.shadow == nullptr)
		renderCache.shadow = ScaleSurfaceToOutput(RenderUTF8_Solid_Wrapped(font, text, shadowColor, rect.w, xAlign));

	SDL_Surface *textSurface = renderCache.text.get();
	SDL_Surface *shadowSurface = renderCache.shadow.get();
	if (textSurface == nullptr)
		return;

	SDL_Rect destRect = rect;
	ScaleOutputRect(&destRect);
	destRect.x += AlignXOffset(flags, destRect, textSurface->w);
	destRect.y += HasAnyOf(flags, UiFlags::VerticalCenter) ? (destRect.h - textSurface->h) / 2 : 0;

	SDL_Rect shadowRect = destRect;
	++shadowRect.x;
	++shadowRect.y;
	if (SDL_BlitSurface(shadowSurface, nullptr, DiabloUiSurface(), &shadowRect) < 0)
		ErrSdl();
	if (SDL_BlitSurface(textSurface, nullptr, DiabloUiSurface(), &destRect) < 0)
		ErrSdl();
}

} // namespace devilution
