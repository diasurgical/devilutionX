#include <gtest/gtest.h>
#include "all.h"

TEST(Doom, doom_get_frame_from_time) {
	dvl::DoomQuestState = 1200 * 8 + 548;
	EXPECT_EQ(dvl::doom_get_frame_from_time(), 8);
}

TEST(Doom, doom_get_frame_from_time_max) {
	dvl::DoomQuestState = 1200 * 30 + 1;
	EXPECT_EQ(dvl::doom_get_frame_from_time(), 31);
}
