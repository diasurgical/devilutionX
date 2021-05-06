/**
 * @file towners.cpp
 *
 * Implementation of functionality for loading and spawning towners.
 */
#include "towners.h"

#include "cursor.h"
#include "inv.h"
#include "minitext.h"
#include "stores.h"
#include "utils/language.h"

namespace devilution {
namespace {

std::unique_ptr<byte[]> CowCels;
bool storeflag;
int CowMsg;
int NumberOfTowners;
int CowClicks;

/**
 * Maps from active cow sound effect index and player class to sound
 * effect ID for interacting with cows in Tristram.
 *
 * ref: enum _sfx_id
 * ref: enum HeroClass
 */
const _sfx_id SnSfx[3][enum_size<HeroClass>::value] = {
	{ PS_WARR52, PS_ROGUE52, PS_MAGE52, PS_MONK52, PS_ROGUE52, PS_WARR52 },
	{ PS_WARR49, PS_ROGUE49, PS_MAGE49, PS_MONK49, PS_ROGUE49, PS_WARR49 },
	{ PS_WARR50, PS_ROGUE50, PS_MAGE50, PS_MONK50, PS_ROGUE50, PS_WARR50 },
};

/** Specifies the animation frame sequence of a given NPC. */
char AnimOrder[6][148] = {
	{ 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
	    5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	    15, 5, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 2, 3, 4,
	    -1 },
	{ 1, 2, 3, 3, 2, 1, 20, 19, 19, 20,
	    1, 2, 3, 3, 2, 1, 20, 19, 19, 20,
	    1, 2, 3, 3, 2, 1, 20, 19, 19, 20,
	    1, 2, 3, 3, 2, 1, 20, 19, 19, 20,
	    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	    11, 12, 13, 14, 15, 16, 15, 14, 13, 12,
	    11, 10, 9, 8, 7, 6, 5, 4, 5, 6,
	    7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	    15, 14, 13, 12, 11, 10, 9, 8, 7, 6,
	    5, 4, 5, 6, 7, 8, 9, 10, 11, 12,
	    13, 14, 15, 16, 17, 18, 19, 20, -1 },
	{ 1, 1, 25, 25, 24, 23, 22, 21, 20, 19,
	    18, 17, 16, 15, 16, 17, 18, 19, 20, 21,
	    22, 23, 24, 25, 25, 25, 1, 1, 1, 25,
	    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	    11, 12, 13, 14, 15, 14, 13, 12, 11, 10,
	    9, 8, 7, 6, 5, 4, 3, 2, 1, -1 },
	{ 1, 2, 3, 3, 2, 1, 16, 15, 14, 14,
	    16, 1, 2, 3, 3, 2, 1, 16, 15, 14,
	    14, 15, 16, 1, 2, 3, 3, 2, 1, 16,
	    15, 14, 14, 15, 16, 1, 2, 3, 3, 2,
	    1, 16, 15, 14, 14, 15, 16, 1, 2, 3,
	    3, 2, 1, 16, 15, 14, 14, 15, 16, 1,
	    2, 3, 3, 2, 1, 16, 15, 14, 14, 15,
	    16, 1, 2, 3, 3, 2, 1, 16, 15, 14,
	    14, 15, 16, 1, 2, 3, 2, 1, 16, 15,
	    14, 14, 15, 16, 1, 2, 3, 4, 5, 6,
	    7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	    -1 },
	{ 1, 1, 1, 2, 3, 4, 5, 6, 7, 8,
	    9, 10, 11, 11, 11, 11, 12, 13, 14, 15,
	    16, 17, 18, 18, 1, 1, 1, 18, 17, 16,
	    15, 14, 13, 12, 11, 10, 11, 12, 13, 14,
	    15, 16, 17, 18, 1, 2, 3, 4, 5, 5,
	    5, 4, 3, 2, -1 },
	{ 4, 4, 4, 5, 6, 6, 6, 5, 4, 15,
	    14, 13, 13, 13, 14, 15, 4, 5, 6, 6,
	    6, 5, 4, 4, 4, 5, 6, 6, 6, 5,
	    4, 15, 14, 13, 13, 13, 14, 15, 4, 5,
	    6, 6, 6, 5, 4, 4, 4, 5, 6, 6,
	    6, 5, 4, 15, 14, 13, 13, 13, 14, 15,
	    4, 5, 6, 6, 6, 5, 4, 3, 2, 1,
	    19, 18, 19, 1, 2, 1, 19, 18, 19, 1,
	    2, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	    10, 11, 12, 13, 14, 15, 15, 15, 14, 13,
	    13, 13, 13, 14, 15, 15, 15, 14, 13, 12,
	    12, 12, 11, 10, 10, 10, 9, 8, 9, 10,
	    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	    1, 2, 1, 19, 18, 19, 1, 2, 1, 2,
	    3, -1 }
};
/** Specifies the start coordinates of the cows in Tristram. */
Point TownCowPositions[] = { { 58, 16 }, { 56, 14 }, { 59, 20 } };
/** Specifies the start directions of the cows in Tristram. */
direction TownCowDir[] = { DIR_SW, DIR_NW, DIR_N };
/**
 * Maps from direction to coordinate delta, which is used when
 * placing cows in Tristram. A single cow may require space of up
 * to three tiles when being placed on the map.
 */
Point CowOffsets[8] = { { -1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 } };

/** Specifies the active sound effect ID for interacting with cows. */
_sfx_id CowPlaying = SFX_NONE;

void CowSFX(PlayerStruct &player)
{
	if (CowPlaying != -1 && effect_is_playing(CowPlaying))
		return;

	CowClicks++;

	if (gbIsSpawn) {
		if (CowClicks == 4) {
			CowClicks = 0;
			CowPlaying = TSFX_COW2;
		} else {
			CowPlaying = TSFX_COW1;
		}
	} else {
		if (CowClicks >= 8) {
			PlaySfxLoc(TSFX_COW1, player.position.tile.x, player.position.tile.y + 5);
			CowClicks = 4;
			CowPlaying = SnSfx[CowMsg][static_cast<std::size_t>(player._pClass)]; /* snSFX is local */
			CowMsg++;
			if (CowMsg >= 3)
				CowMsg = 0;
		} else {
			CowPlaying = CowClicks == 4 ? TSFX_COW2 : TSFX_COW1;
		}
	}

	PlaySfxLoc(CowPlaying, player.position.tile.x, player.position.tile.y);
}

int GetActiveTowner(int t)
{
	for (int i = 0; i < NumberOfTowners; i++) {
		if (towners[i]._ttype == t)
			return i;
	}

	return -1;
}

void SetTownerGPtrs(byte *pData, byte **pAnim)
{
	for (int i = 0; i < 8; i++) {
		pAnim[i] = CelGetFrameStart(pData, i);
	}
}

void NewTownerAnim(int tnum, byte *pAnim, uint8_t numFrames, int delay)
{
	towners[tnum]._tAnimData = pAnim;
	towners[tnum]._tAnimLen = numFrames;
	towners[tnum]._tAnimFrame = 1;
	towners[tnum]._tAnimCnt = 0;
	towners[tnum]._tAnimDelay = delay;
}

void InitTownerInfo(int i, int w, bool sel, _talker_id t, Point position, int ao)
{
	towners[i]._tSelFlag = sel;
	towners[i]._tAnimWidth = w;
	towners[i]._tMsgSaid = false;
	towners[i]._ttype = t;
	towners[i].position = position;
	dMonster[position.x][position.y] = i + 1;
	towners[i]._tAnimOrder = ao;
	towners[i]._tSeed = AdvanceRndSeed();
}

void InitQstSnds(int id, _talker_id type)
{
	for (int i = 0; i < MAXQUESTS; i++) {
		towners[id].qsts[i]._qsttype = quests[i]._qtype;
		towners[id].qsts[i]._qstmsg = Qtalklist[type][i];
		towners[id].qsts[i]._qstmsgact = Qtalklist[type][i] != TEXT_NONE;
	}
}

/**
 * @brief Load Griswold into the game
 */
void InitSmith()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_SMITH, { 62, 63 }, 0);
	InitQstSnds(NumberOfTowners, TOWN_SMITH);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Smith\\SmithN.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 16;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_SW], towners[NumberOfTowners]._tNFrames, 3);
	towners[NumberOfTowners]._tName = _("Griswold the Blacksmith");
	NumberOfTowners++;
}

