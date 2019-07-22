#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE L5dungeon[80][80];
BYTE L5dflags[DMAXX][DMAXY];
BOOL setloadflag;
int HR1;
int HR2;
int HR3;
int VR1;
int VR2;
int VR3;
BYTE *pSetPiece;

const ShadowStruct SPATS[37] = {
	// clang-format off
	// strig, s1, s2, s3, nv1, nv2, nv3
	{      7, 13,  0, 13, 144,   0, 142 },
	{     16, 13,  0, 13, 144,   0, 142 },
	{     15, 13,  0, 13, 145,   0, 142 },
	{      5, 13, 13, 13, 152, 140, 139 },
	{      5, 13,  1, 13, 143, 146, 139 },
	{      5, 13, 13,  2, 143, 140, 148 },
	{      5,  0,  1,  2,   0, 146, 148 },
	{      5, 13, 11, 13, 143, 147, 139 },
	{      5, 13, 13, 12, 143, 140, 149 },
	{      5, 13, 11, 12, 150, 147, 149 },
	{      5, 13,  1, 12, 143, 146, 149 },
	{      5, 13, 11,  2, 143, 147, 148 },
	{      9, 13, 13, 13, 144, 140, 142 },
	{      9, 13,  1, 13, 144, 146, 142 },
	{      9, 13, 11, 13, 151, 147, 142 },
	{      8, 13,  0, 13, 144,   0, 139 },
	{      8, 13,  0, 12, 143,   0, 149 },
	{      8,  0,  0,  2,   0,   0, 148 },
	{     11,  0,  0, 13,   0,   0, 139 },
	{     11, 13,  0, 13, 139,   0, 139 },
	{     11,  2,  0, 13, 148,   0, 139 },
	{     11, 12,  0, 13, 149,   0, 139 },
	{     11, 13, 11, 12, 139,   0, 149 },
	{     14,  0,  0, 13,   0,   0, 139 },
	{     14, 13,  0, 13, 139,   0, 139 },
	{     14,  2,  0, 13, 148,   0, 139 },
	{     14, 12,  0, 13, 149,   0, 139 },
	{     14, 13, 11, 12, 139,   0, 149 },
	{     10,  0, 13,  0,   0, 140,   0 },
	{     10, 13, 13,  0, 140, 140,   0 },
	{     10,  0,  1,  0,   0, 146,   0 },
	{     10, 13, 11,  0, 140, 147,   0 },
	{     12,  0, 13,  0,   0, 140,   0 },
	{     12, 13, 13,  0, 140, 140,   0 },
	{     12,  0,  1,  0,   0, 146,   0 },
	{     12, 13, 11,  0, 140, 147,   0 },
	{      3, 13, 11, 12, 150,   0,   0 }
	// clang-format on
};
const BYTE BSTYPES[206] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 10, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 14, 5, 14,
	10, 4, 14, 4, 5, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	2, 3, 4, 1, 6, 7, 16, 17, 2, 1,
	1, 2, 2, 1, 1, 2, 2, 2, 2, 2,
	1, 1, 11, 1, 13, 13, 13, 1, 2, 1,
	2, 1, 2, 1, 2, 2, 2, 2, 12, 0,
	0, 11, 1, 11, 1, 13, 0, 0, 0, 0,
	0, 0, 0, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 1, 11, 2, 12,
	13, 13, 13, 12, 2, 1, 2, 2, 4, 14,
	4, 10, 13, 13, 4, 4, 1, 1, 4, 2,
	2, 13, 13, 13, 13, 25, 26, 28, 30, 31,
	41, 43, 40, 41, 42, 43, 25, 41, 43, 28,
	28, 1, 2, 25, 26, 22, 22, 25, 26, 0,
	0, 0, 0, 0, 0, 0
};
const BYTE L5BTYPES[206] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 0, 0,
	0, 0, 0, 0, 0, 25, 26, 0, 28, 0,
	30, 31, 0, 0, 0, 0, 0, 0, 0, 0,
	40, 41, 42, 43, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 79,
	80, 0, 82, 0, 0, 0, 0, 0, 0, 79,
	0, 80, 0, 0, 79, 80, 0, 2, 2, 2,
	1, 1, 11, 25, 13, 13, 13, 1, 2, 1,
	2, 1, 2, 1, 2, 2, 2, 2, 12, 0,
	0, 11, 1, 11, 1, 13, 0, 0, 0, 0,
	0, 0, 0, 13, 13, 13, 13, 13, 13, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0
};
const BYTE STAIRSUP[] = { 4, 4, 13, 13, 13, 13, 2, 2, 2, 2, 13, 13, 13, 13, 13, 13, 13, 13, 0, 66, 6, 0, 63, 64, 65, 0, 0, 67, 68, 0, 0, 0, 0, 0 };
const BYTE L5STAIRSUP[] = { 4, 4, 22, 22, 22, 22, 2, 2, 2, 2, 13, 13, 13, 13, 13, 13, 13, 13, 0, 66, 23, 0, 63, 64, 65, 0, 0, 67, 68, 0, 0, 0, 0, 0 };
const BYTE STAIRSDOWN[] = { 4, 3, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 62, 57, 58, 0, 61, 59, 60, 0, 0, 0, 0, 0 };
const BYTE LAMPS[] = { 2, 2, 13, 0, 13, 13, 129, 0, 130, 128 };
const BYTE PWATERIN[] = { 6, 6, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 0, 0, 0, 0, 0, 0, 202, 200, 200, 84, 0, 0, 199, 203, 203, 83, 0, 0, 85, 206, 80, 81, 0, 0, 0, 134, 135, 0, 0, 0, 0, 0, 0, 0, 0 };

/* data */
BYTE L5ConvTbl[16] = { 22, 13, 1, 13, 2, 13, 13, 13, 4, 13, 1, 13, 2, 13, 16, 13 };

