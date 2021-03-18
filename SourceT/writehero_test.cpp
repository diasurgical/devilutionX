#include <gtest/gtest.h>
#include "picosha2.h"
#include <fstream>
#include <vector>
#include <cstdio>
#include "all.h"
#include "paths.h"

using namespace dvl;

int spelldat_vanilla[] = {0, 1, 1, 4, 5, -1, 3, 3, 6, -1, 7, 6, 8, 9,
                          8, 9, -1, -1, -1, -1, 3, 11, -1, 14, -1, -1,
                          -1, -1, -1, 8, 1, 1, -1, 2, 1, 14, 9};

static void PackItemUnique(PkItemStruct *id, int idx)
{
	id->idx = idx;
	id->iCreateInfo = 0x2DE;
	id->bId = 1+2*ITEM_QUALITY_UNIQUE;
	id->bDur = 40;
	id->bMDur = 40;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x1C0C44B0;
}

static void PackItemStaff(PkItemStruct *id)
{
	id->idx = 150;
	id->iCreateInfo = 0x2010;
	id->bId = 1+2*ITEM_QUALITY_MAGIC;
	id->bDur = 75;
	id->bMDur = 75;
	id->bCh = 12;
	id->bMCh = 12;
	id->iSeed = 0x2A15243F;
}

static void PackItemBow(PkItemStruct *id)
{
	id->idx = 145;
	id->iCreateInfo = 0x0814;
	id->bId = 1+2*ITEM_QUALITY_MAGIC;
	id->bDur = 60;
	id->bMDur = 60;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x449D8992;
}

static void PackItemSword(PkItemStruct *id)
{
	id->idx = 122;
	id->iCreateInfo = 0x081E;
	id->bId = 1+2*ITEM_QUALITY_MAGIC;
	id->bDur = 60;
	id->bMDur = 60;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x680FAC02;
}

static void PackItemRing1(PkItemStruct *id)
{
	id->idx = 153;
	id->iCreateInfo = 0xDE;
	id->bId = 1+2*ITEM_QUALITY_MAGIC;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x5B41AFA8;
}

static void PackItemRing2(PkItemStruct *id)
{
	id->idx = 153;
	id->iCreateInfo = 0xDE;
	id->bId = 1+2*ITEM_QUALITY_MAGIC;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x1E41FEFC;
}

static void PackItemAmulet(PkItemStruct *id)
{
	id->idx = 155;
	id->iCreateInfo = 0xDE;
	id->bId = 1+2*ITEM_QUALITY_MAGIC;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x70A0383A;
}

static void PackItemArmor(PkItemStruct *id)
{
	id->idx = 70;
	id->iCreateInfo = 0xDE;
	id->bId = 1+2*ITEM_QUALITY_MAGIC;
	id->bDur = 90;
	id->bMDur = 90;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x63AAC49B;
}

static void PackItemFullRejuv(PkItemStruct *id, int i)
{
	const Uint32 seeds[] = {0x7C253335, 0x3EEFBFF8, 0x76AFB1A9, 0x38EB45FE, 0x1154E197, 0x5964B644, 0x76B58BEB, 0x002A6E5A};
	id->idx = ItemMiscIdIdx(IMISC_FULLREJUV);
	id->iSeed = seeds[i];
	id->iCreateInfo = 0;
	id->bId = 2*ITEM_QUALITY_NORMAL;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
}

static int PrepareInvSlot(PkPlayerStruct *pPack, int pos, int size, int start=0)
{
	static char ret = 0;
	if(start)
		ret = 0;
	++ret;
	if(size == 0) {
		pPack->InvGrid[pos] = ret;
	} else if(size == 1) {
		pPack->InvGrid[pos] = ret;
		pPack->InvGrid[pos-10] = -ret;
		pPack->InvGrid[pos-20] = -ret;
	} else if(size == 2) {
		pPack->InvGrid[pos] = ret;
		pPack->InvGrid[pos+1] = -ret;
		pPack->InvGrid[pos-10] = -ret;
		pPack->InvGrid[pos-10+1] = -ret;
		pPack->InvGrid[pos-20] = -ret;
		pPack->InvGrid[pos-20+1] = -ret;
	} else if(size == 3) {
		pPack->InvGrid[pos] = ret;
		pPack->InvGrid[pos+1] = -ret;
		pPack->InvGrid[pos-10] = -ret;
		pPack->InvGrid[pos-10+1] = -ret;
	} else {
		abort();
	}
	return ret - 1;
}

static void PackPlayerTest(PkPlayerStruct *pPack)
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
		PackItemFullRejuv(pPack->SpdList+i, i);
	for (auto i = 1; i < 37; i++) {
		if (spelldat_vanilla[i] != -1) {
			pPack->pMemSpells |= 1ULL << (i - 1);
			pPack->pSplLvl[i] = 15;
		}
	}
	for (auto i = 0; i < 7; i++)
		pPack->InvBody[i].idx = -1;
	strcpy(pPack->pName, "TestPlayer");
	pPack->pClass = PC_ROGUE;
	pPack->pBaseStr = 20 + 35;
	pPack->pBaseMag = 15 + 55;
	pPack->pBaseDex = 30 + 220;
	pPack->pBaseVit = 20 + 60;
	pPack->pHPBase = ((20+10)<<6)+((20+10)<<5) + 48*128 + (60<<6);
	pPack->pMaxHPBase = pPack->pHPBase;
	pPack->pManaBase = (15<<6)+(15<<5) + 48*128 + (55<<6);
	pPack->pMaxManaBase = pPack->pManaBase;

	PackItemUnique(pPack->InvBody+INVLOC_HEAD, 52);
	PackItemRing1(pPack->InvBody+INVLOC_RING_LEFT);
	PackItemRing2(pPack->InvBody+INVLOC_RING_RIGHT);
	PackItemAmulet(pPack->InvBody+INVLOC_AMULET);
	PackItemArmor(pPack->InvBody+INVLOC_CHEST);
	PackItemBow(pPack->InvBody+INVLOC_HAND_LEFT);

	PackItemStaff(pPack->InvList+PrepareInvSlot(pPack, 28, 2, 1));
	PackItemSword(pPack->InvList+PrepareInvSlot(pPack, 20, 1));

	pPack->_pNumInv = 2;
}

TEST(Writehero, pfile_write_hero) {
	SetPrefPath(".");
	std::remove("multi_0.sv");

	gbVanilla = true;
	gbIsHellfire = false;
	gbIsMultiplayer = true;
	gbIsHellfireSaveGame = false;

	myplr = 0;
	_uiheroinfo info{};
	strcpy(info.name, "TestPlayer");
	info.heroclass = PC_ROGUE;
	pfile_ui_save_create(&info);
	PkPlayerStruct pks;
	PackPlayerTest(&pks);
	UnPackPlayer(&pks, myplr, TRUE);
	pfile_write_hero();

	std::ifstream f("multi_0.sv", std::ios::binary);
	std::vector<unsigned char> s(picosha2::k_digest_size);
	picosha2::hash256(f, s.begin(), s.end());
	EXPECT_EQ(picosha2::bytes_to_hex_string(s.begin(), s.end()),
	          "08e9807d1281e4273268f4e265757b4429cfec7c3e8b6deb89dfa109d6797b1c");
}
