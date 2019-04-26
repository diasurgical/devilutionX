#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

int stextup;    // weak
int storenumh;  // weak
int stextlhold; // weak
ItemStruct boyitem;
int stextshold; // idb
ItemStruct premiumitem[6];
void *pSTextBoxCels;
int premiumlevel; // idb
int talker;       // weak
STextStruct stext[24];
char stextsize;  // weak
int stextsmax;   // weak
int InStoreFlag; // idb
ItemStruct storehold[48];
int gossipstart; // weak
ItemStruct witchitem[20];
BOOL stextscrl;
int numpremium; // idb
ItemStruct healitem[20];
ItemStruct golditem;
char storehidx[48];
void *pSTextSlidCels;
int stextvhold;     // weak
int stextsel;       // weak
char stextscrldbtn; // weak
int gossipend;      // weak
void *pCelBuff;
int stextsval; // idb
int boylevel;  // weak
ItemStruct smithitem[20];
int stextdown;      // weak
char stextscrlubtn; // weak
char stextflag;

int SStringY[24] = {
	0,
	12,
	24,
	36,
	48,
	60,
	72,
	84,
	96,
	108,
	120,
	132,
	144,
	156,
	168,
	180,
	192,
	204,
	216,
	228,
	240,
	252,
	264,
	276
};
char *talkname[9] = {
	"Griswold",
	"Pepin",
	"",
	"Ogden",
	"Cain",
	"Farnham",
	"Adria",
	"Gillian",
	"Wirt"
};

void InitStores()
{
	int i;

	pSTextBoxCels = LoadFileInMem("Data\\TextBox2.CEL", 0);
	pCelBuff = LoadFileInMem("Data\\PentSpn2.CEL", 0);
	pSTextSlidCels = LoadFileInMem("Data\\TextSlid.CEL", 0);
	ClearSText(0, 24);
	stextflag = STORE_NONE;
	InStoreFlag = 1;
	stextsize = 0;
	stextscrl = FALSE;
	numpremium = 0;
	premiumlevel = 1;

	for (i = 0; i < 6; i++)
		premiumitem[i]._itype = -1;

	boyitem._itype = -1;
	boylevel = 0;
}

void SetupTownStores()
{
	int i, l;

	SetRndSeed(glSeedTbl[currlevel] * GetTickCount());
	if (gbMaxPlayers == 1) {
		l = 0;
		for (i = 0; i < 17; i++) {
			if (plr[myplr]._pLvlVisited[i])
				l = i;
		}
	} else {
		l = plr[myplr]._pLevel >> 1;
	}
	l += 2;
	if (l < 6)
		l = 6;
	if (l > 16)
		l = 16;
	SpawnStoreGold();
	SpawnSmith(l);
	SpawnWitch(l);
	SpawnHealer(l);
	SpawnBoy(plr[myplr]._pLevel);
	SpawnPremium(plr[myplr]._pLevel);
}

void FreeStoreMem()
{
	MemFreeDbg(pSTextBoxCels);
	MemFreeDbg(pCelBuff);
	MemFreeDbg(pSTextSlidCels);
}

void DrawSTextBack()
{
	CelDecodeOnly(408, 487, (BYTE *)pSTextBoxCels, 1, 271);

#define TRANS_RECT_X 347
#define TRANS_RECT_Y 28
#define TRANS_RECT_WIDTH 265
#define TRANS_RECT_HEIGHT 297
#include "asm_trans_rect.inc"
}

void PrintSString(int x, int y, unsigned char cjustflag, char *str, int col, int val)
{
	int v6;            // edi
	int v7;            // eax
	int v8;            // ebx
	int v9;            // esi
	int v10;           // esi
	int v11;           // ecx
	int v12;           // eax
	int v13;           // edx
	int v14;           // ecx
	unsigned char v15; // al
	int v16;           // ebx
	int v17;           // ecx
	int v18;           // eax
	int v19;           // esi
	size_t v20;        // ebx
	unsigned char v21; // edx
	int v22;           // ecx
	char valstr[32];   // [esp+Ch] [ebp-3Ch]
	int v24;           // [esp+2Ch] [ebp-1Ch]
	int v25;           // [esp+30h] [ebp-18h]
	int v26;           // [esp+34h] [ebp-14h]
	int v27;           // [esp+38h] [ebp-10h]
	int v28;           // [esp+3Ch] [ebp-Ch]
	int v29;           // [esp+40h] [ebp-8h]
	int v30;           // [esp+44h] [ebp-4h]

	v6 = SStringY[y] + stext[y]._syoff;
	v7 = -(stextsize != 0);
	v8 = x;
	v9 = PitchTbl[v6 + 204];
	_LOWORD(v7) = v7 & 0xFEC0;
	v24 = y;
	v26 = x;
	v27 = v7 + 416;
	v10 = x + v7 + 416 + v9;
	v28 = strlen(str);
	v11 = 0;
	v25 = stextsize != 0 ? 577 : 257;
	v30 = 0;
	if (cjustflag) {
		v12 = 0;
		if (v28 > 0) {
			do {
				v13 = (unsigned char)str[v11++];
				v12 += fontkern[fontframe[gbFontTransTbl[v13]]] + 1;
			} while (v11 < v28);
		}
		if (v12 < v25)
			v30 = (v25 - v12) >> 1;
		v10 += v30;
	}
	if (stextsel == v24) {
		if (cjustflag)
			v14 = v27 + v30 + v8 - 20;
		else
			v14 = v27 + v8 - 20;
		CelDecodeOnly(v14, v6 + 205, (BYTE *)pCelBuff, InStoreFlag, 12);
	}
	v29 = 0;
	if (v28 > 0) {
		do {
			v15 = fontframe[gbFontTransTbl[(unsigned char)str[v29]]];
			v16 = v15;
			v17 = v30 + fontkern[v15] + 1;
			v30 += fontkern[v15] + 1;
			if (v15 && v17 <= v25)
				CPrintString(v10, v15, col);
			v18 = fontkern[v16];
			++v29;
			v10 += v18 + 1;
		} while (v29 < v28);
		v8 = v26;
	}
	if (!cjustflag && val >= 0) {
		sprintf(valstr, "%i", val);
		v19 = PitchTbl[v6 + 204] - v8 + 656;
		v20 = strlen(valstr);
		while ((--v20 & 0x80000000) == 0) {
			v21 = fontframe[gbFontTransTbl[(unsigned char)valstr[v20]]];
			v19 += -1 - fontkern[v21];
			if (fontframe[gbFontTransTbl[(unsigned char)valstr[v20]]])
				CPrintString(v19, v21, col);
		}
		v8 = v26;
	}
	if (stextsel == v24) {
		if (cjustflag)
			v22 = v27 + v30 + v8 + 4;
		else
			v22 = 660 - v8;
		CelDecodeOnly(v22, v6 + 205, (BYTE *)pCelBuff, InStoreFlag, 12);
	}
}
// 6A09E0: using guessed type char stextsize;
// 6A8A28: using guessed type int stextsel;
// 457BD6: using guessed type char valstr[32];

void DrawSLine(int y)
{
	int xy, yy, width, line;

	if (stextsize == 1) {
		xy = SCREENXY(26, 25);
		yy = PitchTbl[SStringY[y] + 198] + 26 + 64;
		width = 586 / 4;
		line = 768 - 586;
	} else {
		xy = SCREENXY(346, 25);
		yy = PitchTbl[SStringY[y] + 198] + 346 + 64;
		width = 266 / 4;
		line = 768 - 266;
	}

	/// ASSERT: assert(gpBuffer);

#ifdef USE_ASM
	__asm {
		mov		esi, gpBuffer
		mov		edi, esi
		add		esi, xy
		add		edi, yy
		mov		ebx, line
		mov		edx, 3
	copyline:
		mov		ecx, width
		rep movsd
		movsw
		add		esi, ebx
		add		edi, ebx
		dec		edx
		jnz		copyline
	}
#else
	int i;
	BYTE *src, *dst;

	src = &gpBuffer[xy];
	dst = &gpBuffer[yy];

	for (i = 0; i < 3; i++, src += 768, dst += 768)
		memcpy(dst, src, 768 - line);
#endif
}
// 6A09E0: using guessed type char stextsize;

void DrawSArrows(int y1, int y2)
{
	int yd1, yd2, yd3;

	yd1 = SStringY[y1] + 204;
	yd2 = SStringY[y2] + 204;
	if (stextscrlubtn != -1)
		CelDecodeOnly(665, yd1, (BYTE *)pSTextSlidCels, 12, 12);
	else
		CelDecodeOnly(665, yd1, (BYTE *)pSTextSlidCels, 10, 12);
	if (stextscrldbtn != -1)
		CelDecodeOnly(665, yd2, (BYTE *)pSTextSlidCels, 11, 12);
	else
		CelDecodeOnly(665, yd2, (BYTE *)pSTextSlidCels, 9, 12);
	yd1 += 12;
	for (yd3 = yd1; yd3 < yd2; yd3 += 12) {
		CelDecodeOnly(665, yd3, (BYTE *)pSTextSlidCels, 14, 12);
	}
	if (stextsel == 22)
		yd3 = stextlhold;
	else
		yd3 = stextsel;
	if (storenumh > 1)
		yd3 = 1000 * (stextsval + ((yd3 - stextup) >> 2)) / (storenumh - 1) * (SStringY[y2] - SStringY[y1] - 24) / 1000;
	else
		yd3 = 0;
	CelDecodeOnly(665, SStringY[y1 + 1] + 204 + yd3, (BYTE *)pSTextSlidCels, 13, 12);
}

void DrawSTextHelp()
{
	stextsel = -1;
	stextsize = 1;
}
// 6A09E0: using guessed type char stextsize;
// 6A8A28: using guessed type int stextsel;

void ClearSText(int s, int e)
{
	int i;

	for (i = s; i < e; i++) {
		stext[i]._sx = 0;
		stext[i]._syoff = 0;
		stext[i]._sstr[0] = 0;
		stext[i]._sjust = 0;
		stext[i]._sclr = 0;
		stext[i]._sline = 0;
		stext[i]._ssel = 0;
		stext[i]._sval = -1;
	}
}

void AddSLine(int y)
{
	stext[y]._sx = 0;
	stext[y]._syoff = 0;
	stext[y]._sstr[0] = 0;
	stext[y]._sline = 1;
}

void AddSTextVal(int y, int val)
{
	stext[y]._sval = val;
}

void OffsetSTextY(int y, int yo)
{
	stext[y]._syoff = yo;
}

void AddSText(int x, int y, int j, char *str, char clr, int sel)
{
	stext[y]._sx = x;
	stext[y]._syoff = 0;
	strcpy(stext[y]._sstr, str);
	stext[y]._sjust = j;
	stext[y]._sclr = clr;
	stext[y]._sline = 0;
	stext[y]._ssel = sel;
}

