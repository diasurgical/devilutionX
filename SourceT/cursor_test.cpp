#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

TEST(Cursor, SetCursor)
{
	int i = ICURS_SPIKED_CLUB + CURSOR_FIRSTITEM;
	SetCursor_(i);
	EXPECT_EQ(pcurs, i);
	EXPECT_EQ(cursW, 1 * 28);
	EXPECT_EQ(cursH, 3 * 28);
	EXPECT_EQ(icursW, 1 * 28);
	EXPECT_EQ(icursH, 3 * 28);
	EXPECT_EQ(icursW28, 1);
	EXPECT_EQ(icursH28, 3);
}
