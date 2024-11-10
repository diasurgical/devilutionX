/**
 * @file path.h
 *
 * Interface of the path finding algorithms.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include <function_ref.hpp>

#include "engine/displacement.hpp"
#include "engine/point.hpp"

namespace devilution {

constexpr size_t MaxPathLengthMonsters = 25;
constexpr size_t MaxPathLengthPlayer = 100;

// Cost for an axis-aligned step (up/down/left/right). Visible for testing.
extern const int PathAxisAlignedStepCost;

// Cost for a diagonal step. Visible for testing.
extern const int PathDiagonalStepCost;

/**
 * @brief Find the shortest path from `startPosition` to `destinationPosition`.
 *
 * @param canStep specifies whether a step between two adjacent points is allowed.
 * @param posOk specifies whether a position can be stepped on.
 * @param startPosition
 * @param destinationPosition
 * @param path Resulting path represented as the step directions, which are indices in `PathDirs`. Must have room for `maxPathLength` steps.
 * @param maxPathLength The maximum allowed length of the resulting path.
 * @return The length of the resulting path, or 0 if there is no valid path.
 */
int FindPath(tl::function_ref<bool(Point, Point)> canStep, tl::function_ref<bool(Point)> posOk, Point startPosition, Point destinationPosition, int8_t *path, size_t maxPathLength);

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
 * Returns a number representing the direction from a starting tile to a neighbouring tile.
 *
 * Used in the pathfinding code, each step direction is assigned a number like this:
 *       dx
 *     -1 0 1
 *     +-----
 *   -1|5 1 6
 * dy 0|2 0 3
 *    1|8 4 7
 */
[[nodiscard]] int8_t GetPathDirection(Point startPosition, Point destinationPosition);

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
std::optional<Point> FindClosestValidPosition(tl::function_ref<bool(Point)> posOk, Point startingPosition, unsigned int minimumRadius = 0, unsigned int maximumRadius = 18);

} // namespace devilution
