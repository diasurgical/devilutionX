/**
 * @file towners.cpp
 *
 * Implementation of functionality for loading and spawning towners.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL storeflag;
int sgnCowMsg;
int numtowners;
DWORD sgdwCowClicks;
BOOL boyloadflag;
BYTE *pCowCels;
TownerStruct towner[NUM_TOWNERS];

/**
 * Maps from active cow sound effect index and player class to sound
 * effect ID for interacting with cows in Tristram.
 *
 * ref: enum _sfx_id
 * ref: enum plr_class
 */
const int snSFX[3][NUM_CLASSES] = {
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
QuestTalkData Qtalklist[] = {
	// clang-format off
	// _qinfra,      _qblkm,       _qgarb,      _qzhar,      _qveil,      _qmod,       _qbutch,      _qbol,         _qblind,      _qblood,      _qanvil,      _qwarlrd,      _qking,       _qpw,           _qbone,      _qvb,         _qgrv,        _qfarm, _qgirl, _qtrade, _qdefiler, _qnakrul, _qjersy, _qhf8
	{ TEXT_INFRA6,  TEXT_MUSH6,  -1,          -1,          TEXT_VEIL5, -1,          TEXT_BUTCH5, TEXT_BANNER6, TEXT_BLIND5, TEXT_BLOOD5, TEXT_ANVIL6, TEXT_WARLRD5, TEXT_KING7,  TEXT_POISON7,  TEXT_BONE5, TEXT_VILE9,  TEXT_GRAVE2, -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ TEXT_INFRA3,  -1,           -1,          -1,          TEXT_VEIL3, -1,          TEXT_BUTCH3, TEXT_BANNER4, TEXT_BLIND3, TEXT_BLOOD3, TEXT_ANVIL3, TEXT_WARLRD3, TEXT_KING5,  TEXT_POISON4,  TEXT_BONE3, TEXT_VILE7,  TEXT_GRAVE3, -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1,           -1,           -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ TEXT_INFRA2,  TEXT_MUSH2,  -1,          -1,          TEXT_VEIL2, -1,          TEXT_BUTCH2, -1,            TEXT_BLIND2, TEXT_BLOOD2, TEXT_ANVIL2, TEXT_WARLRD2, TEXT_KING3,  TEXT_POISON2,  TEXT_BONE2, TEXT_VILE4,  TEXT_GRAVE5, -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ TEXT_INFRA1,  TEXT_MUSH1,  -1,          -1,          TEXT_VEIL1, TEXT_VILE3, TEXT_BUTCH1, TEXT_BANNER1, TEXT_BLIND1, TEXT_BLOOD1, TEXT_ANVIL1, TEXT_WARLRD1, TEXT_KING1,  TEXT_POISON1,  TEXT_BONE1, TEXT_VILE2,  TEXT_GRAVE6, -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ TEXT_INFRA8,  TEXT_MUSH7,  -1,          -1,          TEXT_VEIL6, -1,          TEXT_BUTCH6, TEXT_BANNER7, TEXT_BLIND6, TEXT_BLOOD6, TEXT_ANVIL8, TEXT_WARLRD6, TEXT_KING8,  TEXT_POISON8,  TEXT_BONE6, TEXT_VILE10, TEXT_GRAVE7, -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ TEXT_INFRA9,  TEXT_MUSH9,  -1,          -1,          TEXT_VEIL7, -1,          TEXT_BUTCH7, TEXT_BANNER8, TEXT_BLIND7, TEXT_BLOOD7, TEXT_ANVIL9, TEXT_WARLRD7, TEXT_KING9,  TEXT_POISON9,  TEXT_BONE7, TEXT_VILE11, TEXT_GRAVE1, -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ TEXT_INFRA4,  TEXT_MUSH5,  -1,          -1,          TEXT_VEIL4, -1,          TEXT_BUTCH4, TEXT_BANNER5, TEXT_BLIND4, TEXT_BLOOD4, TEXT_ANVIL4, TEXT_WARLRD4, TEXT_KING6,  TEXT_POISON6,  TEXT_BONE4, TEXT_VILE8,  TEXT_GRAVE8, -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ TEXT_INFRA10, TEXT_MUSH13, -1,          -1,          TEXT_VEIL8, -1,          TEXT_BUTCH8, TEXT_BANNER9, TEXT_BLIND8, TEXT_BLOOD8, TEXT_ANVIL10,TEXT_WARLRD8, TEXT_KING10, TEXT_POISON10, TEXT_BONE8, TEXT_VILE12, TEXT_GRAVE9, -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1,           -1,           -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1,           -1,           -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1,           -1,           -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1,           -1,           -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1,           -1,           -1,     -1,     -1,      -1,        -1,       -1,      -1 },
	// clang-format on
};
/** Specifies the active sound effect ID for interacting with cows. */
int CowPlaying = -1;

static void CowSFX(int pnum)
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
			PlaySfxLoc(TSFX_COW1, plr[pnum]._px, plr[pnum]._py + 5);
			sgdwCowClicks = 4;
			CowPlaying = snSFX[sgnCowMsg][plr[pnum]._pClass]; /* snSFX is local */
			sgnCowMsg++;
			if (sgnCowMsg >= 3)
				sgnCowMsg = 0;
		} else {
			CowPlaying = sgdwCowClicks == 4 ? TSFX_COW2 : TSFX_COW1;
		}
	}

	PlaySfxLoc(CowPlaying, plr[pnum]._px, plr[pnum]._py);
}

