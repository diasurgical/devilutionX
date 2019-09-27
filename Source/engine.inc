__FORCEINLINE BYTE *CelGetFrameStart(BYTE *pCelBuff, int nCel)
{
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pCelBuff;

	return pCelBuff + SwapLE32(pFrameTable[nCel]);
}

__FORCEINLINE int CelGetFrameSize(BYTE *pCelBuff, int nCel)
{
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pCelBuff;

	return SwapLE32(pFrameTable[nCel + 1]) - SwapLE32(pFrameTable[nCel]);
}

__FORCEINLINE BYTE *CelGetFrame(BYTE *pCelBuff, int nCel, int *nDataSize)
{
	DWORD *pFrameTable;
	DWORD nCellStart;

	pFrameTable = (DWORD *)pCelBuff;
	nCellStart = SwapLE32(pFrameTable[nCel]);
	*nDataSize = SwapLE32(pFrameTable[nCel + 1]) - nCellStart;

	return pCelBuff + nCellStart;
}
