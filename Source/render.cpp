#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

#define NO_OVERDRAW
#define USE_SPEEDCELS

typedef enum {
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

inline static void RenderLine(BYTE **dst, BYTE **src, int n, BYTE *tbl, DWORD mask)
{
	int i;

#ifdef NO_OVERDRAW
	if (zoomflag) {
		if ((*dst) < &gpBuffer[(0 + 160) * BUFFER_WIDTH]
		    || (*dst) > &gpBuffer[(VIEWPORT_HEIGHT + 160) * BUFFER_WIDTH]) {
			(*src) += n;
			(*dst) += n;
			return;
		}
	} else {
		if ((*dst) < &gpBuffer[(-17 + 160) * BUFFER_WIDTH]
		    || (*dst) > &gpBuffer[(160 + 160) * BUFFER_WIDTH]) {
			(*src) += n;
			(*dst) += n;
			return;
		}
	}
#endif

	if (mask == 0xFFFFFFFF) {
		if (light_table_index == lightmax) {
			(*src) += n;
			for (i = 0; i < n; i++, (*dst)++) {
				(*dst)[0] = 0;
			}
#ifdef USE_SPEEDCELS
		} else if (tbl == NULL) {
#else
		} else if (light_table_index == 0) {
#endif
			for (i = n & 3; i != 0; i--, (*src)++, (*dst)++) {
				(*dst)[0] = (*src)[0];
			}
			for (i = n >> 2; i != 0; i--, (*src) += 4, (*dst) += 4) {
				((DWORD *)(*dst))[0] = ((DWORD *)(*src))[0];
			}
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
#ifdef USE_SPEEDCELS
		} else if (tbl == NULL) {
#else
		} else if (light_table_index == 0) {
#endif
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

void RenderTile(BYTE *pBuff)
{
	int i, j;
	char c, v, tile;
	BYTE *src, *dst, *tbl;
	DWORD m, *mask, *pFrameTable;

	dst = pBuff;
	pFrameTable = (DWORD *)pDungeonCels;

	src = &pDungeonCels[pFrameTable[level_cel_block & 0xFFF]];
	tile = (level_cel_block & 0x7000) >> 12;
	tbl = &pLightTbl[256 * light_table_index];

#ifdef USE_SPEEDCELS
	if (light_table_index == lightmax || light_table_index == 0) {
		if (level_cel_block & 0x8000) {
			level_cel_block = SpeedFrameTbl[level_cel_block & 0xFFF][0] + (level_cel_block & 0xF000);
		}
		src = &pDungeonCels[pFrameTable[level_cel_block & 0xFFF]];
		tile = (level_cel_block & 0x7000) >> 12;
		tbl = NULL;
	} else if (level_cel_block & 0x8000) {
		src = &pSpeedCels[SpeedFrameTbl[level_cel_block & 0xFFF][light_table_index]];
		tile = (level_cel_block & 0x7000) >> 12;
		tbl = NULL;
	}
#endif

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
			for (j = 32; j != 0; j -= v, m <<= v) {
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

void world_draw_black_tile(BYTE *pBuff)
{
	int i, j, k;
	BYTE *dst;

	dst = pBuff;

	for (i = 30, j = 1; i >= 0; i -= 2, j++, dst -= BUFFER_WIDTH + 64) {
		dst += i;
		for (k = 0; k < 4 * j; k++) {
			*dst++ = 0;
		}
		dst += i;
	}
	for (i = 2, j = 15; i != 32; i += 2, j--, dst -= BUFFER_WIDTH + 64) {
		dst += i;
		for (k = 0; k < 4 * j; k++) {
			*dst++ = 0;
		}
		dst += i;
	}
}

DEVILUTION_END_NAMESPACE