int GetActiveTowner(int t)
{
	int i;

	for (i = 0; i < numtowners; i++) {
		if (towner[i]._ttype == t)
			return i;
	}

	return -1;
}

void SetTownerGPtrs(BYTE *pData, BYTE **pAnim)
{
	int i;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pData;

	for (i = 0; i < 8; i++) {
		pAnim[i] = CelGetFrameStart(pData, i);
	}
}

void NewTownerAnim(int tnum, BYTE *pAnim, int numFrames, int Delay)
{
	towner[tnum]._tAnimData = pAnim;
	towner[tnum]._tAnimLen = numFrames;
	towner[tnum]._tAnimFrame = 1;
	towner[tnum]._tAnimCnt = 0;
	towner[tnum]._tAnimDelay = Delay;
}

void InitTownerInfo(int i, int w, int sel, int t, int x, int y, int ao, int tp)
{
	memset(&towner[i], 0, sizeof(TownerStruct));
	towner[i]._tSelFlag = sel;
	towner[i]._tAnimWidth = w;
	towner[i]._tAnimWidth2 = (w - 64) >> 1;
	towner[i]._tMsgSaid = FALSE;
	towner[i]._ttype = t;
	towner[i]._tx = x;
	towner[i]._ty = y;
	dMonster[x][y] = i + 1;
	towner[i]._tAnimOrder = ao;
	towner[i]._tTenPer = tp;
	towner[i]._tSeed = AdvanceRndSeed();
}

void InitQstSnds(int i)
{
	int j, tl;
	tl = i;
	if (boyloadflag)
		tl++;
	for (j = 0; j < MAXQUESTS; j++) {
		towner[i].qsts[j]._qsttype = quests[j]._qtype;
		towner[i].qsts[j]._qstmsg = ((int *)(Qtalklist + tl))[j];
		if (((int *)(Qtalklist + tl))[j] != -1)
			towner[i].qsts[j]._qstmsgact = TRUE;
		else
			towner[i].qsts[j]._qstmsgact = FALSE;
	}
}

/**
 * @brief Load Griswold into the game

 */
void InitSmith()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_SMITH, 62, 63, 0, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Smith\\SmithN.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 16;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_SW], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Griswold the Blacksmith");
	numtowners++;
}

void InitBarOwner()
{
	InitTownerInfo(numtowners, 96, TRUE, TOWN_TAVERN, 55, 62, 3, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\TwnF\\TwnFN.CEL", NULL);
	for (int i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 16;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_SW], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Ogden the Tavern owner");
	numtowners++;
}

void InitTownDead()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_DEADGUY, 24, 32, -1, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Butch\\Deadguy.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 8;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_N], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Wounded Townsman");
	numtowners++;
}

void InitWitch()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_WITCH, 80, 20, 5, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\TownWmn1\\Witch.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 19;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_S], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Adria the Witch");
	numtowners++;
}

void InitBarmaid()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_BMAID, 43, 66, -1, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\TownWmn1\\WmnN.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 18;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_S], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Gillian the Barmaid");
	numtowners++;
}

void InitBoy()
{
	int i;

	boyloadflag = TRUE;
	InitTownerInfo(numtowners, 96, TRUE, TOWN_PEGBOY, 11, 53, -1, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\TownBoy\\PegKid1.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_S], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Wirt the Peg-legged boy");
	numtowners++;
}

void InitHealer()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_HEALER, 55, 79, 1, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Healer\\Healer.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_SE], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Pepin the Healer");
	numtowners++;
}

void InitTeller()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_STORY, 62, 71, 2, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Strytell\\Strytell.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 25;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_S], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Cain the Elder");
	numtowners++;
}

void InitDrunk()
{
	int i;

	InitTownerInfo(numtowners, 96, TRUE, TOWN_DRUNK, 71, 84, 4, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Drunk\\TwnDrunk.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 18;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_S], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Farnham the Drunk");
	numtowners++;
}

