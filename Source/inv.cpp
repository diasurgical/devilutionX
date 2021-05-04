/**
 * @file inv.cpp
 *
 * Implementation of player inventory.
 */
#include <utility>

#include "cursor.h"
#include "minitext.h"
#include "options.h"
#include "plrmsg.h"
#include "stores.h"
#include "towners.h"
#include "utils/language.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {
namespace {
std::optional<CelSprite> pInvCels;
} // namespace

bool invflag;
bool drawsbarflag;
int sgdwLastTime; // check name

/**
 * Maps from inventory slot to screen position. The inventory slots are
 * arranged as follows:
 *                          00 01
 *                          02 03   06
 *              07 08       19 20       13 14
 *              09 10       21 22       15 16
 *              11 12       23 24       17 18
 *                 04                   05
 *              25 26 27 28 29 30 31 32 33 34
 *              35 36 37 38 39 40 41 42 43 44
 *              45 46 47 48 49 50 51 52 53 54
 *              55 56 57 58 59 60 61 62 63 64
 * 65 66 67 68 69 70 71 72
 * @see graphics/inv/inventory.png
 */
const InvXY InvRect[] = {
	// clang-format off
	//  X,   Y
	{ 132,  31 }, // helmet
	{ 160,  31 }, // helmet
	{ 132,  59 }, // helmet
	{ 160,  59 }, // helmet
	{  45, 205 }, // left ring
	{ 247, 205 }, // right ring
	{ 204,  59 }, // amulet
	{  17, 104 }, // left hand
	{  46, 104 }, // left hand
	{  17, 132 }, // left hand
	{  46, 132 }, // left hand
	{  17, 160 }, // left hand
	{  46, 160 }, // left hand
	{ 247, 104 }, // right hand
	{ 276, 104 }, // right hand
	{ 247, 132 }, // right hand
	{ 276, 132 }, // right hand
	{ 247, 160 }, // right hand
	{ 276, 160 }, // right hand
	{ 132, 104 }, // chest
	{ 160, 104 }, // chest
	{ 132, 132 }, // chest
	{ 160, 132 }, // chest
	{ 132, 160 }, // chest
	{ 160, 160 }, // chest
	{  17, 250 }, // inv row 1
	{  46, 250 }, // inv row 1
	{  75, 250 }, // inv row 1
	{ 104, 250 }, // inv row 1
	{ 133, 250 }, // inv row 1
	{ 162, 250 }, // inv row 1
	{ 191, 250 }, // inv row 1
	{ 220, 250 }, // inv row 1
	{ 249, 250 }, // inv row 1
	{ 278, 250 }, // inv row 1
	{  17, 279 }, // inv row 2
	{  46, 279 }, // inv row 2
	{  75, 279 }, // inv row 2
	{ 104, 279 }, // inv row 2
	{ 133, 279 }, // inv row 2
	{ 162, 279 }, // inv row 2
	{ 191, 279 }, // inv row 2
	{ 220, 279 }, // inv row 2
	{ 249, 279 }, // inv row 2
	{ 278, 279 }, // inv row 2
	{  17, 308 }, // inv row 3
	{  46, 308 }, // inv row 3
	{  75, 308 }, // inv row 3
	{ 104, 308 }, // inv row 3
	{ 133, 308 }, // inv row 3
	{ 162, 308 }, // inv row 3
	{ 191, 308 }, // inv row 3
	{ 220, 308 }, // inv row 3
	{ 249, 308 }, // inv row 3
	{ 278, 308 }, // inv row 3
	{  17, 337 }, // inv row 4
	{  46, 337 }, // inv row 4
	{  75, 337 }, // inv row 4
	{ 104, 337 }, // inv row 4
	{ 133, 337 }, // inv row 4
	{ 162, 337 }, // inv row 4
	{ 191, 337 }, // inv row 4
	{ 220, 337 }, // inv row 4
	{ 249, 337 }, // inv row 4
	{ 278, 337 }, // inv row 4
	{ 205,  33 }, // belt
	{ 234,  33 }, // belt
	{ 263,  33 }, // belt
	{ 292,  33 }, // belt
	{ 321,  33 }, // belt
	{ 350,  33 }, // belt
	{ 379,  33 }, // belt
	{ 408,  33 }  // belt
	// clang-format on
};

/* data */
/** Specifies the starting inventory slots for placement of 2x2 items. */
int AP2x2Tbl[10] = { 8, 28, 6, 26, 4, 24, 2, 22, 0, 20 };

void FreeInvGFX()
{
	pInvCels = std::nullopt;
}

void InitInv()
{
	if (plr[myplr]._pClass == HeroClass::Warrior) {
		pInvCels = LoadCel("Data\\Inv\\Inv.CEL", SPANEL_WIDTH);
	} else if (plr[myplr]._pClass == HeroClass::Rogue) {
		pInvCels = LoadCel("Data\\Inv\\Inv_rog.CEL", SPANEL_WIDTH);
	} else if (plr[myplr]._pClass == HeroClass::Sorcerer) {
		pInvCels = LoadCel("Data\\Inv\\Inv_Sor.CEL", SPANEL_WIDTH);
	} else if (plr[myplr]._pClass == HeroClass::Monk) {
		if (!gbIsSpawn)
			pInvCels = LoadCel("Data\\Inv\\Inv_Sor.CEL", SPANEL_WIDTH);
		else
			pInvCels = LoadCel("Data\\Inv\\Inv.CEL", SPANEL_WIDTH);
	} else if (plr[myplr]._pClass == HeroClass::Bard) {
		pInvCels = LoadCel("Data\\Inv\\Inv_rog.CEL", SPANEL_WIDTH);
	} else if (plr[myplr]._pClass == HeroClass::Barbarian) {
		pInvCels = LoadCel("Data\\Inv\\Inv.CEL", SPANEL_WIDTH);
	}

	invflag = false;
	drawsbarflag = false;
}

static void InvDrawSlotBack(const CelOutputBuffer &out, int X, int Y, int W, int H)
{
	BYTE *dst;

	dst = out.at(X, Y);

	int wdt, hgt;
	BYTE pix;

	for (hgt = H; hgt; hgt--, dst -= out.pitch() + W) {
		for (wdt = W; wdt; wdt--) {
			pix = *dst;
			if (pix >= PAL16_BLUE) {
				if (pix <= PAL16_BLUE + 15)
					pix -= PAL16_BLUE - PAL16_BEIGE;
				else if (pix >= PAL16_GRAY)
					pix -= PAL16_GRAY - PAL16_BEIGE;
			}
			*dst++ = pix;
		}
	}
}

void DrawInv(const CelOutputBuffer &out)
{
	int frame, i, j, ii;
	CelDrawTo(out, RIGHT_PANEL_X, 351, *pInvCels, 1);

	InvXY slotSize[] = {
		{ 2, 2 }, //head
		{ 1, 1 }, //left ring
		{ 1, 1 }, //right ring
		{ 1, 1 }, //amulet
		{ 2, 3 }, //left hand
		{ 2, 3 }, //right hand
		{ 2, 3 }, // chest
	};

	InvXY slotPos[] = {
		{ 133, 59 },  //head
		{ 48, 205 },  //left ring
		{ 249, 205 }, //right ring
		{ 205, 60 },  //amulet
		{ 17, 160 },  //left hand
		{ 248, 160 }, //right hand
		{ 133, 160 }, // chest
	};

	for (int slot = INVLOC_HEAD; slot < NUM_INVLOC; slot++) {
		if (!plr[myplr].InvBody[slot].isEmpty()) {
			int screen_x = slotPos[slot].X;
			int screen_y = slotPos[slot].Y;
			InvDrawSlotBack(out, RIGHT_PANEL_X + screen_x, screen_y, slotSize[slot].X * INV_SLOT_SIZE_PX, slotSize[slot].Y * INV_SLOT_SIZE_PX);

			frame = plr[myplr].InvBody[slot]._iCurs + CURSOR_FIRSTITEM;

			int frameW;
			int frameH;
			std::tie(frameW, frameH) = GetInvItemSize(frame);

			// calc item offsets for weapons smaller than 2x3 slots
			if (slot == INVLOC_HAND_LEFT) {
				screen_x += frameW == INV_SLOT_SIZE_PX ? 14 : 0;
				screen_y += frameH == (3 * INV_SLOT_SIZE_PX) ? 0 : -14;
			} else if (slot == INVLOC_HAND_RIGHT) {
				screen_x += frameW == INV_SLOT_SIZE_PX ? 13 : 1;
				screen_y += frameH == 3 * INV_SLOT_SIZE_PX ? 0 : -14;
			}

			const auto &cel = GetInvItemSprite(frame);
			const int celFrame = GetInvItemFrame(frame);

			if (pcursinvitem == slot) {
				CelBlitOutlineTo(out, GetOutlineColor(plr[myplr].InvBody[slot], true), RIGHT_PANEL_X + screen_x, screen_y, cel, celFrame, false);
			}

			if (plr[myplr].InvBody[slot]._iStatFlag) {
				CelClippedDrawTo(out, RIGHT_PANEL_X + screen_x, screen_y, cel, celFrame);
			} else {
				CelDrawLightRedTo(out, RIGHT_PANEL_X + screen_x, screen_y, cel, celFrame, 1);
			}

			if (slot == INVLOC_HAND_LEFT) {
				if (plr[myplr].InvBody[slot]._iLoc == ILOC_TWOHAND) {
					if (plr[myplr]._pClass != HeroClass::Barbarian
					    || (plr[myplr].InvBody[slot]._itype != ITYPE_SWORD
					        && plr[myplr].InvBody[slot]._itype != ITYPE_MACE)) {
						InvDrawSlotBack(out, RIGHT_PANEL_X + slotPos[INVLOC_HAND_RIGHT].X, slotPos[INVLOC_HAND_RIGHT].Y, slotSize[INVLOC_HAND_RIGHT].X * INV_SLOT_SIZE_PX, slotSize[INVLOC_HAND_RIGHT].Y * INV_SLOT_SIZE_PX);
						light_table_index = 0;
						cel_transparency_active = true;

						const int dst_x = RIGHT_PANEL_X + slotPos[INVLOC_HAND_RIGHT].X + (frameW == INV_SLOT_SIZE_PX ? 13 : -1);
						const int dst_y = slotPos[INVLOC_HAND_RIGHT].Y;
						CelClippedBlitLightTransTo(out, dst_x, dst_y, cel, celFrame);

						cel_transparency_active = false;
					}
				}
			}
		}
	}

	for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
		if (plr[myplr].InvGrid[i] != 0) {
			InvDrawSlotBack(
			    out,
			    InvRect[i + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
			    InvRect[i + SLOTXY_INV_FIRST].Y - 1,
			    INV_SLOT_SIZE_PX,
			    INV_SLOT_SIZE_PX);
		}
	}

	for (j = 0; j < NUM_INV_GRID_ELEM; j++) {
		if (plr[myplr].InvGrid[j] > 0) { // first slot of an item
			ii = plr[myplr].InvGrid[j] - 1;
			frame = plr[myplr].InvList[ii]._iCurs + CURSOR_FIRSTITEM;

			const auto &cel = GetInvItemSprite(frame);
			const int celFrame = GetInvItemFrame(frame);

			if (pcursinvitem == ii + INVITEM_INV_FIRST) {
				CelBlitOutlineTo(
				    out,
				    GetOutlineColor(plr[myplr].InvList[ii], true),
				    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
				    InvRect[j + SLOTXY_INV_FIRST].Y - 1,
				    cel, celFrame, false);
			}

			if (plr[myplr].InvList[ii]._iStatFlag) {
				CelClippedDrawTo(
				    out,
				    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
				    InvRect[j + SLOTXY_INV_FIRST].Y - 1,
				    cel, celFrame);
			} else {
				CelDrawLightRedTo(
				    out,
				    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
				    InvRect[j + SLOTXY_INV_FIRST].Y - 1,
				    cel, celFrame, 1);
			}
		}
	}
}

