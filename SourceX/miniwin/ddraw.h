#include "devilution.h"
#include <SDL.h>

namespace dvl {

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;
extern SDL_Surface *surface;
extern SDL_Palette *palette;
extern SDL_Surface *pal_surface;
extern bool bufferUpdated;

struct StubDraw : public IDirectDraw {
public:
	virtual ULONG Release();
	HRESULT CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette, IUnknown *pUnkOuter);
	HRESULT CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *pUnkOuter);
	HRESULT WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent);
};

struct StubSurface : public IDirectDrawSurface {
public:
	StubSurface(LPDDSURFACEDESC lpDDSurfaceDesc);
	~StubSurface();
	virtual ULONG Release();
	HRESULT BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans);
	HRESULT GetCaps(LPDDSCAPS lpDDSCaps);
	HRESULT GetDC(HDC *lphDC);
	HRESULT GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat);
	HRESULT IsLost();
	HRESULT Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent);
	HRESULT ReleaseDC(HDC hDC);
	HRESULT Restore();
	HRESULT SetPalette(LPDIRECTDRAWPALETTE lpDDPalette);
	HRESULT Unlock(LPVOID lpSurfaceData);
};

struct StubPalette : public IDirectDrawPalette {
public:
	virtual ULONG Release();
	HRESULT GetCaps(LPDWORD lpdwCaps);
	HRESULT GetEntries(DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries);
	HRESULT SetEntries(DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries);
};

} // namespace dvl
