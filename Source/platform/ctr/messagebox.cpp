#include <3ds.h>
#include <SDL.h>

#include "utils/sdl2_to_1_2_backports.h"
#include "utils/str_cat.hpp"

int SDL_ShowSimpleMessageBox(Uint32 flags,
    const char *title,
    const char *message,
    SDL_Surface *window)
{
	if (SDL_ShowCursor(SDL_DISABLE) <= -1)
		SDL_Log("%s", SDL_GetError());

	bool init = !gspHasGpuRight();
	auto text = devilution::StrCat(title, "\n\n", message);

	if (init)
		gfxInitDefault();

	errorConf error;
	errorInit(&error, ERROR_TEXT, CFG_LANGUAGE_EN);
	errorText(&error, text.c_str());
	errorDisp(&error);

	if (init)
		gfxExit();

	return 0;
}
