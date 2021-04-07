/**
 * @file spelldat.cpp
 *
 * Implementation of all spell data.
 */
#include "all.h"

namespace devilution {

/** Data related to each spell ID. */
SpellData spelldata[] = {
	// clang-format off
	// sName,    sManaCost, sType,           sNameText,            sSkillText, sBookLvl, sStaffLvl, sTargeted, sTownSpell, sMinInt, sSFX,     sMissiles[3],                                sManaAdj, sMinMana, sStaffMin, sStaffMax, sBookCost, sStaffCost
	{ SPL_NULL,          0, STYPE_FIRE,      NULL,                 NULL,              0,         0, FALSE,     FALSE,            0, SFX_NONE, { MIS_NULL,          MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_FIREBOLT,      6, STYPE_FIRE,      "Firebolt",           "Firebolt",        1,         1, TRUE,      FALSE,           15, IS_CAST2, { MIS_FIREBOLT,      MIS_NULL,   MIS_NULL },        1,        3,        40,        80,      1000,         50 },
	{ SPL_HEAL,          5, STYPE_MAGIC,     "Healing",            NULL,              1,         1, FALSE,     TRUE,            17, IS_CAST8, { MIS_HEAL,          MIS_NULL,   MIS_NULL },        3,        1,        20,        40,      1000,         50 },
	{ SPL_LIGHTNING,    10, STYPE_LIGHTNING, "Lightning",          NULL,              4,         3, TRUE,      FALSE,           20, IS_CAST4, { MIS_LIGHTCTRL,     MIS_NULL,   MIS_NULL },        1,        6,        20,        60,      3000,        150 },
	{ SPL_FLASH,        30, STYPE_LIGHTNING, "Flash",              NULL,              5,         4, FALSE,     FALSE,           33, IS_CAST4, { MIS_FLASH,         MIS_FLASH2, MIS_NULL },        2,       16,        20,        40,      7500,        500 },
	{ SPL_IDENTIFY,     13, STYPE_MAGIC,     "Identify",           "Identify",       -1,        -1, FALSE,     TRUE,            23, IS_CAST6, { MIS_IDENTIFY,      MIS_NULL,   MIS_NULL },        2,        1,         8,        12,         0,        100 },
	{ SPL_FIREWALL,     28, STYPE_FIRE,      "Fire Wall",          NULL,              3,         2, TRUE,      FALSE,           27, IS_CAST2, { MIS_FIREWALLC,     MIS_NULL,   MIS_NULL },        2,       16,         8,        16,      6000,        400 },
	{ SPL_TOWN,         35, STYPE_MAGIC,     "Town Portal",        NULL,              3,         3, TRUE,      FALSE,           20, IS_CAST6, { MIS_TOWN,          MIS_NULL,   MIS_NULL },        3,       18,         8,        12,      3000,        200 },
	{ SPL_STONE,        60, STYPE_MAGIC,     "Stone Curse",        NULL,              6,         5, TRUE,      FALSE,           51, IS_CAST2, { MIS_STONE,         MIS_NULL,   MIS_NULL },        3,       40,         8,        16,     12000,        800 },
	{ SPL_INFRA,        40, STYPE_MAGIC,     "Infravision",        NULL,             -1,        -1, FALSE,     FALSE,           36, IS_CAST8, { MIS_INFRA,         MIS_NULL,   MIS_NULL },        5,       20,         0,         0,         0,        600 },
	{ SPL_RNDTELEPORT,  12, STYPE_MAGIC,     "Phasing",            NULL,              7,         6, FALSE,     FALSE,           39, IS_CAST2, { MIS_RNDTELEPORT,   MIS_NULL,   MIS_NULL },        2,        4,        40,        80,      3500,        200 },
	{ SPL_MANASHIELD,   33, STYPE_MAGIC,     "Mana Shield",        NULL,              6,         5, FALSE,     FALSE,           25, IS_CAST2, { MIS_MANASHIELD,    MIS_NULL,   MIS_NULL },        0,       33,         4,        10,     16000,       1200 },
	{ SPL_FIREBALL,     16, STYPE_FIRE,      "Fireball",           NULL,              8,         7, TRUE,      FALSE,           48, IS_CAST2, { MIS_FIREBALL,      MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_GUARDIAN,     50, STYPE_FIRE,      "Guardian",           NULL,              9,         8, TRUE,      FALSE,           61, IS_CAST2, { MIS_GUARDIAN,      MIS_NULL,   MIS_NULL },        2,       30,        16,        32,     14000,        950 },
	{ SPL_CHAIN,        30, STYPE_LIGHTNING, "Chain Lightning",    NULL,              8,         7, FALSE,     FALSE,           54, IS_CAST2, { MIS_CHAIN,         MIS_NULL,   MIS_NULL },        1,       18,        20,        60,     11000,        750 },
	{ SPL_WAVE,         35, STYPE_FIRE,      "Flame Wave",         NULL,              9,         8, TRUE,      FALSE,           54, IS_CAST2, { MIS_WAVE,          MIS_NULL,   MIS_NULL },        3,       20,        20,        40,     10000,        650 },
	{ SPL_DOOMSERP,      0, STYPE_LIGHTNING, "Doom Serpents",      NULL,             -1,        -1, FALSE,     FALSE,            0, IS_CAST2, { MIS_NULL,          MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_BLODRIT,       0, STYPE_MAGIC,     "Blood Ritual",       NULL,             -1,        -1, FALSE,     FALSE,            0, IS_CAST2, { MIS_NULL,          MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_NOVA,         60, STYPE_MAGIC,     "Nova",               NULL,             14,        10, FALSE,     FALSE,           87, IS_CAST4, { MIS_NOVA,          MIS_NULL,   MIS_NULL },        3,       35,        16,        32,     21000,       1300 },
	{ SPL_INVISIBIL,     0, STYPE_MAGIC,     "Invisibility",       NULL,             -1,        -1, FALSE,     FALSE,            0, IS_CAST2, { MIS_NULL,          MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_FLAME,        11, STYPE_FIRE,      "Inferno",            NULL,              3,         2, TRUE,      FALSE,           20, IS_CAST2, { MIS_FLAMEC,        MIS_NULL,   MIS_NULL },        1,        6,        20,        40,      2000,        100 },
	{ SPL_GOLEM,       100, STYPE_FIRE,      "Golem",              NULL,             11,         9, FALSE,     FALSE,           81, IS_CAST2, { MIS_GOLEM,         MIS_NULL,   MIS_NULL },        6,       60,        16,        32,     18000,       1100 },
	{ SPL_BLODBOIL,     15, STYPE_MAGIC,     "Rage",               "Rage",           -1,        -1, FALSE,     FALSE,            0, IS_CAST8, { MIS_BLODBOIL,      MIS_NULL,   MIS_NULL },        1,        1,         0,         0,         0,          0 },
	{ SPL_TELEPORT,     35, STYPE_MAGIC,     "Teleport",           NULL,             14,        12, TRUE,      FALSE,          105, IS_CAST6, { MIS_TELEPORT,      MIS_NULL,   MIS_NULL },        3,       15,        16,        32,     20000,       1250 },
	{ SPL_APOCA,       150, STYPE_FIRE,      "Apocalypse",         NULL,             19,        15, FALSE,     FALSE,          149, IS_CAST2, { MIS_APOCA,         MIS_NULL,   MIS_NULL },        6,       90,         8,        12,     30000,       2000 },
	{ SPL_ETHEREALIZE, 100, STYPE_MAGIC,     "Etherealize",        NULL,             -1,        -1, FALSE,     FALSE,           93, IS_CAST2, { MIS_ETHEREALIZE,   MIS_NULL,   MIS_NULL },        0,      100,         2,         6,     26000,       1600 },
	{ SPL_REPAIR,        0, STYPE_MAGIC,     "Item Repair",        "Item Repair",    -1,        -1, FALSE,     TRUE,            -1, IS_CAST6, { MIS_REPAIR,        MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_RECHARGE,      0, STYPE_MAGIC,     "Staff Recharge",     "Staff Recharge", -1,        -1, FALSE,     TRUE,            -1, IS_CAST6, { MIS_RECHARGE,      MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_DISARM,        0, STYPE_MAGIC,     "Trap Disarm",        "Trap Disarm",    -1,        -1, FALSE,     FALSE,           -1, IS_CAST6, { MIS_DISARM,        MIS_NULL,   MIS_NULL },        0,        0,        40,        80,         0,          0 },
	{ SPL_ELEMENT,      35, STYPE_FIRE,      "Elemental",          NULL,              8,         6, FALSE,     FALSE,           68, IS_CAST2, { MIS_ELEMENT,       MIS_NULL,   MIS_NULL },        2,       20,        20,        60,     10500,        700 },
	{ SPL_CBOLT,         6, STYPE_LIGHTNING, "Charged Bolt",       NULL,              1,         1, TRUE,      FALSE,           25, IS_CAST2, { MIS_CBOLT,         MIS_NULL,   MIS_NULL },        1,        6,        40,        80,      1000,         50 },
	{ SPL_HBOLT,         7, STYPE_MAGIC,     "Holy Bolt",          NULL,              1,         1, TRUE,      FALSE,           20, IS_CAST2, { MIS_HBOLT,         MIS_NULL,   MIS_NULL },        1,        3,        40,        80,      1000,         50 },
	{ SPL_RESURRECT,    20, STYPE_MAGIC,     "Resurrect",          NULL,             -1,         5, FALSE,     TRUE,            30, IS_CAST8, { MIS_RESURRECT,     MIS_NULL,   MIS_NULL },        0,       20,         4,        10,      4000,        250 },
	{ SPL_TELEKINESIS,  15, STYPE_MAGIC,     "Telekinesis",        NULL,              2,         2, FALSE,     FALSE,           33, IS_CAST2, { MIS_TELEKINESIS,   MIS_NULL,   MIS_NULL },        2,        8,        20,        40,      2500,        200 },
	{ SPL_HEALOTHER,     5, STYPE_MAGIC,     "Heal Other",         NULL,              1,         1, FALSE,     TRUE,            17, IS_CAST8, { MIS_HEALOTHER,     MIS_NULL,   MIS_NULL },        3,        1,        20,        40,      1000,         50 },
	{ SPL_FLARE,        25, STYPE_MAGIC,     "Blood Star",         NULL,             14,        13, FALSE,     FALSE,           70, IS_CAST2, { MIS_FLARE,         MIS_NULL,   MIS_NULL },        2,       14,        20,        60,     27500,       1800 },
	{ SPL_BONESPIRIT,   24, STYPE_MAGIC,     "Bone Spirit",        NULL,              9,         7, FALSE,     FALSE,           34, IS_CAST2, { MIS_BONESPIRIT,    MIS_NULL,   MIS_NULL },        1,       12,        20,        60,     11500,        800 },
	{ SPL_MANA,        255, STYPE_MAGIC,     "Mana",               NULL,             -1,         5, FALSE,     TRUE,            17, IS_CAST8, { MIS_MANA,          MIS_NULL,   MIS_NULL },        3,        1,        12,        24,      1000,         50 },
	{ SPL_MAGI,        255, STYPE_MAGIC,     "the Magi",           NULL,             -1,        20, FALSE,     TRUE,            45, IS_CAST8, { MIS_MAGI,          MIS_NULL,   MIS_NULL },        3,        1,        15,        30,    100000,        200 },
	{ SPL_JESTER,      255, STYPE_MAGIC,     "the Jester",         NULL,             -1,         4, TRUE,      FALSE,           30, IS_CAST8, { MIS_JESTER,        MIS_NULL,   MIS_NULL },        3,        1,        15,        30,    100000,        200 },
	{ SPL_LIGHTWALL,    28, STYPE_LIGHTNING, "Lightning Wall",     NULL,              3,         2, TRUE,      FALSE,           27, IS_CAST4, { MIS_LIGHTNINGWALL, MIS_NULL,   MIS_NULL },        2,       16,         8,        16,      6000,        400 },
	{ SPL_IMMOLAT,      60, STYPE_FIRE,      "Immolation",         NULL,             14,        10, FALSE,     FALSE,           87, IS_CAST2, { MIS_IMMOLATION,    MIS_NULL,   MIS_NULL },        3,       35,        16,        32,     21000,       1300 },
	{ SPL_WARP,         35, STYPE_MAGIC,     "Warp",               NULL,              3,         3, FALSE,     FALSE,           25, IS_CAST6, { MIS_WARP,          MIS_NULL,   MIS_NULL },        3,       18,         8,        12,      3000,        200 },
	{ SPL_REFLECT,      35, STYPE_MAGIC,     "Reflect",            NULL,              3,         3, FALSE,     FALSE,           25, IS_CAST6, { MIS_REFLECT,       MIS_NULL,   MIS_NULL },        3,       15,         8,        12,      3000,        200 },
	{ SPL_BERSERK,      35, STYPE_MAGIC,     "Berserk",            NULL,              3,         3, TRUE,      FALSE,           35, IS_CAST6, { MIS_BERSERK,       MIS_NULL,   MIS_NULL },        3,       15,         8,        12,      3000,        200 },
	{ SPL_FIRERING,     28, STYPE_FIRE,      "Ring of Fire",       NULL,              5,         5, FALSE,     FALSE,           27, IS_CAST2, { MIS_FIRERING,      MIS_NULL,   MIS_NULL },        2,       16,         8,        16,      6000,        400 },
	{ SPL_SEARCH,       15, STYPE_MAGIC,     "Search",             "Search",          1,         3, FALSE,     FALSE,           25, IS_CAST6, { MIS_SEARCH,        MIS_NULL,   MIS_NULL },        1,        1,         8,        12,      3000,        200 },
	{ SPL_RUNEFIRE,    255, STYPE_MAGIC,     "Rune of Fire",       NULL,             -1,        -1, TRUE,      FALSE,           48, IS_CAST8, { MIS_RUNEFIRE,      MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_RUNELIGHT,   255, STYPE_MAGIC,     "Rune of Light",      NULL,             -1,        -1, TRUE,      FALSE,           48, IS_CAST8, { MIS_RUNELIGHT,     MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_RUNENOVA,    255, STYPE_MAGIC,     "Rune of Nova",       NULL,             -1,        -1, TRUE,      FALSE,           48, IS_CAST8, { MIS_RUNENOVA,      MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_RUNEIMMOLAT, 255, STYPE_MAGIC,     "Rune of Immolation", NULL,             -1,        -1, TRUE,      FALSE,           48, IS_CAST8, { MIS_RUNEIMMOLAT,   MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	{ SPL_RUNESTONE,   255, STYPE_MAGIC,     "Rune of Stone",      NULL,             -1,        -1, TRUE,      FALSE,           48, IS_CAST8, { MIS_RUNESTONE,     MIS_NULL,   MIS_NULL },        1,       10,        40,        80,      8000,        300 },
	// clang-format on
};

} // namespace devilution
