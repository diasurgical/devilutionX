#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

int diabquad1x; // weak
int diabquad1y; // weak
int diabquad3x; // idb
int diabquad3y; // idb
int diabquad2x; // idb
int diabquad2y; // idb
int diabquad4x; // idb
int diabquad4y; // idb
BOOL hallok[20];
int l4holdx; // weak
int l4holdy; // weak
int SP4x1;   // idb
int SP4x2;   // weak
int SP4y1;   // idb
int SP4y2;   // weak
unsigned char L4dungeon[80][80];
unsigned char dung[20][20];
//int dword_52A4DC; // weak

const unsigned char L4ConvTbl[16] = { 30u, 6u, 1u, 6u, 2u, 6u, 6u, 6u, 9u, 6u, 1u, 6u, 2u, 6u, 3u, 6u };
const unsigned char L4USTAIRS[42] = {
	4u,
	5u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	0u,
	0u,
	0u,
	0u,
	36u,
	38u,
	35u,
	0u,
	37u,
	34u,
	33u,
	32u,
	0u,
	0u,
	31u,
	0u,
	0u,
	0u,
	0u,
	0u
};
const unsigned char L4TWARP[42] = {
	4u,
	5u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	0u,
	0u,
	0u,
	0u,
	134u,
	136u,
	133u,
	0u,
	135u,
	132u,
	131u,
	130u,
	0u,
	0u,
	129u,
	0u,
	0u,
	0u,
	0u,
	0u
};
const unsigned char L4DSTAIRS[52] = {
	5u,
	5u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	45u,
	41u,
	0u,
	0u,
	44u,
	43u,
	40u,
	0u,
	0u,
	46u,
	42u,
	39u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u
};
const unsigned char L4PENTA[52] = {
	5u,
	5u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	98u,
	100u,
	103u,
	0u,
	0u,
	99u,
	102u,
	105u,
	0u,
	0u,
	101u,
	104u,
	106u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u
};
const unsigned char L4PENTA2[52] = {
	5u,
	5u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	107u,
	109u,
	112u,
	0u,
	0u,
	108u,
	111u,
	114u,
	0u,
	0u,
	110u,
	113u,
	115u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u
};
const unsigned char L4BTYPES[140] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
	6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 2, 1, 2, 1, 2, 1, 1, 2,
	2, 0, 0, 0, 0, 0, 0, 15, 16, 9,
	12, 4, 5, 7, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void DRLG_LoadL4SP()
{
	setloadflag_2 = 0;
	if (QuestStatus(QTYPE_WARLRD)) {
		pSetPiece_2 = (char *)LoadFileInMem("Levels\\L4Data\\Warlord.DUN", 0);
		setloadflag_2 = 1;
	}
	if (currlevel == 15 && gbMaxPlayers != 1) {
		pSetPiece_2 = (char *)LoadFileInMem("Levels\\L4Data\\Vile1.DUN", 0);
		setloadflag_2 = 1;
	}
}
// 5B50D8: using guessed type int setloadflag_2;
// 679660: using guessed type char gbMaxPlayers;

void DRLG_FreeL4SP()
{
	MemFreeDbg(pSetPiece_2);
}

