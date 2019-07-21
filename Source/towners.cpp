#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL storeflag;
int sgnCowMsg;
int numtowners;
DWORD sgdwCowClicks;
BOOL bannerflag; // unused 0x6AAC28
BOOL boyloadflag;
BYTE *pCowCels;
TownerStruct towner[16];

const int snSFX[3][3] = {
	{ PS_WARR52, PS_ROGUE52, PS_MAGE52 },
	{ PS_WARR49, PS_ROGUE49, PS_MAGE49 },
	{ PS_WARR50, PS_ROGUE50, PS_MAGE50 }
};

/* data */

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
int TownCowX[3] = { 58, 56, 59 };
int TownCowY[3] = { 16, 14, 20 };
int TownCowDir[3] = { 1, 3, 4 };
int cowoffx[8] = { -1, 0, -1, -1, -1, 0, -1, -1 };
int cowoffy[8] = { -1, -1, -1, 0, -1, -1, -1, 0 };
QuestTalkData Qtalklist[11] = {
	// clang-format off
	// _qinfra,      _qblkm,       _qgarb,      _qzhar,      _qveil,      _qmod,       _qbutch,      _qbol,         _qblind,      _qblood,      _qanvil,      _qwarlrd,      _qking,       _qpw,           _qbone,      _qvb
	{ QUEST_INFRA6,  QUEST_MUSH6,  -1,          -1,          QUEST_VEIL5, -1,          QUEST_BUTCH5, QUEST_BANNER6, QUEST_BLIND5, QUEST_BLOOD5, QUEST_ANVIL6, QUEST_WARLRD5, QUEST_KING7,  QUEST_POISON7,  QUEST_BONE5, QUEST_VILE9  },
	{ QUEST_INFRA3,  -1,           -1,          -1,          QUEST_VEIL3, -1,          QUEST_BUTCH3, QUEST_BANNER4, QUEST_BLIND3, QUEST_BLOOD3, QUEST_ANVIL3, QUEST_WARLRD3, QUEST_KING5,  QUEST_POISON4,  QUEST_BONE3, QUEST_VILE7  },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1           },
	{ QUEST_INFRA2,  QUEST_MUSH2,  -1,          -1,          QUEST_VEIL2, -1,          QUEST_BUTCH2, -1,            QUEST_BLIND2, QUEST_BLOOD2, QUEST_ANVIL2, QUEST_WARLRD2, QUEST_KING3,  QUEST_POISON2,  QUEST_BONE2, QUEST_VILE4  },
	{ QUEST_INFRA1,  QUEST_MUSH1,  -1,          -1,          QUEST_VEIL1, QUEST_VILE3, QUEST_BUTCH1, QUEST_BANNER1, QUEST_BLIND1, QUEST_BLOOD1, QUEST_ANVIL1, QUEST_WARLRD1, QUEST_KING1,  QUEST_POISON1,  QUEST_BONE1, QUEST_VILE2  },
	{ QUEST_INFRA8,  QUEST_MUSH7,  -1,          -1,          QUEST_VEIL6, -1,          QUEST_BUTCH6, QUEST_BANNER7, QUEST_BLIND6, QUEST_BLOOD6, QUEST_ANVIL8, QUEST_WARLRD6, QUEST_KING8,  QUEST_POISON8,  QUEST_BONE6, QUEST_VILE10 },
	{ QUEST_INFRA9,  QUEST_MUSH9,  -1,          -1,          QUEST_VEIL7, -1,          QUEST_BUTCH7, QUEST_BANNER8, QUEST_BLIND7, QUEST_BLOOD7, QUEST_ANVIL9, QUEST_WARLRD7, QUEST_KING9,  QUEST_POISON9,  QUEST_BONE7, QUEST_VILE11 },
	{ QUEST_INFRA4,  QUEST_MUSH5,  -1,          -1,          QUEST_VEIL4, -1,          QUEST_BUTCH4, QUEST_BANNER5, QUEST_BLIND4, QUEST_BLOOD4, QUEST_ANVIL4, QUEST_WARLRD4, QUEST_KING6,  QUEST_POISON6,  QUEST_BONE4, QUEST_VILE8  },
	{ QUEST_INFRA10, QUEST_MUSH13, -1,          -1,          QUEST_VEIL8, -1,          QUEST_BUTCH8, QUEST_BANNER9, QUEST_BLIND8, QUEST_BLOOD8, QUEST_ANVIL10,QUEST_WARLRD8, QUEST_KING10, QUEST_POISON10, QUEST_BONE8, QUEST_VILE12 },
	{ -1,            -1,           -1,          -1,          -1,          -1,          -1,           -1,            -1,           -1,           -1,           -1,            -1,           -1,             -1,          -1           },
	{ QUEST_KING1,   QUEST_KING1,  QUEST_KING1, QUEST_KING1, QUEST_KING1, QUEST_KING1, QUEST_KING1,  QUEST_KING1,   QUEST_KING1,  QUEST_KING1,  QUEST_KING1,  QUEST_KING1,   QUEST_KING1,  QUEST_KING1,    QUEST_KING1, QUEST_KING1  }
	// clang-format on
};
int CowPlaying = -1;

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
#ifdef USE_ASM
	BYTE *src;

	for (i = 0; i < 8; i++) {
		src = pData;
		__asm {
			mov		eax, src
			mov		ebx, eax
			mov		edx, i
			shl		edx, 2
			add		ebx, edx
			mov		edx, [ebx]
			add		eax, edx
			mov		src, eax
		}
		pAnim[i] = src;
	}
