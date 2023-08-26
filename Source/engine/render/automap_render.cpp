/**
 * @file automap_render.cpp
 *
 * Line drawing routines for the automap.
 */
#include "engine/render/automap_render.hpp"
#include "automap.h"
#include "engine.h"

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

void SetMapPixel(const Surface &out, Point point, uint8_t color)
{
	if (!AutomapTransparent) {
		out.SetPixel(point, color);
	} else {
		SetHalfTransparentPixel(out, point, color);
	}
}

template <DirectionX DirX, DirectionY DirY>
void DrawMapLine(const Surface &out, Point from, int height, std::uint8_t colorIndex)
{
	while (height-- > 0) {
		SetMapPixel(out, { from.x, from.y + 1 }, 0);
		SetMapPixel(out, from, colorIndex);
		from.x += static_cast<int>(DirX);
		SetMapPixel(out, { from.x, from.y + 1 }, 0);
		SetMapPixel(out, from, colorIndex);
		from.x += static_cast<int>(DirX);
		from.y += static_cast<int>(DirY);
	}
	SetMapPixel(out, { from.x, from.y + 1 }, 0);
	SetMapPixel(out, from, colorIndex);
}

template <DirectionX DirX, DirectionY DirY>
void DrawMapLineSteep(const Surface &out, Point from, int width, std::uint8_t colorIndex)
{
	while (width-- > 0) {
		SetMapPixel(out, { from.x, from.y + 1 }, 0);
		SetMapPixel(out, from, colorIndex);
		from.y += static_cast<int>(DirY);
		SetMapPixel(out, { from.x, from.y + 1 }, 0);
		SetMapPixel(out, from, colorIndex);
		from.y += static_cast<int>(DirY);
		from.x += static_cast<int>(DirX);
	}
	SetMapPixel(out, { from.x, from.y + 1 }, 0);
	SetMapPixel(out, from, colorIndex);
}

void DrawMapFreeLine(const Surface &out, Point from, Point to, uint8_t colorIndex)
{
	int dx = std::abs(to.x - from.x);
	int dy = std::abs(to.y - from.y);
	int sx = from.x < to.x ? 1 : -1;
	int sy = from.y < to.y ? 1 : -1;
	int err = dx - dy;

	while (true) {
		SetMapPixel(out, from, colorIndex);

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

void DrawMapEllipse(const Surface &out, Point from, int radius, uint8_t colorIndex)
{
	const int a = radius;
	const int b = radius / 2;

	int x = 0;
	int y = b;

	// Initial point
	SetMapPixel(out, { from.x, from.y + b }, colorIndex);
	SetMapPixel(out, { from.x, from.y - b }, colorIndex);

	// Initialize the parameters
	int p1 = (b * b) - (a * a * b) + (a * a) / 4;

	// Region 1
	while ((b * b * x) < (a * a * y)) {
		x++;
		if (p1 < 0) {
			p1 += (2 * b * b * x) + (b * b);
		} else {
			y--;
			p1 += (2 * b * b * x) - (2 * a * a * y) + (b * b);
		}

		SetMapPixel(out, { from.x + x, from.y + y }, colorIndex);
		SetMapPixel(out, { from.x - x, from.y + y }, colorIndex);
		SetMapPixel(out, { from.x + x, from.y - y }, colorIndex);
		SetMapPixel(out, { from.x - x, from.y - y }, colorIndex);
	}

	// Initialize the second parameter for Region 2
	int p2 = (b * b * ((x + 1) * (x + 1))) + (a * a * ((y - 1) * (y - 1))) - (a * a * b * b);

	// Region 2
	while (y > 0) {
		y--;
		if (p2 > 0) {
			p2 += (-2 * a * a * y) + (a * a);
		} else {
			x++;
			p2 += (2 * b * b * x) - (2 * a * a * y) + (a * a);
		}

		SetMapPixel(out, { from.x + x, from.y + y }, colorIndex);
		SetMapPixel(out, { from.x - x, from.y + y }, colorIndex);
		SetMapPixel(out, { from.x + x, from.y - y }, colorIndex);
		SetMapPixel(out, { from.x - x, from.y - y }, colorIndex);
	}
}

void DrawMapStar(const Surface &out, Point center, int radius, uint8_t color)
{
	const int scaleFactor = 128;
	Point anchors[5];

	anchors[0] = { center.x - (121 * radius / scaleFactor), center.y + (19 * radius / scaleFactor) }; // Left Point
	anchors[1] = { center.x + (121 * radius / scaleFactor), center.y + (19 * radius / scaleFactor) }; // Right Point
	anchors[2] = { center.x, center.y + (64 * radius / scaleFactor) };                                // Bottom Point
	anchors[3] = { center.x - (75 * radius / scaleFactor), center.y - (51 * radius / scaleFactor) };  // Top Left Point
	anchors[4] = { center.x + (75 * radius / scaleFactor), center.y - (51 * radius / scaleFactor) };  // Top Right Point

	// Draw lines between the anchors to form a star
	DrawMapFreeLine(out, anchors[3], anchors[1], color); // Connect Top Left -> Right
	DrawMapFreeLine(out, anchors[1], anchors[0], color); // Connect Right -> Left
	DrawMapFreeLine(out, anchors[0], anchors[4], color); // Connect Left -> Top Right
	DrawMapFreeLine(out, anchors[4], anchors[2], color); // Connect Top Right -> Bottom
	DrawMapFreeLine(out, anchors[2], anchors[3], color); // Connect Bottom -> Top Left
}

} // namespace devilution
