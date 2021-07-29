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

struct Missile {
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
	/**
      * @brief For moving missiles lastCollisionTargetHash contains the last entity (player or monster) that was checked in CheckMissileCol (needed to avoid multiple hits for a entity at the same tile).
      */
	int16_t lastCollisionTargetHash;
};

extern Missile Missiles[MAXMISSILES];
extern int AvailableMissiles[MAXMISSILES];
extern int ActiveMissiles[MAXMISSILES];
extern int ActiveMissileCount;
extern bool MissilePreFlag;

void GetDamageAmt(int i, int *mind, int *maxd);
int GetSpellLevel(int playerId, spell_id sn);
Direction16 GetDirection16(Point p1, Point p2);
void DeleteMissile(int i);
bool MonsterTrapHit(int m, int mindam, int maxdam, int dist, missile_id t, bool shift);
bool PlayerMHit(int pnum, Monster *monster, int dist, int mind, int maxd, missile_id mtype, bool shift, int earflag, bool *blocked);

/**
 * @brief Sets the missile sprite to represent the direction of travel
 * @param missile this object
 * @param dir Sprite frame representing the desired facing
*/
void SetMissDir(Missile &missile, int dir);

/**
 * @brief Overload to convert a Direction value to the appropriate sprite frame
 * @param missile this object
 * @param dir Desired facing
*/
inline void SetMissDir(Missile &missile, Direction dir)
{
	SetMissDir(missile, static_cast<int>(dir));
}

