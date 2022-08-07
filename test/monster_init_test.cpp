#include <gtest/gtest.h>

#include "diablo.h"
#include "engine/random.hpp"
#include "lighting.h"
#include "monstdat.h"
#include "monster.h"
#include "multi.h"

using namespace devilution;

MonsterData MonsterDataCopy[MonstersDataSize];

void InitMonsterOld(Monster &monster, Direction rd, size_t typeIndex, Point position)
{
	monster.direction = rd;
	monster.position.tile = position;
	monster.position.future = position;
	monster.position.old = position;
	monster.levelType = typeIndex;
	monster.mode = MonsterMode::Stand;
	monster.animInfo = {};
	monster.changeAnimationData(MonsterGraphic::Stand);
	monster.animInfo.tickCounterOfCurrentFrame = GenerateRnd(monster.animInfo.ticksPerFrame - 1);
	monster.animInfo.currentFrame = GenerateRnd(monster.animInfo.numberOfFrames - 1);

	monster.level = monster.data().level;
	int maxhp = monster.data().hitPointsMinimum + GenerateRnd(monster.data().hitPointsMaximum - monster.data().hitPointsMinimum + 1);
	if (monster.type().type == MT_DIABLO && !gbIsHellfire) {
		maxhp /= 2;
		monster.level -= 15;
	}
	monster.maxHitPoints = maxhp << 6;

	if (!gbIsMultiplayer)
		monster.maxHitPoints = std::max(monster.maxHitPoints / 2, 64);

	monster.hitPoints = monster.maxHitPoints;
	monster.ai = monster.data().ai;
	monster.intelligence = monster.data().intelligence;
	monster.goal = MonsterGoal::Normal;
	monster.goalVar1 = 0;
	monster.goalVar2 = 0;
	monster.goalVar3 = 0;
	monster.pathCount = 0;
	monster.isInvalid = false;
	monster.uniqueType = UniqueMonsterType::None;
	monster.activeForTicks = 0;
	monster.lightId = NO_LIGHT; // BUGFIX monsters initial light id should be -1 (fixed)
	monster.rndItemSeed = AdvanceRndSeed();
	monster.aiSeed = AdvanceRndSeed();
	monster.whoHit = 0;
	monster.toHit = monster.data().toHit;
	monster.minDamage = monster.data().minDamage;
	monster.maxDamage = monster.data().maxDamage;
	monster.toHitSpecial = monster.data().toHitSpecial;
	monster.minDamageSpecial = monster.data().minDamageSpecial;
	monster.maxDamageSpecial = monster.data().maxDamageSpecial;
	monster.armorClass = monster.data().armorClass;
	monster.resistance = monster.data().resistance;
	monster.leader = Monster::NoLeader;
	monster.leaderRelation = LeaderRelation::None;
	monster.flags = monster.data().abilityFlags;
	monster.talkMsg = TEXT_NONE;

	if (monster.ai == AI_GARG) {
		monster.changeAnimationData(MonsterGraphic::Special);
		monster.animInfo.currentFrame = 0;
		monster.flags |= MFLAG_ALLOW_SPECIAL;
		monster.mode = MonsterMode::SpecialMeleeAttack;
	}

	if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
		monster.maxHitPoints = 3 * monster.maxHitPoints;
		if (gbIsHellfire)
			monster.maxHitPoints += (gbIsMultiplayer ? 100 : 50) << 6;
		else
			monster.maxHitPoints += 64;
		monster.hitPoints = monster.maxHitPoints;
		monster.level += 15;
		monster.toHit += NightmareToHitBonus;
		monster.minDamage = 2 * (monster.minDamage + 2);
		monster.maxDamage = 2 * (monster.maxDamage + 2);
		monster.toHitSpecial += NightmareToHitBonus;
		monster.minDamageSpecial = 2 * (monster.minDamageSpecial + 2);
		monster.maxDamageSpecial = 2 * (monster.maxDamageSpecial + 2);
		monster.armorClass += NightmareAcBonus;
	} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
		monster.maxHitPoints = 4 * monster.maxHitPoints;
		if (gbIsHellfire)
			monster.maxHitPoints += (gbIsMultiplayer ? 200 : 100) << 6;
		else
			monster.maxHitPoints += 192;
		monster.hitPoints = monster.maxHitPoints;
		monster.level += 30;
		monster.toHit += HellToHitBonus;
		monster.minDamage = 4 * monster.minDamage + 6;
		monster.maxDamage = 4 * monster.maxDamage + 6;
		monster.toHitSpecial += HellToHitBonus;
		monster.minDamageSpecial = 4 * monster.minDamageSpecial + 6;
		monster.maxDamageSpecial = 4 * monster.maxDamageSpecial + 6;
		monster.armorClass += HellAcBonus;
		monster.resistance = monster.data().resistanceHell;
	}
}

