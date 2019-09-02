typedef struct _DDCOLORKEY {
	DWORD dwColorSpaceLowValue;
	DWORD dwColorSpaceHighValue;
} DDCOLORKEY;

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

typedef struct IDirectDrawPalette *LPDIRECTDRAWPALETTE;
typedef struct IDirectDrawSurface *LPDIRECTDRAWSURFACE;
typedef struct IDirectDraw *LPDIRECTDRAW;

// No methods are actually used
DECLARE_INTERFACE_(IDirectDrawPalette, IUnknown)
{
	// clang-format off
	STDMETHOD(GetCaps)(THIS_ LPDWORD lpdwCaps) PURE;
	STDMETHOD(GetEntries)(THIS_ DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries) PURE;
	STDMETHOD(SetEntries)(THIS_ DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries) PURE;
	// clang-format on
};

DECLARE_INTERFACE_(IDirectDrawSurface, IUnknown)
{
	// clang-format off
	STDMETHOD(BltFast)(THIS_ DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans) PURE;
	STDMETHOD(GetCaps)(THIS_ LPDDSCAPS lpDDSCaps) PURE;
	STDMETHOD(GetDC)(THIS_ HDC *lphDC) PURE;
	STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT lpDDPixelFormat) PURE;
	STDMETHOD(IsLost)(THIS) PURE;
	STDMETHOD(Lock)(THIS_ LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent) PURE;
	STDMETHOD(ReleaseDC)(THIS_ HDC hDC) PURE;
	STDMETHOD(Restore)(THIS) PURE;
	STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE lpDDPalette) PURE;
	STDMETHOD(Unlock)(THIS_ LPVOID lpSurfaceData) PURE;
	// clang-format off
};

typedef void *LPDDCAPS;
DECLARE_INTERFACE_(IDirectDraw,IUnknown)
{
	// clang-format off
	STDMETHOD(CreatePalette)(THIS_ DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette, IUnknown *pUnkOuter) PURE;
	STDMETHOD(CreateSurface)(THIS_ LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *pUnkOuter) PURE;
	STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD dwFlags, HANDLE hEvent) PURE;
	// clang-format on
};

