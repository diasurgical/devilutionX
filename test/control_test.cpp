#include <gtest/gtest.h>

#include "control.h"

using namespace devilution;

TEST(Control, SetSpell)
{
	pnumlines = 1;
	pSpell = SPL_FIREBOLT;
	pSplType = RSPLTYPE_CHARGES;
	auto &myPlayer = Players[MyPlayerId];
	SetSpell();
	EXPECT_EQ(spselflag, false);
	EXPECT_EQ(myPlayer._pRSpell, SPL_FIREBOLT);
	EXPECT_EQ(myPlayer._pRSplType, RSPLTYPE_CHARGES);
	EXPECT_EQ(pnumlines, 0);
	EXPECT_EQ(force_redraw, 255);
}

TEST(Control, ClearPanel)
{
	pnumlines = 1;
	ClearPanel();
	EXPECT_EQ(pnumlines, 0);
}
