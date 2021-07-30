/**
 * @file quests.cpp
 *
 * Implementation of functionality for handling quests.
 */
#include "quests.h"

#include <fmt/format.h>

#include "control.h"
#include "cursor.h"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "gendung.h"
#include "init.h"
#include "minitext.h"
#include "missiles.h"
#include "monster.h"
#include "options.h"
#include "stores.h"
#include "towners.h"
#include "trigs.h"
#include "utils/language.h"

namespace devilution {

bool QuestLogIsOpen;
std::optional<CelSprite> pQLogCel;
/** Contains the quests of the current game. */
QuestStruct Quests[MAXQUESTS];
int ReturnLvlX;
int ReturnLvlY;
dungeon_type ReturnLevelType;
int ReturnLevel;

/** Contains the data related to each quest_id. */
QuestDataStruct QuestData[] = {
	// clang-format off
	// _qdlvl,  _qdmultlvl, _qlvlt,          _qdtype,     _qdrnd, _qslvl,          isSinglePlayerOnly, _qdmsg,        _qlstr
	{       5,          -1, DTYPE_NONE,      Q_ROCK,      100,    SL_NONE,         true,               TEXT_INFRA5,   N_( /* TRANSLATORS: Quest Name Block */ "The Magic Rock")           },
	{       9,          -1, DTYPE_NONE,      Q_MUSHROOM,  100,    SL_NONE,         true,               TEXT_MUSH8,    N_("Black Mushroom")           },
	{       4,          -1, DTYPE_NONE,      Q_GARBUD,    100,    SL_NONE,         true,               TEXT_GARBUD1,  N_("Gharbad The Weak")         },
	{       8,          -1, DTYPE_NONE,      Q_ZHAR,      100,    SL_NONE,         true,               TEXT_ZHAR1,    N_("Zhar the Mad")             },
	{      14,          -1, DTYPE_NONE,      Q_VEIL,      100,    SL_NONE,         true,               TEXT_VEIL9,    "Lachdanan"                    },
	{      15,          -1, DTYPE_NONE,      Q_DIABLO,    100,    SL_NONE,         false,              TEXT_VILE3,    "Diablo"                       },
	{       2,           2, DTYPE_NONE,      Q_BUTCHER,   100,    SL_NONE,         false,              TEXT_BUTCH9,   N_("The Butcher")              },
	{       4,          -1, DTYPE_NONE,      Q_LTBANNER,  100,    SL_NONE,         true,               TEXT_BANNER2,  N_("Ogden's Sign")             },
	{       7,          -1, DTYPE_NONE,      Q_BLIND,     100,    SL_NONE,         true,               TEXT_BLINDING, N_("Halls of the Blind")       },
	{       5,          -1, DTYPE_NONE,      Q_BLOOD,     100,    SL_NONE,         true,               TEXT_BLOODY,   N_("Valor")                    },
	{      10,          -1, DTYPE_NONE,      Q_ANVIL,     100,    SL_NONE,         true,               TEXT_ANVIL5,   N_("Anvil of Fury")            },
	{      13,          -1, DTYPE_NONE,      Q_WARLORD,   100,    SL_NONE,         true,               TEXT_BLOODWAR, N_("Warlord of Blood")         },
	{       3,           3, DTYPE_CATHEDRAL, Q_SKELKING,  100,    SL_SKELKING,     false,              TEXT_KING2,    N_("The Curse of King Leoric") },
	{       2,          -1, DTYPE_CAVES,     Q_PWATER,    100,    SL_POISONWATER,  true,               TEXT_POISON3,  N_("Poisoned Water Supply")    },
	{       6,          -1, DTYPE_CATACOMBS, Q_SCHAMB,    100,    SL_BONECHAMB,    true,               TEXT_BONER,    N_("The Chamber of Bone")      },
	{      15,          15, DTYPE_CATHEDRAL, Q_BETRAYER,  100,    SL_VILEBETRAYER, false,              TEXT_VILE1,    N_("Archbishop Lazarus")       },
	{      17,          17, DTYPE_NONE,      Q_GRAVE,     100,    SL_NONE,         false,              TEXT_GRAVE7,   N_("Grave Matters")            },
	{      9,            9, DTYPE_NONE,      Q_FARMER,    100,    SL_NONE,         false,              TEXT_FARMER1,  N_("Farmer's Orchard")         },
	{      17,          -1, DTYPE_NONE,      Q_GIRL,      100,    SL_NONE,         true,               TEXT_GIRL2,    N_("Little Girl")              },
	{      19,          -1, DTYPE_NONE,      Q_TRADER,    100,    SL_NONE,         true,               TEXT_TRADER,   N_("Wandering Trader")         },
	{      17,          17, DTYPE_NONE,      Q_DEFILER,   100,    SL_NONE,         false,              TEXT_DEFILER1, N_("The Defiler")              },
	{      21,          21, DTYPE_NONE,      Q_NAKRUL,    100,    SL_NONE,         false,              TEXT_NAKRUL1,  "Na-Krul"                      },
	{      21,          -1, DTYPE_NONE,      Q_CORNSTN,   100,    SL_NONE,         true,               TEXT_CORNSTN,  N_("Cornerstone of the World") },
	{       9,           9, DTYPE_NONE,      Q_JERSEY,    100,    SL_NONE,         false,              TEXT_JERSEY4,  N_( /* TRANSLATORS: Quest Name Block end*/ "The Jersey's Jersey")      },
	// clang-format on
};

namespace {

int qtopline;
int qline;
int qlist[MAXQUESTS];
int numqlines;
int WaterDone;

/**
 * Specifies a delta in X-coordinates from the quest entrance for
 * which the hover text of the cursor will be visible.
 */
char questxoff[7] = { 0, -1, 0, -1, -2, -1, -2 };
/**
 * Specifies a delta in Y-coordinates from the quest entrance for
 * which the hover text of the cursor will be visible.
 */
char questyoff[7] = { 0, 0, -1, -1, -1, -2, -2 };
const char *const QuestTriggerNames[5] = {
	N_(/* TRANSLATORS: Quest Map*/ "King Leoric's Tomb"),
	N_(/* TRANSLATORS: Quest Map*/ "The Chamber of Bone"),
	N_(/* TRANSLATORS: Quest Map*/ "Maze"),
	N_(/* TRANSLATORS: Quest Map*/ "A Dark Passage"),
	N_(/* TRANSLATORS: Quest Map*/ "Unholy Altar")
};
/**
 * A quest group containing the three quests the Butcher,
 * Ogden's Sign and Gharbad the Weak, which ensures that exactly
 * two of these three quests appear in any single player game.
 */
int QuestGroup1[3] = { Q_BUTCHER, Q_LTBANNER, Q_GARBUD };
/**
 * A quest group containing the three quests Halls of the Blind,
 * the Magic Rock and Valor, which ensures that exactly two of
 * these three quests appear in any single player game.
 */
int QuestGroup2[3] = { Q_BLIND, Q_ROCK, Q_BLOOD };
/**
 * A quest group containing the three quests Black Mushroom,
 * Zhar the Mad and Anvil of Fury, which ensures that exactly
 * two of these three quests appear in any single player game.
 */
int QuestGroup3[3] = { Q_MUSHROOM, Q_ZHAR, Q_ANVIL };
/**
 * A quest group containing the two quests Lachdanan and Warlord
 * of Blood, which ensures that exactly one of these two quests
 * appears in any single player game.
 */
int QuestGroup4[2] = { Q_VEIL, Q_WARLORD };

void DrawButcher()
{
	int x = 2 * setpc_x + 16;
	int y = 2 * setpc_y + 16;
	DRLG_RectTrans(x + 3, y + 3, x + 10, y + 10);
}

void DrawSkelKing(int q, int x, int y)
{
	Quests[q].position = { 2 * x + 28, 2 * y + 23 };
}

void DrawWarLord(int x, int y)
{
	auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\Warlord2.DUN");

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	setpc_x = x;
	setpc_y = y;
	setpc_w = width;
	setpc_h = height;

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(tileLayer[j * width + i]);
			dungeon[x + i][y + j] = (tileId != 0) ? tileId : 6;
		}
	}
}

