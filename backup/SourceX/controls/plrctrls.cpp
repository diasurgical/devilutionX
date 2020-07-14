#include "controls/plrctrls.h"

#include <cstdint>
#include <algorithm>
#include <list>

#include "controls/controller_motion.h"
#include "controls/game_controls.h"

namespace dvl {

typedef struct bfsNode {
	int x;
	int y;
	int steps;
} bfsNode;

bool sgbControllerActive = false;
coords speedspellscoords[50];
const int repeatRate = 100;
int speedspellcount = 0;

// Native game menu, controlled by simulating a keyboard.
bool InGameMenu()
{
	return stextflag > 0
	    || questlog
	    || helpflag
	    || talkflag
	    || qtextflag
	    || gmenu_exception()
	    || PauseMode == 2
	    || plr[myplr]._pInvincible;
}

namespace {

DWORD invmove = 0;
int hsr[3] = { 0, 0, 0 }; // hot spell row counts
int slot = SLOTXY_INV_FIRST;
int spbslot = 0;

/**
 * Number of angles to turn to face the coordinate
 * @param x Tile coordinates
 * @param y Tile coordinates
 * @return -1 == down
 */
int GetRotaryDistance(int x, int y)
{
	int d, d1, d2;

	if (plr[myplr]._px == x && plr[myplr]._py == y)
		return -1;

	d1 = plr[myplr]._pdir;
	d2 = GetDirection(plr[myplr]._px, plr[myplr]._py, x, y);

	d = abs(d1 - d2);
	if (d > 4)
		return 4 - (d % 4);

	return d;
}

/**
 * @brief Get the best case walking steps to coordinates
 * @param dx Tile coordinates
 * @param dy Tile coordinates
 */
int GetMinDistance(int dx, int dy)
{
	return std::max(abs(plr[myplr]._px - dx), abs(plr[myplr]._py - dy));
}

/**
 * @brief Get walking steps to coordinate
 * @param dx Tile coordinates
 * @param dy Tile coordinates
 * @param maxDistance the max number of steps to search
 * @return number of steps, or 0 if not reachable
 */
int GetDistance(int dx, int dy, int maxDistance)
{
	if (GetMinDistance(dx, dy) > maxDistance) {
		return 0;
	}

	char walkpath[25];
	return FindPath(PosOkPlayer, myplr, plr[myplr]._px, plr[myplr]._py, dx, dy, walkpath);
}

/**
 * @brief Get distance to coordinate
 * @param dx Tile coordinates
 * @param dy Tile coordinates
 */
int GetDistanceRanged(int dx, int dy)
{
	int a = abs(plr[myplr]._px - dx);
	int b = abs(plr[myplr]._py - dy);

	return sqrt(a * a + b * b);
}

void FindItemOrObject()
{
	int mx = plr[myplr]._px;
	int my = plr[myplr]._py;
	int rotations = 5;

	// As the player can not stand on the edge fo the map this is safe from OOB
	for (int xx = -1; xx < 2; xx++) {
		for (int yy = -1; yy < 2; yy++) {
			if (dItem[mx + xx][my + yy] <= 0)
				continue;
			int i = dItem[mx + xx][my + yy] - 1;
			if (item[i]._itype == ITYPE_NONE
			    || item[i]._iSelFlag == 0)
				continue;
			int newRotations = GetRotaryDistance(mx + xx, my + yy);
			if (rotations < newRotations)
				continue;
			rotations = newRotations;
			pcursitem = i;
			cursmx = mx + xx;
			cursmy = my + yy;
		}
	}

	if (currlevel == DTYPE_TOWN || pcursitem != -1)
		return; // Don't look for objects in town

	for (int xx = -1; xx < 2; xx++) {
		for (int yy = -1; yy < 2; yy++) {
			if (xx == 0 && yy == 0)
				continue; // Ignore doorway so we don't get stuck behind barrels
			if (dObject[mx + xx][my + yy] == 0)
				continue;
			int o = dObject[mx + xx][my + yy] > 0 ? dObject[mx + xx][my + yy] - 1 : -(dObject[mx + xx][my + yy] + 1);
			if (object[o]._oSelFlag == 0)
				continue;
			int newRotations = GetRotaryDistance(mx + xx, my + yy);
			if (rotations < newRotations)
				continue;
			rotations = newRotations;
			pcursobj = o;
			cursmx = mx + xx;
			cursmy = my + yy;
		}
	}
}

void CheckTownersNearby()
{
	for (int i = 0; i < 16; i++) {
		if (GetDistance(towner[i]._tx, towner[i]._ty, 2) == 0)
			continue;
		pcursmonst = i;
	}
}

bool HasRangedSpell()
{
	int spl = plr[myplr]._pRSpell;

	return spl != SPL_INVALID
	    && spl != SPL_TOWN
	    && spl != SPL_TELEPORT
	    && spelldata[spl].sTargeted
	    && !spelldata[spl].sTownSpell;
}

bool CanTargetMonster(int mi)
{
	const auto &monst = monster[mi];

	if (monst._mFlags & (MFLAG_HIDDEN | MFLAG_GOLEM))
		return false;
	if (monst._mhitpoints >> 6 <= 0) // dead
		return false;

	const int mx = monst._mx;
	const int my = monst._my;
	if (!(dFlags[mx][my] & BFLAG_LIT)) // not visable
		return false;
	if (dMonster[mx][my] == 0)
		return false;

	return true;
}

void FindRangedTarget()
{
	int newRotations, newDdistance;
	int distance = 2 * (MAXDUNX + MAXDUNY);
	int rotations = 5;

	// The first MAX_PLRS monsters are reserved for players' golems.
	for (int i = MAX_PLRS; i < MAXMONSTERS; i++) {
		const auto &monst = monster[i];
		const int mx = monst._mx;
		const int my = monst._my;
		if (!CanTargetMonster(i))
			continue;

		newDdistance = GetDistanceRanged(mx, my);
		if (distance < newDdistance || (pcursmonst != -1 && CanTalkToMonst(i))) {
			continue;
		}
		if (distance == newDdistance) {
			newRotations = GetRotaryDistance(mx, my);
			if (rotations < newRotations)
				continue;
		}
		distance = newDdistance;
		rotations = newRotations;
		pcursmonst = i;
	}
}

void FindMeleeTarget()
{
	bool visited[MAXDUNX][MAXDUNY] = { 0 };
	int maxSteps = 25; // Max steps for FindPath is 25
	int rotations = 5;
	int x = plr[myplr]._px;
	int y = plr[myplr]._py;

	std::list<bfsNode> queue;

	visited[x][y] = true;
	queue.push_back({ x, y, 0 });

	while (!queue.empty()) {
		bfsNode node = queue.front();
		queue.pop_front();

		for (int i = 0; i < 8; i++) {
			const int dx = node.x + pathxdir[i];
			const int dy = node.y + pathydir[i];

			if (visited[dx][dy])
				continue; // already visisted

			if (node.steps >= maxSteps) {
				visited[dx][dy] = true;
				continue;
			}

			if (!PosOkPlayer(myplr, dx, dy)) {
				visited[dx][dy] = true;

				if (dMonster[dx][dy] != 0) {
					int mi = dMonster[dx][dy] > 0 ? dMonster[dx][dy] - 1 : -(dMonster[dx][dy] + 1);
					if (CanTargetMonster(mi)) {
						int newRotations = GetRotaryDistance(dx, dy);
						if (rotations < newRotations || (pcursmonst != -1 && CanTalkToMonst(i)))
							continue;
						rotations = newRotations;
						pcursmonst = mi;
						maxSteps = node.steps; // Monsters found, cap search to current steps
					}
				}

				continue;
			}

			PATHNODE pPath;
			pPath.x = node.x;
			pPath.y = node.y;

			if (path_solid_pieces(&pPath, dx, dy)) {
				queue.push_back({ dx, dy, node.steps + 1 });
				visited[dx][dy] = true;
			}
		}
	}
}

void CheckMonstersNearby()
{
	if (plr[myplr]._pwtype == WT_RANGED || HasRangedSpell()) {
		FindRangedTarget();
		return;
	}

	FindMeleeTarget();
}

void CheckPlayerNearby()
{
	int newRotations, newDdistance;
	int distance = 2 * (MAXDUNX + MAXDUNY);
	int rotations = 5;

	if (pcursmonst != -1)
		return;

	int spl = plr[myplr]._pRSpell;
	if (FriendlyMode && spl != SPL_RESURRECT && spl != SPL_HEALOTHER)
		return;

	for (int i = 0; i < MAX_PLRS; i++) {
		if (i == myplr)
			continue;
		const int mx = plr[i].WorldX;
		const int my = plr[i].WorldY;
		if (dPlayer[mx][my] == 0
		    || !(dFlags[mx][my] & BFLAG_LIT)
		    || (plr[i]._pHitPoints == 0 && spl != SPL_RESURRECT))
			continue;

		if (plr[myplr]._pwtype == WT_RANGED || HasRangedSpell() || spl == SPL_HEALOTHER)
			newDdistance = GetDistanceRanged(mx, my);
		else
			newDdistance = GetDistance(mx, my, distance);

		if (newDdistance == 0 || distance < newDdistance) {
			continue;
		}
		if (distance == newDdistance) {
			newRotations = GetRotaryDistance(mx, my);
			if (rotations < newRotations)
				continue;
		}
		distance = newDdistance;
		rotations = newRotations;
		pcursplr = i;
	}
}

void FindActor()
{
	if (leveltype != DTYPE_TOWN)
		CheckMonstersNearby();
	else
		CheckTownersNearby();

	if (gbMaxPlayers != 1)
		CheckPlayerNearby();
}

void Interact()
{
	if (leveltype == DTYPE_TOWN && pcursmonst != -1) {
		NetSendCmdLocParam1(true, CMD_TALKXY, towner[pcursmonst]._tx, towner[pcursmonst]._ty, pcursmonst);
	} else if (pcursmonst != -1) {
		if (plr[myplr]._pwtype != WT_RANGED || CanTalkToMonst(pcursmonst)) {
			NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
		} else {
			NetSendCmdParam1(true, CMD_RATTACKID, pcursmonst);
		}
	} else if (pcursplr != -1 && !FriendlyMode) {
		NetSendCmdParam1(true, plr[myplr]._pwtype == WT_RANGED ? CMD_RATTACKPID : CMD_ATTACKPID, pcursplr);
	}
}

void AttrIncBtnSnap(MoveDirectionY dir)
{
	if (dir == MoveDirectionY::NONE)
		return;

	if (chrbtnactive && plr[myplr]._pStatPts <= 0)
		return;

	DWORD ticks = GetTickCount();
	if (ticks - invmove < repeatRate) {
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

	if (dir == MoveDirectionY::UP) {
		if (slot > 0)
			--slot;
	} else if (dir == MoveDirectionY::DOWN) {
		if (slot < 3)
			++slot;
	}

	// move cursor to our new location
	int x = ChrBtnsRect[slot].x + (ChrBtnsRect[slot].w / 2);
	int y = ChrBtnsRect[slot].y + (ChrBtnsRect[slot].h / 2);
	SetCursorPos(x, y);
}

// move the cursor around in our inventory
// if mouse coords are at SLOTXY_CHEST_LAST, consider this center of equipment
// small inventory squares are 29x29 (roughly)
void InvMove(MoveDirection dir)
{
	DWORD ticks = GetTickCount();
	if (ticks - invmove < repeatRate) {
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
	if (dir.x == MoveDirectionX::LEFT) {
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
	} else if (dir.x == MoveDirectionX::RIGHT) {
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
	}
	if (dir.y == MoveDirectionY::UP) {
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
	} else if (dir.y == MoveDirectionY::DOWN) {
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

	if (x == MouseX && y == MouseY) {
		return; // Avoid wobeling when scalled
	}

	if (pcurs > 1) {       // [3] Keep item in the same slot, don't jump it up
		if (x != MouseX) { // without this, the cursor keeps moving -10
			x -= 10;
			y -= 10;
		}
	}
	SetCursorPos(x, y);
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

void HotSpellMove(MoveDirection dir)
{
	int x = 0;
	int y = 0;

	DWORD ticks = GetTickCount();
	if (ticks - invmove < repeatRate) {
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

	if (dir.y == MoveDirectionY::UP) {
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
	} else if (dir.y == MoveDirectionY::DOWN) {
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
	}
	if (dir.x == MoveDirectionX::LEFT) {
		if (spbslot >= speedspellcount - 1)
			return;
		spbslot++;
		x = speedspellscoords[spbslot].x;
		y = speedspellscoords[spbslot].y;
	} else if (dir.x == MoveDirectionX::RIGHT) {
		if (spbslot <= 0)
			return;
		spbslot--;
		x = speedspellscoords[spbslot].x;
		y = speedspellscoords[spbslot].y;
	}

	if (x > 0 && y > 0) {
		SetCursorPos(x, y);
	}
}

static const _walk_path kMoveToWalkDir[3][3] = {
	// NONE      UP      DOWN
	{ WALK_NONE, WALK_N, WALK_S }, // NONE
	{ WALK_W, WALK_NW, WALK_SW },  // LEFT
	{ WALK_E, WALK_NE, WALK_SE },  // RIGHT
};
static const direction kFaceDir[3][3] = {
	// NONE      UP      DOWN
	{ DIR_OMNI, DIR_N, DIR_S }, // NONE
	{ DIR_W, DIR_NW, DIR_SW },  // LEFT
	{ DIR_E, DIR_NE, DIR_SE },  // RIGHT
};

void WalkInDir(MoveDirection dir)
{
	if (dir.x == MoveDirectionX::NONE && dir.y == MoveDirectionY::NONE) {
		if (sgbControllerActive && plr[myplr].destAction == ACTION_NONE)
			ClrPlrPath(myplr);
		return;
	}

	ClrPlrPath(myplr);
	plr[myplr].walkpath[0] = kMoveToWalkDir[static_cast<std::size_t>(dir.x)][static_cast<std::size_t>(dir.y)];
	plr[myplr].destAction = ACTION_NONE; // stop attacking, etc.
	plr[myplr]._pdir = kFaceDir[static_cast<std::size_t>(dir.x)][static_cast<std::size_t>(dir.y)];
}

void Movement()
{
	if (InGameMenu())
		return;

	MoveDirection move_dir = GetMoveDirection();
	if (move_dir.x != MoveDirectionX::NONE || move_dir.y != MoveDirectionY::NONE) {
		sgbControllerActive = true;
	}

	if (invflag) {
		InvMove(move_dir);
	} else if (chrflag && plr[myplr]._pStatPts > 0) {
		AttrIncBtnSnap(move_dir.y);
	} else if (spselflag) {
		HotSpellMove(move_dir);
	} else {
		WalkInDir(move_dir);
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
		sgbControllerActive = false;
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
	HandleRightStickMotion();

	// check for monsters first, then items, then towners.
	if (sgbControllerActive) {
		// Clear focuse set by cursor
		pcursplr = -1;
		pcursmonst = -1;
		pcursitem = -1;
		pcursobj = -1;
		if (!invflag) {
			*infostr = '\0';
			ClearPanel();
			FindActor();
			FindItemOrObject();
		}
	}
}

void plrctrls_after_game_logic()
{
	Movement();
}

void UseBeltItem(int type)
{
	for (int i = 0; i < MAXBELTITEMS; i++) {
		const auto id = AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId;
		const auto spellId = AllItemsList[plr[myplr].SpdList[i].IDidx].iSpell;
		if ((type == BLT_HEALING && (id == IMISC_HEAL || id == IMISC_FULLHEAL || (id == IMISC_SCROLL && spellId == SPL_HEAL)))
		    || (type == BLT_MANA && (id == IMISC_MANA || id == IMISC_FULLMANA))
		    || id == IMISC_REJUV || id == IMISC_FULLREJUV) {
			if (plr[myplr].SpdList[i]._itype > -1) {
				UseInvItem(myplr, INVITEM_BELT_FIRST + i);
				break;
			}
		}
	}
}

void PerformPrimaryAction()
{
	if (invflag) { // inventory is open
		if (pcurs == CURSOR_IDENTIFY)
			CheckIdentify(myplr, pcursinvitem);
		else if (pcurs == CURSOR_REPAIR)
			DoRepair(myplr, pcursinvitem);
		else if (pcurs == CURSOR_RECHARGE)
			DoRecharge(myplr, pcursinvitem);
		else
			CheckInvItem();
		return;
	}

	if (spselflag) {
		SetSpell();
		return;
	}

	if (chrflag && !chrbtnactive && plr[myplr]._pStatPts > 0) {
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
		return;
	}

	Interact();
}

bool SpellHasActorTarget()
{
	int spl = plr[myplr]._pRSpell;
	if (spl == SPL_TOWN || spl == SPL_TELEPORT)
		return false;

	if (spl == SPL_FIREWALL && pcursmonst != -1) {
		cursmx = monster[pcursmonst]._mx;
		cursmy = monster[pcursmonst]._my;
	}

	return pcursplr != -1 || pcursmonst != -1;
}

void UpdateSpellTarget()
{
	if (SpellHasActorTarget())
		return;

	pcursplr = -1;
	pcursmonst = -1;

	static const int kOffsets[8][2] = {
		{ 1, 1 },   // DIR_S
		{ 0, 1 },   // DIR_SW
		{ -1, 1 },  // DIR_W
		{ -1, 0 },  // DIR_NW
		{ -1, -1 }, // DIR_N
		{ 0, -1 },  // DIR_NE
		{ 1, -1 },  // DIR_E
		{ 1, 0 },   // DIR_SE
	};
	const auto &player = plr[myplr];
	cursmx = player._px + kOffsets[player._pdir][0];
	cursmy = player._py + kOffsets[player._pdir][1];
}

void PerformSpellAction()
{
	if (invflag) {
		int spl = plr[myplr]._pRSpell;
		if (pcurs >= CURSOR_FIRSTITEM) {
			DropItemBeforeTrig();
			return;
		}
		if (spl != SPL_IDENTIFY && spl != SPL_REPAIR && spl != SPL_RECHARGE)
			return;
	}

	if (spselflag) {
		SetSpell();
		return;
	}

	if (InGameMenu())
		return;
	if (TryIconCurs())
		return;

	int spl = plr[myplr]._pRSpell;
	if ((pcursplr == -1 && (spl == SPL_RESURRECT || spl == SPL_HEALOTHER))
	    || (pcursobj == -1 && spl == SPL_DISARM)) {
		if (plr[myplr]._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR27);
#ifndef SPAWN
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE27);
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE27);
#endif
		}
		return;
	}

	UpdateSpellTarget();
	CheckPlrSpell();
}

void PerformSecondaryAction()
{
	if (invflag) {
		if (pcursinvitem != -1)
			UseInvItem(myplr, pcursinvitem);
		return;
	}

	if (pcursitem != -1 && pcurs == CURSOR_HAND) {
		NetSendCmdLocParam1(true, CMD_GOTOAGETITEM, cursmx, cursmy, pcursitem);
	} else if (pcursobj != -1) {
		NetSendCmdLocParam1(true, CMD_OPOBJXY, cursmx, cursmy, pcursobj);
	}
}

} // namespace dvl
