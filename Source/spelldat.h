/**
 * @file spelldat.h
 *
 * Interface of all spell data.
 */
#pragma once

#include <stdint.h>
#include <string_view>

#include "effects.h"

namespace devilution {

#define MAX_SPELLS 52

enum spell_type : uint8_t {
	RSPLTYPE_SKILL,
	RSPLTYPE_SPELL,
	RSPLTYPE_SCROLL,
	RSPLTYPE_CHARGES,
	RSPLTYPE_INVALID,
};

constexpr std::string_view toString(spell_type value)
{
	switch(value) {
	case RSPLTYPE_SKILL:
		return "Skill";
	case RSPLTYPE_SPELL:
		return "Spell";
	case RSPLTYPE_SCROLL:
		return "Scroll";
	case RSPLTYPE_CHARGES:
		return "Charges";
	case RSPLTYPE_INVALID:
		return "Invalid";
	}
}

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
	SPL_INVALID = -1,
};

constexpr std::string_view toString(spell_id value)
{
	switch(value) {
	case SPL_NULL:
		return "Null";
	case SPL_FIREBOLT:
		return "Firebolt";
	case SPL_HEAL:
		return "Heal";
	case SPL_LIGHTNING:
		return "Lightning";
	case SPL_FLASH:
		return "Flash";
	case SPL_IDENTIFY:
		return "Identify";
	case SPL_FIREWALL:
		return "Firewall";
	case SPL_TOWN:
		return "Town";
	case SPL_STONE:
		return "Stone";
	case SPL_INFRA:
		return "Infra";
	case SPL_RNDTELEPORT:
		return "Rndteleport";
	case SPL_MANASHIELD:
		return "Manashield";
	case SPL_FIREBALL:
		return "Fireball";
	case SPL_GUARDIAN:
		return "Guardian";
	case SPL_CHAIN:
		return "Chain";
	case SPL_WAVE:
		return "Wave";
	case SPL_DOOMSERP:
		return "Doomserp";
	case SPL_BLODRIT:
		return "Blodrit";
	case SPL_NOVA:
		return "Nova";
	case SPL_INVISIBIL:
		return "Invisibil";
	case SPL_FLAME:
		return "Flame";
	case SPL_GOLEM:
		return "Golem";
	case SPL_BLODBOIL:
		return "Blodboil";
	case SPL_TELEPORT:
		return "Teleport";
	case SPL_APOCA:
		return "Apoca";
	case SPL_ETHEREALIZE:
		return "Etherealize";
	case SPL_REPAIR:
		return "Repair";
	case SPL_RECHARGE:
		return "Recharge";
	case SPL_DISARM:
		return "Disarm";
	case SPL_ELEMENT:
		return "Element";
	case SPL_CBOLT:
		return "Cbolt";
	case SPL_HBOLT:
		return "Hbolt";
	case SPL_RESURRECT:
		return "Resurrect";
	case SPL_TELEKINESIS:
		return "Telekinesis";
	case SPL_HEALOTHER:
		return "Healother";
	case SPL_FLARE:
		return "Flare";
	case SPL_BONESPIRIT:
		return "Bonespirit";
	case SPL_MANA:
		return "Mana";
	case SPL_MAGI:
		return "Magi";
	case SPL_JESTER:
		return "Jester";
	case SPL_LIGHTWALL:
		return "Lightwall";
	case SPL_IMMOLAT:
		return "Immolat";
	case SPL_WARP:
		return "Warp";
	case SPL_REFLECT:
		return "Reflect";
	case SPL_BERSERK:
		return "Berserk";
	case SPL_FIRERING:
		return "Firering";
	case SPL_SEARCH:
		return "Search";
	case SPL_RUNEFIRE:
		return "Runefire";
	case SPL_RUNELIGHT:
		return "Runelight";
	case SPL_RUNENOVA:
		return "Runenova";
	case SPL_RUNEIMMOLAT:
		return "Runeimmolat";
	case SPL_RUNESTONE:
		return "Runestone";
	case SPL_INVALID:
		return "Invalid";
	}
}

enum magic_type : uint8_t {
	STYPE_FIRE,
	STYPE_LIGHTNING,
	STYPE_MAGIC,
};

constexpr std::string_view toString(magic_type value)
{
	switch(value) {
	case STYPE_FIRE:
		return "Fire";
	case STYPE_LIGHTNING:
		return "Lightning";
	case STYPE_MAGIC:
		return "Magic";
	}
}

