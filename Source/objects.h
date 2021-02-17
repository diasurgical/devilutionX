/**
 * @file objects.h
 *
 * Interface of object functionality, interaction, spawning, loading, etc.
 */
#ifndef __OBJECTS_H__
#define __OBJECTS_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

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
void SetBookMsg(int i, int msg);
void GetRndObjLoc(int randarea, int *xx, int *yy);
void AddMushPatch();
void AddSlainHero();
void objects_44D8C5(int ot, int v2, int ox, int oy);
void objects_44DA68(int a1, int a2);
void objects_454AF0(int a1, int a2, int a3);
void AddObject(int ot, int ox, int oy);
void Obj_Trap(int i);
void ProcessObjects();
void ObjSetMicro(int dx, int dy, int pn);
void RedoPlayerVision();
void MonstCheckDoors(int m);
void ObjChangeMap(int x1, int y1, int x2, int y2);
void ObjChangeMapResync(int x1, int y1, int x2, int y2);
void TryDisarm(int pnum, int i);
int ItemMiscIdIdx(int imiscid);
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

DEVILUTION_END_NAMESPACE

#endif /* __OBJECTS_H__ */
