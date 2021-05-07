#include <gtest/gtest.h>

#include "control.h"

using namespace devilution;

TEST(Control, SetSpell)
{
	pnumlines = 1;
	pinfoflag = true;
	pSpell = SPL_FIREBOLT;
	pSplType = RSPLTYPE_CHARGES;
	SetSpell();
	EXPECT_EQ(spselflag, false);
	EXPECT_EQ(plr[myplr]._pRSpell, SPL_FIREBOLT);
	EXPECT_EQ(plr[myplr]._pRSplType, RSPLTYPE_CHARGES);
	EXPECT_EQ(pnumlines, 0);
	EXPECT_EQ(pinfoflag, false);
	EXPECT_EQ(force_redraw, 255);
}

TEST(Control, ClearPanel)
{
	pnumlines = 1;
	pinfoflag = true;
	ClearPanel();
	EXPECT_EQ(pnumlines, 0);
	EXPECT_EQ(pinfoflag, false);
}