void DrawInvBelt(const CelOutputBuffer &out)
{
	BYTE fi, ff;

	if (talkflag) {
		return;
	}

	DrawPanelBox(out, 205, 21, 232, 28, PANEL_X + 205, PANEL_Y + 5);

	for (int i = 0; i < MAXBELTITEMS; i++) {
		if (plr[myplr].SpdList[i].isEmpty()) {
			continue;
		}

		InvDrawSlotBack(out, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);
		int frame = plr[myplr].SpdList[i]._iCurs + CURSOR_FIRSTITEM;

		const auto &cel = GetInvItemSprite(frame);
		const int celFrame = GetInvItemFrame(frame);

		if (pcursinvitem == i + INVITEM_BELT_FIRST) {
			if (!sgbControllerActive || invflag) {
				CelBlitOutlineTo(out, GetOutlineColor(plr[myplr].SpdList[i], true), InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, cel, celFrame, false);
			}
		}

		if (plr[myplr].SpdList[i]._iStatFlag) {
			CelClippedDrawTo(out, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, cel, celFrame);
		} else {
			CelDrawLightRedTo(out, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, cel, celFrame, 1);
		}

		if (AllItemsList[plr[myplr].SpdList[i].IDidx].iUsable
		    && plr[myplr].SpdList[i]._iStatFlag
		    && plr[myplr].SpdList[i]._itype != ITYPE_GOLD) {
			fi = i + 49;
			ff = fontframe[gbFontTransTbl[fi]];
			PrintChar(out, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X + INV_SLOT_SIZE_PX - fontkern[ff], InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, ff, COL_WHITE);
		}
	}
}

/**
 * @brief Adds an item to a player's InvGrid array
 * @param playerNumber Player index
 * @param invGridIndex Item's position in InvGrid (this should be the item's topleft grid tile)
 * @param invListIndex The item's InvList index (it's expected this already has +1 added to it since InvGrid can't store a 0 index)
 * @param sizeX Horizontal size of item
 * @param sizeY Vertical size of item
 */
static void AddItemToInvGrid(int playerNumber, int invGridIndex, int invListIndex, int sizeX, int sizeY)
{
	const int pitch = 10;
	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			if (x == 0 && y == sizeY - 1)
				plr[playerNumber].InvGrid[invGridIndex + x] = invListIndex;
			else
				plr[playerNumber].InvGrid[invGridIndex + x] = -invListIndex;
		}
		invGridIndex += pitch;
	}
}

/**
 * @brief Gets the size, in inventory cells, of the given item.
 * @param item The item whose size is to be determined.
 * @return The size, in inventory cells, of the item.
 */
InvXY GetInventorySize(const ItemStruct &item)
{
	int itemSizeIndex = item._iCurs + CURSOR_FIRSTITEM;
	int w;
	int h;
	std::tie(w, h) = GetInvItemSize(itemSizeIndex);
	return { w / INV_SLOT_SIZE_PX, h / INV_SLOT_SIZE_PX };
}

/**
 * @brief Checks whether the given item can fit in a belt slot (i.e. the item's size in inventory cells is 1x1).
 * @param item The item to be checked.
 * @return 'True' in case the item can fit a belt slot and 'False' otherwise.
 */
bool FitsInBeltSlot(const ItemStruct &item)
{
	InvXY size = GetInventorySize(item);

	return size.X == 1 && size.Y == 1;
}

/**
 * @brief Checks whether the given item can be placed on the belt. Takes item size as well as characteristics into account. Items
 * that cannot be placed on the belt have to be placed in the inventory instead.
 * @param item The item to be checked.
 * @return 'True' in case the item can be placed on the belt and 'False' otherwise.
 */
bool CanBePlacedOnBelt(const ItemStruct &item)
{
	return FitsInBeltSlot(item)
	    && item._itype != ITYPE_GOLD
	    && item._iStatFlag
	    && AllItemsList[item.IDidx].iUsable;
}

/**
 * @brief Checks whether the given item can be placed on the specified player's belt. Returns 'True' when the item can be placed
 * on belt slots and the player has at least one empty slot in his belt.
 * If 'persistItem' is 'True', the item is also placed in the belt.
 * @param playerNumber The player number on whose belt will be checked.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the belt. The default is 'False'.
 * @return 'True' in case the item can be placed on the player's belt and 'False' otherwise.
 */
bool AutoPlaceItemInBelt(int playerNumber, const ItemStruct &item, bool persistItem)
{
	if (!CanBePlacedOnBelt(item)) {
		return false;
	}

	for (auto &beltItem : plr[playerNumber].SpdList) {
		if (beltItem.isEmpty()) {
			if (persistItem) {
				beltItem = item;
				CalcPlrScrolls(playerNumber);
				drawsbarflag = true;
			}

			return true;
		}
	}

	return false;
}

/**
 * @brief Checks whether the given item can be equipped. Since this overload doesn't take player information, it only considers
 * general aspects about the item, like if its requirements are met and if the item's target location is valid for the body.
 * @param item The item to check.
 * @return 'True' in case the item could be equipped in a player, and 'False' otherwise.
 */
bool CanEquip(const ItemStruct &item)
{
	return item.isEquipment()
	    && item._iStatFlag;
}

/**
 * @brief A specialized version of 'CanEquip(int, ItemStruct&, int)' that specifically checks whether the item can be equipped
 * in one/both of the player's hands.
 * @param playerNumber The player number whose inventory will be checked for compatibility with the item.
 * @param item The item to check.
 * @return 'True' if the player can currently equip the item in either one of his hands (i.e. the required hands are empty and
 * allow the item), and 'False' otherwise.
 */
bool CanWield(int playerNumber, const ItemStruct &item)
{
	if (!CanEquip(item) || (item._iLoc != ILOC_ONEHAND && item._iLoc != ILOC_TWOHAND))
		return false;

	PlayerStruct &player = plr[playerNumber];
	ItemStruct &leftHandItem = player.InvBody[INVLOC_HAND_LEFT];
	ItemStruct &rightHandItem = player.InvBody[INVLOC_HAND_RIGHT];

	if (leftHandItem.isEmpty() && rightHandItem.isEmpty()) {
		return true;
	}

	if (!leftHandItem.isEmpty() && !rightHandItem.isEmpty()) {
		return false;
	}

	ItemStruct &occupiedHand = !leftHandItem.isEmpty() ? leftHandItem : rightHandItem;

	// Barbarian can wield two handed swords and maces in one hand, so we allow equiping any sword/mace as long as his occupied
	// hand has a shield (i.e. no dual wielding allowed)
	if (player._pClass == HeroClass::Barbarian) {
		if (occupiedHand._itype == ITYPE_SHIELD && (item._itype == ITYPE_SWORD || item._itype == ITYPE_MACE))
			return true;
	}

	// Bard can dual wield swords and maces, so we allow equiping one-handed weapons in her free slot as long as her occupied
	// slot is another one-handed weapon.
	if (player._pClass == HeroClass::Bard) {
		bool occupiedHandIsOneHandedSwordOrMace = occupiedHand._iLoc == ILOC_ONEHAND
		    && (occupiedHand._itype == ITYPE_SWORD || occupiedHand._itype == ITYPE_MACE);

		bool weaponToEquipIsOneHandedSwordOrMace = item._iLoc == ILOC_ONEHAND
		    && (item._itype == ITYPE_SWORD || item._itype == ITYPE_MACE);

		if (occupiedHandIsOneHandedSwordOrMace && weaponToEquipIsOneHandedSwordOrMace) {
			return true;
		}
	}

	return item._iLoc == ILOC_ONEHAND
	    && occupiedHand._iLoc == ILOC_ONEHAND
	    && item._iClass != occupiedHand._iClass;
}

/**
 * @brief Checks whether the specified item can be equipped in the desired body location on the player.
 * @param playerNumber The player number whose inventory will be checked for compatibility with the item.
 * @param item The item to check.
 * @param bodyLocation The location in the inventory to be checked against.
 * @return 'True' if the player can currently equip the item in the specified body location (i.e. the body location is empty and
 * allows the item), and 'False' otherwise.
 */
bool CanEquip(int playerNumber, const ItemStruct &item, inv_body_loc bodyLocation)
{
	PlayerStruct &player = plr[playerNumber];
	if (!CanEquip(item) || player._pmode > PM_WALK3 || !player.InvBody[bodyLocation].isEmpty()) {
		return false;
	}

	switch (bodyLocation) {
	case INVLOC_AMULET:
		return item._iLoc == ILOC_AMULET;

	case INVLOC_CHEST:
		return item._iLoc == ILOC_ARMOR;

	case INVLOC_HAND_LEFT:
	case INVLOC_HAND_RIGHT:
		return CanWield(playerNumber, item);

	case INVLOC_HEAD:
		return item._iLoc == ILOC_HELM;

	case INVLOC_RING_LEFT:
	case INVLOC_RING_RIGHT:
		return item._iLoc == ILOC_RING;

	default:
		return false;
	}
}