void DRLG_Init_Globals()
{
	char c;

	memset(dFlags, 0, sizeof(dFlags));
	memset(dPlayer, 0, sizeof(dPlayer));
	memset(dMonster, 0, sizeof(dMonster));
	memset(dDead, 0, sizeof(dDead));
	memset(dObject, 0, sizeof(dObject));
	memset(dItem, 0, sizeof(dItem));
	memset(dMissile, 0, sizeof(dMissile));
	memset(dArch, 0, sizeof(dArch));
	if (!lightflag)
		c = (light4flag) ? 3 : 15;
	else
		c = 0;
	memset(dLight, c, sizeof(dLight));
}

void LoadL1Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	DRLG_InitTrans();
	pLevelMap = LoadFileInMem(sFileName, NULL);

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 22;
			L5dflags[i][j] = 0;
		}
	}

	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
				L5dflags[i][j] |= DLRG_PROTECTED;
			} else {
				dungeon[i][j] = 13;
			}
			lm += 2;
		}
	}

	DRLG_L1Floor();
	ViewX = vx;
	ViewY = vy;
	DRLG_L1Pass3();
	DRLG_Init_Globals();
	DRLG_InitL1Vals();
	SetMapMonsters(pLevelMap, 0, 0);
	SetMapObjects(pLevelMap, 0, 0);
	mem_free_dbg(pLevelMap);
}

void DRLG_L1Floor()
{
	int i, j;
	LONG rv;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (L5dflags[i][j] == 0 && dungeon[i][j] == 13) {
				rv = random(0, 3);

				if (rv == 1)
					dungeon[i][j] = 162;
				if (rv == 2)
					dungeon[i][j] = 163;
			}
		}
	}
}

void DRLG_L1Pass3()
{
	int i, j, xx, yy;
	long v1, v2, v3, v4, lv;

	lv = 22 - 1;

#ifdef USE_ASM
	__asm {
		mov		esi, pMegaTiles
		mov		eax, lv
		shl		eax, 3
		add		esi, eax
		xor		eax, eax
		lodsw
		inc		eax
		mov		v1, eax
		lodsw
		inc		eax
		mov		v2, eax
		lodsw
		inc		eax
		mov		v3, eax
		lodsw
		inc		eax
		mov		v4, eax
	}
#else
	v1 = *((WORD *)&pMegaTiles[lv * 8]) + 1;
	v2 = *((WORD *)&pMegaTiles[lv * 8] + 1) + 1;
	v3 = *((WORD *)&pMegaTiles[lv * 8] + 2) + 1;
	v4 = *((WORD *)&pMegaTiles[lv * 8] + 3) + 1;
#endif

	for (j = 0; j < MAXDUNY; j += 2)
	{
		for (i = 0; i < MAXDUNX; i += 2) {
			dPiece[i][j] = v1;
			dPiece[i + 1][j] = v2;
			dPiece[i][j + 1] = v3;
			dPiece[i + 1][j + 1] = v4;
		}
	}

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			lv = dungeon[i][j] - 1;
			/// ASSERT: assert(lv >= 0);
#ifdef USE_ASM
			__asm {
				mov		esi, pMegaTiles
				mov		eax, lv
				shl		eax, 3
				add		esi, eax
				xor		eax, eax
				lodsw
				inc		eax
				mov		v1, eax
				lodsw
				inc		eax
				mov		v2, eax
				lodsw
				inc		eax
				mov		v3, eax
				lodsw
				inc		eax
				mov		v4, eax
			}
#else
			v1 = *((WORD *)&pMegaTiles[lv * 8]) + 1;
			v2 = *((WORD *)&pMegaTiles[lv * 8] + 1) + 1;
			v3 = *((WORD *)&pMegaTiles[lv * 8] + 2) + 1;
			v4 = *((WORD *)&pMegaTiles[lv * 8] + 3) + 1;
#endif
			dPiece[xx][yy] = v1;
			dPiece[xx + 1][yy] = v2;
			dPiece[xx][yy + 1] = v3;
			dPiece[xx + 1][yy + 1] = v4;
			xx += 2;
		}
		yy += 2;
	}
}

void DRLG_InitL1Vals()
{
	int i, j, pc;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 12) {
				pc = 1;
			} else if (dPiece[i][j] == 11) {
				pc = 2;
			} else if (dPiece[i][j] == 71) {
				pc = 1;
			} else if (dPiece[i][j] == 259) {
				pc = 5;
			} else if (dPiece[i][j] == 249) {
				pc = 2;
			} else if (dPiece[i][j] == 325) {
				pc = 2;
			} else if (dPiece[i][j] == 321) {
				pc = 1;
			} else if (dPiece[i][j] == 255) {
				pc = 4;
			} else if (dPiece[i][j] == 211) {
				pc = 1;
			} else if (dPiece[i][j] == 344) {
				pc = 2;
			} else if (dPiece[i][j] == 341) {
				pc = 1;
			} else if (dPiece[i][j] == 331) {
				pc = 2;
			} else if (dPiece[i][j] == 418) {
				pc = 1;
			} else if (dPiece[i][j] == 421) {
				pc = 2;
			} else {
				continue;
			}
			dArch[i][j] = pc;
		}
	}
}

void LoadPreL1Dungeon(char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	pLevelMap = LoadFileInMem(sFileName, NULL);

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 22;
			L5dflags[i][j] = 0;
		}
	}

	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
				L5dflags[i][j] |= DLRG_PROTECTED;
			} else {
				dungeon[i][j] = 13;
			}
			lm += 2;
		}
	}

	DRLG_L1Floor();

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}

	mem_free_dbg(pLevelMap);
}

void CreateL5Dungeon(DWORD rseed, int entry)
{
	SetRndSeed(rseed);

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_LoadL1SP();
	DRLG_L5(entry);
	DRLG_L1Pass3();
	DRLG_FreeL1SP();
	DRLG_InitL1Vals();
	DRLG_SetPC();
}

void DRLG_LoadL1SP()
{
	setloadflag = FALSE;
	if (QuestStatus(QTYPE_BUTCH)) {
		pSetPiece = LoadFileInMem("Levels\\L1Data\\rnd6.DUN", NULL);
		setloadflag = TRUE;
	}
	if (QuestStatus(QTYPE_KING) && gbMaxPlayers == 1) {
		pSetPiece = LoadFileInMem("Levels\\L1Data\\SKngDO.DUN", NULL);
		setloadflag = TRUE;
	}
	if (QuestStatus(QTYPE_BOL)) {
		pSetPiece = LoadFileInMem("Levels\\L1Data\\Banner2.DUN", NULL);
		setloadflag = TRUE;
	}
}

