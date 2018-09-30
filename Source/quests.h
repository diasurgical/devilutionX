//HEADER_GOES_HERE
#ifndef __QUESTS_H__
#define __QUESTS_H__

extern int qtopline; // idb
extern int questlog; // weak
extern void *pQLogCel;
extern QuestStruct quests[MAXQUESTS];
extern int qline; // weak
extern int qlist[MAXQUESTS];
extern int numqlines; // weak
extern int WaterDone; // idb
extern int ReturnLvlY; // idb
extern int ReturnLvlX; // idb
extern int ReturnLvlT; // idb
extern int ALLQUESTS; // idb
extern int ReturnLvl; // idb

void __cdecl InitQuests();
void __cdecl CheckQuests();
bool __cdecl ForceQuests();
BOOL __fastcall QuestStatus(int i);
void __fastcall CheckQuestKill(int m, unsigned char sendmsg);
void __cdecl DrawButcher();
void __fastcall DrawSkelKing(int q, int x, int y);
void __fastcall DrawWarLord(int x, int y);
void __fastcall DrawSChamber(int q, int x, int y);
void __fastcall DrawLTBanner(int x, int y);
void __fastcall DrawBlind(int x, int y);
void __fastcall DrawBlood(int x, int y);
void __fastcall DRLG_CheckQuests(int x, int y);
void __cdecl SetReturnLvlPos();
void __cdecl GetReturnLvlPos();
void __cdecl ResyncMPQuests();
void __cdecl ResyncQuests();
void __fastcall PrintQLString(int x, int y, unsigned char cjustflag, char *str, int col);
void __cdecl DrawQuestLog();
void __cdecl StartQuestlog();
void __cdecl QuestlogUp();
void __cdecl QuestlogDown();
void __cdecl QuestlogEnter();
void __cdecl QuestlogESC();
void __fastcall SetMultiQuest(int q, int s, unsigned char l, int v1);

/* rdata */
extern QuestData questlist[MAXQUESTS];
extern char questxoff[7];
extern char questyoff[7];
extern char *questtrigstr[5];
extern int QuestGroup1[3];
extern int QuestGroup2[3];
extern int QuestGroup3[3];
extern int QuestGroup4[2];

#endif /* __QUESTS_H__ */
