//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int numthemes; // idb
bool armorFlag; // weak
int ThemeGoodIn[4];
bool weaponFlag; // weak
bool treasureFlag; // weak
bool mFountainFlag; // weak
bool cauldronFlag; // weak
bool tFountainFlag; // weak
int zharlib; // weak
int themex; // idb
int themey; // idb
int themeVar1; // idb
ThemeStruct themes[MAXTHEMES];
bool pFountainFlag; // weak
bool bFountainFlag; // weak
bool bCrossFlag; // weak
#endif

int ThemeGood[4] = { THEME_GOATSHRINE, THEME_SHRINE, THEME_SKELROOM, THEME_LIBRARY };

int trm5x[25] =
{
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2
};

int trm5y[25] =
{
	-2, -2, -2, -2, -2,
	-1, -1, -1, -1, -1,
	0, 0, 0, 0, 0,
	1, 1, 1, 1, 1,
	2, 2, 2, 2, 2
};

int trm3x[9] =
{
	-1, 0, 1,
	-1, 0, 1,
	-1, 0, 1
};

int trm3y[9] =
{
	-1, -1, -1,
	0, 0, 0,
	1, 1, 1
};

bool __fastcall TFit_Shrine(int i)
{
	int v1; // ecx
	int v2; // esi
	int v3; // eax
	int v4; // edx
	signed int v6; // [esp+Ch] [ebp-8h]
	int v7; // [esp+10h] [ebp-4h]

	v1 = themes[i].ttval;
	v7 = 0;
	v2 = 0;
	v6 = 0;
	while ( 1 )
	{
		v3 = v2 + 112 * v7;
		if ( dung_map[0][v3] != v1 ) /* check */
			goto LABEL_20;
		v4 = dPiece[0][v3 - 1]; // *(_DWORD *)&dflags[39][4 * v3 + 36];
		if ( nTrapTable[v4]
		  && !nSolidTable[dPiece[-1][v3]] // !nSolidTable[*(_DWORD *)&dflags[28][4 * v3 + 32]]
		  && !nSolidTable[dPiece[1][v3]]
		  && dung_map[-1][v3] == v1 // block_lvid[v3 + 1940] == v1
		  && dung_map[1][v3] == v1
		  && !dObject[-1][v3 - 1]
		  && !dObject[0][v3 + 111] )
		{
			v6 = 1;
		}
		if ( v6 )
			break;
		if ( !nTrapTable[dPiece[-1][v3]] // !nTrapTable[*(_DWORD *)&dflags[28][4 * v3 + 32]]
		  || nSolidTable[v4]
		  || nSolidTable[dPiece[0][v3 + 1]]
		  || dung_map[0][v3 - 1] != v1 // *(&byte_5B78EB + v3) != v1
		  || dung_map[0][v3 + 1] != v1
		  || dObject[-1][v3 - 1]
		  || dObject[-1][v3 + 1] ) /* check */
		{
			goto LABEL_21;
		}
		v6 = 2;
LABEL_20:
		if ( v6 )
			break;
LABEL_21:
		if ( ++v7 == 112 )
		{
			++v2;
			v7 = 0;
			if ( v2 == 112 )
				return 0;
		}
	}
	themey = v2;
	themex = v7;
	themeVar1 = v6;
	return 1;
}

bool __fastcall TFit_Obj5(int t)
{
	int v2; // ebx
	int v3; // esi
	int v4; // eax
	int v5; // edi
	int v6; // ecx
	signed int v7; // edx
	int v8; // ecx
	int v10; // [esp+Ch] [ebp-Ch]
	int v11; // [esp+10h] [ebp-8h]
	signed int v12; // [esp+14h] [ebp-4h]

	v2 = 0;
	v3 = 0;
	v4 = random(0, 5) + 1;
	v10 = v4;
	if ( v4 <= 0 )
	{
LABEL_19:
		themex = v2;
		themey = v3;
		return 1;
	}
	v5 = themes[t].ttval;
	v11 = v5;
	while ( 1 )
	{
		v6 = v3 + 112 * v2;
		if ( dung_map[0][v6] == v5 && !nSolidTable[dPiece[0][v6]] )
		{
			v12 = 1;
			v7 = 0;
			do
			{
				if ( v7 >= 25 )
					break;
				v8 = v3 + trm5y[v7] + 112 * (v2 + trm5x[v7]);
				if ( nSolidTable[dPiece[0][v8]] )
					v12 = 0;
				v5 = v11;
				if ( dung_map[0][v8] != v11 )
					v12 = 0;
				++v7;
			}
			while ( v12 );
			if ( v12 )
			{
				--v4;
				goto LABEL_18;
			}
		}
		if ( ++v2 != 112 )
			goto LABEL_18;
		v2 = 0;
		if ( ++v3 != 112 )
			goto LABEL_18;
		if ( v4 == v10 )
			return 0;
		v3 = 0;
LABEL_18:
		if ( v4 <= 0 )
			goto LABEL_19;
	}
}

