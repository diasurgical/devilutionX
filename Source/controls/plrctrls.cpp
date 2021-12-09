#include "controls/plrctrls.h"

#include <algorithm>
#include <cstdint>
#include <list>

#include "automap.h"
#include "control.h"
#include "controls/controller.h"
#include "controls/controller_motion.h"
#include "controls/game_controls.h"
#include "controls/touch/gamepad.h"
#include "cursor.h"
#include "doom.h"
#include "engine/point.hpp"
#include "gmenu.h"
#include "help.h"
#include "inv.h"
#include "items.h"
#include "minitext.h"
#include "missiles.h"
#include "panels/spell_list.hpp"
#include "panels/ui_panels.hpp"
#include "stores.h"
#include "towners.h"
#include "trigs.h"

#define SPLICONLENGTH 56

namespace devilution {

bool sgbTouchActive = false;
bool sgbControllerActive = false;
int pcurstrig = -1;
int pcursmissile = -1;
quest_id pcursquest = Q_INVALID;

/**
 * Native game menu, controlled by simulating a keyboard.
 */
bool InGameMenu()
{
	return stextflag != STORE_NONE
	    || HelpFlag
	    || talkflag
	    || qtextflag
	    || gmenu_is_active()
	    || PauseMode == 2
	    || Players[MyPlayerId]._pInvincible;
}

namespace {

int Slot = SLOTXY_INV_FIRST;
int PreviousInventoryColumn = -1;

const Direction FaceDir[3][3] = {
	// NONE             UP                DOWN
	{ Direction::South, Direction::North, Direction::South },        // NONE
	{ Direction::West, Direction::NorthWest, Direction::SouthWest }, // LEFT
	{ Direction::East, Direction::NorthEast, Direction::SouthEast }, // RIGHT
};

/**
 * Number of angles to turn to face the coordinate
 * @param destination Tile coordinates
 * @return -1 == down
 */
int GetRotaryDistance(Point destination)
{
	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer.position.future == destination)
		return -1;

	int d1 = static_cast<int>(myPlayer._pdir);
	int d2 = static_cast<int>(GetDirection(myPlayer.position.future, destination));

	int d = abs(d1 - d2);
	if (d > 4)
		return 4 - (d % 4);