void InitBarOwner()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_TAVERN, { 55, 62 }, 3);
	InitQstSnds(NumberOfTowners, TOWN_TAVERN);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\TwnF\\TwnFN.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 16;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_SW], towners[NumberOfTowners]._tNFrames, 3);
	towners[NumberOfTowners]._tName = _("Ogden the Tavern owner");
	NumberOfTowners++;
}

void InitTownDead()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_DEADGUY, { 24, 32 }, -1);
	InitQstSnds(NumberOfTowners, TOWN_DEADGUY);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Butch\\Deadguy.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 8;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_N], towners[NumberOfTowners]._tNFrames, 6);
	towners[NumberOfTowners]._tName = _("Wounded Townsman");
	NumberOfTowners++;
}

void InitWitch()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_WITCH, { 80, 20 }, 5);
	InitQstSnds(NumberOfTowners, TOWN_WITCH);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\TownWmn1\\Witch.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 19;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_S], towners[NumberOfTowners]._tNFrames, 6);
	towners[NumberOfTowners]._tName = _("Adria the Witch");
	NumberOfTowners++;
}

void InitBarmaid()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_BMAID, { 43, 66 }, -1);
	InitQstSnds(NumberOfTowners, TOWN_BMAID);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\TownWmn1\\WmnN.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 18;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_S], towners[NumberOfTowners]._tNFrames, 6);
	towners[NumberOfTowners]._tName = _("Gillian the Barmaid");
	NumberOfTowners++;
}

void InitBoy()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_PEGBOY, { 11, 53 }, -1);
	InitQstSnds(NumberOfTowners, TOWN_PEGBOY);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\TownBoy\\PegKid1.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 20;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_S], towners[NumberOfTowners]._tNFrames, 6);
	towners[NumberOfTowners]._tName = _("Wirt the Peg-legged boy");
	NumberOfTowners++;
}

void InitHealer()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_HEALER, { 55, 79 }, 1);
	InitQstSnds(NumberOfTowners, TOWN_HEALER);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Healer\\Healer.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 20;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_SE], towners[NumberOfTowners]._tNFrames, 6);
	towners[NumberOfTowners]._tName = _("Pepin the Healer");
	NumberOfTowners++;
}

void InitTeller()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_STORY, { 62, 71 }, 2);
	InitQstSnds(NumberOfTowners, TOWN_STORY);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Strytell\\Strytell.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 25;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_S], towners[NumberOfTowners]._tNFrames, 3);
	towners[NumberOfTowners]._tName = _("Cain the Elder");
	NumberOfTowners++;
}

void InitDrunk()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_DRUNK, { 71, 84 }, 4);
	InitQstSnds(NumberOfTowners, TOWN_DRUNK);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Drunk\\TwnDrunk.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 18;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_S], towners[NumberOfTowners]._tNFrames, 3);
	towners[NumberOfTowners]._tName = _("Farnham the Drunk");
	NumberOfTowners++;
}

