#include "player_test.h"

#include <cstdint>
#include <cstdio>
#include <vector>

#include <SDL_endian.h>
#include <gtest/gtest.h>
#include <picosha2.h>

#include "cursor.h"
#include "init.h"
#include "loadsave.h"
#include "pack.h"
#include "pfile.h"
#include "playerdat.hpp"
#include "utils/file_util.h"
#include "utils/paths.h"

namespace devilution {
namespace {

constexpr int SpellDatVanilla[] = {
	0, 1, 1, 4, 5, -1, 3, 3, 6, -1, 7, 6, 8, 9,
	8, 9, -1, -1, -1, -1, 3, 11, -1, 14, -1, -1,
	-1, -1, -1, 8, 1, 1, -1, 2, 1, 14, 9
};

void SwapLE(ItemPack &pack)
{
	pack.iSeed = SDL_SwapLE32(pack.iSeed);
	pack.iCreateInfo = SDL_SwapLE16(pack.iCreateInfo);
	pack.idx = SDL_SwapLE16(pack.idx);
	pack.wValue = SDL_SwapLE16(pack.wValue);
	pack.dwBuff = SDL_SwapLE32(pack.dwBuff);
}

void SwapLE(PlayerPack &player)
{
	player.dwLowDateTime = SDL_SwapLE32(player.dwLowDateTime);
	player.dwHighDateTime = SDL_SwapLE32(player.dwHighDateTime);
	player.pExperience = SDL_SwapLE32(player.pExperience);
	player.pGold = SDL_SwapLE32(player.pGold);
	player.pHPBase = SDL_SwapLE32(player.pHPBase);
	player.pMaxHPBase = SDL_SwapLE32(player.pMaxHPBase);
	player.pManaBase = SDL_SwapLE32(player.pManaBase);
	player.pMaxManaBase = SDL_SwapLE32(player.pMaxManaBase);
	player.pMemSpells = SDL_SwapLE64(player.pMemSpells);
	for (ItemPack &item : player.bodySlot) {
		SwapLE(item);
	}
	for (ItemPack &item : player.inventorySlot) {
		SwapLE(item);
	}
	for (ItemPack &item : player.beltSlot) {
		SwapLE(item);
	}
	player.reflections = SDL_SwapLE16(player.reflections);
	player.difficultyCompletion = SDL_SwapLE32(player.difficultyCompletion);
	player.pDifficulty = SDL_SwapLE32(player.pDifficulty);
	player.hellfireFlags = SDL_SwapLE32(player.hellfireFlags);
}

void PackItemUnique(ItemPack *id, int idx)
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

void PackItemStaff(ItemPack *id)
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

void PackItemBow(ItemPack *id)
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

void PackItemSword(ItemPack *id)
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

void PackItemRing1(ItemPack *id)
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

void PackItemRing2(ItemPack *id)
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

void PackItemAmulet(ItemPack *id)
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

void PackItemArmor(ItemPack *id)
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

void PackItemFullRejuv(ItemPack *id, int i)
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

int PrepareInvSlot(PlayerPack *pPack, int pos, int size, int start = 0)
{
	static char ret = 0;
	if (start)
		ret = 0;
	++ret;
	if (size == 0) {
		pPack->inventoryGrid[pos] = ret;
	} else if (size == 1) {
		pPack->inventoryGrid[pos] = ret;
		pPack->inventoryGrid[pos - 10] = -ret;
		pPack->inventoryGrid[pos - 20] = -ret;
	} else if (size == 2) {
		pPack->inventoryGrid[pos] = ret;
		pPack->inventoryGrid[pos + 1] = -ret;
		pPack->inventoryGrid[pos - 10] = -ret;
		pPack->inventoryGrid[pos - 10 + 1] = -ret;
		pPack->inventoryGrid[pos - 20] = -ret;
		pPack->inventoryGrid[pos - 20 + 1] = -ret;
	} else if (size == 3) {
		pPack->inventoryGrid[pos] = ret;
		pPack->inventoryGrid[pos + 1] = -ret;
		pPack->inventoryGrid[pos - 10] = -ret;
		pPack->inventoryGrid[pos - 10 + 1] = -ret;
	} else {
		abort();
	}
	return ret - 1;
}

void PackPlayerTest(PlayerPack *pPack)
{
	memset(pPack, 0, 0x4F2);
	pPack->destinationAction = -1;
	pPack->destinationParam1 = 0;
	pPack->destinationParam2 = 0;
	pPack->dungeonLevel = 0;
	pPack->pExperience = 1583495809;
	pPack->pLevel = 50;
	pPack->px = 75;
	pPack->py = 68;
	pPack->targx = 75;
	pPack->targy = 68;
	pPack->pGold = 0;
	pPack->pStatPts = 0;
	pPack->difficultyCompletion = 3;
	for (auto i = 0; i < 40; i++)
		pPack->inventorySlot[i].idx = -1;
	for (auto i = 0; i < 7; i++)
		pPack->bodySlot[i].idx = -1;
	for (auto i = 0; i < MaxBeltItems; i++)
		PackItemFullRejuv(pPack->beltSlot + i, i);
	for (auto i = 1; i < 37; i++) {
		if (SpellDatVanilla[i] != -1) {
			pPack->pMemSpells |= 1ULL << (i - 1);
			pPack->pSplLvl[i] = 15;
		}
	}
	for (auto i = 0; i < 7; i++)
		pPack->bodySlot[i].idx = -1;
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

	PackItemUnique(pPack->bodySlot + INVLOC_HEAD, 52);
	PackItemRing1(pPack->bodySlot + INVLOC_RING_LEFT);
	PackItemRing2(pPack->bodySlot + INVLOC_RING_RIGHT);
	PackItemAmulet(pPack->bodySlot + INVLOC_AMULET);
	PackItemArmor(pPack->bodySlot + INVLOC_CHEST);
	PackItemBow(pPack->bodySlot + INVLOC_HAND_LEFT);

	PackItemStaff(pPack->inventorySlot + PrepareInvSlot(pPack, 28, 2, 1));
	PackItemSword(pPack->inventorySlot + PrepareInvSlot(pPack, 20, 1));

	pPack->numInventoryItems = 2;

	SwapLE(*pPack);
}

void AssertPlayer(Player &player)
{
	ASSERT_EQ(CountU8(player.spellLevel, 64), 23);
	ASSERT_EQ(Count8(player.inventoryGrid, InventoryGridCells), 9);
	ASSERT_EQ(CountItems(player.bodySlot, NUM_INVLOC), 6);
	ASSERT_EQ(CountItems(player.inventorySlot, InventoryGridCells), 2);
	ASSERT_EQ(CountItems(player.beltSlot, MaxBeltItems), 8);
	ASSERT_EQ(CountItems(&player.heldItem, 1), 0);

	ASSERT_EQ(player.position.tile.x, 75);
	ASSERT_EQ(player.position.tile.y, 68);
	ASSERT_EQ(player.position.future.x, 75);
	ASSERT_EQ(player.position.future.y, 68);
	ASSERT_EQ(player.dungeonLevel, 0);
	ASSERT_EQ(player.destinationAction, -1);
	ASSERT_STREQ(player.name, "TestPlayer");
	ASSERT_EQ(player.heroClass, HeroClass::Rogue);
	ASSERT_EQ(player.baseStrength, 55);
	ASSERT_EQ(player.strength, 124);
	ASSERT_EQ(player.baseMagic, 70);
	ASSERT_EQ(player.magic, 80);
	ASSERT_EQ(player.baseDexterity, 250);
	ASSERT_EQ(player.dexterity, 281);
	ASSERT_EQ(player.baseVitality, 80);
	ASSERT_EQ(player.vitality, 90);
	ASSERT_EQ(player.getCharacterLevel(), 50);
	ASSERT_EQ(player.statPoints, 0);
	ASSERT_EQ(player.experience, 1583495809);
	ASSERT_EQ(player.gold, 0);
	ASSERT_EQ(player.baseMaxLife, 12864);
	ASSERT_EQ(player.baseLife, 12864);
	ASSERT_EQ(player.getBaseToBlock(), 20);
	ASSERT_EQ(player.baseMaxMana, 11104);
	ASSERT_EQ(player.baseMana, 11104);
	ASSERT_EQ(player.learnedSpells, 66309357295);
	ASSERT_EQ(player.numInventoryItems, 2);
	ASSERT_EQ(player.reflections, 0);
	ASSERT_EQ(player.townWarps, 0);
	ASSERT_EQ(player.dungeonMessages, 0);
	ASSERT_EQ(player.dungeonMessages2, 0);
	ASSERT_EQ(player.levelLoading, 0);
	ASSERT_EQ(player.difficultyCompletion, 3);
	ASSERT_EQ(player.hasManaShield, 0);
	ASSERT_EQ(player.hellfireFlags, ItemSpecialEffectHf::None);

	ASSERT_EQ(player.mode, 0);
	ASSERT_EQ(Count8(player.walkPath, MaxPathLength), 25);
	ASSERT_EQ(player.graphic, 36);
	ASSERT_EQ(player.animationInfo.ticksPerFrame, 4);
	ASSERT_EQ(player.animationInfo.tickCounterOfCurrentFrame, 1);
	ASSERT_EQ(player.animationInfo.numberOfFrames, 20);
	ASSERT_EQ(player.animationInfo.currentFrame, 0);
	ASSERT_EQ(player.queuedSpell.spellId, SpellID::Invalid);
	ASSERT_EQ(player.queuedSpell.spellType, SpellType::Invalid);
	ASSERT_EQ(player.queuedSpell.spellFrom, 0);
	ASSERT_EQ(player.inventorySpell, SpellID::Null);
	ASSERT_EQ(player.selectedSpell, SpellID::Invalid);
	ASSERT_EQ(player.selectedSpellType, SpellType::Invalid);
	ASSERT_EQ(player.skills, 134217728);
	ASSERT_EQ(player.scrollSpells, 0);
	ASSERT_EQ(player.spellFlags, SpellFlag::None);
	ASSERT_TRUE(player.UsesRangedWeapon());
	ASSERT_EQ(player.hasBlockFlag, 0);
	ASSERT_EQ(player.lightRadius, 11);
	ASSERT_EQ(player.damageModifier, 101);
	ASSERT_EQ(player.life, 16640);
	ASSERT_EQ(player.maxLife, 16640);
	ASSERT_EQ(player.mana, 14624);
	ASSERT_EQ(player.maxMana, 14624);
	ASSERT_EQ(player.getNextExperienceThreshold(), 1583495809);
	ASSERT_EQ(player.resistMagic, 75);
	ASSERT_EQ(player.resistFire, 16);
	ASSERT_EQ(player.resistLightning, 75);
	ASSERT_EQ(CountBool(player.isLevelVisted, NUMLEVELS), 0);
	ASSERT_EQ(CountBool(player.isSetLevelVisted, NUMLEVELS), 0);
	ASSERT_EQ(player.numIdleFrames, 20);
	ASSERT_EQ(player.numWalkFrames, 8);
	ASSERT_EQ(player.numAttackFrames, 0);
	ASSERT_EQ(player.attackActionFrame, 0);
	ASSERT_EQ(player.numSpellFrames, 16);
	ASSERT_EQ(player.spellActionFrame, 12);
	ASSERT_EQ(player.numRecoveryFrames, 0);
	ASSERT_EQ(player.numDeathFrames, 20);
	ASSERT_EQ(player.numBlockFrames, 0);
	ASSERT_EQ(player.minDamage, 1);
	ASSERT_EQ(player.maxDamage, 14);
	ASSERT_EQ(player.armorClass, 115);
	ASSERT_EQ(player.bonusDamagePercent, 0);
	ASSERT_EQ(player.bonusToHit, 0);
	ASSERT_EQ(player.bonusArmorClass, 0);
	ASSERT_EQ(player.bonusDamage, 0);
	ASSERT_EQ(player.staffSpells, 0);
	ASSERT_EQ(player.flags, ItemSpecialEffect::None);
	ASSERT_EQ(player.damageFromEnemies, 0);
	ASSERT_EQ(player.bonusSpellLevel, 0);
	ASSERT_EQ(player.armorPierce, 0);
	ASSERT_EQ(player.minFireDamage, 0);
	ASSERT_EQ(player.maxFireDamage, 0);
	ASSERT_EQ(player.minLightningDamage, 0);
	ASSERT_EQ(player.maxLightningDamage, 0);
	ASSERT_EQ(player.originalCathedral, 0);
}

TEST(Writehero, pfile_write_hero)
{
	LoadCoreArchives();
	LoadGameArchives();

	// The tests need spawn.mpq or diabdat.mpq
	// Please provide them so that the tests can run successfully
	ASSERT_TRUE(HaveSpawn() || HaveDiabdat());

	paths::SetPrefPath(".");
	std::remove("multi_0.sv");

	gbVanilla = true;
	gbIsHellfire = false;
	gbIsSpawn = false;
	gbIsMultiplayer = true;
	gbIsHellfireSaveGame = false;
	leveltype = DTYPE_TOWN;
	giNumberOfLevels = 17;

	Players.resize(1);
	MyPlayerId = 0;
	MyPlayer = &Players[MyPlayerId];

	LoadSpellData();
	LoadPlayerDataFiles();
	LoadItemData();
	_uiheroinfo info {};
	info.heroclass = HeroClass::Rogue;
	pfile_ui_save_create(&info);
	PlayerPack pks;
	PackPlayerTest(&pks);
	UnPackPlayer(pks, *MyPlayer);
	AssertPlayer(Players[0]);
	pfile_write_hero();

	const char *path = "multi_0.sv";
	uintmax_t fileSize;
	ASSERT_TRUE(GetFileSize(path, &fileSize));
	size_t size = static_cast<size_t>(fileSize);
	FILE *f = std::fopen(path, "rb");
	ASSERT_TRUE(f != nullptr);
	std::unique_ptr<char[]> data { new char[size] };
	ASSERT_EQ(std::fread(data.get(), size, 1, f), 1);
	std::fclose(f);

	std::vector<unsigned char> s(picosha2::k_digest_size);
	picosha2::hash256(data.get(), data.get() + size, s.begin(), s.end());
	EXPECT_EQ(picosha2::bytes_to_hex_string(s.begin(), s.end()),
	    "a79367caae6192d54703168d82e0316aa289b2a33251255fad8abe34889c1d3a");
}

} // namespace
} // namespace devilution
