/**
 * @file random.hpp
 *
 * Contains convenience functions for random number generation
 *
 * This includes specific engine/distribution functions for logic that needs to be compatible with the base game.
 */
#pragma once

#include <cstdint>

namespace devilution
{

void SetRndSeed(uint32_t s);
int32_t AdvanceRndSeed();
int32_t GetRndSeed();
uint32_t GetLCGEngineState();
int32_t GenerateRnd(int32_t v);

} // namespace devilution
