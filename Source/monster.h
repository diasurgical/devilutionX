/**
 * @file monster.h
 *
 * Interface of monster functionality, AI, actions, spawning, loading, etc.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include <array>
#include <functional>

#include "engine.h"
#include "engine/actor_position.hpp"
#include "engine/animationinfo.h"
#include "engine/cel_sprite.hpp"
#include "engine/point.hpp"
#include "engine/sound.h"
#include "engine/world_tile.hpp"
#include "monstdat.h"
#include "spelldat.h"
#include "textdat.h"
#include "utils/language.h"

namespace devilution {

struct Missile;
struct Player;

constexpr size_t MaxMonsters = 200;
constexpr size_t MaxLvlMTypes = 24;

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
enum class UniqueMonsterType : uint8_t {
	Garbud,
	SkeletonKing,
	Zhar,
	SnotSpill,
	Lazarus,
	RedVex,
	BlackJade,
	Lachdan,
	WarlordOfBlood,
	Butcher,
	HorkDemon,
	Defiler,
	NaKrul,
	None = static_cast<uint8_t>(-1),
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

enum class MonsterGoal : uint8_t {
	None,
	Normal,
	Retreat,
	Healing,
	Move,
	Attack,
	Inquiring,
	Talking,
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

enum class MonsterSound {
	Attack,
	Hit,
	Death,
	Special
};

struct CMonster {
	std::unique_ptr<byte[]> animData;
	AnimStruct anims[6];
	std::unique_ptr<TSnd> sounds[4][2];
	const MonsterData *data;

	_monster_id type;
	/** placeflag enum as a flags*/
	uint8_t placeFlags;
	int8_t corpseId;

	/**
	 * @brief Returns AnimStruct for specified graphic
	 */
	const AnimStruct &getAnimData(MonsterGraphic graphic) const
	{
		return anims[static_cast<int>(graphic)];
	}
};

extern CMonster LevelMonsterTypes[MaxLvlMTypes];

struct Monster { // note: missing field _mAFNum
	std::unique_ptr<uint8_t[]> uniqueMonsterTRN;
	/**
	 * @brief Contains information for current animation
	 */
	AnimationInfo animInfo;
	int maxHitPoints;
	int hitPoints;
	uint32_t flags;
	/** Seed used to determine item drops on death */
	uint32_t rndItemSeed;
	/** Seed used to determine AI behaviour/sync sounds in multiplayer games? */
	uint32_t aiSeed;
	uint16_t exp;
	uint16_t toHit;
	uint16_t toHitSpecial;
	uint16_t resistance;
	_speech_id talkMsg;

	/** @brief Specifies monster's behaviour regarding moving and changing goals. */
	int16_t goalVar1;

	/**
	 * @brief Specifies turning direction for @p RoundWalk in most cases.
	 * Used in custom way by @p FallenAi, @p SnakeAi, @p M_FallenFear and @p FallenAi.
	 */
	int8_t goalVar2;

	/**
	 * @brief Controls monster's behaviour regarding special actions.
	 * Used only by @p ScavengerAi and @p MegaAi.
	 */
	int8_t goalVar3;

	int16_t var1;
	int16_t var2;
	int8_t var3;

	ActorPosition position;

	/** Specifies current goal of the monster */
	MonsterGoal goal;

	/** Usually corresponds to the enemy's future position */
	WorldTilePosition enemyPosition;
	uint8_t levelType;
	MonsterMode mode;
	uint8_t pathCount;
	/** Direction faced by monster (direction enum) */
	Direction direction;
	/** The current target of the monster. An index in to either the player or monster array based on the _meflag value. */
	uint8_t enemy;
	bool isInvalid;
	_mai_id ai;
	/**
	 * @brief Specifies monster's behaviour across various actions.
	 * Generally, when monster thinks it decides what to do based on this value, among other things.
	 * Higher values should result in more aggressive behaviour (e.g. some monsters use this to calculate the @p AiDelay).
	 */
	uint8_t intelligence;
	/** Stores information for how many ticks the monster will remain active */
	uint8_t activeForTicks;
	UniqueMonsterType uniqueType;
	uint8_t uniqTrans;
	int8_t corpseId;
	int8_t whoHit;
	int8_t level;
	uint8_t minDamage;
	uint8_t maxDamage;
	uint8_t minDamageSpecial;
	uint8_t maxDamageSpecial;
	uint8_t armorClass;
	uint8_t leader;
	LeaderRelation leaderRelation;
	uint8_t packSize;
	int8_t lightId;

	static constexpr uint8_t NoLeader = -1;

