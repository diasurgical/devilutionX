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

/** This enum contains indexes from UniqueMonstersData array for special unique monsters (usually quest related) */
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
	[[nodiscard]] OptionalCelSprite getCelSpritesForDirection(Direction direction) const
	{
		const byte *spriteData = celSpritesForDirections[static_cast<size_t>(direction)];
		if (spriteData == nullptr)
			return std::nullopt;
		return CelSprite(spriteData, width);
	}

	std::array<byte *, 8> celSpritesForDirections;
	uint16_t width;
	int8_t frames;
	int8_t rate;
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

extern CMonster LevelMonsterTypes[MaxLvlMTypes];

struct Monster { // note: missing field _mAFNum
	const char *mName;
	std::unique_ptr<uint8_t[]> uniqueTRN;
	AnimationInfo AnimInfo;
	int _mgoalvar1;
	int _mgoalvar2;
	int _mgoalvar3;
	int _mVar1;
	int _mVar2;
	int _mVar3;
	int _mmaxhp;
	int _mhitpoints;
	uint32_t _mFlags;
	/** Seed used to determine item drops on death */
	uint32_t _mRndSeed;
	/** Seed used to determine AI behaviour/sync sounds in multiplayer games? */
	uint32_t _mAISeed;

	uint16_t mExp;
	uint16_t mHit;
	uint16_t mHit2;
	uint16_t mMagicRes;
	_speech_id mtalkmsg;
	ActorPosition position;

	/** Usually corresponds to the enemy's future position */
	WorldTilePosition enemyPosition;
	uint8_t _mMTidx;
	MonsterMode _mmode;
	monster_goal _mgoal;
	uint8_t _pathcount;
	/** Direction faced by monster (direction enum) */
	Direction _mdir;
	/** The current target of the monster. An index in to either the plr or monster array based on the _meflag value. */
	uint8_t _menemy;

	/**
	 * @brief Contains information for current animation
	 */
	bool _mDelFlag;

	_mai_id _mAi;
	uint8_t _mint;
	uint8_t _msquelch;
	uint8_t _uniqtype;
	uint8_t _uniqtrans;
	int8_t _udeadval;
	int8_t mWhoHit;
	int8_t mLevel;
	uint8_t mMinDamage;
	uint8_t mMaxDamage;
	uint8_t mMinDamage2;
	uint8_t mMaxDamage2;
	uint8_t mArmorClass;
	uint8_t leader;
	LeaderRelation leaderRelation;
	uint8_t packsize;
	int8_t mlid; // BUGFIX -1 is used when not emitting light this should be signed (fixed)

	/**
	 * @brief Sets the current cell sprite to match the desired direction and animation sequence
	 * @param graphic Animation sequence of interest
	 * @param direction Desired direction the monster should be visually facing
	 */
	void ChangeAnimationData(MonsterGraphic graphic, Direction direction)
	{
		auto &animationData = type().getAnimData(graphic);

		// Passing the Frames and rate properties here is only relevant when initialising a monster, but doesn't cause any harm when switching animations.
		this->AnimInfo.changeAnimationData(animationData.getCelSpritesForDirection(direction), animationData.frames, animationData.rate);
	}

	/**
	 * @brief Sets the current cell sprite to match the desired animation sequence using the direction the monster is currently facing
	 * @param graphic Animation sequence of interest
	 */
	void ChangeAnimationData(MonsterGraphic graphic)
	{
		this->ChangeAnimationData(graphic, this->_mdir);
	}

	/**
	 * @brief Check thats the correct stand Animation is loaded. This is needed if direction is changed (monster stands and looks to player).
	 * @param mdir direction of the monster
	 */
	void CheckStandAnimationIsLoaded(Direction mdir);

	/**
	 * @brief Sets _mmode to MonsterMode::Petrified
	 */
	void Petrify();

	const CMonster &type() const
	{
		return LevelMonsterTypes[_mMTidx];
	}

	const MonsterData &data() const
	{
		return *type().data;
	}

	/**
	 * @brief Returns the network identifier for this monster
	 *
	 * This is currently the index into the Monsters array, but may change in the future.
	 */
	[[nodiscard]] size_t getId() const;

	/**
	 * @brief Is the monster currently walking?
	 */
	bool IsWalking() const;
	bool IsImmune(missile_id mitype) const;
	bool IsResistant(missile_id mitype) const;
	bool IsPossibleToHit() const;
	bool TryLiftGargoyle();
};

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
void M_ClearSquares(const Monster &monster);
void M_GetKnockback(Monster &monster);
void M_StartHit(int monsterId, int dam);
void M_StartHit(int monsterId, int pnum, int dam);
void StartMonsterDeath(int monsterId, int pnum, bool sendmsg);
void M_StartKill(int monsterId, int pnum);
void M_SyncStartKill(int monsterId, Point position, int pnum);
void M_UpdateLeader(int monsterId);
void DoEnding();
void PrepDoEnding();
void M_WalkDir(Monster &monster, Direction md);
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

Monster *MonsterAtPosition(Point position);

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