/**
 * @brief Automatically attempts to equip the specified item in a specific location in the player's body.
 * @note On success, this will broadcast an equipment_change event to let other players know about the equipment change.
 * @param playerNumber The player number whose inventory will be checked for compatibility with the item.
 * @param item The item to equip.
 * @param bodyLocation The location in the inventory where the item should be equipped.
 * @param persistItem Indicates whether or not the item should be persisted in the player's body. Pass 'False' to check
 * whether the player can equip the item but you don't want the item to actually be equipped. 'True' by default.
 * @return 'True' if the item was equipped and 'False' otherwise.
 */
bool AutoEquip(int playerNumber, const ItemStruct &item, inv_body_loc bodyLocation, bool persistItem)
{
	if (!CanEquip(playerNumber, item, bodyLocation)) {
		return false;
	}

	if (persistItem) {
		plr[playerNumber].InvBody[bodyLocation] = item;

		if (sgOptions.Audio.bAutoEquipSound && playerNumber == myplr) {
			PlaySFX(ItemInvSnds[ItemCAnimTbl[item._iCurs]]);
		}

		NetSendCmdChItem(false, bodyLocation);
		CalcPlrInv(playerNumber, true);
	}

	return true;
}

/**
 * @brief Automatically attempts to equip the specified item in the most appropriate location in the player's body.
 * @note On success, this will broadcast an equipment_change event to let other players know about the equipment change.
 * @param playerNumber The player number whose inventory will be checked for compatibility with the item.
 * @param item The item to equip.
 * @param persistItem Indicates whether or not the item should be persisted in the player's body. Pass 'False' to check
 * whether the player can equip the item but you don't want the item to actually be equipped. 'True' by default.
 * @return 'True' if the item was equipped and 'False' otherwise.
 */
bool AutoEquip(int playerNumber, const ItemStruct &item, bool persistItem)
{
	if (!CanEquip(item)) {
		return false;
	}

	for (int bodyLocation = INVLOC_HEAD; bodyLocation < NUM_INVLOC; bodyLocation++) {
		if (AutoEquip(playerNumber, item, (inv_body_loc)bodyLocation, persistItem)) {
			return true;
		}
	}

	return false;
}

/**
 * @brief Checks whether or not auto-equipping behavior is enabled for the given player and item.
 * @param player The player to check.
 * @param item The item to check.
 * @return 'True' if auto-equipping behavior is enabled for the player and item and 'False' otherwise.
 */
bool AutoEquipEnabled(const PlayerStruct &player, const ItemStruct &item)
{
	if (item.isWeapon()) {
		// Monk can use unarmed attack as an encouraged option, thus we do not automatically equip weapons on him so as to not
		// annoy players who prefer that playstyle.
		return player._pClass != HeroClass::Monk && sgOptions.Gameplay.bAutoEquipWeapons;
	}

	if (item.isArmor()) {
		return sgOptions.Gameplay.bAutoEquipArmor;
	}

	if (item.isHelm()) {
		return sgOptions.Gameplay.bAutoEquipHelms;
	}

	if (item.isShield()) {
		return sgOptions.Gameplay.bAutoEquipShields;
	}

	if (item.isJewelry()) {
		return sgOptions.Gameplay.bAutoEquipJewelry;
	}

	return true;
}

/**
 * @brief Checks whether the given item can be placed on the specified player's inventory.
 * If 'persistItem' is 'True', the item is also placed in the inventory.
 * @param playerNumber The player number on whose inventory will be checked.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the inventory. The default is 'False'.
 * @return 'True' in case the item can be placed on the player's inventory and 'False' otherwise.
 */
bool AutoPlaceItemInInventory(int playerNumber, const ItemStruct &item, bool persistItem)
{
	InvXY itemSize = GetInventorySize(item);
	bool done = false;

	if (itemSize.Y == 1) {
		for (int i = 30; i <= 39 && !done; i++) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}
		for (int x = 9; x >= 0 && !done; x--) {
			for (int y = 2; y >= 0 && !done; y--) {
				done = AutoPlaceItemInInventorySlot(playerNumber, 10 * y + x, item, persistItem);
			}
		}
	}

	if (itemSize.X == 1 && itemSize.Y == 2) {
		for (int i = 29; i >= 20 && !done; i--) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}

		for (int i = 9; i >= 0 && !done; i--) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}

		for (int i = 19; i >= 10 && !done; i--) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}
	}

	if (itemSize.X == 1 && itemSize.Y == 3) {
		for (int i = 0; i < 20 && !done; i++) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}
	}

	if (itemSize.X == 2 && itemSize.Y == 2) {
		for (int i = 0; i < 10 && !done; i++) {
			done = AutoPlaceItemInInventorySlot(playerNumber, AP2x2Tbl[i], item, persistItem);
		}

		for (int i = 21; i < 29 && !done; i += 2) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}

		for (int i = 1; i < 9 && !done; i += 2) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}

		for (int i = 10; i < 19 && !done; i++) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}
	}

	if (itemSize.X == 2 && itemSize.Y == 3) {
		for (int i = 0; i < 9 && !done; i++) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}

		for (int i = 10; i < 19 && !done; i++) {
			done = AutoPlaceItemInInventorySlot(playerNumber, i, item, persistItem);
		}
	}

	return done;
}

/**
 * @brief Checks whether the given item can be placed on the specified player's inventory slot.
 * If 'persistItem' is 'True', the item is also placed in the inventory slot.
 * @param playerNumber The player number on whose inventory slot will be checked.
 * @param slotIndex The 0-based index of the slot to put the item on.
 * @param item The item to be checked.
 * @param persistItem Pass 'True' to actually place the item in the inventory slot. The default is 'False'.
 * @return 'True' in case the item can be placed on the specified player's inventory slot and 'False' otherwise.
 */
bool AutoPlaceItemInInventorySlot(int playerNumber, int slotIndex, const ItemStruct &item, bool persistItem)
{
	int i, j, xx, yy;
	bool done;

	done = true;
	yy = 10 * (slotIndex / 10);
	if (yy < 0) {
		yy = 0;
	}

	InvXY itemSize = GetInventorySize(item);
	for (j = 0; j < itemSize.Y && done; j++) {
		if (yy >= NUM_INV_GRID_ELEM) {
			done = false;
		}
		xx = slotIndex % 10;
		if (xx < 0) {
			xx = 0;
		}
		for (i = 0; i < itemSize.X && done; i++) {
			if (xx >= 10) {
				done = false;
			} else {
				done = plr[playerNumber].InvGrid[xx + yy] == 0;
			}
			xx++;
		}
		yy += 10;
	}
	if (done && persistItem) {
		plr[playerNumber].InvList[plr[playerNumber]._pNumInv] = plr[playerNumber].HoldItem;
		plr[playerNumber]._pNumInv++;

		AddItemToInvGrid(playerNumber, slotIndex, plr[playerNumber]._pNumInv, itemSize.X, itemSize.Y);
		CalcPlrScrolls(playerNumber);
	}
	return done;
}

bool GoldAutoPlace(int pnum)
{
	bool done = false;

	for (int i = 0; i < plr[pnum]._pNumInv && !done; i++) {
		if (plr[pnum].InvList[i]._itype != ITYPE_GOLD)
			continue;
		if (plr[pnum].InvList[i]._ivalue >= MaxGold)
			continue;

		plr[pnum].InvList[i]._ivalue += plr[pnum].HoldItem._ivalue;
		if (plr[pnum].InvList[i]._ivalue > MaxGold) {
			plr[pnum].HoldItem._ivalue = plr[pnum].InvList[i]._ivalue - MaxGold;
			SetPlrHandGoldCurs(&plr[pnum].HoldItem);
			plr[pnum].InvList[i]._ivalue = MaxGold;
			if (gbIsHellfire)
				GetPlrHandSeed(&plr[pnum].HoldItem);
		} else {
			plr[pnum].HoldItem._ivalue = 0;
			done = true;
		}

		SetPlrHandGoldCurs(&plr[pnum].InvList[i]);
		plr[pnum]._pGold = CalculateGold(pnum);
	}

	for (int i = 39; i >= 30 && !done; i--) {
		done = GoldAutoPlaceInInventorySlot(pnum, i);
	}
	for (int x = 9; x >= 0 && !done; x--) {
		for (int y = 2; y >= 0 && !done; y--) {
			done = GoldAutoPlaceInInventorySlot(pnum, 10 * y + x);
		}
	}

	return done;
}

bool GoldAutoPlaceInInventorySlot(int pnum, int slotIndex)
{
	bool done = false;

	int yy = 10 * (slotIndex / 10);
	int xx = slotIndex % 10;
	if (plr[pnum].InvGrid[xx + yy] == 0) {
		int ii = plr[pnum]._pNumInv;
		plr[pnum].InvList[ii] = plr[pnum].HoldItem;
		plr[pnum]._pNumInv = plr[pnum]._pNumInv + 1;
		plr[pnum].InvGrid[xx + yy] = plr[pnum]._pNumInv;
		GetPlrHandSeed(&plr[pnum].InvList[ii]);
		int gold = plr[pnum].HoldItem._ivalue;
		if (gold > MaxGold) {
			gold -= MaxGold;
			plr[pnum].HoldItem._ivalue = gold;
			GetPlrHandSeed(&plr[pnum].HoldItem);
			plr[pnum].InvList[ii]._ivalue = MaxGold;
		} else {
			plr[pnum].HoldItem._ivalue = 0;
			done = true;
			plr[pnum]._pGold = CalculateGold(pnum);
			NewCursor(CURSOR_HAND);
		}
	}

	return done;
}

bool WeaponAutoPlace(int pnum)
{
	if (plr[pnum]._pClass == HeroClass::Monk)
		return false;
	if (plr[pnum].HoldItem._iLoc != ILOC_TWOHAND
	    || (plr[pnum]._pClass == HeroClass::Barbarian && (plr[pnum].HoldItem._itype == ITYPE_SWORD || plr[pnum].HoldItem._itype == ITYPE_MACE))) {
		if (plr[pnum]._pClass != HeroClass::Bard) {
			if (!plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty() && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON)
				return false;
			if (!plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty() && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON)
				return false;
		}

		if (plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty()) {
			NetSendCmdChItem(true, INVLOC_HAND_LEFT);
			plr[pnum].InvBody[INVLOC_HAND_LEFT] = plr[pnum].HoldItem;
			return true;
		}
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty() && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iLoc != ILOC_TWOHAND) {
			NetSendCmdChItem(true, INVLOC_HAND_RIGHT);
			plr[pnum].InvBody[INVLOC_HAND_RIGHT] = plr[pnum].HoldItem;
			return true;
		}
	} else if (plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty() && plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty()) {
		NetSendCmdChItem(true, INVLOC_HAND_LEFT);
		plr[pnum].InvBody[INVLOC_HAND_LEFT] = plr[pnum].HoldItem;
		return true;
	}

	return false;
}