bool __fastcall TFit_SkelRoom(int t)
{
	int i; // esi

	if ( leveltype != 1 && leveltype != 2 )
		return 0;
	i = 0;
	if ( nummtypes <= 0 )
		return 0;

	while ( !IsSkel(Monsters[i].mtype) )
	{
		++i;
		if ( i >= nummtypes )
			return 0;
	}
	themeVar1 = i;
	return TFit_Obj5(t);
}
// 5BB1ED: using guessed type char leveltype;

bool __fastcall TFit_GoatShrine(int t)
{
	int i; // esi

	i = 0;
	if ( nummtypes <= 0 )
		return 0;

	while ( !IsGoat(Monsters[i].mtype) )
	{
		++i;
		if ( i >= nummtypes )
			return 0;
	}
	themeVar1 = i;
	return TFit_Obj5(t);
}

bool __fastcall CheckThemeObj3(int xp, int yp, int t, int f)
{
	int i; // edi

	i = 0;
	while ( 1 )
	{
		if ( xp + trm3x[i] < 0 )
			break;
		if ( yp + trm3y[i] < 0 )
			break;
		if ( nSolidTable[dPiece[xp + trm3x[i]][yp + trm3y[i]]] )
			break;
		if ( dung_map[xp + trm3x[i]][yp + trm3y[i]] != themes[t].ttval )
			break;
		if ( dObject[xp + trm3x[i]][yp + trm3y[i]] )
			break;
		if ( f != -1 )
		{
			if ( !random(0, f) )
				break;
		}
		++i;
		if ( i >= 9 )
			return 1;
	}
	return 0;
}

bool __fastcall TFit_Obj3(int t)
{
	int yp; // edi
	int xp; // esi
	char objrnd[4]; // [esp+Bh] [ebp-5h]

	objrnd[0] = 4;
	objrnd[1] = 4;
	objrnd[2] = 3;
	objrnd[3] = 5;
	yp = 1;
	while ( 2 )
	{
		xp = 1;
		do
		{
			if ( CheckThemeObj3(xp, yp, t, objrnd[leveltype-1]) )
			{
				themex = xp;
				themey = yp;
				return 1;
			}
			++xp;
		}
		while ( xp < 111 );
		if ( ++yp < 111 )
			continue;
		break;
	}
	return 0;
}
// 5BB1ED: using guessed type char leveltype;

bool __fastcall CheckThemeReqs(int t)
{
	bool rv; // al
	int v2; // ecx
	int v3; // ecx
	int v4; // ecx
	int v5; // ecx
	bool v6; // zf
	int v7; // ecx
	int v8; // ecx
	int v9; // ecx

	rv = 1;
	if ( t <= 10 )
	{
		if ( t != 10 )
		{
			v2 = t - 1;
			if ( v2 )
			{
				v3 = v2 - 2;
				if ( v3 )
				{
					v4 = v3 - 2;
					if ( v4 )
					{
						v5 = v4 - 2;
						if ( v5 )
						{
							if ( v5 != 2 )
								return rv;
							v6 = pFountainFlag == 0;
						}
						else
						{
							v6 = bFountainFlag == 0;
						}
LABEL_21:
						if ( !v6 )
							return rv;
						return 0;
					}
				}
			}
			if ( leveltype != 3 )
			{
				v6 = leveltype == DTYPE_HELL;
				goto LABEL_21;
			}
			return 0;
		}
LABEL_16:
		v6 = leveltype == DTYPE_CATHEDRAL;
		goto LABEL_21;
	}
	v7 = t - 12;
	if ( v7 )
	{
		v8 = v7 - 1;
		if ( !v8 )
		{
			v6 = mFountainFlag == 0;
			goto LABEL_21;
		}
		v9 = v8 - 1;
		if ( !v9 )
		{
			v6 = tFountainFlag == 0;
			goto LABEL_21;
		}
		if ( v9 != 2 )
			return rv;
		goto LABEL_16;
	}
	if ( leveltype == DTYPE_HELL )
	{
		v6 = cauldronFlag == 0;
		goto LABEL_21;
	}
	return 0;
}
// 5BB1ED: using guessed type char leveltype;
// 6AAA58: using guessed type int mFountainFlag;
// 6AAA5C: using guessed type int cauldronFlag;
// 6AAA60: using guessed type int tFountainFlag;
// 6AAC08: using guessed type int pFountainFlag;
// 6AAC0C: using guessed type int bFountainFlag;