	return d;
}

/**
 * @brief Get the best case walking steps to coordinates
 * @param Position Tile coordinates
 */
int GetMinDistance(Point position)
{
	return Players[MyPlayerId].position.future.WalkingDistance(position);
}

/**
 * @brief Get walking steps to coordinate
 * @param destination Tile coordinates
 * @param maxDistance the max number of steps to search
 * @return number of steps, or 0 if not reachable
 */
int GetDistance(Point destination, int maxDistance)
{
	if (GetMinDistance(destination) > maxDistance) {
		return 0;
	}

	int8_t walkpath[MAX_PATH_LENGTH];
	auto &myPlayer = Players[MyPlayerId];
	int steps = FindPath([&myPlayer](Point position) { return PosOkPlayer(myPlayer, position); }, myPlayer.position.future, destination, walkpath);
	if (steps > maxDistance)
		return 0;

	return steps;
}

/**
 * @brief Get distance to coordinate
 * @param destination Tile coordinates
 */
int GetDistanceRanged(Point destination)
{
	return Players[MyPlayerId].position.future.ExactDistance(destination);
}

/**
 * @todo The loops in this function are iterating through tiles offset from the player position. This
 * could be accomplished by looping over the values in the Direction enum and making use of
 * Point math instead of nested loops from [-1, 1].
 */
void FindItemOrObject()
{
	int mx = Players[MyPlayerId].position.future.x;
	int my = Players[MyPlayerId].position.future.y;
	int rotations = 5;

	// As the player can not stand on the edge of the map this is safe from OOB
	for (int xx = -1; xx < 2; xx++) {
		for (int yy = -1; yy < 2; yy++) {
			if (dItem[mx + xx][my + yy] <= 0)
				continue;
			int i = dItem[mx + xx][my + yy] - 1;
			auto &item = Items[i];
			if (item.isEmpty()
			    || item._iSelFlag == 0)
				continue;
			int newRotations = GetRotaryDistance({ mx + xx, my + yy });
			if (rotations < newRotations)
				continue;
			if (xx != 0 && yy != 0 && GetDistance({ mx + xx, my + yy }, 1) == 0)
				continue;
			rotations = newRotations;
			pcursitem = i;
			cursPosition = Point { mx, my } + Displacement { xx, yy };
		}
	}

	if (leveltype == DTYPE_TOWN || pcursitem != -1)
		return; // Don't look for objects in town

	for (int xx = -1; xx < 2; xx++) {
		for (int yy = -1; yy < 2; yy++) {
			if (dObject[mx + xx][my + yy] == 0)
				continue;
			int o = abs(dObject[mx + xx][my + yy]) - 1;
			if (Objects[o]._oSelFlag == 0)
				continue;
			if (xx == 0 && yy == 0 && Objects[o]._oDoorFlag)
				continue; // Ignore doorway so we don't get stuck behind barrels
			int newRotations = GetRotaryDistance({ mx + xx, my + yy });
			if (rotations < newRotations)
				continue;
			if (xx != 0 && yy != 0 && GetDistance({ mx + xx, my + yy }, 1) == 0)
				continue;
			if (Objects[o].IsDisabled())
				continue;
			rotations = newRotations;
			pcursobj = o;
			cursPosition = Point { mx, my } + Displacement { xx, yy };
		}
	}
}

void CheckTownersNearby()
{
	for (int i = 0; i < 16; i++) {
		int distance = GetDistance(Towners[i].position, 2);
		if (distance == 0)
			continue;
		pcursmonst = i;
	}
}

bool HasRangedSpell()
{
	int spl = Players[MyPlayerId]._pRSpell;

	return spl != SPL_INVALID
	    && spl != SPL_TOWN
	    && spl != SPL_TELEPORT
	    && spelldata[spl].sTargeted
	    && !spelldata[spl].sTownSpell;
}

bool CanTargetMonster(const Monster &monster)
{
	if ((monster._mFlags & (MFLAG_HIDDEN | MFLAG_GOLEM)) != 0)
		return false;
	if (monster._mhitpoints >> 6 <= 0) // dead
		return false;

	if (!IsTileLit(monster.position.tile)) // not visible
		return false;

	const int mx = monster.position.tile.x;
	const int my = monster.position.tile.y;
	if (dMonster[mx][my] == 0)
		return false;

	return true;
}

void FindRangedTarget()
{
	int rotations = 0;
	int distance = 0;
	bool canTalk = false;

	for (int i = 0; i < ActiveMonsterCount; i++) {
		int mi = ActiveMonsters[i];
		const auto &monster = Monsters[mi];

		if (!CanTargetMonster(monster))
			continue;

		const bool newCanTalk = CanTalkToMonst(monster);
		if (pcursmonst != -1 && !canTalk && newCanTalk)
			continue;
		const int newDdistance = GetDistanceRanged(monster.position.future);
		const int newRotations = GetRotaryDistance(monster.position.future);
		if (pcursmonst != -1 && canTalk == newCanTalk) {
			if (distance < newDdistance)
				continue;
			if (distance == newDdistance && rotations < newRotations)
				continue;
		}
		distance = newDdistance;
		rotations = newRotations;
		canTalk = newCanTalk;
		pcursmonst = mi;
	}
}

void FindMeleeTarget()
{
	bool visited[MAXDUNX][MAXDUNY] = { {} };
	int maxSteps = 25; // Max steps for FindPath is 25
	int rotations = 0;
	bool canTalk = false;

	struct SearchNode {
		int x, y;
		int steps;
	};
	std::list<SearchNode> queue;

	auto &myPlayer = Players[MyPlayerId];

	{
		const int startX = myPlayer.position.future.x;
		const int startY = myPlayer.position.future.y;
		visited[startX][startY] = true;
		queue.push_back({ startX, startY, 0 });
	}

	while (!queue.empty()) {
		SearchNode node = queue.front();
		queue.pop_front();

		for (auto pathDir : PathDirs) {
			const int dx = node.x + pathDir.deltaX;
			const int dy = node.y + pathDir.deltaY;

			if (visited[dx][dy])
				continue; // already visisted

			if (node.steps > maxSteps) {
				visited[dx][dy] = true;
				continue;
			}

			if (!PosOkPlayer(myPlayer, { dx, dy })) {
				visited[dx][dy] = true;

				if (dMonster[dx][dy] != 0) {
					const int mi = abs(dMonster[dx][dy]) - 1;
					const auto &monster = Monsters[mi];
					if (CanTargetMonster(monster)) {
						const bool newCanTalk = CanTalkToMonst(monster);
						if (pcursmonst != -1 && !canTalk && newCanTalk)
							continue;
						const int newRotations = GetRotaryDistance({ dx, dy });
						if (pcursmonst != -1 && canTalk == newCanTalk && rotations < newRotations)
							continue;
						rotations = newRotations;
						canTalk = newCanTalk;
						pcursmonst = mi;
						if (!canTalk)
							maxSteps = node.steps; // Monsters found, cap search to current steps
					}
				}

				continue;
			}

			if (path_solid_pieces({ node.x, node.y }, { dx, dy })) {
				queue.push_back({ dx, dy, node.steps + 1 });
				visited[dx][dy] = true;
			}
		}
	}
}

void CheckMonstersNearby()
{
	if (Players[MyPlayerId].UsesRangedWeapon() || HasRangedSpell()) {
		FindRangedTarget();
		return;
	}

	FindMeleeTarget();
}

void CheckPlayerNearby()
{
	int newDdistance;
	int rotations = 0;
	int distance = 0;

	if (pcursmonst != -1)
		return;

	auto &myPlayer = Players[MyPlayerId];

	int spl = myPlayer._pRSpell;
	if (gbFriendlyMode && spl != SPL_RESURRECT && spl != SPL_HEALOTHER)
		return;

	for (int i = 0; i < MAX_PLRS; i++) {
		if (i == MyPlayerId)
			continue;
		const auto &player = Players[i];
		const int mx = player.position.future.x;
		const int my = player.position.future.y;
		if (dPlayer[mx][my] == 0
		    || !IsTileLit(player.position.future)
		    || (player._pHitPoints == 0 && spl != SPL_RESURRECT))
			continue;

		if (myPlayer.UsesRangedWeapon() || HasRangedSpell() || spl == SPL_HEALOTHER) {
			newDdistance = GetDistanceRanged(player.position.future);
		} else {
			newDdistance = GetDistance(player.position.future, distance);
			if (newDdistance == 0)
				continue;
		}

		if (pcursplr != -1 && distance < newDdistance)
			continue;
		const int newRotations = GetRotaryDistance(player.position.future);
		if (pcursplr != -1 && distance == newDdistance && rotations < newRotations)
			continue;

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

	if (gbIsMultiplayer)
		CheckPlayerNearby();
}

void FindTrigger()
{
	int rotations = 0;
	int distance = 0;

	if (pcursitem != -1 || pcursobj != -1)
		return; // Prefer showing items/objects over triggers (use of cursm* conflicts)

	for (int i = 0; i < ActiveMissileCount; i++) {
		int mi = ActiveMissiles[i];
		auto &missile = Missiles[mi];
		if (missile._mitype == MIS_TOWN || missile._mitype == MIS_RPORTAL) {
			const int newDistance = GetDistance(missile.position.tile, 2);
			if (newDistance == 0)
				continue;
			if (pcursmissile != -1 && distance < newDistance)
				continue;
			const int newRotations = GetRotaryDistance(missile.position.tile);
			if (pcursmissile != -1 && distance == newDistance && rotations < newRotations)
				continue;
			cursPosition = missile.position.tile;
			pcursmissile = mi;
			distance = newDistance;
			rotations = newRotations;
		}
	}

	if (pcursmissile == -1) {
		for (int i = 0; i < numtrigs; i++) {
			int tx = trigs[i].position.x;
			int ty = trigs[i].position.y;
			if (trigs[i]._tlvl == 13)
				ty -= 1;
			const int newDistance = GetDistance({ tx, ty }, 2);
			if (newDistance == 0)
				continue;
			cursPosition = { tx, ty };
			pcurstrig = i;
		}

		if (pcurstrig == -1) {
			for (auto &quest : Quests) {
				if (quest._qidx == Q_BETRAYER || currlevel != quest._qlevel || quest._qslvl == 0)
					continue;
				const int newDistance = GetDistance(quest.position, 2);
				if (newDistance == 0)
					continue;
				cursPosition = quest.position;
				pcursquest = quest._qidx;
			}
		}
	}

	if (pcursmonst != -1 || pcursplr != -1 || cursPosition.x == -1 || cursPosition.y == -1)
		return; // Prefer monster/player info text

	CheckTrigForce();
	CheckTown();
	CheckRportal();
}

void Interact()
{
	if (leveltype == DTYPE_TOWN && pcursmonst != -1) {
		NetSendCmdLocParam1(true, CMD_TALKXY, Towners[pcursmonst].position, pcursmonst);
		return;
	}

	bool stand = false;
#ifdef VIRTUAL_GAMEPAD
	stand = VirtualGamepadState.standButton.isHeld;
#endif

	if (leveltype != DTYPE_TOWN && stand) {
		auto &myPlayer = Players[MyPlayerId];
		Direction pdir = myPlayer._pdir;
		AxisDirection moveDir = GetMoveDirection();
		bool motion = moveDir.x != AxisDirectionX_NONE || moveDir.y != AxisDirectionY_NONE;
		if (motion) {
			pdir = FaceDir[static_cast<std::size_t>(moveDir.x)][static_cast<std::size_t>(moveDir.y)];
		}

		Point position = myPlayer.position.tile + pdir;
		if (pcursmonst != -1 && !motion) {
			position = Monsters[pcursmonst].position.tile;
		}

		NetSendCmdLoc(MyPlayerId, true, Players[MyPlayerId].UsesRangedWeapon() ? CMD_RATTACKXY : CMD_SATTACKXY, position);
		return;
	}

	if (pcursmonst != -1) {
		if (!Players[MyPlayerId].UsesRangedWeapon() || CanTalkToMonst(Monsters[pcursmonst])) {
			NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
		} else {
			NetSendCmdParam1(true, CMD_RATTACKID, pcursmonst);
		}
		return;
	}

	if (leveltype != DTYPE_TOWN && pcursplr != -1 && !gbFriendlyMode) {
		NetSendCmdParam1(true, Players[MyPlayerId].UsesRangedWeapon() ? CMD_RATTACKPID : CMD_ATTACKPID, pcursplr);
		return;
	}

	if (pcursobj != -1) {
		NetSendCmdLocParam1(true, CMD_OPOBJXY, cursPosition, pcursobj);
		return;
	}
}

void AttrIncBtnSnap(AxisDirection dir)
{
	static AxisDirectionRepeater repeater;
	dir = repeater.Get(dir);
	if (dir.y == AxisDirectionY_NONE)
		return;

	if (chrbtnactive && Players[MyPlayerId]._pStatPts <= 0)
		return;

	// first, find our cursor location
	int slot = 0;
	Rectangle button;
	for (int i = 0; i < 4; i++) {
		button = ChrBtnsRect[i];
		button.position = GetPanelPosition(UiPanels::Character, button.position);
		if (button.Contains(MousePosition)) {
			slot = i;
			break;
		}
	}

	if (dir.y == AxisDirectionY_UP) {
		if (slot > 0)
			--slot;
	} else if (dir.y == AxisDirectionY_DOWN) {
		if (slot < 3)
			++slot;
	}

	// move cursor to our new location
	button = ChrBtnsRect[slot];
	button.position = GetPanelPosition(UiPanels::Character, button.position);
	SetCursorPos(button.Center());
}

Point InvGetEquipSlotCoord(const inv_body_loc invSlot)
{
	Point result = GetPanelPosition(UiPanels::Inventory);
	result.x -= (icursSize28.width - 1) * (InventorySlotSizeInPixels.width / 2);
	switch (invSlot) {
	case INVLOC_HEAD:
		result.x += ((InvRect[SLOTXY_HEAD_FIRST].x + InvRect[SLOTXY_HEAD_LAST].x) / 2);
		result.y += ((InvRect[SLOTXY_HEAD_FIRST].y + InvRect[SLOTXY_HEAD_LAST].y) / 2);
		break;
	case INVLOC_RING_LEFT:
		result.x += InvRect[SLOTXY_RING_LEFT].x;
		result.y += InvRect[SLOTXY_RING_LEFT].y;
		break;
	case INVLOC_RING_RIGHT:
		result.x += InvRect[SLOTXY_RING_RIGHT].x;
		result.y += InvRect[SLOTXY_RING_RIGHT].y;
		break;
	case INVLOC_AMULET:
		result.x += InvRect[SLOTXY_AMULET].x;
		result.y += InvRect[SLOTXY_AMULET].y;
		break;
	case INVLOC_HAND_LEFT:
		result.x += ((InvRect[SLOTXY_HAND_LEFT_FIRST].x + InvRect[SLOTXY_HAND_LEFT_LAST].x) / 2);
		result.y += ((InvRect[SLOTXY_HAND_LEFT_FIRST].y + InvRect[SLOTXY_HAND_LEFT_LAST].y) / 2);
		break;
	case INVLOC_HAND_RIGHT:
		result.x += ((InvRect[SLOTXY_HAND_RIGHT_FIRST].x + InvRect[SLOTXY_HAND_RIGHT_LAST].x) / 2);
		result.y += ((InvRect[SLOTXY_HAND_RIGHT_FIRST].y + InvRect[SLOTXY_HAND_RIGHT_LAST].y) / 2);
		break;
	case INVLOC_CHEST:
		result.x += ((InvRect[SLOTXY_CHEST_FIRST].x + InvRect[SLOTXY_CHEST_LAST].x) / 2);
		result.y += ((InvRect[SLOTXY_CHEST_FIRST].y + InvRect[SLOTXY_CHEST_LAST].y) / 2);
		break;
	default:
		break;
	}

	return result;
}

Point InvGetEquipSlotCoordFromInvSlot(const inv_xy_slot slot)
{
	switch (slot) {
	case SLOTXY_HEAD_FIRST:
	case SLOTXY_HEAD_LAST:
		return InvGetEquipSlotCoord(INVLOC_HEAD);
	case SLOTXY_RING_LEFT:
		return InvGetEquipSlotCoord(INVLOC_RING_LEFT);
	case SLOTXY_RING_RIGHT:
		return InvGetEquipSlotCoord(INVLOC_RING_RIGHT);
	case SLOTXY_AMULET:
		return InvGetEquipSlotCoord(INVLOC_AMULET);
	case SLOTXY_HAND_LEFT_FIRST:
	case SLOTXY_HAND_LEFT_LAST:
		return InvGetEquipSlotCoord(INVLOC_HAND_LEFT);
	case SLOTXY_HAND_RIGHT_FIRST:
	case SLOTXY_HAND_RIGHT_LAST:
		return InvGetEquipSlotCoord(INVLOC_HAND_RIGHT);
	case SLOTXY_CHEST_FIRST:
	case SLOTXY_CHEST_LAST:
		return InvGetEquipSlotCoord(INVLOC_CHEST);
	default:
		return {};
	}
}

/**
 * Get coordinates for a given slot
 */
Point GetSlotCoord(int slot)
{
	if (slot >= SLOTXY_BELT_FIRST && slot <= SLOTXY_BELT_LAST) {
		return GetPanelPosition(UiPanels::Main, InvRect[slot]);
	}

	return GetPanelPosition(UiPanels::Inventory, InvRect[slot]);
}

/**
 * Return the item id of the current slot
 */
int GetItemIdOnSlot(int slot)
{
	if (slot >= SLOTXY_INV_FIRST && slot <= SLOTXY_INV_LAST) {
		return abs(MyPlayer->InvGrid[slot - SLOTXY_INV_FIRST]);
	}

	return 0;
}

/**
 * Get item size (grid size) on the slot specified. Returns 1x1 if none exists.
 */
Size GetItemSizeOnSlot(int slot)
{
	if (slot >= SLOTXY_INV_FIRST && slot <= SLOTXY_INV_LAST) {
		int8_t ii = GetItemIdOnSlot(slot);
		if (ii != 0) {
			Item &item = MyPlayer->InvList[ii - 1];
			if (!item.isEmpty()) {
				auto size = GetInvItemSize(item._iCurs + CURSOR_FIRSTITEM);
				size.width /= InventorySlotSizeInPixels.width;
				size.height /= InventorySlotSizeInPixels.height;

				return size;
			}
		}
	}

	return { 1, 1 };
}

/**
 * Search for the first slot occupied by an item in the inventory.
 */
int FindFirstSlotOnItem(int8_t itemInvId)
{
	if (itemInvId == 0)
		return -1;
	for (int s = SLOTXY_INV_FIRST; s < SLOTXY_INV_LAST; s++) {
		if (GetItemIdOnSlot(s) == itemInvId)
			return s;
	}
	return -1;
}

/**
 * Reset cursor position based on the current slot.
 */
void ResetInvCursorPosition()
{
	Point mousePos {};
	if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_INV_LAST) {
		int8_t itemInvId = GetItemIdOnSlot(Slot);
		int itemSlot = FindFirstSlotOnItem(itemInvId);
		if (itemSlot >= 0)
			Slot = itemSlot;

		// offset the slot to always move to the top-left most slot of that item
		Size itemSize = GetItemSizeOnSlot(Slot);
		Slot -= ((itemSize.height - 1) * INV_ROW_SLOT_SIZE);
		mousePos = GetSlotCoord(Slot);
		mousePos.x += ((itemSize.width - 1) * InventorySlotSizeInPixels.width) / 2;
		mousePos.y += ((itemSize.height - 1) * InventorySlotSizeInPixels.height) / 2;
	} else if (Slot >= SLOTXY_BELT_FIRST && Slot <= SLOTXY_BELT_LAST) {
		mousePos = GetSlotCoord(Slot);
	} else {
		mousePos = InvGetEquipSlotCoordFromInvSlot((inv_xy_slot)Slot);
	}

	mousePos.x += (InventorySlotSizeInPixels.width / 2);
	mousePos.y -= (InventorySlotSizeInPixels.height / 2);

	SetCursorPos(mousePos);
}

int FindClosestInventorySlot(Point mousePos)
{
	int shortestDistance = std::numeric_limits<int>::max();
	int bestSlot = 0;
	mousePos += Displacement { -INV_SLOT_HALF_SIZE_PX, INV_SLOT_HALF_SIZE_PX };

	for (int i = 0; i < NUM_XY_SLOTS; i++) {
		int distance = mousePos.ManhattanDistance(GetSlotCoord(i));
		if (distance < shortestDistance) {
			shortestDistance = distance;
			bestSlot = i;
		}
	}

	return bestSlot;
}

/**
 * @brief Figures out where on the body to move when on the first row
 */
Point InvMoveToBody(int slot)
{
	PreviousInventoryColumn = slot - SLOTXY_INV_ROW1_FIRST;
	if (slot <= SLOTXY_INV_ROW1_FIRST + 2) { // first 3 general slots
		Slot = SLOTXY_RING_LEFT;
		return InvGetEquipSlotCoord(INVLOC_RING_LEFT);
	} else if (slot <= SLOTXY_INV_ROW1_FIRST + 6) { // middle 4 general slots
		Slot = SLOTXY_CHEST_FIRST;
		return InvGetEquipSlotCoord(INVLOC_CHEST);
	} else { // last 3 general slots
		Slot = SLOTXY_RING_RIGHT;
		return InvGetEquipSlotCoord(INVLOC_RING_RIGHT);
	}

	return GetSlotCoord(0);
}

/**
 * Move the cursor around in our inventory
 * If mouse coords are at SLOTXY_CHEST_LAST, consider this center of equipment
 * small inventory squares are 29x29 (roughly)
 */
void InvMove(AxisDirection dir)
{
	static AxisDirectionRepeater repeater(/*min_interval_ms=*/150);
	dir = repeater.Get(dir);
	if (dir.x == AxisDirectionX_NONE && dir.y == AxisDirectionY_NONE)
		return;

	Point mousePos = MousePosition;

	const bool isHoldingItem = pcurs >= CURSOR_FIRSTITEM;

	// normalize slots
	if (Slot < 0)
		Slot = FindClosestInventorySlot(mousePos);
	else if (Slot >= SLOTXY_HEAD_FIRST && Slot <= SLOTXY_HEAD_LAST)
		Slot = SLOTXY_HEAD_FIRST;
	else if (Slot >= SLOTXY_HAND_LEFT_FIRST && Slot <= SLOTXY_HAND_LEFT_LAST)
		Slot = SLOTXY_HAND_LEFT_FIRST;
	else if (Slot >= SLOTXY_CHEST_FIRST && Slot <= SLOTXY_CHEST_LAST)
		Slot = SLOTXY_CHEST_FIRST;
	else if (Slot >= SLOTXY_HAND_RIGHT_FIRST && Slot <= SLOTXY_HAND_RIGHT_LAST)
		Slot = SLOTXY_HAND_RIGHT_FIRST;
	else if (Slot > SLOTXY_BELT_LAST)
		Slot = SLOTXY_BELT_LAST;

	const int initialSlot = Slot;
	auto &myPlayer = Players[MyPlayerId];

	// when item is on cursor (pcurs > 1), this is the real cursor XY
	if (dir.x == AxisDirectionX_LEFT) {
		if (isHoldingItem) {
			if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_BELT_LAST) {
				if (IsNoneOf(Slot, SLOTXY_INV_ROW1_FIRST, SLOTXY_INV_ROW2_FIRST, SLOTXY_INV_ROW3_FIRST, SLOTXY_INV_ROW4_FIRST, SLOTXY_BELT_FIRST)) {
					Slot -= 1;
					mousePos = GetSlotCoord(Slot);
				}
			} else if (myPlayer.HoldItem._itype == ItemType::Ring) {
				Slot = SLOTXY_RING_LEFT;
				mousePos = InvGetEquipSlotCoord(INVLOC_RING_LEFT);
			} else if (myPlayer.HoldItem.isWeapon() || myPlayer.HoldItem.isShield()) {
				Slot = SLOTXY_HAND_LEFT_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_HAND_LEFT);
			}
		} else {
			if (Slot == SLOTXY_HAND_RIGHT_FIRST) {
				Slot = SLOTXY_CHEST_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_CHEST);
			} else if (Slot == SLOTXY_CHEST_FIRST) {
				Slot = SLOTXY_HAND_LEFT_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_HAND_LEFT);
			} else if (Slot == SLOTXY_AMULET) {
				Slot = SLOTXY_HEAD_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_HEAD);
			} else if (Slot == SLOTXY_RING_RIGHT) {
				Slot = SLOTXY_RING_LEFT;
				mousePos = InvGetEquipSlotCoord(INVLOC_RING_LEFT);
			} else if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_BELT_LAST) {
				int8_t itemId = GetItemIdOnSlot(Slot);
				if (itemId != 0) {
					for (int i = 1; i < INV_ROW_SLOT_SIZE && !IsAnyOf(Slot - i + 1, SLOTXY_INV_ROW1_FIRST, SLOTXY_INV_ROW2_FIRST, SLOTXY_INV_ROW3_FIRST, SLOTXY_INV_ROW4_FIRST, SLOTXY_BELT_FIRST); i++) {
						if (itemId != GetItemIdOnSlot(Slot - i)) {
							Slot -= i;
							break;
						}
					}
				} else if (IsNoneOf(Slot, SLOTXY_INV_ROW1_FIRST, SLOTXY_INV_ROW2_FIRST, SLOTXY_INV_ROW3_FIRST, SLOTXY_INV_ROW4_FIRST, SLOTXY_BELT_FIRST)) {
					Slot -= 1;
				}
				mousePos = GetSlotCoord(Slot);
			}
		}
	} else if (dir.x == AxisDirectionX_RIGHT) {
		if (isHoldingItem) {
			if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_BELT_LAST) {
				if (IsNoneOf(Slot - 1 + icursSize28.width, SLOTXY_INV_ROW1_LAST, SLOTXY_INV_ROW2_LAST, SLOTXY_INV_ROW3_LAST, SLOTXY_INV_ROW4_LAST, SLOTXY_BELT_LAST)) {
					Slot += 1;
					mousePos = GetSlotCoord(Slot);
				}
			} else if (myPlayer.HoldItem._itype == ItemType::Ring) {
				Slot = SLOTXY_RING_RIGHT;
				mousePos = InvGetEquipSlotCoord(INVLOC_RING_RIGHT);
			} else if (myPlayer.HoldItem.isWeapon() || myPlayer.HoldItem.isShield()) {
				Slot = SLOTXY_HAND_RIGHT_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_HAND_RIGHT);
			}
		} else {
			if (Slot == SLOTXY_RING_LEFT) {
				Slot = SLOTXY_RING_RIGHT;
				mousePos = InvGetEquipSlotCoord(INVLOC_RING_RIGHT);
			} else if (Slot == SLOTXY_HAND_LEFT_FIRST) {
				Slot = SLOTXY_CHEST_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_CHEST);
			} else if (Slot == SLOTXY_CHEST_FIRST) {
				Slot = SLOTXY_HAND_RIGHT_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_HAND_RIGHT);
			} else if (Slot == SLOTXY_HEAD_FIRST) {
				Slot = SLOTXY_AMULET;
				mousePos = InvGetEquipSlotCoord(INVLOC_AMULET);
			} else if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_BELT_LAST) {
				int8_t itemId = GetItemIdOnSlot(Slot);
				if (itemId != 0) {
					for (int i = 1; i < INV_ROW_SLOT_SIZE && !IsAnyOf(Slot + i - 1, SLOTXY_INV_ROW1_LAST, SLOTXY_INV_ROW2_LAST, SLOTXY_INV_ROW3_LAST, SLOTXY_INV_ROW4_LAST, SLOTXY_BELT_LAST); i++) {
						if (itemId != GetItemIdOnSlot(Slot + i)) {
							Slot += i;
							break;
						}
					}
				} else if (IsNoneOf(Slot, SLOTXY_INV_ROW1_LAST, SLOTXY_INV_ROW2_LAST, SLOTXY_INV_ROW3_LAST, SLOTXY_INV_ROW4_LAST, SLOTXY_BELT_LAST)) {
					Slot += 1;
				}
				mousePos = GetSlotCoord(Slot);
			}
		}
	}
	if (dir.y == AxisDirectionY_UP) {
		if (isHoldingItem) {
			if (Slot >= SLOTXY_INV_ROW2_FIRST) { // general inventory
				Slot -= INV_ROW_SLOT_SIZE;
				mousePos = GetSlotCoord(Slot);
			} else if (Slot >= SLOTXY_INV_FIRST) {
				if (myPlayer.HoldItem._itype == ItemType::Ring) {
					if (Slot >= SLOTXY_INV_ROW1_FIRST && Slot <= SLOTXY_INV_ROW1_FIRST + (INV_ROW_SLOT_SIZE / 2) - 1) {
						Slot = SLOTXY_RING_LEFT;
						mousePos = InvGetEquipSlotCoord(INVLOC_RING_LEFT);
					} else {
						Slot = SLOTXY_RING_RIGHT;
						mousePos = InvGetEquipSlotCoord(INVLOC_RING_RIGHT);
					}
				} else if (myPlayer.HoldItem.isWeapon()) {
					Slot = SLOTXY_HAND_LEFT_FIRST;
					mousePos = InvGetEquipSlotCoord(INVLOC_HAND_LEFT);
				} else if (myPlayer.HoldItem.isShield()) {
					Slot = SLOTXY_HAND_RIGHT_FIRST;
					mousePos = InvGetEquipSlotCoord(INVLOC_HAND_RIGHT);
				} else if (myPlayer.HoldItem.isHelm()) {
					Slot = SLOTXY_HEAD_FIRST;
					mousePos = InvGetEquipSlotCoord(INVLOC_HEAD);
				} else if (myPlayer.HoldItem.isArmor()) {
					Slot = SLOTXY_CHEST_FIRST;
					mousePos = InvGetEquipSlotCoord(INVLOC_CHEST);
				} else if (myPlayer.HoldItem._itype == ItemType::Amulet) {
					Slot = SLOTXY_AMULET;
					mousePos = InvGetEquipSlotCoord(INVLOC_AMULET);
				}
			}
		} else {
			if (Slot >= SLOTXY_INV_ROW1_FIRST && Slot <= SLOTXY_INV_ROW1_LAST) {
				mousePos = InvMoveToBody(Slot);
			} else if (Slot == SLOTXY_CHEST_FIRST || Slot == SLOTXY_HAND_LEFT_FIRST) {
				Slot = SLOTXY_HEAD_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_HEAD);
			} else if (Slot == SLOTXY_RING_LEFT) {
				Slot = SLOTXY_HAND_LEFT_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_HAND_LEFT);
			} else if (Slot == SLOTXY_RING_RIGHT) {
				Slot = SLOTXY_HAND_RIGHT_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_HAND_RIGHT);
			} else if (Slot == SLOTXY_HAND_RIGHT_FIRST) {
				Slot = SLOTXY_AMULET;
				mousePos = InvGetEquipSlotCoord(INVLOC_AMULET);
			} else if (Slot >= SLOTXY_INV_ROW2_FIRST) {
				int8_t itemId = GetItemIdOnSlot(Slot);
				if (itemId != 0) {
					for (int i = 1; i < 5; i++) {
						if (Slot - i * INV_ROW_SLOT_SIZE < SLOTXY_INV_ROW1_FIRST) {
							mousePos = InvMoveToBody(Slot - (i - 1) * INV_ROW_SLOT_SIZE);
							break;
						}
						if (itemId != GetItemIdOnSlot(Slot - i * INV_ROW_SLOT_SIZE)) {
							Slot -= i * INV_ROW_SLOT_SIZE;
							mousePos = GetSlotCoord(Slot);
							break;
						}
					}
				} else {
					Slot -= INV_ROW_SLOT_SIZE;
					mousePos = GetSlotCoord(Slot);
				}
			}
		}
	} else if (dir.y == AxisDirectionY_DOWN) {
		if (isHoldingItem) {
			if (Slot == SLOTXY_HEAD_FIRST || Slot == SLOTXY_CHEST_FIRST) {
				Slot = SLOTXY_INV_ROW1_FIRST + 4;
				mousePos = GetSlotCoord(Slot);
			} else if (Slot == SLOTXY_RING_LEFT || Slot == SLOTXY_HAND_LEFT_FIRST) {
				Slot = SLOTXY_INV_ROW1_FIRST + 1;
				mousePos = GetSlotCoord(Slot);
			} else if (Slot == SLOTXY_RING_RIGHT || Slot == SLOTXY_HAND_RIGHT_FIRST || Slot == SLOTXY_AMULET) {
				Slot = SLOTXY_INV_ROW1_LAST - 1;
				mousePos = GetSlotCoord(Slot);
			} else if (Slot <= (SLOTXY_INV_ROW4_LAST - (icursSize28.height * INV_ROW_SLOT_SIZE))) {
				Slot += INV_ROW_SLOT_SIZE;
				mousePos = GetSlotCoord(Slot);
			} else if (Slot <= SLOTXY_INV_LAST && myPlayer.HoldItem._itype == ItemType::Misc && icursSize28 == Size { 1, 1 }) { // forcing only 1x1 misc items
				if (Slot + INV_ROW_SLOT_SIZE <= SLOTXY_BELT_LAST)
					Slot += INV_ROW_SLOT_SIZE;
				mousePos = GetSlotCoord(Slot);
			}
		} else {
			if (Slot == SLOTXY_HEAD_FIRST) {
				Slot = SLOTXY_CHEST_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_CHEST);
			} else if (Slot == SLOTXY_CHEST_FIRST) {
				if (PreviousInventoryColumn >= 3 && PreviousInventoryColumn <= 6)
					Slot = SLOTXY_INV_ROW1_FIRST + PreviousInventoryColumn;
				else
					Slot = SLOTXY_INV_ROW1_FIRST + (INV_ROW_SLOT_SIZE / 2);
				mousePos = GetSlotCoord(Slot);
			} else if (Slot == SLOTXY_HAND_LEFT_FIRST) {
				Slot = SLOTXY_RING_LEFT;
				mousePos = InvGetEquipSlotCoord(INVLOC_RING_LEFT);
			} else if (Slot == SLOTXY_RING_LEFT) {
				if (PreviousInventoryColumn >= 0 && PreviousInventoryColumn <= 2)
					Slot = SLOTXY_INV_ROW1_FIRST + PreviousInventoryColumn;
				else
					Slot = SLOTXY_INV_ROW1_FIRST + 1;
				mousePos = GetSlotCoord(Slot);
			} else if (Slot == SLOTXY_RING_RIGHT) {
				if (PreviousInventoryColumn >= 7 && PreviousInventoryColumn <= 9)
					Slot = SLOTXY_INV_ROW1_FIRST + PreviousInventoryColumn;
				else
					Slot = SLOTXY_INV_ROW1_LAST - 1;
				mousePos = GetSlotCoord(Slot);
			} else if (Slot == SLOTXY_AMULET) {
				Slot = SLOTXY_HAND_RIGHT_FIRST;
				mousePos = InvGetEquipSlotCoord(INVLOC_HAND_RIGHT);
			} else if (Slot == SLOTXY_HAND_RIGHT_FIRST) {
				Slot = SLOTXY_RING_RIGHT;
				mousePos = InvGetEquipSlotCoord(INVLOC_RING_RIGHT);
			} else if (Slot <= SLOTXY_INV_LAST) {
				int8_t itemId = GetItemIdOnSlot(Slot);
				if (itemId != 0) {
					for (int i = 1; i < 5 && Slot + i * INV_ROW_SLOT_SIZE <= SLOTXY_BELT_LAST; i++) {
						if (itemId != GetItemIdOnSlot(Slot + i * INV_ROW_SLOT_SIZE)) {
							Slot += i * INV_ROW_SLOT_SIZE;
							break;
						}
					}
				} else if (Slot + INV_ROW_SLOT_SIZE <= SLOTXY_BELT_LAST) {
					Slot += INV_ROW_SLOT_SIZE;
				}
				mousePos = GetSlotCoord(Slot);
			}
		}
	}

	// no movement was made
	if (Slot == initialSlot)
		return;

	// get item under new slot if navigating on the inventory
	if (!isHoldingItem && Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_INV_LAST) {
		int8_t itemInvId = GetItemIdOnSlot(Slot);
		int itemSlot = FindFirstSlotOnItem(itemInvId);
		if (itemSlot < 0)
			itemSlot = Slot;

		// offset the slot to always move to the top-left most slot of that item
		mousePos = GetSlotCoord(itemSlot);
		Size itemSize = GetItemSizeOnSlot(itemSlot);
		mousePos.x += ((itemSize.width - 1) * InventorySlotSizeInPixels.width) / 2;
		mousePos.y += ((itemSize.height - 1) * InventorySlotSizeInPixels.height) / 2;
	}

	// move cursor to the center of the slot if not holding anything or top left is holding an object
	if (isHoldingItem) {
		if (Slot >= SLOTXY_INV_FIRST)
			mousePos.y -= InventorySlotSizeInPixels.height;
		else
			mousePos.y -= (int)((icursSize28.height / 2.0) * InventorySlotSizeInPixels.height) + (InventorySlotSizeInPixels.height / 2);
	} else {
		mousePos.x += (InventorySlotSizeInPixels.width / 2);
		mousePos.y -= (InventorySlotSizeInPixels.height / 2);
	}

	if (mousePos == MousePosition) {
		return; // Avoid wobeling when scalled
	}

	SetCursorPos(mousePos);
}

