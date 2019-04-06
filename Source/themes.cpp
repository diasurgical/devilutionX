//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

int numthemes; // idb
BOOL armorFlag;
int ThemeGoodIn[4];
BOOL weaponFlag;
BOOLEAN treasureFlag;  // weak
BOOLEAN mFountainFlag; // weak
BOOLEAN cauldronFlag;  // weak
BOOLEAN tFountainFlag; // weak
int zharlib;
int themex;    // idb
int themey;    // idb
int themeVar1; // idb
ThemeStruct themes[MAXTHEMES];
BOOLEAN pFountainFlag; // weak
BOOLEAN bFountainFlag; // weak
BOOL bCrossFlag;

int ThemeGood[4] = { THEME_GOATSHRINE, THEME_SHRINE, THEME_SKELROOM, THEME_LIBRARY };

int trm5x[25] = {
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2
};

int trm5y[25] = {
	-2, -2, -2, -2, -2,
	-1, -1, -1, -1, -1,
	0, 0, 0, 0, 0,
	1, 1, 1, 1, 1,
	2, 2, 2, 2, 2
};

int trm3x[9] = {
	-1, 0, 1,
	-1, 0, 1,
	-1, 0, 1
};

int trm3y[9] = {
	-1, -1, -1,
	0, 0, 0,
	1, 1, 1
};

BOOLEAN __fastcall TFit_Shrine(int i)
{
	int v1;        // ecx
	int v2;        // esi
	int v3;        // eax
	int v4;        // edx
	signed int v6; // [esp+Ch] [ebp-8h]
	int v7;        // [esp+10h] [ebp-4h]

	v1 = themes[i].ttval;
	v7 = 0;
	v2 = 0;
	v6 = 0;
	while (1) {
		v3 = v2 + 112 * v7;
		if (dTransVal[0][v3] != v1) { /* check */
			goto LABEL_20;
		}
		v4 = dPiece[0][v3 - 1]; // *(_DWORD *)&dflags[39][4 * v3 + 36];
		if (nTrapTable[v4]
		    && !nSolidTable[dPiece[-1][v3]] // !nSolidTable[*(_DWORD *)&dflags[28][4 * v3 + 32]]
		    && !nSolidTable[dPiece[1][v3]]
		    && dTransVal[-1][v3] == v1 // block_lvid[v3 + 1940] == v1
		    && dTransVal[1][v3] == v1
		    && !dObject[-1][v3 - 1]
		    && !dObject[0][v3 + 111]) {
			v6 = 1;
		}
		if (v6) {
			break;
		}
		if (!nTrapTable[dPiece[-1][v3]] // !nTrapTable[*(_DWORD *)&dflags[28][4 * v3 + 32]]
		    || nSolidTable[v4]
		    || nSolidTable[dPiece[0][v3 + 1]]
		    || dTransVal[0][v3 - 1] != v1 // *(&byte_5B78EB + v3) != v1
		    || dTransVal[0][v3 + 1] != v1
		    || dObject[-1][v3 - 1]
		    || dObject[-1][v3 + 1]) /* check */
		{
			goto LABEL_21;
		}
		v6 = 2;
	LABEL_20:
		if (v6) {
			break;
		}
	LABEL_21:
		if (++v7 == MAXDUNX) {
			++v2;
			v7 = 0;
			if (v2 == MAXDUNY) {
				return 0;
			}
		}
	}
	themey = v2;
	themex = v7;
	themeVar1 = v6;
	return 1;
}

