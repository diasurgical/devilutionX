//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int storeflag; // weak
int sgnCowMsg; // weak
int numtowners; // idb
int sgdwCowClicks; // weak
int bannerflag; // weak // unused 0x6AAC28
int boyloadflag; // weak
void *pCowCels; // idb
TownerStruct towner[16];
#endif


const int snSFX[3][3] =
{
  { PS_WARR52, PS_ROGUE52, PS_MAGE52 },
  { PS_WARR49, PS_ROGUE49, PS_MAGE49 },
  { PS_WARR50, PS_ROGUE50, PS_MAGE50 }
};

/* data */

char AnimOrder[6][148] =
{
	{
		5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	   14,  13,  12,  11,  10,   9,   8,   7,   6,   5,
		5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	   14,  13,  12,  11,  10,   9,   8,   7,   6,   5,
		5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	   14,  13,  12,  11,  10,   9,   8,   7,   6,   5,
		5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	   14,  13,  12,  11,  10,   9,   8,   7,   6,   5,
		5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	   14,  13,  12,  11,  10,   9,   8,   7,   6,   5,
		5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	   15,   5,   1,   1,   1,   1,   1,   1,   1,   1,
		1,   1,   1,   1,   1,   1,   1,   2,   3,   4,
	  -1
	},
	{
		1,   2,   3,   3,   2,   1,  20,  19,  19,  20,
		1,   2,   3,   3,   2,   1,  20,  19,  19,  20,
		1,   2,   3,   3,   2,   1,  20,  19,  19,  20,
		1,   2,   3,   3,   2,   1,  20,  19,  19,  20,
		1,   2,   3,   4,   5,   6,   7,   8,   9,  10,
	   11,  12,  13,  14,  15,  16,  15,  14,  13,  12,
	   11,  10,   9,   8,   7,   6,   5,   4,   5,   6,
		7,   8,   9,  10,  11,  12,  13,  14,  15,  16,
	   15,  14,  13,  12,  11,  10,   9,   8,   7,   6,
		5,   4,   5,   6,   7,   8,   9,  10,  11,  12,
	   13,  14,  15,  16,  17,  18,  19,  20, -1
	},
	{
		1,   1,  25,  25,  24,  23,  22,  21,  20,  19,
	   18,  17,  16,  15,  16,  17,  18,  19,  20,  21,
	   22,  23,  24,  25,  25,  25,   1,   1,   1,  25,
		1,   2,   3,   4,   5,   6,   7,   8,   9,  10,
	   11,  12,  13,  14,  15,  14,  13,  12,  11,  10,
		9,   8,   7,   6,   5,   4,   3,   2,   1, -1
	},
	{
		1,   2,   3,   3,   2,   1,  16,  15,  14,  14,
	   16,   1,   2,   3,   3,   2,   1,  16,  15,  14,
	   14,  15,  16,   1,   2,   3,   3,   2,   1,  16,
	   15,  14,  14,  15,  16,   1,   2,   3,   3,   2,
		1,  16,  15,  14,  14,  15,  16,   1,   2,   3,
		3,   2,   1,  16,  15,  14,  14,  15,  16,   1,
		2,   3,   3,   2,   1,  16,  15,  14,  14,  15,
	   16,   1,   2,   3,   3,   2,   1,  16,  15,  14,
	   14,  15,  16,   1,   2,   3,   2,   1,  16,  15,
	   14,  14,  15,  16,   1,   2,   3,   4,   5,   6,
		7,   8,   9,  10,  11,  12,  13,  14,  15,  16,
	  -1
	},
	{
		1,   1,   1,   2,   3,   4,   5,   6,   7,   8,
		9,  10,  11,  11,  11,  11,  12,  13,  14,  15,
	   16,  17,  18,  18,   1,   1,   1,  18,  17,  16,
	   15,  14,  13,  12,  11,  10,  11,  12,  13,  14,
	   15,  16,  17,  18,   1,   2,   3,   4,   5,   5,
		5,   4,   3,   2, -1
	},
	{
		4,   4,   4,   5,   6,   6,   6,   5,   4,  15,
	   14,  13,  13,  13,  14,  15,   4,   5,   6,   6,
		6,   5,   4,   4,   4,   5,   6,   6,   6,   5,
		4,  15,  14,  13,  13,  13,  14,  15,   4,   5,
		6,   6,   6,   5,   4,   4,   4,   5,   6,   6,
		6,   5,   4,  15,  14,  13,  13,  13,  14,  15,
		4,   5,   6,   6,   6,   5,   4,   3,   2,   1,
	   19,  18,  19,   1,   2,   1,  19,  18,  19,   1,
		2,   1,   2,   3,   4,   5,   6,   7,   8,   9,
	   10,  11,  12,  13,  14,  15,  15,  15,  14,  13,
	   13,  13,  13,  14,  15,  15,  15,  14,  13,  12,
	   12,  12,  11,  10,  10,  10,   9,   8,   9,  10,
	   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
		1,   2,   1,  19,  18,  19,   1,   2,   1,   2,
		3, -1
	}
};
int TownCowX[3] = { 58, 56, 59 };
int TownCowY[3] = { 16, 14, 20 };
int TownCowDir[3] = { 1, 3, 4 };
int cowoffx[8] = { -1, 0, -1, -1, -1, 0, -1, -1 };
int cowoffy[8] = { -1, -1, -1, 0, -1, -1, -1, 0 };
QuestTalkData Qtalklist[11] =
{
  {
	QUEST_INFRA6,
	QUEST_MUSH6,
	-1,
	-1,
	QUEST_VEIL5,
	-1,
	QUEST_BUTCH5,
	QUEST_BANNER6,
	QUEST_BLIND5,
	QUEST_BLOOD5,
	QUEST_ANVIL6,
	QUEST_WARLRD5,
	QUEST_KING7,
	QUEST_POISON7,
	QUEST_BONE5,
	QUEST_VILE9
  },
  {
	QUEST_INFRA3,
	-1,
	-1,
	-1,
	QUEST_VEIL3,
	-1,
	QUEST_BUTCH3,
	QUEST_BANNER4,
	QUEST_BLIND3,
	QUEST_BLOOD3,
	QUEST_ANVIL3,
	QUEST_WARLRD3,
	QUEST_KING5,
	QUEST_POISON4,
	QUEST_BONE3,
	QUEST_VILE7
  },
  {
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1
  },
  {
	QUEST_INFRA2,
	QUEST_MUSH2,
	-1,
	-1,
	QUEST_VEIL2,
	-1,
	QUEST_BUTCH2,
	-1,
	QUEST_BLIND2,
	QUEST_BLOOD2,
	QUEST_ANVIL2,
	QUEST_WARLRD2,
	QUEST_KING3,
	QUEST_POISON2,
	QUEST_BONE2,
	QUEST_VILE4
  },
  {
	QUEST_INFRA1,
	QUEST_MUSH1,
	-1,
	-1,
	QUEST_VEIL1,
	QUEST_VILE3,
	QUEST_BUTCH1,
	QUEST_BANNER1,
	QUEST_BLIND1,
	QUEST_BLOOD1,
	QUEST_ANVIL1,
	QUEST_WARLRD1,
	QUEST_KING1,
	QUEST_POISON1,
	QUEST_BONE1,
	QUEST_VILE2
  },
  {
	QUEST_INFRA8,
	QUEST_MUSH7,
	-1,
	-1,
	QUEST_VEIL6,
	-1,
	QUEST_BUTCH6,
	QUEST_BANNER7,
	QUEST_BLIND6,
	QUEST_BLOOD6,
	QUEST_ANVIL8,
	QUEST_WARLRD6,
	QUEST_KING8,
	QUEST_POISON8,
	QUEST_BONE6,
	QUEST_VILE10
  },
  {
	QUEST_INFRA9,
	QUEST_MUSH9,
	-1,
	-1,
	QUEST_VEIL7,
	-1,
	QUEST_BUTCH7,
	QUEST_BANNER8,
	QUEST_BLIND7,
	QUEST_BLOOD7,
	QUEST_ANVIL9,
	QUEST_WARLRD7,
	QUEST_KING9,
	QUEST_POISON9,
	QUEST_BONE7,
	QUEST_VILE11
  },
  {
	QUEST_INFRA4,
	QUEST_MUSH5,
	-1,
	-1,
	QUEST_VEIL4,
	-1,
	QUEST_BUTCH4,
	QUEST_BANNER5,
	QUEST_BLIND4,
	QUEST_BLOOD4,
	QUEST_ANVIL4,
	QUEST_WARLRD4,
	QUEST_KING6,
	QUEST_POISON6,
	QUEST_BONE4,
	QUEST_VILE8
  },
  {
	QUEST_INFRA10,
	QUEST_MUSH13,
	-1,
	-1,
	QUEST_VEIL8,
	-1,
	QUEST_BUTCH8,
	QUEST_BANNER9,
	QUEST_BLIND8,
	QUEST_BLOOD8,
	QUEST_ANVIL10,
	QUEST_WARLRD8,
	QUEST_KING10,
	QUEST_POISON10,
	QUEST_BONE8,
	QUEST_VILE12
  },
  {
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1
  },
  {
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1,
	QUEST_KING1
  }
};
int CowPlaying = -1;