void HotSpellMove(AxisDirection dir)
{
	static AxisDirectionRepeater repeater;
	dir = repeater.Get(dir);
	if (dir.x == AxisDirectionX_NONE && dir.y == AxisDirectionY_NONE)
		return;

	auto spellListItems = GetSpellListItems();

	Point position = MousePosition;
	int shortestDistance = std::numeric_limits<int>::max();
	for (auto &spellListItem : spellListItems) {
		Point center = spellListItem.location + Displacement { SPLICONLENGTH / 2, -SPLICONLENGTH / 2 };
		int distance = MousePosition.ManhattanDistance(center);
		if (distance < shortestDistance) {
			position = center;
			shortestDistance = distance;
		}
	}

	const auto search = [&](AxisDirection dir, bool searchForward) {
		if (dir.x == AxisDirectionX_NONE && dir.y == AxisDirectionY_NONE)
			return;

		for (size_t i = 0; i < spellListItems.size(); i++) {
			const size_t index = searchForward ? spellListItems.size() - i - 1 : i;

			auto &spellListItem = spellListItems[index];
			if (spellListItem.isSelected)
				continue;

			Point center = spellListItem.location + Displacement { SPLICONLENGTH / 2, -SPLICONLENGTH / 2 };
			if (dir.x == AxisDirectionX_LEFT && center.x >= MousePosition.x)
				continue;
			if (dir.x == AxisDirectionX_RIGHT && center.x <= MousePosition.x)
				continue;
			if (dir.x == AxisDirectionX_NONE && center.x != position.x)
				continue;
			if (dir.y == AxisDirectionY_UP && center.y >= MousePosition.y)
				continue;
			if (dir.y == AxisDirectionY_DOWN && center.y <= MousePosition.y)
				continue;
			if (dir.y == AxisDirectionY_NONE && center.y != position.y)
				continue;

			position = center;
			break;
		}
	};
	search({ AxisDirectionX_NONE, dir.y }, dir.y == AxisDirectionY_DOWN);
	search({ dir.x, AxisDirectionY_NONE }, dir.x == AxisDirectionX_RIGHT);

	if (position != MousePosition) {
		SetCursorPos(position);
	}
}

