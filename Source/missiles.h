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