int SwapItem(ItemStruct *a, ItemStruct *b)
{
	ItemStruct h;

	h = *a;
	*a = *b;
	*b = h;

	return h._iCurs + CURSOR_FIRSTITEM;
}

void CheckInvPaste(int pnum, int mx, int my)
{
	int r, sx, sy;
	int i, j, xx, yy, ii;
	bool done, done2h;
	int il, cn, it, iv, ig, gt;
	ItemStruct tempitem;

	SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
	i = mx + (icursW / 2);
	j = my + (icursH / 2);
	sx = icursW28;
	sy = icursH28;
	done = false;
	for (r = 0; (DWORD)r < NUM_XY_SLOTS && !done; r++) {
		int xo = RIGHT_PANEL;
		int yo = 0;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = PANEL_LEFT;
			yo = PANEL_TOP;
		}

		if (i >= InvRect[r].X + xo && i < InvRect[r].X + xo + INV_SLOT_SIZE_PX) {
			if (j >= InvRect[r].Y + yo - INV_SLOT_SIZE_PX - 1 && j < InvRect[r].Y + yo) {
				done = true;
				r--;
			}
		}
		if (r == SLOTXY_CHEST_LAST) {
			if ((sx & 1) == 0)
				i -= 14;
			if ((sy & 1) == 0)
				j -= 14;
		}
		if (r == SLOTXY_INV_LAST && (sy & 1) == 0)
			j += 14;
	}
	if (!done)
		return;
	il = ILOC_UNEQUIPABLE;
	if (r >= SLOTXY_HEAD_FIRST && r <= SLOTXY_HEAD_LAST)
		il = ILOC_HELM;
	if (r >= SLOTXY_RING_LEFT && r <= SLOTXY_RING_RIGHT)
		il = ILOC_RING;
	if (r == SLOTXY_AMULET)
		il = ILOC_AMULET;
	if (r >= SLOTXY_HAND_LEFT_FIRST && r <= SLOTXY_HAND_RIGHT_LAST)
		il = ILOC_ONEHAND;
	if (r >= SLOTXY_CHEST_FIRST && r <= SLOTXY_CHEST_LAST)
		il = ILOC_ARMOR;
	if (r >= SLOTXY_BELT_FIRST && r <= SLOTXY_BELT_LAST)
		il = ILOC_BELT;
	done = false;
	if (plr[pnum].HoldItem._iLoc == il)
		done = true;
	if (il == ILOC_ONEHAND && plr[pnum].HoldItem._iLoc == ILOC_TWOHAND) {
		if (plr[pnum]._pClass == HeroClass::Barbarian
		    && (plr[pnum].HoldItem._itype == ITYPE_SWORD || plr[pnum].HoldItem._itype == ITYPE_MACE))
			il = ILOC_ONEHAND;
		else
			il = ILOC_TWOHAND;
		done = true;
	}
	if (plr[pnum].HoldItem._iLoc == ILOC_UNEQUIPABLE && il == ILOC_BELT) {
		if (sx == 1 && sy == 1) {
			done = true;
			if (!AllItemsList[plr[pnum].HoldItem.IDidx].iUsable)
				done = false;
			if (!plr[pnum].HoldItem._iStatFlag)
				done = false;
			if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
				done = false;
		}
	}

	if (il == ILOC_UNEQUIPABLE) {
		done = true;
		it = 0;
		ii = r - SLOTXY_INV_FIRST;
		if (plr[pnum].HoldItem._itype == ITYPE_GOLD) {
			yy = 10 * (ii / 10);
			xx = ii % 10;
			if (plr[pnum].InvGrid[xx + yy] != 0) {
				iv = plr[pnum].InvGrid[xx + yy];
				if (iv > 0) {
					if (plr[pnum].InvList[iv - 1]._itype != ITYPE_GOLD) {
						it = iv;
					}
				} else {
					it = -iv;
				}
			}
		} else {
			yy = 10 * ((ii / 10) - ((sy - 1) / 2));
			if (yy < 0)
				yy = 0;
			for (j = 0; j < sy && done; j++) {
				if (yy >= NUM_INV_GRID_ELEM)
					done = false;
				xx = (ii % 10) - ((sx - 1) / 2);
				if (xx < 0)
					xx = 0;
				for (i = 0; i < sx && done; i++) {
					if (xx >= 10) {
						done = false;
					} else {
						if (plr[pnum].InvGrid[xx + yy] != 0) {
							iv = plr[pnum].InvGrid[xx + yy];
							if (iv < 0)
								iv = -iv;
							if (it != 0) {
								if (it != iv)
									done = false;
							} else
								it = iv;
						}
					}
					xx++;
				}
				yy += 10;
			}
		}
	}

	if (!done)
		return;

	if (il != ILOC_UNEQUIPABLE && il != ILOC_BELT && !plr[pnum].HoldItem._iStatFlag) {
		done = false;
		plr[pnum].PlaySpeach(13);
	}

	if (!done)
		return;

	if (pnum == myplr)
		PlaySFX(ItemInvSnds[ItemCAnimTbl[plr[pnum].HoldItem._iCurs]]);

	cn = CURSOR_HAND;
	switch (il) {
	case ILOC_HELM:
		NetSendCmdChItem(false, INVLOC_HEAD);
		if (plr[pnum].InvBody[INVLOC_HEAD].isEmpty())
			plr[pnum].InvBody[INVLOC_HEAD] = plr[pnum].HoldItem;
		else
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HEAD], &plr[pnum].HoldItem);
		break;
	case ILOC_RING:
		if (r == SLOTXY_RING_LEFT) {
			NetSendCmdChItem(false, INVLOC_RING_LEFT);
			if (plr[pnum].InvBody[INVLOC_RING_LEFT].isEmpty())
				plr[pnum].InvBody[INVLOC_RING_LEFT] = plr[pnum].HoldItem;
			else
				cn = SwapItem(&plr[pnum].InvBody[INVLOC_RING_LEFT], &plr[pnum].HoldItem);
		} else {
			NetSendCmdChItem(false, INVLOC_RING_RIGHT);
			if (plr[pnum].InvBody[INVLOC_RING_RIGHT].isEmpty())
				plr[pnum].InvBody[INVLOC_RING_RIGHT] = plr[pnum].HoldItem;
			else
				cn = SwapItem(&plr[pnum].InvBody[INVLOC_RING_RIGHT], &plr[pnum].HoldItem);
		}
		break;
	case ILOC_AMULET:
		NetSendCmdChItem(false, INVLOC_AMULET);
		if (plr[pnum].InvBody[INVLOC_AMULET].isEmpty())
			plr[pnum].InvBody[INVLOC_AMULET] = plr[pnum].HoldItem;
		else
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_AMULET], &plr[pnum].HoldItem);
		break;
	case ILOC_ONEHAND:
		if (r <= SLOTXY_HAND_LEFT_LAST) {
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty()) {
				if ((plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty() || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass != plr[pnum].HoldItem._iClass)
				    || (plr[pnum]._pClass == HeroClass::Bard && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON && plr[pnum].HoldItem._iClass == ICLASS_WEAPON)) {
					NetSendCmdChItem(false, INVLOC_HAND_LEFT);
					plr[pnum].InvBody[INVLOC_HAND_LEFT] = plr[pnum].HoldItem;
				} else {
					NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
					cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
				}
				break;
			}
			if ((plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty() || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass != plr[pnum].HoldItem._iClass)
			    || (plr[pnum]._pClass == HeroClass::Bard && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON && plr[pnum].HoldItem._iClass == ICLASS_WEAPON)) {
				NetSendCmdChItem(false, INVLOC_HAND_LEFT);
				cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].HoldItem);
				break;
			}

			NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
			break;
		}
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty()) {
			if ((plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty() || plr[pnum].InvBody[INVLOC_HAND_LEFT]._iLoc != ILOC_TWOHAND)
			    || (plr[pnum]._pClass == HeroClass::Barbarian && (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE))) {
				if ((plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty() || plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass != plr[pnum].HoldItem._iClass)
				    || (plr[pnum]._pClass == HeroClass::Bard && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON && plr[pnum].HoldItem._iClass == ICLASS_WEAPON)) {
					NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
					plr[pnum].InvBody[INVLOC_HAND_RIGHT] = plr[pnum].HoldItem;
					break;
				}
				NetSendCmdChItem(false, INVLOC_HAND_LEFT);
				cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].HoldItem);
				break;
			}
			NetSendCmdDelItem(false, INVLOC_HAND_LEFT);
			NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
			SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].InvBody[INVLOC_HAND_LEFT]);
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
			break;
		}

		if ((!plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty() && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == plr[pnum].HoldItem._iClass)
		    && !(plr[pnum]._pClass == HeroClass::Bard && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON && plr[pnum].HoldItem._iClass == ICLASS_WEAPON)) {
			NetSendCmdChItem(false, INVLOC_HAND_LEFT);
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].HoldItem);
			break;
		}
		NetSendCmdChItem(false, INVLOC_HAND_RIGHT);
		cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
		break;
	case ILOC_TWOHAND:
		if (!plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty() && !plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty()) {
			tempitem = plr[pnum].HoldItem;
			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD)
				plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HAND_RIGHT];
			else
				plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HAND_LEFT];
			if (pnum == myplr)
				NewCursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			else
				SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			done2h = AutoPlaceItemInInventory(pnum, plr[pnum].HoldItem, true);
			plr[pnum].HoldItem = tempitem;
			if (pnum == myplr)
				NewCursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			else
				SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			if (!done2h)
				return;

			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD)
				plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			else
				plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
		}

		NetSendCmdDelItem(false, INVLOC_HAND_RIGHT);

		if (!plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty() || !plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty()) {
			NetSendCmdChItem(false, INVLOC_HAND_LEFT);
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty())
				SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].InvBody[INVLOC_HAND_RIGHT]);
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].HoldItem);
		} else {
			NetSendCmdChItem(false, INVLOC_HAND_LEFT);
			plr[pnum].InvBody[INVLOC_HAND_LEFT] = plr[pnum].HoldItem;
		}
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iSpell != SPL_NULL && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
			plr[pnum]._pRSpell = plr[pnum].InvBody[INVLOC_HAND_LEFT]._iSpell;
			plr[pnum]._pRSplType = RSPLTYPE_CHARGES;
			force_redraw = 255;
		}
		break;
	case ILOC_ARMOR:
		NetSendCmdChItem(false, INVLOC_CHEST);
		if (plr[pnum].InvBody[INVLOC_CHEST].isEmpty())
			plr[pnum].InvBody[INVLOC_CHEST] = plr[pnum].HoldItem;
		else
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_CHEST], &plr[pnum].HoldItem);
		break;
	case ILOC_UNEQUIPABLE:
		if (plr[pnum].HoldItem._itype == ITYPE_GOLD && it == 0) {
			ii = r - SLOTXY_INV_FIRST;
			yy = 10 * (ii / 10);
			xx = ii % 10;
			if (plr[pnum].InvGrid[yy + xx] > 0) {
				il = plr[pnum].InvGrid[yy + xx];
				il--;
				gt = plr[pnum].InvList[il]._ivalue;
				ig = plr[pnum].HoldItem._ivalue + gt;
				if (ig <= MaxGold) {
					plr[pnum].InvList[il]._ivalue = ig;
					plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
					SetPlrHandGoldCurs(&plr[pnum].InvList[il]);
				} else {
					ig = MaxGold - gt;
					plr[pnum]._pGold += ig;
					plr[pnum].HoldItem._ivalue -= ig;
					plr[pnum].InvList[il]._ivalue = MaxGold;
					plr[pnum].InvList[il]._iCurs = ICURS_GOLD_LARGE;
					// BUGFIX: incorrect values here are leftover from beta (fixed)
					cn = GetGoldCursor(plr[pnum].HoldItem._ivalue);
					cn += CURSOR_FIRSTITEM;
				}
			} else {
				il = plr[pnum]._pNumInv;
				plr[pnum].InvList[il] = plr[pnum].HoldItem;
				plr[pnum]._pNumInv++;
				plr[pnum].InvGrid[yy + xx] = plr[pnum]._pNumInv;
				plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
				SetPlrHandGoldCurs(&plr[pnum].InvList[il]);
			}
		} else {
			if (it == 0) {
				plr[pnum].InvList[plr[pnum]._pNumInv] = plr[pnum].HoldItem;
				plr[pnum]._pNumInv++;
				it = plr[pnum]._pNumInv;
			} else {
				il = it - 1;
				if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
					plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
				cn = SwapItem(&plr[pnum].InvList[il], &plr[pnum].HoldItem);
				if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
					plr[pnum]._pGold = CalculateGold(pnum);
				for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
					if (plr[pnum].InvGrid[i] == it)
						plr[pnum].InvGrid[i] = 0;
					if (plr[pnum].InvGrid[i] == -it)
						plr[pnum].InvGrid[i] = 0;
				}
			}
			ii = r - SLOTXY_INV_FIRST;

			// Calculate top-left position of item for InvGrid and then add item to InvGrid

			yy = 10 * (ii / 10 - ((sy - 1) / 2));
			xx = (ii % 10 - ((sx - 1) / 2));
			if (yy < 0)
				yy = 0;
			if (xx < 0)
				xx = 0;
			AddItemToInvGrid(pnum, xx + yy, it, sx, sy);
		}
		break;
	case ILOC_BELT:
		ii = r - SLOTXY_BELT_FIRST;
		if (plr[pnum].HoldItem._itype == ITYPE_GOLD) {
			if (!plr[pnum].SpdList[ii].isEmpty()) {
				if (plr[pnum].SpdList[ii]._itype == ITYPE_GOLD) {
					i = plr[pnum].HoldItem._ivalue + plr[pnum].SpdList[ii]._ivalue;
					if (i <= MaxGold) {
						plr[pnum].SpdList[ii]._ivalue = i;
						plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
						SetPlrHandGoldCurs(&plr[pnum].SpdList[ii]);
					} else {
						i = MaxGold - plr[pnum].SpdList[ii]._ivalue;
						plr[pnum]._pGold += i;
						plr[pnum].HoldItem._ivalue -= i;
						plr[pnum].SpdList[ii]._ivalue = MaxGold;
						plr[pnum].SpdList[ii]._iCurs = ICURS_GOLD_LARGE;

						// BUGFIX: incorrect values here are leftover from beta (fixed)
						cn = GetGoldCursor(plr[pnum].HoldItem._ivalue);
						cn += CURSOR_FIRSTITEM;
					}
				} else {
					plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
					cn = SwapItem(&plr[pnum].SpdList[ii], &plr[pnum].HoldItem);
				}
			} else {
				plr[pnum].SpdList[ii] = plr[pnum].HoldItem;
				plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
			}
		} else if (plr[pnum].SpdList[ii].isEmpty()) {
			plr[pnum].SpdList[ii] = plr[pnum].HoldItem;
		} else {
			cn = SwapItem(&plr[pnum].SpdList[ii], &plr[pnum].HoldItem);
			if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
				plr[pnum]._pGold = CalculateGold(pnum);
		}
		drawsbarflag = true;
		break;
	}
	CalcPlrInv(pnum, true);
	if (pnum == myplr) {
		if (cn == CURSOR_HAND)
			SetCursorPos(MouseX + (cursW / 2), MouseY + (cursH / 2));
		NewCursor(cn);
	}
}

