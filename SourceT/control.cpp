#include <gtest/gtest.h>
#include "devilution.h"

TEST(Control, SetSpell) {
	dvl::pnumlines = 1;
	dvl::pinfoflag = true;
	dvl::pSpell = dvl::SPL_FIREBOLT;
	dvl::pSplType = dvl::RSPLTYPE_CHARGES;
	dvl::SetSpell();
	ASSERT_EQ(dvl::spselflag, false);
	ASSERT_EQ(dvl::plr[dvl::myplr]._pRSpell, dvl::SPL_FIREBOLT);
	ASSERT_EQ(dvl::plr[dvl::myplr]._pRSplType, dvl::RSPLTYPE_CHARGES);
	ASSERT_EQ(dvl::pnumlines, 0);
	ASSERT_EQ(dvl::pinfoflag, false);
	ASSERT_EQ(dvl::force_redraw, 255);
}

TEST(Control, ClearPanel) {
	dvl::pnumlines = 1;
	dvl::pinfoflag = true;
	dvl::ClearPanel();
	ASSERT_EQ(dvl::pnumlines, 0);
	ASSERT_EQ(dvl::pinfoflag, false);
}
