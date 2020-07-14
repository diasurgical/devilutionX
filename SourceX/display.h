#pragma once

#include <cstdint>

#include "all.h"
#include <SDL.h>
#include <type_traits>

namespace dvl {

extern int refreshDelay; // Screen refresh rate in nanoseconds
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;

extern SDL_Palette *palette;
extern SDL_Surface *pal_surface;
extern unsigned int pal_surface_palette_version;

#ifdef USE_SDL1
void SetVideoMode(int width, int height, int bpp, uint32_t flags);
bool IsFullScreen();
void SetVideoModeToPrimary(bool fullscreen = IsFullScreen());
#endif

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
void ScaleSurfaceToOutput(SDL_Surface **surface);

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
	*x = *x * SCREEN_WIDTH / surface->w;
	*y = *y * SCREEN_HEIGHT / surface->h;
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
	*x = *x * surface->w / SCREEN_WIDTH;
	*y = *y * surface->h / SCREEN_HEIGHT;
#endif
}

} // namespace dvl