void DRLG_FreeL1SP()
{
	MemFreeDbg(pSetPiece);
}

void DRLG_L5(int entry)
{
	int i, j;
	LONG minarea;
	BOOL doneflag;

	switch (currlevel) {
	case 1:
		minarea = 533;
		break;
	case 2:
		minarea = 693;
		break;
	case 3:
	case 4:
		minarea = 761;
		break;
	}

	do {
		DRLG_InitTrans();

		do {
			InitL5Dungeon();
			L5firstRoom();
		} while (L5GetArea() < minarea);

		L5makeDungeon();
		L5makeDmt();
		L5FillChambers();
		L5tileFix();
		L5AddWall();
		L5ClearFlags();
		DRLG_L5FloodTVal();

		doneflag = TRUE;

		if (QuestStatus(QTYPE_PW)) {
			if (entry == 0) {
				if (DRLG_PlaceMiniSet(PWATERIN, 1, 1, 0, 0, 1, -1, 0) < 0)
					doneflag = FALSE;
			} else {
				if (DRLG_PlaceMiniSet(PWATERIN, 1, 1, 0, 0, 0, -1, 0) < 0)
					doneflag = FALSE;
				ViewY--;
			}
		}
		if (QuestStatus(QTYPE_BOL)) {
			if (entry == 0) {
				if (DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 1, -1, 0) < 0)
					doneflag = FALSE;
			} else {
				if (DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 0, -1, 0) < 0)
					doneflag = FALSE;
				if (entry == 1) {
					ViewX = 2 * setpc_x + 20;
					ViewY = 2 * setpc_y + 28;
				} else {
					ViewY--;
				}
			}
		} else if (entry == 0) {
			if (DRLG_PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, 1, -1, 0) < 0)
				doneflag = FALSE;
			else if (DRLG_PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, 0, -1, 1) < 0)
				doneflag = FALSE;
		} else {
			if (DRLG_PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, 0, -1, 0) < 0)
				doneflag = FALSE;
			else if (DRLG_PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, 1, -1, 1) < 0)
				doneflag = FALSE;
			ViewY--;
		}
	} while (doneflag == FALSE);

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 64) {
				int xx = 2 * i + 16; /* todo: fix loop */
				int yy = 2 * j + 16;
				DRLG_CopyTrans(xx, yy + 1, xx, yy);
				DRLG_CopyTrans(xx + 1, yy + 1, xx + 1, yy);
			}
		}
	}

	DRLG_L5TransFix();
	DRLG_L5DirtFix();
	DRLG_L5CornerFix();

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (L5dflags[i][j] & 0x7F)
				DRLG_PlaceDoor(i, j);
		}
	}

	DRLG_L5Subs();
	DRLG_L1Shadows();
	DRLG_PlaceMiniSet(LAMPS, 5, 10, 0, 0, 0, -1, 4);
	DRLG_L1Floor();

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}

	DRLG_Init_Globals();
	DRLG_CheckQuests(setpc_x, setpc_y);
}

void DRLG_PlaceDoor(int x, int y)
{
	if ((L5dflags[x][y] & DLRG_PROTECTED) == 0) {
		BYTE df = L5dflags[x][y] & 0x7F;
		BYTE c = dungeon[x][y];

		if (df == 1) {
			if (y != 1 && c == 2)
				dungeon[x][y] = 26;
			if (y != 1 && c == 7)
				dungeon[x][y] = 31;
			if (y != 1 && c == 14)
				dungeon[x][y] = 42;
			if (y != 1 && c == 4)
				dungeon[x][y] = 43;
			if (x != 1 && c == 1)
				dungeon[x][y] = 25;
			if (x != 1 && c == 10)
				dungeon[x][y] = 40;
			if (x != 1 && c == 6)
				dungeon[x][y] = 30;
		}
		if (df == 2) {
			if (x != 1 && c == 1)
				dungeon[x][y] = 25;
			if (x != 1 && c == 6)
				dungeon[x][y] = 30;
			if (x != 1 && c == 10)
				dungeon[x][y] = 40;
			if (x != 1 && c == 4)
				dungeon[x][y] = 41;
			if (y != 1 && c == 2)
				dungeon[x][y] = 26;
			if (y != 1 && c == 14)
				dungeon[x][y] = 42;
			if (y != 1 && c == 7)
				dungeon[x][y] = 31;
		}
		if (df == 3) {
			if (x != 1 && y != 1 && c == 4)
				dungeon[x][y] = 28;
			if (x != 1 && c == 10)
				dungeon[x][y] = 40;
			if (y != 1 && c == 14)
				dungeon[x][y] = 42;
			if (y != 1 && c == 2)
				dungeon[x][y] = 26;
			if (x != 1 && c == 1)
				dungeon[x][y] = 25;
			if (y != 1 && c == 7)
				dungeon[x][y] = 31;
			if (x != 1 && c == 6)
				dungeon[x][y] = 30;
		}
	}

	L5dflags[x][y] = DLRG_PROTECTED;
}

