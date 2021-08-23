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
#include "monster.h"
#include "spelldat.h"

namespace devilution {

#define MAXMISSILES 125

constexpr Point GolemHoldingCell = Point { 1, 0 };

struct ChainStruct {
	int idx;
	missile_id _mitype;
	int _mirange;
};

struct MissilePosition {
	Point tile;
	/** Sprite's pixel offset from tile. */
	Displacement offset;
	/** Pixel velocity while moving */
	Displacement velocity;
	/** Start position */
	Point start;
	/** Start position */
	Displacement traveled;

	/**
      * @brief Specifies the location (tile) while rendering
      */
	Point tileForRendering;
	/**
      * @brief Specifies the location (offset) while rendering
      */
	Displacement offsetForRendering;

	/**
      * @brief Stops the missile (set velocity to zero and set offset to last renderer location; shouldn't matter cause the missile don't move anymore)
      */
	void StopMissile()
	{
		velocity = {};
		if (tileForRendering == tile)
			offset = offsetForRendering;
	}
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
enum Direction16 {
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
	/** Type of projectile */
	missile_id _mitype;
	MissilePosition position;
	int _mimfnum; // The direction of the missile (direction enum)
	int _mispllvl;
	bool _miDelFlag; // Indicate whether the missile should be deleted
	uint8_t _miAnimType;
	MissileDataFlags _miAnimFlags;
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
	bool limitReached;
};

extern MissileStruct Missiles[MAXMISSILES];
extern int AvailableMissiles[MAXMISSILES];
extern int ActiveMissiles[MAXMISSILES];
extern int ActiveMissileCount;
extern bool MissilePreFlag;

void GetDamageAmt(int i, int *mind, int *maxd);
int GetSpellLevel(int playerId, spell_id sn);
Direction16 GetDirection16(Point p1, Point p2);
void DeleteMissile(int mi, int i);
bool MonsterTrapHit(int m, int mindam, int maxdam, int dist, missile_id t, bool shift);
bool PlayerMHit(int pnum, MonsterStruct *monster, int dist, int mind, int maxd, missile_id mtype, bool shift, int earflag, bool *blocked);
void SetMissDir(MissileStruct &missile, int dir);
void InitMissileGFX();
void FreeMissiles();
void FreeMissiles2();
void InitMissiles();
void AddHiveExplosion(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFireRune(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightningRune(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddGreatLightningRune(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddImmolationRune(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddStoneRune(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddReflection(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBerserk(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddHorkSpawn(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddJester(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddStealPotions(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddManaTrap(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddSpecArrow(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddWarp(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightningWall(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRuneExplosion(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFireNova(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightningArrow(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddMana(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddMagi(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRing(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddSearch(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddCboltArrow(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLArrow(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddArrow(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRndTeleport(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFirebolt(MissileStruct &missile, Point src, Point dst, int midir, int8_t micaster, int id, int dam);
void AddMagmaball(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddTeleport(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightball(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFirewall(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFireball(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightctrl(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddLightning(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddMisexp(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddWeapexp(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddTown(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlash(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlash2(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddManashield(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFiremove(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddGuardian(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddChain(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRhino(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlare(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddAcid(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddAcidpud(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddStone(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddGolem(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBoom(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddHeal(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddHealOther(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddElement(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddIdentify(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFirewallC(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddInfra(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddWave(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddNova(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBlodboil(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRepair(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRecharge(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddDisarm(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddApoca(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlame(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddFlamec(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddCbolt(MissileStruct &missile, Point src, Point dst, int midir, int8_t micaster, int id, int dam);
void AddHbolt(MissileStruct &missile, Point src, Point dst, int midir, int8_t micaster, int id, int dam);
void AddResurrect(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddResurrectBeam(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddTelekinesis(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddBoneSpirit(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddRportal(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
void AddDiabApoca(MissileStruct &missile, Point src, Point dst, int midir, int8_t mienemy, int id, int dam);
int AddMissile(Point src, Point dst, int midir, missile_id mitype, int8_t micaster, int id, int midam, int spllvl);
void MI_Golem(int mi);
void MI_Manashield(int i);
void MI_LArrow(int i);
void MI_Arrow(int i);
void MI_Firebolt(int i);
void MI_Lightball(int i);
void MI_Acidpud(int i);
void MI_Firewall(int i);
void MI_Fireball(int i);
void MI_HorkSpawn(int mi);
void MI_Rune(int i);
void MI_LightningWall(int i);
void MI_HiveExplode(int i);
void MI_LightningArrow(int i);
void MI_FireRing(int i);
void MI_Search(int i);
void MI_LightningWallC(int i);
void MI_FireNova(int i);
void MI_SpecArrow(int i);
void MI_Lightctrl(int i);
void MI_Lightning(int i);
void MI_Town(int i);
void MI_Flash(int i);
void MI_Flash2(int i);
void MI_Firemove(int i);
void MI_Guardian(int i);
void MI_Chain(int mi);
void MI_Weapexp(int i);
void MI_Misexp(int i);
void MI_Acidsplat(int i);
void MI_Teleport(int i);
void MI_Stone(int i);
void MI_Boom(int i);
void MI_Rhino(int i);
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
void RedoMissileFlags();
void ClearMissileSpot(const MissileStruct &missile);

} // namespace devilution
