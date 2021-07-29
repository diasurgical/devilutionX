#include <gtest/gtest.h>

#include "missiles.h"

using namespace devilution;

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
	EXPECT_EQ(Direction16::DIR16_S, GetDirection16({ 0, 0 }, { 15, 15 }));
	EXPECT_EQ(Direction16::DIR16_SW, GetDirection16({ 0, 0 }, { 0, 15 }));
	EXPECT_EQ(Direction16::DIR16_Sw, GetDirection16({ 0, 0 }, { 8, 15 }));
	EXPECT_EQ(Direction16::DIR16_S, GetDirection16({ 0, 0 }, { 8, 8 }));
	EXPECT_EQ(Direction16::DIR16_Se, GetDirection16({ 0, 0 }, { 15, 8 }));
	EXPECT_EQ(Direction16::DIR16_Se, GetDirection16({ 0, 0 }, { 15, 7 }));
	EXPECT_EQ(Direction16::DIR16_Se, GetDirection16({ 0, 0 }, { 11, 7 }));
	EXPECT_EQ(Direction16::DIR16_S, GetDirection16({ 0, 0 }, { 8, 11 }));
	EXPECT_EQ(Direction16::DIR16_N, GetDirection16({ 15, 15 }, { 0, 0 }));
	EXPECT_EQ(Direction16::DIR16_NE, GetDirection16({ 0, 15 }, { 0, 0 }));
	EXPECT_EQ(Direction16::DIR16_Ne, GetDirection16({ 8, 15 }, { 0, 0 }));
	EXPECT_EQ(Direction16::DIR16_N, GetDirection16({ 8, 8 }, { 0, 0 }));
	EXPECT_EQ(Direction16::DIR16_Nw, GetDirection16({ 15, 8 }, { 0, 0 }));
	EXPECT_EQ(Direction16::DIR16_Nw, GetDirection16({ 15, 7 }, { 0, 0 }));
	EXPECT_EQ(Direction16::DIR16_Nw, GetDirection16({ 11, 7 }, { 0, 0 }));
	EXPECT_EQ(Direction16::DIR16_N, GetDirection16({ 8, 11 }, { 0, 0 }));
	EXPECT_EQ(Direction16::DIR16_E, GetDirection16({ 0, 15 }, { 15, 0 }));
	EXPECT_EQ(Direction16::DIR16_SE, GetDirection16({ 0, 0 }, { 15, 0 }));
	EXPECT_EQ(Direction16::DIR16_sE, GetDirection16({ 0, 8 }, { 15, 0 }));
	EXPECT_EQ(Direction16::DIR16_E, GetDirection16({ 0, 8 }, { 8, 0 }));
	EXPECT_EQ(Direction16::DIR16_nE, GetDirection16({ 0, 15 }, { 8, 0 }));
	EXPECT_EQ(Direction16::DIR16_nE, GetDirection16({ 0, 15 }, { 7, 0 }));
	EXPECT_EQ(Direction16::DIR16_nE, GetDirection16({ 0, 11 }, { 7, 0 }));
	EXPECT_EQ(Direction16::DIR16_E, GetDirection16({ 0, 8 }, { 11, 0 }));

	EXPECT_EQ(Direction16::DIR16_S, GetDirection16({ 2, 2 }, { 3, 3 }));
	EXPECT_EQ(Direction16::DIR16_Sw, GetDirection16({ 2, 2 }, { 3, 4 }));
	EXPECT_EQ(Direction16::DIR16_SW, GetDirection16({ 2, 2 }, { 2, 4 }));
	EXPECT_EQ(Direction16::DIR16_sW, GetDirection16({ 2, 2 }, { 1, 4 }));
	EXPECT_EQ(Direction16::DIR16_W, GetDirection16({ 2, 2 }, { 1, 3 }));
	EXPECT_EQ(Direction16::DIR16_nW, GetDirection16({ 2, 2 }, { 0, 3 }));
	EXPECT_EQ(Direction16::DIR16_NW, GetDirection16({ 2, 2 }, { 0, 2 }));
	EXPECT_EQ(Direction16::DIR16_Nw, GetDirection16({ 2, 2 }, { 0, 1 }));
	EXPECT_EQ(Direction16::DIR16_N, GetDirection16({ 2, 2 }, { 1, 1 }));
	EXPECT_EQ(Direction16::DIR16_Ne, GetDirection16({ 2, 2 }, { 1, 0 }));
	EXPECT_EQ(Direction16::DIR16_NE, GetDirection16({ 2, 2 }, { 2, 0 }));
	EXPECT_EQ(Direction16::DIR16_nE, GetDirection16({ 2, 2 }, { 3, 0 }));
	EXPECT_EQ(Direction16::DIR16_E, GetDirection16({ 2, 2 }, { 3, 1 }));
	EXPECT_EQ(Direction16::DIR16_sE, GetDirection16({ 2, 2 }, { 4, 1 }));
	EXPECT_EQ(Direction16::DIR16_SE, GetDirection16({ 2, 2 }, { 4, 2 }));
	EXPECT_EQ(Direction16::DIR16_Se, GetDirection16({ 2, 2 }, { 4, 3 }));

	EXPECT_EQ(Direction16::DIR16_Sw, GetDirection16({ 0, 0 }, { 0, 0 })) << "GetDirection16 is expected to default to DIR16_Sw when the points occupy the same tile";
}
