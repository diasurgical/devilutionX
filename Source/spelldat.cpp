/**
 * @file spelldat.cpp
 *
 * Implementation of all spell data.
 */
#include "spelldat.h"

#include <string_view>

#include <expected.hpp>

#include "utils/language.h"

namespace devilution {

namespace {
const auto Fire = SpellDataFlags::Fire;
const auto Lightning = SpellDataFlags::Lightning;
const auto Magic = SpellDataFlags::Magic;
const auto Targeted = SpellDataFlags::Targeted;
const auto AllowedInTown = SpellDataFlags::AllowedInTown;
} // namespace

/** Data related to each spell ID. */
const SpellData SpellsData[] = {
	// clang-format off
// id                           sNameText,                         sSFX,                  bookCost10,  staffCost10,  sManaCost, flags,                  sBookLvl,  sStaffLvl,  minInt, { sMissiles[2]                                         }   sManaAdj,  sMinMana,  sStaffMin,  sStaffMax
/*SpellID::Null*/             { nullptr,                           SfxID::None,                    0,            0,          0, Fire,                          0,          0,       0, { MissileID::Null,                 MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::Firebolt*/         { P_("spell", "Firebolt"),           SfxID::CastFire,              100,            5,          6, Fire | Targeted,               1,          1,      15, { MissileID::Firebolt,             MissileID::Null,    },         1,         3,         40,         80 },
/*SpellID::Healing*/          { P_("spell", "Healing"),            SfxID::CastHealing,           100,            5,          5, Magic | AllowedInTown,         1,          1,      17, { MissileID::Healing,              MissileID::Null,    },         3,         1,         20,         40 },
/*SpellID::Lightning*/        { P_("spell", "Lightning"),          SfxID::CastLightning,         300,           15,         10, Lightning | Targeted,          4,          3,      20, { MissileID::LightningControl,     MissileID::Null,    },         1,         6,         20,         60 },
/*SpellID::Flash*/            { P_("spell", "Flash"),              SfxID::CastLightning,         750,           50,         30, Lightning,                     5,          4,      33, { MissileID::FlashBottom,          MissileID::FlashTop },         2,        16,         20,         40 },
/*SpellID::Identify*/         { P_("spell", "Identify"),           SfxID::CastSkill,               0,           10,         13, Magic | AllowedInTown,        -1,         -1,      23, { MissileID::Identify,             MissileID::Null,    },         2,         1,          8,         12 },
/*SpellID::FireWall*/         { P_("spell", "Fire Wall"),          SfxID::CastFire,              600,           40,         28, Fire | Targeted,               3,          2,      27, { MissileID::FireWallControl,      MissileID::Null,    },         2,        16,          8,         16 },
/*SpellID::TownPortal*/       { P_("spell", "Town Portal"),        SfxID::CastSkill,             300,           20,         35, Magic | Targeted,              3,          3,      20, { MissileID::TownPortal,           MissileID::Null,    },         3,        18,          8,         12 },
/*SpellID::StoneCurse*/       { P_("spell", "Stone Curse"),        SfxID::CastFire,             1200,           80,         60, Magic | Targeted,              6,          5,      51, { MissileID::StoneCurse,           MissileID::Null,    },         3,        40,          8,         16 },
/*SpellID::Infravision*/      { P_("spell", "Infravision"),        SfxID::CastHealing,             0,           60,         40, Magic,                        -1,         -1,      36, { MissileID::Infravision,          MissileID::Null,    },         5,        20,          0,          0 },
/*SpellID::Phasing*/          { P_("spell", "Phasing"),            SfxID::CastFire,              350,           20,         12, Magic,                         7,          6,      39, { MissileID::Phasing,              MissileID::Null,    },         2,         4,         40,         80 },
/*SpellID::ManaShield*/       { P_("spell", "Mana Shield"),        SfxID::CastFire,             1600,          120,         33, Magic,                         6,          5,      25, { MissileID::ManaShield,           MissileID::Null,    },         0,        33,          4,         10 },
/*SpellID::Fireball*/         { P_("spell", "Fireball"),           SfxID::CastFire,              800,           30,         16, Fire | Targeted,               8,          7,      48, { MissileID::Fireball,             MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::Guardian*/         { P_("spell", "Guardian"),           SfxID::CastFire,             1400,           95,         50, Fire | Targeted,               9,          8,      61, { MissileID::Guardian,             MissileID::Null,    },         2,        30,         16,         32 },
/*SpellID::ChainLightning*/   { P_("spell", "Chain Lightning"),    SfxID::CastFire,             1100,           75,         30, Lightning,                     8,          7,      54, { MissileID::ChainLightning,       MissileID::Null,    },         1,        18,         20,         60 },
/*SpellID::FlameWave*/        { P_("spell", "Flame Wave"),         SfxID::CastFire,             1000,           65,         35, Fire | Targeted,               9,          8,      54, { MissileID::FlameWaveControl,     MissileID::Null,    },         3,        20,         20,         40 },
/*SpellID::DoomSerpents*/     { P_("spell", "Doom Serpents"),      SfxID::CastFire,                0,            0,          0, Lightning,                    -1,         -1,       0, { MissileID::Null,                 MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::BloodRitual*/      { P_("spell", "Blood Ritual"),       SfxID::CastFire,                0,            0,          0, Magic,                        -1,         -1,       0, { MissileID::Null,                 MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::Nova*/             { P_("spell", "Nova"),               SfxID::CastLightning,        2100,          130,         60, Magic,                        14,         10,      87, { MissileID::Nova,                 MissileID::Null,    },         3,        35,         16,         32 },
/*SpellID::Invisibility*/     { P_("spell", "Invisibility"),       SfxID::CastFire,                0,            0,          0, Magic,                        -1,         -1,       0, { MissileID::Null,                 MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::Inferno*/          { P_("spell", "Inferno"),            SfxID::CastFire,              200,           10,         11, Fire | Targeted,               3,          2,      20, { MissileID::InfernoControl,       MissileID::Null,    },         1,         6,         20,         40 },
/*SpellID::Golem*/            { P_("spell", "Golem"),              SfxID::CastFire,             1800,          110,        100, Fire | Targeted,              11,          9,      81, { MissileID::Golem,                MissileID::Null,    },         6,        60,         16,         32 },
/*SpellID::Rage*/             { P_("spell", "Rage"),               SfxID::CastHealing,             0,            0,         15, Magic,                        -1,         -1,       0, { MissileID::Rage,                 MissileID::Null,    },         1,         1,          0,          0 },
/*SpellID::Teleport*/         { P_("spell", "Teleport"),           SfxID::CastSkill,            2000,          125,         35, Magic | Targeted,             14,         12,     105, { MissileID::Teleport,             MissileID::Null,    },         3,        15,         16,         32 },
/*SpellID::Apocalypse*/       { P_("spell", "Apocalypse"),         SfxID::CastFire,             3000,          200,        150, Fire,                         19,         15,     149, { MissileID::Apocalypse,           MissileID::Null,    },         6,        90,          8,         12 },
/*SpellID::Etherealize*/      { P_("spell", "Etherealize"),        SfxID::CastFire,             2600,          160,        100, Magic,                        -1,         -1,      93, { MissileID::Etherealize,          MissileID::Null,    },         0,       100,          2,          6 },
/*SpellID::ItemRepair*/       { P_("spell", "Item Repair"),        SfxID::CastSkill,               0,            0,          0, Magic | AllowedInTown,        -1,         -1,     255, { MissileID::ItemRepair,           MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::StaffRecharge*/    { P_("spell", "Staff Recharge"),     SfxID::CastSkill,               0,            0,          0, Magic | AllowedInTown,        -1,         -1,     255, { MissileID::StaffRecharge,        MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::TrapDisarm*/       { P_("spell", "Trap Disarm"),        SfxID::CastSkill,               0,            0,          0, Magic,                        -1,         -1,     255, { MissileID::TrapDisarm,           MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::Elemental*/        { P_("spell", "Elemental"),          SfxID::CastFire,             1050,           70,         35, Fire,                          8,          6,      68, { MissileID::Elemental,            MissileID::Null,    },         2,        20,         20,         60 },
/*SpellID::ChargedBolt*/      { P_("spell", "Charged Bolt"),       SfxID::CastFire,              100,            5,          6, Lightning | Targeted,          1,          1,      25, { MissileID::ChargedBolt,          MissileID::Null,    },         1,         6,         40,         80 },
/*SpellID::HolyBolt*/         { P_("spell", "Holy Bolt"),          SfxID::CastFire,              100,            5,          7, Magic | Targeted,              1,          1,      20, { MissileID::HolyBolt,             MissileID::Null,    },         1,         3,         40,         80 },
/*SpellID::Resurrect*/        { P_("spell", "Resurrect"),          SfxID::CastHealing,           400,           25,         20, Magic | AllowedInTown,        -1,          5,      30, { MissileID::Resurrect,            MissileID::Null,    },         0,        20,          4,         10 },
/*SpellID::Telekinesis*/      { P_("spell", "Telekinesis"),        SfxID::CastFire,              250,           20,         15, Magic,                         2,          2,      33, { MissileID::Telekinesis,          MissileID::Null,    },         2,         8,         20,         40 },
/*SpellID::HealOther*/        { P_("spell", "Heal Other"),         SfxID::CastHealing,           100,            5,          5, Magic | AllowedInTown,         1,          1,      17, { MissileID::HealOther,            MissileID::Null,    },         3,         1,         20,         40 },
/*SpellID::BloodStar*/        { P_("spell", "Blood Star"),         SfxID::CastFire,             2750,          180,         25, Magic,                        14,         13,      70, { MissileID::BloodStar,            MissileID::Null,    },         2,        14,         20,         60 },
/*SpellID::BoneSpirit*/       { P_("spell", "Bone Spirit"),        SfxID::CastFire,             1150,           80,         24, Magic,                         9,          7,      34, { MissileID::BoneSpirit,           MissileID::Null,    },         1,        12,         20,         60 },
/*SpellID::Mana*/             { P_("spell", "Mana"),               SfxID::CastHealing,           100,            5,        255, Magic | AllowedInTown,        -1,          5,      17, { MissileID::Mana,                 MissileID::Null,    },         3,         1,         12,         24 },
/*SpellID::Magi*/             { P_("spell", "the Magi"),           SfxID::CastHealing,         10000,           20,        255, Magic | AllowedInTown,        -1,         20,      45, { MissileID::Magi,                 MissileID::Null,    },         3,         1,         15,         30 },
/*SpellID::Jester*/           { P_("spell", "the Jester"),         SfxID::CastHealing,         10000,           20,        255, Magic | Targeted,             -1,          4,      30, { MissileID::Jester,               MissileID::Null,    },         3,         1,         15,         30 },
/*SpellID::LightningWall*/    { P_("spell", "Lightning Wall"),     SfxID::CastLightning,         600,           40,         28, Lightning | Targeted,          3,          2,      27, { MissileID::LightningWallControl, MissileID::Null,    },         2,        16,          8,         16 },
/*SpellID::Immolation*/       { P_("spell", "Immolation"),         SfxID::CastFire,             2100,          130,         60, Fire,                         14,         10,      87, { MissileID::Immolation,           MissileID::Null,    },         3,        35,         16,         32 },
/*SpellID::Warp*/             { P_("spell", "Warp"),               SfxID::CastSkill,             300,           20,         35, Magic,                         3,          3,      25, { MissileID::Warp,                 MissileID::Null,    },         3,        18,          8,         12 },
/*SpellID::Reflect*/          { P_("spell", "Reflect"),            SfxID::CastSkill,             300,           20,         35, Magic,                         3,          3,      25, { MissileID::Reflect,              MissileID::Null,    },         3,        15,          8,         12 },
/*SpellID::Berserk*/          { P_("spell", "Berserk"),            SfxID::CastSkill,             300,           20,         35, Magic | Targeted,              3,          3,      35, { MissileID::Berserk,              MissileID::Null,    },         3,        15,          8,         12 },
/*SpellID::RingOfFire*/       { P_("spell", "Ring of Fire"),       SfxID::CastFire,              600,           40,         28, Fire,                          5,          5,      27, { MissileID::RingOfFire,           MissileID::Null,    },         2,        16,          8,         16 },
/*SpellID::Search*/           { P_("spell", "Search"),             SfxID::CastSkill,             300,           20,         15, Magic,                         1,          3,      25, { MissileID::Search,               MissileID::Null,    },         1,         1,          8,         12 },
/*SpellID::RuneOfFire*/       { P_("spell", "Rune of Fire"),       SfxID::CastHealing,           800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfFire,           MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::RuneOfLight*/      { P_("spell", "Rune of Light"),      SfxID::CastHealing,           800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfLight,          MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::RuneOfNova*/       { P_("spell", "Rune of Nova"),       SfxID::CastHealing,           800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfNova,           MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::RuneOfImmolation*/ { P_("spell", "Rune of Immolation"), SfxID::CastHealing,           800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfImmolation,     MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::RuneOfStone*/      { P_("spell", "Rune of Stone"),      SfxID::CastHealing,           800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfStone,          MissileID::Null,    },         1,        10,         40,         80 },
	// clang-format on
};

tl::expected<SpellID, std::string> ParseSpellId(std::string_view value)
{
	if (value == "Null") return SpellID::Null;
	if (value == "Firebolt") return SpellID::Firebolt;
	if (value == "Healing") return SpellID::Healing;
	if (value == "Lightning") return SpellID::Lightning;
	if (value == "Flash") return SpellID::Flash;
	if (value == "Identify") return SpellID::Identify;
	if (value == "FireWall") return SpellID::FireWall;
	if (value == "TownPortal") return SpellID::TownPortal;
	if (value == "StoneCurse") return SpellID::StoneCurse;
	if (value == "Infravision") return SpellID::Infravision;
	if (value == "Phasing") return SpellID::Phasing;
	if (value == "ManaShield") return SpellID::ManaShield;
	if (value == "Fireball") return SpellID::Fireball;
	if (value == "Guardian") return SpellID::Guardian;
	if (value == "ChainLightning") return SpellID::ChainLightning;
	if (value == "FlameWave") return SpellID::FlameWave;
	if (value == "DoomSerpents") return SpellID::DoomSerpents;
	if (value == "BloodRitual") return SpellID::BloodRitual;
	if (value == "Nova") return SpellID::Nova;
	if (value == "Invisibility") return SpellID::Invisibility;
	if (value == "Inferno") return SpellID::Inferno;
	if (value == "Golem") return SpellID::Golem;
	if (value == "Rage") return SpellID::Rage;
	if (value == "Teleport") return SpellID::Teleport;
	if (value == "Apocalypse") return SpellID::Apocalypse;
	if (value == "Etherealize") return SpellID::Etherealize;
	if (value == "ItemRepair") return SpellID::ItemRepair;
	if (value == "StaffRecharge") return SpellID::StaffRecharge;
	if (value == "TrapDisarm") return SpellID::TrapDisarm;
	if (value == "Elemental") return SpellID::Elemental;
	if (value == "ChargedBolt") return SpellID::ChargedBolt;
	if (value == "HolyBolt") return SpellID::HolyBolt;
	if (value == "Resurrect") return SpellID::Resurrect;
	if (value == "Telekinesis") return SpellID::Telekinesis;
	if (value == "HealOther") return SpellID::HealOther;
	if (value == "BloodStar") return SpellID::BloodStar;
	if (value == "BoneSpirit") return SpellID::BoneSpirit;
	if (value == "Mana") return SpellID::Mana;
	if (value == "Magi") return SpellID::Magi;
	if (value == "Jester") return SpellID::Jester;
	if (value == "LightningWall") return SpellID::LightningWall;
	if (value == "Immolation") return SpellID::Immolation;
	if (value == "Warp") return SpellID::Warp;
	if (value == "Reflect") return SpellID::Reflect;
	if (value == "Berserk") return SpellID::Berserk;
	if (value == "RingOfFire") return SpellID::RingOfFire;
	if (value == "Search") return SpellID::Search;
	if (value == "RuneOfFire") return SpellID::RuneOfFire;
	if (value == "RuneOfLight") return SpellID::RuneOfLight;
	if (value == "RuneOfNova") return SpellID::RuneOfNova;
	if (value == "RuneOfImmolation") return SpellID::RuneOfImmolation;
	if (value == "RuneOfStone") return SpellID::RuneOfStone;
	return tl::make_unexpected("Unknown enum value");
}

} // namespace devilution
