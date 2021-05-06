/**
 * @file control.cpp
 *
 * Implementation of the character and main control panels
 */
#include "control.h"

#include <cstddef>
#include <array>

#include "DiabloUI/diabloui.h"
#include "automap.h"
#include "controls/keymapper.hpp"
#include "cursor.h"
#include "engine/render/cel_render.hpp"
#include "error.h"
#include "gamemenu.h"
#include "init.h"
#include "inv.h"
#include "lighting.h"
#include "minitext.h"
#include "missiles.h"
#include "qol/xpbar.h"
#include "stores.h"
#include "towners.h"
#include "trigs.h"
#include "utils/language.h"

namespace devilution {
namespace {
CelOutputBuffer pBtmBuff;
CelOutputBuffer pLifeBuff;
CelOutputBuffer pManaBuff;
std::optional<CelSprite> pTalkBtns;
std::optional<CelSprite> pDurIcons;
std::optional<CelSprite> pChrButtons;
std::optional<CelSprite> pMultiBtns;
std::optional<CelSprite> pPanelButtons;
std::optional<CelSprite> pChrPanel;
std::optional<CelSprite> pGBoxBuff;
std::optional<CelSprite> pSBkBtnCel;
std::optional<CelSprite> pSBkIconCels;
std::optional<CelSprite> pSpellBkCel;
std::optional<CelSprite> pSpellCels;
} // namespace

BYTE sgbNextTalkSave;
BYTE sgbTalkSavePos;

bool drawhpflag;
bool dropGoldFlag;
bool panbtns[8];
bool chrbtn[4];
bool lvlbtndown;
char sgszTalkSave[8][80];
int dropGoldValue;
bool drawmanaflag;
bool chrbtnactive;
char sgszTalkMsg[MAX_SEND_STR_LEN];
std::optional<CelSprite> pPanelText;
bool pstrjust[4];
int pnumlines;
bool pinfoflag;
bool talkButtonsDown[3];
spell_id pSpell;
text_color infoclr;
int sgbPlrTalkTbl;
char tempstr[256];
bool whisperList[MAX_PLRS];
int sbooktab;
spell_type pSplType;
int initialDropGoldIndex;
bool talkflag;
bool sbookflag;
bool chrflag;
bool drawbtnflag;
char infostr[64];
int numpanbtns;
char panelstr[4][64];
bool panelflag;
uint8_t SplTransTbl[256];
int initialDropGoldValue;
bool panbtndown;
bool spselflag;
extern Keymapper keymapper;
extern std::array<Keymapper::ActionIndex, 4> quickSpellActionIndexes;

/** Map of hero class names */
const char *const ClassStrTbl[] = {
	N_("Warrior"),
	N_("Rogue"),
	N_("Sorcerer"),
	N_("Monk"),
	N_("Bard"),
	N_("Barbarian"),
};

/** Maps from font index to smaltext.cel frame number. */
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

/**
 * Maps from smaltext.cel frame number to character width. Note, the
 * character width may be distinct from the frame width, which is 13 for every
 * smaltext.cel frame.
 */
const BYTE fontkern[68] = {
	8, 10, 7, 9, 8, 7, 6, 8, 8, 3,
	3, 8, 6, 11, 9, 10, 6, 9, 9, 6,
	9, 11, 10, 13, 10, 11, 7, 5, 7, 7,
	8, 7, 7, 7, 7, 7, 10, 4, 5, 6,
	3, 3, 4, 3, 6, 6, 3, 3, 3, 3,
	3, 2, 7, 6, 3, 10, 10, 6, 6, 7,
	4, 4, 9, 6, 6, 12, 3, 7
};
/**
 * Line start position for info box text when displaying 1, 2, 3, 4 and 5 lines respectivly
 */
const int LineOffsets[5][5] = {
	{ 82 },
	{ 70, 94 },
	{ 64, 82, 100 },
	{ 60, 75, 89, 104 },
	{ 58, 70, 82, 94, 105 },
};

/**
 * Maps ASCII character code to font index, as used by the
 * small, medium and large sized fonts; which corresponds to smaltext.cel,
 * medtexts.cel and bigtgold.cel respectively.
 */
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
	'o',  'n',  'o',  'o',  'o',  'o',  'o',  '/',  '0',  'u',  'u',  'u',  'u',  'y',  'b',  'y',
	// clang-format on
};

/* data */

/** Maps from spell_id to spelicon.cel frame number. */
char SpellITbl[] = {
	27,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	28,
	13,
	12,
	18,
	16,
	14,
	18,
	19,
	11,
	20,
	15,
	21,
	23,
	24,
	25,
	22,
	26,
	29,
	37,
	38,
	39,
	42,
	41,
	40,
	10,
	36,
	30,
	51,
	51,
	50,
	46,
	47,
	43,
	45,
	48,
	49,
	44,
	35,
	35,
	35,
	35,
	35,
};

enum panel_button_id {
	PanelButtonCharinfo,
	PanelButtonQlog,
	PanelButtonAutomap,
	PanelButtonMainmenu,
	PanelButtonInventory,
	PanelButtonSpellbook,
	PanelButtonSendmsg,
	PanelButtonFriendly,
};

/** Positions of panel buttons. */
SDL_Rect PanBtnPos[8] = {
	// clang-format off
	{   9,   9, 71, 19 }, // char button
	{   9,  35, 71, 19 }, // quests button
	{   9,  75, 71, 19 }, // map button
	{   9, 101, 71, 19 }, // menu button
	{ 560,   9, 71, 19 }, // inv button
	{ 560,  35, 71, 19 }, // spells button
	{  87,  91, 33, 32 }, // chat button
	{ 527,  91, 33, 32 }, // friendly fire button
	// clang-format on
};
/** Maps from panel_button_id to hotkey name. */
const char *const PanBtnHotKey[8] = { "'c'", "'q'", N_("Tab"), N_("Esc"), "'i'", "'b'", N_("Enter"), nullptr };
/** Maps from panel_button_id to panel button description. */
const char *const PanBtnStr[8] = {
	N_("Character Information"),
	N_("Quests log"),
	N_("Automap"),
	N_("Main Menu"),
	N_("Inventory"),
	N_("Spell book"),
	N_("Send Message"),
	"" // Player attack
};
/** Maps from attribute_id to the rectangle on screen used for attribute increment buttons. */
RECT32 ChrBtnsRect[4] = {
	{ 137, 138, 41, 22 },
	{ 137, 166, 41, 22 },
	{ 137, 195, 41, 22 },
	{ 137, 223, 41, 22 }
};

/** Maps from spellbook page number and position to spell_id. */
spell_id SpellPages[6][7] = {
	{ SPL_NULL, SPL_FIREBOLT, SPL_CBOLT, SPL_HBOLT, SPL_HEAL, SPL_HEALOTHER, SPL_FLAME },
	{ SPL_RESURRECT, SPL_FIREWALL, SPL_TELEKINESIS, SPL_LIGHTNING, SPL_TOWN, SPL_FLASH, SPL_STONE },
	{ SPL_RNDTELEPORT, SPL_MANASHIELD, SPL_ELEMENT, SPL_FIREBALL, SPL_WAVE, SPL_CHAIN, SPL_GUARDIAN },
	{ SPL_NOVA, SPL_GOLEM, SPL_TELEPORT, SPL_APOCA, SPL_BONESPIRIT, SPL_FLARE, SPL_ETHEREALIZE },
	{ SPL_LIGHTWALL, SPL_IMMOLAT, SPL_WARP, SPL_REFLECT, SPL_BERSERK, SPL_FIRERING, SPL_SEARCH },
	{ SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID, SPL_INVALID }
};

#define SPLICONLENGTH 56
#define SPLROWICONLS 10
#define SPLICONLAST (gbIsHellfire ? 52 : 43)

/**
 * Draw spell cell onto the given buffer.
 * @param out Output buffer
 * @param xp Buffer coordinate
 * @param yp Buffer coordinate
 * @param cel The CEL sprite
 * @param nCel Index of the cel frame to draw. 0 based.
 */
static void DrawSpellCel(const CelOutputBuffer &out, int xp, int yp, const CelSprite &cel, int nCel)
{
	CelDrawLightTo(out, xp, yp, cel, nCel, SplTransTbl);
}

void SetSpellTrans(spell_type t)
{
	if (t == RSPLTYPE_SKILL) {
		for (int i = 0; i < 128; i++)
			SplTransTbl[i] = i;
	}
	for (int i = 128; i < 256; i++)
		SplTransTbl[i] = i;
	SplTransTbl[255] = 0;

	switch (t) {
	case RSPLTYPE_SPELL:
		SplTransTbl[PAL8_YELLOW] = PAL16_BLUE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_BLUE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_BLUE + 5;
		for (int i = PAL16_BLUE; i < PAL16_BLUE + 16; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_BLUE + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_BLUE + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_BLUE + i] = i;
		}
		break;
	case RSPLTYPE_SCROLL:
		SplTransTbl[PAL8_YELLOW] = PAL16_BEIGE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_BEIGE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_BEIGE + 5;
		for (int i = PAL16_BEIGE; i < PAL16_BEIGE + 16; i++) {
			SplTransTbl[PAL16_YELLOW - PAL16_BEIGE + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_BEIGE + i] = i;
		}
		break;
	case RSPLTYPE_CHARGES:
		SplTransTbl[PAL8_YELLOW] = PAL16_ORANGE + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_ORANGE + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_ORANGE + 5;
		for (int i = PAL16_ORANGE; i < PAL16_ORANGE + 16; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_ORANGE + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_ORANGE + i] = i;
		}
		break;
	case RSPLTYPE_INVALID:
		SplTransTbl[PAL8_YELLOW] = PAL16_GRAY + 1;
		SplTransTbl[PAL8_YELLOW + 1] = PAL16_GRAY + 3;
		SplTransTbl[PAL8_YELLOW + 2] = PAL16_GRAY + 5;
		for (int i = PAL16_GRAY; i < PAL16_GRAY + 15; i++) {
			SplTransTbl[PAL16_BEIGE - PAL16_GRAY + i] = i;
			SplTransTbl[PAL16_YELLOW - PAL16_GRAY + i] = i;
			SplTransTbl[PAL16_ORANGE - PAL16_GRAY + i] = i;
		}
		SplTransTbl[PAL16_BEIGE + 15] = 0;
		SplTransTbl[PAL16_YELLOW + 15] = 0;
		SplTransTbl[PAL16_ORANGE + 15] = 0;
		break;
	case RSPLTYPE_SKILL:
		break;
	}
}