void CheckInvSwap(int pnum, BYTE bLoc, int idx, uint16_t wCI, int seed, bool bId, uint32_t dwBuff)
{
	PlayerStruct *p;

	memset(&items[MAXITEMS], 0, sizeof(*items));
	RecreateItem(MAXITEMS, idx, wCI, seed, 0, (dwBuff & CF_HELLFIRE) != 0);

	p = &plr[pnum];
	p->HoldItem = items[MAXITEMS];

	if (bId) {
		p->HoldItem._iIdentified = true;
	}

	if (bLoc < NUM_INVLOC) {
		p->InvBody[bLoc] = p->HoldItem;

		if (bLoc == INVLOC_HAND_LEFT && p->HoldItem._iLoc == ILOC_TWOHAND) {
			p->InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
		} else if (bLoc == INVLOC_HAND_RIGHT && p->HoldItem._iLoc == ILOC_TWOHAND) {
			p->InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
		}
	}

	CalcPlrInv(pnum, true);
}

void CheckInvCut(int pnum, int mx, int my, bool automaticMove)
{
	int r;
	bool done;
	char ii;
	int iv, ig;
	PlayerStruct &player = plr[pnum];

	if (player._pmode > PM_WALK3) {
		return;
	}

	if (dropGoldFlag) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	}

	done = false;

	for (r = 0; (DWORD)r < NUM_XY_SLOTS && !done; r++) {
		int xo = RIGHT_PANEL;
		int yo = 0;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = PANEL_LEFT;
			yo = PANEL_TOP;
		}

		// check which inventory rectangle the mouse is in, if any
		if (mx >= InvRect[r].X + xo
		    && mx < InvRect[r].X + xo + (INV_SLOT_SIZE_PX + 1)
		    && my >= InvRect[r].Y + yo - (INV_SLOT_SIZE_PX + 1)
		    && my < InvRect[r].Y + yo) {
			done = true;
			r--;
		}
	}

	if (!done) {
		// not on an inventory slot rectangle
		return;
	}

	ItemStruct &holdItem = player.HoldItem;
	holdItem._itype = ITYPE_NONE;

	bool automaticallyMoved = false;
	bool automaticallyEquipped = false;
	bool automaticallyUnequip = false;

	ItemStruct &headItem = player.InvBody[INVLOC_HEAD];
	if (r >= SLOTXY_HEAD_FIRST && r <= SLOTXY_HEAD_LAST && !headItem.isEmpty()) {
		holdItem = headItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(pnum, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_HEAD);
			headItem._itype = ITYPE_NONE;
		}
	}

	ItemStruct &leftRingItem = player.InvBody[INVLOC_RING_LEFT];
	if (r == SLOTXY_RING_LEFT && !leftRingItem.isEmpty()) {
		holdItem = leftRingItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(pnum, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_RING_LEFT);
			leftRingItem._itype = ITYPE_NONE;
		}
	}

	ItemStruct &rightRingItem = player.InvBody[INVLOC_RING_RIGHT];
	if (r == SLOTXY_RING_RIGHT && !rightRingItem.isEmpty()) {
		holdItem = rightRingItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(pnum, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_RING_RIGHT);
			rightRingItem._itype = ITYPE_NONE;
		}
	}

	ItemStruct &amuletItem = player.InvBody[INVLOC_AMULET];
	if (r == SLOTXY_AMULET && !amuletItem.isEmpty()) {
		holdItem = amuletItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(pnum, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_AMULET);
			amuletItem._itype = ITYPE_NONE;
		}
	}

	ItemStruct &leftHandItem = player.InvBody[INVLOC_HAND_LEFT];
	if (r >= SLOTXY_HAND_LEFT_FIRST && r <= SLOTXY_HAND_LEFT_LAST && !leftHandItem.isEmpty()) {
		holdItem = leftHandItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(pnum, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_HAND_LEFT);
			leftHandItem._itype = ITYPE_NONE;
		}
	}

	ItemStruct &rightHandItem = player.InvBody[INVLOC_HAND_RIGHT];
	if (r >= SLOTXY_HAND_RIGHT_FIRST && r <= SLOTXY_HAND_RIGHT_LAST && !rightHandItem.isEmpty()) {
		holdItem = rightHandItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(pnum, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_HAND_RIGHT);
			rightHandItem._itype = ITYPE_NONE;
		}
	}

	ItemStruct &chestItem = player.InvBody[INVLOC_CHEST];
	if (r >= SLOTXY_CHEST_FIRST && r <= SLOTXY_CHEST_LAST && !chestItem.isEmpty()) {
		holdItem = chestItem;
		if (automaticMove) {
			automaticallyUnequip = true;
			automaticallyMoved = automaticallyEquipped = AutoPlaceItemInInventory(pnum, holdItem, true);
		}

		if (!automaticMove || automaticallyMoved) {
			NetSendCmdDelItem(false, INVLOC_CHEST);
			chestItem._itype = ITYPE_NONE;
		}
	}

	if (r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST) {
		ig = r - SLOTXY_INV_FIRST;
		ii = player.InvGrid[ig];
		if (ii != 0) {
			iv = ii;
			if (ii <= 0) {
				iv = -ii;
			}

			holdItem = player.InvList[iv - 1];
			if (automaticMove) {
				if (CanBePlacedOnBelt(holdItem)) {
					automaticallyMoved = AutoPlaceItemInBelt(pnum, holdItem, true);
				} else {
					automaticallyMoved = automaticallyEquipped = AutoEquip(pnum, holdItem);
				}
			}

			if (!automaticMove || automaticallyMoved) {
				RemoveInvItem(pnum, iv - 1, false);
			}
		}
	}

	if (r >= SLOTXY_BELT_FIRST) {
		ItemStruct &beltItem = player.SpdList[r - SLOTXY_BELT_FIRST];
		if (!beltItem.isEmpty()) {
			holdItem = beltItem;
			if (automaticMove) {
				automaticallyMoved = AutoPlaceItemInInventory(pnum, holdItem, true);
			}

			if (!automaticMove || automaticallyMoved) {
				beltItem._itype = ITYPE_NONE;
				drawsbarflag = true;
			}
		}
	}

	if (!holdItem.isEmpty()) {
		if (holdItem._itype == ITYPE_GOLD) {
			player._pGold = CalculateGold(pnum);
		}

		CalcPlrInv(pnum, true);
		CheckItemStats(pnum);

		if (pnum == myplr) {
			if (automaticallyEquipped) {
				PlaySFX(ItemInvSnds[ItemCAnimTbl[holdItem._iCurs]]);
			} else if (!automaticMove || automaticallyMoved) {
				PlaySFX(IS_IGRAB);
			}

			if (automaticMove) {
				if (!automaticallyMoved) {
					if (CanBePlacedOnBelt(holdItem) || automaticallyUnequip) {
						player.PlaySpecificSpeach(15);
					} else {
						player.PlaySpecificSpeach(37);
					}
				}

				holdItem._itype = ITYPE_NONE;
			} else {
				NewCursor(holdItem._iCurs + CURSOR_FIRSTITEM);
				SetCursorPos(mx - (cursW / 2), MouseY - (cursH / 2));
			}
		}
	}
}

