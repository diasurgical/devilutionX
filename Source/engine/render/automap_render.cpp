/**
 * @file automap_render.cpp
 *
 * Line drawing routines for the automap.
 */
#include "engine/render/automap_render.hpp"
#include "options.h"

namespace devilution {
namespace {

enum class DirectionX {
	EAST = 1,
	WEST = -1,
};

enum class DirectionY {
	SOUTH = 1,
	NORTH = -1,
};

template <DirectionX DirX, DirectionY DirY>
void DrawMapLine(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex)
{
	while (height-- > 0) {
		if(sgOptions.Gameplay.bDrawAutomapBorder == true) {
			out.SetPixel({ from.x, from.y + 1 }, 0);
		}
		out.SetPixel(from, colorIndex);
		from.x += static_cast<int>(DirX);
		if(sgOptions.Gameplay.bDrawAutomapBorder == true) {
			out.SetPixel({ from.x, from.y + 1 }, 0);
		}
		out.SetPixel(from, colorIndex);
		from.x += static_cast<int>(DirX);
		from.y += static_cast<int>(DirY);
	}
	if(sgOptions.Gameplay.bDrawAutomapBorder == true) {
		out.SetPixel({ from.x, from.y + 1 }, 0);
	}
		out.SetPixel(from, colorIndex);
}

template <DirectionX DirX, DirectionY DirY>
void DrawMapLineSteep(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	while (width-- > 0) {
		out.SetPixel(from, colorIndex);
		from.y += static_cast<int>(DirY);
		out.SetPixel(from, colorIndex);
		from.y += static_cast<int>(DirY);
		from.x += static_cast<int>(DirX);
	}
	out.SetPixel(from, colorIndex);
}

} // namespace

void DrawMapLineNE(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex)
{
	DrawMapLine<DirectionX::EAST, DirectionY::NORTH>(out, from, height, colorIndex);
}

void DrawMapLineSE(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex)
{
	DrawMapLine<DirectionX::EAST, DirectionY::SOUTH>(out, from, height, colorIndex);
}

void DrawMapLineNW(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex)
{
	DrawMapLine<DirectionX::WEST, DirectionY::NORTH>(out, from, height, colorIndex);
}

void DrawMapLineSW(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex)
{
	DrawMapLine<DirectionX::WEST, DirectionY::SOUTH>(out, from, height, colorIndex);
}

void DrawMapLineSteepNE(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLineSteep<DirectionX::EAST, DirectionY::NORTH>(out, from, width, colorIndex);
}

void DrawMapLineSteepSE(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLineSteep<DirectionX::EAST, DirectionY::SOUTH>(out, from, width, colorIndex);
}

void DrawMapLineSteepNW(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLineSteep<DirectionX::WEST, DirectionY::NORTH>(out, from, width, colorIndex);
}

void DrawMapLineSteepSW(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLineSteep<DirectionX::WEST, DirectionY::SOUTH>(out, from, width, colorIndex);
}

} // namespace devilution
