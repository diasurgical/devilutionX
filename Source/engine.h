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
#include "miniwin/miniwin.h"
#include "utils/stdcompat/cstddef.hpp"
#include "utils/stdcompat/abs.h"

#define TILE_WIDTH 64
#define TILE_HEIGHT 32

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

enum Direction : uint8_t {
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

	constexpr bool operator==(const Point &other) const
	{
		return x == other.x && y == other.y;
	}

	constexpr bool operator!=(const Point &other) const
	{
		return !(*this == other);
	}

	constexpr Point &operator+=(const Point &other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	constexpr Point &operator+=(Direction direction)
	{
		constexpr auto toPoint = [](Direction direction) -> Point {
			switch (direction) {
			case DIR_S:
				return { 1, 1 };
			case DIR_SW:
				return { 0, 1 };
			case DIR_W:
				return { -1, 1 };
			case DIR_NW:
				return { -1, 0 };
			case DIR_N:
				return { -1, -1 };
			case DIR_NE:
				return { 0, -1 };
			case DIR_E:
				return { 1, -1 };
			case DIR_SE:
				return { 1, 0 };
			default:
				return { 0, 0 };
			}
		};

		return (*this) += toPoint(direction);
	}

	constexpr Point &operator-=(const Point &other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	constexpr Point &operator*=(const float factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	constexpr Point &operator*=(const int factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	constexpr friend Point operator+(Point a, const Point &b)
	{
		a += b;
		return a;
	}

	constexpr friend Point operator+(Point a, Direction direction)
	{
		a += direction;
		return a;
	}

	constexpr friend Point operator-(Point a, const Point &b)
	{
		a -= b;
		return a;
	}

	constexpr friend Point operator-(const Point &a)
	{
		return { -a.x, -a.y };
	}

	constexpr friend Point operator*(Point a, const float factor)
	{
		a *= factor;
		return a;
	}

	constexpr friend Point operator*(Point a, const int factor)
	{
		a *= factor;
		return a;
	}

	/**
	 * @brief Fast approximate distance between two points, using only integer arithmetic, with less than ~5% error
	 * @param other Pointer to which we want the distance
	 * @return Magnitude of vector this -> other
	 */

	constexpr int ApproxDistance(Point other) const
	{
		Point offset = abs(other - *this);
		auto minMax = std::minmax(offset.x, offset.y);
		int min = minMax.first;
		int max = minMax.second;

		int approx = max * 1007 + min * 441;
		if (max < (min * 16))
			approx -= max * 40;

		return (approx + 512) / 1024;
	}

	constexpr friend Point abs(Point a)
	{
		return { abs(a.x), abs(a.y) };
	}

	constexpr int ManhattanDistance(Point other) const
	{
		Point offset = abs(*this - other);

		return offset.x + offset.y;
	}

	constexpr int WalkingDistance(Point other) const
	{
		Point offset = abs(*this - other);

		return std::max(offset.x, offset.y);
	}
};

struct Size {
	int Width;
	int Height;

	bool operator==(const Size &other) const
	{
		return Width == other.Width && Height == other.Height;
	}

	bool operator!=(const Size &other) const
	{
		return !(*this == other);
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
};

/**
 * Stores a CEL or CL2 sprite and its width(s).
 *
 * The data may be unowned.
 * Eventually we'd like to remove the unowned version.
 */
class CelSprite {
public:
	CelSprite(std::unique_ptr<byte[]> data, int width)
	    : data_(std::move(data))
	    , data_ptr_(data_.get())
	    , width_(width)
	{
	}

	CelSprite(std::unique_ptr<byte[]> data, const int *widths)
	    : data_(std::move(data))
	    , data_ptr_(data_.get())
	    , widths_(widths)
	{
	}

	/**
	 * Constructs an unowned sprite.
	 * Ideally we'd like to remove all uses of this constructor.
	 */
	CelSprite(const byte *data, int width)
	    : data_ptr_(data)
	    , width_(width)
	{
	}

	CelSprite(CelSprite &&) noexcept = default;
	CelSprite &operator=(CelSprite &&) noexcept = default;

	[[nodiscard]] const byte *Data() const
	{
		return data_ptr_;
	}

	[[nodiscard]] int Width(std::size_t frame = 1) const
	{
		return widths_ == nullptr ? width_ : widths_[frame];
	}

private:
	std::unique_ptr<byte[]> data_;
	const byte *data_ptr_;
	int width_ = 0;
	const int *widths_ = nullptr; // unowned
};

/**
 * @brief Loads a Cel sprite and sets its width
 */
CelSprite LoadCel(const char *pszName, int width);
CelSprite LoadCel(const char *pszName, const int *widths);

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
int32_t GenerateRnd(int32_t v);

size_t GetFileSize(const char *pszName);
void LoadFileData(const char *pszName, byte *buffer, size_t bufferSize);

template <typename T>
void LoadFileInMem(const char *path, T *data, std::size_t count = 0)
{
	if (count == 0)
		count = GetFileSize(path);

	LoadFileData(path, reinterpret_cast<byte *>(data), count * sizeof(T));
}

template <typename T, std::size_t N>
void LoadFileInMem(const char *path, std::array<T, N> &data)
{
	LoadFileInMem(path, &data, N);
}

/**
 * @brief Load a file in to a buffer
 * @param path Path of file
 * @param elements Number of T elements read
 * @return Buffer with content of file
 */
template <typename T = byte>
std::unique_ptr<T[]> LoadFileInMem(const char *path, size_t *elements = nullptr)
{
	const size_t fileLen = GetFileSize(path);

	if ((fileLen % sizeof(T)) != 0)
		app_fatal("File size does not align with type\n%s", path);

	if (elements != nullptr)
		*elements = fileLen / sizeof(T);

	std::unique_ptr<T[]> buf { new T[fileLen / sizeof(T)] };

	LoadFileData(path, reinterpret_cast<byte *>(buf.get()), fileLen);

	return buf;
}

void PlayInGameMovie(const char *pszMovie);

} // namespace devilution