int __fastcall GetActiveTowner(int t)
{
	int i; // eax

	i = 0;
	if ( numtowners <= 0 )
		return -1;

	while ( towner[i]._ttype != t )
	{
		++i;
		if ( i >= numtowners )
			return -1;
	}
	return i;
}

void __fastcall SetTownerGPtrs(void *pData, void **pAnim)
{
	void **v2; // esi
	signed int v3; // edx
	char *v4; // eax
	signed int v5; // [esp+Ch] [ebp-4h]

	v5 = 0;
	v2 = pAnim;
	do
	{
		v3 = v5;
		v4 = (char *)pData + *((_DWORD *)pData + v5++);
		v2[v3] = v4;
	}
	while ( v5 < 8 );
}

void __fastcall NewTownerAnim(int tnum, unsigned char *pAnim, int numFrames, int Delay)
{
	int v4; // ecx

	v4 = tnum;
	towner[v4]._tAnimCnt = 0;
	towner[v4]._tAnimLen = numFrames;
	towner[v4]._tAnimData = pAnim;
	towner[v4]._tAnimFrame = 1;
	towner[v4]._tAnimDelay = Delay;
}

void __fastcall InitTownerInfo(int i, int w, bool sel, int t, int x, int y, int ao, int tp)
{
	int v8; // ebx
	int v9; // esi
	int v10; // edi

	v8 = i;
	v9 = i;
	v10 = w;
	memset(&towner[i], 0, 0xE8u);
	towner[v9]._tSelFlag = sel;
	towner[v9]._ttype = t;
	towner[v9]._tx = x;
	towner[v9]._tMsgSaid = 0;
	towner[v9]._tAnimWidth = v10;
	towner[v9]._tAnimWidth2 = (v10 - 64) >> 1;
	towner[v9]._ty = y;
	dMonster[0][y + 112 * x] = v8 + 1;
	_LOBYTE(towner[v9]._tAnimOrder) = ao;
	towner[v9]._tTenPer = tp;
	towner[v9]._tSeed = GetRndSeed();
}

void __fastcall InitQstSnds(int i)
{
	int v1; // eax
	_BYTE *v2; // ecx
	unsigned char *v3; // esi
	QuestTalkData *v4; // eax
	bool v5; // zf

	v1 = i;
	if ( boyloadflag )
		v1 = i + 1;
	v2 = (unsigned char *)&towner[i].qsts[0]._qstmsgact;
	v3 = &quests[0]._qtype;
	v4 = &Qtalklist[v1];
	do
	{
		v5 = v4->_qinfra == -1;
		*(v2 - 2) = *v3;
		*(v2 - 1) = v4->_qinfra;
		*v2 = !v5;
		v3 += 24;
		v4 = (QuestTalkData *)((char *)v4 + 4);
		v2 += 3;
	}
	while ( (signed int)v3 < (signed int)&quests[16]._qtype );
}
// 69BE90: using guessed type int qline;
// 6AAC2C: using guessed type int boyloadflag;

