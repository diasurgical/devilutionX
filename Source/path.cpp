/**
 * @file path.cpp
 *
 * Implementation of the path finding algorithms.
 */
#include "path.h"

#include <array>

#include "gendung.h"
#include "objects.h"

namespace devilution {

namespace {

/** A linked list of the A* frontier, sorted by distance */
PATHNODE *path_2_nodes;
/**
 * @brief return a node for a position on the frontier, or NULL if not found
 */
PATHNODE *GetNode1(Point targetPosition)
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
 * @brief insert pPath into the frontier (keeping the frontier sorted by total distance)
 */
void NextNode(PATHNODE *pPath)
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

/** A linked list of all visited nodes */
PATHNODE *pnode_ptr;
/**
 * @brief return a node for this position if it was visited, or NULL if not found
 */
PATHNODE *GetNode2(Point targetPosition)
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
 * @brief get the next node on the A* frontier to explore (estimated to be closest to the goal), mark it as visited, and return it
 */
PATHNODE *GetNextPath()
{
	PATHNODE *result = path_2_nodes->NextNode;
	if (result == nullptr) {
		return result;
	}

	path_2_nodes->NextNode = result->NextNode;
	result->NextNode = pnode_ptr->NextNode;
	pnode_ptr->NextNode = result;
	return result;
}

constexpr size_t MAXPATHNODES = 300;

/** Notes visisted by the path finding algorithm. */
PATHNODE path_nodes[MAXPATHNODES];
/** the number of in-use nodes in path_nodes */
uint32_t gdwCurNodes;
/**
 * @brief zero one of the preallocated nodes and return a pointer to it, or NULL if none are available
 */
PATHNODE *NewStep()
{
	if (gdwCurNodes >= MAXPATHNODES)
		return nullptr;

	PATHNODE *newNode = &path_nodes[gdwCurNodes];
	gdwCurNodes++;
	memset(newNode, 0, sizeof(PATHNODE));
	return newNode;
}

/** A stack for recursively searching nodes */
PATHNODE *pnode_tblptr[MAXPATHNODES];
/** size of the pnode_tblptr stack */
uint32_t gdwCurPathStep;
/**
 * @brief push pPath onto the pnode_tblptr stack
 */
void PushActiveStep(PATHNODE *pPath)
{
	assert(gdwCurPathStep < MAXPATHNODES);
	pnode_tblptr[gdwCurPathStep] = pPath;
	gdwCurPathStep++;
}

/**
 * @brief pop and return a node from the pnode_tblptr stack
 */
PATHNODE *PopActiveStep()
{
	gdwCurPathStep--;
	return pnode_tblptr[gdwCurPathStep];
}

/**
 * @brief return 2 if pPath is horizontally/vertically aligned with (dx,dy), else 3
 *
 * This approximates that diagonal movement on a square grid should have a cost
 * of sqrt(2). That's approximately 1.5, so they multiply all step costs by 2,
 * except diagonal steps which are times 3
 */
int CheckEqual(Point startPosition, Point destinationPosition)
{
	if (startPosition.x == destinationPosition.x || startPosition.y == destinationPosition.y)
		return 2;

	return 3;
}

/**
 * @brief update all path costs using depth-first search starting at pPath
 */
void SetCoords(PATHNODE *pPath)
{
	PushActiveStep(pPath);
	// while there are path nodes to check
	while (gdwCurPathStep > 0) {
		PATHNODE *pathOld = PopActiveStep();
		for (auto *pathAct : pathOld->Child) {
			if (pathAct == nullptr)
				break;

			if (pathOld->g + CheckEqual(pathOld->position, pathAct->position) < pathAct->g) {
				if (path_solid_pieces(pathOld->position, pathAct->position)) {
					pathAct->Parent = pathOld;
					pathAct->g = pathOld->g + CheckEqual(pathOld->position, pathAct->position);
					pathAct->f = pathAct->g + pathAct->h;
					PushActiveStep(pathAct);
				}
			}
		}
	}
}

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
int8_t GetPathDirection(Point startPosition, Point destinationPosition)
{
	constexpr int8_t PathDirections[9] = { 5, 1, 6, 2, 0, 3, 8, 4, 7 };
	return PathDirections[3 * (destinationPosition.y - startPosition.y) + 4 + destinationPosition.x - startPosition.x];
}

/**
 * @brief heuristic, estimated cost from startPosition to destinationPosition.
 */
int GetHeuristicCost(Point startPosition, Point destinationPosition)
{
	// see path_check_equal for why this is times 2
	return 2 * startPosition.ManhattanDistance(destinationPosition);
}

/**
 * @brief add a step from pPath to destination, return 1 if successful, and update the frontier/visited nodes accordingly
 *
 * @param pPath pointer to the current path node
 * @param candidatePosition expected to be a neighbour of the current path node position
 * @param destinationPosition where we hope to end up
 * @return true if step successfully added, false if we ran out of nodes to use
 */
bool ParentPath(PATHNODE *pPath, Point candidatePosition, Point destinationPosition)
{
	int nextG = pPath->g + CheckEqual(pPath->position, candidatePosition);

	// 3 cases to consider
	// case 1: (dx,dy) is already on the frontier
	PATHNODE *dxdy = GetNode1(candidatePosition);
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
		dxdy = GetNode2(candidatePosition);
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
				SetCoords(dxdy);
			}
		} else {
			// case 3: (dx,dy) is totally new
			dxdy = NewStep();
			if (dxdy == nullptr)
				return false;
			dxdy->Parent = pPath;
			dxdy->g = nextG;
			dxdy->h = GetHeuristicCost(candidatePosition, destinationPosition);
			dxdy->f = nextG + dxdy->h;
			dxdy->position = candidatePosition;
			// add it to the frontier
			NextNode(dxdy);

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
 * @brief perform a single step of A* bread-first search by trying to step in every possible direction from pPath with goal (x,y). Check each step with PosOk
 *
 * @return false if we ran out of preallocated nodes to use, else true
 */
bool GetPath(const std::function<bool(Point)> &posOk, PATHNODE *pPath, Point destination)
{
	for (auto dir : PathDirs) {
		Point tile = pPath->position + dir;
		bool ok = posOk(tile);
		if ((ok && path_solid_pieces(pPath->position, tile)) || (!ok && tile == destination)) {
			if (!ParentPath(pPath, tile, destination))
				return false;
		}
	}

	return true;
}

} // namespace

bool IsTileNotSolid(Point position)
{
	if (!InDungeonBounds(position)) {
		return false;
	}

	return !nSolidTable[dPiece[position.x][position.y]];
}

bool IsTileSolid(Point position)
{
	if (!InDungeonBounds(position)) {
		return false;
	}

	return nSolidTable[dPiece[position.x][position.y]];
}

bool IsTileWalkable(Point position, bool ignoreDoors)
{
	Object *object = ObjectAtPosition(position);
	if (object != nullptr) {
		if (ignoreDoors && object->IsDoor()) {
			return true;
		}
		if (object->_oSolidFlag) {
			return false;
		}
	}

	return !IsTileSolid(position);
}

bool IsTileOccupied(Point position)
{
	if (!InDungeonBounds(position)) {
		return true; // OOB positions are considered occupied.
	}

	if (IsTileSolid(position)) {
		return true;
	}
	if (dMonster[position.x][position.y] != 0) {
		return true;
	}
	if (dPlayer[position.x][position.y] != 0) {
		return true;
	}
	if (IsObjectAtPosition(position)) {
		return true;
	}

	return false;
}

int FindPath(const std::function<bool(Point)> &posOk, Point startPosition, Point destinationPosition, int8_t path[MAX_PATH_LENGTH])
{
	/**
	 * for reconstructing the path after the A* search is done. The longest
	 * possible path is actually 24 steps, even though we can fit 25
	 */
	static int8_t pnodeVals[MAX_PATH_LENGTH];

	// clear all nodes, create root nodes for the visited/frontier linked lists
	gdwCurNodes = 0;
	path_2_nodes = NewStep();
	pnode_ptr = NewStep();
	gdwCurPathStep = 0;
	PATHNODE *pathStart = NewStep();
	pathStart->g = 0;
	pathStart->h = GetHeuristicCost(startPosition, destinationPosition);
	pathStart->f = pathStart->h + pathStart->g;
	pathStart->position = startPosition;
	path_2_nodes->NextNode = pathStart;
	// A* search until we find (dx,dy) or fail
	PATHNODE *nextNode;
	while ((nextNode = GetNextPath()) != nullptr) {
		// reached the end, success!
		if (nextNode->position == destinationPosition) {
			PATHNODE *current = nextNode;
			int pathLength = 0;
			while (current->Parent != nullptr) {
				if (pathLength >= MAX_PATH_LENGTH)
					break;
				pnodeVals[pathLength++] = GetPathDirection(current->Parent->position, current->position);
				current = current->Parent;
			}
			if (pathLength != MAX_PATH_LENGTH) {
				int i;
				for (i = 0; i < pathLength; i++)
					path[i] = pnodeVals[pathLength - i - 1];
				return i;
			}
			return 0;
		}
		// ran out of nodes, abort!
		if (!GetPath(posOk, nextNode, destinationPosition))
			return 0;
	}
	// frontier is empty, no path!
	return 0;
}

bool path_solid_pieces(Point startPosition, Point destinationPosition)
{
	// These checks are written as if working backwards from the destination to the source, given
	// both tiles are expected to be adjacent this doesn't matter beyond being a bit confusing
	bool rv = true;
	switch (GetPathDirection(startPosition, destinationPosition)) {
	case 5: // Stepping north
		rv = IsTileNotSolid(destinationPosition + Direction::SouthWest) && IsTileNotSolid(destinationPosition + Direction::SouthEast);
		break;
	case 6: // Stepping east
		rv = IsTileNotSolid(destinationPosition + Direction::SouthWest) && IsTileNotSolid(destinationPosition + Direction::NorthWest);
		break;
	case 7: // Stepping south
		rv = IsTileNotSolid(destinationPosition + Direction::NorthEast) && IsTileNotSolid(destinationPosition + Direction::NorthWest);
		break;
	case 8: // Stepping west
		rv = IsTileNotSolid(destinationPosition + Direction::SouthEast) && IsTileNotSolid(destinationPosition + Direction::NorthEast);
		break;
	}
	return rv;
}

std::optional<Point> FindClosestValidPosition(const std::function<bool(Point)> &posOk, Point startingPosition, unsigned int minimumRadius, unsigned int maximumRadius)
{
	if (minimumRadius > maximumRadius) {
		return {}; // No valid search space with the given params.
	}

	if (minimumRadius == 0U) {
		if (posOk(startingPosition)) {
			return startingPosition;
		}
	}

	if (minimumRadius <= 1U && maximumRadius >= 1U) {
		// unrolling the case for radius 1 to save having to guard the corner check in the loop below.

		Point candidatePosition = startingPosition + Direction::SouthWest;
		if (posOk(candidatePosition)) {
			return candidatePosition;
		}
		candidatePosition = startingPosition + Direction::NorthEast;
		if (posOk(candidatePosition)) {
			return candidatePosition;
		}

		candidatePosition = startingPosition + Direction::NorthWest;
		if (posOk(candidatePosition)) {
			return candidatePosition;
		}

		candidatePosition = startingPosition + Direction::SouthEast;
		if (posOk(candidatePosition)) {
			return candidatePosition;
		}
	}

	if (maximumRadius >= 2U) {
		for (int i = static_cast<int>(std::max(minimumRadius, 2U)); i <= static_cast<int>(std::min(maximumRadius, 50U)); i++) {
			int x = 0;
			int y = i;

			// special case the checks when x == 0 to save checking the same tiles twice
			Point candidatePosition = startingPosition + Displacement { x, y };
			if (posOk(candidatePosition)) {
				return candidatePosition;
			}
			candidatePosition = startingPosition + Displacement { x, -y };
			if (posOk(candidatePosition)) {
				return candidatePosition;
			}

			while (x < i - 1) {
				x++;

				candidatePosition = startingPosition + Displacement { -x, y };
				if (posOk(candidatePosition)) {
					return candidatePosition;
				}

				candidatePosition = startingPosition + Displacement { x, y };
				if (posOk(candidatePosition)) {
					return candidatePosition;
				}

				candidatePosition = startingPosition + Displacement { -x, -y };
				if (posOk(candidatePosition)) {
					return candidatePosition;
				}

				candidatePosition = startingPosition + Displacement { x, -y };
				if (posOk(candidatePosition)) {
					return candidatePosition;
				}
			}

			// special case for inset corners
			y--;
			candidatePosition = startingPosition + Displacement { -x, y };
			if (posOk(candidatePosition)) {
				return candidatePosition;
			}

			candidatePosition = startingPosition + Displacement { x, y };
			if (posOk(candidatePosition)) {
				return candidatePosition;
			}

			candidatePosition = startingPosition + Displacement { -x, -y };
			if (posOk(candidatePosition)) {
				return candidatePosition;
			}

			candidatePosition = startingPosition + Displacement { x, -y };
			if (posOk(candidatePosition)) {
				return candidatePosition;
			}
			x++;

			while (y > 0) {
				candidatePosition = startingPosition + Displacement { -x, y };
				if (posOk(candidatePosition)) {
					return candidatePosition;
				}

				candidatePosition = startingPosition + Displacement { x, y };
				if (posOk(candidatePosition)) {
					return candidatePosition;
				}

				candidatePosition = startingPosition + Displacement { -x, -y };
				if (posOk(candidatePosition)) {
					return candidatePosition;
				}

				candidatePosition = startingPosition + Displacement { x, -y };
				if (posOk(candidatePosition)) {
					return candidatePosition;
				}

				y--;
			}

			// as above, special case for y == 0
			candidatePosition = startingPosition + Displacement { -x, y };
			if (posOk(candidatePosition)) {
				return candidatePosition;
			}

			candidatePosition = startingPosition + Displacement { x, y };
			if (posOk(candidatePosition)) {
				return candidatePosition;
			}
		}
	}

	return {};
}

#ifdef BUILD_TESTING
int TestPathGetHeuristicCost(Point startPosition, Point destinationPosition)
{
	return GetHeuristicCost(startPosition, destinationPosition);
}
#endif

} // namespace devilution
