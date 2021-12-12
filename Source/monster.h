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
#include "miniwin/miniwin.h"
#include "monstdat.h"
#include "sound.h"
#include "spelldat.h"
#include "textdat.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

struct Missile;

#define MAXMONSTERS 200
#define MAX_LVLMTYPES 24

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

/** this enum contains indexes from UniqueMonstersData array for special unique monsters (usually quest related) */
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

enum class MonsterMode {
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

enum class MonsterGraphic {
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
 *        If value is differnt from Individual Monster.leader must also be set
 */
enum class LeaderRelation : uint8_t {
	None,
	/**
	 * @brief Minion that sticks with the leader
	 */
	Leashed,
	/**
	 * @brief Minion that was separated from leader and acts individual until it reaches the leader again
	 */
	Separated,
};

struct AnimStruct {
	std::unique_ptr<byte[]> CMem;
	std::array<std::optional<CelSprite>, 8> CelSpritesForDirections;

	inline const std::optional<CelSprite> &GetCelSpritesForDirection(Direction direction) const
	{
		return CelSpritesForDirections[static_cast<size_t>(direction)];
	}

	int Frames;
	int Rate;
};

struct CMonster {
	_monster_id mtype;
	/** placeflag enum as a flags*/
	uint8_t mPlaceFlags;
	AnimStruct Anims[6];
	/**
	 * @brief Returns AnimStruct for specified graphic
	 */
	const AnimStruct &GetAnimData(MonsterGraphic graphic) const
	{
		return Anims[static_cast<int>(graphic)];
	}
	std::unique_ptr<TSnd> Snds[4][2];
	uint16_t mMinHP;
	uint16_t mMaxHP;
	uint8_t mAFNum;
	int8_t mdeadval;
	const MonsterData *MData;
};

struct Monster { // note: missing field _mAFNum
	int _mMTidx;
	MonsterMode _mmode;
	monster_goal _mgoal;
	int _mgoalvar1;
	int _mgoalvar2;
	int _mgoalvar3;
	uint8_t _pathcount;
	ActorPosition position;
	/** Direction faced by monster (direction enum) */
	Direction _mdir;
	/** The current target of the mosnter. An index in to either the plr or monster array based on the _meflag value. */
	int _menemy;
	/** Usually correspond's to the enemy's future position */
	Point enemyPosition;
	/**
	 * @brief Contains Information for current Animation
	 */
	AnimationInfo AnimInfo;
	bool _mDelFlag;
	int _mVar1;
	int _mVar2;
	int _mVar3;
	int _mmaxhp;
	int _mhitpoints;
	_mai_id _mAi;
	uint8_t _mint;
	uint32_t _mFlags;
	uint8_t _msquelch;
	/** Seed used to determine item drops on death */
	uint32_t _mRndSeed;
	/** Seed used to determine AI behaviour/sync sounds in multiplayer games? */
	uint32_t _mAISeed;
	uint8_t _uniqtype;
	uint8_t _uniqtrans;
	int8_t _udeadval;
	int8_t mWhoHit;
	int8_t mLevel;
	uint16_t mExp;
	uint16_t mHit;
	uint8_t mMinDamage;
	uint8_t mMaxDamage;
	uint16_t mHit2;
	uint8_t mMinDamage2;
	uint8_t mMaxDamage2;
	uint8_t mArmorClass;
	uint16_t mMagicRes;
	_speech_id mtalkmsg;
	uint8_t leader;
	LeaderRelation leaderRelation;
	uint8_t packsize;
	int8_t mlid; // BUGFIX -1 is used when not emitting light this should be signed (fixed)
	const char *mName;
	CMonster *MType;
	const MonsterData *MData;

	/**
	 * @brief Sets the current cell sprite to match the desired direction and animation sequence
	 * @param graphic Animation sequence of interest
	 * @param direction Desired direction the monster should be visually facing
	 */
	void ChangeAnimationData(MonsterGraphic graphic, Direction direction)
	{
		auto &animationData = this->MType->GetAnimData(graphic);
		auto &celSprite = animationData.GetCelSpritesForDirection(direction);

		// Passing the Frames and Rate properties here is only relevant when initialising a monster, but doesn't cause any harm when switching animations.
		this->AnimInfo.ChangeAnimationData(celSprite ? &*celSprite : nullptr, animationData.Frames, animationData.Rate);
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

	/**
	 * @brief Is the monster currently walking?
	 */
	bool IsWalking() const;
};

extern CMonster LevelMonsterTypes[MAX_LVLMTYPES];
extern int LevelMonsterTypeCount;
extern Monster Monsters[MAXMONSTERS];
extern int ActiveMonsters[MAXMONSTERS];
extern int ActiveMonsterCount;
extern int MonsterKillCounts[MAXMONSTERS];
extern bool sgbSaveSoundOn;

void InitLevelMonsters();
void GetLevelMTypes();
void InitMonsterGFX(int monst);
void monster_some_crypt();
void InitGolems();
void InitMonsters();
void SetMapMonsters(const uint16_t *dunData, Point startPosition);
int AddMonster(Point position, Direction dir, int mtype, bool inMap);
void AddDoppelganger(Monster &monster);
bool M_Talker(const Monster &monster);
void M_StartStand(Monster &monster, Direction md);
void M_ClearSquares(int i);
void M_GetKnockback(int i);
void M_StartHit(int i, int pnum, int dam);
void M_StartKill(int i, int pnum);
void M_SyncStartKill(int i, Point position, int pnum);
void M_UpdateLeader(int i);
void DoEnding();
void PrepDoEnding();
void M_WalkDir(int i, Direction md);
void GolumAi(int i);
void DeleteMonsterList();
void ProcessMonsters();
void FreeMonsters();
bool DirOK(int i, Direction mdir);
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
bool IsSkel(int mt);
bool IsGoat(int mt);
bool SpawnSkeleton(int ii, Point position);
int PreSpawnSkeleton();
void TalktoMonster(Monster &monster);
void SpawnGolem(int i, Point position, Missile &missile);
bool CanTalkToMonst(const Monster &monster);
bool CheckMonsterHit(Monster &monster, bool *ret);
int encode_enemy(Monster &monster);
void decode_enemy(Monster &monster, int enemyId);

} // namespace devilution