void InitCows()
{
	int i, dir;
	int x, y, xo, yo;

	//if ( pCowCels )
	// assertion_failed(__LINE__, __FILE__, "! pCowCels");
	pCowCels = LoadFileInMem("Towners\\Animals\\Cow.CEL", NULL);
	for (i = 0; i < 3; i++) {
		x = TownCowX[i];
		y = TownCowY[i];
		dir = TownCowDir[i];
		InitTownerInfo(numtowners, 128, FALSE, TOWN_COW, x, y, -1, 10);
		towner[numtowners]._tNData = pCowCels;
		SetTownerGPtrs(towner[numtowners]._tNData, towner[numtowners]._tNAnim);
		towner[numtowners]._tNFrames = 12;
		NewTownerAnim(numtowners, towner[numtowners]._tNAnim[dir], towner[numtowners]._tNFrames, 3);
		towner[numtowners]._tAnimFrame = random_(0, 11) + 1;
		towner[numtowners]._tSelFlag = TRUE;
		strcpy(towner[numtowners]._tName, "Cow");

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

	InitTownerInfo(numtowners, 96, 1, TOWN_FARMER, 62, 16, -1, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\Farmer\\Farmrn2.CEL", NULL);
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 15;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_S], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Lester the farmer");
	numtowners++;
}

void InitCowFarmer()
{
	int i;

	InitTownerInfo(numtowners, 96, 1, TOWN_COWFARM, 61, 22, -1, 10);
	InitQstSnds(numtowners);
	if (quests[Q_JERSEY]._qactive != 3) {
		towner[numtowners]._tNData = LoadFileInMem("Towners\\Farmer\\cfrmrn2.CEL", NULL);
	} else {
		towner[numtowners]._tNData = LoadFileInMem("Towners\\Farmer\\mfrmrn2.CEL", NULL);
	}
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 15;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_SW], towner[numtowners]._tNFrames, 3);
	strcpy(towner[numtowners]._tName, "Complete Nut");
	numtowners++;
}

void InitGirl()
{
	int i;

	InitTownerInfo(numtowners, 96, 1, TOWN_GIRL, 77, 43, -1, 10);
	InitQstSnds(numtowners);
	if (quests[Q_GIRL]._qactive != 3) {
		towner[numtowners]._tNData = LoadFileInMem("Towners\\Girl\\Girlw1.CEL", NULL);
	} else {
		towner[numtowners]._tNData = LoadFileInMem("Towners\\Girl\\Girls1.CEL", NULL);
	}
	for (i = 0; i < 8; i++) {
		towner[numtowners]._tNAnim[i] = towner[numtowners]._tNData;
	}
	towner[numtowners]._tNFrames = 20;
	NewTownerAnim(numtowners, towner[numtowners]._tNAnim[DIR_S], towner[numtowners]._tNFrames, 6);
	strcpy(towner[numtowners]._tName, "Celia");
	numtowners++;
}

void InitTowners()
{
	numtowners = 0;
	boyloadflag = FALSE;
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
		if (gbCowQuest) {
			InitCowFarmer();
		} else if (quests[Q_FARMER]._qactive != 10) {
			InitFarmer();
		}
		if (gbTheoQuest && plr->_pLvlVisited[17]) {
			InitGirl();
		}
	}
}

void FreeTownerGFX()
{
	int i;

	for (i = 0; i < NUM_TOWNERS; i++) {
		if (towner[i]._tNData == pCowCels) {
			towner[i]._tNData = NULL;
		} else if (towner[i]._tNData) {
			MemFreeDbg(towner[i]._tNData);
		}
	}

	MemFreeDbg(pCowCels);
}

