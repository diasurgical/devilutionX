/**
 * @file palette.h
 *
 * Interface of functions for handling the engines color palette.
 */
#pragma once

#include "gendung.h"

namespace devilution {

// Diablo uses a 256 color palette
// Entry 0-127 (0x00-0x7F) are level specific
// Entry 128-255 (0x80-0xFF) are global

// standard palette for all levels
// 8 or 16 shades per color
// example (dark blue): PAL16_BLUE+14, PAL8_BLUE+7
// example (light red): PAL16_RED+2, PAL8_RED
// example (orange): PAL16_ORANGE+8, PAL8_ORANGE+4
#define PAL8_BLUE 128
#define PAL8_RED 136
#define PAL8_YELLOW 144
#define PAL8_ORANGE 152
#define PAL16_BEIGE 160
#define PAL16_BLUE 176
#define PAL16_YELLOW 192
#define PAL16_ORANGE 208
#define PAL16_RED 224
#define PAL16_GRAY 240

extern SDL_Color logical_palette[256];
extern SDL_Color system_palette[256];
extern SDL_Color orig_palette[256];
extern Uint8 paletteTransparencyLookup[256][256];

void palette_update();
void palette_init();
void LoadPalette(const char *pszFileName);
void LoadRndLvlPal(dungeon_type l);
void ResetPal();
void IncreaseGamma();
void ApplyGamma(SDL_Color *dst, const SDL_Color *src, int n);
void DecreaseGamma();
int UpdateGamma(int gamma);
void BlackPalette();
void SetFadeLevel(int fadeval);
/**
 * @brief Fade screen from black
 * @param fr Steps per 50ms
 */
void PaletteFadeIn(int fr);
/**
 * @brief Fade screen to black
 * @param fr Steps per 50ms
 */
void PaletteFadeOut(int fr);
void palette_update_caves();
void palette_update_crypt();
void palette_update_hive();
void palette_update_quest_palette(int n);

} // namespace devilution