	/**
	 * @brief Sets the current cell sprite to match the desired desiredDirection and animation sequence
	 * @param graphic Animation sequence of interest
	 * @param desiredDirection Desired desiredDirection the monster should be visually facing
	 */
	void changeAnimationData(MonsterGraphic graphic, Direction desiredDirection)
	{
		auto &animationData = type().getAnimData(graphic);

		// Passing the frames and rate properties here is only relevant when initialising a monster, but doesn't cause any harm when switching animations.
		this->animInfo.changeAnimationData(animationData.getCelSpritesForDirection(desiredDirection), animationData.frames, animationData.rate);
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

	const CMonster &type() const
	{
		return LevelMonsterTypes[levelType];
	}

	const MonsterData &data() const
	{
		return *type().data;
	}

	/**
	 * @brief Returns monster's name
	 * Internally it returns a name stored in global array of monsters' data.
	 * @return Monster's name
	 */
	string_view name() const
	{
		if (uniqueType != UniqueMonsterType::None)
			return pgettext("monster", UniqueMonstersData[static_cast<int8_t>(uniqueType)].mName);

		return pgettext("monster", data().name);
	}

	/**
	 * @brief Returns the network identifier for this monster
	 *
	 * This is currently the index into the Monsters array, but may change in the future.
	 */
	[[nodiscard]] size_t getId() const;

	[[nodiscard]] Monster *getLeader() const;
	void setLeader(const Monster *leader);

	[[nodiscard]] bool hasLeashedMinions() const
	{
		return isUnique() && UniqueMonstersData[static_cast<size_t>(uniqueType)].monsterPack == UniqueMonsterPack::Leashed;
	}

	/**
	 * @brief Calculates the distance in tiles between this monster and its current target
	 *
	 * The distance is not calculated as the euclidean distance, but rather as
	 * the longest number of tiles in the coordinate system.
	 *
	 * @return The distance in tiles
	 */
	[[nodiscard]] unsigned distanceToEnemy() const;

	/**
	 * @brief Is the monster currently walking?
	 */
	bool isWalking() const;
	bool isImmune(missile_id mitype) const;
	bool isResistant(missile_id mitype) const;

	/**
	 * Is this a player's golem?
	 */
	[[nodiscard]] bool isPlayerMinion() const;

	bool isPossibleToHit() const;
	void tag(const Player &tagger);

	[[nodiscard]] bool isUnique() const
	{
		return uniqueType != UniqueMonsterType::None;
	}

	bool tryLiftGargoyle();
};

extern size_t LevelMonsterTypeCount;
extern Monster Monsters[MaxMonsters];
extern int ActiveMonsters[MaxMonsters];
extern size_t ActiveMonsterCount;
extern int MonsterKillCounts[MaxMonsters];
extern bool sgbSaveSoundOn;

void PrepareUniqueMonst(Monster &monster, UniqueMonsterType monsterType, size_t miniontype, int bosspacksize, const UniqueMonsterData &uniqueMonsterData);
void InitLevelMonsters();
void GetLevelMTypes();
void InitMonsterSND(CMonster &monsterType);
void InitMonsterGFX(CMonster &monsterType);
void WeakenNaKrul();
void InitGolems();
void InitMonsters();
void SetMapMonsters(const uint16_t *dunData, Point startPosition);
Monster *AddMonster(Point position, Direction dir, size_t mtype, bool inMap);
void AddDoppelganger(Monster &monster);
bool M_Talker(const Monster &monster);
void M_StartStand(Monster &monster, Direction md);
void M_ClearSquares(const Monster &monster);
void M_GetKnockback(Monster &monster);
void M_StartHit(Monster &monster, int dam);
void M_StartHit(Monster &monster, const Player &player, int dam);
void StartMonsterDeath(Monster &monster, const Player &player, bool sendmsg);
void MonsterDeath(Monster &monster, Direction md, bool sendmsg);
void M_StartKill(Monster &monster, const Player &player);
void M_SyncStartKill(Monster &monster, Point position, const Player &player);
void M_UpdateRelations(const Monster &monster);
void DoEnding();
void PrepDoEnding();
bool Walk(Monster &monster, Direction md);
void GolumAi(Monster &monster);
void DeleteMonsterList();
void ProcessMonsters();
void FreeMonsters();
bool DirOK(const Monster &monster, Direction mdir);
bool PosOkMissile(Point position);
bool LineClearMissile(Point startPoint, Point endPoint);
bool LineClear(const std::function<bool(Point)> &clear, Point startPoint, Point endPoint);
void SyncMonsterAnim(Monster &monster);
void M_FallenFear(Point position);
void PrintMonstHistory(int mt);
void PrintUniqueHistory();
void PlayEffect(Monster &monster, MonsterSound mode);
void MissToMonst(Missile &missile, Point position);

Monster *MonsterAtPosition(Point position);

/**
 * @brief Check that the given tile is available to the monster
 */
bool IsTileAvailable(const Monster &monster, Point position);
bool IsSkel(_monster_id mt);
bool IsGoat(_monster_id mt);
/**
 * @brief Reveals a monster that was hiding in a container
 * @param monster instance returned from a previous call to PreSpawnSkeleton
 * @param position tile to try spawn the monster at, neighboring tiles will be used as a fallback
 */
void ActivateSkeleton(Monster &monster, Point position);
Monster *PreSpawnSkeleton();
void TalktoMonster(Monster &monster);
void SpawnGolem(Player &player, Monster &golem, Point position, Missile &missile);
bool CanTalkToMonst(const Monster &monster);
int encode_enemy(Monster &monster);
void decode_enemy(Monster &monster, int enemyId);

} // namespace devilution
