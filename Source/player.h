/**
 * @file player.h
 *
 * Interface of player functionality, leveling, actions, creation, loading, etc.
 */
#pragma once

#include <cstdint>
#include <vector>

#include <algorithm>
#include <array>

#include "diablo.h"
#include "engine.h"
#include "engine/actor_position.hpp"
#include "engine/animationinfo.h"
#include "engine/clx_sprite.hpp"
#include "engine/path.h"
#include "engine/point.hpp"
#include "interfac.h"
#include "items.h"
#include "levels/gendung.h"
#include "multi.h"
#include "spelldat.h"
#include "utils/attributes.h"
#include "utils/enum_traits.h"
#include "utils/stdcompat/algorithm.hpp"

namespace devilution {

constexpr int InventoryGridCells = 40;
constexpr int MaxBeltItems = 8;
constexpr int MaxResistance = 75;
constexpr int MaxCharacterLevel = 50;
constexpr int MaxSpellLevel = 15;
constexpr int PlayerNameLength = 32;

constexpr size_t NumHotkeys = 12;
constexpr int BaseHitChance = 50;

/** Walking directions */
enum {
	// clang-format off
	WALK_NE   =  1,
	WALK_NW   =  2,
	WALK_SE   =  3,
	WALK_SW   =  4,
	WALK_N    =  5,
	WALK_E    =  6,
	WALK_S    =  7,
	WALK_W    =  8,
	WALK_NONE = -1,
	// clang-format on
};

enum class HeroClass : uint8_t {
	Warrior,
	Rogue,
	Sorcerer,
	Monk,
	Bard,
	Barbarian,

	LAST = Barbarian
};

enum class CharacterAttribute : uint8_t {
	Strength,
	Magic,
	Dexterity,
	Vitality,

	FIRST = Strength,
	LAST = Vitality
};

// Logical equipment locations
enum inv_body_loc : uint8_t {
	INVLOC_HEAD,
	INVLOC_RING_LEFT,
	INVLOC_RING_RIGHT,
	INVLOC_AMULET,
	INVLOC_HAND_LEFT,
	INVLOC_HAND_RIGHT,
	INVLOC_CHEST,
	NUM_INVLOC,
};

enum class player_graphic : uint8_t {
	Stand,
	Walk,
	Attack,
	Hit,
	Lightning,
	Fire,
	Magic,
	Death,
	Block,

	LAST = Block
};

enum class PlayerWeaponGraphic : uint8_t {
	Unarmed,
	UnarmedShield,
	Sword,
	SwordShield,
	Bow,
	Axe,
	Mace,
	MaceShield,
	Staff,
};

enum PLR_MODE : uint8_t {
	PM_STAND,
	PM_WALK_NORTHWARDS,
	PM_WALK_SOUTHWARDS,
	PM_WALK_SIDEWAYS,
	PM_ATTACK,
	PM_RATTACK,
	PM_BLOCK,
	PM_GOTHIT,
	PM_DEATH,
	PM_SPELL,
	PM_NEWLVL,
	PM_QUIT,
};

enum action_id : int8_t {
	// clang-format off
	ACTION_WALK        = -2, // Automatic walk when using gamepad
	ACTION_NONE        = -1,
	ACTION_ATTACK      = 9,
	ACTION_RATTACK     = 10,
	ACTION_SPELL       = 12,
	ACTION_OPERATE     = 13,
	ACTION_DISARM      = 14,
	ACTION_PICKUPITEM  = 15, // put item in hand (inventory screen open)
	ACTION_PICKUPAITEM = 16, // put item in inventory
	ACTION_TALK        = 17,
	ACTION_OPERATETK   = 18, // operate via telekinesis
	ACTION_ATTACKMON   = 20,
	ACTION_ATTACKPLR   = 21,
	ACTION_RATTACKMON  = 22,
	ACTION_RATTACKPLR  = 23,
	ACTION_SPELLMON    = 24,
	ACTION_SPELLPLR    = 25,
	ACTION_SPELLWALL   = 26,
	// clang-format on
};

enum class SpellFlag : uint8_t {
	// clang-format off
	None         = 0,
	Etherealize  = 1 << 0,
	RageActive   = 1 << 1,
	RageCooldown = 1 << 2,
	// bits 3-7 are unused
	// clang-format on
};
use_enum_as_flags(SpellFlag);

/** Maps from armor animation to letter used in graphic files. */
constexpr std::array<char, 4> ArmourChar = {
	'L', // light
	'M', // medium
	'H', // heavy
};
/** Maps from weapon animation to letter used in graphic files. */
constexpr std::array<char, 9> WepChar = {
	'N', // unarmed
	'U', // no weapon + shield
	'S', // sword + no shield
	'D', // sword + shield
	'B', // bow
	'A', // axe
	'M', // blunt + no shield
	'H', // blunt + shield
	'T', // staff
};

/** Maps from player class to letter used in graphic files. */
constexpr std::array<char, 6> CharChar = {
	'W', // warrior
	'R', // rogue
	'S', // sorcerer
	'M', // monk
	'B',
	'C',
};

/**
 * @brief Contains Data (CelSprites) for a player graphic (player_graphic)
 */
struct PlayerAnimationData {
	/**
	 * @brief Sprite lists for each of the 8 directions.
	 */
	OptionalOwnedClxSpriteSheet sprites;

