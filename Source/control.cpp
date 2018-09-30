//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
char sgbNextTalkSave; // weak
char sgbTalkSavePos; // weak
void *pDurIcons;
void *pChrButtons;
int drawhpflag; // idb
int dropGoldFlag; // weak
int panbtn[8];
int chrbtn[4];
void *pMultiBtns;
void *pPanelButtons;
void *pChrPanel;
int lvlbtndown; // weak
char sgszTalkSave[8][80];
int dropGoldValue; // idb
int drawmanaflag; // idb
int chrbtnactive; // weak
char sgszTalkMsg[80];
void *pPanelText;
int frame_4B8800; // idb
void *pLifeBuff;
void *pBtmBuff;
void *pTalkBtns;
int pstrjust[4];
int pnumlines; // idb
int pinfoflag; // weak
int talkbtndown[3];
int pSpell; // weak
void *pManaBuff;
int infoclr; // weak
int sgbPlrTalkTbl; // weak // should be char [4]
void *pGBoxBuff;
void *pSBkBtnCel;
char tempstr[260];
int sbooktab; // weak
int pSplType; // weak
int frame; // idb
int initialDropGoldIndex; // idb
int talkflag; // weak
void *pSBkIconCels;
int sbookflag; // weak
int chrflag;
int drawbtnflag; // idb
void *pSpellBkCel;
char infostr[260];
int numpanbtns; // weak
void *pStatusPanel;
char panelstr[256];
int panelflag; // weak
char byte_4B8B88[256];
int initialDropGoldValue; // idb
void *pSpellCels;
int panbtndown; // weak
void *pTalkPanel; // idb
int spselflag; // weak
#endif

const unsigned char fontframe[127] =
{
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,  54,  44,  57,  58,  56,  55,  47,
   40,  41,  59,  39,  50,  37,  51,  52,  36,  27,
   28,  29,  30,  31,  32,  33,  34,  35,  48,  49,
   60,  38,  61,  53,  62,   1,   2,   3,   4,   5,
	6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
   26,  42,  63,  43,  64,  65,   0,   1,   2,   3,
	4,   5,   6,   7,   8,   9,  10,  11,  12,  13,
   14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
   24,  25,  26,  40,  66,  41,  67
};
const unsigned char fontkern[68] =
{
	8,  10,   7,   9,   8,   7,   6,   8,   8,   3,
	3,   8,   6,  11,   9,  10,   6,   9,   9,   6,
	9,  11,  10,  13,  10,  11,   7,   5,   7,   7,
	8,   7,   7,   7,   7,   7,  10,   4,   5,   6,
	3,   3,   4,   3,   6,   6,   3,   3,   3,   3,
	3,   2,   7,   6,   3,  10,  10,   6,   6,   7,
	4,   4,   9,   6,   6,  12,   3,   7
};
const int lineoffset[25] =
{
  456433,
  24576,
  24576,
  24576,
  24756,
  447217,
  465649,
  24576,
  24576,
  24576,
  442609,
  456433,
  470257,
  24576,
  24576,
  439537,
  451057,
  461809,
  473329,
  24576,
  438001,
  447217,
  456433,
  465649,
  474097
};
const unsigned char fontidx[256] =
{
	0,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	1,   1,  32,  33,  34,  35,  36,  37,  38,  39,
   40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
   50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
   60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
   70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
   80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
   90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
  100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
  110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
  120, 121, 122, 123, 124, 125, 126,   1,  67, 117,
  101,  97,  97,  97,  97,  99, 101, 101, 101, 105,
  105, 105,  65,  65,  69,  97,  65, 111, 111, 111,
  117, 117, 121,  79,  85,  99,  76,  89,  80, 102,
   97, 105, 111, 117, 110,  78,  97, 111,  63,   1,
	1,   1,   1,  33,  60,  62, 111,  43,  50,  51,
   39, 117,  80,  46,  44,  49,  48,  62,   1,   1,
	1,  63,  65,  65,  65,  65,  65,  65,  65,  67,
   69,  69,  69,  69,  73,  73,  73,  73,  68,  78,
   79,  79,  79,  79,  79,  88,  48,  85,  85,  85,
   85,  89,  98,  66,  97,  97,  97,  97,  97,  97,
   97,  99, 101, 101, 101, 101, 105, 105, 105, 105,
  111, 110, 111, 111, 111, 111, 111,  47,  48, 117,
  117, 117, 117, 121,  98, 121
};

/* data */

unsigned char SpellITbl[37] =
{
	1,   1,   2,   3,   4,   5,   6,   7,   8,   9,
   28,  13,  12,  18,  16,  14,  18,  19,  11,  20,
   15,  21,  23,  24,  25,  22,  26,  29,  37,  38,
   39,  42,  41,  40,  10,  36,  30
};
int PanBtnPos[8][5] =
{
  { 9,   361, 71, 19, 1 },
  { 9,   387, 71, 19, 0 },
  { 9,   427, 71, 19, 1 },
  { 9,   453, 71, 19, 0 },
  { 560, 361, 71, 19, 1 },
  { 560, 387, 71, 19, 0 },
  { 87,  443, 33, 32, 1 },
  { 527, 443, 33, 32, 1 }
};
char *PanBtnHotKey[8] = { "'c'", "'q'", "Tab", "Esc", "'i'", "'b'", "Enter", NULL };
char *PanBtnStr[8] =
{
  "Character Information",
  "Quests log",
  "Automap",
  "Main Menu",
  "Inventory",
  "Spell book",
  "Send Message",
  "Player Attack"
};
int attribute_inc_rects[4][4] =
{
  { 137, 138, 41, 22 },
  { 137, 166, 41, 22 },
  { 137, 195, 41, 22 },
  { 137, 223, 41, 22 }
};

int SpellPages[6][7] =
{
	{ SPL_NULL,		SPL_FIREBOLT,   SPL_CBOLT,	   SPL_HBOLT,	 SPL_HEAL,	   SPL_HEALOTHER, SPL_FLAME },
	{ SPL_RESURRECT,   SPL_FIREWALL,   SPL_TELEKINESIS, SPL_LIGHTNING, SPL_TOWN,	   SPL_FLASH,	 SPL_STONE },
	{ SPL_RNDTELEPORT, SPL_MANASHIELD, SPL_ELEMENT,	 SPL_FIREBALL,  SPL_WAVE,	   SPL_CHAIN,	 SPL_GUARDIAN },
	{ SPL_NOVA,		SPL_GOLEM,	  SPL_TELEPORT,	SPL_APOCA,	 SPL_BONESPIRIT, SPL_FLARE,	 SPL_ETHEREALIZE },
	{ -1,			  -1,			 -1,			  -1,			-1,			 -1,			-1 },
	{ -1,			  -1,			 -1,			  -1,			-1,			 -1,			-1 }
};

void __fastcall DrawSpellCel(int xp, int yp, char *Trans, int nCel, int w)
{
	char *v5; // ebx
	char *v6; // esi
	char *v7; // edi
	int v9; // edx
	unsigned int v10; // eax
	unsigned int v11; // ecx
	char v14; // cf
	unsigned int v15; // ecx
	int v18; // [esp+Ch] [ebp-Ch]
	int _EAX;
	char *_EBX;

	v5 = &Trans[4 * nCel];
	v6 = &Trans[*(_DWORD *)v5];
	v7 = (char *)gpBuffer + screen_y_times_768[yp] + xp;
	v18 = (int)&v6[*((_DWORD *)v5 + 1) - *(_DWORD *)v5];
	_EBX = byte_4B8B88;
	do
	{
		v9 = w;
		do
		{
			while ( 1 )
			{
				v10 = (unsigned char)*v6++;
				if ( (v10 & 0x80u) == 0 )
					break;
				_LOBYTE(v10) = -(char)v10;
				v7 += v10;
				v9 -= v10;
				if ( !v9 )
					goto LABEL_12;
			}
			v9 -= v10;
			v11 = v10 >> 1;
			if ( v10 & 1 )
			{
				_EAX = *v6++;
				ASM_XLAT(_EAX,_EBX);
				*v7++ = _EAX;
				if ( !v11 )
					continue;
			}
			v14 = v11 & 1;
			v15 = v11 >> 1;
			if ( !v14 )
				goto LABEL_15;
			_EAX = *(_WORD *)v6;
			v6 += 2;
			ASM_XLAT(_EAX,_EBX);
			_EAX = __ROR2__(_EAX, 8);
			ASM_XLAT(_EAX,_EBX);
			*(_WORD *)v7 = __ROR2__(_EAX, 8);
			v7 += 2;
			if ( v15 )
			{
LABEL_15:
				do
				{
					_EAX = *(_DWORD *)v6;
					v6 += 4;
					ASM_XLAT(_EAX,_EBX);
					_EAX = __ROR4__(_EAX, 8);
					ASM_XLAT(_EAX,_EBX);
					_EAX = __ROR4__(_EAX, 8);
					ASM_XLAT(_EAX,_EBX);
					_EAX = __ROR4__(_EAX, 8);
					ASM_XLAT(_EAX,_EBX);
					*(_DWORD *)v7 = __ROR4__(_EAX, 8);
					v7 += 4;
					--v15;
				}
				while ( v15 );
			}
		}
		while ( v9 );
LABEL_12:
		v7 = &v7[-w - 768];
	}
	while ( v6 != (char *)v18 );
}

void __fastcall SetSpellTrans(char t)
{
	signed int v1; // eax
	signed int v2; // eax
	signed int v3; // eax
	char *v4; // ecx
	signed int v5; // eax
	char *v6; // ecx
	signed int v7; // eax
	char *v8; // ecx
	signed int v9; // eax
	char *v10; // ecx

	if ( !t )
	{
		v1 = 0;
		do
		{
			byte_4B8B88[v1] = v1;
			++v1;
		}
		while ( v1 < 128 );
	}
	v2 = 128;
	do
	{
		byte_4B8B88[v2] = v2;
		++v2;
	}
	while ( v2 < 256 );
	byte_4B8B88[255] = 0;
	switch ( t )
	{
		case 1:
			byte_4B8B88[144] = -79;
			byte_4B8B88[145] = -77;
			byte_4B8B88[146] = -75;
			v9 = 176;
			do
			{
				v10 = &byte_4B8B88[v9 + 32];
				byte_4B8B88[v9 - 16] = v9;
				*(v10 - 16) = v9;
				*v10 = v9++;
			}
			while ( v9 < 192 );
			break;
		case 2:
			byte_4B8B88[144] = -95;
			byte_4B8B88[145] = -93;
			byte_4B8B88[146] = -91;
			v7 = 160;
			do
			{
				v8 = &byte_4B8B88[v7 + 48];
				*(v8 - 16) = v7;
				*v8 = v7++;
			}
			while ( v7 < 176 );
			break;
		case 3:
			byte_4B8B88[144] = -47;
			byte_4B8B88[145] = -45;
			byte_4B8B88[146] = -43;
			v5 = 208;
			do
			{
				v6 = &byte_4B8B88[v5 - 16];
				*(v6 - 32) = v5;
				*v6 = v5++;
			}
			while ( v5 < 224 );
			break;
		case 4:
			byte_4B8B88[144] = -15;
			byte_4B8B88[145] = -13;
			byte_4B8B88[146] = -11;
			v3 = 240;
			do
			{
				v4 = &byte_4B8B88[v3 - 48];
				*(v4 - 32) = v3;
				*v4 = v3;
				v4[16] = v3++;
			}
			while ( v3 < 255 );
			byte_4B8B88[175] = 0;
			byte_4B8B88[207] = 0;
			byte_4B8B88[223] = 0;
			break;
	}
}

void __cdecl DrawSpell()
{
	int v0; // ebp
	char v1; // cl
	char v2; // bl
	int v3; // edi
	int v4; // esi
	char v6; // [esp+Fh] [ebp-5h]

	v0 = myplr;
	v1 = plr[myplr]._pRSpell;
	v2 = plr[myplr]._pRSplType;
	v3 = v1;
	v6 = plr[myplr]._pRSpell;
	v4 = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[v1];
	if ( v2 == 1 && v1 != -1 )
	{
		if ( !CheckSpell(myplr, v1, 1, 1) )
			v2 = 4;
		v0 = myplr;
		if ( v4 <= 0 )
			v2 = 4;
	}
	if ( !currlevel && v2 != 4 && !*(_DWORD *)&spelldata[v3].sTownSpell )
		v2 = 4;
	if ( plr[v0]._pRSpell < 0 )
		v2 = 4;
	SetSpellTrans(v2);
	if ( v6 == -1 )
		DrawSpellCel(629, 631, (char *)pSpellCels, 27, 56);
	else
		DrawSpellCel(629, 631, (char *)pSpellCels, (char)SpellITbl[v3], 56);
}