/**
 * Sets the spell frame to draw and its position then draws it.
 */
static void DrawSpell(const CelOutputBuffer &out)
{
	spell_id spl = plr[myplr]._pRSpell;
	spell_type st = plr[myplr]._pRSplType;

	// BUGFIX: Move the next line into the if statement to avoid OOB (SPL_INVALID is -1) (fixed)
	if (st == RSPLTYPE_SPELL && spl != SPL_INVALID) {
		int tlvl = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[spl];
		if (!CheckSpell(myplr, spl, st, true))
			st = RSPLTYPE_INVALID;
		if (tlvl <= 0)
			st = RSPLTYPE_INVALID;
	}
	if (currlevel == 0 && st != RSPLTYPE_INVALID && !spelldata[spl].sTownSpell)
		st = RSPLTYPE_INVALID;
	if (plr[myplr]._pRSpell < 0)
		st = RSPLTYPE_INVALID;
	SetSpellTrans(st);
	if (spl != SPL_INVALID)
		DrawSpellCel(out, PANEL_X + 565, PANEL_Y + 119, *pSpellCels, SpellITbl[spl]);
	else
		DrawSpellCel(out, PANEL_X + 565, PANEL_Y + 119, *pSpellCels, 27);
}

static void PrintSBookHotkey(CelOutputBuffer out, int x, int y, const std::string &text, text_color col)
{
	x += SPLICONLENGTH;
	y -= SPLICONLENGTH;

	int totalWidth = 0;

	for (const char txtChar : text) {
		auto c = gbFontTransTbl[static_cast<BYTE>(txtChar)];
		c = fontframe[c];

		totalWidth += fontkern[c] + 1;
	}

	PrintGameStr(out, x - totalWidth - 4, y + 17, text.c_str(), col);
}

void DrawSpellList(const CelOutputBuffer &out)
{
	int c;
	int s;
	uint64_t mask;

	pSpell = SPL_INVALID;
	infostr[0] = '\0';
	int x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
	int y = PANEL_Y - 17;
	ClearPanel();

	for (int i = RSPLTYPE_SKILL; i < RSPLTYPE_INVALID; i++) {
		switch ((spell_type)i) {
		case RSPLTYPE_SKILL:
			SetSpellTrans(RSPLTYPE_SKILL);
			mask = plr[myplr]._pAblSpells;
			c = SPLICONLAST + 3;
			break;
		case RSPLTYPE_SPELL:
			mask = plr[myplr]._pMemSpells;
			c = SPLICONLAST + 4;
			break;
		case RSPLTYPE_SCROLL:
			SetSpellTrans(RSPLTYPE_SCROLL);
			mask = plr[myplr]._pScrlSpells;
			c = SPLICONLAST + 1;
			break;
		case RSPLTYPE_CHARGES:
			SetSpellTrans(RSPLTYPE_CHARGES);
			mask = plr[myplr]._pISpells;
			c = SPLICONLAST + 2;
			break;
		case RSPLTYPE_INVALID:
			break;
		}
		int8_t j = SPL_FIREBOLT;
		for (uint64_t spl = 1; j < MAX_SPELLS; spl <<= 1, j++) {
			if ((mask & spl) == 0)
				continue;
			if (i == RSPLTYPE_SPELL) {
				s = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[j];
				if (s < 0)
					s = 0;
				spell_type trans = RSPLTYPE_INVALID;
				if (s > 0)
					trans = RSPLTYPE_SPELL;
				SetSpellTrans(trans);
			}
			if (currlevel == 0 && !spelldata[j].sTownSpell)
				SetSpellTrans(RSPLTYPE_INVALID);
			DrawSpellCel(out, x, y, *pSpellCels, SpellITbl[j]);
			int lx = x;
			int ly = y - SPLICONLENGTH;
			if (MouseX >= lx && MouseX < lx + SPLICONLENGTH && MouseY >= ly && MouseY < ly + SPLICONLENGTH) {
				pSpell = (spell_id)j;
				pSplType = (spell_type)i;
				if (plr[myplr]._pClass == HeroClass::Monk && j == SPL_SEARCH)
					pSplType = RSPLTYPE_SKILL;
				DrawSpellCel(out, x, y, *pSpellCels, c);
				switch (pSplType) {
				case RSPLTYPE_SKILL:
					sprintf(infostr, _("%s Skill"), _(spelldata[pSpell].sSkillText));
					break;
				case RSPLTYPE_SPELL:
					sprintf(infostr, _("%s Spell"), _(spelldata[pSpell].sNameText));
					if (pSpell == SPL_HBOLT) {
						strcpy(tempstr, _("Damages undead only"));
						AddPanelString(tempstr, true);
					}
					if (s == 0)
						strcpy(tempstr, _("Spell Level 0 - Unusable"));
					else
						sprintf(tempstr, _("Spell Level %i"), s);
					AddPanelString(tempstr, true);
					break;
				case RSPLTYPE_SCROLL: {
					sprintf(infostr, _("Scroll of %s"), _(spelldata[pSpell].sNameText));
					int v = 0;
					for (int t = 0; t < plr[myplr]._pNumInv; t++) {
						if (!plr[myplr].InvList[t].isEmpty()
						    && (plr[myplr].InvList[t]._iMiscId == IMISC_SCROLL || plr[myplr].InvList[t]._iMiscId == IMISC_SCROLLT)
						    && plr[myplr].InvList[t]._iSpell == pSpell) {
							v++;
						}
					}
					for (auto &item : plr[myplr].SpdList) {
						if (!item.isEmpty()
						    && (item._iMiscId == IMISC_SCROLL || item._iMiscId == IMISC_SCROLLT)
						    && item._iSpell == pSpell) {
							v++;
						}
					}
					sprintf(tempstr, ngettext("%i Scroll", "%i Scrolls", v), v);
					AddPanelString(tempstr, true);
				} break;
				case RSPLTYPE_CHARGES: {
					sprintf(infostr, _("Staff of %s"), _(spelldata[pSpell].sNameText));
					int charges = plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges;
					sprintf(tempstr, ngettext("%i Charge", "%i Charges", charges), charges);
					AddPanelString(tempstr, true);
				} break;
				case RSPLTYPE_INVALID:
					break;
				}
				for (int t = 0; t < 4; t++) {
					if (plr[myplr]._pSplHotKey[t] == pSpell && plr[myplr]._pSplTHotKey[t] == pSplType) {
						auto hotkeyName = keymapper.keyNameForAction(quickSpellActionIndexes[t]);
						PrintSBookHotkey(out, x - 1, y + 1, hotkeyName, COL_BLACK);
						PrintSBookHotkey(out, x, y, hotkeyName, COL_WHITE);
						sprintf(tempstr, _("Spell Hotkey %s"), hotkeyName.c_str());
						AddPanelString(tempstr, true);
					}
				}
			}
			x -= SPLICONLENGTH;
			if (x == PANEL_X + 12 - SPLICONLENGTH) {
				x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
				y -= SPLICONLENGTH;
			}
		}
		if (mask != 0 && x != PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS)
			x -= SPLICONLENGTH;
		if (x == PANEL_X + 12 - SPLICONLENGTH) {
			x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
			y -= SPLICONLENGTH;
		}
	}
}

void SetSpell()
{
	spselflag = false;
	if (pSpell != SPL_INVALID) {
		ClearPanel();
		plr[myplr]._pRSpell = pSpell;
		plr[myplr]._pRSplType = pSplType;
		force_redraw = 255;
	}
}

void SetSpeedSpell(int slot)
{
	if (pSpell != SPL_INVALID) {
		for (int i = 0; i < 4; ++i) {
			if (plr[myplr]._pSplHotKey[i] == pSpell && plr[myplr]._pSplTHotKey[i] == pSplType)
				plr[myplr]._pSplHotKey[i] = SPL_INVALID;
		}
		plr[myplr]._pSplHotKey[slot] = pSpell;
		plr[myplr]._pSplTHotKey[slot] = pSplType;
	}
}

void ToggleSpell(int slot)
{
	uint64_t spells;

	if (plr[myplr]._pSplHotKey[slot] == SPL_INVALID) {
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
	case RSPLTYPE_INVALID:
		return;
	}

	if ((spells & GetSpellBitmask(plr[myplr]._pSplHotKey[slot])) != 0) {
		plr[myplr]._pRSpell = plr[myplr]._pSplHotKey[slot];
		plr[myplr]._pRSplType = plr[myplr]._pSplTHotKey[slot];
		force_redraw = 255;
	}
}

void PrintChar(const CelOutputBuffer &out, int sx, int sy, int nCel, text_color col)
{
	int i;
	BYTE pix;
	BYTE tbl[256];

	switch (col) {
	case COL_WHITE:
		CelDrawTo(out, sx, sy, *pPanelText, nCel);
		return;
	case COL_BLUE:
		for (i = 0; i < 256; i++) {
			pix = i;
			if (pix > PAL16_GRAY + 13)
				pix = PAL16_BLUE + 15;
			else if (pix >= PAL16_GRAY)
				pix -= PAL16_GRAY - (PAL16_BLUE + 2);
			tbl[i] = pix;
		}
		break;
	case COL_RED:
		for (i = 0; i < 256; i++) {
			pix = i;
			if (pix >= PAL16_GRAY)
				pix -= PAL16_GRAY - PAL16_RED;
			tbl[i] = pix;
		}
		break;
	case COL_GOLD:
		for (i = 0; i < 256; i++) {
			pix = i;
			if (pix >= PAL16_GRAY) {
				if (pix >= PAL16_GRAY + 14)
					pix = PAL16_YELLOW + 15;
				else
					pix -= PAL16_GRAY - (PAL16_YELLOW + 2);
			}
			tbl[i] = pix;
		}
		break;
	case COL_BLACK:
		light_table_index = 15;
		CelDrawLightTo(out, sx, sy, *pPanelText, nCel, nullptr);
		return;
	}
	CelDrawLightTo(out, sx, sy, *pPanelText, nCel, tbl);
}

