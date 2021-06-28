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

void DrawSTextBack(const CelOutputBuffer &out)
{
	CelDrawTo(out, { PANEL_X + 320 + 24, 327 + UI_OFFSET_Y }, *pSTextBoxCels, 1);
	DrawHalfTransparentRectTo(out, PANEL_X + 347, UI_OFFSET_Y + 28, 265, 297);
}

void DrawSSlider(const CelOutputBuffer &out, int y1, int y2)
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

void AddSText(int x, int y, const char *str, uint16_t flags, bool sel)
{
	stext[y]._sx = x;
	stext[y]._syoff = 0;
	strcpy(stext[y]._sstr, str);
	stext[y].flags = flags;
	stext[y]._sline = 0;
	stext[y]._ssel = sel;
}

void PrintStoreItem(ItemStruct *x, int l, uint16_t flags)
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
	auto &myPlayer = plr[myplr];

	if (AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(myplr, myPlayer.HoldItem)) {
		return;
	}

	if (AutoPlaceItemInBelt(myPlayer, myPlayer.HoldItem, true)) {
		return;
	}

	AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, true);
}

void S_StartSmith()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, _("Welcome to the"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 3, _("Blacksmith's shop"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 7, _("Would you like to:"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 10, _("Talk to Griswold"), UIS_BLUE | UIS_CENTER, true);
	AddSText(0, 12, _("Buy basic items"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 14, _("Buy premium items"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 16, _("Sell items"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 18, _("Repair items"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 20, _("Leave the shop"), UIS_SILVER | UIS_CENTER, true);
	AddSLine(5);
	storenumh = 20;
}

void S_ScrollSBuy(int idx)
{
	ClearSText(5, 21);
	stextup = 5;

	for (int l = 5; l < 20; l += 4) {
		if (!smithitem[idx].isEmpty()) {
			uint16_t iclr = smithitem[idx].getTextColorWithStatCheck();

			if (smithitem[idx]._iMagical != ITEM_QUALITY_NORMAL) {
				AddSText(20, l, smithitem[idx]._iIName, iclr, true);
			} else {
				AddSText(20, l, smithitem[idx]._iName, iclr, true);
			}

			AddSTextVal(l, smithitem[idx]._iIvalue);
			PrintStoreItem(&smithitem[idx], l + 1, iclr);
			stextdown = l;
			idx++;
		}
	}

	if (stextsel != -1 && !stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}

void S_StartSBuy()
{
	stextsize = true;
	stextscrl = true;
	stextsval = 0;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("I have these items for sale:             Your gold: {:d}"), plr[myplr]._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSBuy(stextsval);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, false);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (int i = 0; !smithitem[i].isEmpty(); i++) {
		storenumh++;
	}

	stextsmax = storenumh - 4;
	if (stextsmax < 0)
		stextsmax = 0;
}

void S_ScrollSPBuy(int boughtitems)
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
			uint16_t iclr = premiumitems[idx].getTextColorWithStatCheck();
			AddSText(20, l, premiumitems[idx]._iIName, iclr, true);
			AddSTextVal(l, premiumitems[idx]._iIvalue);
			PrintStoreItem(&premiumitems[idx], l + 1, iclr);
			stextdown = l;
		} else {
			l -= 4;
		}
		idx++;
	}
	if (stextsel != -1 && !stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}

bool S_StartSPBuy()
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
	strcpy(tempstr, fmt::format(_("I have these premium items for sale:     Your gold: {:d}"), plr[myplr]._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, false);
	OffsetSTextY(22, 6);

	stextsmax = storenumh - 4;
	if (stextsmax < 0)
		stextsmax = 0;

	S_ScrollSPBuy(stextsval);

	return true;
}

bool SmithSellOk(int i)
{
	ItemStruct *pI;

	if (i >= 0) {
		pI = &plr[myplr].InvList[i];
	} else {
		pI = &plr[myplr].SpdList[-(i + 1)];
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

void S_ScrollSSell(int idx)
{
	int l;

	ClearSText(5, 21);
	stextup = 5;

	for (l = 5; l < 20; l += 4) {
		if (idx >= storenumh)
			break;
		if (!storehold[idx].isEmpty()) {
			uint16_t iclr = storehold[idx].getTextColorWithStatCheck();

			if (storehold[idx]._iMagical != ITEM_QUALITY_NORMAL && storehold[idx]._iIdentified) {
				AddSText(20, l, storehold[idx]._iIName, iclr, true);
				AddSTextVal(l, storehold[idx]._iIvalue);
			} else {
				AddSText(20, l, storehold[idx]._iName, iclr, true);
				AddSTextVal(l, storehold[idx]._ivalue);
			}

			PrintStoreItem(&storehold[idx], l + 1, iclr);
			stextdown = l;
		}
		idx++;
	}

	stextsmax = storenumh - 4;
	if (stextsmax < 0)
		stextsmax = 0;
}

void S_StartSSell()
{
	stextsize = true;
	bool sellok = false;
	storenumh = 0;

	for (auto &item : storehold) {
		item._itype = ITYPE_NONE;
	}

	const auto &myPlayer = plr[myplr];

	for (int i = 0; i < myPlayer._pNumInv; i++) {
		if (storenumh >= 48)
			break;
		if (SmithSellOk(i)) {
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
		if (SmithSellOk(-(i + 1))) {
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

		AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Which item is for sale?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSSell(stextsval);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
	OffsetSTextY(22, 6);
}

bool SmithRepairOk(int i)
{
	const auto &myPlayer = plr[myplr];

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

void S_StartSRepair()
{
	stextsize = true;
	bool repairok = false;
	storenumh = 0;

	for (auto &item : storehold) {
		item._itype = ITYPE_NONE;
	}

	auto &myPlayer = plr[myplr];

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

		AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Repair which item?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSSell(stextsval);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
	OffsetSTextY(22, 6);
}

void FillManaPlayer()
{
	if (!sgOptions.Gameplay.bAdriaRefillsMana)
		return;

	auto &myPlayer = plr[myplr];

	if (myPlayer._pMana != myPlayer._pMaxMana) {
		PlaySFX(IS_CAST8);
	}
	myPlayer._pMana = myPlayer._pMaxMana;
	myPlayer._pManaBase = myPlayer._pMaxManaBase;
	drawmanaflag = true;
}

void S_StartWitch()
{
	FillManaPlayer();
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("Witch's shack"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 9, _("Would you like to:"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 12, _("Talk to Adria"), UIS_BLUE | UIS_CENTER, true);
	AddSText(0, 14, _("Buy items"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 16, _("Sell items"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 18, _("Recharge staves"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 20, _("Leave the shack"), UIS_SILVER | UIS_CENTER, true);
	AddSLine(5);
	storenumh = 20;
}

void S_ScrollWBuy(int idx)
{
	ClearSText(5, 21);
	stextup = 5;

	for (int l = 5; l < 20; l += 4) {
		if (!witchitem[idx].isEmpty()) {
			uint16_t iclr = witchitem[idx].getTextColorWithStatCheck();

			if (witchitem[idx]._iMagical != ITEM_QUALITY_NORMAL) {
				AddSText(20, l, witchitem[idx]._iIName, iclr, true);
			} else {
				AddSText(20, l, witchitem[idx]._iName, iclr, true);
			}

			AddSTextVal(l, witchitem[idx]._iIvalue);
			PrintStoreItem(&witchitem[idx], l + 1, iclr);
			stextdown = l;
			idx++;
		}
	}

	if (stextsel != -1 && !stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}

void S_StartWBuy()
{
	int i;

	stextsize = true;
	stextscrl = true;
	stextsval = 0;
	stextsmax = 20;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("I have these items for sale:             Your gold: {:d}"), plr[myplr]._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollWBuy(stextsval);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, false);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (i = 0; !witchitem[i].isEmpty(); i++) {
		storenumh++;
	}
	stextsmax = storenumh - 4;
	if (stextsmax < 0)
		stextsmax = 0;
}

bool WitchSellOk(int i)
{
	ItemStruct *pI;

	bool rv = false;

	if (i >= 0)
		pI = &plr[myplr].InvList[i];
	else
		pI = &plr[myplr].SpdList[-(i + 1)];

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

void S_StartWSell()
{
	stextsize = true;
	bool sellok = false;
	storenumh = 0;

	for (auto &item : storehold) {
		item._itype = ITYPE_NONE;
	}

	const auto &myPlayer = plr[myplr];

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

		AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Which item is for sale?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSSell(stextsval);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
	OffsetSTextY(22, 6);
}

bool WitchRechargeOk(int i)
{
	const auto &item = plr[myplr].InvList[i];

	if (item._itype == ITYPE_STAFF && item._iCharges != item._iMaxCharges) {
		return true;
	}

	if ((item._iMiscId == IMISC_UNIQUE || item._iMiscId == IMISC_STAFF) && item._iCharges < item._iMaxCharges) {
		return true;
	}

	return false;
}

void AddStoreHoldRecharge(ItemStruct itm, int i)
{
	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue += spelldata[itm._iSpell].sStaffCost;
	storehold[storenumh]._ivalue = storehold[storenumh]._ivalue * (storehold[storenumh]._iMaxCharges - storehold[storenumh]._iCharges) / (storehold[storenumh]._iMaxCharges * 2);
	storehold[storenumh]._iIvalue = storehold[storenumh]._ivalue;
	storehidx[storenumh] = i;
	storenumh++;
}

void S_StartWRecharge()
{
	stextsize = true;
	bool rechargeok = false;
	storenumh = 0;

	for (auto &item : storehold) {
		item._itype = ITYPE_NONE;
	}

	const auto &myPlayer = plr[myplr];
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

		AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Recharge which item?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSSell(stextsval);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
	OffsetSTextY(22, 6);
}

void S_StartNoMoney()
{
	StartStore(stextshold);
	stextscrl = false;
	stextsize = true;
	ClearSText(5, 23);
	AddSText(0, 14, _("You do not have enough gold"), UIS_SILVER | UIS_CENTER, true);
}

void S_StartNoRoom()
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);
	AddSText(0, 14, _("You do not have enough room in inventory"), UIS_SILVER | UIS_CENTER, true);
}

void S_StartConfirm()
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);

	auto &item = plr[myplr].HoldItem;

	uint16_t iclr = item.getTextColorWithStatCheck();

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
		AddSText(20, 8, item._iIName, iclr, false);
	else
		AddSText(20, 8, item._iName, iclr, false);

	AddSTextVal(8, item._iIvalue);
	PrintStoreItem(&item, 9, iclr);

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
	AddSText(0, 15, tempstr, UIS_SILVER | UIS_CENTER, false);
	AddSText(0, 18, _("Yes"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 20, _("No"), UIS_SILVER | UIS_CENTER, true);
}

void S_StartBoy()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("Wirt the Peg-legged boy"), UIS_GOLD | UIS_CENTER, false);
	AddSLine(5);
	if (!boyitem.isEmpty()) {
		AddSText(0, 8, _("Talk to Wirt"), UIS_BLUE | UIS_CENTER, true);
		AddSText(0, 12, _("I have something for sale,"), UIS_GOLD | UIS_CENTER, false);
		AddSText(0, 14, _("but it will cost 50 gold"), UIS_GOLD | UIS_CENTER, false);
		AddSText(0, 16, _("just to take a look. "), UIS_GOLD | UIS_CENTER, false);
		AddSText(0, 18, _("What have you got?"), UIS_SILVER | UIS_CENTER, true);
		AddSText(0, 20, _("Say goodbye"), UIS_SILVER | UIS_CENTER, true);
	} else {
		AddSText(0, 12, _("Talk to Wirt"), UIS_BLUE | UIS_CENTER, true);
		AddSText(0, 18, _("Say goodbye"), UIS_SILVER | UIS_CENTER, true);
	}
}

void S_StartBBoy()
{
	stextsize = true;
	stextscrl = false;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("I have this item for sale:             Your gold: {:d}"), plr[myplr]._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	uint16_t iclr = boyitem.getTextColorWithStatCheck();

	if (boyitem._iMagical != ITEM_QUALITY_NORMAL)
		AddSText(20, 10, boyitem._iIName, iclr, true);
	else
		AddSText(20, 10, boyitem._iName, iclr, true);

	if (gbIsHellfire)
		AddSTextVal(10, boyitem._iIvalue - (boyitem._iIvalue / 4));
	else
		AddSTextVal(10, boyitem._iIvalue + (boyitem._iIvalue / 2));
	PrintStoreItem(&boyitem, 11, iclr);
	AddSText(0, 22, _("Leave"), UIS_SILVER | UIS_CENTER, true);
	OffsetSTextY(22, 6);
}

void HealPlayer()
{
	auto &myPlayer = plr[myplr];

	if (myPlayer._pHitPoints != myPlayer._pMaxHP) {
		PlaySFX(IS_CAST8);
	}
	myPlayer._pHitPoints = myPlayer._pMaxHP;
	myPlayer._pHPBase = myPlayer._pMaxHPBase;
	drawhpflag = true;
}

void S_StartHealer()
{
	HealPlayer();
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, _("Welcome to the"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 3, _("Healer's home"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 9, _("Would you like to:"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 12, _("Talk to Pepin"), UIS_BLUE | UIS_CENTER, true);
	AddSText(0, 14, _("Buy items"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 16, _("Leave Healer's home"), UIS_SILVER | UIS_CENTER, true);
	AddSLine(5);
	storenumh = 20;
}

void S_ScrollHBuy(int idx)
{
	int l;

	ClearSText(5, 21);
	stextup = 5;
	for (l = 5; l < 20; l += 4) {
		if (!healitem[idx].isEmpty()) {
			uint16_t iclr = healitem[idx].getTextColorWithStatCheck();

			AddSText(20, l, healitem[idx]._iName, iclr, true);
			AddSTextVal(l, healitem[idx]._iIvalue);
			PrintStoreItem(&healitem[idx], l + 1, iclr);
			stextdown = l;
			idx++;
		}
	}

	if (stextsel != -1 && !stext[stextsel]._ssel && stextsel != 22)
		stextsel = stextdown;
}

void S_StartHBuy()
{
	int i;

	stextsize = true;
	stextscrl = true;
	stextsval = 0;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("I have these items for sale:             Your gold: {:d}"), plr[myplr]._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollHBuy(stextsval);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, false);
	OffsetSTextY(22, 6);

	storenumh = 0;
	for (i = 0; !healitem[i].isEmpty(); i++) {
		storenumh++;
	}
	stextsmax = storenumh - 4;
	if (stextsmax < 0)
		stextsmax = 0;
}

void S_StartStory()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("The Town Elder"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 9, _("Would you like to:"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 12, _("Talk to Cain"), UIS_BLUE | UIS_CENTER, true);
	AddSText(0, 14, _("Identify an item"), UIS_SILVER | UIS_CENTER, true);
	AddSText(0, 18, _("Say goodbye"), UIS_SILVER | UIS_CENTER, true);
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

void AddStoreHoldId(ItemStruct itm, int i)
{
	storehold[storenumh] = itm;
	storehold[storenumh]._ivalue = 100;
	storehold[storenumh]._iIvalue = 100;
	storehidx[storenumh] = i;
	storenumh++;
}

void S_StartSIdentify()
{
	bool idok = false;
	stextsize = true;
	storenumh = 0;

	for (auto &item : storehold) {
		item._itype = ITYPE_NONE;
	}

	auto &myPlayer = plr[myplr];

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

		AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
		AddSLine(3);
		AddSLine(21);
		AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
		OffsetSTextY(22, 6);
		return;
	}

	stextscrl = true;
	stextsval = 0;
	stextsmax = myPlayer._pNumInv;

	/* TRANSLATORS: This text is white space sensitive. Check for correct alignment! */
	strcpy(tempstr, fmt::format(_("Identify which item?             Your gold: {:d}"), myPlayer._pGold).c_str());

	AddSText(0, 1, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(3);
	AddSLine(21);
	S_ScrollSSell(stextsval);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
	OffsetSTextY(22, 6);
}

void S_StartIdShow()
{
	StartStore(stextshold);
	stextscrl = false;
	ClearSText(5, 23);

	auto &item = plr[myplr].HoldItem;

	uint16_t iclr = item.getTextColorWithStatCheck();

	AddSText(0, 7, _("This item is:"), UIS_SILVER | UIS_CENTER, false);
	AddSText(20, 11, item._iIName, iclr, false);
	PrintStoreItem(&item, 12, iclr);
	AddSText(0, 18, _("Done"), UIS_SILVER | UIS_CENTER, true);
}

void S_StartTalk()
{
	int la;

	stextsize = false;
	stextscrl = false;
	strcpy(tempstr, fmt::format(_("Talk to {:s}"), TownerNames[talker]).c_str());
	AddSText(0, 2, tempstr, UIS_GOLD | UIS_CENTER, false);
	AddSLine(5);
	if (gbIsSpawn) {
		strcpy(tempstr, fmt::format(_("Talking to {:s}"), TownerNames[talker]).c_str());
		AddSText(0, 10, tempstr, UIS_SILVER | UIS_CENTER, false);
		AddSText(0, 12, _("is not available"), UIS_SILVER | UIS_CENTER, false);
		AddSText(0, 14, _("in the shareware"), UIS_SILVER | UIS_CENTER, false);
		AddSText(0, 16, _("version"), UIS_SILVER | UIS_CENTER, false);
		AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
		return;
	}

	int sn = 0;
	for (int i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && Qtalklist[talker][i] != TEXT_NONE && quests[i]._qlog)
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
		if (quests[i]._qactive == QUEST_ACTIVE && Qtalklist[talker][i] != TEXT_NONE && quests[i]._qlog) {
			AddSText(0, sn, _(questlist[i]._qlstr), UIS_SILVER | UIS_CENTER, true);
			sn += la;
		}
	}
	AddSText(0, sn2, _("Gossip"), UIS_BLUE | UIS_CENTER, true);
	AddSText(0, 22, _("Back"), UIS_SILVER | UIS_CENTER, true);
}

void S_StartTavern()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 1, _("Welcome to the"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 3, _("Rising Sun"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 9, _("Would you like to:"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 12, _("Talk to Ogden"), UIS_BLUE | UIS_CENTER, true);
	AddSText(0, 18, _("Leave the tavern"), UIS_SILVER | UIS_CENTER, true);
	AddSLine(5);
	storenumh = 20;
}

void S_StartBarMaid()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, "Gillian", UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 9, _("Would you like to:"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 12, _("Talk to Gillian"), UIS_BLUE | UIS_CENTER, true);
	AddSText(0, 18, _("Say goodbye"), UIS_SILVER | UIS_CENTER, true);
	AddSLine(5);
	storenumh = 20;
}

void S_StartDrunk()
{
	stextsize = false;
	stextscrl = false;
	AddSText(0, 2, _("Farnham the Drunk"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 9, _("Would you like to:"), UIS_GOLD | UIS_CENTER, false);
	AddSText(0, 12, _("Talk to Farnham"), UIS_BLUE | UIS_CENTER, true);
	AddSText(0, 18, _("Say Goodbye"), UIS_SILVER | UIS_CENTER, true);
	AddSLine(5);
	storenumh = 20;
}

void S_SmithEnter()
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
	auto &item = plr[myplr].HoldItem;

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
	CalcPlrInv(myplr, true);
}

void S_SBuyEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_SMITH);
		stextsel = 12;
		return;
	}

	stextlhold = stextsel;
	stextvhold = stextsval;
	stextshold = STORE_SBUY;

	auto &myPlayer = plr[myplr];

	int idx = stextsval + ((stextsel - stextup) / 4);
	if (myPlayer._pGold < smithitem[idx]._iIvalue) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = smithitem[idx];
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);

	bool done = AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(myplr, myPlayer.HoldItem, false);

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
	auto &item = plr[myplr].HoldItem;

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
	SpawnPremium(myplr);
}

void S_SPBuyEnter()
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

	auto &myPlayer = plr[myplr];

	if (myPlayer._pGold < premiumitems[idx]._iIvalue) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = premiumitems[idx];
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);
	bool done = AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(myplr, myPlayer.HoldItem, false);

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
	int numsqrs = cursSize.width / 28 * (cursSize.height / 28);
	NewCursor(CURSOR_HAND);

	if (numsqrs >= sz)
		return true;

	auto &myPlayer = plr[myplr];

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
	auto &myPlayer = plr[myplr];

	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		int xx = i % 10;
		int yy = 10 * (i / 10);
		if (myPlayer.InvGrid[xx + yy] == 0) {
			int ii = myPlayer._pNumInv;
			GetGoldSeed(myplr, &golditem);
			myPlayer.InvList[ii] = golditem;
			myPlayer._pNumInv++;
			myPlayer.InvGrid[xx + yy] = myPlayer._pNumInv;
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
	auto &myPlayer = plr[myplr];

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

void S_SSellEnter()
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
	plr[myplr].HoldItem = storehold[idx];

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
	auto &myPlayer = plr[myplr];

	TakePlrsMoney(myPlayer.HoldItem._iIvalue);

	int idx = stextvhold + ((stextlhold - stextup) / 4);
	storehold[idx]._iDurability = storehold[idx]._iMaxDur;

	int i = storehidx[idx];

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

void S_SRepairEnter()
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

	auto &myPlayer = plr[myplr];

	myPlayer.HoldItem = storehold[idx];
	if (myPlayer._pGold < storehold[idx]._iIvalue)
		StartStore(STORE_NOMONEY);
	else
		StartStore(STORE_CONFIRM);
}

void S_WitchEnter()
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
	auto &myPlayer = plr[myplr];

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

	CalcPlrInv(myplr, true);
}

void S_WBuyEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 14;
		return;
	}

	stextlhold = stextsel;
	stextvhold = stextsval;
	stextshold = STORE_WBUY;

	auto &myPlayer = plr[myplr];

	int idx = stextsval + ((stextsel - stextup) / 4);

	if (myPlayer._pGold < witchitem[idx]._iIvalue) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = witchitem[idx];
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);
	bool done = AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(myplr, myPlayer.HoldItem, false);

	if (done || AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, false) || AutoPlaceItemInBelt(myPlayer, myPlayer.HoldItem, false))
		StartStore(STORE_CONFIRM);
	else
		StartStore(STORE_NOROOM);

	NewCursor(CURSOR_HAND);
}

void S_WSellEnter()
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
	plr[myplr].HoldItem = storehold[idx];
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
	auto &myPlayer = plr[myplr];

	TakePlrsMoney(myPlayer.HoldItem._iIvalue);

	int idx = stextvhold + ((stextlhold - stextup) / 4);
	storehold[idx]._iCharges = storehold[idx]._iMaxCharges;

	int i = storehidx[idx];
	if (i < 0)
		myPlayer.InvBody[INVLOC_HAND_LEFT]._iCharges = myPlayer.InvBody[INVLOC_HAND_LEFT]._iMaxCharges;
	else
		myPlayer.InvList[i]._iCharges = myPlayer.InvList[i]._iMaxCharges;

	CalcPlrInv(myplr, true);
}

