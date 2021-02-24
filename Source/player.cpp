/**
 * @file player.cpp
 *
 * Implementation of player functionality, leveling, actions, creation, loading, etc.
 */
#include <algorithm>

#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

int plr_lframe_size;
int plr_wframe_size;
BYTE plr_gfx_flag = 0;
int plr_aframe_size;
int myplr;
PlayerStruct plr[MAX_PLRS];
int plr_fframe_size;
int plr_qframe_size;
BOOL deathflag;
int plr_hframe_size;
int plr_bframe_size;
BYTE plr_gfx_bflag = 0;
int plr_sframe_size;
int deathdelay;
int plr_dframe_size;

/** Maps from armor animation to letter used in graphic files. */
const char ArmourChar[4] = { 'L', 'M', 'H', 0 };
/** Maps from weapon animation to letter used in graphic files. */
const char WepChar[10] = { 'N', 'U', 'S', 'D', 'B', 'A', 'M', 'H', 'T', 0 };
/** Maps from player class to letter used in graphic files. */
const char CharChar[] = {
	'W',
	'R',
	'S',
	'M',
	'R',
	'W',
	0
};
const char CharCharHF[] = {
	'W',
	'R',
	'S',
	'M',
	'B',
	'C',
	0
};

/* data */

/** Specifies the X-coordinate delta from the player start location in Tristram. */
int plrxoff[9] = { 0, 2, 0, 2, 1, 0, 1, 2, 1 };
/** Specifies the Y-coordinate delta from the player start location in Tristram. */
int plryoff[9] = { 0, 2, 2, 0, 1, 1, 0, 1, 2 };
/** Specifies the X-coordinate delta from a player, used for instanced when casting resurrect. */
int plrxoff2[9] = { 0, 1, 0, 1, 2, 0, 1, 2, 2 };
/** Specifies the Y-coordinate delta from a player, used for instanced when casting resurrect. */
int plryoff2[9] = { 0, 0, 1, 1, 0, 2, 2, 1, 2 };
/** Specifies the frame of each animation for which an action is triggered, for each player class. */
char PlrGFXAnimLens[NUM_CLASSES][11] = {
	{ 10, 16, 8, 2, 20, 20, 6, 20, 8, 9, 14 },
	{ 8, 18, 8, 4, 20, 16, 7, 20, 8, 10, 12 },
	{ 8, 16, 8, 6, 20, 12, 8, 20, 8, 12, 8 },
	{ 8, 16, 8, 3, 20, 18, 6, 20, 8, 12, 13 },
	{ 8, 18, 8, 4, 20, 16, 7, 20, 8, 10, 12 },
	{ 10, 16, 8, 2, 20, 20, 6, 20, 8, 9, 14 },
};
/** Maps from player class to player velocity. */
int PWVel[NUM_CLASSES][3] = {
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
};
/** Total number of frames in walk animation. */
int AnimLenFromClass[NUM_CLASSES] = {
	8,
	8,
	8,
	8,
	8,
	8,
};
/** Maps from player_class to starting stat in strength. */
int StrengthTbl[NUM_CLASSES] = {
	30,
	20,
	15,
	25,
	20,
	40,
};
/** Maps from player_class to starting stat in magic. */
int MagicTbl[NUM_CLASSES] = {
	// clang-format off
	10,
	15,
	35,
	15,
	20,
	 0,
	// clang-format on
};
/** Maps from player_class to starting stat in dexterity. */
int DexterityTbl[NUM_CLASSES] = {
	20,
	30,
	15,
	25,
	25,
	20,
};
/** Maps from player_class to starting stat in vitality. */
int VitalityTbl[NUM_CLASSES] = {
	25,
	20,
	20,
	20,
	20,
	25,
};
/** Specifies the chance to block bonus of each player class.*/
int ToBlkTbl[NUM_CLASSES] = {
	30,
	20,
	10,
	25,
	25,
	30,
};
const char *const ClassStrTblOld[] = {
	"Warrior",
	"Rogue",
	"Sorceror",
	"Monk",
	"Bard",
	"Barbarian",
};
/** Maps from player_class to maximum stats. */
int MaxStats[NUM_CLASSES][4] = {
	// clang-format off
	{ 250,  50,  60, 100 },
	{  55,  70, 250,  80 },
	{  45, 250,  85,  80 },
	{ 150,  80, 150,  80 },
	{ 120, 120, 120, 100 },
	{ 255,   0,  55, 150 },
	// clang-format on
};
/** Specifies the experience point limit of each level. */
int ExpLvlsTbl[MAXCHARLEVEL] = {
	0,
	2000,
	4620,
	8040,
	12489,
	18258,
	25712,
	35309,
	47622,
	63364,
	83419,
	108879,
	141086,
	181683,
	231075,
	313656,
	424067,
	571190,
	766569,
	1025154,
	1366227,
	1814568,
	2401895,
	3168651,
	4166200,
	5459523,
	7130496,
	9281874,
	12042092,
	15571031,
	20066900,
	25774405,
	32994399,
	42095202,
	53525811,
	67831218,
	85670061,
	107834823,
	135274799,
	169122009,
	210720231,
	261657253,
	323800420,
	399335440,
	490808349,
	601170414,
	733825617,
	892680222,
	1082908612,
	1310707109,
	1583495809
};
const char *const ClassStrTbl[] = {
	"Warrior",
	"Rogue",
	"Sorceror",
	"Monk",
	"Rogue",
	"Warrior",
};

void SetPlayerGPtrs(BYTE *pData, BYTE **pAnim)
{
	int i;

	for (i = 0; i < 8; i++) {
		pAnim[i] = CelGetFrameStart(pData, i);
	}
}

void LoadPlrGFX(int pnum, player_graphic gfxflag)
{
	char prefix[16];
	char pszName[256];
	const char *szCel;
	PlayerStruct *p;
	const char *cs;
	BYTE *pData, *pAnim;
	DWORD i;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("LoadPlrGFX: illegal player %d", pnum);
	}

	p = &plr[pnum];
	if ((p->_pClass != PC_BARD || hfbard_mpq == NULL) && (p->_pClass != PC_BARBARIAN || hfbarb_mpq == NULL)) {
		sprintf(prefix, "%c%c%c", CharChar[p->_pClass], ArmourChar[p->_pgfxnum >> 4], WepChar[p->_pgfxnum & 0xF]);
		cs = ClassStrTbl[p->_pClass];
	} else {
		sprintf(prefix, "%c%c%c", CharCharHF[p->_pClass], ArmourChar[p->_pgfxnum >> 4], WepChar[p->_pgfxnum & 0xF]);
		cs = ClassStrTblOld[p->_pClass];
	}

	for (i = 1; i <= PFILE_NONDEATH; i <<= 1) {
		if (!(i & gfxflag)) {
			continue;
		}

		switch (i) {
		case PFILE_STAND:
			szCel = "AS";
			if (leveltype == DTYPE_TOWN) {
				szCel = "ST";
			}
			pData = p->_pNData;
			pAnim = (BYTE *)p->_pNAnim;
			break;
		case PFILE_WALK:
			szCel = "AW";
			if (leveltype == DTYPE_TOWN) {
				szCel = "WL";
			}
			pData = p->_pWData;
			pAnim = (BYTE *)p->_pWAnim;
			break;
		case PFILE_ATTACK:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "AT";
			pData = p->_pAData;
			pAnim = (BYTE *)p->_pAAnim;
			break;
		case PFILE_HIT:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "HT";
			pData = p->_pHData;
			pAnim = (BYTE *)p->_pHAnim;
			break;
		case PFILE_LIGHTNING:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "LM";
			pData = p->_pLData;
			pAnim = (BYTE *)p->_pLAnim;
			break;
		case PFILE_FIRE:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "FM";
			pData = p->_pFData;
			pAnim = (BYTE *)p->_pFAnim;
			break;
		case PFILE_MAGIC:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "QM";
			pData = p->_pTData;
			pAnim = (BYTE *)p->_pTAnim;
			break;
		case PFILE_DEATH:
			if (p->_pgfxnum & 0xF) {
				continue;
			}
			szCel = "DT";
			pData = p->_pDData;
			pAnim = (BYTE *)p->_pDAnim;
			break;
		case PFILE_BLOCK:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			if (!p->_pBlockFlag) {
				continue;
			}

			szCel = "BL";
			pData = p->_pBData;
			pAnim = (BYTE *)p->_pBAnim;
			break;
		default:
			app_fatal("PLR:2");
			break;
		}

		sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", cs, prefix, prefix, szCel);
		LoadFileWithMem(pszName, pData);
		SetPlayerGPtrs((BYTE *)pData, (BYTE **)pAnim);
		p->_pGFXLoad |= i;
	}
}

void InitPlayerGFX(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitPlayerGFX: illegal player %d", pnum);
	}

	if (plr[pnum]._pHitPoints >> 6 == 0) {
		plr[pnum]._pgfxnum = 0;
		LoadPlrGFX(pnum, PFILE_DEATH);
	} else {
		LoadPlrGFX(pnum, PFILE_NONDEATH);
	}
}

static DWORD GetPlrGFXSize(const char *szCel)
{
	DWORD c;
	const char *a, *w;
	DWORD dwSize, dwMaxSize;
	HANDLE hsFile;
	char pszName[256];
	char Type[16];

	dwMaxSize = 0;

	int classesToLoad = gbIsHellfire ? NUM_CLASSES : 3;

	for (c = 0; c < classesToLoad; c++) {
		if (gbIsSpawn && c != 0)
			continue;
		for (a = &ArmourChar[0]; *a; a++) {
			if (gbIsSpawn && a != &ArmourChar[0])
				break;
			for (w = &WepChar[0]; *w; w++) { // BUGFIX loads non-existing animagions; DT is only for N, BT is only for U, D & H (fixed)
				if (szCel[0] == 'D' && szCel[1] == 'T' && *w != 'N') {
					continue; //Death has no weapon
				}
				if (szCel[0] == 'B' && szCel[1] == 'L' && (*w != 'U' && *w != 'D' && *w != 'H')) {
					continue; //No block without weapon
				}
				if ((c == PC_BARD && hfbard_mpq == NULL) || (c == PC_BARBARIAN && hfbarb_mpq == NULL)) {
					sprintf(Type, "%c%c%c", CharChar[c], *a, *w);
					sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", ClassStrTbl[c], Type, Type, szCel);
				} else {
					sprintf(Type, "%c%c%c", CharCharHF[c], *a, *w);
					sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", ClassStrTblOld[c], Type, Type, szCel);
				}
				if (SFileOpenFile(pszName, &hsFile)) {
					/// ASSERT: assert(hsFile);
					dwSize = SFileGetFileSize(hsFile, NULL);
					SFileCloseFile(hsFile);
					if (dwMaxSize <= dwSize) {
						dwMaxSize = dwSize;
					}
				}
			}
		}
	}

	return dwMaxSize;
}

void InitPlrGFXMem(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitPlrGFXMem: illegal player %d", pnum);
	}

	if (!(plr_gfx_flag & 0x1)) { //STAND
		plr_gfx_flag |= 0x1;
		// ST: TOWN, AS: DUNGEON
		plr_sframe_size = std::max(GetPlrGFXSize("ST"), GetPlrGFXSize("AS"));
	}
	plr[pnum]._pNData = DiabloAllocPtr(plr_sframe_size);

	if (!(plr_gfx_flag & 0x2)) { //WALK
		plr_gfx_flag |= 0x2;
		// WL: TOWN, AW: DUNGEON
		plr_wframe_size = std::max(GetPlrGFXSize("WL"), GetPlrGFXSize("AW"));
	}
	plr[pnum]._pWData = DiabloAllocPtr(plr_wframe_size);

	if (!(plr_gfx_flag & 0x4)) { //ATTACK
		plr_gfx_flag |= 0x4;
		plr_aframe_size = GetPlrGFXSize("AT");
	}
	plr[pnum]._pAData = DiabloAllocPtr(plr_aframe_size);

	if (!(plr_gfx_flag & 0x8)) { //HIT
		plr_gfx_flag |= 0x8;
		plr_hframe_size = GetPlrGFXSize("HT");
	}
	plr[pnum]._pHData = DiabloAllocPtr(plr_hframe_size);

	if (!(plr_gfx_flag & 0x10)) { //LIGHTNING
		plr_gfx_flag |= 0x10;
		plr_lframe_size = GetPlrGFXSize("LM");
	}
	plr[pnum]._pLData = DiabloAllocPtr(plr_lframe_size);

	if (!(plr_gfx_flag & 0x20)) { //FIRE
		plr_gfx_flag |= 0x20;
		plr_fframe_size = GetPlrGFXSize("FM");
	}
	plr[pnum]._pFData = DiabloAllocPtr(plr_fframe_size);

	if (!(plr_gfx_flag & 0x40)) { //MAGIC
		plr_gfx_flag |= 0x40;
		plr_qframe_size = GetPlrGFXSize("QM");
	}
	plr[pnum]._pTData = DiabloAllocPtr(plr_qframe_size);

	if (!(plr_gfx_flag & 0x80)) { //DEATH
		plr_gfx_flag |= 0x80;
		plr_dframe_size = GetPlrGFXSize("DT");
	}
	plr[pnum]._pDData = DiabloAllocPtr(plr_dframe_size);

	if (!(plr_gfx_bflag & 0x1)) { //BLOCK
		plr_gfx_bflag |= 0x1;
		plr_bframe_size = GetPlrGFXSize("BL");
	}
	plr[pnum]._pBData = DiabloAllocPtr(plr_bframe_size);

	plr[pnum]._pGFXLoad = 0;
}

void FreePlayerGFX(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("FreePlayerGFX: illegal player %d", pnum);
	}

	MemFreeDbg(plr[pnum]._pNData);
	MemFreeDbg(plr[pnum]._pWData);
	MemFreeDbg(plr[pnum]._pAData);
	MemFreeDbg(plr[pnum]._pHData);
	MemFreeDbg(plr[pnum]._pLData);
	MemFreeDbg(plr[pnum]._pFData);
	MemFreeDbg(plr[pnum]._pTData);
	MemFreeDbg(plr[pnum]._pDData);
	MemFreeDbg(plr[pnum]._pBData);
	plr[pnum]._pGFXLoad = 0;
}

void NewPlrAnim(int pnum, BYTE *Peq, int numFrames, int Delay, int width)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("NewPlrAnim: illegal player %d", pnum);
	}

	plr[pnum]._pAnimData = Peq;
	plr[pnum]._pAnimLen = numFrames;
	plr[pnum]._pAnimFrame = 1;
	plr[pnum]._pAnimCnt = 0;
	plr[pnum]._pAnimDelay = Delay;
	plr[pnum]._pAnimWidth = width;
	plr[pnum]._pAnimWidth2 = (width - 64) >> 1;
}

void ClearPlrPVars(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ClearPlrPVars: illegal player %d", pnum);
	}

	plr[pnum]._pVar1 = 0;
	plr[pnum]._pVar2 = 0;
	plr[pnum]._pVar3 = 0;
	plr[pnum]._pVar4 = 0;
	plr[pnum]._pVar5 = 0;
	plr[pnum]._pVar6 = 0;
	plr[pnum]._pVar7 = 0;
	plr[pnum]._pVar8 = 0;
}

