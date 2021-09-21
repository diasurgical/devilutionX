#pragma once

#include <cstddef>

#include <SDL.h>

namespace devilution {

// Re-maps a keyboard key as per the REMAP_KEYBOARD_KEYS define.
inline void remap_keyboard_key(SDL_Keycode *sym)
{
#ifdef REMAP_KEYBOARD_KEYS

	struct Mapping {
		SDL_Keycode from;
		SDL_Keycode to;
	};
	constexpr Mapping kMappings[] = { REMAP_KEYBOARD_KEYS };
	for (std::size_t i = 0; i < sizeof(kMappings) / sizeof(kMappings[0]); ++i) {
		if (*sym == kMappings[i].from) {
			*sym = kMappings[i].to;
			return;
		}
	}
#endif
}

} // namespace devilution
