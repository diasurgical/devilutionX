//HEADER_GOES_HERE
#ifndef __TRIGS_H__
#define __TRIGS_H__

extern BOOL townwarps[3];
extern BOOL trigflag;
extern int numtrigs;
extern TriggerStruct trigs[MAXTRIGGERS];
extern int TWarpFrom;

void InitNoTriggers();
void InitTownTriggers();
void InitL1Triggers();
void InitL2Triggers();
void InitL3Triggers();
void InitL4Triggers();
void InitSKingTriggers();
void InitSChambTriggers();
void InitPWaterTriggers();
void InitVPTriggers();
BOOL ForceTownTrig();
BOOL ForceL1Trig();
BOOL ForceL2Trig();
BOOL ForceL3Trig();
BOOL ForceL4Trig();
void Freeupstairs();
BOOL ForceSKingTrig();
BOOL ForceSChambTrig();
BOOL ForcePWaterTrig();
void CheckTrigForce();
void CheckTriggers();

/* rdata */

extern int TownDownList[11];
extern int TownWarp1List[13];
extern int L1UpList[12];
extern int L1DownList[10];
extern int L2UpList[3];
extern int L2DownList[5];
extern int L2TWarpUpList[3];
extern int L3UpList[15];
extern int L3DownList[9];
extern int L3TWarpUpList[14];
extern int L4UpList[4];
extern int L4DownList[6];
extern int L4TWarpUpList[4];
extern int L4PentaList[33];

#endif /* __TRIGS_H__ */