void SetPlrAnims(int pnum)
{
	int pc, gn;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SetPlrAnims: illegal player %d", pnum);
	}

	plr[pnum]._pNWidth = 96;
	plr[pnum]._pWWidth = 96;
	plr[pnum]._pAWidth = 128;
	plr[pnum]._pHWidth = 96;
	plr[pnum]._pSWidth = 96;
	plr[pnum]._pDWidth = 128;
	plr[pnum]._pBWidth = 96;

	pc = plr[pnum]._pClass;

	if (leveltype == DTYPE_TOWN) {
		plr[pnum]._pNFrames = PlrGFXAnimLens[pc][7];
		plr[pnum]._pWFrames = PlrGFXAnimLens[pc][8];
		plr[pnum]._pDFrames = PlrGFXAnimLens[pc][4];
		plr[pnum]._pSFrames = PlrGFXAnimLens[pc][5];
	} else {
		plr[pnum]._pNFrames = PlrGFXAnimLens[pc][0];
		plr[pnum]._pWFrames = PlrGFXAnimLens[pc][2];
		plr[pnum]._pAFrames = PlrGFXAnimLens[pc][1];
		plr[pnum]._pHFrames = PlrGFXAnimLens[pc][6];
		plr[pnum]._pSFrames = PlrGFXAnimLens[pc][5];
		plr[pnum]._pDFrames = PlrGFXAnimLens[pc][4];
		plr[pnum]._pBFrames = PlrGFXAnimLens[pc][3];
		plr[pnum]._pAFNum = PlrGFXAnimLens[pc][9];
	}
	plr[pnum]._pSFNum = PlrGFXAnimLens[pc][10];

	gn = plr[pnum]._pgfxnum & 0xF;
	if (pc == PC_WARRIOR) {
		if (gn == ANIM_ID_BOW) {
			if (leveltype != DTYPE_TOWN) {
				plr[pnum]._pNFrames = 8;
			}
			plr[pnum]._pAWidth = 96;
			plr[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_AXE) {
			plr[pnum]._pAFrames = 20;
			plr[pnum]._pAFNum = 10;
		} else if (gn == ANIM_ID_STAFF) {
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		}
	} else if (pc == PC_ROGUE) {
		if (gn == ANIM_ID_AXE) {
			plr[pnum]._pAFrames = 22;
			plr[pnum]._pAFNum = 13;
		} else if (gn == ANIM_ID_BOW) {
			plr[pnum]._pAFrames = 12;
			plr[pnum]._pAFNum = 7;
		} else if (gn == ANIM_ID_STAFF) {
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		}
	} else if (pc == PC_SORCERER) {
		plr[pnum]._pSWidth = 128;
		if (gn == ANIM_ID_UNARMED) {
			plr[pnum]._pAFrames = 20;
		} else if (gn == ANIM_ID_UNARMED_SHIELD) {
			plr[pnum]._pAFNum = 9;
		} else if (gn == ANIM_ID_BOW) {
			plr[pnum]._pAFrames = 20;
			plr[pnum]._pAFNum = 16;
		} else if (gn == ANIM_ID_AXE) {
			plr[pnum]._pAFrames = 24;
			plr[pnum]._pAFNum = 16;
		}
	} else if (pc == PC_MONK) {
		plr[pnum]._pNWidth = 112;
		plr[pnum]._pWWidth = 112;
		plr[pnum]._pAWidth = 130;
		plr[pnum]._pHWidth = 98;
		plr[pnum]._pSWidth = 114;
		plr[pnum]._pDWidth = 160;
		plr[pnum]._pBWidth = 98;

		switch (gn) {
		case ANIM_ID_UNARMED:
		case ANIM_ID_UNARMED_SHIELD:
			plr[pnum]._pAFrames = 12;
			plr[pnum]._pAFNum = 7;
			break;
		case ANIM_ID_BOW:
			plr[pnum]._pAFrames = 20;
			plr[pnum]._pAFNum = 14;
			break;
		case ANIM_ID_AXE:
			plr[pnum]._pAFrames = 23;
			plr[pnum]._pAFNum = 14;
			break;
		case ANIM_ID_STAFF:
			plr[pnum]._pAFrames = 13;
			plr[pnum]._pAFNum = 8;
			break;
		}
	} else if (pc == PC_BARD) {
		if (gn == ANIM_ID_AXE) {
			plr[pnum]._pAFrames = 22;
			plr[pnum]._pAFNum = 13;
		} else if (gn == ANIM_ID_BOW) {
			plr[pnum]._pAFrames = 12;
			plr[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_STAFF) {
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_SWORD_SHIELD || gn == ANIM_ID_SWORD) {
			plr[pnum]._pAFrames = 10;
		}
	} else if (pc == PC_BARBARIAN) {
		if (gn == ANIM_ID_AXE) {
			plr[pnum]._pAFrames = 20;
			plr[pnum]._pAFNum = 8;
		} else if (gn == ANIM_ID_BOW) {
			if (leveltype != DTYPE_TOWN) {
				plr[pnum]._pNFrames = 8;
			}
			plr[pnum]._pAWidth = 96;
			plr[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_STAFF) {
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_MACE || gn == ANIM_ID_MACE_SHIELD) {
			plr[pnum]._pAFNum = 8;
		}
	}
}

void ClearPlrRVars(PlayerStruct *p)
{
	// TODO: Missing debug assert p != NULL
	p->bReserved[0] = 0;
	p->bReserved[1] = 0;

	p->wReserved[0] = 0;
	p->wReserved[1] = 0;
	p->wReserved[2] = 0;
	p->wReserved[3] = 0;
	p->wReserved[4] = 0;
	p->wReserved[5] = 0;
	p->wReserved[6] = 0;

	p->dwReserved[0] = 0;
	p->dwReserved[1] = 0;
	p->dwReserved[2] = 0;
	p->dwReserved[3] = 0;
	p->dwReserved[4] = 0;
}

/**
 * @param c plr_classes value
 */
void CreatePlayer(int pnum, char c)
{
	char val;
	int hp, mana;
	int i;

	memset(&plr[pnum], 0, sizeof(PlayerStruct));
	ClearPlrRVars(&plr[pnum]);
	SetRndSeed(SDL_GetTicks());

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("CreatePlayer: illegal player %d", pnum);
	}
	plr[pnum]._pClass = c;

	val = StrengthTbl[c];
	plr[pnum]._pStrength = val;
	plr[pnum]._pBaseStr = val;

	val = MagicTbl[c];
	plr[pnum]._pMagic = val;
	plr[pnum]._pBaseMag = val;

	val = DexterityTbl[c];
	plr[pnum]._pDexterity = val;
	plr[pnum]._pBaseDex = val;

	val = VitalityTbl[c];
	plr[pnum]._pVitality = val;
	plr[pnum]._pBaseVit = val;

	plr[pnum]._pStatPts = 0;
	plr[pnum].pTownWarps = 0;
	plr[pnum].pDungMsgs = 0;
	plr[pnum].pDungMsgs2 = 0;
	plr[pnum].pLvlLoad = 0;
	plr[pnum].pDiabloKillLevel = 0;
	plr[pnum].pDifficulty = DIFF_NORMAL;

	if (plr[pnum]._pClass == PC_MONK) {
		plr[pnum]._pDamageMod = (plr[pnum]._pStrength + plr[pnum]._pDexterity) * plr[pnum]._pLevel / 150;
	} else if (plr[pnum]._pClass == PC_ROGUE || plr[pnum]._pClass == PC_BARD) {
		plr[pnum]._pDamageMod = plr[pnum]._pLevel * (plr[pnum]._pStrength + plr[pnum]._pDexterity) / 200;
	} else {
		plr[pnum]._pDamageMod = plr[pnum]._pStrength * plr[pnum]._pLevel / 100;
	}

	plr[pnum]._pBaseToBlk = ToBlkTbl[c];

	plr[pnum]._pHitPoints = (plr[pnum]._pVitality + 10) << 6;
	if (plr[pnum]._pClass == PC_WARRIOR || plr[pnum]._pClass == PC_BARBARIAN) {
		plr[pnum]._pHitPoints <<= 1;
	} else if (plr[pnum]._pClass == PC_ROGUE || plr[pnum]._pClass == PC_MONK || plr[pnum]._pClass == PC_BARD) {
		plr[pnum]._pHitPoints += plr[pnum]._pHitPoints >> 1;
	}

	plr[pnum]._pMaxHP = plr[pnum]._pHitPoints;
	plr[pnum]._pHPBase = plr[pnum]._pHitPoints;
	plr[pnum]._pMaxHPBase = plr[pnum]._pHitPoints;

	plr[pnum]._pMana = plr[pnum]._pMagic << 6;
	if (plr[pnum]._pClass == PC_SORCERER) {
		plr[pnum]._pMana <<= 1;
	} else if (plr[pnum]._pClass == PC_BARD) {
		plr[pnum]._pMana += plr[pnum]._pMana * 3 / 4;
	} else if (plr[pnum]._pClass == PC_ROGUE || plr[pnum]._pClass == PC_MONK) {
		plr[pnum]._pMana += plr[pnum]._pMana >> 1;
	}

	plr[pnum]._pMaxMana = plr[pnum]._pMana;
	plr[pnum]._pManaBase = plr[pnum]._pMana;
	plr[pnum]._pMaxManaBase = plr[pnum]._pMana;

	plr[pnum]._pLevel = 1;
	plr[pnum]._pMaxLvl = plr[pnum]._pLevel;
	plr[pnum]._pExperience = 0;
	plr[pnum]._pMaxExp = plr[pnum]._pExperience;
	plr[pnum]._pNextExper = ExpLvlsTbl[1];
	plr[pnum]._pArmorClass = 0;
	if (plr[pnum]._pClass == PC_BARBARIAN) {
		plr[pnum]._pMagResist = 1;
		plr[pnum]._pFireResist = 1;
		plr[pnum]._pLghtResist = 1;
	} else {
		plr[pnum]._pMagResist = 0;
		plr[pnum]._pFireResist = 0;
		plr[pnum]._pLghtResist = 0;
	}
	plr[pnum]._pLightRad = 10;
	plr[pnum]._pInfraFlag = FALSE;

	if (c == PC_WARRIOR) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_REPAIR);
	} else if (c == PC_ROGUE) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_DISARM);
	} else if (c == PC_SORCERER) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_RECHARGE);
	} else if (c == PC_MONK) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_SEARCH);
	} else if (c == PC_BARD) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_IDENTIFY);
	} else if (c == PC_BARBARIAN) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_BLODBOIL);
	}

	if (c == PC_SORCERER) {
		plr[pnum]._pMemSpells = SPELLBIT(SPL_FIREBOLT);
	} else {
		plr[pnum]._pMemSpells = 0;
	}

	for (i = 0; i < sizeof(plr[pnum]._pSplLvl) / sizeof(plr[pnum]._pSplLvl[0]); i++) {
		plr[pnum]._pSplLvl[i] = 0;
	}

	plr[pnum]._pSpellFlags = 0;

	if (plr[pnum]._pClass == PC_SORCERER) {
		plr[pnum]._pSplLvl[SPL_FIREBOLT] = 2;
	}

	// interestingly, only the first three hotkeys are reset
	// TODO: BUGFIX: clear all 4 hotkeys instead of 3 (demo leftover)
	for (i = 0; i < 3; i++) {
		plr[pnum]._pSplHotKey[i] = -1;
	}

	if (c == PC_WARRIOR) {
		plr[pnum]._pgfxnum = ANIM_ID_SWORD_SHIELD;
	} else if (c == PC_ROGUE) {
		plr[pnum]._pgfxnum = ANIM_ID_BOW;
	} else if (c == PC_SORCERER) {
		plr[pnum]._pgfxnum = ANIM_ID_STAFF;
	} else if (c == PC_MONK) {
		plr[pnum]._pgfxnum = ANIM_ID_STAFF;
	} else if (c == PC_BARD) {
		plr[pnum]._pgfxnum = ANIM_ID_SWORD_SHIELD;
	} else if (c == PC_BARBARIAN) {
		plr[pnum]._pgfxnum = ANIM_ID_SWORD_SHIELD;
	}

	for (i = 0; i < NUMLEVELS; i++) {
		plr[pnum]._pLvlVisited[i] = FALSE;
	}

	for (i = 0; i < 10; i++) {
		plr[pnum]._pSLvlVisited[i] = FALSE;
	}

	plr[pnum]._pLvlChanging = FALSE;
	plr[pnum].pTownWarps = 0;
	plr[pnum].pLvlLoad = 0;
	plr[pnum].pBattleNet = FALSE;
	plr[pnum].pManaShield = FALSE;
	plr[pnum].pDamAcFlags = 0;
	plr[pnum].wReflections = 0;

	InitDungMsgs(pnum);
	CreatePlrItems(pnum);
	SetRndSeed(0);
}

int CalcStatDiff(int pnum)
{
	int c;

	c = plr[pnum]._pClass;
	return MaxStats[c][ATTRIB_STR]
	    - plr[pnum]._pBaseStr
	    + MaxStats[c][ATTRIB_MAG]
	    - plr[pnum]._pBaseMag
	    + MaxStats[c][ATTRIB_DEX]
	    - plr[pnum]._pBaseDex
	    + MaxStats[c][ATTRIB_VIT]
	    - plr[pnum]._pBaseVit;
}

void NextPlrLevel(int pnum)
{
	int hp, mana;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("NextPlrLevel: illegal player %d", pnum);
	}

	plr[pnum]._pLevel++;
	plr[pnum]._pMaxLvl++;

	CalcPlrInv(pnum, TRUE);

	if (CalcStatDiff(pnum) < 5) {
		plr[pnum]._pStatPts = CalcStatDiff(pnum);
	} else {
		plr[pnum]._pStatPts += 5;
	}

	plr[pnum]._pNextExper = ExpLvlsTbl[plr[pnum]._pLevel];

	hp = plr[pnum]._pClass == PC_SORCERER ? 64 : 128;
	if (!gbIsMultiplayer) {
		hp++;
	}
	plr[pnum]._pMaxHP += hp;
	plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
	plr[pnum]._pMaxHPBase += hp;
	plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;

	if (pnum == myplr) {
		drawhpflag = TRUE;
	}

	if (plr[pnum]._pClass == PC_WARRIOR)
		mana = 64;
	else if (plr[pnum]._pClass == PC_BARBARIAN)
		mana = 0;
	else
		mana = 128;

	if (!gbIsMultiplayer) {
		mana++;
	}
	plr[pnum]._pMaxMana += mana;
	plr[pnum]._pMaxManaBase += mana;

	if (!(plr[pnum]._pIFlags & ISPL_NOMANA)) {
		plr[pnum]._pMana = plr[pnum]._pMaxMana;
		plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
	}

	if (pnum == myplr) {
		drawmanaflag = TRUE;
	}

	if (sgbControllerActive)
		FocusOnCharInfo();

	CalcPlrInv(pnum, TRUE);
}

void AddPlrExperience(int pnum, int lvl, int exp)
{
	int powerLvlCap, expCap, newLvl, i;

	if (pnum != myplr) {
		return;
	}

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("AddPlrExperience: illegal player %d", myplr);
	}

	if (plr[myplr]._pHitPoints <= 0) {
		return;
	}

	// Adjust xp based on difference in level between player and monster
	exp *= 1 + ((double)lvl - plr[pnum]._pLevel) / 10;
	if (exp < 0) {
		exp = 0;
	}

	// Prevent power leveling
	if (gbIsMultiplayer) {
		powerLvlCap = plr[pnum]._pLevel < 0 ? 0 : plr[pnum]._pLevel;
		if (powerLvlCap >= 50) {
			powerLvlCap = 50;
		}
		// cap to 1/20 of current levels xp
		if (exp >= ExpLvlsTbl[powerLvlCap] / 20) {
			exp = ExpLvlsTbl[powerLvlCap] / 20;
		}
		// cap to 200 * current level
		expCap = 200 * powerLvlCap;
		if (exp >= expCap) {
			exp = expCap;
		}
	}

	plr[pnum]._pExperience += exp;
	if ((DWORD)plr[pnum]._pExperience > MAXEXP) {
		plr[pnum]._pExperience = MAXEXP;
	}

	if (plr[pnum]._pExperience >= ExpLvlsTbl[49]) {
		plr[pnum]._pLevel = 50;
		return;
	}

	// Increase player level if applicable
	newLvl = 0;
	while (plr[pnum]._pExperience >= ExpLvlsTbl[newLvl]) {
		newLvl++;
	}
	if (newLvl != plr[pnum]._pLevel) {
		for (i = newLvl - plr[pnum]._pLevel; i > 0; i--) {
			NextPlrLevel(pnum);
		}
	}

	NetSendCmdParam1(FALSE, CMD_PLRLEVEL, plr[myplr]._pLevel);
}

void AddPlrMonstExper(int lvl, int exp, char pmask)
{
	int totplrs, i, e;

	totplrs = 0;
	for (i = 0; i < MAX_PLRS; i++) {
		if ((1 << i) & pmask) {
			totplrs++;
		}
	}

	if (totplrs) {
		e = exp / totplrs;
		if (pmask & (1 << myplr))
			AddPlrExperience(myplr, lvl, e);
	}
}

void InitPlayer(int pnum, BOOL FirstTime)
{
	DWORD i;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitPlayer: illegal player %d", pnum);
	}

	ClearPlrRVars(&plr[pnum]);

	if (FirstTime) {
		plr[pnum]._pRSplType = RSPLTYPE_INVALID;
		plr[pnum]._pRSpell = SPL_INVALID;
		plr[pnum]._pSBkSpell = SPL_INVALID;
		plr[pnum]._pSpell = plr[pnum]._pRSpell;
		plr[pnum]._pSplType = plr[pnum]._pRSplType;
		if ((plr[pnum]._pgfxnum & 0xF) == ANIM_ID_BOW) {
			plr[pnum]._pwtype = WT_RANGED;
		} else {
			plr[pnum]._pwtype = WT_MELEE;
		}
		plr[pnum].pManaShield = FALSE;
	}

	if (plr[pnum].plrlevel == currlevel || leveldebug) {

		SetPlrAnims(pnum);

		plr[pnum]._pxoff = 0;
		plr[pnum]._pyoff = 0;
		plr[pnum]._pxvel = 0;
		plr[pnum]._pyvel = 0;

		ClearPlrPVars(pnum);

		if (plr[pnum]._pHitPoints >> 6 > 0) {
			plr[pnum]._pmode = PM_STAND;
			NewPlrAnim(pnum, plr[pnum]._pNAnim[DIR_S], plr[pnum]._pNFrames, 3, plr[pnum]._pNWidth);
			plr[pnum]._pAnimFrame = random_(2, plr[pnum]._pNFrames - 1) + 1;
			plr[pnum]._pAnimCnt = random_(2, 3);
		} else {
			plr[pnum]._pmode = PM_DEATH;
			NewPlrAnim(pnum, plr[pnum]._pDAnim[DIR_S], plr[pnum]._pDFrames, 1, plr[pnum]._pDWidth);
			plr[pnum]._pAnimFrame = plr[pnum]._pAnimLen - 1;
			plr[pnum]._pVar8 = 2 * plr[pnum]._pAnimLen;
		}

		plr[pnum]._pdir = DIR_S;

		if (pnum == myplr) {
			if (!FirstTime || currlevel != 0) {
				plr[pnum]._px = ViewX;
				plr[pnum]._py = ViewY;
			}
			plr[pnum]._ptargx = plr[pnum]._px;
			plr[pnum]._ptargy = plr[pnum]._py;
		} else {
			plr[pnum]._ptargx = plr[pnum]._px;
			plr[pnum]._ptargy = plr[pnum]._py;
			for (i = 0; i < 8 && !PosOkPlayer(pnum, plrxoff2[i] + plr[pnum]._px, plryoff2[i] + plr[pnum]._py); i++)
				;
			plr[pnum]._px += plrxoff2[i];
			plr[pnum]._py += plryoff2[i];
		}

		plr[pnum]._pfutx = plr[pnum]._px;
		plr[pnum]._pfuty = plr[pnum]._py;
		plr[pnum].walkpath[0] = WALK_NONE;
		plr[pnum].destAction = ACTION_NONE;

		if (pnum == myplr) {
			plr[pnum]._plid = AddLight(plr[pnum]._px, plr[pnum]._py, plr[pnum]._pLightRad);
			ChangeLightXY(plr[myplr]._plid, plr[myplr]._px, plr[myplr]._py); // fix for a bug where old light is still visible at the entrance after reentering level
		} else {
			plr[pnum]._plid = NO_LIGHT;
		}
		plr[pnum]._pvid = AddVision(plr[pnum]._px, plr[pnum]._py, plr[pnum]._pLightRad, pnum == myplr);
	}

	if (plr[pnum]._pClass == PC_WARRIOR) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_REPAIR);
	} else if (plr[pnum]._pClass == PC_ROGUE) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_DISARM);
	} else if (plr[pnum]._pClass == PC_SORCERER) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_RECHARGE);
	} else if (plr[pnum]._pClass == PC_MONK) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_SEARCH);
	} else if (plr[pnum]._pClass == PC_BARD) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_IDENTIFY);
	} else if (plr[pnum]._pClass == PC_BARBARIAN) {
		plr[pnum]._pAblSpells = SPELLBIT(SPL_BLODBOIL);
	}

