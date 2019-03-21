#include "dx.h"

#include "devilution.h"
#include "stubs.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

BYTE *gpBuffer;

IDirectDraw *lpDDInterface;
IDirectDrawSurface *lpDDSPrimary;
IDirectDrawSurface *lpDDSBackBuf;
IDirectDrawPalette *lpDDPalette;

char gbBackBuf; // unread
char gbEmulate; // unread

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

/** 32-bit in-memory backbuffer surface */
SDL_Surface *surface;

/** 8-bit surface wrapper around #gpBuffer */
SDL_Surface *pal_surface;
/** Currently active palette */
SDL_Palette *palette;

/**
 * Is #sdl_pal_surface dirty?
 *
 * This is required so the front buffer would not be updated twice per game loop in unlock_buf_priv().
 */
bool surface_dirty;

//
// DirectDraw COM interface stub implementations
//

class StubSurface : public IDirectDrawSurface {
	virtual ULONG Release()
	{
		UNIMPLEMENTED();
	};

	virtual HRESULT AddAttachedSurface(LPDIRECTDRAWSURFACE lpDDSAttachedSurface)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT AddOverlayDirtyRect(LPRECT lpRect)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags,
	    LPDDBLTFX lpDDBltFx)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT BltBatch(LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags)
	{
		UNIMPLEMENTED();
	}

	virtual HRESULT BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans)
	{
		DUMMY_ONCE();

		assert(lpDDSrcSurface == lpDDSBackBuf);

		int w = lpSrcRect->right - lpSrcRect->left + 1;
		int h = lpSrcRect->bottom - lpSrcRect->top + 1;

		SDL_Rect src_rect = { lpSrcRect->left, lpSrcRect->top, w, h };
		SDL_Rect dst_rect = { (int)dwX, (int)dwY, w, h };

		// Convert from 8-bit to 32-bit
		if (SDL_BlitSurface(pal_surface, &src_rect, surface, &dst_rect) != 0) {
			SDL_Log("SDL_BlitSurface: %s\n", SDL_GetError());
			return DVL_E_FAIL;
		}

		surface_dirty = true;
		return DVL_S_OK;
	}

	virtual HRESULT DeleteAttachedSurface(DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSAttachedSurface)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT EnumAttachedSurfaces(LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT EnumOverlayZOrders(DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT Flip(LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetAttachedSurface(LPDDSCAPS lpDDSCaps, LPDIRECTDRAWSURFACE *lplpDDAttachedSurface)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetBltStatus(DWORD dwFlags)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetCaps(LPDDSCAPS lpDDSCaps)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetClipper(LPDIRECTDRAWCLIPPER *lplpDDClipper)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey)
	{
		UNIMPLEMENTED();
	}

	virtual HRESULT GetDC(HDC *lphDC)
	{
		DUMMY_ONCE();
		return DVL_S_OK;
	}

	virtual HRESULT GetFlipStatus(DWORD dwFlags)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetOverlayPosition(LPLONG lplX, LPLONG lplY)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetPalette(LPDIRECTDRAWPALETTE *lplpDDPalette)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT Initialize(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc)
	{
		UNIMPLEMENTED();
	}

	virtual HRESULT IsLost()
	{
		DUMMY_ONCE();
		return DVL_S_OK;
	}

	virtual HRESULT Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
	{
		UNIMPLEMENTED();
	}

	virtual HRESULT ReleaseDC(HDC hDC)
	{
		DUMMY_ONCE();
		return DVL_S_OK;
	}

	virtual HRESULT Restore()
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT SetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT SetOverlayPosition(LONG lX, LONG lY)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT SetPalette(LPDIRECTDRAWPALETTE lpDDPalette)
	{
		DUMMY();
		return DVL_S_OK;
	}
	virtual HRESULT Unlock(LPVOID lpSurfaceData)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE lpDDDestSurface, LPRECT lpDestRect,
	    DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT UpdateOverlayDisplay(DWORD dwFlags)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSReference)
	{
		UNIMPLEMENTED();
	}
};

class StubPalette : public IDirectDrawPalette {
	virtual ULONG Release()
	{
		UNIMPLEMENTED();
	};

	virtual HRESULT GetCaps(LPDWORD lpdwCaps)
	{
		UNIMPLEMENTED();
	};
	virtual HRESULT GetEntries(DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries)
	{
		for (int i = 0; i < dwNumEntries; i++) {
			lpEntries[i].peFlags = 0;
			lpEntries[i].peRed = system_palette[i].peRed;
			lpEntries[i].peGreen = system_palette[i].peGreen;
			lpEntries[i].peBlue = system_palette[i].peBlue;
		}
		return DVL_S_OK;
	};
	virtual HRESULT Initialize(LPDIRECTDRAW lpDD, DWORD dwFlags, LPPALETTEENTRY lpDDColorTable)
	{
		UNIMPLEMENTED();
	};
	virtual HRESULT SetEntries(DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries)
	{
		for (int i = 0; i < dwCount; i++) {
			system_palette[i].peFlags = 0;
			system_palette[i].peRed = lpEntries[i].peRed;
			system_palette[i].peGreen = lpEntries[i].peGreen;
			system_palette[i].peBlue = lpEntries[i].peBlue;
		}
		palette_update();
		return DVL_S_OK;
	};
};

