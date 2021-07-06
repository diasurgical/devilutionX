/**
 * @file setmaps.cpp
 *
 * Implementation of functionality the special quest dungeons.
 */
#include "setmaps.h"

#include "drlg_l1.h"
#include "drlg_l2.h"
#include "drlg_l3.h"
#include "engine/load_file.hpp"
#include "objdat.h"
#include "objects.h"
#include "palette.h"
#include "quests.h"
#include "trigs.h"
#include "utils/language.h"

namespace devilution {

// BUGFIX: constant data should be const
BYTE SkelKingTrans1[] = {
	19, 47, 26, 55,
	26, 49, 30, 53
};

BYTE SkelKingTrans2[] = {
	33, 19, 47, 29,
	37, 29, 43, 39
};

BYTE SkelKingTrans3[] = {
	27, 53, 35, 61,
	27, 35, 34, 42,
	45, 35, 53, 43,
	45, 53, 53, 61,
	31, 39, 49, 57
};

BYTE SkelKingTrans4[] = {
	49, 45, 58, 51,
	57, 31, 62, 37,
	63, 31, 69, 40,
	59, 41, 73, 55,
	63, 55, 69, 65,
	73, 45, 78, 51,
	79, 43, 89, 53
};

BYTE SkelChamTrans1[] = {
	43, 19, 50, 26,
	51, 19, 59, 26,
	35, 27, 42, 34,
	43, 27, 49, 34,
	50, 27, 59, 34
};

BYTE SkelChamTrans2[] = {
	19, 31, 34, 47,
	34, 35, 42, 42
};

BYTE SkelChamTrans3[] = {
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

/** Maps from quest level to quest level names. */
const char *const QuestLevelNames[] = {
	"",
	N_("Skeleton King's Lair"),
	N_("Chamber of Bone"),
	N_("Maze"),
	N_("Poisoned Water Supply"),
	N_("Archbishop Lazarus' Lair"),
};

int ObjIndex(int x, int y)
{
	int i;
	int oi;

	for (i = 0; i < ActiveObjectCount; i++) {
		oi = ActiveObjects[i];
		if (Objects[oi].position.x == x && Objects[oi].position.y == y)
			return oi;
	}
	app_fatal("ObjIndex: Active object not found at (%i,%i)", x, y);
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

void DRLG_SetMapTrans(const char *path)
{
	auto dunData = LoadFileInMem<uint16_t>(path);

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	int layer2Offset = 2 + width * height;

	// The rest of the layers are at dPiece scale
	width *= 2;
	height *= 2;

	const uint16_t *transparantLayer = &dunData[layer2Offset + width * height * 3];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			dTransVal[16 + i][16 + j] = SDL_SwapLE16(*transparantLayer);
			transparantLayer++;
		}
	}
}

/**
 * @brief Load a quest map, the given map is specified via the global setlvlnum
 */
void LoadSetMap()
{
	switch (setlvlnum) {
	case SL_SKELKING:
		if (Quests[Q_SKELKING]._qactive == QUEST_INIT) {
			Quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
			Quests[Q_SKELKING]._qvar1 = 1;
		}
		LoadPreL1Dungeon("Levels\\L1Data\\SklKng1.DUN");
		LoadL1Dungeon("Levels\\L1Data\\SklKng2.DUN", 83, 45);
		LoadPalette("Levels\\L1Data\\L1_2.pal");
		DRLG_AreaTrans(sizeof(SkelKingTrans1) / 4, &SkelKingTrans1[0]);
		DRLG_ListTrans(sizeof(SkelKingTrans2) / 4, &SkelKingTrans2[0]);
		DRLG_AreaTrans(sizeof(SkelKingTrans3) / 4, &SkelKingTrans3[0]);
		DRLG_ListTrans(sizeof(SkelKingTrans4) / 4, &SkelKingTrans4[0]);
		AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
		AddSKingObjs();
		InitSKingTriggers();
		break;
	case SL_BONECHAMB:
		LoadPreL2Dungeon("Levels\\L2Data\\Bonecha2.DUN");
		LoadL2Dungeon("Levels\\L2Data\\Bonecha1.DUN", 69, 39);
		LoadPalette("Levels\\L2Data\\L2_2.pal");
		DRLG_ListTrans(sizeof(SkelChamTrans1) / 4, &SkelChamTrans1[0]);
		DRLG_AreaTrans(sizeof(SkelChamTrans2) / 4, &SkelChamTrans2[0]);
		DRLG_ListTrans(sizeof(SkelChamTrans3) / 4, &SkelChamTrans3[0]);
		AddL2Objs(0, 0, MAXDUNX, MAXDUNY);
		AddSChamObjs();
		InitSChambTriggers();
		break;
	case SL_MAZE:
		LoadPreL1Dungeon("Levels\\L1Data\\Lv1MazeA.DUN");
		LoadL1Dungeon("Levels\\L1Data\\Lv1MazeB.DUN", 20, 50);
		LoadPalette("Levels\\L1Data\\L1_5.pal");
		AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
		DRLG_SetMapTrans("Levels\\L1Data\\Lv1MazeA.DUN");
		break;
	case SL_POISONWATER:
		if (Quests[Q_PWATER]._qactive == QUEST_INIT)
			Quests[Q_PWATER]._qactive = QUEST_ACTIVE;
		LoadPreL3Dungeon("Levels\\L3Data\\Foulwatr.DUN");
		LoadL3Dungeon("Levels\\L3Data\\Foulwatr.DUN", 31, 83);
		LoadPalette("Levels\\L3Data\\L3pfoul.pal");
		InitPWaterTriggers();
		break;
	case SL_VILEBETRAYER:
		if (Quests[Q_BETRAYER]._qactive == QUEST_DONE) {
			Quests[Q_BETRAYER]._qvar2 = 4;
		} else if (Quests[Q_BETRAYER]._qactive == QUEST_ACTIVE) {
			Quests[Q_BETRAYER]._qvar2 = 3;
		}
		LoadPreL1Dungeon("Levels\\L1Data\\Vile1.DUN");
		LoadL1Dungeon("Levels\\L1Data\\Vile2.DUN", 35, 36);
		LoadPalette("Levels\\L1Data\\L1_2.pal");
		AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
		AddVileObjs();
		DRLG_SetMapTrans("Levels\\L1Data\\Vile1.DUN");
		InitNoTriggers();
		break;
	case SL_NONE:
		break;
	}
}

} // namespace devilution
