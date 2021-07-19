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
	{ -1, -1 }, //DIR_N
	{ -1,  1 }, //DIR_W
	{  1, -1 }, //DIR_E
	{  1,  1 }, //DIR_S
	{ -1,  0 }, //DIR_NW
	{  0, -1 }, //DIR_NE
	{  1,  0 }, //DIR_SE
	{  0,  1 }, //DIR_SW
	// clang-format on
};

} // namespace devilution