void InitCows()
{
	assert(CowCels == nullptr);

	CowCels = LoadFileInMem("Towners\\Animals\\Cow.CEL");

	for (int i = 0; i < 3; i++) {
		direction dir = TownCowDir[i];
		Point position = TownCowPositions[i];
		InitTownerInfo(NumberOfTowners, 128, false, TOWN_COW, position, -1);
		towners[NumberOfTowners]._tNData = nullptr;
		SetTownerGPtrs(CowCels.get(), towners[NumberOfTowners]._tNAnim);
		towners[NumberOfTowners]._tNFrames = 12;
		NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[dir], towners[NumberOfTowners]._tNFrames, 3);
		towners[NumberOfTowners]._tAnimFrame = GenerateRnd(11) + 1;
		towners[NumberOfTowners]._tSelFlag = true;
		towners[NumberOfTowners]._tName = _("Cow");

		Point offset = position + CowOffsets[dir];
		if (dMonster[position.x][offset.y] == 0)
			dMonster[position.x][offset.y] = -(NumberOfTowners + 1);
		if (dMonster[offset.x][position.y] == 0)
			dMonster[offset.x][position.y] = -(NumberOfTowners + 1);
		if (dMonster[offset.x][offset.y] == 0)
			dMonster[offset.x][offset.y] = -(NumberOfTowners + 1);

		NumberOfTowners++;
	}
}

void InitFarmer()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_FARMER, { 62, 16 }, -1);
	InitQstSnds(NumberOfTowners, TOWN_FARMER);
	towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Farmer\\Farmrn2.CEL");
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 15;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_S], towners[NumberOfTowners]._tNFrames, 3);
	towners[NumberOfTowners]._tName = _("Lester the farmer");
	NumberOfTowners++;
}

void InitCowFarmer()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_COWFARM, { 61, 22 }, -1);
	InitQstSnds(NumberOfTowners, TOWN_COWFARM);
	if (quests[Q_JERSEY]._qactive != QUEST_DONE) {
		towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Farmer\\cfrmrn2.CEL");
	} else {
		towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Farmer\\mfrmrn2.CEL");
	}
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 15;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_SW], towners[NumberOfTowners]._tNFrames, 3);
	towners[NumberOfTowners]._tName = _("Complete Nut");
	NumberOfTowners++;
}

void InitGirl()
{
	InitTownerInfo(NumberOfTowners, 96, true, TOWN_GIRL, { 77, 43 }, -1);
	InitQstSnds(NumberOfTowners, TOWN_GIRL);
	if (quests[Q_GIRL]._qactive != QUEST_DONE) {
		towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Girl\\Girlw1.CEL");
	} else {
		towners[NumberOfTowners]._tNData = LoadFileInMem("Towners\\Girl\\Girls1.CEL");
	}
	for (auto &animation : towners[NumberOfTowners]._tNAnim) {
		animation = towners[NumberOfTowners]._tNData.get();
	}
	towners[NumberOfTowners]._tNFrames = 20;
	NewTownerAnim(NumberOfTowners, towners[NumberOfTowners]._tNAnim[DIR_S], towners[NumberOfTowners]._tNFrames, 6);
	towners[NumberOfTowners]._tName = "Celia";
	NumberOfTowners++;
}

void TownCtrlMsg(int i)
{
	if (i == -1)
		return;

	if (!towners[i]._tbtcnt) {
		return;
	}

	PlayerStruct *player = towners[i]._tTalkingToPlayer;
	int dx = abs(towners[i].position.x - player->position.tile.x);
	int dy = abs(towners[i].position.y - player->position.tile.y);
	if (dx < 2 && dy < 2) {
		return;
	}

	towners[i]._tbtcnt = false;
	qtextflag = false;
	stream_stop();
}

void TownBlackSmith()
{
	int i = GetActiveTowner(TOWN_SMITH);
	TownCtrlMsg(i);
}

void TownBarOwner()
{
	int i = GetActiveTowner(TOWN_TAVERN);
	TownCtrlMsg(i);
}

void TownDead()
{
	int tidx = GetActiveTowner(TOWN_DEADGUY);
	TownCtrlMsg(tidx);
	if (!qtextflag) {
		if (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && !quests[Q_BUTCHER]._qlog) {
			return;
		}
		if (quests[Q_BUTCHER]._qactive != QUEST_INIT) {
			towners[tidx]._tAnimDelay = 1000;
			towners[tidx]._tAnimFrame = 1;
			towners[tidx]._tName = _("Slain Townsman");
		}
	}
	if (quests[Q_BUTCHER]._qactive != QUEST_INIT)
		towners[tidx]._tAnimCnt = 0;
}

void TownHealer()
{
	int i = GetActiveTowner(TOWN_HEALER);
	TownCtrlMsg(i);
}

void TownStory()
{
	int i = GetActiveTowner(TOWN_STORY);
	TownCtrlMsg(i);
}

void TownDrunk()
{
	int i = GetActiveTowner(TOWN_DRUNK);
	TownCtrlMsg(i);
}

void TownBoy()
{
	int i = GetActiveTowner(TOWN_PEGBOY);
	TownCtrlMsg(i);
}

void TownWitch()
{
	int i = GetActiveTowner(TOWN_WITCH);
	TownCtrlMsg(i);
}

void TownBarMaid()
{
	int i = GetActiveTowner(TOWN_BMAID);
	TownCtrlMsg(i);
}

void TownCow()
{
	int i = GetActiveTowner(TOWN_COW);
	TownCtrlMsg(i);
}

void TownFarmer()
{
	int i = GetActiveTowner(TOWN_FARMER);
	TownCtrlMsg(i);
}

void TownCowFarmer()
{
	int i = GetActiveTowner(TOWN_COWFARM);
	TownCtrlMsg(i);
}

void TownGirl()
{
	int i = GetActiveTowner(TOWN_GIRL);
	TownCtrlMsg(i);
}
void TownerTalk(int first)
{
	CowClicks = 0;
	CowMsg = 0;
	storeflag = true;
	InitQTextMsg(first);
}

