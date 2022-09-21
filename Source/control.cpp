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

#include "automap.h"
#include "controls/modifier_hints.h"
#include "controls/plrctrls.h"
#include "cursor.h"
#include "engine/clx_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/trn.hpp"
#include "error.h"
#include "gamemenu.h"
#include "init.h"
#include "inv.h"
#include "inv_iterators.hpp"
#include "levels/trigs.h"
#include "lighting.h"
#include "minitext.h"
#include "miniwin/misc_msg.h"
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
#include "utils/format_int.hpp"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/sdl_geometry.h"
#include "utils/stdcompat/optional.hpp"
#include "utils/str_cat.hpp"
#include "utils/string_or_view.hpp"
#include "utils/utf8.hpp"

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
int sbooktab;
int8_t initialDropGoldIndex;
bool talkflag;
bool sbookflag;
bool chrflag;
bool drawbtnflag;
StringOrView InfoString;
bool panelflag;
int initialDropGoldValue;
bool panbtndown;
bool spselflag;
Rectangle MainPanel;
Rectangle LeftPanel;
Rectangle RightPanel;
std::optional<OwnedSurface> pBtmBuff;
OptionalOwnedClxSpriteList pGBoxBuff;

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
bool IsLeftPanelOpen()
{
	return chrflag || QuestLogIsOpen || IsStashOpen;
}
bool IsRightPanelOpen()
{
	return invflag || sbookflag;
}

constexpr Size IncrementAttributeButtonSize { 41, 22 };
/** Maps from attribute_id to the rectangle on screen used for attribute increment buttons. */
Rectangle ChrBtnsRect[4] = {
	{ { 137, 138 }, IncrementAttributeButtonSize },
	{ { 137, 166 }, IncrementAttributeButtonSize },
	{ { 137, 195 }, IncrementAttributeButtonSize },
	{ { 137, 223 }, IncrementAttributeButtonSize }
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
OptionalOwnedClxSpriteList talkButtons;
OptionalOwnedClxSpriteList pDurIcons;
OptionalOwnedClxSpriteList multiButtons;
OptionalOwnedClxSpriteList pPanelButtons;

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

enum panel_button_id : uint8_t {
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
	DrawFlask(out, sourceBuffer, { 13, 3 }, GetMainPanel().position + Displacement { offset, -13 }, emptyPortion);
	if (emptyPortion < 13)
		// Draw the filled part of the flask
		DrawFlask(out, *pBtmBuff, { offset, emptyPortion + 3 }, GetMainPanel().position + Displacement { offset, -13 + emptyPortion }, 13 - emptyPortion);
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
		DrawFlaskTop(out, GetMainPanel().position + Displacement { offset, 0 }, sourceBuffer, 16, 85 - filled);

	// It appears that the panel defaults to having a filled flask and DrawFlaskTop only overlays the appropriate amount of empty space.
	// This draw might not be necessary?
	if (filled > 0)
		DrawPanelBox(out, MakeSdlRect(offset, 85 - filled, 88, filled), GetMainPanel().position + Displacement { offset, 69 - filled });
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

	Rectangle line { GetMainPanel().position + Displacement { 177, LineStart[pnumlines] }, { 288, 12 } };

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
			c = 1;
			break;
		case ItemType::Axe:
			c = 5;
			break;
		case ItemType::Bow:
			c = 6;
			break;
		case ItemType::Mace:
			c = 4;
			break;
		case ItemType::Staff:
			c = 7;
			break;
		case ItemType::Shield:
		default:
			c = 0;
			break;
		}
	}
	if (pItem._iDurability > 2)
		c += 8;
	ClxDraw(out, { x, -17 + GetMainPanel().position.y }, (*pDurIcons)[c]);
	return x - 32 - 8;
}

void ResetTalkMsg()
{
#ifdef _DEBUG
	if (CheckDebugTextCommand(TalkMessage))
		return;
#endif

	uint32_t pmask = 0;

	for (size_t i = 0; i < Players.size(); i++) {
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
	if (player.InvList[gi]._ivalue > 0) {
		SetPlrHandGoldCurs(player.InvList[gi]);
		NetSyncInvItem(player, gi);
	} else {
		player.RemoveInvItem(gi);
	}

	MakeGoldStack(player.HoldItem, dropGoldValue);
	NewCursor(player.HoldItem);

	player._pGold = CalculateGold(player);
	dropGoldValue = 0;
}

bool IsLevelUpButtonVisible()
{
	if (spselflag || chrflag || MyPlayer->_pStatPts == 0) {
		return false;
	}
	if (ControlMode == ControlTypes::VirtualGamepad) {
		return false;
	}
	if (stextflag != STORE_NONE || IsStashOpen) {
		return false;
	}
	if (QuestLogIsOpen && GetLeftPanel().contains(GetMainPanel().position + Displacement { 0, -74 })) {
		return false;
	}

	return true;
}

} // namespace