void DRLG_L1Shadows()
{
	int x, y, i;
	BYTE sd[2][2];
	BYTE tnv3;
	BOOL patflag;

	for (y = 1; y < DMAXY; y++) {
		for (x = 1; x < DMAXX; x++) {
			sd[0][0] = BSTYPES[dungeon[x][y]];
			sd[1][0] = BSTYPES[dungeon[x - 1][y]];
			sd[0][1] = BSTYPES[dungeon[x][y - 1]];
			sd[1][1] = BSTYPES[dungeon[x - 1][y - 1]];

			for (i = 0; i < 37; i++) {
				if (SPATS[i].strig == sd[0][0]) {
					patflag = TRUE;
					if (SPATS[i].s1 && SPATS[i].s1 != sd[1][1])
						patflag = FALSE;
					if (SPATS[i].s2 && SPATS[i].s2 != sd[0][1])
						patflag = FALSE;
					if (SPATS[i].s3 && SPATS[i].s3 != sd[1][0])
						patflag = FALSE;
					if (patflag == TRUE) {
						if (SPATS[i].nv1 && !L5dflags[x - 1][y - 1])
							dungeon[x - 1][y - 1] = SPATS[i].nv1;
						if (SPATS[i].nv2 && !L5dflags[x][y - 1])
							dungeon[x][y - 1] = SPATS[i].nv2;
						if (SPATS[i].nv3) {
							if (!L5dflags[x - 1][y])
								dungeon[x - 1][y] = SPATS[i].nv3;
						}
					}
				}
			}
		}
	}

	for (y = 1; y < DMAXY; y++) {
		for (x = 1; x < DMAXX; x++) {
			if (dungeon[x - 1][y] == 139 && !L5dflags[x - 1][y]) {
				tnv3 = 139;
				if (dungeon[x][y] == 29)
					tnv3 = 141;
				if (dungeon[x][y] == 32)
					tnv3 = 141;
				if (dungeon[x][y] == 35)
					tnv3 = 141;
				if (dungeon[x][y] == 37)
					tnv3 = 141;
				if (dungeon[x][y] == 38)
					tnv3 = 141;
				if (dungeon[x][y] == 39)
					tnv3 = 141;
				dungeon[x - 1][y] = tnv3;
			}
			if (dungeon[x - 1][y] == 149 && !L5dflags[x - 1][y]) {
				tnv3 = 149;
				if (dungeon[x][y] == 29)
					tnv3 = 153;
				if (dungeon[x][y] == 32)
					tnv3 = 153;
				if (dungeon[x][y] == 35)
					tnv3 = 153;
				if (dungeon[x][y] == 37)
					tnv3 = 153;
				if (dungeon[x][y] == 38)
					tnv3 = 153;
				if (dungeon[x][y] == 39)
					tnv3 = 153;
				dungeon[x - 1][y] = tnv3;
			}
			if (dungeon[x - 1][y] == 148 && !L5dflags[x - 1][y]) {
				tnv3 = 148;
				if (dungeon[x][y] == 29)
					tnv3 = 154;
				if (dungeon[x][y] == 32)
					tnv3 = 154;
				if (dungeon[x][y] == 35)
					tnv3 = 154;
				if (dungeon[x][y] == 37)
					tnv3 = 154;
				if (dungeon[x][y] == 38)
					tnv3 = 154;
				if (dungeon[x][y] == 39)
					tnv3 = 154;
				dungeon[x - 1][y] = tnv3;
			}
		}
	}
}

int DRLG_PlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, BOOL setview, int noquad, int ldir)
{
	int xx, yy, sx, sy;
	int ii, i, t, found;
	int numt;
	BOOL abort;

	int sw = miniset[0];
	int sh = miniset[1];

	if (tmax - tmin == 0)
		numt = 1;
	else
		numt = random(0, tmax - tmin) + tmin;

	for (i = 0; i < numt; i++) {
		sx = random(0, DMAXX - sw);
		sy = random(0, DMAXY - sh);
		abort = FALSE;
		found = 0;

		while (abort == FALSE) {
			abort = TRUE;
			if (cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx++;
				abort = FALSE;
			}
			if (cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sy++;
				abort = FALSE;
			}

			switch (noquad) {
			case 0:
				if (sx < cx && sy < cy)
					abort = FALSE;
				break;
			case 1:
				if (sx > cx && sy < cy)
					abort = FALSE;
				break;
			case 2:
				if (sx < cx && sy > cy)
					abort = FALSE;
				break;
			case 3:
				if (sx > cx && sy > cy)
					abort = FALSE;
				break;
			}

			ii = 2;

			for (yy = 0; yy < sh && abort == TRUE; yy++) {
				for (xx = 0; xx < sw && abort == TRUE; xx++) {
					if (miniset[ii] && dungeon[xx + sx][sy + yy] != miniset[ii])
						abort = FALSE;
					if (L5dflags[xx + sx][sy + yy])
						abort = FALSE;
					ii++;
				}
			}

			if (abort == FALSE) {
				if (++sx == DMAXX - sw) {
					sx = 0;
					if (++sy == DMAXY - sh)
						sy = 0;
				}
				if (++found > 4000)
					return -1;
			}
		}

		ii = sw * sh + 2;

		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii])
					dungeon[xx + sx][sy + yy] = miniset[ii];
				ii++;
			}
		}
	}

	if (miniset == PWATERIN) {
		t = TransVal;
		TransVal = 0;
		DRLG_MRectTrans(sx, sy + 2, sx + 5, sy + 4);
		TransVal = t;

		quests[QTYPE_PW]._qtx = 2 * sx + 21;
		quests[QTYPE_PW]._qty = 2 * sy + 22;
	}

	if (setview == TRUE) {
		ViewX = 2 * sx + 19;
		ViewY = 2 * sy + 20;
	}

	if (ldir == 0) {
		LvlViewX = 2 * sx + 19;
		LvlViewY = 2 * sy + 20;
	}

	if (sx < cx && sy < cy)
		return 0;
	if (sx > cx && sy < cy)
		return 1;
	if (sx < cx && sy > cy)
		return 2;
	else
		return 3;
}

void InitL5Dungeon()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 0;
			L5dflags[i][j] = 0;
		}
	}
}

void L5ClearFlags()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			L5dflags[i][j] &= 0xBF;
		}
	}
}

