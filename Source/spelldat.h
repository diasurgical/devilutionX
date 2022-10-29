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

enum spell_type : uint8_t {
	/* Spell Type: Skill */
	RSPLTYPE_SKILL,
	/* Spell Type: Spell */
	RSPLTYPE_SPELL,
	/* Spell Type: Scroll */
	RSPLTYPE_SCROLL,
	/* Spell Type: Staff */
	RSPLTYPE_CHARGES,
	/* Spell Type: Invalid */
	RSPLTYPE_INVALID,
};

enum spell_id : int8_t {
	/* Null */
	SPL_NULL,
	/* Firebolt */
	SPL_FIREBOLT,
	/* Healing */
	SPL_HEALING,
	/* Lightning */
	SPL_LIGHTNING,
	/* Flash */
	SPL_FLASH,
	/* Identify */
	SPL_IDENTIFY,
	/* Fire Wall */
	SPL_FIREWALL,
	/* Town Portal */
	SPL_TOWNPORTAL,
	/* Stone Curse */
	SPL_STONECURSE,
	/* Infravision */
	SPL_INFRAVISION,
	/* Phasing */
	SPL_PHASING,
	/* Mana Shield */
	SPL_MANASHIELD,
	/* Fireball */
	SPL_FIREBALL,
	/* Guardian */
	SPL_GUARDIAN,
	/* Chain Lightning */
	SPL_CHAINLIGHTNING,
	/* Flame Wave */
	SPL_FLAMEWAVE,
	/* Doom Serpents (Unused) */
	SPL_NULL_16,
	/* Blood Ritual (Unused) */
	SPL_NULL_17,
	/* Nova */
	SPL_NOVA,
	/* Invisibility (Unused)*/
	SPL_NULL_19,
	/* Inferno */
	SPL_INFERNO,
	/* Golem */
	SPL_GOLEM,
	/* Rage (Originally Blood Boil, which was unused) */
	SPL_RAGE,
	/* Teleport */
	SPL_TELEPORT,
	/* Apocalypse */
	SPL_APOCALYPSE,
	/* Etherealize (Unused) */
	SPL_NULL_25,
	/* Item Repair */
	SPL_ITEMREPAIR,
	/* Staff Recharge */
	SPL_STAFFRECHARGE,
	/* Trap Disarm */
	SPL_TRAPDISARM,
	/* Elemental */
	SPL_ELEMENTAL,
	/* Charged Bolt */
	SPL_CHARGEDBOLT,
	/* Holy Bolt */
	SPL_HOLYBOLT,
	/* Resurrect */
	SPL_RESURRECT,
	/* Telekinesis */
	SPL_TELEKINESIS,
	/* Heal Other */
	SPL_HEALOTHER,
	/* Blood Star */
	SPL_BLOODSTAR,
	/* Bone Spirit */
	SPL_BONESPIRIT,
	SPL_LASTDIABLO = SPL_BONESPIRIT,
	/* Mana */
	SPL_MANA,
	/* Magi */
	SPL_MAGI,
	/* Jester */
	SPL_JESTER,
	/* Lightning Wall */
	SPL_LIGHTNINGWALL,
	/* Immolation */
	SPL_IMMOLATION,
	/* Warp */
	SPL_WARP,
	/* Reflect */
	SPL_REFLECT,
	/* Berserk */
	SPL_BERSERK,
	/* Ring of Fire */
	SPL_RINGOFFIRE,
	/* Search */
	SPL_SEARCH,
	/* Rune of Fire */
	SPL_RUNEOFFIRE,
	/* Rune of Lightning */
	SPL_RUNEOFLIGHTNING,
	/* Greater Rune of Lightning */
	SPL_RUNEOFNOVA,
	/* Greater Rune of Fire */
	SPL_RUNEOFIMMOLATION,
	/* Rune of Stone */
	SPL_RUNEOFSTONE,

	SPL_LAST = SPL_RUNEOFSTONE,
	/* Invalid */
	SPL_INVALID = -1,
};

enum magic_type : uint8_t {
	/* Magic Element: Fire */
	STYPE_FIRE,
	/* Magic Element: Lightning */
	STYPE_LIGHTNING,
	/* Magic Element: Magic */
	STYPE_MAGIC,
};

