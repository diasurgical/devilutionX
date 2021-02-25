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
	BOOL mDraw;
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
	bool _miDelFlag; // Indicate weather the missile should be deleted
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
	Sint32 _miUniqTrans;
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
void missiles_hive_explosion(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_fire_rune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_light_rune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_great_light_rune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_immolation_rune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_stone_rune(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_reflection(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_berserk(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_430624(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_jester(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_steal_pots(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_mana_trap(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_spec_arrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_warp(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_light_wall(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_rune_explosion(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_immo_1(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_immo_2(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_larrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_43303D(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_433040(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_rech_mana(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_magi(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_ring(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_search(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_cbolt_arrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void missiles_hbolt_arrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddLArrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddArrow(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddRndTeleport(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFirebolt(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 micaster, Sint32 id, Sint32 dam);
void AddMagmaball(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void miss_null_33(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
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
void miss_null_11(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void miss_null_12(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void miss_null_13(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddRhino(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void miss_null_32(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddFlare(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddAcid(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void miss_null_1D(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddAcidpud(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddStone(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddGolem(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void AddEtherealize(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void miss_null_1F(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
void miss_null_23(Sint32 mi, Sint32 sx, Sint32 sy, Sint32 dx, Sint32 dy, Sint32 midir, Sint8 mienemy, Sint32 id, Sint32 dam);
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
void MI_Dummy(int i);
void MI_Golem(int i);
void MI_SetManashield(int i);
void MI_LArrow(int i);
void MI_Arrow(int i);
void MI_Firebolt(int i);
void MI_Lightball(int i);
void mi_null_33(int i);
void MI_Acidpud(int i);
void MI_Firewall(int i);
void MI_Fireball(int i);
void missiles_4359A0(int i);
void MI_Rune(int i);
void mi_light_wall(int i);
void mi_hive_explode(int i);
void mi_immolation(int i);
void mi_light_arrow(int i);
void mi_flashfr(int i);
void mi_flashbk(int i);
void mi_reflect(int i);
void mi_fire_ring(int i);
void mi_light_ring(int i);
void mi_search(int i);
void mi_lightning_wall(int i);
void mi_fire_nova(int i);
void mi_spec_arrow(int i);
void MI_Lightctrl(int i);
void MI_Lightning(int i);
void MI_Town(int i);
void MI_Flash(int i);
void MI_Flash2(int i);
void MI_Etherealize(int i);
void MI_Firemove(int i);
void MI_Guardian(int i);
void MI_Chain(int i);
void mi_null_11(int i);
void MI_Weapexp(int i);
void MI_Misexp(int i);
void MI_Acidsplat(int i);
void MI_Teleport(int i);
void MI_Stone(int i);
void MI_Boom(int i);
void MI_Rhino(int i);
void mi_null_32(int i);
void MI_FirewallC(int i);
void MI_Infra(int i);
void MI_Apoca(int i);
void MI_Wave(int i);
void MI_Nova(int i);
void MI_Blodboil(int i);
void MI_Flame(int i);
void MI_Flamec(int i);
void MI_Cbolt(int i);
void MI_Hbolt(int i);
void MI_Element(int i);
void MI_Bonespirit(int i);
void MI_ResurrectBeam(int i);
void MI_Rportal(int i);
void ProcessMissiles();
void missiles_process_charge();
void ClearMissileSpot(int mi);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MISSILES_H__ */
