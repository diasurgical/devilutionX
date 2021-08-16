#pragma once

#include <SDL.h>

#include "miniwin/miniwin.h"

namespace devilution {

/**
 * @brief Creates a read-only SDL_RWops from a Storm file handle.
 *
 * Closes the handle when it gets closed.
 */
SDL_RWops *SFileRw_FromStormHandle(HANDLE handle);

/**
 * @brief Opens a Storm file and creates a read-only SDL_RWops from its handle.
 *
 * Closes the handle when it gets closed.
 */
SDL_RWops *SFileOpenRw(const char *filename);

} // namespace devilution
