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

static DWORD RightMask[32] = {
	0xEAAAAAAA, 0xF5555555,
	0xFEAAAAAA, 0xFF555555,
	0xFFEAAAAA, 0xFFF55555,
	0xFFFEAAAA, 0xFFFF5555,
	0xFFFFEAAA, 0xFFFFF555,
	0xFFFFFEAA, 0xFFFFFF55,
	0xFFFFFFEA, 0xFFFFFFF5,
	0xFFFFFFFE, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF
};

static DWORD LeftMask[32] = {
	0xAAAAAAAB, 0x5555555F,
	0xAAAAAABF, 0x555555FF,
	0xAAAAABFF, 0x55555FFF,
	0xAAAABFFF, 0x5555FFFF,
	0xAAABFFFF, 0x555FFFFF,
	0xAABFFFFF, 0x55FFFFFF,
	0xABFFFFFF, 0x5FFFFFFF,
	0xBFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF
};

static DWORD WallMask[32] = {
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555,
	0xAAAAAAAA, 0x55555555
};

static DWORD SolidMask[32] = {
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF
};

static DWORD RightFoliageMask[32] = {
	0xFFFFFFFF, 0x3FFFFFFF,
	0x0FFFFFFF, 0x03FFFFFF,
	0x00FFFFFF, 0x003FFFFF,
	0x000FFFFF, 0x0003FFFF,
	0x0000FFFF, 0x00003FFF,
	0x00000FFF, 0x000003FF,
	0x000000FF, 0x0000003F,
	0x0000000F, 0x00000003,
	0x00000000, 0x00000003,
	0x0000000F, 0x0000003F,
	0x000000FF, 0x000003FF,
	0x00000FFF, 0x00003FFF,
	0x0000FFFF, 0x0003FFFF,
	0x000FFFFF, 0x003FFFFF,
	0x00FFFFFF, 0x03FFFFFF,
	0x0FFFFFFF, 0x3FFFFFFF,
};

static DWORD LeftFoliageMask[32] = {
	0xFFFFFFFF, 0xFFFFFFFC,
	0xFFFFFFF0, 0xFFFFFFC0,
	0xFFFFFF00, 0xFFFFFC00,
	0xFFFFF000, 0xFFFFC000,
	0xFFFF0000, 0xFFFC0000,
	0xFFF00000, 0xFFC00000,
	0xFF000000, 0xFC000000,
	0xF0000000, 0xC0000000,
	0x00000000, 0xC0000000,
	0xF0000000, 0xFC000000,
	0xFF000000, 0xFFC00000,
	0xFFF00000, 0xFFFC0000,
	0xFFFF0000, 0xFFFFC000,
	0xFFFFF000, 0xFFFFFC00,
	0xFFFFFF00, 0xFFFFFFC0,
	0xFFFFFFF0, 0xFFFFFFFC,
};

