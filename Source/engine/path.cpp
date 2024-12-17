/**
 * @file path.cpp
 *
 * Implementation of the path finding algorithms.
 */
#include "engine/path.h"

#include <algorithm>
#include <array>
#include <cstdint>

#include <function_ref.hpp>

#include "crawl.hpp"
#include "levels/gendung.h"
#include "objects.h"
#include "utils/algorithm/container.hpp"
#include "utils/static_vector.hpp"

namespace devilution {

// The frame times for axis-aligned and diagonal steps are actually the same.
//
// However, we set the diagonal step cost a bit higher to avoid
// excessive diagonal movement. For example, the frame times for these
// two paths are the same: ↑↑ and ↗↖. However, ↑↑ looks more natural.
const int PathAxisAlignedStepCost = 100;
const int PathDiagonalStepCost = 101;

namespace {

constexpr size_t MaxPathNodes = 300;

using NodeIndexType = uint16_t;
using CoordType = uint8_t;
using CostType = uint16_t;
using PointT = PointOf<CoordType>;

struct FrontierNode {
	PointT position;

	// Current best guess of the cost of the path to destination
	// if it goes through this node.
	CostType f;
};

struct ExploredNode {
	// Preceding node (needed to reconstruct the path at the end).
	PointT prev;

	// The current lowest cost from start to this node (0 for the start node).
	CostType g;
};

// A simple map with 4 buckets and static storage.
class ExploredNodes {
	static const size_t NumBuckets = 4;
	static const size_t BucketCapacity = MaxPathNodes / NumBuckets;
	using Entry = std::pair<PointT, ExploredNode>;
	using Bucket = StaticVector<Entry, BucketCapacity>;

public:
	using value_type = Entry;
	using iterator = value_type *;
	using const_iterator = const value_type *;

