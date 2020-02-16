#include <gtest/gtest.h>
#include "all.h"

TEST(Drlg_l4, IsDURWall) {
	EXPECT_EQ(dvl::IsDURWall(25), true);
	EXPECT_EQ(dvl::IsDURWall(28), true);
	EXPECT_EQ(dvl::IsDURWall(23), true);
	EXPECT_EQ(dvl::IsDURWall(20), false);
}

TEST(Drlg_l4, IsDLLWall) {
	EXPECT_EQ(dvl::IsDLLWall(27), true);
	EXPECT_EQ(dvl::IsDLLWall(26), true);
	EXPECT_EQ(dvl::IsDLLWall(22), true);
	EXPECT_EQ(dvl::IsDLLWall(20), false);
}
