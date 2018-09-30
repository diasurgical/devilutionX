//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int plr_lframe_size; // idb
int plr_wframe_size; // idb
char plr_gfx_flag; // weak
int player_cpp_init_value; // weak
int plr_aframe_size; // idb
int myplr;
PlayerStruct plr[MAX_PLRS];
int plr_fframe_size; // idb
int plr_qframe_size; // idb
int deathflag; // idb
int plr_hframe_size; // idb
int plr_bframe_size; // idb
char plr_gfx_bflag; // weak
int plr_sframe_size; // idb
int deathdelay; // weak
int plr_dframe_size; // idb
#endif

const int player_inf = 0x7F800000; // weak
const char ArmourChar[4] = { 'L', 'M', 'H', 0 };
const char WepChar[10] = { 'N', 'U', 'S', 'D', 'B', 'A', 'M', 'H', 'T', 0 };
const char CharChar[4] = { 'W', 'R', 'S', 0 };

/* data */

int plrxoff[9] = { 0, 2, 0, 2, 1, 0, 1, 2, 1 };
int plryoff[9] = { 0, 2, 2, 0, 1, 1, 0, 1, 2 };
int plrxoff2[9] = { 0, 1, 0, 1, 2, 0, 1, 2, 2 };
int plryoff2[9] = { 0, 0, 1, 1, 0, 2, 2, 1, 2 };
char PlrGFXAnimLens[3][11] =
{
  { 10, 16, 8, 2, 20, 20, 6, 20, 8, 9, 14 },
  { 8, 18, 8, 4, 20, 16, 7, 20, 8, 10, 12 },
  { 8, 16, 8, 6, 20, 12, 8, 20, 8, 12, 8 }
};
int PWVel[4][3] = { { 2048, 1024, 512 }, { 2048, 1024, 512 }, { 2048, 1024, 512 }, { 8, 8, 8 } };
int StrengthTbl[3] = { 30, 20, 15 };
int MagicTbl[3] = { 10, 15, 35 };
int DexterityTbl[3] = { 20, 30, 15 };
int VitalityTbl[3] = { 25, 20, 20 };
int ToBlkTbl[3] = { 30, 20, 10 };
char *ClassStrTblOld[3] = { "Warrior", "Rogue", "Sorceror" }; // unused
int MaxStats[3][4] = { { 250, 50, 60, 100 }, { 55, 70, 250, 80 }, { 45, 250, 85, 80 } };
int ExpLvlsTbl[51] =
{
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
char *ClassStrTbl[3] = { "Warrior", "Rogue", "Sorceror" };
unsigned char fix[9] = { 0u, 0u, 3u, 3u, 3u, 6u, 6u, 6u, 8u }; /* PM_ChangeLightOff local type */

struct player_cpp_init
{
	player_cpp_init()
	{
		player_cpp_init_value = player_inf;
	}
} _player_cpp_init;
// 47F204: using guessed type int player_inf;
// 68643C: using guessed type int player_cpp_init_value;

void __fastcall SetPlayerGPtrs(UCHAR *pData, UCHAR **pAnim)
{
	for ( int i = 0; i < 8; i++ ) {
		pAnim[i] = pData + ((DWORD *)pData)[i];
	}
}

void __fastcall LoadPlrGFX(int pnum, player_graphic gfxflag)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("LoadPlrGFX: illegal player %d", pnum);
	}

	char prefix[16];
	char pszName[256];
	char *szCel;
	PlayerStruct *p = &plr[pnum];
	sprintf(prefix, "%c%c%c", CharChar[p->_pClass], ArmourChar[p->_pgfxnum >> 4], WepChar[p->_pgfxnum & 0xF]);
	char *cs = ClassStrTbl[p->_pClass];
	UCHAR *pData;
	UCHAR *pAnim;

	for (DWORD i = 1; i <= PFILE_NONDEATH; i <<= 1) {
		if ( !(i & gfxflag) ) {
			continue;
		}

		switch ( i ) {
			case PFILE_STAND:
				szCel = "AS";
				if ( leveltype == DTYPE_TOWN ) {
					szCel = "ST";
				}
				pData = p->_pNData;
				pAnim = (UCHAR *)p->_pNAnim;
				break;
			case PFILE_WALK:
				szCel = "AW";
				if ( leveltype == DTYPE_TOWN ) {
					szCel = "WL";
				}
				pData = p->_pWData;
				pAnim = (UCHAR *)p->_pWAnim;
				break;
			case PFILE_ATTACK:
				if ( leveltype == DTYPE_TOWN ) {
					continue;
				}
				szCel = "AT";
				pData = p->_pAData;
				pAnim = (UCHAR *)p->_pAAnim;
				break;
			case PFILE_HIT:
				if ( leveltype == DTYPE_TOWN ) {
					continue;
				}
				szCel = "HT";
				pData = p->_pHData;
				pAnim = (UCHAR *)p->_pHAnim;
				break;
			case PFILE_LIGHTNING:
				if ( leveltype == DTYPE_TOWN ) {
					continue;
				}
				szCel = "LM";
				pData = p->_pLData;
				pAnim = (UCHAR *)p->_pLAnim;
				break;
			case PFILE_FIRE:
				if ( leveltype == DTYPE_TOWN ) {
					continue;
				}
				szCel = "FM";
				pData = p->_pFData;
				pAnim = (UCHAR *)p->_pFAnim;
				break;
			case PFILE_MAGIC:
				if ( leveltype == DTYPE_TOWN ) {
					continue;
				}
				szCel = "QM";
				pData = p->_pTData;
				pAnim = (UCHAR *)p->_pTAnim;
				break;
			case PFILE_DEATH:
				if ( p->_pgfxnum & 0xF ) {
					continue;
				}
				szCel = "DT";
				pData = p->_pDData;
				pAnim = (UCHAR *)p->_pDAnim;
				break;
			case PFILE_BLOCK:
				if ( leveltype == DTYPE_TOWN ) {
					continue;
				}
				if ( !p->_pBlockFlag ) {
					continue;
				}

				szCel = "BL";
				pData = p->_pBData;
				pAnim = (UCHAR *)p->_pBAnim;
				break;
			default:
				TermMsg("PLR:2");
				break;
		}

		sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", cs, prefix, prefix, szCel);
		LoadFileWithMem(pszName, pData);
		SetPlayerGPtrs((UCHAR *)pData, (UCHAR **)pAnim);
		p->_pGFXLoad |= i;
	}
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall InitPlayerGFX(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("InitPlayerGFX: illegal player %d", pnum);
	}

	if ( plr[pnum]._pHitPoints >> 6 == 0 ) {
		plr[pnum]._pgfxnum = 0;
		LoadPlrGFX(pnum, PFILE_DEATH);
	} else {
		LoadPlrGFX(pnum, PFILE_NONDEATH);
	}
}

void __fastcall InitPlrGFXMem(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("InitPlrGFXMem: illegal player %d", pnum);
	}

	if ( !(plr_gfx_flag & 1) ) {
		plr_gfx_flag |= 1;
		if ( GetPlrGFXSize("AS") <= GetPlrGFXSize("ST") ) {
			plr_sframe_size = GetPlrGFXSize("ST");
		} else {
			plr_sframe_size = GetPlrGFXSize("AS");
		}
	}
	plr[pnum]._pNData = DiabloAllocPtr(plr_sframe_size);

	if ( !(plr_gfx_flag & 2) ) {
		plr_gfx_flag |= 2;
		if ( GetPlrGFXSize("AW") <= GetPlrGFXSize("WL") ) {
			plr_wframe_size = GetPlrGFXSize("WL");
		} else {
			plr_wframe_size = GetPlrGFXSize("AW");
		}
	}
	plr[pnum]._pWData = DiabloAllocPtr(plr_wframe_size);

	if ( !(plr_gfx_flag & 4) ) {
		plr_gfx_flag |= 4;
		plr_aframe_size = GetPlrGFXSize("AT");
	}
	plr[pnum]._pAData =  DiabloAllocPtr(plr_aframe_size);

	if ( !(plr_gfx_flag & 8) ) {
		plr_gfx_flag |= 8;
		plr_hframe_size = GetPlrGFXSize("HT");
	}
	plr[pnum]._pHData = DiabloAllocPtr(plr_hframe_size);

	if ( !(plr_gfx_flag & 0x10) ) {
		plr_gfx_flag |= 0x10;
		plr_lframe_size = GetPlrGFXSize("LM");
	}
	plr[pnum]._pLData = DiabloAllocPtr(plr_lframe_size);

	if ( !(plr_gfx_flag & 0x20) ) {
		plr_gfx_flag |= 0x20;
		plr_fframe_size = GetPlrGFXSize("FM");
	}
	plr[pnum]._pFData = DiabloAllocPtr(plr_fframe_size);

	if ( !(plr_gfx_flag & 0x40) ) {
		plr_gfx_flag |= 0x40;
		plr_qframe_size = GetPlrGFXSize("QM");
	}
	plr[pnum]._pTData = DiabloAllocPtr(plr_qframe_size);

	if ( !(plr_gfx_flag & 0x80) ) {
		plr_gfx_flag |= 0x80;
		plr_dframe_size = GetPlrGFXSize("DT");
	}
	plr[pnum]._pDData = DiabloAllocPtr(plr_dframe_size);

	if ( !(plr_gfx_bflag & 1) ) {
		plr_gfx_bflag |= 1;
		plr_bframe_size = GetPlrGFXSize("BL");
	}
	plr[pnum]._pBData = DiabloAllocPtr(plr_bframe_size);

	plr[pnum]._pGFXLoad = 0;
}
// 686438: using guessed type char plr_gfx_flag;
// 69B7BC: using guessed type char plr_gfx_bflag;

DWORD __fastcall GetPlrGFXSize(char *szCel)
{
	char prefix[16]; // [esp+10Ch] [ebp-24h]
	char pszName[256]; // [esp+Ch] [ebp-124h]
	void *file; // [esp+124h] [ebp-Ch]
	DWORD size = 0; // [esp+11Ch] [ebp-14h]
	DWORD result = 0;
	int a = 0;
	int w = 0;

	for (int c = 0; c < sizeof(ClassStrTbl) / sizeof(ClassStrTbl[0]); c++) {
		for (a = 0; ArmourChar[a]; a++) {
			for (w = 0; WepChar[w]; w++) {
				sprintf(prefix, "%c%c%c", CharChar[c], ArmourChar[a], WepChar[w]);
				sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", ClassStrTbl[c], prefix, prefix, szCel);
				if ( WOpenFile(pszName, &file, TRUE) ) {
					size = WGetFileSize(file, 0);
					WCloseFile(file);
					if ( result <= size ) {
						result = size;
					}
				}
			}
		}
	}

	return result;
}

void __fastcall FreePlayerGFX(int pnum)
{
	void *ptr;

	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("FreePlayerGFX: illegal player %d", pnum);
	}

	ptr = plr[pnum]._pNData;
	plr[pnum]._pNData = NULL;
	mem_free_dbg(ptr);
	ptr = plr[pnum]._pWData;
	plr[pnum]._pWData = NULL;
	mem_free_dbg(ptr);
	ptr = plr[pnum]._pAData;
	plr[pnum]._pAData = NULL;
	mem_free_dbg(ptr);
	ptr = plr[pnum]._pHData;
	plr[pnum]._pHData = NULL;
	mem_free_dbg(ptr);
	ptr = plr[pnum]._pLData;
	plr[pnum]._pLData = NULL;
	mem_free_dbg(ptr);
	ptr = plr[pnum]._pFData;
	plr[pnum]._pFData = NULL;
	mem_free_dbg(ptr);
	ptr = plr[pnum]._pTData;
	plr[pnum]._pTData = NULL;
	mem_free_dbg(ptr);
	ptr = plr[pnum]._pDData;
	plr[pnum]._pDData = NULL;
	mem_free_dbg(ptr);
	ptr = plr[pnum]._pBData;
	plr[pnum]._pBData = NULL;
	mem_free_dbg(ptr);
	plr[pnum]._pGFXLoad = 0;
}

void __fastcall NewPlrAnim(int pnum, unsigned char *Peq, int numFrames, int Delay, int width)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("NewPlrAnim: illegal player %d", pnum);
	}

	plr[pnum]._pAnimData = Peq;
	plr[pnum]._pAnimLen = numFrames;
	plr[pnum]._pAnimFrame = 1;
	plr[pnum]._pAnimCnt = 0;
	plr[pnum]._pAnimDelay = Delay;
	plr[pnum]._pAnimWidth = width;
	plr[pnum]._pAnimWidth2 = (width - 64) >> 1;
}

