/**
 * @file stores.cpp
 *
 * Implementation of functionality for stores and towner dialogs.
 */
#include "stores.h"

#include <algorithm>
#include <cstdint>
#include <string_view>

#include <fmt/format.h>

#include "controls/plrctrls.h"
#include "cursor.h"
#include "engine/backbuffer_state.hpp"
#include "engine/load_cel.hpp"
#include "engine/random.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/trn.hpp"
#include "init.h"
#include "minitext.h"
#include "options.h"
#include "panels/info_box.hpp"
#include "qol/stash.h"
#include "towners.h"
#include "utils/format_int.hpp"
#include "utils/language.h"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

namespace devilution {

TalkID activeStore;

int currentItemIndex;
int8_t playerItemIndexes[48];
Item playerItem[48];

Item smithItem[SMITH_ITEMS];
int numPremiumItems;
int premiumItemLevel;
Item premiumItem[SMITH_PREMIUM_ITEMS];

Item healerItem[20];

Item witchItem[WITCH_ITEMS];

int boyItemLevel;
Item boyItem;

namespace {

/** The current towner being interacted with */
_talker_id townerId;

/** Is the current dialog full size */
bool isTextFullSize;

/** Number of text lines in the current dialog */
int numTextLines;
/** Remember currently selected text line from textLine while displaying a dialog */
int oldTextLine;
/** Currently selected text line from textLine */
int currentTextLine;

struct STextStruct {
	enum Type : uint8_t {
		Label,
		Divider,
		Selectable,
	};

	std::string text;
	int _sval;
	int y;
	UiFlags flags;
	Type type;
	uint8_t _sx;
	uint8_t _syoff;
	int cursId;
	bool cursIndent;

	[[nodiscard]] bool isDivider() const
	{
		return type == Divider;
	}
	[[nodiscard]] bool isSelectable() const
	{
		return type == Selectable;
	}

