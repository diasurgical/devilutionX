/**
 * @file quests.cpp
 *
 * Interface of functionality for handling quests.
 */
#pragma once

#include <stdint.h>

#include "engine.h"
#include "gendung.h"

namespace devilution {

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

struct QuestStruct {
	Uint8 _qlevel;
	Uint8 _qtype;
	quest_state _qactive;
	dungeon_type _qlvltype;
	Sint32 _qtx;
	Sint32 _qty;
	_setlevels _qslvl;
	Uint8 _qidx;
	Sint32 _qmsg;
	Uint8 _qvar1;
	Uint8 _qvar2;
	bool _qlog;
};

struct QuestData {
	Uint8 _qdlvl;
	Sint8 _qdmultlvl;
	dungeon_type _qlvlt;
	Uint8 _qdtype;
	Uint8 _qdrnd;
	_setlevels _qslvl;
	bool isSinglePlayerOnly;
	Sint32 _qdmsg;
	const char *_qlstr;
};

extern bool questlog;
extern BYTE *pQLogCel;
extern QuestStruct quests[MAXQUESTS];
extern int ReturnLvlX;
extern int ReturnLvlY;
extern dungeon_type ReturnLvlT;
extern int ReturnLvl;

void InitQuests();
void CheckQuests();
bool ForceQuests();
bool QuestStatus(int i);
void CheckQuestKill(int m, bool sendmsg);
void DRLG_CheckQuests(int x, int y);
void SetReturnLvlPos();
void GetReturnLvlPos();
void LoadPWaterPalette();
void ResyncMPQuests();
void ResyncQuests();
void DrawQuestLog(CelOutputBuffer out);
void StartQuestlog();
void QuestlogUp();
void QuestlogDown();
void QuestlogEnter();
void QuestlogESC();
void SetMultiQuest(int q, quest_state s, int l, int v1);

/* rdata */
extern QuestData questlist[];

}
