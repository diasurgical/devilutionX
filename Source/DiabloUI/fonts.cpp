#include "DiabloUI/fonts.h"

#include "diablo.h"
#include "engine/load_file.hpp"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"

namespace devilution {

TTF_Font *font = nullptr;
std::unique_ptr<uint8_t[]> FontTables[4];
Art ArtFonts[4][2];
/** This is so we know ttf has been init when we get to the diablo_deinit() function */
bool was_fonts_init = false;

namespace {

void LoadArtFont(const char *pszFile, int size, int color)
{
	LoadMaskedArt(pszFile, &ArtFonts[size][color], 256, 32);
}

} // namespace

void LoadArtFonts()
{
	FontTables[AFT_SMALL] = LoadFileInMem<uint8_t>("ui_art\\font16.bin");
	FontTables[AFT_MED] = LoadFileInMem<uint8_t>("ui_art\\font24.bin");
	FontTables[AFT_BIG] = LoadFileInMem<uint8_t>("ui_art\\font30.bin");
	FontTables[AFT_HUGE] = LoadFileInMem<uint8_t>("ui_art\\font42.bin");
	LoadArtFont("ui_art\\font16s.pcx", AFT_SMALL, AFC_SILVER);
	LoadArtFont("ui_art\\font16g.pcx", AFT_SMALL, AFC_GOLD);
	LoadArtFont("ui_art\\font24s.pcx", AFT_MED, AFC_SILVER);
	LoadArtFont("ui_art\\font24g.pcx", AFT_MED, AFC_GOLD);
	LoadArtFont("ui_art\\font30s.pcx", AFT_BIG, AFC_SILVER);
	LoadArtFont("ui_art\\font30g.pcx", AFT_BIG, AFC_GOLD);
	LoadArtFont("ui_art\\font42g.pcx", AFT_HUGE, AFC_GOLD);
}

void UnloadArtFonts()
{
	ArtFonts[AFT_SMALL][AFC_SILVER].Unload();
	ArtFonts[AFT_SMALL][AFC_GOLD].Unload();
	ArtFonts[AFT_MED][AFC_SILVER].Unload();
	ArtFonts[AFT_MED][AFC_GOLD].Unload();
	ArtFonts[AFT_BIG][AFC_SILVER].Unload();
	ArtFonts[AFT_BIG][AFC_GOLD].Unload();
	ArtFonts[AFT_HUGE][AFC_GOLD].Unload();
	FontTables[AFT_SMALL] = nullptr;
	FontTables[AFT_MED] = nullptr;
	FontTables[AFT_BIG] = nullptr;
	FontTables[AFT_HUGE] = nullptr;
}

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
