#include "player_test.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <vector>

#include <gtest/gtest.h>
#include <picosha2.h>

#include "loadsave.h"
#include "pack.h"
#include "pfile.h"
#include "utils/paths.h"

using namespace devilution;

int spelldat_vanilla[] = {
	0, 1, 1, 4, 5, -1, 3, 3, 6, -1, 7, 6, 8, 9,
	8, 9, -1, -1, -1, -1, 3, 11, -1, 14, -1, -1,
	-1, -1, -1, 8, 1, 1, -1, 2, 1, 14, 9
};

static void PackItemUnique(ItemPack *id, int idx)
{
	id->idx = idx;
	id->iCreateInfo = 0x2DE;
	id->bId = 1 + 2 * ITEM_QUALITY_UNIQUE;
	id->bDur = 40;
	id->bMDur = 40;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x1C0C44B0;
}

static void PackItemStaff(ItemPack *id)
{
	id->idx = 150;
	id->iCreateInfo = 0x2010;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 75;
	id->bMDur = 75;
	id->bCh = 12;
	id->bMCh = 12;
	id->iSeed = 0x2A15243F;
}

static void PackItemBow(ItemPack *id)
{
	id->idx = 145;
	id->iCreateInfo = 0x0814;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 60;
	id->bMDur = 60;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x449D8992;
}

static void PackItemSword(ItemPack *id)
{
	id->idx = 122;
	id->iCreateInfo = 0x081E;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 60;
	id->bMDur = 60;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x680FAC02;
}

static void PackItemRing1(ItemPack *id)
{
	id->idx = 153;
	id->iCreateInfo = 0xDE;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x5B41AFA8;
}

static void PackItemRing2(ItemPack *id)
{
	id->idx = 153;
	id->iCreateInfo = 0xDE;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x1E41FEFC;
}

static void PackItemAmulet(ItemPack *id)
{
	id->idx = 155;
	id->iCreateInfo = 0xDE;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x70A0383A;
}

static void PackItemArmor(ItemPack *id)
{
	id->idx = 70;
	id->iCreateInfo = 0xDE;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 90;
	id->bMDur = 90;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x63AAC49B;
}

static void PackItemFullRejuv(ItemPack *id, int i)
{
	const uint32_t seeds[] = { 0x7C253335, 0x3EEFBFF8, 0x76AFB1A9, 0x38EB45FE, 0x1154E197, 0x5964B644, 0x76B58BEB, 0x002A6E5A };
	id->idx = ItemMiscIdIdx(IMISC_FULLREJUV);
	id->iSeed = seeds[i];
	id->iCreateInfo = 0;
	id->bId = 2 * ITEM_QUALITY_NORMAL;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
}

static int PrepareInvSlot(PlayerPack *pPack, int pos, int size, int start = 0)
{
	static char ret = 0;
	if (start)
		ret = 0;
	++ret;
	if (size == 0) {
		pPack->InvGrid[pos] = ret;
	} else if (size == 1) {
		pPack->InvGrid[pos] = ret;
		pPack->InvGrid[pos - 10] = -ret;
		pPack->InvGrid[pos - 20] = -ret;
	} else if (size == 2) {
		pPack->InvGrid[pos] = ret;
		pPack->InvGrid[pos + 1] = -ret;
		pPack->InvGrid[pos - 10] = -ret;
		pPack->InvGrid[pos - 10 + 1] = -ret;
		pPack->InvGrid[pos - 20] = -ret;
		pPack->InvGrid[pos - 20 + 1] = -ret;
	} else if (size == 3) {
		pPack->InvGrid[pos] = ret;
		pPack->InvGrid[pos + 1] = -ret;
		pPack->InvGrid[pos - 10] = -ret;
		pPack->InvGrid[pos - 10 + 1] = -ret;
	} else {
		abort();
	}
	return ret - 1;
}