void __fastcall ClearPlrPVars(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("ClearPlrPVars: illegal player %d", pnum);
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

void __fastcall SetPlrAnims(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("SetPlrAnims: illegal player %d", pnum);
	}

	int pc = plr[pnum]._pClass;

	plr[pnum]._pNWidth = 96;
	plr[pnum]._pWWidth = 96;
	plr[pnum]._pAWidth = 128;
	plr[pnum]._pHWidth = 96;
	plr[pnum]._pSWidth = 96;
	plr[pnum]._pDWidth = 128;
	plr[pnum]._pBWidth = 96;

	if ( leveltype == DTYPE_TOWN ) {
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

	int gn = plr[pnum]._pgfxnum & 0xF;
	if ( pc == PC_WARRIOR ) {
		if ( gn == 4 ) {
			if ( leveltype != DTYPE_TOWN ) {
				plr[pnum]._pNFrames = 8;
			}
			plr[pnum]._pAWidth = 96;
			plr[pnum]._pAFNum = 11;
		} else if ( gn == 5 ) {
			plr[pnum]._pAFrames = 20;
			plr[pnum]._pAFNum = 10;
		} else if ( gn == 8 ) {
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		}
	} else if ( pc == PC_ROGUE ) {
		if ( gn == 5 ) {
			plr[pnum]._pAFrames = 22;
			plr[pnum]._pAFNum = 13;
		} else if ( gn == 4 ) {
			plr[pnum]._pAFrames = 12;
			plr[pnum]._pAFNum = 7;
		} else if ( gn == 8 ) {
			plr[pnum]._pAFrames = 16;
			plr[pnum]._pAFNum = 11;
		}
	} else if ( pc == PC_SORCERER ) {
		plr[pnum]._pSWidth = 128;
		if ( gn == 0 ) {
			plr[pnum]._pAFrames = 20;
		} else if ( gn == 1 ) {
			plr[pnum]._pAFNum = 9;
		} else if ( gn == 4 ) {
			plr[pnum]._pAFrames = 20;
			plr[pnum]._pAFNum = 16;
		} else if ( gn == 5 ) {
			plr[pnum]._pAFrames = 24;
			plr[pnum]._pAFNum = 16;
		}
	}
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall ClearPlrRVars(PlayerStruct *p)
{
	// TODO: Missing debug assert p != NULL
	p->bReserved[0] = 0;
	p->bReserved[1] = 0;
	p->bReserved[2] = 0;
	p->wReserved[0] = 0;
	p->wReserved[1] = 0;
	p->wReserved[2] = 0;
	p->wReserved[3] = 0;
	p->wReserved[4] = 0;
	p->wReserved[5] = 0;
	p->wReserved[6] = 0;
	p->wReserved[7] = 0;
	p->dwReserved[0] = 0;
	p->dwReserved[1] = 0;
	p->dwReserved[2] = 0;
	p->dwReserved[3] = 0;
	p->dwReserved[4] = 0;
	p->dwReserved[5] = 0;
	p->dwReserved[6] = 0;
}

// c: plr_classes value
void __fastcall CreatePlayer(int pnum, char c)
{
	ClearPlrRVars(&plr[pnum]);
	SetRndSeed(GetTickCount());

	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("CreatePlayer: illegal player %d", pnum);
	}
	plr[pnum]._pClass = c;

	char val = StrengthTbl[c];
	if ( val < 0 ) {
		val = 0;
	}
	plr[pnum]._pStrength = val;
	plr[pnum]._pBaseStr = val;

	val = MagicTbl[c];
	if ( val < 0 ) {
		val = 0;
	}
	plr[pnum]._pMagic = val;
	plr[pnum]._pBaseMag = val;

	val = DexterityTbl[c];
	if ( val < 0 ) {
		val = 0;
	}
	plr[pnum]._pDexterity = val;
	plr[pnum]._pBaseDex = val;

	val = VitalityTbl[c];
	if ( val < 0 ) {
		val = 0;
	}
	plr[pnum]._pVitality = val;
	plr[pnum]._pBaseVit = val;

	plr[pnum]._pStatPts = 0;
	plr[pnum].pTownWarps = 0;
	plr[pnum].pDungMsgs = 0;
	plr[pnum].pLvlLoad = 0;
	plr[pnum].pDiabloKillLevel = 0;

	if ( c == PC_ROGUE ) {
		plr[pnum]._pDamageMod = plr[pnum]._pLevel * (plr[pnum]._pStrength + plr[pnum]._pDexterity) / 200;
	} else {
		plr[pnum]._pDamageMod = plr[pnum]._pStrength * plr[pnum]._pLevel / 100;
	}

	plr[pnum]._pBaseToBlk = ToBlkTbl[c];


	plr[pnum]._pHitPoints = (val + 10) << 6;
	if ( c == PC_WARRIOR ) {
		plr[pnum]._pHitPoints *= 2;
	}
	if ( c == PC_ROGUE ) {
		plr[pnum]._pHitPoints += plr[pnum]._pHitPoints >> 1;
	}

	int hp = plr[pnum]._pHitPoints;
	plr[pnum]._pMaxHP = hp;
	plr[pnum]._pHPBase = hp;
	plr[pnum]._pMaxHPBase = hp;

	plr[pnum]._pMana = plr[pnum]._pMagic << 6;
	if ( c == PC_SORCERER ) {
		plr[pnum]._pMana *= 2;
	}
	if ( c == PC_ROGUE ) {
		plr[pnum]._pMana += plr[pnum]._pMana >> 1;
	}

	int mana = plr[pnum]._pMana;
	plr[pnum]._pMaxMana = mana;
	plr[pnum]._pManaBase = mana;
	plr[pnum]._pMaxManaBase = mana;

	plr[pnum]._pLevel = 1;
	plr[pnum]._pMaxLvl = 1;
	plr[pnum]._pExperience = 0;
	plr[pnum]._pMaxExp = 0;
	plr[pnum]._pNextExper = ExpLvlsTbl[1];
	plr[pnum]._pArmorClass = 0;
	plr[pnum]._pMagResist = 0;
	plr[pnum]._pFireResist = 0;
	plr[pnum]._pLghtResist = 0;
	plr[pnum]._pLightRad = 10;
	plr[pnum]._pInfraFlag = 0;

	if ( c == PC_WARRIOR ) {
		plr[pnum]._pAblSpells[0] = 0x2000000;
		plr[pnum]._pAblSpells[1] = 0;
	} else if ( c == PC_ROGUE ) {
		plr[pnum]._pAblSpells[0] = 0x8000000;
		plr[pnum]._pAblSpells[1] = 0;
	} else if ( c == PC_SORCERER ) {
		plr[pnum]._pAblSpells[0] = 0x4000000;
		plr[pnum]._pAblSpells[1] = 0;
	}

	if ( c == PC_SORCERER ) {
		plr[pnum]._pMemSpells[0] = 1;
	} else {
		plr[pnum]._pMemSpells[0] = 0;
	}
	plr[pnum]._pMemSpells[1] = 0;

	int i;
	for ( i = 0; i < sizeof(plr[pnum]._pSplLvl); i++ ) {
		plr[pnum]._pSplLvl[i] = 0;
	}

	plr[pnum]._pSpellFlags = 0;

	if ( plr[pnum]._pClass == PC_SORCERER ) {
		plr[pnum]._pSplLvl[1] = 2;
	}

	// interestingly, only the first three hotkeys are reset
	// TODO: BUGFIX: clear all 4 hotkeys instead of 3 (demo leftover)
	for ( i = 0; i < 3; i++ ) {
		plr[pnum]._pSplHotKey[i] = -1;
	}

	if ( c == PC_WARRIOR ) {
		plr[pnum]._pgfxnum = 3;
	} else if ( c == PC_ROGUE ) {
		plr[pnum]._pgfxnum = 4;
	} else if ( c == PC_SORCERER ) {
		plr[pnum]._pgfxnum = 8;
	}

	for ( i = 0; i < sizeof(plr[pnum]._pLvlVisited); i++ ) {
		plr[pnum]._pLvlVisited[i] = 0;
	}

	for ( i = 0; i < sizeof(plr[pnum]._pSLvlVisited); i++ ) {
		plr[pnum]._pSLvlVisited[i] = 0;
	}

	plr[pnum]._pLvlChanging = 0;
	plr[pnum].pTownWarps = 0;
	plr[pnum].pLvlLoad = 0;
	plr[pnum].pBattleNet = 0;
	plr[pnum].pManaShield = 0;

	InitDungMsgs(pnum);
	CreatePlrItems(pnum);
	SetRndSeed(0);
}

int __fastcall CalcStatDiff(int pnum)
{
	int c = plr[pnum]._pClass;
	return MaxStats[c][ATTRIB_STR]
		- plr[pnum]._pBaseStr
		+ MaxStats[c][ATTRIB_MAG]
		- plr[pnum]._pBaseMag
		+ MaxStats[c][ATTRIB_DEX]
		- plr[pnum]._pBaseDex
		+ MaxStats[c][ATTRIB_VIT]
		- plr[pnum]._pBaseVit;
}

void __fastcall NextPlrLevel(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("NextPlrLevel: illegal player %d", pnum);
	}

	plr[pnum]._pLevel++;
	char l = plr[pnum]._pLevel;

	plr[pnum]._pMaxLvl++;

	if ( CalcStatDiff(pnum) < 5 ) {
		plr[pnum]._pStatPts = CalcStatDiff(pnum);
	} else {
		plr[pnum]._pStatPts += 5;
	}

	plr[pnum]._pNextExper = ExpLvlsTbl[l];

	char c = plr[pnum]._pClass;

	int hp = c == PC_SORCERER ? 64 : 128;
	if ( gbMaxPlayers == 1 ) {
		hp++;
	}
	plr[pnum]._pMaxHP += hp;
	plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
	plr[pnum]._pMaxHPBase += hp;
	plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;

	if ( pnum == myplr ) {
		drawhpflag = 1;
	}

	int mana = c != PC_WARRIOR ? 128 : 64;
	if ( gbMaxPlayers == 1 ) {
		mana++;
	}
	plr[pnum]._pMaxMana += mana;
	plr[pnum]._pMaxManaBase += mana;

	if ( !(plr[pnum]._pIFlags & ISPL_NOMANA) ) {
		plr[pnum]._pMana = plr[pnum]._pMaxMana;
		plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
	}

	if ( pnum == myplr ) {
		drawmanaflag = 1;
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall AddPlrExperience(int pnum, int lvl, int exp)
{
	if ( pnum != myplr ) {
		return;
	}

	if ( (DWORD)myplr >= MAX_PLRS ) {
		TermMsg("AddPlrExperience: illegal player %d", myplr);
	}

	if ( plr[myplr]._pHitPoints <= 0 ) {
		return;
	}

	// Adjust xp based on difference in level between player and monster
	exp *= 1 + ((double)lvl - plr[pnum]._pLevel) / 10;
	if ( exp < 0 ) {
		exp = 0;
	}

	// Prevent power leveling
	if ( gbMaxPlayers > 1 ) {
		int powerLvlCap = plr[pnum]._pLevel < 0 ? 0 : plr[pnum]._pLevel;
		if ( powerLvlCap >= 50 ) {
			powerLvlCap = 50;
		}
		// cap to 1/20 of current levels xp
		if ( exp >= ExpLvlsTbl[powerLvlCap] / 20 ) {
			exp = ExpLvlsTbl[powerLvlCap] / 20;
		}
		// cap to 200 * current level
		int expCap = 200 * powerLvlCap;
		if ( exp >= expCap ) {
			exp = expCap;
		}
	}

	plr[pnum]._pExperience += exp;
	if ( (DWORD)plr[pnum]._pExperience > MAXEXP ) {
		plr[pnum]._pExperience = MAXEXP;
	}

	if ( plr[pnum]._pExperience >= ExpLvlsTbl[49] ) {
		plr[pnum]._pLevel = 50;
		return;
	}

	// Increase player level if applicable
	int newLvl = 0;
	while ( plr[pnum]._pExperience >= ExpLvlsTbl[newLvl] ) {
		newLvl++;
	}
	if ( newLvl != plr[pnum]._pLevel ) {
		for ( int i = newLvl - plr[pnum]._pLevel; i > 0; i--) {
			NextPlrLevel(pnum);
		}
	}

	NetSendCmdParam1(FALSE, CMD_PLRLEVEL, plr[myplr]._pLevel);
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall AddPlrMonstExper(int lvl, int exp, char pmask)
{
	int totplrs = 0;
	for (int i = 0; i < 4; i++ ) {
		if ( (1 << i) & pmask ) {
			totplrs++;
		}
	}

	if ( totplrs && (1 << myplr) & pmask ) {
		AddPlrExperience(myplr, lvl, exp / totplrs);
	}
}

void __fastcall InitPlayer(int pnum, BOOL FirstTime)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("InitPlayer: illegal player %d", pnum);
	}

	ClearPlrRVars(&plr[pnum]);

	if ( FirstTime ) {
		plr[pnum]._pRSpell = SPL_INVALID;
		plr[pnum]._pSBkSpell = SPL_INVALID;
		plr[pnum]._pSpell = SPL_INVALID;
		plr[pnum]._pRSplType = RSPLTYPE_INVALID;
		plr[pnum]._pSplType = RSPLTYPE_INVALID;
		if ((plr[pnum]._pgfxnum & 0xF) == 4) {
			plr[pnum]._pwtype = TRUE;
		} else {
			plr[pnum]._pwtype = FALSE;
		}
		plr[pnum].pManaShield = 0;
	}

	if ( plr[pnum].plrlevel == currlevel || leveldebug ) {

		SetPlrAnims(pnum);

		plr[pnum]._pxoff = 0;
		plr[pnum]._pyoff = 0;
		plr[pnum]._pxvel = 0;
		plr[pnum]._pyvel = 0;

		ClearPlrPVars(pnum);

		if ( plr[pnum]._pHitPoints >> 6 > 0 ) {
			plr[pnum]._pmode = PM_STAND;
			NewPlrAnim(pnum, plr[pnum]._pNAnim[0], plr[pnum]._pNFrames, 3, plr[pnum]._pNWidth);
			plr[pnum]._pAnimFrame = random(2, plr[pnum]._pNFrames - 1) + 1;
			plr[pnum]._pAnimCnt = random(2, 3);
		} else {
			plr[pnum]._pmode = PM_DEATH;
			NewPlrAnim(pnum, plr[pnum]._pDAnim[0], plr[pnum]._pDFrames, 1, plr[pnum]._pDWidth);
			plr[pnum]._pAnimFrame = plr[pnum]._pAnimLen - 1;
			plr[pnum]._pVar8 = 2 * plr[pnum]._pAnimLen;
		}

		plr[pnum]._pdir = 0;
		plr[pnum]._peflag = 0;

		if ( pnum == myplr ) {
			if ( !FirstTime || currlevel ) {
				plr[pnum].WorldX = ViewX;
				plr[pnum].WorldY = ViewY;
			}
			plr[pnum]._ptargx = plr[pnum].WorldX;
			plr[pnum]._ptargy = plr[pnum].WorldY;
		} else {
			plr[pnum]._ptargx = plr[pnum].WorldX;
			plr[pnum]._ptargy = plr[pnum].WorldY;
			DWORD i;
			for ( i = 0; i < 8 && !PosOkPlayer(pnum, plrxoff2[i] + plr[pnum].WorldX, plryoff2[i] + plr[pnum].WorldY); ++i );
			plr[pnum].WorldX += plrxoff2[i];
			plr[pnum].WorldY += plryoff2[i];
		}

		plr[pnum]._px = plr[pnum].WorldX;
		plr[pnum]._py = plr[pnum].WorldY;
		plr[pnum].walkpath[0] = -1;
		plr[pnum].destAction = -1;

		if ( pnum == myplr ) {
			plr[pnum]._plid = AddLight(plr[pnum].WorldX, plr[pnum].WorldY, plr[pnum]._pLightRad);
		} else {
			plr[pnum]._plid = -1;
		}
		plr[pnum]._pvid = AddVision(plr[pnum].WorldX, plr[pnum].WorldY, plr[pnum]._pLightRad, pnum == myplr);
	}

	if ( plr[pnum]._pClass == PC_WARRIOR ) {
		plr[pnum]._pAblSpells64 = 1 << (SPL_REPAIR - 1);
	} else if ( plr[pnum]._pClass == PC_ROGUE ) {
		plr[pnum]._pAblSpells64 = 1 << (SPL_DISARM - 1);
	} else if ( plr[pnum]._pClass == PC_SORCERER ) {
		plr[pnum]._pAblSpells64 = 1 << (SPL_RECHARGE - 1);
	}

#ifdef _DEBUG
	if ( debug_mode_dollar_sign && FirstTime ) {
		plr[pnum]._pMemSpells64 |= 1 << (SPL_TELEPORT - 1);
		if ( !plr[myplr]._pSplLvl[SPL_TELEPORT] ) {
			plr[myplr]._pSplLvl[SPL_TELEPORT] = 1;
		}
	}
	if ( debug_mode_key_inverted_v && FirstTime ) {
		plr[pnum]._pMemSpells64 = SPL_INVALID;
	}
#endif

	plr[pnum]._pNextExper = ExpLvlsTbl[plr[pnum]._pLevel];
	plr[pnum]._pInvincible = FALSE;

	if ( pnum == myplr ) {
		deathdelay = 0;
		deathflag = 0;
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = 0;
	}
}
// 44B83C: could not find valid save-restore pair for edi
// 52572C: using guessed type int leveldebug;
// 69B7C4: using guessed type int deathdelay;

void __cdecl InitMultiView()
{
	if ( (DWORD)myplr >= MAX_PLRS ) {
		TermMsg("InitPlayer: illegal player %d", myplr);
	}

	ViewX = plr[myplr].WorldX;
	ViewY = plr[myplr].WorldY;
}

void __fastcall InitPlayerLoc(int pnum, BOOL flag)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("InitPlayer: illegal player %d", pnum);
	}

	int x = plr[pnum].WorldX - 1;
	int y = plr[pnum].WorldY + 1;
	int bitflags = 0;
	USHORT *pieces = (USHORT *)dpiece_defs_map_1 + 16 * gendung_get_dpiece_num_from_coord(x, y);

	int i;
	for ( i = 2; i < 10; i++ ) {
		bitflags |= pieces[i];
	}

	if ( bitflags | nSolidTable[dPiece[x][y]] | dArch[x][y] ) {
		plr[pnum]._peflag = 1;
	}
	else {
		plr[pnum]._peflag = 0;
	}

	if ( flag != 1 || plr[pnum]._peflag != 1 ) {
		return;
	}

	x = plr[pnum].WorldX;
	y = plr[pnum].WorldY + 2;
	bitflags = 0;
	pieces = (USHORT *)dpiece_defs_map_1 + 16 * gendung_get_dpiece_num_from_coord(x, y);

	for ( i = 2; i < 10; i++ ) {
		bitflags |= pieces[i];
	}

	if ( bitflags | dArch[x][y] ) {
		return;
	}

	x = plr[pnum].WorldX - 2;
	y = plr[pnum].WorldY + 1;
	bitflags = 0;
	pieces = (USHORT *)dpiece_defs_map_1 + 16 * gendung_get_dpiece_num_from_coord(x, y);

	for ( i = 2; i < 10; i++ ) {
		bitflags |= pieces[i];
	}

	if ( bitflags | dArch[x][y] ) {
		plr[pnum]._peflag = 2;
	}
}

BOOL __fastcall SolidLoc(int x, int y)
{
	if ( x < 0 || y < 0 || x >= MAXDUNX || y >= MAXDUNY ) {
		return FALSE;
	}

	return nSolidTable[dPiece[x][y]];
}

BOOL __fastcall PlrDirOK(int pnum, int dir)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("PlrDirOK: illegal player %d", pnum);
	}

	int px = plr[pnum].WorldX + offset_x[dir];
	int py = plr[pnum].WorldY + offset_y[dir];

	if ( px < 0 || !dPiece[px][py] || !PosOkPlayer(pnum, px, py) ) {
		return FALSE;
	}

	BOOL isOk = TRUE;
	if ( dir == DIR_E ) {
		isOk = !SolidLoc(px, py + 1) && !(dFlags[px][py + 1] & 32);
	}

	if ( isOk && dir == DIR_W ) {
		isOk = !SolidLoc(px + 1, py) && !(dFlags[px + 1][py] & 32);
	}

	return isOk;
}

void __fastcall PlrClrTrans(int x, int y)
{
	for ( int i = y - 1; i <= y + 1; i++ ) {
		for ( int j = x - 1; j <= x + 1; j++ ) {
			TransList[dung_map[j][i]] = 0;
		}
	}
}

void __fastcall PlrDoTrans(int x, int y)
{
	if ( leveltype != DTYPE_CATHEDRAL && leveltype != DTYPE_CATACOMBS ) {
		TransList[1] = 1;
	} else {
		for ( int i = y - 1; i <= y + 1; i++ ) {
			for ( int j = x - 1; j <= x + 1; j++ ) {
				if ( !nSolidTable[dPiece[j][i]] && dung_map[j][i] ) {
					TransList[dung_map[j][i]] = 1;
				}
			}
		}
	}
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall SetPlayerOld(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("SetPlayerOld: illegal player %d", pnum);
	}

	plr[pnum]._poldx = plr[pnum].WorldX;
	plr[pnum]._poldy = plr[pnum].WorldY;
}

void __fastcall FixPlayerLocation(int pnum, int dir)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("FixPlayerLocation: illegal player %d", pnum);
	}

	plr[pnum]._px = plr[pnum].WorldX;
	plr[pnum]._py = plr[pnum].WorldY;
	plr[pnum]._ptargx = plr[pnum].WorldX;
	plr[pnum]._ptargy = plr[pnum].WorldY;
	plr[pnum]._pxoff = 0;
	plr[pnum]._pyoff = 0;
	InitPlayerLoc(pnum, FALSE);
	plr[pnum]._pdir = dir;
	if ( pnum == myplr ) {
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = 0;
		ViewX = plr[pnum].WorldX;
		ViewY = plr[pnum].WorldY;
	}
}

void __fastcall StartStand(int pnum, int dir)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartStand: illegal player %d", pnum);
	}

	if ( !plr[pnum]._pInvincible || plr[pnum]._pHitPoints || pnum != myplr ) {
		if ( !(plr[pnum]._pGFXLoad & PFILE_STAND) ) {
			LoadPlrGFX(pnum, PFILE_STAND);
		}

		NewPlrAnim(pnum, plr[pnum]._pNAnim[dir], plr[pnum]._pNFrames, 3, plr[pnum]._pNWidth);
		plr[pnum]._pmode = PM_STAND;
		FixPlayerLocation(pnum, dir);
		FixPlrWalkTags(pnum);
		dPlayer[plr[pnum].WorldX][plr[pnum].WorldY] = pnum + 1;
		SetPlayerOld(pnum);
	} else {
		SyncPlrKill(pnum, -1);
	}
}

void __fastcall StartWalkStand(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartWalkStand: illegal player %d", pnum);
	}

	plr[pnum]._pmode = 0;
	plr[pnum]._px = plr[pnum].WorldX;
	plr[pnum]._py = plr[pnum].WorldY;
	plr[pnum]._pxoff = 0;
	plr[pnum]._pyoff = 0;

	InitPlayerLoc(pnum, 0);

	if ( pnum == myplr ) {
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = 0;
		ViewX = plr[pnum].WorldX;
		ViewY = plr[pnum].WorldY;
	}
}

void __fastcall PM_ChangeLightOff(int pnum)
{
	int v1; // esi
	int v2; // esi
	signed int v3; // ebx
	int v4; // edi
	int v5; // edx
	LightListStruct *v6; // eax
	int v7; // ecx
	int v8; // edx
	signed int v9; // edi
	int v10; // ebx
	int v11; // edx
	int v12; // ecx
	int v13; // ebp
	int ly; // [esp+10h] [ebp-Ch]
	int lx; // [esp+18h] [ebp-4h]

	v1 = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("PM_ChangeLightOff: illegal player %d", pnum);
	v2 = v1;
	v3 = -1;
	v4 = plr[v2]._pxoff;
	v5 = 2 * plr[v2]._pyoff;
	v6 = &LightList[plr[v2]._plid];
	v7 = v4 + v5;
	v8 = v5 - v4;
	if ( v7 >= 0 )
	{
		v9 = 1;
	}
	else
	{
		v9 = -1;
		v7 = -v7;
	}
	if ( v8 >= 0 )
		v3 = 1;
	else
		v8 = -v8;
	v10 = v3 * (v8 >> 3);
	v11 = 8 * v6->_ly;
	ly = v11 + v10;
	lx = v9 * (v7 >> 3);
	v12 = 8 * v6->_lx;
	v13 = v11 + v6->_yoff;
	if ( abs(lx - v6->_xoff) >= 3 || abs(ly - v13) >= 3 )
		ChangeLightOff(plr[v2]._plid, lx, v10);
}

void __fastcall PM_ChangeOffset(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("PM_ChangeOffset: illegal player %d", pnum);
	}

	plr[pnum]._pVar8++;
	int px = plr[pnum]._pVar6 >> 8;
	int py = plr[pnum]._pVar7 >> 8;

	plr[pnum]._pVar6 += plr[pnum]._pxvel;
	plr[pnum]._pVar7 += plr[pnum]._pyvel;
	plr[pnum]._pxoff = plr[pnum]._pVar6 >> 8;
	plr[pnum]._pyoff = plr[pnum]._pVar7 >> 8;

	if ( pnum == myplr && ScrollInfo._sdir ) {
		ScrollInfo._sxoff += px - plr[pnum]._pxoff;
		ScrollInfo._syoff += py - plr[pnum]._pyoff;
	}
	PM_ChangeLightOff(pnum);
}

