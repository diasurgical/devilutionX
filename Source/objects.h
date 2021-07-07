/**
 * @file objects.h
 *
 * Interface of object functionality, interaction, spawning, loading, etc.
 */
#pragma once

#include <cstdint>

#include "engine/point.hpp"
#include "itemdat.h"
#include "objdat.h"
#include "textdat.h"

namespace devilution {

#define MAXOBJECTS 127

struct ObjectStruct {
	_object_id _otype;
	Point position;
	bool _oLight;
	uint32_t _oAnimFlag;
	byte *_oAnimData;
	int _oAnimDelay;      // Tick length of each frame in the current animation
	int _oAnimCnt;        // Increases by one each game tick, counting how close we are to _pAnimDelay
	uint32_t _oAnimLen;   // Number of frames in current animation
	uint32_t _oAnimFrame; // Current frame of animation.
	int _oAnimWidth;
	bool _oDelFlag;
	int8_t _oBreak;
	bool _oSolidFlag;
	bool _oMissFlag;
	uint8_t _oSelFlag;
	bool _oPreFlag;
	bool _oTrapFlag;
	bool _oDoorFlag;
	int _olid;
	/**
	 * Saves the absolute value of the engine state (typically from a call to AdvanceRndSeed()) to later use when spawning items from a container object
	 * This is an unsigned value to avoid implementation defined behaviour when reading from this variable.
	 */
	uint32_t _oRndSeed;
	int _oVar1;
	int _oVar2;
	int _oVar3;
	int _oVar4;
	int _oVar5;
	uint32_t _oVar6;
	_speech_id _oVar7;
	int _oVar8;
};

extern ObjectStruct Objects[MAXOBJECTS];
extern int AvailableObjects[MAXOBJECTS];
extern int ActiveObjects[MAXOBJECTS];
extern int ActiveObjectCount;
extern bool ApplyObjectLighting;
extern bool LoadingMapObjects;

void InitObjectGFX();
void FreeObjectGFX();
void AddL1Objs(int x1, int y1, int x2, int y2);
void AddL2Objs(int x1, int y1, int x2, int y2);
void InitObjects();
void SetMapObjects(const uint16_t *dunData, int startx, int starty);
void SetObjMapRange(int i, int x1, int y1, int x2, int y2, int v);
void SetBookMsg(int i, _speech_id msg);
void GetRndObjLoc(int randarea, int *xx, int *yy);
void AddMushPatch();
void AddSlainHero();
void AddCryptBook(_object_id ot, int v2, int ox, int oy);
void AddCryptObject(int i, int a2);
void AddNakrulBook(int a1, int a2, int a3);
void AddObject(_object_id ot, int ox, int oy);
void Obj_Trap(int i);
void ProcessObjects();
void ObjSetMicro(int dx, int dy, int pn);
void RedoPlayerVision();
void MonstCheckDoors(int m);
void ObjChangeMap(int x1, int y1, int x2, int y2);
void ObjChangeMapResync(int x1, int y1, int x2, int y2);
void TryDisarm(int pnum, int i);
int ItemMiscIdIdx(item_misc_id imiscid);
void OperateObject(int pnum, int i, bool TeleFlag);
void SyncOpObject(int pnum, int cmd, int i);
void BreakObject(int pnum, int oi);
void SyncBreakObj(int pnum, int oi);
void SyncObjectAnim(int o);
void GetObjectStr(int i);
void OperateNakrulLever();
void SyncNakrulRoom();
void AddNakrulLeaver();
/**
 * @brief Checks whether the player is activating Na-Krul's spell tomes in the correct order
 *
 * Used as part of the final Diablo: Hellfire quest (from the hints provided to the player in the
 * reconstructed note). This function both updates the state of the variable that tracks progress
 * and also determines whether the spawn conditions are met (i.e. all tomes have been triggered
 * in the correct order).
 *
 * @param s the id of the spell tome
 * @return true if the player has activated all three tomes in the correct order, false otherwise
*/
bool OperateNakrulBook(int s);
bool objectIsDisabled(int i);

} // namespace devilution