void DRLG_L4SetSPRoom(int rx1, int ry1)
{
	int rw, rh, i, j;
	unsigned char *sp;

	rw = (unsigned char)pSetPiece_2[0];
	rh = (unsigned char)pSetPiece_2[2];

	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = rw;
	setpc_h = rh;

	sp = (unsigned char *)&pSetPiece_2[4];

	for(j = 0; j < rh; j++) {
		for(i = 0; i < rw; i++) {
			if(*sp != 0) {
				dungeon[i + rx1][j + ry1] = *sp;
				dflags[i + rx1][j + ry1] |= 0x80;
			} else {
				dungeon[i + rx1][j + ry1] = 6;
			}
			sp += 2;
		}
	}
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void L4SaveQuads()
{
	char *v0;      // esi
	char *v1;      // edx
	char *v2;      // edi
	char *v3;      // eax
	char *v4;      // ecx
	char *v5;      // ebx
	signed int v6; // [esp+Ch] [ebp-14h]
	signed int v7; // [esp+10h] [ebp-10h]
	char *v8;      // [esp+14h] [ebp-Ch]
	char *v9;      // [esp+18h] [ebp-8h]
	char *v10;     // [esp+1Ch] [ebp-4h]

	v0 = &dflags[39][l4holdy - 40 * l4holdx]; /* check */
	v1 = &dflags[39][-l4holdy + 39 + -40 * l4holdx];
	v9 = &dflags[l4holdx][l4holdy];
	v8 = &dflags[0][40 * l4holdx - l4holdy + 39];
	v6 = 14;
	do {
		v2 = v1;
		v10 = v8;
		v3 = v9;
		v4 = v0;
		v7 = 14;
		do {
			v5 = v10;
			v10 += 40;
			*v3 = 1;
			*v4 = 1;
			*v5 = 1;
			*v2 = 1;
			v4 -= 40;
			v2 -= 40;
			v3 += 40;
			--v7;
		} while (v7);
		++v9;
		--v8;
		--v1;
		++v0;
		--v6;
	} while (v6);
}
// 528A34: using guessed type int l4holdx;
// 528A38: using guessed type int l4holdy;

void DRLG_L4SetRoom(unsigned char *pSetPiece, int rx1, int ry1)
{
	int rw, rh, i, j;
	unsigned char *sp;

	rw = pSetPiece[0];
	rh = pSetPiece[2];
	sp = &pSetPiece[4];

	for(j = 0; j < rh; j++) {
		for(i = 0; i < rw; i++) {
			if(*sp != 0) {
				dungeon[i + rx1][j + ry1] = *sp;
				dflags[i + rx1][j + ry1] |= 0x80;
			} else {
				dungeon[i + rx1][j + ry1] = 6;
			}
			sp += 2;
		}
	}
}

void DRLG_LoadDiabQuads(BOOL preflag)
{
	BYTE *lpSetPiece;

	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab1.DUN", 0);
	diabquad1x = 4 + l4holdx;
	diabquad1y = 4 + l4holdy;
	DRLG_L4SetRoom(lpSetPiece, diabquad1x, diabquad1y);
	mem_free_dbg(lpSetPiece);

	if(preflag) {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2b.DUN", 0);
	} else {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", 0);
	}
	diabquad2x = 27 - l4holdx;
	diabquad2y = 1 + l4holdy;
	DRLG_L4SetRoom(lpSetPiece, diabquad2x, diabquad2y);
	mem_free_dbg(lpSetPiece);

	if(preflag) {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3b.DUN", 0);
	} else {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", 0);
	}
	diabquad3x = 1 + l4holdx;
	diabquad3y = 27 - l4holdy;
	DRLG_L4SetRoom(lpSetPiece, diabquad3x, diabquad3y);
	mem_free_dbg(lpSetPiece);

	if(preflag) {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4b.DUN", 0);
	} else {
		lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4a.DUN", 0);
	}
	diabquad4x = 28 - l4holdx;
	diabquad4y = 28 - l4holdy;
	DRLG_L4SetRoom(lpSetPiece, diabquad4x, diabquad4y);
	mem_free_dbg(lpSetPiece);
}
// 5289C4: using guessed type int diabquad1x;
// 5289C8: using guessed type int diabquad1y;
// 528A34: using guessed type int l4holdx;
// 528A38: using guessed type int l4holdy;

BOOL IsDURWall(char d)
{
	if(d == 25) {
		return TRUE;
	}
	if(d == 28) {
		return TRUE;
	}
	if(d == 23) {
		return TRUE;
	}

	return FALSE;
}

BOOL IsDLLWall(char dd)
{
	if(dd == 27) {
		return TRUE;
	}
	if(dd == 26) {
		return TRUE;
	}
	if(dd == 22) {
		return TRUE;
	}

	return FALSE;
}

void L4FixRim()
{
	int i, j;

	for(i = 0; i < 20; i++) {
		dung[i][0] = 0;
	}
	for(j = 0; j < 20; j++) {
		dung[0][j] = 0;
	}
}
// 52A4DC: using guessed type int dword_52A4DC;

void DRLG_L4GeneralFix()
{
	int i, j;

	for(j = 0; j < 39; j++) {
		for(i = 0; i < 39; i++) {
			if((dungeon[i][j] == 24 || dungeon[i][j] == 122) && dungeon[i + 1][j] == 2 && dungeon[i][j + 1] == 5) {
				dungeon[i][j] = 17;
			}
		}
	}
}

void CreateL4Dungeon(unsigned int rseed, int entry)
{
	SetRndSeed(rseed);

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	ViewX = 40;
	ViewY = 40;

	DRLG_InitSetPC();
	DRLG_LoadL4SP();
	DRLG_L4(entry);
	DRLG_L4Pass3();
	DRLG_FreeL4SP();
	DRLG_SetPC();
}
// 5CF328: using guessed type int dmaxx;
// 5CF32C: using guessed type int dmaxy;
// 5D2458: using guessed type int dminx;
// 5D245C: using guessed type int dminy;

void DRLG_L4(int entry)
{
	int i, j, spi, spj;
	BOOL doneflag;

	do {
		DRLG_InitTrans();
		do {
			InitL4Dungeon();
			L4firstRoom();
			L4FixRim();
		} while(GetArea() < 173);
		uShape();
		L4makeDungeon();
		L4makeDmt();
		L4tileFix();
		if(currlevel == 16) {
			L4SaveQuads();
		}
		if(QuestStatus(QTYPE_WARLRD) || currlevel == quests[QTYPE_VB]._qlevel && gbMaxPlayers != 1) {
			for(spi = SP4x1; spi < SP4x2; spi++) {
				for(spj = SP4y1; spj < SP4y2; spj++) {
					dflags[spi][spj] = 1;
				}
			}
		}
		L4AddWall();
		DRLG_L4FloodTVal();
		DRLG_L4TransFix();
		if(setloadflag_2) {
			DRLG_L4SetSPRoom(SP4x1, SP4y1);
		}
		if(currlevel == 16) {
			DRLG_LoadDiabQuads(TRUE);
		}
		if(QuestStatus(QTYPE_WARLRD)) {
			if(entry == 0) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, 0);
				if(doneflag && currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
				}
				ViewX++;
			} else if(entry == 1) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if(doneflag && currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
				}
				ViewX = 2 * setpc_x + 22;
				ViewY = 2 * setpc_y + 22;
			} else {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if(doneflag && currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 1, 6);
				}
				ViewX++;
			}
		} else if(currlevel != 15) {
			if(entry == 0) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, 0);
				if(doneflag && currlevel != 16) {
					doneflag = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 0, 1);
				}
				if(doneflag && currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
				}
				ViewX++;
			} else if(entry == 1) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if(doneflag && currlevel != 16) {
					doneflag = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 1, 1);
				}
				if(doneflag && currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
				}
				ViewY++;
			} else {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if(doneflag && currlevel != 16) {
					doneflag = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 0, 1);
				}
				if(doneflag && currlevel == 13) {
					doneflag = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 1, 6);
				}
				ViewX++;
			}
		} else {
			if(entry == 0) {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, 0);
				if(doneflag) {
					if(gbMaxPlayers == 1 && quests[QTYPE_MOD]._qactive != 2) {
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA, 1, 1, -1, -1, 0, 1);
					} else {
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA2, 1, 1, -1, -1, 0, 1);
					}
				}
				ViewX++;
			} else {
				doneflag = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if(doneflag) {
					if(gbMaxPlayers == 1 && quests[QTYPE_MOD]._qactive != 2) {
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA, 1, 1, -1, -1, 1, 1);
					} else {
						doneflag = DRLG_L4PlaceMiniSet(L4PENTA2, 1, 1, -1, -1, 1, 1);
					}
				}
				ViewY++;
			}
		}
	} while(!doneflag);

	DRLG_L4GeneralFix();

	if(currlevel != 16) {
		DRLG_PlaceThemeRooms(7, 10, 6, 8, 1);
	}

	DRLG_L4Shadows();
	DRLG_L4Corners();
	DRLG_L4Subs();
	DRLG_Init_Globals();

	if(QuestStatus(QTYPE_WARLRD)) {
		for(j = 0; j < DMAXY; j++) {
			for(i = 0; i < DMAXX; i++) {
				pdungeon[i][j] = dungeon[i][j];
			}
		}
	}

	DRLG_CheckQuests(SP4x1, SP4y1);

	if(currlevel == 15) {
		for(j = 0; j < DMAXY; j++) {
			for(i = 0; i < DMAXX; i++) {
				if(dungeon[i][j] == 98) {
					Make_SetPC(i - 1, j - 1, 5, 5);
				}
				if(dungeon[i][j] == 107) {
					Make_SetPC(i - 1, j - 1, 5, 5);
				}
			}
		}
	}
	if(currlevel == 16) {
		for(j = 0; j < DMAXY; j++) {
			for(i = 0; i < DMAXX; i++) {
				pdungeon[i][j] = dungeon[i][j];
			}
		}
		DRLG_LoadDiabQuads(FALSE);
	}
}
// 528A40: using guessed type int SP4x2;
// 528A48: using guessed type int SP4y2;
// 5B50D8: using guessed type int setloadflag_2;
// 679660: using guessed type char gbMaxPlayers;

