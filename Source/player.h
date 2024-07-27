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
#include "playerdat.hpp"
#include "spelldat.h"
#include "utils/attributes.h"
#include "utils/enum_traits.h"

namespace devilution {

constexpr int InventoryGridCells = 40;
constexpr int MaxBeltItems = 8;
constexpr int MaxResistance = 75;
constexpr uint8_t MaxSpellLevel = 15;
constexpr int PlayerNameLength = 32;

constexpr size_t NumHotkeys = 12;

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

/* @brief When the player dies, what is the reason/source why? */
enum class DeathReason {
	/* @brief Monster or Trap (dungeon) */
	MonsterOrTrap,
	/* @brief Other player or selfkill (for example firewall) */
	Player,
	/* @brief HP is zero but we don't know when or where this happend */
	Unknown,
};

/** Maps from armor animation to letter used in graphic files. */
constexpr std::array<char, 3> ArmourChar = {
	'l', // light
	'm', // medium
	'h', // heavy
};
/** Maps from weapon animation to letter used in graphic files. */
constexpr std::array<char, 9> WepChar = {
	'n', // unarmed
	'u', // no weapon + shield
	's', // sword + no shield
	'd', // sword + shield
	'b', // bow
	'a', // axe
	'm', // blunt + no shield
	'h', // blunt + shield
	't', // staff
};

/** Maps from player class to letter used in graphic files. */
constexpr std::array<char, 6> CharChar = {
	'w', // warrior
	'r', // rogue
	's', // sorcerer
	'm', // monk
	'b',
	'c',
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
	SpellID spellId;
	SpellType spellType;
	/**
	 * @brief Inventory location for scrolls
	 */
	int8_t spellFrom;
	/**
	 * @brief Used for spell level
	 */
	int spellLevel;
};

struct Player {
	Player() = default;
	Player(Player &&) noexcept = default;
	Player &operator=(Player &&) noexcept = default;

	char name[PlayerNameLength];            // _pName
	Item bodySlot[NUM_INVLOC];              // _pInvBody
	Item inventorySlot[InventoryGridCells]; // _pInvList
	Item beltSlot[MaxBeltItems];            // _pSpdList
	Item heldItem;                          // _pHoldItem

	int lightId;

	int numInventoryItems;          // _pNumInv
	int strength;                   // _pStrength
	int baseStrength;               // _pBaseStr
	int magic;                      // _pMagic
	int baseMagic;                  // _pBaseMag
	int dexterity;                  // _pDexterity
	int baseDexterity;              // _pBaseDex
	int vitality;                   // _pVitality
	int baseVitality;               // _pBaseVit
	int statPoints;                 // _pStatPts
	int damageModifier;             // _pDamageMod
	int baseLife;                   // _pHPBase
	int baseMaxLife;                // _pMaxHPBase
	int life;                       // _pHitPoints
	int maxLife;                    // _pMaxHP
	int lifePercentage;             // _pHPPer
	int baseMana;                   // _pManaBase
	int baseMaxMana;                // _pMaxManaBase
	int mana;                       // _pMana
	int maxMana;                    // _pMaxMana
	int manaPercentage;             // _pManaPer
	int minDamage;                  // _pIMinDam
	int maxDamage;                  // _pIMaxDam
	int armorClass;                 // _pIAC
	int bonusDamagePercent;         // _pIBonusDam
	int bonusToHit;                 // _pIBonusToHit
	int bonusArmorClass;            // _pIBonusAC
	int bonusDamage;                // _pIBonusDamMod
	int damageFromEnemies;          // _pIGetHit
	int armorPierce;                // _pIEnAc
	int minFireDamage;              // _pIFMinDam
	int maxFireDamage;              // _pIFMaxDam
	int minLightningDamage;         // _pILMinDam
	int maxLightningDamage;         // _pILMaxDam
	uint32_t experience;            // _pExperience
	PLR_MODE mode;                  // _pmode
	int8_t walkPath[MaxPathLength]; // _walkpath
	bool isPlayerActive;            // _pActive
	action_id destinationAction;    // _pDestAction
	int destinationParam1;          // _pDestParam1
	int destinationParam2;          // _pDestParam2
	int destinationParam3;          // _pDestParam3
	int destinationParam4;          // _pDestParam4
	int gold;                       // _pGold