void StoreAutoPlace()
{
	BOOL done;
	int i, w, h, idx;

	SetICursor(plr[myplr].HoldItem._iCurs + CURSOR_FIRSTITEM);
	done = FALSE;
	w = icursW28;
	h = icursH28;
	if (w == 1 && h == 1) {
		idx = plr[myplr].HoldItem.IDidx;
		if (plr[myplr].HoldItem._iStatFlag && AllItemsList[idx].iUsable) {
			for (i = 0; i < 8 && !done; i++) {
				if (plr[myplr].SpdList[i]._itype == ITYPE_NONE) {
					plr[myplr].SpdList[i] = plr[myplr].HoldItem;
					done = TRUE;
				}
			}
		}
		for (i = 30; i <= 39 && !done; i++) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
		for (i = 20; i <= 29 && !done; i++) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
		for (i = 10; i <= 19 && !done; i++) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
		for (i = 0; i <= 9 && !done; i++) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
	}
	if (w == 1 && h == 2) {
		for (i = 29; i >= 20 && !done; i--) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
		for (i = 9; i >= 0 && !done; i--) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
		for (i = 19; i >= 10 && !done; i--) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
	}
	if (w == 1 && h == 3) {
		for (i = 0; i < 20 && !done; i++) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
	}
	if (w == 2 && h == 2) {
		for (i = 0; i < 10 && !done; i++) {
			done = AutoPlace(myplr, AP2x2Tbl[i], w, h, TRUE);
		}
		for (i = 21; i < 29 && !done; i += 2) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
		for (i = 1; i < 9 && !done; i += 2) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
		for (i = 10; i < 19 && !done; i++) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
	}
	if (w == 2 && h == 3) {
		for (i = 0; i < 9 && !done; i++) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
		for (i = 10; i < 19 && !done; i++) {
			done = AutoPlace(myplr, i, w, h, TRUE);
		}
	}
}
// 48E9A8: using guessed type int AP2x2Tbl[10];

void S_StartSmith()
{
	stextsize = 0;
	stextscrl = FALSE;
	AddSText(0, 1, 1u, "Welcome to the", COL_GOLD, 0);
	AddSText(0, 3, 1u, "Blacksmith's shop", COL_GOLD, 0);
	AddSText(0, 7, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 10, 1u, "Talk to Griswold", COL_BLUE, 1);
	AddSText(0, 12, 1u, "Buy basic items", COL_WHITE, 1);
	AddSText(0, 14, 1u, "Buy premium items", COL_WHITE, 1);
	AddSText(0, 16, 1u, "Sell items", COL_WHITE, 1);
	AddSText(0, 18, 1u, "Repair items", COL_WHITE, 1);
	AddSText(0, 20, 1u, "Leave the shop", COL_WHITE, 1);
	AddSLine(5);
	storenumh = 20;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;

void S_ScrollSBuy(int idx)
{
	int l, ls;
	char iclr;

	ls = idx;
	ClearSText(5, 21);
	stextup = 5;

	for (l = 5; l < 20; l += 4) {
		if (smithitem[ls]._itype != ITYPE_NONE) {
			iclr = COL_WHITE;
			if (smithitem[ls]._iMagical) {
				iclr = COL_BLUE;
			}

			if (!smithitem[ls]._iStatFlag) {
				iclr = COL_RED;
			}

			if (smithitem[ls]._iMagical) {
				AddSText(20, l, 0, smithitem[ls]._iIName, iclr, 1);
			} else {
				AddSText(20, l, 0, smithitem[ls]._iName, iclr, 1);
			}

			AddSTextVal(l, smithitem[ls]._iIvalue);
			PrintStoreItem(&smithitem[ls], l + 1, iclr);
			stextdown = l;
			ls++;
		}
	}

	if (!stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}
// 69F108: using guessed type int stextup;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

void PrintStoreItem(ItemStruct *x, int l, char iclr)
{
	char sstr[128];

	sstr[0] = '\0';
	if (x->_iIdentified) {
		if (x->_iMagical != ITEM_QUALITY_UNIQUE) {
			if (x->_iPrePower != -1) {
				PrintItemPower(x->_iPrePower, x);
				strcat(sstr, tempstr);
			}
		}
		if (x->_iSufPower != -1) {
			PrintItemPower(x->_iSufPower, x);
			if (sstr[0])
				strcat(sstr, ",  ");
			strcat(sstr, tempstr);
		}
	}
	if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges) {
		sprintf(tempstr, "Charges: %i/%i", x->_iCharges, x->_iMaxCharges);
		if (sstr[0])
			strcat(sstr, ",  ");
		strcat(sstr, tempstr);
	}
	if (sstr[0]) {
		AddSText(40, l, 0, sstr, iclr, 0);
		l++;
	}
	sstr[0] = '\0';
	if (x->_iClass == ICLASS_WEAPON)
		sprintf(sstr, "Damage: %i-%i  ", x->_iMinDam, x->_iMaxDam);
	if (x->_iClass == ICLASS_ARMOR)
		sprintf(sstr, "Armor: %i  ", x->_iAC);
	if (x->_iMaxDur != 255 && x->_iMaxDur) {
		sprintf(tempstr, "Dur: %i/%i,  ", x->_iDurability, x->_iMaxDur);
		strcat(sstr, tempstr);
	} else {
		strcat(sstr, "Indestructible,  ");
	}
	if (!x->_itype)
		sstr[0] = '\0';
	if (!(x->_iMinStr + x->_iMinMag + x->_iMinDex)) {
		strcat(sstr, "No required attributes");
	} else {
		strcpy(tempstr, "Required:");
		if (x->_iMinStr)
			sprintf(tempstr, "%s %i Str", tempstr, x->_iMinStr);
		if (x->_iMinMag)
			sprintf(tempstr, "%s %i Mag", tempstr, x->_iMinMag);
		if (x->_iMinDex)
			sprintf(tempstr, "%s %i Dex", tempstr, x->_iMinDex);
		strcat(sstr, tempstr);
	}
	AddSText(40, l, 0, sstr, iclr, 0);
	if (x->_iMagical == ITEM_QUALITY_UNIQUE) {
		if (x->_iIdentified)
			AddSText(40, l + 1, 0, "Unique Item", iclr, 0);
	}
}

void S_StartSBuy()
{
	int i;

	stextsize = 1;
	stextscrl = TRUE;
	stextsval = 0;
	sprintf(tempstr, "I have these items for sale :           Your gold : %i", plr[myplr]._pGold);
	AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSBuy(stextsval);
	AddSText(0, 22, 1, "Back", COL_WHITE, 0);
	OffsetSTextY(22, 6);
	storenumh = 0;
	for (i = 0; smithitem[i]._itype != -1; i++)
		storenumh++;
	stextsmax = storenumh - 4;
	if (stextsmax < 0)
		stextsmax = 0;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;

void S_ScrollSPBuy(int idx)
{
	int v1;   // esi
	int v2;   // edi
	int v3;   // eax
	int v4;   // esi
	int *v5;  // ecx
	char *v6; // esi
	int iclr; // [esp+Ch] [ebp-4h]

	v1 = idx;
	v2 = 5;
	ClearSText(5, 21);
	v3 = v1;
	v4 = 0;
	stextup = 5;
	if (v3) {
		v5 = &premiumitem[0]._itype;
		do {
			if (*v5 != -1)
				--v3;
			++v4;
			v5 += 92;
		} while (v3);
	}
	v6 = &premiumitem[v4]._iMagical;
	do {
		if ((signed int)v6 >= (signed int)&premiumitem[6]._iMagical)
			break;
		if (*((_DWORD *)v6 - 13) == -1) {
			v2 -= 4;
		} else {
			_LOBYTE(iclr) = COL_WHITE;
			if (*v6)
				_LOBYTE(iclr) = COL_BLUE;
			if (!*((_DWORD *)v6 + 74))
				_LOBYTE(iclr) = COL_RED;
			AddSText(20, v2, 0, v6 + 65, iclr, 1);
			AddSTextVal(v2, *((_DWORD *)v6 + 35));
			PrintStoreItem((ItemStruct *)(v6 - 60), v2 + 1, iclr);
			stextdown = v2;
		}
		v2 += 4;
		v6 += 368;
	} while (v2 < 20);
	if (!stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}
// 69F108: using guessed type int stextup;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

BOOL S_StartSPBuy()
{
	int i;

	storenumh = 0;
	for (i = 0; i < 6; i++) {
		if (premiumitem[i]._itype != -1)
			storenumh++;
	}
	if (!storenumh) {
		StartStore(STORE_SMITH);
		stextsel = 14;
		return FALSE;
	}

	stextsize = 1;
	stextscrl = TRUE;
	stextsval = 0;

	sprintf(tempstr, "I have these premium items for sale :   Your gold : %i", plr[myplr]._pGold);
	AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
	AddSLine(3);
	AddSLine(21);
	AddSText(0, 22, 1, "Back", COL_WHITE, 0);
	OffsetSTextY(22, 6);

	stextsmax = storenumh - 4;
	if (stextsmax < 0)
		stextsmax = 0;

	S_ScrollSPBuy(stextsval);

	return TRUE;
}
// 69F10C: using guessed type int storenumh;
// 69FB38: using guessed type int talker;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A8A28: using guessed type int stextsel;

BOOL SmithSellOk(int i)
{
	if (plr[myplr].InvList[i]._itype == ITYPE_NONE)
		return FALSE;
	if (plr[myplr].InvList[i]._itype == ITYPE_MISC)
		return FALSE;
	if (plr[myplr].InvList[i]._itype == ITYPE_GOLD)
		return FALSE;
	if (plr[myplr].InvList[i]._itype == ITYPE_0E)
		return FALSE;
	if (plr[myplr].InvList[i]._itype == ITYPE_STAFF)
		return FALSE;
	if (plr[myplr].InvList[i].IDidx == IDI_LAZSTAFF)
		return FALSE;

	return TRUE;
}

void S_ScrollSSell(int idx)
{
	int l;
	char iclr;

	ClearSText(5, 21);
	stextup = 5;

	for (l = 5; l < 20; l += 4) {
		if (idx >= storenumh)
			break;
		if (storehold[idx]._itype != ITYPE_NONE) {
			iclr = COL_WHITE;
			if (storehold[idx]._iMagical) {
				iclr = COL_BLUE;
			}

			if (!storehold[idx]._iStatFlag) {
				iclr = COL_RED;
			}

			if (storehold[idx]._iMagical && storehold[idx]._iIdentified) {
				AddSText(20, l, 0, storehold[idx]._iIName, iclr, 1);
				AddSTextVal(l, storehold[idx]._iIvalue);
			} else {
				AddSText(20, l, 0, storehold[idx]._iName, iclr, 1);
				AddSTextVal(l, storehold[idx]._ivalue);
			}

			PrintStoreItem(&storehold[idx], l + 1, iclr);
			stextdown = l;
		}
		idx++;
	}

	stextsmax = storenumh - 4;
	if (stextsmax < 0)
		stextsmax = 0;
}
// 69F108: using guessed type int stextup;
// 69F10C: using guessed type int storenumh;
// 6A09E4: using guessed type int stextsmax;
// 6AA700: using guessed type int stextdown;

void S_StartSSell()
{
	int i;
	BOOL sellok;

	stextsize = 1;
	sellok = FALSE;
	storenumh = 0;

	for (i = 0; i < 48; i++)
		storehold[i]._itype = -1;

	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (SmithSellOk(i)) {
			sellok = TRUE;
			storehold[storenumh] = plr[myplr].InvList[i];

			if (storehold[storenumh]._iMagical != ITEM_QUALITY_NORMAL && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			if (!(storehold[storenumh]._ivalue >>= 2))
				storehold[storenumh]._ivalue = 1;

			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh++] = i;
		}
	}

	if (!sellok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing I want.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = TRUE;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Which item is for sale?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	}
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;

BOOL SmithRepairOk(int i)
{
	if (plr[myplr].InvList[i]._itype == ITYPE_NONE)
		return FALSE;
	if (plr[myplr].InvList[i]._itype == ITYPE_MISC)
		return FALSE;
	if (plr[myplr].InvList[i]._itype == ITYPE_GOLD)
		return FALSE;
	if (plr[myplr].InvList[i]._itype == ITYPE_0E)
		return FALSE;
	if (plr[myplr].InvList[i]._iDurability == plr[myplr].InvList[i]._iMaxDur)
		return FALSE;

	return TRUE;
}

void S_StartSRepair()
{
	BOOL repairok;
	int i;

	stextsize = 1;
	repairok = FALSE;
	storenumh = 0;
	for (i = 0; i < 40; i++)
		storehold[i]._itype = ITYPE_NONE;
	if (plr[myplr].InvBody[INVLOC_HEAD]._itype != ITYPE_NONE && plr[myplr].InvBody[INVLOC_HEAD]._iDurability != plr[myplr].InvBody[INVLOC_HEAD]._iMaxDur) {
		repairok = TRUE;
		AddStoreHoldRepair(plr[myplr].InvBody, -1);
	}
	if (plr[myplr].InvBody[INVLOC_CHEST]._itype != ITYPE_NONE && plr[myplr].InvBody[INVLOC_CHEST]._iDurability != plr[myplr].InvBody[INVLOC_CHEST]._iMaxDur) {
		repairok = TRUE;
		AddStoreHoldRepair(&plr[myplr].InvBody[INVLOC_CHEST], -2);
	}
	if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && plr[myplr].InvBody[INVLOC_HAND_LEFT]._iDurability != plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMaxDur) {
		repairok = TRUE;
		AddStoreHoldRepair(&plr[myplr].InvBody[INVLOC_HAND_LEFT], -3);
	}
	if (plr[myplr].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE && plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iDurability != plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iMaxDur) {
		repairok = TRUE;
		AddStoreHoldRepair(&plr[myplr].InvBody[INVLOC_HAND_RIGHT], -4);
	}
	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (SmithRepairOk(i)) {
			repairok = TRUE;
			AddStoreHoldRepair(&plr[myplr].InvList[i], i);
		}
	}
	if (!repairok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing to repair.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, 3, 0);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, 1, "Back", 0, 1);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = TRUE;
	stextsval = 0;
	stextsmax = plr[myplr]._pNumInv;
	sprintf(tempstr, "Repair which item?            Your gold : %i", plr[myplr]._pGold);
	AddSText(0, 1, 1, tempstr, 3, 0);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSSell(stextsval);
	AddSText(0, 22, 1, "Back", 0, 1);
	OffsetSTextY(22, 6);
}