void DRLG_L4Shadows()
{
	int x, y;
	BOOL okflag;

	for(y = 1; y < 40; y++) {
		for(x = 1; x < 40; x++) {
			okflag = FALSE;
			if(dungeon[x][y] == 3) {
				okflag = TRUE;
			}
			if(dungeon[x][y] == 4) {
				okflag = TRUE;
			}
			if(dungeon[x][y] == 8) {
				okflag = TRUE;
			}
			if(dungeon[x][y] == 15) {
				okflag = TRUE;
			}
			if(!okflag) {
				continue;
			}
			if(dungeon[x - 1][y] == 6) {
				dungeon[x - 1][y] = 47;
			}
			if(dungeon[x - 1][y - 1] == 6) {
				dungeon[x - 1][y - 1] = 48;
			}
		}
	}
}

void InitL4Dungeon()
{
	int i, j;

	memset(dung, 0, sizeof(dung));
	memset(L4dungeon, 0, sizeof(L4dungeon));

	for(j = 0; j < 40; j++) {
		for(i = 0; i < 40; i++) {
			dungeon[i][j] = 30;
			dflags[i][j] = 0;
		}
	}
}

void L4makeDmt()
{
	int i, j, val, dmtx, dmty;

	for(j = 0, dmty = 1; dmty <= 77; j++, dmty += 2) {
		for(i = 0, dmtx = 1; dmtx <= 77; i++, dmtx += 2) {
			val = L4dungeon[dmtx+1][dmty+1];
			val = 2 * val + L4dungeon[dmtx][dmty+1];
			val = 2 * val + L4dungeon[dmtx+1][dmty];
			val = 2 * val + L4dungeon[dmtx][dmty];
			dungeon[i][j] = L4ConvTbl[val];
		}
	}
}