void __fastcall StartWalk(int pnum, int xvel, int yvel, int xadd, int yadd, int EndDir, int sdir)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartWalk: illegal player %d", pnum);
	}

	if ( plr[pnum]._pInvincible && !plr[pnum]._pHitPoints && pnum == myplr ) {
		SyncPlrKill(pnum, -1);
		return;
	}

	SetPlayerOld(pnum);

	int px = xadd + plr[pnum].WorldX;
	int py = yadd + plr[pnum].WorldY;

	if ( !PlrDirOK(pnum, EndDir) ) {
		return;
	}

	plr[pnum]._px = px;
	plr[pnum]._py = py;

	if ( pnum == myplr ) {
		ScrollInfo._sdx = plr[pnum].WorldX - ViewX;
		ScrollInfo._sdy = plr[pnum].WorldY - ViewY;
	}

	dPlayer[px][py] = -(pnum + 1);
	plr[pnum]._pmode = PM_WALK;
	plr[pnum]._pxvel = xvel;
	plr[pnum]._pyvel = yvel;
	plr[pnum]._pxoff = 0;
	plr[pnum]._pyoff = 0;
	plr[pnum]._pVar1 = xadd;
	plr[pnum]._pVar2 = yadd;
	plr[pnum]._pVar3 = EndDir;

	if ( !(plr[pnum]._pGFXLoad & PFILE_WALK) ) {
		LoadPlrGFX(pnum, PFILE_WALK);
	}
	NewPlrAnim(pnum, plr[pnum]._pWAnim[EndDir], plr[pnum]._pWFrames, 0, plr[pnum]._pWWidth);

	plr[pnum]._pdir = EndDir;
	plr[pnum]._pVar6 = 0;
	plr[pnum]._pVar7 = 0;
	plr[pnum]._pVar8 = 0;

	InitPlayerLoc(pnum, FALSE);

	if ( pnum != myplr ) {
		return;
	}

	if ( zoomflag ) {
		if ( abs(ScrollInfo._sdx) >= 3 || abs(ScrollInfo._sdy) >= 3 ) {
			ScrollInfo._sdir = 0;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if ( abs(ScrollInfo._sdx) >= 2 || abs(ScrollInfo._sdy) >= 2 ) {
		ScrollInfo._sdir = 0;
	} else {
		ScrollInfo._sdir = sdir;
	}
}
// 52569C: using guessed type int zoomflag;

void __fastcall StartWalk2(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir, int sdir)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartWalk2: illegal player %d", pnum);
	}

	if ( plr[pnum]._pInvincible && !plr[pnum]._pHitPoints && pnum == myplr ) {
		SyncPlrKill(pnum, -1);
		return;
	}

	SetPlayerOld(pnum);
	int px = xadd + plr[pnum].WorldX;
	int py = yadd + plr[pnum].WorldY;

	if ( !PlrDirOK(pnum, EndDir) ) {
		return;
	}

	plr[pnum]._px = px;
	plr[pnum]._py = py;

	if ( pnum == myplr ) {
		ScrollInfo._sdx = plr[pnum].WorldX - ViewX;
		ScrollInfo._sdy = plr[pnum].WorldY - ViewY;
	}

	dPlayer[plr[pnum].WorldX][plr[pnum].WorldY] = -1 - pnum;
	plr[pnum]._pVar1 = plr[pnum].WorldX;
	plr[pnum]._pVar2 = plr[pnum].WorldY;
	plr[pnum].WorldX = px;
	plr[pnum].WorldY = py;
	dPlayer[plr[pnum].WorldX][plr[pnum].WorldY] = pnum + 1;
	plr[pnum]._pxoff = xoff;
	plr[pnum]._pyoff = yoff;

	ChangeLightXY(plr[pnum]._plid, plr[pnum].WorldX, plr[pnum].WorldY);
	PM_ChangeLightOff(pnum);

	plr[pnum]._pmode = PM_WALK2;
	plr[pnum]._pxvel = xvel;
	plr[pnum]._pyvel = yvel;
	plr[pnum]._pVar6 = xoff << 8;
	plr[pnum]._pVar7 = yoff << 8;
	plr[pnum]._pVar3 = EndDir;

	if ( !(plr[pnum]._pGFXLoad & PFILE_WALK) ) {
		LoadPlrGFX(pnum, PFILE_WALK);
	}
	NewPlrAnim(pnum, plr[pnum]._pWAnim[EndDir], plr[pnum]._pWFrames, 0, plr[pnum]._pWWidth);

	plr[pnum]._pdir = EndDir;
	plr[pnum]._pVar8 = 0;

	if ( EndDir == DIR_SE ) {
		InitPlayerLoc(pnum, TRUE);
	} else {
		InitPlayerLoc(pnum, FALSE);
	}

	if ( pnum != myplr ) {
		return;
	}

	if ( zoomflag ) {
		if ( abs(ScrollInfo._sdx) >= 3 || abs(ScrollInfo._sdy) >= 3 ) {
			ScrollInfo._sdir = 0;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if ( abs(ScrollInfo._sdx) >= 2 || abs(ScrollInfo._sdy) >= 2 ) {
		ScrollInfo._sdir = 0;
	} else {
		ScrollInfo._sdir = sdir;
	}
}
// 52569C: using guessed type int zoomflag;

void __fastcall StartWalk3(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir, int sdir)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartWalk3: illegal player %d", pnum);
	}

	if ( plr[pnum]._pInvincible && !plr[pnum]._pHitPoints && pnum == myplr ) {
		SyncPlrKill(pnum, -1);
		return;
	}

	SetPlayerOld(pnum);
	int px = xadd + plr[pnum].WorldX;
	int py = yadd + plr[pnum].WorldY;
	int x = mapx + plr[pnum].WorldX;
	int y = mapy + plr[pnum].WorldY;

	if ( !PlrDirOK(pnum, EndDir) ) {
		return;
	}

	plr[pnum]._px = px;
	plr[pnum]._py = py;

	if ( pnum == myplr ) {
		ScrollInfo._sdx = plr[pnum].WorldX - ViewX;
		ScrollInfo._sdy = plr[pnum].WorldY - ViewY;
	}

	dPlayer[plr[pnum].WorldX][plr[pnum].WorldY] = -1 - pnum;
	dPlayer[px][py] = -1 - pnum;
	plr[pnum]._pVar4 = x;
	plr[pnum]._pVar5 = y;
	dFlags[x][y] |= 0x20;
	plr[pnum]._pxoff = xoff;
	plr[pnum]._pyoff = yoff;

	if ( leveltype ) {
		ChangeLightXY(plr[pnum]._plid, x, y);
		PM_ChangeLightOff(pnum);
	}

	plr[pnum]._pmode = PM_WALK3;
	plr[pnum]._pxvel = xvel;
	plr[pnum]._pyvel = yvel;
	plr[pnum]._pVar1 = px;
	plr[pnum]._pVar2 = py;
	plr[pnum]._pVar6 = xoff << 8;
	plr[pnum]._pVar7 = yoff << 8;
	plr[pnum]._pVar3 = EndDir;

	if ( !(plr[pnum]._pGFXLoad & PFILE_WALK) ) {
		LoadPlrGFX(pnum, PFILE_WALK);
	}
	NewPlrAnim(pnum, plr[pnum]._pWAnim[EndDir], plr[pnum]._pWFrames, 0, plr[pnum]._pWWidth);

	plr[pnum]._pdir = EndDir;
	plr[pnum]._pVar8 = 0;

	InitPlayerLoc(pnum, 0);

	if ( pnum != myplr ) {
		return;
	}

	if ( zoomflag ) {
		if ( abs(ScrollInfo._sdx) >= 3 || abs(ScrollInfo._sdy) >= 3 ) {
			ScrollInfo._sdir = 0;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if ( abs(ScrollInfo._sdx) >= 2 || abs(ScrollInfo._sdy) >= 2 ) {
		ScrollInfo._sdir = 0;
	} else {
		ScrollInfo._sdir = sdir;
	}
}
// 52569C: using guessed type int zoomflag;
// 5BB1ED: using guessed type char leveltype;

void __fastcall StartAttack(int pnum, int d)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartAttack: illegal player %d", pnum);
	}

	if ( plr[pnum]._pInvincible && !plr[pnum]._pHitPoints && pnum == myplr ) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if ( !(plr[pnum]._pGFXLoad & PFILE_ATTACK) ) {
		LoadPlrGFX(pnum, PFILE_ATTACK);
	}

	NewPlrAnim(pnum, plr[pnum]._pAAnim[d], plr[pnum]._pAFrames, 0, plr[pnum]._pAWidth);
	plr[pnum]._pmode = PM_ATTACK;
	FixPlayerLocation(pnum, d);
	SetPlayerOld(pnum);
}

void __fastcall StartRangeAttack(int pnum, int d, int cx, int cy)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartRangeAttack: illegal player %d", pnum);
	}

	if ( plr[pnum]._pInvincible && !plr[pnum]._pHitPoints && pnum == myplr ) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if ( !(plr[pnum]._pGFXLoad & PFILE_ATTACK) ) {
		LoadPlrGFX(pnum, PFILE_ATTACK);
	}
	NewPlrAnim(pnum, plr[pnum]._pAAnim[d], plr[pnum]._pAFrames, 0, plr[pnum]._pAWidth);

	plr[pnum]._pmode = PM_RATTACK;
	FixPlayerLocation(pnum, d);
	SetPlayerOld(pnum);
	plr[pnum]._pVar1 = cx;
	plr[pnum]._pVar2 = cy;
}

void __fastcall StartPlrBlock(int pnum, int dir)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartPlrBlock: illegal player %d", pnum);
	}

	if ( plr[pnum]._pInvincible && !plr[pnum]._pHitPoints && pnum == myplr ) {
		SyncPlrKill(pnum, -1);
		return;
	}

	PlaySfxLoc(IS_ISWORD, plr[pnum].WorldX, plr[pnum].WorldY);

	if ( !(plr[pnum]._pGFXLoad & PFILE_BLOCK) ) {
		LoadPlrGFX(pnum, PFILE_BLOCK);
	}
	NewPlrAnim(pnum, plr[pnum]._pBAnim[dir], plr[pnum]._pBFrames, 2, plr[pnum]._pBWidth);

	plr[pnum]._pmode = PM_BLOCK;
	FixPlayerLocation(pnum, dir);
	SetPlayerOld(pnum);
}

void __fastcall StartSpell(int pnum, int d, int cx, int cy)
{
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("StartSpell: illegal player %d", pnum);

	if ( plr[pnum]._pInvincible && !plr[pnum]._pHitPoints && pnum == myplr ) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if ( leveltype != DTYPE_TOWN ) {
		switch ( spelldata[plr[pnum]._pSpell].sType )
		{
			case STYPE_FIRE:
				if ( !(plr[pnum]._pGFXLoad & PFILE_FIRE) ) {
					LoadPlrGFX(pnum, PFILE_FIRE);
				}
				NewPlrAnim(pnum, plr[pnum]._pFAnim[d], plr[pnum]._pSFrames, 0, plr[pnum]._pSWidth);
				break;
			case STYPE_LIGHTNING:
				if ( !(plr[pnum]._pGFXLoad & PFILE_LIGHTNING) ) {
					LoadPlrGFX(pnum, PFILE_LIGHTNING);
				}
				NewPlrAnim(pnum, plr[pnum]._pLAnim[d], plr[pnum]._pSFrames, 0, plr[pnum]._pSWidth);
				break;
			case STYPE_MAGIC:
				if ( !(plr[pnum]._pGFXLoad & PFILE_MAGIC) ) {
					LoadPlrGFX(pnum, PFILE_MAGIC);
				}
				NewPlrAnim(pnum, plr[pnum]._pTAnim[d], plr[pnum]._pSFrames, 0, plr[pnum]._pSWidth);
				break;
		}
	}

	PlaySfxLoc(spelldata[plr[pnum]._pSpell].sSFX, plr[pnum].WorldX, plr[pnum].WorldY);

	plr[pnum]._pmode = PM_SPELL;

	FixPlayerLocation(pnum, d);
	SetPlayerOld(pnum);

	plr[pnum]._pVar1 = cx;
	plr[pnum]._pVar2 = cy;
	plr[pnum]._pVar4 = GetSpellLevel(pnum, plr[pnum]._pSpell);
	plr[pnum]._pVar8 = 1;
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall FixPlrWalkTags(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("FixPlrWalkTags: illegal player %d", pnum);
	}

	int pNext = pnum + 1;
	int pPrev = -(pnum + 1);
	int dx = plr[pnum]._poldx;
	int dy = plr[pnum]._poldy;
	for ( int y = dy - 1; y <= dy + 1; y++ ) {
		for ( int x = dx - 1; x <= dx + 1; x++ ) {
			if ( x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY && (dPlayer[x][y] == pNext || dPlayer[x][y] == pPrev) ) {
				dPlayer[x][y] = 0;
			}
		}
	}

	if ( dx >= 0 && dx < MAXDUNX - 1 && dy >= 0 && dy < MAXDUNY -1 ) {
		dFlags[dx + 1][dy] &= 0xDF;
		dFlags[dx][dy + 1] &= 0xDF;
	}
}

void __fastcall RemovePlrFromMap(int pnum)
{
	int v1; // esi
	signed int v2; // edi
	signed int v3; // edx
	signed int v4; // ebx
	char v5; // al
	signed int v6; // edx
	_BYTE *v7; // eax
	signed int v8; // edi
	int v9; // ecx
	int v10; // [esp+Ch] [ebp-4h]

	v1 = -1 - pnum;
	v10 = pnum + 1;
	v2 = 1;
	do
	{
		v3 = v2;
		v4 = 111;
		do
		{
			if ( dPlayer[0][v3 + 111] == v1 || dPlayer[0][v3] == v1 )
			{
				v5 = dFlags[1][v3];
				if ( v5 & 0x20 )
					dFlags[1][v3] = v5 & 0xDF;
			}
			v3 += 112;
			--v4;
		}
		while ( v4 );
		++v2;
	}
	while ( v2 < 112 );
	v6 = 0;
	do
	{
		v7 = (unsigned char *)dPlayer + v6;
		v8 = 112;
		do
		{
			v9 = (char)*v7;
			if ( v9 == v10 || v9 == v1 )
				*v7 = 0;
			v7 += 112;
			--v8;
		}
		while ( v8 );
		++v6;
	}
	while ( v6 < 112 );
}

void __fastcall StartPlrHit(int pnum, int dam, BOOL forcehit)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartPlrHit: illegal player %d", pnum);
	}

	if ( plr[pnum]._pInvincible && !plr[pnum]._pHitPoints && pnum == myplr ) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if ( plr[pnum]._pClass == PC_WARRIOR ) {
		PlaySfxLoc(PS_WARR69, plr[pnum].WorldX, plr[pnum].WorldY);
	} else if ( plr[pnum]._pClass == PC_ROGUE ) {
		PlaySfxLoc(PS_ROGUE69, plr[pnum].WorldX, plr[pnum].WorldY);
	} else if ( plr[pnum]._pClass == PC_SORCERER ) {
		PlaySfxLoc(PS_MAGE69, plr[pnum].WorldX, plr[pnum].WorldY);
	}

	drawhpflag = 1;
	if ( dam >> 6 >= plr[pnum]._pLevel || forcehit ) {
		int dir = plr[pnum]._pdir;

		if ( !(plr[pnum]._pGFXLoad & PFILE_HIT) ) {
			LoadPlrGFX(pnum, PFILE_HIT);
		}
		NewPlrAnim(pnum, plr[pnum]._pHAnim[dir], plr[pnum]._pHFrames, 0, plr[pnum]._pHWidth);

		plr[pnum]._pmode = PM_GOTHIT;
		FixPlayerLocation(pnum, dir);
		plr[pnum]._pVar8 = 1;
		FixPlrWalkTags(pnum);
		dPlayer[plr[pnum].WorldX][plr[pnum].WorldY] = pnum + 1;
		SetPlayerOld(pnum);
	}
}

void __fastcall RespawnDeadItem(ItemStruct *itm, int x, int y)
{
	ItemStruct *v3; // ebx
	int v4; // eax
	int i; // ST10_4
	//unsigned int v6; // ecx

	v3 = itm;
	if ( numitems < MAXITEMS )
	{
		if ( FindGetItem(itm->IDidx, itm->_iCreateInfo, itm->_iSeed) >= 0 )
		{
			DrawInvMsg("A duplicate item has been detected.  Destroying duplicate...");
			SyncGetItem(x, y, v3->IDidx, v3->_iCreateInfo, v3->_iSeed);
		}
		v4 = itemavail[0];
		i = itemavail[0];
		dItem[x][y] = _LOBYTE(itemavail[0]) + 1;
		//v6 = 4 * numitems;
		itemactive[numitems] = v4;
		v4 *= 368;
		itemavail[0] = itemavail[-numitems + 126]; /* double check, MAXITEMS */
		qmemcpy((char *)item + v4, v3, sizeof(ItemStruct));
		*(int *)((char *)&item[0]._ix + v4) = x;
		*(int *)((char *)&item[0]._iy + v4) = y;
		RespawnItem(i, 1);
		++numitems;
		v3->_itype = -1;
	}
}

void __fastcall StartPlayerKill(int pnum, int earflag)
{
	unsigned int v2; // edi
	unsigned int v3; // esi
	char v4; // al
	int v5; // ecx
	int v6; // ST0C_4
	bool v7; // zf
	int *v8; // eax
	signed int v9; // ecx
	char *v10; // eax
	char v11; // al
	short v12; // cx
	short v13; // ax
	int v14; // ecx
	int v15; // eax
	signed int v17; // ebx
	int v18; // eax
	ItemStruct ear; // [esp+Ch] [ebp-178h]
	BOOL v20; // [esp+17Ch] [ebp-8h]
	struct ItemStruct *itm; // [esp+180h] [ebp-4h]

	v2 = pnum;
	v3 = 21720 * pnum;
	itm = (struct ItemStruct *)earflag;
	if ( plr[pnum]._pHitPoints <= 0 && plr[pnum]._pmode == PM_DEATH )
		return;
	if ( myplr == pnum )
		NetSendCmdParam1(1u, CMD_PLRDEAD, earflag);
	v20 = (unsigned char)gbMaxPlayers > 1u && plr[v3 / 0x54D8].plrlevel == 16;
	if ( v2 >= 4 )
		TermMsg("StartPlayerKill: illegal player %d", v2);
	v4 = plr[v3 / 0x54D8]._pClass;
	if ( v4 )
	{
		if ( v4 == 1 )
		{
			v5 = PS_ROGUE71;
		}
		else
		{
			if ( v4 != 2 )
				goto LABEL_18;
			v5 = PS_MAGE71;
		}
		PlaySfxLoc(v5, plr[v3 / 0x54D8].WorldX, plr[v3 / 0x54D8].WorldY);
		goto LABEL_18;
	}
	PlaySfxLoc(PS_DEAD, plr[v3 / 0x54D8].WorldX, plr[v3 / 0x54D8].WorldY); /// BUGFIX: should use `PS_WARR71` like other classes
LABEL_18:
	if ( plr[v3 / 0x54D8]._pgfxnum )
	{
		plr[v3 / 0x54D8]._pgfxnum = 0;
		plr[v3 / 0x54D8]._pGFXLoad = 0;
		SetPlrAnims(v2);
	}
	if ( !(plr[pnum]._pGFXLoad & PFILE_DEATH) ) {
		LoadPlrGFX(pnum, PFILE_DEATH);
	}
	v6 = plr[v3 / 0x54D8]._pDWidth;
	NewPlrAnim(v2, plr[0]._pDAnim[plr[v3 / 0x54D8]._pdir + v3 / 4], plr[v3 / 0x54D8]._pDFrames, 1, v6);
	plr[v3 / 0x54D8]._pBlockFlag = 0;
	plr[v3 / 0x54D8]._pmode = PM_DEATH;
	plr[v3 / 0x54D8]._pInvincible = 1;
	SetPlayerHitPoints(v2, 0);
	v7 = v2 == myplr;
	plr[v3 / 0x54D8]._pVar8 = 1;
	if ( !v7 && !itm && !v20 )
	{
		v8 = &plr[v3 / 0x54D8].InvBody[0]._itype;
		v9 = 7;
		do
		{
			*v8 = -1;
			v8 += 92;
			--v9;
		}
		while ( v9 );
		CalcPlrInv(v2, 0);
	}
	if ( plr[v3 / 0x54D8].plrlevel == currlevel )
	{
		FixPlayerLocation(v2, plr[v3 / 0x54D8]._pdir);
		RemovePlrFromMap(v2);
		v10 = &dFlags[plr[v3 / 0x54D8].WorldX][plr[v3 / 0x54D8].WorldY];
		*v10 |= 4u;
		SetPlayerOld(v2);
		if ( v2 == myplr )
		{
			drawhpflag = 1;
			deathdelay = 30;
			if ( pcurs >= CURSOR_FIRSTITEM )
			{
				PlrDeadItem(v2, &plr[v3 / 0x54D8].HoldItem, 0, 0);
				SetCursor(CURSOR_HAND);
			}
			if ( !v20 )
			{
				DropHalfPlayersGold(v2);
				if ( itm != (struct ItemStruct *)-1 )
				{
					if ( itm )
					{
						SetPlrHandItem(&ear, IDI_EAR);
						sprintf(ear._iName, "Ear of %s", plr[v3 / 0x54D8]._pName);
						v11 = plr[v3 / 0x54D8]._pClass;
						if ( v11 == 2 )
						{
							ear._iCurs = 19;
						}
						else if ( v11 )
						{
							if ( v11 == 1 )
								ear._iCurs = 21;
						}
						else
						{
							ear._iCurs = 20;
						}
						_LOBYTE(v12) = 0;
						_HIBYTE(v12) = plr[v3 / 0x54D8]._pName[0];
						v13 = v12 | plr[v3 / 0x54D8]._pName[1];
						v14 = plr[v3 / 0x54D8]._pName[3];
						ear._iCreateInfo = v13;
						v15 = plr[v3 / 0x54D8]._pName[5] | ((plr[v3 / 0x54D8]._pName[4] | ((v14 | (plr[v3 / 0x54D8]._pName[2] << 8)) << 8)) << 8);
						ear._ivalue = plr[v3 / 0x54D8]._pLevel;
						ear._iSeed = v15;
						if ( FindGetItem(IDI_EAR, *(int *)&ear._iCreateInfo, v15) == -1 )
							PlrDeadItem(v2, &ear, 0, 0);
					}
					else
					{
						itm = plr[v3 / 0x54D8].InvBody;
						v17 = 7;
						do
						{
							v18 = ((_BYTE)--v17 + (unsigned char)plr[v3 / 0x54D8]._pdir) & 7;
							PlrDeadItem(v2, itm, offset_x[v18], offset_y[v18]);
							++itm;
						}
						while ( v17 );
						CalcPlrInv(v2, 0);
					}
				}
			}
		}
	}
	SetPlayerHitPoints(v2, 0);
}
// 679660: using guessed type char gbMaxPlayers;
// 69B7C4: using guessed type int deathdelay;