void AddStoreHoldRepair(ItemStruct *itm, int i)
{
	ItemStruct *item;
	int v;

	item = &storehold[storenumh];
	storehold[storenumh] = *itm;
	if (item->_iMagical != ITEM_QUALITY_NORMAL && item->_iIdentified)
		item->_ivalue = 30 * item->_iIvalue / 100;
	v = item->_ivalue * (100 * (item->_iMaxDur - item->_iDurability) / item->_iMaxDur) / 100;
	if (!v) {
		if (item->_iMagical != ITEM_QUALITY_NORMAL && item->_iIdentified)
			return;
		v = 1;
	}
	if (v > 1)
		v >>= 1;
	item->_iIvalue = v;
	item->_ivalue = v;
	storehidx[storenumh] = i;
	storenumh++;
}
// 69F10C: using guessed type int storenumh;

void S_StartWitch()
{
	stextsize = 0;
	stextscrl = FALSE;
	AddSText(0, 2, 1u, "Witch's shack", COL_GOLD, 0);
	AddSText(0, 9, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 12, 1u, "Talk to Adria", COL_BLUE, 1);
	AddSText(0, 14, 1u, "Buy items", COL_WHITE, 1);
	AddSText(0, 16, 1u, "Sell items", COL_WHITE, 1);
	AddSText(0, 18, 1u, "Recharge staves", COL_WHITE, 1);
	AddSText(0, 20, 1u, "Leave the shack", COL_WHITE, 1);
	AddSLine(5);
	storenumh = 20;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;

void S_ScrollWBuy(int idx)
{
	int l, ls;
	char iclr;

	ls = idx;
	ClearSText(5, 21);
	stextup = 5;

	for (l = 5; l < 20; l += 4) {
		if (witchitem[ls]._itype != ITYPE_NONE) {
			iclr = COL_WHITE;
			if (witchitem[ls]._iMagical) {
				iclr = COL_BLUE;
			}

			if (!witchitem[ls]._iStatFlag) {
				iclr = COL_RED;
			}

			if (witchitem[ls]._iMagical) {
				AddSText(20, l, 0, witchitem[ls]._iIName, iclr, 1);
			} else {
				AddSText(20, l, 0, witchitem[ls]._iName, iclr, 1);
			}

			AddSTextVal(l, witchitem[ls]._iIvalue);
			PrintStoreItem(&witchitem[ls], l + 1, iclr);
			stextdown = l;
			ls++;
		}
	}

	if (!stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}

void S_StartWBuy()
{
	int v0;  // ST10_4
	int v1;  // eax
	int *v2; // ecx

	v0 = plr[myplr]._pGold;
	stextsize = 1;
	stextscrl = TRUE;
	stextsval = 0;
	stextsmax = 20;
	sprintf(tempstr, "I have these items for sale :           Your gold : %i", v0);
	AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
	AddSLine(3);
	AddSLine(21);
	S_ScrollWBuy(stextsval);
	AddSText(0, 22, 1u, "Back", COL_WHITE, 0);
	OffsetSTextY(22, 6);
	v1 = 0;
	storenumh = 0;
	if (witchitem[0]._itype != -1) {
		v2 = &witchitem[0]._itype;
		do {
			v2 += 92;
			++v1;
		} while (*v2 != -1);
		storenumh = v1;
	}
	stextsmax = v1 - 4;
	if (v1 - 4 < 0)
		stextsmax = 0;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;

BOOL WitchSellOk(int i)
{
	BOOL rv;
	ItemStruct *pI;

	rv = FALSE;

	if (i >= 0)
		pI = &plr[myplr].InvList[i];
	else
		pI = &plr[myplr].SpdList[-(i + 1)];

	if (pI->_itype == ITYPE_MISC)
		rv = TRUE;
	if (pI->_itype == ITYPE_STAFF)
		rv = TRUE;
	if (pI->IDidx >= IDI_FIRSTQUEST && pI->IDidx <= IDI_LASTQUEST)
		rv = FALSE;
	if (pI->IDidx == IDI_LAZSTAFF)
		rv = FALSE;
	return rv;
}

void S_StartWSell()
{
	int i;
	BOOL sellok;

	stextsize = 1;
	sellok = FALSE;
	storenumh = 0;

	for (i = 0; i < 48; i++)
		storehold[i]._itype = -1;

	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (WitchSellOk(i)) {
			sellok = TRUE;
			storehold[storenumh] = plr[myplr].InvList[i];

			if (storehold[storenumh]._iMagical != ITEM_QUALITY_NORMAL && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			if (!(storehold[storenumh]._ivalue >>= 2))
				storehold[storenumh]._ivalue = 1;

			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh++] = i;
		}
	}

	for (i = 0; i < MAXBELTITEMS; i++) {
		if (plr[myplr].SpdList[i]._itype != -1 && WitchSellOk(-(i + 1))) {
			sellok = TRUE;
			storehold[storenumh] = plr[myplr].SpdList[i];

			if (storehold[storenumh]._iMagical != ITEM_QUALITY_NORMAL && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			if (!(storehold[storenumh]._ivalue >>= 2))
				storehold[storenumh]._ivalue = 1;

			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh++] = -(i + 1);
		}
	}

	if (!sellok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing I want.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = TRUE;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Which item is for sale?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	}
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;

BOOL WitchRechargeOk(int i)
{
	BOOL rv;

	rv = FALSE;
	if (plr[myplr].InvList[i]._itype == ITYPE_STAFF
	    && plr[myplr].InvList[i]._iCharges != plr[myplr].InvList[i]._iMaxCharges) {
		rv = TRUE;
	}
	return rv;
}

void AddStoreHoldRecharge(ItemStruct itm, int i)
{
	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue += spelldata[itm._iSpell].sStaffCost;
	storehold[storenumh]._ivalue = storehold[storenumh]._ivalue * (100 * (storehold[storenumh]._iMaxCharges - storehold[storenumh]._iCharges) / storehold[storenumh]._iMaxCharges) / 100 >> 1;
	storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
	storehidx[storenumh] = i;
	storenumh++;
}
// 69F108: using guessed type int stextup;
// 69F10C: using guessed type int storenumh;

void S_StartWRecharge()
{
	int i;
	BOOL rechargeok;

	stextsize = 1;
	rechargeok = FALSE;
	storenumh = 0;

	for (i = 0; i < 48; i++) {
		storehold[i]._itype = ITYPE_NONE;
	}

	if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF
	    && plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges != plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMaxCharges) {
		rechargeok = TRUE;
		AddStoreHoldRecharge(plr[myplr].InvBody[INVLOC_HAND_LEFT], -1);
	}

	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (WitchRechargeOk(i)) {
			rechargeok = TRUE;
			AddStoreHoldRecharge(plr[myplr].InvList[i], i);
		}
	}

	if (!rechargeok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing to recharge.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = TRUE;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Recharge which item?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	}
}

void S_StartNoMoney()
{
	StartStore((unsigned char)stextshold);
	stextscrl = FALSE;
	stextsize = 1;
	ClearSText(5, 23);
	AddSText(0, 14, 1u, "You do not have enough gold", COL_WHITE, 1);
}
// 6A09E0: using guessed type char stextsize;

void S_StartNoRoom()
{
	StartStore((unsigned char)stextshold);
	stextscrl = FALSE;
	ClearSText(5, 23);
	AddSText(0, 14, 1u, "You do not have enough room in inventory", COL_WHITE, 1);
}

void S_StartConfirm()
{
	BOOL idprint;
	char iclr;

	StartStore(stextshold);
	stextscrl = FALSE;
	ClearSText(5, 23);
	iclr = COL_WHITE;

	if (plr[myplr].HoldItem._iMagical != ITEM_QUALITY_NORMAL)
		iclr = COL_BLUE;
	if (!plr[myplr].HoldItem._iStatFlag)
		iclr = COL_RED;

	idprint = plr[myplr].HoldItem._iMagical != ITEM_QUALITY_NORMAL;

	if (stextshold == STORE_SIDENTIFY)
		idprint = FALSE;
	if (plr[myplr].HoldItem._iMagical != ITEM_QUALITY_NORMAL && !plr[myplr].HoldItem._iIdentified) {
		if (stextshold == STORE_SSELL)
			idprint = FALSE;
		if (stextshold == STORE_WSELL)
			idprint = FALSE;
		if (stextshold == STORE_SREPAIR)
			idprint = FALSE;
		if (stextshold == STORE_WRECHARGE)
			idprint = FALSE;
	}
	if (idprint)
		AddSText(20, 8, 0, plr[myplr].HoldItem._iIName, iclr, 0);
	else
		AddSText(20, 8, 0, plr[myplr].HoldItem._iName, iclr, 0);

	AddSTextVal(8, plr[myplr].HoldItem._iIvalue);
	PrintStoreItem(&plr[myplr].HoldItem, 9, iclr);

	switch (stextshold) {
	case STORE_BBOY:
		strcpy(tempstr, "Do we have a deal?");
		break;
	case STORE_SIDENTIFY:
		strcpy(tempstr, "Are you sure you want to identify this item?");
		break;
	case STORE_HBUY:
	case STORE_SPBUY:
	case STORE_WBUY:
	case STORE_SBUY:
		strcpy(tempstr, "Are you sure you want to buy this item?");
		break;
	case STORE_WRECHARGE:
		strcpy(tempstr, "Are you sure you want to recharge this item?");
		break;
	case STORE_SSELL:
	case STORE_WSELL:
		strcpy(tempstr, "Are you sure you want to sell this item?");
		break;
	case STORE_SREPAIR:
		strcpy(tempstr, "Are you sure you want to repair this item?");
		break;
	}
	AddSText(0, 15, 1u, tempstr, COL_WHITE, 0);
	AddSText(0, 18, 1u, "Yes", COL_WHITE, 1);
	AddSText(0, 20, 1u, "No", COL_WHITE, 1);
}

void S_StartBoy()
{
	stextsize = 0;
	stextscrl = FALSE;
	AddSText(0, 2, 1u, "Wirt the Peg-legged boy", COL_GOLD, 0);
	AddSLine(5);
	if (boyitem._itype != -1) {
		AddSText(0, 8, 1u, "Talk to Wirt", COL_BLUE, 1);
		AddSText(0, 12, 1u, "I have something for sale,", COL_GOLD, 0);
		AddSText(0, 14, 1u, "but it will cost 50 gold", COL_GOLD, 0);
		AddSText(0, 16, 1u, "just to take a look. ", COL_GOLD, 0);
		AddSText(0, 18, 1u, "What have you got?", COL_WHITE, 1);
		AddSText(0, 20, 1u, "Say goodbye", COL_WHITE, 1);
	} else {
		AddSText(0, 12, 1u, "Talk to Wirt", COL_BLUE, 1);
		AddSText(0, 18, 1u, "Say goodbye", COL_WHITE, 1);
	}
}
// 6A09E0: using guessed type char stextsize;

void S_StartBBoy()
{
	int iclr; // esi

	stextsize = 1;
	stextscrl = FALSE;
	sprintf(tempstr, "I have this item for sale :           Your gold : %i", plr[myplr]._pGold);
	AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
	AddSLine(3);
	AddSLine(21);
	iclr = COL_WHITE;

	if (boyitem._iMagical != ITEM_QUALITY_NORMAL)
		iclr = COL_BLUE;
	if (!boyitem._iStatFlag)
		iclr = COL_RED;
	if (boyitem._iMagical != ITEM_QUALITY_NORMAL)
		AddSText(20, 10, 0, boyitem._iIName, iclr, 1);
	else
		AddSText(20, 10, 0, boyitem._iName, iclr, 1);

	AddSTextVal(10, boyitem._iIvalue + (boyitem._iIvalue >> 1));
	PrintStoreItem(&boyitem, 11, iclr);
	AddSText(0, 22, 1u, "Leave", COL_WHITE, 1);
	OffsetSTextY(22, 6);
}
// 6A09E0: using guessed type char stextsize;

void S_StartHealer()
{
	stextsize = 0;
	stextscrl = FALSE;
	AddSText(0, 1, 1u, "Welcome to the", COL_GOLD, 0);
	AddSText(0, 3, 1u, "Healer's home", COL_GOLD, 0);
	AddSText(0, 9, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 12, 1u, "Talk to Pepin", COL_BLUE, 1);
	AddSText(0, 14, 1u, "Receive healing", COL_WHITE, 1);
	AddSText(0, 16, 1u, "Buy items", COL_WHITE, 1);
	AddSText(0, 18, 1u, "Leave Healer's home", COL_WHITE, 1);
	AddSLine(5);
	storenumh = 20;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;

void S_ScrollHBuy(int idx)
{
	int l;
	char iclr;

	ClearSText(5, 21);
	stextup = 5;
	for (l = 5; l < 20; l += 4) {
		if (healitem[idx]._itype != ITYPE_NONE) {
			iclr = COL_WHITE;
			if (!healitem[idx]._iStatFlag) {
				iclr = COL_RED;
			}

			AddSText(20, l, 0, healitem[idx]._iName, iclr, 1);
			AddSTextVal(l, healitem[idx]._iIvalue);
			PrintStoreItem(&healitem[idx], l + 1, iclr);
			stextdown = l;
			idx++;
		}
	}

	if (!stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}
// 69F108: using guessed type int stextup;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

void S_StartHBuy()
{
	int v0;  // ST10_4
	int v1;  // eax
	int *v2; // ecx

	v0 = plr[myplr]._pGold;
	stextsize = 1;
	stextscrl = TRUE;
	stextsval = 0;
	sprintf(tempstr, "I have these items for sale :           Your gold : %i", v0);
	AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
	AddSLine(3);
	AddSLine(21);
	S_ScrollHBuy(stextsval);
	AddSText(0, 22, 1u, "Back", COL_WHITE, 0);
	OffsetSTextY(22, 6);
	v1 = 0;
	storenumh = 0;
	if (healitem[0]._itype != -1) {
		v2 = &healitem[0]._itype;
		do {
			v2 += 92;
			++v1;
		} while (*v2 != -1);
		storenumh = v1;
	}
	stextsmax = v1 - 4;
	if (v1 - 4 < 0)
		stextsmax = 0;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;

void S_StartStory()
{
	stextsize = 0;
	stextscrl = FALSE;
	AddSText(0, 2, 1u, "The Town Elder", COL_GOLD, 0);
	AddSText(0, 9, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 12, 1u, "Talk to Cain", COL_BLUE, 1);
	AddSText(0, 14, 1u, "Identify an item", COL_WHITE, 1);
	AddSText(0, 18, 1u, "Say goodbye", COL_WHITE, 1);
	AddSLine(5);
}
// 6A09E0: using guessed type char stextsize;

BOOL IdItemOk(ItemStruct *i)
{
	if (i->_itype == -1) {
		return FALSE;
	}
	if (i->_iMagical == ITEM_QUALITY_NORMAL) {
		return FALSE;
	}
	return !i->_iIdentified;
}

void AddStoreHoldId(ItemStruct itm, int i)
{
	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue = 100;
	storehold[storenumh]._iIvalue = 100;
	storehidx[storenumh] = i;
	storenumh++;
}
// 69F108: using guessed type int stextup;
// 69F10C: using guessed type int storenumh;

void S_StartSIdentify()
{
	BOOL idok;
	int i;

	idok = FALSE;
	stextsize = 1;
	storenumh = 0;

	for (i = 0; i < 48; i++)
		storehold[i]._itype = -1;

	if (IdItemOk(&plr[myplr].InvBody[INVLOC_HEAD])) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].InvBody[INVLOC_HEAD], -1);
	}
	if (IdItemOk(&plr[myplr].InvBody[INVLOC_CHEST])) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].InvBody[INVLOC_CHEST], -2);
	}
	if (IdItemOk(&plr[myplr].InvBody[INVLOC_HAND_LEFT])) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].InvBody[INVLOC_HAND_LEFT], -3);
	}
	if (IdItemOk(&plr[myplr].InvBody[INVLOC_HAND_RIGHT])) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].InvBody[INVLOC_HAND_RIGHT], -4);
	}
	if (IdItemOk(&plr[myplr].InvBody[INVLOC_RING_LEFT])) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].InvBody[INVLOC_RING_LEFT], -5);
	}
	if (IdItemOk(&plr[myplr].InvBody[INVLOC_RING_RIGHT])) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].InvBody[INVLOC_RING_RIGHT], -6);
	}
	if (IdItemOk(&plr[myplr].InvBody[INVLOC_AMULET])) {
		idok = TRUE;
		AddStoreHoldId(plr[myplr].InvBody[INVLOC_AMULET], -7);
	}

	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (IdItemOk(&plr[myplr].InvList[i])) {
			idok = TRUE;
			AddStoreHoldId(plr[myplr].InvList[i], i);
		}
	}

	if (!idok) {
		stextscrl = FALSE;
		sprintf(tempstr, "You have nothing to identify.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	} else {
		stextscrl = TRUE;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Identify which item?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	}
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;

void S_StartIdShow()
{
	char iclr; // [esp+4h] [ebp-4h]

	StartStore(stextshold);
	stextscrl = FALSE;
	ClearSText(5, 23);
	iclr = COL_WHITE;

	if (plr[myplr].HoldItem._iMagical != ITEM_QUALITY_NORMAL)
		iclr = COL_BLUE;
	if (!plr[myplr].HoldItem._iStatFlag)
		iclr = COL_RED;

	AddSText(0, 7, 1u, "This item is:", COL_WHITE, 0);
	AddSText(20, 11, 0, plr[myplr].HoldItem._iIName, iclr, 0);
	PrintStoreItem(&plr[myplr].HoldItem, 12, iclr);
	AddSText(0, 18, 1u, "Done", COL_WHITE, 1);
}

void S_StartTalk()
{
	int *v0;       // edi
	signed int v1; // eax
	int v2;        // edx
	int *v3;       // ecx
	char **v4;     // ebp
	int v5;        // esi
	int v6;        // ebx
	signed int v7; // [esp-4h] [ebp-1Ch]
	signed int v8; // [esp+10h] [ebp-8h]
	int y;         // [esp+14h] [ebp-4h]

	stextsize = 0;
	stextscrl = FALSE;
	sprintf(tempstr, "Talk to %s", talkname[talker]);
	AddSText(0, 2, 1u, tempstr, COL_GOLD, 0);
	AddSLine(5);
	v0 = &quests[0]._qlog;
	v1 = 0;
	v2 = 0;
	v3 = &quests[0]._qlog;
	do {
		if (*((_BYTE *)v3 - 18) == 2 && *((_DWORD *)&Qtalklist[0]._qinfra + v2 + 16 * talker) != -1 && *v3)
			++v1;
		v3 += 6;
		++v2;
	} while ((signed int)v3 < (signed int)&quests[16]._qlog);
	if (v1 <= 6) {
		v7 = 15;
		v8 = 2;
	} else {
		v1 >>= 1;
		v7 = 14;
		v8 = 1;
	}
	v4 = &questlist[0]._qlstr;
	v5 = v7 - v1;
	v6 = 0;
	y = v7 - v1 - 2;
	do {
		if (*((_BYTE *)v0 - 18) == 2 && *((_DWORD *)&Qtalklist[0]._qinfra + v6 + 16 * talker) != -1 && *v0) {
			AddSText(0, v5, 1u, *v4, COL_WHITE, 1);
			v5 += v8;
		}
		v0 += 6;
		++v6;
		v4 += 5;
	} while ((signed int)v0 < (signed int)&quests[16]._qlog);
	AddSText(0, y, 1u, "Gossip", COL_BLUE, 1);
	AddSText(0, 22, 1u, "Back", COL_WHITE, 1);
}
// 69FB38: using guessed type int talker;
// 6A09E0: using guessed type char stextsize;

void S_StartTavern()
{
	stextsize = 0;
	stextscrl = FALSE;
	AddSText(0, 1, 1u, "Welcome to the", COL_GOLD, 0);
	AddSText(0, 3, 1u, "Rising Sun", COL_GOLD, 0);
	AddSText(0, 9, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 12, 1u, "Talk to Ogden", COL_BLUE, 1);
	AddSText(0, 18, 1u, "Leave the tavern", COL_WHITE, 1);
	AddSLine(5);
	storenumh = 20;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;

void S_StartBarMaid()
{
	stextsize = 0;
	stextscrl = FALSE;
	AddSText(0, 2, 1u, "Gillian", COL_GOLD, 0);
	AddSText(0, 9, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 12, 1u, "Talk to Gillian", COL_BLUE, 1);
	AddSText(0, 18, 1u, "Say goodbye", COL_WHITE, 1);
	AddSLine(5);
	storenumh = 20;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;

void S_StartDrunk()
{
	stextsize = 0;
	stextscrl = FALSE;
	AddSText(0, 2, 1u, "Farnham the Drunk", COL_GOLD, 0);
	AddSText(0, 9, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 12, 1u, "Talk to Farnham", COL_BLUE, 1);
	AddSText(0, 18, 1u, "Say Goodbye", COL_WHITE, 1);
	AddSLine(5);
	storenumh = 20;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;

void StartStore(char s)
{
	char t; // bl
	int i;  // ecx

	for (t = s;; t = STORE_SMITH) {
		sbookflag = 0;
		invflag = 0;
		chrflag = 0;
		questlog = FALSE;
		dropGoldFlag = FALSE;
		ClearSText(0, 24);
		ReleaseStoreBtn();
		switch (t) {
		case STORE_SMITH:
			S_StartSmith();
			break;
		case STORE_SBUY:
			if (storenumh > 0)
				S_StartSBuy();
			break;
		case STORE_SSELL:
			S_StartSSell();
			break;
		case STORE_SREPAIR:
			S_StartSRepair();
			break;
		case STORE_WITCH:
			S_StartWitch();
			break;
		case STORE_WBUY:
			if (storenumh > 0)
				S_StartWBuy();
			break;
		case STORE_WSELL:
			S_StartWSell();
			break;
		case STORE_WRECHARGE:
			S_StartWRecharge();
			break;
		case STORE_NOMONEY:
			S_StartNoMoney();
			break;
		case STORE_NOROOM:
			S_StartNoRoom();
			break;
		case STORE_CONFIRM:
			S_StartConfirm();
			break;
		case STORE_BOY:
			S_StartBoy();
			break;
		case STORE_BBOY:
			S_StartBBoy();
			break;
		case STORE_HEALER:
			S_StartHealer();
			break;
		case STORE_STORY:
			S_StartStory();
			break;
		case STORE_HBUY:
			if (storenumh > 0)
				S_StartHBuy();
			break;
		case STORE_SIDENTIFY:
			S_StartSIdentify();
			break;
		case STORE_SPBUY:
			if (!S_StartSPBuy())
				return;
			break;
		case STORE_GOSSIP:
			S_StartTalk();
			break;
		case STORE_IDSHOW:
			S_StartIdShow();
			break;
		case STORE_TAVERN:
			S_StartTavern();
			break;
		case STORE_DRUNK:
			S_StartDrunk();
			break;
		case STORE_BARMAID:
			S_StartBarMaid();
			break;
		}

		for (i = 0; i < 24; i++) {
			if (stext[i]._ssel)
				break;
		}

		stextsel = i == 24 ? -1 : i;
		stextflag = t;
		if (t != STORE_SBUY || storenumh)
			break;
	}
}
// 4B8968: using guessed type int sbookflag;
// 69F10C: using guessed type int storenumh;
// 6A8A28: using guessed type int stextsel;
// 6AA705: using guessed type char stextflag;

void DrawSText()
{
	int i; // edi

	if (stextsize)
		DrawQTextBack();
	else
		DrawSTextBack();
	if (!stextscrl)
		goto LABEL_19;
	if (stextflag > (signed int)STORE_WRECHARGE) {
		switch (stextflag) {
		case STORE_HBUY:
			S_ScrollHBuy(stextsval);
			break;
		case STORE_SIDENTIFY:
			goto LABEL_17;
		case STORE_SPBUY:
			S_ScrollSPBuy(stextsval);
			break;
		}
	} else {
		if (stextflag >= (signed int)STORE_WSELL)
			goto LABEL_17;
		if (stextflag == STORE_SBUY) {
			S_ScrollSBuy(stextsval);
			goto LABEL_19;
		}
		if (stextflag > (signed int)STORE_SBUY) {
			if (stextflag > (signed int)STORE_SREPAIR) {
				if (stextflag == STORE_WBUY)
					S_ScrollWBuy(stextsval);
				goto LABEL_19;
			}
		LABEL_17:
			S_ScrollSSell(stextsval);
			goto LABEL_19;
		}
	}
LABEL_19:

	for (i = 0; i < 24; i++) {
		if (stext[i]._sline)
			DrawSLine(i);
		if (stext[i]._sstr)
			PrintSString(stext[i]._sx, i, stext[i]._sjust, stext[i]._sstr, stext[i]._sclr, stext[i]._sval);
	}

	if (stextscrl)
		DrawSArrows(4, 20);
	InStoreFlag = (InStoreFlag & 7) + 1;
}
// 6A09E0: using guessed type char stextsize;
// 6AA705: using guessed type char stextflag;

void STextESC()
{
	char v0; // cl
	char v1; // cl
	char v2; // cl

	if (qtextflag) {
		qtextflag = FALSE;
		if (leveltype == DTYPE_TOWN)
			sfx_stop();
	} else {
		switch (stextflag) {
		case STORE_SMITH:
		case STORE_WITCH:
		case STORE_BOY:
		case STORE_BBOY:
		case STORE_HEALER:
		case STORE_STORY:
		case STORE_TAVERN:
		case STORE_DRUNK:
		case STORE_BARMAID:
			stextflag = STORE_NONE;
			return;
		case STORE_SBUY:
			StartStore(STORE_SMITH);
			stextsel = 12;
			return;
		case STORE_SSELL:
			v1 = STORE_SMITH;
			goto LABEL_16;
		case STORE_SREPAIR:
			v2 = STORE_SMITH;
			goto LABEL_14;
		case STORE_WBUY:
			v0 = STORE_WITCH;
			goto LABEL_18;
		case STORE_WSELL:
			v1 = STORE_WITCH;
			goto LABEL_16;
		case STORE_WRECHARGE:
			v2 = STORE_WITCH;
		LABEL_14:
			StartStore(v2);
			stextsel = 18;
			return;
		case STORE_NOMONEY:
		case STORE_NOROOM:
		case STORE_CONFIRM:
			StartStore((unsigned char)stextshold);
			stextsel = stextlhold;
			stextsval = stextvhold;
			return;
		case STORE_HBUY:
			v1 = STORE_HEALER;
		LABEL_16:
			StartStore(v1);
			stextsel = 16;
			return;
		case STORE_SIDENTIFY:
			v0 = STORE_STORY;
			goto LABEL_18;
		case STORE_SPBUY:
			v0 = STORE_SMITH;
		LABEL_18:
			StartStore(v0);
			stextsel = 14;
			break;
		case STORE_GOSSIP:
			StartStore((unsigned char)stextshold);
			stextsel = stextlhold;
			break;
		case STORE_IDSHOW:
			StartStore(STORE_SIDENTIFY);
			break;
		}
	}
}
// 646D00: using guessed type char qtextflag;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;
// 6AA705: using guessed type char stextflag;

void STextUp()
{
	int v0; // eax

	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1) {
		if (stextscrl) {
			if (stextsel == stextup) {
				if (stextsval)
					--stextsval;
				return;
			}
			v0 = stextsel - 1;
			stextsel = v0;
			if (stext[v0]._ssel)
				return;
			do {
				if (v0)
					--v0;
				else
					v0 = 23;
			} while (!stext[v0]._ssel);
		LABEL_20:
			stextsel = v0;
			return;
		}
		if (stextsel)
			v0 = stextsel - 1;
		else
			v0 = 23;
		stextsel = v0;
		if (!stext[v0]._ssel) {
			do {
				if (v0)
					--v0;
				else
					v0 = 23;
			} while (!stext[v0]._ssel);
			goto LABEL_20;
		}
	}
}
// 69F108: using guessed type int stextup;
// 6A8A28: using guessed type int stextsel;

void STextDown()
{
	int v0; // eax

	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1) {
		if (stextscrl) {
			if (stextsel == stextdown) {
				if (stextsval < stextsmax)
					++stextsval;
				return;
			}
			v0 = stextsel + 1;
			stextsel = v0;
			if (stext[v0]._ssel)
				return;
			do {
				if (v0 == 23)
					v0 = 0;
				else
					++v0;
			} while (!stext[v0]._ssel);
		LABEL_20:
			stextsel = v0;
			return;
		}
		if (stextsel == 23)
			v0 = 0;
		else
			v0 = stextsel + 1;
		stextsel = v0;
		if (!stext[v0]._ssel) {
			do {
				if (v0 == 23)
					v0 = 0;
				else
					++v0;
			} while (!stext[v0]._ssel);
			goto LABEL_20;
		}
	}
}
// 6A09E4: using guessed type int stextsmax;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

void STextPrior()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1 && stextscrl) {
		if (stextsel == stextup) {
			if (stextsval) {
				stextsval -= 4;
				if (stextsval < 0)
					stextsval = 0;
			}
		} else {
			stextsel = stextup;
		}
	}
}
// 69F108: using guessed type int stextup;
// 6A8A28: using guessed type int stextsel;