void SpellBookMove(AxisDirection dir)
{
	static AxisDirectionRepeater repeater;
	dir = repeater.Get(dir);

	if (dir.x == AxisDirectionX_LEFT) {
		if (sbooktab > 0)
			sbooktab--;
	} else if (dir.x == AxisDirectionX_RIGHT) {
		if ((gbIsHellfire && sbooktab < 4) || (!gbIsHellfire && sbooktab < 3))
			sbooktab++;
	}
}

/**
 * @brief check if stepping in direction (dir) from position is blocked.
 *
 * If you step from A to B, at leat one of the Xs need to be clear:
 *
 *  AX
 *  XB
 *
 *  @return true if step is blocked
 */
bool IsPathBlocked(Point position, Direction dir)
{
	if (IsNoneOf(dir, Direction::North, Direction::East, Direction::South, Direction::West))
		return false; // Steps along a major axis don't need to check corners

	auto leftStep { position + Left(dir) };
	auto rightStep { position + Right(dir) };

	if (IsTileNotSolid(leftStep) && IsTileNotSolid(rightStep))
		return false;

	auto &myPlayer = Players[MyPlayerId];

	return !PosOkPlayer(myPlayer, leftStep) && !PosOkPlayer(myPlayer, rightStep);
}

void WalkInDir(int playerId, AxisDirection dir)
{
	auto &player = Players[playerId];

	if (dir.x == AxisDirectionX_NONE && dir.y == AxisDirectionY_NONE) {
		if (sgbControllerActive && player.walkpath[0] != WALK_NONE && player.destAction == ACTION_NONE)
			NetSendCmdLoc(playerId, true, CMD_WALKXY, player.position.future); // Stop walking
		return;
	}

	const Direction pdir = FaceDir[static_cast<std::size_t>(dir.x)][static_cast<std::size_t>(dir.y)];
	const auto delta = player.position.future + pdir;

	if (!player.IsWalking() && player.CanChangeAction())
		player._pdir = pdir;

#ifdef VIRTUAL_GAMEPAD
	if (VirtualGamepadState.standButton.isHeld) {
		if (player._pmode == PM_STAND)
			StartStand(playerId, pdir);
		return;
	}
#endif

	if (PosOkPlayer(player, delta) && IsPathBlocked(player.position.future, pdir)) {
		if (player._pmode == PM_STAND)
			StartStand(playerId, pdir);
		return; // Don't start backtrack around obstacles
	}

	NetSendCmdLoc(playerId, true, CMD_WALKXY, delta);
}

