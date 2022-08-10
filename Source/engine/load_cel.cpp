#include "engine/load_cel.hpp"

#ifdef DEBUG_CEL_TO_CL2_SIZE
#include <iostream>
#endif

#include <SDL.h>

#include "appfat.h"
#include "engine/assets.hpp"
#include "utils/cel_to_clx.hpp"
#include "utils/str_cat.hpp"

namespace devilution {

OwnedClxSpriteListOrSheet LoadCelListOrSheet(const char *path, PointerOrValue<uint16_t> widthOrWidths)
{
	SDL_RWops *rwops = OpenAsset(path);
	if (rwops == nullptr)
		app_fatal(StrCat("Failed to open file:\n", path, "\n\n", SDL_GetError()));
#ifdef DEBUG_CEL_TO_CL2_SIZE
	std::cout << path;
#endif
	OwnedClxSpriteListOrSheet result = CelToClx(rwops, widthOrWidths);
	SDL_RWclose(rwops);
	return result;
}

} // namespace devilution
