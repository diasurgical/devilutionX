/**
 * @file stores.cpp
 *
 * Implementation of functionality for stores and towner dialogs.
 */
#include "stores.h"

#include <algorithm>

#include <fmt/format.h>

#include "cursor.h"
#include "engine/load_cel.hpp"
#include "engine/random.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "init.h"
#include "minitext.h"
#include "options.h"
#include "towners.h"
#include "utils/language.h"

namespace devilution {

ItemStruct golditem;

std::optional<CelSprite> pSTextBoxCels;
std::optional<CelSprite> pSTextSlidCels;

talk_id stextflag;

int storenumh;
char storehidx[48];
ItemStruct storehold[48];

ItemStruct smithitem[SMITH_ITEMS];
int numpremium;
int premiumlevel;
ItemStruct premiumitems[SMITH_PREMIUM_ITEMS];

ItemStruct healitem[20];

ItemStruct witchitem[WITCH_ITEMS];

int boylevel;
ItemStruct boyitem;

namespace {

/** The current towner being interacted with */
_talker_id talker;

/** Is the curren dialog full size */
bool stextsize;

/** Number of text lines in the current dialog */
int stextsmax;
/** Remember currently selected text line from stext while displaying a dialog */
int stextlhold;
/** Currently selected text line from stext */
int stextsel;
/** Text lines */
STextStruct stext[STORE_LINES];

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

/** Maps from towner IDs to NPC names. */
const char *const TownerNames[] = {
	"Griswold",
	"Pepin",
	"",
	"Ogden",
	"Cain",
	"Farnham",
	"Adria",
	"Gillian",
	"Wirt"
};

void DrawSTextBack(const Surface &out)
{
	CelDrawTo(out, { PANEL_X + 320 + 24, 327 + UI_OFFSET_Y }, *pSTextBoxCels, 1);
	DrawHalfTransparentRectTo(out, PANEL_X + 347, UI_OFFSET_Y + 28, 265, 297);
}

void DrawSSlider(const Surface &out, int y1, int y2)
{
	int yd1 = y1 * 12 + 44 + UI_OFFSET_Y;
	int yd2 = y2 * 12 + 44 + UI_OFFSET_Y;
	if (stextscrlubtn != -1)
		CelDrawTo(out, { PANEL_X + 601, yd1 }, *pSTextSlidCels, 12);
	else
		CelDrawTo(out, { PANEL_X + 601, yd1 }, *pSTextSlidCels, 10);
	if (stextscrldbtn != -1)
		CelDrawTo(out, { PANEL_X + 601, yd2 }, *pSTextSlidCels, 11);
	else
		CelDrawTo(out, { PANEL_X + 601, yd2 }, *pSTextSlidCels, 9);
	yd1 += 12;
	int yd3 = yd1;
	for (; yd3 < yd2; yd3 += 12) {
		CelDrawTo(out, { PANEL_X + 601, yd3 }, *pSTextSlidCels, 14);
	}
	if (stextsel == 22)
		yd3 = stextlhold;
	else
		yd3 = stextsel;
	if (storenumh > 1)
		yd3 = 1000 * (stextsval + ((yd3 - stextup) / 4)) / (storenumh - 1) * (y2 * 12 - y1 * 12 - 24) / 1000;
	else
		yd3 = 0;
	CelDrawTo(out, { PANEL_X + 601, (y1 + 1) * 12 + 44 + UI_OFFSET_Y + yd3 }, *pSTextSlidCels, 13);
}

void AddSLine(int y)
{
	stext[y]._sx = 0;
	stext[y]._syoff = 0;
	stext[y]._sstr[0] = 0;
	stext[y]._sline = 1;
}

void AddSTextVal(int y, int val)
{
	stext[y]._sval = val;
}

void OffsetSTextY(int y, int yo)
{
	stext[y]._syoff = yo;
}

void AddSText(int x, int y, const char *str, UiFlags flags, bool sel)
{
	stext[y]._sx = x;
	stext[y]._syoff = 0;
	strcpy(stext[y]._sstr, str);
	stext[y].flags = flags;
	stext[y]._sline = 0;
	stext[y]._ssel = sel;
}

void PrintStoreItem(ItemStruct *x, int l, UiFlags flags)
{
	char sstr[128];

	sstr[0] = '\0';
	if (x->_iIdentified) {
		if (x->_iMagical != ITEM_QUALITY_UNIQUE) {
			if (x->_iPrePower != -1) {
				PrintItemPower(x->_iPrePower, x);
				strcat(sstr, tempstr);
			}
		}
		if (x->_iSufPower != -1) {
			PrintItemPower(x->_iSufPower, x);
			if (sstr[0] != '\0')
				strcat(sstr, _(",  "));
			strcat(sstr, tempstr);
		}
	}
	if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges != 0) {
		strcpy(tempstr, fmt::format(_("Charges: {:d}/{:d}"), x->_iCharges, x->_iMaxCharges).c_str());
		if (sstr[0] != '\0')
			strcat(sstr, _(",  "));
		strcat(sstr, tempstr);
	}
	if (sstr[0] != '\0') {
		AddSText(40, l, sstr, flags, false);
		l++;
	}
	sstr[0] = '\0';
	if (x->_iClass == ICLASS_WEAPON)
		strcpy(sstr, fmt::format(_("Damage: {:d}-{:d}  "), x->_iMinDam, x->_iMaxDam).c_str());
	if (x->_iClass == ICLASS_ARMOR)
		strcpy(sstr, fmt::format(_("Armor: {:d}  "), x->_iAC).c_str());
	if (x->_iMaxDur != DUR_INDESTRUCTIBLE && x->_iMaxDur != 0) {
		strcpy(tempstr, fmt::format(_("Dur: {:d}/{:d},  "), x->_iDurability, x->_iMaxDur).c_str());
		strcat(sstr, tempstr);
	} else {
		strcat(sstr, _("Indestructible,  "));
	}
	if (x->_itype == ITYPE_MISC)
		sstr[0] = '\0';
	int8_t str = x->_iMinStr;
	uint8_t mag = x->_iMinMag;
	int8_t dex = x->_iMinDex;
	if (str == 0 && mag == 0 && dex == 0) {
		strcat(sstr, _("No required attributes"));
	} else {
		strcpy(tempstr, _("Required:"));
		if (str != 0)
			strcpy(tempstr + strlen(tempstr), fmt::format(_(" {:d} Str"), str).c_str());
		if (mag != 0)
			strcpy(tempstr + strlen(tempstr), fmt::format(_(" {:d} Mag"), mag).c_str());
		if (dex != 0)
			strcpy(tempstr + strlen(tempstr), fmt::format(_(" {:d} Dex"), dex).c_str());
		strcat(sstr, tempstr);
	}
	AddSText(40, l++, sstr, flags, false);
}

void StoreAutoPlace()
{
	auto &myPlayer = Players[MyPlayerId];

	if (AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(MyPlayerId, myPlayer.HoldItem)) {
		return;
	}

	if (AutoPlaceItemInBelt(myPlayer, myPlayer.HoldItem, true)) {
		return;
	}

	AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, true);
}

