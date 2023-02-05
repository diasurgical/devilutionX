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

enum class SpellID : int8_t {
	Null,
	FIRST = Null,
	Firebolt,
	Healing,
	Lightning,
	Flash,
	Identify,
	FireWall,
	TownPortal,
	StoneCurse,
	Infravision,
	Phasing,
	ManaShield,
	Fireball,
	Guardian,
	ChainLightning,
	FlameWave,
	DoomSerpents,
	BloodRitual,
	Nova,
	Invisibility,
	Inferno,
	Golem,
	Rage,
	Teleport,
	Apocalypse,
	Etherealize,
	ItemRepair,
	StaffRecharge,
	TrapDisarm,
	Elemental,
	ChargedBolt,
	HolyBolt,
	Resurrect,
	Telekinesis,
	HealOther,
	BloodStar,
	BoneSpirit,
	LastDiablo = BoneSpirit,
	Mana,
	Magi,
	Jester,
	LightningWall,
	Immolation,
	Warp,
	Reflect,
	Berserk,
	RingOfFire,
	Search,
	RuneOfFire,
	RuneOfLight,
	RuneOfNova,
	RuneOfImmolation,
	RuneOfStone,

	LAST = RuneOfStone,
	Invalid = -1,
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
	SpellID sName;
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

extern const SpellData SpellsData[];

inline const SpellData &GetSpellData(SpellID spellId)
{
	return SpellsData[static_cast<std::underlying_type<SpellID>::type>(spellId)];
}

} // namespace devilution
