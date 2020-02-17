#include <gtest/gtest.h>
#include "all.h"

TEST(Drlg_l3, AddFenceDoors_x) {
	memset(dvl::dungeon, 0, sizeof(dvl::dungeon));
	dvl::dungeon[5][5] = 7;
	dvl::dungeon[5 - 1][5] = 130;
	dvl::dungeon[5 + 1][5] = 152;
	dvl::AddFenceDoors();
	EXPECT_EQ(dvl::dungeon[5][5], 146);
}

TEST(Drlg_l3, AddFenceDoors_y) {
	memset(dvl::dungeon, 0, sizeof(dvl::dungeon));
	dvl::dungeon[5][5] = 7;
	dvl::dungeon[5][5 - 1] = 130;
	dvl::dungeon[5][5 + 1] = 152;
	dvl::AddFenceDoors();
	EXPECT_EQ(dvl::dungeon[5][5], 147);
}

TEST(Drlg_l3, AddFenceDoors_no) {
	memset(dvl::dungeon, 0, sizeof(dvl::dungeon));
	dvl::dungeon[5][5] = 7;
	dvl::dungeon[5 - 1][5] = 130;
	dvl::dungeon[5 + 1][5] = 153;
	dvl::AddFenceDoors();
	EXPECT_EQ(dvl::dungeon[5][5], 7);
}