void __cdecl InitSmith()
{
	int v0; // esi
	int v1; // esi
	_DWORD *v2; // eax
	signed int v3; // ecx
	int v4; // ecx

	InitTownerInfo(numtowners, 96, 1, 0, 62, 63, 0, 10);
	v0 = numtowners;
	InitQstSnds(numtowners);
	v1 = v0;
	towner[v1]._tNData = LoadFileInMem("Towners\\Smith\\SmithN.CEL", 0);
	v2 = (unsigned int *)towner[v1]._tNAnim;
	v3 = 8;
	do
	{
		*v2 = (unsigned int)towner[v1]._tNData;
		++v2;
		--v3;
	}
	while ( v3 );
	v4 = numtowners;
	towner[v1]._tNFrames = 16;
	NewTownerAnim(v4, towner[v1]._tNAnim[1], 16, 3);
	strcpy(towner[v1]._tName, "Griswold the Blacksmith");
	++numtowners;
}

void __cdecl InitBarOwner()
{
	int v0; // esi
	int v1; // esi
	_DWORD *v2; // eax
	signed int v3; // ecx
	int v4; // ecx

	bannerflag = 0; // unused
	InitTownerInfo(numtowners, 96, 1, 3, 55, 62, 3, 10);
	v0 = numtowners;
	InitQstSnds(numtowners);
	v1 = v0;
	towner[v1]._tNData = LoadFileInMem("Towners\\TwnF\\TwnFN.CEL", 0);
	v2 = (unsigned int *)towner[v1]._tNAnim;
	v3 = 8;
	do
	{
		*v2 = (unsigned int)towner[v1]._tNData;
		++v2;
		--v3;
	}
	while ( v3 );
	v4 = numtowners;
	towner[v1]._tNFrames = 16;
	NewTownerAnim(v4, towner[v1]._tNAnim[1], 16, 3);
	strcpy(towner[v1]._tName, "Ogden the Tavern owner");
	++numtowners;
}
// 6AAC28: using guessed type int bannerflag;

void __cdecl InitTownDead()
{
	int v0; // esi
	int v1; // esi
	_DWORD *v2; // eax
	signed int v3; // ecx
	int v4; // ecx

	InitTownerInfo(numtowners, 96, 1, 2, 24, 32, -1, 10);
	v0 = numtowners;
	InitQstSnds(numtowners);
	v1 = v0;
	towner[v1]._tNData = LoadFileInMem("Towners\\Butch\\Deadguy.CEL", 0);
	v2 = (unsigned int *)towner[v1]._tNAnim;
	v3 = 8;
	do
	{
		*v2 = (unsigned int)towner[v1]._tNData;
		++v2;
		--v3;
	}
	while ( v3 );
	v4 = numtowners;
	towner[v1]._tNFrames = 8;
	NewTownerAnim(v4, towner[v1]._tNAnim[4], 8, 6);
	strcpy(towner[v1]._tName, "Wounded Townsman");
	++numtowners;
}

void __cdecl InitWitch()
{
	int v0; // esi
	int v1; // esi
	_DWORD *v2; // ecx
	signed int v3; // edx

	InitTownerInfo(numtowners, 96, 1, 6, 80, 20, 5, 10);
	v0 = numtowners;
	InitQstSnds(numtowners);
	v1 = v0;
	towner[v1]._tNData = LoadFileInMem("Towners\\TownWmn1\\Witch.CEL", 0);
	v2 = (unsigned int *)towner[v1]._tNAnim;
	v3 = 8;
	do
	{
		*v2 = (unsigned int)towner[v1]._tNData;
		++v2;
		--v3;
	}
	while ( v3 );
	towner[v1]._tNFrames = 19;
	NewTownerAnim(numtowners, towner[v1]._tNAnim[0], 19, 6);
	strcpy(towner[v1]._tName, "Adria the Witch");
	++numtowners;
}

void __cdecl InitBarmaid()
{
	int v0; // esi
	int v1; // esi
	_DWORD *v2; // ecx
	signed int v3; // edx

	InitTownerInfo(numtowners, 96, 1, 7, 43, 66, -1, 10);
	v0 = numtowners;
	InitQstSnds(numtowners);
	v1 = v0;
	towner[v1]._tNData = LoadFileInMem("Towners\\TownWmn1\\WmnN.CEL", 0);
	v2 = (unsigned int *)towner[v1]._tNAnim;
	v3 = 8;
	do
	{
		*v2 = (unsigned int)towner[v1]._tNData;
		++v2;
		--v3;
	}
	while ( v3 );
	towner[v1]._tNFrames = 18;
	NewTownerAnim(numtowners, towner[v1]._tNAnim[0], 18, 6);
	strcpy(towner[v1]._tName, "Gillian the Barmaid");
	++numtowners;
}

void __cdecl InitBoy()
{
	int v0; // esi
	int v1; // esi
	_DWORD *v2; // ecx
	signed int v3; // edx

	boyloadflag = 1;
	InitTownerInfo(numtowners, 96, 1, 8, 11, 53, -1, 10);
	v0 = numtowners;
	InitQstSnds(numtowners);
	v1 = v0;
	towner[v1]._tNData = LoadFileInMem("Towners\\TownBoy\\PegKid1.CEL", 0);
	v2 = (unsigned int *)towner[v1]._tNAnim;
	v3 = 8;
	do
	{
		*v2 = (unsigned int)towner[v1]._tNData;
		++v2;
		--v3;
	}
	while ( v3 );
	towner[v1]._tNFrames = 20;
	NewTownerAnim(numtowners, towner[v1]._tNAnim[0], 20, 6);
	strcpy(towner[v1]._tName, "Wirt the Peg-legged boy");
	++numtowners;
}
// 6AAC2C: using guessed type int boyloadflag;

