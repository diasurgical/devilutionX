#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE sgbNextTalkSave;
BYTE sgbTalkSavePos;
BYTE *pDurIcons;
BYTE *pChrButtons;
BOOL drawhpflag;
BOOL dropGoldFlag;
int panbtn[8];
int chrbtn[4];
BYTE *pMultiBtns;
BYTE *pPanelButtons;
BYTE *pChrPanel;
BOOL lvlbtndown;
char sgszTalkSave[8][80];
int dropGoldValue;
BOOL drawmanaflag;
BOOL chrbtnactive;
char sgszTalkMsg[MAX_SEND_STR_LEN];
BYTE *pPanelText;
int nGoldFrame;
BYTE *pLifeBuff;
BYTE *pBtmBuff;
BYTE *pTalkBtns;
int pstrjust[4];
int pnumlines;
BOOL pinfoflag;
BOOL talkbtndown[3];
int pSpell;
BYTE *pManaBuff;
char infoclr;
int sgbPlrTalkTbl; // should be char [4]
BYTE *pGBoxBuff;
BYTE *pSBkBtnCel;
char tempstr[256];
BOOLEAN whisper[MAX_PLRS];
int sbooktab;
int pSplType;
int frame;
int initialDropGoldIndex;
BOOL talkflag;
BYTE *pSBkIconCels;
BOOL sbookflag;
BOOL chrflag;
BOOL drawbtnflag;
BYTE *pSpellBkCel;
char infostr[MAX_PATH];
int numpanbtns;
BYTE *pStatusPanel;
char panelstr[256];
BOOL panelflag;
BYTE SplTransTbl[256];
int initialDropGoldValue;
BYTE *pSpellCels;
BOOL panbtndown;
BYTE *pTalkPanel;
int spselflag;