void InitMissiles();
void AddHiveExplosion(Missile &missile, Point dst, Direction midir);
void AddFireRune(Missile &missile, Point dst, Direction midir);
void AddLightningRune(Missile &missile, Point dst, Direction midir);
void AddGreatLightningRune(Missile &missile, Point dst, Direction midir);
void AddImmolationRune(Missile &missile, Point dst, Direction midir);
void AddStoneRune(Missile &missile, Point dst, Direction midir);
void AddReflection(Missile &missile, Point dst, Direction midir);
void AddBerserk(Missile &missile, Point dst, Direction midir);
void AddHorkSpawn(Missile &missile, Point dst, Direction midir);
void AddJester(Missile &missile, Point dst, Direction midir);
void AddStealPotions(Missile &missile, Point dst, Direction midir);
void AddManaTrap(Missile &missile, Point dst, Direction midir);
void AddSpecArrow(Missile &missile, Point dst, Direction midir);
void AddWarp(Missile &missile, Point dst, Direction midir);
void AddLightningWall(Missile &missile, Point dst, Direction midir);
void AddRuneExplosion(Missile &missile, Point dst, Direction midir);
void AddFireNova(Missile &missile, Point dst, Direction midir);
void AddLightningArrow(Missile &missile, Point dst, Direction midir);
void AddMana(Missile &missile, Point dst, Direction midir);
void AddMagi(Missile &missile, Point dst, Direction midir);
void AddRing(Missile &missile, Point dst, Direction midir);
void AddSearch(Missile &missile, Point dst, Direction midir);
void AddCboltArrow(Missile &missile, Point dst, Direction midir);
void AddLArrow(Missile &missile, Point dst, Direction midir);
void AddArrow(Missile &missile, Point dst, Direction midir);
void AddRndTeleport(Missile &missile, Point dst, Direction midir);
void AddFirebolt(Missile &missile, Point dst, Direction midir);
void AddMagmaball(Missile &missile, Point dst, Direction midir);
void AddTeleport(Missile &missile, Point dst, Direction midir);
void AddLightball(Missile &missile, Point dst, Direction midir);
void AddFirewall(Missile &missile, Point dst, Direction midir);
void AddFireball(Missile &missile, Point dst, Direction midir);
void AddLightctrl(Missile &missile, Point dst, Direction midir);
void AddLightning(Missile &missile, Point dst, Direction midir);
void AddMisexp(Missile &missile, Point dst, Direction midir);
void AddWeapexp(Missile &missile, Point dst, Direction midir);
void AddTown(Missile &missile, Point dst, Direction midir);
void AddFlash(Missile &missile, Point dst, Direction midir);
void AddFlash2(Missile &missile, Point dst, Direction midir);
void AddManashield(Missile &missile, Point dst, Direction midir);
void AddFiremove(Missile &missile, Point dst, Direction midir);
void AddGuardian(Missile &missile, Point dst, Direction midir);
void AddChain(Missile &missile, Point dst, Direction midir);
void AddRhino(Missile &missile, Point dst, Direction midir);
void AddFlare(Missile &missile, Point dst, Direction midir);
void AddAcid(Missile &missile, Point dst, Direction midir);
void AddAcidpud(Missile &missile, Point dst, Direction midir);
void AddStone(Missile &missile, Point dst, Direction midir);
void AddGolem(Missile &missile, Point dst, Direction midir);
void AddBoom(Missile &missile, Point dst, Direction midir);
void AddHeal(Missile &missile, Point dst, Direction midir);
void AddHealOther(Missile &missile, Point dst, Direction midir);
void AddElement(Missile &missile, Point dst, Direction midir);
void AddIdentify(Missile &missile, Point dst, Direction midir);
void AddFirewallC(Missile &missile, Point dst, Direction midir);
void AddInfra(Missile &missile, Point dst, Direction midir);
void AddWave(Missile &missile, Point dst, Direction midir);
void AddNova(Missile &missile, Point dst, Direction midir);
void AddBlodboil(Missile &missile, Point dst, Direction midir);
void AddRepair(Missile &missile, Point dst, Direction midir);
void AddRecharge(Missile &missile, Point dst, Direction midir);
void AddDisarm(Missile &missile, Point dst, Direction midir);
void AddApoca(Missile &missile, Point dst, Direction midir);
void AddFlame(Missile &missile, Point dst, Direction midir);
void AddFlamec(Missile &missile, Point dst, Direction midir);
void AddCbolt(Missile &missile, Point dst, Direction midir);
void AddHbolt(Missile &missile, Point dst, Direction midir);
void AddResurrect(Missile &missile, Point dst, Direction midir);
void AddResurrectBeam(Missile &missile, Point dst, Direction midir);
void AddTelekinesis(Missile &missile, Point dst, Direction midir);
void AddBoneSpirit(Missile &missile, Point dst, Direction midir);
void AddRportal(Missile &missile, Point dst, Direction midir);
void AddDiabApoca(Missile &missile, Point dst, Direction midir);
int AddMissile(Point src, Point dst, Direction midir, missile_id mitype, mienemy_type micaster, int id, int midam, int spllvl);
void MI_Golem(Missile &missile);
void MI_Manashield(Missile &missile);
void MI_LArrow(Missile &missile);
void MI_Arrow(Missile &missile);
void MI_Firebolt(Missile &missile);
void MI_Lightball(Missile &missilei);
void MI_Acidpud(Missile &missile);
void MI_Firewall(Missile &missile);
void MI_Fireball(Missile &missile);
void MI_HorkSpawn(Missile &missile);
void MI_Rune(Missile &missile);
void MI_LightningWall(Missile &missile);
void MI_HiveExplode(Missile &missile);
void MI_LightningArrow(Missile &missile);
void MI_FireRing(Missile &missile);
void MI_Search(Missile &missile);
void MI_LightningWallC(Missile &missile);
void MI_FireNova(Missile &missile);
void MI_SpecArrow(Missile &missile);
void MI_Lightctrl(Missile &missile);
void MI_Lightning(Missile &missile);
void MI_Town(Missile &missile);
void MI_Flash(Missile &missile);
void MI_Flash2(Missile &missile);
void MI_Firemove(Missile &missile);
void MI_Guardian(Missile &missile);
void MI_Chain(Missile &missile);
void MI_Weapexp(Missile &missile);
void MI_Misexp(Missile &missile);
void MI_Acidsplat(Missile &missile);
void MI_Teleport(Missile &missile);
void MI_Stone(Missile &missile);
void MI_Boom(Missile &missile);
void MI_Rhino(Missile &missile);
void MI_FirewallC(Missile &missile);
void MI_Infra(Missile &missile);
void MI_Apoca(Missile &missile);
void MI_Wave(Missile &missile);
void MI_Nova(Missile &missile);
void MI_Blodboil(Missile &missile);
void MI_Flame(Missile &missile);
void MI_Flamec(Missile &missile);
void MI_Cbolt(Missile &missile);
void MI_Hbolt(Missile &missile);
void MI_Element(Missile &missile);
void MI_Bonespirit(Missile &missile);
void MI_ResurrectBeam(Missile &missile);
void MI_Rportal(Missile &missile);
void ProcessMissiles();
void missiles_process_charge();
void RedoMissileFlags();

} // namespace devilution