	[[nodiscard]] ClxSpriteList spritesForDirection(Direction direction) const
	{
		return (*sprites)[static_cast<size_t>(direction)];
	}
};

struct SpellCastInfo {
	spell_id spellId;
	spell_type spellType;
	/* @brief Inventory location for scrolls */
	int8_t spellFrom;
	/* @brief Used for spell level */
	int spellLevel;
};

struct Player {
	Player() = default;
	Player(Player &&) noexcept = default;
	Player &operator=(Player &&) noexcept = default;

	PLR_MODE _pmode;
	int8_t walkpath[MaxPathLength];
	bool plractive;
	action_id destAction;
	int destParam1;
	int destParam2;
	int destParam3;
	int destParam4;
	uint8_t plrlevel;
	bool plrIsOnSetLevel;
	ActorPosition position;
	Direction _pdir; // Direction faced by player (direction enum)
	int _pgfxnum;    // Bitmask indicating what variant of the sprite the player is using. Lower byte define weapon (PlayerWeaponGraphic) and higher values define armour (starting with PlayerArmorGraphic)
	/**
	 * @brief Contains Information for current Animation
	 */
	AnimationInfo AnimInfo;
	/**
	 * @brief Contains a optional preview ClxSprite that is displayed until the current command is handled by the game logic
	 */
	OptionalClxSprite previewCelSprite;
	/**
	 * @brief Contains the progress to next game tick when previewCelSprite was set
	 */
	float progressToNextGameTickWhenPreviewWasSet;
	int _plid;
	int _pvid;
	/* @brief next queued spell */
	SpellCastInfo queuedSpell;
	/* @brief the spell that is currently casted */
	SpellCastInfo executedSpell;
	spell_id _pTSpell;
	spell_id _pRSpell;
	spell_type _pRSplType;
	spell_id _pSBkSpell;
	int8_t _pSplLvl[64];
	uint64_t _pMemSpells;  // Bitmask of learned spells
	uint64_t _pAblSpells;  // Bitmask of abilities
	uint64_t _pScrlSpells; // Bitmask of spells available via scrolls
	SpellFlag _pSpellFlags;
	spell_id _pSplHotKey[NumHotkeys];
	spell_type _pSplTHotKey[NumHotkeys];
	bool _pBlockFlag;
	bool _pInvincible;
	int8_t _pLightRad;
	bool _pLvlChanging; // True when the player is transitioning between levels
	char _pName[PlayerNameLength];
	HeroClass _pClass;
	int _pStrength;
	int _pBaseStr;
	int _pMagic;
	int _pBaseMag;
	int _pDexterity;
	int _pBaseDex;
	int _pVitality;
	int _pBaseVit;
	int _pStatPts;
	int _pDamageMod;
	int _pBaseToBlk;
	int _pHPBase;
	int _pMaxHPBase;
	int _pHitPoints;
	int _pMaxHP;
	int _pHPPer;
	int _pManaBase;
	int _pMaxManaBase;
	int _pMana;
	int _pMaxMana;
	int _pManaPer;
	int8_t _pLevel;
	int8_t _pMaxLvl;
	uint32_t _pExperience;
	uint32_t _pNextExper;
	int8_t _pArmorClass;
	int8_t _pMagResist;
	int8_t _pFireResist;
	int8_t _pLghtResist;
	int _pGold;
	bool _pInfraFlag;
	/** Player's direction when ending movement. Also used for casting direction of SPL_FIREWALL. */
	Direction tempDirection;

