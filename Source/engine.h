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
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <tuple>
#include <utility>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "appfat.h"
#include "miniwin/miniwin.h"

namespace devilution {

#if __cplusplus >= 201703L
using byte = std::byte;
#else
using byte = uint8_t;
#endif

#if defined(__cpp_lib_clamp)
using std::clamp;
#else
template <typename T>
constexpr const T &clamp(const T &x, const T &lower, const T &upper)
{
	return std::min(std::max(x, lower), upper);
}
#endif

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

	/**
	 * @brief Fast approximate distance between two points, using only integer arithmetic, with less than ~5% error
	 * @param other Pointer to which we want the distance
	 * @return Magnitude of vector this -> other
	 */

	int ApproxDistance(Point other) const
	{
		int min;
		int max;

		std::tie(min, max) = std::minmax(std::abs(other.x - x), std::abs(other.y - y));

		int approx = max * 1007 + min * 441;
		if (max < (min * 16))
			approx -= max * 40;

		return (approx + 512) / 1024;
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

inline byte *CelGetFrame(byte *pCelBuff, int nCel, int *nDataSize)
{
	auto frameTable = reinterpret_cast<const uint32_t *>(pCelBuff);
	const uint32_t nCellStart = SDL_SwapLE32(frameTable[nCel]);
	*nDataSize = SDL_SwapLE32(frameTable[nCel + 1]) - nCellStart;
	return &pCelBuff[nCellStart];
}

inline const byte *CelGetFrame(const byte *pCelBuff, int nCel, int *nDataSize)
{
	auto frameTable = reinterpret_cast<const uint32_t *>(pCelBuff);
	const uint32_t nCellStart = SDL_SwapLE32(frameTable[nCel]);
	*nDataSize = SDL_SwapLE32(frameTable[nCel + 1]) - nCellStart;
	return &pCelBuff[nCellStart];
}

struct FrameHeader {
	uint16_t row0;
	uint16_t row32;
	uint16_t row64;
	uint16_t row96;
	uint16_t row128;
};

inline const uint8_t *CelGetFrameClipped(const byte *pCelBuff, int nCel, int *nDataSize)
{
	const byte *pRLEBytes = CelGetFrame(pCelBuff, nCel, nDataSize);

	FrameHeader frameHeader;
	memcpy(&frameHeader, pRLEBytes, sizeof(FrameHeader));

	uint16_t nDataStart = SDL_SwapLE16(frameHeader.row0);
	*nDataSize -= nDataStart;

	return reinterpret_cast<const uint8_t *>(&pRLEBytes[nDataStart]);
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
 * Returns a pair of X coordinates containing the start (inclusive) and end (exclusive)
 * of fully transparent columns in the sprite.
 */
std::pair<int, int> MeasureSolidHorizontalBounds(const CelSprite &cel, int frame = 1);

/**
 * @brief Blit CEL sprite to the back buffer at the given coordinates
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame);

/**
 * @briefBlit CEL sprite to the given buffer, does not perform bounds-checking.
 * @param out Target buffer
 * @param x Cordinate in the target buffer
 * @param y Cordinate in the target buffer
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawUnsafeTo(const CelOutputBuffer &out, int x, int y, const CelSprite &cel, int frame);

/**
 * @brief Same as CelDrawTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedDrawTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame);

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelDrawLightTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame, uint8_t *tbl);

/**
 * @brief Same as CelDrawLightTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedDrawLightTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame);

/**
 * @brief Same as CelBlitLightTransSafeTo
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedBlitLightTransTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame);

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates, translated to a red hue
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 * @param light Light shade to use
 */
void CelDrawLightRedTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame, char light);

/**
 * @brief Blit CEL sprite to the given buffer, checks for drawing outside the buffer.
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 */
void CelBlitSafeTo(const CelOutputBuffer &out, int sx, int sy, const uint8_t *pRLEBytes, int nDataSize, int nWidth);

/**
 * @brief Same as CelClippedDrawTo but checks for drawing outside the buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 */
void CelClippedDrawSafeTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame);

/**
 * @brief Blit CEL sprite, and apply lighting, to the given buffer, checks for drawing outside the buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param tbl Palette translation table
 */
void CelBlitLightSafeTo(const CelOutputBuffer &out, int sx, int sy, const uint8_t *pRLEBytes, int nDataSize, int nWidth, uint8_t *tbl);

/**
 * @brief Same as CelBlitLightSafeTo but with stippled transparancy applied
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 */
void CelBlitLightTransSafeTo(const CelOutputBuffer &out, int sx, int sy, const uint8_t *pRLEBytes, int nDataSize, int nWidth);

/**
 * @brief Same as CelDrawLightRedTo but checks for drawing outside the buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param cel CEL sprite
 * @param frame CEL frame number
 * @param light Light shade to use
 */
void CelDrawLightRedSafeTo(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame, char light);

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the target buffer at the given coordianates
 * @param out Target buffer
 * @param col Color index from current palette
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff CEL buffer
 * @param frame CEL frame number
 * @param skipColorIndexZero If true, color in index 0 will be treated as transparent (these are typically used for shadows in sprites)
 */
void CelBlitOutlineTo(const CelOutputBuffer &out, uint8_t col, int sx, int sy, const CelSprite &cel, int frame, bool skipColorIndexZero = true);

/**
 * @brief Set the value of a single pixel in the back buffer, checks bounds
 * @param out Target buffer
 * @param point Target buffer coordinate
 * @param col Color index from current palette
 */
inline void SetPixel(const CelOutputBuffer &out, Point position, std::uint8_t col)
{
	if (!out.InBounds(position))
		return;

	out[position] = col;
}

/**
 * @brief Blit CL2 sprite, to the back buffer at the given coordianates
 * @param out Output buffer
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 */
void Cl2Draw(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame);

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the given buffer at the given coordianates
 * @param col Color index from current palette
 * @param out Output buffer
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 */
void Cl2DrawOutline(const CelOutputBuffer &out, uint8_t col, int sx, int sy, const CelSprite &cel, int frame);

/**
 * @brief Blit CL2 sprite, and apply a given lighting, to the given buffer at the given coordianates
 * @param out Output buffer
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param light Light shade to use
 */
void Cl2DrawLightTbl(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame, char light);

/**
 * @brief Blit CL2 sprite, and apply lighting, to the given buffer at the given coordinates
 * @param out Output buffer
 * @param sx Output buffer coordinate
 * @param sy Output buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 */
void Cl2DrawLight(const CelOutputBuffer &out, int sx, int sy, const CelSprite &cel, int frame);

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

void Cl2ApplyTrans(byte *p, const std::array<uint8_t, 256> &ttbl, int nCel);
void PlayInGameMovie(const char *pszMovie);

} // namespace devilution
