//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

int trapid;  // weak
int trapdir; // weak
unsigned char *pObjCels[40];
char ObjFileList[40];
int objectactive[MAXOBJECTS];
int nobjects; // idb
int leverid;  // idb
int objectavail[MAXOBJECTS];
ObjectStruct object[MAXOBJECTS];
BOOL InitObjFlag;
int numobjfiles; // weak

int ObjTypeConv[113] = {
	0,
	4,
	20,
	21,
	22,
	24,
	11,
	12,
	13,
	0,
	0,
	0,
	0,
	0,
	25,
	41,
	26,
	0,
	8,
	9,
	10,
	80,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	49,
	0,
	0,
	0,
	0,
	0,
	84,
	85,
	3,
	14,
	15,
	16,
	17,
	18,
	19,
	0,
	0,
	0,
	0,
	0,
	0,
	28,
	0,
	53,
	54,
	36,
	37,
	38,
	39,
	40,
	0,
	0,
	0,
	0,
	0,
	27,
	0,
	0,
	0,
	0,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	5,
	5,
	5,
	6,
	6,
	6,
	7,
	7,
	7,
	0,
	0,
	0,
	0,
	0,
	73,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	83,
	0,
	0,
	89,
	90,
	47,
	46,
	94
};
ObjDataStruct AllObjects[99] = {
	// clang-format off
	// oload, ofindex,   ominlvl, omaxlvl, olvltype, otheme, oquest, oAnimFlag, oAnimDelay, oAnimLen, oAnimWidth, oSolidFlag, oMissFlag, oLightFlag, oBreak, oSelFlag, oTrapFlag
	{      1, OFILE_L1BRAZ,    1,       4,        1,     -1,     -1,         1,          1,       26,         64, TRUE,       TRUE,      FALSE,           0,        0, FALSE     },
	{      1, OFILE_L1DOORS,   1,       4,        1,     -1,     -1,         0,          1,        0,         64, FALSE,      FALSE,     TRUE,            0,        3, TRUE      },
	{      1, OFILE_L1DOORS,   1,       4,        1,     -1,     -1,         0,          2,        0,         64, FALSE,      FALSE,     TRUE,            0,        3, TRUE      },
	{      3, OFILE_SKULFIRE,  0,       0,        0,      3,     -1,         1,          2,       11,         96, TRUE,       TRUE,      FALSE,           0,        0, FALSE     },
	{      1, OFILE_LEVER,     1,       4,        1,     -1,     -1,         0,          1,        1,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{      1, OFILE_CHEST1,    1,      16,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{      1, OFILE_CHEST2,    1,      16,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{      1, OFILE_CHEST3,    1,      16,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{      2, OFILE_L1BRAZ,    0,       0,        0,     -1,     -1,         0,          0,        0,          0, FALSE,      FALSE,     FALSE,           0,        0, FALSE     },
	{      3, OFILE_CANDLE2,   0,       0,        0,      1,     -1,         1,          2,        4,         96, TRUE,       TRUE,      TRUE,            0,        0, FALSE     },
	{      2, OFILE_L1BRAZ,    0,       0,        0,     -1,     -1,         0,          0,        0,          0, FALSE,      FALSE,     FALSE,           0,        0, FALSE     },
	{      3, OFILE_BANNER,    0,       0,        0,      3,     -1,         0,          2,        0,         96, TRUE,       TRUE,      TRUE,            0,        0, FALSE     },
	{      3, OFILE_BANNER,    0,       0,        0,      3,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        0, FALSE     },
	{      3, OFILE_BANNER,    0,       0,        0,      3,     -1,         0,          3,        0,         96, TRUE,       TRUE,      TRUE,            0,        0, FALSE     },
	{      2, OFILE_SKULPILE,  1,       4,        0,     -1,     -1,         0,          0,        1,         96, TRUE,       TRUE,      TRUE,            0,        0, FALSE     },
	{      2, OFILE_L1BRAZ,    0,       0,        0,     -1,     -1,         0,          0,        0,          0, FALSE,      FALSE,     FALSE,           0,        0, FALSE     },
	{      2, OFILE_L1BRAZ,    0,       0,        0,     -1,     -1,         0,          0,        0,          0, FALSE,      FALSE,     FALSE,           0,        0, FALSE     },
	{      2, OFILE_L1BRAZ,    0,       0,        0,     -1,     -1,         0,          0,        0,          0, FALSE,      FALSE,     FALSE,           0,        0, FALSE     },
	{      2, OFILE_L1BRAZ,    0,       0,        0,     -1,     -1,         0,          0,        0,          0, FALSE,      FALSE,     FALSE,           0,        0, FALSE     },
	{      2, OFILE_L1BRAZ,    0,       0,        0,     -1,     -1,         0,          0,        0,          0, FALSE,      FALSE,     FALSE,           0,        0, FALSE     },
	{      2, OFILE_CRUXSK1,   0,       0,        0,     -1,     -1,         0,          1,       15,         96, TRUE,       FALSE,     TRUE,            1,        3, FALSE     },
	{      2, OFILE_CRUXSK2,   0,       0,        0,     -1,     -1,         0,          1,       15,         96, TRUE,       FALSE,     TRUE,            1,        3, FALSE     },
	{      2, OFILE_CRUXSK3,   0,       0,        0,     -1,     -1,         0,          1,       15,         96, TRUE,       FALSE,     TRUE,            1,        3, FALSE     },
	{      1, OFILE_ROCKSTAN,  5,       5,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        0, FALSE     },
	{      2, OFILE_ANGEL,     0,       0,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      2, OFILE_BOOK2,     0,       0,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      2, OFILE_BURNCROS,  0,       0,        0,     -1,     -1,         1,          0,       10,        160, TRUE,       FALSE,     FALSE,           0,        0, FALSE     },
	{      2, OFILE_NUDE2,     0,       0,        0,     -1,     -1,         1,          3,        6,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_SWITCH4,  16,      16,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{      1, OFILE_TNUDEM,   13,      16,        0,     -1,      6,         0,          1,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TNUDEM,   13,      16,        0,      6,      6,         0,          2,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TNUDEM,   13,      16,        0,      6,      6,         0,          3,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TNUDEM,   13,      16,        0,      6,      6,         0,          4,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TNUDEW,   13,      16,        0,      6,      6,         0,          1,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TNUDEW,   13,      16,        0,      6,      6,         0,          2,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TNUDEW,   13,      16,        0,      6,      6,         0,          3,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TSOUL,    13,      16,        0,     -1,      6,         0,          1,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TSOUL,    13,      16,        0,     -1,      6,         0,          2,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TSOUL,    13,      16,        0,     -1,      6,         0,          3,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TSOUL,    13,      16,        0,     -1,      6,         0,          4,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_TSOUL,    13,      16,        0,     -1,      6,         0,          5,        0,        128, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_BOOK2,     6,       6,        0,     -1,     -1,         0,          4,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_L2DOORS,   5,       8,        2,     -1,     -1,         0,          1,        0,         64, FALSE,      FALSE,     TRUE,            0,        3, TRUE      },
	{      1, OFILE_L2DOORS,   5,       8,        2,     -1,     -1,         0,          2,        0,         64, FALSE,      FALSE,     TRUE,            0,        3, FALSE     },
	{      1, OFILE_WTORCH4,   5,       8,        2,     -1,     -1,         1,          1,        9,         96, FALSE,      TRUE,      FALSE,           0,        0, FALSE     },
	{      1, OFILE_WTORCH3,   5,       8,        2,     -1,     -1,         1,          1,        9,         96, FALSE,      TRUE,      FALSE,           0,        0, FALSE     },
	{      1, OFILE_WTORCH1,   5,       8,        2,     -1,     -1,         1,          1,        9,         96, FALSE,      TRUE,      FALSE,           0,        0, FALSE     },
	{      1, OFILE_WTORCH2,   5,       8,        2,     -1,     -1,         1,          1,        9,         96, FALSE,      TRUE,      FALSE,           0,        0, FALSE     },
	{      1, OFILE_SARC,      1,       4,        1,     -1,     -1,         0,          1,        5,        128, TRUE,       TRUE,      TRUE,            0,        3, TRUE      },
	{      2, OFILE_FLAME1,    1,       4,        1,     -1,     -1,         0,          1,       20,         96, FALSE,      TRUE,      TRUE,            0,        0, FALSE     },
	{      2, OFILE_LEVER,     1,       4,        1,     -1,     -1,         0,          1,        2,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{      2, OFILE_MINIWATR,  1,       4,        1,     -1,     -1,         1,          1,       10,         64, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_BOOK1,     3,       4,        1,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_TRAPHOLE,  1,      16,        0,     -1,     -1,         0,          1,        0,         64, FALSE,      TRUE,      TRUE,            0,        0, FALSE     },
	{      1, OFILE_TRAPHOLE,  1,      16,        0,     -1,     -1,         0,          2,        0,         64, FALSE,      TRUE,      TRUE,            0,        0, FALSE     },
	{      2, OFILE_BCASE,     0,       0,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      2, OFILE_WEAPSTND,  0,       0,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      1, OFILE_BARREL,    1,      16,        0,     -1,     -1,         0,          1,        9,         96, TRUE,       TRUE,      TRUE,            1,        3, FALSE     },
	{      1, OFILE_BARRELEX,  1,      16,        0,     -1,     -1,         0,          1,       10,         96, TRUE,       TRUE,      TRUE,            1,        3, FALSE     },
	{      3, OFILE_LSHRINEG,  0,       0,        0,      1,     -1,         0,          1,       11,        128, FALSE,      FALSE,     TRUE,            0,        3, FALSE     },
	{      3, OFILE_RSHRINEG,  0,       0,        0,      1,     -1,         0,          1,       11,        128, FALSE,      FALSE,     TRUE,            0,        3, FALSE     },
	{      3, OFILE_BOOK2,     0,       0,        0,      3,     -1,         0,          4,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      3, OFILE_BCASE,     0,       0,        0,      5,     -1,         0,          3,        0,         96, FALSE,      FALSE,     TRUE,            0,        3, FALSE     },
	{      3, OFILE_BCASE,     0,       0,        0,      5,     -1,         0,          4,        0,         96, FALSE,      FALSE,     TRUE,            0,        3, FALSE     },
	{      3, OFILE_BOOK2,     0,       0,        0,      5,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      3, OFILE_CANDLE2,   0,       0,        0,      5,     -1,         1,          2,        4,         96, TRUE,       TRUE,      TRUE,            0,        0, FALSE     },
	{      3, OFILE_BLOODFNT,  0,       0,        0,      7,     -1,         1,          2,       10,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_DECAP,    13,      16,        0,      8,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, FALSE     },
	{      1, OFILE_CHEST1,    1,      16,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{      1, OFILE_CHEST2,    1,      16,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{      1, OFILE_CHEST3,    1,      16,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{      1, OFILE_BOOK1,     7,       7,        2,     -1,      8,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_BOOK1,     5,       5,        2,     -1,      9,         0,          4,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_PEDISTL,   5,       5,        2,     -1,      9,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_L3DOORS,   9,      12,        3,     -1,     -1,         0,          1,        0,         64, FALSE,      FALSE,     TRUE,            0,        3, TRUE      },
	{      1, OFILE_L3DOORS,   9,      12,        3,     -1,     -1,         0,          2,        0,         64, FALSE,      FALSE,     TRUE,            0,        3, TRUE      },
	{      3, OFILE_PFOUNTN,   0,       0,        0,      9,     -1,         1,          2,       10,        128, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      3, OFILE_ARMSTAND,  0,       0,        0,     10,     -1,         0,          1,        0,         96, TRUE,       FALSE,     TRUE,            0,        3, FALSE     },
	{      3, OFILE_ARMSTAND,  0,       0,        0,     10,     -1,         0,          2,        0,         96, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      3, OFILE_GOATSHRN,  0,       0,        0,     11,     -1,         1,          2,       10,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_CAULDREN, 13,      16,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       FALSE,     TRUE,            0,        3, FALSE     },
	{      3, OFILE_MFOUNTN,   0,       0,        0,     13,     -1,         1,          2,       10,        128, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      3, OFILE_TFOUNTN,   0,       0,        0,     14,     -1,         1,          2,        4,        128, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_ALTBOY,    0,       0,        1,     -1,     15,         0,          1,        0,        128, TRUE,       TRUE,      TRUE,            0,        0, FALSE     },
	{      1, OFILE_MCIRL,     0,       0,        1,     -1,     15,         0,          1,        0,         96, FALSE,      TRUE,      TRUE,            0,        0, FALSE     },
	{      1, OFILE_MCIRL,     0,       0,        1,     -1,     15,         0,          1,        0,         96, FALSE,      TRUE,      TRUE,            0,        0, FALSE     },
	{      1, OFILE_BKSLBRNT,  4,      12,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_CANDLE2,   2,      12,        0,     -1,     15,         1,          2,        4,         96, TRUE,       TRUE,      TRUE,            0,        0, FALSE     },
	{      1, OFILE_BOOK1,    13,      13,        4,     -1,     11,         0,          4,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, FALSE     },
	{      1, OFILE_ARMSTAND, 13,      13,        0,     -1,     11,         0,          1,        0,         96, TRUE,       FALSE,     TRUE,            0,        3, FALSE     },
	{      2, OFILE_WEAPSTND, 13,      13,        0,     -1,     11,         0,          1,        0,         96, TRUE,       FALSE,     TRUE,            0,        3, FALSE     },
	{      2, OFILE_BURNCROS,  0,       0,        0,     15,     -1,         1,          0,       10,        160, TRUE,       FALSE,     FALSE,           0,        0, FALSE     },
	{      2, OFILE_WEAPSTND,  0,       0,        0,     16,     -1,         0,          1,        0,         96, TRUE,       FALSE,     TRUE,            0,        3, FALSE     },
	{      2, OFILE_WEAPSTND,  0,       0,        0,     16,     -1,         0,          2,        0,         96, TRUE,       FALSE,     TRUE,            0,        0, FALSE     },
	{      2, OFILE_MUSHPTCH,  0,       0,        0,     -1,      1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        3, TRUE      },
	{      2, OFILE_LZSTAND,   0,       0,        0,     -1,     15,         0,          1,        0,        128, TRUE,       FALSE,     TRUE,            0,        3, FALSE     },
	{      1, OFILE_DECAP,     9,       9,        3,     -1,     -1,         0,          2,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, FALSE     },
	{      2, OFILE_CHEST3,    0,       0,        0,     -1,     -1,         0,          1,        0,         96, TRUE,       TRUE,      TRUE,            0,        1, TRUE      },
	{     -1, 0,               0,       0,       -1,     -1,     -1,         0,          0,        0,          0, FALSE,      FALSE,     FALSE,           0,        0, FALSE     }
	// clang-format on
};
char *ObjMasterLoadList[56] = {
	"L1Braz",
	"L1Doors",
	"Lever",
	"Chest1",
	"Chest2",
	"Banner",
	"SkulPile",
	"SkulFire",
	"SkulStik",
	"CruxSk1",
	"CruxSk2",
	"CruxSk3",
	"Book1",
	"Book2",
	"Rockstan",
	"Angel",
	"Chest3",
	"Burncros",
	"Candle2",
	"Nude2",
	"Switch4",
	"TNudeM",
	"TNudeW",
	"TSoul",
	"L2Doors",
	"WTorch4",
	"WTorch3",
	"Sarc",
	"Flame1",
	"Prsrplt1",
	"Traphole",
	"MiniWatr",
	"WTorch2",
	"WTorch1",
	"BCase",
	"BShelf",
	"WeapStnd",
	"Barrel",
	"Barrelex",
	"LShrineG",
	"RShrineG",
	"Bloodfnt",
	"Decap",
	"Pedistl",
	"L3Doors",
	"PFountn",
	"Armstand",
	"Goatshrn",
	"Cauldren",
	"MFountn",
	"TFountn",
	"Altboy",
	"Mcirl",
	"Bkslbrnt",
	"Mushptch",
	"LzStand"
};
int bxadd[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
int byadd[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
char *shrinestrs[NUM_SHRINETYPE] = {
	"Mysterious",
	"Hidden",
	"Gloomy",
	"Weird",
	"Magical",
	"Stone",
	"Religious",
	"Enchanted",
	"Thaumaturgic",
	"Fascinating",
	"Cryptic",
	"Magical",
	"Eldritch",
	"Eerie",
	"Divine",
	"Holy",
	"Sacred",
	"Spiritual",
	"Spooky",
	"Abandoned",
	"Creepy",
	"Quiet",
	"Secluded",
	"Ornate",
	"Glimmering",
	"Tainted"
};
unsigned char shrinemin[NUM_SHRINETYPE] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1
};
unsigned char shrinemax[NUM_SHRINETYPE] = {
	16, 16, 16, 16, 16, 16, 16, 8, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16
};
// 0 - sp+mp, 1 - sp only, 2 - mp only
unsigned char shrineavail[NUM_SHRINETYPE] = {
	0, 0, 1, 1, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 2, 0,
	0, 0, 0, 0, 0, 2
};
char *StoryBookName[9] = {
	"The Great Conflict",
	"The Wages of Sin are War",
	"The Tale of the Horadrim",
	"The Dark Exile",
	"The Sin War",
	"The Binding of the Three",
	"The Realms Beyond",
	"Tale of the Three",
	"The Black King"
};
int StoryText[3][3] = {
	{ QUEST_BOOK11, QUEST_BOOK12, QUEST_BOOK13 },
	{ QUEST_BOOK21, QUEST_BOOK22, QUEST_BOOK23 },
	{ QUEST_BOOK31, QUEST_BOOK32, QUEST_BOOK33 }
};

void __cdecl InitObjectGFX()
{
	ObjDataStruct *v0;          // eax
	char *v1;                   // esi
	unsigned char v2;           // cl
	int v3;                     // edx
	int i;                      // eax
	char v5;                    // al
	signed int v7;              // ebx
	char *v8;                   // ST08_4
	unsigned char *v9;          // eax
	int v10;                    // ecx
	unsigned char fileload[56]; // [esp+4h] [ebp-58h]
	char filestr[32];           // [esp+3Ch] [ebp-20h]

	memset(fileload, 0, sizeof(fileload));
	if (AllObjects[0].oload != -1) {
		v0 = AllObjects;
		v1 = &AllObjects[0].otheme;
		do {
			if (v0->oload == 1 && currlevel >= (signed int)(char)*(v1 - 3) && currlevel <= (signed int)(char)*(v1 - 2))
				fileload[(char)*(v1 - 4)] = 1;
			v2 = *v1;
			if (*v1 != -1) {
				v3 = numthemes;
				for (i = 0; i < v3; ++i) {
					if (themes[i].ttype == v2)
						fileload[(char)*(v1 - 4)] = 1;
				}
			}
			v5 = v1[1];
			if (v5 != -1) {
				if (QuestStatus(v5))
					fileload[(char)*(v1 - 4)] = 1;
			}
			v1 += 44;
			v0 = (ObjDataStruct *)(v1 - 5);
		} while (*(v1 - 5) != -1);
	}
	v7 = 0;
	do {
		if (fileload[v7]) {
			v8 = ObjMasterLoadList[v7];
			ObjFileList[numobjfiles] = v7;
			sprintf(filestr, "Objects\\%s.CEL", v8);
			v9 = LoadFileInMem(filestr, 0);
			v10 = numobjfiles++;
			pObjCels[v10] = v9;
		}
		++v7;
	} while (v7 < 56);
}
// 67D7C4: using guessed type int numobjfiles;
// 44121D: using guessed type char fileload[56];

void __cdecl FreeObjectGFX()
{
	int i;    // esi
	void *v1; // ecx

	for (i = 0; i < numobjfiles; ++i) {
		v1 = (void *)pObjCels[i];
		pObjCels[i] = 0;
		mem_free_dbg(v1);
	}
	numobjfiles = 0;
}
// 67D7C4: using guessed type int numobjfiles;

BOOLEAN __fastcall RndLocOk(int xp, int yp)
{
	int v2;         // ecx
	int v3;         // eax
	int v4;         // eax
	BOOLEAN result; // eax

	v2 = xp;
	v3 = v2 * 112 + yp;
	result = 0;
	if (!dMonster[0][v3] && !dPlayer[v2][yp] && !dObject[v2][yp] && !(dFlags[v2][yp] & DFLAG_POPULATED)) {
		v4 = dPiece[0][v3];
		if (!nSolidTable[v4] && (leveltype != 1 || v4 <= 126 || v4 >= 144))
			result = 1;
	}
	return result;
}

void __fastcall InitRndLocObj(int min, int max, int objtype)
{
	int numobjs; // ebx
	int xp;      // esi
	int yp;      // edi
	int i;       // [esp+8h] [ebp-4h]

	i = 0;
	numobjs = min + random(139, max - min);
	if (numobjs > 0) {
		while (1) {
			do {
				xp = random(139, 80) + 16;
				yp = random(139, 80) + 16;
			} while (!RndLocOk(xp - 1, yp - 1));
			if (RndLocOk(xp, yp - 1)) {
				if (RndLocOk(xp + 1, yp - 1)) /* check */
				{
					if (RndLocOk(xp - 1, yp)) {
						if (RndLocOk(xp, yp)) {
							if (RndLocOk(xp + 1, yp)) {
								if (RndLocOk(xp - 1, yp + 1)) {
									if (RndLocOk(xp, yp + 1)) {
										if (RndLocOk(xp + 1, yp + 1)) {
											AddObject(objtype, xp, yp);
											if (++i >= numobjs)
												break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void __fastcall InitRndLocBigObj(int min, int max, int objtype)
{
	int xp;      // edi
	int yp;      // esi
	int numobjs; // [esp+4h] [ebp-8h]
	int i;       // [esp+8h] [ebp-4h]

	i = 0;
	numobjs = min + random(140, max - min);
	if (numobjs > 0) {
		while (1) {
			do {
				xp = random(140, 80) + 16;
				yp = random(140, 80) + 16;
			} while (!RndLocOk(xp - 1, yp - 2));
			if (RndLocOk(xp, yp - 2)) {
				if (RndLocOk(xp + 1, yp - 2)) /* check */
				{
					if (RndLocOk(xp - 1, yp - 1)) {
						if (RndLocOk(xp, yp - 1)) {
							if (RndLocOk(xp + 1, yp - 1)) {
								if (RndLocOk(xp - 1, yp)) {
									if (RndLocOk(xp, yp)) {
										if (RndLocOk(xp + 1, yp)) {
											if (RndLocOk(xp - 1, yp + 1)) {
												if (RndLocOk(xp, yp + 1)) {
													if (RndLocOk(xp + 1, yp + 1)) {
														AddObject(objtype, xp, yp);
														if (++i >= numobjs)
															break;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void __fastcall InitRndLocObj5x5(int min, int max, int objtype)
{
	int v3;         // esi
	int v4;         // edx
	int v6;         // ebx
	int v7;         // eax
	int v9;         // edi
	int v10;        // esi
	int v11;        // edx
	signed int v12; // [esp+Ch] [ebp-14h]
	int v13;        // [esp+10h] [ebp-10h]
	int v14;        // [esp+14h] [ebp-Ch]
	signed int v15; // [esp+18h] [ebp-8h]
	signed int v16; // [esp+1Ch] [ebp-4h]

	v3 = min;
	v4 = max - min;
	v13 = 0;
	v6 = v3 + random(139, v4);
	if (v6 > 0) {
		do {
			v14 = 0;
			while (1) {
				v12 = 1;
				v7 = random(139, 80);
				v9 = v7 + 16;
				v15 = -2;
				v10 = random(139, 80) + 16;
				do {
					v16 = -2;
					v11 = v15 + v10;
					do {
						if (!RndLocOk(v16 + v9, v11))
							v12 = 0;
						++v16;
					} while (v16 <= 2);
					++v15;
				} while (v15 <= 2);
				if (v12)
					break;
				if (++v14 > 20000)
					return;
			}
			AddObject(objtype, v9, v10);
			++v13;
		} while (v13 < v6);
	}
}

void __cdecl ClrAllObjects()
{
	int *v0; // eax
	int v1;  // edx

	v0 = &object[0]._oy;
	do {
		*(v0 - 1) = 0;
		*v0 = 0;
		v0[3] = 0;
		v0[4] = 0;
		v0[5] = 0;
		v0[6] = 0;
		v0[7] = 0;
		v0[10] = 0;
		v0[20] = 0;
		v0[21] = 0;
		v0[22] = 0;
		v0[23] = 0;
		v0 += 30;
	} while ((signed int)v0 < (signed int)&object[MAXOBJECTS]._oy);
	v1 = 0;
	memset(objectactive, 0, sizeof(objectactive));
	nobjects = 0;
	do {
		objectavail[v1] = v1;
		++v1;
	} while (v1 < MAXOBJECTS);
	trapdir = 0;
	trapid = 1;
	leverid = 1;
}
// 679768: using guessed type int trapid;
// 67976C: using guessed type int trapdir;

void __cdecl AddTortures()
{
	int v0;     // esi
	int v1;     // edi
	_DWORD *v2; // [esp+Ch] [ebp-4h]

	v0 = 0;
	do {
		v1 = 2;
		v2 = (_DWORD *)((char *)dPiece + 4 * v0);
		do {
			if (*v2 == 367) {
				AddObject(OBJ_TORTURE1, v1 - 2, v0 + 1);
				AddObject(OBJ_TORTURE3, v1, v0 - 1);
				AddObject(OBJ_TORTURE2, v1 - 2, v0 + 3);
				AddObject(OBJ_TORTURE4, v1 + 2, v0 - 1);
				AddObject(OBJ_TORTURE5, v1 - 2, v0 + 5);
				AddObject(OBJ_TNUDEM1, v1 - 1, v0 + 3);
				AddObject(OBJ_TNUDEM2, v1 + 2, v0 + 5);
				AddObject(OBJ_TNUDEM3, v1, v0);
				AddObject(OBJ_TNUDEM4, v1 + 1, v0 + 2);
				AddObject(OBJ_TNUDEW1, v1, v0 + 4);
				AddObject(OBJ_TNUDEW2, v1, v0 + 1);
				AddObject(OBJ_TNUDEW3, v1 + 2, v0 + 2);
			}
			v2 += 112;
			++v1;
		} while (v1 - 2 < 112);
		++v0;
	} while (v0 < 112);
}

void __cdecl AddCandles()
{
	int v0; // esi
	int v1; // edi
	int v2; // ebx

	v0 = quests[QTYPE_PW]._qtx;
	v1 = quests[QTYPE_PW]._qty;
	v2 = quests[QTYPE_PW]._qty + 1;
	AddObject(OBJ_STORYCANDLE, quests[QTYPE_PW]._qtx - 2, quests[QTYPE_PW]._qty + 1);
	AddObject(OBJ_STORYCANDLE, v0 + 3, v2);
	v1 += 2;
	AddObject(OBJ_STORYCANDLE, v0 - 1, v1);
	AddObject(OBJ_STORYCANDLE, v0 + 2, v1);
}

void __fastcall AddBookLever(int lx1, int ly1, int lx2, int ly2, int x1, int y1, int x2, int y2, int msg)
{
	int v9;         // esi
	int v10;        // edi
	signed int v11; // ebx
	int v12;        // edx
	//int v13; // eax
	//int v14; // eax
	//int v15; // eax
	int v16;        // esi
	signed int v17; // [esp+Ch] [ebp-Ch]
	int v18;        // [esp+10h] [ebp-8h]
	signed int v19; // [esp+14h] [ebp-4h]

	v18 = 0;
	while (1) {
		v17 = 1;
		v9 = random(139, 80) + 16;
		v10 = random(139, 80) + 16;
		v11 = -2;
		do {
			v19 = -2;
			v12 = v11 + v10;
			do {
				if (!RndLocOk(v19 + v9, v12))
					v17 = 0;
				++v19;
			} while (v19 <= 2);
			++v11;
		} while (v11 <= 2);
		if (v17)
			break;
		if (++v18 > 20000)
			return;
	}
	//_LOBYTE(v13) = QuestStatus(QTYPE_BLIND);
	if (QuestStatus(QTYPE_BLIND))
		AddObject(OBJ_BLINDBOOK, v9, v10);
	//_LOBYTE(v14) = QuestStatus(QTYPE_WARLRD);
	if (QuestStatus(QTYPE_WARLRD))
		AddObject(OBJ_STEELTOME, v9, v10);
	//_LOBYTE(v15) = QuestStatus(QTYPE_BLOOD);
	if (QuestStatus(QTYPE_BLOOD)) {
		v9 = 2 * setpc_x + 25;
		v10 = 2 * setpc_y + 40;
		AddObject(OBJ_BLOODBOOK, v9, v10);
	}
	v16 = dObject[v9][v10] - 1;
	SetObjMapRange(v16, x1, y1, x2, y2, leverid);
	SetBookMsg(v16, msg);
	++leverid;
	object[v16]._oVar6 = object[v16]._oAnimFrame + 1;
}

void __cdecl InitRndBarrels()
{
	int v0;        // ebp
	int v1;        // esi
	int v2;        // edi
	int v3;        // eax
	BOOLEAN v4;    // ebx
	int v5;        // edx
	int v6;        // eax
	int v7;        // eax
	signed int v8; // [esp+4h] [ebp-Ch]
	signed int v9; // [esp+8h] [ebp-8h]
	int v10;       // [esp+Ch] [ebp-4h]

	v10 = 0;
	v0 = random(143, 5) + 3;
	if (v0 > 0) {
		do {
			do {
				v1 = random(143, 80) + 16;
				v2 = random(143, 80) + 16;
			} while (!RndLocOk(v1, v2));
			v3 = random(143, 4);
			AddObject(OBJ_BARRELEX - (v3 != 0), v1, v2);
			v4 = 1;
			v5 = 0;
			v9 = 1;
			while (!random(143, v5) && v4) {
				v8 = 0;
				v4 = 0;
				do {
					if (v8 >= 3)
						break;
					v6 = random(143, 8);
					v1 += bxadd[v6];
					v2 += byadd[v6];
					++v8;
					v4 = RndLocOk(v1, v2);
				} while (!v4);
				if (v4) {
					v7 = random(143, 5);
					AddObject(OBJ_BARRELEX - (v7 != 0), v1, v2);
					++v9;
				}
				v5 = v9 >> 1;
			}
			++v10;
		} while (v10 < v0);
	}
}

void __fastcall AddL1Objs(int x1, int y1, int x2, int y2)
{
	int v4;  // ebx
	int *v5; // edi
	int v6;  // esi
	int x;   // [esp+0h] [ebp-8h]
	int y;   // [esp+4h] [ebp-4h]

	x = x1;
	for (y = y1; y < y2; ++y) {
		v4 = x;
		if (x < x2) {
			v5 = (int *)((char *)dPiece + 4 * (y + 112 * x));
			do {
				v6 = *v5;
				if (*v5 == 270)
					AddObject(OBJ_L1LIGHT, v4, y);
				if (v6 == 44 || v6 == 51 || v6 == 214)
					AddObject(OBJ_L1LDOOR, v4, y);
				if (v6 == 46 || v6 == 56)
					AddObject(OBJ_L1RDOOR, v4, y);
				++v4;
				v5 += 112;
			} while (v4 < x2);
		}
	}
}

void __fastcall AddL2Objs(int x1, int y1, int x2, int y2)
{
	int v4;  // ebx
	int *v5; // esi
	int v6;  // edi
	int x;   // [esp+0h] [ebp-8h]
	int y;   // [esp+4h] [ebp-4h]

	x = x1;
	for (y = y1; y < y2; ++y) {
		v4 = x;
		if (x < x2) {
			v5 = (int *)((char *)dPiece + 4 * (y + 112 * x));
			do {
				v6 = *v5;
				if (*v5 == 13 || v6 == 541)
					AddObject(OBJ_L2LDOOR, v4, y);
				if (v6 == 17 || v6 == 542)
					AddObject(OBJ_L2RDOOR, v4, y);
				++v4;
				v5 += 112;
			} while (v4 < x2);
		}
	}
}

void __fastcall AddL3Objs(int x1, int y1, int x2, int y2)
{
	int v4;  // edi
	int *v5; // esi
	int v6;  // ebx
	int x;   // [esp+0h] [ebp-8h]
	int y;   // [esp+4h] [ebp-4h]

	x = x1;
	for (y = y1; y < y2; ++y) {
		v4 = x;
		if (x < x2) {
			v5 = (int *)((char *)dPiece + 4 * (y + 112 * x));
			do {
				v6 = *v5;
				if (*v5 == 531)
					AddObject(OBJ_L3LDOOR, v4, y);
				if (v6 == 534)
					AddObject(OBJ_L3RDOOR, v4, y);
				++v4;
				v5 += 112;
			} while (v4 < x2);
		}
	}
}

BOOLEAN __fastcall WallTrapLocOk(int xp, int yp)
{
	return (~dFlags[xp][yp] & DFLAG_POPULATED) >> 3;
}

void __cdecl AddL2Torches()
{
	int v0;   // esi
	int v1;   // edi
	char *v2; // ebx
	//int v3; // eax
	int(*v5)[112]; // [esp+Ch] [ebp-Ch]
	int v6;        // [esp+10h] [ebp-8h]
	int(*v7)[112]; // [esp+14h] [ebp-4h]

	v0 = 0;
	v7 = dPiece;
	do {
		v1 = 0;
		v2 = &dObject[0][v0 - 1]; /* &dungeon[39][v0 + 39]; */
		v5 = v7;
		do {
			//_LOBYTE(v3) = WallTrapLocOk(v1, v0);
			if (!WallTrapLocOk(v1, v0))
				goto LABEL_18;
			v6 = (*v5)[0];
			if ((*v5)[0] == 1) {
				if (random(145, 3))
					goto LABEL_18;
				AddObject(OBJ_TORCHL2, v1, v0);
			}
			if (v6 == 5) {
				if (random(145, 3))
					goto LABEL_18;
				AddObject(OBJ_TORCHR2, v1, v0);
			}
			if (v6 == 37) {
				if (random(145, 10) || *(v2 - 111))
					goto LABEL_18;
				AddObject(OBJ_TORCHL, v1 - 1, v0);
			}
			if (v6 == 41) {
				if (!random(145, 10) && !*v2)
					AddObject(OBJ_TORCHR, v1, v0 - 1);
			}
		LABEL_18:
			++v5;
			++v1;
			v2 += 112;
		} while (v1 < 112);
		v7 = (int(*)[112])((char *)v7 + 4);
		++v0;
	} while ((signed int)v7 < (signed int)dPiece[1]);
}

BOOL __fastcall TorchLocOK(int xp, int yp)
{
	if (dFlags[xp][yp] & DFLAG_POPULATED)
		return FALSE;

	return nTrapTable[dPiece[xp][yp]] != FALSE;
}

void __cdecl AddObjTraps()
{
	int v0;   // esi
	int *v1;  // eax
	char *v2; // edi
	int v3;   // ebx
	int v4;   // edi
	int *j;   // eax
	//int v6; // eax
	char v7; // al
	int v8;  // edi
	int *i;  // eax
	//int v10; // eax
	int v11;        // eax
	int *v12;       // [esp+0h] [ebp-18h]
	char *v13;      // [esp+4h] [ebp-14h]
	int *v14;       // [esp+8h] [ebp-10h]
	int v15;        // [esp+Ch] [ebp-Ch]
	signed int v16; // [esp+10h] [ebp-8h]
	int x;          // [esp+14h] [ebp-4h]

	if (currlevel == 1)
		v15 = 10;
	if (currlevel >= 2u)
		v15 = 15;
	if (currlevel >= 5u)
		v15 = 20;
	if (currlevel >= 7u)
		v15 = 25;
	v0 = 0;
	v1 = dPiece[-1];
	v12 = dPiece[-1];
	do {
		x = 0;
		v16 = 0;
		v2 = (char *)dObject + v0;
		v14 = v1;
		v13 = (char *)dObject + v0;
		do {
			if (*v2 > 0 && random(144, 100) < v15) {
				v3 = (char)(*v2 - 1);
				if (AllObjects[object[v3]._otype].oTrapFlag) {
					if (random(144, 2)) {
						v8 = v0 - 1;
						for (i = &dPiece[v16][v0 - 1]; !nSolidTable[*i]; i--) /* check dpiece */
							--v8;
						//_LOBYTE(v10) = TorchLocOK(x, v8);
						if (TorchLocOK(x, v8) && v0 - v8 > 1) {
							AddObject(OBJ_TRAPR, x, v8);
							v7 = dObject[v16][v8];
							goto LABEL_27;
						}
					} else {
						v4 = x - 1;
						for (j = v14; !nSolidTable[*j]; j -= 112)
							--v4;
						//_LOBYTE(v6) = TorchLocOK(v4, v0);
						if (TorchLocOK(v4, v0) && x - v4 > 1) {
							AddObject(OBJ_TRAPL, v4, v0);
							v7 = dObject[v4][v0];
						LABEL_27:
							v11 = (char)(v7 - 1);
							object[v11]._oVar2 = v0;
							object[v11]._oVar1 = x;
							object[v3]._oTrapFlag = 1;
							goto LABEL_28;
						}
					}
				}
			}
		LABEL_28:
			++v16;
			++x;
			v14 += 112;
			v2 = (char *)v13 + 112;
			v13 += 112;
		} while (v16 < 112);
		++v0;
		v1 = v12 + 1;
		++v12;
	} while ((signed int)v12 < (signed int)dPiece);
}

void __cdecl AddChestTraps()
{
	signed int v0; // ebp
	_BYTE *v1;     // ebx
	int v2;        // esi
	int v3;        // eax
	BOOLEAN v4;    // zf
	int v5;        // eax
	signed int v6; // [esp+10h] [ebp-4h]

	v0 = 0;
	do {
		v1 = (unsigned char *)dObject + v0;
		v6 = 112;
		do {
			if (*v1 > 0) {
				v2 = (char)(*v1 - 1);
				v3 = object[v2]._otype;
				if (v3 >= OBJ_CHEST1 && v3 <= OBJ_CHEST3 && !object[v2]._oTrapFlag && random(0, 100) < 10) {
					object[v2]._otype += OBJ_BOOKCASER;
					v4 = leveltype == DTYPE_CATACOMBS;
					object[v2]._oTrapFlag = 1;
					if (v4)
						v5 = random(0, 2);
					else
						v5 = random(0, 3);
					object[v2]._oVar4 = v5;
				}
			}
			v1 += 112;
			--v6;
		} while (v6);
		++v0;
	} while (v0 < 112);
}

void __fastcall LoadMapObjects(unsigned char *pMap, int startx, int starty, int x1, int y1, int w, int h, int leveridx)
{
	unsigned char *v8;  // ebx
	int v9;             // esi
	int v10;            // ecx
	int v11;            // eax
	int v12;            // ecx
	int v13;            // eax
	int v14;            // esi
	unsigned char *v15; // ebx
	int i;              // edi
	int v17;            // eax
	int v18;            // [esp+8h] [ebp-10h]
	int v19;            // [esp+Ch] [ebp-Ch]
	int v20;            // [esp+10h] [ebp-8h]
	int v21;            // [esp+14h] [ebp-4h]
	int y;              // [esp+20h] [ebp+8h]

	v8 = pMap + 2;
	InitObjFlag = TRUE;
	v9 = *pMap;
	v10 = pMap[2];
	v11 = v10;
	v12 = 2 * v10;
	v20 = startx;
	v13 = v9 * v11;
	v14 = 2 * v9;
	v19 = v14;
	v18 = v12;
	v15 = &v8[4 * v14 * v12 + 2 + 2 * v13];
	if (v12 > 0) {
		v21 = -16 - starty;
		y = starty + 16;
		do {
			for (i = 0; i < v14; ++i) {
				if (*v15) {
					AddObject(ObjTypeConv[*v15], i + v20 + 16, y);
					v17 = ObjIndex(i + v20 + 16, y);
					SetObjMapRange(v17, x1, y1, x1 + w, y1 + h, leveridx);
					v14 = v19;
					v12 = v18;
				}
				v15 += 2;
			}
			++y;
		} while (y + v21 < v12);
	}
	InitObjFlag = FALSE;
}

void __fastcall LoadMapObjs(unsigned char *pMap, int startx, int starty)
{
	unsigned char *v3;  // esi
	int v4;             // eax
	int v5;             // edi
	int v6;             // ecx
	int v7;             // eax
	int v8;             // ecx
	int v9;             // edi
	unsigned char *v10; // esi
	int i;              // ebx
	int v12;            // [esp+8h] [ebp-8h]
	int v13;            // [esp+Ch] [ebp-4h]
	int y;              // [esp+18h] [ebp+8h]

	v3 = pMap + 2;
	InitObjFlag = TRUE;
	v4 = pMap[2];
	v5 = *pMap;
	v6 = v4;
	v7 = 2 * v4;
	v12 = startx;
	v8 = v5 * v6;
	v9 = 2 * v5;
	v10 = &v3[4 * v9 * v7 + 2 + 2 * v8];
	if (v7 > 0) {
		v13 = v7;
		y = starty + 16;
		do {
			for (i = 0; i < v9; ++i) {
				if (*v10)
					AddObject(ObjTypeConv[*v10], i + v12 + 16, y);
				v10 += 2;
			}
			++y;
			--v13;
		} while (v13);
	}
	InitObjFlag = FALSE;
}

void __cdecl AddDiabObjs()
{
	unsigned char *v0; // esi
	unsigned char *v1; // esi
	unsigned char *v2; // esi

	v0 = LoadFileInMem("Levels\\L4Data\\diab1.DUN", 0);
	LoadMapObjects(v0, 2 * diabquad1x, 2 * diabquad1y, diabquad2x, diabquad2y, 11, 12, 1);
	mem_free_dbg(v0);
	v1 = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", 0);
	LoadMapObjects(v1, 2 * diabquad2x, 2 * diabquad2y, diabquad3x, diabquad3y, 11, 11, 2);
	mem_free_dbg(v1);
	v2 = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", 0);
	LoadMapObjects(v2, 2 * diabquad3x, 2 * diabquad3y, diabquad4x, diabquad4y, 9, 9, 3);
	mem_free_dbg(v2);
}
// 5289C4: using guessed type int diabquad1x;
// 5289C8: using guessed type int diabquad1y;

void __cdecl AddStoryBooks()
{
	int v0;        // esi
	int v1;        // edi
	signed int v2; // ebx
	int v3;        // edx
	int v4;        // esi
	int y;         // [esp+Ch] [ebp-Ch]
	int v6;        // [esp+10h] [ebp-8h]
	signed int v7; // [esp+14h] [ebp-4h]

	v6 = 0;
	while (1) {
		y = 1;
		v0 = random(139, 80) + 16;
		v1 = random(139, 80) + 16;
		v2 = -2;
		do {
			v7 = -3;
			v3 = v2 + v1;
			do {
				if (!RndLocOk(v7 + v0, v3))
					y = 0;
				++v7;
			} while (v7 <= 3);
			++v2;
		} while (v2 <= 2);
		if (y)
			break;
		if (++v6 > 20000)
			return;
	}
	AddObject(OBJ_STORYBOOK, v0, v1);
	AddObject(OBJ_STORYCANDLE, v0 - 2, v1 + 1);
	AddObject(OBJ_STORYCANDLE, v0 - 2, v1);
	AddObject(OBJ_STORYCANDLE, v0 - 1, v1 - 1);
	AddObject(OBJ_STORYCANDLE, v0 + 1, v1 - 1);
	v4 = v0 + 2;
	AddObject(OBJ_STORYCANDLE, v4, v1);
	AddObject(OBJ_STORYCANDLE, v4, v1 + 1);
}

void __fastcall AddHookedBodies(int freq)
{
	int v1;   // ebx
	char *v2; // esi
	int v3;   // edi
	//int v4; // eax
	int v5;  // eax
	int v6;  // eax
	int v7;  // eax
	int v8;  // [esp-8h] [ebp-20h]
	int v9;  // [esp-4h] [ebp-1Ch]
	int max; // [esp+Ch] [ebp-Ch]
	int x;   // [esp+10h] [ebp-8h]
	int y;   // [esp+14h] [ebp-4h]

	y = 0;
	max = freq;
	v1 = 16;
	do {
		x = 0;
		v2 = (char *)dungeon + y;
		v3 = 17;
		do {
			if (*v2 == 1 || *v2 == 2) {
				if (!random(0, max)) {
					//_LOBYTE(v4) = SkipThemeRoom(x, y);
					if (SkipThemeRoom(x, y)) {
						if (*v2 != 1 || v2[40] != 6) {
							if (*v2 == 2 && v2[1] == 6) {
								v7 = random(0, 2);
								if (v7) {
									if (v7 != 1)
										goto LABEL_22;
									v9 = v1;
									v8 = 39;
								} else {
									v9 = v1;
									v8 = 38;
								}
								AddObject(v8, v3 - 1, v9);
							}
						} else {
							v5 = random(0, 3);
							if (v5) {
								v6 = v5 - 1;
								if (v6) {
									if (v6 == 1)
										AddObject(OBJ_TORTURE5, v3, v1);
								} else {
									AddObject(OBJ_TORTURE2, v3, v1);
								}
							} else {
								AddObject(OBJ_TORTURE1, v3, v1);
							}
						}
					}
				}
			}
		LABEL_22:
			++x;
			v3 += 2;
			v2 += 40;
		} while (v3 < 97);
		++y;
		v1 += 2;
	} while (v1 < 96);
}

void __cdecl AddL4Goodies()
{
	AddHookedBodies(6);
	InitRndLocObj(2, 6, OBJ_TNUDEM1);
	InitRndLocObj(2, 6, OBJ_TNUDEM2);
	InitRndLocObj(2, 6, OBJ_TNUDEM3);
	InitRndLocObj(2, 6, OBJ_TNUDEM4);
	InitRndLocObj(2, 6, OBJ_TNUDEW1);
	InitRndLocObj(2, 6, OBJ_TNUDEW2);
	InitRndLocObj(2, 6, OBJ_TNUDEW3);
	InitRndLocObj(2, 6, OBJ_DECAP);
	InitRndLocObj(1, 3, OBJ_CAULDRON);
}

void __cdecl AddLazStand()
{
	int v0;        // edi
	int v1;        // esi
	signed int v2; // ebx
	int v3;        // edx
	int v4;        // edi
	signed int v5; // [esp+Ch] [ebp-Ch]
	int v6;        // [esp+10h] [ebp-8h]
	signed int v7; // [esp+14h] [ebp-4h]

	v6 = 0;
	while (1) {
		v5 = 1;
		v0 = random(139, 80) + 16;
		v1 = random(139, 80) + 16;
		v2 = -3;
		do {
			v7 = -2;
			v3 = v2 + v1;
			do {
				if (!RndLocOk(v7 + v0, v3))
					v5 = 0;
				++v7;
			} while (v7 <= 3);
			++v2;
		} while (v2 <= 3);
		if (v5)
			break;
		if (++v6 > 10000) {
			InitRndLocObj(1, 1, OBJ_LAZSTAND);
			return;
		}
	}
	AddObject(OBJ_LAZSTAND, v0, v1);
	AddObject(OBJ_TNUDEM2, v0, v1 + 2);
	AddObject(OBJ_STORYCANDLE, v0 + 1, v1 + 2);
	AddObject(OBJ_TNUDEM3, v0 + 2, v1 + 2);
	AddObject(OBJ_TNUDEW1, v0, v1 - 2);
	AddObject(OBJ_STORYCANDLE, v0 + 1, v1 - 2);
	AddObject(OBJ_TNUDEW2, v0 + 2, v1 - 2);
	v4 = v0 - 1;
	AddObject(OBJ_STORYCANDLE, v4, v1 - 1);
	AddObject(OBJ_TNUDEW3, v4, v1);
	AddObject(OBJ_STORYCANDLE, v4, v1 + 1);
}

void __cdecl InitObjects()
{
	//int v1; // eax
	//int v2; // eax
	//int v3; // eax
	//int v4; // eax
	//int v5; // eax
	//int v6; // eax
	signed int v8;     // ebx
	unsigned char *v9; // esi
	//int v10; // eax
	//int v12; // eax
	unsigned char *v14; // esi
	//int v15; // eax
	//int v16; // [esp+0h] [ebp-4h]

	ClrAllObjects();
	if (currlevel == 16) {
		AddDiabObjs();
	} else {
		InitObjFlag = TRUE;
		GetRndSeed();
		if (currlevel == 9 && gbMaxPlayers == 1)
			AddSlainHero();
		if (currlevel == quests[QTYPE_BLKM]._qlevel && quests[QTYPE_BLKM]._qactive == 1)
			AddMushPatch();
		if (currlevel == 4)
			AddStoryBooks();
		if (currlevel == 8)
			AddStoryBooks();
		if (currlevel == 12)
			AddStoryBooks();
		if (leveltype == DTYPE_CATHEDRAL) {
			//_LOBYTE(v1) = QuestStatus(QTYPE_BUTCH);
			if (QuestStatus(QTYPE_BUTCH))
				AddTortures();
			//_LOBYTE(v2) = QuestStatus(QTYPE_PW);
			if (QuestStatus(QTYPE_PW))
				AddCandles();
			//_LOBYTE(v3) = QuestStatus(QTYPE_BOL);
			if (QuestStatus(QTYPE_BOL))
				AddObject(OBJ_SIGNCHEST, 2 * setpc_x + 26, 2 * setpc_y + 19);
			InitRndLocBigObj(10, 15, OBJ_SARC);
			AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
			InitRndBarrels();
		}
		if (leveltype == DTYPE_CATACOMBS) {
			//_LOBYTE(v4) = QuestStatus(QTYPE_INFRA);
			if (QuestStatus(QTYPE_INFRA))
				InitRndLocObj5x5(1, 1, OBJ_STAND);
			//_LOBYTE(v5) = QuestStatus(QTYPE_BONE);
			if (QuestStatus(QTYPE_BONE))
				InitRndLocObj5x5(1, 1, OBJ_BOOK2R);
			AddL2Objs(0, 0, MAXDUNX, MAXDUNY);
			AddL2Torches();
			//_LOBYTE(v6) = QuestStatus(QTYPE_BLIND);
			if (QuestStatus(QTYPE_BLIND)) {
				if (plr[myplr]._pClass == PC_WARRIOR) {
					v8 = QUEST_BLINDING;
				} else if (plr[myplr]._pClass == PC_ROGUE) {
					v8 = QUEST_RBLINDING;
				} else if (plr[myplr]._pClass == PC_SORCERER) {
					v8 = QUEST_MBLINDING;
				}
				quests[QTYPE_BLIND]._qmsg = v8;
				AddBookLever(0, 0, MAXDUNX, MAXDUNY, setpc_x, setpc_y, setpc_w + setpc_x + 1, setpc_h + setpc_y + 1, v8);
				v9 = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", 0);
				LoadMapObjs(v9, 2 * setpc_x, 2 * setpc_y);
				mem_free_dbg(v9);
			}
			//_LOBYTE(v10) = QuestStatus(QTYPE_BLOOD);
			if (QuestStatus(QTYPE_BLOOD)) {
				if (plr[myplr]._pClass == PC_WARRIOR) {
					v8 = QUEST_BLOODY;
				} else if (plr[myplr]._pClass == PC_ROGUE) {
					v8 = QUEST_RBLOODY;
				} else if (plr[myplr]._pClass == PC_SORCERER) {
					v8 = QUEST_MBLOODY;
				}
				quests[QTYPE_BLOOD]._qmsg = v8;
				AddBookLever(0, 0, MAXDUNX, MAXDUNY, setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7, v8);
				AddObject(OBJ_PEDISTAL, 2 * setpc_x + 25, 2 * setpc_y + 32);
			}
			InitRndBarrels();
		}
		if (leveltype == DTYPE_CAVES) {
			AddL3Objs(0, 0, MAXDUNX, MAXDUNY);
			InitRndBarrels();
		}
		if (leveltype == DTYPE_HELL) {
			//_LOBYTE(v12) = QuestStatus(QTYPE_WARLRD);
			if (QuestStatus(QTYPE_WARLRD)) {
				if (plr[myplr]._pClass == PC_WARRIOR) {
					v8 = QUEST_BLOODWAR;
				} else if (plr[myplr]._pClass == PC_ROGUE) {
					v8 = QUEST_RBLOODWAR;
				} else if (plr[myplr]._pClass == PC_SORCERER) {
					v8 = QUEST_MBLOODWAR;
				}
				quests[QTYPE_WARLRD]._qmsg = v8;
				AddBookLever(0, 0, MAXDUNX, MAXDUNY, setpc_x, setpc_y, setpc_x + setpc_w, setpc_y + setpc_h, v8);
				v14 = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", 0);
				LoadMapObjs(v14, 2 * setpc_x, 2 * setpc_y);
				mem_free_dbg(v14);
			}
			//_LOBYTE(v15) = QuestStatus(QTYPE_VB);
			if (QuestStatus(QTYPE_VB) && gbMaxPlayers == 1)
				AddLazStand();
			InitRndBarrels();
			AddL4Goodies();
		}
		InitRndLocObj(5, 10, 5);
		InitRndLocObj(3, 6, 6);
		InitRndLocObj(1, 5, 7);
		if (leveltype != 4)
			AddObjTraps();
		if (leveltype > 1u)
			AddChestTraps();
		InitObjFlag = FALSE;
	}
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall SetMapObjects(unsigned char *pMap, int startx, int starty)
{
	unsigned char *v3;  // esi
	int v6;             // edi
	int v7;             // eax
	int v8;             // esi
	int v9;             // ecx
	int v10;            // esi
	int v11;            // ecx
	int v12;            // edi
	_BYTE *v13;         // eax
	int v14;            // ebx
	signed int v15;     // ebx
	char *v16;          // ST08_4
	unsigned char *v17; // eax
	int v18;            // ecx
	int i;              // ebx
	int fileload[56];   // [esp+Ch] [ebp-10Ch]
	char filestr[32];   // [esp+ECh] [ebp-2Ch]
	_BYTE *v22;         // [esp+10Ch] [ebp-Ch]
	int v23;            // [esp+110h] [ebp-8h]
	_BYTE *v24;         // [esp+114h] [ebp-4h]
	int y;              // [esp+120h] [ebp+8h]

	v23 = startx;
	v3 = pMap;
	ClrAllObjects();
	memset(fileload, 0, sizeof(fileload));
	InitObjFlag = TRUE;
	if (AllObjects[0].oload != -1) {
		i = 0;
		do {
			if (AllObjects[i].oload == 1 && leveltype == AllObjects[i].olvltype)
				fileload[AllObjects[i].ofindex] = 1;
			i++;
		} while (AllObjects[i].oload != -1);
	}
	v6 = (unsigned char)*v3;
	v7 = (int)(v3 + 2);
	v8 = (unsigned char)v3[2];
	v9 = v8;
	v10 = 2 * v8;
	v11 = v6 * v9;
	v12 = 2 * v6;
	v13 = (_BYTE *)(2 * v11 + 2 + 4 * v12 * v10 + v7);
	v22 = v13;
	if (v10 > 0) {
		v24 = (_BYTE *)v10;
		do {
			if (v12 > 0) {
				v14 = v12;
				do {
					if (*v13)
						fileload[(char)AllObjects[ObjTypeConv[(unsigned char)*v13]].ofindex] = 1;
					v13 += 2;
					--v14;
				} while (v14);
			}
			--v24;
		} while (v24);
	}
	v15 = 0;
	do {
		if (fileload[v15]) {
			v16 = ObjMasterLoadList[v15];
			ObjFileList[numobjfiles] = v15;
			sprintf(filestr, "Objects\\%s.CEL", v16);
			v17 = LoadFileInMem(filestr, 0);
			v18 = numobjfiles++;
			pObjCels[v18] = v17;
		}
		++v15;
	} while (v15 < 56);
	v24 = v22;
	if (v10 > 0) {
		y = starty + 16;
		do {
			for (i = 0; i < v12; ++i) {
				if (*v24)
					AddObject(ObjTypeConv[(unsigned char)*v24], i + v23 + 16, y);
				v24 += 2;
			}
			++y;
			--v10;
		} while (v10);
	}
	InitObjFlag = FALSE;
}
// 67D7C4: using guessed type int numobjfiles;
// 4427C5: using guessed type int var_10C[56];

void __fastcall DeleteObject_(int oi, int i)
{
	int v2;     // eax
	BOOLEAN v3; // zf
	BOOLEAN v4; // sf

	dObject[object[oi]._ox][object[oi]._oy] = 0;
	v2 = nobjects - 1;
	v3 = nobjects == 1;
	v4 = nobjects - 1 < 0;
	objectavail[-nobjects + MAXOBJECTS] = oi; /* *(&object[0]._otype - nobjects) = oi; */
	nobjects = v2;
	if (!v4 && !v3 && i != v2)
		objectactive[i] = objectactive[v2];
}

void __fastcall SetupObject(int i, int x, int y, int ot)
{
	int v4;  // esi
	int v5;  // edi
	int v6;  // ecx
	int v7;  // edx
	int v8;  // eax
	int v9;  // eax
	int v10; // edx
	int v11; // eax
	int v13; // eax
	int v14; // eax

	v4 = i;
	object[v4]._otype = ot;
	v5 = ot;
	v6 = AllObjects[ot].ofindex;
	object[v4]._ox = x;
	object[v4]._oy = y;
	v7 = ObjFileList[0];
	v8 = 0;
	while (v7 != v6)
		v7 = ObjFileList[v8++ + 1];
	object[v4]._oAnimData = pObjCels[v8];
	v9 = AllObjects[v5].oAnimFlag;
	object[v4]._oAnimFlag = v9;
	if (v9) {
		v10 = AllObjects[v5].oAnimDelay;
		object[v4]._oAnimDelay = v10;
		object[v4]._oAnimCnt = random(146, v10);
		v11 = AllObjects[v5].oAnimLen;
		object[v4]._oAnimLen = v11;
		v13 = random(146, v11 - 1) + 1;
	} else {
		v14 = AllObjects[v5].oAnimLen;
		object[v4]._oAnimDelay = 1000;
		object[v4]._oAnimLen = v14;
		v13 = AllObjects[v5].oAnimDelay;
		object[v4]._oAnimCnt = 0;
	}
	object[v4]._oAnimFrame = v13;
	object[v4]._oAnimWidth = AllObjects[v5].oAnimWidth;
	object[v4]._oSolidFlag = AllObjects[v5].oSolidFlag;
	object[v4]._oMissFlag = AllObjects[v5].oMissFlag;
	object[v4]._oLight = AllObjects[v5].oLightFlag;
	object[v4]._oBreak = AllObjects[v5].oBreak;
	object[v4]._oDelFlag = 0;
	object[v4]._oSelFlag = AllObjects[v5].oSelFlag;
	object[v4]._oPreFlag = 0;
	object[v4]._oTrapFlag = 0;
	object[v4]._oDoorFlag = 0;
}

void __fastcall SetObjMapRange(int i, int x1, int y1, int x2, int y2, int v)
{
	object[i]._oVar1 = x1;
	object[i]._oVar2 = y1;
	object[i]._oVar3 = x2;
	object[i]._oVar4 = y2;
	object[i]._oVar8 = v;
}

void __fastcall SetBookMsg(int i, int msg)
{
	object[i]._oVar7 = msg;
}

void __fastcall AddL1Door(int i, int x, int y, int ot)
{
	int v4;  // ecx
	int v5;  // edx
	int *v6; // eax
	int v7;  // edx
	int v8;  // eax
	int v9;  // eax

	v4 = i;
	v5 = 112 * x;
	object[v4]._oDoorFlag = 1;
	if (ot == 1) {
		v6 = (int *)((char *)dPiece + 4 * (y + v5));
		v7 = *v6;
		v8 = *(v6 - 1);
	} else {
		v9 = v5 + y;
		v7 = dPiece[0][v5 + y];
		v8 = dPiece[-1][v5 + y]; // *(_DWORD *)&dflags[28][4 * v9 + 32]; /* check */
	}
	object[v4]._oVar4 = 0;
	object[v4]._oVar1 = v7;
	object[v4]._oVar2 = v8;
}

void __fastcall AddSCambBook(int i)
{
	object[i]._oVar1 = setpc_x;
	object[i]._oVar2 = setpc_y;
	object[i]._oVar3 = setpc_w + setpc_x + 1;
	object[i]._oVar4 = setpc_h + setpc_y + 1;
	object[i]._oVar6 = object[i]._oAnimFrame + 1;
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void __fastcall AddChest(int i, int t)
{
	if (!random(147, 2))
		object[i]._oAnimFrame += 3;
	object[i]._oRndSeed = GetRndSeed();
	switch (t) {
	case OBJ_CHEST1:
	case OBJ_TCHEST1:
		if (setlevel) {
			object[i]._oVar1 = 1;
			break;
		}
		object[i]._oVar1 = random(147, 2);
		break;
	case OBJ_TCHEST2:
	case OBJ_CHEST2:
		if (setlevel) {
			object[i]._oVar1 = 2;
			break;
		}
		object[i]._oVar1 = random(147, 3);
		break;
	case OBJ_TCHEST3:
	case OBJ_CHEST3:
		if (setlevel) {
			object[i]._oVar1 = 3;
			break;
		}
		object[i]._oVar1 = random(147, 4);
		break;
	}
	object[i]._oVar2 = random(147, 8);
}
// 5CF31D: using guessed type char setlevel;

void __fastcall AddL2Door(int i, int x, int y, int ot)
{
	int v4; // esi

	v4 = i;
	object[i]._oDoorFlag = 1;
	if (ot == OBJ_L2LDOOR)
		ObjSetMicro(x, y, 538);
	else
		ObjSetMicro(x, y, 540);
	object[v4]._oVar4 = 0;
}

void __fastcall AddL3Door(int i, int x, int y, int ot)
{
	int v4; // esi

	v4 = i;
	object[i]._oDoorFlag = 1;
	if (ot == OBJ_L3LDOOR)
		ObjSetMicro(x, y, 531);
	else
		ObjSetMicro(x, y, 534);
	object[v4]._oVar4 = 0;
}

void __fastcall AddSarc(int i)
{
	dObject[object[i]._ox][object[i]._oy - 1] = -(i + 1);
	object[i]._oVar1 = random(153, 10);
	object[i]._oRndSeed = GetRndSeed();
	if (object[i]._oVar1 >= 8)
		object[i]._oVar2 = PreSpawnSkeleton();
}

void __fastcall AddFlameTrap(int i)
{
	object[i]._oVar1 = trapid;
	object[i]._oVar2 = 0;
	object[i]._oVar3 = trapdir;
	object[i]._oVar4 = 0;
}
// 679768: using guessed type int trapid;
// 67976C: using guessed type int trapdir;

void __fastcall AddFlameLvr(int i)
{
	object[i]._oVar1 = trapid;
	object[i]._oVar2 = 49;
}
// 679768: using guessed type int trapid;

void __fastcall AddTrap(int i)
{
	int mt; // eax

	mt = random(148, currlevel / 3 + 1);
	if (!mt)
		object[i]._oVar3 = 0;
	if (mt == 1)
		object[i]._oVar3 = 1;
	if (mt == 2)
		object[i]._oVar3 = 7;
	object[i]._oVar4 = 0;
}

void __fastcall AddObjLight(int i, int r)
{
	if (InitObjFlag) {
		DoLighting(object[i]._ox, object[i]._oy, r, -1);
		object[i]._oVar1 = -1;
	} else {
		object[i]._oVar1 = 0;
	}
}

void __fastcall AddBarrel(int i)
{
	object[i]._oVar1 = 0;
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oVar2 = random(149, 10);
	object[i]._oVar3 = random(149, 3);

	if (object[i]._oVar2 >= 8)
		object[i]._oVar4 = PreSpawnSkeleton();
}

void __fastcall AddShrine(int i)
{
	int v1;        // esi
	signed int v2; // edi
	signed int v3; // eax
	int *v4;       // ecx
	BOOLEAN v5;    // zf
	int v6;        // eax
	int slist[26]; // [esp+8h] [ebp-68h]

	v1 = i;
	v2 = currlevel;
	v3 = 0;
	object[i]._oPreFlag = 1;
	do {
		if (v2 < (char)shrinemin[v3] || v2 > (char)shrinemax[v3]) {
			v4 = &slist[v3];
			*v4 = 0;
		} else {
			v4 = &slist[v3];
			*v4 = 1;
		}
		if (gbMaxPlayers == 1)
			v5 = shrineavail[v3] == 2;
		else
			v5 = shrineavail[v3] == 1;
		if (v5)
			*v4 = 0;
		++v3;
	} while (v3 < 26);
	do {
		v6 = random(150, 26);
	} while (!slist[v6]);
	object[v1]._oVar1 = v6;
	if (random(150, 2)) {
		object[v1]._oAnimFrame = 12;
		object[v1]._oAnimLen = 22;
	}
}
// 679660: using guessed type char gbMaxPlayers;
// 442E0F: using guessed type int var_68[26];

void __fastcall AddBookcase(int i)
{
	int v1; // esi

	v1 = i;
	object[v1]._oRndSeed = GetRndSeed();
	object[v1]._oPreFlag = 1;
}

void __fastcall AddPurifyingFountain(int i)
{
	char *v1; // eax

	v1 = &dObject[object[i]._ox][object[i]._oy];
	*(v1 - 1) = -1 - i;
	*(v1 - 112) = -1 - i;
	*(v1 - 113) = -1 - i;
	object[i]._oRndSeed = GetRndSeed();
}

void __fastcall AddArmorStand(int i)
{
	if (!armorFlag) {
		object[i]._oSelFlag = 0;
		object[i]._oAnimFlag = 2;
	}

	object[i]._oRndSeed = GetRndSeed();
}

void __fastcall AddDecap(int i)
{
	int v1; // esi
	int v2; // eax
	int v4; // eax

	v1 = i;
	v2 = GetRndSeed();
	object[v1]._oRndSeed = v2;
	v4 = random(151, 8);
	object[v1]._oPreFlag = 1;
	object[v1]._oAnimFrame = v4 + 1;
}

void __fastcall AddVilebook(int i)
{
	if (setlevel) {
		if (setlvlnum == SL_VILEBETRAYER)
			object[i]._oAnimFrame = 4;
	}
}
// 5CF31D: using guessed type char setlevel;

void __fastcall AddMagicCircle(int i)
{
	int v1; // esi
	int v2; // eax

	v1 = i;
	v2 = GetRndSeed();
	object[v1]._oVar6 = 0;
	object[v1]._oRndSeed = v2;
	object[v1]._oPreFlag = 1;
	object[v1]._oVar5 = 1;
}

void __fastcall AddBookstand(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void __fastcall AddPedistal(int i)
{
	int v1; // ecx
	int v2; // eax
	int v3; // edx
	int v4; // esi
	int v5; // esi
	int v6; // eax

	v1 = i;
	v2 = setpc_x;
	v3 = setpc_y;
	v4 = setpc_w;
	object[v1]._oVar1 = setpc_x;
	v5 = v2 + v4;
	v6 = setpc_h;
	object[v1]._oVar3 = v5;
	object[v1]._oVar2 = v3;
	object[v1]._oVar4 = v3 + v6;
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void __fastcall AddStoryBook(int i)
{
	int bookframe; // eax
	int v7;        // eax

	SetRndSeed(glSeedTbl[16]);
	bookframe = random(0, 3);

	object[i]._oVar1 = bookframe;
	if (currlevel == 4)
		object[i]._oVar2 = StoryText[bookframe][0];
	if (currlevel == 8)
		object[i]._oVar2 = StoryText[bookframe][1];
	if (currlevel == 12)
		object[i]._oVar2 = StoryText[bookframe][2];
	object[i]._oVar3 = ((unsigned int)currlevel >> 2) + 3 * bookframe - 1;
	v7 = 5 - 2 * bookframe;
	object[i]._oAnimFrame = v7;
	object[i]._oVar4 = v7 + 1;
}

void __fastcall AddWeaponRack(int i)
{
	if (!weaponFlag) {
		object[i]._oSelFlag = 0;
		object[i]._oAnimFlag = 2;
	}
	object[i]._oRndSeed = GetRndSeed();
}

void __fastcall AddTorturedBody(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oPreFlag = 1;
	object[i]._oAnimFrame = random(0, 4) + 1;
}

void __fastcall GetRndObjLoc(int randarea, int *xx, int *yy)
{
	int *v3;    // ebx
	int v4;     // eax
	int v6;     // eax
	int v7;     // esi
	BOOLEAN v8; // eax
	int v9;     // edi
	int v10;    // [esp+Ch] [ebp-Ch]
	int v11;    // [esp+10h] [ebp-8h]
	int v12;    // [esp+14h] [ebp-4h]

	v3 = xx;
	v12 = randarea;
	if (randarea) {
		v10 = 0;
		while (1) {
		LABEL_3:
			if (++v10 > 1000 && v12 > 1)
				--v12;
			v4 = random(0, MAXDUNX);
			*v3 = v4;
			v6 = random(0, MAXDUNY);
			v7 = v6;
			*yy = v6;
			v8 = 0;
			v11 = 0;
			if (v12 <= 0)
				break;
			while (!v8) {
				v9 = 0;
				do {
					if (v8)
						break;
					v8 = RndLocOk(v11 + *v3, v7 + v9++) == 0;
				} while (v9 < v12);
				randarea = ++v11;
				if (v11 >= v12) {
					if (v8)
						goto LABEL_3;
					return;
				}
			}
		}
	}
}

void __cdecl AddMushPatch()
{
	int i; // bl
	int y; // [esp+0h] [ebp-8h]
	int x; // [esp+4h] [ebp-4h]

	if (nobjects < MAXOBJECTS) {
		i = objectavail[0];
		GetRndObjLoc(5, &x, &y);
		dObject[x + 1][y + 1] = -1 - i;
		dObject[x + 2][y + 1] = -1 - i;
		dObject[x + 1][y + 2] = -1 - i;
		AddObject(OBJ_MUSHPATCH, x + 2, y + 2);
	}
}

void __cdecl AddSlainHero()
{
	int x; // [esp+0h] [ebp-8h]
	int y; // [esp+4h] [ebp-4h]

	GetRndObjLoc(5, &x, &y);
	AddObject(OBJ_SLAINHERO, x + 2, y + 2);
}

void __fastcall AddObject(int ot, int ox, int oy)
{
	int v3; // ebp
	int v4; // esi
	//unsigned int v5; // eax
	int v6; // ebx
	int v7; // ebx
	int v8; // eax

	v3 = ox;
	v4 = ot;
	if (nobjects < MAXOBJECTS) {
		//v5 = 4 * nobjects;
		v6 = objectavail[0];
		objectactive[nobjects] = objectavail[0];
		objectavail[0] = objectavail[-nobjects + 126]; /* double check, MAXOBJECTS */
		dObject[ox][oy] = v6 + 1;
		SetupObject(v6, ox, oy, ot);
		switch (v4) {
		case OBJ_L1LIGHT:
		case OBJ_SKFIRE:
		case OBJ_CANDLE1:
		case OBJ_CANDLE2:
		case OBJ_BOOKCANDLE:
			goto LABEL_31;
		case OBJ_L1LDOOR:
		case OBJ_L1RDOOR:
			AddL1Door(v6, v3, oy, v4);
			break;
		case OBJ_CHEST1:
		case OBJ_CHEST2:
		case OBJ_CHEST3:
		case OBJ_TCHEST1:
		case OBJ_TCHEST2:
		case OBJ_TCHEST3:
			AddChest(v6, v4);
			break;
		case OBJ_BOOK2L:
			AddVilebook(v6);
			break;
		case OBJ_BCROSS:
		case OBJ_TBCROSS:
			AddBookstand(v6);
		LABEL_31:
			AddObjLight(v6, 5);
			break;
		case OBJ_TNUDEM2:
			AddTorturedBody(v6);
			break;
		case OBJ_BOOK2R:
			AddSCambBook(v6);
			break;
		case OBJ_L2LDOOR:
		case OBJ_L2RDOOR:
			AddL2Door(v6, v3, oy, v4);
			break;
		case OBJ_TORCHL:
		case OBJ_TORCHR:
		case OBJ_TORCHL2:
		case OBJ_TORCHR2:
			AddObjLight(v6, 8);
			break;
		case OBJ_SARC:
			AddSarc(v6);
			break;
		case OBJ_FLAMEHOLE:
			AddFlameTrap(v6);
			break;
		case OBJ_FLAMELVR:
			AddFlameLvr(v6);
			break;
		case OBJ_WATER:
			object[v6]._oAnimFrame = 1;
			break;
		case OBJ_TRAPL:
		case OBJ_TRAPR:
			AddTrap(v6);
			break;
		case OBJ_BARREL:
		case OBJ_BARRELEX:
			AddBarrel(v6);
			break;
		case OBJ_SHRINEL:
		case OBJ_SHRINER:
			AddShrine(v6);
			break;
		case OBJ_SKELBOOK:
		case OBJ_BOOKSTAND:
			AddBookstand(v6);
			break;
		case OBJ_BOOKCASEL:
		case OBJ_BOOKCASER:
			AddBookcase(v6);
			break;
		case OBJ_BLOODFTN:
			AddBookstand(v6);
			break;
		case OBJ_DECAP:
			AddDecap(v6);
			break;
		case OBJ_PEDISTAL:
			AddPedistal(v6);
			break;
		case OBJ_L3LDOOR:
		case OBJ_L3RDOOR:
			AddL3Door(v6, v3, oy, v4);
			break;
		case OBJ_PURIFYINGFTN:
			AddPurifyingFountain(v6);
			break;
		case OBJ_ARMORSTAND:
		case OBJ_WARARMOR:
			AddArmorStand(v6);
			break;
		case OBJ_GOATSHRINE:
			AddBookstand(v6);
			break;
		case OBJ_CAULDRON:
			AddBookstand(v6);
			break;
		case OBJ_MURKYFTN:
			AddPurifyingFountain(v6);
			break;
		case OBJ_TEARFTN:
			AddBookstand(v6);
			break;
		case OBJ_MCIRCLE1:
		case OBJ_MCIRCLE2:
			AddMagicCircle(v6);
			break;
		case OBJ_STORYBOOK:
			AddStoryBook(v6);
			break;
		case OBJ_STORYCANDLE:
			AddObjLight(v6, 3);
			break;
		case OBJ_WARWEAP:
		case OBJ_WEAPONRACK:
			AddWeaponRack(v6);
			break;
		default:
			break;
		}
		v7 = v6;
		v8 = object[v7]._oAnimWidth - 64;
		++nobjects;
		object[v7]._oAnimWidth2 = v8 >> 1;
	}
}

void __fastcall Obj_Light(int i, int lr)
{
	int v2;         // esi
	int v3;         // ebx
	int *v4;        // edi
	int v5;         // ST18_4
	int v6;         // eax
	int r;          // [esp+Ch] [ebp-14h]
	int x;          // [esp+14h] [ebp-Ch]
	int y;          // [esp+18h] [ebp-8h]
	signed int v10; // [esp+1Ch] [ebp-4h]

	v2 = i;
	r = lr;
	if (object[i]._oVar1 != -1) {
		v10 = 0;
		x = object[v2]._ox;
		v3 = lr + 10;
		y = object[v2]._oy;
		if (lightflag) {
		LABEL_15:
			if (object[v2]._oVar1 == 1)
				AddUnLight(object[v2]._olid);
			object[v2]._oVar1 = 0;
		} else {
			v4 = &plr[0].plrlevel;
			while (!v10) {
				if (*((_BYTE *)v4 - 23)) {
					if (currlevel == *v4) {
						v5 = abs(v4[1] - x);
						v6 = abs(v4[2] - y);
						if (v5 < v3 && v6 < v3)
							v10 = 1;
					}
				}
				v4 += 5430;
				if ((signed int)v4 >= (signed int)&plr[4].plrlevel) {
					if (!v10)
						goto LABEL_15;
					break;
				}
			}
			if (!object[v2]._oVar1)
				object[v2]._olid = AddLight(x, y, r);
			object[v2]._oVar1 = 1;
		}
	}
}
// 646A28: using guessed type int lightflag;

void __fastcall Obj_Circle(int i)
{
	int v1; // ecx
	int v2; // edx
	int v3; // esi
	int v4; // eax
	int v5; // ST1C_4
	int v6; // edx
	int v7; // eax

	v1 = i;
	v2 = object[v1]._ox;
	v3 = object[v1]._oy;
	if (plr[myplr].WorldX != v2 || plr[myplr].WorldY != v3) {
		v7 = object[v1]._otype;
		if (v7 == OBJ_MCIRCLE1)
			object[v1]._oAnimFrame = 1;
		if (v7 == OBJ_MCIRCLE2)
			object[v1]._oAnimFrame = 3;
		object[v1]._oVar6 = 0;
	} else {
		v4 = object[v1]._otype;
		if (v4 == OBJ_MCIRCLE1)
			object[v1]._oAnimFrame = 2;
		if (v4 == OBJ_MCIRCLE2)
			object[v1]._oAnimFrame = 4;
		if (v2 == 45) {
			if (v3 == 47) {
				object[v1]._oVar6 = 2;
				return;
			}
		} else if (v2 == 26 && v3 == 46) {
			object[v1]._oVar6 = 1;
			return;
		}
		object[v1]._oVar6 = 0;
		if (v2 == 35 && v3 == 36 && object[v1]._oVar5 == 3) {
			v5 = object[v1]._oVar4;
			v6 = object[v1]._oVar2;
			object[v1]._oVar6 = 4;
			ObjChangeMapResync(object[v1]._oVar1, v6, object[v1]._oVar3, v5);
			if (quests[QTYPE_VB]._qactive == 2)
				quests[QTYPE_VB]._qvar1 = 4;
			AddMissile(plr[myplr].WorldX, plr[myplr].WorldY, 35, 46, plr[myplr]._pdir, MIS_RNDTELEPORT, 0, myplr, 0, 0);
			track_repeat_walk(0);
			sgbMouseDown = 0;
			ReleaseCapture();
			ClrPlrPath(myplr);
			StartStand(myplr, 0);
		}
	}
}
// 525748: using guessed type char sgbMouseDown;

void __fastcall Obj_StopAnim(int i)
{
	if (object[i]._oAnimFrame == object[i]._oAnimLen) {
		object[i]._oAnimCnt = 0;
		object[i]._oAnimDelay = 1000;
	}
}

void __fastcall Obj_Door(int i)
{
	int dy; // edx
	int dx; // eax

	if (object[i]._oVar4) {
		dy = object[i]._oy;
		dx = object[i]._ox;
		object[i]._oSelFlag = 2;
		object[i]._oMissFlag = TRUE;
		object[i]._oVar4 = ((dItem[dx][dy] == 0
		                        && dDead[dx][dy] == 0
		                        && dPlayer[dx][dy] == 0
		                        && dMonster[dx][dy] == 0)
		                       == 0)
		    + 1;
	} else {
		object[i]._oMissFlag = FALSE;
		object[i]._oSelFlag = 3;
	}
}

void __fastcall Obj_Sarc(int i)
{
	if (object[i]._oAnimFrame == object[i]._oAnimLen)
		object[i]._oAnimFlag = 0;
}

void __fastcall ActivateTrapLine(int ttype, int tid)
{
	int v2; // edi
	int i;  // ebp
	int v4; // esi
	int v5; // edx
	int v6; // ecx
	int v7; // [esp+8h] [ebp-4h]

	v2 = 0;
	v7 = tid;
	for (i = ttype; v2 < nobjects; ++v2) {
		v4 = objectactive[v2];
		if (object[v4]._otype == i && object[v4]._oVar1 == v7) {
			v5 = object[v4]._oy;
			v6 = object[v4]._ox;
			object[v4]._oVar4 = 1;
			object[v4]._oAnimFlag = 1;
			object[v4]._oAnimDelay = 1;
			object[v4]._olid = AddLight(v6, v5, 1);
		}
	}
}

void __fastcall Obj_FlameTrap(int i)
{
	int x, y;
	int j, k;

	if (object[i]._oVar2) {
		if (object[i]._oVar4) {
			object[i]._oAnimFrame--;
			if (object[i]._oAnimFrame == 1) {
				object[i]._oVar4 = 0;
				AddUnLight(object[i]._olid);
			} else if (object[i]._oAnimFrame <= 4) {
				ChangeLightRadius(object[i]._olid, object[i]._oAnimFrame);
			}
		}
	} else if (!object[i]._oVar4) {
		if (object[i]._oVar3 == 2) {
			x = object[i]._ox - 2;
			y = object[i]._oy;
			for (j = 0; j < 5; j++) {
				if (dPlayer[x][y] || dMonster[x][y])
					object[i]._oVar4 = 1;
				x++;
			}
		} else {
			x = object[i]._ox;
			y = object[i]._oy - 2;
			for (k = 0; k < 5; k++) {
				if (dPlayer[x][y] || dMonster[x][y])
					object[i]._oVar4 = 1;
				y++;
			}
		}
		if (object[i]._oVar4)
			ActivateTrapLine(object[i]._otype, object[i]._oVar1);
	} else {
		if (object[i]._oAnimFrame == object[i]._oAnimLen)
			object[i]._oAnimFrame = 11;
		if (object[i]._oAnimFrame <= 5)
			ChangeLightRadius(object[i]._olid, object[i]._oAnimFrame);
	}
}

void __fastcall Obj_Trap(int i)
{
	int oti, dir;
	BOOL otrig;
	int sx, sy, dx, dy, x, y;

	otrig = FALSE;
	if (!object[i]._oVar4) {
		oti = dObject[object[i]._oVar1][object[i]._oVar2] - 1;
		switch (object[oti]._otype) {
		case OBJ_L1LDOOR:
		case OBJ_L1RDOOR:
		case OBJ_L2LDOOR:
		case OBJ_L2RDOOR:
		case OBJ_L3LDOOR:
		case OBJ_L3RDOOR:
			if (object[oti]._oVar4)
				otrig = TRUE;
			break;
		case OBJ_LEVER:
		case OBJ_CHEST1:
		case OBJ_CHEST2:
		case OBJ_CHEST3:
		case OBJ_SWITCHSKL:
		case OBJ_SARC:
			if (!object[oti]._oSelFlag)
				otrig = TRUE;
			break;
		}
		if (otrig) {
			object[i]._oVar4 = 1;
			sx = object[i]._ox;
			sy = object[i]._oy;
			dx = object[oti]._ox;
			dy = object[oti]._oy;
			for (y = dy - 1; y <= object[oti]._oy + 1; y++) {
				for (x = object[oti]._ox - 1; x <= object[oti]._ox + 1; x++) {
					if (dPlayer[x][y]) {
						dx = x;
						dy = y;
					}
				}
			}
			if (!deltaload) {
				dir = GetDirection(sx, sy, dx, dy);
				AddMissile(sx, sy, dx, dy, dir, object[i]._oVar3, 1, -1, 0, 0);
				PlaySfxLoc(IS_TRAP, object[oti]._ox, object[oti]._oy);
			}
			object[oti]._oTrapFlag = 0;
		}
	}
}
// 676190: using guessed type int deltaload;

void __fastcall Obj_BCrossDamage(int i)
{
	int v1;        // esi
	BOOLEAN v2;    // zf
	int v3;        // ecx
	int v4;        // edx
	int v6;        // ecx
	int damage[4]; // [esp+4h] [ebp-18h]
	int v8;        // [esp+18h] [ebp-4h]

	v1 = myplr;
	v8 = i;
	v2 = plr[myplr]._pmode == PM_DEATH;
	damage[0] = 6;
	damage[1] = 8;
	damage[2] = 10;
	damage[3] = 12;
	if (!v2) {
		v3 = plr[v1]._pFireResist;
		if (v3 > 0)
			damage[leveltype - 1] -= v3 * damage[leveltype - 1] / 100;
		if (plr[v1].WorldX == object[v8]._ox && plr[v1].WorldY == object[v8]._oy - 1) {
			v4 = damage[leveltype - 1];
			plr[v1]._pHitPoints -= v4;
			plr[v1]._pHPBase -= v4;
			if (plr[v1]._pHitPoints >> 6 <= 0) {
				SyncPlrKill(myplr, 0);
				drawhpflag = TRUE;
				return;
			}
			if (plr[myplr]._pClass == PC_WARRIOR) {
				v6 = PS_WARR68;
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				v6 = PS_ROGUE68;
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				v6 = PS_MAGE68;
			}
			PlaySfxLoc(v6, plr[v1].WorldX, plr[v1].WorldY);
			drawhpflag = TRUE;
		}
	}
}

void __cdecl ProcessObjects()
{
	int v0;  // ebx
	int v1;  // edi
	int v2;  // esi
	int v3;  // eax
	int *v4; // eax
	int *v5; // eax
	int v6;  // edx

	v0 = 0;
	if (nobjects > 0) {
		while (1) {
			v1 = objectactive[v0];
			v2 = objectactive[v0];
			v3 = object[v2]._otype;
			if (v3 <= OBJ_SARC)
				break;
			if (v3 <= OBJ_L3RDOOR) {
				if (v3 >= OBJ_L3LDOOR)
					goto LABEL_32;
				if (v3 == OBJ_FLAMEHOLE) {
					Obj_FlameTrap(v1);
					goto LABEL_40;
				}
				if (v3 <= OBJ_BOOKLVR)
					goto LABEL_40;
				if (v3 <= OBJ_TRAPR) {
					Obj_Trap(v1);
					goto LABEL_40;
				}
				if (v3 <= OBJ_WEAPRACK)
					goto LABEL_40;
				if (v3 <= OBJ_SHRINER) {
				LABEL_29:
					Obj_StopAnim(v1);
					goto LABEL_40;
				}
				if (v3 != OBJ_BOOKCANDLE)
					goto LABEL_40;
			LABEL_28:
				Obj_Light(v1, 5);
				goto LABEL_40;
			}
			if (v3 < OBJ_MCIRCLE1)
				goto LABEL_40;
			if (v3 <= OBJ_MCIRCLE2) {
				Obj_Circle(v1);
			} else {
				if (v3 != OBJ_STORYCANDLE) {
					if (v3 != OBJ_TBCROSS)
						goto LABEL_40;
					goto LABEL_37;
				}
				Obj_Light(v1, 3);
			}
		LABEL_40:
			if (object[v2]._oAnimFlag) {
				v4 = &object[v2]._oAnimCnt;
				++*v4;
				if (object[v2]._oAnimCnt >= object[v2]._oAnimDelay) {
					*v4 = 0;
					v5 = &object[v2]._oAnimFrame;
					++*v5;
					if (object[v2]._oAnimFrame > object[v2]._oAnimLen)
						*v5 = 1;
				}
			}
			if (++v0 >= nobjects)
				goto LABEL_45;
		}
		if (v3 == OBJ_SARC) {
			Obj_Sarc(v1);
			goto LABEL_40;
		}
		if (v3 > OBJ_CRUX3) {
			if (v3 != OBJ_BCROSS) {
				if (v3 <= OBJ_BOOK2R)
					goto LABEL_40;
				if (v3 > OBJ_L2RDOOR) {
					if (v3 <= OBJ_TORCHR2)
						Obj_Light(v1, 8);
					goto LABEL_40;
				}
			LABEL_32:
				Obj_Door(v1);
				goto LABEL_40;
			}
		LABEL_37:
			Obj_Light(v1, 10);
			Obj_BCrossDamage(v1);
			goto LABEL_40;
		}
		if (v3 >= OBJ_CRUX1)
			goto LABEL_29;
		if (!v3) {
			Obj_Light(v1, 10);
			goto LABEL_40;
		}
		if (v3 <= 0)
			goto LABEL_40;
		if (v3 <= OBJ_L1RDOOR)
			goto LABEL_32;
		if (v3 != OBJ_SKFIRE && v3 != OBJ_CANDLE2)
			goto LABEL_40;
		goto LABEL_28;
	}
LABEL_45:
	v6 = 0;
	while (v6 < nobjects) {
		if (object[objectactive[v6]]._oDelFlag) {
			DeleteObject_(objectactive[v6], v6);
			v6 = 0;
		} else {
			++v6;
		}
	}
}

void __fastcall ObjSetMicro(int dx, int dy, int pn)
{
	int v3;        // esi
	char *v4;      // eax
	char *v5;      // edx
	signed int v6; // ecx
	char *v7;      // esi
	signed int v8; // ecx

	dPiece[dx][dy] = pn;
	v3 = pn - 1;
	v4 = (char *)dpiece_defs_map_1 + 32 * gendung_get_dpiece_num_from_coord(dx, dy);
	if (leveltype == DTYPE_HELL) {
		v7 = (char *)pLevelPieces + 32 * v3;
		v8 = 0;
		do {
			*(_WORD *)&v4[2 * v8] = *(_WORD *)&v7[2 * ((v8 & 1) - (v8 & 0xE)) + 28];
			++v8;
		} while (v8 < 16);
	} else {
		v5 = (char *)pLevelPieces + 20 * v3;
		v6 = 0;
		do {
			*(_WORD *)&v4[2 * v6] = *(_WORD *)&v5[2 * ((v6 & 1) - (v6 & 0xE)) + 16];
			++v6;
		} while (v6 < 10);
	}
}

void __fastcall objects_set_door_piece(int x, int y)
{
	int v2;   // edi
	int v3;   // ST10_4
	int v4;   // ST18_4
	short v5; // ST14_2
	short v6; // ST0C_2

	v2 = y;
	v3 = x;
	v4 = dPiece[x][y] - 1;
	v5 = *((_WORD *)pLevelPieces + 10 * (unsigned short)v4 + 8);
	v6 = *((_WORD *)pLevelPieces + 10 * (unsigned short)v4 + 9);
	dpiece_defs_map_1[0][16 * gendung_get_dpiece_num_from_coord(x, y)] = v5;
	dpiece_defs_map_1[0][16 * gendung_get_dpiece_num_from_coord(v3, v2) + 1] = v6;
}

void __fastcall ObjSetMini(int x, int y, int v)
{
	int xx, yy;
	long v1, v2, v3, v4;

#if (_MSC_VER >= 800) && (_MSC_VER <= 1200)
	__asm {
		mov		esi, pMegaTiles
		xor		eax, eax
		mov		ax, word ptr v
		dec		eax
		shl		eax, 3
		add		esi, eax
		xor		eax, eax
		lodsw
		inc		eax
		mov		v1, eax
		lodsw
		inc		eax
		mov		v2, eax
		lodsw
		inc		eax
		mov		v3, eax
		lodsw
		inc		eax
		mov		v4, eax
	}
#else
	v1 = *((WORD *)&pMegaTiles[((WORD)v-1)*8])+1;
	v2 = *((WORD *)&pMegaTiles[((WORD)v-1)*8]+1)+1;
	v3 = *((WORD *)&pMegaTiles[((WORD)v-1)*8]+2)+1;
	v4 = *((WORD *)&pMegaTiles[((WORD)v-1)*8]+3)+1;
#endif

	xx = 2 * x + 16;
	yy = 2 * y + 16;
	ObjSetMicro(xx, yy, v1);
	ObjSetMicro(xx + 1, yy, v2);
	ObjSetMicro(xx, yy + 1, v3);
	ObjSetMicro(xx + 1, yy + 1, v4);
}

void __fastcall ObjL1Special(int x1, int y1, int x2, int y2)
{
	int i;     // ebx
	int v5;    // edx
	_BYTE *v6; // eax
	int *v7;   // edi
	int v8;    // edx
	int v9;    // esi

	for (i = y1; i <= y2; ++i) {
		if (x1 <= x2) {
			v5 = 112 * x1 + i;
			v6 = (unsigned char *)dArch + v5;
			v7 = (int *)((char *)dPiece + 4 * v5);
			v8 = x2 - x1 + 1;
			do {
				v9 = *v7;
				*v6 = 0;
				if (v9 == 12)
					*v6 = 1;
				if (v9 == 11)
					*v6 = 2;
				if (v9 == 71)
					*v6 = 1;
				if (v9 == 259)
					*v6 = 5;
				if (v9 == 249)
					*v6 = 2;
				if (v9 == 325)
					*v6 = 2;
				if (v9 == 321)
					*v6 = 1;
				if (v9 == 255)
					*v6 = 4;
				if (v9 == 211)
					*v6 = 1;
				if (v9 == 344)
					*v6 = 2;
				if (v9 == 341)
					*v6 = 1;
				if (v9 == 331)
					*v6 = 2;
				if (v9 == 418)
					*v6 = 1;
				if (v9 == 421)
					*v6 = 2;
				v7 += 112;
				v6 += 112;
				--v8;
			} while (v8);
		}
	}
}

void __fastcall ObjL2Special(int x1, int y1, int x2, int y2)
{
	int v4;    // edi
	int v5;    // esi
	_BYTE *v6; // eax
	int *v7;   // ebx
	int v8;    // esi
	int v9;    // edx
	int i;     // edi
	int v11;   // eax
	char *v12; // edx
	int *v13;  // esi
	int v14;   // eax
	int v15;   // ebx
	int v16;   // [esp+Ch] [ebp-4h]

	v4 = y1;
	v16 = y1;
	if (y1 <= y2) {
		do {
			if (x1 <= x2) {
				v5 = 112 * x1 + v4;
				v6 = (unsigned char *)dArch + v5;
				v7 = (int *)((char *)dPiece + 4 * v5);
				v8 = x2 - x1 + 1;
				do {
					v9 = *v7;
					*v6 = 0;
					if (v9 == 541)
						*v6 = 5;
					if (v9 == 178)
						*v6 = 5;
					if (v9 == 551)
						*v6 = 5;
					if (v9 == 542)
						*v6 = 6;
					if (v9 == 553)
						*v6 = 6;
					if (v9 == 13)
						*v6 = 5;
					if (v9 == 17)
						*v6 = 6;
					v7 += 112;
					v6 += 112;
					--v8;
				} while (v8);
			}
			++v4;
		} while (v4 <= y2);
		for (i = v16; i <= y2; ++i) {
			if (x1 <= x2) {
				v11 = i + 112 * x1;
				v12 = &dArch[0][v11 + 2];
				v13 = (int *)((char *)dPiece + 4 * v11);
				v14 = x2 - x1 + 1;
				do {
					v15 = *v13;
					if (*v13 == 132) {
						*(v12 - 1) = 2;
						*v12 = 1;
					}
					if (v15 == 135 || v15 == 139) {
						v12[110] = 3;
						v12[222] = 4;
					}
					v13 += 112;
					v12 += 112;
					--v14;
				} while (v14);
			}
		}
	}
}

void __fastcall DoorSet(int oi, int dx, int dy)
{
	int v3;           // esi
	int v4;           // ebp
	int v5;           // ebx
	ObjectStruct *v6; // ebp

	v3 = dx;
	v4 = oi;
	v5 = dPiece[dx][dy];
	if (v5 == 43)
		ObjSetMicro(dx, dy, 392);
	if (v5 == 45)
		ObjSetMicro(v3, dy, 394);
	if (v5 != 50)
		goto LABEL_10;
	v6 = &object[v4];
	if (v6->_otype == OBJ_L1LDOOR)
		ObjSetMicro(v3, dy, 411);
	if (v6->_otype == OBJ_L1RDOOR) {
		ObjSetMicro(v3, dy, 412);
	LABEL_10:
		if (v5 == 54)
			ObjSetMicro(v3, dy, 397);
		if (v5 == 55)
			ObjSetMicro(v3, dy, 398);
		if (v5 == 61)
			ObjSetMicro(v3, dy, 399);
		if (v5 == 67)
			ObjSetMicro(v3, dy, 400);
		if (v5 == 68)
			ObjSetMicro(v3, dy, 401);
		if (v5 == 69)
			ObjSetMicro(v3, dy, 403);
		if (v5 == 70)
			ObjSetMicro(v3, dy, 404);
		if (v5 == 72)
			ObjSetMicro(v3, dy, 406);
		if (v5 == 212)
			ObjSetMicro(v3, dy, 407);
		if (v5 == 354)
			ObjSetMicro(v3, dy, 409);
		if (v5 == 355)
			ObjSetMicro(v3, dy, 410);
		if (v5 == 411)
			ObjSetMicro(v3, dy, 396);
		if (v5 == 412)
			ObjSetMicro(v3, dy, 396);
	}
}

void __cdecl RedoPlayerVision()
{
	int *v0; // esi

	v0 = &plr[0].plrlevel;
	do {
		if (*((_BYTE *)v0 - 23)) {
			if (currlevel == *v0)
				ChangeVisionXY(v0[27], v0[1], v0[2]);
		}
		v0 += 5430;
	} while ((signed int)v0 < (signed int)&plr[4].plrlevel);
}

void __fastcall OperateL1RDoor(int pnum, int oi, unsigned char sendflag)
{
	int v3;     // esi
	int v4;     // eax
	int v5;     // ebx
	int v6;     // edi
	int v7;     // ST04_4
	int v8;     // [esp+Ch] [ebp-Ch]
	int v9;     // [esp+10h] [ebp-8h]
	int param1; // [esp+14h] [ebp-4h]

	v3 = oi;
	param1 = oi;
	v9 = pnum;
	v4 = object[oi]._oVar4;
	if (v4 != 2) {
		v5 = object[v3]._ox;
		v6 = object[v3]._oy;
		if (v4) {
			if (!deltaload)
				PlaySfxLoc(IS_DOORCLOS, v5, object[v3]._oy);
			v8 = v6 + 112 * v5;
			if (dDead[0][v8] != 0 || dMonster[0][v8] != 0 || dItem[0][v8] != 0) {
				object[v3]._oVar4 = 2;
				return;
			}
			if (v9 == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, param1);
			v7 = object[v3]._oVar1;
			object[v3]._oVar4 = 0;
			object[v3]._oSelFlag = 3;
			ObjSetMicro(v5, v6, v7);
			if (object[v3]._oVar2 == 50) {
				if (dPiece[-1][v8] == 396) /* check *(_DWORD *)&dflags[28][4 * v8 + 32] == 396 ) */
					ObjSetMicro(v5 - 1, v6, 411);
				else
					ObjSetMicro(v5 - 1, v6, 50);
			} else {
				ObjSetMicro(v5 - 1, v6, object[v3]._oVar2);
			}
			object[v3]._oAnimFrame -= 2;
			object[v3]._oPreFlag = 0;
		} else {
			if (pnum == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, object[v3]._ox, object[v3]._oy);
			ObjSetMicro(v5, v6, 395);
			dArch[v5][v6] = 8;
			objects_set_door_piece(v5, v6 - 1);
			object[v3]._oAnimFrame += 2;
			object[v3]._oPreFlag = 1;
			DoorSet(param1, v5 - 1, v6);
			object[v3]._oVar4 = 1;
			object[v3]._oSelFlag = 2;
		}
		RedoPlayerVision();
		return;
	}
	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, object[v3]._oy);
}
// 676190: using guessed type int deltaload;

void __fastcall OperateL1LDoor(int pnum, int oi, unsigned char sendflag)
{
	int v3;     // esi
	int v4;     // eax
	int v5;     // ebx
	int v6;     // edi
	int v7;     // ST04_4
	int v8;     // [esp+Ch] [ebp-Ch]
	int v9;     // [esp+10h] [ebp-8h]
	int param1; // [esp+14h] [ebp-4h]

	v3 = oi;
	param1 = oi;
	v9 = pnum;
	v4 = object[oi]._oVar4;
	if (v4 != 2) {
		v5 = object[v3]._ox;
		v6 = object[v3]._oy;
		if (v4) {
			if (!deltaload)
				PlaySfxLoc(IS_DOORCLOS, v5, object[v3]._oy);
			v8 = v6 + 112 * v5;
			if (dDead[v5][v6] != 0 || dMonster[0][v8] != 0 || dItem[v5][v6] != 0) {
				object[v3]._oVar4 = 2;
				return;
			}
			if (v9 == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, param1);
			v7 = object[v3]._oVar1;
			object[v3]._oVar4 = 0;
			object[v3]._oSelFlag = 3;
			ObjSetMicro(v5, v6, v7);
			if (object[v3]._oVar2 == 50) {
				if (dPiece[0][v8 - 1] == 396) /* check  *(_DWORD *)&dflags[39][v8 * 4 + 36] == 396 ) */
					ObjSetMicro(v5, v6 - 1, 412);
				else
					ObjSetMicro(v5, v6 - 1, 50);
			} else {
				ObjSetMicro(v5, v6 - 1, object[v3]._oVar2);
			}
			object[v3]._oAnimFrame -= 2;
			object[v3]._oPreFlag = 0;
		} else {
			if (pnum == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, object[v3]._ox, object[v3]._oy);
			if (object[v3]._oVar1 == 214)
				ObjSetMicro(v5, v6, 408);
			else
				ObjSetMicro(v5, v6, 393);
			dArch[v5][v6] = 7;
			objects_set_door_piece(v5 - 1, v6);
			object[v3]._oAnimFrame += 2;
			object[v3]._oPreFlag = 1;
			DoorSet(param1, v5, v6 - 1);
			object[v3]._oVar4 = 1;
			object[v3]._oSelFlag = 2;
		}
		RedoPlayerVision();
		return;
	}
	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, object[v3]._oy);
}
// 676190: using guessed type int deltaload;

void __fastcall OperateL2RDoor(int pnum, int oi, unsigned char sendflag)
{
	int v3;       // esi
	int v4;       // eax
	int v5;       // ebx
	short param1; // [esp+Ch] [ebp-Ch]
	int v7;       // [esp+10h] [ebp-8h]
	int v8;       // [esp+14h] [ebp-4h]

	v3 = oi;
	param1 = oi;
	v7 = pnum;
	v4 = object[oi]._oVar4;
	if (v4 != 2) {
		v5 = object[v3]._oy;
		v8 = object[v3]._ox;
		if (v4) {
			if (!deltaload)
				PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, v5);
			if (dDead[v8][v5] != 0 || dMonster[v8][v5] != 0 || dItem[v8][v5] != 0) {
				object[v3]._oVar4 = 2;
				return;
			}
			if (v7 == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, param1);
			object[v3]._oVar4 = 0;
			object[v3]._oSelFlag = 3;
			ObjSetMicro(v8, v5, 540);
			object[v3]._oAnimFrame -= 2;
			object[v3]._oPreFlag = 0;
		} else {
			if (pnum == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, object[v3]._ox, object[v3]._oy);
			ObjSetMicro(v8, v5, 17);
			object[v3]._oAnimFrame += 2;
			object[v3]._oPreFlag = 1;
			object[v3]._oVar4 = 1;
			object[v3]._oSelFlag = 2;
		}
		RedoPlayerVision();
		return;
	}
	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, object[v3]._oy);
}
// 676190: using guessed type int deltaload;

void __fastcall OperateL2LDoor(int pnum, int oi, unsigned char sendflag)
{
	int v3;       // esi
	int v4;       // eax
	int v5;       // ebx
	short param1; // [esp+Ch] [ebp-Ch]
	int v7;       // [esp+10h] [ebp-8h]
	int v8;       // [esp+14h] [ebp-4h]

	v3 = oi;
	param1 = oi;
	v7 = pnum;
	v4 = object[oi]._oVar4;
	if (v4 != 2) {
		v5 = object[v3]._oy;
		v8 = object[v3]._ox;
		if (v4) {
			if (!deltaload)
				PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, v5);
			if (dDead[v8][v5] != 0 || dMonster[v8][v5] != 0 || dItem[v8][v5] != 0) {
				object[v3]._oVar4 = 2;
				return;
			}
			if (v7 == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, param1);
			object[v3]._oVar4 = 0;
			object[v3]._oSelFlag = 3;
			ObjSetMicro(v8, v5, 538);
			object[v3]._oAnimFrame -= 2;
			object[v3]._oPreFlag = 0;
		} else {
			if (pnum == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, object[v3]._ox, object[v3]._oy);
			ObjSetMicro(v8, v5, 13);
			object[v3]._oAnimFrame += 2;
			object[v3]._oPreFlag = 1;
			object[v3]._oVar4 = 1;
			object[v3]._oSelFlag = 2;
		}
		RedoPlayerVision();
		return;
	}
	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, object[v3]._oy);
}
// 676190: using guessed type int deltaload;

void __fastcall OperateL3RDoor(int pnum, int oi, unsigned char sendflag)
{
	int v3;       // esi
	int v4;       // eax
	int v5;       // ebx
	short param1; // [esp+Ch] [ebp-Ch]
	int v7;       // [esp+10h] [ebp-8h]
	int v8;       // [esp+14h] [ebp-4h]

	v3 = oi;
	param1 = oi;
	v7 = pnum;
	v4 = object[oi]._oVar4;
	if (v4 != 2) {
		v5 = object[v3]._oy;
		v8 = object[v3]._ox;
		if (v4) {
			if (!deltaload)
				PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, v5);
			if (dDead[v8][v5] != 0 || dMonster[v8][v5] != 0 || dItem[v8][v5] != 0) {
				object[v3]._oVar4 = 2;
				return;
			}
			if (v7 == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, param1);
			object[v3]._oVar4 = 0;
			object[v3]._oSelFlag = 3;
			ObjSetMicro(v8, v5, 534);
			object[v3]._oAnimFrame -= 2;
			object[v3]._oPreFlag = 0;
		} else {
			if (pnum == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, object[v3]._ox, object[v3]._oy);
			ObjSetMicro(v8, v5, 541);
			object[v3]._oAnimFrame += 2;
			object[v3]._oPreFlag = 1;
			object[v3]._oVar4 = 1;
			object[v3]._oSelFlag = 2;
		}
		RedoPlayerVision();
		return;
	}
	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, object[v3]._oy);
}
// 676190: using guessed type int deltaload;

void __fastcall OperateL3LDoor(int pnum, int oi, unsigned char sendflag)
{
	int v3;       // esi
	int v4;       // eax
	int v5;       // ebx
	short param1; // [esp+Ch] [ebp-Ch]
	int v7;       // [esp+10h] [ebp-8h]
	int v8;       // [esp+14h] [ebp-4h]

	v3 = oi;
	param1 = oi;
	v7 = pnum;
	v4 = object[oi]._oVar4;
	if (v4 != 2) {
		v5 = object[v3]._oy;
		v8 = object[v3]._ox;
		if (v4) {
			if (!deltaload)
				PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, v5);
			if (dDead[v8][v5] != 0 || dMonster[v8][v5] != 0 || dItem[v8][v5] != 0) {
				object[v3]._oVar4 = 2;
				return;
			}
			if (v7 == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, param1);
			object[v3]._oVar4 = 0;
			object[v3]._oSelFlag = 3;
			ObjSetMicro(v8, v5, 531);
			object[v3]._oAnimFrame -= 2;
			object[v3]._oPreFlag = 0;
		} else {
			if (pnum == myplr && sendflag)
				NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, object[v3]._ox, object[v3]._oy);
			ObjSetMicro(v8, v5, 538);
			object[v3]._oAnimFrame += 2;
			object[v3]._oPreFlag = 1;
			object[v3]._oVar4 = 1;
			object[v3]._oSelFlag = 2;
		}
		RedoPlayerVision();
		return;
	}
	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[v3]._ox, object[v3]._oy);
}
// 676190: using guessed type int deltaload;

void __fastcall MonstCheckDoors(int m)
{
	int i, oi;
	int dpx, dpy, mx, my;

	mx = monster[m]._mx;
	my = monster[m]._my;
	if (dObject[mx - 1][my - 1]
	    || dObject[mx][my - 1]
	    || dObject[mx + 1][my - 1]
	    || dObject[mx - 1][my]
	    || dObject[mx + 1][my]
	    || dObject[mx - 1][my + 1]
	    || dObject[mx][my + 1]
	    || dObject[mx + 1][my + 1]) {
		for (i = 0; i < nobjects; ++i) {
			oi = objectactive[i];
			if ((object[oi]._otype == OBJ_L1LDOOR || object[oi]._otype == OBJ_L1RDOOR) && !object[oi]._oVar4) {
				dpx = abs(object[oi]._ox - mx);
				dpy = abs(object[oi]._oy - my);
				if (dpx == 1 && dpy <= 1 && object[oi]._otype == OBJ_L1LDOOR)
					OperateL1LDoor(myplr, oi, TRUE);
				if (dpx <= 1 && dpy == 1 && object[oi]._otype == OBJ_L1RDOOR)
					OperateL1RDoor(myplr, oi, TRUE);
			}
			if ((object[oi]._otype == OBJ_L2LDOOR || object[oi]._otype == OBJ_L2RDOOR) && !object[oi]._oVar4) {
				dpx = abs(object[oi]._ox - mx);
				dpy = abs(object[oi]._oy - my);
				if (dpx == 1 && dpy <= 1 && object[oi]._otype == OBJ_L2LDOOR)
					OperateL2LDoor(myplr, oi, TRUE);
				if (dpx <= 1 && dpy == 1 && object[oi]._otype == OBJ_L2RDOOR)
					OperateL2RDoor(myplr, oi, TRUE);
			}
			if ((object[oi]._otype == OBJ_L3LDOOR || object[oi]._otype == OBJ_L3RDOOR) && !object[oi]._oVar4) {
				dpx = abs(object[oi]._ox - mx);
				dpy = abs(object[oi]._oy - my);
				if (dpx == 1 && dpy <= 1 && object[oi]._otype == OBJ_L3RDOOR)
					OperateL3RDoor(myplr, oi, TRUE);
				if (dpx <= 1 && dpy == 1 && object[oi]._otype == OBJ_L3LDOOR)
					OperateL3LDoor(myplr, oi, TRUE);
			}
		}
	}
}

void __fastcall ObjChangeMap(int x1, int y1, int x2, int y2)
{
	int v4; // ebx
	int v5; // edi
	int v6; // esi
	//int v7; // ecx
	int v8; // edi
	int v9; // ebx
	//int v10; // ecx
	int v11;   // [esp+Ch] [ebp-8h]
	int a2;    // [esp+10h] [ebp-4h]
	int i;     // [esp+1Ch] [ebp+8h]
	int y_end; // [esp+20h] [ebp+Ch]

	v4 = y1;
	v5 = x2;
	v6 = x1;
	for (a2 = y1; a2 <= y2; ++a2) {
		i = v6;
		if (v6 <= v5) {
			v11 = a2 + 40 * v6;
			do {
				ObjSetMini(i++, a2, (unsigned char)pdungeon[0][v11]);
				dungeon[0][v11] = pdungeon[0][v11];
				v11 += 40;
			} while (i <= v5);
		}
	}
	if (leveltype == DTYPE_CATHEDRAL) {
		ObjL1Special(2 * v6 + 16, 2 * v4 + 16, 2 * v5 + 17, 2 * y2 + 17);
		AddL1Objs(2 * v6 + 16, 2 * v4 + 16, 2 * v5 + 17, 2 * y2 + 17); /* v7 */
	}
	if (leveltype == DTYPE_CATACOMBS) {
		v8 = 2 * v5 + 17;
		v9 = 2 * v4 + 16;
		y_end = 2 * y2 + 17;
		ObjL2Special(2 * v6 + 16, v9, v8, y_end);
		AddL2Objs(2 * v6 + 16, v9, v8, y_end); /* v10 */
	}
}

void __fastcall ObjChangeMapResync(int x1, int y1, int x2, int y2)
{
	int v4; // edi
	int v5; // esi
	int v6; // ebx
	int v7; // edi
	int v8; // [esp+Ch] [ebp-Ch]
	int i;  // [esp+10h] [ebp-8h]
	int a2; // [esp+14h] [ebp-4h]

	v4 = y2;
	v5 = y1;
	v6 = x1;
	v8 = y1;
	for (a2 = y1; a2 <= v4; ++a2) {
		i = v6;
		if (v6 <= x2) {
			v7 = a2 + 40 * v6;
			do {
				ObjSetMini(i++, a2, (unsigned char)pdungeon[0][v7]);
				dungeon[0][v7] = pdungeon[0][v7];
				v7 += 40;
			} while (i <= x2);
			v4 = y2;
			v5 = v8;
		}
	}
	if (leveltype == DTYPE_CATHEDRAL)
		ObjL1Special(2 * v6 + 16, 2 * v5 + 16, 2 * x2 + 17, 2 * v4 + 17);
	if (leveltype == DTYPE_CATACOMBS)
		ObjL2Special(2 * v6 + 16, 2 * v5 + 16, 2 * x2 + 17, 2 * v4 + 17);
}

void __fastcall OperateL1Door(int pnum, int i, unsigned char sendflag)
{
	int dpx, dpy;

	dpx = abs(object[i]._ox - plr[pnum].WorldX);
	dpy = abs(object[i]._oy - plr[pnum].WorldY);
	if (dpx == 1 && dpy <= 1 && object[i]._otype == OBJ_L1LDOOR)
		OperateL1LDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && object[i]._otype == OBJ_L1RDOOR)
		OperateL1RDoor(pnum, i, sendflag);
}

void __fastcall OperateLever(int pnum, int i)
{
	int v2;        // esi
	int *v3;       // edi
	signed int v4; // edi
	int v5;        // ecx
	int v6;        // eax
	short param1;  // [esp+8h] [ebp-8h]
	int v8;        // [esp+Ch] [ebp-4h]

	param1 = i;
	v2 = i;
	v3 = (int *)&object[i]._oSelFlag;
	v8 = pnum;
	if (*(_BYTE *)v3) {
		if (!deltaload)
			PlaySfxLoc(IS_LEVER, object[v2]._ox, object[v2]._oy);
		*(_BYTE *)v3 = 0;
		++object[v2]._oAnimFrame;
		v4 = 1;
		if (currlevel != 16)
			goto LABEL_17;
		v5 = 0;
		if (nobjects <= 0)
			goto LABEL_17;
		do {
			v6 = objectactive[v5];
			if (object[v6]._otype == OBJ_SWITCHSKL
			    && object[v2]._oVar8 == object[v6]._oVar8
			    && object[v6]._oSelFlag) {
				v4 = 0;
			}
			++v5;
		} while (v5 < nobjects);
		if (v4)
		LABEL_17:
			ObjChangeMap(object[v2]._oVar1, object[v2]._oVar2, object[v2]._oVar3, object[v2]._oVar4);
		if (v8 == myplr)
			NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, param1);
	}
}
// 676190: using guessed type int deltaload;

void __fastcall OperateBook(int pnum, int i)
{
	signed int v4;  // ecx
	int v5;         // eax
	BOOLEAN v6;     // zf
	int j;          // esi
	signed int v11; // [esp+10h] [ebp-10h]
	signed int v1;  // [esp+14h] [ebp-Ch]
	signed int v2;  // [esp+18h] [ebp-8h]
	int v14;        // [esp+1Ch] [ebp-4h]

	if (!object[i]._oSelFlag)
		return;
	if (!setlevel || setlvlnum != SL_VILEBETRAYER)
		goto LABEL_17;
	v4 = 0;
	v11 = 0;
	v14 = 0;
	if (nobjects > 0) {
		while (1) {
			v5 = objectactive[v14];
			if (object[v5]._otype == OBJ_MCIRCLE2) {
				if (object[v5]._oVar6 == 1) {
					v1 = 27;
					v2 = 29;
					object[v5]._oVar6 = 4;
					v4 = 1;
				}
				if (object[v5]._oVar6 == 2) {
					v1 = 43;
					v2 = 29;
					object[v5]._oVar6 = 4;
					v4 = 1;
				}
			}
			if (v4) {
				++object[dObject[35][36] - 1]._oVar5; // ++objectavail[30 * dObject[35][36] + 123]; /* fix */
				AddMissile(plr[pnum].WorldX, plr[pnum].WorldY, v1, v2, plr[pnum]._pdir, MIS_RNDTELEPORT, 0, pnum, 0, 0);
				v11 = 1;
				v4 = 0;
			}
			if (++v14 >= nobjects)
				break;
		}
		if (v11) {
		LABEL_17:
			++object[i]._oAnimFrame;
			v6 = setlevel == 0;
			object[i]._oSelFlag = 0;
			if (!v6) {
				if (setlvlnum == SL_BONECHAMB) {
					plr[myplr]._pMemSpells |= (__int64)1 << (SPL_GUARDIAN - 1);
					if (plr[pnum]._pSplLvl[SPL_GUARDIAN] < 15)
						plr[myplr]._pSplLvl[SPL_GUARDIAN]++;
					quests[QTYPE_BONE]._qactive = 3;
					if (!deltaload)
						PlaySfxLoc(IS_QUESTDN, object[i]._ox, object[i]._oy);
					InitDiabloMsg(EMSG_BONECHAMB);
					AddMissile(
					    plr[myplr].WorldX,
					    plr[myplr].WorldY,
					    object[i]._ox - 2,
					    object[i]._oy - 4,
					    plr[myplr]._pdir,
					    MIS_GUARDIAN,
					    0,
					    myplr,
					    0,
					    0);
				}
				if (setlevel) {
					if (setlvlnum == SL_VILEBETRAYER) {
						ObjChangeMapResync(
						    object[i]._oVar1,
						    object[i]._oVar2,
						    object[i]._oVar3,
						    object[i]._oVar4);
						for (j = 0; j < nobjects; ++j)
							SyncObjectAnim(objectactive[j]);
					}
				}
			}
			return;
		}
	}
}
// 5CF31D: using guessed type char setlevel;
// 676190: using guessed type int deltaload;

void __fastcall OperateBookLever(int pnum, int i)
{
	int v2;       // esi
	int v3;       // edi
	int v4;       // ebp
	int v5;       // edx
	int v6;       // eax
	int v7;       // ST0C_4
	int v8;       // edx
	char v9;      // bl
	int v10;      // ST08_4
	int v11;      // ecx
	int v12;      // ecx
	int v13;      // [esp+Ch] [ebp-8h]
	short param1; // [esp+10h] [ebp-4h]

	param1 = i;
	v2 = i;
	v13 = pnum;
	v3 = 2 * setpc_x + 16;
	v4 = 2 * setpc_y + 16;
	if (object[i]._oSelFlag && !qtextflag) {
		v5 = object[v2]._otype;
		if (v5 == OBJ_BLINDBOOK && !quests[QTYPE_BLIND]._qvar1) {
			quests[QTYPE_BLIND]._qactive = 2;
			quests[QTYPE_BLIND]._qlog = 1;
			quests[QTYPE_BLIND]._qvar1 = 1;
		}
		if (v5 == OBJ_BLOODBOOK && !quests[QTYPE_BLOOD]._qvar1) {
			quests[QTYPE_BLOOD]._qactive = 2;
			quests[QTYPE_BLOOD]._qlog = 1;
			quests[QTYPE_BLOOD]._qvar1 = 1;
			SpawnQuestItem(21, 2 * setpc_x + 19, 2 * setpc_y + 26, 0, 1);
			SpawnQuestItem(21, 2 * setpc_x + 31, 2 * setpc_y + 26, 0, 1);
			SpawnQuestItem(21, 2 * setpc_x + 25, 2 * setpc_y + 33, 0, 1);
		}
		v6 = object[v2]._otype;
		if (v6 == OBJ_STEELTOME && !quests[QTYPE_WARLRD]._qvar1) {
			quests[QTYPE_WARLRD]._qactive = 2;
			quests[QTYPE_WARLRD]._qlog = 1;
			quests[QTYPE_WARLRD]._qvar1 = 1;
		}
		if (object[v2]._oAnimFrame != object[v2]._oVar6) {
			if (v6 != OBJ_BLOODBOOK)
				ObjChangeMap(object[v2]._oVar1, object[v2]._oVar2, object[v2]._oVar3, object[v2]._oVar4);
			if (object[v2]._otype == OBJ_BLINDBOOK) {
				CreateItem(3, v3 + 5, v4 + 5);
				v7 = object[v2]._oVar4;
				v8 = object[v2]._oVar2;
				v9 = TransVal;
				v10 = object[v2]._oVar3;
				v11 = object[v2]._oVar1;
				TransVal = 9;
				DRLG_MRectTrans(v11, v8, v10, v7);
				TransVal = v9;
			}
		}
		v12 = object[v2]._oVar7;
		object[v2]._oAnimFrame = object[v2]._oVar6;
		InitQTextMsg(v12);
		if (v13 == myplr)
			NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, param1);
	}
}
// 5A5590: using guessed type char TransVal;
// 646D00: using guessed type char qtextflag;

void __fastcall OperateSChambBk(int pnum, int i)
{
	int v2;        // esi
	int j;         // edi
	signed int v5; // ecx
	//int speech_id; // [esp+4h] [ebp-4h]

	v2 = i;
	if (object[i]._oSelFlag && !qtextflag) {
		if (object[v2]._oAnimFrame != object[v2]._oVar6) {
			ObjChangeMapResync(object[v2]._oVar1, object[v2]._oVar2, object[v2]._oVar3, object[v2]._oVar4);
			for (j = 0; j < nobjects; ++j)
				SyncObjectAnim(objectactive[j]);
		}
		object[v2]._oAnimFrame = object[v2]._oVar6;
		if (quests[QTYPE_BONE]._qactive == 1) {
			quests[QTYPE_BONE]._qactive = 2;
			quests[QTYPE_BONE]._qlog = 1;
		}
		if (plr[myplr]._pClass == PC_WARRIOR) {
			v5 = QUEST_BONER;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			v5 = QUEST_RBONER;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			v5 = QUEST_MBONER;
		}
		quests[QTYPE_BONE]._qmsg = v5;
		InitQTextMsg(v5);
	}
}
// 646D00: using guessed type char qtextflag;

void __fastcall OperateChest(int pnum, int i, unsigned char sendmsg)
{
	int v3;         // esi
	BOOLEAN v4;     // zf
	int v5;         // edi
	int v6;         // eax
	int v7;         // eax
	int v8;         // ecx
	int v9;         // ecx
	int v10;        // ecx
	signed int v11; // [esp-8h] [ebp-18h]
	short param2;   // [esp+8h] [ebp-8h]
	int param1;     // [esp+Ch] [ebp-4h]

	param2 = i;
	v3 = i;
	param1 = pnum;
	if (object[i]._oSelFlag) {
		if (!deltaload)
			PlaySfxLoc(IS_CHEST, object[v3]._ox, object[v3]._oy);
		object[v3]._oAnimFrame += 2;
		v4 = deltaload == 0;
		object[v3]._oSelFlag = 0;
		if (v4) {
			SetRndSeed(object[v3]._oRndSeed);
			v5 = 0;
			if (setlevel) {
				if (object[v3]._oVar1 > 0) {
					do {
						CreateRndItem(object[v3]._ox, object[v3]._oy, 1u, sendmsg, 0);
						++v5;
					} while (v5 < object[v3]._oVar1);
				}
			} else if (object[v3]._oVar1 > 0) {
				do {
					if (object[v3]._oVar2)
						CreateRndItem(object[v3]._ox, object[v3]._oy, 0, sendmsg, 0);
					else
						CreateRndUseful(param1, object[v3]._ox, object[v3]._oy, sendmsg);
					++v5;
				} while (v5 < object[v3]._oVar1);
			}
			if (!object[v3]._oTrapFlag)
				goto LABEL_26;
			v6 = object[v3]._otype;
			if (v6 < OBJ_TCHEST1 || v6 > OBJ_TCHEST3)
				goto LABEL_26;
			v7 = GetDirection(object[v3]._ox, object[v3]._oy, plr[param1].WorldX, plr[param1].WorldY);
			v8 = object[v3]._oVar4;
			if (v8) {
				v9 = v8 - 1;
				if (v9) {
					if (v9 != 1) {
						v10 = sendmsg;
						goto LABEL_25;
					}
					v11 = MIS_NOVA;
				} else {
					v11 = MIS_FARROW;
				}
				v10 = v11;
			} else {
				v10 = MIS_ARROW;
			}
		LABEL_25:
			AddMissile(object[v3]._ox, object[v3]._oy, plr[param1].WorldX, plr[param1].WorldY, v7, v10, 1, -1, 0, 0);
			object[v3]._oTrapFlag = 0;
		LABEL_26:
			if (param1 == myplr)
				NetSendCmdParam2(FALSE, CMD_PLROPOBJ, param1, param2);
			return;
		}
	}
}
// 5CF31D: using guessed type char setlevel;
// 676190: using guessed type int deltaload;

void __fastcall OperateMushPatch(int pnum, int i)
{
	int v2;     // esi
	BOOLEAN v3; // zf
	int v5;     // ecx
	int xx;     // [esp+8h] [ebp-8h]
	int yy;     // [esp+Ch] [ebp-4h]

	if (quests[QTYPE_BLKM]._qactive != 2 || quests[QTYPE_BLKM]._qvar1 < QS_TOMEGIVEN) {
		if (!deltaload && pnum == myplr) {
			if (plr[myplr]._pClass == PC_WARRIOR) {
				v5 = PS_WARR13;
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				v5 = PS_ROGUE13;
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				v5 = PS_MAGE13;
			}
			PlaySFX(v5);
		}
	} else {
		v2 = i;
		if (object[i]._oSelFlag) {
			if (!deltaload)
				PlaySfxLoc(IS_CHEST, object[v2]._ox, object[v2]._oy);
			++object[v2]._oAnimFrame;
			v3 = deltaload == 0;
			object[v2]._oSelFlag = 0;
			if (v3) {
				GetSuperItemLoc(object[v2]._ox, object[v2]._oy, &xx, &yy);
				SpawnQuestItem(IDI_MUSHROOM, xx, yy, 0, 0);
				quests[QTYPE_BLKM]._qvar1 = QS_MUSHSPAWNED;
			}
		}
	}
}
// 676190: using guessed type int deltaload;

void __fastcall OperateInnSignChest(int pnum, int i)
{
	int v4;     // esi
	BOOLEAN v5; // zf
	int xx;     // [esp+8h] [ebp-8h]
	int yy;     // [esp+Ch] [ebp-4h]

	if (quests[QTYPE_BOL]._qvar1 == 2) {
		v4 = i;
		if (object[i]._oSelFlag) {
			if (!deltaload)
				PlaySfxLoc(IS_CHEST, object[v4]._ox, object[v4]._oy);
			object[v4]._oAnimFrame += 2;
			v5 = deltaload == 0;
			object[v4]._oSelFlag = 0;
			if (v5) {
				GetSuperItemLoc(object[v4]._ox, object[v4]._oy, &xx, &yy);
				SpawnQuestItem(IDI_BANNER, xx, yy, 0, 0);
			}
		}
	} else if (!deltaload && pnum == myplr) {
		switch (plr[myplr]._pClass) {
		case PC_WARRIOR:
			PlaySFX(PS_WARR24);
			break;
		case PC_ROGUE:
			PlaySFX(PS_ROGUE24);
			break;
		case PC_SORCERER:
			PlaySFX(PS_MAGE24);
			break;
		}
	}
}
// 676190: using guessed type int deltaload;

void __fastcall OperateSlainHero(int pnum, int i, unsigned char sendmsg)
{
	unsigned short v3; // di
	int v4;            // esi
	int v5;            // eax
	BOOLEAN v6;        // zf
	int v8;            // ecx

	v3 = i;
	v4 = pnum;
	v5 = i;
	if (object[i]._oSelFlag) {
		v6 = deltaload == 0;
		object[v5]._oSelFlag = 0;
		if (v6) {
			if (plr[pnum]._pClass == PC_WARRIOR) {
				CreateMagicArmor(object[v5]._ox, object[v5]._oy, 9, ICURS_BREAST_PLATE, 0, 1);
				v8 = PS_WARR9;
			} else if (plr[pnum]._pClass == PC_ROGUE) {
				CreateMagicWeapon(object[v5]._ox, object[v5]._oy, 3, ICURS_LONG_WAR_BOW, 0, 1);
				v8 = PS_ROGUE9;
			} else if (plr[pnum]._pClass == PC_SORCERER) {
				CreateSpellBook(object[v5]._ox, object[v5]._oy, 3, 0, 1);
				v8 = PS_MAGE9;
			}
			PlaySfxLoc(v8, plr[myplr].WorldX, plr[myplr].WorldY);

			if (v4 == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, v3);
			return;
		}
	}
}
// 676190: using guessed type int deltaload;

void __fastcall OperateTrapLvr(int i)
{
	int v1; // ecx
	int v2; // eax
	int v3; // esi
	int v4; // edx
	int v5; // eax
	int v6; // eax

	v1 = i;
	v2 = object[v1]._oAnimFrame;
	v3 = nobjects;
	v4 = 0;
	if (v2 == 1) {
		object[v1]._oAnimFrame = 2;
		if (v3 > 0) {
			do {
				v5 = objectactive[v4];
				if (object[v5]._otype == object[v1]._oVar2 && object[v5]._oVar1 == object[v1]._oVar1) {
					object[v5]._oAnimFlag = 0;
					object[v5]._oVar2 = 1;
				}
				++v4;
			} while (v4 < v3);
		}
	} else {
		object[v1]._oAnimFrame = v2 - 1;
		if (v3 > 0) {
			do {
				v6 = objectactive[v4];
				if (object[v6]._otype == object[v1]._oVar2 && object[v6]._oVar1 == object[v1]._oVar1) {
					object[v6]._oVar2 = 0;
					if (object[v6]._oVar4)
						object[v6]._oAnimFlag = 1;
				}
				++v4;
			} while (v4 < v3);
		}
	}
}

void __fastcall OperateSarc(int pnum, int i, unsigned char sendmsg)
{
	if (object[i]._oSelFlag) {
		if (!deltaload)
			PlaySfxLoc(IS_SARC, object[i]._ox, object[i]._oy);
		object[i]._oSelFlag = 0;
		if (deltaload) {
			object[i]._oAnimFrame = object[i]._oAnimLen;
		} else {
			object[i]._oAnimFlag = 1;
			object[i]._oAnimDelay = 3;
			SetRndSeed(object[i]._oRndSeed);
			if (object[i]._oVar1 <= 2)
				CreateRndItem(object[i]._ox, object[i]._oy, 0, sendmsg, 0);
			if (object[i]._oVar1 >= 8)
				SpawnSkeleton(object[i]._oVar2, object[i]._ox, object[i]._oy);
			if (pnum == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
		}
	}
}
// 676190: using guessed type int deltaload;

void __fastcall OperateL2Door(int pnum, int i, unsigned char sendflag)
{
	int dpx, dpy;

	dpx = abs(object[i]._ox - plr[pnum].WorldX);
	dpy = abs(object[i]._oy - plr[pnum].WorldY);
	if (dpx == 1 && dpy <= 1 && object[i]._otype == OBJ_L2LDOOR)
		OperateL2LDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && object[i]._otype == OBJ_L2RDOOR)
		OperateL2RDoor(pnum, i, sendflag);
}

void __fastcall OperateL3Door(int pnum, int i, unsigned char sendflag)
{
	int dpx, dpy;

	dpx = abs(object[i]._ox - plr[pnum].WorldX);
	dpy = abs(object[i]._oy - plr[pnum].WorldY);
	if (dpx == 1 && dpy <= 1 && object[i]._otype == OBJ_L3RDOOR)
		OperateL3RDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && object[i]._otype == OBJ_L3LDOOR)
		OperateL3LDoor(pnum, i, sendflag);
}

void __fastcall OperatePedistal(int pnum, int i)
{
	int v2;            // esi
	int v3;            // edi
	unsigned char *v4; // edi
	int inv_item_num;  // [esp+8h] [ebp-4h]

	v2 = i;
	v3 = pnum;
	if (object[i]._oVar6 != 3) {
		if (PlrHasItem(pnum, 21, &inv_item_num)) {
			RemoveInvItem(v3, inv_item_num);
			++object[v2]._oAnimFrame;
			++object[v2]._oVar6;
		}
		if (object[v2]._oVar6 == 1) {
			if (!deltaload)
				PlaySfxLoc(LS_PUDDLE, object[v2]._ox, object[v2]._oy);
			ObjChangeMap(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
		}
		if (object[v2]._oVar6 == 2) {
			if (!deltaload)
				PlaySfxLoc(LS_PUDDLE, object[v2]._ox, object[v2]._oy);
			ObjChangeMap(setpc_x + 6, setpc_y + 3, setpc_x + setpc_w, setpc_y + 7);
		}
		if (object[v2]._oVar6 == 3) {
			if (!deltaload)
				PlaySfxLoc(LS_BLODSTAR, object[v2]._ox, object[v2]._oy);
			ObjChangeMap(object[v2]._oVar1, object[v2]._oVar2, object[v2]._oVar3, object[v2]._oVar4);
			v4 = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", 0);
			LoadMapObjs(v4, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(v4);
			CreateItem(7, 2 * setpc_x + 25, 2 * setpc_y + 19);
			object[v2]._oSelFlag = 0;
		}
	}
}
// 5CF334: using guessed type int setpc_w;
// 676190: using guessed type int deltaload;

void __fastcall TryDisarm(int pnum, int i)
{
	int v2;        // edi
	int v3;        // esi
	int v4;        // esi
	int v5;        // edi
	int v6;        // ebx
	int j;         // edx
	signed int v8; // edi
	int v9;        // eax
	int v10;       // ecx
	int v11;       // eax
	int v12;       // [esp+Ch] [ebp-4h]

	v2 = pnum;
	v3 = i;
	v12 = i;
	if (pnum == myplr)
		SetCursor_(CURSOR_HAND);
	v4 = v3;
	if (object[v4]._oTrapFlag) {
		v5 = 2 * plr[v2]._pDexterity - 5 * currlevel;
		if (random(154, 100) <= v5) {
			v6 = nobjects;
			for (j = 0; j < v6; ++j) {
				v8 = 0;
				v9 = objectactive[j];
				v10 = object[v9]._otype;
				if (v10 == OBJ_TRAPL)
					v8 = 1;
				if (v10 == OBJ_TRAPR)
					v8 = 1;
				if (v8 && dObject[object[v9]._oVar1][object[v9]._oVar2] - 1 == v12) {
					object[v9]._oVar4 = 1;
					object[v4]._oTrapFlag = 0;
				}
			}
			v11 = object[v4]._otype;
			if (v11 >= OBJ_TCHEST1 && v11 <= OBJ_TCHEST3)
				object[v4]._oTrapFlag = 0;
		}
	}
}

int __fastcall ItemMiscIdIdx(int imiscid)
{
	int i;

	for (i = 0; AllItemsList[i].iRnd == 0 || AllItemsList[i].iMiscId != imiscid; i++) {
	}

	return i;
}

void __fastcall OperateShrine(int pnum, int i, int sType)
{
	int v1;
	int v12;  // edx
	int v21;  // eax
	int v60;  // ebx
	int j;    // edi
	int v72;  // edi
	int v88;  // ebx
	int v107; // ST38_4
	int v108; // ST34_4
	int v133; // eax
	int xx, yy;
	int min, max;

	if (dropGoldFlag) {
		dropGoldFlag = 0;
		dropGoldValue = 0;
	}

	/// ASSERT: assert((DWORD)i < MAXOBJECTS);

	if (!object[i]._oSelFlag)
		return;

	SetRndSeed(object[i]._oRndSeed);
	object[i]._oSelFlag = 0;

	if (deltaload) {
		object[i]._oAnimFlag = 0;
		object[i]._oAnimFrame = object[i]._oAnimLen;
	} else {
		PlaySfxLoc(sType, object[i]._ox, object[i]._oy);
		object[i]._oAnimFlag = 1;
		object[i]._oAnimDelay = 1;
	}

	switch (object[i]._oVar1) {
	case SHRINE_MYSTERIOUS:
		if (deltaload || pnum != myplr)
			return;
		ModifyPlrStr(pnum, -1);
		ModifyPlrMag(pnum, -1);
		ModifyPlrDex(pnum, -1);
		ModifyPlrVit(pnum, -1);
		switch (random(0, 4)) {
		case 0:
			ModifyPlrStr(pnum, 6);
			break;
		case 1:
			ModifyPlrMag(pnum, 6);
			break;
		case 2:
			ModifyPlrDex(pnum, 6);
			break;
		case 3:
			ModifyPlrVit(pnum, 6);
			break;
		}
		CheckStats(pnum);
		InitDiabloMsg(EMSG_SHRINE_MYSTERIOUS);
		break;
	case SHRINE_HIDDEN:
		v12 = 0;
		if (deltaload || pnum != myplr)
			return;
		for (j = 0; j < 7; j++) {
			if (plr[pnum].InvBody[j]._itype != -1)
				v12++;
		}
		if (v12 > 0) {
			for (j = 0; j < 7; j++) {
				if (plr[pnum].InvBody[j]._itype != -1
				    && plr[pnum].InvBody[j]._iMaxDur != 255
				    && plr[pnum].InvBody[j]._iMaxDur) {
					plr[pnum].InvBody[j]._iDurability += 10;
					plr[pnum].InvBody[j]._iMaxDur += 10;
					if (plr[pnum].InvBody[j]._iDurability > plr[pnum].InvBody[j]._iMaxDur)
						plr[pnum].InvBody[j]._iDurability = plr[pnum].InvBody[j]._iMaxDur;
				}
			}
			v12 = 0;
			for (j = 0; j < 7; j++) {
				if (plr[pnum].InvBody[j]._itype != -1
				    && plr[pnum].InvBody[j]._iMaxDur != 255
				    && plr[pnum].InvBody[j]._iMaxDur)
					v12++;
			}
			if (v12 > 0) { // check
				do {
					v21 = random(0, 7);
				} while (plr[pnum].InvBody[v21]._itype == -1 || plr[pnum].InvBody[v21]._iMaxDur == 255 || !plr[pnum].InvBody[v21]._iMaxDur);

				plr[pnum].InvBody[v21]._iDurability -= 20;
				plr[pnum].InvBody[v21]._iMaxDur -= 20;
				if (plr[pnum].InvBody[v21]._iDurability <= 0)
					plr[pnum].InvBody[v21]._iDurability = 1;
				if (plr[pnum].InvBody[v21]._iMaxDur <= 0)
					plr[pnum].InvBody[v21]._iMaxDur = 1;
			}
		}
		InitDiabloMsg(EMSG_SHRINE_HIDDEN);
		break;
	case SHRINE_GLOOMY:
		if (deltaload)
			return;
		if (pnum == myplr) {
			if (plr[pnum].InvBody[INVLOC_HEAD]._itype != -1)
				plr[pnum].InvBody[INVLOC_HEAD]._iAC += 2;
			if (plr[pnum].InvBody[INVLOC_CHEST]._itype != -1)
				plr[pnum].InvBody[INVLOC_CHEST]._iAC += 2;
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != -1) {
				if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
					plr[pnum].InvBody[INVLOC_HAND_LEFT]._iAC += 2;
				} else {
					plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam--;
					if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam < plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMinDam)
						plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam = plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMinDam;
				}
			}
			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != -1) {
				if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
					plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iAC += 2;
				} else {
					plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam--;
					if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam < plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMinDam)
						plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam = plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMinDam;
				}
			}
			for (j = 0; j < plr[pnum]._pNumInv; j++) {
				if (plr[pnum].InvList[j]._itype > 0) {
					if (plr[pnum].InvList[j]._itype <= ITYPE_MACE || plr[pnum].InvList[j]._itype == ITYPE_STAFF) { // check
						plr[pnum].InvList[j]._iMaxDam--;
						if (plr[pnum].InvList[j]._iMaxDam < plr[pnum].InvList[j]._iMinDam)
							plr[pnum].InvList[j]._iMaxDam = plr[pnum].InvList[j]._iMinDam;
					} else if (plr[pnum].InvList[j]._itype <= 9) {
						plr[pnum].InvList[j]._iAC += 2;
					}
				}
			}
			InitDiabloMsg(EMSG_SHRINE_GLOOMY);
		}
		break;
	case SHRINE_WEIRD:
		if (deltaload)
			return;
		if (pnum == myplr) {
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != -1 && plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_SHIELD)
				plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam++;
			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != -1 && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_SHIELD)
				plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam++;
			for (j = 0; j < plr[pnum]._pNumInv; j++) {
				if (plr[pnum].InvList[j]._itype > 0 && (plr[pnum].InvList[j]._itype <= ITYPE_MACE || plr[pnum].InvList[j]._itype == ITYPE_STAFF))
					plr[pnum].InvList[j]._iMaxDam++;
			}
			InitDiabloMsg(EMSG_SHRINE_WEIRD);
		}
		break;
	case SHRINE_MAGICAL:
	case SHRINE_MAGICAL2:
		if (deltaload)
			return;
		AddMissile(
		    plr[pnum].WorldX,
		    plr[pnum].WorldY,
		    plr[pnum].WorldX,
		    plr[pnum].WorldY,
		    plr[pnum]._pdir,
		    MIS_MANASHIELD,
		    -1,
		    pnum,
		    0,
		    2 * leveltype);
		if (pnum != myplr)
			return;
		InitDiabloMsg(EMSG_SHRINE_MAGICAL);
		break;
	case SHRINE_STONE:
		if (deltaload)
			return;
		if (pnum == myplr) {
			for (j = 0; j < 7; j++) {
				if (plr[pnum].InvBody[j]._itype == ITYPE_STAFF)
					plr[pnum].InvBody[j]._iCharges = plr[pnum].InvBody[j]._iMaxCharges;
			}
			for (j = 0; j < plr[pnum]._pNumInv; j++) {
				if (plr[pnum].InvList[j]._itype == ITYPE_STAFF)
					plr[pnum].InvList[j]._iCharges = plr[pnum].InvList[j]._iMaxCharges;
			}
			for (j = 0; j < 8; j++) {
				if (plr[pnum].SpdList[j]._itype == ITYPE_STAFF)
					plr[pnum].SpdList[j]._iCharges = plr[pnum].SpdList[j]._iMaxCharges; // belt items don't have charges?
			}
			InitDiabloMsg(EMSG_SHRINE_STONE);
		}
		break;
	case SHRINE_RELIGIOUS:
		if (deltaload)
			return;
		if (pnum == myplr) {
			for (j = 0; j < 7; j++)
				plr[pnum].InvBody[j]._iDurability = plr[pnum].InvBody[j]._iMaxDur;
			for (j = 0; j < plr[pnum]._pNumInv; j++)
				plr[pnum].InvList[j]._iDurability = plr[pnum].InvList[j]._iMaxDur;
			for (j = 0; j < 8; j++)
				plr[pnum].SpdList[j]._iDurability = plr[pnum].SpdList[j]._iMaxDur; // belt items don't have durability?
			InitDiabloMsg(EMSG_SHRINE_RELIGIOUS);
		}
		break;
	case SHRINE_ENCHANTED:
		if (deltaload || pnum != myplr)
			return;
		v12 = 0;
		for (j = 1; j <= 37; j++) {
			if (plr[pnum]._pMemSpells & ((__int64)1 << (j - 1))) // j
				v12++;
		}
		if (v12 > 1) {
			for (j = 1; j <= 37; j++) {
				if (plr[pnum]._pMemSpells & ((__int64)1 << (j - 1))) { // j
					if (plr[pnum]._pSplLvl[j] < 15)
						plr[pnum]._pSplLvl[j]++;
				}
			}
			do {
				v60 = random(0, 37) + 1;
			} while (!(plr[pnum]._pMemSpells & ((__int64)1 << (v60 - 1))));
			if (plr[pnum]._pSplLvl[v60] < 2)
				plr[pnum]._pSplLvl[v60] = 0;
			else
				plr[pnum]._pSplLvl[v60] -= 2;
		}
		InitDiabloMsg(EMSG_SHRINE_ENCHANTED);
		break;
	case SHRINE_THAUMATURGIC:
		for (j = 0; j < nobjects; j++) {
			v1 = objectactive[j];
			/// ASSERT: assert((DWORD)v1 < MAXOBJECTS);
			if ((object[v1]._otype == OBJ_CHEST1
			        || object[v1]._otype == OBJ_CHEST2
			        || object[v1]._otype == OBJ_CHEST3)
			    && !object[v1]._oSelFlag) {
				object[v1]._oAnimFrame -= 2;
				object[v1]._oRndSeed = GetRndSeed();
				object[v1]._oSelFlag = 1;
			}
		}
		if (deltaload)
			return;
		if (pnum == myplr)
			InitDiabloMsg(EMSG_SHRINE_THAUMATURGIC);
		break;
	case SHRINE_FASCINATING:
		if (deltaload || pnum != myplr)
			return;
		plr[pnum]._pMemSpells |= (__int64)1 << (SPL_FIREBOLT - 1);
		if (plr[pnum]._pSplLvl[SPL_FIREBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_FIREBOLT]++;
		if (plr[pnum]._pSplLvl[SPL_FIREBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_FIREBOLT]++;
		v72 = plr[pnum]._pMaxManaBase / 10;
		min = plr[pnum]._pMana - plr[pnum]._pManaBase;
		max = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
		plr[pnum]._pManaBase -= v72;
		plr[pnum]._pMana -= v72;
		plr[pnum]._pMaxManaBase -= v72;
		plr[pnum]._pMaxMana -= v72;
		if (plr[pnum]._pMana >> 6 <= 0) {
			plr[pnum]._pMana = min;
			plr[pnum]._pManaBase = 0;
		}
		if (plr[pnum]._pMaxMana >> 6 <= 0) {
			plr[pnum]._pMaxMana = max;
			plr[pnum]._pMaxManaBase = 0;
		}
		InitDiabloMsg(EMSG_SHRINE_FASCINATING);
		break;
	case SHRINE_CRYPTIC:
		if (deltaload)
			return;
		AddMissile(
		    plr[pnum].WorldX,
		    plr[pnum].WorldY,
		    plr[pnum].WorldX,
		    plr[pnum].WorldY,
		    plr[pnum]._pdir,
		    MIS_NOVA,
		    -1,
		    pnum,
		    0,
		    2 * leveltype);
		if (pnum != myplr)
			return;
		plr[pnum]._pMana = plr[pnum]._pMaxMana;
		plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
		InitDiabloMsg(EMSG_SHRINE_CRYPTIC);
		break;
	case SHRINE_ELDRITCH: /// BUGFIX: change `plr[pnum].HoldItem` to use a temporary buffer to prevent deleting item in hand
		if (deltaload)
			return;
		if (pnum == myplr) {
			for (j = 0; j < plr[pnum]._pNumInv; j++) {
				if (!plr[pnum].InvList[j]._itype) {
					if (plr[pnum].InvList[j]._iMiscId == IMISC_HEAL
					    || plr[pnum].InvList[j]._iMiscId == IMISC_MANA) {
						SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_REJUV));
						GetPlrHandSeed(&plr[pnum].HoldItem);
						plr[pnum].HoldItem._iStatFlag = 1;
						qmemcpy(&plr[pnum].InvList[j], &plr[pnum].HoldItem, sizeof(ItemStruct));
					}
					if (plr[pnum].InvList[j]._iMiscId == IMISC_FULLHEAL
					    || plr[pnum].InvList[j]._iMiscId == IMISC_FULLMANA) {
						SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_FULLREJUV));
						GetPlrHandSeed(&plr[pnum].HoldItem);
						plr[pnum].HoldItem._iStatFlag = 1;
						qmemcpy(&plr[pnum].InvList[j], &plr[pnum].HoldItem, sizeof(ItemStruct));
					}
				}
			}
			for (j = 0; j < 8; j++) {
				if (!plr[pnum].SpdList[j]._itype) {
					if (plr[pnum].SpdList[j]._iMiscId == IMISC_HEAL
					    || plr[pnum].SpdList[j]._iMiscId == IMISC_MANA) {
						SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_REJUV));
						GetPlrHandSeed(&plr[pnum].HoldItem);
						plr[pnum].HoldItem._iStatFlag = 1;
						qmemcpy(&plr[pnum].SpdList[j], &plr[pnum].HoldItem, sizeof(ItemStruct));
					}
					if (plr[pnum].SpdList[j]._iMiscId == IMISC_FULLHEAL
					    || plr[pnum].SpdList[j]._iMiscId == IMISC_FULLMANA) {
						SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_FULLREJUV));
						GetPlrHandSeed(&plr[pnum].HoldItem);
						plr[pnum].HoldItem._iStatFlag = 1;
						qmemcpy(&plr[pnum].SpdList[j], &plr[pnum].HoldItem, sizeof(ItemStruct));
					}
				}
			}
			InitDiabloMsg(EMSG_SHRINE_ELDRITCH);
		}
		break;
	case SHRINE_EERIE:
		if (deltaload || pnum != myplr)
			return;
		ModifyPlrMag(pnum, 2);
		CheckStats(pnum);
		InitDiabloMsg(EMSG_SHRINE_EERIE);
		break;
	case SHRINE_DIVINE:
		if (deltaload || pnum != myplr)
			return;
		if (2 * currlevel >= 7) {
			CreateTypeItem(object[i]._ox, object[i]._oy, 0, ITYPE_MISC, IMISC_FULLREJUV, 0, 1);
			CreateTypeItem(object[i]._ox, object[i]._oy, 0, ITYPE_MISC, IMISC_FULLREJUV, 0, 1);
		} else {
			CreateTypeItem(object[i]._ox, object[i]._oy, 0, ITYPE_MISC, IMISC_FULLMANA, 0, 1);
			CreateTypeItem(object[i]._ox, object[i]._oy, 0, ITYPE_MISC, IMISC_FULLHEAL, 0, 1);
		}
		plr[pnum]._pMana = plr[pnum]._pMaxMana;
		plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
		plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
		plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
		InitDiabloMsg(EMSG_SHRINE_DIVINE);
		break;
	case SHRINE_HOLY:
		if (deltaload)
			return;
		v88 = 0;
		do {
			v88++;
			xx = random(159, MAXDUNX);
			yy = random(159, MAXDUNY);
		} while (v88 <= MAXDUNX * 112 && (nSolidTable[dPiece[xx][yy]] || dObject[xx][yy] || dMonster[xx][yy]));
		AddMissile(plr[pnum].WorldX, plr[pnum].WorldY, xx, yy, plr[pnum]._pdir, MIS_RNDTELEPORT, -1, pnum, 0, 2 * leveltype);
		if (pnum != myplr)
			return;
		InitDiabloMsg(EMSG_SHRINE_HOLY);
		break;
	case SHRINE_SACRED:
		if (deltaload || pnum != myplr)
			return;
		plr[pnum]._pMemSpells |= (__int64)1 << (SPL_CBOLT - 1);
		if (plr[pnum]._pSplLvl[SPL_CBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_CBOLT]++;
		if (plr[pnum]._pSplLvl[SPL_CBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_CBOLT]++;
		v72 = plr[pnum]._pMaxManaBase / 10;
		min = plr[pnum]._pMana - plr[pnum]._pManaBase;
		max = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
		plr[pnum]._pManaBase -= v72;
		plr[pnum]._pMana -= v72;
		plr[pnum]._pMaxManaBase -= v72;
		plr[pnum]._pMaxMana -= v72;
		if (plr[pnum]._pMana >> 6 <= 0) {
			plr[pnum]._pMana = min;
			plr[pnum]._pManaBase = 0;
		}
		if (plr[pnum]._pMaxMana >> 6 <= 0) {
			plr[pnum]._pMaxMana = max;
			plr[pnum]._pMaxManaBase = 0;
		}
		InitDiabloMsg(EMSG_SHRINE_SACRED);
		break;
	case SHRINE_SPIRITUAL:
		if (deltaload || pnum != myplr)
			return;
		for (j = 0; j < 40; j++) {
			if (!plr[pnum].InvGrid[j]) {
				v107 = 5 * leveltype + random(160, 10 * leveltype);
				v108 = plr[pnum]._pNumInv; // check
				qmemcpy(&plr[pnum].InvList[v108], &golditem, sizeof(ItemStruct));
				plr[pnum]._pNumInv++;
				plr[pnum].InvList[v108]._iSeed = GetRndSeed();
				plr[pnum].InvGrid[j] = plr[pnum]._pNumInv;
				plr[pnum].InvList[v108]._ivalue = v107;
				plr[pnum]._pGold += v107;
				SetGoldCurs(pnum, v108);
			}
		}
		InitDiabloMsg(EMSG_SHRINE_SPIRITUAL);
		break;
	case SHRINE_SPOOKY:
		if (deltaload)
			return;
		if (pnum == myplr) {
			InitDiabloMsg(EMSG_SHRINE_SPOOKY1);
		} else {
			InitDiabloMsg(EMSG_SHRINE_SPOOKY2);
			plr[myplr]._pHitPoints = plr[myplr]._pMaxHP;
			plr[myplr]._pHPBase = plr[myplr]._pMaxHPBase;
			plr[myplr]._pMana = plr[myplr]._pMaxMana;
			plr[myplr]._pManaBase = plr[myplr]._pMaxManaBase;
		}
		break;
	case SHRINE_ABANDONED:
		if (deltaload || pnum != myplr)
			return;
		ModifyPlrDex(pnum, 2);
		CheckStats(pnum);
		if (pnum == myplr)
			InitDiabloMsg(EMSG_SHRINE_ABANDONED);
		break;
	case SHRINE_CREEPY:
		if (deltaload || pnum != myplr)
			return;
		ModifyPlrStr(pnum, 2);
		CheckStats(pnum);
		if (pnum == myplr)
			InitDiabloMsg(EMSG_SHRINE_CREEPY);
		break;
	case SHRINE_QUIET:
		if (deltaload || pnum != myplr)
			return;
		ModifyPlrVit(pnum, 2);
		CheckStats(pnum);
		if (pnum == myplr)
			InitDiabloMsg(EMSG_SHRINE_QUIET);
		break;
	case SHRINE_SECLUDED:
		if (deltaload)
			return;
		if (pnum == myplr) {
			for (yy = 0; yy < DMAXY; yy++) {
				for (xx = 0; xx < DMAXX; xx++) {
					automapview[xx][yy] = 1;
				}
			}
			InitDiabloMsg(EMSG_SHRINE_SECLUDED);
		}
		break;
	case SHRINE_ORNATE:
		if (deltaload || pnum != myplr)
			return;
		plr[pnum]._pMemSpells |= (__int64)1 << (SPL_HBOLT - 1);
		if (plr[pnum]._pSplLvl[SPL_HBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_HBOLT]++;
		if (plr[pnum]._pSplLvl[SPL_HBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_HBOLT]++;
		v72 = plr[pnum]._pMaxManaBase / 10;
		min = plr[pnum]._pMana - plr[pnum]._pManaBase;
		max = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
		plr[pnum]._pManaBase -= v72;
		plr[pnum]._pMana -= v72;
		plr[pnum]._pMaxManaBase -= v72;
		plr[pnum]._pMaxMana -= v72;
		if (plr[pnum]._pMana >> 6 <= 0) {
			plr[pnum]._pMana = min;
			plr[pnum]._pManaBase = 0;
		}
		if (plr[pnum]._pMaxMana >> 6 <= 0) {
			plr[pnum]._pMaxMana = max;
			plr[pnum]._pMaxManaBase = 0;
		}
		InitDiabloMsg(EMSG_SHRINE_ORNATE);
		break;
	case SHRINE_GLIMMERING:
		if (deltaload || pnum != myplr)
			return;
		for (j = 0; j < 7; j++) {
			if (plr[pnum].InvBody[j]._iMagical && !plr[pnum].InvBody[j]._iIdentified)
				plr[pnum].InvBody[j]._iIdentified = 1;
		}
		for (j = 0; j < plr[pnum]._pNumInv; j++) {
			if (plr[pnum].InvList[j]._iMagical && !plr[pnum].InvList[j]._iIdentified)
				plr[pnum].InvList[j]._iIdentified = 1;
		}
		for (j = 0; j < 8; j++) {
			if (plr[pnum].SpdList[j]._iMagical && !plr[pnum].SpdList[j]._iIdentified)
				plr[pnum].SpdList[j]._iIdentified = 1; // belt items can't be magical?
		}
		InitDiabloMsg(EMSG_SHRINE_GLIMMERING);
		break;
	case SHRINE_TAINTED:
		if (deltaload)
			return;
		if (pnum == myplr) {
			InitDiabloMsg(EMSG_SHRINE_TAINTED1);
		} else {
			InitDiabloMsg(EMSG_SHRINE_TAINTED2);
			v133 = random(155, 4);
			ModifyPlrStr(myplr, v133 == 0 ? 1 : -1);
			ModifyPlrMag(myplr, v133 == 1 ? 1 : -1);
			ModifyPlrDex(myplr, v133 == 2 ? 1 : -1);
			ModifyPlrVit(myplr, v133 == 3 ? 1 : -1);
			CheckStats(myplr);
		}
		break;
	}

	CalcPlrInv(pnum, TRUE);
	drawpanflag = 255;

	if (pnum == myplr)
		NetSendCmdParam2(FALSE, CMD_PLROPOBJ, pnum, i);
}
// 4B84DC: using guessed type int dropGoldFlag;
// 52571C: using guessed type int drawpanflag;
// 676190: using guessed type int deltaload;

void __fastcall OperateSkelBook(int pnum, int i, unsigned char sendmsg)
{
	unsigned short v3; // di
	int v4;            // esi
	BOOLEAN v5;        // zf
	int v7;            // eax
	int v8;            // ecx
	int v9;            // edx
	int v10;           // [esp+Ch] [ebp-4h]

	v3 = i;
	v4 = i;
	v10 = pnum;
	if (object[i]._oSelFlag) {
		if (!deltaload)
			PlaySfxLoc(IS_ISCROL, object[v4]._ox, object[v4]._oy);
		object[v4]._oAnimFrame += 2;
		v5 = deltaload == 0;
		object[v4]._oSelFlag = 0;
		if (v5) {
			SetRndSeed(object[v4]._oRndSeed);
			v7 = random(161, 5);
			v8 = object[v4]._ox;
			v9 = object[v4]._oy;
			if (v7)
				CreateTypeItem(v8, v9, 0, ITYPE_MISC, 21, sendmsg, 0);
			else
				CreateTypeItem(v8, v9, 0, ITYPE_MISC, 24, sendmsg, 0);
			if (v10 == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, v3);
		}
	}
}
// 676190: using guessed type int deltaload;

void __fastcall OperateBookCase(int pnum, int i, unsigned char sendmsg)
{
	unsigned short v3; // di
	int v4;            // ebp
	int v5;            // esi
	BOOLEAN v6;        // zf
	//int v7; // eax

	v3 = i;
	v4 = pnum;
	v5 = i;
	if (object[i]._oSelFlag) {
		if (!deltaload)
			PlaySfxLoc(IS_ISCROL, object[v5]._ox, object[v5]._oy);
		object[v5]._oAnimFrame -= 2;
		v6 = deltaload == 0;
		object[v5]._oSelFlag = 0;
		if (v6) {
			SetRndSeed(object[v5]._oRndSeed);
			CreateTypeItem(object[v5]._ox, object[v5]._oy, 0, ITYPE_MISC, 24, sendmsg, 0);
			//_LOBYTE(v7) = QuestStatus(QTYPE_ZHAR);
			if (QuestStatus(QTYPE_ZHAR)
			    && monster[4].mName == UniqMonst[2].mName
			    && monster[4]._msquelch == -1
			    && monster[4]._mhitpoints) {
				monster[4].mtalkmsg = QUEST_ZHAR2;
				M_StartStand(0, monster[4]._mdir);
				_LOBYTE(monster[4]._mgoal) = MGOAL_SHOOT;
				monster[4]._mmode = MM_TALK;
			}
			if (v4 == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, v3);
		}
	}
}
// 676190: using guessed type int deltaload;

void __fastcall OperateDecap(int pnum, int i, unsigned char sendmsg)
{
	unsigned short v3; // bp
	int v4;            // esi
	int v5;            // edi
	int *v6;           // eax
	BOOLEAN v7;        // zf

	v3 = i;
	v4 = i;
	v5 = pnum;
	v6 = (int *)&object[i]._oSelFlag;
	if (*(_BYTE *)v6) {
		v7 = deltaload == 0;
		*(_BYTE *)v6 = 0;
		if (v7) {
			SetRndSeed(object[v4]._oRndSeed);
			CreateRndItem(object[v4]._ox, object[v4]._oy, 0, sendmsg, 0);
			if (v5 == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, v3);
		}
	}
}
// 676190: using guessed type int deltaload;

void __fastcall OperateArmorStand(int pnum, int i, unsigned char sendmsg)
{
	unsigned short v3; // di
	int v4;            // esi
	int *v5;           // eax
	BOOLEAN v6;        // zf
	unsigned char v8;  // al
	int v9;            // [esp-10h] [ebp-20h]
	int v10;           // [esp-8h] [ebp-18h]
	int v11;           // [esp+Ch] [ebp-4h]

	v3 = i;
	v4 = i;
	v11 = pnum;
	v5 = (int *)&object[i]._oSelFlag;
	if (*(_BYTE *)v5) {
		++object[v4]._oAnimFrame;
		v6 = deltaload == 0;
		*(_BYTE *)v5 = 0;
		if (v6) {
			SetRndSeed(object[v4]._oRndSeed);
			v8 = random(0, 2);
			if (currlevel > 5u) {
				if (currlevel >= 6u && currlevel <= 9u) {
					CreateTypeItem(object[v4]._ox, object[v4]._oy, v8, ITYPE_MARMOR, 0, sendmsg, 0);
					goto LABEL_15;
				}
				if (currlevel >= 0xAu && currlevel <= 0xCu) {
					CreateTypeItem(object[v4]._ox, object[v4]._oy, 0, ITYPE_HARMOR, 0, sendmsg, 0);
					goto LABEL_15;
				}
				if (currlevel < 0xDu || currlevel > 0x10u)
					goto LABEL_15;
				v10 = sendmsg;
				v9 = ITYPE_HARMOR;
			} else {
				v10 = sendmsg;
				v9 = ITYPE_LARMOR;
			}
			CreateTypeItem(object[v4]._ox, object[v4]._oy, 1u, v9, 0, v10, 0);
		LABEL_15:
			if (v11 == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, v3);
			return;
		}
	}
}
// 676190: using guessed type int deltaload;

int __fastcall FindValidShrine(int i)
{
	BOOLEAN done; // esi
	int rv;       // eax
	BOOLEAN v3;   // zf

	do {
		done = 0;
		do {
			rv = random(0, 26);
			if (currlevel >= shrinemin[rv] && currlevel <= shrinemax[rv] && rv != 8)
				done = 1;
		} while (!done);
		if (gbMaxPlayers == 1)
			v3 = shrineavail[rv] == 2;
		else
			v3 = shrineavail[rv] == 1;
	} while (v3);
	return rv;
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall OperateGoatShrine(int pnum, int i, int sType)
{
	int v3; // edi
	int v4; // ebx
	int v5; // esi

	v3 = i;
	v4 = pnum;
	v5 = i;
	SetRndSeed(object[i]._oRndSeed);
	object[v5]._oVar1 = FindValidShrine(v3);
	OperateShrine(v4, v3, sType);
	object[v5]._oAnimDelay = 2;
	drawpanflag = 255;
}
// 52571C: using guessed type int drawpanflag;

void __fastcall OperateCauldron(int pnum, int i, int sType)
{
	int v3; // edi
	int v4; // ebx
	int v5; // esi

	v3 = i;
	v4 = pnum;
	v5 = i;
	SetRndSeed(object[i]._oRndSeed);
	object[v5]._oVar1 = FindValidShrine(v3);
	OperateShrine(v4, v3, sType);
	object[v5]._oAnimFlag = 0;
	object[v5]._oAnimFrame = 3;
	drawpanflag = 255;
}
// 52571C: using guessed type int drawpanflag;

BOOLEAN __fastcall OperateFountains(int pnum, int i)
{
	unsigned short v2; // bx
	int v3;            // esi
	int v4;            // edi
	BOOLEAN v5;        // bp
	signed int v7;     // ebx
	int v8;            // ebp
	int v10;           // eax
	int v11;           // esi
	int v12;           // eax
	int v13;           // eax
	int v14;           // edi
	int v15;           // edx
	int v16;           // edx
	int v17;           // ecx
	int *v18;          // eax
	int v19;           // ecx
	int v20;           // edi
	int v21;           // edx
	int v22;           // ecx
	int v23;           // [esp-4h] [ebp-20h]
	signed int v24;    // [esp+10h] [ebp-Ch]
	signed int v25;    // [esp+14h] [ebp-8h]
	short param1;      // [esp+18h] [ebp-4h]

	v2 = i;
	v3 = i;
	v4 = pnum;
	param1 = i;
	v5 = 0;
	SetRndSeed(object[i]._oRndSeed);
	switch (object[v3]._otype) {
	case OBJ_BLOODFTN:
		if (!deltaload && v4 == myplr) {
			v20 = v4;
			v23 = object[v3]._oy;
			v15 = object[v3]._ox;
			if (plr[v20]._pHitPoints < plr[v20]._pMaxHP) {
				PlaySfxLoc(LS_FOUNTAIN, v15, v23);
				plr[v20]._pHitPoints += 64;
				v21 = plr[v20]._pHitPoints;
				v22 = plr[v20]._pMaxHP;
				v18 = &plr[v20]._pHPBase;
				*v18 += 64;
				if (v21 <= v22)
					goto LABEL_39;
				plr[v20]._pHitPoints = v22;
				v19 = plr[v20]._pMaxHPBase;
				goto LABEL_38;
			}
		LABEL_45:
			PlaySfxLoc(LS_FOUNTAIN, v15, v23);
			break;
		}
		return 0;
	case OBJ_PURIFYINGFTN:
		if (!deltaload && v4 == myplr) {
			v14 = v4;
			v23 = object[v3]._oy;
			v15 = object[v3]._ox;
			if (plr[v14]._pMana < plr[v14]._pMaxMana) {
				PlaySfxLoc(LS_FOUNTAIN, v15, v23);
				plr[v14]._pMana += 64;
				v16 = plr[v14]._pMana;
				v17 = plr[v14]._pMaxMana;
				v18 = &plr[v14]._pManaBase;
				*v18 += 64;
				if (v16 <= v17) {
				LABEL_39:
					v5 = 1;
					break;
				}
				plr[v14]._pMana = v17;
				v19 = plr[v14]._pMaxManaBase;
			LABEL_38:
				*v18 = v19;
				goto LABEL_39;
			}
			goto LABEL_45;
		}
		return 0;
	case OBJ_MURKYFTN:
		if (object[v3]._oSelFlag) {
			if (!deltaload)
				PlaySfxLoc(LS_FOUNTAIN, object[v3]._ox, object[v3]._oy);
			object[v3]._oSelFlag = 0;
			if (deltaload)
				return 0;
			AddMissile(
			    plr[v4].WorldX,
			    plr[v4].WorldY,
			    plr[v4].WorldX,
			    plr[v4].WorldY,
			    plr[v4]._pdir,
			    MIS_INFRA,
			    -1,
			    v4,
			    0,
			    2 * leveltype);
			v5 = 1;
			if (v4 == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, v2);
		}
		break;
	default:
		if (object[v3]._otype == OBJ_TEARFTN && object[v3]._oSelFlag) {
			v7 = -1;
			v8 = -1;
			v25 = 0;
			v24 = 0;
			if (!deltaload)
				PlaySfxLoc(LS_FOUNTAIN, object[v3]._ox, object[v3]._oy);
			object[v3]._oSelFlag = 0;
			if (deltaload || v4 != myplr)
				return 0;
			do {
				v10 = random(0, 4);
				v11 = v10;
				if (v10 != v7) {
					if (v10) {
						v12 = v10 - 1;
						if (v12) {
							v13 = v12 - 1;
							if (v13) {
								if (v13 == 1)
									ModifyPlrVit(v4, v8);
							} else {
								ModifyPlrDex(v4, v8);
							}
						} else {
							ModifyPlrMag(v4, v8);
						}
					} else {
						ModifyPlrStr(v4, v8);
					}
					v7 = v11;
					v8 = 1;
					++v24;
				}
				if (v24 > 1)
					v25 = 1;
			} while (!v25);
			CheckStats(v4);
			v5 = 1;
			if (v4 == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, param1);
		}
		break;
	}
	drawpanflag = 255;
	return v5;
}
// 52571C: using guessed type int drawpanflag;
// 676190: using guessed type int deltaload;

void __fastcall OperateWeaponRack(int pnum, int i, unsigned char sendmsg)
{
	unsigned short v3; // di
	int v4;            // esi
	int v6;            // eax
	int v7;            // eax
	int v8;            // eax
	int v9;            // eax
	BOOLEAN v10;       // zf
	int v11;           // ecx
	int v12;           // edx
	signed int v13;    // [esp-4h] [ebp-14h]
	int v14;           // [esp+Ch] [ebp-4h]

	v3 = i;
	v4 = i;
	v14 = pnum;
	if (!object[i]._oSelFlag)
		return;
	SetRndSeed(object[v4]._oRndSeed);
	v6 = random(0, 4);
	if (v6) {
		v7 = v6 - 1;
		if (!v7) {
			v13 = ITYPE_AXE;
			goto LABEL_7;
		}
		v8 = v7 - 1;
		if (!v8) {
			v13 = ITYPE_BOW;
			goto LABEL_7;
		}
		if (v8 == 1) {
			v13 = ITYPE_MACE;
		LABEL_7:
			v9 = v13;
			goto LABEL_12;
		}
		v9 = sendmsg;
	} else {
		v9 = ITYPE_SWORD;
	}
LABEL_12:
	++object[v4]._oAnimFrame;
	v10 = deltaload == 0;
	object[v4]._oSelFlag = 0;
	if (v10) {
		v11 = object[v4]._ox;
		v12 = object[v4]._oy;
		if (leveltype <= 1u)
			CreateTypeItem(v11, v12, 0, v9, 0, sendmsg, 0);
		else
			CreateTypeItem(v11, v12, 1u, v9, 0, sendmsg, 0);
		if (v14 == myplr)
			NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, v3);
	}
}
// 676190: using guessed type int deltaload;

void __fastcall OperateStoryBook(int pnum, int i)
{
	unsigned short v2; // di
	int v3;            // esi
	int v4;            // ST04_4
	int v5;            // edx

	v2 = i;
	v3 = i;
	if (object[i]._oSelFlag && !deltaload && !qtextflag && pnum == myplr) {
		v4 = object[v3]._oy;
		v5 = object[v3]._ox;
		object[v3]._oAnimFrame = object[v3]._oVar4;
		PlaySfxLoc(IS_ISCROL, v5, v4);
		InitQTextMsg(object[v3]._oVar2);
		NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, v2);
	}
}
// 646D00: using guessed type char qtextflag;
// 676190: using guessed type int deltaload;

void __fastcall OperateLazStand(int pnum, int i)
{
	int v2; // eax
	int v3; // edx
	int xx; // [esp+4h] [ebp-8h]
	int yy; // [esp+8h] [ebp-4h]

	v2 = i;
	if (object[i]._oSelFlag && !deltaload && !qtextflag && pnum == myplr) {
		v3 = object[v2]._oy;
		++object[v2]._oAnimFrame;
		object[v2]._oSelFlag = 0;
		GetSuperItemLoc(object[v2]._ox, v3, &xx, &yy);
		SpawnQuestItem(33, xx, yy, 0, 0);
	}
}
// 646D00: using guessed type char qtextflag;
// 676190: using guessed type int deltaload;

void __fastcall OperateObject(int pnum, int i, BOOL TeleFlag)
{
	int v3;           // esi
	int v4;           // edi
	ObjectStruct *v5; // ebx
	int v6;           // ecx
	BOOLEAN sendmsg;  // [esp+Ch] [ebp-4h]

	v3 = pnum;
	v4 = i;
	sendmsg = pnum == myplr;
	v5 = &object[i];
	v6 = v5->_otype;
	switch (v5->_otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		if (TeleFlag) {
			if (v6 == OBJ_L1LDOOR)
				OperateL1LDoor(v3, i, OBJ_L1LDOOR);
			if (v5->_otype == OBJ_L1RDOOR)
				OperateL1RDoor(v3, v4, 1u);
		} else if (v3 == myplr) {
			OperateL1Door(v3, i, 1u);
		}
		break;
	case OBJ_LEVER:
	case OBJ_SWITCHSKL:
		OperateLever(v3, i);
		break;
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		OperateChest(v3, i, sendmsg);
		break;
	case OBJ_BOOK2L:
		OperateBook(v3, i);
		break;
	case OBJ_BOOK2R:
		OperateSChambBk(v3, i);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		if (TeleFlag) {
			if (v6 == OBJ_L2LDOOR)
				OperateL2LDoor(v3, i, 1u);
			if (v5->_otype == OBJ_L2RDOOR)
				OperateL2RDoor(v3, v4, 1u);
		} else if (v3 == myplr) {
			OperateL2Door(v3, i, 1u);
		}
		break;
	case OBJ_SARC:
		OperateSarc(v3, i, sendmsg);
		break;
	case OBJ_FLAMELVR:
		OperateTrapLvr(i);
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		OperateShrine(v3, i, IS_MAGIC);
		break;
	case OBJ_SKELBOOK:
	case OBJ_BOOKSTAND:
		OperateSkelBook(v3, i, sendmsg);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		OperateBookCase(v3, i, sendmsg);
		break;
	case OBJ_BLOODFTN:
	case OBJ_PURIFYINGFTN:
	case OBJ_MURKYFTN:
	case OBJ_TEARFTN:
		OperateFountains(v3, i);
		break;
	case OBJ_DECAP:
		OperateDecap(v3, i, sendmsg);
		break;
	case OBJ_BLINDBOOK:
	case OBJ_BLOODBOOK:
	case OBJ_STEELTOME:
		OperateBookLever(v3, i);
		break;
	case OBJ_PEDISTAL:
		OperatePedistal(v3, i);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (TeleFlag) {
			if (v6 == OBJ_L3LDOOR)
				OperateL3LDoor(v3, i, 1u);
			if (v5->_otype == OBJ_L3RDOOR)
				OperateL3RDoor(v3, v4, 1u);
		} else if (v3 == myplr) {
			OperateL3Door(v3, i, 1u);
		}
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		OperateArmorStand(v3, i, sendmsg);
		break;
	case OBJ_GOATSHRINE:
		OperateGoatShrine(v3, i, LS_GSHRINE);
		break;
	case OBJ_CAULDRON:
		OperateCauldron(v3, i, LS_CALDRON);
		break;
	case OBJ_STORYBOOK:
		OperateStoryBook(v3, i);
		break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK:
		OperateWeaponRack(v3, i, sendmsg);
		break;
	case OBJ_MUSHPATCH:
		OperateMushPatch(v3, i);
		break;
	case OBJ_LAZSTAND:
		OperateLazStand(v3, i);
		break;
	case OBJ_SLAINHERO:
		OperateSlainHero(v3, i, sendmsg);
		break;
	case OBJ_SIGNCHEST:
		OperateInnSignChest(v3, i);
		break;
	default:
		return;
	}
}

void __fastcall SyncOpL1Door(int pnum, int cmd, int i)
{
	signed int v3;    // eax
	ObjectStruct *v4; // esi

	if (pnum != myplr) {
		v3 = 0;
		if (cmd == 43) {
			if (object[i]._oVar4)
				return;
			v3 = 1;
		}
		if (cmd == 44 && object[i]._oVar4 == 1)
			v3 = 1;
		if (v3) {
			v4 = &object[i];
			if (v4->_otype == 1)
				OperateL1LDoor(-1, i, 0);
			if (v4->_otype == OBJ_L1RDOOR)
				OperateL1RDoor(-1, i, 0);
		}
	}
}

void __fastcall SyncOpL2Door(int pnum, int cmd, int i)
{
	signed int v3;    // eax
	ObjectStruct *v4; // esi

	if (pnum != myplr) {
		v3 = 0;
		if (cmd == 43) {
			if (object[i]._oVar4)
				return;
			v3 = 1;
		}
		if (cmd == 44 && object[i]._oVar4 == 1)
			v3 = 1;
		if (v3) {
			v4 = &object[i];
			if (v4->_otype == OBJ_L2LDOOR)
				OperateL2LDoor(-1, i, 0);
			if (v4->_otype == OBJ_L2RDOOR)
				OperateL2RDoor(-1, i, 0);
		}
	}
}

void __fastcall SyncOpL3Door(int pnum, int cmd, int i)
{
	signed int v3;    // eax
	ObjectStruct *v4; // esi

	if (pnum != myplr) {
		v3 = 0;
		if (cmd == 43) {
			if (object[i]._oVar4)
				return;
			v3 = 1;
		}
		if (cmd == 44 && object[i]._oVar4 == 1)
			v3 = 1;
		if (v3) {
			v4 = &object[i];
			if (v4->_otype == OBJ_L3LDOOR)
				OperateL3LDoor(-1, i, 0);
			if (v4->_otype == OBJ_L3RDOOR)
				OperateL3RDoor(-1, i, 0);
		}
	}
}

void __fastcall SyncOpObject(int pnum, int cmd, int i)
{
	switch (object[i]._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		SyncOpL1Door(pnum, cmd, i);
		break;
	case OBJ_LEVER:
	case OBJ_SWITCHSKL:
		OperateLever(pnum, i);
		break;
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		OperateChest(pnum, i, 0);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		SyncOpL2Door(pnum, cmd, i);
		break;
	case OBJ_SARC:
		OperateSarc(pnum, i, 0);
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		OperateShrine(pnum, i, IS_MAGIC);
		break;
	case OBJ_SKELBOOK:
	case OBJ_BOOKSTAND:
		OperateSkelBook(pnum, i, 0);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		OperateBookCase(pnum, i, 0);
		break;
	case OBJ_DECAP:
		OperateDecap(pnum, i, 0);
		break;
	case OBJ_BLINDBOOK:
	case OBJ_BLOODBOOK:
	case OBJ_STEELTOME:
		OperateBookLever(pnum, i);
		break;
	case OBJ_PEDISTAL:
		OperatePedistal(pnum, i);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		SyncOpL3Door(pnum, cmd, i);
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		OperateArmorStand(pnum, i, 0);
		break;
	case OBJ_GOATSHRINE:
		OperateGoatShrine(pnum, i, LS_GSHRINE);
		break;
	case OBJ_CAULDRON:
		OperateCauldron(pnum, i, LS_CALDRON);
		break;
	case OBJ_MURKYFTN:
	case OBJ_TEARFTN:
		OperateFountains(pnum, i);
		break;
	case OBJ_STORYBOOK:
		OperateStoryBook(pnum, i);
		break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK:
		OperateWeaponRack(pnum, i, 0);
		break;
	case OBJ_MUSHPATCH:
		OperateMushPatch(pnum, i);
		break;
	case OBJ_SLAINHERO:
		OperateSlainHero(pnum, i, 0);
		break;
	case OBJ_SIGNCHEST:
		OperateInnSignChest(pnum, i);
		break;
	default:
		return;
	}
}

void __fastcall BreakCrux(int i)
{
	int v1;        // esi
	int v2;        // edi
	int v3;        // edx
	signed int v4; // eax
	int v5;        // ecx
	int v6;        // ebx

	v1 = i;
	v2 = nobjects;
	object[v1]._oBreak = -1;
	object[v1]._oSelFlag = 0;
	v3 = 0;
	v4 = 1;
	object[v1]._oAnimFlag = 1;
	object[v1]._oAnimFrame = 1;
	object[v1]._oAnimDelay = 1;
	object[v1]._oSolidFlag = TRUE;
	object[v1]._oMissFlag = TRUE;
	if (v2 <= 0)
		goto LABEL_15;
	do {
		v5 = objectactive[v3];
		v6 = object[v5]._otype;
		if ((v6 == OBJ_CRUX1 || v6 == OBJ_CRUX2 || v6 == OBJ_CRUX3)
		    && object[v1]._oVar8 == object[v5]._oVar8
		    && object[v5]._oBreak != -1) {
			v4 = 0;
		}
		++v3;
	} while (v3 < v2);
	if (v4) {
	LABEL_15:
		if (!deltaload)
			PlaySfxLoc(IS_LEVER, object[v1]._ox, object[v1]._oy);
		ObjChangeMap(object[v1]._oVar1, object[v1]._oVar2, object[v1]._oVar3, object[v1]._oVar4);
	}
}
// 676190: using guessed type int deltaload;

void __fastcall BreakBarrel(int pnum, int i, int dam, unsigned char forcebreak, int sendmsg)
{
	int v5;          // esi
	BOOLEAN v6;      // zf
	int v7;          // eax
	int v8;          // edx
	int v9;          // eax
	int v10;         // eax
	int v11;         // eax
	char v12;        // al
	char v13;        // al
	int v14;         // edx
	int v15;         // [esp-4h] [ebp-24h]
	short param2;    // [esp+Ch] [ebp-14h]
	int param1;      // [esp+10h] [ebp-10h]
	int v18;         // [esp+14h] [ebp-Ch]
	int *v19;        // [esp+18h] [ebp-8h]
	int v20;         // [esp+1Ch] [ebp-4h]
	int forcebreaka; // [esp+2Ch] [ebp+Ch]

	param2 = i;
	v5 = i;
	param1 = pnum;
	if (object[i]._oSelFlag) {
		if (forcebreak) {
			object[v5]._oVar1 = 0;
		} else {
			object[v5]._oVar1 -= dam;
			if (pnum != myplr && object[v5]._oVar1 <= 0)
				object[v5]._oVar1 = 1;
		}
		if (object[v5]._oVar1 <= 0) {
			object[v5]._oBreak = -1;
			v6 = deltaload == 0;
			object[v5]._oVar1 = 0;
			object[v5]._oAnimFlag = 1;
			object[v5]._oAnimFrame = 1;
			object[v5]._oAnimDelay = 1;
			object[v5]._oSolidFlag = FALSE;
			object[v5]._oMissFlag = TRUE;
			object[v5]._oSelFlag = 0;
			object[v5]._oPreFlag = 1;
			if (v6) {
				v8 = object[v5]._ox;
				v15 = object[v5]._oy;
				if (object[v5]._otype == OBJ_BARRELEX) {
					PlaySfxLoc(IS_BARLFIRE, v8, v15);
					v9 = object[v5]._oy;
					v20 = v9 - 1;
					if (v9 - 1 <= v9 + 1) {
						do {
							v10 = object[v5]._ox;
							v18 = v10 - 1;
							if (v10 - 1 <= v10 + 1) {
								forcebreaka = 112 * (v10 - 1) + v20;
								v19 = (int *)((char *)dMonster + 4 * forcebreaka);
								do {
									v11 = *v19;
									if (*v19 > 0)
										MonsterTrapHit(v11 - 1, 1, 4, 0, 1, 0);
									v12 = dPlayer[0][forcebreaka];
									if (v12 > 0)
										PlayerMHit(v12 - 1, -1, 0, 8, 16, 1, 0, 0);
									v13 = dObject[0][forcebreaka];
									if (v13 > 0) {
										v14 = v13 - 1;
										if (object[v14]._otype == OBJ_BARRELEX && object[v14]._oBreak != -1)
											BreakBarrel(param1, v14, dam, 1u, sendmsg);
									}
									++v18;
									v19 += 112;
									forcebreaka += 112;
								} while (v18 <= object[v5]._ox + 1);
							}
							++v20;
						} while (v20 <= object[v5]._oy + 1);
					}
				} else {
					PlaySfxLoc(IS_BARREL, v8, v15);
					SetRndSeed(object[v5]._oRndSeed);
					if (object[v5]._oVar2 <= 1) {
						if (object[v5]._oVar3)
							CreateRndItem(object[v5]._ox, object[v5]._oy, 0, sendmsg, 0);
						else
							CreateRndUseful(param1, object[v5]._ox, object[v5]._oy, sendmsg);
					}
					if (object[v5]._oVar2 >= 8)
						SpawnSkeleton(object[v5]._oVar4, object[v5]._ox, object[v5]._oy);
				}
				if (param1 == myplr)
					NetSendCmdParam2(FALSE, CMD_BREAKOBJ, param1, param2);
			} else {
				v7 = object[v5]._oAnimLen;
				object[v5]._oAnimCnt = 0;
				object[v5]._oAnimFrame = v7;
				object[v5]._oAnimDelay = 1000;
			}
		} else if (!deltaload) {
			PlaySfxLoc(IS_IBOW, object[v5]._ox, object[v5]._oy);
		}
	}
}
// 676190: using guessed type int deltaload;

void __fastcall BreakObject(int pnum, int oi)
{
	int v2; // ebx
	int v3; // ebp
	int v4; // esi
	int v5; // edi
	int v6; // ecx
	int v7; // ecx
	int v8; // eax

	v2 = pnum;
	v3 = oi;
	if (pnum == -1) {
		v7 = 10;
	} else {
		v4 = pnum;
		v5 = plr[v2]._pIMinDam;
		v6 = v5 + random(163, plr[v2]._pIMaxDam - v5 + 1);
		v7 = plr[v4]._pIBonusDamMod + plr[v4]._pDamageMod + v6 * plr[v4]._pIBonusDam / 100 + v6;
	}
	v8 = object[v3]._otype;
	if (v8 >= OBJ_CRUX1) {
		if (v8 <= OBJ_CRUX3) {
			BreakCrux(v3);
		} else if (v8 > OBJ_WEAPRACK && v8 <= OBJ_BARRELEX) {
			BreakBarrel(v2, v3, v7, 0, 1);
		}
	}
}

void __fastcall SyncBreakObj(int pnum, int oi)
{
	int v2; // eax

	v2 = object[oi]._otype;
	if (v2 >= OBJ_BARREL && v2 <= OBJ_BARRELEX)
		BreakBarrel(pnum, oi, 0, 1u, 0);
}

void __fastcall SyncL1Doors(int i)
{
	int v1;     // ebx
	int v2;     // eax
	int v3;     // esi
	int v4;     // edi
	BOOLEAN v5; // zf

	v1 = i;
	v2 = i;
	if (object[i]._oVar4) {
		v3 = object[v2]._oy;
		v4 = object[v2]._ox;
		v5 = object[v2]._otype == 1;
		object[v2]._oMissFlag = TRUE;
		object[v2]._oSelFlag = 2;
		if (v5) {
			if (object[v2]._oVar1 == 214)
				ObjSetMicro(v4, v3, 408);
			else
				ObjSetMicro(v4, v3, 393);
			dArch[v4][v3] = 7;
			objects_set_door_piece(v4 - 1, v3--);
		} else {
			ObjSetMicro(v4, v3, 395);
			dArch[v4][v3] = 8;
			objects_set_door_piece(v4--, v3 - 1);
		}
		DoorSet(v1, v4, v3);
	} else {
		object[v2]._oMissFlag = FALSE;
	}
}

void __fastcall SyncCrux(int i)
{
	signed int v1; // ebx
	int v2;        // edx
	int v3;        // eax
	int v4;        // esi

	v1 = 1;
	v2 = 0;
	if (nobjects <= 0)
		goto LABEL_13;
	do {
		v3 = objectactive[v2];
		v4 = object[v3]._otype;
		if ((v4 == OBJ_CRUX1 || v4 == OBJ_CRUX2 || v4 == OBJ_CRUX3)
		    && object[i]._oVar8 == object[v3]._oVar8
		    && object[v3]._oBreak != -1) {
			v1 = 0;
		}
		++v2;
	} while (v2 < nobjects);
	if (v1)
	LABEL_13:
		ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
}

void __fastcall SyncLever(int i)
{
	int v1; // ecx

	v1 = i;
	if (!object[v1]._oSelFlag)
		ObjChangeMap(object[v1]._oVar1, object[v1]._oVar2, object[v1]._oVar3, object[v1]._oVar4);
}

void __fastcall SyncQSTLever(int i)
{
	int v1;  // esi
	int v2;  // edx
	int v3;  // ecx
	int v4;  // ST04_4
	char v5; // bl
	int v6;  // ST00_4

	v1 = i;
	if (object[i]._oAnimFrame == object[i]._oVar6) {
		ObjChangeMapResync(object[v1]._oVar1, object[v1]._oVar2, object[v1]._oVar3, object[v1]._oVar4);
		if (object[v1]._otype == OBJ_BLINDBOOK) {
			v2 = object[v1]._oVar2;
			v3 = object[v1]._oVar1;
			v4 = object[v1]._oVar4;
			v5 = TransVal;
			v6 = object[v1]._oVar3;
			TransVal = 9;
			DRLG_MRectTrans(v3, v2, v6, v4);
			TransVal = v5;
		}
	}
}
// 5A5590: using guessed type char TransVal;

void __fastcall SyncPedistal(int i)
{
	int v1;            // esi
	unsigned char *v2; // esi

	v1 = i;
	if (object[i]._oVar6 == 1)
		ObjChangeMapResync(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
	if (object[v1]._oVar6 == 2) {
		ObjChangeMapResync(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
		ObjChangeMapResync(setpc_x + 6, setpc_y + 3, setpc_x + setpc_w, setpc_y + 7);
	}
	if (object[v1]._oVar6 == 3) {
		ObjChangeMapResync(object[v1]._oVar1, object[v1]._oVar2, object[v1]._oVar3, object[v1]._oVar4);
		v2 = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", 0);
		LoadMapObjs(v2, 2 * setpc_x, 2 * setpc_y);
		mem_free_dbg(v2);
	}
}
// 5CF334: using guessed type int setpc_w;

void __fastcall SyncL2Doors(int i)
{
	int v1; // eax
	int v2; // esi
	int v3; // ecx
	int v4; // edx
	int v5; // eax

	v1 = i;
	v2 = object[i]._oVar4;
	if (v2)
		object[v1]._oMissFlag = TRUE;
	else
		object[v1]._oMissFlag = FALSE;
	v3 = object[v1]._ox;
	v4 = object[v1]._oy;
	object[v1]._oSelFlag = 2;
	v5 = object[v1]._otype;
	if (v5 != OBJ_L2LDOOR)
		goto LABEL_18;
	if (!v2) {
		ObjSetMicro(v3, v4, 538);
		return;
	}
	if (v2 != 1 && v2 != 2) {
	LABEL_18:
		if (v5 == OBJ_L2RDOOR) {
			if (v2) {
				if (v2 == 1 || v2 == 2)
					ObjSetMicro(v3, v4, 17);
			} else {
				ObjSetMicro(v3, v4, 540);
			}
		}
	} else {
		ObjSetMicro(v3, v4, 13);
	}
}

void __fastcall SyncL3Doors(int i)
{
	int v1; // eax
	int v2; // esi
	int v3; // ecx
	int v4; // edx
	int v5; // ebx
	int v6; // eax

	v1 = i;
	v2 = object[i]._otype;
	v3 = object[i]._ox;
	v4 = object[v1]._oy;
	object[v1]._oMissFlag = TRUE;
	object[v1]._oSelFlag = 2;
	if (v2 != OBJ_L3LDOOR)
		goto LABEL_15;
	if (!object[v1]._oVar4) {
		ObjSetMicro(v3, v4, 531);
		return;
	}
	v5 = object[v1]._oVar4;
	if (v5 != 1 && v5 != 2) {
	LABEL_15:
		if (v2 == OBJ_L3RDOOR) {
			if (object[v1]._oVar4) {
				v6 = object[v1]._oVar4;
				if (v6 == 1 || v6 == 2)
					ObjSetMicro(v3, v4, 541);
			} else {
				ObjSetMicro(v3, v4, 534);
			}
		}
	} else {
		ObjSetMicro(v3, v4, 538);
	}
}

void __fastcall SyncObjectAnim(int o)
{
	int v1; // edx
	int v2; // ebx
	int v3; // esi

	v1 = object[o]._otype;
	v2 = ObjFileList[0];
	v3 = 0;
	while (v2 != (char)AllObjects[object[o]._otype].ofindex)
		v2 = ObjFileList[v3++ + 1];
	object[o]._oAnimData = pObjCels[v3];
	if (v1 <= OBJ_BOOK2R) {
		if (v1 != OBJ_BOOK2R) {
			if (v1 > OBJ_L1LIGHT) {
				if (v1 <= OBJ_L1RDOOR) {
					SyncL1Doors(o);
				} else {
					if (v1 == OBJ_LEVER)
						goto LABEL_30;
					if (v1 > OBJ_SKSTICK5) {
						if (v1 <= OBJ_CRUX3) {
							SyncCrux(o);
							return;
						}
						if (v1 == OBJ_BOOK2L || v1 == OBJ_SWITCHSKL)
						LABEL_30:
							SyncLever(o);
					}
				}
			}
			return;
		}
	LABEL_24:
		SyncQSTLever(o);
		return;
	}
	if (v1 >= OBJ_L2LDOOR) {
		if (v1 <= OBJ_L2RDOOR) {
			SyncL2Doors(o);
			return;
		}
		if (v1 == OBJ_BLINDBOOK)
			goto LABEL_24;
		if (v1 == OBJ_PEDISTAL) {
			SyncPedistal(o);
			return;
		}
		if (v1 > OBJ_PEDISTAL) {
			if (v1 <= OBJ_L3RDOOR) {
				SyncL3Doors(o);
				return;
			}
			if (v1 == OBJ_STEELTOME)
				goto LABEL_24;
		}
	}
}

void __fastcall GetObjectStr(int i)
{
	int v1; // edi

	v1 = i;
	switch (object[i]._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (object[v1]._oVar4 == 1)
			strcpy(infostr, "Open Door");
		if (!object[v1]._oVar4)
			strcpy(infostr, "Closed Door");
		if (object[v1]._oVar4 == 2)
			strcpy(infostr, "Blocked Door");
		break;
	case OBJ_LEVER:
	case OBJ_FLAMELVR:
		strcpy(infostr, "Lever");
		break;
	case OBJ_CHEST1:
	case OBJ_TCHEST1:
		strcpy(infostr, "Small Chest");
		break;
	case OBJ_CHEST2:
	case OBJ_TCHEST2:
		strcpy(infostr, "Chest");
		break;
	case OBJ_CHEST3:
	case OBJ_TCHEST3:
	case OBJ_SIGNCHEST:
		strcpy(infostr, "Large Chest");
		break;
	case OBJ_CRUX1:
	case OBJ_CRUX2:
	case OBJ_CRUX3:
		strcpy(infostr, "Crucified Skeleton");
		break;
	case OBJ_BOOK2L:
		if (setlevel) {
			if (setlvlnum == SL_BONECHAMB) {
				strcpy(infostr, "Ancient Tome");
			} else if (setlvlnum == SL_VILEBETRAYER) {
				strcpy(infostr, "Book of Vileness");
			}
		}
		break;
	case OBJ_SWITCHSKL:
		strcpy(infostr, "Skull Lever");
		break;
	case OBJ_BOOK2R:
		strcpy(infostr, "Mythical Book");
		break;
	case OBJ_SARC:
		strcpy(infostr, "Sarcophagus");
		break;
	case OBJ_BOOKSHELF:
		strcpy(infostr, "Bookshelf");
		break;
	case OBJ_BARREL:
	case OBJ_BARRELEX:
		strcpy(infostr, "Barrel");
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		sprintf(tempstr, "%s Shrine", shrinestrs[object[v1]._oVar1]);
		strcpy(infostr, tempstr);
		break;
	case OBJ_SKELBOOK:
		strcpy(infostr, "Skeleton Tome");
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		strcpy(infostr, "Bookcase");
		break;
	case OBJ_BOOKSTAND:
		strcpy(infostr, "Library Book");
		break;
	case OBJ_BLOODFTN:
		strcpy(infostr, "Blood Fountain");
		break;
	case OBJ_DECAP:
		strcpy(infostr, "Decapitated Body");
		break;
	case OBJ_BLINDBOOK:
		strcpy(infostr, "Book of the Blind");
		break;
	case OBJ_BLOODBOOK:
		strcpy(infostr, "Book of Blood");
		break;
	case OBJ_PEDISTAL:
		strcpy(infostr, "Pedestal of Blood");
		break;
	case OBJ_PURIFYINGFTN:
		strcpy(infostr, "Purifying Spring");
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		strcpy(infostr, "Armor");
		break;
	case OBJ_GOATSHRINE:
		strcpy(infostr, "Goat Shrine");
		break;
	case OBJ_CAULDRON:
		strcpy(infostr, "Cauldron");
		break;
	case OBJ_MURKYFTN:
		strcpy(infostr, "Murky Pool");
		break;
	case OBJ_TEARFTN:
		strcpy(infostr, "Fountain of Tears");
		break;
	case OBJ_STORYBOOK:
		strcpy(infostr, StoryBookName[object[v1]._oVar3]);
		break;
	case OBJ_STEELTOME:
		strcpy(infostr, "Steel Tome");
		break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK:
		strcpy(infostr, "Weapon Rack");
		break;
	case OBJ_MUSHPATCH:
		strcpy(infostr, "Mushroom Patch");
		break;
	case OBJ_LAZSTAND:
		strcpy(infostr, "Vile Stand");
		break;
	case OBJ_SLAINHERO:
		strcpy(infostr, "Slain Hero");
		break;
	default:
		break;
	}
	if (plr[myplr]._pClass == PC_ROGUE) {
		if (object[v1]._oTrapFlag) {
			sprintf(tempstr, "Trapped %s", infostr);
			strcpy(infostr, tempstr);
			infoclr = COL_RED;
		}
	}
}
// 4B883C: using guessed type int infoclr;
// 5CF31D: using guessed type char setlevel;

DEVILUTION_END_NAMESPACE
