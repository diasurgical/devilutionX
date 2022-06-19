#include <gtest/gtest.h>

#include "effects.h"
#include "player.h"

using namespace devilution;

TEST(Effects, CalculateSoundPosition_center)
{
	Players.resize(1);
	MyPlayer = &Players[0];
	MyPlayer->position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(CalculateSoundPosition({ 50, 50 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, 0);
	EXPECT_EQ(plPan, 0);
}

TEST(Effects, CalculateSoundPosition_near)
{
	Players.resize(1);
	MyPlayer = &Players[0];
	MyPlayer->position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(CalculateSoundPosition({ 55, 50 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, -320);
	EXPECT_EQ(plPan, 1280);
}

TEST(Effects, CalculateSoundPosition_out_of_range)
{
	Players.resize(1);
	MyPlayer = &Players[0];
	MyPlayer->position.tile = { 12, 12 };
	int plVolume = 1234;
	int plPan = 0;
	EXPECT_EQ(CalculateSoundPosition({ 112, 112 }, &plVolume, &plPan), false);
	EXPECT_EQ(plVolume, 1234);
	EXPECT_EQ(plPan, 0);
}

TEST(Effects, CalculateSoundPosition_extreme_right)
{
	Players.resize(1);
	MyPlayer = &Players[0];
	MyPlayer->position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(CalculateSoundPosition({ 75, 25 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, -2176);
	EXPECT_EQ(plPan, 6400);
}

TEST(Effects, CalculateSoundPosition_extreme_left)
{
	Players.resize(1);
	MyPlayer = &Players[0];
	MyPlayer->position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(CalculateSoundPosition({ 25, 75 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, -2176);
	EXPECT_EQ(plPan, -6400);
}
