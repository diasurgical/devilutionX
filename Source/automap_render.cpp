#include "automap_render.hpp"

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
		SetPixel(out, from, colorIndex);
		from.x += static_cast<int>(DirX);
		SetPixel(out, from, colorIndex);
		from.x += static_cast<int>(DirX);
		from.y += static_cast<int>(DirY);
	}
	SetPixel(out, from, colorIndex);
}

template <DirectionX DirX, DirectionY DirY>
void DrawMapLine2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	while (width-- > 0) {
		SetPixel(out, from, colorIndex);
		from.y += static_cast<int>(DirY);
		SetPixel(out, from, colorIndex);
		from.y += static_cast<int>(DirY);
		from.x += static_cast<int>(DirX);
	}
	SetPixel(out, from, colorIndex);
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

void DrawMapLineNE2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLine2<DirectionX::EAST, DirectionY::NORTH>(out, from, width, colorIndex);
}

void DrawMapLineSE2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLine2<DirectionX::EAST, DirectionY::SOUTH>(out, from, width, colorIndex);
}

void DrawMapLineNW2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLine2<DirectionX::WEST, DirectionY::NORTH>(out, from, width, colorIndex);
}

void DrawMapLineSW2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	DrawMapLine2<DirectionX::WEST, DirectionY::SOUTH>(out, from, width, colorIndex);
}

} // namespace devilution
