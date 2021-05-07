#include <gtest/gtest.h>

#include "dead.h"
#include "engine.h"
#include "gendung.h"

using namespace devilution;

TEST(Dead, AddDead)
{
	AddDead({21, 48}, 8, DIR_W);
	EXPECT_EQ(dDead[21][48], 8 + (DIR_W << 5));
}

TEST(Dead, AddDead_OOB)
{
	AddDead({21, 48}, MaxDead + 1, DIR_W);
	EXPECT_EQ(dDead[21][48], 0 + (DIR_W << 5));
}