void S_WRechargeEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_WITCH);
		stextsel = 18;
		return;
	}

	stextshold = STORE_WRECHARGE;
	stextlhold = stextsel;
	stextvhold = stextsval;

	auto &myPlayer = plr[myplr];

	int idx = stextsval + ((stextsel - stextup) / 4);
	myPlayer.HoldItem = storehold[idx];
	if (myPlayer._pGold < storehold[idx]._iIvalue)
		StartStore(STORE_NOMONEY);
	else
		StartStore(STORE_CONFIRM);
}

void S_BoyEnter()
{
	if (!boyitem.isEmpty() && stextsel == 18) {
		if (plr[myplr]._pGold < 50) {
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
	TakePlrsMoney(plr[myplr].HoldItem._iIvalue);
	StoreAutoPlace();
	boyitem._itype = ITYPE_NONE;
	stextshold = STORE_BOY;
	CalcPlrInv(myplr, true);
	stextlhold = 12;
}

/**
 * @brief Purchases an item from the healer.
 */
void HealerBuyItem()
{
	auto &item = plr[myplr].HoldItem;

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
	CalcPlrInv(myplr, true);
}

void S_BBuyEnter()
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

	auto &myPlayer = plr[myplr];

	if (myPlayer._pGold < price) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = boyitem;
	myPlayer.HoldItem._iIvalue = price;
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);

	bool done = false;
	if (AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(myplr, myPlayer.HoldItem, false)) {
		done = true;
	}

	if (!done) {
		done = AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, false);
	}

	StartStore(done ? STORE_CONFIRM : STORE_NOROOM);

	NewCursor(CURSOR_HAND);
}

