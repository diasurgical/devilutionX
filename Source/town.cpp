#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

void town_clear_upper_buf(BYTE *pBuff)
{
	/// ASSERT: assert(gpBuffer);

#ifdef USE_ASM
	__asm {
		mov		edi, pBuff
		mov		edx, 30
		mov		ebx, 1
		xor		eax, eax
	label1:
		cmp		edi, gpBufEnd
		jb		label4
		add		edi, edx
		mov		ecx, ebx
		rep stosd
		add		edi, edx
		sub		edi, BUFFER_WIDTH + 64
		or		edx, edx
		jz		label2
		sub		edx, 2
		inc		ebx
		jmp		label1
	label2:
		mov		edx, 2
		mov		ebx, 15
	label3:
		cmp		edi, gpBufEnd
		jb		label4
		add		edi, edx
		mov		ecx, ebx
		rep stosd
		add		edi, edx
		sub		edi, BUFFER_WIDTH + 64
		dec		ebx
		add		edx, 2
		cmp		edx, 32
		jnz		label3
	label4:
		nop
	}
#else
	int i, j, k;
	BYTE *dst;

	dst = pBuff;

	for (i = 30, j = 1; i >= 0 && dst >= gpBufEnd; i -= 2, j++, dst -= BUFFER_WIDTH + 64) {
		dst += i;
		for (k = 0; k < 4 * j; k++)
			*dst++ = 0;
		dst += i;
	}
	for (i = 2, j = 15; i != 32 && dst >= gpBufEnd; i += 2, j--, dst -= BUFFER_WIDTH + 64) {
		dst += i;
		for (k = 0; k < 4 * j; k++)
			*dst++ = 0;
		dst += i;
	}
#endif
}

void town_clear_low_buf(BYTE *pBuff)
{
	/// ASSERT: assert(gpBuffer);

#ifdef USE_ASM
	__asm {
		mov		edi, pBuff
		mov		edx, 30
		mov		ebx, 1
		xor		eax, eax
	label1:
		cmp		edi, gpBufEnd
		jb		label2
		add		edi, 64
		jmp		label3
	label2:
		add		edi, edx
		mov		ecx, ebx
		rep stosd
		add		edi, edx
	label3:
		sub		edi, BUFFER_WIDTH + 64
		or		edx, edx
		jz		label4
		sub		edx, 2
		inc		ebx
		jmp		label1
	label4:
		mov		edx, 2
		mov		ebx, 15
	label5:
		cmp		edi, gpBufEnd
		jb		label6
		add		edi, 64
		jmp		label7
	label6:
		add		edi, edx
		mov		ecx, ebx
		rep stosd
		add		edi, edx
	label7:
		sub		edi, BUFFER_WIDTH + 64
		dec		ebx
		add		edx, 2
		cmp		edx, 32
		jnz		label5
	}
#else
	int i, j, k;
	BYTE *dst;

	dst = pBuff;

	for (i = 30, j = 1; i >= 0; i -= 2, j++, dst -= BUFFER_WIDTH + 64) {
		if (dst < gpBufEnd) {
			dst += i;
			for (k = 0; k < 4 * j; k++)
				*dst++ = 0;
			dst += i;
		} else {
			dst += 64;
		}
	}
	for (i = 2, j = 15; i != 32; i += 2, j--, dst -= BUFFER_WIDTH + 64) {
		if (dst < gpBufEnd) {
			dst += i;
			for (k = 0; k < 4 * j; k++)
				*dst++ = 0;
			dst += i;
		} else {
			dst += 64;
		}
	}
#endif
}

