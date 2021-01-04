/**
 * @file quests.cpp
 *
 * Interface of functionality for handling quests.
 */
#ifndef __QUESTS_H__
#define __QUESTS_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern bool allquests;
extern BOOL questlog;
extern BYTE *pQLogCel;
extern QuestStruct quests[MAXQUESTS];
extern int ReturnLvlX;
extern int ReturnLvlY;
extern int ReturnLvlT;
extern int ReturnLvl;

void InitQuests();
void CheckQuests();
BOOL ForceQuests();
BOOL QuestStatus(int i);
void CheckQuestKill(int m, BOOL sendmsg);
void DRLG_CheckQuests(int x, int y);
void SetReturnLvlPos();
void GetReturnLvlPos();
void ResyncMPQuests();
void ResyncQuests();
void DrawQuestLog();
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