void StoryIdItem()
{
	auto &myPlayer = plr[myplr];

	int idx = storehidx[((stextlhold - stextup) / 4) + stextvhold];
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
	CalcPlrInv(myplr, true);
}

void S_ConfirmEnter()
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
			StoryIdItem();
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

void S_HealerEnter()
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

void S_HBuyEnter()
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

	auto &myPlayer = plr[myplr];

	if (myPlayer._pGold < healitem[idx]._iIvalue) {
		StartStore(STORE_NOMONEY);
		return;
	}

	myPlayer.HoldItem = healitem[idx];
	NewCursor(myPlayer.HoldItem._iCurs + CURSOR_FIRSTITEM);

	bool done = AutoEquipEnabled(myPlayer, myPlayer.HoldItem) && AutoEquip(myplr, myPlayer.HoldItem, false);

	if (done || AutoPlaceItemInInventory(myPlayer, myPlayer.HoldItem, false) || AutoPlaceItemInBelt(myPlayer, myPlayer.HoldItem, false))
		StartStore(STORE_CONFIRM);
	else
		StartStore(STORE_NOROOM);

	NewCursor(CURSOR_HAND);
}

void S_StoryEnter()
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

void S_SIDEnter()
{
	if (stextsel == 22) {
		StartStore(STORE_STORY);
		stextsel = 14;
		return;
	}

	stextshold = STORE_SIDENTIFY;
	stextlhold = stextsel;
	stextvhold = stextsval;

	auto &myPlayer = plr[myplr];

	int idx = stextsval + ((stextsel - stextup) / 4);
	myPlayer.HoldItem = storehold[idx];
	if (myPlayer._pGold < storehold[idx]._iIvalue)
		StartStore(STORE_NOMONEY);
	else
		StartStore(STORE_CONFIRM);
}

