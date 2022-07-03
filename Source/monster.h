/**
 * @file monster.h
 *
 * Interface of monster functionality, AI, actions, spawning, loading, etc.
 */
#pragma once

#include <array>
#include <cstdint>
#include <functional>

#include "engine.h"
#include "engine/actor_position.hpp"
#include "engine/animationinfo.h"
#include "engine/cel_sprite.hpp"
#include "engine/point.hpp"
#include "engine/sound.h"
#include "engine/world_tile.hpp"
#include "miniwin/miniwin.h"
#include "monstdat.h"
#include "spelldat.h"
#include "textdat.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

struct Missile;

constexpr int MaxMonsters = 200;
constexpr int MaxLvlMTypes = 24;

enum monster_flag : uint16_t {
	// clang-format off
	MFLAG_HIDDEN          = 1 << 0,
	MFLAG_LOCK_ANIMATION  = 1 << 1,
	MFLAG_ALLOW_SPECIAL   = 1 << 2,
	MFLAG_NOHEAL          = 1 << 3,
	MFLAG_TARGETS_MONSTER = 1 << 4,
	MFLAG_GOLEM           = 1 << 5,
	MFLAG_QUEST_COMPLETE  = 1 << 6,
	MFLAG_KNOCKBACK       = 1 << 7,
	MFLAG_SEARCH          = 1 << 8,
	MFLAG_CAN_OPEN_DOOR   = 1 << 9,
	MFLAG_NO_ENEMY        = 1 << 10,
	MFLAG_BERSERK         = 1 << 11,
	MFLAG_NOLIFESTEAL     = 1 << 12,
	// clang-format on
};

/** Indexes from UniqueMonstersData array for special unique monsters (usually quest related) */
enum : uint8_t {
	UMT_GARBUD,
	UMT_SKELKING,
	UMT_ZHAR,
	UMT_SNOTSPIL,
	UMT_LAZARUS,
	UMT_RED_VEX,
	UMT_BLACKJADE,
	UMT_LACHDAN,
	UMT_WARLORD,
	UMT_BUTCHER,
	UMT_HORKDMN,
	UMT_DEFILER,
	UMT_NAKRUL,
};

enum class MonsterMode : uint8_t {
	Stand,
	/** Movement towards N, NW, or NE */
	MoveNorthwards,
	/** Movement towards S, SW, or SE */
	MoveSouthwards,
	/** Movement towards W or E */
	MoveSideways,
	MeleeAttack,
	HitRecovery,
	Death,
	SpecialMeleeAttack,
	FadeIn,
	FadeOut,
	RangedAttack,
	SpecialStand,
	SpecialRangedAttack,
	Delay,
	Charge,
	Petrified,
	Heal,
	Talk,
};

enum class MonsterGraphic : uint8_t {
	Stand,
	Walk,
	Attack,
	GotHit,
	Death,
	Special,
};

enum monster_goal : uint8_t {
	MGOAL_NONE,
	MGOAL_NORMAL,
	MGOAL_RETREAT,
	MGOAL_HEALING,
	MGOAL_MOVE,
	MGOAL_ATTACK2,
	MGOAL_INQUIRING,
	MGOAL_TALKING,
};

enum placeflag : uint8_t {
	// clang-format off
	PLACE_SCATTER = 1 << 0,
	PLACE_SPECIAL = 1 << 1,
	PLACE_UNIQUE  = 1 << 2,
	// clang-format on
};

/**
 * @brief Defines the relation of the monster to a monster pack.
 *        If value is different from Individual Monster, the leader must also be set
 */
enum class LeaderRelation : uint8_t {
	None,
	/**
	 * @brief Minion that sticks to the leader
	 */
	Leashed,
	/**
	 * @brief Minion that was separated from the leader and acts individually until it reaches the leader again
	 */
	Separated,
};

struct AnimStruct {
	[[nodiscard]] std::optional<CelSprite> getCelSpritesForDirection(Direction direction) const
	{
		const byte *spriteData = celSpritesForDirections[static_cast<size_t>(direction)];
		if (spriteData == nullptr)
			return std::nullopt;
		return CelSprite(spriteData, width);
	}

	std::array<byte *, 8> celSpritesForDirections;
	uint16_t width;
	int frames;
	int rate;
};

struct CMonster {
	_monster_id type;
	/** placeflag enum as a flags*/
	uint8_t placeFlags;
	std::unique_ptr<byte[]> animData;
	AnimStruct anims[6];
	/**
	 * @brief Returns AnimStruct for specified graphic
	 */
	const AnimStruct &getAnimData(MonsterGraphic graphic) const
	{
		return anims[static_cast<int>(graphic)];
	}
	std::unique_ptr<TSnd> sounds[4][2];
	int8_t corpseId;
	const MonsterData *data;
};

struct Monster { // note: missing field _mAFNum
	const char *name;
	CMonster *type;
	const MonsterData *data;
	std::unique_ptr<uint8_t[]> uniqueMonsterTRN;
	AnimationInfo animInfo;
	/** Specifies current goal of the monster */
	monster_goal goal;
	/** Specifies monster's behaviour regarding moving and changing goals. */
	int goalGeneral;
	/** @brief Specifies turning direction for @p RoundWalk in most cases.
	 *  Used in custom way by @p FallenAi, @p SnakeAi, @p M_FallenFear and @p FallenAi.
	 */
	int goalTurning;
	/** @brief Controls monster's behaviour regarding special actions.
	 *  Used only by @p ScavengerAi and @p MegaAi.
	 */
	int goalSpecialAction;
	int var1;
	int var2;
	int var3;
	int maxHp;
	int hitPoints;
	uint32_t flags;
	/** Seed used to determine item drops on death */
	uint32_t rndItemSeed;
	/** Seed used to determine AI behaviour/sync sounds in multiplayer games? */
	uint32_t aiSeed;
	uint16_t exp;
	uint16_t hit;
	uint16_t hit2;
	uint16_t magicRes;
	_speech_id talkMsg;
	ActorPosition position;