#ifdef _DEBUG
	if (debug_mode_dollar_sign && FirstTime) {
		plr[pnum]._pMemSpells |= 1 << (SPL_TELEPORT - 1);
		if (!plr[myplr]._pSplLvl[SPL_TELEPORT]) {
			plr[myplr]._pSplLvl[SPL_TELEPORT] = 1;
		}
	}
	if (debug_mode_key_inverted_v && FirstTime) {
		plr[pnum]._pMemSpells = SPL_INVALID;
	}
#endif

	plr[pnum]._pNextExper = ExpLvlsTbl[plr[pnum]._pLevel];
	plr[pnum]._pInvincible = FALSE;

	if (pnum == myplr) {
		deathdelay = 0;
		deathflag = FALSE;
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = SDIR_NONE;
	}
}

void InitMultiView()
{
	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("InitPlayer: illegal player %d", myplr);
	}

	ViewX = plr[myplr]._px;
	ViewY = plr[myplr]._py;
}

BOOL SolidLoc(int x, int y)
{
	if (x < 0 || y < 0 || x >= MAXDUNX || y >= MAXDUNY) {
		return FALSE;
	}

	return nSolidTable[dPiece[x][y]];
}

BOOL PlrDirOK(int pnum, int dir)
{
	int px, py;
	BOOL isOk;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrDirOK: illegal player %d", pnum);
	}

	px = plr[pnum]._px + offset_x[dir];
	py = plr[pnum]._py + offset_y[dir];

	if (px < 0 || !dPiece[px][py] || !PosOkPlayer(pnum, px, py)) {
		return FALSE;
	}

	isOk = TRUE;
	if (dir == DIR_E) {
		isOk = !SolidLoc(px, py + 1) && !(dFlags[px][py + 1] & BFLAG_PLAYERLR);
	}

	if (isOk && dir == DIR_W) {
		isOk = !SolidLoc(px + 1, py) && !(dFlags[px + 1][py] & BFLAG_PLAYERLR);
	}

	return isOk;
}

void PlrClrTrans(int x, int y)
{
	int i, j;

	for (i = y - 1; i <= y + 1; i++) {
		for (j = x - 1; j <= x + 1; j++) {
			TransList[dTransVal[j][i]] = FALSE;
		}
	}
}

void PlrDoTrans(int x, int y)
{
	int i, j;

	if (leveltype != DTYPE_CATHEDRAL && leveltype != DTYPE_CATACOMBS && leveltype != DTYPE_CRYPT) {
		TransList[1] = TRUE;
	} else {
		for (i = y - 1; i <= y + 1; i++) {
			for (j = x - 1; j <= x + 1; j++) {
				if (!nSolidTable[dPiece[j][i]] && dTransVal[j][i]) {
					TransList[dTransVal[j][i]] = TRUE;
				}
			}
		}
	}
}

void SetPlayerOld(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SetPlayerOld: illegal player %d", pnum);
	}

	plr[pnum]._poldx = plr[pnum]._px;
	plr[pnum]._poldy = plr[pnum]._py;
}

void FixPlayerLocation(int pnum, int bDir)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("FixPlayerLocation: illegal player %d", pnum);
	}

	plr[pnum]._pfutx = plr[pnum]._px;
	plr[pnum]._pfuty = plr[pnum]._py;
	plr[pnum]._ptargx = plr[pnum]._px;
	plr[pnum]._ptargy = plr[pnum]._py;
	plr[pnum]._pxoff = 0;
	plr[pnum]._pyoff = 0;
	plr[pnum]._pdir = bDir;
	if (pnum == myplr) {
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = SDIR_NONE;
		ViewX = plr[pnum]._px;
		ViewY = plr[pnum]._py;
	}
	ChangeLightXY(plr[pnum]._plid, plr[pnum]._px, plr[pnum]._py);
	ChangeVisionXY(plr[pnum]._pvid, plr[pnum]._px, plr[pnum]._py);
}

void StartStand(int pnum, int dir)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartStand: illegal player %d", pnum);
	}

	if (!plr[pnum]._pInvincible || plr[pnum]._pHitPoints != 0 || pnum != myplr) {
		if (!(plr[pnum]._pGFXLoad & PFILE_STAND)) {
			LoadPlrGFX(pnum, PFILE_STAND);
		}

		NewPlrAnim(pnum, plr[pnum]._pNAnim[dir], plr[pnum]._pNFrames, 3, plr[pnum]._pNWidth);
		plr[pnum]._pmode = PM_STAND;
		FixPlayerLocation(pnum, dir);
		FixPlrWalkTags(pnum);
		dPlayer[plr[pnum]._px][plr[pnum]._py] = pnum + 1;
		SetPlayerOld(pnum);
	} else {
		SyncPlrKill(pnum, -1);
	}
}

void StartWalkStand(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartWalkStand: illegal player %d", pnum);
	}

	plr[pnum]._pmode = PM_STAND;
	plr[pnum]._pfutx = plr[pnum]._px;
	plr[pnum]._pfuty = plr[pnum]._py;
	plr[pnum]._pxoff = 0;
	plr[pnum]._pyoff = 0;

	if (pnum == myplr) {
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = SDIR_NONE;
		ViewX = plr[pnum]._px;
		ViewY = plr[pnum]._py;
	}
}

void PM_ChangeLightOff(int pnum)
{
	int x, y;
	int xmul, ymul;
	int lx, ly;
	int offx, offy;
	const LightListStruct *l;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_ChangeLightOff: illegal player %d", pnum);
	}

	if (plr[pnum]._plid == NO_LIGHT)
		return;

	l = &LightList[plr[pnum]._plid];
	x = 2 * plr[pnum]._pyoff + plr[pnum]._pxoff;
	y = 2 * plr[pnum]._pyoff - plr[pnum]._pxoff;
	if (x < 0) {
		xmul = -1;
		x = -x;
	} else {
		xmul = 1;
	}
	if (y < 0) {
		ymul = -1;
		y = -y;
	} else {
		ymul = 1;
	}

	x = (x >> 3) * xmul;
	y = (y >> 3) * ymul;
	lx = x + (l->_lx << 3);
	ly = y + (l->_ly << 3);
	offx = l->_xoff + (l->_lx << 3);
	offy = l->_yoff + (l->_ly << 3);

	if (abs(lx - offx) < 3 && abs(ly - offy) < 3)
		return;

	ChangeLightOff(plr[pnum]._plid, x, y);
}

void PM_ChangeOffset(int pnum)
{
	int px, py;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_ChangeOffset: illegal player %d", pnum);
	}

	plr[pnum]._pVar8++;
	px = plr[pnum]._pVar6 / 256;
	py = plr[pnum]._pVar7 / 256;

	plr[pnum]._pVar6 += plr[pnum]._pxvel;
	plr[pnum]._pVar7 += plr[pnum]._pyvel;

	if (gbIsHellfire && currlevel == 0 && jogging_opt) {
		plr[pnum]._pVar6 += plr[pnum]._pxvel;
		plr[pnum]._pVar7 += plr[pnum]._pyvel;
	}

	plr[pnum]._pxoff = plr[pnum]._pVar6 >> 8;
	plr[pnum]._pyoff = plr[pnum]._pVar7 >> 8;

	px -= plr[pnum]._pVar6 >> 8;
	py -= plr[pnum]._pVar7 >> 8;

	if (pnum == myplr && ScrollInfo._sdir) {
		ScrollInfo._sxoff += px;
		ScrollInfo._syoff += py;
	}

	PM_ChangeLightOff(pnum);
}

/**
 * @brief Start moving a player to a new tile
 */