void L4AddWall()
{
	int i, j, x, y;

	for(j = 0; j < 40; j++) {
		for(i = 0; i < 40; i++) {
			if(dflags[i][j] != 0) {
				continue;
			}
			if(dungeon[i][j] == 10 && random(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if(x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if(dungeon[i][j] == 12 && random(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if(x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if(dungeon[i][j] == 13 && random(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if(x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if(dungeon[i][j] == 15 && random(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if(x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if(dungeon[i][j] == 16 && random(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if(x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if(dungeon[i][j] == 21 && random(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if(x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if(dungeon[i][j] == 22 && random(0, 100) < 100) {
				x = L4HWallOk(i, j);
				if(x != -1) {
					L4HorizWall(i, j, x);
				}
			}
			if(dungeon[i][j] == 8 && random(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if(y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if(dungeon[i][j] == 9 && random(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if(y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if(dungeon[i][j] == 11 && random(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if(y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if(dungeon[i][j] == 14 && random(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if(y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if(dungeon[i][j] == 15 && random(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if(y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if(dungeon[i][j] == 16 && random(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if(y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if(dungeon[i][j] == 21 && random(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if(y != -1) {
					L4VertWall(i, j, y);
				}
			}
			if(dungeon[i][j] == 23 && random(0, 100) < 100) {
				y = L4VWallOk(i, j);
				if(y != -1) {
					L4VertWall(i, j, y);
				}
			}
		}
	}
}

int L4HWallOk(int i, int j)
{
	int x;
	BOOL wallok;

	for(x = 1; dungeon[i + x][j] == 6; x++) {
		if(dflags[i + x][j] != 0) {
			break;
		}
		if(dungeon[i + x][j - 1] != 6) {
			break;
		}
		if(dungeon[i + x][j + 1] != 6) {
			break;
		}
	}

	wallok = FALSE;

	if(dungeon[i + x][j] == 10) {
		wallok = TRUE;
	}
	if(dungeon[i + x][j] == 12) {
		wallok = TRUE;
	}
	if(dungeon[i + x][j] == 13) {
		wallok = TRUE;
	}
	if(dungeon[i + x][j] == 15) {
		wallok = TRUE;
	}
	if(dungeon[i + x][j] == 16) {
		wallok = TRUE;
	}
	if(dungeon[i + x][j] == 21) {
		wallok = TRUE;
	}
	if(dungeon[i + x][j] == 22) {
		wallok = TRUE;
	}
	if(x <= 3) {
		wallok = FALSE;
	}

	if(wallok) {
		return x;
	} else {
		return -1;
	}
}

int L4VWallOk(int i, int j)
{
	int y;
	BOOL wallok;

	for(y = 1; dungeon[i][j + y] == 6; y++) {
		if(dflags[i][j + y] != 0) {
			break;
		}
		if(dungeon[i - 1][j + y] != 6) {
			break;
		}
		if(dungeon[i + 1][j + y] != 6) {
			break;
		}
	}

	wallok = FALSE;

	if(dungeon[i][j + y] == 8) {
		wallok = TRUE;
	}
	if(dungeon[i][j + y] == 9) {
		wallok = TRUE;
	}
	if(dungeon[i][j + y] == 11) {
		wallok = TRUE;
	}
	if(dungeon[i][j + y] == 14) {
		wallok = TRUE;
	}
	if(dungeon[i][j + y] == 15) {
		wallok = TRUE;
	}
	if(dungeon[i][j + y] == 16) {
		wallok = TRUE;
	}
	if(dungeon[i][j + y] == 21) {
		wallok = TRUE;
	}
	if(dungeon[i][j + y] == 23) {
		wallok = TRUE;
	}
	if(y <= 3) {
		wallok = FALSE;
	}

	if(wallok) {
		return y;
	} else {
		return -1;
	}
}

void L4HorizWall(int i, int j, int dx)
{
	int xx;

	if(dungeon[i][j] == 13) {
		dungeon[i][j] = 17;
	}
	if(dungeon[i][j] == 16) {
		dungeon[i][j] = 11;
	}
	if(dungeon[i][j] == 12) {
		dungeon[i][j] = 14;
	}

	for(xx = 1; xx < dx; xx++) {
		dungeon[i + xx][j] = 2;
	}

	if(dungeon[i + dx][j] == 15) {
		dungeon[i + dx][j] = 14;
	}
	if(dungeon[i + dx][j] == 10) {
		dungeon[i + dx][j] = 17;
	}
	if(dungeon[i + dx][j] == 21) {
		dungeon[i + dx][j] = 23;
	}
	if(dungeon[i + dx][j] == 22) {
		dungeon[i + dx][j] = 29;
	}

	xx = random(0, dx - 3) + 1;
	dungeon[i + xx][j] = 57;
	dungeon[i + xx + 2][j] = 56;
	dungeon[i + xx + 1][j] = 60;

	if(dungeon[i + xx][j - 1] == 6) {
		dungeon[i + xx][j - 1] = 58;
	}
	if(dungeon[i + xx + 1][j - 1] == 6) {
		dungeon[i + xx + 1][j - 1] = 59;
	}
}

void L4VertWall(int i, int j, int dy)
{
	int yy;

	if(dungeon[i][j] == 14) {
		dungeon[i][j] = 17;
	}
	if(dungeon[i][j] == 8) {
		dungeon[i][j] = 9;
	}
	if(dungeon[i][j] == 15) {
		dungeon[i][j] = 10;
	}

	for(yy = 1; yy < dy; yy++) {
		dungeon[i][j + yy] = 1;
	}

	if(dungeon[i][j + dy] == 11) {
		dungeon[i][j + dy] = 17;
	}
	if(dungeon[i][j + dy] == 9) {
		dungeon[i][j + dy] = 10;
	}
	if(dungeon[i][j + dy] == 16) {
		dungeon[i][j + dy] = 13;
	}
	if(dungeon[i][j + dy] == 21) {
		dungeon[i][j + dy] = 22;
	}
	if(dungeon[i][j + dy] == 23) {
		dungeon[i][j + dy] = 29;
	}

	yy = random(0, dy - 3) + 1;
	dungeon[i][j + yy] = 53;
	dungeon[i][j + yy + 2] = 52;
	dungeon[i][j + yy + 1] = 6;

	if(dungeon[i - 1][j + yy] == 6) {
		dungeon[i - 1][j + yy] = 54;
	}
	if(dungeon[i - 1][j + yy - 1] == 6) {
		dungeon[i - 1][j + yy - 1] = 55;
	}
}

void L4tileFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 6)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 13;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 14;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 6)
				dungeon[i + 1][j] = 2;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 6)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 13;
			if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 14)
				dungeon[i + 1][j] = 15;
			if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 13)
				dungeon[i][j + 1] = 16;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 6 && dungeon[i][j - 1] == 1)
				dungeon[i][j - 1] = 1;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 27;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 27;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 27)
				dungeon[i + 1][j] = 26;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 22 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] != 0) /* check */
				dungeon[i + 1][j] = 22;
			if (dungeon[i][j] == 22 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 1 && dungeon[i + 1][j - 1] == 1)
				dungeon[i + 1][j] = 13;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 30 && dungeon[i][j + 1] == 6)
				dungeon[i + 1][j] = 28;
			if (dungeon[i][j] == 16 && dungeon[i + 1][j] == 6 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 27;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 30 && dungeon[i + 1][j + 1] == 30)
				dungeon[i][j + 1] = 27;
			if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 30 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 21;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 9)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 2)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 18)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 19 && dungeon[i + 1][j - 1] == 30)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 24 && dungeon[i][j - 1] == 30 && dungeon[i][j - 2] == 6)
				dungeon[i][j - 1] = 21;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 28;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 28;
			if (dungeon[i][j] == 28 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 28 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 19 && dungeon[i + 2][j] == 2 && dungeon[i + 1][j - 1] == 18 && dungeon[i + 1][j + 1] == 1)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 19 && dungeon[i + 2][j] == 2 && dungeon[i + 1][j - 1] == 22 && dungeon[i + 1][j + 1] == 1)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 19 && dungeon[i + 2][j] == 2 && dungeon[i + 1][j - 1] == 18 && dungeon[i + 1][j + 1] == 13)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 21 && dungeon[i + 2][j] == 2 && dungeon[i + 1][j - 1] == 18 && dungeon[i + 1][j + 1] == 1)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j + 1] == 1 && dungeon[i + 1][j - 1] == 22 && dungeon[i + 2][j] == 3)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 30 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 1)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 30)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 28 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j - 1] == 21)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 30)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 18)
				dungeon[i + 1][j] = 25;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 9 && dungeon[i + 2][j] == 2)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 10)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 15 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 22 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 24 && dungeon[i - 1][j] == 30)
				dungeon[i - 1][j] = 19;
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 22 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 22 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 18 && dungeon[i + 2][j] == 30)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 9 && dungeon[i + 1][j + 1] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 2)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 23 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 23 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 25 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 22 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 23 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 23 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 27)
				dungeon[i + 1][j] = 26;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 18)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 26 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 29 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 29 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 1 && dungeon[i][j - 1] == 15)
				dungeon[i][j - 1] = 10;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 23 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 30 && dungeon[i + 1][j + 1] == 30)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 28 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 23 && dungeon[i + 1][j] == 18 && dungeon[i][j - 1] == 6)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 23 && dungeon[i + 2][j] == 30)
				dungeon[i + 1][j] = 28;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 30 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 23 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 29 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 29 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 26 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 25 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 13;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 13 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 16;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 24 && dungeon[i][j + 2] == 1)
				dungeon[i][j + 1] = 17;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j + 1] == 9 && dungeon[i + 1][j - 1] == 1 && dungeon[i + 2][j] == 16)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 2 && dungeon[i - 1][j] == 6)
				dungeon[i - 1][j] = 8;
			if (dungeon[i][j] == 1 && dungeon[i][j - 1] == 6)
				dungeon[i][j - 1] = 7;
			if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 15 && dungeon[i + 1][j + 1] == 4)
				dungeon[i + 1][j] = 10;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 6)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 9 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 10 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 16)
				dungeon[i][j + 1] = 13;
			if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 13)
				dungeon[i][j + 1] = 16;
			if (dungeon[i][j] == 25 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 28 && dungeon[i][j - 1] == 6 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 10)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 10 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 5 && dungeon[i + 1][j - 1] == 16)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 1 && dungeon[i][j - 1] == 8)
				dungeon[i][j - 1] = 9;
			if (dungeon[i][j] == 28 && dungeon[i + 1][j] == 23 && dungeon[i + 1][j + 1] == 3)
				dungeon[i + 1][j] = 16;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 10)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 17 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 10 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 17 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 29 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 9 && dungeon[i][j + 1] == 16)
				dungeon[i][j + 1] = 13;
			if (dungeon[i][j] == 10 && dungeon[i][j + 1] == 16)
				dungeon[i][j + 1] = 13;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 11 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 10 && dungeon[i + 1][j] == 3 && dungeon[i + 1][j - 1] == 16)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 6)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 13 && dungeon[i][j + 1] == 10)
				dungeon[i + 1][j + 1] = 12;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 10)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 22 && dungeon[i][j + 1] == 11)
				dungeon[i][j + 1] = 17;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 16)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 28 && dungeon[i + 1][j] == 23 && dungeon[i + 1][j + 1] == 1 && dungeon[i + 2][j] == 6)
				dungeon[i + 1][j] = 16;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 16)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j - 1] == 21 && dungeon[i + 1][j + 1] == 13 && dungeon[i + 2][j] == 2)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 15 && dungeon[i + 1][j + 1] == 12)
				dungeon[i + 1][j] = 17;
		}
	}
}