void __fastcall PlrDeadItem(int pnum, struct ItemStruct *itm, int xx, int yy)
{
	int v4; // edi
	int v5; // edi
	int v6; // esi
	int v7; // ebx
	int v8; // eax
	int v9; // ST04_4
	ItemStruct *v10; // esi
	int v11; // eax
	int v12; // ebx
	int v13; // esi
	//int v14; // eax
	int v15; // edx
	unsigned char v16; // [esp-8h] [ebp-24h]
	unsigned char v17; // [esp-4h] [ebp-20h]
	int x; // [esp+Ch] [ebp-10h]
	ItemStruct *pItem; // [esp+10h] [ebp-Ch]
	int v20; // [esp+14h] [ebp-8h]
	int v21; // [esp+14h] [ebp-8h]
	int v22; // [esp+18h] [ebp-4h]
	int xxa; // [esp+24h] [ebp+8h]
	int yya; // [esp+28h] [ebp+Ch]

	pItem = itm;
	v4 = pnum;
	if ( itm->_itype != -1 )
	{
		if ( (unsigned int)pnum >= MAX_PLRS )
			TermMsg("PlrDeadItem: illegal player %d", pnum);
		v5 = v4;
		v6 = yy + plr[v5].WorldY;
		v7 = xx + plr[v5].WorldX;
		v20 = yy + plr[v5].WorldY;
		if ( (xx || yy) && (v8 = ItemSpaceOk(v7, v6), v8) )
		{
			v9 = v6;
			v10 = pItem;
			RespawnDeadItem(pItem, v7, v9);
			v17 = v20;
			v16 = v7;
		}
		else
		{
			yya = -1;
			xxa = 1;
			while ( 1 )
			{
				v11 = yya;
				v21 = yya;
LABEL_14:
				if ( v11 <= xxa )
					break;
				++xxa;
				if ( --yya <= -50 )
					return;
			}
			v12 = v21 + plr[v5].WorldY;
			v22 = yya;
			while ( 1 )
			{
				v13 = v22 + plr[v5].WorldX;
				x = v22 + plr[v5].WorldX;
				//_LOBYTE(v14) = ItemSpaceOk(v13, v12);
				if ( ItemSpaceOk(v13, v12) )
					break;
				if ( ++v22 > xxa )
				{
					v11 = ++v21;
					goto LABEL_14;
				}
			}
			v15 = v13;
			v10 = pItem;
			RespawnDeadItem(pItem, v15, v12);
			v17 = v12;
			v16 = x;
		}
		qmemcpy(&plr[v5].HoldItem, v10, sizeof(plr[v5].HoldItem));
		NetSendCmdPItem(0, CMD_RESPAWNITEM, v16, v17);
	}
}

void __fastcall DropHalfPlayersGold(int pnum)
{
	int v1; // ebx
	int v2; // esi
	int v3; // edi
	int v4; // ecx
	int v5; // eax
	int v6; // ecx
	int v7; // eax
	int v8; // edx
	int v9; // ecx
	int v10; // eax
	int v11; // edx
	int v12; // ecx
	int v13; // eax
	int v14; // [esp+Ch] [ebp-8h]
	int v15; // [esp+Ch] [ebp-8h]
	int v16; // [esp+Ch] [ebp-8h]
	int v17; // [esp+Ch] [ebp-8h]
	signed int i; // [esp+10h] [ebp-4h]
	signed int ia; // [esp+10h] [ebp-4h]
	signed int ib; // [esp+10h] [ebp-4h]
	signed int ic; // [esp+10h] [ebp-4h]

	v1 = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("DropHalfPlayersGold: illegal player %d", pnum);
	v2 = v1;
	v3 = plr[v1]._pGold >> 1;
	i = 0;
	while ( v3 > 0 )
	{
		v4 = 368 * i + v2 * 21720;
		v14 = v4;
		if ( *(int *)((char *)&plr[0].SpdList[0]._itype + v4) == ITYPE_GOLD )
		{
			v5 = *(int *)((char *)&plr[0].SpdList[0]._ivalue + v4);
			if ( v5 != 5000 )
			{
				if ( v3 >= v5 )
				{
					v3 -= v5;
					RemoveSpdBarItem(v1, i);
					SetPlrHandItem(&plr[v2].HoldItem, IDI_GOLD);
					GetGoldSeed(v1, &plr[v2].HoldItem);
					SetPlrHandGoldCurs(&plr[v2].HoldItem);
					plr[v2].HoldItem._ivalue = *(int *)((char *)&plr[0].SpdList[0]._ivalue + v14);
					PlrDeadItem(v1, &plr[v2].HoldItem, 0, 0);
					i = -1;
				}
				else
				{
					*(int *)((char *)&plr[0].SpdList[0]._ivalue + v4) = v5 - v3;
					SetSpdbarGoldCurs(v1, i);
					SetPlrHandItem(&plr[v2].HoldItem, IDI_GOLD);
					GetGoldSeed(v1, &plr[v2].HoldItem);
					SetPlrHandGoldCurs(&plr[v2].HoldItem);
					plr[v2].HoldItem._ivalue = v3;
					v3 = 0;
					PlrDeadItem(v1, &plr[v2].HoldItem, 0, 0);
				}
			}
		}
		if ( ++i >= 8 )
		{
			if ( v3 > 0 )
			{
				ia = 0;
				do
				{
					if ( v3 <= 0 )
						break;
					v6 = 368 * ia + v2 * 21720;
					v15 = v6;
					if ( *(int *)((char *)&plr[0].SpdList[0]._itype + v6) == ITYPE_GOLD )
					{
						v7 = *(int *)((char *)&plr[0].SpdList[0]._ivalue + v6);
						if ( v3 >= v7 )
						{
							v3 -= v7;
							RemoveSpdBarItem(v1, ia);
							SetPlrHandItem(&plr[v2].HoldItem, IDI_GOLD);
							GetGoldSeed(v1, &plr[v2].HoldItem);
							SetPlrHandGoldCurs(&plr[v2].HoldItem);
							plr[v2].HoldItem._ivalue = *(int *)((char *)&plr[0].SpdList[0]._ivalue + v15);
							PlrDeadItem(v1, &plr[v2].HoldItem, 0, 0);
							ia = -1;
						}
						else
						{
							*(int *)((char *)&plr[0].SpdList[0]._ivalue + v6) = v7 - v3;
							SetSpdbarGoldCurs(v1, ia);
							SetPlrHandItem(&plr[v2].HoldItem, IDI_GOLD);
							GetGoldSeed(v1, &plr[v2].HoldItem);
							SetPlrHandGoldCurs(&plr[v2].HoldItem);
							plr[v2].HoldItem._ivalue = v3;
							v3 = 0;
							PlrDeadItem(v1, &plr[v2].HoldItem, 0, 0);
						}
					}
					++ia;
				}
				while ( ia < 8 );
			}
			break;
		}
	}
	v8 = 0;
	drawpanflag = 255;
	if ( v3 > 0 )
	{
		ib = 0;
		if ( plr[v2]._pNumInv <= 0 )
		{
LABEL_28:
			if ( v3 > 0 )
			{
				v11 = 0;
				for ( ic = 0; ic < plr[v2]._pNumInv; v11 = ic++ + 1 )
				{
					if ( v3 <= 0 )
						break;
					v12 = 368 * v11 + v2 * 21720;
					v17 = v12;
					if ( *(int *)((char *)&plr[0].InvList[0]._itype + v12) == ITYPE_GOLD )
					{
						v13 = *(int *)((char *)&plr[0].InvList[0]._ivalue + v12);
						if ( v3 >= v13 )
						{
							v3 -= v13;
							RemoveInvItem(v1, v11);
							SetPlrHandItem(&plr[v2].HoldItem, IDI_GOLD);
							GetGoldSeed(v1, &plr[v2].HoldItem);
							SetPlrHandGoldCurs(&plr[v2].HoldItem);
							plr[v2].HoldItem._ivalue = *(int *)((char *)&plr[0].InvList[0]._ivalue + v17);
							PlrDeadItem(v1, &plr[v2].HoldItem, 0, 0);
							ic = -1;
						}
						else
						{
							*(int *)((char *)&plr[0].InvList[0]._ivalue + v12) = v13 - v3;
							SetGoldCurs(v1, v11);
							SetPlrHandItem(&plr[v2].HoldItem, IDI_GOLD);
							GetGoldSeed(v1, &plr[v2].HoldItem);
							SetPlrHandGoldCurs(&plr[v2].HoldItem);
							plr[v2].HoldItem._ivalue = v3;
							v3 = 0;
							PlrDeadItem(v1, &plr[v2].HoldItem, 0, 0);
						}
					}
				}
			}
		}
		else
		{
			while ( v3 > 0 )
			{
				v9 = 368 * v8 + v2 * 21720;
				v16 = v9;
				if ( *(int *)((char *)&plr[0].InvList[0]._itype + v9) == ITYPE_GOLD )
				{
					v10 = *(int *)((char *)&plr[0].InvList[0]._ivalue + v9);
					if ( v10 != 5000 )
					{
						if ( v3 >= v10 )
						{
							v3 -= v10;
							RemoveInvItem(v1, v8);
							SetPlrHandItem(&plr[v2].HoldItem, IDI_GOLD);
							GetGoldSeed(v1, &plr[v2].HoldItem);
							SetPlrHandGoldCurs(&plr[v2].HoldItem);
							plr[v2].HoldItem._ivalue = *(int *)((char *)&plr[0].InvList[0]._ivalue + v16);
							PlrDeadItem(v1, &plr[v2].HoldItem, 0, 0);
							ib = -1;
						}
						else
						{
							*(int *)((char *)&plr[0].InvList[0]._ivalue + v9) = v10 - v3;
							SetGoldCurs(v1, v8);
							SetPlrHandItem(&plr[v2].HoldItem, IDI_GOLD);
							GetGoldSeed(v1, &plr[v2].HoldItem);
							SetPlrHandGoldCurs(&plr[v2].HoldItem);
							plr[v2].HoldItem._ivalue = v3;
							v3 = 0;
							PlrDeadItem(v1, &plr[v2].HoldItem, 0, 0);
						}
					}
				}
				v8 = ib++ + 1;
				if ( ib >= plr[v2]._pNumInv )
					goto LABEL_28;
			}
		}
	}
	plr[v2]._pGold = CalculateGold(v1);
}
// 52571C: using guessed type int drawpanflag;

void __fastcall SyncPlrKill(int pnum, int earflag)
{
	if ( plr[pnum]._pHitPoints == 0 && currlevel == 0 ) {
		SetPlayerHitPoints(pnum, 64);
		return;
	}

	for ( int i = 0; i < nummissiles; i++ ) {
		int ma = missileactive[i];
		if ( missile[ma]._mitype == MIS_MANASHIELD && missile[ma]._misource == pnum && missile[ma]._miDelFlag == 0 ) {
			if ( earflag != -1 ) {
				missile[ma]._miVar8 = earflag;
			}

			return;
		}
	}

	SetPlayerHitPoints(pnum, 0);
	StartPlayerKill(pnum, earflag);
}

void __fastcall RemovePlrMissiles(int pnum)
{
	if ( currlevel != 0 && pnum == myplr && (monster[myplr]._mx != 1 || monster[myplr]._my != 0 ) ) {
		M_StartKill(myplr, myplr);
		AddDead(monster[myplr]._mx, monster[myplr]._my, monster[myplr].MType->mdeadval, (direction)monster[myplr]._mdir);
		dMonster[monster[myplr]._mx][monster[myplr]._my] = 0;
		monster[myplr]._mDelFlag = 1;
		DeleteMonsterList();
	}

	for ( int mi = 0; mi < nummissiles; mi++ ) {
		int am = missileactive[mi];
		if ( missile[am]._mitype == MIS_STONE && missile[am]._misource == pnum ) {
			monster[missile[am]._miVar2]._mmode = missile[am]._miVar1;
		}
		if ( missile[am]._mitype == MIS_MANASHIELD && missile[am]._misource == pnum ) {
			ClearMissileSpot(am);
			DeleteMissile(am, mi);
		}
		if ( missile[am]._mitype == MIS_ETHEREALIZE && missile[am]._misource == pnum ) {
			ClearMissileSpot(am);
			DeleteMissile(am, mi);
		}
	}
}

void __fastcall InitLevelChange(int pnum)
{
	RemovePlrMissiles(pnum);
	if ( pnum == myplr && qtextflag ) {
		qtextflag = 0;
		sfx_stop();
	}

	RemovePlrFromMap(pnum);
	SetPlayerOld(pnum);
	if ( pnum == myplr ) {
		dPlayer[plr[myplr].WorldX][plr[myplr].WorldY] = myplr + 1;
	} else {
		plr[pnum]._pLvlVisited[plr[pnum].plrlevel] = 1;
	}

	ClrPlrPath(pnum);
	plr[pnum].destAction = -1;
	plr[pnum]._pLvlChanging = 1;

	if ( pnum == myplr ) {
		plr[pnum].pLvlLoad = 10;
	}
}
// 646D00: using guessed type char qtextflag;

void __fastcall StartNewLvl(int pnum, int fom, int lvl)
{
	InitLevelChange(pnum);

	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("StartNewLvl: illegal player %d", pnum);
	}

	switch ( fom ) {
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
			TermMsg("StartNewLvl");
	}

	if ( pnum == myplr ) {
		plr[pnum]._pmode = PM_NEWLVL;
		plr[pnum]._pInvincible = TRUE;
		PostMessage(ghMainWnd, fom, 0, 0);
		if ( gbMaxPlayers > 1 ) {
			NetSendCmdParam2(TRUE, CMD_NEWLVL, fom, lvl);
		}
	}
}
// 5BB1ED: using guessed type char leveltype;
// 5CCB10: using guessed type char setlvlnum;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall RestartTownLvl(int pnum)
{
	unsigned int v1; // edi
	unsigned int v2; // esi
	int v3; // eax
	HWND v4; // ST00_4

	v1 = pnum;
	InitLevelChange(pnum);
	if ( v1 >= 4 )
		TermMsg("RestartTownLvl: illegal player %d", v1);
	v2 = v1;
	plr[v2].plrlevel = 0;
	plr[v2]._pInvincible = 0;
	SetPlayerHitPoints(v1, 64);
	v3 = plr[v2]._pMaxManaBase - plr[v2]._pMaxMana;
	plr[v2]._pMana = 0;
	plr[v2]._pManaBase = v3;
	CalcPlrInv(v1, 0);
	if ( v1 == myplr )
	{
		plr[v2]._pmode = PM_NEWLVL;
		v4 = ghMainWnd;
		plr[v2]._pInvincible = 1;
		PostMessage(v4, WM_DIABRETOWN, 0, 0);
	}
}

void __fastcall StartWarpLvl(int pnum, int pidx)
{
	InitLevelChange(pnum);

	if ( gbMaxPlayers != 1 ) {
		if ( plr[pnum].plrlevel != 0 ) {
			plr[pnum].plrlevel = 0;
		} else {
			plr[pnum].plrlevel = portal[pidx].level;
		}
	}

	if ( pnum == myplr ) {
		SetCurrentPortal(pidx);
		plr[pnum]._pmode = PM_NEWLVL;
		plr[pnum]._pInvincible = TRUE;
		PostMessage(ghMainWnd, WM_DIABWARPLVL, 0, 0);
	}
}
// 679660: using guessed type char gbMaxPlayers;

BOOL __fastcall PM_DoStand(int pnum)
{
	return FALSE;
}

BOOL __fastcall PM_DoWalk(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("PM_DoWalk: illegal player %d", pnum);
	}

	if ( plr[pnum]._pAnimFrame == 3
		|| (plr[pnum]._pWFrames == 8 && plr[pnum]._pAnimFrame == 7)
		|| (plr[pnum]._pWFrames != 8 && plr[pnum]._pAnimFrame == 4)
	) {
		PlaySfxLoc(PS_WALK1, plr[pnum].WorldX, plr[pnum].WorldY);
	}

	int vel = 8;
	if ( currlevel ) {
		vel = PWVel[3][plr[pnum]._pClass];
	}

	if ( plr[pnum]._pVar8 == vel ) {
		dPlayer[plr[pnum].WorldX][plr[pnum].WorldY] = 0;
		plr[pnum].WorldX += plr[pnum]._pVar1;
		plr[pnum].WorldY += plr[pnum]._pVar2;
		dPlayer[plr[pnum].WorldX][plr[pnum].WorldY] = pnum + 1;

		if ( leveltype != DTYPE_TOWN ) {
			ChangeLightXY(plr[pnum]._plid, plr[pnum].WorldX, plr[pnum].WorldY);
			ChangeVisionXY(plr[pnum]._pvid, plr[pnum].WorldX, plr[pnum].WorldY);
		}

		if ( pnum == myplr && ScrollInfo._sdir ) {
			ViewX = plr[pnum].WorldX - ScrollInfo._sdx;
			ViewY = plr[pnum].WorldY - ScrollInfo._sdy;
		}

		if ( plr[pnum].walkpath[0] != -1 ) {
			StartWalkStand(pnum);
		} else {
			StartStand(pnum, plr[pnum]._pVar3);
		}

		ClearPlrPVars(pnum);
		if ( leveltype != DTYPE_TOWN ) {
			ChangeLightOff(plr[pnum]._plid, 0, 0);
		}

		return TRUE;
	}

	PM_ChangeOffset(pnum);
	return FALSE;

}
// 5BB1ED: using guessed type char leveltype;

BOOL __fastcall PM_DoWalk2(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("PM_DoWalk2: illegal player %d", pnum);
	}

	if ( plr[pnum]._pAnimFrame == 3
		|| (plr[pnum]._pWFrames == 8 && plr[pnum]._pAnimFrame == 7)
		|| (plr[pnum]._pWFrames != 8 && plr[pnum]._pAnimFrame == 4)
	) {
		PlaySfxLoc(PS_WALK1, plr[pnum].WorldX, plr[pnum].WorldY);
	}

	int vel = 8;
	if ( currlevel ) {
		vel = PWVel[3][plr[pnum]._pClass];
	}

	if ( plr[pnum]._pVar8 == vel ) {
		dPlayer[plr[pnum]._pVar1][plr[pnum]._pVar2] = 0;
		if ( leveltype != DTYPE_TOWN ) {
			ChangeLightXY(plr[pnum]._plid, plr[pnum].WorldX, plr[pnum].WorldY);
			ChangeVisionXY(plr[pnum]._pvid, plr[pnum].WorldX, plr[pnum].WorldY);
		}

		if ( pnum == myplr && ScrollInfo._sdir ) {
			ViewX = plr[pnum].WorldX - ScrollInfo._sdx;
			ViewY = plr[pnum].WorldY - ScrollInfo._sdy;
		}

		if ( plr[pnum].walkpath[0] != -1 ) {
			StartWalkStand(pnum);
		} else {
			StartStand(pnum, plr[pnum]._pVar3);
		}

		ClearPlrPVars(pnum);

		if ( leveltype != DTYPE_TOWN ) {
			ChangeLightOff(plr[pnum]._plid, 0, 0);
		}

		return TRUE;
	}

	PM_ChangeOffset(pnum);
	return FALSE;
}
// 5BB1ED: using guessed type char leveltype;

