/**
 * @file drlg_l2.cpp
 *
 * Implementation of the catacombs level generation algorithms.
 */
#include "drlg_l2.h"

#include <algorithm>
#include <list>

#include "diablo.h"
#include "drlg_l1.h"
#include "engine/load_file.hpp"
#include "objects.h"
#include "player.h"
#include "quests.h"
#include "setmaps.h"

namespace devilution {

BYTE predungeon[DMAXX][DMAXY];

namespace {

int nSx1;
int nSy1;
int nSx2;
int nSy2;
int nRoomCnt;
ROOMNODE RoomList[81];
std::list<HALLNODE> HallList;

int Area_Min = 2;
int Room_Max = 10;
int Room_Min = 4;
const int Dir_Xadd[5] = { 0, 0, 1, 0, -1 };
const int Dir_Yadd[5] = { 0, -1, 0, 1, 0 };
const ShadowStruct SPATSL2[2] = { { 6, 3, 0, 3, 48, 0, 50 }, { 9, 3, 0, 3, 48, 0, 50 } };
//short word_48489A = 0;

const BYTE BTYPESL2[161] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 17, 18, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
const BYTE BSTYPESL2[161] = { 0, 1, 2, 3, 0, 0, 6, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 6, 6, 6, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1, 1, 1, 1, 6, 2, 2, 2, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 3, 3, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/** Miniset: Arch vertical. */
const BYTE VARCH1[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 1,
	3, 4,
	0, 7,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH2[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 1,
	3, 4,
	0, 8,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH3[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 1,
	3, 4,
	0, 6,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH4[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 1,
	3, 4,
	0, 9,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH5[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 1,
	3, 4,
	0, 14,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH6[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 1,
	3, 4,
	0, 13,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH7[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 1,
	3, 4,
	0, 16,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH8[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 1,
	3, 4,
	0, 15,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - corner. */
const BYTE VARCH9[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 8,
	3, 4,
	0, 7,

	48,  0, // replace
	51, 42,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - corner. */
const BYTE VARCH10[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 8,
	3, 4,
	0, 8,

	48,  0, // replace
	51, 42,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - corner. */
const BYTE VARCH11[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 8,
	3, 4,
	0, 6,

	48,  0, // replace
	51, 42,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - corner. */
const BYTE VARCH12[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 8,
	3, 4,
	0, 9,

	48,  0, // replace
	51, 42,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - corner. */
const BYTE VARCH13[] = {
	// clang-format off
	2, 4, // width, height

	3,  0, // search
	3,  8,
	3,  4,
	0, 14,

	48,  0, // replace
	51, 42,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - corner. */
const BYTE VARCH14[] = {
	// clang-format off
	2, 4, // width, height

	3,  0, // search
	3,  8,
	3,  4,
	0, 13,

	48,  0, // replace
	51, 42,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - corner. */
const BYTE VARCH15[] = {
	// clang-format off
	2, 4, // width, height

	3,  0, // search
	3,  8,
	3,  4,
	0, 16,

	48,  0, // replace
	51, 42,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - corner. */
const BYTE VARCH16[] = {
	// clang-format off
	2, 4, // width, height

	3,  0, // search
	3,  8,
	3,  4,
	0, 15,

	48,  0, // replace
	51, 42,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - open wall. */
const BYTE VARCH17[] = {
	// clang-format off
	2, 3, // width, height

	2, 7, // search
	3, 4,
	0, 7,

	141, 39, // replace
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - open wall. */
const BYTE VARCH18[] = {
	// clang-format off
	2, 3, // width, height

	2, 7, // search
	3, 4,
	0, 8,

	141, 39, // replace
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - open wall. */
const BYTE VARCH19[] = {
	// clang-format off
	2, 3, // width, height

	2, 7, // search
	3, 4,
	0, 6,

	141, 39, // replace
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - open wall. */
const BYTE VARCH20[] = {
	// clang-format off
	2, 3, // width, height

	2, 7, // search
	3, 4,
	0, 9,

	141, 39, // replace
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - open wall. */
const BYTE VARCH21[] = {
	// clang-format off
	2, 3, // width, height

	2,  7, // search
	3,  4,
	0, 14,

	141, 39, // replace
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - open wall. */
const BYTE VARCH22[] = {
	// clang-format off
	2, 3, // width, height

	2,  7, // search
	3,  4,
	0, 13,

	141, 39, // replace
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - open wall. */
const BYTE VARCH23[] = {
	// clang-format off
	2, 3, // width, height

	2,  7, // search
	3,  4,
	0, 16,

	141, 39, // replace
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - open wall. */
const BYTE VARCH24[] = {
	// clang-format off
	2, 3, // width, height

	2,  7, // search
	3,  4,
	0, 15,

	141, 39, // replace
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH25[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 4,
	3, 1,
	0, 7,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH26[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 4,
	3, 1,
	0, 8,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH27[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 4,
	3, 1,
	0, 6,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH28[] = {
	// clang-format off
	2, 4, // width, height

	3, 0, // search
	3, 4,
	3, 1,
	0, 9,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH29[] = {
	// clang-format off
	2, 4, // width, height

	3,  0, // search
	3,  4,
	3,  1,
	0, 14,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH30[] = {
	// clang-format off
	2, 4, // width, height

	3,  0, // search
	3,  4,
	3,  1,
	0, 13,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH31[] = {
	// clang-format off
	2, 4, // width, height

	3,  0, // search
	3,  4,
	3,  1,
	0, 16,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical. */
const BYTE VARCH32[] = {
	// clang-format off
	2, 4, // width, height

	3,  0, // search
	3,  4,
	3,  1,
	0, 15,

	48,  0, // replace
	51, 39,
	47, 44,
	 0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - room west entrance. */
const BYTE VARCH33[] = {
	// clang-format off
	2, 4, // width, height

	2, 0, // search
	3, 8,
	3, 4,
	0, 7,

	142,  0, // replace
	 51, 42,
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - room west entrance. */
const BYTE VARCH34[] = {
	// clang-format off
	2, 4, // width, height

	2, 0, // search
	3, 8,
	3, 4,
	0, 8,

	142,  0, // replace
	 51, 42,
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - room west entrance. */
const BYTE VARCH35[] = {
	// clang-format off
	2, 4, // width, height

	2, 0, // search
	3, 8,
	3, 4,
	0, 6,

	142,  0, // replace
	 51, 42,
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - room west entrance. */
const BYTE VARCH36[] = {
	// clang-format off
	2, 4, // width, height

	2, 0, // search
	3, 8,
	3, 4,
	0, 9,

	142,  0, // replace
	 51, 42,
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - room west entrance. */
const BYTE VARCH37[] = {
	// clang-format off
	2, 4, // width, height

	2,  0, // search
	3,  8,
	3,  4,
	0, 14,

	142,  0, // replace
	 51, 42,
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - room west entrance. */
const BYTE VARCH38[] = {
	// clang-format off
	2, 4, // width, height

	2,  0, // search
	3,  8,
	3,  4,
	0, 13,

	142,  0, // replace
	 51, 42,
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - room west entrance. */
const BYTE VARCH39[] = {
	// clang-format off
	2, 4, // width, height

	2,  0, // search
	3,  8,
	3,  4,
	0, 16,

	142,  0, // replace
	 51, 42,
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch vertical - room west entrance. */
const BYTE VARCH40[] = {
	// clang-format off
	2, 4, // width, height

	2,  0, // search
	3,  8,
	3,  4,
	0, 15,

	142,  0, // replace
	 51, 42,
	 47, 44,
	  0,  0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH1[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	2, 5, 9,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH2[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	2, 5, 6,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH3[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	2, 5, 8,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH4[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	2, 5, 7,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH5[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	2, 5, 15,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH6[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	2, 5, 16,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH7[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	2, 5, 13,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH8[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	2, 5, 14,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - north corner. */
const BYTE HARCH9[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	8, 5, 9,

	49, 46, 0, // replace
	43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - north corner. */
const BYTE HARCH10[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	8, 5, 6,

	49, 46, 0, // replace
	43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - north corner. */
const BYTE HARCH11[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	8, 5, 8,

	49, 46, 0, // replace
	43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - north corner. */
const BYTE HARCH12[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	8, 5, 7,

	49, 46, 0, // replace
	43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - north corner. */
const BYTE HARCH13[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	8, 5, 15,

	49, 46, 0, // replace
	43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - north corner. */
const BYTE HARCH14[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	8, 5, 16,

	49, 46, 0, // replace
	43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - north corner. */
const BYTE HARCH15[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	8, 5, 13,

	49, 46, 0, // replace
	43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - north corner. */
const BYTE HARCH16[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	8, 5, 14,

	49, 46, 0, // replace
	43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - wall. */
const BYTE HARCH17[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	8, 5, 9,

	140, 46, 0, // replace
	 43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - wall. */
const BYTE HARCH18[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	8, 5, 6,

	140, 46, 0, // Replace
	 43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - wall. */
const BYTE HARCH19[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	8, 5, 8,

	140, 46, 0, // replace
	 43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - wall. */
const BYTE HARCH20[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	8, 5, 7,

	140, 46, 0, // replace
	 43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - wall. */
const BYTE HARCH21[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	8, 5, 15,

	140, 46, 0, // replace
	 43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - wall. */
const BYTE HARCH22[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	8, 5, 16,

	140, 46, 0, // replace
	 43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - wall. */
const BYTE HARCH23[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	8, 5, 13,

	140, 46, 0, // replace
	 43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - wall. */
const BYTE HARCH24[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	8, 5, 14,

	140, 46, 0, // replace
	 43, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH25[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	5, 2, 9,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH26[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	5, 2, 6,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH27[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	5, 2, 8,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH28[] = {
	// clang-format off
	3, 2, // width, height

	3, 3, 0, // search
	5, 2, 7,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH29[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	5, 2, 15,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH30[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	5, 2, 16,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH31[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	5, 2, 13,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal. */
const BYTE HARCH32[] = {
	// clang-format off
	3, 2, // width, height

	3, 3,  0, // search
	5, 2, 14,

	49, 46, 0, // replace
	40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - west corner. */
const BYTE HARCH33[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	9, 5, 9,

	140, 46, 0, // replace
	 40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - west corner. */
const BYTE HARCH34[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	9, 5, 6,

	140, 46, 0, // replace
	 40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - west corner. */
const BYTE HARCH35[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	9, 5, 8,

	140, 46, 0, // replace
	 40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - west corner. */
const BYTE HARCH36[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	9, 5, 7,

	140, 46, 0, // replace
	 40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - west corner. */
const BYTE HARCH37[] = {
	// clang-format off
	3, 2, // width, height

	1, 3, 0, // search
	9, 5, 15,

	140, 46, 0, // replace
	 40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - west corner. */
const BYTE HARCH38[] = {
	// clang-format off
	3, 2, // width, height

	1, 3,  0, // search
	9, 5, 16,

	140, 46, 0, // replace
	 40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - west corner. */
const BYTE HARCH39[] = {
	// clang-format off
	3, 2, // width, height

	1, 3,  0, // search
	9, 5, 13,

	140, 46, 0, // replace
	 40, 45, 0,
	// clang-format on
};
/** Miniset: Arch horizontal - west corner. */
const BYTE HARCH40[] = {
	// clang-format off
	3, 2, // width, height

	1, 3,  0, // search
	9, 5, 14,

	140, 46, 0, // replace
	 40, 45, 0,
	// clang-format on
};
/** Miniset: Stairs up. */
const BYTE USTAIRS[] = {
	// clang-format off
	4, 4, // width, height

	3, 3, 3, 3, // search
	3, 3, 3, 3,
	3, 3, 3, 3,
	3, 3, 3, 3,

	0,  0,  0, 0, // replace
	0, 72, 77, 0,
	0, 76,  0, 0,
	0,  0,  0, 0,
	// clang-format on
};
/** Miniset: Stairs down. */
const BYTE DSTAIRS[] = {
	// clang-format off
	4, 4, // width, height

	3, 3, 3, 3, // search
	3, 3, 3, 3,
	3, 3, 3, 3,
	3, 3, 3, 3,

	0,  0,  0, 0, // replace
	0, 48, 71, 0,
	0, 50, 78, 0,
	0,  0,  0, 0,
	// clang-format on
};
/** Miniset: Stairs to town. */
const BYTE WARPSTAIRS[] = {
	// clang-format off
	4, 4, // width, height

	3, 3, 3, 3, // search
	3, 3, 3, 3,
	3, 3, 3, 3,
	3, 3, 3, 3,

	0,   0,   0, 0, // replace
	0, 158, 160, 0,
	0, 159,   0, 0,
	0,   0,   0, 0,
	// clang-format on
};
/** Miniset: Crumbled south pillar. */
const BYTE CRUSHCOL[] = {
	// clang-format off
	3, 3, // width, height

	3, 1, 3, // search
	2, 6, 3,
	3, 3, 3,

	0,  0, 0, // replace
	0, 83, 0,
	0,  0, 0,
	// clang-format on
};
/** Miniset: Vertical oil spill. */
const BYTE BIG1[] = {
	// clang-format off
	2, 2, // width, height

	3, 3, // search
	3, 3,

	113, 0, // replace
	112, 0,
	// clang-format on
};
/** Miniset: Horizontal oil spill. */
const BYTE BIG2[] = {
	// clang-format off
	2, 2, // width, height

	3, 3, // search
	3, 3,

	114, 115, // replace
	  0,   0,
	// clang-format on
};
/** Miniset: Horizontal platform. */
const BYTE BIG3[] = {
	// clang-format off
	1, 2, // width, height

	1, // search
	1,

	117, // replace
	116,
	// clang-format on
};
/** Miniset: Vertical platform. */
const BYTE BIG4[] = {
	// clang-format off
	2, 1, // width, height

	2, 2, // search

	118, 119, // replace
	// clang-format on
};
/** Miniset: Large oil spill. */
const BYTE BIG5[] = {
	// clang-format off
	2, 2, // width, height

	3, 3, // search
	3, 3,

	120, 122, // replace
	121, 123,
	// clang-format on
};
/** Miniset: Vertical wall with debris. */
const BYTE BIG6[] = {
	// clang-format off
	1, 2, // width, height

	1, // search
	1,

	125, // replace
	124,
	// clang-format on
};
/** Miniset: Horizontal wall with debris. */
const BYTE BIG7[] = {
	// clang-format off
	2, 1, // width, height

	2, 2, // search

	126, 127, // replace
	// clang-format on
};
/** Miniset: Rock pile. */
const BYTE BIG8[] = {
	// clang-format off
	2, 2, // width, height

	3, 3, // search
	3, 3,

	128, 130, // replace
	129, 131,
	// clang-format on
};
/** Miniset: Vertical wall collapsed. */
const BYTE BIG9[] = {
	// clang-format off
	2, 2, // width, height

	1, 3, // search
	1, 3,

	133, 135, // replace
	132, 134,
	// clang-format on
};
/** Miniset: Horizontal wall collapsed. */
const BYTE BIG10[] = {
	// clang-format off
	2, 2, // width, height

	2, 2, // search
	3, 3,

	136, 137, // replace
	  3,   3,
	// clang-format on
};
/** Miniset: Crumbled vertical wall 1. */
const BYTE RUINS1[] = {
	// clang-format off
	1, 1, // width, height

	1, // search

	80, // replace
	// clang-format on
};
/** Miniset: Crumbled vertical wall 2. */
const BYTE RUINS2[] = {
	// clang-format off
	1, 1, // width, height

	1, // search

	81, // replace
	// clang-format on
};
/** Miniset: Crumbled vertical wall 3. */
const BYTE RUINS3[] = {
	// clang-format off
	1, 1, // width, height

	1, // search

	82, // replace
	// clang-format on
};
/** Miniset: Crumbled horizontal wall 1. */
const BYTE RUINS4[] = {
	// clang-format off
	1, 1, // width, height

	2, // search

	84, // replace
	// clang-format on
};
/** Miniset: Crumbled horizontal wall 2. */
const BYTE RUINS5[] = {
	// clang-format off
	1, 1, // width, height

	2, // search

	85, // replace
	// clang-format on
};
/** Miniset: Crumbled horizontal wall 3. */
const BYTE RUINS6[] = {
	// clang-format off
	1, 1, // width, height

	2, // search

	86, // replace
	// clang-format on
};
/** Miniset: Crumbled north pillar. */
const BYTE RUINS7[] = {
	// clang-format off
	1, 1, // width, height

	8, // search

	87, // replace
	// clang-format on
};
/** Miniset: Bloody gib 1. */
const BYTE PANCREAS1[] = {
	// clang-format off
	5, 3, // width, height

	3, 3, 3, 3, 3, // search
	3, 3, 3, 3, 3,
	3, 3, 3, 3, 3,

	0, 0,   0, 0, 0, // replace
	0, 0, 108, 0, 0,
	0, 0,   0, 0, 0,
	// clang-format on
};
/** Miniset: Bloody gib 2. */
const BYTE PANCREAS2[] = {
	// clang-format off
	5, 3, // width, height

	3, 3, 3, 3, 3, // search
	3, 3, 3, 3, 3,
	3, 3, 3, 3, 3,

	0, 0,   0, 0, 0, // replace
	0, 0, 110, 0, 0,
	0, 0,   0, 0, 0,
	// clang-format on
};
/** Miniset: Move vertical doors away from west pillar 1. */
const BYTE CTRDOOR1[] = {
	// clang-format off
	3, 3, // width, height

	3, 1, 3,  // search
	0, 4, 0,
	0, 9, 0,

	0, 4, 0, // replace
	0, 1, 0,
	0, 0, 0,
	// clang-format on
};
/** Miniset: Move vertical doors away from west pillar 2. */
const BYTE CTRDOOR2[] = {
	// clang-format off
	3, 3, // width, height

	3, 1, 3, // search
	0, 4, 0,
	0, 8, 0,

	0, 4, 0, // replace
	0, 1, 0,
	0, 0, 0,
	// clang-format on
};
/** Miniset: Move vertical doors away from west pillar 3. */
const BYTE CTRDOOR3[] = {
	// clang-format off
	3, 3, // width, height

	3, 1, 3, // search
	0, 4, 0,
	0, 6, 0,

	0, 4, 0, // replace
	0, 1, 0,
	0, 0, 0,
	// clang-format on
};
/** Miniset: Move vertical doors away from west pillar 4. */
const BYTE CTRDOOR4[] = {
	// clang-format off
	3, 3, // width, height

	3, 1, 3, // search
	0, 4, 0,
	0, 7, 0,

	0, 4, 0, // replace
	0, 1, 0,
	0, 0, 0,
	// clang-format on
};
/** Miniset: Move vertical doors away from west pillar 5. */
const BYTE CTRDOOR5[] = {
	// clang-format off
	3, 3, // width, height

	3,  1, 3, // search
	0,  4, 0,
	0, 15, 0,

	0, 4, 0, // replace
	0, 1, 0,
	0, 0, 0,
	// clang-format on
};
/** Miniset: Move vertical doors away from west pillar 6. */
const BYTE CTRDOOR6[] = {
	// clang-format off
	3, 3, // width, height

	3,  1, 3, // search
	0,  4, 0,
	0, 13, 0,

	0, 4, 0, // replace
	0, 1, 0,
	0, 0, 0,
	// clang-format on
};
/** Miniset: Move vertical doors away from west pillar 7. */
const BYTE CTRDOOR7[] = {
	// clang-format off
	3, 3, // width, height

	3,  1, 3, // search
	0,  4, 0,
	0, 16, 0,

	0, 4, 0, // replace
	0, 1, 0,
	0, 0, 0,
	// clang-format on
};
/** Miniset: Move vertical doors away from west pillar 8. */
const BYTE CTRDOOR8[] = {
	// clang-format off
	3, 3, // width, height

	3,  1, 3, // search
	0,  4, 0,
	0, 14, 0,

	0, 4, 0, // replace
	0, 1, 0,
	0, 0, 0,
	// clang-format on
};

int Patterns[100][10] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
	{ 0, 0, 0, 0, 2, 0, 0, 0, 0, 3 },
	{ 0, 7, 0, 0, 1, 0, 0, 5, 0, 2 },
	{ 0, 5, 0, 0, 1, 0, 0, 7, 0, 2 },
	{ 0, 0, 0, 7, 1, 5, 0, 0, 0, 1 },
	{ 0, 0, 0, 5, 1, 7, 0, 0, 0, 1 },
	{ 0, 1, 0, 0, 3, 0, 0, 1, 0, 4 },
	{ 0, 0, 0, 1, 3, 1, 0, 0, 0, 5 },
	{ 0, 6, 0, 6, 1, 0, 0, 0, 0, 6 },
	{ 0, 6, 0, 0, 1, 6, 0, 0, 0, 9 },
	{ 0, 0, 0, 6, 1, 0, 0, 6, 0, 7 },
	{ 0, 0, 0, 0, 1, 6, 0, 6, 0, 8 },
	{ 0, 6, 0, 6, 6, 0, 8, 6, 0, 7 },
	{ 0, 6, 8, 6, 6, 6, 0, 0, 0, 9 },
	{ 0, 6, 0, 0, 6, 6, 0, 6, 8, 8 },
	{ 6, 6, 6, 6, 6, 6, 0, 6, 0, 8 },
	{ 2, 6, 6, 6, 6, 6, 0, 6, 0, 8 },
	{ 7, 7, 7, 6, 6, 6, 0, 6, 0, 8 },
	{ 6, 6, 2, 6, 6, 6, 0, 6, 0, 8 },
	{ 6, 2, 6, 6, 6, 6, 0, 6, 0, 8 },
	{ 2, 6, 6, 6, 6, 6, 0, 6, 0, 8 },
	{ 6, 7, 7, 6, 6, 6, 0, 6, 0, 8 },
	{ 4, 4, 6, 6, 6, 6, 2, 6, 2, 8 },
	{ 2, 2, 2, 2, 6, 2, 2, 6, 2, 7 },
	{ 2, 2, 2, 2, 6, 2, 6, 6, 6, 7 },
	{ 2, 2, 6, 2, 6, 6, 2, 2, 6, 9 },
	{ 2, 6, 2, 2, 6, 2, 2, 2, 2, 6 },
	{ 2, 2, 2, 2, 6, 6, 2, 2, 2, 9 },
	{ 2, 2, 2, 6, 6, 2, 2, 2, 2, 6 },
	{ 2, 2, 0, 2, 6, 6, 2, 2, 0, 9 },
	{ 0, 0, 0, 0, 4, 0, 0, 0, 0, 12 },
	{ 0, 1, 0, 0, 1, 4, 0, 1, 0, 10 },
	{ 0, 0, 0, 1, 1, 1, 0, 4, 0, 11 },
	{ 0, 0, 0, 6, 1, 4, 0, 1, 0, 14 },
	{ 0, 6, 0, 1, 1, 0, 0, 4, 0, 16 },
	{ 0, 6, 0, 0, 1, 1, 0, 4, 0, 15 },
	{ 0, 0, 0, 0, 1, 1, 0, 1, 4, 13 },
	{ 8, 8, 8, 8, 1, 1, 0, 1, 1, 13 },
	{ 8, 8, 4, 8, 1, 1, 0, 1, 1, 10 },
	{ 0, 0, 0, 1, 1, 1, 1, 1, 1, 11 },
	{ 1, 1, 1, 1, 1, 1, 2, 2, 8, 2 },
	{ 0, 1, 0, 1, 1, 4, 1, 1, 0, 16 },
	{ 0, 0, 0, 1, 1, 1, 1, 1, 4, 11 },
	{ 1, 1, 4, 1, 1, 1, 0, 2, 2, 2 },
	{ 1, 1, 1, 1, 1, 1, 6, 2, 6, 2 },
	{ 4, 1, 1, 1, 1, 1, 6, 2, 6, 2 },
	{ 2, 2, 2, 1, 1, 1, 4, 1, 1, 11 },
	{ 4, 1, 1, 1, 1, 1, 2, 2, 2, 2 },
	{ 1, 1, 4, 1, 1, 1, 2, 2, 1, 2 },
	{ 4, 1, 1, 1, 1, 1, 1, 2, 2, 2 },
	{ 2, 2, 6, 1, 1, 1, 4, 1, 1, 11 },
	{ 4, 1, 1, 1, 1, 1, 2, 2, 6, 2 },
	{ 1, 2, 2, 1, 1, 1, 4, 1, 1, 11 },
	{ 0, 1, 1, 0, 1, 1, 0, 1, 1, 10 },
	{ 2, 1, 1, 3, 1, 1, 2, 1, 1, 14 },
	{ 1, 1, 0, 1, 1, 2, 1, 1, 0, 1 },
	{ 0, 4, 0, 1, 1, 1, 0, 1, 1, 14 },
	{ 4, 1, 0, 1, 1, 0, 1, 1, 0, 1 },
	{ 0, 1, 0, 4, 1, 1, 0, 1, 1, 15 },
	{ 1, 1, 1, 1, 1, 1, 0, 2, 2, 2 },
	{ 0, 1, 1, 2, 1, 1, 2, 1, 4, 10 },
	{ 2, 1, 1, 1, 1, 1, 0, 4, 0, 16 },
	{ 1, 1, 4, 1, 1, 2, 0, 1, 2, 1 },
	{ 2, 1, 1, 2, 1, 1, 1, 1, 4, 10 },
	{ 1, 1, 2, 1, 1, 2, 4, 1, 8, 1 },
	{ 2, 1, 4, 1, 1, 1, 4, 4, 1, 16 },
	{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 16 },
	{ 1, 1, 2, 1, 1, 1, 1, 1, 1, 15 },
	{ 1, 1, 1, 1, 1, 1, 2, 1, 1, 14 },
	{ 4, 1, 1, 1, 1, 1, 2, 1, 1, 14 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 2, 8 },
	{ 0, 0, 0, 0, 255, 0, 0, 0, 0, 0 },
};

} // namespace

static bool DRLG_L2PlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, bool setview, int ldir)
{
	int sx, sy, sw, sh, xx, yy, i, ii, numt, bailcnt;
	bool found;

	sw = miniset[0];
	sh = miniset[1];

	if (tmax - tmin == 0) {
		numt = 1;
	} else {
		numt = GenerateRnd(tmax - tmin) + tmin;
	}

	for (i = 0; i < numt; i++) {
		sx = GenerateRnd(DMAXX - sw);
		sy = GenerateRnd(DMAXY - sh);
		found = false;
		for (bailcnt = 0; !found && bailcnt < 200; bailcnt++) {
			found = true;
			if (sx >= nSx1 && sx <= nSx2 && sy >= nSy1 && sy <= nSy2) {
				found = false;
			}
			if (cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx = GenerateRnd(DMAXX - sw);
				sy = GenerateRnd(DMAXY - sh);
				found = false;
			}
			if (cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sx = GenerateRnd(DMAXX - sw);
				sy = GenerateRnd(DMAXY - sh);
				found = false;
			}
			ii = 2;
			for (yy = 0; yy < sh && found; yy++) {
				for (xx = 0; xx < sw && found; xx++) {
					if (miniset[ii] != 0 && dungeon[xx + sx][yy + sy] != miniset[ii]) {
						found = false;
					}
					if (dflags[xx + sx][yy + sy] != 0) {
						found = false;
					}
					ii++;
				}
			}
			if (!found) {
				sx++;
				if (sx == DMAXX - sw) {
					sx = 0;
					sy++;
					if (sy == DMAXY - sh) {
						sy = 0;
					}
				}
			}
		}
		if (bailcnt >= 200) {
			return false;
		}
		ii = sw * sh + 2;
		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) {
					dungeon[xx + sx][yy + sy] = miniset[ii];
				}
				ii++;
			}
		}
	}

	if (setview) {
		ViewX = 2 * sx + 21;
		ViewY = 2 * sy + 22;
	}
	if (ldir == 0) {
		LvlViewX = 2 * sx + 21;
		LvlViewY = 2 * sy + 22;
	}
	if (ldir == 6) {
		LvlViewX = 2 * sx + 21;
		LvlViewY = 2 * sy + 22;
	}

	return true;
}

static void DRLG_L2PlaceRndSet(const BYTE *miniset, int rndper)
{
	int sx, sy, sw, sh, xx, yy, ii, kk;
	bool found;

	sw = miniset[0];
	sh = miniset[1];

	for (sy = 0; sy < DMAXY - sh; sy++) {
		for (sx = 0; sx < DMAXX - sw; sx++) {
			found = true;
			ii = 2;
			if (sx >= nSx1 && sx <= nSx2 && sy >= nSy1 && sy <= nSy2) {
				found = false;
			}
			for (yy = 0; yy < sh && found; yy++) {
				for (xx = 0; xx < sw && found; xx++) {
					if (miniset[ii] != 0 && dungeon[xx + sx][yy + sy] != miniset[ii]) {
						found = false;
					}
					if (dflags[xx + sx][yy + sy] != 0) {
						found = false;
					}
					ii++;
				}
			}
			kk = sw * sh + 2;
			if (found) {
				for (yy = std::max(sy - sh, 0); yy < std::min(sy + 2 * sh, DMAXY) && found; yy++) {
					for (xx = std::max(sx - sw, 0); xx < std::min(sx + 2 * sw, DMAXX); xx++) {
						// BUGFIX: yy and xx can go out of bounds (fixed)
						if (dungeon[xx][yy] == miniset[kk]) {
							found = false;
						}
					}
				}
			}
			if (found && GenerateRnd(100) < rndper) {
				for (yy = 0; yy < sh; yy++) {
					for (xx = 0; xx < sw; xx++) {
						if (miniset[kk] != 0) {
							dungeon[xx + sx][yy + sy] = miniset[kk];
						}
						kk++;
					}
				}
			}
		}
	}
}

static void DRLG_L2Subs()
{
	int x, y, i, j, k, rv;
	BYTE c;

	for (y = 0; y < DMAXY; y++) {
		for (x = 0; x < DMAXX; x++) {
			if ((x < nSx1 || x > nSx2) && (y < nSy1 || y > nSy2) && GenerateRnd(4) == 0) {
				c = BTYPESL2[dungeon[x][y]];
				if (c != 0) {
					rv = GenerateRnd(16);
					k = -1;
					while (rv >= 0) {
						k++;
						if (k == sizeof(BTYPESL2)) {
							k = 0;
						}
						if (c == BTYPESL2[k]) {
							rv--;
						}
					}
					for (j = y - 2; j < y + 2; j++) {
						for (i = x - 2; i < x + 2; i++) {
							if (dungeon[i][j] == k) {
								j = y + 3;
								i = x + 2;
							}
						}
					}
					if (j < y + 3) {
						dungeon[x][y] = k;
					}
				}
			}
		}
	}
}

static void DRLG_L2Shadows()
{
	int x, y, i;
	bool patflag;
	BYTE sd[2][2];

	for (y = 1; y < DMAXY; y++) {
		for (x = 1; x < DMAXX; x++) {
			sd[0][0] = BSTYPESL2[dungeon[x][y]];
			sd[1][0] = BSTYPESL2[dungeon[x - 1][y]];
			sd[0][1] = BSTYPESL2[dungeon[x][y - 1]];
			sd[1][1] = BSTYPESL2[dungeon[x - 1][y - 1]];
			for (i = 0; i < 2; i++) {
				if (SPATSL2[i].strig == sd[0][0]) {
					patflag = true;
					if (SPATSL2[i].s1 != 0 && SPATSL2[i].s1 != sd[1][1]) {
						patflag = false;
					}
					if (SPATSL2[i].s2 != 0 && SPATSL2[i].s2 != sd[0][1]) {
						patflag = false;
					}
					if (SPATSL2[i].s3 != 0 && SPATSL2[i].s3 != sd[1][0]) {
						patflag = false;
					}
					if (patflag) {
						if (SPATSL2[i].nv1 != 0) {
							dungeon[x - 1][y - 1] = SPATSL2[i].nv1;
						}
						if (SPATSL2[i].nv2 != 0) {
							dungeon[x][y - 1] = SPATSL2[i].nv2;
						}
						if (SPATSL2[i].nv3 != 0) {
							dungeon[x - 1][y] = SPATSL2[i].nv3;
						}
					}
				}
			}
		}
	}
}

void InitDungeon()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			predungeon[i][j] = 32;
			dflags[i][j] = 0;
		}
	}
}

static void DRLG_LoadL2SP()
{
	setloadflag = false;

	if (QuestStatus(Q_BLIND)) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blind1.DUN");
		pSetPiece[13] = SDL_SwapLE16(154);  // Close outer wall
		pSetPiece[100] = SDL_SwapLE16(154); // Close outer wall
		setloadflag = true;
	} else if (QuestStatus(Q_BLOOD)) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blood1.DUN");
		setloadflag = true;
	} else if (QuestStatus(Q_SCHAMB)) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L2Data\\Bonestr2.DUN");
		setloadflag = true;
	}
}

static void DRLG_FreeL2SP()
{
	pSetPiece = nullptr;
}

static void DRLG_L2SetRoom(int rx1, int ry1)
{
	int width = SDL_SwapLE16(pSetPiece[0]);
	int height = SDL_SwapLE16(pSetPiece[1]);

	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = width;
	setpc_h = height;

	uint16_t *tileLayer = &pSetPiece[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(tileLayer[j * width + i]);
			if (tileId != 0) {
				dungeon[i + rx1][j + ry1] = tileId;
				dflags[i + rx1][j + ry1] |= DLRG_PROTECTED;
			} else {
				dungeon[i + rx1][j + ry1] = 3;
			}
		}
	}
}

static void DefineRoom(int nX1, int nY1, int nX2, int nY2, bool ForceHW)
{
	int i, j;

	predungeon[nX1][nY1] = 67;
	predungeon[nX1][nY2] = 69;
	predungeon[nX2][nY1] = 66;
	predungeon[nX2][nY2] = 65;

	nRoomCnt++;
	RoomList[nRoomCnt].nRoomx1 = nX1;
	RoomList[nRoomCnt].nRoomx2 = nX2;
	RoomList[nRoomCnt].nRoomy1 = nY1;
	RoomList[nRoomCnt].nRoomy2 = nY2;

	if (ForceHW) {
		for (i = nX1; i < nX2; i++) {
			/// BUGFIX: Should loop j between nY1 and nY2 instead of always using nY1.
			while (i < nY2) {
				dflags[i][nY1] |= DLRG_PROTECTED;
				i++;
			}
		}
	}
	for (i = nX1 + 1; i <= nX2 - 1; i++) {
		predungeon[i][nY1] = 35;
		predungeon[i][nY2] = 35;
	}
	nY2--;
	for (j = nY1 + 1; j <= nY2; j++) {
		predungeon[nX1][j] = 35;
		predungeon[nX2][j] = 35;
		for (i = nX1 + 1; i < nX2; i++) {
			predungeon[i][j] = 46;
		}
	}
}

static void CreateDoorType(int nX, int nY)
{
	if (predungeon[nX - 1][nY] == 68) {
		return;
	}
	if (predungeon[nX + 1][nY] == 68) {
		return;
	}
	if (predungeon[nX][nY - 1] == 68) {
		return;
	}
	if (predungeon[nX][nY + 1] == 68) {
		return;
	}
	if (predungeon[nX][nY] == 66 || predungeon[nX][nY] == 67 || predungeon[nX][nY] == 65 || predungeon[nX][nY] == 69) {
		return;
	}

	predungeon[nX][nY] = 68;
}

static void PlaceHallExt(int nX, int nY)
{
	if (predungeon[nX][nY] == 32) {
		predungeon[nX][nY] = 44;
	}
}

/**
 * Draws a random room rectangle, and then subdivides the rest of the passed in rectangle into 4 and recurses.
 * @param nX1 Lower X boundary of the area to draw into.
 * @param nY1 Lower Y boundary of the area to draw into.
 * @param nX2 Upper X boundary of the area to draw into.
 * @param nY2 Upper Y boundary of the area to draw into.
 * @param nRDest The room number of the parent room this call was invoked for. Zero for empty
 * @param nHDir The direction of the hall from nRDest to this room.
 * @param ForceHW If set, nH and nW are used for room size instead of random values.
 * @param nH Height of the room, if ForceHW is set.
 * @param nW Width of the room, if ForceHW is set.
 */
static void CreateRoom(int nX1, int nY1, int nX2, int nY2, int nRDest, int nHDir, bool ForceHW, int nH, int nW)
{
	int nAw, nAh, nRw, nRh, nRx1, nRy1, nRx2, nRy2, nHw, nHh, nHx1, nHy1, nHx2, nHy2, nRid;

	if (nRoomCnt >= 80) {
		return;
	}

	nAw = nX2 - nX1;
	nAh = nY2 - nY1;
	if (nAw < Area_Min || nAh < Area_Min) {
		return;
	}

	if (nAw > Room_Max) {
		nRw = GenerateRnd(Room_Max - Room_Min) + Room_Min;
	} else if (nAw > Room_Min) {
		nRw = GenerateRnd(nAw - Room_Min) + Room_Min;
	} else {
		nRw = nAw;
	}
	if (nAh > Room_Max) {
		nRh = GenerateRnd(Room_Max - Room_Min) + Room_Min;
	} else if (nAh > Room_Min) {
		nRh = GenerateRnd(nAh - Room_Min) + Room_Min;
	} else {
		nRh = nAh;
	}

	if (ForceHW) {
		nRw = nW;
		nRh = nH;
	}

	nRx1 = GenerateRnd(nX2 - nX1) + nX1;
	nRy1 = GenerateRnd(nY2 - nY1) + nY1;
	nRx2 = nRw + nRx1;
	nRy2 = nRh + nRy1;
	if (nRx2 > nX2) {
		nRx2 = nX2;
		nRx1 = nX2 - nRw;
	}
	if (nRy2 > nY2) {
		nRy2 = nY2;
		nRy1 = nY2 - nRh;
	}

	if (nRx1 >= 38) {
		nRx1 = 38;
	}
	if (nRy1 >= 38) {
		nRy1 = 38;
	}
	if (nRx1 <= 1) {
		nRx1 = 1;
	}
	if (nRy1 <= 1) {
		nRy1 = 1;
	}
	if (nRx2 >= 38) {
		nRx2 = 38;
	}
	if (nRy2 >= 38) {
		nRy2 = 38;
	}
	if (nRx2 <= 1) {
		nRx2 = 1;
	}
	if (nRy2 <= 1) {
		nRy2 = 1;
	}
	DefineRoom(nRx1, nRy1, nRx2, nRy2, ForceHW);

	if (ForceHW) {
		nSx1 = nRx1 + 2;
		nSy1 = nRy1 + 2;
		nSx2 = nRx2;
		nSy2 = nRy2;
	}

	nRid = nRoomCnt;
	RoomList[nRid].nRoomDest = nRDest;

	if (nRDest != 0) {
		if (nHDir == 1) {
			nHx1 = GenerateRnd(nRx2 - nRx1 - 2) + nRx1 + 1;
			nHy1 = nRy1;
			nHw = RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2;
			nHx2 = GenerateRnd(nHw) + RoomList[nRDest].nRoomx1 + 1;
			nHy2 = RoomList[nRDest].nRoomy2;
		}
		if (nHDir == 3) {
			nHx1 = GenerateRnd(nRx2 - nRx1 - 2) + nRx1 + 1;
			nHy1 = nRy2;
			nHw = RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2;
			nHx2 = GenerateRnd(nHw) + RoomList[nRDest].nRoomx1 + 1;
			nHy2 = RoomList[nRDest].nRoomy1;
		}
		if (nHDir == 2) {
			nHx1 = nRx2;
			nHy1 = GenerateRnd(nRy2 - nRy1 - 2) + nRy1 + 1;
			nHx2 = RoomList[nRDest].nRoomx1;
			nHh = RoomList[nRDest].nRoomy2 - RoomList[nRDest].nRoomy1 - 2;
			nHy2 = GenerateRnd(nHh) + RoomList[nRDest].nRoomy1 + 1;
		}
		if (nHDir == 4) {
			nHx1 = nRx1;
			nHy1 = GenerateRnd(nRy2 - nRy1 - 2) + nRy1 + 1;
			nHx2 = RoomList[nRDest].nRoomx2;
			nHh = RoomList[nRDest].nRoomy2 - RoomList[nRDest].nRoomy1 - 2;
			nHy2 = GenerateRnd(nHh) + RoomList[nRDest].nRoomy1 + 1;
		}
		HallList.push_back({ nHx1, nHy1, nHx2, nHy2, nHDir });
	}

	if (nRh > nRw) {
		CreateRoom(nX1 + 2, nY1 + 2, nRx1 - 2, nRy2 - 2, nRid, 2, false, 0, 0);
		CreateRoom(nRx2 + 2, nRy1 + 2, nX2 - 2, nY2 - 2, nRid, 4, false, 0, 0);
		CreateRoom(nX1 + 2, nRy2 + 2, nRx2 - 2, nY2 - 2, nRid, 1, false, 0, 0);
		CreateRoom(nRx1 + 2, nY1 + 2, nX2 - 2, nRy1 - 2, nRid, 3, false, 0, 0);
	} else {
		CreateRoom(nX1 + 2, nY1 + 2, nRx2 - 2, nRy1 - 2, nRid, 3, false, 0, 0);
		CreateRoom(nRx1 + 2, nRy2 + 2, nX2 - 2, nY2 - 2, nRid, 1, false, 0, 0);
		CreateRoom(nX1 + 2, nRy1 + 2, nRx1 - 2, nY2 - 2, nRid, 2, false, 0, 0);
		CreateRoom(nRx2 + 2, nY1 + 2, nX2 - 2, nRy2 - 2, nRid, 4, false, 0, 0);
	}
}

static void ConnectHall(const HALLNODE &node)
{
	int nCurrd, nDx, nDy, nRp, nOrigX1, nOrigY1, fMinusFlag, fPlusFlag;
	bool fDoneflag, fInroom;

	int nX1 = node.nHallx1;
	int nY1 = node.nHally1;
	int nX2 = node.nHallx2;
	int nY2 = node.nHally2;
	int nHd = node.nHalldir;

	fDoneflag = false;
	fMinusFlag = GenerateRnd(100);
	fPlusFlag = GenerateRnd(100);
	nOrigX1 = nX1;
	nOrigY1 = nY1;
	CreateDoorType(nX1, nY1);
	CreateDoorType(nX2, nY2);
	nCurrd = nHd;
	nX2 -= Dir_Xadd[nCurrd];
	nY2 -= Dir_Yadd[nCurrd];
	predungeon[nX2][nY2] = 44;
	fInroom = false;

	while (!fDoneflag) {
		if (nX1 >= 38 && nCurrd == 2) {
			nCurrd = 4;
		}
		if (nY1 >= 38 && nCurrd == 3) {
			nCurrd = 1;
		}
		if (nX1 <= 1 && nCurrd == 4) {
			nCurrd = 2;
		}
		if (nY1 <= 1 && nCurrd == 1) {
			nCurrd = 3;
		}
		if (predungeon[nX1][nY1] == 67 && (nCurrd == 1 || nCurrd == 4)) {
			nCurrd = 2;
		}
		if (predungeon[nX1][nY1] == 66 && (nCurrd == 1 || nCurrd == 2)) {
			nCurrd = 3;
		}
		if (predungeon[nX1][nY1] == 69 && (nCurrd == 4 || nCurrd == 3)) {
			nCurrd = 1;
		}
		if (predungeon[nX1][nY1] == 65 && (nCurrd == 2 || nCurrd == 3)) {
			nCurrd = 4;
		}
		nX1 += Dir_Xadd[nCurrd];
		nY1 += Dir_Yadd[nCurrd];
		if (predungeon[nX1][nY1] == 32) {
			if (fInroom) {
				CreateDoorType(nX1 - Dir_Xadd[nCurrd], nY1 - Dir_Yadd[nCurrd]);
			} else {
				if (fMinusFlag < 50) {
					if (nCurrd != 1 && nCurrd != 3) {
						PlaceHallExt(nX1, nY1 - 1);
					} else {
						PlaceHallExt(nX1 - 1, nY1);
					}
				}
				if (fPlusFlag < 50) {
					if (nCurrd != 1 && nCurrd != 3) {
						PlaceHallExt(nX1, nY1 + 1);
					} else {
						PlaceHallExt(nX1 + 1, nY1);
					}
				}
			}
			predungeon[nX1][nY1] = 44;
			fInroom = false;
		} else {
			if (!fInroom && predungeon[nX1][nY1] == 35) {
				CreateDoorType(nX1, nY1);
			}
			if (predungeon[nX1][nY1] != 44) {
				fInroom = true;
			}
		}
		nDx = abs(nX2 - nX1);
		nDy = abs(nY2 - nY1);
		if (nDx > nDy) {
			nRp = 2 * nDx;
			if (nRp > 30) {
				nRp = 30;
			}
			if (GenerateRnd(100) < nRp) {
				if (nX2 <= nX1 || nX1 >= DMAXX) {
					nCurrd = 4;
				} else {
					nCurrd = 2;
				}
			}
		} else {
			nRp = 5 * nDy;
			if (nRp > 80) {
				nRp = 80;
			}
			if (GenerateRnd(100) < nRp) {
				if (nY2 <= nY1 || nY1 >= DMAXY) {
					nCurrd = 1;
				} else {
					nCurrd = 3;
				}
			}
		}
		if (nDy < 10 && nX1 == nX2 && (nCurrd == 2 || nCurrd == 4)) {
			if (nY2 <= nY1 || nY1 >= DMAXY) {
				nCurrd = 1;
			} else {
				nCurrd = 3;
			}
		}
		if (nDx < 10 && nY1 == nY2 && (nCurrd == 1 || nCurrd == 3)) {
			if (nX2 <= nX1 || nX1 >= DMAXX) {
				nCurrd = 4;
			} else {
				nCurrd = 2;
			}
		}
		if (nDy == 1 && nDx > 1 && (nCurrd == 1 || nCurrd == 3)) {
			if (nX2 <= nX1 || nX1 >= DMAXX) {
				nCurrd = 4;
			} else {
				nCurrd = 2;
			}
		}
		if (nDx == 1 && nDy > 1 && (nCurrd == 2 || nCurrd == 4)) {
			if (nY2 <= nY1 || nX1 >= DMAXX) {
				nCurrd = 1;
			} else {
				nCurrd = 3;
			}
		}
		if (nDx == 0 && predungeon[nX1][nY1] != 32 && (nCurrd == 2 || nCurrd == 4)) {
			if (nX2 <= nOrigX1 || nX1 >= DMAXX) {
				nCurrd = 1;
			} else {
				nCurrd = 3;
			}
		}
		if (nDy == 0 && predungeon[nX1][nY1] != 32 && (nCurrd == 1 || nCurrd == 3)) {
			if (nY2 <= nOrigY1 || nY1 >= DMAXY) {
				nCurrd = 4;
			} else {
				nCurrd = 2;
			}
		}
		if (nX1 == nX2 && nY1 == nY2) {
			fDoneflag = true;
		}
	}
}

static void DoPatternCheck(int i, int j)
{
	int k, l, x, y, nOk;

	for (k = 0; Patterns[k][4] != 255; k++) {
		x = i - 1;
		y = j - 1;
		nOk = 254;
		for (l = 0; l < 9 && nOk == 254; l++) {
			nOk = 255;
			if (l == 3 || l == 6) {
				y++;
				x = i - 1;
			}
			if (x >= 0 && x < DMAXX && y >= 0 && y < DMAXY) {
				switch (Patterns[k][l]) {
				case 0:
					nOk = 254;
					break;
				case 1:
					if (predungeon[x][y] == 35) {
						nOk = 254;
					}
					break;
				case 2:
					if (predungeon[x][y] == 46) {
						nOk = 254;
					}
					break;
				case 4:
					if (predungeon[x][y] == 32) {
						nOk = 254;
					}
					break;
				case 3:
					if (predungeon[x][y] == 68) {
						nOk = 254;
					}
					break;
				case 5:
					if (predungeon[x][y] == 68 || predungeon[x][y] == 46) {
						nOk = 254;
					}
					break;
				case 6:
					if (predungeon[x][y] == 68 || predungeon[x][y] == 35) {
						nOk = 254;
					}
					break;
				case 7:
					if (predungeon[x][y] == 32 || predungeon[x][y] == 46) {
						nOk = 254;
					}
					break;
				case 8:
					if (predungeon[x][y] == 68 || predungeon[x][y] == 35 || predungeon[x][y] == 46) {
						nOk = 254;
					}
					break;
				}
			} else {
				nOk = 254;
			}
			x++;
		}
		if (nOk == 254) {
			dungeon[i][j] = Patterns[k][9];
		}
	}
}

static void L2TileFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 3) {
				dungeon[i][j + 1] = 1;
			}
			if (dungeon[i][j] == 3 && dungeon[i][j + 1] == 1) {
				dungeon[i][j + 1] = 3;
			}
			if (dungeon[i][j] == 3 && dungeon[i + 1][j] == 7) {
				dungeon[i + 1][j] = 3;
			}
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 3) {
				dungeon[i + 1][j] = 2;
			}
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 14) {
				dungeon[i + 1][j] = 16;
			}
		}
	}
}

static bool DL2_Cont(bool x1f, bool y1f, bool x2f, bool y2f)
{
	if (x1f && x2f && y1f && y2f) {
		return false;
	}
	if (x1f && x2f && (y1f || y2f)) {
		return true;
	}
	if (y1f && y2f && (x1f || x2f)) {
		return true;
	}

	return false;
}

static int DL2_NumNoChar()
{
	int t, ii, jj;

	t = 0;
	for (jj = 0; jj < DMAXY; jj++) {
		for (ii = 0; ii < DMAXX; ii++) {
			if (predungeon[ii][jj] == 32) {
				t++;
			}
		}
	}

	return t;
}

static void DL2_DrawRoom(int x1, int y1, int x2, int y2)
{
	int ii, jj;

	for (jj = y1; jj <= y2; jj++) {
		for (ii = x1; ii <= x2; ii++) {
			predungeon[ii][jj] = 46;
		}
	}
	for (jj = y1; jj <= y2; jj++) {
		predungeon[x1][jj] = 35;
		predungeon[x2][jj] = 35;
	}
	for (ii = x1; ii <= x2; ii++) {
		predungeon[ii][y1] = 35;
		predungeon[ii][y2] = 35;
	}
}

static void DL2_KnockWalls(int x1, int y1, int x2, int y2)
{
	int ii, jj;

	for (ii = x1 + 1; ii < x2; ii++) {
		if (predungeon[ii][y1 - 1] == 46 && predungeon[ii][y1 + 1] == 46) {
			predungeon[ii][y1] = 46;
		}
		if (predungeon[ii][y2 - 1] == 46 && predungeon[ii][y2 + 1] == 46) {
			predungeon[ii][y2] = 46;
		}
		if (predungeon[ii][y1 - 1] == 68) {
			predungeon[ii][y1 - 1] = 46;
		}
		if (predungeon[ii][y2 + 1] == 68) {
			predungeon[ii][y2 + 1] = 46;
		}
	}
	for (jj = y1 + 1; jj < y2; jj++) {
		if (predungeon[x1 - 1][jj] == 46 && predungeon[x1 + 1][jj] == 46) {
			predungeon[x1][jj] = 46;
		}
		if (predungeon[x2 - 1][jj] == 46 && predungeon[x2 + 1][jj] == 46) {
			predungeon[x2][jj] = 46;
		}
		if (predungeon[x1 - 1][jj] == 68) {
			predungeon[x1 - 1][jj] = 46;
		}
		if (predungeon[x2 + 1][jj] == 68) {
			predungeon[x2 + 1][jj] = 46;
		}
	}
}

static bool DL2_FillVoids()
{
	int ii, jj, xx, yy, x1, x2, y1, y2;
	bool xf1, xf2, yf1, yf2;
	int to;

	to = 0;
	while (DL2_NumNoChar() > 700 && to < 100) {
		xx = GenerateRnd(38) + 1;
		yy = GenerateRnd(38) + 1;
		if (predungeon[xx][yy] != 35) {
			continue;
		}
		xf1 = xf2 = yf1 = yf2 = false;
		if (predungeon[xx - 1][yy] == 32 && predungeon[xx + 1][yy] == 46) {
			if (predungeon[xx + 1][yy - 1] == 46
			    && predungeon[xx + 1][yy + 1] == 46
			    && predungeon[xx - 1][yy - 1] == 32
			    && predungeon[xx - 1][yy + 1] == 32) {
				xf1 = yf1 = yf2 = true;
			}
		} else if (predungeon[xx + 1][yy] == 32 && predungeon[xx - 1][yy] == 46) {
			if (predungeon[xx - 1][yy - 1] == 46
			    && predungeon[xx - 1][yy + 1] == 46
			    && predungeon[xx + 1][yy - 1] == 32
			    && predungeon[xx + 1][yy + 1] == 32) {
				xf2 = yf1 = yf2 = true;
			}
		} else if (predungeon[xx][yy - 1] == 32 && predungeon[xx][yy + 1] == 46) {
			if (predungeon[xx - 1][yy + 1] == 46
			    && predungeon[xx + 1][yy + 1] == 46
			    && predungeon[xx - 1][yy - 1] == 32
			    && predungeon[xx + 1][yy - 1] == 32) {
				yf1 = xf1 = xf2 = true;
			}
		} else if (predungeon[xx][yy + 1] == 32 && predungeon[xx][yy - 1] == 46) {
			if (predungeon[xx - 1][yy - 1] == 46
			    && predungeon[xx + 1][yy - 1] == 46
			    && predungeon[xx - 1][yy + 1] == 32
			    && predungeon[xx + 1][yy + 1] == 32) {
				yf2 = xf1 = xf2 = true;
			}
		}
		if (DL2_Cont(xf1, yf1, xf2, yf2)) {
			if (xf1) {
				x1 = xx - 1;
			} else {
				x1 = xx;
			}
			if (xf2) {
				x2 = xx + 1;
			} else {
				x2 = xx;
			}
			if (yf1) {
				y1 = yy - 1;
			} else {
				y1 = yy;
			}
			if (yf2) {
				y2 = yy + 1;
			} else {
				y2 = yy;
			}
			if (!xf1) {
				while (yf1 || yf2) {
					if (y1 == 0) {
						yf1 = false;
					}
					if (y2 == DMAXY - 1) {
						yf2 = false;
					}
					if (y2 - y1 >= 14) {
						yf1 = false;
						yf2 = false;
					}
					if (yf1) {
						y1--;
					}
					if (yf2) {
						y2++;
					}
					if (predungeon[x2][y1] != 32) {
						yf1 = false;
					}
					if (predungeon[x2][y2] != 32) {
						yf2 = false;
					}
				}
				y1 += 2;
				y2 -= 2;
				if (y2 - y1 > 5) {
					while (xf2) {
						if (x2 == 39) {
							xf2 = false;
						}
						if (x2 - x1 >= 12) {
							xf2 = false;
						}
						for (jj = y1; jj <= y2; jj++) {
							if (predungeon[x2][jj] != 32) {
								xf2 = false;
							}
						}
						if (xf2) {
							x2++;
						}
					}
					x2 -= 2;
					if (x2 - x1 > 5) {
						DL2_DrawRoom(x1, y1, x2, y2);
						DL2_KnockWalls(x1, y1, x2, y2);
					}
				}
			} else if (!xf2) {
				while (yf1 || yf2) {
					if (y1 == 0) {
						yf1 = false;
					}
					if (y2 == DMAXY - 1) {
						yf2 = false;
					}
					if (y2 - y1 >= 14) {
						yf1 = false;
						yf2 = false;
					}
					if (yf1) {
						y1--;
					}
					if (yf2) {
						y2++;
					}
					if (predungeon[x1][y1] != 32) {
						yf1 = false;
					}
					if (predungeon[x1][y2] != 32) {
						yf2 = false;
					}
				}
				y1 += 2;
				y2 -= 2;
				if (y2 - y1 > 5) {
					while (xf1) {
						if (x1 == 0) {
							xf1 = false;
						}
						if (x2 - x1 >= 12) {
							xf1 = false;
						}
						for (jj = y1; jj <= y2; jj++) {
							if (predungeon[x1][jj] != 32) {
								xf1 = false;
							}
						}
						if (xf1) {
							x1--;
						}
					}
					x1 += 2;
					if (x2 - x1 > 5) {
						DL2_DrawRoom(x1, y1, x2, y2);
						DL2_KnockWalls(x1, y1, x2, y2);
					}
				}
			} else if (!yf1) {
				while (xf1 || xf2) {
					if (x1 == 0) {
						xf1 = false;
					}
					if (x2 == DMAXX - 1) {
						xf2 = false;
					}
					if (x2 - x1 >= 14) {
						xf1 = false;
						xf2 = false;
					}
					if (xf1) {
						x1--;
					}
					if (xf2) {
						x2++;
					}
					if (predungeon[x1][y2] != 32) {
						xf1 = false;
					}
					if (predungeon[x2][y2] != 32) {
						xf2 = false;
					}
				}
				x1 += 2;
				x2 -= 2;
				if (x2 - x1 > 5) {
					while (yf2) {
						if (y2 == DMAXY - 1) {
							yf2 = false;
						}
						if (y2 - y1 >= 12) {
							yf2 = false;
						}
						for (ii = x1; ii <= x2; ii++) {
							if (predungeon[ii][y2] != 32) {
								yf2 = false;
							}
						}
						if (yf2) {
							y2++;
						}
					}
					y2 -= 2;
					if (y2 - y1 > 5) {
						DL2_DrawRoom(x1, y1, x2, y2);
						DL2_KnockWalls(x1, y1, x2, y2);
					}
				}
			} else if (!yf2) {
				while (xf1 || xf2) {
					if (x1 == 0) {
						xf1 = false;
					}
					if (x2 == DMAXX - 1) {
						xf2 = false;
					}
					if (x2 - x1 >= 14) {
						xf1 = false;
						xf2 = false;
					}
					if (xf1) {
						x1--;
					}
					if (xf2) {
						x2++;
					}
					if (predungeon[x1][y1] != 32) {
						xf1 = false;
					}
					if (predungeon[x2][y1] != 32) {
						xf2 = false;
					}
				}
				x1 += 2;
				x2 -= 2;
				if (x2 - x1 > 5) {
					while (yf1) {
						if (y1 == 0) {
							yf1 = false;
						}
						if (y2 - y1 >= 12) {
							yf1 = false;
						}
						for (ii = x1; ii <= x2; ii++) {
							if (predungeon[ii][y1] != 32) {
								yf1 = false;
							}
						}
						if (yf1) {
							y1--;
						}
					}
					y1 += 2;
					if (y2 - y1 > 5) {
						DL2_DrawRoom(x1, y1, x2, y2);
						DL2_KnockWalls(x1, y1, x2, y2);
					}
				}
			}
		}
		to++;
	}

	return DL2_NumNoChar() <= 700;
}

static bool CreateDungeon()
{
	int i, j, ForceH, ForceW;
	bool ForceHW;

	ForceW = 0;
	ForceH = 0;
	ForceHW = false;

	switch (currlevel) {
	case 5:
		if (quests[Q_BLOOD]._qactive != QUEST_NOTAVAIL) {
			ForceHW = true;
			ForceH = 20;
			ForceW = 14;
		}
		break;
	case 6:
		if (quests[Q_SCHAMB]._qactive != QUEST_NOTAVAIL) {
			ForceHW = true;
			ForceW = 10;
			ForceH = 10;
		}
		break;
	case 7:
		if (quests[Q_BLIND]._qactive != QUEST_NOTAVAIL) {
			ForceHW = true;
			ForceW = 15;
			ForceH = 15;
		}
		break;
	case 8:
		break;
	}

	CreateRoom(2, 2, DMAXX - 1, DMAXY - 1, 0, 0, ForceHW, ForceH, ForceW);

	while (!HallList.empty()) {
		ConnectHall(HallList.front());
		HallList.pop_front();
	}

	for (j = 0; j < DMAXY; j++) {     /// BUGFIX: change '<=' to '<' (fixed)
		for (i = 0; i < DMAXX; i++) { /// BUGFIX: change '<=' to '<' (fixed)
			if (predungeon[i][j] == 67) {
				predungeon[i][j] = 35;
			}
			if (predungeon[i][j] == 66) {
				predungeon[i][j] = 35;
			}
			if (predungeon[i][j] == 69) {
				predungeon[i][j] = 35;
			}
			if (predungeon[i][j] == 65) {
				predungeon[i][j] = 35;
			}
			if (predungeon[i][j] == 44) {
				predungeon[i][j] = 46;
				if (predungeon[i - 1][j - 1] == 32) {
					predungeon[i - 1][j - 1] = 35;
				}
				if (predungeon[i - 1][j] == 32) {
					predungeon[i - 1][j] = 35;
				}
				if (predungeon[i - 1][1 + j] == 32) {
					predungeon[i - 1][1 + j] = 35;
				}
				if (predungeon[i + 1][j - 1] == 32) {
					predungeon[i + 1][j - 1] = 35;
				}
				if (predungeon[i + 1][j] == 32) {
					predungeon[i + 1][j] = 35;
				}
				if (predungeon[i + 1][1 + j] == 32) {
					predungeon[i + 1][1 + j] = 35;
				}
				if (predungeon[i][j - 1] == 32) {
					predungeon[i][j - 1] = 35;
				}
				if (predungeon[i][j + 1] == 32) {
					predungeon[i][j + 1] = 35;
				}
			}
		}
	}

	if (!DL2_FillVoids()) {
		return false;
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			DoPatternCheck(i, j);
		}
	}

	return true;
}

static void DRLG_L2Pass3()
{
	DRLG_LPass3(12 - 1);
}

static void DRLG_L2FTVR(int i, int j, int x, int y, int d)
{
	if (dTransVal[x][y] != 0 || dungeon[i][j] != 3) {
		if (d == 1) {
			dTransVal[x][y] = TransVal;
			dTransVal[x][y + 1] = TransVal;
		}
		if (d == 2) {
			dTransVal[x + 1][y] = TransVal;
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if (d == 3) {
			dTransVal[x][y] = TransVal;
			dTransVal[x + 1][y] = TransVal;
		}
		if (d == 4) {
			dTransVal[x][y + 1] = TransVal;
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if (d == 5) {
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if (d == 6) {
			dTransVal[x][y + 1] = TransVal;
		}
		if (d == 7) {
			dTransVal[x + 1][y] = TransVal;
		}
		if (d == 8) {
			dTransVal[x][y] = TransVal;
		}
	} else {
		dTransVal[x][y] = TransVal;
		dTransVal[x + 1][y] = TransVal;
		dTransVal[x][y + 1] = TransVal;
		dTransVal[x + 1][y + 1] = TransVal;
		DRLG_L2FTVR(i + 1, j, x + 2, y, 1);
		DRLG_L2FTVR(i - 1, j, x - 2, y, 2);
		DRLG_L2FTVR(i, j + 1, x, y + 2, 3);
		DRLG_L2FTVR(i, j - 1, x, y - 2, 4);
		DRLG_L2FTVR(i - 1, j - 1, x - 2, y - 2, 5);
		DRLG_L2FTVR(i + 1, j - 1, x + 2, y - 2, 6);
		DRLG_L2FTVR(i - 1, j + 1, x - 2, y + 2, 7);
		DRLG_L2FTVR(i + 1, j + 1, x + 2, y + 2, 8);
	}
}

static void DRLG_L2FloodTVal()
{
	int i, j, xx, yy;

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 3 && dTransVal[xx][yy] == 0) {
				DRLG_L2FTVR(i, j, xx, yy, 0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

static void DRLG_L2TransFix()
{
	int i, j, xx, yy;

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 14 && dungeon[i][j - 1] == 10) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 11) {
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 10) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 11) {
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 16) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			xx += 2;
		}
		yy += 2;
	}
}

static void L2DirtFix()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 13 && dungeon[i + 1][j] != 11) {
				dungeon[i][j] = 146;
			}
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] != 11) {
				dungeon[i][j] = 144;
			}
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] != 11) {
				dungeon[i][j] = 148;
			}
			if (dungeon[i][j] == 10 && dungeon[i][j + 1] != 10) {
				dungeon[i][j] = 143;
			}
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] != 10) {
				dungeon[i][j] = 146;
			}
			if (dungeon[i][j] == 14 && dungeon[i][j + 1] != 15) {
				dungeon[i][j] = 147;
			}
		}
	}
}

void L2LockoutFix()
{
	int i, j;
	bool doorok;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 4 && dungeon[i - 1][j] != 3) {
				dungeon[i][j] = 1;
			}
			if (dungeon[i][j] == 5 && dungeon[i][j - 1] != 3) {
				dungeon[i][j] = 2;
			}
		}
	}
	for (j = 1; j < DMAXY - 1; j++) {
		for (i = 1; i < DMAXX - 1; i++) {
			if ((dflags[i][j] & DLRG_PROTECTED) != 0) {
				continue;
			}
			if ((dungeon[i][j] == 2 || dungeon[i][j] == 5) && dungeon[i][j - 1] == 3 && dungeon[i][j + 1] == 3) {
				doorok = false;
				while (true) {
					if (dungeon[i][j] != 2 && dungeon[i][j] != 5) {
						break;
					}
					if (dungeon[i][j - 1] != 3 || dungeon[i][j + 1] != 3) {
						break;
					}
					if (dungeon[i][j] == 5) {
						doorok = true;
					}
					i++;
				}
				if (!doorok && (dflags[i - 1][j] & DLRG_PROTECTED) == 0) {
					dungeon[i - 1][j] = 5;
				}
			}
		}
	}
	for (j = 1; j < DMAXX - 1; j++) { /* check: might be flipped */
		for (i = 1; i < DMAXY - 1; i++) {
			if ((dflags[j][i] & DLRG_PROTECTED) != 0) {
				continue;
			}
			if ((dungeon[j][i] == 1 || dungeon[j][i] == 4) && dungeon[j - 1][i] == 3 && dungeon[j + 1][i] == 3) {
				doorok = false;
				while (true) {
					if (dungeon[j][i] != 1 && dungeon[j][i] != 4) {
						break;
					}
					if (dungeon[j - 1][i] != 3 || dungeon[j + 1][i] != 3) {
						break;
					}
					if (dungeon[j][i] == 4) {
						doorok = true;
					}
					i++;
				}
				if (!doorok && (dflags[j][i - 1] & DLRG_PROTECTED) == 0) {
					dungeon[j][i - 1] = 4;
				}
			}
		}
	}
}

void L2DoorFix()
{
	int i, j;

	for (j = 1; j < DMAXY; j++) {
		for (i = 1; i < DMAXX; i++) {
			if (dungeon[i][j] == 4 && dungeon[i][j - 1] == 3) {
				dungeon[i][j] = 7;
			}
			if (dungeon[i][j] == 5 && dungeon[i - 1][j] == 3) {
				dungeon[i][j] = 9;
			}
		}
	}
}

static void DRLG_L2(lvl_entry entry)
{
	int i, j;
	bool doneflag;

	doneflag = false;
	while (!doneflag) {
		nRoomCnt = 0;
		InitDungeon();
		DRLG_InitTrans();
		if (!CreateDungeon()) {
			continue;
		}
		L2TileFix();
		if (setloadflag) {
			DRLG_L2SetRoom(nSx1, nSy1);
		}
		DRLG_L2FloodTVal();
		DRLG_L2TransFix();
		if (entry == ENTRY_MAIN) {
			doneflag = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, true, 0);
			if (doneflag) {
				doneflag = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, false, 1);
				if (doneflag && currlevel == 5) {
					doneflag = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, false, 6);
				}
			}
			ViewY -= 2;
		} else if (entry == ENTRY_PREV) {
			doneflag = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, false, 0);
			if (doneflag) {
				doneflag = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, true, 1);
				if (doneflag && currlevel == 5) {
					doneflag = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, false, 6);
				}
			}
			ViewX--;
		} else {
			doneflag = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, false, 0);
			if (doneflag) {
				doneflag = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, false, 1);
				if (doneflag && currlevel == 5) {
					doneflag = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, true, 6);
				}
			}
			ViewY -= 2;
		}
	}

	L2LockoutFix();
	L2DoorFix();
	L2DirtFix();

	DRLG_PlaceThemeRooms(6, 10, 3, 0, false);
	DRLG_L2PlaceRndSet(CTRDOOR1, 100);
	DRLG_L2PlaceRndSet(CTRDOOR2, 100);
	DRLG_L2PlaceRndSet(CTRDOOR3, 100);
	DRLG_L2PlaceRndSet(CTRDOOR4, 100);
	DRLG_L2PlaceRndSet(CTRDOOR5, 100);
	DRLG_L2PlaceRndSet(CTRDOOR6, 100);
	DRLG_L2PlaceRndSet(CTRDOOR7, 100);
	DRLG_L2PlaceRndSet(CTRDOOR8, 100);
	DRLG_L2PlaceRndSet(VARCH33, 100);
	DRLG_L2PlaceRndSet(VARCH34, 100);
	DRLG_L2PlaceRndSet(VARCH35, 100);
	DRLG_L2PlaceRndSet(VARCH36, 100);
	DRLG_L2PlaceRndSet(VARCH37, 100);
	DRLG_L2PlaceRndSet(VARCH38, 100);
	DRLG_L2PlaceRndSet(VARCH39, 100);
	DRLG_L2PlaceRndSet(VARCH40, 100);
	DRLG_L2PlaceRndSet(VARCH1, 100);
	DRLG_L2PlaceRndSet(VARCH2, 100);
	DRLG_L2PlaceRndSet(VARCH3, 100);
	DRLG_L2PlaceRndSet(VARCH4, 100);
	DRLG_L2PlaceRndSet(VARCH5, 100);
	DRLG_L2PlaceRndSet(VARCH6, 100);
	DRLG_L2PlaceRndSet(VARCH7, 100);
	DRLG_L2PlaceRndSet(VARCH8, 100);
	DRLG_L2PlaceRndSet(VARCH9, 100);
	DRLG_L2PlaceRndSet(VARCH10, 100);
	DRLG_L2PlaceRndSet(VARCH11, 100);
	DRLG_L2PlaceRndSet(VARCH12, 100);
	DRLG_L2PlaceRndSet(VARCH13, 100);
	DRLG_L2PlaceRndSet(VARCH14, 100);
	DRLG_L2PlaceRndSet(VARCH15, 100);
	DRLG_L2PlaceRndSet(VARCH16, 100);
	DRLG_L2PlaceRndSet(VARCH17, 100);
	DRLG_L2PlaceRndSet(VARCH18, 100);
	DRLG_L2PlaceRndSet(VARCH19, 100);
	DRLG_L2PlaceRndSet(VARCH20, 100);
	DRLG_L2PlaceRndSet(VARCH21, 100);
	DRLG_L2PlaceRndSet(VARCH22, 100);
	DRLG_L2PlaceRndSet(VARCH23, 100);
	DRLG_L2PlaceRndSet(VARCH24, 100);
	DRLG_L2PlaceRndSet(VARCH25, 100);
	DRLG_L2PlaceRndSet(VARCH26, 100);
	DRLG_L2PlaceRndSet(VARCH27, 100);
	DRLG_L2PlaceRndSet(VARCH28, 100);
	DRLG_L2PlaceRndSet(VARCH29, 100);
	DRLG_L2PlaceRndSet(VARCH30, 100);
	DRLG_L2PlaceRndSet(VARCH31, 100);
	DRLG_L2PlaceRndSet(VARCH32, 100);
	DRLG_L2PlaceRndSet(HARCH1, 100);
	DRLG_L2PlaceRndSet(HARCH2, 100);
	DRLG_L2PlaceRndSet(HARCH3, 100);
	DRLG_L2PlaceRndSet(HARCH4, 100);
	DRLG_L2PlaceRndSet(HARCH5, 100);
	DRLG_L2PlaceRndSet(HARCH6, 100);
	DRLG_L2PlaceRndSet(HARCH7, 100);
	DRLG_L2PlaceRndSet(HARCH8, 100);
	DRLG_L2PlaceRndSet(HARCH9, 100);
	DRLG_L2PlaceRndSet(HARCH10, 100);
	DRLG_L2PlaceRndSet(HARCH11, 100);
	DRLG_L2PlaceRndSet(HARCH12, 100);
	DRLG_L2PlaceRndSet(HARCH13, 100);
	DRLG_L2PlaceRndSet(HARCH14, 100);
	DRLG_L2PlaceRndSet(HARCH15, 100);
	DRLG_L2PlaceRndSet(HARCH16, 100);
	DRLG_L2PlaceRndSet(HARCH17, 100);
	DRLG_L2PlaceRndSet(HARCH18, 100);
	DRLG_L2PlaceRndSet(HARCH19, 100);
	DRLG_L2PlaceRndSet(HARCH20, 100);
	DRLG_L2PlaceRndSet(HARCH21, 100);
	DRLG_L2PlaceRndSet(HARCH22, 100);
	DRLG_L2PlaceRndSet(HARCH23, 100);
	DRLG_L2PlaceRndSet(HARCH24, 100);
	DRLG_L2PlaceRndSet(HARCH25, 100);
	DRLG_L2PlaceRndSet(HARCH26, 100);
	DRLG_L2PlaceRndSet(HARCH27, 100);
	DRLG_L2PlaceRndSet(HARCH28, 100);
	DRLG_L2PlaceRndSet(HARCH29, 100);
	DRLG_L2PlaceRndSet(HARCH30, 100);
	DRLG_L2PlaceRndSet(HARCH31, 100);
	DRLG_L2PlaceRndSet(HARCH32, 100);
	DRLG_L2PlaceRndSet(HARCH33, 100);
	DRLG_L2PlaceRndSet(HARCH34, 100);
	DRLG_L2PlaceRndSet(HARCH35, 100);
	DRLG_L2PlaceRndSet(HARCH36, 100);
	DRLG_L2PlaceRndSet(HARCH37, 100);
	DRLG_L2PlaceRndSet(HARCH38, 100);
	DRLG_L2PlaceRndSet(HARCH39, 100);
	DRLG_L2PlaceRndSet(HARCH40, 100);
	DRLG_L2PlaceRndSet(CRUSHCOL, 99);
	DRLG_L2PlaceRndSet(RUINS1, 10);
	DRLG_L2PlaceRndSet(RUINS2, 10);
	DRLG_L2PlaceRndSet(RUINS3, 10);
	DRLG_L2PlaceRndSet(RUINS4, 10);
	DRLG_L2PlaceRndSet(RUINS5, 10);
	DRLG_L2PlaceRndSet(RUINS6, 10);
	DRLG_L2PlaceRndSet(RUINS7, 50);
	DRLG_L2PlaceRndSet(PANCREAS1, 1);
	DRLG_L2PlaceRndSet(PANCREAS2, 1);
	DRLG_L2PlaceRndSet(BIG1, 3);
	DRLG_L2PlaceRndSet(BIG2, 3);
	DRLG_L2PlaceRndSet(BIG3, 3);
	DRLG_L2PlaceRndSet(BIG4, 3);
	DRLG_L2PlaceRndSet(BIG5, 3);
	DRLG_L2PlaceRndSet(BIG6, 20);
	DRLG_L2PlaceRndSet(BIG7, 20);
	DRLG_L2PlaceRndSet(BIG8, 3);
	DRLG_L2PlaceRndSet(BIG9, 20);
	DRLG_L2PlaceRndSet(BIG10, 20);
	DRLG_L2Subs();
	DRLG_L2Shadows();

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}

	DRLG_Init_Globals();
	DRLG_CheckQuests(nSx1, nSy1);
}

static void DRLG_InitL2Vals()
{
	int i, j, pc;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 541) {
				pc = 5;
			} else if (dPiece[i][j] == 178) {
				pc = 5;
			} else if (dPiece[i][j] == 551) {
				pc = 5;
			} else if (dPiece[i][j] == 542) {
				pc = 6;
			} else if (dPiece[i][j] == 553) {
				pc = 6;
			} else {
				continue;
			}
			dSpecial[i][j] = pc;
		}
	}
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 132) {
				dSpecial[i][j + 1] = 2;
				dSpecial[i][j + 2] = 1;
			} else if (dPiece[i][j] == 135 || dPiece[i][j] == 139) {
				dSpecial[i + 1][j] = 3;
				dSpecial[i + 2][j] = 4;
			}
		}
	}
}

static void LoadL2DungeonData(const uint16_t *dunData)
{
	InitDungeon();
	DRLG_InitTrans();

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 12;
			dflags[i][j] = 0;
		}
	}

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(*tileLayer);
			tileLayer++;
			if (tileId != 0) {
				dungeon[i][j] = tileId;
				dflags[i][j] |= DLRG_PROTECTED;
			} else {
				dungeon[i][j] = 3;
			}
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 0) {
				dungeon[i][j] = 12;
			}
		}
	}
}

void LoadL2Dungeon(const char *path, int vx, int vy)
{
	auto dunData = LoadFileInMem<uint16_t>(path);

	LoadL2DungeonData(dunData.get());

	DRLG_L2Pass3();
	DRLG_Init_Globals();

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			int pc = 0;
			if (dPiece[i][j] == 541) {
				pc = 5;
			}
			if (dPiece[i][j] == 178) {
				pc = 5;
			}
			if (dPiece[i][j] == 551) {
				pc = 5;
			}
			if (dPiece[i][j] == 542) {
				pc = 6;
			}
			if (dPiece[i][j] == 553) {
				pc = 6;
			}
			dSpecial[i][j] = pc;
		}
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 132) {
				dSpecial[i][j + 1] = 2;
				dSpecial[i][j + 2] = 1;
			} else if (dPiece[i][j] == 135 || dPiece[i][j] == 139) {
				dSpecial[i + 1][j] = 3;
				dSpecial[i + 2][j] = 4;
			}
		}
	}

	ViewX = vx;
	ViewY = vy;

	SetMapMonsters(dunData.get(), { 0, 0 });
	SetMapObjects(dunData.get(), 0, 0);
}

void LoadPreL2Dungeon(const char *path)
{
	{
		auto dunData = LoadFileInMem<uint16_t>(path);
		LoadL2DungeonData(dunData.get());
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}
}

void CreateL2Dungeon(uint32_t rseed, lvl_entry entry)
{
	if (!gbIsMultiplayer) {
		if (currlevel == 7 && quests[Q_BLIND]._qactive == QUEST_NOTAVAIL) {
			currlevel = 6;
			CreateL2Dungeon(glSeedTbl[6], ENTRY_LOAD);
			currlevel = 7;
		}
		if (currlevel == 8) {
			if (quests[Q_BLIND]._qactive == QUEST_NOTAVAIL) {
				currlevel = 6;
				CreateL2Dungeon(glSeedTbl[6], ENTRY_LOAD);
				currlevel = 8;
			} else {
				currlevel = 7;
				CreateL2Dungeon(glSeedTbl[7], ENTRY_LOAD);
				currlevel = 8;
			}
		}
	}

	SetRndSeed(rseed);

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_LoadL2SP();
	DRLG_L2(entry);
	DRLG_L2Pass3();
	DRLG_FreeL2SP();
	DRLG_InitL2Vals();
	DRLG_SetPC();
}

} // namespace devilution
