#include "controls/plrctrls.h"

#include <algorithm>
#include <cstdint>
#include <list>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "automap.h"
#include "control.h"
#include "controls/controller_motion.h"
#include "miniwin/misc_msg.h"
#ifndef USE_SDL1
#include "controls/devices/game_controller.h"
#endif
#include "controls/game_controls.h"
#include "controls/touch/gamepad.h"
#include "cursor.h"
#include "doom.h"
#include "engine/point.hpp"
#include "engine/points_in_rectangle_range.hpp"
#include "gmenu.h"
#include "help.h"
#include "hwcursor.hpp"
#include "inv.h"
#include "items.h"
#include "levels/trigs.h"
#include "minitext.h"
#include "missiles.h"
#include "panels/spell_list.hpp"
#include "panels/ui_panels.hpp"
#include "qol/chatlog.h"
#include "qol/stash.h"
#include "stores.h"
#include "towners.h"
#include "track.h"
#include "utils/log.hpp"
#include "utils/str_cat.hpp"

#define SPLICONLENGTH 56

namespace devilution {

ControlTypes ControlMode = ControlTypes::None;
ControlTypes ControlDevice = ControlTypes::None;
ControllerButton ControllerButtonHeld = ControllerButton_NONE;
GamepadLayout GamepadType = GamepadLayout::Generic;
int pcurstrig = -1;
Missile *pcursmissile = nullptr;
quest_id pcursquest = Q_INVALID;

/**
 * Native game menu, controlled by simulating a keyboard.
 */
bool InGameMenu()
{
	return stextflag != STORE_NONE
	    || HelpFlag
	    || ChatLogFlag
	    || talkflag
	    || qtextflag
	    || gmenu_is_active()
	    || PauseMode == 2
	    || (MyPlayer != nullptr && MyPlayer->_pInvincible && MyPlayer->_pHitPoints == 0);
}

namespace {

int Slot = SLOTXY_INV_FIRST;
Point ActiveStashSlot = InvalidStashPoint;
int PreviousInventoryColumn = -1;
bool BeltReturnsToStash = false;

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
	Player &myPlayer = *MyPlayer;

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
 * @param position Tile coordinates
 */
int GetMinDistance(Point position)
{
	return MyPlayer->position.future.WalkingDistance(position);
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

	int8_t walkpath[MaxPathLength];
	Player &myPlayer = *MyPlayer;
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
	return MyPlayer->position.future.ExactDistance(destination);
}

void FindItemOrObject()
{
	Point futurePosition = MyPlayer->position.future;
	int rotations = 5;

	auto searchArea = PointsInRectangleRangeColMajor { Rectangle { futurePosition, 1 } };

	for (Point targetPosition : searchArea) {
		// As the player can not stand on the edge of the map this is safe from OOB
		int8_t itemId = dItem[targetPosition.x][targetPosition.y] - 1;
		if (itemId < 0) {
			// there shouldn't be any items that occupy multiple ground tiles, but just in case only considering positive indexes here

			continue;
		}
		auto &item = Items[itemId];
		if (item.isEmpty() || item._iSelFlag == 0) {
			continue;
		}

		int newRotations = GetRotaryDistance(targetPosition);
		if (rotations < newRotations) {
			continue;
		}
		if (targetPosition != futurePosition && GetDistance(targetPosition, 1) == 0) {
			// Don't check the tile we're leaving if the player is walking
			continue;
		}
		rotations = newRotations;
		pcursitem = itemId;
		cursPosition = targetPosition;
	}

	if (leveltype == DTYPE_TOWN || pcursitem != -1) {
		return; // Don't look for objects in town
	}

	for (Point targetPosition : searchArea) {
		Object *object = FindObjectAtPosition(targetPosition);
		if (object == nullptr || object->_oSelFlag == 0) {
			// No object or non-interactive object
			continue;
		}
		if (targetPosition == futurePosition && object->_oDoorFlag) {
			continue; // Ignore doorway so we don't get stuck behind barrels
		}

		int newRotations = GetRotaryDistance(targetPosition);
		if (rotations < newRotations) {
			continue;
		}
		if (targetPosition != futurePosition && GetDistance(targetPosition, 1) == 0) {
			// Don't check the tile we're leaving if the player is walking
			continue;
		}
		if (object->IsDisabled()) {
			continue;
		}

		rotations = newRotations;
		ObjectUnderCursor = object;
		cursPosition = targetPosition;
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
	int spl = MyPlayer->_pRSpell;

	return spl != SPL_INVALID
	    && spl != SPL_TOWN
	    && spl != SPL_TELEPORT
	    && spelldata[spl].sTargeted
	    && !spelldata[spl].sTownSpell;
}

bool CanTargetMonster(const Monster &monster)
{
	if ((monster.flags & MFLAG_HIDDEN) != 0)
		return false;
	if (monster.isPlayerMinion())
		return false;
	if (monster.hitPoints >> 6 <= 0) // dead
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

	for (size_t i = 0; i < ActiveMonsterCount; i++) {
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

	Player &myPlayer = *MyPlayer;

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
	if (MyPlayer->UsesRangedWeapon() || HasRangedSpell()) {
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

	Player &myPlayer = *MyPlayer;

	int spl = myPlayer._pRSpell;
	if (myPlayer.friendlyMode && spl != SPL_RESURRECT && spl != SPL_HEALOTHER)
		return;

	for (size_t i = 0; i < Players.size(); i++) {
		const Player &player = Players[i];
		if (&player == MyPlayer)
			continue;
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

	if (pcursitem != -1 || ObjectUnderCursor != nullptr)
		return; // Prefer showing items/objects over triggers (use of cursm* conflicts)

	for (auto &missile : Missiles) {
		if (missile._mitype == MIS_TOWN || missile._mitype == MIS_RPORTAL) {
			const int newDistance = GetDistance(missile.position.tile, 2);
			if (newDistance == 0)
				continue;
			if (pcursmissile != nullptr && distance < newDistance)
				continue;
			const int newRotations = GetRotaryDistance(missile.position.tile);
			if (pcursmissile != nullptr && distance == newDistance && rotations < newRotations)
				continue;
			cursPosition = missile.position.tile;
			pcursmissile = &missile;
			distance = newDistance;
			rotations = newRotations;
		}
	}

	if (pcursmissile == nullptr) {
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
#ifndef USE_SDL1
	if (ControlMode == ControlTypes::VirtualGamepad) {
		stand = VirtualGamepadState.standButton.isHeld;
	}
#endif

	Player &myPlayer = *MyPlayer;

	if (leveltype != DTYPE_TOWN && stand) {
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

		NetSendCmdLoc(MyPlayerId, true, myPlayer.UsesRangedWeapon() ? CMD_RATTACKXY : CMD_SATTACKXY, position);
		LastMouseButtonAction = MouseActionType::Attack;
		return;
	}

	if (pcursmonst != -1) {
		if (!myPlayer.UsesRangedWeapon() || CanTalkToMonst(Monsters[pcursmonst])) {
			NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
		} else {
			NetSendCmdParam1(true, CMD_RATTACKID, pcursmonst);
		}
		LastMouseButtonAction = MouseActionType::AttackMonsterTarget;
		return;
	}

	if (leveltype != DTYPE_TOWN && pcursplr != -1 && !myPlayer.friendlyMode) {
		NetSendCmdParam1(true, myPlayer.UsesRangedWeapon() ? CMD_RATTACKPID : CMD_ATTACKPID, pcursplr);
		LastMouseButtonAction = MouseActionType::AttackPlayerTarget;
		return;
	}

	if (ObjectUnderCursor != nullptr) {
		NetSendCmdLoc(MyPlayerId, true, CMD_OPOBJXY, cursPosition);
		LastMouseButtonAction = MouseActionType::OperateObject;
		return;
	}
}

void AttrIncBtnSnap(AxisDirection dir)
{
	static AxisDirectionRepeater repeater;
	dir = repeater.Get(dir);
	if (dir.y == AxisDirectionY_NONE)
		return;

	if (chrbtnactive && MyPlayer->_pStatPts <= 0)
		return;

	// first, find our cursor location
	int slot = 0;
	Rectangle button;
	for (int i = 0; i < 4; i++) {
		button = ChrBtnsRect[i];
		button.position = GetPanelPosition(UiPanels::Character, button.position);
		if (button.contains(MousePosition)) {
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
	if (slot >= SLOTXY_HEAD_FIRST && slot <= SLOTXY_HEAD_LAST) {
		return InvGetEquipSlotCoord(INVLOC_HEAD);
	}
	if (slot == SLOTXY_RING_LEFT) {
		return InvGetEquipSlotCoord(INVLOC_RING_LEFT);
	}
	if (slot == SLOTXY_RING_RIGHT) {
		return InvGetEquipSlotCoord(INVLOC_RING_RIGHT);
	}
	if (slot == SLOTXY_AMULET) {
		return InvGetEquipSlotCoord(INVLOC_AMULET);
	}
	if (slot >= SLOTXY_HAND_LEFT_FIRST && slot <= SLOTXY_HAND_LEFT_LAST) {
		return InvGetEquipSlotCoord(INVLOC_HAND_LEFT);
	}
	if (slot >= SLOTXY_HAND_RIGHT_FIRST && slot <= SLOTXY_HAND_RIGHT_LAST) {
		return InvGetEquipSlotCoord(INVLOC_HAND_RIGHT);
	}
	if (slot >= SLOTXY_CHEST_FIRST && slot <= SLOTXY_CHEST_LAST) {
		return InvGetEquipSlotCoord(INVLOC_CHEST);
	}

	return {};
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
				return GetInventorySize(item);
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

Point FindFirstStashSlotOnItem(StashStruct::StashCell itemInvId)
{
	if (itemInvId == StashStruct::EmptyCell)
		return InvalidStashPoint;

	for (auto point : PointsInRectangleRange({ { 0, 0 }, Size { 10, 10 } })) {
		if (Stash.GetItemIdAtPosition(point) == itemInvId)
			return point;
	}

	return InvalidStashPoint;
}

/**
 * Reset cursor position based on the current slot.
 */
void ResetInvCursorPosition()
{
	Point mousePos {};
	if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_INV_LAST) {
		int8_t itemInvId = GetItemIdOnSlot(Slot);
		if (itemInvId != 0) {
			mousePos = GetSlotCoord(FindFirstSlotOnItem(itemInvId));
			Size itemSize = GetItemSizeOnSlot(Slot);
			mousePos.x += ((itemSize.width - 1) * InventorySlotSizeInPixels.width) / 2;
			mousePos.y += ((itemSize.height - 1) * InventorySlotSizeInPixels.height) / 2;
		} else {
			mousePos = GetSlotCoord(Slot);
		}

		if (!MyPlayer->HoldItem.isEmpty()) {
			mousePos += Displacement { -INV_SLOT_HALF_SIZE_PX, -INV_SLOT_HALF_SIZE_PX };
		}
	} else if (Slot >= SLOTXY_BELT_FIRST && Slot <= SLOTXY_BELT_LAST) {
		mousePos = GetSlotCoord(Slot);
		if (!MyPlayer->HoldItem.isEmpty())
			mousePos += Displacement { -INV_SLOT_HALF_SIZE_PX, -INV_SLOT_HALF_SIZE_PX };
	} else {
		mousePos = InvGetEquipSlotCoordFromInvSlot((inv_xy_slot)Slot);
		if (!MyPlayer->HoldItem.isEmpty()) {
			Size itemSize = GetInventorySize(MyPlayer->HoldItem);
			mousePos += Displacement { -INV_SLOT_HALF_SIZE_PX, -INV_SLOT_HALF_SIZE_PX * itemSize.height };
		}
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

Point FindClosestStashSlot(Point mousePos)
{
	int shortestDistance = std::numeric_limits<int>::max();
	Point bestSlot = {};
	mousePos += Displacement { -INV_SLOT_HALF_SIZE_PX, -INV_SLOT_HALF_SIZE_PX };

	for (auto point : PointsInRectangleRange({ { 0, 0 }, Size { 10, 10 } })) {
		int distance = mousePos.ManhattanDistance(GetStashSlotCoord(point));
		if (distance < shortestDistance) {
			shortestDistance = distance;
			bestSlot = point;
		}
	}

	return bestSlot;
}

/**
 * @brief Figures out where on the body to move when on the first row
 */
inv_xy_slot InventoryMoveToBody(int slot)
{
	PreviousInventoryColumn = slot - SLOTXY_INV_ROW1_FIRST;
	if (slot <= SLOTXY_INV_ROW1_FIRST + 2) { // first 3 general slots
		return SLOTXY_RING_LEFT;
	}
	if (slot <= SLOTXY_INV_ROW1_FIRST + 6) { // middle 4 general slots
		return SLOTXY_CHEST_FIRST;
	}
	// last 3 general slots
	return SLOTXY_RING_RIGHT;
}

void InventoryMove(AxisDirection dir)
{
	Point mousePos = MousePosition;

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

	const Item &heldItem = MyPlayer->HoldItem;
	const bool isHoldingItem = !heldItem.isEmpty();
	Size itemSize = GetInventorySize(heldItem);

	// when item is on cursor (pcurs > 1), this is the real cursor XY
	if (dir.x == AxisDirectionX_LEFT) {
		if (isHoldingItem) {
			if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_BELT_LAST) {
				if (IsNoneOf(Slot, SLOTXY_INV_ROW1_FIRST, SLOTXY_INV_ROW2_FIRST, SLOTXY_INV_ROW3_FIRST, SLOTXY_INV_ROW4_FIRST, SLOTXY_BELT_FIRST)) {
					Slot -= 1;
				}
			} else if (heldItem._itype == ItemType::Ring) {
				Slot = SLOTXY_RING_LEFT;
			} else if (heldItem.isWeapon() || heldItem.isShield()) {
				Slot = SLOTXY_HAND_LEFT_FIRST;
			}
		} else {
			if (Slot == SLOTXY_HAND_RIGHT_FIRST) {
				Slot = SLOTXY_CHEST_FIRST;
			} else if (Slot == SLOTXY_CHEST_FIRST) {
				Slot = SLOTXY_HAND_LEFT_FIRST;
			} else if (Slot == SLOTXY_AMULET) {
				Slot = SLOTXY_HEAD_FIRST;
			} else if (Slot == SLOTXY_RING_RIGHT) {
				Slot = SLOTXY_RING_LEFT;
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
			}
		}
	} else if (dir.x == AxisDirectionX_RIGHT) {
		if (isHoldingItem) {
			if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_BELT_LAST) {
				if (IsNoneOf(Slot + itemSize.width - 1, SLOTXY_INV_ROW1_LAST, SLOTXY_INV_ROW2_LAST, SLOTXY_INV_ROW3_LAST, SLOTXY_INV_ROW4_LAST, SLOTXY_BELT_LAST)) {
					Slot += 1;
				}
			} else if (heldItem._itype == ItemType::Ring) {
				Slot = SLOTXY_RING_RIGHT;
			} else if (heldItem.isWeapon() || heldItem.isShield()) {
				Slot = SLOTXY_HAND_RIGHT_FIRST;
			}
		} else {
			if (Slot == SLOTXY_RING_LEFT) {
				Slot = SLOTXY_RING_RIGHT;
			} else if (Slot == SLOTXY_HAND_LEFT_FIRST) {
				Slot = SLOTXY_CHEST_FIRST;
			} else if (Slot == SLOTXY_CHEST_FIRST) {
				Slot = SLOTXY_HAND_RIGHT_FIRST;
			} else if (Slot == SLOTXY_HEAD_FIRST) {
				Slot = SLOTXY_AMULET;
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
			}
		}
	}
	if (dir.y == AxisDirectionY_UP) {
		if (isHoldingItem) {
			if (Slot >= SLOTXY_INV_ROW2_FIRST) { // general inventory
				Slot -= INV_ROW_SLOT_SIZE;
			} else if (Slot >= SLOTXY_INV_FIRST) {
				if (heldItem._itype == ItemType::Ring) {
					if (Slot >= SLOTXY_INV_ROW1_FIRST && Slot <= SLOTXY_INV_ROW1_FIRST + (INV_ROW_SLOT_SIZE / 2) - 1) {
						Slot = SLOTXY_RING_LEFT;
					} else {
						Slot = SLOTXY_RING_RIGHT;
					}
				} else if (heldItem.isWeapon()) {
					Slot = SLOTXY_HAND_LEFT_FIRST;
				} else if (heldItem.isShield()) {
					Slot = SLOTXY_HAND_RIGHT_FIRST;
				} else if (heldItem.isHelm()) {
					Slot = SLOTXY_HEAD_FIRST;
				} else if (heldItem.isArmor()) {
					Slot = SLOTXY_CHEST_FIRST;
				} else if (heldItem._itype == ItemType::Amulet) {
					Slot = SLOTXY_AMULET;
				}
			}
		} else {
			if (Slot >= SLOTXY_INV_ROW1_FIRST && Slot <= SLOTXY_INV_ROW1_LAST) {
				Slot = InventoryMoveToBody(Slot);
			} else if (Slot == SLOTXY_CHEST_FIRST || Slot == SLOTXY_HAND_LEFT_FIRST) {
				Slot = SLOTXY_HEAD_FIRST;
			} else if (Slot == SLOTXY_RING_LEFT) {
				Slot = SLOTXY_HAND_LEFT_FIRST;
			} else if (Slot == SLOTXY_RING_RIGHT) {
				Slot = SLOTXY_HAND_RIGHT_FIRST;
			} else if (Slot == SLOTXY_HAND_RIGHT_FIRST) {
				Slot = SLOTXY_AMULET;
			} else if (Slot >= SLOTXY_INV_ROW2_FIRST) {
				int8_t itemId = GetItemIdOnSlot(Slot);
				if (itemId != 0) {
					for (int i = 1; i < 5; i++) {
						if (Slot - i * INV_ROW_SLOT_SIZE < SLOTXY_INV_ROW1_FIRST) {
							Slot = InventoryMoveToBody(Slot - (i - 1) * INV_ROW_SLOT_SIZE);
							break;
						}
						if (itemId != GetItemIdOnSlot(Slot - i * INV_ROW_SLOT_SIZE)) {
							Slot -= i * INV_ROW_SLOT_SIZE;
							break;
						}
					}
				} else {
					Slot -= INV_ROW_SLOT_SIZE;
				}
			}
		}
	} else if (dir.y == AxisDirectionY_DOWN) {
		if (isHoldingItem) {
			if (Slot == SLOTXY_HEAD_FIRST || Slot == SLOTXY_CHEST_FIRST) {
				Slot = SLOTXY_INV_ROW1_FIRST + 4;
			} else if (Slot == SLOTXY_RING_LEFT || Slot == SLOTXY_HAND_LEFT_FIRST) {
				Slot = SLOTXY_INV_ROW1_FIRST + 1;
			} else if (Slot == SLOTXY_RING_RIGHT || Slot == SLOTXY_HAND_RIGHT_FIRST || Slot == SLOTXY_AMULET) {
				Slot = SLOTXY_INV_ROW1_LAST - 1;
			} else if (Slot <= (SLOTXY_INV_ROW4_LAST - (itemSize.height * INV_ROW_SLOT_SIZE))) {
				Slot += INV_ROW_SLOT_SIZE;
			} else if (Slot <= SLOTXY_INV_LAST && heldItem._itype == ItemType::Misc && itemSize == Size { 1, 1 }) { // forcing only 1x1 misc items
				if (Slot + INV_ROW_SLOT_SIZE <= SLOTXY_BELT_LAST)
					Slot += INV_ROW_SLOT_SIZE;
			}
		} else {
			if (Slot == SLOTXY_HEAD_FIRST) {
				Slot = SLOTXY_CHEST_FIRST;
			} else if (Slot == SLOTXY_CHEST_FIRST) {
				if (PreviousInventoryColumn >= 3 && PreviousInventoryColumn <= 6)
					Slot = SLOTXY_INV_ROW1_FIRST + PreviousInventoryColumn;
				else
					Slot = SLOTXY_INV_ROW1_FIRST + (INV_ROW_SLOT_SIZE / 2);
			} else if (Slot == SLOTXY_HAND_LEFT_FIRST) {
				Slot = SLOTXY_RING_LEFT;
			} else if (Slot == SLOTXY_RING_LEFT) {
				if (PreviousInventoryColumn >= 0 && PreviousInventoryColumn <= 2)
					Slot = SLOTXY_INV_ROW1_FIRST + PreviousInventoryColumn;
				else
					Slot = SLOTXY_INV_ROW1_FIRST + 1;
			} else if (Slot == SLOTXY_RING_RIGHT) {
				if (PreviousInventoryColumn >= 7 && PreviousInventoryColumn <= 9)
					Slot = SLOTXY_INV_ROW1_FIRST + PreviousInventoryColumn;
				else
					Slot = SLOTXY_INV_ROW1_LAST - 1;
			} else if (Slot == SLOTXY_AMULET) {
				Slot = SLOTXY_HAND_RIGHT_FIRST;
			} else if (Slot == SLOTXY_HAND_RIGHT_FIRST) {
				Slot = SLOTXY_RING_RIGHT;
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
			}
		}
	}

	// no movement was made
	if (Slot == initialSlot)
		return;

	if (Slot < SLOTXY_INV_FIRST) {
		mousePos = InvGetEquipSlotCoordFromInvSlot(static_cast<inv_xy_slot>(Slot));
	} else {
		mousePos = GetSlotCoord(Slot);
	}
	// move cursor to the center of the slot if not holding anything or top left is holding an object
	if (isHoldingItem) {
		if (Slot < SLOTXY_INV_FIRST) {
			// The coordinates we get for body slots are based on the centre of the region relative to the hand cursor
			// Need to adjust the position for items larger than 1x1 so they're aligned as expected
			mousePos.x -= (itemSize.width - 1) * INV_SLOT_HALF_SIZE_PX;
			mousePos.y -= (itemSize.height - 1) * INV_SLOT_HALF_SIZE_PX;
		}
		// Also the y position is off... so shift the mouse a cell up to compensate.
		mousePos.y -= InventorySlotSizeInPixels.height;
	} else {
		// get item under new slot if navigating on the inventory
		if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_INV_LAST) {
			int8_t itemInvId = GetItemIdOnSlot(Slot);
			int itemSlot = FindFirstSlotOnItem(itemInvId);
			if (itemSlot < 0)
				itemSlot = Slot;

			// offset the slot to always move to the top-left most slot of that item
			mousePos = GetSlotCoord(itemSlot);
			itemSize = GetItemSizeOnSlot(itemSlot);
			mousePos.x += ((itemSize.width - 1) * InventorySlotSizeInPixels.width) / 2;
			mousePos.y += ((itemSize.height - 1) * InventorySlotSizeInPixels.height) / 2;
		}

		mousePos.x += (InventorySlotSizeInPixels.width / 2);
		mousePos.y -= (InventorySlotSizeInPixels.height / 2);
	}

	if (mousePos == MousePosition) {
		return; // Avoid wobeling when scalled
	}

	SetCursorPos(mousePos);
}

/**
 * Move the cursor around in the inventory
 * If mouse coords are at SLOTXY_CHEST_LAST, consider this center of equipment
 * small inventory squares are 29x29 (roughly)
 */
void CheckInventoryMove(AxisDirection dir)
{
	static AxisDirectionRepeater repeater(/*min_interval_ms=*/150);
	dir = repeater.Get(dir);
	if (dir.x == AxisDirectionX_NONE && dir.y == AxisDirectionY_NONE)
		return;

	InventoryMove(dir);
}

void StashMove(AxisDirection dir)
{
	static AxisDirectionRepeater repeater(/*min_interval_ms=*/150);
	dir = repeater.Get(dir);
	if (dir.x == AxisDirectionX_NONE && dir.y == AxisDirectionY_NONE)
		return;

	if (Slot < 0 && ActiveStashSlot == InvalidStashPoint) {
		int invSlot = FindClosestInventorySlot(MousePosition);
		Point invSlotCoord = GetSlotCoord(invSlot);
		int invDistance = MousePosition.ManhattanDistance(invSlotCoord);

		Point stashSlot = FindClosestStashSlot(MousePosition);
		Point stashSlotCoord = GetStashSlotCoord(stashSlot);
		int stashDistance = MousePosition.ManhattanDistance(stashSlotCoord);

		if (invDistance < stashDistance) {
			BeltReturnsToStash = false;
			InventoryMove(dir);
			return;
		}

		ActiveStashSlot = stashSlot;
	}

	Item &holdItem = MyPlayer->HoldItem;
	Size itemSize = holdItem.isEmpty() ? Size { 1, 1 } : GetInventorySize(holdItem);

	// Jump from belt to stash
	if (BeltReturnsToStash && Slot >= SLOTXY_BELT_FIRST && Slot <= SLOTXY_BELT_LAST) {
		if (dir.y == AxisDirectionY_UP) {
			int beltSlot = Slot - SLOTXY_BELT_FIRST;
			InvalidateInventorySlot();
			ActiveStashSlot = { 2 + beltSlot, 10 - itemSize.height };
			dir.y = AxisDirectionY_NONE;
		}
	}

	// Jump from general inventory to stash
	if (Slot >= SLOTXY_INV_FIRST && Slot <= SLOTXY_INV_LAST) {
		int firstSlot = Slot;
		if (MyPlayer->HoldItem.isEmpty()) {
			int8_t itemId = GetItemIdOnSlot(Slot);
			if (itemId != 0) {
				firstSlot = FindFirstSlotOnItem(itemId);
			}
		}
		if (IsAnyOf(firstSlot, SLOTXY_INV_ROW1_FIRST, SLOTXY_INV_ROW2_FIRST, SLOTXY_INV_ROW3_FIRST, SLOTXY_INV_ROW4_FIRST)) {
			if (dir.x == AxisDirectionX_LEFT) {
				Point slotCoord = GetSlotCoord(Slot);
				InvalidateInventorySlot();
				ActiveStashSlot = FindClosestStashSlot(slotCoord) - Displacement { itemSize.width - 1, 0 };
				dir.x = AxisDirectionX_NONE;
			}
		}
	}

	bool isHeadSlot = SLOTXY_HEAD_FIRST <= Slot && Slot <= SLOTXY_HEAD_LAST;
	bool isLeftHandSlot = SLOTXY_HAND_LEFT_FIRST <= Slot && Slot <= SLOTXY_HAND_LEFT_LAST;
	bool isLeftRingSlot = Slot == SLOTXY_RING_LEFT;
	if (isHeadSlot || isLeftHandSlot || isLeftRingSlot) {
		if (dir.x == AxisDirectionX_LEFT) {
			Point slotCoord = GetSlotCoord(Slot);
			InvalidateInventorySlot();
			ActiveStashSlot = FindClosestStashSlot(slotCoord) - Displacement { itemSize.width - 1, 0 };
			dir.x = AxisDirectionX_NONE;
		}
	}

	if (Slot >= 0) {
		InventoryMove(dir);
		return;
	}

	if (dir.x == AxisDirectionX_LEFT) {
		if (ActiveStashSlot.x > 0)
			ActiveStashSlot.x--;
	} else if (dir.x == AxisDirectionX_RIGHT) {
		if (ActiveStashSlot.x < 10 - itemSize.width) {
			ActiveStashSlot.x++;
		} else {
			Point stashSlotCoord = GetStashSlotCoord(ActiveStashSlot);
			Point rightPanelCoord = { GetRightPanel().position.x, stashSlotCoord.y };
			Slot = FindClosestInventorySlot(rightPanelCoord);
			ActiveStashSlot = InvalidStashPoint;
			BeltReturnsToStash = false;
		}
	}
	if (dir.y == AxisDirectionY_UP) {
		if (ActiveStashSlot.y > 0)
			ActiveStashSlot.y--;
	} else if (dir.y == AxisDirectionY_DOWN) {
		if (ActiveStashSlot.y < 10 - itemSize.height) {
			ActiveStashSlot.y++;
		} else if ((holdItem.isEmpty() || CanBePlacedOnBelt(holdItem)) && ActiveStashSlot.x > 1) {
			int beltSlot = ActiveStashSlot.x - 2;
			Slot = SLOTXY_BELT_FIRST + beltSlot;
			ActiveStashSlot = InvalidStashPoint;
			BeltReturnsToStash = true;
		}
	}

	if (Slot >= 0) {
		ResetInvCursorPosition();
		return;
	}

	if (ActiveStashSlot != InvalidStashPoint) {
		Point mousePos = GetStashSlotCoord(ActiveStashSlot);
		if (pcurs == CURSOR_HAND) {
			mousePos += Displacement { INV_SLOT_HALF_SIZE_PX, INV_SLOT_HALF_SIZE_PX };
		}
		SetCursorPos(mousePos);
		return;
	}

	FocusOnInventory();
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

	Player &myPlayer = *MyPlayer;

	return !PosOkPlayer(myPlayer, leftStep) && !PosOkPlayer(myPlayer, rightStep);
}

void WalkInDir(size_t playerId, AxisDirection dir)
{
	Player &player = Players[playerId];

	if (dir.x == AxisDirectionX_NONE && dir.y == AxisDirectionY_NONE) {
		if (ControlMode != ControlTypes::KeyboardAndMouse && player.walkpath[0] != WALK_NONE && player.destAction == ACTION_NONE)
			NetSendCmdLoc(playerId, true, CMD_WALKXY, player.position.future); // Stop walking
		return;
	}

	const Direction pdir = FaceDir[static_cast<std::size_t>(dir.x)][static_cast<std::size_t>(dir.y)];
	const auto delta = player.position.future + pdir;

	if (!player.IsWalking() && player.CanChangeAction())
		player._pdir = pdir;

#ifndef USE_SDL1
	if (ControlMode == ControlTypes::VirtualGamepad) {
		if (VirtualGamepadState.standButton.isHeld) {
			if (player._pmode == PM_STAND)
				StartStand(player, pdir);
			return;
		}
	}
#endif

	if (PosOkPlayer(player, delta) && IsPathBlocked(player.position.future, pdir)) {
		if (player._pmode == PM_STAND)
			StartStand(player, pdir);
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
	if (IsStashOpen) {
		return &StashMove;
	}
	if (invflag) {
		return &CheckInventoryMove;
	}
	if (chrflag && MyPlayer->_pStatPts > 0) {
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

void Movement(size_t playerId)
{
	if (InGameMenu()
	    || IsControllerButtonPressed(ControllerButton_BUTTON_START)
	    || IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
		return;

	if (GetLeftStickOrDPadGameUIHandler() == nullptr) {
		WalkInDir(playerId, GetMoveDirection());
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

bool IsStickMovementSignificant()
{
	return leftStickX >= 0.5 || leftStickX <= -0.5
	    || leftStickY >= 0.5 || leftStickY <= -0.5
	    || rightStickX != 0 || rightStickY != 0;
}

ControlTypes GetInputTypeFromEvent(const SDL_Event &event)
{
	if (IsAnyOf(event.type, SDL_KEYDOWN, SDL_KEYUP))
		return ControlTypes::KeyboardAndMouse;
#ifdef USE_SDL1
	if (IsAnyOf(event.type, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP, SDL_MOUSEMOTION))
		return ControlTypes::KeyboardAndMouse;
#else
	if (IsAnyOf(event.type, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP))
		return event.button.which == SDL_TOUCH_MOUSEID ? ControlTypes::VirtualGamepad : ControlTypes::KeyboardAndMouse;
	if (event.type == SDL_MOUSEMOTION)
		return event.motion.which == SDL_TOUCH_MOUSEID ? ControlTypes::VirtualGamepad : ControlTypes::KeyboardAndMouse;
	if (event.type == SDL_MOUSEWHEEL)
		return event.wheel.which == SDL_TOUCH_MOUSEID ? ControlTypes::VirtualGamepad : ControlTypes::KeyboardAndMouse;
	if (IsAnyOf(event.type, SDL_FINGERDOWN, SDL_FINGERUP, SDL_FINGERMOTION))
		return ControlTypes::VirtualGamepad;
	if (event.type == SDL_CONTROLLERAXISMOTION
	    && (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT || event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT
	        || IsStickMovementSignificant()))
		return ControlTypes::Gamepad;
	if (event.type >= SDL_CONTROLLERBUTTONDOWN && event.type <= SDL_CONTROLLERDEVICEREMAPPED)
		return ControlTypes::Gamepad;
	if (IsAnyOf(event.type, SDL_JOYDEVICEADDED, SDL_JOYDEVICEREMOVED))
		return ControlTypes::Gamepad;
#endif
	if (event.type == SDL_JOYAXISMOTION && IsStickMovementSignificant())
		return ControlTypes::Gamepad;
	if (event.type >= SDL_JOYBALLMOTION && event.type <= SDL_JOYBUTTONUP)
		return ControlTypes::Gamepad;

	return ControlTypes::None;
}

float rightStickLastMove = 0;

bool ContinueSimulatedMouseEvent(const SDL_Event &event, const ControllerButtonEvent &gamepadEvent)
{
	if (IsAutomapActive())
		return false;

#if !defined(USE_SDL1) && !defined(JOY_AXIS_RIGHTX) && !defined(JOY_AXIS_RIGHTY)
	if (IsAnyOf(event.type, SDL_JOYAXISMOTION, SDL_JOYHATMOTION, SDL_JOYBUTTONDOWN, SDL_JOYBUTTONUP)) {
		if (!GameController::All().empty())
			return true;
	}
#endif

	if (rightStickX != 0 || rightStickY != 0 || rightStickLastMove != 0) {
		rightStickLastMove = rightStickX + rightStickY; // Allow stick to come to a rest with out breaking simulation
		return true;
	}

	return SimulatingMouseWithSelectAndDPad || IsSimulatedMouseClickBinding(gamepadEvent);
}

string_view ControlTypeToString(ControlTypes controlType)
{
	switch (controlType) {
	case ControlTypes::None:
		return "None";
	case ControlTypes::KeyboardAndMouse:
		return "KeyboardAndMouse";
	case ControlTypes::Gamepad:
		return "Gamepad";
	case ControlTypes::VirtualGamepad:
		return "VirtualGamepad";
	}
	return "Invalid";
}

void LogControlDeviceAndModeChange(ControlTypes newControlDevice, ControlTypes newControlMode)
{
	if (SDL_LOG_PRIORITY_VERBOSE < SDL_LogGetPriority(SDL_LOG_CATEGORY_APPLICATION))
		return;
	if (newControlDevice == ControlDevice && newControlMode == ControlMode)
		return;
	constexpr auto DebugChange = [](ControlTypes before, ControlTypes after) -> std::string {
		if (before == after)
			return std::string { ControlTypeToString(before) };
		return StrCat(ControlTypeToString(before), " -> ", ControlTypeToString(after));
	};
	LogVerbose("Control: device {}, mode {}", DebugChange(ControlDevice, newControlDevice), DebugChange(ControlMode, newControlMode));
}

#ifndef USE_SDL1
string_view GamepadTypeToString(GamepadLayout gamepadLayout)
{
	switch (gamepadLayout) {
	case GamepadLayout::Nintendo:
		return "Nintendo";
	case GamepadLayout::PlayStation:
		return "PlayStation";
	case GamepadLayout::Xbox:
		return "Xbox";
	case GamepadLayout::Generic:
		return "Unknown";
	}
	return "Invalid";
}

void LogGamepadChange(GamepadLayout newGamepad)
{
	if (SDL_LOG_PRIORITY_VERBOSE < SDL_LogGetPriority(SDL_LOG_CATEGORY_APPLICATION))
		return;
	constexpr auto DebugChange = [](GamepadLayout before, GamepadLayout after) -> std::string {
		if (before == after)
			return std::string { GamepadTypeToString(before) };
		return StrCat(GamepadTypeToString(before), " -> ", GamepadTypeToString(after));
	};
	LogVerbose("Control: gamepad {}", DebugChange(GamepadType, newGamepad));
}
#endif

} // namespace

void DetectInputMethod(const SDL_Event &event, const ControllerButtonEvent &gamepadEvent)
{
	ControlTypes inputType = GetInputTypeFromEvent(event);

	if (inputType == ControlTypes::None)
		return;

#ifdef __vita__
	if (inputType == ControlTypes::VirtualGamepad) {
		inputType = ControlTypes::Gamepad;
	}
#endif

#if HAS_KBCTRL == 1
	if (inputType == ControlTypes::KeyboardAndMouse && IsNoneOf(gamepadEvent.button, ControllerButton_NONE, ControllerButton_IGNORE)) {
		inputType = ControlTypes::Gamepad;
	}
#endif

	ControlTypes newControlDevice = inputType;
	ControlTypes newControlMode = inputType;
	if (ContinueSimulatedMouseEvent(event, gamepadEvent)) {
		newControlMode = ControlMode;
	}

	LogControlDeviceAndModeChange(newControlDevice, newControlMode);

	if (newControlDevice != ControlDevice) {
		ControlDevice = newControlDevice;

#ifndef USE_SDL1
		if (ControlDevice != ControlTypes::KeyboardAndMouse) {
			if (IsHardwareCursor())
				SetHardwareCursor(CursorInfo::UnknownCursor());
		} else {
			ResetCursor();
		}
		if (ControlDevice == ControlTypes::Gamepad) {
			GamepadLayout newGamepadLayout = GameController::getLayout(event);
			if (newGamepadLayout != GamepadType) {
				LogGamepadChange(newGamepadLayout);
				GamepadType = newGamepadLayout;
			}
		}
#endif
	}

	if (newControlMode != ControlMode) {
		ControlMode = newControlMode;
		CalculatePanelAreas();
	}
}

bool IsAutomapActive()
{
	return AutomapActive && leveltype != DTYPE_TOWN;
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
			ResetCursor();
			SetCursorPos({ x, y });
			LogControlDeviceAndModeChange(ControlDevice, ControlTypes::KeyboardAndMouse);

			ControlMode = ControlTypes::KeyboardAndMouse;
			lastMouseSetTick = now;
		}
	}
}

void InvalidateInventorySlot()
{
	Slot = -1;
	ActiveStashSlot = InvalidStashPoint;
}

/**
 * @brief Moves the mouse to the first inventory slot.
 */
void FocusOnInventory()
{
	Slot = SLOTXY_INV_FIRST;
	ResetInvCursorPosition();
}

bool PointAndClickState = false;

void SetPointAndClick(bool value)
{
	PointAndClickState = value;
}

bool IsPointAndClick()
{
	return PointAndClickState;
}

void plrctrls_after_check_curs_move()
{
	// check for monsters first, then items, then towners.
	if (ControlMode == ControlTypes::KeyboardAndMouse || IsPointAndClick()) {
		return;
	}

	// While holding the button down we should retain target (but potentially lose it if it dies, goes out of view, etc)
	if (ControllerButtonHeld != ControllerButton_NONE && IsNoneOf(LastMouseButtonAction, MouseActionType::None, MouseActionType::Attack, MouseActionType::Spell)) {
		InvalidateTargets();

		if (pcursmonst == -1 && ObjectUnderCursor == nullptr && pcursitem == -1 && pcursinvitem == -1 && pcursstashitem == uint16_t(-1) && pcursplr == -1) {
			FindTrigger();
		}
		return;
	}

	// Clear focuse set by cursor
	pcursplr = -1;
	pcursmonst = -1;
	pcursitem = -1;
	ObjectUnderCursor = nullptr;

	pcursmissile = nullptr;
	pcurstrig = -1;
	pcursquest = Q_INVALID;
	cursPosition = { -1, -1 };
	if (MyPlayer->_pInvincible) {
		return;
	}
	if (DoomFlag) {
		return;
	}
	if (!invflag) {
		InfoString = {};
		ClearPanel();
		FindActor();
		FindItemOrObject();
		FindTrigger();
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
	for (int i = 0; i < MaxBeltItems; i++) {
		Item &item = MyPlayer->SpdList[i];
		if (item.isEmpty()) {
			continue;
		}

		bool isRejuvenation = IsAnyOf(item._iMiscId, IMISC_REJUV, IMISC_FULLREJUV);
		bool isHealing = isRejuvenation || IsAnyOf(item._iMiscId, IMISC_HEAL, IMISC_FULLHEAL) || item.isScrollOf(SPL_HEAL);
		bool isMana = isRejuvenation || IsAnyOf(item._iMiscId, IMISC_MANA, IMISC_FULLMANA);

		if ((type == BLT_HEALING && isHealing) || (type == BLT_MANA && isMana)) {
			UseInvItem(MyPlayerId, INVITEM_BELT_FIRST + i);
			break;
		}
	}
}

void PerformPrimaryAction()
{
	if (invflag) { // inventory is open
		if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
			TryIconCurs();
			NewCursor(CURSOR_HAND);
		} else if (GetRightPanel().contains(MousePosition) || GetMainPanel().contains(MousePosition)) {
			int inventorySlot = (Slot >= 0) ? Slot : FindClosestInventorySlot(MousePosition);

			const Size cursorSizeInCells = MyPlayer->HoldItem.isEmpty() ? Size { 1, 1 } : GetInventorySize(MyPlayer->HoldItem);

			// Find any item occupying a slot that is currently under the cursor
			int8_t itemUnderCursor = [](int inventorySlot, Size cursorSizeInCells) {
				if (inventorySlot < SLOTXY_INV_FIRST || inventorySlot > SLOTXY_INV_LAST)
					return 0;
				for (int x = 0; x < cursorSizeInCells.width; x++) {
					for (int y = 0; y < cursorSizeInCells.height; y++) {
						int slotUnderCursor = inventorySlot + x + y * INV_ROW_SLOT_SIZE;
						if (slotUnderCursor > SLOTXY_INV_LAST)
							continue;
						int itemId = GetItemIdOnSlot(slotUnderCursor);
						if (itemId != 0)
							return itemId;
					}
				}
				return 0;
			}(inventorySlot, cursorSizeInCells);

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
		} else if (IsStashOpen && GetLeftPanel().contains(MousePosition)) {
			Point stashSlot = (ActiveStashSlot != InvalidStashPoint) ? ActiveStashSlot : FindClosestStashSlot(MousePosition);

			const Size cursorSizeInCells = MyPlayer->HoldItem.isEmpty() ? Size { 1, 1 } : GetInventorySize(MyPlayer->HoldItem);

			// Find any item occupying a slot that is currently under the cursor
			StashStruct::StashCell itemUnderCursor = [](Point stashSlot, Size cursorSizeInCells) -> StashStruct::StashCell {
				if (stashSlot != InvalidStashPoint)
					return StashStruct::EmptyCell;
				for (Point slotUnderCursor : PointsInRectangleRange { { stashSlot, cursorSizeInCells } }) {
					if (slotUnderCursor.x >= 10 || slotUnderCursor.y >= 10)
						continue;
					StashStruct::StashCell itemId = Stash.GetItemIdAtPosition(slotUnderCursor);
					if (itemId != StashStruct::EmptyCell)
						return itemId;
				}
				return StashStruct::EmptyCell;
			}(stashSlot, cursorSizeInCells);

			// The cursor will need to be shifted to
			// this slot if the item is swapped or lifted
			Point jumpSlot = FindFirstStashSlotOnItem(itemUnderCursor);
			CheckStashItem(MousePosition);

			// If we don't find the item in the same position as before,
			// it suggests that the item was swapped or lifted
			Point newSlot = FindFirstStashSlotOnItem(itemUnderCursor);
			if (jumpSlot != InvalidStashPoint && jumpSlot != newSlot) {
				Point mousePos = GetStashSlotCoord(jumpSlot);
				mousePos.y -= InventorySlotSizeInPixels.height;
				ActiveStashSlot = jumpSlot;
				SetCursorPos(mousePos);
			}
		}
		return;
	}

	if (spselflag) {
		SetSpell();
		return;
	}

	if (chrflag && !chrbtnactive && MyPlayer->_pStatPts > 0) {
		CheckChrBtns();
		if (chrbtnactive)
			ReleaseChrBtns(false);
		return;
	}

	Interact();
}

bool SpellHasActorTarget()
{
	spell_id spl = MyPlayer->_pRSpell;
	if (spl == SPL_TOWN || spl == SPL_TELEPORT)
		return false;

	if (IsWallSpell(spl) && pcursmonst != -1) {
		cursPosition = Monsters[pcursmonst].position.tile;
	}

	return pcursplr != -1 || pcursmonst != -1;
}

void UpdateSpellTarget(spell_id spell)
{
	if (SpellHasActorTarget())
		return;

	pcursplr = -1;
	pcursmonst = -1;

	Player &myPlayer = *MyPlayer;

	int range = spell == SPL_TELEPORT ? 4 : 1;

	cursPosition = myPlayer.position.future + Displacement(myPlayer._pdir) * range;
}

/**
 * @brief Try dropping item in all 9 possible places
 */
bool TryDropItem()
{
	Player &myPlayer = *MyPlayer;

	if (myPlayer.HoldItem.isEmpty()) {
		return false;
	}

	if (leveltype == DTYPE_TOWN) {
		if (UseItemOpensHive(myPlayer.HoldItem, myPlayer.position.tile)) {
			NetSendCmdPItem(true, CMD_PUTITEM, { 79, 61 }, myPlayer.HoldItem.pop());
			NewCursor(CURSOR_HAND);
			return true;
		}
		if (UseItemOpensCrypt(myPlayer.HoldItem, myPlayer.position.tile)) {
			NetSendCmdPItem(true, CMD_PUTITEM, { 35, 20 }, myPlayer.HoldItem.pop());
			NewCursor(CURSOR_HAND);
			return true;
		}
	}

	Point position = myPlayer.position.future;
	Direction direction = myPlayer._pdir;
	if (!FindAdjacentPositionForItem(position, direction)) {
		myPlayer.Say(HeroSpeech::WhereWouldIPutThis);
		return false;
	}

	NetSendCmdPItem(true, CMD_PUTITEM, position + direction, myPlayer.HoldItem);
	myPlayer.HoldItem.clear();
	NewCursor(CURSOR_HAND);
	return true;
}

void PerformSpellAction()
{
	if (InGameMenu() || QuestLogIsOpen || sbookflag)
		return;

	if (invflag) {
		if (!MyPlayer->HoldItem.isEmpty())
			TryDropItem();
		else if (pcurs > CURSOR_HAND) {
			TryIconCurs();
			NewCursor(CURSOR_HAND);
		} else if (pcursinvitem != -1) {
			int itemId = GetItemIdOnSlot(Slot);
			CheckInvItem(true, false);
			if (itemId != GetItemIdOnSlot(Slot))
				ResetInvCursorPosition();
		} else if (pcursstashitem != uint16_t(-1)) {
			CheckStashItem(MousePosition, true, false);
		}
		return;
	}

	if (!MyPlayer->HoldItem.isEmpty() && !TryDropItem())
		return;
	if (pcurs > CURSOR_HAND)
		NewCursor(CURSOR_HAND);

	if (spselflag) {
		SetSpell();
		return;
	}

	const Player &myPlayer = *MyPlayer;
	int spl = myPlayer._pRSpell;
	if ((pcursplr == -1 && (spl == SPL_RESURRECT || spl == SPL_HEALOTHER))
	    || (ObjectUnderCursor == nullptr && spl == SPL_DISARM)) {
		myPlayer.Say(HeroSpeech::ICantCastThatHere);
		return;
	}

	UpdateSpellTarget(myPlayer._pRSpell);
	CheckPlrSpell(false);
	if (pcursplr != -1)
		LastMouseButtonAction = MouseActionType::SpellPlayerTarget;
	else if (pcursmonst != -1)
		LastMouseButtonAction = MouseActionType::SpellMonsterTarget;
	else
		LastMouseButtonAction = MouseActionType::Spell;
}

void CtrlUseInvItem()
{
	if (pcursinvitem == -1) {
		return;
	}

	Player &myPlayer = *MyPlayer;
	Item &item = GetInventoryItem(myPlayer, pcursinvitem);
	if (item.isScroll()) {
		if (TargetsMonster(item._iSpell)) {
			return;
		}
		if (spelldata[item._iSpell].sTargeted) {
			UpdateSpellTarget(item._iSpell);
		}
	}

	int itemId = GetItemIdOnSlot(Slot);
	if (item.isEquipment()) {
		CheckInvItem(true, false); // auto-equip if it's an equipment
	} else {
		UseInvItem(MyPlayerId, pcursinvitem);
	}
	if (itemId != GetItemIdOnSlot(Slot)) {
		ResetInvCursorPosition();
	}
}

void CtrlUseStashItem()
{
	if (pcursstashitem == uint16_t(-1)) {
		return;
	}

	const Item &item = Stash.stashList[pcursstashitem];
	if (item.isScroll()) {
		if (TargetsMonster(item._iSpell)) {
			return;
		}
		if (spelldata[item._iSpell].sTargeted) {
			UpdateSpellTarget(item._iSpell);
		}
	}

	if (item.isEquipment()) {
		CheckStashItem(MousePosition, true, false); // Auto-equip if it's equipment
	} else {
		UseStashItem(pcursstashitem);
	}
	// Todo reset cursor position if item is moved
}

void PerformSecondaryAction()
{
	Player &myPlayer = *MyPlayer;
	if (invflag) {
		if (pcurs > CURSOR_HAND && pcurs < CURSOR_FIRSTITEM) {
			TryIconCurs();
			NewCursor(CURSOR_HAND);
		} else if (IsStashOpen) {
			if (pcursstashitem != uint16_t(-1)) {
				TransferItemToInventory(myPlayer, pcursstashitem);
			} else if (pcursinvitem != -1) {
				TransferItemToStash(myPlayer, pcursinvitem);
			}
		} else {
			CtrlUseInvItem();
		}
		return;
	}

	if (!MyPlayer->HoldItem.isEmpty() && !TryDropItem())
		return;
	if (pcurs > CURSOR_HAND)
		NewCursor(CURSOR_HAND);

	if (pcursitem != -1) {
		NetSendCmdLocParam1(true, CMD_GOTOAGETITEM, cursPosition, pcursitem);
	} else if (ObjectUnderCursor != nullptr) {
		NetSendCmdLoc(MyPlayerId, true, CMD_OPOBJXY, cursPosition);
		LastMouseButtonAction = MouseActionType::OperateObject;
	} else {
		if (pcursmissile != nullptr) {
			MakePlrPath(myPlayer, pcursmissile->position.tile, true);
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

void QuickCast(size_t slot)
{
	MouseActionType prevMouseButtonAction = LastMouseButtonAction;
	Player &myPlayer = *MyPlayer;
	spell_id spell = myPlayer._pSplHotKey[slot];
	spell_type spellType = myPlayer._pSplTHotKey[slot];

	if (ControlMode != ControlTypes::KeyboardAndMouse) {
		UpdateSpellTarget(spell);
	}

	CheckPlrSpell(false, spell, spellType);
	LastMouseButtonAction = prevMouseButtonAction;
}

} // namespace devilution