	/**
	 * @brief Contains Information for current Animation (AnimInfo)
	 */
	PlayerAnimationInfo animationInfo;
	/**
	 * @brief Contains a optional preview ClxSprite that is displayed until the current command is handled by the game logic
	 */
	OptionalClxSprite previewCelSprite;
	/**
	 * @brief Contains the progress to next game tick when previewCelSprite was set
	 */
	int8_t progressToNextGameTickWhenPreviewWasSet;
	/**
	 * @brief Bitmask using item_special_effect (_pIFlags)
	 */
	ItemSpecialEffect flags;
	/**
	 * @brief Contains Data (Sprites) for the different Animations (AnimationData)
	 */
	std::array<PlayerAnimationData, enum_size<player_graphic>::value> animationData;
	int8_t numIdleFrames;                     // _pNFrames
	int8_t numWalkFrames;                     // _pWFrames
	int8_t numAttackFrames;                   // _pAFrames
	int8_t attackActionFrame;                 // _pAFNum
	int8_t numSpellFrames;                    // _pSFrames
	int8_t spellActionFrame;                  // _pSFNum
	int8_t numRecoveryFrames;                 // _pRFrames
	int8_t numDeathFrames;                    // _pDFrames
	int8_t numBlockFrames;                    // _pBFrames
	int8_t inventoryGrid[InventoryGridCells]; // InvGrid

	uint8_t dungeonLevel; // plrlevel
	bool isOnSetLevel;    // plrIsOnSetLvl
	ActorPosition position;
	Direction direction; // Direction faced by player (direction enum) (_pdir)
	HeroClass heroClass; // _pClass

private:
	uint8_t characterLevel = 1; // Use get/setCharacterLevel to ensure this attribute stays within the accepted range (_pLevel)

public:
	uint8_t graphic;        // Bitmask indicating what variant of the sprite the player is using. The 3 lower bits define weapon (PlayerWeaponGraphic) and the higher bits define armour (starting with PlayerArmorGraphic) (_pgfxnum)
	int8_t bonusSpellLevel; // _pSplLvlAdd
	/**
	 * @brief Specifies whether players are in non-PvP mode. (friendlyMode)
	 */
	bool isFriendlyMode = true;

	/**
	 * @brief The next queued spell
	 */
	SpellCastInfo queuedSpell;
	/**
	 * @brief The spell that is currently being cast
	 */
	SpellCastInfo executedSpell;
	/**
	 * @brief Which spell should be executed with CURSOR_TELEPORT
	 */
	SpellID inventorySpell;
	/**
	 * @brief Inventory location for scrolls with CURSOR_TELEPORT
	 */
	int8_t spellFrom;
	SpellID selectedSpell;       // _pRSpell
	SpellType selectedSpellType; // _pRSplType
	uint8_t spellLevel[64];      // _pSplLvl
	/**
	 * @brief Bitmask of staff spell (_pISpells)
	 */
	uint64_t staffSpells;
	/**
	 * @brief Bitmask of learned spells (_pMemSpells)
	 */
	uint64_t learnedSpells;
	/**
	 * @brief Bitmask of skills (_pAblSpells)
	 */
	uint64_t skills;
	/**
	 * @brief Bitmask of spells available via scrolls (_pScrlSpells)
	 */
	uint64_t scrollSpells;
	SpellFlag spellFlags;                  // _pSplFlags
	SpellID hotkeySpell[NumHotkeys];       // _pSplHotKey
	SpellType hotkeySpellType[NumHotkeys]; // _pSplTHotKey
	bool hasBlockFlag;                     // _pBlockFlag
	bool isInvincible;                     // _pInvincible
	int8_t lightRadius;                    // _pLightRad
	/**
	 * @brief True when the player is transitioning between levels (_pLvlChanging)
	 */
	bool isChangingLevel;