void StartWalk(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir, int sdir, int variant)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartWalk: illegal player %d", pnum);
	}

	if (plr[pnum]._pInvincible && plr[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	SetPlayerOld(pnum);

	if (!PlrDirOK(pnum, EndDir)) {
		return;
	}

	//The player's tile position after finishing this movement action
	int px = xadd + plr[pnum]._px;
	int py = yadd + plr[pnum]._py;
	plr[pnum]._pfutx = px;
	plr[pnum]._pfuty = py;

	//If this is the local player then update the camera offset position
	if (pnum == myplr) {
		ScrollInfo._sdx = plr[pnum]._px - ViewX;
		ScrollInfo._sdy = plr[pnum]._py - ViewY;
	}

	switch (variant) {
	case PM_WALK:
		dPlayer[px][py] = -(pnum + 1);
		plr[pnum]._pmode = PM_WALK;
		plr[pnum]._pxvel = xvel;
		plr[pnum]._pyvel = yvel;
		plr[pnum]._pxoff = 0;
		plr[pnum]._pyoff = 0;
		plr[pnum]._pVar1 = xadd;
		plr[pnum]._pVar2 = yadd;
		plr[pnum]._pVar3 = EndDir;

		plr[pnum]._pVar6 = 0;
		plr[pnum]._pVar7 = 0;
		break;
	case PM_WALK2:
		dPlayer[plr[pnum]._px][plr[pnum]._py] = -(pnum + 1);
		plr[pnum]._pVar1 = plr[pnum]._px;
		plr[pnum]._pVar2 = plr[pnum]._py;
		plr[pnum]._px = px; // Move player to the next tile to maintain correct render order
		plr[pnum]._py = py;
		dPlayer[plr[pnum]._px][plr[pnum]._py] = pnum + 1;
		plr[pnum]._pxoff = xoff; // Offset player sprite to align with their previous tile position
		plr[pnum]._pyoff = yoff;

		ChangeLightXY(plr[pnum]._plid, plr[pnum]._px, plr[pnum]._py);
		PM_ChangeLightOff(pnum);

		plr[pnum]._pmode = PM_WALK2;
		plr[pnum]._pxvel = xvel;
		plr[pnum]._pyvel = yvel;
		plr[pnum]._pVar6 = xoff * 256;
		plr[pnum]._pVar7 = yoff * 256;
		plr[pnum]._pVar3 = EndDir;
		break;
	case PM_WALK3:
		int x = mapx + plr[pnum]._px;
		int y = mapy + plr[pnum]._py;

		dPlayer[plr[pnum]._px][plr[pnum]._py] = -(pnum + 1);
		dPlayer[px][py] = -(pnum + 1);
		plr[pnum]._pVar4 = x;
		plr[pnum]._pVar5 = y;
		dFlags[x][y] |= BFLAG_PLAYERLR;
		plr[pnum]._pxoff = xoff; // Offset player sprite to align with their previous tile position
		plr[pnum]._pyoff = yoff;

		if (leveltype != DTYPE_TOWN) {
			ChangeLightXY(plr[pnum]._plid, x, y);
			PM_ChangeLightOff(pnum);
		}

		plr[pnum]._pmode = PM_WALK3;
		plr[pnum]._pxvel = xvel;
		plr[pnum]._pyvel = yvel;
		plr[pnum]._pVar1 = px;
		plr[pnum]._pVar2 = py;
		plr[pnum]._pVar6 = xoff * 256;
		plr[pnum]._pVar7 = yoff * 256;
		plr[pnum]._pVar3 = EndDir;
		break;
	}

	//Load walk animation in case it's not loaded yet
	if (!(plr[pnum]._pGFXLoad & PFILE_WALK)) {
		LoadPlrGFX(pnum, PFILE_WALK);
	}

	//Start walk animation
	NewPlrAnim(pnum, plr[pnum]._pWAnim[EndDir], plr[pnum]._pWFrames, 0, plr[pnum]._pWWidth);

	plr[pnum]._pdir = EndDir;
	plr[pnum]._pVar8 = 0;

	if (pnum != myplr) {
		return;
	}

	if (zoomflag) {
		if (abs(ScrollInfo._sdx) >= 3 || abs(ScrollInfo._sdy) >= 3) {
			ScrollInfo._sdir = SDIR_NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (abs(ScrollInfo._sdx) >= 2 || abs(ScrollInfo._sdy) >= 2) {
		ScrollInfo._sdir = SDIR_NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

void StartAttack(int pnum, int d)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartAttack: illegal player %d", pnum);
	}

	if (plr[pnum]._pInvincible && plr[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if (!(plr[pnum]._pGFXLoad & PFILE_ATTACK)) {
		LoadPlrGFX(pnum, PFILE_ATTACK);
	}

	NewPlrAnim(pnum, plr[pnum]._pAAnim[d], plr[pnum]._pAFrames, 0, plr[pnum]._pAWidth);
	plr[pnum]._pmode = PM_ATTACK;
	FixPlayerLocation(pnum, d);
	SetPlayerOld(pnum);
}

void StartRangeAttack(int pnum, int d, int cx, int cy)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartRangeAttack: illegal player %d", pnum);
	}

	if (plr[pnum]._pInvincible && plr[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if (!(plr[pnum]._pGFXLoad & PFILE_ATTACK)) {
		LoadPlrGFX(pnum, PFILE_ATTACK);
	}
	NewPlrAnim(pnum, plr[pnum]._pAAnim[d], plr[pnum]._pAFrames, 0, plr[pnum]._pAWidth);

	plr[pnum]._pmode = PM_RATTACK;
	FixPlayerLocation(pnum, d);
	SetPlayerOld(pnum);
	plr[pnum]._pVar1 = cx;
	plr[pnum]._pVar2 = cy;
}

void StartPlrBlock(int pnum, int dir)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlrBlock: illegal player %d", pnum);
	}

	if (plr[pnum]._pInvincible && plr[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	PlaySfxLoc(IS_ISWORD, plr[pnum]._px, plr[pnum]._py);

	if (!(plr[pnum]._pGFXLoad & PFILE_BLOCK)) {
		LoadPlrGFX(pnum, PFILE_BLOCK);
	}
	NewPlrAnim(pnum, plr[pnum]._pBAnim[dir], plr[pnum]._pBFrames, 2, plr[pnum]._pBWidth);

	plr[pnum]._pmode = PM_BLOCK;
	FixPlayerLocation(pnum, dir);
	SetPlayerOld(pnum);
}

void StartSpell(int pnum, int d, int cx, int cy)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartSpell: illegal player %d", pnum);

	if (plr[pnum]._pInvincible && plr[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if (leveltype != DTYPE_TOWN) {
		switch (spelldata[plr[pnum]._pSpell].sType) {
		case STYPE_FIRE:
			if (!(plr[pnum]._pGFXLoad & PFILE_FIRE)) {
				LoadPlrGFX(pnum, PFILE_FIRE);
			}
			NewPlrAnim(pnum, plr[pnum]._pFAnim[d], plr[pnum]._pSFrames, 0, plr[pnum]._pSWidth);
			break;
		case STYPE_LIGHTNING:
			if (!(plr[pnum]._pGFXLoad & PFILE_LIGHTNING)) {
				LoadPlrGFX(pnum, PFILE_LIGHTNING);
			}
			NewPlrAnim(pnum, plr[pnum]._pLAnim[d], plr[pnum]._pSFrames, 0, plr[pnum]._pSWidth);
			break;
		case STYPE_MAGIC:
			if (!(plr[pnum]._pGFXLoad & PFILE_MAGIC)) {
				LoadPlrGFX(pnum, PFILE_MAGIC);
			}
			NewPlrAnim(pnum, plr[pnum]._pTAnim[d], plr[pnum]._pSFrames, 0, plr[pnum]._pSWidth);
			break;
		}
	}

	PlaySfxLoc(spelldata[plr[pnum]._pSpell].sSFX, plr[pnum]._px, plr[pnum]._py);

	plr[pnum]._pmode = PM_SPELL;

	FixPlayerLocation(pnum, d);
	SetPlayerOld(pnum);

	plr[pnum]._pVar1 = cx;
	plr[pnum]._pVar2 = cy;
	plr[pnum]._pVar4 = GetSpellLevel(pnum, plr[pnum]._pSpell);
	plr[pnum]._pVar8 = 1;
}

void FixPlrWalkTags(int pnum)
{
	int pp, pn;
	int dx, dy, y, x;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("FixPlrWalkTags: illegal player %d", pnum);
	}

	pp = pnum + 1;
	pn = -(pnum + 1);
	dx = plr[pnum]._poldx;
	dy = plr[pnum]._poldy;
	for (y = dy - 1; y <= dy + 1; y++) {
		for (x = dx - 1; x <= dx + 1; x++) {
			if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY && (dPlayer[x][y] == pp || dPlayer[x][y] == pn)) {
				dPlayer[x][y] = 0;
			}
		}
	}

	if (dx >= 0 && dx < MAXDUNX - 1 && dy >= 0 && dy < MAXDUNY - 1) {
		dFlags[dx + 1][dy] &= ~BFLAG_PLAYERLR;
		dFlags[dx][dy + 1] &= ~BFLAG_PLAYERLR;
	}
}

void RemovePlrFromMap(int pnum)
{
	int x, y;
	int pp, pn;

	pp = pnum + 1;
	pn = -(pnum + 1);

	for (y = 1; y < MAXDUNY; y++)
		for (x = 1; x < MAXDUNX; x++)
			if (dPlayer[x][y - 1] == pn || dPlayer[x - 1][y] == pn)
				if (dFlags[x][y] & BFLAG_PLAYERLR)
					dFlags[x][y] &= ~BFLAG_PLAYERLR;

	for (y = 0; y < MAXDUNY; y++)
		for (x = 0; x < MAXDUNX; x++)
			if (dPlayer[x][y] == pp || dPlayer[x][y] == pn)
				dPlayer[x][y] = 0;
}

void StartPlrHit(int pnum, int dam, BOOL forcehit)
{
	int pd;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlrHit: illegal player %d", pnum);
	}

	if (plr[pnum]._pInvincible && plr[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if (plr[pnum]._pClass == PC_WARRIOR) {
		PlaySfxLoc(PS_WARR69, plr[pnum]._px, plr[pnum]._py);
	} else if (plr[pnum]._pClass == PC_ROGUE) {
		PlaySfxLoc(PS_ROGUE69, plr[pnum]._px, plr[pnum]._py);
	} else if (plr[pnum]._pClass == PC_SORCERER) {
		PlaySfxLoc(PS_MAGE69, plr[pnum]._px, plr[pnum]._py);
	} else if (plr[pnum]._pClass == PC_MONK) {
		PlaySfxLoc(PS_MONK69, plr[pnum]._px, plr[pnum]._py);
	} else if (plr[pnum]._pClass == PC_BARD) {
		PlaySfxLoc(PS_ROGUE69, plr[pnum]._px, plr[pnum]._py);
	} else if (plr[pnum]._pClass == PC_BARBARIAN) {
		PlaySfxLoc(PS_WARR69, plr[pnum]._px, plr[pnum]._py);
	}

	drawhpflag = TRUE;
	if (plr[pnum]._pClass == PC_BARBARIAN) {
		if (dam >> 6 < plr[pnum]._pLevel + plr[pnum]._pLevel / 4 && !forcehit) {
			return;
		}
	} else if (dam >> 6 < plr[pnum]._pLevel && !forcehit) {
		return;
	}

	pd = plr[pnum]._pdir;

	if (!(plr[pnum]._pGFXLoad & PFILE_HIT)) {
		LoadPlrGFX(pnum, PFILE_HIT);
	}
	NewPlrAnim(pnum, plr[pnum]._pHAnim[pd], plr[pnum]._pHFrames, 0, plr[pnum]._pHWidth);

	plr[pnum]._pmode = PM_GOTHIT;
	FixPlayerLocation(pnum, pd);
	FixPlrWalkTags(pnum);
	dPlayer[plr[pnum]._px][plr[pnum]._py] = pnum + 1;
	SetPlayerOld(pnum);
}

void RespawnDeadItem(ItemStruct *itm, int x, int y)
{
	int ii;

	if (numitems >= MAXITEMS) {
		return;
	}

	ii = itemavail[0];
	dItem[x][y] = ii + 1;
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = ii;
	item[ii] = *itm;
	item[ii]._ix = x;
	item[ii]._iy = y;
	RespawnItem(ii, TRUE);
	numitems++;
	itm->_itype = ITYPE_NONE;
}

static void PlrDeadItem(int pnum, ItemStruct *itm, int xx, int yy)
{
	int x, y;
	int i, j, k;

	if (itm->_itype == ITYPE_NONE)
		return;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrDeadItem: illegal player %d", pnum);
	}

	x = xx + plr[pnum]._px;
	y = yy + plr[pnum]._py;
	if ((xx || yy) && ItemSpaceOk(x, y)) {
		RespawnDeadItem(itm, x, y);
		plr[pnum].HoldItem = *itm;
		NetSendCmdPItem(FALSE, CMD_RESPAWNITEM, x, y);
		return;
	}

	for (k = 1; k < 50; k++) {
		for (j = -k; j <= k; j++) {
			y = j + plr[pnum]._py;
			for (i = -k; i <= k; i++) {
				x = i + plr[pnum]._px;
				if (ItemSpaceOk(x, y)) {
					RespawnDeadItem(itm, x, y);
					plr[pnum].HoldItem = *itm;
					NetSendCmdPItem(FALSE, CMD_RESPAWNITEM, x, y);
					return;
				}
			}
		}
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void
StartPlayerKill(int pnum, int earflag)
{
	BOOL diablolevel;
	int i, pdd;
	PlayerStruct *p;
	ItemStruct ear;
	ItemStruct *pi;

	p = &plr[pnum];
	if (p->_pHitPoints <= 0 && p->_pmode == PM_DEATH) {
		return;
	}

	if (myplr == pnum) {
		NetSendCmdParam1(TRUE, CMD_PLRDEAD, earflag);
	}

	diablolevel = gbIsMultiplayer && plr[pnum].plrlevel == 16;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlayerKill: illegal player %d", pnum);
	}

	if (plr[pnum]._pClass == PC_WARRIOR) {
		PlaySfxLoc(PS_DEAD, p->_px, p->_py); // BUGFIX: should use `PS_WARR71` like other classes
	} else if (plr[pnum]._pClass == PC_ROGUE) {
		PlaySfxLoc(PS_ROGUE71, p->_px, p->_py);
	} else if (plr[pnum]._pClass == PC_SORCERER) {
		PlaySfxLoc(PS_MAGE71, p->_px, p->_py);
	} else if (plr[pnum]._pClass == PC_MONK) {
		PlaySfxLoc(PS_MONK71, p->_px, p->_py);
	} else if (plr[pnum]._pClass == PC_BARD) {
		PlaySfxLoc(PS_ROGUE71, p->_px, p->_py);
	} else if (plr[pnum]._pClass == PC_BARBARIAN) {
		PlaySfxLoc(PS_WARR71, p->_px, p->_py);
	}

	if (p->_pgfxnum) {
		p->_pgfxnum = 0;
		p->_pGFXLoad = 0;
		SetPlrAnims(pnum);
	}

	if (!(p->_pGFXLoad & PFILE_DEATH)) {
		LoadPlrGFX(pnum, PFILE_DEATH);
	}

	NewPlrAnim(pnum, p->_pDAnim[p->_pdir], p->_pDFrames, 1, p->_pDWidth);

	p->_pBlockFlag = FALSE;
	p->_pmode = PM_DEATH;
	p->_pInvincible = TRUE;
	SetPlayerHitPoints(pnum, 0);
	p->_pVar8 = 1;

	if (pnum != myplr && !earflag && !diablolevel) {
		for (i = 0; i < NUM_INVLOC; i++) {
			p->InvBody[i]._itype = ITYPE_NONE;
		}
		CalcPlrInv(pnum, FALSE);
	}

	if (plr[pnum].plrlevel == currlevel) {
		FixPlayerLocation(pnum, p->_pdir);
		RemovePlrFromMap(pnum);
		dFlags[p->_px][p->_py] |= BFLAG_DEAD_PLAYER;
		SetPlayerOld(pnum);

		if (pnum == myplr) {
			drawhpflag = TRUE;
			deathdelay = 30;

			if (pcurs >= CURSOR_FIRSTITEM) {
				PlrDeadItem(pnum, &p->HoldItem, 0, 0);
				SetCursor_(CURSOR_HAND);
			}

			if (!diablolevel) {
				DropHalfPlayersGold(pnum);
				if (earflag != -1) {
					if (earflag != 0) {
						SetPlrHandItem(&ear, IDI_EAR);
						sprintf(ear._iName, "Ear of %s", plr[pnum]._pName);
						if (plr[pnum]._pClass == PC_SORCERER) {
							ear._iCurs = ICURS_EAR_SORCEROR;
						} else if (plr[pnum]._pClass == PC_WARRIOR) {
							ear._iCurs = ICURS_EAR_WARRIOR;
						} else if (plr[pnum]._pClass == PC_ROGUE) {
							ear._iCurs = ICURS_EAR_ROGUE;
						} else if (plr[pnum]._pClass == PC_MONK || plr[pnum]._pClass == PC_BARD || plr[pnum]._pClass == PC_BARBARIAN) {
							ear._iCurs = ICURS_EAR_ROGUE;
						}

						ear._iCreateInfo = plr[pnum]._pName[0] << 8 | plr[pnum]._pName[1];
						ear._iSeed = plr[pnum]._pName[2] << 24 | plr[pnum]._pName[3] << 16 | plr[pnum]._pName[4] << 8 | plr[pnum]._pName[5];
						ear._ivalue = plr[pnum]._pLevel;

						if (FindGetItem(IDI_EAR, ear._iCreateInfo, ear._iSeed) == -1) {
							PlrDeadItem(pnum, &ear, 0, 0);
						}
					} else {
						pi = &p->InvBody[0];
						i = NUM_INVLOC;
						while (i--) {
							pdd = (i + p->_pdir) & 7;
							PlrDeadItem(pnum, pi, offset_x[pdd], offset_y[pdd]);
							pi++;
						}

						CalcPlrInv(pnum, FALSE);
					}
				}
			}
		}
	}
	SetPlayerHitPoints(pnum, 0);
}

void DropHalfPlayersGold(int pnum)
{
	int i, hGold;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("DropHalfPlayersGold: illegal player %d", pnum);
	}

	hGold = plr[pnum]._pGold >> 1;
	for (i = 0; i < MAXBELTITEMS && hGold > 0; i++) {
		if (plr[pnum].SpdList[i]._itype == ITYPE_GOLD && plr[pnum].SpdList[i]._ivalue != MaxGold) {
			if (hGold < plr[pnum].SpdList[i]._ivalue) {
				plr[pnum].SpdList[i]._ivalue -= hGold;
				SetSpdbarGoldCurs(pnum, i);
				SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &plr[pnum].HoldItem);
				SetPlrHandGoldCurs(&plr[pnum].HoldItem);
				plr[pnum].HoldItem._ivalue = hGold;
				PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
				hGold = 0;
			} else {
				hGold -= plr[pnum].SpdList[i]._ivalue;
				RemoveSpdBarItem(pnum, i);
				SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &plr[pnum].HoldItem);
				SetPlrHandGoldCurs(&plr[pnum].HoldItem);
				plr[pnum].HoldItem._ivalue = plr[pnum].SpdList[i]._ivalue;
				PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
				i = -1;
			}
		}
	}
	if (hGold > 0) {
		for (i = 0; i < MAXBELTITEMS && hGold > 0; i++) {
			if (plr[pnum].SpdList[i]._itype == ITYPE_GOLD) {
				if (hGold < plr[pnum].SpdList[i]._ivalue) {
					plr[pnum].SpdList[i]._ivalue -= hGold;
					SetSpdbarGoldCurs(pnum, i);
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._ivalue = hGold;
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					hGold = 0;
				} else {
					hGold -= plr[pnum].SpdList[i]._ivalue;
					RemoveSpdBarItem(pnum, i);
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._ivalue = plr[pnum].SpdList[i]._ivalue;
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					i = -1;
				}
			}
		}
	}
	force_redraw = 255;
	if (hGold > 0) {
		for (i = 0; i < plr[pnum]._pNumInv && hGold > 0; i++) {
			if (plr[pnum].InvList[i]._itype == ITYPE_GOLD && plr[pnum].InvList[i]._ivalue != MaxGold) {
				if (hGold < plr[pnum].InvList[i]._ivalue) {
					plr[pnum].InvList[i]._ivalue -= hGold;
					SetGoldCurs(pnum, i);
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._ivalue = hGold;
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					hGold = 0;
				} else {
					hGold -= plr[pnum].InvList[i]._ivalue;
					RemoveInvItem(pnum, i);
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._ivalue = plr[pnum].InvList[i]._ivalue;
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					i = -1;
				}
			}
		}
	}
	if (hGold > 0) {
		for (i = 0; i < plr[pnum]._pNumInv && hGold > 0; i++) {
			if (plr[pnum].InvList[i]._itype == ITYPE_GOLD) {
				if (hGold < plr[pnum].InvList[i]._ivalue) {
					plr[pnum].InvList[i]._ivalue -= hGold;
					SetGoldCurs(pnum, i);
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._ivalue = hGold;
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					hGold = 0;
				} else {
					hGold -= plr[pnum].InvList[i]._ivalue;
					RemoveInvItem(pnum, i);
					SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &plr[pnum].HoldItem);
					SetPlrHandGoldCurs(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._ivalue = plr[pnum].InvList[i]._ivalue;
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
					i = -1;
				}
			}
		}
	}
	plr[pnum]._pGold = CalculateGold(pnum);
}

void StripTopGold(int pnum)
{
	ItemStruct tmpItem;
	int i, val;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StripTopGold: illegal player %d", pnum);
	}
	tmpItem = plr[pnum].HoldItem;

	for (i = 0; i < plr[pnum]._pNumInv; i++) {
		if (plr[pnum].InvList[i]._itype == ITYPE_GOLD) {
			if (plr[pnum].InvList[i]._ivalue > MaxGold) {
				val = plr[pnum].InvList[i]._ivalue - MaxGold;
				plr[pnum].InvList[i]._ivalue = MaxGold;
				SetGoldCurs(pnum, i);
				SetPlrHandItem(&plr[pnum].HoldItem, 0);
				GetGoldSeed(pnum, &plr[pnum].HoldItem);
				plr[pnum].HoldItem._ivalue = val;
				SetPlrHandGoldCurs(&plr[pnum].HoldItem);
				if (!GoldAutoPlace(pnum))
					PlrDeadItem(pnum, &plr[pnum].HoldItem, 0, 0);
			}
		}
	}
	plr[pnum]._pGold = CalculateGold(pnum);
	plr[pnum].HoldItem = tmpItem;
}

void SyncPlrKill(int pnum, int earflag)
{
	int ma, i;

	if (plr[pnum]._pHitPoints <= 0 && currlevel == 0) {
		SetPlayerHitPoints(pnum, 64);
		return;
	}

	for (i = 0; i < nummissiles; i++) {
		ma = missileactive[i];
		if (missile[ma]._mitype == MIS_MANASHIELD && missile[ma]._misource == pnum && missile[ma]._miDelFlag == FALSE) {
			if (earflag != -1) {
				missile[ma]._miVar8 = earflag;
			}

			return;
		}
	}

	SetPlayerHitPoints(pnum, 0);
	StartPlayerKill(pnum, earflag);
}

void RemovePlrMissiles(int pnum)
{
	int i, am;
	int mx, my;

	if (currlevel != 0 && pnum == myplr && (monster[myplr]._mx != 1 || monster[myplr]._my != 0)) {
		M_StartKill(myplr, myplr);
		AddDead(monster[myplr]._mx, monster[myplr]._my, (monster[myplr].MType)->mdeadval, monster[myplr]._mdir);
		mx = monster[myplr]._mx;
		my = monster[myplr]._my;
		dMonster[mx][my] = 0;
		monster[myplr]._mDelFlag = TRUE;
		DeleteMonsterList();
	}

	for (i = 0; i < nummissiles; i++) {
		am = missileactive[i];
		if (missile[am]._mitype == MIS_STONE && missile[am]._misource == pnum) {
			monster[missile[am]._miVar2]._mmode = missile[am]._miVar1;
		}
		if (missile[am]._mitype == MIS_MANASHIELD && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
		if (missile[am]._mitype == MIS_ETHEREALIZE && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
	}
}

void InitLevelChange(int pnum)
{
	RemovePlrMissiles(pnum);
	if (pnum == myplr && qtextflag) {
		qtextflag = FALSE;
		stream_stop();
	}

	RemovePlrFromMap(pnum);
	SetPlayerOld(pnum);
	if (pnum == myplr) {
		dPlayer[plr[myplr]._px][plr[myplr]._py] = myplr + 1;
	} else {
		plr[pnum]._pLvlVisited[plr[pnum].plrlevel] = TRUE;
	}

	ClrPlrPath(pnum);
	plr[pnum].destAction = ACTION_NONE;
	plr[pnum]._pLvlChanging = TRUE;

	if (pnum == myplr) {
		plr[pnum].pLvlLoad = 10;
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void
StartNewLvl(int pnum, int fom, int lvl)
{
	InitLevelChange(pnum);

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartNewLvl: illegal player %d", pnum);
	}

	switch (fom) {
	case WM_DIABNEXTLVL:
	case WM_DIABPREVLVL:
		plr[pnum].plrlevel = lvl;
		break;
	case WM_DIABRTNLVL:
	case WM_DIABTOWNWARP:
		plr[pnum].plrlevel = lvl;
		break;
	case WM_DIABSETLVL:
		setlvlnum = lvl;
		break;
	case WM_DIABTWARPUP:
		plr[myplr].pTownWarps |= 1 << (leveltype - 2);
		plr[pnum].plrlevel = lvl;
		break;
	case WM_DIABRETOWN:
		break;
	default:
		app_fatal("StartNewLvl");
		break;
	}

	if (pnum == myplr) {
		plr[pnum]._pmode = PM_NEWLVL;
		plr[pnum]._pInvincible = TRUE;
		PostMessage(fom, 0, 0);
		if (gbIsMultiplayer) {
			NetSendCmdParam2(TRUE, CMD_NEWLVL, fom, lvl);
		}
	}
}
void RestartTownLvl(int pnum)
{
	InitLevelChange(pnum);
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("RestartTownLvl: illegal player %d", pnum);
	}

	plr[pnum].plrlevel = 0;
	plr[pnum]._pInvincible = FALSE;

	SetPlayerHitPoints(pnum, 64);

	plr[pnum]._pMana = 0;
	plr[pnum]._pManaBase = plr[pnum]._pMana - (plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase);

	CalcPlrInv(pnum, FALSE);

	if (pnum == myplr) {
		plr[pnum]._pmode = PM_NEWLVL;
		plr[pnum]._pInvincible = TRUE;
		PostMessage(WM_DIABRETOWN, 0, 0);
	}
}

void StartWarpLvl(int pnum, int pidx)
{
	InitLevelChange(pnum);

	if (gbIsMultiplayer) {
		if (plr[pnum].plrlevel != 0) {
			plr[pnum].plrlevel = 0;
		} else {
			plr[pnum].plrlevel = portal[pidx].level;
		}
	}

	if (pnum == myplr) {
		SetCurrentPortal(pidx);
		plr[pnum]._pmode = PM_NEWLVL;
		plr[pnum]._pInvincible = TRUE;
		PostMessage(WM_DIABWARPLVL, 0, 0);
	}
}

BOOL PM_DoStand(int pnum)
{
	return FALSE;
}

/**
 * @brief Continue movement towards new tile
 */
bool PM_DoWalk(int pnum, int variant)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoWalk: illegal player %d", pnum);
	}

	//Play walking sound effect on certain animation frames
	if (!gbIsHellfire) {
		if (plr[pnum]._pAnimFrame == 3
		    || (plr[pnum]._pWFrames == 8 && plr[pnum]._pAnimFrame == 7)
		    || (plr[pnum]._pWFrames != 8 && plr[pnum]._pAnimFrame == 4)) {
			PlaySfxLoc(PS_WALK1, plr[pnum]._px, plr[pnum]._py);
		}
	}

	//"Jog" in town which works by doubling movement speed and skipping every other animation frame
	if (gbIsHellfire && currlevel == 0 && jogging_opt) {
		if (plr[pnum]._pAnimFrame % 2 == 0) {
			plr[pnum]._pAnimFrame++;
			plr[pnum]._pVar8++;
		}
		if (plr[pnum]._pAnimFrame >= plr[pnum]._pWFrames) {
			plr[pnum]._pAnimFrame = 0;
		}
	}

	//Acquire length of walk animation length (this is 8 for every class, so the AnimLenFromClass array is redundant right now)
	int anim_len = 8;
	if (currlevel != 0) {
		anim_len = AnimLenFromClass[plr[pnum]._pClass];
	}

	//Check if we reached new tile
	if (plr[pnum]._pVar8 >= anim_len) {

		//Update the player's tile position
		switch (variant) {
		case PM_WALK:
			dPlayer[plr[pnum]._px][plr[pnum]._py] = 0;
			plr[pnum]._px += plr[pnum]._pVar1;
			plr[pnum]._py += plr[pnum]._pVar2;
			dPlayer[plr[pnum]._px][plr[pnum]._py] = pnum + 1;
			break;
		case PM_WALK2:
			dPlayer[plr[pnum]._pVar1][plr[pnum]._pVar2] = 0;
			break;
		case PM_WALK3:
			dPlayer[plr[pnum]._px][plr[pnum]._py] = 0;
			dFlags[plr[pnum]._pVar4][plr[pnum]._pVar5] &= ~BFLAG_PLAYERLR;
			plr[pnum]._px = plr[pnum]._pVar1;
			plr[pnum]._py = plr[pnum]._pVar2;
			dPlayer[plr[pnum]._px][plr[pnum]._py] = pnum + 1;
			break;
		}

		//Update the coordinates for lighting and vision entries for the player
		if (leveltype != DTYPE_TOWN) {
			ChangeLightXY(plr[pnum]._plid, plr[pnum]._px, plr[pnum]._py);
			ChangeVisionXY(plr[pnum]._pvid, plr[pnum]._px, plr[pnum]._py);
		}

		//Update the "camera" tile position
		if (pnum == myplr && ScrollInfo._sdir) {
			ViewX = plr[pnum]._px - ScrollInfo._sdx;
			ViewY = plr[pnum]._py - ScrollInfo._sdy;
		}

		if (plr[pnum].walkpath[0] != WALK_NONE) {
			StartWalkStand(pnum);
		} else {
			StartStand(pnum, plr[pnum]._pVar3);
		}

		ClearPlrPVars(pnum);

		//Reset the "sub-tile" position of the player's light entry to 0
		if (leveltype != DTYPE_TOWN) {
			ChangeLightOff(plr[pnum]._plid, 0, 0);
		}

		return true;
	} else { //We didn't reach new tile so update player's "sub-tile" position
		PM_ChangeOffset(pnum);
		return false;
	}
}

static bool WeaponDurDecay(int pnum, int ii)
{
	if (plr[pnum].InvBody[ii]._itype != ITYPE_NONE && plr[pnum].InvBody[ii]._iClass == ICLASS_WEAPON && plr[pnum].InvBody[ii]._iDamAcFlags & 2) {
		plr[pnum].InvBody[ii]._iPLDam -= 5;
		if (plr[pnum].InvBody[ii]._iPLDam <= -100) {
			NetSendCmdDelItem(TRUE, ii);
			plr[pnum].InvBody[ii]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return true;
		}
		CalcPlrInv(pnum, TRUE);
	}
	return false;
}

BOOL WeaponDur(int pnum, int durrnd)
{
	if (pnum != myplr) {
		return FALSE;
	}

	if (WeaponDurDecay(pnum, INVLOC_HAND_LEFT))
		return TRUE;
	if (WeaponDurDecay(pnum, INVLOC_HAND_RIGHT))
		return TRUE;

	if (random_(3, durrnd) != 0) {
		return FALSE;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("WeaponDur: illegal player %d", pnum);
	}

	if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON) {
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability <= 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON) {
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
			plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
			plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PlrHitMonst(int pnum, int m)
{
	BOOL rv, ret;
	int hit, hper, mind, maxd, ddp, dam, skdam, phanditype, tmac;
	hper = 0;
	ret = FALSE;
	BOOL adjacentDamage = FALSE;

	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("PlrHitMonst: illegal monster %d", m);
	}

	if ((monster[m]._mhitpoints >> 6) <= 0) {
		return FALSE;
	}

	if (monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MGOAL_RETREAT) {
		return FALSE;
	}

	if (monster[m]._mmode == MM_CHARGE) {
		return FALSE;
	}

	if (pnum < 0) {
		adjacentDamage = TRUE;
		pnum = -pnum;
		if (plr[pnum]._pLevel > 20)
			hper -= 30;
		else
			hper -= (35 - plr[pnum]._pLevel) * 2;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrHitMonst: illegal player %d", pnum);
	}

	rv = FALSE;

	hit = random_(4, 100);
	if (monster[m]._mmode == MM_STONE) {
		hit = 0;
	}

	tmac = monster[m].mArmorClass;
	if (gbIsHellfire && plr[pnum]._pIEnAc > 0) {
		int _pIEnAc = plr[pnum]._pIEnAc - 1;
		if (_pIEnAc > 0)
			tmac >>= _pIEnAc;
		else
			tmac -= tmac >> 2;

		if (plr[pnum]._pClass == PC_BARBARIAN) {
			tmac -= monster[m].mArmorClass / 8;
		}

		if (tmac < 0)
			tmac = 0;
	} else {
		tmac -= plr[pnum]._pIEnAc;
	}

	hper += (plr[pnum]._pDexterity >> 1) + plr[pnum]._pLevel + 50 - tmac;
	if (plr[pnum]._pClass == PC_WARRIOR) {
		hper += 20;
	}
	hper += plr[pnum]._pIBonusToHit;
	if (hper < 5) {
		hper = 5;
	}
	if (hper > 95) {
		hper = 95;
	}

	if (CheckMonsterHit(m, &ret)) {
		return ret;
	}
#ifdef _DEBUG
	if (hit < hper || debug_mode_key_inverted_v || debug_mode_dollar_sign) {
#else
	if (hit < hper) {
#endif
		if (plr[pnum]._pIFlags & ISPL_FIREDAM && plr[pnum]._pIFlags & ISPL_LIGHTDAM) {
			int midam = plr[pnum]._pIFMinDam + random_(3, plr[pnum]._pIFMaxDam - plr[pnum]._pIFMinDam);
			AddMissile(plr[pnum]._px, plr[pnum]._py, plr[pnum]._pVar1, plr[pnum]._pVar2, plr[pnum]._pdir, MIS_SPECARROW, TARGET_MONSTERS, pnum, midam, 0);
		}
		mind = plr[pnum]._pIMinDam;
		maxd = plr[pnum]._pIMaxDam;
		dam = random_(5, maxd - mind + 1) + mind;
		dam += dam * plr[pnum]._pIBonusDam / 100;
		dam += plr[pnum]._pIBonusDamMod;
		int dam2 = dam << 6;
		dam += plr[pnum]._pDamageMod;
		if (plr[pnum]._pClass == PC_WARRIOR || plr[pnum]._pClass == PC_BARBARIAN) {
			ddp = plr[pnum]._pLevel;
			if (random_(6, 100) < ddp) {
				dam <<= 1;
			}
		}

		phanditype = ITYPE_NONE;
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD) {
			phanditype = ITYPE_SWORD;
		}
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_MACE) {
			phanditype = ITYPE_MACE;
		}

		switch (monster[m].MData->mMonstClass) {
		case MC_UNDEAD:
			if (phanditype == ITYPE_SWORD) {
				dam -= dam >> 1;
			} else if (phanditype == ITYPE_MACE) {
				dam += dam >> 1;
			}
			break;
		case MC_ANIMAL:
			if (phanditype == ITYPE_MACE) {
				dam -= dam >> 1;
			} else if (phanditype == ITYPE_SWORD) {
				dam += dam >> 1;
			}
			break;
		}

		if (plr[pnum]._pIFlags & ISPL_3XDAMVDEM && monster[m].MData->mMonstClass == MC_DEMON) {
			dam *= 3;
		}

		if (plr[pnum].pDamAcFlags & 0x01 && random_(6, 100) < 5) {
			dam *= 3;
		}

		if (plr[pnum].pDamAcFlags & 0x10 && monster[m].MType->mtype != MT_DIABLO && monster[m]._uniqtype == 0 && random_(6, 100) < 10) {
			monster_43C785(m);
		}

		dam <<= 6;
		if (plr[pnum].pDamAcFlags & 0x08) {
			int r = random_(6, 201);
			if (r >= 100)
				r = 100 + (r - 100) * 5;
			dam = dam * r / 100;
		}

		if (adjacentDamage)
			dam >>= 2;

		if (pnum == myplr) {
			if (plr[pnum].pDamAcFlags & 0x04) {
				dam2 += plr[pnum]._pIGetHit << 6;
				if (dam2 >= 0) {
					if (plr[pnum]._pHitPoints > dam2) {
						plr[pnum]._pHitPoints -= dam2;
						plr[pnum]._pHPBase -= dam2;
					} else {
						dam2 = (1 << 6);
						plr[pnum]._pHPBase -= plr[pnum]._pHitPoints - dam2;
						plr[pnum]._pHitPoints = dam2;
					}
				}
				dam <<= 1;
			}
			monster[m]._mhitpoints -= dam;
		}

		if (plr[pnum]._pIFlags & ISPL_RNDSTEALLIFE) {
			skdam = random_(7, dam >> 3);
			plr[pnum]._pHitPoints += skdam;
			if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
				plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
			}
			plr[pnum]._pHPBase += skdam;
			if (plr[pnum]._pHPBase > plr[pnum]._pMaxHPBase) {
				plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			}
			drawhpflag = TRUE;
		}
		if (plr[pnum]._pIFlags & (ISPL_STEALMANA_3 | ISPL_STEALMANA_5) && !(plr[pnum]._pIFlags & ISPL_NOMANA)) {
			if (plr[pnum]._pIFlags & ISPL_STEALMANA_3) {
				skdam = 3 * dam / 100;
			}
			if (plr[pnum]._pIFlags & ISPL_STEALMANA_5) {
				skdam = 5 * dam / 100;
			}
			plr[pnum]._pMana += skdam;
			if (plr[pnum]._pMana > plr[pnum]._pMaxMana) {
				plr[pnum]._pMana = plr[pnum]._pMaxMana;
			}
			plr[pnum]._pManaBase += skdam;
			if (plr[pnum]._pManaBase > plr[pnum]._pMaxManaBase) {
				plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
			}
			drawmanaflag = TRUE;
		}
		if (plr[pnum]._pIFlags & (ISPL_STEALLIFE_3 | ISPL_STEALLIFE_5)) {
			if (plr[pnum]._pIFlags & ISPL_STEALLIFE_3) {
				skdam = 3 * dam / 100;
			}
			if (plr[pnum]._pIFlags & ISPL_STEALLIFE_5) {
				skdam = 5 * dam / 100;
			}
			plr[pnum]._pHitPoints += skdam;
			if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
				plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
			}
			plr[pnum]._pHPBase += skdam;
			if (plr[pnum]._pHPBase > plr[pnum]._pMaxHPBase) {
				plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			}
			drawhpflag = TRUE;
		}
		if (plr[pnum]._pIFlags & ISPL_NOHEALPLR) {
			monster[m]._mFlags |= MFLAG_NOHEAL;
		}