void DRLG_L4Subs()
{
	int x, y, i, rv;
	unsigned char c;

	for(y = 0; y < 40; y++) {
		for(x = 0; x < 40; x++) {
			if(random(0, 3) == 0) {
				c = L4BTYPES[dungeon[x][y]];
				if(c != 0 && dflags[x][y] == 0) {
					rv = random(0, 16);
					i = -1;
					while(rv >= 0) {
						i++;
						if(i == sizeof(L4BTYPES)) {
							i = 0;
						}
						if(c == L4BTYPES[i]) {
							rv--;
						}
					}
					dungeon[x][y] = i;
				}
			}
		}
	}
	for(y = 0; y < 40; y++) {
		for(x = 0; x < 40; x++) {
			if(random(0, 10) == 0) {
				if(L4BTYPES[dungeon[x][y]] == 6 && dflags[x][y] == 0) {
					dungeon[x][y] = random(0, 3) + 95;
				}
			}
		}
	}
}

void L4makeDungeon()
{
	int i, j, k, l;

	for(j = 0; j < 20; j++) {
		for(i = 0; i < 20; i++) {
			k = i << 1;
			l = j << 1;
			L4dungeon[k][l] = dung[i][j];
			L4dungeon[k][l + 1] = dung[i][j];
			L4dungeon[k + 1][l] = dung[i][j];
			L4dungeon[k + 1][l + 1] = dung[i][j];
		}
	}
	for(j = 0; j < 20; j++) {
		for(i = 0; i < 20; i++) {
			k = i << 1;
			l = j << 1;
			L4dungeon[k][l + 40] = dung[i][19 - j];
			L4dungeon[k][l + 41] = dung[i][19 - j];
			L4dungeon[k + 1][l + 40] = dung[i][19 - j];
			L4dungeon[k + 1][l + 41] = dung[i][19 - j];
		}
	}
	for(j = 0; j < 20; j++) {
		for(i = 0; i < 20; i++) {
			k = i << 1;
			l = j << 1;
			L4dungeon[k + 40][l] = dung[19 - i][j];
			L4dungeon[k + 40][l + 1] = dung[19 - i][j];
			L4dungeon[k + 41][l] = dung[19 - i][j];
			L4dungeon[k + 41][l + 1] = dung[19 - i][j];
		}
	}
	for(j = 0; j < 20; j++) {
		for(i = 0; i < 20; i++) {
			k = i << 1;
			l = j << 1;
			L4dungeon[k + 40][l + 40] = dung[19 - i][19 - j];
			L4dungeon[k + 40][l + 41] = dung[19 - i][19 - j];
			L4dungeon[k + 41][l + 40] = dung[19 - i][19 - j];
			L4dungeon[k + 41][l + 41] = dung[19 - i][19 - j];
		}
	}
}

