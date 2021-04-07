/**
 * @file dx.h
 *
 * Interface of functions setting up the graphics pipeline.
 */
#pragma once

namespace devilution {

CelOutputBuffer GlobalBackBuffer();

void dx_init();
void lock_buf(BYTE idx);
void unlock_buf(BYTE idx);
void dx_cleanup();
void dx_reinit();
void InitPalette();
void BltFast(SDL_Rect *src_rect, SDL_Rect *dst_rect);
void Blit(SDL_Surface *src, SDL_Rect *src_rect, SDL_Rect *dst_rect);
void RenderPresent();
void PaletteGetEntries(DWORD dwNumEntries, SDL_Color *lpEntries);

}