BOOLEAN __fastcall TFit_Obj5(int t)
{
	int v2;         // ebx
	int v3;         // esi
	int v4;         // eax
	int v5;         // edi
	int v6;         // ecx
	signed int v7;  // edx
	int v8;         // ecx
	int v10;        // [esp+Ch] [ebp-Ch]
	int v11;        // [esp+10h] [ebp-8h]
	signed int v12; // [esp+14h] [ebp-4h]

	v2 = 0;
	v3 = 0;
	v4 = random(0, 5) + 1;
	v10 = v4;
	if (v4 <= 0) {
	LABEL_19:
		themex = v2;
		themey = v3;
		return 1;
	}
	v5 = themes[t].ttval;
	v11 = v5;
	while (1) {
		v6 = v3 + 112 * v2;
		if (dTransVal[0][v6] == v5 && !nSolidTable[dPiece[0][v6]]) {
			v12 = 1;
			v7 = 0;
			do {
				if (v7 >= 25) {
					break;
				}
				v8 = v3 + trm5y[v7] + 112 * (v2 + trm5x[v7]);
				if (nSolidTable[dPiece[0][v8]]) {
					v12 = 0;
				}
				v5 = v11;
				if (dTransVal[0][v8] != v11) {
					v12 = 0;
				}
				++v7;
			} while (v12);
			if (v12) {
				--v4;
				goto LABEL_18;
			}
		}
		if (++v2 != 112) {
			goto LABEL_18;
		}
		v2 = 0;
		if (++v3 != 112) {
			goto LABEL_18;
		}
		if (v4 == v10) {
			return 0;
		}
		v3 = 0;
	LABEL_18:
		if (v4 <= 0) {
			goto LABEL_19;
		}
	}
}

BOOLEAN __fastcall TFit_SkelRoom(int t)
{
	int i; // esi

	if (leveltype != 1 && leveltype != 2) {
		return 0;
	}
	i = 0;
	if (nummtypes <= 0) {
		return 0;
	}

	while (!IsSkel(Monsters[i].mtype)) {
		++i;
		if (i >= nummtypes) {
			return 0;
		}
	}
	themeVar1 = i;
	return TFit_Obj5(t);
}

BOOLEAN __fastcall TFit_GoatShrine(int t)
{
	int i; // esi

	i = 0;
	if (nummtypes <= 0) {
		return 0;
	}

	while (!IsGoat(Monsters[i].mtype)) {
		++i;
		if (i >= nummtypes) {
			return 0;
		}
	}
	themeVar1 = i;
	return TFit_Obj5(t);
}

BOOL __fastcall CheckThemeObj3(int xp, int yp, int t, int f)
{
	int i;

	for (i = 0; i < 9; i++) {
		if (xp + trm3x[i] < 0 || yp + trm3y[i] < 0)
			return FALSE;
		if (nSolidTable[dPiece[xp + trm3x[i]][yp + trm3y[i]]])
			return FALSE;
		if (dTransVal[xp + trm3x[i]][yp + trm3y[i]] != themes[t].ttval)
			return FALSE;
		if (dObject[xp + trm3x[i]][yp + trm3y[i]])
			return FALSE;
		if (f != -1 && !random(0, f))
			return FALSE;
	}

	return TRUE;
}

BOOLEAN __fastcall TFit_Obj3(int t)
{
	int yp;         // edi
	int xp;         // esi
	char objrnd[4]; // [esp+Bh] [ebp-5h]

	objrnd[0] = 4;
	objrnd[1] = 4;
	objrnd[2] = 3;
	objrnd[3] = 5;
	yp = 1;
	while (2) {
		xp = 1;
		do {
			if (CheckThemeObj3(xp, yp, t, objrnd[leveltype - 1])) {
				themex = xp;
				themey = yp;
				return 1;
			}
			++xp;
		} while (xp < 111);
		if (++yp < 111) {
			continue;
		}
		break;
	}
	return 0;
}

