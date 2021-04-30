/**
 * @file engine.h
 *
 *  of basic engine helper functions:
 * - Sprite blitting
 * - Drawing
 * - Angle calculation
 * - RNG
 * - Memory allocation
 * - File loading
 * - Video playback
 */
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <SDL.h>
#include <cstdint>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "appfat.h"
#include "miniwin/miniwin.h"

namespace devilution {

#if defined(__cpp_lib_clamp)
using std::clamp;
#else
template <typename T>
constexpr const T &clamp(const T &x, const T &lower, const T &upper)
{
	return std::min(std::max(x, lower), upper);
}
#endif

#define MemFreeDbg(p)       \
	{                       \
		void *p__p;         \
		p__p = p;           \
		p = NULL;           \
		mem_free_dbg(p__p); \
	}

enum direction : uint8_t {
	DIR_S,
	DIR_SW,
	DIR_W,
	DIR_NW,
	DIR_N,
	DIR_NE,
	DIR_E,
	DIR_SE,
	DIR_OMNI,
};

struct Point {
	int x;
	int y;

	bool operator==(const Point &other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const Point &other) const
	{
		return !(*this == other);
	}

	Point &operator+=(const Point &other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Point &operator-=(const Point &other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	friend Point operator+(Point a, const Point &b)
	{
		a += b;
		return a;
	}

	friend Point operator-(Point a, const Point &b)
	{
		a -= b;
		return a;
	}
};

struct ActorPosition {
	Point tile;
	/** Future tile position. Set at start of walking animation. */
	Point future;
	/** Tile position of player. Set via network on player input. */
	Point last;
	/** Most recent position in dPlayer. */
	Point old;
	/** Pixel offset from tile. */
	Point offset;
	/** Same as offset but contains the value in a higher range */
	Point offset2;
	/** Pixel velocity while walking. Indirectly applied to offset via _pvar6/7 */
	Point velocity;
	/** Used for referring to position of player when finishing moving one tile (also used to define target coordinates for spells and ranged attacks) */
	Point temp;
};

// `malloc` that returns a user-friendly error on OOM.
//
// Defined as a macro so that:
// 1. We provide the correct location for the OOM error.
// 2. Get better attribution from memory profilers.
#define DiabloAllocPtr(NUM_BYTES)                                                                                    \
	[](std::size_t num_bytes) {                                                                                      \
		BYTE *ptr = static_cast<BYTE *>(std::malloc(num_bytes));                                                     \
		constexpr char kMesage[] = "System memory exhausted.\n"                                                      \
		                           "Make sure you have at least 64MB of free system memory before running the game"; \
		if (ptr == NULL)                                                                                             \
			ErrDlg("Out of Memory Error", kMesage, __FILE__, __LINE__);                                              \
		return ptr;                                                                                                  \
	}(NUM_BYTES)

#define mem_free_dbg(PTR) \
	std::free(PTR)

inline BYTE *CelGetFrameStart(BYTE *pCelBuff, int nCel)
{
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pCelBuff;

	return pCelBuff + SDL_SwapLE32(pFrameTable[nCel]);
}

template <typename T>
constexpr uint32_t LoadLE32(const T *b)
{
	static_assert(sizeof(T) == 1, "invalid argument");

#if BYTE_ORDER == LITTLE_ENDIAN
	return ((uint32_t)(b)[3] << 24) | ((uint32_t)(b)[2] << 16) | ((uint32_t)(b)[1] << 8) | (uint32_t)(b)[0];
#else /* BIG ENDIAN */
	return ((uint32_t)(b)[0] << 24) | ((uint32_t)(b)[1] << 16) | ((uint32_t)(b)[2] << 8) | (uint32_t)(b)[3];
#endif
}

template <typename T>
constexpr uint32_t LoadBE32(const T *b)
{
	static_assert(sizeof(T) == 1, "invalid argument");

#if BYTE_ORDER == LITTLE_ENDIAN
	return ((uint32_t)(b)[0] << 24) | ((uint32_t)(b)[1] << 16) | ((uint32_t)(b)[2] << 8) | (uint32_t)(b)[3];
#else /* BIG ENDIAN */
	return ((uint32_t)(b)[3] << 24) | ((uint32_t)(b)[2] << 16) | ((uint32_t)(b)[1] << 8) | (uint32_t)(b)[0];
#endif
}

inline BYTE *CelGetFrame(BYTE *pCelBuff, int nCel, int *nDataSize)
{
	DWORD nCellStart;

	nCellStart = LoadLE32(&pCelBuff[nCel * 4]);
	*nDataSize = LoadLE32(&pCelBuff[(nCel + 1) * 4]) - nCellStart;
	return pCelBuff + nCellStart;
}

inline BYTE *CelGetFrameClipped(BYTE *pCelBuff, int nCel, int *nDataSize)
{
	DWORD nDataStart;
	BYTE *pRLEBytes = CelGetFrame(pCelBuff, nCel, nDataSize);

	nDataStart = pRLEBytes[1] << 8 | pRLEBytes[0];
	*nDataSize -= nDataStart;

	return pRLEBytes + nDataStart;
}

struct CelOutputBuffer {
	// 8-bit palletized surface.
	SDL_Surface *surface;
	SDL_Rect region;

	CelOutputBuffer()
	    : surface(NULL)
	    , region(SDL_Rect { 0, 0, 0, 0 })
	{
	}

	explicit CelOutputBuffer(SDL_Surface *surface)
	    : surface(surface)
	    , region(SDL_Rect { 0, 0, (Uint16)surface->w, (Uint16)surface->h })
	{
	}

	CelOutputBuffer(SDL_Surface *surface, SDL_Rect region)
	    : surface(surface)
	    , region(region)
	{
	}

	CelOutputBuffer(const CelOutputBuffer &other)
	    : surface(other.surface)
	    , region(other.region)
	{
	}

	void operator=(const CelOutputBuffer &other)
	{
		surface = other.surface;
		region = other.region;
	}

	/**
	 * @brief Allocate a buffer that owns its underlying data.
	 */
	static CelOutputBuffer Alloc(std::size_t width, std::size_t height)
	{
		return CelOutputBuffer(SDL_CreateRGBSurfaceWithFormat(0, width, height, 8, SDL_PIXELFORMAT_INDEX8));
	}

	/**
	 * @brief Free the underlying data.
	 *
	 * Only use this if the buffer owns its data.
	 */
	void Free()
	{
		SDL_FreeSurface(this->surface);
		this->surface = NULL;
	}

	int w() const
	{
		return region.w;
	}
	int h() const
	{
		return region.h;
	}

	BYTE *at(int x, int y) const
	{
		return static_cast<BYTE *>(surface->pixels) + region.x + x + surface->pitch * (region.y + y);
	}

	BYTE *begin() const
	{
		return at(0, 0);
	}
	BYTE *end() const
	{
		return at(0, region.h);
	}

	/**
	 * @brief Line width of the raw underlying byte buffer.
	 * May be wider than its logical width (for power-of-2 alignment).
	 */
	int pitch() const
	{
		return surface->pitch;
	}

	bool in_bounds(Sint16 x, Sint16 y) const
	{
		return x >= 0 && y >= 0 && x < region.w && y < region.h;
	}

	/**
	 * @brief Returns a subregion of the given buffer.
	 */
	CelOutputBuffer subregion(Sint16 x, Sint16 y, Uint16 w, Uint16 h) const
	{
		// In SDL1 SDL_Rect x and y are Sint16. Cast explicitly to avoid a compiler warning.
		using CoordType = decltype(SDL_Rect {}.x);
		return CelOutputBuffer(
		    surface,
		    SDL_Rect {
		        static_cast<CoordType>(region.x + x),
		        static_cast<CoordType>(region.y + y),
		        w, h });
	}

	/**
	 * @brief Returns a buffer that starts at `y` of height `h`.
	 */
	CelOutputBuffer subregionY(Sint16 y, Sint16 h) const
	{
		SDL_Rect subregion = region;
		subregion.y += y;
		subregion.h = h;
		return CelOutputBuffer(surface, subregion);
	}
};

/**
 * @brief Blit CEL sprite to the back buffer at the given coordinates
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelDrawTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @briefBlit CEL sprite to the given buffer, does not perform bounds-checking.
 * @param out Target buffer
 * @param x Cordinate in the target buffer
 * @param y Cordinate in the target buffer
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of cel
 */
void CelDrawUnsafeTo(const CelOutputBuffer &out, int x, int y, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Same as CelDrawTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedDrawTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelDrawLightTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, BYTE *tbl);

/**
 * @brief Same as CelDrawLightTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedDrawLightTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Same as CelBlitLightTransSafeTo
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedBlitLightTransTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates, translated to a red hue
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 * @param light Light shade to use
 */
void CelDrawLightRedTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);

/**
 * @brief Blit CEL sprite to the given buffer, checks for drawing outside the buffer.
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 */
void CelBlitSafeTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth);

/**
 * @brief Same as CelClippedDrawTo but checks for drawing outside the buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedDrawSafeTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Blit CEL sprite, and apply lighting, to the given buffer, checks for drawing outside the buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 * @param tbl Palette translation table
 */
void CelBlitLightSafeTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *tbl);

