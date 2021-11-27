#include "DiabloUI/art_draw.h"

#include "DiabloUI/diabloui.h"
#include "palette.h"
#include "utils/display.h"
#include "utils/sdl_compat.h"

namespace devilution {

void UpdatePalette(Art *art, const SDL_Surface *output)
{
	if (art->surface->format->BitsPerPixel != 8)
		return;

	if (art->palette_version == pal_surface_palette_version)
		return;

	if (output == nullptr || output->format->BitsPerPixel != 8)
		output = PalSurface;

	if (SDLC_SetSurfaceColors(art->surface.get(), output->format->palette) <= -1)
		ErrSdl();

	art->palette_version = pal_surface_palette_version;
}

void DrawArt(Point screenPosition, Art *art, int nFrame, Uint16 srcW, Uint16 srcH)
{
	if (art->surface == nullptr || screenPosition.y >= gnScreenHeight || screenPosition.x >= gnScreenWidth)
		return;

	SDL_Rect srcRect = MakeSdlRect(0, nFrame * art->h(), art->w(), art->h());

	ScaleOutputRect(&srcRect);

	if (srcW != 0 && srcW < srcRect.w)
		srcRect.w = srcW;
	if (srcH != 0 && srcH < srcRect.h)
		srcRect.h = srcH;

	if (screenPosition.x + srcRect.w <= 0 || screenPosition.y + srcRect.h <= 0)
		return;

	SDL_Rect dstRect = MakeSdlRect(screenPosition.x, screenPosition.y, srcRect.w, srcRect.h);
	ScaleOutputRect(&dstRect);

	UpdatePalette(art);

	if (SDL_BlitSurface(art->surface.get(), &srcRect, DiabloUiSurface(), &dstRect) < 0)
		ErrSdl();
}

void DrawArt(const Surface &out, Point position, Art *art, int nFrame, Uint16 srcW, Uint16 srcH)
{
	if (art->surface == nullptr || position.y >= out.h() || position.x >= out.w())
		return;

	SDL_Rect srcRect = MakeSdlRect(0, nFrame * art->h(), art->w(), art->h());
	if (srcW != 0 && srcW < srcRect.w)
		srcRect.w = srcW;
	if (srcH != 0 && srcH < srcRect.h)
		srcRect.h = srcH;

	if (position.x + srcRect.w <= 0 || position.y + srcRect.h <= 0)
		return;

	out.Clip(&srcRect, &position);
	SDL_Rect dstRect = MakeSdlRect(position.x + out.region.x, position.y + out.region.y, 0, 0);

	UpdatePalette(art, out.surface);

	if (SDL_BlitSurface(art->surface.get(), &srcRect, out.surface, &dstRect) < 0)
		ErrSdl();
}

void DrawAnimatedArt(Art *art, Point screenPosition)
{
	DrawArt(screenPosition, art, GetAnimationFrame(art->frames));
}

int GetAnimationFrame(int frames, int fps)
{
	int frame = (SDL_GetTicks() / fps) % frames;

	return frame > frames ? 0 : frame;
}

} // namespace devilution
