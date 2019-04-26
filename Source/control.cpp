#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

char sgbNextTalkSave; // weak
char sgbTalkSavePos;  // weak
void *pDurIcons;
void *pChrButtons;
BOOL drawhpflag; // idb
BOOL dropGoldFlag;
WORD panbtn[8];
WORD chrbtn[4];
void *pMultiBtns;
void *pPanelButtons;
void *pChrPanel;
int lvlbtndown; // weak
char sgszTalkSave[8][80];
int dropGoldValue; // idb
BOOL drawmanaflag; // idb
BOOL chrbtnactive;
char sgszTalkMsg[MAX_SEND_STR_LEN];
BYTE *pPanelText;
int nGoldFrame;
BYTE *pLifeBuff;
BYTE *pBtmBuff;
void *pTalkBtns;
int pstrjust[4];
int pnumlines; // idb
BOOL pinfoflag;
WORD talkbtndown[3];
int pSpell; // weak
BYTE *pManaBuff;
char infoclr;      // weak
int sgbPlrTalkTbl; // weak // should be char [4]
void *pGBoxBuff;
void *pSBkBtnCel;
char tempstr[256];
char byte_4B894C[4];
int sbooktab;             // weak
int pSplType;             // weak
int frame;                // idb
int initialDropGoldIndex; // idb
int talkflag;             // weak
void *pSBkIconCels;
int sbookflag; // weak
int chrflag;
BOOL drawbtnflag;
void *pSpellBkCel;
char infostr[MAX_PATH];
int numpanbtns; // weak
void *pStatusPanel;
char panelstr[256];
int panelflag; // weak
unsigned char SplTransTbl[256];
int initialDropGoldValue; // idb
void *pSpellCels;
BOOL panbtndown;
void *pTalkPanel; // idb
int spselflag;    // weak

const unsigned char fontframe[128] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  54, 44, 57, 58, 56, 55, 47, 40, 41, 59, 39, 50, 37, 51, 52,
	36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 48, 49, 60, 38, 61, 53,
	62, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42, 63, 43, 64, 65,
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 40, 66, 41, 67, 0
};
const unsigned char fontkern[68] = {
	8, 10, 7, 9, 8, 7, 6, 8, 8, 3,
	3, 8, 6, 11, 9, 10, 6, 9, 9, 6,
	9, 11, 10, 13, 10, 11, 7, 5, 7, 7,
	8, 7, 7, 7, 7, 7, 10, 4, 5, 6,
	3, 3, 4, 3, 6, 6, 3, 3, 3, 3,
	3, 2, 7, 6, 3, 10, 10, 6, 6, 7,
	4, 4, 9, 6, 6, 12, 3, 7
};
const int lineoffset[25] = {
	768 * 594 + 241,
	768 * 32,
	768 * 32,
	768 * 32,
	768 * 32 + 180,
	768 * 582 + 241,
	768 * 606 + 241,
	768 * 32,
	768 * 32,
	768 * 32,
	768 * 576 + 241,
	768 * 594 + 241,
	768 * 612 + 241,
	768 * 32,
	768 * 32,
	768 * 572 + 241,
	768 * 587 + 241,
	768 * 601 + 241,
	768 * 616 + 241,
	768 * 32,
	768 * 570 + 241,
	768 * 582 + 241,
	768 * 594 + 241,
	768 * 606 + 241,
	768 * 617 + 241
};
const unsigned char gbFontTransTbl[256] = {
	// clang-format off
	'\0', 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	' ',  '!',  '\"', '#',  '$',  '%',  '&',  '\'', '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
	'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
	'@',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
	'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '[',  '\\', ']',  '^',  '_',
	'`',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
	'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '{',  '|',  '}',  '~',  0x01,
	'C',  'u',  'e',  'a',  'a',  'a',  'a',  'c',  'e',  'e',  'e',  'i',  'i',  'i',  'A',  'A',
	'E',  'a',  'A',  'o',  'o',  'o',  'u',  'u',  'y',  'O',  'U',  'c',  'L',  'Y',  'P',  'f',
	'a',  'i',  'o',  'u',  'n',  'N',  'a',  'o',  '?',  0x01, 0x01, 0x01, 0x01, '!',  '<',  '>',
	'o',  '+',  '2',  '3',  '\'', 'u',  'P',  '.',  ',',  '1',  '0',  '>',  0x01, 0x01, 0x01, '?',
	'A',  'A',  'A',  'A',  'A',  'A',  'A',  'C',  'E',  'E',  'E',  'E',  'I',  'I',  'I',  'I',
	'D',  'N',  'O',  'O',  'O',  'O',  'O',  'X',  '0',  'U',  'U',  'U',  'U',  'Y',  'b',  'B',
	'a',  'a',  'a',  'a',  'a',  'a',  'a',  'c',  'e',  'e',  'e',  'e',  'i',  'i',  'i',  'i',
	'o',  'n',  'o',  'o',  'o',  'o',  'o',  '/',  '0',  'u',  'u',  'u',  'u',  'y',  'b',  'y'
	// clang-format on
};

/* data */

char SpellITbl[MAX_SPELLS] = {
	1, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	28, 13, 12, 18, 16, 14, 18, 19, 11, 20,
	15, 21, 23, 24, 25, 22, 26, 29, 37, 38,
	39, 42, 41, 40, 10, 36, 30
};
int PanBtnPos[8][5] = {
	{ 9, 361, 71, 19, 1 },
	{ 9, 387, 71, 19, 0 },
	{ 9, 427, 71, 19, 1 },
	{ 9, 453, 71, 19, 0 },
	{ 560, 361, 71, 19, 1 },
	{ 560, 387, 71, 19, 0 },
	{ 87, 443, 33, 32, 1 },
	{ 527, 443, 33, 32, 1 }
};
char *PanBtnHotKey[8] = { "'c'", "'q'", "Tab", "Esc", "'i'", "'b'", "Enter", NULL };
char *PanBtnStr[8] = {
	"Character Information",
	"Quests log",
	"Automap",
	"Main Menu",
	"Inventory",
	"Spell book",
	"Send Message",
	"Player Attack"
};
RECT32 attribute_inc_rects[4] = {
	{ 137, 138, 41, 22 },
	{ 137, 166, 41, 22 },
	{ 137, 195, 41, 22 },
	{ 137, 223, 41, 22 }
};

int SpellPages[6][7] = {
	{ SPL_NULL, SPL_FIREBOLT, SPL_CBOLT, SPL_HBOLT, SPL_HEAL, SPL_HEALOTHER, SPL_FLAME },
	{ SPL_RESURRECT, SPL_FIREWALL, SPL_TELEKINESIS, SPL_LIGHTNING, SPL_TOWN, SPL_FLASH, SPL_STONE },
	{ SPL_RNDTELEPORT, SPL_MANASHIELD, SPL_ELEMENT, SPL_FIREBALL, SPL_WAVE, SPL_CHAIN, SPL_GUARDIAN },
	{ SPL_NOVA, SPL_GOLEM, SPL_TELEPORT, SPL_APOCA, SPL_BONESPIRIT, SPL_FLARE, SPL_ETHEREALIZE },
	{ -1, -1, -1, -1, -1, -1, -1 },
	{ -1, -1, -1, -1, -1, -1, -1 }
};

void DrawSpellCel(int xp, int yp, BYTE *Trans, int nCel, int w)
{
	BYTE *dst, *tbl, *end;

	/// ASSERT: assert(gpBuffer);

	dst = &gpBuffer[xp + PitchTbl[yp]];
	tbl = SplTransTbl;

#ifdef USE_ASM
	__asm {
		mov		ebx, Trans
		mov		eax, nCel
		shl		eax, 2
		add		ebx, eax
		mov		eax, [ebx+4]
		sub		eax, [ebx]
		mov		end, eax
		mov		esi, Trans
		add		esi, [ebx]
		mov		edi, dst
		mov		eax, end
		add		eax, esi
		mov		end, eax
		mov		ebx, tbl
	label1:
		mov		edx, w
	label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		label6
		sub		edx, eax
		mov		ecx, eax
		shr		ecx, 1
		jnb		label3
		lodsb
		xlat
		stosb
		jecxz	label5
	label3:
		shr		ecx, 1
		jnb		label4
		lodsw
		xlat
		ror		ax, 8
		xlat
		ror		ax, 8
		stosw
		jecxz	label5
	label4:
		lodsd
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		xlat
		ror		eax, 8
		stosd
		loop	label4
	label5:
		or		edx, edx
		jz		label7
		jmp		label2
	label6:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		label2
	label7:
		sub		edi, 768
		sub		edi, w
		cmp		esi, end
		jnz		label1
	}
#else
	int i;
	BYTE width;
	BYTE *src;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)&Trans[4 * nCel];
	src = &Trans[pFrameTable[0]];
	end = &src[pFrameTable[1] - pFrameTable[0]];

	for (; src != end; dst -= 768 + w) {
		for (i = w; i;) {
			width = *src++;
			if (!(width & 0x80)) {
				i -= width;
				// asm_cel_light_edge(width, tbl, dst, src);
				if (width & 1) {
					dst[0] = tbl[src[0]];
					src++;
					dst++;
				}
				width >>= 1;
				if (width & 1) {
					dst[0] = tbl[src[0]];
					dst[1] = tbl[src[1]];
					src += 2;
					dst += 2;
				}
				width >>= 1;
				for (; width; width--) {
					dst[0] = tbl[src[0]];
					dst[1] = tbl[src[1]];
					dst[2] = tbl[src[2]];
					dst[3] = tbl[src[3]];
					src += 4;
					dst += 4;
				}
			} else {
				width = -(char)width;
				dst += width;
				i -= width;
			}
		}
	}
#endif
}

void SetSpellTrans(char t)
{
	int i;

	if (t == RSPLTYPE_SKILL) {
		for (i = 0; i < 128; i++)
			SplTransTbl[i] = i;
	}
	for (i = 128; i < 256; i++)
		SplTransTbl[i] = i;
	SplTransTbl[255] = 0;

	switch (t) {
	case RSPLTYPE_SPELL:
		SplTransTbl[PAL8_YELLOW] = PAL16_BLUE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_BLUE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_BLUE + 5;
		for (i = PAL16_BLUE; i < PAL16_BLUE + 16; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_BLUE + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_BLUE + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_BLUE + i] = i;
		}
		break;
	case RSPLTYPE_SCROLL:
		SplTransTbl[PAL8_YELLOW] = PAL16_BEIGE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_BEIGE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_BEIGE + 5;
		for (i = PAL16_BEIGE; i < PAL16_BEIGE + 16; i++) {
			SplTransTbl[PAL16_YELLOW - PAL16_BEIGE + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_BEIGE + i] = i;
		}
		break;
	case RSPLTYPE_CHARGES:
		SplTransTbl[PAL8_YELLOW] = PAL16_ORANGE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_ORANGE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_ORANGE + 5;
		for (i = PAL16_ORANGE; i < PAL16_ORANGE + 16; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_ORANGE + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_ORANGE + i] = i;
		}
		break;
	case RSPLTYPE_INVALID:
		SplTransTbl[PAL8_YELLOW] = PAL16_GRAY + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_GRAY + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_GRAY + 5;
		for (i = PAL16_GRAY; i < PAL16_GRAY + 15; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_GRAY + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_GRAY + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_GRAY + i] = i;
		}
		SplTransTbl[PAL16_BEIGE + 15] = 0;
		SplTransTbl[PAL16_YELLOW + 15] = 0;
		SplTransTbl[PAL16_ORANGE + 15] = 0;
		break;
	}
}

