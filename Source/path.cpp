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
const Displacement PathDirs[8] = {
	// clang-format off
	{ -1, -1 },
	{ -1,  1 },
	{  1, -1 },
	{  1,  1 },
	{ -1,  0 },
	{  0, -1 },
	{  1,  0 },
	{  0,  1 },
	// clang-format on
};

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
int FindPath(bool (*posOk)(int, Point), int posOkArg, int sx, int sy, int dx, int dy, int8_t path[MAX_PATH_LENGTH])
{
	// clear all nodes, create root nodes for the visited/frontier linked lists
	gdwCurNodes = 0;
	path_2_nodes = path_new_step();
	pnode_ptr = path_new_step();
	gdwCurPathStep = 0;
	PATHNODE *pathStart = path_new_step();
	pathStart->g = 0;
	pathStart->h = path_get_h_cost(sx, sy, dx, dy);
	pathStart->position.x = sx;
	pathStart->f = pathStart->h + pathStart->g;
	pathStart->position.y = sy;
	path_2_nodes->NextNode = pathStart;
	// A* search until we find (dx,dy) or fail
	PATHNODE *nextNode;
	while ((nextNode = GetNextPath()) != nullptr) {
		// reached the end, success!
		if (nextNode->position.x == dx && nextNode->position.y == dy) {
			PATHNODE *current = nextNode;
			int pathLength = 0;
			while (current->Parent != nullptr) {
				if (pathLength >= MAX_PATH_LENGTH)
					break;
				pnode_vals[pathLength++] = path_directions[3 * (current->position.y - current->Parent->position.y) - current->Parent->position.x + 4 + current->position.x];
				current = current->Parent;
			}
			if (pathLength != MAX_PATH_LENGTH) {
				int i;
				for (i = 0; i < pathLength; i++)
					path[i] = pnode_vals[pathLength - i - 1];
				return i;
			}
			return 0;
		}
		// ran out of nodes, abort!
		if (!path_get_path(posOk, posOkArg, nextNode, dx, dy))
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
	int deltaX = abs(sx - dx);
	int deltaY = abs(sy - dy);

	int min = deltaX < deltaY ? deltaX : deltaY;
	int max = deltaX > deltaY ? deltaX : deltaY;

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
bool path_get_path(bool (*posOk)(int, Point), int posOkArg, PATHNODE *pPath, int x, int y)
{
	for (auto dir : PathDirs) {
		Point tile = pPath->position + dir;
		bool ok = posOk(posOkArg, tile);
		if ((ok && path_solid_pieces(pPath, tile.x, tile.y)) || (!ok && tile == Point { x, y })) {
			if (!path_parent_path(pPath, tile.x, tile.y, x, y))
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
	int nextG;
	PATHNODE *dxdy;
	int i;

	nextG = pPath->g + path_check_equal(pPath, dx, dy);

	// 3 cases to consider
	// case 1: (dx,dy) is already on the frontier
	dxdy = path_get_node1(dx, dy);
	if (dxdy != nullptr) {
		for (i = 0; i < 8; i++) {
			if (pPath->Child[i] == nullptr)
				break;
		}
		pPath->Child[i] = dxdy;
		if (nextG < dxdy->g) {
			if (path_solid_pieces(pPath, dx, dy)) {
				// we'll explore it later, just update
				dxdy->Parent = pPath;
				dxdy->g = nextG;
				dxdy->f = nextG + dxdy->h;
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
			if (nextG < dxdy->g && path_solid_pieces(pPath, dx, dy)) {
				// update the node
				dxdy->Parent = pPath;
				dxdy->g = nextG;
				dxdy->f = nextG + dxdy->h;
				// already explored, so re-update others starting from that node
				path_set_coords(dxdy);
			}
		} else {
			// case 3: (dx,dy) is totally new
			dxdy = path_new_step();
			if (dxdy == nullptr)
				return false;
			dxdy->Parent = pPath;
			dxdy->g = nextG;
			dxdy->h = path_get_h_cost(dx, dy, sx, sy);
			dxdy->f = nextG + dxdy->h;
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
	if (path_2_nodes->NextNode == nullptr) {
		path_2_nodes->NextNode = pPath;
		return;
	}

	PATHNODE *current = path_2_nodes;
	PATHNODE *next = path_2_nodes->NextNode;
	int f = pPath->f;
	while (next != nullptr && next->f < f) {
		current = next;
		next = next->NextNode;
	}
	pPath->NextNode = next;
	current->NextNode = pPath;
}

/**
 * @brief update all path costs using depth-first search starting at pPath
 */
void path_set_coords(PATHNODE *pPath)
{
	PATHNODE *pathOld;
	PATHNODE *pathAct;
	int i;

	path_push_active_step(pPath);
	// while there are path nodes to check
	while (gdwCurPathStep > 0) {
		pathOld = path_pop_active_step();
		for (i = 0; i < 8; i++) {
			pathAct = pathOld->Child[i];
			if (pathAct == nullptr)
				break;

			if (pathOld->g + path_check_equal(pathOld, pathAct->position.x, pathAct->position.y) < pathAct->g) {
				if (path_solid_pieces(pathOld, pathAct->position.x, pathAct->position.y)) {
					pathAct->Parent = pathOld;
					pathAct->g = pathOld->g + path_check_equal(pathOld, pathAct->position.x, pathAct->position.y);
					pathAct->f = pathAct->g + pathAct->h;
					path_push_active_step(pathAct);
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
	int stackIndex = gdwCurPathStep;
	gdwCurPathStep++;
	pnode_tblptr[stackIndex] = pPath;
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
	PATHNODE *newNode;

	if (gdwCurNodes == MAXPATHNODES)
		return nullptr;

	newNode = &path_nodes[gdwCurNodes];
	gdwCurNodes++;
	memset(newNode, 0, sizeof(PATHNODE));
	return newNode;
}

} // namespace devilution