void AddPanelString(const char *str, bool just)
{
	strcpy(panelstr[pnumlines], str);
	pstrjust[pnumlines] = just;

	if (pnumlines < 4)
		pnumlines++;
}

void ClearPanel()
{
	pnumlines = 0;
	pinfoflag = false;
}

void DrawPanelBox(const CelOutputBuffer &out, int x, int y, int w, int h, int sx, int sy)
{
	const BYTE *src = pBtmBuff.at(x, y);
	BYTE *dst = out.at(sx, sy);

	for (int hgt = h; hgt != 0; hgt--, src += pBtmBuff.pitch(), dst += out.pitch()) {
		memcpy(dst, src, w);
	}
}

/**
 * Draws a section of the empty flask cel on top of the panel to create the illusion
 * of the flask getting empty. This function takes a cel and draws a
 * horizontal stripe of height (max-min) onto the given buffer.
 * @param out Target buffer.
 * @param sx Buffer coordinate
 * @param sy Buffer coordinate
 * @param celBuf Buffer of the empty flask cel.
 * @param y0 Top of the flask cel section to draw.
 * @param y1 Bottom of the flask cel section to draw.
 */
static void DrawFlaskTop(const CelOutputBuffer &out, int sx, int sy, CelOutputBuffer celBuf, int y0, int y1)
{
	const BYTE *src = celBuf.at(0, y0);
	BYTE *dst = out.at(sx, sy);

	for (int h = y1 - y0; h != 0; --h, src += celBuf.pitch(), dst += out.pitch())
		memcpy(dst, src, celBuf.w());
}

/**
 * Draws the dome of the flask that protrudes above the panel top line.
 * It draws a rectangle of fixed width 59 and height 'h' from the source buffer
 * into the target buffer.
 * @param out The target buffer.
 * @param celBuf Buffer of the empty flask cel.
 * @param celX Source buffer start coordinate.
 * @param celY Source buffer start coordinate.
 * @param sx Target buffer coordinate.
 * @param sy Target buffer coordinate.
 * @param h How many lines of the source buffer that will be copied.
 */
static void DrawFlask(const CelOutputBuffer &out, const CelOutputBuffer &celBuf, int celX, int celY, int x, int y, int h)
{
	const BYTE *src = celBuf.at(celX, celY);
	BYTE *dst = out.at(x, y);

	for (int hgt = h; hgt != 0; hgt--, src += celBuf.pitch() - 59, dst += out.pitch() - 59) {
		for (int wdt = 59; wdt != 0; wdt--) {
			if (*src != 0)
				*dst = *src;
			src++;
			dst++;
		}
	}
}

