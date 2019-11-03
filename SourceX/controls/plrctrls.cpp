#include "controls/plrctrls.h"

#include <cstdint>

#include "controls/controller_motion.h"
#include "controls/game_controls.h"

// Based on the Nintendo Switch port by @lantus, @erfg12, @rsn8887.

namespace dvl {

bool sgbControllerActive = false;
coords speedspellscoords[50];
int speedspellcount = 0;

// Native game menu, controlled by simulating a keyboard.
bool InGameMenu()
{
	return stextflag > 0 || questlog || helpflag || talkflag || qtextflag || sgpCurrentMenu;
}

namespace {

DWORD invmove = 0;
int hsr[3] = { 0, 0, 0 }; // hot spell row counts
int slot = SLOTXY_INV_FIRST;
int spbslot = 0;

// Menu controlled by simulating a mouse.
bool InControlledMenu()
{
	return invflag || spselflag || chrflag;
}

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
	if (sgbControllerActive)
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
	if (sgbControllerActive)
		pcursobj = -1;
}

void checkTownersNearby(bool interact)
{
	if (pcursitem != -1)
		// Items take priority over towners because the player can move
		// items but not towners.
		return;
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
	// The first MAX_PLRS monsters are reserved for players' golems.
	for (int i = MAX_PLRS; i < MAXMONSTERS; i++) {
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
		static DWORD attacktick = 0;
		DWORD ticks = GetTickCount();
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
	if (pcurs >= CURSOR_FIRSTITEM) // drop item to allow us to pick up other items
		DropItemBeforeTrig();
	SetCursorPos(320, 180);
	MouseX = 320;
	MouseY = 180;
	if (pcurs == CURSOR_REPAIR || pcurs == CURSOR_RECHARGE)
		SetCursor_(CURSOR_HAND);
	sgbControllerActive = true;
}

void attrIncBtnSnap(int key)
{
	if (invflag || spselflag || !chrflag)
		return;

	if (chrbtnactive && !plr[myplr]._pStatPts)
		return;

	DWORD ticks = GetTickCount();
	if (ticks - invmove < 80) {
		return;
	}
	invmove = ticks;

	// first, find our cursor location
	int slot = 0;
	for (int i = 0; i < 4; i++) {
		if (MouseX >= ChrBtnsRect[i].x
		    && MouseX <= ChrBtnsRect[i].x + ChrBtnsRect[i].w
		    && MouseY >= ChrBtnsRect[i].y
		    && MouseY <= ChrBtnsRect[i].h + ChrBtnsRect[i].y) {
			slot = i;
			break;
		}
	}

	// set future location up or down
	if (key == DVL_VK_UP) {
		if (slot > 0)
			--slot;
	} else if (key == DVL_VK_DOWN) {
		if (slot < 3)
			++slot;
	}

	// move cursor to our new location
	int x = ChrBtnsRect[slot].x + (ChrBtnsRect[slot].w / 2);
	int y = ChrBtnsRect[slot].y + (ChrBtnsRect[slot].h / 2);
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

	DWORD ticks = GetTickCount();
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
			SetCursorPos(x - 10, y - 10);
		}
	} else {
		SetCursorPos(x, y);
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
	if (!spselflag)
		return;
	int x = 0;
	int y = 0;

	if (!sgbControllerActive)
		HideCursor();

	DWORD ticks = GetTickCount();
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

	if (key == DVL_VK_UP) {
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
	} else if (key == DVL_VK_DOWN) {
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
	} else if (key == DVL_VK_LEFT) {
		if (spbslot >= speedspellcount - 1)
			return;
		spbslot++;
		x = speedspellscoords[spbslot].x;
		y = speedspellscoords[spbslot].y;
	} else if (key == DVL_VK_RIGHT) {
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

void walkInDir(MoveDirection dir)
{
	if (dir.x == MoveDirectionX::NONE && dir.y == MoveDirectionY::NONE)
		return;
	DWORD ticks = GetTickCount();
	if (ticks - invmove < 370) {
		return;
	}
	invmove = ticks;
	ClrPlrPath(myplr);                   // clear nodes
	plr[myplr].destAction = ACTION_NONE; // stop attacking, etc.
	HideCursor();
	static const _walk_path kMoveToWalkDir[3][3] = {
		// NONE      UP      DOWN
		{ WALK_NONE, WALK_N, WALK_S }, // NONE
		{ WALK_W, WALK_NW, WALK_SW },  // LEFT
		{ WALK_E, WALK_NE, WALK_SE },  // RIGHT
	};
	plr[myplr].walkpath[0] = kMoveToWalkDir[static_cast<std::size_t>(dir.x)][static_cast<std::size_t>(dir.y)];
}

void menuMoveX(MoveDirectionX dir)
{
	if (dir == MoveDirectionX::NONE)
		return;
	invMove(dir == MoveDirectionX::LEFT ? WALK_W : WALK_E);
	hotSpellMove(dir == MoveDirectionX::LEFT ? DVL_VK_LEFT : DVL_VK_RIGHT);
}

void menuMoveY(MoveDirectionY dir)
{
	if (dir == MoveDirectionY::NONE)
		return;
	invMove(dir == MoveDirectionY::UP ? WALK_N : WALK_S);
	const auto key = dir == MoveDirectionY::UP ? DVL_VK_UP : DVL_VK_DOWN;
	hotSpellMove(key);
	attrIncBtnSnap(key);
}

void movement()
{
	if (InGameMenu())
		return;

	MoveDirection move_dir = GetMoveDirection();
	if (InControlledMenu()) {
		menuMoveX(move_dir.x);
		menuMoveY(move_dir.y);
	} else {
		walkInDir(move_dir);
	}

	if (GetAsyncKeyState(DVL_MK_LBUTTON) & 0x8000) {
		if (sgbControllerActive) { // show cursor first, before clicking
			sgbControllerActive = false;
		} else if (spselflag) {
			SetSpell();
		} else {
			LeftMouseCmd(false);
		}
	}
}

// Move map or mouse no more than 60 times per second.
// This is called from both the game loop and the event loop for responsiveness.
void HandleRightStickMotionAt60Fps()
{
	static std::int64_t currentTime = 0;
	static std::int64_t lastTime = 0;
	currentTime = SDL_GetTicks();

	if (currentTime - lastTime > 15) {
		HandleRightStickMotion();
		lastTime = currentTime;
	}
}

struct RightStickAccumulator {
	void start(int *x, int *y)
	{
		hiresDX += rightStickX * kGranularity;
		hiresDY += rightStickY * kGranularity;
		*x += hiresDX / slowdown;
		*y += -hiresDY / slowdown;
	}

	void finish()
	{
		// keep track of remainder for sub-pixel motion
		hiresDX %= slowdown;
		hiresDY %= slowdown;
	}

	static const int kGranularity = (1 << 15) - 1;
	int slowdown; // < kGranularity
	int hiresDX;
	int hiresDY;
};

} // namespace

void HandleRightStickMotion()
{
	// deadzone is handled in ScaleJoystickAxes() already
	if (rightStickX == 0 && rightStickY == 0)
		return;

	if (automapflag) { // move map
		static RightStickAccumulator acc = { /*slowdown=*/(1 << 14) + (1 << 13), 0, 0 };
		int dx = 0, dy = 0;
		acc.start(&dx, &dy);
		if (dy > 1)
			AutomapUp();
		else if (dy < -1)
			AutomapDown();
		else if (dx < -1)
			AutomapRight();
		else if (dx > 1)
			AutomapLeft();
		acc.finish();
	} else { // move cursor
		if (sgbControllerActive) {
			sgbControllerActive = false;
		}
		static RightStickAccumulator acc = { /*slowdown=*/(1 << 13) + (1 << 12), 0, 0 };
		int x = MouseX;
		int y = MouseY;
		acc.start(&x, &y);
		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;
		SetCursorPos(x, y);
		acc.finish();
	}
}

void plrctrls_after_check_curs_move()
{
	HandleRightStickMotionAt60Fps();

	// check for monsters first, then items, then towners.
	if (sgbControllerActive) { // cursor should be missing
		if (!checkMonstersNearby(false)) {
			pcursmonst = -1;
			checkItemsNearby(false);
			checkTownersNearby(false);
		} else {
			pcursitem = -1;
		}
	}
}

void plrctrls_after_game_logic()
{
	movement();
}

void useBeltPotion(bool mana)
{
	for (int i = 0; i < MAXBELTITEMS; i++) {
		const auto id = AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId;
		const auto spellId = AllItemsList[plr[myplr].SpdList[i].IDidx].iSpell;
		if ((!mana && (id == IMISC_HEAL || id == IMISC_FULLHEAL || (id == IMISC_SCROLL && spellId == SPL_HEAL)))
		    || (mana && (id == IMISC_MANA || id == IMISC_FULLMANA))
		    || id == IMISC_REJUV || id == IMISC_FULLREJUV) {
			if (plr[myplr].SpdList[i]._itype > -1) {
				UseInvItem(myplr, INVITEM_BELT_FIRST + i);
				break;
			}
		}
	}
}

void performPrimaryAction()
{
	const DWORD ticks = GetTickCount();
	if (invflag) { // inventory is open
		static DWORD clickinvtimer;
		if (ticks - clickinvtimer >= 300) {
			clickinvtimer = ticks;
			if (pcurs == CURSOR_IDENTIFY)
				CheckIdentify(myplr, pcursinvitem);
			else if (pcurs == CURSOR_REPAIR)
				DoRepair(myplr, pcursinvitem);
			else if (pcurs == CURSOR_RECHARGE)
				DoRecharge(myplr, pcursinvitem);
			else
				CheckInvItem();
		}
	} else if (spselflag) {
		SetSpell();
	} else if (chrflag) {
		static DWORD statuptimer;
		if (ticks - statuptimer >= 400) {
			statuptimer = ticks;
			if (!chrbtnactive && plr[myplr]._pStatPts) {
				CheckChrBtns();
				for (int i = 0; i < 4; i++) {
					if (MouseX >= ChrBtnsRect[i].x
					    && MouseX <= ChrBtnsRect[i].x + ChrBtnsRect[i].w
					    && MouseY >= ChrBtnsRect[i].y
					    && MouseY <= ChrBtnsRect[i].h + ChrBtnsRect[i].y) {
						chrbtn[i] = 1;
						chrbtnactive = true;
						ReleaseChrBtns();
					}
				}
			}
			if (plr[myplr]._pStatPts == 0)
				HideCursor();
		}
	} else {
		static DWORD talkwait;
		if (stextflag)
			talkwait = GetTickCount(); // Wait before we re-initiate talking
		HideCursor();
		DWORD talktick = GetTickCount();
		if (!checkMonstersNearby(true)) {
			if (talktick - talkwait > 600) { // prevent re-entering talk after finished
				talkwait = talktick;
				checkTownersNearby(true);
			}
		}
	}
}

void performSecondaryAction()
{
	if (invflag)
		return;
	static DWORD opentimer = 0;
	HideCursor();
	const DWORD ticks = GetTickCount();
	if (ticks - opentimer > 500) {
		opentimer = ticks;
		checkItemsNearby(true);
	}
}

} // namespace dvl
