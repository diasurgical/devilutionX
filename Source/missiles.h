/**
 * @file missiles.h
 *
 * Interface of missile functionality.
 */
#ifndef __MISSILES_H__
#define __MISSILES_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MissileData {
	Uint8 mName;
	void (*mAddProc)(Sint32, Sint32, Sint32, Sint32, Sint32, Sint32, Sint8, Sint32, Sint32);
	void (*mProc)(Sint32);
	bool mDraw;
	Uint8 mType;
	Uint8 mResist;
	Uint8 mFileNum;
	Sint32 mlSFX;
	Sint32 miSFX;
} MissileData;

typedef struct MisFileData {
	Uint8 mAnimName;
	Uint8 mAnimFAmt;
	const char *mName;
	Sint32 mFlags;
	Uint8 *mAnimData[16];
	Uint8 mAnimDelay[16];
	Uint8 mAnimLen[16];
	Sint32 mAnimWidth[16];
	Sint32 mAnimWidth2[16];
} MisFileData;

typedef struct ChainStruct {
	Sint32 idx;
	Sint32 _mitype;
	Sint32 _mirange;
} ChainStruct;

typedef struct MissileStruct {
	Sint32 _mitype;  // Type of projectile (missile_id)
	Sint32 _mix;     // Tile X-position of the missile
	Sint32 _miy;     // Tile Y-position of the missile
	Sint32 _mixoff;  // Sprite pixel X-offset for the missile
	Sint32 _miyoff;  // Sprite pixel Y-offset for the missile
	Sint32 _mixvel;  // Missile tile X-velocity while walking. This gets added onto _mitxoff each game tick
	Sint32 _miyvel;  // Missile tile Y-velocity while walking. This gets added onto _mitxoff each game tick
	Sint32 _misx;    // Initial tile X-position for missile
	Sint32 _misy;    // Initial tile Y-position for missile
	Sint32 _mitxoff; // How far the missile has travelled in its lifespan along the X-axis. mix/miy/mxoff/myoff get updated every game tick based on this
	Sint32 _mityoff; // How far the missile has travelled in its lifespan along the Y-axis. mix/miy/mxoff/myoff get updated every game tick based on this
	Sint32 _mimfnum; // The direction of the missile (direction enum)
	Sint32 _mispllvl;
	bool _miDelFlag; // Indicate whether the missile should be deleted
	Uint8 _miAnimType;
	Sint32 _miAnimFlags;
	Uint8 *_miAnimData;
	Sint32 _miAnimDelay; // Tick length of each frame in the current animation
	Sint32 _miAnimLen;   // Number of frames in current animation
	Sint32 _miAnimWidth;
	Sint32 _miAnimWidth2;
	Sint32 _miAnimCnt; // Increases by one each game tick, counting how close we are to _pAnimDelay
	Sint32 _miAnimAdd;
	Sint32 _miAnimFrame; // Current frame of animation.
	bool _miDrawFlag;
	bool _miLightFlag;
	bool _miPreFlag;
	Uint32 _miUniqTrans;
	Sint32 _mirange; // Time to live for the missile in game ticks, oncs 0 the missile will be marked for deletion via _miDelFlag
	Sint32 _misource;
	Sint32 _micaster;
	Sint32 _midam;
	bool _miHitFlag;
	Sint32 _midist; // Used for arrows to measure distance travelled (increases by 1 each game tick). Higher value is a penalty for accuracy calculation when hitting enemy
	Sint32 _mlid;
	Sint32 _mirnd;
	Sint32 _miVar1;
	Sint32 _miVar2;
	Sint32 _miVar3;
	Sint32 _miVar4;
	Sint32 _miVar5;
	Sint32 _miVar6;
	Sint32 _miVar7;
	Sint32 _miVar8;
} MissileStruct;

extern int missileactive[MAXMISSILES];
extern int missileavail[MAXMISSILES];
extern MissileStruct missile[MAXMISSILES];
extern int nummissiles;
extern bool MissilePreFlag;

