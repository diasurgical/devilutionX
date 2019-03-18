//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
#pragma warning (disable : 4731) // frame pointer register 'ebp' modified by inline assembly code
#endif

char gbPixelCol;  // automap pixel color 8-bit (palette entry)
int dword_52B970; // BOOLEAN flip - if y < x
int orgseed;      // weak
int sgnWidth;
int sglGameSeed; // weak
static CRITICAL_SECTION sgMemCrit;
int SeedCount;    // weak
int dword_52B99C; // BOOLEAN valid - if x/y are in bounds

const int rand_increment = 1;           // unused
const int rand_multiplier = 0x015A4E35; // unused

void __fastcall CelDrawDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;

	/// ASSERT: assert(pDecodeTo != NULL);
	if(!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if(!pRLEBytes)
		return;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	for(; src != &pRLEBytes[nDataSize]; dst -= 768 + w) {
		for(i = w; i;) {
			width = *src++;
			if(!(width & 0x80)) {
				i -= width;
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
				i -= width;
			}
		}
	}
#endif
}

void __fastcall CelDecodeOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	CelDrawDatOnly(
		&gpBuffer[sx + screen_y_times_768[sy]],
		&pCelBuff[pFrameTable[nCel]],
		pFrameTable[nCel + 1] - pFrameTable[nCel],
		nWidth);
}

void __fastcall CelDecDatOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth)
{
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if(!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	CelDrawDatOnly(
		pBuff,
		&pCelBuff[pFrameTable[nCel]],
		pFrameTable[nCel + 1] - pFrameTable[nCel],
		nWidth);
}

void __fastcall CelDrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int v1, v2, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	v1 = *(WORD *)&pRLEBytes[always_0];

	if(!v1)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	if(dir == 8)
		v2 = 0;
	else
		v2 = *(WORD *)&pRLEBytes[dir];
	if(v2)
		nDataSize = v2 - v1;
	else
		nDataSize -= v1;

	CelDrawDatOnly(
		&gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]],
		&pRLEBytes[v1],
		nDataSize,
		nWidth);
}

void __fastcall CelDecodeHdrOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int v1, v2, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if(!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	v1 = *(WORD *)&pRLEBytes[always_0];

	if(!v1)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	if(dir == 8)
		v2 = 0;
	else
		v2 = *(WORD *)&pRLEBytes[dir];
	if(v2)
		nDataSize = v2 - v1;
	else
		nDataSize -= v1;

	CelDrawDatOnly(pBuff, &pRLEBytes[v1], nDataSize, nWidth);
}

