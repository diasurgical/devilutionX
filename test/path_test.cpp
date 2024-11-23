#include "engine/path.h"

#include <algorithm>
#include <span>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/direction.hpp"
#include "levels/dun_tile.hpp"
#include "levels/gendung.h"
#include "objects.h"
#include "utils/algorithm/container.hpp"

namespace devilution {

extern int TestPathGetHeuristicCost(Point startPosition, Point destinationPosition);

namespace {

using ::testing::ElementsAreArray;

TEST(PathTest, Heuristics)
{
	constexpr Point source { 25, 32 };
	Point destination = source;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 0) << "Wrong cost for travelling to the same tile";

	destination = source + Direction::NorthEast;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 2) << "Wrong cost for travelling to horizontal/vertical adjacent tile";
	destination = source + Direction::SouthEast;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 2) << "Wrong cost for travelling to horizontal/vertical adjacent tile";
	destination = source + Direction::SouthWest;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 2) << "Wrong cost for travelling to horizontal/vertical adjacent tile";
	destination = source + Direction::NorthWest;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 2) << "Wrong cost for travelling to horizontal/vertical adjacent tile";

	destination = source + Direction::North;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 4) << "Wrong cost for travelling to diagonally adjacent tile";
	destination = source + Direction::East;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 4) << "Wrong cost for travelling to diagonally adjacent tile";
	destination = source + Direction::South;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 4) << "Wrong cost for travelling to diagonally adjacent tile";
	destination = source + Direction::West;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 4) << "Wrong cost for travelling to diagonally adjacent tile";
	destination = source + Direction::SouthWest + Direction::SouthEast; // Effectively the same as Direction::South
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 4) << "Wrong cost for travelling to diagonally adjacent tile";

	destination = source + Direction::NorthEast + Direction::North;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 6) << "Wrong cost for travelling to a { 2, 1 } offset";
	destination = source + Direction::SouthEast + Direction::SouthEast;
	EXPECT_EQ(TestPathGetHeuristicCost(source, destination), 4) << "Wrong cost for travelling to a { 2, 0 } offset";
}

TEST(PathTest, Solid)
{
	dPiece[5][5] = 0;
	SOLData[0] = TileProperties::Solid;
	EXPECT_TRUE(IsTileSolid({ 5, 5 })) << "Solid in-bounds tiles are solid";
	EXPECT_FALSE(IsTileNotSolid({ 5, 5 })) << "IsTileNotSolid returns the inverse of IsTileSolid for in-bounds tiles";

	dPiece[6][6] = 1;
	SOLData[1] = TileProperties::None;
	EXPECT_FALSE(IsTileSolid({ 6, 6 })) << "Non-solid in-bounds tiles are not solid";
	EXPECT_TRUE(IsTileNotSolid({ 6, 6 })) << "IsTileNotSolid returns the inverse of IsTileSolid for in-bounds tiles";

	EXPECT_FALSE(IsTileSolid({ -1, 1 })) << "Out of bounds tiles are not solid"; // this reads out of bounds in the current code and may fail unexpectedly
	EXPECT_FALSE(IsTileNotSolid({ -1, 1 })) << "Out of bounds tiles are also not not solid";
}

