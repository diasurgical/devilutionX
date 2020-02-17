#include <gtest/gtest.h>
#include "all.h"

TEST(Control, SetSpell) {
	dvl::pnumlines = 1;
	dvl::pinfoflag = true;
	dvl::pSpell = dvl::SPL_FIREBOLT;
	dvl::pSplType = dvl::RSPLTYPE_CHARGES;
	dvl::SetSpell();
	EXPECT_EQ(dvl::spselflag, false);
	EXPECT_EQ(dvl::plr[dvl::myplr]._pRSpell, dvl::SPL_FIREBOLT);
	EXPECT_EQ(dvl::plr[dvl::myplr]._pRSplType, dvl::RSPLTYPE_CHARGES);
	EXPECT_EQ(dvl::pnumlines, 0);
	EXPECT_EQ(dvl::pinfoflag, false);
	EXPECT_EQ(dvl::force_redraw, 255);
}

TEST(Control, ClearPanel) {
	dvl::pnumlines = 1;
	dvl::pinfoflag = true;
	dvl::ClearPanel();
	EXPECT_EQ(dvl::pnumlines, 0);
	EXPECT_EQ(dvl::pinfoflag, false);
}
