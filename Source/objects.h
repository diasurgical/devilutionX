//HEADER_GOES_HERE
#ifndef __OBJECTS_H__
#define __OBJECTS_H__

extern int trapid; // weak
extern int trapdir; // weak
extern unsigned char *pObjCels[40];
extern char ObjFileList[40];
extern int objectactive[MAXOBJECTS];
extern int nobjects; // idb
extern int leverid; // idb
extern int objectavail[MAXOBJECTS];
extern ObjectStruct object[MAXOBJECTS];
extern int InitObjFlag; // weak
extern int numobjfiles; // weak

void __cdecl InitObjectGFX();
void __cdecl FreeObjectGFX();
bool __fastcall RndLocOk(int xp, int yp);
void __fastcall InitRndLocObj(int min, int max, int objtype);
void __fastcall InitRndLocBigObj(int min, int max, int objtype);
void __fastcall InitRndLocObj5x5(int min, int max, int objtype);
void __cdecl ClrAllObjects();
void __cdecl AddTortures();
void __cdecl AddCandles();
void __fastcall AddBookLever(int lx1, int ly1, int lx2, int ly2, int x1, int y1, int x2, int y2, int msg);
void __cdecl InitRndBarrels();
void __fastcall AddL1Objs(int x1, int y1, int x2, int y2);
void __fastcall AddL2Objs(int x1, int y1, int x2, int y2);
void __fastcall AddL3Objs(int x1, int y1, int x2, int y2);
bool __fastcall WallTrapLocOk(int xp, int yp);
void __cdecl AddL2Torches();
bool __fastcall TorchLocOK(int xp, int yp);
void __cdecl AddObjTraps();
void __cdecl AddChestTraps();
void __fastcall LoadMapObjects(unsigned char *pMap, int startx, int starty, int x1, int y1, int w, int h, int leveridx);
void __fastcall LoadMapObjs(unsigned char *pMap, int startx, int starty);
void __cdecl AddDiabObjs();
void __cdecl AddStoryBooks();
void __fastcall AddHookedBodies(int freq);
void __cdecl AddL4Goodies();
void __cdecl AddLazStand();
void __cdecl InitObjects();
void __fastcall SetMapObjects(unsigned char *pMap, int startx, int starty);
void __fastcall DeleteObject(int oi, int i);
void __fastcall SetupObject(int i, int x, int y, int ot);
void __fastcall SetObjMapRange(int i, int x1, int y1, int x2, int y2, int v);
void __fastcall SetBookMsg(int i, int msg);
void __fastcall AddL1Door(int i, int x, int y, int ot);
void __fastcall AddSCambBook(int i);
void __fastcall AddChest(int i, int t);
void __fastcall AddL2Door(int i, int x, int y, int ot);
void __fastcall AddL3Door(int i, int x, int y, int ot);
void __fastcall AddSarc(int i);
void __fastcall AddFlameTrap(int i);
void __fastcall AddFlameLvr(int i);
void __fastcall AddTrap(int i);
void __fastcall AddObjLight(int i, int r);
void __fastcall AddBarrel(int i);
void __fastcall AddShrine(int i);
void __fastcall AddBookcase(int i);
void __fastcall AddPurifyingFountain(int i);
void __fastcall AddArmorStand(int i);
void __fastcall AddDecap(int i);
void __fastcall AddVilebook(int i);
void __fastcall AddMagicCircle(int i);
void __fastcall AddBookstand(int i);
void __fastcall AddPedistal(int i);
void __fastcall AddStoryBook(int i);
void __fastcall AddWeaponRack(int i);
void __fastcall AddTorturedBody(int i);
void __fastcall GetRndObjLoc(int randarea, int *xx, int *yy);
void __cdecl AddMushPatch();
void __cdecl AddSlainHero();
void __fastcall AddObject(int ot, int ox, int oy);
void __fastcall Obj_Light(int i, int lr);
void __fastcall Obj_Circle(int i);
void __fastcall Obj_StopAnim(int i);
void __fastcall Obj_Door(int i);
void __fastcall Obj_Sarc(int i);
void __fastcall ActivateTrapLine(int ttype, int tid);
void __fastcall Obj_FlameTrap(int i);
void __fastcall Obj_Trap(int i);
void __fastcall Obj_BCrossDamage(int i);
void __cdecl ProcessObjects();
void __fastcall ObjSetMicro(int dx, int dy, int pn);
void __fastcall objects_set_door_piece(int x, int y);
void __fastcall ObjSetMini(int x, int y, int v);
void __fastcall ObjL1Special(int x1, int y1, int x2, int y2);
void __fastcall ObjL2Special(int x1, int y1, int x2, int y2);
void __fastcall DoorSet(int oi, int dx, int dy);
void __cdecl RedoPlayerVision();
void __fastcall OperateL1RDoor(int pnum, int oi, unsigned char sendflag);
void __fastcall OperateL1LDoor(int pnum, int oi, unsigned char sendflag);
void __fastcall OperateL2RDoor(int pnum, int oi, unsigned char sendflag);
void __fastcall OperateL2LDoor(int pnum, int oi, unsigned char sendflag);
void __fastcall OperateL3RDoor(int pnum, int oi, unsigned char sendflag);
void __fastcall OperateL3LDoor(int pnum, int oi, unsigned char sendflag);
void __fastcall MonstCheckDoors(int m);
void __fastcall ObjChangeMap(int x1, int y1, int x2, int y2);
void __fastcall ObjChangeMapResync(int x1, int y1, int x2, int y2);
void __fastcall OperateL1Door(int pnum, int i, unsigned char sendflag);
void __fastcall OperateLever(int pnum, int i);
void __fastcall OperateBook(int pnum, int i);
void __fastcall OperateBookLever(int pnum, int i);
void __fastcall OperateSChambBk(int pnum, int i);
void __fastcall OperateChest(int pnum, int i, unsigned char sendmsg);
void __fastcall OperateMushPatch(int pnum, int i);
void __fastcall OperateInnSignChest(int pnum, int i);
void __fastcall OperateSlainHero(int pnum, int i, unsigned char sendmsg);
void __fastcall OperateTrapLvr(int i);
void __fastcall OperateSarc(int pnum, int i, unsigned char sendmsg);
void __fastcall OperateL2Door(int pnum, int i, unsigned char sendflag);
void __fastcall OperateL3Door(int pnum, int i, unsigned char sendflag);
void __fastcall OperatePedistal(int pnum, int i);
void __fastcall TryDisarm(int pnum, int i);
int __fastcall ItemMiscIdIdx(int imiscid);
void __fastcall OperateShrine(int pnum, int i, int sType);
void __fastcall OperateSkelBook(int pnum, int i, unsigned char sendmsg);
void __fastcall OperateBookCase(int pnum, int i, unsigned char sendmsg);
void __fastcall OperateDecap(int pnum, int i, unsigned char sendmsg);
void __fastcall OperateArmorStand(int pnum, int i, unsigned char sendmsg);
int __fastcall FindValidShrine(int i);
void __fastcall OperateGoatShrine(int pnum, int i, int sType);
void __fastcall OperateCauldron(int pnum, int i, int sType);
bool __fastcall OperateFountains(int pnum, int i);
void __fastcall OperateWeaponRack(int pnum, int i, unsigned char sendmsg);
void __fastcall OperateStoryBook(int pnum, int i);
void __fastcall OperateLazStand(int pnum, int i);
void __fastcall OperateObject(int pnum, int i, unsigned char TeleFlag);
void __fastcall SyncOpL1Door(int pnum, int cmd, int i);
void __fastcall SyncOpL2Door(int pnum, int cmd, int i);
void __fastcall SyncOpL3Door(int pnum, int cmd, int i);
void __fastcall SyncOpObject(int pnum, int cmd, int i);
void __fastcall BreakCrux(int i);
void __fastcall BreakBarrel(int pnum, int i, int dam, unsigned char forcebreak, int sendmsg);
void __fastcall BreakObject(int pnum, int oi);
void __fastcall SyncBreakObj(int pnum, int oi);
void __fastcall SyncL1Doors(int i);
void __fastcall SyncCrux(int i);
void __fastcall SyncLever(int i);
void __fastcall SyncQSTLever(int i);
void __fastcall SyncPedistal(int i);
void __fastcall SyncL2Doors(int i);
void __fastcall SyncL3Doors(int i);
void __fastcall SyncObjectAnim(int o);
void __fastcall GetObjectStr(int i);

/* rdata */

extern int ObjTypeConv[113];
extern ObjDataStruct AllObjects[99];
extern char *ObjMasterLoadList[56];
extern int bxadd[8];
extern int byadd[8];
extern char *shrinestrs[NUM_SHRINETYPE];
extern unsigned char shrinemin[NUM_SHRINETYPE];
extern unsigned char shrinemax[NUM_SHRINETYPE];
extern unsigned char shrineavail[NUM_SHRINETYPE];
extern char *StoryBookName[9];
extern int StoryText[3][3];

#endif /* __OBJECTS_H__ */