void DrawSChamber(int q, int x, int y)
{
	auto dunData = LoadFileInMem<uint16_t>("Levels\\L2Data\\Bonestr1.DUN");

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	setpc_x = x;
	setpc_y = y;
	setpc_w = width;
	setpc_h = height;

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(tileLayer[j * width + i]);
			dungeon[x + i][y + j] = (tileId != 0) ? tileId : 3;
		}
	}

	Quests[q].position = { 2 * x + 22, 2 * y + 23 };
}

void DrawLTBanner(int x, int y)
{
	auto dunData = LoadFileInMem<uint16_t>("Levels\\L1Data\\Banner1.DUN");

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	setpc_x = x;
	setpc_y = y;
	setpc_w = width;
	setpc_h = height;

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(tileLayer[j * width + i]);
			if (tileId != 0) {
				pdungeon[x + i][y + j] = tileId;
			}
		}
	}
}

void DrawBlind(int x, int y)
{
	auto dunData = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blind1.DUN");

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	setpc_x = x;
	setpc_y = y;
	setpc_w = width;
	setpc_h = height;

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(tileLayer[j * width + i]);
			if (tileId != 0) {
				pdungeon[x + i][y + j] = tileId;
			}
		}
	}
}

void DrawBlood(int x, int y)
{
	auto dunData = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blood2.DUN");

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	setpc_x = x;
	setpc_y = y;
	setpc_w = width;
	setpc_h = height;

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(tileLayer[j * width + i]);
			if (tileId != 0) {
				dungeon[x + i][y + j] = tileId;
			}
		}
	}
}

