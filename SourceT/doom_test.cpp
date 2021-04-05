#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

TEST(Doom, doom_get_frame_from_time)
{
	DoomQuestState = 1200 * 8 + 548;
	EXPECT_EQ(doom_get_frame_from_time(), 8);
}

TEST(Doom, doom_get_frame_from_time_max)
{
	DoomQuestState = 1200 * 30 + 1;
	EXPECT_EQ(doom_get_frame_from_time(), 31);
}
