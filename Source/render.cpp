#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

#include "_asm.cpp"

int WorldBoolFlag = 0;
DWORD gdwCurrentMask = 0;
// char world_4B3264 = 0;
BYTE *gpCelFrame = NULL;
DWORD *gpDrawMask = NULL;
// char world_4B326D[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

DWORD RightMask[32] = {
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

DWORD LeftMask[32] = {
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

DWORD WallMask[32] = {
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

int WorldTbl3x16[48] = {
	0, 0, 0,
	4, 4, 4,
	8, 8, 8,
	12, 12, 12,
	16, 16, 16,
	20, 20, 20,
	24, 24, 24,
	28, 28, 28,
	32, 32, 32,
	36, 36, 36,
	40, 40, 40,
	44, 44, 44,
	48, 48, 48,
	52, 52, 52,
	56, 56, 56,
	60, 60, 60
};

// slope/angle tables, left and right
int WorldTbl17_1[17] = { 0, 4, 8, 16, 24, 36, 48, 64, 80, 100, 120, 144, 168, 196, 224, 256, 288 };
int WorldTbl17_2[17] = { 0, 32, 60, 88, 112, 136, 156, 176, 192, 208, 220, 232, 240, 248, 252, 256, 288 };

/*
 32x32 arch types
 add 8 if light index is 0

	|-| 0x8 (0)
	|-|

	/\  0x9 (1)
	\/

	 /| 0xA (2)
	 \|

	|\  0xB (3)
	|/

	|-| 0xC (4)
	 \|

	|-| 0xD (5)
	|/
*/

#ifdef USE_ASM
#include "_render.cpp"
#else
void drawTopArchesUpperScreen(BYTE *pBuff)
{
	BYTE *dst, *src;
	BYTE *tbl;
	short cel_type_16;
	unsigned int width;
	unsigned int chk_sh_and;
	unsigned int n_draw_shift;
	unsigned int x_minus;
	unsigned int y_minus;
	int xx_32, yy_32;
	int i, j;

	gpCelFrame = (BYTE *)SpeedFrameTbl;
	dst = pBuff;
	if (!(BYTE)light_table_index) {
		if (level_cel_block & 0x8000)
			level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
			    + (WORD)(level_cel_block & 0xF000);
		src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
		goto LABEL_11;
	}
	if ((BYTE)light_table_index != lightmax) {
		if (!(level_cel_block & 0x8000)) {
			src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			tbl = &pLightTbl[256 * light_table_index];
			cel_type_16 = (BYTE)(level_cel_block >> 12);
			switch (cel_type_16) {
			case 0: // upper (top transparent), with lighting
				i = 16;
				do {
					if (dst < gpBufEnd)
						break;
					asm_trans_light_square_1_3(8, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					if (dst < gpBufEnd)
						break;
					asm_trans_light_square_0_2(8, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				break;
			case 1: // upper (top transparent), with lighting
				WorldBoolFlag = (BYTE)pBuff & 1;
				xx_32 = 32;
				do {
					yy_32 = 32;
					do {
						while (1) {
							width = *src++;
							if ((width & 0x80) == 0)
								break;
							width = -(char)width;
							dst += width;
							yy_32 -= width;
							if (!yy_32)
								goto LABEL_67;
						}
						if (dst < gpBufEnd)
							return;
						if (((BYTE)dst & 1) == WorldBoolFlag) {
							asm_trans_light_cel_0_2(width, tbl, &dst, &src);
						} else {
							asm_trans_light_cel_1_3(width, tbl, &dst, &src);
						}
						yy_32 -= width;
					} while (yy_32);
				LABEL_67:
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					dst -= (SCREEN_WIDTH + 160);
					--xx_32;
				} while (xx_32);
				break;
			case 2: // upper (top transparent), with lighting
				WorldBoolFlag = 0;
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						asm_trans_light_cel_0_2(32 - xx_32, tbl, &dst, &src);
					} else {
						asm_trans_light_cel_1_3(32 - xx_32, tbl, &dst, &src);
					}
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
					if (xx_32 < 0) {
						yy_32 = 2;
						do {
							if (dst < gpBufEnd)
								break;
							dst += yy_32;
							src += (32 - (BYTE)yy_32) & 2;
							WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
							if (WorldBoolFlag) {
								asm_trans_light_cel_0_2(32 - yy_32, tbl, &dst, &src);
							} else {
								asm_trans_light_cel_1_3(32 - yy_32, tbl, &dst, &src);
							}
							dst -= (SCREEN_WIDTH + 160);
							yy_32 += 2;
						} while (yy_32 != 32);
						return;
					}
				}
				break;
			case 3: // upper (top transparent), with lighting
				WorldBoolFlag = 0;
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						asm_trans_light_cel_0_2(32 - xx_32, tbl, &dst, &src);
					} else {
						asm_trans_light_cel_1_3(32 - xx_32, tbl, &dst, &src);
					}
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
					if (xx_32 < 0) {
						yy_32 = 2;
						do {
							if (dst < gpBufEnd)
								break;
							WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
							if (WorldBoolFlag) {
								asm_trans_light_cel_0_2(32 - yy_32, tbl, &dst, &src);
							} else {
								asm_trans_light_cel_1_3(32 - yy_32, tbl, &dst, &src);
							}
							src += (BYTE)src & 2;
							dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
							yy_32 += 2;
						} while (yy_32 != 32);
						return;
					}
				}
				break;
			case 4: // upper (top transparent), with lighting
				WorldBoolFlag = 0;
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						asm_trans_light_cel_0_2(32 - xx_32, tbl, &dst, &src);
					} else {
						asm_trans_light_cel_1_3(32 - xx_32, tbl, &dst, &src);
					}
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
					if (xx_32 < 0) {
						i = 8;
						do {
							if (dst < gpBufEnd)
								break;
							asm_trans_light_square_1_3(8, tbl, &dst, &src);
							dst -= (SCREEN_WIDTH + 160);
							if (dst < gpBufEnd)
								break;
							asm_trans_light_square_0_2(8, tbl, &dst, &src);
							dst -= (SCREEN_WIDTH + 160);
							--i;
						} while (i);
						return;
					}
				}
				break;
			default: // upper (top transparent), with lighting
				WorldBoolFlag = 0;
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						asm_trans_light_cel_0_2(32 - xx_32, tbl, &dst, &src);
					} else {
						asm_trans_light_cel_1_3(32 - xx_32, tbl, &dst, &src);
					}
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
					if (xx_32 < 0) {
						i = 8;
						do {
							if (dst < gpBufEnd)
								break;
							asm_trans_light_square_1_3(8, tbl, &dst, &src);
							dst -= (SCREEN_WIDTH + 160);
							if (dst < gpBufEnd)
								break;
							asm_trans_light_square_0_2(8, tbl, &dst, &src);
							dst -= (SCREEN_WIDTH + 160);
							--i;
						} while (i);
						return;
					}
				}
				break;
			}
			return;
		}
		src = pSpeedCels
		    + *(DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (BYTE)(level_cel_block >> 12);
	LABEL_11:

		switch (cel_type_16) {
		case 8: // upper (top transparent), without lighting
			i = 16;
			do {
				if (dst < gpBufEnd)
					break;
				j = 8;
				do {
					dst[1] = src[1];
					dst[3] = src[3];
					src += 4;
					dst += 4;
					--j;
				} while (j);
				dst -= (SCREEN_WIDTH + 160);
				if (dst < gpBufEnd)
					break;
				j = 8;
				do {
					dst[0] = src[0];
					dst[2] = src[2];
					src += 4;
					dst += 4;
					--j;
				} while (j);
				dst -= (SCREEN_WIDTH + 160);
				--i;
			} while (i);
			break;
		case 9: // upper (top transparent), without lighting
			WorldBoolFlag = (BYTE)pBuff & 1;
			yy_32 = 32;
		LABEL_251:
			xx_32 = 32;
			while (1) {
				while (1) {
					width = *src++;
					if ((width & 0x80) == 0)
						break;
					width = -(char)width;
					dst += width;
					xx_32 -= width;
					if (!xx_32) {
					LABEL_271:
						WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
						dst -= (SCREEN_WIDTH + 160);
						if (!--yy_32)
							return;
						goto LABEL_251;
					}
				}
				xx_32 -= width;
				if (dst < gpBufEnd)
					return;
				if (((BYTE)dst & 1) == WorldBoolFlag) {
					chk_sh_and = width >> 1;
					if (!(width & 1))
						goto LABEL_258;
					++src;
					++dst;
					if (chk_sh_and) {
					LABEL_265:
						n_draw_shift = chk_sh_and >> 1;
						if (chk_sh_and & 1) {
							dst[0] = src[0];
							src += 2;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[0] = src[0];
								dst[2] = src[2];
								src += 4;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
						goto LABEL_268;
					}
				} else {
					chk_sh_and = width >> 1;
					if (!(width & 1))
						goto LABEL_265;
					*dst++ = *src++;
					if (chk_sh_and) {
					LABEL_258:
						n_draw_shift = chk_sh_and >> 1;
						if (chk_sh_and & 1) {
							dst[1] = src[1];
							src += 2;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[1] = src[1];
								dst[3] = src[3];
								src += 4;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
						goto LABEL_268;
					}
				}
			LABEL_268:
				if (!xx_32)
					goto LABEL_271;
			}
			break;
		case 10: // upper (top transparent), without lighting
			WorldBoolFlag = 0;
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				dst += xx_32;
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				if (WorldBoolFlag) {
					n_draw_shift = x_minus >> 2;
					if (x_minus & 2) {
						dst[1] = src[3];
						src += 4;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							dst[1] = src[1];
							dst[3] = src[3];
							src += 4;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
				} else {
					n_draw_shift = x_minus >> 2;
					if (x_minus & 2) {
						dst[0] = src[2];
						src += 4;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							dst[0] = src[0];
							dst[2] = src[2];
							src += 4;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
				}
				dst -= (SCREEN_WIDTH + 160);
				xx_32 -= 2;
				if (xx_32 < 0) {
					yy_32 = 2;
					do {
						if (dst < gpBufEnd)
							break;
						dst += yy_32;
						y_minus = 32 - yy_32;
						WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
						if (WorldBoolFlag) {
							n_draw_shift = y_minus >> 2;
							if (y_minus & 2) {
								dst[1] = src[3];
								src += 4;
								dst += 2;
							}
							if (n_draw_shift) {
								do {
									dst[1] = src[1];
									dst[3] = src[3];
									src += 4;
									dst += 4;
									--n_draw_shift;
								} while (n_draw_shift);
							}
						} else {
							n_draw_shift = y_minus >> 2;
							if (y_minus & 2) {
								dst[0] = src[2];
								src += 4;
								dst += 2;
							}
							if (n_draw_shift) {
								do {
									dst[0] = src[0];
									dst[2] = src[2];
									src += 4;
									dst += 4;
									--n_draw_shift;
								} while (n_draw_shift);
							}
						}
						dst -= (SCREEN_WIDTH + 160);
						yy_32 += 2;
					} while (yy_32 != 32);
					return;
				}
			}
			break;
		case 11: // upper (top transparent), without lighting
			WorldBoolFlag = 0;
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				if (WorldBoolFlag) {
					for (n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift) {
						dst[1] = src[1];
						dst[3] = src[3];
						src += 4;
						dst += 4;
					}
					if (x_minus & 2) {
						dst[1] = src[1];
						src += 4;
						dst += 2;
					}
				} else {
					for (n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift) {
						dst[0] = src[0];
						dst[2] = src[2];
						src += 4;
						dst += 4;
					}
					if (x_minus & 2) {
						dst[0] = src[0];
						src += 4;
						dst += 2;
					}
				}
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
				if (xx_32 < 0) {
					yy_32 = 2;
					do {
						if (dst < gpBufEnd)
							break;
						y_minus = 32 - yy_32;
						WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
						if (WorldBoolFlag) {
							for (n_draw_shift = y_minus >> 2; n_draw_shift; --n_draw_shift) {
								dst[1] = src[1];
								dst[3] = src[3];
								src += 4;
								dst += 4;
							}
							if (x_minus & 2) /// BUGFIX: change to `y_minus & 2`
							{
								dst[1] = src[1];
								src += 4;
								dst += 2;
							}
						} else {
							for (n_draw_shift = y_minus >> 2; n_draw_shift; --n_draw_shift) {
								dst[0] = src[0];
								dst[2] = src[2];
								src += 4;
								dst += 4;
							}
							if (x_minus & 2) /// BUGFIX: change to `y_minus & 2`
							{
								dst[0] = src[0];
								src += 4;
								dst += 2;
							}
						}
						dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
						yy_32 += 2;
					} while (yy_32 != 32);
					return;
				}
			}
			break;
		case 12: // upper (top transparent), without lighting
			WorldBoolFlag = 0;
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				dst += xx_32;
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				if (WorldBoolFlag) {
					n_draw_shift = x_minus >> 2;
					if (x_minus & 2) {
						dst[1] = src[3];
						src += 4;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							dst[1] = src[1];
							dst[3] = src[3];
							src += 4;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
				} else {
					n_draw_shift = x_minus >> 2;
					if (x_minus & 2) {
						dst[0] = src[2];
						src += 4;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							dst[0] = src[0];
							dst[2] = src[2];
							src += 4;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
				}
				dst -= (SCREEN_WIDTH + 160);
				xx_32 -= 2;
				if (xx_32 < 0) {
					i = 8;
					do {
						if (dst < gpBufEnd)
							break;
						j = 8;
						do {
							dst[1] = src[1];
							dst[3] = src[3];
							src += 4;
							dst += 4;
							--j;
						} while (j);
						dst -= (SCREEN_WIDTH + 160);
						if (dst < gpBufEnd)
							break;
						j = 8;
						do {
							dst[0] = src[0];
							dst[2] = src[2];
							src += 4;
							dst += 4;
							--j;
						} while (j);
						dst -= (SCREEN_WIDTH + 160);
						--i;
					} while (i);
					return;
				}
			}
			break;
		default: // upper (top transparent), without lighting
			WorldBoolFlag = 0;
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				if (WorldBoolFlag) {
					for (n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift) {
						dst[1] = src[1];
						dst[3] = src[3];
						src += 4;
						dst += 4;
					}
					if ((32 - (BYTE)xx_32) & 2) {
						dst[1] = src[1];
						src += 4;
						dst += 2;
					}
				} else {
					for (n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift) {
						dst[0] = src[0];
						dst[2] = src[2];
						src += 4;
						dst += 4;
					}
					if ((32 - (BYTE)xx_32) & 2) {
						dst[0] = src[0];
						src += 4;
						dst += 2;
					}
				}
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
				if (xx_32 < 0) {
					i = 8;
					do {
						if (dst < gpBufEnd)
							break;
						j = 8;
						do {
							dst[1] = src[1];
							dst[3] = src[3];
							src += 4;
							dst += 4;
							--j;
						} while (j);
						dst -= (SCREEN_WIDTH + 160);
						if (dst < gpBufEnd)
							break;
						j = 8;
						do {
							dst[0] = src[0];
							dst[2] = src[2];
							src += 4;
							dst += 4;
							--j;
						} while (j);
						dst -= (SCREEN_WIDTH + 160);
						--i;
					} while (i);
					return;
				}
			}
			break;
		}
		return;
	}
	if (level_cel_block & 0x8000)
		level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
		    + (WORD)(level_cel_block & 0xF000);
	src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
	cel_type_16 = (level_cel_block >> 12) & 7;
	switch (cel_type_16) {
	case 0: // upper (top transparent), black
		i = 16;
		do {
			if (dst < gpBufEnd)
				break;
			j = 8;
			do {
				dst[1] = 0;
				dst[3] = 0;
				dst += 4;
				--j;
			} while (j);
			dst -= (SCREEN_WIDTH + 160);
			if (dst < gpBufEnd)
				break;
			j = 8;
			do {
				dst[0] = 0;
				dst[2] = 0;
				dst += 4;
				--j;
			} while (j);
			dst -= (SCREEN_WIDTH + 160);
			--i;
		} while (i);
		break;
	case 1: // upper (top transparent), black
		WorldBoolFlag = (BYTE)pBuff & 1;
		xx_32 = 32;
		while (1) {
			yy_32 = 32;
			do {
				while (1) {
					width = *src++;
					if ((width & 0x80) != 0)
						break;
					yy_32 -= width;
					if (dst < gpBufEnd)
						return;
					src += width;
					if (((BYTE)dst & 1) == WorldBoolFlag) {
						chk_sh_and = width >> 1;
						if (!(width & 1))
							goto LABEL_378;
						++dst;
						if (chk_sh_and) {
						LABEL_385:
							n_draw_shift = chk_sh_and >> 1;
							if (chk_sh_and & 1) {
								dst[0] = 0;
								dst += 2;
							}
							if (n_draw_shift) {
								do {
									dst[0] = 0;
									dst[2] = 0;
									dst += 4;
									--n_draw_shift;
								} while (n_draw_shift);
							}
							goto LABEL_388;
						}
					} else {
						chk_sh_and = width >> 1;
						if (!(width & 1))
							goto LABEL_385;
						*dst++ = 0;
						if (chk_sh_and) {
						LABEL_378:
							n_draw_shift = chk_sh_and >> 1;
							if (chk_sh_and & 1) {
								dst[1] = 0;
								dst += 2;
							}
							if (n_draw_shift) {
								do {
									dst[1] = 0;
									dst[3] = 0;
									dst += 4;
									--n_draw_shift;
								} while (n_draw_shift);
							}
							goto LABEL_388;
						}
					}
				LABEL_388:
					if (!yy_32)
						goto LABEL_391;
				}
				width = -(char)width;
				dst += width;
				yy_32 -= width;
			} while (yy_32);
		LABEL_391:
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			dst -= (SCREEN_WIDTH + 160);
			if (!--xx_32)
				return;
		}
	case 2: // upper (top transparent), black
		WorldBoolFlag = 0;
		for (xx_32 = 30;; xx_32 -= 2) {
			if (dst < gpBufEnd)
				return;
			dst += xx_32;
			x_minus = 32 - xx_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[1] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[1] = 0;
						dst[3] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			} else {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[0] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[0] = 0;
						dst[2] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32)
				break;
		}
		yy_32 = 2;
		do {
			if (dst < gpBufEnd)
				break;
			dst += yy_32;
			y_minus = 32 - yy_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				n_draw_shift = y_minus >> 2;
				if (y_minus & 2) {
					dst[1] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[1] = 0;
						dst[3] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			} else {
				n_draw_shift = y_minus >> 2;
				if (y_minus & 2) {
					dst[0] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[0] = 0;
						dst[2] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			}
			dst -= (SCREEN_WIDTH + 160);
			yy_32 += 2;
		} while (yy_32 != 32);
		break;
	case 3: // upper (top transparent), black
		WorldBoolFlag = 0;
		for (xx_32 = 30;; xx_32 -= 2) {
			if (dst < gpBufEnd)
				return;
			x_minus = 32 - xx_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[1] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[1] = 0;
						dst[3] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			} else {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[0] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[0] = 0;
						dst[2] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32)
				break;
			dst += xx_32;
		}
		yy_32 = 2;
		do {
			if (dst < gpBufEnd)
				break;
			y_minus = 32 - yy_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				n_draw_shift = y_minus >> 2;
				if (y_minus & 2) {
					dst[1] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[1] = 0;
						dst[3] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			} else {
				n_draw_shift = y_minus >> 2;
				if (y_minus & 2) {
					dst[0] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[0] = 0;
						dst[2] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			}
			dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
			yy_32 += 2;
		} while (yy_32 != 32);
		break;
	case 4: // upper (top transparent), black
		WorldBoolFlag = 0;
		for (xx_32 = 30;; xx_32 -= 2) {
			if (dst < gpBufEnd)
				return;
			dst += xx_32;
			x_minus = 32 - xx_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[1] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[1] = 0;
						dst[3] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			} else {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[0] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[0] = 0;
						dst[2] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32)
				break;
		}
		i = 8;
		do {
			if (dst < gpBufEnd)
				break;
			j = 8;
			do {
				dst[1] = 0;
				dst[3] = 0;
				dst += 4;
				--j;
			} while (j);
			dst -= (SCREEN_WIDTH + 160);
			if (dst < gpBufEnd)
				break;
			j = 8;
			do {
				dst[0] = 0;
				dst[2] = 0;
				dst += 4;
				--j;
			} while (j);
			dst -= (SCREEN_WIDTH + 160);
			--i;
		} while (i);
		break;
	default: // upper (top transparent), black
		WorldBoolFlag = 0;
		for (xx_32 = 30;; xx_32 -= 2) {
			if (dst < gpBufEnd)
				return;
			x_minus = 32 - xx_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[1] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[1] = 0;
						dst[3] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			} else {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[0] = 0;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[0] = 0;
						dst[2] = 0;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32)
				break;
			dst += xx_32;
		}
		i = 8;
		do {
			if (dst < gpBufEnd)
				break;
			j = 8;
			do {
				dst[1] = 0;
				dst[3] = 0;
				dst += 4;
				--j;
			} while (j);
			dst -= (SCREEN_WIDTH + 160);
			if (dst < gpBufEnd)
				break;
			j = 8;
			do {
				dst[0] = 0;
				dst[2] = 0;
				dst += 4;
				--j;
			} while (j);
			dst -= (SCREEN_WIDTH + 160);
			--i;
		} while (i);
		break;
	}
}

