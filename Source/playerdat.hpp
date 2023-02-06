/**
 * @file playerdat.hpp
 *
 * Interface of all player data.
 */
#pragma once

#include <cstdint>

#include "player.h"
#include "textdat.h"

namespace devilution {

struct PlayerData {
	/* Class Name */
	const char *className;
	/* Class Directory Path */
	const char *classPath;
	/* Class Starting Strength Stat */
	uint8_t baseStr;
	/* Class Starting Magic Stat */
	uint8_t baseMag;
	/* Class Starting Dexterity Stat */
	uint8_t baseDex;
	/* Class Starting Vitality Stat */
	uint8_t baseVit;
	/* Class Maximum Strength Stat */
	uint8_t maxStr;
	/* Class Maximum Magic Stat */
	uint8_t maxMag;
	/* Class Maximum Dexterity Stat */
	uint8_t maxDex;
	/* Class Maximum Vitality Stat */
	uint8_t maxVit;
	/* Class Block Bonus % */
	uint8_t blockBonus;
	/* Life gained on level up */
	uint16_t lvlUpLife;
	/* Mana gained on level up */
	uint16_t lvlUpMana;
	/* Life from base Vitality */
	uint16_t chrLife;
	/* Mana from base Magic */
	uint16_t chrMana;
	/* Life from item bonus Vitality */
	uint16_t itmLife;
	/* Mana from item bonus Magic */
	uint16_t itmMana;
	/* Nothing (Idle) frame count */
	int8_t NFrames;
	/* Walking frame count */
	int8_t WFrames;
	/* Blocking frame count */
	int8_t BFrames;
	/* Death frame count */
	int8_t DFrames;
	/* Spellcasting frame count */
	int8_t SFrames;
	/* Hit Recovery frame count */
	int8_t HFrames;
	/* Town Nothing (Idle) frame count */
	int8_t TNFrames;
	/* Town Walking frame count */
	int8_t TWFrames;
	/* Spellcasting action frame number */
	int8_t SFNum;
};

struct PlayerAttackAnimData {
	/* Unarmed frame count */
	int8_t AFramesUnarmed;
	/* Unarmed action frame number */
	int8_t AFNumUnarmed;
	/* UnarmedShield frame count */
	int8_t AFramesUnarmedShield;
	/* UnarmedShield action frame number */
	int8_t AFNumUnarmedShield;
	/* Sword frame count */
	int8_t AFramesSword;
	/* Sword action frame number */
	int8_t AFNumSword;
	/* SwordShield frame count */
	int8_t AFramesSwordShield;
	/* SwordShield action frame number */
	int8_t AFNumSwordShield;
	/* Bow frame count */
	int8_t AFramesBow;
	/* Bow action frame number */
	int8_t AFNumBow;
	/* Axe frame count */
	int8_t AFramesAxe;
	/* Axe action frame number */
	int8_t AFNumAxe;
	/* Mace frame count */
	int8_t AFramesMace;
	/* Mace action frame */
	int8_t AFNumMace;
	/* MaceShield frame count */
	int8_t AFramesMaceShield;
	/* MaceShield action frame number */
	int8_t AFNumMaceShield;
	/* Staff frame count */
	int8_t AFramesStaff;
	/* Staff action frame number */
	int8_t AFNumStaff;
};

extern const _sfx_id herosounds[enum_size<HeroClass>::value][enum_size<HeroSpeech>::value];
extern const uint32_t ExpLvlsTbl[MaxCharacterLevel + 1];
extern const PlayerData PlayersData[];
extern const PlayerAttackAnimData PlayersAttackAnimData[];

} // namespace devilution