class StubDraw : public IDirectDraw {
	virtual ULONG Release()
	{
		UNIMPLEMENTED();
	};

	virtual HRESULT Compact()
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER *lplpDDClipper, IUnknown *pUnkOuter)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette,
	    IUnknown *pUnkOuter)
	{
		DUMMY();
		return DVL_S_OK;
	}
	virtual HRESULT CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface,
	    IUnknown *pUnkOuter)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT DuplicateSurface(LPDIRECTDRAWSURFACE lpDDSurface, LPDIRECTDRAWSURFACE *lplpDupDDSurface)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext,
	    LPDDENUMMODESCALLBACK lpEnumModesCallback)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC lpDDSD, LPVOID lpContext,
	    LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT FlipToGDISurface()
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetCaps(LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetDisplayMode(LPDDSURFACEDESC lpDDSurfaceDesc)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetFourCCCodes(LPDWORD lpNumCodes, LPDWORD lpCodes)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetGDISurface(LPDIRECTDRAWSURFACE *lplpGDIDDSurface)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetMonitorFrequency(LPDWORD lpdwFrequency)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetScanLine(LPDWORD lpdwScanLine)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT GetVerticalBlankStatus(BOOL *lpbIsInVB)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT Initialize(GUID *lpGUID)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT RestoreDisplayMode()
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP)
	{
		UNIMPLEMENTED();
	}
	virtual HRESULT WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent)
	{
		DUMMY_ONCE();
		return DVL_S_OK;
	}
};

static StubDraw stub_draw;
static StubSurface stub_surface;
static StubPalette stub_palette;

//
// Main functions
//

void dx_init(HWND hWnd)
{
	DUMMY();
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
		return;
	}

	char scaleQuality[2] = "2";
	DvlStringSetting("scaling quality", scaleQuality, 2);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaleQuality);
	if (SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT) != 0) {
		SDL_Log("SDL_RenderSetLogicalSize: %s\n", SDL_GetError());
		return;
	}

	surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	if (surface == NULL) {
		SDL_Log("SDL_CreateRGBSurface: %s\n", SDL_GetError());
		return;
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (texture == NULL) {
		SDL_Log("SDL_CreateTexture: %s\n", SDL_GetError());
		return;
	}

	palette = SDL_AllocPalette(256);
	if (palette == NULL) {
		SDL_Log("SDL_AllocPalette: %s\n", SDL_GetError());
		return;
	}

	const int pitch = 64 + SCREEN_WIDTH + 64;
	gpBuffer = (BYTE *)malloc(656 * 768);
	gpBufEnd += (uintptr_t)gpBuffer;

	pal_surface = SDL_CreateRGBSurfaceFrom(gpBuffer, pitch, 160 + SCREEN_HEIGHT + 16, 8, pitch, 0, 0, 0, 0);
	if (pal_surface == NULL) {
		SDL_Log("SDL_CreateRGBSurfaceFrom: %s\n", SDL_GetError());
		return;
	}

	if (SDL_SetSurfacePalette(pal_surface, palette) != 0) {
		SDL_Log("SDL_SetSurfacePalette: %s\n", SDL_GetError());
		return;
	}

	MainWndProc(NULL, DVL_WM_ACTIVATEAPP, true, 0);

	lpDDInterface = &stub_draw;
	lpDDSPrimary = &stub_surface;
	lpDDSBackBuf = &stub_surface;
	lpDDPalette = &stub_palette;
	palette_init();
}

void dx_cleanup()
{
	DUMMY();
}

/** Copy the palette surface to the main backbuffer */
void sdl_update_entire_surface()
{
	assert(surface && pal_surface);
	SDL_Rect src_rect = { 64, 160, SCREEN_WIDTH, SCREEN_HEIGHT };
	if (SDL_BlitSurface(pal_surface, &src_rect, surface, NULL) != 0) {
		SDL_Log("SDL_BlitSurface: %s\n", SDL_GetError());
	}
}

void sdl_present_surface()
{
	assert(!SDL_MUSTLOCK(surface));
	if (SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch) != 0) { //pitch is 2560
		SDL_Log("SDL_UpdateTexture: %s\n", SDL_GetError());
	}

	// Clear the entire screen to our selected color.
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	if (SDL_RenderCopy(renderer, texture, NULL, NULL) != 0) {
		SDL_Log("SDL_RenderCopy: %s\n", SDL_GetError());
	}
	SDL_RenderPresent(renderer);

	surface_dirty = false;
}

void j_lock_buf_priv(BYTE idx)
{
	j_unlock_buf_priv(idx); // what is idx?
}

void j_unlock_buf_priv(BYTE idx)
{
	gpBufEnd -= (uintptr_t)gpBufEnd;

	if (!surface_dirty) {
		return;
	}

	sdl_present_surface();
}

void dx_reinit()
{
	UNIMPLEMENTED();
}

} // namespace dvl