bool __fastcall SpecialThemeFit(int i, int t)
{
	bool rv; // eax

	rv = CheckThemeReqs(t);
	switch ( t )
	{
		case THEME_SHRINE:
		case THEME_LIBRARY:
			if ( rv )
				rv = TFit_Shrine(i);
			break;
		case THEME_SKELROOM:
			if ( rv )
				rv = TFit_SkelRoom(i);
			break;
		case THEME_TREASURE:
			rv = treasureFlag;
			if ( treasureFlag )
				treasureFlag = 0;
			break;
		case THEME_TORTURE:
		case THEME_DECAPITATED:
		case THEME_ARMORSTAND:
		case THEME_BRNCROSS:
		case THEME_WEAPONRACK:
			if ( rv )
				rv = TFit_Obj3(i);
			break;
		case THEME_BLOODFOUNTAIN:
			if ( rv )
			{
				rv = TFit_Obj5(i);
				if ( rv )
					bFountainFlag = 0;
			}
			break;
		case THEME_PURIFYINGFOUNTAIN:
			if ( rv )
			{
				rv = TFit_Obj5(i);
				if ( rv )
					pFountainFlag = 0;
			}
			break;
		case THEME_GOATSHRINE:
			if ( rv )
				rv = TFit_GoatShrine(i);
			break;
		case THEME_CAULDRON:
			if ( rv )
			{
				rv = TFit_Obj5(i);
				if ( rv )
					cauldronFlag = 0;
			}
			break;
		case THEME_MURKYFOUNTAIN:
			if ( rv )
			{
				rv = TFit_Obj5(i);
				if ( rv )
					mFountainFlag = 0;
			}
			break;
		case THEME_TEARFOUNTAIN:
			if ( rv )
			{
				rv = TFit_Obj5(i);
				if ( rv )
					tFountainFlag = 0;
			}
			break;
		default:
			return rv;
	}
	return rv;
}
// 6AAA54: using guessed type int treasureFlag;
// 6AAA58: using guessed type int mFountainFlag;
// 6AAA5C: using guessed type int cauldronFlag;
// 6AAA60: using guessed type int tFountainFlag;
// 6AAC08: using guessed type int pFountainFlag;
// 6AAC0C: using guessed type int bFountainFlag;

bool __fastcall CheckThemeRoom(int tv)
{
	int v1; // esi
	int *v2; // edx
	signed int v3; // edi
	signed int v4; // esi
	signed int v5; // edx
	signed int v6; // eax
	int v7; // edi
	int *v8; // esi
	char *v9; // eax
	int *v10; // edx
	signed int v12; // [esp+Ch] [ebp-8h]

	v1 = 0;
	if ( trigflag[4] <= 0 )
	{
LABEL_5:
		v3 = 0;
		v4 = 0;
		do
		{
			v5 = 0;
			v6 = v4;
			do
			{
				if ( dung_map[0][v6] == tv )
				{
					if ( dFlags[0][v6] & DFLAG_POPULATED )
						return 0;
					++v3;
				}
				++v5;
				v6 += 112;
			}
			while ( v5 < 112 );
			++v4;
		}
		while ( v4 < 112 );
		if ( leveltype != 1 || v3 >= 9 && v3 <= 100 )
		{
			v7 = 0;
			v8 = &dPiece[-1][111];
LABEL_16:
			v12 = 0;
			v9 = &dung_map[-1][v7 + 111];
			v10 = v8;
			while ( v9[1] != tv
				 || nSolidTable[v10[1]]
				 || (*(v9 - 111) == tv || nSolidTable[*(v10 - 111)]) /* check */
				 && (v9[113] == tv || nSolidTable[v10[113]])
				 && (*v9 == tv || nSolidTable[*v10])
				 && (v9[2] == tv || nSolidTable[v10[2]]) )
			{
				++v12;
				v10 += 112;
				v9 += 112;
				if ( v12 >= 112 )
				{
					++v8;
					++v7;
					if ( (signed int)v8 < (signed int)&dPiece[0][111] )
						goto LABEL_16;
					return 1;
				}
			}
		}
	}
	else
	{
		v2 = &trigs[0]._ty;
		while ( dung_map[*(v2 - 1)][*v2] != tv )
		{
			++v1;
			v2 += 4;
			if ( v1 >= trigflag[4] )
				goto LABEL_5;
		}
	}
	return 0;
}
// 5BB1ED: using guessed type char leveltype;