	bool _pLvlVisited[NUMLEVELS];
	bool _pSLvlVisited[NUMLEVELS]; // only 10 used
	/**
	 * @brief Contains Data (Sprites) for the different Animations
	 */
	std::array<PlayerAnimationData, enum_size<player_graphic>::value> AnimationData;
	int8_t _pNFrames;
	int8_t _pWFrames;
	int8_t _pAFrames;
	int8_t _pAFNum;
	int8_t _pSFrames;
	int8_t _pSFNum;
	int8_t _pHFrames;
	int8_t _pDFrames;
	int8_t _pBFrames;
	Item InvBody[NUM_INVLOC];
	Item InvList[InventoryGridCells];
	int _pNumInv;
	int8_t InvGrid[InventoryGridCells];
	Item SpdList[MaxBeltItems];
	Item HoldItem;
	int _pIMinDam;
	int _pIMaxDam;
	int _pIAC;
	int _pIBonusDam;
	int _pIBonusToHit;
	int _pIBonusAC;
	int _pIBonusDamMod;
	/** Bitmask of staff spell */
	uint64_t _pISpells;
	/** Bitmask using item_special_effect */
	ItemSpecialEffect _pIFlags;
	int _pIGetHit;
	int8_t _pISplLvlAdd;
	int _pISplDur;
	int _pIEnAc;
	int _pIFMinDam;
	int _pIFMaxDam;
	int _pILMinDam;
	int _pILMaxDam;
	item_misc_id _pOilType;
	uint8_t pTownWarps;
	uint8_t pDungMsgs;
	uint8_t pLvlLoad;
	bool pBattleNet;
	bool pManaShield;
	uint8_t pDungMsgs2;
	bool pOriginalCathedral;
	uint16_t wReflections;
	uint8_t pDiabloKillLevel;
	_difficulty pDifficulty;
	ItemSpecialEffectHf pDamAcFlags;
	/** @brief Specifies whether players are in non-PvP mode. */
	bool friendlyMode = true;

	void CalcScrolls();

	bool CanUseItem(const Item &item) const
	{
		return _pStrength >= item._iMinStr
		    && _pMagic >= item._iMinMag
		    && _pDexterity >= item._iMinDex;
	}

	/**
	 * @brief Remove an item from player inventory
	 * @param iv invList index of item to be removed
	 * @param calcScrolls If true, CalcScrolls() gets called after removing item
	 */
	void RemoveInvItem(int iv, bool calcScrolls = true);

	/**
	 * @brief Returns the network identifier for this player
	 */
	[[nodiscard]] size_t getId() const;

	void RemoveSpdBarItem(int iv);

	/**
	 * @brief Gets the most valuable item out of all the player's items that match the given predicate.
	 * @param itemPredicate The predicate used to match the items.
	 * @return The most valuable item out of all the player's items that match the given predicate, or 'nullptr' in case no
	 * matching items were found.
	 */
	template <typename TPredicate>
	const Item *GetMostValuableItem(const TPredicate &itemPredicate) const
	{
		const auto getMostValuableItem = [&itemPredicate](const Item *begin, const Item *end, const Item *mostValuableItem = nullptr) {
			for (const auto *item = begin; item < end; item++) {
				if (item->isEmpty() || !itemPredicate(*item)) {
					continue;
				}

				if (mostValuableItem == nullptr || item->_iIvalue > mostValuableItem->_iIvalue) {
					mostValuableItem = item;
				}
			}

			return mostValuableItem;
		};

		const Item *mostValuableItem = getMostValuableItem(SpdList, SpdList + MaxBeltItems);
		mostValuableItem = getMostValuableItem(InvBody, InvBody + inv_body_loc::NUM_INVLOC, mostValuableItem);
		mostValuableItem = getMostValuableItem(InvList, InvList + _pNumInv, mostValuableItem);

		return mostValuableItem;
	}