const BYTE fontframe[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 54, 44, 57, 58, 56, 55, 47, 40, 41, 59, 39, 50, 37, 51, 52,
	36, 27, 28, 29, 30, 31, 32, 33, 34, 35, 48, 49, 60, 38, 61, 53,
	62, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 42, 63, 43, 64, 65,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 40, 66, 41, 67, 0
};
const BYTE fontkern[68] = {
	8, 10, 7, 9, 8, 7, 6, 8, 8, 3,
	3, 8, 6, 11, 9, 10, 6, 9, 9, 6,
	9, 11, 10, 13, 10, 11, 7, 5, 7, 7,
	8, 7, 7, 7, 7, 7, 10, 4, 5, 6,
	3, 3, 4, 3, 6, 6, 3, 3, 3, 3,
	3, 2, 7, 6, 3, 10, 10, 6, 6, 7,
	4, 4, 9, 6, 6, 12, 3, 7
};
const int lineoffset[25] = {
	BUFFER_WIDTH * 594 + 241,
	BUFFER_WIDTH * 32,
	BUFFER_WIDTH * 32,
	BUFFER_WIDTH * 32,
	BUFFER_WIDTH * 32 + 180,
	BUFFER_WIDTH * 582 + 241,
	BUFFER_WIDTH * 606 + 241,
	BUFFER_WIDTH * 32,
	BUFFER_WIDTH * 32,
	BUFFER_WIDTH * 32,
	BUFFER_WIDTH * 576 + 241,
	BUFFER_WIDTH * 594 + 241,
	BUFFER_WIDTH * 612 + 241,
	BUFFER_WIDTH * 32,
	BUFFER_WIDTH * 32,
	BUFFER_WIDTH * 572 + 241,
	BUFFER_WIDTH * 587 + 241,
	BUFFER_WIDTH * 601 + 241,
	BUFFER_WIDTH * 616 + 241,
	BUFFER_WIDTH * 32,
	BUFFER_WIDTH * 570 + 241,
	BUFFER_WIDTH * 582 + 241,
	BUFFER_WIDTH * 594 + 241,
	BUFFER_WIDTH * 606 + 241,
	BUFFER_WIDTH * 617 + 241
};
const BYTE gbFontTransTbl[256] = {
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
		sub		edi, BUFFER_WIDTH
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

	for (; src != end; dst -= BUFFER_WIDTH + w) {
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
		DrawSpellCel(629, 631, pSpellCels, SpellITbl[spl], 56);
	else
		DrawSpellCel(629, 631, pSpellCels, 27, 56);
}

void DrawSpellList()
{
	int i, j, x, y, c, s, t, v, lx, ly, trans;
	unsigned __int64 mask, spl;

	pSpell = -1;
	infostr[0] = '\0';
	x = 636;
	y = 495;
	ClearPanel();
	for (i = 0; i < 4; i++) {
		switch ((spell_type)i) {
		case RSPLTYPE_SKILL:
			SetSpellTrans(RSPLTYPE_SKILL);
			c = 46;
			mask = plr[myplr]._pAblSpells;
			break;
		case RSPLTYPE_SPELL:
			c = 47;
			mask = plr[myplr]._pMemSpells;
			break;
		case RSPLTYPE_SCROLL:
			SetSpellTrans(RSPLTYPE_SCROLL);
			c = 44;
			mask = plr[myplr]._pScrlSpells;
			break;
		case RSPLTYPE_CHARGES:
			SetSpellTrans(RSPLTYPE_CHARGES);
			c = 45;
			mask = plr[myplr]._pISpells;
			break;
		}
		for (spl = 1, j = 1; j < MAX_SPELLS; spl <<= 1, j++) {
			if (!(mask & spl))
				continue;
			if (i == RSPLTYPE_SPELL) {
				s = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[j];
				if (s < 0)
					s = 0;
				if (s > 0)
					trans = RSPLTYPE_SPELL;
				else
					trans = RSPLTYPE_INVALID;
				SetSpellTrans(trans);
			}
			if (currlevel == 0 && !spelldata[j].sTownSpell)
				SetSpellTrans(RSPLTYPE_INVALID);
			DrawSpellCel(x, y, pSpellCels, SpellITbl[j], 56);
			lx = x - 64;
			ly = y - 216;
			if (MouseX >= lx && MouseX < lx + 56 && MouseY >= ly && MouseY < ly + 56) {
				pSpell = j;
				pSplType = i;
				DrawSpellCel(x, y, pSpellCels, c, 56);
				switch (i) {
				case RSPLTYPE_SKILL:
					sprintf(infostr, "%s Skill", spelldata[pSpell].sSkillText);
					break;
				case RSPLTYPE_SPELL:
					sprintf(infostr, "%s Spell", spelldata[pSpell].sNameText);
					if (pSpell == SPL_HBOLT) {
						sprintf(tempstr, "Damages undead only");
						AddPanelString(tempstr, TRUE);
					}
					if (s == 0)
						sprintf(tempstr, "Spell Level 0 - Unusable");
					else
						sprintf(tempstr, "Spell Level %i", s);
					AddPanelString(tempstr, TRUE);
					break;
				case RSPLTYPE_SCROLL:
					sprintf(infostr, "Scroll of %s", spelldata[pSpell].sNameText);
					v = 0;
					for (t = 0; t < plr[myplr]._pNumInv; t++) {
						if (plr[myplr].InvList[t]._itype != ITYPE_NONE
						    && (plr[myplr].InvList[t]._iMiscId == IMISC_SCROLL || plr[myplr].InvList[t]._iMiscId == IMISC_SCROLLT)
						    && plr[myplr].InvList[t]._iSpell == pSpell) {
							v++;
						}
					}
					for (t = 0; t < MAXBELTITEMS; t++) {
						if (plr[myplr].SpdList[t]._itype != ITYPE_NONE
						    && (plr[myplr].SpdList[t]._iMiscId == IMISC_SCROLL || plr[myplr].SpdList[t]._iMiscId == IMISC_SCROLLT)
						    && plr[myplr].SpdList[t]._iSpell == pSpell) {
							v++;
						}
					}
					if (v == 1)
						strcpy(tempstr, "1 Scroll");
					else
						sprintf(tempstr, "%i Scrolls", v);
					AddPanelString(tempstr, TRUE);
					break;
				case RSPLTYPE_CHARGES:
					sprintf(infostr, "Staff of %s", spelldata[pSpell].sNameText);
					if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges == 1)
						strcpy(tempstr, "1 Charge");
					else
						sprintf(tempstr, "%i Charges", plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges);
					AddPanelString(tempstr, TRUE);
					break;
				}
				for (t = 0; t < 4; t++) {
					if (plr[myplr]._pSplHotKey[t] == pSpell && plr[myplr]._pSplTHotKey[t] == pSplType) {
						DrawSpellCel(x, y, pSpellCels, t + 48, 56);
						sprintf(tempstr, "Spell Hot Key #F%i", t + 5);
						AddPanelString(tempstr, TRUE);
					}
				}
			}
			x -= 56;
			if (x == 20) {
				y -= 56;
				x = 636;
			}
		}
		if (mask != 0 && x != 636)
			x -= 56;
		if (x == 20) {
			y -= 56;
			x = 636;
		}
	}
}

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
		sub		edi, BUFFER_WIDTH + 13
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
		sub		edi, BUFFER_WIDTH + 13
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
		sub		edi, BUFFER_WIDTH + 13
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
		sub		edi, BUFFER_WIDTH + 13
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
		for (; src != end; dst -= BUFFER_WIDTH + 13) {
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
		for (; src != end; dst -= BUFFER_WIDTH + 13) {
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
		for (; src != end; dst -= BUFFER_WIDTH + 13) {
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
		for (; src != end; dst -= BUFFER_WIDTH + 13) {
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

	nSrcOff = x + PANEL_WIDTH * y;
	nDstOff = sx + BUFFER_WIDTH * sy;

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
		add		esi, PANEL_WIDTH
		sub		esi, ebx
		add		edi, BUFFER_WIDTH
		sub		edi, ebx
		dec		edx
		jnz		label1
	}
#else
	int wdt, hgt;
	BYTE *src, *dst;

	src = &pBtmBuff[nSrcOff];
	dst = &gpBuffer[nDstOff];

	for (hgt = h; hgt; hgt--, src += PANEL_WIDTH - w, dst += BUFFER_WIDTH - w) {
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
	nDstOff = c + BUFFER_WIDTH * r;
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
		add		edi, BUFFER_WIDTH - 88
		dec		edx
		jnz		label1
	}
#else
	BYTE *src, *dst;

	src = &pCelBuff[nSrcOff];
	dst = &gpBuffer[nDstOff];

	for (; w; w--, src += 88, dst += BUFFER_WIDTH)
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
		add		edi, BUFFER_WIDTH - 59
		dec		edx
		jnz		label1
	}
#else
	int wdt, hgt;
	BYTE *src, *dst;

	src = &pCelBuff[nSrcOff];
	dst = &pBuff[nDstOff];

	for (hgt = h; hgt; hgt--, src += w - 59, dst += BUFFER_WIDTH - 59) {
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

	DrawFlask(pLifeBuff, 88, 277, gpBuffer, BUFFER_WIDTH * 499 + 173, filled);
	if (filled != 13)
		DrawFlask(pBtmBuff, PANEL_WIDTH, PANEL_WIDTH * filled + 2029, gpBuffer, BUFFER_WIDTH * filled + BUFFER_WIDTH * 499 + 173, 13 - filled);
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

	DrawFlask(pManaBuff, 88, 277, gpBuffer, BUFFER_WIDTH * 499 + 173 + 366, filled);
	if (filled != 13)
		DrawFlask(pBtmBuff, PANEL_WIDTH, PANEL_WIDTH * filled + 2029 + 366, gpBuffer, BUFFER_WIDTH * filled + BUFFER_WIDTH * 499 + 173 + 366, 13 - filled);
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
		pBtmBuff = DiabloAllocPtr((PANEL_HEIGHT + 16) * PANEL_WIDTH);
		memset(pBtmBuff, 0, (PANEL_HEIGHT + 16) * PANEL_WIDTH);
	} else {
		pBtmBuff = DiabloAllocPtr((PANEL_HEIGHT + 16) * 2 * PANEL_WIDTH);
		memset(pBtmBuff, 0, (PANEL_HEIGHT + 16) * 2 * PANEL_WIDTH);
	}
	pManaBuff = DiabloAllocPtr(88 * 88);
	memset(pManaBuff, 0, 88 * 88);
	pLifeBuff = DiabloAllocPtr(88 * 88);
	memset(pLifeBuff, 0, 88 * 88);
	pPanelText = LoadFileInMem("CtrlPan\\SmalText.CEL", NULL);
	pChrPanel = LoadFileInMem("Data\\Char.CEL", NULL);
	pSpellCels = LoadFileInMem("CtrlPan\\SpelIcon.CEL", NULL);
	SetSpellTrans(RSPLTYPE_SKILL);
	pStatusPanel = LoadFileInMem("CtrlPan\\Panel8.CEL", NULL);
	CelDecodeRect(pBtmBuff, 0, (PANEL_HEIGHT + 16) - 1, PANEL_WIDTH, pStatusPanel, 1, PANEL_WIDTH);
	MemFreeDbg(pStatusPanel);
	pStatusPanel = LoadFileInMem("CtrlPan\\P8Bulbs.CEL", NULL);
	CelDecodeRect(pLifeBuff, 0, 87, 88, pStatusPanel, 1, 88);
	CelDecodeRect(pManaBuff, 0, 87, 88, pStatusPanel, 2, 88);
	MemFreeDbg(pStatusPanel);
	talkflag = FALSE;
	if (gbMaxPlayers != 1) {
		pTalkPanel = LoadFileInMem("CtrlPan\\TalkPanl.CEL", NULL);
		CelDecodeRect(pBtmBuff, 0, (PANEL_HEIGHT + 16) * 2 - 1, PANEL_WIDTH, pTalkPanel, 1, PANEL_WIDTH);
		MemFreeDbg(pTalkPanel);
		pMultiBtns = LoadFileInMem("CtrlPan\\P8But2.CEL", NULL);
		pTalkBtns = LoadFileInMem("CtrlPan\\TalkButt.CEL", NULL);
		sgbPlrTalkTbl = 0;
		sgszTalkMsg[0] = 0;
		for (i = 0; i < MAX_PLRS; i++)
			whisper[i] = TRUE;
		for (i = 0; i < sizeof(talkbtndown) / sizeof(talkbtndown[0]); i++)
			talkbtndown[i] = FALSE;
	}
	panelflag = FALSE;
	lvlbtndown = FALSE;
	pPanelButtons = LoadFileInMem("CtrlPan\\Panel8bu.CEL", NULL);
	for (i = 0; i < sizeof(panbtn) / sizeof(panbtn[0]); i++)
		panbtn[i] = 0;
	panbtndown = 0;
	if (gbMaxPlayers == 1)
		numpanbtns = 6;
	else
		numpanbtns = 8;
	pChrButtons = LoadFileInMem("Data\\CharBut.CEL", NULL);
	for (i = 0; i < sizeof(chrbtn) / sizeof(chrbtn[0]); i++)
		chrbtn[i] = 0;
	chrbtnactive = FALSE;
	pDurIcons = LoadFileInMem("Items\\DurIcons.CEL", NULL);
	strcpy(infostr, "");
	ClearPanel();
	drawhpflag = TRUE;
	drawmanaflag = TRUE;
	chrflag = FALSE;
	spselflag = 0;
	pSpellBkCel = LoadFileInMem("Data\\SpellBk.CEL", NULL);
	pSBkBtnCel = LoadFileInMem("Data\\SpellBkB.CEL", NULL);
	pSBkIconCels = LoadFileInMem("Data\\SpellI2.CEL", NULL);
	sbooktab = 0;
	sbookflag = FALSE;
	if (plr[myplr]._pClass == PC_WARRIOR) {
		SpellPages[0][0] = SPL_REPAIR;
	} else if (plr[myplr]._pClass == PC_ROGUE) {
		SpellPages[0][0] = SPL_DISARM;
	} else if (plr[myplr]._pClass == PC_SORCERER) {
		SpellPages[0][0] = SPL_RECHARGE;
	}
	pQLogCel = LoadFileInMem("Data\\Quest.CEL", NULL);
	pGBoxBuff = LoadFileInMem("CtrlPan\\Golddrop.cel", NULL);
	dropGoldFlag = FALSE;
	dropGoldValue = 0;
	initialDropGoldValue = 0;
	initialDropGoldIndex = 0;
	nGoldFrame = 1;
}

void ClearCtrlPan()
{
	DrawPanelBox(0, sgbPlrTalkTbl + 16, PANEL_WIDTH, PANEL_HEIGHT, 64, 512);
	DrawInfoBox();
}

void DrawCtrlPan()
{
	int i;

	for (i = 0; i < 6; i++) {
		if (!panbtn[i])
			DrawPanelBox(PanBtnPos[i][0], PanBtnPos[i][1] - 336, 71, 20, PanBtnPos[i][0] + 64, PanBtnPos[i][1] + 160);
		else
			CelDecodeOnly(PanBtnPos[i][0] + 64, PanBtnPos[i][1] + 178, pPanelButtons, i + 1, 71);
	}
	if (numpanbtns == 8) {
		CelDecodeOnly(151, 634, pMultiBtns, panbtn[6] + 1, 33);
		if (FriendlyMode)
			CelDecodeOnly(591, 634, pMultiBtns, panbtn[7] + 3, 33);
		else
			CelDecodeOnly(591, 634, pMultiBtns, panbtn[7] + 5, 33);
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
	if (MouseX >= PanBtnPos[3][0]
	    && MouseX <= PanBtnPos[3][0] + PanBtnPos[3][2]
	    && MouseY >= PanBtnPos[3][1]
	    && MouseY <= PanBtnPos[3][1] + PanBtnPos[3][3]) {
		control_set_button_down(3);
	}
	if (MouseX >= PanBtnPos[6][0]
	    && MouseX <= PanBtnPos[6][0] + PanBtnPos[6][2]
	    && MouseY >= PanBtnPos[6][1]
	    && MouseY <= PanBtnPos[6][1] + PanBtnPos[6][3]) {
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

void CheckPanelInfo()
{
	int i, c, v, s;

	panelflag = FALSE;
	ClearPanel();
	for (i = 0; i < numpanbtns; i++) {
		if (MouseX >= PanBtnPos[i][0]
		    && MouseX <= PanBtnPos[i][0] + PanBtnPos[i][2]
		    && MouseY >= PanBtnPos[i][1]
		    && MouseY <= PanBtnPos[i][1] + PanBtnPos[i][3]) {
			if (i != 7) {
				strcpy(infostr, PanBtnStr[i]);
			} else {
				if (FriendlyMode)
					strcpy(infostr, "Player friendly");
				else
					strcpy(infostr, "Player attack");
			}
			if (PanBtnHotKey[i]) {
				sprintf(tempstr, "Hotkey : %s", PanBtnHotKey[i]);
				AddPanelString(tempstr, TRUE);
			}
			infoclr = COL_WHITE;
			panelflag = TRUE;
			pinfoflag = TRUE;
		}
	}
	if (!spselflag && MouseX >= 565 && MouseX < 621 && MouseY >= 416 && MouseY < 472) {
		strcpy(infostr, "Select current spell button");
		infoclr = COL_WHITE;
		panelflag = TRUE;
		pinfoflag = TRUE;
		strcpy(tempstr, "Hotkey : 's'");
		AddPanelString(tempstr, TRUE);
		v = plr[myplr]._pRSpell;
		if (v != -1) {
			switch (plr[myplr]._pRSplType) {
			case RSPLTYPE_SKILL:
				sprintf(tempstr, "%s Skill", spelldata[v].sSkillText);
				AddPanelString(tempstr, TRUE);
				break;
			case RSPLTYPE_SPELL:
				sprintf(tempstr, "%s Spell", spelldata[v].sNameText);
				AddPanelString(tempstr, TRUE);
				c = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[v];
				if (c < 0)
					c = 0;
				if (!c)
					sprintf(tempstr, "Spell Level 0 - Unusable");
				else
					sprintf(tempstr, "Spell Level %i", c);
				AddPanelString(tempstr, TRUE);
				break;
			case RSPLTYPE_SCROLL:
				sprintf(tempstr, "Scroll of %s", spelldata[v].sNameText);
				AddPanelString(tempstr, TRUE);
				s = 0;
				for (i = 0; i < plr[myplr]._pNumInv; i++) {
					if (plr[myplr].InvList[i]._itype != -1
					    && (plr[myplr].InvList[i]._iMiscId == IMISC_SCROLL || plr[myplr].InvList[i]._iMiscId == IMISC_SCROLLT)
					    && plr[myplr].InvList[i]._iSpell == v) {
						s++;
					}
				}
				for (i = 0; i < MAXBELTITEMS; i++) {
					if (plr[myplr].SpdList[i]._itype != -1
					    && (plr[myplr].SpdList[i]._iMiscId == IMISC_SCROLL || plr[myplr].SpdList[i]._iMiscId == IMISC_SCROLLT)
					    && plr[myplr].SpdList[i]._iSpell == v) {
						s++;
					}
				}
				if (s == 1)
					strcpy(tempstr, "1 Scroll");
				else
					sprintf(tempstr, "%i Scrolls", s);
				AddPanelString(tempstr, TRUE);
				break;
			case RSPLTYPE_CHARGES:
				sprintf(tempstr, "Staff of %s", spelldata[v].sNameText);
				AddPanelString(tempstr, TRUE);
				if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges == 1)
					strcpy(tempstr, "1 Charge");
				else
					sprintf(tempstr, "%i Charges", plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges);
				AddPanelString(tempstr, TRUE);
				break;
			}
		}
	}
	if (MouseX > 190 && MouseX < 437 && MouseY > 356 && MouseY < 385)
		pcursinvitem = CheckInvHLight();
}

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
			chrflag = !chrflag;
			break;
		case PANBTN_QLOG:
			chrflag = FALSE;
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
			sbookflag = FALSE;
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
			sbookflag = !sbookflag;
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

BOOL control_WriteStringToBuffer(BYTE *str)
{
	int k;
	BYTE ichar;

	k = 0;
	while (*str) {
		ichar = gbFontTransTbl[*str];
		str++;
		k += fontkern[fontframe[ichar]];
		if (k >= 125)
			return FALSE;
	}

	return TRUE;
}

void DrawInfoBox()
{
	int nGold;

	DrawPanelBox(177, 62, 288, 60, 241, 558);
	if (!panelflag && !trigflag && pcursinvitem == -1 && !spselflag) {
		infostr[0] = '\0';
		infoclr = COL_WHITE;
		ClearPanel();
	}
	if (spselflag || trigflag) {
		infoclr = COL_WHITE;
	} else if (pcurs >= CURSOR_FIRSTITEM) {
		if (plr[myplr].HoldItem._itype == ITYPE_GOLD) {
			nGold = plr[myplr].HoldItem._ivalue;
			sprintf(infostr, "%i gold %s", nGold, get_pieces_str(plr[myplr].HoldItem._ivalue));
		} else if (!plr[myplr].HoldItem._iStatFlag) {
			ClearPanel();
			AddPanelString("Requirements not met", TRUE);
			pinfoflag = TRUE;
		} else {
			if (plr[myplr].HoldItem._iIdentified)
				strcpy(infostr, plr[myplr].HoldItem._iIName);
			else
				strcpy(infostr, plr[myplr].HoldItem._iName);
			if (plr[myplr].HoldItem._iMagical == ITEM_QUALITY_MAGIC)
				infoclr = COL_BLUE;
			if (plr[myplr].HoldItem._iMagical == ITEM_QUALITY_UNIQUE)
				infoclr = COL_GOLD;
		}
	} else {
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
			AddPanelString(tempstr, TRUE);
			sprintf(tempstr, "Hit Points %i of %i", plr[pcursplr]._pHitPoints >> 6, plr[pcursplr]._pMaxHP >> 6);
			AddPanelString(tempstr, TRUE);
		}
	}
	if (infostr[0] || pnumlines)
		control_draw_info_str();
}

void control_draw_info_str()
{
	int yo, lo, i;

	if (!talkflag) {
		yo = 0;
		lo = 1;
		if (infostr[0]) {
			control_print_info_str(0, infostr, 1, pnumlines);
			yo = 1;
			lo = 0;
		}

		for (i = 0; i < pnumlines; i++) {
			control_print_info_str(i + yo, &panelstr[64 * i], pstrjust[i], pnumlines - lo);
		}
	}
}

void control_print_info_str(int y, char *str, BOOL center, int lines)
{
	BYTE c;
	char *tmp;
	int screen_x, line, nOffset;

	line = 0;
	nOffset = lineoffset[y + 4 * lines + lines];
	if (center == 1) {
		screen_x = 0;
		tmp = str;
		while (*tmp) {
			c = gbFontTransTbl[(BYTE)*tmp++];
			screen_x += fontkern[fontframe[c]] + 1;
		}
		if (screen_x < 288)
			line = (288 - screen_x) >> 1;
		nOffset += line;
	}
	while (*str) {
		c = gbFontTransTbl[(BYTE)*str++];
		c = fontframe[c];
		line += fontkern[c] + 2;
		if (c) {
			if (line < 288) {
				CPrintString(nOffset, c, infoclr);
			}
		}
		nOffset += fontkern[c] + 2;
	}
}

void PrintGameStr(int x, int y, char *str, int color)
{
	BYTE c;
	int off;
	off = PitchTbl[y + SCREEN_Y] + x + 64;
	while (*str) {
		c = gbFontTransTbl[(BYTE)*str++];
		c = fontframe[c];
		if (c)
			CPrintString(off, c, color);
		off += fontkern[c] + 1;
	}
}

void DrawChr()
{
	char col;
	char chrstr[64];
	int pc, mindam, maxdam;

	CelDecodeOnly(64, 511, pChrPanel, 1, 320);
	ADD_PlrStringXY(20, 32, 151, plr[myplr]._pName, COL_WHITE);

	if (plr[myplr]._pClass == PC_WARRIOR) {
		ADD_PlrStringXY(168, 32, 299, "Warrior", COL_WHITE);
	} else if (plr[myplr]._pClass == PC_ROGUE) {
		ADD_PlrStringXY(168, 32, 299, "Rogue", COL_WHITE); /* should use ClassStrTbl ? */
	} else if (plr[myplr]._pClass == PC_SORCERER) {
		ADD_PlrStringXY(168, 32, 299, "Sorceror", COL_WHITE);
	}

	sprintf(chrstr, "%i", plr[myplr]._pLevel);
	ADD_PlrStringXY(66, 69, 109, chrstr, COL_WHITE);

	sprintf(chrstr, "%li", plr[myplr]._pExperience);
	ADD_PlrStringXY(216, 69, 300, chrstr, COL_WHITE);

	if (plr[myplr]._pLevel == MAXCHARLEVEL - 1) {
		strcpy(chrstr, "None");
		col = COL_GOLD;
	} else {
		sprintf(chrstr, "%li", plr[myplr]._pNextExper);
		col = COL_WHITE;
	}
	ADD_PlrStringXY(216, 97, 300, chrstr, col);

	sprintf(chrstr, "%i", plr[myplr]._pGold);
	ADD_PlrStringXY(216, 146, 300, chrstr, COL_WHITE);

	col = COL_WHITE;
	if (plr[myplr]._pIBonusAC > 0)
		col = COL_BLUE;
	if (plr[myplr]._pIBonusAC < 0)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pIBonusAC + plr[myplr]._pIAC + plr[myplr]._pDexterity / 5);
	ADD_PlrStringXY(258, 183, 301, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pIBonusToHit > 0)
		col = COL_BLUE;
	if (plr[myplr]._pIBonusToHit < 0)
		col = COL_RED;
	sprintf(chrstr, "%i%%", (plr[myplr]._pDexterity >> 1) + plr[myplr]._pIBonusToHit + 50);
	ADD_PlrStringXY(258, 211, 301, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pIBonusDam > 0)
		col = COL_BLUE;
	if (plr[myplr]._pIBonusDam < 0)
		col = COL_RED;
	mindam = plr[myplr]._pIMinDam;
	mindam += plr[myplr]._pIBonusDam * mindam / 100;
	mindam += plr[myplr]._pIBonusDamMod;
	if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW) {
		if (plr[myplr]._pClass == PC_ROGUE)
			mindam += plr[myplr]._pDamageMod;
		else
			mindam += plr[myplr]._pDamageMod >> 1;
	} else {
		mindam += plr[myplr]._pDamageMod;
	}
	maxdam = plr[myplr]._pIMaxDam;
	maxdam += plr[myplr]._pIBonusDam * maxdam / 100;
	maxdam += plr[myplr]._pIBonusDamMod;
	if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW) {
		if (plr[myplr]._pClass == PC_ROGUE)
			maxdam += plr[myplr]._pDamageMod;
		else
			maxdam += plr[myplr]._pDamageMod >> 1;
	} else {
		maxdam += plr[myplr]._pDamageMod;
	}
	sprintf(chrstr, "%i-%i", mindam, maxdam);
	if (mindam >= 100 || maxdam >= 100)
		MY_PlrStringXY(254, 239, 305, chrstr, col, -1);
	else
		MY_PlrStringXY(258, 239, 301, chrstr, col, 0);

	col = plr[myplr]._pMagResist == 0 ? COL_WHITE : COL_BLUE;
	if (plr[myplr]._pMagResist < 75) {
		sprintf(chrstr, "%i%%", plr[myplr]._pMagResist);
	} else {
		col = COL_GOLD;
		sprintf(chrstr, "MAX");
	}
	ADD_PlrStringXY(257, 276, 300, chrstr, col);

	col = plr[myplr]._pFireResist == 0 ? COL_WHITE : COL_BLUE;
	if (plr[myplr]._pFireResist < 75) {
		sprintf(chrstr, "%i%%", plr[myplr]._pFireResist);
	} else {
		col = COL_GOLD;
		sprintf(chrstr, "MAX");
	}
	ADD_PlrStringXY(257, 304, 300, chrstr, col);

	col = plr[myplr]._pLghtResist == 0 ? COL_WHITE : COL_BLUE;
	if (plr[myplr]._pLghtResist < 75) {
		sprintf(chrstr, "%i%%", plr[myplr]._pLghtResist);
	} else {
		col = COL_GOLD;
		sprintf(chrstr, "MAX");
	}
	ADD_PlrStringXY(257, 332, 300, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseStr);
	if (MaxStats[plr[myplr]._pClass][ATTRIB_STR] == plr[myplr]._pBaseStr)
		col = COL_GOLD;
	ADD_PlrStringXY(95, 155, 126, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseMag);
	if (MaxStats[plr[myplr]._pClass][ATTRIB_MAG] == plr[myplr]._pBaseMag)
		col = COL_GOLD;
	ADD_PlrStringXY(95, 183, 126, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseDex);
	if (MaxStats[plr[myplr]._pClass][ATTRIB_DEX] == plr[myplr]._pBaseDex)
		col = COL_GOLD;
	ADD_PlrStringXY(95, 211, 126, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseVit);
	if (MaxStats[plr[myplr]._pClass][ATTRIB_VIT] == plr[myplr]._pBaseVit)
		col = COL_GOLD;
	ADD_PlrStringXY(95, 239, 126, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pStrength > plr[myplr]._pBaseStr)
		col = COL_BLUE;
	if (plr[myplr]._pStrength < plr[myplr]._pBaseStr)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pStrength);
	ADD_PlrStringXY(143, 155, 173, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pMagic > plr[myplr]._pBaseMag)
		col = COL_BLUE;
	if (plr[myplr]._pMagic < plr[myplr]._pBaseMag)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pMagic);
	ADD_PlrStringXY(143, 183, 173, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pDexterity > plr[myplr]._pBaseDex)
		col = COL_BLUE;
	if (plr[myplr]._pDexterity < plr[myplr]._pBaseDex)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pDexterity);
	ADD_PlrStringXY(143, 211, 173, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pVitality > plr[myplr]._pBaseVit)
		col = COL_BLUE;
	if (plr[myplr]._pVitality < plr[myplr]._pBaseVit)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pVitality);
	ADD_PlrStringXY(143, 239, 173, chrstr, col);

	if (plr[myplr]._pStatPts > 0) {
		if (CalcStatDiff(myplr) < plr[myplr]._pStatPts) {
			plr[myplr]._pStatPts = CalcStatDiff(myplr);
		}
	}
	if (plr[myplr]._pStatPts > 0) {
		sprintf(chrstr, "%i", plr[myplr]._pStatPts);
		ADD_PlrStringXY(95, 266, 126, chrstr, COL_RED);
		pc = plr[myplr]._pClass;
		if (plr[myplr]._pBaseStr < MaxStats[pc][ATTRIB_STR])
			CelDecodeOnly(201, 319, pChrButtons, chrbtn[ATTRIB_STR] + 2, 41);
		if (plr[myplr]._pBaseMag < MaxStats[pc][ATTRIB_MAG])
			CelDecodeOnly(201, 347, pChrButtons, chrbtn[ATTRIB_MAG] + 4, 41);
		if (plr[myplr]._pBaseDex < MaxStats[pc][ATTRIB_DEX])
			CelDecodeOnly(201, 376, pChrButtons, chrbtn[ATTRIB_DEX] + 6, 41);
		if (plr[myplr]._pBaseVit < MaxStats[pc][ATTRIB_VIT])
			CelDecodeOnly(201, 404, pChrButtons, chrbtn[ATTRIB_VIT] + 8, 41);
	}

	col = plr[myplr]._pMaxHP <= plr[myplr]._pMaxHPBase ? COL_WHITE : COL_BLUE;
	sprintf(chrstr, "%i", plr[myplr]._pMaxHP >> 6);
	ADD_PlrStringXY(95, 304, 126, chrstr, col);
	if (plr[myplr]._pHitPoints != plr[myplr]._pMaxHP)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pHitPoints >> 6);
	ADD_PlrStringXY(143, 304, 174, chrstr, col);

	col = plr[myplr]._pMaxMana <= plr[myplr]._pMaxManaBase ? COL_WHITE : COL_BLUE;
	sprintf(chrstr, "%i", plr[myplr]._pMaxMana >> 6);
	ADD_PlrStringXY(95, 332, 126, chrstr, col);
	if (plr[myplr]._pMana != plr[myplr]._pMaxMana)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pMana >> 6);
	ADD_PlrStringXY(143, 332, 174, chrstr, col);
}

