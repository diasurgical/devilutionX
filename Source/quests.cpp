/**
 * @file quests.cpp
 *
 * Implementation of functionality for handling quests.
 */

#include "control.h"
#include "cursor.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "gendung.h"
#include "init.h"
#include "minitext.h"
#include "missiles.h"
#include "options.h"
#include "stores.h"
#include "towners.h"
#include "trigs.h"
#include "utils/language.h"

namespace devilution {

int qtopline;
bool questlog;
std::optional<CelSprite> pQLogCel;
/** Contains the quests of the current game. */
QuestStruct quests[MAXQUESTS];
int qline;
int qlist[MAXQUESTS];
int numqlines;
int WaterDone;
int ReturnLvlX;
int ReturnLvlY;
dungeon_type ReturnLvlT;
int ReturnLvl;

/** Contains the data related to each quest_id. */
QuestData questlist[] = {
	// clang-format off
	// _qdlvl,  _qdmultlvl, _qlvlt,          _qdtype,     _qdrnd, _qslvl,          isSinglePlayerOnly, _qdmsg,        _qlstr
	{       5,          -1, DTYPE_NONE,      Q_ROCK,      100,    SL_NONE,         true,               TEXT_INFRA5,   N_("The Magic Rock")           },
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
	{       9,           9, DTYPE_NONE,      Q_JERSEY,    100,    SL_NONE,         false,              TEXT_JERSEY4,  N_("The Jersey's Jersey")      },
	// clang-format on
};
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
const char *const questtrigstr[5] = {
	N_("King Leoric's Tomb"),
	N_("The Chamber of Bone"),
	N_("Maze"),
	N_("A Dark Passage"),
	N_("Unholy Altar")
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

void InitQuests()
{
	int i, initiatedQuests;
	DWORD z;

	if (!gbIsMultiplayer) {
		for (i = 0; i < MAXQUESTS; i++) {
			quests[i]._qactive = QUEST_NOTAVAIL;
		}
	} else {
		for (i = 0; i < MAXQUESTS; i++) {
			if (questlist[i].isSinglePlayerOnly) {
				quests[i]._qactive = QUEST_NOTAVAIL;
			}
		}
	}

	Qtalklist[TOWN_HEALER][Q_MUSHROOM] = TEXT_NONE;
	Qtalklist[TOWN_WITCH][Q_MUSHROOM] = TEXT_MUSH9;

	questlog = false;
	WaterDone = 0;
	initiatedQuests = 0;

	for (z = 0; z < MAXQUESTS; z++) {
		if (gbIsMultiplayer && questlist[z].isSinglePlayerOnly)
			continue;
		quests[z]._qtype = questlist[z]._qdtype;
		if (gbIsMultiplayer) {
			quests[z]._qlevel = questlist[z]._qdmultlvl;
			if (!delta_quest_inited(initiatedQuests)) {
				quests[z]._qactive = QUEST_INIT;
				quests[z]._qvar1 = 0;
				quests[z]._qlog = false;
			}
			initiatedQuests++;
		} else {
			quests[z]._qactive = QUEST_INIT;
			quests[z]._qlevel = questlist[z]._qdlvl;
			quests[z]._qvar1 = 0;
			quests[z]._qlog = false;
		}

		quests[z]._qslvl = questlist[z]._qslvl;
		quests[z].position = { 0, 0 };
		quests[z]._qidx = z;
		quests[z]._qlvltype = questlist[z]._qlvlt;
		quests[z]._qvar2 = 0;
		quests[z]._qmsg = questlist[z]._qdmsg;
	}

	if (!gbIsMultiplayer && sgOptions.Gameplay.bRandomizeQuests) {
		SetRndSeed(glSeedTbl[15]);
		if (GenerateRnd(2) != 0)
			quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;
		else
			quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL;

		quests[QuestGroup1[GenerateRnd(sizeof(QuestGroup1) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup2[GenerateRnd(sizeof(QuestGroup2) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup3[GenerateRnd(sizeof(QuestGroup3) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup4[GenerateRnd(sizeof(QuestGroup4) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
	}
#ifdef _DEBUG
	if (questdebug != -1)
		quests[questdebug]._qactive = QUEST_ACTIVE;
#endif

	if (gbIsSpawn) {
		for (z = 0; z < MAXQUESTS; z++) {
			quests[z]._qactive = QUEST_NOTAVAIL;
		}
	}

	if (quests[Q_SKELKING]._qactive == QUEST_NOTAVAIL)
		quests[Q_SKELKING]._qvar2 = 2;
	if (quests[Q_ROCK]._qactive == QUEST_NOTAVAIL)
		quests[Q_ROCK]._qvar2 = 2;
	quests[Q_LTBANNER]._qvar1 = 1;
	if (gbIsMultiplayer)
		quests[Q_BETRAYER]._qvar1 = 2;
}

void CheckQuests()
{
	if (gbIsSpawn)
		return;

	int i, rportx, rporty;

	if (QuestStatus(Q_BETRAYER) && gbIsMultiplayer && quests[Q_BETRAYER]._qvar1 == 2) {
		AddObject(OBJ_ALTBOY, 2 * setpc_x + 20, 2 * setpc_y + 22);
		quests[Q_BETRAYER]._qvar1 = 3;
		NetSendCmdQuest(true, Q_BETRAYER);
	}

	if (gbIsMultiplayer) {
		return;
	}

	if (currlevel == quests[Q_BETRAYER]._qlevel
	    && !setlevel
	    && quests[Q_BETRAYER]._qvar1 >= 2
	    && (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE || quests[Q_BETRAYER]._qactive == QUEST_DONE)
	    && (quests[Q_BETRAYER]._qvar2 == 0 || quests[Q_BETRAYER]._qvar2 == 2)) {
		quests[Q_BETRAYER].position.x = 2 * quests[Q_BETRAYER].position.x + 16;
		quests[Q_BETRAYER].position.y = 2 * quests[Q_BETRAYER].position.y + 16;
		rportx = quests[Q_BETRAYER].position.x;
		rporty = quests[Q_BETRAYER].position.y;
		AddMissile(rportx, rporty, rportx, rporty, 0, MIS_RPORTAL, TARGET_MONSTERS, myplr, 0, 0);
		quests[Q_BETRAYER]._qvar2 = 1;
		if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE) {
			quests[Q_BETRAYER]._qvar1 = 3;
		}
	}

	if (quests[Q_BETRAYER]._qactive == QUEST_DONE
	    && setlevel
	    && setlvlnum == SL_VILEBETRAYER
	    && quests[Q_BETRAYER]._qvar2 == 4) {
		rportx = 35;
		rporty = 32;
		AddMissile(rportx, rporty, rportx, rporty, 0, MIS_RPORTAL, TARGET_MONSTERS, myplr, 0, 0);
		quests[Q_BETRAYER]._qvar2 = 3;
	}

	if (setlevel) {
		if (setlvlnum == quests[Q_PWATER]._qslvl
		    && quests[Q_PWATER]._qactive != QUEST_INIT
		    && leveltype == quests[Q_PWATER]._qlvltype
		    && nummonsters == 4
		    && quests[Q_PWATER]._qactive != QUEST_DONE) {
			quests[Q_PWATER]._qactive = QUEST_DONE;
			PlaySfxLoc(IS_QUESTDN, plr[myplr].position.tile.x, plr[myplr].position.tile.y);
			LoadPalette("Levels\\L3Data\\L3pwater.pal", false);
			UpdatePWaterPalette();
			WaterDone = 32;
		}
	} else if (plr[myplr]._pmode == PM_STAND) {
		for (i = 0; i < MAXQUESTS; i++) {
			if (currlevel == quests[i]._qlevel
			    && quests[i]._qslvl != 0
			    && quests[i]._qactive != QUEST_NOTAVAIL
			    && plr[myplr].position.tile == quests[i].position) {
				if (quests[i]._qlvltype != DTYPE_NONE) {
					setlvltype = quests[i]._qlvltype;
				}
				StartNewLvl(myplr, WM_DIABSETLVL, quests[i]._qslvl);
			}
		}
	}
}

bool ForceQuests()
{
	int i, j, qx, qy, ql;

	if (gbIsSpawn)
		return false;

	if (gbIsMultiplayer) {
		return false;
	}

	for (i = 0; i < MAXQUESTS; i++) {

		if (i != Q_BETRAYER && currlevel == quests[i]._qlevel && quests[i]._qslvl != 0) {
			ql = quests[quests[i]._qidx]._qslvl - 1;
			qx = quests[i].position.x;
			qy = quests[i].position.y;

			for (j = 0; j < 7; j++) {
				if (qx + questxoff[j] == cursmx && qy + questyoff[j] == cursmy) {
					sprintf(infostr, _("To %s"), _(questtrigstr[ql]));
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
	if (currlevel != quests[i]._qlevel)
		return false;
	if (quests[i]._qactive == QUEST_NOTAVAIL)
		return false;
	if (gbIsMultiplayer && questlist[i].isSinglePlayerOnly)
		return false;
	return true;
}

void CheckQuestKill(int m, bool sendmsg)
{
	int i, j;

	if (gbIsSpawn)
		return;

	if (monster[m].MType->mtype == MT_SKING) {
		quests[Q_SKELKING]._qactive = QUEST_DONE;
		plr[myplr].PlaySpeach(82, 30);
		if (sendmsg)
			NetSendCmdQuest(true, Q_SKELKING);

	} else if (monster[m].MType->mtype == MT_CLEAVER) {
		quests[Q_BUTCHER]._qactive = QUEST_DONE;
		plr[myplr].PlaySpeach(80, 30);
		if (sendmsg)
			NetSendCmdQuest(true, Q_BUTCHER);
	} else if (monster[m]._uniqtype - 1 == UMT_GARBUD) { //"Gharbad the Weak"
		quests[Q_GARBUD]._qactive = QUEST_DONE;
		plr[myplr].PlaySpeach(61, 30);
	} else if (monster[m]._uniqtype - 1 == UMT_ZHAR) { //"Zhar the Mad"
		quests[Q_ZHAR]._qactive = QUEST_DONE;
		plr[myplr].PlaySpeach(62, 30);
	} else if (monster[m]._uniqtype - 1 == UMT_LAZURUS && gbIsMultiplayer) { //"Arch-Bishop Lazarus"
		quests[Q_BETRAYER]._qactive = QUEST_DONE;
		quests[Q_BETRAYER]._qvar1 = 7;
		quests[Q_DIABLO]._qactive = QUEST_ACTIVE;

		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] == 370) {
					trigs[numtrigs].position = { i, j };
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
			}
		}
		plr[myplr].PlaySpeach(83, 30);
		if (sendmsg) {
			NetSendCmdQuest(true, Q_BETRAYER);
			NetSendCmdQuest(true, Q_DIABLO);
		}
	} else if (monster[m]._uniqtype - 1 == UMT_LAZURUS && !gbIsMultiplayer) { //"Arch-Bishop Lazarus"
		quests[Q_BETRAYER]._qactive = QUEST_DONE;
		InitVPTriggers();
		quests[Q_BETRAYER]._qvar1 = 7;
		quests[Q_BETRAYER]._qvar2 = 4;
		quests[Q_DIABLO]._qactive = QUEST_ACTIVE;
		AddMissile(35, 32, 35, 32, 0, MIS_RPORTAL, TARGET_MONSTERS, myplr, 0, 0);
		plr[myplr].PlaySpeach(83, 30);
	} else if (monster[m]._uniqtype - 1 == UMT_WARLORD) { //"Warlord of Blood"
		quests[Q_WARLORD]._qactive = QUEST_DONE;
		plr[myplr].PlaySpeach(94, 30);
	}
}

void DrawButcher()
{
	int x, y;

	x = 2 * setpc_x + 16;
	y = 2 * setpc_y + 16;
	DRLG_RectTrans(x + 3, y + 3, x + 10, y + 10);
}

void DrawSkelKing(int q, int x, int y)
{
	quests[q].position = { 2 * x + 28, 2 * y + 23 };
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

	quests[q].position = { 2 * x + 22, 2 * y + 23 };
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

void DRLG_CheckQuests(int x, int y)
{
	int i;

	for (i = 0; i < MAXQUESTS; i++) {
		if (QuestStatus(i)) {
			switch (quests[i]._qtype) {
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
		ReturnLvlX = quests[Q_SKELKING].position.x + 1;
		ReturnLvlY = quests[Q_SKELKING].position.y;
		ReturnLvl = quests[Q_SKELKING]._qlevel;
		ReturnLvlT = DTYPE_CATHEDRAL;
		break;
	case SL_BONECHAMB:
		ReturnLvlX = quests[Q_SCHAMB].position.x + 1;
		ReturnLvlY = quests[Q_SCHAMB].position.y;
		ReturnLvl = quests[Q_SCHAMB]._qlevel;
		ReturnLvlT = DTYPE_CATACOMBS;
		break;
	case SL_MAZE:
		break;
	case SL_POISONWATER:
		ReturnLvlX = quests[Q_PWATER].position.x;
		ReturnLvlY = quests[Q_PWATER].position.y + 1;
		ReturnLvl = quests[Q_PWATER]._qlevel;
		ReturnLvlT = DTYPE_CATHEDRAL;
		break;
	case SL_VILEBETRAYER:
		ReturnLvlX = quests[Q_BETRAYER].position.x + 1;
		ReturnLvlY = quests[Q_BETRAYER].position.y - 1;
		ReturnLvl = quests[Q_BETRAYER]._qlevel;
		ReturnLvlT = DTYPE_HELL;
		break;
	case SL_NONE:
		break;
	}
}

void GetReturnLvlPos()
{
	if (quests[Q_BETRAYER]._qactive == QUEST_DONE)
		quests[Q_BETRAYER]._qvar2 = 2;
	ViewX = ReturnLvlX;
	ViewY = ReturnLvlY;
	currlevel = ReturnLvl;
	leveltype = ReturnLvlT;
}

void LoadPWaterPalette()
{
	if (!setlevel || setlvlnum != quests[Q_PWATER]._qslvl || quests[Q_PWATER]._qactive == QUEST_INIT || leveltype != quests[Q_PWATER]._qlvltype)
		return;

	if (quests[Q_PWATER]._qactive == QUEST_DONE)
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

	if (quests[Q_SKELKING]._qactive == QUEST_INIT
	    && currlevel >= quests[Q_SKELKING]._qlevel - 1
	    && currlevel <= quests[Q_SKELKING]._qlevel + 1) {
		quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_SKELKING);
	}
	if (quests[Q_BUTCHER]._qactive == QUEST_INIT
	    && currlevel >= quests[Q_BUTCHER]._qlevel - 1
	    && currlevel <= quests[Q_BUTCHER]._qlevel + 1) {
		quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_BUTCHER);
	}
	if (quests[Q_BETRAYER]._qactive == QUEST_INIT && currlevel == quests[Q_BETRAYER]._qlevel - 1) {
		quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_BETRAYER);
	}
	if (QuestStatus(Q_BETRAYER))
		AddObject(OBJ_ALTBOY, 2 * setpc_x + 20, 2 * setpc_y + 22);
	if (quests[Q_GRAVE]._qactive == QUEST_INIT && currlevel == quests[Q_GRAVE]._qlevel - 1) {
		quests[Q_GRAVE]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_GRAVE);
	}
	if (quests[Q_DEFILER]._qactive == QUEST_INIT && currlevel == quests[Q_DEFILER]._qlevel - 1) {
		quests[Q_DEFILER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_DEFILER);
	}
	if (quests[Q_NAKRUL]._qactive == QUEST_INIT && currlevel == quests[Q_NAKRUL]._qlevel - 1) {
		quests[Q_NAKRUL]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_NAKRUL);
	}
	if (quests[Q_JERSEY]._qactive == QUEST_INIT && currlevel == quests[Q_JERSEY]._qlevel - 1) {
		quests[Q_JERSEY]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(true, Q_JERSEY);
	}
}

void ResyncQuests()
{
	int i, tren, x, y;

	if (gbIsSpawn)
		return;

	if (QuestStatus(Q_LTBANNER)) {
		if (quests[Q_LTBANNER]._qvar1 == 1) {
			ObjChangeMapResync(
			    setpc_w + setpc_x - 2,
			    setpc_h + setpc_y - 2,
			    setpc_w + setpc_x + 1,
			    setpc_h + setpc_y + 1);
		}
		if (quests[Q_LTBANNER]._qvar1 == 2) {
			ObjChangeMapResync(
			    setpc_w + setpc_x - 2,
			    setpc_h + setpc_y - 2,
			    setpc_w + setpc_x + 1,
			    setpc_h + setpc_y + 1);
			ObjChangeMapResync(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 2, (setpc_h / 2) + setpc_y - 2);
			for (i = 0; i < nobjects; i++)
				SyncObjectAnim(objectactive[i]);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 4, setpc_y + (setpc_h / 2));
			TransVal = tren;
		}
		if (quests[Q_LTBANNER]._qvar1 == 3) {
			x = setpc_x;
			y = setpc_y;
			ObjChangeMapResync(x, y, x + setpc_w + 1, y + setpc_h + 1);
			for (i = 0; i < nobjects; i++)
				SyncObjectAnim(objectactive[i]);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 4, setpc_y + (setpc_h / 2));
			TransVal = tren;
		}
	}
	if (currlevel == quests[Q_MUSHROOM]._qlevel) {
		if (quests[Q_MUSHROOM]._qactive == QUEST_INIT && quests[Q_MUSHROOM]._qvar1 == QS_INIT) {
			SpawnQuestItem(IDI_FUNGALTM, {0, 0}, 5, true);
			quests[Q_MUSHROOM]._qvar1 = QS_TOMESPAWNED;
		} else {
			if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
				if (quests[Q_MUSHROOM]._qvar1 >= QS_MUSHGIVEN) {
					Qtalklist[TOWN_WITCH][Q_MUSHROOM] = TEXT_NONE;
					Qtalklist[TOWN_HEALER][Q_MUSHROOM] = TEXT_MUSH3;
				} else if (quests[Q_MUSHROOM]._qvar1 >= QS_BRAINGIVEN) {
					Qtalklist[TOWN_HEALER][Q_MUSHROOM] = TEXT_NONE;
				}
			}
		}
	}
	if (currlevel == quests[Q_VEIL]._qlevel + 1 && quests[Q_VEIL]._qactive == QUEST_ACTIVE && quests[Q_VEIL]._qvar1 == 0) {
		quests[Q_VEIL]._qvar1 = 1;
		SpawnQuestItem(IDI_GLDNELIX, {0, 0}, 5, true);
	}
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		if (quests[Q_BETRAYER]._qvar1 >= 4)
			ObjChangeMapResync(1, 11, 20, 18);
		if (quests[Q_BETRAYER]._qvar1 >= 6)
			ObjChangeMapResync(1, 18, 20, 24);
		if (quests[Q_BETRAYER]._qvar1 >= 7)
			InitVPTriggers();
		for (i = 0; i < nobjects; i++)
			SyncObjectAnim(objectactive[i]);
	}
	if (currlevel == quests[Q_BETRAYER]._qlevel
	    && !setlevel
	    && (quests[Q_BETRAYER]._qvar2 == 1 || quests[Q_BETRAYER]._qvar2 >= 3)
	    && (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE || quests[Q_BETRAYER]._qactive == QUEST_DONE)) {
		quests[Q_BETRAYER]._qvar2 = 2;
	}
}

static void PrintQLString(const CelOutputBuffer &out, int x, int line, const char *str)
{
	int width = GetLineWidth(str);
	int sx = x + std::max((257 - width) / 2, 0);
	int sy = line * 12 + 44;
	if (qline == line) {
		CelDrawTo(out, sx - 20, sy + 1, *pSPentSpn2Cels, PentSpn2Spin());
	}
	DrawString(out, str, { sx, sy, 257, 0 }, UIS_SILVER);
	if (qline == line) {
		CelDrawTo(out, sx + width + 7, sy + 1, *pSPentSpn2Cels, PentSpn2Spin());
	}
}

void DrawQuestLog(const CelOutputBuffer &out)
{
	DrawString(out, _("Quest Log"), { 32, 44, 257, 0 }, UIS_CENTER);
	CelDrawTo(out, 0, 351, *pQLogCel, 1);
	int line = qtopline;
	for (int i = 0; i < numqlines; i++) {
		PrintQLString(out, 32, line, _(questlist[qlist[i]]._qlstr));
		line += 2;
	}
	PrintQLString(out, 32, 22, _("Close Quest Log"));
}

void StartQuestlog()
{
	DWORD i;

	numqlines = 0;
	for (i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && quests[i]._qlog) {
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
	questlog = true;
}

void QuestlogUp()
{
	if (numqlines) {
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
	if (numqlines) {
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
	if (numqlines && qline != 22)
		InitQTextMsg(quests[qlist[(qline - qtopline) / 2]]._qmsg);
	questlog = false;
}

void QuestlogESC()
{
	int y, i;

	y = (MouseY - 32) / 12;
	if (numqlines) {
		for (i = 0; i < numqlines; i++) {
			if (y == qtopline + 2 * i) {
				qline = y;
				QuestlogEnter();
			}
		}
	}
	if (y == 22) {
		qline = 22;
		QuestlogEnter();
	}
}

void SetMultiQuest(int q, quest_state s, int l, int v1)
{
	if (gbIsSpawn)
		return;

	if (quests[q]._qactive != QUEST_DONE) {
		if (s > quests[q]._qactive)
			quests[q]._qactive = s;
		quests[q]._qlog |= l;
		if (v1 > quests[q]._qvar1)
			quests[q]._qvar1 = v1;
	}
}

} // namespace devilution
