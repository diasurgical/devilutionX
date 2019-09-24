#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "miniwin/ddraw.h"
#include <SDL.h>

namespace dvl {

int sgdwLockCount;
BYTE *gpBuffer;
#ifdef _DEBUG
int locktbl[256];
#endif
static CCritSect sgMemCrit;
char gbEmulate;
HMODULE ghDiabMod;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

/** Currently active palette */
SDL_Palette *palette;
unsigned int pal_surface_palette_version = 0;

/** 32-bit in-memory backbuffer surface */
SDL_Surface *surface;

/** 8-bit surface wrapper around #gpBuffer */
SDL_Surface *pal_surface;

bool bufferUpdated = false;

void dx_init(HWND hWnd)
{
	SDL_RaiseWindow(window);
	MainWndProc(NULL, DVL_WM_ACTIVATEAPP, true, 0); // TODO trigger on SDL_WINDOWEVENT_FOCUS_GAINED

	SDL_ShowWindow(window);

	dx_create_primary_surface();
	palette_init();
	dx_create_back_buffer();
}

void dx_create_back_buffer()
{
	pal_surface = SDL_CreateRGBSurfaceWithFormat(0, BUFFER_WIDTH, BUFFER_HEIGHT, 8, SDL_PIXELFORMAT_INDEX8);
	if (pal_surface == NULL) {
		SDL_Log(SDL_GetError());
		ERR_DLG(IDD_DIALOG1, 0x80000002L); //DDERR_OUTOFMEMORY
		return;
	}

	gpBuffer = (BYTE *)pal_surface->pixels;

#ifdef USE_SDL1
	if (SDL_SetPalette(pal_surface, SDL_LOGPAL, palette->colors, 0, palette->ncolors) != 1) {
#else
	if (SDL_SetSurfacePalette(pal_surface, palette) <= -1) {
#endif
		SDL_Log(SDL_GetError());
		ERR_DLG(IDD_DIALOG1, 1); //MAKE_HRESULT(130);//DVL_MAKE_HRESULT(130);
	}

	pal_surface_palette_version = 1;
}

void dx_create_primary_surface()
{
#ifdef USE_SDL1
	surface = SDL_GetVideoSurface();
#else
	if (renderer) {
		int width, height;
		if (SDL_GetRendererOutputSize(renderer, &width, &height) <= -1) {
			SDL_Log(SDL_GetError());
		}
		// TODO Get format from render/window
		surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
	} else {
		surface = SDL_GetWindowSurface(window);
	}
#endif
	if (surface == NULL) {
		SDL_Log(SDL_GetError());
		ERR_DLG(IDD_DIALOG1, (HRESULT)0x80000002L); //DDERR_OUTOFMEMORY
		return;
	}
}

void lock_buf(BYTE idx)
{
#ifdef _DEBUG
	locktbl[idx]++;
#endif
	lock_buf_priv();
}

void lock_buf_priv()
{
	sgMemCrit.Enter();
	if (sgdwLockCount != 0) {
		sgdwLockCount++;
		return;
	}

	gpBufEnd += (uintptr_t)(BYTE *)pal_surface->pixels;
	gpBuffer = (BYTE *)pal_surface->pixels;
	sgdwLockCount++;
}

void unlock_buf(BYTE idx)
{
#ifdef _DEBUG
	if (!locktbl[idx])
		app_fatal("Draw lock underflow: 0x%x", idx);
	locktbl[idx]--;
#endif
	unlock_buf_priv();
}

void unlock_buf_priv()
{
	HRESULT error_code;

	if (sgdwLockCount == 0)
		app_fatal("draw main unlock error");
	if (!gpBuffer)
		app_fatal("draw consistency error");

	sgdwLockCount--;
	if (sgdwLockCount == 0) {
		gpBufEnd -= (uintptr_t)gpBuffer;
		//gpBuffer = NULL; unable to return to menu
		error_code = RenderPresent();
		if (error_code != DVL_S_OK)
			DD_ERR_MSG(error_code);
	}
	sgMemCrit.Leave();
}

void dx_cleanup()
{
	if (ghMainWnd)
		ShowWindow(ghMainWnd, 0);
	sgMemCrit.Enter();
	sgdwLockCount = 0;
	gpBuffer = NULL;
	sgMemCrit.Leave();

	SDL_FreeSurface(pal_surface);
	SDL_FreePalette(palette);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void dx_reinit()
{
	int lockCount;

	sgMemCrit.Enter();
	ClearCursor();
	lockCount = sgdwLockCount;

	while (sgdwLockCount != 0)
		unlock_buf_priv();

	dx_cleanup();

	drawpanflag = 255;

	dx_init(ghMainWnd);

	while (lockCount != 0) {
		lock_buf_priv();
		lockCount--;
	}

	sgMemCrit.Leave();
}

HRESULT CreatePalette()
{
	palette = SDL_AllocPalette(256);
	if (palette == NULL) {
		SDL_Log(SDL_GetError());
		return (HRESULT)0x80000002L; //DDERR_OUTOFMEMORY
	}

	return DVL_DS_OK;
}

HRESULT BltFast(DWORD dwX, DWORD dwY, LPRECT lpSrcRect)
{
	int w = lpSrcRect->right - lpSrcRect->left + 1;
	int h = lpSrcRect->bottom - lpSrcRect->top + 1;

	SDL_Rect src_rect = { lpSrcRect->left, lpSrcRect->top, w, h };
	SDL_Rect dst_rect = { (int)dwX, (int)dwY, w, h };

	// Convert from 8-bit to 32-bit
	if (SDL_BlitSurface(pal_surface, &src_rect, surface, &dst_rect) <= -1) {
		SDL_Log(SDL_GetError());
		return DVL_E_FAIL;
	}

	bufferUpdated = true;

	return DVL_S_OK;
}

HRESULT RenderPresent()
{
	assert(!SDL_MUSTLOCK(surface));

	if (!bufferUpdated) {
		return DVL_S_OK;
	}

#ifdef USE_SDL1
	if (SDL_Flip(surface) <= -1) {
		SDL_Log(SDL_GetError());
	}
#else
	if (renderer) {
		if (SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch) <= -1) { //pitch is 2560
			SDL_Log(SDL_GetError());
		}

		// Clear buffer to avoid artifacts in case the window was resized
		if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) <= -1) { // TODO only do this if window was resized
			SDL_Log(SDL_GetError());
		}

		if (SDL_RenderClear(renderer) <= -1) {
			SDL_Log(SDL_GetError());
		}

		if (SDL_RenderCopy(renderer, texture, NULL, NULL) <= -1) {
			SDL_Log(SDL_GetError());
		}
		SDL_RenderPresent(renderer);
	} else {
		if (SDL_UpdateWindowSurface(window) <= -1) {
			SDL_Log(SDL_GetError());
		}
	}
#endif

	bufferUpdated = false;

	return DVL_S_OK;
}

void PaletteGetEntries(DWORD dwNumEntries, LPPALETTEENTRY lpEntries)
{
	for (DWORD i = 0; i < dwNumEntries; i++) {
		lpEntries[i].peFlags = 0;
		lpEntries[i].peRed = system_palette[i].peRed;
		lpEntries[i].peGreen = system_palette[i].peGreen;
		lpEntries[i].peBlue = system_palette[i].peBlue;
	}
}

void PaletteSetEntries(DWORD dwCount, LPPALETTEENTRY lpEntries)
{
	for (DWORD i = 0; i < dwCount; i++) {
		system_palette[i].peFlags = 0;
		system_palette[i].peRed = lpEntries[i].peRed;
		system_palette[i].peGreen = lpEntries[i].peGreen;
		system_palette[i].peBlue = lpEntries[i].peBlue;
	}
	palette_update();
}
} // namespace dvl