BOOL __fastcall PM_DoWalk3(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("PM_DoWalk3: illegal player %d", pnum);
	}

	if ( plr[pnum]._pAnimFrame == 3
		|| (plr[pnum]._pWFrames == 8 && plr[pnum]._pAnimFrame == 7)
		|| (plr[pnum]._pWFrames != 8 && plr[pnum]._pAnimFrame == 4)
	) {
		PlaySfxLoc(PS_WALK1, plr[pnum].WorldX, plr[pnum].WorldY);
	}

	int vel = 8;
	if ( currlevel ) {
		vel = PWVel[3][plr[pnum]._pClass];
	}

	if ( plr[pnum]._pVar8 == vel ) {
		dPlayer[plr[pnum].WorldX][plr[pnum].WorldY] = 0;
		dFlags[plr[pnum]._pVar4][plr[pnum]._pVar5] &= 0xDFu;
		plr[pnum].WorldX = plr[pnum]._pVar1;
		plr[pnum].WorldY = plr[pnum]._pVar2;
		dPlayer[plr[pnum]._pVar1][plr[pnum]._pVar2] = pnum + 1;

		if ( leveltype != DTYPE_TOWN ) {
			ChangeLightXY(plr[pnum]._plid, plr[pnum]._pVar1, plr[pnum]._pVar2);
			ChangeVisionXY(plr[pnum]._pvid, plr[pnum].WorldX, plr[pnum].WorldY);
		}

		if ( pnum == myplr && ScrollInfo._sdir ) {
			ViewX = plr[pnum].WorldX - ScrollInfo._sdx;
			ViewY = plr[pnum].WorldY - ScrollInfo._sdy;
		}

		if ( plr[pnum].walkpath[0] != -1 ) {
			StartWalkStand(pnum);
		} else {
			StartStand(pnum, plr[pnum]._pVar3);
		}

		ClearPlrPVars(pnum);

		if ( leveltype != DTYPE_TOWN ) {
			ChangeLightOff(plr[pnum]._plid, 0, 0);
		}

		return TRUE;
	}

	PM_ChangeOffset(pnum);
	return FALSE;
}
// 5BB1ED: using guessed type char leveltype;

BOOL __fastcall WeaponDur(int pnum, int durrnd)
{
	if ( pnum != myplr ) {
		return FALSE;
	}

	if ( random(3, durrnd) != 0 ) {
		return FALSE;
	}

	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("WeaponDur: illegal player %d", pnum);
	}

	if ( plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON ) {
		if ( plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == 255 ) {
			return FALSE;
		}

		plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if ( plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability != 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if ( plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON ) {
		if ( plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == 255 ) {
			return FALSE;
		}

		plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if ( plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability != 0 ) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
			plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if ( plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD ) {
		if ( plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == 255 ) {
			return FALSE;
		}

		plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if ( plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability != 0 ) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
			plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if ( plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD ) {
		if ( plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability != 255 ) {
			plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability--;
			if ( plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability != 0 ) {
				NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
				plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
				CalcPlrInv(pnum, TRUE);
				return TRUE;
			}
		}
	}

	return FALSE;
}

bool __fastcall PlrHitMonst(int pnum, int m)
{
	int v2; // ebx
	unsigned int v3; // esi
	//int v4; // ST04_4
	int v5; // ebx
	//int v7; // ST04_4
	int v8; // eax
	unsigned int v9; // esi
	int v10; // ecx
	int v11; // eax
	int v12; // edi
	int v13; // edi
	//int v14; // eax
	int v16; // edx
	int v17; // eax
	int v18; // ecx
	int v19; // edi
	int v20; // eax
	int v21; // eax
	char v22; // dl
	bool v23; // zf
	int v24; // eax
	int v25; // ecx
	int v26; // edi
	int v27; // eax
	int v28; // edx
	int *v29; // ecx
	int v30; // edx
	int *v31; // ecx
	int v32; // ecx
	int v33; // edx
	int *v34; // ecx
	int v35; // edx
	int *v36; // ecx
	int v37; // edx
	int *v38; // ecx
	int *v39; // ecx
	int v40; // esi
	BOOL ret; // [esp+Ch] [ebp-18h]
	bool v42; // [esp+10h] [ebp-14h]
	int v48;
	int v43; // [esp+14h] [ebp-10h]
	int pnuma; // [esp+18h] [ebp-Ch]
	int arglist; // [esp+1Ch] [ebp-8h]
	int v46; // [esp+20h] [ebp-4h]

	v2 = m;
	v3 = pnum;
	arglist = m;
	pnuma = pnum;
	if ( (unsigned int)m >= 0xC8 )
	{
		TermMsg("PlrHitMonst: illegal monster %d", m);
		//pnum = v4;
	}
	v5 = 228 * v2;
	v43 = v5;
	if ( (signed int)(*(int *)((_BYTE *)&monster[0]._mhitpoints + v5) & 0xFFFFFFC0) <= 0
	  || **(_BYTE **)((char *)&monster[0].MType + v5) == MT_ILLWEAV && *((_BYTE *)&monster[0]._mgoal + v5) == 2
	  || *(MON_MODE *)((char *)&monster[0]._mmode + v5) == MM_CHARGE )
	{
		return 0;
	}
	if ( v3 >= 4 )
	{
		TermMsg("PlrHitMonst: illegal player %d", v3);
		//pnum = v7;
	}
	v42 = 0;
	v8 = random(4, 100);
	v23 = *(MON_MODE *)((char *)&monster[0]._mmode + v5) == MM_STONE;
	v46 = v8;
	if ( v23 )
		v46 = 0;
	v9 = v3;
	v10 = plr[v9]._pLevel;
	v11 = plr[v9]._pIEnAc + (plr[v9]._pDexterity >> 1) - *((unsigned char *)&monster[0].mArmorClass + v5);
	v12 = v11 + v10 + 50;
	if ( !_LOBYTE(plr[v9]._pClass) )
		v12 = v11 + v10 + 70;
	v13 = plr[v9]._pIBonusToHit + v12;
	if ( v13 < 5 )
		v13 = 5;
	if ( v13 > 95 )
		v13 = 95;
	if ( CheckMonsterHit(arglist, &ret) )
		return ret;
#ifdef _DEBUG
	if ( (signed int)v46 < v13 || debug_mode_key_inverted_v || debug_mode_dollar_sign )
#else
	if ( (signed int)v46 < v13 )
#endif
	{
		v16 = plr[v9]._pIMaxDam - plr[v9]._pIMinDam + 1;
		v48 = plr[v9]._pIMinDam;
		v17 = random(5, v16);
		v18 = 100;
		v19 = plr[v9]._pIBonusDamMod + plr[v9]._pDamageMod + (v48 + v17) * plr[v9]._pIBonusDam / 100 + v48 + v17;
		if ( !_LOBYTE(plr[v9]._pClass) )
		{
			v48 = plr[v9]._pLevel;
			v20 = random(6, 100);
			if ( v20 < v48 )
				v19 *= 2;
		}
		v21 = plr[v9].InvBody[4]._itype;
		v46 = -1;
		if ( v21 == 1 || plr[v9].InvBody[5]._itype == 1 )
			v46 = 1;
		if ( v21 == ITYPE_MACE || plr[v9].InvBody[5]._itype == ITYPE_MACE )
			v46 = ITYPE_MACE;
		v22 = (*(MonsterData **)((char *)&monster[0].MData + v5))->mMonstClass;
		if ( v22 )
		{
			if ( v22 != 2 )
				goto LABEL_40;
			if ( v46 == ITYPE_MACE )
				v19 -= v19 >> 1;
			v23 = v46 == 1;
		}
		else
		{
			if ( v46 == 1 )
				v19 -= v19 >> 1;
			v23 = v46 == ITYPE_MACE;
		}
		if ( v23 )
			v19 += v19 >> 1;
LABEL_40:
		v24 = plr[v9]._pIFlags;
		if ( v24 & 0x40000000 && v22 == 1 )
			v19 *= 3;
		v25 = pnuma;
		v26 = v19 << 6;
		if ( pnuma == myplr )
			*(int *)((char *)&monster[0]._mhitpoints + v5) -= v26;
		if ( v24 & 2 )
		{
			v27 = random(7, v26 >> 3);
			v28 = plr[v9]._pMaxHP;
			v29 = &plr[v9]._pHitPoints;
			*v29 += v27;
			if ( plr[v9]._pHitPoints > v28 )
				*v29 = v28;
			v30 = plr[v9]._pMaxHPBase;
			v31 = &plr[v9]._pHPBase;
			*v31 += v27;
			if ( plr[v9]._pHPBase > v30 )
				*v31 = v30;
			v5 = v43;
			drawhpflag = 1;
		}
		else
		{
			v27 = ret;
		}
		v46 = plr[v9]._pIFlags;
		v32 = v46;
		if ( v32 & 0x6000 && !(v46 & 0x8000000) )
		{
			if ( v32 & 0x2000 )
				v27 = 3 * v26 / 100;
			if ( v32 & 0x4000 )
				v27 = 5 * v26 / 100;
			v33 = plr[v9]._pMaxMana;
			v34 = &plr[v9]._pMana;
			*v34 += v27;
			if ( plr[v9]._pMana > v33 )
				*v34 = v33;
			v35 = plr[v9]._pMaxManaBase;
			v36 = &plr[v9]._pManaBase;
			*v36 += v27;
			if ( plr[v9]._pManaBase > v35 )
				*v36 = v35;
			v5 = v43;
			v32 = v46;
			drawmanaflag = 1;
		}
		if ( v32 & 0x18000 )
		{
			if ( (v32 & 0x8000) != 0 )
				v27 = 3 * v26 / 100;
			if ( v32 & 0x10000 )
				v27 = 5 * v26 / 100;
			v37 = plr[v9]._pMaxHP;
			v38 = &plr[v9]._pHitPoints;
			*v38 += v27;
			if ( plr[v9]._pHitPoints > v37 )
				*v38 = v37;
			v39 = &plr[v9]._pHPBase;
			v40 = plr[v9]._pMaxHPBase;
			*v39 += v27;
			if ( *v39 > v40 )
				*v39 = v40;
			BYTE1(v32) = BYTE1(v46);
			v5 = v43;
			drawhpflag = 1;
		}
		if ( v32 & 0x100 )
			*(int *)((char *)&monster[0]._mFlags + v5) |= 8u;
#ifdef _DEBUG
		if ( debug_mode_dollar_sign || debug_mode_key_inverted_v )
			monster[m]._mhitpoints = 0; /* double check */
#endif
		if ( (signed int)(*(int *)((_BYTE *)&monster[0]._mhitpoints + v5) & 0xFFFFFFC0) > 0 )
		{
			if ( *(MON_MODE *)((char *)&monster[0]._mmode + v5) != MM_STONE )
			{
				if ( v32 & 0x800 )
					M_GetKnockback(arglist);
				M_StartHit(arglist, pnuma, v26);
				goto LABEL_85;
			}
			M_StartHit(arglist, pnuma, v26);
		}
		else
		{
			if ( *(MON_MODE *)((char *)&monster[0]._mmode + v5) != MM_STONE )
			{
				M_StartKill(arglist, pnuma);
				goto LABEL_85;
			}
			M_StartKill(arglist, pnuma);
		}
		*(MON_MODE *)((char *)&monster[0]._mmode + v5) = MM_STONE;
LABEL_85:
		v42 = 1;
	}
	return v42;
}

bool __fastcall PlrHitPlr(int pnum, char p)
{
	char v2; // bl
	unsigned int v3; // esi
	//int v4; // ST04_4
	int v5; // edi
	//int v7; // ST04_4
	unsigned int v8; // esi
	int v9; // ecx
	int v10; // eax
	int v11; // ebx
	int v12; // ebx
	int v13; // eax
	int v14; // eax
	int v15; // ecx
	int v16; // eax
	int v17; // ebx
	int v18; // eax
	int v19; // ecx
	int v20; // edi
	int v21; // ebx
	signed int v22; // edi
	int v23; // eax
	int v24; // edx
	int *v25; // ecx
	int *v26; // ecx
	int v27; // esi
	int v28; // [esp+Ch] [ebp-14h]
	int v29; // [esp+10h] [ebp-10h]
	bool v30; // [esp+14h] [ebp-Ch]
	int arglist; // [esp+18h] [ebp-8h]
	char bPlr; // [esp+1Ch] [ebp-4h]

	v2 = p;
	v3 = pnum;
	bPlr = p;
	v28 = pnum;
	if ( (unsigned char)p >= 4u )
	{
		TermMsg("PlrHitPlr: illegal target player %d", p);
		//pnum = v4;
	}
	arglist = v2;
	v5 = v2;
	v30 = 0;
	if ( plr[v5]._pInvincible || plr[v5]._pSpellFlags & 1 )
		return 0;
	if ( v3 >= 4 )
	{
		TermMsg("PlrHitPlr: illegal attacking player %d", v3);
		//pnum = v7;
	}
	v8 = v3;
	v29 = random(4, 100);
	v9 = (plr[v8]._pDexterity >> 1) - plr[v5]._pIBonusAC - plr[v5]._pIAC - plr[v5]._pDexterity / 5;
	v10 = plr[v8]._pLevel;
	v11 = v9 + v10 + 50;
	if ( !_LOBYTE(plr[v8]._pClass) )
		v11 = v9 + v10 + 70;
	v12 = plr[v8]._pIBonusToHit + v11;
	if ( v12 < 5 )
		v12 = 5;
	if ( v12 > 95 )
		v12 = 95;
	v13 = plr[v5]._pmode;
	if ( v13 && v13 != 4 || !plr[v5]._pBlockFlag )
	{
		v14 = 100;
	}
	else
	{
		v14 = random(5, 100);
	}
	v15 = plr[v5]._pDexterity + plr[v5]._pBaseToBlk + 2 * plr[v5]._pLevel - 2 * plr[v8]._pLevel;
	if ( v15 < 0 )
		v15 = 0;
	if ( v15 > 100 )
		v15 = 100;
	if ( v29 < v12 )
	{
		if ( v14 >= v15 )
		{
			v17 = plr[v8]._pIMinDam;
			v18 = random(5, plr[v8]._pIMaxDam - v17 + 1);
			v19 = 100;
			v20 = plr[v8]._pIBonusDamMod + plr[v8]._pDamageMod + (v17 + v18) * plr[v8]._pIBonusDam / 100 + v17 + v18;
			if ( !_LOBYTE(plr[v8]._pClass) )
			{
				v21 = plr[v8]._pLevel;
				if ( random(6, 100) < v21 )
					v20 *= 2;
			}
			v22 = v20 << 6;
			if ( plr[v8]._pIFlags & 2 )
			{
				v23 = random(7, v22 >> 3);
				v24 = plr[v8]._pMaxHP;
				v25 = &plr[v8]._pHitPoints;
				*v25 += v23;
				if ( plr[v8]._pHitPoints > v24 )
					*v25 = v24;
				v26 = &plr[v8]._pHPBase;
				v27 = plr[v8]._pMaxHPBase;
				*v26 += v23;
				if ( *v26 > v27 )
					*v26 = v27;
				drawhpflag = 1;
			}
			if ( v28 == myplr )
				NetSendCmdDamage(1u, bPlr, v22);
			StartPlrHit(arglist, v22, 0);
		}
		else
		{
			v16 = GetDirection(plr[v5].WorldX, plr[v5].WorldY, plr[v8].WorldX, plr[v8].WorldY);
			StartPlrBlock(arglist, v16);
		}
		v30 = 1;
	}
	return v30;
}

BOOL __fastcall PlrHitObj(int pnum, int mx, int my)
{
	int oi;

	if ( dObject[mx][my] > 0 ) {
		oi = dObject[mx][my] - 1;
	} else {
		oi = -dObject[mx][my] - 1;
	}

	if ( object[oi]._oBreak == 1 ) {
		BreakObject(pnum, oi);
		return TRUE;
	}

	return FALSE;
}

int __fastcall PM_DoAttack(int pnum)
{
	int v1; // esi
	int v2; // esi
	int v3; // ecx
	int v4; // eax
	int v5; // eax
	int v6; // ebx
	int v7; // edi
	int v8; // eax
	int v9; // edx
	int v10; // ecx
	//int v11; // eax
	int v12; // edx
	int v13; // eax
	bool v14; // eax
	int v15; // edx
	char v16; // al
	//int v17; // eax
	int v19; // [esp+Ch] [ebp-8h]
	int arglist; // [esp+10h] [ebp-4h]

	v1 = pnum;
	arglist = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("PM_DoAttack: illegal player %d", pnum);
	v2 = v1;
	v3 = plr[v2]._pIFlags;
	v4 = plr[v2]._pAnimFrame;
	if ( v3 & 0x20000 && v4 == 1 ) // quick attack
		plr[v2]._pAnimFrame = 2;
	if ( v3 & 0x40000 && (v4 == 1 || v4 == 3) ) // fast attack
		++plr[v2]._pAnimFrame;
	if ( v3 & 0x80000 && (v4 == 1 || v4 == 3 || v4 == 5) ) // faster attack
		++plr[v2]._pAnimFrame;
	if ( v3 & 0x100000 && (v4 == 1 || v4 == 4) ) // fastest attack
		plr[v2]._pAnimFrame += 2;
	if ( plr[v2]._pAnimFrame == plr[v2]._pAFNum - 1 )
		PlaySfxLoc(PS_SWING, plr[v2].WorldX, plr[v2].WorldY);
	if ( plr[v2]._pAnimFrame != plr[v2]._pAFNum )
		goto LABEL_49;
	v5 = plr[v2]._pdir;
	v6 = plr[v2].WorldX + offset_x[v5];
	v7 = plr[v2].WorldY + offset_y[v5];
	v8 = v7 + 112 * v6;
	v19 = v8;
	v9 = dMonster[0][v8];
	if ( !v9 )
	{
LABEL_29:
		if ( plr[v2]._pIFlags & 0x10 )
		{
			AddMissile(v6, v7, 1, 0, 0, MIS_WEAPEXP, 0, arglist, 0, 0);
			v8 = v19;
		}
		if ( plr[v2]._pIFlags & 0x20 )
		{
			AddMissile(v6, v7, 2, 0, 0, MIS_WEAPEXP, 0, arglist, 0, 0);
			v8 = v19;
		}
		v12 = dMonster[0][v8];
		if ( v12 )
		{
			if ( v12 <= 0 )
				v13 = -1 - v12;
			else
				v13 = v12 - 1;
			v14 = PlrHitMonst(arglist, v13);
			goto LABEL_46;
		}
		v15 = (unsigned char)dPlayer[0][v8];
		if ( (_BYTE)v15 && !FriendlyMode )
		{
			if ( (char)v15 <= 0 )
				v16 = -1 - v15;
			else
				v16 = v15 - 1;
			v14 = PlrHitPlr(arglist, v16);
LABEL_46:
			if ( v14 )
			{
				//_LOBYTE(v17) = WeaponDur(arglist, 30);
				if ( WeaponDur(arglist, 30) )
					goto LABEL_48;
			}
			goto LABEL_49;
		}
		if ( dObject[0][v8] > 0 )
		{
			v14 = PlrHitObj(arglist, v6, v7);
			goto LABEL_46;
		}
LABEL_49:
		if ( plr[v2]._pAnimFrame != plr[v2]._pAFrames )
			return 0;
LABEL_48:
		StartStand(arglist, plr[v2]._pdir);
		ClearPlrPVars(arglist);
		return 1;
	}
	if ( v9 <= 0 )
		v10 = -1 - v9;
	else
		v10 = v9 - 1;
	//_LOBYTE(v11) = CanTalkToMonst(v10);
	if ( !CanTalkToMonst(v10) )
	{
		v8 = v19;
		goto LABEL_29;
	}
	plr[v2]._pVar1 = 0;
	return 0;
}
// 484368: using guessed type int FriendlyMode;

BOOL __fastcall PM_DoRangeAttack(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("PM_DoRangeAttack: illegal player %d", pnum);
	}

	int origFrame = plr[pnum]._pAnimFrame;
	if ( plr[pnum]._pIFlags & ISPL_QUICKATTACK && origFrame == 1 ) {
		plr[pnum]._pAnimFrame++;
	}
	if ( plr[pnum]._pIFlags & ISPL_FASTATTACK && (origFrame == 1 || origFrame == 3) ) {
		plr[pnum]._pAnimFrame++;
	}

	if ( plr[pnum]._pAnimFrame == plr[pnum]._pAFNum ) {
		int mistype = MIS_ARROW;
		if ( plr[pnum]._pIFlags & ISPL_FIRE_ARROWS ) {
			mistype = MIS_FARROW;
		}
		if ( plr[pnum]._pIFlags & ISPL_LIGHT_ARROWS ) {
			mistype = MIS_LARROW;
		}
		AddMissile(
			plr[pnum].WorldX,
			plr[pnum].WorldY,
			plr[pnum]._pVar1,
			plr[pnum]._pVar2,
			plr[pnum]._pdir,
			mistype,
			0,
			pnum,
			4,
			0
		);

		PlaySfxLoc(PS_BFIRE, plr[pnum].WorldX, plr[pnum].WorldY);

		if ( WeaponDur(pnum, 40) ) {
			StartStand(pnum, plr[pnum]._pdir);
			ClearPlrPVars(pnum);
			return TRUE;
		}
	}

	if ( plr[pnum]._pAnimFrame >= plr[pnum]._pAFrames ) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		return TRUE;
	} else {
		return FALSE;
	}

}

void __fastcall ShieldDur(int pnum)
{
	if ( pnum != myplr ) {
		return;
	}

	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("ShieldDur: illegal player %d", pnum);
	}

	if ( plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD ) {
		if ( plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == 255 ) {
			return;
		}

		plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if ( plr[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == 0 ) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
		}
	}

	if ( plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD ) {
		if ( plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability != 255 ) {
			plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability--;
			if ( plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == 0 ) {
				NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
				plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
				CalcPlrInv(pnum, TRUE);
			}
		}
	}
}