	/**
	 * @brief Gets the base value of the player's specified attribute.
	 * @param attribute The attribute to retrieve the base value for
	 * @return The base value for the requested attribute.
	 */
	int GetBaseAttributeValue(CharacterAttribute attribute) const;

	/**
	 * @brief Gets the current value of the player's specified attribute.
	 * @param attribute The attribute to retrieve the current value for
	 * @return The current value for the requested attribute.
	 */
	int GetCurrentAttributeValue(CharacterAttribute attribute) const;

	/**
	 * @brief Gets the maximum value of the player's specified attribute.
	 * @param attribute The attribute to retrieve the maximum value for
	 * @return The maximum value for the requested attribute.
	 */
	int GetMaximumAttributeValue(CharacterAttribute attribute) const;

	/**
	 * @brief Get the tile coordinates a player is moving to (if not moving, then it corresponds to current position).
	 */
	Point GetTargetPosition() const;

	/**
	 * @brief Says a speech line.
	 * @todo BUGFIX Prevent more than one speech to be played at a time (reject new requests).
	 */
	void Say(HeroSpeech speechId) const;
	/**
	 * @brief Says a speech line after a given delay.
	 * @param speechId The speech ID to say.
	 * @param delay Multiple of 50ms wait before starting the speech
	 */
	void Say(HeroSpeech speechId, int delay) const;
	/**
	 * @brief Says a speech line, without random variants.
	 */
	void SaySpecific(HeroSpeech speechId) const;

	/**
	 * @brief Attempts to stop the player from performing any queued up action. If the player is currently walking, his walking will
	 * stop as soon as he reaches the next tile. If any action was queued with the previous command (like targeting a monster,
	 * opening a chest, picking an item up, etc) this action will also be cancelled.
	 */
	void Stop();

	/**
	 * @brief Is the player currently walking?
	 */
	bool IsWalking() const;

	/**
	 * @brief Returns item location taking into consideration barbarian's ability to hold two-handed maces and clubs in one hand.
	 */
	item_equip_type GetItemLocation(const Item &item) const
	{
		if (_pClass == HeroClass::Barbarian && item._iLoc == ILOC_TWOHAND && IsAnyOf(item._itype, ItemType::Sword, ItemType::Mace))
			return ILOC_ONEHAND;
		return item._iLoc;
	}

	/**
	 * @brief Return player's armor value
	 */
	int GetArmor() const
	{
		return _pIBonusAC + _pIAC + _pDexterity / 5;
	}

	/**
	 * @brief Return player's melee to hit value
	 */
	int GetMeleeToHit() const
	{
		int hper = _pLevel + _pDexterity / 2 + _pIBonusToHit + BaseHitChance;
		if (_pClass == HeroClass::Warrior)
			hper += 20;
		return hper;
	}

	/**
	 * @brief Return player's melee to hit value, including armor piercing
	 */
	int GetMeleePiercingToHit() const
	{
		int hper = GetMeleeToHit();
		// in hellfire armor piercing ignores % of enemy armor instead, no way to include it here
		if (!gbIsHellfire)
			hper += _pIEnAc;
		return hper;
	}

	/**
	 * @brief Return player's ranged to hit value
	 */
	int GetRangedToHit() const
	{
		int hper = _pLevel + _pDexterity + _pIBonusToHit + BaseHitChance;
		if (_pClass == HeroClass::Rogue)
			hper += 20;
		else if (_pClass == HeroClass::Warrior || _pClass == HeroClass::Bard)
			hper += 10;
		return hper;
	}