BOOLEAN __fastcall CheckThemeReqs(int t)
{
	BOOLEAN rv; // al
	int v2;     // ecx
	int v3;     // ecx
	int v4;     // ecx
	int v5;     // ecx
	BOOLEAN v6; // zf
	int v7;     // ecx
	int v8;     // ecx
	int v9;     // ecx

	rv = 1;
	if (t <= 10) {
		if (t != 10) {
			v2 = t - 1;
			if (v2) {
				v3 = v2 - 2;
				if (v3) {
					v4 = v3 - 2;
					if (v4) {
						v5 = v4 - 2;
						if (v5) {
							if (v5 != 2) {
								return rv;
							}
							v6 = pFountainFlag == 0;
						} else {
							v6 = bFountainFlag == 0;
						}
					LABEL_21:
						if (!v6) {
							return rv;
						}
						return 0;
					}
				}
			}
			if (leveltype != 3) {
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
	if (v7) {
		v8 = v7 - 1;
		if (!v8) {
			v6 = mFountainFlag == 0;
			goto LABEL_21;
		}
		v9 = v8 - 1;
		if (!v9) {
			v6 = tFountainFlag == 0;
			goto LABEL_21;
		}
		if (v9 != 2) {
			return rv;
		}
		goto LABEL_16;
	}
	if (leveltype == DTYPE_HELL) {
		v6 = cauldronFlag == 0;
		goto LABEL_21;
	}
	return 0;
}
// 6AAA58: using guessed type int mFountainFlag;
// 6AAA5C: using guessed type int cauldronFlag;
// 6AAA60: using guessed type int tFountainFlag;
// 6AAC08: using guessed type int pFountainFlag;
// 6AAC0C: using guessed type int bFountainFlag;

BOOLEAN __fastcall SpecialThemeFit(int i, int t)
{
	BOOLEAN rv; // eax

	rv = CheckThemeReqs(t);
	switch (t) {
	case THEME_SHRINE:
	case THEME_LIBRARY:
		if (rv) {
			rv = TFit_Shrine(i);
		}
		break;
	case THEME_SKELROOM:
		if (rv) {
			rv = TFit_SkelRoom(i);
		}
		break;
	case THEME_TREASURE:
		rv = treasureFlag;
		if (treasureFlag) {
			treasureFlag = 0;
		}
		break;
	case THEME_TORTURE:
	case THEME_DECAPITATED:
	case THEME_ARMORSTAND:
	case THEME_BRNCROSS:
	case THEME_WEAPONRACK:
		if (rv) {
			rv = TFit_Obj3(i);
		}
		break;
	case THEME_BLOODFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
			if (rv) {
				bFountainFlag = 0;
			}
		}
		break;
	case THEME_PURIFYINGFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
			if (rv) {
				pFountainFlag = 0;
			}
		}
		break;
	case THEME_GOATSHRINE:
		if (rv) {
			rv = TFit_GoatShrine(i);
		}
		break;
	case THEME_CAULDRON:
		if (rv) {
			rv = TFit_Obj5(i);
			if (rv) {
				cauldronFlag = 0;
			}
		}
		break;
	case THEME_MURKYFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
			if (rv) {
				mFountainFlag = 0;
			}
		}
		break;
	case THEME_TEARFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
			if (rv) {
				tFountainFlag = 0;
			}
		}
		break;
	}
	return rv;
}
// 6AAA54: using guessed type int treasureFlag;
// 6AAA58: using guessed type int mFountainFlag;
// 6AAA5C: using guessed type int cauldronFlag;
// 6AAA60: using guessed type int tFountainFlag;
// 6AAC08: using guessed type int pFountainFlag;
// 6AAC0C: using guessed type int bFountainFlag;

BOOLEAN __fastcall CheckThemeRoom(int tv)
{
	int v1;         // esi
	int *v2;        // edx
	signed int v3;  // edi
	signed int v4;  // esi
	signed int v5;  // edx
	signed int v6;  // eax
	int v7;         // edi
	int *v8;        // esi
	char *v9;       // eax
	int *v10;       // edx
	signed int v12; // [esp+Ch] [ebp-8h]

	v1 = 0;
	if (trigflag[4] <= 0) {
	LABEL_5:
		v3 = 0;
		v4 = 0;
		do {
			v5 = 0;
			v6 = v4;
			do {
				if (dTransVal[0][v6] == tv) {
					if (dFlags[0][v6] & DFLAG_POPULATED) {
						return 0;
					}
					++v3;
				}
				++v5;
				v6 += 112;
			} while (v5 < 112);
			++v4;
		} while (v4 < 112);
		if (leveltype != 1 || v3 >= 9 && v3 <= 100) {
			v7 = 0;
			v8 = &dPiece[-1][111];
		LABEL_16:
			v12 = 0;
			v9 = &dTransVal[-1][v7 + 111];
			v10 = v8;
			while (v9[1] != tv
			    || nSolidTable[v10[1]]
			    || (*(v9 - 111) == tv || nSolidTable[*(v10 - 111)]) /* check */
			        && (v9[113] == tv || nSolidTable[v10[113]])
			        && (*v9 == tv || nSolidTable[*v10])
			        && (v9[2] == tv || nSolidTable[v10[2]])) {
				++v12;
				v10 += 112;
				v9 += 112;
				if (v12 >= 112) {
					++v8;
					++v7;
					if ((signed int)v8 < (signed int)&dPiece[0][111]) {
						goto LABEL_16;
					}
					return 1;
				}
			}
		}
	} else {
		v2 = &trigs[0]._ty;
		while (dTransVal[*(v2 - 1)][*v2] != tv) {
			++v1;
			v2 += 4;
			if (v1 >= trigflag[4]) {
				goto LABEL_5;
			}
		}
	}
	return 0;
}

