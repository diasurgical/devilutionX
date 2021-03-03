#include <gtest/gtest.h>
#include "all.h"

dvl::PkItemStruct diabloItems[] = {
	// clang-format off
	//     iSeed, iCreateInfo, idx, bId, bDur, bMDur, bCh, bMCh, wValue, dwBuff
	{ 2082213289,       0x119,  53,   3,   60,    60,   0,    0,      0,      0 }, // Amber Helm of harmony
	{  338833725,       0x118, 154,   3,    0,     0,   0,    0,      0,      0 }, // Cobalt Amulet of giants
	{  545145866,       0x11A, 120,   3,   38,    40,   0,    0,      0,      0 }, // Brutal Sword of gore
	{ 1504248345,       0x35A,  70,   5,  255,   255,   0,    0,      0,      0 }, // Demonspike Coat
	{ 1884424756,       0x146, 151,   3,    0,     0,   0,    0,      0,      0 }, // Steel Ring of the jaguar
	{ 1712759905,        0xDC, 151,   3,    0,     0,   0,    0,      0,      0 }, // Ring of the heavens
	{  981777658,       0x11E, 153,   2,    0,     0,   0,    0,      0,      0 }, // Ring of sorcery
	{  844854815,       0x11A,  75,   3,  255,   255,   0,    0,      0,      0 }, // Shield of the ages
	{ 1151513535,       0x11E,  73,   2,   12,    32,   0,    0,      0,      0 }, // Sapphire Shield
	{  640243885,           6,  27,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Town Portal
	{  741806938,           9,  25,   0,    0,     0,   0,    0,      0,      0 }, // IDI_MANA
	{ 1456608333,         257,  79,   0,    0,     0,   0,    0,      0,      0 }, // Mana
	{  554676945,          16,  30,   0,    0,     0,   0,    0,      0,      0 }, // Full mana
	{  355389938,           0,  24,   0,    0,     0,   0,    0,      0,      0 }, // Healing
	{  868435486,          16,  29,   0,    0,     0,   0,    0,      0,      0 }, // Full healing
	{  797389356,           0,   0,   0,    0,     0,   0,    0,    100,      0 }, // Gold
	{ 1372832903,           0,   4,   0,   30,    30,   0,    0,      0,      0 }, // IDI_ROGUE
	{  896239556,        2068,  53,   3,   56,    60,   0,    0,      0,      0 }, // Jade Helm of the wolf
	{ 1286225254,         269, 151,   3,    0,     0,   0,    0,      0,      0 }, // Steel Ring of accuracy
	{  548642293,         266,  21,   0,    0,     0,   0,    0,      0,      0 }, // Blood Stone
	{  307669016,         270, 151,   3,    0,     0,   0,    0,      0,      0 }, // Ring of power
	{  204766888,         332, 154,   3,    0,     0,   0,    0,      0,      0 }, // Gold Amulet of accuracy
	{ 1642744332,         273, 122,   3,   25,    60,   0,    0,      0,      0 }, // Sword of the bat
	{ 1031508271,        1036,  72,   0,   14,    24,   0,    0,      0,      0 }, // Small Shield
	{ 1384620228,         338,  65,   3,   44,    80,   0,    0,      0,      0 }, // Plate of giant
	{  681296530,         266,  87,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Healing
	{ 1857753366,         260,  81,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Rejuvenation
	{  965103261,         273,  82,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Full Rejuvenation
	{  430621075,         217, 141,   3,   45,    45,   0,    0,      0,      0 }, // Savage Bow of perfection
	{ 1272669062,         258, 115,   0,   10,    20,   0,    0,      0,      0 }, // Falchion
	{ 1133884051,         278, 120,   2,   18,    40,   0,    0,      0,      0 }, // Sword of vim
	{          2,         776,   8,   5,    0,     0,   0,    0,      0,      0 }, // Empyrean Band,
	{          3,          776, 10,   5,    0,     0,   0,    0,      0,      0 }, // Optic Amulet
	{          4,         772,  11,   5,    0,     0,   0,    0,      0,      0 }, // Ring of Truth
	{          5,         776,  13,   5,   15,    15,   0,    0,      0,      0 }, // Harlequin Crest
	{          6,         527,  14,   5,   60,    60,   0,    0,      0,      0 }, // Veil of Steel
	{          7,         781,  28,   5,   39,    40,   0,    0,      0,      0 }, // Arkaine's Valor
	{          8,         787,  31,   5,   42,    44,   0,    0,      0,      0 }, // Griswold's Edge
	{          6,         911,  14,   5,   60,    60,   0,    0,      0,      0 }, // Veil of Steel - with morph bug
	// clang-format on
};

const char *diabloItemNames[] = {
	"Amber Helm of harmony",
	"Cobalt Amulet of giants",
	"Brutal Sword of gore",
	"Demonspike Coat",
	"Steel Ring of the jaguar",
	"Ring of the heavens",
	"Ring of sorcery",
	"Shield of the ages",
	"Sapphire Shield",
	"Scroll of Town Portal",
	"Potion of Mana",
	"Potion of Mana",
	"Potion of Full Mana",
	"Potion of Healing",
	"Potion of Full Healing",
	"Gold",
	"Short Bow",
	"Jade Helm of the wolf",
	"Steel Ring of accuracy",
	"Blood Stone",
	"Ring of power",
	"Gold Amulet of accuracy",
	"Sword of the bat",
	"Small Shield",
	"Plate of giants",
	"Scroll of Healing",
	"Potion of Rejuvenation",
	"Potion of Full Rejuvenation",
	"Savage Bow of perfection",
	"Falchion",
	"Sword of vim",
	"Empyrean Band",
	"Optic Amulet",
	"Ring of Truth",
	"Harlequin Crest",
	"Veil of Steel",
	"Arkaine's Valor",
	"Griswold's Edge",
	"Veil of Steel",
};

TEST(pack, UnPackItem_diablo)
{
	dvl::ItemStruct id;

	dvl::gbIsHellfire = false;
	dvl::gbIsHellfireSaveGame = false;
	dvl::gbIsMultiplayer = false;

	for (size_t i = 0; i < sizeof(diabloItems) / sizeof(*diabloItems); i++) {
		dvl::UnPackItem(&diabloItems[i], &id);

		ASSERT_STREQ(id._iIName, diabloItemNames[i]);
	}
}

dvl::PkItemStruct diabloItemsMP[] = {
	// clang-format off
	//     iSeed, iCreateInfo, idx, bId, bDur, bMDur, bCh, bMCh, wValue, dwBuff
    {  309674341,         193, 109,   0,    0,     0,   0,    0,      0,      0 }, // Book of Firebolt
	// clang-format on
};

const char *diabloItemNamesMP[] = {
	"Book of Firebolt",
};

TEST(pack, UnPackItem_diablo_multiplayer)
{
	dvl::ItemStruct id;

	dvl::gbIsHellfire = false;
	dvl::gbIsHellfireSaveGame = false;
	dvl::gbIsMultiplayer = true;

	for (size_t i = 0; i < sizeof(diabloItemsMP) / sizeof(*diabloItemsMP); i++) {
		dvl::UnPackItem(&diabloItemsMP[i], &id);

		ASSERT_STREQ(id._iIName, diabloItemNamesMP[i]);
	}
}

dvl::PkItemStruct hellfireItems[] = {
	// clang-format off
	//     iSeed, iCreateInfo, idx, bId, bDur, bMDur, bCh, bMCh, wValue, dwBuff
	{ 1717442367,         266, 156,   3,    0,     0,   0,    0,      0,      0 }, // Ring of stability
	{ 1268518156,         338, 157,   3,    0,     0,   0,    0,      0,      0 }, // Ring of precision
	{  132733863,         283, 157,   3,    0,     0,   0,    0,      0,      0 }, // Obsidian Ring of wizardry
	{  511953594,         283, 158,   3,    0,     0,   0,    0,      0,      0 }, // Ring of precision
	{ 1183326923,         346, 160,   3,    0,     0,   0,    0,      0,      0 }, // Amulet of titans
	{ 1863009736,         280, 160,   3,    0,     0,   0,    0,      0,      0 }, // Gold Amulet
	{ 1872860650,         734, 135,   5,   75,    75,   0,    0,      0,      0 }, // Messerschmidt's Reaver
	{ 1584694222,         281, 142,   3,  127,   128,   0,    0,      0,      0 }, // Vicious Maul of structure
	{  669112929,         280, 119,   0,   15,    24,   0,    0,      0,      0 }, // Short Sword
	{  303108965,         280, 148,   3,   18,    50,   0,    0,      0,      0 }, // Bow of shock
	{  575830996,         257, 143,   3,   30,    30,   0,    0,      0,      0 }, // Bow of magic
	{ 1488880650,         194, 152,   3,   35,    35,  22,   33,      0,      0 }, // Red Staff of Healing
	{ 1864450901,         263,  71,   0,    6,    16,   0,    0,      0,      0 }, // Buckler
	{   28387651,         263,  49,   0,   15,    20,   0,    0,      0,      0 }, // Skull Cap
	{ 1298183212,         257,  55,   0,    6,     6,   0,    0,      0,      0 }, // Rags
	{ 1113945523,         260,  58,   0,   30,    30,   0,    0,      0,      0 }, // Quilted Armor
	{  765757608,         260,  58,   2,   12,    30,   0,    0,      0,      0 }, // Armor of light
	{  188812770,         346,  67,   3,   75,    75,   0,    0,      0,      0 }, // Saintly Plate of the stars
	{  283577043,        2070,  67,   3,   63,    75,   0,    0,      0,      0 }, // Plate of the stars
	{  123272767,          16,  24,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Healing
	{  433688373,          16,  29,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Full Healing
	{ 1213385484,       32770,  25,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Mana
	{ 1405075219,         280, 110,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Golem
	{ 1478792102,         259,  92,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Search
	{ 1569255955,         262,  94,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Identify
	{ 1291205782,         261,  98,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Town Portal
	{  811925807,         260,  91,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Healing
	{ 1275007287,         257, 161,   0,    0,     0,   0,    0,      0,      0 }, // Rune of Fire
	{  561216242,         278,   0,   0,    0,     0,   0,    0,   1663,      0 }, // Gold
	{          1,         515,   7,   5,   45,    50,   0,    0,      0,      0 }, // The Undead Crown
	{          2,         774,   8,   5,    0,     0,   0,    0,      0,      0 }, // Empyrean Band
	{          4,         769,  11,   5,    0,     0,   0,    0,      0,      0 }, // Ring of Truth
	{          8,         512,  31,   5,   50,    50,   0,    0,      0,      0 }, // Griswold's Edge
	{          9,         850,  32,   5,  255,   255,   0,    0,      0,      0 }, // Bovine Plate
	// clang-format on
};

const char *hellfireItemNames[] = {
	"Ring of stability",
	"Ring of precision",
	"Obsidian Ring of wizardry",
	"Ring of precision",
	"Amulet of titans",
	"Gold Amulet",
	"Messerschmidt's Reaver",
	"Vicious Maul of structure",
	"Short Sword",
	"Bow of shock",
	"Bow of magic",
	"Red Staff of Healing",
	"Buckler",
	"Skull Cap",
	"Rags",
	"Quilted Armor",
	"Armor of light",
	"Saintly Plate of the stars",
	"Plate of the stars",
	"Potion of Healing",
	"Potion of Full Healing",
	"Potion of Mana",
	"Scroll of Golem",
	"Scroll of Search",
	"Scroll of Identify",
	"Scroll of Town Portal",
	"Scroll of Healing",
	"Rune of Fire",
	"Gold",
	"The Undead Crown",
	"Empyrean Band",
	"Ring of Truth",
	"Griswold's Edge",
	"Bovine Plate",

};

TEST(pack, UnPackItem_hellfire)
{
	dvl::ItemStruct id;

	dvl::gbIsHellfire = true;
	dvl::gbIsHellfireSaveGame = true;
	dvl::gbIsMultiplayer = false;

	for (size_t i = 0; i < sizeof(hellfireItems) / sizeof(*hellfireItems); i++) {
		dvl::UnPackItem(&hellfireItems[i], &id);

		ASSERT_STREQ(id._iIName, hellfireItemNames[i]);
	}
}

TEST(pack, UnPackItem_empty)
{
	dvl::PkItemStruct is = { 0, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0 };
	dvl::ItemStruct id;

	dvl::UnPackItem(&is, &id);

	ASSERT_EQ(id._itype, dvl::ITYPE_NONE);
}

TEST(pack, UnPackItem_gold_small)
{
	dvl::PkItemStruct is = { 0, 0, dvl::IDI_GOLD, 0, 0, 0, 0, 0, 1000, 0 };
	dvl::ItemStruct id;

	dvl::UnPackItem(&is, &id);

	ASSERT_EQ(id._iCurs, dvl::ICURS_GOLD_SMALL);
}

TEST(pack, UnPackItem_gold_medium)
{
	dvl::PkItemStruct is = { 0, 0, dvl::IDI_GOLD, 0, 0, 0, 0, 0, 1001, 0 };
	dvl::ItemStruct id;

	dvl::UnPackItem(&is, &id);

	ASSERT_EQ(id._iCurs, dvl::ICURS_GOLD_MEDIUM);
}

TEST(pack, UnPackItem_gold_large)
{
	dvl::PkItemStruct is = { 0, 0, dvl::IDI_GOLD, 0, 0, 0, 0, 0, 2500, 0 };
	dvl::ItemStruct id;

	dvl::UnPackItem(&is, &id);

	ASSERT_EQ(id._iCurs, dvl::ICURS_GOLD_LARGE);
}

TEST(pack, UnPackItem_ear)
{
	dvl::PkItemStruct is = { 1633955154, 17509, 23, 111, 103, 117, 101, 68, 19843, 0 };
	dvl::ItemStruct id;

	dvl::UnPackItem(&is, &id);

	ASSERT_STREQ(id._iName, "Ear of Dead-RogueDM");
	ASSERT_EQ(id._ivalue, 3);
}