void inv_update_rem_item(int pnum, BYTE iv)
{
	if (iv < NUM_INVLOC) {
		plr[pnum].InvBody[iv]._itype = ITYPE_NONE;
	}

	if (plr[pnum]._pmode != PM_DEATH) {
		CalcPlrInv(pnum, true);
	} else {
		CalcPlrInv(pnum, false);
	}
}

void RemoveInvItem(int pnum, int iv, bool calcPlrScrolls)
{
	int i, j;

	iv++;

	//Iterate through invGrid and remove every reference to item
	for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
		if (plr[pnum].InvGrid[i] == iv || plr[pnum].InvGrid[i] == -iv) {
			plr[pnum].InvGrid[i] = 0;
		}
	}

	iv--;
	plr[pnum]._pNumInv--;

	//If the item at the end of inventory array isn't the one we removed, we need to swap its position in the array with the removed item
	if (plr[pnum]._pNumInv > 0 && plr[pnum]._pNumInv != iv) {
		plr[pnum].InvList[iv] = plr[pnum].InvList[plr[pnum]._pNumInv];

		for (j = 0; j < NUM_INV_GRID_ELEM; j++) {
			if (plr[pnum].InvGrid[j] == plr[pnum]._pNumInv + 1) {
				plr[pnum].InvGrid[j] = iv + 1;
			}
			if (plr[pnum].InvGrid[j] == -(plr[pnum]._pNumInv + 1)) {
				plr[pnum].InvGrid[j] = -(iv + 1);
			}
		}
	}

	if (calcPlrScrolls)
		CalcPlrScrolls(pnum);
}

void RemoveSpdBarItem(int pnum, int iv)
{
	plr[pnum].SpdList[iv]._itype = ITYPE_NONE;

	CalcPlrScrolls(pnum);
	force_redraw = 255;
}

void CheckInvItem(bool isShiftHeld)
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		CheckInvPaste(myplr, MouseX, MouseY);
	} else {
		CheckInvCut(myplr, MouseX, MouseY, isShiftHeld);
	}
}

/**
 * Check for interactions with belt
 */
void CheckInvScrn(bool isShiftHeld)
{
	if (MouseX > 190 + PANEL_LEFT && MouseX < 437 + PANEL_LEFT
	    && MouseY > PANEL_TOP && MouseY < 33 + PANEL_TOP) {
		CheckInvItem(isShiftHeld);
	}
}

void CheckItemStats(int pnum)
{
	PlayerStruct *p = &plr[pnum];

	p->HoldItem._iStatFlag = false;

	if (p->_pStrength >= p->HoldItem._iMinStr
	    && p->_pMagic >= p->HoldItem._iMinMag
	    && p->_pDexterity >= p->HoldItem._iMinDex) {
		p->HoldItem._iStatFlag = true;
	}
}

void CheckBookLevel(int pnum)
{
	int slvl;

	if (plr[pnum].HoldItem._iMiscId == IMISC_BOOK) {
		plr[pnum].HoldItem._iMinMag = spelldata[plr[pnum].HoldItem._iSpell].sMinInt;
		slvl = plr[pnum]._pSplLvl[plr[pnum].HoldItem._iSpell];
		while (slvl != 0) {
			plr[pnum].HoldItem._iMinMag += 20 * plr[pnum].HoldItem._iMinMag / 100;
			slvl--;
			if (plr[pnum].HoldItem._iMinMag + 20 * plr[pnum].HoldItem._iMinMag / 100 > 255) {
				plr[pnum].HoldItem._iMinMag = -1;
				slvl = 0;
			}
		}
	}
}

void CheckQuestItem(int pnum)
{
	if (plr[pnum].HoldItem.IDidx == IDI_OPTAMULET && quests[Q_BLIND]._qactive == QUEST_ACTIVE)
		quests[Q_BLIND]._qactive = QUEST_DONE;
	if (plr[pnum].HoldItem.IDidx == IDI_MUSHROOM && quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qvar1 == QS_MUSHSPAWNED) {
		plr[pnum].PlaySpeach(95, 10); // BUGFIX: Voice for this quest might be wrong in MP
		quests[Q_MUSHROOM]._qvar1 = QS_MUSHPICKED;
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ANVIL && quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
		if (quests[Q_ANVIL]._qactive == QUEST_INIT) {
			quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
			quests[Q_ANVIL]._qvar1 = 1;
		}
		if (quests[Q_ANVIL]._qlog) {
			plr[myplr].PlaySpeach(89, 10);
		}
	}
	if (plr[pnum].HoldItem.IDidx == IDI_GLDNELIX && quests[Q_VEIL]._qactive != QUEST_NOTAVAIL) {
		plr[myplr].PlaySpeach(88, 30);
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ROCK && quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
		if (quests[Q_ROCK]._qactive == QUEST_INIT) {
			quests[Q_ROCK]._qactive = QUEST_ACTIVE;
			quests[Q_ROCK]._qvar1 = 1;
		}
		if (quests[Q_ROCK]._qlog) {
			plr[myplr].PlaySpeach(87, 10);
		}
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ARMOFVAL && quests[Q_BLOOD]._qactive == QUEST_ACTIVE) {
		quests[Q_BLOOD]._qactive = QUEST_DONE;
		plr[myplr].PlaySpeach(91, 20);
	}
	if (plr[pnum].HoldItem.IDidx == IDI_MAPOFDOOM) {
		quests[Q_GRAVE]._qlog = false;
		quests[Q_GRAVE]._qactive = QUEST_ACTIVE;
		quests[Q_GRAVE]._qvar1 = 1;
		plr[myplr].PlaySpeach(79, 10);
	}
	if (plr[pnum].HoldItem.IDidx == IDI_NOTE1 || plr[pnum].HoldItem.IDidx == IDI_NOTE2 || plr[pnum].HoldItem.IDidx == IDI_NOTE3) {
		int mask, idx, item_num;
		int n1, n2, n3;
		ItemStruct tmp;
		mask = 0;
		idx = plr[pnum].HoldItem.IDidx;
		if (PlrHasItem(pnum, IDI_NOTE1, &n1) != nullptr || idx == IDI_NOTE1)
			mask = 1;
		if (PlrHasItem(pnum, IDI_NOTE2, &n2) != nullptr || idx == IDI_NOTE2)
			mask |= 2;
		if (PlrHasItem(pnum, IDI_NOTE3, &n3) != nullptr || idx == IDI_NOTE3)
			mask |= 4;
		if (mask == 7) {
			plr[myplr].PlaySpeach(46, 10);
			switch (idx) {
			case IDI_NOTE1:
				PlrHasItem(pnum, IDI_NOTE2, &n2);
				RemoveInvItem(pnum, n2);
				PlrHasItem(pnum, IDI_NOTE3, &n3);
				RemoveInvItem(pnum, n3);
				break;
			case IDI_NOTE2:
				PlrHasItem(pnum, IDI_NOTE1, &n1);
				RemoveInvItem(pnum, n1);
				PlrHasItem(pnum, IDI_NOTE3, &n3);
				RemoveInvItem(pnum, n3);
				break;
			case IDI_NOTE3:
				PlrHasItem(pnum, IDI_NOTE1, &n1);
				RemoveInvItem(pnum, n1);
				PlrHasItem(pnum, IDI_NOTE2, &n2);
				RemoveInvItem(pnum, n2);
				break;
			}
			item_num = itemactive[0];
			tmp = items[item_num];
			memset(&items[item_num], 0, sizeof(*items));
			GetItemAttrs(item_num, IDI_FULLNOTE, 16);
			SetupItem(item_num);
			plr[pnum].HoldItem = items[item_num];
			items[item_num] = tmp;
		}
	}
}

void CleanupItems(ItemStruct *item, int ii)
{
	dItem[item->position.x][item->position.y] = 0;

	if (currlevel == 21 && item->position == CornerStone.position) {
		CornerStone.item._itype = ITYPE_NONE;
		CornerStone.item._iSelFlag = 0;
		CornerStone.item.position = { 0, 0 };
		CornerStone.item._iAnimFlag = false;
		CornerStone.item._iIdentified = false;
		CornerStone.item._iPostDraw = false;
	}

	int i = 0;
	while (i < numitems) {
		if (itemactive[i] == ii) {
			DeleteItem(itemactive[i], i);
			i = 0;
			continue;
		}

		i++;
	}
}

void InvGetItem(int pnum, ItemStruct *item, int ii)
{
	if (dropGoldFlag) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	}

	if (dItem[item->position.x][item->position.y] == 0)
		return;

	if (myplr == pnum && pcurs >= CURSOR_FIRSTITEM)
		NetSendCmdPItem(true, CMD_SYNCPUTITEM, plr[myplr].position.tile);

	item->_iCreateInfo &= ~CF_PREGEN;
	plr[pnum].HoldItem = *item;
	CheckQuestItem(pnum);
	CheckBookLevel(pnum);
	CheckItemStats(pnum);
	bool cursor_updated = false;
	if (plr[pnum].HoldItem._itype == ITYPE_GOLD && GoldAutoPlace(pnum))
		cursor_updated = true;
	CleanupItems(item, ii);
	pcursitem = -1;
	if (!cursor_updated)
		NewCursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
}

