#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

extern float leftStickX;
extern float leftStickY;

#define INV_TOP 240;
#define INV_LEFT 350;
#define INV_HEIGHT 320;
#define DISPLAY_HEIGHT 360;
#define DISPLAY_WIDTH 640;

// JAKE: My functions for movement and interaction via keyboard/controller
bool newCurHidden = false;
int dh = DISPLAY_HEIGHT;
int dw = DISPLAY_WIDTH;
int inv_top = INV_TOP;
int inv_left = INV_LEFT;
int inv_height = INV_HEIGHT;
static DWORD attacktick;
static DWORD invmove;
DWORD ticks;
int slot = SLOTXY_INV_FIRST;
int spbslot = 0;
coords speedspellscoords[50];
int speedspellcount = 0;
int hsr[3] = { 0, 0, 0 }; // hot spell row counts
DWORD talkwait;
DWORD talktick;
DWORD castwait;
bool inmainmenu = false;

// 0 = not near, >0 = distance related player 1 coordinates
coords checkNearbyObjs(int x, int y, int diff)
{
	int diff_x = abs(plr[myplr]._px - x);
	int diff_y = abs(plr[myplr]._py - y);

	if (diff_x <= diff && diff_y <= diff) {
		coords cm = { diff_x, diff_y };
		//sprintf(tempstr, "N-DIFF X:%i Y:%i", diff_x, diff_y);
		//NetSendCmdString(1 << myplr, tempstr);
		return cm;
	}
	return { -1, -1 };
}

void checkItemsNearby(bool interact)
{
	for (int i = 0; i < MAXITEMS; i++) {
		if (checkNearbyObjs(item[i]._ix, item[i]._iy, 1).x != -1 && item[i]._iSelFlag > 0 && item[i]._itype > -1) {
			pcursitem = i;
			if (dItem[item[i]._ix][item[i]._iy] <= 0)
				continue;
			if (interact) {
				//sprintf(tempstr, "FOUND NEARBY ITEM AT X:%i Y:%i SEL:%i", item[i]._ix, item[i]._iy, item[i]._iSelFlag);
				//NetSendCmdString(1 << myplr, tempstr);
				LeftMouseCmd(false);
			}
			return; // item nearby, don't find objects
		}
	}
	if (newCurHidden)
		pcursitem = -1;
	//sprintf(tempstr, "SCANNING FOR OBJECTS");
	//NetSendCmdString(1 << myplr, tempstr);
	for (int i = 0; i < MAXOBJECTS; i++) {
		if (checkNearbyObjs(object[i]._ox, object[i]._oy, 1).x != -1 && object[i]._oSelFlag > 0 && object[i]._otype > -1 && currlevel) { // make sure we're in the dungeon to scan for objs
			pcursobj = i;
			if (interact) {
				LeftMouseCmd(false);
			}
			return;
		}
	}
	if (newCurHidden)
		pcursobj = -1;
}

void checkTownersNearby(bool interact)
{
	for (int i = 0; i < 16; i++) {
		if (checkNearbyObjs(towner[i]._tx, towner[i]._ty, 2).x != -1) {
			if (towner[i]._ttype == -1)
				continue;
			pcursmonst = i;
			if (interact) {
				TalkToTowner(myplr, i);
			}
			break;
		}
	}
}

bool checkMonstersNearby(bool attack)
{
	int closest = 0;                 // monster ID who is closest
	coords objDistLast = { 99, 99 }; // previous obj distance
	for (int i = 0; i < MAXMONSTERS; i++) {
		int d_monster = dMonster[monster[i]._mx][monster[i]._my];
		if (monster[i]._mFlags & MFLAG_HIDDEN || monster[i]._mhitpoints <= 0) // monster is hiding or dead, skip
			continue;
		if (d_monster && dFlags[monster[i]._mx][monster[i]._my] & BFLAG_LIT) {                                                                          // is monster visible
			if (monster[i].MData->mSelFlag & 1 || monster[i].MData->mSelFlag & 2 || monster[i].MData->mSelFlag & 3 || monster[i].MData->mSelFlag & 4) { // is monster selectable
				coords objDist = checkNearbyObjs(monster[i]._mx, monster[i]._my, 6);
				if (objDist.x > -1 && objDist.x <= objDistLast.x && objDist.y <= objDistLast.y) {
					closest = i;
					objDistLast = objDist;
				}
			}
		}
	}
	if (closest > 0) { // did we find a monster
		pcursmonst = closest;
		//sprintf(tempstr, "NEARBY MONSTER WITH HP:%i", monster[closest]._mhitpoints);
		//NetSendCmdString(1 << myplr, tempstr);
	} else if (closest > 0) { // found monster, but we don't want to attack it
		return true;
	} else {
		pcursmonst = -1;
		return false;
	}
	if (attack) {
		if (ticks - attacktick > 100) { // prevent accidental double attacks
			attacktick = ticks;
			LeftMouseCmd(false);
		}
		return true;
	} else {
		return true;
	}
	pcursmonst = -1;
	return false;
}