void drawBottomArchesUpperScreen(BYTE *pBuff, DWORD *pMask)
{
	BYTE *dst, *src;
	BYTE *tbl;
	short cel_type_16;
	unsigned int left_shift;
	unsigned int n_draw_shift;
	int width;
	int and80_i;
	int i;
	int xx_32, yy_32;

	gpCelFrame = (BYTE *)SpeedFrameTbl;
	dst = pBuff;
	gpDrawMask = pMask;
	if (!(BYTE)light_table_index) {
		if (level_cel_block & 0x8000)
			level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
			    + (WORD)(level_cel_block & 0xF000);
		src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
	LABEL_12:
		switch (cel_type_16) {
		case 8: // upper (bottom transparent), without lighting
			xx_32 = 32;
			do {
				if (dst < gpBufEnd)
					break;
				left_shift = *gpDrawMask;
				i = 32;
				do {
					if (left_shift & 0x80000000)
						dst[0] = src[0];
					left_shift *= 2;
					++src;
					++dst;
					--i;
				} while (i);
				dst -= (SCREEN_WIDTH + 160);
				--gpDrawMask;
				--xx_32;
			} while (xx_32);
			break;
		case 9: // upper (bottom transparent), without lighting
			xx_32 = 32;
			do {
				gdwCurrentMask = *gpDrawMask;
				yy_32 = 32;
				do {
					while (1) {
						width = *src++;
						if ((width & 0x80) == 0)
							break;
						width = -(char)width;
						dst += width;
						if (width & 0x1F)
							gdwCurrentMask <<= width & 0x1F;
						yy_32 -= width;
						if (!yy_32)
							goto LABEL_129;
					}
					yy_32 -= width;
					if (dst < gpBufEnd)
						return;
					left_shift = gdwCurrentMask;
					and80_i = width;
					do {
						if (left_shift & 0x80000000)
							dst[0] = src[0];
						left_shift *= 2;
						++src;
						++dst;
						--and80_i;
					} while (and80_i);
					gdwCurrentMask = left_shift;
				} while (yy_32);
			LABEL_129:
				dst -= (SCREEN_WIDTH + 160);
				--gpDrawMask;
				--xx_32;
			} while (xx_32);
			break;
		case 10: // upper (bottom transparent), without lighting
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				dst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ((32 - xx_32) & 2) {
					*(WORD *)dst = *((WORD *)src + 1);
					src += 4;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						*(DWORD *)dst = *(DWORD *)src;
						src += 4;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
				dst -= (SCREEN_WIDTH + 160);
				xx_32 -= 2;
				if (xx_32 < 0) {
					yy_32 = 2;
					do {
						if (dst < gpBufEnd)
							break;
						dst += yy_32;
						n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
						if ((32 - yy_32) & 2) {
							*(WORD *)dst = *((WORD *)src + 1);
							src += 4;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = *(DWORD *)src;
								src += 4;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
						dst -= (SCREEN_WIDTH + 160);
						yy_32 += 2;
					} while (yy_32 != 32);
					return;
				}
			}
			break;
		case 11: // upper (bottom transparent), without lighting
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				for (n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift) {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)xx_32) & 2) {
					*(WORD *)dst = *(WORD *)src;
					src += 4;
					dst += 2;
				}
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
				if (xx_32 < 0) {
					yy_32 = 2;
					do {
						if (dst < gpBufEnd)
							break;
						for (n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift) {
							*(DWORD *)dst = *(DWORD *)src;
							src += 4;
							dst += 4;
						}
						if ((32 - (BYTE)yy_32) & 2) {
							*(WORD *)dst = *(WORD *)src;
							src += 4;
							dst += 2;
						}
						dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
						yy_32 += 2;
					} while (yy_32 != 32);
					return;
				}
			}
			break;
		case 12: // upper (bottom transparent), without lighting
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				dst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ((32 - xx_32) & 2) {
					*(WORD *)dst = *((WORD *)src + 1);
					src += 4;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						*(DWORD *)dst = *(DWORD *)src;
						src += 4;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
				dst -= (SCREEN_WIDTH + 160);
				xx_32 -= 2;
				if (xx_32 < 0) {
					gpDrawMask -= 16;
					yy_32 = 16;
					do {
						if (dst < gpBufEnd)
							break;
						left_shift = *gpDrawMask;
						i = 32;
						do {
							if (left_shift & 0x80000000)
								dst[0] = src[0];
							left_shift *= 2;
							++src;
							++dst;
							--i;
						} while (i);
						dst -= (SCREEN_WIDTH + 160);
						--gpDrawMask;
						--yy_32;
					} while (yy_32);
					return;
				}
			}
			break;
		default: // upper (bottom transparent), without lighting
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				for (n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift) {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)xx_32) & 2) {
					*(WORD *)dst = *(WORD *)src;
					src += 4;
					dst += 2;
				}
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
				if (xx_32 < 0) {
					gpDrawMask -= 16;
					yy_32 = 16;
					do {
						if (dst < gpBufEnd)
							break;
						left_shift = *gpDrawMask;
						i = 32;
						do {
							if (left_shift & 0x80000000)
								dst[0] = src[0];
							left_shift *= 2;
							++src;
							++dst;
							--i;
						} while (i);
						src += (BYTE)src & 2;
						dst -= (SCREEN_WIDTH + 160);
						--gpDrawMask;
						--yy_32;
					} while (yy_32);
					return;
				}
			}
			break;
		}
		return;
	}
	if ((BYTE)light_table_index != lightmax) {
		if (!(level_cel_block & 0x8000)) {
			src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			tbl = &pLightTbl[256 * light_table_index];
			cel_type_16 = (BYTE)(level_cel_block >> 12);
			switch (cel_type_16) {
			case 0: // upper (bottom transparent), with lighting
				xx_32 = 32;
				do {
					if (dst < gpBufEnd)
						break;
					asm_trans_light_mask(32, tbl, &dst, &src, *gpDrawMask);
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--xx_32;
				} while (xx_32);
				break;
			case 1: // upper (bottom transparent), with lighting
				xx_32 = 32;
				do {
					gdwCurrentMask = *gpDrawMask;
					yy_32 = 32;
					do {
						while (1) {
							width = *src++;
							if ((width & 0x80) == 0)
								break;
							width = -(char)width;
							dst += width;
							if (width & 0x1F)
								gdwCurrentMask <<= width & 0x1F;
							yy_32 -= width;
							if (!yy_32)
								goto LABEL_50;
						}
						yy_32 -= width;
						if (dst < gpBufEnd)
							return;
						gdwCurrentMask = asm_trans_light_mask(width, tbl, &dst, &src, gdwCurrentMask);
					} while (yy_32);
				LABEL_50:
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--xx_32;
				} while (xx_32);
				break;
			case 2: // upper (bottom transparent), with lighting
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
					if (xx_32 < 0) {
						yy_32 = 2;
						do {
							if (dst < gpBufEnd)
								break;
							dst += yy_32;
							src += (32 - (BYTE)yy_32) & 2;
							asm_cel_light_edge(32 - yy_32, tbl, &dst, &src);
							dst -= (SCREEN_WIDTH + 160);
							yy_32 += 2;
						} while (yy_32 != 32);
						return;
					}
				}
				break;
			case 3: // upper (bottom transparent), with lighting
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
					if (xx_32 < 0) {
						yy_32 = 2;
						do {
							if (dst < gpBufEnd)
								break;
							asm_cel_light_edge(32 - yy_32, tbl, &dst, &src);
							src += (BYTE)src & 2;
							dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
							yy_32 += 2;
						} while (yy_32 != 32);
						return;
					}
				}
				break;
			case 4: // upper (bottom transparent), with lighting
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
					if (xx_32 < 0) {
						gpDrawMask -= 16;
						yy_32 = 16;
						do {
							if (dst < gpBufEnd)
								break;
							src += (BYTE)src & 2;
							asm_trans_light_mask(32, tbl, &dst, &src, *gpDrawMask);
							dst -= (SCREEN_WIDTH + 160);
							--gpDrawMask;
							--yy_32;
						} while (yy_32);
						return;
					}
				}
				break;
			default: // upper (bottom transparent), with lighting
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
					if (xx_32 < 0) {
						gpDrawMask -= 16;
						yy_32 = 16;
						do {
							if (dst < gpBufEnd)
								break;
							asm_trans_light_mask(32, tbl, &dst, &src, *gpDrawMask);
							src += (BYTE)src & 2;
							dst -= (SCREEN_WIDTH + 160);
							--gpDrawMask;
							--yy_32;
						} while (yy_32);
						return;
					}
				}
				break;
			}
			return;
		}
		src = pSpeedCels
		    + *(DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (BYTE)(level_cel_block >> 12);
		goto LABEL_12;
	}
	if (level_cel_block & 0x8000)
		level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
		    + (WORD)(level_cel_block & 0xF000);
	src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
	cel_type_16 = (level_cel_block >> 12) & 7;
	switch (cel_type_16) {
	case 0: // upper (bottom transparent), black
		xx_32 = 32;
		do {
			if (dst < gpBufEnd)
				break;
			left_shift = *gpDrawMask;
			i = 32;
			do {
				if (left_shift & 0x80000000)
					dst[0] = 0;
				left_shift *= 2;
				++dst;
				--i;
			} while (i);
			dst -= (SCREEN_WIDTH + 160);
			--gpDrawMask;
			--xx_32;
		} while (xx_32);
		break;
	case 1: // upper (bottom transparent), black
		xx_32 = 32;
		do {
			gdwCurrentMask = *gpDrawMask;
			yy_32 = 32;
			do {
				while (1) {
					width = *src++;
					if ((width & 0x80) == 0)
						break;
					width = -(char)width;
					dst += width;
					if (width & 0x1F)
						gdwCurrentMask <<= width & 0x1F;
					yy_32 -= width;
					if (!yy_32)
						goto LABEL_208;
				}
				yy_32 -= width;
				if (dst < gpBufEnd)
					return;
				left_shift = gdwCurrentMask;
				and80_i = width;
				src += width;
				do {
					if (left_shift & 0x80000000)
						dst[0] = 0;
					left_shift *= 2;
					++dst;
					--and80_i;
				} while (and80_i);
				gdwCurrentMask = left_shift;
			} while (yy_32);
		LABEL_208:
			dst -= (SCREEN_WIDTH + 160);
			--gpDrawMask;
			--xx_32;
		} while (xx_32);
		break;
	case 2: // upper (bottom transparent), black
		xx_32 = 30;
		while (dst >= gpBufEnd) {
			dst += xx_32;
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = 0;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = 0;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32) {
				yy_32 = 2;
				do {
					if (dst < gpBufEnd)
						break;
					dst += yy_32;
					n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
					if ((32 - yy_32) & 2) {
						*(WORD *)dst = 0;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							*(DWORD *)dst = 0;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
					dst -= (SCREEN_WIDTH + 160);
					yy_32 += 2;
				} while (yy_32 != 32);
				return;
			}
			xx_32 -= 2;
		}
		break;
	case 3: // upper (bottom transparent), black
		xx_32 = 30;
		while (dst >= gpBufEnd) {
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = 0;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = 0;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32) {
				yy_32 = 2;
				do {
					if (dst < gpBufEnd)
						break;
					n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
					if ((32 - yy_32) & 2) {
						*(WORD *)dst = 0;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							*(DWORD *)dst = 0;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
					dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
					yy_32 += 2;
				} while (yy_32 != 32);
				return;
			}
			dst += xx_32;
			xx_32 -= 2;
		}
		break;
	case 4: // upper (bottom transparent), black
		xx_32 = 30;
		while (dst >= gpBufEnd) {
			dst += xx_32;
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = 0;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = 0;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32) {
				gpDrawMask -= 16;
				yy_32 = 16;
				do {
					if (dst < gpBufEnd)
						break;
					left_shift = *gpDrawMask;
					i = 32;
					do {
						if (left_shift & 0x80000000)
							dst[0] = 0;
						left_shift *= 2;
						++dst;
						--i;
					} while (i);
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--yy_32;
				} while (yy_32);
				return;
			}
			xx_32 -= 2;
		}
		break;
	default: // upper (bottom transparent), black
		xx_32 = 30;
		while (dst >= gpBufEnd) {
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = 0;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = 0;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32) {
				gpDrawMask -= 16;
				yy_32 = 16;
				do {
					if (dst < gpBufEnd)
						break;
					left_shift = *gpDrawMask;
					i = 32;
					do {
						if (left_shift & 0x80000000)
							dst[0] = 0;
						left_shift *= 2;
						++dst;
						--i;
					} while (i);
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--yy_32;
				} while (yy_32);
				return;
			}
			dst += xx_32;
			xx_32 -= 2;
		}
		break;
	}
}