void __cdecl InitHealer()
{
	int v0; // esi
	int v1; // esi
	_DWORD *v2; // eax
	signed int v3; // ecx
	int v4; // ecx

	InitTownerInfo(numtowners, 96, 1, 1, 55, 79, 1, 10);
	v0 = numtowners;
	InitQstSnds(numtowners);
	v1 = v0;
	towner[v1]._tNData = LoadFileInMem("Towners\\Healer\\Healer.CEL", 0);
	v2 = (unsigned int *)towner[v1]._tNAnim;
	v3 = 8;
	do
	{
		*v2 = (unsigned int)towner[v1]._tNData;
		++v2;
		--v3;
	}
	while ( v3 );
	v4 = numtowners;
	towner[v1]._tNFrames = 20;
	NewTownerAnim(v4, towner[v1]._tNAnim[7], 20, 6);
	strcpy(towner[v1]._tName, "Pepin the Healer");
	++numtowners;
}

void __cdecl InitTeller()
{
	int v0; // esi
	int v1; // esi
	_DWORD *v2; // ecx
	signed int v3; // edx

	InitTownerInfo(numtowners, 96, 1, 4, 62, 71, 2, 10);
	v0 = numtowners;
	InitQstSnds(numtowners);
	v1 = v0;
	towner[v1]._tNData = LoadFileInMem("Towners\\Strytell\\Strytell.CEL", 0);
	v2 = (unsigned int *)towner[v1]._tNAnim;
	v3 = 8;
	do
	{
		*v2 = (unsigned int)towner[v1]._tNData;
		++v2;
		--v3;
	}
	while ( v3 );
	towner[v1]._tNFrames = 25;
	NewTownerAnim(numtowners, towner[v1]._tNAnim[0], 25, 3);
	strcpy(towner[v1]._tName, "Cain the Elder");
	++numtowners;
}

void __cdecl InitDrunk()
{
	int v0; // esi
	int v1; // esi
	_DWORD *v2; // ecx
	signed int v3; // edx

	InitTownerInfo(numtowners, 96, 1, 5, 71, 84, 4, 10);
	v0 = numtowners;
	InitQstSnds(numtowners);
	v1 = v0;
	towner[v1]._tNData = LoadFileInMem("Towners\\Drunk\\TwnDrunk.CEL", 0);
	v2 = (unsigned int *)towner[v1]._tNAnim;
	v3 = 8;
	do
	{
		*v2 = (unsigned int)towner[v1]._tNData;
		++v2;
		--v3;
	}
	while ( v3 );
	towner[v1]._tNFrames = 18;
	NewTownerAnim(numtowners, towner[v1]._tNAnim[0], 18, 3);
	strcpy(towner[v1]._tName, "Farnham the Drunk");
	++numtowners;
}

void __cdecl InitCows()
{
	unsigned char *v0; // eax
	int v1; // ecx
	signed int v2; // ebx
	int v3; // esi
	int v4; // ebp
	int v5; // eax
	void **v6; // ecx
	int v7; // edi
	int v9; // edx
	int v10; // eax
	int v11; // ecx
	_DWORD *v12; // esi
	int v13; // edx
	_DWORD *v14; // esi
	_DWORD *v15; // eax
	int v16; // [esp+10h] [ebp-4h]

	v0 = LoadFileInMem("Towners\\Animals\\Cow.CEL", 0);
	v1 = numtowners;
	pCowCels = v0;
	v2 = 0;
	do
	{
		v3 = TownCowX[v2];
		v4 = TownCowDir[v2];
		v16 = TownCowY[v2];
		InitTownerInfo(v1, 128, 0, 9, TownCowX[v2], v16, -1, 10);
		v5 = numtowners;
		v6 = (void **)&towner[numtowners]._tNData;
		*v6 = pCowCels;
		SetTownerGPtrs(*v6, (void **)towner[v5]._tNAnim);
		v7 = numtowners;
		towner[numtowners]._tNFrames = 12;
		NewTownerAnim(v7, towner[0]._tNAnim[v4 + 58 * v7], 12, 3);
		v7 *= 232;
		*(int *)((char *)&towner[0]._tAnimFrame + v7) = random(0, 11) + 1;
		*(int *)((char *)&towner[0]._tSelFlag + v7) = 1;
		strcpy(&towner[0]._tName[v7], "Cow");
		v9 = v3 + cowoffx[v4];
		v10 = v16 + cowoffy[v4];
		v11 = numtowners;
		v12 = (_DWORD *)((char *)dMonster + 4 * (v10 + 112 * v3));
		if ( !*v12 )
			*v12 = -1 - numtowners;
		v13 = 112 * v9;
		v14 = (_DWORD *)((char *)dMonster + 4 * (v13 + v16));
		if ( !*v14 )
			*v14 = -1 - v11;
		v15 = (_DWORD *)((char *)dMonster + 4 * (v10 + v13));
		if ( !*v15 )
			*v15 = -1 - v11;
		++v2;
		v1 = v11 + 1;
		numtowners = v1;
	}
	while ( v2 < 3 );
}
// 6AAC2C: using guessed type int boyloadflag;

void __cdecl InitTowners()
{
	numtowners = 0;
	boyloadflag = 0;
	InitSmith();
	InitHealer();
	if ( quests[QTYPE_BUTCH]._qactive && quests[QTYPE_BUTCH]._qactive != 3 )
		InitTownDead();
	InitBarOwner();
	InitTeller();
	InitDrunk();
	InitWitch();
	InitBarmaid();
	InitBoy();
	InitCows();
}
// 6AAC2C: using guessed type int boyloadflag;

void __cdecl FreeTownerGFX()
{
	void **v0; // esi
	void *v1; // ecx
	void *v2; // ecx

	v0 = (void **)&towner[0]._tNData;
	do
	{
		v1 = *v0;
		if ( *v0 == pCowCels )
		{
			*v0 = 0;
		}
		else if ( v1 )
		{
			*v0 = 0;
			mem_free_dbg(v1);
		}
		v0 += 58;
	}
	while ( (signed int)v0 < (signed int)&towner[16]._tNData );
	v2 = pCowCels;
	pCowCels = 0;
	mem_free_dbg(v2);
}
// 6ABB9C: using guessed type int dword_6ABB9C;

