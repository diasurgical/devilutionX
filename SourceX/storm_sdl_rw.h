#pragma once

#include <SDL.h>

#include "../SourceS/miniwin/misc.h"

namespace dvl {

/**
 * @brief Creates a read-only SDL_RWops from a Storm file handle.
 *
 * Does not close the handle when it gets closed.
 */
SDL_RWops *SFileRw_FromStormHandle(HANDLE handle);

} // namespace dvl