void TalkToBarOwner(PlayerStruct &player, TownerStruct &barOwner)
{
	if (!player._pLvlVisited[0] && !barOwner._tMsgSaid) {
		barOwner._tbtcnt = true;
		barOwner._tTalkingToPlayer = &player;
		InitQTextMsg(TEXT_INTRO);
		barOwner._tMsgSaid = true;
	}
	if ((player._pLvlVisited[2] || player._pLvlVisited[4]) && quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) {
		if (quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) {
			if (quests[Q_SKELKING]._qvar2 == 0 && !barOwner._tMsgSaid) {
				quests[Q_SKELKING]._qvar2 = 1;
				quests[Q_SKELKING]._qlog = true;
				if (quests[Q_SKELKING]._qactive == QUEST_INIT) {
					quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
					quests[Q_SKELKING]._qvar1 = 1;
				}
				barOwner._tbtcnt = true;
				barOwner._tTalkingToPlayer = &player;
				InitQTextMsg(TEXT_KING2);
				barOwner._tMsgSaid = true;
				NetSendCmdQuest(true, Q_SKELKING);
			}
		}
		if (quests[Q_SKELKING]._qactive == QUEST_DONE && quests[Q_SKELKING]._qvar2 == 1 && !barOwner._tMsgSaid) {
			quests[Q_SKELKING]._qvar2 = 2;
			quests[Q_SKELKING]._qvar1 = 2;
			barOwner._tbtcnt = true;
			barOwner._tTalkingToPlayer = &player;
			InitQTextMsg(TEXT_KING4);
			barOwner._tMsgSaid = true;
			NetSendCmdQuest(true, Q_SKELKING);
		}
	}
	if (!gbIsMultiplayer) {
		if (player._pLvlVisited[3] && quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL) {
			if ((quests[Q_LTBANNER]._qactive == QUEST_INIT || quests[Q_LTBANNER]._qactive == QUEST_ACTIVE) && quests[Q_LTBANNER]._qvar2 == 0 && !barOwner._tMsgSaid) {
				quests[Q_LTBANNER]._qvar2 = 1;
				if (quests[Q_LTBANNER]._qactive == QUEST_INIT) {
					quests[Q_LTBANNER]._qvar1 = 1;
					quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
				}
				quests[Q_LTBANNER]._qlog = true;
				barOwner._tbtcnt = true;
				barOwner._tTalkingToPlayer = &player;
				InitQTextMsg(TEXT_BANNER2);
				barOwner._tMsgSaid = true;
			}
			int i;
			if (quests[Q_LTBANNER]._qvar2 == 1 && player.HasItem(IDI_BANNER, &i) && !barOwner._tMsgSaid) {
				quests[Q_LTBANNER]._qactive = QUEST_DONE;
				quests[Q_LTBANNER]._qvar1 = 3;
				player.RemoveInvItem(i);
				SpawnUnique(UITEM_HARCREST, barOwner.position.x, barOwner.position.y + 1);
				barOwner._tbtcnt = true;
				barOwner._tTalkingToPlayer = &player;
				InitQTextMsg(TEXT_BANNER3);
				barOwner._tMsgSaid = true;
			}
		}
	}
	if (!qtextflag) {
		TownerTalk(TEXT_OGDEN1);
		if (storeflag) {
			StartStore(STORE_TAVERN);
		}
	}
}

void TalkToDeadguy(PlayerStruct &player, TownerStruct &deadguy)
{
	if (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qvar1 == 1) {
		deadguy._tbtcnt = true;
		deadguy._tTalkingToPlayer = &player;
		quests[Q_BUTCHER]._qvar1 = 1;
		player.PlaySpecificSpeach(8);
		deadguy._tMsgSaid = true;
		return;
	}

	if (quests[Q_BUTCHER]._qactive == QUEST_DONE && quests[Q_BUTCHER]._qvar1 == 1) {
		quests[Q_BUTCHER]._qvar1 = 1;
		deadguy._tbtcnt = true;
		deadguy._tTalkingToPlayer = &player;
		deadguy._tMsgSaid = true;
		return;
	}

	if (quests[Q_BUTCHER]._qactive == QUEST_INIT || (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qvar1 == 0)) {
		quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
		quests[Q_BUTCHER]._qlog = true;
		quests[Q_BUTCHER]._qmsg = TEXT_BUTCH9;
		quests[Q_BUTCHER]._qvar1 = 1;
		deadguy._tbtcnt = true;
		deadguy._tTalkingToPlayer = &player;
		InitQTextMsg(TEXT_BUTCH9);
		deadguy._tMsgSaid = true;
		NetSendCmdQuest(true, Q_BUTCHER);
		return;
	}
}