void DrawLifeFlask(const CelOutputBuffer &out)
{
	double p = 0.0;
	if (plr[myplr]._pMaxHP > 0) {
		p = (double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP * 80.0;
	}
	plr[myplr]._pHPPer = p;
	int filled = plr[myplr]._pHPPer;

	if (filled > 80)
		filled = 80;

	filled = 80 - filled;
	if (filled > 11)
		filled = 11;
	filled += 2;

	DrawFlask(out, pLifeBuff, 13, 3, PANEL_LEFT + 109, PANEL_TOP - 13, filled);
	if (filled != 13)
		DrawFlask(out, pBtmBuff, 109, filled + 3, PANEL_LEFT + 109, PANEL_TOP - 13 + filled, 13 - filled);
}

void UpdateLifeFlask(const CelOutputBuffer &out)
{
	double p = 0.0;
	if (plr[myplr]._pMaxHP > 0) {
		p = (double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP * 80.0;
	}
	int filled = p;
	plr[myplr]._pHPPer = filled;

	if (filled > 69)
		filled = 69;
	else if (filled < 0)
		filled = 0;
	if (filled != 69)
		DrawFlaskTop(out, 96 + PANEL_X, PANEL_Y, pLifeBuff, 16, 85 - filled);
	if (filled != 0)
		DrawPanelBox(out, 96, 85 - filled, 88, filled, 96 + PANEL_X, PANEL_Y + 69 - filled);
}

void DrawManaFlask(const CelOutputBuffer &out)
{
	int filled = plr[myplr]._pManaPer;
	if (filled > 80)
		filled = 80;
	filled = 80 - filled;
	if (filled > 11)
		filled = 11;
	filled += 2;

	DrawFlask(out, pManaBuff, 13, 3, PANEL_LEFT + 475, PANEL_TOP - 13, filled);
	if (filled != 13)
		DrawFlask(out, pBtmBuff, 475, filled + 3, PANEL_LEFT + 475, PANEL_TOP - 13 + filled, 13 - filled);
}

void control_update_life_mana()
{
	int maxMana = std::max(plr[myplr]._pMaxMana, 0);
	int mana = std::max(plr[myplr]._pMana, 0);
	plr[myplr]._pManaPer = maxMana != 0 ? ((double)mana / (double)maxMana * 80.0) : 0;
	plr[myplr]._pHPPer = (double)plr[myplr]._pHitPoints / (double)plr[myplr]._pMaxHP * 80.0;
}

void UpdateManaFlask(const CelOutputBuffer &out)
{
	int maxMana = std::max(plr[myplr]._pMaxMana, 0);
	int mana = std::max(plr[myplr]._pMana, 0);
	int filled = maxMana != 0 ? ((double)mana / (double)maxMana * 80.0) : 0;

	plr[myplr]._pManaPer = filled;

	filled = std::min(filled, 69);
	if (filled != 69)
		DrawFlaskTop(out, PANEL_X + 464, PANEL_Y, pManaBuff, 16, 85 - filled);
	if (filled != 0)
		DrawPanelBox(out, 464, 85 - filled, 88, filled, PANEL_X + 464, PANEL_Y + 69 - filled);

	DrawSpell(out);
}

void InitControlPan()
{
	pBtmBuff = CelOutputBuffer::Alloc(PANEL_WIDTH, (PANEL_HEIGHT + 16) * (gbIsMultiplayer ? 2 : 1));
	pManaBuff = CelOutputBuffer::Alloc(88, 88);
	pLifeBuff = CelOutputBuffer::Alloc(88, 88);

	pPanelText = LoadCel("CtrlPan\\SmalText.CEL", 13);
	pChrPanel = LoadCel("Data\\Char.CEL", SPANEL_WIDTH);
	if (!gbIsHellfire)
		pSpellCels = LoadCel("CtrlPan\\SpelIcon.CEL", SPLICONLENGTH);
	else
		pSpellCels = LoadCel("Data\\SpelIcon.CEL", SPLICONLENGTH);
	SetSpellTrans(RSPLTYPE_SKILL);
	CelDrawUnsafeTo(pBtmBuff, 0, (PANEL_HEIGHT + 16) - 1, LoadCel("CtrlPan\\Panel8.CEL", PANEL_WIDTH), 1);
	{
		const CelSprite statusPanel = LoadCel("CtrlPan\\P8Bulbs.CEL", 88);
		CelDrawUnsafeTo(pLifeBuff, 0, 87, statusPanel, 1);
		CelDrawUnsafeTo(pManaBuff, 0, 87, statusPanel, 2);
	}
	talkflag = false;
	if (gbIsMultiplayer) {
		CelDrawUnsafeTo(pBtmBuff, 0, (PANEL_HEIGHT + 16) * 2 - 1, LoadCel("CtrlPan\\TalkPanl.CEL", PANEL_WIDTH), 1);
		pMultiBtns = LoadCel("CtrlPan\\P8But2.CEL", 33);
		pTalkBtns = LoadCel("CtrlPan\\TalkButt.CEL", 61);
		sgbPlrTalkTbl = 0;
		sgszTalkMsg[0] = '\0';
		for (bool &whisper : whisperList)
			whisper = true;
		for (bool &talkButtonDown : talkButtonsDown)
			talkButtonDown = false;
	}
	panelflag = false;
	lvlbtndown = false;
	pPanelButtons = LoadCel("CtrlPan\\Panel8bu.CEL", 71);
	for (bool &panbtn : panbtns)
		panbtn = false;
	panbtndown = false;
	if (!gbIsMultiplayer)
		numpanbtns = 6;
	else
		numpanbtns = 8;
	pChrButtons = LoadCel("Data\\CharBut.CEL", 41);
	for (bool &buttonEnabled : chrbtn)
		buttonEnabled = false;
	chrbtnactive = false;
	pDurIcons = LoadCel("Items\\DurIcons.CEL", 32);
	strcpy(infostr, "");
	ClearPanel();
	drawhpflag = true;
	drawmanaflag = true;
	chrflag = false;
	spselflag = false;
	pSpellBkCel = LoadCel("Data\\SpellBk.CEL", SPANEL_WIDTH);

	if (gbIsHellfire) {
		static const int SBkBtnHellfireWidths[] = { 0, 61, 61, 61, 61, 61, 76 };
		pSBkBtnCel = LoadCel("Data\\SpellBkB.CEL", SBkBtnHellfireWidths);
	} else {
		pSBkBtnCel = LoadCel("Data\\SpellBkB.CEL", 76);
	}
	pSBkIconCels = LoadCel("Data\\SpellI2.CEL", 37);
	sbooktab = 0;
	sbookflag = false;
	if (plr[myplr]._pClass == HeroClass::Warrior) {
		SpellPages[0][0] = SPL_REPAIR;
	} else if (plr[myplr]._pClass == HeroClass::Rogue) {
		SpellPages[0][0] = SPL_DISARM;
	} else if (plr[myplr]._pClass == HeroClass::Sorcerer) {
		SpellPages[0][0] = SPL_RECHARGE;
	} else if (plr[myplr]._pClass == HeroClass::Monk) {
		SpellPages[0][0] = SPL_SEARCH;
	} else if (plr[myplr]._pClass == HeroClass::Bard) {
		SpellPages[0][0] = SPL_IDENTIFY;
	} else if (plr[myplr]._pClass == HeroClass::Barbarian) {
		SpellPages[0][0] = SPL_BLODBOIL;
	}
	pQLogCel = LoadCel("Data\\Quest.CEL", SPANEL_WIDTH);
	pGBoxBuff = LoadCel("CtrlPan\\Golddrop.cel", 261);
	dropGoldFlag = false;
	dropGoldValue = 0;
	initialDropGoldValue = 0;
	initialDropGoldIndex = 0;
}

void DrawCtrlPan(const CelOutputBuffer &out)
{
	DrawPanelBox(out, 0, sgbPlrTalkTbl + 16, PANEL_WIDTH, PANEL_HEIGHT, PANEL_X, PANEL_Y);
	DrawInfoBox(out);
}

void DrawCtrlBtns(const CelOutputBuffer &out)
{
	for (int i = 0; i < 6; i++) {
		if (!panbtns[i])
			DrawPanelBox(out, PanBtnPos[i].x, PanBtnPos[i].y + 16, 71, 20, PanBtnPos[i].x + PANEL_X, PanBtnPos[i].y + PANEL_Y);
		else
			CelDrawTo(out, PanBtnPos[i].x + PANEL_X, PanBtnPos[i].y + PANEL_Y + 18, *pPanelButtons, i + 1);
	}
	if (numpanbtns == 8) {
		CelDrawTo(out, 87 + PANEL_X, 122 + PANEL_Y, *pMultiBtns, panbtns[6] ? 2 : 1);
		if (gbFriendlyMode)
			CelDrawTo(out, 527 + PANEL_X, 122 + PANEL_Y, *pMultiBtns, panbtns[7] ? 4 : 3);
		else
			CelDrawTo(out, 527 + PANEL_X, 122 + PANEL_Y, *pMultiBtns, panbtns[7] ? 6 : 5);
	}
}

/**
 * Draws the "Speed Book": the rows of known spells for quick-setting a spell that
 * show up when you click the spell slot at the control panel.
 */
void DoSpeedBook()
{
	spselflag = true;
	int xo = PANEL_X + 12 + SPLICONLENGTH * 10;
	int yo = PANEL_Y - 17;
	int x = xo + SPLICONLENGTH / 2;
	int y = yo - SPLICONLENGTH / 2;

	if (plr[myplr]._pRSpell != SPL_INVALID) {
		for (int i = RSPLTYPE_SKILL; i <= RSPLTYPE_CHARGES; i++) {
			uint64_t spells;
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
			uint64_t spell = 1;
			for (int j = 1; j < MAX_SPELLS; j++) {
				if ((spell & spells) != 0) {
					if (j == plr[myplr]._pRSpell && i == plr[myplr]._pRSplType) {
						x = xo + SPLICONLENGTH / 2;
						y = yo - SPLICONLENGTH / 2;
					}
					xo -= SPLICONLENGTH;
					if (xo == PANEL_X + 12 - SPLICONLENGTH) {
						xo = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
						yo -= SPLICONLENGTH;
					}
				}
				spell <<= 1ULL;
			}
			if (spells != 0 && xo != PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS)
				xo -= SPLICONLENGTH;
			if (xo == PANEL_X + 12 - SPLICONLENGTH) {
				xo = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
				yo -= SPLICONLENGTH;
			}
		}
	}

	SetCursorPos(x, y);
}

/**
 * Checks if the mouse cursor is within any of the panel buttons and flag it if so.
 */
void DoPanBtn()
{
	for (int i = 0; i < numpanbtns; i++) {
		int x = PanBtnPos[i].x + PANEL_LEFT + PanBtnPos[i].w;
		int y = PanBtnPos[i].y + PANEL_TOP + PanBtnPos[i].h;
		if (MouseX >= PanBtnPos[i].x + PANEL_LEFT && MouseX <= x) {
			if (MouseY >= PanBtnPos[i].y + PANEL_TOP && MouseY <= y) {
				panbtns[i] = true;
				drawbtnflag = true;
				panbtndown = true;
			}
		}
	}
	if (!spselflag && MouseX >= 565 + PANEL_LEFT && MouseX < 621 + PANEL_LEFT && MouseY >= 64 + PANEL_TOP && MouseY < 120 + PANEL_TOP) {
		if ((SDL_GetModState() & KMOD_SHIFT) != 0) {
			plr[myplr]._pRSpell = SPL_INVALID;
			plr[myplr]._pRSplType = RSPLTYPE_INVALID;
			force_redraw = 255;
			return;
		}
		DoSpeedBook();
		gamemenu_off();
	}
}

void control_set_button_down(int btnId)
{
	panbtns[btnId] = true;
	drawbtnflag = true;
	panbtndown = true;
}

void control_check_btn_press()
{
	int x = PanBtnPos[3].x + PANEL_LEFT + PanBtnPos[3].w;
	int y = PanBtnPos[3].y + PANEL_TOP + PanBtnPos[3].h;
	if (MouseX >= PanBtnPos[3].x + PANEL_LEFT
	    && MouseX <= x
	    && MouseY >= PanBtnPos[3].y + PANEL_TOP
	    && MouseY <= y) {
		control_set_button_down(3);
	}
	x = PanBtnPos[6].x + PANEL_LEFT + PanBtnPos[6].w;
	y = PanBtnPos[6].y + PANEL_TOP + PanBtnPos[6].h;
	if (MouseX >= PanBtnPos[6].x + PANEL_LEFT
	    && MouseX <= x
	    && MouseY >= PanBtnPos[6].y + PANEL_TOP
	    && MouseY <= y) {
		control_set_button_down(6);
	}
}

void DoAutoMap()
{
	if (currlevel != 0 || gbIsMultiplayer) {
		if (!AutomapActive)
			StartAutomap();
		else
			AutomapActive = false;
	} else {
		InitDiabloMsg(EMSG_NO_AUTOMAP_IN_TOWN);
	}
}

/**
 * Checks the mouse cursor position within the control panel and sets information
 * strings if needed.
 */
void CheckPanelInfo()
{
	panelflag = false;
	ClearPanel();
	for (int i = 0; i < numpanbtns; i++) {
		int xend = PanBtnPos[i].x + PANEL_LEFT + PanBtnPos[i].w;
		int yend = PanBtnPos[i].y + PANEL_TOP + PanBtnPos[i].h;
		if (MouseX >= PanBtnPos[i].x + PANEL_LEFT && MouseX <= xend && MouseY >= PanBtnPos[i].y + PANEL_TOP && MouseY <= yend) {
			if (i != 7) {
				strcpy(infostr, _(PanBtnStr[i]));
			} else {
				if (gbFriendlyMode)
					strcpy(infostr, _("Player friendly"));
				else
					strcpy(infostr, _("Player attack"));
			}
			if (PanBtnHotKey[i] != nullptr) {
				sprintf(tempstr, _("Hotkey: %s"), _(PanBtnHotKey[i]));
				AddPanelString(tempstr, true);
			}
			infoclr = COL_WHITE;
			panelflag = true;
			pinfoflag = true;
		}
	}
	if (!spselflag && MouseX >= 565 + PANEL_LEFT && MouseX < 621 + PANEL_LEFT && MouseY >= 64 + PANEL_TOP && MouseY < 120 + PANEL_TOP) {
		strcpy(infostr, _("Select current spell button"));
		infoclr = COL_WHITE;
		panelflag = true;
		pinfoflag = true;
		strcpy(tempstr, _("Hotkey: 's'"));
		AddPanelString(tempstr, true);
		spell_id v = plr[myplr]._pRSpell;
		if (v != SPL_INVALID) {
			switch (plr[myplr]._pRSplType) {
			case RSPLTYPE_SKILL:
				sprintf(tempstr, _("%s Skill"), _(spelldata[v].sSkillText));
				AddPanelString(tempstr, true);
				break;
			case RSPLTYPE_SPELL: {
				sprintf(tempstr, _("%s Spell"), _(spelldata[v].sNameText));
				AddPanelString(tempstr, true);
				int c = plr[myplr]._pISplLvlAdd + plr[myplr]._pSplLvl[v];
				if (c < 0)
					c = 0;
				if (c == 0)
					strcpy(tempstr, _("Spell Level 0 - Unusable"));
				else
					sprintf(tempstr, _("Spell Level %i"), c);
				AddPanelString(tempstr, true);
			} break;
			case RSPLTYPE_SCROLL: {
				sprintf(tempstr, _("Scroll of %s"), _(spelldata[v].sNameText));
				AddPanelString(tempstr, true);
				int s = 0;
				for (int i = 0; i < plr[myplr]._pNumInv; i++) {
					if (!plr[myplr].InvList[i].isEmpty()
					    && (plr[myplr].InvList[i]._iMiscId == IMISC_SCROLL || plr[myplr].InvList[i]._iMiscId == IMISC_SCROLLT)
					    && plr[myplr].InvList[i]._iSpell == v) {
						s++;
					}
				}
				for (auto &item : plr[myplr].SpdList) {
					if (!item.isEmpty()
					    && (item._iMiscId == IMISC_SCROLL || item._iMiscId == IMISC_SCROLLT)
					    && item._iSpell == v) {
						s++;
					}
				}
				sprintf(tempstr, ngettext("%i Scroll", "%i Scrolls", s), s);
				AddPanelString(tempstr, true);
			} break;
			case RSPLTYPE_CHARGES:
				sprintf(tempstr, _("Staff of %s"), _(spelldata[v].sNameText));
				AddPanelString(tempstr, true);
				sprintf(tempstr, ngettext("%i Charge", "%i Charges", plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges), plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges);
				AddPanelString(tempstr, true);
				break;
			case RSPLTYPE_INVALID:
				break;
			}
		}
	}
	if (MouseX > 190 + PANEL_LEFT && MouseX < 437 + PANEL_LEFT && MouseY > 4 + PANEL_TOP && MouseY < 33 + PANEL_TOP)
		pcursinvitem = CheckInvHLight();

	if (CheckXPBarInfo()) {
		panelflag = true;
		pinfoflag = true;
	}
}

/**
 * Check if the mouse is within a control panel button that's flagged.
 * Takes apropiate action if so.
 */
void CheckBtnUp()
{
	bool gamemenuOff = true;
	drawbtnflag = true;
	panbtndown = false;

	for (int i = 0; i < 8; i++) {
		if (!panbtns[i]) {
			continue;
		}

		panbtns[i] = false;

		if (MouseX < PanBtnPos[i].x + PANEL_LEFT
		    || MouseX > PanBtnPos[i].x + PANEL_LEFT + PanBtnPos[i].w
		    || MouseY < PanBtnPos[i].y + PANEL_TOP
		    || MouseY > PanBtnPos[i].y + PANEL_TOP + PanBtnPos[i].h) {
			continue;
		}

		switch (i) {
		case PanelButtonCharinfo:
			questlog = false;
			chrflag = !chrflag;
			break;
		case PanelButtonQlog:
			chrflag = false;
			if (!questlog)
				StartQuestlog();
			else
				questlog = false;
			break;
		case PanelButtonAutomap:
			DoAutoMap();
			break;
		case PanelButtonMainmenu:
			qtextflag = false;
			gamemenu_handle_previous();
			gamemenuOff = false;
			break;
		case PanelButtonInventory:
			sbookflag = false;
			invflag = !invflag;
			if (dropGoldFlag) {
				dropGoldFlag = false;
				dropGoldValue = 0;
			}
			break;
		case PanelButtonSpellbook:
			invflag = false;
			if (dropGoldFlag) {
				dropGoldFlag = false;
				dropGoldValue = 0;
			}
			sbookflag = !sbookflag;
			break;
		case PanelButtonSendmsg:
			if (talkflag)
				control_reset_talk();
			else
				control_type_message();
			break;
		case PanelButtonFriendly:
			gbFriendlyMode = !gbFriendlyMode;
			break;
		}
	}

	if (gamemenuOff)
		gamemenu_off();
}

void FreeControlPan()
{
	pBtmBuff.Free();
	pManaBuff.Free();
	pLifeBuff.Free();
	pPanelText = std::nullopt;
	pChrPanel = std::nullopt;
	pSpellCels = std::nullopt;
	pPanelButtons = std::nullopt;
	pMultiBtns = std::nullopt;
	pTalkBtns = std::nullopt;
	pChrButtons = std::nullopt;
	pDurIcons = std::nullopt;
	pQLogCel = std::nullopt;
	pSpellBkCel = std::nullopt;
	pSBkBtnCel = std::nullopt;
	pSBkIconCels = std::nullopt;
	pGBoxBuff = std::nullopt;
}

bool control_WriteStringToBuffer(BYTE *str)
{
	int k = 0;
	while (*str != '\0') {
		BYTE ichar = gbFontTransTbl[*str];
		str++;
		k += fontkern[fontframe[ichar]];
		if (k >= 125)
			return false;
	}

	return true;
}

static void CPrintString(const CelOutputBuffer &out, int y, const char *str, bool center, int lines)
{
	int lineOffset = 0;
	int sx = 177 + PANEL_X;
	int sy = LineOffsets[lines][y] + PANEL_Y;
	if (center) {
		int strWidth = 0;
		const char *tmp = str;
		while (*tmp != 0) {
			BYTE c = gbFontTransTbl[(BYTE)*tmp++];
			strWidth += fontkern[fontframe[c]] + 2;
		}
		if (strWidth < 288)
			lineOffset = (288 - strWidth) / 2;
		sx += lineOffset;
	}
	while (*str != '\0') {
		BYTE c = gbFontTransTbl[(BYTE)*str++];
		c = fontframe[c];
		lineOffset += fontkern[c] + 2;
		if (c != 0) {
			if (lineOffset < 288) {
				PrintChar(out, sx, sy, c, infoclr);
			}
		}
		sx += fontkern[c] + 2;
	}
}

static void PrintInfo(const CelOutputBuffer &out)
{
	if (talkflag)
		return;

	int yo = 0;
	int lo = 1;
	if (infostr[0] != '\0') {
		CPrintString(out, 0, infostr, true, pnumlines);
		yo = 1;
		lo = 0;
	}

	for (int i = 0; i < pnumlines; i++) {
		CPrintString(out, i + yo, panelstr[i], pstrjust[i], pnumlines - lo);
	}
}

void DrawInfoBox(const CelOutputBuffer &out)
{
	DrawPanelBox(out, 177, 62, 288, 60, PANEL_X + 177, PANEL_Y + 46);
	if (!panelflag && !trigflag && pcursinvitem == -1 && !spselflag) {
		infostr[0] = '\0';
		infoclr = COL_WHITE;
		ClearPanel();
	}
	if (spselflag || trigflag) {
		infoclr = COL_WHITE;
	} else if (pcurs >= CURSOR_FIRSTITEM) {
		if (plr[myplr].HoldItem._itype == ITYPE_GOLD) {
			int nGold = plr[myplr].HoldItem._ivalue;
			sprintf(infostr, ngettext("%i gold piece", "%i gold pieces", nGold), nGold);
		} else if (!plr[myplr].HoldItem._iStatFlag) {
			ClearPanel();
			AddPanelString(_("Requirements not met"), true);
			pinfoflag = true;
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
		else if (pcursobj != -1)
			GetObjectStr(pcursobj);
		if (pcursmonst != -1) {
			if (leveltype != DTYPE_TOWN) {
				infoclr = COL_WHITE;
				strcpy(infostr, _(monster[pcursmonst].mName));
				ClearPanel();
				if (monster[pcursmonst]._uniqtype != 0) {
					infoclr = COL_GOLD;
					PrintUniqueHistory();
				} else {
					PrintMonstHistory(monster[pcursmonst].MType->mtype);
				}
			} else if (pcursitem == -1) {
				string_view townerName = towners[pcursmonst]._tName;
				strncpy(infostr, townerName.data(), townerName.length());
				infostr[townerName.length()] = '\0';
			}
		}
		if (pcursplr != -1) {
			infoclr = COL_GOLD;
			strcpy(infostr, plr[pcursplr]._pName);
			ClearPanel();
			sprintf(tempstr, _("%s, Level: %i"), _(ClassStrTbl[static_cast<std::size_t>(plr[pcursplr]._pClass)]), plr[pcursplr]._pLevel);
			AddPanelString(tempstr, true);
			sprintf(tempstr, _("Hit Points %i of %i"), plr[pcursplr]._pHitPoints >> 6, plr[pcursplr]._pMaxHP >> 6);
			AddPanelString(tempstr, true);
		}
	}
	if (infostr[0] != '\0' || pnumlines != 0)
		PrintInfo(out);
}

#define ADD_PlrStringXY(out, x, y, width, pszStr, col) MY_PlrStringXY(out, x, y, width, pszStr, col, 1)

void PrintGameStr(const CelOutputBuffer &out, int x, int y, const char *str, text_color color)
{
	while (*str != '\0') {
		BYTE c = gbFontTransTbl[(BYTE)*str++];
		c = fontframe[c];
		if (c != 0)
			PrintChar(out, x, y, c, color);
		x += fontkern[c] + 1;
	}
}

/**
 * @brief Render text string to the given buffer
 * @param out Buffer to render to
 * @param x Screen coordinate
 * @param y Screen coordinate
 * @param endX End of line in screen coordinate
 * @param pszStr String to print, in Windows-1252 encoding
 * @param col text_color color value
 * @param base Letter spacing
 */
static void MY_PlrStringXY(const CelOutputBuffer &out, int x, int y, int endX, const char *pszStr, text_color col, int base)
{
	int widthOffset = endX - x + 1;
	int line = 0;
	int screenX = 0;
	const char *tmp = pszStr;
	while (*tmp != 0) {
		BYTE c = gbFontTransTbl[(BYTE)*tmp++];
		screenX += fontkern[fontframe[c]] + base;
	}
	if (screenX < widthOffset)
		line = (widthOffset - screenX) / 2;
	x += line;
	while (*pszStr != 0) {
		BYTE c = gbFontTransTbl[(BYTE)*pszStr++];
		c = fontframe[c];
		line += fontkern[c] + base;
		if (c != 0) {
			if (line < widthOffset)
				PrintChar(out, x, y, c, col);
		}
		x += fontkern[c] + base;
	}
}

void DrawChr(const CelOutputBuffer &out)
{
	text_color col = COL_WHITE;
	char chrstr[64];

	CelDrawTo(out, 0, 351, *pChrPanel, 1);
	ADD_PlrStringXY(out, 20, 32, 151, plr[myplr]._pName, COL_WHITE);

	ADD_PlrStringXY(out, 168, 32, 299, _(ClassStrTbl[static_cast<std::size_t>(plr[myplr]._pClass)]), COL_WHITE);

	sprintf(chrstr, "%i", plr[myplr]._pLevel);
	ADD_PlrStringXY(out, 66, 69, 109, chrstr, COL_WHITE);

	sprintf(chrstr, "%i", plr[myplr]._pExperience);
	ADD_PlrStringXY(out, 216, 69, 300, chrstr, COL_WHITE);

	if (plr[myplr]._pLevel == MAXCHARLEVEL - 1) {
		strcpy(chrstr, _("None"));
		col = COL_GOLD;
	} else {
		sprintf(chrstr, "%i", plr[myplr]._pNextExper);
		col = COL_WHITE;
	}
	ADD_PlrStringXY(out, 216, 97, 300, chrstr, col);

	sprintf(chrstr, "%i", plr[myplr]._pGold);
	ADD_PlrStringXY(out, 216, 146, 300, chrstr, COL_WHITE);

	col = COL_WHITE;
	if (plr[myplr]._pIBonusAC > 0)
		col = COL_BLUE;
	if (plr[myplr]._pIBonusAC < 0)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pIBonusAC + plr[myplr]._pIAC + plr[myplr]._pDexterity / 5);
	ADD_PlrStringXY(out, 258, 183, 301, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pIBonusToHit > 0)
		col = COL_BLUE;
	if (plr[myplr]._pIBonusToHit < 0)
		col = COL_RED;
	sprintf(chrstr, "%i%%", (plr[myplr]._pDexterity / 2) + plr[myplr]._pIBonusToHit + 50);
	ADD_PlrStringXY(out, 258, 211, 301, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pIBonusDam > 0)
		col = COL_BLUE;
	if (plr[myplr]._pIBonusDam < 0)
		col = COL_RED;
	int mindam = plr[myplr]._pIMinDam;
	mindam += plr[myplr]._pIBonusDam * mindam / 100;
	mindam += plr[myplr]._pIBonusDamMod;
	if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW) {
		if (plr[myplr]._pClass == HeroClass::Rogue)
			mindam += plr[myplr]._pDamageMod;
		else
			mindam += plr[myplr]._pDamageMod / 2;
	} else {
		mindam += plr[myplr]._pDamageMod;
	}
	int maxdam = plr[myplr]._pIMaxDam;
	maxdam += plr[myplr]._pIBonusDam * maxdam / 100;
	maxdam += plr[myplr]._pIBonusDamMod;
	if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW) {
		if (plr[myplr]._pClass == HeroClass::Rogue)
			maxdam += plr[myplr]._pDamageMod;
		else
			maxdam += plr[myplr]._pDamageMod / 2;
	} else {
		maxdam += plr[myplr]._pDamageMod;
	}
	sprintf(chrstr, "%i-%i", mindam, maxdam);
	if (mindam >= 100 || maxdam >= 100)
		MY_PlrStringXY(out, 254, 239, 305, chrstr, col, -1);
	else
		MY_PlrStringXY(out, 258, 239, 301, chrstr, col, 0);

	if (plr[myplr]._pMagResist == 0)
		col = COL_WHITE;
	else
		col = COL_BLUE;
	if (plr[myplr]._pMagResist < MAXRESIST) {
		sprintf(chrstr, "%i%%", plr[myplr]._pMagResist);
	} else {
		col = COL_GOLD;
		strcpy(chrstr, _("MAX"));
	}
	ADD_PlrStringXY(out, 257, 276, 300, chrstr, col);

	if (plr[myplr]._pFireResist == 0)
		col = COL_WHITE;
	else
		col = COL_BLUE;
	if (plr[myplr]._pFireResist < MAXRESIST) {
		sprintf(chrstr, "%i%%", plr[myplr]._pFireResist);
	} else {
		col = COL_GOLD;
		strcpy(chrstr, _("MAX"));
	}
	ADD_PlrStringXY(out, 257, 304, 300, chrstr, col);

	if (plr[myplr]._pLghtResist == 0)
		col = COL_WHITE;
	else
		col = COL_BLUE;
	if (plr[myplr]._pLghtResist < MAXRESIST) {
		sprintf(chrstr, "%i%%", plr[myplr]._pLghtResist);
	} else {
		col = COL_GOLD;
		strcpy(chrstr, _("MAX"));
	}
	ADD_PlrStringXY(out, 257, 332, 300, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseStr);
	if (plr[myplr].GetMaximumAttributeValue(CharacterAttribute::Strength) == plr[myplr]._pBaseStr)
		col = COL_GOLD;
	ADD_PlrStringXY(out, 95, 155, 126, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseMag);
	if (plr[myplr].GetMaximumAttributeValue(CharacterAttribute::Magic) == plr[myplr]._pBaseMag)
		col = COL_GOLD;
	ADD_PlrStringXY(out, 95, 183, 126, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseDex);
	if (plr[myplr].GetMaximumAttributeValue(CharacterAttribute::Dexterity) == plr[myplr]._pBaseDex)
		col = COL_GOLD;
	ADD_PlrStringXY(out, 95, 211, 126, chrstr, col);

	col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pBaseVit);
	if (plr[myplr].GetMaximumAttributeValue(CharacterAttribute::Vitality) == plr[myplr]._pBaseVit)
		col = COL_GOLD;
	ADD_PlrStringXY(out, 95, 239, 126, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pStrength > plr[myplr]._pBaseStr)
		col = COL_BLUE;
	if (plr[myplr]._pStrength < plr[myplr]._pBaseStr)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pStrength);
	ADD_PlrStringXY(out, 143, 155, 173, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pMagic > plr[myplr]._pBaseMag)
		col = COL_BLUE;
	if (plr[myplr]._pMagic < plr[myplr]._pBaseMag)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pMagic);
	ADD_PlrStringXY(out, 143, 183, 173, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pDexterity > plr[myplr]._pBaseDex)
		col = COL_BLUE;
	if (plr[myplr]._pDexterity < plr[myplr]._pBaseDex)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pDexterity);
	ADD_PlrStringXY(out, 143, 211, 173, chrstr, col);

	col = COL_WHITE;
	if (plr[myplr]._pVitality > plr[myplr]._pBaseVit)
		col = COL_BLUE;
	if (plr[myplr]._pVitality < plr[myplr]._pBaseVit)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pVitality);
	ADD_PlrStringXY(out, 143, 239, 173, chrstr, col);

	if (plr[myplr]._pStatPts > 0) {
		if (CalcStatDiff(myplr) < plr[myplr]._pStatPts) {
			plr[myplr]._pStatPts = CalcStatDiff(myplr);
		}
	}
	if (plr[myplr]._pStatPts > 0) {
		sprintf(chrstr, "%i", plr[myplr]._pStatPts);
		ADD_PlrStringXY(out, 95, 266, 126, chrstr, COL_RED);
		if (plr[myplr]._pBaseStr < plr[myplr].GetMaximumAttributeValue(CharacterAttribute::Strength))
			CelDrawTo(out, 137, 159, *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Strength)] ? 3 : 2);
		if (plr[myplr]._pBaseMag < plr[myplr].GetMaximumAttributeValue(CharacterAttribute::Magic))
			CelDrawTo(out, 137, 187, *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Magic)] ? 5 : 4);
		if (plr[myplr]._pBaseDex < plr[myplr].GetMaximumAttributeValue(CharacterAttribute::Dexterity))
			CelDrawTo(out, 137, 216, *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Dexterity)] ? 7 : 6);
		if (plr[myplr]._pBaseVit < plr[myplr].GetMaximumAttributeValue(CharacterAttribute::Vitality))
			CelDrawTo(out, 137, 244, *pChrButtons, chrbtn[static_cast<size_t>(CharacterAttribute::Vitality)] ? 9 : 8);
	}

	if (plr[myplr]._pMaxHP > plr[myplr]._pMaxHPBase)
		col = COL_BLUE;
	else
		col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pMaxHP >> 6);
	ADD_PlrStringXY(out, 95, 304, 126, chrstr, col);
	if (plr[myplr]._pHitPoints != plr[myplr]._pMaxHP)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pHitPoints >> 6);
	ADD_PlrStringXY(out, 143, 304, 174, chrstr, col);

	if (plr[myplr]._pMaxMana > plr[myplr]._pMaxManaBase)
		col = COL_BLUE;
	else
		col = COL_WHITE;
	sprintf(chrstr, "%i", plr[myplr]._pMaxMana >> 6);
	ADD_PlrStringXY(out, 95, 332, 126, chrstr, col);
	if (plr[myplr]._pMana != plr[myplr]._pMaxMana)
		col = COL_RED;
	sprintf(chrstr, "%i", plr[myplr]._pMana >> 6);
	ADD_PlrStringXY(out, 143, 332, 174, chrstr, col);
}