#else
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)pData;

	for (i = 0; i < 8; i++) {
		pAnim[i] = &pData[pFrameTable[i]];
	}
#endif
}

void NewTownerAnim(int tnum, BYTE *pAnim, int numFrames, int Delay)
{
	towner[tnum]._tAnimCnt = 0;
	towner[tnum]._tAnimLen = numFrames;
	towner[tnum]._tAnimData = pAnim;
	towner[tnum]._tAnimFrame = 1;
	towner[tnum]._tAnimDelay = Delay;
}

void InitTownerInfo(int i, int w, int sel, int t, int x, int y, int ao, int tp)
{
	memset(&towner[i], 0, sizeof(TownerStruct));
	towner[i]._tSelFlag = sel;
	towner[i]._tAnimWidth = w;
	towner[i]._tAnimWidth2 = (w - 64) >> 1;
	towner[i]._tMsgSaid = 0;
	towner[i]._ttype = t;
	towner[i]._tx = x;
	towner[i]._ty = y;
	dMonster[x][y] = i + 1;
	towner[i]._tAnimOrder = ao;
	towner[i]._tTenPer = tp;
	towner[i]._tSeed = GetRndSeed();
}

void InitQstSnds(int i)
{
	int j, tl;
	tl = i;
	if (boyloadflag)
		tl++;
	for (j = 0; j < 16; j++) {
		towner[i].qsts[j]._qsttype = quests[j]._qtype;
		towner[i].qsts[j]._qstmsg = ((int *)(Qtalklist + tl))[j];
		if (((int *)(Qtalklist + tl))[j] != -1)
			towner[i].qsts[j]._qstmsgact = TRUE;
		else
			towner[i].qsts[j]._qstmsgact = FALSE;
	}
}

