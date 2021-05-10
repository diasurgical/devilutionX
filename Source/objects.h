/**
 * @file objects.h
 *
 * Interface of object functionality, interaction, spawning, loading, etc.
 */
#pragma once

#include <cstdint>

#include "objdat.h"
#include "textdat.h"
#include "itemdat.h"

namespace devilution {

#define MAXOBJECTS 127

struct ObjectStruct {
	_object_id _otype;
	Point position;
	bool _oLight;
	uint32_t _oAnimFlag;
	byte *_oAnimData;
	int _oAnimDelay; // Tick length of each frame in the current animation
	int _oAnimCnt;   // Increases by one each game tick, counting how close we are to _pAnimDelay
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
	int _oRndSeed;
	int _oVar1;
	int _oVar2;
	int _oVar3;
	int _oVar4;
	int _oVar5;
	uint32_t _oVar6;
	_speech_id _oVar7;
	int _oVar8;
};

extern int objectactive[MAXOBJECTS];
extern int nobjects;
extern int objectavail[MAXOBJECTS];
extern ObjectStruct object[MAXOBJECTS];
extern bool InitObjFlag;
extern bool LoadMapObjsFlag;

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
void objects_44D8C5(_object_id ot, int v2, int ox, int oy);
void objects_44DA68(int a1, int a2);
void objects_454AF0(int a1, int a2, int a3);
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
void operate_lv24_lever();
void objects_454BA8();
void objects_rnd_454BEA();
bool objects_lv_24_454B04(int s);
bool objectIsDisabled(int i);

} // namespace devilution
