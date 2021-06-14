#include <gtest/gtest.h>

#include "missiles.h"

using namespace devilution;

TEST(Missiles, GetDirection8)
{
	EXPECT_EQ(0, GetDirection({ 0, 0 }, { 15, 15 }));
	EXPECT_EQ(1, GetDirection({ 0, 0 }, { 0, 15 }));
	EXPECT_EQ(0, GetDirection({ 0, 0 }, { 8, 15 }));
	EXPECT_EQ(0, GetDirection({ 0, 0 }, { 8, 8 }));
	EXPECT_EQ(0, GetDirection({ 0, 0 }, { 15, 8 }));
	EXPECT_EQ(0, GetDirection({ 0, 0 }, { 15, 7 }));
	EXPECT_EQ(0, GetDirection({ 0, 0 }, { 11, 7 }));
	EXPECT_EQ(0, GetDirection({ 0, 0 }, { 8, 11 }));
	EXPECT_EQ(4, GetDirection({ 15, 15 }, { 0, 0 }));
	EXPECT_EQ(5, GetDirection({ 0, 15 }, { 0, 0 }));
	EXPECT_EQ(4, GetDirection({ 8, 15 }, { 0, 0 }));
	EXPECT_EQ(4, GetDirection({ 8, 8 }, { 0, 0 }));
	EXPECT_EQ(4, GetDirection({ 15, 8 }, { 0, 0 }));
	EXPECT_EQ(4, GetDirection({ 15, 7 }, { 0, 0 }));
	EXPECT_EQ(4, GetDirection({ 11, 7 }, { 0, 0 }));
	EXPECT_EQ(4, GetDirection({ 8, 11 }, { 0, 0 }));
	EXPECT_EQ(6, GetDirection({ 0, 15 }, { 15, 0 }));
	EXPECT_EQ(7, GetDirection({ 0, 0 }, { 15, 0 }));
	EXPECT_EQ(6, GetDirection({ 0, 8 }, { 15, 0 }));
	EXPECT_EQ(6, GetDirection({ 0, 8 }, { 8, 0 }));
	EXPECT_EQ(6, GetDirection({ 0, 15 }, { 8, 0 }));
	EXPECT_EQ(6, GetDirection({ 0, 15 }, { 7, 0 }));
	EXPECT_EQ(6, GetDirection({ 0, 11 }, { 7, 0 }));
	EXPECT_EQ(6, GetDirection({ 0, 8 }, { 11, 0 }));

	EXPECT_EQ(0, GetDirection({ 1, 1 }, { 2, 2 }));
	EXPECT_EQ(1, GetDirection({ 1, 1 }, { 1, 2 }));
	EXPECT_EQ(2, GetDirection({ 1, 1 }, { 0, 2 }));
	EXPECT_EQ(3, GetDirection({ 1, 1 }, { 0, 1 }));
	EXPECT_EQ(4, GetDirection({ 1, 1 }, { 0, 0 }));
	EXPECT_EQ(5, GetDirection({ 1, 1 }, { 1, 0 }));
	EXPECT_EQ(6, GetDirection({ 1, 1 }, { 2, 0 }));
	EXPECT_EQ(7, GetDirection({ 1, 1 }, { 2, 1 }));
}

TEST(Missiles, GetDirection16)
{
	EXPECT_EQ(0, GetDirection16({ 0, 0 }, { 15, 15 }));
	EXPECT_EQ(2, GetDirection16({ 0, 0 }, { 0, 15 }));
	EXPECT_EQ(1, GetDirection16({ 0, 0 }, { 8, 15 }));
	EXPECT_EQ(0, GetDirection16({ 0, 0 }, { 8, 8 }));
	EXPECT_EQ(15, GetDirection16({ 0, 0 }, { 15, 8 }));
	EXPECT_EQ(15, GetDirection16({ 0, 0 }, { 15, 7 }));
	EXPECT_EQ(15, GetDirection16({ 0, 0 }, { 11, 7 }));
	EXPECT_EQ(0, GetDirection16({ 0, 0 }, { 8, 11 }));
	EXPECT_EQ(8, GetDirection16({ 15, 15 }, { 0, 0 }));
	EXPECT_EQ(10, GetDirection16({ 0, 15 }, { 0, 0 }));
	EXPECT_EQ(9, GetDirection16({ 8, 15 }, { 0, 0 }));
	EXPECT_EQ(8, GetDirection16({ 8, 8 }, { 0, 0 }));
	EXPECT_EQ(7, GetDirection16({ 15, 8 }, { 0, 0 }));
	EXPECT_EQ(7, GetDirection16({ 15, 7 }, { 0, 0 }));
	EXPECT_EQ(7, GetDirection16({ 11, 7 }, { 0, 0 }));
	EXPECT_EQ(8, GetDirection16({ 8, 11 }, { 0, 0 }));
	EXPECT_EQ(12, GetDirection16({ 0, 15 }, { 15, 0 }));
	EXPECT_EQ(14, GetDirection16({ 0, 0 }, { 15, 0 }));
	EXPECT_EQ(13, GetDirection16({ 0, 8 }, { 15, 0 }));
	EXPECT_EQ(12, GetDirection16({ 0, 8 }, { 8, 0 }));
	EXPECT_EQ(11, GetDirection16({ 0, 15 }, { 8, 0 }));
	EXPECT_EQ(11, GetDirection16({ 0, 15 }, { 7, 0 }));
	EXPECT_EQ(11, GetDirection16({ 0, 11 }, { 7, 0 }));
	EXPECT_EQ(12, GetDirection16({ 0, 8 }, { 11, 0 }));

	EXPECT_EQ(0, GetDirection16({ 2, 2 }, { 3, 3 }));
	EXPECT_EQ(1, GetDirection16({ 2, 2 }, { 3, 4 }));
	EXPECT_EQ(2, GetDirection16({ 2, 2 }, { 2, 4 }));
	EXPECT_EQ(3, GetDirection16({ 2, 2 }, { 1, 4 }));
	EXPECT_EQ(4, GetDirection16({ 2, 2 }, { 1, 3 }));
	EXPECT_EQ(5, GetDirection16({ 2, 2 }, { 0, 3 }));
	EXPECT_EQ(6, GetDirection16({ 2, 2 }, { 0, 2 }));
	EXPECT_EQ(7, GetDirection16({ 2, 2 }, { 0, 1 }));
	EXPECT_EQ(8, GetDirection16({ 2, 2 }, { 1, 1 }));
	EXPECT_EQ(9, GetDirection16({ 2, 2 }, { 1, 0 }));
	EXPECT_EQ(10, GetDirection16({ 2, 2 }, { 2, 0 }));
	EXPECT_EQ(11, GetDirection16({ 2, 2 }, { 3, 0 }));
	EXPECT_EQ(12, GetDirection16({ 2, 2 }, { 3, 1 }));
	EXPECT_EQ(13, GetDirection16({ 2, 2 }, { 4, 1 }));
	EXPECT_EQ(14, GetDirection16({ 2, 2 }, { 4, 2 }));
	EXPECT_EQ(15, GetDirection16({ 2, 2 }, { 4, 3 }));
}