void InitSmith()
{
	int i;

	InitTownerInfo(numtowners, 96, 1, TOWN_SMITH, 62, 63, 0, 10);
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
	int i;

	bannerflag = FALSE; // unused
	InitTownerInfo(numtowners, 96, 1, TOWN_TAVERN, 55, 62, 3, 10);
	InitQstSnds(numtowners);
	towner[numtowners]._tNData = LoadFileInMem("Towners\\TwnF\\TwnFN.CEL", NULL);
	for (i = 0; i < 8; i++) {
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

	InitTownerInfo(numtowners, 96, 1, TOWN_DEADGUY, 24, 32, -1, 10);
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

	InitTownerInfo(numtowners, 96, 1, TOWN_WITCH, 80, 20, 5, 10);
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

	InitTownerInfo(numtowners, 96, 1, TOWN_BMAID, 43, 66, -1, 10);
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
	InitTownerInfo(numtowners, 96, 1, TOWN_PEGBOY, 11, 53, -1, 10);
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

	InitTownerInfo(numtowners, 96, 1, TOWN_HEALER, 55, 79, 1, 10);
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

	InitTownerInfo(numtowners, 96, 1, TOWN_STORY, 62, 71, 2, 10);
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

	InitTownerInfo(numtowners, 96, 1, TOWN_DRUNK, 71, 84, 4, 10);
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
	//	assertion_failed(300, "C:\\Diablo\\Direct\\towners.cpp", "! pCowCels");
	pCowCels = LoadFileInMem("Towners\\Animals\\Cow.CEL", NULL);
	for (i = 0; i < 3; i++) {
		x = TownCowX[i];
		y = TownCowY[i];
		dir = TownCowDir[i];
		InitTownerInfo(numtowners, 128, 0, TOWN_COW, TownCowX[i], TownCowY[i], -1, 10);
		towner[numtowners]._tNData = pCowCels;
		SetTownerGPtrs(towner[numtowners]._tNData, towner[numtowners]._tNAnim);
		towner[numtowners]._tNFrames = 12;
		NewTownerAnim(numtowners, towner[numtowners]._tNAnim[dir], towner[numtowners]._tNFrames, 3);
		towner[numtowners]._tAnimFrame = random(0, 11) + 1;
		towner[numtowners]._tSelFlag = 1;
		strcpy(towner[numtowners]._tName, "Cow");

		xo = x + cowoffx[dir];
		yo = y + cowoffy[dir];
		if (!dMonster[x][yo])
			dMonster[x][yo] = -(numtowners + 1);
		if (!dMonster[xo][y])
			dMonster[xo][y] = -(numtowners + 1);
		if (!dMonster[xo][yo])
			dMonster[xo][yo] = -(numtowners + 1);

		numtowners++;
	}
}

void InitTowners()
{
	numtowners = 0;
	boyloadflag = FALSE;
	InitSmith();
	InitHealer();
	if (quests[QTYPE_BUTCH]._qactive && quests[QTYPE_BUTCH]._qactive != 3)
		InitTownDead();
	InitBarOwner();
	InitTeller();
	InitDrunk();
	InitWitch();
	InitBarmaid();
	InitBoy();
	InitCows();
}

void FreeTownerGFX()
{
	int i;

	for (i = 0; i < 16; i++) {
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

	if (towner[i]._tbtcnt) {
		p = towner[i]._tVar1;
		dx = abs(towner[i]._tx - plr[p].WorldX);
		dy = abs(towner[i]._ty - plr[p].WorldY);
		if (dx >= 2 || dy >= 2)
			towner[i]._tbtcnt = 0;
		if (!towner[i]._tbtcnt) {
			qtextflag = FALSE;
			sfx_stop();
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
		if ((quests[6]._qactive != 2 || quests[6]._qlog) && quests[6]._qactive != 1) {
			towner[tidx]._tAnimDelay = 1000;
			towner[tidx]._tAnimFrame = 1;
			strcpy(towner[tidx]._tName, "Slain Townsman");
		} else {
			return;
		}
	}
	if (quests[QTYPE_BUTCH]._qactive != 1)
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

void ProcessTowners()
{
	int i, ao;

	for (i = 0; i < 16; i++) {
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

ItemStruct *PlrHasItem(int pnum, int item, int *inv_item_num)
{
	int i;

	for (i = 0, *inv_item_num = i; i < plr[pnum]._pNumInv; i = *inv_item_num + 1, *inv_item_num = i) {
		if (plr[pnum].InvList[*inv_item_num].IDidx == item)
			return &plr[pnum].InvList[*inv_item_num];
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

	rv1 = random(6, 3); /* unused */
	rv2 = random(6, 4); /* unused */
	rv3 = random(6, 5); /* unused */

	dx = abs(plr[p].WorldX - towner[t]._tx);
	dy = abs(plr[p].WorldY - towner[t]._ty);
#ifdef _DEBUG
	if (!debug_mode_key_d && (dx >= 2 || dy >= 2)) {
		return;
	}
#else
	if (dx >= 2 || dy >= 2) {
		return;
	}
#endif

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
			InitQTextMsg(QUEST_INTRO);
			towner[t]._tMsgSaid = TRUE;
		}
		if ((plr[p]._pLvlVisited[2] || plr[p]._pLvlVisited[4]) && quests[QTYPE_KING]._qactive != 0) {
			if (quests[QTYPE_KING]._qvar2 == 0 && !towner[t]._tMsgSaid) {
				quests[QTYPE_KING]._qvar2 = 1;
				quests[QTYPE_KING]._qlog = TRUE;
				if (quests[QTYPE_KING]._qactive == 1) {
					quests[QTYPE_KING]._qactive = 2;
					quests[QTYPE_KING]._qvar1 = 1;
				}
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(QUEST_KING2);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, QTYPE_KING);
			}
			if (quests[QTYPE_KING]._qactive == 3 && quests[QTYPE_KING]._qvar2 == 1 && !towner[t]._tMsgSaid) {
				quests[QTYPE_KING]._qvar2 = 2;
				quests[QTYPE_KING]._qvar1 = 2;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(QUEST_KING4);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, QTYPE_KING);
			}
		}
		if (gbMaxPlayers == 1 && plr[p]._pLvlVisited[3] && quests[QTYPE_BOL]._qactive != 0) {
			if ((quests[QTYPE_BOL]._qactive == 1 || quests[QTYPE_BOL]._qactive == 2) && quests[QTYPE_BOL]._qvar2 == 0 && !towner[t]._tMsgSaid) {
				quests[QTYPE_BOL]._qvar2 = 1;
				if (quests[QTYPE_BOL]._qactive == 1) {
					quests[QTYPE_BOL]._qvar1 = 1;
					quests[QTYPE_BOL]._qactive = 2;
				}
				quests[QTYPE_BOL]._qlog = TRUE;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(QUEST_BANNER2);
				towner[t]._tMsgSaid = TRUE;
			}
			if (quests[QTYPE_BOL]._qvar2 == 1 && PlrHasItem(p, IDI_BANNER, &i) != NULL && !towner[t]._tMsgSaid) {
				quests[QTYPE_BOL]._qactive = 3;
				quests[QTYPE_BOL]._qvar1 = 3;
				RemoveInvItem(p, i);
				CreateItem(UITEM_HARCREST, towner[t]._tx, towner[t]._ty + 1);
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(QUEST_BANNER3);
				towner[t]._tMsgSaid = TRUE;
			}
		}
		if (!qtextflag) {
			TownerTalk(QUEST_OGDEN1, t);
			if (storeflag) {
				StartStore(STORE_TAVERN);
			}
		}
	} else if (t == GetActiveTowner(TOWN_DEADGUY)) {
		if (quests[QTYPE_BUTCH]._qactive == 2 && quests[QTYPE_BUTCH]._qvar1 == 1) {
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			quests[QTYPE_BUTCH]._qvar1 = 1;
			if (plr[p]._pClass == 0 && !effect_is_playing(PS_WARR8)) {
				PlaySFX(PS_WARR8);
			} else if (plr[p]._pClass == 1 && !effect_is_playing(PS_ROGUE8)) {
				PlaySFX(PS_ROGUE8);
			} else if (plr[p]._pClass == 2 && !effect_is_playing(PS_MAGE8)) {
				PlaySFX(PS_MAGE8);
			}
			towner[t]._tMsgSaid = TRUE;
		} else if (quests[QTYPE_BUTCH]._qactive == 3 && quests[QTYPE_BUTCH]._qvar1 == 1) {
			quests[QTYPE_BUTCH]._qvar1 = 1;
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			towner[t]._tMsgSaid = TRUE;
		} else if (quests[QTYPE_BUTCH]._qactive == 1 || quests[QTYPE_BUTCH]._qactive == 2 && quests[QTYPE_BUTCH]._qvar1 == 0) {
			quests[QTYPE_BUTCH]._qactive = 2;
			quests[QTYPE_BUTCH]._qlog = TRUE;
			quests[QTYPE_BUTCH]._qmsg = QUEST_BUTCH9;
			quests[QTYPE_BUTCH]._qvar1 = 1;
			towner[t]._tbtcnt = 50;
			towner[t]._tVar1 = p;
			towner[t]._tVar2 = 3;
			InitQTextMsg(QUEST_BUTCH9);
			towner[t]._tMsgSaid = TRUE;
			NetSendCmdQuest(TRUE, QTYPE_BUTCH);
		}
	} else if (t == GetActiveTowner(TOWN_SMITH)) {
		if (gbMaxPlayers == 1) {
			if (plr[p]._pLvlVisited[4] && quests[QTYPE_INFRA]._qactive != 0) {
				if (quests[QTYPE_INFRA]._qvar2 == 0) {
					quests[QTYPE_INFRA]._qvar2 = 1;
					quests[QTYPE_INFRA]._qlog = TRUE;
					if (quests[QTYPE_INFRA]._qactive == 1) {
						quests[QTYPE_INFRA]._qactive = 2;
						quests[QTYPE_INFRA]._qvar1 = 1;
					}
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(QUEST_INFRA5);
					towner[t]._tMsgSaid = TRUE;
				}
				if (quests[QTYPE_INFRA]._qvar2 == 1 && PlrHasItem(p, IDI_ROCK, &i) != NULL && !towner[t]._tMsgSaid) {
					quests[QTYPE_INFRA]._qactive = 3;
					quests[QTYPE_INFRA]._qvar2 = 2;
					quests[QTYPE_INFRA]._qvar1 = 2;
					RemoveInvItem(p, i);
					CreateItem(UITEM_INFRARING, towner[t]._tx, towner[t]._ty + 1);
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(QUEST_INFRA7);
					towner[t]._tMsgSaid = TRUE;
				}
			}
			if (plr[p]._pLvlVisited[9] && quests[QTYPE_ANVIL]._qactive != 0) {
				if ((quests[QTYPE_ANVIL]._qactive == 1 || quests[QTYPE_ANVIL]._qactive == 2) && quests[QTYPE_ANVIL]._qvar2 == 0 && !towner[t]._tMsgSaid) {
					if (quests[QTYPE_INFRA]._qvar2 == 2 || quests[QTYPE_INFRA]._qactive == 2 && quests[QTYPE_INFRA]._qvar2 == 1) {
						quests[QTYPE_ANVIL]._qvar2 = 1;
						quests[QTYPE_ANVIL]._qlog = TRUE;
						if (quests[QTYPE_ANVIL]._qactive == 1) {
							quests[QTYPE_ANVIL]._qactive = 2;
							quests[QTYPE_ANVIL]._qvar1 = 1;
						}
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(QUEST_ANVIL5);
						towner[t]._tMsgSaid = TRUE;
					}
				}
				if (quests[QTYPE_ANVIL]._qvar2 == 1 && PlrHasItem(p, IDI_ANVIL, &i) != NULL) {
					if (!towner[t]._tMsgSaid) {
						quests[QTYPE_ANVIL]._qactive = 3;
						quests[QTYPE_ANVIL]._qvar2 = 2;
						quests[QTYPE_ANVIL]._qvar1 = 2;
						RemoveInvItem(p, i);
						CreateItem(UITEM_GRISWOLD, towner[t]._tx, towner[t]._ty + 1);
						towner[t]._tbtcnt = 150;
						towner[t]._tVar1 = p;
						InitQTextMsg(QUEST_ANVIL7);
						towner[t]._tMsgSaid = TRUE;
					}
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(QUEST_GRISWOLD1, t);
			if (storeflag) {
				StartStore(STORE_SMITH);
			}
		}
	} else if (t == GetActiveTowner(TOWN_WITCH)) {
		if (quests[QTYPE_BLKM]._qactive == 1 && PlrHasItem(p, IDI_FUNGALTM, &i) != NULL) {
			RemoveInvItem(p, i);
			quests[QTYPE_BLKM]._qactive = 2;
			quests[QTYPE_BLKM]._qlog = TRUE;
			quests[QTYPE_BLKM]._qvar1 = 2;
			towner[t]._tbtcnt = 150;
			towner[t]._tVar1 = p;
			InitQTextMsg(QUEST_MUSH8);
			towner[t]._tMsgSaid = TRUE;
		} else if (quests[QTYPE_BLKM]._qactive == 2) {
			if (quests[QTYPE_BLKM]._qvar1 >= 2 && quests[QTYPE_BLKM]._qvar1 <= 4) {
				if (PlrHasItem(p, IDI_MUSHROOM, &i) != NULL) {
					RemoveInvItem(p, i);
					quests[QTYPE_BLKM]._qvar1 = 5;
					Qtalklist[TOWN_HEALER]._qblkm = QUEST_MUSH3;
					Qtalklist[TOWN_WITCH]._qblkm = -1;
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[QTYPE_BLKM]._qmsg = QUEST_MUSH10;
					InitQTextMsg(QUEST_MUSH10);
					towner[t]._tMsgSaid = TRUE;
				} else if (quests[QTYPE_BLKM]._qmsg != QUEST_MUSH9) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[QTYPE_BLKM]._qmsg = QUEST_MUSH9;
					InitQTextMsg(QUEST_MUSH9);
					towner[t]._tMsgSaid = TRUE;
				}
			} else {
				Item = PlrHasItem(p, IDI_SPECELIX, &i);
				if (Item != NULL) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(QUEST_MUSH12);
					quests[QTYPE_BLKM]._qactive = 3;
					towner[t]._tMsgSaid = TRUE;
					AllItemsList[Item->IDidx].iUsable = TRUE;
				} else if (PlrHasItem(p, IDI_BRAIN, &i) != NULL && quests[QTYPE_BLKM]._qvar2 != QUEST_MUSH11) {
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					quests[QTYPE_BLKM]._qvar2 = QUEST_MUSH11;
					InitQTextMsg(QUEST_MUSH11);
					towner[t]._tMsgSaid = TRUE;
				}
			}
		}
		if (!qtextflag) {
			TownerTalk(QUEST_ADRIA1, t);
			if (storeflag) {
				StartStore(STORE_WITCH);
			}
		}
	} else if (t == GetActiveTowner(TOWN_BMAID)) {
		if (!qtextflag) {
			TownerTalk(QUEST_GILLIAN1, t);
			if (storeflag) {
				StartStore(STORE_BARMAID);
			}
		}
	} else if (t == GetActiveTowner(TOWN_DRUNK)) {
		if (!qtextflag) {
			TownerTalk(QUEST_FARNHAM1, t);
			if (storeflag) {
				StartStore(STORE_DRUNK);
			}
		}
	} else if (t == GetActiveTowner(TOWN_HEALER)) {
		if (gbMaxPlayers == 1) {
			if (plr[p]._pLvlVisited[1] && !towner[t]._tMsgSaid) {
				if (quests[QTYPE_PW]._qactive == 1) {
					quests[QTYPE_PW]._qactive = 2;
					quests[QTYPE_PW]._qlog = TRUE;
					quests[QTYPE_PW]._qmsg = QUEST_POISON3;
					quests[QTYPE_PW]._qvar1 = 1;
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(QUEST_POISON3);
					towner[t]._tMsgSaid = TRUE;
				} else if (quests[QTYPE_PW]._qactive == 3 && quests[QTYPE_PW]._qvar1 != 2) {
					quests[QTYPE_PW]._qvar1 = 2;
					towner[t]._tbtcnt = 150;
					towner[t]._tVar1 = p;
					InitQTextMsg(QUEST_POISON5);
					CreateItem(UITEM_TRING, towner[t]._tx, towner[t]._ty + 1);
					towner[t]._tMsgSaid = TRUE;
				}
			}
			if (quests[QTYPE_BLKM]._qactive == 2 && quests[QTYPE_BLKM]._qmsg == QUEST_MUSH10 && PlrHasItem(p, IDI_BRAIN, &i) != NULL) {
				RemoveInvItem(p, i);
				SpawnQuestItem(IDI_SPECELIX, towner[t]._tx, towner[t]._ty + 1, 0, 0);
				InitQTextMsg(QUEST_MUSH4);
				quests[QTYPE_BLKM]._qvar1 = 7;
				Qtalklist[TOWN_HEALER]._qblkm = -1;
			}
		}
		if (!qtextflag) {
			TownerTalk(QUEST_PEPIN1, t);
			if (storeflag) {
				StartStore(STORE_HEALER);
			}
		}
	} else if (t == GetActiveTowner(TOWN_PEGBOY)) {
		if (!qtextflag) {
			TownerTalk(QUEST_WIRT1, t);
			if (storeflag) {
				StartStore(STORE_BOY);
			}
		}
	} else if (t == GetActiveTowner(TOWN_STORY)) {
		if (gbMaxPlayers == 1) {
			if (quests[QTYPE_VB]._qactive == 1 && PlrHasItem(p, IDI_LAZSTAFF, &i) != NULL) {
				RemoveInvItem(p, i);
				quests[QTYPE_VB]._qvar1 = 2;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(QUEST_VILE1);
				quests[QTYPE_VB]._qactive = 2;
				quests[QTYPE_VB]._qlog = TRUE;
				towner[t]._tMsgSaid = TRUE;
			} else if (quests[QTYPE_VB]._qactive == 3 && quests[QTYPE_VB]._qvar1 == 7) {
				quests[QTYPE_VB]._qvar1 = 8;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(QUEST_VILE3);
				quests[QTYPE_MOD]._qlog = TRUE;
				towner[t]._tMsgSaid = TRUE;
			}
		}
		if (gbMaxPlayers != 1) {
			if (quests[QTYPE_VB]._qactive == 2 && !quests[QTYPE_VB]._qlog) {
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(QUEST_VILE1);
				towner[t]._tMsgSaid = TRUE;
				quests[QTYPE_VB]._qlog = TRUE;
				NetSendCmdQuest(TRUE, QTYPE_VB);
			} else if (quests[QTYPE_VB]._qactive == 3 && quests[QTYPE_VB]._qvar1 == 7) {
				quests[QTYPE_VB]._qvar1 = 8;
				towner[t]._tbtcnt = 150;
				towner[t]._tVar1 = p;
				InitQTextMsg(QUEST_VILE3);
				towner[t]._tMsgSaid = TRUE;
				NetSendCmdQuest(TRUE, QTYPE_VB);
				quests[QTYPE_MOD]._qlog = TRUE;
				NetSendCmdQuest(TRUE, QTYPE_MOD);
			}
		}
		if (!qtextflag) {
			TownerTalk(QUEST_STORY1, t);
			if (storeflag) {
				StartStore(STORE_STORY);
			}
		}
	} else if (towner[t]._ttype == TOWN_COW && !qtextflag) {
		CowSFX(p);
	}
}

void CowSFX(int pnum)
{
	if (CowPlaying == -1 || !effect_is_playing(CowPlaying)) {
		sgdwCowClicks++;
		if (sgdwCowClicks >= 8) {
			PlaySfxLoc(TSFX_COW1, plr[pnum].WorldX, plr[pnum].WorldY + 5);
			sgdwCowClicks = 4;
			CowPlaying = snSFX[sgnCowMsg][plr[pnum]._pClass]; /* snSFX is local */
			sgnCowMsg++;
			if (sgnCowMsg >= 3)
				sgnCowMsg = 0;
		} else {
			CowPlaying = sgdwCowClicks == 4 ? TSFX_COW2 : TSFX_COW1;
		}
		PlaySfxLoc(CowPlaying, plr[pnum].WorldX, plr[pnum].WorldY);
	}
}

DEVILUTION_END_NAMESPACE
