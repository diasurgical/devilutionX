#include <gtest/gtest.h>

#include "control.h"

using namespace devilution;

TEST(Control, ClearPanel)
{
	pnumlines = 1;
	ClearPanel();
	EXPECT_EQ(pnumlines, 0);
}