// hide the cursor when we start walking via keyboard/controller
void HideCursor()
{
	if (pcurs >= CURSOR_FIRSTITEM) // if we don't drop the item on cursor, it will be destroyed
		DropItemBeforeTrig();
	SetCursorPos(320, 180);
	MouseX = 320;
	MouseY = 180;
	SetCursor_(CURSOR_NONE);
	newCurHidden = true;
}

void attrIncBtnSnap(int key)
{
	if (invflag || spselflag || !chrflag)
		return;

	if (chrbtnactive && !plr[myplr]._pStatPts)
		return;

	if (ticks - invmove < 80) {
		return;
	}
	invmove = ticks;

	// first, find our cursor location
	int slot = 0;
	for (int i = 0; i < 4; i++) {
		// 0 = x, 1 = y, 2 = width, 3 = height
		if (MouseX >= attribute_inc_rects2[i][0]
		    && MouseX <= attribute_inc_rects2[i][0] + attribute_inc_rects2[i][2]
		    && MouseY >= attribute_inc_rects2[i][1]
		    && MouseY <= attribute_inc_rects2[i][3] + attribute_inc_rects2[i][1]) {
			slot = i;
			break;
		}
	}

	// set future location up or down
	if (key == VK_UP) {
		if (slot > 0)
			slot--;
	} else if (key == VK_DOWN) {
		if (slot < 3)
			slot++;
	}

	// move cursor to our new location
	int x = attribute_inc_rects2[slot][0] + (attribute_inc_rects2[slot][2] / 2);
	int y = attribute_inc_rects2[slot][1] + (attribute_inc_rects2[slot][3] / 2);
	SetCursorPos(x, y);
	MouseX = x;
	MouseY = y;
}