/**
 * @brief Identical to MY_PlrStringXY(x, y, width, pszStr, col, 1)
 */
void ADD_PlrStringXY(int x, int y, int width, char *pszStr, char col)
{
	BYTE c;
	char *tmp;
	int nOffset, screen_x, line, widthOffset;

	nOffset = x + PitchTbl[y + SCREEN_Y] + 64;
	widthOffset = width - x + 1;
	line = 0;
	screen_x = 0;
	tmp = pszStr;
	while (*tmp) {
		c = gbFontTransTbl[(BYTE)*tmp++];
		screen_x += fontkern[fontframe[c]] + 1;
	}
	if (screen_x < widthOffset)
		line = (widthOffset - screen_x) >> 1;
	nOffset += line;
	while (*pszStr) {
		c = gbFontTransTbl[(BYTE)*pszStr++];
		c = fontframe[c];
		line += fontkern[c] + 1;
		if (c) {
			if (line < widthOffset)
				CPrintString(nOffset, c, col);
		}
		nOffset += fontkern[c] + 1;
	}
}

void MY_PlrStringXY(int x, int y, int width, char *pszStr, char col, int base)
{
	BYTE c;
	char *tmp;
	int nOffset, screen_x, line, widthOffset;

	nOffset = x + PitchTbl[y + SCREEN_Y] + 64;
	widthOffset = width - x + 1;
	line = 0;
	screen_x = 0;
	tmp = pszStr;
	while (*tmp) {
		c = gbFontTransTbl[(BYTE)*tmp++];
		screen_x += fontkern[fontframe[c]] + base;
	}
	if (screen_x < widthOffset)
		line = (widthOffset - screen_x) >> 1;
	nOffset += line;
	while (*pszStr) {
		c = gbFontTransTbl[(BYTE)*pszStr++];
		c = fontframe[c];
		line += fontkern[c] + base;
		if (c) {
			if (line < widthOffset)
				CPrintString(nOffset, c, col);
		}
		nOffset += fontkern[c] + base;
	}
}