void __cdecl DrawSpellList()
{
	int v0; // esi
	signed int v1; // edi
	int v2; // ecx
	int v3; // eax
	signed int v4; // ebp
	int v5; // eax
	int v6; // esi
	int v7; // eax
	bool v8; // sf
	int v9; // esi
	int v10; // eax
	int v11; // ebp
	int v12; // edx
	int *v13; // ecx
	int *v14; // eax
	signed int v15; // edx
	signed int v16; // edi
	int v17; // [esp+Ch] [ebp-34h]
	__int32 xp; // [esp+10h] [ebp-30h]
	__int32 yp; // [esp+14h] [ebp-2Ch]
	unsigned char *v20; // [esp+18h] [ebp-28h]
	__int32 nCel; // [esp+1Ch] [ebp-24h]
	int v22; // [esp+20h] [ebp-20h]
	__int32 v23; // [esp+24h] [ebp-1Ch]
	signed int v24; // [esp+28h] [ebp-18h]
	unsigned __int64 v25; // [esp+2Ch] [ebp-14h]
	signed __int64 v26; // [esp+34h] [ebp-Ch]

	pSpell = -1;
	infostr[0] = 0;
	v17 = 636;
	xp = 495;
	ClearPanel();
	v0 = myplr;
	v1 = 0;
	v24 = 0;
	do
	{
		switch ( v1 )
		{
			case RSPLTYPE_SKILL:
				SetSpellTrans(0);
				yp = 46;
				v2 = plr[v0]._pAblSpells[0];
				v3 = plr[v0]._pAblSpells[1];
				goto LABEL_10;
			case RSPLTYPE_SPELL:
				yp = 47;
				v2 = plr[v0]._pMemSpells[0];
				v3 = plr[v0]._pMemSpells[1];
				goto LABEL_10;
			case RSPLTYPE_SCROLL:
				SetSpellTrans(2);
				yp = 44;
				v2 = plr[v0]._pScrlSpells[0];
				v3 = plr[v0]._pScrlSpells[1];
				goto LABEL_10;
			case RSPLTYPE_CHARGES:
				SetSpellTrans(3);
				yp = 45;
				v2 = plr[v0]._pISpells[0];
				v3 = plr[v0]._pISpells[1];
LABEL_10:
				v25 = __PAIR__(v3, v2);
				break;
		}
		v20 = &spelldata[1].sTownSpell;
		v4 = 1;
		v26 = (__int64)1;
		v23 = 1;
		v22 = xp - 216;
		do
		{
			if ( !(v25 & v26) )
				goto LABEL_68;
			if ( v1 == RSPLTYPE_SPELL )
			{
				v5 = v0;
				v6 = plr[v0]._pSplLvl[v4];
				v7 = plr[v5]._pISplLvlAdd;
				v8 = v7 + v6 < 0;
				v9 = v7 + v6;
				nCel = v9;
				if ( v8 )
				{
					nCel = 0;
					v9 = 0;
				}
				SetSpellTrans(v9 <= 0 ? 4 : 1);
			}
			else
			{
				v9 = nCel;
			}
			if ( !currlevel && !*(_DWORD *)v20 )
				SetSpellTrans(4);
			DrawSpellCel(v17, xp, (char *)pSpellCels, (char)SpellITbl[v4], 56);
			if ( MouseX >= v17 - 64 && MouseX < v17 - 64 + 56 && MouseY >= v22 && MouseY < v22 + 56 )
			{
				pSpell = v4;
				pSplType = v1;
				DrawSpellCel(v17, xp, (char *)pSpellCels, yp, 56);
				if ( v1 )
				{
					switch ( v1 )
					{
						case RSPLTYPE_SPELL:
							sprintf(infostr, "%s Spell", spelldata[pSpell].sNameText);
							if ( pSpell == 31 )
							{
								sprintf(tempstr, "Damages undead only");
								AddPanelString(tempstr, 1);
							}
							if ( v9 )
								sprintf(tempstr, "Spell Level %i", v9);
							else
								sprintf(tempstr, "Spell Level 0 - Unusable");
LABEL_32:
							AddPanelString(tempstr, 1);
							break;
						case RSPLTYPE_SCROLL:
							sprintf(infostr, "Scroll of %s", spelldata[pSpell].sNameText);
							v10 = myplr;
							v11 = 0;
							v12 = plr[myplr]._pNumInv;
							if ( v12 > 0 )
							{
								v13 = &plr[v10].InvList[0]._iMiscId;
								do
								{
									if ( *(v13 - 53) != -1
									  && (*v13 == IMISC_SCROLL || *v13 == IMISC_SCROLLT)
									  && v13[1] == pSpell )
									{
										++v11;
									}
									v13 += 92;
									--v12;
								}
								while ( v12 );
							}
							v14 = &plr[v10].SpdList[0]._iMiscId;
							v15 = 8;
							do
							{
								if ( *(v14 - 53) != -1
								  && (*v14 == IMISC_SCROLL || *v14 == IMISC_SCROLLT)
								  && v14[1] == pSpell )
								{
									++v11;
								}
								v14 += 92;
								--v15;
							}
							while ( v15 );
							if ( v11 == 1 )
								strcpy(tempstr, "1 Scroll");
							else
								sprintf(tempstr, "%i Scrolls", v11);
							AddPanelString(tempstr, 1);
							v4 = v23;
							break;
						case RSPLTYPE_CHARGES:
							sprintf(infostr, "Staff of %s", spelldata[pSpell].sNameText);
							if ( plr[myplr].InvBody[4]._iCharges == 1 )
								strcpy(tempstr, "1 Charge");
							else
								sprintf(tempstr, "%i Charges", plr[myplr].InvBody[4]._iCharges);
							goto LABEL_32;
					}
				}
				else
				{
					sprintf(infostr, "%s Skill", spelldata[pSpell].sSkillText);
				}
				v0 = myplr;
				v16 = 0;
				do
				{
					if ( plr[0]._pSplHotKey[v16 + 5430 * v0] == pSpell && plr[v0]._pSplTHotKey[v16] == pSplType )
					{
						DrawSpellCel(v17, xp, (char *)pSpellCels, v16 + 48, 56);
						sprintf(tempstr, "Spell Hot Key #F%i", v16 + 5);
						AddPanelString(tempstr, 1);
						v0 = myplr;
					}
					++v16;
				}
				while ( v16 < 4 );
				v1 = v24;
				goto LABEL_66;
			}
			v0 = myplr;
LABEL_66:
			v17 -= 56;
			if ( v17 == 20 )
			{
				xp -= 56;
				v22 -= 56;
				v17 = 636;
			}
LABEL_68:
			v20 += 56;
			++v4;
			v26 *= (__int64)2;
			v23 = v4;
		}
		while ( (signed int)v20 < (signed int)&spelldata[37].sTownSpell );
		if ( v25 && v17 != 636 )
			v17 -= 56;
		if ( v17 == 20 )
		{
			xp -= 56;
			v17 = 636;
		}
		v24 = ++v1;
	}
	while ( v1 < 4 );
}
// 4B8834: using guessed type int pSpell;
// 4B8954: using guessed type int pSplType;

void __cdecl SetSpell()
{
	int v0; // eax

	spselflag = 0;
	if ( pSpell != -1 )
	{
		ClearPanel();
		v0 = myplr;
		drawpanflag = 255;
		plr[v0]._pRSpell = pSpell;
		_LOBYTE(plr[v0]._pRSplType) = pSplType;
	}
}
// 4B8834: using guessed type int pSpell;
// 4B8954: using guessed type int pSplType;
// 4B8C98: using guessed type int spselflag;
// 52571C: using guessed type int drawpanflag;

void __fastcall SetSpeedSpell(int slot)
{
	int v1; // esi
	int v2; // eax
	signed int v3; // ebp
	int v4; // edx
	int v5; // ebx
	int *v6; // edi

	v1 = pSpell;
	if ( pSpell != -1 )
	{
		v2 = myplr;
		v3 = 0;
		v4 = myplr;
		v5 = pSplType;
		v6 = plr[myplr]._pSplHotKey;
		do
		{
			if ( *v6 == v1 && plr[v4]._pSplTHotKey[v3] == v5 )
				*v6 = -1;
			++v3;
			++v6;
		}
		while ( v3 < 4 );
		plr[0]._pSplHotKey[slot + 5430 * v2] = v1;
		plr[v4]._pSplTHotKey[slot] = v5;
	}
}
// 4B8834: using guessed type int pSpell;
// 4B8954: using guessed type int pSplType;

void __fastcall ToggleSpell(int slot)
{
	int v1; // eax
	int v2; // edx
	int v3; // esi
	char *v4; // eax
	int v5; // eax
	int v6; // eax
	int v7; // eax
	int v8; // ebx
	int v9; // edi
	//int v10; // [esp+4h] [ebp-Ch]
	char *v11; // [esp+8h] [ebp-8h]
	int v12; // [esp+Ch] [ebp-4h]

	v1 = slot + 5430 * myplr;
	v2 = plr[0]._pSplHotKey[v1];
	v12 = plr[0]._pSplHotKey[v1];
	if ( v2 != -1 )
	{
		v3 = myplr;
		v4 = &plr[myplr]._pSplTHotKey[slot];
		v11 = v4;
		v5 = *v4;
		if ( v5 )
		{
			v6 = v5 - 1;
			if ( v6 )
			{
				v7 = v6 - 1;
				if ( v7 )
				{
					if ( v7 == 1 )
					{
						v8 = plr[v3]._pISpells[0];
						v9 = plr[v3]._pISpells[1];
					}
					else
					{
						v9 = (int)v11;
						v8 = plr[myplr]._pSplHotKey[slot]; /* check */
					}
				}
				else
				{
					v8 = plr[v3]._pScrlSpells[0];
					v9 = plr[v3]._pScrlSpells[1];
				}
			}
			else
			{
				v8 = plr[v3]._pMemSpells[0];
				v9 = plr[v3]._pMemSpells[1];
			}
		}
		else
		{
			v8 = plr[v3]._pAblSpells[0];
			v9 = plr[v3]._pAblSpells[1];
		}
		if ( v9 & ((unsigned __int64)((__int64)1 << ((unsigned char)v2 - 1)) >> 32) | v8 & (unsigned int)((__int64)1 << ((unsigned char)v2 - 1)) )
		{
			drawpanflag = 255;
			plr[v3]._pRSpell = v12;
			_LOBYTE(plr[v3]._pRSplType) = *v11;
		}
	}
}
// 52571C: using guessed type int drawpanflag;

void __fastcall CPrintString(int No, unsigned char pszStr, int Just)
{
	int *v3; // ebx
	char *v4; // esi
	char *v5; // edi
	int v6; // ebx
	signed int v7; // edx
	unsigned int v8; // eax
	unsigned int v9; // ecx
	char v10; // cf
	unsigned int v11; // ecx
	signed int v12; // edx
	int v13; // eax
	int v14; // ecx
	char v15; // al
	signed int v16; // edx
	int v17; // eax
	int v18; // ecx
	char v19; // al
	signed int v20; // edx
	int v21; // eax
	int v22; // ecx
	char v23; // al

	v3 = (int *)((char *)pPanelText + 4 * pszStr);
	v4 = (char *)pPanelText + *v3;
	v5 = (char *)gpBuffer + No;
	v6 = (int)&v4[v3[1] - *v3];
	if ( (_BYTE)Just )
	{
		if ( (unsigned char)Just == 1 )
		{
			do
			{
				v12 = 13;
				do
				{
					while ( 1 )
					{
						v13 = (unsigned char)*v4++;
						if ( (v13 & 0x80u) == 0 )
							break;
						_LOBYTE(v13) = -(char)v13;
						v5 += v13;
						v12 -= v13;
						if ( !v12 )
							goto LABEL_28;
					}
					v12 -= v13;
					v14 = v13;
					do
					{
						v15 = *v4++;
						if ( (unsigned char)v15 > 0xFDu )
						{
							v15 = -65;
						}
						else if ( (unsigned char)v15 >= 0xF0u )
						{
							v15 -= 62;
						}
						*v5++ = v15;
						--v14;
					}
					while ( v14 );
				}
				while ( v12 );
LABEL_28:
				v5 -= 781;
			}
			while ( (char *)v6 != v4 );
		}
		else if ( (unsigned char)Just == 2 )
		{
			do
			{
				v16 = 13;
				do
				{
					while ( 1 )
					{
						v17 = (unsigned char)*v4++;
						if ( (v17 & 0x80u) == 0 )
							break;
						_LOBYTE(v17) = -(char)v17;
						v5 += v17;
						v16 -= v17;
						if ( !v16 )
							goto LABEL_39;
					}
					v16 -= v17;
					v18 = v17;
					do
					{
						v19 = *v4++;
						if ( (unsigned char)v19 >= 0xF0u )
							v19 -= 16;
						*v5++ = v19;
						--v18;
					}
					while ( v18 );
				}
				while ( v16 );
LABEL_39:
				v5 -= 781;
			}
			while ( (char *)v6 != v4 );
		}
		else
		{
			do
			{
				v20 = 13;
				do
				{
					while ( 1 )
					{
						v21 = (unsigned char)*v4++;
						if ( (v21 & 0x80u) == 0 )
							break;
						_LOBYTE(v21) = -(char)v21;
						v5 += v21;
						v20 -= v21;
						if ( !v20 )
							goto LABEL_52;
					}
					v20 -= v21;
					v22 = v21;
					do
					{
						v23 = *v4++;
						if ( (unsigned char)v23 >= 0xF0u )
						{
							if ( (unsigned char)v23 >= 0xFEu )
								v23 = -49;
							else
								v23 -= 46;
						}
						*v5++ = v23;
						--v22;
					}
					while ( v22 );
				}
				while ( v20 );
LABEL_52:
				v5 -= 781;
			}
			while ( (char *)v6 != v4 );
		}
	}
	else
	{
		do
		{
			v7 = 13;
			do
			{
				while ( 1 )
				{
					v8 = (unsigned char)*v4++;
					if ( (v8 & 0x80u) == 0 )
						break;
					_LOBYTE(v8) = -(char)v8;
					v5 += v8;
					v7 -= v8;
					if ( !v7 )
						goto LABEL_15;
				}
				v7 -= v8;
				v9 = v8 >> 1;
				if ( v8 & 1 )
				{
					*v5++ = *v4++;
					if ( !v9 )
						continue;
				}
				v10 = v9 & 1;
				v11 = v8 >> 2;
				if ( v10 )
				{
					*(_WORD *)v5 = *(_WORD *)v4;
					v4 += 2;
					v5 += 2;
					if ( !v11 )
						continue;
				}
				qmemcpy(v5, v4, 4 * v11);
				v4 += 4 * v11;
				v5 += 4 * v11;
			}
			while ( v7 );
LABEL_15:
			v5 -= 781;
		}
		while ( (char *)v6 != v4 );
	}
}

