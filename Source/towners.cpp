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
std::unique_ptr<byte[]> pCowCels;
} // namespace

bool storeflag;
int sgnCowMsg;
int numtowners;
DWORD sgdwCowClicks;
TownerStruct towners[NUM_TOWNERS];

/**
 * Maps from active cow sound effect index and player class to sound
 * effect ID for interacting with cows in Tristram.
 *
 * ref: enum _sfx_id
 * ref: enum HeroClass
 */
const _sfx_id snSFX[3][enum_size<HeroClass>::value] = {
	{ PS_WARR52, PS_ROGUE52, PS_MAGE52, PS_MONK52, PS_ROGUE52, PS_WARR52 }, // BUGFIX: add warrior sounds for barbarian instead of 0 - walk sound (fixed)
	{ PS_WARR49, PS_ROGUE49, PS_MAGE49, PS_MONK49, PS_ROGUE49, PS_WARR49 },
	{ PS_WARR50, PS_ROGUE50, PS_MAGE50, PS_MONK50, PS_ROGUE50, PS_WARR50 },
};

/* data */
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
/** Specifies the start X-coordinates of the cows in Tristram. */
int TownCowX[] = { 58, 56, 59 };
/** Specifies the start Y-coordinates of the cows in Tristram. */
int TownCowY[] = { 16, 14, 20 };
/** Specifies the start directions of the cows in Tristram. */
int TownCowDir[] = { DIR_SW, DIR_NW, DIR_N };
/** Maps from direction to X-coordinate delta, which is used when
 * placing cows in Tristram. A single cow may require space of up
 * to three tiles when being placed on the map.
 */
int cowoffx[8] = { -1, 0, -1, -1, -1, 0, -1, -1 };
/** Maps from direction to Y-coordinate delta, which is used when
 * placing cows in Tristram. A single cow may require space of up
 * to three tiles when being placed on the map.
 */
int cowoffy[8] = { -1, -1, -1, 0, -1, -1, -1, 0 };
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

namespace {

/** Specifies the active sound effect ID for interacting with cows. */
_sfx_id CowPlaying = SFX_NONE;

void CowSFX(int pnum)
{
	if (CowPlaying != -1 && effect_is_playing(CowPlaying))
		return;

	sgdwCowClicks++;

	if (gbIsSpawn) {
		if (sgdwCowClicks == 4) {
			sgdwCowClicks = 0;
			CowPlaying = TSFX_COW2;
		} else {
			CowPlaying = TSFX_COW1;
		}
	} else {
		if (sgdwCowClicks >= 8) {
			PlaySfxLoc(TSFX_COW1, plr[pnum].position.tile.x, plr[pnum].position.tile.y + 5);
			sgdwCowClicks = 4;
			CowPlaying = snSFX[sgnCowMsg][static_cast<std::size_t>(plr[pnum]._pClass)]; /* snSFX is local */
			sgnCowMsg++;
			if (sgnCowMsg >= 3)
				sgnCowMsg = 0;
		} else {
			CowPlaying = sgdwCowClicks == 4 ? TSFX_COW2 : TSFX_COW1;
		}
	}

	PlaySfxLoc(CowPlaying, plr[pnum].position.tile.x, plr[pnum].position.tile.y);
}

int GetActiveTowner(int t)
{
	int i;

	for (i = 0; i < numtowners; i++) {
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

void NewTownerAnim(int tnum, byte *pAnim, uint8_t numFrames, int Delay)
{
	towners[tnum]._tAnimData = pAnim;
	towners[tnum]._tAnimLen = numFrames;
	towners[tnum]._tAnimFrame = 1;
	towners[tnum]._tAnimCnt = 0;
	towners[tnum]._tAnimDelay = Delay;
}

void InitTownerInfo(int i, int w, bool sel, _talker_id t, int x, int y, int ao)
{
	towners[i]._tSelFlag = sel;
	towners[i]._tAnimWidth = w;
	towners[i]._tMsgSaid = false;
	towners[i]._ttype = t;
	towners[i].position = { x, y };
	dMonster[x][y] = i + 1;
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
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_SMITH, 62, 63, 0);
	InitQstSnds(numtowners, TOWN_SMITH);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\Smith\\SmithN.CEL");
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 16;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_SW], towners[numtowners]._tNFrames, 3);
	towners[numtowners]._tName = _("Griswold the Blacksmith");
	numtowners++;
}