void drawUpperScreen(BYTE *pBuff)
{
	BYTE *dst, *src;
	BYTE *tbl;
	short cel_type_16;
	unsigned int width;
	unsigned int chk_sh_and;
	unsigned int n_draw_shift;
	int i, j;
	int xx_32, yy_32;

	if (cel_transparency_active) {
		if (!arch_draw_type) {
			drawTopArchesUpperScreen(pBuff);
			return;
		}
		if (arch_draw_type == 1) {
			if (block_lvid[level_piece_id] == 1 || block_lvid[level_piece_id] == 3) {
				drawBottomArchesUpperScreen(pBuff, &LeftMask[31]);
				return;
			}
		}
		if (arch_draw_type == 2) {
			if (block_lvid[level_piece_id] == 2 || block_lvid[level_piece_id] == 3) {
				drawBottomArchesUpperScreen(pBuff, &RightMask[31]);
				return;
			}
		}
	}
	gpCelFrame = (BYTE *)SpeedFrameTbl;
	dst = pBuff;
	if (!(BYTE)light_table_index) {
		if (level_cel_block & 0x8000)
			level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
			    + (WORD)(level_cel_block & 0xF000);
		src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
	LABEL_22:
		switch (cel_type_16) {
		case 8: // upper (solid), without lighting
			i = 32;
			do {
				if (dst < gpBufEnd)
					break;
				j = 8;
				do {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
					--j;
				} while (j);
				dst -= (SCREEN_WIDTH + 160);
				--i;
			} while (i);
			break;
		case 9: // upper (solid), without lighting
			xx_32 = 32;
			do {
				yy_32 = 32;
				do {
					while (1) {
						width = *src++;
						if ((width & 0x80) == 0)
							break;
						width = -(char)width;
						dst += width;
						yy_32 -= width;
						if (!yy_32)
							goto LABEL_133;
					}
					yy_32 -= width;
					if (dst < gpBufEnd)
						return;
					chk_sh_and = width >> 1;
					if (width & 1) {
						*dst++ = *src++;
						if (!chk_sh_and)
							continue;
					}
					n_draw_shift = chk_sh_and >> 1;
					if (chk_sh_and & 1) {
						*(WORD *)dst = *(WORD *)src;
						src += 2;
						dst += 2;
						if (!n_draw_shift)
							continue;
					}
					do {
						*(DWORD *)dst = *(DWORD *)src;
						src += 4;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				} while (yy_32);
			LABEL_133:
				dst -= (SCREEN_WIDTH + 160);
				--xx_32;
			} while (xx_32);
			break;
		case 10: // upper (solid), without lighting
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				dst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ((32 - xx_32) & 2) {
					*(WORD *)dst = *((WORD *)src + 1);
					src += 4;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						*(DWORD *)dst = *(DWORD *)src;
						src += 4;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
				dst -= (SCREEN_WIDTH + 160);
				xx_32 -= 2;
				if (xx_32 < 0) {
					yy_32 = 2;
					do {
						if (dst < gpBufEnd)
							break;
						dst += yy_32;
						n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
						if ((32 - yy_32) & 2) {
							*(WORD *)dst = *((WORD *)src + 1);
							src += 4;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = *(DWORD *)src;
								src += 4;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
						dst -= (SCREEN_WIDTH + 160);
						yy_32 += 2;
					} while (yy_32 != 32);
					return;
				}
			}
			break;
		case 11: // upper (solid), without lighting
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				for (n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift) {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)xx_32) & 2) {
					*(WORD *)dst = *(WORD *)src;
					src += 4;
					dst += 2;
				}
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
				if (xx_32 < 0) {
					yy_32 = 2;
					do {
						if (dst < gpBufEnd)
							break;
						for (n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift) {
							*(DWORD *)dst = *(DWORD *)src;
							src += 4;
							dst += 4;
						}
						if ((32 - (BYTE)yy_32) & 2) {
							*(WORD *)dst = *(WORD *)src;
							src += 4;
							dst += 2;
						}
						dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
						yy_32 += 2;
					} while (yy_32 != 32);
					return;
				}
			}
			break;
		case 12: // upper (solid), without lighting
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				dst += xx_32;
				n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
				if ((32 - xx_32) & 2) {
					*(WORD *)dst = *((WORD *)src + 1);
					src += 4;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						*(DWORD *)dst = *(DWORD *)src;
						src += 4;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
				dst -= (SCREEN_WIDTH + 160);
				xx_32 -= 2;
				if (xx_32 < 0) {
					i = 16;
					do {
						if (dst < gpBufEnd)
							break;
						j = 8;
						do {
							*(DWORD *)dst = *(DWORD *)src;
							src += 4;
							dst += 4;
							--j;
						} while (j);
						dst -= (SCREEN_WIDTH + 160);
						--i;
					} while (i);
					return;
				}
			}
			break;
		default: // upper (solid), without lighting
			xx_32 = 30;
			while (dst >= gpBufEnd) {
				for (n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift) {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)xx_32) & 2) {
					*(WORD *)dst = *(WORD *)src;
					src += 4;
					dst += 2;
				}
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
				if (xx_32 < 0) {
					i = 16;
					do {
						if (dst < gpBufEnd)
							break;
						j = 8;
						do {
							*(DWORD *)dst = *(DWORD *)src;
							src += 4;
							dst += 4;
							--j;
						} while (j);
						dst -= (SCREEN_WIDTH + 160);
						--i;
					} while (i);
					return;
				}
			}
			break;
		}
		return;
	}
	if ((BYTE)light_table_index != lightmax) {
		if (!(level_cel_block & 0x8000)) {
			src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			tbl = &pLightTbl[256 * light_table_index];
			cel_type_16 = (WORD)level_cel_block >> 12;
			switch (cel_type_16) {
			case 0: // upper (solid), with lighting
				xx_32 = 32;
				do {
					if (dst < gpBufEnd)
						break;
					asm_cel_light_square(8, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					--xx_32;
				} while (xx_32);
				break;
			case 1: // upper (solid), with lighting
				xx_32 = 32;
				do {
					yy_32 = 32;
					do {
						while (1) {
							width = *src++;
							if ((width & 0x80) == 0)
								break;
							width = -(char)width;
							dst += width;
							yy_32 -= width;
							if (!yy_32)
								goto LABEL_58;
						}
						yy_32 -= width;
						if (dst < gpBufEnd)
							return;
						asm_cel_light_edge(width, tbl, &dst, &src);
					} while (yy_32);
				LABEL_58:
					dst -= (SCREEN_WIDTH + 160);
					--xx_32;
				} while (xx_32);
				break;
			case 2: // upper (solid), with lighting
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
					if (xx_32 < 0) {
						yy_32 = 2;
						do {
							if (dst < gpBufEnd)
								break;
							dst += yy_32;
							src += (32 - (BYTE)yy_32) & 2;
							asm_cel_light_edge(32 - yy_32, tbl, &dst, &src);
							dst -= (SCREEN_WIDTH + 160);
							yy_32 += 2;
						} while (yy_32 != 32);
						return;
					}
				}
				break;
			case 3: // upper (solid), with lighting
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
					if (xx_32 < 0) {
						yy_32 = 2;
						do {
							if (dst < gpBufEnd)
								break;
							asm_cel_light_edge(32 - yy_32, tbl, &dst, &src);
							src += (BYTE)src & 2;
							dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
							yy_32 += 2;
						} while (yy_32 != 32);
						return;
					}
				}
				break;
			case 4: // upper (solid), with lighting
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
					if (xx_32 < 0) {
						yy_32 = 16;
						do {
							if (dst < gpBufEnd)
								break;
							asm_cel_light_square(8, tbl, &dst, &src);
							dst -= (SCREEN_WIDTH + 160);
							--yy_32;
						} while (yy_32);
						return;
					}
				}
				break;
			default: // upper (solid), with lighting
				xx_32 = 30;
				while (dst >= gpBufEnd) {
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
					if (xx_32 < 0) {
						yy_32 = 16;
						do {
							if (dst < gpBufEnd)
								break;
							asm_cel_light_square(8, tbl, &dst, &src);
							dst -= (SCREEN_WIDTH + 160);
							--yy_32;
						} while (yy_32);
						return;
					}
				}
				break;
			}
			return;
		}
		src = pSpeedCels
		    + *(DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (WORD)level_cel_block >> 12;
		goto LABEL_22;
	}
	if (level_cel_block & 0x8000)
		level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
		    + (WORD)(level_cel_block & 0xF000);
	src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
	cel_type_16 = ((unsigned int)level_cel_block >> 12) & 7;
	switch (cel_type_16) {
	case 0: // upper (solid), black
		i = 32;
		do {
			if (dst < gpBufEnd)
				break;
			j = 8;
			do {
				*(DWORD *)dst = 0;
				dst += 4;
				--j;
			} while (j);
			dst -= (SCREEN_WIDTH + 160);
			--i;
		} while (i);
		break;
	case 1: // upper (solid), black
		xx_32 = 32;
		do {
			yy_32 = 32;
			do {
				while (1) {
					width = *src++;
					if ((width & 0x80) == 0)
						break;
					width = -(char)width;
					dst += width;
					yy_32 -= width;
					if (!yy_32)
						goto LABEL_205;
				}
				yy_32 -= width;
				if (dst < gpBufEnd)
					return;
				src += width;
				chk_sh_and = width >> 1;
				if (width & 1) {
					*dst++ = 0;
					if (!chk_sh_and)
						continue;
				}
				n_draw_shift = width >> 2;
				if (chk_sh_and & 1) {
					*(WORD *)dst = 0;
					dst += 2;
					if (!n_draw_shift)
						continue;
				}
				do {
					*(DWORD *)dst = 0;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			} while (yy_32);
		LABEL_205:
			dst -= (SCREEN_WIDTH + 160);
			--xx_32;
		} while (xx_32);
		break;
	case 2: // upper (solid), black
		xx_32 = 30;
		while (dst >= gpBufEnd) {
			dst += xx_32;
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = 0;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = 0;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32) {
				yy_32 = 2;
				do {
					if (dst < gpBufEnd)
						break;
					dst += yy_32;
					n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
					if ((32 - yy_32) & 2) {
						*(WORD *)dst = 0;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							*(DWORD *)dst = 0;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
					dst -= (SCREEN_WIDTH + 160);
					yy_32 += 2;
				} while (yy_32 != 32);
				return;
			}
			xx_32 -= 2;
		}
		break;
	case 3: // upper (solid), black
		xx_32 = 30;
		while (dst >= gpBufEnd) {
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = 0;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = 0;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32) {
				yy_32 = 2;
				do {
					if (dst < gpBufEnd)
						break;
					n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
					if ((32 - yy_32) & 2) {
						*(WORD *)dst = 0;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							*(DWORD *)dst = 0;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
					dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
					yy_32 += 2;
				} while (yy_32 != 32);
				return;
			}
			dst += xx_32;
			xx_32 -= 2;
		}
		break;
	case 4: // upper (solid), black
		xx_32 = 30;
		while (dst >= gpBufEnd) {
			dst += xx_32;
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = 0;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = 0;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32) {
				i = 16;
				do {
					if (dst < gpBufEnd)
						break;
					j = 8;
					do {
						*(DWORD *)dst = 0;
						dst += 4;
						--j;
					} while (j);
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				return;
			}
			xx_32 -= 2;
		}
		break;
	default: // upper (solid), black
		xx_32 = 30;
		while (dst >= gpBufEnd) {
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = 0;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = 0;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			if (!xx_32) {
				i = 16;
				do {
					if (dst < gpBufEnd)
						break;
					j = 8;
					do {
						*(DWORD *)dst = 0;
						dst += 4;
						--j;
					} while (j);
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				return;
			}
			dst += xx_32;
			xx_32 -= 2;
		}
		break;
	}
}

void drawTopArchesLowerScreen(BYTE *pBuff)
{
	BYTE *dst, *src;
	BYTE *tbl;
	short cel_type_16;
	unsigned int world_tbl;
	unsigned int width;
	unsigned int chk_sh_and;
	unsigned int x_minus;
	unsigned int n_draw_shift;
	unsigned int y_minus;
	int tile_42_45;
	int xx_32, yy_32;
	int i, j;

	gpCelFrame = (BYTE *)SpeedFrameTbl;
	dst = pBuff;
	if (!(BYTE)light_table_index) {
		if (level_cel_block & 0x8000)
			level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
			    + (WORD)(level_cel_block & 0xF000);
		src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
		goto LABEL_11;
	}
	if ((BYTE)light_table_index == lightmax) {
		if (level_cel_block & 0x8000)
			level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
			    + (WORD)(level_cel_block & 0xF000);
		src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = (level_cel_block >> 12) & 7;
		switch (cel_type_16) {
		case 0: // lower (top transparent), black
			i = 16;
			do {
				if (dst < gpBufEnd) {
					j = 8;
					do {
						dst[1] = 0;
						dst[3] = 0;
						dst += 4;
						--j;
					} while (j);
				} else {
					src += 32;
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				if (dst < gpBufEnd) {
					j = 8;
					do {
						dst[0] = 0;
						dst[2] = 0;
						dst += 4;
						--j;
					} while (j);
				} else {
					src += 32;
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				--i;
			} while (i);
			break;
		case 1: // lower (top transparent), black
			WorldBoolFlag = (BYTE)pBuff & 1;
			xx_32 = 32;
		LABEL_412:
			yy_32 = 32;
			while (1) {
				while (1) {
					width = *src++;
					if ((width & 0x80) == 0)
						break;
					width = -(char)width;
					dst += width;
					yy_32 -= width;
					if (!yy_32) {
					LABEL_433:
						WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
						dst -= (SCREEN_WIDTH + 160);
						if (!--xx_32)
							return;
						goto LABEL_412;
					}
				}
				yy_32 -= width;
				if (dst < gpBufEnd) {
					src += width;
					if (((BYTE)dst & 1) == WorldBoolFlag) {
						chk_sh_and = width >> 1;
						if (!(width & 1))
							goto LABEL_420;
						++dst;
						if (chk_sh_and) {
						LABEL_427:
							n_draw_shift = chk_sh_and >> 1;
							if (chk_sh_and & 1) {
								dst[0] = 0;
								dst += 2;
							}
							if (n_draw_shift) {
								do {
									dst[0] = 0;
									dst[2] = 0;
									dst += 4;
									--n_draw_shift;
								} while (n_draw_shift);
							}
							goto LABEL_430;
						}
					} else {
						chk_sh_and = width >> 1;
						if (!(width & 1))
							goto LABEL_427;
						*dst++ = 0;
						if (chk_sh_and) {
						LABEL_420:
							n_draw_shift = chk_sh_and >> 1;
							if (chk_sh_and & 1) {
								dst[1] = 0;
								dst += 2;
							}
							if (n_draw_shift) {
								do {
									dst[1] = 0;
									dst[3] = 0;
									dst += 4;
									--n_draw_shift;
								} while (n_draw_shift);
							}
							goto LABEL_430;
						}
					}
				} else {
					src += width;
					dst += width;
				}
			LABEL_430:
				if (!yy_32)
					goto LABEL_433;
			}
			break;
		case 2: // lower (top transparent), black
			WorldBoolFlag = 0;
			for (xx_32 = 30;; xx_32 -= 2) {
				if (dst < gpBufEnd) {
					dst += xx_32;
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						n_draw_shift = x_minus >> 2;
						if (x_minus & 2) {
							dst[1] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[1] = 0;
								dst[3] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						n_draw_shift = x_minus >> 2;
						if (x_minus & 2) {
							dst[0] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[0] = 0;
								dst[2] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					}
				} else {
					src = &src[-xx_32 + 32];
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				if (!xx_32)
					break;
			}
			yy_32 = 2;
			do {
				if (dst < gpBufEnd) {
					dst += yy_32;
					y_minus = 32 - yy_32;
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						n_draw_shift = y_minus >> 2;
						if (y_minus & 2) {
							dst[1] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[1] = 0;
								dst[3] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						n_draw_shift = y_minus >> 2;
						if (y_minus & 2) {
							dst[0] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[0] = 0;
								dst[2] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					}
				} else {
					src = &src[-yy_32 + 32];
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				yy_32 += 2;
			} while (yy_32 != 32);
			break;
		case 3: // lower (top transparent), black
			WorldBoolFlag = 0;
			for (xx_32 = 30;; xx_32 -= 2) {
				if (dst < gpBufEnd) {
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						n_draw_shift = x_minus >> 2;
						if (x_minus & 2) {
							dst[1] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[1] = 0;
								dst[3] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						n_draw_shift = x_minus >> 2;
						if (x_minus & 2) {
							dst[0] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[0] = 0;
								dst[2] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					}
				} else {
					src = &src[-xx_32 + 32];
					dst = &dst[-xx_32 + 32];
				}
				dst -= (SCREEN_WIDTH + 160);
				if (!xx_32)
					break;
				dst += xx_32;
			}
			yy_32 = 2;
			do {
				if (dst < gpBufEnd) {
					y_minus = 32 - yy_32;
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						n_draw_shift = y_minus >> 2;
						if (y_minus & 2) {
							dst[1] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[1] = 0;
								dst[3] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						n_draw_shift = y_minus >> 2;
						if (y_minus & 2) {
							dst[0] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[0] = 0;
								dst[2] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					}
				} else {
					src = &src[-yy_32 + 32];
					dst = &dst[-yy_32 + 32];
				}
				dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
				yy_32 += 2;
			} while (yy_32 != 32);
			break;
		case 4: // lower (top transparent), black
			WorldBoolFlag = 0;
			for (xx_32 = 30;; xx_32 -= 2) {
				if (dst < gpBufEnd) {
					dst += xx_32;
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						n_draw_shift = x_minus >> 2;
						if (x_minus & 2) {
							dst[1] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[1] = 0;
								dst[3] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						n_draw_shift = x_minus >> 2;
						if (x_minus & 2) {
							dst[0] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[0] = 0;
								dst[2] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					}
				} else {
					src = &src[-xx_32 + 32];
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				if (!xx_32)
					break;
			}
			i = 8;
			do {
				if (dst < gpBufEnd) {
					j = 8;
					do {
						dst[1] = 0;
						dst[3] = 0;
						dst += 4;
						--j;
					} while (j);
				} else {
					src += 32;
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				if (dst < gpBufEnd) {
					j = 8;
					do {
						dst[0] = 0;
						dst[2] = 0;
						dst += 4;
						--j;
					} while (j);
				} else {
					src += 32;
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				--i;
			} while (i);
			break;
		default: // lower (top transparent), black
			WorldBoolFlag = 0;
			for (xx_32 = 30;; xx_32 -= 2) {
				if (dst < gpBufEnd) {
					x_minus = 32 - xx_32;
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						n_draw_shift = x_minus >> 2;
						if (x_minus & 2) {
							dst[1] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[1] = 0;
								dst[3] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						n_draw_shift = x_minus >> 2;
						if (x_minus & 2) {
							dst[0] = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[0] = 0;
								dst[2] = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					}
				} else {
					src = &src[-xx_32 + 32];
					dst = &dst[-xx_32 + 32];
				}
				dst -= (SCREEN_WIDTH + 160);
				if (!xx_32)
					break;
				dst += xx_32;
			}
			i = 8;
			do {
				if (dst < gpBufEnd) {
					j = 8;
					do {
						dst[1] = 0;
						dst[3] = 0;
						dst += 4;
						--j;
					} while (j);
				} else {
					src += 32;
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				if (dst < gpBufEnd) {
					j = 8;
					do {
						dst[0] = 0;
						dst[2] = 0;
						dst += 4;
						--j;
					} while (j);
				} else {
					src += 32;
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				--i;
			} while (i);
			break;
		}
		return;
	}
	if (!(level_cel_block & 0x8000)) {
		src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		tbl = &pLightTbl[256 * light_table_index];
		cel_type_16 = (BYTE)(level_cel_block >> 12);
		switch (cel_type_16) {
		case 0: // lower (top transparent), with lighting
			i = 16;
			do {
				if (dst < gpBufEnd) {
					asm_trans_light_square_1_3(8, tbl, &dst, &src);
				} else {
					src += 32;
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				if (dst < gpBufEnd) {
					asm_trans_light_square_0_2(8, tbl, &dst, &src);
				} else {
					src += 32;
					dst += 32;
				}
				dst -= (SCREEN_WIDTH + 160);
				--i;
			} while (i);
			break;
		case 1: // lower (top transparent), with lighting
			WorldBoolFlag = (BYTE)pBuff & 1;
			xx_32 = 32;
			do {
				yy_32 = 32;
				do {
					while (1) {
						width = *src++;
						if ((width & 0x80) == 0)
							break;
						width = -(char)width;
						dst += width;
						yy_32 -= width;
						if (!yy_32)
							goto LABEL_69;
					}
					yy_32 -= width;
					if (dst < gpBufEnd) {
						if (((BYTE)dst & 1) == WorldBoolFlag) {
							asm_trans_light_cel_0_2(width, tbl, &dst, &src);
						} else {
							asm_trans_light_cel_1_3(width, tbl, &dst, &src);
						}
					} else {
						src += width;
						dst += width;
					}
				} while (yy_32);
			LABEL_69:
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				dst -= (SCREEN_WIDTH + 160);
				--xx_32;
			} while (xx_32);
			break;
		case 2: // lower (top transparent), with lighting
			WorldBoolFlag = 0;
			xx_32 = 30;
			if (pBuff >= gpBufEnd) {
				tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
				if (tile_42_45 > 45) {
					dst = pBuff - 12288;
					src += 288;
				LABEL_98:
					yy_32 = 2;
					if (dst >= gpBufEnd) {
						tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
						if (tile_42_45 > 42)
							return;
						world_tbl = WorldTbl3x16[tile_42_45];
						src += WorldTbl17_2[world_tbl >> 2];
						dst -= 192 * world_tbl;
						world_tbl >>= 1;
						yy_32 = world_tbl + 2;
						WorldBoolFlag += world_tbl >> 1;
					}
					do {
						dst += yy_32;
						src += (32 - (BYTE)yy_32) & 2;
						WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
						if (WorldBoolFlag) {
							asm_trans_light_cel_0_2(32 - yy_32, tbl, &dst, &src);
						} else {
							asm_trans_light_cel_1_3(32 - yy_32, tbl, &dst, &src);
						}
						dst -= (SCREEN_WIDTH + 160);
						yy_32 += 2;
					} while (yy_32 != 32);
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				src += WorldTbl17_1[world_tbl >> 2];
				dst -= 192 * world_tbl;
				world_tbl >>= 1;
				xx_32 = 30 - world_tbl;
				WorldBoolFlag += world_tbl >> 1;
			}
			do {
				dst += xx_32;
				src += (32 - (BYTE)xx_32) & 2;
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				if (WorldBoolFlag) {
					asm_trans_light_cel_0_2(32 - xx_32, tbl, &dst, &src);
				} else {
					asm_trans_light_cel_1_3(32 - xx_32, tbl, &dst, &src);
				}
				dst -= (SCREEN_WIDTH + 160);
				xx_32 -= 2;
			} while (xx_32 >= 0);
			goto LABEL_98;
		case 3: // lower (top transparent), with lighting
			WorldBoolFlag = 0;
			xx_32 = 30;
			if (pBuff >= gpBufEnd) {
				tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
				if (tile_42_45 > 45) {
					dst = pBuff - 12288;
					src += 288;
				LABEL_154:
					yy_32 = 2;
					if (dst >= gpBufEnd) {
						tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
						if (tile_42_45 > 42)
							return;
						world_tbl = WorldTbl3x16[tile_42_45];
						src += WorldTbl17_2[world_tbl >> 2];
						dst -= 192 * world_tbl;
						world_tbl >>= 1;
						yy_32 = world_tbl + 2;
						WorldBoolFlag += world_tbl >> 1;
					}
					do {
						WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
						if (WorldBoolFlag) {
							asm_trans_light_cel_0_2(32 - yy_32, tbl, &dst, &src);
						} else {
							asm_trans_light_cel_1_3(32 - yy_32, tbl, &dst, &src);
						}
						src += (BYTE)src & 2;
						dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
						yy_32 += 2;
					} while (yy_32 != 32);
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				src += WorldTbl17_1[world_tbl >> 2];
				dst -= 192 * world_tbl;
				world_tbl >>= 1;
				xx_32 = 30 - world_tbl;
				WorldBoolFlag += world_tbl >> 1;
			}
			do {
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				if (WorldBoolFlag) {
					asm_trans_light_cel_0_2(32 - xx_32, tbl, &dst, &src);
				} else {
					asm_trans_light_cel_1_3(32 - xx_32, tbl, &dst, &src);
				}
				src += (BYTE)src & 2;
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
			} while (xx_32 >= 0);
			goto LABEL_154;
		case 4: // lower (top transparent), with lighting
			WorldBoolFlag = 0;
			xx_32 = 30;
			if (pBuff >= gpBufEnd) {
				tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
				if (tile_42_45 > 45) {
					dst = pBuff - 12288;
					src += 288;
				LABEL_210:
					i = 8;
					do {
						if (dst < gpBufEnd) {
							asm_trans_light_square_1_3(8, tbl, &dst, &src);
						} else {
							src += 32;
							dst += 32;
						}
						dst -= (SCREEN_WIDTH + 160);
						if (dst < gpBufEnd) {
							asm_trans_light_square_0_2(8, tbl, &dst, &src);
						} else {
							src += 32;
							dst += 32;
						}
						dst -= (SCREEN_WIDTH + 160);
						--i;
					} while (i);
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				src += WorldTbl17_1[world_tbl >> 2];
				dst -= 192 * world_tbl;
				world_tbl >>= 1;
				xx_32 = 30 - world_tbl;
				WorldBoolFlag += world_tbl >> 1;
			}
			do {
				dst += xx_32;
				src += (32 - (BYTE)xx_32) & 2;
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				if (WorldBoolFlag) {
					asm_trans_light_cel_0_2(32 - xx_32, tbl, &dst, &src);
				} else {
					asm_trans_light_cel_1_3(32 - xx_32, tbl, &dst, &src);
				}
				dst -= (SCREEN_WIDTH + 160);
				xx_32 -= 2;
			} while (xx_32 >= 0);
			goto LABEL_210;
		default: // lower (top transparent), with lighting
			WorldBoolFlag = 0;
			xx_32 = 30;
			if (pBuff >= gpBufEnd) {
				tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
				if (tile_42_45 > 45) {
					dst = pBuff - 12288;
					src += 288;
				LABEL_249:
					i = 8;
					do {
						if (dst < gpBufEnd) {
							asm_trans_light_square_1_3(8, tbl, &dst, &src);
						} else {
							src += 32;
							dst += 32;
						}
						dst -= (SCREEN_WIDTH + 160);
						if (dst < gpBufEnd) {
							asm_trans_light_square_0_2(8, tbl, &dst, &src);
						} else {
							src += 32;
							dst += 32;
						}
						dst -= (SCREEN_WIDTH + 160);
						--i;
					} while (i);
					return;
				}
				world_tbl = WorldTbl3x16[tile_42_45];
				src += WorldTbl17_1[world_tbl >> 2];
				dst -= 192 * world_tbl;
				world_tbl >>= 1;
				xx_32 = 30 - world_tbl;
				WorldBoolFlag += world_tbl >> 1;
			}
			do {
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				if (WorldBoolFlag) {
					asm_trans_light_cel_0_2(32 - xx_32, tbl, &dst, &src);
				} else {
					asm_trans_light_cel_1_3(32 - xx_32, tbl, &dst, &src);
				}
				src += (BYTE)src & 2;
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
			} while (xx_32 >= 0);
			goto LABEL_249;
		}
		return;
	}
	src = pSpeedCels + *(DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
	cel_type_16 = (BYTE)(level_cel_block >> 12);
LABEL_11:
	switch (cel_type_16) {
	case 8: // lower (top transparent), without lighting
		i = 16;
		do {
			if (dst < gpBufEnd) {
				j = 8;
				do {
					dst[1] = src[1];
					dst[3] = src[3];
					src += 4;
					dst += 4;
					--j;
				} while (j);
			} else {
				src += 32;
				dst += 32;
			}
			dst -= (SCREEN_WIDTH + 160);
			if (dst < gpBufEnd) {
				j = 8;
				do {
					dst[0] = src[0];
					dst[2] = src[2];
					src += 4;
					dst += 4;
					--j;
				} while (j);
			} else {
				src += 32;
				dst += 32;
			}
			dst -= (SCREEN_WIDTH + 160);
			--i;
		} while (i);
		break;
	case 9: // lower (top transparent), without lighting
		WorldBoolFlag = (BYTE)pBuff & 1;
		xx_32 = 32;
		while (1) {
			yy_32 = 32;
			do {
				while (1) {
					width = *src++;
					if ((width & 0x80) != 0)
						break;
					yy_32 -= width;
					if (dst < gpBufEnd) {
						if (((BYTE)dst & 1) == WorldBoolFlag) {
							chk_sh_and = width >> 1;
							if (!(width & 1))
								goto LABEL_280;
							++src;
							++dst;
							if (chk_sh_and) {
							LABEL_287:
								n_draw_shift = chk_sh_and >> 1;
								if (chk_sh_and & 1) {
									dst[0] = src[0];
									src += 2;
									dst += 2;
								}
								if (n_draw_shift) {
									do {
										dst[0] = src[0];
										dst[2] = src[2];
										src += 4;
										dst += 4;
										--n_draw_shift;
									} while (n_draw_shift);
								}
								goto LABEL_290;
							}
						} else {
							chk_sh_and = width >> 1;
							if (!(width & 1))
								goto LABEL_287;
							*dst++ = *src++;
							if (chk_sh_and) {
							LABEL_280:
								n_draw_shift = chk_sh_and >> 1;
								if (chk_sh_and & 1) {
									dst[1] = src[1];
									src += 2;
									dst += 2;
								}
								if (n_draw_shift) {
									do {
										dst[1] = src[1];
										dst[3] = src[3];
										src += 4;
										dst += 4;
										--n_draw_shift;
									} while (n_draw_shift);
								}
								goto LABEL_290;
							}
						}
					} else {
						src += width;
						dst += width;
					}
				LABEL_290:
					if (!yy_32)
						goto LABEL_293;
				}
				width = -(char)width;
				dst += width;
				yy_32 -= width;
			} while (yy_32);
		LABEL_293:
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			dst -= (SCREEN_WIDTH + 160);
			if (!--xx_32)
				return;
		}
	case 10: // lower (top transparent), without lighting
		WorldBoolFlag = 0;
		xx_32 = 30;
		if (pBuff >= gpBufEnd) {
			tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 45) {
				dst = pBuff - 12288;
				src += 288;
			LABEL_308:
				yy_32 = 2;
				if (dst >= gpBufEnd) {
					tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 42)
						return;
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_2[world_tbl >> 2];
					dst -= 192 * world_tbl;
					world_tbl >>= 1;
					yy_32 = world_tbl + 2;
					WorldBoolFlag += world_tbl >> 1;
				}
				do {
					dst += yy_32;
					y_minus = 32 - yy_32;
					WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
					if (WorldBoolFlag) {
						n_draw_shift = y_minus >> 2;
						if (y_minus & 2) {
							dst[1] = src[3];
							src += 4;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								dst[1] = src[1];
								dst[3] = src[3];
								src += 4;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						n_draw_shift = y_minus >> 2;
						if (y_minus & 2) {
							dst[0] = src[2];
							src += 4;
							dst += 2;
							--n_draw_shift; /// BUGFIX: delete this line
						}
						if (n_draw_shift) {
							do {
								dst[0] = src[0];
								dst[2] = src[2];
								src += 4;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					}
					dst -= (SCREEN_WIDTH + 160);
					yy_32 += 2;
				} while (yy_32 != 32);
				return;
			}
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			world_tbl >>= 1;
			xx_32 = 30 - world_tbl;
			WorldBoolFlag += world_tbl >> 1;
		}
		do {
			dst += xx_32;
			x_minus = 32 - xx_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[1] = src[3];
					src += 4;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[1] = src[1];
						dst[3] = src[3];
						src += 4;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			} else {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[0] = src[2];
					src += 4;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[0] = src[0];
						dst[2] = src[2];
						src += 4;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			}
			dst -= (SCREEN_WIDTH + 160);
			xx_32 -= 2;
		} while (xx_32 >= 0);
		goto LABEL_308;
	case 11: // lower (top transparent), without lighting
		WorldBoolFlag = 0;
		xx_32 = 30;
		if (pBuff < gpBufEnd)
			goto LABEL_326;
		tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
		if (tile_42_45 <= 45) {
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			world_tbl >>= 1;
			xx_32 = 30 - world_tbl;
			WorldBoolFlag += world_tbl >> 1;
			do {
			LABEL_326:
				x_minus = 32 - xx_32;
				WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
				if (WorldBoolFlag) {
					for (n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift) {
						dst[1] = src[1];
						dst[3] = src[3];
						src += 4;
						dst += 4;
					}
					if ((32 - (BYTE)xx_32) & 2) {
						dst[1] = src[1];
						src += 4;
						dst += 2;
					}
				} else {
					for (n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift) {
						dst[0] = src[0];
						dst[2] = src[2];
						src += 4;
						dst += 4;
					}
					if ((32 - (BYTE)xx_32) & 2) {
						dst[0] = src[0];
						src += 4;
						dst += 2;
					}
				}
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
			} while (xx_32 >= 0);
			goto LABEL_336;
		}
		dst = pBuff - 12288;
		src += 288;
	LABEL_336:
		yy_32 = 2;
		if (dst >= gpBufEnd) {
			tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 42)
				return;
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_2[world_tbl >> 2];
			dst -= 192 * world_tbl;
			world_tbl >>= 1;
			yy_32 = world_tbl + 2;
			WorldBoolFlag += world_tbl >> 1;
		}
		do {
			y_minus = 32 - yy_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				for (n_draw_shift = y_minus >> 2; n_draw_shift; --n_draw_shift) {
					dst[1] = src[1];
					dst[3] = src[3];
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)yy_32) & 2) {
					dst[1] = src[1];
					src += 4;
					dst += 2;
				}
			} else {
				for (n_draw_shift = y_minus >> 2; n_draw_shift; --n_draw_shift) {
					dst[0] = src[0];
					dst[2] = src[2];
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)yy_32) & 2) {
					dst[0] = src[0];
					src += 4;
					dst += 2;
				}
			}
			dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
			yy_32 += 2;
		} while (yy_32 != 32);
		break;
	case 12: // lower (top transparent), without lighting
		WorldBoolFlag = 0;
		xx_32 = 30;
		if (pBuff >= gpBufEnd) {
			tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 45) {
				dst = pBuff - 12288;
				src += 288;
			LABEL_364:
				i = 8;
				do {
					if (dst < gpBufEnd) {
						j = 8;
						do {
							dst[1] = src[1];
							dst[3] = src[3];
							src += 4;
							dst += 4;
							--j;
						} while (j);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					if (dst < gpBufEnd) {
						j = 8;
						do {
							dst[0] = src[0];
							dst[2] = src[2];
							src += 4;
							dst += 4;
							--j;
						} while (j);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				return;
			}
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			world_tbl >>= 1;
			xx_32 = 30 - world_tbl;
			WorldBoolFlag += world_tbl >> 1;
		}
		do {
			dst += xx_32;
			x_minus = 32 - xx_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[1] = src[3];
					src += 4;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[1] = src[1];
						dst[3] = src[3];
						src += 4;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			} else {
				n_draw_shift = x_minus >> 2;
				if (x_minus & 2) {
					dst[0] = src[2];
					src += 4;
					dst += 2;
				}
				if (n_draw_shift) {
					do {
						dst[0] = src[0];
						dst[2] = src[2];
						src += 4;
						dst += 4;
						--n_draw_shift;
					} while (n_draw_shift);
				}
			}
			dst -= (SCREEN_WIDTH + 160);
			xx_32 -= 2;
		} while (xx_32 >= 0);
		goto LABEL_364;
	default: // lower (top transparent), without lighting
		WorldBoolFlag = 0;
		xx_32 = 30;
		if (pBuff >= gpBufEnd) {
			tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 45) {
				dst = pBuff - 12288;
				src += 288;
			LABEL_389:
				i = 8;
				do {
					if (dst < gpBufEnd) {
						j = 8;
						do {
							dst[1] = src[1];
							dst[3] = src[3];
							src += 4;
							dst += 4;
							--j;
						} while (j);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					if (dst < gpBufEnd) {
						j = 8;
						do {
							dst[0] = src[0];
							dst[2] = src[2];
							src += 4;
							dst += 4;
							--j;
						} while (j);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				return;
			}
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			world_tbl >>= 1;
			xx_32 = 30 - world_tbl;
			WorldBoolFlag += world_tbl >> 1;
		}
		do {
			x_minus = 32 - xx_32;
			WorldBoolFlag = ((BYTE)WorldBoolFlag + 1) & 1;
			if (WorldBoolFlag) {
				for (n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift) {
					dst[1] = src[1];
					dst[3] = src[3];
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)xx_32) & 2) {
					dst[1] = src[1];
					src += 4;
					dst += 2;
				}
			} else {
				for (n_draw_shift = x_minus >> 2; n_draw_shift; --n_draw_shift) {
					dst[0] = src[0];
					dst[2] = src[2];
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)xx_32) & 2) {
					dst[0] = src[0];
					src += 4;
					dst += 2;
				}
			}
			dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
			xx_32 -= 2;
		} while (xx_32 >= 0);
		goto LABEL_389;
	}
}

void drawBottomArchesLowerScreen(BYTE *pBuff, DWORD *pMask)
{
	BYTE *dst, *src;
	BYTE *tbl;
	short cel_type_16;
	int and80_i;
	unsigned int world_tbl;
	unsigned int left_shift;
	unsigned int n_draw_shift;
	int tile_42_45;
	int width;
	int xx_32, yy_32;
	int i;

	gpCelFrame = (BYTE *)SpeedFrameTbl;
	dst = pBuff;
	gpDrawMask = pMask;
	if ((BYTE)light_table_index) {
		if ((BYTE)light_table_index == lightmax) {
			if (level_cel_block & 0x8000)
				level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
				    + (WORD)(level_cel_block & 0xF000);
			src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			cel_type_16 = (level_cel_block >> 12) & 7;
			switch (cel_type_16) {
			case 0: // lower (bottom transparent), black
				yy_32 = 32;
				do {
					if (dst < gpBufEnd) {
						left_shift = *gpDrawMask;
						i = 32;
						do {
							if (left_shift & 0x80000000)
								dst[0] = 0;
							left_shift *= 2;
							++dst;
							--i;
						} while (i);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--yy_32;
				} while (yy_32);
				break;
			case 1: // lower (bottom transparent), black
				xx_32 = 32;
				do {
					gdwCurrentMask = *gpDrawMask;
					yy_32 = 32;
					do {
						while (1) {
							width = *src++;
							if ((width & 0x80) != 0)
								break;
							yy_32 -= width;
							if (dst < gpBufEnd) {
								and80_i = width;
								src += width;
								left_shift = gdwCurrentMask;
								do {
									if (left_shift & 0x80000000)
										dst[0] = 0;
									left_shift *= 2;
									++dst;
									--and80_i;
								} while (and80_i);
								gdwCurrentMask = left_shift;
							} else {
								src += width;
								dst += width;
							}
							if (!yy_32)
								goto LABEL_252;
						}
						width = -(char)width;
						dst += width;
						if (width & 0x1F)
							gdwCurrentMask <<= width & 0x1F;
						yy_32 -= width;
					} while (yy_32);
				LABEL_252:
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--xx_32;
				} while (xx_32);
				break;
			case 2: // lower (bottom transparent), black
				for (i = 30;; i -= 2) {
					if (dst < gpBufEnd) {
						dst += i;
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					if (!i)
						break;
				}
				i = 2;
				do {
					if (dst < gpBufEnd) {
						dst += i;
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					i += 2;
				} while (i != 32);
				break;
			case 3: // lower (bottom transparent), black
				for (i = 30;; i -= 2) {
					if (dst < gpBufEnd) {
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst = &dst[32 - i];
					}
					dst -= (SCREEN_WIDTH + 160);
					if (!i)
						break;
					dst += i;
				}
				i = 2;
				do {
					if (dst < gpBufEnd) {
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst = &dst[32 - i];
					}
					dst = &dst[i - (SCREEN_WIDTH + 160)];
					i += 2;
				} while (i != 32);
				break;
			case 4: // lower (bottom transparent), black
				for (i = 30;; i -= 2) {
					if (dst < gpBufEnd) {
						dst += i;
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					if (!i)
						break;
				}
				gpDrawMask -= 16;
				yy_32 = 16;
				do {
					if (dst < gpBufEnd) {
						left_shift = *gpDrawMask;
						i = 32;
						do {
							if (left_shift & 0x80000000)
								dst[0] = 0;
							left_shift *= 2;
							++dst;
							--i;
						} while (i);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--yy_32;
				} while (yy_32);
				break;
			default: // lower (bottom transparent), black
				for (i = 30;; i -= 2) {
					if (dst < gpBufEnd) {
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst = &dst[32 - i];
					}
					dst -= (SCREEN_WIDTH + 160);
					if (!i)
						break;
					dst += i;
				}
				gpDrawMask -= 16;
				yy_32 = 16;
				do {
					if (dst < gpBufEnd) {
						left_shift = *gpDrawMask;
						i = 32;
						do {
							if (left_shift & 0x80000000)
								dst[0] = 0;
							left_shift *= 2;
							++dst;
							--i;
						} while (i);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--yy_32;
				} while (yy_32);
				break;
			}
			return;
		}
		if (!(level_cel_block & 0x8000)) {
			src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			tbl = &pLightTbl[256 * light_table_index];
			cel_type_16 = (BYTE)(level_cel_block >> 12);
			switch (cel_type_16) {
			case 0: // lower (bottom transparent), with lighting
				yy_32 = 32;
				do {
					if (dst < gpBufEnd) {
						asm_trans_light_mask(32, tbl, &dst, &src, *gpDrawMask);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--yy_32;
				} while (yy_32);
				break;
			case 1: // lower (bottom transparent), with lighting
				xx_32 = 32;
				do {
					gdwCurrentMask = *gpDrawMask;
					yy_32 = 32;
					do {
						while (1) {
							width = *src++;
							if ((width & 0x80) != 0)
								break;
							yy_32 -= width;
							if (dst < gpBufEnd) {
								gdwCurrentMask = asm_trans_light_mask(width, tbl, &dst, &src, gdwCurrentMask);
							} else {
								src += width;
								dst += width;
							}
							if (!yy_32)
								goto LABEL_52;
						}
						width = -(char)width;
						dst += width;
						if (width & 0x1F)
							gdwCurrentMask <<= width & 0x1F;
						yy_32 -= width;
					} while (yy_32);
				LABEL_52:
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--xx_32;
				} while (xx_32);
				break;
			case 2: // lower (bottom transparent), with lighting
				xx_32 = 30;
				if (pBuff >= gpBufEnd) {
					tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 45) {
						dst = pBuff - 12288;
						src += 288;
					LABEL_62:
						yy_32 = 2;
						if (dst >= gpBufEnd) {
							tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
							if (tile_42_45 > 42)
								return;
							world_tbl = WorldTbl3x16[tile_42_45];
							src += WorldTbl17_2[world_tbl >> 2];
							dst -= 192 * world_tbl;
							yy_32 = (world_tbl >> 1) + 2;
						}
						do {
							dst += yy_32;
							src += (32 - (BYTE)yy_32) & 2;
							asm_cel_light_edge(32 - yy_32, tbl, &dst, &src);
							yy_32 += 2;
							dst -= (SCREEN_WIDTH + 160);
						} while (yy_32 != 32);
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_1[world_tbl >> 2];
					dst -= 192 * world_tbl;
					xx_32 = 30 - (world_tbl >> 1);
				}
				do {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
				} while (xx_32 >= 0);
				goto LABEL_62;
			case 3: // lower (bottom transparent), with lighting
				xx_32 = 30;
				if (pBuff >= gpBufEnd) {
					tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 45) {
						dst = pBuff - 12288;
						src += 288;
					LABEL_80:
						yy_32 = 2;
						if (dst >= gpBufEnd) {
							tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
							if (tile_42_45 > 42)
								return;
							world_tbl = WorldTbl3x16[tile_42_45];
							src += WorldTbl17_2[world_tbl >> 2];
							dst -= 192 * world_tbl;
							yy_32 = (world_tbl >> 1) + 2;
						}
						do {
							asm_cel_light_edge(32 - yy_32, tbl, &dst, &src);
							/// BUGFIX: uncomment this line
							// src += (BYTE)src & 2;
							dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
							yy_32 += 2;
						} while (yy_32 != 32);
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_1[world_tbl >> 2];
					dst -= 192 * world_tbl;
					xx_32 = 30 - (world_tbl >> 1);
				}
				do {
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
				} while (xx_32 >= 0);
				goto LABEL_80;
			case 4: // lower (bottom transparent), with lighting
				xx_32 = 30;
				if (pBuff >= gpBufEnd) {
					tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 45) {
						dst = pBuff - 12288;
						src += 288;
					LABEL_98:
						gpDrawMask -= 16;
						yy_32 = 16;
						do {
							if (dst < gpBufEnd) {
								asm_trans_light_mask(32, tbl, &dst, &src, *gpDrawMask);
							} else {
								src += 32;
								dst += 32;
							}
							dst -= (SCREEN_WIDTH + 160);
							--gpDrawMask;
							--yy_32;
						} while (yy_32);
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_1[world_tbl >> 2];
					dst -= 192 * world_tbl;
					xx_32 = 30 - (world_tbl >> 1);
				}
				do {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
				} while (xx_32 >= 0);
				goto LABEL_98;
			default: // lower (bottom transparent), with lighting
				xx_32 = 30;
				if (pBuff >= gpBufEnd) {
					tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 45) {
						dst = pBuff - 12288;
						src += 288;
					LABEL_117:
						gpDrawMask -= 16;
						yy_32 = 16;
						do {
							if (dst < gpBufEnd) {
								asm_trans_light_mask(32, tbl, &dst, &src, *gpDrawMask);
								src += (BYTE)src & 2;
							} else {
								src += 32;
								dst += 32;
							}
							dst -= (SCREEN_WIDTH + 160);
							--gpDrawMask;
							--yy_32;
						} while (yy_32);
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_1[world_tbl >> 2];
					dst -= 192 * world_tbl;
					xx_32 = 30 - (world_tbl >> 1);
				}
				do {
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
				} while (xx_32 >= 0);
				goto LABEL_117;
			}
			return;
		}
		src = pSpeedCels
		    + *(DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (BYTE)(level_cel_block >> 12);
	} else {
		if (level_cel_block & 0x8000)
			level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
			    + (WORD)(level_cel_block & 0xF000);
		src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = ((level_cel_block >> 12) & 7) + 8;
	}
	switch (cel_type_16) {
	case 8: // lower (bottom transparent), without lighting
		yy_32 = 32;
		do {
			if (dst < gpBufEnd) {
				left_shift = *gpDrawMask;
				i = 32;
				do {
					if (left_shift & 0x80000000)
						dst[0] = src[0];
					left_shift *= 2;
					++src;
					++dst;
					--i;
				} while (i);
			} else {
				src += 32;
				dst += 32;
			}
			dst -= (SCREEN_WIDTH + 160);
			--gpDrawMask;
			--yy_32;
		} while (yy_32);
		break;
	case 9: // lower (bottom transparent), without lighting
		xx_32 = 32;
		do {
			gdwCurrentMask = *gpDrawMask;
			yy_32 = 32;
			do {
				while (1) {
					width = *src++;
					if ((width & 0x80) != 0)
						break;
					yy_32 -= width;
					if (dst < gpBufEnd) {
						and80_i = width;
						left_shift = gdwCurrentMask;
						do {
							if (left_shift & 0x80000000)
								dst[0] = src[0];
							left_shift *= 2;
							++src;
							++dst;
							--and80_i;
						} while (and80_i);
						gdwCurrentMask = left_shift;
					} else {
						src += width;
						dst += width;
					}
					if (!yy_32)
						goto LABEL_152;
				}
				width = -(char)width;
				dst += width;
				if (width & 0x1F)
					gdwCurrentMask <<= width & 0x1F;
				yy_32 -= width;
			} while (yy_32);
		LABEL_152:
			dst -= (SCREEN_WIDTH + 160);
			--gpDrawMask;
			--xx_32;
		} while (xx_32);
		break;
	case 10: // lower (bottom transparent), without lighting
		xx_32 = 30;
		if (pBuff >= gpBufEnd) {
			tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 45) {
				dst = pBuff - 12288;
				src += 288;
			LABEL_162:
				yy_32 = 2;
				if (dst >= gpBufEnd) {
					tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 42)
						return;
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_2[world_tbl >> 2];
					dst -= 192 * world_tbl;
					yy_32 = (world_tbl >> 1) + 2;
				}
				do {
					dst += yy_32;
					n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
					if ((32 - yy_32) & 2) {
						*(WORD *)dst = *((WORD *)src + 1);
						src += 4;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							*(DWORD *)dst = *(DWORD *)src;
							src += 4;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
					dst -= (SCREEN_WIDTH + 160);
					yy_32 += 2;
				} while (yy_32 != 32);
				return;
			}
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			xx_32 = 30 - (world_tbl >> 1);
		}
		do {
			dst += xx_32;
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = *((WORD *)src + 1);
				src += 4;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			xx_32 -= 2;
		} while (xx_32 >= 0);
		goto LABEL_162;
	case 11: // lower (bottom transparent), without lighting
		xx_32 = 30;
		if (pBuff < gpBufEnd)
			goto LABEL_175;
		tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
		if (tile_42_45 <= 45) {
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			xx_32 = 30 - (world_tbl >> 1);
			do {
			LABEL_175:
				for (n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift) {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)xx_32) & 2) {
					*(WORD *)dst = *(WORD *)src;
					src += 4;
					dst += 2;
				}
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
			} while (xx_32 >= 0);
			goto LABEL_180;
		}
		dst = pBuff - 12288;
		src += 288;
	LABEL_180:
		yy_32 = 2;
		if (dst >= gpBufEnd) {
			tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 42)
				return;
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_2[world_tbl >> 2];
			dst -= 192 * world_tbl;
			yy_32 = (world_tbl >> 1) + 2;
		}
		do {
			for (n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift) {
				*(DWORD *)dst = *(DWORD *)src;
				src += 4;
				dst += 4;
			}
			if ((32 - (BYTE)yy_32) & 2) {
				*(WORD *)dst = *(WORD *)src;
				src += 4;
				dst += 2;
			}
			dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
			yy_32 += 2;
		} while (yy_32 != 32);
		break;
	case 12: // lower (bottom transparent), without lighting
		xx_32 = 30;
		if (pBuff >= gpBufEnd) {
			tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 45) {
				dst = pBuff - 12288;
				src += 288;
			LABEL_198:
				gpDrawMask -= 16;
				yy_32 = 16;
				do {
					if (dst < gpBufEnd) {
						left_shift = *gpDrawMask;
						i = 32;
						do {
							if (left_shift & 0x80000000)
								dst[0] = src[0];
							left_shift *= 2;
							++src;
							++dst;
							--i;
						} while (i);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--yy_32;
				} while (yy_32);
				return;
			}
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			xx_32 = 30 - (world_tbl >> 1);
		}
		do {
			dst += xx_32;
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = *((WORD *)src + 1);
				src += 4;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			xx_32 -= 2;
		} while (xx_32 >= 0);
		goto LABEL_198;
	default: // lower (bottom transparent), without lighting
		xx_32 = 30;
		if (pBuff >= gpBufEnd) {
			tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 45) {
				dst = pBuff - 12288;
				src += 288;
			LABEL_217:
				gpDrawMask -= 16;
				yy_32 = 16;
				do {
					if (dst < gpBufEnd) {
						left_shift = *gpDrawMask;
						i = 32;
						do {
							if (left_shift & 0x80000000)
								dst[0] = src[0];
							left_shift *= 2;
							++src;
							++dst;
							--i;
						} while (i);
						src += (BYTE)src & 2;
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--gpDrawMask;
					--yy_32;
				} while (yy_32);
				return;
			}
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			xx_32 = 30 - (world_tbl >> 1);
		}
		do {
			for (n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift) {
				*(DWORD *)dst = *(DWORD *)src;
				src += 4;
				dst += 4;
			}
			if ((32 - (BYTE)xx_32) & 2) {
				*(WORD *)dst = *(WORD *)src;
				src += 4;
				dst += 2;
			}
			dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
			xx_32 -= 2;
		} while (xx_32 >= 0);
		goto LABEL_217;
	}
}

void drawLowerScreen(BYTE *pBuff)
{
	BYTE *dst, *src;
	BYTE *tbl;
	short cel_type_16;
	unsigned int chk_sh_and;
	unsigned int world_tbl;
	unsigned int n_draw_shift;
	unsigned int width;
	int tile_42_45;
	int xx_32, yy_32;
	int i, j;

	if (cel_transparency_active) {
		if (!arch_draw_type) {
			drawTopArchesLowerScreen(pBuff);
			return;
		}
		if (arch_draw_type == 1) {
			if (block_lvid[level_piece_id] == 1 || block_lvid[level_piece_id] == 3) {
				drawBottomArchesLowerScreen(pBuff, &LeftMask[31]);
				return;
			}
		}
		if (arch_draw_type == 2) {
			if (block_lvid[level_piece_id] == 2 || block_lvid[level_piece_id] == 3) {
				drawBottomArchesLowerScreen(pBuff, &RightMask[31]);
				return;
			}
		}
	}
	gpCelFrame = (BYTE *)SpeedFrameTbl;
	dst = pBuff;
	if ((BYTE)light_table_index) {
		if ((BYTE)light_table_index == lightmax) {
			if (level_cel_block & 0x8000)
				level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
				    + (WORD)(level_cel_block & 0xF000);
			src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			cel_type_16 = (level_cel_block >> 12) & 7;
			switch (cel_type_16) {
			case 0: // lower (solid), black
				i = 32;
				do {
					if (dst < gpBufEnd) {
						j = 8;
						do {
							*(DWORD *)dst = 0;
							dst += 4;
							--j;
						} while (j);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				break;
			case 1: // lower (solid), black
				xx_32 = 32;
				do {
					yy_32 = 32;
					do {
						while (1) {
							width = *src++;
							if ((width & 0x80) == 0)
								break;
							width = -(char)width;
							dst += width;
							yy_32 -= width;
							if (!yy_32)
								goto LABEL_232;
						}
						yy_32 -= width;
						if (dst < gpBufEnd) {
							src += width;
							chk_sh_and = width >> 1;
							if (width & 1) {
								dst[0] = 0;
								++dst;
							}
							if (chk_sh_and) {
								n_draw_shift = width >> 2;
								if (chk_sh_and & 1) {
									*(WORD *)dst = 0;
									dst += 2;
								}
								if (n_draw_shift) {
									do {
										*(DWORD *)dst = 0;
										dst += 4;
										--n_draw_shift;
									} while (n_draw_shift);
								}
							}
						} else {
							src += width;
							dst += width;
						}
					} while (yy_32);
				LABEL_232:
					dst -= (SCREEN_WIDTH + 160);
					--xx_32;
				} while (xx_32);
				break;
			case 2: // lower (solid), black
				for (i = 30;; i -= 2) {
					if (dst < gpBufEnd) {
						dst += i;
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					if (!i)
						break;
				}
				i = 2;
				do {
					if (dst < gpBufEnd) {
						dst += i;
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					i += 2;
				} while (i != 32);
				break;
			case 3: // lower (solid), black
				for (i = 30;; i -= 2) {
					if (dst < gpBufEnd) {
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst = &dst[32 - i];
					}
					dst -= (SCREEN_WIDTH + 160);
					if (!i)
						break;
					dst += i;
				}
				i = 2;
				do {
					if (dst < gpBufEnd) {
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst = &dst[32 - i];
					}
					dst = &dst[i - (SCREEN_WIDTH + 160)];
					i += 2;
				} while (i != 32);
				break;
			case 4: // lower (solid), black
				for (i = 30;; i -= 2) {
					if (dst < gpBufEnd) {
						dst += i;
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					if (!i)
						break;
				}
				i = 16;
				do {
					if (dst < gpBufEnd) {
						j = 8;
						do {
							*(DWORD *)dst = 0;
							dst += 4;
							--j;
						} while (j);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				break;
			default: // lower (solid), black
				for (i = 30;; i -= 2) {
					if (dst < gpBufEnd) {
						n_draw_shift = (unsigned int)(32 - i) >> 2;
						if ((32 - i) & 2) {
							*(WORD *)dst = 0;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = 0;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					} else {
						src = &src[32 - i];
						dst = &dst[32 - i];
					}
					dst -= (SCREEN_WIDTH + 160);
					if (!i)
						break;
					dst += i;
				}
				i = 16;
				do {
					if (dst < gpBufEnd) {
						j = 8;
						do {
							*(DWORD *)dst = 0;
							dst += 4;
							--j;
						} while (j);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				break;
			}
			return;
		}
		if (!(level_cel_block & 0x8000)) {
			src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
			tbl = &pLightTbl[256 * light_table_index];
			cel_type_16 = (WORD)level_cel_block >> 12;
			switch (cel_type_16) {
			case 0: // lower (solid), with lighting
				xx_32 = 32;
				do {
					if (dst < gpBufEnd) {
						asm_cel_light_square(8, tbl, &dst, &src);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--xx_32;
				} while (xx_32);
				break;
			case 1: // lower (solid), with lighting
				xx_32 = 32;
				do {
					yy_32 = 32;
					do {
						width = *src++;
						if ((width & 0x80) == 0) {
							yy_32 -= width;
							if (dst < gpBufEnd) {
								asm_cel_light_edge(width, tbl, &dst, &src);
							} else {
								src += width;
								dst += width;
							}
						} else {
							width = -(char)width;
							dst += width;
							yy_32 -= width;
						}
					} while (yy_32);
					dst -= (SCREEN_WIDTH + 160);
					--xx_32;
				} while (xx_32);
				break;
			case 2: // lower (solid), with lighting
				xx_32 = 30;
				if (pBuff >= gpBufEnd) {
					tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 45) {
						dst = pBuff - 12288;
						src += 288;
					LABEL_68:
						yy_32 = 2;
						if (dst >= gpBufEnd) {
							tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
							if (tile_42_45 > 42)
								return;
							world_tbl = WorldTbl3x16[tile_42_45];
							src += WorldTbl17_2[world_tbl >> 2];
							dst -= 192 * world_tbl;
							yy_32 = (world_tbl >> 1) + 2;
						}
						do {
							dst += yy_32;
							src += (32 - (BYTE)yy_32) & 2;
							asm_cel_light_edge(32 - yy_32, tbl, &dst, &src);
							yy_32 += 2;
							dst -= (SCREEN_WIDTH + 160);
						} while (yy_32 != 32);
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_1[world_tbl >> 2];
					dst -= 192 * world_tbl;
					xx_32 = 30 - (world_tbl >> 1);
				}
				do {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
				} while (xx_32 >= 0);
				goto LABEL_68;
			case 3: // lower (solid), with lighting
				xx_32 = 30;
				if (pBuff >= gpBufEnd) {
					tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 45) {
						dst = pBuff - 12288;
						src += 288;
					LABEL_83:
						yy_32 = 2;
						if (dst >= gpBufEnd) {
							tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
							if (tile_42_45 > 42)
								return;
							world_tbl = WorldTbl3x16[tile_42_45];
							src += WorldTbl17_2[world_tbl >> 2];
							dst -= 192 * world_tbl;
							yy_32 = (world_tbl >> 1) + 2;
						}
						do {
							asm_cel_light_edge(32 - yy_32, tbl, &dst, &src);
							src += (BYTE)src & 2;
							dst = &dst[yy_32 - (SCREEN_WIDTH + 160)];
							yy_32 += 2;
						} while (yy_32 != 32);
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_1[world_tbl >> 2];
					dst -= 192 * world_tbl;
					xx_32 = 30 - (world_tbl >> 1);
				}
				do {
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
				} while (xx_32 >= 0);
				goto LABEL_83;
			case 4: // lower (solid), with lighting
				xx_32 = 30;
				if (pBuff >= gpBufEnd) {
					tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 45) {
						dst = pBuff - 12288;
						src += 288;
					LABEL_100:
						i = 16;
						do {
							if (dst < gpBufEnd) {
								asm_cel_light_square(8, tbl, &dst, &src);
							} else {
								src += 32;
								dst += 32;
							}
							dst -= (SCREEN_WIDTH + 160);
							--i;
						} while (i);
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_1[world_tbl >> 2];
					dst -= 192 * world_tbl;
					xx_32 = 30 - (world_tbl >> 1);
				}
				do {
					dst += xx_32;
					src += (32 - (BYTE)xx_32) & 2;
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					dst -= (SCREEN_WIDTH + 160);
					xx_32 -= 2;
				} while (xx_32 >= 0);
				goto LABEL_100;
			default: // lower (solid), with lighting
				xx_32 = 30;
				if (pBuff >= gpBufEnd) {
					tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 45) {
						dst = pBuff - 12288;
						src += 288;
					LABEL_116:
						j = 16;
						do {
							if (dst < gpBufEnd) {
								asm_cel_light_square(8, tbl, &dst, &src);
							} else {
								src += 32;
								dst += 32;
							}
							dst -= (SCREEN_WIDTH + 160);
							--j;
						} while (j);
						return;
					}
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_1[world_tbl >> 2];
					dst -= 192 * world_tbl;
					xx_32 = 30 - (world_tbl >> 1);
				}
				do {
					asm_cel_light_edge(32 - xx_32, tbl, &dst, &src);
					src += (BYTE)src & 2;
					dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
					xx_32 -= 2;
				} while (xx_32 >= 0);
				goto LABEL_116;
			}
			return;
		}
		src = pSpeedCels
		    + *(DWORD *)&gpCelFrame[4 * (light_table_index + 16 * (level_cel_block & 0xFFF))];
		cel_type_16 = (WORD)level_cel_block >> 12;
	} else {
		if (level_cel_block & 0x8000)
			level_cel_block = *(DWORD *)&gpCelFrame[64 * (level_cel_block & 0xFFF)]
			    + (WORD)(level_cel_block & 0xF000);
		src = pDungeonCels + *((DWORD *)pDungeonCels + (level_cel_block & 0xFFF));
		cel_type_16 = (((unsigned int)level_cel_block >> 12) & 7) + 8;
	}
	switch (cel_type_16) {
	case 8: // lower (solid), without lighting
		i = 32;
		do {
			if (dst < gpBufEnd) {
				j = 8;
				do {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
					--j;
				} while (j);
			} else {
				src += 32;
				dst += 32;
			}
			dst -= (SCREEN_WIDTH + 160);
			--i;
		} while (i);
		break;
	case 9: // lower (solid), without lighting
		xx_32 = 32;
		do {
			yy_32 = 32;
			do {
				while (1) {
					width = *src++;
					if ((width & 0x80) == 0)
						break;
					width = -(char)width;
					dst += width;
					yy_32 -= width;
					if (!yy_32)
						goto LABEL_143;
				}
				yy_32 -= width;
				if (dst < gpBufEnd) {
					chk_sh_and = width >> 1;
					if (width & 1) {
						dst[0] = src[0];
						++src;
						++dst;
					}
					if (chk_sh_and) {
						n_draw_shift = chk_sh_and >> 1;
						if (chk_sh_and & 1) {
							*(WORD *)dst = *(WORD *)src;
							src += 2;
							dst += 2;
						}
						if (n_draw_shift) {
							do {
								*(DWORD *)dst = *(DWORD *)src;
								src += 4;
								dst += 4;
								--n_draw_shift;
							} while (n_draw_shift);
						}
					}
				} else {
					src += width;
					dst += width;
				}
			} while (yy_32);
		LABEL_143:
			dst -= (SCREEN_WIDTH + 160);
			--xx_32;
		} while (xx_32);
		break;
	case 10: // lower (solid), without lighting
		xx_32 = 30;
		if (pBuff >= gpBufEnd) {
			tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 45) {
				dst = pBuff - 12288;
				src += 288;
			LABEL_153:
				yy_32 = 2;
				if (dst >= gpBufEnd) {
					tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
					if (tile_42_45 > 42)
						return;
					world_tbl = WorldTbl3x16[tile_42_45];
					src += WorldTbl17_2[world_tbl >> 2];
					dst -= 192 * world_tbl;
					yy_32 = (world_tbl >> 1) + 2;
				}
				do {
					dst += yy_32;
					n_draw_shift = (unsigned int)(32 - yy_32) >> 2;
					if ((32 - yy_32) & 2) {
						*(WORD *)dst = *((WORD *)src + 1);
						src += 4;
						dst += 2;
					}
					if (n_draw_shift) {
						do {
							*(DWORD *)dst = *(DWORD *)src;
							src += 4;
							dst += 4;
							--n_draw_shift;
						} while (n_draw_shift);
					}
					yy_32 += 2;
					dst -= (SCREEN_WIDTH + 160);
				} while (yy_32 < 32);
				return;
			}
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			xx_32 = 30 - (world_tbl >> 1);
		}
		do {
			dst += xx_32;
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = *((WORD *)src + 1);
				src += 4;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			xx_32 -= 2;
		} while (xx_32 >= 0);
		goto LABEL_153;
	case 11: // lower (solid), without lighting
		xx_32 = 30;
		if (pBuff < gpBufEnd)
			goto LABEL_166;
		tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
		if (tile_42_45 <= 45) {
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			xx_32 = 30 - (world_tbl >> 1);
			do {
			LABEL_166:
				for (n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift) {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
				}
				if ((32 - (BYTE)xx_32) & 2) {
					*(WORD *)dst = *(WORD *)src;
					src += 4;
					dst += 2;
				}
				dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
				xx_32 -= 2;
			} while (xx_32 >= 0);
			goto LABEL_171;
		}
		dst = pBuff - 12288;
		src += 288;
	LABEL_171:
		yy_32 = 2;
		if (dst >= gpBufEnd) {
			tile_42_45 = (unsigned int)(dst - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 42)
				return;
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_2[world_tbl >> 2];
			dst -= 192 * world_tbl;
			yy_32 = (world_tbl >> 1) + 2;
		}
		do {
			for (n_draw_shift = (unsigned int)(32 - yy_32) >> 2; n_draw_shift; --n_draw_shift) {
				*(DWORD *)dst = *(DWORD *)src;
				src += 4;
				dst += 4;
			}
			if ((32 - (BYTE)yy_32) & 2) {
				*(WORD *)dst = *(WORD *)src;
				src += 4;
				dst += 2;
			}
			dst += yy_32;
			yy_32 += 2;
			dst -= (SCREEN_WIDTH + 160);
		} while (yy_32 < 32);
		break;
	case 12: // lower (solid), without lighting
		xx_32 = 30;
		if (pBuff >= gpBufEnd) {
			tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 45) {
				dst = pBuff - 12288;
				src += 288;
			LABEL_189:
				i = 16;
				do {
					if (dst < gpBufEnd) {
						j = 8;
						do {
							*(DWORD *)dst = *(DWORD *)src;
							src += 4;
							dst += 4;
							--j;
						} while (j);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				return;
			}
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			xx_32 = 30 - (world_tbl >> 1);
		}
		do {
			dst += xx_32;
			n_draw_shift = (unsigned int)(32 - xx_32) >> 2;
			if ((32 - xx_32) & 2) {
				*(WORD *)dst = *((WORD *)src + 1);
				src += 4;
				dst += 2;
			}
			if (n_draw_shift) {
				do {
					*(DWORD *)dst = *(DWORD *)src;
					src += 4;
					dst += 4;
					--n_draw_shift;
				} while (n_draw_shift);
			}
			dst -= (SCREEN_WIDTH + 160);
			xx_32 -= 2;
		} while (xx_32 >= 0);
		goto LABEL_189;
	default: // lower (solid), without lighting
		xx_32 = 30;
		if (pBuff >= gpBufEnd) {
			tile_42_45 = (unsigned int)(pBuff - gpBufEnd + 1023) >> 8;
			if (tile_42_45 > 45) {
				dst = pBuff - 12288;
				src += 288;
			LABEL_205:
				i = 16;
				do {
					if (dst < gpBufEnd) {
						j = 8;
						do {
							*(DWORD *)dst = *(DWORD *)src;
							src += 4;
							dst += 4;
							--j;
						} while (j);
					} else {
						src += 32;
						dst += 32;
					}
					dst -= (SCREEN_WIDTH + 160);
					--i;
				} while (i);
				return;
			}
			world_tbl = WorldTbl3x16[tile_42_45];
			src += WorldTbl17_1[world_tbl >> 2];
			dst -= 192 * world_tbl;
			xx_32 = 30 - (world_tbl >> 1);
		}
		do {
			for (n_draw_shift = (unsigned int)(32 - xx_32) >> 2; n_draw_shift; --n_draw_shift) {
				*(DWORD *)dst = *(DWORD *)src;
				src += 4;
				dst += 4;
			}
			if ((32 - (BYTE)xx_32) & 2) {
				*(WORD *)dst = *(WORD *)src;
				src += 4;
				dst += 2;
			}
			dst = &dst[xx_32 - (SCREEN_WIDTH + 160)];
			xx_32 -= 2;
		} while (xx_32 >= 0);
		goto LABEL_205;
	}
}

void world_draw_black_tile(BYTE *pBuff)
{
	BYTE *dst;
	int i, j;
	int xx, yy;

	dst = pBuff;

	xx = 30;
	for (i = 1;; i++) {
		dst += xx;
		j = i;
		do {
			*(DWORD *)dst = 0;
			dst += 4;
			j--;
		} while (j);
		dst = &dst[xx - 832];
		if (!xx)
			break;
		xx -= 2;
	}

	yy = 2;
	i = 15;
	do {
		dst += yy;
		j = i;
		do {
			*(DWORD *)dst = 0;
			dst += 4;
			j--;
		} while (j);
		dst = &dst[yy - 832];
		i--;
		yy += 2;
	} while (yy != 32);
}
#endif

DEVILUTION_END_NAMESPACE