void TalkToBlackSmith(PlayerStruct &player, TownerStruct &blackSmith)
{
	if (!gbIsMultiplayer) {
		if (player._pLvlVisited[4] && quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
			if (quests[Q_ROCK]._qvar2 == 0) {
				quests[Q_ROCK]._qvar2 = 1;
				quests[Q_ROCK]._qlog = true;
				if (quests[Q_ROCK]._qactive == QUEST_INIT) {
					quests[Q_ROCK]._qactive = QUEST_ACTIVE;
					quests[Q_ROCK]._qvar1 = 1;
				}
				blackSmith._tbtcnt = true;
				blackSmith._tTalkingToPlayer = &player;
				InitQTextMsg(TEXT_INFRA5);
				blackSmith._tMsgSaid = true;
			}
			int i;
			if (quests[Q_ROCK]._qvar2 == 1 && player.HasItem(IDI_ROCK, &i) && !blackSmith._tMsgSaid) {
				quests[Q_ROCK]._qactive = QUEST_DONE;
				quests[Q_ROCK]._qvar2 = 2;
				quests[Q_ROCK]._qvar1 = 2;
				player.RemoveInvItem(i);
				SpawnUnique(UITEM_INFRARING, blackSmith.position.x, blackSmith.position.y + 1);
				blackSmith._tbtcnt = true;
				blackSmith._tTalkingToPlayer = &player;
				InitQTextMsg(TEXT_INFRA7);
				blackSmith._tMsgSaid = true;
			}
		}
		if (player._pLvlVisited[9] && quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
			if ((quests[Q_ANVIL]._qactive == QUEST_INIT || quests[Q_ANVIL]._qactive == QUEST_ACTIVE) && quests[Q_ANVIL]._qvar2 == 0 && !blackSmith._tMsgSaid) {
				if (quests[Q_ROCK]._qvar2 == 2 || (quests[Q_ROCK]._qactive == QUEST_ACTIVE && quests[Q_ROCK]._qvar2 == 1)) {
					quests[Q_ANVIL]._qvar2 = 1;
					quests[Q_ANVIL]._qlog = true;
					if (quests[Q_ANVIL]._qactive == QUEST_INIT) {
						quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
						quests[Q_ANVIL]._qvar1 = 1;
					}
					blackSmith._tbtcnt = true;
					blackSmith._tTalkingToPlayer = &player;
					InitQTextMsg(TEXT_ANVIL5);
					blackSmith._tMsgSaid = true;
				}
			}
			int i;
			if (quests[Q_ANVIL]._qvar2 == 1 && player.HasItem(IDI_ANVIL, &i)) {
				if (!blackSmith._tMsgSaid) {
					quests[Q_ANVIL]._qactive = QUEST_DONE;
					quests[Q_ANVIL]._qvar2 = 2;
					quests[Q_ANVIL]._qvar1 = 2;
					player.RemoveInvItem(i);
					SpawnUnique(UITEM_GRISWOLD, blackSmith.position.x, blackSmith.position.y + 1);
					blackSmith._tbtcnt = true;
					blackSmith._tTalkingToPlayer = &player;
					InitQTextMsg(TEXT_ANVIL7);
					blackSmith._tMsgSaid = true;
				}
			}
		}
	}
	if (!qtextflag) {
		TownerTalk(TEXT_GRISWOLD1);
		if (storeflag) {
			StartStore(STORE_SMITH);
		}
	}
}

void TalkToWitch(PlayerStruct &player, TownerStruct &witch)
{
	int i;
	if (quests[Q_MUSHROOM]._qactive == QUEST_INIT && player.HasItem(IDI_FUNGALTM, &i)) {
		player.RemoveInvItem(i);
		quests[Q_MUSHROOM]._qactive = QUEST_ACTIVE;
		quests[Q_MUSHROOM]._qlog = true;
		quests[Q_MUSHROOM]._qvar1 = QS_TOMEGIVEN;
		witch._tbtcnt = true;
		witch._tTalkingToPlayer = &player;
		InitQTextMsg(TEXT_MUSH8);
		witch._tMsgSaid = true;
	} else if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
		if (quests[Q_MUSHROOM]._qvar1 >= QS_TOMEGIVEN && quests[Q_MUSHROOM]._qvar1 <= QS_MUSHPICKED) {
			int i;
			if (player.HasItem(IDI_MUSHROOM, &i)) {
				player.RemoveInvItem(i);
				quests[Q_MUSHROOM]._qvar1 = QS_MUSHGIVEN;
				Qtalklist[TOWN_HEALER][Q_MUSHROOM] = TEXT_MUSH3;
				Qtalklist[TOWN_WITCH][Q_MUSHROOM] = TEXT_NONE;
				witch._tbtcnt = true;
				witch._tTalkingToPlayer = &player;
				quests[Q_MUSHROOM]._qmsg = TEXT_MUSH10;
				InitQTextMsg(TEXT_MUSH10);
				witch._tMsgSaid = true;
			} else if (quests[Q_MUSHROOM]._qmsg != TEXT_MUSH9) {
				witch._tbtcnt = true;
				witch._tTalkingToPlayer = &player;
				quests[Q_MUSHROOM]._qmsg = TEXT_MUSH9;
				InitQTextMsg(TEXT_MUSH9);
				witch._tMsgSaid = true;
			}
		} else {
			if (player.HasItem(IDI_SPECELIX)) {
				witch._tbtcnt = true;
				witch._tTalkingToPlayer = &player;
				InitQTextMsg(TEXT_MUSH12);
				quests[Q_MUSHROOM]._qactive = QUEST_DONE;
				witch._tMsgSaid = true;
				AllItemsList[IDI_SPECELIX].iUsable = true;
			} else if (player.HasItem(IDI_BRAIN) && quests[Q_MUSHROOM]._qvar2 != TEXT_MUSH11) {
				witch._tbtcnt = true;
				witch._tTalkingToPlayer = &player;
				quests[Q_MUSHROOM]._qvar2 = TEXT_MUSH11;
				InitQTextMsg(TEXT_MUSH11);
				witch._tMsgSaid = true;
			}
		}
	}
	if (!qtextflag) {
		TownerTalk(TEXT_ADRIA1);
		if (storeflag) {
			StartStore(STORE_WITCH);
		}
	}
}

void TalkToBarmaid(PlayerStruct &player, TownerStruct &barmaid)
{
	if (!player._pLvlVisited[21] && player.HasItem(IDI_MAPOFDOOM)) {
		quests[Q_GRAVE]._qactive = QUEST_ACTIVE;
		quests[Q_GRAVE]._qlog = true;
		quests[Q_GRAVE]._qmsg = TEXT_GRAVE8;
		InitQTextMsg(TEXT_GRAVE8);
		barmaid._tMsgSaid = true;
	}
	if (!qtextflag) {
		TownerTalk(TEXT_GILLIAN1);
		if (storeflag) {
			StartStore(STORE_BARMAID);
		}
	}
}

