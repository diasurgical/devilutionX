#include <gtest/gtest.h>

#include "drlg_l2.h"

using namespace devilution;

TEST(Drlg_l2, InitDungeon)
{
	InitDungeon();
	EXPECT_EQ(predungeon[0][0], 32);
	EXPECT_EQ(dflags[0][0], 0);
}