void __fastcall AddPanelString(char *str, int just)
{
	strcpy(&panelstr[64 * pnumlines], str);
	pstrjust[pnumlines] = just;

	if ( pnumlines < 4 )
		pnumlines++;
}

void __cdecl ClearPanel()
{
	pnumlines = 0;
	pinfoflag = 0;
}
// 4B8824: using guessed type int pinfoflag;

void __fastcall DrawPanelBox(int x, int y, int w, int h, int sx, int sy)
{
	char *v6; // esi
	char *v7; // edi
	int v8; // edx
	unsigned int v9; // ecx
	char v10; // cf
	unsigned int v11; // ecx

	v6 = (char *)pBtmBuff + 640 * y + x;
	v7 = &gpBuffer->row_unused_1[sy].col_unused_1[sx];
	v8 = h;
	do
	{
		v9 = w >> 1;
		if ( !(w & 1) || (*v7 = *v6, ++v6, ++v7, v9) )
		{
			v10 = v9 & 1;
			v11 = w >> 2;
			if ( !v10 || (*(_WORD *)v7 = *(_WORD *)v6, v6 += 2, v7 += 2, v11) )
			{
				qmemcpy(v7, v6, 4 * v11);
				v6 += 4 * v11;
				v7 += 4 * v11;
			}
		}
		v6 = &v6[-w + 640];
		v7 = &v7[-w + 768];
		--v8;
	}
	while ( v8 );
}

void __fastcall SetFlaskHeight(char *buf, int min, int max, int c, int r)
{
	char *v5; // esi
	char *v6; // edi
	int v7; // edx

	v5 = &buf[88 * min];
	v6 = &gpBuffer->row_unused_1[r].col_unused_1[c];
	v7 = max - min;
	do
	{
		qmemcpy(v6, v5, 0x58u);
		v5 += 88;
		v6 += 768;
		--v7;
	}
	while ( v7 );
}

void __fastcall DrawFlask(void *a1, int a2, int a3, void *a4, int a5, int a6)
{
	char *v6; // esi
	_BYTE *v7; // edi
	int v8; // edx
	signed int v9; // ecx
	char v10; // al
	int v11; // [esp+Ch] [ebp-4h]

	v11 = a2;
	v6 = (char *)a1 + a3;
	v7 = (unsigned char *)a4 + a5;
	v8 = a6;
	do
	{
		v9 = 59;
		do
		{
			v10 = *v6++;
			if ( v10 )
				*v7 = v10;
			++v7;
			--v9;
		}
		while ( v9 );
		v6 = &v6[v11 - 59];
		v7 += 709;
		--v8;
	}
	while ( v8 );
}

