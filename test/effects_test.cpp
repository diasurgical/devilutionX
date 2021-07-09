#include <gtest/gtest.h>

#include "effects.h"
#include "player.h"

using namespace devilution;

TEST(Effects, CalculatePosition_center)
{
	Players[MyPlayerId].position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(TestCalculatePosition({ 50, 50 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, 0);
	EXPECT_EQ(plPan, 0);
}

TEST(Effects, CalculatePosition_near)
{
	Players[MyPlayerId].position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(TestCalculatePosition({ 55, 50 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, -320);
	EXPECT_EQ(plPan, 1280);
}

TEST(Effects, CalculatePosition_out_of_range)
{
	Players[MyPlayerId].position.tile = { 12, 12 };
	int plVolume = 1234;
	int plPan = 0;
	EXPECT_EQ(TestCalculatePosition({ 112, 112 }, &plVolume, &plPan), false);
	EXPECT_EQ(plVolume, 1234);
	EXPECT_EQ(plPan, 0);
}

TEST(Effects, CalculatePosition_extreme_right)
{
	Players[MyPlayerId].position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(TestCalculatePosition({ 75, 25 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, -2176);
	EXPECT_EQ(plPan, 6400);
}

TEST(Effects, CalculatePosition_extreme_left)
{
	Players[MyPlayerId].position.tile = { 50, 50 };
	int plVolume = 0;
	int plPan = 0;
	EXPECT_EQ(TestCalculatePosition({ 25, 75 }, &plVolume, &plPan), true);
	EXPECT_EQ(plVolume, -2176);
	EXPECT_EQ(plPan, -6400);
}