void uShape()
{
	int v0;         // ecx
	signed int v1;  // esi
	signed int v2;  // eax
	char v3;        // dl
	int v4;         // eax
	signed int v5;  // esi
	int v6;         // ecx
	int v7;         // ecx
	int *v8;        // esi
	signed int v9;  // eax
	char v10;       // dl
	int v11;        // eax
	signed int v12; // esi
	char *v13;      // edx

	v0 = 19;
	do {
		v1 = 19;
		do {
			v2 = v1;
			v3 = dung[v1][v0];
			if (v3 == 1 || (hallok[v0] = 0, v3 == 1)) {
				hallok[v0] = dung[v2][v0 + 1] == 1 && !dung[v2 + 1][v0 + 1];
				v1 = 0;
			}
			--v1;
		} while (v1 >= 0);
		--v0;
	} while (v0 >= 0);
	v4 = random(0, 19) + 1;
	do {
		if (hallok[v4]) {
			v5 = 19;
			do {
				v6 = v4 + 20 * v5;
				if (dung[0][v6] == 1) {
					v5 = -1;
					v4 = 0;
				} else {
					dung[0][v6] = 1;
					dung[0][v6 + 1] = 1;
				}
				--v5;
			} while (v5 >= 0);
		} else if (++v4 == 20) {
			v4 = 1;
		}
	} while (v4);
	v7 = 380;
	v8 = &hallok[19];
	do {
		v9 = 19;
		do {
			v10 = dung[0][v7 + v9];
			if (v10 == 1 || (*v8 = 0, v10 == 1)) {
				*v8 = dung[1][v7 + v9] == 1 && !dung[1][v7 + 1 + v9];
				v9 = 0;
			}
			--v9;
		} while (v9 >= 0);
		--v8;
		v7 -= 20;
	} while ((signed int)v8 >= (signed int)hallok);
	v11 = random(0, 19) + 1;
	do {
		if (hallok[v11]) {
			v12 = 19;
			do {
				v13 = (char *)&dung[v11][v12];
				if (*v13 == 1) {
					v12 = -1;
					v11 = 0;
				} else {
					*v13 = 1;
					dung[v11 + 1][v12] = 1;
				}
				--v12;
			} while (v12 >= 0);
		} else if (++v11 == 20) {
			v11 = 1;
		}
	} while (v11);
}

long GetArea()
{
	int i, j;
	long rv;

	rv = 0;

	for(j = 0; j < 20; j++) {
		for(i = 0; i < 20; i++) {
			if(dung[i][j] == 1) {
				rv++;
			}
		}
	}

	return rv;
}

