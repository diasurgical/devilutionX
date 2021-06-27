#include <gtest/gtest.h>

#include "diablo.h"
#include "drlg_l1.h"
#include "lighting.h"

using namespace devilution;

TEST(Drlg_l1, DRLG_Init_Globals_noflag)
{
	lightflag = false;
	DRLG_Init_Globals();
	EXPECT_EQ(dLight[0][0], 15);
}

TEST(Drlg_l1, DRLG_Init_Globals)
{
	lightflag = true;
	DRLG_Init_Globals();
	EXPECT_EQ(dLight[0][0], 0);
}