enum missile_id : int8_t {
	MIS_ARROW,
	MIS_FIREBOLT,
	MIS_GUARDIAN,
	MIS_RNDTELEPORT,
	MIS_LIGHTBALL,
	MIS_FIREWALL,
	MIS_FIREBALL,
	MIS_LIGHTCTRL,
	MIS_LIGHTNING,
	MIS_MISEXP,
	MIS_TOWN,
	MIS_FLASH,
	MIS_FLASH2,
	MIS_MANASHIELD,
	MIS_FIREMOVE,
	MIS_CHAIN,
	MIS_SENTINAL,
	MIS_BLODSTAR,
	MIS_BONE,
	MIS_METLHIT,
	MIS_RHINO,
	MIS_MAGMABALL,
	MIS_LIGHTCTRL2,
	MIS_LIGHTNING2,
	MIS_FLARE,
	MIS_MISEXP2,
	MIS_TELEPORT,
	MIS_FARROW,
	MIS_DOOMSERP,
	MIS_FIREWALLA,
	MIS_STONE,
	MIS_NULL_1F,
	MIS_INVISIBL,
	MIS_GOLEM,
	MIS_ETHEREALIZE,
	MIS_BLODBUR,
	MIS_BOOM,
	MIS_HEAL,
	MIS_FIREWALLC,
	MIS_INFRA,
	MIS_IDENTIFY,
	MIS_WAVE,
	MIS_NOVA,
	MIS_BLODBOIL,
	MIS_APOCA,
	MIS_REPAIR,
	MIS_RECHARGE,
	MIS_DISARM,
	MIS_FLAME,
	MIS_FLAMEC,
	MIS_FIREMAN,
	MIS_KRULL,
	MIS_CBOLT,
	MIS_HBOLT,
	MIS_RESURRECT,
	MIS_TELEKINESIS,
	MIS_LARROW,
	MIS_ACID,
	MIS_MISEXP3,
	MIS_ACIDPUD,
	MIS_HEALOTHER,
	MIS_ELEMENT,
	MIS_RESURRECTBEAM,
	MIS_BONESPIRIT,
	MIS_WEAPEXP,
	MIS_RPORTAL,
	MIS_BOOM2,
	MIS_DIABAPOCA,
	MIS_MANA,
	MIS_MAGI,
	MIS_LIGHTWALL,
	MIS_LIGHTNINGWALL,
	MIS_IMMOLATION,
	MIS_SPECARROW,
	MIS_FIRENOVA,
	MIS_LIGHTARROW,
	MIS_CBOLTARROW,
	MIS_HBOLTARROW,
	MIS_WARP,
	MIS_REFLECT,
	MIS_BERSERK,
	MIS_FIRERING,
	MIS_STEALPOTS,
	MIS_MANATRAP,
	MIS_LIGHTRING,
	MIS_SEARCH,
	MIS_FLASHFR,
	MIS_FLASHBK,
	MIS_IMMOLATION2,
	MIS_RUNEFIRE,
	MIS_RUNELIGHT,
	MIS_RUNENOVA,
	MIS_RUNEIMMOLAT,
	MIS_RUNESTONE,
	MIS_HIVEEXP,
	MIS_HORKDMN,
	MIS_JESTER,
	MIS_HIVEEXP2,
	MIS_LICH,
	MIS_PSYCHORB,
	MIS_NECROMORB,
	MIS_ARCHLICH,
	MIS_BONEDEMON,
	MIS_EXYEL2,
	MIS_EXRED3,
	MIS_EXBL2,
	MIS_EXBL3,
	MIS_EXORA1,
	MIS_NULL = -1,
};

