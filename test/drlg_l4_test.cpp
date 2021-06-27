#include <gtest/gtest.h>

#include "drlg_l4.h"

using namespace devilution;

TEST(Drlg_l4, IsDURWall)
{
	EXPECT_EQ(IsDURWall(25), true);
	EXPECT_EQ(IsDURWall(28), true);
	EXPECT_EQ(IsDURWall(23), true);
	EXPECT_EQ(IsDURWall(20), false);
}

TEST(Drlg_l4, IsDLLWall)
{
	EXPECT_EQ(IsDLLWall(27), true);
	EXPECT_EQ(IsDLLWall(26), true);
	EXPECT_EQ(IsDLLWall(22), true);
	EXPECT_EQ(IsDLLWall(20), false);
}