void CheckLvlBtn()
{
	if (!lvlbtndown && MouseX >= 40 + PANEL_LEFT && MouseX <= 81 + PANEL_LEFT && MouseY >= -39 + PANEL_TOP && MouseY <= -17 + PANEL_TOP)
		lvlbtndown = true;
}

void ReleaseLvlBtn()
{
	if (MouseX >= 40 + PANEL_LEFT && MouseX <= 81 + PANEL_LEFT && MouseY >= -39 + PANEL_TOP && MouseY <= -17 + PANEL_TOP)
		chrflag = true;
	lvlbtndown = false;
}

void DrawLevelUpIcon(const CelOutputBuffer &out)
{
	if (stextflag == STORE_NONE) {
		int nCel = lvlbtndown ? 3 : 2;
		ADD_PlrStringXY(out, PANEL_LEFT + 0, PANEL_TOP - 49, PANEL_LEFT + 120, _("Level Up"), COL_WHITE);
		CelDrawTo(out, 40 + PANEL_X, -17 + PANEL_Y, *pChrButtons, nCel);
	}
}

void CheckChrBtns()
{
	if (chrbtnactive || plr[myplr]._pStatPts == 0)
		return;

	for (auto attribute : enum_values<CharacterAttribute>()) {
		int max = plr[myplr].GetMaximumAttributeValue(attribute);
		switch (attribute) {
		case CharacterAttribute::Strength:
			if (plr[myplr]._pBaseStr >= max)
				continue;
			break;
		case CharacterAttribute::Magic:
			if (plr[myplr]._pBaseMag >= max)
				continue;
			break;
		case CharacterAttribute::Dexterity:
			if (plr[myplr]._pBaseDex >= max)
				continue;
			break;
		case CharacterAttribute::Vitality:
			if (plr[myplr]._pBaseVit >= max)
				continue;
			break;
		default:
			continue;
		}
		auto buttonId = static_cast<size_t>(attribute);
		int x = ChrBtnsRect[buttonId].x + ChrBtnsRect[buttonId].w;
		int y = ChrBtnsRect[buttonId].y + ChrBtnsRect[buttonId].h;
		if (MouseX >= ChrBtnsRect[buttonId].x
		    && MouseX <= x
		    && MouseY >= ChrBtnsRect[buttonId].y
		    && MouseY <= y) {
			chrbtn[buttonId] = true;
			chrbtnactive = true;
		}
	}
}

