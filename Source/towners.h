//HEADER_GOES_HERE
#ifndef __TOWNERS_H__
#define __TOWNERS_H__

extern int storeflag; // weak
extern int sgnCowMsg; // weak
extern int numtowners; // idb
extern int sgdwCowClicks; // weak
extern int bannerflag; // weak // unused 0x6AAC28
extern int boyloadflag; // weak
extern void *pCowCels; // idb
extern TownerStruct towner[16];

int __fastcall GetActiveTowner(int t);
void __fastcall SetTownerGPtrs(void *pData, void **pAnim); /* unsigned char *+** */
void __fastcall NewTownerAnim(int tnum, unsigned char *pAnim, int numFrames, int Delay);
void __fastcall InitTownerInfo(int i, int w, bool sel, int t, int x, int y, int ao, int tp);
void __fastcall InitQstSnds(int i);
void __cdecl InitSmith();
void __cdecl InitBarOwner();
void __cdecl InitTownDead();
void __cdecl InitWitch();
void __cdecl InitBarmaid();
void __cdecl InitBoy();
void __cdecl InitHealer();
void __cdecl InitTeller();
void __cdecl InitDrunk();
void __cdecl InitCows();
void __cdecl InitTowners();
void __cdecl FreeTownerGFX();
void __fastcall TownCtrlMsg(int i);
void __cdecl TownBlackSmith();
void __cdecl TownBarOwner();
void __cdecl TownDead();
void __cdecl TownHealer();
void __cdecl TownStory();
void __cdecl TownDrunk();
void __cdecl TownBoy();
void __cdecl TownWitch();
void __cdecl TownBarMaid();
void __cdecl TownCow();
void __cdecl ProcessTowners();
ItemStruct *__fastcall PlrHasItem(int pnum, int item, int *i);
void __fastcall TownerTalk(int t);
void __fastcall TalkToTowner(int p, int t);
void __fastcall CowSFX(int pnum);

/* rdata */

extern const int snSFX[3][3];

/* data */

extern char AnimOrder[6][148];
extern int TownCowX[3];
extern int TownCowY[3];
extern int TownCowDir[3];
extern int cowoffx[8];
extern int cowoffy[8];
extern QuestTalkData Qtalklist[11];
extern int CowPlaying;

#endif /* __TOWNERS_H__ */
