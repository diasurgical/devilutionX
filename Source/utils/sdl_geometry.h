/**
 * @file sdl_geometry.h
 * Helpers for SDL geometry types
 */
#pragma once

#include <SDL_version.h>

#if SDL_VERSION_ATLEAST(2, 0, 0)
#include <SDL_rect.h>
#else
#include <SDL_video.h>
#endif

/**
 * @brief Same as constructing directly but avoids type conversion warnings.
 */
inline SDL_Rect MakeSdlRect(
    decltype(SDL_Rect {}.x) x, decltype(SDL_Rect {}.y) y,
    decltype(SDL_Rect {}.w) w, decltype(SDL_Rect {}.h) h)
{
	return SDL_Rect { x, y, w, h };
}
