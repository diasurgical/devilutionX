/**
 * @file render.cpp
 *
 * Implementation of functionality for rendering the level tiles.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

#define NO_OVERDRAW

enum {
	RT_SQUARE,
	RT_TRANSPARENT,
	RT_LTRIANGLE,
	RT_RTRIANGLE,
	RT_LTRAPEZOID,
	RT_RTRAPEZOID
};

/** Fully transparent variant of WallMask. */
static DWORD WallMask_FullyTrasparent[TILE_HEIGHT] = {
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
static DWORD RightMask_Transparent[TILE_HEIGHT] = {
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
static DWORD LeftMask_Transparent[TILE_HEIGHT] = {
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
static DWORD RightMask[TILE_HEIGHT] = {
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
static DWORD LeftMask[TILE_HEIGHT] = {
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
static DWORD WallMask[TILE_HEIGHT] = {
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
static DWORD SolidMask[TILE_HEIGHT] = {
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
static DWORD RightFoliageMask[TILE_HEIGHT] = {
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
static DWORD LeftFoliageMask[TILE_HEIGHT] = {
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

inline static int count_leading_zeros(DWORD mask)
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

inline static void RenderLine(BYTE **dst, BYTE **src, int n, BYTE *tbl, DWORD mask)
{
#ifdef NO_OVERDRAW
	if (*dst < &gpBuffer[BUFFER_WIDTH * SCREEN_Y] || *dst > gpBufEnd) {
		goto skip;
	}
#endif

	if (mask == 0xFFFFFFFF) {                // Opaque line
		if (light_table_index == lightmax) { // Complete darkness
			memset(*dst, 0, n);
		} else if (light_table_index == 0) { // Fully lit
			memcpy(*dst, *src, n);
		} else { // Partially lit
			for (int i = 0; i < n; i++) {
				(*dst)[i] = tbl[(*src)[i]];
			}
		}
	} else {
		// The number of iterations is anyway limited by the size of the mask.
		// So we can limit it by ANDing the mask with another mask that only keeps
		// iterations that are lower than n. We can now avoid testing if i < n
		// at every loop iteration.
		assert(n != 0 && n <= sizeof(DWORD) * CHAR_BIT);
		mask &= DWORD(-1) << ((sizeof(DWORD) * CHAR_BIT) - n);

		if (sgOptions.bBlendedTransparancy) {    // Blended transparancy
			if (light_table_index == lightmax) { // Complete darkness
				for (int i = 0; i < n; i++, mask <<= 1) {
					if (mask & 0x80000000)
						(*dst)[i] = 0;
					else
						(*dst)[i] = paletteTransparencyLookup[0][(*dst)[i]];
				}
			} else if (light_table_index == 0) { // Fully lit
				for (int i = 0; i < n; i++, mask <<= 1) {
					if (mask & 0x80000000)
						(*dst)[i] = (*src)[i];
					else
						(*dst)[i] = paletteTransparencyLookup[(*dst)[i]][(*src)[i]];
				}
			} else { // Partially lit
				for (int i = 0; i < n; i++, mask <<= 1) {
					if (mask & 0x80000000)
						(*dst)[i] = tbl[(*src)[i]];
					else
						(*dst)[i] = paletteTransparencyLookup[(*dst)[i]][tbl[(*src)[i]]];
				}
			}
		} else {                                 // Stippled transparancy
			if (light_table_index == lightmax) { // Complete darkness
				foreach_set_bit(mask, [=](int i) { (*dst)[i] = 0; });
			} else if (light_table_index == 0) { // Fully lit
				foreach_set_bit(mask, [=](int i) { (*dst)[i] = (*src)[i]; });
			} else { // Partially lit
				foreach_set_bit(mask, [=](int i) { (*dst)[i] = tbl[(*src)[i]]; });
			}
		}
	}

skip:
	(*src) += n;
	(*dst) += n;
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
/**
 * @brief Blit current world CEL to the given buffer
 * @param pBuff Output buffer
 */
void
RenderTile(BYTE *pBuff)
{
	int i, j;
	char c, v, tile;
	BYTE *src, *dst, *tbl;
	DWORD m, *mask, *pFrameTable;

	dst = pBuff;
	pFrameTable = (DWORD *)pDungeonCels;

	src = &pDungeonCels[SDL_SwapLE32(pFrameTable[level_cel_block & 0xFFF])];
	tile = (level_cel_block & 0x7000) >> 12;
	tbl = &pLightTbl[256 * light_table_index];

	// The mask defines what parts of the tile is opaque
	mask = &SolidMask[TILE_HEIGHT - 1];

	if (cel_transparency_active) {
		if (arch_draw_type == 0) {
			if (sgOptions.bBlendedTransparancy) // Use a fully transparent mask
				mask = &WallMask_FullyTrasparent[TILE_HEIGHT - 1];
			else
				mask = &WallMask[TILE_HEIGHT - 1];
		}
		if (arch_draw_type == 1 && tile != RT_LTRIANGLE) {
			c = block_lvid[level_piece_id];
			if (c == 1 || c == 3) {
				if (sgOptions.bBlendedTransparancy) // Use a fully transparent mask
					mask = &LeftMask_Transparent[TILE_HEIGHT - 1];
				else
					mask = &LeftMask[TILE_HEIGHT - 1];
			}
		}
		if (arch_draw_type == 2 && tile != RT_RTRIANGLE) {
			c = block_lvid[level_piece_id];
			if (c == 2 || c == 3) {
				if (sgOptions.bBlendedTransparancy) // Use a fully transparent mask
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
	if (GetAsyncKeyState(DVL_VK_MENU) & 0x8000) {
		mask = &SolidMask[TILE_HEIGHT - 1];
	}
#endif

	switch (tile) {
	case RT_SQUARE:
		for (i = TILE_HEIGHT; i != 0; i--, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			RenderLine(&dst, &src, TILE_WIDTH / 2, tbl, *mask);
		}
		break;
	case RT_TRANSPARENT:
		for (i = TILE_HEIGHT; i != 0; i--, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			m = *mask;
			for (j = TILE_WIDTH / 2; j != 0; j -= v, v == TILE_WIDTH / 2 ? m = 0 : m <<= v) {
				v = *src++;
				if (v >= 0) {
					RenderLine(&dst, &src, v, tbl, m);
				} else {
					v = -v;
					dst += v;
				}
			}
		}
		break;
	case RT_LTRIANGLE:
		for (i = TILE_HEIGHT - 2; i >= 0; i -= 2, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			src += i & 2;
			dst += i;
			RenderLine(&dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
		}
		for (i = 2; i != TILE_WIDTH / 2; i += 2, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			src += i & 2;
			dst += i;
			RenderLine(&dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
		}
		break;
	case RT_RTRIANGLE:
		for (i = TILE_HEIGHT - 2; i >= 0; i -= 2, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			RenderLine(&dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
			src += i & 2;
			dst += i;
		}
		for (i = 2; i != TILE_HEIGHT; i += 2, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			RenderLine(&dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
			src += i & 2;
			dst += i;
		}
		break;
	case RT_LTRAPEZOID:
		for (i = TILE_HEIGHT - 2; i >= 0; i -= 2, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			src += i & 2;
			dst += i;
			RenderLine(&dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
		}
		for (i = TILE_HEIGHT / 2; i != 0; i--, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			RenderLine(&dst, &src, TILE_WIDTH / 2, tbl, *mask);
		}
		break;
	case RT_RTRAPEZOID:
		for (i = TILE_HEIGHT - 2; i >= 0; i -= 2, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			RenderLine(&dst, &src, TILE_WIDTH / 2 - i, tbl, *mask);
			src += i & 2;
			dst += i;
		}
		for (i = TILE_HEIGHT / 2; i != 0; i--, dst -= BUFFER_WIDTH + TILE_WIDTH / 2, mask--) {
			RenderLine(&dst, &src, TILE_WIDTH / 2, tbl, *mask);
		}
		break;
	}
}

/**
 * @brief Render a black tile
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 */
void world_draw_black_tile(int sx, int sy)
{
	int i, j;
	BYTE *dst;

	if (sx >= SCREEN_X + SCREEN_WIDTH || sy >= SCREEN_Y + VIEWPORT_HEIGHT + TILE_WIDTH / 2)
		return;

	if (sx < SCREEN_X - (TILE_WIDTH - 4) || sy < SCREEN_Y)
		return;

	dst = &gpBuffer[sx + BUFFER_WIDTH * sy] + TILE_WIDTH / 2 - 2;

	for (i = TILE_HEIGHT - 2, j = 1; i >= 0; i -= 2, j++, dst -= BUFFER_WIDTH + 2) {
		if (dst < gpBufEnd)
			memset(dst, 0, 4 * j);
	}
	dst += 4;
	for (i = 2, j = TILE_HEIGHT / 2 - 1; i != TILE_HEIGHT; i += 2, j--, dst -= BUFFER_WIDTH - 2) {
		if (dst < gpBufEnd)
			memset(dst, 0, 4 * j);
	}
}

DEVILUTION_END_NAMESPACE