void L4firstRoom()
{
	int x, y, w, h, rndx, rndy; /* xmin, xmax, ymin, ymax */

	if(currlevel != 16) {
		if(currlevel == quests[QTYPE_WARLRD]._qlevel && quests[QTYPE_WARLRD]._qactive) {
			/// ASSERT: assert(gbMaxPlayers == 1);
			w = 11;
			h = 11;
		} else if(currlevel == quests[QTYPE_VB]._qlevel && gbMaxPlayers != 1) {
			w = 11;
			h = 11;
		} else {
			w = random(0, 5) + 2;
			h = random(0, 5) + 2;
		}
	} else {
		w = 14;
		h = 14;
	}

	rndx = random(0, 19 - w - ((20 - w) >> 1) + 1) + ((20 - w) >> 1);
	if(rndx + w > 19) {
		x = 19 - w + 1;
	} else {
		x = rndx;
	}
	rndy = random(0, 19 - h - ((20 - h) >> 1) + 1) + ((20 - h) >> 1);
	if(rndy + h > 19) {
		y = 19 - h + 1;
	} else {
		y = rndy;
	}

	if(currlevel == 16) {
		l4holdx = x;
		l4holdy = y;
	}
	if(QuestStatus(QTYPE_WARLRD) || currlevel == quests[QTYPE_VB]._qlevel && gbMaxPlayers != 1) {
		SP4x1 = x + 1;
		SP4y1 = y + 1;
		SP4x2 = SP4x1 + w;
		SP4y2 = SP4y1 + h;
	} else {
		SP4x1 = 0;
		SP4y1 = 0;
		SP4x2 = 0;
		SP4y2 = 0;
	}

	L4drawRoom(x, y, w, h);
	L4roomGen(x, y, w, h, random(0, 2));
}
// 528A34: using guessed type int l4holdx;
// 528A38: using guessed type int l4holdy;
// 528A40: using guessed type int SP4x2;
// 528A48: using guessed type int SP4y2;
// 679660: using guessed type char gbMaxPlayers;

void L4drawRoom(int x, int y, int width, int height)
{
	int i, j;

	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			dung[i + x][j + y] = 1;
		}
	}
}

void L4roomGen(int x, int y, int w, int h, int dir)
{
	int num;
	BOOL ran, ran2;
	int width, height, rx, ry, ry2;
	int cw, ch, cx1, cy1, cx2;

	int dirProb = random(0, 4);

	switch(dir == 1 ? dirProb != 0 : dirProb == 0) {
		case FALSE:
			num = 0;
			do {
				cw = (random(0, 5) + 2) & ~1;
				ch = (random(0, 5) + 2) & ~1;
				cy1 = h/2 + y - ch/2;
				cx1 = x-cw;
				ran = L4checkRoom(cx1-1, cy1-1, ch+2, cw+1); /// BUGFIX: swap args 3 and 4 ("ch+2" and "cw+1")
				num++;
			} while(ran == FALSE && num < 20);

			if(ran == TRUE)
				L4drawRoom(cx1, cy1, cw, ch);
			cx2 = x+w;
			ran2 = L4checkRoom(cx2, cy1-1, cw+1, ch+2);
			if(ran2 == TRUE)
				L4drawRoom(cx2, cy1, cw, ch);
			if(ran == TRUE)
				L4roomGen(cx1, cy1, cw, ch, 1);
			if(ran2 == TRUE)
				L4roomGen(cx2, cy1, cw, ch, 1);
			break;
		case TRUE:
			num = 0;
			do {
				width = (random(0, 5) + 2) & ~1;
				height = (random(0, 5) + 2) & ~1;
				rx = w/2 + x - width/2;
				ry = y-height;
				ran = L4checkRoom(rx-1, ry-1, width+2, height+1);
				num++;
			} while(ran == FALSE && num < 20);

			if(ran == TRUE)
				L4drawRoom(rx, ry, width, height);
			ry2 = y+h;
			ran2 = L4checkRoom(rx-1, ry2, width+2, height+1);
			if(ran2 == TRUE)
				L4drawRoom(rx, ry2, width, height);
			if(ran == TRUE)
				L4roomGen(rx, ry, width, height, 0);
			if(ran2 == TRUE)
				L4roomGen(rx, ry2, width, height, 0);
			break;
	}
}

