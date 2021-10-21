#pragma once

#include <SDL.h>

namespace devilution {

/**
 * @brief Opens a Storm file and creates a read-only SDL_RWops from its handle.
 *
 * Closes the handle when it gets closed.
 */
SDL_RWops *SFileOpenRw(const char *filename);

} // namespace devilution