void __cdecl InitThemes()
{
	int v0; // esi
	char v1; // bl
	int v2; // edi
	//int v3; // eax
	int i; // ebx
	//int v6; // eax
	int v8; // esi
	int v9; // ecx
	int j; // eax
	//int v11; // eax
	int *v13; // edi
	int v14; // esi
	int *v15; // ebx
	//int v16; // eax
	int v17; // eax
	int k; // esi
	int l; // ebx
	//int v20; // eax

	zharlib = -1;
	v0 = 0;
	bCrossFlag = 0;
	numthemes = 0;
	armorFlag = 1;
	bFountainFlag = 1;
	cauldronFlag = 1;
	mFountainFlag = 1;
	pFountainFlag = 1;
	tFountainFlag = 1;
	treasureFlag = 1;
	weaponFlag = 1;
	if ( currlevel != 16 )
	{
		v1 = leveltype;
		if ( leveltype == DTYPE_CATHEDRAL )
		{
			ThemeGoodIn[0] = 0;
			ThemeGoodIn[1] = 0;
			ThemeGoodIn[2] = 0;
			ThemeGoodIn[3] = 0;
			v2 = 0;
			do
			{
				if ( v0 >= MAXTHEMES )
					break;
				//_LOBYTE(v3) = CheckThemeRoom(v2);
				if ( CheckThemeRoom(v2) )
				{
					themes[v0].ttval = v2;
					for ( i = ThemeGood[random(0, 4)]; ; i = random(0, 17) )
					{
						//_LOBYTE(v6) = SpecialThemeFit(numthemes, i);
						if ( SpecialThemeFit(numthemes, i) )
							break;
					}
					v8 = numthemes;
					themes[numthemes].ttype = i;
					v1 = leveltype;
					v0 = v8 + 1;
					numthemes = v0;
				}
				++v2;
			}
			while ( v2 < 256 );
		}
		if ( v1 == 2 || v1 == 3 || v1 == 4 )
		{
			v9 = themeCount;
			for ( j = 0; j < v9; ++j )
				themes[j].ttype = -1;
			//_LOBYTE(v11) = QuestStatus(QTYPE_ZHAR);
			v13 = &themeLoc[0].ttval;
			if ( QuestStatus(QTYPE_ZHAR) )
			{
				v14 = 0;
				if ( themeCount > 0 )
				{
					v15 = &themeLoc[0].ttval;
					while ( 1 )
					{
						themes[v14].ttval = *v15;
						//_LOBYTE(v16) = SpecialThemeFit(v14, 5);
						if ( SpecialThemeFit(v14, 5) )
							break;
						++v14;
						v15 += 5;
						if ( v14 >= themeCount )
							goto LABEL_23;
					}
					themes[v14].ttype = 5;
					zharlib = v14;
				}
			}
LABEL_23:
			v17 = themeCount;
			for ( k = 0; k < themeCount; v13 += 5 )
			{
				if ( themes[k].ttype == -1 )
				{
					themes[k].ttval = *v13;
					for ( l = ThemeGood[random(0, 4)]; ; l = random(0, 17) )
					{
						//_LOBYTE(v20) = SpecialThemeFit(k, l);
						if ( SpecialThemeFit(k, l) )
							break;
					}
					themes[k].ttype = l;
				}
				v17 = themeCount;
				++k;
			}
			numthemes += v17;
		}
	}
}
// 5BB1ED: using guessed type char leveltype;
// 6AAA3C: using guessed type int armorFlag;
// 6AAA50: using guessed type int weaponFlag;
// 6AAA54: using guessed type int treasureFlag;
// 6AAA58: using guessed type int mFountainFlag;
// 6AAA5C: using guessed type int cauldronFlag;
// 6AAA60: using guessed type int tFountainFlag;
// 6AAA64: using guessed type int zharlib;
// 6AAC08: using guessed type int pFountainFlag;
// 6AAC0C: using guessed type int bFountainFlag;
// 6AAC10: using guessed type int bCrossFlag;

void __cdecl HoldThemeRooms()
{
	int v0; // ebx
	int i; // esi
	char v2; // dl
	signed int v3; // ecx
	signed int v4; // eax
	signed int v5; // edi

	if ( currlevel != 16 )
	{
		if ( leveltype == DTYPE_CATHEDRAL )
		{
			v0 = numthemes;
			for ( i = 0; i < v0; ++i )
			{
				v2 = themes[i].ttval;
				v3 = 0;
				do
				{
					v4 = v3;
					v5 = 112;
					do
					{
						if ( dung_map[0][v4] == v2 )
							dFlags[0][v4] |= DFLAG_POPULATED;
						v4 += 112;
						--v5;
					}
					while ( v5 );
					++v3;
				}
				while ( v3 < 112 );
			}
		}
		else
		{
			DRLG_HoldThemeRooms();
		}
	}
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall PlaceThemeMonsts(int t, int f)
{
	int numscattypes; // edx
	int i; // ecx
	int yp; // ebx
	int xp; // edi
	int scattertypes[111]; // [esp+Ch] [ebp-1D0h]
	int mtype; // [esp+1CCh] [ebp-10h]

	numscattypes = 0;

	if ( nummtypes > 0 )
	{
		for(i = 0; i < nummtypes; i++)
		{
			if ( Monsters[i].mPlaceFlags & 1 )
				scattertypes[numscattypes++] = i;
		}
	}

	mtype = scattertypes[random(0, numscattypes)];

	for(yp = 0; yp < 112; yp++)
	{
		for(xp = 0; xp < 112; xp++)
		{
			if ( dung_map[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]] && !dItem[xp][yp] && !dObject[xp][yp] )
			{
				if ( !random(0, f) )
					AddMonster(xp, yp, random(0, 8), mtype, 1);
			}
		}
	}
}
// 45D0E1: using guessed type int var_1D0[111];