void CheckLvlBtn()
{
	if (!lvlbtndown && MouseX >= 40 && MouseX <= 81 && MouseY >= 313 && MouseY <= 335)
		lvlbtndown = TRUE;
}

void ReleaseLvlBtn()
{
	if (MouseX >= 40 && MouseX <= 81 && MouseY >= 313 && MouseY <= 335)
		chrflag = TRUE;
	lvlbtndown = FALSE;
}

void DrawLevelUpIcon()
{
	int nCel;

	if (!stextflag) {
		nCel = lvlbtndown ? 3 : 2;
		ADD_PlrStringXY(0, 303, 120, "Level Up", COL_WHITE);
		CelDecodeOnly(104, 495, pChrButtons, nCel, 41);
	}
}

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
				chrbtn[i] = 1;
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
			chrbtn[i] = 0;
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
	PlayerStruct *p;
	int x1, x2, x3, x4;

	if (!chrflag && !questlog || !invflag && !sbookflag) {
		x1 = 656;
		if (invflag || sbookflag)
			x1 = 336;
		p = &plr[myplr];
		x2 = DrawDurIcon4Item(&p->InvBody[INVLOC_HEAD], x1, 4);
		x3 = DrawDurIcon4Item(&p->InvBody[INVLOC_CHEST], x2, 3);
		x4 = DrawDurIcon4Item(&p->InvBody[INVLOC_HAND_LEFT], x3, 0);
		DrawDurIcon4Item(&p->InvBody[INVLOC_HAND_RIGHT], x4, 0);
	}
}

