#include <gtest/gtest.h>

#include "engine/displacement.hpp"

namespace devilution {

TEST(MathTest, WorldScreenTransformation)
{
	Displacement offset = { 5, 2 };
	// Diablo renders tiles with the world origin translated to the top left of the screen, while the normal convention
	// has the screen origin at the bottom left. This means that we end up with negative offsets in screen space for
	// tiles in world space where x > y
	EXPECT_EQ(offset.worldToScreen(), Displacement(-96, -112));

	// Transformation should be reversable (as long as it's not truncating)
	EXPECT_EQ(offset.worldToScreen().screenToWorld(), offset);

	// Tiles with y >= x will still have a negative y coordinate in screen space
	offset = { 2, 5 };
	EXPECT_EQ(offset.worldToScreen(), Displacement(96, -112));

	// Most screen to world transformations will have a further displacement applied, this is a simple case of
	// selecting a tile on the edge of the world with the default origin
	Displacement cursorPosition = { 342, -150 };
	EXPECT_EQ(cursorPosition.screenToWorld(), Displacement(0, 10));

	// Screen > World transforms lose information, so cannot be reversed exactly using ints
	EXPECT_EQ(cursorPosition.screenToWorld().worldToScreen(), Displacement(320, -160));
}

} // namespace devilution
