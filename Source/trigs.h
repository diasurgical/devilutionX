/**
 * @file trigs.h
 *
 * Interface of functionality for triggering events when the player enters an area.
 */
#pragma once

namespace devilution {

struct TriggerStruct {
	int _tx;
	int _ty;
	interface_mode _tmsg;
	int _tlvl;
};

extern bool trigflag;
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
void Freeupstairs();
void CheckTrigForce();
void CheckTriggers();

}