void STextNext()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1 && stextscrl) {
		if (stextsel == stextdown) {
			if (stextsval < stextsmax)
				stextsval += 4;
			if (stextsval > stextsmax)
				stextsval = stextsmax;
		} else {
			stextsel = stextdown;
		}
	}
}
// 6A09E4: using guessed type int stextsmax;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

void S_SmithEnter()
{
	int v0; // ecx

	v0 = 10;
	if (stextsel == 10) {
		talker = 0;
		stextlhold = 10;
		stextshold = 1;
		gossipstart = QUEST_GRISWOLD2;
		gossipend = QUEST_GRISWOLD13;
		_LOBYTE(v0) = STORE_GOSSIP;
		goto LABEL_13;
	}
	v0 = STORE_SBUY;
	switch (stextsel) {
	case 12:
	LABEL_13:
		StartStore(v0);
		return;
	case 14:
		_LOBYTE(v0) = STORE_SPBUY;
		goto LABEL_13;
	case 16:
		_LOBYTE(v0) = STORE_SSELL;
		goto LABEL_13;
	case 18:
		_LOBYTE(v0) = STORE_SREPAIR;
		goto LABEL_13;
	case 20:
		stextflag = STORE_NONE;
		break;
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void SetGoldCurs(int pnum, int i)
{
	if (plr[pnum].InvList[i]._ivalue >= 2500)
		plr[pnum].InvList[i]._iCurs = ICURS_GOLD_LARGE;
	else if (plr[pnum].InvList[i]._ivalue <= 1000)
		plr[pnum].InvList[i]._iCurs = ICURS_GOLD_SMALL;
	else
		plr[pnum].InvList[i]._iCurs = ICURS_GOLD_MEDIUM;
}

void SetSpdbarGoldCurs(int pnum, int i)
{
	if (plr[pnum].SpdList[i]._ivalue >= 2500)
		plr[pnum].SpdList[i]._iCurs = ICURS_GOLD_LARGE;
	else if (plr[pnum].SpdList[i]._ivalue <= 1000)
		plr[pnum].SpdList[i]._iCurs = ICURS_GOLD_SMALL;
	else
		plr[pnum].SpdList[i]._iCurs = ICURS_GOLD_MEDIUM;
}

void TakePlrsMoney(int cost)
{
	int v1;         // edi
	int v2;         // eax
	int v3;         // esi
	int v4;         // ebx
	int v5;         // eax
	_DWORD *v6;     // ecx
	int v7;         // eax
	int v8;         // ebx
	int v9;         // eax
	_DWORD *v10;    // ecx
	int v11;        // eax
	signed int v12; // ebx
	int v13;        // eax
	int v14;        // eax
	_DWORD *v15;    // ecx
	int v16;        // eax
	signed int v17; // ebx
	int v18;        // eax
	int v19;        // eax
	_DWORD *v20;    // ecx
	int v21;        // eax

	v1 = cost;
	v2 = CalculateGold(myplr);
	v3 = myplr;
	v4 = 0;
	plr[myplr]._pGold = v2 - v1;
	while (v1 > 0) {
		v5 = 368 * v4 + 21720 * v3;
		if (*(int *)((char *)&plr[0].SpdList[0]._itype + v5) == ITYPE_GOLD) {
			v6 = (unsigned int *)((char *)&plr[0].SpdList[0]._ivalue + v5);
			v7 = *(int *)((char *)&plr[0].SpdList[0]._ivalue + v5);
			if (v7 != 5000) {
				if (v1 >= v7) {
					v1 -= v7;
					RemoveSpdBarItem(v3, v4);
					v3 = myplr;
					v4 = -1;
				} else {
					*v6 = v7 - v1;
					SetSpdbarGoldCurs(v3, v4);
					v1 = 0;
				}
			}
		}
		if (++v4 >= MAXBELTITEMS) {
			if (v1 > 0) {
				v8 = 0;
				do {
					if (v1 <= 0)
						break;
					v9 = 368 * v8 + 21720 * v3;
					if (*(int *)((char *)&plr[0].SpdList[0]._itype + v9) == ITYPE_GOLD) {
						v10 = (unsigned int *)((char *)&plr[0].SpdList[0]._ivalue + v9);
						v11 = *(int *)((char *)&plr[0].SpdList[0]._ivalue + v9);
						if (v1 >= v11) {
							v1 -= v11;
							RemoveSpdBarItem(v3, v8);
							v3 = myplr;
							v8 = -1;
						} else {
							*v10 = v11 - v1;
							SetSpdbarGoldCurs(v3, v8);
							v1 = 0;
						}
					}
					++v8;
				} while (v8 < MAXBELTITEMS);
			}
			break;
		}
	}
	v12 = 0;
	drawpanflag = 255;
	if (v1 > 0) {
		v13 = 21720 * v3;
		if (plr[v3]._pNumInv <= 0) {
		LABEL_26:
			v17 = 0;
			if (v1 > 0) {
				v18 = 21720 * v3;
				if (plr[v3]._pNumInv > 0) {
					do {
						if (v1 <= 0)
							break;
						v19 = 368 * v17 + v18;
						if (*(int *)((char *)&plr[0].InvList[0]._itype + v19) == ITYPE_GOLD) {
							v20 = (unsigned int *)((char *)&plr[0].InvList[0]._ivalue + v19);
							v21 = *(int *)((char *)&plr[0].InvList[0]._ivalue + v19);
							if (v1 >= v21) {
								v1 -= v21;
								RemoveInvItem(v3, v17);
								v3 = myplr;
								v17 = -1;
							} else {
								*v20 = v21 - v1;
								SetGoldCurs(v3, v17);
								v1 = 0;
							}
						}
						++v17;
						v18 = 21720 * v3;
					} while (v17 < plr[v3]._pNumInv);
				}
			}
		} else {
			while (v1 > 0) {
				v14 = 368 * v12 + v13;
				if (*(int *)((char *)&plr[0].InvList[0]._itype + v14) == ITYPE_GOLD) {
					v15 = (unsigned int *)((char *)&plr[0].InvList[0]._ivalue + v14);
					v16 = *(int *)((char *)&plr[0].InvList[0]._ivalue + v14);
					if (v16 != 5000) {
						if (v1 >= v16) {
							v1 -= v16;
							RemoveInvItem(v3, v12);
							v3 = myplr;
							v12 = -1;
						} else {
							*v15 = v16 - v1;
							SetGoldCurs(v3, v12);
							v1 = 0;
						}
					}
				}
				++v12;
				v13 = 21720 * v3;
				if (v12 >= plr[v3]._pNumInv)
					goto LABEL_26;
			}
		}
	}
}
// 52571C: using guessed type int drawpanflag;

void SmithBuyItem()
{
	int idx;

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	if (plr[myplr].HoldItem._iMagical == ITEM_QUALITY_NORMAL)
		plr[myplr].HoldItem._iIdentified = FALSE;
	StoreAutoPlace();
	idx = stextvhold + ((stextlhold - stextup) >> 2);
	if (idx == 19) {
		smithitem[19]._itype = -1;
	} else {
		for (; smithitem[idx + 1]._itype != -1; idx++) {
			smithitem[idx] = smithitem[idx + 1];
		}
		smithitem[idx]._itype = -1;
	}
	CalcPlrInv(myplr, 1);
}

void S_SBuyEnter()
{
	int idx, i;
	BOOL done;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 12;
	} else {
		stextlhold = stextsel;
		stextvhold = stextsval;
		stextshold = 2;
		idx = stextsval + ((stextsel - stextup) >> 2);
		if (plr[myplr]._pGold < smithitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			plr[myplr].HoldItem = smithitem[idx];
			SetCursor_(plr[myplr].HoldItem._iCurs + CURSOR_FIRSTITEM);
			done = FALSE;

			for (i = 0; i < 40 && !done; i++) {
				done = AutoPlace(myplr, i, cursW / 28, cursH / 28, 0);
			}
			if (done)
				StartStore(STORE_CONFIRM);
			else
				StartStore(STORE_NOROOM);
			SetCursor_(CURSOR_HAND);
		}
	}
}

void SmithBuyPItem()
{
	int xx;     // ecx
	int idx;    // eax
	BOOLEAN v2; // sf
	int v3;     // eax
	int i;      // edx

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	if (plr[myplr].HoldItem._iMagical == ITEM_QUALITY_NORMAL)
		plr[myplr].HoldItem._iIdentified = FALSE;
	StoreAutoPlace();
	xx = 0;
	idx = (stextlhold - stextup) >> 2;
	v2 = stextvhold + idx < 0;
	v3 = stextvhold + idx;
	i = 0;
	if (!v2) {
		do {
			if (premiumitem[i]._itype != -1) {
				--v3;
				xx = i;
			}
			++i;
		} while (v3 >= 0);
	}

	premiumitem[xx]._itype = -1;
	--numpremium;
	SpawnPremium(plr[myplr]._pLevel);
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void S_SPBuyEnter()
{
	int i, idx, xx;
	BOOL done;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 14;
	} else {
		stextshold = 18;
		stextlhold = stextsel;
		stextvhold = stextsval;
		xx = stextsval + ((stextsel - stextup) >> 2);
		idx = 0;
		for (i = 0; xx >= 0; i++) {
			if (premiumitem[i]._itype != -1) {
				xx--;
				idx = i;
			}
		}
		if (plr[myplr]._pGold < premiumitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			plr[myplr].HoldItem = premiumitem[idx];
			SetCursor_(plr[myplr].HoldItem._iCurs + CURSOR_FIRSTITEM);
			done = FALSE;
			for (i = 0; i < 40 && !done; i++) {
				done = AutoPlace(myplr, i, cursW / 28, cursH / 28, 0);
			}
			if (done)
				StartStore(STORE_CONFIRM);
			else
				StartStore(STORE_NOROOM);
			SetCursor_(CURSOR_HAND);
		}
	}
}

BOOL StoreGoldFit(int idx)
{
	int cost;    // edi
	int i;       // ecx
	int sz;      // eax
	int numsqrs; // [esp+Ch] [ebp-4h]

	cost = storehold[idx]._iIvalue;
	sz = cost / 5000;
	if (cost % 5000)
		sz++;

	SetCursor_(storehold[idx]._iCurs + CURSOR_FIRSTITEM);
	numsqrs = cursW / 28 * (cursH / 28);
	SetCursor_(CURSOR_HAND);

	if (numsqrs >= sz)
		return 1;

	for (i = 0; i < 40; i++) {
		if (!plr[myplr].InvGrid[i])
			numsqrs++;
	}

	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (plr[myplr].InvList[i]._itype == ITYPE_GOLD && plr[myplr].InvList[i]._ivalue != 5000) {
			cost += plr[myplr].InvList[i]._ivalue;
			if (cost > 5000)
				cost -= 5000;
			else
				cost = 0;
		}
	}

	sz = cost / 5000;
	if (cost % 5000)
		sz++;
	return numsqrs >= sz;
}
// 4B8C9C: using guessed type int cursH;

