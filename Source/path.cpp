/**
 * @file path.cpp
 *
 * Implementation of the path finding algorithms.
 */
#include "path.h"

#include "gendung.h"
#include "objects.h"

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

bool IsTileNotSolid(Point position)
{
	return !nSolidTable[dPiece[position.x][position.y]];
}

bool IsTileSolid(Point position)
{
	if (position.x < 0 || position.y < 0 || position.x >= MAXDUNX || position.y >= MAXDUNY) {
		return false;
	}

	return nSolidTable[dPiece[position.x][position.y]];
}

/**
 * @brief Checks the position is solid or blocked by an object
 */
bool IsTileWalkable(Point position, bool ignoreDoors)
{
	if (dObject[position.x][position.y] != 0) {
		int oi = abs(dObject[position.x][position.y]) - 1;
		if (ignoreDoors && Objects[oi].IsDoor())
			return true;
		if (Objects[oi]._oSolidFlag)
			return false;
	}

	return !IsTileSolid(position);
}

/**
 * find the shortest path from (sx,sy) to (dx,dy), using PosOk(PosOkArg,x,y) to
 * check that each step is a valid position. Store the step directions (see
 * path_directions) in path, which must have room for 24 steps
 */
int FindPath(const std::function<bool(Point)> &posOk, Point start, Point destination, int8_t path[MAX_PATH_LENGTH])
{
	// clear all nodes, create root nodes for the visited/frontier linked lists
	gdwCurNodes = 0;
	path_2_nodes = path_new_step();
	pnode_ptr = path_new_step();
	gdwCurPathStep = 0;
	PATHNODE *pathStart = path_new_step();
	pathStart->g = 0;
	pathStart->h = path_get_h_cost(start, destination);
	pathStart->f = pathStart->h + pathStart->g;
	pathStart->position = start;
	path_2_nodes->NextNode = pathStart;
	// A* search until we find (dx,dy) or fail
	PATHNODE *nextNode;
	while ((nextNode = GetNextPath()) != nullptr) {
		// reached the end, success!
		if (nextNode->position == destination) {
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
		if (!path_get_path(posOk, nextNode, destination))
			return 0;
	}
	// frontier is empty, no path!
	return 0;
}

/**
 * @brief heuristic, estimated cost from (sx,sy) to (dx,dy)
 */
int path_get_h_cost(Point source, Point destination)
{
	// see path_check_equal for why this is times 2
	return 2 * source.ManhattanDistance(destination);
}

/**
 * @brief return 2 if pPath is horizontally/vertically aligned with (dx,dy), else 3
 *
 * This approximates that diagonal movement on a square grid should have a cost
 * of sqrt(2). That's approximately 1.5, so they multiply all step costs by 2,
 * except diagonal steps which are times 3
 */
int path_check_equal(Point position, Point destination)
{
	if (position.x == destination.x || position.y == destination.y)
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
 * @brief check if stepping from a given position to a neighbouring tile cuts a corner.
 *
 * If you step from A to B, both Xs need to be clear:
 *
 *  AX
 *  XB
 *
 *  @return true if step is allowed
 */
bool path_solid_pieces(Point position, Point destination)
{
	// These checks are written as if working backwards from the destination to the source, given
	// both tiles are expected to be adjacent this doesn't matter beyond being a bit confusing
	bool rv = true;
	switch (path_directions[3 * (destination.y - position.y) + 3 - position.x + 1 + destination.x]) {
	case 5: // Stepping north
		rv = IsTileNotSolid(destination + DIR_SW) && IsTileNotSolid(destination + DIR_SE);
		break;
	case 6: // Stepping east
		rv = IsTileNotSolid(destination + DIR_SW) && IsTileNotSolid(destination + DIR_NW);
		break;
	case 7: // Stepping south
		rv = IsTileNotSolid(destination + DIR_NE) && IsTileNotSolid(destination + DIR_NW);
		break;
	case 8: // Stepping west
		rv = IsTileNotSolid(destination + DIR_SE) && IsTileNotSolid(destination + DIR_NE);
		break;
	}
	return rv;
}

/**
 * @brief perform a single step of A* bread-first search by trying to step in every possible direction from pPath with goal (x,y). Check each step with PosOk
 *
 * @return false if we ran out of preallocated nodes to use, else true
 */
bool path_get_path(const std::function<bool(Point)> &posOk, PATHNODE *pPath, Point destination)
{
	for (auto dir : PathDirs) {
		Point tile = pPath->position + dir;
		bool ok = posOk(tile);
		if ((ok && path_solid_pieces(pPath->position, tile)) || (!ok && tile == destination)) {
			if (!path_parent_path(pPath, tile, destination))
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
bool path_parent_path(PATHNODE *pPath, Point candidatePosition, Point destinationPosition)
{
	int nextG = pPath->g + path_check_equal(pPath->position, candidatePosition);

	// 3 cases to consider
	// case 1: (dx,dy) is already on the frontier
	PATHNODE *dxdy = path_get_node1(candidatePosition);
	if (dxdy != nullptr) {
		int i;
		for (i = 0; i < 8; i++) {
			if (pPath->Child[i] == nullptr)
				break;
		}
		pPath->Child[i] = dxdy;
		if (nextG < dxdy->g) {
			if (path_solid_pieces(pPath->position, candidatePosition)) {
				// we'll explore it later, just update
				dxdy->Parent = pPath;
				dxdy->g = nextG;
				dxdy->f = nextG + dxdy->h;
			}
		}
	} else {
		// case 2: (dx,dy) was already visited
		dxdy = path_get_node2(candidatePosition);
		if (dxdy != nullptr) {
			int i;
			for (i = 0; i < 8; i++) {
				if (pPath->Child[i] == nullptr)
					break;
			}
			pPath->Child[i] = dxdy;
			if (nextG < dxdy->g && path_solid_pieces(pPath->position, candidatePosition)) {
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
			dxdy->h = path_get_h_cost(candidatePosition, destinationPosition);
			dxdy->f = nextG + dxdy->h;
			dxdy->position = candidatePosition;
			// add it to the frontier
			path_next_node(dxdy);

			int i;
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
 * @brief return a node for a position on the frontier, or NULL if not found
 */
PATHNODE *path_get_node1(Point targetPosition)
{
	PATHNODE *result = path_2_nodes->NextNode;
	while (result != nullptr) {
		if (result->position == targetPosition)
			return result;
		result = result->NextNode;
	}
	return nullptr;
}

/**
 * @brief return a node for a given position if it was visited, or NULL if not found
 */
PATHNODE *path_get_node2(Point targetPosition)
{
	PATHNODE *result = pnode_ptr->NextNode;
	while (result != nullptr) {
		if (result->position == targetPosition)
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
	path_push_active_step(pPath);
	// while there are path nodes to check
	while (gdwCurPathStep > 0) {
		PATHNODE *pathOld = path_pop_active_step();
		for (auto *pathAct : pathOld->Child) {
			if (pathAct == nullptr)
				break;

			if (pathOld->g + path_check_equal(pathOld->position, pathAct->position) < pathAct->g) {
				if (path_solid_pieces(pathOld->position, pathAct->position)) {
					pathAct->Parent = pathOld;
					pathAct->g = pathOld->g + path_check_equal(pathOld->position, pathAct->position);
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
	assert(gdwCurPathStep < MAXPATHNODES);
	pnode_tblptr[gdwCurPathStep] = pPath;
	gdwCurPathStep++;
}

/**
 * @brief pop and return a node from the pnode_tblptr stack
 */
PATHNODE *path_pop_active_step()
{
	gdwCurPathStep--;
	assert(gdwCurPathStep >= 0);
	return pnode_tblptr[gdwCurPathStep];
}

/**
 * @brief zero one of the preallocated nodes and return a pointer to it, or NULL if none are available
 */
PATHNODE *path_new_step()
{
	if (gdwCurNodes >= MAXPATHNODES)
		return nullptr;

	PATHNODE *newNode = &path_nodes[gdwCurNodes];
	gdwCurNodes++;
	memset(newNode, 0, sizeof(PATHNODE));
	return newNode;
}

} // namespace devilution
