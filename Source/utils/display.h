#pragma once

#include <cstdint>
#include <type_traits>

#include <SDL.h>

#include "utils/sdl_ptrs.h"
#include "utils/ui_fwd.h"

namespace devilution {

extern int refreshDelay; // Screen refresh rate in nanoseconds
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDLTextureUniquePtr texture;

extern SDLPaletteUniquePtr Palette;
extern SDL_Surface *PalSurface;
extern unsigned int pal_surface_palette_version;

bool IsFullScreen();

// Returns:
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
	if (!renderer)
		return;
	float scaleX;
	SDL_RenderGetScale(renderer, &scaleX, NULL);
	*x = static_cast<T>(*x / scaleX);
	*y = static_cast<T>(*y / scaleX);

	SDL_Rect view;
	SDL_RenderGetViewport(renderer, &view);
	*x -= view.x;
	*y -= view.y;
}

template <
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
void LogicalToOutput(T *x, T *y)
{
	if (!renderer)
		return;
	SDL_Rect view;
	SDL_RenderGetViewport(renderer, &view);
	*x += view.x;
	*y += view.y;

	float scaleX;
	SDL_RenderGetScale(renderer, &scaleX, NULL);
	*x = static_cast<T>(*x * scaleX);
	*y = static_cast<T>(*y * scaleX);
}

} // namespace devilution