void AutoGetItem(int pnum, ItemStruct *item, int ii)
{
	bool done;

	if (pcurs != CURSOR_HAND) {
		return;
	}

	if (dropGoldFlag) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	}

	if (dItem[item->position.x][item->position.y] == 0)
		return;

	item->_iCreateInfo &= ~CF_PREGEN;
	plr[pnum].HoldItem = *item; /// BUGFIX: overwrites cursor item, allowing for belt dupe bug
	CheckQuestItem(pnum);
	CheckBookLevel(pnum);
	CheckItemStats(pnum);
	SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
	if (plr[pnum].HoldItem._itype == ITYPE_GOLD) {
		done = GoldAutoPlace(pnum);
		if (!done) {
			item->_ivalue = plr[pnum].HoldItem._ivalue;
			SetPlrHandGoldCurs(item);
		}
	} else {
		done = AutoEquipEnabled(plr[pnum], plr[pnum].HoldItem) && AutoEquip(pnum, plr[pnum].HoldItem);
		if (!done) {
			done = AutoPlaceItemInBelt(pnum, plr[pnum].HoldItem, true);
		}
		if (!done) {
			done = AutoPlaceItemInInventory(pnum, plr[pnum].HoldItem, true);
		}
	}

	if (done) {
		CleanupItems(&items[ii], ii);
		return;
	}

	if (pnum == myplr) {
		plr[pnum].PlaySpeach(14);
	}
	plr[pnum].HoldItem = *item;
	RespawnItem(item, true);
	NetSendCmdPItem(true, CMD_RESPAWNITEM, item->position);
	plr[pnum].HoldItem._itype = ITYPE_NONE;
}

int FindGetItem(int idx, uint16_t ci, int iseed)
{
	if (numitems <= 0)
		return -1;

	int ii;
	int i = 0;
	while (true) {
		ii = itemactive[i];
		if (items[ii].IDidx == idx && items[ii]._iSeed == iseed && items[ii]._iCreateInfo == ci)
			break;

		i++;

		if (i >= numitems)
			return -1;
	}

	return ii;
}

void SyncGetItem(int x, int y, int idx, uint16_t ci, int iseed)
{
	int ii;

	if (dItem[x][y]) {
		ii = dItem[x][y] - 1;
		if (items[ii].IDidx == idx
		    && items[ii]._iSeed == iseed
		    && items[ii]._iCreateInfo == ci) {
			FindGetItem(idx, ci, iseed);
		} else {
			ii = FindGetItem(idx, ci, iseed);
		}
	} else {
		ii = FindGetItem(idx, ci, iseed);
	}

	if (ii == -1)
		return;

	CleanupItems(&items[ii], ii);
	assert(FindGetItem(idx, ci, iseed) == -1);
}

bool CanPut(int x, int y)
{
	char oi, oi2;

	if (dItem[x][y])
		return false;
	if (nSolidTable[dPiece[x][y]])
		return false;

	if (dObject[x][y] != 0) {
		if (object[dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1)]._oSolidFlag)
			return false;
	}

	oi = dObject[x + 1][y + 1];
	if (oi > 0 && object[oi - 1]._oSelFlag != 0) {
		return false;
	}
	if (oi < 0 && object[-(oi + 1)]._oSelFlag != 0) {
		return false;
	}

	oi = dObject[x + 1][y];
	if (oi > 0) {
		oi2 = dObject[x][y + 1];
		if (oi2 > 0 && object[oi - 1]._oSelFlag != 0 && object[oi2 - 1]._oSelFlag != 0)
			return false;
	}

	if (currlevel == 0 && dMonster[x][y] != 0)
		return false;
	if (currlevel == 0 && dMonster[x + 1][y + 1] != 0)
		return false;

	return true;
}

bool TryInvPut()
{
	if (numitems >= MAXITEMS)
		return false;

	direction dir = GetDirection(plr[myplr].position.tile, { cursmx, cursmy });
	if (CanPut(plr[myplr].position.tile.x + offset_x[dir], plr[myplr].position.tile.y + offset_y[dir])) {
		return true;
	}

	direction dirLeft = left[dir];
	if (CanPut(plr[myplr].position.tile.x + offset_x[dirLeft], plr[myplr].position.tile.y + offset_y[dirLeft])) {
		return true;
	}

	direction dirRight = right[dir];
	if (CanPut(plr[myplr].position.tile.x + offset_x[dirRight], plr[myplr].position.tile.y + offset_y[dirRight])) {
		return true;
	}

	return CanPut(plr[myplr].position.tile.x, plr[myplr].position.tile.y);
}

void DrawInvMsg(const char *msg)
{
	DWORD dwTicks;

	dwTicks = SDL_GetTicks();
	if (dwTicks - sgdwLastTime >= 5000) {
		sgdwLastTime = dwTicks;
		ErrorPlrMsg(msg);
	}
}

static bool PutItem(int pnum, Point &position)
{
	if (numitems >= MAXITEMS)
		return false;

	auto relativePosition = position - plr[pnum].position.tile;

	direction d = GetDirection(plr[pnum].position.tile, position);

	if (abs(relativePosition.x) > 1 || abs(relativePosition.y) > 1) {
		position.x = plr[pnum].position.tile.x + offset_x[d];
		position.y = plr[pnum].position.tile.y + offset_y[d];
	}
	if (CanPut(position.x, position.y))
		return true;

	direction dLeft = left[d];
	position.x = plr[pnum].position.tile.x + offset_x[dLeft];
	position.y = plr[pnum].position.tile.y + offset_y[dLeft];
	if (CanPut(position.x, position.y))
		return true;

	direction dRight = right[d];
	position.x = plr[pnum].position.tile.x + offset_x[dRight];
	position.y = plr[pnum].position.tile.y + offset_y[dRight];
	if (CanPut(position.x, position.y))
		return true;

	for (int l = 1; l < 50; l++) {
		for (int j = -l; j <= l; j++) {
			int yp = j + plr[pnum].position.tile.y;
			for (int i = -l; i <= l; i++) {
				int xp = i + plr[pnum].position.tile.x;
				if (!CanPut(xp, yp))
					continue;

				position = {xp,yp};
				return true;
			}
		}
	}

	return false;
}

int InvPutItem(int pnum, Point position)
{
	if (!PutItem(pnum, position))
		return -1;

	if (currlevel == 0) {
		int yp = cursmy;
		int xp = cursmx;
		if (plr[pnum].HoldItem._iCurs == ICURS_RUNE_BOMB && xp >= 79 && xp <= 82 && yp >= 61 && yp <= 64) {
			Point relativePosition = position - plr[pnum].position.tile;
			NetSendCmdLocParam2(false, CMD_OPENHIVE, plr[pnum].position.tile, relativePosition.x, relativePosition.y);
			quests[Q_FARMER]._qactive = QUEST_DONE;
			if (gbIsMultiplayer) {
				NetSendCmdQuest(true, Q_FARMER);
				return -1;
			}
			return -1;
		}
		if (plr[pnum].HoldItem.IDidx == IDI_MAPOFDOOM && xp >= 35 && xp <= 38 && yp >= 20 && yp <= 24) {
			NetSendCmd(false, CMD_OPENCRYPT);
			quests[Q_GRAVE]._qactive = QUEST_DONE;
			if (gbIsMultiplayer) {
				NetSendCmdQuest(true, Q_GRAVE);
			}
			return -1;
		}
	}

	assert(CanPut(position.x, position.y));

	int ii = AllocateItem();

	dItem[position.x][position.y] = ii + 1;
	items[ii] = plr[pnum].HoldItem;
	items[ii].position = position;
	RespawnItem(&items[ii], true);

	if (currlevel == 21 && position == CornerStone.position) {
		CornerStone.item = items[ii];
		InitQTextMsg(TEXT_CORNSTN);
		quests[Q_CORNSTN]._qlog = false;
		quests[Q_CORNSTN]._qactive = QUEST_DONE;
	}

	NewCursor(CURSOR_HAND);
	return ii;
}

int SyncPutItem(int pnum, Point position, int idx, uint16_t icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff, int to_hit, int max_dam, int min_str, int min_mag, int min_dex, int ac)
{
	if (!PutItem(pnum, position))
		return -1;

	assert(CanPut(position.x, position.y));

	int ii = AllocateItem();

	dItem[position.x][position.y] = ii + 1;

	if (idx == IDI_EAR) {
		RecreateEar(ii, icreateinfo, iseed, Id, dur, mdur, ch, mch, ivalue, ibuff);
	} else {
		RecreateItem(ii, idx, icreateinfo, iseed, ivalue, (ibuff & CF_HELLFIRE) != 0);
		if (Id)
			items[ii]._iIdentified = true;
		items[ii]._iDurability = dur;
		items[ii]._iMaxDur = mdur;
		items[ii]._iCharges = ch;
		items[ii]._iMaxCharges = mch;
		items[ii]._iPLToHit = to_hit;
		items[ii]._iMaxDam = max_dam;
		items[ii]._iMinStr = min_str;
		items[ii]._iMinMag = min_mag;
		items[ii]._iMinDex = min_dex;
		items[ii]._iAC = ac;
		items[ii].dwBuff = ibuff;
	}

	items[ii].position = position;
	RespawnItem(&items[ii], true);

	if (currlevel == 21 && position == CornerStone.position) {
		CornerStone.item = items[ii];
		InitQTextMsg(TEXT_CORNSTN);
		quests[Q_CORNSTN]._qlog = false;
		quests[Q_CORNSTN]._qactive = QUEST_DONE;
	}
	return ii;
}