void PrintQLString(const Surface &out, int x, int line, const char *str)
{
	int width = GetLineWidth(str);
	int sx = x + std::max((257 - width) / 2, 0);
	int sy = line * 12 + 44;
	if (qline == line) {
		CelDrawTo(out, { sx - 20, sy + 1 }, *pSPentSpn2Cels, PentSpn2Spin());
	}
	DrawString(out, str, { { sx, sy }, { 257, 0 } }, UiFlags::ColorSilver);
	if (qline == line) {
		CelDrawTo(out, { sx + width + 7, sy + 1 }, *pSPentSpn2Cels, PentSpn2Spin());
	}
}

} // namespace

void InitQuests()
{
	if (!gbIsMultiplayer) {
		for (auto &quest : Quests) {
			quest._qactive = QUEST_NOTAVAIL;
		}
	} else {
		for (int i = 0; i < MAXQUESTS; i++) {
			if (QuestData[i].isSinglePlayerOnly) {
				Quests[i]._qactive = QUEST_NOTAVAIL;
			}
		}
	}

	QuestDialogTable[TOWN_HEALER][Q_MUSHROOM] = TEXT_NONE;
	QuestDialogTable[TOWN_WITCH][Q_MUSHROOM] = TEXT_MUSH9;

	QuestLogIsOpen = false;
	WaterDone = 0;
	int initiatedQuests = 0;

	for (int i = 0; i < MAXQUESTS; i++) {
		if (gbIsMultiplayer && QuestData[i].isSinglePlayerOnly)
			continue;
		Quests[i]._qtype = QuestData[i]._qdtype;
		if (gbIsMultiplayer) {
			Quests[i]._qlevel = QuestData[i]._qdmultlvl;
			if (!delta_quest_inited(initiatedQuests)) {
				Quests[i]._qactive = QUEST_INIT;
				Quests[i]._qvar1 = 0;
				Quests[i]._qlog = false;
			}
			initiatedQuests++;
		} else {
			Quests[i]._qactive = QUEST_INIT;
			Quests[i]._qlevel = QuestData[i]._qdlvl;
			Quests[i]._qvar1 = 0;
			Quests[i]._qlog = false;
		}

		Quests[i]._qslvl = QuestData[i]._qslvl;
		Quests[i].position = { 0, 0 };
		Quests[i]._qidx = i;
		Quests[i]._qlvltype = QuestData[i]._qlvlt;
		Quests[i]._qvar2 = 0;
		Quests[i]._qmsg = QuestData[i]._qdmsg;
	}

	if (!gbIsMultiplayer && sgOptions.Gameplay.bRandomizeQuests) {
		// Quests are set from the seed used to generate level 16.
		InitialiseQuestPools(glSeedTbl[15], Quests);
	}
#ifdef _DEBUG
	if (questdebug != -1)
		Quests[questdebug]._qactive = QUEST_ACTIVE;
#endif

	if (gbIsSpawn) {
		for (auto &quest : Quests) {
			quest._qactive = QUEST_NOTAVAIL;
		}
	}

	if (Quests[Q_SKELKING]._qactive == QUEST_NOTAVAIL)
		Quests[Q_SKELKING]._qvar2 = 2;
	if (Quests[Q_ROCK]._qactive == QUEST_NOTAVAIL)
		Quests[Q_ROCK]._qvar2 = 2;
	Quests[Q_LTBANNER]._qvar1 = 1;
	if (gbIsMultiplayer)
		Quests[Q_BETRAYER]._qvar1 = 2;
}

