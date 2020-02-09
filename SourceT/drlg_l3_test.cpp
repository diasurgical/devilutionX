#include <gtest/gtest.h>
#include "devilution.h"

TEST(Drlg_l3, AddFenceDoors) {
	dvl::dungeon[5][5] = 7;
	dvl::dungeon[5 - 1][5] = 130;
	dvl::dungeon[5 + 1][5] = 152;
	dvl::AddFenceDoors();
	EXPECT_EQ(dvl::dungeon[5][5], 146);
}

TEST(Drlg_l3, AddFenceDoors_no) {
	dvl::dungeon[5][5] = 7;
	dvl::dungeon[5 - 1][5] = 130;
	dvl::dungeon[5 + 1][5] = 153;
	dvl::AddFenceDoors();
	EXPECT_EQ(dvl::dungeon[5][5], 7);
}