BOOL L4checkRoom(int x, int y, int width, int height)
{
	int i, j;

	if(x <= 0 || y <= 0) {
		return FALSE;
	}

	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			if(i + x < 0 || i + x >= 20 || j + y < 0 || j + y >= 20) {
				return FALSE;
			}
			if(dung[i + x][j + y] != 0) {
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL DRLG_L4PlaceMiniSet(const unsigned char *miniset, int tmin, int tmax, int cx, int cy, BOOL setview, int ldir)
{
	int sx, sy, sw, sh, xx, yy, i, ii, numt, bailcnt;
	BOOL found;

	sw = miniset[0];
	sh = miniset[1];

	if(tmax - tmin == 0) {
		numt = 1;
	} else {
		numt = random(0, tmax - tmin) + tmin;
	}

	for(i = 0; i < numt; i++) {
		sx = random(0, 40 - sw);
		sy = random(0, 40 - sh);
		found = FALSE;
		for(bailcnt = 0; !found && bailcnt < 200; bailcnt++) {
			found = TRUE;
			if(sx >= SP4x1 && sx <= SP4x2 && sy >= SP4y1 && sy <= SP4y2) {
				found = FALSE;
			}
			if(cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx = random(0, 40 - sw);
				sy = random(0, 40 - sh);
				found = FALSE;
			}
			if(cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sx = random(0, 40 - sw);
				sy = random(0, 40 - sh);
				found = FALSE;
			}
			ii = 2;
			for(yy = 0; yy < sh && found == TRUE; yy++) {
				for(xx = 0; xx < sw && found == TRUE; xx++) {
					if(miniset[ii] != 0 && dungeon[xx + sx][yy + sy] != miniset[ii]) {
						found = FALSE;
					}
					if(dflags[xx + sx][yy + sy] != 0) {
						found = FALSE;
					}
					ii++;
				}
			}
			if(!found) {
				sx++;
				if(sx == 40 - sw) {
					sx = 0;
					sy++;
					if(sy == 40 - sh) {
						sy = 0;
					}
				}
			}
		}
		if(bailcnt >= 200) {
			return FALSE;
		}
		ii = sw * sh + 2;
		for(yy = 0; yy < sh; yy++) {
			for(xx = 0; xx < sw; xx++) {
				if(miniset[ii] != 0) {
					dungeon[xx + sx][yy + sy] = miniset[ii];
					dflags[xx + sx][yy + sy] |= 8;
				}
				ii++;
			}
		}
	}

	if(currlevel == 15) {
		quests[15]._qtx = sx + 1;
		quests[15]._qty = sy + 1;
	}
	if(setview == TRUE) {
		ViewX = 2 * sx + 21;
		ViewY = 2 * sy + 22;
	}
	if(ldir == 0) {
		LvlViewX = 2 * sx + 21;
		LvlViewY = 2 * sy + 22;
	}

	return TRUE;
}
// 528A40: using guessed type int SP4x2;
// 528A48: using guessed type int SP4y2;
// 5CF320: using guessed type int LvlViewY;
// 5CF324: using guessed type int LvlViewX;

void DRLG_L4FloodTVal()
{
	int i, j, xx, yy;

	yy = 16;
	for(j = 0; j < DMAXY; j++) {
		xx = 16;
		for(i = 0; i < DMAXX; i++) {
			if(dungeon[i][j] == 6 && dTransVal[xx][yy] == 0) {
				DRLG_L4FTVR(i, j, xx, yy, 0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}
// 5A5590: using guessed type char TransVal;

void DRLG_L4FTVR(int i, int j, int x, int y, int d)
{
	if(dTransVal[x][y] != 0 || dungeon[i][j] != 6) {
		if(d == 1) {
			dTransVal[x][y] = TransVal;
			dTransVal[x][y + 1] = TransVal;
		}
		if(d == 2) {
			dTransVal[x + 1][y] = TransVal;
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if(d == 3) {
			dTransVal[x][y] = TransVal;
			dTransVal[x + 1][y] = TransVal;
		}
		if(d == 4) {
			dTransVal[x][y + 1] = TransVal;
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if(d == 5) {
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if(d == 6) {
			dTransVal[x][y + 1] = TransVal;
		}
		if(d == 7) {
			dTransVal[x + 1][y] = TransVal;
		}
		if(d == 8) {
			dTransVal[x][y] = TransVal;
		}
	} else {
		dTransVal[x][y] = TransVal;
		dTransVal[x + 1][y] = TransVal;
		dTransVal[x][y + 1] = TransVal;
		dTransVal[x + 1][y + 1] = TransVal;
		DRLG_L4FTVR(i + 1, j, x + 2, y, 1);
		DRLG_L4FTVR(i - 1, j, x - 2, y, 2);
		DRLG_L4FTVR(i, j + 1, x, y + 2, 3);
		DRLG_L4FTVR(i, j - 1, x, y - 2, 4);
		DRLG_L4FTVR(i - 1, j - 1, x - 2, y - 2, 5);
		DRLG_L4FTVR(i + 1, j - 1, x + 2, y - 2, 6);
		DRLG_L4FTVR(i - 1, j + 1, x - 2, y + 2, 7);
		DRLG_L4FTVR(i + 1, j + 1, x + 2, y + 2, 8);
	}
}
// 5A5590: using guessed type char TransVal;

void DRLG_L4TransFix()
{
	int i, j, xx, yy;

	yy = 16;
	for(j = 0; j < DMAXY; j++) {
		xx = 16;
		for(i = 0; i < DMAXX; i++) {
			if(IsDURWall(dungeon[i][j]) && dungeon[i][j - 1] == 18) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if(IsDLLWall(dungeon[i][j]) && dungeon[i + 1][j] == 19) {
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if(dungeon[i][j] == 18) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if(dungeon[i][j] == 19) {
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if(dungeon[i][j] == 24) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if(dungeon[i][j] == 57) {
				dTransVal[xx - 1][yy] = dTransVal[xx][yy + 1];
				dTransVal[xx][yy] = dTransVal[xx][yy + 1];
			}
			if(dungeon[i][j] == 53) {
				dTransVal[xx][yy - 1] = dTransVal[xx + 1][yy];
				dTransVal[xx][yy] = dTransVal[xx + 1][yy];
			}
			xx += 2;
		}
		yy += 2;
	}
}

void DRLG_L4Corners()
{
	int i, j;

	for(j = 1; j < DMAXY - 1; j++) {
		for(i = 1; i < DMAXX - 1; i++) {
			if(dungeon[i][j] >= 18 && dungeon[i][j] <= 30) {
				if(dungeon[i + 1][j] < 18) {
					dungeon[i][j] += 98;
				} else if(dungeon[i][j + 1] < 18) {
					dungeon[i][j] += 98;
				}
			}
		}
	}
}

void DRLG_L4Pass3()
{
	int i, j, xx, yy;
	long v1, v2, v3, v4, lv;

	lv = 30 - 1;

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
			lv = (unsigned char)dungeon[i][j] - 1;
#ifdef USE_ASM
			if (lv >= 0) {
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
			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
			}
#else
			if (lv >= 0) {
				v1 = *((WORD *)&pMegaTiles[lv * 8]) + 1;
				v2 = *((WORD *)&pMegaTiles[lv * 8] + 1) + 1;
				v3 = *((WORD *)&pMegaTiles[lv * 8] + 2) + 1;
				v4 = *((WORD *)&pMegaTiles[lv * 8] + 3) + 1;
			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
			}
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

DEVILUTION_END_NAMESPACE
