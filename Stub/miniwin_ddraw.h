#pragma once

#include "miniwin.h"

typedef struct _DDCOLORKEY {
	DWORD dwColorSpaceLowValue;
	DWORD dwColorSpaceHighValue;
} DDCOLORKEY;

typedef DDCOLORKEY *LPDDCOLORKEY;

typedef struct _DDSCAPS {
	DWORD dwCaps;
} DDSCAPS, *LPDDSCAPS;

typedef struct _DDPIXELFORMAT {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	union {
		DWORD dwRGBBitCount;
		DWORD dwYUVBitCount;
		DWORD dwZBufferBitDepth;
		DWORD dwAlphaBitDepth;
	};
	union {
		DWORD dwRBitMask;
		DWORD dwYBitMask;
	};
	union {
		DWORD dwGBitMask;
		DWORD dwUBitMask;
	};
	union {
		DWORD dwBBitMask;
		DWORD dwVBitMask;
	};
	union {
		DWORD dwRGBAlphaBitMask;
		DWORD dwYUVAlphaBitMask;
		DWORD dwRGBZBitMask;
		DWORD dwYUVZBitMask;
	};
} DDPIXELFORMAT, *LPDDPIXELFORMAT;

typedef struct _DDSURFACEDESC {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwHeight;
	DWORD dwWidth;
	union {
		LONG lPitch;
		DWORD dwLinearSize;
	};
	DWORD dwBackBufferCount;
	union {
		DWORD dwMipMapCount;
		DWORD dwZBufferBitDepth;
		DWORD dwRefreshRate;
	};
	DWORD dwAlphaBitDepth;
	DWORD dwReserved;
	LPVOID lpSurface;
	DDCOLORKEY ddckCKDestOverlay;
	DDCOLORKEY ddckCKDestBlt;
	DDCOLORKEY ddckCKSrcOverlay;
	DDCOLORKEY ddckCKSrcBlt;
	DDPIXELFORMAT ddpfPixelFormat;
	DDSCAPS ddsCaps;
} DDSURFACEDESC, *LPDDSURFACEDESC;

#define _FACDD 0x876
#define MAKE_DDHRESULT(code) MAKE_HRESULT(1, _FACDD, code)

#define DDERR_SURFACELOST MAKE_DDHRESULT(450)
#define DDERR_WASSTILLDRAWING MAKE_DDHRESULT(540)
#define DDERR_SURFACEBUSY MAKE_DDHRESULT(430)

typedef struct IDirectDrawPalette *LPDIRECTDRAWPALETTE;
typedef struct IDirectDrawSurface *LPDIRECTDRAWSURFACE;
typedef struct IDirectDraw *LPDIRECTDRAW;

// No methods are actually used
DECLARE_INTERFACE_(IDirectDrawPalette, IUnknown)
{
	// clang-format off
	STDMETHOD(GetCaps)(THIS_ LPDWORD lpdwCaps) PURE;
	STDMETHOD(GetEntries)(THIS_ DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries) PURE;
	STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW lpDD, DWORD dwFlags, LPPALETTEENTRY lpDDColorTable) PURE;
	STDMETHOD(SetEntries)(THIS_ DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries) PURE;
	// clang-format on
};

/*
Actually used methods:

DECLARE_INTERFACE_(IDirectDrawSurface, IUnknown) {
    STDMETHOD(GetDC)(THIS_ HDC *);
    STDMETHOD(IsLost)(THIS);
    STDMETHOD(Lock)(THIS_ LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
    STDMETHOD(ReleaseDC)(THIS_ HDC);
    STDMETHOD(Restore)(THIS);
    STDMETHOD(Unlock)(THIS_ LPVOID);
    STDMETHOD(BltFast)(THIS_ DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
    STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE);
};
*/

