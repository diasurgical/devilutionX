/**
 * @file control.cpp
 *
 * Implementation of the character and main control panels
 */
#include "control.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>

#include <fmt/format.h>

#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"
#include "automap.h"
#include "controls/modifier_hints.h"
#include "controls/plrctrls.h"
#include "cursor.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/trn.hpp"
#include "error.h"
#include "gamemenu.h"
#include "init.h"
#include "inv.h"
#include "inv_iterators.hpp"
#include "lighting.h"
#include "minitext.h"
#include "missiles.h"
#include "options.h"
#include "panels/charpanel.hpp"
#include "panels/mainpanel.hpp"
#include "panels/spell_book.hpp"
#include "panels/spell_icons.hpp"
#include "panels/spell_list.hpp"
#include "qol/stash.h"
#include "qol/xpbar.h"
#include "stores.h"
#include "towners.h"
#include "trigs.h"
#include "utils/language.h"
#include "utils/sdl_geometry.h"
#include "utils/stdcompat/optional.hpp"
#include "utils/utf8.hpp"

#ifdef _DEBUG
#include "debug.h"
#endif

namespace devilution {

/**
 * @brief Set if the life flask needs to be redrawn during next frame
 */
bool drawhpflag;
bool IsDropGoldOpen;
bool chrbtn[4];
int DropGoldValue;
/**
 * @brief Set if the mana flask needs to be redrawn during the next frame
 */
bool drawmanaflag;
bool chrbtnactive;
int pnumlines;
UiFlags InfoColor;
int sbooktab;
int8_t InitialDropGoldIndex;
bool talkflag;
bool sbookflag;
bool chrflag;
bool drawbtnflag;
std::string InfoString;
bool panelflag;
int InitialDropGoldValue;
bool panbtndown;
bool spselflag;
Rectangle MainPanel;
Rectangle LeftPanel;
Rectangle RightPanel;
std::optional<OwnedSurface> pBtmBuff;
std::optional<OwnedCelSprite> pGBoxBuff;

const Rectangle &GetMainPanel()
{
	return MainPanel;
}
const Rectangle &GetLeftPanel()
{
	return LeftPanel;
}
const Rectangle &GetRightPanel()
{
	return RightPanel;
}

/** Maps from attribute_id to the rectangle on screen used for attribute increment buttons. */
Rectangle ChrBtnsRect[4] = {
	{ { 137, 138 }, { 41, 22 } },
	{ { 137, 166 }, { 41, 22 } },
	{ { 137, 195 }, { 41, 22 } },
	{ { 137, 223 }, { 41, 22 } }
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

namespace {

std::optional<OwnedSurface> pLifeBuff;
std::optional<OwnedSurface> pManaBuff;
std::optional<OwnedCelSprite> talkButtons;
std::optional<OwnedCelSprite> pDurIcons;
std::optional<OwnedCelSprite> multiButtons;
std::optional<OwnedCelSprite> pPanelButtons;

bool IsLevelUpButtonPressed;
bool PanelButtons[8];
int PanelButtonIndex;
char TalkSave[8][MAX_SEND_STR_LEN];
uint8_t TalkSaveIndex;
uint8_t NextTalkSave;
char TalkMessage[MAX_SEND_STR_LEN];
bool TalkButtonsDown[3];
int sgbPlrTalkTbl;
bool WhisperList[MAX_PLRS];
char panelstr[4][64];

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
	out.BlitFrom(celBuf, MakeSdlRect(0, static_cast<decltype(SDL_Rect {}.y)>(y0), celBuf.w(), y1 - y0), position);
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
		DrawPanelBox(out, MakeSdlRect(offset, 85 - filled, 88, filled), { PANEL_X + offset, PANEL_Y + 69 - filled });
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

	if (!InfoString.empty()) {
		DrawString(out, InfoString, line, InfoColor | UiFlags::AlignCenter | UiFlags::KerningFitSpacing, 2);
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
	player.InvList[gi]._ivalue -= DropGoldValue;
	if (player.InvList[gi]._ivalue > 0)
		SetPlrHandGoldCurs(player.InvList[gi]);
	else
		player.RemoveInvItem(gi);

	MakeGoldStack(player.HoldItem, DropGoldValue);
	NewCursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);

	player._pGold = CalculateGold(player);
	DropGoldValue = 0;
}

bool IsLevelUpButtonVisible()
{
	if (spselflag || chrflag || Players[MyPlayerId]._pStatPts == 0) {
		return false;
	}
	if (ControlMode == ControlTypes::VirtualGamepad) {
		return false;
	}
	if (stextflag != STORE_NONE || IsStashOpen) {
		return false;
	}
	if (QuestLogIsOpen && GetLeftPanel().Contains(GetMainPanel().position + Displacement { 0, -74 })) {
		return false;
	}

	return true;
}

} // namespace

void CalculatePanelAreas()
{
	MainPanel = {
		{ (gnScreenWidth - PANEL_WIDTH) / 2, gnScreenHeight - PANEL_HEIGHT },
		{ PANEL_WIDTH, PANEL_HEIGHT }
	};
	LeftPanel = {
		{ 0, 0 },
		{ SPANEL_WIDTH, SPANEL_HEIGHT }
	};
	RightPanel = {
		{ 0, 0 },
		{ SPANEL_WIDTH, SPANEL_HEIGHT }
	};

	if (ControlMode == ControlTypes::VirtualGamepad) {
		LeftPanel.position.x = gnScreenWidth / 2 - LeftPanel.size.width;
	} else {
		if (gnScreenWidth - LeftPanel.size.width - RightPanel.size.width > PANEL_WIDTH) {
			LeftPanel.position.x = (gnScreenWidth - LeftPanel.size.width - RightPanel.size.width - PANEL_WIDTH) / 2;
		}
	}
	LeftPanel.position.y = (gnScreenHeight - LeftPanel.size.height - PANEL_HEIGHT) / 2;

	if (ControlMode == ControlTypes::VirtualGamepad) {
		RightPanel.position.x = gnScreenWidth / 2;
	} else {
		RightPanel.position.x = gnScreenWidth - RightPanel.size.width - LeftPanel.position.x;
	}
	RightPanel.position.y = LeftPanel.position.y;
}

bool IsChatAvailable()
{
#ifdef _DEBUG
	return true;
#else
	return gbIsMultiplayer;
#endif
}

void AddPanelString(string_view str)
{
	CopyUtf8(panelstr[pnumlines], str, sizeof(*panelstr));

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
		return GetMainPanel().position + displacement;
	case UiPanels::Quest:
	case UiPanels::Character:
	case UiPanels::Stash:
		return GetLeftPanel().position + displacement;
	case UiPanels::Spell:
	case UiPanels::Inventory:
		return GetRightPanel().position + displacement;
	default:
		return GetMainPanel().position + displacement;
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

void DrawFlaskValues(const Surface &out, Point pos, int currValue, int maxValue)
{
	UiFlags color = (currValue > 0 ? (currValue == maxValue ? UiFlags::ColorGold : UiFlags::ColorWhite) : UiFlags::ColorRed);

	auto drawStringWithShadow = [out, color](string_view text, Point pos) {
		DrawString(out, text, pos + Displacement { -1, -1 }, UiFlags::ColorBlack | UiFlags::KerningFitSpacing, 0);
		DrawString(out, text, pos, color | UiFlags::KerningFitSpacing, 0);
	};

	std::string currText = fmt::format("{:d}", currValue);
	drawStringWithShadow(currText, pos - Displacement { GetLineWidth(currText, GameFont12) + 1, 0 });
	drawStringWithShadow("/", pos);
	drawStringWithShadow(fmt::format("{:d}", maxValue), pos + Displacement { GetLineWidth("/", GameFont12) + 1, 0 });
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
	LoadSpellIcons();
	CelDrawUnsafeTo(*pBtmBuff, { 0, (PANEL_HEIGHT + 16) - 1 }, LoadCel("CtrlPan\\Panel8.CEL", PANEL_WIDTH), 1);
	{
		const Point bulbsPosition { 0, 87 };
		const OwnedCelSprite statusPanel = LoadCel("CtrlPan\\P8Bulbs.CEL", 88);
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
	LoadMainPanel();
	panelflag = false;
	IsLevelUpButtonPressed = false;
	pPanelButtons = LoadCel("CtrlPan\\Panel8bu.CEL", 71);
	ClearPanBtn();
	if (!IsChatAvailable())
		PanelButtonIndex = 6;
	else
		PanelButtonIndex = 8;
	pChrButtons = LoadCel("Data\\CharBut.CEL", 41);
	for (bool &buttonEnabled : chrbtn)
		buttonEnabled = false;
	chrbtnactive = false;
	pDurIcons = LoadCel("Items\\DurIcons.CEL", 32);
	InfoString.clear();
	ClearPanel();
	drawhpflag = true;
	drawmanaflag = true;
	chrflag = false;
	spselflag = false;
	sbooktab = 0;
	sbookflag = false;

	InitSpellBook();
	pQLogCel = LoadCel("Data\\Quest.CEL", SPANEL_WIDTH);
	pGBoxBuff = LoadCel("CtrlPan\\Golddrop.cel", 261);
	CloseGoldDrop();
	DropGoldValue = 0;
	InitialDropGoldValue = 0;
	InitialDropGoldIndex = 0;

	CalculatePanelAreas();

	InitModifierHints();
}

void DrawCtrlPan(const Surface &out)
{
	DrawPanelBox(out, MakeSdlRect(0, sgbPlrTalkTbl + 16, PANEL_WIDTH, PANEL_HEIGHT), { PANEL_X, PANEL_Y });
	DrawInfoBox(out);
}

void DrawCtrlBtns(const Surface &out)
{
	for (int i = 0; i < 6; i++) {
		if (!PanelButtons[i]) {
			DrawPanelBox(out, MakeSdlRect(PanBtnPos[i].x, PanBtnPos[i].y + 16, 71, 20), { PanBtnPos[i].x + PANEL_X, PanBtnPos[i].y + PANEL_Y });
		} else {
			Point position { PanBtnPos[i].x + PANEL_X, PanBtnPos[i].y + PANEL_Y + 18 };
			CelDrawTo(out, position, *pPanelButtons, i + 1);
			DrawArt(out, position + Displacement { 4, -18 }, &PanelButtonDown, i);
		}
	}
	if (PanelButtonIndex == 8) {
		CelDrawTo(out, { 87 + PANEL_X, 122 + PANEL_Y }, *multiButtons, PanelButtons[6] ? 2 : 1);
		if (gbFriendlyMode)
			CelDrawTo(out, { 527 + PANEL_X, 122 + PANEL_Y }, *multiButtons, PanelButtons[7] ? 4 : 3);
		else
			CelDrawTo(out, { 527 + PANEL_X, 122 + PANEL_Y }, *multiButtons, PanelButtons[7] ? 6 : 5);
	}
}

void ClearPanBtn()
{
	for (bool &panelButton : PanelButtons)
		panelButton = false;
	drawbtnflag = true;
	panbtndown = false;
}

void DoPanBtn()
{
	auto &mainPanelPosition = GetMainPanel().position;

	for (int i = 0; i < PanelButtonIndex; i++) {
		int x = PanBtnPos[i].x + mainPanelPosition.x + PanBtnPos[i].w;
		int y = PanBtnPos[i].y + mainPanelPosition.y + PanBtnPos[i].h;
		if (MousePosition.x >= PanBtnPos[i].x + mainPanelPosition.x && MousePosition.x <= x) {
			if (MousePosition.y >= PanBtnPos[i].y + mainPanelPosition.y && MousePosition.y <= y) {
				PanelButtons[i] = true;
				drawbtnflag = true;
				panbtndown = true;
			}
		}
	}
	if (!spselflag && MousePosition.x >= 565 + mainPanelPosition.x && MousePosition.x < 621 + mainPanelPosition.x && MousePosition.y >= 64 + mainPanelPosition.y && MousePosition.y < 120 + mainPanelPosition.y) {
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
	auto &mainPanelPosition = GetMainPanel().position;
	int x = PanBtnPos[3].x + mainPanelPosition.x + PanBtnPos[3].w;
	int y = PanBtnPos[3].y + mainPanelPosition.y + PanBtnPos[3].h;
	if (MousePosition.x >= PanBtnPos[3].x + mainPanelPosition.x
	    && MousePosition.x <= x
	    && MousePosition.y >= PanBtnPos[3].y + mainPanelPosition.y
	    && MousePosition.y <= y) {
		SetButtonStateDown(3);
	}
	x = PanBtnPos[6].x + mainPanelPosition.x + PanBtnPos[6].w;
	y = PanBtnPos[6].y + mainPanelPosition.y + PanBtnPos[6].h;
	if (MousePosition.x >= PanBtnPos[6].x + mainPanelPosition.x
	    && MousePosition.x <= x
	    && MousePosition.y >= PanBtnPos[6].y + mainPanelPosition.y
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
	auto &mainPanelPosition = GetMainPanel().position;
	ClearPanel();
	for (int i = 0; i < PanelButtonIndex; i++) {
		int xend = PanBtnPos[i].x + mainPanelPosition.x + PanBtnPos[i].w;
		int yend = PanBtnPos[i].y + mainPanelPosition.y + PanBtnPos[i].h;
		if (MousePosition.x >= PanBtnPos[i].x + mainPanelPosition.x && MousePosition.x <= xend && MousePosition.y >= PanBtnPos[i].y + mainPanelPosition.y && MousePosition.y <= yend) {
			if (i != 7) {
				InfoString = _(PanBtnStr[i]);
			} else {
				if (gbFriendlyMode)
					InfoString = _("Player friendly");
				else
					InfoString = _("Player attack");
			}
			if (PanBtnHotKey[i] != nullptr) {
				AddPanelString(fmt::format(_("Hotkey: {:s}"), _(PanBtnHotKey[i])));
			}
			InfoColor = UiFlags::ColorWhite;
			panelflag = true;
		}
	}
	if (!spselflag && MousePosition.x >= 565 + mainPanelPosition.x && MousePosition.x < 621 + mainPanelPosition.x && MousePosition.y >= 64 + mainPanelPosition.y && MousePosition.y < 120 + mainPanelPosition.y) {
		InfoString = _("Select current spell button");
		InfoColor = UiFlags::ColorWhite;
		panelflag = true;
		AddPanelString(_("Hotkey: 's'"));
		auto &myPlayer = Players[MyPlayerId];
		const spell_id spellId = myPlayer._pRSpell;
		if (spellId != SPL_INVALID && spellId != SPL_NULL) {
			switch (myPlayer._pRSplType) {
			case RSPLTYPE_SKILL:
				AddPanelString(fmt::format(_("{:s} Skill"), pgettext("spell", spelldata[spellId].sSkillText)));
				break;
			case RSPLTYPE_SPELL: {
				AddPanelString(fmt::format(_("{:s} Spell"), pgettext("spell", spelldata[spellId].sNameText)));
				int c = std::max(myPlayer._pISplLvlAdd + myPlayer._pSplLvl[spellId], 0);
				AddPanelString(c == 0 ? _("Spell Level 0 - Unusable") : fmt::format(_("Spell Level {:d}"), c));
			} break;
			case RSPLTYPE_SCROLL: {
				AddPanelString(fmt::format(_("Scroll of {:s}"), pgettext("spell", spelldata[spellId].sNameText)));
				const InventoryAndBeltPlayerItemsRange items { myPlayer };
				const int scrollCount = std::count_if(items.begin(), items.end(), [spellId](const Item &item) {
					return item.IsScrollOf(spellId);
				});
				AddPanelString(fmt::format(ngettext("{:d} Scroll", "{:d} Scrolls", scrollCount), scrollCount));
			} break;
			case RSPLTYPE_CHARGES:
				AddPanelString(fmt::format(_("Staff of {:s}"), pgettext("spell", spelldata[spellId].sNameText)));
				AddPanelString(fmt::format(ngettext("{:d} Charge", "{:d} Charges", myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges), myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges));
				break;
			case RSPLTYPE_INVALID:
				break;
			}
		}
	}
	if (MousePosition.x > 190 + mainPanelPosition.x && MousePosition.x < 437 + mainPanelPosition.x && MousePosition.y > 4 + mainPanelPosition.y && MousePosition.y < 33 + mainPanelPosition.y)
		pcursinvitem = CheckInvHLight(MousePosition);

	if (CheckXPBarInfo()) {
		panelflag = true;
	}
}

void CheckBtnUp()
{
	bool gamemenuOff = true;
	auto &mainPanelPosition = GetMainPanel().position;

	drawbtnflag = true;
	panbtndown = false;

	for (int i = 0; i < 8; i++) {
		if (!PanelButtons[i]) {
			continue;
		}

		PanelButtons[i] = false;

		if (MousePosition.x < PanBtnPos[i].x + mainPanelPosition.x
		    || MousePosition.x > PanBtnPos[i].x + mainPanelPosition.x + PanBtnPos[i].w
		    || MousePosition.y < PanBtnPos[i].y + mainPanelPosition.y
		    || MousePosition.y > PanBtnPos[i].y + mainPanelPosition.y + PanBtnPos[i].h) {
			continue;
		}

		switch (i) {
		case PanelButtonCharinfo:
			QuestLogIsOpen = false;
			CloseGoldWithdraw();
			IsStashOpen = false;
			chrflag = !chrflag;
			break;
		case PanelButtonQlog:
			chrflag = false;
			CloseGoldWithdraw();
			IsStashOpen = false;
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
			CloseGoldWithdraw();
			IsStashOpen = false;
			invflag = !invflag;
			if (IsDropGoldOpen) {
				CloseGoldDrop();
				DropGoldValue = 0;
			}
			break;
		case PanelButtonSpellbook:
			CloseInventory();
			if (IsDropGoldOpen) {
				CloseGoldDrop();
				DropGoldValue = 0;
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
	FreeSpellIcons();
	FreeSpellBook();
	pPanelButtons = std::nullopt;
	multiButtons = std::nullopt;
	talkButtons = std::nullopt;
	pChrButtons = std::nullopt;
	pDurIcons = std::nullopt;
	pQLogCel = std::nullopt;
	pGBoxBuff = std::nullopt;
	FreeMainPanel();
	FreeCharPanel();
	FreeModifierHints();
}

void DrawInfoBox(const Surface &out)
{
	DrawPanelBox(out, { 177, 62, 288, 60 }, { PANEL_X + 177, PANEL_Y + 46 });
	if (!panelflag && !trigflag && pcursinvitem == -1 && pcursstashitem == uint16_t(-1) && !spselflag) {
		InfoString.clear();
		InfoColor = UiFlags::ColorWhite;
		ClearPanel();
	}
	if (spselflag || trigflag) {
		InfoColor = UiFlags::ColorWhite;
	} else if (pcurs >= CURSOR_FIRSTITEM) {
		auto &myPlayer = Players[MyPlayerId];
		if (myPlayer.HoldItem._itype == ItemType::Gold) {
			int nGold = myPlayer.HoldItem._ivalue;
			InfoString = fmt::format(ngettext("{:d} gold piece", "{:d} gold pieces", nGold), nGold);
		} else if (!myPlayer.CanUseItem(myPlayer.HoldItem)) {
			ClearPanel();
			AddPanelString(_("Requirements not met"));
		} else {
			if (myPlayer.HoldItem._iIdentified)
				InfoString = myPlayer.HoldItem._iIName;
			else
				InfoString = myPlayer.HoldItem._iName;
			InfoColor = myPlayer.HoldItem.getTextColor();
		}
	} else {
		if (pcursitem != -1)
			GetItemStr(Items[pcursitem]);
		else if (pcursobj != -1)
			GetObjectStr(Objects[pcursobj]);
		if (pcursmonst != -1) {
			if (leveltype != DTYPE_TOWN) {
				const auto &monster = Monsters[pcursmonst];
				InfoColor = UiFlags::ColorWhite;
				InfoString = monster.mName;
				ClearPanel();
				if (monster._uniqtype != 0) {
					InfoColor = UiFlags::ColorWhitegold;
					PrintUniqueHistory();
				} else {
					PrintMonstHistory(monster.MType->mtype);
				}
			} else if (pcursitem == -1) {
				InfoString = std::string(Towners[pcursmonst].name);
			}
		}
		if (pcursplr != -1) {
			InfoColor = UiFlags::ColorWhitegold;
			auto &target = Players[pcursplr];
			InfoString = target._pName;
			ClearPanel();
			AddPanelString(fmt::format(_("{:s}, Level: {:d}"), _(ClassStrTbl[static_cast<std::size_t>(target._pClass)]), target._pLevel));
			AddPanelString(fmt::format(_("Hit Points {:d} of {:d}"), target._pHitPoints >> 6, target._pMaxHP >> 6));
		}
	}
	if (!InfoString.empty() || pnumlines != 0)
		PrintInfo(out);
}

bool CheckLevelUpButtonPress()
{
	if (!IsLevelUpButtonVisible()) {
		return false;
	}

	Rectangle button = { GetPanelPosition(UiPanels::Main, { 40, -39 }), { 41, 22 } };
	if (button.Contains(MousePosition)) {
		IsLevelUpButtonPressed = true;
	}

	return IsLevelUpButtonPressed;
}

void CheckLevelUpButtonRelease()
{
	if (!IsLevelUpButtonPressed) {
		return;
	}

	Rectangle button = { GetPanelPosition(UiPanels::Main, { 40, -39 }), { 41, 22 } };
	if (button.Contains(MousePosition)) {
		QuestLogIsOpen = false;
		CloseGoldWithdraw();
		IsStashOpen = false;
		chrflag = true;
	}
	IsLevelUpButtonPressed = false;
}

void DrawLevelUpButton(const Surface &out)
{
	if (!IsLevelUpButtonVisible()) {
		return;
	}

	DrawString(out, _("Level Up"), { { PANEL_LEFT + 0, PANEL_TOP - 62 }, { 120, 0 } }, UiFlags::ColorWhite | UiFlags::AlignCenter);
	CelDrawTo(out, { 40 + PANEL_X, -17 + PANEL_Y }, *pChrButtons, IsLevelUpButtonPressed ? 3 : 2);
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
	bool hasRoomBetweenPanels = RightPanel.position.x - (LeftPanel.position.x + LeftPanel.size.width) >= 16 + (32 + 8 + 32 + 8 + 32 + 8 + 32) + 16;
	bool hasRoomUnderPanels = MainPanel.position.y - (RightPanel.position.y + RightPanel.size.height) >= 16 + 32 + 16;

	if (!hasRoomBetweenPanels && !hasRoomUnderPanels) {
		if ((chrflag || QuestLogIsOpen || IsStashOpen) && (invflag || sbookflag))
			return;
	}

	int x = MainPanel.position.x + MainPanel.size.width - 32 - 16;
	if (!hasRoomUnderPanels) {
		if ((invflag || sbookflag) && MainPanel.position.x + MainPanel.size.width > RightPanel.position.x)
			x -= MainPanel.position.x + MainPanel.size.width - RightPanel.position.x;
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
	uint8_t *tbl = GetPauseTRN();
	for (int h = gnViewportHeight; h != 0; h--, dst += out.pitch() - gnScreenWidth) {
		for (int w = gnScreenWidth; w != 0; w--) {
			if (leveltype != DTYPE_HELL || *dst >= 32)
				*dst = tbl[*dst];
			dst++;
		}
	}
}

void DrawGoldSplit(const Surface &out, int amount)
{
	if (!IsDropGoldOpen) {
		return;
	}

	const int dialogX = 30;

	CelDrawTo(out, GetPanelPosition(UiPanels::Inventory, { dialogX, 178 }), *pGBoxBuff, 1);

	const std::string description = fmt::format(
	    /* TRANSLATORS: {:d} is a number. Dialog is shown when splitting a stash of Gold.*/
	    ngettext(
	        "You have {:d} gold piece. How many do you want to remove?",
	        "You have {:d} gold pieces. How many do you want to remove?",
	        InitialDropGoldValue),
	    InitialDropGoldValue);

	// Pre-wrap the string at spaces, otherwise DrawString would hard wrap in the middle of words
	const std::string wrapped = WordWrapString(description, 200);

	// The split gold dialog is roughly 4 lines high, but we need at least one line for the player to input an amount.
	// Using a clipping region 50 units high (approx 3 lines with a lineheight of 17) to ensure there is enough room left
	//  for the text entered by the player.
	DrawString(out, wrapped, { GetPanelPosition(UiPanels::Inventory, { dialogX + 31, 75 }), { 200, 50 } }, UiFlags::ColorWhitegold | UiFlags::AlignCenter, 1, 17);

	std::string value = "";
	if (amount > 0) {
		value = fmt::format("{:d}", amount);
	}
	// Even a ten digit amount of gold only takes up about half a line. There's no need to wrap or clip text here so we
	// use the Point form of DrawString.
	DrawString(out, value, GetPanelPosition(UiPanels::Inventory, { dialogX + 37, 128 }), UiFlags::ColorWhite | UiFlags::PentaCursor);
}

void DropGoldKeyPress(char vkey)
{
	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pHitPoints >> 6 <= 0) {
		CloseGoldDrop();
		DropGoldValue = 0;
		return;
	}

	if (vkey == DVL_VK_RETURN) {
		if (DropGoldValue > 0)
			RemoveGold(myPlayer, InitialDropGoldIndex);
		CloseGoldDrop();
	} else if (vkey == DVL_VK_ESCAPE) {
		CloseGoldDrop();
		DropGoldValue = 0;
	} else if (vkey == DVL_VK_BACK) {
		DropGoldValue /= 10;
	}
}

void DrawTalkPan(const Surface &out)
{
	if (!talkflag)
		return;

	force_redraw = 255;

	DrawPanelBox(out, MakeSdlRect(175, sgbPlrTalkTbl + 20, 294, 5), { PANEL_X + 175, PANEL_Y + 4 });
	int off = 0;
	for (int i = 293; i > 283; off++, i--) {
		DrawPanelBox(out, MakeSdlRect((off / 2) + 175, sgbPlrTalkTbl + off + 25, i, 1), { (off / 2) + PANEL_X + 175, off + PANEL_Y + 9 });
	}
	DrawPanelBox(out, MakeSdlRect(185, sgbPlrTalkTbl + 35, 274, 30), { PANEL_X + 185, PANEL_Y + 19 });
	DrawPanelBox(out, MakeSdlRect(180, sgbPlrTalkTbl + 65, 284, 5), { PANEL_X + 180, PANEL_Y + 49 });
	for (int i = 0; i < 10; i++) {
		DrawPanelBox(out, MakeSdlRect(180, sgbPlrTalkTbl + i + 70, i + 284, 1), { PANEL_X + 180, i + PANEL_Y + 54 });
	}
	DrawPanelBox(out, MakeSdlRect(170, sgbPlrTalkTbl + 80, 310, 55), { PANEL_X + 170, PANEL_Y + 64 });

	int x = PANEL_LEFT + 200;
	int y = PANEL_Y + 10;

	uint32_t idx = DrawString(out, TalkMessage, { { x, y }, { 250, 27 } }, UiFlags::ColorWhite | UiFlags::PentaCursor, 1, 13);
	if (idx < sizeof(TalkMessage))
		TalkMessage[idx] = '\0';

	x += 46;
	int talkBtn = 0;
	for (int i = 0; i < 4; i++) {
		if (i == MyPlayerId)
			continue;

		UiFlags color = UiFlags::ColorRed;
		const Point talkPanPosition { 172 + PANEL_X, 84 + 18 * talkBtn + PANEL_Y };
		if (WhisperList[i]) {
			color = UiFlags::ColorWhitegold;
			if (TalkButtonsDown[talkBtn]) {
				int nCel = talkBtn != 0 ? 4 : 3;
				CelDrawTo(out, talkPanPosition, *talkButtons, nCel);
				DrawArt(out, talkPanPosition + Displacement { 4, -15 }, &TalkButton, 2);
			}
		} else {
			int nCel = talkBtn != 0 ? 2 : 1;
			if (TalkButtonsDown[talkBtn])
				nCel += 4;
			CelDrawTo(out, talkPanPosition, *talkButtons, nCel);
			DrawArt(out, talkPanPosition + Displacement { 4, -15 }, &TalkButton, TalkButtonsDown[talkBtn] ? 1 : 0);
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

	auto &mainPanelPosition = GetMainPanel().position;

	if (MousePosition.x < 172 + mainPanelPosition.x)
		return false;
	if (MousePosition.y < 69 + mainPanelPosition.y)
		return false;
	if (MousePosition.x > 233 + mainPanelPosition.x)
		return false;
	if (MousePosition.y > 123 + mainPanelPosition.y)
		return false;

	for (bool &talkButtonDown : TalkButtonsDown) {
		talkButtonDown = false;
	}

	TalkButtonsDown[(MousePosition.y - (69 + mainPanelPosition.y)) / 18] = true;

	return true;
}

void control_release_talk_btn()
{
	if (!talkflag)
		return;

	for (bool &talkButtonDown : TalkButtonsDown)
		talkButtonDown = false;

	auto &mainPanelPosition = GetMainPanel().position;

	if (MousePosition.x < 172 + mainPanelPosition.x || MousePosition.y < 69 + mainPanelPosition.y || MousePosition.x > 233 + mainPanelPosition.x || MousePosition.y > 123 + mainPanelPosition.y)
		return;

	int off = (MousePosition.y - (69 + mainPanelPosition.y)) / 18;

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
	SDL_Rect rect = MakeSdlRect(PANEL_LEFT + 200, PANEL_Y + 22, 250, 39);
	SDL_SetTextInputRect(&rect);
	TalkMessage[0] = '\0';
	for (bool &talkButtonDown : TalkButtonsDown) {
		talkButtonDown = false;
	}
	sgbPlrTalkTbl = PANEL_HEIGHT + 16;
	force_redraw = 255;
	TalkSaveIndex = NextTalkSave;
	SDL_StartTextInput();
}

void control_reset_talk()
{
	talkflag = false;
	SDL_StopTextInput();
	sgbPlrTalkTbl = 0;
	force_redraw = 255;
}

bool IsTalkActive()
{
	if (!IsChatAvailable())
		return false;

	if (!talkflag)
		return false;

	return true;
}

void control_new_text(string_view text)
{
	strncat(TalkMessage, text.data(), sizeof(TalkMessage) - strlen(TalkMessage) - 1);
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
		TalkMessage[FindLastUtf8Symbols(TalkMessage)] = '\0';
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

	for (auto &msg : sgOptions.Chat.szHotKeyMsgs[dwMsg]) {

#ifdef _DEBUG
		if (CheckDebugTextCommand(msg))
			continue;
#endif
		char charMsg[MAX_SEND_STR_LEN];
		CopyUtf8(charMsg, msg, sizeof(charMsg));
		NetSendCmdString(0xFFFFFF, charMsg);
	}
}

void CloseGoldDrop()
{
	if (!IsDropGoldOpen)
		return;
	IsDropGoldOpen = false;
	SDL_StopTextInput();
}

void GoldDropNewText(string_view text)
{
	for (char vkey : text) {
		int digit = vkey - '0';
		if (digit >= 0 && digit <= 9) {
			int newGoldValue = DropGoldValue * 10;
			newGoldValue += digit;
			if (newGoldValue <= InitialDropGoldValue) {
				DropGoldValue = newGoldValue;
			}
		}
	}
}

} // namespace devilution
