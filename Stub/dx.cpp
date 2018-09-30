#include "miniwin_sdl.h"

#include "../types.h"
#include "stubs.h"

Screen *gpBuffer;

IDirectDraw *lpDDInterface;
IDirectDrawSurface *lpDDSPrimary;
IDirectDrawSurface *lpDDSBackBuf;
IDirectDrawPalette *lpDDPalette;

char gbBackBuf; // unread
char gbEmulate; // unread

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

SDL_Surface *surface, *pal_surface;
SDL_Palette *palette;

bool surface_dirty;

class StubSurface : public IDirectDrawSurface
{
	virtual HRESULT QueryInterface(void *, void **) { UNIMPLEMENTED(); };
	virtual ULONG AddRef() { UNIMPLEMENTED(); };
	virtual ULONG Release() { UNIMPLEMENTED(); };

	virtual HRESULT AddAttachedSurface(IDirectDrawSurface *) { UNIMPLEMENTED(); };
	virtual HRESULT AddOverlayDirtyRect(RECT *) { UNIMPLEMENTED(); };
	virtual HRESULT Blt(RECT *, IDirectDrawSurface *, RECT *, unsigned int, void *) { UNIMPLEMENTED(); };
	virtual HRESULT BltBatch(void *, unsigned int, unsigned int) { UNIMPLEMENTED(); };

	virtual HRESULT BltFast(unsigned int x, unsigned int y, IDirectDrawSurface *surf, RECT *rect, unsigned int)
	{
		DUMMY_ONCE();

		assert(surf == lpDDSBackBuf);

		int w = rect->right - rect->left + 1;
		int h = rect->bottom - rect->top + 1;
		SDL_Rect src_rect = {rect->left, rect->top, w, h};
		SDL_Rect dst_rect = {(int)x, (int)y, w, h};
		SDL_CHECK(SDL_BlitSurface(pal_surface, &src_rect, surface, &dst_rect));

		surface_dirty = true;
		return S_OK;
	};

	virtual HRESULT DeleteAttachedSurface(unsigned int, IDirectDrawSurface *) { UNIMPLEMENTED(); };
	virtual HRESULT EnumAttachedSurfaces(void *, void *) { UNIMPLEMENTED(); };
	virtual HRESULT EnumOverlayZOrders(unsigned int, void *, void *) { UNIMPLEMENTED(); };
	virtual HRESULT Flip(IDirectDrawSurface *, unsigned int) { UNIMPLEMENTED(); };
	virtual HRESULT GetAttachedSurface(_DDSCAPS *, IDirectDrawSurface **) { UNIMPLEMENTED(); };
	virtual HRESULT GetBltStatus(unsigned int) { UNIMPLEMENTED(); };
	virtual HRESULT GetCaps(_DDSCAPS *) { UNIMPLEMENTED(); };
	virtual HRESULT GetClipper(void **) { UNIMPLEMENTED(); };
	virtual HRESULT GetColorKey(unsigned int, _DDCOLORKEY *) { UNIMPLEMENTED(); };
	virtual HRESULT GetDC(void **)
	{
		DUMMY_ONCE();
		return S_OK;
	};
	virtual HRESULT GetFlipStatus(unsigned int) { UNIMPLEMENTED(); };
	virtual HRESULT GetOverlayPosition(int *, int *) { UNIMPLEMENTED(); };
	virtual HRESULT GetPalette(IDirectDrawPalette **) { UNIMPLEMENTED(); };
	virtual HRESULT GetPixelFormat(_DDPIXELFORMAT *) { UNIMPLEMENTED(); };
	virtual HRESULT GetSurfaceDesc(_DDSURFACEDESC *) { UNIMPLEMENTED(); };
	virtual HRESULT Initialize(IDirectDraw *, _DDSURFACEDESC *) { UNIMPLEMENTED(); };

	virtual HRESULT IsLost()
	{
		DUMMY_ONCE();
		return S_OK;
	};

