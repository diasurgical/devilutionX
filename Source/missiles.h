/**
 * @file missiles.h
 *
 * Interface of missile functionality.
 */
#pragma once

#include <cstdint>
#include <list>

#include "engine.h"
#include "engine/point.hpp"
#include "miniwin/miniwin.h"
#include "misdat.h"
#include "monster.h"
#include "spelldat.h"

namespace devilution {

constexpr Point GolemHoldingCell = Point { 1, 0 };

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

/**
 * Represent a more fine-grained direction than the 8 value Direction enum.
 *
 * This is used when rendering projectiles like arrows which have additional sprites for "half-winds" on a 16-point compass.
 * The sprite sheets are typically 0-indexed and use the following layout (relative to the screen projection)
 *
 *      W  WSW   SW  SSW  S
 *               ^
 *     WNW       |       SSE
 *               |
 *     NW -------+------> SE
 *               |
 *     NNW       |       ESE
 *               |
 *      N  NNE   NE  ENE  E
 */
enum class Direction16 {
	South,
	South_SouthWest,
	SouthWest,
	West_SouthWest,
	West,
	West_NorthWest,
	NorthWest,
	North_NorthWest,
	North,
	North_NorthEast,
	NorthEast,
	East_NorthEast,
	East,
	East_SouthEast,
	SouthEast,
	South_SouthEast,
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

extern std::list<Missile> Missiles;
extern bool MissilePreFlag;

void GetDamageAmt(int i, int *mind, int *maxd);
int GetSpellLevel(int playerId, spell_id sn);

/**
 * @brief Returns the direction a vector from p1(x1, y1) to p2(x2, y2) is pointing to.
 *
 *      W  sW  SW   Sw  S
 *              ^
 *     nW       |       Se
 *              |
 *     NW ------+-----> SE
 *              |
 *     Nw       |       sE
 *              |
 *      N  Ne  NE   nE  E
 *
 * @param x1 the x coordinate of p1
 * @param y1 the y coordinate of p1
 * @param x2 the x coordinate of p2
 * @param y2 the y coordinate of p2
 * @return the direction of the p1->p2 vector
 */
Direction16 GetDirection16(Point p1, Point p2);
bool MonsterTrapHit(int m, int mindam, int maxdam, int dist, missile_id t, bool shift);
bool PlayerMHit(int pnum, Monster *monster, int dist, int mind, int maxd, missile_id mtype, bool shift, int earflag, bool *blocked);

/**
 * @brief Could the missile collide with solid objects? (like walls or closed doors)
 */
bool IsMissileBlockedByTile(Point position);

/**
 * @brief Sets the missile sprite to the given sheet frame
 * @param missile this object
 * @param dir Sprite frame, typically representing a direction but there are some exceptions (arrows being 1 indexed, directionless spells)
 */
void SetMissDir(Missile &missile, int dir);

/**
 * @brief Sets the sprite for this missile so it matches the given Direction
 * @param missile this object
 * @param dir Desired facing
 */
inline void SetMissDir(Missile &missile, Direction dir)
{
	SetMissDir(missile, static_cast<int>(dir));
}

/**
 * @brief Sets the sprite for this missile so it matches the given Direction16
 * @param missile this object
 * @param dir Desired facing at a 22.8125 degree resolution
 */
inline void SetMissDir(Missile &missile, Direction16 dir)
{
	SetMissDir(missile, static_cast<int>(dir));
}

void InitMissiles();

struct AddMissileParameter {
	Point dst;
	Direction midir;
	Missile *pParent;
};

void AddHiveExplosion(Missile &missile, const AddMissileParameter &parameter);
void AddFireRune(Missile &missile, const AddMissileParameter &parameter);
void AddLightningRune(Missile &missile, const AddMissileParameter &parameter);
void AddGreatLightningRune(Missile &missile, const AddMissileParameter &parameter);
void AddImmolationRune(Missile &missile, const AddMissileParameter &parameter);
void AddStoneRune(Missile &missile, const AddMissileParameter &parameter);
void AddReflect(Missile &missile, const AddMissileParameter &parameter);
void AddBerserk(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: Direction to place the spawn
 */
void AddHorkSpawn(Missile &missile, const AddMissileParameter &parameter);
void AddJester(Missile &missile, const AddMissileParameter &parameter);
void AddStealPotions(Missile &missile, const AddMissileParameter &parameter);
void AddManaTrap(Missile &missile, const AddMissileParameter &parameter);
void AddSpecArrow(Missile &missile, const AddMissileParameter &parameter);
void AddWarp(Missile &missile, const AddMissileParameter &parameter);
void AddLightningWall(Missile &missile, const AddMissileParameter &parameter);
void AddRuneExplosion(Missile &missile, const AddMissileParameter &parameter);
void AddImmolation(Missile &missile, const AddMissileParameter &parameter);
void AddBowLightning(Missile &missile, const AddMissileParameter &parameter);
void AddMana(Missile &missile, const AddMissileParameter &parameter);
void AddMagi(Missile &missile, const AddMissileParameter &parameter);
void AddRingOfFire(Missile &missile, const AddMissileParameter &parameter);
void AddSearch(Missile &missile, const AddMissileParameter &parameter);
void AddBowChargedBolt(Missile &missile, const AddMissileParameter &parameter);
void AddElementArrow(Missile &missile, const AddMissileParameter &parameter);
void AddArrow(Missile &missile, const AddMissileParameter &parameter);
void AddPhasing(Missile &missile, const AddMissileParameter &parameter);
void AddFirebolt(Missile &missile, const AddMissileParameter &parameter);
void AddMagmaBall(Missile &missile, const AddMissileParameter &parameter);
void AddTeleport(Missile &missile, const AddMissileParameter &parameter);
void AddNova(Missile &missile, const AddMissileParameter &parameter);
void AddFireWall(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile-light
 * var2: Y coordinate of the missile-light
 * var4: X coordinate of the missile-light
 * var5: Y coordinate of the missile-light
 */
void AddFireball(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile
 * var2: Y coordinate of the missile
 */
void AddLightningC(Missile &missile, const AddMissileParameter &parameter);
void AddLightning(Missile &missile, const AddMissileParameter &parameter);
void AddMissileHit(Missile &missile, const AddMissileParameter &parameter);
void AddElementMeleeHit(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: Animation
 */
void AddTownPortal(Missile &missile, const AddMissileParameter &parameter);
void AddFlash(Missile &missile, const AddMissileParameter &parameter);
void AddFlash2(Missile &missile, const AddMissileParameter &parameter);
void AddManaShield(Missile &missile, const AddMissileParameter &parameter);
void AddFlameWave(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: Animation
 * var3: Light strength
 */
void AddGuardian(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: X coordinate of the destination
 * var2: Y coordinate of the destination
 */
void AddChainLightning(Missile &missile, const AddMissileParameter &parameter);
void AddRhino(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile-light
 * var2: Y coordinate of the missile-light
 */
void AddBloodStar(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile-light
 * var2: Y coordinate of the missile-light
 */
void AddAcid(Missile &missile, const AddMissileParameter &parameter);
void AddAcidPuddle(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: mmode of the monster
 * var2: mnum of the monster
 */
void AddStoneCurse(Missile &missile, const AddMissileParameter &parameter);
void AddGolem(Missile &missile, const AddMissileParameter &parameter);
void AddApocalypse(Missile &missile, const AddMissileParameter &parameter);
void AddHealing(Missile &missile, const AddMissileParameter &parameter);
void AddHealOther(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: X coordinate of the missile-light
 * var2: Y coordinate of the missile-light
 * var4: X coordinate of the destination
 * var5: Y coordinate of the destination
 */
void AddElemental(Missile &missile, const AddMissileParameter &parameter);
void AddIdentify(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: X coordinate of the first wave
 * var2: Y coordinate of the first wave
 * var3: Direction of the first wave
 * var4: Direction of the second wave
 * var5: X coordinate of the second wave
 * var6: Y coordinate of the second wave
 */
void AddFireWallC(Missile &missile, const AddMissileParameter &parameter);
void AddInfravision(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: X coordinate of the destination
 * var2: Y coordinate of the destination
 */
void AddFlameWaveC(Missile &missile, const AddMissileParameter &parameter);
void AddImmolationC(Missile &missile, const AddMissileParameter &parameter);
void AddRage(Missile &missile, const AddMissileParameter &parameter);
void AddItemRepair(Missile &missile, const AddMissileParameter &parameter);
void AddStaffRecharge(Missile &missile, const AddMissileParameter &parameter);
void AddDisarmTrap(Missile &missile, const AddMissileParameter &parameter);
void AddApocalypseC(Missile &missile, const AddMissileParameter &parameter);
void AddInferno(Missile &missile, const AddMissileParameter &parameter);
void AddInfernoC(Missile &missile, const AddMissileParameter &parameter);

/**
 * var1: Light strength
 * var2: Base direction
 */
void AddChargedBolt(Missile &missile, const AddMissileParameter &parameter);
void AddHolyBolt(Missile &missile, const AddMissileParameter &parameter);
void AddResurrect(Missile &missile, const AddMissileParameter &parameter);
void AddResurrectBeam(Missile &missile, const AddMissileParameter &parameter);
void AddTelekinesis(Missile &missile, const AddMissileParameter &parameter);
void AddBoneSpirit(Missile &missile, const AddMissileParameter &parameter);
void AddRedPortal(Missile &missile, const AddMissileParameter &parameter);
void AddDApocalypseC(Missile &missile, const AddMissileParameter &parameter);
Missile *AddMissile(Point src, Point dst, Direction midir, missile_id mitype, mienemy_type micaster, int id, int midam, int spllvl, Missile *pParent = nullptr);
void MI_Golem(Missile &missile);
void MI_ManaShield(Missile &missile);
void MI_ElementArrow(Missile &missile);
void MI_Arrow(Missile &missile);
void MI_Projectile(Missile &missile);
void MI_Nova(Missile &missilei);
void MI_AcidPuddle(Missile &missile);
void MI_FireWall(Missile &missile);
void MI_Fireball(Missile &missile);
void MI_HorkSpawn(Missile &missile);
void MI_Rune(Missile &missile);
void MI_LightningWall(Missile &missile);
void MI_HiveExplode(Missile &missile);
void MI_BowLightning(Missile &missile);
void MI_RingOfFire(Missile &missile);
void MI_Search(Missile &missile);
void MI_LightningWallC(Missile &missile);
void MI_ImmolationC(Missile &missile);
void MI_SpecArrow(Missile &missile);
void MI_LightningC(Missile &missile);
void MI_Lightning(Missile &missile);
void MI_TownPortal(Missile &missile);
void MI_Flash(Missile &missile);
void MI_Flash2(Missile &missile);
void MI_FlameWave(Missile &missile);
void MI_Guardian(Missile &missile);
void MI_ChainLightning(Missile &missile);
void MI_ElementMeleeHit(Missile &missile);
void MI_MissileHit(Missile &missile);
void MI_AcidHit(Missile &missile);
void MI_Teleport(Missile &missile);
void MI_StoneCurse(Missile &missile);
void MI_Apocalypse(Missile &missile);
void MI_Rhino(Missile &missile);
void MI_FireWallC(Missile &missile);
void MI_Infravision(Missile &missile);
void MI_ApocalypseC(Missile &missile);
void MI_FlameWaveC(Missile &missile);
void MI_Immolation1(Missile &missile);
void MI_Rage(Missile &missile);
void MI_Inferno(Missile &missile);
void MI_InfernoC(Missile &missile);
void MI_ChargedBolt(Missile &missile);
void MI_HolyBolt(Missile &missile);
void MI_Elemental(Missile &missile);
void MI_BoneSpirit(Missile &missile);
void MI_ResurrectBeam(Missile &missile);
void MI_RedPortal(Missile &missile);
void ProcessMissiles();
void missiles_process_charge();
void RedoMissileFlags();

} // namespace devilution
