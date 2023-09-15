/**
 * @file automap_render.cpp
 *
 * Line drawing routines for the automap.
 */
#include "engine/render/automap_render.hpp"

#include <cstdint>

namespace devilution {
namespace {

enum class DirectionX : int8_t {
	EAST = 1,
	WEST = -1,
};

enum class DirectionY : int8_t {
	SOUTH = 1,
	NORTH = -1,
};

template <DirectionX DirX, DirectionY DirY>
void DrawMapLine(const Surface &out, Point from, int height, std::uint8_t colorIndex)
{
	while (height-- > 0) {
		out.SetPixel({ from.x, from.y + 1 }, 0);
		out.SetPixel(from, colorIndex);
		from.x += static_cast<int>(DirX);
		out.SetPixel({ from.x, from.y + 1 }, 0);
		out.SetPixel(from, colorIndex);
		from.x += static_cast<int>(DirX);
		from.y += static_cast<int>(DirY);
	}
	out.SetPixel({ from.x, from.y + 1 }, 0);
	out.SetPixel(from, colorIndex);
}

template <DirectionX DirX, DirectionY DirY>
void DrawMapLineSteep(const Surface &out, Point from, int width, std::uint8_t colorIndex)
{
	while (width-- > 0) {
		out.SetPixel({ from.x, from.y + 1 }, 0);
		out.SetPixel(from, colorIndex);
		from.y += static_cast<int>(DirY);
		out.SetPixel({ from.x, from.y + 1 }, 0);
		out.SetPixel(from, colorIndex);
		from.y += static_cast<int>(DirY);
		from.x += static_cast<int>(DirX);
	}
	out.SetPixel({ from.x, from.y + 1 }, 0);
	out.SetPixel(from, colorIndex);
}

} // namespace

void DrawMapLineNE(const Surface &out, Point from, int height, std::uint8_t colorIndex)
{
	DrawMapLine<DirectionX::EAST, DirectionY::NORTH>(out, from, height, colorIndex);
}

void DrawMapLineSE(const Surface &out, Point from, int height, std::uint8_t colorIndex)
{
	DrawMapLine<DirectionX::EAST, DirectionY::SOUTH>(out, from, height, colorIndex);
}

void DrawMapLineNW(const Surface &out, Point from, int height, std::uint8_t colorIndex)
{
	DrawMapLine<DirectionX::WEST, DirectionY::NORTH>(out, from, height, colorIndex);
}

void DrawMapLineSW(const Surface &out, Point from, int height, std::uint8_t colorIndex)
{
	DrawMapLine<DirectionX::WEST, DirectionY::SOUTH>(out, from, height, colorIndex);
}

void DrawMapLineSteepNE(const Surface &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLineSteep<DirectionX::EAST, DirectionY::NORTH>(out, from, width, colorIndex);
}

void DrawMapLineSteepSE(const Surface &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLineSteep<DirectionX::EAST, DirectionY::SOUTH>(out, from, width, colorIndex);
}

void DrawMapLineSteepNW(const Surface &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLineSteep<DirectionX::WEST, DirectionY::NORTH>(out, from, width, colorIndex);
}

void DrawMapLineSteepSW(const Surface &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLineSteep<DirectionX::WEST, DirectionY::SOUTH>(out, from, width, colorIndex);
}

/**
 * @brief Draws a line from first point to second point, unrestricted to the standard automap angles. Doesn't include shadow.
 */
void DrawMapFreeLine(const Surface &out, Point from, Point to, uint8_t colorIndex)
{
	int dx = std::abs(to.x - from.x);
	int dy = std::abs(to.y - from.y);
	int sx = from.x < to.x ? 1 : -1;
	int sy = from.y < to.y ? 1 : -1;
	int err = dx - dy;

	while (true) {
		out.SetPixel(from, colorIndex);

		if (from.x == to.x && from.y == to.y) {
			break;
		}

		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			from.x += sx;
		}
		if (e2 < dx) {
			err += dx;
			from.y += sy;
		}
	}
}

} // namespace devilution
