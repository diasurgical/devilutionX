/**
 * @file drlg_l2.cpp
 *
 * Implementation of the catacombs level generation algorithms.
 */
#include "drlg_l2.h"

#include <algorithm>
#include <list>

#include "diablo.h"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "engine/size.hpp"
#include "gendung.h"
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
const int DirXadd[5] = { 0, 0, 1, 0, -1 };
const int DirYadd[5] = { 0, -1, 0, 1, 0 };
const ShadowStruct SPATSL2[2] = { { 6, 3, 0, 3, 48, 0, 50 }, { 9, 3, 0, 3, 48, 0, 50 } };
// short word_48489A = 0;

const BYTE BTYPESL2[161] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 17, 18, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
const BYTE BSTYPESL2[161] = { 0, 1, 2, 3, 0, 0, 6, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 6, 6, 6, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1, 1, 1, 1, 6, 2, 2, 2, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 3, 3, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

struct Miniset {
	Size size;
	/* these are indexed as [y][x] */
	unsigned char search[4][5];
	unsigned char replace[4][5];

	bool matches(Point position) const
	{
		for (int yy = 0; yy < size.height; yy++) {
			for (int xx = 0; xx < size.width; xx++) {
				if (search[yy][xx] != 0 && dungeon[xx + position.x][yy + position.y] != search[yy][xx])
					return false;
				if (dflags[xx + position.x][yy + position.y] != 0)
					return false;
			}
		}
		return true;
	}

	void place(Point position) const
	{
		for (int y = 0; y < size.height; y++) {
			for (int x = 0; x < size.width; x++) {
				if (replace[y][x] != 0)
					dungeon[x + position.x][y + position.y] = replace[y][x];
			}
		}
	}
};

/** Miniset: Arch vertical. */
const Miniset VARCH1 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 1 },
	    { 3, 4 },
	    { 0, 7 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH2 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 1 },
	    { 3, 4 },
	    { 0, 8 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH3 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 1 },
	    { 3, 4 },
	    { 0, 6 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH4 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 1 },
	    { 3, 4 },
	    { 0, 9 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH5 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 1 },
	    { 3, 4 },
	    { 0, 14 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH6 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 1 },
	    { 3, 4 },
	    { 0, 13 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH7 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 1 },
	    { 3, 4 },
	    { 0, 16 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH8 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 1 },
	    { 3, 4 },
	    { 0, 15 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - corner. */
const Miniset VARCH9 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 7 },
	},
	{
	    { 48, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - corner. */
const Miniset VARCH10 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 8 },
	},
	{
	    { 48, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - corner. */
const Miniset VARCH11 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 6 },
	},
	{
	    { 48, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - corner. */
const Miniset VARCH12 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 9 },
	},
	{
	    { 48, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - corner. */
const Miniset VARCH13 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 14 },
	},
	{
	    { 48, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - corner. */
const Miniset VARCH14 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 13 },
	},
	{
	    { 48, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - corner. */
const Miniset VARCH15 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 16 },
	},
	{
	    { 48, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - corner. */
const Miniset VARCH16 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 15 },
	},
	{
	    { 48, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - open wall. */
const Miniset VARCH17 {
	{ 2, 3 },
	{
	    { 2, 7 },
	    { 3, 4 },
	    { 0, 7 },
	},
	{
	    { 141, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - open wall. */
const Miniset VARCH18 {
	{ 2, 3 },
	{
	    { 2, 7 },
	    { 3, 4 },
	    { 0, 8 },
	},
	{
	    { 141, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - open wall. */
const Miniset VARCH19 {
	{ 2, 3 },
	{
	    { 2, 7 },
	    { 3, 4 },
	    { 0, 6 },
	},
	{
	    { 141, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - open wall. */
const Miniset VARCH20 {
	{ 2, 3 },
	{
	    { 2, 7 },
	    { 3, 4 },
	    { 0, 9 },
	},
	{
	    { 141, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - open wall. */
const Miniset VARCH21 {
	{ 2, 3 },
	{
	    { 2, 7 },
	    { 3, 4 },
	    { 0, 14 },
	},
	{
	    { 141, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - open wall. */
const Miniset VARCH22 {
	{ 2, 3 },
	{
	    { 2, 7 },
	    { 3, 4 },
	    { 0, 13 },
	},
	{
	    { 141, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - open wall. */
const Miniset VARCH23 {
	{ 2, 3 },
	{
	    { 2, 7 },
	    { 3, 4 },
	    { 0, 16 },
	},
	{
	    { 141, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - open wall. */
const Miniset VARCH24 {
	{ 2, 3 },
	{
	    { 2, 7 },
	    { 3, 4 },
	    { 0, 15 },
	},
	{
	    { 141, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH25 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 4 },
	    { 3, 1 },
	    { 0, 7 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH26 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 4 },
	    { 3, 1 },
	    { 0, 8 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH27 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 4 },
	    { 3, 1 },
	    { 0, 6 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH28 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 4 },
	    { 3, 1 },
	    { 0, 9 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH29 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 4 },
	    { 3, 1 },
	    { 0, 14 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH30 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 4 },
	    { 3, 1 },
	    { 0, 13 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH31 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 4 },
	    { 3, 1 },
	    { 0, 16 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical. */
const Miniset VARCH32 {
	{ 2, 4 },
	{
	    { 3, 0 },
	    { 3, 4 },
	    { 3, 1 },
	    { 0, 15 },
	},
	{
	    { 48, 0 },
	    { 51, 39 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - room west entrance. */
const Miniset VARCH33 {
	{ 2, 4 },
	{
	    { 2, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 7 },
	},
	{
	    { 142, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - room west entrance. */
const Miniset VARCH34 {
	{ 2, 4 },
	{
	    { 2, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 8 },
	},
	{
	    { 142, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - room west entrance. */
const Miniset VARCH35 {
	{ 2, 4 },
	{
	    { 2, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 6 },
	},
	{
	    { 142, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - room west entrance. */
const Miniset VARCH36 {
	{ 2, 4 },
	{
	    { 2, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 9 },
	},
	{
	    { 142, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - room west entrance. */
const Miniset VARCH37 {
	{ 2, 4 },
	{
	    { 2, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 14 },
	},
	{
	    { 142, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - room west entrance. */
const Miniset VARCH38 {
	{ 2, 4 },
	{
	    { 2, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 13 },
	},
	{
	    { 142, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - room west entrance. */
const Miniset VARCH39 {
	{ 2, 4 },
	{
	    { 2, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 16 },
	},
	{
	    { 142, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch vertical - room west entrance. */
const Miniset VARCH40 {
	{ 2, 4 },
	{
	    { 2, 0 },
	    { 3, 8 },
	    { 3, 4 },
	    { 0, 15 },
	},
	{
	    { 142, 0 },
	    { 51, 42 },
	    { 47, 44 },
	    { 0, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH1 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 2, 5, 9 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH2 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 2, 5, 6 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH3 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 2, 5, 8 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH4 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 2, 5, 7 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH5 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 2, 5, 15 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH6 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 2, 5, 16 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH7 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 2, 5, 13 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH8 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 2, 5, 14 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal - north corner. */
const Miniset HARCH9 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 8, 5, 9 },
	},
	{
	    { 49, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - north corner. */
const Miniset HARCH10 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 8, 5, 6 },
	},
	{
	    { 49, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - north corner. */
const Miniset HARCH11 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 8, 5, 8 },
	},
	{
	    { 49, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - north corner. */
const Miniset HARCH12 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 8, 5, 7 },
	},
	{
	    { 49, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - north corner. */
const Miniset HARCH13 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 8, 5, 15 },
	},
	{
	    { 49, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - north corner. */
const Miniset HARCH14 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 8, 5, 16 },
	},
	{
	    { 49, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - north corner. */
const Miniset HARCH15 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 8, 5, 13 },
	},
	{
	    { 49, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - north corner. */
const Miniset HARCH16 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 8, 5, 14 },
	},
	{
	    { 49, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - wall. */
const Miniset HARCH17 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 8, 5, 9 },
	},
	{
	    { 140, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - wall. */
const Miniset HARCH18 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 8, 5, 6 },
	},
	{
	    { 140, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - wall. */
const Miniset HARCH19 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 8, 5, 8 },
	},
	{
	    { 140, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - wall. */
const Miniset HARCH20 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 8, 5, 7 },
	},
	{
	    { 140, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - wall. */
const Miniset HARCH21 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 8, 5, 15 },
	},
	{
	    { 140, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - wall. */
const Miniset HARCH22 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 8, 5, 16 },
	},
	{
	    { 140, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - wall. */
const Miniset HARCH23 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 8, 5, 13 },
	},
	{
	    { 140, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal - wall. */
const Miniset HARCH24 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 8, 5, 14 },
	},
	{
	    { 140, 46, 0 },
	    { 43, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH25 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 5, 2, 9 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH26 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 5, 2, 6 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH27 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 5, 2, 8 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH28 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 5, 2, 7 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH29 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 5, 2, 15 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH30 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 5, 2, 16 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH31 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 5, 2, 13 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal. */
const Miniset HARCH32 {
	{ 3, 2 },
	{
	    { 3, 3, 0 },
	    { 5, 2, 14 },
	},
	{
	    { 49, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal - west corner. */
const Miniset HARCH33 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 9, 5, 9 },
	},
	{
	    { 140, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal - west corner. */
const Miniset HARCH34 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 9, 5, 6 },
	},
	{
	    { 140, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal - west corner. */
const Miniset HARCH35 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 9, 5, 8 },
	},
	{
	    { 140, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal - west corner. */
const Miniset HARCH36 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 9, 5, 7 },
	},
	{
	    { 140, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal - west corner. */
const Miniset HARCH37 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 9, 5, 15 },
	},
	{
	    { 140, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal - west corner. */
const Miniset HARCH38 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 9, 5, 16 },
	},
	{
	    { 140, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal - west corner. */
const Miniset HARCH39 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 9, 5, 13 },
	},
	{
	    { 140, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Arch horizontal - west corner. */
const Miniset HARCH40 {
	{ 3, 2 },
	{
	    { 1, 3, 0 },
	    { 9, 5, 14 },
	},
	{
	    { 140, 46, 0 },
	    { 40, 45, 0 },
	}
};
/** Miniset: Stairs up. */
const Miniset USTAIRS {
	{ 4, 4 },
	{
	    { 3, 3, 3, 3 },
	    { 3, 3, 3, 3 },
	    { 3, 3, 3, 3 },
	    { 3, 3, 3, 3 },
	},
	{
	    { 0, 0, 0, 0 },
	    { 0, 72, 77, 0 },
	    { 0, 76, 0, 0 },
	    { 0, 0, 0, 0 },
	}
};
/** Miniset: Stairs down. */
const Miniset DSTAIRS {
	{ 4, 4 },
	{
	    { 3, 3, 3, 3 },
	    { 3, 3, 3, 3 },
	    { 3, 3, 3, 3 },
	    { 3, 3, 3, 3 },
	},
	{
	    { 0, 0, 0, 0 },
	    { 0, 48, 71, 0 },
	    { 0, 50, 78, 0 },
	    { 0, 0, 0, 0 },
	}
};
/** Miniset: Stairs to town. */
const Miniset WARPSTAIRS {
	{ 4, 4 },
	{
	    { 3, 3, 3, 3 },
	    { 3, 3, 3, 3 },
	    { 3, 3, 3, 3 },
	    { 3, 3, 3, 3 },
	},
	{
	    { 0, 0, 0, 0 },
	    { 0, 158, 160, 0 },
	    { 0, 159, 0, 0 },
	    { 0, 0, 0, 0 },
	}
};
/** Miniset: Crumbled south pillar. */
const Miniset CRUSHCOL {
	{ 3, 3 },
	{
	    { 3, 1, 3 },
	    { 2, 6, 3 },
	    { 3, 3, 3 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 83, 0 },
	    { 0, 0, 0 },
	}
};
/** Miniset: Vertical oil spill. */
const Miniset BIG1 {
	{ 2, 2 },
	{
	    { 3, 3 },
	    { 3, 3 },
	},
	{
	    { 113, 0 },
	    { 112, 0 },
	}
};
/** Miniset: Horizontal oil spill. */
const Miniset BIG2 {
	{ 2, 2 },
	{
	    { 3, 3 },
	    { 3, 3 },
	},
	{
	    { 114, 115 },
	    { 0, 0 },
	}
};
/** Miniset: Horizontal platform. */
const Miniset BIG3 {
	{ 1, 2 },
	{
	    { 1 },
	    { 1 },
	},
	{
	    { 117 },
	    { 116 },
	}
};
/** Miniset: Vertical platform. */
const Miniset BIG4 {
	{ 2, 1 },
	{
	    { 2, 2 },
	},
	{
	    { 118, 119 },
	}
};
/** Miniset: Large oil spill. */
const Miniset BIG5 {
	{ 2, 2 },
	{
	    { 3, 3 },
	    { 3, 3 },
	},
	{
	    { 120, 122 },
	    { 121, 123 },
	}
};
/** Miniset: Vertical wall with debris. */
const Miniset BIG6 {
	{ 1, 2 },
	{
	    { 1 },
	    { 1 },
	},
	{
	    { 125 },
	    { 124 },
	}
};
/** Miniset: Horizontal wall with debris. */
const Miniset BIG7 {
	{ 2, 1 },
	{
	    { 2, 2 },
	},
	{
	    { 126, 127 },
	}
};
/** Miniset: Rock pile. */
const Miniset BIG8 {
	{ 2, 2 },
	{
	    { 3, 3 },
	    { 3, 3 },
	},
	{
	    { 128, 130 },
	    { 129, 131 },
	}
};
/** Miniset: Vertical wall collapsed. */
const Miniset BIG9 {
	{ 2, 2 },
	{
	    { 1, 3 },
	    { 1, 3 },
	},
	{
	    { 133, 135 },
	    { 132, 134 },
	}
};
/** Miniset: Horizontal wall collapsed. */
const Miniset BIG10 {
	{ 2, 2 },
	{
	    { 2, 2 },
	    { 3, 3 },
	},
	{
	    { 136, 137 },
	    { 3, 3 },
	}
};
/** Miniset: Crumbled vertical wall 1. */
const Miniset RUINS1 {
	{ 1, 1 },
	{
	    { 1 },
	},
	{
	    { 80 },
	}
};
/** Miniset: Crumbled vertical wall 2. */
const Miniset RUINS2 {
	{ 1, 1 },
	{
	    { 1 },
	},
	{
	    { 81 },
	}
};
/** Miniset: Crumbled vertical wall 3. */
const Miniset RUINS3 {
	{ 1, 1 },
	{
	    { 1 },
	},
	{
	    { 82 },
	}
};
/** Miniset: Crumbled horizontal wall 1. */
const Miniset RUINS4 {
	{ 1, 1 },
	{
	    { 2 },
	},
	{
	    { 84 },
	}
};
/** Miniset: Crumbled horizontal wall 2. */
const Miniset RUINS5 {
	{ 1, 1 },
	{
	    { 2 },
	},
	{
	    { 85 },
	}
};
/** Miniset: Crumbled horizontal wall 3. */
const Miniset RUINS6 {
	{ 1, 1 },
	{
	    { 2 },
	},
	{
	    { 86 },
	}
};
/** Miniset: Crumbled north pillar. */
const Miniset RUINS7 {
	{ 1, 1 },
	{
	    { 8 },
	},
	{
	    { 87 },
	}
};
/** Miniset: Bloody gib 1. */
const Miniset PANCREAS1 {
	{ 5, 3 },
	{
	    { 3, 3, 3, 3, 3 },
	    { 3, 3, 3, 3, 3 },
	    { 3, 3, 3, 3, 3 },
	},
	{
	    { 0, 0, 0, 0, 0 },
	    { 0, 0, 108, 0, 0 },
	    { 0, 0, 0, 0, 0 },
	}
};
/** Miniset: Bloody gib 2. */
const Miniset PANCREAS2 {
	{ 5, 3 },
	{
	    { 3, 3, 3, 3, 3 },
	    { 3, 3, 3, 3, 3 },
	    { 3, 3, 3, 3, 3 },
	},
	{
	    { 0, 0, 0, 0, 0 },
	    { 0, 0, 110, 0, 0 },
	    { 0, 0, 0, 0, 0 },
	}
};
/** Miniset: Move vertical doors away from west pillar 1. */
const Miniset CTRDOOR1 {
	{ 3, 3 },
	{
	    { 3, 1, 3 },
	    { 0, 4, 0 },
	    { 0, 9, 0 },
	},
	{
	    { 0, 4, 0 },
	    { 0, 1, 0 },
	    { 0, 0, 0 },
	}
};
/** Miniset: Move vertical doors away from west pillar 2. */
const Miniset CTRDOOR2 {
	{ 3, 3 },
	{
	    { 3, 1, 3 },
	    { 0, 4, 0 },
	    { 0, 8, 0 },
	},
	{
	    { 0, 4, 0 },
	    { 0, 1, 0 },
	    { 0, 0, 0 },
	}
};
/** Miniset: Move vertical doors away from west pillar 3. */
const Miniset CTRDOOR3 {
	{ 3, 3 },
	{
	    { 3, 1, 3 },
	    { 0, 4, 0 },
	    { 0, 6, 0 },
	},
	{
	    { 0, 4, 0 },
	    { 0, 1, 0 },
	    { 0, 0, 0 },
	}
};
/** Miniset: Move vertical doors away from west pillar 4. */
const Miniset CTRDOOR4 {
	{ 3, 3 },
	{
	    { 3, 1, 3 },
	    { 0, 4, 0 },
	    { 0, 7, 0 },
	},
	{
	    { 0, 4, 0 },
	    { 0, 1, 0 },
	    { 0, 0, 0 },
	}
};
/** Miniset: Move vertical doors away from west pillar 5. */
const Miniset CTRDOOR5 {
	{ 3, 3 },
	{
	    { 3, 1, 3 },
	    { 0, 4, 0 },
	    { 0, 15, 0 },
	},
	{
	    { 0, 4, 0 },
	    { 0, 1, 0 },
	    { 0, 0, 0 },
	}
};
/** Miniset: Move vertical doors away from west pillar 6. */
const Miniset CTRDOOR6 {
	{ 3, 3 },
	{
	    { 3, 1, 3 },
	    { 0, 4, 0 },
	    { 0, 13, 0 },
	},
	{
	    { 0, 4, 0 },
	    { 0, 1, 0 },
	    { 0, 0, 0 },
	}
};
/** Miniset: Move vertical doors away from west pillar 7. */
const Miniset CTRDOOR7 {
	{ 3, 3 },
	{
	    { 3, 1, 3 },
	    { 0, 4, 0 },
	    { 0, 16, 0 },
	},
	{
	    { 0, 4, 0 },
	    { 0, 1, 0 },
	    { 0, 0, 0 },
	}
};
/** Miniset: Move vertical doors away from west pillar 8. */
const Miniset CTRDOOR8 {
	{ 3, 3 },
	{
	    { 3, 1, 3 },
	    { 0, 4, 0 },
	    { 0, 14, 0 },
	},
	{
	    { 0, 4, 0 },
	    { 0, 1, 0 },
	    { 0, 0, 0 },
	}
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

void ApplyShadowsPatterns()
{
	uint8_t sd[2][2];

	for (int y = 1; y < DMAXY; y++) {
		for (int x = 1; x < DMAXX; x++) {
			sd[0][0] = BSTYPESL2[dungeon[x][y]];
			sd[1][0] = BSTYPESL2[dungeon[x - 1][y]];
			sd[0][1] = BSTYPESL2[dungeon[x][y - 1]];
			sd[1][1] = BSTYPESL2[dungeon[x - 1][y - 1]];

			for (const auto &shadow : SPATSL2) {
				if (shadow.strig != sd[0][0])
					continue;
				if (shadow.s1 != 0 && shadow.s1 != sd[1][1])
					continue;
				if (shadow.s2 != 0 && shadow.s2 != sd[0][1])
					continue;
				if (shadow.s3 != 0 && shadow.s3 != sd[1][0])
					continue;

				if (shadow.nv1 != 0) {
					dungeon[x - 1][y - 1] = shadow.nv1;
				}
				if (shadow.nv2 != 0) {
					dungeon[x][y - 1] = shadow.nv2;
				}
				if (shadow.nv3 != 0) {
					dungeon[x - 1][y] = shadow.nv3;
				}
			}
		}
	}
}

bool PlaceMiniSet(const Miniset &miniset, int tmin, int tmax, int cx, int cy, bool setview)
{
	int sw = miniset.size.width;
	int sh = miniset.size.height;

	int numt = 1;
	if (tmax - tmin != 0) {
		numt = GenerateRnd(tmax - tmin) + tmin;
	}

	int sx = 0;
	int sy = 0;
	for (int i = 0; i < numt; i++) {
		sx = GenerateRnd(DMAXX - sw);
		sy = GenerateRnd(DMAXY - sh);
		bool abort = false;
		int bailcnt;

		for (bailcnt = 0; !abort && bailcnt < 200; bailcnt++) {
			abort = true;
			if (sx >= nSx1 && sx <= nSx2 && sy >= nSy1 && sy <= nSy2) {
				abort = false;
			}
			if (cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx = GenerateRnd(DMAXX - sw);
				sy = GenerateRnd(DMAXY - sh);
				abort = false;
			}
			if (cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sx = GenerateRnd(DMAXX - sw);
				sy = GenerateRnd(DMAXY - sh);
				abort = false;
			}

			if (abort)
				abort = miniset.matches({ sx, sy });

			if (!abort) {
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

		miniset.place({ sx, sy });
	}

	if (setview) {
		ViewPosition = Point { 21, 22 } + Displacement { sx, sy } * 2;
	}

	return true;
}

void PlaceMiniSetRandom(const Miniset &miniset, int rndper)
{
	int sw = miniset.size.width;
	int sh = miniset.size.height;

	for (int sy = 0; sy < DMAXY - sh; sy++) {
		for (int sx = 0; sx < DMAXX - sw; sx++) {
			if (sx >= nSx1 && sx <= nSx2 && sy >= nSy1 && sy <= nSy2)
				continue;
			if (!miniset.matches({ sx, sy }))
				continue;
			bool found = true;
			for (int yy = std::max(sy - sh, 0); yy < std::min(sy + 2 * sh, DMAXY) && found; yy++) {
				for (int xx = std::max(sx - sw, 0); xx < std::min(sx + 2 * sw, DMAXX); xx++) {
					// BUGFIX: yy and xx can go out of bounds (fixed)
					if (dungeon[xx][yy] == miniset.replace[0][0]) {
						found = false;
						break;
					}
				}
			}
			if (found && GenerateRnd(100) < rndper)
				miniset.place({ sx, sy });
		}
	}
}

void LoadQuestSetPieces()
{
	setloadflag = false;

	if (Quests[Q_BLIND].IsAvailable()) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blind1.DUN");
		pSetPiece[13] = SDL_SwapLE16(154);  // Close outer wall
		pSetPiece[100] = SDL_SwapLE16(154); // Close outer wall
		setloadflag = true;
	} else if (Quests[Q_BLOOD].IsAvailable()) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blood1.DUN");
		setloadflag = true;
	} else if (Quests[Q_SCHAMB].IsAvailable()) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L2Data\\Bonestr2.DUN");
		setloadflag = true;
	}
}

void FreeQuestSetPieces()
{
	pSetPiece = nullptr;
}

void InitDungeonPieces()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			int8_t pc;
			if (IsAnyOf(dPiece[i][j], 541, 178, 551)) {
				pc = 5;
			} else if (IsAnyOf(dPiece[i][j], 542, 553)) {
				pc = 6;
			} else {
				continue;
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
}

void InitDungeonFlags()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			predungeon[i][j] = 32;
			dflags[i][j] = 0;
		}
	}
}

void MapRoom(int x1, int y1, int x2, int y2)
{
	for (int jj = y1; jj <= y2; jj++) {
		for (int ii = x1; ii <= x2; ii++) {
			predungeon[ii][jj] = 46;
		}
	}
	for (int jj = y1; jj <= y2; jj++) {
		predungeon[x1][jj] = 35;
		predungeon[x2][jj] = 35;
	}
	for (int ii = x1; ii <= x2; ii++) {
		predungeon[ii][y1] = 35;
		predungeon[ii][y2] = 35;
	}
}

void DefineRoom(int nX1, int nY1, int nX2, int nY2, bool forceHW)
{
	predungeon[nX1][nY1] = 67;
	predungeon[nX1][nY2] = 69;
	predungeon[nX2][nY1] = 66;
	predungeon[nX2][nY2] = 65;

	nRoomCnt++;
	RoomList[nRoomCnt].nRoomx1 = nX1;
	RoomList[nRoomCnt].nRoomx2 = nX2;
	RoomList[nRoomCnt].nRoomy1 = nY1;
	RoomList[nRoomCnt].nRoomy2 = nY2;

	if (forceHW) {
		for (int i = nX1; i < nX2; i++) {
			/// BUGFIX: Should loop j between nY1 and nY2 instead of always using nY1.
			while (i < nY2) {
				dflags[i][nY1] |= DLRG_PROTECTED;
				i++;
			}
		}
	}
	for (int i = nX1 + 1; i <= nX2 - 1; i++) {
		predungeon[i][nY1] = 35;
		predungeon[i][nY2] = 35;
	}
	nY2--;
	for (int j = nY1 + 1; j <= nY2; j++) {
		predungeon[nX1][j] = 35;
		predungeon[nX2][j] = 35;
		for (int i = nX1 + 1; i < nX2; i++) {
			predungeon[i][j] = 46;
		}
	}
}

void CreateDoorType(int nX, int nY)
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
	if (IsAnyOf(predungeon[nX][nY], 65, 66, 67, 69)) {
		return;
	}

	predungeon[nX][nY] = 68;
}

void PlaceHallExt(int nX, int nY)
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
 * @param forceHW If set, nH and nW are used for room size instead of random values.
 * @param nH Height of the room, if forceHW is set.
 * @param nW Width of the room, if forceHW is set.
 */
void CreateRoom(int nX1, int nY1, int nX2, int nY2, int nRDest, int nHDir, bool forceHW, int nH, int nW)
{
	if (nRoomCnt >= 80) {
		return;
	}

	int nAw = nX2 - nX1;
	int nAh = nY2 - nY1;
	if (nAw < Area_Min || nAh < Area_Min) {
		return;
	}

	int nRw = nAw;
	if (nAw > Room_Max) {
		nRw = GenerateRnd(Room_Max - Room_Min) + Room_Min;
	} else if (nAw > Room_Min) {
		nRw = GenerateRnd(nAw - Room_Min) + Room_Min;
	}
	int nRh = nAh;
	if (nAh > Room_Max) {
		nRh = GenerateRnd(Room_Max - Room_Min) + Room_Min;
	} else if (nAh > Room_Min) {
		nRh = GenerateRnd(nAh - Room_Min) + Room_Min;
	}

	if (forceHW) {
		nRw = nW;
		nRh = nH;
	}

	int nRx1 = GenerateRnd(nX2 - nX1) + nX1;
	int nRy1 = GenerateRnd(nY2 - nY1) + nY1;
	int nRx2 = nRw + nRx1;
	int nRy2 = nRh + nRy1;
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
	DefineRoom(nRx1, nRy1, nRx2, nRy2, forceHW);

	if (forceHW) {
		nSx1 = nRx1 + 2;
		nSy1 = nRy1 + 2;
		nSx2 = nRx2;
		nSy2 = nRy2;
	}

	int nRid = nRoomCnt;

	if (nRDest != 0) {
		int nHx1 = 0;
		int nHy1 = 0;
		int nHx2 = 0;
		int nHy2 = 0;
		if (nHDir == 1) {
			nHx1 = GenerateRnd(nRx2 - nRx1 - 2) + nRx1 + 1;
			nHy1 = nRy1;
			int nHw = RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2;
			nHx2 = GenerateRnd(nHw) + RoomList[nRDest].nRoomx1 + 1;
			nHy2 = RoomList[nRDest].nRoomy2;
		}
		if (nHDir == 3) {
			nHx1 = GenerateRnd(nRx2 - nRx1 - 2) + nRx1 + 1;
			nHy1 = nRy2;
			int nHw = RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2;
			nHx2 = GenerateRnd(nHw) + RoomList[nRDest].nRoomx1 + 1;
			nHy2 = RoomList[nRDest].nRoomy1;
		}
		if (nHDir == 2) {
			nHx1 = nRx2;
			nHy1 = GenerateRnd(nRy2 - nRy1 - 2) + nRy1 + 1;
			nHx2 = RoomList[nRDest].nRoomx1;
			int nHh = RoomList[nRDest].nRoomy2 - RoomList[nRDest].nRoomy1 - 2;
			nHy2 = GenerateRnd(nHh) + RoomList[nRDest].nRoomy1 + 1;
		}
		if (nHDir == 4) {
			nHx1 = nRx1;
			nHy1 = GenerateRnd(nRy2 - nRy1 - 2) + nRy1 + 1;
			nHx2 = RoomList[nRDest].nRoomx2;
			int nHh = RoomList[nRDest].nRoomy2 - RoomList[nRDest].nRoomy1 - 2;
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

void ConnectHall(const HALLNODE &node)
{
	int nRp;

	int nX1 = node.nHallx1;
	int nY1 = node.nHally1;
	int nX2 = node.nHallx2;
	int nY2 = node.nHally2;
	int nHd = node.nHalldir;

	bool fDoneflag = false;
	int fMinusFlag = GenerateRnd(100);
	int fPlusFlag = GenerateRnd(100);
	int nOrigX1 = nX1;
	int nOrigY1 = nY1;
	CreateDoorType(nX1, nY1);
	CreateDoorType(nX2, nY2);
	int nCurrd = nHd;
	nX2 -= DirXadd[nCurrd];
	nY2 -= DirYadd[nCurrd];
	predungeon[nX2][nY2] = 44;
	bool fInroom = false;

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
		nX1 += DirXadd[nCurrd];
		nY1 += DirYadd[nCurrd];
		if (predungeon[nX1][nY1] == 32) {
			if (fInroom) {
				CreateDoorType(nX1 - DirXadd[nCurrd], nY1 - DirYadd[nCurrd]);
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
		int nDx = abs(nX2 - nX1);
		int nDy = abs(nY2 - nY1);
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

void DoPatternCheck(int i, int j)
{
	for (int k = 0; Patterns[k][4] != 255; k++) {
		int x = i - 1;
		int y = j - 1;
		int nOk = 254;
		for (int l = 0; l < 9 && nOk == 254; l++) {
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

void FixTilesPatterns()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
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

void Substitution()
{
	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			if ((x < nSx1 || x > nSx2) && (y < nSy1 || y > nSy2) && GenerateRnd(4) == 0) {
				uint8_t c = BTYPESL2[dungeon[x][y]];
				if (c != 0) {
					int rv = GenerateRnd(16);
					int i = -1;
					while (rv >= 0) {
						i++;
						if (i == sizeof(BTYPESL2)) {
							i = 0;
						}
						if (c == BTYPESL2[i]) {
							rv--;
						}
					}

					int j;
					for (j = y - 2; j < y + 2; j++) {
						for (int k = x - 2; k < x + 2; k++) {
							if (dungeon[k][j] == i) {
								j = y + 3;
								k = x + 2;
							}
						}
					}
					if (j < y + 3) {
						dungeon[x][y] = i;
					}
				}
			}
		}
	}
}

void SetRoom(int rx1, int ry1)
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
				dungeon[rx1 + i][ry1 + j] = tileId;
				dflags[rx1 + i][ry1 + j] |= DLRG_PROTECTED;
			} else {
				dungeon[rx1 + i][ry1 + j] = 3;
			}
		}
	}
}

int CountEmptyTiles()
{
	int t = 0;
	for (int jj = 0; jj < DMAXY; jj++) {
		for (int ii = 0; ii < DMAXX; ii++) { // NOLINT(modernize-loop-convert)
			if (predungeon[ii][jj] == 32) {
				t++;
			}
		}
	}

	return t;
}

void KnockWalls(int x1, int y1, int x2, int y2)
{
	for (int ii = x1 + 1; ii < x2; ii++) {
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
	for (int jj = y1 + 1; jj < y2; jj++) {
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

void FillVoid(bool xf1, bool yf1, bool xf2, bool yf2, int xx, int yy)
{
	int x1 = xx;
	if (xf1) {
		x1--;
	}
	int x2 = xx;
	if (xf2) {
		x2++;
	}
	int y1 = yy;
	if (yf1) {
		y1--;
	}
	int y2 = yy;
	if (yf2) {
		y2++;
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
				for (int jj = y1; jj <= y2; jj++) {
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
				MapRoom(x1, y1, x2, y2);
				KnockWalls(x1, y1, x2, y2);
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
				for (int jj = y1; jj <= y2; jj++) {
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
				MapRoom(x1, y1, x2, y2);
				KnockWalls(x1, y1, x2, y2);
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
				for (int ii = x1; ii <= x2; ii++) {
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
				MapRoom(x1, y1, x2, y2);
				KnockWalls(x1, y1, x2, y2);
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
				for (int ii = x1; ii <= x2; ii++) {
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
				MapRoom(x1, y1, x2, y2);
				KnockWalls(x1, y1, x2, y2);
			}
		}
	}
}

bool FillVoids()
{
	int to = 0;
	while (CountEmptyTiles() > 700 && to < 100) {
		int xx = GenerateRnd(38) + 1;
		int yy = GenerateRnd(38) + 1;
		if (predungeon[xx][yy] != 35) {
			continue;
		}
		bool xf1 = false;
		bool xf2 = false;
		bool yf1 = false;
		bool yf2 = false;
		if (predungeon[xx - 1][yy] == 32 && predungeon[xx + 1][yy] == 46) {
			if (predungeon[xx + 1][yy - 1] == 46
			    && predungeon[xx + 1][yy + 1] == 46
			    && predungeon[xx - 1][yy - 1] == 32
			    && predungeon[xx - 1][yy + 1] == 32) {
				xf1 = true;
				yf1 = true;
				yf2 = true;
			}
		} else if (predungeon[xx + 1][yy] == 32 && predungeon[xx - 1][yy] == 46) {
			if (predungeon[xx - 1][yy - 1] == 46
			    && predungeon[xx - 1][yy + 1] == 46
			    && predungeon[xx + 1][yy - 1] == 32
			    && predungeon[xx + 1][yy + 1] == 32) {
				xf2 = true;
				yf1 = true;
				yf2 = true;
			}
		} else if (predungeon[xx][yy - 1] == 32 && predungeon[xx][yy + 1] == 46) {
			if (predungeon[xx - 1][yy + 1] == 46
			    && predungeon[xx + 1][yy + 1] == 46
			    && predungeon[xx - 1][yy - 1] == 32
			    && predungeon[xx + 1][yy - 1] == 32) {
				yf1 = true;
				xf1 = true;
				xf2 = true;
			}
		} else if (predungeon[xx][yy + 1] == 32 && predungeon[xx][yy - 1] == 46) {
			if (predungeon[xx - 1][yy - 1] == 46
			    && predungeon[xx + 1][yy - 1] == 46
			    && predungeon[xx - 1][yy + 1] == 32
			    && predungeon[xx + 1][yy + 1] == 32) {
				yf2 = true;
				xf1 = true;
				xf2 = true;
			}
		}
		if (xf1 || yf1 || xf2 || yf2) {
			FillVoid(xf1, yf1, xf2, yf2, xx, yy);
		}
		to++;
	}

	return CountEmptyTiles() <= 700;
}

bool CreateDungeon()
{
	int forceW = 0;
	int forceH = 0;
	bool forceHW = false;

	switch (currlevel) {
	case 5:
		if (Quests[Q_BLOOD]._qactive != QUEST_NOTAVAIL) {
			forceHW = true;
			forceH = 20;
			forceW = 14;
		}
		break;
	case 6:
		if (Quests[Q_SCHAMB]._qactive != QUEST_NOTAVAIL) {
			forceHW = true;
			forceW = 10;
			forceH = 10;
		}
		break;
	case 7:
		if (Quests[Q_BLIND]._qactive != QUEST_NOTAVAIL) {
			forceHW = true;
			forceW = 15;
			forceH = 15;
		}
		break;
	case 8:
		break;
	}

	CreateRoom(2, 2, DMAXX - 1, DMAXY - 1, 0, 0, forceHW, forceH, forceW);

	while (!HallList.empty()) {
		ConnectHall(HallList.front());
		HallList.pop_front();
	}

	for (int j = 0; j < DMAXY; j++) {     /// BUGFIX: change '<=' to '<' (fixed)
		for (int i = 0; i < DMAXX; i++) { /// BUGFIX: change '<=' to '<' (fixed)
			if (IsAnyOf(predungeon[i][j], 65, 66, 67, 69)) {
				predungeon[i][j] = 35;
			}
			if (predungeon[i][j] == 44) {
				predungeon[i][j] = 46;
				for (int a = -1; a <= 1; a++) {
					for (int b = -1; b <= 1; b++) {
						if (a == 0 && b == 0)
							continue;
						if (i + a < 0 || j + b < 0)
							continue;
						if (i + a >= DMAXX || j + b >= DMAXY)
							continue;
						if (predungeon[i + a][j + b] == 32) {
							predungeon[i + a][j + b] = 35;
						}
					}
				}
			}
		}
	}

	if (!FillVoids()) {
		return false;
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			DoPatternCheck(i, j);
		}
	}

	return true;
}

void FixTransparency()
{
	int yy = 16;
	for (int j = 0; j < DMAXY; j++) {
		int xx = 16;
		for (int i = 0; i < DMAXX; i++) {
			// BUGFIX: Should check for `j > 0` first.
			if (dungeon[i][j] == 14 && dungeon[i][j - 1] == 10) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			// BUGFIX: Should check for `i + 1 < DMAXY` first.
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

void FixDirtTiles()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
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

void FixLockout()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 4 && dungeon[i - 1][j] != 3) {
				dungeon[i][j] = 1;
			}
			if (dungeon[i][j] == 5 && dungeon[i][j - 1] != 3) {
				dungeon[i][j] = 2;
			}
		}
	}
	for (int j = 1; j < DMAXY - 1; j++) {
		for (int i = 1; i < DMAXX - 1; i++) {
			if ((dflags[i][j] & DLRG_PROTECTED) != 0) {
				continue;
			}
			if ((dungeon[i][j] == 2 || dungeon[i][j] == 5) && dungeon[i][j - 1] == 3 && dungeon[i][j + 1] == 3) {
				bool doorok = false;
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
	for (int j = 1; j < DMAXX - 1; j++) { /* check: might be flipped */
		for (int i = 1; i < DMAXY - 1; i++) {
			if ((dflags[j][i] & DLRG_PROTECTED) != 0) {
				continue;
			}
			if ((dungeon[j][i] == 1 || dungeon[j][i] == 4) && dungeon[j - 1][i] == 3 && dungeon[j + 1][i] == 3) {
				bool doorok = false;
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

void FixDoors()
{
	for (int j = 1; j < DMAXY; j++) {
		for (int i = 1; i < DMAXX; i++) {
			if (dungeon[i][j] == 4 && dungeon[i][j - 1] == 3) {
				dungeon[i][j] = 7;
			}
			if (dungeon[i][j] == 5 && dungeon[i - 1][j] == 3) {
				dungeon[i][j] = 9;
			}
		}
	}
}

void GenerateLevel(lvl_entry entry)
{
	bool doneflag = false;
	while (!doneflag) {
		nRoomCnt = 0;
		InitDungeonFlags();
		DRLG_InitTrans();
		if (!CreateDungeon()) {
			continue;
		}
		FixTilesPatterns();
		if (setloadflag) {
			SetRoom(nSx1, nSy1);
		}
		FloodTransparencyValues(3);
		FixTransparency();
		if (entry == ENTRY_MAIN) {
			doneflag = PlaceMiniSet(USTAIRS, 1, 1, -1, -1, true);
			if (doneflag) {
				doneflag = PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, false);
				if (doneflag && currlevel == 5) {
					doneflag = PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, false);
				}
			}
			ViewPosition.y -= 2;
		} else if (entry == ENTRY_PREV) {
			doneflag = PlaceMiniSet(USTAIRS, 1, 1, -1, -1, false);
			if (doneflag) {
				doneflag = PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, true);
				if (doneflag && currlevel == 5) {
					doneflag = PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, false);
				}
			}
			ViewPosition.x--;
		} else {
			doneflag = PlaceMiniSet(USTAIRS, 1, 1, -1, -1, false);
			if (doneflag) {
				doneflag = PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, false);
				if (doneflag && currlevel == 5) {
					doneflag = PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, true);
				}
			}
			ViewPosition.y -= 2;
		}
	}

	FixLockout();
	FixDoors();
	FixDirtTiles();

	DRLG_PlaceThemeRooms(6, 10, 3, 0, false);
	PlaceMiniSetRandom(CTRDOOR1, 100);
	PlaceMiniSetRandom(CTRDOOR2, 100);
	PlaceMiniSetRandom(CTRDOOR3, 100);
	PlaceMiniSetRandom(CTRDOOR4, 100);
	PlaceMiniSetRandom(CTRDOOR5, 100);
	PlaceMiniSetRandom(CTRDOOR6, 100);
	PlaceMiniSetRandom(CTRDOOR7, 100);
	PlaceMiniSetRandom(CTRDOOR8, 100);
	PlaceMiniSetRandom(VARCH33, 100);
	PlaceMiniSetRandom(VARCH34, 100);
	PlaceMiniSetRandom(VARCH35, 100);
	PlaceMiniSetRandom(VARCH36, 100);
	PlaceMiniSetRandom(VARCH37, 100);
	PlaceMiniSetRandom(VARCH38, 100);
	PlaceMiniSetRandom(VARCH39, 100);
	PlaceMiniSetRandom(VARCH40, 100);
	PlaceMiniSetRandom(VARCH1, 100);
	PlaceMiniSetRandom(VARCH2, 100);
	PlaceMiniSetRandom(VARCH3, 100);
	PlaceMiniSetRandom(VARCH4, 100);
	PlaceMiniSetRandom(VARCH5, 100);
	PlaceMiniSetRandom(VARCH6, 100);
	PlaceMiniSetRandom(VARCH7, 100);
	PlaceMiniSetRandom(VARCH8, 100);
	PlaceMiniSetRandom(VARCH9, 100);
	PlaceMiniSetRandom(VARCH10, 100);
	PlaceMiniSetRandom(VARCH11, 100);
	PlaceMiniSetRandom(VARCH12, 100);
	PlaceMiniSetRandom(VARCH13, 100);
	PlaceMiniSetRandom(VARCH14, 100);
	PlaceMiniSetRandom(VARCH15, 100);
	PlaceMiniSetRandom(VARCH16, 100);
	PlaceMiniSetRandom(VARCH17, 100);
	PlaceMiniSetRandom(VARCH18, 100);
	PlaceMiniSetRandom(VARCH19, 100);
	PlaceMiniSetRandom(VARCH20, 100);
	PlaceMiniSetRandom(VARCH21, 100);
	PlaceMiniSetRandom(VARCH22, 100);
	PlaceMiniSetRandom(VARCH23, 100);
	PlaceMiniSetRandom(VARCH24, 100);
	PlaceMiniSetRandom(VARCH25, 100);
	PlaceMiniSetRandom(VARCH26, 100);
	PlaceMiniSetRandom(VARCH27, 100);
	PlaceMiniSetRandom(VARCH28, 100);
	PlaceMiniSetRandom(VARCH29, 100);
	PlaceMiniSetRandom(VARCH30, 100);
	PlaceMiniSetRandom(VARCH31, 100);
	PlaceMiniSetRandom(VARCH32, 100);
	PlaceMiniSetRandom(HARCH1, 100);
	PlaceMiniSetRandom(HARCH2, 100);
	PlaceMiniSetRandom(HARCH3, 100);
	PlaceMiniSetRandom(HARCH4, 100);
	PlaceMiniSetRandom(HARCH5, 100);
	PlaceMiniSetRandom(HARCH6, 100);
	PlaceMiniSetRandom(HARCH7, 100);
	PlaceMiniSetRandom(HARCH8, 100);
	PlaceMiniSetRandom(HARCH9, 100);
	PlaceMiniSetRandom(HARCH10, 100);
	PlaceMiniSetRandom(HARCH11, 100);
	PlaceMiniSetRandom(HARCH12, 100);
	PlaceMiniSetRandom(HARCH13, 100);
	PlaceMiniSetRandom(HARCH14, 100);
	PlaceMiniSetRandom(HARCH15, 100);
	PlaceMiniSetRandom(HARCH16, 100);
	PlaceMiniSetRandom(HARCH17, 100);
	PlaceMiniSetRandom(HARCH18, 100);
	PlaceMiniSetRandom(HARCH19, 100);
	PlaceMiniSetRandom(HARCH20, 100);
	PlaceMiniSetRandom(HARCH21, 100);
	PlaceMiniSetRandom(HARCH22, 100);
	PlaceMiniSetRandom(HARCH23, 100);
	PlaceMiniSetRandom(HARCH24, 100);
	PlaceMiniSetRandom(HARCH25, 100);
	PlaceMiniSetRandom(HARCH26, 100);
	PlaceMiniSetRandom(HARCH27, 100);
	PlaceMiniSetRandom(HARCH28, 100);
	PlaceMiniSetRandom(HARCH29, 100);
	PlaceMiniSetRandom(HARCH30, 100);
	PlaceMiniSetRandom(HARCH31, 100);
	PlaceMiniSetRandom(HARCH32, 100);
	PlaceMiniSetRandom(HARCH33, 100);
	PlaceMiniSetRandom(HARCH34, 100);
	PlaceMiniSetRandom(HARCH35, 100);
	PlaceMiniSetRandom(HARCH36, 100);
	PlaceMiniSetRandom(HARCH37, 100);
	PlaceMiniSetRandom(HARCH38, 100);
	PlaceMiniSetRandom(HARCH39, 100);
	PlaceMiniSetRandom(HARCH40, 100);
	PlaceMiniSetRandom(CRUSHCOL, 99);
	PlaceMiniSetRandom(RUINS1, 10);
	PlaceMiniSetRandom(RUINS2, 10);
	PlaceMiniSetRandom(RUINS3, 10);
	PlaceMiniSetRandom(RUINS4, 10);
	PlaceMiniSetRandom(RUINS5, 10);
	PlaceMiniSetRandom(RUINS6, 10);
	PlaceMiniSetRandom(RUINS7, 50);
	PlaceMiniSetRandom(PANCREAS1, 1);
	PlaceMiniSetRandom(PANCREAS2, 1);
	PlaceMiniSetRandom(BIG1, 3);
	PlaceMiniSetRandom(BIG2, 3);
	PlaceMiniSetRandom(BIG3, 3);
	PlaceMiniSetRandom(BIG4, 3);
	PlaceMiniSetRandom(BIG5, 3);
	PlaceMiniSetRandom(BIG6, 20);
	PlaceMiniSetRandom(BIG7, 20);
	PlaceMiniSetRandom(BIG8, 3);
	PlaceMiniSetRandom(BIG9, 20);
	PlaceMiniSetRandom(BIG10, 20);
	Substitution();
	ApplyShadowsPatterns();

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}

	DRLG_Init_Globals();
	DRLG_CheckQuests(nSx1, nSy1);
}

void LoadDungeonData(const uint16_t *dunData)
{
	InitDungeonFlags();
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
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			if (dungeon[i][j] == 0) {
				dungeon[i][j] = 12;
			}
		}
	}
}

void Pass3()
{
	DRLG_LPass3(12 - 1);
}

} // namespace

void LoadL2Dungeon(const char *path, int vx, int vy)
{
	auto dunData = LoadFileInMem<uint16_t>(path);

	LoadDungeonData(dunData.get());

	Pass3();
	DRLG_Init_Globals();

	InitDungeonPieces();

	ViewPosition = { vx, vy };

	SetMapMonsters(dunData.get(), { 0, 0 });
	SetMapObjects(dunData.get(), 0, 0);
}

void LoadPreL2Dungeon(const char *path)
{
	{
		auto dunData = LoadFileInMem<uint16_t>(path);
		LoadDungeonData(dunData.get());
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
		if (currlevel == 7 && Quests[Q_BLIND]._qactive == QUEST_NOTAVAIL) {
			currlevel = 6;
			CreateL2Dungeon(glSeedTbl[6], ENTRY_LOAD);
			currlevel = 7;
		}
		if (currlevel == 8) {
			if (Quests[Q_BLIND]._qactive == QUEST_NOTAVAIL) {
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

	dminPosition = { 16, 16 };
	dmaxPosition = { 96, 96 };

	DRLG_InitTrans();
	DRLG_InitSetPC();
	LoadQuestSetPieces();
	GenerateLevel(entry);
	Pass3();
	FreeQuestSetPieces();
	InitDungeonPieces();
	DRLG_SetPC();
}

} // namespace devilution