void L5firstRoom()
{
	int ys, ye, y;
	int xs, xe, x;

	if (random(0, 2) == 0) {
		ys = 1;
		ye = DMAXY - 1;

		VR1 = random(0, 2);
		VR2 = random(0, 2);
		VR3 = random(0, 2);

		if (VR1 + VR3 <= 1)
			VR2 = 1;
		if (VR1)
			L5drawRoom(15, 1, 10, 10);
		else
			ys = 18;

		if (VR2)
			L5drawRoom(15, 15, 10, 10);
		if (VR3)
			L5drawRoom(15, 29, 10, 10);
		else
			ye = 22;

		for (y = ys; y < ye; y++) {
			dungeon[17][y] = 1;
			dungeon[18][y] = 1;
			dungeon[19][y] = 1;
			dungeon[20][y] = 1;
			dungeon[21][y] = 1;
			dungeon[22][y] = 1;
		}

		if (VR1)
			L5roomGen(15, 1, 10, 10, 0);
		if (VR2)
			L5roomGen(15, 15, 10, 10, 0);
		if (VR3)
			L5roomGen(15, 29, 10, 10, 0);

		HR3 = 0;
		HR2 = 0;
		HR1 = 0;
	} else {
		xs = 1;
		xe = DMAXX - 1;

		HR1 = random(0, 2);
		HR2 = random(0, 2);
		HR3 = random(0, 2);

		if (HR1 + HR3 <= 1)
			HR2 = 1;
		if (HR1)
			L5drawRoom(1, 15, 10, 10);
		else
			xs = 18;

		if (HR2)
			L5drawRoom(15, 15, 10, 10);
		if (HR3)
			L5drawRoom(29, 15, 10, 10);
		else
			xe = 22;

		for (x = xs; x < xe; x++) {
			dungeon[x][17] = 1;
			dungeon[x][18] = 1;
			dungeon[x][19] = 1;
			dungeon[x][20] = 1;
			dungeon[x][21] = 1;
			dungeon[x][22] = 1;
		}

		if (HR1)
			L5roomGen(1, 15, 10, 10, 1);
		if (HR2)
			L5roomGen(15, 15, 10, 10, 1);
		if (HR3)
			L5roomGen(29, 15, 10, 10, 1);

		VR3 = 0;
		VR2 = 0;
		VR1 = 0;
	}
}

void L5drawRoom(int x, int y, int w, int h)
{
	int i, j;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			dungeon[x + i][y + j] = 1;
		}
	}
}

void L5roomGen(int x, int y, int w, int h, int dir)
{
	int num, dirProb;
	BOOL ran, ran2;
	int width, height, rx, ry, ry2;
	int cw, ch, cx1, cy1, cx2;

	dirProb = random(0, 4);

	switch (dir == 1 ? dirProb != 0 : dirProb == 0) {
	case FALSE:
		num = 0;
		do {
			cw = (random(0, 5) + 2) & 0xFFFFFFFE;
			ch = (random(0, 5) + 2) & 0xFFFFFFFE;
			cy1 = h / 2 + y - ch / 2;
			cx1 = x - cw;
			ran = L5checkRoom(cx1 - 1, cy1 - 1, ch + 2, cw + 1); /// BUGFIX: swap args 3 and 4 ("ch+2" and "cw+1")
			num++;
		} while (ran == FALSE && num < 20);

		if (ran == TRUE)
			L5drawRoom(cx1, cy1, cw, ch);
		cx2 = x + w;
		ran2 = L5checkRoom(cx2, cy1 - 1, cw + 1, ch + 2);
		if (ran2 == TRUE)
			L5drawRoom(cx2, cy1, cw, ch);
		if (ran == TRUE)
			L5roomGen(cx1, cy1, cw, ch, 1);
		if (ran2 == TRUE)
			L5roomGen(cx2, cy1, cw, ch, 1);
		break;
	case TRUE:
		num = 0;
		do {
			width = (random(0, 5) + 2) & 0xFFFFFFFE;
			height = (random(0, 5) + 2) & 0xFFFFFFFE;
			rx = w / 2 + x - width / 2;
			ry = y - height;
			ran = L5checkRoom(rx - 1, ry - 1, width + 2, height + 1);
			num++;
		} while (ran == FALSE && num < 20);

		if (ran == TRUE)
			L5drawRoom(rx, ry, width, height);
		ry2 = y + h;
		ran2 = L5checkRoom(rx - 1, ry2, width + 2, height + 1);
		if (ran2 == TRUE)
			L5drawRoom(rx, ry2, width, height);
		if (ran == TRUE)
			L5roomGen(rx, ry, width, height, 0);
		if (ran2 == TRUE)
			L5roomGen(rx, ry2, width, height, 0);
		break;
	}
}

BOOL L5checkRoom(int x, int y, int width, int height)
{
	int i, j;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (i + x < 0 || i + x >= DMAXX || j + y < 0 || j + y >= DMAXY)
				return FALSE;
			if (dungeon[i + x][j + y])
				return FALSE;
		}
	}

	return TRUE;
}

int L5GetArea()
{
	int i, j;
	int rv;

	rv = 0;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 1)
				rv++;
		}
	}

	return rv;
}

void L5makeDungeon()
{
	int i, j;
	int i_2, j_2;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			j_2 = j << 1;
			i_2 = i << 1;
			L5dungeon[i_2][j_2] = dungeon[i][j];
			L5dungeon[i_2][j_2 + 1] = dungeon[i][j];
			L5dungeon[i_2 + 1][j_2] = dungeon[i][j];
			L5dungeon[i_2 + 1][j_2 + 1] = dungeon[i][j];
		}
	}
}

void L5makeDmt()
{
	int i, j, dmtx, dmty;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 22;
		}
	}

	for (j = 0, dmty = 1; dmty <= 77; j++, dmty += 2) {
		for (i = 0, dmtx = 1; dmtx <= 77; i++, dmtx += 2) {
			int val = L5dungeon[dmtx + 1][dmty + 1];
			val = 2 * val + L5dungeon[dmtx][dmty + 1];
			val = 2 * val + L5dungeon[dmtx + 1][dmty];
			val = 2 * val + L5dungeon[dmtx][dmty];
			dungeon[i][j] = L5ConvTbl[val];
		}
	}
}

