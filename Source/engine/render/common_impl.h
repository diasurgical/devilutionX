/**
 * @file common_impl.h
 *
 * Common code for implementing various renderers.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "engine.h"
#include "lighting.h"

namespace devilution {

inline std::uint8_t *GetLightTable(char light)
{
	int idx = 4096;
	if (light == 2)
		idx += 256; // gray colors
	if (light >= 4)
		idx += (light - 1) << 8;
	return &LightTables[idx];
}

struct ClipX {
	std::int_fast16_t left;
	std::int_fast16_t right;
	std::int_fast16_t width;
};

inline ClipX CalculateClipX(std::int_fast16_t x, std::size_t w, const Surface &out)
{
	ClipX clip;
	clip.left = static_cast<std::int_fast16_t>(x < 0 ? -x : 0);
	clip.right = static_cast<std::int_fast16_t>(static_cast<std::int_fast16_t>(x + w) > out.w() ? x + w - out.w() : 0);
	clip.width = static_cast<std::int_fast16_t>(w - clip.left - clip.right);
	return clip;
}

} // namespace devilution