void __fastcall TownCtrlMsg(int i)
{
	int p; // edi
	int dx; // ebx
	int dy; // eax

	if ( towner[i]._tbtcnt )
	{
		p = towner[i]._tVar1;
		dx = abs(towner[i]._tx - plr[p].WorldX);
		dy = abs(towner[i]._ty - plr[p].WorldY);
		if ( dx >= 2 || dy >= 2 )
			towner[i]._tbtcnt = 0;
		if ( !towner[i]._tbtcnt )
		{
			qtextflag = 0;
			sfx_stop();
		}
	}
}
// 646D00: using guessed type char qtextflag;

void __cdecl TownBlackSmith()
{
	int v0; // eax

	v0 = GetActiveTowner(TOWN_SMITH);
	TownCtrlMsg(v0);
}

void __cdecl TownBarOwner()
{
	int v0; // eax

	v0 = GetActiveTowner(TOWN_TAVERN);
	TownCtrlMsg(v0);
}

void __cdecl TownDead()
{
	int v0; // esi
	int v1; // eax

	v0 = GetActiveTowner(TOWN_DEADGUY);
	TownCtrlMsg(v0);
	if ( qtextflag )
		goto LABEL_6;
	if ( (quests[6]._qactive != 2 || quests[6]._qlog) && quests[6]._qactive != 1 )
	{
		v1 = v0;
		towner[v1]._tAnimDelay = 1000;
		towner[v1]._tAnimFrame = 1;
		strcpy(towner[v0]._tName, "Slain Townsman");
LABEL_6:
		if ( quests[QTYPE_BUTCH]._qactive != 1 )
			towner[v0]._tAnimCnt = 0;
	}
}
// 646D00: using guessed type char qtextflag;

void __cdecl TownHealer()
{
	int v0; // eax

	v0 = GetActiveTowner(TOWN_HEALER);
	TownCtrlMsg(v0);
}

void __cdecl TownStory()
{
	int v0; // eax

	v0 = GetActiveTowner(TOWN_STORY);
	TownCtrlMsg(v0);
}

void __cdecl TownDrunk()
{
	int v0; // eax

	v0 = GetActiveTowner(TOWN_DRUNK);
	TownCtrlMsg(v0);
}

void __cdecl TownBoy()
{
	int v0; // eax

	v0 = GetActiveTowner(TOWN_PEGBOY);
	TownCtrlMsg(v0);
}

void __cdecl TownWitch()
{
	int v0; // eax

	v0 = GetActiveTowner(TOWN_WITCH);
	TownCtrlMsg(v0);
}

void __cdecl TownBarMaid()
{
	int v0; // eax

	v0 = GetActiveTowner(TOWN_BMAID);
	TownCtrlMsg(v0);
}

void __cdecl TownCow()
{
	int v0; // eax

	v0 = GetActiveTowner(TOWN_COW);
	TownCtrlMsg(v0);
}

void __cdecl ProcessTowners()
{
	int *v0; // esi
	char v1; // al
	int v2; // ecx
	_DWORD *v3; // eax

	v0 = &towner[0]._tAnimCnt;
	do
	{
		switch ( *(v0 - 10) )
		{
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
			default:
				break;
		}
		if ( ++*v0 >= *(v0 - 1) )
		{
			v1 = *((_BYTE *)v0 + 16);
			*v0 = 0;
			if ( v1 < 0 )
			{
				if ( ++v0[2] > v0[1] )
					v0[2] = 1;
			}
			else
			{
				v2 = 148 * v1;
				v3 = (unsigned int *)v0 + 3;
				++*v3;
				if ( AnimOrder[0][v0[3] + v2] == -1 )
					*v3 = 0;
				v0[2] = (char)AnimOrder[0][*v3 + v2];
			}
		}
		v0 += 58;
	}
	while ( (signed int)v0 < (signed int)&towner[16]._tAnimCnt );
}

ItemStruct *__fastcall PlrHasItem(int pnum, int item, int *i)
{
	unsigned int v3; // eax
	int v4; // ecx

	v3 = 21720 * pnum;
	*i = 0;
	if ( plr[pnum]._pNumInv <= 0 )
		return 0;
	while ( *(int *)((char *)&plr[0].InvList[*i].IDidx + v3) != item )
	{
		v4 = *i + 1;
		*i = v4;
		if ( v4 >= plr[v3 / 0x54D8]._pNumInv )
			return 0;
	}
	return (ItemStruct *)((char *)&plr[0].InvList[*i] + v3);
}

void __fastcall TownerTalk(int t)
{
	sgdwCowClicks = 0;
	sgnCowMsg = 0;
	storeflag = 1;
	InitQTextMsg(t);
}
// 6AAC18: using guessed type int storeflag;
// 6AAC1C: using guessed type int sgnCowMsg;
// 6AAC24: using guessed type int sgdwCowClicks;

