#include <3ds.h>
#include <SDL.h>
#include <fmt/core.h>

#include "utils/sdl2_to_1_2_backports.h"

int SDL_ShowSimpleMessageBox(Uint32 flags,
    const char *title,
    const char *message,
    SDL_Surface *window)
{
	if (SDL_ShowCursor(SDL_DISABLE) <= -1)
		SDL_Log("%s", SDL_GetError());

	bool init = !gspHasGpuRight();
	auto text = fmt::format("{}\n\n{}", title, message);

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