void CalculatePanelAreas()
{
	constexpr Size MainPanelSize { 640, 128 };

	MainPanel = {
		{ (gnScreenWidth - MainPanelSize.width) / 2, gnScreenHeight - MainPanelSize.height },
		MainPanelSize
	};
	LeftPanel = {
		{ 0, 0 },
		SidePanelSize
	};
	RightPanel = {
		{ 0, 0 },
		SidePanelSize
	};

	if (ControlMode == ControlTypes::VirtualGamepad) {
		LeftPanel.position.x = gnScreenWidth / 2 - LeftPanel.size.width;
	} else {
		if (gnScreenWidth - LeftPanel.size.width - RightPanel.size.width > MainPanel.size.width) {
			LeftPanel.position.x = (gnScreenWidth - LeftPanel.size.width - RightPanel.size.width - MainPanel.size.width) / 2;
		}
	}
	LeftPanel.position.y = (gnScreenHeight - LeftPanel.size.height - MainPanel.size.height) / 2;

	if (ControlMode == ControlTypes::VirtualGamepad) {
		RightPanel.position.x = gnScreenWidth / 2;
	} else {
		RightPanel.position.x = gnScreenWidth - RightPanel.size.width - LeftPanel.position.x;
	}
	RightPanel.position.y = LeftPanel.position.y;

	gnViewportHeight = gnScreenHeight;
	if (gnScreenWidth <= MainPanel.size.width) {
		// Part of the screen is fully obscured by the UI
		gnViewportHeight -= MainPanel.size.height;
	}
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
	if (pnumlines >= 4) {
		Log("AddPanelString failed - not enough lines");
		return;
	}
	CopyUtf8(panelstr[pnumlines], str, sizeof(*panelstr));
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
	DrawFlaskUpper(out, *pLifeBuff, LifeFlaskUpperOffset, MyPlayer->_pHPPer);
}

void DrawManaFlaskUpper(const Surface &out)
{
	constexpr int ManaFlaskUpperOffset = 475;
	DrawFlaskUpper(out, *pManaBuff, ManaFlaskUpperOffset, MyPlayer->_pManaPer);
}

void DrawLifeFlaskLower(const Surface &out)
{
	constexpr int LifeFlaskLowerOffset = 96;
	DrawFlaskLower(out, *pLifeBuff, LifeFlaskLowerOffset, MyPlayer->_pHPPer);
}

void DrawManaFlaskLower(const Surface &out)
{
	constexpr int ManaFlaskLowerOffeset = 464;
	DrawFlaskLower(out, *pManaBuff, ManaFlaskLowerOffeset, MyPlayer->_pManaPer);
}

void DrawFlaskValues(const Surface &out, Point pos, int currValue, int maxValue)
{
	UiFlags color = (currValue > 0 ? (currValue == maxValue ? UiFlags::ColorGold : UiFlags::ColorWhite) : UiFlags::ColorRed);

	auto drawStringWithShadow = [out, color](string_view text, Point pos) {
		DrawString(out, text, pos + Displacement { -1, -1 }, UiFlags::ColorBlack | UiFlags::KerningFitSpacing, 0);
		DrawString(out, text, pos, color | UiFlags::KerningFitSpacing, 0);
	};

	std::string currText = StrCat(currValue);
	drawStringWithShadow(currText, pos - Displacement { GetLineWidth(currText, GameFont12) + 1, 0 });
	drawStringWithShadow("/", pos);
	drawStringWithShadow(StrCat(maxValue), pos + Displacement { GetLineWidth("/", GameFont12) + 1, 0 });
}

void control_update_life_mana()
{
	MyPlayer->UpdateManaPercentage();
	MyPlayer->UpdateHitPointPercentage();
}

