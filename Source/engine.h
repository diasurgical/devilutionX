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
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "../types.h"

#ifdef __cplusplus
#include <algorithm>
#endif

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

inline BYTE *CelGetFrameStart(BYTE *pCelBuff, int nCel)
{
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pCelBuff;

	return pCelBuff + SwapLE32(pFrameTable[nCel]);
}

#define LOAD_LE32(b) (((DWORD)(b)[3] << 24) | ((DWORD)(b)[2] << 16) | ((DWORD)(b)[1] << 8) | (DWORD)(b)[0])
#define LOAD_BE32(b) (((DWORD)(b)[0] << 24) | ((DWORD)(b)[1] << 16) | ((DWORD)(b)[2] << 8) | (DWORD)(b)[3])
inline BYTE *CelGetFrame(BYTE *pCelBuff, int nCel, int *nDataSize)
{
	DWORD nCellStart;

	nCellStart = LOAD_LE32(&pCelBuff[nCel * 4]);
	*nDataSize = LOAD_LE32(&pCelBuff[(nCel + 1) * 4]) - nCellStart;
	return pCelBuff + nCellStart;
}

inline BYTE *CelGetFrameClipped(BYTE *pCelBuff, int nCel, int *nDataSize)
{
	DWORD nDataStart;
	BYTE *pRLEBytes = CelGetFrame(pCelBuff, nCel, nDataSize);

	nDataStart = pRLEBytes[1] << 8 | pRLEBytes[0];
	*nDataSize -= nDataStart;

	return pRLEBytes + nDataStart;
}

struct CelOutputBuffer {
	BYTE *begin;
	BYTE *end;
	int line_width;

#ifdef __cplusplus
	BYTE *at(int x, int y) const
	{
		return &begin[x + line_width * y];
	}

	bool in_bounds(int x, int y) const
	{
		return x >= 0 && y >= 0 && begin + x + line_width * y < end;
	}

	CelOutputBuffer subregion(int x0, int y0, int x1, int y1) const
	{
		return CelOutputBuffer { at(x0, y0), std::min(at(x1, y1), end), line_width };
	}
#endif
};

inline CelOutputBuffer GlobalBackBuffer()
{
	extern BYTE *gpBuffer;
	extern BYTE *gpBufEnd;
	return CelOutputBuffer { gpBuffer, gpBufEnd, BUFFER_WIDTH };
}

/**
 * @brief Blit CEL sprite to the back buffer at the given coordinates
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelDrawTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
inline void CelDraw(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	CelDrawTo(GlobalBackBuffer(), sx, sy, pCelBuff, nCel, nWidth);
}

/**
 * @briefBlit CEL sprite to the given buffer, does not perform bounds-checking.
 * @param out Target buffer
 * @param x Cordinate in the target buffer
 * @param y Cordinate in the target buffer
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of cel
 */
void CelDrawUnsafeTo(CelOutputBuffer out, int x, int y, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Same as CelDrawTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedDrawTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
inline void CelClippedDraw(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	CelClippedDrawTo(GlobalBackBuffer(), sx, sy, pCelBuff, nCel, nWidth);
}

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelDrawLightTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, BYTE *tbl);
inline void CelDrawLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, BYTE *tbl)
{
	CelDrawLightTo(GlobalBackBuffer(), sx, sy, pCelBuff, nCel, nWidth, tbl);
}

/**
 * @brief Same as CelDrawLightTo but with the option to skip parts of the top and bottom of the sprite
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedDrawLightTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
inline void CelClippedDrawLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	return CelClippedDrawLightTo(GlobalBackBuffer(), sx, sy, pCelBuff, nCel, nWidth);
}

/**
 * @brief Same as CelBlitLightTransSafeTo
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedBlitLightTransTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
inline void CelClippedBlitLightTrans(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	return CelClippedBlitLightTransTo(GlobalBackBuffer(), sx, sy, pCelBuff, nCel, nWidth);
}

/**
 * @brief Blit CEL sprite, and apply lighting, to the back buffer at the given coordinates, translated to a red hue
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 * @param light Light shade to use
 */
void CelDrawLightRedTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);
inline void CelDrawLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light)
{
	return CelDrawLightRedTo(GlobalBackBuffer(), sx, sy, pCelBuff, nCel, nWidth, light);
}

/**
 * @brief Blit CEL sprite to the given buffer, checks for drawing outside the buffer.
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 */
void CelBlitSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth);
inline void CelBlitSafe(int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	return CelBlitSafeTo(GlobalBackBuffer(), sx, sy, pRLEBytes, nDataSize, nWidth);
}

