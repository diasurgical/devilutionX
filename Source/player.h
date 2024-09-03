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

extern DVL_API_FOR_TEST Player *MyPlayer;

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
	/* @brief HP is zero but we don't know when or where this happened */
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

class SpellCastInfo {
public: // TODO: Make private when all direct usage of members variables are removed.
	SpellID spellId;
	SpellType spellType;
	int8_t spellFrom;
	int spellLevel;

	// TODO: Set public here when all direct usage of members variables are removed.
	SpellID getSpellId() const { return spellId; }
	void setSpellId(SpellID id) { spellId = id; }

	SpellType getSpellType() const { return spellType; }
	void setSpellType(SpellType type) { spellType = type; }

	int8_t getSpellFrom() const { return spellFrom; }
	void setSpellFrom(int8_t from) { spellFrom = from; }

	int getSpellLevel() const { return spellLevel; }
	void setSpellLevel(int level) { spellLevel = level; }
};

class Player {
public: // TODO: Make private when all direct usage of members variables are removed.
	char _pName[PlayerNameLength];
	Item InvBody[NUM_INVLOC];
	Item InvList[InventoryGridCells];
	Item SpdList[MaxBeltItems];
	Item HoldItem;

	int lightId;

	int _pNumInv;
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
	int _pIMinDam;
	int _pIMaxDam;
	int _pIAC;
	int _pIBonusDam;
	int _pIBonusToHit;
	int _pIBonusAC;
	int _pIBonusDamMod;
	int _pIGetHit;
	int _pIEnAc;
	int _pIFMinDam;
	int _pIFMaxDam;
	int _pILMinDam;
	int _pILMaxDam;
	uint32_t _pExperience;
	PLR_MODE _pmode;
	int8_t walkpath[MaxPathLength];
	bool plractive;
	action_id destAction;
	int destParam1;
	int destParam2;
	int destParam3;
	int destParam4;
	int _pGold;

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
	int8_t progressToNextGameTickWhenPreviewWasSet;
	/** @brief Bitmask using item_special_effect */
	ItemSpecialEffect _pIFlags;
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
	int8_t InvGrid[InventoryGridCells];

	uint8_t plrlevel;
	bool plrIsOnSetLevel;
	ActorPosition position;
	Direction _pdir; // Direction faced by player (direction enum)
	HeroClass _pClass;
	uint8_t _pLevel = 1; // Use get/setCharacterLevel to ensure this attribute stays within the accepted range
	uint8_t _pgfxnum;    // Bitmask indicating what variant of the sprite the player is using. The 3 lower bits define weapon (PlayerWeaponGraphic) and the higher bits define armour (starting with PlayerArmorGraphic)
	int8_t _pISplLvlAdd;
	/** @brief Specifies whether players are in non-PvP mode. */
	bool friendlyMode = true;

	/** @brief The next queued spell */
	SpellCastInfo queuedSpell;
	/** @brief The spell that is currently being cast */
	SpellCastInfo executedSpell;
	/* @brief Which spell should be executed with CURSOR_TELEPORT */
	SpellID inventorySpell;
	/* @brief Inventory location for scrolls with CURSOR_TELEPORT */
	int8_t spellFrom;
	SpellID _pRSpell;
	SpellType _pRSplType;
	SpellID _pSBkSpell;
	uint8_t _pSplLvl[64];
	/** @brief Bitmask of staff spell */
	uint64_t _pISpells;
	/** @brief Bitmask of learned spells */
	uint64_t _pMemSpells;
	/** @brief Bitmask of abilities */
	uint64_t _pAblSpells;
	/** @brief Bitmask of spells available via scrolls */
	uint64_t _pScrlSpells;
	SpellFlag _pSpellFlags;
	SpellID _pSplHotKey[NumHotkeys];
	SpellType _pSplTHotKey[NumHotkeys];
	bool _pBlockFlag;
	bool _pInvincible;
	int8_t _pLightRad;
	/** @brief True when the player is transitioning between levels */
	bool _pLvlChanging;

	int8_t _pArmorClass;
	int8_t _pMagResist;
	int8_t _pFireResist;
	int8_t _pLghtResist;
	bool _pInfraFlag;
	/** Player's direction when ending movement. Also used for casting direction of SpellID::FireWall. */
	Direction tempDirection;

	bool _pLvlVisited[NUMLEVELS];
	bool _pSLvlVisited[NUMLEVELS]; // only 10 used