	int GetRangedPiercingToHit() const
	{
		int hper = GetRangedToHit();
		// in hellfire armor piercing ignores % of enemy armor instead, no way to include it here
		if (!gbIsHellfire)
			hper += _pIEnAc;
		return hper;
	}

	/**
	 * @brief Return magic hit chance
	 */
	int GetMagicToHit() const
	{
		int hper = _pMagic + BaseHitChance;
		if (_pClass == HeroClass::Sorcerer)
			hper += 20;
		else if (_pClass == HeroClass::Bard)
			hper += 10;
		return hper;
	}

	/**
	 * @brief Return block chance
	 * @param useLevel - indicate if player's level should be added to block chance (the only case where it isn't is blocking a trap)
	 */
	int GetBlockChance(bool useLevel = true) const
	{
		int blkper = _pDexterity + _pBaseToBlk;
		if (useLevel)
			blkper += _pLevel * 2;
		return blkper;
	}

	/**
	 * @brief Return reciprocal of the factor for calculating damage reduction due to Mana Shield.
	 *
	 * Valid only for players with Mana Shield spell level greater than zero.
	 */
	int GetManaShieldDamageReduction();

	/**
	 * @brief Gets the effective spell level for the player, considering item bonuses
	 * @param spell spell_id enum member identifying the spell
	 * @return effective spell level
	 */
	int GetSpellLevel(spell_id spell) const
	{
		if (spell == SPL_INVALID || static_cast<std::size_t>(spell) >= sizeof(_pSplLvl)) {
			return 0;
		}

		return std::max<int8_t>(_pISplLvlAdd + _pSplLvl[static_cast<std::size_t>(spell)], 0);
	}

	/**
	 * @brief Return monster armor value after including player's armor piercing % (hellfire only)
	 * @param monsterArmor - monster armor before applying % armor pierce
	 * @param isMelee - indicates if it's melee or ranged combat
	 */
	int CalculateArmorPierce(int monsterArmor, bool isMelee) const
	{
		int tmac = monsterArmor;
		if (_pIEnAc > 0) {
			if (gbIsHellfire) {
				int pIEnAc = _pIEnAc - 1;
				if (pIEnAc > 0)
					tmac >>= pIEnAc;
				else
					tmac -= tmac / 4;
			}
			if (isMelee && _pClass == HeroClass::Barbarian) {
				tmac -= monsterArmor / 8;
			}
		}
		if (tmac < 0)
			tmac = 0;

		return tmac;
	}

	/**
	 * @brief Calculates the players current Hit Points as a percentage of their max HP and stores it for later reference
	 *
	 * The stored value is unused...
	 * @see _pHPPer
	 * @return The players current hit points as a percentage of their maximum (from 0 to 80%)
	 */
	int UpdateHitPointPercentage()
	{
		if (_pMaxHP <= 0) { // divide by zero guard
			_pHPPer = 0;
		} else {
			// Maximum achievable HP is approximately 1200. Diablo uses fixed point integers where the last 6 bits are
			// fractional values. This means that we will never overflow HP values normally by doing this multiplication
			// as the max value is representable in 17 bits and the multiplication result will be at most 23 bits
			_pHPPer = clamp(_pHitPoints * 80 / _pMaxHP, 0, 80); // hp should never be greater than maxHP but just in case
		}

		return _pHPPer;
	}

	int UpdateManaPercentage()
	{
		if (_pMaxMana <= 0) {
			_pManaPer = 0;
		} else {
			_pManaPer = clamp(_pMana * 80 / _pMaxMana, 0, 80);
		}

		return _pManaPer;
	}

	/**
	 * @brief Restores between 1/8 (inclusive) and 1/4 (exclusive) of the players max HP (further adjusted by class).
	 *
	 * This determines a random amount of non-fractional life points to restore then scales the value based on the
	 *  player class. Warriors/barbarians get between 1/4 and 1/2 life restored per potion, rogue/monk/bard get 3/16
	 *  to 3/8, and sorcerers get the base amount.
	 */
	void RestorePartialLife();