void InitControlPan()
{
	if (!HeadlessMode) {
		pBtmBuff.emplace(GetMainPanel().size.width, (GetMainPanel().size.height + 16) * (IsChatAvailable() ? 2 : 1));
		pManaBuff.emplace(88, 88);
		pLifeBuff.emplace(88, 88);

		LoadCharPanel();
		LoadSpellIcons();
		{
			const OwnedClxSpriteList sprite = LoadCel("ctrlpan\\panel8.cel", GetMainPanel().size.width);
			ClxDraw(*pBtmBuff, { 0, (GetMainPanel().size.height + 16) - 1 }, sprite[0]);
		}
		{
			const Point bulbsPosition { 0, 87 };
			const OwnedClxSpriteList statusPanel = LoadCel("ctrlpan\\p8bulbs.cel", 88);
			ClxDraw(*pLifeBuff, bulbsPosition, statusPanel[0]);
			ClxDraw(*pManaBuff, bulbsPosition, statusPanel[1]);
		}
	}
	talkflag = false;
	if (IsChatAvailable()) {
		if (!HeadlessMode) {
			{
				const OwnedClxSpriteList sprite = LoadCel("ctrlpan\\talkpanl.cel", GetMainPanel().size.width);
				ClxDraw(*pBtmBuff, { 0, (GetMainPanel().size.height + 16) * 2 - 1 }, sprite[0]);
			}
			multiButtons = LoadCel("ctrlpan\\p8but2.cel", 33);
			talkButtons = LoadCel("ctrlpan\\talkbutt.cel", 61);
		}
		sgbPlrTalkTbl = 0;
		TalkMessage[0] = '\0';
		for (bool &whisper : WhisperList)
			whisper = true;
		for (bool &talkButtonDown : TalkButtonsDown)
			talkButtonDown = false;
	}
	panelflag = false;
	lvlbtndown = false;
	if (!HeadlessMode) {
		LoadMainPanel();
		pPanelButtons = LoadCel("ctrlpan\\panel8bu.cel", 71);

		static const uint16_t CharButtonsFrameWidths[9] { 95, 41, 41, 41, 41, 41, 41, 41, 41 };
		pChrButtons = LoadCel("data\\charbut.cel", CharButtonsFrameWidths);
	}
	ClearPanBtn();
	if (!IsChatAvailable())
		PanelButtonIndex = 6;
	else
		PanelButtonIndex = 8;
	if (!HeadlessMode)
		pDurIcons = LoadCel("items\\duricons.cel", 32);
	for (bool &buttonEnabled : chrbtn)
		buttonEnabled = false;
	chrbtnactive = false;
	InfoString = {};
	ClearPanel();
	drawhpflag = true;
	drawmanaflag = true;
	chrflag = false;
	spselflag = false;
	sbooktab = 0;
	sbookflag = false;

	if (!HeadlessMode) {
		InitSpellBook();
		pQLogCel = LoadCel("data\\quest.cel", static_cast<uint16_t>(SidePanelSize.width));
		pGBoxBuff = LoadCel("ctrlpan\\golddrop.cel", 261);
	}
	CloseGoldDrop();
	dropGoldValue = 0;
	initialDropGoldValue = 0;
	initialDropGoldIndex = 0;

	CalculatePanelAreas();

	if (!HeadlessMode)
		InitModifierHints();
}

void DrawCtrlPan(const Surface &out)
{
	DrawPanelBox(out, MakeSdlRect(0, sgbPlrTalkTbl + 16, GetMainPanel().size.width, GetMainPanel().size.height), GetMainPanel().position);
	DrawInfoBox(out);
}

