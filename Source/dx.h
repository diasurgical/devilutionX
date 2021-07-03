/**
 * @file dx.h
 *
 * Interface of functions setting up the graphics pipeline.
 */
#pragma once

#include "engine.h"

namespace devilution {

/** Whether we render directly to the screen surface, i.e. `pal_surface == GetOutputSurface()` */
extern bool RenderDirectlyToOutputSurface;

Surface GlobalBackBuffer();

void dx_init();
void lock_buf(int idx);
void unlock_buf(int idx);
void dx_cleanup();
void dx_reinit();
void InitPalette();
void BltFast(SDL_Rect *src_rect, SDL_Rect *dst_rect);
void Blit(SDL_Surface *src, SDL_Rect *src_rect, SDL_Rect *dst_rect);
void RenderPresent();
void PaletteGetEntries(DWORD dwNumEntries, SDL_Color *lpEntries);

} // namespace devilution
