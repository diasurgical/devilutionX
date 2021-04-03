/**
 * @file quests.cpp
 *
 * Interface of functionality for handling quests.
 */
#ifndef __QUESTS_H__
#define __QUESTS_H__

#include "engine.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct QuestStruct {
	Uint8 _qlevel;
	Uint8 _qtype;
	Uint8 _qactive;
	dungeon_type _qlvltype;
	Sint32 _qtx;
	Sint32 _qty;
	Uint8 _qslvl;
	Uint8 _qidx;
	Sint32 _qmsg;
	Uint8 _qvar1;
	Uint8 _qvar2;
	bool _qlog;
} QuestStruct;

typedef struct QuestData {
	Uint8 _qdlvl;
	Sint8 _qdmultlvl;
	dungeon_type _qlvlt;
	Uint8 _qdtype;
	Uint8 _qdrnd;
	Uint8 _qslvl;
	Uint32 _qflags; /* unsigned char */
	Sint32 _qdmsg;
	const char *_qlstr;
} QuestData;

extern bool questlog;
extern BYTE *pQLogCel;
extern QuestStruct quests[MAXQUESTS];
extern int ReturnLvlX;
extern int ReturnLvlY;
extern dungeon_type ReturnLvlT;
extern int ReturnLvl;

void InitQuests();
void CheckQuests();
BOOL ForceQuests();
BOOL QuestStatus(int i);
void CheckQuestKill(int m, BOOL sendmsg);
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
void SetMultiQuest(int q, int s, int l, int v1);

/* rdata */
extern QuestData questlist[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __QUESTS_H__ */
