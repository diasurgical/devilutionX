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
	/* Class Skill */
	SpellID skill;
};

struct PlayerSpriteData {
	/* Sprite width: Stand */
	uint8_t stand;
	/* Sprite width: Walk */
	uint8_t walk;
	/* Sprite width: Attack */
	uint8_t attack;
	/* Sprite width: Attack (Bow) */
	uint8_t bow;
	/* Sprite width: Hit Recovery */
	uint8_t swHit;
	/* Sprite width: Block */
	uint8_t block;
	/* Sprite width: Cast Lightning Spell */
	uint8_t lightning;
	/* Sprite width: Cast Fire Spell */
	uint8_t fire;
	/* Sprite width: Cast Magic Spell */
	uint8_t magic;
	/* Sprite width: Death */
	uint8_t death;
};

struct PlayerAnimData {
	/* Unarmed frame count */
	int8_t unarmedFrames;
	/* Unarmed action frame number */
	int8_t unarmedActionFrame;
	/* UnarmedShield frame count */
	int8_t unarmedShieldFrames;
	/* UnarmedShield action frame number */
	int8_t unarmedShieldActionFrame;
	/* Sword frame count */
	int8_t swordFrames;
	/* Sword action frame number */
	int8_t swordActionFrame;
	/* SwordShield frame count */
	int8_t swordShieldFrames;
	/* SwordShield action frame number */
	int8_t swordShieldActionFrame;
	/* Bow frame count */
	int8_t bowFrames;
	/* Bow action frame number */
	int8_t bowActionFrame;
	/* Axe frame count */
	int8_t axeFrames;
	/* Axe action frame number */
	int8_t axeActionFrame;
	/* Mace frame count */
	int8_t maceFrames;
	/* Mace action frame */
	int8_t maceActionFrame;
	/* MaceShield frame count */
	int8_t maceShieldFrames;
	/* MaceShield action frame number */
	int8_t maceShieldActionFrame;
	/* Staff frame count */
	int8_t staffFrames;
	/* Staff action frame number */
	int8_t staffActionFrame;
	/* Nothing (Idle) frame count */
	int8_t idleFrames;
	/* Walking frame count */
	int8_t walkingFrames;
	/* Blocking frame count */
	int8_t blockingFrames;
	/* Death frame count */
	int8_t deathFrames;
	/* Spellcasting frame count */
	int8_t castingFrames;
	/* Hit Recovery frame count */
	int8_t recoveryFrames;
	/* Town Nothing (Idle) frame count */
	int8_t townIdleFrames;
	/* Town Walking frame count */
	int8_t townWalkingFrames;
	/* Spellcasting action frame number */
	int8_t castingActionFrame;
};

extern const _sfx_id herosounds[enum_size<HeroClass>::value][enum_size<HeroSpeech>::value];
extern const uint32_t ExpLvlsTbl[MaxCharacterLevel + 1];
extern const PlayerData PlayersData[];
extern const PlayerSpriteData PlayersSpriteData[];
extern const PlayerAnimData PlayersAnimData[];

} // namespace devilution
