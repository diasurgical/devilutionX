#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

TEST(Diablo, diablo_pause_game_unpause)
{
	gbIsMultiplayer = false;
	PauseMode = 1;
	diablo_pause_game();
	EXPECT_EQ(PauseMode, 0);
}