static void PackPlayerTest(PlayerPack *pPack)
{
	memset(pPack, 0, 0x4F2);
	pPack->destAction = -1;
	pPack->destParam1 = 0;
	pPack->destParam2 = 0;
	pPack->plrlevel = 0;
	pPack->pExperience = 1583495809;
	pPack->pLevel = 50;
	pPack->px = 75;
	pPack->py = 68;
	pPack->targx = 75;
	pPack->targy = 68;
	pPack->pGold = 0;
	pPack->pStatPts = 0;
	pPack->pDiabloKillLevel = 3;
	for (auto i = 0; i < 40; i++)
		pPack->InvList[i].idx = -1;
	for (auto i = 0; i < 7; i++)
		pPack->InvBody[i].idx = -1;
	for (auto i = 0; i < MAXBELTITEMS; i++)
		PackItemFullRejuv(pPack->SpdList + i, i);
	for (auto i = 1; i < 37; i++) {
		if (spelldat_vanilla[i] != -1) {
			pPack->pMemSpells |= 1ULL << (i - 1);
			pPack->pSplLvl[i] = 15;
		}
	}
	for (auto i = 0; i < 7; i++)
		pPack->InvBody[i].idx = -1;
	strcpy(pPack->pName, "TestPlayer");
	pPack->pClass = static_cast<uint8_t>(HeroClass::Rogue);
	pPack->pBaseStr = 20 + 35;
	pPack->pBaseMag = 15 + 55;
	pPack->pBaseDex = 30 + 220;
	pPack->pBaseVit = 20 + 60;
	pPack->pHPBase = ((20 + 10) << 6) + ((20 + 10) << 5) + 48 * 128 + (60 << 6);
	pPack->pMaxHPBase = pPack->pHPBase;
	pPack->pManaBase = (15 << 6) + (15 << 5) + 48 * 128 + (55 << 6);
	pPack->pMaxManaBase = pPack->pManaBase;

	PackItemUnique(pPack->InvBody + INVLOC_HEAD, 52);
	PackItemRing1(pPack->InvBody + INVLOC_RING_LEFT);
	PackItemRing2(pPack->InvBody + INVLOC_RING_RIGHT);
	PackItemAmulet(pPack->InvBody + INVLOC_AMULET);
	PackItemArmor(pPack->InvBody + INVLOC_CHEST);
	PackItemBow(pPack->InvBody + INVLOC_HAND_LEFT);

	PackItemStaff(pPack->InvList + PrepareInvSlot(pPack, 28, 2, 1));
	PackItemSword(pPack->InvList + PrepareInvSlot(pPack, 20, 1));

	pPack->_pNumInv = 2;
}