void TownCtrlMsg(int i)
{
	int p;
	int dx, dy;

	if (i == -1)
		return;

	if (towner[i]._tbtcnt != 0) {
		p = towner[i]._tVar1;
		dx = abs(towner[i]._tx - plr[p]._px);
		dy = abs(towner[i]._ty - plr[p]._py);
		if (dx >= 2 || dy >= 2) {
			towner[i]._tbtcnt = 0;
			qtextflag = FALSE;
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
			towner[tidx]._tAnimDelay = 1000;
			towner[tidx]._tAnimFrame = 1;
			strcpy(towner[tidx]._tName, "Slain Townsman");
		}
	}
	if (quests[Q_BUTCHER]._qactive != QUEST_INIT)
		towner[tidx]._tAnimCnt = 0;
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
		switch (towner[i]._ttype) {
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
		}

		towner[i]._tAnimCnt++;
		if (towner[i]._tAnimCnt >= towner[i]._tAnimDelay) {
			towner[i]._tAnimCnt = 0;

			if (towner[i]._tAnimOrder >= 0) {
				ao = towner[i]._tAnimOrder;
				towner[i]._tAnimFrameCnt++;
				if (AnimOrder[ao][towner[i]._tAnimFrameCnt] == -1)
					towner[i]._tAnimFrameCnt = 0;

				towner[i]._tAnimFrame = AnimOrder[ao][towner[i]._tAnimFrameCnt];
			} else {
				towner[i]._tAnimFrame++;
				if (towner[i]._tAnimFrame > towner[i]._tAnimLen)
					towner[i]._tAnimFrame = 1;
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

	return NULL;
}

void TownerTalk(int first, int t)
{
	sgdwCowClicks = 0;
	sgnCowMsg = 0;
	storeflag = TRUE;
	InitQTextMsg(first);
}

void TalkToTowner(int p, int t)
{
	int i, dx, dy, rv1, rv2, rv3;
	ItemStruct *Item;
	int qt, t2;

	dx = abs(plr[p]._px - towner[t]._tx);
	dy = abs(plr[p]._py - towner[t]._ty);
#ifdef _DEBUG
	if (!debug_mode_key_d)
#endif
		if (dx >= 2 || dy >= 2)
			return;

	if (qtextflag) {
		return;
	}

	towner[t]._tMsgSaid = FALSE;

	if (pcurs >= CURSOR_FIRSTITEM && !DropItemBeforeTrig()) {
		return;
	}

	if (t == GetActiveTowner(TOWN_TAVERN)) {
		if (!plr[p]._pLvlVisited[0] && !towner[t]._tMsgSaid) {
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			InitQTextMsg(TEXT_INTRO);
			towner[t]._tMsgSaid = TRUE;
		}
		if ((plr[p]._pLvlVisited[2] || plr[p]._pLvlVisited[4]) && quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) {
			if (quests[Q_SKELKING]._qactive != QUEST_NOTAVAIL) {
				if (quests[Q_SKELKING]._qvar2 == 0 && !towner[t]._tMsgSaid) {
					quests[Q_SKELKING]._qvar2 = 1;
					quests[Q_SKELKING]._qlog = TRUE;
					if (quests[Q_SKELKING]._qactive == QUEST_INIT) {
						quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
						quests[Q_SKELKING]._qvar1 = 1;
					}
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_KING2);
					towner[t]._tMsgSaid = TRUE;
					NetSendCmdQuest(TRUE, Q_SKELKING);
				}
			}
			if (quests[Q_SKELKING]._qactive == QUEST_DONE && quests[Q_SKELKING]._qvar2 == 1 && !towner[t]._tMsgSaid) {
				quests[Q_SKELKING]._qvar2 = 2;
				quests[Q_SKELKING]._qvar1 = 2;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_KING4);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, Q_SKELKING);
			}
		}
		if (!gbIsMultiplayer) {
			if (plr[p]._pLvlVisited[3] && quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL) {
				if ((quests[Q_LTBANNER]._qactive == QUEST_INIT || quests[Q_LTBANNER]._qactive == QUEST_ACTIVE) && quests[Q_LTBANNER]._qvar2 == 0 && !towner[t]._tMsgSaid) {
					quests[Q_LTBANNER]._qvar2 = 1;
					if (quests[Q_LTBANNER]._qactive == QUEST_INIT) {
						quests[Q_LTBANNER]._qvar1 = 1;
						quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
					}
					quests[Q_LTBANNER]._qlog = TRUE;
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_BANNER2);
					towner[t]._tMsgSaid = TRUE;
				}
				if (quests[Q_LTBANNER]._qvar2 == 1 && PlrHasItem(p, IDI_BANNER, &i) != NULL && !towner[t]._tMsgSaid) {
					quests[Q_LTBANNER]._qactive = QUEST_DONE;
					quests[Q_LTBANNER]._qvar1 = 3;
					RemoveInvItem(p, i);
					SpawnUnique(UITEM_HARCREST, towner[t]._tx, towner[t]._ty + 1);
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_BANNER3);
					towner[t]._tMsgSaid = TRUE;
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_OGDEN1, t);
			if (storeflag) {
				StartStore(STORE_TAVERN);
			}
		}
	} else if (t == GetActiveTowner(TOWN_DEADGUY)) {
		if (quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qvar1 == 1) {
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			quests[Q_BUTCHER]._qvar1 = 1;
			if (plr[p]._pClass == PC_WARRIOR && !effect_is_playing(PS_WARR8)) {
				PlaySFX(PS_WARR8);
			} else if (plr[p]._pClass == PC_ROGUE && !effect_is_playing(PS_ROGUE8)) {
				PlaySFX(PS_ROGUE8);
			} else if (plr[p]._pClass == PC_SORCERER && !effect_is_playing(PS_MAGE8)) {
				PlaySFX(PS_MAGE8);
			} else if (plr[p]._pClass == PC_MONK && !effect_is_playing(PS_MONK8)) {
				PlaySFX(PS_MONK8);
			} else if (plr[p]._pClass == PC_BARD && !effect_is_playing(PS_ROGUE8)) {
				PlaySFX(PS_ROGUE8);
			} else if (plr[p]._pClass == PC_BARBARIAN && !effect_is_playing(PS_WARR8)) {
				PlaySFX(PS_WARR8);
			}
			towner[t]._tMsgSaid = TRUE;
		} else if (quests[Q_BUTCHER]._qactive == QUEST_DONE && quests[Q_BUTCHER]._qvar1 == 1) {
			quests[Q_BUTCHER]._qvar1 = 1;
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			towner[t]._tMsgSaid = TRUE;
		} else if (quests[Q_BUTCHER]._qactive == QUEST_INIT || quests[Q_BUTCHER]._qactive == QUEST_ACTIVE && quests[Q_BUTCHER]._qvar1 == 0) {
			quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
			quests[Q_BUTCHER]._qlog = TRUE;
			quests[Q_BUTCHER]._qmsg = TEXT_BUTCH9;
			quests[Q_BUTCHER]._qvar1 = 1;
			towner[t]._tbtcnt = 50;
			towner[t]._tVar1 = p;
			towner[t]._tVar2 = 3;
			InitQTextMsg(TEXT_BUTCH9);
			towner[t]._tMsgSaid = TRUE;
			NetSendCmdQuest(TRUE, Q_BUTCHER);
		}
	} else if (t == GetActiveTowner(TOWN_SMITH)) {
		if (!gbIsMultiplayer) {
			if (plr[p]._pLvlVisited[4] && quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
				if (quests[Q_ROCK]._qvar2 == 0) {
					quests[Q_ROCK]._qvar2 = 1;
					quests[Q_ROCK]._qlog = TRUE;
					if (quests[Q_ROCK]._qactive == QUEST_INIT) {
						quests[Q_ROCK]._qactive = QUEST_ACTIVE;
						quests[Q_ROCK]._qvar1 = 1;
					}
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_INFRA5);
					towner[t]._tMsgSaid = TRUE;
				}
				if (quests[Q_ROCK]._qvar2 == 1 && PlrHasItem(p, IDI_ROCK, &i) != NULL && !towner[t]._tMsgSaid) {
					quests[Q_ROCK]._qactive = QUEST_DONE;
					quests[Q_ROCK]._qvar2 = 2;
					quests[Q_ROCK]._qvar1 = 2;
					RemoveInvItem(p, i);
					SpawnUnique(UITEM_INFRARING, towner[t]._tx, towner[t]._ty + 1);
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_INFRA7);
					towner[t]._tMsgSaid = TRUE;
				}
			}
			if (plr[p]._pLvlVisited[9] && quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
				if ((quests[Q_ANVIL]._qactive == QUEST_INIT || quests[Q_ANVIL]._qactive == QUEST_ACTIVE) && quests[Q_ANVIL]._qvar2 == 0 && !towner[t]._tMsgSaid) {
					if (quests[Q_ROCK]._qvar2 == 2 || quests[Q_ROCK]._qactive == QUEST_ACTIVE && quests[Q_ROCK]._qvar2 == 1) {
						quests[Q_ANVIL]._qvar2 = 1;
						quests[Q_ANVIL]._qlog = TRUE;
						if (quests[Q_ANVIL]._qactive == QUEST_INIT) {
							quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
							quests[Q_ANVIL]._qvar1 = 1;
						}
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TEXT_ANVIL5);
						towner[t]._tMsgSaid = TRUE;
					}
				}
				if (quests[Q_ANVIL]._qvar2 == 1 && PlrHasItem(p, IDI_ANVIL, &i) != NULL) {
					if (!towner[t]._tMsgSaid) {
						quests[Q_ANVIL]._qactive = QUEST_DONE;
						quests[Q_ANVIL]._qvar2 = 2;
						quests[Q_ANVIL]._qvar1 = 2;
						RemoveInvItem(p, i);
						SpawnUnique(UITEM_GRISWOLD, towner[t]._tx, towner[t]._ty + 1);
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TEXT_ANVIL7);
						towner[t]._tMsgSaid = TRUE;
					}
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_GRISWOLD1, t);
			if (storeflag) {
				StartStore(STORE_SMITH);
			}
		}
	} else if (t == GetActiveTowner(TOWN_WITCH)) {
		if (quests[Q_MUSHROOM]._qactive == QUEST_INIT && PlrHasItem(p, IDI_FUNGALTM, &i) != NULL) {
			RemoveInvItem(p, i);
			quests[Q_MUSHROOM]._qactive = QUEST_ACTIVE;
			quests[Q_MUSHROOM]._qlog = TRUE;
			quests[Q_MUSHROOM]._qvar1 = QS_TOMEGIVEN;
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			InitQTextMsg(TEXT_MUSH8);
			towner[t]._tMsgSaid = TRUE;
		} else if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
			if (quests[Q_MUSHROOM]._qvar1 >= QS_TOMEGIVEN && quests[Q_MUSHROOM]._qvar1 <= QS_MUSHPICKED) {
				if (PlrHasItem(p, IDI_MUSHROOM, &i) != NULL) {
					RemoveInvItem(p, i);
					quests[Q_MUSHROOM]._qvar1 = 5;
					Qtalklist[TOWN_HEALER]._qblkm = TEXT_MUSH3;
					Qtalklist[TOWN_WITCH]._qblkm = -1;
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[Q_MUSHROOM]._qmsg = TEXT_MUSH10;
					InitQTextMsg(TEXT_MUSH10);
					towner[t]._tMsgSaid = TRUE;
				} else if (quests[Q_MUSHROOM]._qmsg != TEXT_MUSH9) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[Q_MUSHROOM]._qmsg = TEXT_MUSH9;
					InitQTextMsg(TEXT_MUSH9);
					towner[t]._tMsgSaid = TRUE;
				}
			} else {
				Item = PlrHasItem(p, IDI_SPECELIX, &i);
				if (Item != NULL) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(TEXT_MUSH12);
					quests[Q_MUSHROOM]._qactive = QUEST_DONE;
					towner[t]._tMsgSaid = TRUE;
					AllItemsList[Item->IDidx].iUsable = TRUE;
				} else if (PlrHasItem(p, IDI_BRAIN, &i) != NULL && quests[Q_MUSHROOM]._qvar2 != TEXT_MUSH11) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[Q_MUSHROOM]._qvar2 = TEXT_MUSH11;
					InitQTextMsg(TEXT_MUSH11);
					towner[t]._tMsgSaid = TRUE;
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_ADRIA1, t);
			if (storeflag) {
				StartStore(STORE_WITCH);
			}
		}
	} else if (t == GetActiveTowner(TOWN_BMAID)) {
		if (plr[p]._pLvlVisited[21] == false && PlrHasItem(p, IDI_MAPOFDOOM, &i)) {
			quests[Q_GRAVE]._qactive = 2;
			quests[Q_GRAVE]._qlog = 1;
			quests[Q_GRAVE]._qmsg = TEXT_GRAVE8;
			InitQTextMsg(TEXT_GRAVE8);
			towner[t]._tMsgSaid = TRUE;
		}
		if (!qtextflag) {
			TownerTalk(TEXT_GILLIAN1, t);
			if (storeflag) {
				StartStore(STORE_BARMAID);
			}
		}
	} else if (t == GetActiveTowner(TOWN_DRUNK)) {
		if (!qtextflag) {
			TownerTalk(TEXT_FARNHAM1, t);
			if (storeflag) {
				StartStore(STORE_DRUNK);
			}
		}
	} else if (t == GetActiveTowner(TOWN_HEALER)) {
		if (!gbIsMultiplayer) {
			if (plr[p]._pLvlVisited[1] || (gbIsHellfire && plr[p]._pLvlVisited[5])) {
				if (!towner[t]._tMsgSaid) {
					if (quests[Q_PWATER]._qactive == QUEST_INIT) {
						quests[Q_PWATER]._qactive = QUEST_ACTIVE;
						quests[Q_PWATER]._qlog = TRUE;
						quests[Q_PWATER]._qmsg = TEXT_POISON3;
						quests[Q_PWATER]._qvar1 = 1;
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TEXT_POISON3);
						towner[t]._tMsgSaid = TRUE;
					} else if (quests[Q_PWATER]._qactive == QUEST_DONE && quests[Q_PWATER]._qvar1 != 2) {
						quests[Q_PWATER]._qvar1 = 2;
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(TEXT_POISON5);
						SpawnUnique(UITEM_TRING, towner[t]._tx, towner[t]._ty + 1);
						towner[t]._tMsgSaid = TRUE;
					}
				}
			}
			if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qmsg == TEXT_MUSH10 && PlrHasItem(p, IDI_BRAIN, &i) != NULL) {
				RemoveInvItem(p, i);
				SpawnQuestItem(IDI_SPECELIX, towner[t]._tx, towner[t]._ty + 1, 0, 0);
				InitQTextMsg(TEXT_MUSH4);
				quests[Q_MUSHROOM]._qvar1 = QS_BRAINGIVEN;
				Qtalklist[TOWN_HEALER]._qblkm = -1;
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_PEPIN1, t);
			if (storeflag) {
				StartStore(STORE_HEALER);
			}
		}
	} else if (t == GetActiveTowner(TOWN_PEGBOY)) {
		if (!qtextflag) {
			TownerTalk(TEXT_WIRT1, t);
			if (storeflag) {
				StartStore(STORE_BOY);
			}
		}
	} else if (t == GetActiveTowner(TOWN_STORY)) {
		if (!gbIsMultiplayer) {
			if (quests[Q_BETRAYER]._qactive == QUEST_INIT && PlrHasItem(p, IDI_LAZSTAFF, &i) != NULL) {
				RemoveInvItem(p, i);
				quests[Q_BETRAYER]._qvar1 = 2;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_VILE1);
				towner[t]._tMsgSaid = TRUE;
				quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
				quests[Q_BETRAYER]._qlog = TRUE;
			} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE && quests[Q_BETRAYER]._qvar1 == 7) {
				quests[Q_BETRAYER]._qvar1 = 8;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_VILE3);
				towner[t]._tMsgSaid = TRUE;
				quests[Q_DIABLO]._qlog = TRUE;
			}
		}
		if (gbIsMultiplayer) {
			if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE && !quests[Q_BETRAYER]._qlog) {
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_VILE1);
				towner[t]._tMsgSaid = TRUE;
				quests[Q_BETRAYER]._qlog = TRUE;
				NetSendCmdQuest(TRUE, Q_BETRAYER);
			} else if (quests[Q_BETRAYER]._qactive == QUEST_DONE && quests[Q_BETRAYER]._qvar1 == 7) {
				quests[Q_BETRAYER]._qvar1 = 8;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(TEXT_VILE3);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, Q_BETRAYER);
				quests[Q_DIABLO]._qlog = TRUE;
				NetSendCmdQuest(TRUE, Q_DIABLO);
			}
		}
		if (!qtextflag) {
			TownerTalk(TEXT_STORY1, t);
			if (storeflag) {
				StartStore(STORE_STORY);
			}
		}
	} else if (towner[t]._ttype == TOWN_COW) {
		if (!qtextflag)
			CowSFX(p);
	} else if (towner[t]._ttype == TOWN_FARMER) {
		if (!qtextflag) {
			qt = 277;
			t2 = 1;
			switch (quests[Q_FARMER]._qactive) {
			case 0:
				if (PlrHasItem(p, IDI_RUNEBOMB, &i)) {
					qt = TEXT_FARMER2;
					quests[Q_FARMER]._qactive = 2;
					quests[Q_FARMER]._qvar1 = 1;
					quests[Q_FARMER]._qlog = 1;
					quests[Q_FARMER]._qmsg = TEXT_FARMER1;
					break;
				} else if (!plr[p]._pLvlVisited[9] && plr[p]._pLevel < 15) {
					qt = 309;
					if (plr[p]._pLvlVisited[2])
						qt = 281;
					if (plr[p]._pLvlVisited[5])
						qt = 308;
					if (plr[p]._pLvlVisited[7])
						qt = 310;
				} else {
					qt = TEXT_FARMER1;
					quests[Q_FARMER]._qactive = 2;
					quests[Q_FARMER]._qvar1 = 1;
					quests[Q_FARMER]._qlog = 1;
					quests[Q_FARMER]._qmsg = TEXT_FARMER1;
					SpawnRuneBomb(towner[t]._tx + 1, towner[t]._ty);
					t2 = 1;
					break;
				}
			case 2:
				if (PlrHasItem(p, IDI_RUNEBOMB, &i))
					qt = TEXT_FARMER2;
				else
					qt = TEXT_FARMER3;
				break;
			case 1:
				if (PlrHasItem(p, IDI_RUNEBOMB, &i)) {
					qt = TEXT_FARMER2;
					quests[Q_FARMER]._qactive = 2;
					quests[Q_FARMER]._qvar1 = 1;
					quests[Q_FARMER]._qmsg = TEXT_FARMER1;
					quests[Q_FARMER]._qlog = 1;
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
					quests[Q_FARMER]._qactive = 2;
					quests[Q_FARMER]._qvar1 = 1;
					quests[Q_FARMER]._qlog = 1;
					quests[Q_FARMER]._qmsg = TEXT_FARMER1;
					SpawnRuneBomb(towner[t]._tx + 1, towner[t]._ty);
					t2 = 1;
				}
				break;
			case 3:
				qt = TEXT_FARMER4;
				SpawnRewardItem(IDI_AURIC, towner[t]._tx + 1, towner[t]._ty);
				quests[Q_FARMER]._qactive = 10;
				quests[Q_FARMER]._qlog = 0;
				t2 = 1;
				break;
			case 10:
				qt = -1;
				break;
			default:
				quests[Q_FARMER]._qactive = 0;
				qt = TEXT_FARMER4;
				break;
			}
			if (qt != -1) {
				if (t2)
					InitQTextMsg(qt);
				else
					PlaySFX(alltext[qt].sfxnr);
			}
			if (gbIsMultiplayer) {
				NetSendCmdQuest(TRUE, Q_FARMER);
			}
		}
	} else if (towner[t]._ttype == TOWN_COWFARM) {
		if (!qtextflag) {
			qt = 297;
			t2 = 1;
			if (PlrHasItem(p, IDI_GREYSUIT, &i)) {
				qt = TEXT_JERSEY7;
				RemoveInvItem(p, i);
			} else if (PlrHasItem(p, IDI_BROWNSUIT, &i)) {
				SpawnUnique(UITEM_BOVINE, towner[t]._tx + 1, towner[t]._ty);
				RemoveInvItem(p, i);
				qt = TEXT_JERSEY8;
				quests[Q_JERSEY]._qactive = 3;
			} else if (PlrHasItem(p, IDI_RUNEBOMB, &i)) {
				qt = TEXT_JERSEY5;
				quests[Q_JERSEY]._qactive = 2;
				quests[Q_JERSEY]._qvar1 = 1;
				quests[Q_JERSEY]._qmsg = TEXT_JERSEY4;
				quests[Q_JERSEY]._qlog = 1;
			} else {
				switch (quests[Q_JERSEY]._qactive) {
				case 0:
					qt = TEXT_JERSEY1;
					quests[Q_JERSEY]._qactive = 7;
					break;
				case 1:
					qt = TEXT_JERSEY1;
					quests[23]._qactive = 7;
					break;
				case 2:
					qt = TEXT_JERSEY5;
					break;
				case 3:
					qt = TEXT_JERSEY1;
					break;
				case 7:
					qt = TEXT_JERSEY2;
					quests[Q_JERSEY]._qactive = 8;
					break;
				case 8:
					qt = TEXT_JERSEY3;
					quests[Q_JERSEY]._qactive = 9;
					break;
				case 9:
					if (!plr[p]._pLvlVisited[9] && plr[p]._pLevel < 15) {
						switch (random_(0, 4) + 9) {
						case 9:
							qt = TEXT_JERSEY9;
							break;
						case 10:
							qt = TEXT_JERSEY10;
							break;
						case 11:
							qt = TEXT_JERSEY11;
							break;
						default:
							qt = TEXT_JERSEY12;
						}
						break;
					} else {
						qt = TEXT_JERSEY4;
						quests[Q_JERSEY]._qactive = 2;
						quests[Q_JERSEY]._qvar1 = 1;
						quests[Q_JERSEY]._qmsg = TEXT_JERSEY4;
						quests[Q_JERSEY]._qlog = 1;
						SpawnRuneBomb(towner[t]._tx + 1, towner[t]._ty);
						t2 = 1;
					}
					break;
				default:
					qt = TEXT_JERSEY5;
					quests[Q_JERSEY]._qactive = 0;
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
				NetSendCmdQuest(TRUE, Q_JERSEY);
			}
		}
	} else if (towner[t]._ttype == TOWN_GIRL) {
		if (!qtextflag) {
			qt = 282;
			t2 = 0;
			if (!PlrHasItem(p, IDI_THEODORE, &i) || quests[Q_GIRL]._qactive == 3) {
				switch (quests[Q_GIRL]._qactive) {
				case 0:
					qt = TEXT_GIRL2;
					quests[Q_GIRL]._qactive = 2;
					quests[Q_GIRL]._qvar1 = 1;
					quests[Q_GIRL]._qlog = 1;
					quests[Q_GIRL]._qmsg = TEXT_GIRL2;
					t2 = 1;
					break;
				case 1:
					qt = TEXT_GIRL2;
					quests[Q_GIRL]._qvar1 = 1;
					quests[Q_GIRL]._qlog = 1;
					quests[Q_GIRL]._qmsg = TEXT_GIRL2;
					quests[Q_GIRL]._qactive = 2;
					t2 = 1;
					break;
				case 2:
					qt = TEXT_GIRL3;
					t2 = 1;
					break;
				case 3:
					qt = -1;
					break;
				default:
					quests[Q_GIRL]._qactive = 0;
					qt = TEXT_GIRL1;
					break;
				}
			} else {
				qt = TEXT_GIRL4;
				RemoveInvItem(p, i);
				CreateAmulet(towner[t]._tx, towner[t]._ty, 13, 0, 1);
				quests[Q_GIRL]._qlog = 0;
				quests[Q_GIRL]._qactive = 3;
				t2 = 1;
			}
			if (qt != -1) {
				if (t2 != 0) {
					InitQTextMsg(qt);
				} else {
					PlaySFX(alltext[qt].sfxnr);
				}
			}
			if (gbIsMultiplayer) {
				NetSendCmdQuest(TRUE, Q_GIRL);
			}
		}
	}
}

DEVILUTION_END_NAMESPACE