void __cdecl DrawLifeFlask()
{
	signed __int64 v0; // rax
	signed int v1; // esi
	int v2; // esi

	v0 = (signed __int64)((double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP * 80.0);
	plr[myplr]._pHPPer = v0;
	if ( (signed int)v0 > 80 )
		LODWORD(v0) = 80;
	v1 = 80 - v0;
	if ( 80 - (signed int)v0 > 11 )
		v1 = 11;
	v2 = v1 + 2;
	DrawFlask(pLifeBuff, 88, 277, gpBuffer, 383405, v2);
	if ( v2 != 13 )
		DrawFlask(pBtmBuff, 640, 640 * v2 + 2029, gpBuffer, 768 * v2 + 383405, 13 - v2);
}

void __cdecl UpdateLifeFlask()
{
	signed __int64 v0; // rax
	signed int v1; // edi

	v0 = (signed __int64)((double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP * 80.0);
	v1 = v0;
	plr[myplr]._pHPPer = v0;
	if ( (signed int)v0 > 69 )
	{
		v1 = 69;
LABEL_8:
		DrawPanelBox(96, 85 - v1, 0x58u, v1, 160, 581 - v1);
		return;
	}
	if ( (signed int)v0 < 0 )
		v1 = 0;
	if ( v1 != 69 )
		SetFlaskHeight((char *)pLifeBuff, 16, 85 - v1, 160, 512);
	if ( v1 )
		goto LABEL_8;
}

void __cdecl DrawManaFlask()
{
	int v0; // eax
	int v1; // esi
	int v2; // esi

	v0 = plr[myplr]._pManaPer;
	if ( v0 > 80 )
		v0 = 80;
	v1 = 80 - v0;
	if ( 80 - v0 > 11 )
		v1 = 11;
	v2 = v1 + 2;
	DrawFlask(pManaBuff, 88, 277, gpBuffer, 383771, v2);
	if ( v2 != 13 )
		DrawFlask(pBtmBuff, 640, 640 * v2 + 2395, gpBuffer, 768 * v2 + 383771, 13 - v2);
}

void __cdecl control_update_life_mana()
{
	int v0; // esi
	signed __int64 v1; // rax
	int v2; // [esp+4h] [ebp-8h]
	int v3; // [esp+8h] [ebp-4h]

	v0 = myplr;
	v3 = plr[myplr]._pMaxMana;
	v2 = plr[myplr]._pMana;
	if ( plr[myplr]._pMaxMana < 0 )
		v3 = 0;
	if ( plr[myplr]._pMana < 0 )
		v2 = 0;
	if ( v3 )
		v1 = (signed __int64)((double)v2 / (double)v3 * 80.0);
	else
		LODWORD(v1) = 0;
	plr[v0]._pManaPer = v1;
	plr[v0]._pHPPer = (signed __int64)((double)plr[v0]._pHitPoints / (double)plr[v0]._pMaxHP * 80.0);
}

void __cdecl UpdateManaFlask()
{
	signed int v0; // edi
	int v1; // [esp+8h] [ebp-8h]
	int v2; // [esp+Ch] [ebp-4h]

	v2 = plr[myplr]._pMaxMana;
	v1 = plr[myplr]._pMana;
	if ( plr[myplr]._pMaxMana < 0 )
		v2 = 0;
	if ( plr[myplr]._pMana < 0 )
		v1 = 0;
	if ( v2 )
		v0 = (signed __int64)((double)v1 / (double)v2 * 80.0);
	else
		v0 = 0;
	plr[myplr]._pManaPer = v0;
	if ( v0 > 69 )
		v0 = 69;
	if ( v0 != 69 )
		SetFlaskHeight((char *)pManaBuff, 16, 85 - v0, 528, 512);
	if ( v0 )
		DrawPanelBox(464, 85 - v0, 0x58u, v0, 528, 581 - v0);
	DrawSpell();
}

void __cdecl InitControlPan()
{
	size_t v0; // esi
	void *v1; // ecx
	void *v2; // ecx
	void *v3; // ecx
	char v4; // al
	unsigned char *v5; // eax

	v0 = 0x16800;
	if ( gbMaxPlayers != 1 )
		v0 = 0x2D000;
	pBtmBuff = DiabloAllocPtr(v0);
	memset(pBtmBuff, 0, v0);
	pManaBuff = DiabloAllocPtr(0x1E40);
	memset(pManaBuff, 0, 0x1E40u);
	pLifeBuff = DiabloAllocPtr(0x1E40);
	memset(pLifeBuff, 0, 0x1E40u);
	pPanelText = LoadFileInMem("CtrlPan\\SmalText.CEL", 0);
	pChrPanel = LoadFileInMem("Data\\Char.CEL", 0);
	pSpellCels = LoadFileInMem("CtrlPan\\SpelIcon.CEL", 0);
	SetSpellTrans(0);
	pStatusPanel = LoadFileInMem("CtrlPan\\Panel8.CEL", 0);
	CelDecodeRect((char *)pBtmBuff, 0, 143, 640, (char *)pStatusPanel, 1, 640);
	v1 = pStatusPanel;
	pStatusPanel = 0;
	mem_free_dbg(v1);
	pStatusPanel = LoadFileInMem("CtrlPan\\P8Bulbs.CEL", 0);
	CelDecodeRect((char *)pLifeBuff, 0, 87, 88, (char *)pStatusPanel, 1, 88);
	CelDecodeRect((char *)pManaBuff, 0, 87, 88, (char *)pStatusPanel, 2, 88);
	v2 = pStatusPanel;
	pStatusPanel = 0;
	mem_free_dbg(v2);
	talkflag = 0;
	if ( gbMaxPlayers != 1 )
	{
		pTalkPanel = LoadFileInMem("CtrlPan\\TalkPanl.CEL", 0);
		CelDecodeRect((char *)pBtmBuff, 0, 287, 640, (char *)pTalkPanel, 1, 640);
		v3 = pTalkPanel;
		pTalkPanel = 0;
		mem_free_dbg(v3);
		pMultiBtns = LoadFileInMem("CtrlPan\\P8But2.CEL", 0);
		pTalkBtns = LoadFileInMem("CtrlPan\\TalkButt.CEL", 0);
		sgbPlrTalkTbl = 0;
		*(_DWORD *)&tempstr[256] = 0x1010101;
		talkbtndown[0] = 0;
		talkbtndown[1] = 0;
		sgszTalkMsg[0] = 0;
		talkbtndown[2] = 0;
	}
	panelflag = 0;
	lvlbtndown = 0;
	pPanelButtons = LoadFileInMem("CtrlPan\\Panel8bu.CEL", 0);
	memset(panbtn, 0, sizeof(panbtn));
	panbtndown = 0;
	numpanbtns = 2 * (gbMaxPlayers != 1) + 6;
	pChrButtons = LoadFileInMem("Data\\CharBut.CEL", 0);
	chrbtn[0] = 0;
	chrbtn[1] = 0;
	chrbtn[2] = 0;
	chrbtnactive = 0;
	chrbtn[3] = 0;
	pDurIcons = LoadFileInMem("Items\\DurIcons.CEL", 0);
	strcpy(infostr, &empty_string);
	ClearPanel();
	drawhpflag = 1;
	drawmanaflag = 1;
	chrflag = 0;
	spselflag = 0;
	pSpellBkCel = LoadFileInMem("Data\\SpellBk.CEL", 0);
	pSBkBtnCel = LoadFileInMem("Data\\SpellBkB.CEL", 0);
	pSBkIconCels = LoadFileInMem("Data\\SpellI2.CEL", 0);
	sbooktab = 0;
	sbookflag = 0;
	v4 = plr[myplr]._pClass;
	if ( v4 )
	{
		if ( v4 == UI_ROGUE )
		{
			SpellPages[0][0] = SPL_DISARM;
		}
		else if ( v4 == UI_SORCERER )
		{
			SpellPages[0][0] = SPL_RECHARGE;
		}
	}
	else
	{
		SpellPages[0][0] = SPL_REPAIR;
	}
	pQLogCel = LoadFileInMem("Data\\Quest.CEL", 0);
	v5 = LoadFileInMem("CtrlPan\\Golddrop.cel", 0);
	frame_4B8800 = 1;
	dropGoldFlag = 0;
	dropGoldValue = 0;
	initialDropGoldValue = 0;
	initialDropGoldIndex = 0;
	pGBoxBuff = v5;
}
// 4B84DC: using guessed type int dropGoldFlag;
// 4B851C: using guessed type int lvlbtndown;
// 4B87A8: using guessed type int chrbtnactive;
// 4B8840: using guessed type int sgbPlrTalkTbl;
// 4B8950: using guessed type int sbooktab;
// 4B8960: using guessed type int talkflag;
// 4B8968: using guessed type int sbookflag;
// 4B8A7C: using guessed type int numpanbtns;
// 4B8B84: using guessed type int panelflag;
// 4B8C90: using guessed type int panbtndown;
// 4B8C98: using guessed type int spselflag;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl ClearCtrlPan()
{
	DrawPanelBox(0, sgbPlrTalkTbl + 16, 0x280u, 0x80u, 64, 512);
	DrawInfoBox();
}
// 4B8840: using guessed type int sgbPlrTalkTbl;

void __cdecl DrawCtrlPan()
{
	signed int v0; // edi
	int *v1; // esi
	int v2; // ecx
	int v3; // eax

	v0 = 0;
	v1 = (int *)PanBtnPos;
	do
	{
		v2 = *v1;
		if ( panbtn[v0] )
			CelDecodeOnly(v2 + 64, v1[1] + 178, pPanelButtons, v0 + 1, 71);
		else
			DrawPanelBox(v2, v1[1] - 336, 0x47u, 0x14u, v2 + 64, v1[1] + 160);
		++v0;
		v1 += 5;
	}
	while ( v0 < 6 );
	if ( numpanbtns == 8 )
	{
		CelDecodeOnly(151, 634, pMultiBtns, panbtn[6] + 1, 33);
		if ( FriendlyMode )
			v3 = panbtn[7] + 3;
		else
			v3 = panbtn[7] + 5;
		CelDecodeOnly(591, 634, pMultiBtns, v3, 33);
	}
}
// 484368: using guessed type int FriendlyMode;
// 4B8A7C: using guessed type int numpanbtns;

void __cdecl DoSpeedBook()
{
	int v0; // eax
	signed int v1; // ebx
	bool v2; // zf
	unsigned __int64 v3; // rdi
	unsigned int v4; // ecx
	//unsigned int v5; // [esp+4h] [ebp-20h]
	//unsigned int v6; // [esp+8h] [ebp-1Ch]
	unsigned int v7; // [esp+8h] [ebp-1Ch]
	int X; // [esp+Ch] [ebp-18h]
	int Y; // [esp+10h] [ebp-14h]
	signed int v10; // [esp+14h] [ebp-10h]
	int v11; // [esp+18h] [ebp-Ch]
	signed int v12; // [esp+1Ch] [ebp-8h]
	signed int v13; // [esp+20h] [ebp-4h]

	v0 = myplr;
	v13 = 636;
	v1 = 1;
	v2 = plr[myplr]._pRSpell == -1;
	spselflag = 1;
	v12 = 495;
	X = 600;
	Y = 307;
	if ( !v2 )
	{
		v11 = 0;
		//v3 = __PAIR__(v5, v6);
		while ( 1 )
		{
			if ( v11 )
			{
				switch ( v11 )
				{
					case RSPLTYPE_SPELL:
						HIDWORD(v3) = plr[v0]._pMemSpells[0];
						LODWORD(v3) = plr[v0]._pMemSpells[1];
						break;
					case RSPLTYPE_SCROLL:
						HIDWORD(v3) = plr[v0]._pScrlSpells[0];
						LODWORD(v3) = plr[v0]._pScrlSpells[1];
						break;
					case RSPLTYPE_CHARGES:
						HIDWORD(v3) = plr[v0]._pISpells[0];
						LODWORD(v3) = plr[v0]._pISpells[1];
						break;
				}
			}
			else
			{
				HIDWORD(v3) = plr[v0]._pAblSpells[0];
				LODWORD(v3) = plr[v0]._pAblSpells[1];
			}
			v7 = 0;
			v10 = 1;
			do
			{
				if ( (unsigned int)v3 & v7 | HIDWORD(v3) & v1 )
				{
					if ( v10 == plr[v0]._pRSpell && v11 == SLOBYTE(plr[v0]._pRSplType) )
					{
						X = v13 - 36;
						Y = v12 - 188;
					}
					v13 -= 56;
					if ( v13 == 20 )
					{
						v12 -= 56;
						v13 = 636;
					}
				}
				v4 = __PAIR__(v7, v1) >> 31;
				v1 *= 2;
				++v10;
				v7 = v4;
			}
			while ( v10 < 37 );
			if ( v3 && v13 != 636 )
				v13 -= 56;
			if ( v13 == 20 )
			{
				v12 -= 56;
				v13 = 636;
			}
			if ( ++v11 >= 4 )
				break;
			v1 = 1;
		}
	}
	SetCursorPos(X, Y);
}
// 4B8C98: using guessed type int spselflag;

void __cdecl DoPanBtn()
{
	int v0; // edx
	int v1; // ebx
	int v2; // edi
	int v3; // esi
	int (*v4)[5]; // eax
	int v5; // ecx

	v0 = MouseX;
	v1 = MouseY;
	v2 = numpanbtns;
	v3 = 0;
	if ( numpanbtns > 0 )
	{
		v4 = PanBtnPos;
		do
		{
			if ( v0 >= (*v4)[0] && v0 <= (*v4)[0] + (*v4)[2] )
			{
				v5 = (*v4)[1];
				if ( v1 >= v5 && v1 <= v5 + (*v4)[3] )
				{
					panbtn[v3] = 1;
					drawbtnflag = 1;
					panbtndown = 1;
				}
			}
			++v3;
			++v4;
		}
		while ( v3 < v2 );
	}
	if ( !spselflag && v0 >= 565 && v0 < 621 && v1 >= 416 && v1 < 472 )
	{
		DoSpeedBook();
		gamemenu_off();
	}
}
// 4B8A7C: using guessed type int numpanbtns;
// 4B8C90: using guessed type int panbtndown;
// 4B8C98: using guessed type int spselflag;

void __fastcall control_set_button_down(int btn_id)
{
	panbtn[btn_id] = 1;
	drawbtnflag = 1;
	panbtndown = 1;
}
// 4B8C90: using guessed type int panbtndown;

void __cdecl control_check_btn_press()
{
	int v0; // edx
	int v1; // esi

	v0 = MouseX;
	v1 = MouseY;
	if ( MouseX >= PanBtnPos[3][0]
	  && MouseX <= PanBtnPos[3][0] + PanBtnPos[3][2]
	  && MouseY >= PanBtnPos[3][1]
	  && MouseY <= PanBtnPos[3][1] + PanBtnPos[3][3] )
	{
		control_set_button_down(3);
	}
	if ( v0 >= PanBtnPos[6][0]
	  && v0 <= PanBtnPos[6][0] + PanBtnPos[6][2]
	  && v1 >= PanBtnPos[6][1]
	  && v1 <= PanBtnPos[6][1] + PanBtnPos[6][3] )
	{
		control_set_button_down(6);
	}
}

void __cdecl DoAutoMap()
{
	if ( currlevel || gbMaxPlayers != 1 )
	{
		if ( automapflag )
			automapflag = 0;
		else
			StartAutomap();
	}
	else
	{
		InitDiabloMsg(1);
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __cdecl CheckPanelInfo()
{
	int v0; // edi
	int v1; // eax
	int v2; // ecx
	int v3; // ecx
	int v4; // edi
	int v5; // eax
	int *v6; // edx
	int v7; // ebx
	int v8; // ebx
	int *v9; // eax
	signed int v10; // edx
	int v11; // ecx
	int v12; // [esp+10h] [ebp-4h]

	v0 = 0;
	panelflag = 0;
	ClearPanel();
	if ( numpanbtns > 0 )
	{
		do
		{
			v1 = v0;
			v2 = PanBtnPos[v0][0];
			if ( MouseX >= v2 && MouseX <= v2 + PanBtnPos[v1][2] )
			{
				v3 = PanBtnPos[v1][1];
				if ( MouseY >= v3 && MouseY <= v3 + PanBtnPos[v1][3] )
				{
					if ( v0 == 7 )
					{
						if ( FriendlyMode )
							strcpy(infostr, "Player friendly");
						else
							strcpy(infostr, "Player attack");
					}
					else
					{
						strcpy(infostr, PanBtnStr[v0]);
					}
					if ( PanBtnHotKey[v0] )
					{
						sprintf(tempstr, "Hotkey : %s", PanBtnHotKey[v0]);
						AddPanelString(tempstr, 1);
					}
					_LOBYTE(infoclr) = 0;
					panelflag = 1;
					pinfoflag = 1;
				}
			}
			++v0;
		}
		while ( v0 < numpanbtns );
	}
	if ( !spselflag && MouseX >= 565 && MouseX < 621 && MouseY >= 416 && MouseY < 472 )
	{
		strcpy(infostr, "Select current spell button");
		_LOBYTE(infoclr) = 0;
		panelflag = 1;
		pinfoflag = 1;
		strcpy(tempstr, "Hotkey : 's'");
		AddPanelString(tempstr, 1);
		v4 = plr[myplr]._pRSpell;
		if ( v4 != -1 )
		{
			switch ( _LOBYTE(plr[myplr]._pRSplType) )
			{
				case RSPLTYPE_SKILL:
					sprintf(tempstr, "%s Skill", spelldata[v4].sSkillText);
LABEL_54:
					AddPanelString(tempstr, 1);
					break;
				case RSPLTYPE_SPELL:
					sprintf(tempstr, "%s Spell", spelldata[v4].sNameText);
					AddPanelString(tempstr, 1);
					v11 = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[v4];
					if ( v11 < 0 )
						v11 = 0;
					if ( v11 )
						sprintf(tempstr, "Spell Level %i", v11);
					else
						sprintf(tempstr, "Spell Level 0 - Unusable");
					goto LABEL_54;
				case RSPLTYPE_SCROLL:
					sprintf(tempstr, "Scroll of %s", spelldata[v4].sNameText);
					AddPanelString(tempstr, 1);
					v12 = 0;
					v5 = myplr;
					if ( plr[myplr]._pNumInv > 0 )
					{
						v6 = &plr[v5].InvList[0]._iMiscId;
						v7 = plr[myplr]._pNumInv;
						do
						{
							if ( *(v6 - 53) != -1 && (*v6 == IMISC_SCROLL || *v6 == IMISC_SCROLLT) && v6[1] == v4 )
								++v12;
							v6 += 92;
							--v7;
						}
						while ( v7 );
					}
					v8 = v12;
					v9 = &plr[v5].SpdList[0]._iMiscId;
					v10 = 8;
					do
					{
						if ( *(v9 - 53) != -1 && (*v9 == IMISC_SCROLL || *v9 == IMISC_SCROLLT) && v9[1] == v4 )
							++v8;
						v9 += 92;
						--v10;
					}
					while ( v10 );
					if ( v8 == 1 )
						strcpy(tempstr, "1 Scroll");
					else
						sprintf(tempstr, "%i Scrolls", v8);
					goto LABEL_54;
				case RSPLTYPE_CHARGES:
					sprintf(tempstr, "Staff of %s", spelldata[v4].sNameText);
					AddPanelString(tempstr, 1);
					if ( plr[myplr].InvBody[4]._iCharges == 1 )
						strcpy(tempstr, "1 Charge");
					else
						sprintf(tempstr, "%i Charges", plr[myplr].InvBody[4]._iCharges);
					goto LABEL_54;
			}
		}
	}
	if ( MouseX > 190 && MouseX < 437 && MouseY > 356 && MouseY < 385 )
		pcursinvitem = CheckInvHLight();
}
// 484368: using guessed type int FriendlyMode;
// 4B8824: using guessed type int pinfoflag;
// 4B883C: using guessed type int infoclr;
// 4B8A7C: using guessed type int numpanbtns;
// 4B8B84: using guessed type int panelflag;
// 4B8C98: using guessed type int spselflag;
// 4B8CB8: using guessed type char pcursinvitem;

void __cdecl CheckBtnUp()
{
	signed int v0; // esi
	int *v1; // eax
	int v2; // edx
	signed int v3; // eax
	int v4; // ecx
	int v5; // ecx
	char v6; // [esp+Fh] [ebp-1h]

	v6 = 1;
	drawbtnflag = 1;
	panbtndown = 0;
	v0 = 0;
	do
	{
		v1 = &panbtn[v0];
		if ( *v1 )
		{
			v2 = MouseX;
			*v1 = 0;
			v3 = v0;
			v4 = PanBtnPos[v0][0];
			if ( v2 >= v4 && v2 <= v4 + PanBtnPos[v3][2] )
			{
				v5 = PanBtnPos[v3][1];
				if ( MouseY >= v5 && MouseY <= v5 + PanBtnPos[v3][3] )
				{
					switch ( v0 )
					{
						case PANBTN_CHARINFO:
							questlog = 0;
							chrflag = chrflag == 0;
							break;
						case PANBTN_QLOG:
							chrflag = 0;
							if ( questlog )
								questlog = 0;
							else
								StartQuestlog();
							break;
						case PANBTN_AUTOMAP:
							DoAutoMap();
							break;
						case PANBTN_MAINMENU:
							qtextflag = 0;
							gamemenu_handle_previous();
							v6 = 0;
							break;
						case PANBTN_INVENTORY:
							sbookflag = 0;
							invflag = invflag == 0;
							if ( dropGoldFlag )
							{
								dropGoldFlag = 0;
								dropGoldValue = 0;
							}
							break;
						case PANBTN_SPELLBOOK:
							invflag = 0;
							if ( dropGoldFlag )
							{
								dropGoldFlag = 0;
								dropGoldValue = 0;
							}
							sbookflag = sbookflag == 0;
							break;
						case PANBTN_SENDMSG:
							if ( talkflag )
								control_reset_talk();
							else
								control_type_message();
							break;
						case PANBTN_FRIENDLY:
							FriendlyMode = FriendlyMode == 0;
							break;
						default:
							break;
					}
				}
			}
		}
		++v0;
	}
	while ( v0 < 8 );
	if ( v6 )
		gamemenu_off();
}
// 484368: using guessed type int FriendlyMode;
// 4B84DC: using guessed type int dropGoldFlag;
// 4B8960: using guessed type int talkflag;
// 4B8968: using guessed type int sbookflag;
// 4B8C90: using guessed type int panbtndown;
// 646D00: using guessed type char qtextflag;
// 69BD04: using guessed type int questlog;

void __cdecl FreeControlPan()
{
	void *v0; // ecx
	void *v1; // ecx
	void *v2; // ecx
	void *v3; // ecx
	void *v4; // ecx
	void *v5; // ecx
	void *v6; // ecx
	void *v7; // ecx
	void *v8; // ecx
	void *v9; // ecx
	void *v10; // ecx
	void *v11; // ecx
	void *v12; // ecx
	void *v13; // ecx
	void *v14; // ecx
	void *v15; // ecx

	v0 = pBtmBuff;
	pBtmBuff = 0;
	mem_free_dbg(v0);
	v1 = pManaBuff;
	pManaBuff = 0;
	mem_free_dbg(v1);
	v2 = pLifeBuff;
	pLifeBuff = 0;
	mem_free_dbg(v2);
	v3 = pPanelText;
	pPanelText = 0;
	mem_free_dbg(v3);
	v4 = pChrPanel;
	pChrPanel = 0;
	mem_free_dbg(v4);
	v5 = pSpellCels;
	pSpellCels = 0;
	mem_free_dbg(v5);
	v6 = pPanelButtons;
	pPanelButtons = 0;
	mem_free_dbg(v6);
	v7 = pMultiBtns;
	pMultiBtns = 0;
	mem_free_dbg(v7);
	v8 = pTalkBtns;
	pTalkBtns = 0;
	mem_free_dbg(v8);
	v9 = pChrButtons;
	pChrButtons = 0;
	mem_free_dbg(v9);
	v10 = pDurIcons;
	pDurIcons = 0;
	mem_free_dbg(v10);
	v11 = pQLogCel;
	pQLogCel = 0;
	mem_free_dbg(v11);
	v12 = pSpellBkCel;
	pSpellBkCel = 0;
	mem_free_dbg(v12);
	v13 = pSBkBtnCel;
	pSBkBtnCel = 0;
	mem_free_dbg(v13);
	v14 = pSBkIconCels;
	pSBkIconCels = 0;
	mem_free_dbg(v14);
	v15 = pGBoxBuff;
	pGBoxBuff = 0;
	mem_free_dbg(v15);
}

int __fastcall control_WriteStringToBuffer(char *str)
{
	signed int v1; // edx
	unsigned char v2; // al

	v1 = 0;
	do
	{
		v2 = *str;
		if ( !*str )
			return 1;
		++str;
		v1 += fontkern[fontframe[fontidx[v2]]];
	}
	while ( v1 < 125 );
	return 0;
}

void __cdecl DrawInfoBox()
{
	int v0; // ecx
	int v1; // eax
	int v2; // eax
	int v3; // esi
	char *v4; // eax
	const char *v5; // eax
	char v6; // al
	signed int v7; // edi
	signed int v8; // ebp
	int v9; // esi
	char *v10; // ebx

	DrawPanelBox(177, 62, 0x120u, 0x3Cu, 241, 558);
	v0 = trigflag[3];
	v1 = spselflag;
	if ( !panelflag && !trigflag[3] && pcursinvitem == -1 )
	{
		if ( spselflag )
		{
LABEL_32:
			_LOBYTE(infoclr) = 0;
			goto LABEL_33;
		}
		infostr[0] = 0;
		_LOBYTE(infoclr) = 0;
		ClearPanel();
	}
	if ( v1 || v0 )
		goto LABEL_32;
	if ( pcurs < CURSOR_FIRSTITEM )
	{
		if ( pcursitem != -1 )
			GetItemStr(pcursitem);
		if ( pcursobj != -1 )
			GetObjectStr(pcursobj);
		if ( pcursmonst != -1 )
		{
			if ( leveltype != DTYPE_TOWN)
			{
				_LOBYTE(infoclr) = 0;
				strcpy(infostr, monster[pcursmonst].mName);
				ClearPanel();
				if ( monster[pcursmonst]._uniqtype )
				{
					_LOBYTE(infoclr) = 3;
					PrintUniqueHistory();
				}
				else
				{
					PrintMonstHistory(monster[pcursmonst].MType->mtype);
				}
			}
			else
			{
				strcpy(infostr, towner[pcursmonst]._tName);
			}
		}
		if ( pcursplr != -1 )
		{
			_LOBYTE(infoclr) = 3;
			strcpy(infostr, plr[pcursplr]._pName);
			ClearPanel();
			sprintf(tempstr, "Level : %i", plr[pcursplr]._pLevel);
			AddPanelString(tempstr, 1);
			sprintf(tempstr, "Hit Points %i of %i", plr[pcursplr]._pHitPoints >> 6, plr[pcursplr]._pMaxHP >> 6);
			AddPanelString(tempstr, 1);
		}
	}
	else
	{
		v2 = myplr;
		if ( plr[myplr].HoldItem._itype == ITYPE_GOLD )
		{
			v3 = plr[v2].HoldItem._ivalue;
			v4 = get_pieces_str(plr[v2].HoldItem._ivalue);
			sprintf(infostr, "%i gold %s", v3, v4);
		}
		else if ( plr[v2].HoldItem._iStatFlag )
		{
			if ( plr[v2].HoldItem._iIdentified )
				v5 = plr[v2].HoldItem._iIName;
			else
				v5 = plr[v2].HoldItem._iName;
			strcpy(infostr, v5);
			v6 = plr[myplr].HoldItem._iMagical;
			if ( v6 == 1 )
				_LOBYTE(infoclr) = 1;
			if ( v6 == 2 )
				_LOBYTE(infoclr) = 3;
		}
		else
		{
			ClearPanel();
			AddPanelString("Requirements not met", 1);
			pinfoflag = 1;
		}
	}
LABEL_33:
	if ( (infostr[0] || pnumlines) && !talkflag )
	{
		v7 = 0;
		v8 = 1;
		if ( infostr[0] )
		{
			control_print_info_str(0, infostr, 1, pnumlines);
			v7 = 1;
			v8 = 0;
		}
		v9 = 0;
		if ( pnumlines > 0 )
		{
			v10 = panelstr;
			do
			{
				control_print_info_str(v9 + v7, v10, pstrjust[v9], pnumlines - v8);
				++v9;
				v10 += 64;
			}
			while ( v9 < pnumlines );
		}
	}
}
// 4B8824: using guessed type int pinfoflag;
// 4B883C: using guessed type int infoclr;
// 4B8960: using guessed type int talkflag;
// 4B8B84: using guessed type int panelflag;
// 4B8C98: using guessed type int spselflag;
// 4B8CB8: using guessed type char pcursinvitem;
// 4B8CC0: using guessed type char pcursitem;
// 4B8CC1: using guessed type char pcursobj;
// 4B8CC2: using guessed type char pcursplr;
// 5BB1ED: using guessed type char leveltype;

void __fastcall control_print_info_str(int y, char *str, bool center, int lines)
{
	int v4; // edi
	char *v5; // ebx
	unsigned char v6; // cl
	signed int v7; // eax
	char *v8; // esi
	int v9; // eax
	unsigned char v10; // esi
	unsigned char v11; // al
	int width; // [esp+18h] [ebp+Ch]

	v4 = 0;
	v5 = str;
	width = lineoffset[y + 4 * lines + lines];
	if ( center == 1 )
	{
		v6 = *str;
		v7 = 0;
		v8 = str;
		if ( !*str )
			goto LABEL_14;
		do
		{
			++v8;
			v7 += fontkern[fontframe[fontidx[v6]]] + 2;
			v6 = *v8;
		}
		while ( *v8 );
		if ( v7 < 288 )
LABEL_14:
			v4 = (288 - v7) >> 1;
		width += v4;
	}
	while ( 1 )
	{
		v11 = *v5;
		if ( !*v5 )
			break;
		++v5;
		v9 = fontidx[v11];
		_LOBYTE(v9) = fontframe[v9];
		v10 = (unsigned char)v9;
		v4 += fontkern[(unsigned char)v9] + 2;
		if ( (_BYTE)v9 )
		{
			if ( v4 < 288 )
			{
				_LOBYTE(v9) = infoclr;
				CPrintString(width, v10, v9);
			}
		}
		width += fontkern[v10] + 2;
	}
}
// 4B883C: using guessed type int infoclr;

void __fastcall PrintGameStr(int x, int y, char *str, int color)
{
	char *v4; // edi
	int v5; // esi
	unsigned char i; // al
	unsigned char v7; // bl

	v4 = str;
	v5 = screen_y_times_768[y + 160] + x + 64;
	for ( i = *str; *v4; i = *v4 )
	{
		++v4;
		v7 = fontframe[fontidx[i]];
		if ( v7 )
			CPrintString(v5, v7, color);
		v5 += fontkern[v7] + 1;
	}
}

void __cdecl DrawChr()
{
	char v0; // al
	int v1; // ecx
	int v2; // ecx
	int v3; // eax
	int v4; // eax
	bool v5; // zf
	int v6; // eax
	int v7; // edi
	int v8; // edi
	char v9; // al
	char v10; // al
	char v11; // al
	int v12; // ecx
	int v13; // eax
	int v14; // ecx
	int v15; // eax
	int v16; // ecx
	int v17; // eax
	int v18; // ecx
	int v19; // eax
	int *v20; // edi
	int v21; // edi
	int v22; // edi
	int v23; // ecx
	int v24; // eax
	int v25; // ecx
	int v26; // ecx
	char a4[64]; // [esp+Ch] [ebp-50h]
	int v28; // [esp+4Ch] [ebp-10h]
	int v29; // [esp+50h] [ebp-Ch]
	int v30; // [esp+54h] [ebp-8h]
	char a5[4]; // [esp+58h] [ebp-4h]

	CelDecodeOnly(64, 511, pChrPanel, 1, 320);
	ADD_PlrStringXY(20, 32, 151, plr[myplr]._pName, 0);
	v0 = plr[myplr]._pClass;
	if ( v0 )
	{
		if ( v0 == 1 )
		{
			ADD_PlrStringXY(168, 32, 299, "Rogue", 0); /* should use ClassStrTbl ? */
		}
		else if ( v0 == 2 )
		{
			ADD_PlrStringXY(168, 32, 299, "Sorceror", 0);
		}
	}
	else
	{
		ADD_PlrStringXY(168, 32, 299, "Warrior", 0);
	}
	sprintf(a4, "%i", plr[myplr]._pLevel);
	ADD_PlrStringXY(66, 69, 109, a4, 0);
	sprintf(a4, "%li", plr[myplr]._pExperience);
	ADD_PlrStringXY(216, 69, 300, a4, 0);
	if ( plr[myplr]._pLevel == 50 )
	{
		strcpy(a4, "None");
		a5[0] = 3;
	}
	else
	{
		sprintf(a4, "%li", plr[myplr]._pNextExper);
		a5[0] = 0;
	}
	ADD_PlrStringXY(216, 97, 300, a4, a5[0]);
	sprintf(a4, "%i", plr[myplr]._pGold);
	ADD_PlrStringXY(216, 146, 300, a4, 0);
	a5[0] = 0;
	v29 = plr[myplr]._pIBonusAC;
	if ( v29 > 0 )
		a5[0] = 1;
	if ( v29 < 0 )
		a5[0] = 2;
	sprintf(a4, "%i", v29 + plr[myplr]._pIAC + plr[myplr]._pDexterity / 5);
	ADD_PlrStringXY(258, 183, 301, a4, a5[0]);
	a5[0] = 0;
	v1 = plr[myplr]._pIBonusToHit;
	if ( v1 > 0 )
		a5[0] = 1;
	if ( v1 < 0 )
		a5[0] = 2;
	sprintf(a4, "%i%%", (plr[myplr]._pDexterity >> 1) + v1 + 50);
	ADD_PlrStringXY(258, 211, 301, a4, a5[0]);
	a5[0] = 0;
	v2 = myplr;
	v3 = plr[myplr]._pIBonusDam;
	if ( v3 > 0 )
		a5[0] = 1;
	if ( v3 < 0 )
		a5[0] = 2;
	v30 = plr[v2]._pIMinDam;
	v30 += plr[v2]._pIBonusDamMod + v30 * v3 / 100;
	v4 = plr[v2]._pDamageMod;
	v5 = plr[v2].InvBody[4]._itype == ITYPE_BOW;
	v29 = plr[v2]._pDamageMod;
	if ( v5 && _LOBYTE(plr[v2]._pClass) != 1 )
		v4 >>= 1;
	v30 += v4;
	v6 = plr[v2]._pIBonusDam;
	v28 = plr[v2]._pIMaxDam;
	v7 = plr[v2]._pIBonusDamMod + v28 * v6 / 100 + v28;
	if ( plr[v2].InvBody[4]._itype != ITYPE_BOW || _LOBYTE(plr[v2]._pClass) == 1 )
		v8 = v29 + v7;
	else
		v8 = (v29 >> 1) + v7;
	sprintf(a4, "%i-%i", v30, v8);
	if ( v30 >= 100 || v8 >= 100 )
		MY_PlrStringXY(254, 239, 305, a4, a5[0], -1);
	else
		MY_PlrStringXY(258, 239, 301, a4, a5[0], 0);
	v9 = plr[myplr]._pMagResist;
	a5[0] = v9 != 0;
	if ( v9 >= 75 )
	{
		a5[0] = 3;
		sprintf(a4, "MAX");
	}
	else
	{
		sprintf(a4, "%i%%", v9);
	}
	ADD_PlrStringXY(257, 276, 300, a4, a5[0]);
	v10 = plr[myplr]._pFireResist;
	a5[0] = v10 != 0;
	if ( v10 >= 75 )
	{
		a5[0] = 3;
		sprintf(a4, "MAX");
	}
	else
	{
		sprintf(a4, "%i%%", v10);
	}
	ADD_PlrStringXY(257, 304, 300, a4, a5[0]);
	v11 = plr[myplr]._pLghtResist;
	a5[0] = v11 != 0;
	if ( v11 >= 75 )
	{
		a5[0] = 3;
		sprintf(a4, "MAX");
	}
	else
	{
		sprintf(a4, "%i%%", v11);
	}
	ADD_PlrStringXY(257, 332, 300, a4, a5[0]);
	a5[0] = 0;
	sprintf(a4, "%i", plr[myplr]._pBaseStr);
	if ( MaxStats[SLOBYTE(plr[myplr]._pClass)][0] == plr[myplr]._pBaseStr )
		a5[0] = 3;
	ADD_PlrStringXY(95, 155, 126, a4, a5[0]);
	a5[0] = 0;
	sprintf(a4, "%i", plr[myplr]._pBaseMag);
	if ( MaxStats[SLOBYTE(plr[myplr]._pClass)][1] == plr[myplr]._pBaseMag )
		a5[0] = 3;
	ADD_PlrStringXY(95, 183, 126, a4, a5[0]);
	a5[0] = 0;
	sprintf(a4, "%i", plr[myplr]._pBaseDex);
	if ( MaxStats[SLOBYTE(plr[myplr]._pClass)][2] == plr[myplr]._pBaseDex )
		a5[0] = 3;
	ADD_PlrStringXY(95, 211, 126, a4, a5[0]);
	a5[0] = 0;
	sprintf(a4, "%i", plr[myplr]._pBaseVit);
	if ( MaxStats[SLOBYTE(plr[myplr]._pClass)][3] == plr[myplr]._pBaseVit )
		a5[0] = 3;
	ADD_PlrStringXY(95, 239, 126, a4, a5[0]);
	a5[0] = 0;
	v12 = plr[myplr]._pStrength;
	v13 = plr[myplr]._pBaseStr;
	if ( v12 > v13 )
		a5[0] = 1;
	if ( v12 < v13 )
		a5[0] = 2;
	sprintf(a4, "%i", v12);
	ADD_PlrStringXY(143, 155, 173, a4, a5[0]);
	a5[0] = 0;
	v14 = plr[myplr]._pMagic;
	v15 = plr[myplr]._pBaseMag;
	if ( v14 > v15 )
		a5[0] = 1;
	if ( v14 < v15 )
		a5[0] = 2;
	sprintf(a4, "%i", v14);
	ADD_PlrStringXY(143, 183, 173, a4, a5[0]);
	a5[0] = 0;
	v16 = plr[myplr]._pDexterity;
	v17 = plr[myplr]._pBaseDex;
	if ( v16 > v17 )
		a5[0] = 1;
	if ( v16 < v17 )
		a5[0] = 2;
	sprintf(a4, "%i", v16);
	ADD_PlrStringXY(143, 211, 173, a4, a5[0]);
	a5[0] = 0;
	v18 = plr[myplr]._pVitality;
	v19 = plr[myplr]._pBaseVit;
	if ( v18 > v19 )
		a5[0] = 1;
	if ( v18 < v19 )
		a5[0] = 2;
	sprintf(a4, "%i", v18);
	ADD_PlrStringXY(143, 239, 173, a4, a5[0]);
	v20 = &plr[myplr]._pStatPts;
	if ( *v20 > 0 )
	{
		v20 = &plr[myplr]._pStatPts;
		if ( CalcStatDiff(myplr) < *v20 )
		{
			v20 = &plr[myplr]._pStatPts;
			*v20 = CalcStatDiff(myplr);
		}
	}
	v21 = *v20;
	if ( v21 > 0 )
	{
		sprintf(a4, "%i", v21);
		ADD_PlrStringXY(95, 266, 126, a4, 2);
		v22 = SLOBYTE(plr[myplr]._pClass);
		if ( plr[myplr]._pBaseStr < MaxStats[v22][0] )
			CelDecodeOnly(201, 319, pChrButtons, chrbtn[0] + 2, 41);
		if ( plr[myplr]._pBaseMag < MaxStats[v22][1] )
			CelDecodeOnly(201, 347, pChrButtons, chrbtn[1] + 4, 41);
		if ( plr[myplr]._pBaseDex < MaxStats[v22][2] )
			CelDecodeOnly(201, 376, pChrButtons, chrbtn[2] + 6, 41);
		if ( plr[myplr]._pBaseVit < MaxStats[v22][3] )
			CelDecodeOnly(201, 404, pChrButtons, chrbtn[3] + 8, 41);
	}
	v23 = plr[myplr]._pMaxHP;
	a5[0] = v23 > plr[myplr]._pMaxHPBase;
	sprintf(a4, "%i", v23 >> 6);
	ADD_PlrStringXY(95, 304, 126, a4, a5[0]);
	v24 = plr[myplr]._pHitPoints;
	if ( v24 != plr[myplr]._pMaxHP )
		a5[0] = 2;
	sprintf(a4, "%i", v24 >> 6);
	ADD_PlrStringXY(143, 304, 174, a4, a5[0]);
	v25 = plr[myplr]._pMaxMana;
	a5[0] = v25 > plr[myplr]._pMaxManaBase;
	sprintf(a4, "%i", v25 >> 6);
	ADD_PlrStringXY(95, 332, 126, a4, a5[0]);
	v26 = plr[myplr]._pMana;
	if ( v26 != plr[myplr]._pMaxMana )
		a5[0] = 2;
	sprintf(a4, "%i", v26 >> 6);
	ADD_PlrStringXY(143, 332, 174, a4, a5[0]);
}

void __fastcall ADD_PlrStringXY(int x, int y, int width, char *pszStr, char col)
{
	int v5; // eax
	char *v6; // edx
	unsigned char v7; // al
	int v8; // esi
	int v9; // edi
	int v10; // ecx
	unsigned char v11; // bl
	unsigned char v12; // al
	int v13; // ebx
	int widtha; // [esp+Ch] [ebp-4h]
	int widthb; // [esp+Ch] [ebp-4h]

	v5 = screen_y_times_768[y + 160];
	v6 = pszStr;
	widtha = v5 + x + 64;
	v7 = *pszStr;
	v8 = width - x + 1;
	v9 = 0;
	v10 = 0;
	if ( *pszStr )
	{
		v11 = *pszStr;
		do
		{
			++v6;
			v10 += fontkern[fontframe[fontidx[v11]]] + 1;
			v11 = *v6;
		}
		while ( *v6 );
	}
	if ( v10 < v8 )
		v9 = (v8 - v10) >> 1;
	widthb = v9 + widtha;
	while ( v7 )
	{
		++pszStr;
		v12 = fontframe[fontidx[v7]];
		v13 = v12;
		v9 += fontkern[v12] + 1;
		if ( v12 )
		{
			if ( v9 < v8 )
				CPrintString(widthb, v12, col);
		}
		widthb += fontkern[v13] + 1;
		v7 = *pszStr;
	}
}

void __fastcall MY_PlrStringXY(int x, int y, int width, char *pszStr, char col, int base)
{
	char *v6; // ebx
	unsigned char v7; // al
	int v8; // edx
	int v9; // esi
	char *v10; // edi
	unsigned char v11; // cl
	unsigned char v12; // al
	int v13; // edi
	int widtha; // [esp+Ch] [ebp-4h]
	int widthb; // [esp+Ch] [ebp-4h]
	int v16; // [esp+18h] [ebp+8h]

	v6 = pszStr;
	widtha = screen_y_times_768[y + 160] + x + 64;
	v7 = *pszStr;
	v8 = 0;
	v9 = width - x + 1;
	v16 = 0;
	v10 = pszStr;
	if ( *pszStr )
	{
		v11 = *pszStr;
		do
		{
			++v10;
			v8 += base + fontkern[fontframe[fontidx[v11]]];
			v11 = *v10;
		}
		while ( *v10 );
	}
	if ( v8 < v9 )
		v16 = (v9 - v8) >> 1;
	widthb = v16 + widtha;
	while ( v7 )
	{
		++v6;
		v12 = fontframe[fontidx[v7]];
		v13 = v12;
		v16 += base + fontkern[v12];
		if ( v12 )
		{
			if ( v16 < v9 )
				CPrintString(widthb, v12, col);
		}
		widthb += base + fontkern[v13];
		v7 = *v6;
	}
}

void __cdecl CheckLvlBtn()
{
	if ( !lvlbtndown && MouseX >= 40 && MouseX <= 81 && MouseY >= 313 && MouseY <= 335 )
		lvlbtndown = 1;
}
// 4B851C: using guessed type int lvlbtndown;

void __cdecl ReleaseLvlBtn()
{
	if ( MouseX >= 40 && MouseX <= 81 && MouseY >= 313 && MouseY <= 335 )
		chrflag = 1;
	lvlbtndown = 0;
}
// 4B851C: using guessed type int lvlbtndown;

void __cdecl DrawLevelUpIcon()
{
	int v0; // esi

	if ( !stextflag )
	{
		v0 = (lvlbtndown != 0) + 2;
		ADD_PlrStringXY(0, 303, 120, "Level Up", 0);
		CelDecodeOnly(104, 495, pChrButtons, v0, 41);
	}
}
// 4B851C: using guessed type int lvlbtndown;
// 6AA705: using guessed type char stextflag;

void __cdecl CheckChrBtns()
{
	int v0; // esi
	int v1; // ecx
	int v2; // ebx
	int v3; // edi
	int v4; // edx
	bool v5; // sf
	unsigned char v6; // of
	int v7; // edx
	int v8; // edx
	int v9; // edx
	int v10; // eax
	int v11; // edx
	int v12; // edx

	v0 = 0;
	if ( !chrbtnactive )
	{
		v1 = myplr;
		if ( plr[myplr]._pStatPts )
		{
			v2 = MouseX;
			v3 = SLOBYTE(plr[v1]._pClass);
			while ( 1 )
			{
				if ( !v0 )
				{
					v9 = plr[v1]._pBaseStr;
					v6 = __OFSUB__(v9, MaxStats[v3][0]);
					v5 = v9 - MaxStats[v3][0] < 0;
					goto LABEL_12;
				}
				if ( v0 == 1 )
				{
					v8 = plr[v1]._pBaseMag;
					v6 = __OFSUB__(v8, MaxStats[v3][1]);
					v5 = v8 - MaxStats[v3][1] < 0;
					goto LABEL_12;
				}
				if ( v0 == 2 )
					break;
				if ( v0 == 3 )
				{
					v4 = plr[v1]._pBaseVit;
					v6 = __OFSUB__(v4, MaxStats[v3][3]);
					v5 = v4 - MaxStats[v3][3] < 0;
LABEL_12:
					if ( v5 ^ v6 )
					{
						v10 = v0;
						v11 = attribute_inc_rects[v0][0];
						if ( v2 >= v11 && v2 <= v11 + attribute_inc_rects[v10][2] )
						{
							v12 = attribute_inc_rects[v10][1];
							if ( MouseY >= v12 && MouseY <= v12 + attribute_inc_rects[v10][3] )
							{
								chrbtn[v0] = 1;
								chrbtnactive = 1;
							}
						}
					}
				}
				if ( ++v0 >= 4 )
					return;
			}
			v7 = plr[v1]._pBaseDex;
			v6 = __OFSUB__(v7, MaxStats[v3][2]);
			v5 = v7 - MaxStats[v3][2] < 0;
			goto LABEL_12;
		}
	}
}
// 4B87A8: using guessed type int chrbtnactive;

void __cdecl ReleaseChrBtns()
{
	signed int v0; // esi
	int *v1; // eax
	signed int v2; // eax
	int v3; // ecx
	int v4; // ecx
	unsigned char v5; // dl

	chrbtnactive = 0;
	v0 = 0;
	do
	{
		v1 = &chrbtn[v0];
		if ( *v1 )
		{
			*v1 = 0;
			v2 = v0;
			v3 = attribute_inc_rects[v0][0];
			if ( MouseX >= v3 && MouseX <= v3 + attribute_inc_rects[v2][2] )
			{
				v4 = attribute_inc_rects[v2][1];
				if ( MouseY >= v4 && MouseY <= v4 + attribute_inc_rects[v2][3] )
				{
					if ( v0 )
					{
						switch ( v0 )
						{
							case ATTRIB_MAG:
								v5 = CMD_ADDMAG;
								break;
							case ATTRIB_DEX:
								v5 = CMD_ADDDEX;
								break;
							case ATTRIB_VIT:
								v5 = CMD_ADDVIT;
								break;
							default:
								goto LABEL_16;
						}
					}
					else
					{
						v5 = CMD_ADDSTR;
					}
					NetSendCmdParam1(1u, v5, 1u);
					--plr[myplr]._pStatPts;
				}
			}
		}
LABEL_16:
		++v0;
	}
	while ( v0 < 4 );
}
// 4B87A8: using guessed type int chrbtnactive;

void __cdecl DrawDurIcon()
{
	int v0; // edx
	PlayerStruct *v1; // esi
	int v2; // eax
	int v3; // eax
	int v4; // eax

	if ( !chrflag && !questlog || !invflag && !sbookflag )
	{
		v0 = 656;
		if ( invflag || sbookflag )
			v0 = 336;
		v1 = &plr[myplr];
		v2 = DrawDurIcon4Item(v1->InvBody, v0, 4);
		v3 = DrawDurIcon4Item(&v1->InvBody[6], v2, 3);
		v4 = DrawDurIcon4Item(&v1->InvBody[4], v3, 0);
		DrawDurIcon4Item(&v1->InvBody[5], v4, 0);
	}
}
// 4B8968: using guessed type int sbookflag;
// 69BD04: using guessed type int questlog;

int __fastcall DrawDurIcon4Item(ItemStruct *pItem, int x, int c)
{
	int v3; // eax
	int v4; // edi
	int v5; // esi
	signed int v7; // edx
	int v8; // eax
	int v9; // eax
	int v10; // eax
	int v11; // eax
	signed int v12; // [esp-4h] [ebp-Ch]

	v3 = pItem->_itype;
	v4 = x;
	if ( v3 == -1 )
		return x;
	v5 = pItem->_iDurability;
	if ( v5 > 5 )
		return x;
	v7 = c;
	if ( !c )
	{
		if ( pItem->_iClass != 1 )
		{
			v7 = 1;
			goto LABEL_18;
		}
		v8 = v3 - 1;
		if ( !v8 )
		{
			v12 = 2;
			goto LABEL_15;
		}
		v9 = v8 - 1;
		if ( !v9 )
		{
			v12 = 6;
			goto LABEL_15;
		}
		v10 = v9 - 1;
		if ( !v10 )
		{
			v12 = 7;
			goto LABEL_15;
		}
		v11 = v10 - 1;
		if ( !v11 )
		{
			v12 = 5;
			goto LABEL_15;
		}
		if ( v11 == 6 )
		{
			v12 = 8;
LABEL_15:
			v7 = v12;
			goto LABEL_18;
		}
	}
LABEL_18:
	if ( v5 > 2 )
		v7 += 8;
	CelDecodeOnly(v4, 495, pDurIcons, v7, 32);
	return v4 - 40;
}

void __cdecl RedBack()
{
	int v0; // eax
	char *v1; // edi
	signed int v3; // edx
	signed int v4; // ecx
	char *v7; // edi
	signed int v9; // edx
	signed int v10; // ecx
	int v12; // [esp+8h] [ebp-4h]
	int _EAX;
	char *_EBX;

	v0 = -(light4flag != 0);
	_LOWORD(v0) = v0 & 0xF400;
	v12 = v0 + 0x1200;
	if ( leveltype == DTYPE_HELL )
	{
		v7 = gpBuffer->row[0].pixels;
		_EBX = &pLightTbl[v12];
		v9 = 352;
		do
		{
			v10 = 640;
			do
			{
				_EAX = *v7;
				if ( (unsigned char)*v7 >= 0x20u )
					ASM_XLAT(_EAX,_EBX);
				*v7++ = _EAX;
				--v10;
			}
			while ( v10 );
			v7 += 128;
			--v9;
		}
		while ( v9 );
	}
	else
	{
		v1 = gpBuffer->row[0].pixels;
		_EBX = &pLightTbl[v12];
		v3 = 352;
		do
		{
			v4 = 640;
			do
			{
				_EAX = *v1;
				ASM_XLAT(_EAX,_EBX);
				*v1++ = _EAX;
				--v4;
			}
			while ( v4 );
			v1 += 128;
			--v3;
		}
		while ( v3 );
	}
}
// 525728: using guessed type int light4flag;
// 5BB1ED: using guessed type char leveltype;

int __fastcall GetSBookTrans(int ii, unsigned char townok)
{
	int v2; // edi
	int v3; // esi
	int result; // eax
	char v6; // [esp+13h] [ebp-5h]
	int v7; // [esp+14h] [ebp-4h]

	v2 = ii;
	v7 = townok;
	v6 = 1;
	v3 = myplr;
	if ( ((unsigned __int64)((__int64)1 << ((unsigned char)ii - 1)) >> 32) & plr[v3]._pISpells[1] | (unsigned int)((__int64)1 << ((unsigned char)ii - 1)) & plr[v3]._pISpells[0] )
		v6 = 3;
	result = plr[v3]._pAblSpells[1] & (1 << (ii - 1) >> 31) | plr[v3]._pAblSpells[0] & (1 << (ii - 1));
	if ( result )
		v6 = 0;
	if ( v6 == 1 )
	{
		if ( !CheckSpell(myplr, ii, 1, 1) )
			v6 = 4;
		result = 21720 * myplr;
		if ( (char)(plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[v2]) <= 0 )
			v6 = 4;
	}
	if ( v7 && !currlevel && v6 != 4 && !*(_DWORD *)&spelldata[v2].sTownSpell )
		v6 = 4;
	_LOBYTE(result) = v6;
	return result;
}

void __cdecl DrawSpellBook()
{
	__int64 v0; // edi
	__int64 v1; // ebp
	int v2; // esi
	char v3; // al
	int v4; // eax
	int v5; // ebx
	int v6; // ecx
	char v7; // [esp+Bh] [ebp-1Dh]
	int v8; // [esp+Ch] [ebp-1Ch]
	signed int v9; // [esp+10h] [ebp-18h]
	int sel; // [esp+14h] [ebp-14h]
	int v11; // [esp+18h] [ebp-10h]
	int v12; // [esp+1Ch] [ebp-Ch]

	CelDecodeOnly(384, 511, pSpellBkCel, 1, 320);
	CelDecodeOnly(76 * sbooktab + 391, 508, pSBkBtnCel, sbooktab + 1, 76);
	v9 = 1;
	v8 = 214;
	v0 = plr[myplr]._pISpells64 | plr[myplr]._pMemSpells64 | plr[myplr]._pAblSpells64;
	do
	{
		v2 = SpellPages[0][v9 + 7 * sbooktab - 1]; // *(&attribute_inc_rects[3].h + v9 + 7 * sbooktab); /* check */
		v1 = (__int64)1 << (v2 - 1);
		if ( v2 != -1 && (v1 & v0) )
		{
			v7 = GetSBookTrans(v2, 1u);
			SetSpellTrans(v7);
			DrawSpellCel(395, v8 + 1, (char *)pSBkIconCels, (char)SpellITbl[v2], 37);
			if ( v2 == plr[myplr]._pRSpell && v7 == _LOBYTE(plr[myplr]._pRSplType) )
			{
				SetSpellTrans(0);
				DrawSpellCel(395, v8 + 1, (char *)pSBkIconCels, 43, 37);
			}
			PrintSBookStr(10, v8 - 22, 0, spelldata[v2].sNameText, 0);
			v3 = GetSBookTrans(v2, 0);
			if ( v3 )
			{
				if ( v3 == 3 )
				{
					sprintf(tempstr, "Staff (%i charges)", plr[myplr].InvBody[4]._iCharges);
				}
				else
				{
					v4 = GetManaAmount(myplr, v2);
					v5 = v4 >> 6;
					v12 = v4 >> 6;
					GetDamageAmt(v2, &sel, &v11);
					if ( sel == -1 )
						sprintf(tempstr, "Mana: %i   Dam: n/a", v5);
					else
						sprintf(tempstr, "Mana: %i  Dam: %i - %i", v5, sel, v11);
					if ( v2 == SPL_BONESPIRIT )
						sprintf(tempstr, "Mana: %i  Dam: 1/3 tgt hp", v12);
					PrintSBookStr(10, v8, 0, tempstr, 0);
					v6 = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[v2];
					if ( v6 < 0 )
						v6 = 0;
					if ( v6 )
						sprintf(tempstr, "Spell Level %i", v6);
					else
						sprintf(tempstr, "Spell Level 0 - Unusable");
				}
			}
			else
			{
				strcpy(tempstr, "Skill");
			}
			PrintSBookStr(10, v8 - 11, 0, tempstr, 0);
		}
		v8 += 43;
		++v9;
	}
	while ( v9 < 8 );
}
// 4B8950: using guessed type int sbooktab;

void __fastcall PrintSBookStr(int x, int y, bool cjustflag, char *pszStr, int bright)
{
	char *v5; // ebx
	signed int v6; // eax
	int v7; // edi
	unsigned char v8; // cl
	char *v9; // esi
	unsigned char v10; // al
	int v11; // esi
	unsigned char v12; // al
	int width; // [esp+Ch] [ebp-4h]

	v5 = pszStr;
	width = screen_y_times_768[y] + x + 440;
	v6 = 0;
	v7 = 0;
	if ( cjustflag )
	{
		v8 = *pszStr;
		v9 = pszStr;
		if ( !*pszStr )
			goto LABEL_14;
		do
		{
			++v9;
			v6 += fontkern[fontframe[fontidx[v8]]] + 1;
			v8 = *v9;
		}
		while ( *v9 );
		if ( v6 < 222 )
LABEL_14:
			v7 = (222 - v6) >> 1;
		width += v7;
	}
	while ( 1 )
	{
		v12 = *v5;
		if ( !*v5 )
			break;
		++v5;
		v10 = fontframe[fontidx[v12]];
		v11 = v10;
		v7 += fontkern[v10] + 1;
		if ( v10 )
		{
			if ( v7 <= 222 )
				CPrintString(width, v10, bright);
		}
		width += fontkern[v11] + 1;
	}
}

void __cdecl CheckSBook()
{
	signed int v0; // ecx
	signed int v1; // esi
	int v2; // eax
	int v3; // esi
	signed __int64 v4; // rax
	char v5; // cl
	__int64 v6; // [esp+8h] [ebp-10h]
	int v7; // [esp+10h] [ebp-8h]

	v0 = MouseY;
	v1 = MouseX;
	if ( MouseX >= 331 && MouseX < 368 && MouseY >= 18 && MouseY < 314 )
	{
		v2 = SpellPages[0][7 * sbooktab + (MouseY - 18) / 43];
		v7 = SpellPages[0][7 * sbooktab + (MouseY - 18) / 43];
		if ( v2 != -1 )
		{
			v3 = myplr;
			LODWORD(v6) = plr[myplr]._pAblSpells[0];
			HIDWORD(v6) = plr[myplr]._pAblSpells[1];
			v4 = (__int64)1 << ((unsigned char)v2 - 1);
			if ( HIDWORD(v4) & (HIDWORD(v6) | plr[myplr]._pISpells[1] | plr[myplr]._pMemSpells[1]) | (unsigned int)v4 & ((unsigned int)v6 | plr[myplr]._pISpells[0] | plr[myplr]._pMemSpells[0]) )
			{
				v5 = 3;
				if ( !(plr[v3]._pISpells[1] & HIDWORD(v4) | plr[v3]._pISpells[0] & (unsigned int)v4) )
					v5 = 1;
				if ( v6 & v4 )
					v5 = 0;
				drawpanflag = 255;
				plr[v3]._pRSpell = v7;
				_LOBYTE(plr[v3]._pRSplType) = v5;
			}
			v1 = MouseX;
			v0 = MouseY;
		}
	}
	if ( v1 >= 327 && v1 < 633 && v0 >= 320 && v0 < 349 ) /// BUGFIX: change `< 633` to `< 631`
		sbooktab = (v1 - 327) / 76;
}
// 4B8950: using guessed type int sbooktab;
// 52571C: using guessed type int drawpanflag;

char *__fastcall get_pieces_str(int nGold)
{
	char *result; // eax

	result = "piece";
	if ( nGold != 1 )
		result = "pieces";
	return result;
}

void __fastcall DrawGoldSplit(int amount)
{
	int v1; // ebp
	char *v2; // eax
	char v3; // cl
	signed int i; // eax
	int screen_x; // [esp+10h] [ebp-4h]
	int screen_xa; // [esp+10h] [ebp-4h]

	screen_x = 0;
	v1 = amount;
	CelDecodeOnly(415, 338, pGBoxBuff, 1, 261);
	sprintf(tempstr, "You have %u gold", initialDropGoldValue);
	ADD_PlrStringXY(366, 87, 600, tempstr, 3);
	v2 = get_pieces_str(initialDropGoldValue);
	sprintf(tempstr, "%s.  How many do", v2);
	ADD_PlrStringXY(366, 103, 600, tempstr, 3);
	ADD_PlrStringXY(366, 121, 600, "you want to remove?", 3);
	if ( v1 <= 0 )
	{
		screen_xa = 450;
	}
	else
	{
		sprintf(tempstr, "%u", v1);
		PrintGameStr(388, 140, tempstr, 0);
		v3 = tempstr[0];
		for ( i = 0; i < v3; v3 = tempstr[i] )
		{
			++i;
			screen_x += fontkern[fontframe[fontidx[(unsigned char)v3]]] + 1;
		}
		screen_xa = screen_x + 452;
	}
	CelDecodeOnly(screen_xa, 300, pCelBuff, frame_4B8800, 12);
	frame_4B8800 = (frame_4B8800 & 7) + 1;
}

void __fastcall control_drop_gold(int vkey)
{
	char v1; // bl
	int v2; // eax
	int v3; // eax
	size_t v4; // esi
	char v6[6]; // [esp+8h] [ebp-8h]

	v1 = vkey;
	if ( (signed int)(plr[myplr]._pHitPoints & 0xFFFFFFC0) <= 0 )
	{
		dropGoldFlag = 0;
		dropGoldValue = 0;
		return;
	}
	memset(v6, 0, 6u);
	_itoa(dropGoldValue, v6, 10);
	if ( v1 != VK_RETURN )
	{
		if ( v1 == VK_ESCAPE )
		{
			dropGoldFlag = 0;
			dropGoldValue = 0;
			return;
		}
		if ( v1 == VK_BACK )
		{
			v6[strlen(v6) - 1] = '\0';
			v2 = atoi(v6);
		}
		else
		{
			v3 = v1 - '0';
			if ( v3 < 0 || v3 > 9 )
				return;
			if ( dropGoldValue || atoi(v6) <= initialDropGoldValue )
			{
				v6[strlen(v6)] = v1;
				if ( atoi(v6) > initialDropGoldValue )
					return;
				v4 = strlen(v6);
				if ( v4 > strlen(v6) )
					return;
			}
			else
			{
				v6[0] = v1;
			}
			v2 = atoi(v6);
		}
		dropGoldValue = v2;
		return;
	}
	if ( dropGoldValue > 0 )
		control_remove_gold(myplr, initialDropGoldIndex);
	dropGoldFlag = 0;
}
// 4B84DC: using guessed type int dropGoldFlag;
// 406C40: using guessed type char var_8[8];

void __fastcall control_remove_gold(int pnum, int gold_index)
{
	int v2; // edi
	int v3; // esi
	int v4; // edx
	_DWORD *v5; // eax
	int v6; // edx
	_DWORD *v7; // eax
	int v8; // eax

	v2 = pnum;
	v3 = pnum;
	if ( gold_index > 46 )
	{
		v6 = gold_index - 47;
		v7 = (unsigned int *)((char *)&plr[0].SpdList[v6]._ivalue + v3 * 21720);
		*v7 -= dropGoldValue;
		if ( *v7 <= 0 )
			RemoveSpdBarItem(pnum, v6);
		else
			SetSpdbarGoldCurs(pnum, v6);
	}
	else
	{
		v4 = gold_index - 7;
		v5 = (unsigned int *)((char *)&plr[0].InvList[v4]._ivalue + v3 * 21720);
		*v5 -= dropGoldValue;
		if ( *v5 <= 0 )
			RemoveInvItem(pnum, v4);
		else
			SetGoldCurs(pnum, v4);
	}
	SetPlrHandItem(&plr[v3].HoldItem, IDI_GOLD);
	GetGoldSeed(v2, &plr[v3].HoldItem);
	plr[v3].HoldItem._ivalue = dropGoldValue;
	plr[v3].HoldItem._iStatFlag = 1;
	control_set_gold_curs(v2);
	v8 = CalculateGold(v2);
	dropGoldValue = 0;
	plr[v3]._pGold = v8;
}

void __fastcall control_set_gold_curs(int pnum)
{
	int v1; // ecx
	int v2; // eax
	int *v3; // eax
	bool v4; // zf
	bool v5; // sf
	unsigned char v6; // of

	v1 = pnum;
	v2 = plr[v1].HoldItem._ivalue;
	if ( v2 < 2500 )
	{
		v6 = __OFSUB__(v2, 1000);
		v4 = v2 == 1000;
		v5 = v2 - 1000 < 0;
		v3 = &plr[v1].HoldItem._iCurs;
		if ( (unsigned char)(v5 ^ v6) | v4 )
			*v3 = 4;
		else
			*v3 = 5;
	}
	else
	{
		v3 = &plr[v1].HoldItem._iCurs;
		plr[v1].HoldItem._iCurs = 6;
	}
	SetCursor(*v3 + 12);
}

void __cdecl DrawTalkPan()
{
	int v0; // esi
	signed int v1; // edi
	signed int v2; // esi
	char *v3; // eax
	int v4; // esi
	int v5; // esi
	int v6; // ebx
	int v7; // eax
	int a4; // [esp+4h] [ebp-Ch]
	char *a1; // [esp+8h] [ebp-8h]
	int v10; // [esp+Ch] [ebp-4h]

	v0 = 0;
	if ( talkflag )
	{
		DrawPanelBox(175, sgbPlrTalkTbl + 20, 0x126u, 5u, 239, 516);
		v1 = 293;
		do
		{
			DrawPanelBox((v0 >> 1) + 175, sgbPlrTalkTbl + v0 + 25, v1, 1u, (v0 >> 1) + 239, v0 + 521);
			++v0;
			--v1;
		}
		while ( v1 > 283 );
		DrawPanelBox(185, sgbPlrTalkTbl + 35, 0x112u, 0x1Eu, 249, 531);
		DrawPanelBox(180, sgbPlrTalkTbl + 65, 0x11Cu, 5u, 244, 561);
		v2 = 0;
		do
		{
			DrawPanelBox(180, sgbPlrTalkTbl + v2 + 70, v2 + 284, 1u, 244, v2 + 566);
			++v2;
		}
		while ( v2 < 10 );
		DrawPanelBox(170, sgbPlrTalkTbl + 80, 0x136u, 0x37u, 234, 576);
		v3 = sgszTalkMsg;
		v4 = 0;
		do
		{
			v3 = control_print_talk_msg(v3, 0, v4, &a4, 0);
			if ( !v3 )
				goto LABEL_10;
			v4 += 13;
		}
		while ( v4 < 39 );
		*v3 = 0;
LABEL_10:
		CelDecDatOnly((char *)gpBuffer + a4, (char *)pCelBuff, frame, 12);
		v5 = 0;
		a1 = plr[0]._pName;
		v10 = 0;
		frame = (frame & 7) + 1;
		while ( v10 == myplr )
		{
LABEL_21:
			a1 += 21720;
			++v10;
			if ( (signed int)a1 >= (signed int)&plr[4]._pName )
				return;
		}
		if ( tempstr[v10 + 256] )
		{
			v6 = 3;
			if ( !talkbtndown[v5] )
			{
LABEL_18:
				if ( *(a1 - 291) )
					control_print_talk_msg(a1, 46, 18 * v5 + 60, &a4, v6);
				++v5;
				goto LABEL_21;
			}
			v7 = (v5 != 0) + 3;
		}
		else
		{
			v7 = (v5 != 0) + 1;
			v6 = 2;
			if ( talkbtndown[v5] )
				v7 = (v5 != 0) + 5;
		}
		CelDecodeOnly(236, 18 * v5 + 596, pTalkBtns, v7, 61);
		goto LABEL_18;
	}
}
// 4B8840: using guessed type int sgbPlrTalkTbl;
// 4B8960: using guessed type int talkflag;

char *__fastcall control_print_talk_msg(char *msg, int x, int y, int *a4, int just)
{
	int v5; // edx
	char *v6; // ebx
	unsigned char v7; // al
	int v8; // ecx
	unsigned char v10; // dl
	int v11; // edi
	int a3; // [esp+14h] [ebp+8h]

	v5 = x + 264;
	v6 = msg;
	*a4 = v5 + screen_y_times_768[y + 534];
	v7 = *msg;
	v8 = v5;
	if ( !v7 )
		return 0;
	while ( 1 )
	{
		v10 = fontframe[fontidx[v7]];
		v11 = v10;
		a3 = v8 + fontkern[v10] + 1;
		if ( a3 > 514 )
			break;
		++v6;
		if ( v10 )
			CPrintString(*a4, v10, just);
		*a4 += fontkern[v11] + 1;
		v7 = *v6;
		if ( !*v6 )
			return 0;
		v8 = a3;
	}
	return v6;
}

int __cdecl control_check_talk_btn()
{
	int v0; // ecx
	int result; // eax

	if ( !talkflag )
		return 0;
	if ( MouseX < 172 )
		return 0;
	v0 = MouseY;
	if ( MouseY < 421 || MouseX > 233 )
		return 0;
	result = 0;
	if ( MouseY <= 475 )
	{
		talkbtndown[0] = 0;
		talkbtndown[1] = 0;
		talkbtndown[2] = 0;
		talkbtndown[(v0 - 421) / 18] = 1;
		result = 1;
	}
	return result;
}
// 4B8960: using guessed type int talkflag;

void __cdecl control_release_talk_btn()
{
	signed int v0; // ecx
	int v1; // eax
	signed int v2; // ecx

	if ( talkflag )
	{
		v0 = MouseX;
		talkbtndown[0] = 0;
		talkbtndown[1] = 0;
		talkbtndown[2] = 0;
		if ( v0 >= 172 && MouseY >= 421 && v0 <= 233 && MouseY <= 475 )
		{
			v1 = (MouseY - 421) / 18;
			v2 = 0;
			do
			{
				if ( v1 == -1 )
					break;
				if ( v2 != myplr )
					--v1;
				++v2;
			}
			while ( v2 < 4 );
			if ( v2 <= 4 )
				tempstr[v2 + 255] = tempstr[v2 + 255] == 0;
		}
	}
}
// 4B8960: using guessed type int talkflag;

void __cdecl control_reset_talk_msg()
{
	int v0; // edi
	signed int v1; // ecx

	v0 = 0;
	v1 = 0;
	do
	{
		if ( tempstr[v1 + 256] )
			v0 |= 1 << v1;
		++v1;
	}
	while ( v1 < 4 );
	if ( !msgcmd_add_server_cmd_W(sgszTalkMsg) )
		NetSendCmdString(v0, sgszTalkMsg);
}

void __cdecl control_type_message()
{
	if ( gbMaxPlayers != 1 )
	{
		sgszTalkMsg[0] = 0;
		talkflag = 1;
		frame = 1;
		talkbtndown[0] = 0;
		talkbtndown[1] = 0;
		talkbtndown[2] = 0;
		sgbPlrTalkTbl = 144;
		drawpanflag = 255;
		sgbTalkSavePos = sgbNextTalkSave;
	}
}
// 4B84CC: using guessed type char sgbNextTalkSave;
// 4B84CD: using guessed type char sgbTalkSavePos;
// 4B8840: using guessed type int sgbPlrTalkTbl;
// 4B8960: using guessed type int talkflag;
// 52571C: using guessed type int drawpanflag;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl control_reset_talk()
{
	talkflag = 0;
	sgbPlrTalkTbl = 0;
	drawpanflag = 255;
}
// 4B8840: using guessed type int sgbPlrTalkTbl;
// 4B8960: using guessed type int talkflag;
// 52571C: using guessed type int drawpanflag;

int __fastcall control_talk_last_key(int a1)
{
	char v1; // bl
	signed int v3; // eax

	v1 = a1;
	if ( gbMaxPlayers == 1 || !talkflag || (unsigned int)a1 < VK_SPACE )
		return 0;
	v3 = strlen(sgszTalkMsg);
	if ( v3 < 78 )
	{
		sgszTalkMsg[v3 + 1] = 0;
		sgszTalkMsg[v3] = v1;
	}
	return 1;
}
// 4B8960: using guessed type int talkflag;
// 679660: using guessed type char gbMaxPlayers;

int __fastcall control_presskeys(int a1)
{
	signed int v1; // eax
	char v2; // cl

	if ( gbMaxPlayers != 1 && talkflag )
	{
		switch ( a1 )
		{
			case VK_SPACE:
				return 1;
			case VK_ESCAPE:
				control_reset_talk();
				return 1;
			case VK_RETURN:
				control_press_enter();
				return 1;
			case VK_BACK:
				v1 = strlen(sgszTalkMsg);
				if ( v1 > 0 )
					sgszTalkMsg[v1 - 1] = '\0';
				return 1;
			case VK_DOWN:
				v2 = 1;
LABEL_15:
				control_up_down(v2);
				return 1;
			case VK_UP:
				v2 = -1;
				goto LABEL_15;
		}
	}
	return 0;
}
// 4B87A8: using guessed type int chrbtnactive;
// 4B8960: using guessed type int talkflag;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl control_press_enter()
{
	signed int v0; // esi
	char (*v1)[80]; // ebp
	char v2; // al
	int v3; // ecx
	char *v4; // ebp

	if ( sgszTalkMsg[0] )
	{
		control_reset_talk_msg();
		v0 = 0;
		v1 = sgszTalkSave;
		do
		{
			if ( !strcmp((const char *)v1, sgszTalkMsg) )
				break;
			++v1;
			++v0;
		}
		while ( (signed int)v1 < (signed int)&sgszTalkSave[8] );
		if ( v0 < 8 )
		{
			v2 = sgbNextTalkSave;
			v3 = (sgbNextTalkSave - 1) & 7;
			if ( v0 != v3 )
			{
				v4 = sgszTalkSave[v3];
				strcpy(sgszTalkSave[v0], sgszTalkSave[v3]);
				strcpy(v4, sgszTalkMsg);
				v2 = sgbNextTalkSave;
			}
		}
		else
		{
			strcpy(sgszTalkSave[(unsigned char)sgbNextTalkSave], sgszTalkMsg);
			v2 = (sgbNextTalkSave + 1) & 7;
			sgbNextTalkSave = (sgbNextTalkSave + 1) & 7;
		}
		sgszTalkMsg[0] = 0;
		sgbTalkSavePos = v2;
	}
	control_reset_talk();
}
// 4B84CC: using guessed type char sgbNextTalkSave;
// 4B84CD: using guessed type char sgbTalkSavePos;

void __fastcall control_up_down(char a1)
{
	unsigned char v1; // al
	int v2; // esi

	v1 = sgbTalkSavePos;
	v2 = 0;
	while ( 1 )
	{
		v1 = (a1 + v1) & 7;
		sgbTalkSavePos = v1;
		if ( sgszTalkSave[v1][0] )
			break;
		if ( ++v2 >= 8 )
			return;
	}
	strcpy(sgszTalkMsg, sgszTalkSave[v1]);
}
// 4B84CD: using guessed type char sgbTalkSavePos;