void InitBarOwner()
{
	InitTownerInfo(numtowners, 96, true, TOWN_TAVERN, 55, 62, 3);
	InitQstSnds(numtowners, TOWN_TAVERN);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\TwnF\\TwnFN.CEL");
	for (auto &towner : towners[numtowners]._tNAnim) {
		towner = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 16;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_SW], towners[numtowners]._tNFrames, 3);
	towners[numtowners]._tName = _("Ogden the Tavern owner");
	numtowners++;
}

void InitTownDead()
{
	InitTownerInfo(numtowners, 96, true, TOWN_DEADGUY, 24, 32, -1);
	InitQstSnds(numtowners, TOWN_DEADGUY);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\Butch\\Deadguy.CEL");
	for (int i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 8;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_N], towners[numtowners]._tNFrames, 6);
	towners[numtowners]._tName = _("Wounded Townsman");
	numtowners++;
}

void InitWitch()
{
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_WITCH, 80, 20, 5);
	InitQstSnds(numtowners, TOWN_WITCH);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\TownWmn1\\Witch.CEL");
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 19;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_S], towners[numtowners]._tNFrames, 6);
	towners[numtowners]._tName = _("Adria the Witch");
	numtowners++;
}

void InitBarmaid()
{
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_BMAID, 43, 66, -1);
	InitQstSnds(numtowners, TOWN_BMAID);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\TownWmn1\\WmnN.CEL");
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 18;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_S], towners[numtowners]._tNFrames, 6);
	towners[numtowners]._tName = _("Gillian the Barmaid");
	numtowners++;
}

void InitBoy()
{
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_PEGBOY, 11, 53, -1);
	InitQstSnds(numtowners, TOWN_PEGBOY);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\TownBoy\\PegKid1.CEL");
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_S], towners[numtowners]._tNFrames, 6);
	towners[numtowners]._tName = _("Wirt the Peg-legged boy");
	numtowners++;
}

void InitHealer()
{
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_HEALER, 55, 79, 1);
	InitQstSnds(numtowners, TOWN_HEALER);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\Healer\\Healer.CEL");
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_SE], towners[numtowners]._tNFrames, 6);
	towners[numtowners]._tName = _("Pepin the Healer");
	numtowners++;
}

void InitTeller()
{
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_STORY, 62, 71, 2);
	InitQstSnds(numtowners, TOWN_STORY);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\Strytell\\Strytell.CEL");
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 25;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_S], towners[numtowners]._tNFrames, 3);
	towners[numtowners]._tName = _("Cain the Elder");
	numtowners++;
}

void InitDrunk()
{
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_DRUNK, 71, 84, 4);
	InitQstSnds(numtowners, TOWN_DRUNK);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\Drunk\\TwnDrunk.CEL");
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 18;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_S], towners[numtowners]._tNFrames, 3);
	towners[numtowners]._tName = _("Farnham the Drunk");
	numtowners++;
}

void InitCows()
{
	int i, dir;
	int x, y, xo, yo;

	//if ( pCowCels )
	// assertion_failed(__LINE__, __FILE__, "! pCowCels");
	pCowCels = LoadFileInMem("Towners\\Animals\\Cow.CEL");
	for (i = 0; i < 3; i++) {
		x = TownCowX[i];
		y = TownCowY[i];
		dir = TownCowDir[i];
		InitTownerInfo(numtowners, 128, false, TOWN_COW, x, y, -1);
		towners[numtowners]._tNData = nullptr;
		SetTownerGPtrs(pCowCels.get(), towners[numtowners]._tNAnim);
		towners[numtowners]._tNFrames = 12;
		NewTownerAnim(numtowners, towners[numtowners]._tNAnim[dir], towners[numtowners]._tNFrames, 3);
		towners[numtowners]._tAnimFrame = GenerateRnd(11) + 1;
		towners[numtowners]._tSelFlag = true;
		towners[numtowners]._tName = _("Cow");

		xo = x + cowoffx[dir];
		yo = y + cowoffy[dir];
		if (dMonster[x][yo] == 0)
			dMonster[x][yo] = -(numtowners + 1);
		if (dMonster[xo][y] == 0)
			dMonster[xo][y] = -(numtowners + 1);
		if (dMonster[xo][yo] == 0)
			dMonster[xo][yo] = -(numtowners + 1);

		numtowners++;
	}
}

void InitFarmer()
{
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_FARMER, 62, 16, -1);
	InitQstSnds(numtowners, TOWN_FARMER);
	towners[numtowners]._tNData = LoadFileInMem("Towners\\Farmer\\Farmrn2.CEL");
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 15;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_S], towners[numtowners]._tNFrames, 3);
	towners[numtowners]._tName = _("Lester the farmer");
	numtowners++;
}