/**
 * @brief Same as CelClippedDrawTo but checks for drawing outside the buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelClippedDrawSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
inline void CelClippedDrawSafe(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	return CelClippedDrawSafeTo(GlobalBackBuffer(), sx, sy, pCelBuff, nCel, nWidth);
}

/**
 * @brief Blit CEL sprite, and apply lighting, to the given buffer, checks for drawing outside the buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 * @param tbl Palette translation table
 */
void CelBlitLightSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *tbl);
inline void CelBlitLightSafe(int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *tbl)
{
	return CelBlitLightSafeTo(GlobalBackBuffer(), sx, sy, pRLEBytes, nDataSize, nWidth, tbl);
}

/**
 * @brief Same as CelBlitLightSafeTo but with stippled transparancy applied
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pRLEBytes CEL pixel stream (run-length encoded)
 * @param nDataSize Size of CEL in bytes
 * @param nWidth Width of sprite
 */
void CelBlitLightTransSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth);
inline void CelBlitLightTransSafe(int sx, int sy, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	return CelBlitLightTransSafeTo(GlobalBackBuffer(), sx, sy, pRLEBytes, nDataSize, nWidth);
}

/**
 * @brief Same as CelDrawLightRedTo but checks for drawing outside the buffer
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff Cel data
 * @param nCel CEL frame number
 * @param nWidth Width of cel
 * @param light Light shade to use
 */
void CelDrawLightRedSafeTo(CelOutputBuffer out, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);
inline void CelDrawLightRedSafe(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light)
{
	return CelDrawLightRedSafeTo(GlobalBackBuffer(), sx, sy, pCelBuff, nCel, nWidth, light);
}

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the target buffer at the given coordianates
 * @param out Target buffer
 * @param col Color index from current palette
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param pCelBuff CEL buffer
 * @param nCel CEL frame number
 * @param nWidth Width of sprite
 */
void CelBlitOutlineTo(CelOutputBuffer out, BYTE col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
inline void CelBlitOutline(BYTE col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	return CelBlitOutlineTo(GlobalBackBuffer(), col, sx, sy, pCelBuff, nCel, nWidth);
}

/**
 * @brief Set the value of a single pixel in the back buffer, checks bounds
 * @param out Target buffer
 * @param sx Target buffer coordinate
 * @param sy Target buffer coordinate
 * @param col Color index from current palette
 */
void SetPixel(CelOutputBuffer out, int sx, int sy, BYTE col);

/**
 * @brief Blit CL2 sprite, to the back buffer at the given coordianates
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2Draw(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Blit a solid colder shape one pixel larger then the given sprite shape, to the back buffer at the given coordianates
 * @param col Color index from current palette
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2DrawOutline(BYTE col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Blit CL2 sprite, and apply a given lighting, to the back buffer at the given coordianates
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 * @param light Light shade to use
 */
void Cl2DrawLightTbl(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, char light);

/**
 * @brief Blit CL2 sprite, and apply lighting, to the back buffer at the given coordinates
 * @param sx Back buffer coordinate
 * @param sy Back buffer coordinate
 * @param pCelBuff CL2 buffer
 * @param nCel CL2 frame number
 * @param nWidth Width of sprite
 */
void Cl2DrawLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);

/**
 * @brief Draw a line in the target buffer
 * @param out Target buffer
 * @param x0 Back buffer coordinate
 * @param y0 Back buffer coordinate
 * @param x1 Back buffer coordinate
 * @param y1 Back buffer coordinate
 * @param color_index Color index from current palette
 */
void DrawLineTo(CelOutputBuffer out, int x0, int y0, int x1, int y1, BYTE color_index);


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
void DrawHalfTransparentRectTo(CelOutputBuffer out, int sx, int sy, int width, int height);

/**
 * @brief Calculate the best fit direction between two points
 * @param x1 Tile coordinate
 * @param y1 Tile coordinate
 * @param x2 Tile coordinate
 * @param y2 Tile coordinate
 * @return A value from the direction enum
 */
int GetDirection(int x1, int y1, int x2, int y2);

void SetRndSeed(int s);
int AdvanceRndSeed();
int GetRndSeed();
int random_(BYTE idx, int v);
BYTE *DiabloAllocPtr(DWORD dwBytes);
void mem_free_dbg(void *p);
BYTE *LoadFileInMem(const char *pszName, DWORD *pdwFileLen);
DWORD LoadFileWithMem(const char *pszName, BYTE *p);
void Cl2ApplyTrans(BYTE *p, BYTE *ttbl, int nCel);
void PlayInGameMovie(const char *pszMovie);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __ENGINE_H__ */
