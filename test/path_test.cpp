#include <gtest/gtest.h>

#include "path.h"

// The following headers are included to access globals used in functions that have not been isolated yet.
#include "gendung.h"
#include "objects.h"

namespace devilution {

extern int TestPathGetHeuristicCost(Point startPosition, Point destinationPosition);

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
	nSolidTable[0] = true;
	EXPECT_TRUE(IsTileSolid({ 5, 5 })) << "Solid in-bounds tiles are solid";
	EXPECT_FALSE(IsTileNotSolid({ 5, 5 })) << "IsTileNotSolid returns the inverse of IsTileSolid for in-bounds tiles";

	dPiece[6][6] = 1;
	nSolidTable[1] = false;
	EXPECT_FALSE(IsTileSolid({ 6, 6 })) << "Non-solid in-bounds tiles are not solid";
	EXPECT_TRUE(IsTileNotSolid({ 6, 6 })) << "IsTileNotSolid returns the inverse of IsTileSolid for in-bounds tiles";

	EXPECT_FALSE(IsTileSolid({ -1, 1 })) << "Out of bounds tiles are not solid"; // this reads out of bounds in the current code and may fail unexpectedly
	EXPECT_FALSE(IsTileNotSolid({ -1, 1 })) << "Out of bounds tiles are also not not solid";
}

TEST(PathTest, SolidPieces)
{
	dPiece[0][0] = 0;
	dPiece[0][1] = 0;
	dPiece[1][0] = 0;
	dPiece[1][1] = 0;
	nSolidTable[0] = false;
	EXPECT_TRUE(path_solid_pieces({ 0, 0 }, { 1, 1 })) << "A step in open space is free of solid pieces";
	EXPECT_TRUE(path_solid_pieces({ 1, 1 }, { 0, 0 })) << "A step in open space is free of solid pieces";
	EXPECT_TRUE(path_solid_pieces({ 1, 0 }, { 0, 1 })) << "A step in open space is free of solid pieces";
	EXPECT_TRUE(path_solid_pieces({ 0, 1 }, { 1, 0 })) << "A step in open space is free of solid pieces";

	nSolidTable[1] = true;
	dPiece[1][0] = 1;
	EXPECT_TRUE(path_solid_pieces({ 0, 1 }, { 1, 0 })) << "Can path to a destination which is solid";
	EXPECT_TRUE(path_solid_pieces({ 1, 0 }, { 0, 1 })) << "Can path from a starting position which is solid";
	EXPECT_TRUE(path_solid_pieces({ 0, 1 }, { 1, 1 })) << "Stepping in a cardinal direction ignores solid pieces";
	EXPECT_TRUE(path_solid_pieces({ 1, 0 }, { 1, 1 })) << "Stepping in a cardinal direction ignores solid pieces";
	EXPECT_TRUE(path_solid_pieces({ 0, 0 }, { 1, 0 })) << "Stepping in a cardinal direction ignores solid pieces";
	EXPECT_TRUE(path_solid_pieces({ 1, 1 }, { 1, 0 })) << "Stepping in a cardinal direction ignores solid pieces";

	EXPECT_FALSE(path_solid_pieces({ 0, 0 }, { 1, 1 })) << "Can't cut a solid corner";
	EXPECT_FALSE(path_solid_pieces({ 1, 1 }, { 0, 0 })) << "Can't cut a solid corner";
	dPiece[0][1] = 1;
	EXPECT_FALSE(path_solid_pieces({ 0, 0 }, { 1, 1 })) << "Can't walk through the boundary between two corners";
	EXPECT_FALSE(path_solid_pieces({ 1, 1 }, { 0, 0 })) << "Can't walk through the boundary between two corners";
	dPiece[1][0] = 0;
	EXPECT_FALSE(path_solid_pieces({ 0, 0 }, { 1, 1 })) << "Can't cut a solid corner";
	EXPECT_FALSE(path_solid_pieces({ 1, 1 }, { 0, 0 })) << "Can't cut a solid corner";
	dPiece[0][1] = 0;

	dPiece[0][0] = 1;
	EXPECT_FALSE(path_solid_pieces({ 1, 0 }, { 0, 1 })) << "Can't cut a solid corner";
	EXPECT_FALSE(path_solid_pieces({ 0, 1 }, { 1, 0 })) << "Can't cut a solid corner";
	dPiece[1][1] = 1;
	EXPECT_FALSE(path_solid_pieces({ 1, 0 }, { 0, 1 })) << "Can't walk through the boundary between two corners";
	EXPECT_FALSE(path_solid_pieces({ 0, 1 }, { 1, 0 })) << "Can't walk through the boundary between two corners";
	dPiece[0][0] = 0;
	EXPECT_FALSE(path_solid_pieces({ 1, 0 }, { 0, 1 })) << "Can't cut a solid corner";
	EXPECT_FALSE(path_solid_pieces({ 0, 1 }, { 1, 0 })) << "Can't cut a solid corner";
	dPiece[1][1] = 0;
}

