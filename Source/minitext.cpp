//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

int qtexty; // weak
char *qtextptr;
int qtextSpd;     // weak
char qtextflag;   // weak
int scrolltexty;  // weak
int sgLastScroll; // weak
void *pMedTextCels;
void *pTextBoxCels;

const unsigned char mfontframe[127] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 37, 49, 38, 0, 39, 40, 47,
	42, 43, 41, 45, 52, 44, 53, 55, 36, 27,
	28, 29, 30, 31, 32, 33, 34, 35, 51, 50,
	48, 46, 49, 54, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 42, 0, 43, 0, 0, 0, 1, 2, 3,
	4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
	14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
	24, 25, 26, 48, 0, 49, 0
};
const unsigned char mfontkern[56] = {
	5, 15, 10, 13, 14, 10, 9, 13, 11, 5,
	5, 11, 10, 16, 13, 16, 10, 15, 12, 10,
	14, 17, 17, 22, 17, 16, 11, 5, 11, 11,
	11, 10, 11, 11, 11, 11, 15, 5, 10, 18,
	15, 8, 6, 6, 7, 10, 9, 6, 10, 10,
	5, 5, 5, 5, 11, 12
};

/* data */

int qscroll_spd_tbl[9] = { 2, 4, 6, 8, 0, -1, -2, -3, -4 };

void __cdecl FreeQuestText()
{
	void *ptr;

	ptr = pMedTextCels;
	pMedTextCels = NULL;
	mem_free_dbg(ptr);
	ptr = pTextBoxCels;
	pTextBoxCels = NULL;
	mem_free_dbg(ptr);
}

void __cdecl InitQuestText()
{
	unsigned char *v0; // eax

	pMedTextCels = LoadFileInMem("Data\\MedTextS.CEL", 0);
	v0 = LoadFileInMem("Data\\TextBox.CEL", 0);
	qtextflag = FALSE;
	pTextBoxCels = v0;
}
// 646D00: using guessed type char qtextflag;

void __fastcall InitQTextMsg(int m)
{
	if (alltext[m].scrlltxt) {
		questlog = 0;
		qtextptr = alltext[m].txtstr;
		qtextflag = TRUE;
		qtexty = 500;
		sgLastScroll = qscroll_spd_tbl[alltext[m].txtspd - 1]; /* double check offset */
		scrolltexty = sgLastScroll;
		qtextSpd = GetTickCount();
	}
	PlaySFX(alltext[m].sfxnr);
}
// 646CF4: using guessed type int qtexty;
// 646CFC: using guessed type int qtextSpd;
// 646D00: using guessed type char qtextflag;
// 646D04: using guessed type int scrolltexty;
// 646D08: using guessed type int sgLastScroll;
// 69BD04: using guessed type int questlog;

void __cdecl DrawQTextBack()
{
	CelDecodeOnly(88, 487, (BYTE *)pTextBoxCels, 1, 591);

#define TRANS_RECT_X 27
#define TRANS_RECT_Y 28
#define TRANS_RECT_WIDTH 585
#define TRANS_RECT_HEIGHT 297
#include "asm_trans_rect.inc"
}