void town_special_lower(BYTE *pBuff, int nCel)
{
#if 0
	int w;
	BYTE *end;

#ifdef USE_ASM
	__asm {
		mov		ebx, pSpecialCels
		mov		eax, nCel
		shl		eax, 2
		add		ebx, eax
		mov		eax, [ebx+4]
		sub		eax, [ebx]
		mov		end, eax
		mov		esi, pSpecialCels
		add		esi, [ebx]
		mov		edi, pBuff
		mov		eax, BUFFER_WIDTH + 64
		mov		w, eax
		mov		ebx, end
		add		ebx, esi
	label1:
		mov		edx, 64
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label7
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		label3
		add		esi, eax
		add		edi, eax
		jmp		label6
	label3:
		mov		ecx, eax
		shr		ecx, 1
		jnb		label4
		movsb
		jecxz	label6
	label4:
		shr		ecx, 1
		jnb		label5
		movsw
		jecxz	label6
	label5:
		rep movsd
	label6:
		or		edx, edx
		jz		label8
		jmp		label2
	label7:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label8:
		sub		edi, w
		cmp		ebx, esi
		jnz		label1
	}
#else
	BYTE width;
	BYTE *src, *dst;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pSpecialCels;
	src = &pSpecialCels[pFrameTable[nCel]];
	dst = pBuff;
	end = &src[pFrameTable[nCel + 1] - pFrameTable[nCel]];

	for(; src != end; dst -= BUFFER_WIDTH + 64) {
		for(w = 64; w;) {
			width = *src++;
			if(!(width & 0x80)) {
				w -= width;
				if(dst < gpBufEnd) {
					if(width & 1) {
						dst[0] = src[0];
						src++;
						dst++;
					}
					width >>= 1;
					if(width & 1) {
						dst[0] = src[0];
						dst[1] = src[1];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for(; width; width--) {
						dst[0] = src[0];
						dst[1] = src[1];
						dst[2] = src[2];
						dst[3] = src[3];
						src += 4;
						dst += 4;
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
#endif
#endif
}

void town_special_upper(BYTE *pBuff, int nCel)
{
#if 0
	int w;
	BYTE *end;

#ifdef USE_ASM
	__asm {
		mov		ebx, pSpecialCels
		mov		eax, nCel
		shl		eax, 2
		add		ebx, eax
		mov		eax, [ebx+4]
		sub		eax, [ebx]
		mov		end, eax
		mov		esi, pSpecialCels
		add		esi, [ebx]
		mov		edi, pBuff
		mov		eax, BUFFER_WIDTH + 64
		mov		w, eax
		mov		ebx, end
		add		ebx, esi
	label1:
		mov		edx, 64
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label6
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		label8
		mov		ecx, eax
		shr		ecx, 1
		jnb		label3
		movsb
		jecxz	label5
	label3:
		shr		ecx, 1
		jnb		label4
		movsw
		jecxz	label5
	label4:
		rep movsd
	label5:
		or		edx, edx
		jz		label7
		jmp		label2
	label6:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label7:
		sub		edi, w
		cmp		ebx, esi
		jnz		label1
	label8:
		nop
	}
#else
	BYTE width;
	BYTE *src, *dst;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pSpecialCels;
	src = &pSpecialCels[pFrameTable[nCel]];
	dst = pBuff;
	end = &src[pFrameTable[nCel + 1] - pFrameTable[nCel]];

	for(; src != end; dst -= BUFFER_WIDTH + 64) {
		for(w = 64; w;) {
			width = *src++;
			if(!(width & 0x80)) {
				w -= width;
				if(dst < gpBufEnd) {
					return;
				}
				if(width & 1) {
					dst[0] = src[0];
					src++;
					dst++;
				}
				width >>= 1;
				if(width & 1) {
					dst[0] = src[0];
					dst[1] = src[1];
					src += 2;
					dst += 2;
				}
				width >>= 1;
				for(; width; width--) {
					dst[0] = src[0];
					dst[1] = src[1];
					dst[2] = src[2];
					dst[3] = src[3];
					src += 4;
					dst += 4;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
#endif
#endif
}

void town_draw_clipped_e_flag(BYTE *pBuff, int x, int y, int sx, int sy)
{
	int i;
	BYTE *dst;
	MICROS *pMap;

	dst = pBuff;
	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];

	for (i = 0; i < 12; i += 2) {
		level_cel_block = pMap->mt[i];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[i + 1];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
		dst -= BUFFER_WIDTH * 32;
	}

	town_draw_clipped_town(pBuff, x, y, sx, sy, 0);
}

void town_draw_clipped_town(BYTE *pBuff, int sx, int sy, int dx, int dy, int eflag)
{
	int mi, px, py;
	char bv;

	/// ASSERT: assert(gpBuffer);

	pBuff = &gpBuffer[dx + PitchTbl[dy]];

	if (dItem[sx][sy] != 0) {
		bv = dItem[sx][sy] - 1;
		px = dx - item[bv]._iAnimWidth2;
		if (bv == pcursitem) {
			CelDrawHdrClrHL(181, px, dy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, 0, 8);
		}
		Cel2DrawHdrOnly(px, dy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, 0, 8);
	}
	if (dFlags[sx][sy] & BFLAG_MONSTLR) {
		mi = -(dMonster[sx][sy - 1] + 1);
		px = dx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelDrawHdrClrHL(166, px, dy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, 8);
		}
		Cel2DrawHdrOnly(px, dy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, 8);
	}
	if (dMonster[sx][sy] > 0) {
		mi = dMonster[sx][sy] - 1;
		px = dx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelDrawHdrClrHL(166, px, dy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, 8);
		}
		Cel2DrawHdrOnly(px, dy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, 8);
	}
	if (dFlags[sx][sy] & BFLAG_PLAYERLR) {
		bv = -(dPlayer[sx][sy - 1] + 1);
		px = dx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = dy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DecodeClrHL(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		}
		Cl2DecodeFrm4(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		if (eflag && plr[bv]._peflag) {
			town_draw_clipped_e_flag(pBuff - 64, sx - 1, sy + 1, dx - 64, dy);
		}
	}
	if (dFlags[sx][sy] & BFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(sx, sy, dx, dy, 0, 8, 1);
	}
	if (dPlayer[sx][sy] > 0) {
		bv = dPlayer[sx][sy] - 1;
		px = dx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = dy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DecodeClrHL(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		}
		Cl2DecodeFrm4(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, 8);
		if (eflag && plr[bv]._peflag) {
			town_draw_clipped_e_flag(pBuff - 64, sx - 1, sy + 1, dx - 64, dy);
		}
	}
	if (dFlags[sx][sy] & BFLAG_MISSILE) {
		DrawClippedMissile(sx, sy, dx, dy, 0, 8, 0);
	}
	if (dArch[sx][sy] != 0) {
		town_special_lower(pBuff, dArch[sx][sy]);
	}
}

void town_draw_lower(int x, int y, int sx, int sy, int chunks, int eflag)
{
	int i, j;
	BYTE *dst;
	MICROS *pMap;

	/// ASSERT: assert(gpBuffer);

	if (eflag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + 32 + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 1; i < 17; i += 2) {
					level_cel_block = pMap->mt[i];
					if (level_cel_block != 0) {
						drawLowerScreen(dst);
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_clipped_town(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 0);
			} else {
				town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
		}
		x++;
		y--;
		sx += 64;
	}

	for (j = 0; j < chunks - eflag; j++) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 16; i += 2) {
					level_cel_block = pMap->mt[i];
					if (level_cel_block != 0) {
						drawLowerScreen(dst);
					}
					level_cel_block = pMap->mt[i + 1];
					if (level_cel_block != 0) {
						drawLowerScreen(dst + 32);
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_clipped_town(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 1);
			} else {
				town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
		}
		x++;
		y--;
		sx += 64;
	}

	if (eflag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 16; i += 2) {
					level_cel_block = pMap->mt[i];
					if (level_cel_block != 0) {
						drawLowerScreen(dst);
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_clipped_town(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 0);
			} else {
				town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
		}
	}
}

void town_draw_clipped_e_flag_2(BYTE *pBuff, int x, int y, int skipChunks, int CelSkip, int sx, int sy)
{
	int i;
	BYTE *dst;
	MICROS *pMap;

	if (skipChunks == 0) {
		dst = pBuff;
	} else {
		dst = &pBuff[BUFFER_WIDTH * 32 * skipChunks];
	}

	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];

	for (i = 0; i < 6; i++) {
		if (skipChunks <= i) {
			level_cel_block = pMap->mt[2 * i + 2];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			level_cel_block = pMap->mt[2 * i + 3];
			if (level_cel_block != 0) {
				drawLowerScreen(dst + 32);
			}
		}
		dst -= BUFFER_WIDTH * 32;
	}

	if (CelSkip < 8) {
		town_draw_clipped_town_2(pBuff, x, y, skipChunks, CelSkip, sx, sy, 0);
	}
}

void town_draw_clipped_town_2(BYTE *pBuff, int sx, int sy, int skipChunks, int CelSkip, int dx, int dy, int eflag)
{
	int mi, px, py;
	char bv;

	if (dItem[sx][sy] != 0) {
		bv = dItem[sx][sy] - 1;
		px = dx - item[bv]._iAnimWidth2;
		if (bv == pcursitem) {
			CelDrawHdrClrHL(181, px, dy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, CelSkip, 8);
		}
		Cel2DrawHdrOnly(px, dy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, CelSkip, 8);
	}
	if (dFlags[sx][sy] & BFLAG_MONSTLR) {
		mi = -(dMonster[sx][sy - 1] + 1);
		px = dx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelDrawHdrClrHL(166, px, dy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, CelSkip, 8);
		}
		Cel2DrawHdrOnly(px, dy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, CelSkip, 8);
	}
	if (dMonster[sx][sy] > 0) {
		mi = dMonster[sx][sy] - 1;
		px = dx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelDrawHdrClrHL(166, px, dy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, CelSkip, 8);
		}
		Cel2DrawHdrOnly(px, dy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, CelSkip, 8);
	}
	if (dFlags[sx][sy] & BFLAG_PLAYERLR) {
		bv = -(dPlayer[sx][sy - 1] + 1);
		px = dx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = dy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DecodeClrHL(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, CelSkip, 8);
		}
		Cl2DecodeFrm4(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, CelSkip, 8);
		if (eflag && plr[bv]._peflag) {
			town_draw_clipped_e_flag_2(pBuff - 64, sx - 1, sy + 1, skipChunks, CelSkip, dx - 64, dy);
		}
	}
	if (dFlags[sx][sy] & BFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(sx, sy, dx, dy, CelSkip, 8, 1);
	}
	if (dPlayer[sx][sy] > 0) {
		bv = dPlayer[sx][sy] - 1;
		px = dx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = dy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DecodeClrHL(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, CelSkip, 8);
		}
		Cl2DecodeFrm4(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, CelSkip, 8);
		if (eflag && plr[bv]._peflag) {
			town_draw_clipped_e_flag_2(pBuff - 64, sx - 1, sy + 1, skipChunks, CelSkip, dx - 64, dy);
		}
	}
	if (dFlags[sx][sy] & BFLAG_MISSILE) {
		DrawClippedMissile(sx, sy, dx, dy, CelSkip, 8, 0);
	}
	if (dArch[sx][sy] != 0) {
		town_special_lower(&pBuff[PitchTbl[16 * CelSkip]], dArch[sx][sy]);
	}
}

void town_draw_lower_2(int x, int y, int sx, int sy, int chunks, int skipChunks, int eflag)
{
	int i, j, CelSkip;
	BYTE *dst;
	MICROS *pMap;

	/// ASSERT: assert(gpBuffer);

	CelSkip = 2 * skipChunks + 2;

	if (eflag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx - (BUFFER_WIDTH * 32 - 32) + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 7; i++) {
					if (skipChunks <= i) {
						level_cel_block = pMap->mt[2 * i + 3];
						if (level_cel_block != 0) {
							drawLowerScreen(dst);
						}
					}
					dst -= BUFFER_WIDTH * 32;
				}
				if (CelSkip < 8) {
					town_draw_clipped_town_2(&gpBuffer[sx + PitchTbl[sy]], x, y, skipChunks, CelSkip, sx, sy, 0);
				}
			} else {
				town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
		}
		x++;
		y--;
		sx += 64;
	}

	for (j = 0; j < chunks - eflag; j++) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx - BUFFER_WIDTH * 32 + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 7; i++) {
					if (skipChunks <= i) {
						level_cel_block = pMap->mt[2 * i + 2];
						if (level_cel_block != 0) {
							drawLowerScreen(dst);
						}
						level_cel_block = pMap->mt[2 * i + 3];
						if (level_cel_block != 0) {
							drawLowerScreen(dst + 32);
						}
					}
					dst -= BUFFER_WIDTH * 32;
				}
				if (CelSkip < 8) {
					town_draw_clipped_town_2(&gpBuffer[sx + PitchTbl[sy] - BUFFER_WIDTH * 16 * CelSkip], x, y, skipChunks, CelSkip, sx, sy, 1);
				}
			} else {
				town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
		}
		x++;
		y--;
		sx += 64;
	}

	if (eflag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx - BUFFER_WIDTH * 32 + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 7; i++) {
					if (skipChunks <= i) {
						level_cel_block = pMap->mt[2 * i + 2];
						if (level_cel_block != 0) {
							drawLowerScreen(dst);
						}
					}
					dst -= BUFFER_WIDTH * 32;
				}
				if (CelSkip < 8) {
					town_draw_clipped_town_2(&gpBuffer[sx + PitchTbl[sy]], x, y, skipChunks, CelSkip, sx, sy, 0);
				}
			} else {
				town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			town_clear_low_buf(&gpBuffer[sx + PitchTbl[sy]]);
		}
	}
}

