#include "DiabloUI/fonts.h"
#include "file_util.h"

namespace dvl {

TTF_Font *font = NULL;
BYTE *FontTables[4];
Art ArtFonts[4][2];
/** This is so we know ttf has been init when we get to the diablo_deinit() function */
BOOL was_fonts_init = false;

namespace {

void LoadArtFont(const char *pszFile, int size, int color)
{
	LoadMaskedArt(pszFile, &ArtFonts[size][color], 256, 32);
}

} // namespace

void LoadArtFonts()
{
	FontTables[AFT_SMALL] = LoadFileInMem("ui_art\\font16.bin", 0);
	FontTables[AFT_MED] = LoadFileInMem("ui_art\\font24.bin", 0);
	FontTables[AFT_BIG] = LoadFileInMem("ui_art\\font30.bin", 0);
	FontTables[AFT_HUGE] = LoadFileInMem("ui_art\\font42.bin", 0);
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
	mem_free_dbg(FontTables[AFT_SMALL]);
	FontTables[AFT_SMALL] = NULL;
	mem_free_dbg(FontTables[AFT_MED]);
	FontTables[AFT_MED] = NULL;
	mem_free_dbg(FontTables[AFT_BIG]);
	FontTables[AFT_BIG] = NULL;
	mem_free_dbg(FontTables[AFT_HUGE]);
	FontTables[AFT_HUGE] = NULL;
}

void LoadTtfFont() {
	if (!TTF_WasInit()) {
		if (TTF_Init() == -1) {
			SDL_Log("TTF_Init: %s", TTF_GetError());
			diablo_quit(1);
		}
		was_fonts_init = true;
	}

	const char* ttf_font_path = TTF_FONT_NAME;
	if (!FileExists(ttf_font_path))
	{
		ttf_font_path = TTF_FONT_DIR TTF_FONT_NAME;
	}
#ifdef __linux__
	if (!FileExists(ttf_font_path))
	{
		ttf_font_path = "/usr/share/fonts/truetype/" TTF_FONT_NAME;
	}
#endif
	font = TTF_OpenFont(ttf_font_path, 17);
	if (font == NULL) {
		SDL_Log("TTF_OpenFont: %s", TTF_GetError());
		return;
	}

	TTF_SetFontKerning(font, false);
	TTF_SetFontHinting(font, TTF_HINTING_MONO);
}

void UnloadTtfFont() {
	if (font && TTF_WasInit())
		TTF_CloseFont(font);
	font = NULL;
}

void FontsCleanup() {
	TTF_Quit();	
}

} // namespace dvl