void QuestLogMove(AxisDirection moveDir)
{
	static AxisDirectionRepeater repeater;
	moveDir = repeater.Get(moveDir);
	if (moveDir.y == AxisDirectionY_UP)
		QuestlogUp();
	else if (moveDir.y == AxisDirectionY_DOWN)
		QuestlogDown();
}

void StoreMove(AxisDirection moveDir)
{
	static AxisDirectionRepeater repeater;
	moveDir = repeater.Get(moveDir);
	if (moveDir.y == AxisDirectionY_UP)
		StoreUp();
	else if (moveDir.y == AxisDirectionY_DOWN)
		StoreDown();
}

using HandleLeftStickOrDPadFn = void (*)(devilution::AxisDirection);

HandleLeftStickOrDPadFn GetLeftStickOrDPadGameUIHandler()
{
	if (invflag) {
		return &InvMove;
	}
	if (chrflag && Players[MyPlayerId]._pStatPts > 0) {
		return &AttrIncBtnSnap;
	}
	if (spselflag) {
		return &HotSpellMove;
	}
	if (sbookflag) {
		return &SpellBookMove;
	}
	if (QuestLogIsOpen) {
		return &QuestLogMove;
	}
	if (stextflag != STORE_NONE) {
		return &StoreMove;
	}
	return nullptr;
}