void town_draw_e_flag(BYTE *pBuff, int x, int y, int capChunks, int CelCap, int sx, int sy)
{
	int i;
	BYTE *dst;
	MICROS *pMap;

	dst = pBuff;
	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];

	for (i = 0; i < 7; i++) {
		if (capChunks >= i) {
			level_cel_block = pMap->mt[2 * i];
			if (level_cel_block != 0) {
				drawUpperScreen(dst);
			}
			level_cel_block = pMap->mt[2 * i + 1];
			if (level_cel_block != 0) {
				drawUpperScreen(dst + 32);
			}
		}
		dst -= BUFFER_WIDTH * 32;
	}

	town_draw_town_all(pBuff, x, y, capChunks, CelCap, sx, sy, 0);
}

void town_draw_town_all(BYTE *pBuff, int x, int y, int capChunks, int CelCap, int sx, int sy, int eflag)
{
	int mi, px, py;
	char bv;

	if (dItem[x][y] != 0) {
		bv = dItem[x][y] - 1;
		px = sx - item[bv]._iAnimWidth2;
		if (bv == pcursitem) {
			CelDecodeClr(181, px, sy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, 0, CelCap);
		}
		/// ASSERT: assert(item[bv]._iAnimData);
		CelDrawHdrOnly(px, sy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth, 0, CelCap);
	}
	if (dFlags[x][y] & BFLAG_MONSTLR) {
		mi = -(dMonster[x][y - 1] + 1);
		px = sx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelDecodeClr(166, px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, CelCap);
		}
		/// ASSERT: assert(towner[mi]._tAnimData);
		CelDrawHdrOnly(px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, CelCap);
	}
	if (dMonster[x][y] > 0) {
		mi = dMonster[x][y] - 1;
		px = sx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelDecodeClr(166, px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, CelCap);
		}
		/// ASSERT: assert(towner[mi]._tAnimData);
		CelDrawHdrOnly(px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth, 0, CelCap);
	}
	if (dFlags[x][y] & BFLAG_PLAYERLR) {
		bv = -(dPlayer[x][y - 1] + 1);
		px = sx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = sy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DecodeFrm2(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, CelCap);
		}
		/// ASSERT: assert(plr[bv]._pAnimData);
		Cl2DecodeFrm1(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, CelCap);
		if (eflag && plr[bv]._peflag) {
			town_draw_e_flag(pBuff - 64, x - 1, y + 1, capChunks, CelCap, sx - 64, sy);
		}
	}
	if (dFlags[x][y] & BFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(x, y, sx, sy, 0, CelCap, 0);
	}
	if (dPlayer[x][y] > 0) {
		bv = dPlayer[x][y] - 1;
		px = sx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = sy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DecodeFrm2(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, CelCap);
		}
		/// ASSERT: assert(plr[bv]._pAnimData);
		Cl2DecodeFrm1(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth, 0, CelCap);
		if (eflag && plr[bv]._peflag) {
			town_draw_e_flag(pBuff - 64, x - 1, y + 1, capChunks, CelCap, sx - 64, sy);
		}
	}
	if (dFlags[x][y] & BFLAG_MISSILE) {
		DrawMissile(x, y, sx, sy, 0, CelCap, 0);
	}
	if (dArch[x][y] != 0) {
		town_special_upper(pBuff, dArch[x][y]);
	}
}