void TalkToDrunk()
{
	TownerTalk(TEXT_FARNHAM1);
	if (storeflag) {
		StartStore(STORE_DRUNK);
	}
}

void TalkToHealer(PlayerStruct &player, TownerStruct &healer)
{
	if (!gbIsMultiplayer) {
		if (player._pLvlVisited[1] || (gbIsHellfire && player._pLvlVisited[5])) {
			if (!healer._tMsgSaid) {
				if (quests[Q_PWATER]._qactive == QUEST_INIT) {
					quests[Q_PWATER]._qactive = QUEST_ACTIVE;
					quests[Q_PWATER]._qlog = true;
					quests[Q_PWATER]._qmsg = TEXT_POISON3;
					quests[Q_PWATER]._qvar1 = 1;
					healer._tbtcnt = true;
					healer._tTalkingToPlayer = &player;
					InitQTextMsg(TEXT_POISON3);
					healer._tMsgSaid = true;
				} else if (quests[Q_PWATER]._qactive == QUEST_DONE && quests[Q_PWATER]._qvar1 != 2) {
					quests[Q_PWATER]._qvar1 = 2;
					healer._tbtcnt = true;
					healer._tTalkingToPlayer = &player;
					InitQTextMsg(TEXT_POISON5);
					SpawnUnique(UITEM_TRING, healer.position.x, healer.position.y + 1);
					healer._tMsgSaid = true;
				}
			}
		}
		int i;
		if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qmsg == TEXT_MUSH10 && player.HasItem(IDI_BRAIN, &i)) {
			player.RemoveInvItem(i);
			SpawnQuestItem(IDI_SPECELIX, healer.position.x, healer.position.y + 1, 0, 0);
			InitQTextMsg(TEXT_MUSH4);
			quests[Q_MUSHROOM]._qvar1 = QS_BRAINGIVEN;
			Qtalklist[TOWN_HEALER][Q_MUSHROOM] = TEXT_NONE;
		}
	}
	if (!qtextflag) {
		TownerTalk(TEXT_PEPIN1);
		if (storeflag) {
			StartStore(STORE_HEALER);
		}
	}
}

void TalkToBoy()
{
	TownerTalk(TEXT_WIRT1);
	if (storeflag) {
		StartStore(STORE_BOY);
	}
}

void TalkToStoryteller(PlayerStruct &player, TownerStruct &storyteller)
{
	if (!gbIsMultiplayer) {
		int i;
		if (quests[Q_BETRAYER]._qactive == QUEST_INIT && player.HasItem(IDI_LAZSTAFF, &i)) {
			player.RemoveInvItem(i);
			quests[Q_BETRAYER]._qvar1 = 2;
			storyteller._tbtcnt = true;
			storyteller._tTalkingToPlayer = &player;
			InitQTextMsg(TEXT_VILE1);
			storyteller._tMsgSaid = true;
			quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
			quests[Q_BETRAYER]._qlog = true;
		} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE && quests[Q_BETRAYER]._qvar1 == 7) {
			quests[Q_BETRAYER]._qvar1 = 8;
			storyteller._tbtcnt = true;
			storyteller._tTalkingToPlayer = &player;
			InitQTextMsg(TEXT_VILE3);
			storyteller._tMsgSaid = true;
			quests[Q_DIABLO]._qlog = true;
		}
	} else {
		if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE && !quests[Q_BETRAYER]._qlog) {
			storyteller._tbtcnt = true;
			storyteller._tTalkingToPlayer = &player;
			InitQTextMsg(TEXT_VILE1);
			storyteller._tMsgSaid = true;
			quests[Q_BETRAYER]._qlog = true;
			NetSendCmdQuest(true, Q_BETRAYER);
		} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE && quests[Q_BETRAYER]._qvar1 == 7) {
			quests[Q_BETRAYER]._qvar1 = 8;
			storyteller._tbtcnt = true;
			storyteller._tTalkingToPlayer = &player;
			InitQTextMsg(TEXT_VILE3);
			storyteller._tMsgSaid = true;
			NetSendCmdQuest(true, Q_BETRAYER);
			quests[Q_DIABLO]._qlog = true;
			NetSendCmdQuest(true, Q_DIABLO);
		}
	}
	if (!qtextflag) {
		TownerTalk(TEXT_STORY1);
		if (storeflag) {
			StartStore(STORE_STORY);
		}
	}
}