int CapStatPointsToAdd(int remainingStatPoints, const PlayerStruct &player, CharacterAttribute attribute)
{
	int pointsToReachCap = player.GetMaximumAttributeValue(attribute) - player.GetBaseAttributeValue(attribute);

	return std::min(remainingStatPoints, pointsToReachCap);
}

void ReleaseChrBtns(bool addAllStatPoints)
{
	chrbtnactive = false;
	for (auto attribute : enum_values<CharacterAttribute>()) {
		auto buttonId = static_cast<size_t>(attribute);
		if (!chrbtn[buttonId])
			continue;

		chrbtn[buttonId] = false;
		if (MouseX >= ChrBtnsRect[buttonId].x
		    && MouseX <= ChrBtnsRect[buttonId].x + ChrBtnsRect[buttonId].w
		    && MouseY >= ChrBtnsRect[buttonId].y
		    && MouseY <= ChrBtnsRect[buttonId].y + ChrBtnsRect[buttonId].h) {
			PlayerStruct &player = plr[myplr];
			int statPointsToAdd = 1;
			if (addAllStatPoints)
				statPointsToAdd = CapStatPointsToAdd(player._pStatPts, player, attribute);
			switch (attribute) {
			case CharacterAttribute::Strength:
				NetSendCmdParam1(true, CMD_ADDSTR, statPointsToAdd);
				player._pStatPts -= statPointsToAdd;
				break;
			case CharacterAttribute::Magic:
				NetSendCmdParam1(true, CMD_ADDMAG, statPointsToAdd);
				player._pStatPts -= statPointsToAdd;
				break;
			case CharacterAttribute::Dexterity:
				NetSendCmdParam1(true, CMD_ADDDEX, statPointsToAdd);
				player._pStatPts -= statPointsToAdd;
				break;
			case CharacterAttribute::Vitality:
				NetSendCmdParam1(true, CMD_ADDVIT, statPointsToAdd);
				player._pStatPts -= statPointsToAdd;
				break;
			}
		}
	}
}

