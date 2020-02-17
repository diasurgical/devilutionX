#include <gtest/gtest.h>
#include "all.h"

TEST(Cursor, SetCursor) {
	int i = dvl::ICURS_SPIKED_CLUB + dvl::CURSOR_FIRSTITEM;
	dvl::SetCursor_(i);
	EXPECT_EQ(dvl::pcurs, i);
	EXPECT_EQ(dvl::cursW, 1 * 28);
	EXPECT_EQ(dvl::cursH, 3 * 28);
	EXPECT_EQ(dvl::icursW, 1 * 28);
	EXPECT_EQ(dvl::icursH, 3 * 28);
	EXPECT_EQ(dvl::icursW28, 1);
	EXPECT_EQ(dvl::icursH28, 3);
}