void town_draw_upper(int x, int y, int sx, int sy, int chunks, int capChunks, int eflag)
{
	int i, j, CelCap;
	BYTE *dst;
	MICROS *pMap;

	/// ASSERT: assert(gpBuffer);

	CelCap = 2 * capChunks + 2;
	if (CelCap > 8) {
		CelCap = 8;
	}

	if (eflag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + 32 + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 7; i++) {
					if (capChunks >= i) {
						level_cel_block = pMap->mt[2 * i + 1];
						if (level_cel_block != 0) {
							drawUpperScreen(dst);
						}
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_town_all(&gpBuffer[sx + PitchTbl[sy]], x, y, capChunks, CelCap, sx, sy, 0);
			} else {
				town_clear_upper_buf(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			town_clear_upper_buf(&gpBuffer[sx + PitchTbl[sy]]);
		}
		x++;
		y--;
		sx += 64;
	}

	for (j = 0; j < chunks - eflag; j++) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 7; i++) {
					if (capChunks >= i) {
						level_cel_block = pMap->mt[2 * i];
						if (level_cel_block != 0) {
							drawUpperScreen(dst);
						}
						level_cel_block = pMap->mt[2 * i + 1];
						if (level_cel_block != 0) {
							drawUpperScreen(dst + 32);
						}
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_town_all(&gpBuffer[sx + PitchTbl[sy]], x, y, capChunks, CelCap, sx, sy, 1);
			} else {
				town_clear_upper_buf(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			town_clear_upper_buf(&gpBuffer[sx + PitchTbl[sy]]);
		}
		x++;
		y--;
		sx += 64;
	}

	if (eflag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + PitchTbl[sy]];
				pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
				for (i = 0; i < 7; i++) {
					if (capChunks >= i) {
						level_cel_block = pMap->mt[2 * i];
						if (level_cel_block != 0) {
							drawUpperScreen(dst);
						}
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_town_all(&gpBuffer[sx + PitchTbl[sy]], x, y, capChunks, CelCap, sx, sy, 0);
			} else {
				town_clear_upper_buf(&gpBuffer[sx + PitchTbl[sy]]);
			}
		} else {
			town_clear_upper_buf(&gpBuffer[sx + PitchTbl[sy]]);
		}
	}
}