BOOL __fastcall PM_DoBlock(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("PM_DoBlock: illegal player %d", pnum);
	}

	if ( plr[pnum]._pIFlags & ISPL_FASTBLOCK && plr[pnum]._pAnimFrame != 1 ) {
		plr[pnum]._pAnimFrame = plr[pnum]._pBFrames;
	}

	if ( plr[pnum]._pAnimFrame >= plr[pnum]._pBFrames ) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);

		if ( !random(3, 10) ) {
			ShieldDur(pnum);
		}
		return TRUE;
	}

	return FALSE;
}

BOOL __fastcall PM_DoSpell(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("PM_DoSpell: illegal player %d", pnum);
	}

	if ( plr[pnum]._pVar8 == plr[pnum]._pSFNum )
	{
		CastSpell(
			pnum,
			plr[pnum]._pSpell,
			plr[pnum].WorldX,
			plr[pnum].WorldY,
			plr[pnum]._pVar1,
			plr[pnum]._pVar2,
			FALSE,
			plr[pnum]._pVar4
		);

		if ( !plr[pnum]._pSplFrom ) {
			if ( plr[pnum]._pRSplType == RSPLTYPE_SCROLL) {
				if ( !(plr[pnum]._pScrlSpells64
					& (UINT64)1 << (plr[pnum]._pRSpell - 1))
				) {
					plr[pnum]._pRSpell = SPL_INVALID;
					plr[pnum]._pRSplType = RSPLTYPE_INVALID;
					drawpanflag = 255;
				}
			}

			if ( plr[pnum]._pRSplType == RSPLTYPE_CHARGES) {
				if ( !(plr[pnum]._pISpells64
					& (UINT64)1 << (plr[pnum]._pRSpell - 1))
				) {
					plr[pnum]._pRSpell = SPL_INVALID;
					plr[pnum]._pRSplType = RSPLTYPE_INVALID;
					drawpanflag = 255;
				}
			}
		}
	}

	plr[pnum]._pVar8++;

	if ( leveltype == DTYPE_TOWN ) {
		if ( plr[pnum]._pVar8 > plr[pnum]._pSFrames ) {
			StartWalkStand(pnum);
			ClearPlrPVars(pnum);
			return TRUE;
		}
	} else if ( plr[pnum]._pAnimFrame == plr[pnum]._pSFrames ) {
		StartStand(pnum, plr[pnum]._pdir);
		ClearPlrPVars(pnum);
		return TRUE;
	}

	return FALSE;
}
// 52571C: using guessed type int drawpanflag;
// 5BB1ED: using guessed type char leveltype;

int __fastcall PM_DoGotHit(int pnum)
{
	int v1; // esi
	int v2; // eax
	int v3; // edx
	int v4; // ecx

	v1 = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("PM_DoGotHit: illegal player %d", pnum);
	v2 = v1;
	v3 = plr[v1]._pIFlags;
	v4 = plr[v1]._pAnimFrame;
	if ( v3 & 0x200000 && v4 == 3 )
		plr[v2]._pAnimFrame = 4;
	if ( v3 & 0x400000 && (v4 == 3 || v4 == 5) )
		++plr[v2]._pAnimFrame;
	if ( v3 & 0x800000 && (v4 == 1 || v4 == 3 || v4 == 5) )
		++plr[v2]._pAnimFrame;
	if ( plr[v2]._pAnimFrame < plr[v2]._pHFrames )
		return 0;
	StartStand(v1, plr[v2]._pdir);
	ClearPlrPVars(v1);
	if ( random(3, 4) )
		ArmorDur(v1);
	return 1;
}

void __fastcall ArmorDur(int pnum)
{
	int v1; // ebp
	//int v2; // ST04_4
	PlayerStruct *v3; // esi
	int v4; // eax
	int v5; // edi
	int v6; // esi
	int v7; // ecx
	int v8; // ecx
	unsigned char v9; // dl

	v1 = pnum;
	if ( pnum == myplr )
	{
		if ( (unsigned int)pnum >= MAX_PLRS )
		{
			TermMsg("ArmorDur: illegal player %d", pnum);
			//pnum = v2;
		}
		v3 = &plr[v1];
		if ( v3->InvBody[6]._itype != -1 || v3->InvBody[0]._itype != -1 )
		{
			v4 = random(8, 3);
			v5 = v3->InvBody[6]._itype;
			if ( v5 == -1 )
				goto LABEL_23;
			if ( v3->InvBody[0]._itype == -1 )
				v4 = 1;
			if ( v5 == -1 )
			{
LABEL_23:
				if ( v3->InvBody[0]._itype != -1 )
					v4 = 0;
			}
			if ( v4 )
				v6 = (int)&v3->InvBody[6];
			else
				v6 = (int)v3->InvBody;
			v7 = *(_DWORD *)(v6 + 236);
			if ( v7 != 255 )
			{
				v8 = v7 - 1;
				*(_DWORD *)(v6 + 236) = v8;
				if ( !v8 )
				{
					if ( v4 )
						v9 = 6;
					else
						v9 = 0;
					NetSendCmdDelItem(1u, v9);
					*(_DWORD *)(v6 + 8) = -1;
					CalcPlrInv(v1, 1u);
				}
			}
		}
	}
}

int __fastcall PM_DoDeath(int pnum)
{
	int v1; // edi
	int v2; // esi
	int v3; // ecx
	int v4; // eax
	int v5; // eax

	v1 = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("PM_DoDeath: illegal player %d", pnum);
	v2 = v1;
	if ( plr[v1]._pVar8 >= 2 * plr[v1]._pDFrames )
	{
		if ( deathdelay > 1 && v1 == myplr && --deathdelay == 1 )
		{
			deathflag = 1;
			if ( gbMaxPlayers == 1 )
				gamemenu_previous();
		}
		v3 = plr[v2].WorldY;
		plr[v2]._pAnimFrame = plr[v2]._pAnimLen;
		v4 = plr[v2].WorldX;
		plr[v2]._pAnimDelay = 10000;
		dFlags[v4][v3] |= 4u;
	}
	v5 = plr[v2]._pVar8;
	if ( v5 < 100 )
		plr[v2]._pVar8 = v5 + 1;
	return 0;
}
// 679660: using guessed type char gbMaxPlayers;
// 69B7C4: using guessed type int deathdelay;

void __fastcall CheckNewPath(int pnum)
{
	int v1; // edi
	int v2; // ebx
	int v3; // eax
	int v4; // ecx
	bool v5; // zf
	int v6; // eax
	int v7; // esi
	int v8; // eax
	int v9; // edx
	int v10; // esi
	int v11; // esi
	int v12; // eax
	int v13; // eax
	int v14; // ecx
	int v15; // edx
	int v16; // eax
	int v17; // eax
	int v18; // eax
	int v19; // ecx
	int v20; // eax
	int v21; // edi
	int v22; // esi
	int v23; // ST38_4
	int v24; // eax
	int v25; // esi
	int v26; // esi
	int v27; // ST38_4
	int v28; // eax
	int v29; // ecx
	int v30; // edx
	int v31; // ecx
	int *v32; // esi
	int *v33; // edi
	int v34; // esi
	int v35; // eax
	int v36; // ecx
	int v37; // eax
	int v38; // eax
	int v39; // eax
	int v40; // eax
	int v41; // eax
	int *v42; // esi
	int *v43; // edi
	int v44; // eax
	int v45; // eax
	int v46; // esi
	int v47; // esi
	int v48; // eax
	int v49; // ecx
	int v50; // esi
	int v51; // eax
	int v52; // ecx
	int v53; // edi
	int v54; // esi
	int v55; // ST38_4
	int v56; // eax
	int v57; // edi
	int v58; // esi
	int v59; // ST38_4
	int v60; // eax
	int v61; // eax
	int v62; // ecx
	int v63; // esi
	int v64; // ST38_4
	int v65; // eax
	int v66; // esi
	int v67; // edi
	int v68; // eax
	int v69; // esi
	int v70; // esi
	int v71; // eax
	int v72; // ecx
	int v73; // eax
	int v74; // eax
	int *v75; // esi
	int *v76; // edi
	int v77; // eax
	int v78; // eax
	int v79; // eax
	int v80; // eax
	int *v81; // esi
	int *v82; // edi
	int v83; // eax
	int v84; // eax
	int v85; // eax
	int v86; // [esp-18h] [ebp-34h]
	int v87; // [esp-10h] [ebp-2Ch]
	int v88; // [esp-10h] [ebp-2Ch]
	int v89; // [esp-Ch] [ebp-28h]
	int v90; // [esp-Ch] [ebp-28h]
	int v91; // [esp-8h] [ebp-24h]
	int v92; // [esp-8h] [ebp-24h]
	int v93; // [esp-8h] [ebp-24h]
	int v94; // [esp-4h] [ebp-20h]
	int v95; // [esp-4h] [ebp-20h]
	int v96; // [esp-4h] [ebp-20h]
	signed int v97; // [esp+Ch] [ebp-10h]
	int arglist; // [esp+10h] [ebp-Ch]
	int arglista; // [esp+10h] [ebp-Ch]
	int arglistb; // [esp+10h] [ebp-Ch]
	int v101; // [esp+14h] [ebp-8h]
	int v102; // [esp+14h] [ebp-8h]
	int v103; // [esp+14h] [ebp-8h]
	int v104; // [esp+14h] [ebp-8h]
	int p; // [esp+18h] [ebp-4h]

	v1 = pnum;
	p = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("CheckNewPath: illegal player %d", pnum);
	v2 = v1;
	if ( plr[v1].destAction == 20 )
		MakePlrPath(v1, monster[plr[v2].destParam1]._mfutx, monster[plr[v2].destParam1]._mfuty, 0);
	if ( plr[v2].destAction == 21 )
		MakePlrPath(v1, plr[plr[v2].destParam1]._px, plr[plr[v2].destParam1]._py, 0);
	if ( plr[v2].walkpath[0] == -1 )
	{
		v18 = plr[v2].destAction;
		if ( v18 == -1 )
			return;
		v19 = plr[v2]._pmode;
		if ( v19 == PM_STAND )
		{
			switch ( v18 )
			{
				case 9:
					v20 = GetDirection(plr[v2].WorldX, plr[v2].WorldY, plr[v2].destParam1, plr[v2].destParam2);
					goto LABEL_52;
				case 10:
					v30 = plr[v2].WorldY;
					v31 = plr[v2].WorldX;
					v32 = &plr[v2].destParam2;
					v33 = &plr[v2].destParam1;
					goto LABEL_59;
				case 12:
					v39 = GetDirection(plr[v2].WorldX, plr[v2].WorldY, plr[v2].destParam1, plr[v2].destParam2);
					StartSpell(p, v39, plr[v2].destParam1, plr[v2].destParam2);
					v40 = plr[v2].destParam3;
					goto LABEL_66;
				case 13:
					v46 = plr[v2].destParam1;
					arglista = v46;
					v47 = v46;
					v102 = abs(plr[v2].WorldX - object[v47]._ox);
					v48 = abs(plr[v2].WorldY - object[v47]._oy);
					if ( v48 > 1 )
					{
						v49 = object[v47]._oy;
						if ( dObject[object[v47]._ox][v49-1] == -1 - arglista ) /* dungeon[39][112 * object[v47]._ox + 39 + v49] check */
							v48 = abs(plr[v2].WorldY - v49 + 1);
					}
					if ( v102 > 1 || v48 > 1 )
						break;
					if ( object[v47]._oBreak != 1 )
						goto LABEL_73;
					goto LABEL_80;
				case 14:
					v50 = plr[v2].destParam1;
					arglista = v50;
					v47 = v50;
					v103 = abs(plr[v2].WorldX - object[v47]._ox);
					v51 = abs(plr[v2].WorldY - object[v47]._oy);
					if ( v51 > 1 )
					{
						v52 = object[v47]._oy;
						if ( dObject[object[v47]._ox][v52-1] == -1 - arglista ) /* dungeon[39][112 * object[v47]._ox + 39 + v52] check */
							v51 = abs(plr[v2].WorldY - v52 + 1);
					}
					if ( v103 > 1 || v51 > 1 )
						break;
					if ( object[v47]._oBreak == 1 )
					{
LABEL_80:
						v20 = GetDirection(plr[v2].WorldX, plr[v2].WorldY, object[v47]._ox, object[v47]._oy);
LABEL_81:
						v29 = p;
LABEL_82:
						StartAttack(v29, v20);
					}
					else
					{
						TryDisarm(p, arglista);
LABEL_73:
						OperateObject(p, arglista, 0);
					}
					break;
				case 15:
					if ( v1 == myplr )
					{
						v53 = plr[v2].destParam1;
						v54 = plr[v2].destParam1;
						v55 = abs(plr[v2].WorldX - item[v54]._ix);
						v56 = abs(plr[v2].WorldY - item[v54]._iy);
						if ( v55 <= 1 && v56 <= 1 && pcurs == 1 && !item[v54]._iRequest )
						{
							NetSendCmdGItem(1u, CMD_REQUESTGITEM, myplr, myplr, v53);
							item[v54]._iRequest = 1;
						}
					}
					break;
				case 16:
					if ( v1 == myplr )
					{
						v57 = plr[v2].destParam1;
						v58 = plr[v2].destParam1;
						v59 = abs(plr[v2].WorldX - item[v58]._ix);
						v60 = abs(plr[v2].WorldY - item[v58]._iy);
						if ( v59 <= 1 && v60 <= 1 && pcurs == 1 )
							NetSendCmdGItem(1u, CMD_REQUESTAGITEM, myplr, myplr, v57);
					}
					break;
				case 17:
					if ( v1 == myplr )
						TalkToTowner(v1, plr[v2].destParam1);
					break;
				case 18:
					if ( object[plr[v2].destParam1]._oBreak != 1 )
						OperateObject(v1, plr[v2].destParam1, 1u);
					break;
				case 20:
					v21 = plr[v2].destParam1;
					v22 = plr[v2].destParam1;
					v23 = abs(plr[v2].WorldX - monster[v22]._mfutx);
					v24 = abs(plr[v2].WorldY - monster[v22]._mfuty);
					if ( v23 > 1 || v24 > 1 )
						break;
					v20 = GetDirection(plr[v2]._px, plr[v2]._py, monster[v22]._mfutx, monster[v22]._mfuty);
					v25 = monster[v22].mtalkmsg;
					if ( v25 && v25 != QUEST_VILE14 )
						goto LABEL_56;
					goto LABEL_81;
				case 21:
					v26 = plr[v2].destParam1;
					v27 = abs(plr[v2].WorldX - plr[v26]._px);
					v28 = abs(plr[v2].WorldY - plr[v26]._py);
					if ( v27 > 1 || v28 > 1 )
						break;
					v20 = GetDirection(plr[v2]._px, plr[v2]._py, plr[v26]._px, plr[v26]._py);
LABEL_52:
					v29 = v1;
					goto LABEL_82;
				case 22:
					v21 = plr[v2].destParam1;
					v34 = plr[v2].destParam1;
					v35 = GetDirection(plr[v2]._px, plr[v2]._py, monster[v34]._mfutx, monster[v34]._mfuty);
					v36 = monster[v34].mtalkmsg;
					if ( v36 && v36 != QUEST_VILE14 )
LABEL_56:
						TalktoMonster(v21);
					else
						StartRangeAttack(p, v35, monster[v34]._mfutx, monster[v34]._mfuty);
					break;
				case 23:
					v30 = plr[v2]._py;
					v37 = plr[v2].destParam1;
					v31 = plr[v2]._px;
					v32 = &plr[v37]._py;
					v33 = &plr[v37]._px;
LABEL_59:
					v38 = GetDirection(v31, v30, *v33, *v32);
					StartRangeAttack(p, v38, *v33, *v32);
					break;
				case 24:
					v41 = plr[v2].destParam1;
					v42 = &monster[v41]._mfuty;
					v43 = &monster[v41]._mfutx;
					goto LABEL_65;
				case 25:
					v44 = plr[v2].destParam1;
					v42 = &plr[v44]._py;
					v43 = &plr[v44]._px;
LABEL_65:
					v45 = GetDirection(plr[v2].WorldX, plr[v2].WorldY, *v43, *v42);
					StartSpell(p, v45, *v43, *v42);
					v40 = plr[v2].destParam2;
					goto LABEL_66;
				case 26:
					StartSpell(v1, plr[v2].destParam3, plr[v2].destParam1, plr[v2].destParam2);
					plr[v2]._pVar3 = plr[v2].destParam3;
					v40 = plr[v2].destParam4;
LABEL_66:
					plr[v2]._pVar4 = v40;
					break;
				default:
					break;
			}
			FixPlayerLocation(p, plr[v2]._pdir);
			goto LABEL_143;
		}
		if ( v19 == 4 && plr[v2]._pAnimFrame > plr[myplr]._pAFNum )
		{
			switch ( v18 )
			{
				case 9:
					v61 = GetDirection(plr[v2]._px, plr[v2]._py, plr[v2].destParam1, plr[v2].destParam2);
LABEL_105:
					v62 = v1;
LABEL_106:
					StartAttack(v62, v61);
LABEL_107:
					plr[v2].destAction = -1;
					break;
				case 20:
					v63 = plr[v2].destParam1;
					v64 = abs(plr[v2].WorldX - monster[v63]._mfutx);
					v65 = abs(plr[v2].WorldY - monster[v63]._mfuty);
					if ( v64 > 1 || v65 > 1 )
						goto LABEL_107;
					v61 = GetDirection(plr[v2]._px, plr[v2]._py, monster[v63]._mfutx, monster[v63]._mfuty);
					goto LABEL_105;
				case 21:
					v66 = plr[v2].destParam1;
					v67 = abs(plr[v2].WorldX - plr[v66]._px);
					v68 = abs(plr[v2].WorldY - plr[v66]._py);
					if ( v67 > 1 || v68 > 1 )
						goto LABEL_107;
					v61 = GetDirection(plr[v2]._px, plr[v2]._py, plr[v66]._px, plr[v66]._py);
					v62 = p;
					goto LABEL_106;
				case 13:
					v69 = plr[v2].destParam1;
					arglistb = v69;
					v70 = v69;
					v104 = abs(plr[v2].WorldX - object[v70]._ox);
					v71 = abs(plr[v2].WorldY - object[v70]._oy);
					if ( v71 > 1 )
					{
						v72 = object[v70]._oy;
						if ( dObject[object[v70]._ox][v72-1] == -1 - arglistb ) /* dungeon[39][112 * object[v70]._ox + 39 + v72] check */
							v71 = abs(plr[v2].WorldY - v72 + 1);
					}
					if ( v104 <= 1 && v71 <= 1 )
					{
						if ( object[v70]._oBreak == 1 )
						{
							v73 = GetDirection(plr[v2].WorldX, plr[v2].WorldY, object[v70]._ox, object[v70]._oy);
							StartAttack(p, v73);
						}
						else
						{
							OperateObject(p, arglistb, 0);
						}
					}
					break;
			}
		}
		if ( plr[v2]._pmode == PM_RATTACK && plr[v2]._pAnimFrame > plr[myplr]._pAFNum )
		{
			v74 = plr[v2].destAction;
			switch ( v74 )
			{
				case 10:
					v75 = &plr[v2].destParam2;
					v76 = &plr[v2].destParam1;
LABEL_133:
					v79 = GetDirection(plr[v2].WorldX, plr[v2].WorldY, *v76, *v75);
					StartRangeAttack(p, v79, *v76, *v75);
					plr[v2].destAction = -1;
					break;
				case 22:
					v77 = plr[v2].destParam1;
					v75 = &monster[v77]._mfuty;
					v76 = &monster[v77]._mfutx;
					goto LABEL_133;
				case 23:
					v78 = plr[v2].destParam1;
					v75 = &plr[v78]._py;
					v76 = &plr[v78]._px;
					goto LABEL_133;
			}
		}
		if ( plr[v2]._pmode == PM_SPELL && plr[v2]._pAnimFrame > plr[v2]._pSFNum )
		{
			v80 = plr[v2].destAction;
			switch ( v80 )
			{
				case 12:
					v81 = &plr[v2].destParam2;
					v82 = &plr[v2].destParam1;
					break;
				case 24:
					v83 = plr[v2].destParam1;
					v81 = &monster[v83]._mfuty;
					v82 = &monster[v83]._mfutx;
					break;
				case 25:
					v84 = plr[v2].destParam1;
					v81 = &plr[v84]._py;
					v82 = &plr[v84]._px;
					break;
				default:
					return;
			}
			v85 = GetDirection(plr[v2].WorldX, plr[v2].WorldY, *v82, *v81);
			StartSpell(p, v85, *v82, *v81);
			goto LABEL_143;
		}
		return;
	}
	if ( plr[v2]._pmode == PM_STAND )
	{
		if ( v1 == myplr )
		{
			v3 = plr[v2].destAction;
			if ( v3 == 20 || v3 == 21 )
			{
				v4 = plr[v2].destParam1;
				v5 = v3 == 20;
				v6 = plr[v2]._px;
				arglist = plr[v2].destParam1;
				if ( v5 )
				{
					v7 = v4;
					v101 = abs(v6 - monster[v4]._mfutx);
					v8 = abs(plr[v2]._py - monster[v7]._mfuty);
					v9 = plr[v2]._py;
					v94 = monster[v7]._mfuty;
					v91 = monster[v7]._mfutx;
				}
				else
				{
					v10 = v4;
					v101 = abs(v6 - plr[v4]._px);
					v8 = abs(plr[v2]._py - plr[v10]._py);
					v9 = plr[v2]._py;
					v94 = plr[v10]._py;
					v91 = plr[v10]._px;
				}
				v97 = v8;
				v11 = GetDirection(plr[v2]._px, v9, v91, v94);
				if ( v101 < 2 && v97 < 2 )
				{
					ClrPlrPath(p);
					v12 = monster[arglist].mtalkmsg;
					if ( v12 && v12 != QUEST_VILE14 )
						TalktoMonster(arglist);
					else
						StartAttack(p, v11);
					plr[v2].destAction = -1;
				}
			}
		}
		if ( currlevel )
		{
			v13 = SLOBYTE(plr[v2]._pClass);
			v14 = PWVel[v13][0];
			v15 = PWVel[v13][1];
			v16 = PWVel[v13][2];
		}
		else
		{
			v14 = 2048;
			v15 = 1024;
			v16 = 512;
		}
		switch ( plr[v2].walkpath[0] )
		{
			case WALK_NE:
				v95 = 2;
				v92 = DIR_NE;
				v89 = -1;
				v87 = 0;
				v17 = -v16;
				goto LABEL_37;
			case WALK_NW:
				v95 = 8;
				v92 = DIR_NW;
				v89 = 0;
				v87 = -1;
				v17 = -v16;
				v15 = -v15;
LABEL_37:
				StartWalk(p, v15, v17, v87, v89, v92, v95);
				break;
			case WALK_SE:
				v96 = 4;
				v93 = DIR_SE;
				v90 = 0;
				v88 = 1;
				v86 = -32;
				goto LABEL_32;
			case WALK_SW:
				v96 = 6;
				v93 = DIR_SW;
				v90 = 1;
				v88 = 0;
				v86 = 32;
				v15 = -v15;
LABEL_32:
				StartWalk2(p, v15, v16, v86, -16, v88, v90, v93, v96);
				break;
			case WALK_N:
				StartWalk(p, 0, -v15, -1, -1, DIR_N, 1);
				break;
			case WALK_E:
				StartWalk3(p, v14, 0, -32, -16, 1, -1, 1, 0, DIR_E, 3);
				break;
			case WALK_S:
				StartWalk2(p, 0, v15, 0, -32, 1, 1, DIR_S, 5);
				break;
			case WALK_W:
				StartWalk3(p, -v14, 0, 32, -16, -1, 1, 0, 1, DIR_W, 7);
				break;
			default:
				break;
		}
		qmemcpy(plr[v2].walkpath, &plr[v2].walkpath[1], 0x18u);
		plr[v2].walkpath[24] = -1;
		if ( plr[v2]._pmode == PM_STAND )
		{
			StartStand(p, plr[v2]._pdir);
LABEL_143:
			plr[v2].destAction = -1;
			return;
		}
	}
}

