/**
 * @file capture.cpp
 *
 * Implementation of the screenshot function.
 */
#include <cstdint>
#include <cstdio>
#include <ctime>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_messagebox.h>
#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "engine/backbuffer_state.hpp"
#include "engine/dx.h"
#include "engine/palette.h"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/str_cat.hpp"
#include "utils/ui_fwd.h"

namespace devilution {
namespace {

std::string GetScreenshotFileName()
{
	const std::time_t tt = std::time(nullptr);
	const std::tm *tm = std::localtime(&tt);
	const std::string filename = tm != nullptr ? fmt::format("Screenshot from {:04}-{:02}-{:02} {:02}-{:02}-{:02}", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec) : "Screenshot";
	std::string dstPath = StrCat(paths::PrefPath(), filename, ".png");
	int i = 0;
	while (FileExists(dstPath.c_str())) {
		i++;
		dstPath = StrCat(paths::PrefPath(), filename, "-", i, ".png");
	}
	return dstPath;
}

bool CaptureScreenToPNG(SDL_Color *palette)
{
	SDL_Surface *surface = SDL_GetWindowSurface(window);
	if (!surface) {
		Log("Failed to capture screen surface.");
		return false;
	}

	std::string fileName = GetScreenshotFileName();
	if (IMG_SavePNG(surface, fileName.c_str()) != 0) {
		Log("Failed to save screenshot to {}", fileName);
		SDL_FreeSurface(surface);
		return false;
	}

	Log("Screenshot saved to {}", fileName);
	SDL_FreeSurface(surface);
	return true;
}

/**
 * @brief Make a red version of the given palette and apply it to the screen.
 */
void RedPalette()
{
	for (int i = 0; i < 256; i++) {
		system_palette[i].g = 0;
		system_palette[i].b = 0;
	}
	palette_update();
	BltFast(nullptr, nullptr);
	RenderPresent();
}
} // namespace

void CaptureScreen()
{
	SDL_Color palette[256];
	DrawAndBlit();
	RedPalette();

	if (CaptureScreenToPNG(palette)) {
		SDL_Delay(300);
		for (int i = 0; i < 256; i++) {
			system_palette[i] = palette[i];
		}
		palette_update();
		RedrawEverything();
	}
}

} // namespace devilution