inline static void RenderLine(BYTE **dst, BYTE **src, int n, BYTE *tbl, DWORD mask)
{
	int i;

#ifdef NO_OVERDRAW
	if (*dst < gpBufStart || *dst > gpBufEnd) {
		*src += n;
		*dst += n;
		return;
	}
#endif

	if (mask == 0xFFFFFFFF) {
		if (light_table_index == lightmax) {
			memset(*dst, 0, n);
			(*src) += n;
			(*dst) += n;
		} else if (light_table_index == 0) {
			memcpy(*dst, *src, n);
			(*src) += n;
			(*dst) += n;
		} else {
			for (i = 0; i < n; i++, (*src)++, (*dst)++) {
				(*dst)[0] = tbl[(*src)[0]];
			}
		}
	} else {
		if (light_table_index == lightmax) {
			(*src) += n;
			for (i = 0; i < n; i++, (*dst)++, mask <<= 1) {
				if (mask & 0x80000000) {
					(*dst)[0] = 0;
				}
			}
		} else if (light_table_index == 0) {
			for (i = 0; i < n; i++, (*src)++, (*dst)++, mask <<= 1) {
				if (mask & 0x80000000) {
					(*dst)[0] = (*src)[0];
				}
			}
		} else {
			for (i = 0; i < n; i++, (*src)++, (*dst)++, mask <<= 1) {
				if (mask & 0x80000000) {
					(*dst)[0] = tbl[(*src)[0]];
				}
			}
		}
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
/**
 * @brief Blit current world CEL to the given buffer
 * @param pBuff Output buffer
 */
void RenderTile(BYTE *pBuff)
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

	mask = &SolidMask[31];

	if (cel_transparency_active) {
		if (arch_draw_type == 0) {
			mask = &WallMask[31];
		}
		if (arch_draw_type == 1 && tile != RT_LTRIANGLE) {
			c = block_lvid[level_piece_id];
			if (c == 1 || c == 3) {
				mask = &LeftMask[31];
			}
		}
		if (arch_draw_type == 2 && tile != RT_RTRIANGLE) {
			c = block_lvid[level_piece_id];
			if (c == 2 || c == 3) {
				mask = &RightMask[31];
			}
		}
	} else if (arch_draw_type && cel_foliage_active) {
		if (tile != RT_TRANSPARENT) {
			return;
		}
		if (arch_draw_type == 1) {
			mask = &LeftFoliageMask[31];
		}
		if (arch_draw_type == 2) {
			mask = &RightFoliageMask[31];
		}
	}

#ifdef _DEBUG
	if (GetAsyncKeyState(VK_MENU) & 0x8000) {
		mask = &SolidMask[31];
	}
#endif

	switch (tile) {
	case RT_SQUARE:
		for (i = 32; i != 0; i--, dst -= BUFFER_WIDTH + 32, mask--) {
			RenderLine(&dst, &src, 32, tbl, *mask);
		}
		break;
	case RT_TRANSPARENT:
		for (i = 32; i != 0; i--, dst -= BUFFER_WIDTH + 32, mask--) {
			m = *mask;
			for (j = 32; j != 0; j -= v, v == 32 ? m = 0 : m <<= v) {
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
		for (i = 30; i >= 0; i -= 2, dst -= BUFFER_WIDTH + 32, mask--) {
			src += i & 2;
			dst += i;
			RenderLine(&dst, &src, 32 - i, tbl, *mask);
		}
		for (i = 2; i != 32; i += 2, dst -= BUFFER_WIDTH + 32, mask--) {
			src += i & 2;
			dst += i;
			RenderLine(&dst, &src, 32 - i, tbl, *mask);
		}
		break;
	case RT_RTRIANGLE:
		for (i = 30; i >= 0; i -= 2, dst -= BUFFER_WIDTH + 32, mask--) {
			RenderLine(&dst, &src, 32 - i, tbl, *mask);
			src += i & 2;
			dst += i;
		}
		for (i = 2; i != 32; i += 2, dst -= BUFFER_WIDTH + 32, mask--) {
			RenderLine(&dst, &src, 32 - i, tbl, *mask);
			src += i & 2;
			dst += i;
		}
		break;
	case RT_LTRAPEZOID:
		for (i = 30; i >= 0; i -= 2, dst -= BUFFER_WIDTH + 32, mask--) {
			src += i & 2;
			dst += i;
			RenderLine(&dst, &src, 32 - i, tbl, *mask);
		}
		for (i = 16; i != 0; i--, dst -= BUFFER_WIDTH + 32, mask--) {
			RenderLine(&dst, &src, 32, tbl, *mask);
		}
		break;
	case RT_RTRAPEZOID:
		for (i = 30; i >= 0; i -= 2, dst -= BUFFER_WIDTH + 32, mask--) {
			RenderLine(&dst, &src, 32 - i, tbl, *mask);
			src += i & 2;
			dst += i;
		}
		for (i = 16; i != 0; i--, dst -= BUFFER_WIDTH + 32, mask--) {
			RenderLine(&dst, &src, 32, tbl, *mask);
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
	int i, j, k;
	BYTE *dst;

	if (sx >= SCREEN_X + SCREEN_WIDTH || sy >= SCREEN_Y + VIEWPORT_HEIGHT + 32)
		return;

	if (sx < SCREEN_X - 60 || sy < SCREEN_Y)
		return;

	dst = &gpBuffer[sx + BUFFER_WIDTH * sy] + 30;

	for (i = 30, j = 1; i >= 0; i -= 2, j++, dst -= BUFFER_WIDTH + 2) {
		if (dst < gpBufEnd)
			memset(dst, 0, 4 * j);
	}
	dst += 4;
	for (i = 2, j = 15; i != 32; i += 2, j--, dst -= BUFFER_WIDTH - 2) {
		if (dst < gpBufEnd)
			memset(dst, 0, 4 * j);
	}
}

/**
 * Draws a half-transparent rectangle by blacking out odd pixels on odd lines,
 * even pixels on even lines.
 * @brief Render a transparent black rectangle
 * @param sx Screen coordinate
 * @param sy Screen coordinate
 * @param width Rectangle width
 * @param height Rectangle height
 */
void trans_rect(int sx, int sy, int width, int height)
{
	int row, col;
	BYTE *pix = &gpBuffer[SCREENXY(sx, sy)];
	for (row = 0; row < height; row++) {
		for (col = 0; col < width; col++) {
			if ((row & 1 && col & 1) || (!(row & 1) && !(col & 1)))
				*pix = 0;
			pix++;
		}
		pix += BUFFER_WIDTH - width;
	}
}

DEVILUTION_END_NAMESPACE
