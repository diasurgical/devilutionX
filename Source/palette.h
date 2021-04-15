/**
 * @file palette.h
 *
 * Interface of functions for handling the engines color palette.
 */
#pragma once

namespace devilution {

extern SDL_Color logical_palette[256];
extern SDL_Color system_palette[256];
extern SDL_Color orig_palette[256];
extern Uint8 paletteTransparencyLookup[256][256];

void palette_update();
void palette_init();
void LoadPalette(const char *pszFileName);
void LoadRndLvlPal(int l);
void ResetPal();
void IncreaseGamma();
void ApplyGamma(SDL_Color *dst, const SDL_Color *src, int n);
void DecreaseGamma();
int UpdateGamma(int gamma);
void BlackPalette();
void SetFadeLevel(DWORD fadeval);
void PaletteFadeIn(int fr);
void PaletteFadeOut(int fr);
void palette_update_caves();
void palette_update_crypt();
void palette_update_hive();
void palette_update_quest_palette(int n);

}
