/**
 * @file path.cpp
 *
 * Implementation of the path finding algorithms.
 */
#include "engine/path.h"

#include <array>

#include "levels/gendung.h"
#include "lighting.h"
#include "objects.h"

namespace devilution {
namespace {

constexpr size_t MaxPathNodes = 300;

struct PathNode {
	static constexpr uint16_t InvalidIndex = std::numeric_limits<uint16_t>::max();
	static constexpr size_t MaxChildren = 8;

	int16_t x = 0;
	int16_t y = 0;
	uint16_t parentIndex = InvalidIndex;
	uint16_t childIndices[MaxChildren] = { InvalidIndex, InvalidIndex, InvalidIndex, InvalidIndex, InvalidIndex, InvalidIndex, InvalidIndex, InvalidIndex };
	uint16_t nextNodeIndex = InvalidIndex;
	uint8_t f = 0;
	uint8_t h = 0;
	uint8_t g = 0;

	[[nodiscard]] Point position() const
	{
		return Point { x, y };
	}

	void addChild(uint16_t childIndex)
	{
		size_t index = 0;
		for (; index < MaxChildren; ++index) {
			if (childIndices[index] == InvalidIndex)
				break;
		}
		assert(index < MaxChildren);
		childIndices[index] = childIndex;
	}
};

PathNode PathNodes[MaxPathNodes];

/** A linked list of the A* frontier, sorted by distance */
PathNode *Path2Nodes;

/**
 * @brief return a node for a position on the frontier, or NULL if not found
 */
uint16_t GetNode1(Point targetPosition)
{
	uint16_t result = Path2Nodes->nextNodeIndex;
	while (result != PathNode::InvalidIndex) {
		if (PathNodes[result].position() == targetPosition)
			return result;
		result = PathNodes[result].nextNodeIndex;
	}
	return PathNode::InvalidIndex;
}

/**
 * @brief insert `front` node into the frontier (keeping the frontier sorted by total distance)
 */
void NextNode(uint16_t front)
{
	if (Path2Nodes->nextNodeIndex == PathNode::InvalidIndex) {
		Path2Nodes->nextNodeIndex = front;
		return;
	}

	PathNode *current = Path2Nodes;
	uint16_t nextIndex = Path2Nodes->nextNodeIndex;
	const uint8_t maxF = PathNodes[front].f;
	while (nextIndex != PathNode::InvalidIndex && PathNodes[nextIndex].f < maxF) {
		current = &PathNodes[nextIndex];
		nextIndex = current->nextNodeIndex;
	}
	PathNodes[front].nextNodeIndex = nextIndex;
	current->nextNodeIndex = front;
}

/** A linked list of all visited nodes */
PathNode *VisitedNodes;

/**
 * @brief return a node for this position if it was visited, or NULL if not found
 */
uint16_t GetNode2(Point targetPosition)
{
	uint16_t result = VisitedNodes->nextNodeIndex;
	while (result != PathNode::InvalidIndex) {
		if (PathNodes[result].position() == targetPosition)
			return result;
		result = PathNodes[result].nextNodeIndex;
	}
	return result;
}

/**
 * @brief get the next node on the A* frontier to explore (estimated to be closest to the goal), mark it as visited, and return it
 */
uint16_t GetNextPath()
{
	uint16_t result = Path2Nodes->nextNodeIndex;
	if (result == PathNode::InvalidIndex) {
		return result;
	}

	Path2Nodes->nextNodeIndex = PathNodes[result].nextNodeIndex;
	PathNodes[result].nextNodeIndex = VisitedNodes->nextNodeIndex;
	VisitedNodes->nextNodeIndex = result;
	return result;
}

/** the number of in-use nodes in path_nodes */
uint32_t gdwCurNodes;
/**
 * @brief zero one of the preallocated nodes and return a pointer to it, or NULL if none are available
 */
uint16_t NewStep()
{
	if (gdwCurNodes >= MaxPathNodes)
		return PathNode::InvalidIndex;

	PathNodes[gdwCurNodes] = {};
	return gdwCurNodes++;
}

/** A stack for recursively searching nodes */
uint16_t pnode_tblptr[MaxPathNodes];
/** size of the pnode_tblptr stack */
uint32_t gdwCurPathStep;
/**
 * @brief push pPath onto the pnode_tblptr stack
 */
void PushActiveStep(uint16_t pPath)
{
	assert(gdwCurPathStep < MaxPathNodes);
	pnode_tblptr[gdwCurPathStep] = pPath;
	gdwCurPathStep++;
}

/**
 * @brief pop and return a node from the pnode_tblptr stack
 */
uint16_t PopActiveStep()
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
void SetCoords(uint16_t pPath)
{
	PushActiveStep(pPath);
	// while there are path nodes to check
	while (gdwCurPathStep > 0) {
		uint16_t pathOldIndex = PopActiveStep();
		const PathNode &pathOld = PathNodes[pathOldIndex];
		for (uint16_t childIndex : pathOld.childIndices) {
			if (childIndex == PathNode::InvalidIndex)
				break;
			PathNode &pathAct = PathNodes[childIndex];

			if (pathOld.g + CheckEqual(pathOld.position(), pathAct.position()) < pathAct.g) {
				if (path_solid_pieces(pathOld.position(), pathAct.position())) {
					pathAct.parentIndex = pathOldIndex;
					pathAct.g = pathOld.g + CheckEqual(pathOld.position(), pathAct.position());
					pathAct.f = pathAct.g + pathAct.h;
					PushActiveStep(childIndex);
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
 * @param pathIndex index of the current path node
 * @param candidatePosition expected to be a neighbour of the current path node position
 * @param destinationPosition where we hope to end up
 * @return true if step successfully added, false if we ran out of nodes to use
 */
bool ParentPath(uint16_t pathIndex, Point candidatePosition, Point destinationPosition)
{
	PathNode &path = PathNodes[pathIndex];
	int nextG = path.g + CheckEqual(path.position(), candidatePosition);

	// 3 cases to consider
	// case 1: (dx,dy) is already on the frontier
	uint16_t dxdyIndex = GetNode1(candidatePosition);
	if (dxdyIndex != PathNode::InvalidIndex) {
		path.addChild(dxdyIndex);
		PathNode &dxdy = PathNodes[dxdyIndex];
		if (nextG < dxdy.g) {
			if (path_solid_pieces(path.position(), candidatePosition)) {
				// we'll explore it later, just update
				dxdy.parentIndex = pathIndex;
				dxdy.g = nextG;
				dxdy.f = nextG + dxdy.h;
			}
		}
	} else {
		// case 2: (dx,dy) was already visited
		dxdyIndex = GetNode2(candidatePosition);
		if (dxdyIndex != PathNode::InvalidIndex) {
			path.addChild(dxdyIndex);
			PathNode &dxdy = PathNodes[dxdyIndex];
			if (nextG < dxdy.g && path_solid_pieces(path.position(), candidatePosition)) {
				// update the node
				dxdy.parentIndex = pathIndex;
				dxdy.g = nextG;
				dxdy.f = nextG + dxdy.h;
				// already explored, so re-update others starting from that node
				SetCoords(dxdyIndex);
			}
		} else {
			// case 3: (dx,dy) is totally new
			dxdyIndex = NewStep();
			if (dxdyIndex == PathNode::InvalidIndex)
				return false;
			PathNode &dxdy = PathNodes[dxdyIndex];
			dxdy.parentIndex = pathIndex;
			dxdy.g = nextG;
			dxdy.h = GetHeuristicCost(candidatePosition, destinationPosition);
			dxdy.f = nextG + dxdy.h;
			dxdy.x = static_cast<int16_t>(candidatePosition.x);
			dxdy.y = static_cast<int16_t>(candidatePosition.y);
			// add it to the frontier
			NextNode(dxdyIndex);
			path.addChild(dxdyIndex);
		}
	}
	return true;
}

/**
 * @brief perform a single step of A* bread-first search by trying to step in every possible direction from pPath with goal (x,y). Check each step with PosOk
 *
 * @return false if we ran out of preallocated nodes to use, else true
 */
bool GetPath(const std::function<bool(Point)> &posOk, uint16_t pathIndex, Point destination)
{
	for (Displacement dir : PathDirs) {
		const PathNode &path = PathNodes[pathIndex];
		const Point tile = path.position() + dir;
		const bool ok = posOk(tile);
		if ((ok && path_solid_pieces(path.position(), tile)) || (!ok && tile == destination)) {
			if (!ParentPath(pathIndex, tile, destination))
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

	return !TileHasAny(dPiece[position.x][position.y], TileProperties::Solid);
}

bool IsTileSolid(Point position)
{
	if (!InDungeonBounds(position)) {
		return false;
	}

	return TileHasAny(dPiece[position.x][position.y], TileProperties::Solid);
}

bool IsTileWalkable(Point position, bool ignoreDoors)
{
	Object *object = FindObjectAtPosition(position);
	if (object != nullptr) {
		if (ignoreDoors && object->isDoor()) {
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

int FindPath(const std::function<bool(Point)> &posOk, Point startPosition, Point destinationPosition, int8_t path[MaxPathLength])
{
	/**
	 * for reconstructing the path after the A* search is done. The longest
	 * possible path is actually 24 steps, even though we can fit 25
	 */
	static int8_t pnodeVals[MaxPathLength];

	// clear all nodes, create root nodes for the visited/frontier linked lists
	gdwCurNodes = 0;
	Path2Nodes = &PathNodes[NewStep()];
	VisitedNodes = &PathNodes[NewStep()];
	gdwCurPathStep = 0;
	const uint16_t pathStartIndex = NewStep();
	PathNode &pathStart = PathNodes[pathStartIndex];
	pathStart.x = static_cast<int16_t>(startPosition.x);
	pathStart.y = static_cast<int16_t>(startPosition.y);
	pathStart.f = pathStart.h + pathStart.g;
	pathStart.h = GetHeuristicCost(startPosition, destinationPosition);
	pathStart.g = 0;
	Path2Nodes->nextNodeIndex = pathStartIndex;
	// A* search until we find (dx,dy) or fail
	uint16_t nextNodeIndex;
	while ((nextNodeIndex = GetNextPath()) != PathNode::InvalidIndex) {
		// reached the end, success!
		if (PathNodes[nextNodeIndex].position() == destinationPosition) {
			const PathNode *current = &PathNodes[nextNodeIndex];
			size_t pathLength = 0;
			while (current->parentIndex != PathNode::InvalidIndex) {
				if (pathLength >= MaxPathLength)
					break;
				pnodeVals[pathLength++] = GetPathDirection(PathNodes[current->parentIndex].position(), current->position());
				current = &PathNodes[current->parentIndex];
			}
			if (pathLength != MaxPathLength) {
				size_t i;
				for (i = 0; i < pathLength; i++)
					path[i] = pnodeVals[pathLength - i - 1];
				return static_cast<int>(i);
			}
			return 0;
		}
		// ran out of nodes, abort!
		if (!GetPath(posOk, nextNodeIndex, destinationPosition))
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
	return Crawl(minimumRadius, maximumRadius, [&](Displacement displacement) -> std::optional<Point> {
		Point candidatePosition = startingPosition + displacement;
		if (posOk(candidatePosition))
			return candidatePosition;
		return {};
	});
}

#ifdef BUILD_TESTING
int TestPathGetHeuristicCost(Point startPosition, Point destinationPosition)
{
	return GetHeuristicCost(startPosition, destinationPosition);
}
#endif

} // namespace devilution
