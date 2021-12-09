/**
 * @file dx.h
 *
 * Interface of functions setting up the graphics pipeline.
 */
#pragma once

#include "engine.h"

namespace devilution {

/** Whether we render directly to the screen surface, i.e. `PalSurface == GetOutputSurface()` */
extern bool RenderDirectlyToOutputSurface;

Surface GlobalBackBuffer();

void dx_init();
void lock_buf(int idx);
void unlock_buf(int idx);
void dx_cleanup();
void dx_reinit();
void dx_resize();
void InitPalette();
void BltFast(SDL_Rect *srcRect, SDL_Rect *dstRect);
void Blit(SDL_Surface *src, SDL_Rect *srcRect, SDL_Rect *dstRect);
void RenderPresent();
void PaletteGetEntries(int dwNumEntries, SDL_Color *lpEntries);

} // namespace devilution