void DrawSpell()
{
	char spl, st;
	int tlvl;

	spl = plr[myplr]._pRSpell;
	st = plr[myplr]._pRSplType;
	tlvl = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[spl];
	if (st == RSPLTYPE_SPELL && spl != SPL_INVALID) {
		if (!CheckSpell(myplr, spl, RSPLTYPE_SPELL, TRUE))
			st = RSPLTYPE_INVALID;
		if (tlvl <= 0)
			st = RSPLTYPE_INVALID;
	}
	if (!currlevel && st != RSPLTYPE_INVALID && !spelldata[spl].sTownSpell)
		st = RSPLTYPE_INVALID;
	if (plr[myplr]._pRSpell < 0)
		st = RSPLTYPE_INVALID;
	SetSpellTrans(st);
	if (spl != SPL_INVALID)
		DrawSpellCel(629, 631, (BYTE *)pSpellCels, SpellITbl[spl], 56);
	else
		DrawSpellCel(629, 631, (BYTE *)pSpellCels, 27, 56);
}

void DrawSpellList()
{
	int v0;               // esi
	signed int v1;        // edi
	signed int v4;        // ebp
	int v5;               // eax
	int v6;               // esi
	int v7;               // eax
	BOOLEAN v8;           // sf
	int v9;               // esi
	int v10;              // eax
	int v11;              // ebp
	int v12;              // edx
	int *v13;             // ecx
	int *v14;             // eax
	signed int v15;       // edx
	signed int v16;       // edi
	int v17;              // [esp+Ch] [ebp-34h]
	__int32 xp;           // [esp+10h] [ebp-30h]
	__int32 yp;           // [esp+14h] [ebp-2Ch]
	BOOL *v20;            // [esp+18h] [ebp-28h]
	__int32 nCel;         // [esp+1Ch] [ebp-24h]
	int v22;              // [esp+20h] [ebp-20h]
	__int32 v23;          // [esp+24h] [ebp-1Ch]
	signed int v24;       // [esp+28h] [ebp-18h]
	unsigned __int64 v25; // [esp+2Ch] [ebp-14h]
	signed __int64 v26;   // [esp+34h] [ebp-Ch]

	pSpell = -1;
	infostr[0] = 0;
	v17 = 636;
	xp = 495;
	ClearPanel();
	v0 = myplr;
	v1 = RSPLTYPE_SKILL;
	v24 = 0;
	nCel = 0;
	do {
		switch (v1) {
		case RSPLTYPE_SKILL:
			SetSpellTrans(RSPLTYPE_SKILL);
			yp = 46;
			v25 = plr[v0]._pAblSpells;
			break;
		case RSPLTYPE_SPELL:
			yp = 47;
			v25 = plr[v0]._pMemSpells;
			break;
		case RSPLTYPE_SCROLL:
			SetSpellTrans(RSPLTYPE_SCROLL);
			yp = 44;
			v25 = plr[v0]._pScrlSpells;
			break;
		case RSPLTYPE_CHARGES:
			SetSpellTrans(RSPLTYPE_CHARGES);
			yp = 45;
			v25 = plr[v0]._pISpells;
			break;
		}
		v20 = &spelldata[1].sTownSpell;
		v4 = 1;
		v26 = (__int64)1;
		v23 = 1;
		v22 = xp - 216;
		do {
			if (!(v25 & v26))
				goto LABEL_68;
			if (v1 == RSPLTYPE_SPELL) {
				v5 = v0;
				v6 = plr[v0]._pSplLvl[v4];
				v7 = plr[v5]._pISplLvlAdd;
				v8 = v7 + v6 < 0;
				v9 = v7 + v6;
				nCel = v9;
				if (v8) {
					nCel = 0;
					v9 = 0;
				}
				SetSpellTrans(v9 <= 0 ? RSPLTYPE_INVALID : RSPLTYPE_SPELL);
			} else {
				v9 = nCel;
			}
			if (!currlevel && !*v20)
				SetSpellTrans(RSPLTYPE_INVALID);
			DrawSpellCel(v17, xp, (BYTE *)pSpellCels, SpellITbl[v4], 56);
			if (MouseX >= v17 - 64 && MouseX < v17 - 64 + 56 && MouseY >= v22 && MouseY < v22 + 56) {
				pSpell = v4;
				pSplType = v1;
				DrawSpellCel(v17, xp, (BYTE *)pSpellCels, yp, 56);
				if (v1) {
					switch (v1) {
					case RSPLTYPE_SPELL:
						sprintf(infostr, "%s Spell", spelldata[pSpell].sNameText);
						if (pSpell == 31) {
							sprintf(tempstr, "Damages undead only");
							AddPanelString(tempstr, 1);
						}
						if (v9)
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
						if (v12 > 0) {
							v13 = &plr[v10].InvList[0]._iMiscId;
							do {
								if (*(v13 - 53) != -1
								    && (*v13 == IMISC_SCROLL || *v13 == IMISC_SCROLLT)
								    && v13[1] == pSpell) {
									++v11;
								}
								v13 += 92;
								--v12;
							} while (v12);
						}
						v14 = &plr[v10].SpdList[0]._iMiscId;
						v15 = MAXBELTITEMS;
						do {
							if (*(v14 - 53) != -1
							    && (*v14 == IMISC_SCROLL || *v14 == IMISC_SCROLLT)
							    && v14[1] == pSpell) {
								++v11;
							}
							v14 += 92;
							--v15;
						} while (v15);
						if (v11 == 1)
							strcpy(tempstr, "1 Scroll");
						else
							sprintf(tempstr, "%i Scrolls", v11);
						AddPanelString(tempstr, 1);
						v4 = v23;
						break;
					case RSPLTYPE_CHARGES:
						sprintf(infostr, "Staff of %s", spelldata[pSpell].sNameText);
						if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges == 1)
							strcpy(tempstr, "1 Charge");
						else
							sprintf(tempstr, "%i Charges", plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges);
						goto LABEL_32;
					}
				} else {
					sprintf(infostr, "%s Skill", spelldata[pSpell].sSkillText);
				}
				v0 = myplr;
				v16 = 0;
				do {
					if (plr[v0]._pSplHotKey[v16] == pSpell && plr[v0]._pSplTHotKey[v16] == pSplType) {
						DrawSpellCel(v17, xp, (BYTE *)pSpellCels, v16 + 48, 56);
						sprintf(tempstr, "Spell Hot Key #F%i", v16 + 5);
						AddPanelString(tempstr, 1);
						v0 = myplr;
					}
					++v16;
				} while (v16 < 4);
				v1 = v24;
				goto LABEL_66;
			}
			v0 = myplr;
		LABEL_66:
			v17 -= 56;
			if (v17 == 20) {
				xp -= 56;
				v22 -= 56;
				v17 = 636;
			}
		LABEL_68:
			v20 += 14;
			++v4;
			v26 *= (__int64)2;
			v23 = v4;
		} while ((INT_PTR)v20 < (INT_PTR)&spelldata[MAX_SPELLS].sTownSpell);
		if (v25 && v17 != 636)
			v17 -= 56;
		if (v17 == 20) {
			xp -= 56;
			v17 = 636;
		}
		v24 = ++v1;
	} while (v1 < 4);
}
// 4B8834: using guessed type int pSpell;
// 4B8954: using guessed type int pSplType;

void SetSpell()
{
	spselflag = 0;
	if (pSpell != -1) {
		ClearPanel();
		drawpanflag = 255;
		plr[myplr]._pRSpell = pSpell;
		plr[myplr]._pRSplType = pSplType;
	}
}
// 4B8834: using guessed type int pSpell;
// 4B8954: using guessed type int pSplType;
// 4B8C98: using guessed type int spselflag;
// 52571C: using guessed type int drawpanflag;

void SetSpeedSpell(int slot)
{
	int i;

	if (pSpell != SPL_INVALID) {
		for (i = 0; i < 4; ++i) {
			if (plr[myplr]._pSplHotKey[i] == pSpell && plr[myplr]._pSplTHotKey[i] == pSplType)
				plr[myplr]._pSplHotKey[i] = SPL_INVALID;
		}
		plr[myplr]._pSplHotKey[slot] = pSpell;
		plr[myplr]._pSplTHotKey[slot] = pSplType;
	}
}

void ToggleSpell(int slot)
{
	unsigned __int64 spells;

	if (plr[myplr]._pSplHotKey[slot] == -1) {
		return;
	}

	switch (plr[myplr]._pSplTHotKey[slot]) {
	case RSPLTYPE_SKILL:
		spells = plr[myplr]._pAblSpells;
		break;
	case RSPLTYPE_SPELL:
		spells = plr[myplr]._pMemSpells;
		break;
	case RSPLTYPE_SCROLL:
		spells = plr[myplr]._pScrlSpells;
		break;
	case RSPLTYPE_CHARGES:
		spells = plr[myplr]._pISpells;
		break;
	}

	if (spells & (__int64)1 << (plr[myplr]._pSplHotKey[slot] - 1)) {
		plr[myplr]._pRSpell = plr[myplr]._pSplHotKey[slot];
		plr[myplr]._pRSplType = plr[myplr]._pSplTHotKey[slot];
		drawpanflag = 255;
	}
}
// 52571C: using guessed type int drawpanflag;