	item_misc_id _pOilType;
	uint8_t pTownWarps;
	uint8_t pDungMsgs;
	uint8_t pLvlLoad;
	bool pManaShield;
	uint8_t pDungMsgs2;
	bool pOriginalCathedral;
	uint8_t pDiabloKillLevel;
	uint16_t wReflections;
	ItemSpecialEffectHf pDamAcFlags;

private:
	void _addExperience(uint32_t experience, int levelDelta);

public:
	Player() = default;
	Player(Player &&) noexcept = default;
	Player &operator=(Player &&) noexcept = default;

	const bool isMyPlayer() const { return this == MyPlayer; }

	// Name
	const char *getName() const { return _pName; }
	void setName(const char *name) { strncpy(_pName, name, PlayerNameLength); }

	// Inventory and items
	const Item &getInvBody(int index) const { return InvBody[index]; }
	void setInvBody(int index, const Item &item) { InvBody[index] = item; }

	const Item &getInvList(int index) const { return InvList[index]; }
	void setInvList(int index, const Item &item) { InvList[index] = item; }

	const Item &getSpdList(int index) const { return SpdList[index]; }
	void setSpdList(int index, const Item &item) { SpdList[index] = item; }

	const Item &getHoldItem() const { return HoldItem; }
	void setHoldItem(const Item &item) { HoldItem = item; }

	// Light ID
	int getLightId() const { return lightId; }
	void setLightId(int id) { lightId = id; }

	// Stats
	int getStrength() const { return _pStrength; }
	void setStrength(int strength) { _pStrength = strength; }

	int getBaseStrength() const { return _pBaseStr; }
	void setBaseStrength(int strength) { _pBaseStr = strength; }

	int getMagic() const { return _pMagic; }
	void setMagic(int magic) { _pMagic = magic; }

	int getBaseMagic() const { return _pBaseMag; }
	void setBaseMagic(int magic) { _pBaseMag = magic; }

	int getDexterity() const { return _pDexterity; }
	void setDexterity(int dexterity) { _pDexterity = dexterity; }

	int getBaseDexterity() const { return _pBaseDex; }
	void setBaseDexterity(int dexterity) { _pBaseDex = dexterity; }

	int getVitality() const { return _pVitality; }
	void setVitality(int vitality) { _pVitality = vitality; }

	int getBaseVitality() const { return _pBaseVit; }
	void setBaseVitality(int vitality) { _pBaseVit = vitality; }

	int getStatPoints() const { return _pStatPts; }
	void setStatPoints(int points) { _pStatPts = points; }

	int getDamageMod() const { return _pDamageMod; }
	void setDamageMod(int mod) { _pDamageMod = mod; }

	int getHPBase() const { return _pHPBase; }
	void setHPBase(int hp) { _pHPBase = hp; }

	int getMaxHPBase() const { return _pMaxHPBase; }
	void setMaxHPBase(int maxHP) { _pMaxHPBase = maxHP; }

	int getHitPoints() const { return _pHitPoints; }
	void setHitPoints(int hp) { _pHitPoints = hp; }

	int getMaxHitPoints() const { return _pMaxHP; }
	void setMaxHitPoints(int maxHP) { _pMaxHP = maxHP; }

	int getHPPer() const { return _pHPPer; }
	void setHPPer(int hpPer) { _pHPPer = hpPer; }

	int getManaBase() const { return _pManaBase; }
	void setManaBase(int mana) { _pManaBase = mana; }

	int getMaxManaBase() const { return _pMaxManaBase; }
	void setMaxManaBase(int maxMana) { _pMaxManaBase = maxMana; }

	int getMana() const { return _pMana; }
	void setMana(int mana) { _pMana = mana; }

	int getMaxMana() const { return _pMaxMana; }
	void setMaxMana(int maxMana) { _pMaxMana = maxMana; }

	int getManaPer() const { return _pManaPer; }
	void setManaPer(int manaPer) { _pManaPer = manaPer; }

	int getIMinDam() const { return _pIMinDam; }
	void setIMinDam(int minDam) { _pIMinDam = minDam; }

	int getIMaxDam() const { return _pIMaxDam; }
	void setIMaxDam(int maxDam) { _pIMaxDam = maxDam; }

	int getIAC() const { return _pIAC; }
	void setIAC(int ac) { _pIAC = ac; }

	int getIBonusDam() const { return _pIBonusDam; }
	void setIBonusDam(int bonusDam) { _pIBonusDam = bonusDam; }

