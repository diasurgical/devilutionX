#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

#include "engine/points_in_rectangle_range.hpp"

namespace devilution {

TEST(DrlgTest, RectangleRangeIterator)
{
	constexpr Rectangle topLeftArea = Rectangle { Point { 1, 1 }, 1 };
	constexpr Rectangle bottomRightArea = Rectangle { Point { 3, 3 }, 1 };
	// Dungeon generation depends on the iteration order remaining unchanged to ensure we generate the same layout as vanilla Diablo/Hellfire
	std::array<std::array<int, 5>, 5> region {};

	int counter = 0;
	// Iterate over a 9 tile area in the top left of the region.
	for (Point position : PointsInRectangleRange { topLeftArea }) {
		region[position.x][position.y] = ++counter;
	}

	EXPECT_EQ(counter, 9) << "Iterating over a 9 tile range should return exactly 9 points";
	EXPECT_EQ(region[2][2], 9) << "Iterating over a 9 tile range should return exactly 9 points";

	EXPECT_EQ(region[0][0], 1) << "Default order should be row-major (where x defines the column, y defines the row)";
	EXPECT_EQ(region[1][0], 2) << "Default order should be row-major (where x defines the column, y defines the row)";
	EXPECT_EQ(region[2][0], 3) << "Default order should be row-major (where x defines the column, y defines the row)";
	EXPECT_EQ(region[0][2], 7) << "Default order should be row-major (where x defines the column, y defines the row)";

	EXPECT_EQ(region[0][3], 0) << "Iterator should not return out of bounds points";
	EXPECT_EQ(region[3][0], 0) << "Iterator should not return out of bounds points";

	region = {};
	counter = 0;

	PointsInRectangleRange rowMajorRange { bottomRightArea };
	std::for_each(rowMajorRange.rbegin(), rowMajorRange.rend(), [&region, &counter](Point position) { region[position.x][position.y] = ++counter; });
	EXPECT_EQ(region[4][4], 1) << "Reverse iterators are required";
	EXPECT_EQ(region[2][4], 3) << "Reverse iterators are required";
	EXPECT_EQ(region[4][2], 7) << "Reverse iterators are required";
	EXPECT_EQ(region[2][2], 9) << "Reverse iterators are required";

	region = {};
	counter = 0;

	for (Point position : PointsInRectangleRangeColMajor { topLeftArea }) {
		region[position.x][position.y] = ++counter;
	}

	EXPECT_EQ(counter, 9) << "Iterating over a 9 tile range should return exactly 9 points";
	EXPECT_EQ(region[2][2], 9) << "Iterating over a 9 tile range should return exactly 9 points";

	EXPECT_EQ(region[0][0], 1) << "col-major iterator must use col-major order (where x defines the column, y defines the row)";
	EXPECT_EQ(region[0][1], 2) << "col-major iterator must use col-major order (where x defines the column, y defines the row)";
	EXPECT_EQ(region[0][2], 3) << "col-major iterator must use col-major order (where x defines the column, y defines the row)";
	EXPECT_EQ(region[2][0], 7) << "col-major iterator must use col-major order (where x defines the column, y defines the row)";

	EXPECT_EQ(region[0][3], 0) << "Iterator should not return out of bounds points";
	EXPECT_EQ(region[3][0], 0) << "Iterator should not return out of bounds points";

	region = {};
	counter = 0;

	PointsInRectangleRangeColMajor colMajorRange { bottomRightArea };
	std::for_each(colMajorRange.rbegin(), colMajorRange.rend(), [&region, &counter](Point position) { region[position.x][position.y] = ++counter; });
	EXPECT_EQ(region[4][4], 1) << "Reverse iterators are required";
	EXPECT_EQ(region[4][2], 3) << "Reverse iterators are required";
	EXPECT_EQ(region[2][4], 7) << "Reverse iterators are required";
	EXPECT_EQ(region[2][2], 9) << "Reverse iterators are required";
}

} // namespace devilution
