/**
 * @file path.h
 *
 * Interface of the path finding algorithms.
 */
#pragma once

#include <SDL.h>

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

int FindPath(bool (*PosOk)(int, Point), int PosOkArg, int sx, int sy, int dx, int dy, int8_t path[MAX_PATH_LENGTH]);
int path_get_h_cost(int sx, int sy, int dx, int dy);
PATHNODE *GetNextPath();
bool path_solid_pieces(PATHNODE *pPath, int dx, int dy);
bool path_get_path(bool (*PosOk)(int, Point), int PosOkArg, PATHNODE *pPath, int x, int y);
bool path_parent_path(PATHNODE *pPath, int dx, int dy, int sx, int sy);
PATHNODE *path_get_node1(int dx, int dy);
PATHNODE *path_get_node2(int dx, int dy);
void path_next_node(PATHNODE *pPath);
void path_set_coords(PATHNODE *pPath);
void path_push_active_step(PATHNODE *pPath);
PATHNODE *path_pop_active_step();
PATHNODE *path_new_step();

/* rdata */

extern const char pathxdir[8];
extern const char pathydir[8];

} // namespace devilution