TEST(PathTest, CanStepTest)
{
	dPiece[0][0] = 0;
	dPiece[0][1] = 0;
	dPiece[1][0] = 0;
	dPiece[1][1] = 0;
	SOLData[0] = TileProperties::None;
	EXPECT_TRUE(CanStep({ 0, 0 }, { 1, 1 })) << "A step in open space is free of solid pieces";
	EXPECT_TRUE(CanStep({ 1, 1 }, { 0, 0 })) << "A step in open space is free of solid pieces";
	EXPECT_TRUE(CanStep({ 1, 0 }, { 0, 1 })) << "A step in open space is free of solid pieces";
	EXPECT_TRUE(CanStep({ 0, 1 }, { 1, 0 })) << "A step in open space is free of solid pieces";

	SOLData[1] = TileProperties::Solid;
	dPiece[1][0] = 1;
	EXPECT_TRUE(CanStep({ 0, 1 }, { 1, 0 })) << "Can path to a destination which is solid";
	EXPECT_TRUE(CanStep({ 1, 0 }, { 0, 1 })) << "Can path from a starting position which is solid";
	EXPECT_TRUE(CanStep({ 0, 1 }, { 1, 1 })) << "Stepping in a cardinal direction ignores solid pieces";
	EXPECT_TRUE(CanStep({ 1, 0 }, { 1, 1 })) << "Stepping in a cardinal direction ignores solid pieces";
	EXPECT_TRUE(CanStep({ 0, 0 }, { 1, 0 })) << "Stepping in a cardinal direction ignores solid pieces";
	EXPECT_TRUE(CanStep({ 1, 1 }, { 1, 0 })) << "Stepping in a cardinal direction ignores solid pieces";

	EXPECT_FALSE(CanStep({ 0, 0 }, { 1, 1 })) << "Can't cut a solid corner";
	EXPECT_FALSE(CanStep({ 1, 1 }, { 0, 0 })) << "Can't cut a solid corner";
	dPiece[0][1] = 1;
	EXPECT_FALSE(CanStep({ 0, 0 }, { 1, 1 })) << "Can't walk through the boundary between two corners";
	EXPECT_FALSE(CanStep({ 1, 1 }, { 0, 0 })) << "Can't walk through the boundary between two corners";
	dPiece[1][0] = 0;
	EXPECT_FALSE(CanStep({ 0, 0 }, { 1, 1 })) << "Can't cut a solid corner";
	EXPECT_FALSE(CanStep({ 1, 1 }, { 0, 0 })) << "Can't cut a solid corner";
	dPiece[0][1] = 0;

	dPiece[0][0] = 1;
	EXPECT_FALSE(CanStep({ 1, 0 }, { 0, 1 })) << "Can't cut a solid corner";
	EXPECT_FALSE(CanStep({ 0, 1 }, { 1, 0 })) << "Can't cut a solid corner";
	dPiece[1][1] = 1;
	EXPECT_FALSE(CanStep({ 1, 0 }, { 0, 1 })) << "Can't walk through the boundary between two corners";
	EXPECT_FALSE(CanStep({ 0, 1 }, { 1, 0 })) << "Can't walk through the boundary between two corners";
	dPiece[0][0] = 0;
	EXPECT_FALSE(CanStep({ 1, 0 }, { 0, 1 })) << "Can't cut a solid corner";
	EXPECT_FALSE(CanStep({ 0, 1 }, { 1, 0 })) << "Can't cut a solid corner";
	dPiece[1][1] = 0;
}

// These symbols are in terms of coordinates (not in terms of on-screen direction).
// -1, -1 is top-left.
enum class Dir {
	None,
	Up,
	Left,
	Right,
	Down,
	UpLeft,
	UpRight,
	DownRight,
	DownLeft
};
std::array<std::string_view, 9> DirSymbols = { "∅", "↑", "←", "→", "↓", "↖", "↗", "↘", "↙" };

std::ostream &operator<<(std::ostream &os, Dir dir)
{
	return os << DirSymbols[static_cast<size_t>(dir)];
}

std::vector<Dir> ToSyms(std::span<const std::string> strings)
{
	std::vector<Dir> result;
	result.reserve(strings.size());
	for (const std::string &str : strings)
		result.emplace_back(static_cast<Dir>(std::distance(DirSymbols.begin(), c_find(DirSymbols, str))));
	return result;
}

std::vector<Dir> ToSyms(std::span<const int8_t> indices)
{
	std::vector<Dir> result;
	result.reserve(indices.size());
	for (const int8_t idx : indices)
		result.emplace_back(static_cast<Dir>(idx));
	return result;
}

void CheckPath(Point startPosition, Point destinationPosition, std::vector<std::string> expectedSteps)
{
	int8_t pathSteps[MaxPathLength];
	auto pathLength = FindPath([](Point) { return true; }, startPosition, destinationPosition, pathSteps);
	EXPECT_THAT(ToSyms(std::span<const int8_t>(pathSteps, pathLength)), ElementsAreArray(ToSyms(expectedSteps)))
	    << "Path steps differs from expectation for a path from "
	    << startPosition << " to " << destinationPosition;
}

TEST(PathTest, FindPathToSelf)
{
	CheckPath({ 8, 8 }, { 8, 8 }, {});
}

TEST(PathTest, FindPathTwoStepsUp)
{
	CheckPath({ 8, 8 }, { 8, 6 }, { "↑", "↑" });
}

