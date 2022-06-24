#include "levels/setmaps.h"

#ifdef _DEBUG
#include "debug.h"
#endif
#include "engine/load_file.hpp"
#include "engine/palette.h"
#include "levels/drlg_l1.h"
#include "levels/drlg_l2.h"
#include "levels/drlg_l3.h"
#include "levels/drlg_l4.h"
#include "levels/gendung.h"
#include "levels/trigs.h"
#include "objdat.h"
#include "objects.h"
#include "quests.h"
#include "utils/language.h"

namespace devilution {

/** Maps from quest level to quest level names. */
const char *const QuestLevelNames[] = {
	"",
	N_("Skeleton King's Lair"),
	N_("Chamber of Bone"),
	N_("Maze"),
	N_("Poisoned Water Supply"),
	N_("Archbishop Lazarus' Lair"),
};

namespace {

void AddSKingObjs()
{
	constexpr Rectangle SmallSecretRoom { { 20, 7 }, { 3, 3 } };
	ObjectAtPosition({ 64, 34 })->InitializeLoadedObject(SmallSecretRoom, 1);

	constexpr Rectangle Gate { { 20, 14 }, { 1, 2 } };
	ObjectAtPosition({ 64, 59 })->InitializeLoadedObject(Gate, 2);

	constexpr Rectangle LargeSecretRoom { { 8, 1 }, { 7, 10 } };
	ObjectAtPosition({ 27, 37 })->InitializeLoadedObject(LargeSecretRoom, 3);
	ObjectAtPosition({ 46, 35 })->InitializeLoadedObject(LargeSecretRoom, 3);
	ObjectAtPosition({ 49, 53 })->InitializeLoadedObject(LargeSecretRoom, 3);
	ObjectAtPosition({ 27, 53 })->InitializeLoadedObject(LargeSecretRoom, 3);
}

void AddSChamObjs()
{
	ObjectAtPosition({ 37, 30 })->InitializeLoadedObject({ { 17, 0 }, { 4, 5 } }, 1);
	ObjectAtPosition({ 37, 46 })->InitializeLoadedObject({ { 13, 0 }, { 3, 5 } }, 2);
}

void AddVileObjs()
{
	ObjectAtPosition({ 26, 45 })->InitializeLoadedObject({ { 1, 1 }, { 8, 9 } }, 1);
	ObjectAtPosition({ 45, 46 })->InitializeLoadedObject({ { 11, 1 }, { 9, 9 } }, 2);
	ObjectAtPosition({ 35, 36 })->InitializeLoadedObject({ { 7, 11 }, { 6, 7 } }, 3);
}

void SetMapTransparency(const char *path)
{
	auto dunData = LoadFileInMem<uint16_t>(path);
	LoadTransparency(dunData.get());
}

} // namespace

void LoadSetMap()
{
	switch (setlvlnum) {
	case SL_SKELKING:
		if (Quests[Q_SKELKING]._qactive == QUEST_INIT) {
			Quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
			Quests[Q_SKELKING]._qvar1 = 1;
		}
		LoadPreL1Dungeon("Levels\\L1Data\\SklKng1.DUN");
		LoadL1Dungeon("Levels\\L1Data\\SklKng2.DUN", { 83, 45 });
		SetMapTransparency("Levels\\L1Data\\SklKngT.dun");
		LoadPalette("Levels\\L1Data\\L1_2.pal");
		AddSKingObjs();
		InitSKingTriggers();
		break;
	case SL_BONECHAMB:
		LoadPreL2Dungeon("Levels\\L2Data\\Bonecha2.DUN");
		LoadL2Dungeon("Levels\\L2Data\\Bonecha1.DUN", { 69, 39 });
		SetMapTransparency("Levels\\L2Data\\BonechaT.dun");
		LoadPalette("Levels\\L2Data\\L2_2.pal");
		AddSChamObjs();
		InitSChambTriggers();
		break;
	case SL_MAZE:
		break;
	case SL_POISONWATER:
		if (Quests[Q_PWATER]._qactive == QUEST_INIT)
			Quests[Q_PWATER]._qactive = QUEST_ACTIVE;
		LoadL3Dungeon("Levels\\L3Data\\Foulwatr.DUN", { 31, 83 });
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
		LoadL1Dungeon("Levels\\L1Data\\Vile2.DUN", { 35, 36 });
		SetMapTransparency("Levels\\L1Data\\Vile1.DUN");
		LoadPalette("Levels\\L1Data\\L1_2.pal");
		AddVileObjs();
		InitNoTriggers();
		break;
	case SL_NONE:
#ifdef _DEBUG
		switch (setlvltype) {
		case DTYPE_CATHEDRAL:
		case DTYPE_CRYPT:
			LoadL1Dungeon(TestMapPath.c_str(), ViewPosition);
			break;
		case DTYPE_CATACOMBS:
			LoadL2Dungeon(TestMapPath.c_str(), ViewPosition);
			break;
		case DTYPE_CAVES:
		case DTYPE_NEST:
			LoadL3Dungeon(TestMapPath.c_str(), ViewPosition);
			break;
		case DTYPE_HELL:
			LoadL4Dungeon(TestMapPath.c_str(), ViewPosition);
			break;
		case DTYPE_TOWN:
		case DTYPE_NONE:
			break;
		}
		LoadRndLvlPal(setlvltype);
		InitNoTriggers();
#endif
		break;
	}
}

} // namespace devilution