void GetDamageAmt(int i, int *mind, int *maxd);
int GetSpellLevel(int id, int sn);
int GetDirection8(int x1, int y1, int x2, int y2);
int GetDirection16(int x1, int y1, int x2, int y2);
void DeleteMissile(int mi, int i);
BOOL MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, BOOLEAN shift);
BOOL PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, BOOLEAN shift, int earflag, BOOLEAN *blocked);
void SetMissAnim(int mi, int animtype);
void SetMissDir(int mi, int dir);
void LoadMissileGFX(BYTE mi);
void InitMissileGFX();
void FreeMissiles();
void FreeMissiles2();
void InitMissiles();
void AddHiveExplosion(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFireRune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddLightningRune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddGreatLightningRune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddImmolationRune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddStoneRune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddReflection(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddBerserk(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddHorkSpawn(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddJester(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddStealPotions(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddManaTrap(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddSpecArrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddWarp(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddLightningWall(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddRuneExplosion(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddImmolation(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFireNova(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddLightningArrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFlashFront(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFlashBack(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddMana(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddMagi(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddRing(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddSearch(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddCboltArrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddHboltArrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddLArrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddArrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddRndTeleport(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFirebolt(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 micaster, Sint32 id, Sint32 dam);
void AddMagmaball(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddKrull(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddTeleport(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddLightball(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFirewall(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFireball(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddLightctrl(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddLightning(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddMisexp(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddWeapexp(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddTown(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFlash(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFlash2(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddManashield(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFiremove(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddGuardian(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddChain(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddBloodStar(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddBone(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddMetlHit(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddRhino(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFireman(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFlare(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddAcid(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFireWallA(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddAcidpud(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddStone(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddGolem(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddEtherealize(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddDummy(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddBlodbur(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddBoom(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddHeal(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddHealOther(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddElement(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddIdentify(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFirewallC(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddInfra(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddWave(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddNova(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddBlodboil(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddRepair(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddRecharge(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddDisarm(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddApoca(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFlame(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFlamec(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddCbolt(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 micaster, Sint32 id, Sint32 dam);
void AddHbolt(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 micaster, Sint32 id, Sint32 dam);
void AddResurrect(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddResurrectBeam(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddTelekinesis(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddBoneSpirit(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddRportal(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddDiabApoca(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
int AddMissile(int sx, int sy, int dx, int dy, int midir, int mitype, char micaster, int id, int midam, int spllvl);
void MI_Dummy(Sint32 i);
void MI_Golem(Sint32 i);
void MI_SetManashield(Sint32 i);
void MI_LArrow(Sint32 i);
void MI_Arrow(Sint32 i);
void MI_Firebolt(Sint32 i);
void MI_Lightball(Sint32 i);
void MI_Krull(Sint32 i);
void MI_Acidpud(Sint32 i);
void MI_Firewall(Sint32 i);
void MI_Fireball(Sint32 i);
void MI_HorkSpawn(Sint32 i);
void MI_Rune(Sint32 i);
void MI_LightningWall(Sint32 i);
void MI_HiveExplode(Sint32 i);
void MI_Immolation(Sint32 i);
void MI_LightningArrow(Sint32 i);
void MI_FlashFront(Sint32 i);
void MI_FlashBack(Sint32 i);
void MI_Reflect(Sint32 i);
void MI_FireRing(Sint32 i);
void MI_LightningRing(Sint32 i);
void MI_Search(Sint32 i);
void MI_LightningWallC(Sint32 i);
void MI_FireNova(Sint32 i);
void MI_SpecArrow(Sint32 i);
void MI_Lightctrl(Sint32 i);
void MI_Lightning(Sint32 i);
void MI_Town(Sint32 i);
void MI_Flash(Sint32 i);
void MI_Flash2(Sint32 i);
void MI_Etherealize(Sint32 i);
void MI_Firemove(Sint32 i);
void MI_Guardian(Sint32 i);
void MI_Chain(Sint32 i);
void MI_Blood(Sint32 i);
void MI_Weapexp(Sint32 i);
void MI_Misexp(Sint32 i);
void MI_Acidsplat(Sint32 i);
void MI_Teleport(Sint32 i);
void MI_Stone(Sint32 i);
void MI_Boom(Sint32 i);
void MI_Rhino(Sint32 i);
void MI_Fireman(Sint32 i);
void MI_FirewallC(Sint32 i);
void MI_Infra(Sint32 i);
void MI_Apoca(Sint32 i);
void MI_Wave(Sint32 i);
void MI_Nova(Sint32 i);
void MI_Blodboil(Sint32 i);
void MI_Flame(Sint32 i);
void MI_Flamec(Sint32 i);
void MI_Cbolt(Sint32 i);
void MI_Hbolt(Sint32 i);
void MI_Element(Sint32 i);
void MI_Bonespirit(Sint32 i);
void MI_ResurrectBeam(Sint32 i);
void MI_Rportal(Sint32 i);
void ProcessMissiles();
void missiles_process_charge();
void ClearMissileSpot(int mi);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MISSILES_H__ */