#ifdef _DEBUG
		if (debug_mode_dollar_sign || debug_mode_key_inverted_v) {
			monster[m]._mhitpoints = 0; /* double check */
		}
#endif
		if ((monster[m]._mhitpoints >> 6) <= 0) {
			if (monster[m]._mmode == MM_STONE) {
				M_StartKill(m, pnum);
				monster[m]._mmode = MM_STONE;
			} else {
				M_StartKill(m, pnum);
			}
		} else {
			if (monster[m]._mmode == MM_STONE) {
				M_StartHit(m, pnum, dam);
				monster[m]._mmode = MM_STONE;
			} else {
				if (plr[pnum]._pIFlags & ISPL_KNOCKBACK) {
					M_GetKnockback(m);
				}
				M_StartHit(m, pnum, dam);
			}
		}
		rv = TRUE;
	}

	return rv;
}

BOOL PlrHitPlr(int pnum, char p)
{
	BOOL rv;
	int hit, hper, blk, blkper, dir, mind, maxd, dam, lvl, skdam, tac;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("PlrHitPlr: illegal target player %d", p);
	}

	rv = FALSE;

	if (plr[p]._pInvincible) {
		return rv;
	}

	if (plr[p]._pSpellFlags & 1) {
		return rv;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrHitPlr: illegal attacking player %d", pnum);
	}

	hit = random_(4, 100);

	hper = (plr[pnum]._pDexterity >> 1) + plr[pnum]._pLevel + 50 - (plr[p]._pIBonusAC + plr[p]._pIAC + plr[p]._pDexterity / 5);

	if (plr[pnum]._pClass == PC_WARRIOR) {
		hper += 20;
	}
	hper += plr[pnum]._pIBonusToHit;
	if (hper < 5) {
		hper = 5;
	}
	if (hper > 95) {
		hper = 95;
	}

	if ((plr[p]._pmode == PM_STAND || plr[p]._pmode == PM_ATTACK) && plr[p]._pBlockFlag) {
		blk = random_(5, 100);
	} else {
		blk = 100;
	}

	blkper = plr[p]._pDexterity + plr[p]._pBaseToBlk + (plr[p]._pLevel << 1) - (plr[pnum]._pLevel << 1);
	if (blkper < 0) {
		blkper = 0;
	}
	if (blkper > 100) {
		blkper = 100;
	}

	if (hit < hper) {
		if (blk < blkper) {
			dir = GetDirection(plr[p]._px, plr[p]._py, plr[pnum]._px, plr[pnum]._py);
			StartPlrBlock(p, dir);
		} else {
			mind = plr[pnum]._pIMinDam;
			maxd = plr[pnum]._pIMaxDam;
			dam = random_(5, maxd - mind + 1) + mind;
			dam += (dam * plr[pnum]._pIBonusDam) / 100;
			dam += plr[pnum]._pIBonusDamMod + plr[pnum]._pDamageMod;

			if (plr[pnum]._pClass == PC_WARRIOR || plr[pnum]._pClass == PC_BARBARIAN) {
				lvl = plr[pnum]._pLevel;
				if (random_(6, 100) < lvl) {
					dam <<= 1;
				}
			}
			skdam = dam << 6;
			if (plr[pnum]._pIFlags & ISPL_RNDSTEALLIFE) {
				tac = random_(7, skdam >> 3);
				plr[pnum]._pHitPoints += tac;
				if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
					plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
				}
				plr[pnum]._pHPBase += tac;
				if (plr[pnum]._pHPBase > plr[pnum]._pMaxHPBase) {
					plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
				}
				drawhpflag = TRUE;
			}
			if (pnum == myplr) {
				NetSendCmdDamage(TRUE, p, skdam);
			}
			StartPlrHit(p, skdam, FALSE);
		}

		rv = TRUE;
	}

	return rv;
}

BOOL PlrHitObj(int pnum, int mx, int my)
{
	int oi;

	if (dObject[mx][my] > 0) {
		oi = dObject[mx][my] - 1;
	} else {
		oi = -dObject[mx][my] - 1;
	}

	if (object[oi]._oBreak == 1) {
		BreakObject(pnum, oi);
		return TRUE;
	}

	return FALSE;
}

