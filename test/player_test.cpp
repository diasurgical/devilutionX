#include "player_test.h"

#include <gtest/gtest.h>

using namespace devilution;

namespace devilution {
extern bool TestPlayerDoGotHit(int pnum);
}

int RunBlockTest(int frames, int flags)
{
	int pnum = 0;
	auto &player = Players[pnum];

	player._pHFrames = frames;
	player._pIFlags = flags;
	StartPlrHit(pnum, 5, Direction::DIR_S);

	int i = 1;
	for (; i < 100; i++) {
		TestPlayerDoGotHit(pnum);
		if (player._pmode != PM_GOTHIT)
			break;
		player.AnimInfo.CurrentFrame++;
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

static void AssertPlayer(PlayerStruct &player)
{
	ASSERT_EQ(Count8(player._pSplLvl, 64), 0);
	ASSERT_EQ(Count8(player.InvGrid, NUM_INV_GRID_ELEM), 1);
	ASSERT_EQ(CountItems(player.InvBody, NUM_INVLOC), 1);
	ASSERT_EQ(CountItems(player.InvList, NUM_INV_GRID_ELEM), 1);
	ASSERT_EQ(CountItems(player.SpdList, MAXBELTITEMS), 2);
	ASSERT_EQ(CountItems(&player.HoldItem, 1), 1);

	ASSERT_EQ(player.position.tile.x, 0);
	ASSERT_EQ(player.position.tile.y, 0);
	ASSERT_EQ(player.position.future.x, 0);
	ASSERT_EQ(player.position.future.y, 0);
	ASSERT_EQ(player.plrlevel, 0);
	ASSERT_EQ(player.destAction, 0);
	ASSERT_STREQ(player._pName, "");
	ASSERT_EQ(player._pClass, HeroClass::Rogue);
	ASSERT_EQ(player._pBaseStr, 20);
	ASSERT_EQ(player._pStrength, 20);
	ASSERT_EQ(player._pBaseMag, 15);
	ASSERT_EQ(player._pMagic, 15);
	ASSERT_EQ(player._pBaseDex, 30);
	ASSERT_EQ(player._pDexterity, 30);
	ASSERT_EQ(player._pBaseVit, 20);
	ASSERT_EQ(player._pVitality, 20);
	ASSERT_EQ(player._pLevel, 1);
	ASSERT_EQ(player._pStatPts, 0);
	ASSERT_EQ(player._pExperience, 0);
	ASSERT_EQ(player._pGold, 100);
	ASSERT_EQ(player._pMaxHPBase, 2880);
	ASSERT_EQ(player._pHPBase, 2880);
	ASSERT_EQ(player._pBaseToBlk, 20);
	ASSERT_EQ(player._pMaxManaBase, 1440);
	ASSERT_EQ(player._pManaBase, 1440);
	ASSERT_EQ(player._pMemSpells, 0);
	ASSERT_EQ(player._pNumInv, 1);
	ASSERT_EQ(player.wReflections, 0);
	ASSERT_EQ(player.pTownWarps, 0);
	ASSERT_EQ(player.pDungMsgs, 0);
	ASSERT_EQ(player.pDungMsgs2, 0);
	ASSERT_EQ(player.pLvlLoad, 0);
	ASSERT_EQ(player.pDiabloKillLevel, 0);
	ASSERT_EQ(player.pBattleNet, 0);
	ASSERT_EQ(player.pManaShield, 0);
	ASSERT_EQ(player.pDifficulty, 0);
	ASSERT_EQ(player.pDamAcFlags, 0);

	ASSERT_EQ(player._pmode, 0);
	ASSERT_EQ(Count8(player.walkpath, MAX_PATH_LENGTH), 0);
	ASSERT_EQ(player._pSpell, 0);
	ASSERT_EQ(player._pSplType, 0);
	ASSERT_EQ(player._pSplFrom, 0);
	ASSERT_EQ(player._pTSpell, 0);
	ASSERT_EQ(player._pRSpell, 28);
	ASSERT_EQ(player._pRSplType, 0);
	ASSERT_EQ(player._pSBkSpell, 0);
	ASSERT_EQ(player._pAblSpells, 134217728);
	ASSERT_EQ(player._pScrlSpells, 0);
	ASSERT_EQ(player._pSpellFlags, 0);
	ASSERT_EQ(player._pBlockFlag, 0);
	ASSERT_EQ(player._pLightRad, 10);
	ASSERT_EQ(player._pDamageMod, 0);
	ASSERT_EQ(player._pHitPoints, 2880);
	ASSERT_EQ(player._pMaxHP, 2880);
	ASSERT_EQ(player._pMana, 1440);
	ASSERT_EQ(player._pMaxMana, 1440);
	ASSERT_EQ(player._pNextExper, 2000);
	ASSERT_EQ(player._pMagResist, 0);
	ASSERT_EQ(player._pFireResist, 0);
	ASSERT_EQ(player._pLghtResist, 0);
	ASSERT_EQ(CountBool(player._pLvlVisited, NUMLEVELS), 0);
	ASSERT_EQ(CountBool(player._pSLvlVisited, NUMLEVELS), 0);
	ASSERT_EQ(player._pIMinDam, 1);
	ASSERT_EQ(player._pIMaxDam, 1);
	ASSERT_EQ(player._pIAC, 0);
	ASSERT_EQ(player._pIBonusDam, 0);
	ASSERT_EQ(player._pIBonusToHit, 0);
	ASSERT_EQ(player._pIBonusAC, 0);
	ASSERT_EQ(player._pIBonusDamMod, 0);
	ASSERT_EQ(player._pISpells, 0);
	ASSERT_EQ(player._pIFlags, 0);
	ASSERT_EQ(player._pIGetHit, 0);
	ASSERT_EQ(player._pISplLvlAdd, 0);
	ASSERT_EQ(player._pISplDur, 0);
	ASSERT_EQ(player._pIEnAc, 0);
	ASSERT_EQ(player._pIFMinDam, 0);
	ASSERT_EQ(player._pIFMaxDam, 0);
	ASSERT_EQ(player._pILMinDam, 0);
	ASSERT_EQ(player._pILMaxDam, 0);
}

TEST(Player, CreatePlayer)
{
	CreatePlayer(0, HeroClass::Rogue);
    AssertPlayer(Players[0]);
}