BOOL __fastcall PlrDeathModeOK(int pnum)
{
	if ( pnum != myplr ) {
		return TRUE;
	}

	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("PlrDeathModeOK: illegal player %d", pnum);
	}

	if (plr[pnum]._pmode == PM_DEATH) {
		return TRUE;
	} else if (plr[pnum]._pmode == PM_QUIT) {
		return TRUE;
	} else if (plr[pnum]._pmode == PM_NEWLVL) {
		return TRUE;
	}

	return FALSE;
}

void __cdecl ValidatePlayer()
{
	int v0; // edi
	int v1; // esi
	char *v2; // eax
	int v3; // ecx
	int v4; // ecx
	int *v5; // eax
	int v6; // eax
	int v7; // edx
	int v8; // edx
	int v9; // edx
	int v10; // eax
	int *v11; // ebx
	signed int v12; // edi
	char *v13; // eax
	__int64 v14; // [esp+Ch] [ebp-8h]

	v0 = 0;
	v14 = (__int64)0;
	if ( (unsigned int)myplr >= 4 )
		TermMsg("ValidatePlayer: illegal player %d", myplr);
	v1 = myplr;
	v2 = &plr[myplr]._pLevel;
	if ( *v2 > 50 )
		*v2 = 50;
	v3 = plr[v1]._pNextExper;
	if ( plr[v1]._pExperience > v3 )
		plr[v1]._pExperience = v3;
	v4 = 0;
	if ( plr[v1]._pNumInv > 0 )
	{
		v5 = &plr[v1].InvList[0]._ivalue;
		do
		{
			if ( *(v5 - 47) == 11 )
			{
				if ( *v5 > 5000 )
					*v5 = 5000;
				v4 += *v5;
			}
			++v0;
			v5 += 92;
		}
		while ( v0 < plr[v1]._pNumInv );
	}
	if ( v4 != plr[v1]._pGold )
		plr[v1]._pGold = v4;
	v6 = SLOBYTE(plr[v1]._pClass);
	v7 = MaxStats[v6][0];
	if ( plr[v1]._pBaseStr > v7 )
		plr[v1]._pBaseStr = v7;
	v8 = MaxStats[v6][1];
	if ( plr[v1]._pBaseMag > v8 )
		plr[v1]._pBaseMag = v8;
	v9 = MaxStats[v6][2];
	if ( plr[v1]._pBaseDex > v9 )
		plr[v1]._pBaseDex = v9;
	v10 = MaxStats[v6][3];
	if ( plr[v1]._pBaseVit > v10 )
		plr[v1]._pBaseVit = v10;
	v11 = &spelldata[1].sBookLvl;
	v12 = 1;
	do
	{
		if ( *v11 != -1 )
		{
			v14 |= (__int64)1 << ((unsigned char)v12 - 1);
			v13 = &plr[v1]._pSplLvl[v12];
			if ( *v13 > 15 )
				*v13 = 15;
		}
		v11 += 14;
		++v12;
	}
	while ( (signed int)v11 < (signed int)&spelldata[37].sBookLvl );
	*(_QWORD *)plr[v1]._pMemSpells &= v14;
}

void __cdecl ProcessPlayers()
{
	int v0; // eax
	int v1; // eax
	unsigned char *v2; // ecx
	char v3; // al
	int v4; // ebp
	int *v5; // esi
	int v6; // eax
	//int v7; // eax
	int v8; // eax
	int v9; // eax
	int v10; // eax
	int v11; // edi
	int v12; // eax
	char *v13; // eax
	char *v14; // eax

	v0 = myplr;
	if ( (unsigned int)myplr >= 4 )
	{
		TermMsg("ProcessPlayers: illegal player %d", myplr);
		v0 = myplr;
	}
	v1 = v0;
	v2 = &plr[v1].pLvlLoad;
	v3 = plr[v1].pLvlLoad;
	if ( v3 )
		*v2 = v3 - 1;
	v4 = 0;
	if ( sfxdelay > 0 && !--sfxdelay )
		PlaySFX(sfxdnum);
	ValidatePlayer();
	v5 = &plr[0]._pHitPoints;
	do
	{
		v6 = (int)(v5 - 89);
		if ( *((_BYTE *)v5 - 379) && currlevel == *(_DWORD *)v6 && (v4 == myplr || !*(_BYTE *)(v6 + 267)) )
		{
			CheckCheatStats(v4);
			//_LOBYTE(v7) = PlrDeathModeOK(v4);
			if ( !PlrDeathModeOK(v4) && (signed int)(*v5 & 0xFFFFFFC0) <= 0 )
				SyncPlrKill(v4, -1);
			if ( v4 == myplr )
			{
				if ( v5[5294] & 0x40 && currlevel )
				{
					*v5 -= 4;
					v8 = *v5;
					*(v5 - 2) -= 4;
					if ( (signed int)(v8 & 0xFFFFFFC0) <= 0 )
						SyncPlrKill(v4, 0);
					drawhpflag = 1;
				}
				if ( *((_BYTE *)v5 + 21179) & 8 )
				{
					v9 = v5[3];
					if ( v9 > 0 )
					{
						v10 = v9 - v5[5];
						v5[5] = 0;
						drawmanaflag = 1;
						v5[3] = v10;
					}
				}
			}
			v11 = 0;
			do
			{
				switch ( *(v5 - 102) )
				{
					case PM_STAND:
						v12 = PM_DoStand(v4);
						goto LABEL_38;
					case PM_WALK:
						v12 = PM_DoWalk(v4);
						goto LABEL_38;
					case PM_WALK2:
						v12 = PM_DoWalk2(v4);
						goto LABEL_38;
					case PM_WALK3:
						v12 = PM_DoWalk3(v4);
						goto LABEL_38;
					case PM_ATTACK:
						v12 = PM_DoAttack(v4);
						goto LABEL_38;
					case PM_RATTACK:
						v12 = PM_DoRangeAttack(v4);
						goto LABEL_38;
					case PM_BLOCK:
						v12 = PM_DoBlock(v4);
						goto LABEL_38;
					case PM_GOTHIT:
						v12 = PM_DoGotHit(v4);
						goto LABEL_38;
					case PM_DEATH:
						v12 = PM_DoDeath(v4);
						goto LABEL_38;
					case PM_SPELL:
						v12 = PM_DoSpell(v4);
						goto LABEL_38;
					case PM_NEWLVL:
						v12 = PM_DoStand(v4);
LABEL_38:
						v11 = v12;
						break;
					default:
						break;
				}
				CheckNewPath(v4);
			}
			while ( v11 );
			v13 = (char *)(v5 - 69);
			++*(_DWORD *)v13;
			if ( *(v5 - 69) > *(v5 - 70) )
			{
				*(_DWORD *)v13 = 0;
				v14 = (char *)(v5 - 67);
				++*(_DWORD *)v14;
				if ( *(v5 - 67) > *(v5 - 68) )
					*(_DWORD *)v14 = 1;
			}
		}
		v5 += 5430;
		++v4;
	}
	while ( (signed int)v5 < (signed int)&plr[MAX_PLRS]._pHitPoints );
}
// 52A554: using guessed type int sfxdelay;

void __fastcall CheckCheatStats(int pnum)
{
	if ( plr[pnum]._pStrength > 750 ) {
		plr[pnum]._pStrength = 750;
	}

	if ( plr[pnum]._pDexterity > 750 ) {
		plr[pnum]._pDexterity = 750;
	}

	if ( plr[pnum]._pMagic > 750 ) {
		plr[pnum]._pMagic = 750;
	}

	if ( plr[pnum]._pVitality > 750 ) {
		plr[pnum]._pVitality = 750;
	}

	if ( plr[pnum]._pHitPoints > 128000 ) {
		plr[pnum]._pHitPoints = 128000;
	}

	if ( plr[pnum]._pMana > 128000 ) {
		plr[pnum]._pMana = 128000;
	}
}

void __fastcall ClrPlrPath(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("ClrPlrPath: illegal player %d", pnum);
	}

	memset(plr[pnum].walkpath, -1, sizeof(plr[pnum].walkpath));
}

BOOL __fastcall PosOkPlayer(int pnum, int px, int py)
{
	char v8; // cl
	unsigned int v9; // ecx
	int v10; // esi
	char v11; // al
	char v12; // cl
	bool result; // eax

	result = 0;
	if ( px >= 0 && px < 112 && py >= 0 && py < 112 && !SolidLoc(px, py) )
	{
		if ( dPiece[px][py] )
		{
			v8 = dPlayer[px][py];
			if ( !v8 || (v8 <= 0 ? (v9 = -1 - v8) : (v9 = v8 - 1), v9 == pnum || v9 >= 4 || !plr[v9]._pHitPoints) )
			{
				v10 = dMonster[px][py];
				if ( !v10 || currlevel && v10 > 0 && (signed int)(monster[v10-1]._mhitpoints & 0xFFFFFFC0) <= 0 ) /* fix */
				{
					v11 = dObject[px][py];
					if ( !v11 || (v11 <= 0 ? (v12 = -1 - v11) : (v12 = v11 - 1), !object[v12]._oSolidFlag) )
						result = 1;
				}
			}
		}
	}
	return result;
}

void __fastcall MakePlrPath(int pnum, int xx, int yy, unsigned char endspace)
{
	int v4; // esi
	int v5; // ebx
	int v6; // esi
	int v7; // edi
	int v8; // eax
	int v9; // eax
	int v10; // ecx
	int a2; // [esp+Ch] [ebp-4h]

	v4 = pnum;
	v5 = xx;
	a2 = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("MakePlrPath: illegal player %d", pnum);
	v6 = v4;
	v7 = yy;
	v8 = plr[v6]._px;
	plr[v6]._ptargx = v5;
	plr[v6]._ptargy = yy;
	if ( v8 != v5 || plr[v6]._py != yy )
	{
		v9 = FindPath(PosOkPlayer, a2, v8, plr[v6]._py, v5, yy, plr[v6].walkpath);
		if ( v9 )
		{
			if ( !endspace )
			{
				v10 = plr[v6].walkpath[--v9]; /* *((char *)&plr[v6]._pmode + v9-- + 3) */
				switch ( v10 )
				{
					case 1: // N
						goto LABEL_12;
					case 2: // W
						++v5;
						break;
					case 3: // E
						--v5;
						break;
					case 4: // S
						goto LABEL_15;
					case 5: // NW
						++v5;
						goto LABEL_12;
					case 6: // NE
						--v5;
LABEL_12:
						v7 = yy + 1;
						break;
					case 7: // SE
						--v5;
						goto LABEL_15;
					case 8: // SW
						++v5;
LABEL_15:
						v7 = yy - 1;
						break;
					default: // 0/Neutral
						break;
				}
				plr[v6]._ptargx = v5;
				plr[v6]._ptargy = v7;
			}
			plr[v6].walkpath[v9] = -1;
		}
	}
}

void __fastcall CheckPlrSpell()
{
	int v0; // ecx
	int v1; // eax
	int v2; // edx
	char v3; // al
	int v4; // ecx
	char v5; // al
	int v6; // eax
	int v7; // edx
	int v8; // esi
	int v9; // ST10_4
	int v10; // eax
	int v11; // eax
	int v12; // eax
	int v13; // ST10_4
	int v14; // eax
	char v15; // al

	v0 = myplr;
	if ( (unsigned int)myplr >= 4 )
	{
		TermMsg("CheckPlrSpell: illegal player %d", myplr);
		v0 = myplr;
	}
	v1 = 21720 * v0;
	v2 = plr[v0]._pRSpell;
	if ( v2 != -1 )
	{
		if ( leveltype == DTYPE_TOWN && !*(_DWORD *)&spelldata[v2].sTownSpell )
		{
			v5 = *((_BYTE *)&plr[0]._pClass + v1);
			switch ( v5 )
			{
				case PC_WARRIOR:
					v4 = PS_WARR27;
					goto LABEL_53;
				case PC_ROGUE:
					v4 = PS_ROGUE27;
					goto LABEL_53;
				case PC_SORCERER:
					v4 = PS_MAGE27;
					goto LABEL_53;
			}
			return;
		}
		if ( pcurs != CURSOR_HAND
		  || MouseY >= 352
		  || (chrflag && MouseX < 320 || invflag && MouseX > 320)
		  && v2 != 2
		  && v2 != 5
		  && v2 != 26
		  && v2 != 9
		  && v2 != 27 )
		{
			return;
		}
		_LOBYTE(v1) = *((_BYTE *)&plr[0]._pRSplType + v1);
		if ( (v1 & 0x80u) != 0 )
			goto LABEL_46;
		if ( (char)v1 <= 1 )
		{
			v6 = CheckSpell(v0, v2, v1, 0);
		}
		else
		{
			if ( (_BYTE)v1 != 2 )
			{
				if ( (_BYTE)v1 == 3 )
				{
					v6 = UseStaff();
					goto LABEL_36;
				}
LABEL_46:
				if ( _LOBYTE(plr[v0]._pRSplType) == 1 )
				{
					v15 = plr[v0]._pClass;
					switch ( v15 )
					{
						case PC_WARRIOR:
							v4 = PS_WARR35;
							goto LABEL_53;
						case PC_ROGUE:
							v4 = PS_ROGUE35;
							goto LABEL_53;
						case PC_SORCERER:
							v4 = PS_MAGE35;
							goto LABEL_53;
					}
				}
				return;
			}
			v6 = UseScroll();
		}
LABEL_36:
		v0 = myplr;
		if ( v6 )
		{
			v7 = plr[myplr]._pRSpell;
			if ( v7 == 6 )
			{
				v8 = GetDirection(plr[myplr].WorldX, plr[myplr].WorldY, cursmx, cursmy);
				v9 = GetSpellLevel(myplr, plr[myplr]._pRSpell);
				v10 = 21720 * myplr;
				_LOWORD(v10) = plr[myplr]._pRSpell;
				NetSendCmdLocParam3(1u, CMD_SPELLXYD, cursmx, cursmy, v10, v8, v9);
			}
			else if ( pcursmonst == -1 )
			{
				if ( pcursplr == -1 )
				{
					v13 = GetSpellLevel(myplr, v7);
					v14 = 21720 * myplr;
					_LOWORD(v14) = plr[myplr]._pRSpell;
					NetSendCmdLocParam2(1u, CMD_SPELLXY, cursmx, cursmy, v14, v13);
				}
				else
				{
					v12 = GetSpellLevel(myplr, v7);
					NetSendCmdParam3(1u, CMD_SPELLPID, pcursplr, plr[myplr]._pRSpell, v12);
				}
			}
			else
			{
				v11 = GetSpellLevel(myplr, v7);
				NetSendCmdParam3(1u, CMD_SPELLID, pcursmonst, plr[myplr]._pRSpell, v11);
			}
			return;
		}
		goto LABEL_46;
	}
	v3 = *((_BYTE *)&plr[0]._pClass + v1);
	switch ( v3 )
	{
		case PC_WARRIOR:
			v4 = PS_WARR34;
LABEL_53:
			PlaySFX(v4);
			return;
		case PC_ROGUE:
			v4 = PS_ROGUE34;
			goto LABEL_53;
		case PC_SORCERER:
			v4 = PS_MAGE34;
			goto LABEL_53;
	}
}
// 4B8CC2: using guessed type char pcursplr;
// 5BB1ED: using guessed type char leveltype;

