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
	mienemy_type _micaster;
	int _midam;
	bool _miHitFlag;
	int _midist; // Used for arrows to measure distance travelled (increases by 1 each game tick). Higher value is a penalty for accuracy calculation when hitting enemy
	int _mlid;
	int _mirnd;
	int var1;
	int var2;
	int var3;
	int var4;
	int var5;
	int var6;
	int var7;
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
void AddHiveExplosion(MissileStruct &missile, Point dst, Direction midir);
void AddFireRune(MissileStruct &missile, Point dst, Direction midir);
void AddLightningRune(MissileStruct &missile, Point dst, Direction midir);
void AddGreatLightningRune(MissileStruct &missile, Point dst, Direction midir);
void AddImmolationRune(MissileStruct &missile, Point dst, Direction midir);
void AddStoneRune(MissileStruct &missile, Point dst, Direction midir);
void AddReflection(MissileStruct &missile, Point dst, Direction midir);
void AddBerserk(MissileStruct &missile, Point dst, Direction midir);
void AddHorkSpawn(MissileStruct &missile, Point dst, Direction midir);
void AddJester(MissileStruct &missile, Point dst, Direction midir);
void AddStealPotions(MissileStruct &missile, Point dst, Direction midir);
void AddManaTrap(MissileStruct &missile, Point dst, Direction midir);
void AddSpecArrow(MissileStruct &missile, Point dst, Direction midir);
void AddWarp(MissileStruct &missile, Point dst, Direction midir);
void AddLightningWall(MissileStruct &missile, Point dst, Direction midir);
void AddRuneExplosion(MissileStruct &missile, Point dst, Direction midir);
void AddFireNova(MissileStruct &missile, Point dst, Direction midir);
void AddLightningArrow(MissileStruct &missile, Point dst, Direction midir);
void AddMana(MissileStruct &missile, Point dst, Direction midir);
void AddMagi(MissileStruct &missile, Point dst, Direction midir);
void AddRing(MissileStruct &missile, Point dst, Direction midir);
void AddSearch(MissileStruct &missile, Point dst, Direction midir);
void AddCboltArrow(MissileStruct &missile, Point dst, Direction midir);
void AddLArrow(MissileStruct &missile, Point dst, Direction midir);
void AddArrow(MissileStruct &missile, Point dst, Direction midir);
void AddRndTeleport(MissileStruct &missile, Point dst, Direction midir);
void AddFirebolt(MissileStruct &missile, Point dst, Direction midir);
void AddMagmaball(MissileStruct &missile, Point dst, Direction midir);
void AddTeleport(MissileStruct &missile, Point dst, Direction midir);
void AddLightball(MissileStruct &missile, Point dst, Direction midir);
void AddFirewall(MissileStruct &missile, Point dst, Direction midir);
void AddFireball(MissileStruct &missile, Point dst, Direction midir);
void AddLightctrl(MissileStruct &missile, Point dst, Direction midir);
void AddLightning(MissileStruct &missile, Point dst, Direction midir);
void AddMisexp(MissileStruct &missile, Point dst, Direction midir);
void AddWeapexp(MissileStruct &missile, Point dst, Direction midir);
void AddTown(MissileStruct &missile, Point dst, Direction midir);
void AddFlash(MissileStruct &missile, Point dst, Direction midir);
void AddFlash2(MissileStruct &missile, Point dst, Direction midir);
void AddManashield(MissileStruct &missile, Point dst, Direction midir);
void AddFiremove(MissileStruct &missile, Point dst, Direction midir);
void AddGuardian(MissileStruct &missile, Point dst, Direction midir);
void AddChain(MissileStruct &missile, Point dst, Direction midir);
void AddRhino(MissileStruct &missile, Point dst, Direction midir);
void AddFlare(MissileStruct &missile, Point dst, Direction midir);
void AddAcid(MissileStruct &missile, Point dst, Direction midir);
void AddAcidpud(MissileStruct &missile, Point dst, Direction midir);
void AddStone(MissileStruct &missile, Point dst, Direction midir);
void AddGolem(MissileStruct &missile, Point dst, Direction midir);
void AddBoom(MissileStruct &missile, Point dst, Direction midir);
void AddHeal(MissileStruct &missile, Point dst, Direction midir);
void AddHealOther(MissileStruct &missile, Point dst, Direction midir);
void AddElement(MissileStruct &missile, Point dst, Direction midir);
void AddIdentify(MissileStruct &missile, Point dst, Direction midir);
void AddFirewallC(MissileStruct &missile, Point dst, Direction midir);
void AddInfra(MissileStruct &missile, Point dst, Direction midir);
void AddWave(MissileStruct &missile, Point dst, Direction midir);
void AddNova(MissileStruct &missile, Point dst, Direction midir);
void AddBlodboil(MissileStruct &missile, Point dst, Direction midir);
void AddRepair(MissileStruct &missile, Point dst, Direction midir);
void AddRecharge(MissileStruct &missile, Point dst, Direction midir);
void AddDisarm(MissileStruct &missile, Point dst, Direction midir);
void AddApoca(MissileStruct &missile, Point dst, Direction midir);
void AddFlame(MissileStruct &missile, Point dst, Direction midir);
void AddFlamec(MissileStruct &missile, Point dst, Direction midir);
void AddCbolt(MissileStruct &missile, Point dst, Direction midir);
void AddHbolt(MissileStruct &missile, Point dst, Direction midir);
void AddResurrect(MissileStruct &missile, Point dst, Direction midir);
void AddResurrectBeam(MissileStruct &missile, Point dst, Direction midir);
void AddTelekinesis(MissileStruct &missile, Point dst, Direction midir);
void AddBoneSpirit(MissileStruct &missile, Point dst, Direction midir);
void AddRportal(MissileStruct &missile, Point dst, Direction midir);
void AddDiabApoca(MissileStruct &missile, Point dst, Direction midir);
int AddMissile(Point src, Point dst, Direction midir, missile_id mitype, mienemy_type micaster, int id, int midam, int spllvl);
void MI_Golem(MissileStruct &missile);
void MI_Manashield(MissileStruct &missile);
void MI_LArrow(MissileStruct &missile);
void MI_Arrow(MissileStruct &missile);
void MI_Firebolt(MissileStruct &missile);
void MI_Lightball(MissileStruct &missilei);
void MI_Acidpud(MissileStruct &missile);
void MI_Firewall(MissileStruct &missile);
void MI_Fireball(MissileStruct &missile);
void MI_HorkSpawn(MissileStruct &missile);
void MI_Rune(MissileStruct &missile);
void MI_LightningWall(MissileStruct &missile);
void MI_HiveExplode(MissileStruct &missile);
void MI_LightningArrow(MissileStruct &missile);
void MI_FireRing(MissileStruct &missile);
void MI_Search(MissileStruct &missile);
void MI_LightningWallC(MissileStruct &missile);
void MI_FireNova(MissileStruct &missile);
void MI_SpecArrow(MissileStruct &missile);
void MI_Lightctrl(MissileStruct &missile);
void MI_Lightning(MissileStruct &missile);
void MI_Town(MissileStruct &missile);
void MI_Flash(MissileStruct &missile);
void MI_Flash2(MissileStruct &missile);
void MI_Firemove(MissileStruct &missile);
void MI_Guardian(MissileStruct &missile);
void MI_Chain(MissileStruct &missile);
void MI_Weapexp(MissileStruct &missile);
void MI_Misexp(MissileStruct &missile);
void MI_Acidsplat(MissileStruct &missile);
void MI_Teleport(MissileStruct &missile);
void MI_Stone(MissileStruct &missile);
void MI_Boom(MissileStruct &missile);
void MI_Rhino(MissileStruct &missile);
void MI_FirewallC(MissileStruct &missile);
void MI_Infra(MissileStruct &missile);
void MI_Apoca(MissileStruct &missile);
void MI_Wave(MissileStruct &missile);
void MI_Nova(MissileStruct &missile);
void MI_Blodboil(MissileStruct &missile);
void MI_Flame(MissileStruct &missile);
void MI_Flamec(MissileStruct &missile);
void MI_Cbolt(MissileStruct &missile);
void MI_Hbolt(MissileStruct &missile);
void MI_Element(MissileStruct &missile);
void MI_Bonespirit(MissileStruct &missile);
void MI_ResurrectBeam(MissileStruct &missile);
void MI_Rportal(MissileStruct &missile);
void ProcessMissiles();
void missiles_process_charge();
void RedoMissileFlags();

} // namespace devilution