void L5AddWall()
{
	int i, j, x, y;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (!L5dflags[i][j]) {
				if (dungeon[i][j] == 3 && random(0, 100) < 100) {
					x = L5HWallOk(i, j);
					if (x != -1)
						L5HorizWall(i, j, 2, x);
				}
				if (dungeon[i][j] == 3 && random(0, 100) < 100) {
					y = L5VWallOk(i, j);
					if (y != -1)
						L5VertWall(i, j, 1, y);
				}
				if (dungeon[i][j] == 6 && random(0, 100) < 100) {
					x = L5HWallOk(i, j);
					if (x != -1)
						L5HorizWall(i, j, 4, x);
				}
				if (dungeon[i][j] == 7 && random(0, 100) < 100) {
					y = L5VWallOk(i, j);
					if (y != -1)
						L5VertWall(i, j, 4, y);
				}
				if (dungeon[i][j] == 2 && random(0, 100) < 100) {
					x = L5HWallOk(i, j);
					if (x != -1)
						L5HorizWall(i, j, 2, x);
				}
				if (dungeon[i][j] == 1 && random(0, 100) < 100) {
					y = L5VWallOk(i, j);
					if (y != -1)
						L5VertWall(i, j, 1, y);
				}
			}
		}
	}
}

int L5HWallOk(int i, int j)
{
	int x;
	BOOL wallok;

	for (x = 1; dungeon[i + x][j] == 13; x++) {
		if (dungeon[i + x][j - 1] != 13 || dungeon[i + x][j + 1] != 13 || L5dflags[i + x][j])
			break;
	}

	wallok = FALSE;
	if (dungeon[i + x][j] >= 3 && dungeon[i + x][j] <= 7)
		wallok = TRUE;
	if (dungeon[i + x][j] >= 16 && dungeon[i + x][j] <= 24)
		wallok = TRUE;
	if (dungeon[i + x][j] == 22)
		wallok = FALSE;
	if (x == 1)
		wallok = FALSE;

	if (wallok)
		return x;
	else
		return -1;
}

int L5VWallOk(int i, int j)
{
	int y;
	BOOL wallok;

	for (y = 1; dungeon[i][j + y] == 13; y++) {
		if (dungeon[i - 1][j + y] != 13 || dungeon[i + 1][j + y] != 13 || L5dflags[i][j + y])
			break;
	}

	wallok = FALSE;
	if (dungeon[i][j + y] >= 3 && dungeon[i][j + y] <= 7)
		wallok = TRUE;
	if (dungeon[i][j + y] >= 16 && dungeon[i][j + y] <= 24)
		wallok = TRUE;
	if (dungeon[i][j + y] == 22)
		wallok = FALSE;
	if (y == 1)
		wallok = FALSE;

	if (wallok)
		return y;
	else
		return -1;
}

void L5HorizWall(int i, int j, char p, int dx)
{
	int xx;
	char wt, dt;

	switch (random(0, 4)) {
	case 0:
	case 1:
		dt = 2;
		break;
	case 2:
		dt = 12;
		if (p == 2)
			p = 12;
		if (p == 4)
			p = 10;
		break;
	case 3:
		dt = 36;
		if (p == 2)
			p = 36;
		if (p == 4)
			p = 27;
		break;
	}

	if (random(0, 6) == 5)
		wt = 12;
	else
		wt = 26;
	if (dt == 12)
		wt = 12;

	dungeon[i][j] = p;

	for (xx = 1; xx < dx; xx++) {
		dungeon[i + xx][j] = dt;
	}

	xx = random(0, dx - 1) + 1;

	if (wt == 12) {
		dungeon[i + xx][j] = wt;
	} else {
		dungeon[i + xx][j] = 2;
		L5dflags[i + xx][j] |= DLRG_HDOOR;
	}
}

void L5VertWall(int i, int j, char p, int dy)
{
	int yy;
	char wt, dt;

	switch (random(0, 4)) {
	case 0:
	case 1:
		dt = 1;
		break;
	case 2:
		dt = 11;
		if (p == 1)
			p = 11;
		if (p == 4)
			p = 14;
		break;
	case 3:
		dt = 35;
		if (p == 1)
			p = 35;
		if (p == 4)
			p = 37;
		break;
	}

	if (random(0, 6) == 5)
		wt = 11;
	else
		wt = 25;
	if (dt == 11)
		wt = 11;

	dungeon[i][j] = p;

	for (yy = 1; yy < dy; yy++) {
		dungeon[i][j + yy] = dt;
	}

	yy = random(0, dy - 1) + 1;

	if (wt == 11) {
		dungeon[i][j + yy] = wt;
	} else {
		dungeon[i][j + yy] = 1;
		L5dflags[i][j + yy] |= DLRG_VDOOR;
	}
}

void L5tileFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 22)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 22)
				dungeon[i + 1][j] = 18;
			if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 2)
				dungeon[i + 1][j] = 7;
			if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 22)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 22)
				dungeon[i][j + 1] = 24;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 1)
				dungeon[i][j + 1] = 6;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 22)
				dungeon[i][j + 1] = 19;
		}
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 19)
				dungeon[i + 1][j] = 21;
			if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 22)
				dungeon[i + 1][j] = 20;
			if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 22)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 24)
				dungeon[i + 1][j] = 21;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 22)
				dungeon[i + 1][j] = 20;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 19)
				dungeon[i + 1][j] = 21;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 6;
			if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 19)
				dungeon[i + 1][j] = 21;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 6;
			if (dungeon[i][j] == 3 && dungeon[i + 1][j] == 22)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 6;
			if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 6;
			if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 24)
				dungeon[i + 1][j] = 21;
			if (dungeon[i][j] == 4 && dungeon[i + 1][j] == 16)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 13)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 24)
				dungeon[i + 1][j] = 21;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 13)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 23 && dungeon[i - 1][j] == 22)
				dungeon[i - 1][j] = 19;
			if (dungeon[i][j] == 19 && dungeon[i - 1][j] == 23)
				dungeon[i - 1][j] = 21;
			if (dungeon[i][j] == 6 && dungeon[i - 1][j] == 22)
				dungeon[i - 1][j] = 24;
			if (dungeon[i][j] == 6 && dungeon[i - 1][j] == 23)
				dungeon[i - 1][j] = 21;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 7;
			if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 18)
				dungeon[i][j + 1] = 21;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 7;
			if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 7;
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 7;
			if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 22)
				dungeon[i][j + 1] = 24;
			if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 13)
				dungeon[i][j + 1] = 16;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 13)
				dungeon[i][j + 1] = 16;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 16)
				dungeon[i][j + 1] = 17;
			if (dungeon[i][j] == 6 && dungeon[i][j - 1] == 22)
				dungeon[i][j - 1] = 7;
			if (dungeon[i][j] == 6 && dungeon[i][j - 1] == 22)
				dungeon[i][j - 1] = 24;
			if (dungeon[i][j] == 7 && dungeon[i][j - 1] == 24)
				dungeon[i][j - 1] = 21;
			if (dungeon[i][j] == 18 && dungeon[i][j - 1] == 24)
				dungeon[i][j - 1] = 21;
		}
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 4 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 7;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 19)
				dungeon[i + 1][j] = 21;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 22)
				dungeon[i][j + 1] = 20;
		}
	}
}