	virtual HRESULT Lock(RECT *, _DDSURFACEDESC *, unsigned int, void *) { UNIMPLEMENTED(); };
	virtual HRESULT ReleaseDC(void *)
	{
		DUMMY_ONCE();
		return S_OK;
	};
	virtual HRESULT Restore() { UNIMPLEMENTED(); };
	virtual HRESULT SetClipper(void *) { UNIMPLEMENTED(); };
	virtual HRESULT SetColorKey(unsigned int, _DDCOLORKEY *) { UNIMPLEMENTED(); };
	virtual HRESULT SetOverlayPosition(int, int) { UNIMPLEMENTED(); };
	virtual HRESULT SetPalette(IDirectDrawPalette *) { UNIMPLEMENTED(); };
	virtual HRESULT Unlock(void *) { UNIMPLEMENTED(); };
	virtual HRESULT UpdateOverlay(RECT *, IDirectDrawSurface *, RECT *, unsigned int, void *) { UNIMPLEMENTED(); };
	virtual HRESULT UpdateOverlayDisplay(unsigned int) { UNIMPLEMENTED(); };
	virtual HRESULT UpdateOverlayZOrder(unsigned int, IDirectDrawSurface *) { UNIMPLEMENTED(); };
};

class StubPalette : public IDirectDrawPalette
{
	virtual HRESULT QueryInterface(void *, void **) { UNIMPLEMENTED(); };
	virtual ULONG AddRef() { UNIMPLEMENTED(); };
	virtual ULONG Release() { UNIMPLEMENTED(); };

	virtual HRESULT GetCaps(LPDWORD) { UNIMPLEMENTED(); };
	virtual HRESULT GetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY) { UNIMPLEMENTED(); };
	virtual HRESULT Initialize(LPDIRECTDRAW, DWORD, LPPALETTEENTRY) { UNIMPLEMENTED(); };
	virtual HRESULT SetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY) { UNIMPLEMENTED(); };
};

class StubDraw : public IDirectDraw
{
	virtual HRESULT QueryInterface(void *, void **) { UNIMPLEMENTED(); };
	virtual ULONG AddRef() { UNIMPLEMENTED(); };
	virtual ULONG Release() { UNIMPLEMENTED(); };

	virtual HRESULT Compact() { UNIMPLEMENTED(); };
	virtual HRESULT CreateClipper(DWORD, void **, IUnknown *) { UNIMPLEMENTED(); };
	virtual HRESULT CreatePalette(DWORD, LPPALETTEENTRY, IDirectDrawPalette **, IUnknown *) { UNIMPLEMENTED(); };
	virtual HRESULT CreateSurface(LPDDSURFACEDESC, IDirectDrawSurface **, IUnknown *) { UNIMPLEMENTED(); };
	virtual HRESULT DuplicateSurface(LPDIRECTDRAWSURFACE, IDirectDrawSurface **) { UNIMPLEMENTED(); };
	virtual HRESULT EnumDisplayModes(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK) { UNIMPLEMENTED(); };
	virtual HRESULT EnumSurfaces(DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK) { UNIMPLEMENTED(); };
	virtual HRESULT FlipToGDISurface() { UNIMPLEMENTED(); };
	virtual HRESULT GetCaps(LPDDCAPS, LPDDCAPS) { UNIMPLEMENTED(); };
	virtual HRESULT GetDisplayMode(LPDDSURFACEDESC) { UNIMPLEMENTED(); };
	virtual HRESULT GetFourCCCodes(LPDWORD, LPDWORD) { UNIMPLEMENTED(); };
	virtual HRESULT GetGDISurface(IDirectDrawSurface **) { UNIMPLEMENTED(); };
	virtual HRESULT GetMonitorFrequency(LPDWORD) { UNIMPLEMENTED(); };
	virtual HRESULT GetScanLine(LPDWORD) { UNIMPLEMENTED(); };
	virtual HRESULT GetVerticalBlankStatus(BOOL *) { UNIMPLEMENTED(); };
	virtual HRESULT Initialize(GUID *) { UNIMPLEMENTED(); };
	virtual HRESULT RestoreDisplayMode() { UNIMPLEMENTED(); };
	virtual HRESULT SetCooperativeLevel(HWND, DWORD) { UNIMPLEMENTED(); };
	virtual HRESULT SetDisplayMode(DWORD, DWORD, DWORD) { UNIMPLEMENTED(); };

