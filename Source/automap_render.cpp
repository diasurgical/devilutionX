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
void UnsafeDrawMapLine(const CelOutputBuffer &out, Point from, int height, bool drawTipPixel, std::uint8_t colorIndex)
{
	auto *dst = out.at(from.x, from.y);
	const auto pitch = out.pitch();
	while (height-- > 0) {
		*dst = colorIndex;
		dst += static_cast<int>(DirX);
		*dst = colorIndex;
		dst += static_cast<int>(DirX) + static_cast<int>(DirY) * pitch;
	}
	if (drawTipPixel)
		*dst = colorIndex;
}

int Width(int height)
{
	return 2 * height;
}

int Height(int width)
{
	return width / 2;
}

int HeightCeil(int width)
{
	return (width + 1) / 2;
}

template <DirectionX DirX, DirectionY DirY>
bool InDirectionBounds(const CelOutputBuffer &out, Point from)
{
	if (DirX == DirectionX::EAST)
		if (from.x >= out.w())
			return false;
	if (DirX == DirectionX::WEST)
		if (from.x < 0)
			return false;
	if (DirY == DirectionY::SOUTH)
		if (from.y >= out.h())
			return false;
	if (DirY == DirectionY::NORTH)
		if (from.y < 0)
			return false;
	return true;
}

template <DirectionX DirX, DirectionY DirY>
void DrawMapLine(const CelOutputBuffer &out, Point from, int height, std::uint8_t colorIndex)
{
	if (!InDirectionBounds<DirX, DirY>(out, from))
		return;

	bool drawFirstPixel = true; // skip the first pixel

	// First clip in the X direction. Allows for a 1-2 pixel overdraw, taken care of later.
	if (DirX == DirectionX::EAST) {
		if (from.x < 0) {
			int skip = -from.x;
			if (skip % 2 != 0) {
				drawFirstPixel = false;
				++skip;
				--height;
			}
			height -= Height(skip);
			from.x = 0;
			from.y += static_cast<int>(DirY) * Height(skip);
		}
		if (from.x + Width(height) > out.w()) {
			height = HeightCeil(out.w() - from.x);
		}
	} else {
		if (from.x >= out.w()) {
			int skip = from.x - out.w() + 1;
			if (skip % 2 != 0) {
				drawFirstPixel = false;
				++skip;
				--height;
			}
			height -= Height(skip);
			from.x = out.w() - 1;
			from.y += static_cast<int>(DirY) * Height(skip);
		}
		if (from.x < Width(height)) {
			height = HeightCeil(from.x + 1);
		}
	}

	if (DirY == DirectionY::SOUTH) {
		if (from.y < 0) {
			const int skip = -from.y;
			height -= skip;
			from.y = 0;
			from.x += static_cast<int>(DirX) * Width(skip);
		}
		if (from.y + height > out.h()) {
			height = out.h() - from.y;
		}
	} else {
		if (from.y >= out.h()) {
			const int skip = from.y - out.h() + 1;
			from.y = out.h() - 1;
			height -= skip;
			from.x += static_cast<int>(DirX) * Width(skip);
		}
		if (from.y < height) {
			height = from.y + 1;
		}
	}

	if (!InDirectionBounds<DirX, DirY>(out, from))
		return;

	const int overdrawX = DirX == DirectionX::EAST
	    ? from.x + Width(height) + 1 - out.w()
	    : Width(height) + 1 - from.x;

	const bool drawTipPixel = overdrawX != 1 && overdrawX != 2
	    && !((DirY == DirectionY::SOUTH && from.y + height == out.h()) || (DirY == DirectionY::NORTH && from.y + 1 - height == 0));
	const bool drawLastNonTipPixel = overdrawX != 2;

	if (!drawFirstPixel)
		SetPixel(out, { from.x + static_cast<int>(DirX), from.y }, colorIndex);
	if (!drawLastNonTipPixel)
		--height;
	if (height >= 0)
		UnsafeDrawMapLine<DirX, DirY>(out, from, height, drawTipPixel, colorIndex);
	if (!drawLastNonTipPixel) {
		SetPixel(
		    out,
		    { from.x + 2 * static_cast<int>(DirX) * (height),
		        from.y + static_cast<int>(DirY) * (height) },
		    colorIndex);
	}
}

template <DirectionX DirX, DirectionY DirY>
void DrawMapLine2(const CelOutputBuffer &out, Point from, int width, std::uint8_t colorIndex)
{
	// This is only used to draw small detail, so there is no unsafe version.
	// We bounds-check each pixel individually instead.
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