void InitCowFarmer()
{
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_COWFARM, 61, 22, -1);
	InitQstSnds(numtowners, TOWN_COWFARM);
	if (quests[Q_JERSEY]._qactive != QUEST_DONE) {
		towners[numtowners]._tNData = LoadFileInMem("Towners\\Farmer\\cfrmrn2.CEL");
	} else {
		towners[numtowners]._tNData = LoadFileInMem("Towners\\Farmer\\mfrmrn2.CEL");
	}
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 15;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_SW], towners[numtowners]._tNFrames, 3);
	towners[numtowners]._tName = _("Complete Nut");
	numtowners++;
}

void InitGirl()
{
	int i;

	InitTownerInfo(numtowners, 96, true, TOWN_GIRL, 77, 43, -1);
	InitQstSnds(numtowners, TOWN_GIRL);
	if (quests[Q_GIRL]._qactive != QUEST_DONE) {
		towners[numtowners]._tNData = LoadFileInMem("Towners\\Girl\\Girlw1.CEL");
	} else {
		towners[numtowners]._tNData = LoadFileInMem("Towners\\Girl\\Girls1.CEL");
	}
	for (i = 0; i < 8; i++) {
		towners[numtowners]._tNAnim[i] = towners[numtowners]._tNData.get();
	}
	towners[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towners[numtowners]._tNAnim[DIR_S], towners[numtowners]._tNFrames, 6);
	towners[numtowners]._tName = "Celia";
	numtowners++;
}

} // namespace

void InitTowners()
{
	numtowners = 0;
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
	if (gbIsHellfire) {
		if (sgGameInitInfo.bCowQuest) {
			InitCowFarmer();
		} else if (quests[Q_FARMER]._qactive != 10) {
			InitFarmer();
		}
		if (sgGameInitInfo.bTheoQuest && plr->_pLvlVisited[17]) {
			InitGirl();
		}
	}
}

void FreeTownerGFX()
{
	int i;

	for (i = 0; i < NUM_TOWNERS; i++) {
		towners[i]._tNData = nullptr;
	}

	pCowCels = nullptr;
}

void TownCtrlMsg(int i)
{
	int p;
	int dx, dy;

	if (i == -1)
		return;

	if (towners[i]._tbtcnt) {
		p = towners[i]._tTalkingToPlayer;
		dx = abs(towners[i].position.x - plr[p].position.tile.x);
		dy = abs(towners[i].position.y - plr[p].position.tile.y);
		if (dx >= 2 || dy >= 2) {
			towners[i]._tbtcnt = false;
			qtextflag = false;
			stream_stop();
		}
	}
}

void TownBlackSmith()
{
	int i;

	i = GetActiveTowner(TOWN_SMITH);
	TownCtrlMsg(i);
}

void TownBarOwner()
{
	int i;

	i = GetActiveTowner(TOWN_TAVERN);
	TownCtrlMsg(i);
}

void TownDead()
{
	int tidx;

	tidx = GetActiveTowner(TOWN_DEADGUY);
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
	int i;

	i = GetActiveTowner(TOWN_HEALER);
	TownCtrlMsg(i);
}

void TownStory()
{
	int i;

	i = GetActiveTowner(TOWN_STORY);
	TownCtrlMsg(i);
}

void TownDrunk()
{
	int i;

	i = GetActiveTowner(TOWN_DRUNK);
	TownCtrlMsg(i);
}

void TownBoy()
{
	int i;

	i = GetActiveTowner(TOWN_PEGBOY);
	TownCtrlMsg(i);
}

void TownWitch()
{
	int i;

	i = GetActiveTowner(TOWN_WITCH);
	TownCtrlMsg(i);
}

void TownBarMaid()
{
	int i;

	i = GetActiveTowner(TOWN_BMAID);
	TownCtrlMsg(i);
}

void TownCow()
{
	int i;

	i = GetActiveTowner(TOWN_COW);
	TownCtrlMsg(i);
}

void TownFarmer()
{
	int i;

	i = GetActiveTowner(TOWN_FARMER);
	TownCtrlMsg(i);
}

void TownCowFarmer()
{
	int i;

	i = GetActiveTowner(TOWN_COWFARM);
	TownCtrlMsg(i);
}

void TownGirl()
{
	int i;

	i = GetActiveTowner(TOWN_GIRL);
	TownCtrlMsg(i);
}

