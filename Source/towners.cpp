#include "towners.h"

#include "cursor.h"
#include "engine/cel_header.hpp"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "inv.h"
#include "minitext.h"
#include "stores.h"
#include "utils/language.h"

namespace devilution {
namespace {

std::unique_ptr<byte[]> CowCels;
int CowMsg;
int CowClicks;

/**
 * Maps from direction to coordinate delta, which is used when
 * placing cows in Tristram. A single cow may require space of up
 * to three tiles when being placed on the map.
 */
Displacement CowOffsets[8] = { { -1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 } };

/** Specifies the active sound effect ID for interacting with cows. */
_sfx_id CowPlaying = SFX_NONE;

struct TownerInit {
	_talker_id type;
	Point position;
	Direction dir;
	void (*init)(TownerStruct &towner, const TownerInit &initData);
	void (*talk)(PlayerStruct &player, TownerStruct &towner);
};

void NewTownerAnim(TownerStruct &towner, byte *pAnim, uint8_t numFrames, int delay)
{
	towner._tAnimData = pAnim;
	towner._tAnimLen = numFrames;
	towner._tAnimFrame = 1;
	towner._tAnimCnt = 0;
	towner._tAnimDelay = delay;
}

void InitTownerInfo(int i, const TownerInit &initData)
{
	auto &towner = Towners[i];

	towner._ttype = initData.type;
	towner.position = initData.position;
	towner.talk = initData.talk;
	towner.seed = AdvanceRndSeed(); // TODO: Narrowing conversion, tSeed might need to be uint16_t

	dMonster[towner.position.x][towner.position.y] = i + 1;

	initData.init(towner, initData);
}

void LoadTownerAnimations(TownerStruct &towner, const char *path, int frames, Direction dir, int delay)
{
	towner.data = LoadFileInMem(path);
	for (auto &animation : towner._tNAnim) {
		animation = towner.data.get();
	}
	NewTownerAnim(towner, towner._tNAnim[dir], frames, delay);
}

/**
 * @brief Load Griswold into the game
 */
void InitSmith(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	static const uint8_t AnimOrder[] = {
		// clang-format off
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2, 3, 4
		// clang-format on
	};
	towner.animOrder = AnimOrder;
	towner.animOrderSize = sizeof(AnimOrder);
	LoadTownerAnimations(towner, "Towners\\Smith\\SmithN.CEL", 16, initData.dir, 3);
	towner.name = _("Griswold the Blacksmith");
}

void InitBarOwner(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	static const uint8_t AnimOrder[] = {
		// clang-format off
		1, 2, 3, 3, 2,  1, 16, 15, 14, 14, 15, 16,
		1, 2, 3, 3, 2,  1, 16, 15, 14, 14, 15, 16,
		1, 2, 3, 3, 2,  1, 16, 15, 14, 14, 15, 16,
		1, 2, 3, 3, 2,  1, 16, 15, 14, 14, 15, 16,
		1, 2, 3, 3, 2,  1, 16, 15, 14, 14, 15, 16,
		1, 2, 3, 3, 2,  1, 16, 15, 14, 14, 15, 16,
		1, 2, 3, 3, 2,  1, 16, 15, 14, 14, 15, 16,
		1, 2, 3, 2, 1, 16, 15, 14, 14, 15, 16,
		1, 2, 3, 4, 5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
		// clang-format on
	};
	towner.animOrder = AnimOrder;
	towner.animOrderSize = sizeof(AnimOrder);
	LoadTownerAnimations(towner, "Towners\\TwnF\\TwnFN.CEL", 16, initData.dir, 3);
	towner.name = _("Ogden the Tavern owner");
}

void InitTownDead(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	towner.animOrder = nullptr;
	towner.animOrderSize = 0;
	LoadTownerAnimations(towner, "Towners\\Butch\\Deadguy.CEL", 8, initData.dir, 6);
	towner.name = _("Wounded Townsman");
}

void InitWitch(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	static const uint8_t AnimOrder[] = {
		// clang-format off
		 4,  4,  4,  5,  6,  6,  6,  5,  4, 15, 14, 13, 13, 13, 14, 15, 4, 5, 6, 6, 6, 5,
		 4,  4,  4,  5,  6,  6,  6,  5,  4, 15, 14, 13, 13, 13, 14, 15, 4, 5, 6, 6, 6, 5,
		 4,  4,  4,  5,  6,  6,  6,  5,  4, 15, 14, 13, 13, 13, 14, 15, 4, 5, 6, 6, 6, 5,
		 4,  3,  2,  1, 19, 18, 19,  1,  2,  1, 19, 18, 19,  1,  2,
		 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
		15, 15, 14, 13, 13, 13, 13, 14, 15,
		15, 15, 14, 13, 12, 12, 12, 11, 10, 10, 10,  9,
		 8,  9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		 1,  2,  1, 19, 18, 19,  1,  2,  1,  2,  3
		// clang-format on
	};
	towner.animOrder = AnimOrder;
	towner.animOrderSize = sizeof(AnimOrder);
	LoadTownerAnimations(towner, "Towners\\TownWmn1\\Witch.CEL", 19, initData.dir, 6);
	towner.name = _("Adria the Witch");
}

void InitBarmaid(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	towner.animOrder = nullptr;
	towner.animOrderSize = 0;
	LoadTownerAnimations(towner, "Towners\\TownWmn1\\WmnN.CEL", 18, initData.dir, 6);
	towner.name = _("Gillian the Barmaid");
}

void InitBoy(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	towner.animOrder = nullptr;
	towner.animOrderSize = 0;
	LoadTownerAnimations(towner, "Towners\\TownBoy\\PegKid1.CEL", 20, initData.dir, 6);
	towner.name = _("Wirt the Peg-legged boy");
}

void InitHealer(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	static const uint8_t AnimOrder[] = {
		// clang-format off
		 1,  2,  3,  3,  2,  1, 20, 19, 19, 20,
		 1,  2,  3,  3,  2,  1, 20, 19, 19, 20,
		 1,  2,  3,  3,  2,  1, 20, 19, 19, 20,
		 1,  2,  3,  3,  2,  1, 20, 19, 19, 20,
		 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
		15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,
		 5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
		15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,
		 5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
		// clang-format on
	};
	towner.animOrder = AnimOrder;
	towner.animOrderSize = sizeof(AnimOrder);
	LoadTownerAnimations(towner, "Towners\\Healer\\Healer.CEL", 20, initData.dir, 6);
	towner.name = _("Pepin the Healer");
}

void InitTeller(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	static const uint8_t AnimOrder[] = {
		// clang-format off
		 1,  1, 25, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25,  1,  1,  1, 25,
		 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
		14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1
		// clang-format on
	};
	towner.animOrder = AnimOrder;
	towner.animOrderSize = sizeof(AnimOrder);
	LoadTownerAnimations(towner, "Towners\\Strytell\\Strytell.CEL", 25, initData.dir, 3);
	towner.name = _("Cain the Elder");
}

void InitDrunk(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	static const uint8_t AnimOrder[] = {
		// clang-format off
		 1, 1, 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 11, 11, 11, 12, 13, 14, 15, 16, 17, 18, 18,
		 1, 1, 1, 18, 17, 16, 15, 14, 13, 12, 11, 10, 11, 12, 13, 14, 15, 16, 17, 18,
		 1, 2, 3,  4,  5,  5,  5,  4,  3,  2
		// clang-format on
	};
	towner.animOrder = AnimOrder;
	towner.animOrderSize = sizeof(AnimOrder);
	LoadTownerAnimations(towner, "Towners\\Drunk\\TwnDrunk.CEL", 18, initData.dir, 3);
	towner.name = _("Farnham the Drunk");
}

void InitCows(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 128;
	towner.animOrder = nullptr;
	towner.animOrderSize = 0;
	for (int i = 0; i < 8; i++) {
		towner._tNAnim[i] = CelGetFrame(CowCels.get(), i);
	}
	NewTownerAnim(towner, towner._tNAnim[initData.dir], 12, 3);
	towner._tAnimFrame = GenerateRnd(11) + 1;
	towner.name = _("Cow");

	const Point position = initData.position;
	const Point offset = position + CowOffsets[initData.dir];
	int index = -dMonster[position.x][position.y];
	if (dMonster[position.x][offset.y] == 0)
		dMonster[position.x][offset.y] = index;
	if (dMonster[offset.x][position.y] == 0)
		dMonster[offset.x][position.y] = index;
	if (dMonster[offset.x][offset.y] == 0)
		dMonster[offset.x][offset.y] = index;
}

void InitFarmer(TownerStruct &towner, const TownerInit &initData)
{
	towner._tAnimWidth = 96;
	towner.animOrder = nullptr;
	towner.animOrderSize = 0;
	LoadTownerAnimations(towner, "Towners\\Farmer\\Farmrn2.CEL", 15, initData.dir, 3);
	towner.name = _("Lester the farmer");
}

void InitCowFarmer(TownerStruct &towner, const TownerInit &initData)
{
	const char *celPath = "Towners\\Farmer\\cfrmrn2.CEL";
	if (Quests[Q_JERSEY]._qactive == QUEST_DONE) {
		celPath = "Towners\\Farmer\\mfrmrn2.CEL";
	}
	towner._tAnimWidth = 96;
	towner.animOrder = nullptr;
	towner.animOrderSize = 0;
	LoadTownerAnimations(towner, celPath, 15, initData.dir, 3);
	towner.name = _("Complete Nut");
}

void InitGirl(TownerStruct &towner, const TownerInit &initData)
{
	const char *celPath = "Towners\\Girl\\Girlw1.CEL";
	if (Quests[Q_GIRL]._qactive == QUEST_DONE) {
		celPath = "Towners\\Girl\\Girls1.CEL";
	}
	towner._tAnimWidth = 96;
	towner.animOrder = nullptr;
	towner.animOrderSize = 0;
	LoadTownerAnimations(towner, celPath, 20, initData.dir, 6);
	towner.name = "Celia";
}

void TownDead(TownerStruct &towner)
{
	if (qtextflag) {
		if (Quests[Q_BUTCHER]._qvar1 == 1)
			towner._tAnimCnt = 0; // Freeze while speaking
		return;
	}

	if ((Quests[Q_BUTCHER]._qactive == QUEST_DONE || Quests[Q_BUTCHER]._qvar1 == 1) && towner._tAnimLen != 1) {
		towner._tAnimLen = 1;
		towner.name = _("Slain Townsman");
	}
}

void TownerTalk(_speech_id message)
{
	CowClicks = 0;
	CowMsg = 0;
	InitQTextMsg(message);
}

void TalkToBarOwner(PlayerStruct &player, TownerStruct &barOwner)
{
	if (!player._pLvlVisited[0]) {
		InitQTextMsg(TEXT_INTRO);
		return;
	}

	if (Quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) {
		if (player._pLvlVisited[2] || player._pLvlVisited[4]) {
			if (Quests[Q_SKELKING]._qvar2 == 0) {
				Quests[Q_SKELKING]._qvar2 = 1;
				Quests[Q_SKELKING]._qlog = true;
				if (Quests[Q_SKELKING]._qactive == QUEST_INIT) {
					Quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
					Quests[Q_SKELKING]._qvar1 = 1;
				}
				InitQTextMsg(TEXT_KING2);
				NetSendCmdQuest(true, Q_SKELKING);
				return;
			}
			if (Quests[Q_SKELKING]._qactive == QUEST_DONE && Quests[Q_SKELKING]._qvar2 == 1) {
				Quests[Q_SKELKING]._qvar2 = 2;
				Quests[Q_SKELKING]._qvar1 = 2;
				InitQTextMsg(TEXT_KING4);
				NetSendCmdQuest(true, Q_SKELKING);
				return;
			}
		}
	}

	if (Quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL) {
		if (player._pLvlVisited[3] && Quests[Q_LTBANNER]._qactive != QUEST_DONE) {
			if (Quests[Q_LTBANNER]._qvar2 == 0) {
				Quests[Q_LTBANNER]._qvar2 = 1;
				if (Quests[Q_LTBANNER]._qactive == QUEST_INIT) {
					Quests[Q_LTBANNER]._qvar1 = 1;
					Quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
				}
				Quests[Q_LTBANNER]._qlog = true;
				InitQTextMsg(TEXT_BANNER2);
				return;
			}

			if (Quests[Q_LTBANNER]._qvar2 == 1 && player.TryRemoveInvItemById(IDI_BANNER)) {
				Quests[Q_LTBANNER]._qactive = QUEST_DONE;
				Quests[Q_LTBANNER]._qvar1 = 3;
				SpawnUnique(UITEM_HARCREST, barOwner.position + DIR_SW);
				InitQTextMsg(TEXT_BANNER3);
				return;
			}
		}
	}

	TownerTalk(TEXT_OGDEN1);
	StartStore(STORE_TAVERN);
}

void TalkToDeadguy(PlayerStruct &player, TownerStruct & /*deadguy*/)
{
	if (Quests[Q_BUTCHER]._qactive == QUEST_DONE)
		return;

	if (Quests[Q_BUTCHER]._qvar1 == 1) {
		player.SaySpecific(HeroSpeech::YourDeathWillBeAvenged);
		return;
	}

	Quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
	Quests[Q_BUTCHER]._qlog = true;
	Quests[Q_BUTCHER]._qmsg = TEXT_BUTCH9;
	Quests[Q_BUTCHER]._qvar1 = 1;
	InitQTextMsg(TEXT_BUTCH9);
	NetSendCmdQuest(true, Q_BUTCHER);
}

void TalkToBlackSmith(PlayerStruct &player, TownerStruct &blackSmith)
{
	if (Quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
		if (player._pLvlVisited[4] && Quests[Q_ROCK]._qactive != QUEST_DONE) {
			if (Quests[Q_ROCK]._qvar2 == 0) {
				Quests[Q_ROCK]._qvar2 = 1;
				Quests[Q_ROCK]._qlog = true;
				if (Quests[Q_ROCK]._qactive == QUEST_INIT) {
					Quests[Q_ROCK]._qactive = QUEST_ACTIVE;
				}
				InitQTextMsg(TEXT_INFRA5);
				return;
			}

			if (Quests[Q_ROCK]._qvar2 == 1 && player.TryRemoveInvItemById(IDI_ROCK)) {
				Quests[Q_ROCK]._qactive = QUEST_DONE;
				SpawnUnique(UITEM_INFRARING, blackSmith.position + DIR_SW);
				InitQTextMsg(TEXT_INFRA7);
				return;
			}
		}
	}
	if (Quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
		if (player._pLvlVisited[9] && Quests[Q_ANVIL]._qactive != QUEST_DONE) {
			if (Quests[Q_ANVIL]._qvar2 == 0 && Quests[Q_ROCK]._qactive != QUEST_INIT) {
				Quests[Q_ANVIL]._qvar2 = 1;
				Quests[Q_ANVIL]._qlog = true;
				if (Quests[Q_ANVIL]._qactive == QUEST_INIT) {
					Quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
				}
				InitQTextMsg(TEXT_ANVIL5);
				return;
			}

			if (Quests[Q_ANVIL]._qvar2 == 1 && player.TryRemoveInvItemById(IDI_ANVIL)) {
				Quests[Q_ANVIL]._qactive = QUEST_DONE;
				SpawnUnique(UITEM_GRISWOLD, blackSmith.position + DIR_SW);
				InitQTextMsg(TEXT_ANVIL7);
				return;
			}
		}
	}

	TownerTalk(TEXT_GRISWOLD1);
	StartStore(STORE_SMITH);
}

void TalkToWitch(PlayerStruct &player, TownerStruct & /*witch*/)
{
	if (Quests[Q_MUSHROOM]._qactive != QUEST_NOTAVAIL) {
		if (Quests[Q_MUSHROOM]._qactive == QUEST_INIT && player.TryRemoveInvItemById(IDI_FUNGALTM)) {
			Quests[Q_MUSHROOM]._qactive = QUEST_ACTIVE;
			Quests[Q_MUSHROOM]._qlog = true;
			Quests[Q_MUSHROOM]._qvar1 = QS_TOMEGIVEN;
			InitQTextMsg(TEXT_MUSH8);
			return;
		}
		if (Quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
			if (Quests[Q_MUSHROOM]._qvar1 >= QS_TOMEGIVEN && Quests[Q_MUSHROOM]._qvar1 < QS_MUSHGIVEN) {
				if (player.TryRemoveInvItemById(IDI_MUSHROOM)) {
					Quests[Q_MUSHROOM]._qvar1 = QS_MUSHGIVEN;
					QuestDialogTable[TOWN_HEALER][Q_MUSHROOM] = TEXT_MUSH3;
					QuestDialogTable[TOWN_WITCH][Q_MUSHROOM] = TEXT_NONE;
					Quests[Q_MUSHROOM]._qmsg = TEXT_MUSH10;
					InitQTextMsg(TEXT_MUSH10);
					return;
				}
				if (Quests[Q_MUSHROOM]._qmsg != TEXT_MUSH9) {
					Quests[Q_MUSHROOM]._qmsg = TEXT_MUSH9;
					InitQTextMsg(TEXT_MUSH9);
					return;
				}
			}
			if (Quests[Q_MUSHROOM]._qvar1 >= QS_MUSHGIVEN) {
				if (player.HasItem(IDI_BRAIN)) {
					Quests[Q_MUSHROOM]._qmsg = TEXT_MUSH11;
					InitQTextMsg(TEXT_MUSH11);
					return;
				}
				if (player.HasItem(IDI_SPECELIX)) {
					InitQTextMsg(TEXT_MUSH12);
					Quests[Q_MUSHROOM]._qactive = QUEST_DONE;
					AllItemsList[IDI_SPECELIX].iUsable = true; /// BUGFIX: This will cause the elixir to be usable in the next game
					return;
				}
			}
		}
	}

	TownerTalk(TEXT_ADRIA1);
	StartStore(STORE_WITCH);
}

void TalkToBarmaid(PlayerStruct &player, TownerStruct & /*barmaid*/)
{
	if (!player._pLvlVisited[21] && player.HasItem(IDI_MAPOFDOOM)) {
		Quests[Q_GRAVE]._qactive = QUEST_ACTIVE;
		Quests[Q_GRAVE]._qlog = true;
		Quests[Q_GRAVE]._qmsg = TEXT_GRAVE8;
		InitQTextMsg(TEXT_GRAVE8);
		return;
	}

	TownerTalk(TEXT_GILLIAN1);
	StartStore(STORE_BARMAID);
}

void TalkToDrunk(PlayerStruct & /*player*/, TownerStruct & /*drunk*/)
{
	TownerTalk(TEXT_FARNHAM1);
	StartStore(STORE_DRUNK);
}

void TalkToHealer(PlayerStruct &player, TownerStruct &healer)
{
	if (Quests[Q_PWATER]._qactive != QUEST_NOTAVAIL) {
		if ((player._pLvlVisited[1] || player._pLvlVisited[5]) && Quests[Q_PWATER]._qactive == QUEST_INIT) {
			Quests[Q_PWATER]._qactive = QUEST_ACTIVE;
			Quests[Q_PWATER]._qlog = true;
			Quests[Q_PWATER]._qmsg = TEXT_POISON3;
			InitQTextMsg(TEXT_POISON3);
			return;
		}
		if (Quests[Q_PWATER]._qactive == QUEST_DONE && Quests[Q_PWATER]._qvar1 != 2) {
			Quests[Q_PWATER]._qvar1 = 2;
			InitQTextMsg(TEXT_POISON5);
			SpawnUnique(UITEM_TRING, healer.position + DIR_SW);
			return;
		}
	}
	if (Quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
		if (Quests[Q_MUSHROOM]._qvar1 >= QS_MUSHGIVEN && Quests[Q_MUSHROOM]._qvar1 < QS_BRAINGIVEN && player.TryRemoveInvItemById(IDI_BRAIN)) {
			SpawnQuestItem(IDI_SPECELIX, healer.position + Displacement { 0, 1 }, 0, 0);
			InitQTextMsg(TEXT_MUSH4);
			Quests[Q_MUSHROOM]._qvar1 = QS_BRAINGIVEN;
			QuestDialogTable[TOWN_HEALER][Q_MUSHROOM] = TEXT_NONE;
			return;
		}
	}

	TownerTalk(TEXT_PEPIN1);
	StartStore(STORE_HEALER);
}

void TalkToBoy(PlayerStruct & /*player*/, TownerStruct & /*boy*/)
{
	TownerTalk(TEXT_WIRT1);
	StartStore(STORE_BOY);
}

void TalkToStoryteller(PlayerStruct &player, TownerStruct & /*storyteller*/)
{
	if (!gbIsMultiplayer) {
		if (Quests[Q_BETRAYER]._qactive == QUEST_INIT && player.TryRemoveInvItemById(IDI_LAZSTAFF)) {
			InitQTextMsg(TEXT_VILE1);
			Quests[Q_BETRAYER]._qlog = true;
			Quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
			Quests[Q_BETRAYER]._qvar1 = 2;
			return;
		}
	} else {
		if (Quests[Q_BETRAYER]._qactive == QUEST_ACTIVE && !Quests[Q_BETRAYER]._qlog) {
			InitQTextMsg(TEXT_VILE1);
			Quests[Q_BETRAYER]._qlog = true;
			NetSendCmdQuest(true, Q_BETRAYER);
			return;
		}
	}
	if (Quests[Q_BETRAYER]._qactive == QUEST_DONE && Quests[Q_BETRAYER]._qvar1 == 7) {
		Quests[Q_BETRAYER]._qvar1 = 8;
		InitQTextMsg(TEXT_VILE3);
		Quests[Q_DIABLO]._qlog = true;
		if (gbIsMultiplayer) {
			NetSendCmdQuest(true, Q_BETRAYER);
			NetSendCmdQuest(true, Q_DIABLO);
		}
		return;
	}

	TownerTalk(TEXT_STORY1);
	StartStore(STORE_STORY);
}

void TalkToCow(PlayerStruct &player, TownerStruct &cow)
{
	if (CowPlaying != SFX_NONE && effect_is_playing(CowPlaying))
		return;

	CowClicks++;

	CowPlaying = TSFX_COW1;
	if (CowClicks == 4) {
		if (gbIsSpawn)
			CowClicks = 0;

		CowPlaying = TSFX_COW2;
	} else if (CowClicks >= 8 && !gbIsSpawn) {
		CowClicks = 4;

		static const HeroSpeech SnSfx[3] = {
			HeroSpeech::YepThatsACowAlright,
			HeroSpeech::ImNotThirsty,
			HeroSpeech::ImNoMilkmaid,
		};
		player.SaySpecific(SnSfx[CowMsg]);
		CowMsg++;
		if (CowMsg >= 3)
			CowMsg = 0;
	}

	PlaySfxLoc(CowPlaying, cow.position);
}

void TalkToFarmer(PlayerStruct &player, TownerStruct &farmer)
{
	switch (Quests[Q_FARMER]._qactive) {
	case QUEST_NOTAVAIL:
	case QUEST_INIT:
		if (player.HasItem(IDI_RUNEBOMB)) {
			InitQTextMsg(TEXT_FARMER2);
			Quests[Q_FARMER]._qactive = QUEST_ACTIVE;
			Quests[Q_FARMER]._qvar1 = 1;
			Quests[Q_FARMER]._qmsg = TEXT_FARMER1;
			Quests[Q_FARMER]._qlog = true;
			if (gbIsMultiplayer)
				NetSendCmdQuest(true, Q_FARMER);
			break;
		}

		if (!player._pLvlVisited[9] && player._pLevel < 15) {
			_speech_id qt = TEXT_FARMER8;
			if (player._pLvlVisited[2])
				qt = TEXT_FARMER5;
			if (player._pLvlVisited[5])
				qt = TEXT_FARMER7;
			if (player._pLvlVisited[7])
				qt = TEXT_FARMER9;
			InitQTextMsg(qt);
			break;
		}

		InitQTextMsg(TEXT_FARMER1);
		Quests[Q_FARMER]._qactive = QUEST_ACTIVE;
		Quests[Q_FARMER]._qvar1 = 1;
		Quests[Q_FARMER]._qlog = true;
		Quests[Q_FARMER]._qmsg = TEXT_FARMER1;
		SpawnRuneBomb(farmer.position + Displacement { 1, 0 });
		if (gbIsMultiplayer)
			NetSendCmdQuest(true, Q_FARMER);
		break;
	case QUEST_ACTIVE:
		InitQTextMsg(player.HasItem(IDI_RUNEBOMB) ? TEXT_FARMER2 : TEXT_FARMER3);
		break;
	case QUEST_DONE:
		InitQTextMsg(TEXT_FARMER4);
		SpawnRewardItem(IDI_AURIC, farmer.position + Displacement { 1, 0 });
		Quests[Q_FARMER]._qactive = QUEST_HIVE_DONE;
		Quests[Q_FARMER]._qlog = false;
		if (gbIsMultiplayer)
			NetSendCmdQuest(true, Q_FARMER);
		break;
	default:
		InitQTextMsg(TEXT_FARMER4);
		break;
	}
}

void TalkToCowFarmer(PlayerStruct &player, TownerStruct &cowFarmer)
{
	if (player.TryRemoveInvItemById(IDI_GREYSUIT)) {
		InitQTextMsg(TEXT_JERSEY7);
	}

	if (player.TryRemoveInvItemById(IDI_BROWNSUIT)) {
		SpawnUnique(UITEM_BOVINE, cowFarmer.position + DIR_SE);
		InitQTextMsg(TEXT_JERSEY8);
		Quests[Q_JERSEY]._qactive = QUEST_DONE;
		return;
	}

	if (player.HasItem(IDI_RUNEBOMB)) {
		InitQTextMsg(TEXT_JERSEY5);
		Quests[Q_JERSEY]._qactive = QUEST_ACTIVE;
		Quests[Q_JERSEY]._qvar1 = 1;
		Quests[Q_JERSEY]._qmsg = TEXT_JERSEY4;
		Quests[Q_JERSEY]._qlog = true;
		return;
	}

	switch (Quests[Q_JERSEY]._qactive) {
	case QUEST_NOTAVAIL:
	case QUEST_INIT:
		InitQTextMsg(TEXT_JERSEY1);
		Quests[Q_JERSEY]._qactive = QUEST_HIVE_TEASE1;
		if (gbIsMultiplayer)
			NetSendCmdQuest(true, Q_JERSEY);
		break;
	case QUEST_ACTIVE:
		InitQTextMsg(TEXT_JERSEY5);
		break;
	case QUEST_DONE:
		InitQTextMsg(TEXT_JERSEY1);
		break;
	case QUEST_HIVE_TEASE1:
		InitQTextMsg(TEXT_JERSEY2);
		Quests[Q_JERSEY]._qactive = QUEST_HIVE_TEASE2;
		if (gbIsMultiplayer)
			NetSendCmdQuest(true, Q_JERSEY);
		break;
	case QUEST_HIVE_TEASE2:
		InitQTextMsg(TEXT_JERSEY3);
		Quests[Q_JERSEY]._qactive = QUEST_HIVE_ACTIVE;
		if (gbIsMultiplayer)
			NetSendCmdQuest(true, Q_JERSEY);
		break;
	case QUEST_HIVE_ACTIVE:
		if (!player._pLvlVisited[9] && player._pLevel < 15) {
			_speech_id qt = TEXT_JERSEY12;
			switch (GenerateRnd(4)) {
			case 0:
				qt = TEXT_JERSEY9;
				break;
			case 1:
				qt = TEXT_JERSEY10;
				break;
			case 2:
				qt = TEXT_JERSEY11;
				break;
			}
			InitQTextMsg(qt);
			break;
		}

		InitQTextMsg(TEXT_JERSEY4);
		Quests[Q_JERSEY]._qactive = QUEST_ACTIVE;
		Quests[Q_JERSEY]._qvar1 = 1;
		Quests[Q_JERSEY]._qmsg = TEXT_JERSEY4;
		Quests[Q_JERSEY]._qlog = true;
		SpawnRuneBomb(cowFarmer.position + Displacement { 1, 0 });
		if (gbIsMultiplayer)
			NetSendCmdQuest(true, Q_JERSEY);
		break;
	default:
		InitQTextMsg(TEXT_JERSEY5);
		break;
	}
}

void TalkToGirl(PlayerStruct &player, TownerStruct &girl)
{
	if (Quests[Q_GIRL]._qactive != QUEST_DONE && player.TryRemoveInvItemById(IDI_THEODORE)) {
		InitQTextMsg(TEXT_GIRL4);
		CreateAmulet(girl.position, 13, false, true);
		Quests[Q_GIRL]._qlog = false;
		Quests[Q_GIRL]._qactive = QUEST_DONE;
		if (gbIsMultiplayer)
			NetSendCmdQuest(true, Q_GIRL);
	}

	switch (Quests[Q_GIRL]._qactive) {
	case QUEST_NOTAVAIL:
	case QUEST_INIT:
		InitQTextMsg(TEXT_GIRL2);
		Quests[Q_GIRL]._qactive = QUEST_ACTIVE;
		Quests[Q_GIRL]._qvar1 = 1;
		Quests[Q_GIRL]._qlog = true;
		Quests[Q_GIRL]._qmsg = TEXT_GIRL2;
		if (gbIsMultiplayer)
			NetSendCmdQuest(true, Q_GIRL);
		return;
	case QUEST_ACTIVE:
		InitQTextMsg(TEXT_GIRL3);
		return;
	default:
		PlaySFX(Texts[TEXT_GIRL1].sfxnr);
		return;
	}
}

const TownerInit TownerInitList[] = {
	// clang-format off
	// type         position    dir     init           talk
	{ TOWN_SMITH,   { 62, 63 }, DIR_SW, InitSmith,     TalkToBlackSmith  },
	{ TOWN_HEALER,  { 55, 79 }, DIR_SE, InitHealer,    TalkToHealer      },
	{ TOWN_DEADGUY, { 24, 32 }, DIR_N,  InitTownDead,  TalkToDeadguy     },
	{ TOWN_TAVERN,  { 55, 62 }, DIR_SW, InitBarOwner,  TalkToBarOwner    },
	{ TOWN_STORY,   { 62, 71 }, DIR_S,  InitTeller,    TalkToStoryteller },
	{ TOWN_DRUNK,   { 71, 84 }, DIR_S,  InitDrunk,     TalkToDrunk       },
	{ TOWN_WITCH,   { 80, 20 }, DIR_S,  InitWitch,     TalkToWitch       },
	{ TOWN_BMAID,   { 43, 66 }, DIR_S,  InitBarmaid,   TalkToBarmaid     },
	{ TOWN_PEGBOY,  { 11, 53 }, DIR_S,  InitBoy,       TalkToBoy         },
	{ TOWN_COW,     { 58, 16 }, DIR_SW, InitCows,      TalkToCow         },
	{ TOWN_COW,     { 56, 14 }, DIR_NW, InitCows,      TalkToCow         },
	{ TOWN_COW,     { 59, 20 }, DIR_N,  InitCows,      TalkToCow         },
	{ TOWN_COWFARM, { 61, 22 }, DIR_SW, InitCowFarmer, TalkToCowFarmer   },
	{ TOWN_FARMER,  { 62, 16 }, DIR_S,  InitFarmer,    TalkToFarmer      },
	{ TOWN_GIRL,    { 77, 43 }, DIR_S,  InitGirl,      TalkToGirl        },
	// clang-format on
};

} // namespace

TownerStruct Towners[NUM_TOWNERS];

/** Contains the data related to quest gossip for each towner ID. */
_speech_id QuestDialogTable[NUM_TOWNER_TYPES][MAXQUESTS] = {
	// clang-format off
	//                 Q_ROCK,       Q_MUSHROOM,  Q_GARBUD,  Q_ZHAR,    Q_VEIL,     Q_DIABLO,   Q_BUTCHER,   Q_LTBANNER,   Q_BLIND,     Q_BLOOD,     Q_ANVIL,      Q_WARLORD,    Q_SKELKING,  Q_PWATER,      Q_SCHAMB,   Q_BETRAYER,  Q_GRAVE,     Q_FARMER,  Q_GIRL,    Q_TRADER,  Q_DEFILER, Q_NAKRUL,  Q_CORNSTN, Q_JERSEY
	/*TOWN_SMITH*/   { TEXT_INFRA6,  TEXT_MUSH6,  TEXT_NONE, TEXT_NONE, TEXT_VEIL5, TEXT_NONE,  TEXT_BUTCH5, TEXT_BANNER6, TEXT_BLIND5, TEXT_BLOOD5, TEXT_ANVIL6,  TEXT_WARLRD5, TEXT_KING7,  TEXT_POISON7,  TEXT_BONE5, TEXT_VILE9,  TEXT_GRAVE2, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_HEALER*/  { TEXT_INFRA3,  TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_VEIL3, TEXT_NONE,  TEXT_BUTCH3, TEXT_BANNER4, TEXT_BLIND3, TEXT_BLOOD3, TEXT_ANVIL3,  TEXT_WARLRD3, TEXT_KING5,  TEXT_POISON4,  TEXT_BONE3, TEXT_VILE7,  TEXT_GRAVE3, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_DEADGUY*/ { TEXT_NONE,    TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE,  TEXT_NONE,  TEXT_NONE,   TEXT_NONE,    TEXT_NONE,   TEXT_NONE,   TEXT_NONE,    TEXT_NONE,    TEXT_NONE,   TEXT_NONE,     TEXT_NONE,  TEXT_NONE,   TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_TAVERN*/  { TEXT_INFRA2,  TEXT_MUSH2,  TEXT_NONE, TEXT_NONE, TEXT_VEIL2, TEXT_NONE,  TEXT_BUTCH2, TEXT_NONE,    TEXT_BLIND2, TEXT_BLOOD2, TEXT_ANVIL2,  TEXT_WARLRD2, TEXT_KING3,  TEXT_POISON2,  TEXT_BONE2, TEXT_VILE4,  TEXT_GRAVE5, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_STORY*/   { TEXT_INFRA1,  TEXT_MUSH1,  TEXT_NONE, TEXT_NONE, TEXT_VEIL1, TEXT_VILE3, TEXT_BUTCH1, TEXT_BANNER1, TEXT_BLIND1, TEXT_BLOOD1, TEXT_ANVIL1,  TEXT_WARLRD1, TEXT_KING1,  TEXT_POISON1,  TEXT_BONE1, TEXT_VILE2,  TEXT_GRAVE6, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_DRUNK*/   { TEXT_INFRA8,  TEXT_MUSH7,  TEXT_NONE, TEXT_NONE, TEXT_VEIL6, TEXT_NONE,  TEXT_BUTCH6, TEXT_BANNER7, TEXT_BLIND6, TEXT_BLOOD6, TEXT_ANVIL8,  TEXT_WARLRD6, TEXT_KING8,  TEXT_POISON8,  TEXT_BONE6, TEXT_VILE10, TEXT_GRAVE7, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_WITCH*/   { TEXT_INFRA9,  TEXT_MUSH9,  TEXT_NONE, TEXT_NONE, TEXT_VEIL7, TEXT_NONE,  TEXT_BUTCH7, TEXT_BANNER8, TEXT_BLIND7, TEXT_BLOOD7, TEXT_ANVIL9,  TEXT_WARLRD7, TEXT_KING9,  TEXT_POISON9,  TEXT_BONE7, TEXT_VILE11, TEXT_GRAVE1, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_BMAID*/   { TEXT_INFRA4,  TEXT_MUSH5,  TEXT_NONE, TEXT_NONE, TEXT_VEIL4, TEXT_NONE,  TEXT_BUTCH4, TEXT_BANNER5, TEXT_BLIND4, TEXT_BLOOD4, TEXT_ANVIL4,  TEXT_WARLRD4, TEXT_KING6,  TEXT_POISON6,  TEXT_BONE4, TEXT_VILE8,  TEXT_GRAVE8, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_PEGBOY*/  { TEXT_INFRA10, TEXT_MUSH13, TEXT_NONE, TEXT_NONE, TEXT_VEIL8, TEXT_NONE,  TEXT_BUTCH8, TEXT_BANNER9, TEXT_BLIND8, TEXT_BLOOD8, TEXT_ANVIL10, TEXT_WARLRD8, TEXT_KING10, TEXT_POISON10, TEXT_BONE8, TEXT_VILE12, TEXT_GRAVE9, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_COW*/     { TEXT_NONE,    TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE,  TEXT_NONE,  TEXT_NONE,   TEXT_NONE,    TEXT_NONE,   TEXT_NONE,   TEXT_NONE,    TEXT_NONE,    TEXT_NONE,   TEXT_NONE,     TEXT_NONE,  TEXT_NONE,   TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_FARMER*/  { TEXT_NONE,    TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE,  TEXT_NONE,  TEXT_NONE,   TEXT_NONE,    TEXT_NONE,   TEXT_NONE,   TEXT_NONE,    TEXT_NONE,    TEXT_NONE,   TEXT_NONE,     TEXT_NONE,  TEXT_NONE,   TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_GIRL*/    { TEXT_NONE,    TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE,  TEXT_NONE,  TEXT_NONE,   TEXT_NONE,    TEXT_NONE,   TEXT_NONE,   TEXT_NONE,    TEXT_NONE,    TEXT_NONE,   TEXT_NONE,     TEXT_NONE,  TEXT_NONE,   TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	/*TOWN_COWFARM*/ { TEXT_NONE,    TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE,  TEXT_NONE,  TEXT_NONE,   TEXT_NONE,    TEXT_NONE,   TEXT_NONE,   TEXT_NONE,    TEXT_NONE,    TEXT_NONE,   TEXT_NONE,     TEXT_NONE,  TEXT_NONE,   TEXT_NONE,   TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE, TEXT_NONE },
	// clang-format on
};

void InitTowners()
{
	assert(CowCels == nullptr);

	CowCels = LoadFileInMem("Towners\\Animals\\Cow.CEL");

	int i = 0;
	for (const auto &townerInit : TownerInitList) {
		switch (townerInit.type) {
		case TOWN_DEADGUY:
			if (Quests[Q_BUTCHER]._qactive == QUEST_NOTAVAIL || Quests[Q_BUTCHER]._qactive == QUEST_DONE)
				continue;
			break;
		case TOWN_FARMER:
			if (!gbIsHellfire || sgGameInitInfo.bCowQuest != 0)
				continue;
			break;
		case TOWN_COWFARM:
			if (!gbIsHellfire || sgGameInitInfo.bCowQuest == 0 || Quests[Q_FARMER]._qactive == 10)
				continue;
			break;
		case TOWN_GIRL:
			if (!gbIsHellfire || sgGameInitInfo.bTheoQuest == 0 || !Players->_pLvlVisited[17])
				continue;
			break;
		default:
			break;
		}

		InitTownerInfo(i, townerInit);
		i++;
	}
}

void FreeTownerGFX()
{
	for (auto &towner : Towners) {
		towner.data = nullptr;
	}

	CowCels = nullptr;
}

void ProcessTowners()
{
	// BUGFIX: should be `i < numtowners`, was `i < NUM_TOWNERS`
	for (auto &towner : Towners) {
		if (towner._ttype == TOWN_DEADGUY) {
			TownDead(towner);
		}

		towner._tAnimCnt++;
		if (towner._tAnimCnt < towner._tAnimDelay) {
			continue;
		}

		towner._tAnimCnt = 0;

		if (towner.animOrderSize > 0) {
			towner._tAnimFrameCnt++;
			if (towner._tAnimFrameCnt > towner.animOrderSize - 1)
				towner._tAnimFrameCnt = 0;

			towner._tAnimFrame = towner.animOrder[towner._tAnimFrameCnt];
			continue;
		}

		towner._tAnimFrame++;
		if (towner._tAnimFrame > towner._tAnimLen)
			towner._tAnimFrame = 1;
	}
}

void TalkToTowner(PlayerStruct &player, int t)
{
	auto &towner = Towners[t];

	if (player.position.tile.WalkingDistance(towner.position) >= 2)
		return;

	if (pcurs >= CURSOR_FIRSTITEM) {
		return;
	}

	towner.talk(player, towner);
}

} // namespace devilution