int DrawDurIcon4Item(ItemStruct *pItem, int x, int c)
{
	if (pItem->_itype == ITYPE_NONE)
		return x;
	if (pItem->_iDurability > 5)
		return x;
	if (c == 0) {
		if (pItem->_iClass == ICLASS_WEAPON) {
			switch (pItem->_itype) {
			case ITYPE_SWORD:
				c = 2;
				break;
			case ITYPE_AXE:
				c = 6;
				break;
			case ITYPE_BOW:
				c = 7;
				break;
			case ITYPE_MACE:
				c = 5;
				break;
			case ITYPE_STAFF:
				c = 8;
				break;
			}
		} else {
			c = 1;
		}
	}
	if (pItem->_iDurability > 2)
		c += 8;
	CelDecodeOnly(x, 495, pDurIcons, c, 32);
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
			mov		edx, PANEL_TOP
		lx_label1:
			mov		ecx, SCREEN_WIDTH
		lx_label2:
			mov		al, [edi]
			xlat
			stosb
			loop	lx_label2
			add		edi, BUFFER_WIDTH - SCREEN_WIDTH
			dec		edx
			jnz		lx_label1
		}
	} else {
		__asm {
			mov		edi, gpBuffer
			add		edi, SCREENXY(0, 0)
			mov		ebx, pLightTbl
			add		ebx, idx
			mov		edx, PANEL_TOP
		l4_label1:
			mov		ecx, SCREEN_WIDTH
		l4_label2:
			mov		al, [edi]
			cmp		al, 32
			jb		l4_label3
			xlat
		l4_label3:
			stosb
			loop	l4_label2
			add		edi, BUFFER_WIDTH - SCREEN_WIDTH
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
		for (h = PANEL_TOP; h; h--, dst += BUFFER_WIDTH - SCREEN_WIDTH) {
			for (w = SCREEN_WIDTH; w; w--) {
				*dst = tbl[*dst];
				dst++;
			}
		}
	} else {
		dst = &gpBuffer[SCREENXY(0, 0)];
		tbl = &pLightTbl[idx];
		for (h = PANEL_TOP; h; h--, dst += BUFFER_WIDTH - SCREEN_WIDTH) {
			for (w = SCREEN_WIDTH; w; w--) {
				if (*dst >= 32)
					*dst = tbl[*dst];
				dst++;
			}
		}
	}
#endif
}

