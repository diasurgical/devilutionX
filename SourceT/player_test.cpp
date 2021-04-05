#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

namespace dvl {
extern int PM_DoGotHit(int pnum);
}

int RunBlockTest(int frames, int flags)
{
	int pnum = 0;
	plr[pnum]._pAnimFrame = 1;
	plr[pnum]._pHFrames = frames;
	plr[pnum]._pVar8 = 1;
	plr[pnum]._pIFlags = flags;
	plr[pnum]._pmode = PM_GOTHIT;
	plr[pnum]._pGFXLoad = -1;

	int i = 1;
	for (; i < 100; i++) {
		PM_DoGotHit(pnum);
		if (plr[pnum]._pmode != PM_GOTHIT)
			break;
		plr[pnum]._pAnimFrame++;
	}

	return i;
}

#define NORM 0
#define BAL ISPL_FASTRECOVER
#define STA ISPL_FASTERRECOVER
#define HAR ISPL_FASTESTRECOVER
#define BALSTA (ISPL_FASTRECOVER | ISPL_FASTERRECOVER)
#define BALHAR (ISPL_FASTRECOVER | ISPL_FASTESTRECOVER)
#define STAHAR (ISPL_FASTERRECOVER | ISPL_FASTESTRECOVER)
#define ZEN (ISPL_FASTRECOVER | ISPL_FASTERRECOVER | ISPL_FASTESTRECOVER)
#define WAR 6
#define ROU 7
#define SRC 8

int BlockData[][3] = {
	{ 6, WAR, NORM },
	{ 7, ROU, NORM },
	{ 8, SRC, NORM },

	{ 5, WAR, BAL },
	{ 6, ROU, BAL },
	{ 7, SRC, BAL },

	{ 4, WAR, STA },
	{ 5, ROU, STA },
	{ 6, SRC, STA },

	{ 3, WAR, HAR },
	{ 4, ROU, HAR },
	{ 5, SRC, HAR },

	{ 4, WAR, BALSTA },
	{ 5, ROU, BALSTA },
	{ 6, SRC, BALSTA },

	{ 3, WAR, BALHAR },
	{ 4, ROU, BALHAR },
	{ 5, SRC, BALHAR },

	{ 3, WAR, STAHAR },
	{ 4, ROU, STAHAR },
	{ 5, SRC, STAHAR },

	{ 2, WAR, ZEN },
	{ 3, ROU, ZEN },
	{ 4, SRC, ZEN },
};

TEST(Player, PM_DoGotHit)
{
	for (size_t i = 0; i < sizeof(BlockData) / sizeof(*BlockData); i++) {
		EXPECT_EQ(BlockData[i][0], RunBlockTest(BlockData[i][1], BlockData[i][2]));
	}
}