	/**
	 * @brief Resets hp to maxHp
	 */
	void RestoreFullLife()
	{
		_pHitPoints = _pMaxHP;
		_pHPBase = _pMaxHPBase;
	}

	/**
	 * @brief Restores between 1/8 (inclusive) and 1/4 (exclusive) of the players max Mana (further adjusted by class).
	 *
	 * This determines a random amount of non-fractional mana points to restore then scales the value based on the
	 *  player class. Sorcerers get between 1/4 and 1/2 mana restored per potion, rogue/monk/bard get 3/16 to 3/8,
	 *  and warrior/barbarian get the base amount. However if the player can't use magic due to an equipped item then
	 *  they get nothing.
	 */
	void RestorePartialMana();

	/**
	 * @brief Resets mana to maxMana (if the player can use magic)
	 */
	void RestoreFullMana()
	{
		if (HasNoneOf(_pIFlags, ItemSpecialEffect::NoMana)) {
			_pMana = _pMaxMana;
			_pManaBase = _pMaxManaBase;
		}
	}
	/**
	 * @brief Sets the readied spell to the spell in the specified equipment slot. Does nothing if the item does not have a valid spell.
	 * @param bodyLocation - the body location whose item will be checked for the spell.
	 */
	void ReadySpellFromEquipment(inv_body_loc bodyLocation);

	/**
	 * @brief Does the player currently have a ranged weapon equipped?
	 */
	bool UsesRangedWeapon() const
	{
		return static_cast<PlayerWeaponGraphic>(_pgfxnum & 0xF) == PlayerWeaponGraphic::Bow;
	}

	bool CanChangeAction()
	{
		if (_pmode == PM_STAND)
			return true;
		if (_pmode == PM_ATTACK && AnimInfo.currentFrame >= _pAFNum)
			return true;
		if (_pmode == PM_RATTACK && AnimInfo.currentFrame >= _pAFNum)
			return true;
		if (_pmode == PM_SPELL && AnimInfo.currentFrame >= _pSFNum)
			return true;
		if (IsWalking() && AnimInfo.isLastFrame())
			return true;
		return false;
	}

	[[nodiscard]] player_graphic getGraphic() const;

	[[nodiscard]] uint16_t getSpriteWidth() const;

	void getAnimationFramesAndTicksPerFrame(player_graphic graphics, int8_t &numberOfFrames, int8_t &ticksPerFrame) const;

	/**
	 * @brief Updates previewCelSprite according to new requested command
	 * @param cmdId What command is requested
	 * @param point Point for the command
	 * @param wParam1 First Parameter
	 * @param wParam2 Second Parameter
	 */
	void UpdatePreviewCelSprite(_cmd_id cmdId, Point point, uint16_t wParam1, uint16_t wParam2);

	/** @brief Checks if the player is on the same level as the local player (MyPlayer). */
	bool isOnActiveLevel() const
	{
		if (setlevel)
			return isOnLevel(setlvlnum);
		return isOnLevel(currlevel);
	}