void CPrintString(int nOffset, int nCel, char col)
{
	/// ASSERT: assert(gpBuffer);

#ifdef USE_ASM
	__asm {
		mov		ebx, pPanelText
		mov		eax, nCel
		shl		eax, 2
		add		ebx, eax
		mov		edx, [ebx+4]
		sub		edx, [ebx]
		mov		esi, pPanelText
		add		esi, [ebx]
		mov		edi, gpBuffer
		add		edi, nOffset
		mov		ebx, edx
		add		ebx, esi
		xor		edx, edx
		mov		dl, col
		cmp		edx, COL_WHITE
		jz		c0_label1
		cmp		edx, COL_BLUE
		jz		c1_label1
		cmp		edx, COL_RED
		jz		c2_label1
		jmp		d_label1

		            // Case 0
	c0_label1:
		mov		edx, 13
	c0_label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		c0_label6
		sub		edx, eax
		mov		ecx, eax
		shr		ecx, 1
		jnb		c0_label3
		movsb
		jecxz	c0_label5
	c0_label3:
		shr		ecx, 1
		jnb		c0_label4
		movsw
		jecxz	c0_label5
	c0_label4:
		rep movsd
	c0_label5:
		or		edx, edx
		jz		c0_label7
		jmp		c0_label2
	c0_label6:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		c0_label2
	c0_label7:
		sub		edi, 768 + 13
		cmp		ebx, esi
		jnz		c0_label1
		jmp		labret

		            // Case 1
	c1_label1:
		mov		edx, 13
	c1_label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		c1_label6
		sub		edx, eax
		mov		ecx, eax
	c1_label3:
		lodsb
		cmp		al, PAL16_GRAY + 13
		ja		c1_label4
		cmp		al, PAL16_GRAY
		jb		c1_label5
		sub		al, PAL16_GRAY - (PAL16_BLUE + 2)
		jmp		c1_label5
	c1_label4:
		mov		al, PAL16_BLUE + 15
	c1_label5:
		stosb
		loop	c1_label3
		or		edx, edx
		jz		c1_label7
		jmp		c1_label2
	c1_label6:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		c1_label2
	c1_label7:
		sub		edi, 768 + 13
		cmp		ebx, esi
		jnz		c1_label1
		jmp		labret

		            // Case 2
	c2_label1:
		mov		edx, 13
	c2_label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		c2_label5
		sub		edx, eax
		mov		ecx, eax
	c2_label3:
		lodsb
		cmp		al, PAL16_GRAY
		jb		c2_label4
		sub		al, PAL16_GRAY - PAL16_RED
	c2_label4:
		stosb
		loop	c2_label3
		or		edx, edx
		jz		c2_label6
		jmp		c2_label2
	c2_label5:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		c2_label2
	c2_label6:
		sub		edi, 768 + 13
		cmp		ebx, esi
		jnz		c2_label1
		jmp		labret

		            // Default
	d_label1:
		mov		edx, 13
	d_label2:
		xor		eax, eax
		lodsb
		or		al, al
		js		d_label6
		sub		edx, eax
		mov		ecx, eax
	d_label3:
		lodsb
		cmp		al, PAL16_GRAY
		jb		d_label5
		cmp		al, PAL16_GRAY + 14
		jnb		d_label4
		sub		al, PAL16_GRAY - (PAL16_YELLOW + 2)
		jmp		d_label5
	d_label4:
		mov		al, PAL16_YELLOW + 15
	d_label5:
		stosb
		loop	d_label3
		or		edx, edx
		jz		d_label7
		jmp		d_label2
	d_label6:
		neg		al
		add		edi, eax
		sub		edx, eax
		jnz		d_label2
	d_label7:
		sub		edi, 768 + 13
		cmp		ebx, esi
		jnz		d_label1

	labret:
	}
#else
	int i;
	BYTE width, pix;
	BYTE *src, *dst, *end;
	DWORD *pFrameTable;

	pFrameTable = (DWORD *)&pPanelText[4 * nCel];
	src = &pPanelText[pFrameTable[0]];
	end = &src[pFrameTable[1] - pFrameTable[0]];
	dst = &gpBuffer[nOffset];

	switch (col) {
	case COL_WHITE:
		for (; src != end; dst -= 768 + 13) {
			for (i = 13; i;) {
				width = *src++;
				if (!(width & 0x80)) {
					i -= width;
					if (width & 1) {
						dst[0] = src[0];
						src++;
						dst++;
					}
					width >>= 1;
					if (width & 1) {
						dst[0] = src[0];
						dst[1] = src[1];
						src += 2;
						dst += 2;
					}
					width >>= 1;
					while (width) {
						dst[0] = src[0];
						dst[1] = src[1];
						dst[2] = src[2];
						dst[3] = src[3];
						src += 4;
						dst += 4;
						width--;
					}
				} else {
					width = -(char)width;
					dst += width;
					i -= width;
				}
			}
		}
		break;
	case COL_BLUE:
		for (; src != end; dst -= 768 + 13) {
			for (i = 13; i;) {
				width = *src++;
				if (!(width & 0x80)) {
					i -= width;
					while (width) {
						pix = *src++;
						if (pix > PAL16_GRAY + 13)
							pix = PAL16_BLUE + 15;
						else if (pix >= PAL16_GRAY)
							pix -= PAL16_GRAY - (PAL16_BLUE + 2);
						*dst++ = pix;
						width--;
					}
				} else {
					width = -(char)width;
					dst += width;
					i -= width;
				}
			}
		}
		break;
	case COL_RED:
		for (; src != end; dst -= 768 + 13) {
			for (i = 13; i;) {
				width = *src++;
				if (!(width & 0x80)) {
					i -= width;
					while (width) {
						pix = *src++;
						if (pix >= PAL16_GRAY)
							pix -= PAL16_GRAY - PAL16_RED;
						*dst++ = pix;
						width--;
					}
				} else {
					width = -(char)width;
					dst += width;
					i -= width;
				}
			}
		}
		break;
	default:
		for (; src != end; dst -= 768 + 13) {
			for (i = 13; i;) {
				width = *src++;
				if (!(width & 0x80)) {
					i -= width;
					while (width) {
						pix = *src++;
						if (pix >= PAL16_GRAY) {
							if (pix >= PAL16_GRAY + 14)
								pix = PAL16_YELLOW + 15;
							else
								pix -= PAL16_GRAY - (PAL16_YELLOW + 2);
						}
						*dst++ = pix;
						width--;
					}
				} else {
					width = -(char)width;
					dst += width;
					i -= width;
				}
			}
		}
		break;
	}
#endif
}

void AddPanelString(char *str, BOOL just)
{
	strcpy(&panelstr[64 * pnumlines], str);
	pstrjust[pnumlines] = just;

	if (pnumlines < 4)
		pnumlines++;
}

void ClearPanel()
{
	pnumlines = 0;
	pinfoflag = FALSE;
}

void DrawPanelBox(int x, int y, int w, int h, int sx, int sy)
{
	int nSrcOff, nDstOff;

	/// ASSERT: assert(gpBuffer);

	nSrcOff = x + 640 * y;
	nDstOff = sx + 768 * sy;

#ifdef USE_ASM
	__asm {
		mov		esi, pBtmBuff
		add		esi, nSrcOff
		mov		edi, gpBuffer
		add		edi, nDstOff
		xor		ebx, ebx
		mov		bx, word ptr w
		xor		edx, edx
		mov		dx, word ptr h
	label1:
		mov		ecx, ebx
		shr		ecx, 1
		jnb		label2
		movsb
		jecxz	label4
	label2:
		shr		ecx, 1
		jnb		label3
		movsw
		jecxz	label4
	label3:
		rep movsd
	label4:
		add		esi, 640
		sub		esi, ebx
		add		edi, 768
		sub		edi, ebx
		dec		edx
		jnz		label1
	}
#else
	int wdt, hgt;
	BYTE *src, *dst;

	src = &pBtmBuff[nSrcOff];
	dst = &gpBuffer[nDstOff];

	for (hgt = h; hgt; hgt--, src += 640 - w, dst += 768 - w) {
		wdt = w;
		if (wdt & 1) {
			dst[0] = src[0];
			src++;
			dst++;
		}
		wdt >>= 1;
		if (wdt & 1) {
			dst[0] = src[0];
			dst[1] = src[1];
			src += 2;
			dst += 2;
		}
		wdt >>= 1;
		while (wdt) {
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
			dst[3] = src[3];
			src += 4;
			dst += 4;
			wdt--;
		}
	}
#endif
}

void SetFlaskHeight(BYTE *pCelBuff, int min, int max, int c, int r)
{
	int nSrcOff, nDstOff, w;

	/// ASSERT: assert(gpBuffer);

	nSrcOff = 88 * min;
	nDstOff = c + 768 * r;
	w = max - min;

#ifdef USE_ASM
	__asm {
		mov		esi, pCelBuff
		add		esi, nSrcOff
		mov		edi, gpBuffer
		add		edi, nDstOff
		mov		edx, w
	label1:
		mov		ecx, 88 / 4
		rep movsd
		add		edi, 768 - 88
		dec		edx
		jnz		label1
	}
#else
	BYTE *src, *dst;

	src = &pCelBuff[nSrcOff];
	dst = &gpBuffer[nDstOff];

	for (; w; w--, src += 88, dst += 768)
		memcpy(dst, src, 88);
#endif
}

void DrawFlask(BYTE *pCelBuff, int w, int nSrcOff, BYTE *pBuff, int nDstOff, int h)
{
#ifdef USE_ASM
	__asm {
		mov		esi, pCelBuff
		add		esi, nSrcOff
		mov		edi, pBuff
		add		edi, nDstOff
		mov		edx, h
	label1:
		mov		ecx, 59
	label2:
		lodsb
		or		al, al
		jz		label3
		mov		[edi], al
	label3:
		inc		edi
		loop	label2
		add		esi, w
		sub		esi, 59
		add		edi, 768 - 59
		dec		edx
		jnz		label1
	}
#else
	int wdt, hgt;
	BYTE *src, *dst;

	src = &pCelBuff[nSrcOff];
	dst = &pBuff[nDstOff];

	for (hgt = h; hgt; hgt--, src += w - 59, dst += 768 - 59) {
		for (wdt = 59; wdt; wdt--) {
			if (*src)
				*dst = *src;
			src++;
			dst++;
		}
	}
#endif
}

