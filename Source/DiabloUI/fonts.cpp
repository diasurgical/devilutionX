#include "DiabloUI/fonts.h"

#include "diablo.h"
#include "engine/load_file.hpp"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"

namespace devilution {

TTF_Font *font = nullptr;
/** This is so we know ttf has been init when we get to the diablo_deinit() function */
bool was_fonts_init = false;

void LoadTtfFont()
{
	if (TTF_WasInit() == 0) {
		if (TTF_Init() == -1) {
			Log("TTF_Init: {}", TTF_GetError());
			diablo_quit(1);
		}
		was_fonts_init = true;
	}

	std::string ttfFontPath = paths::TtfPath() + paths::TtfName();
#if defined(__linux__) && !defined(__ANDROID__)
	if (!FileExists(ttfFontPath.c_str())) {
		ttfFontPath = "/usr/share/fonts/truetype/" + paths::TtfName();
	}
#endif
	font = TTF_OpenFont(ttfFontPath.c_str(), 17);
	if (font == nullptr) {
		Log("TTF_OpenFont: {}", TTF_GetError());
		return;
	}

	TTF_SetFontKerning(font, 0);
	TTF_SetFontHinting(font, TTF_HINTING_MONO);
}

void UnloadTtfFont()
{
	if (font != nullptr && TTF_WasInit() != 0)
		TTF_CloseFont(font);
	font = nullptr;
}

void FontsCleanup()
{
	TTF_Quit();
}

} // namespace devilution
