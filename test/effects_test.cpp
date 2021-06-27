#include <gtest/gtest.h>

#include "effects.h"
#include "player.h"

using namespace devilution;

TEST(Effects, calc_snd_position_center)
{
	plr[myplr].position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(calc_snd_position({ 50, 50 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, 0);
	EXPECT_EQ(plPan, 0);
}

TEST(Effects, calc_snd_position_near)
{
	plr[myplr].position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(calc_snd_position({ 55, 50 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, -320);
	EXPECT_EQ(plPan, 1280);
}

TEST(Effects, calc_snd_position_out_of_range)
{
	plr[myplr].position.tile = { 12, 12 };
	int plVolume = 1234;
	int plPan = 0;
	EXPECT_EQ(calc_snd_position({ 112, 112 }, &plVolume, &plPan), false);
	EXPECT_EQ(plVolume, 1234);
	EXPECT_EQ(plPan, 0);
}

TEST(Effects, calc_snd_position_extreme_right)
{
	plr[myplr].position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(calc_snd_position({ 75, 25 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, -2176);
	EXPECT_EQ(plPan, 6400);
}

TEST(Effects, calc_snd_position_extreme_left)
{
	plr[myplr].position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(calc_snd_position({ 25, 75 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, -2176);
	EXPECT_EQ(plPan, -6400);
}
