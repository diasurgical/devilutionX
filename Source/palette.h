//HEADER_GOES_HERE
#ifndef __PALETTE_H__
#define __PALETTE_H__

extern SDL_Color logical_palette[256];
extern SDL_Color system_palette[256];
extern SDL_Color orig_palette[256];
extern int gdwPalEntries;

void SaveGamma();
void palette_init();
void LoadPalette(char *pszFileName);
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
void palette_update_quest_palette(int n);
BOOL palette_get_color_cycling();
BOOL palette_set_color_cycling(BOOL enabled);

/* rdata */

/* data */

extern int gamma_correction;
extern BOOL color_cycling_enabled;

#endif /* __PALETTE_H__ */