TEST(PathTest, FindPathTwoStepsLeft)
{
	CheckPath({ 8, 8 }, { 6, 8 }, { "←", "←" });
}

TEST(PathTest, FindPathTwoStepsRight)
{
	CheckPath({ 8, 8 }, { 10, 8 }, { "→", "→" });
}

TEST(PathTest, FindPathTwoStepsDown)
{
	CheckPath({ 8, 8 }, { 8, 10 }, { "↓", "↓" });
}

TEST(PathTest, FindPathDiagonalsFirst3Left2Up)
{
	// Pathing biases along diagonals and the diagonal steps will always be first
	CheckPath({ 8, 8 }, { 5, 6 }, { "↖", "↖", "←" });
}

TEST(PathTest, FindPathDiagonalsFirst4Left4Up)
{
	CheckPath({ 8, 8 }, { 4, 4 }, { "↖", "↖", "↖", "↖" });
}

TEST(PathTest, FindPathDiagonalsFirst2Right4Down)
{
	CheckPath({ 8, 8 }, { 10, 12 }, { "↘", "↘", "↓", "↓" });
}

TEST(PathTest, FindPathDiagonalsFirst4Right12Down)
{
	CheckPath({ 8, 8 }, { 12, 20 }, { "↘", "↘", "↘", "↘", "↓", "↓", "↓", "↓", "↓", "↓", "↓", "↓" });
}

TEST(PathTest, LongPaths)
{
	// Starting from the middle of the world and trying to path to a border exceeds the maximum path size
	CheckPath({ 56, 56 }, { 0, 0 }, {});

	// Longest possible path is currently 24 steps meaning tiles 24 units away are reachable
	Point startingPosition { 56, 56 };
	CheckPath(startingPosition, startingPosition + Displacement { 24, 24 }, std::vector<std::string>(24, "↘"));

	// But trying to navigate 25 units fails
	CheckPath(startingPosition, startingPosition + Displacement { 25, 25 }, {});
}

TEST(PathTest, Walkable)
{
	dPiece[5][5] = 0;
	SOLData[0] = TileProperties::Solid; // Doing this manually to save running through the code in gendung.cpp
	EXPECT_FALSE(IsTileWalkable({ 5, 5 })) << "Tile which is marked as solid should be considered blocked";
	EXPECT_FALSE(IsTileWalkable({ 5, 5 }, true)) << "Solid non-door tiles remain unwalkable when ignoring doors";

	SOLData[0] = TileProperties::None;
	EXPECT_TRUE(IsTileWalkable({ 5, 5 })) << "Non-solid tiles are walkable";
	EXPECT_TRUE(IsTileWalkable({ 5, 5 }, true)) << "Non-solid tiles remain walkable when ignoring doors";

	dObject[5][5] = 1;
	Objects[0]._oSolidFlag = true;
	EXPECT_FALSE(IsTileWalkable({ 5, 5 })) << "Tile occupied by a solid object is unwalkable";
	EXPECT_FALSE(IsTileWalkable({ 5, 5 }, true)) << "Tile occupied by a solid non-door object are unwalkable when ignoring doors";

	Objects[0]._otype = _object_id::OBJ_L1LDOOR;
	EXPECT_FALSE(IsTileWalkable({ 5, 5 })) << "Tile occupied by a door which is marked as solid should be considered blocked";
	EXPECT_TRUE(IsTileWalkable({ 5, 5 }, true)) << "Tile occupied by a door is considered walkable when ignoring doors";

	Objects[0]._oSolidFlag = false;
	EXPECT_TRUE(IsTileWalkable({ 5, 5 })) << "Tile occupied by an open door is walkable";
	EXPECT_TRUE(IsTileWalkable({ 5, 5 }, true)) << "Tile occupied by a door is considered walkable when ignoring doors";

	SOLData[0] = TileProperties::Solid;
	EXPECT_FALSE(IsTileWalkable({ 5, 5 })) << "Solid tiles occupied by an open door remain unwalkable";
	EXPECT_TRUE(IsTileWalkable({ 5, 5 }, true)) << "Solid tiles occupied by an open door become walkable when ignoring doors";
}

