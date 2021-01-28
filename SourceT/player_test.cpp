#include <gtest/gtest.h>
#include "all.h"

namespace dvl {
extern int PM_DoGotHit(int pnum);
}

int RunBlockTest(bool hellfire, int frames, int flags)
{
	dvl::gbIsHellfire = hellfire;

	int pnum = 0;
	dvl::plr[pnum]._pAnimFrame = 1;
	dvl::plr[pnum]._pHFrames = frames;
	dvl::plr[pnum]._pVar8 = 1;
	dvl::plr[pnum]._pIFlags = flags;
	dvl::plr[pnum]._pmode = 7;
	dvl::plr[pnum]._pGFXLoad = -1;

	int i = 1;
	for (; i < 100; i++) {
		dvl::PM_DoGotHit(pnum);
		if (dvl::plr[pnum]._pmode != 7)
			break;
		dvl::plr[pnum]._pAnimFrame++;
	}

    return i;
}

#define NORM 0
#define BAL dvl::ISPL_FASTRECOVER
#define STA dvl::ISPL_FASTERRECOVER
#define HAR dvl::ISPL_FASTESTRECOVER
#define BALSTA (dvl::ISPL_FASTRECOVER | dvl::ISPL_FASTERRECOVER)
#define BALHAR (dvl::ISPL_FASTRECOVER | dvl::ISPL_FASTESTRECOVER)
#define STAHAR (dvl::ISPL_FASTERRECOVER | dvl::ISPL_FASTESTRECOVER)
#define ZEN (dvl::ISPL_FASTRECOVER | dvl::ISPL_FASTERRECOVER | dvl::ISPL_FASTESTRECOVER)
#define WAR 6
#define ROU 7
#define SRC 8

int BlockData[][4] = {
    { 6, false, WAR, NORM }, // D
    { 7, false, ROU, NORM }, // D
    { 8, false, SRC, NORM }, // D
    { 6, true, WAR, NORM }, // HF
    { 7, true, ROU, NORM }, // HF
    { 8, true, SRC, NORM }, // HF

    { 5, false, WAR, BAL }, // D
    { 6, false, ROU, BAL }, // D
    { 7, false, SRC, BAL }, // D
    { 5, true, WAR, BAL }, // HF
    { 6, true, ROU, BAL }, // HF
    { 7, true, SRC, BAL }, // HF

    { 4, false, WAR, STA }, // D
    { 5, false, ROU, STA }, // D
    { 6, false, SRC, STA }, // D
    { 4, true, WAR, STA }, // HF
    { 5, true, ROU, STA }, // HF
    { 6, true, SRC, STA }, // HF

    { 3, false, WAR, HAR }, // D
    { 4, false, ROU, HAR }, // D
    { 5, false, SRC, HAR }, // D
    { 3, true, WAR, HAR }, // HF
    { 4, true, ROU, HAR }, // HF
    { 5, true, SRC, HAR }, // HF

    { 4, false, WAR, BALSTA }, // D
    { 5, false, ROU, BALSTA }, // D
    { 6, false, SRC, BALSTA }, // D
    { 4, true, WAR, BALSTA }, // HF
    { 5, true, ROU, BALSTA }, // HF
    { 6, true, SRC, BALSTA }, // HF

    { 3, false, WAR, BALHAR }, // D
    { 4, false, ROU, BALHAR }, // D
    { 5, false, SRC, BALHAR }, // D
    { 3, true, WAR, BALHAR }, // HF
    { 4, true, ROU, BALHAR }, // HF
    { 5, true, SRC, BALHAR }, // HF

    { 3, false, WAR, STAHAR }, // D
    { 4, false, ROU, STAHAR }, // D
    { 5, false, SRC, STAHAR }, // D
    { 3, true, WAR, STAHAR }, // HF
    { 4, true, ROU, STAHAR }, // HF
    { 5, true, SRC, STAHAR }, // HF

    { 2, false, WAR, ZEN }, // D
    { 3, false, ROU, ZEN }, // D
    { 4, false, SRC, ZEN }, // D
    { 3, true, WAR, ZEN }, // HF
    { 4, true, ROU, ZEN }, // HF
    { 5, true, SRC, ZEN }, // HF
};

TEST(Player, PM_DoGotHit)
{
    for (int i = 0; i < sizeof(BlockData) / sizeof(*BlockData); i++) {
        EXPECT_EQ(BlockData[i][0], RunBlockTest(BlockData[i][1], BlockData[i][2], BlockData[i][3]));
    }
}