	virtual HRESULT WaitForVerticalBlank(DWORD, HANDLE)
	{
		DUMMY();
		return S_OK;
	};
};

StubDraw stub_draw;
StubSurface stub_surface;
StubPalette stub_palette;

void __fastcall dx_init(HWND hWnd)
{
	DUMMY();

	SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));

	window = SDL_CreateWindow("devil-test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	assert(window);

	// Hack since ShowCursor is called before dx_init()
	SDL_ShowCursor(SDL_DISABLE);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	assert(renderer);

	// FUTURE: Use SDL_CreateRGBSurfaceWithFormat with SDL_PIXELFORMAT_RGBA8888
	surface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	assert(surface);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
	assert(texture);

	palette = SDL_AllocPalette(256);
	assert(palette);

	gbActive = TRUE;

	lpDDInterface = &stub_draw;
	lpDDSPrimary = &stub_surface;
	lpDDSBackBuf = &stub_surface;
	lpDDPalette = &stub_palette;
}

static void sdl_update_entire_surface()
{
	assert(surface && pal_surface);
	SDL_Rect src_rect = {64, 160, 640, 480};
	SDL_CHECK(SDL_BlitSurface(pal_surface, &src_rect, surface, NULL));
}

static void sdl_present_surface()
{
	assert(!SDL_MUSTLOCK(surface));
	SDL_CHECK(SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch));

	SDL_CHECK(SDL_RenderCopy(renderer, texture, NULL, NULL));
	SDL_RenderPresent(renderer);

	surface_dirty = false;
}

BOOL STORMAPI SDrawUpdatePalette(unsigned int firstentry, unsigned int numentries, PALETTEENTRY *pPalEntries, int a4)
{
	DUMMY_PRINT("first: %d num: %d", firstentry, numentries);
	assert(palette);

	assert(firstentry == 0);
	assert(numentries == 256);

	SDL_Color colors[256];
	for (int i = firstentry; i < numentries; i++) {
		SDL_Color *c = &colors[i];
		PALETTEENTRY *p = &pPalEntries[i];
		c->r = p->peRed;
		c->g = p->peGreen;
		c->b = p->peBlue;
		c->a = SDL_ALPHA_OPAQUE;
	}

	SDL_CHECK(SDL_SetPaletteColors(palette, colors, firstentry, numentries));

	if (pal_surface) {
		sdl_update_entire_surface();
		sdl_present_surface();
	}

	return TRUE;
}

const int pitch = 640 + 64 + 64;

void __cdecl lock_buf_priv()
{
	if (!gpBuffer) {
		gpBuffer = (Screen *)malloc(sizeof(Screen));
		gpBufEnd += (unsigned int)gpBuffer;

		pal_surface = SDL_CreateRGBSurfaceFrom(gpBuffer, pitch, 160 + 480 + 16, 8, pitch, 0, 0, 0, 0);
		assert(pal_surface);

		SDL_CHECK(SDL_SetSurfacePalette(pal_surface, palette));
	}
}

void __cdecl unlock_buf_priv()
{
	gpBufEnd -= (unsigned int)gpBufEnd;

	if (!surface_dirty) {
		return;
	}

	sdl_present_surface();
}

void __cdecl dx_reinit()
{
	UNIMPLEMENTED();
}

WINBOOL WINAPI TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
	DUMMY_ONCE();

	assert(window);
	SDL_SetWindowTitle(window, lpString);

	return TRUE;
}