void T_DrawGame(int x, int y)
{
	int i, sx, sy, chunks, blocks;

	scr_pix_width = SCREEN_WIDTH;
	scr_pix_height = VIEWPORT_HEIGHT;
	dword_5C2FF8 = SCREEN_WIDTH / 64;
	dword_5C2FFC = VIEWPORT_HEIGHT / 32;

	sx = ScrollInfo._sxoff + 64;
	sy = ScrollInfo._syoff + 175;
	x -= 10;
	y--;
	chunks = 10;
	blocks = 5;

	if (chrflag || questlog) {
		x += 2;
		y -= 2;
		sx += 288;
		chunks = 6;
	}
	if (invflag || sbookflag) {
		x += 2;
		y -= 2;
		sx -= 32;
		chunks = 6;
	}

	switch (ScrollInfo._sdir) {
	case SDIR_NONE:
		break;
	case SDIR_N:
		sy -= 32;
		x--;
		y--;
		blocks++;
		break;
	case SDIR_NE:
		sy -= 32;
		x--;
		y--;
		chunks++;
		blocks++;
		break;
	case SDIR_E:
		chunks++;
		break;
	case SDIR_SE:
		chunks++;
		blocks++;
		break;
	case SDIR_S:
		blocks++;
		break;
	case SDIR_SW:
		sx -= 64;
		x--;
		y++;
		chunks++;
		blocks++;
		break;
	case SDIR_W:
		sx -= 64;
		x--;
		y++;
		chunks++;
		break;
	case SDIR_NW:
		sx -= 64;
		sy -= 32;
		x -= 2;
		chunks++;
		blocks++;
		break;
	}

	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[SCREEN_Y]];
	for (i = 0; i < 7; i++) {
		town_draw_upper(x, y, sx, sy, chunks, i, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw_upper(x, y, sx, sy, chunks, i, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[VIEWPORT_HEIGHT + SCREEN_Y]];
	for (i = 0; i < blocks; i++) {
		town_draw_lower(x, y, sx, sy, chunks, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw_lower(x, y, sx, sy, chunks, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	for (i = 0; i < 7; i++) {
		town_draw_lower_2(x, y, sx, sy, chunks, i, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw_lower_2(x, y, sx, sy, chunks, i, 1);
		x++;
		sx += 32;
		sy += 16;
	}
}

void T_DrawZoom(int x, int y)
{
	int i, sx, sy, chunks, blocks;
	int wdt, nSrcOff, nDstOff;

	scr_pix_width = ZOOM_WIDTH;
	scr_pix_height = 192;
	dword_5C2FF8 = ZOOM_WIDTH / 64;
	dword_5C2FFC = 192 / 32;

	sx = ScrollInfo._sxoff + 64;
	sy = ScrollInfo._syoff + 143;
	x -= 6;
	y--;
	chunks = 6;
	blocks = 0;

	switch (ScrollInfo._sdir) {
	case SDIR_NONE:
		break;
	case SDIR_N:
		sy -= 32;
		x--;
		y--;
		blocks++;
		break;
	case SDIR_NE:
		sy -= 32;
		x--;
		y--;
		chunks++;
		blocks++;
		break;
	case SDIR_E:
		chunks++;
		break;
	case SDIR_SE:
		chunks++;
		blocks++;
		break;
	case SDIR_S:
		blocks++;
		break;
	case SDIR_SW:
		sx -= 64;
		x--;
		y++;
		chunks++;
		blocks++;
		break;
	case SDIR_W:
		sx -= 64;
		x--;
		y++;
		chunks++;
		break;
	case SDIR_NW:
		sx -= 64;
		sy -= 32;
		x -= 2;
		chunks++;
		blocks++;
		break;
	}

	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[-17 + SCREEN_Y]];
	for (i = 0; i < 7; i++) {
		town_draw_upper(x, y, sx, sy, chunks, i, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw_upper(x, y, sx, sy, chunks, i, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[160 + SCREEN_Y]];
	for (i = 0; i < blocks; i++) {
		town_draw_lower(x, y, sx, sy, chunks, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw_lower(x, y, sx, sy, chunks, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	for (i = 0; i < 7; i++) {
		town_draw_lower_2(x, y, sx, sy, chunks, i, 0);
		y++;
		sx -= 32;
		sy += 16;
		town_draw_lower_2(x, y, sx, sy, chunks, i, 1);
		x++;
		sx += 32;
		sy += 16;
	}

	if (chrflag || questlog) {
		nSrcOff = SCREENXY(112, 159);
		nDstOff = SCREENXY(320, 350);
		wdt = (SCREEN_WIDTH - 320) / 2;
	} else if (invflag || sbookflag) {
		nSrcOff = SCREENXY(112, 159);
		nDstOff = SCREENXY(0, 350);
		wdt = (SCREEN_WIDTH - 320) / 2;
	} else {
		nSrcOff = SCREENXY(32, 159);
		nDstOff = SCREENXY(0, 350);
		wdt = SCREEN_WIDTH / 2;
	}

	/// ASSERT: assert(gpBuffer);

#ifdef USE_ASM
	__asm {
		mov		esi, gpBuffer
		mov		edx, nDstOff
		mov		edi, esi
		mov		ecx, nSrcOff
		add		edi, edx
		add		esi, ecx
		mov		ebx, edi
		add		ebx, BUFFER_WIDTH
		mov		edx, 176
	label1:
		mov		ecx, wdt
	label2:
		mov		al, [esi]
		inc		esi
		mov		ah, al
		mov		[edi], ax
		mov		[ebx], ax
		add		edi, 2
		add		ebx, 2
		dec		ecx
		jnz		label2
		mov		eax, BUFFER_WIDTH
		add		eax, wdt
		sub		esi, eax
		add		eax, eax
		sub		ebx, eax
		sub		edi, eax
		dec		edx
		jnz		label1
	}
#else
	int hgt;
	BYTE *src, *dst1, *dst2;

	src = &gpBuffer[nSrcOff];
	dst1 = &gpBuffer[nDstOff];
	dst2 = &gpBuffer[nDstOff + BUFFER_WIDTH];

	for (hgt = 176; hgt != 0; hgt--, src -= BUFFER_WIDTH + wdt, dst1 -= 2 * (BUFFER_WIDTH + wdt), dst2 -= 2 * (BUFFER_WIDTH + wdt)) {
		for (i = wdt; i != 0; i--) {
			*dst1++ = *src;
			*dst1++ = *src;
			*dst2++ = *src;
			*dst2++ = *src;
			src++;
		}
	}
#endif
}

void T_DrawView(int StartX, int StartY)
{
	light_table_index = 0;
	cel_transparency_active = 0;
	if (zoomflag) {
		T_DrawGame(StartX, StartY);
	} else {
		T_DrawZoom(StartX, StartY);
	}
	if (automapflag) {
		DrawAutomap();
	}
	if (stextflag && !qtextflag)
		DrawSText();
	if (invflag) {
		DrawInv();
	} else if (sbookflag) {
		DrawSpellBook();
	}

	DrawDurIcon();

	if (chrflag) {
		DrawChr();
	} else if (questlog) {
		DrawQuestLog();
	} else if (plr[myplr]._pStatPts != 0 && !spselflag) {
		DrawLevelUpIcon();
	}
	if (uitemflag) {
		DrawUniqueInfo();
	}
	if (qtextflag) {
		DrawQText();
	}
	if (spselflag) {
		DrawSpellList();
	}
	if (dropGoldFlag) {
		DrawGoldSplit(dropGoldValue);
	}
	if (helpflag) {
		DrawHelp();
	}
	if (msgflag) {
		DrawDiabloMsg();
	}
	if (PauseMode != 0 && !deathflag) {
		gmenu_draw_pause();
	}

	DrawPlrMsg();
	gmenu_draw();
	doom_draw();
	DrawInfoBox();
	DrawLifeFlask();
	DrawManaFlask();
}

void SetTownMicros()
{
	int i, x, y, lv;
	WORD *pPiece;
	MICROS *pMap;

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			lv = dPiece[x][y];
			pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
			if (lv != 0) {
				lv--;
				pPiece = (WORD *)&pLevelPieces[32 * lv];
				for (i = 0; i < 16; i++) {
					pMap->mt[i] = pPiece[(i & 1) + 14 - (i & 0xE)];
				}
			} else {
				for (i = 0; i < 16; i++) {
					pMap->mt[i] = 0;
				}
			}
		}
	}

	if (zoomflag) {
		scr_pix_width = SCREEN_WIDTH;
		scr_pix_height = VIEWPORT_HEIGHT;
		dword_5C2FF8 = SCREEN_WIDTH / 64;
		dword_5C2FFC = VIEWPORT_HEIGHT / 32;
	} else {
		scr_pix_width = ZOOM_WIDTH;
		scr_pix_height = ZOOM_HEIGHT;
		dword_5C2FF8 = ZOOM_WIDTH / 64;
		dword_5C2FFC = ZOOM_HEIGHT / 32;
	}
}

void T_FillSector(BYTE *P3Tiles, BYTE *pSector, int xi, int yi, int w, int h)
{
	int i, j, xx, yy;
	long v1, v2, v3, v4, ii;

	ii = 4;
	yy = yi;
	for (j = 0; j < h; j++) {
		xx = xi;
		for (i = 0; i < w; i++) {
#ifdef USE_ASM
			__asm {
				mov		esi, pSector
				mov		eax, ii
				add		esi, eax
				xor		eax, eax
				lodsw
				or		eax, eax
				jz		label1
				dec		eax
				mov		esi, P3Tiles
				shl		eax, 3
				add		esi, eax
				xor		eax, eax
				lodsw
				inc		eax
				mov		v1, eax
				lodsw
				inc		eax
				mov		v2, eax
				lodsw
				inc		eax
				mov		v3, eax
				lodsw
				inc		eax
				mov		v4, eax
				jmp		label2
			label1:
				mov		v1, eax
				mov		v2, eax
				mov		v3, eax
				mov		v4, eax
			label2:
				nop
			}
#else
			WORD *Map;

			Map = (WORD *)&pSector[ii];
			if (*Map) {
				v1 = *((WORD *)&P3Tiles[(*Map - 1) * 8]) + 1;
				v2 = *((WORD *)&P3Tiles[(*Map - 1) * 8] + 1) + 1;
				v3 = *((WORD *)&P3Tiles[(*Map - 1) * 8] + 2) + 1;
				v4 = *((WORD *)&P3Tiles[(*Map - 1) * 8] + 3) + 1;
			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
			}
#endif
			dPiece[xx][yy] = v1;
			dPiece[xx + 1][yy] = v2;
			dPiece[xx][yy + 1] = v3;
			dPiece[xx + 1][yy + 1] = v4;
			xx += 2;
			ii += 2;
		}
		yy += 2;
	}
}

void T_FillTile(BYTE *P3Tiles, int xx, int yy, int t)
{
	long v1, v2, v3, v4;

#ifdef USE_ASM
	__asm {
		mov		eax, t
		dec		eax
		mov		esi, P3Tiles
		shl		eax, 3
		add		esi, eax
		xor		eax, eax
		lodsw
		inc		eax
		mov		v1, eax
		lodsw
		inc		eax
		mov		v2, eax
		lodsw
		inc		eax
		mov		v3, eax
		lodsw
		inc		eax
		mov		v4, eax
		jmp		label1
		mov		v1, eax
		mov		v2, eax
		mov		v3, eax
		mov		v4, eax
	label1:
		nop
	}
#else
	v1 = *((WORD *)&P3Tiles[(t - 1) * 8]) + 1;
	v2 = *((WORD *)&P3Tiles[(t - 1) * 8] + 1) + 1;
	v3 = *((WORD *)&P3Tiles[(t - 1) * 8] + 2) + 1;
	v4 = *((WORD *)&P3Tiles[(t - 1) * 8] + 3) + 1;
#endif

	dPiece[xx][yy] = v1;
	dPiece[xx + 1][yy] = v2;
	dPiece[xx][yy + 1] = v3;
	dPiece[xx + 1][yy + 1] = v4;
}

void T_Pass3()
{
	int xx, yy, x;
	BYTE *P3Tiles, *pSector;

	for (yy = 0; yy < MAXDUNY; yy += 2) {
		for (xx = 0; xx < MAXDUNX; xx += 2) {
			dPiece[xx][yy] = 0;
			dPiece[xx + 1][yy] = 0;
			dPiece[xx][yy + 1] = 0;
			dPiece[xx + 1][yy + 1] = 0;
		}
	}

	P3Tiles = LoadFileInMem("Levels\\TownData\\Town.TIL", NULL);
	pSector = LoadFileInMem("Levels\\TownData\\Sector1s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 46, 46, 25, 25);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector2s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 46, 0, 25, 23);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector3s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 0, 46, 23, 25);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector4s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 0, 0, 23, 23);
	mem_free_dbg(pSector);

	if (gbMaxPlayers == 1) {
		if (!(plr[myplr].pTownWarps & 1)) {
			T_FillTile(P3Tiles, 48, 20, 320);
		}
		if (!(plr[myplr].pTownWarps & 2)) {
			T_FillTile(P3Tiles, 16, 68, 332);
			T_FillTile(P3Tiles, 16, 70, 331);
		}
		if (!(plr[myplr].pTownWarps & 4)) {
			for (x = 36; x < 46; x++) {
				T_FillTile(P3Tiles, x, 78, random(0, 4) + 1);
			}
		}
	}

	if (quests[13]._qactive != 3 && quests[13]._qactive) {
		T_FillTile(P3Tiles, 60, 70, 342);
	} else {
		T_FillTile(P3Tiles, 60, 70, 71);
	}

	mem_free_dbg(P3Tiles);
}

void CreateTown(int entry)
{
	int x, y;

	dminx = 10;
	dminy = 10;
	dmaxx = 84;
	dmaxy = 84;

	if (entry == 0) {
		ViewX = 75;
		ViewY = 68;
	} else if (entry == 1) {
		ViewX = 25;
		ViewY = 31;
	} else if (entry == 7) {
		if (TWarpFrom == 5) {
			ViewX = 49;
			ViewY = 22;
		}
		if (TWarpFrom == 9) {
			ViewX = 18;
			ViewY = 69;
		}
		if (TWarpFrom == 13) {
			ViewX = 41;
			ViewY = 81;
		}
	}

	T_Pass3();
	memset(dLight, 0, sizeof(dLight));
	memset(dFlags, 0, sizeof(dFlags));
	memset(dPlayer, 0, sizeof(dPlayer));
	memset(dMonster, 0, sizeof(dMonster));
	memset(dObject, 0, sizeof(dObject));
	memset(dItem, 0, sizeof(dItem));
	memset(dArch, 0, sizeof(dArch));

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			if (dPiece[x][y] == 360) {
				dArch[x][y] = 1;
			} else if (dPiece[x][y] == 358) {
				dArch[x][y] = 2;
			} else if (dPiece[x][y] == 129) {
				dArch[x][y] = 6;
			} else if (dPiece[x][y] == 130) {
				dArch[x][y] = 7;
			} else if (dPiece[x][y] == 128) {
				dArch[x][y] = 8;
			} else if (dPiece[x][y] == 117) {
				dArch[x][y] = 9;
			} else if (dPiece[x][y] == 157) {
				dArch[x][y] = 10;
			} else if (dPiece[x][y] == 158) {
				dArch[x][y] = 11;
			} else if (dPiece[x][y] == 156) {
				dArch[x][y] = 12;
			} else if (dPiece[x][y] == 162) {
				dArch[x][y] = 13;
			} else if (dPiece[x][y] == 160) {
				dArch[x][y] = 14;
			} else if (dPiece[x][y] == 214) {
				dArch[x][y] = 15;
			} else if (dPiece[x][y] == 212) {
				dArch[x][y] = 16;
			} else if (dPiece[x][y] == 217) {
				dArch[x][y] = 17;
			} else if (dPiece[x][y] == 216) {
				dArch[x][y] = 18;
			}
		}
	}

	SetTownMicros();
}

DEVILUTION_END_NAMESPACE
