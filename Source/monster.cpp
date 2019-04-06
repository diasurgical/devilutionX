//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

// Tracks which missile files are already loaded
int MissileFileFlag;

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

const char plr2monst[9] = { 0, 5, 3, 7, 1, 4, 6, 0, 2 };
const unsigned char counsmiss[4] = { MIS_FIREBOLT, MIS_CBOLT, MIS_LIGHTCTRL, MIS_FIREBALL };

/* data */

MonsterData monsterdata[112] = {
	// clang-format off
	// width, mImage, GraphicType,                       has_special, sndfile,                             snd_special, has_trans, TransFile,                         Frames[6],                  Rate[6],              mName,               mMinDLvl, mMaxDLvl, mLevel, mMinHP, mMaxHP, mAi,         mFlags,                                                              mInt, mHit, mAFNum, mMinDamage, mMaxDamage, mHit2, mAFNum2, mMinDamage2, mMaxDamage2, mArmorClass, mMonstClass, mMagicRes                                                    , mMagicRes2                                                   , mTreasure, mSelFlag,  mExp
	{    128,    799, "Monsters\\Zombie\\Zombie%c.CL2",  FALSE,       "Monsters\\Zombie\\Zombie%c%i.WAV",  FALSE,       FALSE,     NULL,                              { 11, 24, 12,  6, 16,  0 }, { 4, 0, 0, 0, 0, 0 }, "Zombie",                   1,        3,      1,      4,      7, AI_ZOMBIE,   0                                                                  ,    0,   10,      8,          2,          5,     0,       0,           0,           0,           5, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    54 },
	{    128,    799, "Monsters\\Zombie\\Zombie%c.CL2",  FALSE,       "Monsters\\Zombie\\Zombie%c%i.WAV",  FALSE,       TRUE,      "Monsters\\Zombie\\Bluered.TRN",   { 11, 24, 12,  6, 16,  0 }, { 4, 0, 0, 0, 0, 0 }, "Ghoul",                    2,        4,      2,      7,     11, AI_ZOMBIE,   0                                                                  ,    1,   10,      8,          3,         10,     0,       0,           0,           0,          10, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    58 },
	{    128,    799, "Monsters\\Zombie\\Zombie%c.CL2",  FALSE,       "Monsters\\Zombie\\Zombie%c%i.WAV",  FALSE,       TRUE,      "Monsters\\Zombie\\Grey.TRN",      { 11, 24, 12,  6, 16,  0 }, { 4, 0, 0, 0, 0, 0 }, "Rotting Carcass",          2,        6,      4,     15,     25, AI_ZOMBIE,   0                                                                  ,    2,   25,      8,          5,         15,     0,       0,           0,           0,          15, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,   136 },
	{    128,    799, "Monsters\\Zombie\\Zombie%c.CL2",  FALSE,       "Monsters\\Zombie\\Zombie%c%i.WAV",  FALSE,       TRUE,      "Monsters\\Zombie\\Yellow.TRN",    { 11, 24, 12,  6, 16,  0 }, { 4, 0, 0, 0, 0, 0 }, "Black Death",              4,        8,      6,     25,     40, AI_ZOMBIE,   0                                                                  ,    3,   30,      8,          6,         22,     0,       0,           0,           0,          20, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   240 },
	{    128,    543, "Monsters\\FalSpear\\Phall%c.CL2", TRUE,        "Monsters\\FalSpear\\Phall%c%i.WAV", TRUE,        TRUE,      "Monsters\\FalSpear\\FallenT.TRN", { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Fallen One",               1,        3,      1,      1,      4, AI_FALLEN,   0                                                                  ,    0,   15,      7,          1,          3,     0,       5,           0,           0,           0, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        3,    46 },
	{    128,    543, "Monsters\\FalSpear\\Phall%c.CL2", TRUE,        "Monsters\\FalSpear\\Phall%c%i.WAV", TRUE,        TRUE,      "Monsters\\FalSpear\\Dark.TRN",    { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Carver",                   2,        5,      3,      4,      8, AI_FALLEN,   0                                                                  ,    2,   20,      7,          2,          5,     0,       5,           0,           0,           5, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        3,    80 },
	{    128,    543, "Monsters\\FalSpear\\Phall%c.CL2", TRUE,        "Monsters\\FalSpear\\Phall%c%i.WAV", TRUE,        FALSE,     NULL,                              { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Devil Kin",                3,        7,      5,     12,     24, AI_FALLEN,   0                                                                  ,    2,   25,      7,          3,          7,     0,       5,           0,           0,          10, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        3,   155 },
	{    128,    543, "Monsters\\FalSpear\\Phall%c.CL2", TRUE,        "Monsters\\FalSpear\\Phall%c%i.WAV", TRUE,        TRUE,      "Monsters\\FalSpear\\Blue.TRN",    { 11, 11, 13, 11, 18, 13 }, { 3, 0, 0, 0, 0, 0 }, "Dark One",                 5,        9,      7,     20,     36, AI_FALLEN,   0                                                                  ,    3,   30,      7,          4,          8,     0,       5,           0,           0,          15, MC_ANIMAL,                                                   IMUNE_NULL_40,                              RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   255 },
	{    128,    553, "Monsters\\SkelAxe\\SklAx%c.CL2",  TRUE,        "Monsters\\SkelAxe\\SklAx%c%i.WAV",  FALSE,       TRUE,      "Monsters\\SkelAxe\\White.TRN",    { 12,  8, 13,  6, 17, 16 }, { 5, 0, 0, 0, 0, 0 }, "Skeleton",                 1,        3,      1,      2,      4, AI_SKELSD,   0                                                                  ,    0,   20,      8,          1,          4,     0,       0,           0,           0,           0, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    64 },
	{    128,    553, "Monsters\\SkelAxe\\SklAx%c.CL2",  TRUE,        "Monsters\\SkelAxe\\SklAx%c%i.WAV",  FALSE,       TRUE,      "Monsters\\SkelAxe\\Skelt.TRN",    { 12,  8, 13,  6, 17, 16 }, { 4, 0, 0, 0, 0, 0 }, "Corpse Axe",               2,        5,      2,      4,      7, AI_SKELSD,   0                                                                  ,    1,   25,      8,          3,          5,     0,       0,           0,           0,           0, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    68 },
	{    128,    553, "Monsters\\SkelAxe\\SklAx%c.CL2",  TRUE,        "Monsters\\SkelAxe\\SklAx%c%i.WAV",  FALSE,       FALSE,     NULL,                              { 12,  8, 13,  6, 17, 16 }, { 2, 0, 0, 0, 0, 0 }, "Burning Dead",             2,        6,      4,      8,     12, AI_SKELSD,   0                                                                  ,    2,   30,      8,          3,          7,     0,       0,           0,           0,           5, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        3,   154 },
	{    128,    553, "Monsters\\SkelAxe\\SklAx%c.CL2",  TRUE,        "Monsters\\SkelAxe\\SklAx%c%i.WAV",  FALSE,       TRUE,      "Monsters\\SkelAxe\\Black.TRN",    { 12,  8, 13,  6, 17, 16 }, { 3, 0, 0, 0, 0, 0 }, "Horror",                   4,        8,      6,     12,     20, AI_SKELSD,   0                                                                  ,    3,   35,      8,          4,          9,     0,       0,           0,           0,          15, MC_UNDEAD,   IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   264 },
	{    128,    623, "Monsters\\FalSword\\Fall%c.CL2",  TRUE,        "Monsters\\FalSword\\Fall%c%i.WAV",  TRUE,        TRUE,      "Monsters\\FalSword\\FallenT.TRN", { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Fallen One",               1,        3,      1,      2,      5, AI_FALLEN,   0                                                                  ,    0,   15,      8,          1,          4,     0,       5,           0,           0,          10, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        3,    52 },
	{    128,    623, "Monsters\\FalSword\\Fall%c.CL2",  TRUE,        "Monsters\\FalSword\\Fall%c%i.WAV",  TRUE,        TRUE,      "Monsters\\FalSword\\Dark.TRN",    { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Carver",                   2,        5,      3,      5,      9, AI_FALLEN,   0                                                                  ,    1,   20,      8,          2,          7,     0,       5,           0,           0,          15, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        3,    90 },
	{    128,    623, "Monsters\\FalSword\\Fall%c.CL2",  TRUE,        "Monsters\\FalSword\\Fall%c%i.WAV",  TRUE,        FALSE,     NULL,                              { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Devil Kin",                3,        7,      5,     16,     24, AI_FALLEN,   0                                                                  ,    2,   25,      8,          4,         10,     0,       5,           0,           0,          20, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        3,   180 },
	{    128,    623, "Monsters\\FalSword\\Fall%c.CL2",  TRUE,        "Monsters\\FalSword\\Fall%c%i.WAV",  TRUE,        TRUE,      "Monsters\\FalSword\\Blue.TRN",    { 12, 12, 13, 11, 14, 15 }, { 3, 0, 0, 0, 0, 0 }, "Dark One",                 5,        9,      7,     24,     36, AI_FALLEN,   0                                                                  ,    3,   30,      8,          4,         12,     0,       5,           0,           0,          25, MC_ANIMAL,                                                   IMUNE_NULL_40,                              RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   280 },
	{    128,    410, "Monsters\\Scav\\Scav%c.CL2",      TRUE,        "Monsters\\Scav\\Scav%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 12,  8, 12,  6, 20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Scavenger",                1,        4,      2,      3,      6, AI_SCAV,     0                                                                  ,    0,   20,      7,          1,          5,     0,       0,           0,           0,          10, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        3,    80 },
	{    128,    410, "Monsters\\Scav\\Scav%c.CL2",      TRUE,        "Monsters\\Scav\\Scav%c%i.WAV",      FALSE,       TRUE,      "Monsters\\Scav\\ScavBr.TRN",      { 12,  8, 12,  6, 20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Plague Eater",             3,        6,      4,     12,     24, AI_SCAV,     0                                                                  ,    1,   30,      7,          1,          8,     0,       0,           0,           0,          20, MC_ANIMAL,   0                                                            ,                              RESIST_LIGHTNING                ,         0,        3,   188 },
	{    128,    410, "Monsters\\Scav\\Scav%c.CL2",      TRUE,        "Monsters\\Scav\\Scav%c%i.WAV",      FALSE,       TRUE,      "Monsters\\Scav\\ScavBe.TRN",      { 12,  8, 12,  6, 20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Shadow Beast",             4,        8,      6,     24,     36, AI_SCAV,     0                                                                  ,    2,   35,      7,          3,         12,     0,       0,           0,           0,          25, MC_ANIMAL,                                                   IMUNE_NULL_40,                RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,   375 },
	{    128,    410, "Monsters\\Scav\\Scav%c.CL2",      TRUE,        "Monsters\\Scav\\Scav%c%i.WAV",      FALSE,       TRUE,      "Monsters\\Scav\\ScavW.TRN",       { 12,  8, 12,  6, 20, 11 }, { 2, 0, 0, 0, 0, 0 }, "Bone Gasher",              6,       10,      8,     28,     40, AI_SCAV,     0                                                                  ,    3,   35,      7,          5,         15,     0,       0,           0,           0,          30, MC_ANIMAL,   RESIST_MAGIC |                                  IMUNE_NULL_40,                              RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   552 },
	{    128,    567, "Monsters\\SkelBow\\SklBw%c.CL2",  TRUE,        "Monsters\\SkelBow\\SklBw%c%i.WAV",  FALSE,       TRUE,      "Monsters\\SkelBow\\White.TRN",    {  9,  8, 16,  5, 16, 16 }, { 4, 0, 0, 0, 0, 0 }, "Skeleton",                 2,        5,      3,      2,      4, AI_SKELBOW,  0                                                                  ,    0,   15,     12,          1,          2,     0,       0,           0,           0,           0, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   110 },
	{    128,    567, "Monsters\\SkelBow\\SklBw%c.CL2",  TRUE,        "Monsters\\SkelBow\\SklBw%c%i.WAV",  FALSE,       TRUE,      "Monsters\\SkelBow\\Skelt.TRN",    {  9,  8, 16,  5, 16, 16 }, { 4, 0, 0, 0, 0, 0 }, "Corpse Bow",               3,        7,      5,      8,     16, AI_SKELBOW,  0                                                                  ,    1,   25,     12,          1,          4,     0,       0,           0,           0,           0, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   210 },
	{    128,    567, "Monsters\\SkelBow\\SklBw%c.CL2",  TRUE,        "Monsters\\SkelBow\\SklBw%c%i.WAV",  FALSE,       FALSE,     NULL,                              {  9,  8, 16,  5, 16, 16 }, { 2, 0, 0, 0, 0, 0 }, "Burning Dead",             5,        9,      7,     10,     24, AI_SKELBOW,  0                                                                  ,    2,   30,     12,          1,          6,     0,       0,           0,           0,           5, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        3,   364 },
	{    128,    567, "Monsters\\SkelBow\\SklBw%c.CL2",  TRUE,        "Monsters\\SkelBow\\SklBw%c%i.WAV",  FALSE,       TRUE,      "Monsters\\SkelBow\\Black.TRN",    {  9,  8, 16,  5, 16, 16 }, { 3, 0, 0, 0, 0, 0 }, "Horror",                   7,       11,      9,     15,     45, AI_SKELBOW,  0                                                                  ,    3,   35,     12,          2,          9,     0,       0,           0,           0,          15, MC_UNDEAD,   IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   594 },
	{    128,    575, "Monsters\\SkelSd\\SklSr%c.CL2",   TRUE,        "Monsters\\SkelSd\\SklSr%c%i.WAV",   TRUE,        TRUE,      "Monsters\\SkelSd\\White.TRN",     { 13,  8, 12,  7, 15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Skeleton Captain",         1,        4,      2,      3,      6, AI_SKELSD,   0                                                                  ,    0,   20,      8,          2,          7,     0,       0,           0,           0,          10, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,    90 },
	{    128,    575, "Monsters\\SkelSd\\SklSr%c.CL2",   TRUE,        "Monsters\\SkelSd\\SklSr%c%i.WAV",   FALSE,       TRUE,      "Monsters\\SkelSd\\Skelt.TRN",     { 13,  8, 12,  7, 15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Corpse Captain",           2,        6,      4,     12,     20, AI_SKELSD,   0                                                                  ,    1,   30,      8,          3,          9,     0,       0,           0,           0,           5, MC_UNDEAD,   IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   200 },
	{    128,    575, "Monsters\\SkelSd\\SklSr%c.CL2",   TRUE,        "Monsters\\SkelSd\\SklSr%c%i.WAV",   FALSE,       FALSE,     NULL,                              { 13,  8, 12,  7, 15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Burning Dead Captain",     4,        8,      6,     16,     30, AI_SKELSD,   0                                                                  ,    2,   35,      8,          4,         10,     0,       0,           0,           0,          15, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        3,   393 },
	{    128,    575, "Monsters\\SkelSd\\SklSr%c.CL2",   TRUE,        "Monsters\\SkelSd\\SklSr%c%i.WAV",   FALSE,       TRUE,      "Monsters\\SkelSd\\Black.TRN",     { 13,  8, 12,  7, 15, 16 }, { 4, 0, 0, 0, 0, 0 }, "Horror Captain",           6,       10,      8,     35,     50, AI_SKELSD,                                    MFLAG_SEARCH                      ,    3,   40,      8,          5,         14,     0,       0,           0,           0,          30, MC_UNDEAD,   IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,   604 },
	{    128,   2000, "Monsters\\TSneak\\TSneak%c.CL2",  FALSE,       "Monsters\\TSneak\\Sneakl%c%i.WAV",  FALSE,       FALSE,     NULL,                              { 13, 13, 15, 11, 16,  0 }, { 2, 0, 0, 0, 0, 0 }, "Invisible Lord",          14,       14,     14,    278,    278, AI_SKELSD,                                    MFLAG_SEARCH                      ,    3,   65,      8,         16,         30,     0,       0,           0,           0,          60, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,  2000 },
	{    128,    992, "Monsters\\Sneak\\Sneak%c.CL2",    TRUE,        "Monsters\\Sneak\\Sneak%c%i.WAV",    FALSE,       FALSE,     NULL,                              { 16,  8, 12,  8, 24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Hidden",                   3,        8,      5,      8,     24, AI_SNEAK,    MFLAG_HIDDEN                                                       ,    0,   35,      8,          3,          6,     0,       0,           0,           0,          25, MC_DEMON,    0                                                            ,                                                 IMUNE_NULL_40,         0,        3,   278 },
	{    128,    992, "Monsters\\Sneak\\Sneak%c.CL2",    TRUE,        "Monsters\\Sneak\\Sneak%c%i.WAV",    FALSE,       TRUE,      "Monsters\\Sneak\\Sneakv2.TRN",    { 16,  8, 12,  8, 24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Stalker",                  8,       12,      9,     30,     45, AI_SNEAK,    MFLAG_HIDDEN |                   MFLAG_SEARCH                      ,    1,   40,      8,          8,         16,     0,       0,           0,           0,          30, MC_DEMON,    0                                                            ,                                                 IMUNE_NULL_40,         0,        3,   630 },
	{    128,    992, "Monsters\\Sneak\\Sneak%c.CL2",    TRUE,        "Monsters\\Sneak\\Sneak%c%i.WAV",    FALSE,       TRUE,      "Monsters\\Sneak\\Sneakv3.TRN",    { 16,  8, 12,  8, 24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Unseen",                  10,       14,     11,     35,     50, AI_SNEAK,    MFLAG_HIDDEN |                   MFLAG_SEARCH                      ,    2,   45,      8,         12,         20,     0,       0,           0,           0,          30, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   935 },
	{    128,    992, "Monsters\\Sneak\\Sneak%c.CL2",    TRUE,        "Monsters\\Sneak\\Sneak%c%i.WAV",    FALSE,       TRUE,      "Monsters\\Sneak\\Sneakv1.TRN",    { 16,  8, 12,  8, 24, 15 }, { 2, 0, 0, 0, 0, 0 }, "Illusion Weaver",         14,       18,     13,     40,     60, AI_SNEAK,    MFLAG_HIDDEN |                   MFLAG_SEARCH                      ,    3,   60,      8,         16,         24,     0,       0,           0,           0,          30, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE                                   , IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,  1500 },
	{    160,   2000, "Monsters\\GoatLord\\GoatL%c.CL2", FALSE,       "Monsters\\GoatLord\\Goatl%c%i.WAV", FALSE,       FALSE,     NULL,                              { 13, 13, 14,  9, 16,  0 }, { 2, 0, 0, 0, 0, 0 }, "Lord Sayter",             13,       13,     12,    351,    351, AI_SKELSD,                                    MFLAG_SEARCH                      ,    3,   80,      8,         14,         24,     0,       0,           0,           0,          60, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40, RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,  1500 },
	{    128,   1030, "Monsters\\GoatMace\\Goat%c.CL2",  TRUE,        "Monsters\\GoatMace\\Goat%c%i.WAV",  FALSE,       FALSE,     NULL,                              { 12,  8, 12,  6, 20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Flesh Clan",               6,       10,      8,     30,     45, AI_GOATMC,                                    MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    0,   50,      8,          4,         10,     0,       0,           0,           0,          40, MC_DEMON,    0                                                            , 0                                                            ,         0,        3,   460 },
	{    128,   1030, "Monsters\\GoatMace\\Goat%c.CL2",  TRUE,        "Monsters\\GoatMace\\Goat%c%i.WAV",  FALSE,       TRUE,      "Monsters\\GoatMace\\Beige.TRN",   { 12,  8, 12,  6, 20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Stone Clan",               8,       12,     10,     40,     55, AI_GOATMC,                                    MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    1,   60,      8,          6,         12,     0,       0,           0,           0,          40, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   685 },
	{    128,   1030, "Monsters\\GoatMace\\Goat%c.CL2",  TRUE,        "Monsters\\GoatMace\\Goat%c%i.WAV",  FALSE,       TRUE,      "Monsters\\GoatMace\\Red.TRN",     { 12,  8, 12,  6, 20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Fire Clan",               10,       14,     12,     50,     65, AI_GOATMC,                                    MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    2,   70,      8,          8,         16,     0,       0,           0,           0,          45, MC_DEMON,                   RESIST_FIRE                                   ,                IMUNE_FIRE                                    ,         0,        3,   906 },
	{    128,   1030, "Monsters\\GoatMace\\Goat%c.CL2",  TRUE,        "Monsters\\GoatMace\\Goat%c%i.WAV",  FALSE,       TRUE,      "Monsters\\GoatMace\\Gray.TRN",    { 12,  8, 12,  6, 20, 12 }, { 2, 0, 0, 0, 1, 0 }, "Night Clan",              12,       16,     14,     55,     70, AI_GOATMC,                                    MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,   80,      8,         10,         20,    15,       0,          30,          30,          50, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,  1190 },
	{     96,    364, "Monsters\\Bat\\Bat%c.CL2",        FALSE,       "Monsters\\Bat\\Bat%c%i.WAV",        FALSE,       TRUE,      "Monsters\\Bat\\red.trn",          {  9, 13, 10,  9, 13,  0 }, { 0, 0, 0, 0, 0, 0 }, "Fiend",                    2,        5,      3,      3,      6, AI_BAT,      0                                                                  ,    0,   35,      5,          1,          6,     0,       0,           0,           0,           0, MC_ANIMAL,   0                                                            , 0                                                            ,    0x4000,        6,   102 },
	{     96,    364, "Monsters\\Bat\\Bat%c.CL2",        FALSE,       "Monsters\\Bat\\Bat%c%i.WAV",        FALSE,       FALSE,     NULL,                              {  9, 13, 10,  9, 13,  0 }, { 0, 0, 0, 0, 0, 0 }, "Blink",                    5,        9,      7,     12,     28, AI_BAT,      0                                                                  ,    1,   45,      5,          1,          8,     0,       0,           0,           0,          15, MC_ANIMAL,   0                                                            , 0                                                            ,    0x4000,        6,   340 },
	{     96,    364, "Monsters\\Bat\\Bat%c.CL2",        FALSE,       "Monsters\\Bat\\Bat%c%i.WAV",        FALSE,       TRUE,      "Monsters\\Bat\\grey.trn",         {  9, 13, 10,  9, 13,  0 }, { 0, 0, 0, 0, 0, 0 }, "Gloom",                    7,       11,      9,     28,     36, AI_BAT,                                       MFLAG_SEARCH                      ,    2,   70,      5,          4,         12,     0,       0,           0,           0,          35, MC_ANIMAL,   RESIST_MAGIC                                                 , RESIST_MAGIC |                                  IMUNE_NULL_40,    0x4000,        6,   509 },
	{     96,    364, "Monsters\\Bat\\Bat%c.CL2",        FALSE,       "Monsters\\Bat\\Bat%c%i.WAV",        FALSE,       TRUE,      "Monsters\\Bat\\orange.trn",       {  9, 13, 10,  9, 13,  0 }, { 0, 0, 0, 0, 0, 0 }, "Familiar",                11,       15,     13,     20,     35, AI_BAT,                                       MFLAG_SEARCH                      ,    3,   50,      5,          4,         16,     0,       0,           0,           0,          35, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING                 , RESIST_MAGIC |               IMUNE_LIGHTNING  | IMUNE_NULL_40,    0x4000,        6,   448 },
	{    128,   1040, "Monsters\\GoatBow\\GoatB%c.CL2",  FALSE,       "Monsters\\GoatBow\\GoatB%c%i.WAV",  FALSE,       FALSE,     NULL,                              { 12,  8, 16,  6, 20,  0 }, { 3, 0, 0, 0, 0, 0 }, "Flesh Clan",               6,       10,      8,     20,     35, AI_GOATBOW,                                                  MFLAG_CAN_OPEN_DOOR,    0,   35,     13,          1,          7,     0,       0,           0,           0,          35, MC_DEMON,    0                                                            , 0                                                            ,         0,        3,   448 },
	{    128,   1040, "Monsters\\GoatBow\\GoatB%c.CL2",  FALSE,       "Monsters\\GoatBow\\GoatB%c%i.WAV",  FALSE,       TRUE,      "Monsters\\GoatBow\\Beige.TRN",    { 12,  8, 16,  6, 20,  0 }, { 3, 0, 0, 0, 0, 0 }, "Stone Clan",               8,       12,     10,     30,     40, AI_GOATBOW,                                                  MFLAG_CAN_OPEN_DOOR,    1,   40,     13,          2,          9,     0,       0,           0,           0,          35, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,   645 },
	{    128,   1040, "Monsters\\GoatBow\\GoatB%c.CL2",  FALSE,       "Monsters\\GoatBow\\GoatB%c%i.WAV",  FALSE,       TRUE,      "Monsters\\GoatBow\\Red.TRN",      { 12,  8, 16,  6, 20,  0 }, { 3, 0, 0, 0, 0, 0 }, "Fire Clan",               10,       14,     12,     40,     50, AI_GOATBOW,                                   MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    2,   45,     13,          3,         11,     0,       0,           0,           0,          35, MC_DEMON,                   RESIST_FIRE                                   ,                IMUNE_FIRE                                    ,         0,        3,   822 },
	{    128,   1040, "Monsters\\GoatBow\\GoatB%c.CL2",  FALSE,       "Monsters\\GoatBow\\GoatB%c%i.WAV",  FALSE,       TRUE,      "Monsters\\GoatBow\\Gray.TRN",     { 12,  8, 16,  6, 20,  0 }, { 3, 0, 0, 0, 0, 0 }, "Night Clan",              12,       16,     14,     50,     65, AI_GOATBOW,                                   MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,   50,     13,          4,         13,    15,       0,           0,           0,          40, MC_DEMON,    RESIST_MAGIC |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        3,  1092 },
	{    128,    716, "Monsters\\Acid\\Acid%c.CL2",      TRUE,        "Monsters\\Acid\\Acid%c%i.WAV",      TRUE,        FALSE,     NULL,                              { 13,  8, 12,  8, 16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Acid Beast",              10,       14,     11,     40,     66, AI_ACID,     0                                                                  ,    0,   40,      8,          4,         12,    25,       8,           0,           0,          30, MC_ANIMAL,                                                   IMUNE_ACID   , IMUNE_MAGIC  |                                  IMUNE_ACID   ,         0,        3,   846 },
	{    128,    716, "Monsters\\Acid\\Acid%c.CL2",      TRUE,        "Monsters\\Acid\\Acid%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Acid\\AcidBlk.TRN",     { 13,  8, 12,  8, 16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Poison Spitter",          14,       18,     15,     60,     85, AI_ACID,     0                                                                  ,    1,   45,      8,          4,         16,    25,       8,           0,           0,          30, MC_ANIMAL,                                                   IMUNE_ACID   , IMUNE_MAGIC  |                                  IMUNE_ACID   ,         0,        3,  1248 },
	{    128,    716, "Monsters\\Acid\\Acid%c.CL2",      TRUE,        "Monsters\\Acid\\Acid%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Acid\\AcidB.TRN",       { 13,  8, 12,  8, 16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Pit Beast",               18,       22,     21,     80,    110, AI_ACID,     0                                                                  ,    2,   55,      8,          8,         18,    35,       8,           0,           0,          35, MC_ANIMAL,   RESIST_MAGIC |                                  IMUNE_ACID   , IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_ACID   ,         0,        3,  2060 },
	{    128,    716, "Monsters\\Acid\\Acid%c.CL2",      TRUE,        "Monsters\\Acid\\Acid%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Acid\\AcidR.TRN",       { 13,  8, 12,  8, 16, 12 }, { 0, 0, 0, 0, 0, 0 }, "Lava Maw",                22,       27,     25,    100,    150, AI_ACID,     0                                                                  ,    3,   65,      8,         10,         20,    40,       8,           0,           0,          35, MC_ANIMAL,   RESIST_MAGIC | IMUNE_FIRE  |                    IMUNE_ACID   , IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_ACID   ,         0,        3,  2940 },
	{    160,   1010, "Monsters\\SKing\\SKing%c.CL2",    TRUE,        "Monsters\\SKing\\SKing%c%i.WAV",    TRUE,        TRUE,      "Monsters\\SkelAxe\\White.TRN",    {  8,  6, 16,  6, 16,  6 }, { 2, 0, 0, 0, 0, 2 }, "Skeleton King",            6,        6,      9,    140,    140, AI_SKELKING,                                  MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,   60,      8,          6,         16,     0,       0,           0,           0,          70, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,    0x8001,        7,   570 },
	{    128,    980, "Monsters\\FatC\\FatC%c.CL2",      FALSE,       "Monsters\\FatC\\FatC%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 10,  8, 12,  6, 16,  0 }, { 1, 0, 0, 0, 0, 0 }, "The Butcher",              0,        0,      1,    320,    320, AI_CLEAVER,  0                                                                  ,    3,   50,      8,          6,         12,     0,       0,           0,           0,          50, MC_DEMON,                   RESIST_FIRE | RESIST_LIGHTNING                , RESIST_MAGIC | IMUNE_FIRE  | IMUNE_LIGHTNING                 ,    0x8000,        3,   710 },
	{    128,   1130, "Monsters\\Fat\\Fat%c.CL2",        TRUE,        "Monsters\\Fat\\Fat%c%i.WAV",        FALSE,       FALSE,     NULL,                              {  8, 10, 15,  6, 16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Overlord",                 8,       12,     10,     60,     80, AI_FAT,      0                                                                  ,    0,   55,      8,          6,         12,     0,       0,           0,           0,          55, MC_DEMON,    0                                                            ,                RESIST_FIRE                                   ,         0,        3,   635 },
	{    128,   1130, "Monsters\\Fat\\Fat%c.CL2",        TRUE,        "Monsters\\Fat\\Fat%c%i.WAV",        FALSE,       TRUE,      "Monsters\\Fat\\Blue.TRN",         {  8, 10, 15,  6, 16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Mud Man",                 13,       17,     14,    100,    125, AI_FAT,                                       MFLAG_SEARCH                      ,    1,   60,      8,          8,         16,     0,       0,           0,           0,          60, MC_DEMON,    0                                                            ,                              IMUNE_LIGHTNING                 ,         0,        3,  1165 },
	{    128,   1130, "Monsters\\Fat\\Fat%c.CL2",        TRUE,        "Monsters\\Fat\\Fat%c%i.WAV",        FALSE,       TRUE,      "Monsters\\Fat\\FatB.TRN",         {  8, 10, 15,  6, 16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Toad Demon",              15,       19,     16,    135,    160, AI_FAT,                                       MFLAG_SEARCH                      ,    2,   70,      8,          8,         16,    40,       0,           8,          20,          65, MC_DEMON,    IMUNE_MAGIC                                                  , IMUNE_MAGIC  |               RESIST_LIGHTNING                ,         0,        3,  1380 },
	{    128,   1130, "Monsters\\Fat\\Fat%c.CL2",        TRUE,        "Monsters\\Fat\\Fat%c%i.WAV",        FALSE,       TRUE,      "Monsters\\Fat\\FatF.TRN",         {  8, 10, 15,  6, 16, 10 }, { 4, 0, 0, 0, 0, 0 }, "Flayed One",              19,       23,     20,    160,    200, AI_FAT,                                       MFLAG_SEARCH                      ,    3,   85,      8,         10,         20,     0,       0,           0,           0,          70, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        3,  2058 },
	{    160,   2420, "Monsters\\Worm\\Worm%c.CL2",      FALSE,       "Monsters\\Fat\\Fat%c%i.WAV",        FALSE,       FALSE,     NULL,                              { 13, 13, 13, 11, 19,  0 }, { 0, 0, 0, 0, 0, 0 }, "Wyrm",                     9,       13,     11,     60,     90, AI_SKELSD,   0                                                                  ,    0,   40,      8,          4,         10,     0,       0,           0,           0,          25, MC_ANIMAL,   RESIST_MAGIC                                                 , RESIST_MAGIC                                                 ,         0,        3,   660 },
	{    160,   2420, "Monsters\\Worm\\Worm%c.CL2",      FALSE,       "Monsters\\Fat\\Fat%c%i.WAV",        FALSE,       FALSE,     NULL,                              { 13, 13, 13, 11, 19,  0 }, { 0, 0, 0, 0, 0, 0 }, "Cave Slug",               11,       15,     13,     75,    110, AI_SKELSD,   0                                                                  ,    1,   50,      8,          6,         13,     0,       0,           0,           0,          30, MC_ANIMAL,   RESIST_MAGIC                                                 , RESIST_MAGIC                                                 ,         0,        3,   994 },
	{    160,   2420, "Monsters\\Worm\\Worm%c.CL2",      FALSE,       "Monsters\\Fat\\Fat%c%i.WAV",        FALSE,       FALSE,     NULL,                              { 13, 13, 13, 11, 19,  0 }, { 0, 0, 0, 0, 0, 0 }, "Devil Wyrm",              13,       17,     15,    100,    140, AI_SKELSD,   0                                                                  ,    2,   55,      8,          8,         16,     0,       0,           0,           0,          30, MC_ANIMAL,   RESIST_MAGIC | RESIST_FIRE                                   , RESIST_MAGIC | RESIST_FIRE                                   ,         0,        3,  1320 },
	{    160,   2420, "Monsters\\Worm\\Worm%c.CL2",      FALSE,       "Monsters\\Fat\\Fat%c%i.WAV",        FALSE,       FALSE,     NULL,                              { 13, 13, 13, 11, 19,  0 }, { 0, 0, 0, 0, 0, 0 }, "Devourer",                15,       19,     17,    125,    200, AI_SKELSD,   0                                                                  ,    3,   60,      8,         10,         20,     0,       0,           0,           0,          35, MC_ANIMAL,   RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40, RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40,         0,        3,  1827 },
	{    128,   1680, "Monsters\\Magma\\Magma%c.CL2",    TRUE,        "Monsters\\Magma\\Magma%c%i.WAV",    TRUE,        FALSE,     NULL,                              {  8, 10, 14,  7, 18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Magma Demon",             14,       17,     13,     50,     70, AI_MAGMA,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    0,   45,      4,          2,         10,    50,      13,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE                                   , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  1076 },
	{    128,   1680, "Monsters\\Magma\\Magma%c.CL2",    TRUE,        "Monsters\\Magma\\Magma%c%i.WAV",    TRUE,        TRUE,      "Monsters\\Magma\\Yellow.TRN",     {  8, 10, 14,  7, 18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Blood Stone",             15,       19,     14,     55,     75, AI_MAGMA,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    1,   50,      4,          2,         12,    50,      14,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  1309 },
	{    128,   1680, "Monsters\\Magma\\Magma%c.CL2",    TRUE,        "Monsters\\Magma\\Magma%c%i.WAV",    TRUE,        TRUE,      "Monsters\\Magma\\Blue.TRN",       {  8, 10, 14,  7, 18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Hell Stone",              16,       20,     16,     60,     80, AI_MAGMA,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    2,   60,      4,          2,         20,    60,      14,           0,           0,          50, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  1680 },
	{    128,   1680, "Monsters\\Magma\\Magma%c.CL2",    TRUE,        "Monsters\\Magma\\Magma%c%i.WAV",    TRUE,        TRUE,      "Monsters\\Magma\\Wierd.TRN",      {  8, 10, 14,  7, 18, 18 }, { 2, 0, 0, 0, 1, 0 }, "Lava Lord",               17,       21,     18,     70,     85, AI_MAGMA,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,   75,      4,          4,         24,    60,      14,           0,           0,          60, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  2124 },
	{    160,   1630, "Monsters\\Rhino\\Rhino%c.CL2",    TRUE,        "Monsters\\Rhino\\Rhino%c%i.WAV",    TRUE,        FALSE,     NULL,                              {  8,  8, 14,  6, 16,  6 }, { 2, 0, 0, 0, 0, 0 }, "Horned Demon",            12,       16,     13,     40,     80, AI_RHINO,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    0,   60,      7,          2,         16,   100,       0,           5,          32,          40, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        7,  1172 },
	{    160,   1630, "Monsters\\Rhino\\Rhino%c.CL2",    TRUE,        "Monsters\\Rhino\\Rhino%c%i.WAV",    TRUE,        TRUE,      "Monsters\\Rhino\\Orange.TRN",     {  8,  8, 14,  6, 16,  6 }, { 2, 0, 0, 0, 0, 0 }, "Mud Runner",              14,       18,     15,     50,     90, AI_RHINO,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    1,   70,      7,          6,         18,   100,       0,          12,          36,          45, MC_ANIMAL,   0                                                            ,                RESIST_FIRE                                   ,         0,        7,  1404 },
	{    160,   1630, "Monsters\\Rhino\\Rhino%c.CL2",    TRUE,        "Monsters\\Rhino\\Rhino%c%i.WAV",    TRUE,        TRUE,      "Monsters\\Rhino\\Blue.TRN",       {  8,  8, 14,  6, 16,  6 }, { 2, 0, 0, 0, 0, 0 }, "Frost Charger",           16,       20,     17,     60,    100, AI_RHINO,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    2,   80,      7,          8,         20,   100,       0,          20,          40,          50, MC_ANIMAL,   IMUNE_MAGIC  |               RESIST_LIGHTNING                , IMUNE_MAGIC  |               RESIST_LIGHTNING                ,         0,        7,  1720 },
	{    160,   1630, "Monsters\\Rhino\\Rhino%c.CL2",    TRUE,        "Monsters\\Rhino\\Rhino%c%i.WAV",    TRUE,        TRUE,      "Monsters\\Rhino\\RhinoB.TRN",     {  8,  8, 14,  6, 16,  6 }, { 2, 0, 0, 0, 0, 0 }, "Obsidian Lord",           18,       22,     19,     70,    110, AI_RHINO,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,   90,      7,         10,         22,   100,       0,          20,          50,          55, MC_ANIMAL,   IMUNE_MAGIC  |               RESIST_LIGHTNING                , IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING                 ,         0,        7,  1809 },
	{    128,   1740, "Monsters\\Demskel\\Demskl%c.CL2", TRUE,        "Monsters\\Thin\\Thin%c%i.WAV",      TRUE,        FALSE,     "Monsters\\Thin\\Thinv3.TRN",      { 10,  8, 20,  6, 24, 16 }, { 3, 0, 0, 0, 0, 0 }, "Bone Demon",              10,       14,     12,     70,     70, AI_STORM,    0                                                                  ,    0,   60,      8,          6,         14,    12,       0,           0,           0,          50, MC_DEMON,    IMUNE_MAGIC  |                                  IMUNE_NULL_40, IMUNE_MAGIC  |                                  IMUNE_NULL_40,         0,        7,  1344 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      TRUE,        "Monsters\\Thin\\Thin%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Thin\\Thinv3.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Red Death",               14,       18,     16,     96,     96, AI_STORM,    0                                                                  ,    1,   75,      5,         10,         20,     0,       0,           0,           0,          60, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  2168 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      TRUE,        "Monsters\\Thin\\Thin%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Thin\\Thinv3.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Litch Demon",             16,       20,     18,    110,    110, AI_STORM,    0                                                                  ,    2,   80,      5,         10,         24,     0,       0,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  2736 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      TRUE,        "Monsters\\Thin\\Thin%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Thin\\Thinv3.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Undead Balrog",           20,       24,     22,    130,    130, AI_STORM,    0                                                                  ,    3,   85,      5,         12,         30,     0,       0,           0,           0,          65, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  3575 },
	{    128,   1460, "Monsters\\Fireman\\FireM%c.CL2",  TRUE,        "Monsters\\Acid\\Acid%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 14, 19, 20,  8, 14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Incinerator",             14,       18,     16,     30,     45, AI_FIREMAN,  0                                                                  ,    0,   75,      8,          8,         16,     0,       0,           0,           0,          25, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        3,  1888 },
	{    128,   1460, "Monsters\\Fireman\\FireM%c.CL2",  TRUE,        "Monsters\\Acid\\Acid%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 14, 19, 20,  8, 14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Flame Lord",              16,       20,     18,     40,     55, AI_FIREMAN,  0                                                                  ,    1,   75,      8,         10,         20,     0,       0,           0,           0,          25, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE                                    , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        3,  2250 },
	{    128,   1460, "Monsters\\Fireman\\FireM%c.CL2",  TRUE,        "Monsters\\Acid\\Acid%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 14, 19, 20,  8, 14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Doom Fire",               18,       22,     20,     50,     65, AI_FIREMAN,  0                                                                  ,    2,   80,      8,         12,         24,     0,       0,           0,           0,          30, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                , IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,         0,        3,  2740 },
	{    128,   1460, "Monsters\\Fireman\\FireM%c.CL2",  TRUE,        "Monsters\\Acid\\Acid%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 14, 19, 20,  8, 14, 23 }, { 0, 0, 0, 0, 0, 0 }, "Hell Burner",             20,       24,     22,     60,     80, AI_FIREMAN,  0                                                                  ,    3,   85,      8,         15,         30,     0,       0,           0,           0,          30, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                , IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,         0,        3,  3355 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      TRUE,        "Monsters\\Thin\\Thin%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Thin\\Thinv3.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Red Storm",               17,       21,     18,     55,    110, AI_STORM,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    0,   80,      5,          8,         18,    75,       8,           4,          16,          30, MC_DEMON,    IMUNE_MAGIC  |               RESIST_LIGHTNING                , IMUNE_MAGIC  |               IMUNE_LIGHTNING                 ,         0,        7,  2160 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      TRUE,        "Monsters\\Thin\\Thin%c%i.WAV",      TRUE,        FALSE,     NULL,                              {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Storm Rider",             19,       23,     20,     60,    120, AI_STORM,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    1,   80,      5,          8,         18,    80,       8,           4,          16,          30, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING                 , IMUNE_MAGIC  |               IMUNE_LIGHTNING                 ,         0,        7,  2391 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      TRUE,        "Monsters\\Thin\\Thin%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Thin\\Thinv2.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Storm Lord",              21,       25,     22,     75,    135, AI_STORM,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    2,   85,      5,         12,         24,    75,       8,           4,          16,          35, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING                 , IMUNE_MAGIC  |               IMUNE_LIGHTNING                 ,         0,        7,  2775 },
	{    160,   1740, "Monsters\\Thin\\Thin%c.CL2",      TRUE,        "Monsters\\Thin\\Thin%c%i.WAV",      TRUE ,       TRUE,      "Monsters\\Thin\\Thinv1.TRN",      {  8,  8, 18,  4, 17, 14 }, { 3, 0, 0, 0, 0, 0 }, "Maelstorm",               23,       27,     24,     90,    150, AI_STORM,                                     MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,   90,      5,         12,         28,    75,       8,           4,          16,          40, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  3177 },
	{    128,   1650, "Monsters\\BigFall\\Fallg%c.CL2",  TRUE,        "Monsters\\BigFall\\Bfal%c%i.WAV",   FALSE,       FALSE,     NULL,                              { 10,  8, 11,  8, 17,  0 }, { 0, 0, 0, 0, 2, 2 }, "Devil Kin Brute",         20,       20,     24,    160,    220, AI_SKELSD,                                    MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,  100,      6,         18,         24,     0,       0,           0,           0,          75, MC_ANIMAL,   0                                                            , 0                                                            ,         0,        6,  2000 },
	{    160,   1650, "Monsters\\Gargoyle\\Gargo%c.CL2", TRUE,        "Monsters\\Gargoyle\\Gargo%c%i.WAV", FALSE,       FALSE,     NULL,                              { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 2 }, "Winged-Demon",             8,       12,      9,     45,     60, AI_GARG,                                                     MFLAG_CAN_OPEN_DOOR,    0,   50,       7,        10,         16,     0,       0,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        6,   662 },
	{    160,   1650, "Monsters\\Gargoyle\\Gargo%c.CL2", TRUE,        "Monsters\\Gargoyle\\Gargo%c%i.WAV", FALSE,       TRUE,      "Monsters\\Gargoyle\\GarE.TRN",    { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 2 }, "Gargoyle",                12,       16,     13,     60,     90, AI_GARG,                                                     MFLAG_CAN_OPEN_DOOR,    1,   65,       7,        10,         16,     0,       0,           0,           0,          45, MC_DEMON,    IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        6,  1205 },
	{    160,   1650, "Monsters\\Gargoyle\\Gargo%c.CL2", TRUE,        "Monsters\\Gargoyle\\Gargo%c%i.WAV", FALSE,       TRUE,      "Monsters\\Gargoyle\\GargBr.TRN",  { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 0 }, "Blood Claw",              16,       20,     19,     75,    125, AI_GARG,                                                     MFLAG_CAN_OPEN_DOOR,    2,   80,       7,        14,         22,     0,       0,           0,           0,          50, MC_DEMON,    IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        6,  1873 },
	{    160,   1650, "Monsters\\Gargoyle\\Gargo%c.CL2", TRUE,        "Monsters\\Gargoyle\\Gargo%c%i.WAV", FALSE,       TRUE,      "Monsters\\Gargoyle\\GargB.TRN",   { 14, 14, 14, 10, 18, 14 }, { 0, 0, 0, 0, 0, 0 }, "Death Wing",              18,       22,     23,     90,    150, AI_GARG,                                                     MFLAG_CAN_OPEN_DOOR,    3,   95,       7,        16,         28,     0,       0,           0,           0,          60, MC_DEMON,    IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        6,  2278 },
	{    160,   2220, "Monsters\\Mega\\Mega%c.CL2",      TRUE,        "Monsters\\Mega\\Mega%c%i.WAV",      TRUE,        FALSE,     NULL,                              {  6,  7, 14,  1, 24,  5 }, { 3, 0, 0, 0, 2, 0 }, "Slayer",                  19,       23,     20,    120,    140, AI_MEGA,                                      MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    0,  100,      8,         12,         20,     0,       3,           0,           0,          60, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE                                    , RESIST_MAGIC | IMUNE_FIRE                                    ,         0,        7,  2300 },
	{    160,   2220, "Monsters\\Mega\\Mega%c.CL2",      TRUE,        "Monsters\\Mega\\Mega%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Mega\\Guard.TRN",       {  6,  7, 14,  1, 24,  5 }, { 3, 0, 0, 0, 2, 0 }, "Guardian",                21,       25,     22,    140,    160, AI_MEGA,                                      MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    1,  110,      8,         14,         22,     0,       3,           0,           0,          65, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE                                    , RESIST_MAGIC | IMUNE_FIRE                                    ,         0,        7,  2714 },
	{    160,   2220, "Monsters\\Mega\\Mega%c.CL2",      TRUE,        "Monsters\\Mega\\Mega%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Mega\\Vtexl.TRN",       {  6,  7, 14,  1, 24,  5 }, { 3, 0, 0, 0, 2, 0 }, "Vortex Lord",             23,       26,     24,    160,    180, AI_MEGA,                                      MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    2,  120,      8,         18,         24,     0,       3,           0,           0,          70, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  |                    IMUNE_NULL_40, RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  3252 },
	{    160,   2220, "Monsters\\Mega\\Mega%c.CL2",      TRUE,        "Monsters\\Mega\\Mega%c%i.WAV",      TRUE,        TRUE,      "Monsters\\Mega\\Balr.TRN",        {  6,  7, 14,  1, 24,  5 }, { 3, 0, 0, 0, 2, 0 }, "Balrog",                  25,       29,     26,    180,    200, AI_MEGA,                                      MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,  130,      8,         22,         30,     0,       3,           0,           0,          75, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  |                    IMUNE_NULL_40, RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  3643 },
	{    160,   1270, "Monsters\\Snake\\Snake%c.CL2",    FALSE,       "Monsters\\Snake\\Snake%c%i.WAV",    FALSE,       FALSE,     NULL,                              { 12, 11, 13,  5, 18,  0 }, { 2, 0, 0, 0, 1, 0 }, "Cave Viper",              20,       24,     21,    100,    150, AI_SNAKE,                                     MFLAG_SEARCH                      ,    0,   90,      8,          8,         20,     0,       0,           0,           0,          60, MC_DEMON,    IMUNE_MAGIC                                                  , IMUNE_MAGIC                                                  ,         0,        7,  2725 },
	{    160,   1270, "Monsters\\Snake\\Snake%c.CL2",    FALSE,       "Monsters\\Snake\\Snake%c%i.WAV",    FALSE,       TRUE,      "Monsters\\Snake\\SnakR.TRN",      { 12, 11, 13,  5, 18,  0 }, { 2, 0, 0, 0, 1, 0 }, "Fire Drake",              22,       26,     23,    120,    170, AI_SNAKE,                                     MFLAG_SEARCH                      ,    1,  105,      8,         12,         24,     0,       0,           0,           0,          65, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE                                   , IMUNE_MAGIC  | IMUNE_FIRE                                    ,         0,        7,  3139 },
	{    160,   1270, "Monsters\\Snake\\Snake%c.CL2",    FALSE,       "Monsters\\Snake\\Snake%c%i.WAV",    FALSE,       TRUE,      "Monsters\\Snake\\Snakg.TRN",      { 12, 11, 13,  5, 18,  0 }, { 2, 0, 0, 0, 1, 0 }, "Gold Viper",              24,       27,     25,    140,    180, AI_SNAKE,                                     MFLAG_SEARCH                      ,    2,  120,      8,         15,         26,     0,       0,           0,           0,          70, MC_DEMON,    IMUNE_MAGIC  |               RESIST_LIGHTNING                , IMUNE_MAGIC  |               RESIST_LIGHTNING                ,         0,        7,  3540 },
	{    160,   1270, "Monsters\\Snake\\Snake%c.CL2",    FALSE,       "Monsters\\Snake\\Snake%c%i.WAV",    FALSE,       TRUE,      "Monsters\\Snake\\Snakb.TRN",      { 12, 11, 13,  5, 18,  0 }, { 2, 0, 0, 0, 1, 0 }, "Azure Drake",             28,       30,     27,    160,    200, AI_SNAKE,                                     MFLAG_SEARCH                      ,    3,  130,      8,         18,         30,     0,       0,           0,           0,          75, MC_DEMON,                   RESIST_FIRE | RESIST_LIGHTNING                , IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING                 ,         0,        7,  3791 },
	{    160,   2120, "Monsters\\Black\\Black%c.CL2",    FALSE,       "Monsters\\Black\\Black%c%i.WAV",    FALSE,       FALSE,     NULL,                              {  8,  8, 16,  4, 24,  0 }, { 2, 0, 0, 0, 0, 0 }, "Black Knight",            23,       27,     24,    150,    150, AI_SKELSD,                                    MFLAG_SEARCH                      ,    0,  110,      8,         15,         20,     0,       0,           0,           0,          75, MC_DEMON,    RESIST_MAGIC |               RESIST_LIGHTNING | IMUNE_NULL_40, RESIST_MAGIC |               IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  3360 },
	{    160,   2120, "Monsters\\Black\\Black%c.CL2",    FALSE,       "Monsters\\Black\\Black%c%i.WAV",    FALSE,       TRUE,      "Monsters\\Black\\BlkKntRT.TRN",   {  8,  8, 16,  4, 24,  0 }, { 2, 0, 0, 0, 0, 0 }, "Doom Guard",              25,       29,     26,    165,    165, AI_SKELSD,                                    MFLAG_SEARCH                      ,    0,  130,      8,         18,         25,     0,       0,           0,           0,          75, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE |                    IMUNE_NULL_40, RESIST_MAGIC | IMUNE_FIRE  |                    IMUNE_NULL_40,         0,        7,  3650 },
	{    160,   2120, "Monsters\\Black\\Black%c.CL2",    FALSE,       "Monsters\\Black\\Black%c%i.WAV",    FALSE,       TRUE,      "Monsters\\Black\\BlkKntBT.TRN",   {  8,  8, 16,  4, 24,  0 }, { 2, 0, 0, 0, 0, 0 }, "Steel Lord",              27,       30,     28,    180,    180, AI_SKELSD,                                    MFLAG_SEARCH                      ,    1,  120,      8,         20,         30,     0,       0,           0,           0,          80, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  4252 },
	{    160,   2120, "Monsters\\Black\\Black%c.CL2",    FALSE,       "Monsters\\Black\\Black%c%i.WAV",    FALSE,       TRUE,      "Monsters\\Black\\BlkKntBe.TRN",   {  8,  8, 16,  4, 24,  0 }, { 2, 0, 0, 0, 0, 0 }, "Blood Knight",            24,       26,     30,    200,    200, AI_SKELSD,                                    MFLAG_SEARCH                      ,    1,  130,      8,         25,         35,     0,       0,           0,           0,          85, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  5130 },
	{     96,    484, "Monsters\\Unrav\\Unrav%c.CL2",    FALSE,       "Monsters\\Acid\\Acid%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 10, 10, 12,  5, 16,  0 }, { 0, 0, 0, 0, 0, 0 }, "Unraveler",               26,       28,     25,     70,    150, AI_SKELSD,   0                                                                  ,    0,   75,      7,         10,         20,     0,       0,           0,           0,          70, MC_UNDEAD,   IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        3,  3812 },
	{     96,    484, "Monsters\\Unrav\\Unrav%c.CL2",    FALSE,       "Monsters\\Acid\\Acid%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 10, 10, 12,  5, 16,  0 }, { 0, 0, 0, 0, 0, 0 }, "Hollow One",              28,       30,     27,    135,    240, AI_SKELSD,   0                                                                  ,    1,   75,      7,         12,         24,     0,       0,           0,           0,          75, MC_UNDEAD,   IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,  4374 },
	{     96,    484, "Monsters\\Unrav\\Unrav%c.CL2",    FALSE,       "Monsters\\Acid\\Acid%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 10, 10, 12,  5, 16,  0 }, { 0, 0, 0, 0, 0, 0 }, "Pain Master",             27,       30,     29,    110,    200, AI_SKELSD,   0                                                                  ,    2,   80,      7,         16,         30,     0,       0,           0,           0,          80, MC_UNDEAD,   IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,  5147 },
	{     96,    484, "Monsters\\Unrav\\Unrav%c.CL2",    FALSE,       "Monsters\\Acid\\Acid%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 10, 10, 12,  5, 16,  0 }, { 0, 0, 0, 0, 0, 0 }, "Reality Weaver",          28,       30,     30,    135,    240, AI_SKELSD,   0                                                                  ,    3,   85,      7,         20,         35,     0,       0,           0,           0,          85, MC_UNDEAD,   RESIST_MAGIC | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40, RESIST_MAGIC | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        3,  5925 },
	{    128,    980, "Monsters\\Succ\\Scbs%c.CL2",      FALSE,       "Monsters\\Succ\\Scbs%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 14,  8, 16,  7, 24,  0 }, { 0, 0, 0, 0, 0, 0 }, "Succubus",                22,       26,     24,    120,    150, AI_SUCC,                                                     MFLAG_CAN_OPEN_DOOR,    0,  100,     10,          1,         20,     0,       0,           0,           0,          60, MC_DEMON,    RESIST_MAGIC                                                 , IMUNE_MAGIC  | RESIST_FIRE                                   ,         0,        3,  3696 },
	{    128,    980, "Monsters\\Succ\\Scbs%c.CL2",      FALSE,       "Monsters\\Succ\\Scbs%c%i.WAV",      FALSE,       TRUE,      "Monsters\\Succ\\Succb.TRN",       { 14,  8, 16,  7, 24,  0 }, { 0, 0, 0, 0, 0, 0 }, "Snow Witch",              25,       28,     26,    135,    175, AI_SUCC,                                                     MFLAG_CAN_OPEN_DOOR,    1,  110,     10,          1,         24,     0,       0,           0,           0,          65, MC_DEMON,                                 RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,         0,        3,  4084 },
	{    128,    980, "Monsters\\Succ\\Scbs%c.CL2",      FALSE,       "Monsters\\Succ\\Scbs%c%i.WAV",      FALSE,       TRUE,      "Monsters\\Succ\\Succrw.TRN",      { 14,  8, 16,  7, 24,  0 }, { 0, 0, 0, 0, 0, 0 }, "Hell Spawn",              27,       30,     28,    150,    200, AI_SUCC,                                      MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    2,  115,     10,          1,         30,     0,       0,           0,           0,          75, MC_DEMON,    RESIST_MAGIC |               IMUNE_LIGHTNING                 , IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,         0,        3,  4480 },
	{    128,    980, "Monsters\\Succ\\Scbs%c.CL2",      FALSE,       "Monsters\\Succ\\Scbs%c%i.WAV",      FALSE,       TRUE,      "Monsters\\Succ\\Succbw.TRN",      { 14,  8, 16,  7, 24,  0 }, { 0, 0, 0, 0, 0, 0 }, "Soul Burner",             28,       30,     30,    140,    225, AI_SUCC,                                      MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,  120,     10,          1,         35,     0,       0,           0,           0,          85, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING                , IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING                 ,         0,        3,  4644 },
	{    128,   2000, "Monsters\\Mage\\Mage%c.CL2",      TRUE,        "Monsters\\Mage\\Mage%c%i.WAV",      FALSE,       FALSE,     NULL,                              { 12,  1, 20,  8, 28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Counselor",               24,       26,     25,     70,     70, AI_COUNSLR,                                                  MFLAG_CAN_OPEN_DOOR,    0,   90,      8,          8,         20,     0,       0,           0,           0,           0, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING                , RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING                ,         0,        7,  4070 },
	{    128,   2000, "Monsters\\Mage\\Mage%c.CL2",      TRUE,        "Monsters\\Mage\\Mage%c%i.WAV",      FALSE,       TRUE,      "Monsters\\Mage\\Cnselg.TRN",      { 12,  1, 20,  8, 28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Magistrate",              26,       28,     27,     85,     85, AI_COUNSLR,                                                  MFLAG_CAN_OPEN_DOOR,    1,  100,      8,         10,         24,     0,       0,           0,           0,           0, MC_DEMON,    RESIST_MAGIC | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7,  4478 },
	{    128,   2000, "Monsters\\Mage\\Mage%c.CL2",      TRUE,        "Monsters\\Mage\\Mage%c%i.WAV",      FALSE,       TRUE,      "Monsters\\Mage\\Cnselgd.TRN",     { 12,  1, 20,  8, 28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Cabalist",                28,       30,     29,    120,    120, AI_COUNSLR,                                                  MFLAG_CAN_OPEN_DOOR,    2,  110,      8,         14,         30,     0,       0,           0,           0,           0, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  4929 },
	{    128,   2000, "Monsters\\Mage\\Mage%c.CL2",      TRUE,        "Monsters\\Mage\\Mage%c%i.WAV",      FALSE,       TRUE,      "Monsters\\Mage\\Cnselbk.TRN",     { 12,  1, 20,  8, 28, 20 }, { 0, 0, 0, 0, 0, 0 }, "Advocate",                30,       30,     30,    145,    145, AI_COUNSLR,                                                  MFLAG_CAN_OPEN_DOOR,    3,  120,      8,         15,         25,     0,       0,           0,           0,           0, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  4968 },
	{     96,    386, "Monsters\\Golem\\Golem%c.CL2",    TRUE,        "Monsters\\Golem\\Golm%c%i.WAV",     FALSE,       FALSE,     NULL,                              {  0, 16, 12,  0, 12, 20 }, { 0, 0, 0, 0, 0, 0 }, "Golem",                    0,        0,     12,      1,      1, AI_GOLUM,                                                    MFLAG_CAN_OPEN_DOOR,    0,    0,      7,          1,          1,     0,       0,           0,           0,           1, MC_DEMON,    0                                                            , 0                                                            ,         0,        0,     0 },
	{    160,   2000, "Monsters\\Diablo\\Diablo%c.CL2",  TRUE,        "Monsters\\Diablo\\Diablo%c%i.WAV",  TRUE,        FALSE,     NULL,                              { 16,  6, 16,  6, 16, 16 }, { 0, 0, 0, 0, 0, 0 }, "The Dark Lord",           50,       50,     30,   1666,   1666, AI_DIABLO,                  MFLAG_KNOCKBACK | MFLAG_SEARCH | MFLAG_CAN_OPEN_DOOR,    3,  220,      4,         30,         60,     0,      11,           0,           0,          70, MC_DEMON,    IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,         0,        7, 31666 },
	{    128,   1060, "Monsters\\DarkMage\\Dmage%c.CL2", TRUE,        "Monsters\\DarkMage\\Dmag%c%i.WAV",  FALSE,       FALSE,     NULL,                              {  6,  1, 21,  6, 23, 18 }, { 0, 0, 0, 0, 0, 0 }, "The Arch-Litch Malignus", 30,       30,     30,    160,    160, AI_COUNSLR,                                                  MFLAG_CAN_OPEN_DOOR,    3,  120,      8,         20,         40,     0,       0,           0,           0,          70, MC_DEMON,    RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,         0,        7,  4968 }
	// clang-format on
};
char MonstConvTbl[128] = {
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

unsigned char MonstAvailTbl[112] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 0, 2,
	2, 2, 2, 0, 2, 2, 2, 2, 1, 1,
	1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 2, 2, 2, 2, 0, 0, 0, 0,
	2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
	0, 0, 0, 0, 0, 0, 2, 2, 2, 2,
	0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 0, 0, 0,
	0, 2, 2, 2, 2, 2, 2, 2, 2, 0,
	0, 0
};
UniqMonstStruct UniqMonst[98] = {
	// clang-format off
	// mtype,       mName,                   mTrnName, mlevel, mmaxhp, mAi,      mint, mMinDamage, mMaxDamage, mMagicRes,                                                     mUnqAttr, mUnqVar1, mUnqVar2, mtalkmsg
	{  MT_NGOATMC,  "Gharbad the Weak",         "BSDB",     4,    120, AI_GARBUD,   3,          8,         16,                              IMUNE_LIGHTNING  | IMUNE_NULL_40,        0,        0,        0, QUEST_GARBUD1  },
	{  MT_SKING,    "Skeleton King",            "GENRL",    0,    240, AI_SKELKING, 3,          6,         16, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        1,        0,        0, 0              },
	{  MT_COUNSLR,  "Zhar the Mad",             "GENERAL",  8,    360, AI_ZHAR,     3,         16,         40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING                ,        0,        0,        0, QUEST_ZHAR1    },
	{  MT_BFALLSP,  "Snotspill",                "BNG",      4,    220, AI_SNOTSPIL, 3,         10,         18,                              RESIST_LIGHTNING                ,        0,        0,        0, QUEST_BANNER10 },
	{  MT_ADVOCATE, "Arch-Bishop Lazarus",      "GENERAL",  0,    600, AI_LAZURUS,  3,         30,         50, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, QUEST_VILE13   },
	{  MT_HLSPWN,   "Red Vex",                  "REDV",     0,    400, AI_LAZHELP,  3,         30,         50, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        0,        0,        0, QUEST_VILE13   },
	{  MT_HLSPWN,   "BlackJade",                "BLKJD",    0,    400, AI_LAZHELP,  3,         30,         50, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, QUEST_VILE13   },
	{  MT_RBLACK,   "Lachdanan",                "BHKA",    14,    500, AI_LACHDAN,  3,          0,          0, 0                                                            ,        0,        0,        0, QUEST_VEIL9    },
	{  MT_BTBLACK,  "Warlord of Blood",         "GENERAL", 13,    850, AI_WARLORD,  3,         35,         50, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,        0,        0,        0, QUEST_WARLRD9  },
	{  MT_CLEAVER,  "The Butcher",              "GENRL",    0,    220, AI_CLEAVER,  3,          6,         12,                RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_TSKELAX,  "Bonehead Keenaxe",         "BHKA",     2,     91, AI_SKELSD,   2,          4,         10, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        7,      100,        0, 0              },
	{  MT_RFALLSD,  "Bladeskin the Slasher",    "BSTS",     2,     51, AI_FALLEN,   0,          6,         18,                RESIST_FIRE                                   ,       11,       45,        0, 0              },
	{  MT_NZOMBIE,  "Soulpus",                  "GENERAL",  2,    133, AI_ZOMBIE,   0,          4,          8,                RESIST_FIRE | RESIST_LIGHTNING                ,        0,        0,        0, 0              },
	{  MT_RFALLSP,  "Pukerat the Unclean",      "PTU",      2,     77, AI_FALLEN,   3,          1,          5,                RESIST_FIRE                                   ,        0,        0,        0, 0              },
	{  MT_WSKELAX,  "Boneripper",               "BR",       2,     54, AI_BAT,      0,          6,         15, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_NZOMBIE,  "Rotfeast the Hungry",      "ETH",      2,     85, AI_SKELSD,   3,          4,         12, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_DFALLSD,  "Gutshank the Quick",       "GTQ",      3,     66, AI_BAT,      2,          6,         16,                RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MT_TSKELSD,  "Brokenhead Bangshield",    "BHBS",     3,    108, AI_SKELSD,   3,         12,         20, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_YFALLSP,  "Bongo",                    "BNG",      3,    178, AI_FALLEN,   3,          9,         21, 0                                                            ,        3,        0,        0, 0              },
	{  MT_BZOMBIE,  "Rotcarnage",               "RCRN",     3,    102, AI_ZOMBIE,   3,          9,         24, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,       11,       45,        0, 0              },
	{  MT_NSCAV,    "Shadowbite",               "SHBT",     2,     60, AI_SKELSD,   3,          3,         20,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_WSKELBW,  "Deadeye",                  "DE",       2,     49, AI_GOATBOW,  0,          6,          9, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_RSKELAX,  "Madeye the Dead",          "MTD",      4,     75, AI_BAT,      0,          9,         21, IMUNE_MAGIC  | IMUNE_FIRE                                    ,       11,       30,        0, 0              },
	{  MT_BSCAV,    "El Chupacabras",           "GENERAL",  3,    120, AI_GOATMC,   0,         10,         18,                RESIST_FIRE                                   ,        3,       30,        0, 0              },
	{  MT_TSKELBW,  "Skullfire",                "SKFR",     3,    125, AI_GOATBOW,  1,          6,         10,                IMUNE_FIRE                                    ,        0,      100,        0, 0              },
	{  MT_SNEAK,    "Warpskull",                "TSPO",     3,    117, AI_SNEAK,    2,          6,         18,                RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_GZOMBIE,  "Goretongue",               "PMR",      3,    156, AI_SKELSD,   1,         15,         30, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_WSCAV,    "Pulsecrawler",             "BHKA",     4,    150, AI_SCAV,     0,         16,         20,                IMUNE_FIRE  | RESIST_LIGHTNING                ,       11,       45,        0, 0              },
	{  MT_BLINK,    "Moonbender",               "GENERAL",  4,    135, AI_BAT,      0,          9,         27,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_BLINK,    "Wrathraven",               "GENERAL",  5,    135, AI_BAT,      2,          9,         22,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_YSCAV,    "Spineeater",               "GENERAL",  4,    180, AI_SCAV,     1,         18,         25,                              IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_RSKELBW,  "Blackash the Burning",     "BASHTB",   4,    120, AI_GOATBOW,  0,          6,         16, IMUNE_MAGIC  | IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_BFALLSD,  "Shadowcrow",               "GENERAL",  5,    270, AI_SNEAK,    2,         12,         25, 0                                                            ,        3,        0,        0, 0              },
	{  MT_LRDSAYTR, "Blightstone the Weak",     "BHKA",     4,    360, AI_SKELSD,   0,          4,         12, IMUNE_MAGIC  |               RESIST_LIGHTNING                ,        7,       70,        0, 0              },
	{  MT_FAT,      "Bilefroth the Pit Master", "BFTP",     6,    210, AI_BAT,      1,         16,         23, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_NGOATBW,  "Bloodskin Darkbow",        "BSDB",     5,    207, AI_GOATBOW,  0,          3,         16,                RESIST_FIRE | RESIST_LIGHTNING                ,       11,       55,        0, 0              },
	{  MT_GLOOM,    "Foulwing",                 "DB",       5,    246, AI_RHINO,    3,         12,         28,                RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MT_XSKELSD,  "Shadowdrinker",            "SHDR",     5,    300, AI_SNEAK,    1,         18,         26, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        8,       45,        0, 0              },
	{  MT_UNSEEN,   "Hazeshifter",              "BHKA",     5,    285, AI_SNEAK,    3,         18,         30,                              IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_NACID,    "Deathspit",                "BFDS",     6,    303, AI_ACIDUNIQ, 0,         12,         32,                RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_RGOATMC,  "Bloodgutter",              "BGBL",     6,    315, AI_BAT,      1,         24,         34,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_BGOATMC,  "Deathshade Fleshmaul",     "DSFM",     6,    276, AI_RHINO,    0,         12,         24, IMUNE_MAGIC  | RESIST_FIRE                                   ,        8,       65,        0, 0              },
	{  MT_WYRM,     "Warmaggot the Mad",        "GENERAL",  6,    246, AI_BAT,      3,         15,         30,                              RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_STORM,    "Glasskull the Jagged",     "BHKA",     7,    354, AI_STORM,    0,         18,         30, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_RGOATBW,  "Blightfire",               "BLF",      7,    321, AI_SUCC,     2,         13,         21,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_GARGOYLE, "Nightwing the Cold",       "GENERAL",  7,    342, AI_BAT,      1,         18,         26, IMUNE_MAGIC  |               RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_GGOATBW,  "Gorestone",                "GENERAL",  7,    303, AI_GOATBOW,  1,         15,         28,                              RESIST_LIGHTNING | IMUNE_NULL_40,        7,       70,        0, 0              },
	{  MT_BMAGMA,   "Bronzefist Firestone",     "GENERAL",  8,    360, AI_MAGMA,    0,         30,         36, IMUNE_MAGIC  | RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MT_INCIN,    "Wrathfire the Doomed",     "WFTD",     8,    270, AI_SKELSD,   2,         20,         30, IMUNE_MAGIC  | RESIST_FIRE |  RESIST_LIGHTNING               ,        3,        0,        0, 0              },
	{  MT_NMAGMA,   "Firewound the Grim",       "BHKA",     8,    303, AI_MAGMA,    0,         18,         22, IMUNE_MAGIC  | RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MT_MUDMAN,   "Baron Sludge",             "BSM",      8,    315, AI_SNEAK,    3,         25,         34, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,       11,       75,        0, 0              },
	{  MT_GGOATMC,  "Blighthorn Steelmace",     "BHSM",     7,    250, AI_RHINO,    0,         20,         28,                              RESIST_LIGHTNING                ,       11,       45,        0, 0              },
	{  MT_RACID,    "Chaoshowler",              "GENERAL",  8,    240, AI_ACIDUNIQ, 0,         12,         20, 0                                                            ,        3,        0,        0, 0              },
	{  MT_REDDTH,   "Doomgrin the Rotting",     "GENERAL",  8,    405, AI_STORM,    3,         25,         50, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_FLAMLRD,  "Madburner",                "GENERAL",  9,    270, AI_STORM,    0,         20,         40, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING                 ,        3,        0,        0, 0              },
	{  MT_LTCHDMN,  "Bonesaw the Litch",        "GENERAL",  9,    495, AI_STORM,    2,         30,         55, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_MUDRUN,   "Breakspine",               "GENERAL",  9,    351, AI_RHINO,    0,         25,         34,                RESIST_FIRE                                   ,        3,        0,        0, 0              },
	{  MT_REDDTH,   "Devilskull Sharpbone",     "GENERAL",  9,    444, AI_STORM,    1,         25,         40,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_STORM,    "Brokenstorm",              "GENERAL",  9,    411, AI_STORM,    2,         25,         36,                              IMUNE_LIGHTNING                 ,        3,        0,        0, 0              },
	{  MT_RSTORM,   "Stormbane",                "GENERAL",  9,    555, AI_STORM,    3,         30,         30,                              IMUNE_LIGHTNING                 ,        3,        0,        0, 0              },
	{  MT_TOAD,     "Oozedrool",                "GENERAL",  9,    483, AI_FAT,      3,         25,         30,                              RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_BLOODCLW, "Goldblight of the Flame",  "GENERAL", 10,    405, AI_GARG,     0,         15,         35, IMUNE_MAGIC  | IMUNE_FIRE                                    ,       11,       80,        0, 0              },
	{  MT_OBLORD,   "Blackstorm",               "GENERAL", 10,    525, AI_RHINO,    3,         20,         40, IMUNE_MAGIC  |               IMUNE_LIGHTNING                 ,       11,       90,        0, 0              },
	{  MT_RACID,    "Plaguewrath",              "GENERAL", 10,    450, AI_ACIDUNIQ, 2,         20,         30, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_RSTORM,   "The Flayer",               "GENERAL", 10,    501, AI_STORM,    1,         20,         35, RESIST_MAGIC | RESIST_FIRE | IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_FROSTC,   "Bluehorn",                 "GENERAL", 11,    477, AI_RHINO,    1,         25,         30, IMUNE_MAGIC  | RESIST_FIRE                                   ,       11,       90,        0, 0              },
	{  MT_HELLBURN, "Warpfire Hellspawn",       "GENERAL", 11,    525, AI_FIREMAN,  3,         10,         40, RESIST_MAGIC | IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_NSNAKE,   "Fangspeir",                "GENERAL", 11,    444, AI_SKELSD,   1,         15,         32,                IMUNE_FIRE  |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_UDEDBLRG, "Festerskull",              "GENERAL", 11,    600, AI_STORM,    2,         15,         30, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_NBLACK,   "Lionskull the Bent",       "GENERAL", 12,    525, AI_SKELSD,   2,         25,         25, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_COUNSLR,  "Blacktongue",              "GENERAL", 12,    360, AI_COUNSLR,  3,         15,         30,                RESIST_FIRE |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_DEATHW,   "Viletouch",                "GENERAL", 12,    525, AI_GARG,     3,         20,         40,                              IMUNE_LIGHTNING  | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_RSNAKE,   "Viperflame",               "GENERAL", 12,    570, AI_SKELSD,   1,         25,         35,                IMUNE_FIRE  | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_BSNAKE,   "Fangskin",                 "BHKA",    14,    681, AI_SKELSD,   2,         15,         50, IMUNE_MAGIC  |               RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_SUCCUBUS, "Witchfire the Unholy",     "GENERAL", 12,    444, AI_SUCC,     3,         10,         20, IMUNE_MAGIC  | IMUNE_FIRE  | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_BALROG,   "Blackskull",               "BHKA",    13,    750, AI_SKELSD,   3,         25,         40, IMUNE_MAGIC  |               RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_UNRAV,    "Soulslash",                "GENERAL", 12,    450, AI_SKELSD,   0,         25,         25, IMUNE_MAGIC  |                                  IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_VTEXLRD,  "Windspawn",                "GENERAL", 12,    711, AI_SKELSD,   1,         35,         40, IMUNE_MAGIC  | IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_GSNAKE,   "Lord of the Pit",          "GENERAL", 13,    762, AI_SKELSD,   2,         25,         42,                RESIST_FIRE |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_RTBLACK,  "Rustweaver",               "GENERAL", 13,    400, AI_SKELSD,   3,          1,         60, IMUNE_MAGIC  | IMUNE_FIRE  | IMUNE_LIGHTNING  | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_HOLOWONE, "Howlingire the Shade",     "GENERAL", 13,    450, AI_SKELSD,   2,         40,         75,                RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_MAEL,     "Doomcloud",                "GENERAL", 13,    612, AI_STORM,    1,          1,         60,                RESIST_FIRE | IMUNE_LIGHTNING                 ,        0,        0,        0, 0              },
	{  MT_PAINMSTR, "Bloodmoon Soulfire",       "GENERAL", 13,    684, AI_SKELSD,   1,         15,         40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_SNOWWICH, "Witchmoon",                "GENERAL", 13,    310, AI_SUCC,     3,         30,         40,                              RESIST_LIGHTNING                ,        0,        0,        0, 0              },
	{  MT_VTEXLRD,  "Gorefeast",                "GENERAL", 13,    771, AI_SKELSD,   3,         20,         55,                RESIST_FIRE |                    IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_RTBLACK,  "Graywar the Slayer",       "GENERAL", 14,    672, AI_SKELSD,   1,         30,         50,                              RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_MAGISTR,  "Dreadjudge",               "GENERAL", 14,    540, AI_COUNSLR,  1,         30,         40, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING                ,        3,        0,        0, 0              },
	{  MT_HLSPWN,   "Stareye the Witch",        "GENERAL", 14,    726, AI_SUCC,     2,         30,         50,                IMUNE_FIRE                                    ,        0,        0,        0, 0              },
	{  MT_BTBLACK,  "Steelskull the Hunter",    "GENERAL", 14,    831, AI_SKELSD,   3,         40,         50,                              RESIST_LIGHTNING | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_RBLACK,   "Sir Gorash",               "GENERAL", 16,   1050, AI_SKELSD,   1,         20,         60,                                                 IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_CABALIST, "The Vizier",               "GENERAL", 15,    850, AI_COUNSLR,  2,         25,         40,                IMUNE_FIRE                                    ,        3,        0,        0, 0              },
	{  MT_REALWEAV, "Zamphir",                  "GENERAL", 15,    891, AI_SKELSD,   2,         30,         50, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_HLSPWN,   "Bloodlust",                "GENERAL", 15,    825, AI_SUCC,     1,         20,         55, IMUNE_MAGIC  |               IMUNE_LIGHTNING  | IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_HLSPWN,   "Webwidow",                 "GENERAL", 16,    774, AI_SUCC,     1,         20,         50, IMUNE_MAGIC  | IMUNE_FIRE  |                    IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_SOLBRNR,  "Fleshdancer",              "GENERAL", 16,    999, AI_SUCC,     3,         30,         50, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        0,        0,        0, 0              },
	{  MT_OBLORD,   "Grimspike",                "GENERAL", 19,    534, AI_SNEAK,    1,         25,         40, IMUNE_MAGIC  | RESIST_FIRE |                    IMUNE_NULL_40,        3,        0,        0, 0              },
	{  MT_STORML,   "Doomlock",                 "GENERAL", 28,    534, AI_SNEAK,    1,         35,         55, IMUNE_MAGIC  | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_NULL_40,        3,        0,        0, 0              },
	{  -1,          NULL,                       NULL,       0,      0, 0,           0,          0,          0, 0                                                            ,        0,        0,        0, 0              }
	// clang-format on
};
int MWVel[24][3] = {
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

void(__fastcall *AiProc[])(int i) = {
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

void __fastcall InitMonsterTRN(int monst, BOOL special)
{
	BYTE *f;
	int i, n, j, k;

	f = Monsters[monst].trans_file;
	for (i = 0; i < 256; i++) {
		if (*f == 255) {
			*f = 0;
		}
		f++;
	}

	n = special ? 6 : 5;
	for (j = 0; j < n; ++j) {
		if (j != 1 || Monsters[monst].mtype < MT_COUNSLR || Monsters[monst].mtype > MT_ADVOCATE) {
			for (k = 0; k < 8; k++) {
				Cl2ApplyTrans(
				    Monsters[monst].Anims[j].Data[k],
				    Monsters[monst].trans_file,
				    Monsters[monst].Anims[j].Frames);
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

	for (i = 0; i < MAX_LVLMTYPES; i++) {
		Monsters[i].mPlaceFlags = 0;
	}

	ClrAllMonsters();
	nummonsters = 0;
	totalmonsters = MAXMONSTERS;

	for (i = 0; i < MAXMONSTERS; i++) {
		monstactive[i] = i;
	}

	uniquetrans = 0;
}

int __fastcall AddMonsterType(int type, int placeflag)
{
	BOOL done = FALSE;
	int i;

	for (i = 0; i < nummtypes && !done; i++) {
		done = Monsters[i].mtype == type;
	}

	i--;

	if (!done) {
		i = nummtypes;
		nummtypes++;
		Monsters[i].mtype = type;
		monstimgtot += monsterdata[type].mImage;
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
	char mamask;
	const int numskeltypes = 19;

	int nt; // number of types

	mamask = 3; // monster availability mask

	AddMonsterType(MT_GOLEM, 2);
	if (currlevel == 16) {
		AddMonsterType(MT_ADVOCATE, 1);
		AddMonsterType(MT_RBLACK, 1);
		AddMonsterType(MT_DIABLO, 2);
		return;
	}

	if (!setlevel) {
		if (QuestStatus(QTYPE_BUTCH))
			AddMonsterType(MT_CLEAVER, 2);
		if (QuestStatus(QTYPE_GARB))
			AddMonsterType(UniqMonst[0].mtype, 4);
		if (QuestStatus(QTYPE_ZHAR))
			AddMonsterType(UniqMonst[2].mtype, 4);
		if (QuestStatus(QTYPE_BOL))
			AddMonsterType(UniqMonst[3].mtype, 4);
		if (QuestStatus(QTYPE_VEIL))
			AddMonsterType(UniqMonst[7].mtype, 4);
		if (QuestStatus(QTYPE_WARLRD))
			AddMonsterType(UniqMonst[8].mtype, 4);

		if (gbMaxPlayers != 1 && currlevel == quests[QTYPE_KING]._qlevel) {

			AddMonsterType(MT_SKING, 4);

			nt = 0;
			for (i = MT_WSKELAX; i <= MT_WSKELAX + numskeltypes; i++) {
				if (IsSkel(i)) {
					minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
					maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

					if (currlevel >= minl && currlevel <= maxl) {
						if (MonstAvailTbl[i] & mamask) {
							skeltypes[nt++] = i;
						}
					}
				}
			}
			AddMonsterType(skeltypes[random(88, nt)], 1);
		}

		nt = 0;
		for (i = 0; i < 111; i++) {
			minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
			maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

			if (currlevel >= minl && currlevel <= maxl) {
				if (MonstAvailTbl[i] & mamask) {
					typelist[nt++] = i;
				}
			}
		}

		if (monstdebug) {
			for (i = 0; i < debugmonsttypes; i++)
				AddMonsterType(DebugMonsters[i], 1);
		} else {

			while (nt > 0 && nummtypes < MAX_LVLMTYPES && monstimgtot < 4000) {
				for (i = 0; i < nt;) {
					if (monsterdata[typelist[i]].mImage > 4000 - monstimgtot) {
						typelist[i] = typelist[--nt];
						continue;
					}

					i++;
				}

				if (nt != 0) {
					i = random(88, nt);
					AddMonsterType(typelist[i], 1);
					typelist[i] = typelist[--nt];
				}
			}
		}

	} else {
		if (setlvlnum == SL_SKELKING) {
			AddMonsterType(MT_SKING, 4);
		}
	}
}

void __fastcall InitMonsterGFX(int monst)
{
	int mtype, anim, i;
	char strBuff[256];
	unsigned char *celBuf;
	void *trans_file;

	mtype = (unsigned char)Monsters[monst].mtype;

	for (anim = 0; anim < 6; anim++) {
		if ((animletter[anim] != 's' || monsterdata[mtype].has_special) && monsterdata[mtype].Frames[anim] > 0) {
			sprintf(strBuff, monsterdata[mtype].GraphicType, animletter[anim]);

			celBuf = LoadFileInMem(strBuff, NULL);
			Monsters[monst].Anims[anim].CMem = celBuf;

			if (Monsters[monst].mtype != MT_GOLEM || (animletter[anim] != 's' && animletter[anim] != 'd')) {

				for (i = 0; i < 8; i++) {
					Monsters[monst].Anims[anim].Data[i] = &celBuf[((int *)celBuf)[i]];
				}
			} else {
				for (i = 0; i < 8; i++) {
					Monsters[monst].Anims[anim].Data[i] = celBuf;
				}
			}
		}

		// TODO: either the AnimStruct members have wrong naming or the MonsterData ones it seems
		Monsters[monst].Anims[anim].Frames = monsterdata[mtype].Frames[anim];
		Monsters[monst].Anims[anim].Rate = monsterdata[mtype].Rate[anim];
	}

	Monsters[monst].width = monsterdata[mtype].width;
	Monsters[monst].width2 = (monsterdata[mtype].width - 64) >> 1;
	Monsters[monst].mMinHP = monsterdata[mtype].mMinHP;
	Monsters[monst].mMaxHP = monsterdata[mtype].mMaxHP;
	Monsters[monst].has_special = monsterdata[mtype].has_special;
	Monsters[monst].mAFNum = monsterdata[mtype].mAFNum;
	Monsters[monst].MData = &monsterdata[mtype];

	if (monsterdata[mtype].has_trans) {
		Monsters[monst].trans_file = LoadFileInMem(monsterdata[mtype].TransFile, NULL);
		InitMonsterTRN(monst, monsterdata[mtype].has_special);

		trans_file = Monsters[monst].trans_file;
		Monsters[monst].trans_file = NULL;

		mem_free_dbg(trans_file);
	}

	if (mtype >= MT_NMAGMA && mtype <= MT_WMAGMA && !(MissileFileFlag & 1)) {
		MissileFileFlag |= 1;
		LoadMissileGFX(MFILE_MAGBALL);
	}
	if (mtype >= MT_STORM && mtype <= MT_MAEL && !(MissileFileFlag & 2)) {
		MissileFileFlag |= 2;
		LoadMissileGFX(MFILE_THINLGHT);
	}
	if (mtype == MT_SUCCUBUS) {
		if (MissileFileFlag & 4)
			return;

		MissileFileFlag |= 4;
		LoadMissileGFX(MFILE_FLARE);
		LoadMissileGFX(MFILE_FLAREEXP);
	}
	if (mtype == MT_SNOWWICH) {
		if (MissileFileFlag & 0x20)
			return;

		MissileFileFlag |= 0x20;
		LoadMissileGFX(MFILE_SCUBMISB);
		LoadMissileGFX(MFILE_SCBSEXPB);
	}
	if (mtype == MT_HLSPWN) {
		if (MissileFileFlag & 0x40)
			return;

		MissileFileFlag |= 0x40;
		LoadMissileGFX(MFILE_SCUBMISD);
		LoadMissileGFX(MFILE_SCBSEXPD);
	}
	if (mtype == MT_SOLBRNR) {
		if (MissileFileFlag & 0x80)
			return;

		MissileFileFlag |= 0x80;
		LoadMissileGFX(MFILE_SCUBMISC);
		LoadMissileGFX(MFILE_SCBSEXPC);
	}
	if (mtype >= MT_INCIN && mtype <= MT_HELLBURN && !(MissileFileFlag & 8)) {
		MissileFileFlag |= 8;
		LoadMissileGFX(MFILE_KRULL);
	}
	if (mtype >= MT_NACID && mtype <= MT_XACID && !(MissileFileFlag & 0x10)) {
		MissileFileFlag |= 0x10;
		LoadMissileGFX(MFILE_ACIDBF);
		LoadMissileGFX(MFILE_ACIDSPLA);
		LoadMissileGFX(MFILE_ACIDPUD);
	}
	if (mtype == MT_DIABLO) {
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
	monster[i]._mAnimData = monst->Anims[MA_STAND].Data[rd];
	monster[i]._mAnimDelay = monst->Anims[MA_STAND].Rate;
	monster[i]._mAnimCnt = random(88, monst->Anims[MA_STAND].Rate - 1);
	monster[i]._mAnimLen = monst->Anims[MA_STAND].Frames;
	monster[i]._mAnimFrame = random(88, monst->Anims[MA_STAND].Frames - 1) + 1;

	if (monst->mtype == MT_DIABLO) {
		monster[i]._mmaxhp = (random(88, 1) + 1666) << 6;
	} else {
		monster[i]._mmaxhp = (monst->mMinHP + random(88, monst->mMaxHP - monst->mMinHP + 1)) << 6;
	}

	if (gbMaxPlayers == 1) {
		monster[i]._mmaxhp >>= 1;
		if (monster[i]._mmaxhp < 64) {
			monster[i]._mmaxhp = 64;
		}
	}

	monster[i]._mhitpoints = monster[i]._mmaxhp;
	monster[i]._mAi = monst->MData->mAi;
	monster[i]._mint = monst->MData->mInt;
	monster[i]._mgoal = MGOAL_NORMAL;
	monster[i]._mgoalvar1 = 0;
	monster[i]._mgoalvar2 = 0;
	monster[i]._mgoalvar3 = 0;
	monster[i]._pathcount = 0;
	monster[i]._uniqtype = 0;
	monster[i]._msquelch = 0;
	monster[i].field_18 = 0;
	monster[i]._mDelFlag = FALSE;
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

	if (monster[i]._mAi == AI_GARG) {
		monster[i]._mAnimData = monst->Anims[MA_SPECIAL].Data[rd];
		monster[i]._mAnimFrame = 1;
		monster[i]._mFlags |= MFLAG_ALLOW_SPECIAL;
		monster[i]._mmode = MM_SATTACK;
	}

	if (gnDifficulty == DIFF_NIGHTMARE) {
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

	if (gnDifficulty == DIFF_HELL) {
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
	int i;
	MonsterStruct *Monst;

	for (i = 0; i < MAXMONSTERS; i++) {
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
		Monst->_mDelFlag = FALSE;
		Monst->_menemy = random(89, gbActivePlayers);
		Monst->_menemyx = plr[Monst->_menemy]._px;
		Monst->_menemyy = plr[Monst->_menemy]._py;
	}
}

BOOL __fastcall MonstPlace(int xp, int yp)
{
	char f;

	if (xp < 0 || xp >= MAXDUNX
	    || yp < 0 || yp >= MAXDUNY
	    || dMonster[xp][yp]
	    || dPlayer[xp][yp]) {
		return FALSE;
	}

	f = dFlags[xp][yp];

	if (f & DFLAG_VISIBLE) {
		return FALSE;
	}

	if (f & DFLAG_POPULATED) {
		return FALSE;
	}

	return !SolidLoc(xp, yp);
}

void __fastcall PlaceMonster(int i, int mtype, int x, int y)
{
	int rd;

	dMonster[x][y] = i + 1;

	rd = random(90, 8);
	InitMonster(i, rd, mtype, x, y);
}

void __fastcall PlaceUniqueMonst(int uniqindex, int miniontype, int packsize)
{
	int xp, yp, x, y, i;
	int uniqtype;
	int count2;
	char filestr[64];
	BOOL zharflag, done;

	UniqMonstStruct *Uniq = &UniqMonst[uniqindex];
	MonsterStruct *Monst = &monster[nummonsters];
	int count = 0;

	if ((uniquetrans + 19) << 8 >= LIGHTSIZE) {
		return;
	}

	for (uniqtype = 0; uniqtype < nummtypes; uniqtype++) {
		if (Monsters[uniqtype].mtype == Uniq->mtype) {
			break;
		}
	}

	while (1) {
		xp = random(91, 80) + 16;
		yp = random(91, 80) + 16;
		count2 = 0;
		for (x = xp - 3; x < xp + 3; x++) {
			for (y = yp - 3; y < yp + 3; y++) {
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && MonstPlace(x, y)) {
					count2++;
				}
			}
		}

		if (count2 < 9) {
			count++;
			if (count < 1000) {
				continue;
			}
		}

		if (MonstPlace(xp, yp)) {
			break;
		}
	}

	if (uniqindex == 3) {
		xp = 2 * setpc_x + 24;
		yp = 2 * setpc_y + 28;
	}
	if (uniqindex == 8) {
		xp = 2 * setpc_x + 22;
		yp = 2 * setpc_y + 23;
	}
	if (uniqindex == 2) {
		zharflag = TRUE;
		for (i = 0; i < themeCount; i++) {
			if (i == zharlib && zharflag == TRUE) {
				zharflag = FALSE;
				xp = 2 * themeLoc[i].x + 20;
				yp = 2 * themeLoc[i].y + 20;
			}
		}
	}
	if (gbMaxPlayers == 1) {
		if (uniqindex == 4) {
			xp = 32;
			yp = 46;
		}
		if (uniqindex == 5) {
			xp = 40;
			yp = 45;
		}
		if (uniqindex == 6) {
			xp = 38;
			yp = 49;
		}
		if (uniqindex == 1) {
			xp = 35;
			yp = 47;
		}
	} else {
		if (uniqindex == 4) {
			xp = 2 * setpc_x + 19;
			yp = 2 * setpc_y + 22;
		}
		if (uniqindex == 5) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 19;
		}
		if (uniqindex == 6) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 25;
		}
	}
	if (uniqindex == 9) {
		done = FALSE;
		for (yp = 0; yp < MAXDUNY && !done; yp++) {
			for (xp = 0; xp < MAXDUNX && !done; xp++) {
				done = dPiece[xp][yp] == 367;
			}
		}
	}

	PlaceMonster(nummonsters, uniqtype, xp, yp);
	Monst->_uniqtype = uniqindex + 1;

	if (Uniq->mlevel) {
		Monst->mLevel = 2 * Uniq->mlevel;
	} else {
		Monst->mLevel += 5;
	}

	Monst->mExp *= 2;
	Monst->mName = Uniq->mName;
	Monst->_mmaxhp = Uniq->mmaxhp << 6;

	if (gbMaxPlayers == 1) {
		Monst->_mmaxhp = Monst->_mmaxhp >> 1;
		if (Monst->_mmaxhp < 64) {
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

	if (gbMaxPlayers == 1) {
		if (Monst->mtalkmsg) {
			Monst->_mgoal = MGOAL_INQUIRING;
		}
	} else {
		if (Monst->_mAi == AI_LAZHELP) {
			Monst->mtalkmsg = 0;
		}

		if (Monst->_mAi != AI_LAZURUS || quests[QTYPE_VB]._qvar1 <= 3) {
			if (Monst->mtalkmsg) {
				Monst->_mgoal = MGOAL_INQUIRING;
			}
		} else {
			Monst->_mgoal = MGOAL_NORMAL;
		}
	}

	if (gnDifficulty == DIFF_NIGHTMARE) {
		Monst->mLevel += 15;
		Monst->_mmaxhp = 3 * Monst->_mmaxhp + 64;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 2 * (Monst->mExp + 1000);
		Monst->mMinDamage = 2 * (Monst->mMinDamage + 2);
		Monst->mMaxDamage = 2 * (Monst->mMaxDamage + 2);
		Monst->mMinDamage2 = 2 * (Monst->mMinDamage2 + 2);
		Monst->mMaxDamage2 = 2 * (Monst->mMaxDamage2 + 2);
	}

	if (gnDifficulty == DIFF_HELL) {
		Monst->mLevel += 30;
		Monst->_mmaxhp = 4 * Monst->_mmaxhp + 192;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 4 * (Monst->mExp + 1000);
		Monst->mMinDamage = 4 * Monst->mMinDamage + 6;
		Monst->mMaxDamage = 4 * Monst->mMaxDamage + 6;
		Monst->mMinDamage2 = 4 * Monst->mMinDamage2 + 6;
		Monst->mMaxDamage2 = 4 * Monst->mMaxDamage2 + 6;
	}

	sprintf(filestr, "Monsters\\Monsters\\%s.TRN", Uniq->mTrnName);
	LoadFileWithMem(filestr, &pLightTbl[256 * (uniquetrans + 19)]);

	Monst->_uniqtrans = uniquetrans++;

	if (Uniq->mUnqAttr & 4) {
		Monst->mHit = Uniq->mUnqVar1;
		Monst->mHit2 = Uniq->mUnqVar1;
	}
	if (Uniq->mUnqAttr & 8) {
		Monst->mArmorClass = Uniq->mUnqVar1;
	}

	nummonsters++;

	if (Uniq->mUnqAttr & 1) {
		PlaceGroup(miniontype, packsize, Uniq->mUnqAttr, nummonsters - 1);
	}

	if (Monst->_mAi != AI_GARG) {
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
		Monst->_mAnimFrame = random(88, Monst->_mAnimLen - 1) + 1;
		Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		Monst->_mmode = MM_STAND;
	}
}

void __cdecl PlaceQuestMonsters()
{
	int skeltype;
	unsigned char *setp;

	if (!setlevel) {
		if (QuestStatus(QTYPE_BUTCH)) {
			PlaceUniqueMonst(9, 0, 0);
		}

		if (currlevel == quests[QTYPE_KING]._qlevel && gbMaxPlayers != 1) {
			skeltype = 0;

			for (skeltype = 0; skeltype < nummtypes; skeltype++) {
				if (IsSkel(Monsters[skeltype].mtype)) {
					break;
				}
			}

			PlaceUniqueMonst(1, skeltype, 30);
		}

		if (QuestStatus(QTYPE_BOL)) {
			setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(QTYPE_BLOOD)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(QTYPE_BLIND)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(QTYPE_ANVIL)) {
			setp = LoadFileInMem("Levels\\L3Data\\Anvil.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x + 2, 2 * setpc_y + 2);
			mem_free_dbg(setp);
		}
		if (QuestStatus(QTYPE_WARLRD)) {
			setp = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
			AddMonsterType(UniqMonst[8].mtype, 1);
		}
		if (QuestStatus(QTYPE_VEIL)) {
			AddMonsterType(UniqMonst[7].mtype, 1);
		}
		if (QuestStatus(QTYPE_ZHAR) && zharlib == -1) {
			quests[QTYPE_ZHAR]._qactive = 0;
		}

		if (currlevel == quests[QTYPE_VB]._qlevel && gbMaxPlayers != 1) {
			AddMonsterType(UniqMonst[4].mtype, 4);
			AddMonsterType(UniqMonst[5].mtype, 4);
			PlaceUniqueMonst(4, 0, 0);
			PlaceUniqueMonst(5, 0, 0);
			PlaceUniqueMonst(6, 0, 0);
			setp = LoadFileInMem("Levels\\L4Data\\Vile1.DUN", 0);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
	} else {
		if (setlvlnum == SL_SKELKING) {
			PlaceUniqueMonst(1, 0, 0);
		}
	}
}

void __fastcall PlaceGroup(int mtype, int num, int leaderf, int leader)
{
	int placed, try1, try2, j;
	int xp, yp, x1, y1;

	placed = 0;

	for (try1 = 0; try1 < 10; try1++) {
		while (placed) {
			nummonsters--;
			placed--;
			dMonster[monster[nummonsters]._mx][monster[nummonsters]._my] = 0;
		}

		if (leaderf & 1) {
			int offset = random(92, 8);
			xp = monster[leader]._mx + offset_x[offset];
			yp = monster[leader]._my + offset_y[offset];
			x1 = xp;
			y1 = yp;
		} else {
			do {
				xp = random(93, 80) + 16;
				x1 = xp;
				yp = random(93, 80) + 16;
				y1 = yp;
			} while (!MonstPlace(xp, yp));
		}

		if (num + nummonsters > totalmonsters) {
			num = totalmonsters - nummonsters;
		}

		j = 0;
		for (try2 = 0; j < num && try2 < 100; xp += offset_x[random(94, 8)], yp += offset_x[random(94, 8)]) {
			if (!MonstPlace(xp, yp)
			    || (dTransVal[x1][y1] != dTransVal[xp][yp])
			    || (leaderf & 2) && ((abs(xp - x1) >= 4) || (abs(yp - y1) >= 4))) {
				try2++;
				continue;
			}

			PlaceMonster(nummonsters, mtype, xp, yp);
			if (leaderf & 1) {
				monster[nummonsters]._mmaxhp *= 2;
				monster[nummonsters]._mhitpoints = monster[nummonsters]._mmaxhp;
				monster[nummonsters]._mint = monster[leader]._mint;

				if (leaderf & 2) {
					monster[nummonsters].leader = leader;
					monster[nummonsters].leaderflag = 1;
					monster[nummonsters]._mAi = monster[leader]._mAi;
				}

				if (monster[nummonsters]._mAi != AI_GARG) {
					monster[nummonsters]._mAnimData = monster[nummonsters].MType->Anims[MA_STAND].Data[monster[nummonsters]._mdir];
					monster[nummonsters]._mAnimFrame = random(88, monster[nummonsters]._mAnimLen - 1) + 1;
					monster[nummonsters]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
					monster[nummonsters]._mmode = MM_STAND;
				}
			}
			nummonsters++;
			placed++;
			j++;
		}

		if (placed >= num) {
			break;
		}
	}

	if (leaderf & 2) {
		monster[leader].packsize = placed;
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
	int v0;                // ebp
	int v1;                // ebx
	TriggerStruct *v2;     // esi
	signed int v3;         // ebp
	signed int v4;         // edi
	int v5;                // edi
	int v6;                // esi
	int v7;                // eax
	int v8;                // ecx
	int v9;                // edx
	int v10;               // eax
	int v11;               // esi
	unsigned char *v12;    // edi
	int v13;               // ebx
	int v15;               // esi
	int v17;               // eax
	int v18;               // eax
	int v19;               // ebx
	TriggerStruct *v20;    // esi
	signed int v21;        // ebp
	signed int v22;        // edi
	int max;               // [esp+10h] [ebp-1C4h]
	int v24;               // [esp+14h] [ebp-1C0h]
	int scattertypes[111]; // [esp+18h] [ebp-1BCh]

	v0 = 0;
	max = 0;
	if (gbMaxPlayers != 1)
		CheckDungeonClear();
	if (!setlevel) {
		AddMonster(1, 0, 0, 0, 0);
		AddMonster(1, 0, 0, 0, 0);
		AddMonster(1, 0, 0, 0, 0);
		AddMonster(1, 0, 0, 0, 0);
		if (!setlevel && currlevel == 16)
			LoadDiabMonsts();
	}
	v24 = trigflag[4];
	if (currlevel == 15)
		v24 = 1;
	v1 = v24;
	if (v24 > 0) {
		v2 = trigs;
		do {
			v3 = -2;
			do {
				v4 = -2;
				do
					DoVision(v3 + v2->_tx, v4++ + v2->_ty, 15, 0, 0);
				while (v4 < 2);
				++v3;
			} while (v3 < 2);
			++v2;
			--v1;
		} while (v1);
		v0 = 0;
	}
	PlaceQuestMonsters();
	if (!setlevel) {
		PlaceUniques();
		v5 = 16;
		do {
			v6 = 16;
			do {
				if (!SolidLoc(v5, v6))
					++v0;
				++v6;
			} while (v6 < 96);
			++v5;
		} while (v5 < 96);
		v7 = v0 / 30;
		if (gbMaxPlayers != 1)
			v7 += v7 >> 1;
		v8 = nummonsters;
		if (nummonsters + v7 > 190)
			v7 = 190 - nummonsters;
		v9 = nummtypes;
		v10 = nummonsters + v7;
		v11 = 0;
		totalmonsters = v10;
		if (nummtypes > 0) {
			v12 = &Monsters[0].mPlaceFlags;
			do {
				if (*v12 & 1) {
					v13 = max++;
					scattertypes[v13] = v11;
				}
				++v11;
				v12 += 328;
			} while (v11 < v9);
		}
		if (v8 < v10) {
			while (1) {
				v15 = scattertypes[random(95, max)];
				if (currlevel == 1)
					break;
				if (!random(95, 2))
					break;
				if (currlevel == 2) {
					v17 = random(95, 2) + 1;
				LABEL_40:
					v18 = v17 + 1;
					goto LABEL_41;
				}
				v18 = random(95, 3) + 3;
			LABEL_41:
				PlaceGroup(v15, v18, 0, 0);
				if (nummonsters >= totalmonsters)
					goto LABEL_42;
			}
			v17 = 0;
			goto LABEL_40;
		}
	}
LABEL_42:
	v19 = v24;
	if (v24 > 0) {
		v20 = trigs;
		do {
			v21 = -2;
			do {
				v22 = -2;
				do
					DoUnVision(v21 + v20->_tx, v22++ + v20->_ty, 15);
				while (v22 < 2);
				++v21;
			} while (v21 < 2);
			++v20;
			--v19;
		} while (v19);
	}
}
// 5CF31D: using guessed type char setlevel;
// 658550: using guessed type int totalmonsters;
// 679660: using guessed type char gbMaxPlayers;
// 432637: using guessed type int var_1BC[111];

void __cdecl PlaceUniques()
{
	int v0;              // edi
	int v1;              // eax
	UniqMonstStruct *v2; // ecx
	int v3;              // eax
	int v4;              // edx
	CMonster *v5;        // esi
	int v6;              // eax
	int v7;              // edx

	v0 = 0;
	if (UniqMonst[0].mtype != -1) {
		v1 = 0;
		v2 = UniqMonst;
		while (UniqMonst[v1].mlevel != currlevel) {
		LABEL_25:
			v1 = ++v0;
			v2 = &UniqMonst[v0];
			if (v2->mtype == -1)
				return;
		}
		v3 = 0;
		v4 = 0;
		if (nummtypes > 0) {
			v5 = Monsters;
			do {
				if (v3)
					break;
				v6 = -((char)v2->mtype != (unsigned char)v5->mtype);
				++v5;
				v3 = v6 + 1;
				++v4;
			} while (v4 < nummtypes);
		}
		v7 = v4 - 1;
		if (!v0) {
			if (quests[QTYPE_GARB]._qactive)
				goto LABEL_23;
			v3 = 0;
		}
		if (v0 == 2) {
			if (quests[QTYPE_ZHAR]._qactive)
				goto LABEL_23;
			v3 = 0;
		}
		if (v0 == 3) {
			if (quests[QTYPE_BOL]._qactive)
				goto LABEL_23;
			v3 = 0;
		}
		if (v0 != 7) {
		LABEL_20:
			if (v0 == 8 && !quests[QTYPE_WARLRD]._qactive)
				v3 = 0;
			goto LABEL_23;
		}
		if (!quests[QTYPE_VEIL]._qactive) {
			v3 = 0;
			goto LABEL_20;
		}
	LABEL_23:
		if (v3)
			PlaceUniqueMonst(v0, v7, 8);
		goto LABEL_25;
	}
}

void __fastcall SetMapMonsters(unsigned char *pMap, int startx, int starty)
{
	unsigned char *v3;  // esi
	unsigned short v4;  // cx
	int v5;             // edx
	int v6;             // edi
	int v7;             // ecx
	unsigned char *v8;  // edx
	int i;              // esi
	int v10;            // eax
	int v11;            // ecx
	int v12;            // [esp+Ch] [ebp-Ch]
	int v13;            // [esp+10h] [ebp-8h]
	unsigned char *v14; // [esp+14h] [ebp-4h]
	int startya;        // [esp+20h] [ebp+8h]

	v12 = startx;
	v3 = pMap;
	AddMonsterType(MT_GOLEM, 2);
	AddMonster(1, 0, 0, 0, 0);
	AddMonster(1, 0, 0, 0, 0);
	AddMonster(1, 0, 0, 0, 0);
	AddMonster(1, 0, 0, 0, 0);
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
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
	if (v7 > 0) {
		v13 = v7;
		startya = starty + 16;
		do {
			for (i = 0; i < v6; v14 += 2) {
				if (*(_WORD *)v8) {
					v10 = AddMonsterType(MonstConvTbl[*(unsigned short *)v8 - 1], 2); /* fix */
					v11 = nummonsters++;
					PlaceMonster(v11, v10, i + v12 + 16, startya);
				}
				v8 = v14 + 2;
				++i;
			}
			++startya;
			--v13;
		} while (v13);
	}
}
// 5CF31D: using guessed type char setlevel;

void __fastcall DeleteMonster(int i)
{
	int temp;

	nummonsters--;
	temp = monstactive[nummonsters];
	monstactive[nummonsters] = monstactive[i];
	monstactive[i] = temp;
}

int __fastcall AddMonster(int x, int y, int dir, int mtype, int InMap)
{
	if (nummonsters < MAXMONSTERS) {
		int i = monstactive[nummonsters++];
		if (InMap)
			dMonster[x][y] = i + 1;
		InitMonster(i, dir, mtype, x, y);
		return i;
	}

	return -1;
}

void __fastcall NewMonsterAnim(int i, AnimStruct *anim, int md)
{
	monster[i]._mAnimData = anim->Data[md];
	monster[i]._mAnimCnt = 0;
	monster[i]._mAnimLen = anim->Frames;
	monster[i]._mAnimFrame = 1;
	monster[i]._mFlags &= ~(MFLAG_LOCK_ANIMATION | MFLAG_ALLOW_SPECIAL);
	monster[i]._mAnimDelay = anim->Rate;
	monster[i]._mdir = md;
}

BOOL __fastcall M_Ranged(int i)
{
	char ai = monster[i]._mAi;
	return ai == AI_SKELBOW || ai == AI_GOATBOW || ai == AI_SUCC || ai == AI_LAZHELP;
}

BOOL __fastcall M_Talker(int i)
{
	char ai = monster[i]._mAi;
	return ai == AI_LAZURUS
	    || ai == AI_WARLORD
	    || ai == AI_GARBUD
	    || ai == AI_ZHAR
	    || ai == AI_SNOTSPIL
	    || ai == AI_LACHDAN
	    || ai == AI_LAZHELP;
}

void __fastcall M_Enemy(int i)
{
	MonsterStruct *v1; // esi
	int *v2;           // edi
	int v3;            // eax
	int v4;            // ecx
	int v5;            // ebx
	int v6;            // eax
	int v7;            // eax
	int v8;            // eax
	int v9;            // ecx
	int v10;           // edi
	//int v11; // edx
	int v12;        // eax
	int v13;        // ecx
	int v14;        // ebx
	int v15;        // eax
	int v16;        // eax
	int v17;        // [esp+Ch] [ebp-20h]
	int v18;        // [esp+10h] [ebp-1Ch]
	BOOL v19;       // [esp+14h] [ebp-18h]
	BOOL v20;       // [esp+14h] [ebp-18h]
	signed int v21; // [esp+18h] [ebp-14h]
	int j;          // [esp+18h] [ebp-14h]
	signed int v23; // [esp+1Ch] [ebp-10h]
	signed int v24; // [esp+20h] [ebp-Ch]
	BOOL v25;       // [esp+24h] [ebp-8h]
	char v26;       // [esp+2Ah] [ebp-2h]
	char v27;       // [esp+2Bh] [ebp-1h]

	v24 = -1;
	v18 = i;
	v23 = -1;
	v1 = &monster[i];
	v25 = 0;
	if (!(v1->_mFlags & MFLAG_GOLEM)) {
		v21 = 0;
		v2 = &plr[0].plrlevel;
		do {
			if (!*((_BYTE *)v2 - 23) || currlevel != *v2 || *((_BYTE *)v2 + 267) || !v2[89] && gbMaxPlayers != 1)
				goto LABEL_18;
			v3 = v1->_my;
			v4 = v2[2];
			v19 = dTransVal[v2[1]][v4] == dTransVal[v1->_mx][v3];
			v5 = abs(v3 - v4);
			if (abs(v1->_mx - v2[1]) <= v5)
				v6 = v1->_my - v2[2];
			else
				v6 = v1->_mx - v2[1];
			v7 = abs(v6);
			if (v19) {
				if (!v25)
					goto LABEL_17;
			} else if (v25) {
				goto LABEL_16;
			}
			if (v7 < v23)
				goto LABEL_17;
		LABEL_16:
			if (v24 == -1) {
			LABEL_17:
				v1->_mFlags &= ~MFLAG_TARGETS_MONSTER;
				v24 = v21;
				v27 = *((_BYTE *)v2 + 12);
				v26 = *((_BYTE *)v2 + 16);
				v23 = v7;
				v25 = v19;
			}
		LABEL_18:
			++v21;
			v2 += 5430;
		} while ((signed int)v2 < (signed int)&plr[4].plrlevel);
	}
	v8 = 0;
	for (j = 0; j < nummonsters; v8 = j++ + 1) {
		v9 = monstactive[v8];
		v17 = monstactive[v8];
		if (v9 == v18)
			continue;
		v10 = v9;
		if (monster[v9]._mx == 1 && !monster[v10]._my)
			continue;
		if (M_Talker(v9) && monster[v10].mtalkmsg)
			continue;
		if (!(v1->_mFlags & MFLAG_GOLEM)
		    && ((abs(monster[v10]._mx - v1->_mx) >= 2 || abs(monster[v10]._my - v1->_my) >= 2) && !M_Ranged(v18) /* v11 */
		           || !(v1->_mFlags & MFLAG_GOLEM) && !(monster[v10]._mFlags & MFLAG_GOLEM))) {
			continue;
		}
		v12 = v1->_my;
		v13 = monster[v10]._my;
		v20 = dTransVal[monster[v10]._mx][v13] == dTransVal[v1->_mx][v12];
		v14 = abs(v12 - v13);
		if (abs(v1->_mx - monster[v10]._mx) <= v14)
			v15 = v1->_my - monster[v10]._my;
		else
			v15 = v1->_mx - monster[v10]._mx;
		v16 = abs(v15);
		if (v20) {
			if (!v25)
				goto LABEL_40;
		} else if (v25) {
			goto LABEL_39;
		}
		if (v16 < v23)
			goto LABEL_40;
	LABEL_39:
		if (v24 == -1) {
		LABEL_40:
			v1->_mFlags |= MFLAG_TARGETS_MONSTER;
			v24 = v17;
			v27 = monster[v10]._mfutx;
			v26 = monster[v10]._mfuty;
			v23 = v16;
			v25 = v20;
		}
	}
	if (v24 == -1) {
		v1->_mFlags |= MFLAG_NO_ENEMY;
	} else {
		v1->_mFlags &= ~MFLAG_NO_ENEMY;
		v1->_menemy = v24;
		v1->_menemyx = v27;
		v1->_menemyy = v26;
	}
}
// 679660: using guessed type char gbMaxPlayers;

int __fastcall M_GetDir(int i)
{
	return GetDirection(monster[i]._mx, monster[i]._my, monster[i]._menemyx, monster[i]._menemyy);
}

void __fastcall M_CheckEFlag(int i)
{
	int v1;        // ecx
	int v2;        // edi
	char *v3;      // eax
	signed int v4; // edx

	v1 = i;
	v2 = 0;
	v3 = (char *)dpiece_defs_map_2 + 32 * (112 * (monster[v1]._mx - 1) + monster[v1]._my + 1);
	if (v3 < (char *)dpiece_defs_map_2)
		goto LABEL_9;
	v4 = 2;
	do
		v2 |= *(unsigned short *)&v3[2 * v4++];
	while (v4 < 10);
	if (v2 | dArch[monster[v1]._mx - 1][monster[v1]._my + 1])
		monster[v1]._meflag = 1;
	else
	LABEL_9:
		monster[v1]._meflag = 0;
}

void __fastcall M_StartStand(int i, int md)
{
	ClearMVars(i);
	if (monster[i].MType->mtype == MT_GOLEM)
		NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], md);
	else
		NewMonsterAnim(i, &monster[i].MType->Anims[MA_STAND], md);
	monster[i]._mVar1 = monster[i]._mmode;
	monster[i]._mVar2 = 0;
	monster[i]._mmode = MM_STAND;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
	M_Enemy(i);
}

void __fastcall M_StartDelay(int i, int len)
{
	if (len <= 0) {
		return;
	}

	if (monster[i]._mAi != AI_LAZURUS) {
		monster[i]._mVar2 = len;
		monster[i]._mmode = MM_DELAY;
	}
}

void __fastcall M_StartSpStand(int i, int md)
{
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_SPSTAND;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void __fastcall M_StartWalk(int i, int xvel, int yvel, int xadd, int yadd, int EndDir)
{
	int fx = xadd + monster[i]._mx;
	int fy = yadd + monster[i]._my;

	dMonster[fx][fy] = -(i + 1);
	monster[i]._mmode = MM_WALK;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mfutx = fx;
	monster[i]._mfuty = fy;
	monster[i]._mxvel = xvel;
	monster[i]._myvel = yvel;
	monster[i]._mVar1 = xadd;
	monster[i]._mVar2 = yadd;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], EndDir);
	monster[i]._mVar6 = 0;
	monster[i]._mVar7 = 0;
	monster[i]._mVar8 = 0;
	M_CheckEFlag(i);
}

void __fastcall M_StartWalk2(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir)
{
	int fx = xadd + monster[i]._mx;
	int fy = yadd + monster[i]._my;

	dMonster[monster[i]._mx][monster[i]._my] = -(i + 1);
	monster[i]._mVar1 = monster[i]._mx;
	monster[i]._mVar2 = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mx = fx;
	monster[i]._my = fy;
	monster[i]._mfutx = fx;
	monster[i]._mfuty = fy;
	dMonster[fx][fy] = i + 1;
	if (monster[i]._uniqtype != 0)
		ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
	monster[i]._mxoff = xoff;
	monster[i]._myoff = yoff;
	monster[i]._mmode = MM_WALK2;
	monster[i]._mxvel = xvel;
	monster[i]._myvel = yvel;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], EndDir);
	monster[i]._mVar6 = 16 * xoff;
	monster[i]._mVar7 = 16 * yoff;
	monster[i]._mVar8 = 0;
	M_CheckEFlag(i);
}

void __fastcall M_StartWalk3(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir)
{
	int fx = xadd + monster[i]._mx;
	int fy = yadd + monster[i]._my;
	int x = mapx + monster[i]._mx;
	int y = mapy + monster[i]._my;

	if (monster[i]._uniqtype != 0)
		ChangeLightXY(monster[i].mlid, x, y);

	dMonster[monster[i]._mx][monster[i]._my] = -(i + 1);
	dMonster[fx][fy] = -(i + 1);
	monster[i]._mVar4 = x;
	monster[i]._mVar5 = y;
	dFlags[x][y] |= DFLAG_MONSTER;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mfutx = fx;
	monster[i]._mfuty = fy;
	monster[i]._mxoff = xoff;
	monster[i]._myoff = yoff;
	monster[i]._mmode = MM_WALK3;
	monster[i]._mxvel = xvel;
	monster[i]._myvel = yvel;
	monster[i]._mVar1 = fx;
	monster[i]._mVar2 = fy;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], EndDir);
	monster[i]._mVar6 = 16 * xoff;
	monster[i]._mVar7 = 16 * yoff;
	monster[i]._mVar8 = 0;
	M_CheckEFlag(i);
}

void __fastcall M_StartAttack(int i)
{
	int md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_ATTACK], md);
	monster[i]._mmode = MM_ATTACK;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void __fastcall M_StartRAttack(int i, int missile_type, int dam)
{
	int md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_ATTACK], md);
	monster[i]._mmode = MM_RATTACK;
	monster[i]._mVar1 = missile_type;
	monster[i]._mVar2 = dam;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void __fastcall M_StartRSpAttack(int i, int missile_type, int dam)
{
	int md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_RSPATTACK;
	monster[i]._mVar1 = missile_type;
	monster[i]._mVar2 = 0;
	monster[i]._mVar3 = dam;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void __fastcall M_StartSpAttack(int i)
{
	int md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_SATTACK;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void __fastcall M_StartEat(int i)
{
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], monster[i]._mdir);
	monster[i]._mmode = MM_SATTACK;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	M_CheckEFlag(i);
}

void __fastcall M_ClearSquares(int i)
{
	int x, y, mx, my, m1, m2;

	mx = monster[i]._moldx;
	my = monster[i]._moldy;
	m1 = -1 - i;
	m2 = i + 1;

	for (y = my - 1; y <= my + 1; y++) {
		if (y >= 0 && y < MAXDUNY) {
			for (x = mx - 1; x <= mx + 1; x++) {
				if (x >= 0 && x < MAXDUNX && (dMonster[x][y] == m1 || dMonster[x][y] == m2))
					dMonster[x][y] = 0;
			}
		}
	}

	if (mx + 1 < MAXDUNX)
		dFlags[mx + 1][my] &= ~DFLAG_MONSTER;
	if (my + 1 < MAXDUNY)
		dFlags[mx][my + 1] &= ~DFLAG_MONSTER;
}

void __fastcall M_GetKnockback(int i)
{
	int d = (monster[i]._mdir - 4) & 7;
	if (DirOK(i, d)) {
		M_ClearSquares(i);
		monster[i]._moldx += offset_x[d];
		monster[i]._moldy += offset_y[d];
		NewMonsterAnim(i, &monster[i].MType->Anims[MA_GOTHIT], monster[i]._mdir);
		monster[i]._mmode = MM_GOTHIT;
		monster[i]._mxoff = 0;
		monster[i]._myoff = 0;
		monster[i]._mx = monster[i]._moldx;
		monster[i]._my = monster[i]._moldy;
		monster[i]._mfutx = monster[i]._moldx;
		monster[i]._mfuty = monster[i]._moldy;
		M_CheckEFlag(i);
		M_ClearSquares(i);
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
	}
}

void __fastcall M_StartHit(int i, int pnum, int dam)
{
	if (pnum >= 0)
		monster[i].mWhoHit |= 1 << pnum;
	if (pnum == myplr) {
		delta_monster_hp(i, monster[i]._mhitpoints, currlevel);
		NetSendCmdParam2(FALSE, CMD_MONSTDAMAGE, i, dam);
	}
	PlayEffect(i, 1);
	if (monster[i].MType->mtype >= MT_SNEAK && monster[i].MType->mtype <= MT_ILLWEAV || dam >> 6 >= monster[i].mLevel + 3) {
		if (pnum >= 0) {
			monster[i]._mFlags &= ~MFLAG_TARGETS_MONSTER;
			monster[i]._menemy = pnum;
			monster[i]._menemyx = plr[pnum]._px;
			monster[i]._menemyy = plr[pnum]._py;
			monster[i]._mdir = M_GetDir(i);
		}
		if (monster[i].MType->mtype == MT_BLINK) {
			M_Teleport(i);
		} else if (monster[i].MType->mtype >= MT_NSCAV && monster[i].MType->mtype <= MT_YSCAV) {
			monster[i]._mgoal = MGOAL_NORMAL;
		}
		if (monster[i]._mmode != MM_STONE) {
			NewMonsterAnim(i, &monster[i].MType->Anims[MA_GOTHIT], monster[i]._mdir);
			monster[i]._mmode = MM_GOTHIT;
			monster[i]._mxoff = 0;
			monster[i]._myoff = 0;
			monster[i]._mx = monster[i]._moldx;
			monster[i]._my = monster[i]._moldy;
			monster[i]._mfutx = monster[i]._moldx;
			monster[i]._mfuty = monster[i]._moldy;
			M_CheckEFlag(i);
			M_ClearSquares(i);
			dMonster[monster[i]._mx][monster[i]._my] = i + 1;
		}
	}
}

void __fastcall M_DiabloDeath(int i, BOOL sendmsg)
{
	int v2;     // esi
	int v3;     // edi
	int v4;     // eax
	int v5;     // ebx
	int v6;     // esi
	int v7;     // ecx
	int v8;     // eax
	int v9;     // esi
	int v10;    // eax
	double v11; // st7
	int v12;    // eax
	int v13;    // ecx
	int v14;    // esi
	int v15;    // [esp+8h] [ebp-8h]
	int j;      // [esp+Ch] [ebp-4h]
	int v17;    // [esp+Ch] [ebp-4h]

	v15 = i;
	v2 = sendmsg;
	v3 = i;
	PlaySFX(USFX_DIABLOD);
	quests[QTYPE_MOD]._qactive = 3;
	if (v2)
		NetSendCmdQuest(TRUE, 5u);
	gbProcessPlayers = FALSE;
	_LOBYTE(sgbSaveSoundOn) = gbSoundOn;
	v4 = 0;
	for (j = 0; j < nummonsters; ++j) {
		v5 = monstactive[v4];
		if (v5 != v15 && monster[v3]._msquelch) {
			v6 = v5;
			NewMonsterAnim(monstactive[v4], &monster[v5].MType->Anims[MA_DEATH], monster[v5]._mdir);
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
			dMonster[monster[v6]._mx][monster[v6]._my] = v5 + 1;
		}
		v4 = j + 1;
	}
	AddLight(monster[v3]._mx, monster[v3]._my, 8);
	DoVision(monster[v3]._mx, monster[v3]._my, 8, 0, 1);
	v9 = abs(ViewY - monster[v3]._my);
	if (abs(ViewX - monster[v3]._mx) <= v9)
		v10 = ViewY - monster[v3]._my;
	else
		v10 = ViewX - monster[v3]._mx;
	v17 = abs(v10);
	if (v17 > 20)
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
// 64D32C: using guessed type int sgbSaveSoundOn;

void __fastcall M2MStartHit(int mid, int i, int dam)
{
	if ((DWORD)mid >= MAXMONSTERS) {
		app_fatal("Invalid monster %d getting hit by monster", mid);
	}

	if (monster[mid].MType == NULL) {
		app_fatal("Monster %d \"%s\" getting hit by monster: MType NULL", mid, monster[mid].mName);
	}

	if (i >= 0)
		monster[i].mWhoHit |= 1 << i;

	delta_monster_hp(mid, monster[mid]._mhitpoints, currlevel);
	NetSendCmdParam2(FALSE, CMD_MONSTDAMAGE, mid, dam);
	PlayEffect(mid, 1);

	if (monster[mid].MType->mtype >= MT_SNEAK && monster[mid].MType->mtype <= MT_ILLWEAV || dam >> 6 >= monster[mid].mLevel + 3) {
		if (i >= 0)
			monster[mid]._mdir = (monster[i]._mdir - 4) & 7;

		if (monster[mid].MType->mtype == MT_BLINK) {
			M_Teleport(mid);
		} else if (monster[mid].MType->mtype >= MT_NSCAV && monster[mid].MType->mtype <= MT_YSCAV) {
			monster[mid]._mgoal = MGOAL_NORMAL;
		}

		if (monster[mid]._mmode != MM_STONE) {
			if (monster[mid].MType->mtype != MT_GOLEM) {
				NewMonsterAnim(mid, &monster[mid].MType->Anims[MA_GOTHIT], monster[mid]._mdir);
				monster[mid]._mmode = MM_GOTHIT;
			}

			monster[mid]._mxoff = 0;
			monster[mid]._myoff = 0;
			monster[mid]._mx = monster[mid]._moldx;
			monster[mid]._my = monster[mid]._moldy;
			monster[mid]._mfutx = monster[mid]._moldx;
			monster[mid]._mfuty = monster[mid]._moldy;
			M_CheckEFlag(mid);
			M_ClearSquares(mid);
			dMonster[monster[mid]._mx][monster[mid]._my] = mid + 1;
		}
	}
}

void __fastcall MonstStartKill(int i, int pnum, BOOL sendmsg)
{
	int md;

	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("MonstStartKill: Invalid monster %d", i);
	}
	if (!monster[i].MType) {
		app_fatal("MonstStartKill: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	}

	if (pnum >= 0)
		monster[i].mWhoHit |= 1 << pnum;
	if (pnum < 4 && i > 4)
		AddPlrMonstExper(monster[i].mLevel, monster[i].mExp, monster[i].mWhoHit);
	monstkills[monster[i].MType->mtype]++;
	monster[i]._mhitpoints = 0;
	SetRndSeed(monster[i]._mRndSeed);
	if (QuestStatus(QTYPE_GARB) && monster[i].mName == UniqMonst[0].mName) {
		CreateTypeItem(monster[i]._mx + 1, monster[i]._my + 1, TRUE, 4, FALSE, TRUE, FALSE);
	} else if (i > 3) {
		SpawnItem(i, monster[i]._mx, monster[i]._my, sendmsg);
	}
	if (monster[i].MType->mtype == MT_DIABLO)
		M_DiabloDeath(i, TRUE);
	else
		PlayEffect(i, 2);

	if (pnum >= 0)
		md = M_GetDir(i);
	else
		md = monster[i]._mdir;
	monster[i]._mdir = md;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_DEATH], md);
	monster[i]._mmode = MM_DEATH;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mVar1 = 0;
	monster[i]._mx = monster[i]._moldx;
	monster[i]._my = monster[i]._moldy;
	monster[i]._mfutx = monster[i]._moldx;
	monster[i]._mfuty = monster[i]._moldy;
	M_CheckEFlag(i);
	M_ClearSquares(i);
	dMonster[monster[i]._mx][monster[i]._my] = i + 1;
	CheckQuestKill(i, sendmsg);
	M_FallenFear(monster[i]._mx, monster[i]._my);
	if (monster[i].MType->mtype >= MT_NACID && monster[i].MType->mtype <= MT_XACID)
		AddMissile(monster[i]._mx, monster[i]._my, 0, 0, 0, MIS_ACIDPUD, 1, i, monster[i]._mint + 1, 0);
}

void __fastcall M2MStartKill(int i, int mid)
{
	int md;

	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("M2MStartKill: Invalid monster (attacker) %d", i);
		app_fatal("M2MStartKill: Invalid monster (killed) %d", mid);
	}
	if (!monster[i].MType)
		app_fatal("M2MStartKill: Monster %d \"%s\" MType NULL", mid, monster[mid].mName);

	delta_kill_monster(mid, monster[mid]._mx, monster[mid]._my, currlevel);
	NetSendCmdLocParam1(FALSE, CMD_MONSTDEATH, monster[mid]._mx, monster[mid]._my, mid);

	monster[mid].mWhoHit |= 1 << i;
	if (i < 4)
		AddPlrMonstExper(monster[mid].mLevel, monster[mid].mExp, monster[mid].mWhoHit);

	monstkills[monster[mid].MType->mtype]++;
	monster[mid]._mhitpoints = 0;
	SetRndSeed(monster[mid]._mRndSeed);

	if (mid >= 4)
		SpawnItem(mid, monster[mid]._mx, monster[mid]._my, TRUE);

	if (monster[mid].MType->mtype == MT_DIABLO)
		M_DiabloDeath(mid, TRUE);
	else
		PlayEffect(i, 2);

	PlayEffect(mid, 2);

	md = (monster[i]._mdir - 4) & 7;
	if (monster[mid].MType->mtype == MT_GOLEM)
		md = 0;

	monster[mid]._mdir = md;
	NewMonsterAnim(mid, &monster[mid].MType->Anims[MA_DEATH], md);
	monster[mid]._mmode = MM_DEATH;
	monster[mid]._mxoff = 0;
	monster[mid]._myoff = 0;
	monster[mid]._mx = monster[mid]._moldx;
	monster[mid]._my = monster[mid]._moldy;
	monster[mid]._mfutx = monster[mid]._moldx;
	monster[mid]._mfuty = monster[mid]._moldy;
	M_CheckEFlag(mid);
	M_ClearSquares(mid);
	dMonster[monster[mid]._mx][monster[mid]._my] = mid + 1;
	CheckQuestKill(mid, TRUE);
	M_FallenFear(monster[mid]._mx, monster[mid]._my);
	if (monster[mid].MType->mtype >= MT_NACID && monster[mid].MType->mtype <= MT_XACID)
		AddMissile(monster[mid]._mx, monster[mid]._my, 0, 0, 0, MIS_ACIDPUD, 1, mid, monster[mid]._mint + 1, 0);
}

void __fastcall M_StartKill(int i, int pnum)
{
	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("M_StartKill: Invalid monster %d", i);
	}

	if (myplr == pnum) {
		delta_kill_monster(i, monster[i]._mx, monster[i]._my, currlevel);
		if (i != pnum) {
			NetSendCmdLocParam1(FALSE, CMD_MONSTDEATH, monster[i]._mx, monster[i]._my, i);
		} else {
			NetSendCmdLocParam1(FALSE, CMD_KILLGOLEM, monster[i]._mx, monster[i]._my, currlevel);
		}
	}

	MonstStartKill(i, pnum, TRUE);
}

void __fastcall M_SyncStartKill(int i, int x, int y, int pnum)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_SyncStartKill: Invalid monster %d", i);

	if (monster[i]._mhitpoints == 0 || monster[i]._mmode == MM_DEATH) {
		return;
	}

	if (dMonster[x][y] == 0) {
		M_ClearSquares(i);
		monster[i]._mx = x;
		monster[i]._my = y;
		monster[i]._moldx = x;
		monster[i]._moldy = y;
	}

	if (monster[i]._mmode == MM_STONE) {
		MonstStartKill(i, pnum, FALSE);
		monster[i]._mmode = MM_STONE;
	} else {
		MonstStartKill(i, pnum, FALSE);
	}
}

void __fastcall M_StartFadein(int i, int md, BOOL backwards)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_StartFadein: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_StartFadein: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_FADEIN;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	M_CheckEFlag(i);
	monster[i]._mdir = md;
	monster[i]._mFlags &= ~MFLAG_HIDDEN;
	if (backwards) {
		monster[i]._mFlags |= MFLAG_LOCK_ANIMATION;
		monster[i]._mAnimFrame = monster[i]._mAnimLen;
	}
}

void __fastcall M_StartFadeout(int i, int md, BOOL backwards)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_StartFadeout: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_StartFadeout: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_FADEOUT;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	M_CheckEFlag(i);
	monster[i]._mdir = md;
	if (backwards) {
		monster[i]._mFlags |= MFLAG_LOCK_ANIMATION;
		monster[i]._mAnimFrame = monster[i]._mAnimLen;
	}
}

void __fastcall M_StartHeal(int i)
{
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_StartHeal: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_StartHeal: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	Monst = &monster[i];
	Monst->_mAnimData = Monst->MType->Anims[MA_SPECIAL].Data[Monst->_mdir];
	Monst->_mAnimFrame = Monst->MType->Anims[MA_SPECIAL].Frames;
	Monst->_mFlags |= MFLAG_LOCK_ANIMATION;
	Monst->_mmode = MM_HEAL;
	Monst->_mVar1 = Monst->_mmaxhp / (16 * (random(97, 5) + 4));
}

void __fastcall M_ChangeLightOffset(int monst)
{
	int lx, ly, _mxoff, _myoff, sign;

	if ((DWORD)monst >= MAXMONSTERS)
		app_fatal("M_ChangeLightOffset: Invalid monster %d", monst);

	lx = monster[monst]._mxoff + 2 * monster[monst]._myoff;
	ly = 2 * monster[monst]._myoff - monster[monst]._mxoff;

	if (lx < 0) {
		sign = -1;
		lx = -lx;
	} else {
		sign = 1;
	}

	_mxoff = sign * (lx >> 3);
	if (ly < 0) {
		_myoff = -1;
		ly = -ly;
	} else {
		_myoff = 1;
	}

	ChangeLightOff(monster[monst].mlid, _mxoff, _myoff * (ly >> 3));
}

int __fastcall M_DoStand(int i)
{
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoStand: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoStand: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	Monst = &monster[i];
	if (Monst->MType->mtype == MT_GOLEM)
		Monst->_mAnimData = Monst->MType->Anims[MA_WALK].Data[Monst->_mdir];
	else
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];

	if (Monst->_mAnimFrame == Monst->_mAnimLen)
		M_Enemy(i);

	Monst->_mVar2++;

	return FALSE;
}

BOOL __fastcall M_DoWalk(int i)
{
	BOOL rv;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoWalk: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoWalk: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	rv = FALSE;
	if (monster[i]._mVar8 == monster[i].MType->Anims[MA_WALK].Frames) {
		dMonster[monster[i]._mx][monster[i]._my] = 0;
		monster[i]._mx += monster[i]._mVar1;
		monster[i]._my += monster[i]._mVar2;
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
		if (monster[i]._uniqtype != 0)
			ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
		M_StartStand(i, monster[i]._mdir);
		rv = TRUE;
	} else if (!monster[i]._mAnimCnt) {
		monster[i]._mVar8++;
		monster[i]._mVar6 += monster[i]._mxvel;
		monster[i]._mVar7 += monster[i]._myvel;
		monster[i]._mxoff = monster[i]._mVar6 >> 4;
		monster[i]._myoff = monster[i]._mVar7 >> 4;
	}

	if (monster[i]._uniqtype != 0)
		M_ChangeLightOffset(i);

	return rv;
}

BOOL __fastcall M_DoWalk2(int i)
{
	BOOL rv;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoWalk2: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoWalk2: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	if (monster[i]._mVar8 == monster[i].MType->Anims[MA_WALK].Frames) {
		dMonster[monster[i]._mVar1][monster[i]._mVar2] = 0;
		if (monster[i]._uniqtype != 0)
			ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
		M_StartStand(i, monster[i]._mdir);
		rv = TRUE;
	} else {
		if (!monster[i]._mAnimCnt) {
			monster[i]._mVar8++;
			monster[i]._mVar6 += monster[i]._mxvel;
			monster[i]._mVar7 += monster[i]._myvel;
			monster[i]._mxoff = monster[i]._mVar6 >> 4;
			monster[i]._myoff = monster[i]._mVar7 >> 4;
		}
		rv = FALSE;
	}
	if (monster[i]._uniqtype != 0)
		M_ChangeLightOffset(i);

	return rv;
}

BOOL __fastcall M_DoWalk3(int i)
{
	BOOL rv;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoWalk3: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoWalk3: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	if (monster[i]._mVar8 == monster[i].MType->Anims[MA_WALK].Frames) {
		dMonster[monster[i]._mx][monster[i]._my] = 0;
		monster[i]._mx = monster[i]._mVar1;
		monster[i]._my = monster[i]._mVar2;
		dFlags[monster[i]._mVar4][monster[i]._mVar5] &= ~DFLAG_MONSTER;
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
		if (monster[i]._uniqtype)
			ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
		M_StartStand(i, monster[i]._mdir);
		rv = TRUE;
	} else {
		if (!monster[i]._mAnimCnt) {
			monster[i]._mVar8++;
			monster[i]._mVar6 += monster[i]._mxvel;
			monster[i]._mVar7 += monster[i]._myvel;
			monster[i]._mxoff = monster[i]._mVar6 >> 4;
			monster[i]._myoff = monster[i]._mVar7 >> 4;
		}
		rv = FALSE;
	}
	if (monster[i]._uniqtype != 0)
		M_ChangeLightOffset(i);

	return rv;
}

void __fastcall M_TryM2MHit(int i, int mid, int hper, int mind, int maxd)
{
	BOOL ret;

	if ((DWORD)mid >= MAXMONSTERS) {
		app_fatal("M_TryM2MHit: Invalid monster %d", mid);
	}
	if (monster[mid].MType == NULL)
		app_fatal("M_TryM2MHit: Monster %d \"%s\" MType NULL", mid, monster[mid].mName);
	if (monster[mid]._mhitpoints >> 6 > 0 && (monster[mid].MType->mtype != MT_ILLWEAV || monster[mid]._mgoal != MGOAL_RETREAT)) {
		int hit = random(4, 100);
		if (monster[mid]._mmode == MM_STONE)
			hit = 0;
		if (!CheckMonsterHit(mid, &ret) && hit < hper) {
			int dam = (mind + random(5, maxd - mind + 1)) << 6;
			monster[mid]._mhitpoints -= dam;
			if (monster[mid]._mhitpoints >> 6 <= 0) {
				if (monster[mid]._mmode == MM_STONE) {
					M2MStartKill(i, mid);
					monster[mid]._mmode = MM_STONE;
				} else {
					M2MStartKill(i, mid);
				}
			} else {
				if (monster[mid]._mmode == MM_STONE) {
					M2MStartHit(mid, i, dam);
					monster[mid]._mmode = MM_STONE;
				} else {
					M2MStartHit(mid, i, dam);
				}
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
	int v12;     // ecx
	int v13;     // edi
	int v14;     // eax
	int v15;     // eax
	int *v16;    // ecx
	int v17;     // eax
	int v18;     // edi
	int v19;     // edx
	int v20;     // eax
	int v21;     // eax
	int v22;     // edx
	int v23;     // eax
	BOOLEAN v24; // zf
	BOOLEAN v25; // sf
	//unsigned char v26; // of
	int v27;     // eax
	int v29;     // edi
	int v30;     // eax
	int v31;     // eax
	int v32;     // eax
	int v33;     // edi
	int v34;     // ebx
	int v35;     // edx
	int v36;     // [esp+Ch] [ebp-Ch]
	int arglist; // [esp+10h] [ebp-8h]
	int plr_num; // [esp+14h] [ebp-4h]
	int hper;    // [esp+20h] [ebp+8h]

	v5 = i;
	plr_num = pnum;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_TryH2HHit: Invalid monster %d", i);
	v6 = v5;
	if (monster[v5].MType == NULL)
		app_fatal("M_TryH2HHit: Monster %d \"%s\" MType NULL", v5, monster[v6].mName);
	if (monster[v6]._mFlags & MFLAG_TARGETS_MONSTER) {
		M_TryM2MHit(v5, plr_num, Hit, MinDam, MaxDam);
		return;
	}
	v7 = plr_num;
	if (plr[plr_num]._pHitPoints >> 6 > 0 && !plr[v7]._pInvincible && !(plr[v7]._pSpellFlags & 1)) {
		v8 = abs(monster[v6]._mx - plr[v7].WorldX);
		v9 = abs(monster[v6]._my - plr[v7].WorldY);
		//v11 = v10;
		if (v8 < 2 && v9 < 2) {
			v36 = random(98, 100);
#ifdef _DEBUG
			if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
				v36 = 1000;
#endif
			v12 = 5;
			v13 = Hit
			    + 2 * (SLOBYTE(monster[v6].mLevel) - plr[v7]._pLevel)
			    + 30
			    - plr[v7]._pIBonusAC
			    - plr[v7]._pIAC
			    - plr[v7]._pDexterity / 5;
			if (v13 < 15)
				v13 = 15;
			if (currlevel == 14) {
				if (v13 >= 20)
					goto LABEL_23;
				v13 = 20;
			}
			if (currlevel != 15) {
			LABEL_20:
				if (currlevel == 16 && v13 < 30)
					v13 = 30;
				goto LABEL_23;
			}
			if (v13 < 25) {
				v13 = 25;
				goto LABEL_20;
			}
		LABEL_23:
			v14 = plr[v7]._pmode;
			if (v14 && v14 != 4 || !plr[v7]._pBlockFlag) {
				v15 = 100;
			} else {
				v15 = random(98, 100);
			}
			v16 = (int *)(plr[v7]._pDexterity
			    + plr[v7]._pBaseToBlk
			    - 2 * SLOBYTE(monster[v6].mLevel)
			    + 2 * plr[v7]._pLevel);
			if ((signed int)v16 < 0)
				v16 = 0;
			if ((signed int)v16 > 100)
				v16 = (int *)100;
			if (v36 < v13) {
				if (v15 >= (signed int)v16) {
					if (monster[v6].MType->mtype == MT_YZOMBIE && plr_num == myplr) {
						v18 = -1;
						v19 = 0;
						for (hper = -1; v19 < nummissiles; ++v19) {
							v20 = missileactive[v19];
							if (missile[v20]._mitype == MIS_MANASHIELD) {
								if (missile[v20]._misource == plr_num) {
									v18 = missileactive[v19];
									hper = missileactive[v19];
								} else {
									v18 = hper;
								}
							}
						}
						v16 = &plr[v7]._pMaxHP;
						v21 = plr[v7]._pMaxHP;
						if (v21 > 64) {
							v22 = plr[v7]._pMaxHPBase;
							if (v22 > 64) {
								v23 = v21 - 64;
								//v26 = __OFSUB__(plr[v7]._pHitPoints, v23);
								v24 = plr[v7]._pHitPoints == v23;
								v25 = plr[v7]._pHitPoints - v23 < 0;
								*v16 = v23;
								if (!(v25 | v24)) { //if (!((unsigned char)(v25 ^ v26) | v24)) {
									plr[v7]._pHitPoints = v23;
									if (v18 >= 0)
										missile[v18]._miVar1 = v23;
								}
								v16 = &plr[v7]._pHPBase;
								v27 = v22 - 64;
								plr[v7]._pMaxHPBase = v22 - 64;
								if (plr[v7]._pHPBase > v22 - 64) {
									*v16 = v27;
									if (v18 >= 0)
										missile[v18]._miVar2 = v27;
								}
							}
						}
					}
					v29 = (plr[v7]._pIGetHit << 6) + (MinDam << 6) + random(99, (MaxDam - MinDam + 1) << 6);
					if (v29 < 64)
						v29 = 64;
					if (plr_num == myplr) {
						plr[v7]._pHitPoints -= v29;
						plr[v7]._pHPBase -= v29;
					}
					if (plr[v7]._pIFlags & ISPL_FASTERRECOVER) {
						v30 = (random(99, 3) + 1) << 6;
						monster[v6]._mhitpoints -= v30;
						if (monster[v6]._mhitpoints >> 6 > 0)
							M_StartHit(arglist, plr_num, v30);
						else
							M_StartKill(arglist, plr_num);
					}
					if (!(monster[v6]._mFlags & MFLAG_NOLIFESTEAL) && monster[v6].MType->mtype == MT_SKING && gbMaxPlayers != 1)
						monster[v6]._mhitpoints += v29;
					v31 = plr[v7]._pMaxHP;
					if (plr[v7]._pHitPoints > v31) {
						plr[v7]._pHitPoints = v31;
						plr[v7]._pHPBase = plr[v7]._pMaxHPBase;
					}
					if (plr[v7]._pHitPoints >> 6 > 0) {
						StartPlrHit(plr_num, v29, 0);
						if (monster[v6]._mFlags & MFLAG_KNOCKBACK) {
							if (plr[v7]._pmode != PM_GOTHIT)
								StartPlrHit(plr_num, 0, 1u);
							v32 = monster[v6]._mdir;
							v33 = plr[v7].WorldX + offset_x[v32];
							v34 = plr[v7].WorldY + offset_y[v32];
							if (PosOkPlayer(plr_num, v33, v34)) {
								v35 = plr[v7]._pdir;
								plr[v7].WorldX = v33;
								plr[v7].WorldY = v34;
								FixPlayerLocation(plr_num, v35);
								FixPlrWalkTags(plr_num);
								dPlayer[v33][v34] = plr_num + 1;
								SetPlayerOld(plr_num);
							}
						}
					} else {
						SyncPlrKill(plr_num, 0);
					}
				} else {
					v17 = GetDirection(plr[v7].WorldX, plr[v7].WorldY, monster[v6]._mx, monster[v6]._my);
					StartPlrBlock(plr_num, v17);
				}
			}
			return;
		}
	}
}
// 679660: using guessed type char gbMaxPlayers;

BOOL __fastcall M_DoAttack(int i)
{
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoAttack: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->MType == NULL)
		app_fatal("M_DoAttack: Monster %d \"%s\" MType NULL", i, Monst->mName);
	if (Monst->MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoAttack: Monster %d \"%s\" MData NULL", i, Monst->mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit, monster[i].mMinDamage, monster[i].mMaxDamage);
		if (monster[i]._mAi != AI_SNAKE)
			PlayEffect(i, 0);
	}
	if (monster[i].MType->mtype >= MT_NMAGMA && monster[i].MType->mtype <= MT_WMAGMA && monster[i]._mAnimFrame == 9) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit + 10, monster[i].mMinDamage - 2, monster[i].mMaxDamage - 2);
		PlayEffect(i, 0);
	}
	if (monster[i].MType->mtype >= MT_STORM && monster[i].MType->mtype <= MT_MAEL && monster[i]._mAnimFrame == 13) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit - 20, monster[i].mMinDamage + 4, monster[i].mMaxDamage + 4);
		PlayEffect(i, 0);
	}
	if (monster[i]._mAi == AI_SNAKE && monster[i]._mAnimFrame == 1)
		PlayEffect(i, 0);
	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL __fastcall M_DoRAttack(int i)
{
	int multimissiles, mi;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoRAttack: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoRAttack: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	if (monster[i].MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoRAttack: Monster %d \"%s\" MData NULL", i, monster[i].mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum) {
		if (monster[i]._mVar1 != -1) {
			if (monster[i]._mVar1 != MIS_CBOLT)
				multimissiles = 1;
			else
				multimissiles = 3;
			for (mi = 0; mi < multimissiles; mi++) {
				AddMissile(
				    monster[i]._mx,
				    monster[i]._my,
				    monster[i]._menemyx,
				    monster[i]._menemyy,
				    monster[i]._mdir,
				    monster[i]._mVar1,
				    1,
				    i,
				    monster[i]._mVar2,
				    0);
			}
		}
		PlayEffect(i, 0);
	}

	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

int __fastcall M_DoRSpAttack(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoRSpAttack: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoRSpAttack: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	if (monster[i].MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoRSpAttack: Monster %d \"%s\" MData NULL", i, monster[i].mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum2 && !monster[i]._mAnimCnt) {
		AddMissile(
		    monster[i]._mx,
		    monster[i]._my,
		    monster[i]._menemyx,
		    monster[i]._menemyy,
		    monster[i]._mdir,
		    monster[i]._mVar1,
		    1,
		    i,
		    monster[i]._mVar3,
		    0);
		PlayEffect(i, 3);
	}

	if (monster[i]._mAi == AI_MEGA && monster[i]._mAnimFrame == 3) {
		int hadV2 = monster[i]._mVar2;
		monster[i]._mVar2++;
		if (hadV2 == 0) {
			monster[i]._mFlags |= MFLAG_ALLOW_SPECIAL;
		} else if (monster[i]._mVar2 == 15) {
			monster[i]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
	}

	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL __fastcall M_DoSAttack(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoSAttack: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoSAttack: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	if (monster[i].MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoSAttack: Monster %d \"%s\" MData NULL", i, monster[i].mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum2)
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit2, monster[i].mMinDamage2, monster[i].mMaxDamage2);

	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL __fastcall M_DoFadein(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoFadein: Invalid monster %d", i);

	if ((!(monster[i]._mFlags & MFLAG_LOCK_ANIMATION) || monster[i]._mAnimFrame != 1)
	    && (monster[i]._mFlags & MFLAG_LOCK_ANIMATION || monster[i]._mAnimFrame != monster[i]._mAnimLen)) {
		return FALSE;
	}

	M_StartStand(i, monster[i]._mdir);
	monster[i]._mFlags &= ~MFLAG_LOCK_ANIMATION;

	return TRUE;
}

BOOL __fastcall M_DoFadeout(int i)
{
	int mt;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoFadeout: Invalid monster %d", i);

	if ((!(monster[i]._mFlags & MFLAG_LOCK_ANIMATION) || monster[i]._mAnimFrame != 1)
	    && (monster[i]._mFlags & MFLAG_LOCK_ANIMATION || monster[i]._mAnimFrame != monster[i]._mAnimLen)) {
		return FALSE;
	}

	mt = monster[i].MType->mtype;
	if (mt < MT_INCIN || mt > MT_HELLBURN) {
		monster[i]._mFlags &= ~MFLAG_LOCK_ANIMATION;
		monster[i]._mFlags |= MFLAG_HIDDEN;
	} else {
		monster[i]._mFlags &= ~MFLAG_LOCK_ANIMATION;
	}

	M_StartStand(i, monster[i]._mdir);

	return TRUE;
}

int __fastcall M_DoHeal(int i)
{
	int v1;  // esi
	int v2;  // eax
	int v3;  // esi
	int *v4; // edx
	int v5;  // ecx
	int v6;  // edi
	int v7;  // edi
	int v8;  // esi

	v1 = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoHeal: Invalid monster %d", i);
	v2 = v1;
	if (monster[v1]._mFlags & MFLAG_NOHEAL) {
		monster[v2]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		monster[v2]._mmode = MM_SATTACK;
	} else if (monster[v2]._mAnimFrame == 1) {
		v3 = monster[v2]._mVar1;
		v4 = &monster[v2]._mhitpoints;
		v5 = monster[v2]._mFlags & ~MFLAG_LOCK_ANIMATION | MFLAG_ALLOW_SPECIAL;
		v6 = monster[v2]._mhitpoints;
		monster[v2]._mFlags = v5;
		v7 = v3 + v6;
		v8 = monster[v2]._mmaxhp;
		if (v7 >= v8) {
			*v4 = v8;
			monster[v2]._mFlags = v5 & ~MFLAG_ALLOW_SPECIAL;
			monster[v2]._mmode = MM_SATTACK;
		} else {
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
	int v4;   // eax
	int v5;   // edx
	int v6;   // ecx
	char v7;  // bl
	int v8;   // eax
	char *v9; // eax

	v1 = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoTalk: Invalid monster %d", i);
	v2 = v1;
	M_StartStand(v1, monster[v1]._mdir);
	_LOBYTE(monster[v1]._mgoal) = MGOAL_TALKING;
	//_LOBYTE(v3) = effect_is_playing(alltext[monster[v1].mtalkmsg].sfxnr);
	if (!effect_is_playing(alltext[monster[v1].mtalkmsg].sfxnr)) {
		InitQTextMsg(monster[v2].mtalkmsg);
		if (monster[v2].mName == UniqMonst[0].mName) {
			v4 = monster[v2].mtalkmsg;
			if (v4 == QUEST_GARBUD1)
				quests[QTYPE_GARB]._qactive = 2;
			quests[QTYPE_GARB]._qlog = 1;
			if (v4 == QUEST_GARBUD2 && !(monster[v2]._mFlags & MFLAG_QUEST_COMPLETE)) {
				SpawnItem(v1, monster[v2]._mx + 1, monster[v2]._my + 1, 1u);
				monster[v2]._mFlags |= MFLAG_QUEST_COMPLETE;
			}
		}
		if (monster[v2].mName == UniqMonst[2].mName
		    && monster[v2].mtalkmsg == QUEST_ZHAR1
		    && !(monster[v2]._mFlags & MFLAG_QUEST_COMPLETE)) {
			v5 = monster[v2]._my + 1;
			v6 = monster[v2]._mx + 1;
			quests[QTYPE_ZHAR]._qactive = 2;
			quests[QTYPE_ZHAR]._qlog = 1;
			CreateTypeItem(v6, v5, 0, 0, 24, 1, 0);
			monster[v2]._mFlags |= MFLAG_QUEST_COMPLETE;
		}
		if (monster[v2].mName == UniqMonst[3].mName) {
			if (monster[v2].mtalkmsg == QUEST_BANNER10 && !(monster[v2]._mFlags & MFLAG_QUEST_COMPLETE)) {
				ObjChangeMap(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 2, (setpc_h >> 1) + setpc_y - 2);
				v7 = TransVal;
				TransVal = 9;
				DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 4, setpc_y + (setpc_h >> 1));
				TransVal = v7;
				quests[QTYPE_BOL]._qvar1 = 2;
				if (quests[QTYPE_BOL]._qactive == 1)
					quests[QTYPE_BOL]._qactive = 2;
				monster[v2]._mFlags |= MFLAG_QUEST_COMPLETE;
			}
			if (quests[QTYPE_BOL]._qvar1 < 2u) {
				sprintf(tempstr, "SS Talk = %i, Flags = %i", monster[v2].mtalkmsg, monster[v2]._mFlags);
				app_fatal(tempstr);
			}
		}
		if (monster[v2].mName == UniqMonst[7].mName) {
			v8 = monster[v2].mtalkmsg;
			if (v8 == QUEST_VEIL9) {
				quests[QTYPE_VEIL]._qactive = 2;
				quests[QTYPE_VEIL]._qlog = 1;
			}
			if (v8 == QUEST_VEIL11 && !(monster[v2]._mFlags & MFLAG_QUEST_COMPLETE)) {
				SpawnUnique(UITEM_STEELVEIL, monster[v2]._mx + 1, monster[v2]._my + 1);
				monster[v2]._mFlags |= MFLAG_QUEST_COMPLETE;
			}
		}
		v9 = monster[v2].mName;
		if (v9 == UniqMonst[8].mName)
			quests[QTYPE_WARLRD]._qvar1 = 2;
		if (v9 == UniqMonst[4].mName && gbMaxPlayers != 1) {
			monster[v2]._msquelch = -1;
			monster[v2].mtalkmsg = 0;
			quests[QTYPE_VB]._qvar1 = 6;
			_LOBYTE(monster[v2]._mgoal) = MGOAL_NORMAL;
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
	BOOL tren;
	MonsterStruct *Monst;
	int k, j, x, y, _mx, _my, rx, ry;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_Teleport: Invalid monster %d", i);

	tren = FALSE;

	Monst = &monster[i];
	if (Monst->_mmode != MM_STONE) {
		_mx = Monst->_menemyx;
		_my = Monst->_menemyy;
		rx = 2 * random(100, 2) - 1;
		ry = 2 * random(100, 2) - 1;

		for (j = -1; j <= 1 && !tren; j++) {
			for (k = -1; k < 1 && !tren; k++) {
				if (j != 0 || k != 0) {
					x = _mx + rx * j;
					y = _my + ry * k;
					if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && x != Monst->_mx && y != Monst->_my) {
						if (PosOkMonst(i, x, y))
							tren = TRUE;
					}
				}
			}
		}
	}

	if (tren) {
		M_ClearSquares(i);
		dMonster[Monst->_mx][Monst->_my] = 0;
		dMonster[x][y] = i + 1;
		Monst->_moldx = x;
		Monst->_moldy = y;
		Monst->_mdir = M_GetDir(i);
		M_CheckEFlag(i);
	}
}

BOOL __fastcall M_DoGotHit(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoGotHit: Invalid monster %d", i);

	if (monster[i].MType == NULL)
		app_fatal("M_DoGotHit: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);

		return TRUE;
	}

	return FALSE;
}

void __fastcall M_UpdateLeader(int i)
{
	int ma, j;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_UpdateLeader: Invalid monster %d", i);

	for (j = 0; j < nummonsters; j++) {
		ma = monstactive[j];
		if (monster[ma].leaderflag == 1 && monster[ma].leader == i)
			monster[ma].leaderflag = 0;
	}

	if (monster[i].leaderflag == 1) {
		monster[monster[i].leader].packsize--;
	}
}

void __cdecl DoEnding()
{
	BOOL bMusicOn;
	int musicVolume;

	if (gbMaxPlayers > 1) {
		SNetLeaveGame(0x40000004);
	}

	music_stop();

	if (gbMaxPlayers > 1) {
		Sleep(1000);
	}

	if (plr[myplr]._pClass == PC_WARRIOR) {
		play_movie("gendata\\DiabVic2.smk", 0);
	} else if (plr[myplr]._pClass == PC_SORCERER) {
		play_movie("gendata\\DiabVic1.smk", 0);
	} else {
		play_movie("gendata\\DiabVic3.smk", 0);
	}
	play_movie("gendata\\Diabend.smk", 0);

	bMusicOn = gbMusicOn;
	gbMusicOn = TRUE;

	musicVolume = sound_get_or_set_music_volume(1);
	sound_get_or_set_music_volume(0);

	music_start(2);
	loop_movie = TRUE;
	play_movie("gendata\\loopdend.smk", 1);
	loop_movie = FALSE;
	music_stop();

	sound_get_or_set_music_volume(musicVolume);
	gbMusicOn = bMusicOn;
}
// 679660: using guessed type char gbMaxPlayers;

void __cdecl PrepDoEnding()
{
	int newKillLevel, i;
	DWORD *killLevel;

	gbSoundOn = sgbSaveSoundOn;
	gbRunGame = FALSE;
	deathflag = FALSE;
	cineflag = TRUE;

	killLevel = &plr[myplr].pDiabloKillLevel;
	newKillLevel = gnDifficulty + 1;
	if (*killLevel > newKillLevel)
		newKillLevel = *killLevel;
	plr[myplr].pDiabloKillLevel = newKillLevel;

	for (i = 0; i < MAX_PLRS; i++) {
		plr[i]._pmode = PM_QUIT;
		plr[i]._pBlockFlag = TRUE;
		if (gbMaxPlayers > 1) {
			if (plr[i]._pHitPoints >> 6 == 0)
				plr[i]._pHitPoints = 64;
			if (plr[i]._pMana >> 6 == 0)
				plr[i]._pMana = 64;
		}
	}
}
// 525718: using guessed type char cineflag;
// 64D32C: using guessed type int sgbSaveSoundOn;
// 679660: using guessed type char gbMaxPlayers;

BOOL __fastcall M_DoDeath(int i)
{
	int var1;
	int x, y;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoDeath: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoDeath: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	monster[i]._mVar1++;
	var1 = monster[i]._mVar1;
	if (monster[i].MType->mtype == MT_DIABLO) {
		x = monster[i]._mx - ViewX;
		if (x < 0)
			x = -1;
		else
			x = x > 0;
		ViewX += x;

		y = monster[i]._my - ViewY;
		if (y < 0) {
			y = -1;
		} else {
			y = y > 0;
		}
		ViewY += y;

		if (var1 == 140)
			PrepDoEnding();
	} else if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		if (monster[i]._uniqtype == 0)
			AddDead(monster[i]._mx, monster[i]._my, monster[i].MType->mdeadval, (direction)monster[i]._mdir);
		else
			AddDead(monster[i]._mx, monster[i]._my, monster[i]._udeadval, (direction)monster[i]._mdir);

		monster[i]._mDelFlag = TRUE;
		dMonster[monster[i]._mx][monster[i]._my] = 0;

		M_UpdateLeader(i);
	}
	return FALSE;
}

BOOL __fastcall M_DoSpStand(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoSpStand: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoSpStand: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum2)
		PlayEffect(i, 3);

	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL __fastcall M_DoDelay(int i)
{
	int mVar2;
	int oFrame;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoDelay: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoDelay: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	monster[i]._mAnimData = monster[i].MType->Anims[MA_STAND].Data[M_GetDir(i)];
	if (monster[i]._mAi == AI_LAZURUS) {
		if (monster[i]._mVar2 > 8 || monster[i]._mVar2 < 0)
			monster[i]._mVar2 = 8;
	}

	mVar2 = monster[i]._mVar2;
	monster[i]._mVar2--;

	if (!mVar2) {
		oFrame = monster[i]._mAnimFrame;
		M_StartStand(i, monster[i]._mdir);
		monster[i]._mAnimFrame = oFrame;
		return TRUE;
	}

	return FALSE;
}

BOOL __fastcall M_DoStone(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoStone: Invalid monster %d", i);

	if (!monster[i]._mhitpoints) {
		dMonster[monster[i]._mx][monster[i]._my] = 0;
		monster[i]._mDelFlag = TRUE;
	}

	return FALSE;
}

void __fastcall M_WalkDir(int i, int md)
{
	int mwi;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_WalkDir: Invalid monster %d", i);

	mwi = monster[i].MType->Anims[MA_WALK].Frames - 1;
	switch (md) {
	case DIR_N:
		M_StartWalk(i, 0, -MWVel[mwi][1], -1, -1, DIR_N);
		break;
	case DIR_NE:
		M_StartWalk(i, MWVel[mwi][1], -MWVel[mwi][0], 0, -1, DIR_NE);
		break;
	case DIR_E:
		M_StartWalk3(i, MWVel[mwi][2], 0, -32, -16, 1, -1, 1, 0, DIR_E);
		break;
	case DIR_SE:
		M_StartWalk2(i, MWVel[mwi][1], MWVel[mwi][0], -32, -16, 1, 0, DIR_SE);
		break;
	case DIR_S:
		M_StartWalk2(i, 0, MWVel[mwi][1], 0, -32, 1, 1, DIR_S);
		break;
	case DIR_SW:
		M_StartWalk2(i, -MWVel[mwi][1], MWVel[mwi][0], 32, -16, 0, 1, DIR_SW);
		break;
	case DIR_W:
		M_StartWalk3(i, -MWVel[mwi][2], 0, 32, -16, -1, 1, 0, 1, DIR_W);
		break;
	case DIR_NW:
		M_StartWalk(i, -MWVel[mwi][1], -MWVel[mwi][0], -1, 0, DIR_NW);
		break;
	}
}

void __fastcall GroupUnity(int i)
{
	int v1;           // ebx
	int v2;           // esi
	int v3;           // ebp
	int v4;           // edi
	BOOLEAN v5;       // eax
	int v6;           // eax
	unsigned char v8; // al
	int v9;           // ebp
	int j;            // edi
	int v11;          // eax

	v1 = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("GroupUnity: Invalid monster %d", i);
	v2 = v1;
	if (monster[v1].leaderflag) {
		v3 = (unsigned char)monster[v2].leader;
		v4 = v3;
		v5 = LineClearF(
		    CheckNoSolid,
		    monster[v2]._mx,
		    monster[v2]._my,
		    monster[v4]._mfutx,
		    monster[v4]._mfuty);
		if (v5) {
			if (monster[v2].leaderflag == 2
			    && abs(monster[v2]._mx - monster[v4]._mfutx) < 4
			    && abs(monster[v2]._my - monster[v4]._mfuty) < 4) {
				++monster[v4].packsize;
				monster[v2].leaderflag = 1;
			}
		} else {
			if (monster[v2].leaderflag != 1)
				goto LABEL_18;
			--monster[v4].packsize;
			monster[v2].leaderflag = 2;
		}
	} else {
		v3 = 0; /* v13 */
	}
	if (monster[v2].leaderflag == 1) {
		v6 = v3;
		if (monster[v2]._msquelch > monster[v3]._msquelch) {
			monster[v6]._lastx = monster[v2]._mx;
			monster[v6]._lasty = monster[v2]._my;
			monster[v6]._msquelch = monster[v2]._msquelch - 1;
		}
		if (monster[v6]._mAi == AI_GARG) {
			if (monster[v6]._mFlags & MFLAG_ALLOW_SPECIAL) {
				monster[v6]._mmode = MM_SATTACK;
				monster[v6]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
			}
		}
		return;
	}
LABEL_18:
	v8 = monster[v2]._uniqtype;
	if (v8 != 0) {
		if (UniqMonst[v8 - 1].mUnqAttr & 2) {
			v9 = nummonsters;
			for (j = 0; j < v9; ++j) {
				v11 = monstactive[j];
				if (monster[v11].leaderflag == 1 && (unsigned char)monster[v11].leader == v1) {
					if (monster[v2]._msquelch > monster[v11]._msquelch) {
						monster[v11]._lastx = monster[v2]._mx;
						monster[v11]._lasty = monster[v2]._my;
						monster[v11]._msquelch = monster[v2]._msquelch - 1;
					}
					if (monster[v11]._mAi == AI_GARG) {
						if (monster[v11]._mFlags & MFLAG_ALLOW_SPECIAL) {
							monster[v11]._mmode = MM_SATTACK;
							monster[v11]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
						}
					}
				}
			}
		}
	}
}

BOOL __fastcall M_CallWalk(int i, int md)
{
	int v2; // esi
	int v3; // edi
	int v4; // ebp
	//int v5; // eax
	BOOLEAN v7; // ebx
	int v9;     // ebx
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
	if (random(101, 2)) {
		if (v7)
			goto LABEL_10;
		v9 = v2;
		v2 = left[v2];
		//_LOBYTE(v10) = DirOK(v3, v2);
		if (DirOK(v3, v2))
			goto LABEL_10;
		v2 = right[v9];
	} else {
		if (v7)
			goto LABEL_10;
		v11 = v2;
		v2 = right[v2];
		//_LOBYTE(v12) = DirOK(v3, v2);
		if (DirOK(v3, v2))
			goto LABEL_10;
		v2 = left[v11];
	}
	//_LOBYTE(v13) = DirOK(v3, v2);
	if (!DirOK(v3, v2)) {
		v14 = 0;
		goto LABEL_11;
	}
LABEL_10:
	v14 = 1;
LABEL_11:
	if (random(102, 2)) {
		if (v14)
			goto LABEL_20;
		v2 = right[right[v4]];
		//_LOBYTE(v15) = DirOK(v3, v2);
		if (DirOK(v3, v2))
			goto LABEL_20;
		v2 = left[left[v4]];
	} else {
		if (v14)
			goto LABEL_20;
		v2 = left[left[v4]];
		//_LOBYTE(v16) = DirOK(v3, v2);
		if (DirOK(v3, v2))
			goto LABEL_20;
		v2 = right[right[v4]];
	}
	//_LOBYTE(v17) = DirOK(v3, v2);
	if (DirOK(v3, v2)) {
	LABEL_20:
		v18 = 1;
		M_WalkDir(v3, v2);
		return v18;
	}
	return 0;
}

BOOL __fastcall M_PathWalk(int i)
{
	char path[25];
	BOOL(__fastcall * Check)
	(int, int, int);

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_PathWalk: Invalid monster %d", i);

	Check = PosOkMonst3;
	if (!(monster[i]._mFlags & MFLAG_CAN_OPEN_DOOR))
		Check = PosOkMonst;

	if (FindPath(Check, i, monster[i]._mx, monster[i]._my, monster[i]._menemyx, monster[i]._menemyy, path)) {
		M_CallWalk(i, plr2monst[path[0]]); /* plr2monst is local */
		return TRUE;
	}

	return FALSE;
}

BOOL __fastcall M_CallWalk2(int i, int md)
{
	BOOL ok;
	int mdtemp;

	mdtemp = md;
	ok = DirOK(i, md);    // Can we continue in the same direction
	if (random(101, 2)) { // Randomly go left or right
		ok = ok || (mdtemp = left[md], DirOK(i, left[md])) || (mdtemp = right[md], DirOK(i, right[md]));
	} else {
		ok = ok || (mdtemp = right[md], DirOK(i, right[md])) || (mdtemp = left[md], DirOK(i, left[md]));
	}

	if (ok)
		M_WalkDir(i, mdtemp);

	return ok;
}

BOOL __fastcall M_DumbWalk(int i, int md)
{
	BOOL ok;
	ok = DirOK(i, md);
	if (ok)
		M_WalkDir(i, md);

	return ok;
}

BOOLEAN __fastcall M_RoundWalk(int i, int md, int *dir)
{
	int *v3; // ebp
	int v4;  // ebx
	int v5;  // esi
	//int v6; // eax
	BOOLEAN v7; // di
	int v8;     // edi
	//int v9; // eax
	//int v10; // eax
	int *v11; // ebp
	//int v12; // eax
	//int v13; // eax

	v3 = dir;
	v4 = i;
	if (*dir)
		v5 = left[left[md]];
	else
		v5 = right[right[md]];
	//_LOBYTE(v6) = DirOK(i, v5);
	v7 = DirOK(i, v5);
	if (v7)
		goto LABEL_12;
	v8 = v5;
	if (!*dir) {
		v11 = &left[v8];
		v5 = left[v8];
		//_LOBYTE(v12) = DirOK(v4, left[v8]);
		if (DirOK(v4, left[v8]))
			goto LABEL_11;
		v5 = left[*v11];
		//_LOBYTE(v13) = DirOK(v4, left[*v11]);
		if (DirOK(v4, left[*v11]))
			goto LABEL_11;
		v3 = dir;
	LABEL_14:
		*v3 = *v3 == 0;
		return M_CallWalk(v4, opposite[v8]);
	}
	v5 = right[v8];
	//_LOBYTE(v9) = DirOK(v4, right[v8]);
	if (!DirOK(v4, right[v8])) {
		v5 = right[right[v8]];
		//_LOBYTE(v10) = DirOK(v4, v5);
		if (!DirOK(v4, v5))
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
	MonsterStruct *Monst;
	int mx, my;
	int md, v;

	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("MAI_Zombie: Invalid monster %d", i);
	}

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	if (!(dFlags[mx][my] & DFLAG_VISIBLE)) {
		return;
	}

	mx = mx - Monst->_menemyx;
	my = my - Monst->_menemyy;
	md = Monst->_mdir;
	v = random(103, 100);
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if (v < 2 * Monst->_mint + 10) {
			if (abs(mx) >= 2 * Monst->_mint + 4 || abs(my) >= 2 * Monst->_mint + 4) {
				if (random(104, 100) < 2 * Monst->_mint + 20) {
					md = random(104, 8);
				}
				M_DumbWalk(i, md);
			} else {
				md = M_GetDir(i);
				M_CallWalk(i, md);
			}
		}
	} else if (v < 2 * Monst->_mint + 10) {
		M_StartAttack(i);
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
}

void __fastcall MAI_SkelSd(int i)
{
	MonsterStruct *Monst;
	int mx, my, x, y, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_SkelSd: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || !Monst->_msquelch) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	x = mx - Monst->_menemyx;
	y = my - Monst->_menemyy;
	md = GetDirection(mx, my, Monst->_lastx, Monst->_lasty);
	Monst->_mdir = md;
	if (abs(x) >= 2 || abs(y) >= 2) {
		if (Monst->_mVar1 == MM_DELAY || (random(106, 100) >= 35 - 4 * Monst->_mint)) {
			M_CallWalk(i, md);
		} else {
			M_StartDelay(i, 15 - 2 * Monst->_mint + random(106, 10));
		}
	} else {
		if (Monst->_mVar1 == MM_DELAY || (random(105, 100) < 2 * Monst->_mint + 20)) {
			M_StartAttack(i);
		} else {
			M_StartDelay(i, 2 * (5 - Monst->_mint) + random(105, 10));
		}
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

BOOL __fastcall MAI_Path(int i)
{
	MonsterStruct *Monst;
	BOOL clear;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Path: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->MType->mtype != MT_GOLEM) {
		if (!Monst->_msquelch)
			return FALSE;
		if (Monst->_mmode != MM_STAND)
			return FALSE;
		if (Monst->_mgoal != MGOAL_NORMAL && Monst->_mgoal != MGOAL_MOVE && Monst->_mgoal != MGOAL_SHOOT)
			return FALSE;
		if (Monst->_mx == 1 && Monst->_my == 0)
			return FALSE;
	}

	clear = LineClearF1(
	    PosOkMonst2,
	    i,
	    Monst->_mx,
	    Monst->_my,
	    Monst->_menemyx,
	    Monst->_menemyy);
	if (!clear || Monst->_pathcount >= 5 && Monst->_pathcount < 8) {
		if (Monst->_mFlags & MFLAG_CAN_OPEN_DOOR)
			MonstCheckDoors(i);
		Monst->_pathcount++;
		if (Monst->_pathcount < 5)
			return FALSE;
		if (M_PathWalk(i))
			return TRUE;
	}

	if (Monst->MType->mtype != MT_GOLEM)
		Monst->_pathcount = 0;

	return FALSE;
}

void __fastcall MAI_Snake(int i)
{
	int esi1;            // esi
	MonsterStruct *esi3; // esi
	BOOLEAN v3;          // zf
	int v4;              // ecx
	int v5;              // eax
	int v6;              // ST1C_4
	int v7;              // edi
	int v8;              // edx
	int v9;              // ST18_4
	int v10;             // ebx
	int v11;             // eax
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
	int v24;         // ebx
	int v26;         // ecx
	int v27;         // eax
	int v28;         // ecx
	int v29;         // ecx
	int v30;         // eax
	int v31;         // edx
	int v32;         // eax
	int v33;         // ecx
	int v34;         // ecx
	int v35;         // eax
	char pattern[6]; // [esp+4h] [ebp-1Ch]
	int micaster;    // [esp+Ch] [ebp-14h]
	int midir;       // [esp+10h] [ebp-10h]
	int v1;          // [esp+14h] [ebp-Ch]
	int v2;          // [esp+18h] [ebp-8h]
	int arglist;     // [esp+1Ch] [ebp-4h]

	esi1 = i;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Snake: Invalid monster %d", i);
	pattern[2] = 0;
	pattern[3] = -1;
	pattern[4] = -1;
	pattern[5] = 0;
	esi3 = &monster[esi1];
	pattern[0] = 1;
	pattern[1] = 1;
	v3 = esi3->_mmode == MM_STAND;
	micaster = esi3->_menemy;
	if (v3 && esi3->_msquelch) {
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
		if (abs(v7) < 2) {
			v11 = abs(v10);
			//v13 = v12;
			if (v11 < 2) {
				v14 = esi3->_mVar1;
				if (v14 == MM_DELAY
				    || v14 == MM_CHARGE
				    || (v15 = random(105, 100), v16 = (unsigned char)esi3->_mint + 20, v15 < v16)) {
					M_StartAttack(arglist);
				LABEL_49:
					if (esi3->_mmode == MM_STAND)
						esi3->_mAnimData = esi3->MType->Anims[MA_STAND].Data[esi3->_mdir];
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
		if (v19 >= 3 || (v22 = abs(v10), v22 >= 3)) /* v21 = v23,  */
		{
			v24 = arglist;
		} else {
			v24 = arglist;
			if (LineClearF1(PosOkMonst, arglist, esi3->_mx, esi3->_my, v1, v2) && esi3->_mVar1 != MM_CHARGE) {
				if (AddMissile(esi3->_mx, esi3->_my, v1, v2, midir, MIS_RHINO, micaster, arglist, 0, 0) != -1) {
					PlayEffect(arglist, 0);
					v26 = esi3->_my + 112 * esi3->_mx;
					esi3->_mmode = MM_CHARGE;
					dMonster[0][v26] = -1 - v24;
				}
				goto LABEL_49;
			}
		}
		if (esi3->_mVar1 != MM_DELAY) {
			v27 = random(106, 100);
			v28 = 2 * (unsigned char)esi3->_mint;
			if (v27 < 35 - v28) {
				v17 = 15 - (unsigned char)esi3->_mint + random(106, 10);
				v18 = v24;
				goto LABEL_11;
			}
		}
		v29 = esi3->_mgoalvar1;
		v30 = midir + pattern[v29];
		if (v30 >= 0) {
			v31 = v30 - 8;
			if (v30 < 8)
				v31 = midir + pattern[v29];
		} else {
			v31 = v30 + 8;
		}
		esi3->_mgoalvar1 = v29 + 1;
		if (v29 + 1 > 5)
			esi3->_mgoalvar1 = 0;
		v32 = esi3->_mgoalvar2;
		v33 = v31 - v32;
		if (v31 - v32 >= 0) {
			if (v33 >= 8)
				v33 -= 8;
		} else {
			v33 += 8;
		}
		if (v33 <= 0) {
		LABEL_47:
			if (!M_DumbWalk(v24, esi3->_mgoalvar2))
				M_CallWalk2(v24, esi3->_mdir);
			goto LABEL_49;
		}
		if (v33 >= 4) {
			if (v33 == 4) {
				esi3->_mgoalvar2 = v31;
				goto LABEL_47;
			}
			v34 = v32 - 1;
			if (v32 - 1 < 0) {
				v35 = v32 + 7;
				goto LABEL_46;
			}
			if (v34 >= 8) {
				v35 = v32 - 9;
				goto LABEL_46;
			}
		} else {
			v34 = v32 + 1;
			if (v32 + 1 < 0) {
				v35 = v32 + 9;
			LABEL_46:
				esi3->_mgoalvar2 = v35;
				goto LABEL_47;
			}
			if (v34 >= 8) {
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
	MonsterStruct *Monst;
	int md, v, pnum;
	int fx, fy, xd, yd;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Bat: Invalid monster %d", i);

	Monst = &monster[i];
	pnum = Monst->_menemy;
	if (Monst->_mmode != MM_STAND || !Monst->_msquelch) {
		return;
	}

	xd = Monst->_mx - Monst->_menemyx;
	yd = Monst->_my - Monst->_menemyy;
	md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
	Monst->_mdir = md;
	v = random(107, 100);
	if (Monst->_mgoal == MGOAL_RETREAT) {
		if (!Monst->_mgoalvar1) {
			M_CallWalk(i, opposite[md]);
			Monst->_mgoalvar1++;
		} else {
			if (random(108, 2))
				M_CallWalk(i, left[md]);
			else
				M_CallWalk(i, right[md]);
			Monst->_mgoal = MGOAL_NORMAL;
		}
		return;
	}

	fx = Monst->_menemyx;
	fy = Monst->_menemyy;
	if (Monst->MType->mtype == MT_GLOOM
	    && (abs(xd) >= 5 || abs(yd) >= 5)
	    && v < 4 * Monst->_mint + 33
	    && LineClearF1(PosOkMonst, i, Monst->_mx, Monst->_my, fx, fy)) {
		if (AddMissile(Monst->_mx, Monst->_my, fx, fy, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
			dMonster[Monst->_mx][Monst->_my] = -(i + 1);
			Monst->_mmode = MM_CHARGE;
		}
	} else if (abs(xd) >= 2 || abs(yd) >= 2) {
		if (Monst->_mVar2 > 20 && v < Monst->_mint + 13
		    || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
		        && Monst->_mVar2 == 0
		        && v < Monst->_mint + 63) {
			M_CallWalk(i, md);
		}
	} else if (v < 4 * Monst->_mint + 8) {
		M_StartAttack(i);
		Monst->_mgoal = MGOAL_RETREAT;
		Monst->_mgoalvar1 = 0;
		if (Monst->MType->mtype == MT_FAMILIAR) {
			AddMissile(Monst->_menemyx, Monst->_menemyy, Monst->_menemyx + 1, 0, -1, MIS_LIGHTNING, 1, i, random(109, 10) + 1, 0);
		}
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void __fastcall MAI_SkelBow(int i)
{
	MonsterStruct *Monst;
	int mx, my, md, v;
	BOOL walking;

	walking = FALSE;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_SkelBow: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || !Monst->_msquelch) {
		return;
	}

	mx = Monst->_mx - Monst->_menemyx;
	my = Monst->_my - Monst->_menemyy;

	md = M_GetDir(i);
	Monst->_mdir = md;
	v = random(110, 100);

	if (abs(mx) < 4 && abs(my) < 4) {
		if (Monst->_mVar2 > 20 && v < 2 * Monst->_mint + 13
		    || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
		        && Monst->_mVar2 == 0
		        && v < 2 * Monst->_mint + 63) {
			walking = M_DumbWalk(i, opposite[md]);
		}
	}

	mx = Monst->_menemyx;
	my = Monst->_menemyy;
	if (!walking) {
		if (random(110, 100) < 2 * Monst->_mint + 3) {
			if (LineClear(Monst->_mx, Monst->_my, mx, my))
				M_StartRAttack(i, MIS_ARROW, 4);
		}
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void __fastcall MAI_Fat(int i)
{
	MonsterStruct *Monst;
	int mx, my, md, v;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Fat: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || !Monst->_msquelch) {
		return;
	}

	mx = Monst->_mx - Monst->_menemyx;
	my = Monst->_my - Monst->_menemyy;
	md = M_GetDir(i);
	Monst->_mdir = md;
	v = random(111, 100);
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if (Monst->_mVar2 > 20 && v < 4 * Monst->_mint + 20
		    || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
		        && Monst->_mVar2 == 0
		        && v < 4 * Monst->_mint + 70) {
			M_CallWalk(i, md);
		}
	} else if (v < 4 * Monst->_mint + 15) {
		M_StartAttack(i);
	} else if (v < 4 * Monst->_mint + 20) {
		M_StartSpAttack(i);
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void __fastcall MAI_Sneak(int i)
{
	int v1;            // edi
	MonsterStruct *v2; // esi
	int v3;            // ebx
	int v4;            // ebx
	int v6;            // edi
	int v7;            // eax
	//int v8; // ST04_4
	int v9; // eax
	//int v10; // ST04_4
	int v11;        // eax
	int v12;        // edi
	signed int v13; // ecx
	int v14;        // eax
	int v15;        // [esp+Ch] [ebp-10h]
	int arglist;    // [esp+10h] [ebp-Ch]
	int v17;        // [esp+14h] [ebp-8h]
	int md;         // [esp+18h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("MAI_Sneak: Invalid monster %d", i);
	}

	v2 = &monster[v1];
	if (v2->_mmode == MM_STAND) {
		v3 = v2->_my;
		if (dLight[v2->_mx][v3] != lightmax) {
			v17 = v2->_mx - (unsigned char)v2->_menemyx;
			v4 = v3 - (unsigned char)v2->_menemyy;
			md = M_GetDir(v1);
			v6 = 5 - (unsigned char)v2->_mint;
			if (v2->_mVar1 == MM_GOTHIT) {
				v2->_mgoalvar1 = 0;
				_LOBYTE(v2->_mgoal) = MGOAL_RETREAT;
			} else {
				v7 = abs(v17);
				//v5 = v8;
				if (v7 >= v6 + 3 || (v9 = abs(v4), v9 >= v6 + 3) || v2->_mgoalvar1 > 8) /* v5 = v10,  */
				{
					v2->_mgoalvar1 = 0;
					_LOBYTE(v2->_mgoal) = MGOAL_NORMAL;
				}
			}
			if (_LOBYTE(v2->_mgoal) == MGOAL_RETREAT) {
				if (v2->_mFlags & MFLAG_TARGETS_MONSTER)
					md = GetDirection(v2->_mx, v2->_my, plr[v2->_menemy]._pownerx, plr[v2->_menemy]._pownery);
				md = opposite[md];
				if (v2->MType->mtype == MT_UNSEEN) {
					if (random(112, 2))
						v11 = left[md];
					else
						v11 = right[md];
					md = v11;
				}
			}
			v2->_mdir = md;
			v15 = random(112, 100);
			if (abs(v17) < v6 && abs(v4) < v6 && v2->_mFlags & MFLAG_HIDDEN) {
				M_StartFadein(arglist, md, FALSE);
			} else {
				v12 = v6 + 1;
				if (abs(v17) < v12 && abs(v4) < v12 || v2->_mFlags & MFLAG_HIDDEN) {
					if (_LOBYTE(v2->_mgoal) == MGOAL_RETREAT
					    || (abs(v17) >= 2 || abs(v4) >= 2)
					        && ((v13 = v2->_mVar2, v13 > 20) && v15 < 4 * (unsigned char)v2->_mint + 14
					               || ((v14 = v2->_mVar1, v14 == MM_WALK) || v14 == MM_WALK2 || v14 == MM_WALK3)
					                   && !v13
					                   && v15 < 4 * (unsigned char)v2->_mint + 64)) {
						++v2->_mgoalvar1;
						M_CallWalk(arglist, md);
					}
				} else {
					M_StartFadeout(arglist, md, TRUE);
				}
			}
			if (v2->_mmode == MM_STAND) {
				if (abs(v17) >= 2 || abs(v4) >= 2 || v15 >= 4 * (unsigned char)v2->_mint + 10)
					v2->_mAnimData = v2->MType->Anims[MA_STAND].Data[md];
				else
					M_StartAttack(arglist);
			}
		}
	}
}
// 642A14: using guessed type char lightmax;

void __fastcall MAI_Fireman(int i)
{
	int xd, yd;
	int md, pnum;
	int fx, fy;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Fireman: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND || !Monst->_msquelch)
		return;

	pnum = monster[i]._menemy;
	fx = monster[i]._menemyx;
	fy = monster[i]._menemyy;
	xd = monster[i]._mx - fx;
	yd = monster[i]._my - fy;

	md = M_GetDir(i);
	if (Monst->_mgoal == MGOAL_NORMAL) {
		if (LineClear(Monst->_mx, Monst->_my, fx, fy)
		    && AddMissile(Monst->_mx, Monst->_my, fx, fy, md, MIS_FIREMAN, pnum, i, 0, 0) != -1) {
			Monst->_mmode = MM_CHARGE;
			Monst->_mgoal = MGOAL_SHOOT;
			Monst->_mgoalvar1 = 0;
		}
	} else if (Monst->_mgoal == MGOAL_SHOOT) {
		if (Monst->_mgoalvar1 == 3) {
			Monst->_mgoal = MGOAL_NORMAL;
			M_StartFadeout(i, md, TRUE);
		} else if (LineClear(Monst->_mx, Monst->_my, fx, fy)) {
			M_StartRAttack(i, MIS_KRULL, 4);
			Monst->_mgoalvar1++;
		} else {
			M_StartDelay(i, random(112, 10) + 5);
			Monst->_mgoalvar1++;
		}
	} else if (Monst->_mgoal == MGOAL_RETREAT) {
		M_StartFadein(i, md, FALSE);
		Monst->_mgoal = MGOAL_SHOOT;
	}
	Monst->_mdir = md;
	random(112, 100);
	if (Monst->_mmode != MM_STAND)
		return;

	if (abs(xd) < 2 && abs(yd) < 2 && Monst->_mgoal == MGOAL_NORMAL) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit, monster[i].mMinDamage, monster[i].mMaxDamage);
		Monst->_mgoal = MGOAL_RETREAT;
		if (!M_CallWalk(i, opposite[md])) {
			M_StartFadein(i, md, FALSE);
			Monst->_mgoal = MGOAL_SHOOT;
		}
	} else if (!M_CallWalk(i, md) && (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_RETREAT)) {
		M_StartFadein(i, md, FALSE);
		Monst->_mgoal = MGOAL_SHOOT;
	}
}

void __fastcall MAI_Fallen(int i)
{
	int x, y, xpos, ypos;
	int m, rad, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("MAI_Fallen: Invalid monster %d", i);
	}
	if (monster[i]._mgoal == MGOAL_SHOOT) {
		if (monster[i]._mgoalvar1)
			monster[i]._mgoalvar1--;
		else
			monster[i]._mgoal = MGOAL_NORMAL;
	}

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND || !monster[i]._msquelch) {
		return;
	}

	if (Monst->_mgoal == MGOAL_RETREAT) {
		if (!Monst->_mgoalvar1--) {
			Monst->_mgoal = MGOAL_NORMAL;
			M_StartStand(i, opposite[Monst->_mdir]);
		}
	}

	if (Monst->_mAnimFrame == Monst->_mAnimLen) {
		if (random(113, 4)) {
			return;
		}
		if (!(Monst->_mFlags & MFLAG_NOHEAL)) {
			M_StartSpStand(i, Monst->_mdir);
			rad = 2 * Monst->_mint + 2;
			if (Monst->_mmaxhp - rad >= Monst->_mhitpoints)
				Monst->_mhitpoints = rad + Monst->_mhitpoints;
			else
				Monst->_mhitpoints = Monst->_mmaxhp;
		}
		rad = 2 * Monst->_mint + 4;
		for (y = -rad; y <= rad; y++) {
			for (x = -rad; x <= rad; x++) {
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
					m = dMonster[x + Monst->_mx][y + Monst->_my];
					if (m > 0) {
						m--;
						if (monster[m]._mAi == AI_FALLEN) {
							monster[m]._mgoal = MGOAL_SHOOT;
							monster[m]._mgoalvar1 = 30 * Monst->_mint + 105;
						}
					}
				}
			}
		}
	} else if (Monst->_mgoal == MGOAL_RETREAT) {
		md = Monst->_mdir;
		M_CallWalk(i, md);
	} else if (Monst->_mgoal == MGOAL_SHOOT) {
		xpos = Monst->_mx - Monst->_menemyx;
		ypos = Monst->_my - Monst->_menemyy;
		if (abs(xpos) < 2 && abs(ypos) < 2) {
			M_StartAttack(i);
		} else {
			md = M_GetDir(i);
			M_CallWalk(i, md);
		}
	} else {
		MAI_SkelSd(i);
	}
}

void __fastcall MAI_Cleaver(int i)
{
	MonsterStruct *Monst;
	int x, y, mx, my, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Cleaver: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || !Monst->_msquelch) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	x = mx - Monst->_menemyx;
	y = my - Monst->_menemyy;

	md = GetDirection(mx, my, Monst->_lastx, Monst->_lasty);
	Monst->_mdir = md;

	if (abs(x) >= 2 || abs(y) >= 2)
		M_CallWalk(i, md);
	else
		M_StartAttack(i);

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void __fastcall MAI_Round(int i, BOOL special)
{
	int v2;            // esi
	MonsterStruct *v3; // esi
	int v4;            // edx
	int v5;            // ecx
	int v6;            // edi
	int v7;            // ebx
	int v9;            // eax
	//int v10; // ST04_4
	int v12; // eax
	//int v13; // ST04_4
	int v15;     // edi
	int v16;     // eax
	int v17;     // ecx
	BOOLEAN v18; // eax
	//int v19; // eax
	int v21; // eax
	int v22; // eax
	//int v23; // ST04_4
	signed int v25; // ecx
	int v26;        // eax
	int v27;        // [esp+4h] [ebp-18h]
	int v28;        // [esp+8h] [ebp-14h]
	char *v29;      // [esp+8h] [ebp-14h]
	int v30;        // [esp+Ch] [ebp-10h]
	int md;         // [esp+10h] [ebp-Ch]
	int v32;        // [esp+14h] [ebp-8h]
	int arglist;    // [esp+18h] [ebp-4h]

	v2 = i;
	v27 = special;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Round: Invalid monster %d", i);
	v3 = &monster[v2];
	if (v3->_mmode == MM_STAND && v3->_msquelch) {
		v4 = v3->_my;
		v5 = v3->_mx;
		v28 = (unsigned char)v3->_menemyy;
		v6 = (unsigned char)v3->_menemyx;
		v7 = v5 - v6;
		v32 = v4 - v28;
		md = GetDirection(v5, v4, v3->_lastx, v3->_lasty);
		if (v3->_msquelch < 0xFFu) /* check sign */
			MonstCheckDoors(arglist);
		v30 = random(114, 100);
		if ((abs(v7) >= 2 || abs(v32) >= 2) && v3->_msquelch == -1) {
			v29 = &dTransVal[v6][v28];
			if (dTransVal[v3->_mx][v3->_my] == *v29) {
				if (_LOBYTE(v3->_mgoal) != MGOAL_MOVE) {
					v9 = abs(v7);
					//v11 = v10;
					if (v9 < 4) {
						v12 = abs(v32);
						//v11 = v13;
						if (v12 < 4)
							goto LABEL_26;
					}
					if (random(115, 4))
						goto LABEL_26;
					if (_LOBYTE(v3->_mgoal) != MGOAL_MOVE) {
						v3->_mgoalvar1 = 0;
						v3->_mgoalvar2 = random(116, 2);
					}
				}
				_LOBYTE(v3->_mgoal) = MGOAL_MOVE;
				v15 = abs(v32);
				if (abs(v7) <= v15)
					v16 = abs(v32);
				else
					v16 = abs(v7);
				v17 = v3->_mgoalvar1;
				v3->_mgoalvar1 = v17 + 1;
				if (v17 < 2 * v16 || (v18 = DirOK(arglist, md), !v18)) {
					if (dTransVal[v3->_mx][v3->_my] == *v29) {
						//_LOBYTE(v19) = M_RoundWalk(arglist, md, &v3->_mgoalvar2);
						if (!M_RoundWalk(arglist, md, &v3->_mgoalvar2)) {
							v21 = random(125, 10);
							M_StartDelay(arglist, v21 + 10);
						}
						goto LABEL_26;
					}
				}
			}
		}
		_LOBYTE(v3->_mgoal) = MGOAL_NORMAL;
	LABEL_26:
		if (_LOBYTE(v3->_mgoal) == MGOAL_NORMAL) {
			if (abs(v7) >= 2 || (v22 = abs(v32), v22 >= 2)) /* v24 = v23,  */
			{
				v25 = v3->_mVar2;
				if (v25 > 20 && v30 < 2 * (unsigned char)v3->_mint + 28
				    || ((v26 = v3->_mVar1, v26 == MM_WALK) || v26 == MM_WALK2 || v26 == MM_WALK3)
				        && !v25
				        && v30 < 2 * (unsigned char)v3->_mint + 78) {
					M_CallWalk(arglist, md);
				}
			} else if (v30 < 2 * (unsigned char)v3->_mint + 23) {
				v3->_mdir = md;
				if (v27 && v3->_mhitpoints<v3->_mmaxhp>> 1 && random(117, 2))
					M_StartSpAttack(arglist);
				else
					M_StartAttack(arglist);
			}
		}
		if (v3->_mmode == MM_STAND)
			v3->_mAnimData = v3->MType->Anims[MA_STAND].Data[md];
	}
}

void __fastcall MAI_GoatMc(int i)
{
	MAI_Round(i, 1u);
}

void __fastcall MAI_Ranged(int i, int missile_type, BOOL special)
{
	int md;
	int mx, my, fx, fy;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Ranged: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	if (Monst->_msquelch == -1 || Monst->_mFlags & MFLAG_TARGETS_MONSTER) {
		mx = Monst->_menemyx;
		my = Monst->_menemyy;
		fx = Monst->_mx - mx;
		fy = Monst->_my - my;
		md = M_GetDir(i);
		if ((DWORD)Monst->_msquelch < -1) /* check sign */
			MonstCheckDoors(i);
		Monst->_mdir = md;
		if (Monst->_mVar1 == MM_RATTACK) {
			M_StartDelay(i, random(118, 20));
		} else if (abs(fx) < 4 && abs(fy) < 4) {
			if (random(119, 100) < 10 * (Monst->_mint + 7))
				M_CallWalk(i, opposite[md]);
		}
		if (Monst->_mmode == MM_STAND) {
			if (LineClear(Monst->_mx, Monst->_my, mx, my)) {
				if (special)
					M_StartRSpAttack(i, missile_type, 4);
				else
					M_StartRAttack(i, missile_type, 4);
			} else {
				Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
			}
		}
	} else if (Monst->_msquelch) {
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		M_CallWalk(i, md);
	}
}

void __fastcall MAI_GoatBow(int i)
{
	MAI_Ranged(i, MIS_ARROW, 0);
}

void __fastcall MAI_Succ(int i)
{
	MAI_Ranged(i, MIS_FLARE, 0);
}

void __fastcall MAI_AcidUniq(int i)
{
	MAI_Ranged(i, MIS_ACID, 1u);
}

void __fastcall MAI_Scav(int i)
{
	int v1;            // edi
	int v2;            // esi
	unsigned char *v3; // eax
	int v4;            // ecx
	int v5;            // ecx
	signed int v6;     // ebx
	signed int v7;     // edi
	int v8;            // edx
	int v9;            // eax
	BOOLEAN v10;       // eax
	int v11;           // ebx
	int v12;           // edi
	signed int v13;    // edi
	int v14;           // edx
	int v15;           // eax
	BOOLEAN v16;       // eax
	int v17;           // eax
	int v18;           // eax
	int arglist;       // [esp+Ch] [ebp-8h]
	BOOL v20;          // [esp+10h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Scav: Invalid monster %d", i);
	v2 = v1;
	v20 = 0;
	if (monster[v1]._mmode == MM_STAND) {
		if (monster[v2]._mhitpoints<monster[v2]._mmaxhp>> 1) {
			if (_LOBYTE(monster[v2]._mgoal) == MGOAL_HEALING)
				goto LABEL_10;
			if (monster[v2].leaderflag) {
				v3 = &monster[(unsigned char)monster[v2].leader].packsize;
				--*v3;
				monster[v2].leaderflag = 0;
			}
			_LOBYTE(monster[v2]._mgoal) = MGOAL_HEALING;
			monster[v2]._mgoalvar3 = 10;
		}
		if (_LOBYTE(monster[v2]._mgoal) != MGOAL_HEALING) {
		LABEL_52:
			if (monster[v2]._mmode == MM_STAND)
				MAI_SkelSd(arglist);
			return;
		}
	LABEL_10:
		v4 = monster[v2]._mgoalvar3;
		if (v4) {
			monster[v2]._mgoalvar3 = v4 - 1;
			v5 = monster[v2]._my;
			if (dDead[monster[v2]._mx][v5]) {
				M_StartEat(v1);
				if (!(monster[v2]._mFlags & MFLAG_NOHEAL))
					monster[v2]._mhitpoints += 64;
				if (monster[v2]._mhitpoints >= (monster[v2]._mmaxhp >> 1) + (monster[v2]._mmaxhp >> 2)) {
					_LOBYTE(monster[v2]._mgoal) = MGOAL_NORMAL;
					monster[v2]._mgoalvar1 = 0;
					monster[v2]._mgoalvar2 = 0;
				}
			} else {
				if (!monster[v2]._mgoalvar1) {
					v6 = arglist;
					if (random(120, 2)) {
						v7 = -4;
						do {
							if (v20)
								break;
							v6 = -4;
							do {
								if (v20)
									break;
								if (v7 >= 0 && v7 < MAXDUNY && v6 >= 0 && v6 < MAXDUNX) {
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
							} while (v6 <= 4);
							++v7;
						} while (v7 <= 4);
						v11 = v6 - 1;
						v12 = v7 - 1;
					} else {
						v13 = 4;
						do {
							if (v20)
								break;
							v6 = 4;
							do {
								if (v20)
									break;
								if (v13 >= 0 && v13 < MAXDUNY && v6 >= 0 && v6 < MAXDUNX) {
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
							} while (v6 >= -4);
							--v13;
						} while (v13 >= -4);
						v11 = v6 + 1;
						v12 = v13 + 1;
					}
					if (v20) {
						monster[v2]._mgoalvar1 = monster[v2]._mx + v11 + 1;
						monster[v2]._mgoalvar2 = monster[v2]._my + v12 + 1;
					}
				}
				v17 = monster[v2]._mgoalvar1;
				if (v17) {
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
	MonsterStruct *Monst;
	int mx, my, dx, dy, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Garg: Invalid monster %d", i);

	Monst = &monster[i];
	dx = Monst->_mx - Monst->_lastx;
	dy = Monst->_my - Monst->_lasty;
	md = M_GetDir(i);
	if (Monst->_msquelch && Monst->_mFlags & MFLAG_ALLOW_SPECIAL) {
		M_Enemy(i);
		mx = Monst->_mx - Monst->_menemyx;
		my = Monst->_my - Monst->_menemyy;
		if (abs(mx) < Monst->_mint + 2 && abs(my) < Monst->_mint + 2) {
			Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
		return;
	}

	if (Monst->_mmode != MM_STAND || !Monst->_msquelch) {
		return;
	}

	if (Monst->_mhitpoints<Monst->_mmaxhp>> 1 && !(Monst->_mFlags & MFLAG_NOHEAL))
		Monst->_mgoal = MGOAL_RETREAT;
	if (Monst->_mgoal == MGOAL_RETREAT) {
		if (abs(dx) >= Monst->_mint + 2 || abs(dy) >= Monst->_mint + 2) {
			Monst->_mgoal = MGOAL_NORMAL;
			M_StartHeal(i);
		} else if (!M_CallWalk(i, opposite[md])) {
			Monst->_mgoal = MGOAL_NORMAL;
		}
	}
	MAI_Round(i, 0);
}

void __fastcall MAI_RoundRanged(int i, int missile_type, unsigned char checkdoors, int dam, int lessmissiles)
{
	int v5;            // esi
	MonsterStruct *v6; // esi
	int v7;            // edx
	int v8;            // ebx
	int v9;            // edi
	int v11;           // eax
	//int v12; // ST04_4
	int v13; // ecx
	int v14; // eax
	//int v15; // ST04_4
	int v16; // eax
	//int v17; // ST04_4
	int v18;     // ecx
	int v19;     // ebx
	int v20;     // eax
	int v21;     // ecx
	BOOLEAN v22; // eax
	BOOLEAN v23; // eax
	BOOLEAN v24; // eax
	int v25;     // eax
	//int v26; // ST04_4
	int v27; // eax
	//int v28; // ST04_4
	int v29;           // eax
	int v30;           // edx
	int v31;           // eax
	int missile_typea; // [esp+4h] [ebp-18h]
	int v33;           // [esp+8h] [ebp-14h]
	int x2;            // [esp+Ch] [ebp-10h]
	int md;            // [esp+10h] [ebp-Ch]
	int y2;            // [esp+14h] [ebp-8h]
	int arglist;       // [esp+18h] [ebp-4h]
	int checkdoorsa;   // [esp+24h] [ebp+8h]

	v5 = i;
	missile_typea = missile_type;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_RoundRanged: Invalid monster %d", i);
	v6 = &monster[v5];
	if (v6->_mmode == MM_STAND && v6->_msquelch) {
		v7 = v6->_my;
		y2 = (unsigned char)v6->_menemyy;
		v8 = v7 - y2;
		x2 = (unsigned char)v6->_menemyx;
		v9 = v6->_mx - x2;
		v33 = v7 - y2;
		md = GetDirection(v6->_mx, v7, v6->_lastx, v6->_lasty);
		if (checkdoors && v6->_msquelch < 0xFFu) /* check sign */
			MonstCheckDoors(arglist);
		checkdoorsa = random(121, 10000);
		v11 = abs(v9);
		//v13 = v12;
		if (v11 < 2) {
			v14 = abs(v8);
			//v13 = v15;
			if (v14 < 2)
				goto LABEL_50;
		}
		if (v6->_msquelch != -1)
			goto LABEL_50;
		//v13 = y2;
		if (dTransVal[v6->_mx][v6->_my] != dTransVal[x2][y2])
			goto LABEL_50;
		if (_LOBYTE(v6->_mgoal) != MGOAL_MOVE) {
			if (abs(v9) < 3) {
				v16 = abs(v8);
				//v13 = v17;
				if (v16 < 3)
					goto LABEL_28;
			}
			v18 = lessmissiles;
			if (random(122, 4 << lessmissiles))
				goto LABEL_28;
			if (_LOBYTE(v6->_mgoal) != MGOAL_MOVE) {
				v6->_mgoalvar1 = 0;
				v6->_mgoalvar2 = random(123, 2);
			}
		}
		_LOBYTE(v6->_mgoal) = MGOAL_MOVE;
		v19 = abs(v8);
		if (abs(v9) <= v19) {
			v8 = v33;
			v20 = abs(v33);
		} else {
			v20 = abs(v9);
			v8 = v33;
		}
		v21 = v6->_mgoalvar1;
		v6->_mgoalvar1 = v21 + 1;
		if (v21 >= 2 * v20 && (v22 = DirOK(arglist, md), v22)) {
		LABEL_50:
			_LOBYTE(v6->_mgoal) = MGOAL_NORMAL;
		} else if (checkdoorsa<500 * ((unsigned char)v6->_mint + 1)>> lessmissiles
		    && (v23 = LineClear(v6->_mx, v6->_my, x2, y2), v23)) {
			M_StartRSpAttack(arglist, missile_typea, dam);
		} else {
			M_RoundWalk(arglist, md, &v6->_mgoalvar2);
		}
	LABEL_28:
		if (_LOBYTE(v6->_mgoal) == MGOAL_NORMAL) {
			if (((abs(v9) >= 3 || abs(v8) >= 3) && checkdoorsa<500 * ((unsigned char)v6->_mint + 2)>> lessmissiles
			        || checkdoorsa<500 * ((unsigned char)v6->_mint + 1)>> lessmissiles)
			    && (v24 = LineClear(v6->_mx, v6->_my, x2, y2), v24)) {
				M_StartRSpAttack(arglist, missile_typea, dam);
			} else {
				v25 = abs(v9);
				//v13 = v26;
				if (v25 >= 2 || (v27 = abs(v8), v27 >= 2)) /* v13 = v28,  */
				{
					v29 = random(124, 100);
					v30 = (unsigned char)v6->_mint;
					if (v29 < 1000 * (v30 + 5)
					    || ((v13 = v6->_mVar1, v13 == MM_WALK) || v13 == MM_WALK2 || v13 == MM_WALK3) && !v6->_mVar2 && v29 < 1000 * (v30 + 8)) {
						M_CallWalk(arglist, md);
					}
				} else if (checkdoorsa < 1000 * ((unsigned char)v6->_mint + 6)) {
					v6->_mdir = md;
					M_StartAttack(arglist);
				}
			}
		}
		if (v6->_mmode == MM_STAND) {
			v31 = random(125, 10);
			M_StartDelay(arglist, v31 + 5);
		}
	}
}

void __fastcall MAI_Magma(int i)
{
	MAI_RoundRanged(i, MIS_MAGMABALL, 1u, 4, 0);
}

void __fastcall MAI_Storm(int i)
{
	MAI_RoundRanged(i, MIS_LIGHTCTRL2, 1u, 4, 0);
}

void __fastcall MAI_Acid(int i)
{
	MAI_RoundRanged(i, MIS_ACID, 0, 4, 1);
}

void __fastcall MAI_Diablo(int i)
{
	MAI_RoundRanged(i, MIS_DIABAPOCA, 0, 40, 0);
}

void __fastcall MAI_RR2(int i, int mistype, int dam)
{
	int v3;            // ebx
	MonsterStruct *v4; // esi
	int v5;            // edi
	int v6;            // edx
	int v7;            // ebx
	int v8;            // edi
	int v10;           // eax
	//int v11; // ST04_4
	int v12; // ecx
	int v13; // eax
	//int v14; // ST04_4
	int v15; // eax
	//int v16; // ST04_4
	int v17; // eax
	//int v18; // ST04_4
	int v19;     // ebx
	int v20;     // eax
	BOOLEAN v21; // eax
	BOOLEAN v22; // eax
	int v23;     // ecx
	int v24;     // eax
	//int v25; // ST04_4
	int v27; // eax
	//int v28; // ST04_4
	int v29;          // eax
	int v30;          // eax
	int v31;          // eax
	int v32;          // edx
	int v33;          // eax
	int missile_type; // [esp+Ch] [ebp-1Ch]
	int x2;           // [esp+10h] [ebp-18h]
	int v36;          // [esp+14h] [ebp-14h]
	int y2;           // [esp+18h] [ebp-10h]
	int v38;          // [esp+1Ch] [ebp-Ch]
	int md;           // [esp+20h] [ebp-8h]
	int arglist;      // [esp+24h] [ebp-4h]

	v3 = i;
	missile_type = mistype;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_RR2: Invalid monster %d", i);
	v4 = &monster[v3];
	v5 = v4->_my - (unsigned char)v4->_menemyy;
	if (abs(v4->_mx - (unsigned char)v4->_menemyx) >= 5 || abs(v5) >= 5) {
		MAI_SkelSd(v3);
		return;
	}
	if (v4->_mmode == MM_STAND && v4->_msquelch) {
		v6 = v4->_my;
		y2 = (unsigned char)v4->_menemyy;
		v7 = v6 - y2;
		x2 = (unsigned char)v4->_menemyx;
		v8 = v4->_mx - x2;
		v36 = v6 - y2;
		md = GetDirection(v4->_mx, v6, v4->_lastx, v4->_lasty);
		if (v4->_msquelch < 0xFFu) /* check sign */
			MonstCheckDoors(arglist);
		v38 = random(121, 100);
		v10 = abs(v8);
		//v12 = v11;
		if (v10 >= 2 || (v13 = abs(v7), v13 >= 2)) /* v12 = v14,  */
		{
			if (v4->_msquelch == -1) {
				//v12 = y2;
				if (dTransVal[v4->_mx][v4->_my] == dTransVal[x2][y2]) {
					if (_LOBYTE(v4->_mgoal) != MGOAL_MOVE) {
						v15 = abs(v8);
						//v12 = v16;
						if (v15 < 3) {
							v17 = abs(v7);
							//v12 = v18;
							if (v17 < 3)
								goto LABEL_26;
						}
						if (_LOBYTE(v4->_mgoal) != MGOAL_MOVE) {
							v4->_mgoalvar1 = 0;
							v4->_mgoalvar2 = random(123, 2);
						}
					}
					_LOBYTE(v4->_mgoal) = MGOAL_MOVE;
					v4->_mgoalvar3 = 4;
					v19 = abs(v7);
					if (abs(v8) <= v19) {
						v7 = v36;
						v20 = abs(v36);
					} else {
						v20 = abs(v8);
						v7 = v36;
					}
					v12 = v4->_mgoalvar1;
					v4->_mgoalvar1 = v12 + 1;
					if (v12 < 2 * v20 || (v21 = DirOK(arglist, md), !v21)) {
						if (v38 < 5 * ((unsigned char)v4->_mint + 16))
							M_RoundWalk(arglist, md, &v4->_mgoalvar2);
					LABEL_26:
						if (_LOBYTE(v4->_mgoal) != MGOAL_NORMAL)
							goto LABEL_48;
						if (((abs(v8) >= 3 || abs(v7) >= 3) && v38 < 5 * ((unsigned char)v4->_mint + 2)
						        || v38 < 5 * ((unsigned char)v4->_mint + 1)
						        || v4->_mgoalvar3 == 4)
						    && (v22 = LineClear(v4->_mx, v4->_my, x2, y2), v22)) {
							v23 = arglist;
						} else {
							v24 = abs(v8);
							//v26 = v25;
							if (v24 >= 2 || (v27 = abs(v7), v27 >= 2)) /* v26 = v28,  */
							{
								v31 = random(124, 100);
								v12 = (unsigned char)v4->_mint;
								if (v31 < 2 * (5 * v12 + 25)
								    || ((v32 = v4->_mVar1, v32 == MM_WALK) || v32 == MM_WALK2 || v32 == MM_WALK3)
								        && !v4->_mVar2
								        && (v12 = 2 * (5 * v12 + 40), v31 < v12)) {
									M_CallWalk(arglist, md);
								}
								goto LABEL_47;
							}
							v29 = random(124, 100);
							v12 = 10 * ((unsigned char)v4->_mint + 4);
							if (v29 >= v12) {
							LABEL_47:
								v4->_mgoalvar3 = 1;
							LABEL_48:
								if (v4->_mmode == MM_STAND) {
									v33 = random(125, 10);
									M_StartDelay(arglist, v33 + 5);
								}
								return;
							}
							v4->_mdir = md;
							v30 = random(124, 2);
							v23 = arglist;
							if (v30) {
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
		_LOBYTE(v4->_mgoal) = MGOAL_NORMAL;
		goto LABEL_26;
	}
}

void __fastcall MAI_Mega(int i)
{
	MAI_RR2(i, MIS_FLAMEC, 0);
}

void __fastcall MAI_Golum(int i)
{
	int mx, my, _mex, _mey;
	int md, j, k, _menemy;
	MonsterStruct *Monst;
	BOOL have_enemy, ok;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Golum: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mx == 1 && Monst->_my == 0) {
		return;
	}

	if (Monst->_mmode == MM_DEATH
	    || Monst->_mmode == MM_SPSTAND
	    || (Monst->_mmode >= MM_WALK && Monst->_mmode <= MM_WALK3)) {
		return;
	}

	if (!(Monst->_mFlags & MFLAG_TARGETS_MONSTER))
		M_Enemy(i);

	have_enemy = !(monster[i]._mFlags & MFLAG_NO_ENEMY);

	if (Monst->_mmode == MM_ATTACK) {
		return;
	}

	_menemy = monster[i]._menemy;

	mx = monster[i]._mx;
	my = monster[i]._my;
	_mex = mx - monster[_menemy]._mfutx;
	_mey = my - monster[_menemy]._mfuty;
	md = GetDirection(mx, my, monster[_menemy]._mx, monster[_menemy]._my);
	monster[i]._mdir = md;
	if (abs(_mex) >= 2 || abs(_mey) >= 2) {
		if (have_enemy && MAI_Path(i))
			return;
	} else if (have_enemy) {
		_menemy = monster[i]._menemy;
		monster[i]._menemyx = monster[_menemy]._mx;
		monster[i]._menemyy = monster[_menemy]._my;
		if (!monster[_menemy]._msquelch) {
			monster[_menemy]._msquelch = -1;
			monster[monster[i]._menemy]._lastx = monster[i]._mx;
			monster[monster[i]._menemy]._lasty = monster[i]._my;
			for (j = 0; j < 5; j++) {
				for (k = 0; k < 5; k++) {
					_menemy = dMonster[monster[i]._mx + k - 2][monster[i]._my + j - 2];
					if (_menemy > 0)
						monster[_menemy]._msquelch = -1;
				}
			}
		}
		M_StartAttack(i);
		return;
	}

	monster[i]._pathcount++;
	if (monster[i]._pathcount > 8)
		monster[i]._pathcount = 5;

	ok = M_CallWalk(i, plr[i]._pdir);
	if (!ok) {
		md = (md - 1) & 7;
		for (j = 0; j < 8 && !ok; j++) {
			md = (md + 1) & 7;
			ok = DirOK(i, md);
		}
		if (!ok) {
			return;
		}
		M_WalkDir(i, md);
	}
}

void __fastcall MAI_SkelKing(int i)
{
	int v1;            // esi
	MonsterStruct *v2; // esi
	int v3;            // edx
	int v4;            // ebx
	int v5;            // edi
	int v7;            // eax
	//int v8; // ST04_4
	int v10; // eax
	//int v11; // ST04_4
	int v13;     // ebx
	int v14;     // eax
	int v15;     // ecx
	BOOLEAN v16; // eax
	//int v17; // eax
	int v19;     // eax
	BOOLEAN v20; // eax
	int v21;     // edi
	int v22;     // ebx
	int v23;     // eax
	//int v24; // ST04_4
	int v26; // eax
	//int v27; // ST04_4
	int v28;     // eax
	int v29;     // ecx
	int v30;     // edx
	int v31;     // eax
	char *v32;   // [esp+4h] [ebp-1Ch]
	int x2;      // [esp+8h] [ebp-18h]
	int v34;     // [esp+Ch] [ebp-14h]
	int v35;     // [esp+10h] [ebp-10h]
	int y2;      // [esp+14h] [ebp-Ch]
	int md;      // [esp+18h] [ebp-8h]
	int arglist; // [esp+1Ch] [ebp-4h]

	v1 = i;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_SkelKing: Invalid monster %d", i);
	v2 = &monster[v1];
	if (v2->_mmode == MM_STAND && v2->_msquelch) {
		v3 = v2->_my;
		y2 = (unsigned char)v2->_menemyy;
		v4 = v3 - y2;
		x2 = (unsigned char)v2->_menemyx;
		v5 = v2->_mx - x2;
		v34 = v3 - y2;
		md = GetDirection(v2->_mx, v3, v2->_lastx, v2->_lasty);
		if (v2->_msquelch < 0xFFu) /* check sign */
			MonstCheckDoors(arglist);
		v35 = random(126, 100);
		if ((abs(v5) >= 2 || abs(v4) >= 2) && v2->_msquelch == -1) {
			v32 = &dTransVal[x2][y2];
			if (dTransVal[v2->_mx][v2->_my] == *v32) {
				if (_LOBYTE(v2->_mgoal) != MGOAL_MOVE) {
					v7 = abs(v5);
					//v9 = v8;
					if (v7 < 3) {
						v10 = abs(v4);
						//v9 = v11;
						if (v10 < 3)
							goto LABEL_26;
					}
					if (random(127, 4))
						goto LABEL_26;
					if (_LOBYTE(v2->_mgoal) != MGOAL_MOVE) {
						v2->_mgoalvar1 = 0;
						v2->_mgoalvar2 = random(128, 2);
					}
				}
				_LOBYTE(v2->_mgoal) = MGOAL_MOVE;
				v13 = abs(v4);
				if (abs(v5) <= v13) {
					v4 = v34;
					v14 = abs(v34);
				} else {
					v14 = abs(v5);
					v4 = v34;
				}
				v15 = v2->_mgoalvar1;
				v2->_mgoalvar1 = v15 + 1;
				if (v15 < 2 * v14 || (v16 = DirOK(arglist, md), !v16)) {
					if (dTransVal[v2->_mx][v2->_my] == *v32) {
						//_LOBYTE(v17) = M_RoundWalk(arglist, md, &v2->_mgoalvar2);
						if (!M_RoundWalk(arglist, md, &v2->_mgoalvar2)) {
							v19 = random(125, 10);
							M_StartDelay(arglist, v19 + 10);
						}
						goto LABEL_26;
					}
				}
			}
		}
		_LOBYTE(v2->_mgoal) = MGOAL_NORMAL;
	LABEL_26:
		if (_LOBYTE(v2->_mgoal) == MGOAL_NORMAL) {
			if (gbMaxPlayers == 1
			    && ((abs(v5) >= 3 || abs(v4) >= 3) && v35 < 4 * (unsigned char)v2->_mint + 35 || v35 < 6)
			    && (v20 = LineClear(v2->_mx, v2->_my, x2, y2), v20)) {
				v21 = v2->_mx + offset_x[md];
				v22 = v2->_my + offset_y[md];
				if (PosOkMonst(arglist, v21, v22) && nummonsters < MAXMONSTERS) {
					M_SpawnSkel(v21, v22, md);
					M_StartSpStand(arglist, md);
				}
			} else {
				v23 = abs(v5);
				//v25 = v24;
				if (v23 >= 2 || (v26 = abs(v4), v26 >= 2)) /* v25 = v27,  */
				{
					v28 = random(129, 100);
					v29 = (unsigned char)v2->_mint;
					if (v28 >= v29 + 25
					    && ((v30 = v2->_mVar1, v30 != MM_WALK) && v30 != MM_WALK2 && v30 != MM_WALK3 || v2->_mVar2 || (v29 += 75, v28 >= v29))) {
						v31 = random(130, 10);
						M_StartDelay(arglist, v31 + 10);
					} else {
						M_CallWalk(arglist, md);
					}
				} else if (v35 < (unsigned char)v2->_mint + 20) {
					v2->_mdir = md;
					M_StartAttack(arglist);
				}
			}
		}
		if (v2->_mmode == MM_STAND)
			v2->_mAnimData = v2->MType->Anims[MA_STAND].Data[md];
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall MAI_Rhino(int i)
{
	int esi1;            // esi
	MonsterStruct *esi3; // esi
	int v3;              // edx
	int v4;              // ebx
	int v5;              // edi
	int v7;              // eax
	//int v8; // ST1C_4
	int v10; // eax
	//int v11; // ST1C_4
	int v13; // ebx
	int v14; // eax
	int v15; // ecx
	//int v16; // eax
	int v18;     // eax
	BOOLEAN v19; // eax
	int v20;     // ecx
	int v21;     // eax
	//int v22; // ST1C_4
	int v24; // eax
	//int v25; // ST1C_4
	int v26;     // eax
	int v27;     // ecx
	int v28;     // edx
	int v29;     // eax
	int v30;     // [esp+4h] [ebp-18h]
	int v31;     // [esp+8h] [ebp-14h]
	int v1;      // [esp+Ch] [ebp-10h]
	int midir;   // [esp+10h] [ebp-Ch]
	int v2;      // [esp+14h] [ebp-8h]
	int arglist; // [esp+18h] [ebp-4h]

	esi1 = i;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Rhino: Invalid monster %d", i);
	esi3 = &monster[esi1];
	if (esi3->_mmode == MM_STAND && esi3->_msquelch) {
		v3 = esi3->_my;
		v2 = (unsigned char)esi3->_menemyy;
		v4 = v3 - v2;
		v1 = (unsigned char)esi3->_menemyx;
		v5 = esi3->_mx - v1;
		v31 = v3 - v2;
		midir = GetDirection(esi3->_mx, v3, esi3->_lastx, esi3->_lasty);
		if (esi3->_msquelch < 0xFFu) /* check sign */
			MonstCheckDoors(arglist);
		v30 = random(131, 100);
		if (abs(v5) >= 2 || abs(v4) >= 2) {
			if (_LOBYTE(esi3->_mgoal) != MGOAL_MOVE) {
				v7 = abs(v5);
				//v9 = v8;
				if (v7 < 5) {
					v10 = abs(v4);
					//v9 = v11;
					if (v10 < 5)
						goto LABEL_23;
				}
				if (!random(132, 4))
					goto LABEL_23;
				if (_LOBYTE(esi3->_mgoal) != MGOAL_MOVE) {
					esi3->_mgoalvar1 = 0;
					esi3->_mgoalvar2 = random(133, 2);
				}
			}
			_LOBYTE(esi3->_mgoal) = 4;
			v13 = abs(v4);
			if (abs(v5) <= v13) {
				v4 = v31;
				v14 = abs(v31);
			} else {
				v14 = abs(v5);
				v4 = v31;
			}
			v15 = esi3->_mgoalvar1;
			esi3->_mgoalvar1 = v15 + 1;
			if (v15 < 2 * v14 && dTransVal[esi3->_mx][esi3->_my] == dTransVal[v1][v2]) {
				//_LOBYTE(v16) = M_RoundWalk(arglist, midir, &esi3->_mgoalvar2);
				if (!M_RoundWalk(arglist, midir, &esi3->_mgoalvar2)) {
					v18 = random(125, 10);
					M_StartDelay(arglist, v18 + 10);
				}
				goto LABEL_23;
			}
		}
		_LOBYTE(esi3->_mgoal) = MGOAL_NORMAL;
	LABEL_23:
		if (_LOBYTE(esi3->_mgoal) == MGOAL_NORMAL) {
			if ((abs(v5) >= 5 || abs(v4) >= 5)
			    && v30 < 2 * (unsigned char)esi3->_mint + 43
			    && (v19 = LineClearF1(
			            PosOkMonst,
			            arglist,
			            esi3->_mx,
			            esi3->_my,
			            v1,
			            v2),
			           v19)) {
				if (AddMissile(esi3->_mx, esi3->_my, v1, v2, midir, MIS_RHINO, esi3->_menemy, arglist, 0, 0) != -1) {
					if (esi3->MData->snd_special)
						PlayEffect(arglist, 3);
					v20 = esi3->_my + 112 * esi3->_mx;
					esi3->_mmode = MM_CHARGE;
					dMonster[0][v20] = -1 - arglist;
				}
			} else {
				v21 = abs(v5);
				//v23 = v22;
				if (v21 >= 2 || (v24 = abs(v4), v24 >= 2)) /* v23 = v25,  */
				{
					v26 = random(134, 100);
					v27 = 2 * (unsigned char)esi3->_mint;
					if (v26 >= v27 + 33
					    && ((v28 = esi3->_mVar1, v28 != MM_WALK) && v28 != MM_WALK2 && v28 != MM_WALK3
					           || esi3->_mVar2
					           || (v27 += 83, v26 >= v27))) {
						v29 = random(135, 10);
						M_StartDelay(arglist, v29 + 10);
					} else {
						M_CallWalk(arglist, midir);
					}
				} else if (v30 < 2 * (unsigned char)esi3->_mint + 28) {
					esi3->_mdir = midir;
					M_StartAttack(arglist);
				}
			}
		}
		if (esi3->_mmode == MM_STAND)
			esi3->_mAnimData = esi3->MType->Anims[MA_STAND].Data[esi3->_mdir];
	}
}

void __fastcall MAI_Counselor(int i)
{
	int v1;      // ebx
	int v2;      // esi
	int v3;      // ecx
	int v4;      // edi
	int v5;      // edx
	int v6;      // ebp
	char v9;     // al
	int v10;     // ecx
	BOOLEAN v11; // zf
	BOOLEAN v12; // sf
	//unsigned char v13; // of
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
	int v32;     // eax
	int v33;     // eax
	int v34;     // eax
	int md;      // [esp+8h] [ebp-14h]
	int arglist; // [esp+Ch] [ebp-10h]
	int y2;      // [esp+10h] [ebp-Ch]
	int x2;      // [esp+14h] [ebp-8h]
	int v39;     // [esp+18h] [ebp-4h]

	v1 = i;
	arglist = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Counselor: Invalid monster %d", i);
	v2 = v1;
	if (monster[v1]._mmode == MM_STAND && monster[v2]._msquelch) {
		v3 = monster[v2]._mx;
		x2 = (unsigned char)monster[v2]._menemyx;
		v4 = v3 - x2;
		v5 = monster[v2]._my;
		y2 = (unsigned char)monster[v2]._menemyy;
		v6 = v5 - y2;
		md = GetDirection(v3, v5, monster[v2]._lastx, monster[v2]._lasty);
		if (monster[v2]._msquelch < 0xFFu) /* check sign */
			MonstCheckDoors(v1);
		v39 = random(121, 100);
		v9 = monster[v2]._mgoal;
		if (v9 == MGOAL_RETREAT) {
			v10 = monster[v2]._mgoalvar1;
			//v13 = __OFSUB__(v10, 3);
			v11 = v10 == 3;
			v12 = v10 - 3 < 0;
			v14 = v10 + 1;
			v15 = v1;
			monster[v2]._mgoalvar1 = v14;
			if (v12 | v11) { //if ((unsigned char)(v12 ^ v13) | v11) {
				M_CallWalk(v1, opposite[md]);
				goto LABEL_39;
			}
			goto LABEL_21;
		}
		if (v9 == MGOAL_MOVE) {
			v16 = abs(v6);
			if (abs(v4) <= v16)
				v17 = abs(v6);
			else
				v17 = abs(v4);
			v18 = v17;
			if (abs(v4) < 2 && abs(v6) < 2
			    || monster[v2]._msquelch != -1
			    || dTransVal[monster[v2]._mx][monster[v2]._my] != dTransVal[x2][y2]) {
				v1 = arglist;
			LABEL_20:
				v15 = v1;
			LABEL_21:
				_LOBYTE(monster[v2]._mgoal) = MGOAL_NORMAL;
				M_StartFadein(v15, md, TRUE);
				goto LABEL_39;
			}
			v19 = 2 * v18;
			v1 = arglist;
			v20 = monster[v2]._mgoalvar1;
			monster[v2]._mgoalvar1 = v20 + 1;
			if (v20 >= v19) {
				//_LOBYTE(v21) = DirOK(arglist, md);
				if (DirOK(arglist, md))
					goto LABEL_20;
			}
			M_RoundWalk(arglist, md, &monster[v2]._mgoalvar2);
		LABEL_39:
			if (monster[v2]._mmode == MM_STAND) {
				v34 = random(125, 10);
				M_StartDelay(v1, v34 + 5);
			}
			return;
		}
		if (v9 != MGOAL_NORMAL)
			goto LABEL_39;
		v22 = abs(v4);
		//v24 = v23;
		if (v22 >= 2 || (v25 = abs(v6), v25 >= 2)) /* v24 = v26,  */
		{
			if (v39 < 5 * ((unsigned char)monster[v2]._mint + 10)) {
				//_LOBYTE(v31) = LineClear(monster[v2]._mx, monster[v2]._my, x2, y2);
				if (LineClear(monster[v2]._mx, monster[v2]._my, x2, y2)) {
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
			if (random(124, 100) < 30) {
				v27 = md;
				_LOBYTE(monster[v2]._mgoal) = MGOAL_MOVE;
				goto LABEL_29;
			}
		} else {
			v27 = md;
			v28 = monster[v2]._mmaxhp >> 1;
			//v13 = __OFSUB__(monster[v2]._mhitpoints, v28);
			v12 = monster[v2]._mhitpoints - v28 < 0;
			monster[v2]._mdir = md;
			if (v12) { //if (v12 ^ v13) {
				_LOBYTE(monster[v2]._mgoal) = MGOAL_RETREAT;
			LABEL_29:
				monster[v2]._mgoalvar1 = 0;
				M_StartFadeout(v1, v27, FALSE);
				goto LABEL_39;
			}
			if (monster[v2]._mVar1 == MM_DELAY
			    || (v29 = random(105, 100),
			           v30 = 2 * (unsigned char)monster[v2]._mint + 20,
			           v29 < v30)) {
				M_StartRAttack(v1, -1, 0);
				AddMissile(monster[v2]._mx, monster[v2]._my, 0, 0, monster[v2]._mdir, MIS_FLASH, 1, v1, 4, 0);
				AddMissile(monster[v2]._mx, monster[v2]._my, 0, 0, monster[v2]._mdir, MIS_FLASH2, 1, v1, 4, 0);
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
	int _mx, _my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Garbud: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND) {
		return;
	}

	_mx = Monst->_mx;
	_my = Monst->_my;
	md = M_GetDir(i);

	if (Monst->mtalkmsg < QUEST_GARBUD4
	    && Monst->mtalkmsg > QUEST_DOOM10
	    && !(dFlags[_mx][_my] & DFLAG_VISIBLE)
	    && Monst->_mgoal == MGOAL_TALKING) {
		Monst->_mgoal = MGOAL_INQUIRING;
		Monst->mtalkmsg++;
	}

	if (dFlags[_mx][_my] & DFLAG_VISIBLE) {
		if (Monst->mtalkmsg == QUEST_GARBUD4) {
			if (!effect_is_playing(USFX_GARBUD4) && Monst->_mgoal == MGOAL_TALKING) {
				Monst->_mgoal = MGOAL_NORMAL;
				Monst->_msquelch = -1;
				Monst->mtalkmsg = 0;
			}
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_MOVE)
		MAI_Round(i, TRUE);

	monster[i]._mdir = md;

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void __fastcall MAI_Zhar(int i)
{
	int mx, my, _mx, _my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Zhar: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	my = Monst->_my;
	mx = Monst->_mx;
	md = M_GetDir(i);
	if (Monst->mtalkmsg == QUEST_ZHAR1 && !(dFlags[mx][my] & DFLAG_VISIBLE) && Monst->_mgoal == MGOAL_TALKING) {
		Monst->mtalkmsg = QUEST_ZHAR2;
		Monst->_mgoal = MGOAL_INQUIRING;
	}

	if (dFlags[mx][my] & DFLAG_VISIBLE) {
		_mx = Monst->_mx - Monst->_menemyx;
		_my = Monst->_my - Monst->_menemyy;
		if (abs(_mx) > abs(_my))
			abs(_mx);
		else
			abs(_my);
		if (Monst->mtalkmsg == QUEST_ZHAR2) {
			if (!effect_is_playing(USFX_ZHAR2) && Monst->_mgoal == MGOAL_TALKING) {
				Monst->_msquelch = -1;
				Monst->mtalkmsg = 0;
				Monst->_mgoal = MGOAL_NORMAL;
			}
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_RETREAT || Monst->_mgoal == MGOAL_MOVE)
		MAI_Counselor(i);

	Monst->_mdir = md;

	if (monster[i]._mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void __fastcall MAI_SnotSpil(int i)
{
	int mx, my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_SnotSpil: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	md = M_GetDir(i);

	if (Monst->mtalkmsg == QUEST_BANNER10 && !(dFlags[mx][my] & DFLAG_VISIBLE) && Monst->_mgoal == MGOAL_TALKING) {
		Monst->mtalkmsg = QUEST_BANNER11;
		Monst->_mgoal = MGOAL_INQUIRING;
	}

	if (Monst->mtalkmsg == QUEST_BANNER11 && quests[QTYPE_BOL]._qvar1 == 3) {
		Monst->mtalkmsg = 0;
		Monst->_mgoal = MGOAL_NORMAL;
	}

	if (dFlags[mx][my] & DFLAG_VISIBLE) {
		if (Monst->mtalkmsg == QUEST_BANNER12) {
			if (!effect_is_playing(USFX_SNOT3) && Monst->_mgoal == MGOAL_TALKING) {
				ObjChangeMap(setpc_x, setpc_y, setpc_w + setpc_x + 1, setpc_h + setpc_y + 1);
				quests[QTYPE_BOL]._qvar1 = 3;
				RedoPlayerVision();
				Monst->_msquelch = -1;
				Monst->mtalkmsg = 0;
				Monst->_mgoal = MGOAL_NORMAL;
			}
		}
		if (quests[QTYPE_BOL]._qvar1 == 3) {
			if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_SHOOT)
				MAI_Fallen(i);
		}
	}

	Monst->_mdir = md;

	if (monster[i]._mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void __fastcall MAI_Lazurus(int i)
{
	int mx, my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Lazurus: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	md = M_GetDir(i);
	if (dFlags[mx][my] & DFLAG_VISIBLE) {
		if (gbMaxPlayers == 1) {
			if (Monst->mtalkmsg == QUEST_VILE13 && Monst->_mgoal == MGOAL_INQUIRING && plr[myplr].WorldX == QUEST_VILE13 && plr[myplr].WorldY == 46) {
				PlayInGameMovie("gendata\\fprst3.smk");
				Monst->_mmode = MM_TALK;
				quests[QTYPE_VB]._qvar1 = 5;
			}

			if (Monst->mtalkmsg == QUEST_VILE13 && !effect_is_playing(USFX_LAZ1) && Monst->_mgoal == MGOAL_TALKING) {
				ObjChangeMapResync(1, 18, 20, 24);
				RedoPlayerVision();
				Monst->_msquelch = -1;
				Monst->mtalkmsg = 0;
				quests[QTYPE_VB]._qvar1 = 6;
				Monst->_mgoal = MGOAL_NORMAL;
			}
		}

		if (gbMaxPlayers != 1 && Monst->mtalkmsg == QUEST_VILE13 && Monst->_mgoal == MGOAL_INQUIRING && quests[QTYPE_VB]._qvar1 <= 3) {
			Monst->_mmode = MM_TALK;
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_RETREAT || Monst->_mgoal == MGOAL_MOVE) {
		Monst->mtalkmsg = 0;
		MAI_Counselor(i);
	}

	Monst->_mdir = md;

	if (monster[i]._mmode == MM_STAND || monster[i]._mmode == MM_TALK)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
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
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Lazhelp: Invalid monster %d", i);
	v2 = v1;
	if (monster[v2]._mmode == MM_STAND) {
		v3 = monster[v2]._my;
		v4 = monster[v2]._mx;
		v5 = M_GetDir(ia);
		if (dFlags[v4][v3] & DFLAG_VISIBLE) {
			if (gbMaxPlayers == 1) {
				if (quests[QTYPE_VB]._qvar1 <= 5u) {
					_LOBYTE(monster[v2]._mgoal) = MGOAL_INQUIRING;
					goto LABEL_10;
				}
				monster[v2].mtalkmsg = 0;
			}
			_LOBYTE(monster[v2]._mgoal) = MGOAL_NORMAL;
		}
	LABEL_10:
		if (_LOBYTE(monster[v2]._mgoal) == MGOAL_NORMAL)
			MAI_Succ(ia);
		monster[v2]._mdir = v5;
		if (monster[v2]._mmode == MM_STAND)
			monster[v2]._mAnimData = monster[v2].MType->Anims[MA_STAND].Data[v5];
	}
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall MAI_Lachdanan(int i)
{
	int _mx, _my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Lachdanan: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND) {
		return;
	}

	_mx = Monst->_mx;
	_my = Monst->_my;
	md = M_GetDir(i);
	if (Monst->mtalkmsg == QUEST_VEIL9 && !(dFlags[_mx][_my] & DFLAG_VISIBLE) && Monst->_mgoal == MGOAL_TALKING) {
		Monst->mtalkmsg = QUEST_VEIL10;
		Monst->_mgoal = MGOAL_INQUIRING;
	}

	if (dFlags[_mx][_my] & DFLAG_VISIBLE) {
		if (Monst->mtalkmsg == QUEST_VEIL11) {
			if (!effect_is_playing(USFX_LACH3) && Monst->_mgoal == MGOAL_TALKING) {
				Monst->mtalkmsg = 0;
				quests[QTYPE_VEIL]._qactive = 3;
				M_StartKill(i, -1);
			}
		}
	}

	Monst->_mdir = md;

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void __fastcall MAI_Warlord(int i)
{
	MonsterStruct *Monst;
	int mx, my, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Warlord: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	md = M_GetDir(i);
	if (dFlags[mx][my] & DFLAG_VISIBLE) {
		if (Monst->mtalkmsg == QUEST_WARLRD9 && Monst->_mgoal == MGOAL_INQUIRING)
			Monst->_mmode = MM_TALK;
		if (Monst->mtalkmsg == QUEST_WARLRD9 && !effect_is_playing(USFX_WARLRD1) && Monst->_mgoal == MGOAL_TALKING) {
			Monst->_msquelch = -1;
			Monst->mtalkmsg = 0;
			Monst->_mgoal = MGOAL_NORMAL;
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL)
		MAI_SkelSd(i);

	Monst->_mdir = md;

	if (monster[i]._mmode == MM_STAND || monster[i]._mmode == MM_TALK)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
}

void __cdecl DeleteMonsterList()
{
	int i;
	for (i = 0; i < 4; i++) {
		if (monster[i]._mDelFlag) {
			monster[i]._mx = 1;
			monster[i]._my = 0;
			monster[i]._mfutx = 0;
			monster[i]._mfuty = 0;
			monster[i]._moldx = 0;
			monster[i]._moldy = 0;
			monster[i]._mDelFlag = FALSE;
		}
	}

	i = 4;
	while (i < nummonsters) {
		if (monster[monstactive[i]]._mDelFlag) {
			DeleteMonster(i);
			i = 0; // TODO: check if this should be i=4.
		} else {
			i++;
		}
	}
}

void __cdecl ProcessMonsters()
{
	int v0;          // edi
	int v1;          // esi
	int v2;          // ecx
	int v3;          // eax
	char *v4;        // ebx
	unsigned int v5; // eax
	int v6;          // eax
	int v7;          // edx
	int v8;          // eax
	unsigned int v9; // eax
	int v10;         // eax
	BOOLEAN v11;     // zf
	char *v12;       // ecx
	char *v13;       // eax
	int v14;         // ecx
	int v15;         // eax
	char v16;        // al
	int v17;         // ecx
	BOOLEAN v18;     // eax
	int v19;         // eax
	int v20;         // ecx
	int *v21;        // eax
	int *v22;        // eax
	int v23;         // [esp+0h] [ebp-Ch]
	int v24;         // [esp+4h] [ebp-8h]
	int v25;         // [esp+8h] [ebp-4h]

	DeleteMonsterList();
	v24 = 0;
	if (nummonsters <= 0)
		goto LABEL_60;
	do {
		v25 = 0;
		v23 = monstactive[v24];
		v0 = v23;
		v1 = v23;
		if ((unsigned char)gbMaxPlayers > 1u) {
			SetRndSeed(monster[v1]._mAISeed);
			monster[v1]._mAISeed = GetRndSeed();
		}
		if (!(monster[v1]._mFlags & MFLAG_NOHEAL)) {
			v2 = monster[v1]._mhitpoints;
			if (v2 < monster[v1]._mmaxhp && v2 >> 6 > 0) {
				v3 = SLOBYTE(monster[v1].mLevel);
				if ((char)v3 > 1)
					v3 = (char)v3 >> 1;
				monster[v1]._mhitpoints = v2 + v3;
			}
		}
		v4 = &dFlags[monster[v1]._mx][monster[v1]._my];
		if (*v4 & DFLAG_VISIBLE && !monster[v1]._msquelch && monster[v1].MType->mtype == MT_CLEAVER)
			PlaySFX(USFX_CLEAVER);
		if (monster[v1]._mFlags & MFLAG_TARGETS_MONSTER) {
			v5 = monster[v1]._menemy;
			if (v5 >= MAXMONSTERS)
				app_fatal("Illegal enemy monster %d for monster \"%s\"", v5, monster[v1].mName);
			v6 = monster[v1]._menemy;
			v7 = monster[v6]._mfutx;
			monster[v1]._lastx = v7;
			monster[v1]._menemyx = v7;
			v8 = monster[v6]._mfuty;
			monster[v1]._menemyy = v8;
			monster[v1]._lasty = v8;
		} else {
			v9 = monster[v1]._menemy;
			if (v9 >= MAX_PLRS)
				app_fatal("Illegal enemy player %d for monster \"%s\"", v9, monster[v1].mName);
			v10 = monster[v1]._menemy;
			v11 = (*v4 & DFLAG_VISIBLE) == 0;
			v12 = (char *)&plr[v10]._px;
			v13 = (char *)&plr[v10]._py;
			monster[v1]._menemyx = *v12;
			monster[v1]._menemyy = *v13;
			if (v11) {
				v16 = monster[v1]._msquelch;
				if (v16 && monster[v1]._mAi != MT_DIABLO) /// BUGFIX: test `MT_DIABLO` with 'MType->mtype' instead of '_mAi'
					monster[v1]._msquelch = v16 - 1;
			} else {
				v14 = *(_DWORD *)v12;
				v15 = *(_DWORD *)v13;
				monster[v1]._msquelch = -1;
				monster[v1]._lastx = v14;
				monster[v1]._lasty = v15;
			}
			v0 = v23;
		}
		while (1) {
			v17 = v0;
			if (monster[v1]._mFlags & MFLAG_SEARCH) {
				v18 = MAI_Path(v0);
				if (v18)
					goto LABEL_30;
				v17 = v0;
			}
			AiProc[(unsigned char)monster[v1]._mAi](v17);
		LABEL_30:
			switch (monster[v1]._mmode) {
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
			}
			if (!v25)
				break;
			GroupUnity(v0);
		}
	LABEL_51:
		if (monster[v1]._mmode != MM_STONE) {
			v20 = monster[v1]._mFlags;
			v21 = &monster[v1]._mAnimCnt;
			++*v21;
			if (!(v20 & MFLAG_ALLOW_SPECIAL) && monster[v1]._mAnimCnt >= monster[v1]._mAnimDelay) {
				*v21 = 0;
				v22 = &monster[v1]._mAnimFrame;
				if (v20 & MFLAG_LOCK_ANIMATION) {
					v11 = (*v22)-- == 1;
					if (v11)
						*v22 = monster[v1]._mAnimLen;
				} else if (++*v22 > monster[v1]._mAnimLen) {
					*v22 = 1;
				}
			}
		}
		++v24;
	} while (v24 < nummonsters);
LABEL_60:
	DeleteMonsterList();
}
// 679660: using guessed type char gbMaxPlayers;

void __cdecl FreeMonsters()
{
	int mtype;
	int i, j;
	void *ptr;

	for (i = 0; i < nummtypes; i++) {
		mtype = Monsters[i].mtype;
		for (j = 0; j < 6; j++) {
			if (animletter[j] != 's' || monsterdata[mtype].has_special) {
				ptr = Monsters[i].Anims[j].CMem;
				Monsters[i].Anims[j].CMem = NULL;
				mem_free_dbg(ptr);
			}
		}
	}

	FreeMissiles2();
}

BOOL __fastcall DirOK(int i, int mdir)
{
	int v2;             // ebx
	int v3;             // esi
	int v4;             // ebx
	int v5;             // edi
	int v6;             // esi
	int v7;             // edi
	BOOLEAN v8;         // zf
	int v9;             // edx
	unsigned char *v11; // ebx
	unsigned char v12;  // al
	int v13;            // edx
	int v14;            // eax
	int v15;            // edi
	int v16;            // ecx
	signed int j;       // esi
	int v18;            // eax
	BOOLEAN v19;        // zf
	int v20;            // eax
	int v21;            // [esp+Ch] [ebp-14h]
	int v22;            // [esp+10h] [ebp-10h]
	int v23;            // [esp+14h] [ebp-Ch]
	int a1;             // [esp+18h] [ebp-8h]
	int v25;            // [esp+1Ch] [ebp-4h]
	int v26;            // [esp+1Ch] [ebp-4h]

	v2 = i;
	v3 = mdir;
	v25 = mdir;
	a1 = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("DirOK: Invalid monster %d", i);
	v4 = v2;
	v5 = offset_y[v3];
	v6 = monster[v4]._mx + offset_x[v3];
	v7 = monster[v4]._my + v5;
	if (v7 < 0 || v7 >= MAXDUNY || v6 < 0 || v6 >= MAXDUNX || !PosOkMonst(a1, v6, v7))
		return 0;
	if (v25 == DIR_E) {
		if (!SolidLoc(v6, v7 + 1)) {
			v8 = (dFlags[v6][v7 + 1] & DFLAG_MONSTER) == 0;
			goto LABEL_18;
		}
		return 0;
	}
	if (v25 == DIR_W) {
		if (SolidLoc(v6 + 1, v7))
			return 0;
		v8 = (dFlags[v6 + 1][v7] & DFLAG_MONSTER) == 0;
	} else {
		if (v25 == DIR_N) {
			if (SolidLoc(v6 + 1, v7))
				return 0;
			v9 = v7 + 1;
		} else {
			if (v25)
				goto LABEL_24;
			if (SolidLoc(v6 - 1, v7))
				return 0;
			v9 = v7 - 1;
		}
		v8 = SolidLoc(v6, v9) == 0;
	}
LABEL_18:
	if (!v8)
		return 0;
LABEL_24:
	if (monster[v4].leaderflag == 1) {
		v11 = &monster[v4].leader;
		if (abs(v6 - monster[(unsigned char)*v11]._mfutx) >= 4
		    || abs(v7 - monster[(unsigned char)*v11]._mfuty) >= 4) {
			return 0;
		}
		return 1;
	}
	v12 = monster[v4]._uniqtype;
	if (v12 == 0 || !(UniqMonst[v12 - 1].mUnqAttr & 2))
		return 1;
	v26 = 0;
	v13 = v6 - 3;
	v21 = v6 + 3;
	if (v6 - 3 <= v6 + 3) {
		v14 = v7 - 3;
		v15 = v7 + 3;
		v23 = v14;
		v22 = v15;
		v16 = 112 * v13;
		do {
			for (j = v23; j <= v15; ++j) {
				if (j >= 0 && j < MAXDUNY && v16 >= 0 && v16 < MAXDUNX * 112) {
					v18 = dMonster[0][v16 + j];
					v19 = v18 == 0;
					if (v18 < 0) {
						v18 = -v18;
						v19 = v18 == 0;
					}
					if (!v19)
						--v18;
					v20 = v18;
					if (monster[v20].leaderflag == 1
					    && (unsigned char)monster[v20].leader == a1
					    && monster[v20]._mfutx == v13
					    && monster[v20]._mfuty == j) {
						++v26;
					}
				}
				v15 = v22;
			}
			++v13;
			v16 += 112;
		} while (v13 <= v21);
	}
	return v26 == (unsigned char)monster[v4].packsize;
}

BOOL __fastcall PosOkMissile(int x, int y)
{
	return !nMissileTable[dPiece[x][y]] && !(dFlags[x][y] & DFLAG_MONSTER);
}

BOOL __fastcall CheckNoSolid(int x, int y)
{
	return nSolidTable[dPiece[x][y]] == 0;
}

BOOL __fastcall LineClearF(BOOL(__fastcall *Clear)(int, int), int x1, int y1, int x2, int y2)
{
	int v5;         // esi
	int v6;         // edi
	int v7;         // ebx
	int v8;         // eax
	int v9;         // eax
	int v10;        // eax
	int v11;        // ebx
	int v12;        // esi
	signed int v13; // edi
	int v14;        // edx
	int v15;        // ecx
	int v16;        // eax
	int v17;        // eax
	int v18;        // eax
	int v19;        // ebx
	int v20;        // edi
	signed int v21; // esi
	int v22;        // ecx
	int v25;        // [esp+10h] [ebp-10h]
	int v26;        // [esp+14h] [ebp-Ch]
	int v27;        // [esp+18h] [ebp-8h]
	int v28;        // [esp+18h] [ebp-8h]
	int v29;        // [esp+1Ch] [ebp-4h]

	v5 = y2 - y1;
	v29 = x1;
	v25 = x1;
	v26 = y1;
	v6 = x2 - x1;
	v7 = abs(y2 - y1);
	if (abs(v6) <= v7) {
		if (v5 < 0) {
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
		if (v6 <= 0) {
			v19 = v18 + v5;
			v20 = 2 * (v5 + v6);
			v21 = -1;
		} else {
			v19 = v18 - v5;
			v20 = 2 * (v6 - v5);
			v21 = 1;
		}
		while (1) {
			v22 = v29;
			if (y1 == y2 && v29 == x2)
				break;
			if (v19 <= 0 == v21 < 0) {
				v19 += v20;
				v22 = v21 + v29;
				v29 += v21;
			} else {
				v19 += v28;
			}
			if ((++y1 != v26 || v22 != v25) && !Clear(v22, y1)) /* check args */
				goto LABEL_29;
		}
	} else {
		if (v6 < 0) {
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
		if (v5 <= 0) {
			v11 = v10 + v6;
			v12 = 2 * (v6 + v5);
			v13 = -1;
		} else {
			v11 = v10 - v6;
			v12 = 2 * (v5 - v6);
			v13 = 1;
		}
		do {
			v14 = y1;
			if (v29 == x2 && y1 == y2)
				break;
			if (v11 <= 0 == v13 < 0) {
				v11 += v12;
				v14 = v13 + y1;
				y1 += v13;
			} else {
				v11 += v27;
			}
			v15 = v29 + 1;
		} while (++v29 == v25 && v14 == v26 || Clear(v15, v14));
	LABEL_29:
		if (v29 != x2)
			return 0;
	}
	if (y1 == y2)
		return 1;
	return 0;
}

BOOL __fastcall LineClear(int x1, int y1, int x2, int y2)
{
	return LineClearF(PosOkMissile, x1, y1, x2, y2);
}

BOOL __fastcall LineClearF1(BOOL(__fastcall *Clear)(int, int, int), int monst, int x1, int y1, int x2, int y2)
{
	int v6;         // esi
	int v7;         // edi
	int v8;         // ebx
	int v9;         // eax
	int v10;        // eax
	int v11;        // eax
	int v12;        // ebx
	int v13;        // esi
	signed int v14; // edi
	int v15;        // eax
	int v16;        // eax
	int v17;        // eax
	int v18;        // eax
	int v19;        // ebx
	int v20;        // edi
	signed int v21; // esi
	int v22;        // edx
	int v25;        // [esp+10h] [ebp-10h]
	int v26;        // [esp+14h] [ebp-Ch]
	int v27;        // [esp+18h] [ebp-8h]
	int v28;        // [esp+1Ch] [ebp-4h]
	int v29;        // [esp+1Ch] [ebp-4h]

	v6 = y2 - y1;
	v25 = monst;
	v26 = x1;
	v27 = y1;
	v7 = x2 - x1;
	v8 = abs(y2 - y1);
	if (abs(x2 - x1) <= v8) {
		if (v6 < 0) {
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
		if (v7 <= 0) {
			v19 = v18 + v6;
			v20 = 2 * (v6 + v7);
			v21 = -1;
		} else {
			v19 = v18 - v6;
			v20 = 2 * (v7 - v6);
			v21 = 1;
		}
		while (1) {
			v22 = x1;
			if (y1 == y2 && x1 == x2)
				break;
			if (v19 <= 0 == v21 < 0) {
				v19 += v20;
				v22 = v21 + x1;
				x1 += v21;
			} else {
				v19 += v29;
			}
			if ((++y1 != v27 || v22 != v26) && !Clear(v25, v22, y1))
				goto LABEL_29;
		}
	} else {
		if (v7 < 0) {
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
		if (v6 <= 0) {
			v12 = v11 + v7;
			v13 = 2 * (v7 + v6);
			v14 = -1;
		} else {
			v12 = v11 - v7;
			v13 = 2 * (v6 - v7);
			v14 = 1;
		}
		do {
			v15 = y1;
			if (x1 == x2 && y1 == y2)
				break;
			if (v12 <= 0 == v14 < 0) {
				v12 += v13;
				v15 = v14 + y1;
				y1 += v14;
			} else {
				v12 += v28;
			}
		} while (++x1 == v26 && v15 == v27 || Clear(v25, x1, v15)); /* check args */
	LABEL_29:
		if (x1 != x2)
			return 0;
	}
	if (y1 == y2)
		return 1;
	return 0;
}

void __fastcall SyncMonsterAnim(int i)
{
	int v1;             // esi
	int v2;             // eax
	int v3;             // edx
	MonsterData *v4;    // esi
	CMonster *v5;       // ecx
	unsigned char v6;   // dl
	char *v7;           // edx
	int v8;             // esi
	int v9;             // edx
	unsigned char *v10; // ecx
	unsigned char *v11; // edx
	int v12;            // ecx
	unsigned char *v13; // edx

	v1 = i;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("SyncMonsterAnim: Invalid monster %d", i);
	v2 = v1;
	v3 = monster[v1]._mMTidx;
	v4 = Monsters[v3].MData;
	v5 = &Monsters[v3];
	v6 = monster[v2]._uniqtype;
	monster[v2].MType = v5;
	monster[v2].MData = v4;
	if (v6 != 0)
		v7 = UniqMonst[v6 - 1].mName;
	else
		v7 = v4->mName;
	v8 = monster[v2]._mmode;
	monster[v2].mName = v7;
	v9 = monster[v2]._mdir;
	switch (v8) {
	case MM_STAND:
	case MM_DELAY:
	case MM_TALK:
		v10 = v5->Anims[MA_STAND].Data[v9];
		goto LABEL_13;
	case MM_WALK:
	case MM_WALK2:
	case MM_WALK3:
		v10 = v5->Anims[MA_WALK].Data[v9];
		goto LABEL_13;
	case MM_ATTACK:
	case MM_RATTACK:
		v10 = v5->Anims[MA_ATTACK].Data[v9];
		goto LABEL_13;
	case MM_GOTHIT:
		v10 = v5->Anims[MA_GOTHIT].Data[v9];
		goto LABEL_13;
	case MM_DEATH:
		v10 = v5->Anims[MA_DEATH].Data[v9];
		goto LABEL_13;
	case MM_SATTACK:
	case MM_FADEIN:
	case MM_FADEOUT:
	case MM_SPSTAND:
	case MM_RSPATTACK:
	case MM_HEAL:
		v10 = v5->Anims[MA_SPECIAL].Data[v9];
	LABEL_13:
		monster[v2]._mAnimData = v10;
		return;
	case MM_CHARGE:
		v11 = v5->Anims[MA_ATTACK].Data[v9];
		monster[v2]._mAnimFrame = 1;
		monster[v2]._mAnimData = v11;
		v12 = v5->Anims[MA_ATTACK].Frames;
		break;
	default:
		v13 = v5->Anims[MA_STAND].Data[v9];
		monster[v2]._mAnimFrame = 1;
		monster[v2]._mAnimData = v13;
		v12 = v5->Anims[MA_STAND].Frames;
		break;
	}
	monster[v2]._mAnimLen = v12;
}

void __fastcall M_FallenFear(int x, int y)
{
	int v2;         // eax
	int *v3;        // ebx
	int v4;         // edi
	int v5;         // esi
	signed int v6;  // eax
	int v7;         // eax
	BOOLEAN v8;     // zf
	int v9;         // eax
	int v10;        // eax
	signed int v11; // [esp-10h] [ebp-1Ch]
	int v12;        // [esp+0h] [ebp-Ch]
	int x1;         // [esp+4h] [ebp-8h]
	int y1;         // [esp+8h] [ebp-4h]

	v2 = 0;
	y1 = y;
	x1 = x;
	v12 = 0;
	if (nummonsters > 0) {
		v3 = &monster[0]._mx;
		do {
			v4 = 0;
			v5 = monstactive[v2];
			v6 = monster[v5].MType->mtype;
			if (v6 > MT_RFALLSD) {
				v9 = v6 - 13;
				v8 = v9 == 0;
			} else {
				if (v6 == MT_RFALLSD || (v7 = v6 - 4) == 0) {
					v11 = 7;
					goto LABEL_15;
				}
				v9 = v7 - 1;
				v8 = v9 == 0;
			}
			if (v8) {
				v11 = 5;
			} else {
				v10 = v9 - 1;
				if (v10) {
					if (v10 != 1)
						goto LABEL_16;
					v11 = 2;
				} else {
					v11 = 3;
				}
			}
		LABEL_15:
			v4 = v11;
		LABEL_16:
			if (monster[v5]._mAi == AI_FALLEN
			    && v4
			    && abs(x1 - monster[v5]._mx) < 5
			    && abs(y1 - monster[v5]._my) < 5
			    && monster[v5]._mhitpoints >> 6 > 0) {
				_LOBYTE(monster[v5]._mgoal) = MGOAL_RETREAT;
				monster[v5]._mgoalvar1 = v4;
				monster[v5]._mdir = GetDirection(x1, y1, *v3, v3[1]);
			}
			v3 += 57;
			v2 = v12++ + 1;
		} while (v12 < nummonsters);
	}
}

void __fastcall PrintMonstHistory(int mt)
{
	int v1;   // edi
	int *v2;  // ebx
	int v3;   // ecx
	int v4;   // eax
	int v5;   // edi
	short v6; // bx
	int v7;   // ebx

	v1 = mt;
	v2 = &monstkills[mt];
	sprintf(tempstr, "Total kills : %i", *v2);
	AddPanelString(tempstr, 1);
	if (*v2 >= 30) {
		v3 = monsterdata[v1].mMinHP;
		v4 = monsterdata[v1].mMaxHP;
		if (gbMaxPlayers == 1) {
			v3 = monsterdata[v1].mMinHP >> 1;
			v4 = monsterdata[v1].mMaxHP >> 1;
		}
		if (v3 < 1)
			v3 = 1;
		if (v4 < 1)
			v4 = 1;
		if (gnDifficulty == DIFF_NIGHTMARE) {
			v3 = 3 * v3 + 1;
			v4 = 3 * v4 + 1;
		}
		if (gnDifficulty == DIFF_HELL) {
			v3 = 4 * v3 + 3;
			v4 = 4 * v4 + 3;
		}
		sprintf(tempstr, "Hit Points : %i-%i", v3, v4);
		AddPanelString(tempstr, 1);
	}
	if (*v2 >= 15) {
		v5 = v1 << 7;
		if (gnDifficulty == DIFF_HELL)
			v6 = *(short *)((char *)&monsterdata[0].mMagicRes2 + v5);
		else
			v6 = *(short *)((char *)&monsterdata[0].mMagicRes + v5);
		v7 = v6 & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING);
		if (v7) {
			if (v7 & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING)) {
				strcpy(tempstr, "Resists : ");
				if (v7 & RESIST_MAGIC)
					strcat(tempstr, "Magic ");
				if (v7 & RESIST_FIRE)
					strcat(tempstr, "Fire ");
				if (v7 & RESIST_LIGHTNING)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, 1);
			}
			if (v7 & (IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING)) {
				strcpy(tempstr, "Immune : ");
				if (v7 & IMUNE_MAGIC)
					strcat(tempstr, "Magic ");
				if (v7 & IMUNE_FIRE)
					strcat(tempstr, "Fire ");
				if (v7 & IMUNE_LIGHTNING)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, 1);
			}
		} else {
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

	v0 = monster[pcursmonst].mMagicRes & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING);
	if (v0) {
		if (monster[pcursmonst].mMagicRes & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING))
			strcpy(tempstr, "Some Magic Resistances");
		else
			strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, 1);
		if (v0 & (IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING)) {
			strcpy(tempstr, "Some Magic Immunities");
			goto LABEL_4;
		}
	} else {
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
	int v3;            // edi
	MissileStruct *v4; // edi
	unsigned int v5;   // ebx
	MonsterStruct *v6; // esi
	int v7;            // edx
	char v8;           // al
	int v9;            // eax
	char *v10;         // edi
	int v11;           // eax
	int v12;           // edx
	char v13;          // al
	char v14;          // al
	int v15;           // ebx
	int v16;           // eax
	int v17;           // esi
	int v18;           // edi
	int v19;           // esi
	int v20;           // edx
	int *v21;          // ebx
	char v22;          // cl
	char v23;          // al
	int v24;           // esi
	int v25;           // edi
	int v26;           // esi
	int v27;           // eax
	int v28;           // eax
	int ia;            // [esp+Ch] [ebp-10h]
	int v30;           // [esp+10h] [ebp-Ch]
	int v31;           // [esp+14h] [ebp-8h]
	int v32;           // [esp+18h] [ebp-4h]
	int arglist;       // [esp+24h] [ebp+8h]

	v3 = i;
	v30 = x;
	if ((DWORD)i >= MAXMISSILES)
		app_fatal("MissToMonst: Invalid missile %d", i);
	v4 = &missile[v3];
	v5 = v4->_misource;
	ia = v4->_misource;
	if (v5 >= MAXMONSTERS)
		app_fatal("MissToMonst: Invalid monster %d", v5);
	v32 = v4->_mix;
	v31 = v4->_miy;
	v6 = &monster[v5];
	v6->_mx = v30;
	dMonster[v30][y] = v5 + 1;
	v7 = v4->_mimfnum;
	v6->_mdir = v7;
	v6->_my = y;
	M_StartStand(v5, v7);
	v8 = v6->MType->mtype;
	if (v8 < MT_INCIN || v8 > MT_HELLBURN) {
		if (v6->_mFlags & MFLAG_TARGETS_MONSTER)
			M2MStartHit(v5, -1, 0);
		else
			M_StartHit(v5, -1, 0);
	} else {
		M_StartFadein(v5, v6->_mdir, FALSE);
	}
	v9 = v32;
	if (v6->_mFlags & MFLAG_TARGETS_MONSTER) {
		v21 = (int *)((char *)dMonster + 4 * (v31 + v9 * 112));
		if (*v21 > 0) {
			v22 = v6->MType->mtype;
			if (v22 != MT_GLOOM && (v22 < MT_INCIN || v22 > MT_HELLBURN)) {
				M_TryM2MHit(ia, *v21 - 1, 500, (unsigned char)v6->mMinDamage2, (unsigned char)v6->mMaxDamage2);
				v23 = v6->MType->mtype;
				if (v23 < MT_NSNAKE || v23 > MT_GSNAKE) {
					v24 = v6->_mdir;
					v25 = v32 + offset_x[v24];
					v26 = v31 + offset_y[v24];
					if (PosOkMonst(*v21 - 1, v25, v26)) {
						v27 = *v21;
						dMonster[v25][v26] = *v21;
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
	} else {
		v10 = &dPlayer[v9][v31];
		v11 = *v10;
		v12 = v11 - 1;
		arglist = v11 - 1;
		if (*v10 > 0) {
			v13 = v6->MType->mtype;
			if (v13 != MT_GLOOM && (v13 < MT_INCIN || v13 > MT_HELLBURN)) {
				M_TryH2HHit(v5, v12, 500, (unsigned char)v6->mMinDamage2, (unsigned char)v6->mMaxDamage2);
				if (arglist == *v10 - 1) {
					v14 = v6->MType->mtype;
					if (v14 < MT_NSNAKE || v14 > MT_GSNAKE) {
						v15 = arglist;
						v16 = plr[arglist]._pmode;
						if (v16 != 7 && v16 != 8)
							StartPlrHit(arglist, 0, 1u);
						v17 = v6->_mdir;
						v18 = v32 + offset_x[v17];
						v19 = v31 + offset_y[v17];
						if (PosOkPlayer(arglist, v18, v19)) {
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
	int oi, mi, j;
	BOOL ret, fire;

	fire = FALSE;
	ret = !SolidLoc(x, y) && !dPlayer[x][y] && !dMonster[x][y];
	if (ret && dObject[x][y]) {
		oi = dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1);
		if (object[oi]._oSolidFlag)
			ret = FALSE;
	}

	if (ret && dMissile[x][y] && i >= 0) {
		mi = dMissile[x][y];
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MIS_FIREWALL) { // BUGFIX: 'mi' should be 'mi - 1' (corrected)
				fire = TRUE;
			} else {
				for (j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MIS_FIREWALL)
						fire = TRUE;
				}
			}
		}
		if (fire && (!(monster[i].mMagicRes & IMUNE_FIRE) || monster[i].MType->mtype == MT_DIABLO))
			ret = FALSE;
	}

	return ret;
}

BOOL __fastcall PosOkMonst2(int i, int x, int y)
{
	int oi, mi, j;
	BOOL ret, fire;

	fire = FALSE;
	ret = !SolidLoc(x, y); //12-15
	if (ret && dObject[x][y]) {
		oi = dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1);
		if (object[oi]._oSolidFlag)
			ret = FALSE;
	}

	if (ret && dMissile[x][y] && i >= 0) { //37
		mi = dMissile[x][y];
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MIS_FIREWALL) { // BUGFIX: 'mi' should be 'mi - 1' (corrected)
				fire = TRUE;
			} else {
				for (j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MIS_FIREWALL)
						fire = TRUE;
				}
			}
		}
		if (fire && (!(monster[i].mMagicRes & IMUNE_FIRE) || monster[i].MType->mtype == MT_DIABLO))
			ret = FALSE;
	}

	return ret;
}

BOOL __fastcall PosOkMonst3(int i, int x, int y)
{
	int oi, mi, j, objtype;
	BOOL ret, fire, isdoor;

	fire = FALSE;
	isdoor = FALSE;
	if (dObject[x][y]) {
		oi = dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1);
		objtype = object[oi]._otype;
		isdoor = TRUE;
		if (objtype != OBJ_L1LDOOR
		    && objtype != OBJ_L1RDOOR
		    && objtype != OBJ_L2LDOOR
		    && objtype != OBJ_L2RDOOR
		    && objtype != OBJ_L3LDOOR
		    && objtype != OBJ_L3RDOOR) {
			isdoor = FALSE;
		}
		if (object[oi]._oSolidFlag && !isdoor)
			return FALSE;
	}

	if (SolidLoc(x, y) && !isdoor || dPlayer[x][y] || dMonster[x][y])
		return FALSE;

	ret = TRUE;
	if (dMissile[x][y] && i >= 0) {
		mi = dMissile[x][y];
		if (mi > 0) {
			if (missile[mi]._mitype == MIS_FIREWALL) {
				fire = TRUE;
			} else {
				for (j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MIS_FIREWALL)
						fire = TRUE;
				}
			}
		}
		if (fire && (!(monster[i].mMagicRes & IMUNE_FIRE) || monster[i].MType->mtype == MT_DIABLO))
			ret = FALSE;
	}
	return ret;
}

BOOL __fastcall IsSkel(int mt)
{
	return mt >= MT_WSKELAX && mt <= MT_XSKELAX
	    || mt >= MT_WSKELBW && mt <= MT_XSKELBW
	    || mt >= MT_WSKELSD && mt <= MT_XSKELSD;
}

BOOL __fastcall IsGoat(int mt)
{
	return mt >= MT_NGOATMC && mt <= MT_GGOATMC
	    || mt >= MT_NGOATBW && mt <= MT_GGOATBW;
}

int __fastcall M_SpawnSkel(int x, int y, int dir)
{
	int i, j, skeltypes, skel;

	j = 0;
	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(Monsters[i].mtype))
			j++;
	}

	if (j) {
		skeltypes = random(136, j);
		j = 0;
		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
			if (IsSkel(Monsters[i].mtype))
				j++;
		}
		skel = AddMonster(x, y, dir, i - 1, 1);
		if (skel != -1)
			M_StartSpStand(skel, dir);

		return skel;
	}

	return -1;
}

void __fastcall ActivateSpawn(int i, int x, int y, int dir)
{
	dMonster[x][y] = i + 1;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
	M_StartSpStand(i, dir);
}

BOOL __fastcall SpawnSkeleton(int ii, int x, int y)
{
	int dx, dy, xx, yy, dir, j, k, rs;
	BOOL savail;
	int monstok[3][3];

	if (ii == -1)
		return FALSE;

	if (PosOkMonst(-1, x, y)) {
		dir = GetDirection(x, y, x, y);
		ActivateSpawn(ii, x, y, dir);
		return TRUE;
	}

	savail = FALSE;
	yy = 0;
	for (j = y - 1; j <= y + 1; j++) {
		xx = 0;
		for (k = x - 1; k <= x + 1; k++) {
			monstok[xx][yy] = PosOkMonst(-1, k, j);
			savail |= monstok[xx][yy];
			xx++;
		}
		yy++;
	}
	if (!savail) {
		return FALSE;
	}

	rs = random(137, 15) + 1;
	xx = 0;
	yy = 0;
	while (rs > 0) {
		if (monstok[xx][yy])
			rs--;
		if (rs > 0) {
			xx++;
			if (xx == 3) {
				xx = 0;
				yy++;
				if (yy == 3)
					yy = 0;
			}
		}
	}

	dx = x - 1 + xx;
	dy = y - 1 + yy;
	dir = GetDirection(dx, dy, x, y);
	ActivateSpawn(ii, dx, dy, dir);

	return TRUE;
}

int __cdecl PreSpawnSkeleton()
{
	int i, j, skeltypes, skel;

	j = 0;

	if (nummtypes <= 0)
		return -1;

	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(Monsters[i].mtype))
			j++;
	}

	if (j) {
		skeltypes = random(136, j);
		j = 0;
		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
			if (IsSkel(Monsters[i].mtype))
				j++;
		}
		skel = AddMonster(0, 0, 0, i - 1, 0);
		if (skel != -1)
			M_StartStand(skel, 0);

		return skel;
	}

	return -1;
}

void __fastcall TalktoMonster(int i)
{
	MonsterStruct *Monst;
	int pnum, itm;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("TalktoMonster: Invalid monster %d", i);

	Monst = &monster[i];
	pnum = Monst->_menemy;
	Monst->_mmode = MM_TALK;
	if (Monst->_mAi == AI_SNOTSPIL || Monst->_mAi == AI_LACHDAN) {
		if (QuestStatus(QTYPE_BOL) && quests[QTYPE_BOL]._qvar1 == 2 && PlrHasItem(pnum, IDI_BANNER, &itm)) {
			RemoveInvItem(pnum, itm);
			quests[QTYPE_BOL]._qactive = 3;
			Monst->mtalkmsg = QUEST_BANNER12;
			Monst->_mgoal = MGOAL_INQUIRING;
		}
		if (QuestStatus(QTYPE_VEIL) && Monst->mtalkmsg >= QUEST_VEIL9) {
			if (PlrHasItem(pnum, IDI_GLDNELIX, &itm)) {
				RemoveInvItem(pnum, itm);
				Monst->mtalkmsg = QUEST_VEIL11;
				Monst->_mgoal = MGOAL_INQUIRING;
			}
		}
	}
}

void __fastcall SpawnGolum(int i, int x, int y, int mi)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("SpawnGolum: Invalid monster %d", i);

	dMonster[x][y] = i + 1;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
	monster[i]._pathcount = 0;
	monster[i]._mFlags |= MFLAG_GOLEM;
	monster[i].mArmorClass = 25;
	monster[i]._mmaxhp = 2 * (320 * missile[mi]._mispllvl + plr[i]._pMaxMana / 3);
	monster[i]._mhitpoints = monster[i]._mmaxhp;
	monster[i].mHit = 5 * (missile[mi]._mispllvl + 8) + 2 * plr[i]._pLevel;
	monster[i].mMinDamage = 2 * (missile[mi]._mispllvl + 4);
	monster[i].mMaxDamage = 2 * (missile[mi]._mispllvl + 8);
	M_StartSpStand(i, 0);
	M_Enemy(i);
	if (i == myplr) {
		NetSendCmdGolem(
		    monster[i]._mx,
		    monster[i]._my,
		    monster[i]._mdir,
		    monster[i]._menemy,
		    monster[i]._mhitpoints,
		    currlevel);
	}
}

BOOL __fastcall CanTalkToMonst(int m)
{
	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("CanTalkToMonst: Invalid monster %d", m);
	}

	if (monster[m]._mgoal == MGOAL_INQUIRING) {
		return TRUE;
	}

	return monster[m]._mgoal == MGOAL_TALKING;
}

BOOL __fastcall CheckMonsterHit(int m, BOOL *ret)
{
	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("CheckMonsterHit: Invalid monster %d", m);
	}

	if (monster[m]._mAi == AI_GARG && monster[m]._mFlags & MFLAG_ALLOW_SPECIAL) {
		monster[m]._mmode = MM_SATTACK;
		monster[m]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		*ret = TRUE;
		return TRUE;
	}

	if (monster[m].MType->mtype < MT_COUNSLR || monster[m].MType->mtype > MT_ADVOCATE || monster[m]._mgoal == MGOAL_NORMAL) {
		return FALSE;
	} else {
		*ret = FALSE;
	}

	return TRUE;
}

int __fastcall encode_enemy(int m)
{
	int enemy;

	enemy = monster[m]._menemy;
	if (monster[m]._mFlags & MFLAG_TARGETS_MONSTER)
		enemy += 4;

	return enemy;
}

void __fastcall decode_enemy(int m, int enemy)
{
	if (enemy < 4) {
		monster[m]._mFlags &= ~MFLAG_TARGETS_MONSTER;
		monster[m]._menemy = enemy;
		monster[m]._menemyx = plr[enemy]._px;
		monster[m]._menemyy = plr[enemy]._py;
	} else {
		monster[m]._mFlags |= MFLAG_TARGETS_MONSTER;
		enemy -= 4;
		monster[m]._menemy = enemy;
		monster[m]._menemyx = monster[enemy]._mfutx;
		monster[m]._menemyy = monster[enemy]._mfuty;
	}
}

DEVILUTION_END_NAMESPACE
