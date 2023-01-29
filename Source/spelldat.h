/**
 * @file spelldat.h
 *
 * Interface of all spell data.
 */
#pragma once

#include <cstdint>

#include "effects.h"

namespace devilution {

#define MAX_SPELLS 52

enum class SpellType : uint8_t {
	Skill,
	FIRST = Skill,
	Spell,
	Scroll,
	Charges,
	LAST = Charges,
	Invalid,
};

enum spell_id : int8_t {
	SPL_NULL,
	SPL_FIREBOLT,
	SPL_HEAL,
	SPL_LIGHTNING,
	SPL_FLASH,
	SPL_IDENTIFY,
	SPL_FIREWALL,
	SPL_TOWN,
	SPL_STONE,
	SPL_INFRA,
	SPL_RNDTELEPORT,
	SPL_MANASHIELD,
	SPL_FIREBALL,
	SPL_GUARDIAN,
	SPL_CHAIN,
	SPL_WAVE,
	SPL_DOOMSERP,
	SPL_BLODRIT,
	SPL_NOVA,
	SPL_INVISIBIL,
	SPL_FLAME,
	SPL_GOLEM,
	SPL_BLODBOIL,
	SPL_TELEPORT,
	SPL_APOCA,
	SPL_ETHEREALIZE,
	SPL_REPAIR,
	SPL_RECHARGE,
	SPL_DISARM,
	SPL_ELEMENT,
	SPL_CBOLT,
	SPL_HBOLT,
	SPL_RESURRECT,
	SPL_TELEKINESIS,
	SPL_HEALOTHER,
	SPL_FLARE,
	SPL_BONESPIRIT,
	SPL_LASTDIABLO = SPL_BONESPIRIT,
	SPL_MANA,
	SPL_MAGI,
	SPL_JESTER,
	SPL_LIGHTWALL,
	SPL_IMMOLAT,
	SPL_WARP,
	SPL_REFLECT,
	SPL_BERSERK,
	SPL_FIRERING,
	SPL_SEARCH,
	SPL_RUNEFIRE,
	SPL_RUNELIGHT,
	SPL_RUNENOVA,
	SPL_RUNEIMMOLAT,
	SPL_RUNESTONE,

	SPL_LAST = SPL_RUNESTONE,
	SPL_INVALID = -1,
};

enum class MagicType : uint8_t {
	Fire,
	Lightning,
	Magic,
};

enum class MissileID : int8_t {
	// clang-format off
	Arrow,
	Firebolt,
	Guardian,
	Phasing,
	NovaBall,
	FireWall,
	Fireball,
	LightningControl,
	Lightning,
	MagmaBallExplosion,
	TownPortal,
	FlashBottom,
	FlashTop,
	ManaShield,
	FlameWave,
	ChainLightning,
	ChainBall, // unused
	BloodHit, // unused
	BoneHit, // unused
	MetalHit, // unused
	Rhino,
	MagmaBall,
	ThinLightningControl,
	ThinLightning,
	BloodStar,
	BloodStarExplosion,
	Teleport,
	FireArrow,
	DoomSerpents, // unused
	FireOnly, // unused
	StoneCurse,
	BloodRitual, // unused
	Invisibility, // unused
	Golem,
	Etherealize,
	Spurt, // unused
	ApocalypseBoom,
	Healing,
	FireWallControl,
	Infravision,
	Identify,
	FlameWaveControl,
	Nova,
	Rage, // BloodBoil in Diablo
	Apocalypse,
	ItemRepair,
	StaffRecharge,
	TrapDisarm,
	Inferno,
	InfernoControl,
	FireMan, // unused
	Krull, // unused
	ChargedBolt,
	HolyBolt,
	Resurrect,
	Telekinesis,
	LightningArrow,
	Acid,
	AcidSplat,
	AcidPuddle,
	HealOther,
	Elemental,
	ResurrectBeam,
	BoneSpirit,
	WeaponExplosion,
	RedPortal,
	DiabloApocalypseBoom,
	DiabloApocalypse,
	Mana,
	Magi,
	LightningWall,
	LightningWallControl,
	Immolation,
	SpectralArrow,
	FireballBow,
	LightningBow,
	ChargedBoltBow,
	HolyBoltBow,
	Warp,
	Reflect,
	Berserk,
	RingOfFire,
	StealPotions,
	StealMana,
	RingOfLightning, // unused
	Search,
	Aura, // unused
	Aura2, // unused
	SpiralFireball, // unused
	RuneOfFire,
	RuneOfLight,
	RuneOfNova,
	RuneOfImmolation,
	RuneOfStone,
	BigExplosion,
	HorkSpawn,
	Jester,
	OpenNest,
	OrangeFlare,
	BlueFlare,
	RedFlare,
	YellowFlare,
	BlueFlare2,
	YellowExplosion,
	RedExplosion,
	BlueExplosion,
	BlueExplosion2,
	OrangeExplosion,
	Null = -1,
	// clang-format on
};

struct SpellData {
	spell_id sName;
	uint8_t sManaCost;
	MagicType sType;
	const char *sNameText;
	int8_t sBookLvl;
	int8_t sStaffLvl;
	bool sTargeted;
	bool sTownSpell;
	int16_t sMinInt;
	_sfx_id sSFX;
	MissileID sMissiles[3];
	uint8_t sManaAdj;
	uint8_t sMinMana;
	uint8_t sStaffMin;
	uint8_t sStaffMax;
	uint32_t sBookCost;
	uint16_t sStaffCost;
};

extern const SpellData spelldata[];

} // namespace devilution
