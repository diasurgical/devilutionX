//HEADER_GOES_HERE
#ifndef __ENGINE_H__
#define __ENGINE_H__

//offset 0
//pCelBuff->pFrameTable[0]

extern char gbPixelCol; // automap pixel color 8-bit (palette entry)
extern BOOL gbRotateMap; // flip - if y < x
extern int orgseed;
extern int SeedCount;
extern BOOL gbNotInView; // valid - if x/y are in bounds

inline BYTE *CelGetFrameStart(BYTE *pCelBuff, int nCel)
{
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pCelBuff;

	return pCelBuff + SwapLE32(pFrameTable[nCel]);
}

inline int CelGetFrameSize(BYTE *pCelBuff, int nCel)
{
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pCelBuff;

	return SwapLE32(pFrameTable[nCel + 1]) - SwapLE32(pFrameTable[nCel]);
}

inline BYTE *CelGetFrame(BYTE *pCelBuff, int nCel, int *nDataSize)
{
	DWORD *pFrameTable;
	DWORD nCellStart;

	pFrameTable = (DWORD *)pCelBuff;
	nCellStart = SwapLE32(pFrameTable[nCel]);
	*nDataSize = SwapLE32(pFrameTable[nCel + 1]) - nCellStart;

	return pCelBuff + nCellStart;
}

inline BYTE *CelGetFrameClipped(BYTE *pCelBuff, int nCel, int CelSkip, int CelCap, int *nDataSize)
{
	BYTE *pRLEBytes;
	DWORD *pFrameTable;
	int nDataStart, nCellStart, nDataCap;

	pFrameTable = (DWORD *)pCelBuff;

	nCellStart = SwapLE32(pFrameTable[nCel]);

	const int nCellEnd = SwapLE32(pFrameTable[nCel + 1]);
	if (CelSkip + 1 >= nCellEnd)
		return nullptr;

	pRLEBytes = &pCelBuff[nCellStart];

	const auto read_as_word = [pRLEBytes](int i) -> WORD {
#ifdef PLATFORM_LITTLE_ENDIAN
		return (pRLEBytes[i] << 8) | pRLEBytes[i + 1];
#else
		return pRLEBytes[i] | (pRLEBytes[i + 1] << 8);
#endif
	};

	nDataStart = read_as_word(CelSkip);
	if (nDataStart == 0)
		return NULL;

	*nDataSize = nCellEnd - nCellStart;
	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = read_as_word(CelCap);

	if (nDataCap)
		*nDataSize = nDataCap - nDataStart;
	else
		*nDataSize -= nDataStart;

	return pRLEBytes + nDataStart;
}

void CelDraw(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
void CelBlitFrame(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth);
void CelClippedDraw(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void CelClippedBlit(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void CelDrawLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, BYTE *tbl = NULL);
void CelClippedDrawLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void CelClippedBlitLightTrans(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void CelDrawLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light);
void CelBlitSafe(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void CelClippedDrawSafe(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void CelBlitLightSafe(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *tbl = NULL);
void CelBlitLightTransSafe(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void CelDrawLightRedSafe(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light);
void CelBlitWidth(BYTE *pBuff, int always_0, int hgt, int wdt, BYTE *pCelBuff, int nCel, int nWidth);
void CelBlitOutline(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void CelBlitOutlineSafe(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void ENG_set_pixel(int sx, int sy, BYTE col);
void engine_draw_pixel(int sx, int sy);
void DrawLine(int x0, int y0, int x1, int y1, BYTE col);
int GetDirection(int x1, int y1, int x2, int y2);
void SetRndSeed(int s);
int GetRndSeed();
int random(BYTE idx, int v);
void engine_debug_trap(BOOL show_cursor);
BYTE *DiabloAllocPtr(DWORD dwBytes);
void mem_free_dbg(void *p);
BYTE *LoadFileInMem(char *pszName, DWORD *pdwFileLen);
DWORD LoadFileWithMem(const char *pszName, void *p);
void Cl2ApplyTrans(BYTE *p, BYTE *ttbl, int nCel);
void Cl2Draw(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void Cl2DrawOutline(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void Cl2DrawLightTbl(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light);
void Cl2DrawLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip = 0, int CelCap = 8);
void Cl2BlitSafe(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void Cl2BlitOutlineSafe(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col);
void Cl2BlitLightSafe(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable);
void PlayInGameMovie(char *pszMovie);

/* rdata */

extern const int RndInc;
extern const int RndMult;

#endif /* __ENGINE_H__ */