	[[nodiscard]] bool hasText() const
	{
		return !text.empty();
	}
};

/** Text lines */
STextStruct textLine[STORE_LINES];

/** Whether to render the player's gold amount in the top left */
bool renderGold;

/** Does the current panel have a scrollbar */
bool hasScrollbar;
/** Remember last scroll position */
int oldScrollPos;
/** Scroll position */
int scrollPos;
/** Next scroll position */
int nextScrollPos;
/** Previous scroll position */
int previousScrollPos;
/** Countdown for the push state of the scroll up button */
int8_t countdownScrollUp;
/** Countdown for the push state of the scroll down button */
int8_t countdownScrollDown;

/** Remember current store while displaying a dialog */
TalkID oldActiveStore;

/** Temporary item used to hold the item being traded */
Item tempItem;

/** Maps from towner IDs to NPC names. */
const char *const TownerNames[] = {
	N_("Griswold"),
	N_("Pepin"),
	"",
	N_("Ogden"),
	N_("Cain"),
	N_("Farnham"),
	N_("Adria"),
	N_("Gillian"),
	N_("Wirt"),
};

constexpr int PaddingTop = 32;

// For most languages, line height is always 12.
// This includes blank lines and divider line.
constexpr int SmallLineHeight = 12;
constexpr int SmallTextHeight = 12;

// For larger small fonts (Chinese and Japanese), text lines are
// taller and overflow.
// We space out blank lines a bit more to give space to 3-line store items.
constexpr int LargeLineHeight = SmallLineHeight + 1;
constexpr int LargeTextHeight = 18;

/**
 * The line index with the Back / Leave button.
 * This is a special button that is always the last line.
 *
 * For lists with a scrollbar, it is not selectable (mouse-only).
 */
int BackButtonLine()
{
	if (IsSmallFontTall()) {
		return hasScrollbar ? 21 : 20;
	}
	return 22;
}

int LineHeight()
{
	return IsSmallFontTall() ? LargeLineHeight : SmallLineHeight;
}

int TextHeight()
{
	return IsSmallFontTall() ? LargeTextHeight : SmallTextHeight;
}

void CalculateLineHeights()
{
	textLine[0].y = 0;
	if (IsSmallFontTall()) {
		for (int i = 1; i < STORE_LINES; ++i) {
			// Space out consecutive text lines, unless they are both selectable (never the case currently).
			if (textLine[i].hasText() && textLine[i - 1].hasText() && !(textLine[i].isSelectable() && textLine[i - 1].isSelectable())) {
				textLine[i].y = textLine[i - 1].y + LargeTextHeight;
			} else {
				textLine[i].y = i * LargeLineHeight;
			}
		}
	} else {
		for (int i = 1; i < STORE_LINES; ++i) {
			textLine[i].y = i * SmallLineHeight;
		}
	}
}

void DrawSTextBack(const Surface &out)
{
	const Point uiPosition = GetUIRectangle().position;
	ClxDraw(out, { uiPosition.x + 320 + 24, 327 + uiPosition.y }, (*pSTextBoxCels)[0]);
	DrawHalfTransparentRectTo(out, uiPosition.x + 347, uiPosition.y + 28, 265, 297);
}

void DrawSSlider(const Surface &out, int y1, int y2)
{
	const Point uiPosition = GetUIRectangle().position;
	int yd1 = y1 * 12 + 44 + uiPosition.y;
	int yd2 = y2 * 12 + 44 + uiPosition.y;
	if (countdownScrollUp != -1)
		ClxDraw(out, { uiPosition.x + 601, yd1 }, (*pSTextSlidCels)[11]);
	else
		ClxDraw(out, { uiPosition.x + 601, yd1 }, (*pSTextSlidCels)[9]);
	if (countdownScrollDown != -1)
		ClxDraw(out, { uiPosition.x + 601, yd2 }, (*pSTextSlidCels)[10]);
	else
		ClxDraw(out, { uiPosition.x + 601, yd2 }, (*pSTextSlidCels)[8]);
	yd1 += 12;
	int yd3 = yd1;
	for (; yd3 < yd2; yd3 += 12) {
		ClxDraw(out, { uiPosition.x + 601, yd3 }, (*pSTextSlidCels)[13]);
	}
	if (currentTextLine == BackButtonLine())
		yd3 = oldTextLine;
	else
		yd3 = currentTextLine;
	if (currentItemIndex > 1)
		yd3 = 1000 * (scrollPos + ((yd3 - previousScrollPos) / 4)) / (currentItemIndex - 1) * (y2 * 12 - y1 * 12 - 24) / 1000;
	else
		yd3 = 0;
	ClxDraw(out, { uiPosition.x + 601, (y1 + 1) * 12 + 44 + uiPosition.y + yd3 }, (*pSTextSlidCels)[12]);
}

void AddSLine(size_t y)
{
	textLine[y]._sx = 0;
	textLine[y]._syoff = 0;
	textLine[y].text.clear();
	textLine[y].text.shrink_to_fit();
	textLine[y].type = STextStruct::Divider;
	textLine[y].cursId = -1;
	textLine[y].cursIndent = false;
}

void AddSTextVal(size_t y, int val)
{
	textLine[y]._sval = val;
}

void AddSText(uint8_t x, size_t y, std::string_view text, UiFlags flags, bool sel, int cursId = -1, bool cursIndent = false)
{
	textLine[y]._sx = x;
	textLine[y]._syoff = 0;
	textLine[y].text.clear();
	textLine[y].text.append(text);
	textLine[y].flags = flags;
	textLine[y].type = sel ? STextStruct::Selectable : STextStruct::Label;
	textLine[y].cursId = cursId;
	textLine[y].cursIndent = cursIndent;
}

void AddOptionsBackButton()
{
	const int line = BackButtonLine();
	AddSText(0, line, _("Back"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	textLine[line]._syoff = IsSmallFontTall() ? 0 : 6;
}

void AddItemListBackButton(bool selectable = false)
{
	const int line = BackButtonLine();
	std::string_view text = _("Back");
	if (!selectable && IsSmallFontTall()) {
		AddSText(0, line, text, UiFlags::ColorWhite | UiFlags::AlignRight, selectable);
	} else {
		AddSLine(line - 1);
		AddSText(0, line, text, UiFlags::ColorWhite | UiFlags::AlignCenter, selectable);
		textLine[line]._syoff = 6;
	}
}

void PrintStoreItem(const Item &item, int l, UiFlags flags, bool cursIndent = false)
{
	std::string productLine;

	if (item._iIdentified) {
		if (item._iMagical != ITEM_QUALITY_UNIQUE) {
			if (item._iPrePower != -1) {
				productLine.append(PrintItemPower(item._iPrePower, item));
			}
		}
		if (item._iSufPower != -1) {
			if (!productLine.empty())
				productLine.append(_(",  "));
			productLine.append(PrintItemPower(item._iSufPower, item));
		}
	}
	if (item._iMiscId == IMISC_STAFF && item._iMaxCharges != 0) {
		if (!productLine.empty())
			productLine.append(_(",  "));
		productLine.append(fmt::format(fmt::runtime(_("Charges: {:d}/{:d}")), item._iCharges, item._iMaxCharges));
	}
	if (!productLine.empty()) {
		AddSText(40, l, productLine, flags, false, -1, cursIndent);
		l++;
		productLine.clear();
	}

	if (item._itype != ItemType::Misc) {
		if (item._iClass == ICLASS_WEAPON)
			productLine = fmt::format(fmt::runtime(_("Damage: {:d}-{:d}  ")), item._iMinDam, item._iMaxDam);
		else if (item._iClass == ICLASS_ARMOR)
			productLine = fmt::format(fmt::runtime(_("Armor: {:d}  ")), item._iAC);
		if (item._iMaxDur != DUR_INDESTRUCTIBLE && item._iMaxDur != 0)
			productLine += fmt::format(fmt::runtime(_("Dur: {:d}/{:d}")), item._iDurability, item._iMaxDur);
		else
			productLine.append(_("Indestructible"));
	}

	int8_t str = item._iMinStr;
	uint8_t mag = item._iMinMag;
	int8_t dex = item._iMinDex;

	if (str != 0 || mag != 0 || dex != 0) {
		if (!productLine.empty())
			productLine.append(_(",  "));
		productLine.append(_("Required:"));
		if (str != 0)
			productLine.append(fmt::format(fmt::runtime(_(" {:d} Str")), str));
		if (mag != 0)
			productLine.append(fmt::format(fmt::runtime(_(" {:d} Mag")), mag));
		if (dex != 0)
			productLine.append(fmt::format(fmt::runtime(_(" {:d} Dex")), dex));
	}
	AddSText(40, l++, productLine, flags, false, -1, cursIndent);
}

bool StoreAutoPlace(Item &item, bool persistItem)
{
	Player &player = *MyPlayer;

	if (AutoEquipEnabled(player, item) && AutoEquip(player, item, persistItem, true)) {
		return true;
	}

	if (AutoPlaceItemInBelt(player, item, persistItem, true)) {
		return true;
	}

	return AutoPlaceItemInInventory(player, item, persistItem, true);
}

void ScrollVendorStore(Item *itemData, int storeLimit, int idx, int selling = true)
{
	ClearSText(5, 21);
	previousScrollPos = 5;

	for (int l = 5; l < 20 && idx < storeLimit; l += 4) {
		const Item &item = itemData[idx];
		if (!item.isEmpty()) {
			UiFlags itemColor = item.getTextColorWithStatCheck();
			AddSText(20, l, item.getName(), itemColor, true, item._iCurs, true);
			AddSTextVal(l, item._iIdentified ? item._iIvalue : item._ivalue);
			PrintStoreItem(item, l + 1, itemColor, true);
			nextScrollPos = l;
		} else {
			l -= 4;
		}
		idx++;
	}
	if (selling) {
		if (currentTextLine != -1 && !textLine[currentTextLine].isSelectable() && currentTextLine != BackButtonLine())
			currentTextLine = nextScrollPos;
	} else {
		numTextLines = std::max(static_cast<int>(storeLimit) - 4, 0);
	}
}

void StartSmith()
{
	isTextFullSize = false;
	hasScrollbar = false;
	AddSText(0, 1, _("Welcome to the"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 3, _("Blacksmith's shop"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 7, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 10, _("Talk to Griswold"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 12, _("Buy basic items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Buy premium items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 16, _("Sell items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Repair items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 20, _("Leave the shop"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	currentItemIndex = 20;
}

void ScrollSmithBuy(int idx)
{
	ScrollVendorStore(smithItem, static_cast<int>(std::size(smithItem)), idx);
}

uint32_t TotalPlayerGold()
{
	return MyPlayer->_pGold + Stash.gold;
}

// TODO: Change `_iIvalue` to be unsigned instead of passing `int` here.
bool PlayerCanAfford(int price)
{
	return TotalPlayerGold() >= static_cast<uint32_t>(price);
}

void StartSmithBuy()
{
	isTextFullSize = true;
	hasScrollbar = true;
	scrollPos = 0;

	renderGold = true;
	AddSText(20, 1, _("I have these items for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollSmithBuy(scrollPos);
	AddItemListBackButton();

	currentItemIndex = 0;
	for (Item &item : smithItem) {
		if (item.isEmpty())
			continue;

		item._iStatFlag = MyPlayer->CanUseItem(item);
		currentItemIndex++;
	}

	numTextLines = std::max(currentItemIndex - 4, 0);
}

void ScrollSmithPremiumBuy(int boughtitems)
{
	int idx = 0;
	for (; boughtitems != 0; idx++) {
		if (!premiumItem[idx].isEmpty())
			boughtitems--;
	}

	ScrollVendorStore(premiumItem, static_cast<int>(std::size(premiumItem)), idx);
}

bool StartSmithPremiumBuy()
{
	currentItemIndex = 0;
	for (Item &item : premiumItem) {
		if (item.isEmpty())
			continue;

		item._iStatFlag = MyPlayer->CanUseItem(item);
		currentItemIndex++;
	}
	if (currentItemIndex == 0) {
		StartStore(TalkID::Smith);
		currentTextLine = 14;
		return false;
	}

	isTextFullSize = true;
	hasScrollbar = true;
	scrollPos = 0;

	renderGold = true;
	AddSText(20, 1, _("I have these premium items for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	AddItemListBackButton();

	numTextLines = std::max(currentItemIndex - 4, 0);

	ScrollSmithPremiumBuy(scrollPos);

	return true;
}

bool SmithSellOk(int i)
{
	Item *pI;

	if (i >= 0) {
		pI = &MyPlayer->InvList[i];
	} else {
		pI = &MyPlayer->SpdList[-(i + 1)];
	}

	if (pI->isEmpty())
		return false;

	if (pI->_iMiscId > IMISC_OILFIRST && pI->_iMiscId < IMISC_OILLAST)
		return true;

	if (pI->_itype == ItemType::Misc)
		return false;
	if (pI->_itype == ItemType::Gold)
		return false;
	if (pI->_itype == ItemType::Staff && (!gbIsHellfire || IsValidSpell(pI->_iSpell)))
		return false;
	if (pI->_iClass == ICLASS_QUEST)
		return false;
	if (pI->IDidx == IDI_LAZSTAFF)
		return false;

	return true;
}

void ScrollSmithSell(int idx)
{
	ScrollVendorStore(playerItem, currentItemIndex, idx, false);
}

void StartSmithSell()
{
	isTextFullSize = true;
	bool sellOk = false;
	currentItemIndex = 0;

	for (auto &item : playerItem) {
		item.clear();
	}

	const Player &myPlayer = *MyPlayer;

	for (int8_t i = 0; i < myPlayer._pNumInv; i++) {
		if (currentItemIndex >= 48)
			break;
		if (SmithSellOk(i)) {
			sellOk = true;
			playerItem[currentItemIndex] = myPlayer.InvList[i];

			if (playerItem[currentItemIndex]._iMagical != ITEM_QUALITY_NORMAL && playerItem[currentItemIndex]._iIdentified)
				playerItem[currentItemIndex]._ivalue = playerItem[currentItemIndex]._iIvalue;

			playerItem[currentItemIndex]._ivalue = std::max(playerItem[currentItemIndex]._ivalue / 4, 1);
			playerItem[currentItemIndex]._iIvalue = playerItem[currentItemIndex]._ivalue;
			playerItemIndexes[currentItemIndex] = i;
			currentItemIndex++;
		}
	}

	for (int i = 0; i < MaxBeltItems; i++) {
		if (currentItemIndex >= 48)
			break;
		if (SmithSellOk(-(i + 1))) {
			sellOk = true;
			playerItem[currentItemIndex] = myPlayer.SpdList[i];

			if (playerItem[currentItemIndex]._iMagical != ITEM_QUALITY_NORMAL && playerItem[currentItemIndex]._iIdentified)
				playerItem[currentItemIndex]._ivalue = playerItem[currentItemIndex]._iIvalue;

			playerItem[currentItemIndex]._ivalue = std::max(playerItem[currentItemIndex]._ivalue / 4, 1);
			playerItem[currentItemIndex]._iIvalue = playerItem[currentItemIndex]._ivalue;
			playerItemIndexes[currentItemIndex] = -(i + 1);
			currentItemIndex++;
		}
	}

	if (!sellOk) {
		hasScrollbar = false;

		renderGold = true;
		AddSText(20, 1, _("You have nothing I want."), UiFlags::ColorWhitegold, false);
		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	hasScrollbar = true;
	scrollPos = 0;
	numTextLines = myPlayer._pNumInv;

	renderGold = true;
	AddSText(20, 1, _("Which item is for sale?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollSmithSell(scrollPos);
	AddItemListBackButton();
}

bool SmithRepairOk(int i)
{
	const Player &myPlayer = *MyPlayer;
	const Item &item = myPlayer.InvList[i];

	if (item.isEmpty())
		return false;
	if (item._itype == ItemType::Misc)
		return false;
	if (item._itype == ItemType::Gold)
		return false;
	if (item._iDurability == item._iMaxDur)
		return false;
	if (item._iMaxDur == DUR_INDESTRUCTIBLE)
		return false;

	return true;
}

void StartSmithRepair()
{
	isTextFullSize = true;
	currentItemIndex = 0;

	for (auto &item : playerItem) {
		item.clear();
	}

	Player &myPlayer = *MyPlayer;

	auto &helmet = myPlayer.InvBody[INVLOC_HEAD];
	if (!helmet.isEmpty() && helmet._iDurability != helmet._iMaxDur) {
		AddStoreHoldRepair(&helmet, -1);
	}

	auto &armor = myPlayer.InvBody[INVLOC_CHEST];
	if (!armor.isEmpty() && armor._iDurability != armor._iMaxDur) {
		AddStoreHoldRepair(&armor, -2);
	}

	auto &leftHand = myPlayer.InvBody[INVLOC_HAND_LEFT];
	if (!leftHand.isEmpty() && leftHand._iDurability != leftHand._iMaxDur) {
		AddStoreHoldRepair(&leftHand, -3);
	}

	auto &rightHand = myPlayer.InvBody[INVLOC_HAND_RIGHT];
	if (!rightHand.isEmpty() && rightHand._iDurability != rightHand._iMaxDur) {
		AddStoreHoldRepair(&rightHand, -4);
	}

	for (int i = 0; i < myPlayer._pNumInv; i++) {
		if (currentItemIndex >= 48)
			break;
		if (SmithRepairOk(i)) {
			AddStoreHoldRepair(&myPlayer.InvList[i], i);
		}
	}

	if (currentItemIndex == 0) {
		hasScrollbar = false;

		renderGold = true;
		AddSText(20, 1, _("You have nothing to repair."), UiFlags::ColorWhitegold, false);
		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	hasScrollbar = true;
	scrollPos = 0;
	numTextLines = myPlayer._pNumInv;

	renderGold = true;
	AddSText(20, 1, _("Repair which item?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);

	ScrollSmithSell(scrollPos);
	AddItemListBackButton();
}

void FillManaPlayer()
{
	if (!*sgOptions.Gameplay.adriaRefillsMana)
		return;

	Player &myPlayer = *MyPlayer;

	if (myPlayer._pMana != myPlayer._pMaxMana) {
		PlaySFX(SfxID::CastHealing);
	}
	myPlayer._pMana = myPlayer._pMaxMana;
	myPlayer._pManaBase = myPlayer._pMaxManaBase;
	RedrawComponent(PanelDrawComponent::Mana);
}

void StartWitch()
{
	FillManaPlayer();
	isTextFullSize = false;
	hasScrollbar = false;
	AddSText(0, 2, _("Witch's shack"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Adria"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Buy items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 16, _("Sell items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Recharge staves"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 20, _("Leave the shack"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	currentItemIndex = 20;
}

void ScrollWitchBuy(int idx)
{
	ScrollVendorStore(witchItem, static_cast<int>(std::size(witchItem)), idx);
}

void WitchBookLevel(Item &bookItem)
{
	if (bookItem._iMiscId != IMISC_BOOK)
		return;
	bookItem._iMinMag = GetSpellData(bookItem._iSpell).minInt;
	uint8_t spellLevel = MyPlayer->_pSplLvl[static_cast<int8_t>(bookItem._iSpell)];
	while (spellLevel > 0) {
		bookItem._iMinMag += 20 * bookItem._iMinMag / 100;
		spellLevel--;
		if (bookItem._iMinMag + 20 * bookItem._iMinMag / 100 > 255) {
			bookItem._iMinMag = 255;
			spellLevel = 0;
		}
	}
}

void StartWitchBuy()
{
	isTextFullSize = true;
	hasScrollbar = true;
	scrollPos = 0;
	numTextLines = 20;

	renderGold = true;
	AddSText(20, 1, _("I have these items for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollWitchBuy(scrollPos);
	AddItemListBackButton();

	currentItemIndex = 0;
	for (Item &item : witchItem) {
		if (item.isEmpty())
			continue;

		WitchBookLevel(item);
		item._iStatFlag = MyPlayer->CanUseItem(item);
		currentItemIndex++;
	}
	numTextLines = std::max(currentItemIndex - 4, 0);
}

bool WitchSellOk(int i)
{
	Item *pI;

	bool rv = false;

	if (i >= 0)
		pI = &MyPlayer->InvList[i];
	else
		pI = &MyPlayer->SpdList[-(i + 1)];

	if (pI->_itype == ItemType::Misc)
		rv = true;
	if (pI->_iMiscId > 29 && pI->_iMiscId < 41)
		rv = false;
	if (pI->_iClass == ICLASS_QUEST)
		rv = false;
	if (pI->_itype == ItemType::Staff && (!gbIsHellfire || IsValidSpell(pI->_iSpell)))
		rv = true;
	if (pI->IDidx >= IDI_FIRSTQUEST && pI->IDidx <= IDI_LASTQUEST)
		rv = false;
	if (pI->IDidx == IDI_LAZSTAFF)
		rv = false;
	return rv;
}

void StartWitchSell()
{
	isTextFullSize = true;
	bool sellok = false;
	currentItemIndex = 0;

	for (auto &item : playerItem) {
		item.clear();
	}

	const Player &myPlayer = *MyPlayer;

	for (int i = 0; i < myPlayer._pNumInv; i++) {
		if (currentItemIndex >= 48)
			break;
		if (WitchSellOk(i)) {
			sellok = true;
			playerItem[currentItemIndex] = myPlayer.InvList[i];

			if (playerItem[currentItemIndex]._iMagical != ITEM_QUALITY_NORMAL && playerItem[currentItemIndex]._iIdentified)
				playerItem[currentItemIndex]._ivalue = playerItem[currentItemIndex]._iIvalue;

			playerItem[currentItemIndex]._ivalue = std::max(playerItem[currentItemIndex]._ivalue / 4, 1);
			playerItem[currentItemIndex]._iIvalue = playerItem[currentItemIndex]._ivalue;
			playerItemIndexes[currentItemIndex] = i;
			currentItemIndex++;
		}
	}

	for (int i = 0; i < MaxBeltItems; i++) {
		if (currentItemIndex >= 48)
			break;
		if (!myPlayer.SpdList[i].isEmpty() && WitchSellOk(-(i + 1))) {
			sellok = true;
			playerItem[currentItemIndex] = myPlayer.SpdList[i];

			if (playerItem[currentItemIndex]._iMagical != ITEM_QUALITY_NORMAL && playerItem[currentItemIndex]._iIdentified)
				playerItem[currentItemIndex]._ivalue = playerItem[currentItemIndex]._iIvalue;

			playerItem[currentItemIndex]._ivalue = std::max(playerItem[currentItemIndex]._ivalue / 4, 1);
			playerItem[currentItemIndex]._iIvalue = playerItem[currentItemIndex]._ivalue;
			playerItemIndexes[currentItemIndex] = -(i + 1);
			currentItemIndex++;
		}
	}

	if (!sellok) {
		hasScrollbar = false;

		renderGold = true;
		AddSText(20, 1, _("You have nothing I want."), UiFlags::ColorWhitegold, false);

		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	hasScrollbar = true;
	scrollPos = 0;
	numTextLines = myPlayer._pNumInv;

	renderGold = true;
	AddSText(20, 1, _("Which item is for sale?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollSmithSell(scrollPos);
	AddItemListBackButton();
}

bool WitchRechargeOk(int i)
{
	const auto &item = MyPlayer->InvList[i];

	if (item._itype == ItemType::Staff && item._iCharges != item._iMaxCharges) {
		return true;
	}

	if ((item._iMiscId == IMISC_UNIQUE || item._iMiscId == IMISC_STAFF) && item._iCharges < item._iMaxCharges) {
		return true;
	}

	return false;
}

void AddStoreHoldRecharge(Item itm, int8_t i)
{
	playerItem[currentItemIndex] = itm;
	playerItem[currentItemIndex]._ivalue += GetSpellData(itm._iSpell).staffCost();
	playerItem[currentItemIndex]._ivalue = playerItem[currentItemIndex]._ivalue * (playerItem[currentItemIndex]._iMaxCharges - playerItem[currentItemIndex]._iCharges) / (playerItem[currentItemIndex]._iMaxCharges * 2);
	playerItem[currentItemIndex]._iIvalue = playerItem[currentItemIndex]._ivalue;
	playerItemIndexes[currentItemIndex] = i;
	currentItemIndex++;
}

void StartWitchRecharge()
{
	isTextFullSize = true;
	bool rechargeok = false;
	currentItemIndex = 0;

	for (auto &item : playerItem) {
		item.clear();
	}

	const Player &myPlayer = *MyPlayer;
	const auto &leftHand = myPlayer.InvBody[INVLOC_HAND_LEFT];

	if ((leftHand._itype == ItemType::Staff || leftHand._iMiscId == IMISC_UNIQUE) && leftHand._iCharges != leftHand._iMaxCharges) {
		rechargeok = true;
		AddStoreHoldRecharge(leftHand, -1);
	}

	for (int i = 0; i < myPlayer._pNumInv; i++) {
		if (currentItemIndex >= 48)
			break;
		if (WitchRechargeOk(i)) {
			rechargeok = true;
			AddStoreHoldRecharge(myPlayer.InvList[i], i);
		}
	}

	if (!rechargeok) {
		hasScrollbar = false;

		renderGold = true;
		AddSText(20, 1, _("You have nothing to recharge."), UiFlags::ColorWhitegold, false);
		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	hasScrollbar = true;
	scrollPos = 0;
	numTextLines = myPlayer._pNumInv;

	renderGold = true;
	AddSText(20, 1, _("Recharge which item?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollSmithSell(scrollPos);
	AddItemListBackButton();
}

void StoreNoMoney()
{
	StartStore(oldActiveStore);
	hasScrollbar = false;
	isTextFullSize = true;
	renderGold = true;
	ClearSText(5, 23);
	AddSText(0, 14, _("You do not have enough gold"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
}

void StoreNoRoom()
{
	StartStore(oldActiveStore);
	hasScrollbar = false;
	ClearSText(5, 23);
	AddSText(0, 14, _("You do not have enough room in inventory"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
}

void StoreConfirm(Item &item)
{
	StartStore(oldActiveStore);
	hasScrollbar = false;
	ClearSText(5, 23);

	UiFlags itemColor = item.getTextColorWithStatCheck();
	AddSText(20, 8, item.getName(), itemColor, false);
	AddSTextVal(8, item._iIvalue);
	PrintStoreItem(item, 9, itemColor);

	std::string_view prompt;

	switch (oldActiveStore) {
	case TalkID::BoyBuy:
		prompt = _("Do we have a deal?");
		break;
	case TalkID::StorytellerIdentify:
		prompt = _("Are you sure you want to identify this item?");
		break;
	case TalkID::HealerBuy:
	case TalkID::SmithPremiumBuy:
	case TalkID::WitchBuy:
	case TalkID::SmithBuy:
		prompt = _("Are you sure you want to buy this item?");
		break;
	case TalkID::WitchRecharge:
		prompt = _("Are you sure you want to recharge this item?");
		break;
	case TalkID::SmithSell:
	case TalkID::WitchSell:
		prompt = _("Are you sure you want to sell this item?");
		break;
	case TalkID::SmithRepair:
		prompt = _("Are you sure you want to repair this item?");
		break;
	default:
		app_fatal(StrCat("Unknown store dialog ", static_cast<int>(oldActiveStore)));
	}
	AddSText(0, 15, prompt, UiFlags::ColorWhite | UiFlags::AlignCenter, false);
	AddSText(0, 18, _("Yes"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 20, _("No"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
}

void StartBoy()
{
	isTextFullSize = false;
	hasScrollbar = false;
	AddSText(0, 2, _("Wirt the Peg-legged boy"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSLine(5);
	if (!boyItem.isEmpty()) {
		AddSText(0, 8, _("Talk to Wirt"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
		AddSText(0, 12, _("I have something for sale,"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
		AddSText(0, 14, _("but it will cost 50 gold"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
		AddSText(0, 16, _("just to take a look. "), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
		AddSText(0, 18, _("What have you got?"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
		AddSText(0, 20, _("Say goodbye"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	} else {
		AddSText(0, 12, _("Talk to Wirt"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
		AddSText(0, 18, _("Say goodbye"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	}
}

void SStartBoyBuy()
{
	isTextFullSize = true;
	hasScrollbar = false;

	renderGold = true;
	AddSText(20, 1, _("I have this item for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);

	boyItem._iStatFlag = MyPlayer->CanUseItem(boyItem);
	UiFlags itemColor = boyItem.getTextColorWithStatCheck();
	AddSText(20, 10, boyItem.getName(), itemColor, true, boyItem._iCurs, true);
	if (gbIsHellfire)
		AddSTextVal(10, boyItem._iIvalue - (boyItem._iIvalue / 4));
	else
		AddSTextVal(10, boyItem._iIvalue + (boyItem._iIvalue / 2));
	PrintStoreItem(boyItem, 11, itemColor, true);

	{
		// Add a Leave button. Unlike the other item list back buttons,
		// this one has different text and different layout in LargerSmallFont locales.
		const int line = BackButtonLine();
		AddSLine(line - 1);
		AddSText(0, line, _("Leave"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
		textLine[line]._syoff = 6;
	}
}

void HealPlayer()
{
	Player &myPlayer = *MyPlayer;

	if (myPlayer._pHitPoints != myPlayer._pMaxHP) {
		PlaySFX(SfxID::CastHealing);
	}
	myPlayer._pHitPoints = myPlayer._pMaxHP;
	myPlayer._pHPBase = myPlayer._pMaxHPBase;
	RedrawComponent(PanelDrawComponent::Health);
}

void StartHealer()
{
	HealPlayer();
	isTextFullSize = false;
	hasScrollbar = false;
	AddSText(0, 1, _("Welcome to the"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 3, _("Healer's home"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Pepin"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Buy items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Leave Healer's home"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	currentItemIndex = 20;
}

void ScrollHealerBuy(int idx)
{
	ScrollVendorStore(healerItem, static_cast<int>(std::size(healerItem)), idx);
}

void StartHealerBuy()
{
	isTextFullSize = true;
	hasScrollbar = true;
	scrollPos = 0;

	renderGold = true;
	AddSText(20, 1, _("I have these items for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);

	ScrollHealerBuy(scrollPos);
	AddItemListBackButton();

	currentItemIndex = 0;
	for (Item &item : healerItem) {
		if (item.isEmpty())
			continue;

		item._iStatFlag = MyPlayer->CanUseItem(item);
		currentItemIndex++;
	}

	numTextLines = std::max(currentItemIndex - 4, 0);
}

void StartStoryteller()
{
	isTextFullSize = false;
	hasScrollbar = false;
	AddSText(0, 2, _("The Town Elder"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Cain"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Identify an item"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Say goodbye"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
}

bool IdItemOk(Item *i)
{
	if (i->isEmpty()) {
		return false;
	}
	if (i->_iMagical == ITEM_QUALITY_NORMAL) {
		return false;
	}
	return !i->_iIdentified;
}

void AddStoreHoldId(Item itm, int8_t i)
{
	playerItem[currentItemIndex] = itm;
	playerItem[currentItemIndex]._ivalue = 100;
	playerItem[currentItemIndex]._iIvalue = 100;
	playerItemIndexes[currentItemIndex] = i;
	currentItemIndex++;
}

void StartStorytellerIdentify()
{
	bool idok = false;
	isTextFullSize = true;
	currentItemIndex = 0;

	for (auto &item : playerItem) {
		item.clear();
	}

	Player &myPlayer = *MyPlayer;

	auto &helmet = myPlayer.InvBody[INVLOC_HEAD];
	if (IdItemOk(&helmet)) {
		idok = true;
		AddStoreHoldId(helmet, -1);
	}

	auto &armor = myPlayer.InvBody[INVLOC_CHEST];
	if (IdItemOk(&armor)) {
		idok = true;
		AddStoreHoldId(armor, -2);
	}

	auto &leftHand = myPlayer.InvBody[INVLOC_HAND_LEFT];
	if (IdItemOk(&leftHand)) {
		idok = true;
		AddStoreHoldId(leftHand, -3);
	}

	auto &rightHand = myPlayer.InvBody[INVLOC_HAND_RIGHT];
	if (IdItemOk(&rightHand)) {
		idok = true;
		AddStoreHoldId(rightHand, -4);
	}

	auto &leftRing = myPlayer.InvBody[INVLOC_RING_LEFT];
	if (IdItemOk(&leftRing)) {
		idok = true;
		AddStoreHoldId(leftRing, -5);
	}

	auto &rightRing = myPlayer.InvBody[INVLOC_RING_RIGHT];
	if (IdItemOk(&rightRing)) {
		idok = true;
		AddStoreHoldId(rightRing, -6);
	}

	auto &amulet = myPlayer.InvBody[INVLOC_AMULET];
	if (IdItemOk(&amulet)) {
		idok = true;
		AddStoreHoldId(amulet, -7);
	}

	for (int i = 0; i < myPlayer._pNumInv; i++) {
		if (currentItemIndex >= 48)
			break;
		auto &item = myPlayer.InvList[i];
		if (IdItemOk(&item)) {
			idok = true;
			AddStoreHoldId(item, i);
		}
	}

	if (!idok) {
		hasScrollbar = false;

		renderGold = true;
		AddSText(20, 1, _("You have nothing to identify."), UiFlags::ColorWhitegold, false);
		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	hasScrollbar = true;
	scrollPos = 0;
	numTextLines = myPlayer._pNumInv;

	renderGold = true;
	AddSText(20, 1, _("Identify which item?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);

	ScrollSmithSell(scrollPos);
	AddItemListBackButton();
}

void StartStorytellerIdentifyShow(Item &item)
{
	StartStore(oldActiveStore);
	hasScrollbar = false;
	ClearSText(5, 23);

	UiFlags itemColor = item.getTextColorWithStatCheck();

	AddSText(0, 7, _("This item is:"), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
	AddSText(20, 11, item.getName(), itemColor, false);
	PrintStoreItem(item, 12, itemColor);
	AddSText(0, 18, _("Done"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
}

void StartTalk()
{
	int la;

	isTextFullSize = false;
	hasScrollbar = false;
	AddSText(0, 2, fmt::format(fmt::runtime(_("Talk to {:s}")), _(TownerNames[townerId])), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSLine(5);
	if (gbIsSpawn) {
		AddSText(0, 10, fmt::format(fmt::runtime(_("Talking to {:s}")), _(TownerNames[townerId])), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
		AddSText(0, 12, _("is not available"), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
		AddSText(0, 14, _("in the shareware"), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
		AddSText(0, 16, _("version"), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
		AddOptionsBackButton();
		return;
	}

	int sn = 0;
	for (auto &quest : Quests) {
		if (quest._qactive == QUEST_ACTIVE && QuestDialogTable[townerId][quest._qidx] != TEXT_NONE && quest._qlog)
			sn++;
	}

	if (sn > 6) {
		sn = 14 - (sn / 2);
		la = 1;
	} else {
		sn = 15 - sn;
		la = 2;
	}

	int sn2 = sn - 2;

	for (auto &quest : Quests) {
		if (quest._qactive == QUEST_ACTIVE && QuestDialogTable[townerId][quest._qidx] != TEXT_NONE && quest._qlog) {
			AddSText(0, sn, _(QuestsData[quest._qidx]._qlstr), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
			sn += la;
		}
	}
	AddSText(0, sn2, _("Gossip"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddOptionsBackButton();
}

void StartTavern()
{
	isTextFullSize = false;
	hasScrollbar = false;
	AddSText(0, 1, _("Welcome to the"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 3, _("Rising Sun"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Ogden"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Leave the tavern"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	currentItemIndex = 20;
}

void StartBarmaid()
{
	isTextFullSize = false;
	hasScrollbar = false;
	AddSText(0, 2, _("Gillian"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Gillian"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Access Storage"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Say goodbye"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	currentItemIndex = 20;
}

void StartDrunk()
{
	isTextFullSize = false;
	hasScrollbar = false;
	AddSText(0, 2, _("Farnham the Drunk"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Farnham"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Say Goodbye"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	currentItemIndex = 20;
}

void SmithEnter()
{
	switch (currentTextLine) {
	case 10:
		townerId = TOWN_SMITH;
		oldTextLine = 10;
		oldActiveStore = TalkID::Smith;
		StartStore(TalkID::Gossip);
		break;
	case 12:
		StartStore(TalkID::SmithBuy);
		break;
	case 14:
		StartStore(TalkID::SmithPremiumBuy);
		break;
	case 16:
		StartStore(TalkID::SmithSell);
		break;
	case 18:
		StartStore(TalkID::SmithRepair);
		break;
	case 20:
		activeStore = TalkID::None;
		break;
	}
}

/**
 * @brief Purchases an item from the smith.
 */
void SmithBuyItem(Item &item)
{
	TakePlrsMoney(item._iIvalue);
	if (item._iMagical == ITEM_QUALITY_NORMAL)
		item._iIdentified = false;
	StoreAutoPlace(item, true);
	int idx = oldScrollPos + ((oldTextLine - previousScrollPos) / 4);
	if (idx == SMITH_ITEMS - 1) {
		smithItem[SMITH_ITEMS - 1].clear();
	} else {
		for (; !smithItem[idx + 1].isEmpty(); idx++) {
			smithItem[idx] = std::move(smithItem[idx + 1]);
		}
		smithItem[idx].clear();
	}
	CalcPlrInv(*MyPlayer, true);
}

void SmithBuyEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(TalkID::Smith);
		currentTextLine = 12;
		return;
	}

	oldTextLine = currentTextLine;
	oldScrollPos = scrollPos;
	oldActiveStore = TalkID::SmithBuy;

	int idx = scrollPos + ((currentTextLine - previousScrollPos) / 4);
	if (!PlayerCanAfford(smithItem[idx]._iIvalue)) {
		StartStore(TalkID::NoMoney);
		return;
	}

	if (!StoreAutoPlace(smithItem[idx], false)) {
		StartStore(TalkID::NoRoom);
		return;
	}

	tempItem = smithItem[idx];
	StartStore(TalkID::Confirm);
}

/**
 * @brief Purchases a premium item from the smith.
 */
void SmithBuyPItem(Item &item)
{
	TakePlrsMoney(item._iIvalue);
	if (item._iMagical == ITEM_QUALITY_NORMAL)
		item._iIdentified = false;
	StoreAutoPlace(item, true);

	int idx = oldScrollPos + ((oldTextLine - previousScrollPos) / 4);
	int xx = 0;
	for (int i = 0; idx >= 0; i++) {
		if (!premiumItem[i].isEmpty()) {
			idx--;
			xx = i;
		}
	}

	premiumItem[xx].clear();
	numPremiumItems--;
	SpawnPremium(*MyPlayer);
}

void SmithPremiumBuyEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(TalkID::Smith);
		currentTextLine = 14;
		return;
	}

	oldActiveStore = TalkID::SmithPremiumBuy;
	oldTextLine = currentTextLine;
	oldScrollPos = scrollPos;

	int xx = scrollPos + ((currentTextLine - previousScrollPos) / 4);
	int idx = 0;
	for (int i = 0; xx >= 0; i++) {
		if (!premiumItem[i].isEmpty()) {
			xx--;
			idx = i;
		}
	}

	if (!PlayerCanAfford(premiumItem[idx]._iIvalue)) {
		StartStore(TalkID::NoMoney);
		return;
	}

	if (!StoreAutoPlace(premiumItem[idx], false)) {
		StartStore(TalkID::NoRoom);
		return;
	}

	tempItem = premiumItem[idx];
	StartStore(TalkID::Confirm);
}

bool StoreGoldFit(Item &item)
{
	int cost = item._iIvalue;

	Size itemSize = GetInventorySize(item);
	int itemRoomForGold = itemSize.width * itemSize.height * MaxGold;

	if (cost <= itemRoomForGold) {
		return true;
	}

	return cost <= itemRoomForGold + RoomForGold();
}

/**
 * @brief Sells an item from the player's inventory or belt.
 */
void StoreSellItem()
{
	Player &myPlayer = *MyPlayer;

	int idx = oldScrollPos + ((oldTextLine - previousScrollPos) / 4);
	if (playerItemIndexes[idx] >= 0)
		myPlayer.RemoveInvItem(playerItemIndexes[idx]);
	else
		myPlayer.RemoveSpdBarItem(-(playerItemIndexes[idx] + 1));

	int cost = playerItem[idx]._iIvalue;
	currentItemIndex--;
	if (idx != currentItemIndex) {
		while (idx < currentItemIndex) {
			playerItem[idx] = playerItem[idx + 1];
			playerItemIndexes[idx] = playerItemIndexes[idx + 1];
			idx++;
		}
	}

	AddGoldToInventory(myPlayer, cost);

	myPlayer._pGold += cost;
}

void SmithSellEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(TalkID::Smith);
		currentTextLine = 16;
		return;
	}

	oldTextLine = currentTextLine;
	oldActiveStore = TalkID::SmithSell;
	oldScrollPos = scrollPos;

	int idx = scrollPos + ((currentTextLine - previousScrollPos) / 4);

	if (!StoreGoldFit(playerItem[idx])) {
		StartStore(TalkID::NoRoom);
		return;
	}

	tempItem = playerItem[idx];
	StartStore(TalkID::Confirm);
}

/**
 * @brief Repairs an item in the player's inventory or body in the smith.
 */
void SmithRepairItem(int price)
{
	int idx = oldScrollPos + ((oldTextLine - previousScrollPos) / 4);
	playerItem[idx]._iDurability = playerItem[idx]._iMaxDur;

	int8_t i = playerItemIndexes[idx];

	Player &myPlayer = *MyPlayer;

	if (i < 0) {
		if (i == -1)
			myPlayer.InvBody[INVLOC_HEAD]._iDurability = myPlayer.InvBody[INVLOC_HEAD]._iMaxDur;
		if (i == -2)
			myPlayer.InvBody[INVLOC_CHEST]._iDurability = myPlayer.InvBody[INVLOC_CHEST]._iMaxDur;
		if (i == -3)
			myPlayer.InvBody[INVLOC_HAND_LEFT]._iDurability = myPlayer.InvBody[INVLOC_HAND_LEFT]._iMaxDur;
		if (i == -4)
			myPlayer.InvBody[INVLOC_HAND_RIGHT]._iDurability = myPlayer.InvBody[INVLOC_HAND_RIGHT]._iMaxDur;
		TakePlrsMoney(price);
		return;
	}

	myPlayer.InvList[i]._iDurability = myPlayer.InvList[i]._iMaxDur;
	TakePlrsMoney(price);
}

void SmithRepairEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(TalkID::Smith);
		currentTextLine = 18;
		return;
	}

	oldActiveStore = TalkID::SmithRepair;
	oldTextLine = currentTextLine;
	oldScrollPos = scrollPos;

	int idx = scrollPos + ((currentTextLine - previousScrollPos) / 4);

	if (!PlayerCanAfford(playerItem[idx]._iIvalue)) {
		StartStore(TalkID::NoMoney);
		return;
	}

	tempItem = playerItem[idx];
	StartStore(TalkID::Confirm);
}

void WitchEnter()
{
	switch (currentTextLine) {
	case 12:
		oldTextLine = 12;
		townerId = TOWN_WITCH;
		oldActiveStore = TalkID::Witch;
		StartStore(TalkID::Gossip);
		break;
	case 14:
		StartStore(TalkID::WitchBuy);
		break;
	case 16:
		StartStore(TalkID::WitchSell);
		break;
	case 18:
		StartStore(TalkID::WitchRecharge);
		break;
	case 20:
		activeStore = TalkID::None;
		break;
	}
}

/**
 * @brief Purchases an item from the witch.
 */
void WitchBuyItem(Item &item)
{
	int idx = oldScrollPos + ((oldTextLine - previousScrollPos) / 4);

	if (idx < 3)
		item._iSeed = AdvanceRndSeed();

	TakePlrsMoney(item._iIvalue);
	StoreAutoPlace(item, true);

	if (idx >= 3) {
		if (idx == WITCH_ITEMS - 1) {
			witchItem[WITCH_ITEMS - 1].clear();
		} else {
			for (; !witchItem[idx + 1].isEmpty(); idx++) {
				witchItem[idx] = std::move(witchItem[idx + 1]);
			}
			witchItem[idx].clear();
		}
	}

	CalcPlrInv(*MyPlayer, true);
}

void WitchBuyEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(TalkID::Witch);
		currentTextLine = 14;
		return;
	}

	oldTextLine = currentTextLine;
	oldScrollPos = scrollPos;
	oldActiveStore = TalkID::WitchBuy;

	int idx = scrollPos + ((currentTextLine - previousScrollPos) / 4);

	if (!PlayerCanAfford(witchItem[idx]._iIvalue)) {
		StartStore(TalkID::NoMoney);
		return;
	}

	if (!StoreAutoPlace(witchItem[idx], false)) {
		StartStore(TalkID::NoRoom);
		return;
	}

	tempItem = witchItem[idx];
	StartStore(TalkID::Confirm);
}

void WitchSellEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(TalkID::Witch);
		currentTextLine = 16;
		return;
	}

	oldTextLine = currentTextLine;
	oldActiveStore = TalkID::WitchSell;
	oldScrollPos = scrollPos;

	int idx = scrollPos + ((currentTextLine - previousScrollPos) / 4);

	if (!StoreGoldFit(playerItem[idx])) {
		StartStore(TalkID::NoRoom);
		return;
	}

	tempItem = playerItem[idx];
	StartStore(TalkID::Confirm);
}

/**
 * @brief Recharges an item in the player's inventory or body in the witch.
 */
void WitchRechargeItem(int price)
{
	int idx = oldScrollPos + ((oldTextLine - previousScrollPos) / 4);
	playerItem[idx]._iCharges = playerItem[idx]._iMaxCharges;

	Player &myPlayer = *MyPlayer;

	int8_t i = playerItemIndexes[idx];
	if (i < 0) {
		myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges = myPlayer.InvBody[INVLOC_HAND_LEFT]._iMaxCharges;
		NetSendCmdChItem(true, INVLOC_HAND_LEFT);
	} else {
		myPlayer.InvList[i]._iCharges = myPlayer.InvList[i]._iMaxCharges;
		NetSyncInvItem(myPlayer, i);
	}

	TakePlrsMoney(price);
	CalcPlrInv(myPlayer, true);
}

void WitchRechargeEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(TalkID::Witch);
		currentTextLine = 18;
		return;
	}

	oldActiveStore = TalkID::WitchRecharge;
	oldTextLine = currentTextLine;
	oldScrollPos = scrollPos;

	int idx = scrollPos + ((currentTextLine - previousScrollPos) / 4);

	if (!PlayerCanAfford(playerItem[idx]._iIvalue)) {
		StartStore(TalkID::NoMoney);
		return;
	}

	tempItem = playerItem[idx];
	StartStore(TalkID::Confirm);
}

void BoyEnter()
{
	if (!boyItem.isEmpty() && currentTextLine == 18) {
		if (!PlayerCanAfford(50)) {
			oldActiveStore = TalkID::Boy;
			oldTextLine = 18;
			oldScrollPos = scrollPos;
			StartStore(TalkID::NoMoney);
		} else {
			TakePlrsMoney(50);
			StartStore(TalkID::BoyBuy);
		}
		return;
	}

	if ((currentTextLine != 8 && !boyItem.isEmpty()) || (currentTextLine != 12 && boyItem.isEmpty())) {
		activeStore = TalkID::None;
		return;
	}

	townerId = TOWN_PEGBOY;
	oldActiveStore = TalkID::Boy;
	oldTextLine = currentTextLine;
	StartStore(TalkID::Gossip);
}

void BoyBuyItem(Item &item)
{
	TakePlrsMoney(item._iIvalue);
	StoreAutoPlace(item, true);
	boyItem.clear();
	oldActiveStore = TalkID::Boy;
	CalcPlrInv(*MyPlayer, true);
	oldTextLine = 12;
}

/**
 * @brief Purchases an item from the healer.
 */
void HealerBuyItem(Item &item)
{
	int idx = oldScrollPos + ((oldTextLine - previousScrollPos) / 4);
	if (!gbIsMultiplayer) {
		if (idx < 2)
			item._iSeed = AdvanceRndSeed();
	} else {
		if (idx < 3)
			item._iSeed = AdvanceRndSeed();
	}

	TakePlrsMoney(item._iIvalue);
	if (item._iMagical == ITEM_QUALITY_NORMAL)
		item._iIdentified = false;
	StoreAutoPlace(item, true);

	if (!gbIsMultiplayer) {
		if (idx < 2)
			return;
	} else {
		if (idx < 3)
			return;
	}
	idx = oldScrollPos + ((oldTextLine - previousScrollPos) / 4);
	if (idx == 19) {
		healerItem[19].clear();
	} else {
		for (; !healerItem[idx + 1].isEmpty(); idx++) {
			healerItem[idx] = std::move(healerItem[idx + 1]);
		}
		healerItem[idx].clear();
	}
	CalcPlrInv(*MyPlayer, true);
}

void BoyBuyEnter()
{
	if (currentTextLine != 10) {
		activeStore = TalkID::None;
		return;
	}

	oldActiveStore = TalkID::BoyBuy;
	oldScrollPos = scrollPos;
	oldTextLine = 10;
	int price = boyItem._iIvalue;
	if (gbIsHellfire)
		price -= boyItem._iIvalue / 4;
	else
		price += boyItem._iIvalue / 2;

	if (!PlayerCanAfford(price)) {
		StartStore(TalkID::NoMoney);
		return;
	}

	if (!StoreAutoPlace(boyItem, false)) {
		StartStore(TalkID::NoRoom);
		return;
	}

	tempItem = boyItem;
	tempItem._iIvalue = price;
	StartStore(TalkID::Confirm);
}

void StorytellerIdentifyItem(Item &item)
{
	Player &myPlayer = *MyPlayer;

	int8_t idx = playerItemIndexes[((oldTextLine - previousScrollPos) / 4) + oldScrollPos];
	if (idx < 0) {
		if (idx == -1)
			myPlayer.InvBody[INVLOC_HEAD]._iIdentified = true;
		if (idx == -2)
			myPlayer.InvBody[INVLOC_CHEST]._iIdentified = true;
		if (idx == -3)
			myPlayer.InvBody[INVLOC_HAND_LEFT]._iIdentified = true;
		if (idx == -4)
			myPlayer.InvBody[INVLOC_HAND_RIGHT]._iIdentified = true;
		if (idx == -5)
			myPlayer.InvBody[INVLOC_RING_LEFT]._iIdentified = true;
		if (idx == -6)
			myPlayer.InvBody[INVLOC_RING_RIGHT]._iIdentified = true;
		if (idx == -7)
			myPlayer.InvBody[INVLOC_AMULET]._iIdentified = true;
	} else {
		myPlayer.InvList[idx]._iIdentified = true;
	}
	item._iIdentified = true;
	TakePlrsMoney(item._iIvalue);
	CalcPlrInv(myPlayer, true);
}

void ConfirmEnter(Item &item)
{
	if (currentTextLine == 18) {
		switch (oldActiveStore) {
		case TalkID::SmithBuy:
			SmithBuyItem(item);
			break;
		case TalkID::SmithSell:
		case TalkID::WitchSell:
			StoreSellItem();
			break;
		case TalkID::SmithRepair:
			SmithRepairItem(item._iIvalue);
			break;
		case TalkID::WitchBuy:
			WitchBuyItem(item);
			break;
		case TalkID::WitchRecharge:
			WitchRechargeItem(item._iIvalue);
			break;
		case TalkID::BoyBuy:
			BoyBuyItem(item);
			break;
		case TalkID::HealerBuy:
			HealerBuyItem(item);
			break;
		case TalkID::StorytellerIdentify:
			StorytellerIdentifyItem(item);
			StartStore(TalkID::StorytellerIdentifyShow);
			return;
		case TalkID::SmithPremiumBuy:
			SmithBuyPItem(item);
			break;
		default:
			break;
		}
	}

	StartStore(oldActiveStore);

	if (currentTextLine == BackButtonLine())
		return;

	currentTextLine = oldTextLine;
	scrollPos = std::min(oldScrollPos, numTextLines);

	while (currentTextLine != -1 && !textLine[currentTextLine].isSelectable()) {
		currentTextLine--;
	}
}

void HealerEnter()
{
	switch (currentTextLine) {
	case 12:
		oldTextLine = 12;
		townerId = TOWN_HEALER;
		oldActiveStore = TalkID::Healer;
		StartStore(TalkID::Gossip);
		break;
	case 14:
		StartStore(TalkID::HealerBuy);
		break;
	case 18:
		activeStore = TalkID::None;
		break;
	}
}

void HealerBuyEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(TalkID::Healer);
		currentTextLine = 14;
		return;
	}

	oldTextLine = currentTextLine;
	oldScrollPos = scrollPos;
	oldActiveStore = TalkID::HealerBuy;

	int idx = scrollPos + ((currentTextLine - previousScrollPos) / 4);

	if (!PlayerCanAfford(healerItem[idx]._iIvalue)) {
		StartStore(TalkID::NoMoney);
		return;
	}

	if (!StoreAutoPlace(healerItem[idx], false)) {
		StartStore(TalkID::NoRoom);
		return;
	}

	tempItem = healerItem[idx];
	StartStore(TalkID::Confirm);
}

void StorytellerEnter()
{
	switch (currentTextLine) {
	case 12:
		oldTextLine = 12;
		townerId = TOWN_STORY;
		oldActiveStore = TalkID::Storyteller;
		StartStore(TalkID::Gossip);
		break;
	case 14:
		StartStore(TalkID::StorytellerIdentify);
		break;
	case 18:
		activeStore = TalkID::None;
		break;
	}
}

void StorytellerIdentifyEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(TalkID::Storyteller);
		currentTextLine = 14;
		return;
	}

	oldActiveStore = TalkID::StorytellerIdentify;
	oldTextLine = currentTextLine;
	oldScrollPos = scrollPos;

	int idx = scrollPos + ((currentTextLine - previousScrollPos) / 4);

	if (!PlayerCanAfford(playerItem[idx]._iIvalue)) {
		StartStore(TalkID::NoMoney);
		return;
	}

	tempItem = playerItem[idx];
	StartStore(TalkID::Confirm);
}

void TalkEnter()
{
	if (currentTextLine == BackButtonLine()) {
		StartStore(oldActiveStore);
		currentTextLine = oldTextLine;
		return;
	}

	int sn = 0;
	for (auto &quest : Quests) {
		if (quest._qactive == QUEST_ACTIVE && QuestDialogTable[townerId][quest._qidx] != TEXT_NONE && quest._qlog)
			sn++;
	}
	int la = 2;
	if (sn > 6) {
		sn = 14 - (sn / 2);
		la = 1;
	} else {
		sn = 15 - sn;
	}

	if (currentTextLine == sn - 2) {
		Towner *target = GetTowner(townerId);
		assert(target != nullptr);
		InitQTextMsg(target->gossip);
		return;
	}

	for (auto &quest : Quests) {
		if (quest._qactive == QUEST_ACTIVE && QuestDialogTable[townerId][quest._qidx] != TEXT_NONE && quest._qlog) {
			if (sn == currentTextLine) {
				InitQTextMsg(QuestDialogTable[townerId][quest._qidx]);
			}
			sn += la;
		}
	}
}

void TavernEnter()
{
	switch (currentTextLine) {
	case 12:
		oldTextLine = 12;
		townerId = TOWN_TAVERN;
		oldActiveStore = TalkID::Tavern;
		StartStore(TalkID::Gossip);
		break;
	case 18:
		activeStore = TalkID::None;
		break;
	}
}

void BarmaidEnter()
{
	switch (currentTextLine) {
	case 12:
		oldTextLine = 12;
		townerId = TOWN_BMAID;
		oldActiveStore = TalkID::Barmaid;
		StartStore(TalkID::Gossip);
		break;
	case 14:
		activeStore = TalkID::None;
		IsStashOpen = true;
		Stash.RefreshItemStatFlags();
		invflag = true;
		if (ControlMode != ControlTypes::KeyboardAndMouse) {
			if (pcurs == CURSOR_DISARM)
				NewCursor(CURSOR_HAND);
			FocusOnInventory();
		}
		break;
	case 18:
		activeStore = TalkID::None;
		break;
	}
}

void DrunkEnter()
{
	switch (currentTextLine) {
	case 12:
		oldTextLine = 12;
		townerId = TOWN_DRUNK;
		oldActiveStore = TalkID::Drunk;
		StartStore(TalkID::Gossip);
		break;
	case 18:
		activeStore = TalkID::None;
		break;
	}
}

int TakeGold(Player &player, int cost, bool skipMaxPiles)
{
	for (int i = 0; i < player._pNumInv; i++) {
		auto &item = player.InvList[i];
		if (item._itype != ItemType::Gold || (skipMaxPiles && item._ivalue == MaxGold))
			continue;

		if (cost < item._ivalue) {
			item._ivalue -= cost;
			SetPlrHandGoldCurs(player.InvList[i]);
			return 0;
		}

		cost -= item._ivalue;
		player.RemoveInvItem(i);
		i = -1;
	}

	return cost;
}

void DrawSelector(const Surface &out, const Rectangle &rect, std::string_view text, UiFlags flags)
{
	int lineWidth = GetLineWidth(text);

	int x1 = rect.position.x - 20;
	if (HasAnyOf(flags, UiFlags::AlignCenter))
		x1 += (rect.size.width - lineWidth) / 2;

	ClxDraw(out, { x1, rect.position.y + 13 }, (*pSPentSpn2Cels)[PentSpn2Spin()]);

	int x2 = rect.position.x + rect.size.width + 5;
	if (HasAnyOf(flags, UiFlags::AlignCenter))
		x2 = rect.position.x + (rect.size.width - lineWidth) / 2 + lineWidth + 5;

	ClxDraw(out, { x2, rect.position.y + 13 }, (*pSPentSpn2Cels)[PentSpn2Spin()]);
}

} // namespace

void AddStoreHoldRepair(Item *itm, int8_t i)
{
	Item *item;
	int v;

	item = &playerItem[currentItemIndex];
	playerItem[currentItemIndex] = *itm;

	int due = item->_iMaxDur - item->_iDurability;
	if (item->_iMagical != ITEM_QUALITY_NORMAL && item->_iIdentified) {
		v = 30 * item->_iIvalue * due / (item->_iMaxDur * 100 * 2);
		if (v == 0)
			return;
	} else {
		v = item->_ivalue * due / (item->_iMaxDur * 2);
		v = std::max(v, 1);
	}
	item->_iIvalue = v;
	item->_ivalue = v;
	playerItemIndexes[currentItemIndex] = i;
	currentItemIndex++;
}

void InitStores()
{
	ClearSText(0, STORE_LINES);
	activeStore = TalkID::None;
	isTextFullSize = false;
	hasScrollbar = false;
	numPremiumItems = 0;
	premiumItemLevel = 1;

	for (auto &premiumitem : premiumItem)
		premiumitem.clear();

	boyItem.clear();
	boyItemLevel = 0;
}

void SetupTownStores()
{
	Player &myPlayer = *MyPlayer;

	int l = myPlayer.getCharacterLevel() / 2;
	if (!gbIsMultiplayer) {
		l = 0;
		for (int i = 0; i < NUMLEVELS; i++) {
			if (myPlayer._pLvlVisited[i])
				l = i;
		}
	}

	l = std::clamp(l + 2, 6, 16);
	SpawnSmith(l);
	SpawnWitch(l);
	SpawnHealer(l);
	SpawnBoy(myPlayer.getCharacterLevel());
	SpawnPremium(myPlayer);
}

void FreeStoreMem()
{
	if (*sgOptions.Gameplay.showItemGraphicsInStores) {
		FreeHalfSizeItemSprites();
	}
	activeStore = TalkID::None;
	for (STextStruct &entry : textLine) {
		entry.text.clear();
		entry.text.shrink_to_fit();
	}
}

void PrintSString(const Surface &out, int margin, int line, std::string_view text, UiFlags flags, int price, int cursId, bool cursIndent)
{
	const Point uiPosition = GetUIRectangle().position;
	int sx = uiPosition.x + 32 + margin;
	if (!isTextFullSize) {
		sx += 320;
	}

	const int sy = uiPosition.y + PaddingTop + textLine[line].y + textLine[line]._syoff;

	int width = isTextFullSize ? 575 : 255;
	if (hasScrollbar && line >= 4 && line <= 20) {
		width -= 9; // Space for the selector
	}
	width -= margin * 2;

	const Rectangle rect { { sx, sy }, { width, 0 } };

	// Space reserved for item graphic is based on the size of 2x3 cursor sprites
	constexpr int CursWidth = INV_SLOT_SIZE_PX * 2;
	constexpr int HalfCursWidth = CursWidth / 2;

	if (*sgOptions.Gameplay.showItemGraphicsInStores && cursId >= 0) {
		const Size size = GetInvItemSize(static_cast<int>(CURSOR_FIRSTITEM) + cursId);
		const bool useHalfSize = size.width > INV_SLOT_SIZE_PX || size.height > INV_SLOT_SIZE_PX;
		const bool useRed = HasAnyOf(flags, UiFlags::ColorRed);
		const ClxSprite sprite = useHalfSize
		    ? (useRed ? GetHalfSizeItemSpriteRed(cursId) : GetHalfSizeItemSprite(cursId))
		    : GetInvItemSprite(static_cast<int>(CURSOR_FIRSTITEM) + cursId);
		const Point position {
			rect.position.x + (HalfCursWidth - sprite.width()) / 2,
			rect.position.y + (TextHeight() * 3 + sprite.height()) / 2
		};
		if (useHalfSize || !useRed) {
			ClxDraw(out, position, sprite);
		} else {
			ClxDrawTRN(out, position, sprite, GetInfravisionTRN());
		}
	}

	if (*sgOptions.Gameplay.showItemGraphicsInStores && cursIndent) {
		const Rectangle textRect { { rect.position.x + HalfCursWidth + 8, rect.position.y }, { rect.size.width - HalfCursWidth + 8, rect.size.height } };
		DrawString(out, text, textRect, { .flags = flags });
	} else {
		DrawString(out, text, rect, { .flags = flags });
	}

	if (price > 0)
		DrawString(out, FormatInteger(price), rect, { .flags = flags | UiFlags::AlignRight });

	if (currentTextLine == line) {
		DrawSelector(out, rect, text, flags);
	}
}

void DrawSLine(const Surface &out, int sy)
{
	const Point uiPosition = GetUIRectangle().position;
	int sx = 26;
	int width = 587;

	if (!isTextFullSize) {
		sx += SidePanelSize.width;
		width -= SidePanelSize.width;
	}

	uint8_t *src = out.at(uiPosition.x + sx, uiPosition.y + 25);
	uint8_t *dst = out.at(uiPosition.x + sx, sy);

	for (int i = 0; i < 3; i++, src += out.pitch(), dst += out.pitch())
		memcpy(dst, src, width);
}

void DrawSTextHelp()
{
	currentTextLine = -1;
	isTextFullSize = true;
}

void ClearSText(int s, int e)
{
	for (int i = s; i < e; i++) {
		textLine[i]._sx = 0;
		textLine[i]._syoff = 0;
		textLine[i].text.clear();
		textLine[i].text.shrink_to_fit();
		textLine[i].flags = UiFlags::None;
		textLine[i].type = STextStruct::Label;
		textLine[i]._sval = 0;
	}
}

void StartStore(TalkID s)
{
	if (*sgOptions.Gameplay.showItemGraphicsInStores) {
		CreateHalfSizeItemSprites();
	}
	SpellbookFlag = false;
	CloseInventory();
	CloseCharPanel();
	renderGold = false;
	QuestLogIsOpen = false;
	CloseGoldDrop();
	ClearSText(0, STORE_LINES);
	ReleaseStoreBtn();
	switch (s) {
	case TalkID::Smith:
		StartSmith();
		break;
	case TalkID::SmithBuy: {
		bool hasAnyItems = false;
		for (int i = 0; !smithItem[i].isEmpty(); i++) {
			hasAnyItems = true;
			break;
		}
		if (hasAnyItems)
			StartSmithBuy();
		else {
			activeStore = TalkID::SmithBuy;
			oldTextLine = 12;
			StoreESC();
			return;
		}
		break;
	}
	case TalkID::SmithSell:
		StartSmithSell();
		break;
	case TalkID::SmithRepair:
		StartSmithRepair();
		break;
	case TalkID::Witch:
		StartWitch();
		break;
	case TalkID::WitchBuy:
		if (currentItemIndex > 0)
			StartWitchBuy();
		break;
	case TalkID::WitchSell:
		StartWitchSell();
		break;
	case TalkID::WitchRecharge:
		StartWitchRecharge();
		break;
	case TalkID::NoMoney:
		StoreNoMoney();
		break;
	case TalkID::NoRoom:
		StoreNoRoom();
		break;
	case TalkID::Confirm:
		StoreConfirm(tempItem);
		break;
	case TalkID::Boy:
		StartBoy();
		break;
	case TalkID::BoyBuy:
		SStartBoyBuy();
		break;
	case TalkID::Healer:
		StartHealer();
		break;
	case TalkID::Storyteller:
		StartStoryteller();
		break;
	case TalkID::HealerBuy:
		if (currentItemIndex > 0)
			StartHealerBuy();
		break;
	case TalkID::StorytellerIdentify:
		StartStorytellerIdentify();
		break;
	case TalkID::SmithPremiumBuy:
		if (!StartSmithPremiumBuy())
			return;
		break;
	case TalkID::Gossip:
		StartTalk();
		break;
	case TalkID::StorytellerIdentifyShow:
		StartStorytellerIdentifyShow(tempItem);
		break;
	case TalkID::Tavern:
		StartTavern();
		break;
	case TalkID::Drunk:
		StartDrunk();
		break;
	case TalkID::Barmaid:
		StartBarmaid();
		break;
	case TalkID::None:
		break;
	}

	currentTextLine = -1;
	for (int i = 0; i < STORE_LINES; i++) {
		if (textLine[i].isSelectable()) {
			currentTextLine = i;
			break;
		}
	}

	activeStore = s;
}

void DrawSText(const Surface &out)
{
	if (!isTextFullSize)
		DrawSTextBack(out);
	else
		DrawQTextBack(out);

	if (hasScrollbar) {
		switch (activeStore) {
		case TalkID::SmithBuy:
			ScrollSmithBuy(scrollPos);
			break;
		case TalkID::SmithSell:
		case TalkID::SmithRepair:
		case TalkID::WitchSell:
		case TalkID::WitchRecharge:
		case TalkID::StorytellerIdentify:
			ScrollSmithSell(scrollPos);
			break;
		case TalkID::WitchBuy:
			ScrollWitchBuy(scrollPos);
			break;
		case TalkID::HealerBuy:
			ScrollHealerBuy(scrollPos);
			break;
		case TalkID::SmithPremiumBuy:
			ScrollSmithPremiumBuy(scrollPos);
			break;
		default:
			break;
		}
	}

	CalculateLineHeights();
	const Point uiPosition = GetUIRectangle().position;
	for (int i = 0; i < STORE_LINES; i++) {
		if (textLine[i].isDivider())
			DrawSLine(out, uiPosition.y + PaddingTop + textLine[i].y + TextHeight() / 2);
		else if (textLine[i].hasText())
			PrintSString(out, textLine[i]._sx, i, textLine[i].text, textLine[i].flags, textLine[i]._sval, textLine[i].cursId, textLine[i].cursIndent);
	}

	if (renderGold) {
		PrintSString(out, 28, 1, fmt::format(fmt::runtime(_("Your gold: {:s}")), FormatInteger(TotalPlayerGold())).c_str(), UiFlags::ColorWhitegold | UiFlags::AlignRight);
	}

	if (hasScrollbar)
		DrawSSlider(out, 4, 20);
}

void StoreESC()
{
	if (qtextflag) {
		qtextflag = false;
		if (leveltype == DTYPE_TOWN)
			stream_stop();
		return;
	}

	switch (activeStore) {
	case TalkID::Smith:
	case TalkID::Witch:
	case TalkID::Boy:
	case TalkID::BoyBuy:
	case TalkID::Healer:
	case TalkID::Storyteller:
	case TalkID::Tavern:
	case TalkID::Drunk:
	case TalkID::Barmaid:
		activeStore = TalkID::None;
		break;
	case TalkID::Gossip:
		StartStore(oldActiveStore);
		currentTextLine = oldTextLine;
		break;
	case TalkID::SmithBuy:
		StartStore(TalkID::Smith);
		currentTextLine = 12;
		break;
	case TalkID::SmithPremiumBuy:
		StartStore(TalkID::Smith);
		currentTextLine = 14;
		break;
	case TalkID::SmithSell:
		StartStore(TalkID::Smith);
		currentTextLine = 16;
		break;
	case TalkID::SmithRepair:
		StartStore(TalkID::Smith);
		currentTextLine = 18;
		break;
	case TalkID::WitchBuy:
		StartStore(TalkID::Witch);
		currentTextLine = 14;
		break;
	case TalkID::WitchSell:
		StartStore(TalkID::Witch);
		currentTextLine = 16;
		break;
	case TalkID::WitchRecharge:
		StartStore(TalkID::Witch);
		currentTextLine = 18;
		break;
	case TalkID::HealerBuy:
		StartStore(TalkID::Healer);
		currentTextLine = 14;
		break;
	case TalkID::StorytellerIdentify:
		StartStore(TalkID::Storyteller);
		currentTextLine = 14;
		break;
	case TalkID::StorytellerIdentifyShow:
		StartStore(TalkID::StorytellerIdentify);
		break;
	case TalkID::NoMoney:
	case TalkID::NoRoom:
	case TalkID::Confirm:
		StartStore(oldActiveStore);
		currentTextLine = oldTextLine;
		scrollPos = oldScrollPos;
		break;
	case TalkID::None:
		break;
	}
}

void StoreUp()
{
	PlaySFX(SfxID::MenuMove);
	if (currentTextLine == -1) {
		return;
	}

	if (hasScrollbar) {
		if (currentTextLine == previousScrollPos) {
			if (scrollPos != 0)
				scrollPos--;
			return;
		}

		currentTextLine--;
		while (!textLine[currentTextLine].isSelectable()) {
			if (currentTextLine == 0)
				currentTextLine = STORE_LINES - 1;
			else
				currentTextLine--;
		}
		return;
	}

	if (currentTextLine == 0)
		currentTextLine = STORE_LINES - 1;
	else
		currentTextLine--;

	while (!textLine[currentTextLine].isSelectable()) {
		if (currentTextLine == 0)
			currentTextLine = STORE_LINES - 1;
		else
			currentTextLine--;
	}
}

void StoreDown()
{
	PlaySFX(SfxID::MenuMove);
	if (currentTextLine == -1) {
		return;
	}

	if (hasScrollbar) {
		if (currentTextLine == nextScrollPos) {
			if (scrollPos < numTextLines)
				scrollPos++;
			return;
		}

		currentTextLine++;
		while (!textLine[currentTextLine].isSelectable()) {
			if (currentTextLine == STORE_LINES - 1)
				currentTextLine = 0;
			else
				currentTextLine++;
		}
		return;
	}

	if (currentTextLine == STORE_LINES - 1)
		currentTextLine = 0;
	else
		currentTextLine++;

	while (!textLine[currentTextLine].isSelectable()) {
		if (currentTextLine == STORE_LINES - 1)
			currentTextLine = 0;
		else
			currentTextLine++;
	}
}

void StorePrior()
{
	PlaySFX(SfxID::MenuMove);
	if (currentTextLine != -1 && hasScrollbar) {
		if (currentTextLine == previousScrollPos) {
			scrollPos = std::max(scrollPos - 4, 0);
		} else {
			currentTextLine = previousScrollPos;
		}
	}
}

void StoreNext()
{
	PlaySFX(SfxID::MenuMove);
	if (currentTextLine != -1 && hasScrollbar) {
		if (currentTextLine == nextScrollPos) {
			if (scrollPos < numTextLines)
				scrollPos += 4;
			if (scrollPos > numTextLines)
				scrollPos = numTextLines;
		} else {
			currentTextLine = nextScrollPos;
		}
	}
}

void TakePlrsMoney(int cost)
{
	Player &myPlayer = *MyPlayer;

	myPlayer._pGold -= std::min(cost, myPlayer._pGold);

	cost = TakeGold(myPlayer, cost, true);
	if (cost != 0) {
		cost = TakeGold(myPlayer, cost, false);
	}

	Stash.gold -= cost;
	Stash.dirty = true;
}

void StoreEnter()
{
	if (qtextflag) {
		qtextflag = false;
		if (leveltype == DTYPE_TOWN)
			stream_stop();

		return;
	}

	PlaySFX(SfxID::MenuSelect);
	switch (activeStore) {
	case TalkID::Smith:
		SmithEnter();
		break;
	case TalkID::SmithPremiumBuy:
		SmithPremiumBuyEnter();
		break;
	case TalkID::SmithBuy:
		SmithBuyEnter();
		break;
	case TalkID::SmithSell:
		SmithSellEnter();
		break;
	case TalkID::SmithRepair:
		SmithRepairEnter();
		break;
	case TalkID::Witch:
		WitchEnter();
		break;
	case TalkID::WitchBuy:
		WitchBuyEnter();
		break;
	case TalkID::WitchSell:
		WitchSellEnter();
		break;
	case TalkID::WitchRecharge:
		WitchRechargeEnter();
		break;
	case TalkID::NoMoney:
	case TalkID::NoRoom:
		StartStore(oldActiveStore);
		currentTextLine = oldTextLine;
		scrollPos = oldScrollPos;
		break;
	case TalkID::Confirm:
		ConfirmEnter(tempItem);
		break;
	case TalkID::Boy:
		BoyEnter();
		break;
	case TalkID::BoyBuy:
		BoyBuyEnter();
		break;
	case TalkID::Healer:
		HealerEnter();
		break;
	case TalkID::Storyteller:
		StorytellerEnter();
		break;
	case TalkID::HealerBuy:
		HealerBuyEnter();
		break;
	case TalkID::StorytellerIdentify:
		StorytellerIdentifyEnter();
		break;
	case TalkID::Gossip:
		TalkEnter();
		break;
	case TalkID::StorytellerIdentifyShow:
		StartStore(TalkID::StorytellerIdentify);
		break;
	case TalkID::Drunk:
		DrunkEnter();
		break;
	case TalkID::Tavern:
		TavernEnter();
		break;
	case TalkID::Barmaid:
		BarmaidEnter();
		break;
	case TalkID::None:
		break;
	}
}

void CheckStoreBtn()
{
	const Point uiPosition = GetUIRectangle().position;
	const Rectangle windowRect { { uiPosition.x + 344, uiPosition.y + PaddingTop - 7 }, { 271, 303 } };
	const Rectangle windowRectFull { { uiPosition.x + 24, uiPosition.y + PaddingTop - 7 }, { 591, 303 } };

	if (!isTextFullSize) {
		if (!windowRect.contains(MousePosition)) {
			while (activeStore != TalkID::None)
				StoreESC();
		}
	} else {
		if (!windowRectFull.contains(MousePosition)) {
			while (activeStore != TalkID::None)
				StoreESC();
		}
	}

	if (qtextflag) {
		qtextflag = false;
		if (leveltype == DTYPE_TOWN)
			stream_stop();
	} else if (currentTextLine != -1) {
		const int relativeY = MousePosition.y - (uiPosition.y + PaddingTop);

		if (hasScrollbar && MousePosition.x > 600 + uiPosition.x) {
			// Scroll bar is always measured in terms of the small line height.
			int y = relativeY / SmallLineHeight;
			if (y == 4) {
				if (countdownScrollUp <= 0) {
					StoreUp();
					countdownScrollUp = 10;
				} else {
					countdownScrollUp--;
				}
			}
			if (y == 20) {
				if (countdownScrollDown <= 0) {
					StoreDown();
					countdownScrollDown = 10;
				} else {
					countdownScrollDown--;
				}
			}
			return;
		}

		int y = relativeY / LineHeight();

		// Large small fonts draw beyond LineHeight. Check if the click was on the overflow text.
		if (IsSmallFontTall() && y > 0 && y < STORE_LINES
		    && textLine[y - 1].hasText() && !textLine[y].hasText()
		    && relativeY < textLine[y - 1].y + LargeTextHeight) {
			--y;
		}

		if (y >= 5) {
			if (y >= BackButtonLine() + 1)
				y = BackButtonLine();
			if (hasScrollbar && y <= 20 && !textLine[y].isSelectable()) {
				if (textLine[y - 2].isSelectable()) {
					y -= 2;
				} else if (textLine[y - 1].isSelectable()) {
					y--;
				}
			}
			if (textLine[y].isSelectable() || (hasScrollbar && y == BackButtonLine())) {
				currentTextLine = y;
				StoreEnter();
			}
		}
	}
}

void ReleaseStoreBtn()
{
	countdownScrollUp = -1;
	countdownScrollDown = -1;
}

} // namespace devilution