void S_TalkEnter()
{
	if (stextsel == 22) {
		StartStore(stextshold);
		stextsel = stextlhold;
		return;
	}

	int sn = 0;
	for (int i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && Qtalklist[talker][i] != TEXT_NONE && quests[i]._qlog)
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
		SetRndSeed(towners[talker]._tSeed);
		auto tq = static_cast<_speech_id>(gossipstart + GenerateRnd(gossipend - gossipstart + 1));
		InitQTextMsg(tq);
		return;
	}

	for (int i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && Qtalklist[talker][i] != TEXT_NONE && quests[i]._qlog) {
			if (sn == stextsel) {
				InitQTextMsg(Qtalklist[talker][i]);
			}
			sn += la;
		}
	}
}

void S_TavernEnter()
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

void S_BarmaidEnter()
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

void S_DrunkEnter()
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

} // namespace

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

void AddStoreHoldRepair(ItemStruct *itm, int i)
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

	auto &myPlayer = plr[myplr];

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
	SpawnPremium(myplr);
}

void FreeStoreMem()
{
	pSTextBoxCels = std::nullopt;
	pSTextSlidCels = std::nullopt;
}

static void DrawSelector(const CelOutputBuffer &out, const Rectangle &rect, const char *text, uint16_t flags)
{
	int lineWidth = GetLineWidth(text);

	int x1 = rect.position.x - 20;
	if ((flags & UIS_CENTER) != 0)
		x1 += (rect.size.width - lineWidth) / 2;

	CelDrawTo(out, { x1, rect.position.y + 1 }, *pSPentSpn2Cels, PentSpn2Spin());

	int x2 = rect.position.x + rect.size.width + 5;
	if ((flags & UIS_CENTER) != 0)
		x2 = rect.position.x + (rect.size.width - lineWidth) / 2 + lineWidth + 5;

	CelDrawTo(out, { x2, rect.position.y + 1 }, *pSPentSpn2Cels, PentSpn2Spin());
}