	int8_t resistMagic;      // _pMagResist
	int8_t resistFire;       // _pFireResist
	int8_t resistLightning;  // _pLghtResist
	bool hasInfravisionFlag; // _pInfraFlag
	/**
	 * @brief Player's direction when ending movement. Also used for casting direction of SpellID::FireWall.
	 */
	Direction tempDirection;

	bool isLevelVisted[NUMLEVELS];    // _pLvlVisited
	bool isSetLevelVisted[NUMLEVELS]; // only 10 used (_pSLvlVisited)

	item_misc_id oilType;              // _pOilType
	uint8_t townWarps;                 // pTownWarps
	uint8_t dungeonMessages;           // pDungMsgs
	uint8_t levelLoading;              // pLvlLoad
	bool hasManaShield;                // pManaShield
	uint8_t dungeonMessages2;          // pDungMsgs2
	bool originalCathedral;            // pOriginalCathedral
	uint8_t difficultyCompletion;      // pDiabloKillLevel
	uint16_t reflections;              // wReflections
	ItemSpecialEffectHf hellfireFlags; // pDamAcFlags

	/**
	 * @brief Convenience function to get the base stats/bonuses for this player's class
	 */
	[[nodiscard]] const ClassAttributes &getClassAttributes() const
	{
		return GetClassAttributes(heroClass);
	}

	[[nodiscard]] const PlayerCombatData &getPlayerCombatData() const
	{
		return GetPlayerCombatDataForClass(heroClass);
	}

	[[nodiscard]] const PlayerData &getPlayerData() const
	{
		return GetPlayerDataForClass(heroClass);
	}

	/**
	 * @brief Gets the translated name for the character's class
	 */
	[[nodiscard]] std::string_view getClassName() const
	{
		return _(getPlayerData().className);
	}

	[[nodiscard]] int getBaseToBlock() const
	{
		return getPlayerCombatData().baseToBlock;
	}

	void CalcScrolls();

	bool CanUseItem(const Item &item) const
	{
		return strength >= item._iMinStr
		    && magic >= item._iMinMag
		    && dexterity >= item._iMinDex;
	}

	bool CanCleave()
	{
		switch (heroClass) {
		case HeroClass::Warrior:
		case HeroClass::Rogue:
		case HeroClass::Sorcerer:
			return false;
		case HeroClass::Monk:
			return isEquipped(ItemType::Staff);
		case HeroClass::Bard:
			return bodySlot[INVLOC_HAND_LEFT]._itype == ItemType::Sword && bodySlot[INVLOC_HAND_RIGHT]._itype == ItemType::Sword;
		case HeroClass::Barbarian:
			return isEquipped(ItemType::Axe) || (!isEquipped(ItemType::Shield) && (isEquipped(ItemType::Mace, true) || isEquipped(ItemType::Sword, true)));
		default:
			return false;
		}
	}