typedef void *LPDDBLTBATCH, *LPDDBLTFX, *LPDIRECTDRAWCLIPPER, *LPDDENUMSURFACESCALLBACK, *LPDDOVERLAYFX;
DECLARE_INTERFACE_(IDirectDrawSurface, IUnknown)
{
	// clang-format off
	STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE lpDDSAttachedSurface) PURE;
	STDMETHOD(AddOverlayDirtyRect)(THIS_ LPRECT lpRect) PURE;
	STDMETHOD(Blt)(THIS_ LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx) PURE;
	STDMETHOD(BltBatch)(THIS_ LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags) PURE;
	STDMETHOD(BltFast)(THIS_ DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans) PURE;
	STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSAttachedSurface) PURE;
	STDMETHOD(EnumAttachedSurfaces)(THIS_ LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) PURE;
	STDMETHOD(EnumOverlayZOrders)(THIS_ DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback) PURE;
	STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags) PURE;
	STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS lpDDSCaps, LPDIRECTDRAWSURFACE *lplpDDAttachedSurface) PURE;
	STDMETHOD(GetBltStatus)(THIS_ DWORD dwFlags) PURE;
	STDMETHOD(GetCaps)(THIS_ LPDDSCAPS lpDDSCaps) PURE;
	STDMETHOD(GetClipper)(THIS_ LPDIRECTDRAWCLIPPER *lplpDDClipper) PURE;
	STDMETHOD(GetColorKey)(THIS_ DWORD dwFlags, LPDDCOLORKEY lpDDColorKey) PURE;
	STDMETHOD(GetDC)(THIS_ HDC *lphDC) PURE;
	STDMETHOD(GetFlipStatus)(THIS_ DWORD dwFlags) PURE;
	STDMETHOD(GetOverlayPosition)(THIS_ LPLONG lplX, LPLONG lplY) PURE;
	STDMETHOD(GetPalette)(THIS_ LPDIRECTDRAWPALETTE *lplpDDPalette) PURE;
	STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT lpDDPixelFormat) PURE;
	STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC lpDDSurfaceDesc) PURE;
	STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc) PURE;
	STDMETHOD(IsLost)(THIS) PURE;
	STDMETHOD(Lock)(THIS_ LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent) PURE;
	STDMETHOD(ReleaseDC)(THIS_ HDC hDC) PURE;
	STDMETHOD(Restore)(THIS) PURE;
	STDMETHOD(SetClipper)(THIS_ LPDIRECTDRAWCLIPPER lpDDClipper) PURE;
	STDMETHOD(SetColorKey)(THIS_ DWORD dwFlags, LPDDCOLORKEY lpDDColorKey) PURE;
	STDMETHOD(SetOverlayPosition)(THIS_ LONG lX, LONG lY) PURE;
	STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE lpDDPalette) PURE;
	STDMETHOD(Unlock)(THIS_ LPVOID lpSurfaceData) PURE;
	STDMETHOD(UpdateOverlay)(THIS_ LPRECT lpSrcRect, LPDIRECTDRAWSURFACE lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx) PURE;
	STDMETHOD(UpdateOverlayDisplay)(THIS_ DWORD dwFlags) PURE;
	STDMETHOD(UpdateOverlayZOrder)(THIS_ DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSReference) PURE;
	// clang-format off
};

#define DDBLTFAST_WAIT 0x00000010

#define DDLOCK_WRITEONLY 0x00000020L
#define DDLOCK_WAIT 0x00000001L

#define DDPCAPS_ALLOW256 0x00000040l
#define DDPCAPS_8BIT 0x00000004l

#define PC_RESERVED 0x01
#define PC_NOCOLLAPSE 0x04

#define DDWAITVB_BLOCKBEGIN 0x00000001l

/*
Actually used methods:

DECLARE_INTERFACE_(IDirectDraw, IUnknown) {
	STDMETHOD(CreatePalette)(THIS_ DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown *);
	STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE);
};
*/

typedef void *LPDDENUMMODESCALLBACK, *LPDDCAPS;
DECLARE_INTERFACE_(IDirectDraw,IUnknown)
{
	// clang-format off
	STDMETHOD(Compact)(THIS) PURE;
	STDMETHOD(CreateClipper)(THIS_ DWORD dwFlags, LPDIRECTDRAWCLIPPER *lplpDDClipper, IUnknown *pUnkOuter) PURE;
	STDMETHOD(CreatePalette)(THIS_ DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette, IUnknown *pUnkOuter) PURE;
	STDMETHOD(CreateSurface)(THIS_ LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *pUnkOuter) PURE;
	STDMETHOD(DuplicateSurface)(THIS_ LPDIRECTDRAWSURFACE lpDDSurface, LPDIRECTDRAWSURFACE *lplpDupDDSurface) PURE;
	STDMETHOD(EnumDisplayModes)(THIS_ DWORD dwFlags, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK lpEnumModesCallback) PURE;
	STDMETHOD(EnumSurfaces)(THIS_ DWORD dwFlags, LPDDSURFACEDESC lpDDSD, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback) PURE;
	STDMETHOD(FlipToGDISurface)(THIS) PURE;
	STDMETHOD(GetCaps)(THIS_ LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps) PURE;
	STDMETHOD(GetDisplayMode)(THIS_ LPDDSURFACEDESC lpDDSurfaceDesc) PURE;
	STDMETHOD(GetFourCCCodes)(THIS_ LPDWORD lpNumCodes, LPDWORD lpCodes) PURE;
	STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE *lplpGDIDDSurface) PURE;
	STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD lpdwFrequency) PURE;
	STDMETHOD(GetScanLine)(THIS_ LPDWORD lpdwScanLine) PURE;
	STDMETHOD(GetVerticalBlankStatus)(THIS_ BOOL *lpbIsInVB) PURE;
	STDMETHOD(Initialize)(THIS_ GUID *lpGUID) PURE;
	STDMETHOD(RestoreDisplayMode)(THIS) PURE;
	STDMETHOD(SetCooperativeLevel)(THIS_ HWND hWnd, DWORD dwFlags) PURE;
	STDMETHOD(SetDisplayMode)(THIS_ DWORD dwWidth, DWORD dwHeight, DWORD dwBPP) PURE;
	STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD dwFlags, HANDLE hEvent) PURE;
	// clang-format on
};