void PlaceStoreGold(int v)
{
	BOOL done;
	int ii, xx, yy, i;

	done = FALSE;

	for (i = 0; i < 40 && !done; i++) {
		yy = 10 * (i / 10);
		xx = i % 10;
		if (!plr[myplr].InvGrid[xx + yy]) {
			ii = plr[myplr]._pNumInv;
			GetGoldSeed(myplr, &golditem);
			plr[myplr].InvList[ii] = golditem;
			plr[myplr]._pNumInv++;
			plr[myplr].InvGrid[xx + yy] = plr[myplr]._pNumInv;
			plr[myplr].InvList[ii]._ivalue = v;
			SetGoldCurs(myplr, ii);
			done = TRUE;
		}
	}
}

void StoreSellItem()
{
	int i, idx, cost;

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	if (storehidx[idx] >= 0)
		RemoveInvItem(myplr, storehidx[idx]);
	else
		RemoveSpdBarItem(myplr, -(storehidx[idx] + 1));
	cost = storehold[idx]._iIvalue;
	storenumh--;
	if (idx != storenumh) {
		while (idx < storenumh) {
			storehold[idx] = storehold[idx + 1];
			storehidx[idx] = storehidx[idx + 1];
			idx++;
		}
	}
	plr[myplr]._pGold += cost;
	for (i = 0; i < plr[myplr]._pNumInv && cost > 0; i++) {
		if (plr[myplr].InvList[i]._itype == ITYPE_GOLD && plr[myplr].InvList[i]._ivalue != 5000) {
			if (cost + plr[myplr].InvList[i]._ivalue <= 5000) {
				plr[myplr].InvList[i]._ivalue += cost;
				SetGoldCurs(myplr, i);
				cost = 0;
			} else {
				cost -= 5000 - plr[myplr].InvList[i]._ivalue;
				plr[myplr].InvList[i]._ivalue = 5000;
				SetGoldCurs(myplr, i);
			}
		}
	}
	if (cost > 0) {
		while (cost > 5000) {
			PlaceStoreGold(5000);
			cost -= 5000;
		}
		PlaceStoreGold(cost);
	}
}

