#include <gtest/gtest.h>

#include "dead.h"
#include "engine.h"
#include "levels/gendung.h"

using namespace devilution;

TEST(Corpses, AddCorpse)
{
	AddCorpse({ 21, 48 }, 8, Direction::West);
	EXPECT_EQ(dCorpse[21][48], 8 + (static_cast<int>(Direction::West) << 5));
}

TEST(Corpses, AddCorpse_OOB)
{
	AddCorpse({ 21, 48 }, MaxCorpses + 1, Direction::West);
	EXPECT_EQ(dCorpse[21][48], 0 + (static_cast<int>(Direction::West) << 5));
}
