/**
 * @file spelldat.cpp
 *
 * Implementation of all spell data.
 */
#include "spelldat.h"
#include "utils/language.h"

namespace devilution {

/** Data related to each spell ID. */
const SpellData spelldata[] = {
	// clang-format off
	// sName,    sManaCost, sType,           sNameText,                         sBookLvl, sStaffLvl, sTargeted, sTownSpell, sMinInt, sSFX,     sMissiles[3],                                sManaAdj, sMinMana, sStaffMin, sStaffMax, sBookCost, sStaffCost
	{ SPL_NULL,          0, STYPE_FIRE,      nullptr,                                  0,         0, false,     false,            0, SFX_NONE, { MIS_NULL,          MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_FIREBOLT,      6, STYPE_FIRE,      P_("spell", "Firebolt"),                  1,         1, true,      false,           15, IS_CAST2, { MIS_FIREBOLT,      MIS_NULL,   MIS_NULL },        1,        3,        40,        80,      1000,         50 },
	{ SPL_HEAL,          5, STYPE_MAGIC,     P_("spell", "Healing"),                   1,         1, false,     true,            17, IS_CAST8, { MIS_HEAL,          MIS_NULL,   MIS_NULL },        3,        1,        20,        40,      1000,         50 },
	{ SPL_LIGHTNING,    10, STYPE_LIGHTNING, P_("spell", "Lightning"),                 4,         3, true,      false,           20, IS_CAST4, { MIS_LIGHTCTRL,     MIS_NULL,   MIS_NULL },        1,        6,        20,        60,      3000,        150 },
	{ SPL_FLASH,        30, STYPE_LIGHTNING, P_("spell", "Flash"),                     5,         4, false,     false,           33, IS_CAST4, { MIS_FLASH,         MIS_FLASH2, MIS_NULL },        2,       16,        20,        40,      7500,        500 },
	{ SPL_IDENTIFY,     13, STYPE_MAGIC,     P_("spell", "Identify"),                 -1,        -1, false,     true,            23, IS_CAST6, { MIS_IDENTIFY,      MIS_NULL,   MIS_NULL },        2,        1,         8,        12,         0,        100 },
	{ SPL_FIREWALL,     28, STYPE_FIRE,      P_("spell", "Fire Wall"),                 3,         2, true,      false,           27, IS_CAST2, { MIS_FIREWALLC,     MIS_NULL,   MIS_NULL },        2,       16,         8,        16,      6000,        400 },
	{ SPL_TOWN,         35, STYPE_MAGIC,     P_("spell", "Town Portal"),               3,         3, true,      false,           20, IS_CAST6, { MIS_TOWN,          MIS_NULL,   MIS_NULL },        3,       18,         8,        12,      3000,        200 },
	{ SPL_STONE,        60, STYPE_MAGIC,     P_("spell", "Stone Curse"),               6,         5, true,      false,           51, IS_CAST2, { MIS_STONE,         MIS_NULL,   MIS_NULL },        3,       40,         8,        16,     12000,        800 },
	{ SPL_INFRA,        40, STYPE_MAGIC,     P_("spell", "Infravision"),              -1,        -1, false,     false,           36, IS_CAST8, { MIS_INFRA,         MIS_NULL,   MIS_NULL },        5,       20,         0,         0,         0,        600 },
	{ SPL_RNDTELEPORT,  12, STYPE_MAGIC,     P_("spell", "Phasing"),                   7,         6, false,     false,           39, IS_CAST2, { MIS_RNDTELEPORT,   MIS_NULL,   MIS_NULL },        2,        4,        40,        80,      3500,        200 },
	{ SPL_MANASHIELD,   33, STYPE_MAGIC,     P_("spell", "Mana Shield"),               6,         5, false,     false,           25, IS_CAST2, { MIS_MANASHIELD,    MIS_NULL,   MIS_NULL },        0,       33,         4,        10,     16000,       1200 },
	{ SPL_FIREBALL,     16, STYPE_FIRE,      P_("spell", "Fireball"),                  8,         7, true,      false,           48, IS_CAST2, { MIS_FIREBALL,      MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_GUARDIAN,     50, STYPE_FIRE,      P_("spell", "Guardian"),                  9,         8, true,      false,           61, IS_CAST2, { MIS_GUARDIAN,      MIS_NULL,   MIS_NULL },        2,       30,        16,        32,     14000,        950 },
	{ SPL_CHAIN,        30, STYPE_LIGHTNING, P_("spell", "Chain Lightning"),           8,         7, false,     false,           54, IS_CAST2, { MIS_CHAIN,         MIS_NULL,   MIS_NULL },        1,       18,        20,        60,     11000,        750 },
	{ SPL_WAVE,         35, STYPE_FIRE,      P_("spell", "Flame Wave"),                9,         8, true,      false,           54, IS_CAST2, { MIS_WAVE,          MIS_NULL,   MIS_NULL },        3,       20,        20,        40,     10000,        650 },
	{ SPL_DOOMSERP,      0, STYPE_LIGHTNING, P_("spell", "Doom Serpents"),            -1,        -1, false,     false,            0, IS_CAST2, { MIS_NULL,          MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_BLODRIT,       0, STYPE_MAGIC,     P_("spell", "Blood Ritual"),             -1,        -1, false,     false,            0, IS_CAST2, { MIS_NULL,          MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_NOVA,         60, STYPE_MAGIC,     P_("spell", "Nova"),                     14,        10, false,     false,           87, IS_CAST4, { MIS_NOVA,          MIS_NULL,   MIS_NULL },        3,       35,        16,        32,     21000,       1300 },
	{ SPL_INVISIBIL,     0, STYPE_MAGIC,     P_("spell", "Invisibility"),             -1,        -1, false,     false,            0, IS_CAST2, { MIS_NULL,          MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_FLAME,        11, STYPE_FIRE,      P_("spell", "Inferno"),                   3,         2, true,      false,           20, IS_CAST2, { MIS_FLAMEC,        MIS_NULL,   MIS_NULL },        1,        6,        20,        40,      2000,        100 },
	{ SPL_GOLEM,       100, STYPE_FIRE,      P_("spell", "Golem"),                    11,         9, false,     false,           81, IS_CAST2, { MIS_GOLEM,         MIS_NULL,   MIS_NULL },        6,       60,        16,        32,     18000,       1100 },
	{ SPL_BLODBOIL,     15, STYPE_MAGIC,     P_("spell", "Rage"),                     -1,        -1, false,     false,            0, IS_CAST8, { MIS_BLODBOIL,      MIS_NULL,   MIS_NULL },        1,        1,         0,         0,         0,          0 },
	{ SPL_TELEPORT,     35, STYPE_MAGIC,     P_("spell", "Teleport"),                 14,        12, true,      false,          105, IS_CAST6, { MIS_TELEPORT,      MIS_NULL,   MIS_NULL },        3,       15,        16,        32,     20000,       1250 },
	{ SPL_APOCA,       150, STYPE_FIRE,      P_("spell", "Apocalypse"),               19,        15, false,     false,          149, IS_CAST2, { MIS_APOCA,         MIS_NULL,   MIS_NULL },        6,       90,         8,        12,     30000,       2000 },
	{ SPL_ETHEREALIZE, 100, STYPE_MAGIC,     P_("spell", "Etherealize"),              -1,        -1, false,     false,           93, IS_CAST2, { MIS_ETHEREALIZE,   MIS_NULL,   MIS_NULL },        0,      100,         2,         6,     26000,       1600 },
	{ SPL_REPAIR,        0, STYPE_MAGIC,     P_("spell", "Item Repair"),              -1,        -1, false,     true,            -1, IS_CAST6, { MIS_REPAIR,        MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_RECHARGE,      0, STYPE_MAGIC,     P_("spell", "Staff Recharge"),           -1,        -1, false,     true,            -1, IS_CAST6, { MIS_RECHARGE,      MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_DISARM,        0, STYPE_MAGIC,     P_("spell", "Trap Disarm"),              -1,        -1, false,     false,           -1, IS_CAST6, { MIS_DISARM,        MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_ELEMENT,      35, STYPE_FIRE,      P_("spell", "Elemental"),                 8,         6, false,     false,           68, IS_CAST2, { MIS_ELEMENT,       MIS_NULL,   MIS_NULL },        2,       20,        20,        60,     10500,        700 },
	{ SPL_CBOLT,         6, STYPE_LIGHTNING, P_("spell", "Charged Bolt"),              1,         1, true,      false,           25, IS_CAST2, { MIS_CBOLT,         MIS_NULL,   MIS_NULL },        1,        6,        40,        80,      1000,         50 },
	{ SPL_HBOLT,         7, STYPE_MAGIC,     P_("spell", "Holy Bolt"),                 1,         1, true,      false,           20, IS_CAST2, { MIS_HBOLT,         MIS_NULL,   MIS_NULL },        1,        3,        40,        80,      1000,         50 },
	{ SPL_RESURRECT,    20, STYPE_MAGIC,     P_("spell", "Resurrect"),                -1,         5, false,     true,            30, IS_CAST8, { MIS_RESURRECT,     MIS_NULL,   MIS_NULL },        0,       20,         4,        10,      4000,        250 },
	{ SPL_TELEKINESIS,  15, STYPE_MAGIC,     P_("spell", "Telekinesis"),               2,         2, false,     false,           33, IS_CAST2, { MIS_TELEKINESIS,   MIS_NULL,   MIS_NULL },        2,        8,        20,        40,      2500,        200 },
	{ SPL_HEALOTHER,     5, STYPE_MAGIC,     P_("spell", "Heal Other"),                1,         1, false,     true,            17, IS_CAST8, { MIS_HEALOTHER,     MIS_NULL,   MIS_NULL },        3,        1,        20,        40,      1000,         50 },
	{ SPL_FLARE,        25, STYPE_MAGIC,     P_("spell", "Blood Star"),               14,        13, false,     false,           70, IS_CAST2, { MIS_FLARE,         MIS_NULL,   MIS_NULL },        2,       14,        20,        60,     27500,       1800 },
	{ SPL_BONESPIRIT,   24, STYPE_MAGIC,     P_("spell", "Bone Spirit"),               9,         7, false,     false,           34, IS_CAST2, { MIS_BONESPIRIT,    MIS_NULL,   MIS_NULL },        1,       12,        20,        60,     11500,        800 },
	{ SPL_MANA,        255, STYPE_MAGIC,     P_("spell", "Mana"),                     -1,         5, false,     true,            17, IS_CAST8, { MIS_MANA,          MIS_NULL,   MIS_NULL },        3,        1,        12,        24,      1000,         50 },
	{ SPL_MAGI,        255, STYPE_MAGIC,     P_("spell", "the Magi"),                 -1,        20, false,     true,            45, IS_CAST8, { MIS_MAGI,          MIS_NULL,   MIS_NULL },        3,        1,        15,        30,    100000,        200 },
	{ SPL_JESTER,      255, STYPE_MAGIC,     P_("spell", "the Jester"),               -1,         4, true,      false,           30, IS_CAST8, { MIS_JESTER,        MIS_NULL,   MIS_NULL },        3,        1,        15,        30,    100000,        200 },
	{ SPL_LIGHTWALL,    28, STYPE_LIGHTNING, P_("spell", "Lightning Wall"),            3,         2, true,      false,           27, IS_CAST4, { MIS_LIGHTNINGWALL, MIS_NULL,   MIS_NULL },        2,       16,         8,        16,      6000,        400 },
	{ SPL_IMMOLAT,      60, STYPE_FIRE,      P_("spell", "Immolation"),               14,        10, false,     false,           87, IS_CAST2, { MIS_IMMOLATION,    MIS_NULL,   MIS_NULL },        3,       35,        16,        32,     21000,       1300 },
	{ SPL_WARP,         35, STYPE_MAGIC,     P_("spell", "Warp"),                      3,         3, false,     false,           25, IS_CAST6, { MIS_WARP,          MIS_NULL,   MIS_NULL },        3,       18,         8,        12,      3000,        200 },
	{ SPL_REFLECT,      35, STYPE_MAGIC,     P_("spell", "Reflect"),                   3,         3, false,     false,           25, IS_CAST6, { MIS_REFLECT,       MIS_NULL,   MIS_NULL },        3,       15,         8,        12,      3000,        200 },
	{ SPL_BERSERK,      35, STYPE_MAGIC,     P_("spell", "Berserk"),                   3,         3, true,      false,           35, IS_CAST6, { MIS_BERSERK,       MIS_NULL,   MIS_NULL },        3,       15,         8,        12,      3000,        200 },
	{ SPL_FIRERING,     28, STYPE_FIRE,      P_("spell", "Ring of Fire"),              5,         5, false,     false,           27, IS_CAST2, { MIS_FIRERING,      MIS_NULL,   MIS_NULL },        2,       16,         8,        16,      6000,        400 },
	{ SPL_SEARCH,       15, STYPE_MAGIC,     P_("spell", "Search"),                    1,         3, false,     false,           25, IS_CAST6, { MIS_SEARCH,        MIS_NULL,   MIS_NULL },        1,        1,         8,        12,      3000,        200 },
	{ SPL_RUNEFIRE,    255, STYPE_MAGIC,     P_("spell", "Rune of Fire"),             -1,        -1, true,      false,           48, IS_CAST8, { MIS_RUNEFIRE,      MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_RUNELIGHT,   255, STYPE_MAGIC,     P_("spell", "Rune of Light"),            -1,        -1, true,      false,           48, IS_CAST8, { MIS_RUNELIGHT,     MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_RUNENOVA,    255, STYPE_MAGIC,     P_("spell", "Rune of Nova"),             -1,        -1, true,      false,           48, IS_CAST8, { MIS_RUNENOVA,      MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_RUNEIMMOLAT, 255, STYPE_MAGIC,     P_("spell", "Rune of Immolation"),       -1,        -1, true,      false,           48, IS_CAST8, { MIS_RUNEIMMOLAT,   MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_RUNESTONE,   255, STYPE_MAGIC,     P_("spell", "Rune of Stone"),            -1,        -1, true,      false,           48, IS_CAST8, { MIS_RUNESTONE,     MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	// clang-format on
};

} // namespace devilution