void __fastcall Theme_Barrel(int t)
{
	int yp; // edi
	int xp; // ebx
	int r;
	char monstrnd[4];
	char barrnd[4];

	barrnd[0] = 2;
	barrnd[1] = 6;
	barrnd[2] = 4;
	barrnd[3] = 8;
	monstrnd[0] = 5;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;

	for(yp = 0; yp < 112; yp++)
	{
		for(xp = 0; xp < 112; xp++)
		{
			if ( dung_map[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]] )
			{
				if ( !random(0, barrnd[leveltype-1]) )
				{
					r = random(0, barrnd[leveltype-1]);
					AddObject((r != 0) + OBJ_BARREL, xp, yp);
				}
			}
		}
	}

	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_Shrine(int t)
{
	char monstrnd[4]; // [esp+3h] [ebp-5h]

	monstrnd[0] = 6;
	monstrnd[1] = 6;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	TFit_Shrine(t);
	if ( themeVar1 == 1 )
	{
		AddObject(OBJ_CANDLE2, themex - 1, themey);
		AddObject(OBJ_SHRINER, themex, themey);
		AddObject(OBJ_CANDLE2, themex + 1, themey);
	}
	else
	{
		AddObject(OBJ_CANDLE2, themex, themey - 1);
		AddObject(OBJ_SHRINEL, themex, themey);
		AddObject(OBJ_CANDLE2, themex, themey + 1);
	}
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_MonstPit(int t)
{
	int r; // eax
	int ixp; // ecx
	int iyp; // edx
	char monstrnd[4]; // [esp+4h] [ebp-8h]

	monstrnd[0] = 6;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	r = random(0, 100) + 1;
	ixp = 0;
	iyp = 0;
	if ( r > 0 )
	{
		while ( 1 )
		{
			if ( dung_map[ixp][iyp] == themes[t].ttval && !nSolidTable[dPiece[ixp][iyp]] )
				--r;
			if ( r <= 0 )
				break;
			if ( ++ixp == 112 )
			{
				ixp = 0;
				if ( ++iyp == 112 )
					iyp = 0;
			}
		}
	}
	CreateRndItem(ixp, iyp, 1u, 0, 1);
	ItemNoFlippy();
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_SkelRoom(int t)
{
	int yp; // esi
	int xp; // edi
	char monstrnd[4]; // [esp+Bh] [ebp-5h]

	monstrnd[0] = 6;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	TFit_SkelRoom(t);
	yp = themey;
	xp = themex;
	AddObject(OBJ_SKFIRE, themex, themey);

	if ( random(0, monstrnd[leveltype-1]) )
		SpawnSkeleton(PreSpawnSkeleton(), xp - 1, yp - 1);
	else
		AddObject(OBJ_BANNERL, xp - 1, yp - 1);

	SpawnSkeleton(PreSpawnSkeleton(), xp, yp - 1);

	if ( random(0, monstrnd[leveltype-1]) )
		SpawnSkeleton(PreSpawnSkeleton(), xp + 1, yp - 1);
	else
		AddObject(OBJ_BANNERR, xp + 1, yp - 1);

	if ( random(0, monstrnd[leveltype-1]) )
		SpawnSkeleton(PreSpawnSkeleton(), xp - 1, yp);
	else
		AddObject(OBJ_BANNERM, xp - 1, yp);

	if ( random(0, monstrnd[leveltype-1]) )
		SpawnSkeleton(PreSpawnSkeleton(), xp + 1, yp);
	else
		AddObject(OBJ_BANNERM, xp + 1, yp);

	if ( random(0, monstrnd[leveltype-1]) )
		SpawnSkeleton(PreSpawnSkeleton(), xp - 1, yp + 1);
	else
		AddObject(OBJ_BANNERR, xp - 1, yp + 1);

	SpawnSkeleton(PreSpawnSkeleton(), xp, yp + 1);

	if ( random(0, monstrnd[leveltype-1]) )
		SpawnSkeleton(PreSpawnSkeleton(), xp + 1, yp + 1);
	else
		AddObject(OBJ_BANNERL, xp + 1, yp + 1);

	if ( !dObject[xp][yp - 3] ) // dungeon[xp + 39][yp + 37] ) /* fix */
		AddObject(OBJ_SKELBOOK, xp, yp - 2);
	if ( !dObject[xp][yp + 3] )
		AddObject(OBJ_SKELBOOK, xp, yp + 2);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_Treasure(int t)
{
	int yp; // esi
	int xp; // edi
	int rv; // [esp+14h] [ebp-10h]
	char monstrnd[4]; // [esp+18h] [ebp-Ch]
	char treasrnd[4]; // [esp+20h] [ebp-4h]

	treasrnd[0] = 4;
	treasrnd[1] = 9;
	treasrnd[2] = 7;
	treasrnd[3] = 10;
	monstrnd[0] = 6;
	monstrnd[1] = 8;
	monstrnd[2] = 3;
	monstrnd[3] = 7;
	GetRndSeed();
	for(yp = 0; yp < 112; yp++)
	{
		for(xp = 0; xp < 112; xp++)
		{
			if ( dung_map[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]] )
			{
				rv = random(0, treasrnd[leveltype-1]);
				if ( !(2 * random(0, treasrnd[leveltype-1])) )
				{
					CreateTypeItem(xp, yp, 0, ITYPE_GOLD, 0, 0, 1);
					ItemNoFlippy();
				}
				if ( !rv )
				{
					CreateRndItem(xp, yp, 0, 0, 1);
					ItemNoFlippy();
				}
				if ( rv >= treasrnd[leveltype-1] - 2 && leveltype != 1 )
					item[ItemNoFlippy()]._ivalue >>= 1; /* check */
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_Library(int t)
{
	int v1; // edi
	int v2; // ebx
	char *v3; // esi
	//int v4; // eax
	int v7; // eax
	//int v8; // eax
	int ta; // [esp+Ch] [ebp-14h]
	int *v10; // [esp+10h] [ebp-10h]
	int *v11; // [esp+14h] [ebp-Ch]
	char monstrnd[4]; // [esp+18h] [ebp-8h]
	char librnd[4]; // [esp+1Ch] [ebp-4h]

	ta = t;
	librnd[0] = 1;
	librnd[1] = 2;
	librnd[2] = 2;
	librnd[3] = 5;
	monstrnd[0] = 5;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	TFit_Shrine(t);
	v1 = 1;
	if ( themeVar1 == 1 )
	{
		AddObject(OBJ_BOOKCANDLE, themex - 1, themey);
		AddObject(OBJ_BOOKCASER, themex, themey);
		AddObject(OBJ_BOOKCANDLE, themex + 1, themey);
	}
	else
	{
		AddObject(OBJ_BOOKCANDLE, themex, themey - 1);
		AddObject(OBJ_BOOKCASEL, themex, themey);
		AddObject(OBJ_BOOKCANDLE, themex, themey + 1);
	}
	v11 = &dMonster[1][1];
	do
	{
		v2 = 1;
		v3 = &dObject[1][v1];
		v10 = v11;
		do
		{
			//LOBYTE(v4) = CheckThemeObj3(v2, v1, ta, -1);
			if ( CheckThemeObj3(v2, v1, ta, -1) )
			{
				if ( !*v10 )
				{
					if ( !random(0, librnd[leveltype-1]) )
					{
						AddObject(OBJ_BOOKSTAND, v2, v1);
						if ( random(0, 2 * librnd[leveltype-1]) )
						{
							v7 = *v3 - 1;
							object[v7]._oSelFlag = 0;
							object[v7]._oAnimFrame += 2;
						}
					}
				}
			}
			v10 += 112;
			++v2;
			v3 += 112;
		}
		while ( v2 < 111 );
		++v11;
		++v1;
	}
	while ( (signed int)v11 < (signed int)&dMonster[1][111] );
	//LOBYTE(v8) = QuestStatus(QTYPE_ZHAR);
	if ( !QuestStatus(QTYPE_ZHAR) || ta != zharlib )
		PlaceThemeMonsts(ta, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;
// 6AAA64: using guessed type int zharlib;

void __fastcall Theme_Torture(int t)
{
	int v1; // ebx
	int v2; // esi
	char *v3; // edi
	//int v4; // eax
	int *x; // [esp+Ch] [ebp-14h]
	char monstrnd[4]; // [esp+10h] [ebp-10h]
	int *v8; // [esp+14h] [ebp-Ch]
	char tortrnd[4]; // [esp+18h] [ebp-8h]
	int v10; // [esp+1Ch] [ebp-4h]

	v1 = t;
	tortrnd[0] = 6;
	tortrnd[1] = 8;
	tortrnd[2] = 3;
	tortrnd[3] = 8;
	monstrnd[0] = 6;
	monstrnd[1] = 8;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	v2 = 1;
	v8 = &dPiece[1][1];
	do
	{
		v10 = 1;
		v3 = &dung_map[1][v2];
		x = v8;
		do
		{
			if ( *v3 == themes[v1].ttval && !nSolidTable[*x] )
			{
				//LOBYTE(v4) = CheckThemeObj3(v10, v2, v1, -1);
				if ( CheckThemeObj3(v10, v2, v1, -1) )
				{
					if ( !random(0, tortrnd[leveltype-1]) )
						AddObject(OBJ_TNUDEM2, v10, v2);
				}
			}
			++v10;
			x += 112;
			v3 += 112;
		}
		while ( v10 < 111 );
		++v8;
		++v2;
	}
	while ( (signed int)v8 < (signed int)&dPiece[1][111] );
	PlaceThemeMonsts(v1, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_BloodFountain(int t)
{
	char monstrnd[4]; // [esp+3h] [ebp-5h]

	monstrnd[0] = 6;
	monstrnd[1] = 8;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	TFit_Obj5(t);
	AddObject(OBJ_BLOODFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_Decap(int t)
{
	int v1; // ebx
	int v2; // esi
	char *v3; // edi
	//int v4; // eax
	int *x; // [esp+Ch] [ebp-14h]
	char monstrnd[4]; // [esp+10h] [ebp-10h]
	int *v8; // [esp+14h] [ebp-Ch]
	char decaprnd[4]; // [esp+18h] [ebp-8h]
	int v10; // [esp+1Ch] [ebp-4h]

	v1 = t;
	decaprnd[0] = 6;
	decaprnd[1] = 8;
	decaprnd[2] = 3;
	decaprnd[3] = 8;
	monstrnd[0] = 6;
	monstrnd[1] = 8;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	v2 = 1;
	v8 = &dPiece[1][1];
	do
	{
		v10 = 1;
		v3 = &dung_map[1][v2];
		x = v8;
		do
		{
			if ( *v3 == themes[v1].ttval && !nSolidTable[*x] )
			{
				//LOBYTE(v4) = CheckThemeObj3(v10, v2, v1, -1);
				if ( CheckThemeObj3(v10, v2, v1, -1) )
				{
					if ( !random(0, decaprnd[leveltype-1]) )
						AddObject(OBJ_DECAP, v10, v2);
				}
			}
			++v10;
			x += 112;
			v3 += 112;
		}
		while ( v10 < 111 );
		++v8;
		++v2;
	}
	while ( (signed int)v8 < (signed int)&dPiece[1][111] );
	PlaceThemeMonsts(v1, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_PurifyingFountain(int t)
{
	char monstrnd[4]; // [esp+3h] [ebp-5h]

	monstrnd[0] = 6;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	TFit_Obj5(t);
	AddObject(OBJ_PURIFYINGFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_ArmorStand(int t)
{
	int v1; // esi
	int v2; // ebx
	char *v3; // edi
	//int v4; // eax
	int ta; // [esp+Ch] [ebp-14h]
	int *v7; // [esp+10h] [ebp-10h]
	char monstrnd[4]; // [esp+14h] [ebp-Ch]
	int *v9; // [esp+18h] [ebp-8h]
	char armorrnd[4]; // [esp+1Ch] [ebp-4h]

	v1 = 0;
	ta = t;
	armorrnd[0] = 6;
	armorrnd[1] = 8;
	armorrnd[2] = 3;
	armorrnd[3] = 8;
	monstrnd[0] = 6;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	if ( armorFlag )
	{
		TFit_Obj3(t);
		AddObject(OBJ_ARMORSTAND, themex, themey);
	}
	v9 = (int *)dPiece;
	do
	{
		v2 = 0;
		v3 = (char *)dung_map + v1;
		v7 = v9;
		do
		{
			if ( *v3 == themes[ta].ttval && !nSolidTable[*v7] )
			{
				//LOBYTE(v4) = CheckThemeObj3(v2, v1, ta, -1);
				if ( CheckThemeObj3(v2, v1, ta, -1) )
				{
					if ( !random(0, armorrnd[leveltype-1]) )
						AddObject(OBJ_ARMORSTANDN, v2, v1);
				}
			}
			v7 += 112;
			++v2;
			v3 += 112;
		}
		while ( v2 < 112 );
		++v9;
		++v1;
	}
	while ( (signed int)v9 < (signed int)dPiece[1] );
	PlaceThemeMonsts(ta, monstrnd[leveltype-1]);
	armorFlag = 0;
}
// 5BB1ED: using guessed type char leveltype;
// 6AAA3C: using guessed type int armorFlag;

void __fastcall Theme_GoatShrine(int t)
{
	int v1; // edx
	int v2; // esi
	int v3; // ecx
	int v4; // edi
	int v5; // eax
	char *v6; // ebx
	_DWORD *v7; // [esp+4h] [ebp-8h]

	TFit_GoatShrine(t);
	AddObject(OBJ_GOATSHRINE, themex, themey);
	v1 = themey;
	v2 = themey - 1;
	if ( themey - 1 <= themey + 1 )
	{
		v3 = themex;
		do
		{
			v4 = v3 - 1;
			if ( (unsigned char)(__OFSUB__(v3 - 1, v3 + 1) ^ 1) | (v3 - 1 == v3 + 1) )
			{
				v5 = v2 + 112 * v4;
				v6 = (char *)dung_map + v5;
				v7 = (_DWORD *)((char *)dPiece + 4 * v5);
				do
				{
					if ( *v6 == themes[t].ttval && !nSolidTable[*v7] && (v4 != v3 || v2 != v1) )
					{
						AddMonster(v4, v2, 1, themeVar1, 1);
						v1 = themey;
						v3 = themex;
					}
					v7 += 112;
					++v4;
					v6 += 112;
				}
				while ( v4 <= v3 + 1 );
			}
			++v2;
		}
		while ( v2 <= v1 + 1 );
	}
}

void __fastcall Theme_Cauldron(int t)
{
	char monstrnd[4]; // [esp+3h] [ebp-5h]

	monstrnd[0] = 6;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	TFit_Obj5(t);
	AddObject(OBJ_CAULDRON, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_MurkyFountain(int t)
{
	char monstrnd[4]; // [esp+3h] [ebp-5h]

	monstrnd[0] = 6;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	TFit_Obj5(t);
	AddObject(OBJ_MURKYFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_TearFountain(int t)
{
	char monstrnd[4]; // [esp+3h] [ebp-5h]

	monstrnd[0] = 6;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	TFit_Obj5(t);
	AddObject(OBJ_TEARFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype-1]);
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall Theme_BrnCross(int t)
{
	int v1; // esi
	int v2; // ebx
	char *v3; // edi
	//int v4; // eax
	int ta; // [esp+Ch] [ebp-14h]
	int *v7; // [esp+10h] [ebp-10h]
	char monstrnd[4]; // [esp+14h] [ebp-Ch]
	int *v9; // [esp+18h] [ebp-8h]
	char bcrossrnd[4]; // [esp+1Ch] [ebp-4h]

	ta = t;
	monstrnd[0] = 6;
	monstrnd[1] = 8;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	bcrossrnd[0] = 5;
	bcrossrnd[1] = 7;
	bcrossrnd[2] = 3;
	bcrossrnd[3] = 8;
	v1 = 0;
	v9 = (int *)dPiece;
	do
	{
		v2 = 0;
		v3 = (char *)dung_map + v1;
		v7 = v9;
		do
		{
			if ( *v3 == themes[ta].ttval && !nSolidTable[*v7] )
			{
				//LOBYTE(v4) = CheckThemeObj3(v2, v1, ta, -1);
				if ( CheckThemeObj3(v2, v1, ta, -1) )
				{
					if ( !random(0, bcrossrnd[leveltype-1]) )
						AddObject(OBJ_TBCROSS, v2, v1);
				}
			}
			v7 += 112;
			++v2;
			v3 += 112;
		}
		while ( v2 < 112 );
		++v9;
		++v1;
	}
	while ( (signed int)v9 < (signed int)dPiece[1] );
	PlaceThemeMonsts(ta, monstrnd[leveltype-1]);
	bCrossFlag = 1;
}
// 5BB1ED: using guessed type char leveltype;
// 6AAC10: using guessed type int bCrossFlag;

void __fastcall Theme_WeaponRack(int t)
{
	int v1; // esi
	int v2; // ebx
	char *v3; // edi
	//int v4; // eax
	int ta; // [esp+Ch] [ebp-14h]
	int *v7; // [esp+10h] [ebp-10h]
	char monstrnd[4]; // [esp+14h] [ebp-Ch]
	int *v9; // [esp+18h] [ebp-8h]
	char weaponrnd[4]; // [esp+1Ch] [ebp-4h]

	v1 = 0;
	ta = t;
	weaponrnd[0] = 6;
	weaponrnd[1] = 8;
	weaponrnd[2] = 5;
	weaponrnd[3] = 8;
	monstrnd[0] = 6;
	monstrnd[1] = 7;
	monstrnd[2] = 3;
	monstrnd[3] = 9;
	if ( weaponFlag )
	{
		TFit_Obj3(t);
		AddObject(OBJ_WEAPONRACK, themex, themey);
	}
	v9 = (int *)dPiece;
	do
	{
		v2 = 0;
		v3 = (char *)dung_map + v1;
		v7 = v9;
		do
		{
			if ( *v3 == themes[ta].ttval && !nSolidTable[*v7] )
			{
				//LOBYTE(v4) = CheckThemeObj3(v2, v1, ta, -1);
				if ( CheckThemeObj3(v2, v1, ta, -1) )
				{
					if ( !random(0, weaponrnd[leveltype-1]) )
						AddObject(OBJ_WEAPONRACKN, v2, v1);
				}
			}
			v7 += 112;
			++v2;
			v3 += 112;
		}
		while ( v2 < 112 );
		++v9;
		++v1;
	}
	while ( (signed int)v9 < (signed int)dPiece[1] );
	PlaceThemeMonsts(ta, monstrnd[leveltype-1]);
	weaponFlag = 0;
}
// 5BB1ED: using guessed type char leveltype;
// 6AAA50: using guessed type int weaponFlag;

void __cdecl UpdateL4Trans()
{
	int i; // ecx
	int j; // edx

	for(i = 0; i < 112; i++)
	{
		for(j = 0; j < 112; j++)
		{
			if ( dung_map[i][j] )
				dung_map[i][j] = 1;
		}
	}
}

void __cdecl CreateThemeRooms()
{
	int i; // esi

	if ( currlevel != 16 )
	{
		InitObjFlag = 1;
		for ( i = 0; i < numthemes; i++ )
		{
			themex = 0;
			themey = 0;
			switch ( themes[i].ttype )
			{
				case THEME_BARREL:
					Theme_Barrel(i);
					break;
				case THEME_SHRINE:
					Theme_Shrine(i);
					break;
				case THEME_MONSTPIT:
					Theme_MonstPit(i);
					break;
				case THEME_SKELROOM:
					Theme_SkelRoom(i);
					break;
				case THEME_TREASURE:
					Theme_Treasure(i);
					break;
				case THEME_LIBRARY:
					Theme_Library(i);
					break;
				case THEME_TORTURE:
					Theme_Torture(i);
					break;
				case THEME_BLOODFOUNTAIN:
					Theme_BloodFountain(i);
					break;
				case THEME_DECAPITATED:
					Theme_Decap(i);
					break;
				case THEME_PURIFYINGFOUNTAIN:
					Theme_PurifyingFountain(i);
					break;
				case THEME_ARMORSTAND:
					Theme_ArmorStand(i);
					break;
				case THEME_GOATSHRINE:
					Theme_GoatShrine(i);
					break;
				case THEME_CAULDRON:
					Theme_Cauldron(i);
					break;
				case THEME_MURKYFOUNTAIN:
					Theme_MurkyFountain(i);
					break;
				case THEME_TEARFOUNTAIN:
					Theme_TearFountain(i);
					break;
				case THEME_BRNCROSS:
					Theme_BrnCross(i);
					break;
				case THEME_WEAPONRACK:
					Theme_WeaponRack(i);
					break;
				default:
					continue;
			}
		}
		InitObjFlag = 0;
		if ( leveltype == DTYPE_HELL && themeCount > 0 )
			UpdateL4Trans();
	}
}
// 5BB1ED: using guessed type char leveltype;
// 67D7C0: using guessed type int InitObjFlag;
