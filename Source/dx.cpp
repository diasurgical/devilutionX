/**
 * @file dx.cpp
 *
 * Implementation of functions setting up the graphics pipeline.
 */
#include "dx.h"

#include <SDL.h>

#include "engine.h"
#include "options.h"
#include "storm/storm.h"
#include "utils/display.h"
#include "utils/log.hpp"
#include "utils/sdl_mutex.h"
#include "utils/sdl_wrap.h"

namespace devilution {

int refreshDelay;
SDL_Renderer *renderer;
SDLTextureUniquePtr texture;

/** Currently active palette */
SDLPaletteUniquePtr Palette;
unsigned int pal_surface_palette_version = 0;

/** 24-bit renderer texture surface */
SDLSurfaceUniquePtr RendererTextureSurface;

/** 8-bit surface that we render to */
SDL_Surface *PalSurface;
namespace {
SDLSurfaceUniquePtr PinnedPalSurface;
} // namespace

/** Whether we render directly to the screen surface, i.e. `PalSurface == GetOutputSurface()` */
bool RenderDirectlyToOutputSurface;

namespace {

int sgdwLockCount;
#ifdef _DEBUG
int locktbl[256];
#endif
SdlMutex MemCrit;

bool CanRenderDirectlyToOutputSurface()
{
	return false;
}

void CreateBackBuffer()
{
	if (CanRenderDirectlyToOutputSurface()) {
		Log("{}", "Will render directly to the SDL output surface");
		PalSurface = GetOutputSurface();
		RenderDirectlyToOutputSurface = true;
	} else {
		PinnedPalSurface = SDLWrap::CreateRGBSurfaceWithFormat(
		    /*flags=*/0,
		    /*width=*/gnScreenWidth,
		    /*height=*/gnScreenHeight,
		    /*depth=*/8,
		    SDL_PIXELFORMAT_INDEX8);
		PalSurface = PinnedPalSurface.get();
	}

	if (SDL_SetSurfacePalette(PalSurface, Palette.get()) < 0)
		ErrSdl();

	pal_surface_palette_version = 1;
}

void CreatePrimarySurface()
{
	if (renderer != nullptr) {
		int width = 0;
		int height = 0;
		SDL_RenderGetLogicalSize(renderer, &width, &height);
		Uint32 format;
		if (SDL_QueryTexture(texture.get(), &format, nullptr, nullptr, nullptr) < 0)
			ErrSdl();
		RendererTextureSurface = SDLWrap::CreateRGBSurfaceWithFormat(0, width, height, SDL_BITSPERPIXEL(format), format);
	}
}

void LockBufPriv()
{
	MemCrit.lock();
	if (sgdwLockCount != 0) {
		sgdwLockCount++;
		return;
	}

	sgdwLockCount++;
}

void UnlockBufPriv()
{
	if (sgdwLockCount == 0)
		app_fatal("draw main unlock error");

	sgdwLockCount--;
	MemCrit.unlock();
}

/**
 * @brief Limit FPS to avoid high CPU load, use when v-sync isn't available
 */
void LimitFrameRate()
{
	if (!sgOptions.Graphics.bFPSLimit)
		return;
	static uint32_t frameDeadline;
	uint32_t tc = SDL_GetTicks() * 1000;
	uint32_t v = 0;
	if (frameDeadline > tc) {
		v = tc % refreshDelay;
		SDL_Delay(v / 1000 + 1); // ceil
	}
	frameDeadline = tc + v + refreshDelay;
}

} // namespace

void dx_init()
{
	SDL_RaiseWindow(ghMainWnd);
	SDL_ShowWindow(ghMainWnd);

	CreatePrimarySurface();
	palette_init();
	CreateBackBuffer();
}

void lock_buf(int idx) // NOLINT(misc-unused-parameters)
{
#ifdef _DEBUG
	++locktbl[idx];
#endif
	LockBufPriv();
}

void unlock_buf(int idx) // NOLINT(misc-unused-parameters)
{
#ifdef _DEBUG
	if (locktbl[idx] == 0)
		app_fatal("Draw lock underflow: 0x%x", idx);
	--locktbl[idx];
#endif
	UnlockBufPriv();
}

Surface GlobalBackBuffer()
{
	if (sgdwLockCount == 0) {
		Log("WARNING: Trying to obtain GlobalBackBuffer() without holding a lock");
		return Surface();
	}

	return Surface(PalSurface, SDL_Rect { 0, 0, gnScreenWidth, gnScreenHeight });
}

void dx_cleanup()
{
	if (ghMainWnd != nullptr)
		SDL_HideWindow(ghMainWnd);
	MemCrit.lock();
	sgdwLockCount = 0;
	MemCrit.unlock();

	PalSurface = nullptr;
	PinnedPalSurface = nullptr;
	Palette = nullptr;
	RendererTextureSurface = nullptr;
	texture = nullptr;
	if (sgOptions.Graphics.bUpscale)
		SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(ghMainWnd);
}

void dx_reinit()
{
	Uint32 flags = 0;
	if (!IsFullScreen()) {
		flags = renderer != nullptr ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN;
	}
	if (SDL_SetWindowFullscreen(ghMainWnd, flags) != 0) {
		ErrSdl();
	}
	force_redraw = 255;
}

void InitPalette()
{
	Palette = SDLWrap::AllocPalette();
}

void BltFast(SDL_Rect *srcRect, SDL_Rect *dstRect)
{
	if (RenderDirectlyToOutputSurface)
		return;
	Blit(PalSurface, srcRect, dstRect);
}

void Blit(SDL_Surface *src, SDL_Rect *srcRect, SDL_Rect *dstRect)
{
	SDL_Surface *dst = GetOutputSurface();
	if (SDL_BlitSurface(src, srcRect, dst, dstRect) < 0)
		ErrSdl();
}

void RenderPresent()
{
	SDL_Surface *surface = GetOutputSurface();

	if (!gbActive) {
		LimitFrameRate();
		return;
	}

	if (renderer != nullptr) {
		if (SDL_UpdateTexture(texture.get(), nullptr, surface->pixels, surface->pitch) <= -1) { //pitch is 2560
			ErrSdl();
		}

		// Clear buffer to avoid artifacts in case the window was resized
		if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) <= -1) { // TODO only do this if window was resized
			ErrSdl();
		}

		if (SDL_RenderClear(renderer) <= -1) {
			ErrSdl();
		}
		if (SDL_RenderCopy(renderer, texture.get(), nullptr, nullptr) <= -1) {
			ErrSdl();
		}
		SDL_RenderPresent(renderer);

		if (!sgOptions.Graphics.bVSync) {
			LimitFrameRate();
		}
	} else {
		if (SDL_UpdateWindowSurface(ghMainWnd) <= -1) {
			ErrSdl();
		}
		LimitFrameRate();
	}
}

void PaletteGetEntries(int dwNumEntries, SDL_Color *lpEntries)
{
	for (int i = 0; i < dwNumEntries; i++) {
		lpEntries[i] = system_palette[i];
	}
}
} // namespace devilution
