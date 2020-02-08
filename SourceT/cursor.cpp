#include <gtest/gtest.h>
#include "devilution.h"

TEST(Cursor, SetCursor) {
	int i = dvl::ICURS_SPIKED_CLUB + dvl::CURSOR_FIRSTITEM;
	dvl::SetCursor_(i);
	ASSERT_EQ(dvl::pcurs, i);
	ASSERT_EQ(dvl::cursW, 1 * 28);
	ASSERT_EQ(dvl::cursH, 3 * 28);
	ASSERT_EQ(dvl::icursW, 1 * 28);
	ASSERT_EQ(dvl::icursH, 3 * 28);
	ASSERT_EQ(dvl::icursW28, 1);
	ASSERT_EQ(dvl::icursH28, 3);
}