void S_SSellEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 16;
	} else {
		stextlhold = stextsel;
		idx = stextsval + ((stextsel - stextup) >> 2);
		stextshold = 3;
		stextvhold = stextsval;
		plr[myplr].HoldItem = storehold[idx];

		if (StoreGoldFit(idx))
			StartStore(STORE_CONFIRM);
		else
			StartStore(STORE_NOROOM);
	}
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void SmithRepairItem()
{
	int i;   // edx
	int idx; // eax

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	i = storehidx[idx];
	storehold[idx]._iDurability = storehold[idx]._iMaxDur;

	if (i >= 0) {
		plr[myplr].InvList[i]._iDurability = plr[myplr].InvList[i]._iMaxDur;
	} else {
		if (i == -1)
			plr[myplr].InvBody[INVLOC_HEAD]._iDurability = plr[myplr].InvBody[INVLOC_HEAD]._iMaxDur;
		if (i == -2)
			plr[myplr].InvBody[INVLOC_CHEST]._iDurability = plr[myplr].InvBody[INVLOC_CHEST]._iMaxDur;
		if (i == -3)
			plr[myplr].InvBody[INVLOC_HAND_LEFT]._iDurability = plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMaxDur;
		if (i == -4)
			plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iDurability = plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iMaxDur;
	}
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void S_SRepairEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 18;
	} else {
		stextshold = 4;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = stextsval + ((stextsel - stextup) >> 2);
		plr[myplr].HoldItem = storehold[idx];
		if (plr[myplr]._pGold < storehold[idx]._iIvalue)
			StartStore(STORE_NOMONEY);
		else
			StartStore(STORE_CONFIRM);
	}
}

