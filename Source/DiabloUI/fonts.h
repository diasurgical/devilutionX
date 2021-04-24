#pragma once

#include <SDL_ttf.h>
#include <stdint.h>
#include <string_view>

#include "DiabloUI/art.h"

namespace devilution {

enum _artFontTables : uint8_t {
	AFT_SMALL,
	AFT_MED,
	AFT_BIG,
	AFT_HUGE,
};

[[maybe_unused]] constexpr std::string_view toString(_artFontTables value)
{
	switch(value) {
	case AFT_SMALL:
		return "Small";
	case AFT_MED:
		return "Med";
	case AFT_BIG:
		return "Big";
	case AFT_HUGE:
		return "Huge";
	}
}

enum _artFontColors : uint8_t {
	AFC_SILVER,
	AFC_GOLD,
};

[[maybe_unused]] constexpr std::string_view toString(_artFontColors value)
{
	switch(value) {
	case AFC_SILVER:
		return "Silver";
	case AFC_GOLD:
		return "Gold";
	}
}

extern TTF_Font *font;
extern BYTE *FontTables[4];
extern Art ArtFonts[4][2];

void LoadArtFonts();
void UnloadArtFonts();

void LoadTtfFont();
void UnloadTtfFont();
void FontsCleanup();

} // namespace devilution
