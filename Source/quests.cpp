/**
 * @file quests.cpp
 *
 * Implementation of functionality for handling quests.
 */
#include "quests.h"

#include <fmt/format.h>

#include "DiabloUI/ui_flags.hpp"
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
#include "panels/ui_panels.hpp"
#include "stores.h"
#include "towners.h"
#include "trigs.h"
#include "utils/language.h"

namespace devilution {

bool QuestLogIsOpen;
std::optional<CelSprite> pQLogCel;
/** Contains the quests of the current game. */
Quest Quests[MAXQUESTS];
Point ReturnLvlPosition;
dungeon_type ReturnLevelType;
int ReturnLevel;

/** Contains the data related to each quest_id. */
QuestData QuestsData[] = {
	// clang-format off
	// _qdlvl,  _qdmultlvl, _qlvlt,          bookOrder,   _qdrnd, _qslvl,          isSinglePlayerOnly, _qdmsg,        _qlstr
	{       5,          -1, DTYPE_NONE,          5,      100,    SL_NONE,         true,               TEXT_INFRA5,   N_( /* TRANSLATORS: Quest Name Block */ "The Magic Rock")           },
	{       9,          -1, DTYPE_NONE,         10,      100,    SL_NONE,         true,               TEXT_MUSH8,    N_("Black Mushroom")           },
	{       4,          -1, DTYPE_NONE,          3,      100,    SL_NONE,         true,               TEXT_GARBUD1,  N_("Gharbad The Weak")         },
	{       8,          -1, DTYPE_NONE,          9,      100,    SL_NONE,         true,               TEXT_ZHAR1,    N_("Zhar the Mad")             },
	{      14,          -1, DTYPE_NONE,         21,      100,    SL_NONE,         true,               TEXT_VEIL9,    N_("Lachdanan")                },
	{      15,          -1, DTYPE_NONE,         23,      100,    SL_NONE,         false,              TEXT_VILE3,    N_("Diablo")                   },
	{       2,           2, DTYPE_NONE,          0,      100,    SL_NONE,         false,              TEXT_BUTCH9,   N_("The Butcher")              },
	{       4,          -1, DTYPE_NONE,          4,      100,    SL_NONE,         true,               TEXT_BANNER2,  N_("Ogden's Sign")             },
	{       7,          -1, DTYPE_NONE,          8,      100,    SL_NONE,         true,               TEXT_BLINDING, N_("Halls of the Blind")       },
	{       5,          -1, DTYPE_NONE,          6,      100,    SL_NONE,         true,               TEXT_BLOODY,   N_("Valor")                    },
	{      10,          -1, DTYPE_NONE,         11,      100,    SL_NONE,         true,               TEXT_ANVIL5,   N_("Anvil of Fury")            },
	{      13,          -1, DTYPE_NONE,         20,      100,    SL_NONE,         true,               TEXT_BLOODWAR, N_("Warlord of Blood")         },
	{       3,           3, DTYPE_CATHEDRAL,     2,      100,    SL_SKELKING,     false,              TEXT_KING2,    N_("The Curse of King Leoric") },
	{       2,          -1, DTYPE_CAVES,         1,      100,    SL_POISONWATER,  true,               TEXT_POISON3,  N_("Poisoned Water Supply")    },
	{       6,          -1, DTYPE_CATACOMBS,     7,      100,    SL_BONECHAMB,    true,               TEXT_BONER,    N_("The Chamber of Bone")      },
	{      15,          15, DTYPE_CATHEDRAL,    22,      100,    SL_VILEBETRAYER, false,              TEXT_VILE1,    N_("Archbishop Lazarus")       },
	{      17,          17, DTYPE_NONE,         17,      100,    SL_NONE,         false,              TEXT_GRAVE7,   N_("Grave Matters")            },
	{      9,            9, DTYPE_NONE,         12,      100,    SL_NONE,         false,              TEXT_FARMER1,  N_("Farmer's Orchard")         },
	{      17,          -1, DTYPE_NONE,         14,      100,    SL_NONE,         true,               TEXT_GIRL2,    N_("Little Girl")              },
	{      19,          -1, DTYPE_NONE,         16,      100,    SL_NONE,         true,               TEXT_TRADER,   N_("Wandering Trader")         },
	{      17,          17, DTYPE_NONE,         15,      100,    SL_NONE,         false,              TEXT_DEFILER1, N_("The Defiler")              },
	{      21,          21, DTYPE_NONE,         19,      100,    SL_NONE,         false,              TEXT_NAKRUL1,  N_("Na-Krul")                  },
	{      21,          -1, DTYPE_NONE,         18,      100,    SL_NONE,         true,               TEXT_CORNSTN,  N_("Cornerstone of the World") },
	{       9,           9, DTYPE_NONE,         13,      100,    SL_NONE,         false,              TEXT_JERSEY4,  N_( /* TRANSLATORS: Quest Name Block end*/ "The Jersey's Jersey")      },
	// clang-format on
};

namespace {

int WaterDone;

/** Indices of quests to display in quest log window. `FirstFinishedQuest` are active quests the rest are completed */
quest_id EncounteredQuests[MAXQUESTS];
/** Overall number of EncounteredQuests entries */
int EncounteredQuestCount;
/** First (nonselectable) finished quest in list */
int FirstFinishedQuest;
/** Currently selected quest list item */
int SelectedQuest;

constexpr Rectangle InnerPanel { { 32, 26 }, { 280, 300 } };
constexpr int LineHeight = 12;
constexpr int MaxSpacing = LineHeight * 2;
int ListYOffset;
int LineSpacing;
/** The number of pixels to move finished quest, to seperate them from the active ones */
int FinishedQuestOffset;

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

void DrawSkelKing(quest_id q, int x, int y)
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

void DrawSChamber(quest_id q, int x, int y)
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

int QuestLogMouseToEntry()
{
	Rectangle innerArea = InnerPanel;
	innerArea.position += Displacement(GetLeftPanel().position.x, GetLeftPanel().position.y);
	if (!innerArea.Contains(MousePosition) || (EncounteredQuestCount == 0))
		return -1;
	int y = MousePosition.y - innerArea.position.y;
	for (int i = 0; i < FirstFinishedQuest; i++) {
		if ((y >= ListYOffset + i * LineSpacing)
		    && (y < ListYOffset + i * LineSpacing + LineHeight)) {
			return i;
		}
	}
	return -1;
}

void PrintQLString(const Surface &out, int x, int y, const char *str, bool marked, bool disabled = false)
{
	int width = GetLineWidth(str);
	x += std::max((257 - width) / 2, 0);
	if (marked) {
		CelDrawTo(out, GetPanelPosition(UiPanels::Quest, { x - 20, y + 13 }), *pSPentSpn2Cels, PentSpn2Spin());
	}
	DrawString(out, str, { GetPanelPosition(UiPanels::Quest, { x, y }), { 257, 0 } }, disabled ? UiFlags::ColorWhitegold : UiFlags::ColorWhite);
	if (marked) {
		CelDrawTo(out, GetPanelPosition(UiPanels::Quest, { x + width + 7, y + 13 }), *pSPentSpn2Cels, PentSpn2Spin());
	}
}

} // namespace

void InitQuests()
{
	QuestDialogTable[TOWN_HEALER][Q_MUSHROOM] = TEXT_NONE;
	QuestDialogTable[TOWN_WITCH][Q_MUSHROOM] = TEXT_MUSH9;

	QuestLogIsOpen = false;
	WaterDone = 0;

	int initiatedQuests = 0;
	int q = 0;
	for (auto &quest : Quests) {
		quest._qidx = static_cast<quest_id>(q);
		auto &questData = QuestsData[q];
		q++;

		quest._qactive = QUEST_NOTAVAIL;
		quest.position = { 0, 0 };
		quest._qlvltype = questData._qlvlt;
		quest._qslvl = questData._qslvl;
		quest._qvar1 = 0;
		quest._qvar2 = 0;
		quest._qlog = false;
		quest._qmsg = questData._qdmsg;

		if (!gbIsMultiplayer) {
			quest._qlevel = questData._qdlvl;
			quest._qactive = QUEST_INIT;
		} else if (!questData.isSinglePlayerOnly) {
			quest._qlevel = questData._qdmultlvl;
			if (!delta_quest_inited(initiatedQuests)) {
				quest._qactive = QUEST_INIT;
			}
			initiatedQuests++;
		}
	}

	if (!gbIsMultiplayer && *sgOptions.Gameplay.randomizeQuests) {
		// Quests are set from the seed used to generate level 16.
		InitialiseQuestPools(glSeedTbl[15], Quests);
	}

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

void InitialiseQuestPools(uint32_t seed, Quest quests[])
{
	SetRndSeed(seed);
	if (GenerateRnd(2) != 0)
		quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;
	else
		quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL;

	// using int and not size_t here to detect negative values from GenerateRnd
	int randomIndex = GenerateRnd(sizeof(QuestGroup1) / sizeof(*QuestGroup1));

	if (randomIndex >= 0)
		quests[QuestGroup1[randomIndex]]._qactive = QUEST_NOTAVAIL;

	randomIndex = GenerateRnd(sizeof(QuestGroup2) / sizeof(*QuestGroup2));
	if (randomIndex >= 0)
		quests[QuestGroup2[randomIndex]]._qactive = QUEST_NOTAVAIL;

	randomIndex = GenerateRnd(sizeof(QuestGroup3) / sizeof(*QuestGroup3));
	if (randomIndex >= 0)
		quests[QuestGroup3[randomIndex]]._qactive = QUEST_NOTAVAIL;

	randomIndex = GenerateRnd(sizeof(QuestGroup4) / sizeof(*QuestGroup4));

	// always true, QuestGroup4 has two members
	if (randomIndex >= 0)
		quests[QuestGroup4[randomIndex]]._qactive = QUEST_NOTAVAIL;
}

void CheckQuests()
{
	if (gbIsSpawn)
		return;

	auto &quest = Quests[Q_BETRAYER];
	if (quest.IsAvailable() && gbIsMultiplayer && quest._qvar1 == 2) {
		AddObject(OBJ_ALTBOY, { 2 * setpc_x + 20, 2 * setpc_y + 22 });
		quest._qvar1 = 3;
		NetSendCmdQuest(true, quest);
	}

	if (gbIsMultiplayer) {
		return;
	}

	if (currlevel == quest._qlevel
	    && !setlevel
	    && quest._qvar1 >= 2
	    && (quest._qactive == QUEST_ACTIVE || quest._qactive == QUEST_DONE)
	    && (quest._qvar2 == 0 || quest._qvar2 == 2)) {
		quest.position.x = 2 * quest.position.x + 16;
		quest.position.y = 2 * quest.position.y + 16;
		int rportx = quest.position.x;
		int rporty = quest.position.y;
		AddMissile({ rportx, rporty }, { rportx, rporty }, Direction::South, MIS_RPORTAL, TARGET_MONSTERS, MyPlayerId, 0, 0);
		quest._qvar2 = 1;
		if (quest._qactive == QUEST_ACTIVE) {
			quest._qvar1 = 3;
		}
	}

	if (quest._qactive == QUEST_DONE
	    && setlevel
	    && setlvlnum == SL_VILEBETRAYER
	    && quest._qvar2 == 4) {
		int rportx = 35;
		int rporty = 32;
		AddMissile({ rportx, rporty }, { rportx, rporty }, Direction::South, MIS_RPORTAL, TARGET_MONSTERS, MyPlayerId, 0, 0);
		quest._qvar2 = 3;
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

	for (auto &quest : Quests) {
		if (quest._qidx != Q_BETRAYER && currlevel == quest._qlevel && quest._qslvl != 0) {
			int ql = quest._qslvl - 1;

			if (EntranceBoundaryContains(quest.position, cursPosition)) {
				strcpy(infostr, fmt::format(_(/* TRANSLATORS: Used for Quest Portals. {:s} is a Map Name */ "To {:s}"), _(QuestTriggerNames[ql])).c_str());
				cursPosition = quest.position;
				return true;
			}
		}
	}

	return false;
}

void CheckQuestKill(const Monster &monster, bool sendmsg)
{
	if (gbIsSpawn)
		return;

	auto &myPlayer = Players[MyPlayerId];

	if (monster.MType->mtype == MT_SKING) {
		auto &quest = Quests[Q_SKELKING];
		quest._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::RestWellLeoricIllFindYourSon, 30);
		if (sendmsg)
			NetSendCmdQuest(true, quest);

	} else if (monster.MType->mtype == MT_CLEAVER) {
		auto &quest = Quests[Q_BUTCHER];
		quest._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::TheSpiritsOfTheDeadAreNowAvenged, 30);
		if (sendmsg)
			NetSendCmdQuest(true, quest);
	} else if (monster._uniqtype - 1 == UMT_GARBUD) { //"Gharbad the Weak"
		Quests[Q_GARBUD]._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::ImNotImpressed, 30);
	} else if (monster._uniqtype - 1 == UMT_ZHAR) { //"Zhar the Mad"
		Quests[Q_ZHAR]._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::ImSorryDidIBreakYourConcentration, 30);
	} else if (monster._uniqtype - 1 == UMT_LAZARUS && gbIsMultiplayer) { //"Arch-Bishop Lazarus"
		auto &betrayerQuest = Quests[Q_BETRAYER];
		auto &diabloQuest = Quests[Q_DIABLO];
		betrayerQuest._qactive = QUEST_DONE;
		betrayerQuest._qvar1 = 7;
		diabloQuest._qactive = QUEST_ACTIVE;

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
			NetSendCmdQuest(true, betrayerQuest);
			NetSendCmdQuest(true, diabloQuest);
		}
	} else if (monster._uniqtype - 1 == UMT_LAZARUS && !gbIsMultiplayer) { //"Arch-Bishop Lazarus"
		Quests[Q_BETRAYER]._qactive = QUEST_DONE;
		InitVPTriggers();
		Quests[Q_BETRAYER]._qvar1 = 7;
		Quests[Q_BETRAYER]._qvar2 = 4;
		Quests[Q_DIABLO]._qactive = QUEST_ACTIVE;
		AddMissile({ 35, 32 }, { 35, 32 }, Direction::South, MIS_RPORTAL, TARGET_MONSTERS, MyPlayerId, 0, 0);
		myPlayer.Say(HeroSpeech::YourMadnessEndsHereBetrayer, 30);
	} else if (monster._uniqtype - 1 == UMT_WARLORD) { //"Warlord of Blood"
		Quests[Q_WARLORD]._qactive = QUEST_DONE;
		myPlayer.Say(HeroSpeech::YourReignOfPainHasEnded, 30);
	}
}