void ProcessTowners()
{
	int i, ao;

	for (i = 0; i < NUM_TOWNERS; i++) {
		switch (towners[i]._ttype) {
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
			app_fatal("Unkown towner: %d", towners[i]._ttype);
		}

		towners[i]._tAnimCnt++;
		if (towners[i]._tAnimCnt >= towners[i]._tAnimDelay) {
			towners[i]._tAnimCnt = 0;

			if (towners[i]._tAnimOrder >= 0) {
				ao = towners[i]._tAnimOrder;
				towners[i]._tAnimFrameCnt++;
				if (AnimOrder[ao][towners[i]._tAnimFrameCnt] == -1)
					towners[i]._tAnimFrameCnt = 0;

				towners[i]._tAnimFrame = AnimOrder[ao][towners[i]._tAnimFrameCnt];
			} else {
				towners[i]._tAnimFrame++;
				if (towners[i]._tAnimFrame > towners[i]._tAnimLen)
					towners[i]._tAnimFrame = 1;
			}
		}
	}
}

ItemStruct *PlrHasItem(int pnum, int item, int *i)
{
	for (*i = 0; *i < plr[pnum]._pNumInv; (*i)++) {
		if (plr[pnum].InvList[*i].IDidx == item)
			return &plr[pnum].InvList[*i];
	}

	return nullptr;
}

void TownerTalk(int first)
{
	sgdwCowClicks = 0;
	sgnCowMsg = 0;
	storeflag = true;
	InitQTextMsg(first);
}

