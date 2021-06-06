#include <gtest/gtest.h>

#include "cursor.h"
#include "inv.h"
#include "player.h"

using namespace devilution;

bool TestGrid[5][5];

bool CheckNoSolid(int entity, Point position)
{
    SDL_Log("%ix%i", position.x, position.y);

	return true;//TestGrid[position.x][position.y];
}

TEST(Monster, LineClearSolid)
{
    SDL_Log("05");
	EXPECT_EQ(LineClear(CheckNoSolid, 0, { 0, 0 }, { 5, 1 }), true);
    SDL_Log("50");
	EXPECT_EQ(LineClear(CheckNoSolid, 0, { 5, 1 }, { 0, 0 }), true);
    SDL_Log("05,2");
	EXPECT_EQ(LineClear(CheckNoSolid, 0, { 0, 0 }, { 5, 2 }), true);
}
