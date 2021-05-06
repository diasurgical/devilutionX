/**
 * @file missiles.h
 *
 * Interface of missile functionality.
 */
#pragma once

#include <cstdint>

#include "miniwin/miniwin.h"
#include "engine.h"
#include "misdat.h"
#include "spelldat.h"

namespace devilution {

#define MAXMISSILES 125

struct ChainStruct {
	int idx;
	int _mitype;
	int _mirange;
};

struct MissilePosition {
	Point tile;
	/** Sprite's pixel offset from tile. */
	Point offset;
	/** Pixel velocity while moving */
	Point velocity;
	/** Start position */
	Point start;
	/** Start position */
	Point traveled;
};

struct MissileStruct {
	int _mitype; // Type of projectile (missile_id)
	MissilePosition position;
	int _mimfnum; // The direction of the missile (direction enum)
	int _mispllvl;
	bool _miDelFlag; // Indicate whether the missile should be deleted
	uint8_t _miAnimType;
	int _miAnimFlags;
	byte *_miAnimData;
	int _miAnimDelay; // Tick length of each frame in the current animation
	int _miAnimLen;   // Number of frames in current animation
	int _miAnimWidth;
	int _miAnimWidth2;
	int _miAnimCnt; // Increases by one each game tick, counting how close we are to _pAnimDelay
	int _miAnimAdd;
	int _miAnimFrame; // Current frame of animation.
	bool _miDrawFlag;
	bool _miLightFlag;
	bool _miPreFlag;
	uint32_t _miUniqTrans;
	int _mirange; // Time to live for the missile in game ticks, oncs 0 the missile will be marked for deletion via _miDelFlag
	int _misource;
	/** mienemy_type or player id*/
	int8_t _micaster;
	int _midam;
	bool _miHitFlag;
	int _midist; // Used for arrows to measure distance travelled (increases by 1 each game tick). Higher value is a penalty for accuracy calculation when hitting enemy
	int _mlid;
	int _mirnd;
	int _miVar1;
	int _miVar2;
	int _miVar3;
	int _miVar4;
	int _miVar5;
	int _miVar6;
	int _miVar7;
	int _miVar8;
};

extern int missileactive[MAXMISSILES];
extern int missileavail[MAXMISSILES];
extern MissileStruct missile[MAXMISSILES];
extern int nummissiles;
extern bool MissilePreFlag;

void GetDamageAmt(int i, int *mind, int *maxd);
int GetSpellLevel(int id, spell_id sn);
int GetDirection16(int x1, int y1, int x2, int y2);
void DeleteMissile(int mi, int i);
bool MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, bool shift);
bool PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, bool shift, int earflag, bool *blocked);
void SetMissAnim(int mi, int animtype);
void SetMissDir(int mi, int dir);
void LoadMissileGFX(BYTE mi);
void InitMissileGFX();
void FreeMissiles();
void FreeMissiles2();
void InitMissiles();
void AddHiveExplosion(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFireRune(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddLightningRune(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddGreatLightningRune(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddImmolationRune(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddStoneRune(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddReflection(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddBerserk(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddHorkSpawn(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddJester(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddStealPotions(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddManaTrap(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddSpecArrow(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddWarp(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddLightningWall(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddRuneExplosion(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddImmolation(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFireNova(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddLightningArrow(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddMana(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddMagi(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddRing(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddSearch(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddCboltArrow(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddLArrow(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddArrow(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddRndTeleport(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFirebolt(int mi, int sx, int sy, int dx, int dy, int midir, int8_t micaster, int id, int dam);
void AddMagmaball(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddKrull(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddTeleport(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddLightball(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFirewall(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFireball(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddLightctrl(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddLightning(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddMisexp(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddWeapexp(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddTown(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFlash(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFlash2(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddManashield(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFiremove(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddGuardian(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddChain(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddBloodStar(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddBone(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddMetlHit(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddRhino(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFireman(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFlare(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddAcid(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFireWallA(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddAcidpud(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddStone(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddGolem(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddEtherealize(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddDummy(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddBlodbur(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddBoom(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddHeal(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddHealOther(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddElement(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddIdentify(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFirewallC(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddInfra(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddWave(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddNova(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddBlodboil(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddRepair(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddRecharge(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddDisarm(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddApoca(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFlame(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddFlamec(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddCbolt(int mi, int sx, int sy, int dx, int dy, int midir, int8_t micaster, int id, int dam);
void AddHbolt(int mi, int sx, int sy, int dx, int dy, int midir, int8_t micaster, int id, int dam);
void AddResurrect(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddResurrectBeam(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddTelekinesis(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddBoneSpirit(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddRportal(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
void AddDiabApoca(int mi, int sx, int sy, int dx, int dy, int midir, int8_t mienemy, int id, int dam);
int AddMissile(int sx, int sy, int dx, int dy, int midir, int mitype, int8_t micaster, int id, int midam, int spllvl);
void MI_Dummy(int i);
void MI_Golem(int i);
void MI_Manashield(int i);
void MI_LArrow(int i);
void MI_Arrow(int i);
void MI_Firebolt(int i);
void MI_Lightball(int i);
void MI_Krull(int i);
void MI_Acidpud(int i);
void MI_Firewall(int i);
void MI_Fireball(int i);
void MI_HorkSpawn(int i);
void MI_Rune(int i);
void MI_LightningWall(int i);
void MI_HiveExplode(int i);
void MI_Immolation(int i);
void MI_LightningArrow(int i);
void MI_Reflect(int i);
void MI_FireRing(int i);
void MI_LightningRing(int i);
void MI_Search(int i);
void MI_LightningWallC(int i);
void MI_FireNova(int i);
void MI_SpecArrow(int i);
void MI_Lightctrl(int i);
void MI_Lightning(int i);
void MI_Town(int i);
void MI_Flash(int i);
void MI_Flash2(int i);
void MI_Etherealize(int i);
void MI_Firemove(int i);
void MI_Guardian(int i);
void MI_Chain(int i);
void MI_Blood(int i);
void MI_Weapexp(int i);
void MI_Misexp(int i);
void MI_Acidsplat(int i);
void MI_Teleport(int i);
void MI_Stone(int i);
void MI_Boom(int i);
void MI_Rhino(int i);
void MI_Fireman(int i);
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

} // namespace devilution