void ProcessLeftStickOrDPadGameUI()
{
	HandleLeftStickOrDPadFn handler = GetLeftStickOrDPadGameUIHandler();
	if (handler != nullptr)
		handler(GetLeftStickOrDpadDirection(true));
}

void Movement(int playerId)
{
	if (InGameMenu()
	    || IsControllerButtonPressed(ControllerButton_BUTTON_START)
	    || IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
		return;

	AxisDirection moveDir = GetMoveDirection();
	if (moveDir.x != AxisDirectionX_NONE || moveDir.y != AxisDirectionY_NONE) {
		sgbControllerActive = true;
	}

	if (GetLeftStickOrDPadGameUIHandler() == nullptr) {
		WalkInDir(playerId, moveDir);
	}
}

struct RightStickAccumulator {

	RightStickAccumulator()
	{
		lastTc = SDL_GetTicks();
		hiresDX = 0;
		hiresDY = 0;
	}

	void Pool(int *x, int *y, int slowdown)
	{
		const Uint32 tc = SDL_GetTicks();
		const int dtc = tc - lastTc;
		hiresDX += rightStickX * dtc;
		hiresDY += rightStickY * dtc;
		const int dx = static_cast<int>(hiresDX / slowdown);
		const int dy = static_cast<int>(hiresDY / slowdown);
		*x += dx;
		*y -= dy;
		lastTc = tc;
		// keep track of remainder for sub-pixel motion
		hiresDX -= dx * slowdown;
		hiresDY -= dy * slowdown;
	}

	void Clear()
	{
		lastTc = SDL_GetTicks();
	}

	uint32_t lastTc;
	float hiresDX;
	float hiresDY;
};

} // namespace