void __fastcall PrintQTextChr(int sx, int sy, BYTE *pCelBuff, int nCel)
{
	BYTE *dst, *pStart, *pEnd, *end;

	/// ASSERT: assert(gpBuffer);

	dst = &gpBuffer[sx + screen_y_times_768[sy]];
	pStart = &gpBuffer[screen_y_times_768[209]];
	pEnd = &gpBuffer[screen_y_times_768[469]];

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
	__asm {
		mov		ebx, pCelBuff
		mov		eax, nCel
		shl		eax, 2
		add		ebx, eax
		mov		eax, [ebx+4]
		sub		eax, [ebx]
		mov		end, eax
		mov		esi, pCelBuff
		add		esi, [ebx]
		mov		edi, dst
		mov		ebx, end
		add		ebx, esi
	label1:
		mov		edx, 22
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label7
		sub		edx, eax
		cmp		edi, pStart
		jb		label5
		cmp		edi, pEnd
		ja		label5
		mov		ecx, eax
		shr		ecx, 1
		jnb		label3
		movsb
		jecxz	label6
	label3:
		shr		ecx, 1
		jnb		label4
		movsw
		jecxz	label6
	label4:
		rep movsd
		jmp		label6
	label5:
		add		esi, eax
		add		edi, eax
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
		sub		edi, 768 + 22
		cmp		ebx, esi
		jnz		label1
	}
#else
	int i;
	BYTE width;
	BYTE *src;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)&pCelBuff[4 * nCel];
	src = &pCelBuff[pFrameTable[0]];
	end = &src[pFrameTable[1] - pFrameTable[0]];

	for (; src != end; dst -= 768 + 22) {
		for (i = 22; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				if (dst >= pStart && dst <= pEnd) {
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

void __cdecl DrawQText()
{
	char *v0;          // edi
	signed int v1;     // edx
	int v2;            // ecx
	char *i;           // esi
	unsigned char v4;  // al
	unsigned char v5;  // al
	char v6;           // dl
	char *v7;          // eax
	unsigned char v8;  // al
	char *v9;          // esi
	unsigned char v10; // bl
	DWORD v11;         // eax
	char qstr[128];    // [esp+8h] [ebp-90h]
	char *v13;         // [esp+88h] [ebp-10h]
	int v14;           // [esp+8Ch] [ebp-Ch]
	int screen_y;      // [esp+90h] [ebp-8h]
	int screen_x;      // [esp+94h] [ebp-4h]

	DrawQTextBack();
	v0 = qtextptr;
	screen_x = MAXDUNX;
	v13 = 0;
	screen_y = qtexty;
	v14 = 0;
	do {
		v1 = 0;
		v2 = 0;
		for (i = v0; *i != 10; ++v2) {
			if (*i == 124 || v1 >= 543)
				break;
			v4 = *i++;
			v5 = gbFontTransTbl[v4];
			if (v5) {
				qstr[v2] = v5;
				v1 += mfontkern[mfontframe[v5]] + 2;
			} else {
				--v2;
			}
		}
		v6 = *i;
		v7 = &qstr[v2];
		qstr[v2] = 0;
		if (v6 == 124) {
			*v7 = 0;
			v14 = 1;
		} else if (v6 != 10) {
			while (*v7 != 32 && v2 > 0) {
				*v7 = 0;
				v7 = &qstr[--v2];
			}
		}
		v8 = qstr[0];
		if (qstr[0]) {
			v9 = qstr;
			do {
				++v0;
				v10 = mfontframe[gbFontTransTbl[v8]];
				if (*v0 == 10)
					++v0;
				if (v10)
					PrintQTextChr(screen_x, screen_y, (BYTE *)pMedTextCels, v10);
				++v9;
				screen_x += mfontkern[v10] + 2;
				v8 = *v9;
			} while (*v9);
		}
		if (!v13)
			v13 = v0;
		screen_y += 38;
		screen_x = MAXDUNX;
		if (screen_y > 501)
			v14 = 1;
	} while (!v14);
	v11 = GetTickCount();
	while (1) {
		if (sgLastScroll <= 0) {
			qtexty = qtexty + sgLastScroll - 1;
			goto LABEL_33;
		}
		if (--scrolltexty) {
			--qtexty;
		LABEL_33:
			if (scrolltexty)
				goto LABEL_35;
		}
		scrolltexty = sgLastScroll;
	LABEL_35:
		if (qtexty <= 209)
			break;
		qtextSpd += 50;
		if (v11 - qtextSpd >= 0x7FFFFFFF)
			return;
	}
	qtexty += 38;
	qtextptr = v13;
	if (*v13 == 124)
		qtextflag = FALSE;
}
// 646CF4: using guessed type int qtexty;
// 646CFC: using guessed type int qtextSpd;
// 646D00: using guessed type char qtextflag;
// 646D04: using guessed type int scrolltexty;
// 646D08: using guessed type int sgLastScroll;
// 428202: using guessed type char qstr[128];

DEVILUTION_END_NAMESPACE
