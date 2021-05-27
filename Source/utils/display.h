#pragma once

#include <cstdint>
#include <type_traits>

#include <SDL.h>
#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#else
#include "utils/sdl2_backports.h"
#endif
#if !defined(NONET) && !defined(DISABLE_TCP)
#include <SDL_net.h>
#endif

#include "utils/sdl_ptrs.h"
#include "utils/ui_fwd.h"

namespace devilution {

extern int refreshDelay; // Screen refresh rate in nanoseconds
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;

extern SDL_Palette *palette;
extern SDL_Surface *pal_surface;
extern unsigned int pal_surface_palette_version;

#ifdef USE_SDL1
void SetVideoMode(int width, int height, int bpp, uint32_t flags);
void SetVideoModeToPrimary(bool fullscreen, int width, int height);
#endif

bool IsFullScreen();

// Returns:
// SDL1: Video surface.
// SDL2, no upscale: Window surface.
// SDL2, upscale: Renderer texture surface.
SDL_Surface *GetOutputSurface();

// Whether the output surface requires software scaling.
// Always returns false on SDL2.
bool OutputRequiresScaling();

// Scales rect if necessary.
void ScaleOutputRect(SDL_Rect *rect);

// If the output requires software scaling, replaces the given surface with a scaled one.
SDLSurfaceUniquePtr ScaleSurfaceToOutput(SDLSurfaceUniquePtr surface);

// Convert from output coordinates to logical (resolution-independent) coordinates.
template <
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
void OutputToLogical(T *x, T *y)
{
#ifndef USE_SDL1
	if (!renderer)
		return;
	float scaleX;
	SDL_RenderGetScale(renderer, &scaleX, NULL);
	*x /= scaleX;
	*y /= scaleX;

	SDL_Rect view;
	SDL_RenderGetViewport(renderer, &view);
	*x -= view.x;
	*y -= view.y;
#else
	if (!OutputRequiresScaling())
		return;
	const SDL_Surface *surface = GetOutputSurface();
	*x = *x * gnScreenWidth / surface->w;
	*y = *y * gnScreenHeight / surface->h;
#endif
}

template <
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
void LogicalToOutput(T *x, T *y)
{
#ifndef USE_SDL1
	if (!renderer)
		return;
	SDL_Rect view;
	SDL_RenderGetViewport(renderer, &view);
	*x += view.x;
	*y += view.y;

	float scaleX;
	SDL_RenderGetScale(renderer, &scaleX, NULL);
	*x *= scaleX;
	*y *= scaleX;
#else
	if (!OutputRequiresScaling())
		return;
	const SDL_Surface *surface = GetOutputSurface();
	*x = *x * surface->w / gnScreenWidth;
	*y = *y * surface->h / gnScreenHeight;
#endif
}

} // namespace devilution