// move the cursor around in our inventory
// if mouse coords are at SLOTXY_CHEST_LAST, consider this center of equipment
// small inventory squares are 29x29 (roughly)
void invMove(int key)
{
	if (!invflag)
		return;
	if (ticks - invmove < 80) {
		return;
	}
	invmove = ticks;
	int x = MouseX;
	int y = MouseY;

	// check which inventory rectangle the mouse is in, if any
	for (int r = 0; (DWORD)r < NUM_XY_SLOTS; r++) {
		if (x >= InvRect[r].X && x < InvRect[r].X + (INV_SLOT_SIZE_PX + 1) && y >= InvRect[r].Y - (INV_SLOT_SIZE_PX + 1) && y < InvRect[r].Y) {
			slot = r;
			break;
		}
	}

	if (slot < 0)
		slot = 0;
	if (slot > SLOTXY_BELT_LAST)
		slot = SLOTXY_BELT_LAST;

	// when item is on cursor, this is the real cursor XY
	if (key == WALK_W) {
		if (slot >= SLOTXY_HAND_RIGHT_FIRST && slot <= SLOTXY_HAND_RIGHT_LAST) {
			x = InvRect[SLOTXY_CHEST_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_CHEST_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_CHEST_FIRST && slot <= SLOTXY_CHEST_LAST) {
			x = InvRect[SLOTXY_HAND_LEFT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_LEFT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_AMULET) {
			x = InvRect[SLOTXY_HEAD_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HEAD_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_RIGHT) {
			x = InvRect[SLOTXY_RING_LEFT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_LEFT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_BELT_FIRST) {
			// do nothing
		} else if (slot == SLOTXY_RING_LEFT) {                                        // left ring
			                                                                          // do nothing
		} else if (slot >= SLOTXY_HAND_LEFT_FIRST && slot <= SLOTXY_HAND_LEFT_LAST) { // left hand
			                                                                          // do nothing
		} else if (slot >= SLOTXY_HEAD_FIRST && slot <= SLOTXY_HEAD_LAST) {           // head
			                                                                          // do nothing
		} else if (slot > SLOTXY_INV_FIRST) {                                         // general inventory
			if (slot != SLOTXY_INV_FIRST && slot != 35 && slot != 45 && slot != 55) { // left bounds
				slot -= 1;
				x = InvRect[slot].X + (INV_SLOT_SIZE_PX / 2);
				y = InvRect[slot].Y - (INV_SLOT_SIZE_PX / 2);
			}
		}
	} else if (key == WALK_E) {
		if (slot == SLOTXY_RING_LEFT) {
			x = InvRect[SLOTXY_RING_RIGHT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_RIGHT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_LEFT_FIRST && slot <= SLOTXY_HAND_LEFT_LAST) {
			x = InvRect[SLOTXY_CHEST_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_CHEST_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_CHEST_FIRST && slot <= SLOTXY_CHEST_LAST) {
			x = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HEAD_FIRST && slot <= SLOTXY_HEAD_LAST) { // head to amulet
			x = InvRect[SLOTXY_AMULET].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_AMULET].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_RIGHT_FIRST && slot <= SLOTXY_HAND_RIGHT_LAST) { // right hand
			                                                                            // do nothing
		} else if (slot == SLOTXY_AMULET) {
			// do nothing
		} else if (slot == SLOTXY_RING_RIGHT) {
			// do nothing
		} else if (slot < SLOTXY_BELT_LAST && slot >= SLOTXY_INV_FIRST) {            // general inventory
			if (slot != 34 && slot != 44 && slot != 54 && slot != SLOTXY_INV_LAST) { // right bounds
				slot += 1;
				x = InvRect[slot].X + (INV_SLOT_SIZE_PX / 2);
				y = InvRect[slot].Y - (INV_SLOT_SIZE_PX / 2);
			}
		}
	} else if (key == WALK_N) {
		if (slot > 24 && slot <= 27) { // first 3 general slots
			x = InvRect[SLOTXY_RING_LEFT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_LEFT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= 28 && slot <= 32) { // middle 4 general slots
			x = InvRect[SLOTXY_CHEST_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_CHEST_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= 33 && slot < 35) { // last 3 general slots
			x = InvRect[SLOTXY_RING_RIGHT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_RIGHT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_CHEST_FIRST && slot <= SLOTXY_CHEST_LAST) { // chest to head
			x = InvRect[SLOTXY_HEAD_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HEAD_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_LEFT) { // left ring to left hand
			x = InvRect[SLOTXY_HAND_LEFT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_LEFT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_RIGHT) { // right ring to right hand
			x = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_RIGHT_FIRST && slot <= SLOTXY_HAND_RIGHT_LAST) { // right hand to amulet
			x = InvRect[SLOTXY_AMULET].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_AMULET].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HEAD_FIRST && slot <= SLOTXY_HEAD_LAST) {
			// do nothing
		} else if (slot >= SLOTXY_HAND_LEFT_FIRST && slot <= SLOTXY_HAND_LEFT_LAST) { // left hand to head
			x = InvRect[SLOTXY_HEAD_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HEAD_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_AMULET) {
			// do nothing
		} else if (slot >= (SLOTXY_INV_FIRST + 10)) { // general inventory
			slot -= 10;
			x = InvRect[slot].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[slot].Y - (INV_SLOT_SIZE_PX / 2);
		}
	} else if (key == WALK_S) {
		if (slot >= SLOTXY_HEAD_FIRST && slot <= SLOTXY_HEAD_LAST) {
			x = InvRect[SLOTXY_CHEST_FIRST].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_CHEST_FIRST].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_CHEST_FIRST && slot <= SLOTXY_CHEST_LAST) {
			x = InvRect[30].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[30].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_LEFT_FIRST && slot <= SLOTXY_HAND_LEFT_LAST) {
			x = InvRect[SLOTXY_RING_LEFT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_LEFT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_LEFT) {
			x = InvRect[26].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[26].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_RING_RIGHT) {
			x = InvRect[34].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[34].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot == SLOTXY_AMULET) {
			x = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_HAND_RIGHT_FIRST + 2].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot >= SLOTXY_HAND_RIGHT_FIRST && slot <= SLOTXY_HAND_RIGHT_LAST) {
			x = InvRect[SLOTXY_RING_RIGHT].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[SLOTXY_RING_RIGHT].Y - (INV_SLOT_SIZE_PX / 2);
		} else if (slot < (SLOTXY_BELT_LAST - 10)) { // general inventory
			slot += 10;
			x = InvRect[slot].X + (INV_SLOT_SIZE_PX / 2);
			y = InvRect[slot].Y - (INV_SLOT_SIZE_PX / 2);
		}
	}

	if (pcurs > 1) {     // [3] Keep item in the same slot, don't jump it up
		if (x != MouseX) // without this, the cursor keeps moving -10
			{
				SetCursorPos((x - 10), (y - 10));
				MouseX = x - 10;
				MouseY = y - 10;
			}
	} else {
		SetCursorPos(x, y);
		MouseX = x;
		MouseY = y;
	}
}
// check if hot spell at X Y exists
bool HSExists(int x, int y)
{
	for (int r = 0; r < speedspellcount; r++) { // speedbook cells are 56x56
		if (MouseX >= speedspellscoords[r].x - 28 && MouseX < speedspellscoords[r].x + (28) && MouseY >= speedspellscoords[r].y - (28) && MouseY < speedspellscoords[r].y + 28) {
			return true;
		}
	}
	return false;
}

void hotSpellMove(int key)
{
	int x = 0;
	int y = 0;
	if (!spselflag)
		return;

	if (pcurs > 0)
		HideCursor();

	if (ticks - invmove < 80) {
		return;
	}
	invmove = ticks;

	for (int r = 0; r < speedspellcount; r++) { // speedbook cells are 56x56
		// our 3 rows by y axis
		if (speedspellscoords[r].y == 307)
			hsr[0]++;
		if (speedspellscoords[r].y == 251)
			hsr[1]++;
		if (speedspellscoords[r].y == 195)
			hsr[2]++;
		if (MouseX >= speedspellscoords[r].x - 28 && MouseX < speedspellscoords[r].x + (28) && MouseY >= speedspellscoords[r].y - (28) && MouseY < speedspellscoords[r].y + 28) {
			spbslot = r;
			//sprintf(tempstr, "IN HOT SPELL CELL NUM:%i", r);
			//NetSendCmdString(1 << myplr, tempstr);
		}
	}

	if (key == VK_UP) {
		if (speedspellscoords[spbslot].y == 307 && hsr[1] > 0) { // we're in row 1, check if row 2 has spells
			if (HSExists(MouseX, 251)) {
				x = MouseX;
				y = 251;
			}
		} else if (speedspellscoords[spbslot].y == 251 && hsr[2] > 0) { // we're in row 2, check if row 3 has spells
			if (HSExists(MouseX, 195)) {
				x = MouseX;
				y = 195;
			}
		}
	} else if (key == VK_DOWN) {
		if (speedspellscoords[spbslot].y == 251) { // we're in row 2
			if (HSExists(MouseX, 307)) {
				x = MouseX;
				y = 307;
			}
		} else if (speedspellscoords[spbslot].y == 195) { // we're in row 3
			if (HSExists(MouseX, 251)) {
				x = MouseX;
				y = 251;
			}
		}
	} else if (key == VK_LEFT) {
		if (spbslot >= speedspellcount - 1)
			return;
		spbslot++;
		x = speedspellscoords[spbslot].x;
		y = speedspellscoords[spbslot].y;
	} else if (key == VK_RIGHT) {
		if (spbslot <= 0)
			return;
		spbslot--;
		x = speedspellscoords[spbslot].x;
		y = speedspellscoords[spbslot].y;
	}

	if (x > 0 && y > 0) {
		SetCursorPos(x, y);
		MouseX = x;
		MouseY = y;
	}
}
// walk in the direction specified

void walkInDir(int dir)
{
	if (invflag || spselflag || chrflag || questlog) // don't walk if inventory, speedbook or char info windows are open
		return;
	ticks = GetTickCount();
	if (ticks - invmove < 370) {
		return;
	}
	invmove = ticks;
	ClrPlrPath(myplr);                   // clear nodes
	plr[myplr].destAction = ACTION_NONE; // stop attacking, etc.
	HideCursor();
	plr[myplr].walkpath[0] = dir;
}
static DWORD menuopenslow;
void useBeltPotion(bool mana)
{
	int invNum = 0;
	if (ticks - menuopenslow < 300) {
		return;
	}
	menuopenslow = ticks;
	for (int i = 0; i < MAXBELTITEMS; i++) {
		if ((AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_HEAL && mana == false) || (AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_FULLHEAL && mana == false) || (AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_MANA && mana == true) || (AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_FULLMANA && mana == true) || (AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_REJUV && AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_FULLREJUV)) {
			if (plr[myplr].SpdList[i]._itype > -1) {
				invNum = i + INVITEM_BELT_FIRST;
				UseInvItem(myplr, invNum);
				break;
			}
		}
	}
}

void movements(int key) {
	if (key == VK_UP) {
		invMove(WALK_N);
		hotSpellMove(VK_UP);
		attrIncBtnSnap(VK_UP);
		walkInDir(WALK_N);
	} else if (key == VK_RIGHT) {
		invMove(WALK_E);
		hotSpellMove(VK_RIGHT);
		walkInDir(WALK_E);
	} else if (key == VK_DOWN) {
		invMove(WALK_S);
		hotSpellMove(VK_DOWN);
		attrIncBtnSnap(VK_DOWN);
		walkInDir(WALK_S);
	} else if (key == VK_LEFT) {
		invMove(WALK_W);
		hotSpellMove(VK_LEFT);
		walkInDir(WALK_W);
	}
}

void charMovement() {
	if (stextflag > 0 || questlog || helpflag || talkflag || qtextflag)
		return;

	if (!invflag && !spselflag && !chrflag) {
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(0x44) & 0x8000 && GetAsyncKeyState(0x53) & 0x8000 || leftStickY <= -0.5 && leftStickX >= 0.5) {
			walkInDir(WALK_SE);
		} else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(0x57) & 0x8000 && GetAsyncKeyState(0x44) & 0x8000 || leftStickY >= 0.5 && leftStickX >= 0.5) {
			walkInDir(WALK_NE);
		} else if (GetAsyncKeyState(VK_LEFT) & 0x8000 && GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(0x41) & 0x8000 && GetAsyncKeyState(0x53) & 0x8000 || leftStickY <= -0.5 && leftStickX <= -0.5) {
			walkInDir(WALK_SW);
		} else if (GetAsyncKeyState(VK_LEFT) & 0x8000 && GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(0x57) & 0x8000 && GetAsyncKeyState(0x41) & 0x8000 || leftStickY >= 0.40 && leftStickX <= -0.5) {
			walkInDir(WALK_NW);
		}
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(0x57) & 0x8000 || leftStickY >= 0.5) {
		movements(VK_UP);
	} else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState(0x44) & 0x8000 || leftStickX >= 0.5) {
		movements(VK_RIGHT);
	} else if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(0x53) & 0x8000 || leftStickY <= -0.5) {
		movements(VK_DOWN);
	} else if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(0x41) & 0x8000 || leftStickX <= -0.5) {
		movements(VK_LEFT);
	}

	if (GetAsyncKeyState(MK_LBUTTON) & 0x8000) {
		if (newCurHidden) { // show cursor first, before clicking
			SetCursor_(CURSOR_HAND);
			newCurHidden = false;
		} else if (spselflag) {
			SetSpell();
		} else {
			LeftMouseCmd(false);
		}
	}
}

void keyboardExpansion(int vikey)
{
	static DWORD opentimer;
	static DWORD clickinvtimer;
	static DWORD statuptimer;
	ticks = GetTickCount();

	if (stextflag > 0 || questlog || helpflag || talkflag || qtextflag)
		return;
	if (vikey == VK_SPACE) { // similar to X button on PS1 ccontroller. Talk to towners, click on inv items, attack.
		if (invflag) { // inventory is open
			if (ticks - clickinvtimer >= 300) {
				clickinvtimer = ticks;
				if (pcurs == CURSOR_IDENTIFY)
					CheckIdentify(myplr, pcursinvitem);
				else if (pcurs == CURSOR_REPAIR)
					DoRepair(myplr, pcursinvitem);
				else
					CheckInvItem();
			}
		} else if (spselflag) {
			SetSpell();
		} else if (chrflag) {
			if (ticks - statuptimer >= 400) {
				statuptimer = ticks;
				for (int i = 0; i < 4; i++) {
					if (MouseX >= attribute_inc_rects2[i][0]
					    && MouseX <= attribute_inc_rects2[i][0] + attribute_inc_rects2[i][2]
					    && MouseY >= attribute_inc_rects2[i][1]
					    && MouseY <= attribute_inc_rects2[i][3] + attribute_inc_rects2[i][1]) {
						chrbtn[i] = 1;
						chrbtnactive = TRUE;
						ReleaseChrBtns();
					}
				}
			}
		} else {
			HideCursor();
			talktick = GetTickCount(); // this is shared with STextESC, do NOT duplicate or use anywhere else
			if (!checkMonstersNearby(true)) {
				if (talktick - talkwait > 600) { // prevent re-entering talk after finished
					talkwait = talktick;
					checkTownersNearby(true);
				}
			}
		}
	} else if (vikey == VK_RETURN) { // similar to [] button on PS1 controller. Open chests, doors, pickup items
		if (!invflag) {
			HideCursor();
			if (ticks - opentimer > 500) {
				opentimer = ticks;
				checkItemsNearby(true);
			}
		}
	}
}

DEVILUTION_END_NAMESPACE
