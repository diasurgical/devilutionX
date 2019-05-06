#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef USE_ASM
#pragma warning(disable : 4731) // frame pointer register 'ebp' modified by inline assembly code
#endif

char gbPixelCol;  // automap pixel color 8-bit (palette entry)
int gbRotateMap; // BOOLEAN flip - if y < x
int orgseed;      // weak
int sgnWidth;
int sglGameSeed; // weak
#ifdef __cplusplus
static CCritSect sgMemCrit;
#endif
int SeedCount;    // weak
int gbNotInView; // BOOLEAN valid - if x/y are in bounds

const int rand_increment = 1;
const int rand_multiplier = 0x015A4E35;

void CelDrawDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;

#ifdef USE_ASM
	__asm {
		mov		esi, pRLEBytes
		mov		edi, pDecodeTo
		mov		eax, 768
		add		eax, nWidth
		mov		w, eax
		mov		ebx, nDataSize
		add		ebx, esi
	label1:
		mov		edx, nWidth
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label6
		sub		edx, eax
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
	}
#else
	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= 768 + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (width & 1) {
					dst[0] = src[0];
					src++;
					dst++;
				}
				width >>= 1;
				if (width & 1) {
					dst[0] = src[0];
					dst[1] = src[1];
					src += 2;
					dst += 2;
				}
				width >>= 1;
				for (; width; width--) {
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
				i -= width;
			}
		}
	}
#endif
}

void CelDecodeOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	CelDrawDatOnly(
	    &gpBuffer[sx + PitchTbl[sy]],
	    &pCelBuff[pFrameTable[nCel]],
	    pFrameTable[nCel + 1] - pFrameTable[nCel],
	    nWidth);
}

void CelDecDatOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth)
{
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if (!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	CelDrawDatOnly(
	    pBuff,
	    &pCelBuff[pFrameTable[nCel]],
	    pFrameTable[nCel + 1] - pFrameTable[nCel],
	    nWidth);
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataCap, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	CelDrawDatOnly(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize,
	    nWidth);
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDecodeHdrOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataCap, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if (!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	CelDrawDatOnly(pBuff, pRLEBytes + nDataStart, nDataSize, nWidth);
}

void CelDecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;

#ifdef USE_ASM
	__asm {
		mov		eax, light_table_index
		shl		eax, 8
		add		eax, pLightTbl
		mov		tbl, eax
		mov		esi, pRLEBytes
		mov		edi, pDecodeTo
		mov		eax, 768
		add		eax, nWidth
		mov		w, eax
		mov		ebx, nDataSize
		add		ebx, esi
	label1:
		mov		edx, nWidth
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label3
		push	ebx
		mov		ebx, tbl
		sub		edx, eax
		mov		ecx, eax
		push	edx
		call	CelDecDatLightEntry
		pop		edx
		pop		ebx
		or		edx, edx
		jnz		label2
		jmp		label4
	label3:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label4:
		sub		edi, w
		cmp		ebx, esi
		jnz		label1
		jmp		labexit
	}

	/* Assembly Macro */
	// clang-format off
	__asm {
	CelDecDatLightEntry:
		shr		cl, 1
		jnb		label5
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		mov		[edi], dl
		add		esi, 1
		add		edi, 1
	label5:
		shr		cl, 1
		jnb		label6
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	label6:
		test	cl, cl
		jz		labret
	label7:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		dec		cl
		jnz		label7
	labret:
		retn
	}
	// clang-format on

	__asm {
	labexit:
	}
#else
	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= 768 + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
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
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
#endif
}
// 69BEF8: using guessed type int light_table_index;

void CelDecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BOOL shift;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;

#ifdef USE_ASM
	__asm {
		mov		eax, light_table_index
		shl		eax, 8
		add		eax, pLightTbl
		mov		tbl, eax
		mov		esi, pRLEBytes
		mov		edi, pDecodeTo
		mov		eax, 768
		add		eax, nWidth
		mov		w, eax
		mov		ebx, nDataSize
		add		ebx, esi
		mov		eax, edi
		and		eax, 1
		mov		shift, eax
	label1:
		mov		edx, nWidth
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label9
		push	ebx
		mov		ebx, tbl
		sub		edx, eax
		mov		ecx, eax
		mov		eax, edi
		and		eax, 1
		cmp		eax, shift
		jnz		label5
		shr		ecx, 1
		jnb		label3
		inc		esi
		inc		edi
		jecxz	label8
		jmp		label6
	label3:
		shr		ecx, 1
		jnb		label4
		inc		esi
		inc		edi
		lodsb
		xlat
		stosb
		jecxz	label8
	label4:
		lodsd
		inc		edi
		ror		eax, 8
		xlat
		stosb
		ror		eax, 10h
		inc		edi
		xlat
		stosb
		loop	label4
		jmp		label8
	label5:
		shr		ecx, 1
		jnb		label6
		lodsb
		xlat
		stosb
		jecxz	label8
		jmp		label3
	label6:
		shr		ecx, 1
		jnb		label7
		lodsb
		xlat
		stosb
		inc		esi
		inc		edi
		jecxz	label8
	label7:
		lodsd
		xlat
		stosb
		inc		edi
		ror		eax, 10h
		xlat
		stosb
		inc		edi
		loop	label7
	label8:
		pop		ebx
		or		edx, edx
		jz		label10
		jmp		label2
	label9:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label10:
		sub		edi, w
		mov		eax, shift
		inc		eax
		and		eax, 1
		mov		shift, eax
		cmp		ebx, esi
		jnz		label1
	}
#else
	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;
	shift = (BYTE)dst & 1;

	for (; src != &pRLEBytes[nDataSize]; dst -= 768 + w, shift = (shift + 1) & 1) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (((BYTE)dst & 1) == shift) {
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
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
#endif
}
// 69BEF8: using guessed type int light_table_index;

void CelDecodeLightOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	BYTE *pDecodeTo, *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy]];

	if (light_table_index)
		CelDecDatLightOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
	else
		CelDrawDatOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDecodeHdrLightOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataCap, nDataSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	if (light_table_index)
		CelDecDatLightOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
	else
		CelDrawDatOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDecodeHdrLightTrans(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataCap, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if (!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;

	if (cel_transparency_active)
		CelDecDatLightTrans(pBuff, pRLEBytes, nDataSize, nWidth);
	else if (light_table_index)
		CelDecDatLightOnly(pBuff, pRLEBytes, nDataSize, nWidth);
	else
		CelDrawDatOnly(pBuff, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light)
{
	int w, idx, nDataStart, nDataCap, nDataSize;
	BYTE *src, *dst, *tbl;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	src = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&src[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&src[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	src += nDataStart;
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256;
	if (light >= 4)
		idx += (light - 1) << 8;

	tbl = &pLightTbl[idx];

#ifdef USE_ASM
	__asm {
		mov		esi, src
		mov		edi, dst
		mov		eax, 768
		add		eax, nWidth
		mov		w, eax /* use C for w? w = 768 + nWidth */
		mov		ebx, nDataSize
		add		ebx, esi
	label1:
		mov		edx, nWidth
	label2:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		label4
		push	ebx
		mov		ebx, tbl
		sub		edx, eax
		mov		ecx, eax
	label3:
		mov		al, [esi]
		inc		esi
		mov		al, [ebx+eax]
		mov		[edi], al
		dec		ecx
		lea		edi, [edi+1]
		jnz		label3
		pop		ebx
		test	edx, edx
		jz		label5
		jmp		label2
	label4:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label5:
		sub		edi, w
		cmp		ebx, esi
		jnz		label1
	}
#else
	BYTE width;
	BYTE *end;

	end = &src[nDataSize];

	for (; src != end; dst -= 768 + nWidth) {
		for (w = nWidth; w;) {
			width = *src++;
			if (!(width & 0x80)) {
				w -= width;
				while (width) {
					*dst = tbl[*src];
					src++;
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
#endif
}
// 525728: using guessed type int light4flag;

void Cel2DecDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

#ifdef USE_ASM
	__asm {
		mov		esi, pRLEBytes
		mov		edi, pDecodeTo
		mov		eax, 768
		add		eax, nWidth
		mov		w, eax
		mov		ebx, nDataSize
		add		ebx, esi
	label1:
		mov		edx, nWidth
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
	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= 768 + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < gpBufEnd) {
					if (width & 1) {
						dst[0] = src[0];
						src++;
						dst++;
					}
					width >>= 1;
					if (width & 1) {
						dst[0] = src[0];
						dst[1] = src[1];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for (; width; width--) {
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
				i -= width;
			}
		}
	}
#endif
}
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataCap, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	if (CelCap == 8)
		nDataCap = 0;
	else
		nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	Cel2DecDatOnly(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize,
	    nWidth);
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DecodeHdrOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataCap, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if (!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	Cel2DecDatOnly(pBuff, pRLEBytes + nDataStart, nDataSize, nWidth);
}

void Cel2DecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

#ifdef USE_ASM
	__asm {
		mov		eax, light_table_index
		shl		eax, 8
		add		eax, pLightTbl
		mov		tbl, eax
		mov		esi, pRLEBytes
		mov		edi, pDecodeTo
		mov		eax, 768
		add		eax, nWidth
		mov		w, eax
		mov		ebx, nDataSize
		add		ebx, esi
	label1:
		mov		edx, nWidth
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label5
		push	ebx
		mov		ebx, tbl
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		label3
		add		esi, eax
		add		edi, eax
		jmp		label4
	label3:
		mov		ecx, eax
		push	edx
		call	Cel2DecDatLightEntry
		pop		edx
	label4:
		pop		ebx
		or		edx, edx
		jz		label6
		jmp		label2
	label5:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label6:
		sub		edi, w
		cmp		ebx, esi
		jnz		label1
		jmp		labexit
	}

	/* Assembly Macro */
	// clang-format off
	__asm {
	Cel2DecDatLightEntry:
		shr		cl, 1
		jnb		label7
		mov		dl, [esi]
		mov		dl, [ebx+edx]
		mov		[edi], dl
		add		esi, 1
		add		edi, 1
	label7:
		shr		cl, 1
		jnb		label8
		mov		dl, [esi]
		mov		ch, [ebx+edx]
		mov		[edi], ch
		mov		dl, [esi+1]
		mov		ch, [ebx+edx]
		mov		[edi+1], ch
		add		esi, 2
		add		edi, 2
	label8:
		test	cl, cl
		jz		labret
	label9:
		mov		eax, [esi]
		add		esi, 4
		mov		dl, al
		mov		ch, [ebx+edx]
		mov		dl, ah
		ror		eax, 10h
		mov		[edi], ch
		mov		ch, [ebx+edx]
		mov		dl, al
		mov		[edi+1], ch
		mov		ch, [ebx+edx]
		mov		dl, ah
		mov		[edi+2], ch
		mov		ch, [ebx+edx]
		mov		[edi+3], ch
		add		edi, 4
		dec		cl
		jnz		label9
	labret:
		retn
	}
	// clang-format on

	__asm {
	labexit:
	}
#else
	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;

	for (; src != &pRLEBytes[nDataSize]; dst -= 768 + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < gpBufEnd) {
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
#endif
}
// 69BEF8: using guessed type int light_table_index;
// 69CF0C: using guessed type int gpBufEnd;

void Cel2DecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BOOL shift;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if (!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if (!pRLEBytes)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

#ifdef USE_ASM
	__asm {
		mov		eax, light_table_index
		shl		eax, 8
		add		eax, pLightTbl
		mov		tbl, eax
		mov		esi, pRLEBytes
		mov		edi, pDecodeTo
		mov		eax, 768
		add		eax, nWidth
		mov		w, eax
		mov		ebx, nDataSize
		add		ebx, esi
		mov		eax, edi
		and		eax, 1
		mov		shift, eax
	label1:
		mov		edx, nWidth
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label10
		push	ebx
		mov		ebx, tbl
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		label3
		add		esi, eax
		add		edi, eax
		jmp		label9
	label3:
		mov		ecx, eax
		mov		eax, edi
		and		eax, 1
		cmp		eax, shift
		jnz		label6
		shr		ecx, 1
		jnb		label4
		inc		esi
		inc		edi
		jecxz	label9
		jmp		label7
	label4:
		shr		ecx, 1
		jnb		label5
		inc		esi
		inc		edi
		lodsb
		xlat
		stosb
		jecxz	label9
	label5:
		lodsd
		inc		edi
		ror		eax, 8
		xlat
		stosb
		ror		eax, 10h
		inc		edi
		xlat
		stosb
		loop	label5
		jmp		label9
	label6:
		shr		ecx, 1
		jnb		label7
		lodsb
		xlat
		stosb
		jecxz	label9
		jmp		label4
	label7:
		shr		ecx, 1
		jnb		label8
		lodsb
		xlat
		stosb
		inc		esi
		inc		edi
		jecxz	label9
	label8:
		lodsd
		xlat
		stosb
		inc		edi
		ror		eax, 10h
		xlat
		stosb
		inc		edi
		loop	label8
	label9:
		pop		ebx
		or		edx, edx
		jz		label11
		jmp		label2
	label10:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label11:
		sub		edi, w
		mov		eax, shift
		inc		eax
		and		eax, 1
		mov		shift, eax
		cmp		ebx, esi
		jnz		label1
	}
#else
	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = &pLightTbl[light_table_index * 256];
	w = nWidth;
	shift = (BYTE)dst & 1;

	for (; src != &pRLEBytes[nDataSize]; dst -= 768 + w, shift = (shift + 1) & 1) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst < gpBufEnd) {
					if (((BYTE)dst & 1) == shift) {
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
#endif
}
// 69BEF8: using guessed type int light_table_index;
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DecodeHdrLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataCap, nDataSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;
	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	if (light_table_index)
		Cel2DecDatLightOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
	else
		Cel2DecDatOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DecodeLightTrans(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataCap, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	pRLEBytes += nDataStart;

	if (cel_transparency_active)
		Cel2DecDatLightTrans(pBuff, pRLEBytes, nDataSize, nWidth);
	else if (light_table_index)
		Cel2DecDatLightOnly(pBuff, pRLEBytes, nDataSize, nWidth);
	else
		Cel2DecDatOnly(pBuff, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cel2DrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light)
{
	int w, hdr, idx, nDataSize, v1;
	BYTE *src, *dst, *tbl;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	src = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&src[CelSkip];
	if (!hdr)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	if (CelCap == 8)
		v1 = 0;
	else
		v1 = *(WORD *)&src[CelCap];
	if (v1)
		nDataSize = v1 - hdr;
	else
		nDataSize -= hdr;

	src += hdr;
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256;
	if (light >= 4)
		idx += (light - 1) << 8;

	tbl = &pLightTbl[idx];

#ifdef USE_ASM
	w = 768 + nWidth;

	__asm {
		mov		esi, src
		mov		edi, dst
		mov		ecx, nDataSize
		add		ecx, esi
	label1:
		push	ecx
		mov		edx, nWidth
		xor		ecx, ecx
	label2:
		xor		eax, eax
		mov		al, [esi]
		inc		esi
		test	al, al
		js		label5
		mov		ebx, tbl
		sub		edx, eax
		cmp		edi, gpBufEnd
		jb		label3
		add		esi, eax
		add		edi, eax
		jmp		label4
	label3:
		mov		cl, [esi]
		inc		esi
		mov		cl, [ebx+ecx]
		mov		[edi], cl
		dec		eax
		lea		edi, [edi+1]
		jnz		label3
	label4:
		test	edx, edx
		jz		label6
		jmp		label2
	label5:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label6:
		pop		ecx
		sub		edi, w
		cmp		ecx, esi
		jnz		label1
	}
#else
	BYTE width;
	BYTE *end;

	end = &src[nDataSize];

	for (; src != end; dst -= 768 + nWidth) {
		for (w = nWidth; w;) {
			width = *src++;
			if (!(width & 0x80)) {
				w -= width;
				if (dst < gpBufEnd) {
					while (width) {
						*dst = tbl[*src];
						src++;
						dst++;
						width--;
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
}
// 525728: using guessed type int light4flag;
// 69CF0C: using guessed type int gpBufEnd;

void CelDecodeRect(BYTE *pBuff, int CelSkip, int hgt, int wdt, BYTE *pCelBuff, int nCel, int nWidth)
{
	BYTE *src, *dst, *end;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if (!pBuff)
		return;

#ifdef USE_ASM
	__asm {
		mov		ebx, pCelBuff
		mov		eax, nCel
		shl		eax, 2
		add		ebx, eax
		mov		eax, [ebx+4]
		sub		eax, [ebx]
		mov		end, eax
		mov		eax, pCelBuff
		add		eax, [ebx]
		mov		src, eax
	}

	dst = &pBuff[hgt * wdt + CelSkip];

	__asm {
		mov		esi, src
		mov		edi, dst
		mov		eax, wdt
		add		eax, nWidth
		mov		wdt, eax
		mov		ebx, end
		add		ebx, esi
	label1:
		mov		edx, nWidth
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label6
		sub		edx, eax
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
		sub		edi, wdt
		cmp		ebx, esi
		jnz		label1
	}
#else
	int i;
	BYTE width;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	src = &pCelBuff[pFrameTable[0]];
	end = &src[pFrameTable[1] - pFrameTable[0]];
	dst = &pBuff[hgt * wdt + CelSkip];

	for (; src != end; dst -= wdt + nWidth) {
		for (i = nWidth; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (width & 1) {
					dst[0] = src[0];
					src++;
					dst++;
				}
				width >>= 1;
				if (width & 1) {
					dst[0] = src[0];
					dst[1] = src[1];
					src += 2;
					dst += 2;
				}
				width >>= 1;
				while (width) {
					dst[0] = src[0];
					dst[1] = src[1];
					dst[2] = src[2];
					dst[3] = src[3];
					src += 4;
					dst += 4;
					width--;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
#endif
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDecodeClr(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int w, nDataStart, nDataCap, nDataSize;
	BYTE *src, *dst;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

#ifdef USE_ASM
	__asm {
		mov		ebx, pCelBuff
		mov		eax, nCel
		shl		eax, 2
		add		ebx, eax
		mov		eax, [ebx+4]
		sub		eax, [ebx]
		mov		nDataSize, eax
		mov		edx, pCelBuff
		add		edx, [ebx]
		mov		src, edx
		add		edx, CelSkip
		xor		eax, eax
		mov		ax, [edx]
		mov		nDataStart, eax
		mov		edx, src
		add		edx, CelCap
		mov		ax, [edx]
		mov		nDataCap, eax
	}

	if (!nDataStart) return;

	if (CelCap == 8)
		nDataCap = 0;
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	src += nDataStart;
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	__asm {
		mov		esi, src
		mov		edi, dst
		mov		eax, 768
		add		eax, nWidth
		mov		w, eax
		mov		ebx, nDataSize
		add		ebx, esi
	label1:
		mov		edx, nWidth
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label5
		sub		edx, eax
		mov		ecx, eax
		mov		ah, col
	label3:
		lodsb
		or		al, al
		jz		label4
		mov		[edi-768], ah
		mov		[edi-1], ah
		mov		[edi+1], ah
		mov		[edi+768], ah
	label4:
		inc		edi
		loop	label3
		or		edx, edx
		jz		label6
		jmp		label2
	label5:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label6:
		sub		edi, w
		cmp		ebx, esi
		jnz		label1
	}
#else
	BYTE width;
	BYTE *end, *pRLEBytes;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	pRLEBytes = &pCelBuff[pFrameTable[0]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize = pFrameTable[1] - pFrameTable[0] - nDataStart;

	src = pRLEBytes + nDataStart;
	end = &src[nDataSize];
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	for (; src != end; dst -= 768 + nWidth) {
		for (w = nWidth; w;) {
			width = *src++;
			if (!(width & 0x80)) {
				w -= width;
				while (width) {
					if (*src++) {
						dst[-768] = col;
						dst[-1] = col;
						dst[1] = col;
						dst[768] = col;
					}
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
#endif
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void CelDrawHdrClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int w, nDataStart, nDataCap, nDataSize;
	BYTE *src, *dst;

	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(gpBuffer);
	if (!gpBuffer)
		return;

#ifdef USE_ASM
	__asm {
		mov		ebx, pCelBuff
		mov		eax, nCel
		shl		eax, 2
		add		ebx, eax
		mov		eax, [ebx+4]
		sub		eax, [ebx]
		mov		nDataSize, eax
		mov		edx, pCelBuff
		add		edx, [ebx]
		mov		src, edx
		add		edx, CelSkip
		xor		eax, eax
		mov		ax, [edx]
		mov		nDataStart, eax
		mov		edx, src
		add		edx, CelCap
		mov		ax, [edx]
		mov		nDataCap, eax
	}

	if (!nDataStart) return;

	if (CelCap == 8)
		nDataCap = 0;
	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize -= nDataStart;

	src += nDataStart;
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	__asm {
		mov		esi, src
		mov		edi, dst
		mov		eax, 768
		add		eax, nWidth
		mov		w, eax
		mov		ebx, nDataSize
		add		ebx, esi
	label1:
		mov		edx, nWidth
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label10
		sub		edx, eax
		mov		ecx, gpBufEnd
		cmp		edi, ecx
		jb		label3
		add		esi, eax
		add		edi, eax
		jmp		label9
	label3:
		sub		ecx, 768
		cmp		edi, ecx
		jnb		label6
		mov		ecx, eax
		mov		ah, col
	label4:
		lodsb
		or		al, al
		jz		label5
		mov		[edi-768], ah
		mov		[edi-1], ah
		mov		[edi+1], ah
		mov		[edi+768], ah
	label5:
		inc		edi
		loop	label4
		jmp		label9
	label6:
		mov		ecx, eax
		mov		ah, col
	label7:
		lodsb
		or		al, al
		jz		label8
		mov		[edi-768], ah
		mov		[edi-1], ah
		mov		[edi+1], ah
	label8:
		inc		edi
		loop	label7
	label9:
		or		edx, edx
		jz		label11
		jmp		label2
	label10:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label11:
		sub		edi, w
		cmp		ebx, esi
		jnz		label1
	}
#else
	BYTE width;
	BYTE *end, *pRLEBytes;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	pRLEBytes = &pCelBuff[pFrameTable[0]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	nDataCap = *(WORD *)&pRLEBytes[CelCap];
	if (CelCap == 8)
		nDataCap = 0;

	if (nDataCap)
		nDataSize = nDataCap - nDataStart;
	else
		nDataSize = pFrameTable[1] - pFrameTable[0] - nDataStart;

	src = pRLEBytes + nDataStart;
	end = &src[nDataSize];
	dst = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	for (; src != end; dst -= 768 + nWidth) {
		for (w = nWidth; w;) {
			width = *src++;
			if (!(width & 0x80)) {
				w -= width;
				if (dst < gpBufEnd) {
					if (dst >= gpBufEnd - 768) {
						while (width) {
							if (*src++) {
								dst[-768] = col;
								dst[-1] = col;
								dst[1] = col;
							}
							dst++;
							width--;
						}
					} else {
						while (width) {
							if (*src++) {
								dst[-768] = col;
								dst[-1] = col;
								dst[1] = col;
								dst[768] = col;
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
#endif
}
// 69CF0C: using guessed type int gpBufEnd;

void ENG_set_pixel(int sx, int sy, BYTE col)
{
	BYTE *dst;

	/// ASSERT: assert(gpBuffer);

	if (sy < 0 || sy >= 640 || sx < 64 || sx >= 704)
		return;

	dst = &gpBuffer[sx + PitchTbl[sy]];

#ifdef USE_ASM
	__asm {
		mov		edi, dst
		cmp		edi, gpBufEnd
		jnb		label1
		mov		al, col
		mov		[edi], al
	label1:
	}
#else
	if (dst < gpBufEnd)
		*dst = col;
#endif
}
// 69CF0C: using guessed type int gpBufEnd;

void engine_draw_pixel(int sx, int sy)
{
	BYTE *dst;

	/// ASSERT: assert(gpBuffer);

	if (gbRotateMap) {
		if (gbNotInView && (sx < 0 || sx >= 640 || sy < 64 || sy >= 704))
			return;
		dst = &gpBuffer[sy + PitchTbl[sx]];
	} else {
		if (gbNotInView && (sy < 0 || sy >= 640 || sx < 64 || sx >= 704))
			return;
		dst = &gpBuffer[sx + PitchTbl[sy]];
	}

#ifdef USE_ASM
	__asm {
		mov		edi, dst
		cmp		edi, gpBufEnd
		jnb		label1
		mov		al, gbPixelCol
		mov		[edi], al
	label1:
	}
#else
	if (dst < gpBufEnd)
		*dst = gbPixelCol;
#endif
}
// 52B96C: using guessed type char gbPixelCol;
// 52B970: using guessed type int gbRotateMap;
// 52B99C: using guessed type int gbNotInView;
// 69CF0C: using guessed type int gpBufEnd;

/*
 * Xiaolin Wu's anti-aliased line algorithm
 */
void DrawLine(int x0, int y0, int x1, int y1, BYTE col)
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

	for(i = 0; i <= steps; i++, sx += ix, sy += iy) {
		ENG_set_pixel(sx, sy, col);
	}
}
// 52B96C: using guessed type char gbPixelCol;
// 52B970: using guessed type int gbRotateMap;
// 52B99C: using guessed type int gbNotInView;

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
		ny = -mx;
		if (my >= 0) {
			md = DIR_W;
			if (2 * ny < my)
				md = DIR_SW;
		} else {
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

void SetRndSeed(int s)
{
	SeedCount = 0;
	sglGameSeed = s;
	orgseed = s;
}
// 52B974: using guessed type int orgseed;
// 52B97C: using guessed type int sglGameSeed;
// 52B998: using guessed type int SeedCount;

int GetRndSeed()
{
	SeedCount++;
	sglGameSeed = rand_multiplier * sglGameSeed + rand_increment;
	return abs(sglGameSeed);
}
// 52B97C: using guessed type int sglGameSeed;
// 52B998: using guessed type int SeedCount;

int random(BYTE idx, int v)
{
	if (v <= 0)
		return 0;
	if (v >= 0xFFFF)
		return GetRndSeed() % v;
	return (GetRndSeed() >> 16) % v;
}

void engine_debug_trap(BOOL show_cursor)
{
/*
	TMemBlock *pCurr;

	sgMemCrit.Enter();
	while(sgpMemBlock != NULL) {
		pCurr = sgpMemBlock->pNext;
		SMemFree(sgpMemBlock, "C:\\Diablo\\Direct\\ENGINE.CPP", 1970);
		sgpMemBlock = pCurr;
	}
	sgMemCrit.Leave();
*/
}

unsigned char *DiabloAllocPtr(int dwBytes)
{
	BYTE *buf;

#ifdef __cplusplus
	sgMemCrit.Enter();
#endif
	buf = (BYTE *)SMemAlloc(dwBytes, "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2236, 0);
#ifdef __cplusplus
	sgMemCrit.Leave();
#endif

	if (buf == NULL) {
		ErrDlg(IDD_DIALOG2, GetLastError(), "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2269);
	}

	return buf;
}

void mem_free_dbg(void *p)
{
	if (p) {
#ifdef __cplusplus
		sgMemCrit.Enter();
#endif
		SMemFree(p, "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2317, 0);
#ifdef __cplusplus
		sgMemCrit.Leave();
#endif
	}
}

BYTE *LoadFileInMem(char *pszName, int *pdwFileLen)
{
	HANDLE file;
	BYTE *buf;
	int fileLen;

	WOpenFile(pszName, &file, FALSE);
	fileLen = WGetFileSize(file, NULL);

	if (pdwFileLen)
		*pdwFileLen = fileLen;

	if (!fileLen)
		app_fatal("Zero length SFILE:\n%s", pszName);

	buf = (BYTE *)DiabloAllocPtr(fileLen);

	WReadFile(file, buf, fileLen);
	WCloseFile(file);

	return buf;
}

DWORD LoadFileWithMem(const char *pszName, void *p)
{
	DWORD dwFileLen;
	HANDLE hsFile;

	/// ASSERT: assert(pszName);
	if(p == NULL) {
		app_fatal("LoadFileWithMem(NULL):\n%s", pszName);
	}

	WOpenFile(pszName, &hsFile, FALSE);

	dwFileLen = WGetFileSize(hsFile, NULL);
	if(dwFileLen == 0) {
		app_fatal("Zero length SFILE:\n%s", pszName);
	}

	WReadFile(hsFile, p, dwFileLen);
	WCloseFile(hsFile);

	return dwFileLen;
}

void Cl2ApplyTrans(BYTE *p, BYTE *ttbl, int nCel)
{
	int i, nDataSize;
	char width;
	BYTE *dst;
	DWORD *pFrameTable;

	/// ASSERT: assert(p != NULL);
	/// ASSERT: assert(ttbl != NULL);

	for (i = 1; i <= nCel; i++) {
		pFrameTable = (DWORD *)&p[4 * i];
		dst = &p[pFrameTable[0] + 10];
		nDataSize = pFrameTable[1] - pFrameTable[0] - 10;
		while (nDataSize) {
			width = *dst++;
			nDataSize--;
			/// ASSERT: assert(nDataSize >= 0);
			if (width < 0) {
				width = -width;
				if (width > 65) {
					nDataSize--;
					/// ASSERT: assert(nDataSize >= 0);
					*dst = ttbl[*dst];
					dst++;
				} else {
					nDataSize -= width;
					/// ASSERT: assert(nDataSize >= 0);
					while (width) {
						*dst = ttbl[*dst];
						dst++;
						width--;
					}
				}
			}
		}
	}
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm1(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	Cl2DecDatFrm1(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize - nDataStart,
	    nWidth);
}

void Cl2DecDatFrm1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
#ifdef USE_ASM
	__asm {
		push	ebx
		push	esi
		push	edi
		mov		esi, edx /// UNSAFE: use 'mov esi, pRLEBytes'
		mov		edi, ecx /// UNSAFE: use 'mov edi, pDecodeTo'
		xor		eax, eax
		mov		ebx, nWidth
		mov		ecx, nDataSize
	label1:
		mov		al, [esi]
		inc		esi
		dec		ecx
		test	al, al
		jns		label6
		neg		al
		cmp		al, 41h
		jle		label3
		sub		al, 41h
		dec		ecx
		mov		dl, [esi]
		inc		esi
		sub		ebx, eax
	label2:
		mov		[edi], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label2
		jmp		label5
	label3:
		sub		ecx, eax
		sub		ebx, eax
	label4:
		mov		dl, [esi]
		inc		esi
		mov		[edi], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label4
	label5:
		test	ebx, ebx
		jnz		label10
		mov		ebx, nWidth
		sub		edi, 768
		sub		edi, ebx
		jmp		label10
	label6:
		cmp		eax, ebx
		jle		label7
		mov		edx, ebx
		add		edi, ebx
		sub		eax, ebx
		jmp		label8
	label7:
		mov		edx, eax
		add		edi, eax
		xor		eax, eax
	label8:
		sub		ebx, edx
		jnz		label9
		mov		ebx, nWidth
		sub		edi, 768
		sub		edi, ebx
	label9:
		test	eax, eax
		jnz		label6
	label10:
		test	ecx, ecx
		jnz		label1
		pop		edi
		pop		esi
		pop		ebx
	}
#else
	int w;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
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
				w -= width;
				while (width) {
					*dst = fill;
					dst++;
					width--;
				}
				if (!w) {
					w = nWidth;
					dst -= 768 + w;
				}
				continue;
			} else {
				nDataSize -= width;
				w -= width;
				while (width) {
					*dst = *src;
					src++;
					dst++;
					width--;
				}
				if (!w) {
					w = nWidth;
					dst -= 768 + w;
				}
				continue;
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
				dst -= 768 + w;
			}
		}
	}
#endif
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm2(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	Cl2DecDatFrm2(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize - nDataStart,
	    nWidth,
	    col);
}

void Cl2DecDatFrm2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col)
{
#ifdef USE_ASM
	__asm {
		push	ebx
		push	esi
		push	edi
		mov		esi, edx /// UNSAFE: use 'mov esi, pRLEBytes'
		mov		edi, ecx /// UNSAFE: use 'mov edi, pDecodeTo'
		xor		eax, eax
		mov		ebx, nWidth
		xor		edx, edx
		mov		ecx, nDataSize
		mov		dl, col
	label1:
		mov		al, [esi]
		inc		esi
		dec		ecx
		test	al, al
		jns		label7
		neg		al
		cmp		al, 41h
		jle		label3
		sub		al, 41h
		dec		ecx
		mov		dh, [esi]
		inc		esi
		test	dh, dh
		jz		label7
		mov		[edi-1], dl
		sub		ebx, eax
		mov		[edi+eax], dl
	label2:
		mov		[edi-768], dl
		mov		[edi+768], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label2
		jmp		label6
	label3:
		sub		ecx, eax
		sub		ebx, eax
	label4:
		mov		dh, [esi]
		inc		esi
		test	dh, dh
		jz		label5
		mov		[edi-1], dl
		mov		[edi+1], dl
		mov		[edi-768], dl
		mov		[edi+768], dl
	label5:
		dec		eax
		lea		edi, [edi+1]
		jnz		label4
	label6:
		test	ebx, ebx
		jnz		label11
		mov		ebx, nWidth
		sub		edi, 768
		sub		edi, ebx
		jmp		label11
	label7:
		cmp		eax, ebx
		jle		label8
		mov		edx, ebx
		add		edi, ebx
		sub		eax, ebx
		jmp		label9
	label8:
		mov		edx, eax
		add		edi, eax
		xor		eax, eax
	label9:
		sub		ebx, edx
		jnz		label10
		mov		ebx, nWidth
		sub		edi, 768
		sub		edi, ebx
	label10:
		test	eax, eax
		jnz		label7
		mov		dl, col
	label11:
		test	ecx, ecx
		jnz		label1
		pop		edi
		pop		esi
		pop		ebx
	}
#else
	int w;
	char width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				if (*src++) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while (width) {
						dst[-768] = col;
						dst[768] = col;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= 768 + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				w -= width;
				while (width) {
					if (*src++) {
						dst[-1] = col;
						dst[1] = col;
						dst[-768] = col;
						dst[768] = col;
					}
					dst++;
					width--;
				}
				if (!w) {
					w = nWidth;
					dst -= 768 + w;
				}
				continue;
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
				dst -= 768 + w;
			}
		}
	}
#endif
}

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm3(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light)
{
	int hdr, idx, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[CelSkip];
	if (!hdr)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256;
	if (light >= 4)
		idx += (light - 1) << 8;

	Cl2DecDatLightTbl1(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    &pRLEBytes[hdr],
	    nDataSize - hdr,
	    nWidth,
	    &pLightTbl[idx]);
}
// 525728: using guessed type int light4flag;

void Cl2DecDatLightTbl1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable)
{
#ifdef USE_ASM
	__asm {
		push	ebx
		push	esi
		push	edi
		mov		esi, edx /// UNSAFE: use 'mov esi, pRLEBytes'
		mov		edi, ecx /// UNSAFE: use 'mov edi, pDecodeTo'
		mov		ebx, nWidth
		mov		ecx, nDataSize
		mov		edx, pTable
		push	ebp
		mov		sgnWidth, ebx
		mov		ebp, edx
		xor		eax, eax
		xor		edx, edx
	label1:
		mov		al, [esi]
		inc		esi
		dec		ecx
		test	al, al
		jns		label6
		neg		al
		cmp		al, 41h
		jle		label3
		sub		al, 41h
		dec		ecx
		sub		ebx, eax
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebp+edx]
	label2:
		mov		[edi], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label2
		jmp		label5
	label3:
		sub		ecx, eax
		sub		ebx, eax
	label4:
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebp+edx]
		mov		[edi], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label4
	label5:
		test	ebx, ebx
		jnz		label10
		mov		ebx, sgnWidth
		sub		edi, 768
		sub		edi, ebx
		jmp		label10
	label6:
		cmp		eax, ebx
		jle		label7
		mov		edx, ebx
		add		edi, ebx
		sub		eax, ebx
		jmp		label8
	label7:
		mov		edx, eax
		add		edi, eax
		xor		eax, eax
	label8:
		sub		ebx, edx
		jnz		label9
		mov		ebx, sgnWidth
		sub		edi, 768
		sub		edi, ebx
	label9:
		test	eax, eax
		jnz		label6
	label10:
		test	ecx, ecx
		jnz		label1
		pop		ebp
		pop		edi
		pop		esi
		pop		ebx
	}
#else
	int w;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;
	sgnWidth = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = pTable[*src++];
				w -= width;
				while (width) {
					*dst = fill;
					dst++;
					width--;
				}
				if (!w) {
					w = sgnWidth;
					dst -= 768 + w;
				}
				continue;
			} else {
				nDataSize -= width;
				w -= width;
				while (width) {
					*dst = pTable[*src];
					src++;
					dst++;
					width--;
				}
				if (!w) {
					w = sgnWidth;
					dst -= 768 + w;
				}
				continue;
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
				w = sgnWidth;
				dst -= 768 + w;
			}
		}
	}
#endif
}
// 52B978: using guessed type int sgnWidth;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeLightTbl(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int hdr, nDataSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[CelSkip];
	if (!hdr)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	if (light_table_index)
		Cl2DecDatLightTbl1(pDecodeTo, &pRLEBytes[hdr], nDataSize - hdr, nWidth, &pLightTbl[light_table_index * 256]);
	else
		Cl2DecDatFrm1(pDecodeTo, &pRLEBytes[hdr], nDataSize - hdr, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm4(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	Cl2DecDatFrm4(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize - nDataStart,
	    nWidth);
}

void Cl2DecDatFrm4(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
#ifdef USE_ASM
	__asm {
		push	ebx
		push	esi
		push	edi
		mov		esi, edx /// UNSAFE: use 'mov esi, pRLEBytes'
		mov		edi, ecx /// UNSAFE: use 'mov edi, pDecodeTo'
		xor		eax, eax
		mov		ebx, nWidth
		mov		ecx, nDataSize
	label1:
		mov		al, [esi]
		inc		esi
		dec		ecx
		test	al, al
		jns		label7
		neg		al
		cmp		al, 41h
		jle		label3
		sub		al, 41h
		dec		ecx
		mov		dl, [esi]
		inc		esi
		cmp		edi, gpBufEnd
		jge		label7
		sub		ebx, eax
	label2:
		mov		[edi], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label2
		jmp		label6
	label3:
		sub		ecx, eax
		cmp		edi, gpBufEnd
		jl		label4
		add		esi, eax
		jmp		label7
	label4:
		sub		ebx, eax
	label5:
		mov		dl, [esi]
		inc		esi
		mov		[edi], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label5
	label6:
		test	ebx, ebx
		jnz		label11
		mov		ebx, nWidth
		sub		edi, 768
		sub		edi, ebx
		jmp		label11
	label7:
		cmp		eax, ebx
		jle		label8
		mov		edx, ebx
		add		edi, ebx
		sub		eax, ebx
		jmp		label9
	label8:
		mov		edx, eax
		add		edi, eax
		xor		eax, eax
	label9:
		sub		ebx, edx
		jnz		label10
		mov		ebx, nWidth
		sub		edi, 768
		sub		edi, ebx
	label10:
		test	eax, eax
		jnz		label7
	label11:
		test	ecx, ecx
		jnz		label1
		pop		edi
		pop		esi
		pop		ebx
	}
#else
	int w;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
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
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						*dst = fill;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= 768 + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						*dst = *src;
						src++;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= 768 + w;
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
				dst -= 768 + w;
			}
		}
	}
#endif
}
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int nDataStart, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	nDataStart = *(WORD *)&pRLEBytes[CelSkip];
	if (!nDataStart)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	gpBufEnd -= 768;
	Cl2DecDatClrHL(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    pRLEBytes + nDataStart,
	    nDataSize - nDataStart,
	    nWidth,
	    col);
	gpBufEnd += 768;
}
// 69CF0C: using guessed type int gpBufEnd;

void Cl2DecDatClrHL(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col)
{
#ifdef USE_ASM
	__asm {
		push	ebx
		push	esi
		push	edi
		mov		esi, edx /// UNSAFE: use 'mov esi, pRLEBytes'
		mov		edi, ecx /// UNSAFE: use 'mov edi, pDecodeTo'
		xor		eax, eax
		mov		ebx, nWidth
		xor		edx, edx
		mov		ecx, nDataSize
		mov		dl, col
	label1:
		mov		al, [esi]
		inc		esi
		dec		ecx
		test	al, al
		jns		label9
		neg		al
		cmp		al, 41h
		jle		label3
		sub		al, 41h
		dec		ecx
		mov		dh, [esi]
		inc		esi
		test	dh, dh
		jz		label9
		cmp		edi, gpBufEnd
		jge		label9
		mov		[edi-1], dl
		sub		ebx, eax
		mov		[edi+eax], dl
	label2:
		mov		[edi-768], dl
		mov		[edi+768], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label2
		jmp		label7
	label3:
		sub		ecx, eax
		cmp		edi, gpBufEnd
		jl		label4
		add		esi, eax
		jmp		label9
	label4:
		sub		ebx, eax
	label5:
		mov		dh, [esi]
		inc		esi
		test	dh, dh
		jz		label6
		mov		[edi-1], dl
		mov		[edi+1], dl
		mov		[edi-768], dl
		mov		[edi+768], dl
	label6:
		dec		eax
		lea		edi, [edi+1]
		jnz		label5
	label7:
		test	ebx, ebx
		jnz		label13
		mov		ebx, nWidth
		sub		edi, 768
		sub		edi, ebx
		jmp		label13
	label9:
		cmp		eax, ebx
		jle		label10
		mov		edx, ebx
		add		edi, ebx
		sub		eax, ebx
		jmp		label11
	label10:
		mov		edx, eax
		add		edi, eax
		xor		eax, eax
	label11:
		sub		ebx, edx
		jnz		label12
		mov		ebx, nWidth
		sub		edi, 768
		sub		edi, ebx
	label12:
		test	eax, eax
		jnz		label9
		mov		dl, col
	label13:
		test	ecx, ecx
		jnz		label1
		pop		edi
		pop		esi
		pop		ebx
	}
#else
	int w;
	char width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				if (*src++ && dst < gpBufEnd) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while (width) {
						dst[-768] = col;
						dst[768] = col;
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= 768 + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						if (*src++) {
							dst[-1] = col;
							dst[1] = col;
							dst[-768] = col;
							dst[768] = col;
						}
						dst++;
						width--;
					}
					if (!w) {
						w = nWidth;
						dst -= 768 + w;
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
				dst -= 768 + w;
			}
		}
	}
#endif
}
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm5(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap, char light)
{
	int hdr, idx, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[CelSkip];
	if (!hdr)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	idx = light4flag ? 1024 : 4096;
	if (light == 2)
		idx += 256;
	if (light >= 4)
		idx += (light - 1) << 8;

	Cl2DecDatLightTbl2(
	    &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]],
	    &pRLEBytes[hdr],
	    nDataSize - hdr,
	    nWidth,
	    &pLightTbl[idx]);
}
// 525728: using guessed type int light4flag;

void Cl2DecDatLightTbl2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable)
{
#ifdef USE_ASM
	__asm {
		push	ebx
		push	esi
		push	edi
		mov		esi, edx /// UNSAFE: use 'mov esi, pRLEBytes'
		mov		edi, ecx /// UNSAFE: use 'mov edi, pDecodeTo'
		mov		ebx, nWidth
		mov		ecx, nDataSize
		mov		edx, pTable
		push	ebp
		mov		sgnWidth, ebx
		mov		ebp, edx
		xor		eax, eax
		xor		edx, edx
	label1:
		mov		al, [esi]
		inc		esi
		dec		ecx
		test	al, al
		jns		label7
		neg		al
		cmp		al, 41h
		jle		label3
		sub		al, 41h
		dec		ecx
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebp+edx]
		cmp		edi, gpBufEnd
		jge		label7
		sub		ebx, eax
	label2:
		mov		[edi], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label2
		jmp		label6
	label3:
		sub		ecx, eax
		cmp		edi, gpBufEnd
		jl		label4
		add		esi, eax
		jmp		label7
	label4:
		sub		ebx, eax
	label5:
		mov		dl, [esi]
		inc		esi
		mov		dl, [ebp+edx]
		mov		[edi], dl
		dec		eax
		lea		edi, [edi+1]
		jnz		label5
	label6:
		test	ebx, ebx
		jnz		label11
		mov		ebx, sgnWidth
		sub		edi, 768
		sub		edi, ebx
		jmp		label11
	label7:
		cmp		eax, ebx
		jle		label8
		mov		edx, ebx
		add		edi, ebx
		sub		eax, ebx
		jmp		label9
	label8:
		mov		edx, eax
		add		edi, eax
		xor		eax, eax
	label9:
		sub		ebx, edx
		jnz		label10
		mov		ebx, sgnWidth
		sub		edi, 768
		sub		edi, ebx
	label10:
		test	eax, eax
		jnz		label7
	label11:
		test	ecx, ecx
		jnz		label1
		pop		ebp
		pop		edi
		pop		esi
		pop		ebx
	}
#else
	int w;
	char width;
	BYTE fill;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	w = nWidth;
	sgnWidth = nWidth;

	while (nDataSize) {
		width = *src++;
		nDataSize--;
		if (width < 0) {
			width = -width;
			if (width > 65) {
				width -= 65;
				nDataSize--;
				fill = pTable[*src++];
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						*dst = fill;
						dst++;
						width--;
					}
					if (!w) {
						w = sgnWidth;
						dst -= 768 + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if (dst < gpBufEnd) {
					w -= width;
					while (width) {
						*dst = pTable[*src];
						src++;
						dst++;
						width--;
					}
					if (!w) {
						w = sgnWidth;
						dst -= 768 + w;
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
				w = sgnWidth;
				dst -= 768 + w;
			}
		}
	}
#endif
}
// 52B978: using guessed type int sgnWidth;
// 69CF0C: using guessed type int gpBufEnd;

/**
 * @param CelSkip Skip lower parts of sprite, must be multiple of 2, max 8
 * @param CelCap Amount of sprite to render from lower to upper, must be multiple of 2, max 8
 */
void Cl2DecodeFrm6(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int hdr, nDataSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if (!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if (!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if (nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[CelSkip];
	if (!hdr)
		return;

	if (CelCap == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[CelCap];
	if (!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	pDecodeTo = &gpBuffer[sx + PitchTbl[sy - 16 * CelSkip]];

	if (light_table_index)
		Cl2DecDatLightTbl2(pDecodeTo, &pRLEBytes[hdr], nDataSize - hdr, nWidth, &pLightTbl[light_table_index * 256]);
	else
		Cl2DecDatFrm4(pDecodeTo, &pRLEBytes[hdr], nDataSize - hdr, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

void PlayInGameMovie(char *pszMovie)
{
	PaletteFadeOut(8);
	play_movie(pszMovie, 0);
	ClearScreenBuffer();
	drawpanflag = 255;
	scrollrt_draw_game_screen(1);
	PaletteFadeIn(8);
	drawpanflag = 255;
}
// 52571C: using guessed type int drawpanflag;

DEVILUTION_END_NAMESPACE