static void AssertPlayer(Player &player)
{
	ASSERT_EQ(Count8(player._pSplLvl, 64), 23);
	ASSERT_EQ(Count8(player.InvGrid, NUM_INV_GRID_ELEM), 9);
	ASSERT_EQ(CountItems(player.InvBody, NUM_INVLOC), 6);
	ASSERT_EQ(CountItems(player.InvList, NUM_INV_GRID_ELEM), 2);
	ASSERT_EQ(CountItems(player.SpdList, MAXBELTITEMS), 8);
	ASSERT_EQ(CountItems(&player.HoldItem, 1), 1);

	ASSERT_EQ(player.position.tile.x, 75);
	ASSERT_EQ(player.position.tile.y, 68);
	ASSERT_EQ(player.position.future.x, 75);
	ASSERT_EQ(player.position.future.y, 68);
	ASSERT_EQ(player.plrlevel, 0);
	ASSERT_EQ(player.destAction, -1);
	ASSERT_STREQ(player._pName, "TestPlayer");
	ASSERT_EQ(player._pClass, HeroClass::Rogue);
	ASSERT_EQ(player._pBaseStr, 55);
	ASSERT_EQ(player._pStrength, 124);
	ASSERT_EQ(player._pBaseMag, 70);
	ASSERT_EQ(player._pMagic, 80);
	ASSERT_EQ(player._pBaseDex, 250);
	ASSERT_EQ(player._pDexterity, 281);
	ASSERT_EQ(player._pBaseVit, 80);
	ASSERT_EQ(player._pVitality, 90);
	ASSERT_EQ(player._pLevel, 50);
	ASSERT_EQ(player._pStatPts, 0);
	ASSERT_EQ(player._pExperience, 1583495809);
	ASSERT_EQ(player._pGold, 0);
	ASSERT_EQ(player._pMaxHPBase, 12864);
	ASSERT_EQ(player._pHPBase, 12864);
	ASSERT_EQ(player._pBaseToBlk, 20);
	ASSERT_EQ(player._pMaxManaBase, 11104);
	ASSERT_EQ(player._pManaBase, 11104);
	ASSERT_EQ(player._pMemSpells, 66309357295);
	ASSERT_EQ(player._pNumInv, 2);
	ASSERT_EQ(player.wReflections, 0);
	ASSERT_EQ(player.pTownWarps, 0);
	ASSERT_EQ(player.pDungMsgs, 0);
	ASSERT_EQ(player.pDungMsgs2, 0);
	ASSERT_EQ(player.pLvlLoad, 0);
	ASSERT_EQ(player.pDiabloKillLevel, 3);
	ASSERT_EQ(player.pBattleNet, 0);
	ASSERT_EQ(player.pManaShield, 0);
	ASSERT_EQ(player.pDifficulty, 0);
	ASSERT_EQ(player.pDamAcFlags, 0);

	ASSERT_EQ(player._pmode, 0);
	ASSERT_EQ(Count8(player.walkpath, MAX_PATH_LENGTH), 25);
	ASSERT_EQ(player._pgfxnum, 36);
	ASSERT_EQ(player.AnimInfo.TicksPerFrame, 4);
	ASSERT_EQ(player.AnimInfo.TickCounterOfCurrentFrame, 1);
	ASSERT_EQ(player.AnimInfo.NumberOfFrames, 20);
	ASSERT_EQ(player.AnimInfo.CurrentFrame, 1);
	ASSERT_EQ(player._pSpell, -1);
	ASSERT_EQ(player._pSplType, 4);
	ASSERT_EQ(player._pSplFrom, 0);
	ASSERT_EQ(player._pTSpell, 0);
	ASSERT_EQ(player._pRSpell, -1);
	ASSERT_EQ(player._pRSplType, 4);
	ASSERT_EQ(player._pSBkSpell, -1);
	ASSERT_EQ(player._pAblSpells, 134217728);
	ASSERT_EQ(player._pScrlSpells, 0);
	ASSERT_EQ(player._pSpellFlags, 0);
	ASSERT_TRUE(player.UsesRangedWeapon());
	ASSERT_EQ(player._pBlockFlag, 0);
	ASSERT_EQ(player._pLightRad, 11);
	ASSERT_EQ(player._pDamageMod, 101);
	ASSERT_EQ(player._pHitPoints, 16640);
	ASSERT_EQ(player._pMaxHP, 16640);
	ASSERT_EQ(player._pMana, 14624);
	ASSERT_EQ(player._pMaxMana, 14624);
	ASSERT_EQ(player._pNextExper, 1583495809);
	ASSERT_EQ(player._pMagResist, 75);
	ASSERT_EQ(player._pFireResist, 16);
	ASSERT_EQ(player._pLghtResist, 75);
	ASSERT_EQ(CountBool(player._pLvlVisited, NUMLEVELS), 0);
	ASSERT_EQ(CountBool(player._pSLvlVisited, NUMLEVELS), 0);
	ASSERT_EQ(player._pNFrames, 20);
	ASSERT_EQ(player._pWFrames, 8);
	ASSERT_EQ(player._pAFrames, 0);
	ASSERT_EQ(player._pAFNum, 0);
	ASSERT_EQ(player._pSFrames, 16);
	ASSERT_EQ(player._pSFNum, 12);
	ASSERT_EQ(player._pHFrames, 0);
	ASSERT_EQ(player._pDFrames, 20);
	ASSERT_EQ(player._pBFrames, 0);
	ASSERT_EQ(player._pIMinDam, 1);
	ASSERT_EQ(player._pIMaxDam, 14);
	ASSERT_EQ(player._pIAC, 115);
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
	ASSERT_EQ(player.pOriginalCathedral, 0);
}

TEST(Writehero, pfile_write_hero)
{
	paths::SetPrefPath(".");
	std::remove("multi_0.sv");

	gbVanilla = true;
	gbIsHellfire = false;
	gbIsMultiplayer = true;
	gbIsHellfireSaveGame = false;
	leveltype = DTYPE_TOWN;
	giNumberOfLevels = 17;

	MyPlayerId = 0;
	MyPlayer = &Players[MyPlayerId];
	*MyPlayer = {};

	_uiheroinfo info {};
	strcpy(info.name, "TestPlayer");
	info.heroclass = HeroClass::Rogue;
	pfile_ui_save_create(&info);
	PlayerPack pks;
	PackPlayerTest(&pks);
	UnPackPlayer(&pks, Players[MyPlayerId], true);
	AssertPlayer(Players[0]);
	pfile_write_hero();

	std::ifstream f("multi_0.sv", std::ios::binary);
	std::vector<unsigned char> s(picosha2::k_digest_size);
	picosha2::hash256(f, s.begin(), s.end());
	EXPECT_EQ(picosha2::bytes_to_hex_string(s.begin(), s.end()),
	    "a79367caae6192d54703168d82e0316aa289b2a33251255fad8abe34889c1d3a");
}