bool CompareMonsters(Monster &m1, Monster &m2)
{
	return m1.flags == m2.flags && m1.toHit == m2.toHit && m1.resistance == m2.resistance
	    && m1.talkMsg == m2.talkMsg && m1.goalVar1 == m2.goalVar1 && m1.goalVar2 == m2.goalVar2 && m1.goalVar3 == m2.goalVar3
	    && m1.goal == m2.goal && m1.levelType == m2.levelType && m1.mode == m2.mode && m1.pathCount == m2.pathCount
	    && m1.direction == m2.direction && m1.isInvalid == m2.isInvalid && m1.ai == m2.ai
	    && m1.intelligence == m2.intelligence && m1.activeForTicks == m2.activeForTicks
	    && m1.whoHit == m2.whoHit && m1.level == m2.level && m1.minDamage == m2.minDamage && m1.maxDamage == m2.maxDamage
	    && m1.minDamageSpecial == m2.minDamageSpecial && m1.maxDamageSpecial == m2.maxDamageSpecial && m1.armorClass == m2.armorClass
	    && m1.leader == m2.leader && m1.leaderRelation == m2.leaderRelation && m1.lightId == m2.lightId;
}

void CopyMonsterData(MonsterData destination[], MonsterData source[])
{
	for (size_t i = 0; i < MonstersDataSize; i++) {
		destination[i] = source[i];
	}
}

bool test_general(_difficulty testDifficulty)
{
	_difficulty lastDifficulty = sgGameInitInfo.nDifficulty;
	sgGameInitInfo.nDifficulty = testDifficulty;

	Monster monsters_old_init[MonstersDataSize];
	MonsterData currentMonstersData[MonstersDataSize];
	CopyMonsterData(currentMonstersData, MonstersData);
	CopyMonsterData(MonstersData, MonsterDataCopy);

	for (size_t i = 0; i < MonstersDataSize; i++) {
		LevelMonsterTypes[i % 24].data = &MonstersData[i];
		InitMonsterOld(monsters_old_init[i], Direction::South, i % 24, Point(0, 0));
	}

	CopyMonsterData(MonstersData, currentMonstersData);

	InitLevelMonsters();
	Monster *monsters_new_init[MonstersDataSize];
	InitMonstersData(testDifficulty, lastDifficulty);

	for (size_t i = 0; i < MonstersDataSize; i++) {
		LevelMonsterTypes[i % 24].data = &MonstersData[i];
		monsters_new_init[i] = AddMonster(Point(0, 0), Direction::South, i % 24, false);
		if (!CompareMonsters(*monsters_new_init[i], monsters_old_init[i]))
			return false;
	}

	return true;
}

TEST(MonsterInit, normal_difficulty)
{
	CopyMonsterData(MonsterDataCopy, MonstersData);
	EXPECT_TRUE(test_general(devilution::DIFF_NORMAL));
}

TEST(MonsterInit, nightmare_difficulty)
{
	EXPECT_TRUE(test_general(devilution::DIFF_NIGHTMARE));
}

TEST(MonsterInit, hell_difficulty)
{
	EXPECT_TRUE(test_general(devilution::DIFF_HELL));
}

// This simulates loading the game from hell difficulty to normal one. This is non-trivial and can be done wrong, hence this test.
TEST(MonsterInit, normal_difficulty_once_again)
{
	EXPECT_TRUE(test_general(devilution::DIFF_NORMAL));
}