void __fastcall TalkToTowner(int p, int t)
{
	int v2; // ebx
	int v3; // edi
	int v6; // ebp
	int v7; // esi
	int v8; // eax
	int v9; // ecx
	//char v10; // cl
	bool v11; // zf
	int v12; // edi
	//int v13; // eax
	//int v14; // eax
	//int v15; // eax
	//char v16; // cl
	_speech_id v17; // ecx
	ItemStruct *Item; // ebp
	unsigned char v19; // dl
	int inv_item_num; // [esp+10h] [ebp-8h]
	int v21; // [esp+14h] [ebp-4h]

	v2 = t;
	v3 = p;
	v21 = t;
	random(6, 3); /* figure out what these are for */
	random(6, 4);
	random(6, 5);
	v6 = v3;
	v7 = v2;
	inv_item_num = abs(plr[v3].WorldX - towner[v2]._tx);
	v8 = abs(plr[v3].WorldY - towner[v2]._ty);
	if ( inv_item_num >= 2 )
		return;
	if ( v8 >= 2 )
		return;
	if ( qtextflag )
		return;
	towner[v7]._tMsgSaid = 0;
	if ( pcurs >= CURSOR_FIRSTITEM && !DropItemBeforeTrig() )
		return;
	if ( v2 == GetActiveTowner(TOWN_TAVERN) )
	{
		if ( !plr[v6]._pLvlVisited[0] && !towner[v7]._tMsgSaid )
		{
			towner[v7]._tbtcnt = 150;
			towner[v7]._tVar1 = v3;
			InitQTextMsg(QUEST_INTRO);
			towner[v7]._tMsgSaid = 1;
		}
		if ( (plr[v6]._pLvlVisited[2] || plr[v6]._pLvlVisited[4]) && quests[QTYPE_KING]._qactive )
		{
			if ( !quests[QTYPE_KING]._qvar2 && !towner[v7]._tMsgSaid )
			{
				quests[QTYPE_KING]._qvar2 = 1;
				quests[QTYPE_KING]._qlog = 1;
				if ( quests[QTYPE_KING]._qactive == 1 )
				{
					quests[QTYPE_KING]._qactive = 2;
					quests[QTYPE_KING]._qvar1 = 1;
				}
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_KING2);
				towner[v7]._tMsgSaid = 1;
				NetSendCmdQuest(1u, 0xCu);
			}
			if ( quests[QTYPE_KING]._qactive == 3 && quests[QTYPE_KING]._qvar2 == 1 && !towner[v7]._tMsgSaid )
			{
				quests[QTYPE_KING]._qvar2 = 2;
				quests[QTYPE_KING]._qvar1 = 2;
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_KING4);
				towner[v7]._tMsgSaid = 1;
				NetSendCmdQuest(1u, 0xCu);
			}
		}
		if ( gbMaxPlayers == 1 && plr[v6]._pLvlVisited[3] && quests[QTYPE_BOL]._qactive )
		{
			if ( (quests[QTYPE_BOL]._qactive == 1 || quests[QTYPE_BOL]._qactive == 2) && !quests[QTYPE_BOL]._qvar2 )
			{
				if ( towner[v7]._tMsgSaid )
					goto LABEL_36;
				quests[QTYPE_BOL]._qvar2 = 1;
				if ( quests[QTYPE_BOL]._qactive == 1 )
				{
					quests[QTYPE_BOL]._qvar1 = 1;
					quests[QTYPE_BOL]._qactive = 2;
				}
				quests[QTYPE_BOL]._qlog = 1;
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_BANNER2);
				towner[v7]._tMsgSaid = 1;
			}
			if ( quests[QTYPE_BOL]._qvar2 == 1 && PlrHasItem(v3, IDI_BANNER, &inv_item_num) && !towner[v7]._tMsgSaid )
			{
				quests[QTYPE_BOL]._qactive = 3;
				quests[QTYPE_BOL]._qvar1 = 3;
				RemoveInvItem(v3, inv_item_num);
				CreateItem(UITEM_HARCREST, towner[v7]._tx, towner[v7]._ty + 1);
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_BANNER3);
				towner[v7]._tMsgSaid = 1;
			}
		}