void InitialiseQuestPools(uint32_t seed, QuestStruct quests[])
{
	SetRndSeed(seed);
	if (GenerateRnd(2) != 0)
		quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;
	else
		quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL;

	quests[QuestGroup1[GenerateRnd(sizeof(QuestGroup1) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
	quests[QuestGroup2[GenerateRnd(sizeof(QuestGroup2) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
	quests[QuestGroup3[GenerateRnd(sizeof(QuestGroup3) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
	quests[QuestGroup4[GenerateRnd(sizeof(QuestGroup4) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
}

void CheckQuests()
{
	if (gbIsSpawn)
		return;

	if (QuestStatus(Q_BETRAYER) && gbIsMultiplayer && Quests[Q_BETRAYER]._qvar1 == 2) {
		AddObject(OBJ_ALTBOY, { 2 * setpc_x + 20, 2 * setpc_y + 22 });
		Quests[Q_BETRAYER]._qvar1 = 3;
		NetSendCmdQuest(true, Q_BETRAYER);
	}

	if (gbIsMultiplayer) {
		return;
	}

	if (currlevel == Quests[Q_BETRAYER]._qlevel
	    && !setlevel
	    && Quests[Q_BETRAYER]._qvar1 >= 2
	    && (Quests[Q_BETRAYER]._qactive == QUEST_ACTIVE || Quests[Q_BETRAYER]._qactive == QUEST_DONE)
	    && (Quests[Q_BETRAYER]._qvar2 == 0 || Quests[Q_BETRAYER]._qvar2 == 2)) {
		Quests[Q_BETRAYER].position.x = 2 * Quests[Q_BETRAYER].position.x + 16;
		Quests[Q_BETRAYER].position.y = 2 * Quests[Q_BETRAYER].position.y + 16;
		int rportx = Quests[Q_BETRAYER].position.x;
		int rporty = Quests[Q_BETRAYER].position.y;
		AddMissile({ rportx, rporty }, { rportx, rporty }, 0, MIS_RPORTAL, TARGET_MONSTERS, MyPlayerId, 0, 0);
		Quests[Q_BETRAYER]._qvar2 = 1;
		if (Quests[Q_BETRAYER]._qactive == QUEST_ACTIVE) {
			Quests[Q_BETRAYER]._qvar1 = 3;
		}
	}

	if (Quests[Q_BETRAYER]._qactive == QUEST_DONE
	    && setlevel
	    && setlvlnum == SL_VILEBETRAYER
	    && Quests[Q_BETRAYER]._qvar2 == 4) {
		int rportx = 35;
		int rporty = 32;
		AddMissile({ rportx, rporty }, { rportx, rporty }, 0, MIS_RPORTAL, TARGET_MONSTERS, MyPlayerId, 0, 0);
		Quests[Q_BETRAYER]._qvar2 = 3;
	}

	if (setlevel) {
		if (setlvlnum == Quests[Q_PWATER]._qslvl
		    && Quests[Q_PWATER]._qactive != QUEST_INIT
		    && leveltype == Quests[Q_PWATER]._qlvltype
		    && ActiveMonsterCount == 4
		    && Quests[Q_PWATER]._qactive != QUEST_DONE) {
			Quests[Q_PWATER]._qactive = QUEST_DONE;
			PlaySfxLoc(IS_QUESTDN, Players[MyPlayerId].position.tile);
			LoadPalette("Levels\\L3Data\\L3pwater.pal", false);
			UpdatePWaterPalette();
			WaterDone = 32;
		}
	} else if (Players[MyPlayerId]._pmode == PM_STAND) {
		for (auto &quest : Quests) {
			if (currlevel == quest._qlevel
			    && quest._qslvl != 0
			    && quest._qactive != QUEST_NOTAVAIL
			    && Players[MyPlayerId].position.tile == quest.position) {
				if (quest._qlvltype != DTYPE_NONE) {
					setlvltype = quest._qlvltype;
				}
				StartNewLvl(MyPlayerId, WM_DIABSETLVL, quest._qslvl);
			}
		}
	}
}

bool ForceQuests()
{
	if (gbIsSpawn)
		return false;

	if (gbIsMultiplayer) {
		return false;
	}

	for (int i = 0; i < MAXQUESTS; i++) {
		if (i != Q_BETRAYER && currlevel == Quests[i]._qlevel && Quests[i]._qslvl != 0) {
			int ql = Quests[Quests[i]._qidx]._qslvl - 1;
			int qx = Quests[i].position.x;
			int qy = Quests[i].position.y;

			for (int j = 0; j < 7; j++) {
				if (qx + questxoff[j] == cursmx && qy + questyoff[j] == cursmy) {
					strcpy(infostr, fmt::format(_(/* TRANSLATORS: Used for Quest Portals. {:s} is a Map Name */ "To {:s}"), _(QuestTriggerNames[ql])).c_str());
					cursmx = qx;
					cursmy = qy;
					return true;
				}
			}
		}
	}

	return false;
}

bool QuestStatus(int i)
{
	if (setlevel)
		return false;
	if (currlevel != Quests[i]._qlevel)
		return false;
	if (Quests[i]._qactive == QUEST_NOTAVAIL)
		return false;
	if (gbIsMultiplayer && QuestData[i].isSinglePlayerOnly)
		return false;
	return true;
}

void CheckQuestKill(const MonsterStruct &monster, bool sendmsg)
{
	if (gbIsSpawn)
		return;

	auto &myPlayer = Players[MyPlayerId];

	if (monster.MType->mtype == MT_SKING) {
		Quests[Q_SKELKING]._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::RestWellLeoricIllFindYourSon, 30);
		if (sendmsg)
			NetSendCmdQuest(true, Q_SKELKING);

	} else if (monster.MType->mtype == MT_CLEAVER) {
		Quests[Q_BUTCHER]._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::TheSpiritsOfTheDeadAreNowAvenged, 30);
		if (sendmsg)
			NetSendCmdQuest(true, Q_BUTCHER);
	} else if (monster._uniqtype - 1 == UMT_GARBUD) { //"Gharbad the Weak"
		Quests[Q_GARBUD]._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::ImNotImpressed, 30);
	} else if (monster._uniqtype - 1 == UMT_ZHAR) { //"Zhar the Mad"
		Quests[Q_ZHAR]._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::ImSorryDidIBreakYourConcentration, 30);
	} else if (monster._uniqtype - 1 == UMT_LAZARUS && gbIsMultiplayer) { //"Arch-Bishop Lazarus"
		Quests[Q_BETRAYER]._qactive = QUEST_DONE;
		Quests[Q_BETRAYER]._qvar1 = 7;
		Quests[Q_DIABLO]._qactive = QUEST_ACTIVE;

		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] == 370) {
					trigs[numtrigs].position = { i, j };
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
			}
		}
		myPlayer.Say(HeroSpeech::YourMadnessEndsHereBetrayer, 30);
		if (sendmsg) {
			NetSendCmdQuest(true, Q_BETRAYER);
			NetSendCmdQuest(true, Q_DIABLO);
		}
	} else if (monster._uniqtype - 1 == UMT_LAZARUS && !gbIsMultiplayer) { //"Arch-Bishop Lazarus"
		Quests[Q_BETRAYER]._qactive = QUEST_DONE;
		InitVPTriggers();
		Quests[Q_BETRAYER]._qvar1 = 7;
		Quests[Q_BETRAYER]._qvar2 = 4;
		Quests[Q_DIABLO]._qactive = QUEST_ACTIVE;
		AddMissile({ 35, 32 }, { 35, 32 }, 0, MIS_RPORTAL, TARGET_MONSTERS, MyPlayerId, 0, 0);
		myPlayer.Say(HeroSpeech::YourMadnessEndsHereBetrayer, 30);
	} else if (monster._uniqtype - 1 == UMT_WARLORD) { //"Warlord of Blood"
		Quests[Q_WARLORD]._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::YourReignOfPainHasEnded, 30);
	}
}

void DRLG_CheckQuests(int x, int y)
{
	for (int i = 0; i < MAXQUESTS; i++) {
		if (QuestStatus(i)) {
			switch (Quests[i]._qtype) {
			case Q_BUTCHER:
				DrawButcher();
				break;
			case Q_LTBANNER:
				DrawLTBanner(x, y);
				break;
			case Q_BLIND:
				DrawBlind(x, y);
				break;
			case Q_BLOOD:
				DrawBlood(x, y);
				break;
			case Q_WARLORD:
				DrawWarLord(x, y);
				break;
			case Q_SKELKING:
				DrawSkelKing(i, x, y);
				break;
			case Q_SCHAMB:
				DrawSChamber(i, x, y);
				break;
			}
		}
	}
}

void SetReturnLvlPos()
{
	switch (setlvlnum) {
	case SL_SKELKING:
		ReturnLvlX = Quests[Q_SKELKING].position.x + 1;
		ReturnLvlY = Quests[Q_SKELKING].position.y;
		ReturnLevel = Quests[Q_SKELKING]._qlevel;
		ReturnLevelType = DTYPE_CATHEDRAL;
		break;
	case SL_BONECHAMB:
		ReturnLvlX = Quests[Q_SCHAMB].position.x + 1;
		ReturnLvlY = Quests[Q_SCHAMB].position.y;
		ReturnLevel = Quests[Q_SCHAMB]._qlevel;
		ReturnLevelType = DTYPE_CATACOMBS;
		break;
	case SL_MAZE:
		break;
	case SL_POISONWATER:
		ReturnLvlX = Quests[Q_PWATER].position.x;
		ReturnLvlY = Quests[Q_PWATER].position.y + 1;
		ReturnLevel = Quests[Q_PWATER]._qlevel;
		ReturnLevelType = DTYPE_CATHEDRAL;
		break;
	case SL_VILEBETRAYER:
		ReturnLvlX = Quests[Q_BETRAYER].position.x + 1;
		ReturnLvlY = Quests[Q_BETRAYER].position.y - 1;
		ReturnLevel = Quests[Q_BETRAYER]._qlevel;
		ReturnLevelType = DTYPE_HELL;
		break;
	case SL_NONE:
		break;
	}
}

void GetReturnLvlPos()
{
	if (Quests[Q_BETRAYER]._qactive == QUEST_DONE)
		Quests[Q_BETRAYER]._qvar2 = 2;
	ViewX = ReturnLvlX;
	ViewY = ReturnLvlY;
	currlevel = ReturnLevel;
	leveltype = ReturnLevelType;
}

void LoadPWaterPalette()
{
	if (!setlevel || setlvlnum != Quests[Q_PWATER]._qslvl || Quests[Q_PWATER]._qactive == QUEST_INIT || leveltype != Quests[Q_PWATER]._qlvltype)
		return;

	if (Quests[Q_PWATER]._qactive == QUEST_DONE)
		LoadPalette("Levels\\L3Data\\L3pwater.pal");
	else
		LoadPalette("Levels\\L3Data\\L3pfoul.pal");
}

void UpdatePWaterPalette()
{
	if (WaterDone > 0) {
		palette_update_quest_palette(WaterDone);
		WaterDone--;
		return;
	}
	palette_update_caves();
}

void ResyncMPQuests()
{
	if (gbIsSpawn)
		return;

	if (Quests[Q_SKELKING]._qactive == QUEST_INIT
	    && currlevel >= Quests[Q_SKELKING]._qlevel - 1
	    && currlevel <= Quests[Q_SKELKING]._qlevel + 1) {
		Quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_SKELKING);
	}
	if (Quests[Q_BUTCHER]._qactive == QUEST_INIT
	    && currlevel >= Quests[Q_BUTCHER]._qlevel - 1
	    && currlevel <= Quests[Q_BUTCHER]._qlevel + 1) {
		Quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_BUTCHER);
	}
	if (Quests[Q_BETRAYER]._qactive == QUEST_INIT && currlevel == Quests[Q_BETRAYER]._qlevel - 1) {
		Quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_BETRAYER);
	}
	if (QuestStatus(Q_BETRAYER))
		AddObject(OBJ_ALTBOY, { 2 * setpc_x + 20, 2 * setpc_y + 22 });
	if (Quests[Q_GRAVE]._qactive == QUEST_INIT && currlevel == Quests[Q_GRAVE]._qlevel - 1) {
		Quests[Q_GRAVE]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_GRAVE);
	}
	if (Quests[Q_DEFILER]._qactive == QUEST_INIT && currlevel == Quests[Q_DEFILER]._qlevel - 1) {
		Quests[Q_DEFILER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_DEFILER);
	}
	if (Quests[Q_NAKRUL]._qactive == QUEST_INIT && currlevel == Quests[Q_NAKRUL]._qlevel - 1) {
		Quests[Q_NAKRUL]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_NAKRUL);
	}
	if (Quests[Q_JERSEY]._qactive == QUEST_INIT && currlevel == Quests[Q_JERSEY]._qlevel - 1) {
		Quests[Q_JERSEY]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_JERSEY);
	}
}