void S_WitchEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 6;
		stextshold = 5;
		gossipstart = QUEST_ADRIA2;
		gossipend = QUEST_ADRIA13;
		StartStore(STORE_GOSSIP);
		return;
	case 14:
		StartStore(STORE_WBUY);
		return;
	case 16:
		StartStore(STORE_WSELL);
		return;
	case 18:
		StartStore(STORE_WRECHARGE);
		return;
	case 20:
		stextflag = STORE_NONE;
		break;
	}
}

void WitchBuyItem()
{
	int idx;

	idx = stextvhold + ((stextlhold - stextup) >> 2);

	if (idx < 3)
		plr[myplr].HoldItem._iSeed = GetRndSeed();

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	StoreAutoPlace();

	if (idx >= 3) {
		if (idx == 19) {
			witchitem[19]._itype = -1;
		} else {
			for (; witchitem[idx + 1]._itype != -1; idx++) {
				witchitem[idx] = witchitem[idx + 1];
			}
			witchitem[idx]._itype = -1;
		}
	}

	CalcPlrInv(myplr, TRUE);
}

void S_WBuyEnter()
{
	int i, idx;
	BOOL done;

	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 14;
	} else {
		stextlhold = stextsel;
		stextvhold = stextsval;
		stextshold = 6;
		idx = stextsval + ((stextsel - stextup) >> 2);

		if (plr[myplr]._pGold < witchitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			plr[myplr].HoldItem = witchitem[idx];
			SetCursor_(plr[myplr].HoldItem._iCurs + CURSOR_FIRSTITEM);
			done = FALSE;

			for (i = 0; i < 40 && !done; i++) {
				done = SpecialAutoPlace(myplr, i, cursW / 28, cursH / 28, 0);
			}

			if (done)
				StartStore(STORE_CONFIRM);
			else
				StartStore(STORE_NOROOM);

			SetCursor_(CURSOR_HAND);
		}
	}
}
// 4B8C9C: using guessed type int cursH;
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void S_WSellEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 16;
	} else {
		stextlhold = stextsel;
		idx = stextsval + ((stextsel - stextup) >> 2);
		stextshold = 7;
		stextvhold = stextsval;
		plr[myplr].HoldItem = storehold[idx];
		if (StoreGoldFit(idx))
			StartStore(STORE_CONFIRM);
		else
			StartStore(STORE_NOROOM);
	}
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void WitchRechargeItem()
{
	int i, idx;

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	storehold[idx]._iCharges = storehold[idx]._iMaxCharges;

	i = storehidx[idx];
	if (i < 0)
		plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges = plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMaxCharges;
	else
		plr[myplr].InvList[i]._iCharges = plr[myplr].InvList[i]._iMaxCharges;

	CalcPlrInv(myplr, 1u);
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void S_WRechargeEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 18;
	} else {
		stextshold = 8;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = stextsval + ((stextsel - stextup) >> 2);
		plr[myplr].HoldItem = storehold[idx];
		if (plr[myplr]._pGold < storehold[idx]._iIvalue)
			StartStore(STORE_NOMONEY);
		else
			StartStore(STORE_CONFIRM);
	}
}

void S_BoyEnter()
{
	if (boyitem._itype != ITYPE_NONE && stextsel == 18) {
		if (plr[myplr]._pGold < 50) {
			stextshold = 12;
			stextlhold = 18;
			stextvhold = stextsval;
			StartStore(STORE_NOMONEY);
		} else {
			TakePlrsMoney(50);
			StartStore(STORE_BBOY);
		}
	} else if (stextsel == 8 && boyitem._itype != ITYPE_NONE || stextsel == 12 && boyitem._itype == ITYPE_NONE) {
		talker = 8;
		stextshold = 12;
		stextlhold = stextsel;
		gossipstart = QUEST_WIRT2;
		gossipend = QUEST_WIRT12;
		StartStore(STORE_GOSSIP);
	} else {
		stextflag = STORE_NONE;
	}
}

void BoyBuyItem()
{
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	StoreAutoPlace();
	boyitem._itype = -1;
	stextshold = 12;
	CalcPlrInv(myplr, 1u);
}

void HealerBuyItem()
{
	int idx;
	BOOL ok;

	idx = stextvhold + ((stextlhold - stextup) >> 2);

	ok = FALSE;
	if (gbMaxPlayers == 1) {
		if (idx < 2)
			ok = TRUE;
	} else {
		if (idx < 3)
			ok = TRUE;
	}
	if (ok) {
		plr[myplr].HoldItem._iSeed = GetRndSeed();
	}

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	if (plr[myplr].HoldItem._iMagical == ITEM_QUALITY_NORMAL)
		plr[myplr].HoldItem._iIdentified = FALSE;
	StoreAutoPlace();

	ok = FALSE;
	if (gbMaxPlayers == 1) {
		if (idx >= 2)
			ok = TRUE;
	} else {
		if (idx >= 3)
			ok = TRUE;
	}
	if (ok) {
		idx = stextvhold + ((stextlhold - stextup) >> 2);
		if (idx == 19) {
			healitem[19]._itype = -1;
		} else {
			for (; healitem[idx + 1]._itype != -1; idx++) {
				healitem[idx] = healitem[idx + 1];
			}
			healitem[idx]._itype = -1;
		}
		CalcPlrInv(myplr, TRUE);
	}
}

void S_BBuyEnter()
{
	BOOL done;
	int i;

	if (stextsel == 10) {
		stextlhold = 10;
		stextvhold = stextsval;
		stextshold = 13;
		if (plr[myplr]._pGold >= boyitem._iIvalue + (boyitem._iIvalue >> 1)) {
			StartStore(STORE_NOMONEY);
		} else {
			plr[myplr].HoldItem = boyitem;
			plr[myplr].HoldItem._iIvalue += plr[myplr].HoldItem._iIvalue >> 1;
			SetCursor_(plr[myplr].HoldItem._iCurs + CURSOR_FIRSTITEM);
			done = FALSE;
			for (i = 0; i < 40 && !done; i++) {
				done = AutoPlace(myplr, i, cursW / 28, cursH / 28, 0);
			}
			if (done)
				StartStore(STORE_CONFIRM);
			else
				StartStore(STORE_NOROOM);
			SetCursor_(CURSOR_HAND);
		}
	} else {
		stextflag = STORE_NONE;
	}
}