	/** Usually corresponds to the enemy's future position */
	WorldTilePosition enemyPosition;
	uint8_t levelType;
	MonsterMode mode;
	uint8_t pathCount;
	/** Direction faced by monster (direction enum) */
	Direction direction;
	/** The current target of the monster. An index in to either the player or monster array based on the _meflag value. */
	uint8_t enemy;

	/**
	 * @brief Contains information for current animation
	 */
	bool invalidate;

	_mai_id ai;
	/** Specifies monster behaviour across various actions */
	uint8_t intelligence;
	uint8_t activityTicks;
	uint8_t uniqType;
	uint8_t uniqTrans;
	int8_t corpseId;
	int8_t whoHit;
	int8_t level;
	uint8_t minDamage;
	uint8_t maxDamage;
	uint8_t minDamage2;
	uint8_t maxDamage2;
	uint8_t armorClass;
	uint8_t leader;
	LeaderRelation leaderRelation;
	uint8_t packSize;
	int8_t lightId;

	/**
	 * @brief Sets the current cell sprite to match the desired desiredDirection and animation sequence
	 * @param graphic Animation sequence of interest
	 * @param desiredDirection Desired desiredDirection the monster should be visually facing
	 */
	void changeAnimationData(MonsterGraphic graphic, Direction desiredDirection)
	{
		auto &animationData = this->type->getAnimData(graphic);

		// Passing the Frames and rate properties here is only relevant when initialising a monster, but doesn't cause any harm when switching animations.
		this->animInfo.ChangeAnimationData(animationData.getCelSpritesForDirection(desiredDirection), animationData.frames, animationData.rate);
	}

	/**
	 * @brief Sets the current cell sprite to match the desired animation sequence using the direction the monster is currently facing
	 * @param graphic Animation sequence of interest
	 */
	void changeAnimationData(MonsterGraphic graphic)
	{
		this->changeAnimationData(graphic, this->direction);
	}

	/**
	 * @brief Check if the correct stand Animation is loaded. This is needed if direction is changed (monster stands and looks at the player).
	 * @param dir direction of the monster
	 */
	void checkStandAnimationIsLoaded(Direction dir);

	/**
	 * @brief Sets mode to MonsterMode::Petrified
	 */
	void petrify();

	/**
	 * @brief Is the monster currently walking?
	 */
	bool isWalking() const;
	bool isImmune(missile_id mitype) const;
	bool isResistant(missile_id mitype) const;
	bool isPossibleToHit() const;
	bool tryLiftGargoyle();
};

extern CMonster LevelMonsterTypes[MaxLvlMTypes];
extern int LevelMonsterTypeCount;
extern Monster Monsters[MaxMonsters];
extern int ActiveMonsters[MaxMonsters];
extern int ActiveMonsterCount;
extern int MonsterKillCounts[MaxMonsters];
extern bool sgbSaveSoundOn;

void PrepareUniqueMonst(Monster &monster, int uniqindex, int miniontype, int bosspacksize, const UniqueMonsterData &uniqueMonsterData);
void InitLevelMonsters();
void GetLevelMTypes();
void InitMonsterGFX(int monsterTypeIndex);
void WeakenNaKrul();
void InitGolems();
void InitMonsters();
void SetMapMonsters(const uint16_t *dunData, Point startPosition);
int AddMonster(Point position, Direction dir, int mtype, bool inMap);
void AddDoppelganger(Monster &monster);
bool M_Talker(const Monster &monster);
void M_StartStand(Monster &monster, Direction md);
void M_ClearSquares(int monsterId);
void M_GetKnockback(int monsterId);
void M_StartHit(int monsterId, int dam);
void M_StartHit(int monsterId, int pnum, int dam);
void StartMonsterDeath(int monsterId, int pnum, bool sendmsg);
void M_StartKill(int monsterId, int pnum);
void M_SyncStartKill(int monsterId, Point position, int pnum);
void M_UpdateLeader(int monsterId);
void DoEnding();
void PrepDoEnding();
void M_WalkDir(int monsterId, Direction md);
void GolumAi(int monsterId);
void DeleteMonsterList();
void ProcessMonsters();
void FreeMonsters();
bool DirOK(int monsterId, Direction mdir);
bool PosOkMissile(Point position);
bool LineClearMissile(Point startPoint, Point endPoint);
bool LineClear(const std::function<bool(Point)> &clear, Point startPoint, Point endPoint);
void SyncMonsterAnim(Monster &monster);
void M_FallenFear(Point position);
void PrintMonstHistory(int mt);
void PrintUniqueHistory();
void PlayEffect(Monster &monster, int mode);
void MissToMonst(Missile &missile, Point position);

/**
 * @brief Check that the given tile is available to the monster
 */
bool IsTileAvailable(const Monster &monster, Point position);
bool IsSkel(_monster_id mt);
bool IsGoat(_monster_id mt);
bool SpawnSkeleton(int ii, Point position);
int PreSpawnSkeleton();
void TalktoMonster(Monster &monster);
void SpawnGolem(int id, Point position, Missile &missile);
bool CanTalkToMonst(const Monster &monster);
int encode_enemy(Monster &monster);
void decode_enemy(Monster &monster, int enemyId);

} // namespace devilution