void DRLG_CheckQuests(int x, int y)
{
	for (auto &quest : Quests) {
		if (quest.IsAvailable()) {
			switch (quest._qidx) {
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
				DrawSkelKing(quest._qidx, x, y);
				break;
			case Q_SCHAMB:
				DrawSChamber(quest._qidx, x, y);
				break;
			default:
				break;
			}
		}
	}
}

void SetReturnLvlPos()
{
	switch (setlvlnum) {
	case SL_SKELKING:
		ReturnLvlPosition = Quests[Q_SKELKING].position + Direction::SouthEast;
		ReturnLevel = Quests[Q_SKELKING]._qlevel;
		ReturnLevelType = DTYPE_CATHEDRAL;
		break;
	case SL_BONECHAMB:
		ReturnLvlPosition = Quests[Q_SCHAMB].position + Direction::SouthEast;
		ReturnLevel = Quests[Q_SCHAMB]._qlevel;
		ReturnLevelType = DTYPE_CATACOMBS;
		break;
	case SL_MAZE:
		break;
	case SL_POISONWATER:
		ReturnLvlPosition = Quests[Q_PWATER].position + Direction::SouthWest;
		ReturnLevel = Quests[Q_PWATER]._qlevel;
		ReturnLevelType = DTYPE_CATHEDRAL;
		break;
	case SL_VILEBETRAYER:
		ReturnLvlPosition = Quests[Q_BETRAYER].position + Direction::East;
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
	ViewPosition = ReturnLvlPosition;
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

	auto &kingQuest = Quests[Q_SKELKING];
	if (kingQuest._qactive == QUEST_INIT
	    && currlevel >= kingQuest._qlevel - 1
	    && currlevel <= kingQuest._qlevel + 1) {
		kingQuest._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, kingQuest);
	}

	auto &butcherQuest = Quests[Q_BUTCHER];
	if (butcherQuest._qactive == QUEST_INIT
	    && currlevel >= butcherQuest._qlevel - 1
	    && currlevel <= butcherQuest._qlevel + 1) {
		butcherQuest._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, butcherQuest);
	}

	auto &betrayerQuest = Quests[Q_BETRAYER];
	if (betrayerQuest._qactive == QUEST_INIT && currlevel == betrayerQuest._qlevel - 1) {
		betrayerQuest._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, betrayerQuest);
	}
	if (betrayerQuest.IsAvailable())
		AddObject(OBJ_ALTBOY, { 2 * setpc_x + 20, 2 * setpc_y + 22 });

	auto &cryptQuest = Quests[Q_GRAVE];
	if (cryptQuest._qactive == QUEST_INIT && currlevel == cryptQuest._qlevel - 1) {
		cryptQuest._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, cryptQuest);
	}

	auto &defilerQuest = Quests[Q_DEFILER];
	if (defilerQuest._qactive == QUEST_INIT && currlevel == defilerQuest._qlevel - 1) {
		defilerQuest._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, defilerQuest);
	}

	auto &nakrulQuest = Quests[Q_NAKRUL];
	if (nakrulQuest._qactive == QUEST_INIT && currlevel == nakrulQuest._qlevel - 1) {
		nakrulQuest._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, nakrulQuest);
	}

	auto &cowQuest = Quests[Q_JERSEY];
	if (cowQuest._qactive == QUEST_INIT && currlevel == cowQuest._qlevel - 1) {
		cowQuest._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, cowQuest);
	}
}