	/** @brief Checks if the player is on the corresponding level. */
	bool isOnLevel(uint8_t level) const
	{
		return !this->plrIsOnSetLevel && this->plrlevel == level;
	}
	/** @brief Checks if the player is on the corresponding level. */
	bool isOnLevel(_setlevels level) const
	{
		return this->plrIsOnSetLevel && this->plrlevel == static_cast<uint8_t>(level);
	}
	void setLevel(uint8_t level)
	{
		this->plrlevel = level;
		this->plrIsOnSetLevel = false;
	}
	void setLevel(_setlevels level)
	{
		this->plrlevel = static_cast<uint8_t>(level);
		this->plrIsOnSetLevel = true;
	}
};

extern DVL_API_FOR_TEST size_t MyPlayerId;
extern DVL_API_FOR_TEST Player *MyPlayer;
extern DVL_API_FOR_TEST std::vector<Player> Players;
extern bool MyPlayerIsDead;
extern const int BlockBonuses[enum_size<HeroClass>::value];

Player *PlayerAtPosition(Point position);

void LoadPlrGFX(Player &player, player_graphic graphic);
void InitPlayerGFX(Player &player);
void ResetPlayerGFX(Player &player);

/**
 * @brief Sets the new Player Animation with all relevant information for rendering
 * @param player The player to set the animation for
 * @param graphic What player animation should be displayed
 * @param dir Direction of the animation
 * @param numberOfFrames Number of Frames in Animation
 * @param delayLen Delay after each Animation sequence
 * @param flags Specifies what special logics are applied to this Animation
 * @param numSkippedFrames Number of Frames that will be skipped (for example with modifier "faster attack")
 * @param distributeFramesBeforeFrame Distribute the numSkippedFrames only before this frame
 */
void NewPlrAnim(Player &player, player_graphic graphic, Direction dir, AnimationDistributionFlags flags = AnimationDistributionFlags::None, int8_t numSkippedFrames = 0, int8_t distributeFramesBeforeFrame = 0);
void SetPlrAnims(Player &player);
void CreatePlayer(Player &player, HeroClass c);
int CalcStatDiff(Player &player);
#ifdef _DEBUG
void NextPlrLevel(Player &player);
#endif
void AddPlrExperience(Player &player, int lvl, int exp);
void AddPlrMonstExper(int lvl, int exp, char pmask);
void ApplyPlrDamage(Player &player, int dam, int minHP = 0, int frac = 0, int earflag = 0);
void InitPlayer(Player &player, bool FirstTime);
void InitMultiView();
void PlrClrTrans(Point position);
void PlrDoTrans(Point position);
void SetPlayerOld(Player &player);
void FixPlayerLocation(Player &player, Direction bDir);
void StartStand(Player &player, Direction dir);
void StartPlrBlock(Player &player, Direction dir);
void FixPlrWalkTags(const Player &player);
void StartPlrHit(Player &player, int dam, bool forcehit);
void StartPlayerKill(Player &player, int earflag);
/**
 * @brief Strip the top off gold piles that are larger than MaxGold
 */
void StripTopGold(Player &player);
void SyncPlrKill(Player &player, int earflag);
void RemovePlrMissiles(const Player &player);
void StartNewLvl(Player &player, interface_mode fom, int lvl);
void RestartTownLvl(Player &player);
void StartWarpLvl(Player &player, size_t pidx);
void ProcessPlayers();
void ClrPlrPath(Player &player);
bool PosOkPlayer(const Player &player, Point position);
void MakePlrPath(Player &player, Point targetPosition, bool endspace);
void CalcPlrStaff(Player &player);
void CheckPlrSpell(bool isShiftHeld, spell_id spellID = MyPlayer->_pRSpell, spell_type spellType = MyPlayer->_pRSplType);
void SyncPlrAnim(Player &player);
void SyncInitPlrPos(Player &player);
void SyncInitPlr(Player &player);
void CheckStats(Player &player);
void ModifyPlrStr(Player &player, int l);
void ModifyPlrMag(Player &player, int l);
void ModifyPlrDex(Player &player, int l);
void ModifyPlrVit(Player &player, int l);
void SetPlayerHitPoints(Player &player, int val);
void SetPlrStr(Player &player, int v);
void SetPlrMag(Player &player, int v);
void SetPlrDex(Player &player, int v);
void SetPlrVit(Player &player, int v);
void InitDungMsgs(Player &player);
void PlayDungMsgs();

/* data */

extern const int8_t plrxoff[9];
extern const int8_t plryoff[9];
extern const int8_t plrxoff2[9];
extern const int8_t plryoff2[9];
extern const int StrengthTbl[enum_size<HeroClass>::value];
extern const int MagicTbl[enum_size<HeroClass>::value];
extern const int DexterityTbl[enum_size<HeroClass>::value];
extern const int VitalityTbl[enum_size<HeroClass>::value];
extern const uint32_t ExpLvlsTbl[MaxCharacterLevel + 1];

} // namespace devilution
