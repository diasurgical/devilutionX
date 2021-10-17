/**
 * @file stash.h
 *
 * Interface of player stash.
 */
#pragma once

#include <cstdint>

#include "engine/point.hpp"
#include "items.h"
#include "palette.h"
#include "player.h"

namespace devilution {

enum stash_item : int8_t {
	// clang-format off
	STASHITEM_STASH_FIRST  = 0,
	STASHITEM_STASH_LAST   = 99
	// clang-format on
};

/**
 * identifiers for each of the stash squares
 * @see #StashRect
 */
enum stash_xy_slot : uint8_t {
	// clang-format off

	SLOTXY_STASH_FIRST = 0,
	SLOTXY_STASH_ROW1_FIRST   = SLOTXY_STASH_FIRST,
	SLOTXY_STASH_ROW1_LAST    = 9,
	SLOTXY_STASH_ROW2_FIRST   = 10,
	SLOTXY_STASH_ROW2_LAST    = 11,
	SLOTXY_STASH_ROW3_FIRST   = 20,
	SLOTXY_STASH_ROW3_LAST    = 29,
	SLOTXY_STASH_ROW4_FIRST   = 30,
	SLOTXY_STASH_ROW4_LAST    = 39,
	SLOTXY_STASH_ROW5_FIRST   = 40,
	SLOTXY_STASH_ROW5_LAST    = 49,
	SLOTXY_STASH_ROW6_FIRST   = 50,
	SLOTXY_STASH_ROW6_LAST    = 59,
	SLOTXY_STASH_ROW7_FIRST   = 60,
	SLOTXY_STASH_ROW7_LAST    = 69,
	SLOTXY_STASH_ROW8_FIRST   = 70,
	SLOTXY_STASH_ROW8_LAST    = 79,
	SLOTXY_STASH_ROW9_FIRST   = 80,
	SLOTXY_STASH_ROW9_LAST    = 89,
	SLOTXY_STASH_ROW10_FIRST  = 90,
	SLOTXY_STASH_ROW10_LAST   = 99,
	SLOTXY_STASH_LAST  = SLOTXY_STASH_ROW10_LAST,
	STASH_NUM_XY_SLOTS = 100
	// clang-format on
};

#define NUM_STASH_GRID_ELEM 100

struct StashStruct {
	void RemoveStashItem(int iv, bool calcScrolls);

	Item StashList[100];
	int8_t StashGrid[NUM_STASH_GRID_ELEM];
	int _pNumStash;
};




extern bool stashflag;
extern const Point StashRect[100];
extern int stashpage;

/**
 * @brief Function type which performs an operation on the given item.
 */
using ItemFunc = void (*)(Item &);

void FreeStashGFX();
void InitStash();

/**
 * @brief Render the inventory panel to the given buffer.
 */
void DrawStash(const Surface &out);

void CheckStashSwap(Player &player, inv_body_loc bLoc, int idx, uint16_t wCI, int seed, bool bId, uint32_t dwBuff);
void CheckStashItem(bool isShiftHeld = false, bool isCtrlHeld = false);
void StashCheckItemStats(Player &player);
int StashFindGetItem(int idx, uint16_t ci, int iseed);
void StashSyncGetItem(Point position, int idx, uint16_t ci, int iseed);
bool StashCanPut(Point position);
bool TryStashPut();
int StashPutItem(Player &player, Point position);
int SyncStashPutItem(Player &player, Point position, int idx, uint16_t icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, uint32_t ibuff, int toHit, int maxDam, int minStr, int minMag, int minDex, int ac);
bool UseStashItem(int pnum, int cii);
int CalculateStashGold(Player &player);
bool DropStashItemBeforeTrig();
void LoadStash(int page);
void SaveStash(int page);
int8_t CheckStashHLight();


/* data */

} // namespace devilution