void ResyncQuests()
{
	if (gbIsSpawn)
		return;

	if (QuestStatus(Q_LTBANNER)) {
		if (Quests[Q_LTBANNER]._qvar1 == 1) {
			ObjChangeMapResync(
			    setpc_w + setpc_x - 2,
			    setpc_h + setpc_y - 2,
			    setpc_w + setpc_x + 1,
			    setpc_h + setpc_y + 1);
		}
		if (Quests[Q_LTBANNER]._qvar1 == 2) {
			ObjChangeMapResync(
			    setpc_w + setpc_x - 2,
			    setpc_h + setpc_y - 2,
			    setpc_w + setpc_x + 1,
			    setpc_h + setpc_y + 1);
			ObjChangeMapResync(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 2, (setpc_h / 2) + setpc_y - 2);
			for (int i = 0; i < ActiveObjectCount; i++)
				SyncObjectAnim(Objects[ActiveObjects[i]]);
			auto tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 4, setpc_y + (setpc_h / 2));
			TransVal = tren;
		}
		if (Quests[Q_LTBANNER]._qvar1 == 3) {
			int x = setpc_x;
			int y = setpc_y;
			ObjChangeMapResync(x, y, x + setpc_w + 1, y + setpc_h + 1);
			for (int i = 0; i < ActiveObjectCount; i++)
				SyncObjectAnim(Objects[ActiveObjects[i]]);
			auto tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 4, setpc_y + (setpc_h / 2));
			TransVal = tren;
		}
	}
	if (currlevel == Quests[Q_MUSHROOM]._qlevel) {
		if (Quests[Q_MUSHROOM]._qactive == QUEST_INIT && Quests[Q_MUSHROOM]._qvar1 == QS_INIT) {
			SpawnQuestItem(IDI_FUNGALTM, { 0, 0 }, 5, 1);
			Quests[Q_MUSHROOM]._qvar1 = QS_TOMESPAWNED;
		} else {
			if (Quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
				if (Quests[Q_MUSHROOM]._qvar1 >= QS_MUSHGIVEN) {
					QuestDialogTable[TOWN_WITCH][Q_MUSHROOM] = TEXT_NONE;
					QuestDialogTable[TOWN_HEALER][Q_MUSHROOM] = TEXT_MUSH3;
				} else if (Quests[Q_MUSHROOM]._qvar1 >= QS_BRAINGIVEN) {
					QuestDialogTable[TOWN_HEALER][Q_MUSHROOM] = TEXT_NONE;
				}
			}
		}
	}
	if (currlevel == Quests[Q_VEIL]._qlevel + 1 && Quests[Q_VEIL]._qactive == QUEST_ACTIVE && Quests[Q_VEIL]._qvar1 == 0) {
		Quests[Q_VEIL]._qvar1 = 1;
		SpawnQuestItem(IDI_GLDNELIX, { 0, 0 }, 5, 1);
	}
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		if (Quests[Q_BETRAYER]._qvar1 >= 4)
			ObjChangeMapResync(1, 11, 20, 18);
		if (Quests[Q_BETRAYER]._qvar1 >= 6)
			ObjChangeMapResync(1, 18, 20, 24);
		if (Quests[Q_BETRAYER]._qvar1 >= 7)
			InitVPTriggers();
		for (int i = 0; i < ActiveObjectCount; i++)
			SyncObjectAnim(Objects[ActiveObjects[i]]);
	}
	if (currlevel == Quests[Q_BETRAYER]._qlevel
	    && !setlevel
	    && (Quests[Q_BETRAYER]._qvar2 == 1 || Quests[Q_BETRAYER]._qvar2 >= 3)
	    && (Quests[Q_BETRAYER]._qactive == QUEST_ACTIVE || Quests[Q_BETRAYER]._qactive == QUEST_DONE)) {
		Quests[Q_BETRAYER]._qvar2 = 2;
	}
}

