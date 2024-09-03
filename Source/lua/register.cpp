#include "register.hpp"

#include <sol/sol.hpp>

#include "engine/actor_position.hpp"
#include "player.h"
#include "utils/enum_traits.h"

namespace devilution {

static void RegisterPlayerStruct(sol::state &lua)
{
	lua.new_usertype<Player>("Player",
	    // Constructors
	    sol::constructors<Player()>(),

	    // Member variables
	    "_pName", &Player::_pName,
	    "InvBody", &Player::InvBody,
	    "InvList", &Player::InvList,
	    "SpdList", &Player::SpdList,
	    "HoldItem", &Player::HoldItem,
	    "lightId", &Player::lightId,
	    "_pNumInv", &Player::_pNumInv,
	    "_pStrength", &Player::_pStrength,
	    "_pBaseStr", &Player::_pBaseStr,
	    "_pMagic", &Player::_pMagic,
	    "_pBaseMag", &Player::_pBaseMag,
	    "_pDexterity", &Player::_pDexterity,
	    "_pBaseDex", &Player::_pBaseDex,
	    "_pVitality", &Player::_pVitality,
	    "_pBaseVit", &Player::_pBaseVit,
	    "_pStatPts", &Player::_pStatPts,
	    "_pDamageMod", &Player::_pDamageMod,
	    "_pHPBase", &Player::_pHPBase,
	    "_pMaxHPBase", &Player::_pMaxHPBase,
	    "_pHitPoints", &Player::_pHitPoints,
	    "_pMaxHP", &Player::_pMaxHP,
	    "_pHPPer", &Player::_pHPPer,
	    "_pManaBase", &Player::_pManaBase,
	    "_pMaxManaBase", &Player::_pMaxManaBase,
	    "_pMana", &Player::_pMana,
	    "_pMaxMana", &Player::_pMaxMana,
	    "_pManaPer", &Player::_pManaPer,
	    "_pIMinDam", &Player::_pIMinDam,
	    "_pIMaxDam", &Player::_pIMaxDam,
	    "_pIAC", &Player::_pIAC,
	    "_pIBonusDam", &Player::_pIBonusDam,
	    "_pIBonusToHit", &Player::_pIBonusToHit,
	    "_pIBonusAC", &Player::_pIBonusAC,
	    "_pIBonusDamMod", &Player::_pIBonusDamMod,
	    "_pIGetHit", &Player::_pIGetHit,
	    "_pIEnAc", &Player::_pIEnAc,
	    "_pIFMinDam", &Player::_pIFMinDam,
	    "_pIFMaxDam", &Player::_pIFMaxDam,
	    "_pILMinDam", &Player::_pILMinDam,
	    "_pILMaxDam", &Player::_pILMaxDam,
	    "_pExperience", &Player::_pExperience,
	    "_pmode", &Player::_pmode,
	    "walkpath", &Player::walkpath,
	    "plractive", &Player::plractive,
	    "destAction", &Player::destAction,
	    "destParam1", &Player::destParam1,
	    "destParam2", &Player::destParam2,
	    "destParam3", &Player::destParam3,
	    "destParam4", &Player::destParam4,
	    "_pGold", &Player::_pGold,

	    // Member methods
	    "getClassAttributes", &Player::getClassAttributes,
	    "getPlayerCombatData", &Player::getPlayerCombatData,
	    "getPlayerData", &Player::getPlayerData,
	    "getClassName", &Player::getClassName,
	    "getBaseToBlock", &Player::getBaseToBlock,
	    "CalcScrolls", &Player::CalcScrolls,
	    "CanUseItem", &Player::CanUseItem,
	    "CanCleave", &Player::CanCleave,
	    "isEquipped", &Player::isEquipped,
	    "RemoveInvItem", &Player::RemoveInvItem,
	    "getId", &Player::getId,
	    "RemoveSpdBarItem", &Player::RemoveSpdBarItem,
	    //"GetMostValuableItem", &Player::GetMostValuableItem,
	    "GetBaseAttributeValue", &Player::GetBaseAttributeValue,
	    "GetCurrentAttributeValue", &Player::GetCurrentAttributeValue,
	    "GetMaximumAttributeValue", &Player::GetMaximumAttributeValue,
	    "GetTargetPosition", &Player::GetTargetPosition,
	    "IsPositionInPath", &Player::IsPositionInPath,
	    "Say", sol::overload(static_cast<void (Player::*)(HeroSpeech) const>(&Player::Say), static_cast<void (Player::*)(HeroSpeech, int) const>(&Player::Say), static_cast<void (Player::*)(HeroSpeech) const>(&Player::SaySpecific)),
	    "Stop", &Player::Stop,
	    "isWalking", &Player::isWalking,
	    "GetItemLocation", &Player::GetItemLocation,
	    "GetArmor", &Player::GetArmor,
	    "GetMeleeToHit", &Player::GetMeleeToHit,
	    "GetMeleePiercingToHit", &Player::GetMeleePiercingToHit,
	    "GetRangedToHit", &Player::GetRangedToHit,
	    "GetRangedPiercingToHit", &Player::GetRangedPiercingToHit,
	    "GetMagicToHit", &Player::GetMagicToHit,
	    "GetBlockChance", &Player::GetBlockChance,
	    "GetManaShieldDamageReduction", &Player::GetManaShieldDamageReduction,
	    "GetSpellLevel", &Player::GetSpellLevel,
	    "CalculateArmorPierce", &Player::CalculateArmorPierce,
	    "UpdateHitPointPercentage", &Player::UpdateHitPointPercentage,
	    "UpdateManaPercentage", &Player::UpdateManaPercentage,
	    "RestorePartialLife", &Player::RestorePartialLife,
	    "RestoreFullLife", &Player::RestoreFullLife,
	    "RestorePartialMana", &Player::RestorePartialMana,
	    "RestoreFullMana", &Player::RestoreFullMana,
	    "ReadySpellFromEquipment", &Player::ReadySpellFromEquipment,
	    "UsesRangedWeapon", &Player::UsesRangedWeapon,
	    "CanChangeAction", &Player::CanChangeAction,
	    "getGraphic", &Player::getGraphic,
	    "getSpriteWidth", &Player::getSpriteWidth,
	    "getAnimationFramesAndTicksPerFrame", &Player::getAnimationFramesAndTicksPerFrame,
	    "currentSprite", &Player::currentSprite,
	    "getRenderingOffset", &Player::getRenderingOffset,
	    "UpdatePreviewCelSprite", &Player::UpdatePreviewCelSprite,
	    "getCharacterLevel", &Player::getCharacterLevel,
	    "setCharacterLevel", &Player::setCharacterLevel,
	    "getMaxCharacterLevel", &Player::getMaxCharacterLevel,
	    "isMaxCharacterLevel", &Player::isMaxCharacterLevel,
	    "addExperience", sol::overload(static_cast<void (Player::*)(uint32_t)>(&Player::addExperience), static_cast<void (Player::*)(uint32_t, int)>(&Player::addExperience)),
	    "getNextExperienceThreshold", &Player::getNextExperienceThreshold,
	    "isOnActiveLevel", &Player::isOnActiveLevel,
	    "isOnLevel", sol::overload(static_cast<bool (Player::*)(uint8_t) const>(&Player::isOnLevel), static_cast<bool (Player::*)(_setlevels) const>(&Player::isOnLevel)),
	    "isOnArenaLevel", &Player::isOnArenaLevel,
	    "setLevel", sol::overload(static_cast<void (Player::*)(uint8_t)>(&Player::setLevel), static_cast<void (Player::*)(_setlevels)>(&Player::setLevel)),
	    "calculateBaseLife", &Player::calculateBaseLife,
	    "calculateBaseMana", &Player::calculateBaseMana,
	    "occupyTile", &Player::occupyTile,
	    "isLevelOwnedByLocalClient", &Player::isLevelOwnedByLocalClient,
	    "isHoldingItem", &Player::isHoldingItem);
}

static void RegisterPlayerGlobals(sol::state &lua)
{
	lua["MyPlayer"] = &MyPlayer;
}

static void RegisterPlayerFunctions(sol::state &lua)
{
	lua.set_function("NewPlrAnim", &NewPlrAnim);
	lua.set_function("FixPlayerLocation", &FixPlayerLocation);
	lua.set_function("SetPlayerOld", &SetPlayerOld);
	lua.set_function("SyncPlrKill", &SyncPlrKill);
}

static void RegisterEnums(sol::state &lua)
{
	lua.new_enum<DeathReason>("DeathReason",
	    { { "MonsterOrTrap", DeathReason::MonsterOrTrap },
	        { "Player", DeathReason::Player },
	        { "Unknown", DeathReason::Unknown } });

	lua.new_enum<PLR_MODE>("PLR_MODE",
	    { { "PM_STAND", PM_STAND },
	        { "PM_WALK_NORTHWARDS", PM_WALK_NORTHWARDS },
	        { "PM_WALK_SOUTHWARDS", PM_WALK_SOUTHWARDS },
	        { "PM_WALK_SIDEWAYS", PM_WALK_SIDEWAYS },
	        { "PM_ATTACK", PM_ATTACK },
	        { "PM_RATTACK", PM_RATTACK },
	        { "PM_BLOCK", PM_BLOCK },
	        { "PM_GOTHIT", PM_GOTHIT },
	        { "PM_DEATH", PM_DEATH },
	        { "PM_SPELL", PM_SPELL },
	        { "PM_NEWLVL", PM_NEWLVL },
	        { "PM_QUIT", PM_QUIT } });
}

static void RegisterActorPosition(sol::state &lua)
{
	lua.new_usertype<ActorPosition>("ActorPosition",
	    "tile", &ActorPosition::tile,
	    "future", &ActorPosition::future,
	    "old", &ActorPosition::old,
	    "temp", &ActorPosition::temp);
}

static void RegisterPoint(sol::state &lua)
{
	lua.new_usertype<Point>("Point",
	    sol::constructors<Point(int, int)>(),
	    "x", &Point::x,
	    "y", &Point::y);
}

void RegisterAllBindings(sol::state &lua)
{
	RegisterPlayerStruct(lua);    // Register Player struct and its member functions
	RegisterPlayerGlobals(lua);   // Register global variables like MyPlayer
	RegisterPlayerFunctions(lua); // Register standalone functions related to Player
	RegisterEnums(lua);           // Register the necessary enums
	RegisterPoint(lua);           // Register the Point struct
	RegisterActorPosition(lua);   // Register the ActorPosition struct
}

} // namespace devilution