void TalkToFarmer(PlayerStruct &player, TownerStruct &farmer)
{
	_speech_id qt = TEXT_FARMER1;
	bool t2 = true;
	switch (quests[Q_FARMER]._qactive) {
	case QUEST_NOTAVAIL:
		if (player.HasItem(IDI_RUNEBOMB)) {
			qt = TEXT_FARMER2;
			quests[Q_FARMER]._qactive = QUEST_ACTIVE;
			quests[Q_FARMER]._qvar1 = 1;
			quests[Q_FARMER]._qlog = true;
			quests[Q_FARMER]._qmsg = TEXT_FARMER1;
			break;
		} else if (!player._pLvlVisited[9] && player._pLevel < 15) {
			qt = TEXT_FARMER8;
			if (player._pLvlVisited[2])
				qt = TEXT_FARMER5;
			if (player._pLvlVisited[5])
				qt = TEXT_FARMER7;
			if (player._pLvlVisited[7])
				qt = TEXT_FARMER9;
		} else {
			qt = TEXT_FARMER1;
			quests[Q_FARMER]._qactive = QUEST_ACTIVE;
			quests[Q_FARMER]._qvar1 = 1;
			quests[Q_FARMER]._qlog = true;
			quests[Q_FARMER]._qmsg = TEXT_FARMER1;
			SpawnRuneBomb(farmer.position.x + 1, farmer.position.y);
			t2 = true;
			break;
		}
	case QUEST_ACTIVE:
		if (player.HasItem(IDI_RUNEBOMB))
			qt = TEXT_FARMER2;
		else
			qt = TEXT_FARMER3;
		break;
	case QUEST_INIT:
		if (player.HasItem(IDI_RUNEBOMB)) {
			qt = TEXT_FARMER2;
			quests[Q_FARMER]._qactive = QUEST_ACTIVE;
			quests[Q_FARMER]._qvar1 = 1;
			quests[Q_FARMER]._qmsg = TEXT_FARMER1;
			quests[Q_FARMER]._qlog = true;
		} else if (!player._pLvlVisited[9] && player._pLevel < 15) {
			qt = TEXT_FARMER8;
			if (player._pLvlVisited[2]) {
				qt = TEXT_FARMER5;
			}
			if (player._pLvlVisited[5]) {
				qt = TEXT_FARMER7;
			}
			if (player._pLvlVisited[7]) {
				qt = TEXT_FARMER9;
			}
		} else {
			qt = TEXT_FARMER1;
			quests[Q_FARMER]._qactive = QUEST_ACTIVE;
			quests[Q_FARMER]._qvar1 = 1;
			quests[Q_FARMER]._qlog = true;
			quests[Q_FARMER]._qmsg = TEXT_FARMER1;
			SpawnRuneBomb(farmer.position.x + 1, farmer.position.y);
			t2 = true;
		}
		break;
	case QUEST_DONE:
		qt = TEXT_FARMER4;
		SpawnRewardItem(IDI_AURIC, farmer.position.x + 1, farmer.position.y);
		quests[Q_FARMER]._qactive = QUEST_HIVE_DONE;
		quests[Q_FARMER]._qlog = false;
		t2 = true;
		break;
	case QUEST_HIVE_DONE:
		qt = TEXT_NONE;
		break;
	default:
		quests[Q_FARMER]._qactive = QUEST_NOTAVAIL;
		qt = TEXT_FARMER4;
		break;
	}
	if (qt != TEXT_NONE) {
		if (t2) {
			InitQTextMsg(qt);
		} else {
			PlaySFX(alltext[qt].sfxnr);
		}
	}
	if (gbIsMultiplayer) {
		NetSendCmdQuest(true, Q_FARMER);
	}
}

void TalkToCowFarmer(PlayerStruct &player, TownerStruct &cowFarmer)
{
	_speech_id qt = TEXT_JERSEY1;
	bool t2 = true;
	int i;
	if (player.HasItem(IDI_GREYSUIT, &i)) {
		qt = TEXT_JERSEY7;
		player.RemoveInvItem(i);
	} else if (player.HasItem(IDI_BROWNSUIT, &i)) {
		SpawnUnique(UITEM_BOVINE, cowFarmer.position.x + 1, cowFarmer.position.y);
		player.RemoveInvItem(i);
		qt = TEXT_JERSEY8;
		quests[Q_JERSEY]._qactive = QUEST_DONE;
	} else if (player.HasItem(IDI_RUNEBOMB)) {
		qt = TEXT_JERSEY5;
		quests[Q_JERSEY]._qactive = QUEST_ACTIVE;
		quests[Q_JERSEY]._qvar1 = 1;
		quests[Q_JERSEY]._qmsg = TEXT_JERSEY4;
		quests[Q_JERSEY]._qlog = true;
	} else {
		switch (quests[Q_JERSEY]._qactive) {
		case QUEST_NOTAVAIL:
		case QUEST_INIT:
			qt = TEXT_JERSEY1;
			quests[Q_JERSEY]._qactive = QUEST_HIVE_TEASE1;
			break;
		case QUEST_ACTIVE:
			qt = TEXT_JERSEY5;
			break;
		case QUEST_DONE:
			qt = TEXT_JERSEY1;
			break;
		case QUEST_HIVE_TEASE1:
			qt = TEXT_JERSEY2;
			quests[Q_JERSEY]._qactive = QUEST_HIVE_TEASE2;
			break;
		case QUEST_HIVE_TEASE2:
			qt = TEXT_JERSEY3;
			quests[Q_JERSEY]._qactive = QUEST_HIVE_ACTIVE;
			break;
		case QUEST_HIVE_ACTIVE:
			if (!player._pLvlVisited[9] && player._pLevel < 15) {
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
				default:
					qt = TEXT_JERSEY12;
				}
				break;
			} else {
				qt = TEXT_JERSEY4;
				quests[Q_JERSEY]._qactive = QUEST_ACTIVE;
				quests[Q_JERSEY]._qvar1 = 1;
				quests[Q_JERSEY]._qmsg = TEXT_JERSEY4;
				quests[Q_JERSEY]._qlog = true;
				SpawnRuneBomb(cowFarmer.position.x + 1, cowFarmer.position.y);
				t2 = true;
			}
			break;
		default:
			qt = TEXT_JERSEY5;
			quests[Q_JERSEY]._qactive = QUEST_NOTAVAIL;
			break;
		}
	}
	if (qt != TEXT_NONE) {
		if (t2) {
			InitQTextMsg(qt);
		} else {
			PlaySFX(alltext[qt].sfxnr);
		}
	}
	if (gbIsMultiplayer) {
		NetSendCmdQuest(true, Q_JERSEY);
	}
}