char GetSBookTrans(int ii, BOOL townok)
{
	char st;

	st = RSPLTYPE_SPELL;
	if (plr[myplr]._pISpells & (__int64)1 << (ii - 1)) {
		st = RSPLTYPE_CHARGES;
	}
	if (plr[myplr]._pAblSpells & 1 << (ii - 1)) { /// BUGFIX: missing (__int64)
		st = RSPLTYPE_SKILL;
	}
	if (st == RSPLTYPE_SPELL) {
		if (!CheckSpell(myplr, ii, RSPLTYPE_SPELL, TRUE)) {
			st = RSPLTYPE_INVALID;
		}
		if ((char)(plr[myplr]._pSplLvl[ii] + plr[myplr]._pISplLvlAdd) <= 0) {
			st = RSPLTYPE_INVALID;
		}
	}
	if (townok && currlevel == 0 && st != RSPLTYPE_INVALID && !spelldata[ii].sTownSpell) {
		st = RSPLTYPE_INVALID;
	}

	return st;
}

void DrawSpellBook()
{
	int i, sn, mana, lvl, yp, min, max;
	char st;
	unsigned __int64 spl;

	CelDecodeOnly(384, 511, pSpellBkCel, 1, 320);
	CelDecodeOnly(76 * sbooktab + 391, 508, pSBkBtnCel, sbooktab + 1, 76);

	spl = plr[myplr]._pMemSpells | plr[myplr]._pISpells | plr[myplr]._pAblSpells;

	yp = 215;
	for (i = 1; i < 8; i++) {
		sn = SpellPages[sbooktab][i - 1];
		if (sn != -1 && spl & (__int64)1 << (sn - 1)) {
			st = GetSBookTrans(sn, TRUE);
			SetSpellTrans(st);
			DrawSpellCel(395, yp, pSBkIconCels, SpellITbl[sn], 37);
			if (sn == plr[myplr]._pRSpell && st == plr[myplr]._pRSplType) {
				SetSpellTrans(RSPLTYPE_SKILL);
				DrawSpellCel(395, yp, pSBkIconCels, 43, 37);
			}
			PrintSBookStr(10, yp - 23, FALSE, spelldata[sn].sNameText, COL_WHITE);
			switch (GetSBookTrans(sn, FALSE)) {
			case RSPLTYPE_SKILL:
				strcpy(tempstr, "Skill");
				break;
			case RSPLTYPE_CHARGES:
				sprintf(tempstr, "Staff (%i charges)", plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges);
				break;
			default:
				mana = GetManaAmount(myplr, sn) >> 6;
				GetDamageAmt(sn, &min, &max);
				if (min != -1) {
					sprintf(tempstr, "Mana: %i  Dam: %i - %i", mana, min, max);
				} else {
					sprintf(tempstr, "Mana: %i   Dam: n/a", mana);
				}
				if (sn == SPL_BONESPIRIT) {
					sprintf(tempstr, "Mana: %i  Dam: 1/3 tgt hp", mana);
				}
				PrintSBookStr(10, yp - 1, FALSE, tempstr, COL_WHITE);
				lvl = plr[myplr]._pSplLvl[sn] + plr[myplr]._pISplLvlAdd;
				if (lvl < 0) {
					lvl = 0;
				}
				if (lvl == 0) {
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

void PrintSBookStr(int x, int y, BOOL cjustflag, char *pszStr, char col)
{
	BYTE c;
	char *tmp;
	int screen_x, line, width;

	width = PitchTbl[y] + x + 440;
	line = 0;
	if (cjustflag) {
		screen_x = 0;
		tmp = pszStr;
		while (*tmp) {
			c = gbFontTransTbl[(BYTE)*tmp++];
			screen_x += fontkern[fontframe[c]] + 1;
		}
		if (screen_x < 222)
			line = (222 - screen_x) >> 1;
		width += line;
	}
	while (*pszStr) {
		c = gbFontTransTbl[(BYTE)*pszStr++];
		c = fontframe[c];
		line += fontkern[c] + 1;
		if (c) {
			if (line <= 222)
				CPrintString(width, c, col);
		}
		width += fontkern[c] + 1;
	}
}

void CheckSBook()
{
	int sn;
	char st;
	unsigned __int64 spl;

	if (MouseX >= 331 && MouseX < 368 && MouseY >= 18 && MouseY < 314) {
		spl = plr[myplr]._pMemSpells | plr[myplr]._pISpells | plr[myplr]._pAblSpells;
		sn = SpellPages[sbooktab][(MouseY - 18) / 43];
		if (sn != -1 && spl & (__int64)1 << (sn - 1)) {
			st = RSPLTYPE_SPELL;
			if (plr[myplr]._pISpells & (__int64)1 << (sn - 1)) {
				st = RSPLTYPE_CHARGES;
			}
			if (plr[myplr]._pAblSpells & (__int64)1 << (sn - 1)) {
				st = RSPLTYPE_SKILL;
			}
			plr[myplr]._pRSpell = sn;
			plr[myplr]._pRSplType = st;
			drawpanflag = 255;
		}
	}
	if (MouseX >= 327 && MouseX < 633 && MouseY >= 320 && MouseY < 349) { /// BUGFIX: change `< 633` to `< 631`
		sbooktab = (MouseX - 327) / 76;
	}
}

char *get_pieces_str(int nGold)
{
	char *result;

	result = "piece";
	if (nGold != 1)
		result = "pieces";
	return result;
}

void DrawGoldSplit(int amount)
{
	int screen_x, i;

	screen_x = 0;
	CelDecodeOnly(415, 338, pGBoxBuff, 1, 261);
	sprintf(tempstr, "You have %u gold", initialDropGoldValue);
	ADD_PlrStringXY(366, 87, 600, tempstr, COL_GOLD);
	sprintf(tempstr, "%s.  How many do", get_pieces_str(initialDropGoldValue));
	ADD_PlrStringXY(366, 103, 600, tempstr, COL_GOLD);
	ADD_PlrStringXY(366, 121, 600, "you want to remove?", COL_GOLD);
	if (amount > 0) {
		sprintf(tempstr, "%u", amount);
		PrintGameStr(388, 140, tempstr, 0);
		for (i = 0; i < tempstr[i]; i++) {
			screen_x += fontkern[fontframe[gbFontTransTbl[(BYTE)tempstr[i]]]] + 1;
		}
		screen_x += 452;
	} else {
		screen_x = 450;
	}
	CelDecodeOnly(screen_x, 300, pSPentSpn2Cels, nGoldFrame, 12);
	nGoldFrame = (nGoldFrame & 7) + 1;
}

void control_drop_gold(char vkey)
{
	char input[6];

	if (plr[myplr]._pHitPoints >> 6 <= 0) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
		return;
	}

	memset(input, 0, sizeof(input));
	_itoa(dropGoldValue, input, 10);
	if (vkey == VK_RETURN) {
		if (dropGoldValue > 0)
			control_remove_gold(myplr, initialDropGoldIndex);
		dropGoldFlag = 0;
	} else if (vkey == VK_ESCAPE) {
		dropGoldFlag = 0;
		dropGoldValue = 0;
	} else if (vkey == VK_BACK) {
		input[strlen(input) - 1] = '\0';
		dropGoldValue = atoi(input);
	} else if (vkey - '0' >= 0 && vkey - '0' <= 9) {
		if (dropGoldValue || atoi(input) <= initialDropGoldValue) {
			input[strlen(input)] = vkey;
			if (atoi(input) > initialDropGoldValue)
				return;
			if (strlen(input) > strlen(input))
				return;
		} else {
			input[0] = vkey;
		}
		dropGoldValue = atoi(input);
	}
}

void control_remove_gold(int pnum, int gold_index)
{
	int gi;

	if (gold_index <= 46) {
		gi = gold_index - 7;
		plr[pnum].InvList[gi]._ivalue -= dropGoldValue;
		if (plr[pnum].InvList[gi]._ivalue > 0)
			SetGoldCurs(pnum, gi);
		else
			RemoveInvItem(pnum, gi);
	} else {
		gi = gold_index - 47;
		plr[pnum].SpdList[gi]._ivalue -= dropGoldValue;
		if (plr[pnum].SpdList[gi]._ivalue > 0)
			SetSpdbarGoldCurs(pnum, gi);
		else
			RemoveSpdBarItem(pnum, gi);
	}
	SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
	GetGoldSeed(pnum, &plr[pnum].HoldItem);
	plr[pnum].HoldItem._ivalue = dropGoldValue;
	plr[pnum].HoldItem._iStatFlag = 1;
	control_set_gold_curs(pnum);
	plr[pnum]._pGold = CalculateGold(pnum);
	dropGoldValue = 0;
}

void control_set_gold_curs(int pnum)
{
	if (plr[pnum].HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
		plr[pnum].HoldItem._iCurs = ICURS_GOLD_LARGE;
	else if (plr[pnum].HoldItem._ivalue <= GOLD_SMALL_LIMIT)
		plr[pnum].HoldItem._iCurs = ICURS_GOLD_SMALL;
	else
		plr[pnum].HoldItem._iCurs = ICURS_GOLD_MEDIUM;

	NewCursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
}

void DrawTalkPan()
{	int i, off, talk_btn, color, nCel, x;
	char *msg;

	off = 0;
	if (!talkflag)
		return;

	DrawPanelBox(175, sgbPlrTalkTbl + 20, 294, 5, 239, 516);
	for (i = 293; i > 283; off++, i--) {
		DrawPanelBox((off >> 1) + 175, sgbPlrTalkTbl + off + 25, i, 1, (off >> 1) + 239, off + 521);
	}
	DrawPanelBox(185, sgbPlrTalkTbl + 35, 274, 30, 249, 531);
	DrawPanelBox(180, sgbPlrTalkTbl + 65, 284, 5, 244, 561);
	for (i = 0; i < 10; i++) {
		DrawPanelBox(180, sgbPlrTalkTbl + i + 70, i + 284, 1, 244, i + 566);
	}
	DrawPanelBox(170, sgbPlrTalkTbl + 80, 310, 55, 234, 576);
	msg = sgszTalkMsg;
	for (i = 0; i < 39; i += 13) {
		msg = control_print_talk_msg(msg, 0, i, &x, 0);
		if (!msg)
			break;
	}
	if (msg)
		*msg = '\0';
	CelDecDatOnly(gpBuffer + x, pSPentSpn2Cels, frame, 12);
	talk_btn = 0;
	frame = (frame & 7) + 1;
	for (i = 0; i < 4; i++) {
		if (i == myplr)
			continue;
		if (whisper[i]) {
			color = COL_GOLD;
			if (talkbtndown[talk_btn]) {
				if (talk_btn != 0)
					nCel = 4;
				else
					nCel = 3;
				CelDecodeOnly(236, 596 + 18 * talk_btn, pTalkBtns, nCel, 61);
			}
		} else {
			color = COL_RED;
			if (talk_btn != 0)
				nCel = 2;
			else
				nCel = 1;
			if (talkbtndown[talk_btn])
				nCel += 4;
			CelDecodeOnly(236, 596 + 18 * talk_btn, pTalkBtns, nCel, 61);
		}
		if (plr[i].plractive) {
			control_print_talk_msg(plr[i]._pName, 46, 60 + talk_btn * 18, &x, color);
		}

		talk_btn++;
	}
}

char *control_print_talk_msg(char *msg, int x, int y, int *nOffset, int color)
{
	BYTE c;
	int width;

	x += 264;
	width = x;
	*nOffset = PitchTbl[y + 534] + x;
	while (*msg) {

		c = fontframe[gbFontTransTbl[(BYTE)*msg]];
		width += fontkern[c] + 1;
		if (width > 514)
			return msg;
		msg++;
		if (c) {
			CPrintString(*nOffset, c, color);
		}
		*nOffset += fontkern[c] + 1;
	}
	return NULL;
}

BOOL control_check_talk_btn()
{
	int i;

	if (!talkflag)
		return FALSE;

	if (MouseX < 172)
		return FALSE;
	if (MouseY < 421)
		return FALSE;
	if (MouseX > 233)
		return FALSE;
	if (MouseY > 475)
		return FALSE;

	for (i = 0; i < sizeof(talkbtndown) / sizeof(talkbtndown[0]); i++) {
		talkbtndown[i] = FALSE;
	}

	talkbtndown[(MouseY - 421) / 18] = TRUE;

	return TRUE;
}

void control_release_talk_btn()
{
	int i, p, off;

	if (talkflag) {
		for (i = 0; i < sizeof(talkbtndown) / sizeof(talkbtndown[0]); i++)
			talkbtndown[i] = FALSE;
		if (MouseX >= 172 && MouseY >= 421 && MouseX <= 233 && MouseY <= 475) {
			off = (MouseY - 421) / 18;

			for (p = 0; p < MAX_PLRS && off != -1; p++) {
				if (p != myplr)
					off--;
			}
			if (p <= MAX_PLRS)
				whisper[p - 1] = !whisper[p - 1];
		}
	}
}

void control_reset_talk_msg(char *msg)
{
	int i, pmask;
	pmask = 0;

	for (i = 0; i < MAX_PLRS; i++) {
		if (whisper[i])
			pmask |= 1 << i;
	}

	if (!msgcmd_add_server_cmd_W(sgszTalkMsg))
		NetSendCmdString(pmask, sgszTalkMsg);
}

void control_type_message()
{
	int i;

	if (gbMaxPlayers == 1) {
		return;
	}

	talkflag = TRUE;
	sgszTalkMsg[0] = 0;
	frame = 1;
	for (i = 0; i < 3; i++) {
		talkbtndown[i] = FALSE;
	}
	sgbPlrTalkTbl = PANEL_HEIGHT + 16;
	drawpanflag = 255;
	sgbTalkSavePos = sgbNextTalkSave;
}

void control_reset_talk()
{
	talkflag = FALSE;
	sgbPlrTalkTbl = 0;
	drawpanflag = 255;
}

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

BOOL control_presskeys(int vkey)
{
	int len;
	BOOL ret;

	if (gbMaxPlayers != 1) {
		if (!talkflag) {
			ret = FALSE;
		} else {
			if (vkey == VK_SPACE) {
			} else if (vkey == VK_ESCAPE) {
				control_reset_talk();
			} else if (vkey == VK_RETURN) {
				control_press_enter();
			} else if (vkey == VK_BACK) {
				len = strlen(sgszTalkMsg);
				if (len > 0)
					sgszTalkMsg[len - 1] = '\0';
			} else if (vkey == VK_DOWN) {
				control_up_down(1);
			} else if (vkey == VK_UP) {
				control_up_down(-1);
			} else {
				return FALSE;
			}
			ret = TRUE;
		}
	} else {
		ret = FALSE;
	}
	return ret;
}

void control_press_enter()
{
	int i;
	BYTE talk_save;

	if (sgszTalkMsg[0]) {
		control_reset_talk_msg(sgszTalkMsg);
		for (i = 0; i < 8; i++) {
			if (!strcmp(sgszTalkSave[i], sgszTalkMsg))
				break;
		}
		if (i >= 8) {
			strcpy(sgszTalkSave[sgbNextTalkSave], sgszTalkMsg);
			sgbNextTalkSave = sgbNextTalkSave + 1;
			sgbNextTalkSave &= 7;
		} else {
			talk_save = sgbNextTalkSave - 1;
			talk_save &= 7;
			if (i != talk_save) {
				strcpy(sgszTalkSave[i], sgszTalkSave[talk_save]);
				strcpy(sgszTalkSave[talk_save], sgszTalkMsg);
			}
		}
		sgszTalkMsg[0] = '\0';
		sgbTalkSavePos = sgbNextTalkSave;
	}
	control_reset_talk();
}

void control_up_down(int v)
{
	int i;

	i = 0;
	while (1) {
		sgbTalkSavePos = (v + sgbTalkSavePos) & 7;
		if (sgszTalkSave[sgbTalkSavePos][0])
			break;
		i++;
		if (i >= 8) {
			return;
		}
	}
	strcpy(sgszTalkMsg, sgszTalkSave[sgbTalkSavePos]);
}

DEVILUTION_END_NAMESPACE
