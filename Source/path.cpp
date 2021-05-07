/**
 * @file path.cpp
 *
 * Implementation of the path finding algorithms.
 */
#include "path.h"

#include "gendung.h"

namespace devilution {

#define MAXPATHNODES 300

/** Notes visisted by the path finding algorithm. */
PATHNODE path_nodes[MAXPATHNODES];
/** size of the pnode_tblptr stack */
int gdwCurPathStep;
/** the number of in-use nodes in path_nodes */
int gdwCurNodes;
/**
 * for reconstructing the path after the A* search is done. The longest
 * possible path is actually 24 steps, even though we can fit 25
 */
int8_t pnode_vals[MAX_PATH_LENGTH];
/** A linked list of all visited nodes */
PATHNODE *pnode_ptr;
/** A stack for recursively searching nodes */
PATHNODE *pnode_tblptr[MAXPATHNODES];
/** A linked list of the A* frontier, sorted by distance */
PATHNODE *path_2_nodes;

/** For iterating over the 8 possible movement directions */
const char pathxdir[8] = { -1, -1, 1, 1, -1, 0, 1, 0 };
const char pathydir[8] = { -1, 1, -1, 1, 0, -1, 0, 1 };

/* data */

/**
 * each step direction is assigned a number like this:
 *       dx
 *     -1 0 1
 *     +-----
 *   -1|5 1 6
 * dy 0|2 0 3
 *    1|8 4 7
 */
int8_t path_directions[9] = { 5, 1, 6, 2, 0, 3, 8, 4, 7 };

/**
 * find the shortest path from (sx,sy) to (dx,dy), using PosOk(PosOkArg,x,y) to
 * check that each step is a valid position. Store the step directions (see
 * path_directions) in path, which must have room for 24 steps
 */
int FindPath(bool (*PosOk)(int, int, int), int PosOkArg, int sx, int sy, int dx, int dy, int8_t path[MAX_PATH_LENGTH])
{
	PATHNODE *path_start, *next_node, *current;
	int path_length, i;

	// clear all nodes, create root nodes for the visited/frontier linked lists
	gdwCurNodes = 0;
	path_2_nodes = path_new_step();
	pnode_ptr = path_new_step();
	gdwCurPathStep = 0;
	path_start = path_new_step();
	path_start->g = 0;
	path_start->h = path_get_h_cost(sx, sy, dx, dy);
	path_start->position.x = sx;
	path_start->f = path_start->h + path_start->g;
	path_start->position.y = sy;
	path_2_nodes->NextNode = path_start;
	// A* search until we find (dx,dy) or fail
	while ((next_node = GetNextPath())) {
		// reached the end, success!
		if (next_node->position.x == dx && next_node->position.y == dy) {
			current = next_node;
			path_length = 0;
			while (current->Parent != nullptr) {
				if (path_length >= MAX_PATH_LENGTH)
					break;
				pnode_vals[path_length++] = path_directions[3 * (current->position.y - current->Parent->position.y) - current->Parent->position.x + 4 + current->position.x];
				current = current->Parent;
			}
			if (path_length != MAX_PATH_LENGTH) {
				for (i = 0; i < path_length; i++)
					path[i] = pnode_vals[path_length - i - 1];
				return i;
			}
			return 0;
		}
		// ran out of nodes, abort!
		if (!path_get_path(PosOk, PosOkArg, next_node, dx, dy))
			return 0;
	}
	// frontier is empty, no path!
	return 0;
}

/**
 * @brief heuristic, estimated cost from (sx,sy) to (dx,dy)
 */
int path_get_h_cost(int sx, int sy, int dx, int dy)
{
	int delta_x = abs(sx - dx);
	int delta_y = abs(sy - dy);

	int min = delta_x < delta_y ? delta_x : delta_y;
	int max = delta_x > delta_y ? delta_x : delta_y;

	// see path_check_equal for why this is times 2
	return 2 * (min + max);
}

/**
 * @brief return 2 if pPath is horizontally/vertically aligned with (dx,dy), else 3
 *
 * This approximates that diagonal movement on a square grid should have a cost
 * of sqrt(2). That's approximately 1.5, so they multiply all step costs by 2,
 * except diagonal steps which are times 3
 */
int path_check_equal(PATHNODE *pPath, int dx, int dy)
{
	if (pPath->position.x == dx || pPath->position.y == dy)
		return 2;

	return 3;
}

/**
 * @brief get the next node on the A* frontier to explore (estimated to be closest to the goal), mark it as visited, and return it
 */
PATHNODE *GetNextPath()
{
	PATHNODE *result;

	result = path_2_nodes->NextNode;
	if (result == nullptr) {
		return result;
	}

	path_2_nodes->NextNode = result->NextNode;
	result->NextNode = pnode_ptr->NextNode;
	pnode_ptr->NextNode = result;
	return result;
}

/**
 * @brief check if stepping from pPath to (dx,dy) cuts a corner.
 *
 * If you step from A to B, both Xs need to be clear:
 *
 *  AX
 *  XB
 *
 *  @return true if step is allowed
 */
bool path_solid_pieces(PATHNODE *pPath, int dx, int dy)
{
	bool rv = true;
	switch (path_directions[3 * (dy - pPath->position.y) + 3 - pPath->position.x + 1 + dx]) {
	case 5:
		rv = !nSolidTable[dPiece[dx][dy + 1]] && !nSolidTable[dPiece[dx + 1][dy]];
		break;
	case 6:
		rv = !nSolidTable[dPiece[dx][dy + 1]] && !nSolidTable[dPiece[dx - 1][dy]];
		break;
	case 7:
		rv = !nSolidTable[dPiece[dx][dy - 1]] && !nSolidTable[dPiece[dx - 1][dy]];
		break;
	case 8:
		rv = !nSolidTable[dPiece[dx + 1][dy]] && !nSolidTable[dPiece[dx][dy - 1]];
		break;
	}
	return rv;
}

/**
 * @brief perform a single step of A* bread-first search by trying to step in every possible direction from pPath with goal (x,y). Check each step with PosOk
 *
 * @return false if we ran out of preallocated nodes to use, else true
 */
bool path_get_path(bool (*PosOk)(int, int, int), int PosOkArg, PATHNODE *pPath, int x, int y)
{
	int dx, dy;
	int i;
	bool ok;

	for (i = 0; i < 8; i++) {
		dx = pPath->position.x + pathxdir[i];
		dy = pPath->position.y + pathydir[i];
		ok = PosOk(PosOkArg, dx, dy);
		if ((ok && path_solid_pieces(pPath, dx, dy)) || (!ok && dx == x && dy == y)) {
			if (!path_parent_path(pPath, dx, dy, x, y))
				return false;
		}
	}

	return true;
}

/**
 * @brief add a step from pPath to (dx,dy), return 1 if successful, and update the frontier/visited nodes accordingly
 *
 * @return true if step successfully added, false if we ran out of nodes to use
 */
bool path_parent_path(PATHNODE *pPath, int dx, int dy, int sx, int sy)
{
	int next_g;
	PATHNODE *dxdy;
	int i;

	next_g = pPath->g + path_check_equal(pPath, dx, dy);

	// 3 cases to consider
	// case 1: (dx,dy) is already on the frontier
	dxdy = path_get_node1(dx, dy);
	if (dxdy != nullptr) {
		for (i = 0; i < 8; i++) {
			if (pPath->Child[i] == nullptr)
				break;
		}
		pPath->Child[i] = dxdy;
		if (next_g < dxdy->g) {
			if (path_solid_pieces(pPath, dx, dy)) {
				// we'll explore it later, just update
				dxdy->Parent = pPath;
				dxdy->g = next_g;
				dxdy->f = next_g + dxdy->h;
			}
		}
	} else {
		// case 2: (dx,dy) was already visited
		dxdy = path_get_node2(dx, dy);
		if (dxdy != nullptr) {
			for (i = 0; i < 8; i++) {
				if (pPath->Child[i] == nullptr)
					break;
			}
			pPath->Child[i] = dxdy;
			if (next_g < dxdy->g && path_solid_pieces(pPath, dx, dy)) {
				// update the node
				dxdy->Parent = pPath;
				dxdy->g = next_g;
				dxdy->f = next_g + dxdy->h;
				// already explored, so re-update others starting from that node
				path_set_coords(dxdy);
			}
		} else {
			// case 3: (dx,dy) is totally new
			dxdy = path_new_step();
			if (dxdy == nullptr)
				return false;
			dxdy->Parent = pPath;
			dxdy->g = next_g;
			dxdy->h = path_get_h_cost(dx, dy, sx, sy);
			dxdy->f = next_g + dxdy->h;
			dxdy->position = { dx, dy };
			// add it to the frontier
			path_next_node(dxdy);

			for (i = 0; i < 8; i++) {
				if (pPath->Child[i] == nullptr)
					break;
			}
			pPath->Child[i] = dxdy;
		}
	}
	return true;
}

/**
 * @brief return a node for (dx,dy) on the frontier, or NULL if not found
 */
PATHNODE *path_get_node1(int dx, int dy)
{
	PATHNODE *result = path_2_nodes->NextNode;
	while (result != nullptr) {
		if (result->position.x == dx && result->position.y == dy)
			return result;
		result = result->NextNode;
	}
	return nullptr;
}

/**
 * @brief return a node for (dx,dy) if it was visited, or NULL if not found
 */
PATHNODE *path_get_node2(int dx, int dy)
{
	PATHNODE *result = pnode_ptr->NextNode;
	while (result != nullptr) {
		if (result->position.x == dx && result->position.y == dy)
			return result;
		result = result->NextNode;
	}
	return nullptr;
}

/**
 * @brief insert pPath into the frontier (keeping the frontier sorted by total distance)
 */
void path_next_node(PATHNODE *pPath)
{
	PATHNODE *next, *current;
	int f;

	next = path_2_nodes;
	if (path_2_nodes->NextNode == nullptr) {
		path_2_nodes->NextNode = pPath;
	} else {
		current = path_2_nodes;
		next = path_2_nodes->NextNode;
		f = pPath->f;
		while (next && next->f < f) {
			current = next;
			next = next->NextNode;
		}
		pPath->NextNode = next;
		current->NextNode = pPath;
	}
}

/**
 * @brief update all path costs using depth-first search starting at pPath
 */
void path_set_coords(PATHNODE *pPath)
{
	PATHNODE *PathOld;
	PATHNODE *PathAct;
	int i;

	path_push_active_step(pPath);
	while (gdwCurPathStep) {
		PathOld = path_pop_active_step();
		for (i = 0; i < 8; i++) {
			PathAct = PathOld->Child[i];
			if (PathAct == nullptr)
				break;

			if (PathOld->g + path_check_equal(PathOld, PathAct->position.x, PathAct->position.y) < PathAct->g) {
				if (path_solid_pieces(PathOld, PathAct->position.x, PathAct->position.y)) {
					PathAct->Parent = PathOld;
					PathAct->g = PathOld->g + path_check_equal(PathOld, PathAct->position.x, PathAct->position.y);
					PathAct->f = PathAct->g + PathAct->h;
					path_push_active_step(PathAct);
				}
			}
		}
	}
}

/**
 * @brief push pPath onto the pnode_tblptr stack
 */
void path_push_active_step(PATHNODE *pPath)
{
	int stack_index = gdwCurPathStep;
	gdwCurPathStep++;
	pnode_tblptr[stack_index] = pPath;
}

/**
 * @brief pop and return a node from the pnode_tblptr stack
 */
PATHNODE *path_pop_active_step()
{
	gdwCurPathStep--;
	return pnode_tblptr[gdwCurPathStep];
}

/**
 * @brief zero one of the preallocated nodes and return a pointer to it, or NULL if none are available
 */
PATHNODE *path_new_step()
{
	PATHNODE *new_node;

	if (gdwCurNodes == MAXPATHNODES)
		return nullptr;

	new_node = &path_nodes[gdwCurNodes];
	gdwCurNodes++;
	memset(new_node, 0, sizeof(PATHNODE));
	return new_node;
}

} // namespace devilution