BOOL PM_DoAttack(int pnum)
{
	int frame, dir, dx, dy, m;
	BOOL didhit = FALSE;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoAttack: illegal player %d", pnum);
	}

	frame = plr[pnum]._pAnimFrame;
	if (plr[pnum]._pIFlags & ISPL_QUICKATTACK && frame == 1) {
		plr[pnum]._pAnimFrame++;
	}
	if (plr[pnum]._pIFlags & ISPL_FASTATTACK && (frame == 1 || frame == 3)) {
		plr[pnum]._pAnimFrame++;
	}
	if (plr[pnum]._pIFlags & ISPL_FASTERATTACK && (frame == 1 || frame == 3 || frame == 5)) {
		plr[pnum]._pAnimFrame++;
	}
	if (plr[pnum]._pIFlags & ISPL_FASTESTATTACK && (frame == 1 || frame == 4)) {
		plr[pnum]._pAnimFrame += 2;
	}
	if (plr[pnum]._pAnimFrame == plr[pnum]._pAFNum - 1) {
		PlaySfxLoc(PS_SWING, plr[pnum]._px, plr[pnum]._py);
	}

	if (plr[pnum]._pAnimFrame == plr[pnum]._pAFNum) {
		dx = plr[pnum]._px + offset_x[plr[pnum]._pdir];
		dy = plr[pnum]._py + offset_y[plr[pnum]._pdir];

		if (dMonster[dx][dy] != 0) {
			if (dMonster[dx][dy] > 0) {
				m = dMonster[dx][dy] - 1;
			} else {
				m = -(dMonster[dx][dy] + 1);
			}
			if (CanTalkToMonst(m)) {
				plr[pnum]._pVar1 = 0;
				return FALSE;
			}
		}

		if (!(plr[pnum]._pIFlags & ISPL_FIREDAM) || !(plr[pnum]._pIFlags & ISPL_LIGHTDAM)) {
			if (plr[pnum]._pIFlags & ISPL_FIREDAM) {
				AddMissile(dx, dy, 1, 0, 0, MIS_WEAPEXP, TARGET_MONSTERS, pnum, 0, 0);
			} else if (plr[pnum]._pIFlags & ISPL_LIGHTDAM) {
				AddMissile(dx, dy, 2, 0, 0, MIS_WEAPEXP, TARGET_MONSTERS, pnum, 0, 0);
			}
		}

		if (dMonster[dx][dy]) {
			m = dMonster[dx][dy];
			if (dMonster[dx][dy] > 0) {
				m = dMonster[dx][dy] - 1;
			} else {
				m = -(dMonster[dx][dy] + 1);
			}
			didhit = PlrHitMonst(pnum, m);
		} else if (dPlayer[dx][dy] != 0 && !FriendlyMode) {
			BYTE p = dPlayer[dx][dy];
			if (dPlayer[dx][dy] > 0) {
				p = dPlayer[dx][dy] - 1;
			} else {
				p = -(dPlayer[dx][dy] + 1);
			}
			didhit = PlrHitPlr(pnum, p);
		} else if (dObject[dx][dy] > 0) {
			didhit = PlrHitObj(pnum, dx, dy);
		}
		if ((plr[pnum]._pClass == PC_MONK
		        && (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_STAFF))
		    || (plr[pnum]._pClass == PC_BARD
		        && plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD)
		    || (plr[pnum]._pClass == PC_BARBARIAN
		        && (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_AXE || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_AXE
		            || (((plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND)
		                    || (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_MACE && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iLoc == ILOC_TWOHAND)
		                    || (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND)
		                    || (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iLoc == ILOC_TWOHAND))
		                && !(plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD))))) {
			dx = plr[pnum]._px + offset_x[(plr[pnum]._pdir + 1) % 8];
			dy = plr[pnum]._py + offset_y[(plr[pnum]._pdir + 1) % 8];
			m = ((dMonster[dx][dy] > 0) ? dMonster[dx][dy] : -dMonster[dx][dy]) - 1;
			if (dMonster[dx][dy] != 0 && !CanTalkToMonst(m) && monster[m]._moldx == dx && monster[m]._moldy == dy) {
				if (PlrHitMonst(-pnum, m))
					didhit = TRUE;
			}
			dx = plr[pnum]._px + offset_x[(plr[pnum]._pdir + 7) % 8];
			dy = plr[pnum]._py + offset_y[(plr[pnum]._pdir + 7) % 8];
			m = ((dMonster[dx][dy] > 0) ? dMonster[dx][dy] : -dMonster[dx][dy]) - 1;
			if (dMonster[dx][dy] != 0 && !CanTalkToMonst(m) && monster[m]._moldx == dx && monster[m]._moldy == dy) {
				if (PlrHitMonst(-pnum, m))
					didhit = TRUE;
			}
		}

		if (didhit && WeaponDur(pnum, 30)) {
			StartStand(pnum, plr[pnum]._pdir);
			ClearPlrPVars(pnum);
			return TRUE;
		}
	}

	if (plr[pnum]._pAnimFrame == plr[pnum]._pAFrames) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL PM_DoRangeAttack(int pnum)
{
	int origFrame, mistype;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoRangeAttack: illegal player %d", pnum);
	}

	origFrame = plr[pnum]._pAnimFrame;
	if (plr[pnum]._pIFlags & ISPL_QUICKATTACK && origFrame == 1) {
		plr[pnum]._pAnimFrame++;
	}
	if (plr[pnum]._pIFlags & ISPL_FASTATTACK && (origFrame == 1 || origFrame == 3)) {
		plr[pnum]._pAnimFrame++;
	}

	if (plr[pnum]._pAnimFrame == plr[pnum]._pAFNum) {
		mistype = MIS_ARROW;
		if (plr[pnum]._pIFlags & ISPL_FIRE_ARROWS) {
			mistype = MIS_FARROW;
		}
		if (plr[pnum]._pIFlags & ISPL_LIGHT_ARROWS) {
			mistype = MIS_LARROW;
		}
		AddMissile(
		    plr[pnum]._px,
		    plr[pnum]._py,
		    plr[pnum]._pVar1,
		    plr[pnum]._pVar2,
		    plr[pnum]._pdir,
		    mistype,
		    TARGET_MONSTERS,
		    pnum,
		    4,
		    0);

		PlaySfxLoc(PS_BFIRE, plr[pnum]._px, plr[pnum]._py);

		if (WeaponDur(pnum, 40)) {
			StartStand(pnum, plr[pnum]._pdir);
			ClearPlrPVars(pnum);
			return TRUE;
		}
	}

	if (plr[pnum]._pAnimFrame >= plr[pnum]._pAFrames) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		return TRUE;
	} else {
		return FALSE;
	}
}

void ShieldDur(int pnum)
{
	if (pnum != myplr) {
		return;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ShieldDur: illegal player %d", pnum);
	}

	if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return;
		}

		plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
		}
	}

	if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability != DUR_INDESTRUCTIBLE) {
			plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability--;
			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
				NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
				plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
				CalcPlrInv(pnum, TRUE);
			}
		}
	}
}

BOOL PM_DoBlock(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoBlock: illegal player %d", pnum);
	}

	if (plr[pnum]._pIFlags & ISPL_FASTBLOCK && plr[pnum]._pAnimFrame != 1) {
		plr[pnum]._pAnimFrame = plr[pnum]._pBFrames;
	}

	if (plr[pnum]._pAnimFrame >= plr[pnum]._pBFrames) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);

		if (random_(3, 10) == 0) {
			ShieldDur(pnum);
		}
		return TRUE;
	}

	return FALSE;
}

static void ArmorDur(int pnum)
{
	int a;
	ItemStruct *pi;
	PlayerStruct *p;

	if (pnum != myplr) {
		return;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ArmorDur: illegal player %d", pnum);
	}

	p = &plr[pnum];
	if (p->InvBody[INVLOC_CHEST]._itype == ITYPE_NONE && p->InvBody[INVLOC_HEAD]._itype == ITYPE_NONE) {
		return;
	}

	a = random_(8, 3);
	if (p->InvBody[INVLOC_CHEST]._itype != ITYPE_NONE && p->InvBody[INVLOC_HEAD]._itype == ITYPE_NONE) {
		a = 1;
	}
	if (p->InvBody[INVLOC_CHEST]._itype == ITYPE_NONE && p->InvBody[INVLOC_HEAD]._itype != ITYPE_NONE) {
		a = 0;
	}

	if (a != 0) {
		pi = &p->InvBody[INVLOC_CHEST];
	} else {
		pi = &p->InvBody[INVLOC_HEAD];
	}
	if (pi->_iDurability == DUR_INDESTRUCTIBLE) {
		return;
	}

	pi->_iDurability--;
	if (pi->_iDurability != 0) {
		return;
	}

	if (a != 0) {
		NetSendCmdDelItem(TRUE, INVLOC_CHEST);
	} else {
		NetSendCmdDelItem(TRUE, INVLOC_HEAD);
	}
	pi->_itype = ITYPE_NONE;
	CalcPlrInv(pnum, TRUE);
}

BOOL PM_DoSpell(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoSpell: illegal player %d", pnum);
	}

	if (plr[pnum]._pVar8 == plr[pnum]._pSFNum) {
		CastSpell(
		    pnum,
		    plr[pnum]._pSpell,
		    plr[pnum]._px,
		    plr[pnum]._py,
		    plr[pnum]._pVar1,
		    plr[pnum]._pVar2,
		    plr[pnum]._pVar4);

		if (plr[pnum]._pSplFrom == 0) {
			EnsureValidReadiedSpell(plr[pnum]);
		}
	}

	plr[pnum]._pVar8++;

	if (leveltype == DTYPE_TOWN) {
		if (plr[pnum]._pVar8 > plr[pnum]._pSFrames) {
			StartWalkStand(pnum);
			ClearPlrPVars(pnum);
			return TRUE;
		}
	} else if (plr[pnum]._pAnimFrame == plr[pnum]._pSFrames) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		return TRUE;
	}

	return FALSE;
}

BOOL PM_DoGotHit(int pnum)
{
	int frame;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoGotHit: illegal player %d", pnum);
	}

	frame = plr[pnum]._pAnimFrame;
	if (plr[pnum]._pIFlags & ISPL_FASTRECOVER && frame == 3) {
		plr[pnum]._pAnimFrame++;
	}
	if (plr[pnum]._pIFlags & ISPL_FASTERRECOVER && (frame == 3 || frame == 5)) {
		if (!gbIsHellfire || !(plr[pnum]._pIFlags & ISPL_FASTESTRECOVER))
			plr[pnum]._pAnimFrame++;
	}
	if (plr[pnum]._pIFlags & ISPL_FASTESTRECOVER && (frame == 1 || frame == 3 || frame == 5)) {
		plr[pnum]._pAnimFrame++;
	}

	if (plr[pnum]._pAnimFrame >= plr[pnum]._pHFrames) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		if (random_(3, 4) != 0) {
			ArmorDur(pnum);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL PM_DoDeath(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoDeath: illegal player %d", pnum);
	}

	if (plr[pnum]._pVar8 >= 2 * plr[pnum]._pDFrames) {
		if (deathdelay > 1 && pnum == myplr) {
			deathdelay--;
			if (deathdelay == 1) {
				deathflag = TRUE;
				if (!gbIsMultiplayer) {
					gamemenu_on();
				}
			}
		}

		plr[pnum]._pAnimDelay = 10000;
		plr[pnum]._pAnimFrame = plr[pnum]._pAnimLen;
		dFlags[plr[pnum]._px][plr[pnum]._py] |= BFLAG_DEAD_PLAYER;
	}

	if (plr[pnum]._pVar8 < 100) {
		plr[pnum]._pVar8++;
	}

	return FALSE;
}

BOOL PM_DoNewLvl(int pnum)
{
	return FALSE;
}

void CheckNewPath(int pnum)
{
	int i, x, y, d;
	int xvel3, xvel, yvel;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("CheckNewPath: illegal player %d", pnum);
	}

	if (plr[pnum].destAction == ACTION_ATTACKMON) {
		i = plr[pnum].destParam1;
		MakePlrPath(pnum, monster[i]._mfutx, monster[i]._mfuty, FALSE);
	}

	if (plr[pnum].destAction == ACTION_ATTACKPLR) {
		i = plr[pnum].destParam1;
		MakePlrPath(pnum, plr[i]._pfutx, plr[i]._pfuty, FALSE);
	}

	if (plr[pnum].walkpath[0] != WALK_NONE) {
		if (plr[pnum]._pmode == PM_STAND) {
			if (pnum == myplr) {
				if (plr[pnum].destAction == ACTION_ATTACKMON || plr[pnum].destAction == ACTION_ATTACKPLR) {
					i = plr[pnum].destParam1;

					if (plr[pnum].destAction == ACTION_ATTACKMON) {
						x = abs(plr[pnum]._pfutx - monster[i]._mfutx);
						y = abs(plr[pnum]._pfuty - monster[i]._mfuty);
						d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
					} else {
						x = abs(plr[pnum]._pfutx - plr[i]._pfutx);
						y = abs(plr[pnum]._pfuty - plr[i]._pfuty);
						d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[i]._pfutx, plr[i]._pfuty);
					}

					if (x < 2 && y < 2) {
						ClrPlrPath(pnum);
						if (monster[i].mtalkmsg && monster[i].mtalkmsg != TEXT_VILE14) {
							TalktoMonster(i);
						} else {
							StartAttack(pnum, d);
						}
						plr[pnum].destAction = ACTION_NONE;
					}
				}
			}

			if (currlevel != 0) {
				xvel3 = PWVel[plr[pnum]._pClass][0];
				xvel = PWVel[plr[pnum]._pClass][1];
				yvel = PWVel[plr[pnum]._pClass][2];
			} else {
				xvel3 = 2048;
				xvel = 1024;
				yvel = 512;
			}

			switch (plr[pnum].walkpath[0]) {
			case WALK_N:
				StartWalk(pnum, 0, -xvel, 0, 0, -1, -1, 0, 0, DIR_N, SDIR_N, PM_WALK);
				break;
			case WALK_NE:
				StartWalk(pnum, xvel, -yvel, 0, 0, 0, -1, 0, 0, DIR_NE, SDIR_NE, PM_WALK);
				break;
			case WALK_E:
				StartWalk(pnum, xvel3, 0, -32, -16, 1, -1, 1, 0, DIR_E, SDIR_E, PM_WALK3);
				break;
			case WALK_SE:
				StartWalk(pnum, xvel, yvel, -32, -16, 1, 0, 0, 0, DIR_SE, SDIR_SE, PM_WALK2);
				break;
			case WALK_S:
				StartWalk(pnum, 0, xvel, 0, -32, 1, 1, 0, 0, DIR_S, SDIR_S, PM_WALK2);
				break;
			case WALK_SW:
				StartWalk(pnum, -xvel, yvel, 32, -16, 0, 1, 0, 0, DIR_SW, SDIR_SW, PM_WALK2);
				break;
			case WALK_W:
				StartWalk(pnum, -xvel3, 0, 32, -16, -1, 1, 0, 1, DIR_W, SDIR_W, PM_WALK3);
				break;
			case WALK_NW:
				StartWalk(pnum, -xvel, -yvel, 0, 0, -1, 0, 0, 0, DIR_NW, SDIR_NW, PM_WALK);
				break;
			}

			for (i = 1; i < MAX_PATH_LENGTH; i++) {
				plr[pnum].walkpath[i - 1] = plr[pnum].walkpath[i];
			}

			plr[pnum].walkpath[MAX_PATH_LENGTH - 1] = WALK_NONE;

			if (plr[pnum]._pmode == PM_STAND) {
				StartStand(pnum, plr[pnum]._pdir);
				plr[pnum].destAction = ACTION_NONE;
			}
		}

		return;
	}
	if (plr[pnum].destAction == ACTION_NONE) {
		return;
	}

	if (plr[pnum]._pmode == PM_STAND) {
		switch (plr[pnum].destAction) {
		case ACTION_ATTACK:
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
			StartAttack(pnum, d);
			break;
		case ACTION_ATTACKMON:
			i = plr[pnum].destParam1;
			x = abs(plr[pnum]._px - monster[i]._mfutx);
			y = abs(plr[pnum]._py - monster[i]._mfuty);
			if (x <= 1 && y <= 1) {
				d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
				if (monster[i].mtalkmsg && monster[i].mtalkmsg != TEXT_VILE14) {
					TalktoMonster(i);
				} else {
					StartAttack(pnum, d);
				}
			}
			break;
		case ACTION_ATTACKPLR:
			i = plr[pnum].destParam1;
			x = abs(plr[pnum]._px - plr[i]._pfutx);
			y = abs(plr[pnum]._py - plr[i]._pfuty);
			if (x <= 1 && y <= 1) {
				d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[i]._pfutx, plr[i]._pfuty);
				StartAttack(pnum, d);
			}
			break;
		case ACTION_RATTACK:
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
			StartRangeAttack(pnum, d, plr[pnum].destParam1, plr[pnum].destParam2);
			break;
		case ACTION_RATTACKMON:
			i = plr[pnum].destParam1;
			d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
			if (monster[i].mtalkmsg && monster[i].mtalkmsg != TEXT_VILE14) {
				TalktoMonster(i);
			} else {
				StartRangeAttack(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
			}
			break;
		case ACTION_RATTACKPLR:
			i = plr[pnum].destParam1;
			d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[i]._pfutx, plr[i]._pfuty);
			StartRangeAttack(pnum, d, plr[i]._pfutx, plr[i]._pfuty);
			break;
		case ACTION_SPELL:
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
			StartSpell(pnum, d, plr[pnum].destParam1, plr[pnum].destParam2);
			plr[pnum]._pVar4 = plr[pnum].destParam3;
			break;
		case ACTION_SPELLWALL:
			StartSpell(pnum, plr[pnum].destParam3, plr[pnum].destParam1, plr[pnum].destParam2);
			plr[pnum]._pVar3 = plr[pnum].destParam3;
			plr[pnum]._pVar4 = plr[pnum].destParam4;
			break;
		case ACTION_SPELLMON:
			i = plr[pnum].destParam1;
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, monster[i]._mfutx, monster[i]._mfuty);
			StartSpell(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
			plr[pnum]._pVar4 = plr[pnum].destParam2;
			break;
		case ACTION_SPELLPLR:
			i = plr[pnum].destParam1;
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[i]._pfutx, plr[i]._pfuty);
			StartSpell(pnum, d, plr[i]._pfutx, plr[i]._pfuty);
			plr[pnum]._pVar4 = plr[pnum].destParam2;
			break;
		case ACTION_OPERATE:
			i = plr[pnum].destParam1;
			x = abs(plr[pnum]._px - object[i]._ox);
			y = abs(plr[pnum]._py - object[i]._oy);
			if (y > 1 && dObject[object[i]._ox][object[i]._oy - 1] == -(i + 1)) {
				y = abs(plr[pnum]._py - object[i]._oy + 1);
			}
			if (x <= 1 && y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(plr[pnum]._px, plr[pnum]._py, object[i]._ox, object[i]._oy);
					StartAttack(pnum, d);
				} else {
					OperateObject(pnum, i, FALSE);
				}
			}
			break;
		case ACTION_DISARM:
			i = plr[pnum].destParam1;
			x = abs(plr[pnum]._px - object[i]._ox);
			y = abs(plr[pnum]._py - object[i]._oy);
			if (y > 1 && dObject[object[i]._ox][object[i]._oy - 1] == -(i + 1)) {
				y = abs(plr[pnum]._py - object[i]._oy + 1);
			}
			if (x <= 1 && y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(plr[pnum]._px, plr[pnum]._py, object[i]._ox, object[i]._oy);
					StartAttack(pnum, d);
				} else {
					TryDisarm(pnum, i);
					OperateObject(pnum, i, FALSE);
				}
			}
			break;
		case ACTION_OPERATETK:
			i = plr[pnum].destParam1;
			if (object[i]._oBreak != 1) {
				OperateObject(pnum, i, TRUE);
			}
			break;
		case ACTION_PICKUPITEM:
			if (pnum == myplr) {
				i = plr[pnum].destParam1;
				x = abs(plr[pnum]._px - item[i]._ix);
				y = abs(plr[pnum]._py - item[i]._iy);
				if (x <= 1 && y <= 1 && pcurs == CURSOR_HAND && !item[i]._iRequest) {
					NetSendCmdGItem(TRUE, CMD_REQUESTGITEM, myplr, myplr, i);
					item[i]._iRequest = TRUE;
				}
			}
			break;
		case ACTION_PICKUPAITEM:
			if (pnum == myplr) {
				i = plr[pnum].destParam1;
				x = abs(plr[pnum]._px - item[i]._ix);
				y = abs(plr[pnum]._py - item[i]._iy);
				if (x <= 1 && y <= 1 && pcurs == CURSOR_HAND) {
					NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, myplr, myplr, i);
				}
			}
			break;
		case ACTION_TALK:
			if (pnum == myplr) {
				TalkToTowner(pnum, plr[pnum].destParam1);
			}
			break;
		}

		FixPlayerLocation(pnum, plr[pnum]._pdir);
		plr[pnum].destAction = ACTION_NONE;

		return;
	}

	if (plr[pnum]._pmode == PM_ATTACK && plr[pnum]._pAnimFrame > plr[myplr]._pAFNum) {
		if (plr[pnum].destAction == ACTION_ATTACK) {
			d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[pnum].destParam1, plr[pnum].destParam2);
			StartAttack(pnum, d);
			plr[pnum].destAction = ACTION_NONE;
		} else if (plr[pnum].destAction == ACTION_ATTACKMON) {
			i = plr[pnum].destParam1;
			x = abs(plr[pnum]._px - monster[i]._mfutx);
			y = abs(plr[pnum]._py - monster[i]._mfuty);
			if (x <= 1 && y <= 1) {
				d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
				StartAttack(pnum, d);
			}
			plr[pnum].destAction = ACTION_NONE;
		} else if (plr[pnum].destAction == ACTION_ATTACKPLR) {
			i = plr[pnum].destParam1;
			x = abs(plr[pnum]._px - plr[i]._pfutx);
			y = abs(plr[pnum]._py - plr[i]._pfuty);
			if (x <= 1 && y <= 1) {
				d = GetDirection(plr[pnum]._pfutx, plr[pnum]._pfuty, plr[i]._pfutx, plr[i]._pfuty);
				StartAttack(pnum, d);
			}
			plr[pnum].destAction = ACTION_NONE;
		} else if (plr[pnum].destAction == ACTION_OPERATE) {
			i = plr[pnum].destParam1;
			x = abs(plr[pnum]._px - object[i]._ox);
			y = abs(plr[pnum]._py - object[i]._oy);
			if (y > 1 && dObject[object[i]._ox][object[i]._oy - 1] == -(i + 1)) {
				y = abs(plr[pnum]._py - object[i]._oy + 1);
			}
			if (x <= 1 && y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(plr[pnum]._px, plr[pnum]._py, object[i]._ox, object[i]._oy);
					StartAttack(pnum, d);
				} else {
					OperateObject(pnum, i, FALSE);
				}
			}
		}
	}

	if (plr[pnum]._pmode == PM_RATTACK && plr[pnum]._pAnimFrame > plr[myplr]._pAFNum) {
		if (plr[pnum].destAction == ACTION_RATTACK) {
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
			StartRangeAttack(pnum, d, plr[pnum].destParam1, plr[pnum].destParam2);
			plr[pnum].destAction = ACTION_NONE;
		} else if (plr[pnum].destAction == ACTION_RATTACKMON) {
			i = plr[pnum].destParam1;
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, monster[i]._mfutx, monster[i]._mfuty);
			StartRangeAttack(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
			plr[pnum].destAction = ACTION_NONE;
		} else if (plr[pnum].destAction == ACTION_RATTACKPLR) {
			i = plr[pnum].destParam1;
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[i]._pfutx, plr[i]._pfuty);
			StartRangeAttack(pnum, d, plr[i]._pfutx, plr[i]._pfuty);
			plr[pnum].destAction = ACTION_NONE;
		}
	}

	if (plr[pnum]._pmode == PM_SPELL && plr[pnum]._pAnimFrame > plr[pnum]._pSFNum) {
		if (plr[pnum].destAction == ACTION_SPELL) {
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[pnum].destParam1, plr[pnum].destParam2);
			StartSpell(pnum, d, plr[pnum].destParam1, plr[pnum].destParam2);
			plr[pnum].destAction = ACTION_NONE;
		} else if (plr[pnum].destAction == ACTION_SPELLMON) {
			i = plr[pnum].destParam1;
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, monster[i]._mfutx, monster[i]._mfuty);
			StartSpell(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
			plr[pnum].destAction = ACTION_NONE;
		} else if (plr[pnum].destAction == ACTION_SPELLPLR) {
			i = plr[pnum].destParam1;
			d = GetDirection(plr[pnum]._px, plr[pnum]._py, plr[i]._pfutx, plr[i]._pfuty);
			StartSpell(pnum, d, plr[i]._pfutx, plr[i]._pfuty);
			plr[pnum].destAction = ACTION_NONE;
		}
	}
}

