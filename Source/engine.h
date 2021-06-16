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
#include <array>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <tuple>
#include <utility>

// We include `cinttypes` here so that it is included before `inttypes.h`
// to work around a bug in older GCC versions on some platforms,
// where including `inttypes.h` before `cinttypes` leads to missing
// defines for `PRIuMAX` et al. SDL transitively includes `inttypes.h`.
// See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97044
#include <cinttypes>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "appfat.h"
#include "engine/point.hpp"
#include "engine/size.hpp"
#include "miniwin/miniwin.h"
#include "utils/stdcompat/cstddef.hpp"

#define TILE_WIDTH 64
#define TILE_HEIGHT 32

namespace devilution {

struct Rectangle {
	Point position;
	Size size;
};

inline byte *CelGetFrameStart(byte *pCelBuff, int nCel)
{
	const uint32_t *pFrameTable = reinterpret_cast<const std::uint32_t *>(pCelBuff);

	return &pCelBuff[SDL_SwapLE32(pFrameTable[nCel])];
}

template <typename T>
constexpr uint32_t LoadLE32(const T *b)
{
	static_assert(sizeof(T) == 1, "invalid argument");

	return ((uint32_t)(b)[3] << 24) | ((uint32_t)(b)[2] << 16) | ((uint32_t)(b)[1] << 8) | (uint32_t)(b)[0];
}

template <typename T>
constexpr uint32_t LoadBE32(const T *b)
{
	static_assert(sizeof(T) == 1, "invalid argument");

	return ((uint32_t)(b)[0] << 24) | ((uint32_t)(b)[1] << 16) | ((uint32_t)(b)[2] << 8) | (uint32_t)(b)[3];
}

inline byte *CelGetFrame(byte *pCelBuff, int nCel, int *nDataSize)
{
	const uint32_t nCellStart = LoadLE32(&pCelBuff[nCel * sizeof(std::uint32_t)]);
	*nDataSize = LoadLE32(&pCelBuff[(nCel + 1) * sizeof(std::uint32_t)]) - nCellStart;
	return &pCelBuff[nCellStart];
}

inline const byte *CelGetFrame(const byte *pCelBuff, int nCel, int *nDataSize)
{
	const uint32_t nCellStart = LoadLE32(&pCelBuff[nCel * sizeof(std::uint32_t)]);
	*nDataSize = LoadLE32(&pCelBuff[(nCel + 1) * sizeof(std::uint32_t)]) - nCellStart;
	return &pCelBuff[nCellStart];
}

struct FrameHeader {
	uint16_t row0;
	uint16_t row32;
	uint16_t row64;
	uint16_t row96;
	uint16_t row128;
};

inline const byte *CelGetFrameClipped(const byte *pCelBuff, int nCel, int *nDataSize)
{
	const byte *pRLEBytes = CelGetFrame(pCelBuff, nCel, nDataSize);

	FrameHeader frameHeader;
	memcpy(&frameHeader, pRLEBytes, sizeof(FrameHeader));

	uint16_t nDataStart = SDL_SwapLE16(frameHeader.row0);
	*nDataSize -= nDataStart;

	return &pRLEBytes[nDataStart];
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

	std::uint8_t &operator[](Point p) const
	{
		return *at(p.x, p.y);
	}

	uint8_t *at(int x, int y) const
	{
		return static_cast<uint8_t *>(surface->pixels) + region.x + x + surface->pitch * (region.y + y);
	}

	uint8_t *begin() const
	{
		return at(0, 0);
	}
	uint8_t *end() const
	{
		return at(0, region.h);
	}

	/**
	 * @brief Set the value of a single pixel if it is in bounds.
	 * @param point Target buffer coordinate
	 * @param col Color index from current palette
	 */
	void SetPixel(Point position, std::uint8_t col) const
	{
		if (InBounds(position))
			(*this)[position] = col;
	}

	/**
	 * @brief Line width of the raw underlying byte buffer.
	 * May be wider than its logical width (for power-of-2 alignment).
	 */
	int pitch() const
	{
		return surface->pitch;
	}

	bool InBounds(Point position) const
	{
		return position.x >= 0 && position.y >= 0 && position.x < region.w && position.y < region.h;
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

	/**
	 * @brief Clips srcRect and targetPosition to this output buffer.
	 */
	void Clip(SDL_Rect *srcRect, Point *targetPosition) const
	{
		if (targetPosition->x < 0) {
			srcRect->x -= targetPosition->x;
			srcRect->w += targetPosition->x;
			targetPosition->x = 0;
		}
		if (targetPosition->y < 0) {
			srcRect->y -= targetPosition->y;
			srcRect->h += targetPosition->y;
			targetPosition->y = 0;
		}
		if (targetPosition->x + srcRect->w > region.w) {
			srcRect->w = region.w - targetPosition->x;
		}
		if (targetPosition->y + srcRect->h > region.h) {
			srcRect->h = region.h - targetPosition->y;
		}
	}

	/**
	 * @brief Copies the `srcRect` portion of the given buffer to this buffer at `targetPosition`.
	 */
	void BlitFrom(const CelOutputBuffer &src, SDL_Rect srcRect, Point targetPosition) const;

	/**
	 * @brief Copies the `srcRect` portion of the given buffer to this buffer at `targetPosition`.
	 * Source pixels with index 0 are not copied.
	 */
	void BlitFromSkipColorIndexZero(const CelOutputBuffer &src, SDL_Rect srcRect, Point targetPosition) const;
};

/**
 * @brief Draw a horizontal line segment in the target buffer (left to right)
 * @param out Target buffer
 * @param from Start of the line segment
 * @param width
 * @param colorIndex Color index from current palette
 */
void DrawHorizontalLine(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex);

/** Same as DrawHorizontalLine but without bounds clipping. */
void UnsafeDrawHorizontalLine(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex);

/**
 * @brief Draw a vertical line segment in the target buffer (top to bottom)
 * @param out Target buffer
 * @param from Start of the line segment
 * @param height
 * @param colorIndex Color index from current palette
 */
void DrawVerticalLine(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex);

/** Same as DrawVerticalLine but without bounds clipping. */
void UnsafeDrawVerticalLine(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex);

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
Direction GetDirection(Point start, Point destination);

/**
 * @brief Calculate Width2 from the orginal Width
 * Width2 is needed for savegame compatiblity and to render animations centered
 * @return Returns Width2
 */
int CalculateWidth2(int width);

void SetRndSeed(int32_t s);
int32_t AdvanceRndSeed();
int32_t GetRndSeed();
uint32_t GetLCGEngineState();
int32_t GenerateRnd(int32_t v);

void PlayInGameMovie(const char *pszMovie);

} // namespace devilution