LABEL_36:
		if ( !qtextflag )
		{
			TownerTalk(QUEST_OGDEN1);
			if ( storeflag )
			{
				_LOBYTE(v9) = STORE_TAVERN;
LABEL_39:
				StartStore(v9);
				return;
			}
		}
		return;
	}
	if ( v2 == GetActiveTowner(TOWN_DEADGUY) )
	{
		if ( quests[QTYPE_BUTCH]._qactive == 2 )
		{
			if ( quests[QTYPE_BUTCH]._qvar1 == 1 )
			{
				v11 = _LOBYTE(plr[v6]._pClass) == 0;
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				quests[QTYPE_BUTCH]._qvar1 = 1;
				if ( v11 && (v12 = PS_WARR8, !effect_is_playing(PS_WARR8))
				  || _LOBYTE(plr[v6]._pClass) == 1 && (v12 = PS_ROGUE8, !effect_is_playing(PS_ROGUE8))
				  || _LOBYTE(plr[v6]._pClass) == 2 && (v12 = PS_MAGE8, !effect_is_playing(PS_MAGE8)) )
				{
					PlaySFX(v12);
				}
LABEL_53:
				towner[v7]._tMsgSaid = 1;
				return;
			}
			if ( quests[QTYPE_BUTCH]._qvar1 )
				return;
		}
		else
		{
			if ( quests[QTYPE_BUTCH]._qactive == 3 )
			{
				if ( quests[QTYPE_BUTCH]._qvar1 != 1 )
					return;
				quests[QTYPE_BUTCH]._qvar1 = 1;
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				goto LABEL_53;
			}
			if ( quests[QTYPE_BUTCH]._qactive != 1 )
				return;
		}
		quests[QTYPE_BUTCH]._qactive = 2;
		quests[QTYPE_BUTCH]._qlog = 1;
		quests[QTYPE_BUTCH]._qmsg = QUEST_BUTCH9;
		quests[QTYPE_BUTCH]._qvar1 = 1;
		towner[v7]._tbtcnt = 50;
		towner[v7]._tVar1 = v3;
		towner[v7]._tVar2 = 3;
		InitQTextMsg(QUEST_BUTCH9);
		towner[v7]._tMsgSaid = 1;
		NetSendCmdQuest(1u, 6u);
		return;
	}
	if ( v2 != GetActiveTowner(0) )
	{
		if ( v2 == GetActiveTowner(TOWN_WITCH) )
		{
			if ( quests[QTYPE_BLKM]._qactive == 1 )
			{
				if ( PlrHasItem(v3, IDI_FUNGALTM, &inv_item_num) )
				{
					RemoveInvItem(v3, inv_item_num);
					quests[QTYPE_BLKM]._qactive = 2;
					quests[QTYPE_BLKM]._qlog = 1;
					quests[QTYPE_BLKM]._qvar1 = QS_TOMEGIVEN;
					v17 = QUEST_MUSH8;
LABEL_105:
					towner[v7]._tVar1 = v3;
					towner[v7]._tbtcnt = 150;
					InitQTextMsg(v17);
					towner[v7]._tMsgSaid = 1;
					goto LABEL_106;
				}
			}
			else if ( quests[QTYPE_BLKM]._qactive == 2 )
			{
				if ( quests[QTYPE_BLKM]._qvar1 >= QS_TOMEGIVEN && quests[QTYPE_BLKM]._qvar1 <= QS_MUSHPICKED )
				{
					if ( PlrHasItem(v3, IDI_MUSHROOM, &inv_item_num) )
					{
						RemoveInvItem(v3, inv_item_num);
						Qtalklist[TOWN_WITCH]._qblkm = -1;
						quests[QTYPE_BLKM]._qvar1 = QS_MUSHGIVEN;
						Qtalklist[TOWN_HEALER]._qblkm = QUEST_MUSH3;
						v17 = QUEST_MUSH10;
					}
					else
					{
						v17 = QUEST_MUSH9;
						if ( quests[QTYPE_BLKM]._qmsg == QUEST_MUSH9 )
							goto LABEL_106;
					}
					quests[QTYPE_BLKM]._qmsg = v17;
					goto LABEL_105;
				}
				Item = PlrHasItem(v3, IDI_SPECELIX, &inv_item_num);
				if ( Item )
				{
					towner[v7]._tbtcnt = 150;
					towner[v7]._tVar1 = v3;
					InitQTextMsg(QUEST_MUSH12);
					quests[QTYPE_BLKM]._qactive = 3;
					towner[v7]._tMsgSaid = 1;
					AllItemsList[Item->IDidx].iUsable = 1;
				}
				else if ( PlrHasItem(v3, IDI_BRAIN, &inv_item_num) )
				{
					v17 = QUEST_MUSH11;
					if ( quests[QTYPE_BLKM]._qvar2 != QUEST_MUSH11 )
					{
						quests[QTYPE_BLKM]._qvar2 = QUEST_MUSH11;
						goto LABEL_105;
					}
				}
			}
LABEL_106:
			if ( !qtextflag )
			{
				TownerTalk(QUEST_ADRIA1);
				if ( storeflag )
				{
					_LOBYTE(v9) = STORE_WITCH;
					goto LABEL_39;
				}
			}
			return;
		}
		if ( v2 == GetActiveTowner(TOWN_BMAID) )
		{
			if ( !qtextflag )
			{
				TownerTalk(QUEST_GILLIAN1);
				if ( storeflag )
				{
					_LOBYTE(v9) = STORE_BARMAID;
					goto LABEL_39;
				}
			}
			return;
		}
		if ( v2 == GetActiveTowner(TOWN_DRUNK) )
		{
			if ( !qtextflag )
			{
				TownerTalk(QUEST_FARNHAM1);
				if ( storeflag )
				{
					_LOBYTE(v9) = STORE_DRUNK;
					goto LABEL_39;
				}
			}
			return;
		}
		if ( v21 == GetActiveTowner(1) )
		{
			if ( gbMaxPlayers != 1 )
				goto LABEL_131;
			if ( plr[v6]._pLvlVisited[1] && !towner[v7]._tMsgSaid )
			{
				if ( quests[QTYPE_PW]._qactive == 1 )
				{
					quests[QTYPE_PW]._qactive = 2;
					quests[QTYPE_PW]._qlog = 1;
					quests[QTYPE_PW]._qmsg = QUEST_POISON3;
					quests[QTYPE_PW]._qvar1 = 1;
					towner[v7]._tbtcnt = 150;
					towner[v7]._tVar1 = v3;
					InitQTextMsg(QUEST_POISON3);
LABEL_126:
					towner[v7]._tMsgSaid = 1;
					goto LABEL_127;
				}
				if ( quests[QTYPE_PW]._qactive == 3 && quests[QTYPE_PW]._qvar1 != 2 )
				{
					quests[QTYPE_PW]._qvar1 = 2;
					towner[v7]._tbtcnt = 150;
					towner[v7]._tVar1 = v3;
					InitQTextMsg(QUEST_POISON5);
					CreateItem(UITEM_TRING, towner[v7]._tx, towner[v7]._ty + 1);
					goto LABEL_126;
				}
			}
LABEL_127:
			if ( quests[QTYPE_BLKM]._qactive == 2 && quests[QTYPE_BLKM]._qmsg == QUEST_MUSH10 && PlrHasItem(v3, IDI_BRAIN, &inv_item_num) )
			{
				RemoveInvItem(v3, inv_item_num);
				SpawnQuestItem(IDI_SPECELIX, towner[v7]._tx, towner[v7]._ty + 1, 0, 0);
				InitQTextMsg(QUEST_MUSH4);
				Qtalklist[TOWN_HEALER]._qblkm = -1;
				quests[QTYPE_BLKM]._qvar1 = QS_BRAINGIVEN;
			}
LABEL_131:
			if ( !qtextflag )
			{
				TownerTalk(QUEST_PEPIN1);
				if ( storeflag )
				{
					_LOBYTE(v9) = STORE_HEALER;
					goto LABEL_39;
				}
			}
			return;
		}
		if ( v21 == GetActiveTowner(TOWN_PEGBOY) )
		{
			if ( !qtextflag )
			{
				TownerTalk(QUEST_WIRT1);
				if ( storeflag )
				{
					_LOBYTE(v9) = STORE_BOY;
					goto LABEL_39;
				}
			}
			return;
		}
		if ( v21 != GetActiveTowner(TOWN_STORY) )
		{
			if ( towner[v7]._ttype == 9 && !qtextflag )
				CowSFX(v3);
			return;
		}
		if ( gbMaxPlayers == 1 )
		{
			if ( quests[QTYPE_VB]._qactive == 1 )
			{
				if ( !PlrHasItem(v3, IDI_LAZSTAFF, &inv_item_num) )
					goto LABEL_154;
				RemoveInvItem(v3, inv_item_num);
				quests[QTYPE_VB]._qvar1 = 2;
				towner[v7]._tbtcnt = QUEST_STORY1;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_VILE1);
				quests[QTYPE_VB]._qactive = 2;
				quests[QTYPE_VB]._qlog = 1;
			}
			else
			{
				if ( quests[QTYPE_VB]._qactive != 3 || quests[QTYPE_VB]._qvar1 != 7 )
					goto LABEL_154;
				quests[QTYPE_VB]._qvar1 = 8;
				towner[v7]._tbtcnt = QUEST_STORY1;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_VILE3);
				quests[QTYPE_VB]._qlog = 1;
			}
			v11 = gbMaxPlayers == 1;
			towner[v7]._tMsgSaid = 1;
			if ( v11 )
				goto LABEL_154;
		}
		if ( quests[QTYPE_VB]._qactive == 2 )
		{
			if ( !quests[QTYPE_VB]._qlog )
			{
				towner[v7]._tbtcnt = QUEST_STORY1;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_VILE1);
				towner[v7]._tMsgSaid = 1;
				quests[QTYPE_VB]._qlog = 1;
				v19 = 15;
LABEL_153:
				NetSendCmdQuest(1u, v19);
				goto LABEL_154;
			}
		}
		else if ( quests[QTYPE_VB]._qactive == 3 && quests[QTYPE_VB]._qvar1 == 7 )
		{
			quests[QTYPE_VB]._qvar1 = 8;
			towner[v7]._tbtcnt = QUEST_STORY1;
			towner[v7]._tVar1 = v3;
			InitQTextMsg(QUEST_VILE3);
			towner[v7]._tMsgSaid = 1;
			NetSendCmdQuest(1u, 0xFu);
			quests[QTYPE_VB]._qlog = 1;
			v19 = 5;
			goto LABEL_153;
		}