void DrawCtrlBtns(const Surface &out)
{
	const Point mainPanelPosition = GetMainPanel().position;
	for (int i = 0; i < 6; i++) {
		if (!PanelButtons[i]) {
			DrawPanelBox(out, MakeSdlRect(PanBtnPos[i].x, PanBtnPos[i].y + 16, 71, 20), mainPanelPosition + Displacement { PanBtnPos[i].x, PanBtnPos[i].y });
		} else {
			Point position = mainPanelPosition + Displacement { PanBtnPos[i].x, PanBtnPos[i].y + 18 };
			ClxDraw(out, position, (*pPanelButtons)[i]);
			RenderClxSprite(out, (*PanelButtonDown)[i], position + Displacement { 4, -18 });
		}
	}

	if (PanelButtonIndex == 8) {
		ClxDraw(out, mainPanelPosition + Displacement { 87, 122 }, (*multiButtons)[PanelButtons[6] ? 1 : 0]);
		if (MyPlayer->friendlyMode)
			ClxDraw(out, mainPanelPosition + Displacement { 527, 122 }, (*multiButtons)[PanelButtons[7] ? 3 : 2]);
		else
			ClxDraw(out, mainPanelPosition + Displacement { 527, 122 }, (*multiButtons)[PanelButtons[7] ? 5 : 4]);
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
	const Point mainPanelPosition = GetMainPanel().position;

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
			Player &myPlayer = *MyPlayer;
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
	const Point mainPanelPosition = GetMainPanel().position;
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
	if (!AutomapActive)
		StartAutomap();
	else
		AutomapActive = false;
}

void CheckPanelInfo()
{
	panelflag = false;
	const Point mainPanelPosition = GetMainPanel().position;
	ClearPanel();
	for (int i = 0; i < PanelButtonIndex; i++) {
		int xend = PanBtnPos[i].x + mainPanelPosition.x + PanBtnPos[i].w;
		int yend = PanBtnPos[i].y + mainPanelPosition.y + PanBtnPos[i].h;
		if (MousePosition.x >= PanBtnPos[i].x + mainPanelPosition.x && MousePosition.x <= xend && MousePosition.y >= PanBtnPos[i].y + mainPanelPosition.y && MousePosition.y <= yend) {
			if (i != 7) {
				InfoString = _(PanBtnStr[i]);
			} else {
				if (MyPlayer->friendlyMode)
					InfoString = _("Player friendly");
				else
					InfoString = _("Player attack");
			}
			if (PanBtnHotKey[i] != nullptr) {
				AddPanelString(fmt::format(fmt::runtime(_("Hotkey: {:s}")), _(PanBtnHotKey[i])));
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
		Player &myPlayer = *MyPlayer;
		const spell_id spellId = myPlayer._pRSpell;
		if (IsValidSpell(spellId)) {
			switch (myPlayer._pRSplType) {
			case RSPLTYPE_SKILL:
				AddPanelString(fmt::format(fmt::runtime(_("{:s} Skill")), pgettext("spell", spelldata[spellId].sNameText)));
				break;
			case RSPLTYPE_SPELL: {
				AddPanelString(fmt::format(fmt::runtime(_("{:s} Spell")), pgettext("spell", spelldata[spellId].sNameText)));
				const int spellLevel = myPlayer.GetSpellLevel(spellId);
				AddPanelString(spellLevel == 0 ? _("Spell Level 0 - Unusable") : fmt::format(fmt::runtime(_("Spell Level {:d}")), spellLevel));
			} break;
			case RSPLTYPE_SCROLL: {
				AddPanelString(fmt::format(fmt::runtime(_("Scroll of {:s}")), pgettext("spell", spelldata[spellId].sNameText)));
				const InventoryAndBeltPlayerItemsRange items { myPlayer };
				const int scrollCount = std::count_if(items.begin(), items.end(), [spellId](const Item &item) {
					return item.isScrollOf(spellId);
				});
				AddPanelString(fmt::format(fmt::runtime(ngettext("{:d} Scroll", "{:d} Scrolls", scrollCount)), scrollCount));
			} break;
			case RSPLTYPE_CHARGES:
				AddPanelString(fmt::format(fmt::runtime(_("Staff of {:s}")), pgettext("spell", spelldata[spellId].sNameText)));
				AddPanelString(fmt::format(fmt::runtime(ngettext("{:d} Charge", "{:d} Charges", myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges)), myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges));
				break;
			case RSPLTYPE_INVALID:
				break;
			}
		}
	}
	if (MousePosition.x > 190 + mainPanelPosition.x && MousePosition.x < 437 + mainPanelPosition.x && MousePosition.y > 4 + mainPanelPosition.y && MousePosition.y < 33 + mainPanelPosition.y)
		pcursinvitem = CheckInvHLight();

	if (CheckXPBarInfo()) {
		panelflag = true;
	}
}

void CheckBtnUp()
{
	bool gamemenuOff = true;
	const Point mainPanelPosition = GetMainPanel().position;

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
			if (dropGoldFlag) {
				CloseGoldDrop();
				dropGoldValue = 0;
			}
			break;
		case PanelButtonSpellbook:
			CloseInventory();
			if (dropGoldFlag) {
				CloseGoldDrop();
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
			// Toggle friendly Mode
			NetSendCmd(true, CMD_FRIENDLYMODE);
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
	DrawPanelBox(out, { 177, 62, 288, 63 }, GetMainPanel().position + Displacement { 177, 46 });
	if (!panelflag && !trigflag && pcursinvitem == -1 && pcursstashitem == uint16_t(-1) && !spselflag) {
		InfoString = {};
		InfoColor = UiFlags::ColorWhite;
		ClearPanel();
	}
	Player &myPlayer = *MyPlayer;
	if (spselflag || trigflag) {
		InfoColor = UiFlags::ColorWhite;
	} else if (!myPlayer.HoldItem.isEmpty()) {
		if (myPlayer.HoldItem._itype == ItemType::Gold) {
			int nGold = myPlayer.HoldItem._ivalue;
			InfoString = fmt::format(fmt::runtime(ngettext("{:s} gold piece", "{:s} gold pieces", nGold)), FormatInteger(nGold));
		} else if (!myPlayer.CanUseItem(myPlayer.HoldItem)) {
			InfoString = _("Requirements not met");
		} else {
			if (myPlayer.HoldItem._iIdentified)
				InfoString = string_view(myPlayer.HoldItem._iIName);
			else
				InfoString = string_view(myPlayer.HoldItem._iName);
			InfoColor = myPlayer.HoldItem.getTextColor();
		}
	} else {
		if (pcursitem != -1)
			GetItemStr(Items[pcursitem]);
		else if (ObjectUnderCursor != nullptr)
			GetObjectStr(*ObjectUnderCursor);
		if (pcursmonst != -1) {
			if (leveltype != DTYPE_TOWN) {
				const auto &monster = Monsters[pcursmonst];
				InfoColor = UiFlags::ColorWhite;
				InfoString = monster.name();
				ClearPanel();
				if (monster.isUnique()) {
					InfoColor = UiFlags::ColorWhitegold;
					PrintUniqueHistory();
				} else {
					PrintMonstHistory(monster.type().type);
				}
			} else if (pcursitem == -1) {
				InfoString = string_view(Towners[pcursmonst].name);
			}
		}
		if (pcursplr != -1) {
			InfoColor = UiFlags::ColorWhitegold;
			auto &target = Players[pcursplr];
			InfoString = string_view(target._pName);
			ClearPanel();
			AddPanelString(fmt::format(fmt::runtime(_("{:s}, Level: {:d}")), _(ClassStrTbl[static_cast<std::size_t>(target._pClass)]), target._pLevel));
			AddPanelString(fmt::format(fmt::runtime(_("Hit Points {:d} of {:d}")), target._pHitPoints >> 6, target._pMaxHP >> 6));
		}
	}
	if (!InfoString.empty() || pnumlines != 0)
		PrintInfo(out);
}

void CheckLvlBtn()
{
	if (!IsLevelUpButtonVisible()) {
		return;
	}

	const Point mainPanelPosition = GetMainPanel().position;
	if (!lvlbtndown && MousePosition.x >= 40 + mainPanelPosition.x && MousePosition.x <= 81 + mainPanelPosition.x && MousePosition.y >= -39 + mainPanelPosition.y && MousePosition.y <= -17 + mainPanelPosition.y)
		lvlbtndown = true;
}

void ReleaseLvlBtn()
{
	const Point mainPanelPosition = GetMainPanel().position;
	if (MousePosition.x >= 40 + mainPanelPosition.x && MousePosition.x <= 81 + mainPanelPosition.x && MousePosition.y >= -39 + mainPanelPosition.y && MousePosition.y <= -17 + mainPanelPosition.y) {
		QuestLogIsOpen = false;
		CloseGoldWithdraw();
		IsStashOpen = false;
		chrflag = true;
	}
	lvlbtndown = false;
}

void DrawLevelUpIcon(const Surface &out)
{
	if (IsLevelUpButtonVisible()) {
		int nCel = lvlbtndown ? 2 : 1;
		DrawString(out, _("Level Up"), { GetMainPanel().position + Displacement { 0, -62 }, { 120, 0 } }, UiFlags::ColorWhite | UiFlags::AlignCenter);
		ClxDraw(out, GetMainPanel().position + Displacement { 40, -17 }, (*pChrButtons)[nCel]);
	}
}

void CheckChrBtns()
{
	Player &myPlayer = *MyPlayer;

	if (chrbtnactive || myPlayer._pStatPts == 0)
		return;

	for (auto attribute : enum_values<CharacterAttribute>()) {
		if (myPlayer.GetBaseAttributeValue(attribute) >= myPlayer.GetMaximumAttributeValue(attribute))
			continue;
		auto buttonId = static_cast<size_t>(attribute);
		Rectangle button = ChrBtnsRect[buttonId];
		button.position = GetPanelPosition(UiPanels::Character, button.position);
		if (button.contains(MousePosition)) {
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
		if (button.contains(MousePosition)) {
			Player &myPlayer = *MyPlayer;
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
		if (IsLeftPanelOpen() && IsRightPanelOpen())
			return;
	}

	int x = MainPanel.position.x + MainPanel.size.width - 32 - 16;
	if (!hasRoomUnderPanels) {
		if (IsRightPanelOpen() && MainPanel.position.x + MainPanel.size.width > RightPanel.position.x)
			x -= MainPanel.position.x + MainPanel.size.width - RightPanel.position.x;
	}

	Player &myPlayer = *MyPlayer;
	x = DrawDurIcon4Item(out, myPlayer.InvBody[INVLOC_HEAD], x, 3);
	x = DrawDurIcon4Item(out, myPlayer.InvBody[INVLOC_CHEST], x, 2);
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
	const int dialogX = 30;

	ClxDraw(out, GetPanelPosition(UiPanels::Inventory, { dialogX, 178 }), (*pGBoxBuff)[0]);

	const std::string description = fmt::format(
	    fmt::runtime(ngettext(
	        /* TRANSLATORS: {:s} is a number with separators. Dialog is shown when splitting a stash of Gold.*/
	        "You have {:s} gold piece. How many do you want to remove?",
	        "You have {:s} gold pieces. How many do you want to remove?",
	        initialDropGoldValue)),
	    FormatInteger(initialDropGoldValue));

	// Pre-wrap the string at spaces, otherwise DrawString would hard wrap in the middle of words
	const std::string wrapped = WordWrapString(description, 200);

	// The split gold dialog is roughly 4 lines high, but we need at least one line for the player to input an amount.
	// Using a clipping region 50 units high (approx 3 lines with a lineheight of 17) to ensure there is enough room left
	//  for the text entered by the player.
	DrawString(out, wrapped, { GetPanelPosition(UiPanels::Inventory, { dialogX + 31, 75 }), { 200, 50 } }, UiFlags::ColorWhitegold | UiFlags::AlignCenter, 1, 17);

	std::string value;
	if (amount > 0) {
		value = StrCat(amount);
	}
	// Even a ten digit amount of gold only takes up about half a line. There's no need to wrap or clip text here so we
	// use the Point form of DrawString.
	DrawString(out, value, GetPanelPosition(UiPanels::Inventory, { dialogX + 37, 128 }), UiFlags::ColorWhite | UiFlags::PentaCursor);
}

void control_drop_gold(SDL_Keycode vkey)
{
	Player &myPlayer = *MyPlayer;

	if (myPlayer._pHitPoints >> 6 <= 0) {
		CloseGoldDrop();
		dropGoldValue = 0;
		return;
	}

	if (vkey == SDLK_RETURN || vkey == SDLK_KP_ENTER) {
		if (dropGoldValue > 0)
			RemoveGold(myPlayer, initialDropGoldIndex);
		CloseGoldDrop();
	} else if (vkey == SDLK_ESCAPE) {
		CloseGoldDrop();
		dropGoldValue = 0;
	} else if (vkey == SDLK_BACKSPACE) {
		dropGoldValue = dropGoldValue / 10;
	}
}

void DrawTalkPan(const Surface &out)
{
	if (!talkflag)
		return;

	force_redraw = 255;

	const Point mainPanelPosition = GetMainPanel().position;

	DrawPanelBox(out, MakeSdlRect(175, sgbPlrTalkTbl + 20, 294, 5), mainPanelPosition + Displacement { 175, 4 });
	int off = 0;
	for (int i = 293; i > 283; off++, i--) {
		DrawPanelBox(out, MakeSdlRect((off / 2) + 175, sgbPlrTalkTbl + off + 25, i, 1), mainPanelPosition + Displacement { (off / 2) + 175, off + 9 });
	}
	DrawPanelBox(out, MakeSdlRect(185, sgbPlrTalkTbl + 35, 274, 30), mainPanelPosition + Displacement { 185, 19 });
	DrawPanelBox(out, MakeSdlRect(180, sgbPlrTalkTbl + 65, 284, 5), mainPanelPosition + Displacement { 180, 49 });
	for (int i = 0; i < 10; i++) {
		DrawPanelBox(out, MakeSdlRect(180, sgbPlrTalkTbl + i + 70, i + 284, 1), mainPanelPosition + Displacement { 180, i + 54 });
	}
	DrawPanelBox(out, MakeSdlRect(170, sgbPlrTalkTbl + 80, 310, 55), mainPanelPosition + Displacement { 170, 64 });

	int x = mainPanelPosition.x + 200;
	int y = mainPanelPosition.y + 10;

	uint32_t idx = DrawString(out, TalkMessage, { { x, y }, { 250, 27 } }, UiFlags::ColorWhite | UiFlags::PentaCursor, 1, 13);
	if (idx < sizeof(TalkMessage))
		TalkMessage[idx] = '\0';

	x += 46;
	int talkBtn = 0;
	for (size_t i = 0; i < Players.size(); i++) {
		Player &player = Players[i];
		if (&player == MyPlayer)
			continue;

		UiFlags color = player.friendlyMode ? UiFlags::ColorWhitegold : UiFlags::ColorRed;
		const Point talkPanPosition = mainPanelPosition + Displacement { 172, 84 + 18 * talkBtn };
		if (WhisperList[i]) {
			if (TalkButtonsDown[talkBtn]) {
				ClxDraw(out, talkPanPosition, (*talkButtons)[talkBtn != 0 ? 3 : 2]);
				RenderClxSprite(out, (*TalkButton)[2], talkPanPosition + Displacement { 4, -15 });
			}
		} else {
			int nCel = talkBtn != 0 ? 1 : 0;
			if (TalkButtonsDown[talkBtn])
				nCel += 4;
			ClxDraw(out, talkPanPosition, (*talkButtons)[nCel]);
			RenderClxSprite(out, (*TalkButton)[TalkButtonsDown[talkBtn] ? 1 : 0], talkPanPosition + Displacement { 4, -15 });
		}
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

	const Point mainPanelPosition = GetMainPanel().position;

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

	const Point mainPanelPosition = GetMainPanel().position;

	if (MousePosition.x < 172 + mainPanelPosition.x || MousePosition.y < 69 + mainPanelPosition.y || MousePosition.x > 233 + mainPanelPosition.x || MousePosition.y > 123 + mainPanelPosition.y)
		return;

	int off = (MousePosition.y - (69 + mainPanelPosition.y)) / 18;

	size_t playerId = 0;
	for (; playerId < Players.size() && off != -1; ++playerId) {
		if (playerId != MyPlayerId)
			off--;
	}
	if (playerId > 0 && playerId <= Players.size())
		WhisperList[playerId - 1] = !WhisperList[playerId - 1];
}

void control_type_message()
{
	if (!IsChatAvailable())
		return;

	talkflag = true;
	SDL_Rect rect = MakeSdlRect(GetMainPanel().position.x + 200, GetMainPanel().position.y + 22, 250, 39);
	SDL_SetTextInputRect(&rect);
	TalkMessage[0] = '\0';
	for (bool &talkButtonDown : TalkButtonsDown) {
		talkButtonDown = false;
	}
	sgbPlrTalkTbl = GetMainPanel().size.height + 16;
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

bool control_presskeys(SDL_Keycode vkey)
{
	if (!IsChatAvailable())
		return false;
	if (!talkflag)
		return false;

	if (vkey == SDLK_ESCAPE) {
		control_reset_talk();
	} else if (vkey == SDLK_RETURN || vkey == SDLK_KP_ENTER) {
		ControlPressEnter();
	} else if (vkey == SDLK_BACKSPACE) {
		TalkMessage[FindLastUtf8Symbols(TalkMessage)] = '\0';
	} else if (vkey == SDLK_DOWN) {
		ControlUpDown(1);
	} else if (vkey == SDLK_UP) {
		ControlUpDown(-1);
	} else if (vkey != SDLK_SPACE) {
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
	if (!dropGoldFlag)
		return;
	dropGoldFlag = false;
	SDL_StopTextInput();
}

void GoldDropNewText(string_view text)
{
	for (char vkey : text) {
		int digit = vkey - '0';
		if (digit >= 0 && digit <= 9) {
			int newGoldValue = dropGoldValue * 10;
			newGoldValue += digit;
			if (newGoldValue <= initialDropGoldValue) {
				dropGoldValue = newGoldValue;
			}
		}
	}
}

} // namespace devilution