	[[nodiscard]] const_iterator find(const PointT &point) const
	{
		const Bucket &b = bucket(point);
		const auto it = c_find_if(b, [&point](const Entry &e) { return e.first == point; });
		if (it == b.end()) return nullptr;
		return it;
	}
	[[nodiscard]] iterator find(const PointT &point)
	{
		Bucket &b = bucket(point);
		auto it = c_find_if(b, [&point](const Entry &e) { return e.first == point; });
		if (it == b.end()) return nullptr;
		return it;
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	[[nodiscard]] const_iterator end() const { return nullptr; }
	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	[[nodiscard]] iterator end() { return nullptr; }

	void emplace(const PointT &point, const ExploredNode &exploredNode)
	{
		bucket(point).emplace_back(point, exploredNode);
	}

	[[nodiscard]] bool canInsert(const PointT &point) const
	{
		return bucket(point).size() < BucketCapacity;
	}

private:
	[[nodiscard]] const Bucket &bucket(const PointT &point) const { return buckets_[bucketIndex(point)]; }
	[[nodiscard]] Bucket &bucket(const PointT &point) { return buckets_[bucketIndex(point)]; }
	[[nodiscard]] static size_t bucketIndex(const PointT &point)
	{
		return ((point.x & 0x1) << 1) | (point.y & 0x1);
	}

	std::array<Bucket, NumBuckets> buckets_;
};

bool IsDiagonalStep(const Point &a, const Point &b)
{
	return a.x != b.x && a.y != b.y;
}

/**
 * @brief Returns the distance between 2 adjacent nodes.
 */
CostType GetDistance(PointT startPosition, PointT destinationPosition)
{
	return IsDiagonalStep(startPosition, destinationPosition)
	    ? PathDiagonalStepCost
	    : PathAxisAlignedStepCost;
}

/**
 * @brief heuristic, estimated cost from startPosition to destinationPosition.
 */
CostType GetHeuristicCost(PointT startPosition, PointT destinationPosition)
{
	// This function needs to be admissible, i.e. it should never over-estimate
	// the distance.
	//
	// This calculation assumes we can take diagonal steps until we reach
	// the same row or column and then take the remaining axis-aligned steps.
	const int dx = std::abs(static_cast<int>(startPosition.x) - static_cast<int>(destinationPosition.x));
	const int dy = std::abs(static_cast<int>(startPosition.y) - static_cast<int>(destinationPosition.y));
	const int diagSteps = std::min(dx, dy);

	// After we've taken `diagSteps`, the remaining steps in one coordinate
	// will be zero, and in the other coordinate it will be reduced by `diagSteps`.
	// We then still need to take the remaining steps:
	//   max(dx, dy) - diagSteps = max(dx, dy) - min(dx, dy) = abs(dx - dy)
	const int axisAlignedSteps = std::abs(dx - dy);
	return diagSteps * PathDiagonalStepCost + axisAlignedSteps * PathAxisAlignedStepCost;
}

/**
 * Returns a number representing the direction from a starting tile to a neighbouring tile.
 *
 * Used to represent the steps in the resulting path.
 *
 * Each step direction is assigned a number like this:
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

int ReconstructPath(const ExploredNodes &explored, PointT dest, int8_t path[MaxPathLength])
{
	int len = 0;
	PointT cur = dest;
	while (true) {
		const auto it = explored.find(cur);
		if (it == explored.end()) app_fatal("Failed to reconstruct path");
		if (it->second.g == 0) break; // reached start
		path[len++] = GetPathDirection(it->second.prev, cur);
		cur = it->second.prev;
		if (len == MaxPathLength) {
			// Path too long.
			len = 0;
			break;
		}
	}
	std::reverse(path, path + len);
	std::fill(path + len, path + MaxPathLength, -1);
	return len;
}

} // namespace

bool IsTileNotSolid(Point position)
{
	if (!InDungeonBounds(position)) {
		return false;
	}

	return !TileHasAny(position, TileProperties::Solid);
}

bool IsTileSolid(Point position)
{
	if (!InDungeonBounds(position)) {
		return false;
	}

	return TileHasAny(position, TileProperties::Solid);
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

	return IsTileNotSolid(position);
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

int FindPath(tl::function_ref<bool(Point)> posOk, Point startPosition, Point destinationPosition, int8_t path[MaxPathLength])
{
	const PointT start { startPosition };
	const PointT dest { destinationPosition };

	StaticVector<FrontierNode, MaxPathNodes> frontier;
	ExploredNodes explored;
	{
		frontier.emplace_back(FrontierNode { .position = start, .f = GetHeuristicCost(start, dest) });
		explored.emplace(start, ExploredNode { .prev = {}, .g = 0 });
	}

	const auto frontierComparator = [&explored](const FrontierNode &a, const FrontierNode &b) {
		// We use heap functions from <algorithm> which form a max-heap.
		// We reverse the comparison sign here to get a min-heap.
		if (a.f != b.f) return a.f > b.f;

		// Prefer diagonal steps first.
		const bool isDiagonalA = IsDiagonalStep(explored.find(a.position)->second.prev, a.position);
		const bool isDiagonalB = IsDiagonalStep(explored.find(a.position)->second.prev, b.position);
		if (isDiagonalA != isDiagonalB) return isDiagonalB;

		// Finally, disambiguate by coordinate:
		if (a.position.x != b.position.x) return a.position.x > b.position.x;
		return a.position.y > b.position.y;
	};

	while (!frontier.empty()) {
		const FrontierNode cur = frontier.front(); // argmin(node.f) for node in openSet

		if (cur.position == destinationPosition) {
			return ReconstructPath(explored, cur.position, path);
		}

		std::pop_heap(frontier.begin(), frontier.end(), frontierComparator);
		frontier.pop_back();
		const CostType curG = explored.find(cur.position)->second.g;

		// Discard invalid nodes.

		// If this node is already at the maximum number of steps, we can skip processing it.
		// We don't keep track of the maximum number of steps, so we approximate it.
		if (curG >= PathDiagonalStepCost * MaxPathLength) continue;

		// When we discover a better path to a node, we push the node to the heap
		// with the new `f` value even if the node is already in the heap.
		if (curG + GetHeuristicCost(cur.position, dest) > cur.f) continue;

		for (const DisplacementOf<int8_t> d : PathDirs) {
			// We're using `uint8_t` for coordinates. Avoid underflow:
			if ((cur.position.x == 0 && d.deltaX < 0) || (cur.position.y == 0 && d.deltaY < 0)) continue;
			const PointT neighborPos = cur.position + d;
			const bool ok = posOk(neighborPos);
			if (ok) {
				if (!CanStep(cur.position, neighborPos)) continue;
			} else {
				// We only check CanStep for destination if `posOk` returns true for it.
				// This seems like a bug
				if (neighborPos != dest) continue;
			}
			const CostType g = curG + GetDistance(cur.position, neighborPos);
			if (curG >= PathDiagonalStepCost * MaxPathLength) continue;
			bool improved = false;
			if (auto it = explored.find(neighborPos); it == explored.end()) {
				if (explored.canInsert(neighborPos)) {
					explored.emplace(neighborPos, ExploredNode { .prev = cur.position, .g = g });
					improved = true;
				}
			} else if (it->second.g > g) {
				it->second.prev = cur.position;
				it->second.g = g;
				improved = true;
			}
			if (improved) {
				const CostType f = g + GetHeuristicCost(neighborPos, dest);
				if (frontier.size() < MaxPathNodes) {
					// We always push the node to the heap, even if the same position already exists in it.
					// When popping from the heap, we discard invalid nodes by checking that `g + h <= f`.
					frontier.emplace_back(FrontierNode { .position = neighborPos, .f = f });
					std::push_heap(frontier.begin(), frontier.end(), frontierComparator);
				}
			}
		}
	}

	return 0; // no path
}

bool CanStep(Point startPosition, Point destinationPosition)
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

std::optional<Point> FindClosestValidPosition(tl::function_ref<bool(Point)> posOk, Point startingPosition, unsigned int minimumRadius, unsigned int maximumRadius)
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