void CheckPath(Point startPosition, Point destinationPosition, std::vector<int8_t> expectedSteps)
{
	static int8_t pathSteps[MAX_PATH_LENGTH];
	auto pathLength = FindPath([](Point) { return true; }, startPosition, destinationPosition, pathSteps);

	EXPECT_EQ(pathLength, expectedSteps.size()) << "Wrong path length for a path from " << startPosition << " to " << destinationPosition;
	// Die early if the wrong path length is returned as we don't want to read oob in expectedSteps
	ASSERT_LE(pathLength, expectedSteps.size()) << "Path is longer than expected.";

	for (auto i = 0; i < pathLength; i++) {
		EXPECT_EQ(pathSteps[i], expectedSteps[i]) << "Path step " << i << " differs from expectation for a path from "
		                                          << startPosition << " to " << destinationPosition; // this shouldn't be a requirement but...

		// Path directions are all jacked up compared to the Direction enum. Most consumers have their own mapping definition
		// startPosition += Direction { path[i] - 1 };
	}
	// Given that we can't really make any assumptions about how the path is actually used.
	// EXPECT_EQ(startPosition, destinationPosition) << "Path doesn't lead to destination";
}

TEST(PathTest, FindPath)
{
	CheckPath({ 8, 8 }, { 8, 8 }, {});

	// Traveling in cardinal directions is the only way to get a first step in a cardinal direction
	CheckPath({ 8, 8 }, { 8, 6 }, { 1, 1 });
	CheckPath({ 8, 8 }, { 6, 8 }, { 2, 2 });
	CheckPath({ 8, 8 }, { 10, 8 }, { 3, 3 });
	CheckPath({ 8, 8 }, { 8, 10 }, { 4, 4 });

	// Otherwise pathing biases along diagonals and the diagonal steps will always be first
	CheckPath({ 8, 8 }, { 5, 6 }, { 5, 5, 2 });
	CheckPath({ 8, 8 }, { 4, 4 }, { 5, 5, 5, 5 });
	CheckPath({ 8, 8 }, { 12, 20 }, { 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 4 });
}

TEST(PathTest, Walkable)
{
	dPiece[5][5] = 0;
	nSolidTable[0] = true; // Doing this manually to save running through the code in gendung.cpp
	EXPECT_FALSE(IsTileWalkable({ 5, 5 })) << "Tile which is marked as solid should be considered blocked";
	EXPECT_FALSE(IsTileWalkable({ 5, 5 }, true)) << "Solid non-door tiles remain unwalkable when ignoring doors";

	nSolidTable[0] = false;
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

	nSolidTable[0] = true;
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

		for (int x = 0; x < searchedTiles.size(); x++) {
			for (int y = 0; y < searchedTiles[x].size(); y++) {
				if (IsAnyOf(x, 0, 100) && IsAnyOf(y, 0, 100)) {
					EXPECT_EQ(searchedTiles[x][y], 0) << "Extreme corners should be skipped due to the inset/rounded search space";
				} else {
					EXPECT_EQ(searchedTiles[x][y], 1) << "Position " << Point { x, y } << " should have been searched exactly once";
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

		for (int x = 0; x < searchedTiles.size(); x++) {
			for (int y = 0; y < searchedTiles[x].size(); y++) {
				if (Point { x, y } == Point { 2, 2 }) {
					EXPECT_EQ(searchedTiles[x][y], 0) << "The starting tile should be skipped with a min radius of 1";
				} else if (IsAnyOf(x, 0, 4) && IsAnyOf(y, 0, 4)) {
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

		for (int x = 0; x < searchedTiles.size(); x++) {
			for (int y = 0; y < searchedTiles[x].size(); y++) {
				if (Point { x, y } == Point { 1, 1 }) {
					EXPECT_EQ(searchedTiles[x][y], 1) << "Only the starting tile should be searched with max radius 0";
				} else {
					EXPECT_EQ(searchedTiles[x][y], 0) << "Position " << Point { x, y } << " should not have been searched";
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

		for (int x = 0; x < searchedTiles.size(); x++) {
			for (int y = 0; y < searchedTiles[x].size(); y++) {
				if ((IsAnyOf(x, 1, 5) && IsAnyOf(y, 1, 5))     // inset corners
				    || (IsAnyOf(x, 0, 6) && IsNoneOf(y, 0, 6)) // left/right sides
				    || (IsNoneOf(x, 0, 6) && IsAnyOf(y, 0, 6)) // top/bottom sides
				) {
					EXPECT_EQ(searchedTiles[x][y], 1) << "Searching with a fixed radius should make a square with inset corners";
				} else {
					EXPECT_EQ(searchedTiles[x][y], 0) << "Position " << Point { x, y } << " should not have been searched";
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
} // namespace devilution