/**
 * @brief Same as CelBlitLightSafeTo but with stippled transparancy applied
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 */
void CelBlitLightTransSafeTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth);

/**
 * @brief Same as CelDrawLightRedTo but checks for drawing outside the buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of cel
 * @param light Light shade to use
 */
void CelDrawLightRedSafeTo(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the target buffer at the given coordianates
 * @param out Target buffer
 * @param col Color index from current palette
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff CEL buffer
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 * @param skipColorIndexZero If true, color in index 0 will be treated as transparent (these are typically used for shadows in sprites)
 */
void CelBlitOutlineTo(const CelOutputBuffer &out, BYTE col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, bool skipColorIndexZero = true);

/**
 * @brief Set the value of a single pixel in the back buffer, checks bounds
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param col Color index from current palette
 */
void SetPixel(const CelOutputBuffer &out, int sx, int sy, BYTE col);

/**
 * @brief Blit CL2 sprite, to the back buffer at the given coordianates
 * @param out Output buffer
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2Draw(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the given buffer at the given coordianates
 * @param col Color index from current palette
 * @param out Output buffer
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2DrawOutline(const CelOutputBuffer &out, BYTE col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Blit CL2 sprite, and apply a given lighting, to the given buffer at the given coordianates
 * @param out Output buffer
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 * @param light Light shade to use
 */
void Cl2DrawLightTbl(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);

/**
 * @brief Blit CL2 sprite, and apply lighting, to the given buffer at the given coordinates
 * @param out Output buffer
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2DrawLight(const CelOutputBuffer &out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Draw a line in the target buffer
 * @param out Target buffer
 * @param x0 Back buffer coordinate
 * @param y0 Back buffer coordinate
 * @param x1 Back buffer coordinate
 * @param y1 Back buffer coordinate
 * @param color_index Color index from current palette
 */
void DrawLineTo(const CelOutputBuffer &out, int x0, int y0, int x1, int y1, BYTE color_index);

/**
 * Draws a half-transparent rectangle by blacking out odd pixels on odd lines,
 * even pixels on even lines.
 *
 * @brief Render a transparent black rectangle
 * @param out Target buffer
 * @param sx Screen coordinate
 * @param sy Screen coordinate
 * @param width Rectangle width
 * @param height Rectangle height
 */
void DrawHalfTransparentRectTo(const CelOutputBuffer &out, int sx, int sy, int width, int height);

/**
 * @brief Calculate the best fit direction between two points
 * @param start Tile coordinate
 * @param destination Tile coordinate
 * @return A value from the direction enum
 */
direction GetDirection(Point start, Point destination);

/**
 * @brief Calculate Width2 from the orginal Width
 * Width2 is needed for savegame compatiblity and to render animations centered
 * @return Returns Width2
 */
int CalculateWidth2(int width);

void SetRndSeed(int32_t s);
int32_t AdvanceRndSeed();
int32_t GetRndSeed();
int32_t GenerateRnd(int32_t v);
BYTE *LoadFileInMem(const char *pszName, DWORD *pdwFileLen);
DWORD LoadFileWithMem(const char *pszName, BYTE *p);
void Cl2ApplyTrans(BYTE *p, BYTE *ttbl, int nCel);
void PlayInGameMovie(const char *pszMovie);

} // namespace devilution
