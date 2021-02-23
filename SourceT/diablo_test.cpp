#include <gtest/gtest.h>
#include "all.h"

TEST(Diablo, diablo_pause_game_unpause)
{
	dvl::gbIsMultiplayer = false;
	dvl::PauseMode = 1;
	dvl::diablo_pause_game();
	EXPECT_EQ(dvl::PauseMode, 0);
}
