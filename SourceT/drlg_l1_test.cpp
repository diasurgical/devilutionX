#include <gtest/gtest.h>
#include "all.h"

TEST(Drlg_l1, DRLG_Init_Globals_4flag) {
	dvl::lightflag = false;
	dvl::light4flag = true;
	dvl::DRLG_Init_Globals();
	EXPECT_EQ(dvl::dLight[0][0], 3);
}

TEST(Drlg_l1, DRLG_Init_Globals_noflag) {
	dvl::lightflag = false;
	dvl::light4flag = false;
	dvl::DRLG_Init_Globals();
	EXPECT_EQ(dvl::dLight[0][0], 15);
}

TEST(Drlg_l1, DRLG_Init_Globals) {
	dvl::lightflag = true;
	dvl::DRLG_Init_Globals();
	EXPECT_EQ(dvl::dLight[0][0], 0);
}