BOOL PlrDeathModeOK(int p)
{
	if (p != myplr) {
		return TRUE;
	}

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("PlrDeathModeOK: illegal player %d", p);
	}

	if (plr[p]._pmode == PM_DEATH) {
		return TRUE;
	} else if (plr[p]._pmode == PM_QUIT) {
		return TRUE;
	} else if (plr[p]._pmode == PM_NEWLVL) {
		return TRUE;
	}

	return FALSE;
}

void ValidatePlayer()
{
	__int64 msk;
	int gt, pc, i, b;

	msk = 0;

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("ValidatePlayer: illegal player %d", myplr);
	}
	if (plr[myplr]._pLevel > MAXCHARLEVEL - 1)
		plr[myplr]._pLevel = MAXCHARLEVEL - 1;
	if (plr[myplr]._pExperience > plr[myplr]._pNextExper)
		plr[myplr]._pExperience = plr[myplr]._pNextExper;

	gt = 0;
	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (plr[myplr].InvList[i]._itype == ITYPE_GOLD) {
			int maxGold = gbIsHellfire ? auricGold : GOLD_MAX_LIMIT;
			if (plr[myplr].InvList[i]._ivalue > maxGold) {
				plr[myplr].InvList[i]._ivalue = maxGold;
			}
			gt += plr[myplr].InvList[i]._ivalue;
		}
	}
	if (gt != plr[myplr]._pGold)
		plr[myplr]._pGold = gt;

	pc = plr[myplr]._pClass;
	if (plr[myplr]._pBaseStr > MaxStats[pc][ATTRIB_STR]) {
		plr[myplr]._pBaseStr = MaxStats[pc][ATTRIB_STR];
	}
	if (plr[myplr]._pBaseMag > MaxStats[pc][ATTRIB_MAG]) {
		plr[myplr]._pBaseMag = MaxStats[pc][ATTRIB_MAG];
	}
	if (plr[myplr]._pBaseDex > MaxStats[pc][ATTRIB_DEX]) {
		plr[myplr]._pBaseDex = MaxStats[pc][ATTRIB_DEX];
	}
	if (plr[myplr]._pBaseVit > MaxStats[pc][ATTRIB_VIT]) {
		plr[myplr]._pBaseVit = MaxStats[pc][ATTRIB_VIT];
	}

	int maxSpells = gbIsHellfire ? MAX_SPELLS : 37;
	for (b = 1; b < maxSpells; b++) {
		if (GetSpellBookLevel(b) != -1) {
			msk |= SPELLBIT(b);
			if (plr[myplr]._pSplLvl[b] > MAX_SPELL_LEVEL)
				plr[myplr]._pSplLvl[b] = MAX_SPELL_LEVEL;
		}
	}

	plr[myplr]._pMemSpells &= msk;
}

static void CheckCheatStats(int pnum)
{
	if (plr[pnum]._pStrength > 750) {
		plr[pnum]._pStrength = 750;
	}

	if (plr[pnum]._pDexterity > 750) {
		plr[pnum]._pDexterity = 750;
	}

	if (plr[pnum]._pMagic > 750) {
		plr[pnum]._pMagic = 750;
	}

	if (plr[pnum]._pVitality > 750) {
		plr[pnum]._pVitality = 750;
	}

	if (plr[pnum]._pHitPoints > 128000) {
		plr[pnum]._pHitPoints = 128000;
	}

	if (plr[pnum]._pMana > 128000) {
		plr[pnum]._pMana = 128000;
	}
}

void ProcessPlayers()
{
	int pnum;
	BOOL tplayer;

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("ProcessPlayers: illegal player %d", myplr);
	}

	if (plr[myplr].pLvlLoad > 0) {
		plr[myplr].pLvlLoad--;
	}

	if (sfxdelay > 0) {
		sfxdelay--;
		if (sfxdelay == 0) {
			switch (sfxdnum) {
			case USFX_DEFILER1:
				InitQTextMsg(286);
				break;
			case USFX_DEFILER2:
				InitQTextMsg(287);
				break;
			case USFX_DEFILER3:
				InitQTextMsg(288);
				break;
			case USFX_DEFILER4:
				InitQTextMsg(289);
				break;
			default:
				PlaySFX(sfxdnum);
			}
		}
	}

	ValidatePlayer();

	for (pnum = 0; pnum < MAX_PLRS; pnum++) {
		if (plr[pnum].plractive && currlevel == plr[pnum].plrlevel && (pnum == myplr || !plr[pnum]._pLvlChanging)) {
			CheckCheatStats(pnum);

			if (!PlrDeathModeOK(pnum) && (plr[pnum]._pHitPoints >> 6) <= 0) {
				SyncPlrKill(pnum, -1);
			}

			if (pnum == myplr) {
				if ((plr[pnum]._pIFlags & ISPL_DRAINLIFE) && currlevel != 0) {
					plr[pnum]._pHitPoints -= 4;
					plr[pnum]._pHPBase -= 4;
					if ((plr[pnum]._pHitPoints >> 6) <= 0) {
						SyncPlrKill(pnum, 0);
					}
					drawhpflag = TRUE;
				}
				if (plr[pnum]._pIFlags & ISPL_NOMANA && plr[pnum]._pManaBase > 0) {
					plr[pnum]._pManaBase -= plr[pnum]._pMana;
					plr[pnum]._pMana = 0;
					drawmanaflag = TRUE;
				}
			}

			tplayer = FALSE;
			do {
				switch (plr[pnum]._pmode) {
				case PM_STAND:
					tplayer = PM_DoStand(pnum);
					break;
				case PM_WALK:
				case PM_WALK2:
				case PM_WALK3:
					tplayer = PM_DoWalk(pnum, plr[pnum]._pmode);
					break;
				case PM_ATTACK:
					tplayer = PM_DoAttack(pnum);
					break;
				case PM_RATTACK:
					tplayer = PM_DoRangeAttack(pnum);
					break;
				case PM_BLOCK:
					tplayer = PM_DoBlock(pnum);
					break;
				case PM_SPELL:
					tplayer = PM_DoSpell(pnum);
					break;
				case PM_GOTHIT:
					tplayer = PM_DoGotHit(pnum);
					break;
				case PM_DEATH:
					tplayer = PM_DoDeath(pnum);
					break;
				case PM_NEWLVL:
					tplayer = PM_DoNewLvl(pnum);
					break;
				}
				CheckNewPath(pnum);
			} while (tplayer);

			plr[pnum]._pAnimCnt++;
			if (plr[pnum]._pAnimCnt > plr[pnum]._pAnimDelay) {
				plr[pnum]._pAnimCnt = 0;
				plr[pnum]._pAnimFrame++;
				if (plr[pnum]._pAnimFrame > plr[pnum]._pAnimLen) {
					plr[pnum]._pAnimFrame = 1;
				}
			}
		}
	}
}

void ClrPlrPath(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ClrPlrPath: illegal player %d", pnum);
	}

	memset(plr[pnum].walkpath, WALK_NONE, sizeof(plr[pnum].walkpath));
}

BOOL PosOkPlayer(int pnum, int x, int y)
{
	DWORD p;
	char bv;

	if (x < 0 || x >= MAXDUNX || y < 0 || y >= MAXDUNY)
		return FALSE;
	if (dPiece[x][y] == 0)
		return FALSE;
	if (SolidLoc(x, y))
		return FALSE;
	if (dPlayer[x][y] != 0) {
		if (dPlayer[x][y] > 0) {
			p = dPlayer[x][y] - 1;
		} else {
			p = -(dPlayer[x][y] + 1);
		}
		if (p != pnum
		    && p < MAX_PLRS
		    && plr[p]._pHitPoints != 0) {
			return FALSE;
		}
	}

	if (dMonster[x][y] != 0) {
		if (currlevel == 0) {
			return FALSE;
		}
		if (dMonster[x][y] <= 0) {
			return FALSE;
		}
		if ((monster[dMonster[x][y] - 1]._mhitpoints >> 6) > 0) {
			return FALSE;
		}
	}

	if (dObject[x][y] != 0) {
		if (dObject[x][y] > 0) {
			bv = dObject[x][y] - 1;
		} else {
			bv = -(dObject[x][y] + 1);
		}
		if (object[bv]._oSolidFlag) {
			return FALSE;
		}
	}

	return TRUE;
}

void MakePlrPath(int pnum, int xx, int yy, BOOL endspace)
{
	int path;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("MakePlrPath: illegal player %d", pnum);
	}

	plr[pnum]._ptargx = xx;
	plr[pnum]._ptargy = yy;
	if (plr[pnum]._pfutx == xx && plr[pnum]._pfuty == yy) {
		return;
	}

	path = FindPath(PosOkPlayer, pnum, plr[pnum]._pfutx, plr[pnum]._pfuty, xx, yy, plr[pnum].walkpath);
	if (!path) {
		return;
	}

	if (!endspace) {
		path--;

		switch (plr[pnum].walkpath[path]) {
		case WALK_NE:
			yy++;
			break;
		case WALK_NW:
			xx++;
			break;
		case WALK_SE:
			xx--;
			break;
		case WALK_SW:
			yy--;
			break;
		case WALK_N:
			xx++;
			yy++;
			break;
		case WALK_E:
			xx--;
			yy++;
			break;
		case WALK_S:
			xx--;
			yy--;
			break;
		case WALK_W:
			xx++;
			yy--;
			break;
		}

		plr[pnum]._ptargx = xx;
		plr[pnum]._ptargy = yy;
	}

	plr[pnum].walkpath[path] = WALK_NONE;
}