void StartSmith()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, _("Welcome to the"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 3, _("Blacksmith's shop"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 7, _("Would you like to:"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 10, _("Talk to Griswold"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 12, _("Buy basic items"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Buy premium items"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 16, _("Sell items"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Repair items"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 20, _("Leave the shop"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
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
			PrintStoreItem(&smithitem[idx], l + 1, itemColor);
			stextdown = l;
			idx++;
		}
	}

	if (stextsel != -1 && !stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}

void StartSmithBuy()
{
	stextsize = true;
	stextscrl = true;
	stextsval = 0;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("I have these items for sale:             Your gold: {:d}"), Players[MyPlayerId]._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	ScrollSmithBuy(stextsval);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, false);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (int i = 0; !smithitem[i].isEmpty(); i++) {
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
			PrintStoreItem(&premiumitems[idx], l + 1, itemColor);
			stextdown = l;
		} else {
			l -= 4;
		}
		idx++;
	}
	if (stextsel != -1 && !stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}

bool StartSmithPremiumBuy()
{
	storenumh = 0;
	for (const auto &item : premiumitems) {
		if (!item.isEmpty())
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

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("I have these premium items for sale:     Your gold: {:d}"), Players[MyPlayerId]._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, false);
	OffsetSTextY(22, 6);

	stextsmax = std::max(storenumh - 4, 0);

	ScrollSmithPremiumBuy(stextsval);

	return true;
}

bool SmithSellOk(int i)
{
	ItemStruct *pI;

	if (i >= 0) {
		pI = &Players[MyPlayerId].InvList[i];
	} else {
		pI = &Players[MyPlayerId].SpdList[-(i + 1)];
	}

	if (pI->isEmpty())
		return false;

	if (pI->_iMiscId > IMISC_OILFIRST && pI->_iMiscId < IMISC_OILLAST)
		return true;

	if (pI->_itype == ITYPE_MISC)
		return false;
	if (pI->_itype == ITYPE_GOLD)
		return false;
	if (pI->_itype == ITYPE_STAFF && (!gbIsHellfire || pI->_iSpell != SPL_NULL))
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

			PrintStoreItem(&storehold[idx], l + 1, itemColor);
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
		item._itype = ITYPE_NONE;
	}

	const auto &myPlayer = Players[MyPlayerId];

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

	for (int i = 0; i < MAXBELTITEMS; i++) {
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

		/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
		strcpy(tempstr, fmt::format(_("You have nothing I want.             Your gold: {:d}"), myPlayer._pGold).c_str());

		AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Which item is for sale?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	ScrollSmithSell(stextsval);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	OffsetSTextY(22, 6);
}

bool SmithRepairOk(int i)
{
	const auto &myPlayer = Players[MyPlayerId];

	if (myPlayer.InvList[i].isEmpty())
		return false;
	if (myPlayer.InvList[i]._itype == ITYPE_MISC)
		return false;
	if (myPlayer.InvList[i]._itype == ITYPE_GOLD)
		return false;
	if (myPlayer.InvList[i]._iDurability == myPlayer.InvList[i]._iMaxDur)
		return false;

	return true;
}

void StartSmithRepair()
{
	stextsize = true;
	bool repairok = false;
	storenumh = 0;

	for (auto &item : storehold) {
		item._itype = ITYPE_NONE;
	}

	auto &myPlayer = Players[MyPlayerId];

	auto &helmet = myPlayer.InvBody[INVLOC_HEAD];
	if (!helmet.isEmpty() && helmet._iDurability != helmet._iMaxDur) {
		repairok = true;
		AddStoreHoldRepair(&helmet, -1);
	}

	auto &armor = myPlayer.InvBody[INVLOC_CHEST];
	if (!armor.isEmpty() && armor._iDurability != armor._iMaxDur) {
		repairok = true;
		AddStoreHoldRepair(&armor, -2);
	}

	auto &leftHand = myPlayer.InvBody[INVLOC_HAND_LEFT];
	if (!leftHand.isEmpty() && leftHand._iDurability != leftHand._iMaxDur) {
		repairok = true;
		AddStoreHoldRepair(&leftHand, -3);
	}

	auto &rightHand = myPlayer.InvBody[INVLOC_HAND_RIGHT];
	if (!rightHand.isEmpty() && rightHand._iDurability != rightHand._iMaxDur) {
		repairok = true;
		AddStoreHoldRepair(&rightHand, -4);
	}

	for (int i = 0; i < myPlayer._pNumInv; i++) {
		if (storenumh >= 48)
			break;
		if (SmithRepairOk(i)) {
			repairok = true;
			AddStoreHoldRepair(&myPlayer.InvList[i], i);
		}
	}

	if (!repairok) {
		stextscrl = false;

		/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
		strcpy(tempstr, fmt::format(_("You have nothing to repair.             Your gold: {:d}"), myPlayer._pGold).c_str());

		AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Repair which item?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	ScrollSmithSell(stextsval);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	OffsetSTextY(22, 6);
}

void FillManaPlayer()
{
	if (!sgOptions.Gameplay.bAdriaRefillsMana)
		return;

	auto &myPlayer = Players[MyPlayerId];

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
	AddSText(0, 2, _("Witch's shack"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Adria"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Buy items"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 16, _("Sell items"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Recharge staves"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 20, _("Leave the shack"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
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
			PrintStoreItem(&witchitem[idx], l + 1, itemColor);
			stextdown = l;
			idx++;
		}
	}

	if (stextsel != -1 && !stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}

void StartWitchBuy()
{
	stextsize = true;
	stextscrl = true;
	stextsval = 0;
	stextsmax = 20;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("I have these items for sale:             Your gold: {:d}"), Players[MyPlayerId]._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	ScrollWitchBuy(stextsval);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, false);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (int i = 0; !witchitem[i].isEmpty(); i++) {
		storenumh++;
	}
	stextsmax = std::max(storenumh - 4, 0);
}

bool WitchSellOk(int i)
{
	ItemStruct *pI;

	bool rv = false;

	if (i >= 0)
		pI = &Players[MyPlayerId].InvList[i];
	else
		pI = &Players[MyPlayerId].SpdList[-(i + 1)];

	if (pI->_itype == ITYPE_MISC)
		rv = true;
	if (pI->_iMiscId > 29 && pI->_iMiscId < 41)
		rv = false;
	if (pI->_iClass == ICLASS_QUEST)
		rv = false;
	if (pI->_itype == ITYPE_STAFF && (!gbIsHellfire || pI->_iSpell != SPL_NULL))
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
		item._itype = ITYPE_NONE;
	}

	const auto &myPlayer = Players[MyPlayerId];

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

	for (int i = 0; i < MAXBELTITEMS; i++) {
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

		/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
		strcpy(tempstr, fmt::format(_("You have nothing I want.             Your gold: {:d}"), myPlayer._pGold).c_str());

		AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Which item is for sale?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	ScrollSmithSell(stextsval);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	OffsetSTextY(22, 6);
}

bool WitchRechargeOk(int i)
{
	const auto &item = Players[MyPlayerId].InvList[i];

	if (item._itype == ITYPE_STAFF && item._iCharges != item._iMaxCharges) {
		return true;
	}

	if ((item._iMiscId == IMISC_UNIQUE || item._iMiscId == IMISC_STAFF) && item._iCharges < item._iMaxCharges) {
		return true;
	}

	return false;
}

void AddStoreHoldRecharge(ItemStruct itm, int8_t i)
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
		item._itype = ITYPE_NONE;
	}

	const auto &myPlayer = Players[MyPlayerId];
	const auto &leftHand = myPlayer.InvBody[INVLOC_HAND_LEFT];

	if ((leftHand._itype == ITYPE_STAFF || leftHand._iMiscId == IMISC_UNIQUE) && leftHand._iCharges != leftHand._iMaxCharges) {
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

		/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
		strcpy(tempstr, fmt::format(_("You have nothing to recharge.             Your gold: {:d}"), myPlayer._pGold).c_str());

		AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Recharge which item?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	ScrollSmithSell(stextsval);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	OffsetSTextY(22, 6);
}

void StoreNoMoney()
{
	StartStore(stextshold);
	stextscrl = false;
	stextsize = true;
	ClearSText(5, 23);
	AddSText(0, 14, _("You do not have enough gold"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
}

void StoreNoRoom()
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);
	AddSText(0, 14, _("You do not have enough room in inventory"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
}

void StoreConfirm()
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);

	auto &item = Players[MyPlayerId].HoldItem;

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
	PrintStoreItem(&item, 9, itemColor);

	switch (stextshold) {
	case STORE_BBOY:
		strcpy(tempstr, _("Do we have a deal?"));
		break;
	case STORE_SIDENTIFY:
		strcpy(tempstr, _("Are you sure you want to identify this item?"));
		break;
	case STORE_HBUY:
	case STORE_SPBUY:
	case STORE_WBUY:
	case STORE_SBUY:
		strcpy(tempstr, _("Are you sure you want to buy this item?"));
		break;
	case STORE_WRECHARGE:
		strcpy(tempstr, _("Are you sure you want to recharge this item?"));
		break;
	case STORE_SSELL:
	case STORE_WSELL:
		strcpy(tempstr, _("Are you sure you want to sell this item?"));
		break;
	case STORE_SREPAIR:
		strcpy(tempstr, _("Are you sure you want to repair this item?"));
		break;
	default:
		app_fatal("Unknown store dialog %i", stextshold);
	}
	AddSText(0, 15, tempstr, UiFlags::ColorSilver | UiFlags::AlignCenter, false);
	AddSText(0, 18, _("Yes"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 20, _("No"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
}

void StartBoy()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("Wirt the Peg-legged boy"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(5);
	if (!boyitem.isEmpty()) {
		AddSText(0, 8, _("Talk to Wirt"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
		AddSText(0, 12, _("I have something for sale,"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
		AddSText(0, 14, _("but it will cost 50 gold"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
		AddSText(0, 16, _("just to take a look. "), UiFlags::ColorGold | UiFlags::AlignCenter, false);
		AddSText(0, 18, _("What have you got?"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
		AddSText(0, 20, _("Say goodbye"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	} else {
		AddSText(0, 12, _("Talk to Wirt"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
		AddSText(0, 18, _("Say goodbye"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	}
}

void SStartBoyBuy()
{
	stextsize = true;
	stextscrl = false;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("I have this item for sale:             Your gold: {:d}"), Players[MyPlayerId]._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	UiFlags itemColor = boyitem.getTextColorWithStatCheck();

	if (boyitem._iMagical != ITEM_QUALITY_NORMAL)
		AddSText(20, 10, boyitem._iIName, itemColor, true);
	else
		AddSText(20, 10, boyitem._iName, itemColor, true);

	if (gbIsHellfire)
		AddSTextVal(10, boyitem._iIvalue - (boyitem._iIvalue / 4));
	else
		AddSTextVal(10, boyitem._iIvalue + (boyitem._iIvalue / 2));
	PrintStoreItem(&boyitem, 11, itemColor);
	AddSText(0, 22, _("Leave"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	OffsetSTextY(22, 6);
}

void HealPlayer()
{
	auto &myPlayer = Players[MyPlayerId];

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
	AddSText(0, 1, _("Welcome to the"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 3, _("Healer's home"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Pepin"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Buy items"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 16, _("Leave Healer's home"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
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
			PrintStoreItem(&healitem[idx], l + 1, itemColor);
			stextdown = l;
			idx++;
		}
	}

	if (stextsel != -1 && !stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}

void StartHealerBuy()
{
	stextsize = true;
	stextscrl = true;
	stextsval = 0;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("I have these items for sale:             Your gold: {:d}"), Players[MyPlayerId]._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	ScrollHealerBuy(stextsval);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, false);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (int i = 0; !healitem[i].isEmpty(); i++) {
		storenumh++;
	}

	stextsmax = std::max(storenumh - 4, 0);
}

void StartStoryteller()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("The Town Elder"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Cain"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 14, _("Identify an item"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Say goodbye"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSLine(5);
}

bool IdItemOk(ItemStruct *i)
{
	if (i->isEmpty()) {
		return false;
	}
	if (i->_iMagical == ITEM_QUALITY_NORMAL) {
		return false;
	}
	return !i->_iIdentified;
}

void AddStoreHoldId(ItemStruct itm, int8_t i)
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
		item._itype = ITYPE_NONE;
	}

	auto &myPlayer = Players[MyPlayerId];

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

		/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
		strcpy(tempstr, fmt::format(_("You have nothing to identify.             Your gold: {:d}"), myPlayer._pGold).c_str());

		AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Identify which item?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(3);
	AddSLine(21);
	ScrollSmithSell(stextsval);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	OffsetSTextY(22, 6);
}

void StartStorytellerIdentifyShow()
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);

	auto &item = Players[MyPlayerId].HoldItem;

	UiFlags itemColor = item.getTextColorWithStatCheck();

	AddSText(0, 7, _("This item is:"), UiFlags::ColorSilver | UiFlags::AlignCenter, false);
	AddSText(20, 11, item._iIName, itemColor, false);
	PrintStoreItem(&item, 12, itemColor);
	AddSText(0, 18, _("Done"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
}

void StartTalk()
{
	int la;

	stextsize = false;
	stextscrl = false;
	strcpy(tempstr, fmt::format(_("Talk to {:s}"), TownerNames[talker]).c_str());
	AddSText(0, 2, tempstr, UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSLine(5);
	if (gbIsSpawn) {
		strcpy(tempstr, fmt::format(_("Talking to {:s}"), TownerNames[talker]).c_str());
		AddSText(0, 10, tempstr, UiFlags::ColorSilver | UiFlags::AlignCenter, false);
		AddSText(0, 12, _("is not available"), UiFlags::ColorSilver | UiFlags::AlignCenter, false);
		AddSText(0, 14, _("in the shareware"), UiFlags::ColorSilver | UiFlags::AlignCenter, false);
		AddSText(0, 16, _("version"), UiFlags::ColorSilver | UiFlags::AlignCenter, false);
		AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
		return;
	}

	int sn = 0;
	for (int i = 0; i < MAXQUESTS; i++) {
		if (Quests[i]._qactive == QUEST_ACTIVE && QuestDialogTable[talker][i] != TEXT_NONE && Quests[i]._qlog)
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

	for (int i = 0; i < MAXQUESTS; i++) {
		if (Quests[i]._qactive == QUEST_ACTIVE && QuestDialogTable[talker][i] != TEXT_NONE && Quests[i]._qlog) {
			AddSText(0, sn, _(QuestData[i]._qlstr), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
			sn += la;
		}
	}
	AddSText(0, sn2, _("Gossip"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 22, _("Back"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
}

void StartTavern()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, _("Welcome to the"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 3, _("Rising Sun"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Ogden"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Leave the tavern"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSLine(5);
	storenumh = 20;
}

void StartBarmaid()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, "Gillian", UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Gillian"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Say goodbye"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
	AddSLine(5);
	storenumh = 20;
}

void StartDrunk()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("Farnham the Drunk"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 9, _("Would you like to:"), UiFlags::ColorGold | UiFlags::AlignCenter, false);
	AddSText(0, 12, _("Talk to Farnham"), UiFlags::ColorBlue | UiFlags::AlignCenter, true);
	AddSText(0, 18, _("Say Goodbye"), UiFlags::ColorSilver | UiFlags::AlignCenter, true);
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
void SmithBuyItem()
{
	auto &item = Players[MyPlayerId].HoldItem;

	TakePlrsMoney(item._iIvalue);
	if (item._iMagical == ITEM_QUALITY_NORMAL)
		item._iIdentified = false;
	StoreAutoPlace();
	int idx = stextvhold + ((stextlhold - stextup) / 4);
	if (idx == SMITH_ITEMS - 1) {
		smithitem[SMITH_ITEMS - 1]._itype = ITYPE_NONE;
	} else {
		for (; !smithitem[idx + 1].isEmpty(); idx++) {
			smithitem[idx] = smithitem[idx + 1];
		}
		smithitem[idx]._itype = ITYPE_NONE;
	}
	CalcPlrInv(MyPlayerId, true);
}

void SmitBuyEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 12;
		return;
	}

	stextlhold = stextsel;
	stextvhold = stextsval;
	stextshold = STORE_SBUY;

	auto &myPlayer = Players[MyPlayerId];

	int idx = stextsval + ((stextsel - stextup) / 4);
	if (myPlayer._pGold < smithitem[idx]._iIvalue) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = smithitem[idx];
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);

	bool done = AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(MyPlayerId, myPlayer.HoldItem, false);

	if (done || AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, false))
		StartStore(STORE_CONFIRM);
	else
		StartStore(STORE_NOROOM);
	NewCursor(CURSOR_HAND);
}

/**
 * @brief Purchases a premium item from the smith.
 */
void SmithBuyPItem()
{
	auto &item = Players[MyPlayerId].HoldItem;

	TakePlrsMoney(item._iIvalue);

	if (item._iMagical == ITEM_QUALITY_NORMAL)
		item._iIdentified = false;

	StoreAutoPlace();

	int idx = stextvhold + ((stextlhold - stextup) / 4);
	int xx = 0;
	for (int i = 0; idx >= 0; i++) {
		if (!premiumitems[i].isEmpty()) {
			idx--;
			xx = i;
		}
	}

	premiumitems[xx]._itype = ITYPE_NONE;
	numpremium--;
	SpawnPremium(MyPlayerId);
}

void SmitPremiumBuyEnter()
{
	if (stextsel == 22) {
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

	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pGold < premiumitems[idx]._iIvalue) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = premiumitems[idx];
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);
	bool done = AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(MyPlayerId, myPlayer.HoldItem, false);

	if (done || AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, false))
		StartStore(STORE_CONFIRM);
	else
		StartStore(STORE_NOROOM);

	NewCursor(CURSOR_HAND);
}

bool StoreGoldFit(int idx)
{
	int cost = storehold[idx]._iIvalue;
	int sz = cost / MaxGold;
	if (cost % MaxGold != 0)
		sz++;

	NewCursor(storehold[idx]._iCurs + CURSOR_FIRSTITEM);
	int numsqrs = cursW / 28 * (cursH / 28);
	NewCursor(CURSOR_HAND);

	if (numsqrs >= sz)
		return true;

	auto &myPlayer = Players[MyPlayerId];

	for (int8_t itemId : myPlayer.InvGrid) {
		if (itemId == 0)
			numsqrs++;
	}

	for (int i = 0; i < myPlayer._pNumInv; i++) {
		const auto &item = myPlayer.InvList[i];
		if (item._itype == ITYPE_GOLD && item._ivalue != MaxGold) {
			if (cost + item._ivalue <= MaxGold)
				cost = 0;
			else
				cost -= MaxGold - item._ivalue;
		}
	}

	sz = cost / MaxGold;
	if ((cost % MaxGold) != 0)
		sz++;

	return numsqrs >= sz;
}

/**
 * @brief Add gold pile to the players invetory
 * @param v The value of the gold pile
 */
void PlaceStoreGold(int v)
{
	auto &myPlayer = Players[MyPlayerId];

	for (auto &gridNum : myPlayer.InvGrid) {
		if (gridNum == 0) {
			int ii = myPlayer._pNumInv;
			GetGoldSeed(MyPlayerId, &golditem);
			myPlayer.InvList[ii] = golditem;
			myPlayer._pNumInv++;
			gridNum = myPlayer._pNumInv;
			myPlayer.InvList[ii]._ivalue = v;
			SetPlrHandGoldCurs(&myPlayer.InvList[ii]);
			return;
		}
	}
}

/**
 * @brief Sells an item from the player's inventory or belt.
 */
void StoreSellItem()
{
	auto &myPlayer = Players[MyPlayerId];

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
	myPlayer._pGold += cost;
	for (int i = 0; i < myPlayer._pNumInv && cost > 0; i++) {
		auto &item = myPlayer.InvList[i];
		if (item._itype == ITYPE_GOLD && item._ivalue != MaxGold) {
			if (cost + item._ivalue <= MaxGold) {
				item._ivalue += cost;
				cost = 0;
			} else {
				cost -= MaxGold - item._ivalue;
				item._ivalue = MaxGold;
			}
			SetPlrHandGoldCurs(&myPlayer.InvList[i]);
		}
	}
	if (cost > 0) {
		while (cost > MaxGold) {
			PlaceStoreGold(MaxGold);
			cost -= MaxGold;
		}
		PlaceStoreGold(cost);
	}
}

void SmitSellEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 16;
		return;
	}

	stextlhold = stextsel;
	int idx = stextsval + ((stextsel - stextup) / 4);
	stextshold = STORE_SSELL;
	stextvhold = stextsval;
	Players[MyPlayerId].HoldItem = storehold[idx];

	if (StoreGoldFit(idx))
		StartStore(STORE_CONFIRM);
	else
		StartStore(STORE_NOROOM);
}

/**
 * @brief Repairs an item in the player's inventory or body in the smith.
 */
void SmithRepairItem()
{
	auto &myPlayer = Players[MyPlayerId];

	TakePlrsMoney(myPlayer.HoldItem._iIvalue);

	int idx = stextvhold + ((stextlhold - stextup) / 4);
	storehold[idx]._iDurability = storehold[idx]._iMaxDur;

	int8_t i = storehidx[idx];

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

void SmitRepairEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 18;
		return;
	}

	stextshold = STORE_SREPAIR;
	stextlhold = stextsel;
	stextvhold = stextsval;

	int idx = stextsval + ((stextsel - stextup) / 4);

	auto &myPlayer = Players[MyPlayerId];

	myPlayer.HoldItem = storehold[idx];
	if (myPlayer._pGold < storehold[idx]._iIvalue)
		StartStore(STORE_NOMONEY);
	else
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
void WitchBuyItem()
{
	auto &myPlayer = Players[MyPlayerId];

	int idx = stextvhold + ((stextlhold - stextup) / 4);

	if (idx < 3)
		myPlayer.HoldItem._iSeed = AdvanceRndSeed();

	TakePlrsMoney(myPlayer.HoldItem._iIvalue);
	StoreAutoPlace();

	if (idx >= 3) {
		if (idx == WITCH_ITEMS - 1) {
			witchitem[WITCH_ITEMS - 1]._itype = ITYPE_NONE;
		} else {
			for (; !witchitem[idx + 1].isEmpty(); idx++) {
				witchitem[idx] = witchitem[idx + 1];
			}
			witchitem[idx]._itype = ITYPE_NONE;
		}
	}

	CalcPlrInv(MyPlayerId, true);
}

void WitchBuyEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 14;
		return;
	}

	stextlhold = stextsel;
	stextvhold = stextsval;
	stextshold = STORE_WBUY;

	auto &myPlayer = Players[MyPlayerId];

	int idx = stextsval + ((stextsel - stextup) / 4);

	if (myPlayer._pGold < witchitem[idx]._iIvalue) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = witchitem[idx];
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);
	bool done = AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(MyPlayerId, myPlayer.HoldItem, false);

	if (done || AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, false) || AutoPlaceItemInBelt(myPlayer, myPlayer.HoldItem, false))
		StartStore(STORE_CONFIRM);
	else
		StartStore(STORE_NOROOM);

	NewCursor(CURSOR_HAND);
}

void WitchSellEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 16;
		return;
	}

	stextlhold = stextsel;
	stextshold = STORE_WSELL;
	stextvhold = stextsval;

	int idx = stextsval + ((stextsel - stextup) / 4);
	Players[MyPlayerId].HoldItem = storehold[idx];
	if (StoreGoldFit(idx))
		StartStore(STORE_CONFIRM);
	else
		StartStore(STORE_NOROOM);
}

/**
 * @brief Recharges an item in the player's inventory or body in the witch.
 */
void WitchRechargeItem()
{
	auto &myPlayer = Players[MyPlayerId];

	TakePlrsMoney(myPlayer.HoldItem._iIvalue);

	int idx = stextvhold + ((stextlhold - stextup) / 4);
	storehold[idx]._iCharges = storehold[idx]._iMaxCharges;

	int8_t i = storehidx[idx];
	if (i < 0)
		myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges = myPlayer.InvBody[INVLOC_HAND_LEFT]._iMaxCharges;
	else
		myPlayer.InvList[i]._iCharges = myPlayer.InvList[i]._iMaxCharges;

	CalcPlrInv(MyPlayerId, true);
}

void WitchRechargeEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 18;
		return;
	}

	stextshold = STORE_WRECHARGE;
	stextlhold = stextsel;
	stextvhold = stextsval;

	auto &myPlayer = Players[MyPlayerId];

	int idx = stextsval + ((stextsel - stextup) / 4);
	myPlayer.HoldItem = storehold[idx];
	if (myPlayer._pGold < storehold[idx]._iIvalue)
		StartStore(STORE_NOMONEY);
	else
		StartStore(STORE_CONFIRM);
}

void BoyEnter()
{
	if (!boyitem.isEmpty() && stextsel == 18) {
		if (Players[MyPlayerId]._pGold < 50) {
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

void BoyBuyItem()
{
	TakePlrsMoney(Players[MyPlayerId].HoldItem._iIvalue);
	StoreAutoPlace();
	boyitem._itype = ITYPE_NONE;
	stextshold = STORE_BOY;
	CalcPlrInv(MyPlayerId, true);
	stextlhold = 12;
}

/**
 * @brief Purchases an item from the healer.
 */
void HealerBuyItem()
{
	auto &item = Players[MyPlayerId].HoldItem;

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
	StoreAutoPlace();

	if (!gbIsMultiplayer) {
		if (idx < 2)
			return;
	} else {
		if (idx < 3)
			return;
	}
	idx = stextvhold + ((stextlhold - stextup) / 4);
	if (idx == 19) {
		healitem[19]._itype = ITYPE_NONE;
	} else {
		for (; !healitem[idx + 1].isEmpty(); idx++) {
			healitem[idx] = healitem[idx + 1];
		}
		healitem[idx]._itype = ITYPE_NONE;
	}
	CalcPlrInv(MyPlayerId, true);
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

	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pGold < price) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = boyitem;
	myPlayer.HoldItem._iIvalue = price;
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);

	bool done = false;
	if (AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(MyPlayerId, myPlayer.HoldItem, false)) {
		done = true;
	}

	if (!done) {
		done = AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, false);
	}

	StartStore(done ? STORE_CONFIRM : STORE_NOROOM);

	NewCursor(CURSOR_HAND);
}

void StorytellerIdentifyItem()
{
	auto &myPlayer = Players[MyPlayerId];

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
	myPlayer.HoldItem._iIdentified = true;
	TakePlrsMoney(myPlayer.HoldItem._iIvalue);
	CalcPlrInv(MyPlayerId, true);
}

void ConfirmEnter()
{
	if (stextsel == 18) {
		switch (stextshold) {
		case STORE_SBUY:
			SmithBuyItem();
			break;
		case STORE_SSELL:
		case STORE_WSELL:
			StoreSellItem();
			break;
		case STORE_SREPAIR:
			SmithRepairItem();
			break;
		case STORE_WBUY:
			WitchBuyItem();
			break;
		case STORE_WRECHARGE:
			WitchRechargeItem();
			break;
		case STORE_BBOY:
			BoyBuyItem();
			break;
		case STORE_HBUY:
			HealerBuyItem();
			break;
		case STORE_SIDENTIFY:
			StorytellerIdentifyItem();
			StartStore(STORE_IDSHOW);
			return;
		case STORE_SPBUY:
			SmithBuyPItem();
			break;
		default:
			break;
		}
	}

	StartStore(stextshold);

	if (stextsel == 22)
		return;

	stextsel = stextlhold;
	stextsval = std::min(stextvhold, stextsmax);

	while (stextsel != -1 && !stext[stextsel]._ssel) {
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
	case 16:
		stextflag = STORE_NONE;
		break;
	}
}

void HealerBuyEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_HEALER);
		stextsel = 16;
		return;
	}

	stextlhold = stextsel;
	stextvhold = stextsval;
	stextshold = STORE_HBUY;

	int idx = stextsval + ((stextsel - stextup) / 4);

	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pGold < healitem[idx]._iIvalue) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = healitem[idx];
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);

	bool done = AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(MyPlayerId, myPlayer.HoldItem, false);

	if (done || AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, false) || AutoPlaceItemInBelt(myPlayer, myPlayer.HoldItem, false))
		StartStore(STORE_CONFIRM);
	else
		StartStore(STORE_NOROOM);

	NewCursor(CURSOR_HAND);
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
	if (stextsel == 22) {
		StartStore(STORE_STORY);
		stextsel = 14;
		return;
	}

	stextshold = STORE_SIDENTIFY;
	stextlhold = stextsel;
	stextvhold = stextsval;

	auto &myPlayer = Players[MyPlayerId];

	int idx = stextsval + ((stextsel - stextup) / 4);
	myPlayer.HoldItem = storehold[idx];
	if (myPlayer._pGold < storehold[idx]._iIvalue)
		StartStore(STORE_NOMONEY);
	else
		StartStore(STORE_CONFIRM);
}

void TalkEnter()
{
	if (stextsel == 22) {
		StartStore(stextshold);
		stextsel = stextlhold;
		return;
	}

	int sn = 0;
	for (int i = 0; i < MAXQUESTS; i++) {
		if (Quests[i]._qactive == QUEST_ACTIVE && QuestDialogTable[talker][i] != TEXT_NONE && Quests[i]._qlog)
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

	for (int i = 0; i < MAXQUESTS; i++) {
		if (Quests[i]._qactive == QUEST_ACTIVE && QuestDialogTable[talker][i] != TEXT_NONE && Quests[i]._qlog) {
			if (sn == stextsel) {
				InitQTextMsg(QuestDialogTable[talker][i]);
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

int TakeGold(PlayerStruct &player, int cost, bool skipMaxPiles)
{
	for (int i = 0; i < player._pNumInv; i++) {
		auto &item = player.InvList[i];
		if (item._itype != ITYPE_GOLD || (skipMaxPiles && item._ivalue == MaxGold))
			continue;

		if (cost < item._ivalue) {
			item._ivalue -= cost;
			SetPlrHandGoldCurs(&player.InvList[i]);
			return 0;
		}

		cost -= item._ivalue;
		player.RemoveInvItem(i);
		i = -1;
	}

	return cost;
}

void DrawSelector(const Surface &out, const Rectangle &rect, const char *text, UiFlags flags)
{
	int lineWidth = GetLineWidth(text);

	int x1 = rect.position.x - 20;
	if (HasAnyOf(flags, UiFlags::AlignCenter))
		x1 += (rect.size.width - lineWidth) / 2;

	CelDrawTo(out, { x1, rect.position.y + 1 }, *pSPentSpn2Cels, PentSpn2Spin());

	int x2 = rect.position.x + rect.size.width + 5;
	if (HasAnyOf(flags, UiFlags::AlignCenter))
		x2 = rect.position.x + (rect.size.width - lineWidth) / 2 + lineWidth + 5;

	CelDrawTo(out, { x2, rect.position.y + 1 }, *pSPentSpn2Cels, PentSpn2Spin());
}

} // namespace

void AddStoreHoldRepair(ItemStruct *itm, int8_t i)
{
	ItemStruct *item;
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
	pSTextBoxCels = LoadCel("Data\\TextBox2.CEL", 271);
	pSTextSlidCels = LoadCel("Data\\TextSlid.CEL", 12);
	ClearSText(0, STORE_LINES);
	stextflag = STORE_NONE;
	stextsize = false;
	stextscrl = false;
	numpremium = 0;
	premiumlevel = 1;

	for (auto &premiumitem : premiumitems)
		premiumitem._itype = ITYPE_NONE;

	boyitem._itype = ITYPE_NONE;
	boylevel = 0;
}

void SetupTownStores()
{
	SetRndSeed(glSeedTbl[currlevel] * SDL_GetTicks());

	auto &myPlayer = Players[MyPlayerId];

	int l = myPlayer._pLevel / 2;
	if (!gbIsMultiplayer) {
		l = 0;
		for (int i = 0; i < NUMLEVELS; i++) {
			if (myPlayer._pLvlVisited[i])
				l = i;
		}
	}

	l = clamp(l + 2, 6, 16);
	SpawnStoreGold();
	SpawnSmith(l);
	SpawnWitch(l);
	SpawnHealer(l);
	SpawnBoy(myPlayer._pLevel);
	SpawnPremium(MyPlayerId);
}

void FreeStoreMem()
{
	pSTextBoxCels = std::nullopt;
	pSTextSlidCels = std::nullopt;
}

void PrintSString(const Surface &out, int margin, int line, const char *text, UiFlags flags, int price)
{
	int sx = PANEL_X + 32 + margin;
	if (!stextsize) {
		sx += 320;
	}

	int sy = UI_OFFSET_Y + 44 + line * 12 + stext[line]._syoff;

	int width = stextsize ? 575 : 255;
	if (stextscrl && line >= 4 && line <= 20) {
		width -= 9; // Space for the selector
	}
	width -= margin * 2;

	const Rectangle rect { { sx, sy }, { width, 0 } };
	DrawString(out, text, rect, flags);
	if (price > 0) {
		char valstr[32];
		sprintf(valstr, "%i", price);
		DrawString(out, valstr, rect, flags | UiFlags::AlignRight);
	}

	if (stextsel == line) {
		DrawSelector(out, rect, text, flags);
	}
}

void DrawSLine(const Surface &out, int y)
{
	int sx = 26;
	int sy = y * 12;
	int width = 587;

	if (!stextsize) {
		sx += SPANEL_WIDTH;
		width -= SPANEL_WIDTH;
	}

	BYTE *src = out.at(PANEL_LEFT + sx, UI_OFFSET_Y + 25);
	BYTE *dst = out.at(PANEL_X + sx, UI_OFFSET_Y + sy + 38);

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
		stext[i]._sstr[0] = 0;
		stext[i].flags = UiFlags::None;
		stext[i]._sline = 0;
		stext[i]._ssel = false;
		stext[i]._sval = 0;
	}
}

void StartStore(talk_id s)
{
	sbookflag = false;
	invflag = false;
	chrflag = false;
	QuestLogIsOpen = false;
	dropGoldFlag = false;
	ClearSText(0, STORE_LINES);
	ReleaseStoreBtn();
	switch (s) {
	case STORE_SMITH:
		StartSmith();
		break;
	case STORE_SBUY:
		if (storenumh > 0)
			StartSmithBuy();
		else
			StartSmith();
		break;
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
		StoreConfirm();
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
		StartStorytellerIdentifyShow();
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
		if (stext[i]._ssel) {
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

	for (int i = 0; i < STORE_LINES; i++) {
		if (stext[i]._sline != 0)
			DrawSLine(out, i);
		if (stext[i]._sstr[0] != '\0')
			PrintSString(out, stext[i]._sx, i, stext[i]._sstr, stext[i].flags, stext[i]._sval);
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
		stextsel = 16;
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
		while (!stext[stextsel]._ssel) {
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

	while (!stext[stextsel]._ssel) {
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
		while (!stext[stextsel]._ssel) {
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

	while (!stext[stextsel]._ssel) {
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
	auto &myPlayer = Players[MyPlayerId];

	myPlayer._pGold -= cost;

	cost = TakeGold(myPlayer, cost, true);
	if (cost != 0) {
		TakeGold(myPlayer, cost, false);
	}
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
		SmitPremiumBuyEnter();
		break;
	case STORE_SBUY:
		SmitBuyEnter();
		break;
	case STORE_SSELL:
		SmitSellEnter();
		break;
	case STORE_SREPAIR:
		SmitRepairEnter();
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
		ConfirmEnter();
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
	if (qtextflag) {
		qtextflag = false;
		if (leveltype == DTYPE_TOWN)
			stream_stop();
	} else if (stextsel != -1 && MousePosition.y >= (32 + UI_OFFSET_Y) && MousePosition.y <= (320 + UI_OFFSET_Y)) {
		if (!stextsize) {
			if (MousePosition.x < 344 + PANEL_LEFT || MousePosition.x > 616 + PANEL_LEFT)
				return;
		} else {
			if (MousePosition.x < 24 + PANEL_LEFT || MousePosition.x > 616 + PANEL_LEFT)
				return;
		}
		int y = (MousePosition.y - (32 + UI_OFFSET_Y)) / 12;
		if (stextscrl && MousePosition.x > 600 + PANEL_LEFT) {
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
		} else if (y >= 5) {
			if (y >= 23)
				y = 22;
			if (stextscrl && y < 21 && !stext[y]._ssel) {
				if (stext[y - 2]._ssel) {
					y -= 2;
				} else if (stext[y - 1]._ssel) {
					y--;
				}
			}
			if (stext[y]._ssel || (stextscrl && y == 22)) {
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