	int getIBonusToHit() const { return _pIBonusToHit; }
	void setIBonusToHit(int bonusToHit) { _pIBonusToHit = bonusToHit; }

	int getIBonusAC() const { return _pIBonusAC; }
	void setIBonusAC(int bonusAC) { _pIBonusAC = bonusAC; }

	int getIBonusDamMod() const { return _pIBonusDamMod; }
	void setIBonusDamMod(int bonusDamMod) { _pIBonusDamMod = bonusDamMod; }

	int getIGetHit() const { return _pIGetHit; }
	void setIGetHit(int getHit) { _pIGetHit = getHit; }

	int getIEnAc() const { return _pIEnAc; }
	void setIEnAc(int enAc) { _pIEnAc = enAc; }

	int getIFMinDam() const { return _pIFMinDam; }
	void setIFMinDam(int minDam) { _pIFMinDam = minDam; }

	int getIFMaxDam() const { return _pIFMaxDam; }
	void setIFMaxDam(int maxDam) { _pIFMaxDam = maxDam; }

	int getILMinDam() const { return _pILMinDam; }
	void setILMinDam(int minDam) { _pILMinDam = minDam; }

	int getILMaxDam() const { return _pILMaxDam; }
	void setILMaxDam(int maxDam) { _pILMaxDam = maxDam; }

	uint32_t getExperience() const { return _pExperience; }
	void setExperience(uint32_t experience) { _pExperience = experience; }

	PLR_MODE getMode() const { return _pmode; }
	void setMode(PLR_MODE mode) { _pmode = mode; }

	const int8_t *getWalkPath() const { return walkpath; }
	void setWalkPath(const int8_t path[MaxPathLength]) { std::copy(path, path + MaxPathLength, walkpath); }

	bool isActive() const { return plractive; }
	void setActive(bool active) { plractive = active; }

	action_id getDestAction() const { return destAction; }
	void setDestAction(action_id action) { destAction = action; }

	int getDestParam1() const { return destParam1; }
	void setDestParam1(int param) { destParam1 = param; }

	int getDestParam2() const { return destParam2; }
	void setDestParam2(int param) { destParam2 = param; }

	int getDestParam3() const { return destParam3; }
	void setDestParam3(int param) { destParam3 = param; }

	int getDestParam4() const { return destParam4; }
	void setDestParam4(int param) { destParam4 = param; }

	int getGold() const { return _pGold; }
	void setGold(int gold) { _pGold = gold; }

	// AnimationInfo
	const AnimationInfo &getAnimInfo() const { return AnimInfo; }
	void setAnimInfo(const AnimationInfo &animInfo) { AnimInfo = animInfo; }

	// Preview Sprite
	const OptionalClxSprite &getPreviewCelSprite() const { return previewCelSprite; }
	void setPreviewCelSprite(const OptionalClxSprite &sprite) { previewCelSprite = sprite; }

	// Progress to Next Game Tick
	int8_t getProgressToNextGameTickWhenPreviewWasSet() const { return progressToNextGameTickWhenPreviewWasSet; }
	void setProgressToNextGameTickWhenPreviewWasSet(int8_t progress) { progressToNextGameTickWhenPreviewWasSet = progress; }

	// Flags
	ItemSpecialEffect getIFlags() const { return _pIFlags; }
	void setIFlags(ItemSpecialEffect flags) { _pIFlags = flags; }

	// Animation Data
	const std::array<PlayerAnimationData, enum_size<player_graphic>::value> &getAnimationData() const { return AnimationData; }
	void setAnimationData(std::array<PlayerAnimationData, enum_size<player_graphic>::value> &&data)
	{
		AnimationData = std::move(data);
	}

	// Frames
	int8_t getNFrames() const { return _pNFrames; }
	void setNFrames(int8_t nFrames) { _pNFrames = nFrames; }

	int8_t getWFrames() const { return _pWFrames; }
	void setWFrames(int8_t wFrames) { _pWFrames = wFrames; }

	int8_t getAFrames() const { return _pAFrames; }
	void setAFrames(int8_t aFrames) { _pAFrames = aFrames; }

	int8_t getAFNum() const { return _pAFNum; }
	void setAFNum(int8_t afNum) { _pAFNum = afNum; }

	int8_t getSFrames() const { return _pSFrames; }
	void setSFrames(int8_t sFrames) { _pSFrames = sFrames; }

