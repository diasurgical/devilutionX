#include "devilution.h"
#include "miniwin/ddraw.h"
#include "stubs.h"
#include <SDL.h>

#include "DiabloUI/diabloui.h"

namespace dvl {

SDL_Window *window;
SDL_Renderer *renderer;

/** Currently active palette */
SDL_Palette *palette;
SDL_Texture *texture;

/** 32-bit in-memory backbuffer surface */
SDL_Surface *surface;

/** 8-bit surface wrapper around #gpBuffer */
SDL_Surface *pal_surface;

bool bufferUpdated = false;

ULONG StubDraw::Release()
{
	DUMMY();
	SDL_FreeSurface(pal_surface);
	SDL_FreePalette(palette);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
};

HRESULT StubDraw::CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette,
    IUnknown *pUnkOuter)
{
	DUMMY();
	*lplpDDPalette = new StubPalette();

	palette = SDL_AllocPalette(256);
	if (palette == NULL) {
		SDL_Log(SDL_GetError());
		return (HRESULT)0x80000002L; //DDERR_OUTOFMEMORY
	}

	return DVL_DS_OK;
}

HRESULT StubDraw::CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface,
    IUnknown *pUnkOuter)
{
	DUMMY();

	if (!lpDDSurfaceDesc->dwWidth) {
		// TODO Get value from render/window
		lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = SDL_PIXELFORMAT_RGBA8888;
		lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = 32;

		if (renderer) {
			if (SDL_GetRendererOutputSize(renderer, (int *)&lpDDSurfaceDesc->dwWidth, (int *)&lpDDSurfaceDesc->dwHeight) <= -1) {
				SDL_Log(SDL_GetError());
			}
			surface = SDL_CreateRGBSurfaceWithFormat(0, lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight, lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount, lpDDSurfaceDesc->ddpfPixelFormat.dwFlags);
		} else {
			SDL_GetWindowSize(window, (int *)&lpDDSurfaceDesc->dwWidth, (int *)&lpDDSurfaceDesc->dwHeight);
			surface = SDL_GetWindowSurface(window);
		}
		if (surface == NULL) {
			SDL_Log(SDL_GetError());
			return (HRESULT)0x80000002L; //DDERR_OUTOFMEMORY
		}
		lpDDSurfaceDesc->lpSurface = (BYTE *)surface->pixels;
		*lplpDDSurface = new StubSurface(lpDDSurfaceDesc);
		return DVL_DS_OK;
	}

	pal_surface = SDL_CreateRGBSurfaceWithFormat(0, lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight, lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount, lpDDSurfaceDesc->ddpfPixelFormat.dwFlags);
	if (pal_surface == NULL) {
		SDL_Log(SDL_GetError());
		return (HRESULT)0x80000002L; //DDERR_OUTOFMEMORY
	}
	lpDDSurfaceDesc->lpSurface = (BYTE *)surface->pixels;
	*lplpDDSurface = new StubSurface(lpDDSurfaceDesc);

	gpBuffer = (BYTE *)pal_surface->pixels; // Hack should happen in dx.cpp, but gives error

	if (SDL_SetSurfacePalette(pal_surface, palette) <= -1) {
		SDL_Log(SDL_GetError());
		return 1; //MAKE_HRESULT(130);//DVL_MAKE_HRESULT(130);
	}

	return DVL_DS_OK;
}

HRESULT StubDraw::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	DUMMY();
	return DVL_DS_OK;
}

HRESULT StubDraw::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
{
	DUMMY();

	width = dwWidth;
	height = dwHeight;

	return DVL_DS_OK;
}

HRESULT StubDraw::WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent)
{
	DUMMY_ONCE();
	return DVL_DS_OK;
}

StubSurface::StubSurface(LPDDSURFACEDESC lpDDSurfaceDesc)
{
}

StubSurface::~StubSurface()
{
}

ULONG StubSurface::Release()
{
	DUMMY();
	return 0;
};

HRESULT StubSurface::BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans)
{
	DUMMY_ONCE();

	assert(lpDDSrcSurface == lpDDSBackBuf); // is src pal_surface

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

HRESULT StubSurface::GetCaps(LPDDSCAPS lpDDSCaps)
{
	DUMMY();
	return DVL_S_OK;
}

HRESULT StubSurface::GetDC(HDC *lphDC)
{
	DUMMY_ONCE();
	return DVL_S_OK;
}

HRESULT StubSurface::GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat)
{
	lpDDPixelFormat->dwFlags = SDL_PIXELFORMAT_INDEX8;
	lpDDPixelFormat->dwRGBBitCount = 8;
	return DVL_S_OK;
}

HRESULT StubSurface::IsLost()
{
	DUMMY_ONCE();
	return DVL_S_OK;
}

HRESULT StubSurface::Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
{
	DUMMY_ONCE();
	lpDDSurfaceDesc->lpSurface = (BYTE *)pal_surface->pixels;
	return DVL_S_OK;
}

HRESULT StubSurface::ReleaseDC(HDC hDC)
{
	DUMMY_ONCE();
	return DVL_S_OK;
}

HRESULT StubSurface::Restore()
{
	UNIMPLEMENTED();
	return DVL_S_OK;
}

HRESULT StubSurface::SetPalette(LPDIRECTDRAWPALETTE lpDDPalette)
{
	DUMMY();
	return DVL_S_OK;
}

HRESULT StubSurface::Unlock(LPVOID lpSurfaceData)
{
	DUMMY_ONCE();
	assert(!SDL_MUSTLOCK(surface));

	if (!bufferUpdated) {
		return DVL_S_OK;
	}

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

	bufferUpdated = false;

	return DVL_S_OK;
}

ULONG StubPalette::Release()
{
	DUMMY();
	return 0;
};

HRESULT StubPalette::GetCaps(LPDWORD lpdwCaps)
{
	UNIMPLEMENTED();
	return DVL_S_OK;
}

HRESULT StubPalette::GetEntries(DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries)
{
	for (int i = 0; i < dwNumEntries; i++) {
		lpEntries[i].peFlags = 0;
		lpEntries[i].peRed = system_palette[i].peRed;
		lpEntries[i].peGreen = system_palette[i].peGreen;
		lpEntries[i].peBlue = system_palette[i].peBlue;
	}
	return DVL_S_OK;
}

HRESULT StubPalette::SetEntries(DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries)
{
	for (int i = 0; i < dwCount; i++) {
		system_palette[i].peFlags = 0;
		system_palette[i].peRed = lpEntries[i].peRed;
		system_palette[i].peGreen = lpEntries[i].peGreen;
		system_palette[i].peBlue = lpEntries[i].peBlue;
	}
	palette_update();
	return DVL_S_OK;
}

} // namespace dvl
