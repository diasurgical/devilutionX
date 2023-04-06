/**
 * @file spelldat.cpp
 *
 * Implementation of all spell data.
 */
#include "spelldat.h"
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
// id                           sNameText,                         sSFX,      bookCost10,  staffCost10,  sManaCost, flags,                  sBookLvl,  sStaffLvl,  minInt, { sMissiles[2]                                         }   sManaAdj,  sMinMana,  sStaffMin,  sStaffMax
/*SpellID::Null*/             { nullptr,                           SFX_NONE,           0,            0,          0, Fire,                          0,          0,       0, { MissileID::Null,                 MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::Firebolt*/         { P_("spell", "Firebolt"),           IS_CAST2,         100,            5,          6, Fire | Targeted,               1,          1,      15, { MissileID::Firebolt,             MissileID::Null,    },         1,         3,         40,         80 },
/*SpellID::Healing*/          { P_("spell", "Healing"),            IS_CAST8,         100,            5,          5, Magic | AllowedInTown,         1,          1,      17, { MissileID::Healing,              MissileID::Null,    },         3,         1,         20,         40 },
/*SpellID::Lightning*/        { P_("spell", "Lightning"),          IS_CAST4,         300,           15,         10, Lightning | Targeted,          4,          3,      20, { MissileID::LightningControl,     MissileID::Null,    },         1,         6,         20,         60 },
/*SpellID::Flash*/            { P_("spell", "Flash"),              IS_CAST4,         750,           50,         30, Lightning,                     5,          4,      33, { MissileID::FlashBottom,          MissileID::FlashTop },         2,        16,         20,         40 },
/*SpellID::Identify*/         { P_("spell", "Identify"),           IS_CAST6,           0,           10,         13, Magic | AllowedInTown,        -1,         -1,      23, { MissileID::Identify,             MissileID::Null,    },         2,         1,          8,         12 },
/*SpellID::FireWall*/         { P_("spell", "Fire Wall"),          IS_CAST2,         600,           40,         28, Fire | Targeted,               3,          2,      27, { MissileID::FireWallControl,      MissileID::Null,    },         2,        16,          8,         16 },
/*SpellID::TownPortal*/       { P_("spell", "Town Portal"),        IS_CAST6,         300,           20,         35, Magic | Targeted,              3,          3,      20, { MissileID::TownPortal,           MissileID::Null,    },         3,        18,          8,         12 },
/*SpellID::StoneCurse*/       { P_("spell", "Stone Curse"),        IS_CAST2,        1200,           80,         60, Magic | Targeted,              6,          5,      51, { MissileID::StoneCurse,           MissileID::Null,    },         3,        40,          8,         16 },
/*SpellID::Infravision*/      { P_("spell", "Infravision"),        IS_CAST8,           0,           60,         40, Magic,                        -1,         -1,      36, { MissileID::Infravision,          MissileID::Null,    },         5,        20,          0,          0 },
/*SpellID::Phasing*/          { P_("spell", "Phasing"),            IS_CAST2,         350,           20,         12, Magic,                         7,          6,      39, { MissileID::Phasing,              MissileID::Null,    },         2,         4,         40,         80 },
/*SpellID::ManaShield*/       { P_("spell", "Mana Shield"),        IS_CAST2,        1600,          120,         33, Magic,                         6,          5,      25, { MissileID::ManaShield,           MissileID::Null,    },         0,        33,          4,         10 },
/*SpellID::Fireball*/         { P_("spell", "Fireball"),           IS_CAST2,         800,           30,         16, Fire | Targeted,               8,          7,      48, { MissileID::Fireball,             MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::Guardian*/         { P_("spell", "Guardian"),           IS_CAST2,        1400,           95,         50, Fire | Targeted,               9,          8,      61, { MissileID::Guardian,             MissileID::Null,    },         2,        30,         16,         32 },
/*SpellID::ChainLightning*/   { P_("spell", "Chain Lightning"),    IS_CAST2,        1100,           75,         30, Lightning,                     8,          7,      54, { MissileID::ChainLightning,       MissileID::Null,    },         1,        18,         20,         60 },
/*SpellID::FlameWave*/        { P_("spell", "Flame Wave"),         IS_CAST2,        1000,           65,         35, Fire | Targeted,               9,          8,      54, { MissileID::FlameWaveControl,     MissileID::Null,    },         3,        20,         20,         40 },
/*SpellID::DoomSerpents*/     { P_("spell", "Doom Serpents"),      IS_CAST2,           0,            0,          0, Lightning,                    -1,         -1,       0, { MissileID::Null,                 MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::BloodRitual*/      { P_("spell", "Blood Ritual"),       IS_CAST2,           0,            0,          0, Magic,                        -1,         -1,       0, { MissileID::Null,                 MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::Nova*/             { P_("spell", "Nova"),               IS_CAST4,        2100,          130,         60, Magic,                        14,         10,      87, { MissileID::Nova,                 MissileID::Null,    },         3,        35,         16,         32 },
/*SpellID::Invisibility*/     { P_("spell", "Invisibility"),       IS_CAST2,           0,            0,          0, Magic,                        -1,         -1,       0, { MissileID::Null,                 MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::Inferno*/          { P_("spell", "Inferno"),            IS_CAST2,         200,           10,         11, Fire | Targeted,               3,          2,      20, { MissileID::InfernoControl,       MissileID::Null,    },         1,         6,         20,         40 },
/*SpellID::Golem*/            { P_("spell", "Golem"),              IS_CAST2,        1800,          110,        100, Fire,                         11,          9,      81, { MissileID::Golem,                MissileID::Null,    },         6,        60,         16,         32 },
/*SpellID::Rage*/             { P_("spell", "Rage"),               IS_CAST8,           0,            0,         15, Magic,                        -1,         -1,       0, { MissileID::Rage,                 MissileID::Null,    },         1,         1,          0,          0 },
/*SpellID::Teleport*/         { P_("spell", "Teleport"),           IS_CAST6,        2000,          125,         35, Magic | Targeted,             14,         12,     105, { MissileID::Teleport,             MissileID::Null,    },         3,        15,         16,         32 },
/*SpellID::Apocalypse*/       { P_("spell", "Apocalypse"),         IS_CAST2,        3000,          200,        150, Fire,                         19,         15,     149, { MissileID::Apocalypse,           MissileID::Null,    },         6,        90,          8,         12 },
/*SpellID::Etherealize*/      { P_("spell", "Etherealize"),        IS_CAST2,        2600,          160,        100, Magic,                        -1,         -1,      93, { MissileID::Etherealize,          MissileID::Null,    },         0,       100,          2,          6 },
/*SpellID::ItemRepair*/       { P_("spell", "Item Repair"),        IS_CAST6,           0,            0,          0, Magic | AllowedInTown,        -1,         -1,     255, { MissileID::ItemRepair,           MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::StaffRecharge*/    { P_("spell", "Staff Recharge"),     IS_CAST6,           0,            0,          0, Magic | AllowedInTown,        -1,         -1,     255, { MissileID::StaffRecharge,        MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::TrapDisarm*/       { P_("spell", "Trap Disarm"),        IS_CAST6,           0,            0,          0, Magic,                        -1,         -1,     255, { MissileID::TrapDisarm,           MissileID::Null,    },         0,         0,         40,         80 },
/*SpellID::Elemental*/        { P_("spell", "Elemental"),          IS_CAST2,        1050,           70,         35, Fire,                          8,          6,      68, { MissileID::Elemental,            MissileID::Null,    },         2,        20,         20,         60 },
/*SpellID::ChargedBolt*/      { P_("spell", "Charged Bolt"),       IS_CAST2,         100,            5,          6, Lightning | Targeted,          1,          1,      25, { MissileID::ChargedBolt,          MissileID::Null,    },         1,         6,         40,         80 },
/*SpellID::HolyBolt*/         { P_("spell", "Holy Bolt"),          IS_CAST2,         100,            5,          7, Magic | Targeted,              1,          1,      20, { MissileID::HolyBolt,             MissileID::Null,    },         1,         3,         40,         80 },
/*SpellID::Resurrect*/        { P_("spell", "Resurrect"),          IS_CAST8,         400,           25,         20, Magic | AllowedInTown,        -1,          5,      30, { MissileID::Resurrect,            MissileID::Null,    },         0,        20,          4,         10 },
/*SpellID::Telekinesis*/      { P_("spell", "Telekinesis"),        IS_CAST2,         250,           20,         15, Magic,                         2,          2,      33, { MissileID::Telekinesis,          MissileID::Null,    },         2,         8,         20,         40 },
/*SpellID::HealOther*/        { P_("spell", "Heal Other"),         IS_CAST8,         100,            5,          5, Magic | AllowedInTown,         1,          1,      17, { MissileID::HealOther,            MissileID::Null,    },         3,         1,         20,         40 },
/*SpellID::BloodStar*/        { P_("spell", "Blood Star"),         IS_CAST2,        2750,          180,         25, Magic,                        14,         13,      70, { MissileID::BloodStar,            MissileID::Null,    },         2,        14,         20,         60 },
/*SpellID::BoneSpirit*/       { P_("spell", "Bone Spirit"),        IS_CAST2,        1150,           80,         24, Magic,                         9,          7,      34, { MissileID::BoneSpirit,           MissileID::Null,    },         1,        12,         20,         60 },
/*SpellID::Mana*/             { P_("spell", "Mana"),               IS_CAST8,         100,            5,        255, Magic | AllowedInTown,        -1,          5,      17, { MissileID::Mana,                 MissileID::Null,    },         3,         1,         12,         24 },
/*SpellID::Magi*/             { P_("spell", "the Magi"),           IS_CAST8,       10000,           20,        255, Magic | AllowedInTown,        -1,         20,      45, { MissileID::Magi,                 MissileID::Null,    },         3,         1,         15,         30 },
/*SpellID::Jester*/           { P_("spell", "the Jester"),         IS_CAST8,       10000,           20,        255, Magic | Targeted,             -1,          4,      30, { MissileID::Jester,               MissileID::Null,    },         3,         1,         15,         30 },
/*SpellID::LightningWall*/    { P_("spell", "Lightning Wall"),     IS_CAST4,         600,           40,         28, Lightning | Targeted,          3,          2,      27, { MissileID::LightningWallControl, MissileID::Null,    },         2,        16,          8,         16 },
/*SpellID::Immolation*/       { P_("spell", "Immolation"),         IS_CAST2,        2100,          130,         60, Fire,                         14,         10,      87, { MissileID::Immolation,           MissileID::Null,    },         3,        35,         16,         32 },
/*SpellID::Warp*/             { P_("spell", "Warp"),               IS_CAST6,         300,           20,         35, Magic,                         3,          3,      25, { MissileID::Warp,                 MissileID::Null,    },         3,        18,          8,         12 },
/*SpellID::Reflect*/          { P_("spell", "Reflect"),            IS_CAST6,         300,           20,         35, Magic,                         3,          3,      25, { MissileID::Reflect,              MissileID::Null,    },         3,        15,          8,         12 },
/*SpellID::Berserk*/          { P_("spell", "Berserk"),            IS_CAST6,         300,           20,         35, Magic | Targeted,              3,          3,      35, { MissileID::Berserk,              MissileID::Null,    },         3,        15,          8,         12 },
/*SpellID::RingOfFire*/       { P_("spell", "Ring of Fire"),       IS_CAST2,         600,           40,         28, Fire,                          5,          5,      27, { MissileID::RingOfFire,           MissileID::Null,    },         2,        16,          8,         16 },
/*SpellID::Search*/           { P_("spell", "Search"),             IS_CAST6,         300,           20,         15, Magic,                         1,          3,      25, { MissileID::Search,               MissileID::Null,    },         1,         1,          8,         12 },
/*SpellID::RuneOfFire*/       { P_("spell", "Rune of Fire"),       IS_CAST8,         800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfFire,           MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::RuneOfLight*/      { P_("spell", "Rune of Light"),      IS_CAST8,         800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfLight,          MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::RuneOfNova*/       { P_("spell", "Rune of Nova"),       IS_CAST8,         800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfNova,           MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::RuneOfImmolation*/ { P_("spell", "Rune of Immolation"), IS_CAST8,         800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfImmolation,     MissileID::Null,    },         1,        10,         40,         80 },
/*SpellID::RuneOfStone*/      { P_("spell", "Rune of Stone"),      IS_CAST8,         800,           30,        255, Magic | Targeted,             -1,         -1,      48, { MissileID::RuneOfStone,          MissileID::Null,    },         1,        10,         40,         80 },
	// clang-format on
};

} // namespace devilution