TEST(PathTest, FindClosest)
{
	{
		std::array<std::array<int, 101>, 101> searchedTiles {};

		std::optional<Point> nearPosition = FindClosestValidPosition(
		    [&searchedTiles](Point testPosition) {
			    searchedTiles[testPosition.x][testPosition.y]++;
			    return false;
		    },
		    { 50, 50 }, 0, 50);

		EXPECT_FALSE(nearPosition) << "Searching with no valid tiles should return an empty optional";

		for (size_t x = 0; x < searchedTiles.size(); x++) {
			for (size_t y = 0; y < searchedTiles[x].size(); y++) {
				if ((x == 0 || x == 100) && (y == 0 || y == 100)) {
					EXPECT_EQ(searchedTiles[x][y], 0) << "Extreme corners should be skipped due to the inset/rounded search space";
				} else {
					EXPECT_EQ(searchedTiles[x][y], 1) << "Position " << x << " " << y << " should have been searched exactly once";
				}
			}
		}
	}
	{
		std::array<std::array<int, 5>, 5> searchedTiles {};

		std::optional<Point> nearPosition = FindClosestValidPosition(
		    [&searchedTiles](Point testPosition) {
			    searchedTiles[testPosition.x][testPosition.y]++;
			    return false;
		    },
		    { 2, 2 }, 1, 2);

		EXPECT_FALSE(nearPosition) << "Still shouldn't find a valid position with no valid tiles";

		for (size_t x = 0; x < searchedTiles.size(); x++) {
			for (size_t y = 0; y < searchedTiles[x].size(); y++) {
				if (x == 2 && y == 2) {
					EXPECT_EQ(searchedTiles[x][y], 0) << "The starting tile should be skipped with a min radius of 1";
				} else if ((x == 0 || x == 4) && (y == 0 || y == 4)) {
					EXPECT_EQ(searchedTiles[x][y], 0) << "Corners should be skipped";
				} else {
					EXPECT_EQ(searchedTiles[x][y], 1) << "All tiles in range should be searched exactly once";
				}
			}
		}
	}
	{
		std::array<std::array<int, 3>, 3> searchedTiles {};

		std::optional<Point> nearPosition = FindClosestValidPosition(
		    [&searchedTiles](Point testPosition) {
			    searchedTiles[testPosition.x][testPosition.y]++;
			    return false;
		    },
		    { 1, 1 }, 0, 0);

		EXPECT_FALSE(nearPosition) << "Searching with no valid tiles should return an empty optional";

		for (size_t x = 0; x < searchedTiles.size(); x++) {
			for (size_t y = 0; y < searchedTiles[x].size(); y++) {
				if (x == 1 && y == 1) {
					EXPECT_EQ(searchedTiles[x][y], 1) << "Only the starting tile should be searched with max radius 0";
				} else {
					EXPECT_EQ(searchedTiles[x][y], 0) << "Position " << x << " " << y << " should not have been searched";
				}
			}
		}
	}

	{
		std::array<std::array<int, 7>, 7> searchedTiles {};

		std::optional<Point> nearPosition = FindClosestValidPosition(
		    [&searchedTiles](Point testPosition) {
			    searchedTiles[testPosition.x][testPosition.y]++;
			    return false;
		    },
		    { 3, 3 }, 3, 3);

		EXPECT_FALSE(nearPosition) << "Searching with no valid tiles should return an empty optional";

		for (size_t x = 0; x < searchedTiles.size(); x++) {
			for (size_t y = 0; y < searchedTiles[x].size(); y++) {
				if (((x == 1 || x == 5) && (y == 1 || y == 5))  // inset corners
				    || ((x == 0 || x == 6) && y != 0 && y != 6) // left/right sides
				    || (x != 0 && x != 6 && (y == 0 || y == 6)) // top/bottom sides
				) {
					EXPECT_EQ(searchedTiles[x][y], 1) << "Searching with a fixed radius should make a square with inset corners";
				} else {
					EXPECT_EQ(searchedTiles[x][y], 0) << "Position " << x << " " << y << " should not have been searched";
				}
			}
		}
	}
	{
		std::optional<Point> nearPosition = FindClosestValidPosition(
		    [](Point testPosition) {
			    return true;
		    },
		    { 50, 50 }, 21, 50);

		EXPECT_EQ(*nearPosition, (Point { 50, 50 } + Displacement { 0, 21 })) << "First candidate position with a minimum radius should be at {0, +y}";
	}
}

} // namespace
} // namespace devilution
