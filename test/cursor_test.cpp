#include <gtest/gtest.h>

#include "cursor.h"
#include "itemdat.h"

using namespace devilution;

TEST(Cursor, SetCursor)
{
	int i = ICURS_SPIKED_CLUB + CURSOR_FIRSTITEM;
	NewCursor(i);
	EXPECT_EQ(pcurs, i);
	EXPECT_EQ(cursW, 1 * 28);
	EXPECT_EQ(cursH, 3 * 28);
	EXPECT_EQ(icursW, 1 * 28);
	EXPECT_EQ(icursH, 3 * 28);
	EXPECT_EQ(icursW28, 1);
	EXPECT_EQ(icursH28, 3);
}