void TalkToTowner(int p, int t)
{
	int i, dx, dy;
	ItemStruct *Item;
	int qt;
	bool t2;

	dx = abs(plr[p].position.tile.x - towners[t].position.x);
	dy = abs(plr[p].position.tile.y - towners[t].position.y);
#ifdef _DEBUG
	if (!debug_mode_key_d)
#endif
		if (dx >= 2 || dy >= 2)
			return;

	if (qtextflag) {
		return;
	}

	towners[t]._tMsgSaid = false;

	if (pcurs >= CURSOR_FIRSTITEM) {
		return;
	}

	if (t == GetActiveTowner(TOWN_TAVERN)) {
		if (!plr[p]._pLvlVisited[0] && !towners[t]._tMsgSaid) {
			towners[t]._tbtcnt = true;
			towners[t]._tTalkingToPlayer = p;
			InitQTextMsg(TEXT_INTRO);
			towners[t]._tMsgSaid = true;
		}
		if ((plr[p]._pLvlVisited[2] || plr[p]._pLvlVisited[4]) && quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) {
			if (quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) {
				if (quests[Q_SKELKING]._qvar2 == 0 && !towners[t]._tMsgSaid) {
					quests[Q_SKELKING]._qvar2 = 1;
					quests[Q_SKELKING]._qlog = true;
					if (quests[Q_SKELKING]._qactive == QUEST_INIT) {
						quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
						quests[Q_SKELKING]._qvar1 = 1;
					}
					towners[t]._tbtcnt = true;
					towners[t]._tTalkingToPlayer = p;
					InitQTextMsg(TEXT_KING2);
					towners[t]._tMsgSaid = true;
					NetSendCmdQuest(true, Q_SKELKING);
				}
			}
			if (quests[Q_SKELKING]._qactive == QUEST_DONE && quests[Q_SKELKING]._qvar2 == 1 && !towners[t]._tMsgSaid) {
				quests[Q_SKELKING]._qvar2 = 2;
				quests[Q_SKELKING]._qvar1 = 2;
				towners[t]._tbtcnt = true;
				towners[t]._tTalkingToPlayer = p;
				InitQTextMsg(TEXT_KING4);
				towners[t]._tMsgSaid = true;
				NetSendCmdQuest(true, Q_SKELKING);
			}
		}
		if (!gbIsMultiplayer) {
			if (plr[p]._pLvlVisited[3] && quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL) {
				if ((quests[Q_LTBANNER]._qactive == QUEST_INIT || quests[Q_LTBANNER]._qactive == QUEST_ACTIVE) && quests[Q_LTBANNER]._qvar2 == 0 && !towners[t]._tMsgSaid) {
					quests[Q_LTBANNER]._qvar2 = 1;
					if (quests[Q_LTBANNER]._qactive == QUEST_INIT) {
						quests[Q_LTBANNER]._qvar1 = 1;
						quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
					}
					quests[Q_LTBANNER]._qlog = true;
					towners[t]._tbtcnt = true;
					towners[t]._tTalkingToPlayer = p;
					InitQTextMsg(TEXT_BANNER2);
					towners[t]._tMsgSaid = true;
				}
				if (quests[Q_LTBANNER]._qvar2 == 1 && PlrHasItem(p, IDI_BANNER, &i) != nullptr && !towners[t]._tMsgSaid) {
					quests[Q_LTBANNER]._qactive = QUEST_DONE;
					quests[Q_LTBANNER]._qvar1 = 3;
					RemoveInvItem(p, i);
					SpawnUnique(UITEM_HARCREST, towners[t].position.x, towners[t].position.y + 1);
					towners[t]._tbtcnt = true;
					towners[t]._tTalkingToPlayer = p;
					InitQTextMsg(TEXT_BANNER3);
					towners[t]._tMsgSaid = true;
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_OGDEN1);
			if (storeflag) {
				StartStore(STORE_TAVERN);
			}
		}
	} else if (t == GetActiveTowner(TOWN_DEADGUY)) {
		if (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qvar1 == 1) {
			towners[t]._tbtcnt = true;
			towners[t]._tTalkingToPlayer = p;
			quests[Q_BUTCHER]._qvar1 = 1;
			plr[p].PlaySpecificSpeach(8);
			towners[t]._tMsgSaid = true;
		} else if (quests[Q_BUTCHER]._qactive == QUEST_DONE && quests[Q_BUTCHER]._qvar1 == 1) {
			quests[Q_BUTCHER]._qvar1 = 1;
			towners[t]._tbtcnt = true;
			towners[t]._tTalkingToPlayer = p;
			towners[t]._tMsgSaid = true;
		} else if (quests[Q_BUTCHER]._qactive == QUEST_INIT || (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qvar1 == 0)) {
			quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
			quests[Q_BUTCHER]._qlog = true;
			quests[Q_BUTCHER]._qmsg = TEXT_BUTCH9;
			quests[Q_BUTCHER]._qvar1 = 1;
			towners[t]._tbtcnt = true;
			towners[t]._tTalkingToPlayer = p;
			InitQTextMsg(TEXT_BUTCH9);
			towners[t]._tMsgSaid = true;
			NetSendCmdQuest(true, Q_BUTCHER);
		}
	} else if (t == GetActiveTowner(TOWN_SMITH)) {
		if (!gbIsMultiplayer) {
			if (plr[p]._pLvlVisited[4] && quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
				if (quests[Q_ROCK]._qvar2 == 0) {
					quests[Q_ROCK]._qvar2 = 1;
					quests[Q_ROCK]._qlog = true;
					if (quests[Q_ROCK]._qactive == QUEST_INIT) {
						quests[Q_ROCK]._qactive = QUEST_ACTIVE;
						quests[Q_ROCK]._qvar1 = 1;
					}
					towners[t]._tbtcnt = true;
					towners[t]._tTalkingToPlayer = p;
					InitQTextMsg(TEXT_INFRA5);
					towners[t]._tMsgSaid = true;
				}
				if (quests[Q_ROCK]._qvar2 == 1 && PlrHasItem(p, IDI_ROCK, &i) != nullptr && !towners[t]._tMsgSaid) {
					quests[Q_ROCK]._qactive = QUEST_DONE;
					quests[Q_ROCK]._qvar2 = 2;
					quests[Q_ROCK]._qvar1 = 2;
					RemoveInvItem(p, i);
					SpawnUnique(UITEM_INFRARING, towners[t].position.x, towners[t].position.y + 1);
					towners[t]._tbtcnt = true;
					towners[t]._tTalkingToPlayer = p;
					InitQTextMsg(TEXT_INFRA7);
					towners[t]._tMsgSaid = true;
				}
			}
			if (plr[p]._pLvlVisited[9] && quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
				if ((quests[Q_ANVIL]._qactive == QUEST_INIT || quests[Q_ANVIL]._qactive == QUEST_ACTIVE) && quests[Q_ANVIL]._qvar2 == 0 && !towners[t]._tMsgSaid) {
					if (quests[Q_ROCK]._qvar2 == 2 || (quests[Q_ROCK]._qactive == QUEST_ACTIVE && quests[Q_ROCK]._qvar2 == 1)) {
						quests[Q_ANVIL]._qvar2 = 1;
						quests[Q_ANVIL]._qlog = true;
						if (quests[Q_ANVIL]._qactive == QUEST_INIT) {
							quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
							quests[Q_ANVIL]._qvar1 = 1;
						}
						towners[t]._tbtcnt = true;
						towners[t]._tTalkingToPlayer = p;
						InitQTextMsg(TEXT_ANVIL5);
						towners[t]._tMsgSaid = true;
					}
				}
				if (quests[Q_ANVIL]._qvar2 == 1 && PlrHasItem(p, IDI_ANVIL, &i) != nullptr) {
					if (!towners[t]._tMsgSaid) {
						quests[Q_ANVIL]._qactive = QUEST_DONE;
						quests[Q_ANVIL]._qvar2 = 2;
						quests[Q_ANVIL]._qvar1 = 2;
						RemoveInvItem(p, i);
						SpawnUnique(UITEM_GRISWOLD, towners[t].position.x, towners[t].position.y + 1);
						towners[t]._tbtcnt = true;
						towners[t]._tTalkingToPlayer = p;
						InitQTextMsg(TEXT_ANVIL7);
						towners[t]._tMsgSaid = true;
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
	} else if (t == GetActiveTowner(TOWN_WITCH)) {
		if (quests[Q_MUSHROOM]._qactive == QUEST_INIT && PlrHasItem(p, IDI_FUNGALTM, &i) != nullptr) {
			RemoveInvItem(p, i);
			quests[Q_MUSHROOM]._qactive = QUEST_ACTIVE;
			quests[Q_MUSHROOM]._qlog = true;
			quests[Q_MUSHROOM]._qvar1 = QS_TOMEGIVEN;
			towners[t]._tbtcnt = true;
			towners[t]._tTalkingToPlayer = p;
			InitQTextMsg(TEXT_MUSH8);
			towners[t]._tMsgSaid = true;
		} else if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
			if (quests[Q_MUSHROOM]._qvar1 >= QS_TOMEGIVEN && quests[Q_MUSHROOM]._qvar1 <= QS_MUSHPICKED) {
				if (PlrHasItem(p, IDI_MUSHROOM, &i) != nullptr) {
					RemoveInvItem(p, i);
					quests[Q_MUSHROOM]._qvar1 = QS_MUSHGIVEN;
					Qtalklist[TOWN_HEALER][Q_MUSHROOM] = TEXT_MUSH3;
					Qtalklist[TOWN_WITCH][Q_MUSHROOM] = TEXT_NONE;
					towners[t]._tbtcnt = true;
					towners[t]._tTalkingToPlayer = p;
					quests[Q_MUSHROOM]._qmsg = TEXT_MUSH10;
					InitQTextMsg(TEXT_MUSH10);
					towners[t]._tMsgSaid = true;
				} else if (quests[Q_MUSHROOM]._qmsg != TEXT_MUSH9) {
					towners[t]._tbtcnt = true;
					towners[t]._tTalkingToPlayer = p;
					quests[Q_MUSHROOM]._qmsg = TEXT_MUSH9;
					InitQTextMsg(TEXT_MUSH9);
					towners[t]._tMsgSaid = true;
				}
			} else {
				Item = PlrHasItem(p, IDI_SPECELIX, &i);
				if (Item != nullptr) {
					towners[t]._tbtcnt = true;
					towners[t]._tTalkingToPlayer = p;
					InitQTextMsg(TEXT_MUSH12);
					quests[Q_MUSHROOM]._qactive = QUEST_DONE;
					towners[t]._tMsgSaid = true;
					AllItemsList[Item->IDidx].iUsable = true;
				} else if (PlrHasItem(p, IDI_BRAIN, &i) != nullptr && quests[Q_MUSHROOM]._qvar2 != TEXT_MUSH11) {
					towners[t]._tbtcnt = true;
					towners[t]._tTalkingToPlayer = p;
					quests[Q_MUSHROOM]._qvar2 = TEXT_MUSH11;
					InitQTextMsg(TEXT_MUSH11);
					towners[t]._tMsgSaid = true;
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_ADRIA1);
			if (storeflag) {
				StartStore(STORE_WITCH);
			}
		}
	} else if (t == GetActiveTowner(TOWN_BMAID)) {
		if (!plr[p]._pLvlVisited[21] && PlrHasItem(p, IDI_MAPOFDOOM, &i) != nullptr) {
			quests[Q_GRAVE]._qactive = QUEST_ACTIVE;
			quests[Q_GRAVE]._qlog = true;
			quests[Q_GRAVE]._qmsg = TEXT_GRAVE8;
			InitQTextMsg(TEXT_GRAVE8);
			towners[t]._tMsgSaid = true;
		}
		if (!qtextflag) {
			TownerTalk(TEXT_GILLIAN1);
			if (storeflag) {
				StartStore(STORE_BARMAID);
			}
		}
	} else if (t == GetActiveTowner(TOWN_DRUNK)) {
		if (!qtextflag) {
			TownerTalk(TEXT_FARNHAM1);
			if (storeflag) {
				StartStore(STORE_DRUNK);
			}
		}
	} else if (t == GetActiveTowner(TOWN_HEALER)) {
		if (!gbIsMultiplayer) {
			if (plr[p]._pLvlVisited[1] || (gbIsHellfire && plr[p]._pLvlVisited[5])) {
				if (!towners[t]._tMsgSaid) {
					if (quests[Q_PWATER]._qactive == QUEST_INIT) {
						quests[Q_PWATER]._qactive = QUEST_ACTIVE;
						quests[Q_PWATER]._qlog = true;
						quests[Q_PWATER]._qmsg = TEXT_POISON3;
						quests[Q_PWATER]._qvar1 = 1;
						towners[t]._tbtcnt = true;
						towners[t]._tTalkingToPlayer = p;
						InitQTextMsg(TEXT_POISON3);
						towners[t]._tMsgSaid = true;
					} else if (quests[Q_PWATER]._qactive == QUEST_DONE && quests[Q_PWATER]._qvar1 != 2) {
						quests[Q_PWATER]._qvar1 = 2;
						towners[t]._tbtcnt = true;
						towners[t]._tTalkingToPlayer = p;
						InitQTextMsg(TEXT_POISON5);
						SpawnUnique(UITEM_TRING, towners[t].position.x, towners[t].position.y + 1);
						towners[t]._tMsgSaid = true;
					}
				}
			}
			if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qmsg == TEXT_MUSH10 && PlrHasItem(p, IDI_BRAIN, &i) != nullptr) {
				RemoveInvItem(p, i);
				SpawnQuestItem(IDI_SPECELIX, towners[t].position.x, towners[t].position.y + 1, 0, false);
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
	} else if (t == GetActiveTowner(TOWN_PEGBOY)) {
		if (!qtextflag) {
			TownerTalk(TEXT_WIRT1);
			if (storeflag) {
				StartStore(STORE_BOY);
			}
		}
	} else if (t == GetActiveTowner(TOWN_STORY)) {
		if (!gbIsMultiplayer) {
			if (quests[Q_BETRAYER]._qactive == QUEST_INIT && PlrHasItem(p, IDI_LAZSTAFF, &i) != nullptr) {
				RemoveInvItem(p, i);
				quests[Q_BETRAYER]._qvar1 = 2;
				towners[t]._tbtcnt = true;
				towners[t]._tTalkingToPlayer = p;
				InitQTextMsg(TEXT_VILE1);
				towners[t]._tMsgSaid = true;
				quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
				quests[Q_BETRAYER]._qlog = true;
			} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE && quests[Q_BETRAYER]._qvar1 == 7) {
				quests[Q_BETRAYER]._qvar1 = 8;
				towners[t]._tbtcnt = true;
				towners[t]._tTalkingToPlayer = p;
				InitQTextMsg(TEXT_VILE3);
				towners[t]._tMsgSaid = true;
				quests[Q_DIABLO]._qlog = true;
			}
		}
		if (gbIsMultiplayer) {
			if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE && !quests[Q_BETRAYER]._qlog) {
				towners[t]._tbtcnt = true;
				towners[t]._tTalkingToPlayer = p;
				InitQTextMsg(TEXT_VILE1);
				towners[t]._tMsgSaid = true;
				quests[Q_BETRAYER]._qlog = true;
				NetSendCmdQuest(true, Q_BETRAYER);
			} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE && quests[Q_BETRAYER]._qvar1 == 7) {
				quests[Q_BETRAYER]._qvar1 = 8;
				towners[t]._tbtcnt = true;
				towners[t]._tTalkingToPlayer = p;
				InitQTextMsg(TEXT_VILE3);
				towners[t]._tMsgSaid = true;
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
	} else if (towners[t]._ttype == TOWN_COW) {
		if (!qtextflag)
			CowSFX(p);
	} else if (towners[t]._ttype == TOWN_FARMER) {
		if (!qtextflag) {
			qt = TEXT_FARMER1;
			t2 = true;
			switch (quests[Q_FARMER]._qactive) {
			case QUEST_NOTAVAIL:
				if (PlrHasItem(p, IDI_RUNEBOMB, &i) != nullptr) {
					qt = TEXT_FARMER2;
					quests[Q_FARMER]._qactive = QUEST_ACTIVE;
					quests[Q_FARMER]._qvar1 = 1;
					quests[Q_FARMER]._qlog = true;
					quests[Q_FARMER]._qmsg = TEXT_FARMER1;
					break;
				} else if (!plr[p]._pLvlVisited[9] && plr[p]._pLevel < 15) {
					qt = TEXT_FARMER8;
					if (plr[p]._pLvlVisited[2])
						qt = TEXT_FARMER5;
					if (plr[p]._pLvlVisited[5])
						qt = TEXT_FARMER7;
					if (plr[p]._pLvlVisited[7])
						qt = TEXT_FARMER9;
				} else {
					qt = TEXT_FARMER1;
					quests[Q_FARMER]._qactive = QUEST_ACTIVE;
					quests[Q_FARMER]._qvar1 = 1;
					quests[Q_FARMER]._qlog = true;
					quests[Q_FARMER]._qmsg = TEXT_FARMER1;
					SpawnRuneBomb(towners[t].position.x + 1, towners[t].position.y);
					t2 = true;
					break;
				}
			case QUEST_ACTIVE:
				if (PlrHasItem(p, IDI_RUNEBOMB, &i) != nullptr)
					qt = TEXT_FARMER2;
				else
					qt = TEXT_FARMER3;
				break;
			case QUEST_INIT:
				if (PlrHasItem(p, IDI_RUNEBOMB, &i) != nullptr) {
					qt = TEXT_FARMER2;
					quests[Q_FARMER]._qactive = QUEST_ACTIVE;
					quests[Q_FARMER]._qvar1 = 1;
					quests[Q_FARMER]._qmsg = TEXT_FARMER1;
					quests[Q_FARMER]._qlog = true;
				} else if (!plr[p]._pLvlVisited[9] && plr[p]._pLevel < 15) {
					qt = TEXT_FARMER8;
					if (plr[p]._pLvlVisited[2]) {
						qt = TEXT_FARMER5;
					}
					if (plr[p]._pLvlVisited[5]) {
						qt = TEXT_FARMER7;
					}
					if (plr[p]._pLvlVisited[7]) {
						qt = TEXT_FARMER9;
					}
				} else {
					qt = TEXT_FARMER1;
					quests[Q_FARMER]._qactive = QUEST_ACTIVE;
					quests[Q_FARMER]._qvar1 = 1;
					quests[Q_FARMER]._qlog = true;
					quests[Q_FARMER]._qmsg = TEXT_FARMER1;
					SpawnRuneBomb(towners[t].position.x + 1, towners[t].position.y);
					t2 = true;
				}
				break;
			case QUEST_DONE:
				qt = TEXT_FARMER4;
				SpawnRewardItem(IDI_AURIC, towners[t].position.x + 1, towners[t].position.y);
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
				if (t2)
					InitQTextMsg(qt);
				else
					PlaySFX(alltext[qt].sfxnr);
			}
			if (gbIsMultiplayer) {
				NetSendCmdQuest(true, Q_FARMER);
			}
		}
	} else if (towners[t]._ttype == TOWN_COWFARM) {
		if (!qtextflag) {
			qt = TEXT_JERSEY1;
			t2 = true;
			if (PlrHasItem(p, IDI_GREYSUIT, &i) != nullptr) {
				qt = TEXT_JERSEY7;
				RemoveInvItem(p, i);
			} else if (PlrHasItem(p, IDI_BROWNSUIT, &i) != nullptr) {
				SpawnUnique(UITEM_BOVINE, towners[t].position.x + 1, towners[t].position.y);
				RemoveInvItem(p, i);
				qt = TEXT_JERSEY8;
				quests[Q_JERSEY]._qactive = QUEST_DONE;
			} else if (PlrHasItem(p, IDI_RUNEBOMB, &i) != nullptr) {
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
					if (!plr[p]._pLvlVisited[9] && plr[p]._pLevel < 15) {
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
						SpawnRuneBomb(towners[t].position.x + 1, towners[t].position.y);
						t2 = true;
					}
					break;
				default:
					qt = TEXT_JERSEY5;
					quests[Q_JERSEY]._qactive = QUEST_NOTAVAIL;
					break;
				}
			}
			if (qt != -1) {
				if (t2)
					InitQTextMsg(qt);
				else
					PlaySFX(alltext[qt].sfxnr);
			}
			if (gbIsMultiplayer) {
				NetSendCmdQuest(true, Q_JERSEY);
			}
		}
	} else if (towners[t]._ttype == TOWN_GIRL) {
		if (!qtextflag) {
			qt = TEXT_GIRL1;
			t2 = false;
			if (PlrHasItem(p, IDI_THEODORE, &i) == nullptr || quests[Q_GIRL]._qactive == QUEST_DONE) {
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
				RemoveInvItem(p, i);
				CreateAmulet(towners[t].position.x, towners[t].position.y, 13, false, true);
				quests[Q_GIRL]._qlog = false;
				quests[Q_GIRL]._qactive = QUEST_DONE;
				t2 = true;
			}
			if (qt != -1) {
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
	}
}

} // namespace devilution
