#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <utility>

#include "levels/dun_tile.hpp"

namespace devilution {

/**
 * @brief Re-encodes dungeon cels, removing redundant padding bytes from triangles and trapezoids.
 *
 * This reduces memory usage and simplifies the rendering.
 */
void ReencodeDungeonCels(std::unique_ptr<std::byte[]> &dungeonCels, std::span<std::pair<uint16_t, TileType>> frames);

} // namespace devilution
