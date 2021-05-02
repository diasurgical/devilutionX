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

} // namespace devilution