LABEL_154:
		if ( !qtextflag )
		{
			TownerTalk(QUEST_STORY1);
			if ( storeflag )
			{
				_LOBYTE(v9) = STORE_STORY;
				goto LABEL_39;
			}
		}
		return;
	}
	if ( gbMaxPlayers == 1 )
	{
		if ( plr[v6]._pLvlVisited[4] != 0 && quests[QTYPE_INFRA]._qactive )
		{
			if ( quests[QTYPE_INFRA]._qvar2 == 0 )
			{
				quests[QTYPE_INFRA]._qvar2 = 1;
				quests[QTYPE_INFRA]._qlog = 1;
				if ( quests[QTYPE_INFRA]._qactive == 1 )
				{
					quests[QTYPE_INFRA]._qactive = 2;
					quests[QTYPE_INFRA]._qvar1 = 1;
				}
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_INFRA5);
				towner[v7]._tMsgSaid = 1;
			}
			if ( quests[QTYPE_INFRA]._qvar2 == 1 && PlrHasItem(v3, IDI_ROCK, &inv_item_num) && !towner[v7]._tMsgSaid )
			{
				quests[QTYPE_INFRA]._qactive = 3;
				quests[QTYPE_INFRA]._qvar2 = 2;
				quests[QTYPE_INFRA]._qvar1 = 2;
				RemoveInvItem(v3, inv_item_num);
				CreateItem(UITEM_INFRARING, towner[v7]._tx, towner[v7]._ty + 1);
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_INFRA7);
				towner[v7]._tMsgSaid = 1;
			}
		}
		if ( plr[v6]._pLvlVisited[9] && quests[QTYPE_ANVIL]._qactive )
		{
			if ( (quests[QTYPE_ANVIL]._qactive == 1 || quests[QTYPE_ANVIL]._qactive == 2) && !quests[QTYPE_ANVIL]._qvar2 )
			{
				if ( towner[v7]._tMsgSaid || quests[QTYPE_INFRA]._qvar2 != 2 && (quests[QTYPE_INFRA]._qactive != 2 || quests[QTYPE_INFRA]._qvar2 != 1) )
					goto LABEL_86;
				quests[QTYPE_ANVIL]._qvar2 = 1;
				quests[QTYPE_ANVIL]._qlog = 1;
				if ( quests[QTYPE_ANVIL]._qactive == 1 )
				{
					quests[QTYPE_ANVIL]._qactive = 2;
					quests[QTYPE_ANVIL]._qvar1 = 1;
				}
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_ANVIL5);
				towner[v7]._tMsgSaid = 1;
			}
			if ( quests[QTYPE_ANVIL]._qvar2 == 1 && PlrHasItem(v3, IDI_ANVIL, &inv_item_num) && !towner[v7]._tMsgSaid )
			{
				quests[QTYPE_ANVIL]._qactive = 3;
				quests[QTYPE_ANVIL]._qvar2 = 2;
				quests[QTYPE_ANVIL]._qvar1 = 2;
				RemoveInvItem(v3, inv_item_num);
				CreateItem(UITEM_GRISWOLD, towner[v7]._tx, towner[v7]._ty + 1);
				towner[v7]._tbtcnt = 150;
				towner[v7]._tVar1 = v3;
				InitQTextMsg(QUEST_ANVIL7);
				towner[v7]._tMsgSaid = 1;
			}
		}
	}
LABEL_86:
	if ( !qtextflag )
	{
		TownerTalk(QUEST_GRISWOLD1);
		if ( storeflag )
		{
			_LOBYTE(v9) = 1;
			goto LABEL_39;
		}
	}
}
// 646D00: using guessed type char qtextflag;
// 679660: using guessed type char gbMaxPlayers;
// 6AAC18: using guessed type int storeflag;

void __fastcall CowSFX(int pnum)
{
	if ( CowPlaying == -1 || !effect_is_playing(CowPlaying) )
	{
		if ( sgdwCowClicks++ < 8 )
		{
			CowPlaying = (sgdwCowClicks == 4) + TSFX_COW1;
		}
		else
		{
			PlaySfxLoc(TSFX_COW1, plr[pnum].WorldX, plr[pnum].WorldY + 5);
			sgdwCowClicks = 4;
			CowPlaying = snSFX[sgnCowMsg][plr[pnum]._pClass]; /* snSFX is local */
			if ( sgnCowMsg++ >= 2 )
				sgnCowMsg = 0;
		}
		PlaySfxLoc(CowPlaying, plr[pnum].WorldX, plr[pnum].WorldY);
	}
}
// 6AAC1C: using guessed type int sgnCowMsg;
// 6AAC24: using guessed type int sgdwCowClicks;
