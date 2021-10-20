#pragma once
/**
 * @brief std::unique_ptr specializations for SDL types.
 */

#include <memory>
#include <type_traits>

#include <SDL.h>
#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#else
#include "utils/sdl2_backports.h"
#endif

namespace devilution {

/**
 * @brief Deletes the SDL surface using `SDL_FreeSurface`.
 */
struct SDLSurfaceDeleter {
	void operator()(SDL_Surface *surface) const
	{
		SDL_FreeSurface(surface);
	}
};

using SDLSurfaceUniquePtr = std::unique_ptr<SDL_Surface, SDLSurfaceDeleter>;

#if SDL_VERSION_ATLEAST(2, 0, 0)
struct SDLCursorDeleter {
	void operator()(SDL_Cursor *cursor) const
	{
		SDL_FreeCursor(cursor);
	}
};

using SDLCursorUniquePtr = std::unique_ptr<SDL_Cursor, SDLCursorDeleter>;
#endif

#ifndef USE_SDL1
struct SDLTextureDeleter {
	void operator()(SDL_Texture *texture) const
	{
		SDL_DestroyTexture(texture);
	}
};

using SDLTextureUniquePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;
#endif

struct SDLPaletteDeleter {
	void operator()(SDL_Palette *palette) const
	{
		SDL_FreePalette(palette);
	}
};

using SDLPaletteUniquePtr = std::unique_ptr<SDL_Palette, SDLPaletteDeleter>;

/**
 * @brief Deletes the object using `SDL_free`.
 */
template <typename T>
struct SDLFreeDeleter {
	static_assert(!std::is_same<T, SDL_Surface>::value,
	    "SDL_Surface should use SDLSurfaceUniquePtr instead.");

	void operator()(T *obj) const
	{
		SDL_free(obj);
	}
};

/**
 * @brief A unique pointer to T that is deleted with SDL_free.
 */
template <typename T>
using SDLUniquePtr = std::unique_ptr<T, SDLFreeDeleter<T>>;

} // namespace devilution
