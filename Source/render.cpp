/**
 * @file render.cpp
 *
 * Implementation of functionality for rendering the level tiles.
 */
#include "render.h"

#include <climits>

#include "lighting.h"
#include "options.h"

namespace devilution {

#define NO_OVERDRAW

namespace {

enum {
	RT_SQUARE,
	RT_TRANSPARENT,
	RT_LTRIANGLE,
	RT_RTRIANGLE,
	RT_LTRAPEZOID,
	RT_RTRAPEZOID
};

/** Fully transparent variant of WallMask. */
const DWORD WallMask_FullyTrasparent[TILE_HEIGHT] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
};
/** Transparent variant of RightMask. */
const DWORD RightMask_Transparent[TILE_HEIGHT] = {
	0xC0000000,
	0xF0000000,
	0xFC000000,
	0xFF000000,
	0xFFC00000,
	0xFFF00000,
	0xFFFC0000,
	0xFFFF0000,
	0xFFFFC000,
	0xFFFFF000,
	0xFFFFFC00,
	0xFFFFFF00,
	0xFFFFFFC0,
	0xFFFFFFF0,
	0xFFFFFFFC,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Transparent variant of LeftMask. */
const DWORD LeftMask_Transparent[TILE_HEIGHT] = {
	0x00000003,
	0x0000000F,
	0x0000003F,
	0x000000FF,
	0x000003FF,
	0x00000FFF,
	0x00003FFF,
	0x0000FFFF,
	0x0003FFFF,
	0x000FFFFF,
	0x003FFFFF,
	0x00FFFFFF,
	0x03FFFFFF,
	0x0FFFFFFF,
	0x3FFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Specifies the draw masks used to render transparency of the right side of tiles. */
const DWORD RightMask[TILE_HEIGHT] = {
	0xEAAAAAAA,
	0xF5555555,
	0xFEAAAAAA,
	0xFF555555,
	0xFFEAAAAA,
	0xFFF55555,
	0xFFFEAAAA,
	0xFFFF5555,
	0xFFFFEAAA,
	0xFFFFF555,
	0xFFFFFEAA,
	0xFFFFFF55,
	0xFFFFFFEA,
	0xFFFFFFF5,
	0xFFFFFFFE,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Specifies the draw masks used to render transparency of the left side of tiles. */
const DWORD LeftMask[TILE_HEIGHT] = {
	0xAAAAAAAB,
	0x5555555F,
	0xAAAAAABF,
	0x555555FF,
	0xAAAAABFF,
	0x55555FFF,
	0xAAAABFFF,
	0x5555FFFF,
	0xAAABFFFF,
	0x555FFFFF,
	0xAABFFFFF,
	0x55FFFFFF,
	0xABFFFFFF,
	0x5FFFFFFF,
	0xBFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Specifies the draw masks used to render transparency of wall tiles. */
const DWORD WallMask[TILE_HEIGHT] = {
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555,
	0xAAAAAAAA,
	0x55555555
};
/** Fully opaque mask */
const DWORD SolidMask[TILE_HEIGHT] = {
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};
/** Used to mask out the left half of the tile diamond and only render additional content */
const DWORD RightFoliageMask[TILE_HEIGHT] = {
	0xFFFFFFFF,
	0x3FFFFFFF,
	0x0FFFFFFF,
	0x03FFFFFF,
	0x00FFFFFF,
	0x003FFFFF,
	0x000FFFFF,
	0x0003FFFF,
	0x0000FFFF,
	0x00003FFF,
	0x00000FFF,
	0x000003FF,
	0x000000FF,
	0x0000003F,
	0x0000000F,
	0x00000003,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};
/** Used to mask out the left half of the tile diamond and only render additional content */
const DWORD LeftFoliageMask[TILE_HEIGHT] = {
	0xFFFFFFFF,
	0xFFFFFFFC,
	0xFFFFFFF0,
	0xFFFFFFC0,
	0xFFFFFF00,
	0xFFFFFC00,
	0xFFFFF000,
	0xFFFFC000,
	0xFFFF0000,
	0xFFFC0000,
	0xFFF00000,
	0xFFC00000,
	0xFF000000,
	0xFC000000,
	0xF0000000,
	0xC0000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

inline int count_leading_zeros(DWORD mask)
{
	// Note: This function assumes that the argument is not zero,
	// which means there is at least one bit set.
	static_assert(
	    sizeof(DWORD) == sizeof(uint32_t),
	    "count_leading_zeros: DWORD must be 32bits");
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_clz(mask);
#else
	// Count the number of leading zeros using binary search.
	int n = 0;
	if ((mask & 0xFFFF0000) == 0)
		n += 16, mask <<= 16;
	if ((mask & 0xFF000000) == 0)
		n += 8, mask <<= 8;
	if ((mask & 0xF0000000) == 0)
		n += 4, mask <<= 4;
	if ((mask & 0xC0000000) == 0)
		n += 2, mask <<= 2;
	if ((mask & 0x80000000) == 0)
		n += 1;
	return n;
#endif
}

template <typename F>
void foreach_set_bit(DWORD mask, const F &f)
{
	int i = 0;
	while (mask != 0) {
		int z = count_leading_zeros(mask);
		i += z, mask <<= z;
		for (; mask & 0x80000000; i++, mask <<= 1)
			f(i);
	}
}

inline void DoRenderLine(BYTE *dst, BYTE *src, size_t n, BYTE *tbl, DWORD mask)
{
	if (mask == 0xFFFFFFFF) {                // Opaque line
		if (light_table_index == lightmax) { // Complete darkness
			memset(dst, 0, n);
		} else if (light_table_index == 0) { // Fully lit
			memcpy(dst, src, n);
		} else { // Partially lit
			for (size_t i = 0; i < n; i++) {
				dst[i] = tbl[src[i]];
			}
		}
	} else {
		// The number of iterations is anyway limited by the size of the mask.
		// So we can limit it by ANDing the mask with another mask that only keeps
		// iterations that are lower than n. We can now avoid testing if i < n
		// at every loop iteration.
		assert(n != 0 && n <= sizeof(DWORD) * CHAR_BIT);
		mask &= DWORD(-1) << ((sizeof(DWORD) * CHAR_BIT) - n);

		if (sgOptions.Graphics.bBlendedTransparancy) { // Blended transparancy
			if (light_table_index == lightmax) {       // Complete darkness
				for (size_t i = 0; i < n; i++, mask <<= 1) {
					if ((mask & 0x80000000) != 0)
						dst[i] = 0;
					else
						dst[i] = paletteTransparencyLookup[0][dst[i]];
				}
			} else if (light_table_index == 0) { // Fully lit
				for (size_t i = 0; i < n; i++, mask <<= 1) {
					if ((mask & 0x80000000) != 0)
						dst[i] = src[i];
					else
						dst[i] = paletteTransparencyLookup[dst[i]][src[i]];
				}
			} else { // Partially lit
				for (size_t i = 0; i < n; i++, mask <<= 1) {
					if ((mask & 0x80000000) != 0)
						dst[i] = tbl[src[i]];
					else
						dst[i] = paletteTransparencyLookup[dst[i]][tbl[src[i]]];
				}
			}
		} else {                                 // Stippled transparancy
			if (light_table_index == lightmax) { // Complete darkness
				foreach_set_bit(mask, [=](int i) { dst[i] = 0; });
			} else if (light_table_index == 0) { // Fully lit
				foreach_set_bit(mask, [=](int i) { dst[i] = src[i]; });
			} else { // Partially lit
				foreach_set_bit(mask, [=](int i) { dst[i] = tbl[src[i]]; });
			}
		}
	}
}

#if DVL_HAVE_ATTRIBUTE(always_inline) || (defined(__GNUC__) && !defined(__clang__))
__attribute__((always_inline))
#endif
inline void
RenderLine(BYTE *dst_begin, BYTE *dst_end, BYTE **dst, BYTE **src, int n, BYTE *tbl, DWORD mask)
{
#ifdef NO_OVERDRAW
	if (*dst >= dst_begin && *dst <= dst_end)
#endif
		DoRenderLine(*dst, *src, n, tbl, mask);
	(*src) += n;
	(*dst) += n;
}

} // namespace

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif

void
RenderTile(CelOutputBuffer out, int x, int y)
{
	int i, j;
	char c, v, tile;
	BYTE *src, *tbl;
	DWORD m, *pFrameTable;
	const DWORD *mask;

	// TODO: Get rid of overdraw by rendering edge tiles separately.
	out.region.x -= BUFFER_BORDER_LEFT;
	out.region.y -= BUFFER_BORDER_TOP;
	out.region.w += BUFFER_BORDER_LEFT;
	out.region.h += BUFFER_BORDER_TOP;
	x += BUFFER_BORDER_LEFT;
	y += BUFFER_BORDER_TOP;

	pFrameTable = (DWORD *)pDungeonCels;

	src = &pDungeonCels[SDL_SwapLE32(pFrameTable[level_cel_block & 0xFFF])];
	tile = (level_cel_block & 0x7000) >> 12;
	tbl = &pLightTbl[256 * light_table_index];

	// The mask defines what parts of the tile is opaque
	mask = &SolidMask[TILE_HEIGHT - 1];

	if (cel_transparency_active) {
		if (arch_draw_type == 0) {
			if (sgOptions.Graphics.bBlendedTransparancy) // Use a fully transparent mask
				mask = &WallMask_FullyTrasparent[TILE_HEIGHT - 1];
			else
				mask = &WallMask[TILE_HEIGHT - 1];
		}
		if (arch_draw_type == 1 && tile != RT_LTRIANGLE) {
			c = block_lvid[level_piece_id];
			if (c == 1 || c == 3) {
				if (sgOptions.Graphics.bBlendedTransparancy) // Use a fully transparent mask
					mask = &LeftMask_Transparent[TILE_HEIGHT - 1];
				else
					mask = &LeftMask[TILE_HEIGHT - 1];
			}
		}
		if (arch_draw_type == 2 && tile != RT_RTRIANGLE) {
			c = block_lvid[level_piece_id];
			if (c == 2 || c == 3) {
				if (sgOptions.Graphics.bBlendedTransparancy) // Use a fully transparent mask
					mask = &RightMask_Transparent[TILE_HEIGHT - 1];
				else
					mask = &RightMask[TILE_HEIGHT - 1];
			}
		}
	} else if (arch_draw_type && cel_foliage_active) {
		if (tile != RT_TRANSPARENT) {
			return;
		}
		if (arch_draw_type == 1) {
			mask = &LeftFoliageMask[TILE_HEIGHT - 1];
		}
		if (arch_draw_type == 2) {
			mask = &RightFoliageMask[TILE_HEIGHT - 1];
		}
	}

#ifdef _DEBUG
	if ((GetAsyncKeyState(DVL_VK_MENU) & 0x8000) != 0) {
		mask = &SolidMask[TILE_HEIGHT - 1];
	}
#endif

	BYTE *dst_begin = out.at(0, BUFFER_BORDER_TOP);
	BYTE *dst_end = out.end();
	BYTE *dst = out.at(x, y);
	const int dst_pitch = out.pitch();
	switch (tile) {
	case RT_SQUARE:
		for (i = TILE_HEIGHT; i != 0; i--, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			RenderLine(dst_begin, dst_end, &dst, &src, TILE_WIDTH / 2, tbl, *mask);
		}
		break;
	case RT_TRANSPARENT:
		for (i = TILE_HEIGHT; i != 0; i--, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			m = *mask;
			for (j = TILE_WIDTH / 2; j != 0; j -= v, v == TILE_WIDTH / 2 ? m = 0 : m <<= v) {
				v = *src++;
				if (v >= 0) {
					RenderLine(dst_begin, dst_end, &dst, &src, v, tbl, m);
				} else {
					v = -v;
					dst += v;
				}
			}
		}
		break;
	case RT_LTRIANGLE:
		for (i = TILE_HEIGHT - 2; i >= 0; i -= 2, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			src += i & 2;
			dst += i;
			RenderLine(dst_begin, dst_end, &dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
		}
		for (i = 2; i != TILE_WIDTH / 2; i += 2, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			src += i & 2;
			dst += i;
			RenderLine(dst_begin, dst_end, &dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
		}
		break;
	case RT_RTRIANGLE:
		for (i = TILE_HEIGHT - 2; i >= 0; i -= 2, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			RenderLine(dst_begin, dst_end, &dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
			src += i & 2;
			dst += i;
		}
		for (i = 2; i != TILE_HEIGHT; i += 2, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			RenderLine(dst_begin, dst_end, &dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
			src += i & 2;
			dst += i;
		}
		break;
	case RT_LTRAPEZOID:
		for (i = TILE_HEIGHT - 2; i >= 0; i -= 2, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			src += i & 2;
			dst += i;
			RenderLine(dst_begin, dst_end, &dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
		}
		for (i = TILE_HEIGHT / 2; i != 0; i--, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			RenderLine(dst_begin, dst_end, &dst, &src, TILE_WIDTH / 2, tbl, *mask);
		}
		break;
	case RT_RTRAPEZOID:
		for (i = TILE_HEIGHT - 2; i >= 0; i -= 2, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			RenderLine(dst_begin, dst_end, &dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
			src += i & 2;
			dst += i;
		}
		for (i = TILE_HEIGHT / 2; i != 0; i--, dst -= dst_pitch + TILE_WIDTH / 2, mask--) {
			RenderLine(dst_begin, dst_end, &dst, &src, TILE_WIDTH / 2, tbl, *mask);
		}
		break;
	}
}

namespace {

// A tile is a diamond shape consisting of 2 triangles, upper and lower.
// The tile's bounding box is 64 by 31 px.
//
// Heights of the upper and lower triangles of the tile:
constexpr int UpperHeight = TILE_HEIGHT / 2 - 1;
constexpr int LowerHeight = TILE_HEIGHT / 2;
constexpr int Height = LowerHeight + LowerHeight;
constexpr int XStep = 2;

// Blit with left and vertical clipping.
void BlitBlackTileClipLeftY(BYTE *dst, int dst_pitch, int sx, int skip_lower_bottom, int skip_lower_top, int skip_upper_bottom, int skip_upper_top)
{
	// Lower triangle (drawn bottom to top):
	for (int i = skip_lower_bottom + 1; i <= LowerHeight - skip_lower_top; ++i, dst -= dst_pitch + XStep) {
		const int w = 2 * XStep * i;
		const int cur_x = sx + TILE_WIDTH / 2 - XStep * i;
		if (cur_x >= 0) {
			memset(dst, 0, w);
		} else if (-cur_x <= w) {
			memset(dst - cur_x, 0, w + cur_x);
		}
	}
	dst += 2 * XStep;
	// Upper triangle (drawn bottom to top):
	for (int i = skip_upper_bottom, j = UpperHeight - skip_upper_bottom; i < UpperHeight - skip_upper_top; ++i, --j, dst -= dst_pitch - XStep) {
		const int w = 2 * XStep * j;
		const int cur_x = sx + TILE_WIDTH / 2 - XStep * j;
		if (cur_x >= 0) {
			memset(dst, 0, w);
		} else if (-cur_x <= w) {
			memset(dst - cur_x, 0, w + cur_x);
		} else {
			break;
		}
	}
}

// Blit with right and vertical clipping.
void BlitBlackTileClipWidthY(BYTE *dst, int dst_pitch, int max_w, int skip_lower_bottom, int skip_lower_top, int skip_upper_bottom, int skip_upper_top)
{
	// Lower triangle (drawn bottom to top):
	for (int i = skip_lower_bottom + 1; i <= LowerHeight - skip_lower_top; ++i, dst -= dst_pitch + XStep) {
		if (max_w > 0)
			memset(dst, 0, std::min(2 * XStep * i, max_w));
		max_w += 2 * XStep * i;
	}
	dst += 2 * XStep;
	// Upper triangle (drawn bottom to top):
	for (int i = skip_upper_bottom, j = UpperHeight - skip_upper_bottom; i < UpperHeight - skip_upper_top; ++i, --j, dst -= dst_pitch - XStep) {
		max_w -= 2 * XStep;
		if (max_w <= 0)
			break;
		memset(dst, 0, std::min(2 * XStep * j, max_w));
	}
}

// Blit with vertical clipping only.
void BlitBlackTileClipY(BYTE *dst, int dst_pitch, int skip_lower_bottom, int skip_lower_top, int skip_upper_bottom, int skip_upper_top)
{
	// Lower triangle (drawn bottom to top):
	for (int i = skip_lower_bottom + 1; i <= LowerHeight - skip_lower_top; ++i, dst -= dst_pitch + XStep) {
		memset(dst, 0, 2 * XStep * i);
	}
	dst += 2 * XStep;
	// Upper triangle (drawn bottom to top):
	for (int i = skip_upper_bottom, j = UpperHeight - skip_upper_bottom; i < UpperHeight - skip_upper_top; ++i, --j, dst -= dst_pitch - XStep) {
		memset(dst, 0, 2 * XStep * j);
	}
}

// Blit a black tile without clipping (must be fully in bounds).
void BlitBlackTileFull(BYTE *dst, int dst_pitch)
{
	// Tile is fully in bounds, can use constant loop boundaries.
	// Lower triangle (drawn bottom to top):
	for (int i = 1; i <= LowerHeight; ++i, dst -= dst_pitch + XStep) {
		memset(dst, 0, 2 * XStep * i);
	}
	dst += 2 * XStep;
	// Upper triangle (drawn bottom to to top):
	for (int i = 0, j = UpperHeight; i < UpperHeight; ++i, --j, dst -= dst_pitch - XStep) {
		memset(dst, 0, 2 * XStep * j);
	}
}

} // namespace

void world_draw_black_tile(const CelOutputBuffer &out, int sx, int sy)
{
	if (sx <= -TILE_WIDTH || sx >= out.region.w || sy < 0 || sy + 1 > out.region.h + Height)
		return;

	// Initial out position: the bottom corner of the lower triangle.
	int out_x = sx + TILE_WIDTH / 2 - XStep;
	int out_y = sy;

	// How many lines to skip for the lower and upper triangles:
	int skip_lower_bottom = 0;
	int skip_lower_top = 0;
	int skip_upper_bottom = 0;
	int skip_upper_top = 0;
	if (sy >= out.region.h) {
		skip_lower_bottom = sy - out.region.h + 1;
		out_y -= skip_lower_bottom;
		if (skip_lower_bottom > LowerHeight) {
			skip_upper_bottom = skip_lower_bottom - LowerHeight;
			skip_lower_bottom = LowerHeight;
		}
		out_x += XStep * (skip_upper_bottom - skip_lower_bottom);
	} else if (sy + 1 < Height) {
		skip_upper_top = Height - (sy + 1) - 1;
		if (skip_upper_top > UpperHeight) {
			skip_lower_top = skip_upper_top - UpperHeight;
			skip_upper_top = UpperHeight;
		}
	}

	BYTE *dst = out.at(out_x, out_y);
	if (out_x < TILE_WIDTH / 2 - XStep) {
		BlitBlackTileClipLeftY(dst, out.pitch(), sx, skip_lower_bottom, skip_lower_top, skip_upper_bottom, skip_upper_top);
	} else if (sx > out.region.w - TILE_WIDTH) {
		BlitBlackTileClipWidthY(dst, out.pitch(), out.region.w - out_x, skip_lower_bottom, skip_lower_top, skip_upper_bottom, skip_upper_top);
	} else if (skip_upper_top != 0 || skip_lower_bottom != 0) {
		BlitBlackTileClipY(dst, out.pitch(), skip_lower_bottom, skip_lower_top, skip_upper_bottom, skip_upper_top);
	} else {
		BlitBlackTileFull(dst, out.pitch());
	}
}

} // namespace devilution
