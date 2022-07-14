#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <random>

#include "engine/points_in_rectangle_range.hpp"
#include "levels/gendung.h"

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

std::optional<Size> LegacyThemeRoomSize()
{
	bool yFlag = true;
	bool xFlag = true;
	int xCount = 0;
	int yCount = 0;

	int xArray[20] = {};
	int yArray[20] = {};

	for (int ii = 0; ii < 10; ii++) {
		if (xFlag && ii < DMAXY) {
			for (int xx = 0; xx < 10 && xx < DMAXX; xx++) {
				if (dungeon[xx][ii] != 0) {
					if (xx >= 5) {
						break;
					}
					xFlag = false;
				} else {
					xCount++;
				}
			}
			if (xFlag) {
				xArray[ii] = xCount;
				xCount = 0;
			}
		}
		if (yFlag && ii < DMAXX) {
			for (int yy = 0; yy < 10 && yy < DMAXY; yy++) {
				if (dungeon[ii][yy] != 0) {
					if (yy >= 5) {
						break;
					}
					yFlag = false;
				} else {
					yCount++;
				}
			}
			if (yFlag) {
				yArray[ii] = yCount;
				yCount = 0;
			}
		}
	}

	for (int ii = 0; ii < 5; ii++) {
		if (xArray[ii] < 5 || yArray[ii] < 5) {
			return {};
		}
	}

	int xSmallest = xArray[0];
	int ySmallest = yArray[0];

	for (int ii = 0; ii < 10; ii++) {
		if (xArray[ii] < 5 || yArray[ii] < 5) {
			break;
		}
		if (xArray[ii] < xSmallest) {
			xSmallest = xArray[ii];
		}
		if (yArray[ii] < ySmallest) {
			ySmallest = yArray[ii];
		}
	}
	return Size { xSmallest, ySmallest } - 2;
}

TEST(DrlgTest, ThemeRoomSize)
{
	memset(dungeon, 0, sizeof(dungeon));

	EXPECT_EQ(GetSizeForThemeRoom(), Size(8, 8)) << "All floor theme area should be 8x8";

	dungeon[9][9] = 1;
	EXPECT_EQ(GetSizeForThemeRoom(), Size(7, 7)) << "Corners shrink the chosen dimensions";
	dungeon[9][5] = 1;
	EXPECT_EQ(GetSizeForThemeRoom(), Size(7, 3)) << "Minimum dimensions are determined by corners outside the min area";
	dungeon[9][4] = 1;
	EXPECT_EQ(GetSizeForThemeRoom(), Size(7, 8)) << "Walls below the min size let larger opposing dimensions get picked";
	dungeon[9][5] = 0;
	dungeon[9][4] = 0;
	dungeon[9][9] = 0;

	// Time for some fucked up cases
	dungeon[7][2] = 1;
	dungeon[5][9] = 1;
	EXPECT_EQ(GetSizeForThemeRoom(), Size(5, 7)) << "Search space terminates at width 8 due to the wall being in the first three rows";

	dungeon[6][4] = 1;
	EXPECT_EQ(GetSizeForThemeRoom(), Size(4, 7)) << "Smallest width now defined by row 5, height still extends due to minSize";
	dungeon[6][4] = 0;

	dungeon[5][9] = 0;
	dungeon[7][2] = 0;

	dungeon[7][0] = 1;
	dungeon[6][6] = 1;
	dungeon[8][5] = 1;
	EXPECT_EQ(GetSizeForThemeRoom(), Size(4, 4)) << "Search is terminated by the 0 width row 7, inset corner gives a larger height than otherwise expected";

	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_int_distribution dist(0, 99);
	for (size_t i = 0; i < 50000000U; i++) {
		int roll = dist(engine);
		int x = roll / 10;
		int y = roll % 10;

		if (dungeon[x][y] == 0) {
			dungeon[x][y] = 1;
		} else {
			dungeon[x][y] = 0;
		}

		auto newSize = GetSizeForThemeRoom();
		auto legacySize = LegacyThemeRoomSize();

		if (newSize != legacySize) {
			EXPECT_EQ(newSize, legacySize) << "Sizes differ";

			Size uwNew = newSize.value_or(Size());
			Size uwLegacy = legacySize.value_or(Size());
			for (int col = 0; col < 10; col++) {
				std::string ascii(10, '.');
				for (int row = 0; row < 10; row++) {
					if (dungeon[row][col] != 0) {
						ascii[row] = '+';
					} else if (row < uwNew.width + 2 && col < uwNew.height + 2) {
						ascii[row] = '%';
					} else if (row < uwLegacy.width + 2 && col < uwLegacy.height + 2) {
						ascii[row] = '@';
					}
				}
				std::cout << ascii << "\n";
			}
		}

		if (x < 5 && y < 5)
			dungeon[x][y] = 0;
	}
}

} // namespace devilution