	int8_t getSFNum() const { return _pSFNum; }
	void setSFNum(int8_t sfNum) { _pSFNum = sfNum; }

	int8_t getHFrames() const { return _pHFrames; }
	void setHFrames(int8_t hFrames) { _pHFrames = hFrames; }

	int8_t getDFrames() const { return _pDFrames; }
	void setDFrames(int8_t dFrames) { _pDFrames = dFrames; }

	int8_t getBFrames() const { return _pBFrames; }
	void setBFrames(int8_t bFrames) { _pBFrames = bFrames; }

	// Inventory Grid
	const int8_t *getInvGrid() const { return InvGrid; }
	void setInvGrid(const int8_t grid[InventoryGridCells]) { std::copy(grid, grid + InventoryGridCells, InvGrid); }

	// Player Level
	uint8_t getPlayerLevel() const { return plrlevel; }
	void setPlayerLevel(uint8_t level) { plrlevel = level; }

	bool isOnSetLevel() const { return plrIsOnSetLevel; }
	void setOnSetLevel(bool onSetLevel) { plrIsOnSetLevel = onSetLevel; }

	// Player Position
	ActorPosition &getPosition() { return position; }
	const ActorPosition &getPosition() const { return position; }
	void setPosition(const ActorPosition &pos) { position = pos; }

	// Player Direction
	Direction getDirection() const { return _pdir; }
	void setDirection(Direction dir) { _pdir = dir; }

	// Player Class
	HeroClass getPlayerClass() const { return _pClass; }
	void setPlayerClass(HeroClass playerClass) { _pClass = playerClass; }

	// GFX Number
	uint8_t getGfxNum() const { return _pgfxnum; }
	void setGfxNum(uint8_t gfxNum) { _pgfxnum = gfxNum; }

	// Spell Level Addition
	int8_t getSpellLevelAdd() const { return _pISplLvlAdd; }
	void setSpellLevelAdd(int8_t spellLevelAdd) { _pISplLvlAdd = spellLevelAdd; }

	// Friendly Mode
	bool isFriendlyMode() const { return friendlyMode; }
	void setFriendlyMode(bool friendly) { friendlyMode = friendly; }

	// Queued Spell
	SpellCastInfo &getQueuedSpell() { return queuedSpell; }
	const SpellCastInfo &getQueuedSpell() const { return queuedSpell; }
	void setQueuedSpell(const SpellCastInfo &spell) { queuedSpell = spell; }

	// Executed Spell
	const SpellCastInfo &getExecutedSpell() const { return executedSpell; }
	void setExecutedSpell(const SpellCastInfo &spell) { executedSpell = spell; }

	// Inventory Spell
	SpellID getInventorySpell() const { return inventorySpell; }
	void setInventorySpell(SpellID spell) { inventorySpell = spell; }

	// Spell From
	int8_t getSpellFrom() const { return spellFrom; }
	void setSpellFrom(int8_t from) { spellFrom = from; }

	// Ready Spell
	SpellID getReadySpell() const { return _pRSpell; }
	void setReadySpell(SpellID spell) { _pRSpell = spell; }

	// Ready Spell Type
	SpellType getReadySpellType() const { return _pRSplType; }
	void setReadySpellType(SpellType type) { _pRSplType = type; }

	// Spell Book Spell
	SpellID getSpellBookSpell() const { return _pSBkSpell; }
	void setSpellBookSpell(SpellID spell) { _pSBkSpell = spell; }

	// Spell Levels
	const uint8_t *getSpellLevels() const { return _pSplLvl; }
	void setSpellLevels(const uint8_t levels[64]) { std::copy(levels, levels + 64, _pSplLvl); }

	// Spell Flags
	SpellFlag getSpellFlags() const { return _pSpellFlags; }
	void setSpellFlags(SpellFlag flags) { _pSpellFlags = flags; }

	// Hotkeys
	const SpellID *getSpellHotKeys() const { return _pSplHotKey; }
	void setSpellHotKeys(const SpellID hotKeys[NumHotkeys]) { std::copy(hotKeys, hotKeys + NumHotkeys, _pSplHotKey); }

	const SpellType *getSpellHotKeyTypes() const { return _pSplTHotKey; }
	void setSpellHotKeyTypes(const SpellType hotKeyTypes[NumHotkeys]) { std::copy(hotKeyTypes, hotKeyTypes + NumHotkeys, _pSplTHotKey); }