static int DrawDurIcon4Item(const CelOutputBuffer &out, ItemStruct *pItem, int x, int c)
{
	if (pItem->isEmpty())
		return x;
	if (pItem->_iDurability > 5)
		return x;
	if (c == 0) {
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
		default:
			c = 1;
			break;
		}
	}
	if (pItem->_iDurability > 2)
		c += 8;
	CelDrawTo(out, x, -17 + PANEL_Y, *pDurIcons, c);
	return x - 32 - 8;
}

void DrawDurIcon(const CelOutputBuffer &out)
{
	bool hasRoomBetweenPanels = gnScreenWidth >= PANEL_WIDTH + 16 + (32 + 8 + 32 + 8 + 32 + 8 + 32) + 16;
	bool hasRoomUnderPanels = gnScreenHeight >= SPANEL_HEIGHT + PANEL_HEIGHT + 16 + 32 + 16;

	if (!hasRoomBetweenPanels && !hasRoomUnderPanels) {
		if ((chrflag || questlog) && (invflag || sbookflag))
			return;
	}

	int x = PANEL_X + PANEL_WIDTH - 32 - 16;
	if (!hasRoomUnderPanels) {
		if (invflag || sbookflag)
			x -= SPANEL_WIDTH - (gnScreenWidth - PANEL_WIDTH) / 2;
	}

	PlayerStruct *p = &plr[myplr];
	x = DrawDurIcon4Item(out, &p->InvBody[INVLOC_HEAD], x, 4);
	x = DrawDurIcon4Item(out, &p->InvBody[INVLOC_CHEST], x, 3);
	x = DrawDurIcon4Item(out, &p->InvBody[INVLOC_HAND_LEFT], x, 0);
	DrawDurIcon4Item(out, &p->InvBody[INVLOC_HAND_RIGHT], x, 0);
}

void RedBack(const CelOutputBuffer &out)
{
	uint8_t *dst = out.begin();
	uint8_t *tbl = &pLightTbl[4608];
	for (int h = gnViewportHeight; h != 0; h--, dst += out.pitch() - gnScreenWidth) {
		for (int w = gnScreenWidth; w != 0; w--) {
			if (leveltype != DTYPE_HELL || *dst >= 32)
				*dst = tbl[*dst];
			dst++;
		}
	}
}

static void PrintSBookStr(const CelOutputBuffer &out, int x, int y, bool cjustflag, const char *pszStr, text_color col)
{
	int sx = x + RIGHT_PANEL_X + SPLICONLENGTH;
	int line = 0;
	if (cjustflag) {
		int screenX = 0;
		const char *tmp = pszStr;
		while (*tmp != 0) {
			BYTE c = gbFontTransTbl[(BYTE)*tmp++];
			screenX += fontkern[fontframe[c]] + 1;
		}
		if (screenX < 222)
			line = (222 - screenX) / 2;
		sx += line;
	}
	while (*pszStr != 0) {
		BYTE c = gbFontTransTbl[(BYTE)*pszStr++];
		c = fontframe[c];
		line += fontkern[c] + 1;
		if (c != 0) {
			if (line <= 222)
				PrintChar(out, sx, y, c, col);
		}
		sx += fontkern[c] + 1;
	}
}

