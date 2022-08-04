/**
 * @file stores.cpp
 *
 * Implementation of functionality for stores and towner dialogs.
 */
#include "stores.h"

#include <algorithm>

#include <fmt/format.h>

#include "controls/plrctrls.h"
#include "cursor.h"
#include "engine/load_cel.hpp"
#include "engine/random.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/render/text_render.hpp"
#include "init.h"
#include "minitext.h"
#include "options.h"
#include "panels/info_box.hpp"
#include "qol/stash.h"
#include "towners.h"
#include "utils/format_int.hpp"
#include "utils/language.h"
#include "utils/stdcompat/string_view.hpp"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

namespace devilution {

talk_id stextflag;

int storenumh;
char storehidx[48];
Item storehold[48];

Item smithitem[SMITH_ITEMS];
int numpremium;
int premiumlevel;
Item premiumitems[SMITH_PREMIUM_ITEMS];

Item healitem[20];

Item witchitem[WITCH_ITEMS];

int boylevel;
Item boyitem;

namespace {

/** The current towner being interacted with */
_talker_id talker;

/** Is the current dialog full size */
bool stextsize;

/** Number of text lines in the current dialog */
int stextsmax;
/** Remember currently selected text line from stext while displaying a dialog */
int stextlhold;
/** Currently selected text line from stext */
int stextsel;

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
STextStruct stext[STORE_LINES];

/** Whether to render the player's gold amount in the top left */
bool RenderGold;

/** Does the current panel have a scrollbar */
bool stextscrl;
/** Remember last scoll position */
int stextvhold;
/** Scoll position */
int stextsval;
/** Next scoll position */
int stextdown;
/** Previous scoll position */
int stextup;
/** Count down for the push state of the scroll up button */
char stextscrlubtn;
/** Count down for the push state of the scroll down button */
char stextscrldbtn;

/** Remember current store while displaying a dialog */
talk_id stextshold;

/** Start of possible gossip dialogs for current store */
_speech_id gossipstart;
/** End of possible gossip dialogs for current store */
_speech_id gossipend;

/** Temporary item used to hold the the item being traided */
Item StoreItem;

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
		return stextscrl ? 21 : 20;
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
	stext[0].y = 0;
	if (IsSmallFontTall()) {
		for (int i = 1; i < STORE_LINES; ++i) {
			// Space out consecutive text lines, unless they are both selectable (never the case currently).
			if (stext[i].hasText() && stext[i - 1].hasText() && !(stext[i].isSelectable() && stext[i - 1].isSelectable())) {
				stext[i].y = stext[i - 1].y + LargeTextHeight;
			} else {
				stext[i].y = i * LargeLineHeight;
			}
		}
	} else {
		for (int i = 1; i < STORE_LINES; ++i) {
			stext[i].y = i * SmallLineHeight;
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
	if (stextscrlubtn != -1)
		ClxDraw(out, { uiPosition.x + 601, yd1 }, (*pSTextSlidCels)[11]);
	else
		ClxDraw(out, { uiPosition.x + 601, yd1 }, (*pSTextSlidCels)[9]);
	if (stextscrldbtn != -1)
		ClxDraw(out, { uiPosition.x + 601, yd2 }, (*pSTextSlidCels)[10]);
	else
		ClxDraw(out, { uiPosition.x + 601, yd2 }, (*pSTextSlidCels)[8]);
	yd1 += 12;
	int yd3 = yd1;
	for (; yd3 < yd2; yd3 += 12) {
		ClxDraw(out, { uiPosition.x + 601, yd3 }, (*pSTextSlidCels)[13]);
	}
	if (stextsel == BackButtonLine())
		yd3 = stextlhold;
	else
		yd3 = stextsel;
	if (storenumh > 1)
		yd3 = 1000 * (stextsval + ((yd3 - stextup) / 4)) / (storenumh - 1) * (y2 * 12 - y1 * 12 - 24) / 1000;
	else
		yd3 = 0;
	ClxDraw(out, { uiPosition.x + 601, (y1 + 1) * 12 + 44 + uiPosition.y + yd3 }, (*pSTextSlidCels)[12]);
}

void AddSLine(size_t y)
{
	stext[y]._sx = 0;
	stext[y]._syoff = 0;
	stext[y].text.clear();
	stext[y].text.shrink_to_fit();
	stext[y].type = STextStruct::Divider;
}

void AddSTextVal(size_t y, int val)
{
	stext[y]._sval = val;
}

void AddSText(uint8_t x, size_t y, string_view text, UiFlags flags, bool sel)
{
	stext[y]._sx = x;
	stext[y]._syoff = 0;
	stext[y].text.clear();
	AppendStrView(stext[y].text, text);
	stext[y].flags = flags;
	stext[y].type = sel ? STextStruct::Selectable : STextStruct::Label;
}

void AddOptionsBackButton()
{
	const int line = BackButtonLine();
	AddSText(0, line, _("Back"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	stext[line]._syoff = IsSmallFontTall() ? 0 : 6;
}

void AddItemListBackButton(bool selectable = false)
{
	const int line = BackButtonLine();
	string_view text = _("Back");
	if (!selectable && IsSmallFontTall()) {
		AddSText(0, line, text, UiFlags::ColorWhite | UiFlags::AlignRight, selectable);
	} else {
		AddSLine(line - 1);
		AddSText(0, line, text, UiFlags::ColorWhite | UiFlags::AlignCenter, selectable);
		stext[line]._syoff = 6;
	}
}

void PrintStoreItem(const Item &item, int l, UiFlags flags)
{
	std::string productLine;

	if (item._iIdentified) {
		if (item._iMagical != ITEM_QUALITY_UNIQUE) {
			if (item._iPrePower != -1) {
				AppendStrView(productLine, PrintItemPower(item._iPrePower, item));
			}
		}
		if (item._iSufPower != -1) {
			if (!productLine.empty())
				AppendStrView(productLine, _(",  "));
			AppendStrView(productLine, PrintItemPower(item._iSufPower, item));
		}
	}
	if (item._iMiscId == IMISC_STAFF && item._iMaxCharges != 0) {
		if (!productLine.empty())
			AppendStrView(productLine, _(",  "));
		productLine.append(fmt::format(fmt::runtime(_("Charges: {:d}/{:d}")), item._iCharges, item._iMaxCharges));
	}
	if (!productLine.empty()) {
		AddSText(40, l, productLine, flags, false);
		l++;
		productLine.clear();
	}

	if (item._itype != ItemType::Misc) {
		if (item._iClass == ICLASS_WEAPON)
			productLine = fmt::format(fmt::runtime(_("Damage: {:d}-{:d}  ")), item._iMinDam, item._iMaxDam);
		else if (item._iClass == ICLASS_ARMOR)
			productLine = fmt::format(fmt::runtime(_("Armor: {:d}  ")), item._iAC);
		if (item._iMaxDur != DUR_INDESTRUCTIBLE && item._iMaxDur != 0)
			productLine += fmt::format(fmt::runtime(_("Dur: {:d}/{:d},  ")), item._iDurability, item._iMaxDur);
		else
			AppendStrView(productLine, _("Indestructible,  "));
	}

	int8_t str = item._iMinStr;
	uint8_t mag = item._iMinMag;
	int8_t dex = item._iMinDex;

	if (str == 0 && mag == 0 && dex == 0) {
		AppendStrView(productLine, _("No required attributes"));
	} else {
		AppendStrView(productLine, _("Required:"));
		if (str != 0)
			productLine.append(fmt::format(fmt::runtime(_(" {:d} Str")), str));
		if (mag != 0)
			productLine.append(fmt::format(fmt::runtime(_(" {:d} Mag")), mag));
		if (dex != 0)
			productLine.append(fmt::format(fmt::runtime(_(" {:d} Dex")), dex));
	}
	AddSText(40, l++, productLine, flags, false);
}

bool StoreAutoPlace(Item &item, bool persistItem)
{
	Player &player = *MyPlayer;

	if (AutoEquipEnabled(player, item) && AutoEquip(player, item, persistItem)) {
		return true;
	}

	if (AutoPlaceItemInBelt(player, item, persistItem)) {
		return true;
	}

	return AutoPlaceItemInInventory(player, item, persistItem);
}

void StartSmith()
{
	stextsize = false;
	stextscrl = false;
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
	storenumh = 20;
}

void ScrollSmithBuy(int idx)
{
	ClearSText(5, 21);
	stextup = 5;

	for (int l = 5; l < 20; l += 4) {
		if (!smithitem[idx].isEmpty()) {
			UiFlags itemColor = smithitem[idx].getTextColorWithStatCheck();

			if (smithitem[idx]._iMagical != ITEM_QUALITY_NORMAL) {
				AddSText(20, l, smithitem[idx]._iIName, itemColor, true);
			} else {
				AddSText(20, l, smithitem[idx]._iName, itemColor, true);
			}

			AddSTextVal(l, smithitem[idx]._iIvalue);
			PrintStoreItem(smithitem[idx], l + 1, itemColor);
			stextdown = l;
			idx++;
		}
	}

	if (stextsel != -1 && !stext[stextsel].isSelectable() && stextsel != BackButtonLine())
		stextsel = stextdown;
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
	stextsize = true;
	stextscrl = true;
	stextsval = 0;

	RenderGold = true;
	AddSText(20, 1, _("I have these items for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollSmithBuy(stextsval);
	AddItemListBackButton();

	storenumh = 0;
	for (Item &item : smithitem) {
		if (item.isEmpty())
			continue;

		item._iStatFlag = MyPlayer->CanUseItem(item);
		storenumh++;
	}

	stextsmax = std::max(storenumh - 4, 0);
}

void ScrollSmithPremiumBuy(int boughtitems)
{
	ClearSText(5, 21);
	stextup = 5;

	int idx = 0;
	for (; boughtitems != 0; idx++) {
		if (!premiumitems[idx].isEmpty())
			boughtitems--;
	}

	for (int l = 5; l < 20 && idx < SMITH_PREMIUM_ITEMS; l += 4) {
		if (!premiumitems[idx].isEmpty()) {
			UiFlags itemColor = premiumitems[idx].getTextColorWithStatCheck();
			AddSText(20, l, premiumitems[idx]._iIName, itemColor, true);
			AddSTextVal(l, premiumitems[idx]._iIvalue);
			PrintStoreItem(premiumitems[idx], l + 1, itemColor);
			stextdown = l;
		} else {
			l -= 4;
		}
		idx++;
	}
	if (stextsel != -1 && !stext[stextsel].isSelectable() && stextsel != BackButtonLine())
		stextsel = stextdown;
}

bool StartSmithPremiumBuy()
{
	storenumh = 0;
	for (Item &item : premiumitems) {
		if (item.isEmpty())
			continue;

		item._iStatFlag = MyPlayer->CanUseItem(item);
		storenumh++;
	}
	if (storenumh == 0) {
		StartStore(STORE_SMITH);
		stextsel = 14;
		return false;
	}

	stextsize = true;
	stextscrl = true;
	stextsval = 0;

	RenderGold = true;
	AddSText(20, 1, _("I have these premium items for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	AddItemListBackButton();

	stextsmax = std::max(storenumh - 4, 0);

	ScrollSmithPremiumBuy(stextsval);

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
	ClearSText(5, 21);
	stextup = 5;

	for (int l = 5; l < 20; l += 4) {
		if (idx >= storenumh)
			break;
		if (!storehold[idx].isEmpty()) {
			UiFlags itemColor = storehold[idx].getTextColorWithStatCheck();

			if (storehold[idx]._iMagical != ITEM_QUALITY_NORMAL && storehold[idx]._iIdentified) {
				AddSText(20, l, storehold[idx]._iIName, itemColor, true);
				AddSTextVal(l, storehold[idx]._iIvalue);
			} else {
				AddSText(20, l, storehold[idx]._iName, itemColor, true);
				AddSTextVal(l, storehold[idx]._ivalue);
			}

			PrintStoreItem(storehold[idx], l + 1, itemColor);
			stextdown = l;
		}
		idx++;
	}

	stextsmax = std::max(storenumh - 4, 0);
}

void StartSmithSell()
{
	stextsize = true;
	bool sellOk = false;
	storenumh = 0;

	for (auto &item : storehold) {
		item.clear();
	}

	const Player &myPlayer = *MyPlayer;

	for (int8_t i = 0; i < myPlayer._pNumInv; i++) {
		if (storenumh >= 48)
			break;
		if (SmithSellOk(i)) {
			sellOk = true;
			storehold[storenumh] = myPlayer.InvList[i];

			if (storehold[storenumh]._iMagical != ITEM_QUALITY_NORMAL && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			storehold[storenumh]._ivalue = std::max(storehold[storenumh]._ivalue / 4, 1);
			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh] = i;
			storenumh++;
		}
	}

	for (int i = 0; i < MaxBeltItems; i++) {
		if (storenumh >= 48)
			break;
		if (SmithSellOk(-(i + 1))) {
			sellOk = true;
			storehold[storenumh] = myPlayer.SpdList[i];

			if (storehold[storenumh]._iMagical != ITEM_QUALITY_NORMAL && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			storehold[storenumh]._ivalue = std::max(storehold[storenumh]._ivalue / 4, 1);
			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh] = -(i + 1);
			storenumh++;
		}
	}

	if (!sellOk) {
		stextscrl = false;

		RenderGold = true;
		AddSText(20, 1, _("You have nothing I want."), UiFlags::ColorWhitegold, false);
		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	RenderGold = true;
	AddSText(20, 1, _("Which item is for sale?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollSmithSell(stextsval);
	AddItemListBackButton();
}

bool SmithRepairOk(int i)
{
	const Player &myPlayer = *MyPlayer;

	if (myPlayer.InvList[i].isEmpty())
		return false;
	if (myPlayer.InvList[i]._itype == ItemType::Misc)
		return false;
	if (myPlayer.InvList[i]._itype == ItemType::Gold)
		return false;
	if (myPlayer.InvList[i]._iDurability == myPlayer.InvList[i]._iMaxDur)
		return false;

	return true;
}

void StartSmithRepair()
{
	stextsize = true;
	storenumh = 0;

	for (auto &item : storehold) {
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
		if (storenumh >= 48)
			break;
		if (SmithRepairOk(i)) {
			AddStoreHoldRepair(&myPlayer.InvList[i], i);
		}
	}

	if (storenumh == 0) {
		stextscrl = false;

		RenderGold = true;
		AddSText(20, 1, _("You have nothing to repair."), UiFlags::ColorWhitegold, false);
		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	RenderGold = true;
	AddSText(20, 1, _("Repair which item?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);

	ScrollSmithSell(stextsval);
	AddItemListBackButton();
}

void FillManaPlayer()
{
	if (!*sgOptions.Gameplay.adriaRefillsMana)
		return;

	Player &myPlayer = *MyPlayer;

	if (myPlayer._pMana != myPlayer._pMaxMana) {
		PlaySFX(IS_CAST8);
	}
	myPlayer._pMana = myPlayer._pMaxMana;
	myPlayer._pManaBase = myPlayer._pMaxManaBase;
	drawmanaflag = true;
}

void StartWitch()
{
	FillManaPlayer();
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("Witch's shack"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Adria"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Buy items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 16, _("Sell items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Recharge staves"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 20, _("Leave the shack"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	storenumh = 20;
}

void ScrollWitchBuy(int idx)
{
	ClearSText(5, 21);
	stextup = 5;

	for (int l = 5; l < 20; l += 4) {
		if (!witchitem[idx].isEmpty()) {
			UiFlags itemColor = witchitem[idx].getTextColorWithStatCheck();

			if (witchitem[idx]._iMagical != ITEM_QUALITY_NORMAL) {
				AddSText(20, l, witchitem[idx]._iIName, itemColor, true);
			} else {
				AddSText(20, l, witchitem[idx]._iName, itemColor, true);
			}

			AddSTextVal(l, witchitem[idx]._iIvalue);
			PrintStoreItem(witchitem[idx], l + 1, itemColor);
			stextdown = l;
			idx++;
		}
	}

	if (stextsel != -1 && !stext[stextsel].isSelectable() && stextsel != BackButtonLine())
		stextsel = stextdown;
}

void WitchBookLevel(Item &bookItem)
{
	if (bookItem._iMiscId != IMISC_BOOK)
		return;
	bookItem._iMinMag = spelldata[bookItem._iSpell].sMinInt;
	int8_t spellLevel = MyPlayer->_pSplLvl[bookItem._iSpell];
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
	stextsize = true;
	stextscrl = true;
	stextsval = 0;
	stextsmax = 20;

	RenderGold = true;
	AddSText(20, 1, _("I have these items for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollWitchBuy(stextsval);
	AddItemListBackButton();

	storenumh = 0;
	for (Item &item : witchitem) {
		if (item.isEmpty())
			continue;

		WitchBookLevel(item);
		item._iStatFlag = MyPlayer->CanUseItem(item);
		storenumh++;
	}
	stextsmax = std::max(storenumh - 4, 0);
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
	stextsize = true;
	bool sellok = false;
	storenumh = 0;

	for (auto &item : storehold) {
		item.clear();
	}

	const Player &myPlayer = *MyPlayer;

	for (int i = 0; i < myPlayer._pNumInv; i++) {
		if (storenumh >= 48)
			break;
		if (WitchSellOk(i)) {
			sellok = true;
			storehold[storenumh] = myPlayer.InvList[i];

			if (storehold[storenumh]._iMagical != ITEM_QUALITY_NORMAL && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			storehold[storenumh]._ivalue = std::max(storehold[storenumh]._ivalue / 4, 1);
			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh] = i;
			storenumh++;
		}
	}

	for (int i = 0; i < MaxBeltItems; i++) {
		if (storenumh >= 48)
			break;
		if (!myPlayer.SpdList[i].isEmpty() && WitchSellOk(-(i + 1))) {
			sellok = true;
			storehold[storenumh] = myPlayer.SpdList[i];

			if (storehold[storenumh]._iMagical != ITEM_QUALITY_NORMAL && storehold[storenumh]._iIdentified)
				storehold[storenumh]._ivalue = storehold[storenumh]._iIvalue;

			storehold[storenumh]._ivalue = std::max(storehold[storenumh]._ivalue / 4, 1);
			storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
			storehidx[storenumh] = -(i + 1);
			storenumh++;
		}
	}

	if (!sellok) {
		stextscrl = false;

		RenderGold = true;
		AddSText(20, 1, _("You have nothing I want."), UiFlags::ColorWhitegold, false);

		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	RenderGold = true;
	AddSText(20, 1, _("Which item is for sale?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollSmithSell(stextsval);
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
	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue += spelldata[itm._iSpell].sStaffCost;
	storehold[storenumh]._ivalue = storehold[storenumh]._ivalue * (storehold[storenumh]._iMaxCharges - storehold[storenumh]._iCharges) / (storehold[storenumh]._iMaxCharges * 2);
	storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
	storehidx[storenumh] = i;
	storenumh++;
}

void StartWitchRecharge()
{
	stextsize = true;
	bool rechargeok = false;
	storenumh = 0;

	for (auto &item : storehold) {
		item.clear();
	}

	const Player &myPlayer = *MyPlayer;
	const auto &leftHand = myPlayer.InvBody[INVLOC_HAND_LEFT];

	if ((leftHand._itype == ItemType::Staff || leftHand._iMiscId == IMISC_UNIQUE) && leftHand._iCharges != leftHand._iMaxCharges) {
		rechargeok = true;
		AddStoreHoldRecharge(leftHand, -1);
	}

	for (int i = 0; i < myPlayer._pNumInv; i++) {
		if (storenumh >= 48)
			break;
		if (WitchRechargeOk(i)) {
			rechargeok = true;
			AddStoreHoldRecharge(myPlayer.InvList[i], i);
		}
	}

	if (!rechargeok) {
		stextscrl = false;

		RenderGold = true;
		AddSText(20, 1, _("You have nothing to recharge."), UiFlags::ColorWhitegold, false);
		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	RenderGold = true;
	AddSText(20, 1, _("Recharge which item?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);
	ScrollSmithSell(stextsval);
	AddItemListBackButton();
}

void StoreNoMoney()
{
	StartStore(stextshold);
	stextscrl = false;
	stextsize = true;
	RenderGold = true;
	ClearSText(5, 23);
	AddSText(0, 14, _("You do not have enough gold"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
}

void StoreNoRoom()
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);
	AddSText(0, 14, _("You do not have enough room in inventory"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
}

void StoreConfirm(Item &item)
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);

	UiFlags itemColor = item.getTextColorWithStatCheck();

	bool idprint = item._iMagical != ITEM_QUALITY_NORMAL;

	if (stextshold == STORE_SIDENTIFY)
		idprint = false;
	if (item._iMagical != ITEM_QUALITY_NORMAL && !item._iIdentified) {
		if (stextshold == STORE_SSELL)
			idprint = false;
		if (stextshold == STORE_WSELL)
			idprint = false;
		if (stextshold == STORE_SREPAIR)
			idprint = false;
		if (stextshold == STORE_WRECHARGE)
			idprint = false;
	}
	if (idprint)
		AddSText(20, 8, item._iIName, itemColor, false);
	else
		AddSText(20, 8, item._iName, itemColor, false);

	AddSTextVal(8, item._iIvalue);
	PrintStoreItem(item, 9, itemColor);

	string_view prompt;

	switch (stextshold) {
	case STORE_BBOY:
		prompt = _("Do we have a deal?");
		break;
	case STORE_SIDENTIFY:
		prompt = _("Are you sure you want to identify this item?");
		break;
	case STORE_HBUY:
	case STORE_SPBUY:
	case STORE_WBUY:
	case STORE_SBUY:
		prompt = _("Are you sure you want to buy this item?");
		break;
	case STORE_WRECHARGE:
		prompt = _("Are you sure you want to recharge this item?");
		break;
	case STORE_SSELL:
	case STORE_WSELL:
		prompt = _("Are you sure you want to sell this item?");
		break;
	case STORE_SREPAIR:
		prompt = _("Are you sure you want to repair this item?");
		break;
	default:
		app_fatal(StrCat("Unknown store dialog ", static_cast<int>(stextshold)));
	}
	AddSText(0, 15, prompt, UiFlags::ColorWhite | UiFlags::AlignCenter, false);
	AddSText(0, 18, _("Yes"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 20, _("No"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
}

void StartBoy()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("Wirt the Peg-legged boy"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSLine(5);
	if (!boyitem.isEmpty()) {
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
	stextsize = true;
	stextscrl = false;

	RenderGold = true;
	AddSText(20, 1, _("I have this item for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);

	boyitem._iStatFlag = MyPlayer->CanUseItem(boyitem);
	UiFlags itemColor = boyitem.getTextColorWithStatCheck();

	if (boyitem._iMagical != ITEM_QUALITY_NORMAL)
		AddSText(20, 10, boyitem._iIName, itemColor, true);
	else
		AddSText(20, 10, boyitem._iName, itemColor, true);

	if (gbIsHellfire)
		AddSTextVal(10, boyitem._iIvalue - (boyitem._iIvalue / 4));
	else
		AddSTextVal(10, boyitem._iIvalue + (boyitem._iIvalue / 2));
	PrintStoreItem(boyitem, 11, itemColor);

	{
		// Add a Leave button. Unlike the other item list back buttons,
		// this one has different text and different layout in LargerSmallFont locales.
		const int line = BackButtonLine();
		AddSLine(line - 1);
		AddSText(0, line, _("Leave"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
		stext[line]._syoff = 6;
	}
}

void HealPlayer()
{
	Player &myPlayer = *MyPlayer;

	if (myPlayer._pHitPoints != myPlayer._pMaxHP) {
		PlaySFX(IS_CAST8);
	}
	myPlayer._pHitPoints = myPlayer._pMaxHP;
	myPlayer._pHPBase = myPlayer._pMaxHPBase;
	drawhpflag = true;
}

void StartHealer()
{
	HealPlayer();
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, _("Welcome to the"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 3, _("Healer's home"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Pepin"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Buy items"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Leave Healer's home"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	storenumh = 20;
}

void ScrollHealerBuy(int idx)
{
	ClearSText(5, 21);
	stextup = 5;
	for (int l = 5; l < 20; l += 4) {
		if (!healitem[idx].isEmpty()) {
			UiFlags itemColor = healitem[idx].getTextColorWithStatCheck();

			AddSText(20, l, healitem[idx]._iName, itemColor, true);
			AddSTextVal(l, healitem[idx]._iIvalue);
			PrintStoreItem(healitem[idx], l + 1, itemColor);
			stextdown = l;
			idx++;
		}
	}

	if (stextsel != -1 && !stext[stextsel].isSelectable() && stextsel != BackButtonLine())
		stextsel = stextdown;
}

void StartHealerBuy()
{
	stextsize = true;
	stextscrl = true;
	stextsval = 0;

	RenderGold = true;
	AddSText(20, 1, _("I have these items for sale:"), UiFlags::ColorWhitegold, false);
	AddSLine(3);

	ScrollHealerBuy(stextsval);
	AddItemListBackButton();

	storenumh = 0;
	for (Item &item : healitem) {
		if (item.isEmpty())
			continue;

		item._iStatFlag = MyPlayer->CanUseItem(item);
		storenumh++;
	}

	stextsmax = std::max(storenumh - 4, 0);
}

void StartStoryteller()
{
	stextsize = false;
	stextscrl = false;
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
	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue = 100;
	storehold[storenumh]._iIvalue = 100;
	storehidx[storenumh] = i;
	storenumh++;
}

void StartStorytellerIdentify()
{
	bool idok = false;
	stextsize = true;
	storenumh = 0;

	for (auto &item : storehold) {
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
		if (storenumh >= 48)
			break;
		auto &item = myPlayer.InvList[i];
		if (IdItemOk(&item)) {
			idok = true;
			AddStoreHoldId(item, i);
		}
	}

	if (!idok) {
		stextscrl = false;

		RenderGold = true;
		AddSText(20, 1, _("You have nothing to identify."), UiFlags::ColorWhitegold, false);
		AddSLine(3);
		AddItemListBackButton(/*selectable=*/true);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	RenderGold = true;
	AddSText(20, 1, _("Identify which item?"), UiFlags::ColorWhitegold, false);
	AddSLine(3);

	ScrollSmithSell(stextsval);
	AddItemListBackButton();
}

void StartStorytellerIdentifyShow(Item &item)
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);

	UiFlags itemColor = item.getTextColorWithStatCheck();

	AddSText(0, 7, _("This item is:"), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
	AddSText(20, 11, item._iIName, itemColor, false);
	PrintStoreItem(item, 12, itemColor);
	AddSText(0, 18, _("Done"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
}

void StartTalk()
{
	int la;

	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, fmt::format(fmt::runtime(_("Talk to {:s}")), _(TownerNames[talker])), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSLine(5);
	if (gbIsSpawn) {
		AddSText(0, 10, fmt::format(fmt::runtime(_("Talking to {:s}")), _(TownerNames[talker])), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
		AddSText(0, 12, _("is not available"), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
		AddSText(0, 14, _("in the shareware"), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
		AddSText(0, 16, _("version"), UiFlags::ColorWhite | UiFlags::AlignCenter, false);
		AddOptionsBackButton();
		return;
	}

	int sn = 0;
	for (auto &quest : Quests) {
		if (quest._qactive == QUEST_ACTIVE && QuestDialogTable[talker][quest._qidx] != TEXT_NONE && quest._qlog)
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
		if (quest._qactive == QUEST_ACTIVE && QuestDialogTable[talker][quest._qidx] != TEXT_NONE && quest._qlog) {
			AddSText(0, sn, _(QuestsData[quest._qidx]._qlstr), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
			sn += la;
		}
	}
	AddSText(0, sn2, _("Gossip"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddOptionsBackButton();
}

void StartTavern()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, _("Welcome to the"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 3, _("Rising Sun"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Ogden"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Leave the tavern"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	storenumh = 20;
}

void StartBarmaid()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("Gillian"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Gillian"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Access Storage"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Say goodbye"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	storenumh = 20;
}

void StartDrunk()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("Farnham the Drunk"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorWhitegold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Farnham"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Say Goodbye"), UiFlags::ColorWhite | UiFlags::AlignCenter, true);
	AddSLine(5);
	storenumh = 20;
}

void SmithEnter()
{
	switch (stextsel) {
	case 10:
		talker = TOWN_SMITH;
		stextlhold = 10;
		stextshold = STORE_SMITH;
		gossipstart = TEXT_GRISWOLD2;
		gossipend = TEXT_GRISWOLD13;
		StartStore(STORE_GOSSIP);
		break;
	case 12:
		StartStore(STORE_SBUY);
		break;
	case 14:
		StartStore(STORE_SPBUY);
		break;
	case 16:
		StartStore(STORE_SSELL);
		break;
	case 18:
		StartStore(STORE_SREPAIR);
		break;
	case 20:
		stextflag = STORE_NONE;
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
	int idx = stextvhold + ((stextlhold - stextup) / 4);
	if (idx == SMITH_ITEMS - 1) {
		smithitem[SMITH_ITEMS - 1].clear();
	} else {
		for (; !smithitem[idx + 1].isEmpty(); idx++) {
			smithitem[idx] = std::move(smithitem[idx + 1]);
		}
		smithitem[idx].clear();
	}
	CalcPlrInv(*MyPlayer, true);
}

void SmithBuyEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(STORE_SMITH);
		stextsel = 12;
		return;
	}

	stextlhold = stextsel;
	stextvhold = stextsval;
	stextshold = STORE_SBUY;

	int idx = stextsval + ((stextsel - stextup) / 4);
	if (!PlayerCanAfford(smithitem[idx]._iIvalue)) {
		StartStore(STORE_NOMONEY);
		return;
	}

	if (!StoreAutoPlace(smithitem[idx], false)) {
		StartStore(STORE_NOROOM);
		return;
	}

	StoreItem = smithitem[idx];
	StartStore(STORE_CONFIRM);
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

	int idx = stextvhold + ((stextlhold - stextup) / 4);
	int xx = 0;
	for (int i = 0; idx >= 0; i++) {
		if (!premiumitems[i].isEmpty()) {
			idx--;
			xx = i;
		}
	}

	premiumitems[xx].clear();
	numpremium--;
	SpawnPremium(*MyPlayer);
}

void SmithPremiumBuyEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(STORE_SMITH);
		stextsel = 14;
		return;
	}

	stextshold = STORE_SPBUY;
	stextlhold = stextsel;
	stextvhold = stextsval;

	int xx = stextsval + ((stextsel - stextup) / 4);
	int idx = 0;
	for (int i = 0; xx >= 0; i++) {
		if (!premiumitems[i].isEmpty()) {
			xx--;
			idx = i;
		}
	}

	if (!PlayerCanAfford(premiumitems[idx]._iIvalue)) {
		StartStore(STORE_NOMONEY);
		return;
	}

	if (!StoreAutoPlace(premiumitems[idx], false)) {
		StartStore(STORE_NOROOM);
		return;
	}

	StoreItem = premiumitems[idx];
	StartStore(STORE_CONFIRM);
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

	int idx = stextvhold + ((stextlhold - stextup) / 4);
	if (storehidx[idx] >= 0)
		myPlayer.RemoveInvItem(storehidx[idx]);
	else
		myPlayer.RemoveSpdBarItem(-(storehidx[idx] + 1));

	int cost = storehold[idx]._iIvalue;
	storenumh--;
	if (idx != storenumh) {
		while (idx < storenumh) {
			storehold[idx] = storehold[idx + 1];
			storehidx[idx] = storehidx[idx + 1];
			idx++;
		}
	}

	AddGoldToInventory(myPlayer, cost);

	myPlayer._pGold += cost;
}

void SmithSellEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(STORE_SMITH);
		stextsel = 16;
		return;
	}

	stextlhold = stextsel;
	stextshold = STORE_SSELL;
	stextvhold = stextsval;

	int idx = stextsval + ((stextsel - stextup) / 4);

	if (!StoreGoldFit(storehold[idx])) {
		StartStore(STORE_NOROOM);
		return;
	}

	StoreItem = storehold[idx];
	StartStore(STORE_CONFIRM);
}

/**
 * @brief Repairs an item in the player's inventory or body in the smith.
 */
void SmithRepairItem(int price)
{
	TakePlrsMoney(price);

	int idx = stextvhold + ((stextlhold - stextup) / 4);
	storehold[idx]._iDurability = storehold[idx]._iMaxDur;

	int8_t i = storehidx[idx];

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
		return;
	}

	myPlayer.InvList[i]._iDurability = myPlayer.InvList[i]._iMaxDur;
}

void SmithRepairEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(STORE_SMITH);
		stextsel = 18;
		return;
	}

	stextshold = STORE_SREPAIR;
	stextlhold = stextsel;
	stextvhold = stextsval;

	int idx = stextsval + ((stextsel - stextup) / 4);

	if (!PlayerCanAfford(storehold[idx]._iIvalue)) {
		StartStore(STORE_NOMONEY);
		return;
	}

	StoreItem = storehold[idx];
	StartStore(STORE_CONFIRM);
}

void WitchEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = TOWN_WITCH;
		stextshold = STORE_WITCH;
		gossipstart = TEXT_ADRIA2;
		gossipend = TEXT_ADRIA13;
		StartStore(STORE_GOSSIP);
		break;
	case 14:
		StartStore(STORE_WBUY);
		break;
	case 16:
		StartStore(STORE_WSELL);
		break;
	case 18:
		StartStore(STORE_WRECHARGE);
		break;
	case 20:
		stextflag = STORE_NONE;
		break;
	}
}

/**
 * @brief Purchases an item from the witch.
 */
void WitchBuyItem(Item &item)
{
	int idx = stextvhold + ((stextlhold - stextup) / 4);

	if (idx < 3)
		item._iSeed = AdvanceRndSeed();

	TakePlrsMoney(item._iIvalue);
	StoreAutoPlace(item, true);

	if (idx >= 3) {
		if (idx == WITCH_ITEMS - 1) {
			witchitem[WITCH_ITEMS - 1].clear();
		} else {
			for (; !witchitem[idx + 1].isEmpty(); idx++) {
				witchitem[idx] = std::move(witchitem[idx + 1]);
			}
			witchitem[idx].clear();
		}
	}

	CalcPlrInv(*MyPlayer, true);
}

void WitchBuyEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(STORE_WITCH);
		stextsel = 14;
		return;
	}

	stextlhold = stextsel;
	stextvhold = stextsval;
	stextshold = STORE_WBUY;

	int idx = stextsval + ((stextsel - stextup) / 4);

	if (!PlayerCanAfford(witchitem[idx]._iIvalue)) {
		StartStore(STORE_NOMONEY);
		return;
	}

	if (!StoreAutoPlace(witchitem[idx], false)) {
		StartStore(STORE_NOROOM);
		return;
	}

	StoreItem = witchitem[idx];
	StartStore(STORE_CONFIRM);
}

void WitchSellEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(STORE_WITCH);
		stextsel = 16;
		return;
	}

	stextlhold = stextsel;
	stextshold = STORE_WSELL;
	stextvhold = stextsval;

	int idx = stextsval + ((stextsel - stextup) / 4);

	if (!StoreGoldFit(storehold[idx])) {
		StartStore(STORE_NOROOM);
		return;
	}

	StoreItem = storehold[idx];
	StartStore(STORE_CONFIRM);
}

/**
 * @brief Recharges an item in the player's inventory or body in the witch.
 */
void WitchRechargeItem(int price)
{
	TakePlrsMoney(price);

	int idx = stextvhold + ((stextlhold - stextup) / 4);
	storehold[idx]._iCharges = storehold[idx]._iMaxCharges;

	Player &myPlayer = *MyPlayer;

	int8_t i = storehidx[idx];
	if (i < 0)
		myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges = myPlayer.InvBody[INVLOC_HAND_LEFT]._iMaxCharges;
	else
		myPlayer.InvList[i]._iCharges = myPlayer.InvList[i]._iMaxCharges;

	CalcPlrInv(myPlayer, true);
}

void WitchRechargeEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(STORE_WITCH);
		stextsel = 18;
		return;
	}

	stextshold = STORE_WRECHARGE;
	stextlhold = stextsel;
	stextvhold = stextsval;

	int idx = stextsval + ((stextsel - stextup) / 4);

	if (!PlayerCanAfford(storehold[idx]._iIvalue)) {
		StartStore(STORE_NOMONEY);
		return;
	}

	StoreItem = storehold[idx];
	StartStore(STORE_CONFIRM);
}

void BoyEnter()
{
	if (!boyitem.isEmpty() && stextsel == 18) {
		if (!PlayerCanAfford(50)) {
			stextshold = STORE_BOY;
			stextlhold = 18;
			stextvhold = stextsval;
			StartStore(STORE_NOMONEY);
		} else {
			TakePlrsMoney(50);
			StartStore(STORE_BBOY);
		}
		return;
	}

	if ((stextsel != 8 && !boyitem.isEmpty()) || (stextsel != 12 && boyitem.isEmpty())) {
		stextflag = STORE_NONE;
		return;
	}

	talker = TOWN_PEGBOY;
	stextshold = STORE_BOY;
	stextlhold = stextsel;
	gossipstart = TEXT_WIRT2;
	gossipend = TEXT_WIRT12;
	StartStore(STORE_GOSSIP);
}

void BoyBuyItem(Item &item)
{
	TakePlrsMoney(item._iIvalue);
	StoreAutoPlace(item, true);
	boyitem.clear();
	stextshold = STORE_BOY;
	CalcPlrInv(*MyPlayer, true);
	stextlhold = 12;
}

/**
 * @brief Purchases an item from the healer.
 */
void HealerBuyItem(Item &item)
{
	int idx = stextvhold + ((stextlhold - stextup) / 4);
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
	idx = stextvhold + ((stextlhold - stextup) / 4);
	if (idx == 19) {
		healitem[19].clear();
	} else {
		for (; !healitem[idx + 1].isEmpty(); idx++) {
			healitem[idx] = std::move(healitem[idx + 1]);
		}
		healitem[idx].clear();
	}
	CalcPlrInv(*MyPlayer, true);
}

void BoyBuyEnter()
{
	if (stextsel != 10) {
		stextflag = STORE_NONE;
		return;
	}

	stextshold = STORE_BBOY;
	stextvhold = stextsval;
	stextlhold = 10;
	int price = boyitem._iIvalue;
	if (gbIsHellfire)
		price -= boyitem._iIvalue / 4;
	else
		price += boyitem._iIvalue / 2;

	if (!PlayerCanAfford(price)) {
		StartStore(STORE_NOMONEY);
		return;
	}

	if (!StoreAutoPlace(boyitem, false)) {
		StartStore(STORE_NOROOM);
		return;
	}

	StoreItem = boyitem;
	StoreItem._iIvalue = price;
	StartStore(STORE_CONFIRM);
}

void StorytellerIdentifyItem(Item &item)
{
	Player &myPlayer = *MyPlayer;

	int8_t idx = storehidx[((stextlhold - stextup) / 4) + stextvhold];
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
	if (stextsel == 18) {
		switch (stextshold) {
		case STORE_SBUY:
			SmithBuyItem(item);
			break;
		case STORE_SSELL:
		case STORE_WSELL:
			StoreSellItem();
			break;
		case STORE_SREPAIR:
			SmithRepairItem(item._iIvalue);
			break;
		case STORE_WBUY:
			WitchBuyItem(item);
			break;
		case STORE_WRECHARGE:
			WitchRechargeItem(item._iIvalue);
			break;
		case STORE_BBOY:
			BoyBuyItem(item);
			break;
		case STORE_HBUY:
			HealerBuyItem(item);
			break;
		case STORE_SIDENTIFY:
			StorytellerIdentifyItem(item);
			StartStore(STORE_IDSHOW);
			return;
		case STORE_SPBUY:
			SmithBuyPItem(item);
			break;
		default:
			break;
		}
	}

	StartStore(stextshold);

	if (stextsel == BackButtonLine())
		return;

	stextsel = stextlhold;
	stextsval = std::min(stextvhold, stextsmax);

	while (stextsel != -1 && !stext[stextsel].isSelectable()) {
		stextsel--;
	}
}

void HealerEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = TOWN_HEALER;
		stextshold = STORE_HEALER;
		gossipstart = TEXT_PEPIN2;
		gossipend = TEXT_PEPIN11;
		StartStore(STORE_GOSSIP);
		break;
	case 14:
		StartStore(STORE_HBUY);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void HealerBuyEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(STORE_HEALER);
		stextsel = 14;
		return;
	}

	stextlhold = stextsel;
	stextvhold = stextsval;
	stextshold = STORE_HBUY;

	int idx = stextsval + ((stextsel - stextup) / 4);

	if (!PlayerCanAfford(healitem[idx]._iIvalue)) {
		StartStore(STORE_NOMONEY);
		return;
	}

	if (!StoreAutoPlace(healitem[idx], false)) {
		StartStore(STORE_NOROOM);
		return;
	}

	StoreItem = healitem[idx];
	StartStore(STORE_CONFIRM);
}

void StorytellerEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = TOWN_STORY;
		stextshold = STORE_STORY;
		gossipstart = TEXT_STORY2;
		gossipend = TEXT_STORY11;
		StartStore(STORE_GOSSIP);
		break;
	case 14:
		StartStore(STORE_SIDENTIFY);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void StorytellerIdentifyEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(STORE_STORY);
		stextsel = 14;
		return;
	}

	stextshold = STORE_SIDENTIFY;
	stextlhold = stextsel;
	stextvhold = stextsval;

	int idx = stextsval + ((stextsel - stextup) / 4);

	if (!PlayerCanAfford(storehold[idx]._iIvalue)) {
		StartStore(STORE_NOMONEY);
		return;
	}

	StoreItem = storehold[idx];
	StartStore(STORE_CONFIRM);
}

void TalkEnter()
{
	if (stextsel == BackButtonLine()) {
		StartStore(stextshold);
		stextsel = stextlhold;
		return;
	}

	int sn = 0;
	for (auto &quest : Quests) {
		if (quest._qactive == QUEST_ACTIVE && QuestDialogTable[talker][quest._qidx] != TEXT_NONE && quest._qlog)
			sn++;
	}
	int la = 2;
	if (sn > 6) {
		sn = 14 - (sn / 2);
		la = 1;
	} else {
		sn = 15 - sn;
	}

	if (stextsel == sn - 2) {
		SetRndSeed(Towners[talker].seed);
		auto tq = static_cast<_speech_id>(gossipstart + GenerateRnd(gossipend - gossipstart + 1));
		InitQTextMsg(tq);
		return;
	}

	for (auto &quest : Quests) {
		if (quest._qactive == QUEST_ACTIVE && QuestDialogTable[talker][quest._qidx] != TEXT_NONE && quest._qlog) {
			if (sn == stextsel) {
				InitQTextMsg(QuestDialogTable[talker][quest._qidx]);
			}
			sn += la;
		}
	}
}

void TavernEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = TOWN_TAVERN;
		stextshold = STORE_TAVERN;
		gossipstart = TEXT_OGDEN2;
		gossipend = TEXT_OGDEN10;
		StartStore(STORE_GOSSIP);
		break;
	case 18:
		stextflag = STORE_NONE;
		break;
	}
}

void BarmaidEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = TOWN_BMAID;
		stextshold = STORE_BARMAID;
		gossipstart = TEXT_GILLIAN2;
		gossipend = TEXT_GILLIAN10;
		StartStore(STORE_GOSSIP);
		break;
	case 14:
		stextflag = STORE_NONE;
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
		stextflag = STORE_NONE;
		break;
	}
}

void DrunkEnter()
{
	switch (stextsel) {
	case 12:
		stextlhold = 12;
		talker = TOWN_DRUNK;
		stextshold = STORE_DRUNK;
		gossipstart = TEXT_FARNHAM2;
		gossipend = TEXT_FARNHAM13;
		StartStore(STORE_GOSSIP);
		break;
	case 18:
		stextflag = STORE_NONE;
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

void DrawSelector(const Surface &out, const Rectangle &rect, string_view text, UiFlags flags)
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

	item = &storehold[storenumh];
	storehold[storenumh] = *itm;

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
	storehidx[storenumh] = i;
	storenumh++;
}

void InitStores()
{
	ClearSText(0, STORE_LINES);
	stextflag = STORE_NONE;
	stextsize = false;
	stextscrl = false;
	numpremium = 0;
	premiumlevel = 1;

	for (auto &premiumitem : premiumitems)
		premiumitem.clear();

	boyitem.clear();
	boylevel = 0;
}

void SetupTownStores()
{
	Player &myPlayer = *MyPlayer;

	int l = myPlayer._pLevel / 2;
	if (!gbIsMultiplayer) {
		l = 0;
		for (int i = 0; i < NUMLEVELS; i++) {
			if (myPlayer._pLvlVisited[i])
				l = i;
		}
	} else {
		SetRndSeed(glSeedTbl[currlevel] * SDL_GetTicks());
	}

	l = clamp(l + 2, 6, 16);
	SpawnSmith(l);
	SpawnWitch(l);
	SpawnHealer(l);
	SpawnBoy(myPlayer._pLevel);
	SpawnPremium(myPlayer);
}

void FreeStoreMem()
{
	stextflag = STORE_NONE;
	for (STextStruct &entry : stext) {
		entry.text.clear();
		entry.text.shrink_to_fit();
	}
}

void PrintSString(const Surface &out, int margin, int line, string_view text, UiFlags flags, int price)
{
	const Point uiPosition = GetUIRectangle().position;
	int sx = uiPosition.x + 32 + margin;
	if (!stextsize) {
		sx += 320;
	}

	const int sy = uiPosition.y + PaddingTop + stext[line].y + stext[line]._syoff;

	int width = stextsize ? 575 : 255;
	if (stextscrl && line >= 4 && line <= 20) {
		width -= 9; // Space for the selector
	}
	width -= margin * 2;

	const Rectangle rect { { sx, sy }, { width, 0 } };
	DrawString(out, text, rect, flags);
	if (price > 0)
		DrawString(out, FormatInteger(price), rect, flags | UiFlags::AlignRight);

	if (stextsel == line) {
		DrawSelector(out, rect, text, flags);
	}
}

void DrawSLine(const Surface &out, int sy)
{
	const Point uiPosition = GetUIRectangle().position;
	int sx = 26;
	int width = 587;

	if (!stextsize) {
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
	stextsel = -1;
	stextsize = true;
}

void ClearSText(int s, int e)
{
	for (int i = s; i < e; i++) {
		stext[i]._sx = 0;
		stext[i]._syoff = 0;
		stext[i].text.clear();
		stext[i].text.shrink_to_fit();
		stext[i].flags = UiFlags::None;
		stext[i].type = STextStruct::Label;
		stext[i]._sval = 0;
	}
}

void StartStore(talk_id s)
{
	sbookflag = false;
	CloseInventory();
	chrflag = false;
	RenderGold = false;
	QuestLogIsOpen = false;
	CloseGoldDrop();
	ClearSText(0, STORE_LINES);
	ReleaseStoreBtn();
	switch (s) {
	case STORE_SMITH:
		StartSmith();
		break;
	case STORE_SBUY: {
		bool hasAnyItems = false;
		for (int i = 0; !smithitem[i].isEmpty(); i++) {
			hasAnyItems = true;
			break;
		}
		if (hasAnyItems)
			StartSmithBuy();
		else {
			stextflag = STORE_SBUY;
			stextlhold = 12;
			StoreESC();
			return;
		}
		break;
	}
	case STORE_SSELL:
		StartSmithSell();
		break;
	case STORE_SREPAIR:
		StartSmithRepair();
		break;
	case STORE_WITCH:
		StartWitch();
		break;
	case STORE_WBUY:
		if (storenumh > 0)
			StartWitchBuy();
		break;
	case STORE_WSELL:
		StartWitchSell();
		break;
	case STORE_WRECHARGE:
		StartWitchRecharge();
		break;
	case STORE_NOMONEY:
		StoreNoMoney();
		break;
	case STORE_NOROOM:
		StoreNoRoom();
		break;
	case STORE_CONFIRM:
		StoreConfirm(StoreItem);
		break;
	case STORE_BOY:
		StartBoy();
		break;
	case STORE_BBOY:
		SStartBoyBuy();
		break;
	case STORE_HEALER:
		StartHealer();
		break;
	case STORE_STORY:
		StartStoryteller();
		break;
	case STORE_HBUY:
		if (storenumh > 0)
			StartHealerBuy();
		break;
	case STORE_SIDENTIFY:
		StartStorytellerIdentify();
		break;
	case STORE_SPBUY:
		if (!StartSmithPremiumBuy())
			return;
		break;
	case STORE_GOSSIP:
		StartTalk();
		break;
	case STORE_IDSHOW:
		StartStorytellerIdentifyShow(StoreItem);
		break;
	case STORE_TAVERN:
		StartTavern();
		break;
	case STORE_DRUNK:
		StartDrunk();
		break;
	case STORE_BARMAID:
		StartBarmaid();
		break;
	case STORE_NONE:
		break;
	}

	stextsel = -1;
	for (int i = 0; i < STORE_LINES; i++) {
		if (stext[i].isSelectable()) {
			stextsel = i;
			break;
		}
	}

	stextflag = s;
}

void DrawSText(const Surface &out)
{
	if (!stextsize)
		DrawSTextBack(out);
	else
		DrawQTextBack(out);

	if (stextscrl) {
		switch (stextflag) {
		case STORE_SBUY:
			ScrollSmithBuy(stextsval);
			break;
		case STORE_SSELL:
		case STORE_SREPAIR:
		case STORE_WSELL:
		case STORE_WRECHARGE:
		case STORE_SIDENTIFY:
			ScrollSmithSell(stextsval);
			break;
		case STORE_WBUY:
			ScrollWitchBuy(stextsval);
			break;
		case STORE_HBUY:
			ScrollHealerBuy(stextsval);
			break;
		case STORE_SPBUY:
			ScrollSmithPremiumBuy(stextsval);
			break;
		default:
			break;
		}
	}

	CalculateLineHeights();
	const Point uiPosition = GetUIRectangle().position;
	for (int i = 0; i < STORE_LINES; i++) {
		if (stext[i].isDivider())
			DrawSLine(out, uiPosition.y + PaddingTop + stext[i].y + TextHeight() / 2);
		else if (stext[i].hasText())
			PrintSString(out, stext[i]._sx, i, stext[i].text, stext[i].flags, stext[i]._sval);
	}

	if (RenderGold) {
		PrintSString(out, 28, 1, fmt::format(fmt::runtime(_("Your gold: {:s}")), FormatInteger(TotalPlayerGold())).c_str(), UiFlags::ColorWhitegold | UiFlags::AlignRight);
	}

	if (stextscrl)
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

	switch (stextflag) {
	case STORE_SMITH:
	case STORE_WITCH:
	case STORE_BOY:
	case STORE_BBOY:
	case STORE_HEALER:
	case STORE_STORY:
	case STORE_TAVERN:
	case STORE_DRUNK:
	case STORE_BARMAID:
		stextflag = STORE_NONE;
		break;
	case STORE_GOSSIP:
		StartStore(stextshold);
		stextsel = stextlhold;
		break;
	case STORE_SBUY:
		StartStore(STORE_SMITH);
		stextsel = 12;
		break;
	case STORE_SPBUY:
		StartStore(STORE_SMITH);
		stextsel = 14;
		break;
	case STORE_SSELL:
		StartStore(STORE_SMITH);
		stextsel = 16;
		break;
	case STORE_SREPAIR:
		StartStore(STORE_SMITH);
		stextsel = 18;
		break;
	case STORE_WBUY:
		StartStore(STORE_WITCH);
		stextsel = 14;
		break;
	case STORE_WSELL:
		StartStore(STORE_WITCH);
		stextsel = 16;
		break;
	case STORE_WRECHARGE:
		StartStore(STORE_WITCH);
		stextsel = 18;
		break;
	case STORE_HBUY:
		StartStore(STORE_HEALER);
		stextsel = 14;
		break;
	case STORE_SIDENTIFY:
		StartStore(STORE_STORY);
		stextsel = 14;
		break;
	case STORE_IDSHOW:
		StartStore(STORE_SIDENTIFY);
		break;
	case STORE_NOMONEY:
	case STORE_NOROOM:
	case STORE_CONFIRM:
		StartStore(stextshold);
		stextsel = stextlhold;
		stextsval = stextvhold;
		break;
	case STORE_NONE:
		break;
	}
}

void StoreUp()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel == -1) {
		return;
	}

	if (stextscrl) {
		if (stextsel == stextup) {
			if (stextsval != 0)
				stextsval--;
			return;
		}

		stextsel--;
		while (!stext[stextsel].isSelectable()) {
			if (stextsel == 0)
				stextsel = STORE_LINES - 1;
			else
				stextsel--;
		}
		return;
	}

	if (stextsel == 0)
		stextsel = STORE_LINES - 1;
	else
		stextsel--;

	while (!stext[stextsel].isSelectable()) {
		if (stextsel == 0)
			stextsel = STORE_LINES - 1;
		else
			stextsel--;
	}
}

void StoreDown()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel == -1) {
		return;
	}

	if (stextscrl) {
		if (stextsel == stextdown) {
			if (stextsval < stextsmax)
				stextsval++;
			return;
		}

		stextsel++;
		while (!stext[stextsel].isSelectable()) {
			if (stextsel == STORE_LINES - 1)
				stextsel = 0;
			else
				stextsel++;
		}
		return;
	}

	if (stextsel == STORE_LINES - 1)
		stextsel = 0;
	else
		stextsel++;

	while (!stext[stextsel].isSelectable()) {
		if (stextsel == STORE_LINES - 1)
			stextsel = 0;
		else
			stextsel++;
	}
}

void StorePrior()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1 && stextscrl) {
		if (stextsel == stextup) {
			stextsval = std::max(stextsval - 4, 0);
		} else {
			stextsel = stextup;
		}
	}
}

void StoreNext()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1 && stextscrl) {
		if (stextsel == stextdown) {
			if (stextsval < stextsmax)
				stextsval += 4;
			if (stextsval > stextsmax)
				stextsval = stextsmax;
		} else {
			stextsel = stextdown;
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

	PlaySFX(IS_TITLSLCT);
	switch (stextflag) {
	case STORE_SMITH:
		SmithEnter();
		break;
	case STORE_SPBUY:
		SmithPremiumBuyEnter();
		break;
	case STORE_SBUY:
		SmithBuyEnter();
		break;
	case STORE_SSELL:
		SmithSellEnter();
		break;
	case STORE_SREPAIR:
		SmithRepairEnter();
		break;
	case STORE_WITCH:
		WitchEnter();
		break;
	case STORE_WBUY:
		WitchBuyEnter();
		break;
	case STORE_WSELL:
		WitchSellEnter();
		break;
	case STORE_WRECHARGE:
		WitchRechargeEnter();
		break;
	case STORE_NOMONEY:
	case STORE_NOROOM:
		StartStore(stextshold);
		stextsel = stextlhold;
		stextsval = stextvhold;
		break;
	case STORE_CONFIRM:
		ConfirmEnter(StoreItem);
		break;
	case STORE_BOY:
		BoyEnter();
		break;
	case STORE_BBOY:
		BoyBuyEnter();
		break;
	case STORE_HEALER:
		HealerEnter();
		break;
	case STORE_STORY:
		StorytellerEnter();
		break;
	case STORE_HBUY:
		HealerBuyEnter();
		break;
	case STORE_SIDENTIFY:
		StorytellerIdentifyEnter();
		break;
	case STORE_GOSSIP:
		TalkEnter();
		break;
	case STORE_IDSHOW:
		StartStore(STORE_SIDENTIFY);
		break;
	case STORE_DRUNK:
		DrunkEnter();
		break;
	case STORE_TAVERN:
		TavernEnter();
		break;
	case STORE_BARMAID:
		BarmaidEnter();
		break;
	case STORE_NONE:
		break;
	}
}

void CheckStoreBtn()
{
	const Point uiPosition = GetUIRectangle().position;
	if (qtextflag) {
		qtextflag = false;
		if (leveltype == DTYPE_TOWN)
			stream_stop();
	} else if (stextsel != -1 && MousePosition.y >= (PaddingTop + uiPosition.y) && MousePosition.y <= (320 + uiPosition.y)) {
		if (!stextsize) {
			if (MousePosition.x < 344 + uiPosition.x || MousePosition.x > 616 + uiPosition.x)
				return;
		} else {
			if (MousePosition.x < 24 + uiPosition.x || MousePosition.x > 616 + uiPosition.x)
				return;
		}

		const int relativeY = MousePosition.y - (uiPosition.y + PaddingTop);

		if (stextscrl && MousePosition.x > 600 + uiPosition.x) {
			// Scroll bar is always measured in terms of the small line height.
			int y = relativeY / SmallLineHeight;
			if (y == 4) {
				if (stextscrlubtn <= 0) {
					StoreUp();
					stextscrlubtn = 10;
				} else {
					stextscrlubtn--;
				}
			}
			if (y == 20) {
				if (stextscrldbtn <= 0) {
					StoreDown();
					stextscrldbtn = 10;
				} else {
					stextscrldbtn--;
				}
			}
			return;
		}

		int y = relativeY / LineHeight();

		// Large small fonts draw beyond LineHeight. Check if the click was on the overflow text.
		if (IsSmallFontTall() && y > 0 && y < STORE_LINES
		    && stext[y - 1].hasText() && !stext[y].hasText()
		    && relativeY < stext[y - 1].y + LargeTextHeight) {
			--y;
		}

		if (y >= 5) {
			if (y >= BackButtonLine() + 1)
				y = BackButtonLine();
			if (stextscrl && y <= 20 && !stext[y].isSelectable()) {
				if (stext[y - 2].isSelectable()) {
					y -= 2;
				} else if (stext[y - 1].isSelectable()) {
					y--;
				}
			}
			if (stext[y].isSelectable() || (stextscrl && y == BackButtonLine())) {
				stextsel = y;
				StoreEnter();
			}
		}
	}
}

void ReleaseStoreBtn()
{
	stextscrlubtn = -1;
	stextscrldbtn = -1;
}

} // namespace devilution