void ResyncQuests()
{
	if (gbIsSpawn)
		return;

	if (Quests[Q_LTBANNER].IsAvailable()) {
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
	int l = QuestLogMouseToEntry();
	if (l >= 0) {
		SelectedQuest = l;
	}
	const auto x = InnerPanel.position.x;
	CelDrawTo(out, GetPanelPosition(UiPanels::Quest, { 0, 351 }), *pQLogCel, 1);
	int y = InnerPanel.position.y + ListYOffset;
	for (int i = 0; i < EncounteredQuestCount; i++) {
		if (i == FirstFinishedQuest) {
			y += FinishedQuestOffset;
		}
		PrintQLString(out, x, y, _(QuestsData[EncounteredQuests[i]]._qlstr), i == SelectedQuest, i >= FirstFinishedQuest);
		y += LineSpacing;
	}
}

void StartQuestlog()
{

	auto sortQuestIdx = [](int a, int b) {
		return QuestsData[a].questBookOrder < QuestsData[b].questBookOrder;
	};

	EncounteredQuestCount = 0;
	for (auto &quest : Quests) {
		if (quest._qactive == QUEST_ACTIVE && quest._qlog) {
			EncounteredQuests[EncounteredQuestCount] = quest._qidx;
			EncounteredQuestCount++;
		}
	}
	FirstFinishedQuest = EncounteredQuestCount;
	for (auto &quest : Quests) {
		if (quest._qactive == QUEST_DONE || quest._qactive == QUEST_HIVE_DONE) {
			EncounteredQuests[EncounteredQuestCount] = quest._qidx;
			EncounteredQuestCount++;
		}
	}

	std::sort(&EncounteredQuests[0], &EncounteredQuests[FirstFinishedQuest], sortQuestIdx);
	std::sort(&EncounteredQuests[FirstFinishedQuest], &EncounteredQuests[EncounteredQuestCount], sortQuestIdx);

	bool twoBlocks = FirstFinishedQuest != 0 && FirstFinishedQuest < EncounteredQuestCount;

	ListYOffset = 0;
	FinishedQuestOffset = !twoBlocks ? 0 : LineHeight / 2;

	int overallMinHeight = EncounteredQuestCount * LineHeight + FinishedQuestOffset;
	int space = InnerPanel.size.height;

	if (EncounteredQuestCount > 0) {
		int additionalSpace = space - overallMinHeight;
		int addLineSpacing = additionalSpace / EncounteredQuestCount;
		addLineSpacing = std::min(MaxSpacing - LineHeight, addLineSpacing);
		LineSpacing = LineHeight + addLineSpacing;
		if (twoBlocks) {
			int additionalSepSpace = additionalSpace - (addLineSpacing * EncounteredQuestCount);
			additionalSepSpace = std::min(LineHeight, additionalSepSpace);
			FinishedQuestOffset = std::max(4, additionalSepSpace);
		}

		int overallHeight = EncounteredQuestCount * LineSpacing + FinishedQuestOffset;
		ListYOffset += (space - overallHeight) / 2;
	}

	SelectedQuest = FirstFinishedQuest == 0 ? -1 : 0;
	QuestLogIsOpen = true;
}

void QuestlogUp()
{
	if (FirstFinishedQuest == 0) {
		SelectedQuest = -1;
	} else {
		SelectedQuest--;
		if (SelectedQuest < 0) {
			SelectedQuest = FirstFinishedQuest - 1;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

void QuestlogDown()
{
	if (FirstFinishedQuest == 0) {
		SelectedQuest = -1;
	} else {
		SelectedQuest++;
		if (SelectedQuest == FirstFinishedQuest) {
			SelectedQuest = 0;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

void QuestlogEnter()
{
	PlaySFX(IS_TITLSLCT);
	if (EncounteredQuestCount != 0 && SelectedQuest >= 0 && SelectedQuest < FirstFinishedQuest)
		InitQTextMsg(Quests[EncounteredQuests[SelectedQuest]]._qmsg);
	QuestLogIsOpen = false;
}

void QuestlogESC()
{
	int l = QuestLogMouseToEntry();
	if (l != -1) {
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

bool Quest::IsAvailable()
{
	if (setlevel)
		return false;
	if (currlevel != _qlevel)
		return false;
	if (_qactive == QUEST_NOTAVAIL)
		return false;
	if (gbIsMultiplayer && QuestsData[_qidx].isSinglePlayerOnly)
		return false;

	return true;
}

} // namespace devilution
