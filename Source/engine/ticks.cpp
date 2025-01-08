#include "engine/ticks.hpp"

#include <cstdint>

#include <SDL.h>

namespace devilution {

uint32_t GetAnimationFrame(uint32_t frames, uint32_t fps)
{
	return (SDL_GetTicks() / fps) % frames;
}

} // namespace devilution