void __fastcall SyncPlrAnim(int pnum)
{
	int v1; // esi
	int v2; // eax
	int v3; // ecx
	unsigned char *v4; // ecx
	int v5; // edx

	v1 = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("SyncPlrAnim: illegal player %d", pnum);
	v2 = v1;
	v3 = plr[v1]._pdir;
	switch ( plr[v1]._pmode )
	{
		case PM_STAND:
		case PM_NEWLVL:
		case PM_QUIT:
			v4 = plr[0]._pNAnim[v3 + 5430 * v1];
			goto LABEL_19;
		case PM_WALK:
		case PM_WALK2:
		case PM_WALK3:
			v4 = plr[0]._pWAnim[v3 + 5430 * v1];
			goto LABEL_19;
		case PM_ATTACK:
		case PM_RATTACK:
			v4 = plr[0]._pAAnim[v3 + 5430 * v1];
			goto LABEL_19;
		case PM_BLOCK:
			v4 = plr[0]._pBAnim[v3 + 5430 * v1];
			goto LABEL_19;
		case PM_GOTHIT:
			v4 = plr[0]._pHAnim[v3 + 5430 * v1];
			goto LABEL_19;
		case PM_DEATH:
			v4 = plr[0]._pDAnim[v3 + 5430 * v1];
			goto LABEL_19;
		case PM_SPELL:
			if ( v1 == myplr )
				v5 = (unsigned char)spelldata[plr[v2]._pSpell].sType;
			else
				v5 = 0;
			if ( !v5 )
				plr[v2]._pAnimData = plr[0]._pFAnim[v3 + 5430 * v1];
			if ( v5 == STYPE_LIGHTNING )
				plr[v2]._pAnimData = plr[0]._pLAnim[v3 + 5430 * v1];
			if ( v5 == STYPE_MAGIC )
			{
				v4 = plr[0]._pTAnim[v3 + 5430 * v1];
LABEL_19:
				plr[v2]._pAnimData = v4;
			}
			break;
		default:
			TermMsg("SyncPlrAnim");
			break;
	}
}

void __fastcall SyncInitPlrPos(int pnum)
{
	int v1; // esi
	bool v2; // zf
	unsigned int v3; // eax
	int v4; // ebx
	int v5; // edi
	int v6; // eax
	signed int v7; // [esp+Ch] [ebp-18h]
	int p; // [esp+10h] [ebp-14h]
	int v9; // [esp+14h] [ebp-10h]
	signed int v10; // [esp+18h] [ebp-Ch]
	signed int v11; // [esp+1Ch] [ebp-8h]
	unsigned int i; // [esp+20h] [ebp-4h]
	signed int v13; // [esp+20h] [ebp-4h]

	p = pnum;
	v1 = pnum;
	v2 = gbMaxPlayers == 1;
	plr[v1]._ptargx = plr[pnum].WorldX;
	plr[v1]._ptargy = plr[pnum].WorldY;
	if ( !v2 && plr[v1].plrlevel == currlevel )
	{
		v3 = 0;
		for ( i = 0; ; v3 = i )
		{
			v4 = plr[v1].WorldX + *(int *)((char *)plrxoff2 + v3);
			v5 = plr[v1].WorldY + *(int *)((char *)plryoff2 + v3);
			if ( PosOkPlayer(p, v4, v5) )
				break;
			i += 4;
			if ( i >= 0x20 )
				break;
		}
		if ( !PosOkPlayer(p, v4, v5) )
		{
			v11 = 0;
			v6 = -1;
			v13 = 1;
			v7 = -1;
			do
			{
				if ( v11 )
					break;
				v9 = v6;
				while ( v6 <= v13 && !v11 )
				{
					v5 = v9 + plr[v1].WorldY;
					v10 = v7;
					do
					{
						if ( v11 )
							break;
						v4 = v10 + plr[v1].WorldX;
						if ( PosOkPlayer(p, v10 + plr[v1].WorldX, v5) && !PosOkPortal(currlevel, v4, v5) )
							v11 = 1;
						++v10;
					}
					while ( v10 <= v13 );
					v6 = ++v9;
				}
				++v13;
				v6 = v7-- - 1;
			}
			while ( v7 > -50 );
		}
		plr[v1].WorldX = v4;
		v2 = p == myplr;
		plr[v1].WorldY = v5;
		dPlayer[v4][v5] = p + 1;
		if ( v2 )
		{
			plr[v1]._px = v4;
			plr[v1]._py = v5;
			plr[v1]._ptargx = v4;
			plr[v1]._ptargy = v5;
			ViewX = v4;
			ViewY = v5;
		}
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall SyncInitPlr(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("SyncInitPlr: illegal player %d", pnum);
	}

	SetPlrAnims(pnum);
	SyncInitPlrPos(pnum);
}

void __fastcall CheckStats(int pnum)
{
	int v1; // esi
	int v2; // eax
	char v3; // cl
	signed int v4; // esi
	signed int v5; // edi
	int v6; // edx
	int v7; // ecx
	int v8; // edx
	int v9; // ecx
	int v10; // edx
	int v11; // ecx
	int v12; // edx
	int v13; // ecx
	//signed int v14; // [esp+Ch] [ebp-4h]

	v1 = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("CheckStats: illegal player %d", pnum);
	v2 = v1;
	v3 = plr[v1]._pClass;
	if ( v3 )
	{
		if ( v3 == 1 )
		{
			v4 = 1;
		}
		else if ( v3 == 2 )
		{
			v4 = 2;
		}
		/*else
		{
			v4 = v14;
		}*/
	}
	else
	{
		v4 = 0;
	}
	v5 = 0;
	do
	{
		if ( v5 )
		{
			switch ( v5 )
			{
				case ATTRIB_MAG:
					v10 = plr[v2]._pBaseMag;
					v11 = MaxStats[v4][1];
					if ( v10 <= v11 )
					{
						if ( v10 < 0 )
							plr[v2]._pBaseMag = 0;
					}
					else
					{
						plr[v2]._pBaseMag = v11;
					}
					break;
				case ATTRIB_DEX:
					v8 = plr[v2]._pBaseDex;
					v9 = MaxStats[v4][2];
					if ( v8 <= v9 )
					{
						if ( v8 < 0 )
							plr[v2]._pBaseDex = 0;
					}
					else
					{
						plr[v2]._pBaseDex = v9;
					}
					break;
				case ATTRIB_VIT:
					v6 = plr[v2]._pBaseVit;
					v7 = MaxStats[v4][3];
					if ( v6 <= v7 )
					{
						if ( v6 < 0 )
							plr[v2]._pBaseVit = 0;
					}
					else
					{
						plr[v2]._pBaseVit = v7;
					}
					break;
			}
		}
		else
		{
			v12 = plr[v2]._pBaseStr;
			v13 = MaxStats[v4][0];
			if ( v12 <= v13 )
			{
				if ( v12 < 0 )
					plr[v2]._pBaseStr = 0;
			}
			else
			{
				plr[v2]._pBaseStr = v13;
			}
		}
		++v5;
	}
	while ( v5 < 4 );
}

void __fastcall ModifyPlrStr(int pnum, int l)
{
	int v2; // esi
	int v3; // edi
	int v4; // esi
	char v5; // dl
	int v6; // ecx
	int v7; // eax
	int v8; // ebx
	int v9; // eax
	signed int v10; // ecx
	int p; // [esp+8h] [ebp-4h]

	v2 = pnum;
	v3 = l;
	p = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("ModifyPlrStr: illegal player %d", pnum);
	v4 = v2;
	v5 = plr[v4]._pClass;
	v6 = plr[v4]._pBaseStr;
	v7 = MaxStats[v5][0];
	if ( v6 + v3 > v7 )
		v3 = v7 - v6;
	plr[v4]._pBaseStr = v3 + v6;
	plr[v4]._pStrength += v3;
	v8 = plr[v4]._pStrength;
	if ( v5 == 1 )
	{
		v9 = plr[v4]._pLevel * (v8 + plr[v4]._pDexterity);
		v10 = 200;
	}
	else
	{
		v9 = v8 * plr[v4]._pLevel;
		v10 = 100;
	}
	plr[v4]._pDamageMod = v9 / v10;
	CalcPlrInv(p, 1u);
	if ( p == myplr )
		NetSendCmdParam1(0, CMD_SETSTR, plr[v4]._pBaseStr);
}

void __fastcall ModifyPlrMag(int pnum, int l)
{
	int v2; // esi
	int v3; // edi
	int v4; // esi
	char v5; // dl
	int v6; // ecx
	int v7; // eax
	int v8; // eax
	int v9; // edi
	int p; // [esp+8h] [ebp-4h]

	v2 = pnum;
	v3 = l;
	p = pnum;
	if ( (unsigned int)pnum >= MAX_PLRS )
		TermMsg("ModifyPlrMag: illegal player %d", pnum);
	v4 = v2;
	v5 = plr[v4]._pClass;
	v6 = MaxStats[v5][1];
	v7 = plr[v4]._pBaseMag;
	if ( v7 + v3 > v6 )
		v3 = v6 - v7;
	plr[v4]._pMagic += v3;
	v8 = v3 + v7;
	v9 = v3 << 6;
	plr[v4]._pBaseMag = v8;
	if ( v5 == 2 )
		v9 *= 2;
	plr[v4]._pMaxManaBase += v9;
	plr[v4]._pMaxMana += v9;
	if ( !(plr[v4]._pIFlags & 0x8000000) )
	{
		plr[v4]._pManaBase += v9;
		plr[v4]._pMana += v9;
	}
	CalcPlrInv(p, 1u);
	if ( p == myplr )
		NetSendCmdParam1(0, CMD_SETMAG, plr[v4]._pBaseMag);
}

void __fastcall ModifyPlrDex(int pnum, int l)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("ModifyPlrDex: illegal player %d", pnum);
	}

	int max = MaxStats[plr[pnum]._pClass][ATTRIB_DEX];
	if ( plr[pnum]._pBaseDex + l > max )
	{
		l = max - plr[pnum]._pBaseDex;
	}

	plr[pnum]._pDexterity += l;
	plr[pnum]._pBaseDex += l;
	CalcPlrInv(pnum, 1);

	if ( plr[pnum]._pClass == PC_ROGUE ) {
		plr[pnum]._pDamageMod = plr[pnum]._pLevel * (plr[pnum]._pDexterity + plr[pnum]._pStrength) / 200;
	}

	if ( pnum == myplr ) {
		NetSendCmdParam1(0, CMD_SETDEX, plr[pnum]._pBaseDex);
	}
}

void __fastcall ModifyPlrVit(int pnum, int l)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("ModifyPlrVit: illegal player %d", pnum);
	}

	int max = MaxStats[plr[pnum]._pClass][ATTRIB_VIT];
	if ( plr[pnum]._pBaseVit + l > max )
	{
		l = max - plr[pnum]._pBaseVit;
	}

	plr[pnum]._pVitality += l;
	plr[pnum]._pBaseVit += l;

	int ms = l << 6;
	if ( plr[pnum]._pClass == PC_WARRIOR ) {
		ms *= 2;
	}

	plr[pnum]._pHPBase += ms;
	plr[pnum]._pMaxHPBase += ms;
	plr[pnum]._pHitPoints += ms;
	plr[pnum]._pMaxHP += ms;

	CalcPlrInv(pnum, TRUE);

	if ( pnum == myplr ) {
		NetSendCmdParam1(FALSE, CMD_SETVIT, plr[pnum]._pBaseVit);
	}
}

void __fastcall SetPlayerHitPoints(int pnum, int newhp)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("SetPlayerHitPoints: illegal player %d", pnum);
	}

	plr[pnum]._pHitPoints = newhp;
	plr[pnum]._pHPBase = newhp + plr[pnum]._pMaxHPBase - plr[pnum]._pMaxHP;

	if ( pnum == myplr ) {
		drawhpflag = 1;
	}
}

void __fastcall SetPlrStr(int pnum, int v)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("SetPlrStr: illegal player %d", pnum);
	}

	plr[pnum]._pBaseStr = v;
	CalcPlrInv(pnum, TRUE);

	int dm;
	if ( plr[pnum]._pClass == PC_ROGUE ) {
		dm = plr[pnum]._pLevel * (plr[pnum]._pStrength + plr[pnum]._pDexterity) / 200;
	} else {
		dm = plr[pnum]._pLevel * plr[pnum]._pStrength / 100;
	}

	plr[pnum]._pDamageMod = dm;
}

void __fastcall SetPlrMag(int pnum, int v)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("SetPlrMag: illegal player %d", pnum);
	}

	plr[pnum]._pBaseMag = v;

	int m = v << 6;
	if ( plr[pnum]._pClass == PC_SORCERER ) {
		m *= 2;
	}

	plr[pnum]._pMaxManaBase = m;
	plr[pnum]._pMaxMana = m;
	CalcPlrInv(pnum, TRUE);
}

void __fastcall SetPlrDex(int pnum, int v)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("SetPlrDex: illegal player %d", pnum);
	}

	plr[pnum]._pBaseDex = v;
	CalcPlrInv(pnum, TRUE);

	int dm;
	if ( plr[pnum]._pClass == PC_ROGUE ) {
		dm = plr[pnum]._pLevel * (plr[pnum]._pStrength + plr[pnum]._pDexterity) / 200;
	} else {
		dm = plr[pnum]._pStrength * plr[pnum]._pLevel / 100;
	}

	plr[pnum]._pDamageMod = dm;
}

void __fastcall SetPlrVit(int pnum, int v)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("SetPlrVit: illegal player %d", pnum);
	}

	plr[pnum]._pBaseVit = v;

	int hp = v << 6;
	if ( !_LOBYTE(plr[pnum]._pClass) ) {
		hp *= 2;
	}

	plr[pnum]._pHPBase = hp;
	plr[pnum]._pMaxHPBase = hp;
	CalcPlrInv(pnum, TRUE);
}

void __fastcall InitDungMsgs(int pnum)
{
	if ( (DWORD)pnum >= MAX_PLRS ) {
		TermMsg("InitDungMsgs: illegal player %d", pnum);
	}

	plr[pnum].pDungMsgs = 0;
}

void __cdecl PlayDungMsgs()
{
	int v0; // eax
	int v1; // eax
	char v2; // cl
	char v3; // dl
	char v4; // cl
	char v5; // cl
	char v6; // dl
	char v7; // cl
	char v8; // dl
	char v9; // cl
	char v10; // dl
	char v11; // cl
	char v12; // dl

	v0 = myplr;
	if ( (unsigned int)myplr >= 4 )
	{
		TermMsg("PlayDungMsgs: illegal player %d", myplr);
		v0 = myplr;
	}
	switch ( currlevel )
	{
		case 1u:
			v1 = v0;
			if ( !plr[v1]._pLvlVisited[1] && gbMaxPlayers == currlevel )
			{
				v2 = plr[v1].pDungMsgs;
				if ( !(v2 & 1) )
				{
					v3 = plr[v1]._pClass;
					sfxdelay = 40;
					if ( v3 )
					{
						if ( v3 == 1 )
						{
							sfxdnum = PS_ROGUE97;
						}
						else if ( v3 == 2 )
						{
							sfxdnum = PS_MAGE97;
						}
					}
					else
					{
						sfxdnum = PS_WARR97;
					}
					v4 = v2 | 1;
LABEL_14:
					plr[v1].pDungMsgs = v4;
					return;
				}
			}
			break;
		case 5u:
			v1 = v0;
			if ( !plr[v1]._pLvlVisited[5] && gbMaxPlayers == 1 )
			{
				v5 = plr[v1].pDungMsgs;
				if ( !(v5 & 2) )
				{
					v6 = plr[v1]._pClass;
					sfxdelay = 40;
					if ( v6 )
					{
						if ( v6 == 1 )
						{
							sfxdnum = PS_ROGUE96;
						}
						else if ( v6 == 2 )
						{
							sfxdnum = PS_MAGE96;
						}
					}
					else
					{
						sfxdnum = PS_WARR96B;
					}
					v4 = v5 | 2;
					goto LABEL_14;
				}
			}
			break;
		case 9u:
			v1 = v0;
			if ( !plr[v1]._pLvlVisited[9] && gbMaxPlayers == 1 )
			{
				v7 = plr[v1].pDungMsgs;
				if ( !(v7 & 4) )
				{
					v8 = plr[v1]._pClass;
					sfxdelay = 40;
					if ( v8 )
					{
						if ( v8 == 1 )
						{
							sfxdnum = PS_ROGUE98;
						}
						else if ( v8 == 2 )
						{
							sfxdnum = PS_MAGE98;
						}
					}
					else
					{
						sfxdnum = PS_WARR98;
					}
					v4 = v7 | 4;
					goto LABEL_14;
				}
			}
			break;
		case 13u:
			v1 = v0;
			if ( !plr[v1]._pLvlVisited[13] && gbMaxPlayers == 1 )
			{
				v9 = plr[v1].pDungMsgs;
				if ( !(v9 & 8) )
				{
					v10 = plr[v1]._pClass;
					sfxdelay = 40;
					if ( v10 )
					{
						if ( v10 == 1 )
						{
							sfxdnum = PS_ROGUE99;
						}
						else if ( v10 == 2 )
						{
							sfxdnum = PS_MAGE99;
						}
					}
					else
					{
						sfxdnum = PS_WARR99;
					}
					v4 = v9 | 8;
					goto LABEL_14;
				}
			}
			break;
		case 16u:
			v1 = v0;
			if ( !plr[v1]._pLvlVisited[15] && gbMaxPlayers == 1 )
			{
				v11 = plr[v1].pDungMsgs;
				if ( !(v11 & 0x10) )
				{
					v12 = plr[v1]._pClass;
					sfxdelay = 40;
					if ( !v12 || v12 == 1 || v12 == 2 )
						sfxdnum = PS_DIABLVLINT;
					v4 = v11 | 0x10;
					goto LABEL_14;
				}
			}
			break;
	}
	sfxdelay = 0;
}
// 52A554: using guessed type int sfxdelay;
// 679660: using guessed type char gbMaxPlayers;
