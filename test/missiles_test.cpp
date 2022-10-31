#include <gtest/gtest.h>

#include "engine/random.hpp"
#include "missiles.h"

using namespace devilution;

TEST(Missiles, RotateBlockedMissileArrow)
{
	Players.resize(1);
	MyPlayerId = 0;
	MyPlayer = &Players[MyPlayerId];
	*MyPlayer = {};

	Player &player = Players[0];
	// missile can be a copy or a reference, there's no nullptr check and the functions that use it don't expect the instance to be part of a global structure so it doesn't really matter for this use.
	Missile missile = *AddMissile({ 0, 0 }, { 0, 0 }, Direction::South, MIS_ARROW, TARGET_MONSTERS, player.getId(), 0, 0);
	EXPECT_EQ(missile._miAnimFrame, 1);

	SetRndSeed(0);
	TestRotateBlockedMissile(missile);
	EXPECT_EQ(missile._miAnimFrame, 16);

	SetRndSeed(3210);
	TestRotateBlockedMissile(missile);
	EXPECT_EQ(missile._miAnimFrame, 1);

	missile._miAnimFrame = 5;
	SetRndSeed(1234);
	TestRotateBlockedMissile(missile);
	EXPECT_EQ(missile._miAnimFrame, 6);
}

TEST(Missiles, GetDirection8)
{
	EXPECT_EQ(Direction::South, GetDirection({ 0, 0 }, { 15, 15 }));
	EXPECT_EQ(Direction::SouthWest, GetDirection({ 0, 0 }, { 0, 15 }));
	EXPECT_EQ(Direction::South, GetDirection({ 0, 0 }, { 8, 15 }));
	EXPECT_EQ(Direction::South, GetDirection({ 0, 0 }, { 8, 8 }));
	EXPECT_EQ(Direction::South, GetDirection({ 0, 0 }, { 15, 8 }));
	EXPECT_EQ(Direction::South, GetDirection({ 0, 0 }, { 15, 7 }));
	EXPECT_EQ(Direction::South, GetDirection({ 0, 0 }, { 11, 7 }));
	EXPECT_EQ(Direction::South, GetDirection({ 0, 0 }, { 8, 11 }));
	EXPECT_EQ(Direction::North, GetDirection({ 15, 15 }, { 0, 0 }));
	EXPECT_EQ(Direction::NorthEast, GetDirection({ 0, 15 }, { 0, 0 }));
	EXPECT_EQ(Direction::North, GetDirection({ 8, 15 }, { 0, 0 }));
	EXPECT_EQ(Direction::North, GetDirection({ 8, 8 }, { 0, 0 }));
	EXPECT_EQ(Direction::North, GetDirection({ 15, 8 }, { 0, 0 }));
	EXPECT_EQ(Direction::North, GetDirection({ 15, 7 }, { 0, 0 }));
	EXPECT_EQ(Direction::North, GetDirection({ 11, 7 }, { 0, 0 }));
	EXPECT_EQ(Direction::North, GetDirection({ 8, 11 }, { 0, 0 }));
	EXPECT_EQ(Direction::East, GetDirection({ 0, 15 }, { 15, 0 }));
	EXPECT_EQ(Direction::SouthEast, GetDirection({ 0, 0 }, { 15, 0 }));
	EXPECT_EQ(Direction::East, GetDirection({ 0, 8 }, { 15, 0 }));
	EXPECT_EQ(Direction::East, GetDirection({ 0, 8 }, { 8, 0 }));
	EXPECT_EQ(Direction::East, GetDirection({ 0, 15 }, { 8, 0 }));
	EXPECT_EQ(Direction::East, GetDirection({ 0, 15 }, { 7, 0 }));
	EXPECT_EQ(Direction::East, GetDirection({ 0, 11 }, { 7, 0 }));
	EXPECT_EQ(Direction::East, GetDirection({ 0, 8 }, { 11, 0 }));

	EXPECT_EQ(Direction::South, GetDirection({ 1, 1 }, { 2, 2 }));
	EXPECT_EQ(Direction::SouthWest, GetDirection({ 1, 1 }, { 1, 2 }));
	EXPECT_EQ(Direction::West, GetDirection({ 1, 1 }, { 0, 2 }));
	EXPECT_EQ(Direction::NorthWest, GetDirection({ 1, 1 }, { 0, 1 }));
	EXPECT_EQ(Direction::North, GetDirection({ 1, 1 }, { 0, 0 }));
	EXPECT_EQ(Direction::NorthEast, GetDirection({ 1, 1 }, { 1, 0 }));
	EXPECT_EQ(Direction::East, GetDirection({ 1, 1 }, { 2, 0 }));
	EXPECT_EQ(Direction::SouthEast, GetDirection({ 1, 1 }, { 2, 1 }));

	EXPECT_EQ(Direction::SouthWest, GetDirection({ 0, 0 }, { 0, 0 })) << "GetDirection is expected to default to Direction::SouthWest when the points occupy the same tile";
}

