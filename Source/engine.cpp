/**
 * @file engine.cpp
 *
 * Implementation of basic engine helper functions:
 * - Sprite blitting
 * - Drawing
 * - Angle calculation
 * - RNG
 * - Memory allocation
 * - File loading
 * - Video playback
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

/** Seed value before the most recent call to SetRndSeed() */
int orgseed;
/** Current game seed */
int sglGameSeed;
static CCritSect sgMemCrit;

/**
 * Specifies the increment used in the Borland C/C++ pseudo-random.
 */
const int RndInc = 1;

/**
 * Specifies the multiplier used in the Borland C/C++ pseudo-random number generator algorithm.
 */
const int RndMult = 0x015A4E35;

void CelDrawTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	BYTE *pRLEBytes;

	assert(pCelBuff != NULL);
	assert(out.begin != NULL);

	pRLEBytes = CelGetFrame(pCelBuff, nCel, &nDataSize);
	CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth);
}

void CelClippedDrawTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	BYTE *pRLEBytes;
	int nDataSize;

	assert(out.begin != NULL);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth);
}

void CelDrawLightTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, BYTE *tbl)
{
	int nDataSize;
	BYTE *pRLEBytes;

	assert(out.begin != NULL);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrame(pCelBuff, nCel, &nDataSize);

	if (light_table_index || tbl)
		CelBlitLightSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth, tbl);
	else
		CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth);
}

void CelClippedDrawLightTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	BYTE *pRLEBytes;

	assert(out.begin != NULL);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	if (light_table_index)
		CelBlitLightSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth, NULL);
	else
		CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth);
}

void CelDrawLightRedTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light)
{
	int nDataSize, w, idx;
	BYTE *pRLEBytes, *dst, *tbl;

	assert(out.begin != NULL);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	dst = out.at(sx, sy);

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256; // gray colors
	if (light >= 4)
		idx += (light - 1) << 8;

	BYTE width;
	BYTE *end;

	tbl = &pLightTbl[idx];
	end = &pRLEBytes[nDataSize];

	for (; pRLEBytes != end; dst -= out.line_width + nWidth) {
		for (w = nWidth; w;) {
			width = *pRLEBytes++;
			if (!(width & 0x80)) {
				w -= width;
				while (width) {
					*dst = tbl[*pRLEBytes];
					pRLEBytes++;
					dst++;
					width--;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
}

void CelBlitSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int i, w;
	BYTE width;
	BYTE *src, *dst;

	assert(out.begin != NULL);
	assert(pRLEBytes != NULL);

	src = pRLEBytes;
	dst = out.at(sx, sy);
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= out.line_width + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < out.end && dst > out.begin) {
					memcpy(dst, src, width);
				}
				src += width;
				dst += width;
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}

void CelClippedDrawSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	BYTE *pRLEBytes;
	int nDataSize;

	assert(out.begin != NULL);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth);
}

void CelBlitLightSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *tbl)
{
	int i, w;
	BYTE width;
	BYTE *src, *dst;

	assert(out.begin != NULL);
	assert(pRLEBytes != NULL);

	src = pRLEBytes;
	dst = out.at(sx, sy);
	if (tbl == NULL)
		tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= out.line_width + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < out.end && dst > out.begin) {
					if (width & 1) {
						dst[0] = tbl[src[0]];
						src++;
						dst++;
					}
					width >>= 1;
					if (width & 1) {
						dst[0] = tbl[src[0]];
						dst[1] = tbl[src[1]];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for (; width; width--) {
						dst[0] = tbl[src[0]];
						dst[1] = tbl[src[1]];
						dst[2] = tbl[src[2]];
						dst[3] = tbl[src[3]];
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
				i -= width;
			}
		}
	}
}

void CelBlitLightTransSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BOOL shift;
	BYTE *tbl;

	assert(pRLEBytes != NULL);
	assert(out.begin != NULL);

	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = out.at(sx, sy);
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;
	shift = (BYTE)(size_t)dst & 1;

	for (; src != &pRLEBytes[nDataSize]; dst -= out.line_width + w, shift = (shift + 1) & 1) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < out.end && dst > out.begin) {
					if (((BYTE)(size_t)dst & 1) == shift) {
						if (!(width & 1)) {
							goto L_ODD;
						} else {
							src++;
							dst++;
						L_EVEN:
							width >>= 1;
							if (width & 1) {
								dst[0] = tbl[src[0]];
								src += 2;
								dst += 2;
							}
							width >>= 1;
							for (; width; width--) {
								dst[0] = tbl[src[0]];
								dst[2] = tbl[src[2]];
								src += 4;
								dst += 4;
							}
						}
					} else {
						if (!(width & 1)) {
							goto L_EVEN;
						} else {
							dst[0] = tbl[src[0]];
							src++;
							dst++;
						L_ODD:
							width >>= 1;
							if (width & 1) {
								dst[1] = tbl[src[1]];
								src += 2;
								dst += 2;
							}
							width >>= 1;
							for (; width; width--) {
								dst[1] = tbl[src[1]];
								dst[3] = tbl[src[3]];
								src += 4;
								dst += 4;
							}
						}
					}
				} else {
					src += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}

/**
 * @brief Same as CelBlitLightSafe, with blended transparancy applied
 * @param out The output buffer
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 * @param tbl Palette translation table
 */
static void CelBlitLightBlendedSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *tbl)
{
	int i, w;
	BYTE width;
	BYTE *src, *dst;

	assert(out.begin != NULL);
	assert(pRLEBytes != NULL);

	src = pRLEBytes;
	dst = out.at(sx, sy);
	if (tbl == NULL)
		tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= out.line_width + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < out.end && dst > out.begin) {
					if (width & 1) {
						dst[0] = paletteTransparencyLookup[dst[0]][tbl[src[0]]];
						src++;
						dst++;
					}
					width >>= 1;
					if (width & 1) {
						dst[0] = paletteTransparencyLookup[dst[0]][tbl[src[0]]];
						dst[1] = paletteTransparencyLookup[dst[1]][tbl[src[1]]];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for (; width; width--) {
						dst[0] = paletteTransparencyLookup[dst[0]][tbl[src[0]]];
						dst[1] = paletteTransparencyLookup[dst[1]][tbl[src[1]]];
						dst[2] = paletteTransparencyLookup[dst[2]][tbl[src[2]]];
						dst[3] = paletteTransparencyLookup[dst[3]][tbl[src[3]]];
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
				i -= width;
			}
		}
	}
}

void CelClippedBlitLightTransTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	BYTE *pRLEBytes;

	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	if (cel_transparency_active) {
		if (sgOptions.bBlendedTransparancy)
			CelBlitLightBlendedSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth, NULL);
		else
			CelBlitLightTransSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth);
	} else if (light_table_index)
		CelBlitLightSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth, NULL);
	else
		CelBlitSafeTo(out, sx, sy, pRLEBytes, nDataSize, nWidth);
}

void CelDrawLightRedSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light)
{
	int nDataSize, w, idx;
	BYTE *pRLEBytes, *dst, *tbl;

	assert(out.begin != NULL);
	assert(pCelBuff != NULL);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	dst = out.at(sx, sy);

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256; // gray colors
	if (light >= 4)
		idx += (light - 1) << 8;

	tbl = &pLightTbl[idx];

	BYTE width;
	BYTE *end;

	end = &pRLEBytes[nDataSize];

	for (; pRLEBytes != end; dst -= out.line_width + nWidth) {
		for (w = nWidth; w;) {
			width = *pRLEBytes++;
			if (!(width & 0x80)) {
				w -= width;
				if (dst < out.end && dst > out.begin) {
					while (width) {
						*dst = tbl[*pRLEBytes];
						pRLEBytes++;
						dst++;
						width--;
					}
				} else {
					pRLEBytes += width;
					dst += width;
				}
			} else {
				width = -(char)width;
				dst += width;
				w -= width;
			}
		}
	}
}

