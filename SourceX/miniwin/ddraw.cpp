#include "miniwin/ddraw.h"

namespace dvl {

extern SDL_Surface *renderer_texture_surface; // defined in dx.cpp

SDL_Surface *GetOutputSurface()
{
#ifdef USE_SDL1
	return SDL_GetVideoSurface();
#else
	if (renderer)
		return renderer_texture_surface;
	return SDL_GetWindowSurface(window);
#endif
}

bool OutputRequiresScaling()
{
#ifdef USE_SDL1
	return SCREEN_WIDTH != GetOutputSurface()->w;
#else // SDL2, scaling handled by renderer.
	return false;
#endif
}

void ScaleOutputRect(SDL_Rect *rect)
{
	if (!OutputRequiresScaling())
		return;
	const auto *surface = GetOutputSurface();
	rect->x = rect->x * surface->w / SCREEN_WIDTH;
	rect->y = rect->y * surface->h / SCREEN_HEIGHT;
	rect->w = rect->w * surface->w / SCREEN_WIDTH;
	rect->h = rect->h * surface->h / SCREEN_HEIGHT;
}

} // namespace dvl
