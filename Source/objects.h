/**
 * @file objects.h
 *
 * Interface of object functionality, interaction, spawning, loading, etc.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ObjectStruct {
	_object_id _otype;
	Sint32 _ox;
	Sint32 _oy;
	bool _oLight;
	Uint32 _oAnimFlag;
	unsigned char *_oAnimData;
	Sint32 _oAnimDelay; // Tick length of each frame in the current animation
	Sint32 _oAnimCnt;   // Increases by one each game tick, counting how close we are to _pAnimDelay
	Sint32 _oAnimLen;   // Number of frames in current animation
	Sint32 _oAnimFrame; // Current frame of animation.
	Sint32 _oAnimWidth;
	Sint32 _oAnimWidth2;
	bool _oDelFlag;
	Uint8 _oBreak;
	bool _oSolidFlag;
	bool _oMissFlag;
	Uint8 _oSelFlag;
	bool _oPreFlag;
	bool _oTrapFlag;
	bool _oDoorFlag;
	Sint32 _olid;
	Sint32 _oRndSeed;
	Sint32 _oVar1;
	Sint32 _oVar2;
	Sint32 _oVar3;
	Sint32 _oVar4;
	Sint32 _oVar5;
	Sint32 _oVar6;
	_speech_id _oVar7;
	Sint32 _oVar8;
} ObjectStruct;

extern int objectactive[MAXOBJECTS];
extern int nobjects;
extern int objectavail[MAXOBJECTS];
extern ObjectStruct object[MAXOBJECTS];
extern BOOL InitObjFlag;
extern BOOL LoadMapObjsFlag;

void InitObjectGFX();
void FreeObjectGFX();
void AddL1Objs(int x1, int y1, int x2, int y2);
void AddL2Objs(int x1, int y1, int x2, int y2);
void InitObjects();
void SetMapObjects(BYTE *pMap, int startx, int starty);
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
void OperateObject(int pnum, int i, BOOL TeleFlag);
void SyncOpObject(int pnum, int cmd, int i);
void BreakObject(int pnum, int oi);
void SyncBreakObj(int pnum, int oi);
void SyncObjectAnim(int o);
void GetObjectStr(int i);
void operate_lv24_lever();
void objects_454BA8();
void objects_rnd_454BEA();
bool objects_lv_24_454B04(int s);

#ifdef __cplusplus
}
#endif

}
