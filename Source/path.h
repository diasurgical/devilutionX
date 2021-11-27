/**
 * @file path.h
 *
 * Interface of the path finding algorithms.
 */
#pragma once

#include <functional>

#include <SDL.h>

#include "engine/direction.hpp"
#include "engine/point.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

#define MAX_PATH_LENGTH 25

struct PATHNODE {
	uint8_t f;
	uint8_t h;
	uint8_t g;
	Point position;
	struct PATHNODE *Parent;
	struct PATHNODE *Child[8];
	struct PATHNODE *NextNode;
};

bool IsTileNotSolid(Point position);
bool IsTileSolid(Point position);

/**
 * @brief Checks the position is solid or blocked by an object
 */
bool IsTileWalkable(Point position, bool ignoreDoors = false);

/**
 * @brief Checks if the position contains an object, player, monster, or solid dungeon piece
 */
bool IsTileOccupied(Point position);

/**
 * @brief Find the shortest path from startPosition to destinationPosition, using PosOk(Point) to check that each step is a valid position.
 * Store the step directions (corresponds to an index in PathDirs) in path, which must have room for 24 steps
 */
int FindPath(const std::function<bool(Point)> &posOk, Point startPosition, Point destinationPosition, int8_t path[MAX_PATH_LENGTH]);

/**
 * @brief check if stepping from a given position to a neighbouring tile cuts a corner.
 *
 * If you step from A to B, both Xs need to be clear:
 *
 *  AX
 *  XB
 *
 * @return true if step is allowed
 */
bool path_solid_pieces(Point startPosition, Point destinationPosition);

/** For iterating over the 8 possible movement directions */
const Displacement PathDirs[8] = {
	// clang-format off
	{ -1, -1 }, //Direction::North
	{ -1,  1 }, //Direction::West
	{  1, -1 }, //Direction::East
	{  1,  1 }, //Direction::South
	{ -1,  0 }, //Direction::NorthWest
	{  0, -1 }, //Direction::NorthEast
	{  1,  0 }, //Direction::SouthEast
	{  0,  1 }, //Direction::SouthWest
	// clang-format on
};

/**
 * @brief Searches for the closest position that passes the check in expanding "rings".
 *
 * The search space is roughly equivalent to a square of tiles where the walking distance is equal to the radius except
 * the corners are "rounded" (inset) by one tile. For example the following is a search space of radius 4:
 * _XXXXXXX_
 * XX_____XX
 * X_______X
 * < snip  >
 * X_______X
 * XX_____XX
 * _XXXXXXX_
 *
 * @param posOk Used to check if a position is valid
 * @param startingPosition dungeon tile location to start the search from
 * @param minimumRadius A value from 0 to 50, allows skipping nearby tiles (e.g. specify radius 1 to skip checking the starting tile)
 * @param maximumRadius The maximum distance to check, defaults to 18 for vanilla compatibility but supports values up to 50
 * @return either the closest valid point or an empty optional
 */
std::optional<Point> FindClosestValidPosition(const std::function<bool(Point)> &posOk, Point startingPosition, unsigned int minimumRadius = 0, unsigned int maximumRadius = 18);

} // namespace devilution