char CheckInvHLight()
{
	int r, ii, nGold;
	ItemStruct *pi;
	PlayerStruct *p;

	for (r = 0; (DWORD)r < NUM_XY_SLOTS; r++) {
		int xo = RIGHT_PANEL;
		int yo = 0;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = PANEL_LEFT;
			yo = PANEL_TOP;
		}

		if (MouseX >= InvRect[r].X + xo
		    && MouseX < InvRect[r].X + xo + (INV_SLOT_SIZE_PX + 1)
		    && MouseY >= InvRect[r].Y + yo - (INV_SLOT_SIZE_PX + 1)
		    && MouseY < InvRect[r].Y + yo) {
			break;
		}
	}

	if ((DWORD)r >= NUM_XY_SLOTS)
		return -1;

	int8_t rv = -1;
	infoclr = COL_WHITE;
	pi = nullptr;
	p = &plr[myplr];
	ClearPanel();
	if (r >= SLOTXY_HEAD_FIRST && r <= SLOTXY_HEAD_LAST) {
		rv = INVLOC_HEAD;
		pi = &p->InvBody[rv];
	} else if (r == SLOTXY_RING_LEFT) {
		rv = INVLOC_RING_LEFT;
		pi = &p->InvBody[rv];
	} else if (r == SLOTXY_RING_RIGHT) {
		rv = INVLOC_RING_RIGHT;
		pi = &p->InvBody[rv];
	} else if (r == SLOTXY_AMULET) {
		rv = INVLOC_AMULET;
		pi = &p->InvBody[rv];
	} else if (r >= SLOTXY_HAND_LEFT_FIRST && r <= SLOTXY_HAND_LEFT_LAST) {
		rv = INVLOC_HAND_LEFT;
		pi = &p->InvBody[rv];
	} else if (r >= SLOTXY_HAND_RIGHT_FIRST && r <= SLOTXY_HAND_RIGHT_LAST) {
		pi = &p->InvBody[INVLOC_HAND_LEFT];
		if (pi->isEmpty() || pi->_iLoc != ILOC_TWOHAND
		    || (p->_pClass == HeroClass::Barbarian && (p->InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || p->InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE))) {
			rv = INVLOC_HAND_RIGHT;
			pi = &p->InvBody[rv];
		} else {
			rv = INVLOC_HAND_LEFT;
		}
	} else if (r >= SLOTXY_CHEST_FIRST && r <= SLOTXY_CHEST_LAST) {
		rv = INVLOC_CHEST;
		pi = &p->InvBody[rv];
	} else if (r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST) {
		r = abs(p->InvGrid[r - SLOTXY_INV_FIRST]);
		if (r == 0)
			return -1;
		ii = r - 1;
		rv = ii + INVITEM_INV_FIRST;
		pi = &p->InvList[ii];
	} else if (r >= SLOTXY_BELT_FIRST) {
		r -= SLOTXY_BELT_FIRST;
		drawsbarflag = true;
		pi = &p->SpdList[r];
		if (pi->isEmpty())
			return -1;
		rv = r + INVITEM_BELT_FIRST;
	}

	if (pi->isEmpty())
		return -1;

	if (pi->_itype == ITYPE_GOLD) {
		nGold = pi->_ivalue;
		sprintf(infostr, ngettext("%i gold piece", "%i gold pieces", nGold), nGold);
	} else {
		if (pi->_iMagical == ITEM_QUALITY_MAGIC) {
			infoclr = COL_BLUE;
		} else if (pi->_iMagical == ITEM_QUALITY_UNIQUE) {
			infoclr = COL_GOLD;
		}
		if (pi->_iIdentified) {
			strcpy(infostr, pi->_iIName);
			PrintItemDetails(pi);
		} else {
			strcpy(infostr, pi->_iName);
			PrintItemDur(pi);
		}
	}

	return rv;
}

void RemoveScroll(int pnum)
{
	int i;

	for (i = 0; i < plr[pnum]._pNumInv; i++) {
		if (!plr[pnum].InvList[i].isEmpty()
		    && (plr[pnum].InvList[i]._iMiscId == IMISC_SCROLL || plr[pnum].InvList[i]._iMiscId == IMISC_SCROLLT)
		    && plr[pnum].InvList[i]._iSpell == plr[pnum]._pRSpell) {
			RemoveInvItem(pnum, i);
			CalcPlrScrolls(pnum);
			return;
		}
	}
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (!plr[pnum].SpdList[i].isEmpty()
		    && (plr[pnum].SpdList[i]._iMiscId == IMISC_SCROLL || plr[pnum].SpdList[i]._iMiscId == IMISC_SCROLLT)
		    && plr[pnum].SpdList[i]._iSpell == plr[pnum]._pSpell) {
			RemoveSpdBarItem(pnum, i);
			CalcPlrScrolls(pnum);
			return;
		}
	}
}

bool UseScroll()
{
	int i;

	if (pcurs != CURSOR_HAND)
		return false;
	if (leveltype == DTYPE_TOWN && !spelldata[plr[myplr]._pRSpell].sTownSpell)
		return false;

	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (!plr[myplr].InvList[i].isEmpty()
		    && (plr[myplr].InvList[i]._iMiscId == IMISC_SCROLL || plr[myplr].InvList[i]._iMiscId == IMISC_SCROLLT)
		    && plr[myplr].InvList[i]._iSpell == plr[myplr]._pRSpell) {
			return true;
		}
	}
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (!plr[myplr].SpdList[i].isEmpty()
		    && (plr[myplr].SpdList[i]._iMiscId == IMISC_SCROLL || plr[myplr].SpdList[i]._iMiscId == IMISC_SCROLLT)
		    && plr[myplr].SpdList[i]._iSpell == plr[myplr]._pRSpell) {
			return true;
		}
	}

	return false;
}

void UseStaffCharge(int pnum)
{
	if (!plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty()
	    && (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_STAFF
	        || plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_UNIQUE // BUGFIX: myplr->pnum
	        )
	    && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iSpell == plr[pnum]._pRSpell
	    && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
		plr[pnum].InvBody[INVLOC_HAND_LEFT]._iCharges--;
		CalcPlrStaff(pnum);
	}
}

bool UseStaff()
{
	if (pcurs == CURSOR_HAND) {
		if (!plr[myplr].InvBody[INVLOC_HAND_LEFT].isEmpty()
		    && (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_STAFF || plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_UNIQUE)
		    && plr[myplr].InvBody[INVLOC_HAND_LEFT]._iSpell == plr[myplr]._pRSpell
		    && plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
			return true;
		}
	}

	return false;
}

void StartGoldDrop()
{
	initialDropGoldIndex = pcursinvitem;
	if (pcursinvitem <= INVITEM_INV_LAST)
		initialDropGoldValue = plr[myplr].InvList[pcursinvitem - INVITEM_INV_FIRST]._ivalue;
	else
		initialDropGoldValue = plr[myplr].SpdList[pcursinvitem - INVITEM_BELT_FIRST]._ivalue;
	dropGoldFlag = true;
	dropGoldValue = 0;
	if (talkflag)
		control_reset_talk();
}

bool UseInvItem(int pnum, int cii)
{
	int c, idata;
	ItemStruct *Item;
	bool speedlist;

	if (plr[pnum]._pInvincible && plr[pnum]._pHitPoints == 0 && pnum == myplr)
		return true;
	if (pcurs != CURSOR_HAND)
		return true;
	if (stextflag != STORE_NONE)
		return true;
	if (cii < INVITEM_INV_FIRST)
		return false;

	if (cii <= INVITEM_INV_LAST) {
		c = cii - INVITEM_INV_FIRST;
		Item = &plr[pnum].InvList[c];
		speedlist = false;
	} else {
		if (talkflag)
			return true;
		c = cii - INVITEM_BELT_FIRST;
		Item = &plr[pnum].SpdList[c];
		speedlist = true;
	}

	switch (Item->IDidx) {
	case IDI_MUSHROOM:
		plr[pnum].PlaySpeach(95, 10);
		return true;
	case IDI_FUNGALTM:
		PlaySFX(IS_IBOOK);
		plr[pnum].PlaySpeach(29, 10);
		return true;
	}

	if (!AllItemsList[Item->IDidx].iUsable)
		return false;

	if (!Item->_iStatFlag) {
		plr[pnum].PlaySpeach(13);
		return true;
	}

	if (Item->_iMiscId == IMISC_NONE && Item->_itype == ITYPE_GOLD) {
		StartGoldDrop();
		return true;
	}

	if (dropGoldFlag) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	}

	if (Item->_iMiscId == IMISC_SCROLL && currlevel == 0 && !spelldata[Item->_iSpell].sTownSpell) {
		return true;
	}

	if (Item->_iMiscId == IMISC_SCROLLT && currlevel == 0 && !spelldata[Item->_iSpell].sTownSpell) {
		return true;
	}

	if (Item->_iMiscId > IMISC_RUNEFIRST && Item->_iMiscId < IMISC_RUNELAST && currlevel == 0) {
		return true;
	}

	idata = ItemCAnimTbl[Item->_iCurs];
	if (Item->_iMiscId == IMISC_BOOK)
		PlaySFX(IS_RBOOK);
	else if (pnum == myplr)
		PlaySFX(ItemInvSnds[idata]);

	UseItem(pnum, Item->_iMiscId, Item->_iSpell);

	if (speedlist) {
		if (plr[pnum].SpdList[c]._iMiscId == IMISC_NOTE) {
			InitQTextMsg(TEXT_BOOK9);
			invflag = false;
			return true;
		}
		RemoveSpdBarItem(pnum, c);
		return true;
	}
	if (plr[pnum].InvList[c]._iMiscId == IMISC_MAPOFDOOM)
		return true;
	if (plr[pnum].InvList[c]._iMiscId == IMISC_NOTE) {
		InitQTextMsg(TEXT_BOOK9);
		invflag = false;
		return true;
	}
	RemoveInvItem(pnum, c);

	return true;
}

void DoTelekinesis()
{
	if (pcursobj != -1)
		NetSendCmdParam1(true, CMD_OPOBJT, pcursobj);
	if (pcursitem != -1)
		NetSendCmdGItem(true, CMD_REQUESTAGITEM, myplr, myplr, pcursitem);
	if (pcursmonst != -1 && !M_Talker(pcursmonst) && monster[pcursmonst].mtalkmsg == 0)
		NetSendCmdParam1(true, CMD_KNOCKBACK, pcursmonst);
	NewCursor(CURSOR_HAND);
}

int CalculateGold(int pnum)
{
	int i, gold;

	gold = 0;
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (plr[pnum].SpdList[i]._itype == ITYPE_GOLD) {
			gold += plr[pnum].SpdList[i]._ivalue;
			force_redraw = 255;
		}
	}
	for (i = 0; i < plr[pnum]._pNumInv; i++) {
		if (plr[pnum].InvList[i]._itype == ITYPE_GOLD)
			gold += plr[pnum].InvList[i]._ivalue;
	}

	return gold;
}

bool DropItemBeforeTrig()
{
	if (TryInvPut()) {
		NetSendCmdPItem(true, CMD_PUTITEM, { cursmx, cursmy });
		NewCursor(CURSOR_HAND);
		return true;
	}

	return false;
}

} // namespace devilution
