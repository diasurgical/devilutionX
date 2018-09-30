//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
// Tracks which missile files are already loaded
int MissileFileFlag;

int monster_cpp_init_value; // weak
int monstkills[MAXMONSTERS];
int monstactive[MAXMONSTERS];
int nummonsters;
int sgbSaveSoundOn; // weak
MonsterStruct monster[MAXMONSTERS];
int totalmonsters; // weak
CMonster Monsters[16];
// int END_Monsters_17; // weak
int monstimgtot; // weak
int uniquetrans;
int nummtypes;
#endif

const int monster_inf = 0x7F800000; // weak
const char plr2monst[9] = { 0, 5, 3, 7, 1, 4, 6, 0, 2 };
const unsigned char counsmiss[4] = { MIS_FIREBOLT, MIS_CBOLT, MIS_LIGHTCTRL, MIS_FIREBALL };

/* data */

MonsterData monsterdata[112] =
{
	{ 128, 799,  "Monsters\\Zombie\\Zombie%c.CL2",  0, "Monsters\\Zombie\\Zombie%c%i.WAV",  0, 0, NULL,							  { 11, 24, 12, 6,  16, 0  }, { 4, 0, 0, 0, 0, 0 }, "Zombie",				  1,  3,  1,  4,	7,	AI_ZOMBIE,	0,   0, 10,  8,  2,  5,  0,   0,  0,  0,  5,  MC_UNDEAD, 72,  72,  0,	 3, 54	},
	{ 128, 799,  "Monsters\\Zombie\\Zombie%c.CL2",  0, "Monsters\\Zombie\\Zombie%c%i.WAV",  0, 1, "Monsters\\Zombie\\Bluered.TRN",   { 11, 24, 12, 6,  16, 0  }, { 4, 0, 0, 0, 0, 0 }, "Ghoul",				   2,  4,  2,  7,	11,   AI_ZOMBIE,	0,   1, 10,  8,  3,  10, 0,   0,  0,  0,  10, MC_UNDEAD, 72,  72,  0,	 3, 58	},
	{ 128, 799,  "Monsters\\Zombie\\Zombie%c.CL2",  0, "Monsters\\Zombie\\Zombie%c%i.WAV",  0, 1, "Monsters\\Zombie\\Grey.TRN",	  { 11, 24, 12, 6,  16, 0  }, { 4, 0, 0, 0, 0, 0 }, "Rotting Carcass",		 2,  6,  4,  15,   25,   AI_ZOMBIE,	0,   2, 25,  8,  5,  15, 0,   0,  0,  0,  15, MC_UNDEAD, 72,  74,  0,	 3, 136   },
	{ 128, 799,  "Monsters\\Zombie\\Zombie%c.CL2",  0, "Monsters\\Zombie\\Zombie%c%i.WAV",  0, 1, "Monsters\\Zombie\\Yellow.TRN",	{ 11, 24, 12, 6,  16, 0  }, { 4, 0, 0, 0, 0, 0 }, "Black Death",			 4,  8,  6,  25,   40,   AI_ZOMBIE,	0,   3, 30,  8,  6,  22, 0,   0,  0,  0,  20, MC_UNDEAD, 72,  76,  0,	 3, 240   },
	{ 128, 543,  "Monsters\\FalSpear\\Phall%c.CL2", 1, "Monsters\\FalSpear\\Phall%c%i.WAV", 1, 1, "Monsters\\FalSpear\\FallenT.TRN", { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Fallen One",			  1,  3,  1,  1,	4,	AI_FALLEN,	0,   0, 15,  7,  1,  3,  0,   5,  0,  0,  0,  MC_ANIMAL, 0,   0,   0,	 3, 46	},
	{ 128, 543,  "Monsters\\FalSpear\\Phall%c.CL2", 1, "Monsters\\FalSpear\\Phall%c%i.WAV", 1, 1, "Monsters\\FalSpear\\Dark.TRN",	{ 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Carver",				  2,  5,  3,  4,	8,	AI_FALLEN,	0,   2, 20,  7,  2,  5,  0,   5,  0,  0,  5,  MC_ANIMAL, 0,   0,   0,	 3, 80	},
	{ 128, 543,  "Monsters\\FalSpear\\Phall%c.CL2", 1, "Monsters\\FalSpear\\Phall%c%i.WAV", 1, 0, NULL,							  { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Devil Kin",			   3,  7,  5,  12,   24,   AI_FALLEN,	0,   2, 25,  7,  3,  7,  0,   5,  0,  0,  10, MC_ANIMAL, 0,   2,   0,	 3, 155   },
	{ 128, 543,  "Monsters\\FalSpear\\Phall%c.CL2", 1, "Monsters\\FalSpear\\Phall%c%i.WAV", 1, 1, "Monsters\\FalSpear\\Blue.TRN",	{ 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Dark One",				5,  9,  7,  20,   36,   AI_FALLEN,	0,   3, 30,  7,  4,  8,  0,   5,  0,  0,  15, MC_ANIMAL, 64,  68,  0,	 3, 255   },
	{ 128, 553,  "Monsters\\SkelAxe\\SklAx%c.CL2",  1, "Monsters\\SkelAxe\\SklAx%c%i.WAV",  0, 1, "Monsters\\SkelAxe\\White.TRN",	{ 12, 8,  13, 6,  17, 16 }, { 5, 0, 0, 0, 0, 0 }, "Skeleton",				1,  3,  1,  2,	4,	AI_SKELSD,	0,   0, 20,  8,  1,  4,  0,   0,  0,  0,  0,  MC_UNDEAD, 72,  72,  0,	 3, 64	},
	{ 128, 553,  "Monsters\\SkelAxe\\SklAx%c.CL2",  1, "Monsters\\SkelAxe\\SklAx%c%i.WAV",  0, 1, "Monsters\\SkelAxe\\Skelt.TRN",	{ 12, 8,  13, 6,  17, 16 }, { 4, 0, 0, 0, 0, 0 }, "Corpse Axe",			  2,  5,  2,  4,	7,	AI_SKELSD,	0,   1, 25,  8,  3,  5,  0,   0,  0,  0,  0,  MC_UNDEAD, 72,  72,  0,	 3, 68	},
	{ 128, 553,  "Monsters\\SkelAxe\\SklAx%c.CL2",  1, "Monsters\\SkelAxe\\SklAx%c%i.WAV",  0, 0, NULL,							  { 12, 8,  13, 6,  17, 16 }, { 2, 0, 0, 0, 0, 0 }, "Burning Dead",			2,  6,  4,  8,	12,   AI_SKELSD,	0,   2, 30,  8,  3,  7,  0,   0,  0,  0,  5,  MC_UNDEAD, 74,  88,  0,	 3, 154   },
	{ 128, 553,  "Monsters\\SkelAxe\\SklAx%c.CL2",  1, "Monsters\\SkelAxe\\SklAx%c%i.WAV",  0, 1, "Monsters\\SkelAxe\\Black.TRN",	{ 12, 8,  13, 6,  17, 16 }, { 3, 0, 0, 0, 0, 0 }, "Horror",				  4,  8,  6,  12,   20,   AI_SKELSD,	0,   3, 35,  8,  4,  9,  0,   0,  0,  0,  15, MC_UNDEAD, 76,  76,  0,	 3, 264   },
	{ 128, 623,  "Monsters\\FalSword\\Fall%c.CL2",  1, "Monsters\\FalSword\\Fall%c%i.WAV",  1, 1, "Monsters\\FalSword\\FallenT.TRN", { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Fallen One",			  1,  3,  1,  2,	5,	AI_FALLEN,	0,   0, 15,  8,  1,  4,  0,   5,  0,  0,  10, MC_ANIMAL, 0,   0,   0,	 3, 52	},
	{ 128, 623,  "Monsters\\FalSword\\Fall%c.CL2",  1, "Monsters\\FalSword\\Fall%c%i.WAV",  1, 1, "Monsters\\FalSword\\Dark.TRN",	{ 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Carver",				  2,  5,  3,  5,	9,	AI_FALLEN,	0,   1, 20,  8,  2,  7,  0,   5,  0,  0,  15, MC_ANIMAL, 0,   0,   0,	 3, 90	},
	{ 128, 623,  "Monsters\\FalSword\\Fall%c.CL2",  1, "Monsters\\FalSword\\Fall%c%i.WAV",  1, 0, NULL,							  { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Devil Kin",			   3,  7,  5,  16,   24,   AI_FALLEN,	0,   2, 25,  8,  4,  10, 0,   5,  0,  0,  20, MC_ANIMAL, 0,   2,   0,	 3, 180   },
	{ 128, 623,  "Monsters\\FalSword\\Fall%c.CL2",  1, "Monsters\\FalSword\\Fall%c%i.WAV",  1, 1, "Monsters\\FalSword\\Blue.TRN",	{ 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Dark One",				5,  9,  7,  24,   36,   AI_FALLEN,	0,   3, 30,  8,  4,  12, 0,   5,  0,  0,  25, MC_ANIMAL, 64,  68,  0,	 3, 280   },
	{ 128, 410,  "Monsters\\Scav\\Scav%c.CL2",	  1, "Monsters\\Scav\\Scav%c%i.WAV",	  0, 0, NULL,							  { 12, 8,  12, 6,  20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Scavenger",			   1,  4,  2,  3,	6,	AI_SCAV,	  0,   0, 20,  7,  1,  5,  0,   0,  0,  0,  10, MC_ANIMAL, 0,   2,   0,	 3, 80	},
	{ 128, 410,  "Monsters\\Scav\\Scav%c.CL2",	  1, "Monsters\\Scav\\Scav%c%i.WAV",	  0, 1, "Monsters\\Scav\\ScavBr.TRN",	  { 12, 8,  12, 6,  20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Plague Eater",			3,  6,  4,  12,   24,   AI_SCAV,	  0,   1, 30,  7,  1,  8,  0,   0,  0,  0,  20, MC_ANIMAL, 0,   4,   0,	 3, 188   },
	{ 128, 410,  "Monsters\\Scav\\Scav%c.CL2",	  1, "Monsters\\Scav\\Scav%c%i.WAV",	  0, 1, "Monsters\\Scav\\ScavBe.TRN",	  { 12, 8,  12, 6,  20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Shadow Beast",			4,  8,  6,  24,   36,   AI_SCAV,	  0,   2, 35,  7,  3,  12, 0,   0,  0,  0,  25, MC_ANIMAL, 64,  66,  0,	 3, 375   },
	{ 128, 410,  "Monsters\\Scav\\Scav%c.CL2",	  1, "Monsters\\Scav\\Scav%c%i.WAV",	  0, 1, "Monsters\\Scav\\ScavW.TRN",	   { 12, 8,  12, 6,  20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Bone Gasher",			 6,  10, 8,  28,   40,   AI_SCAV,	  0,   3, 35,  7,  5,  15, 0,   0,  0,  0,  30, MC_ANIMAL, 65,  68,  0,	 3, 552   },
	{ 128, 567,  "Monsters\\SkelBow\\SklBw%c.CL2",  1, "Monsters\\SkelBow\\SklBw%c%i.WAV",  0, 1, "Monsters\\SkelBow\\White.TRN",	{ 9,  8,  16, 5,  16, 16 }, { 4, 0, 0, 0, 0, 0 }, "Skeleton",				2,  5,  3,  2,	4,	AI_SKELBOW,   0,   0, 15,  12, 1,  2,  0,   0,  0,  0,  0,  MC_UNDEAD, 72,  72,  0,	 3, 110   },
	{ 128, 567,  "Monsters\\SkelBow\\SklBw%c.CL2",  1, "Monsters\\SkelBow\\SklBw%c%i.WAV",  0, 1, "Monsters\\SkelBow\\Skelt.TRN",	{ 9,  8,  16, 5,  16, 16 }, { 4, 0, 0, 0, 0, 0 }, "Corpse Bow",			  3,  7,  5,  8,	16,   AI_SKELBOW,   0,   1, 25,  12, 1,  4,  0,   0,  0,  0,  0,  MC_UNDEAD, 72,  72,  0,	 3, 210   },
	{ 128, 567,  "Monsters\\SkelBow\\SklBw%c.CL2",  1, "Monsters\\SkelBow\\SklBw%c%i.WAV",  0, 0, NULL,							  { 9,  8,  16, 5,  16, 16 }, { 2, 0, 0, 0, 0, 0 }, "Burning Dead",			5,  9,  7,  10,   24,   AI_SKELBOW,   0,   2, 30,  12, 1,  6,  0,   0,  0,  0,  5,  MC_UNDEAD, 74,  88,  0,	 3, 364   },
	{ 128, 567,  "Monsters\\SkelBow\\SklBw%c.CL2",  1, "Monsters\\SkelBow\\SklBw%c%i.WAV",  0, 1, "Monsters\\SkelBow\\Black.TRN",	{ 9,  8,  16, 5,  16, 16 }, { 3, 0, 0, 0, 0, 0 }, "Horror",				  7,  11, 9,  15,   45,   AI_SKELBOW,   0,   3, 35,  12, 2,  9,  0,   0,  0,  0,  15, MC_UNDEAD, 76,  76,  0,	 3, 594   },
	{ 128, 575,  "Monsters\\SkelSd\\SklSr%c.CL2",   1, "Monsters\\SkelSd\\SklSr%c%i.WAV",   1, 1, "Monsters\\SkelSd\\White.TRN",	 { 13, 8,  12, 7,  15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Skeleton Captain",		1,  4,  2,  3,	6,	AI_SKELSD,	0,   0, 20,  8,  2,  7,  0,   0,  0,  0,  10, MC_UNDEAD, 72,  72,  0,	 3, 90	},
	{ 128, 575,  "Monsters\\SkelSd\\SklSr%c.CL2",   1, "Monsters\\SkelSd\\SklSr%c%i.WAV",   0, 1, "Monsters\\SkelSd\\Skelt.TRN",	 { 13, 8,  12, 7,  15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Corpse Captain",		  2,  6,  4,  12,   20,   AI_SKELSD,	0,   1, 30,  8,  3,  9,  0,   0,  0,  0,  5,  MC_UNDEAD, 72,  72,  0,	 3, 200   },
	{ 128, 575,  "Monsters\\SkelSd\\SklSr%c.CL2",   1, "Monsters\\SkelSd\\SklSr%c%i.WAV",   0, 0, NULL,							  { 13, 8,  12, 7,  15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Burning Dead Captain",	4,  8,  6,  16,   30,   AI_SKELSD,	0,   2, 35,  8,  4,  10, 0,   0,  0,  0,  15, MC_UNDEAD, 74,  88,  0,	 3, 393   },
	{ 128, 575,  "Monsters\\SkelSd\\SklSr%c.CL2",   1, "Monsters\\SkelSd\\SklSr%c%i.WAV",   0, 1, "Monsters\\SkelSd\\Black.TRN",	 { 13, 8,  12, 7,  15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Horror Captain",		  6,  10, 8,  35,   50,   AI_SKELSD,	256, 3, 40,  8,  5,  14, 0,   0,  0,  0,  30, MC_UNDEAD, 76,  76,  0,	 3, 604   },
	{ 128, 2000, "Monsters\\TSneak\\TSneak%c.CL2",  0, "Monsters\\TSneak\\Sneakl%c%i.WAV",  0, 0, NULL,							  { 13, 13, 15, 11, 16, 0  }, { 2, 0, 0, 0, 0, 0 }, "Invisible Lord",		  14, 14, 14, 278,  278,  AI_SKELSD,	256, 3, 65,  8,  16, 30, 0,   0,  0,  0,  60, MC_DEMON,  71,  71,  0,	 3, 2000  },
	{ 128, 992,  "Monsters\\Sneak\\Sneak%c.CL2",	1, "Monsters\\Sneak\\Sneak%c%i.WAV",	0, 0, NULL,							  { 16, 8,  12, 8,  24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Hidden",				  3,  8,  5,  8,	24,   AI_SNEAK,	 1,   0, 35,  8,  3,  6,  0,   0,  0,  0,  25, MC_DEMON,  0,   64,  0,	 3, 278   },
	{ 128, 992,  "Monsters\\Sneak\\Sneak%c.CL2",	1, "Monsters\\Sneak\\Sneak%c%i.WAV",	0, 1, "Monsters\\Sneak\\Sneakv2.TRN",	{ 16, 8,  12, 8,  24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Stalker",				 8,  12, 9,  30,   45,   AI_SNEAK,	 257, 1, 40,  8,  8,  16, 0,   0,  0,  0,  30, MC_DEMON,  0,   64,  0,	 3, 630   },
	{ 128, 992,  "Monsters\\Sneak\\Sneak%c.CL2",	1, "Monsters\\Sneak\\Sneak%c%i.WAV",	0, 1, "Monsters\\Sneak\\Sneakv3.TRN",	{ 16, 8,  12, 8,  24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Unseen",				  10, 14, 11, 35,   50,   AI_SNEAK,	 257, 2, 45,  8,  12, 20, 0,   0,  0,  0,  30, MC_DEMON,  65,  72,  0,	 3, 935   },
	{ 128, 992,  "Monsters\\Sneak\\Sneak%c.CL2",	1, "Monsters\\Sneak\\Sneak%c%i.WAV",	0, 1, "Monsters\\Sneak\\Sneakv1.TRN",	{ 16, 8,  12, 8,  24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Illusion Weaver",		 14, 18, 13, 40,   60,   AI_SNEAK,	 257, 3, 60,  8,  16, 24, 0,   0,  0,  0,  30, MC_DEMON,  3,   74,  0,	 3, 1500  },
	{ 160, 2000, "Monsters\\GoatLord\\GoatL%c.CL2", 0, "Monsters\\GoatLord\\Goatl%c%i.WAV", 0, 0, NULL,							  { 13, 13, 14, 9,  16, 0  }, { 2, 0, 0, 0, 0, 0 }, "Lord Sayter",			 13, 13, 12, 351,  351,  AI_SKELSD,	256, 3, 80,  8,  14, 24, 0,   0,  0,  0,  60, MC_DEMON,  67,  67,  0,	 3, 1500  },
	{ 128, 1030, "Monsters\\GoatMace\\Goat%c.CL2",  1, "Monsters\\GoatMace\\Goat%c%i.WAV",  0, 0, NULL,							  { 12, 8,  12, 6,  20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Flesh Clan",			  6,  10, 8,  30,   45,   AI_GOATMC,	768, 0, 50,  8,  4,  10, 0,   0,  0,  0,  40, MC_DEMON,  0,   0,   0,	 3, 460   },
	{ 128, 1030, "Monsters\\GoatMace\\Goat%c.CL2",  1, "Monsters\\GoatMace\\Goat%c%i.WAV",  0, 1, "Monsters\\GoatMace\\Beige.TRN",   { 12, 8,  12, 6,  20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Stone Clan",			  8,  12, 10, 40,   55,   AI_GOATMC,	768, 1, 60,  8,  6,  12, 0,   0,  0,  0,  40, MC_DEMON,  65,  72,  0,	 3, 685   },
	{ 128, 1030, "Monsters\\GoatMace\\Goat%c.CL2",  1, "Monsters\\GoatMace\\Goat%c%i.WAV",  0, 1, "Monsters\\GoatMace\\Red.TRN",	 { 12, 8,  12, 6,  20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Fire Clan",			   10, 14, 12, 50,   65,   AI_GOATMC,	768, 2, 70,  8,  8,  16, 0,   0,  0,  0,  45, MC_DEMON,  2,   16,  0,	 3, 906   },
	{ 128, 1030, "Monsters\\GoatMace\\Goat%c.CL2",  1, "Monsters\\GoatMace\\Goat%c%i.WAV",  0, 1, "Monsters\\GoatMace\\Gray.TRN",	{ 12, 8,  12, 6,  20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Night Clan",			  12, 16, 14, 55,   70,   AI_GOATMC,	768, 3, 80,  8,  10, 20, 15,  0,  30, 30, 50, MC_DEMON,  65,  72,  0,	 3, 1190  },
	{ 96,  364,  "Monsters\\Bat\\Bat%c.CL2",		0, "Monsters\\Bat\\Bat%c%i.WAV",		0, 1, "Monsters\\Bat\\red.trn",		  { 9,  13, 10, 9,  13, 0  }, { 0, 0, 0, 0, 0, 0 }, "Fiend",				   2,  5,  3,  3,	6,	AI_BAT,	   0,   0, 35,  5,  1,  6,  0,   0,  0,  0,  0,  MC_ANIMAL, 0,   0,   16384, 6, 102   },
	{ 96,  364,  "Monsters\\Bat\\Bat%c.CL2",		0, "Monsters\\Bat\\Bat%c%i.WAV",		0, 0, NULL,							  { 9,  13, 10, 9,  13, 0  }, { 0, 0, 0, 0, 0, 0 }, "Blink",				   5,  9,  7,  12,   28,   AI_BAT,	   0,   1, 45,  5,  1,  8,  0,   0,  0,  0,  15, MC_ANIMAL, 0,   0,   16384, 6, 340   },
	{ 96,  364,  "Monsters\\Bat\\Bat%c.CL2",		0, "Monsters\\Bat\\Bat%c%i.WAV",		0, 1, "Monsters\\Bat\\grey.trn",		 { 9,  13, 10, 9,  13, 0  }, { 0, 0, 0, 0, 0, 0 }, "Gloom",				   7,  11, 9,  28,   36,   AI_BAT,	   256, 2, 70,  5,  4,  12, 0,   0,  0,  0,  35, MC_ANIMAL, 1,   65,  16384, 6, 509   },
	{ 96,  364,  "Monsters\\Bat\\Bat%c.CL2",		0, "Monsters\\Bat\\Bat%c%i.WAV",		0, 1, "Monsters\\Bat\\orange.trn",	   { 9,  13, 10, 9,  13, 0  }, { 0, 0, 0, 0, 0, 0 }, "Familiar",				11, 15, 13, 20,   35,   AI_BAT,	   256, 3, 50,  5,  4,  16, 0,   0,  0,  0,  35, MC_DEMON,  33,  97,  16384, 6, 448   },
	{ 128, 1040, "Monsters\\GoatBow\\GoatB%c.CL2",  0, "Monsters\\GoatBow\\GoatB%c%i.WAV",  0, 0, NULL,							  { 12, 8,  16, 6,  20, 0  }, { 3, 0, 0, 0, 0, 0 }, "Flesh Clan",			  6,  10, 8,  20,   35,   AI_GOATBOW,   512, 0, 35,  13, 1,  7,  0,   0,  0,  0,  35, MC_DEMON,  0,   0,   0,	 3, 448   },
	{ 128, 1040, "Monsters\\GoatBow\\GoatB%c.CL2",  0, "Monsters\\GoatBow\\GoatB%c%i.WAV",  0, 1, "Monsters\\GoatBow\\Beige.TRN",	{ 12, 8,  16, 6,  20, 0  }, { 3, 0, 0, 0, 0, 0 }, "Stone Clan",			  8,  12, 10, 30,   40,   AI_GOATBOW,   512, 1, 40,  13, 2,  9,  0,   0,  0,  0,  35, MC_DEMON,  65,  72,  0,	 3, 645   },
	{ 128, 1040, "Monsters\\GoatBow\\GoatB%c.CL2",  0, "Monsters\\GoatBow\\GoatB%c%i.WAV",  0, 1, "Monsters\\GoatBow\\Red.TRN",	  { 12, 8,  16, 6,  20, 0  }, { 3, 0, 0, 0, 0, 0 }, "Fire Clan",			   10, 14, 12, 40,   50,   AI_GOATBOW,   768, 2, 45,  13, 3,  11, 0,   0,  0,  0,  35, MC_DEMON,  2,   16,  0,	 3, 822   },
	{ 128, 1040, "Monsters\\GoatBow\\GoatB%c.CL2",  0, "Monsters\\GoatBow\\GoatB%c%i.WAV",  0, 1, "Monsters\\GoatBow\\Gray.TRN",	 { 12, 8,  16, 6,  20, 0  }, { 3, 0, 0, 0, 0, 0 }, "Night Clan",			  12, 16, 14, 50,   65,   AI_GOATBOW,   768, 3, 50,  13, 4,  13, 15,  0,  0,  0,  40, MC_DEMON,  65,  72,  0,	 3, 1092  },
	{ 128, 716,  "Monsters\\Acid\\Acid%c.CL2",	  1, "Monsters\\Acid\\Acid%c%i.WAV",	  1, 0, NULL,							  { 13, 8,  12, 8,  16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Acid Beast",			  10, 14, 11, 40,   66,   AI_ACID,	  0,   0, 40,  8,  4,  12, 25,  8,  0,  0,  30, MC_ANIMAL, 128, 136, 0,	 3, 846   },
	{ 128, 716,  "Monsters\\Acid\\Acid%c.CL2",	  1, "Monsters\\Acid\\Acid%c%i.WAV",	  1, 1, "Monsters\\Acid\\AcidBlk.TRN",	 { 13, 8,  12, 8,  16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Poison Spitter",		  14, 18, 15, 60,   85,   AI_ACID,	  0,   1, 45,  8,  4,  16, 25,  8,  0,  0,  30, MC_ANIMAL, 128, 136, 0,	 3, 1248  },
	{ 128, 716,  "Monsters\\Acid\\Acid%c.CL2",	  1, "Monsters\\Acid\\Acid%c%i.WAV",	  1, 1, "Monsters\\Acid\\AcidB.TRN",	   { 13, 8,  12, 8,  16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Pit Beast",			   18, 22, 21, 80,   110,  AI_ACID,	  0,   2, 55,  8,  8,  18, 35,  8,  0,  0,  35, MC_ANIMAL, 129, 140, 0,	 3, 2060  },
	{ 128, 716,  "Monsters\\Acid\\Acid%c.CL2",	  1, "Monsters\\Acid\\Acid%c%i.WAV",	  1, 1, "Monsters\\Acid\\AcidR.TRN",	   { 13, 8,  12, 8,  16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Lava Maw",				22, 27, 25, 100,  150,  AI_ACID,	  0,   3, 65,  8,  10, 20, 40,  8,  0,  0,  35, MC_ANIMAL, 145, 152, 0,	 3, 2940  },
	{ 160, 1010, "Monsters\\SKing\\SKing%c.CL2",	1, "Monsters\\SKing\\SKing%c%i.WAV",	1, 1, "Monsters\\SkelAxe\\White.TRN",	{ 8,  6,  16, 6,  16, 6  }, { 2, 0, 0, 0, 0, 2 }, "Skeleton King",		   6,  6,  9,  140,  140,  AI_SKELKING,  768, 3, 60,  8,  6,  16, 0,   0,  0,  0,  70, MC_UNDEAD, 78,  120, 32769, 7, 570   },
	{ 128, 980,  "Monsters\\FatC\\FatC%c.CL2",	  0, "Monsters\\FatC\\FatC%c%i.WAV",	  0, 0, NULL,							  { 10, 8,  12, 6,  16, 0  }, { 1, 0, 0, 0, 0, 0 }, "The Butcher",			 0,  0,  1,  320,  320,  AI_CLEAVER,   0,   3, 50,  8,  6,  12, 0,   0,  0,  0,  50, MC_DEMON,  6,   49,  32768, 3, 710   },
	{ 128, 1130, "Monsters\\Fat\\Fat%c.CL2",		1, "Monsters\\Fat\\Fat%c%i.WAV",		0, 0, NULL,							  { 8,  10, 15, 6,  16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Overlord",				8,  12, 10, 60,   80,   AI_FAT,	   0,   0, 55,  8,  6,  12, 0,   0,  0,  0,  55, MC_DEMON,  0,   2,   0,	 3, 635   },
	{ 128, 1130, "Monsters\\Fat\\Fat%c.CL2",		1, "Monsters\\Fat\\Fat%c%i.WAV",		0, 1, "Monsters\\Fat\\Blue.TRN",		 { 8,  10, 15, 6,  16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Mud Man",				 13, 17, 14, 100,  125,  AI_FAT,	   256, 1, 60,  8,  8,  16, 0,   0,  0,  0,  60, MC_DEMON,  0,   32,  0,	 3, 1165  },
	{ 128, 1130, "Monsters\\Fat\\Fat%c.CL2",		1, "Monsters\\Fat\\Fat%c%i.WAV",		0, 1, "Monsters\\Fat\\FatB.TRN",		 { 8,  10, 15, 6,  16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Toad Demon",			  15, 19, 16, 135,  160,  AI_FAT,	   256, 2, 70,  8,  8,  16, 40,  0,  8,  20, 65, MC_DEMON,  8,   12,  0,	 3, 1380  },
	{ 128, 1130, "Monsters\\Fat\\Fat%c.CL2",		1, "Monsters\\Fat\\Fat%c%i.WAV",		0, 1, "Monsters\\Fat\\FatF.TRN",		 { 8,  10, 15, 6,  16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Flayed One",			  19, 23, 20, 160,  200,  AI_FAT,	   256, 3, 85,  8,  10, 20, 0,   0,  0,  0,  70, MC_DEMON,  17,  24,  0,	 3, 2058  },
	{ 160, 2420, "Monsters\\Worm\\Worm%c.CL2",	  0, "Monsters\\Fat\\Fat%c%i.WAV",		0, 0, NULL,							  { 13, 13, 13, 11, 19, 0  }, { 0, 0, 0, 0, 0, 0 }, "Wyrm",					9,  13, 11, 60,   90,   AI_SKELSD,	0,   0, 40,  8,  4,  10, 0,   0,  0,  0,  25, MC_ANIMAL, 1,   1,   0,	 3, 660   },
	{ 160, 2420, "Monsters\\Worm\\Worm%c.CL2",	  0, "Monsters\\Fat\\Fat%c%i.WAV",		0, 0, NULL,							  { 13, 13, 13, 11, 19, 0  }, { 0, 0, 0, 0, 0, 0 }, "Cave Slug",			   11, 15, 13, 75,   110,  AI_SKELSD,	0,   1, 50,  8,  6,  13, 0,   0,  0,  0,  30, MC_ANIMAL, 1,   1,   0,	 3, 994   },
	{ 160, 2420, "Monsters\\Worm\\Worm%c.CL2",	  0, "Monsters\\Fat\\Fat%c%i.WAV",		0, 0, NULL,							  { 13, 13, 13, 11, 19, 0  }, { 0, 0, 0, 0, 0, 0 }, "Devil Wyrm",			  13, 17, 15, 100,  140,  AI_SKELSD,	0,   2, 55,  8,  8,  16, 0,   0,  0,  0,  30, MC_ANIMAL, 3,   3,   0,	 3, 1320  },
	{ 160, 2420, "Monsters\\Worm\\Worm%c.CL2",	  0, "Monsters\\Fat\\Fat%c%i.WAV",		0, 0, NULL,							  { 13, 13, 13, 11, 19, 0  }, { 0, 0, 0, 0, 0, 0 }, "Devourer",				15, 19, 17, 125,  200,  AI_SKELSD,	0,   3, 60,  8,  10, 20, 0,   0,  0,  0,  35, MC_ANIMAL, 67,  67,  0,	 3, 1827  },
	{ 128, 1680, "Monsters\\Magma\\Magma%c.CL2",	1, "Monsters\\Magma\\Magma%c%i.WAV",	1, 0, NULL,							  { 8,  10, 14, 7,  18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Magma Demon",			 14, 17, 13, 50,   70,   AI_MAGMA,	 768, 0, 45,  4,  2,  10, 50,  13, 0,  0,  45, MC_DEMON,  10,  24,  0,	 7, 1076  },
	{ 128, 1680, "Monsters\\Magma\\Magma%c.CL2",	1, "Monsters\\Magma\\Magma%c%i.WAV",	1, 1, "Monsters\\Magma\\Yellow.TRN",	 { 8,  10, 14, 7,  18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Blood Stone",			 15, 19, 14, 55,   75,   AI_MAGMA,	 768, 1, 50,  4,  2,  12, 50,  14, 0,  0,  45, MC_DEMON,  24,  24,  0,	 7, 1309  },
	{ 128, 1680, "Monsters\\Magma\\Magma%c.CL2",	1, "Monsters\\Magma\\Magma%c%i.WAV",	1, 1, "Monsters\\Magma\\Blue.TRN",	   { 8,  10, 14, 7,  18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Hell Stone",			  16, 20, 16, 60,   80,   AI_MAGMA,	 768, 2, 60,  4,  2,  20, 60,  14, 0,  0,  50, MC_DEMON,  24,  24,  0,	 7, 1680  },
	{ 128, 1680, "Monsters\\Magma\\Magma%c.CL2",	1, "Monsters\\Magma\\Magma%c%i.WAV",	1, 1, "Monsters\\Magma\\Wierd.TRN",	  { 8,  10, 14, 7,  18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Lava Lord",			   17, 21, 18, 70,   85,   AI_MAGMA,	 768, 3, 75,  4,  4,  24, 60,  14, 0,  0,  60, MC_DEMON,  24,  24,  0,	 7, 2124  },
	{ 160, 1630, "Monsters\\Rhino\\Rhino%c.CL2",	1, "Monsters\\Rhino\\Rhino%c%i.WAV",	1, 0, NULL,							  { 8,  8,  14, 6,  16, 6  }, { 2, 0, 0, 0, 0, 0 }, "Horned Demon",			12, 16, 13, 40,   80,   AI_RHINO,	 768, 0, 60,  7,  2,  16, 100, 0,  5,  32, 40, MC_ANIMAL, 0,   2,   0,	 7, 1172  },
	{ 160, 1630, "Monsters\\Rhino\\Rhino%c.CL2",	1, "Monsters\\Rhino\\Rhino%c%i.WAV",	1, 1, "Monsters\\Rhino\\Orange.TRN",	 { 8,  8,  14, 6,  16, 6  }, { 2, 0, 0, 0, 0, 0 }, "Mud Runner",			  14, 18, 15, 50,   90,   AI_RHINO,	 768, 1, 70,  7,  6,  18, 100, 0,  12, 36, 45, MC_ANIMAL, 0,   2,   0,	 7, 1404  },
	{ 160, 1630, "Monsters\\Rhino\\Rhino%c.CL2",	1, "Monsters\\Rhino\\Rhino%c%i.WAV",	1, 1, "Monsters\\Rhino\\Blue.TRN",	   { 8,  8,  14, 6,  16, 6  }, { 2, 0, 0, 0, 0, 0 }, "Frost Charger",		   16, 20, 17, 60,   100,  AI_RHINO,	 768, 2, 80,  7,  8,  20, 100, 0,  20, 40, 50, MC_ANIMAL, 12,  12,  0,	 7, 1720  },
	{ 160, 1630, "Monsters\\Rhino\\Rhino%c.CL2",	1, "Monsters\\Rhino\\Rhino%c%i.WAV",	1, 1, "Monsters\\Rhino\\RhinoB.TRN",	 { 8,  8,  14, 6,  16, 6  }, { 2, 0, 0, 0, 0, 0 }, "Obsidian Lord",		   18, 22, 19, 70,   110,  AI_RHINO,	 768, 3, 90,  7,  10, 22, 100, 0,  20, 50, 55, MC_ANIMAL, 12,  56,  0,	 7, 1809  },
	{ 128, 1740, "Monsters\\Demskel\\Demskl%c.CL2", 1, "Monsters\\Thin\\Thin%c%i.WAV",	  1, 0, "Monsters\\Thin\\Thinv3.TRN",	  { 10, 8,  20, 6,  24, 16 }, { 3, 0, 0, 0, 0, 0 }, "Bone Demon",			  10, 14, 12, 70,   70,   AI_STORM,	 0,   0, 60,  8,  6,  14, 12,  0,  0,  0,  50, MC_DEMON,  72,  72,  0,	 7, 1344  },
	{ 160, 1740, "Monsters\\Thin\\Thin%c.CL2",	  1, "Monsters\\Thin\\Thin%c%i.WAV",	  1, 1, "Monsters\\Thin\\Thinv3.TRN",	  { 8,  8,  18, 4,  17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Red Death",			   14, 18, 16, 96,   96,   AI_STORM,	 0,   1, 75,  5,  10, 20, 0,   0,  0,  0,  60, MC_DEMON,  24,  24,  0,	 7, 2168  },
	{ 160, 1740, "Monsters\\Thin\\Thin%c.CL2",	  1, "Monsters\\Thin\\Thin%c%i.WAV",	  1, 1, "Monsters\\Thin\\Thinv3.TRN",	  { 8,  8,  18, 4,  17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Litch Demon",			 16, 20, 18, 110,  110,  AI_STORM,	 0,   2, 80,  5,  10, 24, 0,   0,  0,  0,  45, MC_DEMON,  104, 104, 0,	 7, 2736  },
	{ 160, 1740, "Monsters\\Thin\\Thin%c.CL2",	  1, "Monsters\\Thin\\Thin%c%i.WAV",	  1, 1, "Monsters\\Thin\\Thinv3.TRN",	  { 8,  8,  18, 4,  17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Undead Balrog",		   20, 24, 22, 130,  130,  AI_STORM,	 0,   3, 85,  5,  12, 30, 0,   0,  0,  0,  65, MC_DEMON,  78,  78,  0,	 7, 3575  },
	{ 128, 1460, "Monsters\\Fireman\\FireM%c.CL2",  1, "Monsters\\Acid\\Acid%c%i.WAV",	  0, 0, NULL,							  { 14, 19, 20, 8,  14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Incinerator",			 14, 18, 16, 30,   45,   AI_FIREMAN,   0,   0, 75,  8,  8,  16, 0,   0,  0,  0,  25, MC_DEMON,  24,  24,  0,	 3, 1888  },
	{ 128, 1460, "Monsters\\Fireman\\FireM%c.CL2",  1, "Monsters\\Acid\\Acid%c%i.WAV",	  0, 0, NULL,							  { 14, 19, 20, 8,  14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Flame Lord",			  16, 20, 18, 40,   55,   AI_FIREMAN,   0,   1, 75,  8,  10, 20, 0,   0,  0,  0,  25, MC_DEMON,  24,  24,  0,	 3, 2250  },
	{ 128, 1460, "Monsters\\Fireman\\FireM%c.CL2",  1, "Monsters\\Acid\\Acid%c%i.WAV",	  0, 0, NULL,							  { 14, 19, 20, 8,  14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Doom Fire",			   18, 22, 20, 50,   65,   AI_FIREMAN,   0,   2, 80,  8,  12, 24, 0,   0,  0,  0,  30, MC_DEMON,  28,  28,  0,	 3, 2740  },
	{ 128, 1460, "Monsters\\Fireman\\FireM%c.CL2",  1, "Monsters\\Acid\\Acid%c%i.WAV",	  0, 0, NULL,							  { 14, 19, 20, 8,  14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Hell Burner",			 20, 24, 22, 60,   80,   AI_FIREMAN,   0,   3, 85,  8,  15, 30, 0,   0,  0,  0,  30, MC_DEMON,  28,  28,  0,	 3, 3355  },
	{ 160, 1740, "Monsters\\Thin\\Thin%c.CL2",	  1, "Monsters\\Thin\\Thin%c%i.WAV",	  1, 1, "Monsters\\Thin\\Thinv3.TRN",	  { 8,  8,  18, 4,  17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Red Storm",			   17, 21, 18, 55,   110,  AI_STORM,	 768, 0, 80,  5,  8,  18, 75,  8,  4,  16, 30, MC_DEMON,  12,  40,  0,	 7, 2160  },
	{ 160, 1740, "Monsters\\Thin\\Thin%c.CL2",	  1, "Monsters\\Thin\\Thin%c%i.WAV",	  1, 0, NULL,							  { 8,  8,  18, 4,  17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Storm Rider",			 19, 23, 20, 60,   120,  AI_STORM,	 768, 1, 80,  5,  8,  18, 80,  8,  4,  16, 30, MC_DEMON,  33,  40,  0,	 7, 2391  },
	{ 160, 1740, "Monsters\\Thin\\Thin%c.CL2",	  1, "Monsters\\Thin\\Thin%c%i.WAV",	  1, 1, "Monsters\\Thin\\Thinv2.TRN",	  { 8,  8,  18, 4,  17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Storm Lord",			  21, 25, 22, 75,   135,  AI_STORM,	 768, 2, 85,  5,  12, 24, 75,  8,  4,  16, 35, MC_DEMON,  33,  40,  0,	 7, 2775  },
	{ 160, 1740, "Monsters\\Thin\\Thin%c.CL2",	  1, "Monsters\\Thin\\Thin%c%i.WAV",	  1, 1, "Monsters\\Thin\\Thinv1.TRN",	  { 8,  8,  18, 4,  17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Maelstorm",			   23, 27, 24, 90,   150,  AI_STORM,	 768, 3, 90,  5,  12, 28, 75,  8,  4,  16, 40, MC_DEMON,  97,  104, 0,	 7, 3177  },
	{ 128, 1650, "Monsters\\BigFall\\Fallg%c.CL2",  1, "Monsters\\BigFall\\Bfal%c%i.WAV",   0, 0, NULL,							  { 10, 8,  11, 8,  17, 0  }, { 0, 0, 0, 0, 2, 2 }, "Devil Kin Brute",		 20, 20, 24, 160,  220,  AI_SKELSD,	768, 3, 100, 6,  18, 24, 0,   0,  0,  0,  75, MC_ANIMAL, 0,   0,   0,	 6, 2000  },
	{ 160, 1650, "Monsters\\Gargoyle\\Gargo%c.CL2", 1, "Monsters\\Gargoyle\\Gargo%c%i.WAV", 0, 0, NULL,							  { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 2 }, "Winged-Demon",			8,  12, 9,  45,   60,   AI_GARG,	  512, 0, 50,  7,  10, 16, 0,   0,  0,  0,  45, MC_DEMON,  74,  88,  0,	 6, 662   },
	{ 160, 1650, "Monsters\\Gargoyle\\Gargo%c.CL2", 1, "Monsters\\Gargoyle\\Gargo%c%i.WAV", 0, 1, "Monsters\\Gargoyle\\GarE.TRN",	{ 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 2 }, "Gargoyle",				12, 16, 13, 60,   90,   AI_GARG,	  512, 1, 65,  7,  10, 16, 0,   0,  0,  0,  45, MC_DEMON,  76,  104, 0,	 6, 1205  },
	{ 160, 1650, "Monsters\\Gargoyle\\Gargo%c.CL2", 1, "Monsters\\Gargoyle\\Gargo%c%i.WAV", 0, 1, "Monsters\\Gargoyle\\GargBr.TRN",  { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 0 }, "Blood Claw",			  16, 20, 19, 75,   125,  AI_GARG,	  512, 2, 80,  7,  14, 22, 0,   0,  0,  0,  50, MC_DEMON,  88,  92,  0,	 6, 1873  },
	{ 160, 1650, "Monsters\\Gargoyle\\Gargo%c.CL2", 1, "Monsters\\Gargoyle\\Gargo%c%i.WAV", 0, 1, "Monsters\\Gargoyle\\GargB.TRN",   { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 0 }, "Death Wing",			  18, 22, 23, 90,   150,  AI_GARG,	  512, 3, 95,  7,  16, 28, 0,   0,  0,  0,  60, MC_DEMON,  104, 106, 0,	 6, 2278  },
	{ 160, 2220, "Monsters\\Mega\\Mega%c.CL2",	  1, "Monsters\\Mega\\Mega%c%i.WAV",	  1, 0, NULL,							  { 6,  7,  14, 1,  24, 5  }, { 3, 0, 0, 0, 2, 0 }, "Slayer",				  19, 23, 20, 120,  140,  AI_MEGA,	  768, 0, 100, 8,  12, 20, 0,   3,  0,  0,  60, MC_DEMON,  17,  17,  0,	 7, 2300  },
	{ 160, 2220, "Monsters\\Mega\\Mega%c.CL2",	  1, "Monsters\\Mega\\Mega%c%i.WAV",	  1, 1, "Monsters\\Mega\\Guard.TRN",	   { 6,  7,  14, 1,  24, 5  }, { 3, 0, 0, 0, 2, 0 }, "Guardian",				21, 25, 22, 140,  160,  AI_MEGA,	  768, 1, 110, 8,  14, 22, 0,   3,  0,  0,  65, MC_DEMON,  17,  17,  0,	 7, 2714  },
	{ 160, 2220, "Monsters\\Mega\\Mega%c.CL2",	  1, "Monsters\\Mega\\Mega%c%i.WAV",	  1, 1, "Monsters\\Mega\\Vtexl.TRN",	   { 6,  7,  14, 1,  24, 5  }, { 3, 0, 0, 0, 2, 0 }, "Vortex Lord",			 23, 26, 24, 160,  180,  AI_MEGA,	  768, 2, 120, 8,  18, 24, 0,   3,  0,  0,  70, MC_DEMON,  81,  85,  0,	 7, 3252  },
	{ 160, 2220, "Monsters\\Mega\\Mega%c.CL2",	  1, "Monsters\\Mega\\Mega%c%i.WAV",	  1, 1, "Monsters\\Mega\\Balr.TRN",		{ 6,  7,  14, 1,  24, 5  }, { 3, 0, 0, 0, 2, 0 }, "Balrog",				  25, 29, 26, 180,  200,  AI_MEGA,	  768, 3, 130, 8,  22, 30, 0,   3,  0,  0,  75, MC_DEMON,  81,  85,  0,	 7, 3643  },
	{ 160, 1270, "Monsters\\Snake\\Snake%c.CL2",	0, "Monsters\\Snake\\Snake%c%i.WAV",	0, 0, NULL,							  { 12, 11, 13, 5,  18, 0  }, { 2, 0, 0, 0, 1, 0 }, "Cave Viper",			  20, 24, 21, 100,  150,  AI_SNAKE,	 256, 0, 90,  8,  8,  20, 0,   0,  0,  0,  60, MC_DEMON,  8,   8,   0,	 7, 2725  },
	{ 160, 1270, "Monsters\\Snake\\Snake%c.CL2",	0, "Monsters\\Snake\\Snake%c%i.WAV",	0, 1, "Monsters\\Snake\\SnakR.TRN",	  { 12, 11, 13, 5,  18, 0  }, { 2, 0, 0, 0, 1, 0 }, "Fire Drake",			  22, 26, 23, 120,  170,  AI_SNAKE,	 256, 1, 105, 8,  12, 24, 0,   0,  0,  0,  65, MC_DEMON,  10,  24,  0,	 7, 3139  },
	{ 160, 1270, "Monsters\\Snake\\Snake%c.CL2",	0, "Monsters\\Snake\\Snake%c%i.WAV",	0, 1, "Monsters\\Snake\\Snakg.TRN",	  { 12, 11, 13, 5,  18, 0  }, { 2, 0, 0, 0, 1, 0 }, "Gold Viper",			  24, 27, 25, 140,  180,  AI_SNAKE,	 256, 2, 120, 8,  15, 26, 0,   0,  0,  0,  70, MC_DEMON,  12,  12,  0,	 7, 3540  },
	{ 160, 1270, "Monsters\\Snake\\Snake%c.CL2",	0, "Monsters\\Snake\\Snake%c%i.WAV",	0, 1, "Monsters\\Snake\\Snakb.TRN",	  { 12, 11, 13, 5,  18, 0  }, { 2, 0, 0, 0, 1, 0 }, "Azure Drake",			 28, 30, 27, 160,  200,  AI_SNAKE,	 256, 3, 130, 8,  18, 30, 0,   0,  0,  0,  75, MC_DEMON,  6,   42,  0,	 7, 3791  },
	{ 160, 2120, "Monsters\\Black\\Black%c.CL2",	0, "Monsters\\Black\\Black%c%i.WAV",	0, 0, NULL,							  { 8,  8,  16, 4,  24, 0  }, { 2, 0, 0, 0, 0, 0 }, "Black Knight",			23, 27, 24, 150,  150,  AI_SKELSD,	256, 0, 110, 8,  15, 20, 0,   0,  0,  0,  75, MC_DEMON,  69,  97,  0,	 7, 3360  },
	{ 160, 2120, "Monsters\\Black\\Black%c.CL2",	0, "Monsters\\Black\\Black%c%i.WAV",	0, 1, "Monsters\\Black\\BlkKntRT.TRN",   { 8,  8,  16, 4,  24, 0  }, { 2, 0, 0, 0, 0, 0 }, "Doom Guard",			  25, 29, 26, 165,  165,  AI_SKELSD,	256, 0, 130, 8,  18, 25, 0,   0,  0,  0,  75, MC_DEMON,  67,  81,  0,	 7, 3650  },
	{ 160, 2120, "Monsters\\Black\\Black%c.CL2",	0, "Monsters\\Black\\Black%c%i.WAV",	0, 1, "Monsters\\Black\\BlkKntBT.TRN",   { 8,  8,  16, 4,  24, 0  }, { 2, 0, 0, 0, 0, 0 }, "Steel Lord",			  27, 30, 28, 180,  180,  AI_SKELSD,	256, 1, 120, 8,  20, 30, 0,   0,  0,  0,  80, MC_DEMON,  85,  92,  0,	 7, 4252  },
	{ 160, 2120, "Monsters\\Black\\Black%c.CL2",	0, "Monsters\\Black\\Black%c%i.WAV",	0, 1, "Monsters\\Black\\BlkKntBe.TRN",   { 8,  8,  16, 4,  24, 0  }, { 2, 0, 0, 0, 0, 0 }, "Blood Knight",			24, 26, 30, 200,  200,  AI_SKELSD,	256, 1, 130, 8,  25, 35, 0,   0,  0,  0,  85, MC_DEMON,  106, 106, 0,	 7, 5130  },
	{ 96,  484,  "Monsters\\Unrav\\Unrav%c.CL2",	0, "Monsters\\Acid\\Acid%c%i.WAV",	  0, 0, NULL,							  { 10, 10, 12, 5,  16, 0  }, { 0, 0, 0, 0, 0, 0 }, "Unraveler",			   26, 28, 25, 70,   150,  AI_SKELSD,	0,   0, 75,  7,  10, 20, 0,   0,  0,  0,  70, MC_UNDEAD, 106, 106, 0,	 3, 3812  },
	{ 96,  484,  "Monsters\\Unrav\\Unrav%c.CL2",	0, "Monsters\\Acid\\Acid%c%i.WAV",	  0, 0, NULL,							  { 10, 10, 12, 5,  16, 0  }, { 0, 0, 0, 0, 0, 0 }, "Hollow One",			  28, 30, 27, 135,  240,  AI_SKELSD,	0,   1, 75,  7,  12, 24, 0,   0,  0,  0,  75, MC_UNDEAD, 92,  92,  0,	 3, 4374  },
	{ 96,  484,  "Monsters\\Unrav\\Unrav%c.CL2",	0, "Monsters\\Acid\\Acid%c%i.WAV",	  0, 0, NULL,							  { 10, 10, 12, 5,  16, 0  }, { 0, 0, 0, 0, 0, 0 }, "Pain Master",			 27, 30, 29, 110,  200,  AI_SKELSD,	0,   2, 80,  7,  16, 30, 0,   0,  0,  0,  80, MC_UNDEAD, 92,  92,  0,	 3, 5147  },
	{ 96,  484,  "Monsters\\Unrav\\Unrav%c.CL2",	0, "Monsters\\Acid\\Acid%c%i.WAV",	  0, 0, NULL,							  { 10, 10, 12, 5,  16, 0  }, { 0, 0, 0, 0, 0, 0 }, "Reality Weaver",		  28, 30, 30, 135,  240,  AI_SKELSD,	0,   3, 85,  7,  20, 35, 0,   0,  0,  0,  85, MC_UNDEAD, 113, 113, 0,	 3, 5925  },
	{ 128, 980,  "Monsters\\Succ\\Scbs%c.CL2",	  0, "Monsters\\Succ\\Scbs%c%i.WAV",	  0, 0, NULL,							  { 14, 8,  16, 7,  24, 0  }, { 0, 0, 0, 0, 0, 0 }, "Succubus",				22, 26, 24, 120,  150,  AI_SUCC,	  512, 0, 100, 10, 1,  20, 0,   0,  0,  0,  60, MC_DEMON,  1,   10,  0,	 3, 3696  },
	{ 128, 980,  "Monsters\\Succ\\Scbs%c.CL2",	  0, "Monsters\\Succ\\Scbs%c%i.WAV",	  0, 1, "Monsters\\Succ\\Succb.TRN",	   { 14, 8,  16, 7,  24, 0  }, { 0, 0, 0, 0, 0, 0 }, "Snow Witch",			  25, 28, 26, 135,  175,  AI_SUCC,	  512, 1, 110, 10, 1,  24, 0,   0,  0,  0,  65, MC_DEMON,  68,  76,  0,	 3, 4084  },
	{ 128, 980,  "Monsters\\Succ\\Scbs%c.CL2",	  0, "Monsters\\Succ\\Scbs%c%i.WAV",	  0, 1, "Monsters\\Succ\\Succrw.TRN",	  { 14, 8,  16, 7,  24, 0  }, { 0, 0, 0, 0, 0, 0 }, "Hell Spawn",			  27, 30, 28, 150,  200,  AI_SUCC,	  768, 2, 115, 10, 1,  30, 0,   0,  0,  0,  75, MC_DEMON,  33,  28,  0,	 3, 4480  },
	{ 128, 980,  "Monsters\\Succ\\Scbs%c.CL2",	  0, "Monsters\\Succ\\Scbs%c%i.WAV",	  0, 1, "Monsters\\Succ\\Succbw.TRN",	  { 14, 8,  16, 7,  24, 0  }, { 0, 0, 0, 0, 0, 0 }, "Soul Burner",			 28, 30, 30, 140,  225,  AI_SUCC,	  768, 3, 120, 10, 1,  35, 0,   0,  0,  0,  85, MC_DEMON,  21,  56,  0,	 3, 4644  },
	{ 128, 2000, "Monsters\\Mage\\Mage%c.CL2",	  1, "Monsters\\Mage\\Mage%c%i.WAV",	  0, 0, NULL,							  { 12, 1,  20, 8,  28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Counselor",			   24, 26, 25, 70,   70,   AI_COUNSLR,   512, 0, 90,  8,  8,  20, 0,   0,  0,  0,  0,  MC_DEMON,  7,   7,   0,	 7, 4070  },
	{ 128, 2000, "Monsters\\Mage\\Mage%c.CL2",	  1, "Monsters\\Mage\\Mage%c%i.WAV",	  0, 1, "Monsters\\Mage\\Cnselg.TRN",	  { 12, 1,  20, 8,  28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Magistrate",			  26, 28, 27, 85,   85,   AI_COUNSLR,   512, 1, 100, 8,  10, 24, 0,   0,  0,  0,  0,  MC_DEMON,  85,  92,  0,	 7, 4478  },
	{ 128, 2000, "Monsters\\Mage\\Mage%c.CL2",	  1, "Monsters\\Mage\\Mage%c%i.WAV",	  0, 1, "Monsters\\Mage\\Cnselgd.TRN",	 { 12, 1,  20, 8,  28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Cabalist",				28, 30, 29, 120,  120,  AI_COUNSLR,   512, 2, 110, 8,  14, 30, 0,   0,  0,  0,  0,  MC_DEMON,  99,  106, 0,	 7, 4929  },
	{ 128, 2000, "Monsters\\Mage\\Mage%c.CL2",	  1, "Monsters\\Mage\\Mage%c%i.WAV",	  0, 1, "Monsters\\Mage\\Cnselbk.TRN",	 { 12, 1,  20, 8,  28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Advocate",				30, 30, 30, 145,  145,  AI_COUNSLR,   512, 3, 120, 8,  15, 25, 0,   0,  0,  0,  0,  MC_DEMON,  106, 120, 0,	 7, 4968  },
	{ 96,  386,  "Monsters\\Golem\\Golem%c.CL2",	1, "Monsters\\Golem\\Golm%c%i.WAV",	 0, 0, NULL,							  { 0,  16, 12, 0,  12, 20 }, { 0, 0, 0, 0, 0, 0 }, "Golem",				   0,  0,  12, 1,	1,	AI_GOLUM,	 512, 0, 0,   7,  1,  1,  0,   0,  0,  0,  1,  MC_DEMON,  0,   0,   0,	 0, 0	 },
	{ 160, 2000, "Monsters\\Diablo\\Diablo%c.CL2",  1, "Monsters\\Diablo\\Diablo%c%i.WAV",  1, 0, NULL,							  { 16, 6,  16, 6,  16, 16 }, { 0, 0, 0, 0, 0, 0 }, "The Dark Lord",		   50, 50, 30, 1666, 1666, AI_DIABLO,	896, 3, 220, 4,  30, 60, 0,   11, 0,  0,  70, MC_DEMON,  78,  78,  0,	 7, 31666 },
	{ 128, 1060, "Monsters\\DarkMage\\Dmage%c.CL2", 1, "Monsters\\DarkMage\\Dmag%c%i.WAV",  0, 0, NULL,							  { 6,  1,  21, 6,  23, 18 }, { 0, 0, 0, 0, 0, 0 }, "The Arch-Litch Malignus", 30, 30, 30, 160,  160,  AI_COUNSLR,   512, 3, 120, 8,  20, 40, 0,   0,  0,  0,  70, MC_DEMON,  71,  120, 0,	 7, 4968  }
};
char MonstConvTbl[128] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 29, 30,
	31, 32, 34, 35, 36, 37, 38, 40, 39, 41,
	42, 43, 44, 45, 46, 47, 48, 49, 50, 52,
	53, 54, 55, 56, 57, 59, 58, 60, 61, 62,
	63, 64, 65, 66, 67, 68, 69, 70, 71, 0,
	0, 0, 0, 72, 73, 74, 75, 0, 0, 0,
	0, 77, 76, 78, 79, 81, 82, 83, 84, 85,
	86, 87, 88, 89, 90, 92, 91, 93, 94, 95,
	96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
	106, 107, 108, 0, 110, 0, 109, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 80, 111
};

unsigned char MonstAvailTbl[112] =
{
	1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	1,   1,   1,   1,   1,   1,   1,   1,   0,   2,
	2,   2,   2,   0,   2,   2,   2,   2,   1,   1,
	1,   1,   2,   2,   2,   2,   2,   2,   2,   2,
	0,   0,   2,   2,   2,   2,   0,   0,   0,   0,
	2,   2,   2,   2,   2,   2,   2,   2,   0,   0,
	0,   0,   0,   0,   0,   0,   2,   2,   2,   2,
	0,   2,   2,   2,   2,   2,   2,   2,   2,   2,
	2,   2,   2,   2,   2,   2,   2,   0,   0,   0,
	0,   2,   2,   2,   2,   2,   2,   2,   2,   0,
	0,   0
};
UniqMonstStruct UniqMonst[98] =
{
	{ MT_NGOATMC,  "Gharbad the Weak",		 "BSDB",	4,  120,  AI_GARBUD,   3, 8,  16, 96,  0,  0,   0, QUEST_GARBUD1  },
	{ MT_SKING,	"Skeleton King",			"GENRL",   0,  240,  AI_SKELKING, 3, 6,  16, 78,  1,  0,   0, 0			  },
	{ MT_COUNSLR,  "Zhar the Mad",			 "GENERAL", 8,  360,  AI_ZHAR,	 3, 16, 40, 14,  0,  0,   0, QUEST_ZHAR1	},
	{ MT_BFALLSP,  "Snotspill",				"BNG",	 4,  220,  AI_SNOTSPIL, 3, 10, 18, 4,   0,  0,   0, QUEST_BANNER10 },
	{ MT_ADVOCATE, "Arch-Bishop Lazarus",	  "GENERAL", 0,  600,  AI_LAZURUS,  3, 30, 50, 78,  0,  0,   0, QUEST_VILE13   },
	{ MT_HLSPWN,   "Red Vex",				  "REDV",	0,  400,  AI_LAZHELP,  3, 30, 50, 74,  0,  0,   0, QUEST_VILE13   },
	{ MT_HLSPWN,   "BlackJade",				"BLKJD",   0,  400,  AI_LAZHELP,  3, 30, 50, 76,  0,  0,   0, QUEST_VILE13   },
	{ MT_RBLACK,   "Lachdanan",				"BHKA",	14, 500,  AI_LACHDAN,  3, 0,  0,  0,   0,  0,   0, QUEST_VEIL9	},
	{ MT_BTBLACK,  "Warlord of Blood",		 "GENERAL", 13, 850,  AI_WARLORD,  3, 35, 50, 120, 0,  0,   0, QUEST_WARLRD9  },
	{ MT_CLEAVER,  "The Butcher",			  "GENRL",   0,  220,  AI_CLEAVER,  3, 6,  12, 70,  0,  0,   0, 0			  },
	{ MT_TSKELAX,  "Bonehead Keenaxe",		 "BHKA",	2,  91,   AI_SKELSD,   2, 4,  10, 72,  7,  100, 0, 0			  },
	{ MT_RFALLSD,  "Bladeskin the Slasher",	"BSTS",	2,  51,   AI_FALLEN,   0, 6,  18, 2,   11, 45,  0, 0			  },
	{ MT_NZOMBIE,  "Soulpus",				  "GENERAL", 2,  133,  AI_ZOMBIE,   0, 4,  8,  6,   0,  0,   0, 0			  },
	{ MT_RFALLSP,  "Pukerat the Unclean",	  "PTU",	 2,  77,   AI_FALLEN,   3, 1,  5,  2,   0,  0,   0, 0			  },
	{ MT_WSKELAX,  "Boneripper",			   "BR",	  2,  54,   AI_BAT,	  0, 6,  15, 88,  3,  0,   0, 0			  },
	{ MT_NZOMBIE,  "Rotfeast the Hungry",	  "ETH",	 2,  85,   AI_SKELSD,   3, 4,  12, 72,  3,  0,   0, 0			  },
	{ MT_DFALLSD,  "Gutshank the Quick",	   "GTQ",	 3,  66,   AI_BAT,	  2, 6,  16, 2,   3,  0,   0, 0			  },
	{ MT_TSKELSD,  "Brokenhead Bangshield",	"BHBS",	3,  108,  AI_SKELSD,   3, 12, 20, 76,  3,  0,   0, 0			  },
	{ MT_YFALLSP,  "Bongo",					"BNG",	 3,  178,  AI_FALLEN,   3, 9,  21, 0,   3,  0,   0, 0			  },
	{ MT_BZOMBIE,  "Rotcarnage",			   "RCRN",	3,  102,  AI_ZOMBIE,   3, 9,  24, 76,  11, 45,  0, 0			  },
	{ MT_NSCAV,	"Shadowbite",			   "SHBT",	2,  60,   AI_SKELSD,   3, 3,  20, 16,  3,  0,   0, 0			  },
	{ MT_WSKELBW,  "Deadeye",				  "DE",	  2,  49,   AI_GOATBOW,  0, 6,  9,  74,  0,  0,   0, 0			  },
	{ MT_RSKELAX,  "Madeye the Dead",		  "MTD",	 4,  75,   AI_BAT,	  0, 9,  21, 24,  11, 30,  0, 0			  },
	{ MT_BSCAV,	"El Chupacabras",		   "GENERAL", 3,  120,  AI_GOATMC,   0, 10, 18, 2,   3,  30,  0, 0			  },
	{ MT_TSKELBW,  "Skullfire",				"SKFR",	3,  125,  AI_GOATBOW,  1, 6,  10, 16,  0,  100, 0, 0			  },
	{ MT_SNEAK,	"Warpskull",				"TSPO",	3,  117,  AI_SNEAK,	2, 6,  18, 6,   3,  0,   0, 0			  },
	{ MT_GZOMBIE,  "Goretongue",			   "PMR",	 3,  156,  AI_SKELSD,   1, 15, 30, 72,  0,  0,   0, 0			  },
	{ MT_WSCAV,	"Pulsecrawler",			 "BHKA",	4,  150,  AI_SCAV,	 0, 16, 20, 20,  11, 45,  0, 0			  },
	{ MT_BLINK,	"Moonbender",			   "GENERAL", 4,  135,  AI_BAT,	  0, 9,  27, 16,  3,  0,   0, 0			  },
	{ MT_BLINK,	"Wrathraven",			   "GENERAL", 5,  135,  AI_BAT,	  2, 9,  22, 16,  3,  0,   0, 0			  },
	{ MT_YSCAV,	"Spineeater",			   "GENERAL", 4,  180,  AI_SCAV,	 1, 18, 25, 96,  3,  0,   0, 0			  },
	{ MT_RSKELBW,  "Blackash the Burning",	 "BASHTB",  4,  120,  AI_GOATBOW,  0, 6,  16, 24,  3,  0,   0, 0			  },
	{ MT_BFALLSD,  "Shadowcrow",			   "GENERAL", 5,  270,  AI_SNEAK,	2, 12, 25, 0,   3,  0,   0, 0			  },
	{ MT_LRDSAYTR, "Blightstone the Weak",	 "BHKA",	4,  360,  AI_SKELSD,   0, 4,  12, 12,  7,  70,  0, 0			  },
	{ MT_FAT,	  "Bilefroth the Pit Master", "BFTP",	6,  210,  AI_BAT,	  1, 16, 23, 28,  3,  0,   0, 0			  },
	{ MT_NGOATBW,  "Bloodskin Darkbow",		"BSDB",	5,  207,  AI_GOATBOW,  0, 3,  16, 6,   11, 55,  0, 0			  },
	{ MT_GLOOM,	"Foulwing",				 "DB",	  5,  246,  AI_RHINO,	3, 12, 28, 2,   3,  0,   0, 0			  },
	{ MT_XSKELSD,  "Shadowdrinker",			"SHDR",	5,  300,  AI_SNEAK,	1, 18, 26, 78,  8,  45,  0, 0			  },
	{ MT_UNSEEN,   "Hazeshifter",			  "BHKA",	5,  285,  AI_SNEAK,	3, 18, 30, 96,  3,  0,   0, 0			  },
	{ MT_NACID,	"Deathspit",				"BFDS",	6,  303,  AI_ACIDUNIQ, 0, 12, 32, 6,   3,  0,   0, 0			  },
	{ MT_RGOATMC,  "Bloodgutter",			  "BGBL",	6,  315,  AI_BAT,	  1, 24, 34, 16,  3,  0,   0, 0			  },
	{ MT_BGOATMC,  "Deathshade Fleshmaul",	 "DSFM",	6,  276,  AI_RHINO,	0, 12, 24, 10,  8,  65,  0, 0			  },
	{ MT_WYRM,	 "Warmaggot the Mad",		"GENERAL", 6,  246,  AI_BAT,	  3, 15, 30, 4,   3,  0,   0, 0			  },
	{ MT_STORM,	"Glasskull the Jagged",	 "BHKA",	7,  354,  AI_STORM,	0, 18, 30, 88,  3,  0,   0, 0			  },
	{ MT_RGOATBW,  "Blightfire",			   "BLF",	 7,  321,  AI_SUCC,	 2, 13, 21, 16,  3,  0,   0, 0			  },
	{ MT_GARGOYLE, "Nightwing the Cold",	   "GENERAL", 7,  342,  AI_BAT,	  1, 18, 26, 76,  3,  0,   0, 0			  },
	{ MT_GGOATBW,  "Gorestone",				"GENERAL", 7,  303,  AI_GOATBOW,  1, 15, 28, 68,  7,  70,  0, 0			  },
	{ MT_BMAGMA,   "Bronzefist Firestone",	 "GENERAL", 8,  360,  AI_MAGMA,	0, 30, 36, 10,  3,  0,   0, 0			  },
	{ MT_INCIN,	"Wrathfire the Doomed",	 "WFTD",	8,  270,  AI_SKELSD,   2, 20, 30, 14,  3,  0,   0, 0			  },
	{ MT_NMAGMA,   "Firewound the Grim",	   "BHKA",	8,  303,  AI_MAGMA,	0, 18, 22, 10,  3,  0,   0, 0			  },
	{ MT_MUDMAN,   "Baron Sludge",			 "BSM",	 8,  315,  AI_SNEAK,	3, 25, 34, 78,  11, 75,  0, 0			  },
	{ MT_GGOATMC,  "Blighthorn Steelmace",	 "BHSM",	7,  250,  AI_RHINO,	0, 20, 28, 4,   11, 45,  0, 0			  },
	{ MT_RACID,	"Chaoshowler",			  "GENERAL", 8,  240,  AI_ACIDUNIQ, 0, 12, 20, 0,   3,  0,   0, 0			  },
	{ MT_REDDTH,   "Doomgrin the Rotting",	 "GENERAL", 8,  405,  AI_STORM,	3, 25, 50, 78,  3,  0,   0, 0			  },
	{ MT_FLAMLRD,  "Madburner",				"GENERAL", 9,  270,  AI_STORM,	0, 20, 40, 56,  3,  0,   0, 0			  },
	{ MT_LTCHDMN,  "Bonesaw the Litch",		"GENERAL", 9,  495,  AI_STORM,	2, 30, 55, 78,  3,  0,   0, 0			  },
	{ MT_MUDRUN,   "Breakspine",			   "GENERAL", 9,  351,  AI_RHINO,	0, 25, 34, 2,   3,  0,   0, 0			  },
	{ MT_REDDTH,   "Devilskull Sharpbone",	 "GENERAL", 9,  444,  AI_STORM,	1, 25, 40, 16,  3,  0,   0, 0			  },
	{ MT_STORM,	"Brokenstorm",			  "GENERAL", 9,  411,  AI_STORM,	2, 25, 36, 32,  3,  0,   0, 0			  },
	{ MT_RSTORM,   "Stormbane",				"GENERAL", 9,  555,  AI_STORM,	3, 30, 30, 32,  3,  0,   0, 0			  },
	{ MT_TOAD,	 "Oozedrool",				"GENERAL", 9,  483,  AI_FAT,	  3, 25, 30, 4,   3,  0,   0, 0			  },
	{ MT_BLOODCLW, "Goldblight of the Flame",  "GENERAL", 10, 405,  AI_GARG,	 0, 15, 35, 24,  11, 80,  0, 0			  },
	{ MT_OBLORD,   "Blackstorm",			   "GENERAL", 10, 525,  AI_RHINO,	3, 20, 40, 40,  11, 90,  0, 0			  },
	{ MT_RACID,	"Plaguewrath",			  "GENERAL", 10, 450,  AI_ACIDUNIQ, 2, 20, 30, 74,  3,  0,   0, 0			  },
	{ MT_RSTORM,   "The Flayer",			   "GENERAL", 10, 501,  AI_STORM,	1, 20, 35, 99,  3,  0,   0, 0			  },
	{ MT_FROSTC,   "Bluehorn",				 "GENERAL", 11, 477,  AI_RHINO,	1, 25, 30, 10,  11, 90,  0, 0			  },
	{ MT_HELLBURN, "Warpfire Hellspawn",	   "GENERAL", 11, 525,  AI_FIREMAN,  3, 10, 40, 17,  3,  0,   0, 0			  },
	{ MT_NSNAKE,   "Fangspeir",				"GENERAL", 11, 444,  AI_SKELSD,   1, 15, 32, 80,  3,  0,   0, 0			  },
	{ MT_UDEDBLRG, "Festerskull",			  "GENERAL", 11, 600,  AI_STORM,	2, 15, 30, 72,  3,  0,   0, 0			  },
	{ MT_NBLACK,   "Lionskull the Bent",	   "GENERAL", 12, 525,  AI_SKELSD,   2, 25, 25, 120, 3,  0,   0, 0			  },
	{ MT_COUNSLR,  "Blacktongue",			  "GENERAL", 12, 360,  AI_COUNSLR,  3, 15, 30, 66,  3,  0,   0, 0			  },
	{ MT_DEATHW,   "Viletouch",				"GENERAL", 12, 525,  AI_GARG,	 3, 20, 40, 96,  3,  0,   0, 0			  },
	{ MT_RSNAKE,   "Viperflame",			   "GENERAL", 12, 570,  AI_SKELSD,   1, 25, 35, 20,  3,  0,   0, 0			  },
	{ MT_BSNAKE,   "Fangskin",				 "BHKA",	14, 681,  AI_SKELSD,   2, 15, 50, 12,  3,  0,   0, 0			  },
	{ MT_SUCCUBUS, "Witchfire the Unholy",	 "GENERAL", 12, 444,  AI_SUCC,	 3, 10, 20, 28,  3,  0,   0, 0			  },
	{ MT_BALROG,   "Blackskull",			   "BHKA",	13, 750,  AI_SKELSD,   3, 25, 40, 12,  3,  0,   0, 0			  },
	{ MT_UNRAV,	"Soulslash",				"GENERAL", 12, 450,  AI_SKELSD,   0, 25, 25, 72,  3,  0,   0, 0			  },
	{ MT_VTEXLRD,  "Windspawn",				"GENERAL", 12, 711,  AI_SKELSD,   1, 35, 40, 24,  3,  0,   0, 0			  },
	{ MT_GSNAKE,   "Lord of the Pit",		  "GENERAL", 13, 762,  AI_SKELSD,   2, 25, 42, 66,  3,  0,   0, 0			  },
	{ MT_RTBLACK,  "Rustweaver",			   "GENERAL", 13, 400,  AI_SKELSD,   3, 1,  60, 120, 0,  0,   0, 0			  },
	{ MT_HOLOWONE, "Howlingire the Shade",	 "GENERAL", 13, 450,  AI_SKELSD,   2, 40, 75, 6,   3,  0,   0, 0			  },
	{ MT_MAEL,	 "Doomcloud",				"GENERAL", 13, 612,  AI_STORM,	1, 1,  60, 34,  0,  0,   0, 0			  },
	{ MT_PAINMSTR, "Bloodmoon Soulfire",	   "GENERAL", 13, 684,  AI_SKELSD,   1, 15, 40, 14,  3,  0,   0, 0			  },
	{ MT_SNOWWICH, "Witchmoon",				"GENERAL", 13, 310,  AI_SUCC,	 3, 30, 40, 4,   0,  0,   0, 0			  },
	{ MT_VTEXLRD,  "Gorefeast",				"GENERAL", 13, 771,  AI_SKELSD,   3, 20, 55, 66,  0,  0,   0, 0			  },
	{ MT_RTBLACK,  "Graywar the Slayer",	   "GENERAL", 14, 672,  AI_SKELSD,   1, 30, 50, 68,  0,  0,   0, 0			  },
	{ MT_MAGISTR,  "Dreadjudge",			   "GENERAL", 14, 540,  AI_COUNSLR,  1, 30, 40, 14,  3,  0,   0, 0			  },
	{ MT_HLSPWN,   "Stareye the Witch",		"GENERAL", 14, 726,  AI_SUCC,	 2, 30, 50, 16,  0,  0,   0, 0			  },
	{ MT_BTBLACK,  "Steelskull the Hunter",	"GENERAL", 14, 831,  AI_SKELSD,   3, 40, 50, 68,  0,  0,   0, 0			  },
	{ MT_RBLACK,   "Sir Gorash",			   "GENERAL", 16, 1050, AI_SKELSD,   1, 20, 60, 64,  0,  0,   0, 0			  },
	{ MT_CABALIST, "The Vizier",			   "GENERAL", 15, 850,  AI_COUNSLR,  2, 25, 40, 16,  3,  0,   0, 0			  },
	{ MT_REALWEAV, "Zamphir",				  "GENERAL", 15, 891,  AI_SKELSD,   2, 30, 50, 78,  3,  0,   0, 0			  },
	{ MT_HLSPWN,   "Bloodlust",				"GENERAL", 15, 825,  AI_SUCC,	 1, 20, 55, 104, 0,  0,   0, 0			  },
	{ MT_HLSPWN,   "Webwidow",				 "GENERAL", 16, 774,  AI_SUCC,	 1, 20, 50, 88,  0,  0,   0, 0			  },
	{ MT_SOLBRNR,  "Fleshdancer",			  "GENERAL", 16, 999,  AI_SUCC,	 3, 30, 50, 74,  0,  0,   0, 0			  },
	{ MT_OBLORD,   "Grimspike",				"GENERAL", 19, 534,  AI_SNEAK,	1, 25, 40, 74,  3,  0,   0, 0			  },
	{ MT_STORML,   "Doomlock",				 "GENERAL", 28, 534,  AI_SNEAK,	1, 35, 55, 78,  3,  0,   0, 0			  },
	{ -1,		  NULL,					   NULL,	  0,  0,	0,		   0, 0,  0,  0,   0,  0,   0, 0			  }
};
int MWVel[24][3] =
{
  { 256, 512, 1024 },
  { 128, 256, 512 },
  { 85, 170, 341 },
  { 64, 128, 256 },
  { 51, 102, 204 },
  { 42, 85, 170 },
  { 36, 73, 146 },
  { 32, 64, 128 },
  { 28, 56, 113 },
  { 26, 51, 102 },
  { 23, 46, 93 },
  { 21, 42, 85 },
  { 19, 39, 78 },
  { 18, 36, 73 },
  { 17, 34, 68 },
  { 16, 32, 64 },
  { 15, 30, 60 },
  { 14, 28, 57 },
  { 13, 26, 54 },
  { 12, 25, 51 },
  { 12, 24, 48 },
  { 11, 23, 46 },
  { 11, 22, 44 },
  { 10, 21, 42 }
};
char animletter[7] = "nwahds";
int left[8] = { 7, 0, 1, 2, 3, 4, 5, 6 };
int right[8] = { 1, 2, 3, 4, 5, 6, 7, 0 };
int opposite[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
int offset_x[8] = { 1, 0, -1, -1, -1, 0, 1, 1 };
int offset_y[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

/* unused */
int rnd5[4] = { 5, 10, 15, 20 };
int rnd10[4] = { 10, 15, 20, 30 };
int rnd20[4] = { 20, 30, 40, 50 };
int rnd60[4] = { 60, 70, 80, 90 };
//

void(__fastcall *AiProc[])(int i) =
{
  &MAI_Zombie,
  &MAI_Fat,
  &MAI_SkelSd,
  &MAI_SkelBow,
  &MAI_Scav,
  &MAI_Rhino,
  &MAI_GoatMc,
  &MAI_GoatBow,
  &MAI_Fallen,
  &MAI_Magma,
  &MAI_SkelKing,
  &MAI_Bat,
  &MAI_Garg,
  &MAI_Cleaver,
  &MAI_Succ,
  &MAI_Sneak,
  &MAI_Storm,
  &MAI_Fireman,
  &MAI_Garbud,
  &MAI_Acid,
  &MAI_AcidUniq,
  &MAI_Golum,
  &MAI_Zhar,
  &MAI_SnotSpil,
  &MAI_Snake,
  &MAI_Counselor,
  &MAI_Mega,
  &MAI_Diablo,
  &MAI_Lazurus,
  &MAI_Lazhelp,
  &MAI_Lachdanan,
  &MAI_Warlord
};

struct monster_cpp_init
{
	monster_cpp_init()
	{
		monster_cpp_init_value = monster_inf;
	}
} _monster_cpp_init;
// 47F130: using guessed type int monster_inf;
// 64CCE4: using guessed type int monster_cpp_init_value;

void __fastcall InitMonsterTRN(int monst, BOOL special)
{
	BYTE *f = Monsters[monst].trans_file;
	for ( int i = 0; i < 256; i++ )
	{
		if ( *f == 255 )
		{
			*f = 0;
		}
		f++;
	}

	int n = special ? 6 : 5;
	for ( int j = 0; j < n; ++j )
	{
		if ( j != 1 || Monsters[monst].mtype < MT_COUNSLR || Monsters[monst].mtype > MT_ADVOCATE )
		{
			for ( int k = 0; k < 8; k++ )
			{
				Cl2ApplyTrans(
					Monsters[monst].Anims[j].Frames[k],
					Monsters[monst].trans_file,
					Monsters[monst].Anims[j].Rate);
			}
		}
	}
}

void __cdecl InitLevelMonsters()
{
	int i;

	nummtypes = 0;
	monstimgtot = 0;
	MissileFileFlag = 0;

	for ( i = 0; i < MAX_LVLMTYPES; i++ )
	{
		Monsters[i].mPlaceFlags = 0;
	}

	ClrAllMonsters();
	nummonsters = 0;
	totalmonsters = MAXMONSTERS;

	for ( i = 0; i < MAXMONSTERS; i++ )
	{
		monstactive[i] = i;
	}

	uniquetrans = 0;
}

int __fastcall AddMonsterType(int type, int placeflag)
{
	BOOL done = FALSE;
	int i;

	for ( i = 0; i < nummtypes && !done; i++ )
	{
		done = Monsters[i].mtype == type;
	}

	i--;

	if ( !done )
	{
		i = nummtypes;
		nummtypes++;
		Monsters[i].mtype = type;
		monstimgtot += monsterdata[type].mType;
		InitMonsterGFX(i);
		InitMonsterSND(i);
	}

	Monsters[i].mPlaceFlags |= placeflag;
	return i;
}

void __cdecl GetLevelMTypes()
{
	int i;

	// this array is merged with skeltypes down below.
	int typelist[MAXMONSTERS];
	int skeltypes[NUM_MTYPES];

	int minl; // min level
	int maxl; // max level
	char mamask = 3; // monster availability mask

	int nt; // number of types

	AddMonsterType(MT_GOLEM, 2);
	if ( currlevel == 16 )
	{
		AddMonsterType(MT_ADVOCATE, 1);
		AddMonsterType(MT_RBLACK, 1);
		AddMonsterType(MT_DIABLO, 2);
		return;
	}

	if ( !setlevel )
	{
		if ( QuestStatus(QTYPE_BUTCH) )
			AddMonsterType(MT_CLEAVER, 2);
		if ( QuestStatus(QTYPE_GARB) )
			AddMonsterType(UniqMonst[0].mtype, 4);
		if ( QuestStatus(QTYPE_ZHAR) )
			AddMonsterType(UniqMonst[2].mtype, 4);
		if ( QuestStatus(QTYPE_BOL) )
			AddMonsterType(UniqMonst[3].mtype, 4);
		if ( QuestStatus(QTYPE_VEIL) )
			AddMonsterType(UniqMonst[7].mtype, 4);
		if ( QuestStatus(QTYPE_WARLRD) )
			AddMonsterType(UniqMonst[8].mtype, 4);

		if ( gbMaxPlayers != 1 && currlevel == quests[QTYPE_KING]._qlevel )
		{

			AddMonsterType(MT_SKING, 4);
			const int numskeltypes = 19;

			nt = 0;
			for ( i = MT_WSKELAX; i <= MT_WSKELAX + numskeltypes; i++ )
			{
				if ( IsSkel(i) )
				{
					minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
					maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

					if ( currlevel >= minl && currlevel <= maxl )
					{
						if ( MonstAvailTbl[i] & mamask )
						{
							skeltypes[nt++] = i;
						}
					}
				}
			}
			AddMonsterType(skeltypes[random(88, nt)], 1);
		}

		nt = 0;
		for ( i = 0; i < 111; i++ )
		{
			minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
			maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

			if ( currlevel >= minl && currlevel <= maxl )
			{
				if ( MonstAvailTbl[i] & mamask )
				{
					typelist[nt++] = i;
				}
			}
		}

		if ( monstdebug )
		{
			for ( i = 0; i < debugmonsttypes; i++ )
				AddMonsterType(DebugMonsters[i], 1);
		}
		else
		{

			while ( nt > 0 && nummtypes < MAX_LVLMTYPES && monstimgtot < 4000 )
			{
				for ( i = 0; i < nt; )
				{
					if ( monsterdata[typelist[i]].mType > 4000 - monstimgtot )
					{
						typelist[i] = typelist[--nt];
						continue;
					}

					i++;
				}

				if ( nt != 0 )
				{
					i = random(88, nt);
					AddMonsterType(typelist[i], 1);
					typelist[i] = typelist[--nt];
				}
			}
		}

	}
	else
	{
		if ( setlvlnum == SL_SKELKING )
		{
			AddMonsterType(MT_SKING, 4);
		}
	}

}

void __fastcall InitMonsterGFX(int monst)
{
	int mtype = (unsigned char)Monsters[monst].mtype;
	char strBuff[256];

	for ( int anim = 0; anim < 6; anim++ )
	{
		if ( (animletter[anim] != 's' || monsterdata[mtype].has_special) && monsterdata[mtype].Frames[anim] > 0 )
		{
			sprintf(strBuff, monsterdata[mtype].GraphicType, animletter[anim]);

			unsigned char* celBuf = LoadFileInMem(strBuff, NULL);
			Monsters[monst].Anims[anim].CMem = celBuf;

			if ( Monsters[monst].mtype != MT_GOLEM || (animletter[anim] != 's' && animletter[anim] != 'd') )
			{

				for ( int i = 0; i < 8; i++ )
				{
					Monsters[monst].Anims[anim].Frames[i] =
						&celBuf[((int *)celBuf)[i]];
				}
			}
			else
			{
				for ( int i = 0; i < 8; i++ )
				{
					Monsters[monst].Anims[anim].Frames[i] = celBuf;
				}
			}
		}

		// TODO: either the AnimStruct members have wrong naming or the MonsterData ones it seems
		Monsters[monst].Anims[anim].Rate = monsterdata[mtype].Frames[anim];
		Monsters[monst].Anims[anim].Delay = monsterdata[mtype].Rate[anim];
	}

	Monsters[monst].flags_1 = monsterdata[mtype].flags;
	Monsters[monst].flags_2 = (monsterdata[mtype].flags - 64) >> 1;
	Monsters[monst].mMinHP = monsterdata[mtype].mMinHP;
	Monsters[monst].mMaxHP = monsterdata[mtype].mMaxHP;
	Monsters[monst].has_special = monsterdata[mtype].has_special;
	Monsters[monst].mAFNum = monsterdata[mtype].mAFNum;
	Monsters[monst].MData = &monsterdata[mtype];

	if ( monsterdata[mtype].has_trans )
	{
		Monsters[monst].trans_file = LoadFileInMem(monsterdata[mtype].TransFile, NULL);
		InitMonsterTRN(monst, monsterdata[mtype].has_special);

		void *trans_file = Monsters[monst].trans_file;
		Monsters[monst].trans_file = NULL;

		mem_free_dbg(trans_file);
	}

	if ( mtype >= MT_NMAGMA && mtype <= MT_WMAGMA && !(MissileFileFlag & 1) )
	{
		MissileFileFlag |= 1;
		LoadMissileGFX(MFILE_MAGBALL);
	}
	if ( mtype >= MT_STORM && mtype <= MT_MAEL && !(MissileFileFlag & 2) )
	{
		MissileFileFlag |= 2;
		LoadMissileGFX(MFILE_THINLGHT);
	}
	if ( mtype == MT_SUCCUBUS )
	{
		if ( MissileFileFlag & 4 ) return;

		MissileFileFlag |= 4;
		LoadMissileGFX(MFILE_FLARE);
		LoadMissileGFX(MFILE_FLAREEXP);
	}
	if ( mtype == MT_SNOWWICH )
	{
		if ( MissileFileFlag & 0x20 ) return;

		MissileFileFlag |= 0x20;
		LoadMissileGFX(MFILE_SCUBMISB);
		LoadMissileGFX(MFILE_SCBSEXPB);
	}
	if ( mtype == MT_HLSPWN )
	{
		if ( MissileFileFlag & 0x40 ) return;

		MissileFileFlag |= 0x40;
		LoadMissileGFX(MFILE_SCUBMISD);
		LoadMissileGFX(MFILE_SCBSEXPD);
	}
	if ( mtype == MT_SOLBRNR )
	{
		if ( MissileFileFlag & 0x80 ) return;

		MissileFileFlag |= 0x80;
		LoadMissileGFX(MFILE_SCUBMISC);
		LoadMissileGFX(MFILE_SCBSEXPC);
	}
	if ( mtype >= MT_INCIN && mtype <= MT_HELLBURN && !(MissileFileFlag & 8) )
	{
		MissileFileFlag |= 8;
		LoadMissileGFX(MFILE_KRULL);
	}
	if ( mtype >= MT_NACID && mtype <= MT_XACID && !(MissileFileFlag & 0x10) )
	{
		MissileFileFlag |= 0x10;
		LoadMissileGFX(MFILE_ACIDBF);
		LoadMissileGFX(MFILE_ACIDSPLA);
		LoadMissileGFX(MFILE_ACIDPUD);
	}
	if ( mtype == MT_DIABLO )
	{
		LoadMissileGFX(MFILE_FIREPLAR);
	}
}

void __fastcall ClearMVars(int i)
{
	monster[i]._mVar1 = 0;
	monster[i]._mVar2 = 0;
	monster[i]._mVar3 = 0;
	monster[i]._mVar4 = 0;
	monster[i]._mVar5 = 0;
	monster[i]._mVar6 = 0;
	monster[i]._mVar7 = 0;
	monster[i]._mVar8 = 0;
}

void __fastcall InitMonster(int i, int rd, int mtype, int x, int y)
{
	CMonster *monst = &Monsters[mtype];

	monster[i]._mdir = rd;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
	monster[i]._mmode = MM_STAND;
	monster[i]._mMTidx = mtype;
	monster[i].mName = monst->MData->mName;
	monster[i].MType = monst;
	monster[i].MData = monst->MData;
	monster[i]._mAnimData = monst->Anims[0].Frames[rd];
	monster[i]._mAnimDelay = monst->Anims[0].Delay;
	monster[i]._mAnimCnt = random(88, monst->Anims[0].Delay - 1);
	monster[i]._mAnimLen = monst->Anims[0].Rate;
	monster[i]._mAnimFrame = random(88, monst->Anims[0].Rate - 1) + 1;

	if ( monst->mtype == MT_DIABLO )
	{
		monster[i]._mmaxhp = (random(88, 1) + 1666) << 6;
	}
	else
	{
		monster[i]._mmaxhp = (monst->mMinHP + random(88, monst->mMaxHP - monst->mMinHP + 1)) << 6;
	}

	if ( gbMaxPlayers == 1 )
	{
		monster[i]._mmaxhp >>= 1;
		if ( monster[i]._mmaxhp < 64 )
		{
			monster[i]._mmaxhp = 64;
		}
	}

	monster[i]._mhitpoints = monster[i]._mmaxhp;
	monster[i]._mAi = monst->MData->mAi;
	monster[i]._mint = monst->MData->mInt;
	monster[i]._mgoal = 1;
	monster[i]._mgoalvar1 = 0;
	monster[i]._mgoalvar2 = 0;
	monster[i]._mgoalvar3 = 0;
	monster[i]._pathcount = 0;
	monster[i]._uniqtype = 0;
	monster[i]._msquelch = 0;
	monster[i].field_18 = 0;
	monster[i]._mDelFlag = 0;
	monster[i]._mRndSeed = GetRndSeed();
	monster[i]._mAISeed = GetRndSeed();
	monster[i].mWhoHit = 0;
	monster[i].mLevel = monst->MData->mLevel;
	monster[i].mExp = monst->MData->mExp;
	monster[i].mHit = monst->MData->mHit;
	monster[i].mMinDamage = monst->MData->mMinDamage;
	monster[i].mMaxDamage = monst->MData->mMaxDamage;
	monster[i].mHit2 = monst->MData->mHit2;
	monster[i].mMinDamage2 = monst->MData->mMinDamage2;
	monster[i].mMaxDamage2 = monst->MData->mMaxDamage2;
	monster[i].mArmorClass = monst->MData->mArmorClass;
	monster[i].mMagicRes = monst->MData->mMagicRes;
	monster[i].leader = 0;
	monster[i].leaderflag = 0;
	monster[i]._mFlags = monst->MData->mFlags;
	monster[i].mtalkmsg = 0;

	if ( monster[i]._mAi == AI_GARG )
	{
		monster[i]._mAnimData = monst->Anims[5].Frames[rd];
		monster[i]._mAnimFrame = 1;
		monster[i]._mFlags |= 4u;
		monster[i]._mmode = MM_SATTACK;
	}

	if ( gnDifficulty == DIFF_NIGHTMARE )
	{
		monster[i].mLevel += 15;
		monster[i].mHit += 85;
		monster[i].mHit2 += 85;
		monster[i]._mmaxhp = 3 * monster[i]._mmaxhp + 64;
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mExp = 2 * (monster[i].mExp + 1000);
		monster[i].mMinDamage = 2 * (monster[i].mMinDamage + 2);
		monster[i].mMaxDamage = 2 * (monster[i].mMaxDamage + 2);
		monster[i].mMinDamage2 = 2 * (monster[i].mMinDamage2 + 2);
		monster[i].mMaxDamage2 = 2 * (monster[i].mMaxDamage2 + 2);
		monster[i].mArmorClass += 50;
	}

	if ( gnDifficulty == DIFF_HELL )
	{
		monster[i].mLevel += 30;
		monster[i]._mmaxhp = 4 * monster[i]._mmaxhp + 192;
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mHit += 120;
		monster[i].mHit2 += 120;
		monster[i].mExp = 4 * (monster[i].mExp + 1000);
		monster[i].mMinDamage = 4 * monster[i].mMinDamage + 6;
		monster[i].mMaxDamage = 4 * monster[i].mMaxDamage + 6;
		monster[i].mMinDamage2 = 4 * monster[i].mMinDamage2 + 6;
		monster[i].mMaxDamage2 = 4 * monster[i].mMaxDamage2 + 6;
		monster[i].mArmorClass += 80;
		monster[i].mMagicRes = monst->MData->mMagicRes2;
	}
}

void __cdecl ClrAllMonsters()
{
	MonsterStruct *Monst;

	for ( int i = 0; i < 200; i++ )
	{
		Monst = &monster[i];
		ClearMVars(i);
		Monst->mName = "Invalid Monster";
		Monst->_mgoal = 0;
		Monst->_mmode = MM_STAND;
		Monst->_mVar1 = 0;
		Monst->_mVar2 = 0;
		Monst->_mx = 0;
		Monst->_my = 0;
		Monst->_mfutx = 0;
		Monst->_mfuty = 0;
		Monst->_moldx = 0;
		Monst->_moldy = 0;
		Monst->_mdir = random(89, 8);
		Monst->_mxvel = 0;
		Monst->_myvel = 0;
		Monst->_mAnimData = NULL;
		Monst->_mAnimDelay = 0;
		Monst->_mAnimCnt = 0;
		Monst->_mAnimLen = 0;
		Monst->_mAnimFrame = 0;
		Monst->_mFlags = 0;
		Monst->_mDelFlag = 0;
		Monst->_menemy = random(89, gbActivePlayers);
		Monst->_menemyx = plr[Monst->_menemy]._px;
		Monst->_menemyy = plr[Monst->_menemy]._py;
	}
}

BOOL __fastcall MonstPlace(int xp, int yp)
{
	if ( xp < 0 || xp >= 112
		|| yp < 0 || yp >= 112
		|| dMonster[xp][yp]
		|| dPlayer[xp][yp] )
	{
		return FALSE;
	}

	char f = dFlags[xp][yp];

	// TODO: Add enum values here
	if ( f & 2 )
	{
		return FALSE;
	}

	if ( f & 8 )
	{
		return FALSE;
	}

	return !SolidLoc(xp, yp);
}

void __fastcall PlaceMonster(int i, int mtype, int x, int y)
{
	dMonster[x][y] = i + 1;

	int rd = random(90, 8);
	InitMonster(i, rd, mtype, x, y);
}

void __fastcall PlaceUniqueMonst(int uniqindex, int miniontype, int unpackfilesize)
{
	int xp;
	int yp;
	char filestr[64];

	UniqMonstStruct *Uniq = &UniqMonst[uniqindex];
	MonsterStruct *Monst = &monster[nummonsters];
	int count = 0;

	if ( (uniquetrans + 19) << 8 >= LIGHTSIZE )
	{
		return;
	}

	int uniqtype;
	for ( uniqtype = 0; uniqtype < nummtypes; uniqtype++ )
	{
		if ( Monsters[uniqtype].mtype == Uniq->mtype )
		{
			break;
		}
	}

	while (true)
	{

		xp = random(91, 80) + 16;
		yp = random(91, 80) + 16;
		int count2 = 0;
		for ( int x = xp - 3; x < xp + 3; x++ )
		{
			for ( int y = yp - 3; y < yp + 3; y++ )
			{
				if ( y >= 0 && y < 112 && x >= 0 && x < 112 && MonstPlace(x, y) )
				{
					count2++;
				}
			}
		}

		if ( count2 < 9 ) {
			count++;
			if ( count < 1000 )
			{
				continue;
			}
		}


		if ( MonstPlace(xp, yp) )
		{
			break;
		}
	}

	if ( uniqindex == 3 )
	{
		xp = 2 * setpc_x + 24;
		yp = 2 * setpc_y + 28;
	}
	if ( uniqindex == 8 )
	{
		xp = 2 * setpc_x + 22;
		yp = 2 * setpc_y + 23;
	}
	if ( uniqindex == 2 )
	{
		BOOL zharflag = TRUE;
		for ( int i = 0; i < themeCount; i++ )
		{
			if ( i == zharlib && zharflag == TRUE )
			{
				zharflag = FALSE;
				xp = 2 * themeLoc[i].x + 20;
				yp = 2 * themeLoc[i].y + 20;
			}
		}
	}
	if ( gbMaxPlayers == 1 )
	{
		if ( uniqindex == 4 )
		{
			xp = 32;
			yp = 46;
		}
		if ( uniqindex == 5 )
		{
			xp = 40;
			yp = 45;
		}
		if ( uniqindex == 6 )
		{
			xp = 38;
			yp = 49;
		}
		if ( uniqindex == 1 )
		{
			xp = 35;
			yp = 47;
		}
	}
	else
	{
		if ( uniqindex == 4 )
		{
			xp = 2 * setpc_x + 19;
			yp = 2 * setpc_y + 22;
		}
		if ( uniqindex == 5 )
		{
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 19;
		}
		if ( uniqindex == 6 )
		{
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 25;
		}
	}
	if ( uniqindex == 9 )
	{
		BOOL done = FALSE;
		for ( yp = 0; yp < 112 && !done; yp++ )
		{
			for ( xp = 0; xp < 112 && !done; xp++ )
			{
				done = dPiece[xp][yp] == 367;
			}
		}
	}

	PlaceMonster(nummonsters, uniqtype, xp, yp);
	Monst->_uniqtype = uniqindex + 1;

	if ( Uniq->mlevel )
	{
		Monst->mLevel = 2 * Uniq->mlevel;
	}
	else
	{
		Monst->mLevel += 5;
	}

	Monst->mExp *= 2;
	Monst->mName = Uniq->mName;
	Monst->_mmaxhp = Uniq->mmaxhp << 6;

	if ( gbMaxPlayers == 1 )
	{
		Monst->_mmaxhp = Monst->_mmaxhp >> 1;
		if ( Monst->_mmaxhp < 64 )
		{
			Monst->_mmaxhp = 64;
		}
	}

	Monst->_mhitpoints = Monst->_mmaxhp;
	Monst->_mAi = Uniq->mAi;
	Monst->_mint = Uniq->mint;
	Monst->mMinDamage = Uniq->mMinDamage;
	Monst->mMaxDamage = Uniq->mMaxDamage;
	Monst->mMinDamage2 = Uniq->mMinDamage;
	Monst->mMaxDamage2 = Uniq->mMaxDamage;
	Monst->mMagicRes = Uniq->mMagicRes;
	Monst->mtalkmsg = Uniq->mtalkmsg;
	Monst->mlid = AddLight(Monst->_mx, Monst->_my, 3);

	if ( gbMaxPlayers == 1 )
	{
		if ( Monst->mtalkmsg )
		{
			Monst->_mgoal = 6;
		}
	}
	else
	{
		if ( Monst->_mAi == AI_LAZHELP )
		{
			Monst->mtalkmsg = 0;
		}

		if ( Monst->_mAi != AI_LAZURUS || quests[15]._qvar1 <= 3 )
		{
			if ( Monst->mtalkmsg )
			{
				Monst->_mgoal = 6;
			}
		}
		else
		{
			Monst->_mgoal = 1;
		}
	}

	if ( gnDifficulty == DIFF_NIGHTMARE )
	{
		Monst->mLevel += 15;
		Monst->_mmaxhp = 3 * Monst->_mmaxhp + 64;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 2 * (Monst->mExp + 1000);
		Monst->mMinDamage = 2 * (Monst->mMinDamage + 2);
		Monst->mMaxDamage = 2 * (Monst->mMaxDamage + 2);
		Monst->mMinDamage2 = 2 * (Monst->mMinDamage2 + 2);
		Monst->mMaxDamage2 = 2 * (Monst->mMaxDamage2 + 2);
	}

	if ( gnDifficulty == DIFF_HELL )
	{
		Monst->mLevel += 30;
		Monst->_mmaxhp = 4 * Monst->_mmaxhp + 192;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 4 * (Monst->mExp + 1000);
		Monst->mMinDamage = 4 * Monst->mMinDamage + 6;
		Monst->mMaxDamage = 4 * Monst->mMaxDamage + 6;
		Monst->mMinDamage2 = 4 * Monst->mMinDamage2 + 6;
		Monst->mMaxDamage2 = 4 * Monst->mMaxDamage2 + 6;
	}

	sprintf(filestr, "Monsters\\Monsters\\%s.TRN", Uniq->mMode);
	LoadFileWithMem(filestr, &pLightTbl[256 * (uniquetrans + 19)]);

	Monst->_uniqtrans = uniquetrans++;

	if ( Uniq->mUnqAttr & 4 )
	{
		Monst->mHit = Uniq->mUnqVar1;
		Monst->mHit2 = Uniq->mUnqVar1;
	}
	if ( Uniq->mUnqAttr & 8 )
	{
		Monst->mArmorClass = Uniq->mUnqVar1;
	}

	nummonsters++;

	if ( Uniq->mUnqAttr & 1 )
	{
		PlaceGroup(miniontype, unpackfilesize, Uniq->mUnqAttr, nummonsters - 1);
	}

	if ( Monst->_mAi != AI_GARG )
	{
		Monst->_mAnimData = Monst->MType->Anims[0].Frames[Monst->_mdir];
		Monst->_mAnimFrame = random(88, Monst->_mAnimLen - 1) + 1;
		Monst->_mFlags &= 0xFFFFFFFB;
		Monst->_mmode = MM_STAND;
	}
}

void __cdecl PlaceQuestMonsters()
{
	int skeltype;
	unsigned char *setp;

	if ( !setlevel )
	{
		if ( QuestStatus(QTYPE_BUTCH) )
		{
			PlaceUniqueMonst(9, 0, 0);
		}

		if ( currlevel == quests[12]._qlevel && gbMaxPlayers != 1 )
		{
			skeltype = 0;

			for ( skeltype = 0; skeltype < nummtypes; skeltype++ )
			{
				if ( IsSkel(Monsters[skeltype].mtype) )
				{
					break;
				}
			}

			PlaceUniqueMonst(1, skeltype, 30);
		}

		if ( QuestStatus(QTYPE_BOL) )
		{
			setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if ( QuestStatus(QTYPE_BLOOD) )
		{
			setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if ( QuestStatus(QTYPE_BLIND) )
		{
			setp = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if ( QuestStatus(QTYPE_ANVIL) )
		{
			setp = LoadFileInMem("Levels\\L3Data\\Anvil.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x + 2, 2 * setpc_y + 2);
			mem_free_dbg(setp);
		}
		if ( QuestStatus(QTYPE_WARLRD) )
		{
			setp = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
			AddMonsterType(UniqMonst[8].mtype, 1);
		}
		if ( QuestStatus(QTYPE_VEIL) )
		{
			AddMonsterType(UniqMonst[7].mtype, 1);
		}
		if ( QuestStatus(QTYPE_ZHAR) && zharlib == -1 )
		{
			quests[3]._qactive = 0;
		}

		if ( currlevel == quests[15]._qlevel && gbMaxPlayers != 1 )
		{
			AddMonsterType(UniqMonst[4].mtype, 4);
			AddMonsterType(UniqMonst[5].mtype, 4);
			PlaceUniqueMonst(4, 0, 0);
			PlaceUniqueMonst(5, 0, 0);
			PlaceUniqueMonst(6, 0, 0);
			setp = LoadFileInMem("Levels\\L4Data\\Vile1.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
	}
	else
	{
		if ( setlvlnum == SL_SKELKING )
		{
			PlaceUniqueMonst(1, 0, 0);
		}
	}
}

void __fastcall PlaceGroup(int mtype, int num, int leaderf, int leader)
{
	int placed = 0;
	int xp;
	int yp;
	int x1;
	int y1;

	for ( int try1 = 0; try1 < 10; try1++ )
	{
		while ( placed )
		{
			nummonsters--;
			placed--;
			dMonster[monster[nummonsters]._mx][monster[nummonsters]._my] = 0;
		}

		if ( leaderf & 1 )
		{
			int offset = random(92, 8);
			xp = monster[leader]._mx + offset_x[offset];
			yp = monster[leader]._my + offset_y[offset];
			x1 = xp;
			y1 = yp;
		}
		else
		{
			do
			{
				xp = random(93, 80) + 16;
				x1 = xp;
				yp = random(93, 80) + 16;
				y1 = yp;
			}
			while ( !MonstPlace(xp, yp) );
		}

		if ( num + nummonsters > totalmonsters )
		{
			num = totalmonsters - nummonsters;
		}

		int j = 0;
		for ( int try2 = 0; j < num && try2 < 100; xp += offset_x[random(94, 8)], yp += offset_x[random(94, 8)] )
		{
			if ( !MonstPlace(xp, yp)
				|| (dung_map[x1][y1] != dung_map[xp][yp])
				|| (leaderf & 2) && ((abs(xp - x1) >= 4) || (abs(yp - y1) >= 4)) )
			{
				try2++;
				continue;
			}

			PlaceMonster(nummonsters, mtype, xp, yp);
			if ( leaderf & 1 )
			{
				monster[nummonsters]._mmaxhp *= 2;
				monster[nummonsters]._mhitpoints = monster[nummonsters]._mmaxhp;
				monster[nummonsters]._mint = monster[leader]._mint;

				if ( leaderf & 2 )
				{
					monster[nummonsters].leader = leader;
					monster[nummonsters].leaderflag = 1;
					monster[nummonsters]._mAi = monster[leader]._mAi;
				}

				if ( monster[nummonsters]._mAi != AI_GARG )
				{
					monster[nummonsters]._mAnimData = monster[nummonsters].MType->Anims[0].Frames[monster[nummonsters]._mdir];
					monster[nummonsters]._mAnimFrame = random(88, monster[nummonsters]._mAnimLen - 1) + 1;
					monster[nummonsters]._mFlags &= 0xFFFFFFFB;
					monster[nummonsters]._mmode = MM_STAND;
				}

			}
			nummonsters++;
			placed++;
			j++;
		}

		if ( placed >= num )
		{
			break;
		}
	}

	if ( leaderf & 2 )
	{
		monster[leader].unpackfilesize = placed;
	}
}

void __cdecl LoadDiabMonsts()
{
	unsigned char *lpSetPiece; // esi

	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab1.DUN", 0);
	SetMapMonsters(lpSetPiece, 2 * diabquad1x, 2 * diabquad1y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", 0);
	SetMapMonsters(lpSetPiece, 2 * diabquad2x, 2 * diabquad2y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", 0);
	SetMapMonsters(lpSetPiece, 2 * diabquad3x, 2 * diabquad3y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4a.DUN", 0);
	SetMapMonsters(lpSetPiece, 2 * diabquad4x, 2 * diabquad4y);
	mem_free_dbg(lpSetPiece);
}
// 5289C4: using guessed type int diabquad1x;
// 5289C8: using guessed type int diabquad1y;

void __cdecl InitMonsters()
{
	int v0; // ebp
	int v1; // ebx
	TriggerStruct *v2; // esi
	signed int v3; // ebp
	signed int v4; // edi
	int v5; // edi
	int v6; // esi
	int v7; // eax
	int v8; // ecx
	int v9; // edx
	int v10; // eax
	int v11; // esi
	unsigned char *v12; // edi
	int v13; // ebx
	int v15; // esi
	int v17; // eax
	int v18; // eax
	int v19; // ebx
	TriggerStruct *v20; // esi
	signed int v21; // ebp
	signed int v22; // edi
	int max; // [esp+10h] [ebp-1C4h]
	int v24; // [esp+14h] [ebp-1C0h]
	int scattertypes[111]; // [esp+18h] [ebp-1BCh]

	v0 = 0;
	max = 0;
	if ( gbMaxPlayers != 1 )
		CheckDungeonClear();
	if ( !setlevel )
	{
		AddMonster(1, 0, 0, 0, 0);
		AddMonster(1, 0, 0, 0, 0);
		AddMonster(1, 0, 0, 0, 0);
		AddMonster(1, 0, 0, 0, 0);
		if ( !setlevel && currlevel == 16 )
			LoadDiabMonsts();
	}
	v24 = trigflag[4];
	if ( currlevel == 15 )
		v24 = 1;
	v1 = v24;
	if ( v24 > 0 )
	{
		v2 = trigs;
		do
		{
			v3 = -2;
			do
			{
				v4 = -2;
				do
					DoVision(v3 + v2->_tx, v4++ + v2->_ty, 15, 0, 0);
				while ( v4 < 2 );
				++v3;
			}
			while ( v3 < 2 );
			++v2;
			--v1;
		}
		while ( v1 );
		v0 = 0;
	}
	PlaceQuestMonsters();
	if ( !setlevel )
	{
		PlaceUniques();
		v5 = 16;
		do
		{
			v6 = 16;
			do
			{
				if ( !SolidLoc(v5, v6) )
					++v0;
				++v6;
			}
			while ( v6 < 96 );
			++v5;
		}
		while ( v5 < 96 );
		v7 = v0 / 30;
		if ( gbMaxPlayers != 1 )
			v7 += v7 >> 1;
		v8 = nummonsters;
		if ( nummonsters + v7 > 190 )
			v7 = 190 - nummonsters;
		v9 = nummtypes;
		v10 = nummonsters + v7;
		v11 = 0;
		totalmonsters = v10;
		if ( nummtypes > 0 )
		{
			v12 = &Monsters[0].mPlaceFlags;
			do
			{
				if ( *v12 & 1 )
				{
					v13 = max++;
					scattertypes[v13] = v11;
				}
				++v11;
				v12 += 328;
			}
			while ( v11 < v9 );
		}
		if ( v8 < v10 )
		{
			while ( 1 )
			{
				v15 = scattertypes[random(95, max)];
				if ( currlevel == 1 )
					break;
				if ( !random(95, 2) )
					break;
				if ( currlevel == 2 )
				{
					v17 = random(95, 2) + 1;
				LABEL_40:
					v18 = v17 + 1;
					goto LABEL_41;
				}
				v18 = random(95, 3) + 3;
			LABEL_41:
				PlaceGroup(v15, v18, 0, 0);
				if ( nummonsters >= totalmonsters )
					goto LABEL_42;
			}
			v17 = 0;
			goto LABEL_40;
		}
	}
LABEL_42:
	v19 = v24;
	if ( v24 > 0 )
	{
		v20 = trigs;
		do
		{
			v21 = -2;
			do
			{
				v22 = -2;
				do
					DoUnVision(v21 + v20->_tx, v22++ + v20->_ty, 15);
				while ( v22 < 2 );
				++v21;
			}
			while ( v21 < 2 );
			++v20;
			--v19;
		}
		while ( v19 );
	}
}
// 5CF31D: using guessed type char setlevel;
// 658550: using guessed type int totalmonsters;
// 679660: using guessed type char gbMaxPlayers;
// 432637: using guessed type int var_1BC[111];

void __cdecl PlaceUniques()
{
	int v0; // edi
	int v1; // eax
	UniqMonstStruct *v2; // ecx
	int v3; // eax
	int v4; // edx
	CMonster *v5; // esi
	int v6; // eax
	int v7; // edx

	v0 = 0;
	if ( UniqMonst[0].mtype != -1 )
	{
		v1 = 0;
		v2 = UniqMonst;
		while ( UniqMonst[v1].mlevel != currlevel )
		{
		LABEL_25:
			v1 = ++v0;
			v2 = &UniqMonst[v0];
			if ( v2->mtype == -1 )
				return;
		}
		v3 = 0;
		v4 = 0;
		if ( nummtypes > 0 )
		{
			v5 = Monsters;
			do
			{
				if ( v3 )
					break;
				v6 = -((char)v2->mtype != (unsigned char)v5->mtype);
				++v5;
				v3 = v6 + 1;
				++v4;
			}
			while ( v4 < nummtypes );
		}
		v7 = v4 - 1;
		if ( !v0 )
		{
			if ( quests[2]._qactive )
				goto LABEL_23;
			v3 = 0;
		}
		if ( v0 == 2 )
		{
			if ( quests[3]._qactive )
				goto LABEL_23;
			v3 = 0;
		}
		if ( v0 == 3 )
		{
			if ( quests[7]._qactive )
				goto LABEL_23;
			v3 = 0;
		}
		if ( v0 != 7 )
		{
		LABEL_20:
			if ( v0 == 8 && !quests[11]._qactive )
				v3 = 0;
			goto LABEL_23;
		}
		if ( !quests[4]._qactive )
		{
			v3 = 0;
			goto LABEL_20;
		}
	LABEL_23:
		if ( v3 )
			PlaceUniqueMonst(v0, v7, 8);
		goto LABEL_25;
	}
}

void __fastcall SetMapMonsters(unsigned char *pMap, int startx, int starty)
{
	unsigned char *v3; // esi
	unsigned short v4; // cx
	int v5; // edx
	int v6; // edi
	int v7; // ecx
	unsigned char *v8; // edx
	int i; // esi
	int v10; // eax
	int v11; // ecx
	int v12; // [esp+Ch] [ebp-Ch]
	int v13; // [esp+10h] [ebp-8h]
	unsigned char *v14; // [esp+14h] [ebp-4h]
	int startya; // [esp+20h] [ebp+8h]

	v12 = startx;
	v3 = pMap;
	AddMonsterType(MT_GOLEM, 2);
	AddMonster(1, 0, 0, 0, 0);
	AddMonster(1, 0, 0, 0, 0);
	AddMonster(1, 0, 0, 0, 0);
	AddMonster(1, 0, 0, 0, 0);
	if ( setlevel && setlvlnum == SL_VILEBETRAYER )
	{
		AddMonsterType((char)UniqMonst[4].mtype, 4);
		AddMonsterType((char)UniqMonst[5].mtype, 4);
		AddMonsterType((char)UniqMonst[6].mtype, 4);
		PlaceUniqueMonst(4, 0, 0);
		PlaceUniqueMonst(5, 0, 0);
		PlaceUniqueMonst(6, 0, 0);
	}
	v4 = *((_WORD *)v3 + 1);
	v5 = *(unsigned short *)v3 * v4;
	v6 = (unsigned short)(2 * *(_WORD *)v3);
	v7 = (unsigned short)(2 * v4);
	v8 = &v3[2 * v5 + 4 + 2 * v7 * v6];
	v14 = v8;
	if ( v7 > 0 )
	{
		v13 = v7;
		startya = starty + 16;
		do
		{
			for ( i = 0; i < v6; v14 += 2 )
			{
				if ( *(_WORD *)v8 )
				{
					v10 = AddMonsterType(MonstConvTbl[*(unsigned short *)v8 - 1], 2); /* fix */
					v11 = nummonsters++;
					PlaceMonster(v11, v10, i + v12 + 16, startya);
				}
				v8 = v14 + 2;
				++i;
			}
			++startya;
			--v13;
		}
		while ( v13 );
	}
}
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;

void __fastcall DeleteMonster(int i)
{
	int *v1; // ecx
	int *v2; // eax
	int v3; // edx

	--nummonsters;
	v1 = &monstactive[i];
	v2 = &monstactive[nummonsters];
	v3 = *v2;
	*v2 = *v1;
	*v1 = v3;
}

int __fastcall AddMonster(int x, int y, int dir, int mtype, int InMap)
{
	int i; // esi

	if ( nummonsters >= MAXMONSTERS )
		return -1;
	i = monstactive[nummonsters++];
	if ( InMap )
		dMonster[x][y] = i + 1;
	InitMonster(i, dir, mtype, x, y);
	return i;
}

void __fastcall NewMonsterAnim(int i, AnimStruct *anim, int md)
{
	MonsterStruct *v3; // eax
	int v4; // esi
	int v5; // edx

	v3 = &monster[i];
	v3->_mAnimData = anim->Frames[md];
	v4 = anim->Rate;
	v3->_mAnimCnt = 0;
	v3->_mAnimLen = v4;
	v3->_mAnimFrame = 1;
	v5 = anim->Delay;
	v3->_mFlags &= 0xFFFFFFF9;
	v3->_mAnimDelay = v5;
	v3->_mdir = md;
}

bool __fastcall M_Ranged(int i)
{
	char v1; // cl

	v1 = monster[i]._mAi;
	return v1 == AI_SKELBOW || v1 == AI_GOATBOW || v1 == AI_SUCC || v1 == AI_LAZHELP;
}

bool __fastcall M_Talker(int i)
{
	char v1; // cl

	v1 = monster[i]._mAi;
	return v1 == AI_LAZURUS
		|| v1 == AI_WARLORD
		|| v1 == AI_GARBUD
		|| v1 == AI_ZHAR
		|| v1 == AI_SNOTSPIL
		|| v1 == AI_LACHDAN
		|| v1 == AI_LAZHELP;
}

void __fastcall M_Enemy(int i)
{
	MonsterStruct *v1; // esi
	int *v2; // edi
	int v3; // eax
	int v4; // ecx
	int v5; // ebx
	int v6; // eax
	int v7; // eax
	int v8; // eax
	int v9; // ecx
	int v10; // edi
	//int v11; // edx
	int v12; // eax
	int v13; // ecx
	int v14; // ebx
	int v15; // eax
	int v16; // eax
	int v17; // [esp+Ch] [ebp-20h]
	int v18; // [esp+10h] [ebp-1Ch]
	BOOL v19; // [esp+14h] [ebp-18h]
	BOOL v20; // [esp+14h] [ebp-18h]
	signed int v21; // [esp+18h] [ebp-14h]
	int j; // [esp+18h] [ebp-14h]
	signed int v23; // [esp+1Ch] [ebp-10h]
	signed int v24; // [esp+20h] [ebp-Ch]
	BOOL v25; // [esp+24h] [ebp-8h]
	char v26; // [esp+2Ah] [ebp-2h]
	char v27; // [esp+2Bh] [ebp-1h]

	v24 = -1;
	v18 = i;
	v23 = -1;
	v1 = &monster[i];
	v25 = 0;
	if ( !(v1->_mFlags & 0x20) )
	{
		v21 = 0;
		v2 = &plr[0].plrlevel;
		do
		{
			if ( !*((_BYTE *)v2 - 23) || currlevel != *v2 || *((_BYTE *)v2 + 267) || !v2[89] && gbMaxPlayers != 1 )
				goto LABEL_18;
			v3 = v1->_my;
			v4 = v2[2];
			v19 = dung_map[v2[1]][v4] == dung_map[v1->_mx][v3];
			v5 = abs(v3 - v4);
			if ( abs(v1->_mx - v2[1]) <= v5 )
				v6 = v1->_my - v2[2];
			else
				v6 = v1->_mx - v2[1];
			v7 = abs(v6);
			if ( v19 )
			{
				if ( !v25 )
					goto LABEL_17;
			}
			else if ( v25 )
			{
				goto LABEL_16;
			}
			if ( v7 < v23 )
				goto LABEL_17;
		LABEL_16:
			if ( v24 == -1 )
			{
			LABEL_17:
				v1->_mFlags &= 0xFFFFFFEF;
				v24 = v21;
				v27 = *((_BYTE *)v2 + 12);
				v26 = *((_BYTE *)v2 + 16);
				v23 = v7;
				v25 = v19;
			}
		LABEL_18:
			++v21;
			v2 += 5430;
		}
		while ( (signed int)v2 < (signed int)&plr[4].plrlevel );
	}
	v8 = 0;
	for ( j = 0; j < nummonsters; v8 = j++ + 1 )
	{
		v9 = monstactive[v8];
		v17 = monstactive[v8];
		if ( v9 == v18 )
			continue;
		v10 = v9;
		if ( monster[v9]._mx == 1 && !monster[v10]._my )
			continue;
		if ( M_Talker(v9) && monster[v10].mtalkmsg )
			continue;
		if ( !(v1->_mFlags & 0x20)
			&& ((abs(monster[v10]._mx - v1->_mx) >= 2 || abs(monster[v10]._my - v1->_my) >= 2) && !M_Ranged(v18) /* v11 */
				|| !(v1->_mFlags & 0x20) && !(monster[v10]._mFlags & 0x20)) )
		{
			continue;
		}
		v12 = v1->_my;
		v13 = monster[v10]._my;
		v20 = dung_map[monster[v10]._mx][v13] == dung_map[v1->_mx][v12];
		v14 = abs(v12 - v13);
		if ( abs(v1->_mx - monster[v10]._mx) <= v14 )
			v15 = v1->_my - monster[v10]._my;
		else
			v15 = v1->_mx - monster[v10]._mx;
		v16 = abs(v15);
		if ( v20 )
		{
			if ( !v25 )
				goto LABEL_40;
		}
		else if ( v25 )
		{
			goto LABEL_39;
		}
		if ( v16 < v23 )
			goto LABEL_40;
	LABEL_39:
		if ( v24 == -1 )
		{
		LABEL_40:
			v1->_mFlags |= 0x10u;
			v24 = v17;
			v27 = monster[v10]._mfutx;
			v26 = monster[v10]._mfuty;
			v23 = v16;
			v25 = v20;
		}
	}
	if ( v24 == -1 )
	{
		BYTE1(v1->_mFlags) |= 4u;
	}
	else
	{
		BYTE1(v1->_mFlags) &= 0xFBu;
		v1->_menemy = v24;
		v1->_menemyx = v27;
		v1->_menemyy = v26;
	}
}
// 679660: using guessed type char gbMaxPlayers;

int __fastcall M_GetDir(int i)
{
	return GetDirection(
		monster[i]._mx,
		monster[i]._my,
		(unsigned char)monster[i]._menemyx,
		(unsigned char)monster[i]._menemyy);
}

void __fastcall M_CheckEFlag(int i)
{
	int v1; // ecx
	int v2; // edi
	char *v3; // eax
	signed int v4; // edx

	v1 = i;
	v2 = 0;
	v3 = (char *)dpiece_defs_map_2 + 32 * (112 * (monster[v1]._mx - 1) + monster[v1]._my + 1);
	if ( v3 < (char *)dpiece_defs_map_2 )
		goto LABEL_9;
	v4 = 2;
	do
		v2 |= *(unsigned short *)&v3[2 * v4++];
	while ( v4 < 10 );
	if ( v2 | dArch[monster[v1]._mx - 1][monster[v1]._my + 1] )
		monster[v1]._meflag = 1;
	else
		LABEL_9:
	monster[v1]._meflag = 0;
}

void __fastcall M_StartStand(int i, int md)
{
	int v2; // ebx
	int v3; // edi
	int v4; // esi
	CMonster *v5; // eax
	AnimStruct *v6; // edx
	int v7; // eax
	int v8; // ecx

	v2 = md;
	v3 = i;
	ClearMVars(i);
	v4 = v3;
	v5 = monster[v3].MType;
	v6 = &v5->Anims[1];
	if ( v5->mtype != MT_GOLEM )
		v6 = v5->Anims;
	NewMonsterAnim(v3, v6, v2);
	monster[v4]._mdir = v2;
	monster[v4]._mVar1 = monster[v4]._mmode;
	monster[v4]._mVar2 = 0;
	monster[v4]._mmode = MM_STAND;
	v7 = monster[v4]._mx;
	monster[v4]._mxoff = 0;
	monster[v4]._myoff = 0;
	v8 = monster[v4]._my;
	monster[v4]._mfuty = v8;
	monster[v4]._moldy = v8;
	monster[v4]._mfutx = v7;
	monster[v4]._moldx = v7;
	M_CheckEFlag(v3);
	M_Enemy(v3);
}

void __fastcall M_StartDelay(int i, int len)
{
	int v2; // eax

	if ( len > 0 )
	{
		v2 = i;
		if ( monster[i]._mAi != AI_LAZURUS )
		{
			monster[v2]._mVar2 = len;
			monster[v2]._mmode = MM_DELAY;
		}
	}
}

void __fastcall M_StartSpStand(int i, int md)
{
	int v2; // ebx
	int v3; // esi
	int v4; // edi
	int v5; // eax
	int v6; // ecx

	v2 = i;
	v3 = i;
	v4 = md;
	NewMonsterAnim(i, &monster[i].MType->Anims[5], md);
	v5 = monster[v3]._mx;
	v6 = monster[v3]._my;
	monster[v3]._mxoff = 0;
	monster[v3]._myoff = 0;
	monster[v3]._mdir = v4;
	monster[v3]._mmode = MM_SPSTAND;
	monster[v3]._mfutx = v5;
	monster[v3]._mfuty = v6;
	monster[v3]._moldx = v5;
	monster[v3]._moldy = v6;
	M_CheckEFlag(v2);
}

void __fastcall M_StartWalk(int i, int xvel, int yvel, int xadd, int yadd, int EndDir)
{
	int v6; // ST18_4
	int v7; // esi
	int v8; // eax
	int v9; // ecx
	CMonster *v10; // edx

	v6 = i;
	v7 = i;
	v8 = monster[i]._mx;
	monster[v7]._moldx = v8;
	v9 = monster[i]._my;
	monster[v7]._mfuty = v9 + yadd;
	monster[v7]._mxvel = xvel;
	monster[v7]._myvel = yvel;
	monster[v7]._mVar1 = xadd;
	monster[v7]._mVar2 = yadd;
	dMonster[0][v9 + yadd + 112 * (v8 + xadd)] = -1 - v6;
	v10 = monster[v7].MType;
	monster[v7]._moldy = v9;
	monster[v7]._mmode = MM_WALK;
	monster[v7]._mfutx = v8 + xadd;
	monster[v7]._mVar3 = EndDir;
	monster[v7]._mdir = EndDir;
	NewMonsterAnim(v6, &v10->Anims[1], EndDir);
	monster[v7]._mVar6 = 0;
	monster[v7]._mVar7 = 0;
	monster[v7]._mVar8 = 0;
	M_CheckEFlag(v6);
}

void __fastcall M_StartWalk2(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir)
{
	int v8; // esi
	int v9; // edx
	int v10; // ecx
	int v11; // eax
	int v12; // eax
	bool v13; // zf
	CMonster *v14; // edx
	int v15; // [esp+Ch] [ebp-8h]
	int ia; // [esp+10h] [ebp-4h]
	int EndDira; // [esp+28h] [ebp+14h]

	v15 = xvel;
	ia = i;
	v8 = i;
	v9 = xadd + monster[i]._mx;
	EndDira = monster[i]._mx;
	v10 = monster[i]._my;
	v11 = monster[v8]._my;
	monster[v8]._mVar2 = v10;
	dMonster[0][v10 + 112 * EndDira] = -1 - ia;
	monster[v8]._mVar1 = EndDira;
	monster[v8]._moldx = EndDira;
	v12 = yadd + v11;
	monster[v8]._moldy = v10;
	v13 = monster[v8]._uniqtype == 0;
	monster[v8]._mx = v9;
	monster[v8]._my = v12;
	monster[v8]._mfutx = v9;
	monster[v8]._mfuty = v12;
	dMonster[0][v12 + 112 * v9] = ia + 1;
	if ( !v13 )
		ChangeLightXY((unsigned char)monster[v8].mlid, v9, v12);
	v14 = monster[v8].MType;
	monster[v8]._mxvel = v15;
	monster[v8]._myvel = yvel;
	monster[v8]._mxoff = xoff;
	monster[v8]._myoff = yoff;
	monster[v8]._mmode = MM_WALK2;
	monster[v8]._mVar3 = EndDir;
	monster[v8]._mdir = EndDir;
	NewMonsterAnim(ia, &v14->Anims[1], EndDir);
	monster[v8]._mVar8 = 0;
	monster[v8]._mVar6 = 16 * xoff;
	monster[v8]._mVar7 = 16 * yoff;
	M_CheckEFlag(ia);
}

void __fastcall M_StartWalk3(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir)
{
	int v10; // esi
	int v11; // ebx
	int v12; // edi
	int v13; // edi
	int v14; // ebx
	int v15; // ecx
	CMonster *v16; // edx
	int v17; // [esp+Ch] [ebp-8h]
	int ia; // [esp+10h] [ebp-4h]
	int a6a; // [esp+28h] [ebp+14h]
	int a7a; // [esp+2Ch] [ebp+18h]

	ia = i;
	v10 = i;
	v11 = monster[i]._my;
	v12 = monster[i]._mx;
	v17 = xvel;
	a6a = v12 + xadd;
	a7a = v11 + yadd;
	v13 = mapx + v12;
	v14 = mapy + v11;
	if ( monster[i]._uniqtype )
		ChangeLightXY((unsigned char)monster[v10].mlid, v13, v14);
	v15 = monster[v10]._my + 112 * monster[v10]._mx;
	monster[v10]._mVar4 = v13;
	dMonster[0][v15] = -1 - ia;
	monster[v10]._mVar5 = v14;
	dMonster[0][a7a + 112 * a6a] = -1 - ia;
	monster[v10]._moldx = monster[v10]._mx;
	monster[v10]._moldy = monster[v10]._my;
	monster[v10]._mfutx = a6a;
	monster[v10]._mxvel = v17;
	dFlags[v13][v14] |= 0x10u;
	v16 = monster[v10].MType;
	monster[v10]._myvel = yvel;
	monster[v10]._mfuty = a7a;
	monster[v10]._mVar1 = a6a;
	monster[v10]._mVar2 = a7a;
	monster[v10]._mxoff = xoff;
	monster[v10]._myoff = yoff;
	monster[v10]._mmode = MM_WALK3;
	monster[v10]._mVar3 = EndDir;
	monster[v10]._mdir = EndDir;
	NewMonsterAnim(ia, &v16->Anims[1], EndDir);
	monster[v10]._mVar8 = 0;
	monster[v10]._mVar6 = 16 * xoff;
	monster[v10]._mVar7 = 16 * yoff;
	M_CheckEFlag(ia);
}

void __fastcall M_StartAttack(int i)
{
	int v1; // edi
	int v2; // ebx
	int v3; // esi
	int v4; // ecx
	int v5; // eax

	v1 = i;
	v2 = M_GetDir(i);
	v3 = v1;
	NewMonsterAnim(v1, &monster[v1].MType->Anims[2], v2);
	v4 = monster[v1]._my;
	v5 = monster[v1]._mx;
	monster[v3]._mxoff = 0;
	monster[v3]._myoff = 0;
	monster[v3]._mfuty = v4;
	monster[v3]._moldy = v4;
	monster[v3]._mmode = MM_ATTACK;
	monster[v3]._mfutx = v5;
	monster[v3]._moldx = v5;
	monster[v3]._mdir = v2;
	M_CheckEFlag(v1);
}

void __fastcall M_StartRAttack(int i, int missile_type, int dam)
{
	int v3; // ebp
	int v4; // edi
	int v5; // ebx
	int v6; // esi
	int v7; // ecx
	int v8; // eax

	v3 = missile_type;
	v4 = i;
	v5 = M_GetDir(i);
	v6 = v4;
	NewMonsterAnim(v4, &monster[v4].MType->Anims[2], v5);
	v7 = monster[v4]._my;
	monster[v6]._mxoff = 0;
	monster[v6]._myoff = 0;
	monster[v6]._mVar2 = dam;
	v8 = monster[v4]._mx;
	monster[v6]._mfuty = v7;
	monster[v6]._moldy = v7;
	monster[v6]._mmode = MM_RATTACK;
	monster[v6]._mVar1 = v3;
	monster[v6]._mfutx = v8;
	monster[v6]._moldx = v8;
	monster[v6]._mdir = v5;
	M_CheckEFlag(v4);
}

void __fastcall M_StartRSpAttack(int i, int missile_type, int dam)
{
	int v3; // ebp
	int v4; // edi
	int v5; // ebx
	int v6; // esi
	int v7; // ecx
	int v8; // eax

	v3 = missile_type;
	v4 = i;
	v5 = M_GetDir(i);
	v6 = v4;
	NewMonsterAnim(v4, &monster[v4].MType->Anims[5], v5);
	monster[v6]._mmode = MM_RSPATTACK;
	monster[v6]._mVar2 = 0;
	monster[v6]._mVar3 = dam;
	v7 = monster[v4]._my;
	monster[v6]._mxoff = 0;
	monster[v6]._myoff = 0;
	v8 = monster[v4]._mx;
	monster[v6]._mfuty = v7;
	monster[v6]._moldy = v7;
	monster[v6]._mVar1 = v3;
	monster[v6]._mfutx = v8;
	monster[v6]._moldx = v8;
	monster[v6]._mdir = v5;
	M_CheckEFlag(v4);
}

void __fastcall M_StartSpAttack(int i)
{
	int v1; // edi
	int v2; // ebx
	int v3; // esi
	int v4; // ecx
	int v5; // eax

	v1 = i;
	v2 = M_GetDir(i);
	v3 = v1;
	NewMonsterAnim(v1, &monster[v1].MType->Anims[5], v2);
	v4 = monster[v1]._my;
	v5 = monster[v1]._mx;
	monster[v3]._mxoff = 0;
	monster[v3]._myoff = 0;
	monster[v3]._mfuty = v4;
	monster[v3]._moldy = v4;
	monster[v3]._mmode = MM_SATTACK;
	monster[v3]._mfutx = v5;
	monster[v3]._moldx = v5;
	monster[v3]._mdir = v2;
	M_CheckEFlag(v1);
}

void __fastcall M_StartEat(int i)
{
	int v1; // edi
	int v2; // esi
	int v3; // ecx
	int v4; // eax

	v1 = i;
	v2 = i;
	NewMonsterAnim(i, &monster[i].MType->Anims[5], monster[i]._mdir);
	v3 = monster[v2]._my;
	v4 = monster[v2]._mx;
	monster[v2]._mxoff = 0;
	monster[v2]._myoff = 0;
	monster[v2]._mfuty = v3;
	monster[v2]._moldy = v3;
	monster[v2]._mmode = MM_SATTACK;
	monster[v2]._mfutx = v4;
	monster[v2]._moldx = v4;
	M_CheckEFlag(v1);
}

void __fastcall M_ClearSquares(int i)
{
	int v1; // edx
	int v2; // eax
	int v3; // esi
	int v4; // ecx
	int v5; // edi
	int v6; // [esp+8h] [ebp-Ch]
	_DWORD *v7; // [esp+Ch] [ebp-8h]
	int v8; // [esp+10h] [ebp-4h]

	v1 = monster[i]._moldx;
	v2 = monster[i]._moldy;
	v3 = -1 - i;
	v6 = i + 1;
	v4 = v2 - 1;
	v5 = v2 + 1;
	if ( (unsigned char)(__OFSUB__(v2 - 1, v2 + 1) ^ 1) | (v2 - 1 == v2 + 1) )
	{
		do
		{
			if ( v4 >= 0 && v4 < 112 )
			{
				v8 = v1 - 1;
				if ( (unsigned char)(__OFSUB__(v1 - 1, v1 + 1) ^ 1) | (v1 - 1 == v1 + 1) )
				{
					v7 = (_DWORD *)((char *)dMonster + 4 * (v4 + 112 * (v1 - 1)));
					do
					{
						if ( v8 >= 0 && v8 < 112 && (*v7 == v3 || *v7 == v6) )
							*v7 = 0;
						++v8;
						v7 += 112;
					}
					while ( v8 <= v1 + 1 );
				}
			}
			++v4;
			v5 = v2 + 1;
		}
		while ( v4 <= v2 + 1 );
	}
	if ( v1 + 1 < 112 )
		dFlags[v1 + 1][v2] &= 0xEFu;
	if ( v5 < 112 )
		dFlags[v1][v2 + 1] &= 0xEFu;
}

void __fastcall M_GetKnockback(int i)
{
	int v1; // edi
	int v2; // esi
	int v3; // ebx
	//int v4; // eax
	int v5; // ST00_4
	AnimStruct *v6; // edx
	int v7; // eax
	int v8; // ecx
	int v9; // eax

	v1 = i;
	v2 = i;
	v3 = ((unsigned char)monster[i]._mdir - 4) & 7;
	//_LOBYTE(v4) = DirOK(i, v3);
	if ( DirOK(i, v3) )
	{
		M_ClearSquares(v1);
		v5 = monster[v2]._mdir;
		v6 = &monster[v2].MType->Anims[3];
		v7 = offset_y[v3];
		monster[v2]._moldx += offset_x[v3];
		monster[v2]._moldy += v7;
		NewMonsterAnim(v1, v6, v5);
		v8 = monster[v2]._moldy;
		v9 = monster[v2]._moldx;
		monster[v2]._mxoff = 0;
		monster[v2]._myoff = 0;
		monster[v2]._my = v8;
		monster[v2]._mfuty = v8;
		monster[v2]._mmode = MM_GOTHIT;
		monster[v2]._mx = v9;
		monster[v2]._mfutx = v9;
		M_CheckEFlag(v1);
		M_ClearSquares(v1);
		dMonster[0][monster[v2]._my + 112 * monster[v2]._mx] = v1 + 1;
	}
}

void __fastcall M_StartHit(int i, int pnum, int dam)
{
	int v3; // ebx
	int v4; // edi
	int v5; // esi
	unsigned char v6; // al
	char v7; // al
	unsigned char v8; // al
	int v9; // ecx
	int v10; // eax

	v3 = pnum;
	v4 = i;
	if ( pnum >= 0 )
		monster[i].mWhoHit |= 1 << pnum;
	if ( pnum == myplr )
	{
		delta_monster_hp(i, monster[i]._mhitpoints, currlevel);
		NetSendCmdParam2(0, CMD_MONSTDAMAGE, v4, dam);
	}
	PlayEffect(v4, 1);
	v5 = v4;
	v6 = monster[v4].MType->mtype;
	if ( v6 >= MT_SNEAK && v6 <= MT_ILLWEAV || dam >> 6 >= SLOBYTE(monster[v5].mLevel) + 3 )
	{
		if ( v3 >= 0 )
		{
			monster[v5]._mFlags &= 0xFFFFFFEF;
			monster[v5]._menemy = v3;
			v7 = plr[v3]._py;
			monster[v5]._menemyx = plr[v3]._px;
			monster[v5]._menemyy = v7;
			monster[v5]._mdir = M_GetDir(v4);
		}
		v8 = monster[v5].MType->mtype;
		if ( v8 == MT_BLINK )
		{
			M_Teleport(v4);
		}
		else if ( v8 >= MT_NSCAV && v8 <= MT_YSCAV )
		{
			_LOBYTE(monster[v5]._mgoal) = 1;
		}
		if ( monster[v5]._mmode != MM_STONE )
		{
			NewMonsterAnim(v4, &monster[v5].MType->Anims[3], monster[v5]._mdir);
			v9 = monster[v5]._moldy;
			v10 = monster[v5]._moldx;
			monster[v5]._mxoff = 0;
			monster[v5]._myoff = 0;
			monster[v5]._my = v9;
			monster[v5]._mfuty = v9;
			monster[v5]._mmode = MM_GOTHIT;
			monster[v5]._mx = v10;
			monster[v5]._mfutx = v10;
			M_CheckEFlag(v4);
			M_ClearSquares(v4);
			dMonster[0][monster[v5]._my + 112 * monster[v5]._mx] = v4 + 1;
		}
	}
}

void __fastcall M_DiabloDeath(int i, unsigned char sendmsg)
{
	int v2; // esi
	int v3; // edi
	int v4; // eax
	int v5; // ebx
	int v6; // esi
	int v7; // ecx
	int v8; // eax
	int v9; // esi
	int v10; // eax
	double v11; // st7
	int v12; // eax
	int v13; // ecx
	int v14; // esi
	int v15; // [esp+8h] [ebp-8h]
	int j; // [esp+Ch] [ebp-4h]
	int v17; // [esp+Ch] [ebp-4h]

	v15 = i;
	v2 = sendmsg;
	v3 = i;
	PlaySFX(USFX_DIABLOD);
	quests[5]._qactive = 3;
	if ( v2 )
		NetSendCmdQuest(1u, 5u);
	gbProcessPlayers = 0;
	_LOBYTE(sgbSaveSoundOn) = gbSoundOn;
	v4 = 0;
	for ( j = 0; j < nummonsters; ++j )
	{
		v5 = monstactive[v4];
		if ( v5 != v15 && monster[v3]._msquelch )
		{
			v6 = v5;
			NewMonsterAnim(monstactive[v4], &monster[v5].MType->Anims[4], monster[v5]._mdir);
			v7 = monster[v5]._moldy;
			monster[v6]._mxoff = 0;
			monster[v6]._myoff = 0;
			monster[v6]._mVar1 = 0;
			v8 = monster[v5]._moldx;
			monster[v6]._my = v7;
			monster[v6]._mfuty = v7;
			monster[v6]._mmode = MM_DEATH;
			monster[v6]._mx = v8;
			monster[v6]._mfutx = v8;
			M_CheckEFlag(v5);
			M_ClearSquares(v5);
			dMonster[0][monster[v6]._my + 112 * monster[v6]._mx] = v5 + 1;
		}
		v4 = j + 1;
	}
	AddLight(monster[v3]._mx, monster[v3]._my, 8);
	DoVision(monster[v3]._mx, monster[v3]._my, 8, 0, 1);
	v9 = abs(ViewY - monster[v3]._my);
	if ( abs(ViewX - monster[v3]._mx) <= v9 )
		v10 = ViewY - monster[v3]._my;
	else
		v10 = ViewX - monster[v3]._mx;
	v17 = abs(v10);
	if ( v17 > 20 )
		v17 = 20;
	v11 = (double)v17;
	v12 = ViewX << 16;
	v13 = monster[v3]._mx << 16;
	monster[v3]._mVar3 = ViewX << 16;
	v14 = ViewY << 16;
	monster[v3]._mVar4 = ViewY << 16;
	monster[v3]._mVar5 = (signed __int64)((double)(v12 - v13) / v11);
	monster[v3]._mVar6 = (signed __int64)((double)(v14 - (monster[v3]._my << 16)) / v11);
}
// 4A22D5: using guessed type char gbSoundOn;
// 5256A0: using guessed type int gbProcessPlayers;
// 64D32C: using guessed type int sgbSaveSoundOn;

void __fastcall M2MStartHit(int mid, int i, int dam)
{
	int v3; // edi
	int v4; // ebx
	int v5; // esi
	CMonster *v6; // eax
	char v7; // al
	CMonster *v8; // eax
	int v9; // ecx
	int v10; // eax
	int v11; // [esp+Ch] [ebp-4h]

	v3 = mid;
	v4 = i;
	v11 = i;
	if ( (unsigned int)mid >= MAXMONSTERS )
		TermMsg("Invalid monster %d getting hit by monster", mid);
	v5 = v3;
	if ( !monster[v3].MType )
		TermMsg("Monster %d \"%s\" getting hit by monster: MType NULL", v3, monster[v5].mName);
	if ( v4 >= 0 )
		monster[v4].mWhoHit |= 1 << v4;
	delta_monster_hp(v3, monster[v5]._mhitpoints, currlevel);
	NetSendCmdParam2(0, CMD_MONSTDAMAGE, v3, dam);
	PlayEffect(v3, 1);
	v6 = monster[v5].MType;
	if ( v6->mtype >= MT_SNEAK && v6->mtype <= MT_ILLWEAV || dam >> 6 >= SLOBYTE(monster[v5].mLevel) + 3 )
	{
		if ( v11 >= 0 )
			monster[v5]._mdir = ((unsigned char)monster[v11]._mdir - 4) & 7;
		v7 = v6->mtype;
		if ( v7 == 39 )
		{
			M_Teleport(v3);
		}
		else if ( v7 >= MT_NSCAV && v7 <= MT_YSCAV )
		{
			_LOBYTE(monster[v5]._mgoal) = 1;
		}
		if ( monster[v5]._mmode != MM_STONE )
		{
			v8 = monster[v5].MType;
			if ( v8->mtype != MT_GOLEM )
			{
				NewMonsterAnim(v3, &v8->Anims[3], monster[v5]._mdir);
				monster[v5]._mmode = MM_GOTHIT;
			}
			v9 = monster[v5]._moldy;
			v10 = monster[v5]._moldx;
			monster[v5]._mxoff = 0;
			monster[v5]._myoff = 0;
			monster[v5]._my = v9;
			monster[v5]._mfuty = v9;
			monster[v5]._mx = v10;
			monster[v5]._mfutx = v10;
			M_CheckEFlag(v3);
			M_ClearSquares(v3);
			dMonster[0][monster[v5]._my + 112 * monster[v5]._mx] = v3 + 1;
		}
	}
}

void __fastcall MonstStartKill(int i, int pnum, unsigned char sendmsg)
{
	signed int v3; // edi
	int v4; // ebx
	signed int v5; // esi
	int v6; // ecx
	int v7; // eax
	//int v8; // eax
	int v9; // eax
	AnimStruct *v10; // edx
	int v11; // ecx
	int v12; // eax
	unsigned char v13; // al

	v3 = i;
	v4 = pnum;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MonstStartKill: Invalid monster %d", i);
	v5 = v3;
	if ( !monster[v3].MType )
		TermMsg("MonstStartKill: Monster %d \"%s\" MType NULL", v3, monster[v5].mName);
	if ( v4 >= 0 )
		monster[v5].mWhoHit |= 1 << v4;
	if ( v4 < 4 && v3 > 4 )
		AddPlrMonstExper(SLOBYTE(monster[v5].mLevel), (unsigned short)monster[v5].mExp, monster[v5].mWhoHit);
	v6 = monster[v5]._mRndSeed;
	v7 = monster[v5].MType->mtype;
	monster[v5]._mhitpoints = 0;
	++monstkills[v7];
	SetRndSeed(v6);
	//_LOBYTE(v8) = QuestStatus(2);
	if ( QuestStatus(2) && monster[v5].mName == UniqMonst[0].mName )
	{
		CreateTypeItem(monster[v5]._mx + 1, monster[v5]._my + 1, 1u, 4, 0, 1, 0);
	}
	else if ( v3 > 3 )
	{
		SpawnItem(v3, monster[v5]._mx, monster[v5]._my, sendmsg);
	}
	if ( monster[v5].MType->mtype == MT_DIABLO )
		M_DiabloDeath(v3, 1u);
	else
		PlayEffect(v3, 2);
	if ( v4 < 0 )
		v9 = monster[v5]._mdir;
	else
		v9 = M_GetDir(v3);
	v10 = &monster[v5].MType->Anims[4];
	monster[v5]._mdir = v9;
	NewMonsterAnim(v3, v10, v9);
	v11 = monster[v5]._moldy;
	v12 = monster[v5]._moldx;
	monster[v5]._my = v11;
	monster[v5]._mfuty = v11;
	monster[v5]._mmode = MM_DEATH;
	monster[v5]._mxoff = 0;
	monster[v5]._myoff = 0;
	monster[v5]._mVar1 = 0;
	monster[v5]._mx = v12;
	monster[v5]._mfutx = v12;
	M_CheckEFlag(v3);
	M_ClearSquares(v3);
	dMonster[0][monster[v5]._my + 112 * monster[v5]._mx] = v3 + 1;
	CheckQuestKill(v3, sendmsg);
	M_FallenFear(monster[v5]._mx, monster[v5]._my);
	v13 = monster[v5].MType->mtype;
	if ( v13 >= MT_NACID && v13 <= MT_XACID )
		AddMissile(monster[v5]._mx, monster[v5]._my, 0, 0, 0, 59, 1, v3, (unsigned char)monster[v5]._mint + 1, 0);
}

void __fastcall M2MStartKill(int i, int mid)
{
	signed int v2; // ebx
	signed int v3; // edi
	signed int v4; // esi
	int v5; // ecx
	int v6; // eax
	CMonster *v7; // ecx
	int v8; // eax
	int v9; // ecx
	int v10; // eax
	unsigned char v11; // al

	v2 = i;
	v3 = mid;
	if ( (unsigned int)i >= MAXMONSTERS )
	{
		TermMsg("M2MStartKill: Invalid monster (attacker) %d", i);
		TermMsg("M2MStartKill: Invalid monster (killed) %d", v3);
	}
	if ( !monster[v2].MType )
		TermMsg("M2MStartKill: Monster %d \"%s\" MType NULL", v3, monster[v3].mName);
	v4 = v3;
	delta_kill_monster(v3, monster[v3]._mx, monster[v3]._my, currlevel);
	NetSendCmdLocParam1(0, CMD_MONSTDEATH, monster[v4]._mx, monster[v4]._my, v3);
	monster[v4].mWhoHit |= 1 << v2;
	if ( v2 < 4 )
		AddPlrMonstExper(SLOBYTE(monster[v4].mLevel), (unsigned short)monster[v4].mExp, monster[v3].mWhoHit);
	v5 = monster[v4]._mRndSeed;
	v6 = monster[v4].MType->mtype;
	monster[v4]._mhitpoints = 0;
	++monstkills[v6];
	SetRndSeed(v5);
	if ( v3 >= 4 )
		SpawnItem(v3, monster[v4]._mx, monster[v4]._my, 1u);
	if ( monster[v4].MType->mtype == MT_DIABLO )
		M_DiabloDeath(v3, 1u);
	else
		PlayEffect(v2, 2);
	PlayEffect(v3, 2);
	v7 = monster[v4].MType;
	v8 = ((unsigned char)monster[v2]._mdir - 4) & 7;
	if ( v7->mtype == MT_GOLEM )
		v8 = 0;
	monster[v4]._mdir = v8;
	NewMonsterAnim(v3, &v7->Anims[4], v8);
	v9 = monster[v4]._moldy;
	v10 = monster[v4]._moldx;
	monster[v4]._my = v9;
	monster[v4]._mfuty = v9;
	monster[v4]._mmode = MM_DEATH;
	monster[v4]._mxoff = 0;
	monster[v4]._myoff = 0;
	monster[v4]._mx = v10;
	monster[v4]._mfutx = v10;
	M_CheckEFlag(v3);
	M_ClearSquares(v3);
	dMonster[0][monster[v4]._my + 112 * monster[v4]._mx] = v3 + 1;
	CheckQuestKill(v3, 1u);
	M_FallenFear(monster[v4]._mx, monster[v4]._my);
	v11 = monster[v4].MType->mtype;
	if ( v11 >= MT_NACID && v11 <= MT_XACID )
		AddMissile(monster[v4]._mx, monster[v4]._my, 0, 0, 0, 59, 1, v3, (unsigned char)monster[v4]._mint + 1, 0);
}

void __fastcall M_StartKill(int i, int pnum)
{
	int v2; // edi
	int v3; // ebx
	int v4; // esi
	int v5; // eax

	v2 = i;
	v3 = pnum;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_StartKill: Invalid monster %d", i);
	if ( myplr == v3 )
	{
		v4 = v2;
		delta_kill_monster(v2, monster[v2]._mx, monster[v2]._my, currlevel);
		if ( v2 == v3 )
		{
			_LOWORD(v5) = currlevel;
			NetSendCmdLocParam1(0, CMD_KILLGOLEM, monster[v4]._mx, monster[v4]._my, v5);
		}
		else
		{
			NetSendCmdLocParam1(0, CMD_MONSTDEATH, monster[v4]._mx, monster[v4]._my, v2);
		}
	}
	MonstStartKill(v2, v3, 1u);
}

void __fastcall M_SyncStartKill(int i, int x, int y, int pnum)
{
	int v4; // esi
	int v5; // ebx
	int v6; // esi
	int arglist; // [esp+Ch] [ebp-4h]

	v4 = i;
	v5 = x;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_SyncStartKill: Invalid monster %d", i);
	v6 = v4;
	if ( monster[v6]._mhitpoints && monster[v6]._mmode != MM_DEATH )
	{
		if ( !dMonster[0][y + 112 * v5] )
		{
			M_ClearSquares(arglist);
			monster[v6]._mx = v5;
			monster[v6]._my = y;
			monster[v6]._moldx = v5;
			monster[v6]._moldy = y;
		}
		if ( monster[v6]._mmode == MM_STONE )
		{
			MonstStartKill(arglist, pnum, 0);
			monster[v6]._mmode = MM_STONE;
		}
		else
		{
			MonstStartKill(arglist, pnum, 0);
		}
	}
}

void __fastcall M_StartFadein(int i, int md, unsigned char backwards)
{
	int v3; // esi
	int v4; // ebx
	int v5; // esi
	int v6; // ecx
	int v7; // eax
	int *v8; // eax
	int arglist; // [esp+Ch] [ebp-4h]

	v3 = i;
	v4 = md;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_StartFadein: Invalid monster %d", i);
	v5 = v3;
	if ( !monster[v5].MType )
		TermMsg("M_StartFadein: Monster %d \"%s\" MType NULL", arglist, monster[v5].mName);
	NewMonsterAnim(arglist, &monster[v5].MType->Anims[5], v4);
	v6 = monster[v5]._my;
	v7 = monster[v5]._mx;
	monster[v5]._mfuty = v6;
	monster[v5]._moldy = v6;
	monster[v5]._mmode = MM_FADEIN;
	monster[v5]._mxoff = 0;
	monster[v5]._myoff = 0;
	monster[v5]._mfutx = v7;
	monster[v5]._moldx = v7;
	M_CheckEFlag(arglist);
	v8 = &monster[v5]._mFlags;
	monster[v5]._mdir = v4;
	*v8 &= 0xFFFFFFFE;
	if ( backwards )
	{
		*v8 = monster[v5]._mFlags | 2;
		monster[v5]._mAnimFrame = monster[v5]._mAnimLen;
	}
}

void __fastcall M_StartFadeout(int i, int md, unsigned char backwards)
{
	int v3; // ebx
	int v4; // esi
	CMonster **v5; // edi
	int v6; // ecx
	int v7; // eax
	int v8; // eax
	int mda; // [esp+Ch] [ebp-4h]

	v3 = i;
	mda = md;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_StartFadeout: Invalid monster %d", i);
	v4 = v3;
	v5 = &monster[v3].MType;
	if ( !*v5 )
		TermMsg("M_StartFadeout: Monster %d \"%s\" MType NULL", v3, monster[v4].mName);
	NewMonsterAnim(v3, &(*v5)->Anims[5], mda);
	v6 = monster[v4]._my;
	v7 = monster[v4]._mx;
	monster[v4]._mfuty = v6;
	monster[v4]._moldy = v6;
	monster[v4]._mmode = MM_FADEOUT;
	monster[v4]._mxoff = 0;
	monster[v4]._myoff = 0;
	monster[v4]._mfutx = v7;
	monster[v4]._moldx = v7;
	M_CheckEFlag(v3);
	monster[v4]._mdir = mda;
	if ( backwards )
	{
		v8 = monster[v4]._mAnimLen;
		monster[v4]._mFlags |= 2u;
		monster[v4]._mAnimFrame = v8;
	}
}

void __fastcall M_StartHeal(int i)
{
	int v1; // edi
	int v2; // esi
	CMonster *v3; // eax
	unsigned char *v4; // ecx
	int v5; // eax

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_StartHeal: Invalid monster %d", i);
	v2 = v1;
	if ( !monster[v1].MType )
		TermMsg("M_StartHeal: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
	v3 = monster[v2].MType;
	v4 = v3->Anims[5].Frames[monster[v2]._mdir];
	monster[v2]._mAnimData = v4;
	v5 = v3->Anims[5].Rate;
	monster[v2]._mFlags |= 2u;
	monster[v2]._mAnimFrame = v5;
	monster[v2]._mmode = MM_HEAL;
	monster[v2]._mVar1 = monster[v2]._mmaxhp / (16 * (random(97, 5) + 4));
}

void __fastcall M_ChangeLightOffset(int monst)
{
	int v1; // esi
	int v2; // ecx
	int v3; // eax
	int v4; // esi
	int v5; // edx
	int v6; // eax
	signed int v7; // esi
	int v8; // edx
	signed int v9; // esi

	v1 = monst;
	if ( (unsigned int)monst >= MAXMONSTERS )
		TermMsg("M_ChangeLightOffset: Invalid monster %d", monst);
	v2 = v1;
	v3 = monster[v1]._myoff;
	v4 = monster[v1]._mxoff;
	v3 *= 2;
	v5 = v4 + v3;
	v6 = v3 - v4;
	if ( v5 >= 0 )
	{
		v7 = 1;
	}
	else
	{
		v7 = -1;
		v5 = -v5;
	}
	v8 = v7 * (v5 >> 3);
	if ( v6 >= 0 )
	{
		v9 = 1;
	}
	else
	{
		v9 = -1;
		v6 = -v6;
	}
	ChangeLightOff((unsigned char)monster[v2].mlid, v8, v9 * (v6 >> 3));
}

int __fastcall M_DoStand(int i)
{
	int v1; // edi
	int v2; // esi
	CMonster *v3; // eax
	int v4; // ecx
	unsigned char *v5; // eax

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoStand: Invalid monster %d", i);
	v2 = v1;
	if ( !monster[v1].MType )
		TermMsg("M_DoStand: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
	v3 = monster[v2].MType;
	v4 = monster[v2]._mdir;
	if ( v3->mtype == MT_GOLEM )
		v5 = v3->Anims[1].Frames[v4];
	else
		v5 = v3->Anims[0].Frames[v4];
	monster[v2]._mAnimData = v5;
	if ( monster[v2]._mAnimFrame == monster[v2]._mAnimLen )
		M_Enemy(v1);
	++monster[v2]._mVar2;
	return 0;
}

int __fastcall M_DoWalk(int i)
{
	int v1; // ebx
	int v2; // esi
	int v3; // edi
	int v4; // eax
	int v5; // edi
	int v6; // ecx
	int v7; // edx
	int v8; // eax
	bool v9; // zf
	int v10; // ecx
	int v11; // edx
	int v12; // eax
	int v13; // ecx

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoWalk: Invalid monster %d", i);
	v2 = v1;
	v3 = 0;
	if ( !monster[v1].MType )
		TermMsg("M_DoWalk: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
	v4 = monster[v2]._mVar8;
	if ( v4 == monster[v2].MType->Anims[1].Rate )
	{
		v5 = monster[v2]._my;
		v6 = monster[v2]._mx;
		dMonster[0][v5 + 112 * monster[v2]._mx] = 0;
		v7 = v6 + monster[v2]._mVar1;
		monster[v2]._mx = v7;
		v8 = v5 + monster[v2]._mVar2;
		v9 = monster[v2]._uniqtype == 0;
		monster[v2]._my = v8;
		dMonster[0][v8 + 112 * v7] = v1 + 1;
		if ( !v9 )
			ChangeLightXY((unsigned char)monster[v2].mlid, v7, v8);
		M_StartStand(v1, monster[v2]._mdir);
		v3 = 1;
	}
	else if ( !monster[v2]._mAnimCnt )
	{
		v10 = monster[v2]._mxvel;
		v11 = monster[v2]._myvel;
		monster[v2]._mVar8 = v4 + 1;
		monster[v2]._mVar6 += v10;
		v12 = monster[v2]._mVar6 >> 4;
		monster[v2]._mVar7 += v11;
		v13 = monster[v2]._mVar7 >> 4;
		monster[v2]._mxoff = v12;
		monster[v2]._myoff = v13;
	}
	if ( monster[v2]._uniqtype )
		M_ChangeLightOffset(v1);
	return v3;
}

int __fastcall M_DoWalk2(int i)
{
	int v1; // ebp
	int v2; // esi
	int v3; // eax
	bool v4; // zf
	int v5; // edi
	int v6; // ecx
	int v7; // edx
	int v8; // eax
	int v9; // ecx

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoWalk2: Invalid monster %d", i);
	v2 = v1;
	if ( !monster[v1].MType )
		TermMsg("M_DoWalk2: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
	v3 = monster[v2]._mVar8;
	if ( v3 == monster[v2].MType->Anims[1].Rate )
	{
		v4 = monster[v2]._uniqtype == 0;
		dMonster[0][monster[v2]._mVar2 + 112 * monster[v2]._mVar1] = 0;
		if ( !v4 )
			ChangeLightXY((unsigned char)monster[v2].mlid, monster[v2]._mx, monster[v2]._my);
		M_StartStand(v1, monster[v2]._mdir);
		v5 = 1;
	}
	else
	{
		if ( !monster[v2]._mAnimCnt )
		{
			v6 = monster[v2]._mxvel;
			v7 = monster[v2]._myvel;
			monster[v2]._mVar8 = v3 + 1;
			monster[v2]._mVar6 += v6;
			v8 = monster[v2]._mVar6 >> 4;
			monster[v2]._mVar7 += v7;
			v9 = monster[v2]._mVar7 >> 4;
			monster[v2]._mxoff = v8;
			monster[v2]._myoff = v9;
		}
		v5 = 0;
	}
	if ( monster[v2]._uniqtype )
		M_ChangeLightOffset(v1);
	return v5;
}

int __fastcall M_DoWalk3(int i)
{
	int v1; // ebp
	int v2; // esi
	int v3; // eax
	int v4; // edi
	int v5; // edx
	int v6; // ecx
	int v7; // edx
	char *v8; // eax
	bool v9; // zf
	int v10; // edi
	int v11; // ecx
	int v12; // edx
	int v13; // eax
	int v14; // ecx

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoWalk3: Invalid monster %d", i);
	v2 = v1;
	if ( !monster[v1].MType )
		TermMsg("M_DoWalk3: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
	v3 = monster[v2]._mVar8;
	if ( v3 == monster[v2].MType->Anims[1].Rate )
	{
		v4 = monster[v2]._mVar2;
		v5 = monster[v2]._my + 112 * monster[v2]._mx;
		monster[v2]._my = v4;
		v6 = monster[v2]._mVar5;
		dMonster[0][v5] = 0;
		v7 = monster[v2]._mVar1;
		monster[v2]._mx = v7;
		v8 = &dFlags[monster[v2]._mVar4][v6];
		*v8 &= 0xEFu;
		v9 = monster[v2]._uniqtype == 0;
		dMonster[0][v4 + 112 * v7] = v1 + 1;
		if ( !v9 )
			ChangeLightXY((unsigned char)monster[v2].mlid, v7, v4);
		M_StartStand(v1, monster[v2]._mdir);
		v10 = 1;
	}
	else
	{
		if ( !monster[v2]._mAnimCnt )
		{
			v11 = monster[v2]._mxvel;
			v12 = monster[v2]._myvel;
			monster[v2]._mVar8 = v3 + 1;
			monster[v2]._mVar6 += v11;
			v13 = monster[v2]._mVar6 >> 4;
			monster[v2]._mVar7 += v12;
			v14 = monster[v2]._mVar7 >> 4;
			monster[v2]._mxoff = v13;
			monster[v2]._myoff = v14;
		}
		v10 = 0;
	}
	if ( monster[v2]._uniqtype )
		M_ChangeLightOffset(v1);
	return v10;
}

void __fastcall M_TryM2MHit(int i, int mid, int hper, int mind, int maxd)
{
	int v5; // edi
	//int v6; // ST08_4
	int v7; // esi
	int v8; // ebx
	//int v9; // eax
	int v11; // eax
	BOOL ret; // [esp+Ch] [ebp-Ch]
	char v13[4]; // [esp+10h] [ebp-8h]
	char arglist[4]; // [esp+14h] [ebp-4h]

	v5 = mid;
	*(_DWORD *)arglist = mid;
	*(_DWORD *)v13 = i;
	if ( (unsigned int)mid >= MAXMONSTERS )
	{
		TermMsg("M_TryM2MHit: Invalid monster %d", mid);
		//i = v6;
	}
	v7 = v5;
	if ( !monster[v5].MType )
		TermMsg("M_TryM2MHit: Monster %d \"%s\" MType NULL", v5, monster[v7].mName);
	if ( (signed int)(monster[v7]._mhitpoints & 0xFFFFFFC0) > 0
		&& (monster[v7].MType->mtype != MT_ILLWEAV || _LOBYTE(monster[v7]._mgoal) != 2) )
	{
		v8 = random(4, 100);
		if ( monster[v7]._mmode == MM_STONE )
			v8 = 0;
		//_LOBYTE(v9) = CheckMonsterHit(*(int *)arglist, &ret);
		if ( !CheckMonsterHit(*(int *)arglist, &ret) && v8 < hper )
		{
			v11 = (mind + random(5, maxd - mind + 1)) << 6;
			monster[v7]._mhitpoints -= v11;
			if ( (signed int)(monster[v7]._mhitpoints & 0xFFFFFFC0) > 0 )
			{
				if ( monster[v7]._mmode == MM_STONE )
				{
					M2MStartHit(*(int *)arglist, *(int *)v13, v11);
					goto LABEL_15;
				}
				M2MStartHit(*(int *)arglist, *(int *)v13, v11);
			}
			else
			{
				if ( monster[v7]._mmode == MM_STONE )
				{
					M2MStartKill(*(int *)v13, *(int *)arglist);
				LABEL_15:
					monster[v7]._mmode = MM_STONE;
					return;
				}
				M2MStartKill(*(int *)v13, *(int *)arglist);
			}
		}
	}
}

void __fastcall M_TryH2HHit(int i, int pnum, int Hit, int MinDam, int MaxDam)
{
	int v5; // esi
	int v6; // ebx
	int v7; // esi
	int v8; // edi
	int v9; // eax
	//int v10; // ST08_4
	int v12; // ecx
	int v13; // edi
	int v14; // eax
	int v15; // eax
	int *v16; // ecx
	int v17; // eax
	int v18; // edi
	int v19; // edx
	int v20; // eax
	int v21; // eax
	int v22; // edx
	int v23; // eax
	bool v24; // zf
	bool v25; // sf
	unsigned char v26; // of
	int v27; // eax
	int v29; // edi
	int v30; // eax
	int v31; // eax
	int v32; // eax
	int v33; // edi
	int v34; // ebx
	int v35; // edx
	int v36; // [esp+Ch] [ebp-Ch]
	int arglist; // [esp+10h] [ebp-8h]
	int plr_num; // [esp+14h] [ebp-4h]
	int hper; // [esp+20h] [ebp+8h]

	v5 = i;
	plr_num = pnum;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_TryH2HHit: Invalid monster %d", i);
	v6 = v5;
	if ( !monster[v5].MType )
		TermMsg("M_TryH2HHit: Monster %d \"%s\" MType NULL", v5, monster[v6].mName);
	if ( monster[v6]._mFlags & 0x10 )
	{
		M_TryM2MHit(v5, plr_num, Hit, MinDam, MaxDam);
		return;
	}
	v7 = plr_num;
	if ( (signed int)(plr[plr_num]._pHitPoints & 0xFFFFFFC0) > 0 && !plr[v7]._pInvincible && !(plr[v7]._pSpellFlags & 1) )
	{
		v8 = abs(monster[v6]._mx - plr[v7].WorldX);
		v9 = abs(monster[v6]._my - plr[v7].WorldY);
		//v11 = v10;
		if ( v8 < 2 && v9 < 2 )
		{
			v36 = random(98, 100);
#ifdef _DEBUG
			if ( debug_mode_dollar_sign || debug_mode_key_inverted_v )
				v36 = 1000;
#endif
			v12 = 5;
			v13 = Hit
				+ 2 * (SLOBYTE(monster[v6].mLevel) - plr[v7]._pLevel)
				+ 30
				- plr[v7]._pIBonusAC
				- plr[v7]._pIAC
				- plr[v7]._pDexterity / 5;
			if ( v13 < 15 )
				v13 = 15;
			if ( currlevel == 14 )
			{
				if ( v13 >= 20 )
					goto LABEL_23;
				v13 = 20;
			}
			if ( currlevel != 15 )
			{
			LABEL_20:
				if ( currlevel == 16 && v13 < 30 )
					v13 = 30;
				goto LABEL_23;
			}
			if ( v13 < 25 )
			{
				v13 = 25;
				goto LABEL_20;
			}
		LABEL_23:
			v14 = plr[v7]._pmode;
			if ( v14 && v14 != 4 || !plr[v7]._pBlockFlag )
			{
				v15 = 100;
			}
			else
			{
				v15 = random(98, 100);
			}
			v16 = (int *)(plr[v7]._pDexterity
				+ plr[v7]._pBaseToBlk
				- 2 * SLOBYTE(monster[v6].mLevel)
				+ 2 * plr[v7]._pLevel);
			if ( (signed int)v16 < 0 )
				v16 = 0;
			if ( (signed int)v16 > 100 )
				v16 = (int *)100;
			if ( v36 < v13 )
			{
				if ( v15 >= (signed int)v16 )
				{
					if ( monster[v6].MType->mtype == MT_YZOMBIE && plr_num == myplr )
					{
						v18 = -1;
						v19 = 0;
						for ( hper = -1; v19 < nummissiles; ++v19 )
						{
							v20 = missileactive[v19];
							if ( missile[v20]._mitype == 13 )
							{
								if ( missile[v20]._misource == plr_num )
								{
									v18 = missileactive[v19];
									hper = missileactive[v19];
								}
								else
								{
									v18 = hper;
								}
							}
						}
						v16 = &plr[v7]._pMaxHP;
						v21 = plr[v7]._pMaxHP;
						if ( v21 > 64 )
						{
							v22 = plr[v7]._pMaxHPBase;
							if ( v22 > 64 )
							{
								v23 = v21 - 64;
								v26 = __OFSUB__(plr[v7]._pHitPoints, v23);
								v24 = plr[v7]._pHitPoints == v23;
								v25 = plr[v7]._pHitPoints - v23 < 0;
								*v16 = v23;
								if ( !((unsigned char)(v25 ^ v26) | v24) )
								{
									plr[v7]._pHitPoints = v23;
									if ( v18 >= 0 )
										missile[v18]._miVar1 = v23;
								}
								v16 = &plr[v7]._pHPBase;
								v27 = v22 - 64;
								plr[v7]._pMaxHPBase = v22 - 64;
								if ( plr[v7]._pHPBase > v22 - 64 )
								{
									*v16 = v27;
									if ( v18 >= 0 )
										missile[v18]._miVar2 = v27;
								}
							}
						}
					}
					v29 = (plr[v7]._pIGetHit << 6) + (MinDam << 6) + random(99, (MaxDam - MinDam + 1) << 6);
					if ( v29 < 64 )
						v29 = 64;
					if ( plr_num == myplr )
					{
						plr[v7]._pHitPoints -= v29;
						plr[v7]._pHPBase -= v29;
					}
					if ( plr[v7]._pIFlags & 0x4000000 )
					{
						v30 = (random(99, 3) + 1) << 6;
						monster[v6]._mhitpoints -= v30;
						if ( (signed int)(monster[v6]._mhitpoints & 0xFFFFFFC0) > 0 )
							M_StartHit(arglist, plr_num, v30);
						else
							M_StartKill(arglist, plr_num);
					}
					if ( !(monster[v6]._mFlags & 0x1000) && monster[v6].MType->mtype == MT_SKING && gbMaxPlayers != 1 )
						monster[v6]._mhitpoints += v29;
					v31 = plr[v7]._pMaxHP;
					if ( plr[v7]._pHitPoints > v31 )
					{
						plr[v7]._pHitPoints = v31;
						plr[v7]._pHPBase = plr[v7]._pMaxHPBase;
					}
					if ( (signed int)(plr[v7]._pHitPoints & 0xFFFFFFC0) > 0 )
					{
						StartPlrHit(plr_num, v29, 0);
						if ( SLOBYTE(monster[v6]._mFlags) < 0 )
						{
							if ( plr[v7]._pmode != PM_GOTHIT )
								StartPlrHit(plr_num, 0, 1u);
							v32 = monster[v6]._mdir;
							v33 = plr[v7].WorldX + offset_x[v32];
							v34 = plr[v7].WorldY + offset_y[v32];
							if ( PosOkPlayer(plr_num, v33, v34) )
							{
								v35 = plr[v7]._pdir;
								plr[v7].WorldX = v33;
								plr[v7].WorldY = v34;
								FixPlayerLocation(plr_num, v35);
								FixPlrWalkTags(plr_num);
								dPlayer[v33][v34] = plr_num + 1;
								SetPlayerOld(plr_num);
							}
						}
					}
					else
					{
						SyncPlrKill(plr_num, 0);
					}
				}
				else
				{
					v17 = GetDirection(plr[v7].WorldX, plr[v7].WorldY, monster[v6]._mx, monster[v6]._my);
					StartPlrBlock(plr_num, v17);
				}
			}
			return;
		}
	}
}
// 679660: using guessed type char gbMaxPlayers;

int __fastcall M_DoAttack(int i)
{
	int v1; // edi
	int v2; // esi
	CMonster **v3; // ebx
	unsigned char v4; // al
	unsigned char v5; // al

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoAttack: Invalid monster %d", i);
	v2 = v1;
	v3 = &monster[v1].MType;
	if ( !*v3 )
	{
		TermMsg("M_DoAttack: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
		if ( !*v3 )
			TermMsg("M_DoAttack: Monster %d \"%s\" MData NULL", v1, monster[v2].mName);
	}
	if ( monster[v2]._mAnimFrame == monster[v2].MData->mAFNum )
	{
		M_TryH2HHit(
			v1,
			monster[v2]._menemy,
			(unsigned char)monster[v2].mHit,
			(unsigned char)monster[v2].mMinDamage,
			(unsigned char)monster[v2].mMaxDamage);
		if ( monster[v2]._mAi != AI_SNAKE )
			PlayEffect(v1, 0);
	}
	v4 = monster[v2].MType->mtype;
	if ( v4 >= MT_NMAGMA && v4 <= MT_WMAGMA && monster[v2]._mAnimFrame == 9 )
	{
		M_TryH2HHit(
			v1,
			monster[v2]._menemy,
			(unsigned char)monster[v2].mHit + 10,
			(unsigned char)monster[v2].mMinDamage - 2,
			(unsigned char)monster[v2].mMaxDamage - 2);
		PlayEffect(v1, 0);
	}
	v5 = monster[v2].MType->mtype;
	if ( v5 >= MT_STORM && v5 <= MT_MAEL && monster[v2]._mAnimFrame == 13 )
	{
		M_TryH2HHit(
			v1,
			monster[v2]._menemy,
			(unsigned char)monster[v2].mHit - 20,
			(unsigned char)monster[v2].mMinDamage + 4,
			(unsigned char)monster[v2].mMaxDamage + 4);
		PlayEffect(v1, 0);
	}
	if ( monster[v2]._mAi == AI_SNAKE && monster[v2]._mAnimFrame == 1 )
		PlayEffect(v1, 0);
	if ( monster[v2]._mAnimFrame != monster[v2]._mAnimLen )
		return 0;
	M_StartStand(v1, monster[v2]._mdir);
	return 1;
}

int __fastcall M_DoRAttack(int i)
{
	int v1; // ebx
	int v2; // esi
	CMonster **v3; // edi
	int v4; // eax
	int v5; // eax
	int v6; // edi

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoRAttack: Invalid monster %d", i);
	v2 = v1;
	v3 = &monster[v1].MType;
	if ( !*v3 )
	{
		TermMsg("M_DoRAttack: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
		if ( !*v3 )
			TermMsg("M_DoRAttack: Monster %d \"%s\" MData NULL", v1, monster[v2].mName);
	}
	if ( monster[v2]._mAnimFrame == monster[v2].MData->mAFNum )
	{
		v4 = monster[v2]._mVar1;
		if ( v4 != -1 )
		{
			v5 = 2 * (v4 == 52) + 1;
			if ( v5 > 0 )
			{
				v6 = v5;
				do
				{
					AddMissile(
						monster[v2]._mx,
						monster[v2]._my,
						(unsigned char)monster[v2]._menemyx,
						(unsigned char)monster[v2]._menemyy,
						monster[v2]._mdir,
						monster[v2]._mVar1,
						1,
						v1,
						monster[v2]._mVar2,
						0);
					--v6;
				}
				while ( v6 );
			}
		}
		PlayEffect(v1, 0);
	}
	if ( monster[v2]._mAnimFrame != monster[v2]._mAnimLen )
		return 0;
	M_StartStand(v1, monster[v2]._mdir);
	return 1;
}

int __fastcall M_DoRSpAttack(int i)
{
	int v1; // ebx
	int v2; // esi
	CMonster **v3; // edi
	bool v4; // zf
	int v5; // ecx

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoRSpAttack: Invalid monster %d", i);
	v2 = v1;
	v3 = &monster[v1].MType;
	v4 = *v3 == 0;
	if ( !*v3 )
	{
		TermMsg("M_DoRSpAttack: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
		v4 = *v3 == 0;
	}
	if ( v4 )
		TermMsg("M_DoRSpAttack: Monster %d \"%s\" MData NULL", v1, monster[v2].mName);
	if ( monster[v2]._mAnimFrame == monster[v2].MData->mAFNum2 && !monster[v2]._mAnimCnt )
	{
		AddMissile(
			monster[v2]._mx,
			monster[v2]._my,
			(unsigned char)monster[v2]._menemyx,
			(unsigned char)monster[v2]._menemyy,
			monster[v2]._mdir,
			monster[v2]._mVar1,
			1,
			v1,
			monster[v2]._mVar3,
			0);
		PlayEffect(v1, 3);
	}
	if ( monster[v2]._mAi == AI_MEGA && monster[v2]._mAnimFrame == 3 )
	{
		v5 = monster[v2]._mVar2;
		monster[v2]._mVar2 = v5 + 1;
		if ( v5 )
		{
			if ( v5 == 14 )
				monster[v2]._mFlags &= 0xFFFFFFFB;
		}
		else
		{
			monster[v2]._mFlags |= 4u;
		}
	}
	if ( monster[v2]._mAnimFrame != monster[v2]._mAnimLen )
		return 0;
	M_StartStand(v1, monster[v2]._mdir);
	return 1;
}

int __fastcall M_DoSAttack(int i)
{
	int v1; // ebx
	int v2; // esi
	CMonster **v3; // edi
	bool v4; // zf

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoSAttack: Invalid monster %d", i);
	v2 = v1;
	v3 = &monster[v1].MType;
	v4 = *v3 == 0;
	if ( !*v3 )
	{
		TermMsg("M_DoSAttack: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
		v4 = *v3 == 0;
	}
	if ( v4 )
		TermMsg("M_DoSAttack: Monster %d \"%s\" MData NULL", v1, monster[v2].mName);
	if ( monster[v2]._mAnimFrame == monster[v2].MData->mAFNum2 )
		M_TryH2HHit(
			v1,
			monster[v2]._menemy,
			(unsigned char)monster[v2].mHit2,
			(unsigned char)monster[v2].mMinDamage2,
			(unsigned char)monster[v2].mMaxDamage2);
	if ( monster[v2]._mAnimFrame != monster[v2]._mAnimLen )
		return 0;
	M_StartStand(v1, monster[v2]._mdir);
	return 1;
}

int __fastcall M_DoFadein(int i)
{
	int v1; // edi
	int v2; // esi

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoFadein: Invalid monster %d", i);
	v2 = v1;
	if ( (!(monster[v1]._mFlags & 2) || monster[v2]._mAnimFrame != 1)
		&& (monster[v1]._mFlags & 2 || monster[v2]._mAnimFrame != monster[v2]._mAnimLen) )
	{
		return 0;
	}
	M_StartStand(v1, monster[v2]._mdir);
	monster[v2]._mFlags &= 0xFFFFFFFD;
	return 1;
}

int __fastcall M_DoFadeout(int i)
{
	int v1; // esi
	int v2; // eax
	int v3; // ecx
	signed int v4; // edx
	int v5; // ecx
	int v6; // edx

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoFadeout: Invalid monster %d", i);
	v2 = v1;
	v3 = monster[v1]._mFlags;
	if ( (!(monster[v1]._mFlags & 2) || monster[v2]._mAnimFrame != 1)
		&& (monster[v1]._mFlags & 2 || monster[v2]._mAnimFrame != monster[v2]._mAnimLen) )
	{
		return 0;
	}
	v4 = monster[v2].MType->mtype;
	if ( v4 < MT_INCIN || v4 > MT_HELLBURN )
		v5 = v3 & 0xFFFFFFFD | 1;
	else
		v5 = v3 & 0xFFFFFFFD;
	v6 = monster[v2]._mdir;
	monster[v2]._mFlags = v5;
	M_StartStand(v1, v6);
	return 1;
}

int __fastcall M_DoHeal(int i)
{
	int v1; // esi
	int v2; // eax
	int v3; // esi
	int *v4; // edx
	int v5; // ecx
	int v6; // edi
	int v7; // edi
	int v8; // esi

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoHeal: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v1]._mFlags & 8 )
	{
		monster[v2]._mFlags &= 0xFFFFFFFB;
		monster[v2]._mmode = MM_SATTACK;
	}
	else if ( monster[v2]._mAnimFrame == 1 )
	{
		v3 = monster[v2]._mVar1;
		v4 = &monster[v2]._mhitpoints;
		v5 = monster[v2]._mFlags & 0xFFFFFFFD | 4;
		v6 = monster[v2]._mhitpoints;
		monster[v2]._mFlags = v5;
		v7 = v3 + v6;
		v8 = monster[v2]._mmaxhp;
		if ( v7 >= v8 )
		{
			*v4 = v8;
			monster[v2]._mFlags = v5 & 0xFFFFFFFB;
			monster[v2]._mmode = MM_SATTACK;
		}
		else
		{
			*v4 = v7;
		}
	}
	return 0;
}

int __fastcall M_DoTalk(int i)
{
	int v1; // edi
	int v2; // esi
	//int v3; // eax
	int v4; // eax
	int v5; // edx
	int v6; // ecx
	char v7; // bl
	int v8; // eax
	char *v9; // eax

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoTalk: Invalid monster %d", i);
	v2 = v1;
	M_StartStand(v1, monster[v1]._mdir);
	_LOBYTE(monster[v1]._mgoal) = 7;
	//_LOBYTE(v3) = effect_is_playing(alltext[monster[v1].mtalkmsg].sfxnr);
	if ( !effect_is_playing(alltext[monster[v1].mtalkmsg].sfxnr) )
	{
		InitQTextMsg(monster[v2].mtalkmsg);
		if ( monster[v2].mName == UniqMonst[0].mName )
		{
			v4 = monster[v2].mtalkmsg;
			if ( v4 == QUEST_GARBUD1 )
				quests[2]._qactive = 2;
			quests[2]._qlog = 1;
			if ( v4 == QUEST_GARBUD2 && !(monster[v2]._mFlags & 0x40) )
			{
				SpawnItem(v1, monster[v2]._mx + 1, monster[v2]._my + 1, 1u);
				monster[v2]._mFlags |= 0x40u;
			}
		}
		if ( monster[v2].mName == UniqMonst[2].mName
			&& monster[v2].mtalkmsg == QUEST_ZHAR1
			&& !(monster[v2]._mFlags & 0x40) )
		{
			v5 = monster[v2]._my + 1;
			v6 = monster[v2]._mx + 1;
			quests[3]._qactive = 2;
			quests[3]._qlog = 1;
			CreateTypeItem(v6, v5, 0, 0, 24, 1, 0);
			monster[v2]._mFlags |= 0x40u;
		}
		if ( monster[v2].mName == UniqMonst[3].mName )
		{
			if ( monster[v2].mtalkmsg == QUEST_BANNER10 && !(monster[v2]._mFlags & 0x40) )
			{
				ObjChangeMap(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 2, (setpc_h >> 1) + setpc_y - 2);
				v7 = TransVal;
				TransVal = 9;
				DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 4, setpc_y + (setpc_h >> 1));
				TransVal = v7;
				quests[7]._qvar1 = 2;
				if ( quests[7]._qactive == 1 )
					quests[7]._qactive = 2;
				monster[v2]._mFlags |= 0x40u;
			}
			if ( quests[7]._qvar1 < 2u )
			{
				sprintf(tempstr, "SS Talk = %i, Flags = %i", monster[v2].mtalkmsg, monster[v2]._mFlags);
				TermMsg(tempstr);
			}
		}
		if ( monster[v2].mName == UniqMonst[7].mName )
		{
			v8 = monster[v2].mtalkmsg;
			if ( v8 == QUEST_VEIL9 )
			{
				quests[4]._qactive = 2;
				quests[4]._qlog = 1;
			}
			if ( v8 == QUEST_VEIL11 && !(monster[v2]._mFlags & 0x40) )
			{
				SpawnUnique(UITEM_STEELVEIL, monster[v2]._mx + 1, monster[v2]._my + 1);
				monster[v2]._mFlags |= 0x40u;
			}
		}
		v9 = monster[v2].mName;
		if ( v9 == UniqMonst[8].mName )
			quests[11]._qvar1 = 2;
		if ( v9 == UniqMonst[4].mName && gbMaxPlayers != 1 )
		{
			monster[v2]._msquelch = -1;
			monster[v2].mtalkmsg = 0;
			quests[15]._qvar1 = 6;
			_LOBYTE(monster[v2]._mgoal) = 1;
		}
	}
	return 0;
}
// 4351F5: could not find valid save-restore pair for ebp
// 5A5590: using guessed type char TransVal;
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall M_Teleport(int i)
{
	int v1; // ebx
	//int v2; // ST04_4
	MonsterStruct *v3; // esi
	int v4; // eax
	int v6; // edi
	int v7; // ebx
	int v8; // eax
	int v9; // [esp+Ch] [ebp-24h]
	int v10; // [esp+10h] [ebp-20h]
	int v11; // [esp+14h] [ebp-1Ch]
	int v12; // [esp+18h] [ebp-18h]
	int v13; // [esp+1Ch] [ebp-14h]
	int a1; // [esp+20h] [ebp-10h]
	signed int v15; // [esp+24h] [ebp-Ch]
	signed int v16; // [esp+28h] [ebp-8h]
	signed int v17; // [esp+2Ch] [ebp-4h]

	v1 = i;
	a1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
	{
		TermMsg("M_Teleport: Invalid monster %d", i);
		//i = v2;
	}
	v15 = 0;
	v3 = &monster[v1];
	if ( v3->_mmode != MM_STONE )
	{
		v10 = (unsigned char)v3->_menemyx;
		v12 = (unsigned char)v3->_menemyy;
		v4 = random(100, 2);
		v11 = 2 * v4 - 1;
		v17 = -1;
		v6 = 0; /* v9 */
		v13 = 2 * random(100, 2) - 1;
		while ( !v15 )
		{
			v16 = -1;
			v7 = v12 - v13;
			do
			{
				if ( v15 )
					break;
				if ( v17 || v16 )
				{
					v9 = v7;
					v6 = v10 + v11 * v17;
					if ( v7 >= 0 && v7 < 112 && v6 >= 0 && v6 < 112 && v6 != v3->_mx && v7 != v3->_my )
					{
						if ( PosOkMonst(a1, v10 + v11 * v17, v7) )
							v15 = 1;
					}
				}
				++v16;
				v7 += v13;
			}
			while ( v16 < 1 );
			if ( ++v17 > 1 )
			{
				if ( !v15 )
					return;
				v1 = a1;
				break;
			}
			v1 = a1;
		}
		M_ClearSquares(v1);
		v8 = v3->_my + 112 * v3->_mx;
		v3->_moldx = v6;
		dMonster[0][v8] = 0;
		v3->_moldy = v9;
		dMonster[0][v9 + 112 * v6] = v1 + 1;
		v3->_mdir = M_GetDir(v1);
		M_CheckEFlag(v1);
	}
}

int __fastcall M_DoGotHit(int i)
{
	int v1; // edi
	int v2; // esi

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoGotHit: Invalid monster %d", i);
	v2 = v1;
	if ( !monster[v1].MType )
		TermMsg("M_DoGotHit: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
	if ( monster[v2]._mAnimFrame != monster[v2]._mAnimLen )
		return 0;
	M_StartStand(v1, monster[v2]._mdir);
	return 1;
}

void __fastcall M_UpdateLeader(int i)
{
	int v1; // edi
	int v2; // esi
	int j; // edx
	int v4; // eax
	unsigned char *v5; // eax

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_UpdateLeader: Invalid monster %d", i);
	v2 = nummonsters;
	for ( j = 0; j < v2; ++j )
	{
		v4 = monstactive[j];
		if ( monster[v4].leaderflag == 1 && (unsigned char)monster[v4].leader == v1 )
			monster[v4].leaderflag = 0;
	}
	if ( monster[v1].leaderflag == 1 )
	{
		v5 = &monster[(unsigned char)monster[v1].leader].unpackfilesize;
		--*v5;
	}
}

void __cdecl DoEnding()
{
	if ( gbMaxPlayers > 1 ) {
		SNetLeaveGame(0x40000004);
	}

	music_stop();

	if ( gbMaxPlayers > 1 ) {
		Sleep(1000);
	}

	if ( plr[myplr]._pClass == PC_WARRIOR ) {
		play_movie("gendata\\DiabVic2.smk", 0);
	} else if ( plr[myplr]._pClass == PC_SORCERER ) {
		play_movie("gendata\\DiabVic1.smk", 0);
	} else {
		play_movie("gendata\\DiabVic3.smk", 0);
	}
	play_movie("gendata\\Diabend.smk", 0);

	BOOL bMusicOn = gbMusicOn;
	gbMusicOn = 1;

	int musicVolume = sound_get_or_set_music_volume(1);
	sound_get_or_set_music_volume(0);

	music_start(2);
	loop_movie = 1;
	play_movie("gendata\\loopdend.smk", 1);
	loop_movie = 0;
	music_stop();

	sound_get_or_set_music_volume(musicVolume);
	gbMusicOn = bMusicOn;
}
// 4A22D4: using guessed type char gbMusicOn;
// 659AFC: using guessed type int loop_movie;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl PrepDoEnding()
{
	int *v0; // eax
	int v1; // ecx
	int *v2; // eax
	bool v3; // cf
	bool v4; // zf

	gbSoundOn = sgbSaveSoundOn;
	gbRunGame = 0;
	deathflag = 0;
	v0 = &plr[myplr].pDiabloKillLevel;
	v1 = gnDifficulty + 1;
	cineflag = 1;
	if ( *v0 > (unsigned int)(gnDifficulty + 1) )
		v1 = *v0;
	*v0 = v1;
	v2 = &plr[0]._pHitPoints;
	do
	{
		v3 = (unsigned char)gbMaxPlayers < 1u;
		v4 = gbMaxPlayers == 1;
		*(v2 - 102) = 11;
		*((_BYTE *)v2 - 91) = 1;
		if ( !v3 && !v4 )
		{
			if ( !(*v2 & 0xFFFFFFC0) )
				*v2 = 64;
			if ( !(v2[5] & 0xFFFFFFC0) )
				v2[5] = 64;
		}
		v2 += 5430;
	}
	while ( (signed int)v2 < (signed int)&plr[4]._pHitPoints );
}
// 4A22D5: using guessed type char gbSoundOn;
// 525650: using guessed type int gbRunGame;
// 525718: using guessed type char cineflag;
// 64D32C: using guessed type int sgbSaveSoundOn;
// 679660: using guessed type char gbMaxPlayers;

int __fastcall M_DoDeath(int i)
{
	int v1; // edi
	int v2; // esi
	CMonster *v3; // ecx
	int v4; // eax
	int v5; // ecx
	signed int v6; // ecx
	int v7; // esi
	int v8; // esi
	signed int v9; // ecx
	char v10; // al
	int v11; // eax

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoDeath: Invalid monster %d", i);
	v2 = v1;
	if ( !monster[v1].MType )
		TermMsg("M_DoDeath: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
	v3 = monster[v2].MType;
	v4 = ++monster[v2]._mVar1;
	if ( v3->mtype == MT_DIABLO )
	{
		v5 = monster[v2]._mx - ViewX;
		if ( v5 >= 0 )
			v6 = v5 > 0;
		else
			v6 = -1;
		v7 = monster[v2]._my;
		ViewX += v6;
		v8 = v7 - ViewY;
		if ( v8 >= 0 )
		{
			v9 = v8 < 0;
			_LOBYTE(v9) = v8 > 0;
		}
		else
		{
			v9 = -1;
		}
		ViewY += v9;
		if ( v4 == 140 )
			PrepDoEnding();
	}
	else if ( monster[v2]._mAnimFrame == monster[v2]._mAnimLen )
	{
		if ( monster[v2]._uniqtype )
			v10 = monster[v2]._udeadval;
		else
			v10 = v3->mdeadval;
		AddDead(monster[v2]._mx, monster[v2]._my, v10, (direction)monster[v2]._mdir);
		v11 = monster[v2]._my + 112 * monster[v2]._mx;
		monster[v2]._mDelFlag = 1;
		dMonster[0][v11] = 0;
		M_UpdateLeader(v1);
	}
	return 0;
}

int __fastcall M_DoSpStand(int i)
{
	int v1; // ebx
	int v2; // esi

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoSpStand: Invalid monster %d", i);
	v2 = v1;
	if ( !monster[v1].MType )
		TermMsg("M_DoSpStand: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
	if ( monster[v2]._mAnimFrame == monster[v2].MData->mAFNum2 )
		PlayEffect(v1, 3);
	if ( monster[v2]._mAnimFrame != monster[v2]._mAnimLen )
		return 0;
	M_StartStand(v1, monster[v2]._mdir);
	return 1;
}

int __fastcall M_DoDelay(int i)
{
	int v1; // ebp
	int v2; // esi
	int v3; // eax
	bool v4; // zf
	int v5; // ecx
	int v6; // ecx
	int v7; // ebx

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoDelay: Invalid monster %d", i);
	v2 = v1;
	if ( !monster[v1].MType )
		TermMsg("M_DoDelay: Monster %d \"%s\" MType NULL", v1, monster[v2].mName);
	v3 = M_GetDir(v1);
	v4 = monster[v2]._mAi == AI_LAZURUS;
	monster[v2]._mAnimData = monster[v2].MType->Anims[0].Frames[v3];
	if ( v4 )
	{
		v5 = monster[v2]._mVar2;
		if ( v5 > 8 || v5 < 0 )
			monster[v2]._mVar2 = 8;
	}
	v6 = monster[v2]._mVar2;
	monster[v2]._mVar2 = v6 - 1;
	if ( v6 )
		return 0;
	v7 = monster[v2]._mAnimFrame;
	M_StartStand(v1, monster[v2]._mdir);
	monster[v2]._mAnimFrame = v7;
	return 1;
}

int __fastcall M_DoStone(int i)
{
	int v1; // esi
	int v2; // eax
	int v3; // ecx

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_DoStone: Invalid monster %d", i);
	v2 = v1;
	if ( !monster[v1]._mhitpoints )
	{
		v3 = monster[v2]._mx;
		monster[v2]._mDelFlag = 1;
		dMonster[0][monster[v2]._my + 112 * v3] = 0;
	}
	return 0;
}

void __fastcall M_WalkDir(int i, int md)
{
	int v2; // esi
	int v3; // edi
	int v4; // eax
	int v5; // eax
	int v6; // edx
	int v7; // ecx
	int v8; // eax
	int v9; // edx
	int v10; // eax
	int v11; // [esp-14h] [ebp-1Ch]
	int v12; // [esp-Ch] [ebp-14h]
	int v13; // [esp-Ch] [ebp-14h]
	int v14; // [esp-8h] [ebp-10h]
	int v15; // [esp-8h] [ebp-10h]
	int v16; // [esp-4h] [ebp-Ch]
	int v17; // [esp-4h] [ebp-Ch]

	v2 = i;
	v3 = md;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_WalkDir: Invalid monster %d", i);
	v4 = monster[v2].MType->Anims[1].Rate - 1;
	switch ( v3 )
	{
	case DIR_S:
		M_StartWalk2(v2, 0, MWVel[v4][1], 0, -32, 1, 1, 0);
		return;
	case DIR_SW:
		v17 = 1;
		v8 = v4;
		v15 = 1;
		v13 = 0;
		v11 = 32;
		v9 = -MWVel[v8][1];
		goto LABEL_10;
	case DIR_W:
		M_StartWalk3(v2, -MWVel[v4][2], 0, 32, -16, -1, 1, 0, 1, 2);
		return;
	case DIR_NW:
		v16 = 3;
		v10 = v4;
		v14 = 0;
		v12 = -1;
		v7 = -MWVel[v10][0];
		v6 = -MWVel[v10][1];
		goto LABEL_15;
	case DIR_N:
		M_StartWalk(v2, 0, -MWVel[v4][1], -1, -1, 4);
		break;
	case DIR_NE:
		v16 = 5;
		v5 = v4;
		v14 = -1;
		v12 = 0;
		v6 = MWVel[v5][1];
		v7 = -MWVel[v5][0];
	LABEL_15:
		M_StartWalk(v2, v6, v7, v12, v14, v16);
		break;
	case DIR_E:
		M_StartWalk3(v2, MWVel[v4][2], 0, -32, -16, 1, -1, 1, 0, 6);
		break;
	case DIR_SE:
		v17 = 7;
		v8 = v4;
		v15 = 0;
		v13 = 1;
		v9 = MWVel[v8][1];
		v11 = -32;
	LABEL_10:
		M_StartWalk2(v2, v9, MWVel[v8][0], v11, -16, v13, v15, v17);
		break;
	default:
		return;
	}
}

void __fastcall GroupUnity(int i)
{
	int v1; // ebx
	int v2; // esi
	int v3; // ebp
	int v4; // edi
	bool v5; // eax
	int v6; // eax
	int v7; // ecx
	unsigned char v8; // al
	int v9; // ebp
	int j; // edi
	int v11; // eax
	int v12; // ecx
	//int v13; // [esp+10h] [ebp-4h]

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("GroupUnity: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v1].leaderflag )
	{
		v3 = (unsigned char)monster[v2].leader;
		v4 = v3;
		v5 = LineClearF(
			CheckNoSolid,
			monster[v2]._mx,
			monster[v2]._my,
			monster[v4]._mfutx,
			monster[v4]._mfuty);
		if ( v5 )
		{
			if ( monster[v2].leaderflag == 2
				&& abs(monster[v2]._mx - monster[v4]._mfutx) < 4
				&& abs(monster[v2]._my - monster[v4]._mfuty) < 4 )
			{
				++monster[v4].unpackfilesize;
				monster[v2].leaderflag = 1;
			}
		}
		else
		{
			if ( monster[v2].leaderflag != 1 )
				goto LABEL_18;
			--monster[v4].unpackfilesize;
			monster[v2].leaderflag = 2;
		}
	}
	else
	{
		v3 = 0; /* v13 */
	}
	if ( monster[v2].leaderflag == 1 )
	{
		v6 = v3;
		if ( monster[v2]._msquelch > monster[v3]._msquelch )
		{
			monster[v6]._lastx = monster[v2]._mx;
			monster[v6]._lasty = monster[v2]._my;
			monster[v6]._msquelch = monster[v2]._msquelch - 1;
		}
		if ( monster[v6]._mAi == AI_GARG )
		{
			v7 = monster[v6]._mFlags;
			if ( v7 & 4 )
			{
				monster[v6]._mmode = MM_SATTACK;
				monster[v6]._mFlags = v7 & 0xFFFFFFFB;
			}
		}
		return;
	}
LABEL_18:
	v8 = monster[v2]._uniqtype;
	if ( v8 )
	{
		if ( UniqMonst[v8 - 1].mUnqAttr & 2 )
		{
			v9 = nummonsters;
			for ( j = 0; j < v9; ++j )
			{
				v11 = monstactive[j];
				if ( monster[v11].leaderflag == 1 && (unsigned char)monster[v11].leader == v1 )
				{
					if ( monster[v2]._msquelch > monster[v11]._msquelch )
					{
						monster[v11]._lastx = monster[v2]._mx;
						monster[v11]._lasty = monster[v2]._my;
						monster[v11]._msquelch = monster[v2]._msquelch - 1;
					}
					if ( monster[v11]._mAi == AI_GARG )
					{
						v12 = monster[v11]._mFlags;
						if ( v12 & 4 )
						{
							monster[v11]._mmode = MM_SATTACK;
							monster[v11]._mFlags = v12 & 0xFFFFFFFB;
						}
					}
				}
			}
		}
	}
}

bool __fastcall M_CallWalk(int i, int md)
{
	int v2; // esi
	int v3; // edi
	int v4; // ebp
	//int v5; // eax
	bool v7; // ebx
	int v9; // ebx
	//int v10; // eax
	int v11; // ebx
	//int v12; // eax
	//int v13; // eax
	signed int v14; // ebx
	//int v15; // eax
	//int v16; // eax
	//int v17; // eax
	unsigned char v18; // bl

	v2 = md;
	v3 = i;
	v4 = md;
	//_LOBYTE(v5) = DirOK(i, md);
	v7 = DirOK(i, md);
	if ( random(101, 2) )
	{
		if ( v7 )
			goto LABEL_10;
		v9 = v2;
		v2 = left[v2];
		//_LOBYTE(v10) = DirOK(v3, v2);
		if ( DirOK(v3, v2) )
			goto LABEL_10;
		v2 = right[v9];
	}
	else
	{
		if ( v7 )
			goto LABEL_10;
		v11 = v2;
		v2 = right[v2];
		//_LOBYTE(v12) = DirOK(v3, v2);
		if ( DirOK(v3, v2) )
			goto LABEL_10;
		v2 = left[v11];
	}
	//_LOBYTE(v13) = DirOK(v3, v2);
	if ( !DirOK(v3, v2) )
	{
		v14 = 0;
		goto LABEL_11;
	}
LABEL_10:
	v14 = 1;
LABEL_11:
	if ( random(102, 2) )
	{
		if ( v14 )
			goto LABEL_20;
		v2 = right[right[v4]];
		//_LOBYTE(v15) = DirOK(v3, v2);
		if ( DirOK(v3, v2) )
			goto LABEL_20;
		v2 = left[left[v4]];
	}
	else
	{
		if ( v14 )
			goto LABEL_20;
		v2 = left[left[v4]];
		//_LOBYTE(v16) = DirOK(v3, v2);
		if ( DirOK(v3, v2) )
			goto LABEL_20;
		v2 = right[right[v4]];
	}
	//_LOBYTE(v17) = DirOK(v3, v2);
	if ( DirOK(v3, v2) )
	{
	LABEL_20:
		v18 = 1;
		M_WalkDir(v3, v2);
		return v18;
	}
	return 0;
}

bool __fastcall M_PathWalk(int i)
{
	int v1; // esi
	BOOL(__fastcall *Check)(int, int, int); // ecx
	char path[25]; // [esp+4h] [ebp-1Ch]

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("M_PathWalk: Invalid monster %d", i);
	Check = PosOkMonst3;
	if ( !(monster[v1]._mFlags & 0x200) )
		Check = PosOkMonst;
	if ( !FindPath(
		Check,
		v1,
		monster[v1]._mx,
		monster[v1]._my,
		(unsigned char)monster[v1]._menemyx,
		(unsigned char)monster[v1]._menemyy,
		path) )
		return 0;
	M_CallWalk(v1, (char)plr2monst[path[0]]); /* plr2monst is local */
	return 1;
}

bool __fastcall M_CallWalk2(int i, int md)
{
	int v2; // esi
	int v3; // ebx
	//int v4; // eax
	bool v6; // edi
	int v7; // edi
	//int v8; // eax
	int v9; // edi
	//int v10; // eax
	//int v11; // eax
	bool v12; // di

	v2 = md;
	v3 = i;
	//_LOBYTE(v4) = DirOK(i, md);
	v6 = DirOK(i, md);
	if ( random(101, 2) )
	{
		if ( v6 )
			goto LABEL_10;
		v7 = v2;
		v2 = left[v2];
		//_LOBYTE(v8) = DirOK(v3, v2);
		if ( DirOK(v3, v2) )
			goto LABEL_10;
		v2 = right[v7];
	}
	else
	{
		if ( v6 )
			goto LABEL_10;
		v9 = v2;
		v2 = right[v2];
		//_LOBYTE(v10) = DirOK(v3, v2);
		if ( DirOK(v3, v2) )
			goto LABEL_10;
		v2 = left[v9];
	}
	//_LOBYTE(v11) = DirOK(v3, v2);
	if ( DirOK(v3, v2) )
	{
	LABEL_10:
		v12 = 1;
		M_WalkDir(v3, v2);
		return v12;
	}
	return 0;
}

bool __fastcall M_DumbWalk(int i, int md)
{
	int v2; // esi
	int v3; // edi
	//int v4; // eax
	bool v5; // bl

	v2 = md;
	v3 = i;
	//_LOBYTE(v4) = DirOK(i, md);
	v5 = DirOK(i, md);
	if ( v5 )
		M_WalkDir(v3, v2);
	return v5;
}

bool __fastcall M_RoundWalk(int i, int md, int *dir)
{
	int *v3; // ebp
	int v4; // ebx
	int v5; // esi
	//int v6; // eax
	bool v7; // di
	int v8; // edi
	//int v9; // eax
	//int v10; // eax
	int *v11; // ebp
	//int v12; // eax
	//int v13; // eax

	v3 = dir;
	v4 = i;
	if ( *dir )
		v5 = left[left[md]];
	else
		v5 = right[right[md]];
	//_LOBYTE(v6) = DirOK(i, v5);
	v7 = DirOK(i, v5);
	if ( v7 )
		goto LABEL_12;
	v8 = v5;
	if ( !*dir )
	{
		v11 = &left[v8];
		v5 = left[v8];
		//_LOBYTE(v12) = DirOK(v4, left[v8]);
		if ( DirOK(v4, left[v8]) )
			goto LABEL_11;
		v5 = left[*v11];
		//_LOBYTE(v13) = DirOK(v4, left[*v11]);
		if ( DirOK(v4, left[*v11]) )
			goto LABEL_11;
		v3 = dir;
	LABEL_14:
		*v3 = *v3 == 0;
		return M_CallWalk(v4, opposite[v8]);
	}
	v5 = right[v8];
	//_LOBYTE(v9) = DirOK(v4, right[v8]);
	if ( !DirOK(v4, right[v8]) )
	{
		v5 = right[right[v8]];
		//_LOBYTE(v10) = DirOK(v4, v5);
		if ( !DirOK(v4, v5) )
			goto LABEL_14;
	}
LABEL_11:
	v7 = 1;
LABEL_12:
	M_WalkDir(v4, v5);
	return v7;
}

void __fastcall MAI_Zombie(int i)
{
	int v1; // esi
	//int v2; // ST04_4
	MonsterStruct *v3; // esi
	int v4; // edi
	int v5; // ebx
	int v6; // edi
	int v7; // eax
	int v8; // ecx
	int v9; // eax
	int v10; // eax
	int v11; // eax
	int v12; // ecx
	int md; // [esp+Ch] [ebp-Ch]
	int v14; // [esp+10h] [ebp-8h]
	int arglist; // [esp+14h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
	{
		TermMsg("MAI_Zombie: Invalid monster %d", i);
		//i = v2;
	}
	v3 = &monster[v1];
	if ( v3->_mmode == MM_STAND )
	{
		v4 = v3->_my;
		if ( dFlags[v3->_mx][v4] & 2 )
		{
			v5 = v3->_mx - (unsigned char)v3->_menemyx;
			v6 = v4 - (unsigned char)v3->_menemyy;
			md = v3->_mdir;
			v14 = random(103, 100);
			if ( abs(v5) >= 2 || abs(v6) >= 2 )
			{
				if ( v14 < 2 * (unsigned char)v3->_mint + 10 )
				{
					v7 = abs(v5);
					v8 = 2 * (unsigned char)v3->_mint + 4;
					if ( v7 >= v8 || (v9 = abs(v6), v8 = 2 * (unsigned char)v3->_mint + 4, v9 >= v8) )
					{
						v11 = random(104, 100);
						v12 = 2 * (unsigned char)v3->_mint + 20;
						if ( v11 < v12 )
						{
							md = random(104, 8);
						}
						M_DumbWalk(arglist, md);
					}
					else
					{
						v10 = M_GetDir(arglist);
						M_CallWalk(arglist, v10);
					}
				}
			}
			else if ( v14 < 2 * (unsigned char)v3->_mint + 10 )
			{
				M_StartAttack(arglist);
			}
			if ( v3->_mmode == MM_STAND )
				v3->_mAnimData = v3->MType->Anims[0].Frames[v3->_mdir];
		}
	}
}

void __fastcall MAI_SkelSd(int i)
{
	int v1; // esi
	MonsterStruct *v2; // esi
	int v3; // ecx
	int v4; // edx
	int v5; // edi
	int v6; // ebp
	int v7; // ebx
	int v8; // eax
	//int v9; // ST04_4
	int v11; // eax
	//int v12; // ST04_4
	int v13; // eax
	int v14; // ecx
	int v15; // edx
	int v16; // eax
	int v17; // ecx
	int arglist; // [esp+8h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_SkelSd: Invalid monster %d", i);
	v2 = &monster[v1];
	if ( v2->_mmode == MM_STAND && v2->_msquelch )
	{
		v3 = v2->_mx;
		v4 = v2->_my;
		v5 = v3 - (unsigned char)v2->_menemyx;
		v6 = v4 - (unsigned char)v2->_menemyy;
		v7 = GetDirection(v3, v4, v2->_lastx, v2->_lasty);
		v2->_mdir = v7;
		v8 = abs(v5);
		//v10 = v9;
		if ( v8 >= 2 || (v11 = abs(v6), v11 >= 2) ) /* v10 = v12,  */
		{
			if ( v2->_mVar1 != 13 )
			{
				v16 = random(106, 100);
				v17 = 4 * (unsigned char)v2->_mint;
				if ( v16 < 35 - v17 )
				{
					v15 = 15 - 2 * (unsigned char)v2->_mint + random(106, 10);
					goto LABEL_10;
				}
			}
			M_CallWalk(arglist, v7);
		}
		else
		{
			if ( v2->_mVar1 != 13 )
			{
				v13 = random(105, 100);
				v14 = 2 * (unsigned char)v2->_mint + 20;
				if ( v13 >= v14 )
				{
					v15 = random(105, 10) + 2 * (5 - (unsigned char)v2->_mint);
				LABEL_10:
					M_StartDelay(arglist, v15);
					goto LABEL_16;
				}
			}
			M_StartAttack(arglist);
		}
	LABEL_16:
		if ( v2->_mmode == MM_STAND )
			v2->_mAnimData = v2->MType->Anims[0].Frames[v7];
	}
}

bool __fastcall MAI_Path(int i)
{
	int v1; // edi
	MonsterStruct *v2; // esi
	char v3; // al
	bool v4; // eax
	unsigned char v5; // al

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Path: Invalid monster %d", i);
	v2 = &monster[v1];
	if ( v2->MType->mtype != MT_GOLEM )
	{
		if ( !v2->_msquelch )
			return 0;
		if ( v2->_mmode )
			return 0;
		v3 = v2->_mgoal;
		if ( v3 != 1 && v3 != 4 && v3 != 5 )
			return 0;
		if ( v2->_mx == 1 && !v2->_my )
			return 0;
	}
	v4 = LineClearF1(
		PosOkMonst2,
		v1,
		v2->_mx,
		v2->_my,
		(unsigned char)v2->_menemyx,
		(unsigned char)v2->_menemyy);
	if ( !v4 || (v5 = v2->_pathcount, v5 >= 5u) && v5 < 8u )
	{
		if ( v2->_mFlags & 0x200 )
			MonstCheckDoors(v1);
		if ( ++_LOBYTE(v2->_pathcount) < 5u )
			return 0;
		if ( M_PathWalk(v1) )
			return 1;
	}
	if ( v2->MType->mtype != MT_GOLEM )
		_LOBYTE(v2->_pathcount) = 0;
	return 0;
}

void __fastcall MAI_Snake(int i)
{
	int esi1; // esi
	MonsterStruct *esi3; // esi
	bool v3; // zf
	int v4; // ecx
	int v5; // eax
	int v6; // ST1C_4
	int v7; // edi
	int v8; // edx
	int v9; // ST18_4
	int v10; // ebx
	int v11; // eax
	//int v12; // ST1C_4
	int v14; // eax
	int v15; // eax
	int v16; // ecx
	int v17; // edx
	int v18; // ecx
	int v19; // eax
	//int v20; // ST1C_4
	int v22; // eax
	//int v23; // ST1C_4
	int v24; // ebx
	int v26; // ecx
	int v27; // eax
	int v28; // ecx
	int v29; // ecx
	int v30; // eax
	int v31; // edx
	int v32; // eax
	int v33; // ecx
	int v34; // ecx
	int v35; // eax
	char pattern[6]; // [esp+4h] [ebp-1Ch]
	int micaster; // [esp+Ch] [ebp-14h]
	int midir; // [esp+10h] [ebp-10h]
	int v1; // [esp+14h] [ebp-Ch]
	int v2; // [esp+18h] [ebp-8h]
	int arglist; // [esp+1Ch] [ebp-4h]

	esi1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Snake: Invalid monster %d", i);
	pattern[2] = 0;
	pattern[3] = -1;
	pattern[4] = -1;
	pattern[5] = 0;
	esi3 = &monster[esi1];
	pattern[0] = 1;
	pattern[1] = 1;
	v3 = esi3->_mmode == MM_STAND;
	micaster = esi3->_menemy;
	if ( v3 && esi3->_msquelch )
	{
		v4 = esi3->_mx;
		v5 = (unsigned char)esi3->_menemyy;
		v6 = esi3->_lasty;
		v1 = (unsigned char)esi3->_menemyx;
		v7 = v4 - v1;
		v8 = esi3->_my;
		v9 = esi3->_lastx;
		v2 = v5;
		v10 = v8 - v5;
		midir = GetDirection(v4, v8, v9, v6);
		esi3->_mdir = midir;
		if ( abs(v7) < 2 )
		{
			v11 = abs(v10);
			//v13 = v12;
			if ( v11 < 2 )
			{
				v14 = esi3->_mVar1;
				if ( v14 == 13
					|| v14 == 14
					|| (v15 = random(105, 100), v16 = (unsigned char)esi3->_mint + 20, v15 < v16) )
				{
					M_StartAttack(arglist);
				LABEL_49:
					if ( esi3->_mmode == MM_STAND )
						esi3->_mAnimData = esi3->MType->Anims[0].Frames[esi3->_mdir];
					return;
				}
				v17 = 10 - (unsigned char)esi3->_mint + random(105, 10);
				v18 = arglist;
			LABEL_11:
				M_StartDelay(v18, v17);
				goto LABEL_49;
			}
		}
		v19 = abs(v7);
		//v21 = v20;
		if ( v19 >= 3 || (v22 = abs(v10), v22 >= 3) ) /* v21 = v23,  */
		{
			v24 = arglist;
		}
		else
		{
			v24 = arglist;
			if ( LineClearF1(PosOkMonst, arglist, esi3->_mx, esi3->_my, v1, v2) && esi3->_mVar1 != 14 )
			{
				if ( AddMissile(esi3->_mx, esi3->_my, v1, v2, midir, 20, micaster, arglist, 0, 0) != -1 )
				{
					PlayEffect(arglist, 0);
					v26 = esi3->_my + 112 * esi3->_mx;
					esi3->_mmode = MM_CHARGE;
					dMonster[0][v26] = -1 - v24;
				}
				goto LABEL_49;
			}
		}
		if ( esi3->_mVar1 != 13 )
		{
			v27 = random(106, 100);
			v28 = 2 * (unsigned char)esi3->_mint;
			if ( v27 < 35 - v28 )
			{
				v17 = 15 - (unsigned char)esi3->_mint + random(106, 10);
				v18 = v24;
				goto LABEL_11;
			}
		}
		v29 = esi3->_mgoalvar1;
		v30 = midir + pattern[v29];
		if ( v30 >= 0 )
		{
			v31 = v30 - 8;
			if ( v30 < 8 )
				v31 = midir + pattern[v29];
		}
		else
		{
			v31 = v30 + 8;
		}
		esi3->_mgoalvar1 = v29 + 1;
		if ( v29 + 1 > 5 )
			esi3->_mgoalvar1 = 0;
		v32 = esi3->_mgoalvar2;
		v33 = v31 - v32;
		if ( v31 - v32 >= 0 )
		{
			if ( v33 >= 8 )
				v33 -= 8;
		}
		else
		{
			v33 += 8;
		}
		if ( v33 <= 0 )
		{
		LABEL_47:
			if ( !M_DumbWalk(v24, esi3->_mgoalvar2) )
				M_CallWalk2(v24, esi3->_mdir);
			goto LABEL_49;
		}
		if ( v33 >= 4 )
		{
			if ( v33 == 4 )
			{
				esi3->_mgoalvar2 = v31;
				goto LABEL_47;
			}
			v34 = v32 - 1;
			if ( v32 - 1 < 0 )
			{
				v35 = v32 + 7;
				goto LABEL_46;
			}
			if ( v34 >= 8 )
			{
				v35 = v32 - 9;
				goto LABEL_46;
			}
		}
		else
		{
			v34 = v32 + 1;
			if ( v32 + 1 < 0 )
			{
				v35 = v32 + 9;
			LABEL_46:
				esi3->_mgoalvar2 = v35;
				goto LABEL_47;
			}
			if ( v34 >= 8 )
			{
				v35 = v32 - 7;
				goto LABEL_46;
			}
		}
		v35 = v34;
		goto LABEL_46;
	}
}

void __fastcall MAI_Bat(int i)
{
	int esi1; // esi
	MonsterStruct *esi3; // esi
	int v3; // ecx
	int v4; // edx
	int v5; // edi
	int v6; // ebx
	int v7; // eax
	int v10; // edx
	bool v11; // eax
	int v12; // ecx
	CMonster *v14; // eax
	int v15; // edi
	int v16; // eax
	signed int v17; // ecx
	int v18; // eax
	int micaster; // [esp+Ch] [ebp-18h]
	int v1; // [esp+10h] [ebp-14h]
	int v2; // [esp+14h] [ebp-10h]
	int v22; // [esp+18h] [ebp-Ch]
	int midir; // [esp+1Ch] [ebp-8h]
	int arglist; // [esp+20h] [ebp-4h]

	esi1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Bat: Invalid monster %d", i);
	esi3 = &monster[esi1];
	micaster = esi3->_menemy;
	if ( esi3->_mmode == MM_STAND && esi3->_msquelch )
	{
		v3 = esi3->_mx;
		v4 = esi3->_my;
		v5 = v3 - (unsigned char)esi3->_menemyx;
		v6 = v4 - (unsigned char)esi3->_menemyy;
		v7 = GetDirection(v3, v4, esi3->_lastx, esi3->_lasty);
		midir = v7;
		esi3->_mdir = v7;
		v22 = random(107, 100);
		if ( _LOBYTE(esi3->_mgoal) == 2 )
		{
			if ( esi3->_mgoalvar1 )
			{
				if ( random(108, 2) )
					v10 = left[midir];
				else
					v10 = right[midir];
				M_CallWalk(arglist, v10);
				_LOBYTE(esi3->_mgoal) = 1;
			}
			else
			{
				M_CallWalk(arglist, opposite[midir]);
				++esi3->_mgoalvar1;
			}
		}
		else
		{
			v1 = (unsigned char)esi3->_menemyx;
			v2 = (unsigned char)esi3->_menemyy;
			if ( esi3->MType->mtype == MT_GLOOM
				&& (abs(v5) >= 5 || abs(v6) >= 5)
				&& v22 < 4 * (unsigned char)esi3->_mint + 33
				&& (v11 = LineClearF1(
					PosOkMonst,
					arglist,
					esi3->_mx,
					esi3->_my,
					v1,
					v2),
					v11) )
			{
				if ( AddMissile(esi3->_mx, esi3->_my, v1, v2, midir, 20, micaster, arglist, 0, 0) != -1 )
				{
					v12 = esi3->_my + 112 * esi3->_mx;
					esi3->_mmode = MM_CHARGE;
					dMonster[0][v12] = -1 - arglist;
				}
			}
			else if ( abs(v5) >= 2 || abs(v6) >= 2 )
			{
				v17 = esi3->_mVar2;
				if ( v17 > 20 && v22 < (unsigned char)esi3->_mint + 13
					|| ((v18 = esi3->_mVar1, v18 == 1) || v18 == 2 || v18 == 3)
					&& !v17
					&& v22 < (unsigned char)esi3->_mint + 63 )
				{
					M_CallWalk(arglist, midir);
				}
			}
			else if ( v22 < 4 * (unsigned char)esi3->_mint + 8 )
			{
				M_StartAttack(arglist);
				v14 = esi3->MType;
				esi3->_mgoalvar1 = 0;
				_LOBYTE(esi3->_mgoal) = 2;
				if ( v14->mtype == 41 )
				{
					v15 = (unsigned char)esi3->_menemyx;
					v16 = random(109, 10);
					AddMissile(v15, (unsigned char)esi3->_menemyy, v15 + 1, 0, -1, 8, 1, arglist, v16 + 1, 0);
				}
			}
			if ( esi3->_mmode == MM_STAND )
				esi3->_mAnimData = esi3->MType->Anims[0].Frames[midir];
		}
	}
}

void __fastcall MAI_SkelBow(int i)
{
	int v1; // esi
	MonsterStruct *v2; // esi
	int v3; // edi
	int v4; // ebx
	int v5; // eax
	int v7; // eax
	//int v8; // ST04_4
	int v9; // ecx
	int v10; // eax
	//int v11; // ST04_4
	int v12; // eax
	//int v13; // eax
	int v14; // edi
	int v15; // ebx
	//int v16; // eax
	int v17; // [esp+4h] [ebp-10h]
	bool v18; // [esp+8h] [ebp-Ch]
	int v19; // [esp+Ch] [ebp-8h]
	int arglist; // [esp+10h] [ebp-4h]

	v18 = 0;
	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_SkelBow: Invalid monster %d", i);
	v2 = &monster[v1];
	if ( v2->_mmode == MM_STAND && v2->_msquelch )
	{
		v3 = v2->_mx - (unsigned char)v2->_menemyx;
		v4 = v2->_my - (unsigned char)v2->_menemyy;
		v5 = M_GetDir(arglist);
		v17 = v5;
		v2->_mdir = v5;
		v19 = random(110, 100);
		v7 = abs(v3);
		//v9 = v8;
		if ( v7 < 4 )
		{
			v10 = abs(v4);
			//v9 = v11;
			if ( v10 < 4 )
			{
				if ( (v9 = v2->_mVar2, v9 > 20) && v19 < 2 * (unsigned char)v2->_mint + 13
					|| ((v12 = v2->_mVar1, v12 == 1) || v12 == 2 || v12 == 3)
					&& !v9
					&& v19 < 2 * (unsigned char)v2->_mint + 63 )
				{
					//_LOBYTE(v13) = M_DumbWalk(arglist, opposite[v17]);
					v18 = M_DumbWalk(arglist, opposite[v17]);
				}
			}
		}
		v14 = (unsigned char)v2->_menemyx;
		v15 = (unsigned char)v2->_menemyy;
		if ( !v18 )
		{
			if ( random(110, 100) < 2 * (unsigned char)v2->_mint + 3 )
			{
				//_LOBYTE(v16) = LineClear(v2->_mx, v2->_my, v14, v15);
				if ( LineClear(v2->_mx, v2->_my, v14, v15) )
					M_StartRAttack(arglist, 0, 4);
			}
		}
		if ( v2->_mmode == MM_STAND )
			v2->_mAnimData = v2->MType->Anims[0].Frames[v17];
	}
}

void __fastcall MAI_Fat(int i)
{
	int v1; // esi
	MonsterStruct *v2; // esi
	int v3; // edi
	int v4; // ebx
	int v5; // eax
	int v7; // eax
	signed int v8; // ecx
	int v9; // eax
	int md; // [esp+4h] [ebp-Ch]
	int arglist; // [esp+8h] [ebp-8h]
	int v12; // [esp+Ch] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Fat: Invalid monster %d", i);
	v2 = &monster[v1];
	if ( v2->_mmode == MM_STAND && v2->_msquelch )
	{
		v3 = v2->_mx - (unsigned char)v2->_menemyx;
		v4 = v2->_my - (unsigned char)v2->_menemyy;
		v5 = M_GetDir(arglist);
		md = v5;
		v2->_mdir = v5;
		v12 = random(111, 100);
		if ( abs(v3) >= 2 || abs(v4) >= 2 )
		{
			v8 = v2->_mVar2;
			if ( v8 > 20 && v12 < 4 * (unsigned char)v2->_mint + 20
				|| ((v9 = v2->_mVar1, v9 == 1) || v9 == 2 || v9 == 3) && !v8 && v12 < 4 * (unsigned char)v2->_mint + 70 )
			{
				M_CallWalk(arglist, md);
			}
		}
		else
		{
			v7 = (unsigned char)v2->_mint;
			if ( v12 >= 4 * v7 + 15 )
			{
				if ( v12 < 4 * v7 + 20 )
					M_StartSpAttack(arglist);
			}
			else
			{
				M_StartAttack(arglist);
			}
		}
		if ( v2->_mmode == MM_STAND )
			v2->_mAnimData = v2->MType->Anims[0].Frames[md];
	}
}

void __fastcall MAI_Sneak(int i)
{
	int v1; // edi
	MonsterStruct *v2; // esi
	int v3; // ebx
	int v4; // ebx
	int v6; // edi
	int v7; // eax
	//int v8; // ST04_4
	int v9; // eax
	//int v10; // ST04_4
	int v11; // eax
	int v12; // edi
	signed int v13; // ecx
	int v14; // eax
	int v15; // [esp+Ch] [ebp-10h]
	int arglist; // [esp+10h] [ebp-Ch]
	int v17; // [esp+14h] [ebp-8h]
	int md; // [esp+18h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Sneak: Invalid monster %d", i);
	v2 = &monster[v1];
	if ( v2->_mmode == MM_STAND )
	{
		v3 = v2->_my;
		if ( dTransVal[v2->_mx][v3] != lightmax )
		{
			v17 = v2->_mx - (unsigned char)v2->_menemyx;
			v4 = v3 - (unsigned char)v2->_menemyy;
			md = M_GetDir(v1);
			v6 = 5 - (unsigned char)v2->_mint;
			if ( v2->_mVar1 == 5 )
			{
				v2->_mgoalvar1 = 0;
				_LOBYTE(v2->_mgoal) = 2;
			}
			else
			{
				v7 = abs(v17);
				//v5 = v8;
				if ( v7 >= v6 + 3 || (v9 = abs(v4), v9 >= v6 + 3) || v2->_mgoalvar1 > 8 ) /* v5 = v10,  */
				{
					v2->_mgoalvar1 = 0;
					_LOBYTE(v2->_mgoal) = 1;
				}
			}
			if ( _LOBYTE(v2->_mgoal) == 2 )
			{
				if ( v2->_mFlags & 0x10 )
					md = GetDirection(v2->_mx, v2->_my, plr[v2->_menemy]._pownerx, plr[v2->_menemy]._pownery);
				md = opposite[md];
				if ( v2->MType->mtype == MT_UNSEEN )
				{
					if ( random(112, 2) )
						v11 = left[md];
					else
						v11 = right[md];
					md = v11;
				}
			}
			v2->_mdir = md;
			v15 = random(112, 100);
			if ( abs(v17) < v6 && abs(v4) < v6 && v2->_mFlags & 1 )
			{
				M_StartFadein(arglist, md, 0);
			}
			else
			{
				v12 = v6 + 1;
				if ( abs(v17) < v12 && abs(v4) < v12 || v2->_mFlags & 1 )
				{
					if ( _LOBYTE(v2->_mgoal) == 2
						|| (abs(v17) >= 2 || abs(v4) >= 2)
						&& ((v13 = v2->_mVar2, v13 > 20) && v15 < 4 * (unsigned char)v2->_mint + 14
							|| ((v14 = v2->_mVar1, v14 == 1) || v14 == 2 || v14 == 3)
							&& !v13
							&& v15 < 4 * (unsigned char)v2->_mint + 64) )
					{
						++v2->_mgoalvar1;
						M_CallWalk(arglist, md);
					}
				}
				else
				{
					M_StartFadeout(arglist, md, 1u);
				}
			}
			if ( v2->_mmode == MM_STAND )
			{
				if ( abs(v17) >= 2 || abs(v4) >= 2 || v15 >= 4 * (unsigned char)v2->_mint + 10 )
					v2->_mAnimData = v2->MType->Anims[0].Frames[md];
				else
					M_StartAttack(arglist);
			}
		}
	}
}
// 642A14: using guessed type char lightmax;

void __fastcall MAI_Fireman(int i)
{
	int esi1; // esi
	int esi3; // esi
	int v3; // ecx
	int v4; // eax
	int v5; // ebx
	int v6; // edi
	int v7; // edx
	char v9; // al
	//int v10; // eax
	//int v11; // eax
	int v13; // eax
	//int v14; // eax
	int v15; // edx
	//int v16; // eax
	int v17; // eax
	int micaster; // [esp+Ch] [ebp-14h]
	int v1; // [esp+10h] [ebp-10h]
	int v2; // [esp+14h] [ebp-Ch]
	int midir; // [esp+18h] [ebp-8h]
	int arglist; // [esp+1Ch] [ebp-4h]

	esi1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Fireman: Invalid monster %d", i);
	esi3 = esi1;
	if ( monster[esi3]._mmode || !monster[esi3]._msquelch )
		return;
	v3 = (unsigned char)monster[esi3]._menemyy;
	micaster = monster[esi3]._menemy;
	v4 = (unsigned char)monster[esi3]._menemyx;
	v2 = v3;
	v5 = monster[esi3]._my - v3;
	v1 = v4;
	v6 = monster[esi3]._mx - v4;
	v7 = M_GetDir(arglist);
	v9 = monster[esi3]._mgoal;
	midir = v7;
	switch ( v9 )
	{
	case 1:
		//_LOBYTE(v10) = LineClear(monster[esi3]._mx, monster[esi3]._my, v1, v2);
		if ( !LineClear(monster[esi3]._mx, monster[esi3]._my, v1, v2)
			|| AddMissile(monster[esi3]._mx, monster[esi3]._my, v1, v2, midir, 50, micaster, arglist, 0, 0) == -1 )
		{
			break;
		}
		monster[esi3]._mgoalvar1 = 0;
		monster[esi3]._mmode = MM_CHARGE;
		goto LABEL_18;
	case 5:
		if ( monster[esi3]._mgoalvar1 == 3 )
		{
			_LOBYTE(monster[esi3]._mgoal) = 1;
			M_StartFadeout(arglist, v7, 1u);
		}
		else
		{
			//_LOBYTE(v11) = LineClear(monster[esi3]._mx, monster[esi3]._my, v1, v2);
			if ( LineClear(monster[esi3]._mx, monster[esi3]._my, v1, v2) )
			{
				M_StartRAttack(arglist, 51, 4);
			}
			else
			{
				//_LOBYTE(v11) = LineClear(monster[esi3]._mx, monster[esi3]._my, v1, v2);
				if ( LineClear(monster[esi3]._mx, monster[esi3]._my, v1, v2) )
				{
					M_StartRAttack(arglist, 51, 4);
				}
				else
				{
					v13 = random(112, 10);
					M_StartDelay(arglist, v13 + 5);
				}
				++monster[esi3]._mgoalvar1;
			}
			++monster[esi3]._mgoalvar1;
		}
		break;
	case 2:
		M_StartFadein(arglist, v7, 0);
	LABEL_18:
		_LOBYTE(monster[esi3]._mgoal) = 5;
		break;
	}
	monster[esi3]._mdir = midir;
	random(112, 100);
	if ( monster[esi3]._mmode == MM_STAND )
		return;
	if ( abs(v6) < 2 && abs(v5) < 2 && _LOBYTE(monster[esi3]._mgoal) == 1 )
	{
		M_TryH2HHit(
			arglist,
			monster[esi3]._menemy,
			(unsigned char)monster[esi3].mHit,
			(unsigned char)monster[esi3].mMinDamage,
			(unsigned char)monster[esi3].mMaxDamage);
		_LOBYTE(monster[esi3]._mgoal) = 2;
		//_LOBYTE(v14) = M_CallWalk(arglist, opposite[midir]);
		if ( M_CallWalk(arglist, opposite[midir]) )
			return;
		v15 = midir;
		goto LABEL_29;
	}
	//_LOBYTE(v16) = M_CallWalk(arglist, midir);
	if ( !M_CallWalk(arglist, midir) )
	{
		v17 = _LOBYTE(monster[esi3]._mgoal);
		if ( (_BYTE)v17 == 1 || (_BYTE)v17 == 2 )
		{
			v15 = midir;
		LABEL_29:
			M_StartFadein(arglist, v15, 0);
			_LOBYTE(monster[esi3]._mgoal) = 5;
			return;
		}
	}
}

void __fastcall MAI_Fallen(int i)
{
	int v1; // edi
	//int v2; // ST04_4
	int v3; // esi
	int v4; // eax
	int v5; // ecx
	int *v6; // eax
	int v7; // edx
	int v8; // edx
	int j; // edi
	int k; // ecx
	int v11; // eax
	int v12; // eax
	char v13; // al
	int v14; // edx
	int v15; // eax
	int v16; // esi

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
	{
		TermMsg("MAI_Fallen: Invalid monster %d", i);
		//i = v2;
	}
	v3 = v1;
	if ( _LOBYTE(monster[v1]._mgoal) == 5 )
	{
		i = monster[v3]._mgoalvar1;
		if ( i )
			monster[v3]._mgoalvar1 = --i;
		else
			_LOBYTE(monster[v3]._mgoal) = 1;
	}
	if ( monster[v3]._mmode == MM_STAND && monster[v3]._msquelch )
	{
		if ( _LOBYTE(monster[v3]._mgoal) == 2 )
		{
			i = monster[v3]._mgoalvar1;
			monster[v3]._mgoalvar1 = i - 1;
			if ( !i )
			{
				v4 = monster[v3]._mdir;
				_LOBYTE(monster[v3]._mgoal) = 1;
				M_StartStand(v1, opposite[v4]);
			}
		}
		if ( monster[v3]._mAnimFrame != monster[v3]._mAnimLen )
		{
			v13 = monster[v3]._mgoal;
			if ( v13 == 2 )
			{
				v14 = monster[v3]._mdir;
			}
			else
			{
				if ( v13 != 5 )
				{
					MAI_SkelSd(v1);
					return;
				}
				v15 = monster[v3]._mx - (unsigned char)monster[v3]._menemyx;
				v16 = monster[v3]._my - (unsigned char)monster[v3]._menemyy;
				if ( abs(v15) < 2 && abs(v16) < 2 )
				{
					M_StartAttack(v1);
					return;
				}
				v14 = M_GetDir(v1);
			}
			M_CallWalk(v1, v14);
			return;
		}
		if ( !random(113, 4) )
		{
			if ( !(monster[v3]._mFlags & 8) )
			{
				M_StartSpStand(v1, monster[v3]._mdir);
				v5 = 2 * (unsigned char)monster[v3]._mint + 2;
				v6 = &monster[v3]._mhitpoints;
				v7 = monster[v3]._mhitpoints;
				if ( monster[v3]._mmaxhp - v5 < v7 )
					*v6 = monster[v3]._mmaxhp;
				else
					*v6 = v5 + v7;
			}
			v8 = 2 * (unsigned char)monster[v3]._mint + 4;
			for ( j = -v8; j <= v8; ++j )
			{
				for ( k = -v8; k <= v8; ++k )
				{
					if ( j >= 0 && j < 112 && k >= 0 && k < 112 )
					{
						v11 = dMonster[0][j + monster[v3]._my + 112 * (k + monster[v3]._mx)];
						if ( v11 > 0 )
						{
							v12 = v11 - 1;
							if ( monster[v12]._mAi == AI_FALLEN )
							{
								_LOBYTE(monster[v12]._mgoal) = 5;
								monster[v12]._mgoalvar1 = 30 * (unsigned char)monster[v3]._mint + 105;
							}
						}
					}
				}
			}
		}
	}
}

void __fastcall MAI_Cleaver(int i)
{
	int v1; // esi
	MonsterStruct *v2; // esi
	int v3; // ecx
	int v4; // edx
	int v5; // edi
	int v6; // ebp
	int v7; // ebx
	int arglist; // [esp+8h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Cleaver: Invalid monster %d", i);
	v2 = &monster[v1];
	if ( v2->_mmode == MM_STAND && v2->_msquelch )
	{
		v3 = v2->_mx;
		v4 = v2->_my;
		v5 = v3 - (unsigned char)v2->_menemyx;
		v6 = v4 - (unsigned char)v2->_menemyy;
		v7 = GetDirection(v3, v4, v2->_lastx, v2->_lasty);
		v2->_mdir = v7;
		if ( abs(v5) >= 2 || abs(v6) >= 2 )
			M_CallWalk(arglist, v7);
		else
			M_StartAttack(arglist);
		if ( v2->_mmode == MM_STAND )
			v2->_mAnimData = v2->MType->Anims[0].Frames[v7];
	}
}

void __fastcall MAI_Round(int i, unsigned char special)
{
	int v2; // esi
	MonsterStruct *v3; // esi
	int v4; // edx
	int v5; // ecx
	int v6; // edi
	int v7; // ebx
	int v9; // eax
	//int v10; // ST04_4
	int v12; // eax
	//int v13; // ST04_4
	int v15; // edi
	int v16; // eax
	int v17; // ecx
	bool v18; // eax
	//int v19; // eax
	int v21; // eax
	int v22; // eax
	//int v23; // ST04_4
	signed int v25; // ecx
	int v26; // eax
	int v27; // [esp+4h] [ebp-18h]
	int v28; // [esp+8h] [ebp-14h]
	char *v29; // [esp+8h] [ebp-14h]
	int v30; // [esp+Ch] [ebp-10h]
	int md; // [esp+10h] [ebp-Ch]
	int v32; // [esp+14h] [ebp-8h]
	int arglist; // [esp+18h] [ebp-4h]

	v2 = i;
	v27 = special;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Round: Invalid monster %d", i);
	v3 = &monster[v2];
	if ( v3->_mmode == MM_STAND && v3->_msquelch )
	{
		v4 = v3->_my;
		v5 = v3->_mx;
		v28 = (unsigned char)v3->_menemyy;
		v6 = (unsigned char)v3->_menemyx;
		v7 = v5 - v6;
		v32 = v4 - v28;
		md = GetDirection(v5, v4, v3->_lastx, v3->_lasty);
		if ( v3->_msquelch < 0xFFu ) /* check sign */
			MonstCheckDoors(arglist);
		v30 = random(114, 100);
		if ( (abs(v7) >= 2 || abs(v32) >= 2) && v3->_msquelch == -1 )
		{
			v29 = &dung_map[v6][v28];
			if ( dung_map[v3->_mx][v3->_my] == *v29 )
			{
				if ( _LOBYTE(v3->_mgoal) != 4 )
				{
					v9 = abs(v7);
					//v11 = v10;
					if ( v9 < 4 )
					{
						v12 = abs(v32);
						//v11 = v13;
						if ( v12 < 4 )
							goto LABEL_26;
					}
					if ( random(115, 4) )
						goto LABEL_26;
					if ( _LOBYTE(v3->_mgoal) != 4 )
					{
						v3->_mgoalvar1 = 0;
						v3->_mgoalvar2 = random(116, 2);
					}
				}
				_LOBYTE(v3->_mgoal) = 4;
				v15 = abs(v32);
				if ( abs(v7) <= v15 )
					v16 = abs(v32);
				else
					v16 = abs(v7);
				v17 = v3->_mgoalvar1;
				v3->_mgoalvar1 = v17 + 1;
				if ( v17 < 2 * v16 || (v18 = DirOK(arglist, md), !v18) )
				{
					if ( dung_map[v3->_mx][v3->_my] == *v29 )
					{
						//_LOBYTE(v19) = M_RoundWalk(arglist, md, &v3->_mgoalvar2);
						if ( !M_RoundWalk(arglist, md, &v3->_mgoalvar2) )
						{
							v21 = random(125, 10);
							M_StartDelay(arglist, v21 + 10);
						}
						goto LABEL_26;
					}
				}
			}
		}
		_LOBYTE(v3->_mgoal) = 1;
	LABEL_26:
		if ( _LOBYTE(v3->_mgoal) == 1 )
		{
			if ( abs(v7) >= 2 || (v22 = abs(v32), v22 >= 2) ) /* v24 = v23,  */
			{
				v25 = v3->_mVar2;
				if ( v25 > 20 && v30 < 2 * (unsigned char)v3->_mint + 28
					|| ((v26 = v3->_mVar1, v26 == 1) || v26 == 2 || v26 == 3)
					&& !v25
					&& v30 < 2 * (unsigned char)v3->_mint + 78 )
				{
					M_CallWalk(arglist, md);
				}
			}
			else if ( v30 < 2 * (unsigned char)v3->_mint + 23 )
			{
				v3->_mdir = md;
				if ( v27 && v3->_mhitpoints < v3->_mmaxhp >> 1 && random(117, 2) )
					M_StartSpAttack(arglist);
				else
					M_StartAttack(arglist);
			}
		}
		if ( v3->_mmode == MM_STAND )
			v3->_mAnimData = v3->MType->Anims[0].Frames[md];
	}
}

void __fastcall MAI_GoatMc(int i)
{
	MAI_Round(i, 1u);
}

void __fastcall MAI_Ranged(int i, int missile_type, unsigned char special)
{
	int v3; // edi
	int v4; // esi
	char v5; // al
	int v6; // eax
	int v7; // ecx
	int v8; // ebx
	int v9; // edi
	bool v11; // zf
	int v12; // eax
	int v13; // eax
	//int v14; // ST00_4
	//int v16; // eax
	int x2; // [esp+8h] [ebp-14h]
	int y2; // [esp+Ch] [ebp-10h]
	int missile_typea; // [esp+10h] [ebp-Ch]
	int v20; // [esp+14h] [ebp-8h]
	int arglist; // [esp+18h] [ebp-4h]

	v3 = i;
	missile_typea = missile_type;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Ranged: Invalid monster %d", i);
	v4 = v3;
	if ( monster[v3]._mmode == MM_STAND )
	{
		v5 = monster[v4]._msquelch;
		if ( v5 == -1 || monster[v4]._mFlags & 0x10 )
		{
			v7 = (unsigned char)monster[v4]._menemyy;
			y2 = v7;
			v8 = monster[v4]._my - v7;
			x2 = (unsigned char)monster[v4]._menemyx;
			v9 = monster[v4]._mx - x2;
			v20 = M_GetDir(arglist);
			if ( monster[v4]._msquelch < 0xFFu ) /* check sign */
				MonstCheckDoors(arglist);
			v11 = monster[v4]._mVar1 == 10;
			monster[v4]._mdir = v20;
			if ( v11 )
			{
				v12 = random(118, 20);
				M_StartDelay(arglist, v12);
			}
			else if ( abs(v9) < 4 )
			{
				v13 = abs(v8);
				//v15 = v14;
				if ( v13 < 4 )
				{
					if ( random(119, 100) < 10 * ((unsigned char)monster[v4]._mint + 7) )
						M_CallWalk(arglist, opposite[v20]);
				}
			}
			if ( monster[v4]._mmode == MM_STAND )
			{
				//_LOBYTE(v16) = LineClear(monster[v4]._mx, monster[v4]._my, x2, y2);
				if ( LineClear(monster[v4]._mx, monster[v4]._my, x2, y2) )
				{
					if ( special )
						M_StartRSpAttack(arglist, missile_typea, 4);
					else
						M_StartRAttack(arglist, missile_typea, 4);
				}
				else
				{
					monster[v4]._mAnimData = monster[v4].MType->Anims[0].Frames[v20];
				}
			}
		}
		else if ( v5 )
		{
			v6 = GetDirection(monster[v4]._mx, monster[v4]._my, monster[v4]._lastx, monster[v4]._lasty);
			M_CallWalk(v3, v6);
		}
	}
}

void __fastcall MAI_GoatBow(int i)
{
	MAI_Ranged(i, 0, 0);
}

void __fastcall MAI_Succ(int i)
{
	MAI_Ranged(i, 24, 0);
}

void __fastcall MAI_AcidUniq(int i)
{
	MAI_Ranged(i, 57, 1u);
}

void __fastcall MAI_Scav(int i)
{
	int v1; // edi
	int v2; // esi
	unsigned char *v3; // eax
	int v4; // ecx
	int v5; // ecx
	signed int v6; // ebx
	signed int v7; // edi
	int v8; // edx
	int v9; // eax
	bool v10; // eax
	int v11; // ebx
	int v12; // edi
	signed int v13; // edi
	int v14; // edx
	int v15; // eax
	bool v16; // eax
	int v17; // eax
	int v18; // eax
	int arglist; // [esp+Ch] [ebp-8h]
	BOOL v20; // [esp+10h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Scav: Invalid monster %d", i);
	v2 = v1;
	v20 = 0;
	if ( monster[v1]._mmode == MM_STAND )
	{
		if ( monster[v2]._mhitpoints < monster[v2]._mmaxhp >> 1 )
		{
			if ( _LOBYTE(monster[v2]._mgoal) == 3 )
				goto LABEL_10;
			if ( monster[v2].leaderflag )
			{
				v3 = &monster[(unsigned char)monster[v2].leader].unpackfilesize;
				--*v3;
				monster[v2].leaderflag = 0;
			}
			_LOBYTE(monster[v2]._mgoal) = 3;
			monster[v2]._mgoalvar3 = 10;
		}
		if ( _LOBYTE(monster[v2]._mgoal) != 3 )
		{
		LABEL_52:
			if ( monster[v2]._mmode == MM_STAND )
				MAI_SkelSd(arglist);
			return;
		}
	LABEL_10:
		v4 = monster[v2]._mgoalvar3;
		if ( v4 )
		{
			monster[v2]._mgoalvar3 = v4 - 1;
			v5 = monster[v2]._my;
			if ( dDead[monster[v2]._mx][v5] )
			{
				M_StartEat(v1);
				if ( !(monster[v2]._mFlags & 8) )
					monster[v2]._mhitpoints += 64;
				if ( monster[v2]._mhitpoints >= (monster[v2]._mmaxhp >> 1) + (monster[v2]._mmaxhp >> 2) )
				{
					_LOBYTE(monster[v2]._mgoal) = 1;
					monster[v2]._mgoalvar1 = 0;
					monster[v2]._mgoalvar2 = 0;
				}
			}
			else
			{
				if ( !monster[v2]._mgoalvar1 )
				{
					v6 = arglist;
					if ( random(120, 2) )
					{
						v7 = -4;
						do
						{
							if ( v20 )
								break;
							v6 = -4;
							do
							{
								if ( v20 )
									break;
								if ( v7 >= 0 && v7 < 112 && v6 >= 0 && v6 < 112 )
								{
									v8 = monster[v2]._mx;
									v9 = monster[v2]._my;
									v20 = dDead[v8 + v6][v9 + v7]
										&& (v10 = LineClearF(
											CheckNoSolid,
											v8,
											v9,
											v8 + v6,
											v9 + v7),
											v10);
								}
								++v6;
							}
							while ( v6 <= 4 );
							++v7;
						}
						while ( v7 <= 4 );
						v11 = v6 - 1;
						v12 = v7 - 1;
					}
					else
					{
						v13 = 4;
						do
						{
							if ( v20 )
								break;
							v6 = 4;
							do
							{
								if ( v20 )
									break;
								if ( v13 >= 0 && v13 < 112 && v6 >= 0 && v6 < 112 )
								{
									v14 = monster[v2]._mx;
									v15 = monster[v2]._my;
									v20 = dDead[v14 + v6][v15 + v13]
										&& (v16 = LineClearF(
											CheckNoSolid,
											v14,
											v15,
											v14 + v6,
											v15 + v13),
											v16);
								}
								--v6;
							}
							while ( v6 >= -4 );
							--v13;
						}
						while ( v13 >= -4 );
						v11 = v6 + 1;
						v12 = v13 + 1;
					}
					if ( v20 )
					{
						monster[v2]._mgoalvar1 = monster[v2]._mx + v11 + 1;
						monster[v2]._mgoalvar2 = monster[v2]._my + v12 + 1;
					}
				}
				v17 = monster[v2]._mgoalvar1;
				if ( v17 )
				{
					v18 = GetDirection(monster[v2]._mx, monster[v2]._my, v17 - 1, monster[v2]._mgoalvar2 - 1);
					monster[v2]._mdir = v18;
					M_CallWalk(arglist, v18);
				}
			}
		}
		goto LABEL_52;
	}
}

void __fastcall MAI_Garg(int i)
{
	int v1; // ebp
	MonsterStruct *v2; // esi
	int v3; // edi
	int v4; // ebx
	char v5; // al
	int v6; // edi
	//int v7; // eax
	int v8; // [esp+10h] [ebp-4h]

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Garg: Invalid monster %d", i);
	v2 = &monster[v1];
	v3 = v2->_mx - v2->_lastx;
	v4 = v2->_my - v2->_lasty;
	v8 = M_GetDir(v1);
	v5 = v2->_msquelch;
	if ( v5 && v2->_mFlags & 4 )
	{
		M_Enemy(v1);
		v6 = v2->_my - (unsigned char)v2->_menemyy;
		if ( abs(v2->_mx - (unsigned char)v2->_menemyx) < (unsigned char)v2->_mint + 2
			&& abs(v6) < (unsigned char)v2->_mint + 2 )
		{
			v2->_mFlags &= 0xFFFFFFFB;
		}
	}
	else if ( v2->_mmode == MM_STAND && v5 )
	{
		if ( v2->_mhitpoints < v2->_mmaxhp >> 1 && !(v2->_mFlags & 8) )
			_LOBYTE(v2->_mgoal) = 2;
		if ( _LOBYTE(v2->_mgoal) == 2 )
		{
			if ( abs(v3) >= (unsigned char)v2->_mint + 2 || abs(v4) >= (unsigned char)v2->_mint + 2 )
			{
				_LOBYTE(v2->_mgoal) = 1;
				M_StartHeal(v1);
			}
			else
			{
				//_LOBYTE(v7) = M_CallWalk(v1, opposite[v8]);
				if ( !M_CallWalk(v1, opposite[v8]) )
					_LOBYTE(v2->_mgoal) = 1;
			}
		}
		MAI_Round(v1, 0);
	}
}

void __fastcall MAI_RoundRanged(int i, int missile_type, unsigned char checkdoors, int dam, int lessmissiles)
{
	int v5; // esi
	MonsterStruct *v6; // esi
	int v7; // edx
	int v8; // ebx
	int v9; // edi
	int v11; // eax
	//int v12; // ST04_4
	int v13; // ecx
	int v14; // eax
	//int v15; // ST04_4
	int v16; // eax
	//int v17; // ST04_4
	int v18; // ecx
	int v19; // ebx
	int v20; // eax
	int v21; // ecx
	bool v22; // eax
	bool v23; // eax
	bool v24; // eax
	int v25; // eax
	//int v26; // ST04_4
	int v27; // eax
	//int v28; // ST04_4
	int v29; // eax
	int v30; // edx
	int v31; // eax
	int missile_typea; // [esp+4h] [ebp-18h]
	int v33; // [esp+8h] [ebp-14h]
	int x2; // [esp+Ch] [ebp-10h]
	int md; // [esp+10h] [ebp-Ch]
	int y2; // [esp+14h] [ebp-8h]
	int arglist; // [esp+18h] [ebp-4h]
	int checkdoorsa; // [esp+24h] [ebp+8h]

	v5 = i;
	missile_typea = missile_type;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_RoundRanged: Invalid monster %d", i);
	v6 = &monster[v5];
	if ( v6->_mmode == MM_STAND && v6->_msquelch )
	{
		v7 = v6->_my;
		y2 = (unsigned char)v6->_menemyy;
		v8 = v7 - y2;
		x2 = (unsigned char)v6->_menemyx;
		v9 = v6->_mx - x2;
		v33 = v7 - y2;
		md = GetDirection(v6->_mx, v7, v6->_lastx, v6->_lasty);
		if ( checkdoors && v6->_msquelch < 0xFFu ) /* check sign */
			MonstCheckDoors(arglist);
		checkdoorsa = random(121, 10000);
		v11 = abs(v9);
		//v13 = v12;
		if ( v11 < 2 )
		{
			v14 = abs(v8);
			//v13 = v15;
			if ( v14 < 2 )
				goto LABEL_50;
		}
		if ( v6->_msquelch != -1 )
			goto LABEL_50;
		//v13 = y2;
		if ( dung_map[v6->_mx][v6->_my] != dung_map[x2][y2] )
			goto LABEL_50;
		if ( _LOBYTE(v6->_mgoal) != 4 )
		{
			if ( abs(v9) < 3 )
			{
				v16 = abs(v8);
				//v13 = v17;
				if ( v16 < 3 )
					goto LABEL_28;
			}
			v18 = lessmissiles;
			if ( random(122, 4 << lessmissiles) )
				goto LABEL_28;
			if ( _LOBYTE(v6->_mgoal) != 4 )
			{
				v6->_mgoalvar1 = 0;
				v6->_mgoalvar2 = random(123, 2);
			}
		}
		_LOBYTE(v6->_mgoal) = 4;
		v19 = abs(v8);
		if ( abs(v9) <= v19 )
		{
			v8 = v33;
			v20 = abs(v33);
		}
		else
		{
			v20 = abs(v9);
			v8 = v33;
		}
		v21 = v6->_mgoalvar1;
		v6->_mgoalvar1 = v21 + 1;
		if ( v21 >= 2 * v20 && (v22 = DirOK(arglist, md), v22) )
		{
		LABEL_50:
			_LOBYTE(v6->_mgoal) = 1;
		}
		else if ( checkdoorsa < 500 * ((unsigned char)v6->_mint + 1) >> lessmissiles
			&& (v23 = LineClear(v6->_mx, v6->_my, x2, y2), v23) )
		{
			M_StartRSpAttack(arglist, missile_typea, dam);
		}
		else
		{
			M_RoundWalk(arglist, md, &v6->_mgoalvar2);
		}
	LABEL_28:
		if ( _LOBYTE(v6->_mgoal) == 1 )
		{
			if ( ((abs(v9) >= 3 || abs(v8) >= 3) && checkdoorsa < 500 * ((unsigned char)v6->_mint + 2) >> lessmissiles
				|| checkdoorsa < 500 * ((unsigned char)v6->_mint + 1) >> lessmissiles)
				&& (v24 = LineClear(v6->_mx, v6->_my, x2, y2), v24) )
			{
				M_StartRSpAttack(arglist, missile_typea, dam);
			}
			else
			{
				v25 = abs(v9);
				//v13 = v26;
				if ( v25 >= 2 || (v27 = abs(v8), v27 >= 2) ) /* v13 = v28,  */
				{
					v29 = random(124, 100);
					v30 = (unsigned char)v6->_mint;
					if ( v29 < 1000 * (v30 + 5)
						|| ((v13 = v6->_mVar1, v13 == 1) || v13 == 2 || v13 == 3) && !v6->_mVar2 && v29 < 1000 * (v30 + 8) )
					{
						M_CallWalk(arglist, md);
					}
				}
				else if ( checkdoorsa < 1000 * ((unsigned char)v6->_mint + 6) )
				{
					v6->_mdir = md;
					M_StartAttack(arglist);
				}
			}
		}
		if ( v6->_mmode == MM_STAND )
		{
			v31 = random(125, 10);
			M_StartDelay(arglist, v31 + 5);
		}
	}
}

void __fastcall MAI_Magma(int i)
{
	MAI_RoundRanged(i, 21, 1u, 4, 0);
}

void __fastcall MAI_Storm(int i)
{
	MAI_RoundRanged(i, 22, 1u, 4, 0);
}

void __fastcall MAI_Acid(int i)
{
	MAI_RoundRanged(i, 57, 0, 4, 1);
}

void __fastcall MAI_Diablo(int i)
{
	MAI_RoundRanged(i, 67, 0, 40, 0);
}

void __fastcall MAI_RR2(int i, int mistype, int dam)
{
	int v3; // ebx
	MonsterStruct *v4; // esi
	int v5; // edi
	int v6; // edx
	int v7; // ebx
	int v8; // edi
	int v10; // eax
	//int v11; // ST04_4
	int v12; // ecx
	int v13; // eax
	//int v14; // ST04_4
	int v15; // eax
	//int v16; // ST04_4
	int v17; // eax
	//int v18; // ST04_4
	int v19; // ebx
	int v20; // eax
	bool v21; // eax
	bool v22; // eax
	int v23; // ecx
	int v24; // eax
	//int v25; // ST04_4
	int v27; // eax
	//int v28; // ST04_4
	int v29; // eax
	int v30; // eax
	int v31; // eax
	int v32; // edx
	int v33; // eax
	int missile_type; // [esp+Ch] [ebp-1Ch]
	int x2; // [esp+10h] [ebp-18h]
	int v36; // [esp+14h] [ebp-14h]
	int y2; // [esp+18h] [ebp-10h]
	int v38; // [esp+1Ch] [ebp-Ch]
	int md; // [esp+20h] [ebp-8h]
	int arglist; // [esp+24h] [ebp-4h]

	v3 = i;
	missile_type = mistype;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_RR2: Invalid monster %d", i);
	v4 = &monster[v3];
	v5 = v4->_my - (unsigned char)v4->_menemyy;
	if ( abs(v4->_mx - (unsigned char)v4->_menemyx) >= 5 || abs(v5) >= 5 )
	{
		MAI_SkelSd(v3);
		return;
	}
	if ( v4->_mmode == MM_STAND && v4->_msquelch )
	{
		v6 = v4->_my;
		y2 = (unsigned char)v4->_menemyy;
		v7 = v6 - y2;
		x2 = (unsigned char)v4->_menemyx;
		v8 = v4->_mx - x2;
		v36 = v6 - y2;
		md = GetDirection(v4->_mx, v6, v4->_lastx, v4->_lasty);
		if ( v4->_msquelch < 0xFFu ) /* check sign */
			MonstCheckDoors(arglist);
		v38 = random(121, 100);
		v10 = abs(v8);
		//v12 = v11;
		if ( v10 >= 2 || (v13 = abs(v7), v13 >= 2) ) /* v12 = v14,  */
		{
			if ( v4->_msquelch == -1 )
			{
				//v12 = y2;
				if ( dung_map[v4->_mx][v4->_my] == dung_map[x2][y2] )
				{
					if ( _LOBYTE(v4->_mgoal) != 4 )
					{
						v15 = abs(v8);
						//v12 = v16;
						if ( v15 < 3 )
						{
							v17 = abs(v7);
							//v12 = v18;
							if ( v17 < 3 )
								goto LABEL_26;
						}
						if ( _LOBYTE(v4->_mgoal) != 4 )
						{
							v4->_mgoalvar1 = 0;
							v4->_mgoalvar2 = random(123, 2);
						}
					}
					_LOBYTE(v4->_mgoal) = 4;
					v4->_mgoalvar3 = 4;
					v19 = abs(v7);
					if ( abs(v8) <= v19 )
					{
						v7 = v36;
						v20 = abs(v36);
					}
					else
					{
						v20 = abs(v8);
						v7 = v36;
					}
					v12 = v4->_mgoalvar1;
					v4->_mgoalvar1 = v12 + 1;
					if ( v12 < 2 * v20 || (v21 = DirOK(arglist, md), !v21) )
					{
						if ( v38 < 5 * ((unsigned char)v4->_mint + 16) )
							M_RoundWalk(arglist, md, &v4->_mgoalvar2);
					LABEL_26:
						if ( _LOBYTE(v4->_mgoal) != 1 )
							goto LABEL_48;
						if ( ((abs(v8) >= 3 || abs(v7) >= 3) && v38 < 5 * ((unsigned char)v4->_mint + 2)
							|| v38 < 5 * ((unsigned char)v4->_mint + 1)
							|| v4->_mgoalvar3 == 4)
							&& (v22 = LineClear(v4->_mx, v4->_my, x2, y2), v22) )
						{
							v23 = arglist;
						}
						else
						{
							v24 = abs(v8);
							//v26 = v25;
							if ( v24 >= 2 || (v27 = abs(v7), v27 >= 2) ) /* v26 = v28,  */
							{
								v31 = random(124, 100);
								v12 = (unsigned char)v4->_mint;
								if ( v31 < 2 * (5 * v12 + 25)
									|| ((v32 = v4->_mVar1, v32 == 1) || v32 == 2 || v32 == 3)
									&& !v4->_mVar2
									&& (v12 = 2 * (5 * v12 + 40), v31 < v12) )
								{
									M_CallWalk(arglist, md);
								}
								goto LABEL_47;
							}
							v29 = random(124, 100);
							v12 = 10 * ((unsigned char)v4->_mint + 4);
							if ( v29 >= v12 )
							{
							LABEL_47:
								v4->_mgoalvar3 = 1;
							LABEL_48:
								if ( v4->_mmode == MM_STAND )
								{
									v33 = random(125, 10);
									M_StartDelay(arglist, v33 + 5);
								}
								return;
							}
							v4->_mdir = md;
							v30 = random(124, 2);
							v23 = arglist;
							if ( v30 )
							{
								M_StartAttack(arglist);
								goto LABEL_47;
							}
						}
						M_StartRSpAttack(v23, missile_type, dam);
						goto LABEL_47;
					}
				}
			}
		}
		_LOBYTE(v4->_mgoal) = 1;
		goto LABEL_26;
	}
}

void __fastcall MAI_Mega(int i)
{
	MAI_RR2(i, 49, 0);
}

void __fastcall MAI_Golum(int i)
{
	int v1; // edi
	int v2; // esi
	int v3; // eax
	int v4; // eax
	int v5; // edx
	int v6; // edi
	int v7; // ebx
	int v8; // eax
	char v9; // cl
	//char v10; // eax
	signed int v11; // edx
	signed int v12; // ecx
	int v13; // eax
	bool v14; // eax
	unsigned char *v15; // esi
	bool v16; // eax
	int v17; // esi
	int v18; // edi
	int v19; // [esp+Ch] [ebp-Ch]
	unsigned int v20; // [esp+10h] [ebp-8h]
	int arglist; // [esp+14h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Golum: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v1]._mx != 1 || monster[v2]._my )
	{
		v3 = monster[v2]._mmode;
		if ( v3 != MM_DEATH && v3 != MM_SPSTAND && (v3 < MM_WALK || v3 > MM_WALK3) )
		{
			if ( !(monster[v2]._mFlags & 0x10) )
				M_Enemy(v1);
			v20 = ((unsigned int)~monster[v2]._mFlags >> 10) & 1;
			if ( monster[v2]._mmode != MM_ATTACK )
			{
				v4 = monster[v2]._menemy;
				v5 = monster[v2]._my;
				v6 = monster[v2]._mx - monster[v4]._mfutx;
				v7 = v5 - monster[v4]._mfuty;
				v19 = GetDirection(monster[v2]._mx, v5, monster[v4]._mx, monster[v4]._my);
				monster[v2]._mdir = v19;
				if ( abs(v6) >= 2 || abs(v7) >= 2 )
				{
					if ( v20 )
					{
						v14 = MAI_Path(arglist);
						if ( v14 )
							return;
					}
				}
				else if ( v20 )
				{
					v8 = monster[v2]._menemy;
					monster[v2]._menemyx = monster[v8]._mx;
					v9 = monster[v8]._my;
					monster[v2]._menemyy = v9;
					if ( !monster[v8]._msquelch )
					{
						monster[v8]._msquelch = -1;
						monster[monster[v2]._menemy]._lastx = monster[v2]._mx;
						v11 = 0;
						monster[monster[v2]._menemy]._lasty = monster[v2]._my;
						do
						{
							v12 = 0;
							do
							{
								/* v13 = *(_DWORD *)&nTransTable[4
															* (monster[v2]._my + v11 + 112 * (v12 + monster[v2]._mx))
															+ 1148]; check */
								v13 = dMonster[monster[v2]._mx + v12 - 2][monster[v2]._my + v11 - 2];
								if ( v13 > 0 )
									monster[v13]._msquelch = -1;
								++v12;
							}
							while ( v12 < 5 );
							++v11;
						}
						while ( v11 < 5 );
					}
					M_StartAttack(arglist);
					return;
				}
				v15 = &monster[v2]._pathcount;
				if ( ++*(_BYTE *)v15 > 8u )
					*(_BYTE *)v15 = 5;
				v16 = M_CallWalk(arglist, plr[arglist]._pdir);
				if ( !v16 )
				{
					v17 = ((_BYTE)v19 - 1) & 7;
					v18 = 0;
					while ( !v16 )
					{
						v17 = ((_BYTE)v17 + 1) & 7;
						v16 = DirOK(arglist, v17);
						if ( ++v18 >= 8 )
						{
							if ( !v16 )
								return;
							break;
						}
					}
					M_WalkDir(arglist, v17);
				}
			}
		}
	}
}

void __fastcall MAI_SkelKing(int i)
{
	int v1; // esi
	MonsterStruct *v2; // esi
	int v3; // edx
	int v4; // ebx
	int v5; // edi
	int v7; // eax
	//int v8; // ST04_4
	int v10; // eax
	//int v11; // ST04_4
	int v13; // ebx
	int v14; // eax
	int v15; // ecx
	bool v16; // eax
	//int v17; // eax
	int v19; // eax
	bool v20; // eax
	int v21; // edi
	int v22; // ebx
	int v23; // eax
	//int v24; // ST04_4
	int v26; // eax
	//int v27; // ST04_4
	int v28; // eax
	int v29; // ecx
	int v30; // edx
	int v31; // eax
	char *v32; // [esp+4h] [ebp-1Ch]
	int x2; // [esp+8h] [ebp-18h]
	int v34; // [esp+Ch] [ebp-14h]
	int v35; // [esp+10h] [ebp-10h]
	int y2; // [esp+14h] [ebp-Ch]
	int md; // [esp+18h] [ebp-8h]
	int arglist; // [esp+1Ch] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_SkelKing: Invalid monster %d", i);
	v2 = &monster[v1];
	if ( v2->_mmode == MM_STAND && v2->_msquelch )
	{
		v3 = v2->_my;
		y2 = (unsigned char)v2->_menemyy;
		v4 = v3 - y2;
		x2 = (unsigned char)v2->_menemyx;
		v5 = v2->_mx - x2;
		v34 = v3 - y2;
		md = GetDirection(v2->_mx, v3, v2->_lastx, v2->_lasty);
		if ( v2->_msquelch < 0xFFu ) /* check sign */
			MonstCheckDoors(arglist);
		v35 = random(126, 100);
		if ( (abs(v5) >= 2 || abs(v4) >= 2) && v2->_msquelch == -1 )
		{
			v32 = &dung_map[x2][y2];
			if ( dung_map[v2->_mx][v2->_my] == *v32 )
			{
				if ( _LOBYTE(v2->_mgoal) != 4 )
				{
					v7 = abs(v5);
					//v9 = v8;
					if ( v7 < 3 )
					{
						v10 = abs(v4);
						//v9 = v11;
						if ( v10 < 3 )
							goto LABEL_26;
					}
					if ( random(127, 4) )
						goto LABEL_26;
					if ( _LOBYTE(v2->_mgoal) != 4 )
					{
						v2->_mgoalvar1 = 0;
						v2->_mgoalvar2 = random(128, 2);
					}
				}
				_LOBYTE(v2->_mgoal) = 4;
				v13 = abs(v4);
				if ( abs(v5) <= v13 )
				{
					v4 = v34;
					v14 = abs(v34);
				}
				else
				{
					v14 = abs(v5);
					v4 = v34;
				}
				v15 = v2->_mgoalvar1;
				v2->_mgoalvar1 = v15 + 1;
				if ( v15 < 2 * v14 || (v16 = DirOK(arglist, md), !v16) )
				{
					if ( dung_map[v2->_mx][v2->_my] == *v32 )
					{
						//_LOBYTE(v17) = M_RoundWalk(arglist, md, &v2->_mgoalvar2);
						if ( !M_RoundWalk(arglist, md, &v2->_mgoalvar2) )
						{
							v19 = random(125, 10);
							M_StartDelay(arglist, v19 + 10);
						}
						goto LABEL_26;
					}
				}
			}
		}
		_LOBYTE(v2->_mgoal) = 1;
	LABEL_26:
		if ( _LOBYTE(v2->_mgoal) == 1 )
		{
			if ( gbMaxPlayers == 1
				&& ((abs(v5) >= 3 || abs(v4) >= 3) && v35 < 4 * (unsigned char)v2->_mint + 35 || v35 < 6)
				&& (v20 = LineClear(v2->_mx, v2->_my, x2, y2), v20) )
			{
				v21 = v2->_mx + offset_x[md];
				v22 = v2->_my + offset_y[md];
				if ( PosOkMonst(arglist, v21, v22) && nummonsters < MAXMONSTERS )
				{
					M_SpawnSkel(v21, v22, md);
					M_StartSpStand(arglist, md);
				}
			}
			else
			{
				v23 = abs(v5);
				//v25 = v24;
				if ( v23 >= 2 || (v26 = abs(v4), v26 >= 2) ) /* v25 = v27,  */
				{
					v28 = random(129, 100);
					v29 = (unsigned char)v2->_mint;
					if ( v28 >= v29 + 25
						&& ((v30 = v2->_mVar1, v30 != 1) && v30 != 2 && v30 != 3 || v2->_mVar2 || (v29 += 75, v28 >= v29)) )
					{
						v31 = random(130, 10);
						M_StartDelay(arglist, v31 + 10);
					}
					else
					{
						M_CallWalk(arglist, md);
					}
				}
				else if ( v35 < (unsigned char)v2->_mint + 20 )
				{
					v2->_mdir = md;
					M_StartAttack(arglist);
				}
			}
		}
		if ( v2->_mmode == MM_STAND )
			v2->_mAnimData = v2->MType->Anims[0].Frames[md];
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall MAI_Rhino(int i)
{
	int esi1; // esi
	MonsterStruct *esi3; // esi
	int v3; // edx
	int v4; // ebx
	int v5; // edi
	int v7; // eax
	//int v8; // ST1C_4
	int v10; // eax
	//int v11; // ST1C_4
	int v13; // ebx
	int v14; // eax
	int v15; // ecx
	//int v16; // eax
	int v18; // eax
	bool v19; // eax
	int v20; // ecx
	int v21; // eax
	//int v22; // ST1C_4
	int v24; // eax
	//int v25; // ST1C_4
	int v26; // eax
	int v27; // ecx
	int v28; // edx
	int v29; // eax
	int v30; // [esp+4h] [ebp-18h]
	int v31; // [esp+8h] [ebp-14h]
	int v1; // [esp+Ch] [ebp-10h]
	int midir; // [esp+10h] [ebp-Ch]
	int v2; // [esp+14h] [ebp-8h]
	int arglist; // [esp+18h] [ebp-4h]

	esi1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Rhino: Invalid monster %d", i);
	esi3 = &monster[esi1];
	if ( esi3->_mmode == MM_STAND && esi3->_msquelch )
	{
		v3 = esi3->_my;
		v2 = (unsigned char)esi3->_menemyy;
		v4 = v3 - v2;
		v1 = (unsigned char)esi3->_menemyx;
		v5 = esi3->_mx - v1;
		v31 = v3 - v2;
		midir = GetDirection(esi3->_mx, v3, esi3->_lastx, esi3->_lasty);
		if ( esi3->_msquelch < 0xFFu ) /* check sign */
			MonstCheckDoors(arglist);
		v30 = random(131, 100);
		if ( abs(v5) >= 2 || abs(v4) >= 2 )
		{
			if ( _LOBYTE(esi3->_mgoal) != 4 )
			{
				v7 = abs(v5);
				//v9 = v8;
				if ( v7 < 5 )
				{
					v10 = abs(v4);
					//v9 = v11;
					if ( v10 < 5 )
						goto LABEL_23;
				}
				if ( !random(132, 4) )
					goto LABEL_23;
				if ( _LOBYTE(esi3->_mgoal) != 4 )
				{
					esi3->_mgoalvar1 = 0;
					esi3->_mgoalvar2 = random(133, 2);
				}
			}
			_LOBYTE(esi3->_mgoal) = 4;
			v13 = abs(v4);
			if ( abs(v5) <= v13 )
			{
				v4 = v31;
				v14 = abs(v31);
			}
			else
			{
				v14 = abs(v5);
				v4 = v31;
			}
			v15 = esi3->_mgoalvar1;
			esi3->_mgoalvar1 = v15 + 1;
			if ( v15 < 2 * v14 && dung_map[esi3->_mx][esi3->_my] == dung_map[v1][v2] )
			{
				//_LOBYTE(v16) = M_RoundWalk(arglist, midir, &esi3->_mgoalvar2);
				if ( !M_RoundWalk(arglist, midir, &esi3->_mgoalvar2) )
				{
					v18 = random(125, 10);
					M_StartDelay(arglist, v18 + 10);
				}
				goto LABEL_23;
			}
		}
		_LOBYTE(esi3->_mgoal) = 1;
	LABEL_23:
		if ( _LOBYTE(esi3->_mgoal) == 1 )
		{
			if ( (abs(v5) >= 5 || abs(v4) >= 5)
				&& v30 < 2 * (unsigned char)esi3->_mint + 43
				&& (v19 = LineClearF1(
					PosOkMonst,
					arglist,
					esi3->_mx,
					esi3->_my,
					v1,
					v2),
					v19) )
			{
				if ( AddMissile(esi3->_mx, esi3->_my, v1, v2, midir, 20, esi3->_menemy, arglist, 0, 0) != -1 )
				{
					if ( esi3->MData->snd_special )
						PlayEffect(arglist, 3);
					v20 = esi3->_my + 112 * esi3->_mx;
					esi3->_mmode = MM_CHARGE;
					dMonster[0][v20] = -1 - arglist;
				}
			}
			else
			{
				v21 = abs(v5);
				//v23 = v22;
				if ( v21 >= 2 || (v24 = abs(v4), v24 >= 2) ) /* v23 = v25,  */
				{
					v26 = random(134, 100);
					v27 = 2 * (unsigned char)esi3->_mint;
					if ( v26 >= v27 + 33
						&& ((v28 = esi3->_mVar1, v28 != 1) && v28 != 2 && v28 != 3
							|| esi3->_mVar2
							|| (v27 += 83, v26 >= v27)) )
					{
						v29 = random(135, 10);
						M_StartDelay(arglist, v29 + 10);
					}
					else
					{
						M_CallWalk(arglist, midir);
					}
				}
				else if ( v30 < 2 * (unsigned char)esi3->_mint + 28 )
				{
					esi3->_mdir = midir;
					M_StartAttack(arglist);
				}
			}
		}
		if ( esi3->_mmode == MM_STAND )
			esi3->_mAnimData = esi3->MType->Anims[0].Frames[esi3->_mdir];
	}
}

void __fastcall MAI_Counselor(int i)
{
	int v1; // ebx
	int v2; // esi
	int v3; // ecx
	int v4; // edi
	int v5; // edx
	int v6; // ebp
	char v9; // al
	int v10; // ecx
	bool v11; // zf
	bool v12; // sf
	unsigned char v13; // of
	int v14; // edx
	int v15; // ecx
	int v16; // ebx
	int v17; // eax
	int v18; // ebx
	int v19; // edx
	int v20; // ecx
	//int v21; // eax
	int v22; // eax
	//int v23; // ST1C_4
	int v25; // eax
	//int v26; // ST1C_4
	int v27; // edx
	int v28; // eax
	int v29; // eax
	int v30; // ecx
	//int v31; // eax
	int v32; // eax
	int v33; // eax
	int v34; // eax
	int md; // [esp+8h] [ebp-14h]
	int arglist; // [esp+Ch] [ebp-10h]
	int y2; // [esp+10h] [ebp-Ch]
	int x2; // [esp+14h] [ebp-8h]
	int v39; // [esp+18h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Counselor: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v1]._mmode == MM_STAND && monster[v2]._msquelch )
	{
		v3 = monster[v2]._mx;
		x2 = (unsigned char)monster[v2]._menemyx;
		v4 = v3 - x2;
		v5 = monster[v2]._my;
		y2 = (unsigned char)monster[v2]._menemyy;
		v6 = v5 - y2;
		md = GetDirection(v3, v5, monster[v2]._lastx, monster[v2]._lasty);
		if ( monster[v2]._msquelch < 0xFFu ) /* check sign */
			MonstCheckDoors(v1);
		v39 = random(121, 100);
		v9 = monster[v2]._mgoal;
		if ( v9 == 2 )
		{
			v10 = monster[v2]._mgoalvar1;
			v13 = __OFSUB__(v10, 3);
			v11 = v10 == 3;
			v12 = v10 - 3 < 0;
			v14 = v10 + 1;
			v15 = v1;
			monster[v2]._mgoalvar1 = v14;
			if ( (unsigned char)(v12 ^ v13) | v11 )
			{
				M_CallWalk(v1, opposite[md]);
				goto LABEL_39;
			}
			goto LABEL_21;
		}
		if ( v9 == 4 )
		{
			v16 = abs(v6);
			if ( abs(v4) <= v16 )
				v17 = abs(v6);
			else
				v17 = abs(v4);
			v18 = v17;
			if ( abs(v4) < 2 && abs(v6) < 2
				|| monster[v2]._msquelch != -1
				|| dung_map[monster[v2]._mx][monster[v2]._my] != dung_map[x2][y2] )
			{
				v1 = arglist;
			LABEL_20:
				v15 = v1;
			LABEL_21:
				_LOBYTE(monster[v2]._mgoal) = 1;
				M_StartFadein(v15, md, 1u);
				goto LABEL_39;
			}
			v19 = 2 * v18;
			v1 = arglist;
			v20 = monster[v2]._mgoalvar1;
			monster[v2]._mgoalvar1 = v20 + 1;
			if ( v20 >= v19 )
			{
				//_LOBYTE(v21) = DirOK(arglist, md);
				if ( DirOK(arglist, md) )
					goto LABEL_20;
			}
			M_RoundWalk(arglist, md, &monster[v2]._mgoalvar2);
		LABEL_39:
			if ( monster[v2]._mmode == MM_STAND )
			{
				v34 = random(125, 10);
				M_StartDelay(v1, v34 + 5);
			}
			return;
		}
		if ( v9 != 1 )
			goto LABEL_39;
		v22 = abs(v4);
		//v24 = v23;
		if ( v22 >= 2 || (v25 = abs(v6), v25 >= 2) ) /* v24 = v26,  */
		{
			if ( v39 < 5 * ((unsigned char)monster[v2]._mint + 10) )
			{
				//_LOBYTE(v31) = LineClear(monster[v2]._mx, monster[v2]._my, x2, y2);
				if ( LineClear(monster[v2]._mx, monster[v2]._my, x2, y2) )
				{
					v32 = random(
						77,
						(unsigned char)monster[v2].mMaxDamage - (unsigned char)monster[v2].mMinDamage + 1);
					M_StartRAttack(
						v1,
						(unsigned char)counsmiss[(unsigned char)monster[v2]._mint], /* counsmiss is local */
						(unsigned char)monster[v2].mMinDamage + v32);
					goto LABEL_39;
				}
			}
			if ( random(124, 100) < 30 )
			{
				v27 = md;
				_LOBYTE(monster[v2]._mgoal) = 4;
				goto LABEL_29;
			}
		}
		else
		{
			v27 = md;
			v28 = monster[v2]._mmaxhp >> 1;
			v13 = __OFSUB__(monster[v2]._mhitpoints, v28);
			v12 = monster[v2]._mhitpoints - v28 < 0;
			monster[v2]._mdir = md;
			if ( v12 ^ v13 )
			{
				_LOBYTE(monster[v2]._mgoal) = 2;
			LABEL_29:
				monster[v2]._mgoalvar1 = 0;
				M_StartFadeout(v1, v27, 0);
				goto LABEL_39;
			}
			if ( monster[v2]._mVar1 == 13
				|| (v29 = random(105, 100),
					v30 = 2 * (unsigned char)monster[v2]._mint + 20,
					v29 < v30) )
			{
				M_StartRAttack(v1, -1, 0);
				AddMissile(monster[v2]._mx, monster[v2]._my, 0, 0, monster[v2]._mdir, 11, 1, v1, 4, 0);
				AddMissile(monster[v2]._mx, monster[v2]._my, 0, 0, monster[v2]._mdir, 12, 1, v1, 4, 0);
				goto LABEL_39;
			}
		}
		v33 = random(105, 10);
		M_StartDelay(v1, v33 + 2 * (5 - (unsigned char)monster[v2]._mint));
		goto LABEL_39;
	}
}

void __fastcall MAI_Garbud(int i)
{
	int v1; // esi
	int v2; // esi
	int v3; // ebx
	int v4; // edi
	int v5; // eax
	//int v6; // eax
	char v7; // al
	int v8; // [esp+4h] [ebp-8h]
	int arglist; // [esp+8h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Garbud: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v2]._mmode == MM_STAND )
	{
		v3 = monster[v2]._my;
		v4 = monster[v2]._mx;
		v8 = M_GetDir(arglist);
		v5 = monster[v2].mtalkmsg;
		if ( v5 < (signed int)QUEST_GARBUD4
			&& v5 >(signed int)QUEST_DOOM10
			&& !(dFlags[v4][v3] & 2)
			&& _LOBYTE(monster[v2]._mgoal) == 7 )
		{
			_LOBYTE(monster[v2]._mgoal) = 6;
			monster[v2].mtalkmsg = v5 + 1;
		}
		if ( dFlags[v4][v3] & 2 )
		{
			if ( monster[v2].mtalkmsg == QUEST_GARBUD4 )
			{
				//_LOBYTE(v6) = effect_is_playing(USFX_GARBUD4);
				if ( !effect_is_playing(USFX_GARBUD4) && _LOBYTE(monster[v2]._mgoal) == 7 )
				{
					monster[v2]._msquelch = -1;
					monster[v2].mtalkmsg = 0;
					_LOBYTE(monster[v2]._mgoal) = 1;
				}
			}
		}
		v7 = monster[v2]._mgoal;
		if ( v7 == 1 || v7 == 4 )
			MAI_Round(arglist, 1u);
		monster[v2]._mdir = v8;
		if ( monster[v2]._mmode == MM_STAND )
			monster[v2]._mAnimData = monster[v2].MType->Anims[0].Frames[v8];
	}
}

void __fastcall MAI_Zhar(int i)
{
	int v1; // ebp
	int v2; // esi
	int v3; // ebx
	int v4; // edi
	int v5; // edi
	int v6; // ebx
	int v7; // ebp
	//int v8; // eax
	char v9; // al
	int arglist; // [esp+8h] [ebp-8h]
	int v11; // [esp+Ch] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Zhar: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v1]._mmode == MM_STAND )
	{
		v3 = monster[v2]._my;
		v4 = monster[v2]._mx;
		v11 = M_GetDir(v1);
		if ( monster[v2].mtalkmsg == QUEST_ZHAR1 && !(dFlags[v4][v3] & 2) && _LOBYTE(monster[v2]._mgoal) == 7 )
		{
			monster[v2].mtalkmsg = QUEST_ZHAR2;
			_LOBYTE(monster[v2]._mgoal) = 6;
		}
		if ( dFlags[v4][v3] & 2 )
		{
			v5 = monster[v2]._mx - (unsigned char)monster[v2]._menemyx;
			v6 = monster[v2]._my - (unsigned char)monster[v2]._menemyy;
			v7 = abs(v6);
			if ( abs(v5) <= v7 )
				abs(v6);
			else
				abs(v5);
			if ( monster[v2].mtalkmsg == QUEST_ZHAR2 )
			{
				//_LOBYTE(v8) = effect_is_playing(USFX_ZHAR2);
				if ( !effect_is_playing(USFX_ZHAR2) && _LOBYTE(monster[v2]._mgoal) == 7 )
				{
					monster[v2]._msquelch = -1;
					monster[v2].mtalkmsg = 0;
					_LOBYTE(monster[v2]._mgoal) = 1;
				}
			}
		}
		v9 = monster[v2]._mgoal;
		if ( v9 == 1 || v9 == 2 || v9 == 4 )
			MAI_Counselor(arglist);
		monster[v2]._mdir = v11;
		if ( monster[v2]._mmode == MM_STAND )
			monster[v2]._mAnimData = monster[v2].MType->Anims[0].Frames[v11];
	}
}

void __fastcall MAI_SnotSpil(int i)
{
	int v1; // ebp
	int v2; // esi
	int v3; // ebx
	int v4; // edi
	int v5; // ebp
	//int v6; // eax
	char v7; // al
	int arglist; // [esp+8h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_SnotSpil: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v1]._mmode == MM_STAND )
	{
		v3 = monster[v2]._my;
		v4 = monster[v2]._mx;
		v5 = M_GetDir(v1);
		if ( monster[v2].mtalkmsg == QUEST_BANNER10 && !(dFlags[v4][v3] & 2) && _LOBYTE(monster[v2]._mgoal) == 7 )
		{
			monster[v2].mtalkmsg = QUEST_BANNER11;
			_LOBYTE(monster[v2]._mgoal) = 6;
		}
		if ( monster[v2].mtalkmsg == QUEST_BANNER11 && quests[7]._qvar1 == 3 )
		{
			monster[v2].mtalkmsg = 0;
			_LOBYTE(monster[v2]._mgoal) = 1;
		}
		if ( dFlags[v4][v3] & 2 )
		{
			if ( monster[v2].mtalkmsg == QUEST_BANNER12 )
			{
				//_LOBYTE(v6) = effect_is_playing(USFX_SNOT3);
				if ( !effect_is_playing(USFX_SNOT3) && _LOBYTE(monster[v2]._mgoal) == 7 )
				{
					ObjChangeMap(setpc_x, setpc_y, setpc_w + setpc_x + 1, setpc_h + setpc_y + 1);
					quests[7]._qvar1 = 3;
					RedoPlayerVision();
					monster[v2]._msquelch = -1;
					monster[v2].mtalkmsg = 0;
					_LOBYTE(monster[v2]._mgoal) = 1;
				}
			}
			if ( quests[7]._qvar1 == 3 )
			{
				v7 = monster[v2]._mgoal;
				if ( v7 == 1 || v7 == 5 )
					MAI_Fallen(arglist);
			}
		}
		monster[v2]._mdir = v5;
		if ( monster[v2]._mmode == MM_STAND )
			monster[v2]._mAnimData = monster[v2].MType->Anims[0].Frames[v5];
	}
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void __fastcall MAI_Lazurus(int i)
{
	int v1; // ebx
	int v2; // esi
	int v3; // ebp
	int v4; // edi
	int v5; // ebx
	//int v6; // eax
	char v7; // al
	int v8; // eax
	int arglist; // [esp+8h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Lazurus: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v1]._mmode == MM_STAND )
	{
		v3 = monster[v2]._my;
		v4 = monster[v2]._mx;
		v5 = M_GetDir(v1);
		if ( dFlags[v4][v3] & 2 )
		{
			if ( gbMaxPlayers != 1 )
				goto LABEL_29;
			if ( monster[v2].mtalkmsg == QUEST_VILE13 )
			{
				if ( _LOBYTE(monster[v2]._mgoal) == 6 && plr[myplr].WorldX == QUEST_VILE13 && plr[myplr].WorldY == 46 )
				{
					PlayInGameMovie("gendata\\fprst3.smk");
					monster[v2]._mmode = MM_TALK;
					quests[15]._qvar1 = 5;
				}
				if ( monster[v2].mtalkmsg == QUEST_VILE13 )
				{
					//_LOBYTE(v6) = effect_is_playing(USFX_LAZ1);
					if ( !effect_is_playing(USFX_LAZ1) && _LOBYTE(monster[v2]._mgoal) == 7 )
					{
						ObjChangeMapResync(1, 18, 20, 24);
						RedoPlayerVision();
						monster[v2]._msquelch = -1;
						monster[v2].mtalkmsg = 0;
						quests[15]._qvar1 = 6;
						_LOBYTE(monster[v2]._mgoal) = 1;
					}
				}
			}
			if ( gbMaxPlayers != 1 )
			{
			LABEL_29:
				if ( monster[v2].mtalkmsg == QUEST_VILE13 && _LOBYTE(monster[v2]._mgoal) == 6 && quests[15]._qvar1 <= 3u )
					monster[v2]._mmode = MM_TALK;
			}
		}
		v7 = monster[v2]._mgoal;
		if ( v7 == 1 || v7 == 2 || v7 == 4 )
		{
			monster[v2].mtalkmsg = 0;
			MAI_Counselor(arglist);
		}
		monster[v2]._mdir = v5;
		v8 = monster[v2]._mmode;
		if ( v8 == MM_STAND || v8 == MM_TALK )
			monster[v2]._mAnimData = monster[v2].MType->Anims[0].Frames[v5];
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall MAI_Lazhelp(int i)
{
	int v1; // esi
	int v2; // esi
	int v3; // ebx
	int v4; // edi
	int v5; // [esp+4h] [ebp-8h]
	int ia; // [esp+8h] [ebp-4h]

	v1 = i;
	ia = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Lazhelp: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v2]._mmode == MM_STAND )
	{
		v3 = monster[v2]._my;
		v4 = monster[v2]._mx;
		v5 = M_GetDir(ia);
		if ( dFlags[v4][v3] & 2 )
		{
			if ( gbMaxPlayers == 1 )
			{
				if ( quests[15]._qvar1 <= 5u )
				{
					_LOBYTE(monster[v2]._mgoal) = 6;
					goto LABEL_10;
				}
				monster[v2].mtalkmsg = 0;
			}
			_LOBYTE(monster[v2]._mgoal) = 1;
		}
	LABEL_10:
		if ( _LOBYTE(monster[v2]._mgoal) == 1 )
			MAI_Succ(ia);
		monster[v2]._mdir = v5;
		if ( monster[v2]._mmode == MM_STAND )
			monster[v2]._mAnimData = monster[v2].MType->Anims[0].Frames[v5];
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall MAI_Lachdanan(int i)
{
	int v1; // ebp
	int v2; // esi
	int v3; // ebx
	int v4; // edi
	//int v5; // eax
	int v6; // [esp+8h] [ebp-4h]

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Lachdanan: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v1]._mmode == MM_STAND )
	{
		v3 = monster[v2]._my;
		v4 = monster[v2]._mx;
		v6 = M_GetDir(v1);
		if ( monster[v2].mtalkmsg == QUEST_VEIL9 && !(dFlags[v4][v3] & 2) && _LOBYTE(monster[v2]._mgoal) == 7 )
		{
			monster[v2].mtalkmsg = QUEST_VEIL10;
			_LOBYTE(monster[v2]._mgoal) = 6;
		}
		if ( dFlags[v4][v3] & 2 )
		{
			if ( monster[v2].mtalkmsg == QUEST_VEIL11 )
			{
				//_LOBYTE(v5) = effect_is_playing(USFX_LACH3);
				if ( !effect_is_playing(USFX_LACH3) && _LOBYTE(monster[v2]._mgoal) == 7 )
				{
					monster[v2].mtalkmsg = 0;
					quests[4]._qactive = 3;
					M_StartKill(v1, -1);
				}
			}
		}
		monster[v2]._mdir = v6;
		if ( monster[v2]._mmode == MM_STAND )
			monster[v2]._mAnimData = monster[v2].MType->Anims[0].Frames[v6];
	}
}

void __fastcall MAI_Warlord(int i)
{
	int v1; // ebp
	int v2; // esi
	int v3; // ebx
	int v4; // edi
	int v5; // ebp
	//int v6; // eax
	int v7; // eax
	int arglist; // [esp+8h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("MAI_Warlord: Invalid monster %d", i);
	v2 = v1;
	if ( monster[v1]._mmode == MM_STAND )
	{
		v3 = monster[v2]._my;
		v4 = monster[v2]._mx;
		v5 = M_GetDir(v1);
		if ( dFlags[v4][v3] & 2 && monster[v2].mtalkmsg == QUEST_WARLRD9 )
		{
			if ( _LOBYTE(monster[v2]._mgoal) == 6 )
				monster[v2]._mmode = MM_TALK;
			//_LOBYTE(v6) = effect_is_playing(USFX_WARLRD1);
			if ( !effect_is_playing(USFX_WARLRD1) && _LOBYTE(monster[v2]._mgoal) == 7 )
			{
				monster[v2]._msquelch = -1;
				monster[v2].mtalkmsg = 0;
				_LOBYTE(monster[v2]._mgoal) = 1;
			}
		}
		if ( _LOBYTE(monster[v2]._mgoal) == 1 )
			MAI_SkelSd(arglist);
		monster[v2]._mdir = v5;
		v7 = monster[v2]._mmode;
		if ( v7 == MM_STAND || v7 == MM_TALK )
			monster[v2]._mAnimData = monster[v2].MType->Anims[0].Frames[v5];
	}
}

void __cdecl DeleteMonsterList()
{
	int *v0; // eax
	signed int v1; // ecx

	v0 = &monster[0]._my;
	do
	{
		if ( v0[18] )
		{
			*(v0 - 1) = 1;
			*v0 = 0;
			v0[1] = 0;
			v0[2] = 0;
			v0[3] = 0;
			v0[4] = 0;
			v0[18] = 0;
		}
		v0 += 57;
	}
	while ( (signed int)v0 < (signed int)&monster[4]._my );
	v1 = 4;
	while ( v1 < nummonsters )
	{
		if ( monster[monstactive[v1]]._mDelFlag )
		{
			DeleteMonster(v1);
			v1 = 0;
		}
		else
		{
			++v1;
		}
	}
}

void __cdecl ProcessMonsters()
{
	int v0; // edi
	int v1; // esi
	int v2; // ecx
	int v3; // eax
	char *v4; // ebx
	unsigned int v5; // eax
	int v6; // eax
	int v7; // edx
	int v8; // eax
	unsigned int v9; // eax
	int v10; // eax
	bool v11; // zf
	char *v12; // ecx
	char *v13; // eax
	int v14; // ecx
	int v15; // eax
	char v16; // al
	int v17; // ecx
	bool v18; // eax
	int v19; // eax
	int v20; // ecx
	int *v21; // eax
	int *v22; // eax
	int v23; // [esp+0h] [ebp-Ch]
	int v24; // [esp+4h] [ebp-8h]
	int v25; // [esp+8h] [ebp-4h]

	DeleteMonsterList();
	v24 = 0;
	if ( nummonsters <= 0 )
		goto LABEL_60;
	do
	{
		v25 = 0;
		v23 = monstactive[v24];
		v0 = v23;
		v1 = v23;
		if ( (unsigned char)gbMaxPlayers > 1u )
		{
			SetRndSeed(monster[v1]._mAISeed);
			monster[v1]._mAISeed = GetRndSeed();
		}
		if ( !(monster[v1]._mFlags & 8) )
		{
			v2 = monster[v1]._mhitpoints;
			if ( v2 < monster[v1]._mmaxhp && (signed int)(v2 & 0xFFFFFFC0) > 0 )
			{
				v3 = SLOBYTE(monster[v1].mLevel);
				if ( (char)v3 > 1 )
					v3 = (char)v3 >> 1;
				monster[v1]._mhitpoints = v2 + v3;
			}
		}
		v4 = &dFlags[monster[v1]._mx][monster[v1]._my];
		if ( *v4 & 2 && !monster[v1]._msquelch && monster[v1].MType->mtype == MT_CLEAVER )
			PlaySFX(USFX_CLEAVER);
		if ( monster[v1]._mFlags & 0x10 )
		{
			v5 = monster[v1]._menemy;
			if ( v5 >= MAXMONSTERS )
				TermMsg("Illegal enemy monster %d for monster \"%s\"", v5, monster[v1].mName);
			v6 = monster[v1]._menemy;
			v7 = monster[v6]._mfutx;
			monster[v1]._lastx = v7;
			monster[v1]._menemyx = v7;
			v8 = monster[v6]._mfuty;
			monster[v1]._menemyy = v8;
			monster[v1]._lasty = v8;
		}
		else
		{
			v9 = monster[v1]._menemy;
			if ( v9 >= MAX_PLRS )
				TermMsg("Illegal enemy player %d for monster \"%s\"", v9, monster[v1].mName);
			v10 = monster[v1]._menemy;
			v11 = (*v4 & 2) == 0;
			v12 = (char *)&plr[v10]._px;
			v13 = (char *)&plr[v10]._py;
			monster[v1]._menemyx = *v12;
			monster[v1]._menemyy = *v13;
			if ( v11 )
			{
				v16 = monster[v1]._msquelch;
				if ( v16 && monster[v1]._mAi != MT_DIABLO ) /// BUGFIX: test `MT_DIABLO` with 'MType->mtype' instead of '_mAi'
					monster[v1]._msquelch = v16 - 1;
			}
			else
			{
				v14 = *(_DWORD *)v12;
				v15 = *(_DWORD *)v13;
				monster[v1]._msquelch = -1;
				monster[v1]._lastx = v14;
				monster[v1]._lasty = v15;
			}
			v0 = v23;
		}
		while ( 1 )
		{
			v17 = v0;
			if ( monster[v1]._mFlags & 0x100 )
			{
				v18 = MAI_Path(v0);
				if ( v18 )
					goto LABEL_30;
				v17 = v0;
			}
			AiProc[(unsigned char)monster[v1]._mAi](v17);
		LABEL_30:
			switch ( monster[v1]._mmode )
			{
			case MM_STAND:
				v19 = M_DoStand(v0);
				goto LABEL_48;
			case MM_WALK:
				v19 = M_DoWalk(v0);
				goto LABEL_48;
			case MM_WALK2:
				v19 = M_DoWalk2(v0);
				goto LABEL_48;
			case MM_WALK3:
				v19 = M_DoWalk3(v0);
				goto LABEL_48;
			case MM_ATTACK:
				v19 = M_DoAttack(v0);
				goto LABEL_48;
			case MM_GOTHIT:
				v19 = M_DoGotHit(v0);
				goto LABEL_48;
			case MM_DEATH:
				v19 = M_DoDeath(v0);
				goto LABEL_48;
			case MM_SATTACK:
				v19 = M_DoSAttack(v0);
				goto LABEL_48;
			case MM_FADEIN:
				v19 = M_DoFadein(v0);
				goto LABEL_48;
			case MM_FADEOUT:
				v19 = M_DoFadeout(v0);
				goto LABEL_48;
			case MM_RATTACK:
				v19 = M_DoRAttack(v0);
				goto LABEL_48;
			case MM_SPSTAND:
				v19 = M_DoSpStand(v0);
				goto LABEL_48;
			case MM_RSPATTACK:
				v19 = M_DoRSpAttack(v0);
				goto LABEL_48;
			case MM_DELAY:
				v19 = M_DoDelay(v0);
				goto LABEL_48;
			case MM_CHARGE:
				goto LABEL_51;
			case MM_STONE:
				v19 = M_DoStone(v0);
				goto LABEL_48;
			case MM_HEAL:
				v19 = M_DoHeal(v0);
				goto LABEL_48;
			case MM_TALK:
				v19 = M_DoTalk(v0);
			LABEL_48:
				v25 = v19;
				break;
			default:
				break;
			}
			if ( !v25 )
				break;
			GroupUnity(v0);
		}
	LABEL_51:
		if ( monster[v1]._mmode != MM_STONE )
		{
			v20 = monster[v1]._mFlags;
			v21 = &monster[v1]._mAnimCnt;
			++*v21;
			if ( !(v20 & 4) && monster[v1]._mAnimCnt >= monster[v1]._mAnimDelay )
			{
				*v21 = 0;
				v22 = &monster[v1]._mAnimFrame;
				if ( v20 & 2 )
				{
					v11 = (*v22)-- == 1;
					if ( v11 )
						*v22 = monster[v1]._mAnimLen;
				}
				else if ( ++*v22 > monster[v1]._mAnimLen )
				{
					*v22 = 1;
				}
			}
		}
		++v24;
	}
	while ( v24 < nummonsters );
LABEL_60:
	DeleteMonsterList();
}
// 679660: using guessed type char gbMaxPlayers;

void __cdecl FreeMonsters()
{
	void **v0; // edi
	int v1; // ebx
	signed int v2; // ebp
	void **v3; // esi
	void *v4; // ecx
	int v5; // [esp+0h] [ebp-4h]

	v5 = 0;
	if ( nummtypes > 0 )
	{
		v0 = (void **)Monsters[0].Anims;
		do
		{
			v1 = *((unsigned char *)v0 - 4);
			v2 = 0;
			v3 = v0;
			do
			{
				if ( animletter[v2] != 's' || monsterdata[v1].has_special )
				{
					v4 = *v3;
					*v3 = 0;
					mem_free_dbg(v4);
				}
				++v2;
				v3 += 11;
			}
			while ( v2 < 6 );
			++v5;
			v0 += 82;
		}
		while ( v5 < nummtypes );
	}
	FreeMissiles2();
}

bool __fastcall DirOK(int i, int mdir)
{
	int v2; // ebx
	int v3; // esi
	int v4; // ebx
	int v5; // edi
	int v6; // esi
	int v7; // edi
	bool v8; // zf
	int v9; // edx
	unsigned char *v11; // ebx
	unsigned char v12; // al
	int v13; // edx
	int v14; // eax
	int v15; // edi
	int v16; // ecx
	signed int j; // esi
	int v18; // eax
	bool v19; // zf
	int v20; // eax
	int v21; // [esp+Ch] [ebp-14h]
	int v22; // [esp+10h] [ebp-10h]
	int v23; // [esp+14h] [ebp-Ch]
	int a1; // [esp+18h] [ebp-8h]
	int v25; // [esp+1Ch] [ebp-4h]
	int v26; // [esp+1Ch] [ebp-4h]

	v2 = i;
	v3 = mdir;
	v25 = mdir;
	a1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("DirOK: Invalid monster %d", i);
	v4 = v2;
	v5 = offset_y[v3];
	v6 = monster[v4]._mx + offset_x[v3];
	v7 = monster[v4]._my + v5;
	if ( v7 < 0 || v7 >= 112 || v6 < 0 || v6 >= 112 || !PosOkMonst(a1, v6, v7) )
		return 0;
	if ( v25 == DIR_E )
	{
		if ( !SolidLoc(v6, v7 + 1) )
		{
			v8 = (dFlags[v6][v7 + 1] & 0x10) == 0;
			goto LABEL_18;
		}
		return 0;
	}
	if ( v25 == DIR_W )
	{
		if ( SolidLoc(v6 + 1, v7) )
			return 0;
		v8 = (dFlags[v6 + 1][v7] & 0x10) == 0;
	}
	else
	{
		if ( v25 == DIR_N )
		{
			if ( SolidLoc(v6 + 1, v7) )
				return 0;
			v9 = v7 + 1;
		}
		else
		{
			if ( v25 )
				goto LABEL_24;
			if ( SolidLoc(v6 - 1, v7) )
				return 0;
			v9 = v7 - 1;
		}
		v8 = SolidLoc(v6, v9) == 0;
	}
LABEL_18:
	if ( !v8 )
		return 0;
LABEL_24:
	if ( monster[v4].leaderflag == 1 )
	{
		v11 = &monster[v4].leader;
		if ( abs(v6 - monster[(unsigned char)*v11]._mfutx) >= 4
			|| abs(v7 - monster[(unsigned char)*v11]._mfuty) >= 4 )
		{
			return 0;
		}
		return 1;
	}
	v12 = monster[v4]._uniqtype;
	if ( !v12 || !(UniqMonst[v12 - 1].mUnqAttr & 2) )
		return 1;
	v26 = 0;
	v13 = v6 - 3;
	v21 = v6 + 3;
	if ( v6 - 3 <= v6 + 3 )
	{
		v14 = v7 - 3;
		v15 = v7 + 3;
		v23 = v14;
		v22 = v15;
		v16 = 112 * v13;
		do
		{
			for ( j = v23; j <= v15; ++j )
			{
				if ( j >= 0 && j < 112 && v16 >= 0 && v16 < 12544 )
				{
					v18 = dMonster[0][v16 + j];
					v19 = v18 == 0;
					if ( v18 < 0 )
					{
						v18 = -v18;
						v19 = v18 == 0;
					}
					if ( !v19 )
						--v18;
					v20 = v18;
					if ( monster[v20].leaderflag == 1
						&& (unsigned char)monster[v20].leader == a1
						&& monster[v20]._mfutx == v13
						&& monster[v20]._mfuty == j )
					{
						++v26;
					}
				}
				v15 = v22;
			}
			++v13;
			v16 += 112;
		}
		while ( v13 <= v21 );
	}
	return v26 == (unsigned char)monster[v4].unpackfilesize;
}

BOOL __fastcall PosOkMissile(int x, int y)
{
	return !nMissileTable[dPiece[x][y]] && !(dFlags[x][y] & 0x10);
}

BOOL __fastcall CheckNoSolid(int x, int y)
{
	return nSolidTable[dPiece[x][y]] == 0;
}

BOOL __fastcall LineClearF(BOOL(__fastcall *Clear)(int, int), int x1, int y1, int x2, int y2)
{
	int v5; // esi
	int v6; // edi
	int v7; // ebx
	int v8; // eax
	int v9; // eax
	int v10; // eax
	int v11; // ebx
	int v12; // esi
	signed int v13; // edi
	int v14; // edx
	int v15; // ecx
	int v16; // eax
	int v17; // eax
	int v18; // eax
	int v19; // ebx
	int v20; // edi
	signed int v21; // esi
	int v22; // ecx
	int v25; // [esp+10h] [ebp-10h]
	int v26; // [esp+14h] [ebp-Ch]
	int v27; // [esp+18h] [ebp-8h]
	int v28; // [esp+18h] [ebp-8h]
	int v29; // [esp+1Ch] [ebp-4h]

	v5 = y2 - y1;
	v29 = x1;
	v25 = x1;
	v26 = y1;
	v6 = x2 - x1;
	v7 = abs(y2 - y1);
	if ( abs(v6) <= v7 )
	{
		if ( v5 < 0 )
		{
			v16 = y1;
			y1 = y2;
			y2 = v16;
			v17 = v29;
			v5 = -v5;
			v29 = x2;
			x2 = v17;
			v6 = -v6;
		}
		v18 = 2 * v6;
		v28 = 2 * v6;
		if ( v6 <= 0 )
		{
			v19 = v18 + v5;
			v20 = 2 * (v5 + v6);
			v21 = -1;
		}
		else
		{
			v19 = v18 - v5;
			v20 = 2 * (v6 - v5);
			v21 = 1;
		}
		while ( 1 )
		{
			v22 = v29;
			if ( y1 == y2 && v29 == x2 )
				break;
			if ( v19 <= 0 == v21 < 0 )
			{
				v19 += v20;
				v22 = v21 + v29;
				v29 += v21;
			}
			else
			{
				v19 += v28;
			}
			if ( (++y1 != v26 || v22 != v25) && !Clear(v22, y1) ) /* check args */
				goto LABEL_29;
		}
	}
	else
	{
		if ( v6 < 0 )
		{
			v8 = v29;
			v29 = x2;
			x2 = v8;
			v9 = y1;
			v6 = -v6;
			y1 = y2;
			y2 = v9;
			v5 = -v5;
		}
		v10 = 2 * v5;
		v27 = 2 * v5;
		if ( v5 <= 0 )
		{
			v11 = v10 + v6;
			v12 = 2 * (v6 + v5);
			v13 = -1;
		}
		else
		{
			v11 = v10 - v6;
			v12 = 2 * (v5 - v6);
			v13 = 1;
		}
		do
		{
			v14 = y1;
			if ( v29 == x2 && y1 == y2 )
				break;
			if ( v11 <= 0 == v13 < 0 )
			{
				v11 += v12;
				v14 = v13 + y1;
				y1 += v13;
			}
			else
			{
				v11 += v27;
			}
			v15 = v29 + 1;
		}
		while ( ++v29 == v25 && v14 == v26 || Clear(v15, v14) );
	LABEL_29:
		if ( v29 != x2 )
			return 0;
	}
	if ( y1 == y2 )
		return 1;
	return 0;
}

BOOL __fastcall LineClear(int x1, int y1, int x2, int y2)
{
	return LineClearF(PosOkMissile, x1, y1, x2, y2);
}

BOOL __fastcall LineClearF1(BOOL(__fastcall *Clear)(int, int, int), int monst, int x1, int y1, int x2, int y2)
{
	int v6; // esi
	int v7; // edi
	int v8; // ebx
	int v9; // eax
	int v10; // eax
	int v11; // eax
	int v12; // ebx
	int v13; // esi
	signed int v14; // edi
	int v15; // eax
	int v16; // eax
	int v17; // eax
	int v18; // eax
	int v19; // ebx
	int v20; // edi
	signed int v21; // esi
	int v22; // edx
	int v25; // [esp+10h] [ebp-10h]
	int v26; // [esp+14h] [ebp-Ch]
	int v27; // [esp+18h] [ebp-8h]
	int v28; // [esp+1Ch] [ebp-4h]
	int v29; // [esp+1Ch] [ebp-4h]

	v6 = y2 - y1;
	v25 = monst;
	v26 = x1;
	v27 = y1;
	v7 = x2 - x1;
	v8 = abs(y2 - y1);
	if ( abs(x2 - x1) <= v8 )
	{
		if ( v6 < 0 )
		{
			v16 = y1;
			y1 = y2;
			y2 = v16;
			v17 = x1;
			v6 = -v6;
			x1 = x2;
			x2 = v17;
			v7 = -v7;
		}
		v18 = 2 * v7;
		v29 = 2 * v7;
		if ( v7 <= 0 )
		{
			v19 = v18 + v6;
			v20 = 2 * (v6 + v7);
			v21 = -1;
		}
		else
		{
			v19 = v18 - v6;
			v20 = 2 * (v7 - v6);
			v21 = 1;
		}
		while ( 1 )
		{
			v22 = x1;
			if ( y1 == y2 && x1 == x2 )
				break;
			if ( v19 <= 0 == v21 < 0 )
			{
				v19 += v20;
				v22 = v21 + x1;
				x1 += v21;
			}
			else
			{
				v19 += v29;
			}
			if ( (++y1 != v27 || v22 != v26) && !Clear(v25, v22, y1) )
				goto LABEL_29;
		}
	}
	else
	{
		if ( v7 < 0 )
		{
			v9 = x1;
			x1 = x2;
			x2 = v9;
			v10 = y1;
			v7 = -v7;
			y1 = y2;
			y2 = v10;
			v6 = -v6;
		}
		v11 = 2 * v6;
		v28 = 2 * v6;
		if ( v6 <= 0 )
		{
			v12 = v11 + v7;
			v13 = 2 * (v7 + v6);
			v14 = -1;
		}
		else
		{
			v12 = v11 - v7;
			v13 = 2 * (v6 - v7);
			v14 = 1;
		}
		do
		{
			v15 = y1;
			if ( x1 == x2 && y1 == y2 )
				break;
			if ( v12 <= 0 == v14 < 0 )
			{
				v12 += v13;
				v15 = v14 + y1;
				y1 += v14;
			}
			else
			{
				v12 += v28;
			}
		}
		while ( ++x1 == v26 && v15 == v27 || Clear(v25, x1, v15) ); /* check args */
	LABEL_29:
		if ( x1 != x2 )
			return 0;
	}
	if ( y1 == y2 )
		return 1;
	return 0;
}

void __fastcall SyncMonsterAnim(int i)
{
	int v1; // esi
	int v2; // eax
	int v3; // edx
	MonsterData *v4; // esi
	CMonster *v5; // ecx
	unsigned char v6; // dl
	char *v7; // edx
	int v8; // esi
	int v9; // edx
	unsigned char *v10; // ecx
	unsigned char *v11; // edx
	int v12; // ecx
	unsigned char *v13; // edx

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("SyncMonsterAnim: Invalid monster %d", i);
	v2 = v1;
	v3 = monster[v1]._mMTidx;
	v4 = Monsters[v3].MData;
	v5 = &Monsters[v3];
	v6 = monster[v2]._uniqtype;
	monster[v2].MType = v5;
	monster[v2].MData = v4;
	if ( v6 )
		v7 = UniqMonst[v6 - 1].mName;
	else
		v7 = v4->mName;
	v8 = monster[v2]._mmode;
	monster[v2].mName = v7;
	v9 = monster[v2]._mdir;
	switch ( v8 )
	{
		case MM_STAND:
		case MM_DELAY:
		case MM_TALK:
			v10 = v5->Anims[0].Frames[v9];
			goto LABEL_13;
		case MM_WALK:
		case MM_WALK2:
		case MM_WALK3:
			v10 = v5->Anims[1].Frames[v9];
			goto LABEL_13;
		case MM_ATTACK:
		case MM_RATTACK:
			v10 = v5->Anims[2].Frames[v9];
			goto LABEL_13;
		case MM_GOTHIT:
			v10 = v5->Anims[3].Frames[v9];
			goto LABEL_13;
		case MM_DEATH:
			v10 = v5->Anims[4].Frames[v9];
			goto LABEL_13;
		case MM_SATTACK:
		case MM_FADEIN:
		case MM_FADEOUT:
		case MM_SPSTAND:
		case MM_RSPATTACK:
		case MM_HEAL:
			v10 = v5->Anims[5].Frames[v9];
LABEL_13:
			monster[v2]._mAnimData = v10;
			return;
		case MM_CHARGE:
			v11 = v5->Anims[2].Frames[v9];
			monster[v2]._mAnimFrame = 1;
			monster[v2]._mAnimData = v11;
			v12 = v5->Anims[2].Rate;
			break;
		default:
			v13 = v5->Anims[0].Frames[v9];
			monster[v2]._mAnimFrame = 1;
			monster[v2]._mAnimData = v13;
			v12 = v5->Anims[0].Rate;
			break;
	}
	monster[v2]._mAnimLen = v12;
}

void __fastcall M_FallenFear(int x, int y)
{
	int v2; // eax
	int *v3; // ebx
	int v4; // edi
	int v5; // esi
	signed int v6; // eax
	int v7; // eax
	bool v8; // zf
	int v9; // eax
	int v10; // eax
	signed int v11; // [esp-10h] [ebp-1Ch]
	int v12; // [esp+0h] [ebp-Ch]
	int x1; // [esp+4h] [ebp-8h]
	int y1; // [esp+8h] [ebp-4h]

	v2 = 0;
	y1 = y;
	x1 = x;
	v12 = 0;
	if ( nummonsters > 0 )
	{
		v3 = &monster[0]._mx;
		do
		{
			v4 = 0;
			v5 = monstactive[v2];
			v6 = monster[v5].MType->mtype;
			if ( v6 > MT_RFALLSD )
			{
				v9 = v6 - 13;
				v8 = v9 == 0;
			}
			else
			{
				if ( v6 == MT_RFALLSD || (v7 = v6 - 4) == 0 )
				{
					v11 = 7;
					goto LABEL_15;
				}
				v9 = v7 - 1;
				v8 = v9 == 0;
			}
			if ( v8 )
			{
				v11 = 5;
			}
			else
			{
				v10 = v9 - 1;
				if ( v10 )
				{
					if ( v10 != 1 )
						goto LABEL_16;
					v11 = 2;
				}
				else
				{
					v11 = 3;
				}
			}
		LABEL_15:
			v4 = v11;
		LABEL_16:
			if ( monster[v5]._mAi == AI_FALLEN
				&& v4
				&& abs(x1 - monster[v5]._mx) < 5
				&& abs(y1 - monster[v5]._my) < 5
				&& (signed int)(monster[v5]._mhitpoints & 0xFFFFFFC0) > 0 )
			{
				_LOBYTE(monster[v5]._mgoal) = 2;
				monster[v5]._mgoalvar1 = v4;
				monster[v5]._mdir = GetDirection(x1, y1, *v3, v3[1]);
			}
			v3 += 57;
			v2 = v12++ + 1;
		}
		while ( v12 < nummonsters );
	}
}

void __fastcall PrintMonstHistory(int mt)
{
	int v1; // edi
	int *v2; // ebx
	int v3; // ecx
	int v4; // eax
	int v5; // edi
	short v6; // bx
	int v7; // ebx

	v1 = mt;
	v2 = &monstkills[mt];
	sprintf(tempstr, "Total kills : %i", *v2);
	AddPanelString(tempstr, 1);
	if ( *v2 >= 30 )
	{
		v3 = monsterdata[v1].mMinHP;
		v4 = monsterdata[v1].mMaxHP;
		if ( gbMaxPlayers == 1 )
		{
			v3 = monsterdata[v1].mMinHP >> 1;
			v4 = monsterdata[v1].mMaxHP >> 1;
		}
		if ( v3 < 1 )
			v3 = 1;
		if ( v4 < 1 )
			v4 = 1;
		if ( gnDifficulty == DIFF_NIGHTMARE )
		{
			v3 = 3 * v3 + 1;
			v4 = 3 * v4 + 1;
		}
		if ( gnDifficulty == DIFF_HELL )
		{
			v3 = 4 * v3 + 3;
			v4 = 4 * v4 + 3;
		}
		sprintf(tempstr, "Hit Points : %i-%i", v3, v4);
		AddPanelString(tempstr, 1);
	}
	if ( *v2 >= 15 )
	{
		v5 = v1 << 7;
		if ( gnDifficulty == DIFF_HELL )
			v6 = *(short *)((char *)&monsterdata[0].mMagicRes2 + v5);
		else
			v6 = *(short *)((char *)&monsterdata[0].mMagicRes + v5);
		v7 = v6 & 0x3F;
		if ( v7 )
		{
			if ( v7 & 7 )
			{
				strcpy(tempstr, "Resists : ");
				if ( v7 & 1 )
					strcat(tempstr, "Magic ");
				if ( v7 & 2 )
					strcat(tempstr, "Fire ");
				if ( v7 & 4 )
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, 1);
			}
			if ( v7 & 0x38 )
			{
				strcpy(tempstr, "Immune : ");
				if ( v7 & 8 )
					strcat(tempstr, "Magic ");
				if ( v7 & 0x10 )
					strcat(tempstr, "Fire ");
				if ( v7 & 0x20 )
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, 1);
			}
		}
		else
		{
			strcpy(tempstr, "No magic resistance");
			AddPanelString(tempstr, 1);
		}
	}
	pinfoflag = 1;
}
// 4B8824: using guessed type int pinfoflag;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl PrintUniqueHistory()
{
	char v0; // bl

	v0 = monster[pcursmonst].mMagicRes & 0x3F;
	if ( v0 )
	{
		if ( monster[pcursmonst].mMagicRes & 7 )
			strcpy(tempstr, "Some Magic Resistances");
		else
			strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, 1);
		if ( v0 & 0x38 )
		{
			strcpy(tempstr, "Some Magic Immunities");
			goto LABEL_4;
		}
	}
	else
	{
		strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, 1);
	}
	strcpy(tempstr, "No Immunities");
LABEL_4:
	AddPanelString(tempstr, 1);
	pinfoflag = 1;
}
// 4B8824: using guessed type int pinfoflag;

void __fastcall MissToMonst(int i, int x, int y)
{
	int v3; // edi
	MissileStruct *v4; // edi
	unsigned int v5; // ebx
	MonsterStruct *v6; // esi
	int v7; // edx
	char v8; // al
	int v9; // eax
	char *v10; // edi
	int v11; // eax
	int v12; // edx
	char v13; // al
	char v14; // al
	int v15; // ebx
	int v16; // eax
	int v17; // esi
	int v18; // edi
	int v19; // esi
	int v20; // edx
	int *v21; // ebx
	char v22; // cl
	char v23; // al
	int v24; // esi
	int v25; // edi
	int v26; // esi
	int v27; // eax
	int v28; // eax
	int ia; // [esp+Ch] [ebp-10h]
	int v30; // [esp+10h] [ebp-Ch]
	int v31; // [esp+14h] [ebp-8h]
	int v32; // [esp+18h] [ebp-4h]
	int arglist; // [esp+24h] [ebp+8h]

	v3 = i;
	v30 = x;
	if ( (unsigned int)i >= MAXMISSILES )
		TermMsg("MissToMonst: Invalid missile %d", i);
	v4 = &missile[v3];
	v5 = v4->_misource;
	ia = v4->_misource;
	if ( v5 >= MAXMONSTERS )
		TermMsg("MissToMonst: Invalid monster %d", v5);
	v32 = v4->_mix;
	v31 = v4->_miy;
	v6 = &monster[v5];
	v6->_mx = v30;
	dMonster[0][y + 112 * v30] = v5 + 1;
	v7 = v4->_mimfnum;
	v6->_mdir = v7;
	v6->_my = y;
	M_StartStand(v5, v7);
	v8 = v6->MType->mtype;
	if ( v8 < MT_INCIN || v8 > MT_HELLBURN )
	{
		if ( v6->_mFlags & 0x10 )
			M2MStartHit(v5, -1, 0);
		else
			M_StartHit(v5, -1, 0);
	}
	else
	{
		M_StartFadein(v5, v6->_mdir, 0);
	}
	v9 = v32;
	if ( v6->_mFlags & 0x10 )
	{
		v21 = (int *)((char *)dMonster + 4 * (v31 + v9 * 112));
		if ( *v21 > 0 )
		{
			v22 = v6->MType->mtype;
			if ( v22 != MT_GLOOM && (v22 < MT_INCIN || v22 > MT_HELLBURN) )
			{
				M_TryM2MHit(ia, *v21 - 1, 500, (unsigned char)v6->mMinDamage2, (unsigned char)v6->mMaxDamage2);
				v23 = v6->MType->mtype;
				if ( v23 < MT_NSNAKE || v23 > MT_GSNAKE )
				{
					v24 = v6->_mdir;
					v25 = v32 + offset_x[v24];
					v26 = v31 + offset_y[v24];
					if ( PosOkMonst(*v21 - 1, v25, v26) )
					{
						v27 = *v21;
						dMonster[0][v26 + 112 * v25] = *v21;
						*v21 = 0;
						v28 = v27 - 1;
						monster[v28]._mx = v25;
						monster[v28]._mfutx = v25;
						monster[v28]._my = v26;
						monster[v28]._mfuty = v26;
					}
				}
			}
		}
	}
	else
	{
		v10 = &dPlayer[v9][v31];
		v11 = *v10;
		v12 = v11 - 1;
		arglist = v11 - 1;
		if ( *v10 > 0 )
		{
			v13 = v6->MType->mtype;
			if ( v13 != MT_GLOOM && (v13 < MT_INCIN || v13 > MT_HELLBURN) )
			{
				M_TryH2HHit(v5, v12, 500, (unsigned char)v6->mMinDamage2, (unsigned char)v6->mMaxDamage2);
				if ( arglist == *v10 - 1 )
				{
					v14 = v6->MType->mtype;
					if ( v14 < MT_NSNAKE || v14 > MT_GSNAKE )
					{
						v15 = arglist;
						v16 = plr[arglist]._pmode;
						if ( v16 != 7 && v16 != 8 )
							StartPlrHit(arglist, 0, 1u);
						v17 = v6->_mdir;
						v18 = v32 + offset_x[v17];
						v19 = v31 + offset_y[v17];
						if ( PosOkPlayer(arglist, v18, v19) )
						{
							v20 = plr[v15]._pdir;
							plr[v15].WorldX = v18;
							plr[v15].WorldY = v19;
							FixPlayerLocation(arglist, v20);
							FixPlrWalkTags(arglist);
							dPlayer[v18][v19] = arglist + 1;
							SetPlayerOld(arglist);
						}
					}
				}
			}
		}
	}
}

BOOL __fastcall PosOkMonst(int i, int x, int y)
{
	int v3; // edi
	signed int v4; // ebx
	int v5; // ecx
	char v6; // dl
	bool result; // eax
	int v8; // edx
	int v9; // ecx
	int v10; // [esp+Ch] [ebp-4h]

	v3 = x;
	v10 = i;
	v4 = 0;
	if ( SolidLoc(x, y) )
		return 0;
	v5 = 112 * v3;
	if ( dPlayer[v3][y] || dMonster[0][v5 + y] )
		return 0;
	v6 = dObject[0][v5 + y];
	result = 1;
	if ( v6 )
	{
		v8 = v6 <= 0 ? -1 - v6 : v6 - 1;
		if ( object[v8]._oSolidFlag )
			return 0;
	}
	_LOBYTE(v5) = dMissile[0][v5 + y];
	if ( (_BYTE)v5 )
	{
		if ( v10 >= 0 )
		{
			v5 = (char)v5;
			if ( (char)v5 > 0 )
			{
				if ( missile[v5]._mitype == 5 )
					goto LABEL_24;
				v9 = 0;
				if ( nummissiles > 0 )
				{
					do
					{
						if ( missile[missileactive[v9]]._mitype == 5 )
							v4 = 1;
						++v9;
					}
					while ( v9 < nummissiles );
					if ( v4 )
					{
					LABEL_24:
						if ( !(monster[v10].mMagicRes & 0x10) || monster[v10].MType->mtype == MT_DIABLO )
							return 0;
					}
				}
			}
		}
	}
	return result;
}

BOOL __fastcall PosOkMonst2(int i, int x, int y)
{
	int v3; // edi
	int v4; // ebx
	signed int v5; // ebp
	bool result; // eax
	char v7; // dl
	int v8; // edx
	int v9; // ecx
	int v10; // ecx

	v3 = x;
	v4 = i;
	v5 = 0;
	result = SolidLoc(x, y) == 0;
	if ( result )
	{
		v7 = dObject[v3][y];
		if ( v7 )
		{
			v8 = v7 <= 0 ? -1 - v7 : v7 - 1;
			if ( object[v8]._oSolidFlag )
				result = 0;
		}
		if ( result )
		{
			_LOBYTE(v9) = dMissile[v3][y];
			if ( (_BYTE)v9 )
			{
				if ( v4 >= 0 )
				{
					v9 = (char)v9;
					if ( (char)v9 > 0 )
					{
						if ( missile[v9]._mitype == 5 )
							goto LABEL_23;
						v10 = 0;
						if ( nummissiles > 0 )
						{
							do
							{
								if ( missile[missileactive[v10]]._mitype == 5 )
									v5 = 1;
								++v10;
							}
							while ( v10 < nummissiles );
							if ( v5 )
							{
							LABEL_23:
								if ( !(monster[v4].mMagicRes & 0x10) || monster[v4].MType->mtype == MT_DIABLO )
									result = 0;
							}
						}
					}
				}
			}
		}
	}
	return result;
}

BOOL __fastcall PosOkMonst3(int i, int x, int y)
{
	int v3; // esi
	signed int v4; // ebp
	char v5; // al
	int v6; // eax
	int v7; // eax
	int v8; // ecx
	int v9; // ecx
	bool result; // eax
	int v11; // ecx
	signed int v12; // [esp+10h] [ebp-8h]
	int v13; // [esp+14h] [ebp-4h]

	v12 = 0;
	v3 = x;
	v4 = 0;
	v13 = i;
	v5 = dObject[x][y];
	if ( v5 )
	{
		if ( v5 <= 0 )
			v6 = -1 - v5;
		else
			v6 = v5 - 1;
		v7 = v6;
		v8 = object[v7]._otype;
		v4 = 1;
		if ( v8 != 1
			&& v8 != OBJ_L1RDOOR
			&& v8 != OBJ_L2LDOOR
			&& v8 != OBJ_L2RDOOR
			&& v8 != OBJ_L3LDOOR
			&& v8 != OBJ_L3RDOOR )
		{
			v4 = 0;
		}
		if ( object[v7]._oSolidFlag && !v4 )
			return 0;
	}
	if ( SolidLoc(x, y) && !v4 || dPlayer[v3][y] || dMonster[0][v3 * 112 + y] )
		return 0;
	_LOBYTE(v9) = dMissile[v3][y];
	result = 1;
	if ( (_BYTE)v9 )
	{
		if ( v13 >= 0 )
		{
			v9 = (char)v9;
			if ( (char)v9 > 0 )
			{
				if ( missile[v9]._mitype == 5 )
					goto LABEL_33;
				v11 = 0;
				if ( nummissiles > 0 )
				{
					do
					{
						if ( missile[missileactive[v11]]._mitype == 5 )
							v12 = 1;
						++v11;
					}
					while ( v11 < nummissiles );
					if ( v12 )
					{
					LABEL_33:
						if ( !(monster[v13].mMagicRes & 0x10) || monster[v13].MType->mtype == MT_DIABLO )
							return 0;
					}
				}
			}
		}
	}
	return result;
}

BOOL __fastcall IsSkel(int mt)
{
	return mt >= MT_WSKELAX && mt <= MT_XSKELAX
		|| mt >= MT_WSKELBW && mt <= MT_XSKELBW
		|| mt >= MT_WSKELSD && mt <= MT_XSKELSD;
}

bool __fastcall IsGoat(int mt)
{
	return mt >= MT_NGOATMC && mt <= MT_GGOATMC || mt >= MT_NGOATBW && mt <= MT_GGOATBW;
}

int __fastcall M_SpawnSkel(int x, int y, int dir)
{
	CMonster *v3; // ebx
	CMonster *v4; // esi
	int v5; // edx
	int v7; // esi
	//int v8; // edx
	int v9; // eax
	int v10; // esi
	int xa; // [esp+Ch] [ebp-10h]
	int ya; // [esp+10h] [ebp-Ch]
	int v14; // [esp+14h] [ebp-8h]
	int v15; // [esp+18h] [ebp-4h]
	int v16; // [esp+18h] [ebp-4h]

	ya = y;
	xa = x;
	v5 = 0;
	if ( nummtypes <= 0 )
		return -1;
	v3 = Monsters;
	v15 = nummtypes;
	v4 = Monsters;
	do
	{
		if ( IsSkel((unsigned char)v4->mtype) )
			++v5;
		++v4;
		--v15;
	}
	while ( v15 );
	if ( !v5 )
		return -1;
	v7 = 0;
	v14 = random(136, v5);
	v16 = 0;
	if ( nummtypes > 0 )
	{
		do
		{
			if ( v16 > v14 )
				break;
			if ( IsSkel((unsigned char)v3->mtype) )
				++v16;
			++v7;
			++v3;
		}
		while ( v7 < nummtypes ); /* v8 */
	}
	v9 = AddMonster(xa, ya, dir, v7 - 1, 1);
	v10 = v9;
	if ( v9 != -1 )
		M_StartSpStand(v9, dir);
	return v10;
}

void __fastcall ActivateSpawn(int i, int x, int y, int dir)
{
	int v4; // eax

	dMonster[0][y + 112 * x] = i + 1;
	v4 = i;
	monster[v4]._mx = x;
	monster[v4]._mfutx = x;
	monster[v4]._moldx = x;
	monster[v4]._my = y;
	monster[v4]._mfuty = y;
	monster[v4]._moldy = y;
	M_StartSpStand(i, dir);
}

bool __fastcall SpawnSkeleton(int ii, int x, int y)
{
	int v3; // esi
	int v4; // ebx
	int v5; // ST04_4
	int v6; // ecx
	int v7; // edi
	int *v8; // esi
	bool v9; // eax
	int v11; // eax
	int v12; // ecx
	int v13; // edx
	int v14; // esi
	int v15; // edi
	int v16; // ST04_4
	int monstok[9]; // [esp+Ch] [ebp-34h]
	int i; // [esp+30h] [ebp-10h]
	int x2; // [esp+34h] [ebp-Ch]
	int v20; // [esp+38h] [ebp-8h]
	int *v21; // [esp+3Ch] [ebp-4h]
	int a3; // [esp+48h] [ebp+8h]
	int a3a; // [esp+48h] [ebp+8h]

	i = ii;
	v3 = x;
	x2 = x;
	if ( ii == -1 )
		return 0;
	v4 = y;
	if ( !PosOkMonst(-1, x, y) )
	{
		v20 = 0;
		v6 = y - 1;
		a3 = y - 1;
		if ( (unsigned char)(__OFSUB__(v4 - 1, v4 + 1) ^ 1) | (v4 - 1 == v4 + 1) )
		{
			v21 = monstok;
			do
			{
				v7 = v3 - 1;
				if ( (unsigned char)(__OFSUB__(v3 - 1, v3 + 1) ^ 1) | (v3 - 1 == v3 + 1) )
				{
					v8 = v21;
					do
					{
						v9 = PosOkMonst(-1, v7, a3);
						v20 |= v9;
						*v8 = v9;
						v8 += 3;
						++v7;
					}
					while ( v7 <= x2 + 1 );
					v3 = x2;
				}
				++v21;
				++a3;
			}
			while ( a3 <= v4 + 1 );
			if ( v20 )
			{
				v11 = random(137, 15);
				v12 = 0;
				v13 = 0;
				a3a = v11 + 1;
				if ( v11 + 1 > 0 )
				{
					while ( 1 )
					{
						if ( monstok[v13 + 2 * v12 + v12] )
							--a3a;
						if ( a3a <= 0 )
							break;
						if ( ++v12 == 3 )
						{
							v12 = 0;
							if ( ++v13 == 3 )
								v13 = 0;
						}
					}
				}
				v14 = v12 + v3 - 1;
				v15 = v13 + v4 - 1;
				v16 = GetDirection(v14, v15, x2, v4);
				ActivateSpawn(i, v14, v15, v16);
				return 1;
			}
		}
		return 0;
	}
	v5 = GetDirection(v3, y, v3, y);
	ActivateSpawn(i, v3, y, v5);
	return 1;
}
// 43A879: using guessed type int var_34[9];

int __cdecl PreSpawnSkeleton()
{
	int skeltypes; // edx // should be i/j
	int j; // edx // remove
	int skel; // eax
	int i; // [esp+10h] [ebp-4h] // should be skeltypes

	skeltypes = 0;

	if ( nummtypes <= 0 )
		return -1;

	for ( i = 0; i < nummtypes; i++ )
	{
		if ( IsSkel(Monsters[i].mtype) )
			++skeltypes;
	}

	if ( !skeltypes )
		return -1;

	j = random(136, skeltypes); /* check this code -i integer is messed up*/
	skeltypes = 0;

	for ( i = 0; i < nummtypes; ++i )
	{
		if ( skeltypes > j )
			break;
		if ( IsSkel(Monsters[i].mtype) )
			++skeltypes;
	}
	skel = AddMonster(0, 0, 0, i - 1, 0);
	if ( skel != -1 )
		M_StartStand(skel, 0);
	return skel;
}

void __fastcall TalktoMonster(int i)
{
	int v1; // esi
	MonsterStruct *v2; // esi
	char v3; // al
	int v4; // edi
	//int v5; // eax
	//int v6; // eax
	int inv_item_num; // [esp+8h] [ebp-4h]

	v1 = i;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("TalktoMonster: Invalid monster %d", i);
	v2 = &monster[v1];
	v3 = v2->_mAi;
	v4 = v2->_menemy;
	v2->_mmode = MM_TALK;
	if ( v3 == AI_SNOTSPIL || v3 == AI_LACHDAN )
	{
		//_LOBYTE(v5) = QuestStatus(7);
		if ( QuestStatus(7) && quests[7]._qvar1 == 2 && PlrHasItem(v4, IDI_BANNER, &inv_item_num) )
		{
			RemoveInvItem(v4, inv_item_num);
			quests[7]._qactive = 3;
			v2->mtalkmsg = QUEST_BANNER12;
			_LOBYTE(v2->_mgoal) = 6;
		}
		//_LOBYTE(v6) = QuestStatus(4);
		if ( QuestStatus(4) && v2->mtalkmsg >= (signed int)QUEST_VEIL9 )
		{
			if ( PlrHasItem(v4, IDI_GLDNELIX, &inv_item_num) )
			{
				RemoveInvItem(v4, inv_item_num);
				v2->mtalkmsg = QUEST_VEIL11;
				_LOBYTE(v2->_mgoal) = 6;
			}
		}
	}
}

void __fastcall SpawnGolum(int i, int x, int y, int mi)
{
	int v4; // edi
	int v5; // ebx
	int v6; // esi
	int v7; // eax
	int *v8; // edx
	int v9; // eax
	char v10; // cl
	int v11; // eax

	v4 = i;
	v5 = x;
	if ( (unsigned int)i >= MAXMONSTERS )
		TermMsg("SpawnGolum: Invalid monster %d", i);
	v6 = v4;
	monster[v6]._mx = v5;
	monster[v6]._my = y;
	monster[v6]._mfuty = y;
	monster[v6]._moldy = y;
	monster[v6]._mfutx = v5;
	monster[v6]._moldx = v5;
	v7 = plr[v4]._pMaxMana;
	dMonster[0][y + 112 * v5] = v4 + 1;
	_LOBYTE(monster[v6]._pathcount) = 0;
	monster[v6]._mFlags |= 0x20u;
	v8 = &missile[mi]._mispllvl;
	monster[v6].mArmorClass = 25;
	v9 = 320 * *v8 + v7 / 3;
	v10 = *(_BYTE *)v8;
	_LOBYTE(v8) = plr[v4]._pLevel;
	v9 *= 2;
	monster[v6]._mmaxhp = v9;
	monster[v6]._mhitpoints = v9;
	monster[v6].mHit = 5 * (v10 + 8) + 2 * (_BYTE)v8;
	monster[v6].mMinDamage = 2 * (v10 + 4);
	monster[v6].mMaxDamage = 2 * (v10 + 8);
	M_StartSpStand(v4, 0);
	M_Enemy(v4);
	if ( v4 == myplr )
	{
		_LOBYTE(v11) = currlevel;
		NetSendCmdGolem(
			monster[v6]._mx,
			monster[v6]._my,
			monster[v6]._mdir,
			monster[v6]._menemy,
			monster[v6]._mhitpoints,
			v11);
	}
}

bool __fastcall CanTalkToMonst(int m)
{
	int v1; // esi
	char v2; // al
	bool result; // al

	v1 = m;
	if ( (unsigned int)m >= MAXMONSTERS )
		TermMsg("CanTalkToMonst: Invalid monster %d", m);
	v2 = monster[v1]._mgoal;
	if ( v2 == 6 )
		result = 1;
	else
		result = v2 == 7;
	return result;
}

BOOL __fastcall CheckMonsterHit(int m, BOOL *ret)
{
	if ( (DWORD)m >= MAXMONSTERS ) {
		TermMsg("CheckMonsterHit: Invalid monster %d", m);
	}

	if ( monster[m]._mAi == AI_GARG && monster[m]._mFlags & 4) {
		monster[m]._mmode = MM_SATTACK;
		monster[m]._mFlags &= 0xFFFFFFFB;
		*ret = TRUE;
		return TRUE;
	}

	if ( monster[m].MType->mtype < MT_COUNSLR || monster[m].MType->mtype > MT_ADVOCATE || monster[m]._mgoal == 1 ) {
		return FALSE;
	} else {
		*ret = FALSE;
	}

	return TRUE;
}

int __fastcall encode_enemy(int m)
{
	int v1; // ecx
	int result; // eax

	v1 = m;
	result = monster[v1]._menemy;
	if ( monster[v1]._mFlags & 0x10 )
		result += 4;
	return result;
}

void __fastcall decode_enemy(int m, int enemy)
{
	int v2; // eax
	int v3; // edx
	char v4; // cl
	int v5; // edx

	v2 = m;
	if ( enemy >= 4 )
	{
		monster[v2]._mFlags |= 0x10u;
		v5 = enemy - 4;
		monster[v2]._menemy = v5;
		monster[v2]._menemyx = monster[v5]._mfutx;
		v4 = monster[v5]._mfuty;
	}
	else
	{
		monster[v2]._mFlags &= 0xFFFFFFEF;
		monster[v2]._menemy = enemy;
		v3 = enemy;
		monster[v2]._menemyx = plr[v3]._px;
		v4 = plr[v3]._py;
	}
	monster[v2]._menemyy = v4;
}
