#include <gtest/gtest.h>

#include "cursor.h"

using namespace devilution;

TEST(Cursor, NewCursor)
{
	NewCursor(CURSOR_HOURGLASS);
	EXPECT_EQ(pcurs, CURSOR_HOURGLASS);
}
