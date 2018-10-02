//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int stextup; // weak
int storenumh; // weak
int stextlhold; // weak
ItemStruct boyitem;
int stextshold; // idb
ItemStruct premiumitem[6];
void *pSTextBoxCels;
int premiumlevel; // idb
int talker; // weak
STextStruct stext[24];
char stextsize; // weak
int stextsmax; // weak
int InStoreFlag; // idb
ItemStruct storehold[48];
int gossipstart; // weak
ItemStruct witchitem[20];
int stextscrl; // weak
int numpremium; // idb
ItemStruct healitem[20];
ItemStruct golditem;
char storehidx[48];
void *pSTextSlidCels;
int stextvhold; // weak
int stextsel; // weak
char stextscrldbtn; // weak
int gossipend; // weak
void *pCelBuff;
int stextsval; // idb
int boylevel; // weak
ItemStruct smithitem[20];
int stextdown; // weak
char stextscrlubtn; // weak
char stextflag; // weak
#endif

int SStringY[24] =
{
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
char *talkname[9] =
{
  "Griswold",
  "Pepin",
  &empty_string,
  "Ogden",
  "Cain",
  "Farnham",
  "Adria",
  "Gillian",
  "Wirt"
};

void __cdecl InitStores()
{
	int i; // eax

	pSTextBoxCels = LoadFileInMem("Data\\TextBox2.CEL", 0);
	pCelBuff = LoadFileInMem("Data\\PentSpn2.CEL", 0);
	pSTextSlidCels = LoadFileInMem("Data\\TextSlid.CEL", 0);
	ClearSText(0, 24);
	stextflag = 0;
	InStoreFlag = 1;
	premiumlevel = 1;
	stextsize = 0;
	stextscrl = 0;
	numpremium = 0;

	for(i = 0; i < 6; i++)
		premiumitem[i]._itype = -1;

	boyitem._itype = -1;
	boylevel = 0;
}
// 69FB38: using guessed type int talker;
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;
// 6A8A3C: using guessed type int boylevel;
// 6AA705: using guessed type char stextflag;

void __cdecl SetupTownStores()
{
	int i; // eax
	int l; // esi

	SetRndSeed(glSeedTbl[currlevel] * GetTickCount());
	if ( gbMaxPlayers == 1 )
	{
		l = 0;
		for(i = 0; i < 17; i++)
		{
			if ( plr[myplr]._pLvlVisited[i] )
				l = i;
		}
	}
	else
	{
		l = plr[myplr]._pLevel >> 1;
	}
	l += 2;
	if ( l < 6 )
		l = 6;
	if ( l > 16 )
		l = 16;
	SpawnStoreGold();
	SpawnSmith(l);
	SpawnWitch(l);
	SpawnHealer(l);
	SpawnBoy(plr[myplr]._pLevel);
	SpawnPremium(plr[myplr]._pLevel);
}
// 679660: using guessed type char gbMaxPlayers;

void __cdecl FreeStoreMem()
{
	void *v0; // ecx
	void *v1; // ecx
	void *v2; // ecx

	v0 = pSTextBoxCels;
	pSTextBoxCels = 0;
	mem_free_dbg(v0);
	v1 = pCelBuff;
	pCelBuff = 0;
	mem_free_dbg(v1);
	v2 = pSTextSlidCels;
	pSTextSlidCels = 0;
	mem_free_dbg(v2);
}

void __cdecl DrawSTextBack()
{
	char *v0; // edi
	signed int v1; // edx
	signed int v2; // ecx
	int v3; // edi
	signed int v4; // ecx
	_BYTE *v5; // edi
	signed int v6; // ecx

	CelDecodeOnly(408, 487, pSTextBoxCels, 1, 271);
	v0 = &gpBuffer->row[324].pixels[347];
	v1 = 148;
	do
	{
		v2 = 132;
		do
		{
			*v0 = 0;
			v0 += 2;
			--v2;
		}
		while ( v2 );
		*v0 = 0;
		v3 = (int)(v0 - 1032);
		v4 = 132;
		do
		{
			v5 = (_BYTE *)(v3 + 1);
			*v5 = 0;
			v3 = (int)(v5 + 1);
			--v4;
		}
		while ( v4 );
		v0 = (char *)(v3 - 1032);
		--v1;
	}
	while ( v1 );
	v6 = 132;
	do
	{
		*v0 = 0;
		v0 += 2;
		--v6;
	}
	while ( v6 );
	*v0 = 0;
}

void __fastcall PrintSString(int x, int y, unsigned char cjustflag, char *str, int col, int val)
{
	int v6; // edi
	int v7; // eax
	int v8; // ebx
	int v9; // esi
	int v10; // esi
	int v11; // ecx
	int v12; // eax
	int v13; // edx
	int v14; // ecx
	unsigned char v15; // al
	int v16; // ebx
	int v17; // ecx
	int v18; // eax
	int v19; // esi
	size_t v20; // ebx
	unsigned char v21; // edx
	int v22; // ecx
	char valstr[32]; // [esp+Ch] [ebp-3Ch]
	int v24; // [esp+2Ch] [ebp-1Ch]
	int v25; // [esp+30h] [ebp-18h]
	int v26; // [esp+34h] [ebp-14h]
	int v27; // [esp+38h] [ebp-10h]
	int v28; // [esp+3Ch] [ebp-Ch]
	int v29; // [esp+40h] [ebp-8h]
	int v30; // [esp+44h] [ebp-4h]

	v6 = SStringY[y] + stext[y]._syoff;
	v7 = -(stextsize != 0);
	v8 = x;
	v9 = screen_y_times_768[v6 + 204];
	_LOWORD(v7) = v7 & 0xFEC0;
	v24 = y;
	v26 = x;
	v27 = v7 + 416;
	v10 = x + v7 + 416 + v9;
	v28 = strlen(str);
	v11 = 0;
	v25 = stextsize != 0 ? 577 : 257;
	v30 = 0;
	if ( cjustflag )
	{
		v12 = 0;
		if ( v28 > 0 )
		{
			do
			{
				v13 = (unsigned char)str[v11++];
				v12 += fontkern[fontframe[fontidx[v13]]] + 1;
			}
			while ( v11 < v28 );
		}
		if ( v12 < v25 )
			v30 = (v25 - v12) >> 1;
		v10 += v30;
	}
	if ( stextsel == v24 )
	{
		if ( cjustflag )
			v14 = v27 + v30 + v8 - 20;
		else
			v14 = v27 + v8 - 20;
		CelDecodeOnly(v14, v6 + 205, pCelBuff, InStoreFlag, 12);
	}
	v29 = 0;
	if ( v28 > 0 )
	{
		do
		{
			v15 = fontframe[fontidx[(unsigned char)str[v29]]];
			v16 = v15;
			v17 = v30 + fontkern[v15] + 1;
			v30 += fontkern[v15] + 1;
			if ( v15 && v17 <= v25 )
				CPrintString(v10, v15, col);
			v18 = fontkern[v16];
			++v29;
			v10 += v18 + 1;
		}
		while ( v29 < v28 );
		v8 = v26;
	}
	if ( !cjustflag && val >= 0 )
	{
		sprintf(valstr, "%i", val);
		v19 = screen_y_times_768[v6 + 204] - v8 + 656;
		v20 = strlen(valstr);
		while ( (--v20 & 0x80000000) == 0 )
		{
			v21 = fontframe[fontidx[(unsigned char)valstr[v20]]];
			v19 += -1 - fontkern[v21];
			if ( fontframe[fontidx[(unsigned char)valstr[v20]]] )
				CPrintString(v19, v21, col);
		}
		v8 = v26;
	}
	if ( stextsel == v24 )
	{
		if ( cjustflag )
			v22 = v27 + v30 + v8 + 4;
		else
			v22 = 660 - v8;
		CelDecodeOnly(v22, v6 + 205, pCelBuff, InStoreFlag, 12);
	}
}
// 6A09E0: using guessed type char stextsize;
// 6A8A28: using guessed type int stextsel;
// 457BD6: using guessed type char valstr[32];

void __fastcall DrawSLine(int y)
{
	int v1; // eax
	int v2; // eax
	char *v3; // esi
	char *v4; // edi
	signed int v5; // edx
	char *v6; // edi
	char *v7; // esi
	signed int v8; // [esp+0h] [ebp-10h]
	signed int v9; // [esp+8h] [ebp-8h]
	signed int v10; // [esp+Ch] [ebp-4h]

	v1 = screen_y_times_768[SStringY[y] + 198];
	if ( stextsize == 1 )
	{
		v8 = 142170;
		v2 = v1 + 90;
		v10 = 146;
		v9 = 182;
	}
	else
	{
		v8 = 142490;
		v2 = v1 + 410;
		v10 = 66;
		v9 = 502;
	}
	v3 = (char *)gpBuffer + v8;
	v4 = (char *)gpBuffer + v2;
	v5 = 3;
	do
	{
		qmemcpy(v4, v3, 4 * v10);
		v7 = &v3[4 * v10];
		v6 = &v4[4 * v10];
		*(_WORD *)v6 = *(_WORD *)v7;
		v3 = &v7[v9 + 2];
		v4 = &v6[v9 + 2];
		--v5;
	}
	while ( v5 );
}
// 6A09E0: using guessed type char stextsize;

void __fastcall DrawSArrows(int y1, int y2)
{
	int *v2; // ebp
	int v3; // ebx
	int v4; // edi
	int v5; // esi
	int v6; // eax
	int v7; // eax

	v2 = &SStringY[y2];
	v3 = y1;
	v4 = SStringY[y1] + 204;
	v5 = *v2 + 204;
	if ( stextscrlubtn == -1 )
		CelDecodeOnly(665, v4, pSTextSlidCels, 10, 12);
	else
		CelDecodeOnly(665, v4, pSTextSlidCels, 12, 12);
	if ( stextscrldbtn == -1 )
		CelDecodeOnly(665, v5, pSTextSlidCels, 9, 12);
	else
		CelDecodeOnly(665, v5, pSTextSlidCels, 11, 12);
	while ( 1 )
	{
		v4 += 12;
		if ( v4 >= v5 )
			break;
		CelDecodeOnly(665, v4, pSTextSlidCels, 14, 12);
	}
	v6 = stextsel;
	if ( stextsel == 22 )
		v6 = stextlhold;
	if ( storenumh <= 1 )
		v7 = 0;
	else
		v7 = (*v2 - SStringY[v3] - 24) * (1000 * (stextsval + ((v6 - stextup) >> 2)) / (storenumh - 1)) / 1000;
	CelDecodeOnly(665, SStringY[v3 + 1] + v7 + 204, pSTextSlidCels, 13, 12);
}
// 69F108: using guessed type int stextup;
// 69F10C: using guessed type int storenumh;
// 69F110: using guessed type int stextlhold;
// 6A8A28: using guessed type int stextsel;
// 6A8A2C: using guessed type char stextscrldbtn;
// 6AA704: using guessed type char stextscrlubtn;

void __cdecl DrawSTextHelp()
{
	stextsel = -1;
	stextsize = 1;
}
// 6A09E0: using guessed type char stextsize;
// 6A8A28: using guessed type int stextsel;

void __fastcall ClearSText(int s, int e)
{
	int v2; // edx
	int *v3; // eax

	if ( s < e )
	{
		v2 = e - s;
		v3 = &stext[s]._syoff;
		do
		{
			v3[37] = -1;
			*(v3 - 1) = 0;
			*v3 = 0;
			*((_BYTE *)v3 + 4) = 0;
			v3[33] = 0;
			*((_BYTE *)v3 + 136) = 0;
			v3[35] = 0;
			v3[36] = 0;
			v3 += 39;
			--v2;
		}
		while ( v2 );
	}
}

void __fastcall AddSLine(int y)
{
	int v1; // ecx

	v1 = y;
	stext[v1]._sx = 0;
	stext[v1]._syoff = 0;
	stext[v1]._sstr[0] = 0;
	stext[v1]._sline = 1;
}

void __fastcall AddSTextVal(int y, int val)
{
	stext[y]._sval = val;
}

void __fastcall OffsetSTextY(int y, int yo)
{
	stext[y]._syoff = yo;
}

void __fastcall AddSText(int x, int y, unsigned char j, char *str, int clr, int sel)
{
	int v6; // esi

	v6 = y;
	stext[v6]._syoff = 0;
	stext[v6]._sx = x;
	strcpy(stext[y]._sstr, str);
	stext[v6]._sline = 0;
	stext[v6]._sjust = j;
	_LOBYTE(stext[v6]._sclr) = clr;
	stext[v6]._ssel = sel;
}

void __cdecl StoreAutoPlace()
{
	int v0; // edi
	int v1; // eax
	int v2; // edx
	ItemStruct *v3; // ebp
	int v4; // esi
	int v5; // esi
	int v6; // esi
	int v7; // esi
	int v8; // esi
	int v9; // esi
	int v10; // esi
	int v11; // esi
	int *v12; // esi
	int v13; // esi
	int v14; // esi
	int v15; // esi
	int v16; // esi
	int v17; // esi
	signed int v19; // [esp+10h] [ebp-Ch]
	int v20; // [esp+14h] [ebp-8h]
	int v21; // [esp+18h] [ebp-4h]

	SetICursor(plr[myplr].HoldItem._iCurs + 12);
	v0 = icursH28;
	v1 = 0;
	v21 = icursW28;
	v20 = icursH28;
	if ( icursW28 == 1 )
	{
		if ( icursH28 == 1 )
		{
			v2 = myplr;
			if ( plr[myplr].HoldItem._iStatFlag && AllItemsList[plr[v2].HoldItem.IDidx].iUsable )
			{
				v19 = 0;
				v3 = plr[v2].SpdList;
				do
				{
					if ( v1 )
						break;
					if ( v3->_itype == -1 )
					{
						qmemcpy(v3, &plr[v2].HoldItem, sizeof(ItemStruct));
						v0 = v20;
						v1 = 1;
					}
					++v19;
					++v3;
				}
				while ( v19 < 8 );
			}
			v4 = 30;
			do
			{
				if ( v1 )
					break;
				v1 = AutoPlace(myplr, v4++, 1, 1, 1);
			}
			while ( v4 <= 39 );
			v5 = 20;
			do
			{
				if ( v1 )
					break;
				v1 = AutoPlace(myplr, v5++, 1, 1, 1);
			}
			while ( v5 <= 29 );
			v6 = 10;
			do
			{
				if ( v1 )
					break;
				v1 = AutoPlace(myplr, v6++, 1, 1, 1);
			}
			while ( v6 <= 19 );
			v7 = 0;
			while ( !v1 )
			{
				v1 = AutoPlace(myplr, v7++, 1, 1, 1);
				if ( v7 > 9 )
					goto LABEL_22;
			}
		}
		else
		{
LABEL_22:
			if ( v0 == 2 )
			{
				v8 = 29;
				do
				{
					if ( v1 )
						break;
					v1 = AutoPlace(myplr, v8--, 1, 2, 1);
				}
				while ( v8 >= 20 );
				v9 = 9;
				do
				{
					if ( v1 )
						break;
					v1 = AutoPlace(myplr, v9--, 1, 2, 1);
				}
				while ( v9 >= 0 );
				v10 = 19;
				while ( !v1 )
				{
					v1 = AutoPlace(myplr, v10--, 1, 2, 1);
					if ( v10 < 10 )
						goto LABEL_32;
				}
			}
			else
			{
LABEL_32:
				if ( v0 == 3 )
				{
					v11 = 0;
					while ( !v1 )
					{
						v1 = AutoPlace(myplr, v11++, 1, 3, 1);
						if ( v11 >= 20 )
							goto LABEL_36;
					}
				}
			}
		}
	}
	else
	{
LABEL_36:
		if ( v21 == 2 )
		{
			if ( v0 == 2 )
			{
				v12 = AP2x2Tbl;
				do
				{
					if ( v1 )
						break;
					v1 = AutoPlace(myplr, *v12, 2, 2, 1);
					++v12;
				}
				while ( (signed int)v12 < (signed int)&AP2x2Tbl[10] );
				v13 = 21;
				do
				{
					if ( v1 )
						break;
					v1 = AutoPlace(myplr, v13, 2, 2, 1);
					v13 += 2;
				}
				while ( v13 < 29 );
				v14 = 1;
				do
				{
					if ( v1 )
						break;
					v1 = AutoPlace(myplr, v14, 2, 2, 1);
					v14 += 2;
				}
				while ( v14 < 9 );
				v15 = 10;
				while ( !v1 )
				{
					v1 = AutoPlace(myplr, v15++, 2, 2, 1);
					if ( v15 >= 19 )
						goto LABEL_50;
				}
			}
			else
			{
LABEL_50:
				if ( v0 == 3 )
				{
					v16 = 0;
					do
					{
						if ( v1 )
							break;
						v1 = AutoPlace(myplr, v16++, 2, 3, 1);
					}
					while ( v16 < 9 );
					v17 = 10;
					do
					{
						if ( v1 )
							break;
						v1 = AutoPlace(myplr, v17++, 2, 3, 1);
					}
					while ( v17 < 19 );
				}
			}
		}
	}
}
// 48E9A8: using guessed type int AP2x2Tbl[10];

void __cdecl S_StartSmith()
{
	stextsize = 0;
	stextscrl = 0;
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
// 6A6BB8: using guessed type int stextscrl;

void __fastcall S_ScrollSBuy(int idx)
{
	int v1; // esi
	int v2; // edi
	char *v3; // esi
	char *v4; // eax
	int iclr; // [esp+Ch] [ebp-4h]

	v1 = idx;
	v2 = 5;
	ClearSText(5, 21);
	v3 = &smithitem[v1]._iMagical;
	stextup = 5;
	do
	{
		if ( *((_DWORD *)v3 - 13) != -1 )
		{
			_LOBYTE(iclr) = COL_WHITE;
			if ( *v3 )
				_LOBYTE(iclr) = COL_BLUE;
			if ( !*((_DWORD *)v3 + 74) )
				_LOBYTE(iclr) = COL_RED;
			v4 = v3 + 65;
			if ( !*v3 )
				v4 = v3 + 1;
			AddSText(20, v2, 0, v4, iclr, 1);
			AddSTextVal(v2, *((_DWORD *)v3 + 35));
			PrintStoreItem((ItemStruct *)(v3 - 60), v2 + 1, iclr);
			stextdown = v2;
			v3 += 368;
		}
		v2 += 4;
	}
	while ( v2 < 20 );
	if ( !stext[stextsel]._ssel && stextsel != 22 )
		stextsel = stextdown;
}
// 69F108: using guessed type int stextup;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

void __fastcall PrintStoreItem(ItemStruct *x, int l, char iclr)
{
	ItemStruct *v3; // esi
	bool v4; // zf
	char v5; // cl
	char v6; // cl
	int v7; // eax
	char v8; // al
	unsigned char v9; // al
	char v10; // al
	int v11; // edi
	char sstr[128]; // [esp+Ch] [ebp-84h]
	int y; // [esp+8Ch] [ebp-4h]

	sstr[0] = 0;
	v3 = x;
	v4 = x->_iIdentified == 0;
	y = l;
	if ( !v4 )
	{
		if ( x->_iMagical != 2 )
		{
			v5 = x->_iPrePower;
			if ( v5 != -1 )
			{
				PrintItemPower(v5, v3);
				strcat(sstr, tempstr);
			}
		}
		v6 = v3->_iSufPower;
		if ( v6 != -1 )
		{
			PrintItemPower(v6, v3);
			if ( sstr[0] )
				strcat(sstr, ",  ");
			strcat(sstr, tempstr);
		}
	}
	if ( v3->_iMiscId == IMISC_STAFF && v3->_iMaxCharges )
	{
		sprintf(tempstr, "Charges: %i/%i", v3->_iCharges, v3->_iMaxCharges);
		if ( sstr[0] )
			strcat(sstr, ",  ");
		strcat(sstr, tempstr);
	}
	if ( sstr[0] )
		AddSText(40, y++, 0, sstr, iclr, 0);
	sstr[0] = 0;
	if ( v3->_iClass == 1 )
		sprintf(sstr, "Damage: %i-%i  ", v3->_iMinDam, v3->_iMaxDam);
	if ( v3->_iClass == 2 )
		sprintf(sstr, "Armor: %i  ", v3->_iAC);
	v7 = v3->_iMaxDur;
	if ( v7 != 255 && v7 )
	{
		sprintf(tempstr, "Dur: %i/%i,  ", v3->_iDurability, v3->_iMaxDur);
		strcat(sstr, tempstr);
	}
	else
	{
		strcat(sstr, "Indestructible,  ");
	}
	if ( !v3->_itype )
		sstr[0] = 0;
	if ( v3->_iMinStr + (unsigned char)v3->_iMinMag + v3->_iMinDex )
	{
		strcpy(tempstr, "Required:");
		v8 = v3->_iMinStr;
		if ( v8 )
			sprintf(tempstr, "%s %i Str", tempstr, v8);
		v9 = v3->_iMinMag;
		if ( v9 )
			sprintf(tempstr, "%s %i Mag", tempstr, v9);
		v10 = v3->_iMinDex;
		if ( v10 )
			sprintf(tempstr, "%s %i Dex", tempstr, v10);
		strcat(sstr, tempstr);
	}
	else
	{
		strcat(sstr, "No required attributes");
	}
	v11 = y;
	AddSText(40, y, 0, sstr, iclr, 0);
	if ( v3->_iMagical == 2 )
	{
		if ( v3->_iIdentified )
			AddSText(40, v11 + 1, 0, "Unique Item", iclr, 0);
	}
}

void __cdecl S_StartSBuy()
{
	int v0; // ST10_4
	int v1; // eax
	int *v2; // ecx

	v0 = plr[myplr]._pGold;
	stextsize = 1;
	stextscrl = 1;
	stextsval = 0;
	sprintf(tempstr, "I have these items for sale :           Your gold : %i", v0);
	AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSBuy(stextsval);
	AddSText(0, 22, 1u, "Back", COL_WHITE, 0);
	OffsetSTextY(22, 6);
	v1 = 0;
	storenumh = 0;
	if ( smithitem[0]._itype != -1 )
	{
		v2 = &smithitem[0]._itype;
		do
		{
			v2 += 92;
			++v1;
		}
		while ( *v2 != -1 );
		storenumh = v1;
	}
	stextsmax = v1 - 4;
	if ( v1 - 4 < 0 )
		stextsmax = 0;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;

void __fastcall S_ScrollSPBuy(int idx)
{
	int v1; // esi
	int v2; // edi
	int v3; // eax
	int v4; // esi
	int *v5; // ecx
	char *v6; // esi
	int iclr; // [esp+Ch] [ebp-4h]

	v1 = idx;
	v2 = 5;
	ClearSText(5, 21);
	v3 = v1;
	v4 = 0;
	stextup = 5;
	if ( v3 )
	{
		v5 = &premiumitem[0]._itype;
		do
		{
			if ( *v5 != -1 )
				--v3;
			++v4;
			v5 += 92;
		}
		while ( v3 );
	}
	v6 = &premiumitem[v4]._iMagical;
	do
	{
		if ( (signed int)v6 >= (signed int)&premiumitem[6]._iMagical )
			break;
		if ( *((_DWORD *)v6 - 13) == -1 )
		{
			v2 -= 4;
		}
		else
		{
			_LOBYTE(iclr) = COL_WHITE;
			if ( *v6 )
				_LOBYTE(iclr) = COL_BLUE;
			if ( !*((_DWORD *)v6 + 74) )
				_LOBYTE(iclr) = COL_RED;
			AddSText(20, v2, 0, v6 + 65, iclr, 1);
			AddSTextVal(v2, *((_DWORD *)v6 + 35));
			PrintStoreItem((ItemStruct *)(v6 - 60), v2 + 1, iclr);
			stextdown = v2;
		}
		v2 += 4;
		v6 += 368;
	}
	while ( v2 < 20 );
	if ( !stext[stextsel]._ssel && stextsel != 22 )
		stextsel = stextdown;
}
// 69F108: using guessed type int stextup;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

bool __cdecl S_StartSPBuy()
{
	int *v0; // eax
	bool result; // al
	int v2; // ST10_4

	storenumh = 0;
	v0 = &premiumitem[0]._itype;
	do
	{
		if ( *v0 != -1 )
			++storenumh;
		v0 += 92;
	}
	while ( (signed int)v0 < (signed int)&premiumitem[6]._itype );
	if ( storenumh )
	{
		v2 = plr[myplr]._pGold;
		stextsval = 0;
		stextsize = 1;
		stextscrl = 1;
		sprintf(tempstr, "I have these premium items for sale :   Your gold : %i", v2);
		AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, 1u, "Back", COL_WHITE, 0);
		OffsetSTextY(22, 6);
		stextsmax = storenumh - 4;
		if ( storenumh - 4 < 0 )
			stextsmax = 0;
		S_ScrollSPBuy(stextsval);
		result = 1;
	}
	else
	{
		StartStore(STORE_SMITH);
		stextsel = 14;
		result = 0;
	}
	return result;
}
// 69F10C: using guessed type int storenumh;
// 69FB38: using guessed type int talker;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;
// 6A8A28: using guessed type int stextsel;

bool __fastcall SmithSellOk(int i)
{
	if ( plr[myplr].InvList[i]._itype != ITYPE_NONE
	  && plr[myplr].InvList[i]._itype
	  && plr[myplr].InvList[i]._itype != ITYPE_GOLD
	  && plr[myplr].InvList[i]._itype != ITYPE_0E
	  && plr[myplr].InvList[i]._itype != ITYPE_STAFF )
		return plr[myplr].InvList[i].IDidx != IDI_LAZSTAFF;
	else
		return 0;
}

void __fastcall S_ScrollSSell(int idx)
{
	int v1; // esi
	int v2; // edi
	char *v3; // esi
	int v4; // edx
	int v5; // [esp+Ch] [ebp-8h]
	int iclr; // [esp+10h] [ebp-4h]

	v1 = idx;
	v5 = idx;
	v2 = 5;
	ClearSText(5, 21);
	v3 = &storehold[v1]._iMagical;
	stextup = 5;
	do
	{
		if ( v5 >= storenumh )
			break;
		if ( *((_DWORD *)v3 - 13) != -1 )
		{
			_LOBYTE(iclr) = 0;
			if ( *v3 )
				_LOBYTE(iclr) = 1;
			if ( !*((_DWORD *)v3 + 74) )
				_LOBYTE(iclr) = 2;
			if ( *v3 && *((_DWORD *)v3 - 1) )
			{
				AddSText(20, v2, 0, v3 + 65, iclr, 1);
				v4 = *((_DWORD *)v3 + 35);
			}
			else
			{
				AddSText(20, v2, 0, v3 + 1, iclr, 1);
				v4 = *((_DWORD *)v3 + 34);
			}
			AddSTextVal(v2, v4);
			PrintStoreItem((ItemStruct *)(v3 - 60), v2 + 1, iclr);
			stextdown = v2;
		}
		++v5;
		v2 += 4;
		v3 += 368;
	}
	while ( v2 < 20 );
	stextsmax = storenumh - 4;
	if ( storenumh - 4 < 0 )
		stextsmax = 0;
}
// 69F108: using guessed type int stextup;
// 69F10C: using guessed type int storenumh;
// 6A09E4: using guessed type int stextsmax;
// 6AA700: using guessed type int stextdown;

void __cdecl S_StartSSell()
{
	int i; // eax
	bool sellok; // [esp+14h] [ebp-4h]

	stextsize = 1;
	sellok = 0;
	storenumh = 0;

	for(i = 0; i < 48; i++)
		storehold[i]._itype = -1;

	for(i = 0; i < plr[myplr]._pNumInv; i++)
	{
		if ( SmithSellOk(i) )
		{
			sellok = 1;
			qmemcpy(&storehold[storenumh], &plr[myplr].InvList[i], sizeof(ItemStruct));

			if ( storehold[storenumh]._iMagical && storehold[storenumh]._iIdentified )
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			if ( !(storehold[storenumh]._ivalue >>= 2) )
				storehold[storenumh]._ivalue = 1;

			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh++] = i;
		}
	}

	if ( sellok )
	{
		stextsmax = plr[myplr]._pNumInv;
		stextscrl = 1;
		stextsval = 0;
		sprintf(tempstr, "Which item is for sale?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	}
	else
	{
		stextscrl = 0;
		sprintf(tempstr, "You have nothing I want.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, 1, "Back", COL_WHITE, 1);
		OffsetSTextY(22, 6);
	}
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;

bool __fastcall SmithRepairOk(int i)
{
	if ( plr[myplr].InvList[i]._itype != ITYPE_NONE
	  && plr[myplr].InvList[i]._itype
	  && plr[myplr].InvList[i]._itype != ITYPE_GOLD
	  && plr[myplr].InvList[i]._itype != ITYPE_0E )
		return plr[myplr].InvList[i]._iDurability != plr[myplr].InvList[i]._iMaxDur;
	else
		return 0;
}

void __cdecl S_StartSRepair()
{
	int v0; // ebp
	int *v1; // eax
	int v2; // esi
	int v3; // eax
	int v4; // eax
	int v5; // eax
	int v6; // eax
	int v7; // edi
	//int v8; // eax
	int v9; // esi
	int v10; // eax
	int v11; // [esp-4h] [ebp-1Ch]
	signed int v12; // [esp+10h] [ebp-8h]
	int v13; // [esp+14h] [ebp-4h]

	v0 = 0;
	stextsize = 1;
	v12 = 0;
	storenumh = 0;
	v1 = &storehold[0]._itype;
	do
	{
		*v1 = -1;
		v1 += 92;
	}
	while ( (signed int)v1 < (signed int)&storehold[48]._itype );
	v2 = myplr;
	v3 = myplr;
	if ( plr[myplr].InvBody[0]._itype != -1 && plr[v3].InvBody[0]._iDurability != plr[v3].InvBody[0]._iMaxDur )
	{
		v12 = 1;
		AddStoreHoldRepair(plr[v3].InvBody, -1);
		v2 = myplr;
	}
	v4 = v2;
	if ( plr[v2].InvBody[6]._itype != -1 && plr[v4].InvBody[6]._iDurability != plr[v4].InvBody[6]._iMaxDur )
	{
		v12 = 1;
		AddStoreHoldRepair(&plr[v4].InvBody[6], -2);
		v2 = myplr;
	}
	v5 = v2;
	if ( plr[v2].InvBody[4]._itype != -1 && plr[v5].InvBody[4]._iDurability != plr[v5].InvBody[4]._iMaxDur )
	{
		v12 = 1;
		AddStoreHoldRepair(&plr[v5].InvBody[4], -3);
		v2 = myplr;
	}
	v6 = v2;
	if ( plr[v2].InvBody[5]._itype != -1 && plr[v6].InvBody[5]._iDurability != plr[v6].InvBody[5]._iMaxDur )
	{
		v12 = 1;
		AddStoreHoldRepair(&plr[v6].InvBody[5], -4);
		v2 = myplr;
	}
	v7 = 21720 * v2;
	if ( plr[v2]._pNumInv > 0 )
	{
		v13 = 0;
		do
		{
			//_LOBYTE(v8) = SmithRepairOk(v0);
			if ( SmithRepairOk(v0) )
			{
				v12 = 1;
				AddStoreHoldRepair((ItemStruct *)((char *)&plr[0].InvList[v13] + v7), v0);
				v2 = myplr;
			}
			++v13;
			v7 = 21720 * v2;
			++v0;
		}
		while ( v0 < plr[v2]._pNumInv );
	}
	v9 = v2;
	v11 = plr[v9]._pGold;
	if ( v12 )
	{
		stextsval = 0;
		v10 = plr[v9]._pNumInv;
		stextscrl = 1;
		stextsmax = v10;
		sprintf(tempstr, "Repair which item?            Your gold : %i", v11);
		AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
	}
	else
	{
		stextscrl = 0;
		sprintf(tempstr, "You have nothing to repair.            Your gold : %i", v11);
		AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
	}
	AddSText(0, 22, 1u, "Back", COL_WHITE, 1);
	OffsetSTextY(22, 6);
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;

void __fastcall AddStoreHoldRepair(ItemStruct *itm, int i)
{
	int v2; // ebx
	ItemStruct *v3; // ebp
	int v4; // ecx
	int v5; // eax

	v2 = storenumh;
	v3 = &storehold[storenumh];
	qmemcpy(&storehold[storenumh], itm, sizeof(ItemStruct));
	v4 = (unsigned char)v3->_iMagical;
	if ( (_BYTE)v4 && v3->_iIdentified )
		v3->_ivalue = 30 * v3->_iIvalue / 100;
	v5 = v3->_ivalue * (100 * (v3->_iMaxDur - v3->_iDurability) / v3->_iMaxDur) / 100;
	if ( !v5 )
	{
		if ( (_BYTE)v4 && v3->_iIdentified )
			return;
		v5 = 1;
	}
	if ( v5 > 1 )
		v5 >>= 1;
	v3->_iIvalue = v5;
	v3->_ivalue = v5;
	storehidx[v2] = i;
	storenumh = v2 + 1;
}
// 69F10C: using guessed type int storenumh;

void __cdecl S_StartWitch()
{
	stextsize = 0;
	stextscrl = 0;
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
// 6A6BB8: using guessed type int stextscrl;

void __fastcall S_ScrollWBuy(int idx)
{
	int v1; // esi
	int v2; // edi
	char *v3; // esi
	char *v4; // eax
	int iclr; // [esp+Ch] [ebp-4h]

	v1 = idx;
	v2 = 5;
	ClearSText(5, 21);
	v3 = &witchitem[v1]._iMagical;
	stextup = 5;
	do
	{
		if ( *((_DWORD *)v3 - 13) != -1 )
		{
			_LOBYTE(iclr) = 0;
			if ( *v3 )
				_LOBYTE(iclr) = 1;
			if ( !*((_DWORD *)v3 + 74) )
				_LOBYTE(iclr) = 2;
			v4 = v3 + 65;
			if ( !*v3 )
				v4 = v3 + 1;
			AddSText(20, v2, 0, v4, iclr, 1);
			AddSTextVal(v2, *((_DWORD *)v3 + 35));
			PrintStoreItem((ItemStruct *)(v3 - 60), v2 + 1, iclr);
			stextdown = v2;
			v3 += 368;
		}
		v2 += 4;
	}
	while ( v2 < 20 );
	if ( !stext[stextsel]._ssel && stextsel != 22 )
		stextsel = stextdown;
}
// 69F108: using guessed type int stextup;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

void __cdecl S_StartWBuy()
{
	int v0; // ST10_4
	int v1; // eax
	int *v2; // ecx

	v0 = plr[myplr]._pGold;
	stextsize = 1;
	stextscrl = 1;
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
	if ( witchitem[0]._itype != -1 )
	{
		v2 = &witchitem[0]._itype;
		do
		{
			v2 += 92;
			++v1;
		}
		while ( *v2 != -1 );
		storenumh = v1;
	}
	stextsmax = v1 - 4;
	if ( v1 - 4 < 0 )
		stextsmax = 0;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;

bool __fastcall WitchSellOk(int i)
{
	bool rv; // al
	ItemStruct *pI; // edx

	rv = 0;

	if ( i < 0 )
		pI = &plr[myplr].SpdList[~i]; // -(i+1)
	else
		pI = &plr[myplr].InvList[i];

	if ( pI->_itype == ITYPE_MISC )
		rv = 1;
	if ( pI->_itype == ITYPE_STAFF )
		rv = 1;
	if ( pI->IDidx >= IDI_FIRSTQUEST && pI->IDidx <= IDI_LASTQUEST )
		rv = 0;
	if ( pI->IDidx == IDI_LAZSTAFF )
		rv = 0;
	return rv;
}

void __cdecl S_StartWSell()
{
	int i; // eax
	bool sellok; // [esp+18h] [ebp-8h]

	stextsize = 1;
	sellok = 0;
	storenumh = 0;

	for(i = 0; i < 48; i++)
		storehold[i]._itype = -1;

	for(i = 0; i < plr[myplr]._pNumInv; i++)
	{
		if ( WitchSellOk(i) )
		{
			sellok = 1;
			qmemcpy(&storehold[storenumh], &plr[myplr].InvList[i], sizeof(ItemStruct));

			if ( storehold[storenumh]._iMagical && storehold[storenumh]._iIdentified )
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			if ( !(storehold[storenumh]._ivalue >>= 2) )
				storehold[storenumh]._ivalue = 1;

			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh++] = i;
		}
	}

	for(i = 0; i < MAXBELTITEMS; i++)
	{
		if ( plr[myplr].SpdList[i]._itype != -1 && WitchSellOk(~i) )
		{
			sellok = 1;
			qmemcpy(&storehold[storenumh], &plr[myplr].SpdList[i], sizeof(ItemStruct));

			if ( storehold[storenumh]._iMagical && storehold[storenumh]._iIdentified )
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			if ( !(storehold[storenumh]._ivalue >>= 2) )
				storehold[storenumh]._ivalue = 1;

			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh++] = ~i;
		}
	}

	if ( sellok )
	{
		stextscrl = 1;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Which item is for sale?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
	}
	else
	{
		stextscrl = 0;
		sprintf(tempstr, "You have nothing I want.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
	}

	AddSText(0, 22, 1, "Back", COL_WHITE, 1);
	OffsetSTextY(22, 6);
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;

bool __fastcall WitchRechargeOk(int i)
{
	bool rv; // al

	rv = 0;
	if ( plr[myplr].InvList[i]._itype == ITYPE_STAFF
	  && plr[myplr].InvList[i]._iCharges != plr[myplr].InvList[i]._iMaxCharges )
	{
		rv = 1;
	}
	return rv;
}

void __fastcall AddStoreHoldRecharge(ItemStruct itm, int i)
{
	int v2; // ebx
	int v3; // eax
	char v4; // ST10_1
	int v5; // ecx
	int v6; // eax

	v2 = storenumh;
	v3 = spelldata[itm._iSpell].sStaffCost;
	v4 = i;
	qmemcpy(&storehold[storenumh], &itm, sizeof(ItemStruct));
	storehold[v2]._ivalue += v3;
	v5 = storenumh;
	v6 = storehold[v2]._ivalue
	   * (100
		* (storehold[v2]._iMaxCharges - storehold[v2]._iCharges)
		/ storehold[v2]._iMaxCharges)
	   / 100 >> 1;
	++storenumh;
	storehold[v2]._ivalue = v6;
	storehold[v2]._iIvalue = v6;
	storehidx[v5] = v4;
}
// 69F108: using guessed type int stextup;
// 69F10C: using guessed type int storenumh;

void __cdecl S_StartWRecharge()
{
	int *v0; // eax
	int v1; // ebp
	int v2; // eax
	//int v3; // eax
	ItemStruct v4; // [esp-170h] [ebp-18Ch]
	int v5; // [esp-4h] [ebp-20h]
	int inv_num; // [esp+10h] [ebp-Ch]
	ItemStruct *v7; // [esp+14h] [ebp-8h]
	int v8; // [esp+18h] [ebp-4h]

	stextsize = 1;
	v8 = 0;
	storenumh = 0;
	v0 = &storehold[0]._itype;
	do
	{
		*v0 = -1;
		v0 += 92;
	}
	while ( (signed int)v0 < (signed int)&storehold[48]._itype );
	v1 = myplr;
	if ( plr[myplr].InvBody[4]._itype == ITYPE_STAFF && plr[v1].InvBody[4]._iCharges != plr[v1].InvBody[4]._iMaxCharges )
	{
		v8 = 1;
		qmemcpy(&v4, &plr[v1].InvBody[4], sizeof(v4));
		AddStoreHoldRecharge(v4, -1);
	}
	v2 = plr[v1]._pNumInv;
	inv_num = 0;
	if ( v2 > 0 )
	{
		v7 = plr[v1].InvList;
		do
		{
			//_LOBYTE(v3) = WitchRechargeOk(inv_num);
			if ( WitchRechargeOk(inv_num) )
			{
				v8 = 1;
				qmemcpy(&v4, v7, sizeof(v4));
				AddStoreHoldRecharge(v4, inv_num);
			}
			++inv_num;
			v2 = plr[v1]._pNumInv;
			++v7;
		}
		while ( inv_num < v2 );
	}
	v5 = plr[v1]._pGold;
	if ( v8 )
	{
		stextscrl = 1;
		stextsval = 0;
		stextsmax = v2;
		sprintf(tempstr, "Recharge which item?            Your gold : %i", v5);
		AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
	}
	else
	{
		stextscrl = 0;
		sprintf(tempstr, "You have nothing to recharge.            Your gold : %i", v5);
		AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
	}
	AddSText(0, 22, 1u, "Back", COL_WHITE, 1);
	OffsetSTextY(22, 6);
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartNoMoney()
{
	StartStore((unsigned char)stextshold);
	stextscrl = 0;
	stextsize = 1;
	ClearSText(5, 23);
	AddSText(0, 14, 1u, "You do not have enough gold", COL_WHITE, 1);
}
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartNoRoom()
{
	StartStore((unsigned char)stextshold);
	stextscrl = 0;
	ClearSText(5, 23);
	AddSText(0, 14, 1u, "You do not have enough room in inventory", COL_WHITE, 1);
}
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartConfirm()
{
	BOOL idprint; // esi
	char iclr; // [esp+Ch] [ebp-4h]

	StartStore(stextshold);
	stextscrl = 0;
	ClearSText(5, 23);
	iclr = COL_WHITE;

	if ( plr[myplr].HoldItem._iMagical )
		iclr = COL_BLUE;
	if ( !plr[myplr].HoldItem._iStatFlag )
		iclr = COL_RED;

	idprint = plr[myplr].HoldItem._iMagical != 0;

	if ( stextshold == STORE_SIDENTIFY )
		idprint = 0;
	if ( plr[myplr].HoldItem._iMagical && !plr[myplr].HoldItem._iIdentified )
	{
		if ( stextshold == STORE_SSELL )
			idprint = 0;
		if ( stextshold == STORE_WSELL )
			idprint = 0;
		if ( stextshold == STORE_SREPAIR )
			idprint = 0;
		if ( stextshold == STORE_WRECHARGE )
			idprint = 0;
	}
	if ( idprint )
		AddSText(20, 8, 0, plr[myplr].HoldItem._iIName, iclr, 0);
	else
		AddSText(20, 8, 0, plr[myplr].HoldItem._iName, iclr, 0);

	AddSTextVal(8, plr[myplr].HoldItem._iIvalue);
	PrintStoreItem(&plr[myplr].HoldItem, 9, iclr);

	if ( stextshold > STORE_WRECHARGE )
	{
		if ( stextshold == STORE_BBOY )
		{
			strcpy(tempstr, "Do we have a deal?");
			goto LABEL_37;
		}
		if ( stextshold != STORE_HBUY )
		{
			if ( stextshold == STORE_SIDENTIFY )
			{
				strcpy(tempstr, "Are you sure you want to identify this item?");
				goto LABEL_37;
			}
			if ( stextshold != STORE_SPBUY )
				goto LABEL_37;
		}
LABEL_34:
		strcpy(tempstr, "Are you sure you want to buy this item?");
		goto LABEL_37;
	}
	switch ( stextshold )
	{
		case STORE_WRECHARGE:
			strcpy(tempstr, "Are you sure you want to recharge this item?");
			break;
		case STORE_SBUY:
			goto LABEL_34;
		case STORE_SSELL:
LABEL_27:
			strcpy(tempstr, "Are you sure you want to sell this item?");
			break;
		case STORE_SREPAIR:
			strcpy(tempstr, "Are you sure you want to repair this item?");
			break;
		case STORE_WBUY:
			goto LABEL_34;
		case STORE_WSELL:
			goto LABEL_27;
	}
LABEL_37:
	AddSText(0, 15, 1u, tempstr, COL_WHITE, 0);
	AddSText(0, 18, 1u, "Yes", COL_WHITE, 1);
	AddSText(0, 20, 1u, "No", COL_WHITE, 1);
}
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartBoy()
{
	stextsize = 0;
	stextscrl = 0;
	AddSText(0, 2, 1u, "Wirt the Peg-legged boy", COL_GOLD, 0);
	AddSLine(5);
	if ( boyitem._itype == -1 )
	{
		AddSText(0, 12, 1u, "Talk to Wirt", COL_BLUE, 1);
		AddSText(0, 18, 1u, "Say goodbye", COL_WHITE, 1);
	}
	else
	{
		AddSText(0, 8, 1u, "Talk to Wirt", COL_BLUE, 1);
		AddSText(0, 12, 1u, "I have something for sale,", COL_GOLD, 0);
		AddSText(0, 14, 1u, "but it will cost 50 gold", COL_GOLD, 0);
		AddSText(0, 16, 1u, "just to take a look. ", COL_GOLD, 0);
		AddSText(0, 18, 1u, "What have you got?", COL_WHITE, 1);
		AddSText(0, 20, 1u, "Say goodbye", COL_WHITE, 1);
	}
}
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartBBoy()
{
	int iclr; // esi

	stextsize = 1;
	stextscrl = 0;
	sprintf(tempstr, "I have this item for sale :           Your gold : %i", plr[myplr]._pGold);
	AddSText(0, 1, 1u, tempstr, COL_GOLD, 0);
	AddSLine(3);
	AddSLine(21);
	iclr = COL_WHITE;

	if ( boyitem._iMagical )
		iclr = COL_BLUE;
	if ( !boyitem._iStatFlag )
		iclr = COL_RED;
	if ( boyitem._iMagical )
		AddSText(20, 10, 0, boyitem._iIName, iclr, 1);
	else
		AddSText(20, 10, 0, boyitem._iName, iclr, 1);

	AddSTextVal(10, boyitem._iIvalue + (boyitem._iIvalue >> 1));
	PrintStoreItem(&boyitem, 11, iclr);
	AddSText(0, 22, 1u, "Leave", COL_WHITE, 1);
	OffsetSTextY(22, 6);
}
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartHealer()
{
	stextsize = 0;
	stextscrl = 0;
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
// 6A6BB8: using guessed type int stextscrl;

void __fastcall S_ScrollHBuy(int idx)
{
	int v1; // esi
	int v2; // edi
	int *v3; // esi
	int iclr; // [esp+8h] [ebp-4h]

	v1 = idx;
	v2 = 5;
	ClearSText(5, 21);
	stextup = 5;
	v3 = &healitem[v1]._iStatFlag;
	do
	{
		if ( *(v3 - 87) != -1 )
		{
			_LOBYTE(iclr) = COL_WHITE;
			if ( !*v3 )
				_LOBYTE(iclr) = COL_RED;
			AddSText(20, v2, 0, (char *)v3 - 295, iclr, 1);
			AddSTextVal(v2, *(v3 - 39));
			PrintStoreItem((ItemStruct *)(v3 - 89), v2 + 1, iclr);
			stextdown = v2;
			v3 += 92;
		}
		v2 += 4;
	}
	while ( v2 < 20 );
	if ( !stext[stextsel]._ssel && stextsel != 22 )
		stextsel = stextdown;
}
// 69F108: using guessed type int stextup;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

void __cdecl S_StartHBuy()
{
	int v0; // ST10_4
	int v1; // eax
	int *v2; // ecx

	v0 = plr[myplr]._pGold;
	stextsize = 1;
	stextscrl = 1;
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
	if ( healitem[0]._itype != -1 )
	{
		v2 = &healitem[0]._itype;
		do
		{
			v2 += 92;
			++v1;
		}
		while ( *v2 != -1 );
		storenumh = v1;
	}
	stextsmax = v1 - 4;
	if ( v1 - 4 < 0 )
		stextsmax = 0;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartStory()
{
	stextsize = 0;
	stextscrl = 0;
	AddSText(0, 2, 1u, "The Town Elder", COL_GOLD, 0);
	AddSText(0, 9, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 12, 1u, "Talk to Cain", COL_BLUE, 1);
	AddSText(0, 14, 1u, "Identify an item", COL_WHITE, 1);
	AddSText(0, 18, 1u, "Say goodbye", COL_WHITE, 1);
	AddSLine(5);
}
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;

bool __fastcall IdItemOk(ItemStruct *i)
{
	bool result; // al

	result = 0;
	if ( i->_itype != -1 )
	{
		if ( i->_iMagical )
			result = i->_iIdentified == 0;
	}
	return result;
}

void __fastcall AddStoreHoldId(ItemStruct itm, int i)
{
	qmemcpy(&storehold[storenumh], &itm, sizeof(ItemStruct));
	storehold[storenumh]._ivalue = 100;
	storehold[storenumh]._iIvalue = 100;
	storehidx[storenumh++] = i;
}
// 69F108: using guessed type int stextup;
// 69F10C: using guessed type int storenumh;

void __cdecl S_StartSIdentify()
{
	ItemStruct itm; // [esp-170h] [ebp-18Ch]
	bool idok; // [esp+10h] [ebp-Ch]
	int i; // [esp+14h] [ebp-8h]

	idok = 0;
	storenumh = 0;
	stextsize = 1;

	for(i = 0; i < 48; i++)
		storehold[i]._itype = -1;

	if ( IdItemOk(plr[myplr].InvBody) )
	{
		idok = 1;
		qmemcpy(&itm, plr[myplr].InvBody, sizeof(ItemStruct));
		AddStoreHoldId(itm, -1);
	}
	if ( IdItemOk(&plr[myplr].InvBody[6]) )
	{
		idok = 1;
		qmemcpy(&itm, &plr[myplr].InvBody[6], sizeof(ItemStruct));
		AddStoreHoldId(itm, -2);
	}
	if ( IdItemOk(&plr[myplr].InvBody[4]) )
	{
		idok = 1;
		qmemcpy(&itm, &plr[myplr].InvBody[4], sizeof(ItemStruct));
		AddStoreHoldId(itm, -3);
	}
	if ( IdItemOk(&plr[myplr].InvBody[5]) )
	{
		idok = 1;
		qmemcpy(&itm, &plr[myplr].InvBody[5], sizeof(ItemStruct));
		AddStoreHoldId(itm, -4);
	}
	if ( IdItemOk(&plr[myplr].InvBody[1]) )
	{
		idok = 1;
		qmemcpy(&itm, &plr[myplr].InvBody[1], sizeof(ItemStruct));
		AddStoreHoldId(itm, -5);
	}
	if ( IdItemOk(&plr[myplr].InvBody[2]) )
	{
		idok = 1;
		qmemcpy(&itm, &plr[myplr].InvBody[2], sizeof(ItemStruct));
		AddStoreHoldId(itm, -6);
	}
	if ( IdItemOk(&plr[myplr].InvBody[3]) )
	{
		idok = 1;
		qmemcpy(&itm, &plr[myplr].InvBody[3], sizeof(ItemStruct));
		AddStoreHoldId(itm, -7);
	}

	for(i = 0; i < plr[myplr]._pNumInv; i++)
	{
		if ( IdItemOk(&plr[myplr].InvList[i]) )
		{
			idok = 1;
			qmemcpy(&itm, &plr[myplr].InvList[i], sizeof(ItemStruct));
			AddStoreHoldId(itm, i);
		}
	}

	if ( idok )
	{
		stextscrl = 1;
		stextsval = 0;
		stextsmax = plr[myplr]._pNumInv;
		sprintf(tempstr, "Identify which item?            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
		S_ScrollSSell(stextsval);
	}
	else
	{
		stextscrl = 0;
		sprintf(tempstr, "You have nothing to identify.            Your gold : %i", plr[myplr]._pGold);
		AddSText(0, 1, 1, tempstr, COL_GOLD, 0);
		AddSLine(3);
		AddSLine(21);
	}

	AddSText(0, 22, 1, "Back", COL_WHITE, 1);
	OffsetSTextY(22, 6);
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartIdShow()
{
	char iclr; // [esp+4h] [ebp-4h]

	StartStore(stextshold);
	stextscrl = 0;
	ClearSText(5, 23);
	iclr = COL_WHITE;

	if ( plr[myplr].HoldItem._iMagical )
		iclr = COL_BLUE;
	if ( !plr[myplr].HoldItem._iStatFlag )
		iclr = COL_RED;

	AddSText(0, 7, 1u, "This item is:", COL_WHITE, 0);
	AddSText(20, 11, 0, plr[myplr].HoldItem._iIName, iclr, 0);
	PrintStoreItem(&plr[myplr].HoldItem, 12, iclr);
	AddSText(0, 18, 1u, "Done", COL_WHITE, 1);
}
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartTalk()
{
	int *v0; // edi
	signed int v1; // eax
	int v2; // edx
	int *v3; // ecx
	char **v4; // ebp
	int v5; // esi
	int v6; // ebx
	signed int v7; // [esp-4h] [ebp-1Ch]
	signed int v8; // [esp+10h] [ebp-8h]
	int y; // [esp+14h] [ebp-4h]

	stextsize = 0;
	stextscrl = 0;
	sprintf(tempstr, "Talk to %s", talkname[talker]);
	AddSText(0, 2, 1u, tempstr, COL_GOLD, 0);
	AddSLine(5);
	v0 = &quests[0]._qlog;
	v1 = 0;
	v2 = 0;
	v3 = &quests[0]._qlog;
	do
	{
		if ( *((_BYTE *)v3 - 18) == 2 && *((_DWORD *)&Qtalklist[0]._qinfra + v2 + 16 * talker) != -1 && *v3 )
			++v1;
		v3 += 6;
		++v2;
	}
	while ( (signed int)v3 < (signed int)&quests[16]._qlog );
	if ( v1 <= 6 )
	{
		v7 = 15;
		v8 = 2;
	}
	else
	{
		v1 >>= 1;
		v7 = 14;
		v8 = 1;
	}
	v4 = &questlist[0]._qlstr;
	v5 = v7 - v1;
	v6 = 0;
	y = v7 - v1 - 2;
	do
	{
		if ( *((_BYTE *)v0 - 18) == 2 && *((_DWORD *)&Qtalklist[0]._qinfra + v6 + 16 * talker) != -1 && *v0 )
		{
			AddSText(0, v5, 1u, *v4, COL_WHITE, 1);
			v5 += v8;
		}
		v0 += 6;
		++v6;
		v4 += 5;
	}
	while ( (signed int)v0 < (signed int)&quests[16]._qlog );
	AddSText(0, y, 1u, "Gossip", COL_BLUE, 1);
	AddSText(0, 22, 1u, "Back", COL_WHITE, 1);
}
// 69FB38: using guessed type int talker;
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartTavern()
{
	stextsize = 0;
	stextscrl = 0;
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
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartBarMaid()
{
	stextsize = 0;
	stextscrl = 0;
	AddSText(0, 2, 1u, "Gillian", COL_GOLD, 0);
	AddSText(0, 9, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 12, 1u, "Talk to Gillian", COL_BLUE, 1);
	AddSText(0, 18, 1u, "Say goodbye", COL_WHITE, 1);
	AddSLine(5);
	storenumh = 20;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;

void __cdecl S_StartDrunk()
{
	stextsize = 0;
	stextscrl = 0;
	AddSText(0, 2, 1u, "Farnham the Drunk", COL_GOLD, 0);
	AddSText(0, 9, 1u, "Would you like to:", COL_GOLD, 0);
	AddSText(0, 12, 1u, "Talk to Farnham", COL_BLUE, 1);
	AddSText(0, 18, 1u, "Say Goodbye", COL_WHITE, 1);
	AddSLine(5);
	storenumh = 20;
}
// 69F10C: using guessed type int storenumh;
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;

void __fastcall StartStore(char s)
{
	char t; // bl
	int i; // ecx

	for ( t = s; ; t = 1 )
	{
		sbookflag = 0;
		invflag = 0;
		chrflag = 0;
		questlog = 0;
		dropGoldFlag = 0;
		ClearSText(0, 24);
		ReleaseStoreBtn();
		switch ( t )
		{
			case STORE_SMITH:
				S_StartSmith();
				break;
			case STORE_SBUY:
				if ( storenumh > 0 )
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
				if ( storenumh > 0 )
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
				if ( storenumh > 0 )
					S_StartHBuy();
				break;
			case STORE_SIDENTIFY:
				S_StartSIdentify();
				break;
			case STORE_SPBUY:
				if ( !S_StartSPBuy() )
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
			default:
				break;
		}

		for(i = 0; i < 24; i++)
		{
			if ( stext[i]._ssel )
				break;
		}

		stextsel = i == 24 ? -1 : i;
		stextflag = t;
		if ( t != 2 || storenumh )
			break;
	}
}
// 4B84DC: using guessed type int dropGoldFlag;
// 4B8968: using guessed type int sbookflag;
// 69BD04: using guessed type int questlog;
// 69F10C: using guessed type int storenumh;
// 6A8A28: using guessed type int stextsel;
// 6AA705: using guessed type char stextflag;

void __cdecl DrawSText()
{
	int i; // edi

	if ( stextsize )
		DrawQTextBack();
	else
		DrawSTextBack();
	if ( !stextscrl )
		goto LABEL_19;
	if ( stextflag > (signed int)STORE_WRECHARGE )
	{
		switch ( stextflag )
		{
			case STORE_HBUY:
				S_ScrollHBuy(stextsval);
				break;
			case STORE_SIDENTIFY:
				goto LABEL_17;
			case STORE_SPBUY:
				S_ScrollSPBuy(stextsval);
				break;
		}
	}
	else
	{
		if ( stextflag >= (signed int)STORE_WSELL )
			goto LABEL_17;
		if ( stextflag == STORE_SBUY )
		{
			S_ScrollSBuy(stextsval);
			goto LABEL_19;
		}
		if ( stextflag > (signed int)STORE_SBUY )
		{
			if ( stextflag > (signed int)STORE_SREPAIR )
			{
				if ( stextflag == STORE_WBUY )
					S_ScrollWBuy(stextsval);
				goto LABEL_19;
			}
LABEL_17:
			S_ScrollSSell(stextsval);
			goto LABEL_19;
		}
	}
LABEL_19:

	for(i = 0; i < 24; i++)
	{
		if ( stext[i]._sline )
			DrawSLine(i);
		if ( stext[i]._sstr )
			PrintSString(stext[i]._sx, i, stext[i]._sjust, stext[i]._sstr, stext[i]._sclr, stext[i]._sval);
	}

	if ( stextscrl )
		DrawSArrows(4, 20);
	InStoreFlag = (InStoreFlag & 7) + 1;
}
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;
// 6AA705: using guessed type char stextflag;

void __cdecl STextESC()
{
	char v0; // cl
	char v1; // cl
	char v2; // cl

	if ( qtextflag )
	{
		qtextflag = 0;
		if ( leveltype == DTYPE_TOWN )
			sfx_stop();
	}
	else
	{
		switch ( stextflag )
		{
			case STORE_SMITH:
			case STORE_WITCH:
			case STORE_BOY:
			case STORE_BBOY:
			case STORE_HEALER:
			case STORE_STORY:
			case STORE_TAVERN:
			case STORE_DRUNK:
			case STORE_BARMAID:
				stextflag = 0;
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
			default:
				return;
		}
	}
}
// 5BB1ED: using guessed type char leveltype;
// 646D00: using guessed type char qtextflag;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;
// 6AA705: using guessed type char stextflag;

void __cdecl STextUp()
{
	int v0; // eax

	PlaySFX(IS_TITLEMOV);
	if ( stextsel != -1 )
	{
		if ( stextscrl )
		{
			if ( stextsel == stextup )
			{
				if ( stextsval )
					--stextsval;
				return;
			}
			v0 = stextsel - 1;
			stextsel = v0;
			if ( stext[v0]._ssel )
				return;
			do
			{
				if ( v0 )
					--v0;
				else
					v0 = 23;
			}
			while ( !stext[v0]._ssel );
LABEL_20:
			stextsel = v0;
			return;
		}
		if ( stextsel )
			v0 = stextsel - 1;
		else
			v0 = 23;
		stextsel = v0;
		if ( !stext[v0]._ssel )
		{
			do
			{
				if ( v0 )
					--v0;
				else
					v0 = 23;
			}
			while ( !stext[v0]._ssel );
			goto LABEL_20;
		}
	}
}
// 69F108: using guessed type int stextup;
// 6A6BB8: using guessed type int stextscrl;
// 6A8A28: using guessed type int stextsel;

void __cdecl STextDown()
{
	int v0; // eax

	PlaySFX(IS_TITLEMOV);
	if ( stextsel != -1 )
	{
		if ( stextscrl )
		{
			if ( stextsel == stextdown )
			{
				if ( stextsval < stextsmax )
					++stextsval;
				return;
			}
			v0 = stextsel + 1;
			stextsel = v0;
			if ( stext[v0]._ssel )
				return;
			do
			{
				if ( v0 == 23 )
					v0 = 0;
				else
					++v0;
			}
			while ( !stext[v0]._ssel );
LABEL_20:
			stextsel = v0;
			return;
		}
		if ( stextsel == 23 )
			v0 = 0;
		else
			v0 = stextsel + 1;
		stextsel = v0;
		if ( !stext[v0]._ssel )
		{
			do
			{
				if ( v0 == 23 )
					v0 = 0;
				else
					++v0;
			}
			while ( !stext[v0]._ssel );
			goto LABEL_20;
		}
	}
}
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

void __cdecl STextPrior()
{
	PlaySFX(IS_TITLEMOV);
	if ( stextsel != -1 && stextscrl )
	{
		if ( stextsel == stextup )
		{
			if ( stextsval )
			{
				stextsval -= 4;
				if ( stextsval < 0 )
					stextsval = 0;
			}
		}
		else
		{
			stextsel = stextup;
		}
	}
}
// 69F108: using guessed type int stextup;
// 6A6BB8: using guessed type int stextscrl;
// 6A8A28: using guessed type int stextsel;

void __cdecl STextNext()
{
	PlaySFX(IS_TITLEMOV);
	if ( stextsel != -1 && stextscrl )
	{
		if ( stextsel == stextdown )
		{
			if ( stextsval < stextsmax )
				stextsval += 4;
			if ( stextsval > stextsmax )
				stextsval = stextsmax;
		}
		else
		{
			stextsel = stextdown;
		}
	}
}
// 6A09E4: using guessed type int stextsmax;
// 6A6BB8: using guessed type int stextscrl;
// 6A8A28: using guessed type int stextsel;
// 6AA700: using guessed type int stextdown;

void __cdecl S_SmithEnter()
{
	int v0; // ecx

	v0 = 10;
	if ( stextsel == 10 )
	{
		talker = 0;
		stextlhold = 10;
		stextshold = 1;
		gossipstart = QUEST_GRISWOLD2;
		gossipend = QUEST_GRISWOLD13;
		_LOBYTE(v0) = STORE_GOSSIP;
		goto LABEL_13;
	}
	v0 = STORE_SBUY;
	switch ( stextsel )
	{
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
			stextflag = 0;
			break;
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void __fastcall SetGoldCurs(int pnum, int i)
{
	if ( plr[pnum].InvList[i]._ivalue < 2500 )
	{
		if ( plr[pnum].InvList[i]._ivalue > 1000 )
			plr[pnum].InvList[i]._iCurs = ICURS_GOLD_MEDIUM;
		else
			plr[pnum].InvList[i]._iCurs = ICURS_GOLD_SMALL;
	}
	else
	{
		plr[pnum].InvList[i]._iCurs = ICURS_GOLD_LARGE;
	}
}

void __fastcall SetSpdbarGoldCurs(int pnum, int i)
{
	if ( plr[pnum].SpdList[i]._ivalue < 2500 )
	{
		if ( plr[pnum].SpdList[i]._ivalue > 1000 )
			plr[pnum].SpdList[i]._iCurs = ICURS_GOLD_MEDIUM;
		else
			plr[pnum].SpdList[i]._iCurs = ICURS_GOLD_SMALL;
	}
	else
	{
		plr[pnum].SpdList[i]._iCurs = ICURS_GOLD_LARGE;
	}
}

void __fastcall TakePlrsMoney(int cost)
{
	int v1; // edi
	int v2; // eax
	int v3; // esi
	int v4; // ebx
	int v5; // eax
	_DWORD *v6; // ecx
	int v7; // eax
	int v8; // ebx
	int v9; // eax
	_DWORD *v10; // ecx
	int v11; // eax
	signed int v12; // ebx
	int v13; // eax
	int v14; // eax
	_DWORD *v15; // ecx
	int v16; // eax
	signed int v17; // ebx
	int v18; // eax
	int v19; // eax
	_DWORD *v20; // ecx
	int v21; // eax

	v1 = cost;
	v2 = CalculateGold(myplr);
	v3 = myplr;
	v4 = 0;
	plr[myplr]._pGold = v2 - v1;
	while ( v1 > 0 )
	{
		v5 = 368 * v4 + 21720 * v3;
		if ( *(int *)((char *)&plr[0].SpdList[0]._itype + v5) == ITYPE_GOLD )
		{
			v6 = (unsigned int *)((char *)&plr[0].SpdList[0]._ivalue + v5);
			v7 = *(int *)((char *)&plr[0].SpdList[0]._ivalue + v5);
			if ( v7 != 5000 )
			{
				if ( v1 >= v7 )
				{
					v1 -= v7;
					RemoveSpdBarItem(v3, v4);
					v3 = myplr;
					v4 = -1;
				}
				else
				{
					*v6 = v7 - v1;
					SetSpdbarGoldCurs(v3, v4);
					v1 = 0;
				}
			}
		}
		if ( ++v4 >= MAXBELTITEMS )
		{
			if ( v1 > 0 )
			{
				v8 = 0;
				do
				{
					if ( v1 <= 0 )
						break;
					v9 = 368 * v8 + 21720 * v3;
					if ( *(int *)((char *)&plr[0].SpdList[0]._itype + v9) == ITYPE_GOLD )
					{
						v10 = (unsigned int *)((char *)&plr[0].SpdList[0]._ivalue + v9);
						v11 = *(int *)((char *)&plr[0].SpdList[0]._ivalue + v9);
						if ( v1 >= v11 )
						{
							v1 -= v11;
							RemoveSpdBarItem(v3, v8);
							v3 = myplr;
							v8 = -1;
						}
						else
						{
							*v10 = v11 - v1;
							SetSpdbarGoldCurs(v3, v8);
							v1 = 0;
						}
					}
					++v8;
				}
				while ( v8 < MAXBELTITEMS );
			}
			break;
		}
	}
	v12 = 0;
	drawpanflag = 255;
	if ( v1 > 0 )
	{
		v13 = 21720 * v3;
		if ( plr[v3]._pNumInv <= 0 )
		{
LABEL_26:
			v17 = 0;
			if ( v1 > 0 )
			{
				v18 = 21720 * v3;
				if ( plr[v3]._pNumInv > 0 )
				{
					do
					{
						if ( v1 <= 0 )
							break;
						v19 = 368 * v17 + v18;
						if ( *(int *)((char *)&plr[0].InvList[0]._itype + v19) == ITYPE_GOLD )
						{
							v20 = (unsigned int *)((char *)&plr[0].InvList[0]._ivalue + v19);
							v21 = *(int *)((char *)&plr[0].InvList[0]._ivalue + v19);
							if ( v1 >= v21 )
							{
								v1 -= v21;
								RemoveInvItem(v3, v17);
								v3 = myplr;
								v17 = -1;
							}
							else
							{
								*v20 = v21 - v1;
								SetGoldCurs(v3, v17);
								v1 = 0;
							}
						}
						++v17;
						v18 = 21720 * v3;
					}
					while ( v17 < plr[v3]._pNumInv );
				}
			}
		}
		else
		{
			while ( v1 > 0 )
			{
				v14 = 368 * v12 + v13;
				if ( *(int *)((char *)&plr[0].InvList[0]._itype + v14) == ITYPE_GOLD )
				{
					v15 = (unsigned int *)((char *)&plr[0].InvList[0]._ivalue + v14);
					v16 = *(int *)((char *)&plr[0].InvList[0]._ivalue + v14);
					if ( v16 != 5000 )
					{
						if ( v1 >= v16 )
						{
							v1 -= v16;
							RemoveInvItem(v3, v12);
							v3 = myplr;
							v12 = -1;
						}
						else
						{
							*v15 = v16 - v1;
							SetGoldCurs(v3, v12);
							v1 = 0;
						}
					}
				}
				++v12;
				v13 = 21720 * v3;
				if ( v12 >= plr[v3]._pNumInv )
					goto LABEL_26;
			}
		}
	}
}
// 52571C: using guessed type int drawpanflag;

void __cdecl SmithBuyItem()
{
	int idx; // eax
	ItemStruct *v1; // edx
	ItemStruct *v2; // edi
	bool v3; // zf

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	if ( !plr[myplr].HoldItem._iMagical )
		plr[myplr].HoldItem._iIdentified = 0;
	StoreAutoPlace();
	idx = stextvhold + ((stextlhold - stextup) >> 2);
	if ( idx == 19 )
	{
		smithitem[19]._itype = -1;
	}
	else
	{
		if ( smithitem[idx + 1]._itype != -1 )
		{
			v1 = &smithitem[idx];
			do
			{
				v2 = v1;
				++v1;
				++idx;
				v3 = v1[1]._itype == -1;
				qmemcpy(v2, v1, sizeof(ItemStruct));
			}
			while ( !v3 );
		}
		smithitem[idx]._itype = -1;
	}
	CalcPlrInv(myplr, 1u);
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void __cdecl S_SBuyEnter()
{
	int v0; // eax
	int idx; // ecx
	int done; // eax
	int i; // esi
	char v4; // cl

	if ( stextsel == 22 )
	{
		StartStore(STORE_SMITH);
		stextsel = 12;
	}
	else
	{
		stextlhold = stextsel;
		stextvhold = stextsval;
		stextshold = 2;
		v0 = myplr;
		idx = stextsval + ((stextsel - stextup) >> 2);
		if ( plr[myplr]._pGold >= smithitem[idx]._iIvalue )
		{
			qmemcpy(&plr[v0].HoldItem, &smithitem[idx], sizeof(plr[v0].HoldItem));
			SetCursor(plr[v0].HoldItem._iCurs + 12);
			done = 0;
			i = 0;
			do
			{
				if ( done )
					goto LABEL_9;
				done = AutoPlace(myplr, i++, cursW / 28, cursH / 28, 0);
			}
			while ( i < 40 );
			if ( done )
			{
LABEL_9:
				v4 = STORE_CONFIRM;
				goto LABEL_11;
			}
			v4 = STORE_NOROOM;
LABEL_11:
			StartStore(v4);
			SetCursor(CURSOR_HAND);
		}
		else
		{
			StartStore(STORE_NOMONEY);
		}
	}
}
// 4B8C9C: using guessed type int cursH;
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void __cdecl SmithBuyPItem()
{
	int xx; // ecx
	int idx; // eax
	bool v2; // sf
	int v3; // eax
	int i; // edx

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	if ( !plr[myplr].HoldItem._iMagical )
		plr[myplr].HoldItem._iIdentified = 0;
	StoreAutoPlace();
	xx = 0;
	idx = (stextlhold - stextup) >> 2;
	v2 = stextvhold + idx < 0;
	v3 = stextvhold + idx;
	i = 0;
	if ( !v2 )
	{
		do
		{
			if ( premiumitem[i]._itype != -1 )
			{
				--v3;
				xx = i;
			}
			++i;
		}
		while ( v3 >= 0 );
	}

	premiumitem[xx]._itype = -1;
	--numpremium;
	SpawnPremium(plr[myplr]._pLevel);
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void __cdecl S_SPBuyEnter()
{
	int v0; // eax
	bool v1; // sf
	int v2; // eax
	int v3; // ecx
	int v4; // edx
	int *v5; // esi
	int v6; // ecx
	int v7; // eax
	int v8; // eax
	int v9; // esi
	char v10; // cl

	if ( stextsel == 22 )
	{
		StartStore(STORE_SMITH);
		stextsel = 14;
	}
	else
	{
		stextlhold = stextsel;
		stextshold = 18;
		stextvhold = stextsval;
		v0 = (stextsel - stextup) >> 2;
		v1 = stextsval + v0 < 0;
		v2 = stextsval + v0;
		v3 = 0;
		v4 = 0;
		if ( !v1 )
		{
			v5 = &premiumitem[0]._itype;
			do
			{
				if ( *v5 != -1 )
				{
					--v2;
					v3 = v4;
				}
				++v4;
				v5 += 92;
			}
			while ( v2 >= 0 );
		}
		v6 = v3;
		v7 = myplr;
		if ( plr[myplr]._pGold >= premiumitem[v6]._iIvalue )
		{
			qmemcpy(&plr[v7].HoldItem, &premiumitem[v6], sizeof(plr[v7].HoldItem));
			SetCursor(plr[v7].HoldItem._iCurs + 12);
			v8 = 0;
			v9 = 0;
			do
			{
				if ( v8 )
					goto LABEL_14;
				v8 = AutoPlace(myplr, v9++, cursW / 28, cursH / 28, 0);
			}
			while ( v9 < 40 );
			if ( v8 )
			{
LABEL_14:
				v10 = STORE_CONFIRM;
				goto LABEL_16;
			}
			v10 = STORE_NOROOM;
LABEL_16:
			StartStore(v10);
			SetCursor(CURSOR_HAND);
		}
		else
		{
			StartStore(STORE_NOMONEY);
		}
	}
}
// 4B8C9C: using guessed type int cursH;
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

bool __fastcall StoreGoldFit(int idx)
{
	int cost; // edi
	int i; // ecx
	int sz; // eax
	int numsqrs; // [esp+Ch] [ebp-4h]

	cost = storehold[idx]._iIvalue;
	sz = cost / 5000;
	if ( cost % 5000 )
		sz++;

	SetCursor(storehold[idx]._iCurs + 12);
	numsqrs = cursW / 28 * (cursH / 28);
	SetCursor(CURSOR_HAND);

	if ( numsqrs >= sz )
		return 1;

	for(i = 0; i < 40; i++)
	{
		if ( !plr[myplr].InvGrid[i] )
			numsqrs++;
	}

	for(i = 0; i < plr[myplr]._pNumInv; i++)
	{
		if ( plr[myplr].InvList[i]._itype == ITYPE_GOLD && plr[myplr].InvList[i]._ivalue != 5000 )
		{
			cost += plr[myplr].InvList[i]._ivalue;
			if ( cost > 5000 )
				cost -= 5000;
			else
				cost = 0;
		}
	}

	sz = cost / 5000;
	if ( cost % 5000 )
		sz++;
	return numsqrs >= sz;
}
// 4B8C9C: using guessed type int cursH;

void __fastcall PlaceStoreGold(int v)
{
	bool done; // ecx
	int ii; // ebp
	int xx; // esi
	int yy; // ST20_4
	int i; // [esp+10h] [ebp-10h]

	done = 0;

	for(i = 0; i < 40; i++)
	{
		if ( done )
			break;
		ii = 10 * (i / 10);
		if ( !plr[myplr].InvGrid[i % 10 + ii] )
		{
			xx = plr[myplr]._pNumInv;
			yy = plr[myplr]._pNumInv;
			GetGoldSeed(myplr, &golditem);
			qmemcpy(&plr[myplr].InvList[xx], &golditem, sizeof(ItemStruct));
			++plr[myplr]._pNumInv;
			plr[myplr].InvGrid[i % 10 + ii] = plr[myplr]._pNumInv;
			plr[myplr].InvList[xx]._ivalue = v;
			SetGoldCurs(myplr, yy);
			done = 1;
		}
	}
}

void __cdecl StoreSellItem()
{
	int idx; // ebx
	char v1; // al
	int v2; // eax
	int cost; // ebp
	bool v4; // sf
	unsigned char v5; // of
	unsigned int v6; // eax
	int v8; // edx
	int *v10; // edi
	int v11; // eax
	unsigned int v12; // esi
	int v13; // [esp+10h] [ebp-4h]

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	v1 = storehidx[idx];
	if ( v1 < 0 )
		RemoveSpdBarItem(myplr, -1 - v1);
	else
		RemoveInvItem(myplr, v1);
	v2 = storenumh - 1;
	cost = storehold[idx]._iIvalue;
	v5 = __OFSUB__(idx, storenumh - 1);
	v4 = idx - (storenumh-- - 1) < 0;
	if ( v4 ^ v5 )
	{
		v6 = v2 - idx;
		qmemcpy(&storehidx[idx], &storehidx[idx + 1], v6);
		qmemcpy(&storehold[idx], &storehold[idx + 1], 4 * (368 * v6 >> 2));
	}
	v8 = 0;
	v13 = 0;
	plr[myplr]._pGold += cost;
	if ( plr[myplr]._pNumInv <= 0 )
	{
LABEL_15:
		if ( cost > 0 )
		{
			if ( cost > 5000 )
			{
				v12 = (cost - 5001) / 5000 + 1;
				cost += -5000 * v12;
				do
				{
					PlaceStoreGold(5000);
					--v12;
				}
				while ( v12 );
			}
			PlaceStoreGold(cost);
		}
	}
	else
	{
		v10 = &plr[myplr].InvList[0]._ivalue;
		while ( cost > 0 )
		{
			if ( *(v10 - 47) == ITYPE_GOLD && *v10 != 5000 )
			{
				v11 = cost + *v10;
				if ( v11 > 5000 )
				{
					*v10 = 5000;
					cost = v11 - 5000;
					SetGoldCurs(myplr, v8);
				}
				else
				{
					*v10 = v11;
					SetGoldCurs(myplr, v8);
					cost = 0;
				}
			}
			v10 += 92;
			v8 = v13++ + 1;
			if ( v13 >= plr[myplr]._pNumInv )
				goto LABEL_15;
		}
	}
}
// 69F108: using guessed type int stextup;
// 69F10C: using guessed type int storenumh;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void __cdecl S_SSellEnter()
{
	int idx; // eax

	if ( stextsel == 22 )
	{
		StartStore(STORE_SMITH);
		stextsel = 16;
	}
	else
	{
		stextlhold = stextsel;
		idx = stextsval + ((stextsel - stextup) >> 2);
		stextshold = 3;
		stextvhold = stextsval;

		qmemcpy(&plr[myplr].HoldItem, &storehold[idx], sizeof(plr[myplr].HoldItem));

		if ( !StoreGoldFit(idx) )
			StartStore(STORE_NOROOM);
		else
			StartStore(STORE_CONFIRM);
	}
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void __cdecl SmithRepairItem()
{
	int i; // edx
	int idx; // eax

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	i = storehidx[idx];
	storehold[idx]._iDurability = storehold[idx]._iMaxDur;

	if ( i >= 0 )
	{
		plr[myplr].InvList[i]._iDurability = plr[myplr].InvList[i]._iMaxDur;
	}
	else
	{
		if ( i == -1 )
			plr[myplr].InvBody[0]._iDurability = plr[myplr].InvBody[0]._iMaxDur;
		if ( i == -2 )
			plr[myplr].InvBody[6]._iDurability = plr[myplr].InvBody[6]._iMaxDur;
		if ( i == -3 )
			plr[myplr].InvBody[4]._iDurability = plr[myplr].InvBody[4]._iMaxDur;
		if ( i == -4 )
			plr[myplr].InvBody[5]._iDurability = plr[myplr].InvBody[5]._iMaxDur;
	}
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void __cdecl S_SRepairEnter()
{
	int idx; // eax
	int v1; // edx
	int v2; // ecx
	bool v3; // sf
	unsigned char v4; // of
	char v5; // cl

	if ( stextsel == 22 )
	{
		StartStore(STORE_SMITH);
		stextsel = 18;
	}
	else
	{
		stextlhold = stextsel;
		stextshold = 4;
		idx = stextsval + ((stextsel - stextup) >> 2);
		v1 = myplr;
		stextvhold = stextsval;
		qmemcpy(&plr[myplr].HoldItem, &storehold[idx], sizeof(plr[myplr].HoldItem));
		v2 = plr[v1]._pGold;
		v4 = __OFSUB__(v2, storehold[idx]._iIvalue);
		v3 = v2 - storehold[idx]._iIvalue < 0;
		v5 = STORE_NOMONEY;
		if ( !(v3 ^ v4) )
			v5 = STORE_CONFIRM;
		StartStore(v5);
	}
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void __cdecl S_WitchEnter()
{
	int v0; // ecx

	v0 = 12;
	if ( stextsel == 12 )
	{
		stextlhold = 12;
		talker = 6;
		stextshold = 5;
		gossipstart = QUEST_ADRIA2;
		gossipend = QUEST_ADRIA13;
		_LOBYTE(v0) = STORE_GOSSIP;
		goto LABEL_12;
	}
	v0 = 2;
	switch ( stextsel )
	{
		case 14:
			_LOBYTE(v0) = STORE_WBUY;
			goto LABEL_12;
		case 16:
			_LOBYTE(v0) = STORE_WSELL;
			goto LABEL_12;
		case 18:
			_LOBYTE(v0) = STORE_WRECHARGE;
LABEL_12:
			StartStore(v0);
			return;
		case 20:
			stextflag = 0;
			break;
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void __cdecl WitchBuyItem()
{
	int idx; // ebx
	ItemStruct *v3; // eax
	ItemStruct *v4; // edi

	idx = stextvhold + ((stextlhold - stextup) >> 2);

	if ( idx < 3 )
		plr[myplr].HoldItem._iSeed = GetRndSeed();

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	StoreAutoPlace();

	if ( idx >= 3 )
	{
		if ( idx == 19 )
		{
			witchitem[19]._itype = -1;
		}
		else
		{
			if ( witchitem[idx + 1]._itype != -1 )
			{
				v3 = &witchitem[idx];
				do
				{
					v4 = v3;
					++v3;
					++idx;
					qmemcpy(v4, v3, sizeof(ItemStruct));
				}
				while ( v3[1]._itype != -1 );
			}
			witchitem[idx]._itype = -1;
		}
	}
	CalcPlrInv(myplr, 1u);
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void __cdecl S_WBuyEnter()
{
	int idx; // ecx
	int done; // eax
	int i; // esi

	if ( stextsel == 22 )
	{
		StartStore(STORE_WITCH);
		stextsel = 14;
	}
	else
	{
		stextlhold = stextsel;
		stextvhold = stextsval;
		stextshold = 6;
		idx = stextsval + ((stextsel - stextup) >> 2);

		if ( plr[myplr]._pGold >= witchitem[idx]._iIvalue )
		{
			qmemcpy(&plr[myplr].HoldItem, &witchitem[idx], sizeof(ItemStruct));
			SetCursor(plr[myplr].HoldItem._iCurs + 12);
			done = 0;

			for(i = 0; i < 40; i++)
			{
				if ( done )
					break;
				done = SpecialAutoPlace(myplr, i, cursW / 28, cursH / 28, 0);
			}

			if ( done )
				StartStore(STORE_CONFIRM);
			else
				StartStore(STORE_NOROOM);

			SetCursor(CURSOR_HAND);
		}
		else
		{
			StartStore(STORE_NOMONEY);
		}
	}
}
// 4B8C9C: using guessed type int cursH;
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void __cdecl S_WSellEnter()
{
	int idx; // eax
	char v2; // cl

	if ( stextsel == 22 )
	{
		StartStore(STORE_WITCH);
		stextsel = 16;
	}
	else
	{
		stextlhold = stextsel;
		idx = stextsval + ((stextsel - stextup) >> 2);
		stextshold = 7;
		stextvhold = stextsval;
		qmemcpy(&plr[myplr].HoldItem, &storehold[idx], sizeof(plr[myplr].HoldItem));
		v2 = STORE_CONFIRM;
		if ( !StoreGoldFit(idx) )
			v2 = STORE_NOROOM;
		StartStore(v2);
	}
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void __cdecl WitchRechargeItem()
{
	int i; // ecx
	int idx; // eax

	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	i = storehidx[idx];
	storehold[idx]._iCharges = storehold[idx]._iMaxCharges;

	if ( i >= 0 )
		plr[myplr].InvList[i]._iCharges = plr[myplr].InvList[i]._iMaxCharges;
	else
		plr[myplr].InvBody[4]._iCharges = plr[myplr].InvBody[4]._iMaxCharges;

	CalcPlrInv(myplr, 1u);
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void __cdecl S_WRechargeEnter()
{
	int idx; // eax
	int v1; // edx
	int v2; // ecx
	bool v3; // sf
	unsigned char v4; // of
	char v5; // cl

	if ( stextsel == 22 )
	{
		StartStore(STORE_WITCH);
		stextsel = 18;
	}
	else
	{
		stextlhold = stextsel;
		stextshold = 8;
		idx = stextsval + ((stextsel - stextup) >> 2);
		v1 = myplr;
		stextvhold = stextsval;
		qmemcpy(&plr[myplr].HoldItem, &storehold[idx], sizeof(plr[myplr].HoldItem));
		v2 = plr[v1]._pGold;
		v4 = __OFSUB__(v2, storehold[idx]._iIvalue);
		v3 = v2 - storehold[idx]._iIvalue < 0;
		v5 = STORE_NOMONEY;
		if ( !(v3 ^ v4) )
			v5 = STORE_CONFIRM;
		StartStore(v5);
	}
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void __cdecl S_BoyEnter()
{
	signed int v0; // ecx

	v0 = boyitem._itype;
	if ( boyitem._itype != -1 && stextsel == 18 )
	{
		v0 = 50;
		if ( plr[myplr]._pGold >= 50 )
		{
			TakePlrsMoney(50);
			_LOBYTE(v0) = STORE_BBOY;
		}
		else
		{
			stextshold = 12;
			stextlhold = 18;
			stextvhold = stextsval;
			_LOBYTE(v0) = STORE_NOMONEY;
		}
		goto LABEL_5;
	}
	if ( stextsel == 8 && boyitem._itype != -1 || stextsel == 12 && boyitem._itype == -1 )
	{
		talker = 8;
		stextshold = 12;
		stextlhold = stextsel;
		gossipstart = QUEST_WIRT2;
		gossipend = QUEST_WIRT12;
		_LOBYTE(v0) = STORE_GOSSIP;
LABEL_5:
		StartStore(v0);
		return;
	}
	stextflag = 0;
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void __cdecl BoyBuyItem()
{
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	StoreAutoPlace();
	boyitem._itype = -1;
	stextshold = 12;
	CalcPlrInv(myplr, 1u);
}

void __cdecl HealerBuyItem()
{
	int idx; // esi
	bool v1; // sf
	unsigned char v2; // of
	int v3; // eax
	int v4; // ecx
	bool v5; // sf
	unsigned char v6; // of
	int v7; // eax
	ItemStruct *v8; // edx
	ItemStruct *v9; // edi
	bool v10; // zf

	idx = stextvhold + ((stextlhold - stextup) >> 2);
	if ( gbMaxPlayers == 1 )
	{
		v2 = __OFSUB__(idx, 2);
		v1 = idx - 2 < 0;
	}
	else
	{
		v2 = __OFSUB__(idx, 3);
		v1 = idx - 3 < 0;
	}
	if ( v1 ^ v2 )
	{
		v3 = GetRndSeed();
		v4 = myplr;
		plr[myplr].HoldItem._iSeed = v3;
	}
	else
	{
		v4 = myplr;
	}
	TakePlrsMoney(plr[v4].HoldItem._iIvalue);
	if ( !plr[myplr].HoldItem._iMagical )
		plr[myplr].HoldItem._iIdentified = 0;
	StoreAutoPlace();
	if ( gbMaxPlayers == 1 )
	{
		v6 = __OFSUB__(idx, 2);
		v5 = idx - 2 < 0;
	}
	else
	{
		v6 = __OFSUB__(idx, 3);
		v5 = idx - 3 < 0;
	}
	if ( !(v5 ^ v6) )
	{
		v7 = stextvhold + ((stextlhold - stextup) >> 2);
		if ( v7 == 19 )
		{
			healitem[19]._itype = -1;
		}
		else
		{
			if ( healitem[v7 + 1]._itype != -1 )
			{
				v8 = &healitem[v7];
				do
				{
					v9 = v8;
					++v8;
					++v7;
					v10 = v8[1]._itype == -1;
					qmemcpy(v9, v8, sizeof(ItemStruct));
				}
				while ( !v10 );
			}
			healitem[v7]._itype = -1;
		}
		CalcPlrInv(myplr, 1u);
	}
}
// 679660: using guessed type char gbMaxPlayers;
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void __cdecl S_BBuyEnter()
{
	int v0; // ecx
	int v1; // eax
	int v2; // ecx
	int v3; // eax
	int v4; // esi

	if ( stextsel == 10 )
	{
		v0 = boyitem._iIvalue;
		stextvhold = stextsval;
		v1 = myplr;
		stextshold = 13;
		stextlhold = 10;
		if ( plr[myplr]._pGold >= boyitem._iIvalue + (boyitem._iIvalue >> 1) )
		{
			qmemcpy(&plr[v1].HoldItem, &boyitem, sizeof(plr[v1].HoldItem));
			plr[v1].HoldItem._iIvalue += plr[v1].HoldItem._iIvalue >> 1;
			SetCursor(plr[v1].HoldItem._iCurs + 12);
			v3 = 0;
			v4 = 0;
			do
			{
				if ( v3 )
					goto LABEL_8;
				v3 = AutoPlace(myplr, v4++, cursW / 28, cursH / 28, 0);
			}
			while ( v4 < 40 );
			if ( v3 )
			{
LABEL_8:
				_LOBYTE(v2) = STORE_CONFIRM;
				goto LABEL_10;
			}
			_LOBYTE(v2) = STORE_NOROOM;
LABEL_10:
			StartStore(v2);
			SetCursor(CURSOR_HAND);
		}
		else
		{
			_LOBYTE(v0) = STORE_NOMONEY;
			StartStore(v0);
		}
	}
	else
	{
		stextflag = 0;
	}
}
// 4B8C9C: using guessed type int cursH;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;
// 6AA705: using guessed type char stextflag;

void __cdecl StoryIdItem()
{
	int v0; // ecx
	int v1; // eax
	int v2; // eax

	v0 = storehidx[((stextlhold - stextup) >> 2) + stextvhold];
	v1 = myplr;
	if ( v0 >= 0 )
	{
		plr[myplr].InvList[v0]._iIdentified = 1;
	}
	else
	{
		if ( v0 == -1 )
			plr[myplr].InvBody[0]._iIdentified = 1;
		if ( v0 == -2 )
			plr[v1].InvBody[6]._iIdentified = 1;
		if ( v0 == -3 )
			plr[v1].InvBody[4]._iIdentified = 1;
		if ( v0 == -4 )
			plr[v1].InvBody[5]._iIdentified = 1;
		if ( v0 == -5 )
			plr[v1].InvBody[1]._iIdentified = 1;
		if ( v0 == -6 )
			plr[v1].InvBody[2]._iIdentified = 1;
		if ( v0 == -7 )
			plr[v1].InvBody[3]._iIdentified = 1;
	}
	v2 = v1;
	plr[v2].HoldItem._iIdentified = 1;
	TakePlrsMoney(plr[v2].HoldItem._iIvalue);
	CalcPlrInv(myplr, 1u);
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;

void __cdecl S_ConfirmEnter()
{
	char v0; // cl

	if ( stextsel == 18 )
	{
		if ( stextshold > STORE_WRECHARGE )
		{
			switch ( stextshold )
			{
				case STORE_BBOY:
					BoyBuyItem();
					break;
				case STORE_HBUY:
					HealerBuyItem();
					break;
				case STORE_SIDENTIFY:
					StoryIdItem();
					v0 = STORE_IDSHOW;
LABEL_20:
					StartStore(v0);
					return;
				case STORE_SPBUY:
					SmithBuyPItem();
					break;
			}
		}
		else
		{
			switch ( stextshold )
			{
				case STORE_WRECHARGE:
					WitchRechargeItem();
					break;
				case STORE_SBUY:
					SmithBuyItem();
					break;
				case STORE_SSELL:
					goto LABEL_27;
				case STORE_SREPAIR:
					SmithRepairItem();
					break;
				case STORE_WBUY:
					WitchBuyItem();
					break;
				case STORE_WSELL:
LABEL_27:
					StoreSellItem();
					break;
			}
		}
		v0 = stextshold;
		goto LABEL_20;
	}
	StartStore((unsigned char)stextshold);
	stextsel = stextlhold;
	stextsval = stextvhold;
}
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void __cdecl S_HealerEnter()
{
	int v0; // ecx
	int v1; // eax

	v0 = 12;
	if ( stextsel == 12 )
	{
		stextlhold = 12;
		talker = 1;
		stextshold = 14;
		gossipstart = QUEST_PEPIN2;
		gossipend = QUEST_PEPIN11;
		_LOBYTE(v0) = STORE_GOSSIP;
		goto LABEL_12;
	}
	if ( stextsel != 14 )
	{
		if ( stextsel != 16 )
		{
			if ( stextsel == 18 )
				stextflag = 0;
			return;
		}
		_LOBYTE(v0) = STORE_HBUY;
LABEL_12:
		StartStore(v0);
		return;
	}
	if ( plr[myplr]._pHitPoints != plr[myplr]._pMaxHP )
		PlaySFX(IS_CAST8);
	drawhpflag = TRUE;
	v1 = myplr;
	plr[v1]._pHitPoints = plr[myplr]._pMaxHP;
	plr[v1]._pHPBase = plr[v1]._pMaxHPBase;
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void __cdecl S_HBuyEnter()
{
	int v0; // eax
	int idx; // ecx
	int done; // eax
	int i; // esi
	char v4; // cl

	if ( stextsel == 22 )
	{
		StartStore(STORE_HEALER);
		stextsel = 16;
	}
	else
	{
		stextlhold = stextsel;
		stextvhold = stextsval;
		stextshold = 16;
		v0 = myplr;
		idx = stextsval + ((stextsel - stextup) >> 2);
		if ( plr[myplr]._pGold >= healitem[idx]._iIvalue )
		{
			qmemcpy(&plr[v0].HoldItem, &healitem[idx], sizeof(plr[v0].HoldItem));
			SetCursor(plr[v0].HoldItem._iCurs + 12);
			done = 0;
			i = 0;
			do
			{
				if ( done )
					goto LABEL_9;
				done = SpecialAutoPlace(myplr, i++, cursW / 28, cursH / 28, 0);
			}
			while ( i < 40 );
			if ( done )
			{
LABEL_9:
				v4 = STORE_CONFIRM;
				goto LABEL_11;
			}
			v4 = STORE_NOROOM;
LABEL_11:
			StartStore(v4);
			SetCursor(CURSOR_HAND);
		}
		else
		{
			StartStore(STORE_NOMONEY);
		}
	}
}
// 4B8C9C: using guessed type int cursH;
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void __cdecl S_StoryEnter()
{
	int v0; // ecx

	v0 = 12;
	switch ( stextsel )
	{
		case 12:
			stextlhold = 12;
			talker = 4;
			stextshold = 15;
			gossipstart = QUEST_STORY2;
			gossipend = QUEST_STORY11;
			_LOBYTE(v0) = STORE_GOSSIP;
			goto LABEL_8;
		case 14:
			_LOBYTE(v0) = STORE_SIDENTIFY;
LABEL_8:
			StartStore(v0);
			return;
		case 18:
			stextflag = 0;
			break;
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void __cdecl S_SIDEnter()
{
	int idx; // eax
	int v1; // edx
	int v2; // ecx
	bool v3; // sf
	unsigned char v4; // of
	char v5; // cl

	if ( stextsel == 22 )
	{
		StartStore(STORE_STORY);
		stextsel = 14;
	}
	else
	{
		stextlhold = stextsel;
		stextshold = 17;
		idx = stextsval + ((stextsel - stextup) >> 2);
		v1 = myplr;
		stextvhold = stextsval;
		qmemcpy(&plr[myplr].HoldItem, &storehold[idx], sizeof(plr[myplr].HoldItem));
		v2 = plr[v1]._pGold;
		v4 = __OFSUB__(v2, storehold[idx]._iIvalue);
		v3 = v2 - storehold[idx]._iIvalue < 0;
		v5 = STORE_NOMONEY;
		if ( !(v3 ^ v4) )
			v5 = STORE_CONFIRM;
		StartStore(v5);
	}
}
// 69F108: using guessed type int stextup;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;

void __cdecl S_TalkEnter()
{
	int v0; // edx
	int *v1; // edi
	signed int v2; // eax
	int v3; // esi
	int *v4; // ecx
	int v5; // esi
	signed int v6; // ebp
	int v8; // eax
	int v9; // ebx
	int v10; // ecx

	if ( stextsel == 22 )
	{
		StartStore((unsigned char)stextshold);
		stextsel = stextlhold;
	}
	else
	{
		v0 = talker;
		v1 = &quests[0]._qlog;
		v2 = 0;
		v3 = 0;
		v4 = &quests[0]._qlog;
		do
		{
			if ( *((_BYTE *)v4 - 18) == 2 && *((_DWORD *)&Qtalklist[0]._qinfra + v3 + 16 * talker) != -1 && *v4 )
				++v2;
			v4 += 6;
			++v3;
		}
		while ( (signed int)v4 < (signed int)&quests[16]._qlog );
		if ( v2 <= 6 )
		{
			v5 = 15 - v2;
			v6 = 2;
		}
		else
		{
			v5 = 14 - (v2 >> 1);
			v6 = 1;
		}
		if ( stextsel == v5 - 2 )
		{
			SetRndSeed(towner[talker]._tSeed);
			v8 = random(0, gossipend - gossipstart + 1);
			InitQTextMsg(gossipstart + v8);
		}
		else
		{
			v9 = 0;
			do
			{
				if ( *((_BYTE *)v1 - 18) == 2 )
				{
					v10 = *((_DWORD *)&Qtalklist[0]._qinfra + v9 + 16 * v0);
					if ( v10 != -1 )
					{
						if ( *v1 )
						{
							if ( v5 == stextsel )
							{
								InitQTextMsg(v10);
								v0 = talker;
							}
							v5 += v6;
						}
					}
				}
				v1 += 6;
				++v9;
			}
			while ( (signed int)v1 < (signed int)&quests[16]._qlog );
		}
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;

void __cdecl S_TavernEnter()
{
	int v0; // ecx

	v0 = 12;
	if ( stextsel == 12 )
	{
		stextlhold = 12;
		talker = 3;
		stextshold = 21;
		gossipstart = QUEST_OGDEN2;
		gossipend = QUEST_OGDEN10;
		_LOBYTE(v0) = STORE_GOSSIP;
		StartStore(v0);
	}
	else if ( stextsel == 18 )
	{
		stextflag = 0;
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void __cdecl S_BarmaidEnter()
{
	int v0; // ecx

	v0 = 12;
	if ( stextsel == 12 )
	{
		stextlhold = 12;
		talker = 7;
		stextshold = 23;
		gossipstart = QUEST_GILLIAN2;
		gossipend = QUEST_GILLIAN10;
		_LOBYTE(v0) = STORE_GOSSIP;
		StartStore(v0);
	}
	else if ( stextsel == 18 )
	{
		stextflag = 0;
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void __cdecl S_DrunkEnter()
{
	int v0; // ecx

	v0 = 12;
	if ( stextsel == 12 )
	{
		stextlhold = 12;
		talker = 5;
		stextshold = 22;
		gossipstart = QUEST_FARNHAM2;
		gossipend = QUEST_FARNHAM13;
		_LOBYTE(v0) = STORE_GOSSIP;
		StartStore(v0);
	}
	else if ( stextsel == 18 )
	{
		stextflag = 0;
	}
}
// 69F110: using guessed type int stextlhold;
// 69FB38: using guessed type int talker;
// 6A4EF0: using guessed type int gossipstart;
// 6A8A28: using guessed type int stextsel;
// 6A8A30: using guessed type int gossipend;
// 6AA705: using guessed type char stextflag;

void __cdecl STextEnter()
{
	if ( qtextflag )
	{
		qtextflag = 0;
		if ( leveltype == DTYPE_TOWN )
			sfx_stop();
	}
	else
	{
		PlaySFX(IS_TITLSLCT);
		switch ( stextflag )
		{
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
			default:
				return;
		}
	}
}
// 5BB1ED: using guessed type char leveltype;
// 646D00: using guessed type char qtextflag;
// 69F110: using guessed type int stextlhold;
// 6A8A24: using guessed type int stextvhold;
// 6A8A28: using guessed type int stextsel;
// 6AA705: using guessed type char stextflag;

void __cdecl CheckStoreBtn()
{
	bool v0; // sf
	unsigned char v1; // of
	int v2; // eax
	int *v3; // ecx

	if ( qtextflag )
	{
		qtextflag = 0;
		if ( leveltype == DTYPE_TOWN )
			sfx_stop();
	}
	else if ( stextsel != -1 && MouseY >= 32 && MouseY <= 320 )
	{
		if ( stextsize )
		{
			v1 = __OFSUB__(MouseX, 24);
			v0 = MouseX - 24 < 0;
		}
		else
		{
			v1 = __OFSUB__(MouseX, 344);
			v0 = MouseX - 344 < 0;
		}
		if ( !(v0 ^ v1) && MouseX <= 616 )
		{
			v2 = (MouseY - 32) / 12;
			if ( stextscrl && MouseX > 600 )
			{
				if ( v2 == 4 )
				{
					if ( stextscrlubtn <= 0 )
					{
						STextUp();
						stextscrlubtn = 10;
						return;
					}
					--stextscrlubtn;
				}
				if ( v2 == 20 )
				{
					if ( stextscrldbtn > 0 )
					{
						--stextscrldbtn;
					}
					else
					{
						STextDown();
						stextscrldbtn = 10;
					}
				}
			}
			else if ( v2 >= 5 )
			{
				if ( v2 >= 23 )
					v2 = 22;
				if ( stextscrl )
				{
					if ( v2 < 21 )
					{
						v3 = &stext[v2]._ssel;
						if ( !*v3 )
						{
							if ( stext[v2 - 2]._ssel )
							{
								v2 -= 2;
							}
							else if ( *(v3 - 39) )
							{
								--v2;
							}
						}
					}
				}
				if ( stext[v2]._ssel || stextscrl && v2 == 22 )
				{
					stextsel = v2;
					STextEnter();
				}
			}
		}
	}
}
// 5BB1ED: using guessed type char leveltype;
// 646D00: using guessed type char qtextflag;
// 6A09E0: using guessed type char stextsize;
// 6A6BB8: using guessed type int stextscrl;
// 6A8A28: using guessed type int stextsel;
// 6A8A2C: using guessed type char stextscrldbtn;
// 6AA704: using guessed type char stextscrlubtn;

void __cdecl ReleaseStoreBtn()
{
	stextscrlubtn = -1;
	stextscrldbtn = -1;
}
// 6A8A2C: using guessed type char stextscrldbtn;
// 6AA704: using guessed type char stextscrlubtn;