	// Block Flag
	bool hasBlockFlag() const { return _pBlockFlag; }
	void setBlockFlag(bool blockFlag) { _pBlockFlag = blockFlag; }

	// Invincible Flag
	bool isInvincible() const { return _pInvincible; }
	void setInvincible(bool invincible) { _pInvincible = invincible; }

	// Light Radius
	int8_t getLightRadius() const { return _pLightRad; }
	void setLightRadius(int8_t lightRad) { _pLightRad = lightRad; }

	// Level Changing Flag
	bool isLevelChanging() const { return _pLvlChanging; }
	void setLevelChanging(bool levelChanging) { _pLvlChanging = levelChanging; }

	// Armor Class
	int8_t getArmorClass() const { return _pArmorClass; }
	void setArmorClass(int8_t armorClass) { _pArmorClass = armorClass; }

	// Magic Resistance
	int8_t getMagicResist() const { return _pMagResist; }
	void setMagicResist(int8_t resist) { _pMagResist = resist; }

	// Fire Resistance
	int8_t getFireResist() const { return _pFireResist; }
	void setFireResist(int8_t resist) { _pFireResist = resist; }

	// Lightning Resistance
	int8_t getLightningResist() const { return _pLghtResist; }
	void setLightningResist(int8_t resist) { _pLghtResist = resist; }

	// Infrared Flag
	bool hasInfraFlag() const { return _pInfraFlag; }
	void setInfraFlag(bool infraFlag) { _pInfraFlag = infraFlag; }

	// Temporary Direction
	Direction getTempDirection() const { return tempDirection; }
	void setTempDirection(Direction dir) { tempDirection = dir; }

	// Level Visited Flags
	const bool *getLevelVisited() const { return _pLvlVisited; }
	void setLevelVisited(const bool visited[NUMLEVELS]) { std::copy(visited, visited + NUMLEVELS, _pLvlVisited); }

	const bool *getSetLevelVisited() const { return _pSLvlVisited; }
	void setSetLevelVisited(const bool visited[NUMLEVELS]) { std::copy(visited, visited + NUMLEVELS, _pSLvlVisited); }

	// Oil Type
	item_misc_id getOilType() const { return _pOilType; }
	void setOilType(item_misc_id oilType) { _pOilType = oilType; }

	// Town Warps
	uint8_t getTownWarps() const { return pTownWarps; }
	void setTownWarps(uint8_t warps) { pTownWarps = warps; }

	// Dungeon Messages
	uint8_t getDungeonMessages() const { return pDungMsgs; }
	void setDungeonMessages(uint8_t msgs) { pDungMsgs = msgs; }

	uint8_t getLevelLoad() const { return pLvlLoad; }
	void setLevelLoad(uint8_t load) { pLvlLoad = load; }

	// Mana Shield
	bool hasManaShield() const { return pManaShield; }
	void setManaShield(bool manaShield) { pManaShield = manaShield; }

	// Original Cathedral
	bool hasOriginalCathedral() const { return pOriginalCathedral; }
	void setOriginalCathedral(bool originalCathedral) { pOriginalCathedral = originalCathedral; }

	// Diablo Kill Level
	uint8_t getDiabloKillLevel() const { return pDiabloKillLevel; }
	void setDiabloKillLevel(uint8_t level) { pDiabloKillLevel = level; }

	// Reflections
	uint16_t getReflections() const { return wReflections; }
	void setReflections(uint16_t reflections) { wReflections = reflections; }

	// Damage & Armor Class Flags
	ItemSpecialEffectHf getDamageArmorClassFlags() const { return pDamAcFlags; }
	void setDamageArmorClassFlags(ItemSpecialEffectHf flags) { pDamAcFlags = flags; }

	/**
	 * @brief Convenience function to get the base stats/bonuses for this player's class
	 */
	[[nodiscard]] const ClassAttributes &getClassAttributes() const
	{
		return GetClassAttributes(_pClass);
	}

	[[nodiscard]] const PlayerCombatData &getPlayerCombatData() const
	{
		return GetPlayerCombatDataForClass(_pClass);
	}

	[[nodiscard]] const PlayerData &getPlayerData() const
	{
		return GetPlayerDataForClass(_pClass);
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
		return _pStrength >= item._iMinStr
		    && _pMagic >= item._iMinMag
		    && _pDexterity >= item._iMinDex;
	}

