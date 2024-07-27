#include "player_test.h"

#include <gtest/gtest.h>

#include "cursor.h"
#include "init.h"
#include "playerdat.hpp"

using namespace devilution;

namespace devilution {
extern bool TestPlayerDoGotHit(Player &player);
}

int RunBlockTest(int frames, ItemSpecialEffect flags)
{
	Player &player = Players[0];

	player.numRecoveryFrames = frames;
	player.flags = flags;
	// StartPlrHit compares damage (a 6 bit fixed point value) to character level to determine if the player shrugs off the hit.
	// We don't initialise player so this comparison can't be relied on, instead we use forcehit to ensure the player enters hit mode
	StartPlrHit(player, 0, true);

	int i = 1;
	for (; i < 100; i++) {
		TestPlayerDoGotHit(player);
		if (player.mode != PM_GOTHIT)
			break;
		player.animationInfo.currentFrame++;
	}

	return i;
}

constexpr ItemSpecialEffect Normal = ItemSpecialEffect::None;
constexpr ItemSpecialEffect Balance = ItemSpecialEffect::FastHitRecovery;
constexpr ItemSpecialEffect Stability = ItemSpecialEffect::FasterHitRecovery;
constexpr ItemSpecialEffect Harmony = ItemSpecialEffect::FastestHitRecovery;
constexpr ItemSpecialEffect BalanceStability = Balance | Stability;
constexpr ItemSpecialEffect BalanceHarmony = Balance | Harmony;
constexpr ItemSpecialEffect StabilityHarmony = Stability | Harmony;

constexpr int Warrior = 6;
constexpr int Rogue = 7;
constexpr int Sorcerer = 8;

struct BlockTestCase {
	int expectedRecoveryFrame;
	int maxRecoveryFrame;
	ItemSpecialEffect itemFlags;
};

BlockTestCase BlockData[] = {
	{ 6, Warrior, Normal },
	{ 7, Rogue, Normal },
	{ 8, Sorcerer, Normal },

	{ 5, Warrior, Balance },
	{ 6, Rogue, Balance },
	{ 7, Sorcerer, Balance },

	{ 4, Warrior, Stability },
	{ 5, Rogue, Stability },
	{ 6, Sorcerer, Stability },

	{ 3, Warrior, Harmony },
	{ 4, Rogue, Harmony },
	{ 5, Sorcerer, Harmony },

	{ 4, Warrior, BalanceStability },
	{ 5, Rogue, BalanceStability },
	{ 6, Sorcerer, BalanceStability },

	{ 3, Warrior, BalanceHarmony },
	{ 4, Rogue, BalanceHarmony },
	{ 5, Sorcerer, BalanceHarmony },

	{ 3, Warrior, StabilityHarmony },
	{ 4, Rogue, StabilityHarmony },
	{ 5, Sorcerer, StabilityHarmony },
};

TEST(Player, PM_DoGotHit)
{
	Players.resize(1);
	MyPlayer = &Players[0];
	for (size_t i = 0; i < sizeof(BlockData) / sizeof(*BlockData); i++) {
		EXPECT_EQ(BlockData[i].expectedRecoveryFrame, RunBlockTest(BlockData[i].maxRecoveryFrame, BlockData[i].itemFlags));
	}
}