void DrawLifeFlask()
{
	int filled = (double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP * 80.0;
	plr[myplr]._pHPPer = filled;

	if (filled > 80)
		filled = 80;
	filled = 80 - filled;
	if (filled > 11)
		filled = 11;
	filled += 2;

	DrawFlask(pLifeBuff, 88, 277, gpBuffer, 768 * 499 + 173, filled);
	if (filled != 13)
		DrawFlask(pBtmBuff, 640, 640 * filled + 2029, gpBuffer, 768 * filled + 768 * 499 + 173, 13 - filled);
}

void UpdateLifeFlask()
{
	int filled = (double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP * 80.0;
	plr[myplr]._pHPPer = filled;

	if (filled > 69)
		filled = 69;
	else if (filled < 0)
		filled = 0;
	if (filled != 69)
		SetFlaskHeight(pLifeBuff, 16, 85 - filled, 160, 512);
	if (filled)
		DrawPanelBox(96, 85 - filled, 88, filled, 160, 581 - filled);
}

void DrawManaFlask()
{
	int filled = plr[myplr]._pManaPer;
	if (filled > 80)
		filled = 80;
	filled = 80 - filled;
	if (filled > 11)
		filled = 11;
	filled += 2;

	DrawFlask(pManaBuff, 88, 277, gpBuffer, 768 * 499 + 173 + 366, filled);
	if (filled != 13)
		DrawFlask(pBtmBuff, 640, 640 * filled + 2029 + 366, gpBuffer, 768 * filled + 768 * 499 + 173 + 366, 13 - filled);
}

void control_update_life_mana()
{
	int manaPer;
	int maxMana = plr[myplr]._pMaxMana;
	int mana = plr[myplr]._pMana;
	if (maxMana < 0)
		maxMana = 0;
	if (mana < 0)
		mana = 0;
	if (maxMana == 0)
		manaPer = 0;
	else
		manaPer = (double)mana / (double)maxMana * 80.0;
	plr[myplr]._pManaPer = manaPer;
	plr[myplr]._pHPPer = (double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP * 80.0;
}

void UpdateManaFlask()
{
	int filled;
	int maxMana = plr[myplr]._pMaxMana;
	int mana = plr[myplr]._pMana;
	if (maxMana < 0)
		maxMana = 0;
	if (mana < 0)
		mana = 0;

	if (maxMana == 0)
		filled = 0;
	else
		filled = (double)mana / (double)maxMana * 80.0;

	plr[myplr]._pManaPer = filled;

	if (filled > 69)
		filled = 69;
	if (filled != 69)
		SetFlaskHeight(pManaBuff, 16, 85 - filled, 160 + 368, 512);
	if (filled)
		DrawPanelBox(96 + 368, 85 - filled, 88, filled, 160 + 368, 581 - filled);

	DrawSpell();
}

void InitControlPan()
{
	int i;

	if (gbMaxPlayers == 1) {
		pBtmBuff = DiabloAllocPtr(144 * 640);
		memset(pBtmBuff, 0, 144 * 640);
	} else {
		pBtmBuff = DiabloAllocPtr(288 * 640);
		memset(pBtmBuff, 0, 288 * 640);
	}
	pManaBuff = DiabloAllocPtr(88 * 88);
	memset(pManaBuff, 0, 88 * 88);
	pLifeBuff = DiabloAllocPtr(88 * 88);
	memset(pLifeBuff, 0, 88 * 88);
	pPanelText = LoadFileInMem("CtrlPan\\SmalText.CEL", 0);
	pChrPanel = LoadFileInMem("Data\\Char.CEL", 0);
	pSpellCels = LoadFileInMem("CtrlPan\\SpelIcon.CEL", 0);
	SetSpellTrans(RSPLTYPE_SKILL);
	pStatusPanel = LoadFileInMem("CtrlPan\\Panel8.CEL", 0);
	CelDecodeRect((BYTE *)pBtmBuff, 0, 143, 640, (BYTE *)pStatusPanel, 1, 640);
	MemFreeDbg(pStatusPanel);
	pStatusPanel = LoadFileInMem("CtrlPan\\P8Bulbs.CEL", 0);
	CelDecodeRect((BYTE *)pLifeBuff, 0, 87, 88, (BYTE *)pStatusPanel, 1, 88);
	CelDecodeRect((BYTE *)pManaBuff, 0, 87, 88, (BYTE *)pStatusPanel, 2, 88);
	MemFreeDbg(pStatusPanel);
	talkflag = 0;
	if (gbMaxPlayers != 1) {
		pTalkPanel = LoadFileInMem("CtrlPan\\TalkPanl.CEL", 0);
		CelDecodeRect((BYTE *)pBtmBuff, 0, 287, 640, (BYTE *)pTalkPanel, 1, 640);
		MemFreeDbg(pTalkPanel);
		pMultiBtns = LoadFileInMem("CtrlPan\\P8But2.CEL", 0);
		pTalkBtns = LoadFileInMem("CtrlPan\\TalkButt.CEL", 0);
		sgbPlrTalkTbl = 0;
		sgszTalkMsg[0] = 0;
		for (i = 0; i < sizeof(byte_4B894C); i++)
			byte_4B894C[i] = 1;
		for (i = 0; i < sizeof(talkbtndown); i++)
			talkbtndown[i] = 0;
	}
	panelflag = 0;
	lvlbtndown = 0;
	pPanelButtons = LoadFileInMem("CtrlPan\\Panel8bu.CEL", 0);
	for (i = 0; i < sizeof(panbtn); i++)
		panbtn[i] = 0;
	panbtndown = 0;
	if (gbMaxPlayers == 1)
		numpanbtns = 6;
	else
		numpanbtns = 8;
	pChrButtons = LoadFileInMem("Data\\CharBut.CEL", 0);
	for (i = 0; i < sizeof(chrbtn); i++)
		chrbtn[i] = 0;
	chrbtnactive = FALSE;
	pDurIcons = LoadFileInMem("Items\\DurIcons.CEL", 0);
	strcpy(infostr, "");
	ClearPanel();
	drawhpflag = TRUE;
	drawmanaflag = TRUE;
	chrflag = 0;
	spselflag = 0;
	pSpellBkCel = LoadFileInMem("Data\\SpellBk.CEL", 0);
	pSBkBtnCel = LoadFileInMem("Data\\SpellBkB.CEL", 0);
	pSBkIconCels = LoadFileInMem("Data\\SpellI2.CEL", 0);
	sbooktab = 0;
	sbookflag = 0;
	if (plr[myplr]._pClass == PC_WARRIOR) {
		SpellPages[0][0] = SPL_REPAIR;
	} else if (plr[myplr]._pClass == PC_ROGUE) {
		SpellPages[0][0] = SPL_DISARM;
	} else if (plr[myplr]._pClass == PC_SORCERER) {
		SpellPages[0][0] = SPL_RECHARGE;
	}
	pQLogCel = LoadFileInMem("Data\\Quest.CEL", 0);
	pGBoxBuff = LoadFileInMem("CtrlPan\\Golddrop.cel", 0);
	dropGoldFlag = FALSE;
	dropGoldValue = 0;
	initialDropGoldValue = 0;
	initialDropGoldIndex = 0;
	nGoldFrame = 1;
}

void ClearCtrlPan()
{
	DrawPanelBox(0, sgbPlrTalkTbl + 16, 640, 128, 64, 512);
	DrawInfoBox();
}
// 4B8840: using guessed type int sgbPlrTalkTbl;

void DrawCtrlPan()
{
	int i;

	for (i = 0; i < 6; i++) {
		if (!panbtn[i])
			DrawPanelBox(PanBtnPos[i][0], PanBtnPos[i][1] - 336, 71, 20, PanBtnPos[i][0] + 64, PanBtnPos[i][1] + 160);
		else
			CelDecodeOnly(PanBtnPos[i][0] + 64, PanBtnPos[i][1] + 178, (BYTE *)pPanelButtons, i + 1, 71);
	}
	if (numpanbtns == 8) {
		CelDecodeOnly(151, 634, (BYTE *)pMultiBtns, panbtn[6] + 1, 33);
		if (FriendlyMode)
			CelDecodeOnly(591, 634, (BYTE *)pMultiBtns, panbtn[7] + 3, 33);
		else
			CelDecodeOnly(591, 634, (BYTE *)pMultiBtns, panbtn[7] + 5, 33);
	}
}

void DoSpeedBook()
{
	unsigned __int64 spells, spell;
	int xo, yo, X, Y, i, j;

	spselflag = 1;
	xo = 636;
	yo = 495;
	X = 600;
	Y = 307;
	if (plr[myplr]._pRSpell != -1) {
		for (i = 0; i < 4; i++) {
			switch (i) {
			case RSPLTYPE_SKILL:
				spells = plr[myplr]._pAblSpells;
				break;
			case RSPLTYPE_SPELL:
				spells = plr[myplr]._pMemSpells;
				break;
			case RSPLTYPE_SCROLL:
				spells = plr[myplr]._pScrlSpells;
				break;
			case RSPLTYPE_CHARGES:
				spells = plr[myplr]._pISpells;
				break;
			}
			spell = (__int64)1;
			for (j = 1; j < MAX_SPELLS; j++) {
				if (spell & spells) {
					if (j == plr[myplr]._pRSpell && i == plr[myplr]._pRSplType) {
						X = xo - 36;
						Y = yo - 188;
					}
					xo -= 56;
					if (xo == 20) {
						xo = 636;
						yo -= 56;
					}
				}
				spell <<= (__int64)1;
			}
			if (spells && xo != 636)
				xo -= 56;
			if (xo == 20) {
				xo = 636;
				yo -= 56;
			}
		}
	}

	SetCursorPos(X, Y);
}
// 4B8C98: using guessed type int spselflag;

void DoPanBtn()
{
	int i;

	for (i = 0; i < numpanbtns; i++) {
		if (MouseX >= PanBtnPos[i][0] && MouseX <= PanBtnPos[i][0] + PanBtnPos[i][2]) {
			if (MouseY >= PanBtnPos[i][1] && MouseY <= PanBtnPos[i][1] + PanBtnPos[i][3]) {
				panbtn[i] = 1;
				drawbtnflag = TRUE;
				panbtndown = TRUE;
			}
		}
	}
	if (!spselflag && MouseX >= 565 && MouseX < 621 && MouseY >= 416 && MouseY < 472) {
		DoSpeedBook();
		gamemenu_off();
	}
}

void control_set_button_down(int btn_id)
{
	panbtn[btn_id] = 1;
	drawbtnflag = TRUE;
	panbtndown = TRUE;
}

void control_check_btn_press()
{
	int v0; // edx
	int v1; // esi

	v0 = MouseX;
	v1 = MouseY;
	if (MouseX >= PanBtnPos[3][0]
	    && MouseX <= PanBtnPos[3][0] + PanBtnPos[3][2]
	    && MouseY >= PanBtnPos[3][1]
	    && MouseY <= PanBtnPos[3][1] + PanBtnPos[3][3]) {
		control_set_button_down(3);
	}
	if (v0 >= PanBtnPos[6][0]
	    && v0 <= PanBtnPos[6][0] + PanBtnPos[6][2]
	    && v1 >= PanBtnPos[6][1]
	    && v1 <= PanBtnPos[6][1] + PanBtnPos[6][3]) {
		control_set_button_down(6);
	}
}

void DoAutoMap()
{
	if (currlevel || gbMaxPlayers != 1) {
		if (!automapflag)
			StartAutomap();
		else
			automapflag = 0;
	} else {
		InitDiabloMsg(EMSG_NO_AUTOMAP_IN_TOWN);
	}
}
// 679660: using guessed type char gbMaxPlayers;

void CheckPanelInfo()
{
	int v0;         // edi
	int v1;         // eax
	int v2;         // ecx
	int v3;         // ecx
	int v4;         // edi
	int v5;         // eax
	int *v6;        // edx
	int v7;         // ebx
	int v8;         // ebx
	int *v9;        // eax
	signed int v10; // edx
	int v11;        // ecx
	int v12;        // [esp+10h] [ebp-4h]

	v0 = 0;
	panelflag = 0;
	ClearPanel();
	if (numpanbtns > 0) {
		do {
			v1 = v0;
			v2 = PanBtnPos[v0][0];
			if (MouseX >= v2 && MouseX <= v2 + PanBtnPos[v1][2]) {
				v3 = PanBtnPos[v1][1];
				if (MouseY >= v3 && MouseY <= v3 + PanBtnPos[v1][3]) {
					if (v0 == 7) {
						if (FriendlyMode)
							strcpy(infostr, "Player friendly");
						else
							strcpy(infostr, "Player attack");
					} else {
						strcpy(infostr, PanBtnStr[v0]);
					}
					if (PanBtnHotKey[v0]) {
						sprintf(tempstr, "Hotkey : %s", PanBtnHotKey[v0]);
						AddPanelString(tempstr, 1);
					}
					infoclr = COL_WHITE;
					panelflag = 1;
					pinfoflag = TRUE;
				}
			}
			++v0;
		} while (v0 < numpanbtns);
	}
	if (!spselflag && MouseX >= 565 && MouseX < 621 && MouseY >= 416 && MouseY < 472) {
		strcpy(infostr, "Select current spell button");
		infoclr = COL_WHITE;
		panelflag = 1;
		pinfoflag = TRUE;
		strcpy(tempstr, "Hotkey : 's'");
		AddPanelString(tempstr, 1);
		v4 = plr[myplr]._pRSpell;
		if (v4 != -1) {
			switch (_LOBYTE(plr[myplr]._pRSplType)) {
			case RSPLTYPE_SKILL:
				sprintf(tempstr, "%s Skill", spelldata[v4].sSkillText);
			LABEL_54:
				AddPanelString(tempstr, 1);
				break;
			case RSPLTYPE_SPELL:
				sprintf(tempstr, "%s Spell", spelldata[v4].sNameText);
				AddPanelString(tempstr, 1);
				v11 = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[v4];
				if (v11 < 0)
					v11 = 0;
				if (v11)
					sprintf(tempstr, "Spell Level %i", v11);
				else
					sprintf(tempstr, "Spell Level 0 - Unusable");
				goto LABEL_54;
			case RSPLTYPE_SCROLL:
				sprintf(tempstr, "Scroll of %s", spelldata[v4].sNameText);
				AddPanelString(tempstr, 1);
				v12 = 0;
				v5 = myplr;
				if (plr[myplr]._pNumInv > 0) {
					v6 = &plr[v5].InvList[0]._iMiscId;
					v7 = plr[myplr]._pNumInv;
					do {
						if (*(v6 - 53) != -1 && (*v6 == IMISC_SCROLL || *v6 == IMISC_SCROLLT) && v6[1] == v4)
							++v12;
						v6 += 92;
						--v7;
					} while (v7);
				}
				v8 = v12;
				v9 = &plr[v5].SpdList[0]._iMiscId;
				v10 = MAXBELTITEMS;
				do {
					if (*(v9 - 53) != -1 && (*v9 == IMISC_SCROLL || *v9 == IMISC_SCROLLT) && v9[1] == v4)
						++v8;
					v9 += 92;
					--v10;
				} while (v10);
				if (v8 == 1)
					strcpy(tempstr, "1 Scroll");
				else
					sprintf(tempstr, "%i Scrolls", v8);
				goto LABEL_54;
			case RSPLTYPE_CHARGES:
				sprintf(tempstr, "Staff of %s", spelldata[v4].sNameText);
				AddPanelString(tempstr, 1);
				if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges == 1)
					strcpy(tempstr, "1 Charge");
				else
					sprintf(tempstr, "%i Charges", plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges);
				goto LABEL_54;
			}
		}
	}
	if (MouseX > 190 && MouseX < 437 && MouseY > 356 && MouseY < 385)
		pcursinvitem = CheckInvHLight();
}
// 484368: using guessed type int FriendlyMode;
// 4B883C: using guessed type int infoclr;
// 4B8A7C: using guessed type int numpanbtns;
// 4B8B84: using guessed type int panelflag;
// 4B8C98: using guessed type int spselflag;
// 4B8CB8: using guessed type char pcursinvitem;

void CheckBtnUp()
{
	int i;
	char gamemenuOff;

	gamemenuOff = 1;
	drawbtnflag = TRUE;
	panbtndown = FALSE;

	for (i = 0; i < 8; i++) {
		if (!panbtn[i]) {
			continue;
		}

		panbtn[i] = 0;

		if (MouseX < PanBtnPos[i][0]
		    || MouseX > PanBtnPos[i][0] + PanBtnPos[i][2]
		    || MouseY < PanBtnPos[i][1]
		    || MouseY > PanBtnPos[i][1] + PanBtnPos[i][3]) {
			continue;
		}

		switch (i) {
		case PANBTN_CHARINFO:
			questlog = FALSE;
			chrflag = chrflag == 0;
			break;
		case PANBTN_QLOG:
			chrflag = 0;
			if (!questlog)
				StartQuestlog();
			else
				questlog = FALSE;
			break;
		case PANBTN_AUTOMAP:
			DoAutoMap();
			break;
		case PANBTN_MAINMENU:
			qtextflag = FALSE;
			gamemenu_handle_previous();
			gamemenuOff = 0;
			break;
		case PANBTN_INVENTORY:
			sbookflag = 0;
			invflag = invflag == 0;
			if (dropGoldFlag) {
				dropGoldFlag = FALSE;
				dropGoldValue = 0;
			}
			break;
		case PANBTN_SPELLBOOK:
			invflag = 0;
			if (dropGoldFlag) {
				dropGoldFlag = FALSE;
				dropGoldValue = 0;
			}
			sbookflag = sbookflag == 0;
			break;
		case PANBTN_SENDMSG:
			if (talkflag)
				control_reset_talk();
			else
				control_type_message();
			break;
		case PANBTN_FRIENDLY:
			FriendlyMode = FriendlyMode == 0;
			break;
		}
	}

	if (gamemenuOff)
		gamemenu_off();
}
// 484368: using guessed type int FriendlyMode;
// 4B8960: using guessed type int talkflag;
// 4B8968: using guessed type int sbookflag;
// 646D00: using guessed type char qtextflag;

void FreeControlPan()
{
	MemFreeDbg(pBtmBuff);
	MemFreeDbg(pManaBuff);
	MemFreeDbg(pLifeBuff);
	MemFreeDbg(pPanelText);
	MemFreeDbg(pChrPanel);
	MemFreeDbg(pSpellCels);
	MemFreeDbg(pPanelButtons);
	MemFreeDbg(pMultiBtns);
	MemFreeDbg(pTalkBtns);
	MemFreeDbg(pChrButtons);
	MemFreeDbg(pDurIcons);
	MemFreeDbg(pQLogCel);
	MemFreeDbg(pSpellBkCel);
	MemFreeDbg(pSBkBtnCel);
	MemFreeDbg(pSBkIconCels);
	MemFreeDbg(pGBoxBuff);
}

int control_WriteStringToBuffer(char *str)
{
	signed int v1;    // edx
	unsigned char v2; // al

	v1 = 0;
	do {
		v2 = *str;
		if (!*str)
			return 1;
		++str;
		v1 += fontkern[fontframe[gbFontTransTbl[v2]]];
	} while (v1 < 125);
	return 0;
}

void DrawInfoBox()
{
	int v0;         // ecx
	int v1;         // eax
	int v2;         // eax
	int v3;         // esi
	char *v4;       // eax
	const char *v5; // eax
	signed int v7;  // edi
	signed int v8;  // ebp
	int v9;         // esi
	char *v10;      // ebx

	DrawPanelBox(177, 62, 288, 60, 241, 558);
	v0 = trigflag_3;
	v1 = spselflag;
	if (!panelflag && !trigflag_3 && pcursinvitem == -1) {
		if (spselflag) {
		LABEL_32:
			infoclr = COL_WHITE;
			goto LABEL_33;
		}
		infostr[0] = 0;
		infoclr = COL_WHITE;
		ClearPanel();
	}
	if (v1 || v0)
		goto LABEL_32;
	if (pcurs < CURSOR_FIRSTITEM) {
		if (pcursitem != -1)
			GetItemStr(pcursitem);
		if (pcursobj != -1)
			GetObjectStr(pcursobj);
		if (pcursmonst != -1) {
			if (leveltype != DTYPE_TOWN) {
				infoclr = COL_WHITE;
				strcpy(infostr, monster[pcursmonst].mName);
				ClearPanel();
				if (monster[pcursmonst]._uniqtype) {
					infoclr = COL_GOLD;
					PrintUniqueHistory();
				} else {
					PrintMonstHistory(monster[pcursmonst].MType->mtype);
				}
			} else {
				strcpy(infostr, towner[pcursmonst]._tName);
			}
		}
		if (pcursplr != -1) {
			infoclr = COL_GOLD;
			strcpy(infostr, plr[pcursplr]._pName);
			ClearPanel();
			sprintf(tempstr, "Level : %i", plr[pcursplr]._pLevel);
			AddPanelString(tempstr, 1);
			sprintf(tempstr, "Hit Points %i of %i", plr[pcursplr]._pHitPoints >> 6, plr[pcursplr]._pMaxHP >> 6);
			AddPanelString(tempstr, 1);
		}
	} else {
		v2 = myplr;
		if (plr[myplr].HoldItem._itype == ITYPE_GOLD) {
			v3 = plr[v2].HoldItem._ivalue;
			v4 = get_pieces_str(plr[v2].HoldItem._ivalue);
			sprintf(infostr, "%i gold %s", v3, v4);
		} else if (plr[v2].HoldItem._iStatFlag) {
			if (plr[v2].HoldItem._iIdentified)
				v5 = plr[v2].HoldItem._iIName;
			else
				v5 = plr[v2].HoldItem._iName;
			strcpy(infostr, v5);
			if (plr[myplr].HoldItem._iMagical == ITEM_QUALITY_MAGIC)
				infoclr = COL_BLUE;
			if (plr[myplr].HoldItem._iMagical == ITEM_QUALITY_UNIQUE)
				infoclr = COL_GOLD;
		} else {
			ClearPanel();
			AddPanelString("Requirements not met", 1);
			pinfoflag = TRUE;
		}
	}
LABEL_33:
	if ((infostr[0] || pnumlines) && !talkflag) {
		v7 = 0;
		v8 = 1;
		if (infostr[0]) {
			control_print_info_str(0, infostr, 1, pnumlines);
			v7 = 1;
			v8 = 0;
		}
		v9 = 0;
		if (pnumlines > 0) {
			v10 = panelstr;
			do {
				control_print_info_str(v9 + v7, v10, pstrjust[v9], pnumlines - v8);
				++v9;
				v10 += 64;
			} while (v9 < pnumlines);
		}
	}
}
// 4B883C: using guessed type int infoclr;
// 4B8960: using guessed type int talkflag;
// 4B8B84: using guessed type int panelflag;
// 4B8C98: using guessed type int spselflag;
// 4B8CB8: using guessed type char pcursinvitem;
// 4B8CC0: using guessed type char pcursitem;
// 4B8CC1: using guessed type char pcursobj;
// 4B8CC2: using guessed type char pcursplr;

void control_print_info_str(int y, char *str, BOOLEAN center, int lines)
{
	int v4;            // edi
	char *v5;          // ebx
	unsigned char v6;  // cl
	signed int v7;     // eax
	char *v8;          // esi
	int v9;            // eax
	unsigned char v10; // esi
	unsigned char v11; // al
	int width;         // [esp+18h] [ebp+Ch]

	v4 = 0;
	v5 = str;
	width = lineoffset[y + 4 * lines + lines];
	if (center == 1) {
		v6 = *str;
		v7 = 0;
		v8 = str;
		if (!*str)
			goto LABEL_14;
		do {
			++v8;
			v7 += fontkern[fontframe[gbFontTransTbl[v6]]] + 2;
			v6 = *v8;
		} while (*v8);
		if (v7 < 288)
		LABEL_14:
			v4 = (288 - v7) >> 1;
		width += v4;
	}
	while (1) {
		v11 = *v5;
		if (!*v5)
			break;
		++v5;
		v9 = gbFontTransTbl[v11];
		_LOBYTE(v9) = fontframe[v9];
		v10 = (unsigned char)v9;
		v4 += fontkern[(unsigned char)v9] + 2;
		if ((_BYTE)v9) {
			if (v4 < 288) {
				CPrintString(width, v10, infoclr);
			}
		}
		width += fontkern[v10] + 2;
	}
}
// 4B883C: using guessed type int infoclr;

void PrintGameStr(int x, int y, char *str, int color)
{
	char *v4;         // edi
	int v5;           // esi
	unsigned char i;  // al
	unsigned char v7; // bl

	v4 = str;
	v5 = PitchTbl[y + 160] + x + 64;
	for (i = *str; *v4; i = *v4) {
		++v4;
		v7 = fontframe[gbFontTransTbl[i]];
		if (v7)
			CPrintString(v5, v7, color);
		v5 += fontkern[v7] + 1;
	}
}

void DrawChr()
{
	int v1;      // ecx
	int v2;      // ecx
	int v3;      // eax
	int v4;      // eax
	BOOLEAN v5;  // zf
	int v6;      // eax
	int v7;      // edi
	int v8;      // edi
	char v9;     // al
	char v10;    // al
	char v11;    // al
	int v12;     // ecx
	int v13;     // eax
	int v14;     // ecx
	int v15;     // eax
	int v16;     // ecx
	int v17;     // eax
	int v18;     // ecx
	int v19;     // eax
	int *v20;    // edi
	int v21;     // edi
	int v22;     // edi
	int v23;     // ecx
	int v24;     // eax
	int v25;     // ecx
	int v26;     // ecx
	char a4[64]; // [esp+Ch] [ebp-50h]
	int v28;     // [esp+4Ch] [ebp-10h]
	int v29;     // [esp+50h] [ebp-Ch]
	int v30;     // [esp+54h] [ebp-8h]
	char a5[4];  // [esp+58h] [ebp-4h]

	CelDecodeOnly(64, 511, (BYTE *)pChrPanel, 1, 320);
	ADD_PlrStringXY(20, 32, 151, plr[myplr]._pName, 0);
	if (plr[myplr]._pClass == PC_WARRIOR) {
		ADD_PlrStringXY(168, 32, 299, "Warrior", 0);
	} else if (plr[myplr]._pClass == PC_ROGUE) {
		ADD_PlrStringXY(168, 32, 299, "Rogue", 0); /* should use ClassStrTbl ? */
	} else if (plr[myplr]._pClass == PC_SORCERER) {
		ADD_PlrStringXY(168, 32, 299, "Sorceror", 0);
	}
	sprintf(a4, "%i", plr[myplr]._pLevel);
	ADD_PlrStringXY(66, 69, 109, a4, 0);
	sprintf(a4, "%li", plr[myplr]._pExperience);
	ADD_PlrStringXY(216, 69, 300, a4, 0);
	if (plr[myplr]._pLevel == 50) {
		strcpy(a4, "None");
		a5[0] = 3;
	} else {
		sprintf(a4, "%li", plr[myplr]._pNextExper);
		a5[0] = 0;
	}
	ADD_PlrStringXY(216, 97, 300, a4, a5[0]);
	sprintf(a4, "%i", plr[myplr]._pGold);
	ADD_PlrStringXY(216, 146, 300, a4, 0);
	a5[0] = 0;
	v29 = plr[myplr]._pIBonusAC;
	if (v29 > 0)
		a5[0] = 1;
	if (v29 < 0)
		a5[0] = 2;
	sprintf(a4, "%i", v29 + plr[myplr]._pIAC + plr[myplr]._pDexterity / 5);
	ADD_PlrStringXY(258, 183, 301, a4, a5[0]);
	a5[0] = 0;
	v1 = plr[myplr]._pIBonusToHit;
	if (v1 > 0)
		a5[0] = 1;
	if (v1 < 0)
		a5[0] = 2;
	sprintf(a4, "%i%%", (plr[myplr]._pDexterity >> 1) + v1 + 50);
	ADD_PlrStringXY(258, 211, 301, a4, a5[0]);
	a5[0] = 0;
	v2 = myplr;
	v3 = plr[myplr]._pIBonusDam;
	if (v3 > 0)
		a5[0] = 1;
	if (v3 < 0)
		a5[0] = 2;
	v30 = plr[v2]._pIMinDam;
	v30 += plr[v2]._pIBonusDamMod + v30 * v3 / 100;
	v4 = plr[v2]._pDamageMod;
	v5 = plr[v2].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW;
	v29 = plr[v2]._pDamageMod;
	if (v5 && plr[v2]._pClass != PC_ROGUE)
		v4 >>= 1;
	v30 += v4;
	v6 = plr[v2]._pIBonusDam;
	v28 = plr[v2]._pIMaxDam;
	v7 = plr[v2]._pIBonusDamMod + v28 * v6 / 100 + v28;
	if (plr[v2].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_BOW || plr[v2]._pClass == PC_ROGUE)
		v8 = v29 + v7;
	else
		v8 = (v29 >> 1) + v7;
	sprintf(a4, "%i-%i", v30, v8);
	if (v30 >= 100 || v8 >= 100)
		MY_PlrStringXY(254, 239, 305, a4, a5[0], -1);
	else
		MY_PlrStringXY(258, 239, 301, a4, a5[0], 0);
	v9 = plr[myplr]._pMagResist;
	a5[0] = v9 != 0;
	if (v9 < 75) {
		sprintf(a4, "%i%%", v9);
	} else {
		a5[0] = 3;
		sprintf(a4, "MAX");
	}
	ADD_PlrStringXY(257, 276, 300, a4, a5[0]);
	v10 = plr[myplr]._pFireResist;
	a5[0] = v10 != 0;
	if (v10 < 75) {
		sprintf(a4, "%i%%", v10);
	} else {
		a5[0] = 3;
		sprintf(a4, "MAX");
	}
	ADD_PlrStringXY(257, 304, 300, a4, a5[0]);
	v11 = plr[myplr]._pLghtResist;
	a5[0] = v11 != 0;
	if (v11 < 75) {
		sprintf(a4, "%i%%", v11);
	} else {
		a5[0] = 3;
		sprintf(a4, "MAX");
	}
	ADD_PlrStringXY(257, 332, 300, a4, a5[0]);
	a5[0] = 0;
	sprintf(a4, "%i", plr[myplr]._pBaseStr);
	if (MaxStats[plr[myplr]._pClass][ATTRIB_STR] == plr[myplr]._pBaseStr)
		a5[0] = 3;
	ADD_PlrStringXY(95, 155, 126, a4, a5[0]);
	a5[0] = 0;
	sprintf(a4, "%i", plr[myplr]._pBaseMag);
	if (MaxStats[plr[myplr]._pClass][ATTRIB_MAG] == plr[myplr]._pBaseMag)
		a5[0] = 3;
	ADD_PlrStringXY(95, 183, 126, a4, a5[0]);
	a5[0] = 0;
	sprintf(a4, "%i", plr[myplr]._pBaseDex);
	if (MaxStats[plr[myplr]._pClass][ATTRIB_DEX] == plr[myplr]._pBaseDex)
		a5[0] = 3;
	ADD_PlrStringXY(95, 211, 126, a4, a5[0]);
	a5[0] = 0;
	sprintf(a4, "%i", plr[myplr]._pBaseVit);
	if (MaxStats[plr[myplr]._pClass][ATTRIB_VIT] == plr[myplr]._pBaseVit)
		a5[0] = 3;
	ADD_PlrStringXY(95, 239, 126, a4, a5[0]);
	a5[0] = 0;
	v12 = plr[myplr]._pStrength;
	v13 = plr[myplr]._pBaseStr;
	if (v12 > v13)
		a5[0] = 1;
	if (v12 < v13)
		a5[0] = 2;
	sprintf(a4, "%i", v12);
	ADD_PlrStringXY(143, 155, 173, a4, a5[0]);
	a5[0] = 0;
	v14 = plr[myplr]._pMagic;
	v15 = plr[myplr]._pBaseMag;
	if (v14 > v15)
		a5[0] = 1;
	if (v14 < v15)
		a5[0] = 2;
	sprintf(a4, "%i", v14);
	ADD_PlrStringXY(143, 183, 173, a4, a5[0]);
	a5[0] = 0;
	v16 = plr[myplr]._pDexterity;
	v17 = plr[myplr]._pBaseDex;
	if (v16 > v17)
		a5[0] = 1;
	if (v16 < v17)
		a5[0] = 2;
	sprintf(a4, "%i", v16);
	ADD_PlrStringXY(143, 211, 173, a4, a5[0]);
	a5[0] = 0;
	v18 = plr[myplr]._pVitality;
	v19 = plr[myplr]._pBaseVit;
	if (v18 > v19)
		a5[0] = 1;
	if (v18 < v19)
		a5[0] = 2;
	sprintf(a4, "%i", v18);
	ADD_PlrStringXY(143, 239, 173, a4, a5[0]);
	v20 = &plr[myplr]._pStatPts;
	if (*v20 > 0) {
		v20 = &plr[myplr]._pStatPts;
		if (CalcStatDiff(myplr) < *v20) {
			v20 = &plr[myplr]._pStatPts;
			*v20 = CalcStatDiff(myplr);
		}
	}
	v21 = *v20;
	if (v21 > 0) {
		sprintf(a4, "%i", v21);
		ADD_PlrStringXY(95, 266, 126, a4, 2);
		v22 = plr[myplr]._pClass;
		if (plr[myplr]._pBaseStr < MaxStats[v22][ATTRIB_STR])
			CelDecodeOnly(201, 319, (BYTE *)pChrButtons, chrbtn[0] + 2, 41);
		if (plr[myplr]._pBaseMag < MaxStats[v22][ATTRIB_MAG])
			CelDecodeOnly(201, 347, (BYTE *)pChrButtons, chrbtn[1] + 4, 41);
		if (plr[myplr]._pBaseDex < MaxStats[v22][ATTRIB_DEX])
			CelDecodeOnly(201, 376, (BYTE *)pChrButtons, chrbtn[2] + 6, 41);
		if (plr[myplr]._pBaseVit < MaxStats[v22][ATTRIB_VIT])
			CelDecodeOnly(201, 404, (BYTE *)pChrButtons, chrbtn[3] + 8, 41);
	}
	v23 = plr[myplr]._pMaxHP;
	a5[0] = v23 > plr[myplr]._pMaxHPBase;
	sprintf(a4, "%i", v23 >> 6);
	ADD_PlrStringXY(95, 304, 126, a4, a5[0]);
	v24 = plr[myplr]._pHitPoints;
	if (v24 != plr[myplr]._pMaxHP)
		a5[0] = 2;
	sprintf(a4, "%i", v24 >> 6);
	ADD_PlrStringXY(143, 304, 174, a4, a5[0]);
	v25 = plr[myplr]._pMaxMana;
	a5[0] = v25 > plr[myplr]._pMaxManaBase;
	sprintf(a4, "%i", v25 >> 6);
	ADD_PlrStringXY(95, 332, 126, a4, a5[0]);
	v26 = plr[myplr]._pMana;
	if (v26 != plr[myplr]._pMaxMana)
		a5[0] = 2;
	sprintf(a4, "%i", v26 >> 6);
	ADD_PlrStringXY(143, 332, 174, a4, a5[0]);
}

void ADD_PlrStringXY(int x, int y, int width, char *pszStr, char col)
{
	int v5;            // eax
	char *v6;          // edx
	unsigned char v7;  // al
	int v8;            // esi
	int v9;            // edi
	int v10;           // ecx
	unsigned char v11; // bl
	unsigned char v12; // al
	int v13;           // ebx
	int widtha;        // [esp+Ch] [ebp-4h]
	int widthb;        // [esp+Ch] [ebp-4h]

	v5 = PitchTbl[y + 160];
	v6 = pszStr;
	widtha = v5 + x + 64;
	v7 = *pszStr;
	v8 = width - x + 1;
	v9 = 0;
	v10 = 0;
	if (*pszStr) {
		v11 = *pszStr;
		do {
			++v6;
			v10 += fontkern[fontframe[gbFontTransTbl[v11]]] + 1;
			v11 = *v6;
		} while (*v6);
	}
	if (v10 < v8)
		v9 = (v8 - v10) >> 1;
	widthb = v9 + widtha;
	while (v7) {
		++pszStr;
		v12 = fontframe[gbFontTransTbl[v7]];
		v13 = v12;
		v9 += fontkern[v12] + 1;
		if (v12) {
			if (v9 < v8)
				CPrintString(widthb, v12, col);
		}
		widthb += fontkern[v13] + 1;
		v7 = *pszStr;
	}
}

void MY_PlrStringXY(int x, int y, int width, char *pszStr, char col, int base)
{
	char *v6;          // ebx
	unsigned char v7;  // al
	int v8;            // edx
	int v9;            // esi
	char *v10;         // edi
	unsigned char v11; // cl
	unsigned char v12; // al
	int v13;           // edi
	int widtha;        // [esp+Ch] [ebp-4h]
	int widthb;        // [esp+Ch] [ebp-4h]
	int v16;           // [esp+18h] [ebp+8h]

	v6 = pszStr;
	widtha = PitchTbl[y + 160] + x + 64;
	v7 = *pszStr;
	v8 = 0;
	v9 = width - x + 1;
	v16 = 0;
	v10 = pszStr;
	if (*pszStr) {
		v11 = *pszStr;
		do {
			++v10;
			v8 += base + fontkern[fontframe[gbFontTransTbl[v11]]];
			v11 = *v10;
		} while (*v10);
	}
	if (v8 < v9)
		v16 = (v9 - v8) >> 1;
	widthb = v16 + widtha;
	while (v7) {
		++v6;
		v12 = fontframe[gbFontTransTbl[v7]];
		v13 = v12;
		v16 += base + fontkern[v12];
		if (v12) {
			if (v16 < v9)
				CPrintString(widthb, v12, col);
		}
		widthb += base + fontkern[v13];
		v7 = *v6;
	}
}

void CheckLvlBtn()
{
	if (!lvlbtndown && MouseX >= 40 && MouseX <= 81 && MouseY >= 313 && MouseY <= 335)
		lvlbtndown = 1;
}
// 4B851C: using guessed type int lvlbtndown;

void ReleaseLvlBtn()
{
	if (MouseX >= 40 && MouseX <= 81 && MouseY >= 313 && MouseY <= 335)
		chrflag = 1;
	lvlbtndown = 0;
}
// 4B851C: using guessed type int lvlbtndown;

void DrawLevelUpIcon()
{
	int v0; // esi

	if (!stextflag) {
		v0 = (lvlbtndown != 0) + 2;
		ADD_PlrStringXY(0, 303, 120, "Level Up", 0);
		CelDecodeOnly(104, 495, (BYTE *)pChrButtons, v0, 41);
	}
}
// 4B851C: using guessed type int lvlbtndown;
// 6AA705: using guessed type char stextflag;

void CheckChrBtns()
{
	int pc, i;

	if (!chrbtnactive && plr[myplr]._pStatPts) {
		pc = plr[myplr]._pClass;
		for (i = 0; i < 4; i++) {
			switch (i) {
			case ATTRIB_STR:
				if (plr[myplr]._pBaseStr >= MaxStats[pc][ATTRIB_STR])
					continue;
				break;
			case ATTRIB_MAG:
				if (plr[myplr]._pBaseMag >= MaxStats[pc][ATTRIB_MAG])
					continue;
				break;
			case ATTRIB_DEX:
				if (plr[myplr]._pBaseDex >= MaxStats[pc][ATTRIB_DEX])
					continue;
				break;
			case ATTRIB_VIT:
				if (plr[myplr]._pBaseVit >= MaxStats[pc][ATTRIB_VIT])
					continue;
				break;
			default:
				continue;
			}
			if (MouseX >= attribute_inc_rects[i].x
			    && MouseX <= attribute_inc_rects[i].x + attribute_inc_rects[i].w
			    && MouseY >= attribute_inc_rects[i].y
			    && MouseY <= attribute_inc_rects[i].y + attribute_inc_rects[i].h) {
				chrbtn[i] = TRUE;
				chrbtnactive = TRUE;
			}
		}
	}
}

void ReleaseChrBtns()
{
	int i;

	chrbtnactive = FALSE;
	for (i = 0; i < 4; ++i) {
		if (chrbtn[i]) {
			chrbtn[i] = FALSE;
			if (MouseX >= attribute_inc_rects[i].x
			    && MouseX <= attribute_inc_rects[i].x + attribute_inc_rects[i].w
			    && MouseY >= attribute_inc_rects[i].y
			    && MouseY <= attribute_inc_rects[i].y + attribute_inc_rects[i].h) {
				switch (i) {
				case 0:
					NetSendCmdParam1(TRUE, CMD_ADDSTR, 1);
					plr[myplr]._pStatPts--;
					break;
				case 1:
					NetSendCmdParam1(TRUE, CMD_ADDMAG, 1);
					plr[myplr]._pStatPts--;
					break;
				case 2:
					NetSendCmdParam1(TRUE, CMD_ADDDEX, 1);
					plr[myplr]._pStatPts--;
					break;
				case 3:
					NetSendCmdParam1(TRUE, CMD_ADDVIT, 1);
					plr[myplr]._pStatPts--;
					break;
				}
			}
		}
	}
}

void DrawDurIcon()
{
	int v0;           // edx
	PlayerStruct *v1; // esi
	int v2;           // eax
	int v3;           // eax
	int v4;           // eax

	if (!chrflag && !questlog || !invflag && !sbookflag) {
		v0 = 656;
		if (invflag || sbookflag)
			v0 = 336;
		v1 = &plr[myplr];
		v2 = DrawDurIcon4Item(v1->InvBody, v0, 4);
		v3 = DrawDurIcon4Item(&v1->InvBody[INVLOC_CHEST], v2, 3);
		v4 = DrawDurIcon4Item(&v1->InvBody[INVLOC_HAND_LEFT], v3, 0);
		DrawDurIcon4Item(&v1->InvBody[INVLOC_HAND_RIGHT], v4, 0);
	}
}
// 4B8968: using guessed type int sbookflag;

int DrawDurIcon4Item(ItemStruct *item, int x, int frame)
{
	if (item->_itype == ITYPE_NONE)
		return x;
	if (item->_iDurability > 5)
		return x;
	if (frame == 0) {
		if (item->_iClass == ICLASS_WEAPON) {
			switch (item->_itype) {
			case ITYPE_SWORD:
				frame = 2;
				break;
			case ITYPE_AXE:
				frame = 6;
				break;
			case ITYPE_BOW:
				frame = 7;
				break;
			case ITYPE_MACE:
				frame = 5;
				break;
			case ITYPE_STAFF:
				frame = 8;
				break;
			default:
				break;
			}
		} else {
			frame = 1;
		}
	}
	if (item->_iDurability > 2)
		frame += 8;
	CelDecodeOnly(x, 495, (BYTE *)pDurIcons, frame, 32);
	return x - 40;
}

void RedBack()
{
	int idx;

	idx = light4flag ? 1536 : 4608;

	/// ASSERT: assert(gpBuffer);

#ifdef USE_ASM
	if (leveltype != DTYPE_HELL) {
		__asm {
			mov		edi, gpBuffer
			add		edi, SCREENXY(0, 0)
			mov		ebx, pLightTbl
			add		ebx, idx
			mov		edx, 352
		lx_label1:
			mov		ecx, 640
		lx_label2:
			mov		al, [edi]
			xlat
			stosb
			loop	lx_label2
			add		edi, 768 - 640
			dec		edx
			jnz		lx_label1
		}
	} else {
		__asm {
			mov		edi, gpBuffer
			add		edi, SCREENXY(0, 0)
			mov		ebx, pLightTbl
			add		ebx, idx
			mov		edx, 352
		l4_label1:
			mov		ecx, 640
		l4_label2:
			mov		al, [edi]
			cmp		al, 32
			jb		l4_label3
			xlat
		l4_label3:
			stosb
			loop	l4_label2
			add		edi, 768 - 640
			dec		edx
			jnz		l4_label1
		}
	}
#else
	int w, h;
	BYTE *dst, *tbl;

	if (leveltype != DTYPE_HELL) {
		dst = &gpBuffer[SCREENXY(0, 0)];
		tbl = &pLightTbl[idx];
		for (h = 352; h; h--, dst += 768 - 640) {
			for (w = 640; w; w--) {
				*dst = tbl[*dst];
				dst++;
			}
		}
	} else {
		dst = &gpBuffer[SCREENXY(0, 0)];
		tbl = &pLightTbl[idx];
		for (h = 352; h; h--, dst += 768 - 640) {
			for (w = 640; w; w--) {
				if (*dst >= 32)
					*dst = tbl[*dst];
				dst++;
			}
		}
	}
#endif
}
// 525728: using guessed type int light4flag;

char GetSBookTrans(int ii, BOOL townok)
{
	char st;

	st = RSPLTYPE_SPELL;
	if(plr[myplr]._pISpells & (__int64)1 << (ii - 1)) {
		st = RSPLTYPE_CHARGES;
	}
	if(plr[myplr]._pAblSpells & 1 << (ii - 1)) { /// BUGFIX: missing (__int64)
		st = RSPLTYPE_SKILL;
	}
	if(st == RSPLTYPE_SPELL) {
		if(!CheckSpell(myplr, ii, RSPLTYPE_SPELL, TRUE)) {
			st = RSPLTYPE_INVALID;
		}
		if((char)(plr[myplr]._pSplLvl[ii] + plr[myplr]._pISplLvlAdd) <= 0) {
			st = RSPLTYPE_INVALID;
		}
	}
	if(townok && currlevel == 0 && st != RSPLTYPE_INVALID && !spelldata[ii].sTownSpell) {
		st = RSPLTYPE_INVALID;
	}

	return st;
}

void DrawSpellBook()
{
	int i, sn, mana, lvl, yp, min, max;
	char st;
	unsigned __int64 spl;

	CelDecodeOnly(384, 511, (BYTE *)pSpellBkCel, 1, 320);
	CelDecodeOnly(76 * sbooktab + 391, 508, (BYTE *)pSBkBtnCel, sbooktab + 1, 76);

	spl = plr[myplr]._pMemSpells | plr[myplr]._pISpells | plr[myplr]._pAblSpells;

	yp = 215;
	for(i = 1; i < 8; i++) {
		sn = SpellPages[sbooktab][i - 1];
		if(sn != -1 && spl & (__int64)1 << (sn - 1)) {
			st = GetSBookTrans(sn, TRUE);
			SetSpellTrans(st);
			DrawSpellCel(395, yp, (BYTE *)pSBkIconCels, SpellITbl[sn], 37);
			if(sn == plr[myplr]._pRSpell && st == plr[myplr]._pRSplType) {
				SetSpellTrans(RSPLTYPE_SKILL);
				DrawSpellCel(395, yp, (BYTE *)pSBkIconCels, 43, 37);
			}
			PrintSBookStr(10, yp - 23, FALSE, spelldata[sn].sNameText, COL_WHITE);
			switch(GetSBookTrans(sn, FALSE)) {
			case RSPLTYPE_SKILL:
				strcpy(tempstr, "Skill");
				break;
			case RSPLTYPE_CHARGES:
				sprintf(tempstr, "Staff (%i charges)", plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges);
				break;
			default:
				mana = GetManaAmount(myplr, sn) >> 6;
				GetDamageAmt(sn, &min, &max);
				if(min != -1) {
					sprintf(tempstr, "Mana: %i  Dam: %i - %i", mana, min, max);
				} else {
					sprintf(tempstr, "Mana: %i   Dam: n/a", mana);
				}
				if(sn == SPL_BONESPIRIT) {
					sprintf(tempstr, "Mana: %i  Dam: 1/3 tgt hp", mana);
				}
				PrintSBookStr(10, yp - 1, FALSE, tempstr, COL_WHITE);
				lvl = plr[myplr]._pSplLvl[sn] + plr[myplr]._pISplLvlAdd;
				if(lvl < 0) {
					lvl = 0;
				}
				if(lvl == 0) {
					sprintf(tempstr, "Spell Level 0 - Unusable");
				} else {
					sprintf(tempstr, "Spell Level %i", lvl);
				}
				break;
			}
			PrintSBookStr(10, yp - 12, FALSE, tempstr, COL_WHITE);
		}
		yp += 43;
	}
}
// 4B8950: using guessed type int sbooktab;

void PrintSBookStr(int x, int y, BOOL cjustflag, char *pszStr, char col)
{
	char *v5;          // ebx
	signed int v6;     // eax
	int v7;            // edi
	unsigned char v8;  // cl
	char *v9;          // esi
	unsigned char v10; // al
	int v11;           // esi
	unsigned char v12; // al
	int width;         // [esp+Ch] [ebp-4h]

	v5 = pszStr;
	width = PitchTbl[y] + x + 440;
	v6 = 0;
	v7 = 0;
	if (cjustflag) {
		v8 = *pszStr;
		v9 = pszStr;
		if (!*pszStr)
			goto LABEL_14;
		do {
			++v9;
			v6 += fontkern[fontframe[gbFontTransTbl[v8]]] + 1;
			v8 = *v9;
		} while (*v9);
		if (v6 < 222)
		LABEL_14:
			v7 = (222 - v6) >> 1;
		width += v7;
	}
	while (1) {
		v12 = *v5;
		if (!*v5)
			break;
		++v5;
		v10 = fontframe[gbFontTransTbl[v12]];
		v11 = v10;
		v7 += fontkern[v10] + 1;
		if (v10) {
			if (v7 <= 222)
				CPrintString(width, v10, col);
		}
		width += fontkern[v11] + 1;
	}
}

void CheckSBook()
{
	int sn;
	char st;
	unsigned __int64 spl;

	if(MouseX >= 331 && MouseX < 368 && MouseY >= 18 && MouseY < 314) {
		spl = plr[myplr]._pMemSpells | plr[myplr]._pISpells | plr[myplr]._pAblSpells;
		sn = SpellPages[sbooktab][(MouseY - 18) / 43];
		if(sn != -1 && spl & (__int64)1 << (sn - 1)) {
			st = RSPLTYPE_SPELL;
			if(plr[myplr]._pISpells & (__int64)1 << (sn - 1)) {
				st = RSPLTYPE_CHARGES;
			}
			if(plr[myplr]._pAblSpells & (__int64)1 << (sn - 1)) {
				st = RSPLTYPE_SKILL;
			}
			plr[myplr]._pRSpell = sn;
			plr[myplr]._pRSplType = st;
			drawpanflag = 255;
		}
	}
	if(MouseX >= 327 && MouseX < 633 && MouseY >= 320 && MouseY < 349) { /// BUGFIX: change `< 633` to `< 631`
		sbooktab = (MouseX - 327) / 76;
	}
}
// 4B8950: using guessed type int sbooktab;
// 52571C: using guessed type int drawpanflag;

char *get_pieces_str(int nGold)
{
	char *result; // eax

	result = "piece";
	if (nGold != 1)
		result = "pieces";
	return result;
}

void DrawGoldSplit(int amount)
{
	int v1;        // ebp
	char *v2;      // eax
	char v3;       // cl
	signed int i;  // eax
	int screen_x;  // [esp+10h] [ebp-4h]
	int screen_xa; // [esp+10h] [ebp-4h]

	screen_x = 0;
	v1 = amount;
	CelDecodeOnly(415, 338, (BYTE *)pGBoxBuff, 1, 261);
	sprintf(tempstr, "You have %u gold", initialDropGoldValue);
	ADD_PlrStringXY(366, 87, 600, tempstr, 3);
	v2 = get_pieces_str(initialDropGoldValue);
	sprintf(tempstr, "%s.  How many do", v2);
	ADD_PlrStringXY(366, 103, 600, tempstr, 3);
	ADD_PlrStringXY(366, 121, 600, "you want to remove?", 3);
	if (v1 <= 0) {
		screen_xa = 450;
	} else {
		sprintf(tempstr, "%u", v1);
		PrintGameStr(388, 140, tempstr, 0);
		v3 = tempstr[0];
		for (i = 0; i < v3; v3 = tempstr[i]) {
			++i;
			screen_x += fontkern[fontframe[gbFontTransTbl[(unsigned char)v3]]] + 1;
		}
		screen_xa = screen_x + 452;
	}
	CelDecodeOnly(screen_xa, 300, (BYTE *)pCelBuff, nGoldFrame, 12);
	nGoldFrame = (nGoldFrame & 7) + 1;
}

void control_drop_gold(char vkey)
{
	char v1;    // bl
	int v2;     // eax
	int v3;     // eax
	size_t v4;  // esi
	char v6[6]; // [esp+8h] [ebp-8h]

	v1 = vkey;
	if (plr[myplr]._pHitPoints >> 6 <= 0) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
		return;
	}
	memset(v6, 0, sizeof(v6));
	_itoa(dropGoldValue, v6, 10);
	if (v1 != VK_RETURN) {
		if (v1 == VK_ESCAPE) {
			dropGoldFlag = FALSE;
			dropGoldValue = 0;
			return;
		}
		if (v1 == VK_BACK) {
			v6[strlen(v6) - 1] = '\0';
			v2 = atoi(v6);
		} else {
			v3 = v1 - '0';
			if (v3 < 0 || v3 > 9)
				return;
			if (dropGoldValue || atoi(v6) <= initialDropGoldValue) {
				v6[strlen(v6)] = v1;
				if (atoi(v6) > initialDropGoldValue)
					return;
				v4 = strlen(v6);
				if (v4 > strlen(v6))
					return;
			} else {
				v6[0] = v1;
			}
			v2 = atoi(v6);
		}
		dropGoldValue = v2;
		return;
	}
	if (dropGoldValue > 0)
		control_remove_gold(myplr, initialDropGoldIndex);
	dropGoldFlag = FALSE;
}
// 406C40: using guessed type char var_8[8];

void control_remove_gold(int pnum, int gold_index)
{
	int v2;     // edi
	int v3;     // esi
	int v4;     // edx
	_DWORD *v5; // eax
	int v6;     // edx
	_DWORD *v7; // eax
	int v8;     // eax

	v2 = pnum;
	v3 = pnum;
	if (gold_index > 46) {
		v6 = gold_index - 47;
		v7 = (unsigned int *)((char *)&plr[v3].SpdList[v6]._ivalue);
		*v7 -= dropGoldValue;
		if (*v7 <= 0)
			RemoveSpdBarItem(pnum, v6);
		else
			SetSpdbarGoldCurs(pnum, v6);
	} else {
		v4 = gold_index - 7;
		v5 = (unsigned int *)((char *)&plr[v3].InvList[v4]._ivalue);
		*v5 -= dropGoldValue;
		if (*v5 <= 0)
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

void control_set_gold_curs(int pnum)
{
	if (plr[pnum].HoldItem._ivalue >= 2500) {
		plr[pnum].HoldItem._iCurs = ICURS_GOLD_LARGE;
	} else {
		if (plr[pnum].HoldItem._ivalue <= 1000)
			plr[pnum].HoldItem._iCurs = ICURS_GOLD_SMALL;
		else
			plr[pnum].HoldItem._iCurs = ICURS_GOLD_MEDIUM;
	}
	SetCursor_(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
}

void DrawTalkPan()
{
	int v0;        // esi
	signed int v1; // edi
	signed int v2; // esi
	char *v3;      // eax
	int v4;        // esi
	int v5;        // esi
	int v6;        // ebx
	int v7;        // eax
	int a4;        // [esp+4h] [ebp-Ch]
	char *a1;      // [esp+8h] [ebp-8h]
	int v10;       // [esp+Ch] [ebp-4h]

	v0 = 0;
	if (talkflag) {
		DrawPanelBox(175, sgbPlrTalkTbl + 20, 294, 5, 239, 516);
		v1 = 293;
		do {
			DrawPanelBox((v0 >> 1) + 175, sgbPlrTalkTbl + v0 + 25, v1, 1, (v0 >> 1) + 239, v0 + 521);
			++v0;
			--v1;
		} while (v1 > 283);
		DrawPanelBox(185, sgbPlrTalkTbl + 35, 274, 30, 249, 531);
		DrawPanelBox(180, sgbPlrTalkTbl + 65, 284, 5, 244, 561);
		v2 = 0;
		do {
			DrawPanelBox(180, sgbPlrTalkTbl + v2 + 70, v2 + 284, 1, 244, v2 + 566);
			++v2;
		} while (v2 < 10);
		DrawPanelBox(170, sgbPlrTalkTbl + 80, 310, 55, 234, 576);
		v3 = sgszTalkMsg;
		v4 = 0;
		do {
			v3 = control_print_talk_msg(v3, 0, v4, &a4, 0);
			if (!v3)
				goto LABEL_10;
			v4 += 13;
		} while (v4 < 39);
		*v3 = 0;
	LABEL_10:
		CelDecDatOnly((BYTE *)gpBuffer + a4, (BYTE *)pCelBuff, frame, 12);
		v5 = 0;
		a1 = plr[0]._pName;
		v10 = 0;
		frame = (frame & 7) + 1;
		while (v10 == myplr) {
		LABEL_21:
			a1 += 21720;
			++v10;
			if ((INT_PTR)a1 >= (INT_PTR)&plr[4]._pName)
				return;
		}
		if (byte_4B894C[v10]) {
			v6 = 3;
			if (!talkbtndown[v5]) {
			LABEL_18:
				if (*(a1 - 291))
					control_print_talk_msg(a1, 46, 18 * v5 + 60, &a4, v6);
				++v5;
				goto LABEL_21;
			}
			v7 = (v5 != 0) + 3;
		} else {
			v7 = (v5 != 0) + 1;
			v6 = 2;
			if (talkbtndown[v5])
				v7 = (v5 != 0) + 5;
		}
		CelDecodeOnly(236, 18 * v5 + 596, (BYTE *)pTalkBtns, v7, 61);
		goto LABEL_18;
	}
}
// 4B8840: using guessed type int sgbPlrTalkTbl;
// 4B8960: using guessed type int talkflag;

char *control_print_talk_msg(char *msg, int x, int y, int *a4, int just)
{
	int v5;            // edx
	char *v6;          // ebx
	unsigned char v7;  // al
	int v8;            // ecx
	unsigned char v10; // dl
	int v11;           // edi
	int a3;            // [esp+14h] [ebp+8h]

	v5 = x + 264;
	v6 = msg;
	*a4 = v5 + PitchTbl[y + 534];
	v7 = *msg;
	v8 = v5;
	if (!v7)
		return 0;
	while (1) {
		v10 = fontframe[gbFontTransTbl[v7]];
		v11 = v10;
		a3 = v8 + fontkern[v10] + 1;
		if (a3 > 514)
			break;
		++v6;
		if (v10)
			CPrintString(*a4, v10, just);
		*a4 += fontkern[v11] + 1;
		v7 = *v6;
		if (!*v6)
			return 0;
		v8 = a3;
	}
	return v6;
}

int control_check_talk_btn()
{
	int v0;     // ecx
	int result; // eax

	if (!talkflag)
		return 0;
	if (MouseX < 172)
		return 0;
	v0 = MouseY;
	if (MouseY < 421 || MouseX > 233)
		return 0;
	result = 0;
	if (MouseY <= 475) {
		talkbtndown[0] = 0;
		talkbtndown[1] = 0;
		talkbtndown[2] = 0;
		talkbtndown[(v0 - 421) / 18] = 1;
		result = 1;
	}
	return result;
}
// 4B8960: using guessed type int talkflag;

void control_release_talk_btn()
{
	signed int v0; // ecx
	int v1;        // eax
	signed int v2; // ecx

	if (talkflag) {
		v0 = MouseX;
		talkbtndown[0] = 0;
		talkbtndown[1] = 0;
		talkbtndown[2] = 0;
		if (v0 >= 172 && MouseY >= 421 && v0 <= 233 && MouseY <= 475) {
			v1 = (MouseY - 421) / 18;
			v2 = 0;
			do {
				if (v1 == -1)
					break;
				if (v2 != myplr)
					--v1;
				++v2;
			} while (v2 < 4);
			if (v2 <= 4)
				byte_4B894C[v2 - 1] = byte_4B894C[v2 - 1] == 0;
		}
	}
}
// 4B8960: using guessed type int talkflag;

void control_reset_talk_msg()
{
	signed int v1; // ecx

	BOOL v0 = FALSE;
	v1 = 0;
	do {
		if (byte_4B894C[v1])
			v0 |= 1 << v1;
		++v1;
	} while (v1 < 4);
	if (!msgcmd_add_server_cmd_W(sgszTalkMsg))
		NetSendCmdString(v0, sgszTalkMsg);
}

void control_type_message()
{
	if (gbMaxPlayers != 1) {
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

void control_reset_talk()
{
	talkflag = 0;
	sgbPlrTalkTbl = 0;
	drawpanflag = 255;
}
// 4B8840: using guessed type int sgbPlrTalkTbl;
// 4B8960: using guessed type int talkflag;
// 52571C: using guessed type int drawpanflag;

BOOL control_talk_last_key(int vkey)
{
	int result;

	if (gbMaxPlayers == 1)
		return FALSE;

	if (!talkflag)
		return FALSE;

	if ((DWORD)vkey < VK_SPACE)
		return FALSE;

	result = strlen(sgszTalkMsg);
	if (result < 78) {
		sgszTalkMsg[result] = vkey;
		sgszTalkMsg[result + 1] = '\0';
	}
	return TRUE;
}

int control_presskeys(int a1)
{
	signed int v1; // eax
	char v2;       // cl

	if (gbMaxPlayers != 1 && talkflag) {
		switch (a1) {
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
			if (v1 > 0)
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
// 4B8960: using guessed type int talkflag;
// 679660: using guessed type char gbMaxPlayers;

void control_press_enter()
{
	signed int v0; // esi
	char(*v1)[80]; // ebp
	char v2;       // al
	int v3;        // ecx
	char *v4;      // ebp

	if (sgszTalkMsg[0]) {
		control_reset_talk_msg();
		v0 = 0;
		v1 = sgszTalkSave;
		do {
			if (!strcmp((const char *)v1, sgszTalkMsg))
				break;
			++v1;
			++v0;
		} while ((INT_PTR)v1 < (INT_PTR)&sgszTalkSave[8]);
		if (v0 < 8) {
			v2 = sgbNextTalkSave;
			v3 = (sgbNextTalkSave - 1) & 7;
			if (v0 != v3) {
				v4 = sgszTalkSave[v3];
				strcpy(sgszTalkSave[v0], sgszTalkSave[v3]);
				strcpy(v4, sgszTalkMsg);
				v2 = sgbNextTalkSave;
			}
		} else {
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

void control_up_down(char a1)
{
	unsigned char v1; // al
	int v2;           // esi

	v1 = sgbTalkSavePos;
	v2 = 0;
	while (1) {
		v1 = (a1 + v1) & 7;
		sgbTalkSavePos = v1;
		if (sgszTalkSave[v1][0])
			break;
		if (++v2 >= 8)
			return;
	}
	strcpy(sgszTalkMsg, sgszTalkSave[v1]);
}
// 4B84CD: using guessed type char sgbTalkSavePos;

DEVILUTION_END_NAMESPACE