enum missile_id : int8_t {
	/* Arrow */
	MIS_ARROW,
	/* Firebolt */
	MIS_FIREBOLT,
	/* Guardian */
	MIS_GUARDIAN,
	/* Phasing */
	MIS_PHASING,
	/* Nova segment */
	MIS_NOVA_SEGEMENT,
	/* Fire Wall segment */
	MIS_FIREWALL_SEGMENT,
	/* Fireball */
	MIS_FIREBALL,
	/* Lightning control segment */
	MIS_LIGHTNING_CTRL,
	/* Lightning segment */
	MIS_LIGHTNING_SEGMENT,
	/* Magma Ball impact */
	MIS_MAGMABALL_EXP,
	/* Town Portal */
	MIS_TOWNPORTAL,
	/* Flash segment */
	MIS_FLASH_SEGMENT,
	/* Flash segment 2 */
	MIS_FLASH_SEGMENT_2,
	/* Mana Shield */
	MIS_MANASHIELD,
	/* Flame Wave segment */
	MIS_FLAMEWAVE_SEGMENT,
	/* Chain Lightning segment */
	MIS_CHAINLIGHTNING_SEGMENT,
	/* Sentinel (Unused) */
	MIS_NULL_16,
	/* Blood Star? (Unused) */
	MIS_NULL_17,
	/* Bone Spirit? (Unused) */
	MIS_NULL_18,
	/* Metal Hit? (Unused) */
	MIS_NULL_19,
	/* Rhino (Monster charge attack) */
	MIS_RHINO,
	/* Magma Ball */
	MIS_MAGMABALL,
	/* Monster Lightning control segment */
	MIS_LIGHTNING_CTRL_MONST,
	/* Monster Lightning segment */
	MIS_LIGHTNING_SEGEMENT_MONST,
	/* Blood Star */
	MIS_BLOODSTAR,
	/* Monster Blood Star impact */
	MIS_BLOODSTAR_EXP,
	/* Teleport */
	MIS_TELEPORT,
	/* Fire Arrow */
	MIS_FIREARROW,
	/* Doom Serpent (Unused) */
	MIS_NULL_28,
	/* Fire Wall (Unused) */
	MIS_NULL_29,
	/* Stone Curse */
	MIS_STONECURSE,
	/* Null (Unused) */
	MIS_NULL_31,
	/* Invisibility (Unused) */
	MIS_NULL_32,
	/* Golem */
	MIS_GOLEM,
	/* Etherealize (Unused) */
	MIS_NULL_34,
	/* Blood Burst? (Unused) */
	MIS_NULL_35,
	/* Apocalypse impact */
	MIS_APOCALYPSE_EXP,
	/* Healing */
	MIS_HEALING,
	/* Fire Wall cast */
	MIS_FIREWALL_CAST,
	/* Infravision */
	MIS_INFRAVISION,
	/* Identify */
	MIS_IDENTIFY,
	/* Flame Wave */
	MIS_FLAMEWAVE,
	/* Nova */
	MIS_NOVA,
	/* Rage (Originally Blood Boil, which was unused) */
	MIS_RAGE,
	/* Apocalypse cast */
	MIS_APOCALYPSE,
	/* Item Repair */
	MIS_ITEMREPAIR,
	/* Staff Recharged */
	MIS_STAFFRECHARGE,
	/* Trap Disarm */
	MIS_TRAPDISARM,
	/* Inferno */
	MIS_INFERNO,
	/* Inferno cast */
	MIS_INFERNO_CAST,
	/* Unraveler monster missile (Unused) */
	MIS_NULL_50,
	/* Unraveler monster missile (Unused) */
	MIS_NULL_51,
	/* Charged Bolt cast */
	MIS_CHARGEDBOLT,
	/* Holy Bolt */
	MIS_HOLYBOLT,
	/* Resurrect cast */
	MIS_RESURRECT,
	/* Telekinesis */
	MIS_TELEKINESIS,
	/* Lightning Arrow */
	MIS_LIGHTNINGARROW,
	/* Monster Acid */
	MIS_ACID,
	/* Monster Acid impact */
	MIS_ACID_EXP,
	/* Monster Acid puddle */
	MIS_ACID_PUDDLE,
	/* Heal Other */
	MIS_HEALOTHER,
	/* Elemental */
	MIS_ELEMENTAL,
	/* Resurrect */
	MIS_RESURRECT_BEAM,
	/* Bone Spirit */
	MIS_BONESPIRIT,
	/* Melee element damage impact */
	MIS_WEAPON_EXP,
	/* Red Portal (Lazarus level) */
	MIS_REDPORTAL,
	/* Diablo Apocalypse impact */
	MIS_APOCALYPSE_EXP_MONST,
	/* Diablo Apocalypse cast */
	MIS_APOCALYPSE_MONST,
	/* Mana */
	MIS_MANA,
	/* Magi */
	MIS_MAGI,
	/* Lightning Wall segment */
	MIS_LIGHTNINGWALL_SEGMENT,
	/* Lightning Wall cast*/
	MIS_LIGHTNINGWALL,
	/* Immolation cast */
	MIS_IMMOLATION_CAST,
	/* Spectral Arrow */
	MIS_SPECTRALARROW,
	/* Immolation projectile */
	MIS_IMMOLATION,
	/* Lightning Arrow */
	MIS_LIGHTNING_BOW,
	/* Charged Bolt Arrow */
	MIS_CHARGEDBOLT_BOW,
	/* Holy Bolt Arrow */
	MIS_HOLYBOLT_BOW,
	/* Warp */
	MIS_WARP,
	/* Reflect */
	MIS_REFLECT,
	/* Berserk */
	MIS_BERSERK,
	/* Ring of Fire cast */
	MIS_RINGOFFIRE,
	/* Trap (Take player potions) */
	MIS_TRAP_POTIONS,
	/* Trap (Take player mana) */
	MIS_TRAP_MANA,
	/* Ring of Lightning cast (Unused) */
	MIS_NULL_84,
	/* Search */
	MIS_SEARCH,
	/* Flash front (Unused) */
	MIS_NULL_86,
	/* Flash back (Unused) */
	MIS_NULL_87,
	/* Immolation 2 (Unused) */
	MIS_NULL_88,
	/* Rune of Fire */
	MIS_RUNEOFFIRE,
	/* Rune of Lightning */
	MIS_RUNEOFLIGHTNING,
	/* Greater Rune of Lightning */
	MIS_RUNEOFNOVA,
	/* Greater Rune of Fire */
	MIS_RUNEOFIMMOLATION,
	/* Rune of Stone */
	MIS_RUNEOFSTONE,
	/* Rune impact */
	MIS_RUNEBOMB_EXP,
	/* Hork Demon missile */
	MIS_HORKDEMON,
	/* Jester cast */
	MIS_JESTER,
	/* Hive Entrance impact*/
	MIS_RUNEBOMB_EXP_2,
	/* Lich projectile */
	MIS_LICH,
	/* Psychorb projectile */
	MIS_PSYCHORB,
	/* Necromorb projectile */
	MIS_NECROMORB,
	/* Arch Lich projectile */
	MIS_ARCHLICH,
	/* Bone Demon projectile */
	MIS_BONEDEMON,
	/* Arch Lich projectile impact */
	MIS_ARCHLICH_EXP,
	/* Necromorb projectile impact */
	MIS_NECROMORB_EXP,
	/* Psychorb projectile impact */
	MIS_PSYCHORB_EXP,
	/* Bone Demon projectile impact */
	MIS_BONEDEMON_EXP,
	/* Lich projectile impact */
	MIS_LICH_EXP,
	/* Null */
	MIS_NULL = -1,
};

struct SpellData {
	spell_id sName;
	uint8_t sManaCost;
	magic_type sType;
	const char *sNameText;
	int sBookLvl;
	int sStaffLvl;
	bool sTargeted;
	bool sTownSpell;
	int sMinInt;
	_sfx_id sSFX;
	missile_id sMissiles[3];
	uint8_t sManaAdj;
	uint8_t sMinMana;
	int sStaffMin;
	int sStaffMax;
	int sBookCost;
	int sStaffCost;
};

extern const SpellData spelldata[];

} // namespace devilution