bool IsAutomapActive()
{
	return AutomapActive && leveltype != DTYPE_TOWN;
}

bool IsMovingMouseCursorWithController()
{
	return rightStickX != 0 || rightStickY != 0;
}

void HandleRightStickMotion()
{
	static RightStickAccumulator acc;
	// deadzone is handled in ScaleJoystickAxes() already
	if (rightStickX == 0 && rightStickY == 0) {
		acc.Clear();
		return;
	}

	if (IsAutomapActive()) { // move map
		int dx = 0;
		int dy = 0;
		acc.Pool(&dx, &dy, 32);
		AutomapOffset.deltaX += dy + dx;
		AutomapOffset.deltaY += dy - dx;
		return;
	}

	{ // move cursor
		sgbControllerActive = false;
		InvalidateInventorySlot();
		int x = MousePosition.x;
		int y = MousePosition.y;
		acc.Pool(&x, &y, 2);
		x = std::min(std::max(x, 0), gnScreenWidth - 1);
		y = std::min(std::max(y, 0), gnScreenHeight - 1);

		// We avoid calling `SetCursorPos` within the same SDL tick because
		// that can cause all stick motion events to arrive before all
		// cursor position events.
		static int lastMouseSetTick = 0;
		const int now = SDL_GetTicks();
		if (now - lastMouseSetTick > 0) {
			SetCursorPos({ x, y });
			lastMouseSetTick = now;
		}
	}
}

void InvalidateInventorySlot()
{
	Slot = -1;
}

/**
 * @brief Moves the mouse to the first inventory slot.
 */
void FocusOnInventory()
{
	Slot = SLOTXY_INV_FIRST;
	ResetInvCursorPosition();
}

void plrctrls_after_check_curs_move()
{
	// check for monsters first, then items, then towners.
	if (sgbControllerActive) {
		// Clear focuse set by cursor
		pcursplr = -1;
		pcursmonst = -1;
		pcursitem = -1;
		pcursobj = -1;
		pcursmissile = -1;
		pcurstrig = -1;
		pcursquest = Q_INVALID;
		cursPosition = { -1, -1 };
		if (Players[MyPlayerId]._pInvincible) {
			return;
		}
		if (DoomFlag) {
			return;
		}
		if (!invflag) {
			*infostr = '\0';
			ClearPanel();
			FindActor();
			FindItemOrObject();
			FindTrigger();
		}
	}
}

void plrctrls_every_frame()
{
	ProcessLeftStickOrDPadGameUI();
	HandleRightStickMotion();
}

void plrctrls_after_game_logic()
{
	Movement(MyPlayerId);
}

void UseBeltItem(int type)
{
	for (int i = 0; i < MAXBELTITEMS; i++) {
		auto &myPlayer = Players[MyPlayerId];
		const int id = AllItemsList[myPlayer.SpdList[i].IDidx].iMiscId;
		const int spellId = AllItemsList[myPlayer.SpdList[i].IDidx].iSpell;
		if ((type == BLT_HEALING && (id == IMISC_HEAL || id == IMISC_FULLHEAL || (id == IMISC_SCROLL && spellId == SPL_HEAL)))
		    || (type == BLT_MANA && (id == IMISC_MANA || id == IMISC_FULLMANA))
		    || id == IMISC_REJUV || id == IMISC_FULLREJUV) {
			if (!myPlayer.SpdList[i].isEmpty()) {
				UseInvItem(MyPlayerId, INVITEM_BELT_FIRST + i);
				break;
			}
		}
	}
}

