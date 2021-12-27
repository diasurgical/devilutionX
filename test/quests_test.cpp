#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "quests.h"

#include "objdat.h" // For quest IDs

namespace devilution {
void ResetQuests()
{
	for (auto &quest : Quests)
		quest._qactive = QUEST_INIT;
}

std::vector<quest_state> GetActiveFlagsForSlice(std::initializer_list<quest_id> ids)
{
	std::vector<quest_state> temp;

	for (auto id : ids)
		temp.push_back(Quests[id]._qactive);

	return temp;
}

TEST(QuestTest, SinglePlayerBadPools)
{
	ResetQuests();

	// (INT_MIN >> 16) % 2 = 0, so the times when the RNG calls GenerateRnd(2) don't end up with a negative value.
	InitialiseQuestPools(1457187811, Quests);
	EXPECT_EQ(Quests[Q_SKELKING]._qactive, QUEST_NOTAVAIL) << "Skeleton King quest is deactivated with 'bad' seed";
	ResetQuests();

	InitialiseQuestPools(988045466, Quests);
	EXPECT_THAT(GetActiveFlagsForSlice({ Q_BUTCHER, Q_LTBANNER, Q_GARBUD }), ::testing::Each(QUEST_INIT)) << "All quests in pool 2 remain active with 'bad' seed";
	ResetQuests();

	InitialiseQuestPools(4203210069U, Quests);
	EXPECT_THAT(GetActiveFlagsForSlice({ Q_BLIND, Q_ROCK, Q_BLOOD }), ::testing::Each(QUEST_INIT)) << "All quests in pool 3 remain active with 'bad' seed";
	ResetQuests();

	InitialiseQuestPools(2557708932U, Quests);
	EXPECT_THAT(GetActiveFlagsForSlice({ Q_MUSHROOM, Q_ZHAR, Q_ANVIL }), ::testing::Each(QUEST_INIT)) << "All quests in pool 4 remain active with 'bad' seed";
	ResetQuests();

	InitialiseQuestPools(1272442071, Quests);
	EXPECT_EQ(Quests[Q_VEIL]._qactive, QUEST_NOTAVAIL) << "Lachdan quest is deactivated with 'bad' seed";
}

TEST(QuestTest, SinglePlayerGoodPools)
{
	ResetQuests();

	InitialiseQuestPools(509604, Quests);
	EXPECT_EQ(Quests[Q_SKELKING]._qactive, QUEST_INIT) << "Expected Skeleton King quest to be available with the given seed";
	EXPECT_EQ(Quests[Q_PWATER]._qactive, QUEST_NOTAVAIL) << "Expected Poison Water quest to be deactivated with the given seed";

	EXPECT_EQ(Quests[Q_BUTCHER]._qactive, QUEST_INIT) << "Expected Butcher quest to be available with the given seed";
	EXPECT_EQ(Quests[Q_LTBANNER]._qactive, QUEST_INIT) << "Expected Ogden's Sign quest to be available with the given seed";
	EXPECT_EQ(Quests[Q_GARBUD]._qactive, QUEST_NOTAVAIL) << "Expected Gharbad the Weak quest to be deactivated with the given seed";

	EXPECT_EQ(Quests[Q_BLIND]._qactive, QUEST_INIT) << "Expected Halls of the Blind quest to be available with the given seed";
	EXPECT_EQ(Quests[Q_ROCK]._qactive, QUEST_NOTAVAIL) << "Expected Magic Rock quest to be deactivated with the given seed";
	EXPECT_EQ(Quests[Q_BLOOD]._qactive, QUEST_INIT) << "Expected Valor quest to be available with the given seed";

	EXPECT_EQ(Quests[Q_MUSHROOM]._qactive, QUEST_INIT) << "Expected Black Mushroom quest to be available with the given seed";
	EXPECT_EQ(Quests[Q_ZHAR]._qactive, QUEST_NOTAVAIL) << "Expected Zhar the Mad quest to be deactivated with the given seed";
	EXPECT_EQ(Quests[Q_ANVIL]._qactive, QUEST_INIT) << "Expected Anvil of Fury quest to be available with the given seed";

	EXPECT_EQ(Quests[Q_VEIL]._qactive, QUEST_NOTAVAIL) << "Expected Lachdanan quest to be deactivated with the given seed";
	EXPECT_EQ(Quests[Q_WARLORD]._qactive, QUEST_INIT) << "Expected Warlord of Blood quest to be available with the given seed";
}
} // namespace devilution