static void AssertPlayer(Player &player)
{
	ASSERT_EQ(CountU8(player.spellLevel, 64), 0);
	ASSERT_EQ(Count8(player.inventoryGrid, InventoryGridCells), 1);
	ASSERT_EQ(CountItems(player.bodySlot, NUM_INVLOC), 1);
	ASSERT_EQ(CountItems(player.inventorySlot, InventoryGridCells), 1);
	ASSERT_EQ(CountItems(player.beltSlot, MaxBeltItems), 2);
	ASSERT_EQ(CountItems(&player.heldItem, 1), 0);

	ASSERT_EQ(player.position.tile.x, 0);
	ASSERT_EQ(player.position.tile.y, 0);
	ASSERT_EQ(player.position.future.x, 0);
	ASSERT_EQ(player.position.future.y, 0);
	ASSERT_EQ(player.dungeonLevel, 0);
	ASSERT_EQ(player.destinationAction, 0);
	ASSERT_STREQ(player.name, "");
	ASSERT_EQ(player.heroClass, HeroClass::Rogue);
	ASSERT_EQ(player.baseStrength, 20);
	ASSERT_EQ(player.strength, 20);
	ASSERT_EQ(player.baseMagic, 15);
	ASSERT_EQ(player.magic, 15);
	ASSERT_EQ(player.baseDexterity, 30);
	ASSERT_EQ(player.dexterity, 30);
	ASSERT_EQ(player.baseVitality, 20);
	ASSERT_EQ(player.vitality, 20);
	ASSERT_EQ(player.getCharacterLevel(), 1);
	ASSERT_EQ(player.statPoints, 0);
	ASSERT_EQ(player.experience, 0);
	ASSERT_EQ(player.gold, 100);
	ASSERT_EQ(player.baseMaxLife, 2880);
	ASSERT_EQ(player.baseLife, 2880);
	ASSERT_EQ(player.getBaseToBlock(), 20);
	ASSERT_EQ(player.baseMaxMana, 1440);
	ASSERT_EQ(player.baseMana, 1440);
	ASSERT_EQ(player.learnedSpells, 0);
	ASSERT_EQ(player.numInventoryItems, 1);
	ASSERT_EQ(player.reflections, 0);
	ASSERT_EQ(player.townWarps, 0);
	ASSERT_EQ(player.dungeonMessages, 0);
	ASSERT_EQ(player.dungeonMessages2, 0);
	ASSERT_EQ(player.levelLoading, 0);
	ASSERT_EQ(player.difficultyCompletion, 0);
	ASSERT_EQ(player.hasManaShield, 0);
	ASSERT_EQ(player.hellfireFlags, ItemSpecialEffectHf::None);

	ASSERT_EQ(player.mode, 0);
	ASSERT_EQ(Count8(player.walkPath, MaxPathLength), 0);
	ASSERT_EQ(player.queuedSpell.spellId, SpellID::Null);
	ASSERT_EQ(player.queuedSpell.spellType, SpellType::Skill);
	ASSERT_EQ(player.queuedSpell.spellFrom, 0);
	ASSERT_EQ(player.inventorySpell, SpellID::Null);
	ASSERT_EQ(player.selectedSpell, SpellID::TrapDisarm);
	ASSERT_EQ(player.selectedSpellType, SpellType::Skill);
	ASSERT_EQ(player.skills, 134217728);
	ASSERT_EQ(player.scrollSpells, 0);
	ASSERT_EQ(player.spellFlags, SpellFlag::None);
	ASSERT_EQ(player.hasBlockFlag, 0);
	ASSERT_EQ(player.lightRadius, 10);
	ASSERT_EQ(player.damageModifier, 0);
	ASSERT_EQ(player.life, 2880);
	ASSERT_EQ(player.maxLife, 2880);
	ASSERT_EQ(player.mana, 1440);
	ASSERT_EQ(player.maxMana, 1440);
	ASSERT_EQ(player.getNextExperienceThreshold(), 2000);
	ASSERT_EQ(player.resistMagic, 0);
	ASSERT_EQ(player.resistFire, 0);
	ASSERT_EQ(player.resistLightning, 0);
	ASSERT_EQ(CountBool(player.isLevelVisted, NUMLEVELS), 0);
	ASSERT_EQ(CountBool(player.isSetLevelVisted, NUMLEVELS), 0);
	// This test case uses a Rogue, starting loadout is a short bow with damage 1-4
	ASSERT_EQ(player.minDamage, 1);
	ASSERT_EQ(player.maxDamage, 4);
	ASSERT_EQ(player.armorClass, 0);
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
}

TEST(Player, CreatePlayer)
{
	LoadCoreArchives();
	LoadGameArchives();

	// The tests need spawn.mpq or diabdat.mpq
	// Please provide them so that the tests can run successfully
	ASSERT_TRUE(HaveSpawn() || HaveDiabdat());

	LoadPlayerDataFiles();
	LoadItemData();
	Players.resize(1);
	CreatePlayer(Players[0], HeroClass::Rogue);
	AssertPlayer(Players[0]);
}
