#pragma once
/**
 * @brief std::unique_ptr specializations for SDL types.
 */

#include <memory>
#include <type_traits>

#include <SDL.h>

namespace devilution {

/**
 * @brief Deletes the object using `SDL_free*`.
 */
template <typename T>
struct SDLFreeDeleter {
	void operator()(T *obj) const
	{
		if constexpr (std::is_same<T, SDL_Surface>::value)
			SDL_FreeSurface(obj);
		else if constexpr (std::is_same<T, SDL_Palette>::value)
			SDL_FreePalette(obj);
		else
			SDL_free(obj);
	}
};

/**
 * @brief A unique pointer to T that is deleted with SDL_free.
 */
template <typename T>
using SDLUniquePtr = std::unique_ptr<T, SDLFreeDeleter<T>>;

} // namespace devilution
