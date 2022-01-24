#pragma once

#include "engine/render/text_render.hpp"

namespace devilution {

/** The number of spaces between left and right hints. */
constexpr int MidSpaces = 5;

/** Vertical distance between text lines. */
constexpr int LineHeight = 25;

/** Horizontal margin of the hints circle from panel edge. */
constexpr int CircleMarginX = 16;

/** Distance between the panel top and the circle top. */
constexpr int CircleTop = 101;

/** Spell icon side size. */
constexpr int IconSize = 37;

/** Spell icon text right margin. */
constexpr int IconSizeTextMarginRight = 3;

/** Spell icon text top margin. */
constexpr int IconSizeTextMarginTop = 2;

inline int SpaceWidth()
{
	static const int Result = GetLineWidth(" ");
	return Result;
}

struct CircleMenuHint {
	CircleMenuHint(bool isDpad, const char *top, const char *right, const char *bottom, const char *left)
	    : isDpad(isDpad)
	    , top(top)
	    , topW(GetLineWidth(top))
	    , right(right)
	    , rightW(GetLineWidth(right))
	    , bottom(bottom)
	    , bottomW(GetLineWidth(bottom))
	    , left(left)
	    , leftW(GetLineWidth(left))
	    , xMid(leftW + SpaceWidth() * MidSpaces / 2)
	{
	}

	CircleMenuHint()
	    : isDpad(false)
	    , top("")
	    , topW(0)
	    , right("")
	    , rightW(0)
	    , bottom("")
	    , bottomW(0)
	    , left("")
	    , leftW(0)
	    , xMid(0)
	{
	}

	[[nodiscard]] int Width() const
	{
		return 2 * xMid;
	}

	bool isDpad;

	const char *top;
	int topW;
	const char *right;
	int rightW;
	const char *bottom;
	int bottomW;
	const char *left;
	int leftW;

	int xMid;
};

} // namespace devilution