void StoryIdItem()
{
	int idx;

	idx = storehidx[((stextlhold - stextup) >> 2) + stextvhold];
	if (idx < 0) {
		if (idx == -1)
			plr[myplr].InvBody[INVLOC_HEAD]._iIdentified = TRUE;
		if (idx == -2)
			plr[myplr].InvBody[INVLOC_CHEST]._iIdentified = TRUE;
		if (idx == -3)
			plr[myplr].InvBody[INVLOC_HAND_LEFT]._iIdentified = TRUE;
		if (idx == -4)
			plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iIdentified = TRUE;
		if (idx == -5)
			plr[myplr].InvBody[INVLOC_RING_LEFT]._iIdentified = TRUE;
		if (idx == -6)
			plr[myplr].InvBody[INVLOC_RING_RIGHT]._iIdentified = TRUE;
		if (idx == -7)
			plr[myplr].InvBody[INVLOC_AMULET]._iIdentified = TRUE;
	} else {
		plr[myplr].InvList[idx]._iIdentified = TRUE;
	}
	plr[myplr].HoldItem._iIdentified = TRUE;
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	CalcPlrInv(myplr, TRUE);
}

void S_ConfirmEnter()
{
	if (stextsel == 18) {
		switch (stextshold) {
		case STORE_SBUY:
			SmithBuyItem();
			break;
		case STORE_SSELL:
		case STORE_WSELL:
			StoreSellItem();
			break;
		case STORE_SREPAIR:
			SmithRepairItem();
			break;
		case STORE_WBUY:
			WitchBuyItem();
			break;
		case STORE_WRECHARGE:
			WitchRechargeItem();
			break;
		case STORE_BBOY:
			BoyBuyItem();
			break;
		case STORE_HBUY:
			HealerBuyItem();
			break;
		case STORE_SIDENTIFY:
			StoryIdItem();
			StartStore(STORE_IDSHOW);
			return;
		case STORE_SPBUY:
			SmithBuyPItem();
			break;
		}
		StartStore(stextshold);
	} else {
		StartStore(stextshold);
		stextsel = stextlhold;
		stextsval = stextvhold;
	}
}

void S_HealerEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 1;
		stextshold = 14;
		gossipstart = QUEST_PEPIN2;
		gossipend = QUEST_PEPIN11;
		StartStore(STORE_GOSSIP);
		return;
	case 14:
		if (plr[myplr]._pHitPoints != plr[myplr]._pMaxHP)
			PlaySFX(IS_CAST8);
		drawhpflag = TRUE;
		plr[myplr]._pHitPoints = plr[myplr]._pMaxHP;
		plr[myplr]._pHPBase = plr[myplr]._pMaxHPBase;
		return;
	case 16:
		StartStore(STORE_HBUY);
		return;
	case 18:
		stextflag = STORE_NONE;
		return;
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void S_HBuyEnter()
{
	int i, idx;
	BOOL done;

	if (stextsel == 22) {
		StartStore(STORE_HEALER);
		stextsel = 16;
	} else {
		stextlhold = stextsel;
		stextvhold = stextsval;
		stextshold = 16;
		idx = stextsval + ((stextsel - stextup) >> 2);
		if (plr[myplr]._pGold < healitem[idx]._iIvalue) {
			StartStore(STORE_NOMONEY);
		} else {
			plr[myplr].HoldItem = healitem[idx];
			SetCursor_(plr[myplr].HoldItem._iCurs + CURSOR_FIRSTITEM);
			done = FALSE;
			i = 0;
			for (i = 0; i < 40 && !done; i++) {
				done = SpecialAutoPlace(myplr, i, cursW / 28, cursH / 28, 0);
			}
			if (done)
				StartStore(STORE_CONFIRM);
			else
				StartStore(STORE_NOROOM);
			SetCursor_(CURSOR_HAND);
		}
	}
}

void S_StoryEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 4;
		stextshold = 15;
		gossipstart = QUEST_STORY2;
		gossipend = QUEST_STORY11;
		StartStore(STORE_GOSSIP);
		break;
	case 14:
		StartStore(STORE_SIDENTIFY);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void S_SIDEnter()
{
	int idx;

	if (stextsel == 22) {
		StartStore(STORE_STORY);
		stextsel = 14;
	} else {
		stextshold = 17;
		stextlhold = stextsel;
		stextvhold = stextsval;
		idx = stextsval + ((stextsel - stextup) >> 2);
		plr[myplr].HoldItem = storehold[idx];
		if (plr[myplr]._pGold < storehold[idx]._iIvalue)
			StartStore(STORE_NOMONEY);
		else
			StartStore(STORE_CONFIRM);
	}
}

void S_TalkEnter()
{
	int v0;        // edx
	int *v1;       // edi
	signed int v2; // eax
	int v3;        // esi
	int *v4;       // ecx
	int v5;        // esi
	signed int v6; // ebp
	int v8;        // eax
	int v9;        // ebx
	int v10;       // ecx

	if (stextsel == 22) {
		StartStore((unsigned char)stextshold);
		stextsel = stextlhold;
	} else {
		v0 = talker;
		v1 = &quests[0]._qlog;
		v2 = 0;
		v3 = 0;
		v4 = &quests[0]._qlog;
		do {
			if (*((_BYTE *)v4 - 18) == 2 && *((_DWORD *)&Qtalklist[0]._qinfra + v3 + 16 * talker) != -1 && *v4)
				++v2;
			v4 += 6;
			++v3;
		} while ((signed int)v4 < (signed int)&quests[16]._qlog);
		if (v2 <= 6) {
			v5 = 15 - v2;
			v6 = 2;
		} else {
			v5 = 14 - (v2 >> 1);
			v6 = 1;
		}
		if (stextsel == v5 - 2) {
			SetRndSeed(towner[talker]._tSeed);
			v8 = random(0, gossipend - gossipstart + 1);
			InitQTextMsg(gossipstart + v8);
		} else {
			v9 = 0;
			do {
				if (*((_BYTE *)v1 - 18) == 2) {
					v10 = *((_DWORD *)&Qtalklist[0]._qinfra + v9 + 16 * v0);
					if (v10 != -1) {
						if (*v1) {
							if (v5 == stextsel) {
								InitQTextMsg(v10);
								v0 = talker;
							}
							v5 += v6;
						}
					}
				}
				v1 += 6;
				++v9;
			} while ((signed int)v1 < (signed int)&quests[16]._qlog);
		}
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;

void S_TavernEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 3;
		stextshold = 21;
		gossipstart = QUEST_OGDEN2;
		gossipend = QUEST_OGDEN10;
		StartStore(STORE_GOSSIP);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void S_BarmaidEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 7;
		stextshold = 23;
		gossipstart = QUEST_GILLIAN2;
		gossipend = QUEST_GILLIAN10;
		StartStore(STORE_GOSSIP);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void S_DrunkEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = 5;
		stextshold = 22;
		gossipstart = QUEST_FARNHAM2;
		gossipend = QUEST_FARNHAM13;
		StartStore(STORE_GOSSIP);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void STextEnter()
{
	if (qtextflag) {
		qtextflag = FALSE;
		if (leveltype == DTYPE_TOWN)
			sfx_stop();
	} else {
		PlaySFX(IS_TITLSLCT);
		switch (stextflag) {
		case STORE_SMITH:
			S_SmithEnter();
			break;
		case STORE_SBUY:
			S_SBuyEnter();
			break;
		case STORE_SSELL:
			S_SSellEnter();
			break;
		case STORE_SREPAIR:
			S_SRepairEnter();
			break;
		case STORE_WITCH:
			S_WitchEnter();
			break;
		case STORE_WBUY:
			S_WBuyEnter();
			break;
		case STORE_WSELL:
			S_WSellEnter();
			break;
		case STORE_WRECHARGE:
			S_WRechargeEnter();
			break;
		case STORE_NOMONEY:
		case STORE_NOROOM:
			StartStore(stextshold);
			stextsel = stextlhold;
			stextsval = stextvhold;
			break;
		case STORE_CONFIRM:
			S_ConfirmEnter();
			break;
		case STORE_BOY:
			S_BoyEnter();
			break;
		case STORE_BBOY:
			S_BBuyEnter();
			break;
		case STORE_HEALER:
			S_HealerEnter();
			break;
		case STORE_STORY:
			S_StoryEnter();
			break;
		case STORE_HBUY:
			S_HBuyEnter();
			break;
		case STORE_SIDENTIFY:
			S_SIDEnter();
			break;
		case STORE_SPBUY:
			S_SPBuyEnter();
			break;
		case STORE_GOSSIP:
			S_TalkEnter();
			break;
		case STORE_IDSHOW:
			StartStore(STORE_SIDENTIFY);
			break;
		case STORE_TAVERN:
			S_TavernEnter();
			break;
		case STORE_DRUNK:
			S_DrunkEnter();
			break;
		case STORE_BARMAID:
			S_BarmaidEnter();
			break;
		}
	}
}
// 646D00: using guessed type char qtextflag;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;
// 6AA705: using guessed type char stextflag;

void CheckStoreBtn()
{
	BOOLEAN v0; // sf
	//unsigned char v1; // of
	int v2;  // eax
	int *v3; // ecx

	if (qtextflag) {
		qtextflag = FALSE;
		if (leveltype == DTYPE_TOWN)
			sfx_stop();
	} else if (stextsel != -1 && MouseY >= 32 && MouseY <= 320) {
		if (stextsize) {
			//v1 = __OFSUB__(MouseX, 24);
			v0 = MouseX - 24 < 0;
		} else {
			//v1 = __OFSUB__(MouseX, 344);
			v0 = MouseX - 344 < 0;
		}
		if (!v0 && MouseX <= 616) { //if (!(v0 ^ v1) && MouseX <= 616) {
			v2 = (MouseY - 32) / 12;
			if (stextscrl && MouseX > 600) {
				if (v2 == 4) {
					if (stextscrlubtn <= 0) {
						STextUp();
						stextscrlubtn = 10;
						return;
					}
					--stextscrlubtn;
				}
				if (v2 == 20) {
					if (stextscrldbtn > 0) {
						--stextscrldbtn;
					} else {
						STextDown();
						stextscrldbtn = 10;
					}
				}
			} else if (v2 >= 5) {
				if (v2 >= 23)
					v2 = 22;
				if (stextscrl) {
					if (v2 < 21) {
						v3 = &stext[v2]._ssel;
						if (!*v3) {
							if (stext[v2 - 2]._ssel) {
								v2 -= 2;
							} else if (*(v3 - 39)) {
								--v2;
							}
						}
					}
				}
				if (stext[v2]._ssel || stextscrl && v2 == 22) {
					stextsel = v2;
					STextEnter();
				}
			}
		}
	}
}
// 646D00: using guessed type char qtextflag;
// 6A09E0: using guessed type char stextsize;
// 6A8A28: using guessed type int stextsel;
// 6A8A2C: using guessed type char stextscrldbtn;
// 6AA704: using guessed type char stextscrlubtn;

void ReleaseStoreBtn()
{
	stextscrlubtn = -1;
	stextscrldbtn = -1;
}
// 6A8A2C: using guessed type char stextscrldbtn;
// 6AA704: using guessed type char stextscrlubtn;

DEVILUTION_END_NAMESPACE