void TalkToGirl(PlayerStruct &player, TownerStruct &girl)
{
	_speech_id qt = TEXT_GIRL1;
	bool t2 = false;
	int i;
	if (!player.HasItem(IDI_THEODORE, &i) || quests[Q_GIRL]._qactive == QUEST_DONE) {
		switch (quests[Q_GIRL]._qactive) {
		case 0:
			qt = TEXT_GIRL2;
			quests[Q_GIRL]._qactive = QUEST_ACTIVE;
			quests[Q_GIRL]._qvar1 = 1;
			quests[Q_GIRL]._qlog = true;
			quests[Q_GIRL]._qmsg = TEXT_GIRL2;
			t2 = true;
			break;
		case 1:
			qt = TEXT_GIRL2;
			quests[Q_GIRL]._qvar1 = 1;
			quests[Q_GIRL]._qlog = true;
			quests[Q_GIRL]._qmsg = TEXT_GIRL2;
			quests[Q_GIRL]._qactive = QUEST_ACTIVE;
			t2 = true;
			break;
		case 2:
			qt = TEXT_GIRL3;
			t2 = true;
			break;
		case 3:
			qt = TEXT_NONE;
			break;
		default:
			quests[Q_GIRL]._qactive = QUEST_NOTAVAIL;
			qt = TEXT_GIRL1;
			break;
		}
	} else {
		qt = TEXT_GIRL4;
		player.RemoveInvItem(i);
		CreateAmulet(girl.position, 13, false, true);
		quests[Q_GIRL]._qlog = false;
		quests[Q_GIRL]._qactive = QUEST_DONE;
		t2 = true;
	}
	if (qt != TEXT_NONE) {
		if (t2) {
			InitQTextMsg(qt);
		} else {
			PlaySFX(alltext[qt].sfxnr);
		}
	}
	if (gbIsMultiplayer) {
		NetSendCmdQuest(true, Q_GIRL);
	}
}

} // namespace

TownerStruct towners[NUM_TOWNERS];

/** Contains the data related to quest gossip for each towner ID. */
_speech_id Qtalklist[NUM_TOWNER_TYPES][MAXQUESTS] = {
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
	NumberOfTowners = 0;
	InitSmith();
	InitHealer();
	if (quests[Q_BUTCHER]._qactive != QUEST_NOTAVAIL && quests[Q_BUTCHER]._qactive != QUEST_DONE)
		InitTownDead();
	InitBarOwner();
	InitTeller();
	InitDrunk();
	InitWitch();
	InitBarmaid();
	InitBoy();
	InitCows();
	if (!gbIsHellfire) {
		return;
	}

	if (sgGameInitInfo.bCowQuest != 0) {
		InitCowFarmer();
	} else if (quests[Q_FARMER]._qactive != 10) {
		InitFarmer();
	}

	if (sgGameInitInfo.bTheoQuest != 0 && plr->_pLvlVisited[17]) {
		InitGirl();
	}
}

void FreeTownerGFX()
{
	for (auto &animation : towners) {
		animation._tNData = nullptr;
	}

	CowCels = nullptr;
}

void ProcessTowners()
{
	for (auto &towner : towners) {
		switch (towner._ttype) {
		case TOWN_SMITH:
			TownBlackSmith();
			break;
		case TOWN_HEALER:
			TownHealer();
			break;
		case TOWN_DEADGUY:
			TownDead();
			break;
		case TOWN_TAVERN:
			TownBarOwner();
			break;
		case TOWN_STORY:
			TownStory();
			break;
		case TOWN_DRUNK:
			TownDrunk();
			break;
		case TOWN_WITCH:
			TownWitch();
			break;
		case TOWN_BMAID:
			TownBarMaid();
			break;
		case TOWN_PEGBOY:
			TownBoy();
			break;
		case TOWN_COW:
			TownCow();
			break;
		case TOWN_FARMER:
			TownFarmer();
			break;
		case TOWN_GIRL:
			TownGirl();
			break;
		case TOWN_COWFARM:
			TownCowFarmer();
			break;
		case NUM_TOWNER_TYPES:
			app_fatal("Unkown towner: %d", towner._ttype);
		}

		towner._tAnimCnt++;
		if (towner._tAnimCnt >= towner._tAnimDelay) {
			towner._tAnimCnt = 0;

			if (towner._tAnimOrder >= 0) {
				int ao = towner._tAnimOrder;
				towner._tAnimFrameCnt++;
				if (AnimOrder[ao][towner._tAnimFrameCnt] == -1)
					towner._tAnimFrameCnt = 0;

				towner._tAnimFrame = AnimOrder[ao][towner._tAnimFrameCnt];
			} else {
				towner._tAnimFrame++;
				if (towner._tAnimFrame > towner._tAnimLen)
					towner._tAnimFrame = 1;
			}
		}
	}
}

void TalkToTowner(PlayerStruct &player, int t)
{
	auto &towner = towners[t];

	int dx = abs(player.position.tile.x - towner.position.x);
	int dy = abs(player.position.tile.y - towner.position.y);
#ifdef _DEBUG
	if (!debug_mode_key_d) {
#endif
		if (dx >= 2 || dy >= 2)
			return;
#ifdef _DEBUG
	}
#endif

	if (qtextflag) {
		return;
	}

	towner._tMsgSaid = false;

	if (pcurs >= CURSOR_FIRSTITEM) {
		return;
	}

	switch (towner._ttype) {
	case TOWN_TAVERN:
		TalkToBarOwner(player, towner);
		break;
	case TOWN_DEADGUY:
		TalkToDeadguy(player, towner);
		break;
	case TOWN_SMITH:
		TalkToBlackSmith(player, towner);
		break;
	case TOWN_WITCH:
		TalkToWitch(player, towner);
		break;
	case TOWN_BMAID:
		TalkToBarmaid(player, towner);
		break;
	case TOWN_DRUNK:
		TalkToDrunk();
		break;
	case TOWN_HEALER:
		TalkToHealer(player, towner);
		break;
	case TOWN_PEGBOY:
		TalkToBoy();
		break;
	case TOWN_STORY:
		TalkToStoryteller(player, towner);
		break;
	case TOWN_COW:
		CowSFX(player);
		break;
	case TOWN_FARMER:
		TalkToFarmer(player, towner);
		break;
	case TOWN_COWFARM:
		TalkToCowFarmer(player, towner);
		break;
	case TOWN_GIRL:
		TalkToGirl(player, towner);
		break;
	}
}

} // namespace devilution