void CelDrawUnsafeTo(CelOutputBuffer out, int x, int y, BYTE *pCelBuff, int nCel, int nWidth)
{
	BYTE *pRLEBytes, *dst, *end;

	assert(pCelBuff != NULL);
	assert(out.begin != NULL);

	int i, nDataSize;
	BYTE width;

	pRLEBytes = CelGetFrame(pCelBuff, nCel, &nDataSize);
	end = &pRLEBytes[nDataSize];
	dst = out.at(x, y);

	for (; pRLEBytes != end; dst -= out.line_width + nWidth) {
		for (i = nWidth; i;) {
			width = *pRLEBytes++;
			if (!(width & 0x80)) {
				i -= width;
				memcpy(dst, pRLEBytes, width);
				dst += width;
				pRLEBytes += width;
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
}

void CelBlitOutlineTo(CelOutputBuffer out, BYTE col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	int nDataSize, w;
	BYTE *src, *dst, *end;
	BYTE width;

	assert(pCelBuff != NULL);
	assert(out.begin != NULL);

	src = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);
	end = &src[nDataSize];
	dst = out.at(sx, sy);

	for (; src != end; dst -= out.line_width + nWidth) {
		for (w = nWidth; w;) {
			width = *src++;
			if (!(width & 0x80)) {
				w -= width;
				if (dst < out.end && dst > out.begin) {
					if (dst >= out.end - out.line_width) {
						while (width) {
							if (*src++) {
								dst[-out.line_width] = col;
								dst[-1] = col;
								dst[1] = col;
							}
							dst++;
							width--;
						}
					} else {
						while (width) {
							if (*src++) {
								dst[-out.line_width] = col;
								dst[-1] = col;
								dst[1] = col;
								dst[out.line_width] = col;
							}
							dst++;
							width--;
						}
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
}

void SetPixel(CelOutputBuffer out, int sx, int sy, BYTE col)
{
	assert(out.begin != NULL);
	if (!out.in_bounds(sx, sy))
		return;
	*out.at(sx, sy) = col;
}

void DrawLineTo(CelOutputBuffer out, int x0, int y0, int x1, int y1, BYTE color_index)
{
	int i, dx, dy, steps;
	float ix, iy, sx, sy;

	dx = x1 - x0;
	dy = y1 - y0;
	steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
	ix = dx / (float)steps;
	iy = dy / (float)steps;
	sx = x0;
	sy = y0;

	for (i = 0; i <= steps; i++, sx += ix, sy += iy) {
		SetPixel(out, sx, sy, color_index);
	}
}

static void DrawHalfTransparentBlendedRectTo(CelOutputBuffer out, int sx, int sy, int width, int height)
{
	BYTE *pix = out.at(sx, sy);
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			*pix = paletteTransparencyLookup[0][*pix];
			pix++;
		}
		pix += out.line_width - width;
	}
}

static void DrawHalfTransparentStippledRectTo(CelOutputBuffer out, int sx, int sy, int width, int height)
{
	BYTE *pix = out.at(sx, sy);
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			if ((row & 1 && col & 1) || (!(row & 1) && !(col & 1)))
				*pix = 0;
			pix++;
		}
		pix += out.line_width - width;
	}
}

void DrawHalfTransparentRectTo(CelOutputBuffer out, int sx, int sy, int width, int height)
{
	if (sgOptions.bBlendedTransparancy) {
		DrawHalfTransparentBlendedRectTo(out, sx, sy, width, height);
	} else {
		DrawHalfTransparentStippledRectTo(out, sx, sy, width, height);
	}
}

int GetDirection(int x1, int y1, int x2, int y2)
{
	int mx, my;
	int md, ny;

	mx = x2 - x1;
	my = y2 - y1;

	if (mx >= 0) {
		if (my >= 0) {
			md = DIR_S;
			if (2 * mx < my)
				md = DIR_SW;
		} else {
			my = -my;
			md = DIR_E;
			if (2 * mx < my)
				md = DIR_NE;
		}
		if (2 * my < mx)
			return DIR_SE;
	} else {
		if (my >= 0) {
			ny = -mx;
			md = DIR_W;
			if (2 * ny < my)
				md = DIR_SW;
		} else {
			ny = -mx;
			my = -my;
			md = DIR_N;
			if (2 * ny < my)
				md = DIR_NE;
		}
		if (2 * my < ny)
			return DIR_NW;
	}

	return md;
}

/**
 * @brief Set the RNG seed
 * @param s RNG seed
 */
void SetRndSeed(int s)
{
	sglGameSeed = s;
	orgseed = s;
}

/**
 * @brief Advance the internal RNG seed and return the new value
 * @return RNG seed
 */
int AdvanceRndSeed()
{
	sglGameSeed = static_cast<unsigned int>(RndMult) * sglGameSeed + RndInc;
	return abs(sglGameSeed);
}

/**
 * @brief Get the current RNG seed
 * @return RNG seed
 */
int GetRndSeed()
{
	return abs(sglGameSeed);
}

/**
 * @brief Main RNG function
 * @param idx Unused
 * @param v The upper limit for the return value
 * @return A random number from 0 to (v-1)
 */
int random_(BYTE idx, int v)
{
	if (v <= 0)
		return 0;
	if (v < 0xFFFF)
		return (AdvanceRndSeed() >> 16) % v;
	return AdvanceRndSeed() % v;
}

/**
 * @brief Multithreaded safe malloc
 * @param dwBytes Byte size to allocate
 */
BYTE *DiabloAllocPtr(DWORD dwBytes)
{
	BYTE *buf;

	sgMemCrit.Enter();
	buf = (BYTE *)SMemAlloc(dwBytes, __FILE__, __LINE__, 0);
	sgMemCrit.Leave();

	if (buf == NULL) {
		const char *text = "System memory exhausted.\n"
		                   "Make sure you have at least 64MB of free system memory before running the game";
		ERR_DLG("Out of Memory Error", text);
	}

	return buf;
}

/**
 * @brief Multithreaded safe memfree
 * @param p Memory pointer to free
 */
void mem_free_dbg(void *p)
{
	if (p) {
		sgMemCrit.Enter();
		SMemFree(p, __FILE__, __LINE__, 0);
		sgMemCrit.Leave();
	}
}

/**
 * @brief Load a file in to a buffer
 * @param pszName Path of file
 * @param pdwFileLen Will be set to file size if non-NULL
 * @return Buffer with content of file
 */
BYTE *LoadFileInMem(const char *pszName, DWORD *pdwFileLen)
{
	HANDLE file;
	BYTE *buf;
	int fileLen;

	SFileOpenFile(pszName, &file);
	fileLen = SFileGetFileSize(file, NULL);

	if (pdwFileLen)
		*pdwFileLen = fileLen;

	if (!fileLen)
		app_fatal("Zero length SFILE:\n%s", pszName);

	buf = (BYTE *)DiabloAllocPtr(fileLen);

	SFileReadFile(file, buf, fileLen, NULL, NULL);
	SFileCloseFile(file);

	return buf;
}

/**
 * @brief Load a file in to the given buffer
 * @param pszName Path of file
 * @param p Target buffer
 * @return Size of file
 */
DWORD LoadFileWithMem(const char *pszName, BYTE *p)
{
	DWORD dwFileLen;
	HANDLE hsFile;

	assert(pszName);
	if (p == NULL) {
		app_fatal("LoadFileWithMem(NULL):\n%s", pszName);
	}

	SFileOpenFile(pszName, &hsFile);

	dwFileLen = SFileGetFileSize(hsFile, NULL);
	if (dwFileLen == 0) {
		app_fatal("Zero length SFILE:\n%s", pszName);
	}

	SFileReadFile(hsFile, p, dwFileLen, NULL, NULL);
	SFileCloseFile(hsFile);

	return dwFileLen;
}

/**
 * @brief Apply the color swaps to a CL2 sprite
 * @param p CL2 buffer
 * @param ttbl Palette translation table
 * @param nCel Frame number in CL2 file
 */
void Cl2ApplyTrans(BYTE *p, BYTE *ttbl, int nCel)
{
	int i, nDataSize;
	char width;
	BYTE *dst;

	assert(p != NULL);
	assert(ttbl != NULL);

	for (i = 1; i <= nCel; i++) {
		dst = CelGetFrame(p, i, &nDataSize) + 10;
		nDataSize -= 10;
		while (nDataSize) {
			width = *dst++;
			nDataSize--;
			assert(nDataSize >= 0);
			if (width < 0) {
				width = -width;
				if (width > 65) {
					nDataSize--;
					assert(nDataSize >= 0);
					*dst = ttbl[*dst];
					dst++;
				} else {
					nDataSize -= width;
					assert(nDataSize >= 0);
					while (width--) {
						*dst = ttbl[*dst];
						dst++;
					}
				}
			}
		}
	}
}

/**
 * @brief Blit CL2 sprite to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 */
static void Cl2BlitSafe(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = out.at(sx, sy);
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = *src++;
				if (dst < out.end && dst > out.begin) {
					w -= width;
					while (width) {
						*dst = fill;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= out.line_width + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < out.end && dst > out.begin) {
					w -= width;
					while (width) {
						*dst = *src;
						src++;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= out.line_width + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = nWidth;
				dst -= out.line_width + w;
			}
		}
	}
}

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth Width of sprite
 * @param col Color index from current palette
 */
static void Cl2BlitOutlineSafe(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE col)
{
	int w;
	char width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = out.at(sx, sy);
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				if (*src++ && dst < out.end && dst > out.begin) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while (width) {
						dst[-out.line_width] = col;
						dst[out.line_width] = col;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= out.line_width + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < out.end && dst > out.begin) {
					w -= width;
					while (width) {
						if (*src++) {
							dst[-1] = col;
							dst[1] = col;
							dst[-out.line_width] = col;
							// BUGFIX: only set `if (dst+out.line_width < out.end)`
							dst[out.line_width] = col;
						}
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= out.line_width + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (!w) {
				w = nWidth;
				dst -= out.line_width + w;
			}
		}
	}
}

/**
 * @brief Blit CL2 sprite, and apply lighting, to the given buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CL2 pixel stream (run-length encoded)
 * @param nDataSize Size of CL2 in bytes
 * @param nWidth With of CL2 sprite
 * @param pTable Light color table
 */
static void Cl2BlitLightSafe(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable)
{
	int w, spriteWidth;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = out.at(sx, sy);
	w = nWidth;
	spriteWidth = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = pTable[*src++];
				if (dst < out.end && dst > out.begin) {
					w -= width;
					while (width) {
						*dst = fill;
						dst++;
						width--;
					}
					if (w == 0) {
						w = spriteWidth;
						dst -= out.line_width + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < out.end && dst > out.begin) {
					w -= width;
					while (width) {
						*dst = pTable[*src];
						src++;
						dst++;
						width--;
					}
					if (w == 0) {
						w = spriteWidth;
						dst -= out.line_width + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while (width) {
			if (width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if (w == 0) {
				w = spriteWidth;
				dst -= out.line_width + w;
			}
		}
	}
}

void Cl2Draw(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	BYTE *pRLEBytes;
	int nDataSize;

	assert(gpBuffer != NULL);
	assert(pCelBuff != NULL);
	assert(nCel > 0);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	Cl2BlitSafe(GlobalBackBuffer(), sx, sy, pRLEBytes, nDataSize, nWidth);
}
/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the back buffer at the given coordianates
 * @param col Color index from current palette
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2DrawOutline(BYTE col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	BYTE *pRLEBytes;

	assert(gpBuffer != NULL);
	assert(pCelBuff != NULL);
	assert(nCel > 0);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	CelOutputBuffer out = GlobalBackBuffer();
	out.end -= out.line_width;
	Cl2BlitOutlineSafe(out, sx, sy, pRLEBytes, nDataSize, nWidth, col);
}

void Cl2DrawLightTbl(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light)
{
	int nDataSize, idx;
	BYTE *pRLEBytes;

	assert(gpBuffer != NULL);
	assert(pCelBuff != NULL);
	assert(nCel > 0);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256; // gray colors
	if (light >= 4)
		idx += (light - 1) << 8;

	Cl2BlitLightSafe(GlobalBackBuffer(), sx, sy, pRLEBytes, nDataSize, nWidth, &pLightTbl[idx]);
}

void Cl2DrawLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	BYTE *pRLEBytes;

	assert(gpBuffer != NULL);
	assert(pCelBuff != NULL);
	assert(nCel > 0);

	pRLEBytes = CelGetFrameClipped(pCelBuff, nCel, &nDataSize);

	if (light_table_index)
		Cl2BlitLightSafe(GlobalBackBuffer(), sx, sy, pRLEBytes, nDataSize, nWidth, &pLightTbl[light_table_index * 256]);
	else
		Cl2BlitSafe(GlobalBackBuffer(), sx, sy, pRLEBytes, nDataSize, nWidth);
}

/**
 * @brief Fade to black and play a video
 * @param pszMovie file path of movie
 */
void PlayInGameMovie(const char *pszMovie)
{
	PaletteFadeOut(8);
	play_movie(pszMovie, FALSE);
	ClearScreenBuffer();
	force_redraw = 255;
	scrollrt_draw_game_screen(TRUE);
	PaletteFadeIn(8);
	force_redraw = 255;
}

DEVILUTION_END_NAMESPACE