	bool isEquipped(ItemType itemType, bool isTwoHanded = false)
	{
		switch (itemType) {
		case ItemType::Sword:
		case ItemType::Axe:
		case ItemType::Bow:
		case ItemType::Mace:
		case ItemType::Shield:
		case ItemType::Staff:
			return (bodySlot[INVLOC_HAND_LEFT]._itype == itemType && (!isTwoHanded || bodySlot[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND))
			    || (bodySlot[INVLOC_HAND_RIGHT]._itype == itemType && (!isTwoHanded || bodySlot[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND));
		case ItemType::LightArmor:
		case ItemType::MediumArmor:
		case ItemType::HeavyArmor:
			return bodySlot[INVLOC_CHEST]._itype == itemType;
		case ItemType::Helm:
			return bodySlot[INVLOC_HEAD]._itype == itemType;
		case ItemType::Ring:
			return bodySlot[INVLOC_RING_LEFT]._itype == itemType || bodySlot[INVLOC_RING_RIGHT]._itype == itemType;
		case ItemType::Amulet:
			return bodySlot[INVLOC_AMULET]._itype == itemType;
		default:
			return false;
		}
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
	[[nodiscard]] uint8_t getId() const;

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

		const Item *mostValuableItem = getMostValuableItem(beltSlot, beltSlot + MaxBeltItems);
		mostValuableItem = getMostValuableItem(bodySlot, bodySlot + inv_body_loc::NUM_INVLOC, mostValuableItem);
		mostValuableItem = getMostValuableItem(inventorySlot, inventorySlot + numInventoryItems, mostValuableItem);

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
	 * @brief Check if position is in player's path.
	 */
	bool IsPositionInPath(Point position);

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
	bool isWalking() const;

	/**
	 * @brief Returns item location taking into consideration barbarian's ability to hold two-handed maces and clubs in one hand.
	 */
	item_equip_type GetItemLocation(const Item &item) const
	{
		if (heroClass == HeroClass::Barbarian && item._iLoc == ILOC_TWOHAND && IsAnyOf(item._itype, ItemType::Sword, ItemType::Mace))
			return ILOC_ONEHAND;
		return item._iLoc;
	}

	/**
	 * @brief Return player's armor value
	 */
	int GetArmor() const
	{
		return bonusArmorClass + armorClass + dexterity / 5;
	}

	/**
	 * @brief Return player's melee to hit value
	 */
	int GetMeleeToHit() const
	{
		return getCharacterLevel() + dexterity / 2 + bonusToHit + getPlayerCombatData().baseMeleeToHit;
	}

	/**
	 * @brief Return player's melee to hit value, including armor piercing
	 */
	int GetMeleePiercingToHit() const
	{
		int hper = GetMeleeToHit();
		// in hellfire armor piercing ignores % of enemy armor instead, no way to include it here
		if (!gbIsHellfire)
			hper += armorPierce;
		return hper;
	}

	/**
	 * @brief Return player's ranged to hit value
	 */
	int GetRangedToHit() const
	{
		return getCharacterLevel() + dexterity + bonusToHit + getPlayerCombatData().baseRangedToHit;
	}

	int GetRangedPiercingToHit() const
	{
		int hper = GetRangedToHit();
		// in hellfire armor piercing ignores % of enemy armor instead, no way to include it here
		if (!gbIsHellfire)
			hper += armorPierce;
		return hper;
	}

	/**
	 * @brief Return magic hit chance
	 */
	int GetMagicToHit() const
	{
		return magic + getPlayerCombatData().baseMagicToHit;
	}

	/**
	 * @brief Return block chance
	 * @param useLevel - indicate if player's level should be added to block chance (the only case where it isn't is blocking a trap)
	 */
	int GetBlockChance(bool useLevel = true) const
	{
		int blkper = dexterity + getBaseToBlock();
		if (useLevel)
			blkper += getCharacterLevel() * 2;
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
	 * @param spell SpellID enum member identifying the spell
	 * @return effective spell level
	 */
	int GetSpellLevel(SpellID spell) const
	{
		if (spell == SpellID::Invalid || static_cast<std::size_t>(spell) >= sizeof(spellLevel)) {
			return 0;
		}

		return std::max<int>(bonusSpellLevel + spellLevel[static_cast<std::size_t>(spell)], 0);
	}

	/**
	 * @brief Return monster armor value after including player's armor piercing % (hellfire only)
	 * @param monsterArmor - monster armor before applying % armor pierce
	 * @param isMelee - indicates if it's melee or ranged combat
	 */
	int CalculateArmorPierce(int monsterArmor, bool isMelee) const
	{
		int tmac = monsterArmor;
		if (armorPierce > 0) {
			if (gbIsHellfire) {
				int pIEnAc = armorPierce - 1;
				if (pIEnAc > 0)
					tmac >>= pIEnAc;
				else
					tmac -= tmac / 4;
			}
			if (isMelee && heroClass == HeroClass::Barbarian) {
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
	 * @see lifePercentage
	 * @return The players current hit points as a percentage of their maximum (from 0 to 80%)
	 */
	int UpdateHitPointPercentage()
	{
		if (maxLife <= 0) { // divide by zero guard
			lifePercentage = 0;
		} else {
			// Maximum achievable HP is approximately 1200. Diablo uses fixed point integers where the last 6 bits are
			// fractional values. This means that we will never overflow HP values normally by doing this multiplication
			// as the max value is representable in 17 bits and the multiplication result will be at most 23 bits
			lifePercentage = std::clamp(life * 80 / maxLife, 0, 80); // hp should never be greater than maxHP but just in case
		}

		return lifePercentage;
	}

	int UpdateManaPercentage()
	{
		if (maxMana <= 0) {
			manaPercentage = 0;
		} else {
			manaPercentage = std::clamp(mana * 80 / maxMana, 0, 80);
		}

		return manaPercentage;
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
		life = maxLife;
		baseLife = baseMaxLife;
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
		if (HasNoneOf(flags, ItemSpecialEffect::NoMana)) {
			mana = maxMana;
			baseMana = baseMaxMana;
		}
	}
	/**
	 * @brief Sets the readied spell to the spell in the specified equipment slot. Does nothing if the item does not have a valid spell.
	 * @param bodyLocation - the body location whose item will be checked for the spell.
	 * @param forceSpell - if true, always change active spell, if false, only when current spell slot is empty
	 */
	void ReadySpellFromEquipment(inv_body_loc bodyLocation, bool forceSpell);

	/**
	 * @brief Does the player currently have a ranged weapon equipped?
	 */
	bool UsesRangedWeapon() const
	{
		return static_cast<PlayerWeaponGraphic>(graphic & 0xF) == PlayerWeaponGraphic::Bow;
	}

	bool CanChangeAction()
	{
		if (mode == PM_STAND)
			return true;
		if (mode == PM_ATTACK && animationInfo.currentFrame >= attackActionFrame)
			return true;
		if (mode == PM_RATTACK && animationInfo.currentFrame >= attackActionFrame)
			return true;
		if (mode == PM_SPELL && animationInfo.currentFrame >= spellActionFrame)
			return true;
		if (isWalking() && animationInfo.isLastFrame())
			return true;
		return false;
	}

	[[nodiscard]] player_graphic getGraphic() const;

	[[nodiscard]] uint16_t getSpriteWidth() const;

	void getAnimationFramesAndTicksPerFrame(player_graphic graphics, int8_t &numberOfFrames, int8_t &ticksPerFrame) const;

	[[nodiscard]] ClxSprite currentSprite() const
	{
		return previewCelSprite ? *previewCelSprite : animationInfo.currentSprite();
	}
	[[nodiscard]] Displacement getRenderingOffset(const ClxSprite sprite) const
	{
		Displacement offset = { -CalculateWidth2(sprite.width()), 0 };
		if (isWalking())
			offset += GetOffsetForWalking(animationInfo, direction);
		return offset;
	}

	/**
	 * @brief Updates previewCelSprite according to new requested command
	 * @param cmdId What command is requested
	 * @param point Point for the command
	 * @param wParam1 First Parameter
	 * @param wParam2 Second Parameter
	 */
	void UpdatePreviewCelSprite(_cmd_id cmdId, Point point, uint16_t wParam1, uint16_t wParam2);

	[[nodiscard]] uint8_t getCharacterLevel() const
	{
		return characterLevel;
	}

	/**
	 * @brief Sets the character level to the target level or nearest valid value.
	 * @param level New character level, will be clamped to the allowed range
	 */
	void setCharacterLevel(uint8_t level);

	[[nodiscard]] uint8_t getMaxCharacterLevel() const;

	[[nodiscard]] bool isMaxCharacterLevel() const
	{
		return getCharacterLevel() >= getMaxCharacterLevel();
	}

private:
	void _addExperience(uint32_t experience, int levelDelta);

public:
	/**
	 * @brief Adds experience to the local player based on the current game mode
	 * @param experience base value to add, this will be adjusted to prevent power leveling in multiplayer games
	 */
	void addExperience(uint32_t experience)
	{
		_addExperience(experience, 0);
	}

	/**
	 * @brief Adds experience to the local player based on the difference between the monster level
	 * and current level, then also applying the power level cap in multiplayer games.
	 * @param experience base value to add, will be scaled up/down by the difference between player and monster level
	 * @param monsterLevel level of the monster that has rewarded this experience
	 */
	void addExperience(uint32_t experience, int monsterLevel)
	{
		_addExperience(experience, monsterLevel - getCharacterLevel());
	}

	[[nodiscard]] uint32_t getNextExperienceThreshold() const;

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
		return !this->isOnSetLevel && this->dungeonLevel == level;
	}
	/** @brief Checks if the player is on the corresponding level. */
	bool isOnLevel(_setlevels level) const
	{
		return this->isOnSetLevel && this->dungeonLevel == static_cast<uint8_t>(level);
	}
	/** @brief Checks if the player is on a arena level. */
	bool isOnArenaLevel() const
	{
		return isOnSetLevel && IsArenaLevel(static_cast<_setlevels>(dungeonLevel));
	}
	void setLevel(uint8_t level)
	{
		this->dungeonLevel = level;
		this->isOnSetLevel = false;
	}
	void setLevel(_setlevels level)
	{
		this->dungeonLevel = static_cast<uint8_t>(level);
		this->isOnSetLevel = true;
	}

	/** @brief Returns a character's life based on starting life, character level, and base vitality. */
	int32_t calculateBaseLife() const;

	/** @brief Returns a character's mana based on starting mana, character level, and base magic. */
	int32_t calculateBaseMana() const;

	/**
	 * @brief Sets a tile/dPlayer to be occupied by the player
	 * @param position tile to update
	 * @param isMoving specifies whether the player is moving or not (true/moving results in a negative index in dPlayer)
	 */
	void occupyTile(Point position, bool isMoving) const;

	/** @brief Checks if the player level is owned by local client. */
	bool isLevelOwnedByLocalClient() const;

	/** @brief Checks if the player is holding an item of the provided type, and is usable. */
	bool isHoldingItem(const ItemType type) const
	{
		const Item &leftHandItem = bodySlot[INVLOC_HAND_LEFT];
		const Item &rightHandItem = bodySlot[INVLOC_HAND_RIGHT];

		return (type == leftHandItem._itype && leftHandItem._iStatFlag) || (type == rightHandItem._itype && rightHandItem._iStatFlag);
	}
};

extern DVL_API_FOR_TEST uint8_t MyPlayerId;
extern DVL_API_FOR_TEST Player *MyPlayer;
extern DVL_API_FOR_TEST std::vector<Player> Players;
/** @brief What Player items and stats should be displayed? Normally this is identical to MyPlayer but can differ when /inspect was used. */
extern Player *InspectPlayer;
/** @brief Do we currently inspect a remote player (/inspect was used)? In this case the (remote) players items and stats can't be modified. */
inline bool IsInspectingPlayer()
{
	return MyPlayer != InspectPlayer;
}
extern bool MyPlayerIsDead;

Player *PlayerAtPosition(Point position, bool ignoreMovingPlayers = false);

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
void AddPlrMonstExper(int lvl, unsigned int exp, char pmask);
void ApplyPlrDamage(DamageType damageType, Player &player, int dam, int minHP = 0, int frac = 0, DeathReason deathReason = DeathReason::MonsterOrTrap);
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
void StartPlayerKill(Player &player, DeathReason deathReason);
/**
 * @brief Strip the top off gold piles that are larger than MaxGold
 */
void StripTopGold(Player &player);
void SyncPlrKill(Player &player, DeathReason deathReason);
void RemovePlrMissiles(const Player &player);
void StartNewLvl(Player &player, interface_mode fom, int lvl);
void RestartTownLvl(Player &player);
void StartWarpLvl(Player &player, size_t pidx);
void ProcessPlayers();
void ClrPlrPath(Player &player);
bool PosOkPlayer(const Player &player, Point position);
void MakePlrPath(Player &player, Point targetPosition, bool endspace);
void CalcPlrStaff(Player &player);
void CheckPlrSpell(bool isShiftHeld, SpellID spellID = MyPlayer->selectedSpell, SpellType spellType = MyPlayer->selectedSpellType);
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

} // namespace devilution
