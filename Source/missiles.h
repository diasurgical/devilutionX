/**
 * @file missiles.h
 *
 * Interface of missile functionality.
 */
#pragma once

#include <cstdint>

#include "miniwin/miniwin.h"
#include "engine.h"
#include "engine/point.hpp"
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


/*
 *      W  sW  SW   Sw  S
 *              ^
 *     nW       |       Se
 *              |
 *     NW ------+-----> SE
 *              |
 *     Nw       |       sE
 *              |
 *      N  Ne  NE   nE  E
 */
enum Direction16
{
	DIR16_S,
	DIR16_Sw,
	DIR16_SW,
	DIR16_sW,
	DIR16_W,
	DIR16_nW,
	DIR16_NW,
	DIR16_Nw,
	DIR16_N,
	DIR16_Ne,
	DIR16_NE,
	DIR16_nE,
	DIR16_E,
	DIR16_sE,
	DIR16_SE,
	DIR16_Se,
};

struct MissileStruct {
	int _mitype; // Type of projectile (missile_id)
	MissilePosition position;
	int _mimfnum; // The direction of the missile (direction enum)
	int _mispllvl;
	bool _miDelFlag; // Indicate whether the missile should be deleted
	uint8_t _miAnimType;
	int _miAnimFlags;
	const byte *_miAnimData;
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
int GetSpellLevel(int playerId, spell_id sn);
Direction16 GetDirection16(Point p1, Point p2);
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
void AddHiveExplosion(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFireRune(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightningRune(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddGreatLightningRune(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddImmolationRune(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddStoneRune(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddReflection(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBerserk(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddHorkSpawn(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddJester(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddStealPotions(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddManaTrap(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddSpecArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddWarp(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightningWall(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRuneExplosion(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddImmolation(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFireNova(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightningArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddMana(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddMagi(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRing(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddSearch(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddCboltArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRndTeleport(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFirebolt(int mi, Point src, Point dst, int midir, int8_t micaster, int id, int dam);
void AddMagmaball(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddKrull(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddTeleport(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightball(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFirewall(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFireball(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightctrl(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightning(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddMisexp(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddWeapexp(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddTown(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlash(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlash2(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddManashield(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFiremove(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddGuardian(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddChain(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBloodStar(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBone(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddMetlHit(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRhino(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFireman(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlare(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddAcid(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFireWallA(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddAcidpud(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddStone(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddGolem(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddEtherealize(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddDummy(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBlodbur(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBoom(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddHeal(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddHealOther(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddElement(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddIdentify(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFirewallC(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddInfra(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddWave(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddNova(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBlodboil(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRepair(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRecharge(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddDisarm(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddApoca(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlame(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlamec(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddCbolt(int mi, Point src, Point dst, int midir, int8_t micaster, int id, int dam);
void AddHbolt(int mi, Point src, Point dst, int midir, int8_t micaster, int id, int dam);
void AddResurrect(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddResurrectBeam(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddTelekinesis(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBoneSpirit(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRportal(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddDiabApoca(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
int AddMissile(Point src, Point dst, int midir, int mitype, int8_t micaster, int id, int midam, int spllvl);
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