constexpr std::string_view toString(missile_id value)
{
	switch(value) {
	case MIS_ARROW:
		return "Arrow";
	case MIS_FIREBOLT:
		return "Firebolt";
	case MIS_GUARDIAN:
		return "Guardian";
	case MIS_RNDTELEPORT:
		return "Rndteleport";
	case MIS_LIGHTBALL:
		return "Lightball";
	case MIS_FIREWALL:
		return "Firewall";
	case MIS_FIREBALL:
		return "Fireball";
	case MIS_LIGHTCTRL:
		return "Lightctrl";
	case MIS_LIGHTNING:
		return "Lightning";
	case MIS_MISEXP:
		return "Misexp";
	case MIS_TOWN:
		return "Town";
	case MIS_FLASH:
		return "Flash";
	case MIS_FLASH2:
		return "Flash2";
	case MIS_MANASHIELD:
		return "Manashield";
	case MIS_FIREMOVE:
		return "Firemove";
	case MIS_CHAIN:
		return "Chain";
	case MIS_SENTINAL:
		return "Sentinal";
	case MIS_BLODSTAR:
		return "Blodstar";
	case MIS_BONE:
		return "Bone";
	case MIS_METLHIT:
		return "Metlhit";
	case MIS_RHINO:
		return "Rhino";
	case MIS_MAGMABALL:
		return "Magmaball";
	case MIS_LIGHTCTRL2:
		return "Lightctrl2";
	case MIS_LIGHTNING2:
		return "Lightning2";
	case MIS_FLARE:
		return "Flare";
	case MIS_MISEXP2:
		return "Misexp2";
	case MIS_TELEPORT:
		return "Teleport";
	case MIS_FARROW:
		return "Farrow";
	case MIS_DOOMSERP:
		return "Doomserp";
	case MIS_FIREWALLA:
		return "Firewalla";
	case MIS_STONE:
		return "Stone";
	case MIS_NULL_1F:
		return "Null 1f";
	case MIS_INVISIBL:
		return "Invisibl";
	case MIS_GOLEM:
		return "Golem";
	case MIS_ETHEREALIZE:
		return "Etherealize";
	case MIS_BLODBUR:
		return "Blodbur";
	case MIS_BOOM:
		return "Boom";
	case MIS_HEAL:
		return "Heal";
	case MIS_FIREWALLC:
		return "Firewallc";
	case MIS_INFRA:
		return "Infra";
	case MIS_IDENTIFY:
		return "Identify";
	case MIS_WAVE:
		return "Wave";
	case MIS_NOVA:
		return "Nova";
	case MIS_BLODBOIL:
		return "Blodboil";
	case MIS_APOCA:
		return "Apoca";
	case MIS_REPAIR:
		return "Repair";
	case MIS_RECHARGE:
		return "Recharge";
	case MIS_DISARM:
		return "Disarm";
	case MIS_FLAME:
		return "Flame";
	case MIS_FLAMEC:
		return "Flamec";
	case MIS_FIREMAN:
		return "Fireman";
	case MIS_KRULL:
		return "Krull";
	case MIS_CBOLT:
		return "Cbolt";
	case MIS_HBOLT:
		return "Hbolt";
	case MIS_RESURRECT:
		return "Resurrect";
	case MIS_TELEKINESIS:
		return "Telekinesis";
	case MIS_LARROW:
		return "Larrow";
	case MIS_ACID:
		return "Acid";
	case MIS_MISEXP3:
		return "Misexp3";
	case MIS_ACIDPUD:
		return "Acidpud";
	case MIS_HEALOTHER:
		return "Healother";
	case MIS_ELEMENT:
		return "Element";
	case MIS_RESURRECTBEAM:
		return "Resurrectbeam";
	case MIS_BONESPIRIT:
		return "Bonespirit";
	case MIS_WEAPEXP:
		return "Weapexp";
	case MIS_RPORTAL:
		return "Rportal";
	case MIS_BOOM2:
		return "Boom2";
	case MIS_DIABAPOCA:
		return "Diabapoca";
	case MIS_MANA:
		return "Mana";
	case MIS_MAGI:
		return "Magi";
	case MIS_LIGHTWALL:
		return "Lightwall";
	case MIS_LIGHTNINGWALL:
		return "Lightningwall";
	case MIS_IMMOLATION:
		return "Immolation";
	case MIS_SPECARROW:
		return "Specarrow";
	case MIS_FIRENOVA:
		return "Firenova";
	case MIS_LIGHTARROW:
		return "Lightarrow";
	case MIS_CBOLTARROW:
		return "Cboltarrow";
	case MIS_HBOLTARROW:
		return "Hboltarrow";
	case MIS_WARP:
		return "Warp";
	case MIS_REFLECT:
		return "Reflect";
	case MIS_BERSERK:
		return "Berserk";
	case MIS_FIRERING:
		return "Firering";
	case MIS_STEALPOTS:
		return "Stealpots";
	case MIS_MANATRAP:
		return "Manatrap";
	case MIS_LIGHTRING:
		return "Lightring";
	case MIS_SEARCH:
		return "Search";
	case MIS_FLASHFR:
		return "Flashfr";
	case MIS_FLASHBK:
		return "Flashbk";
	case MIS_IMMOLATION2:
		return "Immolation2";
	case MIS_RUNEFIRE:
		return "Runefire";
	case MIS_RUNELIGHT:
		return "Runelight";
	case MIS_RUNENOVA:
		return "Runenova";
	case MIS_RUNEIMMOLAT:
		return "Runeimmolat";
	case MIS_RUNESTONE:
		return "Runestone";
	case MIS_HIVEEXP:
		return "Hiveexp";
	case MIS_HORKDMN:
		return "Horkdmn";
	case MIS_JESTER:
		return "Jester";
	case MIS_HIVEEXP2:
		return "Hiveexp2";
	case MIS_LICH:
		return "Lich";
	case MIS_PSYCHORB:
		return "Psychorb";
	case MIS_NECROMORB:
		return "Necromorb";
	case MIS_ARCHLICH:
		return "Archlich";
	case MIS_BONEDEMON:
		return "Bonedemon";
	case MIS_EXYEL2:
		return "Exyel2";
	case MIS_EXRED3:
		return "Exred3";
	case MIS_EXBL2:
		return "Exbl2";
	case MIS_EXBL3:
		return "Exbl3";
	case MIS_EXORA1:
		return "Exora1";
	case MIS_NULL:
		return "Null";
	}
}

struct SpellData {
	spell_id sName;
	Uint8 sManaCost;
	magic_type sType;
	const char *sNameText;
	const char *sSkillText;
	Sint32 sBookLvl;
	Sint32 sStaffLvl;
	bool sTargeted;
	bool sTownSpell;
	Sint32 sMinInt;
	_sfx_id sSFX;
	missile_id sMissiles[3];
	Uint8 sManaAdj;
	Uint8 sMinMana;
	Sint32 sStaffMin;
	Sint32 sStaffMax;
	Sint32 sBookCost;
	Sint32 sStaffCost;
};

extern SpellData spelldata[];

} // namespace devilution
