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
	SPL_HEAL,
	/* Lightning */
	SPL_LIGHTNING,
	/* Flash */
	SPL_FLASH,
	/* Identify */
	SPL_IDENTIFY,
	/* Fire Wall */
	SPL_FIREWALL,
	/* Town Portal */
	SPL_TOWN,
	/* Stone Curse */
	SPL_STONE,
	/* Infravision */
	SPL_INFRA,
	/* Phasing */
	SPL_RNDTELEPORT,
	/* Mana Shield */
	SPL_MANASHIELD,
	/* Fireball */
	SPL_FIREBALL,
	/* Guardian */
	SPL_GUARDIAN,
	/* Chain Lightning */
	SPL_CHAIN,
	/* Flame Wave */
	SPL_WAVE,
	/* Doom Serpents (Unused) */
	SPL_DOOMSERP,
	/* Blood Ritual (Unused) */
	SPL_BLODRIT,
	/* Nova */
	SPL_NOVA,
	/* Invisibility (Unused)*/
	SPL_INVISIBIL,
	/* Inferno */
	SPL_FLAME,
	/* Golem */
	SPL_GOLEM,
	/* Rage (Originally Blood Boil, which was unused) */
	SPL_BLODBOIL,
	/* Teleport */
	SPL_TELEPORT,
	/* Apocalypse */
	SPL_APOCA,
	/* Etherealize (Unused) */
	SPL_ETHEREALIZE,
	/* Item Repair */
	SPL_REPAIR,
	/* Staff Recharge */
	SPL_RECHARGE,
	/* Trap Disarm */
	SPL_DISARM,
	/* Elemental */
	SPL_ELEMENT,
	/* Charged Bolt */
	SPL_CBOLT,
	/* Holy Bolt */
	SPL_HBOLT,
	/* Resurrect */
	SPL_RESURRECT,
	/* Telekinesis */
	SPL_TELEKINESIS,
	/* Heal Other */
	SPL_HEALOTHER,
	/* Blood Star */
	SPL_FLARE,
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
	SPL_LIGHTWALL,
	/* Immolation */
	SPL_IMMOLAT,
	/* Warp */
	SPL_WARP,
	/* Reflect */
	SPL_REFLECT,
	/* Berserk */
	SPL_BERSERK,
	/* Ring of Fire */
	SPL_FIRERING,
	/* Search */
	SPL_SEARCH,
	/* Rune of Fire */
	SPL_RUNEFIRE,
	/* Rune of Lightning */
	SPL_RUNELIGHT,
	/* Greater Rune of Lightning */
	SPL_RUNENOVA,
	/* Greater Rune of Fire */
	SPL_RUNEIMMOLAT,
	/* Rune of Stone */
	SPL_RUNESTONE,

	SPL_LAST = SPL_RUNESTONE,
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
	MIS_RNDTELEPORT,
	/* Nova segment */
	MIS_LIGHTBALL,
	/* Fire Wall segment */
	MIS_FIREWALL,
	/* Fireball */
	MIS_FIREBALL,
	/* Lightning control segment */
	MIS_LIGHTCTRL,
	/* Lightning segment */
	MIS_LIGHTNING,
	/* Magma Ball impact */
	MIS_MISEXP,
	/* Town Portal */
	MIS_TOWN,
	/* Flash segment */
	MIS_FLASH,
	/* Flash segment 2 */
	MIS_FLASH2,
	/* Mana Shield */
	MIS_MANASHIELD,
	/* Flame Wave segment */
	MIS_FIREMOVE,
	/* Chain Lightning segment */
	MIS_CHAIN,
	/* Sentinel (Unused) */
	MIS_SENTINAL,
	/* Blood Star? (Unused) */
	MIS_BLODSTAR,
	/* Bone Spirit? (Unused) */
	MIS_BONE,
	/* Metal Hit? (Unused) */
	MIS_METLHIT,
	/* Rhino (Monster charge attack) */
	MIS_RHINO,
	/* Magma Ball */
	MIS_MAGMABALL,
	/* Monster Lightning control segment */
	MIS_LIGHTCTRL2,
	/* Monster Lightning segment */
	MIS_LIGHTNING2,
	/* Blood Star */
	MIS_FLARE,
	/* Monster Blood Star impact */
	MIS_MISEXP2,
	/* Teleport */
	MIS_TELEPORT,
	/* Fire Arrow */
	MIS_FARROW,
	/* Doom Serpent (Unused) */
	MIS_DOOMSERP,
	/* Fire Wall (Unused) */
	MIS_FIREWALLA,
	/* Stone Curse */
	MIS_STONE,
	/* Null (Unused) */
	MIS_NULL_1F,
	/* Invisibility (Unused) */
	MIS_INVISIBL,
	/* Golem */
	MIS_GOLEM,
	/* Etherealize (Unused) */
	MIS_ETHEREALIZE,
	/* Blood Burst? (Unused) */
	MIS_BLODBUR,
	/* Apocalypse impact */
	MIS_BOOM,
	/* Healing */
	MIS_HEAL,
	/* Fire Wall cast */
	MIS_FIREWALLC,
	/* Infravision */
	MIS_INFRA,
	/* Identify */
	MIS_IDENTIFY,
	/* Flame Wave */
	MIS_WAVE,
	/* Nova */
	MIS_NOVA,
	/* Rage (Originally Blood Boil, which was unused) */
	MIS_BLODBOIL,
	/* Apocalypse cast */
	MIS_APOCA,
	/* Item Repair */
	MIS_REPAIR,
	/* Staff Recharged */
	MIS_RECHARGE,
	/* Trap Disarm */
	MIS_DISARM,
	/* Inferno */
	MIS_FLAME,
	/* Inferno cast */
	MIS_FLAMEC,
	/* Unraveler monster missile (Unused) */
	MIS_FIREMAN,
	/* Unraveler monster missile (Unused) */
	MIS_KRULL,
	/* Charged Bolt cast */
	MIS_CBOLT,
	/* Holy Bolt */
	MIS_HBOLT,
	/* Resurrect cast */
	MIS_RESURRECT,
	/* Telekinesis */
	MIS_TELEKINESIS,
	/* Lightning Arrow */
	MIS_LARROW,
	/* Monster Acid */
	MIS_ACID,
	/* Monster Acid impact */
	MIS_MISEXP3,
	/* Monster Acid puddle */
	MIS_ACIDPUD,
	/* Heal Other */
	MIS_HEALOTHER,
	/* Elemental */
	MIS_ELEMENT,
	/* Resurrect */
	MIS_RESURRECTBEAM,
	/* Bone Spirit */
	MIS_BONESPIRIT,
	/* Melee element damage impact */
	MIS_WEAPEXP,
	/* Red Portal (Lazarus level) */
	MIS_RPORTAL,
	/* Diablo Apocalypse impact */
	MIS_BOOM2,
	/* Diablo Apocalypse cast */
	MIS_DIABAPOCA,
	/* Mana */
	MIS_MANA,
	/* Magi */
	MIS_MAGI,
	/* Lightning Wall segment */
	MIS_LIGHTWALL,
	/* Lightning Wall cast*/
	MIS_LIGHTNINGWALL,
	/* Immolation cast */
	MIS_IMMOLATION,
	/* Spectral Arrow */
	MIS_SPECARROW,
	/* Immolation projectile */
	MIS_FIRENOVA,
	/* Lightning Arrow */
	MIS_LIGHTARROW,
	/* Charged Bolt Arrow */
	MIS_CBOLTARROW,
	/* Holy Bolt Arrow */
	MIS_HBOLTARROW,
	/* Warp */
	MIS_WARP,
	/* Reflect */
	MIS_REFLECT,
	/* Berserk */
	MIS_BERSERK,
	/* Ring of Fire cast */
	MIS_FIRERING,
	/* Trap (Take player potions) */
	MIS_STEALPOTS,
	/* Trap (Take player mana) */
	MIS_MANATRAP,
	/* Ring of Lightning cast (Unused) */
	MIS_LIGHTRING,
	/* Search */
	MIS_SEARCH,
	/* Flash front (Unused) */
	MIS_FLASHFR,
	/* Flash back (Unused) */
	MIS_FLASHBK,
	/* Immolation 2 (Unused) */
	MIS_IMMOLATION2,
	/* Rune of Fire */
	MIS_RUNEFIRE,
	/* Rune of Lightning */
	MIS_RUNELIGHT,
	/* Greater Rune of Lightning */
	MIS_RUNENOVA,
	/* Greater Rune of Fire */
	MIS_RUNEIMMOLAT,
	/* Rune of Stone */
	MIS_RUNESTONE,
	/* Rune impact */
	MIS_HIVEEXP,
	/* Hork Demon missile */
	MIS_HORKDMN,
	/* Jester cast */
	MIS_JESTER,
	/* Hive Entrance impact*/
	MIS_HIVEEXP2,
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
	MIS_EXYEL2,
	/* Necromorb projectile impact */
	MIS_EXRED3,
	/* Psychorb projectile impact */
	MIS_EXBL2,
	/* Bone Demon projectile impact */
	MIS_EXBL3,
	/* Lich projectile impact */
	MIS_EXORA1,
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