TEST(Missiles, GetDirection16)
{
	EXPECT_EQ(Direction16::South, GetDirection16({ 0, 0 }, { 15, 15 }));
	EXPECT_EQ(Direction16::SouthWest, GetDirection16({ 0, 0 }, { 0, 15 }));
	EXPECT_EQ(Direction16::South_SouthWest, GetDirection16({ 0, 0 }, { 8, 15 }));
	EXPECT_EQ(Direction16::South, GetDirection16({ 0, 0 }, { 8, 8 }));
	EXPECT_EQ(Direction16::South_SouthEast, GetDirection16({ 0, 0 }, { 15, 8 }));
	EXPECT_EQ(Direction16::South_SouthEast, GetDirection16({ 0, 0 }, { 15, 7 }));
	EXPECT_EQ(Direction16::South_SouthEast, GetDirection16({ 0, 0 }, { 11, 7 }));
	EXPECT_EQ(Direction16::South, GetDirection16({ 0, 0 }, { 8, 11 }));
	EXPECT_EQ(Direction16::North, GetDirection16({ 15, 15 }, { 0, 0 }));
	EXPECT_EQ(Direction16::NorthEast, GetDirection16({ 0, 15 }, { 0, 0 }));
	EXPECT_EQ(Direction16::North_NorthEast, GetDirection16({ 8, 15 }, { 0, 0 }));
	EXPECT_EQ(Direction16::North, GetDirection16({ 8, 8 }, { 0, 0 }));
	EXPECT_EQ(Direction16::North_NorthWest, GetDirection16({ 15, 8 }, { 0, 0 }));
	EXPECT_EQ(Direction16::North_NorthWest, GetDirection16({ 15, 7 }, { 0, 0 }));
	EXPECT_EQ(Direction16::North_NorthWest, GetDirection16({ 11, 7 }, { 0, 0 }));
	EXPECT_EQ(Direction16::North, GetDirection16({ 8, 11 }, { 0, 0 }));
	EXPECT_EQ(Direction16::East, GetDirection16({ 0, 15 }, { 15, 0 }));
	EXPECT_EQ(Direction16::SouthEast, GetDirection16({ 0, 0 }, { 15, 0 }));
	EXPECT_EQ(Direction16::East_SouthEast, GetDirection16({ 0, 8 }, { 15, 0 }));
	EXPECT_EQ(Direction16::East, GetDirection16({ 0, 8 }, { 8, 0 }));
	EXPECT_EQ(Direction16::East_NorthEast, GetDirection16({ 0, 15 }, { 8, 0 }));
	EXPECT_EQ(Direction16::East_NorthEast, GetDirection16({ 0, 15 }, { 7, 0 }));
	EXPECT_EQ(Direction16::East_NorthEast, GetDirection16({ 0, 11 }, { 7, 0 }));
	EXPECT_EQ(Direction16::East, GetDirection16({ 0, 8 }, { 11, 0 }));

	EXPECT_EQ(Direction16::South, GetDirection16({ 2, 2 }, { 3, 3 }));
	EXPECT_EQ(Direction16::South_SouthWest, GetDirection16({ 2, 2 }, { 3, 4 }));
	EXPECT_EQ(Direction16::SouthWest, GetDirection16({ 2, 2 }, { 2, 4 }));
	EXPECT_EQ(Direction16::West_SouthWest, GetDirection16({ 2, 2 }, { 1, 4 }));
	EXPECT_EQ(Direction16::West, GetDirection16({ 2, 2 }, { 1, 3 }));
	EXPECT_EQ(Direction16::West_NorthWest, GetDirection16({ 2, 2 }, { 0, 3 }));
	EXPECT_EQ(Direction16::NorthWest, GetDirection16({ 2, 2 }, { 0, 2 }));
	EXPECT_EQ(Direction16::North_NorthWest, GetDirection16({ 2, 2 }, { 0, 1 }));
	EXPECT_EQ(Direction16::North, GetDirection16({ 2, 2 }, { 1, 1 }));
	EXPECT_EQ(Direction16::North_NorthEast, GetDirection16({ 2, 2 }, { 1, 0 }));
	EXPECT_EQ(Direction16::NorthEast, GetDirection16({ 2, 2 }, { 2, 0 }));
	EXPECT_EQ(Direction16::East_NorthEast, GetDirection16({ 2, 2 }, { 3, 0 }));
	EXPECT_EQ(Direction16::East, GetDirection16({ 2, 2 }, { 3, 1 }));
	EXPECT_EQ(Direction16::East_SouthEast, GetDirection16({ 2, 2 }, { 4, 1 }));
	EXPECT_EQ(Direction16::SouthEast, GetDirection16({ 2, 2 }, { 4, 2 }));
	EXPECT_EQ(Direction16::South_SouthEast, GetDirection16({ 2, 2 }, { 4, 3 }));

	EXPECT_EQ(Direction16::South_SouthWest, GetDirection16({ 0, 0 }, { 0, 0 })) << "GetDirection16 is expected to default to Direction16::South_SouthWest when the points occupy the same tile";
}