void __cdecl InitThemes()
{
	int v0;  // esi
	char v1; // bl
	int v2;  // edi
	//int v3; // eax
	int i; // ebx
	//int v6; // eax
	int v8; // esi
	int v9; // ecx
	int j;  // eax
	//int v11; // eax
	int *v13; // edi
	int v14;  // esi
	int *v15; // ebx
	//int v16; // eax
	int v17; // eax
	int k;   // esi
	int l;   // ebx
	//int v20; // eax

	zharlib = -1;
	v0 = 0;
	bCrossFlag = FALSE;
	numthemes = 0;
	armorFlag = TRUE;
	bFountainFlag = 1;
	cauldronFlag = 1;
	mFountainFlag = 1;
	pFountainFlag = 1;
	tFountainFlag = 1;
	treasureFlag = 1;
	weaponFlag = TRUE;
	if (currlevel != 16) {
		v1 = leveltype;
		if (leveltype == DTYPE_CATHEDRAL) {
			ThemeGoodIn[0] = 0;
			ThemeGoodIn[1] = 0;
			ThemeGoodIn[2] = 0;
			ThemeGoodIn[3] = 0;
			v2 = 0;
			do {
				if (v0 >= MAXTHEMES) {
					break;
				}
				//_LOBYTE(v3) = CheckThemeRoom(v2);
				if (CheckThemeRoom(v2)) {
					themes[v0].ttval = v2;
					for (i = ThemeGood[random(0, 4)];; i = random(0, 17)) {
						//_LOBYTE(v6) = SpecialThemeFit(numthemes, i);
						if (SpecialThemeFit(numthemes, i)) {
							break;
						}
					}
					v8 = numthemes;
					themes[numthemes].ttype = i;
					v1 = leveltype;
					v0 = v8 + 1;
					numthemes = v0;
				}
				++v2;
			} while (v2 < 256);
		}
		if (v1 == 2 || v1 == 3 || v1 == 4) {
			v9 = themeCount;
			for (j = 0; j < v9; ++j)
				themes[j].ttype = -1;
			//_LOBYTE(v11) = QuestStatus(QTYPE_ZHAR);
			v13 = &themeLoc[0].ttval;
			if (QuestStatus(QTYPE_ZHAR)) {
				v14 = 0;
				if (themeCount > 0) {
					v15 = &themeLoc[0].ttval;
					while (1) {
						themes[v14].ttval = *v15;
						//_LOBYTE(v16) = SpecialThemeFit(v14, 5);
						if (SpecialThemeFit(v14, 5)) {
							break;
						}
						++v14;
						v15 += 5;
						if (v14 >= themeCount) {
							goto LABEL_23;
						}
					}
					themes[v14].ttype = 5;
					zharlib = v14;
				}
			}
		LABEL_23:
			v17 = themeCount;
			for (k = 0; k < themeCount; v13 += 5) {
				if (themes[k].ttype == -1) {
					themes[k].ttval = *v13;
					for (l = ThemeGood[random(0, 4)];; l = random(0, 17)) {
						//_LOBYTE(v20) = SpecialThemeFit(k, l);
						if (SpecialThemeFit(k, l)) {
							break;
						}
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
// 6AAA54: using guessed type int treasureFlag;
// 6AAA58: using guessed type int mFountainFlag;
// 6AAA5C: using guessed type int cauldronFlag;
// 6AAA60: using guessed type int tFountainFlag;
// 6AAC08: using guessed type int pFountainFlag;
// 6AAC0C: using guessed type int bFountainFlag;

// HoldThemeRooms marks theme rooms as populated.
void __cdecl HoldThemeRooms()
{
	int i, x, y;

	if (currlevel != 16) {
		if (leveltype == DTYPE_CATHEDRAL) {
			for (i = 0; i < numthemes; i++) {
				for (y = 0; y < MAXDUNY; y++) {
					for (x = 0; x < MAXDUNX; x++) {
						if (dTransVal[x][y] == (char)themes[i].ttval) {
							dFlags[x][y] |= DFLAG_POPULATED;
						}
					}
				}
			}
		} else {
			DRLG_HoldThemeRooms();
		}
	}
}

// PlaceThemeMonsts places theme monsters with the specified frequency.
//
// Parameters:
//    - t: theme number (index into themes array).
//    - f: frequency (1/f likelihood of adding monster).
void __fastcall PlaceThemeMonsts(int t, int f)
{
	int xp, yp;
	int scattertypes[111];
	int numscattypes, mtype, i;

	numscattypes = 0;
	for (i = 0; i < nummtypes; i++) {
		if (Monsters[i].mPlaceFlags & 1) {
			scattertypes[numscattypes] = i;
			numscattypes++;
		}
	}
	mtype = scattertypes[random(0, numscattypes)];
	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]] && dItem[xp][yp] == 0 && dObject[xp][yp] == 0) {
				if (random(0, f) == 0) {
					AddMonster(xp, yp, random(0, 8), mtype, 1);
				}
			}
		}
	}
}

// Theme_Barrel initializes the barrel theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_Barrel(int t)
{
	int xp, yp, r;
	char barrnd[4] = { 2, 6, 4, 8 };
	char monstrnd[4] = { 5, 7, 3, 9 };

	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]]) {
				if (random(0, barrnd[leveltype - 1]) == 0) {
					r = random(0, barrnd[leveltype - 1]) != 0;
					r += OBJ_BARREL;
					AddObject(r, xp, yp);
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_Shrine initializes the shrine theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_Shrine(int t)
{
	char monstrnd[4] = { 6, 6, 3, 9 };

	TFit_Shrine(t);
	if (themeVar1 == 1) {
		AddObject(OBJ_CANDLE2, themex - 1, themey);
		AddObject(OBJ_SHRINER, themex, themey);
		AddObject(OBJ_CANDLE2, themex + 1, themey);
	} else {
		AddObject(OBJ_CANDLE2, themex, themey - 1);
		AddObject(OBJ_SHRINEL, themex, themey);
		AddObject(OBJ_CANDLE2, themex, themey + 1);
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_MonstPit initializes the monster pit theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_MonstPit(int t)
{
	int r;
	int ixp, iyp;
	char monstrnd[4] = { 6, 7, 3, 9 };

	r = random(0, 100) + 1;
	ixp = 0;
	iyp = 0;
	if (r > 0) {
		while (TRUE) {
			if (dTransVal[ixp][iyp] == themes[t].ttval && !nSolidTable[dPiece[ixp][iyp]]) {
				--r;
			}
			if (r <= 0) {
				break;
			}
			ixp++;
			if (ixp == MAXDUNX) {
				ixp = 0;
				iyp++;
				if (iyp == MAXDUNY) {
					iyp = 0;
				}
			}
		}
	}
	CreateRndItem(ixp, iyp, TRUE, FALSE, TRUE);
	ItemNoFlippy();
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_SkelRoom initializes the skeleton room theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_SkelRoom(int t)
{
	int xp, yp;
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_SkelRoom(t);
	xp = themex;
	yp = themey;
	AddObject(OBJ_SKFIRE, xp, yp);
	if (random(0, monstrnd[leveltype - 1]) != 0) {
		SpawnSkeleton(PreSpawnSkeleton(), xp - 1, yp - 1);
	} else {
		AddObject(OBJ_BANNERL, xp - 1, yp - 1);
	}
	SpawnSkeleton(PreSpawnSkeleton(), xp, yp - 1);
	if (random(0, monstrnd[leveltype - 1]) != 0) {
		SpawnSkeleton(PreSpawnSkeleton(), xp + 1, yp - 1);
	} else {
		AddObject(OBJ_BANNERR, xp + 1, yp - 1);
	}
	if (random(0, monstrnd[leveltype - 1]) != 0) {
		SpawnSkeleton(PreSpawnSkeleton(), xp - 1, yp);
	} else {
		AddObject(OBJ_BANNERM, xp - 1, yp);
	}
	if (random(0, monstrnd[leveltype - 1]) != 0) {
		SpawnSkeleton(PreSpawnSkeleton(), xp + 1, yp);
	} else {
		AddObject(OBJ_BANNERM, xp + 1, yp);
	}
	if (random(0, monstrnd[leveltype - 1]) != 0) {
		SpawnSkeleton(PreSpawnSkeleton(), xp - 1, yp + 1);
	} else {
		AddObject(OBJ_BANNERR, xp - 1, yp + 1);
	}
	SpawnSkeleton(PreSpawnSkeleton(), xp, yp + 1);
	if (random(0, monstrnd[leveltype - 1]) != 0) {
		SpawnSkeleton(PreSpawnSkeleton(), xp + 1, yp + 1);
	} else {
		AddObject(OBJ_BANNERL, xp + 1, yp + 1);
	}
	if (dObject[xp][yp - 3] == 0) {
		AddObject(OBJ_SKELBOOK, xp, yp - 2);
	}
	if (dObject[xp][yp + 3] == 0) {
		AddObject(OBJ_SKELBOOK, xp, yp + 2);
	}
}

// Theme_Treasure initializes the treasure theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_Treasure(int t)
{
	int xp, yp;
	int i;
	char treasrnd[4] = { 4, 9, 7, 10 };
	char monstrnd[4] = { 6, 8, 3, 7 };

	GetRndSeed();
	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]]) {
				int rv = random(0, treasrnd[leveltype - 1]);
				// BUGFIX: the `2*` in `2*random(0, treasrnd...) == 0` has no effect, should probably be `random(0, 2*treasrnd...) == 0`
				if ((2 * random(0, treasrnd[leveltype - 1])) == 0) {
					CreateTypeItem(xp, yp, FALSE, ITYPE_GOLD, IMISC_NONE, FALSE, TRUE);
					ItemNoFlippy();
				}
				if (rv == 0) {
					CreateRndItem(xp, yp, FALSE, FALSE, TRUE);
					ItemNoFlippy();
				}
				if (rv == 0 || rv >= treasrnd[leveltype - 1] - 2) {
					i = ItemNoFlippy();
					if (rv >= treasrnd[leveltype - 1] - 2 && leveltype != DTYPE_CATHEDRAL) {
						item[i]._ivalue >>= 1;
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_Library initializes the library theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_Library(int t)
{
	int xp, yp;
	int oi;
	char librnd[4] = { 1, 2, 2, 5 };
	char monstrnd[4] = { 5, 7, 3, 9 };

	TFit_Shrine(t);
	if (themeVar1 == 1) {
		AddObject(OBJ_BOOKCANDLE, themex - 1, themey);
		AddObject(OBJ_BOOKCASER, themex, themey);
		AddObject(OBJ_BOOKCANDLE, themex + 1, themey);
	} else {
		AddObject(OBJ_BOOKCANDLE, themex, themey - 1);
		AddObject(OBJ_BOOKCASEL, themex, themey);
		AddObject(OBJ_BOOKCANDLE, themex, themey + 1);
	}

	for (yp = 1; yp < MAXDUNY - 1; yp++) {
		for (xp = 1; xp < MAXDUNX - 1; xp++) {
			if (CheckThemeObj3(xp, yp, t, -1)) {
				if (dMonster[xp][yp] == 0) {
					if (random(0, librnd[leveltype - 1]) == 0) {
						AddObject(OBJ_BOOKSTAND, xp, yp);
						if (random(0, 2 * librnd[leveltype - 1]) != 0) {
							oi = dObject[xp][yp] - 1;
							object[oi]._oSelFlag = 0;
							object[oi]._oAnimFrame += 2;
						}
					}
				}
			}
		}
	}

	if (!QuestStatus(QTYPE_ZHAR) || t != zharlib) {
		PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
	}
}

// Theme_Torture initializes the torture theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_Torture(int t)
{
	int xp, yp;
	char tortrnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 8, 3, 9 };

	for (yp = 1; yp < MAXDUNY - 1; yp++) {
		for (xp = 1; xp < MAXDUNX - 1; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]]) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random(0, tortrnd[leveltype - 1]) == 0) {
						AddObject(OBJ_TNUDEM2, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_BloodFountain initializes the blood fountain theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_BloodFountain(int t)
{
	char monstrnd[4] = { 6, 8, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_BLOODFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_Decap initializes the decapitated theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_Decap(int t)
{
	int xp, yp;
	char decaprnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 8, 3, 9 };

	for (yp = 1; yp < MAXDUNY - 1; yp++) {
		for (xp = 1; xp < MAXDUNX - 1; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]]) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random(0, decaprnd[leveltype - 1]) == 0) {
						AddObject(OBJ_DECAP, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_PurifyingFountain initializes the purifying fountain theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_PurifyingFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_PURIFYINGFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_ArmorStand initializes the armor stand theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_ArmorStand(int t)
{
	int xp, yp;
	char armorrnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 7, 3, 9 };

	if (armorFlag) {
		TFit_Obj3(t);
		AddObject(OBJ_ARMORSTAND, themex, themey);
	}
	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]]) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random(0, armorrnd[leveltype - 1]) == 0) {
						AddObject(OBJ_ARMORSTANDN, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
	armorFlag = FALSE;
}

// Theme_GoatShrine initializes the goat shrine theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_GoatShrine(int t)
{
	int xx, yy;

	TFit_GoatShrine(t);
	AddObject(OBJ_GOATSHRINE, themex, themey);
	for (yy = themey - 1; yy <= themey + 1; yy++) {
		for (xx = themex - 1; xx <= themex + 1; xx++) {
			if (dTransVal[xx][yy] == themes[t].ttval && !nSolidTable[dPiece[xx][yy]] && (xx != themex || yy != themey)) {
				AddMonster(xx, yy, DIR_SW, themeVar1, 1);
			}
		}
	}
}

// Theme_Cauldron initializes the cauldron theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_Cauldron(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_CAULDRON, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_MurkyFountain initializes the murky fountain theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_MurkyFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_MURKYFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_TearFountain initializes the tear fountain theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_TearFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_TEARFTN, themex, themey);
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

// Theme_BrnCross initializes the burning cross theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_BrnCross(int t)
{
	int xp, yp;
	char monstrnd[4] = { 6, 8, 3, 9 };
	char bcrossrnd[4] = { 5, 7, 3, 8 };

	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]]) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random(0, bcrossrnd[leveltype - 1]) == 0) {
						AddObject(OBJ_TBCROSS, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
	bCrossFlag = TRUE;
}

// Theme_WeaponRack initializes the weapon rack theme.
//
// Parameters:
//    - t: theme number (index into themes array).
void __fastcall Theme_WeaponRack(int t)
{
	int xp, yp;
	char weaponrnd[4] = { 6, 8, 5, 8 };
	char monstrnd[4] = { 6, 7, 3, 9 };

	if (weaponFlag) {
		TFit_Obj3(t);
		AddObject(OBJ_WEAPONRACK, themex, themey);
	}
	for (yp = 0; yp < MAXDUNY; yp++) {
		for (xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && !nSolidTable[dPiece[xp][yp]]) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (random(0, weaponrnd[leveltype - 1]) == 0) {
						AddObject(OBJ_WEAPONRACKN, xp, yp);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
	weaponFlag = FALSE;
}

// UpdateL4Trans sets each value of the transparency map to 1.
void __cdecl UpdateL4Trans()
{
	int i, j;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dTransVal[i][j]) {
				dTransVal[i][j] = 1;
			}
		}
	}
}

// CreateThemeRooms adds thematic elements to rooms.
void __cdecl CreateThemeRooms()
{
	int i;

	if (currlevel == 16) {
		return;
	}
	InitObjFlag = TRUE;
	for (i = 0; i < numthemes; i++) {
		themex = 0;
		themey = 0;
		switch ((char)themes[i].ttype) {
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
		}
	}
	InitObjFlag = FALSE;
	if (leveltype == DTYPE_HELL && themeCount > 0) {
		UpdateL4Trans();
	}
}

DEVILUTION_END_NAMESPACE