void DRLG_L5Subs()
{
	int x, y, rv, i;

	for (y = 0; y < DMAXY; y++) {
		for (x = 0; x < DMAXX; x++) {
			if (!random(0, 4)) {
				BYTE c = L5BTYPES[dungeon[x][y]];

				if (c && !L5dflags[x][y]) {
					rv = random(0, 16);
					i = -1;

					while (rv >= 0) {
						if (++i == sizeof(L5BTYPES))
							i = 0;
						if (c == L5BTYPES[i])
							rv--;
					}

					if (i == 89) {
						if (L5BTYPES[dungeon[x][y - 1]] != 79 || L5dflags[x][y - 1])
							i = 79;
						else
							dungeon[x][y - 1] = 90;
					}
					if (i == 91) {
						if (L5BTYPES[dungeon[x + 1][y]] != 80 || L5dflags[x + 1][y])
							i = 80;
						else
							dungeon[x + 1][y] = 92;
					}
					dungeon[x][y] = i;
				}
			}
		}
	}
}

void L5FillChambers()
{
	int c;

	if (HR1)
		DRLG_L5GChamber(0, 14, 0, 0, 0, 1);

	if (HR2) {
		if (HR1 && !HR3)
			DRLG_L5GChamber(14, 14, 0, 0, 1, 0);
		if (!HR1 && HR3)
			DRLG_L5GChamber(14, 14, 0, 0, 0, 1);
		if (HR1 && HR3)
			DRLG_L5GChamber(14, 14, 0, 0, 1, 1);
		if (!HR1 && !HR3)
			DRLG_L5GChamber(14, 14, 0, 0, 0, 0);
	}

	if (HR3)
		DRLG_L5GChamber(28, 14, 0, 0, 1, 0);
	if (HR1 && HR2)
		DRLG_L5GHall(12, 18, 14, 18);
	if (HR2 && HR3)
		DRLG_L5GHall(26, 18, 28, 18);
	if (HR1 && !HR2 && HR3)
		DRLG_L5GHall(12, 18, 28, 18);
	if (VR1)
		DRLG_L5GChamber(14, 0, 0, 1, 0, 0);

	if (VR2) {
		if (VR1 && !VR3)
			DRLG_L5GChamber(14, 14, 1, 0, 0, 0);
		if (!VR1 && VR3)
			DRLG_L5GChamber(14, 14, 0, 1, 0, 0);
		if (VR1 && VR3)
			DRLG_L5GChamber(14, 14, 1, 1, 0, 0);
		if (!VR1 && !VR3)
			DRLG_L5GChamber(14, 14, 0, 0, 0, 0);
	}

	if (VR3)
		DRLG_L5GChamber(14, 28, 1, 0, 0, 0);
	if (VR1 && VR2)
		DRLG_L5GHall(18, 12, 18, 14);
	if (VR2 && VR3)
		DRLG_L5GHall(18, 26, 18, 28);
	if (VR1 && !VR2 && VR3)
		DRLG_L5GHall(18, 12, 18, 28);

	if (setloadflag) {
		if (VR1 || VR2 || VR3) {
			c = 1;
			if (!VR1 && VR2 && VR3 && random(0, 2))
				c = 2;
			if (VR1 && VR2 && !VR3 && random(0, 2))
				c = 0;

			if (VR1 && !VR2 && VR3) {
				if (random(0, 2))
					c = 0;
				else
					c = 2;
			}

			if (VR1 && VR2 && VR3)
				c = random(0, 3);

			switch (c) {
			case 0:
				DRLG_L5SetRoom(16, 2);
				break;
			case 1:
				DRLG_L5SetRoom(16, 16);
				break;
			case 2:
				DRLG_L5SetRoom(16, 30);
				break;
			}
		} else {
			c = 1;
			if (!HR1 && HR2 && HR3 && random(0, 2))
				c = 2;
			if (HR1 && HR2 && !HR3 && random(0, 2))
				c = 0;

			if (HR1 && !HR2 && HR3) {
				if (random(0, 2))
					c = 0;
				else
					c = 2;
			}

			if (HR1 && HR2 && HR3)
				c = random(0, 3);

			switch (c) {
			case 0:
				DRLG_L5SetRoom(2, 16);
				break;
			case 1:
				DRLG_L5SetRoom(16, 16);
				break;
			case 2:
				DRLG_L5SetRoom(30, 16);
				break;
			}
		}
	}
}