	bool CanCleave()
	{
		switch (_pClass) {
		case HeroClass::Warrior:
		case HeroClass::Rogue:
		case HeroClass::Sorcerer:
			return false;
		case HeroClass::Monk:
			return isEquipped(ItemType::Staff);
		case HeroClass::Bard:
			return InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Sword && InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Sword;
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
			return (InvBody[INVLOC_HAND_LEFT]._itype == itemType && (!isTwoHanded || InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND))
			    || (InvBody[INVLOC_HAND_RIGHT]._itype == itemType && (!isTwoHanded || InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND));
		case ItemType::LightArmor:
		case ItemType::MediumArmor:
		case ItemType::HeavyArmor:
			return InvBody[INVLOC_CHEST]._itype == itemType;
		case ItemType::Helm:
			return InvBody[INVLOC_HEAD]._itype == itemType;
		case ItemType::Ring:
			return InvBody[INVLOC_RING_LEFT]._itype == itemType || InvBody[INVLOC_RING_RIGHT]._itype == itemType;
		case ItemType::Amulet:
			return InvBody[INVLOC_AMULET]._itype == itemType;
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
		return getCharacterLevel() + _pDexterity / 2 + _pIBonusToHit + getPlayerCombatData().baseMeleeToHit;
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
		return getCharacterLevel() + _pDexterity + _pIBonusToHit + getPlayerCombatData().baseRangedToHit;
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
		return _pMagic + getPlayerCombatData().baseMagicToHit;
	}

	/**
	 * @brief Return block chance
	 * @param useLevel - indicate if player's level should be added to block chance (the only case where it isn't is blocking a trap)
	 */
	int GetBlockChance(bool useLevel = true) const
	{
		int blkper = _pDexterity + getBaseToBlock();
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
		if (spell == SpellID::Invalid || static_cast<std::size_t>(spell) >= sizeof(_pSplLvl)) {
			return 0;
		}

		return std::max<int>(_pISplLvlAdd + _pSplLvl[static_cast<std::size_t>(spell)], 0);
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
			_pHPPer = std::clamp(_pHitPoints * 80 / _pMaxHP, 0, 80); // hp should never be greater than maxHP but just in case
		}

		return _pHPPer;
	}

	int UpdateManaPercentage()
	{
		if (_pMaxMana <= 0) {
			_pManaPer = 0;
		} else {
			_pManaPer = std::clamp(_pMana * 80 / _pMaxMana, 0, 80);
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
	 * @param forceSpell - if true, always change active spell, if false, only when current spell slot is empty
	 */
	void ReadySpellFromEquipment(inv_body_loc bodyLocation, bool forceSpell);

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
		if (isWalking() && AnimInfo.isLastFrame())
			return true;
		return false;
	}

	[[nodiscard]] player_graphic getGraphic() const;

	[[nodiscard]] uint16_t getSpriteWidth() const;

	void getAnimationFramesAndTicksPerFrame(player_graphic graphics, int8_t &numberOfFrames, int8_t &ticksPerFrame) const;

	[[nodiscard]] ClxSprite currentSprite() const
	{
		return previewCelSprite ? *previewCelSprite : AnimInfo.currentSprite();
	}
	[[nodiscard]] Displacement getRenderingOffset(const ClxSprite sprite) const
	{
		Displacement offset = { -CalculateWidth2(sprite.width()), 0 };
		if (isWalking())
			offset += GetOffsetForWalking(AnimInfo, _pdir);
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
		return _pLevel;
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
		return !this->plrIsOnSetLevel && this->plrlevel == level;
	}
	/** @brief Checks if the player is on the corresponding level. */
	bool isOnLevel(_setlevels level) const
	{
		return this->plrIsOnSetLevel && this->plrlevel == static_cast<uint8_t>(level);
	}
	/** @brief Checks if the player is on a arena level. */
	bool isOnArenaLevel() const
	{
		return plrIsOnSetLevel && IsArenaLevel(static_cast<_setlevels>(plrlevel));
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
		const Item &leftHandItem = InvBody[INVLOC_HAND_LEFT];
		const Item &rightHandItem = InvBody[INVLOC_HAND_RIGHT];

		return (type == leftHandItem._itype && leftHandItem._iStatFlag) || (type == rightHandItem._itype && rightHandItem._iStatFlag);
	}
};

extern DVL_API_FOR_TEST uint8_t MyPlayerId;
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
void CheckPlrSpell(bool isShiftHeld, SpellID spellID = MyPlayer->getReadySpell(), SpellType spellType = MyPlayer->getReadySpellType());
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