void DrawQuestLog(const Surface &out)
{
	DrawString(out, _("Quest Log"), { { 32, 44 }, { 257, 0 } }, UiFlags::AlignCenter);
	CelDrawTo(out, { 0, 351 }, *pQLogCel, 1);
	int line = qtopline;
	for (int i = 0; i < numqlines; i++) {
		PrintQLString(out, 32, line, _(QuestData[qlist[i]]._qlstr));
		line += 2;
	}
	PrintQLString(out, 32, 22, _("Close Quest Log"));
}

void StartQuestlog()
{
	numqlines = 0;
	for (int i = 0; i < MAXQUESTS; i++) {
		if (Quests[i]._qactive == QUEST_ACTIVE && Quests[i]._qlog) {
			qlist[numqlines] = i;
			numqlines++;
		}
	}
	if (numqlines > 5) {
		qtopline = 5 - (numqlines / 2);
	} else {
		qtopline = 8;
	}
	qline = 22;
	if (numqlines != 0)
		qline = qtopline;
	QuestLogIsOpen = true;
}

void QuestlogUp()
{
	if (numqlines != 0) {
		if (qline == qtopline) {
			qline = 22;
		} else if (qline == 22) {
			qline = qtopline + 2 * numqlines - 2;
		} else {
			qline -= 2;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

void QuestlogDown()
{
	if (numqlines != 0) {
		if (qline == 22) {
			qline = qtopline;
		} else if (qline == qtopline + 2 * numqlines - 2) {
			qline = 22;
		} else {
			qline += 2;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

void QuestlogEnter()
{
	PlaySFX(IS_TITLSLCT);
	if (numqlines != 0 && qline != 22)
		InitQTextMsg(Quests[qlist[(qline - qtopline) / 2]]._qmsg);
	QuestLogIsOpen = false;
}

void QuestlogESC()
{
	int y = (MousePosition.y - 32) / 12;
	for (int i = 0; i < numqlines; i++) {
		if (y == qtopline + 2 * i) {
			qline = y;
			QuestlogEnter();
		}
	}
	if (y == 22) {
		qline = 22;
		QuestlogEnter();
	}
}

void SetMultiQuest(int q, quest_state s, bool log, int v1)
{
	if (gbIsSpawn)
		return;

	if (Quests[q]._qactive != QUEST_DONE) {
		if (s > Quests[q]._qactive)
			Quests[q]._qactive = s;
		if (log)
			Quests[q]._qlog = true;
		if (v1 > Quests[q]._qvar1)
			Quests[q]._qvar1 = v1;
	}
}

} // namespace devilution