void CheckPlrSpell()
{
	BOOL addflag = FALSE;
	int rspell, sd, sl;

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("CheckPlrSpell: illegal player %d", myplr);
	}

	rspell = plr[myplr]._pRSpell;
	if (rspell == SPL_INVALID) {
		if (plr[myplr]._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR34);
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE34);
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE34);
		} else if (plr[myplr]._pClass == PC_MONK) {
			PlaySFX(PS_MONK34);
		} else if (plr[myplr]._pClass == PC_BARD) {
			PlaySFX(PS_ROGUE34);
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			PlaySFX(PS_WARR34);
		}
		return;
	}

	if (leveltype == DTYPE_TOWN && !spelldata[rspell].sTownSpell) {
		if (plr[myplr]._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR27);
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE27);
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE27);
		} else if (plr[myplr]._pClass == PC_MONK) {
			PlaySFX(PS_MONK27);
		} else if (plr[myplr]._pClass == PC_BARD) {
			PlaySFX(PS_ROGUE27);
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			PlaySFX(PS_WARR27);
		}
		return;
	}

	if (!sgbControllerActive) {
		if (pcurs != CURSOR_HAND)
			return;

		if (MouseY >= PANEL_TOP && MouseX >= PANEL_LEFT && MouseX <= RIGHT_PANEL) // inside main panel
			return;

		if (
		    ((chrflag || questlog) && MouseX < SPANEL_WIDTH && MouseY < SPANEL_HEIGHT)    // inside left panel
		    || ((invflag || sbookflag) && MouseX > RIGHT_PANEL && MouseY < SPANEL_HEIGHT) // inside right panel
		) {
			if (rspell != SPL_HEAL
			    && rspell != SPL_IDENTIFY
			    && rspell != SPL_REPAIR
			    && rspell != SPL_INFRA
			    && rspell != SPL_RECHARGE)
				return;
		}
	}

	switch (plr[myplr]._pRSplType) {
	case RSPLTYPE_SKILL:
	case RSPLTYPE_SPELL:
		addflag = CheckSpell(myplr, rspell, plr[myplr]._pRSplType, FALSE);
		break;
	case RSPLTYPE_SCROLL:
		addflag = UseScroll();
		break;
	case RSPLTYPE_CHARGES:
		addflag = UseStaff();
		break;
	}

	if (addflag) {
		if (plr[myplr]._pRSpell == SPL_FIREWALL || plr[myplr]._pRSpell == SPL_LIGHTWALL) {
			sd = GetDirection(plr[myplr]._px, plr[myplr]._py, cursmx, cursmy);
			sl = GetSpellLevel(myplr, plr[myplr]._pRSpell);
			NetSendCmdLocParam3(TRUE, CMD_SPELLXYD, cursmx, cursmy, plr[myplr]._pRSpell, sd, sl);
		} else if (pcursmonst != -1) {
			sl = GetSpellLevel(myplr, plr[myplr]._pRSpell);
			NetSendCmdParam3(TRUE, CMD_SPELLID, pcursmonst, plr[myplr]._pRSpell, sl);
		} else if (pcursplr != -1) {
			sl = GetSpellLevel(myplr, plr[myplr]._pRSpell);
			NetSendCmdParam3(TRUE, CMD_SPELLPID, pcursplr, plr[myplr]._pRSpell, sl);
		} else { //145
			sl = GetSpellLevel(myplr, plr[myplr]._pRSpell);
			NetSendCmdLocParam2(TRUE, CMD_SPELLXY, cursmx, cursmy, plr[myplr]._pRSpell, sl);
		}
		return;
	}

	if (plr[myplr]._pRSplType == RSPLTYPE_SPELL) {
		if (plr[myplr]._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR35);
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE35);
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE35);
		} else if (plr[myplr]._pClass == PC_MONK) {
			PlaySFX(PS_MONK35);
		} else if (plr[myplr]._pClass == PC_BARD) {
			PlaySFX(PS_ROGUE35);
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			PlaySFX(PS_WARR35);
		}
	}
}

void SyncPlrAnim(int pnum)
{
	int dir, sType;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SyncPlrAnim: illegal player %d", pnum);
	}

	dir = plr[pnum]._pdir;
	switch (plr[pnum]._pmode) {
	case PM_STAND:
		plr[pnum]._pAnimData = plr[pnum]._pNAnim[dir];
		break;
	case PM_WALK:
	case PM_WALK2:
	case PM_WALK3:
		plr[pnum]._pAnimData = plr[pnum]._pWAnim[dir];
		break;
	case PM_ATTACK:
		plr[pnum]._pAnimData = plr[pnum]._pAAnim[dir];
		break;
	case PM_RATTACK:
		plr[pnum]._pAnimData = plr[pnum]._pAAnim[dir];
		break;
	case PM_BLOCK:
		plr[pnum]._pAnimData = plr[pnum]._pBAnim[dir];
		break;
	case PM_SPELL:
		if (pnum == myplr)
			sType = spelldata[plr[pnum]._pSpell].sType;
		else
			sType = STYPE_FIRE;
		if (sType == STYPE_FIRE)
			plr[pnum]._pAnimData = plr[pnum]._pFAnim[dir];
		if (sType == STYPE_LIGHTNING)
			plr[pnum]._pAnimData = plr[pnum]._pLAnim[dir];
		if (sType == STYPE_MAGIC)
			plr[pnum]._pAnimData = plr[pnum]._pTAnim[dir];
		break;
	case PM_GOTHIT:
		plr[pnum]._pAnimData = plr[pnum]._pHAnim[dir];
		break;
	case PM_NEWLVL:
		plr[pnum]._pAnimData = plr[pnum]._pNAnim[dir];
		break;
	case PM_DEATH:
		plr[pnum]._pAnimData = plr[pnum]._pDAnim[dir];
		break;
	case PM_QUIT:
		plr[pnum]._pAnimData = plr[pnum]._pNAnim[dir];
		break;
	default:
		app_fatal("SyncPlrAnim");
		break;
	}
}

void SyncInitPlrPos(int pnum)
{
	int x, y, xx, yy, range;
	DWORD i;
	BOOL posOk;

	plr[pnum]._ptargx = plr[pnum]._px;
	plr[pnum]._ptargy = plr[pnum]._py;

	if (!gbIsMultiplayer || plr[pnum].plrlevel != currlevel) {
		return;
	}

	for (i = 0; i < 8; i++) {
		x = plr[pnum]._px + plrxoff2[i];
		y = plr[pnum]._py + plryoff2[i];
		if (PosOkPlayer(pnum, x, y)) {
			break;
		}
	}

	if (!PosOkPlayer(pnum, x, y)) {
		posOk = FALSE;
		for (range = 1; range < 50 && !posOk; range++) {
			for (yy = -range; yy <= range && !posOk; yy++) {
				y = yy + plr[pnum]._py;
				for (xx = -range; xx <= range && !posOk; xx++) {
					x = xx + plr[pnum]._px;
					if (PosOkPlayer(pnum, x, y) && !PosOkPortal(currlevel, x, y)) {
						posOk = TRUE;
					}
				}
			}
		}
	}

	plr[pnum]._px = x;
	plr[pnum]._py = y;
	dPlayer[x][y] = pnum + 1;

	if (pnum == myplr) {
		plr[pnum]._pfutx = x;
		plr[pnum]._pfuty = y;
		plr[pnum]._ptargx = x;
		plr[pnum]._ptargy = y;
		ViewX = x;
		ViewY = y;
	}
}

void SyncInitPlr(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SyncInitPlr: illegal player %d", pnum);
	}

	SetPlrAnims(pnum);
	SyncInitPlrPos(pnum);
}

void CheckStats(int p)
{
	int c, i;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("CheckStats: illegal player %d", p);
	}

	if (plr[p]._pClass == PC_WARRIOR) {
		c = PC_WARRIOR;
	} else if (plr[p]._pClass == PC_ROGUE) {
		c = PC_ROGUE;
	} else if (plr[p]._pClass == PC_SORCERER) {
		c = PC_SORCERER;
	} else if (plr[p]._pClass == PC_MONK) {
		c = PC_MONK;
	} else if (plr[p]._pClass == PC_BARD) {
		c = PC_BARD;
	} else if (plr[p]._pClass == PC_BARBARIAN) {
		c = PC_BARBARIAN;
	}

	for (i = 0; i < 4; i++) {
		switch (i) {
		case ATTRIB_STR:
			if (plr[p]._pBaseStr > MaxStats[c][ATTRIB_STR]) {
				plr[p]._pBaseStr = MaxStats[c][ATTRIB_STR];
			} else if (plr[p]._pBaseStr < 0) {
				plr[p]._pBaseStr = 0;
			}
			break;
		case ATTRIB_MAG:
			if (plr[p]._pBaseMag > MaxStats[c][ATTRIB_MAG]) {
				plr[p]._pBaseMag = MaxStats[c][ATTRIB_MAG];
			} else if (plr[p]._pBaseMag < 0) {
				plr[p]._pBaseMag = 0;
			}
			break;
		case ATTRIB_DEX:
			if (plr[p]._pBaseDex > MaxStats[c][ATTRIB_DEX]) {
				plr[p]._pBaseDex = MaxStats[c][ATTRIB_DEX];
			} else if (plr[p]._pBaseDex < 0) {
				plr[p]._pBaseDex = 0;
			}
			break;
		case ATTRIB_VIT:
			if (plr[p]._pBaseVit > MaxStats[c][ATTRIB_VIT]) {
				plr[p]._pBaseVit = MaxStats[c][ATTRIB_VIT];
			} else if (plr[p]._pBaseVit < 0) {
				plr[p]._pBaseVit = 0;
			}
			break;
		}
	}
}

void ModifyPlrStr(int p, int l)
{
	int max;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrStr: illegal player %d", p);
	}

	max = MaxStats[plr[p]._pClass][ATTRIB_STR];
	if (plr[p]._pBaseStr + l > max) {
		l = max - plr[p]._pBaseStr;
	}

	plr[p]._pStrength += l;
	plr[p]._pBaseStr += l;

	if (plr[p]._pClass == PC_ROGUE) {
		plr[p]._pDamageMod = plr[p]._pLevel * (plr[p]._pStrength + plr[p]._pDexterity) / 200;
	} else {
		plr[p]._pDamageMod = plr[p]._pLevel * plr[p]._pStrength / 100;
	}

	CalcPlrInv(p, TRUE);

	if (p == myplr) {
		NetSendCmdParam1(FALSE, CMD_SETSTR, plr[p]._pBaseStr);
	}
}

void ModifyPlrMag(int p, int l)
{
	int max, ms;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrMag: illegal player %d", p);
	}

	max = MaxStats[plr[p]._pClass][ATTRIB_MAG];
	if (plr[p]._pBaseMag + l > max) {
		l = max - plr[p]._pBaseMag;
	}

	plr[p]._pMagic += l;
	plr[p]._pBaseMag += l;

	ms = l << 6;
	if (plr[p]._pClass == PC_SORCERER) {
		ms <<= 1;
	} else if (plr[p]._pClass == PC_BARD) {
		ms += ms >> 1;
	}

	plr[p]._pMaxManaBase += ms;
	plr[p]._pMaxMana += ms;
	if (!(plr[p]._pIFlags & ISPL_NOMANA)) {
		plr[p]._pManaBase += ms;
		plr[p]._pMana += ms;
	}

	CalcPlrInv(p, TRUE);

	if (p == myplr) {
		NetSendCmdParam1(FALSE, CMD_SETMAG, plr[p]._pBaseMag);
	}
}

void ModifyPlrDex(int p, int l)
{
	int max;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrDex: illegal player %d", p);
	}

	max = MaxStats[plr[p]._pClass][ATTRIB_DEX];
	if (plr[p]._pBaseDex + l > max) {
		l = max - plr[p]._pBaseDex;
	}

	plr[p]._pDexterity += l;
	plr[p]._pBaseDex += l;
	CalcPlrInv(p, TRUE);

	if (plr[p]._pClass == PC_ROGUE) {
		plr[p]._pDamageMod = plr[p]._pLevel * (plr[p]._pDexterity + plr[p]._pStrength) / 200;
	}

	if (p == myplr) {
		NetSendCmdParam1(FALSE, CMD_SETDEX, plr[p]._pBaseDex);
	}
}

void ModifyPlrVit(int p, int l)
{
	int max, ms;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrVit: illegal player %d", p);
	}

	max = MaxStats[plr[p]._pClass][ATTRIB_VIT];
	if (plr[p]._pBaseVit + l > max) {
		l = max - plr[p]._pBaseVit;
	}

	plr[p]._pVitality += l;
	plr[p]._pBaseVit += l;

	ms = l << 6;
	if (plr[p]._pClass == PC_WARRIOR) {
		ms <<= 1;
	} else if (plr[p]._pClass == PC_BARBARIAN) {
		ms <<= 1;
	}

	plr[p]._pHPBase += ms;
	plr[p]._pMaxHPBase += ms;
	plr[p]._pHitPoints += ms;
	plr[p]._pMaxHP += ms;

	CalcPlrInv(p, TRUE);

	if (p == myplr) {
		NetSendCmdParam1(FALSE, CMD_SETVIT, plr[p]._pBaseVit);
	}
}

void SetPlayerHitPoints(int pnum, int val)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SetPlayerHitPoints: illegal player %d", pnum);
	}

	plr[pnum]._pHitPoints = val;
	plr[pnum]._pHPBase = val + plr[pnum]._pMaxHPBase - plr[pnum]._pMaxHP;

	if (pnum == myplr) {
		drawhpflag = TRUE;
	}
}

void SetPlrStr(int p, int v)
{
	int dm;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrStr: illegal player %d", p);
	}

	plr[p]._pBaseStr = v;
	CalcPlrInv(p, TRUE);

	if (plr[p]._pClass == PC_ROGUE) {
		dm = plr[p]._pLevel * (plr[p]._pStrength + plr[p]._pDexterity) / 200;
	} else {
		dm = plr[p]._pLevel * plr[p]._pStrength / 100;
	}

	plr[p]._pDamageMod = dm;
}

void SetPlrMag(int p, int v)
{
	int m;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrMag: illegal player %d", p);
	}

	plr[p]._pBaseMag = v;

	m = v << 6;
	if (plr[p]._pClass == PC_SORCERER) {
		m <<= 1;
	} else if (plr[p]._pClass == PC_BARD) {
		m += m >> 1;
	}

	plr[p]._pMaxManaBase = m;
	plr[p]._pMaxMana = m;
	CalcPlrInv(p, TRUE);
}

void SetPlrDex(int p, int v)
{
	int dm;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrDex: illegal player %d", p);
	}

	plr[p]._pBaseDex = v;
	CalcPlrInv(p, TRUE);

	if (plr[p]._pClass == PC_ROGUE) {
		dm = plr[p]._pLevel * (plr[p]._pStrength + plr[p]._pDexterity) / 200;
	} else {
		dm = plr[p]._pStrength * plr[p]._pLevel / 100;
	}

	plr[p]._pDamageMod = dm;
}

void SetPlrVit(int p, int v)
{
	int hp;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrVit: illegal player %d", p);
	}

	plr[p]._pBaseVit = v;

	hp = v << 6;
	if (plr[p]._pClass == PC_WARRIOR) {
		hp <<= 1;
	} else if (plr[p]._pClass == PC_BARBARIAN) {
		hp <<= 1;
	}

	plr[p]._pHPBase = hp;
	plr[p]._pMaxHPBase = hp;
	CalcPlrInv(p, TRUE);
}

void InitDungMsgs(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitDungMsgs: illegal player %d", pnum);
	}

	plr[pnum].pDungMsgs = 0;
	plr[pnum].pDungMsgs2 = 0;
}

void PlayDungMsgs()
{
	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("PlayDungMsgs: illegal player %d", myplr);
	}

	if (currlevel == 1 && !plr[myplr]._pLvlVisited[1] && !gbIsMultiplayer && !(plr[myplr].pDungMsgs & DMSG_CATHEDRAL)) {
		sfxdelay = 40;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR97;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE97;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE97;
		} else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK97;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE97;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR97;
		}
		plr[myplr].pDungMsgs = plr[myplr].pDungMsgs | DMSG_CATHEDRAL;
	} else if (currlevel == 5 && !plr[myplr]._pLvlVisited[5] && !gbIsMultiplayer && !(plr[myplr].pDungMsgs & DMSG_CATACOMBS)) {
		sfxdelay = 40;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR96B;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE96;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE96;
		} else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK96;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE96;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR96B;
		}
		plr[myplr].pDungMsgs |= DMSG_CATACOMBS;
	} else if (currlevel == 9 && !plr[myplr]._pLvlVisited[9] && !gbIsMultiplayer && !(plr[myplr].pDungMsgs & DMSG_CAVES)) {
		sfxdelay = 40;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR98;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE98;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE98;
		} else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK98;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE98;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR98;
		}
		plr[myplr].pDungMsgs |= DMSG_CAVES;
	} else if (currlevel == 13 && !plr[myplr]._pLvlVisited[13] && !gbIsMultiplayer && !(plr[myplr].pDungMsgs & DMSG_HELL)) {
		sfxdelay = 40;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR99;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE99;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE99;
		} else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK99;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE99;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR99;
		}
		plr[myplr].pDungMsgs |= DMSG_HELL;
	} else if (currlevel == 16 && !plr[myplr]._pLvlVisited[15] && !gbIsMultiplayer && !(plr[myplr].pDungMsgs & DMSG_DIABLO)) { // BUGFIX: _pLvlVisited should check 16 or this message will never play
		sfxdelay = 40;
		if (plr[myplr]._pClass == PC_WARRIOR || plr[myplr]._pClass == PC_ROGUE || plr[myplr]._pClass == PC_SORCERER || plr[myplr]._pClass == PC_MONK || plr[myplr]._pClass == PC_BARD || plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_DIABLVLINT;
		}
		plr[myplr].pDungMsgs |= DMSG_DIABLO;
	} else if (currlevel == 17 && !plr[myplr]._pLvlVisited[17] && !gbIsMultiplayer && !(plr[myplr].pDungMsgs2 & 1)) {
		sfxdelay = 10;
		sfxdnum = USFX_DEFILER1;
		quests[Q_DEFILER]._qactive = 2;
		quests[Q_DEFILER]._qlog = 1;
		quests[Q_DEFILER]._qmsg = 286;
		plr[myplr].pDungMsgs2 |= 1;
	} else if (currlevel == 19 && !plr[myplr]._pLvlVisited[19] && !gbIsMultiplayer && !(plr[myplr].pDungMsgs2 & 4)) {
		sfxdelay = 10;
		sfxdnum = USFX_DEFILER3;
		plr[myplr].pDungMsgs2 |= 4;
	} else if (currlevel == 21 && !plr[myplr]._pLvlVisited[21] && !gbIsMultiplayer && !(plr[myplr].pDungMsgs & 32)) {
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR92;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE92;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE92;
		} else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK92;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE92;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR92;
		}
		plr[myplr].pDungMsgs |= 32;
	} else {
		sfxdelay = 0;
	}
}

int get_max_strength(int i)
{
	return MaxStats[i][ATTRIB_STR];
}

int get_max_magic(int i)
{
	return MaxStats[i][ATTRIB_MAG];
}

int get_max_dexterity(int i)
{
	return MaxStats[i][ATTRIB_DEX];
}

DEVILUTION_END_NAMESPACE