void PrintSString(const CelOutputBuffer &out, int margin, int line, const char *text, uint16_t flags, int price)
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

	const Rectangle rect { sx, sy, width, 0 };
	DrawString(out, text, rect, flags);
	if (price > 0) {
		char valstr[32];
		sprintf(valstr, "%i", price);
		DrawString(out, valstr, rect, flags | UIS_RIGHT);
	}

	if (stextsel == line) {
		DrawSelector(out, rect, text, flags);
	}
}

void DrawSLine(const CelOutputBuffer &out, int y)
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
		stext[i].flags = 0;
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
	questlog = false;
	dropGoldFlag = false;
	ClearSText(0, STORE_LINES);
	ReleaseStoreBtn();
	switch (s) {
	case STORE_SMITH:
		S_StartSmith();
		break;
	case STORE_SBUY:
		if (storenumh > 0)
			S_StartSBuy();
		else
			S_StartSmith();
		break;
	case STORE_SSELL:
		S_StartSSell();
		break;
	case STORE_SREPAIR:
		S_StartSRepair();
		break;
	case STORE_WITCH:
		S_StartWitch();
		break;
	case STORE_WBUY:
		if (storenumh > 0)
			S_StartWBuy();
		break;
	case STORE_WSELL:
		S_StartWSell();
		break;
	case STORE_WRECHARGE:
		S_StartWRecharge();
		break;
	case STORE_NOMONEY:
		S_StartNoMoney();
		break;
	case STORE_NOROOM:
		S_StartNoRoom();
		break;
	case STORE_CONFIRM:
		S_StartConfirm();
		break;
	case STORE_BOY:
		S_StartBoy();
		break;
	case STORE_BBOY:
		S_StartBBoy();
		break;
	case STORE_HEALER:
		S_StartHealer();
		break;
	case STORE_STORY:
		S_StartStory();
		break;
	case STORE_HBUY:
		if (storenumh > 0)
			S_StartHBuy();
		break;
	case STORE_SIDENTIFY:
		S_StartSIdentify();
		break;
	case STORE_SPBUY:
		if (!S_StartSPBuy())
			return;
		break;
	case STORE_GOSSIP:
		S_StartTalk();
		break;
	case STORE_IDSHOW:
		S_StartIdShow();
		break;
	case STORE_TAVERN:
		S_StartTavern();
		break;
	case STORE_DRUNK:
		S_StartDrunk();
		break;
	case STORE_BARMAID:
		S_StartBarMaid();
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

void DrawSText(const CelOutputBuffer &out)
{
	int i;

	if (!stextsize)
		DrawSTextBack(out);
	else
		DrawQTextBack(out);

	if (stextscrl) {
		switch (stextflag) {
		case STORE_SBUY:
			S_ScrollSBuy(stextsval);
			break;
		case STORE_SSELL:
		case STORE_SREPAIR:
		case STORE_WSELL:
		case STORE_WRECHARGE:
		case STORE_SIDENTIFY:
			S_ScrollSSell(stextsval);
			break;
		case STORE_WBUY:
			S_ScrollWBuy(stextsval);
			break;
		case STORE_HBUY:
			S_ScrollHBuy(stextsval);
			break;
		case STORE_SPBUY:
			S_ScrollSPBuy(stextsval);
			break;
		default:
			break;
		}
	}

	for (i = 0; i < STORE_LINES; i++) {
		if (stext[i]._sline != 0)
			DrawSLine(out, i);
		if (stext[i]._sstr[0] != '\0')
			PrintSString(out, stext[i]._sx, i, stext[i]._sstr, stext[i].flags, stext[i]._sval);
	}

	if (stextscrl)
		DrawSSlider(out, 4, 20);
}

void STextESC()
{
	if (qtextflag) {
		qtextflag = false;
		if (leveltype == DTYPE_TOWN)
			stream_stop();
	} else {
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
}

void STextUp()
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

void STextDown()
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

void STextPrior()
{
	PlaySFX(IS_TITLEMOV);
	if (stextsel != -1 && stextscrl) {
		if (stextsel == stextup) {
			if (stextsval != 0)
				stextsval -= 4;
			if (stextsval < 0)
				stextsval = 0;
		} else {
			stextsel = stextup;
		}
	}
}

void STextNext()
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

static int TakeGold(PlayerStruct &player, int cost, bool skipMaxPiles)
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

void TakePlrsMoney(int cost)
{
	auto &myPlayer = plr[myplr];

	myPlayer._pGold -= cost;

	cost = TakeGold(myPlayer, cost, true);
	if (cost != 0) {
		TakeGold(myPlayer, cost, false);
	}
}

void STextEnter()
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
		S_SmithEnter();
		break;
	case STORE_SPBUY:
		S_SPBuyEnter();
		break;
	case STORE_SBUY:
		S_SBuyEnter();
		break;
	case STORE_SSELL:
		S_SSellEnter();
		break;
	case STORE_SREPAIR:
		S_SRepairEnter();
		break;
	case STORE_WITCH:
		S_WitchEnter();
		break;
	case STORE_WBUY:
		S_WBuyEnter();
		break;
	case STORE_WSELL:
		S_WSellEnter();
		break;
	case STORE_WRECHARGE:
		S_WRechargeEnter();
		break;
	case STORE_NOMONEY:
	case STORE_NOROOM:
		StartStore(stextshold);
		stextsel = stextlhold;
		stextsval = stextvhold;
		break;
	case STORE_CONFIRM:
		S_ConfirmEnter();
		break;
	case STORE_BOY:
		S_BoyEnter();
		break;
	case STORE_BBOY:
		S_BBuyEnter();
		break;
	case STORE_HEALER:
		S_HealerEnter();
		break;
	case STORE_STORY:
		S_StoryEnter();
		break;
	case STORE_HBUY:
		S_HBuyEnter();
		break;
	case STORE_SIDENTIFY:
		S_SIDEnter();
		break;
	case STORE_GOSSIP:
		S_TalkEnter();
		break;
	case STORE_IDSHOW:
		StartStore(STORE_SIDENTIFY);
		break;
	case STORE_DRUNK:
		S_DrunkEnter();
		break;
	case STORE_TAVERN:
		S_TavernEnter();
		break;
	case STORE_BARMAID:
		S_BarmaidEnter();
		break;
	case STORE_NONE:
		break;
	}
}

void CheckStoreBtn()
{
	int y;

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
		y = (MousePosition.y - (32 + UI_OFFSET_Y)) / 12;
		if (stextscrl && MousePosition.x > 600 + PANEL_LEFT) {
			if (y == 4) {
				if (stextscrlubtn <= 0) {
					STextUp();
					stextscrlubtn = 10;
				} else {
					stextscrlubtn--;
				}
			}
			if (y == 20) {
				if (stextscrldbtn <= 0) {
					STextDown();
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
				STextEnter();
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