spell_type GetSBookTrans(spell_id ii, bool townok)
{
	if ((plr[myplr]._pClass == HeroClass::Monk) && (ii == SPL_SEARCH))
		return RSPLTYPE_SKILL;
	spell_type st = RSPLTYPE_SPELL;
	if ((plr[myplr]._pISpells & GetSpellBitmask(ii)) != 0) {
		st = RSPLTYPE_CHARGES;
	}
	if ((plr[myplr]._pAblSpells & GetSpellBitmask(ii)) != 0) {
		st = RSPLTYPE_SKILL;
	}
	if (st == RSPLTYPE_SPELL) {
		if (!CheckSpell(myplr, ii, st, true)) {
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

void DrawSpellBook(const CelOutputBuffer &out)
{
	CelDrawTo(out, RIGHT_PANEL_X, 351, *pSpellBkCel, 1);
	if (gbIsHellfire && sbooktab < 5) {
		CelDrawTo(out, RIGHT_PANEL_X + 61 * sbooktab + 7, 348, *pSBkBtnCel, sbooktab + 1);
	} else {
		// BUGFIX: rendering of page 3 and page 4 buttons are both off-by-one pixel (fixed).
		int sx = RIGHT_PANEL_X + 76 * sbooktab + 7;
		if (sbooktab == 2 || sbooktab == 3) {
			sx++;
		}
		CelDrawTo(out, sx, 348, *pSBkBtnCel, sbooktab + 1);
	}
	uint64_t spl = plr[myplr]._pMemSpells | plr[myplr]._pISpells | plr[myplr]._pAblSpells;

	int yp = 55;
	for (int i = 1; i < 8; i++) {
		spell_id sn = SpellPages[sbooktab][i - 1];
		if (sn != SPL_INVALID && (spl & GetSpellBitmask(sn)) != 0) {
			spell_type st = GetSBookTrans(sn, true);
			SetSpellTrans(st);
			DrawSpellCel(out, RIGHT_PANEL_X + 11, yp, *pSBkIconCels, SpellITbl[sn]);
			if (sn == plr[myplr]._pRSpell && st == plr[myplr]._pRSplType) {
				SetSpellTrans(RSPLTYPE_SKILL);
				DrawSpellCel(out, RIGHT_PANEL_X + 11, yp, *pSBkIconCels, SPLICONLAST);
			}
			PrintSBookStr(out, 10, yp - 23, false, _(spelldata[sn].sNameText), COL_WHITE);
			switch (GetSBookTrans(sn, false)) {
			case RSPLTYPE_SKILL:
				strcpy(tempstr, _("Skill"));
				break;
			case RSPLTYPE_CHARGES: {
				int charges = plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges;
				sprintf(tempstr, ngettext("Staff (%i charge)", "Staff (%i charges)", charges), charges);
			} break;
			default: {
				int mana = GetManaAmount(myplr, sn) >> 6;
				int min;
				int max;
				GetDamageAmt(sn, &min, &max);
				if (min != -1) {
					sprintf(tempstr, _("Mana: %i  Dam: %i - %i"), mana, min, max);
				} else {
					sprintf(tempstr, _("Mana: %i   Dam: n/a"), mana);
				}
				if (sn == SPL_BONESPIRIT) {
					sprintf(tempstr, _("Mana: %i  Dam: 1/3 tgt hp"), mana);
				}
				PrintSBookStr(out, 10, yp - 1, false, tempstr, COL_WHITE);
				int lvl = plr[myplr]._pSplLvl[sn] + plr[myplr]._pISplLvlAdd;
				if (lvl < 0) {
					lvl = 0;
				}
				if (lvl == 0) {
					strcpy(tempstr, _("Spell Level 0 - Unusable"));
				} else {
					sprintf(tempstr, _("Spell Level %i"), lvl);
				}
			} break;
			}
			PrintSBookStr(out, 10, yp - 12, false, tempstr, COL_WHITE);
		}
		yp += 43;
	}
}

void CheckSBook()
{
	if (MouseX >= RIGHT_PANEL + 11 && MouseX < RIGHT_PANEL + 48 && MouseY >= 18 && MouseY < 314) {
		spell_id sn = SpellPages[sbooktab][(MouseY - 18) / 43];
		uint64_t spl = plr[myplr]._pMemSpells | plr[myplr]._pISpells | plr[myplr]._pAblSpells;
		if (sn != SPL_INVALID && (spl & GetSpellBitmask(sn)) != 0) {
			spell_type st = RSPLTYPE_SPELL;
			if ((plr[myplr]._pISpells & GetSpellBitmask(sn)) != 0) {
				st = RSPLTYPE_CHARGES;
			}
			if ((plr[myplr]._pAblSpells & GetSpellBitmask(sn)) != 0) {
				st = RSPLTYPE_SKILL;
			}
			plr[myplr]._pRSpell = sn;
			plr[myplr]._pRSplType = st;
			force_redraw = 255;
		}
	}
	if (MouseX >= RIGHT_PANEL + 7 && MouseX < RIGHT_PANEL + 311 && MouseY >= SPANEL_WIDTH && MouseY < 349) {
		sbooktab = (MouseX - (RIGHT_PANEL + 7)) / (gbIsHellfire ? 61 : 76);
	}
}

void DrawGoldSplit(const CelOutputBuffer &out, int amount)
{
	int screenX = 0;
	CelDrawTo(out, 351, 178, *pGBoxBuff, 1);
	sprintf(tempstr, _("You have %u gold"), initialDropGoldValue);
	ADD_PlrStringXY(out, 366, 87, 600, tempstr, COL_GOLD);
	sprintf(tempstr, ngettext("piece.  How many do", "pieces.  How many do", initialDropGoldValue), initialDropGoldValue);
	ADD_PlrStringXY(out, 366, 103, 600, tempstr, COL_GOLD);
	ADD_PlrStringXY(out, 366, 121, 600, _("you want to remove?"), COL_GOLD);
	if (amount > 0) {
		sprintf(tempstr, "%u", amount);
		PrintGameStr(out, 388, 140, tempstr, COL_WHITE);
	}
	if (amount > 0) {
		for (int i = 0; i < tempstr[i]; i++) {
			BYTE c = fontframe[gbFontTransTbl[(BYTE)tempstr[i]]];
			screenX += fontkern[c] + 1;
		}
		screenX += 388;
	} else {
		screenX = 386;
	}
	CelDrawTo(out, screenX, 140, *pSPentSpn2Cels, PentSpn2Spin());
}

void control_drop_gold(char vkey)
{
	char input[6];

	if (plr[myplr]._pHitPoints >> 6 <= 0) {
		dropGoldFlag = false;
		dropGoldValue = 0;
		return;
	}

	memset(input, 0, sizeof(input));
	snprintf(input, sizeof(input), "%d", dropGoldValue);
	if (vkey == DVL_VK_RETURN) {
		if (dropGoldValue > 0)
			control_remove_gold(myplr, initialDropGoldIndex);
		dropGoldFlag = false;
	} else if (vkey == DVL_VK_ESCAPE) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	} else if (vkey == DVL_VK_BACK) {
		input[strlen(input) - 1] = '\0';
		dropGoldValue = atoi(input);
	} else if (vkey - '0' >= 0 && vkey - '0' <= 9) {
		if (dropGoldValue != 0 || atoi(input) <= initialDropGoldValue) {
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

void control_remove_gold(int pnum, int goldIndex)
{
	if (goldIndex <= INVITEM_INV_LAST) {
		int gi = goldIndex - INVITEM_INV_FIRST;
		plr[pnum].InvList[gi]._ivalue -= dropGoldValue;
		if (plr[pnum].InvList[gi]._ivalue > 0)
			SetGoldCurs(pnum, gi);
		else
			RemoveInvItem(pnum, gi);
	} else {
		int gi = goldIndex - INVITEM_BELT_FIRST;
		plr[pnum].SpdList[gi]._ivalue -= dropGoldValue;
		if (plr[pnum].SpdList[gi]._ivalue > 0)
			SetSpdbarGoldCurs(pnum, gi);
		else
			RemoveSpdBarItem(pnum, gi);
	}
	SetPlrHandItem(&plr[pnum].HoldItem, IDI_GOLD);
	GetGoldSeed(pnum, &plr[pnum].HoldItem);
	plr[pnum].HoldItem._ivalue = dropGoldValue;
	plr[pnum].HoldItem._iStatFlag = true;
	control_set_gold_curs(pnum);
	plr[pnum]._pGold = CalculateGold(pnum);
	dropGoldValue = 0;
}

void control_set_gold_curs(int pnum)
{
	SetPlrHandGoldCurs(&plr[pnum].HoldItem);
	NewCursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
}

static char *ControlPrintTalkMsg(const CelOutputBuffer &out, char *msg, int *x, int y, text_color color)
{
	*x += 200;
	y += 22 + PANEL_Y;
	int width = *x;
	while (*msg != 0) {
		BYTE c = gbFontTransTbl[(BYTE)*msg];
		c = fontframe[c];
		width += fontkern[c] + 1;
		if (width > 450 + PANEL_X)
			return msg;
		msg++;
		if (c != 0) {
			PrintChar(out, *x, y, c, color);
		}
		*x += fontkern[c] + 1;
	}
	return nullptr;
}

void DrawTalkPan(const CelOutputBuffer &out)
{
	if (!talkflag)
		return;

	DrawPanelBox(out, 175, sgbPlrTalkTbl + 20, 294, 5, PANEL_X + 175, PANEL_Y + 4);
	int off = 0;
	for (int i = 293; i > 283; off++, i--) {
		DrawPanelBox(out, (off / 2) + 175, sgbPlrTalkTbl + off + 25, i, 1, (off / 2) + PANEL_X + 175, off + PANEL_Y + 9);
	}
	DrawPanelBox(out, 185, sgbPlrTalkTbl + 35, 274, 30, PANEL_X + 185, PANEL_Y + 19);
	DrawPanelBox(out, 180, sgbPlrTalkTbl + 65, 284, 5, PANEL_X + 180, PANEL_Y + 49);
	for (int i = 0; i < 10; i++) {
		DrawPanelBox(out, 180, sgbPlrTalkTbl + i + 70, i + 284, 1, PANEL_X + 180, i + PANEL_Y + 54);
	}
	DrawPanelBox(out, 170, sgbPlrTalkTbl + 80, 310, 55, PANEL_X + 170, PANEL_Y + 64);
	char *msg = sgszTalkMsg;
	int i = 0;
	int x = 0;
	for (; i < 39; i += 13) {
		x = PANEL_LEFT;
		msg = ControlPrintTalkMsg(out, msg, &x, i, COL_WHITE);
		if (msg == nullptr)
			break;
	}
	if (msg != nullptr)
		*msg = '\0';
	CelDrawTo(out, x, i + 22 + PANEL_Y, *pSPentSpn2Cels, PentSpn2Spin());
	int talkBtn = 0;
	for (int i = 0; i < 4; i++) {
		if (i == myplr)
			continue;
		text_color color = COL_RED;
		if (whisperList[i]) {
			color = COL_GOLD;
			if (talkButtonsDown[talkBtn]) {
				int nCel = talkBtn != 0 ? 4 : 3;
				CelDrawTo(out, 172 + PANEL_X, 84 + 18 * talkBtn + PANEL_Y, *pTalkBtns, nCel);
			}
		} else {
			int nCel = talkBtn != 0 ? 2 : 1;
			if (talkButtonsDown[talkBtn])
				nCel += 4;
			CelDrawTo(out, 172 + PANEL_X, 84 + 18 * talkBtn + PANEL_Y, *pTalkBtns, nCel);
		}
		if (plr[i].plractive) {
			int x = 46 + PANEL_LEFT;
			ControlPrintTalkMsg(out, plr[i]._pName, &x, 60 + talkBtn * 18, color);
		}

		talkBtn++;
	}
}

bool control_check_talk_btn()
{
	if (!talkflag)
		return false;

	if (MouseX < 172 + PANEL_LEFT)
		return false;
	if (MouseY < 69 + PANEL_TOP)
		return false;
	if (MouseX > 233 + PANEL_LEFT)
		return false;
	if (MouseY > 123 + PANEL_TOP)
		return false;

	for (bool &talkButtonDown : talkButtonsDown) {
		talkButtonDown = false;
	}

	talkButtonsDown[(MouseY - (69 + PANEL_TOP)) / 18] = true;

	return true;
}

void control_release_talk_btn()
{
	if (!talkflag)
		return;

	for (bool &talkButtonDown : talkButtonsDown)
		talkButtonDown = false;

	if (MouseX < 172 + PANEL_LEFT || MouseY < 69 + PANEL_TOP || MouseX > 233 + PANEL_LEFT || MouseY > 123 + PANEL_TOP)
		return;

	int off = (MouseY - (69 + PANEL_TOP)) / 18;

	int p = 0;
	for (; p < MAX_PLRS && off != -1; p++) {
		if (p != myplr)
			off--;
	}
	if (p <= MAX_PLRS)
		whisperList[p - 1] = !whisperList[p - 1];
}

void control_reset_talk_msg()
{
	uint32_t pmask = 0;

	for (int i = 0; i < MAX_PLRS; i++) {
		if (whisperList[i])
			pmask |= 1 << i;
	}
	NetSendCmdString(pmask, sgszTalkMsg);
}

void control_type_message()
{
	if (!gbIsMultiplayer)
		return;

	talkflag = true;
	sgszTalkMsg[0] = '\0';
	for (bool &talkButtonDown : talkButtonsDown) {
		talkButtonDown = false;
	}
	sgbPlrTalkTbl = PANEL_HEIGHT + 16;
	force_redraw = 255;
	sgbTalkSavePos = sgbNextTalkSave;
}

void control_reset_talk()
{
	talkflag = false;
	sgbPlrTalkTbl = 0;
	force_redraw = 255;
}

static void ControlPressEnter()
{
	if (sgszTalkMsg[0] != 0) {
		control_reset_talk_msg();
		int i = 0;
		for (; i < 8; i++) {
			if (strcmp(sgszTalkSave[i], sgszTalkMsg) == 0)
				break;
		}
		if (i >= 8) {
			strcpy(sgszTalkSave[sgbNextTalkSave], sgszTalkMsg);
			sgbNextTalkSave++;
			sgbNextTalkSave &= 7;
		} else {
			BYTE talkSave = sgbNextTalkSave - 1;
			talkSave &= 7;
			if (i != talkSave) {
				strcpy(sgszTalkSave[i], sgszTalkSave[talkSave]);
				strcpy(sgszTalkSave[talkSave], sgszTalkMsg);
			}
		}
		sgszTalkMsg[0] = '\0';
		sgbTalkSavePos = sgbNextTalkSave;
	}
	control_reset_talk();
}

bool control_talk_last_key(int vkey)
{
	if (!gbIsMultiplayer)
		return false;

	if (!talkflag)
		return false;

	if ((DWORD)vkey < DVL_VK_SPACE)
		return false;

	int result = strlen(sgszTalkMsg);
	if (result < 78) {
		sgszTalkMsg[result] = vkey;
		sgszTalkMsg[result + 1] = '\0';
	}
	return true;
}

static void ControlUpDown(int v)
{
	for (int i = 0; i < 8; i++) {
		sgbTalkSavePos = (v + sgbTalkSavePos) & 7;
		if (sgszTalkSave[sgbTalkSavePos][0] != 0) {
			strcpy(sgszTalkMsg, sgszTalkSave[sgbTalkSavePos]);
			return;
		}
	}
}

bool control_presskeys(int vkey)
{
	if (!gbIsMultiplayer)
		return false;
	if (!talkflag)
		return false;

	if (vkey == DVL_VK_ESCAPE) {
		control_reset_talk();
	} else if (vkey == DVL_VK_RETURN) {
		ControlPressEnter();
	} else if (vkey == DVL_VK_BACK) {
		int len = strlen(sgszTalkMsg);
		if (len > 0)
			sgszTalkMsg[len - 1] = '\0';
	} else if (vkey == DVL_VK_DOWN) {
		ControlUpDown(1);
	} else if (vkey == DVL_VK_UP) {
		ControlUpDown(-1);
	} else if (vkey != DVL_VK_SPACE) {
		return false;
	}

	return true;
}

} // namespace devilution
