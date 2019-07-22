#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

// BUGFIX: constant data should be const
BYTE SkelKingTrans1[8] = {
	19, 47, 26, 55,
	26, 49, 30, 53
};

BYTE SkelKingTrans2[8] = {
	33, 19, 47, 29,
	37, 29, 43, 39
};

BYTE SkelKingTrans3[20] = {
	27, 53, 35, 61,
	27, 35, 34, 42,
	45, 35, 53, 43,
	45, 53, 53, 61,
	31, 39, 49, 57
};

BYTE SkelKingTrans4[28] = {
	49, 45, 58, 51,
	57, 31, 62, 37,
	63, 31, 69, 40,
	59, 41, 73, 55,
	63, 55, 69, 65,
	73, 45, 78, 51,
	79, 43, 89, 53
};

BYTE SkelChamTrans1[20] = {
	43, 19, 50, 26,
	51, 19, 59, 26,
	35, 27, 42, 34,
	43, 27, 49, 34,
	50, 27, 59, 34
};

BYTE SkelChamTrans2[8] = {
	19, 31, 34, 47,
	34, 35, 42, 42
};

BYTE SkelChamTrans3[36] = {
	43, 35, 50, 42,
	51, 35, 62, 42,
	63, 31, 66, 46,
	67, 31, 78, 34,
	67, 35, 78, 42,
	67, 43, 78, 46,
	35, 43, 42, 51,
	43, 43, 49, 51,
	50, 43, 59, 51
};

char *quest_level_names[] = {
	"",
	"Skeleton King's Lair",
	"Bone Chamber",
	"Maze",
	"Poisoned Water Supply",
	"Archbishop Lazarus' Lair"
};

int ObjIndex(int x, int y)
{
	int i;
	int oi;

	for (i = 0; i < nobjects; i++) {
		oi = objectactive[i];
		if (object[oi]._ox == x && object[oi]._oy == y)
			return oi;
	}
	app_fatal("ObjIndex: Active object not found at (%d,%d)", x, y);
	return -1;
}

void AddSKingObjs()
{
	SetObjMapRange(ObjIndex(64, 34), 20, 7, 23, 10, 1);
	SetObjMapRange(ObjIndex(64, 59), 20, 14, 21, 16, 2);
	SetObjMapRange(ObjIndex(27, 37), 8, 1, 15, 11, 3);
	SetObjMapRange(ObjIndex(46, 35), 8, 1, 15, 11, 3);
	SetObjMapRange(ObjIndex(49, 53), 8, 1, 15, 11, 3);
	SetObjMapRange(ObjIndex(27, 53), 8, 1, 15, 11, 3);
}

void AddSChamObjs()
{
	SetObjMapRange(ObjIndex(37, 30), 17, 0, 21, 5, 1);
	SetObjMapRange(ObjIndex(37, 46), 13, 0, 16, 5, 2);
}

void AddVileObjs()
{
	SetObjMapRange(ObjIndex(26, 45), 1, 1, 9, 10, 1);
	SetObjMapRange(ObjIndex(45, 46), 11, 1, 20, 10, 2);
	SetObjMapRange(ObjIndex(35, 36), 7, 11, 13, 18, 3);
}

void DRLG_SetMapTrans(char *sFileName)
{
	int x, y;
	int i, j;
	BYTE *pLevelMap;
	BYTE *d;
	DWORD dwOffset;

	pLevelMap = LoadFileInMem(sFileName, NULL);
	d = pLevelMap + 2;
	x = pLevelMap[0];
	y = *d;
	dwOffset = (x * y + 1) * 2;
	x <<= 1;
	y <<= 1;
	dwOffset += 3 * x * y * 2;
	d += dwOffset;

	for (j = 0; j < y; j++) {
		for (i = 0; i < x; i++) {
			dTransVal[16 + i][16 + j] = *d;
			d += 2;
		}
	}
	mem_free_dbg(pLevelMap);
}

void LoadSetMap()
{
	switch (setlvlnum) {
	case SL_SKELKING:
		if (quests[QTYPE_KING]._qactive == 1) {
			quests[QTYPE_KING]._qactive = 2;
			quests[QTYPE_KING]._qvar1 = 1;
		}
		LoadPreL1Dungeon("Levels\\L1Data\\SklKng1.DUN", 83, 45);
		LoadL1Dungeon("Levels\\L1Data\\SklKng2.DUN", 83, 45);
		LoadPalette("Levels\\L1Data\\L1_2.pal");
		DRLG_AreaTrans(sizeof(SkelKingTrans1) / 4, SkelKingTrans1);
		DRLG_ListTrans(sizeof(SkelKingTrans2) / 4, SkelKingTrans2);
		DRLG_AreaTrans(sizeof(SkelKingTrans3) / 4, SkelKingTrans3);
		DRLG_ListTrans(sizeof(SkelKingTrans4) / 4, SkelKingTrans4);
		AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
		AddSKingObjs();
		InitSKingTriggers();
		break;
	case SL_BONECHAMB:
		LoadPreL2Dungeon("Levels\\L2Data\\Bonecha2.DUN", 69, 39);
		LoadL2Dungeon("Levels\\L2Data\\Bonecha1.DUN", 69, 39);
		LoadPalette("Levels\\L2Data\\L2_2.pal");
		DRLG_ListTrans(sizeof(SkelChamTrans1) / 4, SkelChamTrans1);
		DRLG_AreaTrans(sizeof(SkelChamTrans2) / 4, SkelChamTrans2);
		DRLG_ListTrans(sizeof(SkelChamTrans3) / 4, SkelChamTrans3);
		AddL2Objs(0, 0, MAXDUNX, MAXDUNY);
		AddSChamObjs();
		InitSChambTriggers();
		break;
	case SL_MAZE:
		LoadPreL1Dungeon("Levels\\L1Data\\Lv1MazeA.DUN", 20, 50);
		LoadL1Dungeon("Levels\\L1Data\\Lv1MazeB.DUN", 20, 50);
		LoadPalette("Levels\\L1Data\\L1_5.pal");
		AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
		DRLG_SetMapTrans("Levels\\L1Data\\Lv1MazeA.DUN");
		break;
	case SL_POISONWATER:
		if (quests[QTYPE_PW]._qactive == 1)
			quests[QTYPE_PW]._qactive = 2;
		LoadPreL3Dungeon("Levels\\L3Data\\Foulwatr.DUN", 19, 50);
		LoadL3Dungeon("Levels\\L3Data\\Foulwatr.DUN", 20, 50);
		LoadPalette("Levels\\L3Data\\L3pfoul.pal");
		InitPWaterTriggers();
		break;
	case SL_VILEBETRAYER:
		if (quests[QTYPE_VB]._qactive == 3) {
			quests[QTYPE_VB]._qvar2 = 4;
		} else if (quests[QTYPE_VB]._qactive == 2) {
			quests[QTYPE_VB]._qvar2 = 3;
		}
		LoadPreL1Dungeon("Levels\\L1Data\\Vile1.DUN", 35, 36);
		LoadL1Dungeon("Levels\\L1Data\\Vile2.DUN", 35, 36);
		LoadPalette("Levels\\L1Data\\L1_2.pal");
		AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
		AddVileObjs();
		DRLG_SetMapTrans("Levels\\L1Data\\Vile1.DUN");
		InitNoTriggers();
		break;
	}
}

DEVILUTION_END_NAMESPACE
