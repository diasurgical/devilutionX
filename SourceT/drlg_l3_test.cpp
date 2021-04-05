#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

TEST(Drlg_l3, AddFenceDoors_x)
{
	memset(dungeon, 0, sizeof(dungeon));
	dungeon[5][5] = 7;
	dungeon[5 - 1][5] = 130;
	dungeon[5 + 1][5] = 152;
	AddFenceDoors();
	EXPECT_EQ(dungeon[5][5], 146);
}

TEST(Drlg_l3, AddFenceDoors_y)
{
	memset(dungeon, 0, sizeof(dungeon));
	dungeon[5][5] = 7;
	dungeon[5][5 - 1] = 130;
	dungeon[5][5 + 1] = 152;
	AddFenceDoors();
	EXPECT_EQ(dungeon[5][5], 147);
}

TEST(Drlg_l3, AddFenceDoors_no)
{
	memset(dungeon, 0, sizeof(dungeon));
	dungeon[5][5] = 7;
	dungeon[5 - 1][5] = 130;
	dungeon[5 + 1][5] = 153;
	AddFenceDoors();
	EXPECT_EQ(dungeon[5][5], 7);
}
