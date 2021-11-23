/**
 * @file quests.cpp
 *
 * Interface of functionality for handling quests.
 */
#pragma once

#include <cstdint>

#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "engine/point.hpp"
#include "gendung.h"
#include "monster.h"
#include "objdat.h"
#include "textdat.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

#define MAXQUESTS 24

/** States of the mushroom quest */
enum {
	QS_INIT,
	QS_TOMESPAWNED,
	QS_TOMEGIVEN,
	QS_MUSHSPAWNED,
	QS_MUSHPICKED,
	QS_MUSHGIVEN,
	QS_BRAINSPAWNED,
	QS_BRAINGIVEN,
};

enum quest_state : uint8_t {
	QUEST_NOTAVAIL, // quest did not spawn this game
	QUEST_INIT,     // quest has spawned, waiting to trigger
	QUEST_ACTIVE,   // quest is currently in progress
	QUEST_DONE,     // quest log closed and finished
	QUEST_HIVE_TEASE1 = 7,
	QUEST_HIVE_TEASE2,
	QUEST_HIVE_ACTIVE,
	QUEST_HIVE_DONE,
	QUEST_INVALID = 0xFF,
};

struct Quest {
	quest_id _qidx;
	quest_state _qactive;
	uint8_t _qlevel;
	Point position;
	dungeon_type _qlvltype;
	_setlevels _qslvl;
	bool _qlog;
	_speech_id _qmsg;
	uint8_t _qvar1;
	uint8_t _qvar2;

	bool IsAvailable();
};

struct QuestData {
	uint8_t _qdlvl;
	int8_t _qdmultlvl;
	dungeon_type _qlvlt;
	int8_t questBookOrder;
	uint8_t _qdrnd;
	_setlevels _qslvl;
	bool isSinglePlayerOnly;
	_speech_id _qdmsg;
	const char *_qlstr;
};

extern bool QuestLogIsOpen;
extern std::optional<CelSprite> pQLogCel;
extern Quest Quests[MAXQUESTS];
extern Point ReturnLvlPosition;
extern dungeon_type ReturnLevelType;
extern int ReturnLevel;

void InitQuests();

/**
 * @brief Deactivates quests from each quest pool at random to provide variety for single player games
 * @param seed The seed used to control which quests are deactivated
 * @param quests The available quest list, this function will make some of them inactive by the time it returns
 */
void InitialiseQuestPools(uint32_t seed, Quest quests[]);
void CheckQuests();
bool ForceQuests();
void CheckQuestKill(const Monster &monster, bool sendmsg);
void DRLG_CheckQuests(int x, int y);
void SetReturnLvlPos();
void GetReturnLvlPos();
void LoadPWaterPalette();
void UpdatePWaterPalette();
void ResyncMPQuests();
void ResyncQuests();
void DrawQuestLog(const Surface &out);
void StartQuestlog();
void QuestlogUp();
void QuestlogDown();
void QuestlogEnter();
void QuestlogESC();
void SetMultiQuest(int q, quest_state s, bool log, int v1);

/* rdata */
extern QuestData QuestsData[];

} // namespace devilution