void DRLG_L5GChamber(int sx, int sy, BOOL topflag, BOOL bottomflag, BOOL leftflag, BOOL rightflag)
{
	int i, j;

	if (topflag == TRUE) {
		dungeon[sx + 2][sy] = 12;
		dungeon[sx + 3][sy] = 12;
		dungeon[sx + 4][sy] = 3;
		dungeon[sx + 7][sy] = 9;
		dungeon[sx + 8][sy] = 12;
		dungeon[sx + 9][sy] = 2;
	}
	if (bottomflag == TRUE) {
		sy += 11;
		dungeon[sx + 2][sy] = 10;
		dungeon[sx + 3][sy] = 12;
		dungeon[sx + 4][sy] = 8;
		dungeon[sx + 7][sy] = 5;
		dungeon[sx + 8][sy] = 12;
		if (dungeon[sx + 9][sy] != 4) {
			dungeon[sx + 9][sy] = 21;
		}
		sy -= 11;
	}
	if (leftflag == TRUE) {
		dungeon[sx][sy + 2] = 11;
		dungeon[sx][sy + 3] = 11;
		dungeon[sx][sy + 4] = 3;
		dungeon[sx][sy + 7] = 8;
		dungeon[sx][sy + 8] = 11;
		dungeon[sx][sy + 9] = 1;
	}
	if (rightflag == TRUE) {
		sx += 11;
		dungeon[sx][sy + 2] = 14;
		dungeon[sx][sy + 3] = 11;
		dungeon[sx][sy + 4] = 9;
		dungeon[sx][sy + 7] = 5;
		dungeon[sx][sy + 8] = 11;
		if (dungeon[sx][sy + 9] != 4) {
			dungeon[sx][sy + 9] = 21;
		}
		sx -= 11;
	}

	for (j = 1; j < 11; j++) {
		for (i = 1; i < 11; i++) {
			dungeon[i + sx][j + sy] = 13;
			L5dflags[i + sx][j + sy] |= DLRG_CHAMBER;
		}
	}

	dungeon[sx + 4][sy + 4] = 15;
	dungeon[sx + 7][sy + 4] = 15;
	dungeon[sx + 4][sy + 7] = 15;
	dungeon[sx + 7][sy + 7] = 15;
}

void DRLG_L5GHall(int x1, int y1, int x2, int y2)
{
	int i;

	if (y1 == y2) {
		for (i = x1; i < x2; i++) {
			dungeon[i][y1] = 12;
			dungeon[i][y1 + 3] = 12;
		}
	} else {
		for (i = y1; i < y2; i++) {
			dungeon[x1][i] = 11;
			dungeon[x1 + 3][i] = 11;
		}
	}
}

void DRLG_L5SetRoom(int rx1, int ry1)
{
	int rw, rh, i, j;
	BYTE *sp;

	rw = *pSetPiece;
	rh = *(pSetPiece + 2);

	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = rw;
	setpc_h = rh;

	sp = pSetPiece + 4;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp) {
				dungeon[rx1 + i][ry1 + j] = *sp;
				L5dflags[rx1 + i][ry1 + j] |= DLRG_PROTECTED;
			} else {
				dungeon[rx1 + i][ry1 + j] = 13;
			}
			sp += 2;
		}
	}
}

void DRLG_L5FloodTVal()
{
	int xx, yy, i, j;

	yy = 16;

	for (j = 0; j < DMAXY; j++) {
		xx = 16;

		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 13 && !dTransVal[xx][yy]) {
				DRLG_L5FTVR(i, j, xx, yy, 0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

void DRLG_L5FTVR(int i, int j, int x, int y, int d)
{
	if (dTransVal[x][y] || dungeon[i][j] != 13) {
		if (d == 1) {
			dTransVal[x][y] = TransVal;
			dTransVal[x][y + 1] = TransVal;
		}
		if (d == 2) {
			dTransVal[x + 1][y] = TransVal;
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if (d == 3) {
			dTransVal[x][y] = TransVal;
			dTransVal[x + 1][y] = TransVal;
		}
		if (d == 4) {
			dTransVal[x][y + 1] = TransVal;
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if (d == 5)
			dTransVal[x + 1][y + 1] = TransVal;
		if (d == 6)
			dTransVal[x][y + 1] = TransVal;
		if (d == 7)
			dTransVal[x + 1][y] = TransVal;
		if (d == 8)
			dTransVal[x][y] = TransVal;
	} else {
		dTransVal[x][y] = TransVal;
		dTransVal[x + 1][y] = TransVal;
		dTransVal[x][y + 1] = TransVal;
		dTransVal[x + 1][y + 1] = TransVal;
		DRLG_L5FTVR(i + 1, j, x + 2, y, 1);
		DRLG_L5FTVR(i - 1, j, x - 2, y, 2);
		DRLG_L5FTVR(i, j + 1, x, y + 2, 3);
		DRLG_L5FTVR(i, j - 1, x, y - 2, 4);
		DRLG_L5FTVR(i - 1, j - 1, x - 2, y - 2, 5);
		DRLG_L5FTVR(i + 1, j - 1, x + 2, y - 2, 6);
		DRLG_L5FTVR(i - 1, j + 1, x - 2, y + 2, 7);
		DRLG_L5FTVR(i + 1, j + 1, x + 2, y + 2, 8);
	}
}

void DRLG_L5TransFix()
{
	int xx, yy, i, j;

	yy = 16;

	for (j = 0; j < DMAXY; j++) {
		xx = 16;

		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 23 && dungeon[i][j - 1] == 18) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 24 && dungeon[i + 1][j] == 19) {
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 18) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 19) {
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 20) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			xx += 2;
		}
		yy += 2;
	}
}

void DRLG_L5DirtFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] != 19)
				dungeon[i][j] = 202;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] != 19)
				dungeon[i][j] = 200;
			if (dungeon[i][j] == 24 && dungeon[i + 1][j] != 19)
				dungeon[i][j] = 205;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] != 18)
				dungeon[i][j] = 199;
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] != 18)
				dungeon[i][j] = 202;
			if (dungeon[i][j] == 23 && dungeon[i][j + 1] != 18)
				dungeon[i][j] = 204;
		}
	}
}

void DRLG_L5CornerFix()
{
	int i, j;

	for (j = 1; j < DMAXY - 1; j++) {
		for (i = 1; i < DMAXX - 1; i++) {
			if (!(L5dflags[i][j] & DLRG_PROTECTED) && dungeon[i][j] == 17 && dungeon[i - 1][j] == 13 && dungeon[i][j - 1] == 1) {
				dungeon[i][j] = 16;
				L5dflags[i][j - 1] &= DLRG_PROTECTED;
			}
			if (dungeon[i][j] == 202 && dungeon[i + 1][j] == 13 && dungeon[i][j + 1] == 1) {
				dungeon[i][j] = 8;
			}
		}
	}
}

DEVILUTION_END_NAMESPACE
