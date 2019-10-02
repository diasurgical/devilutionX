#include "DiabloUI/art_draw.h"

namespace dvl {

extern SDL_Surface *pal_surface;
extern unsigned int pal_surface_palette_version;

void DrawArt(int screenX, int screenY, Art *art, int nFrame,
    decltype(SDL_Rect().w) srcW, decltype(SDL_Rect().h) srcH)
{
	if (screenY >= SCREEN_Y + SCREEN_HEIGHT || screenX >= SCREEN_X + SCREEN_WIDTH || art->surface == nullptr)
		return;

	SDL_Rect src_rect = {
		0,
		static_cast<decltype(SDL_Rect().y)>(nFrame * art->h()),
		static_cast<decltype(SDL_Rect().w)>(art->w()),
		static_cast<decltype(SDL_Rect().h)>(art->h())
	};
	if (srcW && srcW < src_rect.w)
		src_rect.w = srcW;
	if (srcH && srcH < src_rect.h)
		src_rect.h = srcH;
	SDL_Rect dst_rect = {
		static_cast<decltype(SDL_Rect().x)>(screenX + SCREEN_X),
		static_cast<decltype(SDL_Rect().y)>(screenY + SCREEN_Y),
		src_rect.w, src_rect.h
	};

	if (art->surface->format->BitsPerPixel == 8 && art->palette_version != pal_surface_palette_version) {
#ifdef USE_SDL1
		if (SDL_SetPalette(art->surface, SDL_LOGPAL, pal_surface->format->palette->colors, 0, 256) != 1)
			SDL_Log(SDL_GetError());
#else
		if (SDL_SetSurfacePalette(art->surface, pal_surface->format->palette) <= -1)
			SDL_Log(SDL_GetError());
#endif
		art->palette_version = pal_surface_palette_version;
	}

	if (SDL_BlitSurface(art->surface, &src_rect, pal_surface, &dst_rect) <= -1) {
		SDL_Log(SDL_GetError());
	}
}

void DrawAnimatedArt(Art *art, int screenX, int screenY) {
	DrawArt(screenX, screenY, art, GetAnimationFrame(art->frames));
}

int GetAnimationFrame(int frames, int fps)
{
	int frame = (SDL_GetTicks() / fps) % frames;

	return frame > frames ? 0 : frame;
}

} // namespace dvl
