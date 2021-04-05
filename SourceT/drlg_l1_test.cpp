#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

TEST(Drlg_l1, DRLG_Init_Globals_4flag)
{
	lightflag = false;
	light4flag = true;
	DRLG_Init_Globals();
	EXPECT_EQ(dLight[0][0], 3);
}

TEST(Drlg_l1, DRLG_Init_Globals_noflag)
{
	lightflag = false;
	light4flag = false;
	DRLG_Init_Globals();
	EXPECT_EQ(dLight[0][0], 15);
}

TEST(Drlg_l1, DRLG_Init_Globals)
{
	lightflag = true;
	DRLG_Init_Globals();
	EXPECT_EQ(dLight[0][0], 0);
}