void __fastcall CelDecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if(!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if(!pRLEBytes)
		return;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	__asm {
	labexit:
	}
#else
	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = (BYTE *)&pLightTbl[light_table_index * 256];
	w = nWidth;

	for(; src != &pRLEBytes[nDataSize]; dst -= 768 + w) {
		for(i = w; i;) {
			width = *src++;
			if(!(width & 0x80)) {
				i -= width;
				if(width & 1) {
					dst[0] = tbl[src[0]];
					src++;
					dst++;
				}
				width >>= 1;
				if(width & 1) {
					dst[0] = tbl[src[0]];
					dst[1] = tbl[src[1]];
					src += 2;
					dst += 2;
				}
				width >>= 1;
				for(; width; width--) {
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

void __fastcall CelDecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BOOL shift;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if(!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if(!pRLEBytes)
		return;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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
	tbl = (BYTE *)&pLightTbl[light_table_index * 256];
	w = nWidth;
	shift = (BYTE)dst & 1;

	for(; src != &pRLEBytes[nDataSize]; dst -= 768 + w, shift = (shift + 1) & 1) {
		for(i = w; i;) {
			width = *src++;
			if(!(width & 0x80)) {
				i -= width;
				if(((BYTE)dst & 1) == shift) {
					if(!(width & 1)) {
						goto L_ODD;
					} else {
						src++;
						dst++;
L_EVEN:
						width >>= 1;
						if(width & 1) {
							dst[0] = tbl[src[0]];
							src += 2;
							dst += 2;
						}
						width >>= 1;
						for(; width; width--) {
							dst[0] = tbl[src[0]];
							dst[2] = tbl[src[2]];
							src += 4;
							dst += 4;
						}
					}
				} else {
					if(!(width & 1)) {
						goto L_EVEN;
					} else {
						dst[0] = tbl[src[0]];
						src++;
						dst++;
L_ODD:
						width >>= 1;
						if(width & 1) {
							dst[1] = tbl[src[1]];
							src += 2;
							dst += 2;
						}
						width >>= 1;
						for(; width; width--) {
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

void __fastcall CelDecodeLightOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth)
{
	int nDataSize;
	BYTE *pDecodeTo, *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	pDecodeTo = &gpBuffer[sx + screen_y_times_768[sy]];

	if(light_table_index)
		CelDecDatLightOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
	else
		CelDrawDatOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall CelDecodeHdrLightOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize, v1, v2;
	BYTE *pRLEBytes, *pDecodeTo, *v9;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	v9 = &pCelBuff[pFrameTable[0]];
	hdr = *(WORD *)&v9[always_0];
	if(!hdr)
		return;

	v1 = pFrameTable[1] - pFrameTable[0];
	if(dir == 8)
		v2 = 0;
	else
		v2 = *(WORD *)&v9[dir];
	if(v2)
		nDataSize = v2 - hdr;
	else
		nDataSize = v1 - hdr;

	pRLEBytes = &v9[hdr];
	pDecodeTo = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

	if(light_table_index)
		CelDecDatLightOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
	else
		CelDrawDatOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall CelDecodeHdrLightTrans(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize, v1, v2;
	BYTE *pRLEBytes, *v9;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if(!pBuff)
		return;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	v9 = &pCelBuff[pFrameTable[0]];
	hdr = *(WORD *)&v9[always_0];
	if(!hdr)
		return;

	v1 = pFrameTable[1] - pFrameTable[0];
	if(dir == 8)
		v2 = 0;
	else
		v2 = *(WORD *)&v9[dir];
	if(v2)
		nDataSize = v2 - hdr;
	else
		nDataSize = v1 - hdr;

	pRLEBytes = &v9[hdr];

	if(cel_transparency_active)
		CelDecDatLightTrans(pBuff, pRLEBytes, nDataSize, nWidth);
	else if(light_table_index)
		CelDecDatLightOnly(pBuff, pRLEBytes, nDataSize, nWidth);
	else
		CelDrawDatOnly(pBuff, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;

void __fastcall CelDrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir, char light)
{
	int w, hdr, idx, nDataSize, v1;
	BYTE *src, *dst, *tbl, *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	pRLEBytes = &pCelBuff[pFrameTable[0]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		v1 = 0;
	else
		v1 = *(WORD *)&pRLEBytes[dir];
	if(v1)
		nDataSize = v1 - hdr;
	else
		nDataSize = pFrameTable[1] - pFrameTable[0] - hdr;

	src = &pRLEBytes[hdr];
	dst = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

	idx = light4flag ? 1024 : 4096;
	if(light == 2)
		idx += 256;
	if(light >= 4)
		idx += (light - 1) << 8;

	tbl = (BYTE *)&pLightTbl[idx];

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	for(; src != end; dst -= 768 + nWidth) {
		for(w = nWidth; w;) {
			width = *src++;
			if(!(width & 0x80)) {
				w -= width;
				while(width) {
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

void __fastcall Cel2DecDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;

	/// ASSERT: assert(pDecodeTo != NULL);
	if(!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if(!pRLEBytes)
		return;
	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	for(; src != &pRLEBytes[nDataSize]; dst -= 768 + w) {
		for(i = w; i;) {
			width = *src++;
			if(!(width & 0x80)) {
				i -= width;
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
				i -= width;
			}
		}
	}
#endif
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cel2DrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int v1, v2, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	v1 = *(WORD *)&pRLEBytes[always_0];

	if(!v1)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	if(dir == 8)
		v2 = 0;
	else
		v2 = *(WORD *)&pRLEBytes[dir];
	if(v2)
		nDataSize = v2 - v1;
	else
		nDataSize -= v1;

	Cel2DecDatOnly(
		&gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]],
		&pRLEBytes[v1],
		nDataSize,
		nWidth);
}

void __fastcall Cel2DecodeHdrOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int v1, v2, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if(!pBuff)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	v1 = *(WORD *)&pRLEBytes[always_0];

	if(!v1)
		return;

	nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	v2 = *(WORD *)&pRLEBytes[dir];
	if(dir == 8)
		v2 = 0;
	if(v2)
		nDataSize = v2 - v1;
	else
		nDataSize -= v1;

	Cel2DecDatOnly(pBuff, &pRLEBytes[v1], nDataSize, nWidth);
}

void __fastcall Cel2DecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if(!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if(!pRLEBytes)
		return;
	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	__asm {
	labexit:
	}
#else
	int i;
	BYTE width;
	BYTE *src, *dst;

	src = pRLEBytes;
	dst = pDecodeTo;
	tbl = (BYTE *)&pLightTbl[light_table_index * 256];
	w = nWidth;

	for(; src != &pRLEBytes[nDataSize]; dst -= 768 + w) {
		for(i = w; i;) {
			width = *src++;
			if(!(width & 0x80)) {
				i -= width;
				if(dst < gpBufEnd) {
					if(width & 1) {
						dst[0] = tbl[src[0]];
						src++;
						dst++;
					}
					width >>= 1;
					if(width & 1) {
						dst[0] = tbl[src[0]];
						dst[1] = tbl[src[1]];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					for(; width; width--) {
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

void __fastcall Cel2DecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
	int w;
	BOOL shift;
	BYTE *tbl;

	/// ASSERT: assert(pDecodeTo != NULL);
	if(!pDecodeTo)
		return;
	/// ASSERT: assert(pRLEBytes != NULL);
	if(!pRLEBytes)
		return;
	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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
	tbl = (BYTE *)&pLightTbl[light_table_index * 256];
	w = nWidth;
	shift = (BYTE)dst & 1;

	for(; src != &pRLEBytes[nDataSize]; dst -= 768 + w, shift = (shift + 1) & 1) {
		for(i = w; i;) {
			width = *src++;
			if(!(width & 0x80)) {
				i -= width;
				if(dst < gpBufEnd) {
					if(((BYTE)dst & 1) == shift) {
						if(!(width & 1)) {
							goto L_ODD;
						} else {
							src++;
							dst++;
L_EVEN:
							width >>= 1;
							if(width & 1) {
								dst[0] = tbl[src[0]];
								src += 2;
								dst += 2;
							}
							width >>= 1;
							for(; width; width--) {
								dst[0] = tbl[src[0]];
								dst[2] = tbl[src[2]];
								src += 4;
								dst += 4;
							}
						}
					} else {
						if(!(width & 1)) {
							goto L_EVEN;
						} else {
							dst[0] = tbl[src[0]];
							src++;
							dst++;
L_ODD:
							width >>= 1;
							if(width & 1) {
								dst[1] = tbl[src[1]];
								src += 2;
								dst += 2;
							}
							width >>= 1;
							for(; width; width--) {
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

void __fastcall Cel2DecodeHdrLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize, v1;
	BYTE *pRLEBytes, *pDecodeTo, *v9;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	v9 = &pCelBuff[pFrameTable[0]];
	hdr = *(WORD *)&v9[always_0];
	if(!hdr)
		return;

	v1 = *(WORD *)&v9[dir];
	if(dir == 8)
		v1 = 0;
	if(v1)
		nDataSize = v1 - hdr;
	else
		nDataSize = pFrameTable[1] - pFrameTable[0] - hdr;

	pRLEBytes = &v9[hdr];
	pDecodeTo = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

	if(light_table_index)
		Cel2DecDatLightOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
	else
		Cel2DecDatOnly(pDecodeTo, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall Cel2DecodeLightTrans(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize, v1, v2;
	BYTE *pRLEBytes, *v9;
	DWORD *pFrameTable;

	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	v9 = &pCelBuff[pFrameTable[0]];
	hdr = *(WORD *)&v9[always_0];
	if(!hdr)
		return;

	v1 = pFrameTable[1] - pFrameTable[0];
	v2 = *(WORD *)&v9[dir];
	if(dir == 8)
		v2 = 0;
	if(v2)
		nDataSize = v2 - hdr;
	else
		nDataSize = v1 - hdr;

	pRLEBytes = &v9[hdr];

	if(cel_transparency_active)
		Cel2DecDatLightTrans(pBuff, pRLEBytes, nDataSize, nWidth);
	else if(light_table_index)
		Cel2DecDatLightOnly(pBuff, pRLEBytes, nDataSize, nWidth);
	else
		Cel2DecDatOnly(pBuff, pRLEBytes, nDataSize, nWidth);
}
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;

void __fastcall Cel2DrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir, char light)
{
	int w, hdr, idx, nDataSize, v1;
	BYTE *src, *dst, *tbl, *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	pRLEBytes = &pCelBuff[pFrameTable[0]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		v1 = 0;
	else
		v1 = *(WORD *)&pRLEBytes[dir];
	if(v1)
		nDataSize = v1 - hdr;
	else
		nDataSize = pFrameTable[1] - pFrameTable[0] - hdr;

	src = &pRLEBytes[hdr];
	dst = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

	idx = light4flag ? 1024 : 4096;
	if(light == 2)
		idx += 256;
	if(light >= 4)
		idx += (light - 1) << 8;

	tbl = (BYTE *)&pLightTbl[idx];

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
	__asm {
		mov		eax, nWidth
		add		eax, 768
		mov		w, eax /* use C for w? w = 768 + nWidth */
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

	for(; src != end; dst -= 768 + nWidth) {
		for(w = nWidth; w;) {
			width = *src++;
			if(!(width & 0x80)) {
				w -= width;
				if(dst < gpBufEnd) {
					while(width) {
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

void __fastcall CelDecodeRect(BYTE *pBuff, int always_0, int hgt, int wdt, BYTE *pCelBuff, int nCel, int nWidth)
{
	BYTE *src, *dst, *end;

	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(pBuff != NULL);
	if(!pBuff)
		return;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	dst = &pBuff[hgt * wdt + always_0];

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
	dst = &pBuff[hgt * wdt + always_0];

	for(; src != end; dst -= wdt + nWidth) {
		for(i = nWidth; i;) {
			width = *src++;
			if(!(width & 0x80)) {
				i -= width;
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
				while(width) {
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

void __fastcall CelDecodeClr(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int w, hdr, nDataSize, v1;
	BYTE *src, *dst;

	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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
		add		edx, always_0
		xor		eax, eax
		mov		ax, [edx]
		mov		hdr, eax
		mov		edx, src
		add		edx, dir
		mov		ax, [edx]
		mov		v1, eax
	}

	if(!hdr)
		return;

	if(dir == 8)
		v1 = 0;
	if(v1)
		nDataSize = v1 - hdr;
	else
		nDataSize -= hdr;

	src += hdr;
	dst = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

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
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	v1 = *(WORD *)&pRLEBytes[dir];
	if(dir == 8)
		v1 = 0;
	if(v1)
		nDataSize = v1 - hdr;
	else
		nDataSize = pFrameTable[1] - pFrameTable[0] - hdr;

	src = &pRLEBytes[hdr];
	end = &src[nDataSize];
	dst = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

	for(; src != end; dst -= 768 + nWidth) {
		for(w = nWidth; w;) {
			width = *src++;
			if(!(width & 0x80)) {
				w -= width;
				while(width) {
					if(*src++) {
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

void __fastcall CelDrawHdrClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int w, hdr, nDataSize, v1;
	BYTE *src, *dst;

	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(gpBuffer);
	if(!gpBuffer)
		return;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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
		add		edx, always_0
		xor		eax, eax
		mov		ax, [edx]
		mov		hdr, eax
		mov		edx, src
		add		edx, dir
		mov		ax, [edx]
		mov		v1, eax
	}

	if(!hdr)
		return;

	if(dir == 8)
		v1 = 0;
	if(v1)
		nDataSize = v1 - hdr;
	else
		nDataSize -= hdr;

	src += hdr;
	dst = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

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
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	v1 = *(WORD *)&pRLEBytes[dir];
	if(dir == 8)
		v1 = 0;
	if(v1)
		nDataSize = v1 - hdr;
	else
		nDataSize = pFrameTable[1] - pFrameTable[0] - hdr;

	src = &pRLEBytes[hdr];
	end = &src[nDataSize];
	dst = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

	for(; src != end; dst -= 768 + nWidth) {
		for(w = nWidth; w;) {
			width = *src++;
			if(!(width & 0x80)) {
				w -= width;
				if(dst < gpBufEnd) {
					if(dst >= gpBufEnd - 768) {
						while(width) {
							if(*src++) {
								dst[-768] = col;
								dst[-1] = col;
								dst[1] = col;
							}
							dst++;
							width--;
						}
					} else {
						while(width) {
							if(*src++) {
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

void __fastcall ENG_set_pixel(int screen_x, int screen_y, UCHAR pixel)
{
	UCHAR *v3; // edi

	if (screen_y >= 0 && screen_y < 640 && screen_x >= 64 && screen_x < 704) {
		v3 = (UCHAR *)gpBuffer + screen_y_times_768[screen_y] + screen_x;
		if (v3 < gpBufEnd)
			*v3 = pixel;
	}
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall engine_draw_pixel(int x, int y)
{
	_BYTE *v2; // eax

	if (dword_52B970) {
		if (!dword_52B99C || x >= 0 && x < 640 && y >= 64 && y < 704) {
			v2 = (unsigned char *)gpBuffer + screen_y_times_768[x] + y;
			goto LABEL_14;
		}
	} else if (!dword_52B99C || y >= 0 && y < 640 && x >= 64 && x < 704) {
		v2 = (unsigned char *)gpBuffer + screen_y_times_768[y] + x;
	LABEL_14:
		if (v2 < gpBufEnd)
			*v2 = gbPixelCol;
		return;
	}
}
// 52B96C: using guessed type char gbPixelCol;
// 52B970: using guessed type int dword_52B970;
// 52B99C: using guessed type int dword_52B99C;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall DrawLine(int x0, int y0, int x1, int y1, UCHAR col)
{
	int v5;         // ST18_4
	int v6;         // ST2C_4
	int v7;         // ST20_4
	int v8;         // [esp+Ch] [ebp-48h]
	int v9;         // [esp+10h] [ebp-44h]
	int v10;        // [esp+14h] [ebp-40h]
	int v11;        // [esp+18h] [ebp-3Ch]
	signed int v12; // [esp+1Ch] [ebp-38h]
	int v13;        // [esp+20h] [ebp-34h]
	int v14;        // [esp+24h] [ebp-30h]
	int v15;        // [esp+28h] [ebp-2Ch]
	int y;          // [esp+2Ch] [ebp-28h]
	int ya;         // [esp+2Ch] [ebp-28h]
	int yb;         // [esp+2Ch] [ebp-28h]
	int yc;         // [esp+2Ch] [ebp-28h]
	int j;          // [esp+30h] [ebp-24h]
	int i;          // [esp+30h] [ebp-24h]
	int x;          // [esp+34h] [ebp-20h]
	int xa;         // [esp+34h] [ebp-20h]
	int xb;         // [esp+34h] [ebp-20h]
	int xc;         // [esp+34h] [ebp-20h]
	int xd;         // [esp+34h] [ebp-20h]
	int xe;         // [esp+34h] [ebp-20h]
	int xf;         // [esp+34h] [ebp-20h]
	int xg;         // [esp+34h] [ebp-20h]
	int xh;         // [esp+34h] [ebp-20h]
	int v31;        // [esp+38h] [ebp-1Ch]
	int v32;        // [esp+3Ch] [ebp-18h]
	int v33;        // [esp+3Ch] [ebp-18h]
	int v34;        // [esp+3Ch] [ebp-18h]
	signed int v35; // [esp+40h] [ebp-14h]
	signed int v36; // [esp+44h] [ebp-10h]
	int v37;        // [esp+48h] [ebp-Ch]
	int v38;        // [esp+48h] [ebp-Ch]
	int v39;        // [esp+4Ch] [ebp-8h]
	int v40;        // [esp+4Ch] [ebp-8h]
	int v41;        // [esp+50h] [ebp-4h]
	int x2a;        // [esp+5Ch] [ebp+8h]

	v8 = y0;
	v9 = x0;
	gbPixelCol = col;
	dword_52B99C = 0;
	if (x0 < 64 || x0 >= 704)
		dword_52B99C = 1;
	if (x1 < 64 || x1 >= 704)
		dword_52B99C = 1;
	if (y0 < 160 || y0 >= 512)
		dword_52B99C = 1;
	if (y1 < 160 || y1 >= 512)
		dword_52B99C = 1;
	if (x1 - x0 < 0)
		v36 = -1;
	else
		v36 = 1;
	v11 = v36 * (x1 - x0);
	if (y1 - y0 < 0)
		v35 = -1;
	else
		v35 = 1;
	v10 = v35 * (y1 - y0);
	if (v35 == v36)
		v12 = 1;
	else
		v12 = -1;
	if (v11 >= v10) {
		dword_52B970 = 0;
	} else {
		v8 = y0 ^ x0 ^ y0;
		v9 = v8 ^ y0 ^ x0;
		x2a = y1 ^ x1;
		y1 ^= x2a;
		x1 = y1 ^ x2a;
		v5 = v10 ^ v11;
		v10 ^= v5;
		v11 = v10 ^ v5;
		dword_52B970 = 1;
	}
	if (x1 >= v9) {
		x = v9;
		y = v8;
		v32 = x1;
		v13 = y1;
	} else {
		x = x1;
		y = y1;
		v32 = v9;
		v13 = v8;
	}
	v31 = (v11 - 1) / 4;
	v41 = (v11 - 1) % 4; /* (((v11 - 1) >> 31) ^ abs(v11 - 1) & 3) - ((v11 - 1) >> 31) */
	engine_draw_pixel(x, y);
	engine_draw_pixel(v32, v13);
	v14 = 4 * v10 - 2 * v11;
	if (v14 >= 0) {
		v40 = 2 * (v10 - v11);
		v15 = 4 * (v10 - v11);
		v38 = v15 + v11;
		for (i = 0; i < v31; ++i) {
			xe = x + 1;
			v34 = v32 - 1;
			if (v38 <= 0) {
				if (v40 <= v38) {
					y += v12;
					engine_draw_pixel(xe, y);
					x = xe + 1;
					engine_draw_pixel(x, y);
					v13 -= v12;
					engine_draw_pixel(v34, v13);
				} else {
					engine_draw_pixel(xe, y);
					y += v12;
					x = xe + 1;
					engine_draw_pixel(x, y);
					engine_draw_pixel(v34, v13);
					v13 -= v12;
				}
				v32 = v34 - 1;
				engine_draw_pixel(v32, v13);
				v38 += v14;
			} else {
				v6 = v12 + y;
				engine_draw_pixel(xe, v6);
				y = v12 + v6;
				x = xe + 1;
				engine_draw_pixel(x, y);
				v7 = v13 - v12;
				engine_draw_pixel(v34, v7);
				v13 = v7 - v12;
				v32 = v34 - 1;
				engine_draw_pixel(v32, v13);
				v38 += v15;
			}
		}
		if (v41) {
			if (v38 <= 0) {
				if (v40 <= v38) {
					yc = v12 + y;
					xh = x + 1;
					engine_draw_pixel(xh, yc);
					if (v41 > 1)
						engine_draw_pixel(xh + 1, yc);
					if (v41 > 2) {
						if (v40 >= v38)
							engine_draw_pixel(v32 - 1, v13);
						else
							engine_draw_pixel(v32 - 1, v13 - v12);
					}
				} else {
					xg = x + 1;
					engine_draw_pixel(xg, y);
					if (v41 > 1)
						engine_draw_pixel(xg + 1, v12 + y);
					if (v41 > 2)
						engine_draw_pixel(v32 - 1, v13);
				}
			} else {
				yb = v12 + y;
				xf = x + 1;
				engine_draw_pixel(xf, yb);
				if (v41 > 1)
					engine_draw_pixel(xf + 1, v12 + yb);
				if (v41 > 2)
					engine_draw_pixel(v32 - 1, v13 - v12);
			}
		}
	} else {
		v39 = 2 * v10;
		v37 = 4 * v10 - v11;
		for (j = 0; j < v31; ++j) {
			xa = x + 1;
			v33 = v32 - 1;
			if (v37 >= 0) {
				if (v39 <= v37) {
					y += v12;
					engine_draw_pixel(xa, y);
					x = xa + 1;
					engine_draw_pixel(x, y);
					v13 -= v12;
					engine_draw_pixel(v33, v13);
				} else {
					engine_draw_pixel(xa, y);
					y += v12;
					x = xa + 1;
					engine_draw_pixel(x, y);
					engine_draw_pixel(v33, v13);
					v13 -= v12;
				}
				v32 = v33 - 1;
				engine_draw_pixel(v32, v13);
				v37 += v14;
			} else {
				engine_draw_pixel(xa, y);
				x = xa + 1;
				engine_draw_pixel(x, y);
				engine_draw_pixel(v33, v13);
				v32 = v33 - 1;
				engine_draw_pixel(v32, v13);
				v37 += 4 * v10;
			}
		}
		if (v41) {
			if (v37 >= 0) {
				if (v39 <= v37) {
					ya = v12 + y;
					xd = x + 1;
					engine_draw_pixel(xd, ya);
					if (v41 > 1)
						engine_draw_pixel(xd + 1, ya);
					if (v41 > 2)
						engine_draw_pixel(v32 - 1, v13 - v12);
				} else {
					xc = x + 1;
					engine_draw_pixel(xc, y);
					if (v41 > 1)
						engine_draw_pixel(xc + 1, v12 + y);
					if (v41 > 2)
						engine_draw_pixel(v32 - 1, v13);
				}
			} else {
				xb = x + 1;
				engine_draw_pixel(xb, y);
				if (v41 > 1)
					engine_draw_pixel(xb + 1, y);
				if (v41 > 2)
					engine_draw_pixel(v32 - 1, v13);
			}
		}
	}
}
// 52B96C: using guessed type char gbPixelCol;
// 52B970: using guessed type int dword_52B970;
// 52B99C: using guessed type int dword_52B99C;

int __fastcall GetDirection(int x1, int y1, int x2, int y2)
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

void __fastcall SetRndSeed(int s)
{
	SeedCount = 0;
	sglGameSeed = s;
	orgseed = s;
}
// 52B974: using guessed type int orgseed;
// 52B97C: using guessed type int sglGameSeed;
// 52B998: using guessed type int SeedCount;

int __cdecl GetRndSeed()
{
	SeedCount++;
	sglGameSeed = 0x015A4E35 * sglGameSeed + 1;
	return abs(sglGameSeed);
}
// 52B97C: using guessed type int sglGameSeed;
// 52B998: using guessed type int SeedCount;

int __fastcall random(BYTE idx, int v)
{
	if (v <= 0)
		return 0;
	if (v >= 0xFFFF)
		return GetRndSeed() % v;
	return (GetRndSeed() >> 16) % v;
}

#ifndef _MSC_VER
__attribute__((constructor))
#endif
static void
engine_c_init(void)
{
	mem_init_mutex();
	mem_atexit_mutex();
}

SEG_ALLOCATE(SEGMENT_C_INIT)
_PVFV engine_c_init_funcs[] = { &engine_c_init };

void __cdecl mem_init_mutex()
{
	InitializeCriticalSection(&sgMemCrit);
}

void __cdecl mem_atexit_mutex()
{
	atexit(mem_free_mutex);
}

void __cdecl mem_free_mutex(void)
{
	DeleteCriticalSection(&sgMemCrit);
}

unsigned char *__fastcall DiabloAllocPtr(int dwBytes)
{
	BYTE *buf;

	EnterCriticalSection(&sgMemCrit);
	buf = (BYTE *)SMemAlloc(dwBytes, "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2236, 0);
	LeaveCriticalSection(&sgMemCrit);

	if (buf == NULL) {
		ErrDlg(IDD_DIALOG2, GetLastError(), "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2269);
	}

	return buf;
}

void __fastcall mem_free_dbg(void *p)
{
	if (p) {
		EnterCriticalSection(&sgMemCrit);
		SMemFree(p, "C:\\Src\\Diablo\\Source\\ENGINE.CPP", 2317, 0);
		LeaveCriticalSection(&sgMemCrit);
	}
}

BYTE *__fastcall LoadFileInMem(char *pszName, int *pdwFileLen)
{
	HANDLE file;
	BYTE *buf;
	int fileLen;

	WOpenFile(pszName, &file, FALSE);
	fileLen = WGetFileSize(file, NULL);

	if (pdwFileLen)
		*pdwFileLen = fileLen;

	if (!fileLen)
		TermMsg("Zero length SFILE:\n%s", pszName);

	buf = (BYTE *)DiabloAllocPtr(fileLen);

	WReadFile(file, buf, fileLen);
	WCloseFile(file);

	return buf;
}

void __fastcall LoadFileWithMem(char *pszName, void *buf)
{
	char *v2; // ebx
	char *v3; // edi
	int v4;   // esi
	HANDLE a1; // [esp+Ch] [ebp-4h]

	v2 = (char *)buf;
	v3 = pszName;
	if (!buf)
		TermMsg("LoadFileWithMem(NULL):\n%s", pszName);
	WOpenFile(v3, &a1, 0);
	v4 = WGetFileSize(a1, 0);
	if (!v4)
		TermMsg("Zero length SFILE:\n%s", v3);
	WReadFile(a1, v2, v4);
	WCloseFile(a1);
}

void __fastcall Cl2ApplyTrans(BYTE *p, BYTE *ttbl, int nCel)
{
	int i, nDataSize;
	char width;
	BYTE *dst;
	DWORD *pFrameTable;

	/// ASSERT: assert(p != NULL);
	/// ASSERT: assert(ttbl != NULL);

	for(i = 1; i <= nCel; i++) {
		pFrameTable = (DWORD *)&p[4 * i];
		dst = &p[pFrameTable[0] + 10];
		nDataSize = pFrameTable[1] - pFrameTable[0] - 10;
		while(nDataSize) {
			width = *dst++;
			nDataSize--;
			/// ASSERT: assert(nDataSize >= 0);
			if(width < 0) {
				width = -width;
				if(width > 65) {
					nDataSize--;
					/// ASSERT: assert(nDataSize >= 0);
					*dst = ttbl[*dst];
					dst++;
				} else {
					nDataSize -= width;
					/// ASSERT: assert(nDataSize >= 0);
					while(width) {
						*dst = ttbl[*dst];
						dst++;
						width--;
					}
				}
			}
		}
	}
}

void __fastcall Cl2DecodeFrm1(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if(nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[dir];
	if(!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	Cl2DecDatFrm1(
		&gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]],
		&pRLEBytes[hdr],
		nDataSize - hdr,
		nWidth);
}

void __fastcall Cl2DecDatFrm1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	while(nDataSize) {
		width = *src++;
		nDataSize--;
		if(width < 0) {
			width = -width;
			if(width > 65) {
				width -= 65;
				nDataSize--;
				fill = *src++;
				w -= width;
				while(width) {
					*dst = fill;
					dst++;
					width--;
				}
				if(!w) {
					w = nWidth;
					dst -= 768 + w;
				}
				continue;
			} else {
				nDataSize -= width;
				w -= width;
				while(width) {
					*dst = *src;
					src++;
					dst++;
					width--;
				}
				if(!w) {
					w = nWidth;
					dst -= 768 + w;
				}
				continue;
			}
		}
		while(width) {
			if(width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if(!w) {
				w = nWidth;
				dst -= 768 + w;
			}
		}
	}
#endif
}

void __fastcall Cl2DecodeFrm2(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if(nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[dir];
	if(!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	Cl2DecDatFrm2(
		&gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]],
		&pRLEBytes[hdr],
		nDataSize - hdr,
		nWidth,
		col);
}

void __fastcall Cl2DecDatFrm2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col)
{
#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	while(nDataSize) {
		width = *src++;
		nDataSize--;
		if(width < 0) {
			width = -width;
			if(width > 65) {
				width -= 65;
				nDataSize--;
				if(*src++) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while(width) {
						dst[-768] = col;
						dst[768] = col;
						dst++;
						width--;
					}
					if(!w) {
						w = nWidth;
						dst -= 768 + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				w -= width;
				while(width) {
					if(*src++) {
						dst[-1] = col;
						dst[1] = col;
						dst[-768] = col;
						dst[768] = col;
					}
					dst++;
					width--;
				}
				if(!w) {
					w = nWidth;
					dst -= 768 + w;
				}
				continue;
			}
		}
		while(width) {
			if(width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if(!w) {
				w = nWidth;
				dst -= 768 + w;
			}
		}
	}
#endif
}

void __fastcall Cl2DecodeFrm3(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir, char light)
{
	int hdr, idx, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if(nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[dir];
	if(!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	idx = light4flag ? 1024 : 4096;
	if(light == 2)
		idx += 256;
	if(light >= 4)
		idx += (light - 1) << 8;

	Cl2DecDatLightTbl1(
		&gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]],
		&pRLEBytes[hdr],
		nDataSize - hdr,
		nWidth,
		(BYTE *)&pLightTbl[idx]);
}
// 525728: using guessed type int light4flag;

void __fastcall Cl2DecDatLightTbl1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable)
{
#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	while(nDataSize) {
		width = *src++;
		nDataSize--;
		if(width < 0) {
			width = -width;
			if(width > 65) {
				width -= 65;
				nDataSize--;
				fill = pTable[*src++];
				w -= width;
				while(width) {
					*dst = fill;
					dst++;
					width--;
				}
				if(!w) {
					w = sgnWidth;
					dst -= 768 + w;
				}
				continue;
			} else {
				nDataSize -= width;
				w -= width;
				while(width) {
					*dst = pTable[*src];
					src++;
					dst++;
					width--;
				}
				if(!w) {
					w = sgnWidth;
					dst -= 768 + w;
				}
				continue;
			}
		}
		while(width) {
			if(width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if(!w) {
				w = sgnWidth;
				dst -= 768 + w;
			}
		}
	}
#endif
}
// 52B978: using guessed type int sgnWidth;

void __fastcall Cl2DecodeLightTbl(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if(nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[dir];
	if(!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	pDecodeTo = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

	if(light_table_index)
		Cl2DecDatLightTbl1(pDecodeTo, &pRLEBytes[hdr], nDataSize - hdr, nWidth, (BYTE *)&pLightTbl[light_table_index * 256]);
	else
		Cl2DecDatFrm1(pDecodeTo, &pRLEBytes[hdr], nDataSize - hdr, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall Cl2DecodeFrm4(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if(nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[dir];
	if(!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	Cl2DecDatFrm4(
		&gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]],
		&pRLEBytes[hdr],
		nDataSize - hdr,
		nWidth);
}

void __fastcall Cl2DecDatFrm4(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth)
{
#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	while(nDataSize) {
		width = *src++;
		nDataSize--;
		if(width < 0) {
			width = -width;
			if(width > 65) {
				width -= 65;
				nDataSize--;
				fill = *src++;
				if(dst < gpBufEnd) {
					w -= width;
					while(width) {
						*dst = fill;
						dst++;
						width--;
					}
					if(!w) {
						w = nWidth;
						dst -= 768 + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if(dst < gpBufEnd) {
					w -= width;
					while(width) {
						*dst = *src;
						src++;
						dst++;
						width--;
					}
					if(!w) {
						w = nWidth;
						dst -= 768 + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while(width) {
			if(width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if(!w) {
				w = nWidth;
				dst -= 768 + w;
			}
		}
	}
#endif
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cl2DecodeClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if(nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[dir];
	if(!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	gpBufEnd -= 768;
	Cl2DecDatClrHL(
		&gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]],
		&pRLEBytes[hdr],
		nDataSize - hdr,
		nWidth,
		col);
	gpBufEnd += 768;
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cl2DecDatClrHL(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col)
{
#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	while(nDataSize) {
		width = *src++;
		nDataSize--;
		if(width < 0) {
			width = -width;
			if(width > 65) {
				width -= 65;
				nDataSize--;
				if(*src++ && dst < gpBufEnd) {
					w -= width;
					dst[-1] = col;
					dst[width] = col;
					while(width) {
						dst[-768] = col;
						dst[768] = col;
						dst++;
						width--;
					}
					if(!w) {
						w = nWidth;
						dst -= 768 + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if(dst < gpBufEnd) {
					w -= width;
					while(width) {
						if(*src++) {
							dst[-1] = col;
							dst[1] = col;
							dst[-768] = col;
							dst[768] = col;
						}
						dst++;
						width--;
					}
					if(!w) {
						w = nWidth;
						dst -= 768 + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while(width) {
			if(width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if(!w) {
				w = nWidth;
				dst -= 768 + w;
			}
		}
	}
#endif
}
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cl2DecodeFrm5(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir, char light)
{
	int hdr, idx, nDataSize;
	BYTE *pRLEBytes;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if(nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[dir];
	if(!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	idx = light4flag ? 1024 : 4096;
	if(light == 2)
		idx += 256;
	if(light >= 4)
		idx += (light - 1) << 8;

	Cl2DecDatLightTbl2(
		&gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]],
		&pRLEBytes[hdr],
		nDataSize - hdr,
		nWidth,
		(BYTE *)&pLightTbl[idx]);
}
// 525728: using guessed type int light4flag;

void __fastcall Cl2DecDatLightTbl2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable)
{
#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
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

	while(nDataSize) {
		width = *src++;
		nDataSize--;
		if(width < 0) {
			width = -width;
			if(width > 65) {
				width -= 65;
				nDataSize--;
				fill = pTable[*src++];
				if(dst < gpBufEnd) {
					w -= width;
					while(width) {
						*dst = fill;
						dst++;
						width--;
					}
					if(!w) {
						w = sgnWidth;
						dst -= 768 + w;
					}
					continue;
				}
			} else {
				nDataSize -= width;
				if(dst < gpBufEnd) {
					w -= width;
					while(width) {
						*dst = pTable[*src];
						src++;
						dst++;
						width--;
					}
					if(!w) {
						w = sgnWidth;
						dst -= 768 + w;
					}
					continue;
				} else {
					src += width;
				}
			}
		}
		while(width) {
			if(width > w) {
				dst += w;
				width -= w;
				w = 0;
			} else {
				dst += width;
				w -= width;
				width = 0;
			}
			if(!w) {
				w = sgnWidth;
				dst -= 768 + w;
			}
		}
	}
#endif
}
// 52B978: using guessed type int sgnWidth;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall Cl2DecodeFrm6(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir)
{
	int hdr, nDataSize;
	BYTE *pRLEBytes, *pDecodeTo;
	DWORD *pFrameTable;

	/// ASSERT: assert(gpBuffer != NULL);
	if(!gpBuffer)
		return;
	/// ASSERT: assert(pCelBuff != NULL);
	if(!pCelBuff)
		return;
	/// ASSERT: assert(nCel > 0);
	if(nCel <= 0)
		return;

	pFrameTable = (DWORD *)pCelBuff;
	/// ASSERT: assert(nCel <= (int) pFrameTable[0]);

	pRLEBytes = &pCelBuff[pFrameTable[nCel]];
	hdr = *(WORD *)&pRLEBytes[always_0];
	if(!hdr)
		return;

	if(dir == 8)
		nDataSize = 0;
	else
		nDataSize = *(WORD *)&pRLEBytes[dir];
	if(!nDataSize)
		nDataSize = pFrameTable[nCel + 1] - pFrameTable[nCel];

	pDecodeTo = &gpBuffer[sx + screen_y_times_768[sy - 16 * always_0]];

	if(light_table_index)
		Cl2DecDatLightTbl2(pDecodeTo, &pRLEBytes[hdr], nDataSize - hdr, nWidth, (BYTE *)&pLightTbl[light_table_index * 256]);
	else
		Cl2DecDatFrm4(pDecodeTo, &pRLEBytes[hdr], nDataSize - hdr, nWidth);
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall PlayInGameMovie(char *pszMovie)
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
