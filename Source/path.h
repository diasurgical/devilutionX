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
bool IsTileWalkable(Point position, bool ignoreDoors = false);
int FindPath(const std::function<bool(Point)> &posOk, Point start, Point destination, int8_t path[MAX_PATH_LENGTH]);
int path_get_h_cost(Point source, Point destination);
PATHNODE *GetNextPath();
bool path_solid_pieces(Point position, Point destination);
bool path_get_path(const std::function<bool(Point)> &posOk, PATHNODE *pPath, Point destination);
bool path_parent_path(PATHNODE *pPath, Point candidatePosition, Point destinationPosition);
PATHNODE *path_get_node1(Point);
PATHNODE *path_get_node2(Point);
void path_next_node(PATHNODE *pPath);
void path_set_coords(PATHNODE *pPath);
void path_push_active_step(PATHNODE *pPath);
PATHNODE *path_pop_active_step();
PATHNODE *path_new_step();

/* rdata */

extern const Displacement PathDirs[8];

} // namespace devilution
