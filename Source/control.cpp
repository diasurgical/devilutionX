/**
 * @file control.cpp
 *
 * Implementation of the character and main control panels
 */
#include "control.h"

#include <array>
#include <cstddef>

#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "automap.h"
#include "controls/keymapper.hpp"
#include "cursor.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "error.h"
#include "gamemenu.h"
#include "init.h"
#include "inv.h"
#include "lighting.h"
#include "minitext.h"
#include "missiles.h"
#include "panels/charpanel.hpp"
#include "qol/xpbar.h"
#include "stores.h"
#include "towners.h"
#include "trigs.h"
#include "utils/language.h"
#include "utils/sdl_geometry.h"
#include "utils/stdcompat/optional.hpp"
#include "options.h"

#ifdef _DEBUG
#include "debug.h"
#endif

namespace devilution {
/**
 * @brief Set if the life flask needs to be redrawn during next frame
 */
bool drawhpflag;
bool dropGoldFlag;
bool chrbtn[4];
bool lvlbtndown;
int dropGoldValue;
/**
 * @brief Set if the mana flask needs to be redrawn during the next frame
 */
bool drawmanaflag;
bool chrbtnactive;
int pnumlines;
UiFlags InfoColor;
char tempstr[256];
int sbooktab;
int8_t initialDropGoldIndex;
bool talkflag;
bool sbookflag;
bool chrflag;
bool drawbtnflag;
char infostr[64];
bool panelflag;
int initialDropGoldValue;
bool panbtndown;
bool spselflag;
Rectangle MainPanel;
Rectangle LeftPanel;
Rectangle RightPanel;

extern std::array<Keymapper::ActionIndex, 4> quickSpellActionIndexes;

/** Maps from attribute_id to the rectangle on screen used for attribute increment buttons. */
Rectangle ChrBtnsRect[4] = {
	{ { 137, 138 }, { 41, 22 } },
	{ { 137, 166 }, { 41, 22 } },
	{ { 137, 195 }, { 41, 22 } },
	{ { 137, 223 }, { 41, 22 } }
};

namespace {

std::optional<OwnedSurface> pBtmBuff;
std::optional<OwnedSurface> pLifeBuff;
std::optional<OwnedSurface> pManaBuff;
std::optional<CelSprite> talkButtons;
std::optional<CelSprite> pDurIcons;
std::optional<CelSprite> multiButtons;
std::optional<CelSprite> pPanelButtons;
std::optional<CelSprite> pGBoxBuff;
std::optional<CelSprite> pSBkBtnCel;
std::optional<CelSprite> pSBkIconCels;
std::optional<CelSprite> pSpellBkCel;
std::optional<CelSprite> pSpellCels;

bool PanelButtons[8];
int PanelButtonIndex;
char TalkSave[8][80];
uint8_t TalkSaveIndex;
uint8_t NextTalkSave;
char TalkMessage[MAX_SEND_STR_LEN];
bool TalkButtonsDown[3];
int sgbPlrTalkTbl;
bool WhisperList[MAX_PLRS];
char panelstr[4][64];
uint8_t SplTransTbl[256];

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

void CalculatePanelAreas()
{
	MainPanel = { { (gnScreenWidth - PANEL_WIDTH) / 2, gnScreenHeight - PANEL_HEIGHT }, { PANEL_WIDTH, PANEL_HEIGHT } };
	LeftPanel = { { 0, 0 }, { SPANEL_WIDTH, SPANEL_HEIGHT } };
	RightPanel = { { 0, 0 }, { SPANEL_WIDTH, SPANEL_HEIGHT } };

	if (gnScreenWidth - 2 * SPANEL_WIDTH > PANEL_WIDTH) {
		LeftPanel.position.x = (gnScreenWidth - 2 * SPANEL_WIDTH - PANEL_WIDTH) / 2;
	} else {
		LeftPanel.position.x = 0;
	}

	LeftPanel.position.y = (gnScreenHeight - LeftPanel.size.height - PANEL_HEIGHT) / 2;

	RightPanel.position.x = gnScreenWidth - RightPanel.size.width - LeftPanel.position.x;
	RightPanel.position.y = LeftPanel.position.y;
}

/**
 * Draw spell cell onto the given buffer.
 * @param out Output buffer
 * @param position Buffer coordinates
 * @param cel The CEL sprite
 * @param nCel Index of the cel frame to draw. 0 based.
 */
void DrawSpellCel(const Surface &out, Point position, const CelSprite &cel, int nCel)
{
	CelDrawLightTo(out, position, cel, nCel, SplTransTbl);
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

void PrintSBookHotkey(const Surface &out, Point position, const std::string &text)
{
	// Align the hot key text with the top-right corner of the spell icon
	position += Displacement { SPLICONLENGTH - (GetLineWidth(text.c_str()) + 5), 5 - SPLICONLENGTH };

	// Draw a drop shadow below and to the left of the text
	DrawString(out, text, position + Displacement { -1, 1 }, UiFlags::ColorBlack);
	// Then draw the text over the top
	DrawString(out, text, position, UiFlags::ColorSilver);
}

/**
 * Draws a section of the empty flask cel on top of the panel to create the illusion
 * of the flask getting empty. This function takes a cel and draws a
 * horizontal stripe of height (max-min) onto the given buffer.
 * @param out Target buffer.
 * @param position Buffer coordinate.
 * @param celBuf Buffer of the empty flask cel.
 * @param y0 Top of the flask cel section to draw.
 * @param y1 Bottom of the flask cel section to draw.
 */
void DrawFlaskTop(const Surface &out, Point position, const Surface &celBuf, int y0, int y1)
{
	out.BlitFrom(celBuf, SDL_Rect { 0, static_cast<decltype(SDL_Rect {}.y)>(y0), celBuf.w(), y1 - y0 }, position);
}

/**
 * Draws the dome of the flask that protrudes above the panel top line.
 * It draws a rectangle of fixed width 59 and height 'h' from the source buffer
 * into the target buffer.
 * @param out The target buffer.
 * @param celBuf Buffer of the empty flask cel.
 * @param sourcePosition Source buffer start coordinate.
 * @param targetPosition Target buffer coordinate.
 * @param h How many lines of the source buffer that will be copied.
 */
void DrawFlask(const Surface &out, const Surface &celBuf, Point sourcePosition, Point targetPosition, int h)
{
	constexpr int FlaskWidth = 59;
	out.BlitFromSkipColorIndexZero(celBuf, MakeSdlRect(sourcePosition.x, sourcePosition.y, FlaskWidth, h), targetPosition);
}

/**
 * @brief Draws the part of the life/mana flasks protruding above the bottom panel
 * @see DrawFlaskLower()
 * @param out The display region to draw to
 * @param sourceBuffer A sprite representing the appropriate background/empty flask style
 * @param offset X coordinate offset for where the flask should be drawn
 * @param fillPer How full the flask is (a value from 0 to 80)
 */
void DrawFlaskUpper(const Surface &out, const Surface &sourceBuffer, int offset, int fillPer)
{
	// clamping because this function only draws the top 12% of the flask display
	int emptyPortion = clamp(80 - fillPer, 0, 11) + 2; // +2 to account for the frame being included in the sprite

	// Draw the empty part of the flask
	DrawFlask(out, sourceBuffer, { 13, 3 }, { PANEL_LEFT + offset, PANEL_TOP - 13 }, emptyPortion);
	if (emptyPortion < 13)
		// Draw the filled part of the flask
		DrawFlask(out, *pBtmBuff, { offset, emptyPortion + 3 }, { PANEL_LEFT + offset, PANEL_TOP - 13 + emptyPortion }, 13 - emptyPortion);
}

/**
 * @brief Draws the part of the life/mana flasks inside the bottom panel
 * @see DrawFlaskUpper()
 * @param out The display region to draw to
 * @param sourceBuffer A sprite representing the appropriate background/empty flask style
 * @param offset X coordinate offset for where the flask should be drawn
 * @param fillPer How full the flask is (a value from 0 to 80)
 */
void DrawFlaskLower(const Surface &out, const Surface &sourceBuffer, int offset, int fillPer)
{
	int filled = clamp(fillPer, 0, 69);

	if (filled < 69)
		DrawFlaskTop(out, { PANEL_X + offset, PANEL_Y }, sourceBuffer, 16, 85 - filled);

	// It appears that the panel defaults to having a filled flask and DrawFlaskTop only overlays the appropriate amount of empty space.
	// This draw might not be necessary?
	if (filled > 0)
		DrawPanelBox(out, { offset, 85 - filled, 88, filled }, { PANEL_X + offset, PANEL_Y + 69 - filled });
}

void SetButtonStateDown(int btnId)
{
	PanelButtons[btnId] = true;
	drawbtnflag = true;
	panbtndown = true;
}

void PrintInfo(const Surface &out)
{
	if (talkflag)
		return;

	const int LineStart[] = { 70, 58, 52, 48, 46 };
	const int LineHeights[] = { 30, 24, 18, 15, 12 };

	Rectangle line { { PANEL_X + 177, PANEL_Y + LineStart[pnumlines] }, { 288, 12 } };

	if (infostr[0] != '\0') {
		DrawString(out, infostr, line, InfoColor | UiFlags::AlignCenter | UiFlags::KerningFitSpacing, 2);
		line.position.y += LineHeights[pnumlines];
	}

	for (int i = 0; i < pnumlines; i++) {
		DrawString(out, panelstr[i], line, InfoColor | UiFlags::AlignCenter | UiFlags::KerningFitSpacing, 2);
		line.position.y += LineHeights[pnumlines];
	}
}

int CapStatPointsToAdd(int remainingStatPoints, const Player &player, CharacterAttribute attribute)
{
	int pointsToReachCap = player.GetMaximumAttributeValue(attribute) - player.GetBaseAttributeValue(attribute);

	return std::min(remainingStatPoints, pointsToReachCap);
}

int DrawDurIcon4Item(const Surface &out, Item &pItem, int x, int c)
{
	if (pItem.isEmpty())
		return x;
	if (pItem._iDurability > 5)
		return x;
	if (c == 0) {
		switch (pItem._itype) {
		case ItemType::Sword:
			c = 2;
			break;
		case ItemType::Axe:
			c = 6;
			break;
		case ItemType::Bow:
			c = 7;
			break;
		case ItemType::Mace:
			c = 5;
			break;
		case ItemType::Staff:
			c = 8;
			break;
		default:
			c = 1;
			break;
		}
	}
	if (pItem._iDurability > 2)
		c += 8;
	CelDrawTo(out, { x, -17 + PANEL_Y }, *pDurIcons, c);
	return x - 32 - 8;
}

void PrintSBookStr(const Surface &out, Point position, const char *text)
{
	DrawString(out, text, { GetPanelPosition(UiPanels::Spell, { SPLICONLENGTH + position.x, position.y }), { 222, 0 } }, UiFlags::ColorSilver);
}

spell_type GetSBookTrans(spell_id ii, bool townok)
{
	auto &myPlayer = Players[MyPlayerId];
	if ((myPlayer._pClass == HeroClass::Monk) && (ii == SPL_SEARCH))
		return RSPLTYPE_SKILL;
	spell_type st = RSPLTYPE_SPELL;
	if ((myPlayer._pISpells & GetSpellBitmask(ii)) != 0) {
		st = RSPLTYPE_CHARGES;
	}
	if ((myPlayer._pAblSpells & GetSpellBitmask(ii)) != 0) {
		st = RSPLTYPE_SKILL;
	}
	if (st == RSPLTYPE_SPELL) {
		if (CheckSpell(MyPlayerId, ii, st, true) != SpellCheckResult::Success) {
			st = RSPLTYPE_INVALID;
		}
		if ((char)(myPlayer._pSplLvl[ii] + myPlayer._pISplLvlAdd) <= 0) {
			st = RSPLTYPE_INVALID;
		}
	}
	if (townok && currlevel == 0 && st != RSPLTYPE_INVALID && !spelldata[ii].sTownSpell) {
		st = RSPLTYPE_INVALID;
	}

	return st;
}

void ControlSetGoldCurs(Player &player)
{
	SetPlrHandGoldCurs(player.HoldItem);
	NewCursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
}

void ResetTalkMsg()
{
#ifdef _DEBUG
	if (CheckDebugTextCommand(TalkMessage))
		return;
#endif

	uint32_t pmask = 0;

	for (int i = 0; i < MAX_PLRS; i++) {
		if (WhisperList[i])
			pmask |= 1 << i;
	}

	NetSendCmdString(pmask, TalkMessage);
}

void ControlPressEnter()
{
	if (TalkMessage[0] != 0) {
		ResetTalkMsg();
		uint8_t i = 0;
		for (; i < 8; i++) {
			if (strcmp(TalkSave[i], TalkMessage) == 0)
				break;
		}
		if (i >= 8) {
			strcpy(TalkSave[NextTalkSave], TalkMessage);
			NextTalkSave++;
			NextTalkSave &= 7;
		} else {
			uint8_t talkSave = NextTalkSave - 1;
			talkSave &= 7;
			if (i != talkSave) {
				strcpy(TalkSave[i], TalkSave[talkSave]);
				strcpy(TalkSave[talkSave], TalkMessage);
			}
		}
		TalkMessage[0] = '\0';
		TalkSaveIndex = NextTalkSave;
	}
	control_reset_talk();
}

void ControlUpDown(int v)
{
	for (int i = 0; i < 8; i++) {
		TalkSaveIndex = (v + TalkSaveIndex) & 7;
		if (TalkSave[TalkSaveIndex][0] != 0) {
			strcpy(TalkMessage, TalkSave[TalkSaveIndex]);
			return;
		}
	}
}

void RemoveGold(Player &player, int goldIndex)
{
	int gi = goldIndex - INVITEM_INV_FIRST;
	player.InvList[gi]._ivalue -= dropGoldValue;
	if (player.InvList[gi]._ivalue > 0)
		SetPlrHandGoldCurs(player.InvList[gi]);
	else
		player.RemoveInvItem(gi);
	SetPlrHandItem(player.HoldItem, IDI_GOLD);
	SetGoldSeed(player, player.HoldItem);
	player.HoldItem._ivalue = dropGoldValue;
	player.HoldItem._iStatFlag = true;
	ControlSetGoldCurs(player);
	player._pGold = CalculateGold(player);
	dropGoldValue = 0;
}

struct SpellListItem {
	Point location;
	spell_type type;
	spell_id id;
	bool isSelected;
};

std::vector<SpellListItem> GetSpellListItems()
{
	std::vector<SpellListItem> spellListItems;

	uint64_t mask;

	int x = PANEL_X + 12 + SPLICONLENGTH * SPLROWICONLS;
	int y = PANEL_Y - 17;

	for (int i = RSPLTYPE_SKILL; i < RSPLTYPE_INVALID; i++) {
		auto &myPlayer = Players[MyPlayerId];
		switch ((spell_type)i) {
		case RSPLTYPE_SKILL:
			mask = myPlayer._pAblSpells;
			break;
		case RSPLTYPE_SPELL:
			mask = myPlayer._pMemSpells;
			break;
		case RSPLTYPE_SCROLL:
			mask = myPlayer._pScrlSpells;
			break;
		case RSPLTYPE_CHARGES:
			mask = myPlayer._pISpells;
			break;
		case RSPLTYPE_INVALID:
			break;
		}
		int8_t j = SPL_FIREBOLT;
		for (uint64_t spl = 1; j < MAX_SPELLS; spl <<= 1, j++) {
			if ((mask & spl) == 0)
				continue;
			int lx = x;
			int ly = y - SPLICONLENGTH;
			bool isSelected = (MousePosition.x >= lx && MousePosition.x < lx + SPLICONLENGTH && MousePosition.y >= ly && MousePosition.y < ly + SPLICONLENGTH);
			spellListItems.emplace_back(SpellListItem { { x, y }, (spell_type)i, (spell_id)j, isSelected });
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

	return spellListItems;
}

bool GetSpellListSelection(spell_id &pSpell, spell_type &pSplType)
{
	pSpell = spell_id::SPL_INVALID;
	pSplType = spell_type::RSPLTYPE_INVALID;
	auto &myPlayer = Players[MyPlayerId];

	for (auto &spellListItem : GetSpellListItems()) {
		if (spellListItem.isSelected) {
			pSpell = spellListItem.id;
			pSplType = spellListItem.type;
			if (myPlayer._pClass == HeroClass::Monk && spellListItem.id == SPL_SEARCH)
				pSplType = RSPLTYPE_SKILL;
			return true;
		}
	}

	return false;
}

bool IsChatAvailable()
{
#ifdef _DEBUG
	return true;
#else
	return gbIsMultiplayer;
#endif
}

} // namespace

void DrawSpell(const Surface &out)
{
	auto &myPlayer = Players[MyPlayerId];
	spell_id spl = myPlayer._pRSpell;
	spell_type st = myPlayer._pRSplType;

	// BUGFIX: Move the next line into the if statement to avoid OOB (SPL_INVALID is -1) (fixed)
	if (st == RSPLTYPE_SPELL && spl != SPL_INVALID) {
		int tlvl = myPlayer._pISplLvlAdd + myPlayer._pSplLvl[spl];
		if (CheckSpell(MyPlayerId, spl, st, true) != SpellCheckResult::Success)
			st = RSPLTYPE_INVALID;
		if (tlvl <= 0)
			st = RSPLTYPE_INVALID;
	}
	if (currlevel == 0 && st != RSPLTYPE_INVALID && !spelldata[spl].sTownSpell)
		st = RSPLTYPE_INVALID;
	SetSpellTrans(st);
	const int nCel = (spl != SPL_INVALID) ? SpellITbl[spl] : 27;
	const Point position { PANEL_X + 565, PANEL_Y + 119 };
	DrawSpellCel(out, position, *pSpellCels, nCel);
}

void DrawSpellList(const Surface &out)
{
	infostr[0] = '\0';
	ClearPanel();

	auto &myPlayer = Players[MyPlayerId];

	for (auto &spellListItem : GetSpellListItems()) {
		spell_type transType = spellListItem.type;
		int spellLevel = 0;
		const SpellData &spellDataItem = spelldata[static_cast<size_t>(spellListItem.id)];
		if (currlevel == 0 && !spellDataItem.sTownSpell) {
			transType = RSPLTYPE_INVALID;
		}
		if (spellListItem.type == RSPLTYPE_SPELL) {
			spellLevel = std::max(myPlayer._pISplLvlAdd + myPlayer._pSplLvl[spellListItem.id], 0);
			if (spellLevel == 0)
				transType = RSPLTYPE_INVALID;
		}

		SetSpellTrans(transType);
		DrawSpellCel(out, spellListItem.location, *pSpellCels, SpellITbl[static_cast<size_t>(spellListItem.id)]);

		if (!spellListItem.isSelected)
			continue;

		switch (spellListItem.type) {
		case RSPLTYPE_SKILL:
			DrawSpellCel(out, spellListItem.location, *pSpellCels, SPLICONLAST + 3);
			strcpy(infostr, fmt::format(_("{:s} Skill"), _(spellDataItem.sSkillText)).c_str());
			break;
		case RSPLTYPE_SPELL:
			DrawSpellCel(out, spellListItem.location, *pSpellCels, SPLICONLAST + 4);
			strcpy(infostr, fmt::format(_("{:s} Spell"), _(spellDataItem.sNameText)).c_str());
			if (spellListItem.id == SPL_HBOLT) {
				strcpy(tempstr, _("Damages undead only"));
				AddPanelString(tempstr);
			}
			if (spellLevel == 0)
				strcpy(tempstr, _("Spell Level 0 - Unusable"));
			else
				strcpy(tempstr, fmt::format(_("Spell Level {:d}"), spellLevel).c_str());
			AddPanelString(tempstr);
			break;
		case RSPLTYPE_SCROLL: {
			DrawSpellCel(out, spellListItem.location, *pSpellCels, SPLICONLAST + 1);
			strcpy(infostr, fmt::format(_("Scroll of {:s}"), _(spellDataItem.sNameText)).c_str());
			int v = 0;
			for (int t = 0; t < myPlayer._pNumInv; t++) {
				if (!myPlayer.InvList[t].isEmpty()
				    && (myPlayer.InvList[t]._iMiscId == IMISC_SCROLL || myPlayer.InvList[t]._iMiscId == IMISC_SCROLLT)
				    && myPlayer.InvList[t]._iSpell == spellListItem.id) {
					v++;
				}
			}
			for (auto &item : myPlayer.SpdList) {
				if (!item.isEmpty()
				    && (item._iMiscId == IMISC_SCROLL || item._iMiscId == IMISC_SCROLLT)
				    && item._iSpell == spellListItem.id) {
					v++;
				}
			}
			strcpy(tempstr, fmt::format(ngettext("{:d} Scroll", "{:d} Scrolls", v), v).c_str());
			AddPanelString(tempstr);
		} break;
		case RSPLTYPE_CHARGES: {
			DrawSpellCel(out, spellListItem.location, *pSpellCels, SPLICONLAST + 2);
			strcpy(infostr, fmt::format(_("Staff of {:s}"), _(spellDataItem.sNameText)).c_str());
			int charges = myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges;
			strcpy(tempstr, fmt::format(ngettext("{:d} Charge", "{:d} Charges", charges), charges).c_str());
			AddPanelString(tempstr);
		} break;
		case RSPLTYPE_INVALID:
			break;
		}
		for (int t = 0; t < 4; t++) {
			if (myPlayer._pSplHotKey[t] == spellListItem.id && myPlayer._pSplTHotKey[t] == spellListItem.type) {
				auto hotkeyName = keymapper.KeyNameForAction(quickSpellActionIndexes[t]);
				PrintSBookHotkey(out, spellListItem.location, hotkeyName);
				strcpy(tempstr, fmt::format(_("Spell Hotkey {:s}"), hotkeyName.c_str()).c_str());
				AddPanelString(tempstr);
			}
		}
	}
}

void SetSpell()
{
	spell_id pSpell;
	spell_type pSplType;

	spselflag = false;
	if (!GetSpellListSelection(pSpell, pSplType)) {
		return;
	}

	ClearPanel();

	auto &myPlayer = Players[MyPlayerId];
	myPlayer._pRSpell = pSpell;
	myPlayer._pRSplType = pSplType;

	force_redraw = 255;
}

void SetSpeedSpell(int slot)
{
	spell_id pSpell;
	spell_type pSplType;

	if (!GetSpellListSelection(pSpell, pSplType)) {
		return;
	}
	auto &myPlayer = Players[MyPlayerId];
	for (int i = 0; i < 4; ++i) {
		if (myPlayer._pSplHotKey[i] == pSpell && myPlayer._pSplTHotKey[i] == pSplType)
			myPlayer._pSplHotKey[i] = SPL_INVALID;
	}
	myPlayer._pSplHotKey[slot] = pSpell;
	myPlayer._pSplTHotKey[slot] = pSplType;
}

void ToggleSpell(int slot)
{
	uint64_t spells;

	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pSplHotKey[slot] == SPL_INVALID) {
		return;
	}

	switch (myPlayer._pSplTHotKey[slot]) {
	case RSPLTYPE_SKILL:
		spells = myPlayer._pAblSpells;
		break;
	case RSPLTYPE_SPELL:
		spells = myPlayer._pMemSpells;
		break;
	case RSPLTYPE_SCROLL:
		spells = myPlayer._pScrlSpells;
		break;
	case RSPLTYPE_CHARGES:
		spells = myPlayer._pISpells;
		break;
	case RSPLTYPE_INVALID:
		return;
	}

	if ((spells & GetSpellBitmask(myPlayer._pSplHotKey[slot])) != 0) {
		myPlayer._pRSpell = myPlayer._pSplHotKey[slot];
		myPlayer._pRSplType = myPlayer._pSplTHotKey[slot];
		force_redraw = 255;
	}
}

void AddPanelString(const char *str)
{
	strcpy(panelstr[pnumlines], str);

	if (pnumlines < 4)
		pnumlines++;
}

void ClearPanel()
{
	pnumlines = 0;
}

Point GetPanelPosition(UiPanels panel, Point offset)
{
	Displacement displacement { offset.x, offset.y };

	switch (panel) {
	case UiPanels::Main:
		return MainPanel.position + displacement;
	case UiPanels::Quest:
	case UiPanels::Character:
		return LeftPanel.position + displacement;
	case UiPanels::Spell:
	case UiPanels::Inventory:
		return RightPanel.position + displacement;
	default:
		return MainPanel.position + displacement;
	}
}

void DrawPanelBox(const Surface &out, SDL_Rect srcRect, Point targetPosition)
{
	out.BlitFrom(*pBtmBuff, srcRect, targetPosition);
}

void DrawLifeFlaskUpper(const Surface &out)
{
	constexpr int LifeFlaskUpperOffset = 109;
	DrawFlaskUpper(out, *pLifeBuff, LifeFlaskUpperOffset, Players[MyPlayerId]._pHPPer);
}

void DrawManaFlaskUpper(const Surface &out)
{
	constexpr int ManaFlaskUpperOffset = 475;
	DrawFlaskUpper(out, *pManaBuff, ManaFlaskUpperOffset, Players[MyPlayerId]._pManaPer);
}

void DrawLifeFlaskLower(const Surface &out)
{
	constexpr int LifeFlaskLowerOffset = 96;
	DrawFlaskLower(out, *pLifeBuff, LifeFlaskLowerOffset, Players[MyPlayerId]._pHPPer);
}

void DrawManaFlaskLower(const Surface &out)
{
	constexpr int ManaFlaskLowerOffeset = 464;
	DrawFlaskLower(out, *pManaBuff, ManaFlaskLowerOffeset, Players[MyPlayerId]._pManaPer);
}

void control_update_life_mana()
{
	Players[MyPlayerId].UpdateManaPercentage();
	Players[MyPlayerId].UpdateHitPointPercentage();
}

void InitControlPan()
{
	pBtmBuff.emplace(PANEL_WIDTH, (PANEL_HEIGHT + 16) * (IsChatAvailable() ? 2 : 1));
	pManaBuff.emplace(88, 88);
	pLifeBuff.emplace(88, 88);

	LoadCharPanel();

	if (!gbIsHellfire)
		pSpellCels = LoadCel("CtrlPan\\SpelIcon.CEL", SPLICONLENGTH);
	else
		pSpellCels = LoadCel("Data\\SpelIcon.CEL", SPLICONLENGTH);
	SetSpellTrans(RSPLTYPE_SKILL);
	CelDrawUnsafeTo(*pBtmBuff, { 0, (PANEL_HEIGHT + 16) - 1 }, LoadCel("CtrlPan\\Panel8.CEL", PANEL_WIDTH), 1);
	{
		const Point bulbsPosition { 0, 87 };
		const CelSprite statusPanel = LoadCel("CtrlPan\\P8Bulbs.CEL", 88);
		CelDrawUnsafeTo(*pLifeBuff, bulbsPosition, statusPanel, 1);
		CelDrawUnsafeTo(*pManaBuff, bulbsPosition, statusPanel, 2);
	}
	talkflag = false;
	if (IsChatAvailable()) {
		CelDrawUnsafeTo(*pBtmBuff, { 0, (PANEL_HEIGHT + 16) * 2 - 1 }, LoadCel("CtrlPan\\TalkPanl.CEL", PANEL_WIDTH), 1);
		multiButtons = LoadCel("CtrlPan\\P8But2.CEL", 33);
		talkButtons = LoadCel("CtrlPan\\TalkButt.CEL", 61);
		sgbPlrTalkTbl = 0;
		TalkMessage[0] = '\0';
		for (bool &whisper : WhisperList)
			whisper = true;
		for (bool &talkButtonDown : TalkButtonsDown)
			talkButtonDown = false;
	}
	panelflag = false;
	lvlbtndown = false;
	pPanelButtons = LoadCel("CtrlPan\\Panel8bu.CEL", 71);
	for (bool &panbtn : PanelButtons)
		panbtn = false;
	panbtndown = false;
	if (!IsChatAvailable())
		PanelButtonIndex = 6;
	else
		PanelButtonIndex = 8;
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

	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pClass == HeroClass::Warrior) {
		SpellPages[0][0] = SPL_REPAIR;
	} else if (myPlayer._pClass == HeroClass::Rogue) {
		SpellPages[0][0] = SPL_DISARM;
	} else if (myPlayer._pClass == HeroClass::Sorcerer) {
		SpellPages[0][0] = SPL_RECHARGE;
	} else if (myPlayer._pClass == HeroClass::Monk) {
		SpellPages[0][0] = SPL_SEARCH;
	} else if (myPlayer._pClass == HeroClass::Bard) {
		SpellPages[0][0] = SPL_IDENTIFY;
	} else if (myPlayer._pClass == HeroClass::Barbarian) {
		SpellPages[0][0] = SPL_BLODBOIL;
	}
	pQLogCel = LoadCel("Data\\Quest.CEL", SPANEL_WIDTH);
	pGBoxBuff = LoadCel("CtrlPan\\Golddrop.cel", 261);
	dropGoldFlag = false;
	dropGoldValue = 0;
	initialDropGoldValue = 0;
	initialDropGoldIndex = 0;

	CalculatePanelAreas();
}

void DrawCtrlPan(const Surface &out)
{
	DrawPanelBox(out, { 0, sgbPlrTalkTbl + 16, PANEL_WIDTH, PANEL_HEIGHT }, { PANEL_X, PANEL_Y });
	DrawInfoBox(out);
}

void DrawCtrlBtns(const Surface &out)
{
	for (int i = 0; i < 6; i++) {
		if (!PanelButtons[i])
			DrawPanelBox(out, { PanBtnPos[i].x, PanBtnPos[i].y + 16, 71, 20 }, { PanBtnPos[i].x + PANEL_X, PanBtnPos[i].y + PANEL_Y });
		else
			CelDrawTo(out, { PanBtnPos[i].x + PANEL_X, PanBtnPos[i].y + PANEL_Y + 18 }, *pPanelButtons, i + 1);
	}
	if (PanelButtonIndex == 8) {
		CelDrawTo(out, { 87 + PANEL_X, 122 + PANEL_Y }, *multiButtons, PanelButtons[6] ? 2 : 1);
		if (gbFriendlyMode)
			CelDrawTo(out, { 527 + PANEL_X, 122 + PANEL_Y }, *multiButtons, PanelButtons[7] ? 4 : 3);
		else
			CelDrawTo(out, { 527 + PANEL_X, 122 + PANEL_Y }, *multiButtons, PanelButtons[7] ? 6 : 5);
	}
}

void DoSpeedBook()
{
	spselflag = true;
	int xo = PANEL_X + 12 + SPLICONLENGTH * 10;
	int yo = PANEL_Y - 17;
	int x = xo + SPLICONLENGTH / 2;
	int y = yo - SPLICONLENGTH / 2;

	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pRSpell != SPL_INVALID) {
		for (int i = RSPLTYPE_SKILL; i <= RSPLTYPE_CHARGES; i++) {
			uint64_t spells;
			switch (i) {
			case RSPLTYPE_SKILL:
				spells = myPlayer._pAblSpells;
				break;
			case RSPLTYPE_SPELL:
				spells = myPlayer._pMemSpells;
				break;
			case RSPLTYPE_SCROLL:
				spells = myPlayer._pScrlSpells;
				break;
			case RSPLTYPE_CHARGES:
				spells = myPlayer._pISpells;
				break;
			}
			uint64_t spell = 1;
			for (int j = 1; j < MAX_SPELLS; j++) {
				if ((spell & spells) != 0) {
					if (j == myPlayer._pRSpell && i == myPlayer._pRSplType) {
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

	SetCursorPos({ x, y });
}

void ClearPanBtn()
{
	for (int i = 0; i < PanelButtonIndex; i++)
		PanelButtons[i] = false;
	drawbtnflag = true;
	panbtndown = false;
}

void DoPanBtn()
{
	for (int i = 0; i < PanelButtonIndex; i++) {
		int x = PanBtnPos[i].x + PANEL_LEFT + PanBtnPos[i].w;
		int y = PanBtnPos[i].y + PANEL_TOP + PanBtnPos[i].h;
		if (MousePosition.x >= PanBtnPos[i].x + PANEL_LEFT && MousePosition.x <= x) {
			if (MousePosition.y >= PanBtnPos[i].y + PANEL_TOP && MousePosition.y <= y) {
				PanelButtons[i] = true;
				drawbtnflag = true;
				panbtndown = true;
			}
		}
	}
	if (!spselflag && MousePosition.x >= 565 + PANEL_LEFT && MousePosition.x < 621 + PANEL_LEFT && MousePosition.y >= 64 + PANEL_TOP && MousePosition.y < 120 + PANEL_TOP) {
		if ((SDL_GetModState() & KMOD_SHIFT) != 0) {
			auto &myPlayer = Players[MyPlayerId];
			myPlayer._pRSpell = SPL_INVALID;
			myPlayer._pRSplType = RSPLTYPE_INVALID;
			force_redraw = 255;
			return;
		}
		DoSpeedBook();
		gamemenu_off();
	}
}

void control_check_btn_press()
{
	int x = PanBtnPos[3].x + PANEL_LEFT + PanBtnPos[3].w;
	int y = PanBtnPos[3].y + PANEL_TOP + PanBtnPos[3].h;
	if (MousePosition.x >= PanBtnPos[3].x + PANEL_LEFT
	    && MousePosition.x <= x
	    && MousePosition.y >= PanBtnPos[3].y + PANEL_TOP
	    && MousePosition.y <= y) {
		SetButtonStateDown(3);
	}
	x = PanBtnPos[6].x + PANEL_LEFT + PanBtnPos[6].w;
	y = PanBtnPos[6].y + PANEL_TOP + PanBtnPos[6].h;
	if (MousePosition.x >= PanBtnPos[6].x + PANEL_LEFT
	    && MousePosition.x <= x
	    && MousePosition.y >= PanBtnPos[6].y + PANEL_TOP
	    && MousePosition.y <= y) {
		SetButtonStateDown(6);
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

void CheckPanelInfo()
{
	panelflag = false;
	ClearPanel();
	for (int i = 0; i < PanelButtonIndex; i++) {
		int xend = PanBtnPos[i].x + PANEL_LEFT + PanBtnPos[i].w;
		int yend = PanBtnPos[i].y + PANEL_TOP + PanBtnPos[i].h;
		if (MousePosition.x >= PanBtnPos[i].x + PANEL_LEFT && MousePosition.x <= xend && MousePosition.y >= PanBtnPos[i].y + PANEL_TOP && MousePosition.y <= yend) {
			if (i != 7) {
				strcpy(infostr, _(PanBtnStr[i]));
			} else {
				if (gbFriendlyMode)
					strcpy(infostr, _("Player friendly"));
				else
					strcpy(infostr, _("Player attack"));
			}
			if (PanBtnHotKey[i] != nullptr) {
				strcpy(tempstr, fmt::format(_("Hotkey: {:s}"), _(PanBtnHotKey[i])).c_str());
				AddPanelString(tempstr);
			}
			InfoColor = UiFlags::ColorSilver;
			panelflag = true;
		}
	}
	if (!spselflag && MousePosition.x >= 565 + PANEL_LEFT && MousePosition.x < 621 + PANEL_LEFT && MousePosition.y >= 64 + PANEL_TOP && MousePosition.y < 120 + PANEL_TOP) {
		strcpy(infostr, _("Select current spell button"));
		InfoColor = UiFlags::ColorSilver;
		panelflag = true;
		strcpy(tempstr, _("Hotkey: 's'"));
		AddPanelString(tempstr);
		auto &myPlayer = Players[MyPlayerId];
		spell_id v = myPlayer._pRSpell;
		if (v != SPL_INVALID) {
			switch (myPlayer._pRSplType) {
			case RSPLTYPE_SKILL:
				strcpy(tempstr, fmt::format(_("{:s} Skill"), _(spelldata[v].sSkillText)).c_str());
				AddPanelString(tempstr);
				break;
			case RSPLTYPE_SPELL: {
				strcpy(tempstr, fmt::format(_("{:s} Spell"), _(spelldata[v].sNameText)).c_str());
				AddPanelString(tempstr);
				int c = std::max(myPlayer._pISplLvlAdd + myPlayer._pSplLvl[v], 0);
				if (c == 0)
					strcpy(tempstr, _("Spell Level 0 - Unusable"));
				else
					strcpy(tempstr, fmt::format(_("Spell Level {:d}"), c).c_str());
				AddPanelString(tempstr);
			} break;
			case RSPLTYPE_SCROLL: {
				strcpy(tempstr, fmt::format(_("Scroll of {:s}"), _(spelldata[v].sNameText)).c_str());
				AddPanelString(tempstr);
				int s = 0;
				for (int i = 0; i < myPlayer._pNumInv; i++) {
					if (!myPlayer.InvList[i].isEmpty()
					    && (myPlayer.InvList[i]._iMiscId == IMISC_SCROLL || myPlayer.InvList[i]._iMiscId == IMISC_SCROLLT)
					    && myPlayer.InvList[i]._iSpell == v) {
						s++;
					}
				}
				for (auto &item : myPlayer.SpdList) {
					if (!item.isEmpty()
					    && (item._iMiscId == IMISC_SCROLL || item._iMiscId == IMISC_SCROLLT)
					    && item._iSpell == v) {
						s++;
					}
				}
				strcpy(tempstr, fmt::format(ngettext("{:d} Scroll", "{:d} Scrolls", s), s).c_str());
				AddPanelString(tempstr);
			} break;
			case RSPLTYPE_CHARGES:
				strcpy(tempstr, fmt::format(_("Staff of {:s}"), _(spelldata[v].sNameText)).c_str());
				AddPanelString(tempstr);
				strcpy(tempstr, fmt::format(ngettext("{:d} Charge", "{:d} Charges", myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges), myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges).c_str());
				AddPanelString(tempstr);
				break;
			case RSPLTYPE_INVALID:
				break;
			}
		}
	}
	if (MousePosition.x > 190 + PANEL_LEFT && MousePosition.x < 437 + PANEL_LEFT && MousePosition.y > 4 + PANEL_TOP && MousePosition.y < 33 + PANEL_TOP)
		pcursinvitem = CheckInvHLight();

	if (CheckXPBarInfo()) {
		panelflag = true;
	}
}

void CheckBtnUp()
{
	bool gamemenuOff = true;
	drawbtnflag = true;
	panbtndown = false;

	for (int i = 0; i < 8; i++) {
		if (!PanelButtons[i]) {
			continue;
		}

		PanelButtons[i] = false;

		if (MousePosition.x < PanBtnPos[i].x + PANEL_LEFT
		    || MousePosition.x > PanBtnPos[i].x + PANEL_LEFT + PanBtnPos[i].w
		    || MousePosition.y < PanBtnPos[i].y + PANEL_TOP
		    || MousePosition.y > PanBtnPos[i].y + PANEL_TOP + PanBtnPos[i].h) {
			continue;
		}

		switch (i) {
		case PanelButtonCharinfo:
			QuestLogIsOpen = false;
			chrflag = !chrflag;
			break;
		case PanelButtonQlog:
			chrflag = false;
			if (!QuestLogIsOpen)
				StartQuestlog();
			else
				QuestLogIsOpen = false;
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
	pBtmBuff = std::nullopt;
	pManaBuff = std::nullopt;
	pLifeBuff = std::nullopt;
	pSpellCels = std::nullopt;
	pPanelButtons = std::nullopt;
	multiButtons = std::nullopt;
	talkButtons = std::nullopt;
	pChrButtons = std::nullopt;
	pDurIcons = std::nullopt;
	pQLogCel = std::nullopt;
	pSpellBkCel = std::nullopt;
	pSBkBtnCel = std::nullopt;
	pSBkIconCels = std::nullopt;
	pGBoxBuff = std::nullopt;
	FreeCharPanel();
}

void DrawInfoBox(const Surface &out)
{
	DrawPanelBox(out, { 177, 62, 288, 60 }, { PANEL_X + 177, PANEL_Y + 46 });
	if (!panelflag && !trigflag && pcursinvitem == -1 && !spselflag) {
		infostr[0] = '\0';
		InfoColor = UiFlags::ColorSilver;
		ClearPanel();
	}
	if (spselflag || trigflag) {
		InfoColor = UiFlags::ColorSilver;
	} else if (pcurs >= CURSOR_FIRSTITEM) {
		auto &myPlayer = Players[MyPlayerId];
		if (myPlayer.HoldItem._itype == ItemType::Gold) {
			int nGold = myPlayer.HoldItem._ivalue;
			strcpy(infostr, fmt::format(ngettext("{:d} gold piece", "{:d} gold pieces", nGold), nGold).c_str());
		} else if (!myPlayer.HoldItem._iStatFlag) {
			ClearPanel();
			AddPanelString(_("Requirements not met"));
		} else {
			if (myPlayer.HoldItem._iIdentified)
				strcpy(infostr, myPlayer.HoldItem._iIName);
			else
				strcpy(infostr, myPlayer.HoldItem._iName);
			InfoColor = myPlayer.HoldItem.getTextColor();
		}
	} else {
		if (pcursitem != -1)
			GetItemStr(Items[pcursitem]);
		else if (pcursobj != -1)
			GetObjectStr(pcursobj);
		if (pcursmonst != -1) {
			const auto &monster = Monsters[pcursmonst];
			if (leveltype != DTYPE_TOWN) {
				InfoColor = UiFlags::ColorSilver;
				strcpy(infostr, monster.mName);
				ClearPanel();
				if (monster._uniqtype != 0) {
					InfoColor = UiFlags::ColorGold;
					PrintUniqueHistory();
				} else {
					PrintMonstHistory(monster.MType->mtype);
				}
			} else if (pcursitem == -1) {
				string_view townerName = Towners[pcursmonst].name;
				strncpy(infostr, townerName.data(), townerName.length());
				infostr[townerName.length()] = '\0';
			}
		}
		if (pcursplr != -1) {
			InfoColor = UiFlags::ColorGold;
			auto &target = Players[pcursplr];
			strcpy(infostr, target._pName);
			ClearPanel();
			strcpy(tempstr, fmt::format(_("{:s}, Level: {:d}"), _(ClassStrTbl[static_cast<std::size_t>(target._pClass)]), target._pLevel).c_str());
			AddPanelString(tempstr);
			strcpy(tempstr, fmt::format(_("Hit Points {:d} of {:d}"), target._pHitPoints >> 6, target._pMaxHP >> 6).c_str());
			AddPanelString(tempstr);
		}
	}
	if (infostr[0] != '\0' || pnumlines != 0)
		PrintInfo(out);
}

void CheckLvlBtn()
{
	if (!lvlbtndown && MousePosition.x >= 40 + PANEL_LEFT && MousePosition.x <= 81 + PANEL_LEFT && MousePosition.y >= -39 + PANEL_TOP && MousePosition.y <= -17 + PANEL_TOP)
		lvlbtndown = true;
}

void ReleaseLvlBtn()
{
	if (MousePosition.x >= 40 + PANEL_LEFT && MousePosition.x <= 81 + PANEL_LEFT && MousePosition.y >= -39 + PANEL_TOP && MousePosition.y <= -17 + PANEL_TOP) {
		QuestLogIsOpen = false;
		chrflag = true;
	}
	lvlbtndown = false;
}

void DrawLevelUpIcon(const Surface &out)
{
	if (stextflag == STORE_NONE) {
		int nCel = lvlbtndown ? 3 : 2;
		DrawString(out, _("Level Up"), { { PANEL_LEFT + 0, PANEL_TOP - 62 }, { 120, 0 } }, UiFlags::ColorSilver | UiFlags::AlignCenter);
		CelDrawTo(out, { 40 + PANEL_X, -17 + PANEL_Y }, *pChrButtons, nCel);
	}
}

void CheckChrBtns()
{
	auto &myPlayer = Players[MyPlayerId];

	if (chrbtnactive || myPlayer._pStatPts == 0)
		return;

	for (auto attribute : enum_values<CharacterAttribute>()) {
		if (myPlayer.GetBaseAttributeValue(attribute) >= myPlayer.GetMaximumAttributeValue(attribute))
			continue;
		auto buttonId = static_cast<size_t>(attribute);
		Rectangle button = ChrBtnsRect[buttonId];
		button.position = GetPanelPosition(UiPanels::Character, button.position);
		if (button.Contains(MousePosition)) {
			chrbtn[buttonId] = true;
			chrbtnactive = true;
		}
	}
}

void ReleaseChrBtns(bool addAllStatPoints)
{
	chrbtnactive = false;
	for (auto attribute : enum_values<CharacterAttribute>()) {
		auto buttonId = static_cast<size_t>(attribute);
		if (!chrbtn[buttonId])
			continue;

		chrbtn[buttonId] = false;
		Rectangle button = ChrBtnsRect[buttonId];
		button.position = GetPanelPosition(UiPanels::Character, button.position);
		if (button.Contains(MousePosition)) {
			auto &myPlayer = Players[MyPlayerId];
			int statPointsToAdd = 1;
			if (addAllStatPoints)
				statPointsToAdd = CapStatPointsToAdd(myPlayer._pStatPts, myPlayer, attribute);
			switch (attribute) {
			case CharacterAttribute::Strength:
				NetSendCmdParam1(true, CMD_ADDSTR, statPointsToAdd);
				myPlayer._pStatPts -= statPointsToAdd;
				break;
			case CharacterAttribute::Magic:
				NetSendCmdParam1(true, CMD_ADDMAG, statPointsToAdd);
				myPlayer._pStatPts -= statPointsToAdd;
				break;
			case CharacterAttribute::Dexterity:
				NetSendCmdParam1(true, CMD_ADDDEX, statPointsToAdd);
				myPlayer._pStatPts -= statPointsToAdd;
				break;
			case CharacterAttribute::Vitality:
				NetSendCmdParam1(true, CMD_ADDVIT, statPointsToAdd);
				myPlayer._pStatPts -= statPointsToAdd;
				break;
			}
		}
	}
}

void DrawDurIcon(const Surface &out)
{
	bool hasRoomBetweenPanels = gnScreenWidth >= PANEL_WIDTH + 16 + (32 + 8 + 32 + 8 + 32 + 8 + 32) + 16;
	bool hasRoomUnderPanels = gnScreenHeight >= SPANEL_HEIGHT + PANEL_HEIGHT + 16 + 32 + 16;

	if (!hasRoomBetweenPanels && !hasRoomUnderPanels) {
		if ((chrflag || QuestLogIsOpen) && (invflag || sbookflag))
			return;
	}

	int x = PANEL_X + PANEL_WIDTH - 32 - 16;
	if (!hasRoomUnderPanels) {
		if (invflag || sbookflag)
			x -= SPANEL_WIDTH - (gnScreenWidth - PANEL_WIDTH) / 2;
	}

	auto &myPlayer = Players[MyPlayerId];
	x = DrawDurIcon4Item(out, myPlayer.InvBody[INVLOC_HEAD], x, 4);
	x = DrawDurIcon4Item(out, myPlayer.InvBody[INVLOC_CHEST], x, 3);
	x = DrawDurIcon4Item(out, myPlayer.InvBody[INVLOC_HAND_LEFT], x, 0);
	DrawDurIcon4Item(out, myPlayer.InvBody[INVLOC_HAND_RIGHT], x, 0);
}

void RedBack(const Surface &out)
{
	uint8_t *dst = out.begin();
	uint8_t *tbl = &LightTables[4608];
	for (int h = gnViewportHeight; h != 0; h--, dst += out.pitch() - gnScreenWidth) {
		for (int w = gnScreenWidth; w != 0; w--) {
			if (leveltype != DTYPE_HELL || *dst >= 32)
				*dst = tbl[*dst];
			dst++;
		}
	}
}

void DrawSpellBook(const Surface &out)
{
	CelDrawTo(out, GetPanelPosition(UiPanels::Spell, { 0, 351 }), *pSpellBkCel, 1);
	if (gbIsHellfire && sbooktab < 5) {
		CelDrawTo(out, GetPanelPosition(UiPanels::Spell, { 61 * sbooktab + 7, 348 }), *pSBkBtnCel, sbooktab + 1);
	} else {
		// BUGFIX: rendering of page 3 and page 4 buttons are both off-by-one pixel (fixed).
		int sx = 76 * sbooktab + 7;
		if (sbooktab == 2 || sbooktab == 3) {
			sx++;
		}
		CelDrawTo(out, GetPanelPosition(UiPanels::Spell, { sx, 348 }), *pSBkBtnCel, sbooktab + 1);
	}
	auto &myPlayer = Players[MyPlayerId];
	uint64_t spl = myPlayer._pMemSpells | myPlayer._pISpells | myPlayer._pAblSpells;

	int yp = 43;
	for (int i = 1; i < 8; i++) {
		spell_id sn = SpellPages[sbooktab][i - 1];
		if (sn != SPL_INVALID && (spl & GetSpellBitmask(sn)) != 0) {
			spell_type st = GetSBookTrans(sn, true);
			SetSpellTrans(st);
			const Point spellCellPosition = GetPanelPosition(UiPanels::Spell, { 11, yp + 12 });
			DrawSpellCel(out, spellCellPosition, *pSBkIconCels, SpellITbl[sn]);
			if (sn == myPlayer._pRSpell && st == myPlayer._pRSplType) {
				SetSpellTrans(RSPLTYPE_SKILL);
				DrawSpellCel(out, spellCellPosition, *pSBkIconCels, SPLICONLAST);
			}
			PrintSBookStr(out, { 10, yp - 23 }, _(spelldata[sn].sNameText));
			switch (GetSBookTrans(sn, false)) {
			case RSPLTYPE_SKILL:
				strcpy(tempstr, _("Skill"));
				break;
			case RSPLTYPE_CHARGES: {
				int charges = myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges;
				strcpy(tempstr, fmt::format(ngettext("Staff ({:d} charge)", "Staff ({:d} charges)", charges), charges).c_str());
			} break;
			default: {
				int mana = GetManaAmount(myPlayer, sn) >> 6;
				int min;
				int max;
				GetDamageAmt(sn, &min, &max);
				if (min != -1) {
					strcpy(tempstr, fmt::format(_(/* TRANSLATORS: Dam refers to damage. UI constrains, keep short please.*/ "Mana: {:d}  Dam: {:d} - {:d}"), mana, min, max).c_str());
				} else {
					strcpy(tempstr, fmt::format(_(/* TRANSLATORS: Dam refers to damage. UI constrains, keep short please.*/ "Mana: {:d}   Dam: n/a"), mana).c_str());
				}
				if (sn == SPL_BONESPIRIT) {
					strcpy(tempstr, fmt::format(_(/* TRANSLATORS: Dam refers to damage. UI constrains, keep short please.*/ "Mana: {:d}  Dam: 1/3 tgt hp"), mana).c_str());
				}
				PrintSBookStr(out, { 10, yp - 1 }, tempstr);
				int lvl = std::max(myPlayer._pSplLvl[sn] + myPlayer._pISplLvlAdd, 0);
				if (lvl == 0) {
					strcpy(tempstr, _("Spell Level 0 - Unusable"));
				} else {
					strcpy(tempstr, fmt::format(_("Spell Level {:d}"), lvl).c_str());
				}
			} break;
			}
			PrintSBookStr(out, { 10, yp - 12 }, tempstr);
		}
		yp += 43;
	}
}

void CheckSBook()
{
	Rectangle iconArea = { GetPanelPosition(UiPanels::Spell, { 11, 18 }), { 48 - 11, 314 - 18 } };
	Rectangle tabArea = { GetPanelPosition(UiPanels::Spell, { 7, 320 }), { 311 - 7, 349 - 320 } };
	if (iconArea.Contains(MousePosition)) {
		spell_id sn = SpellPages[sbooktab][(MousePosition.y - RightPanel.position.y - 18) / 43];
		auto &myPlayer = Players[MyPlayerId];
		uint64_t spl = myPlayer._pMemSpells | myPlayer._pISpells | myPlayer._pAblSpells;
		if (sn != SPL_INVALID && (spl & GetSpellBitmask(sn)) != 0) {
			spell_type st = RSPLTYPE_SPELL;
			if ((myPlayer._pISpells & GetSpellBitmask(sn)) != 0) {
				st = RSPLTYPE_CHARGES;
			}
			if ((myPlayer._pAblSpells & GetSpellBitmask(sn)) != 0) {
				st = RSPLTYPE_SKILL;
			}
			myPlayer._pRSpell = sn;
			myPlayer._pRSplType = st;
			force_redraw = 255;
		}
	}
	if (tabArea.Contains(MousePosition)) {
		sbooktab = (MousePosition.x - (RightPanel.position.x + 7)) / (gbIsHellfire ? 61 : 76);
	}
}

void DrawGoldSplit(const Surface &out, int amount)
{
	const int dialogX = 30;

	CelDrawTo(out, GetPanelPosition(UiPanels::Inventory, { dialogX, 178 }), *pGBoxBuff, 1);

	constexpr auto BufferSize = sizeof(tempstr) / sizeof(*tempstr);

	// strncpy copies up to the maximum number of characters specified, it does not ensure that a null character is
	// written to the end of the c-string. To be safe we specify a limit one character shorter than the buffer size and
	// ensure that the buffer ends in a null character manually.
	strncpy(
	    tempstr,
	    fmt::format(ngettext(
	                    /* TRANSLATORS: {:d} is a number. Dialog is shown when splitting a stash of Gold.*/ "You have {:d} gold piece. How many do you want to remove?",
	                    "You have {:d} gold pieces. How many do you want to remove?",
	                    initialDropGoldValue),
	        initialDropGoldValue)
	        .c_str(),
	    BufferSize - 1);
	// Ensure the prompt shown to the player is terminated properly (in case the formatted/translated string ends up
	// being longer than 255 characters)
	tempstr[BufferSize - 1] = '\0';

	// Pre-wrap the string at spaces, otherwise DrawString would hard wrap in the middle of words
	WordWrapString(tempstr, 200);

	// The split gold dialog is roughly 4 lines high, but we need at least one line for the player to input an amount.
	// Using a clipping region 50 units high (approx 3 lines with a lineheight of 17) to ensure there is enough room left
	//  for the text entered by the player.
	DrawString(out, tempstr, { GetPanelPosition(UiPanels::Inventory, { dialogX + 31, 75 }), { 200, 50 } }, UiFlags::ColorGold | UiFlags::AlignCenter, 1, 17);

	tempstr[0] = '\0';
	if (amount > 0) {
		// snprintf ensures that the destination buffer ends in a null character.
		snprintf(tempstr, BufferSize, "%u", amount);
	}
	// Even a ten digit amount of gold only takes up about half a line. There's no need to wrap or clip text here so we
	// use the Point form of DrawString.
	DrawString(out, tempstr, GetPanelPosition(UiPanels::Inventory, { dialogX + 37, 128 }), UiFlags::ColorSilver | UiFlags::PentaCursor);
}

void control_drop_gold(char vkey)
{
	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pHitPoints >> 6 <= 0) {
		dropGoldFlag = false;
		dropGoldValue = 0;
		return;
	}

	char input[6];
	memset(input, 0, sizeof(input));
	snprintf(input, sizeof(input), "%i", dropGoldValue);
	if (vkey == DVL_VK_RETURN) {
		if (dropGoldValue > 0)
			RemoveGold(myPlayer, initialDropGoldIndex);
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
		} else {
			input[0] = vkey;
		}
		dropGoldValue = atoi(input);
	}
}

void DrawTalkPan(const Surface &out)
{
	if (!talkflag)
		return;

	DrawPanelBox(out, { 175, sgbPlrTalkTbl + 20, 294, 5 }, { PANEL_X + 175, PANEL_Y + 4 });
	int off = 0;
	for (int i = 293; i > 283; off++, i--) {
		DrawPanelBox(out, { (off / 2) + 175, sgbPlrTalkTbl + off + 25, i, 1 }, { (off / 2) + PANEL_X + 175, off + PANEL_Y + 9 });
	}
	DrawPanelBox(out, { 185, sgbPlrTalkTbl + 35, 274, 30 }, { PANEL_X + 185, PANEL_Y + 19 });
	DrawPanelBox(out, { 180, sgbPlrTalkTbl + 65, 284, 5 }, { PANEL_X + 180, PANEL_Y + 49 });
	for (int i = 0; i < 10; i++) {
		DrawPanelBox(out, { 180, sgbPlrTalkTbl + i + 70, i + 284, 1 }, { PANEL_X + 180, i + PANEL_Y + 54 });
	}
	DrawPanelBox(out, { 170, sgbPlrTalkTbl + 80, 310, 55 }, { PANEL_X + 170, PANEL_Y + 64 });
	char *msg = TalkMessage;

	int x = PANEL_LEFT + 200;
	int y = PANEL_Y + 10;

	int idx = DrawString(out, msg, { { x, y }, { 250, 27 } }, UiFlags::ColorSilver | UiFlags::PentaCursor, 1, 13);
	msg[idx] = '\0';

	x += 46;
	int talkBtn = 0;
	for (int i = 0; i < 4; i++) {
		if (i == MyPlayerId)
			continue;

		UiFlags color = UiFlags::ColorRed;
		const Point talkPanPosition { 172 + PANEL_X, 84 + 18 * talkBtn + PANEL_Y };
		if (WhisperList[i]) {
			color = UiFlags::ColorGold;
			if (TalkButtonsDown[talkBtn]) {
				int nCel = talkBtn != 0 ? 4 : 3;
				CelDrawTo(out, talkPanPosition, *talkButtons, nCel);
			}
		} else {
			int nCel = talkBtn != 0 ? 2 : 1;
			if (TalkButtonsDown[talkBtn])
				nCel += 4;
			CelDrawTo(out, talkPanPosition, *talkButtons, nCel);
		}
		auto &player = Players[i];
		if (player.plractive) {
			DrawString(out, player._pName, { { x, y + 60 + talkBtn * 18 }, { 204, 0 } }, color);
		}

		talkBtn++;
	}
}

bool control_check_talk_btn()
{
	if (!talkflag)
		return false;

	if (MousePosition.x < 172 + PANEL_LEFT)
		return false;
	if (MousePosition.y < 69 + PANEL_TOP)
		return false;
	if (MousePosition.x > 233 + PANEL_LEFT)
		return false;
	if (MousePosition.y > 123 + PANEL_TOP)
		return false;

	for (bool &talkButtonDown : TalkButtonsDown) {
		talkButtonDown = false;
	}

	TalkButtonsDown[(MousePosition.y - (69 + PANEL_TOP)) / 18] = true;

	return true;
}

void control_release_talk_btn()
{
	if (!talkflag)
		return;

	for (bool &talkButtonDown : TalkButtonsDown)
		talkButtonDown = false;

	if (MousePosition.x < 172 + PANEL_LEFT || MousePosition.y < 69 + PANEL_TOP || MousePosition.x > 233 + PANEL_LEFT || MousePosition.y > 123 + PANEL_TOP)
		return;

	int off = (MousePosition.y - (69 + PANEL_TOP)) / 18;

	int p = 0;
	for (; p < MAX_PLRS && off != -1; p++) {
		if (p != MyPlayerId)
			off--;
	}
	if (p <= MAX_PLRS)
		WhisperList[p - 1] = !WhisperList[p - 1];
}

void control_type_message()
{
	if (!IsChatAvailable())
		return;

	talkflag = true;
	TalkMessage[0] = '\0';
	for (bool &talkButtonDown : TalkButtonsDown) {
		talkButtonDown = false;
	}
	sgbPlrTalkTbl = PANEL_HEIGHT + 16;
	force_redraw = 255;
	TalkSaveIndex = NextTalkSave;
}

void control_reset_talk()
{
	talkflag = false;
	sgbPlrTalkTbl = 0;
	force_redraw = 255;
}

bool control_talk_last_key(char vkey)
{
	if (!IsChatAvailable())
		return false;

	if (!talkflag)
		return false;

	if (vkey >= 0 && vkey < DVL_VK_SPACE)
		return false;

	std::size_t result = strlen(TalkMessage);
	if (result < 78) {
		TalkMessage[result] = vkey;
		TalkMessage[result + 1] = '\0';
	}
	return true;
}

bool control_presskeys(int vkey)
{
	if (!IsChatAvailable())
		return false;
	if (!talkflag)
		return false;

	if (vkey == DVL_VK_ESCAPE) {
		control_reset_talk();
	} else if (vkey == DVL_VK_RETURN) {
		ControlPressEnter();
	} else if (vkey == DVL_VK_BACK) {
		std::size_t len = strlen(TalkMessage);
		if (len > 0)
			TalkMessage[len - 1] = '\0';
	} else if (vkey == DVL_VK_DOWN) {
		ControlUpDown(1);
	} else if (vkey == DVL_VK_UP) {
		ControlUpDown(-1);
	} else if (vkey != DVL_VK_SPACE) {
		return false;
	}

	return true;
}

void DiabloHotkeyMsg(uint32_t dwMsg)
{
	if (!IsChatAvailable()) {
		return;
	}

	assert(dwMsg < QUICK_MESSAGE_OPTIONS);

#ifdef _DEBUG
	if (CheckDebugTextCommand(sgOptions.Chat.szHotKeyMsgs[dwMsg]))
		return;
#endif

	NetSendCmdString(0xFFFFFF, sgOptions.Chat.szHotKeyMsgs[dwMsg]);
}

} // namespace devilution