void PerformPrimaryAction()
{
	if (invflag) { // inventory is open
		if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
			TryIconCurs();
			NewCursor(CURSOR_HAND);
		} else {
			int inventorySlot = (Slot >= 0) ? Slot : FindClosestInventorySlot(MousePosition);

			// Find any item occupying a slot that is currently under the cursor
			int8_t itemUnderCursor = [](int inventorySlot) {
				if (inventorySlot < SLOTXY_INV_FIRST || inventorySlot > SLOTXY_INV_LAST)
					return 0;
				for (int x = 0; x < icursSize28.width; x++) {
					for (int y = 0; y < icursSize28.height; y++) {
						int slotUnderCursor = inventorySlot + x + y * INV_ROW_SLOT_SIZE;
						if (slotUnderCursor > SLOTXY_INV_LAST)
							continue;
						int itemId = GetItemIdOnSlot(slotUnderCursor);
						if (itemId != 0)
							return itemId;
					}
				}
				return 0;
			}(inventorySlot);

			// The cursor will need to be shifted to
			// this slot if the item is swapped or lifted
			int jumpSlot = FindFirstSlotOnItem(itemUnderCursor);
			CheckInvItem();

			// If we don't find the item in the same position as before,
			// it suggests that the item was swapped or lifted
			int newSlot = FindFirstSlotOnItem(itemUnderCursor);
			if (jumpSlot >= 0 && jumpSlot != newSlot) {
				Point mousePos = GetSlotCoord(jumpSlot);
				mousePos.y -= InventorySlotSizeInPixels.height;
				Slot = jumpSlot;
				SetCursorPos(mousePos);
			}
		}
		return;
	}

	if (spselflag) {
		SetSpell();
		return;
	}

	if (chrflag && !chrbtnactive && Players[MyPlayerId]._pStatPts > 0) {
		CheckChrBtns();
		if (chrbtnactive)
			ReleaseChrBtns(false);
		return;
	}

	Interact();
}

bool SpellHasActorTarget()
{
	spell_id spl = Players[MyPlayerId]._pRSpell;
	if (spl == SPL_TOWN || spl == SPL_TELEPORT)
		return false;

	if (IsWallSpell(spl) && pcursmonst != -1) {
		cursPosition = Monsters[pcursmonst].position.tile;
	}

	return pcursplr != -1 || pcursmonst != -1;
}

void UpdateSpellTarget()
{
	if (SpellHasActorTarget())
		return;

	pcursplr = -1;
	pcursmonst = -1;

	auto &myPlayer = Players[MyPlayerId];

	int range = myPlayer._pRSpell == SPL_TELEPORT ? 4 : 1;

	cursPosition = myPlayer.position.future + Displacement(myPlayer._pdir) * range;
}

/**
 * @brief Try dropping item in all 9 possible places
 */
bool TryDropItem()
{
	const auto &myPlayer = Players[MyPlayerId];

	if (myPlayer.HoldItem.isEmpty()) {
		return false;
	}

	if (currlevel == 0) {
		if (UseItemOpensHive(myPlayer.HoldItem, myPlayer.position.tile)) {
			NetSendCmdPItem(true, CMD_PUTITEM, { 79, 61 });
			NewCursor(CURSOR_HAND);
			return true;
		}
		if (UseItemOpensCrypt(myPlayer.HoldItem, myPlayer.position.tile)) {
			NetSendCmdPItem(true, CMD_PUTITEM, { 35, 20 });
			NewCursor(CURSOR_HAND);
			return true;
		}
	}

	cursPosition = myPlayer.position.future + Direction::SouthEast;
	if (!DropItemBeforeTrig()) {
		// Try to drop on the other side
		cursPosition = myPlayer.position.future + Direction::SouthWest;
		DropItemBeforeTrig();
	}

	if (pcurs != CURSOR_HAND) {
		myPlayer.Say(HeroSpeech::WhereWouldIPutThis);
	}

	return pcurs == CURSOR_HAND;
}

void PerformSpellAction()
{
	if (InGameMenu() || QuestLogIsOpen || sbookflag)
		return;

	if (invflag) {
		if (pcurs >= CURSOR_FIRSTITEM)
			TryDropItem();
		else if (pcurs > CURSOR_HAND) {
			TryIconCurs();
			NewCursor(CURSOR_HAND);
		} else {
			int itemId = GetItemIdOnSlot(Slot);
			CheckInvItem(true, false);
			if (itemId != GetItemIdOnSlot(Slot))
				ResetInvCursorPosition();
		}
		return;
	}

	if (pcurs >= CURSOR_FIRSTITEM && !TryDropItem())
		return;
	if (pcurs > CURSOR_HAND)
		NewCursor(CURSOR_HAND);

	if (spselflag) {
		SetSpell();
		return;
	}

	const auto &myPlayer = Players[MyPlayerId];
	int spl = myPlayer._pRSpell;
	if ((pcursplr == -1 && (spl == SPL_RESURRECT || spl == SPL_HEALOTHER))
	    || (pcursobj == -1 && spl == SPL_DISARM)) {
		myPlayer.Say(HeroSpeech::ICantCastThatHere);
		return;
	}

	UpdateSpellTarget();
	CheckPlrSpell(false);
}

void CtrlUseInvItem()
{
	Item *item;

	if (pcursinvitem == -1)
		return;

	auto &myPlayer = Players[MyPlayerId];

	if (pcursinvitem < INVITEM_INV_FIRST)
		item = &myPlayer.InvBody[pcursinvitem];
	else if (pcursinvitem <= INVITEM_INV_LAST)
		item = &myPlayer.InvList[pcursinvitem - INVITEM_INV_FIRST];
	else
		item = &myPlayer.SpdList[pcursinvitem - INVITEM_BELT_FIRST];

	if (item->IsScroll() && spelldata[item->_iSpell].sTargeted) {
		return;
	}

	if (item->isEquipment()) {
		int itemId = GetItemIdOnSlot(Slot);
		CheckInvItem(true, false); // auto-equip if it's an equipment
		if (itemId != GetItemIdOnSlot(Slot))
			ResetInvCursorPosition();
	} else {
		UseInvItem(MyPlayerId, pcursinvitem);
	}
}

void PerformSecondaryAction()
{
	if (invflag) {
		if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
			TryIconCurs();
			NewCursor(CURSOR_HAND);
		} else {
			CtrlUseInvItem();
		}
		return;
	}

	if (pcurs >= CURSOR_FIRSTITEM && !TryDropItem())
		return;
	if (pcurs > CURSOR_HAND)
		NewCursor(CURSOR_HAND);

	if (pcursitem != -1) {
		NetSendCmdLocParam1(true, CMD_GOTOAGETITEM, cursPosition, pcursitem);
	} else if (pcursobj != -1) {
		NetSendCmdLocParam1(true, CMD_OPOBJXY, cursPosition, pcursobj);
	} else {
		auto &myPlayer = Players[MyPlayerId];
		if (pcursmissile != -1) {
			MakePlrPath(myPlayer, Missiles[pcursmissile].position.tile, true);
			myPlayer.destAction = ACTION_WALK;
		} else if (pcurstrig != -1) {
			MakePlrPath(myPlayer, trigs[pcurstrig].position, true);
			myPlayer.destAction = ACTION_WALK;
		} else if (pcursquest != Q_INVALID) {
			MakePlrPath(myPlayer, Quests[pcursquest].position, true);
			myPlayer.destAction = ACTION_WALK;
		}
	}
}

void QuickCast(int slot)
{
	auto &myPlayer = Players[MyPlayerId];

	CheckPlrSpell(false, myPlayer._pSplHotKey[slot], myPlayer._pSplTHotKey[slot]);
}

} // namespace devilution
