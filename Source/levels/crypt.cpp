#include "levels/crypt.h"

#include "engine/point.hpp"
#include "levels/drlg_l1.h"

namespace devilution {

int UberRow;
int UberCol;
bool IsUberRoomOpened;
bool IsUberLeverActivated;
int UberDiabloMonsterIndex;

/** Miniset: stairs up. */
const Miniset L5STAIRSUP {
	{ 4, 4 },
	{
	    { 22, 22, 22, 22 },
	    { 2, 2, 2, 2 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 0, 66, 23, 0 },
	    { 63, 64, 65, 0 },
	    { 0, 67, 68, 0 },
	    { 0, 0, 0, 0 },
	}
};

namespace {

const Miniset L5STAIRSUPHF {
	{ 4, 5 },
	{
	    { 22, 22, 22, 22 },
	    { 22, 22, 22, 22 },
	    { 2, 2, 2, 2 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 0, 54, 23, 0 },
	    { 0, 53, 18, 0 },
	    { 55, 56, 57, 0 },
	    { 58, 59, 60, 0 },
	    { 0, 0, 0, 0 },
	}
};
const Miniset L5STAIRSDOWN {
	{ 4, 5 },
	{
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 0, 0, 52, 0 },
	    { 0, 48, 51, 0 },
	    { 0, 47, 50, 0 },
	    { 45, 46, 49, 0 },
	    { 0, 0, 0, 0 },
	}
};
const Miniset L5STAIRSTOWN {
	{ 4, 5 },
	{
	    { 22, 22, 22, 22 },
	    { 22, 22, 22, 22 },
	    { 2, 2, 2, 2 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 0, 62, 23, 0 },
	    { 0, 61, 18, 0 },
	    { 63, 64, 65, 0 },
	    { 66, 67, 68, 0 },
	    { 0, 0, 0, 0 },
	}
};
const Miniset VWallSection {
	{ 1, 3 },
	{
	    { 1 },
	    { 1 },
	    { 1 },
	},
	{
	    { 91 },
	    { 90 },
	    { 89 },
	}
};
const Miniset HWallSection {
	{ 3, 1 },
	{ { 2, 2, 2 } },
	{ { 94, 93, 92 } }
};
const Miniset CryptFloorLave {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 101, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar1 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 167, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar2 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 168, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar3 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 169, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar4 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 170, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar5 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 171, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptStar {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 172, 0 },
	    { 0, 0, 0 },
	}
};

const Miniset UberRoomPattern {
	{ 4, 6 },
	{},
	{
	    { 115, 130, 6, 13 },
	    { 129, 108, 1, 13 },
	    { 1, 107, 103, 13 },
	    { 146, 106, 102, 13 },
	    { 129, 168, 1, 13 },
	    { 7, 2, 3, 13 },
	}
};
const Miniset CornerstoneRoomPattern {
	{ 5, 5 },
	{},
	{
	    { 4, 2, 2, 2, 6 },
	    { 1, 111, 172, 13, 1 },
	    { 1, 172, 13, 13, 25 },
	    { 1, 13, 13, 13, 1 },
	    { 7, 2, 2, 2, 3 },
	}
};

enum Tile : uint8_t {
	// clang-format off
	VWall          =   1,
	HWall          =   2,
	Corner         =   3,
	DWall          =   4,
	DArch          =   5,
	VWallEnd       =   6,
	HWallEnd       =   7,
	HArchEnd       =   8,
	VArchEnd       =   9,
	HArchVWall     =  10,
	VArch          =  11,
	HArch          =  12,
	Floor          =  13,
	HWallVArch     =  14,
	Pillar         =  15,
	Pillar1        =  16,
	Pillar2        =  17,
	DirtCorner     =  21,
	VDoor          =  25,
	HDoor          =  26,
	HFenceVWall    =  27,
	HDoorVDoor     =  28,
	DFence         =  29,
	VDoorEnd       =  30,
	HDoorEnd       =  31,
	VFenceEnd      =  32,
	VFence         =  35,
	HFence         =  36,
	HWallVFence    =  37,
	HArchVFence    =  38,
	HArchVDoor     =  39,
	EntranceStairs =  64,
	VWall5         =  89,
	VWall6         =  90,
	VWall7         =  91,
	HWall5         =  92,
	HWall6         =  93,
	HWall7         =  94,
	VArch5         =  95,
	HArch5         =  96,
	Floor6         =  97,
	Floor7         =  98,
	Floor8         =  99,
	Floor9         = 100,
	Floor10        = 101,
	VWall2         = 112,
	HWall2         = 113,
	Corner2        = 114,
	DWall2         = 115,
	DArch2         = 116,
	VWallEnd2      = 117,
	HWallEnd2      = 118,
	HArchEnd2      = 119,
	VArchEnd2      = 120,
	HArchVWall2    = 121,
	VArch2         = 122,
	HArch2         = 123,
	Floor2         = 124,
	HWallVArch2    = 125,
	Pillar3        = 126,
	Pillar4        = 127,
	Pillar5        = 128,
	VWall3         = 129,
	HWall3         = 130,
	Corner3        = 131,
	DWall3         = 132,
	DArch3         = 133,
	VWallEnd3      = 134,
	HWallEnd3      = 135,
	HArchEnd3      = 136,
	VArchEnd3      = 137,
	HArchVWall3    = 138,
	VArch3         = 139,
	HArch3         = 140,
	Floor3         = 141,
	HWallVArch3    = 142,
	Pillar6        = 143,
	Pillar7        = 144,
	Pillar8        = 145,
	VWall4         = 146,
	HWall4         = 147,
	Corner4        = 148,
	DWall4         = 149,
	DArch4         = 150,
	VWallEnd4      = 151,
	HWallEnd4      = 152,
	HArchEnd4      = 153,
	VArchEnd4      = 154,
	HArchVWall4    = 155,
	VArch4         = 156,
	HArch4         = 157,
	Floor4         = 158,
	HWallVArch4    = 159,
	Pillar9        = 160,
	Pillar10       = 161,
	Pillar11       = 162,
	Floor11        = 163,
	Floor12        = 164,
	Floor13        = 165,
	Floor14        = 166,
	PillarHalf     = 167,
	VWall8         = 173,
	VWall9         = 174,
	VWall10        = 175,
	VWall11        = 176,
	VWall12        = 177,
	VWall13        = 178,
	HWall8         = 179,
	HWall9         = 180,
	HWall10        = 181,
	HWall11        = 182,
	HWall12        = 183,
	HWall13        = 184,
	VArch6         = 185,
	VArch7         = 186,
	HArch6         = 187,
	HArch7         = 188,
	Floor15        = 189,
	Floor16        = 190,
	Floor17        = 191,
	Pillar12       = 192,
	Floor18        = 193,
	Floor19        = 194,
	Floor20        = 195,
	Floor21        = 196,
	Floor22        = 197,
	Floor23        = 198,
	VDemon         = 199,
	HDemon         = 200,
	VSuccubus      = 201,
	HSuccubus      = 202,
	Shadow1        = 203,
	Shadow2        = 204,
	Shadow3        = 205,
	Shadow4        = 206,
	Shadow5        = 207,
	Shadow6        = 208,
	Shadow7        = 209,
	Shadow8        = 210,
	Shadow9        = 211,
	Shadow10       = 212,
	Shadow11       = 213,
	Shadow12       = 214,
	Shadow13       = 215,
	Shadow14       = 216,
	Shadow15       = 217,
	// clang-format on
};

void ApplyCryptShadowsPatterns()
{
	for (int j = 1; j < DMAXY; j++) {
		for (int i = 1; i < DMAXX; i++) {
			switch (dungeon[i][j]) {
			case DArch:
			case DArch2:
			case DArch3:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Shadow1;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Shadow2;
				if (dungeon[i][j - 1] == Floor)
					dungeon[i][j - 1] = Shadow3;
				break;
			case HWallEnd:
			case HWallEnd2:
			case HWallEnd3:
			case HWallEnd4:
			case Pillar:
			case Pillar2:
			case Pillar3:
			case Pillar5:
			case Pillar9:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Shadow4;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Shadow5;
				break;
			case HArchEnd:
			case HArchEnd2:
			case HArchEnd3:
			case HArchEnd4:
			case HWallVArch:
			case HWallVArch2:
			case HWallVArch3:
			case HWallVArch4:
			case VArch:
			case VArch4:
			case VArch5:
			case VArch6:
			case VArch7:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Shadow1;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Shadow2;
				break;
			case VArchEnd:
			case VArchEnd2:
			case VArchEnd4:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Shadow4;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Shadow5;
				if (dungeon[i][j - 1] == Floor)
					dungeon[i][j - 1] = Shadow3;
				break;
			case HArch:
			case HArch2:
			case HArchVWall:
			case HArchVWall2:
			case HArchVWall3:
			case HArchVWall4:
				if (dungeon[i][j - 1] == Floor)
					dungeon[i][j - 1] = Shadow3;
				break;
			case HArch5:
			case HArch6:
				if (dungeon[i][j - 1] == Floor)
					dungeon[i][j - 1] = Shadow6;
				break;
			case VArch2:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Shadow9;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Shadow10;
				break;
			case VArchEnd3:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Tile::Shadow11;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Tile::Shadow12;
				if (dungeon[i][j - 1] == Floor)
					dungeon[i][j - 1] = Shadow3;
				break;
			case VArch3:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Shadow13;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Shadow14;
				break;
			case HArch3:
			case HArch4:
				if (dungeon[i][j - 1] == Floor)
					dungeon[i][j - 1] = Shadow15;
				break;
			case Pillar6:
			case Pillar8:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Tile::Shadow11;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Tile::Shadow12;
				break;
			case DArch4:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Shadow1;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Shadow2;
				if (dungeon[i][j - 1] == Floor)
					dungeon[i][j - 1] = Shadow15;
				break;
			case Pillar11:
			case Pillar12:
			case PillarHalf:
				if (dungeon[i - 1][j] == Floor)
					dungeon[i - 1][j] = Shadow7;
				if (dungeon[i - 1][j - 1] == Floor)
					dungeon[i - 1][j - 1] = Shadow8;
				break;
			}
		}
	}
}

void PlaceMiniSetRandom1x1(uint8_t search, uint8_t replace, int rndper)
{
	PlaceMiniSetRandom({ { 1, 1 }, { { search } }, { { replace } } }, rndper);
}

void CryptStatues(int rndper)
{
	PlaceMiniSetRandom1x1(Tile::VWall, Tile::VDemon, rndper);
	PlaceMiniSetRandom1x1(Tile::VWall, Tile::VSuccubus, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall, Tile::HDemon, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall, Tile::HSuccubus, rndper);
}

void CryptCracked(int rndper)
{
	// clang-format off
	PlaceMiniSetRandom1x1(Tile::VWall,      Tile::VWall2,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWall,      Tile::HWall2,      rndper);
	PlaceMiniSetRandom1x1(Tile::Corner,     Tile::Corner2,     rndper);
	PlaceMiniSetRandom1x1(Tile::DWall,      Tile::DWall2,      rndper);
	PlaceMiniSetRandom1x1(Tile::DArch,      Tile::DArch2,      rndper);
	PlaceMiniSetRandom1x1(Tile::VWallEnd,   Tile::VWallEnd2,   rndper);
	PlaceMiniSetRandom1x1(Tile::HWallEnd,   Tile::HWallEnd2,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchEnd,   Tile::HArchEnd2,   rndper);
	PlaceMiniSetRandom1x1(Tile::VArchEnd,   Tile::VArchEnd2,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchVWall, Tile::HArchVWall2, rndper);
	PlaceMiniSetRandom1x1(Tile::VArch,      Tile::VArch2,      rndper);
	PlaceMiniSetRandom1x1(Tile::HArch,      Tile::HArch2,      rndper);
	PlaceMiniSetRandom1x1(Tile::Floor,      Tile::Floor2,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWallVArch, Tile::HWallVArch2, rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar,     Tile::Pillar3,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar1,    Tile::Pillar4,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar2,    Tile::Pillar5,      rndper);
	// clang-format on
}

void CryptBroken(int rndper)
{
	// clang-format off
	PlaceMiniSetRandom1x1(Tile::VWall,      Tile::VWall3,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWall,      Tile::HWall3,      rndper);
	PlaceMiniSetRandom1x1(Tile::Corner,     Tile::Corner3,     rndper);
	PlaceMiniSetRandom1x1(Tile::DWall,      Tile::DWall3,      rndper);
	PlaceMiniSetRandom1x1(Tile::DArch,      Tile::DArch3,      rndper);
	PlaceMiniSetRandom1x1(Tile::VWallEnd,   Tile::VWallEnd3,   rndper);
	PlaceMiniSetRandom1x1(Tile::HWallEnd,   Tile::HWallEnd3,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchEnd,   Tile::HArchEnd3,   rndper);
	PlaceMiniSetRandom1x1(Tile::VArchEnd,   Tile::VArchEnd3,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchVWall, Tile::HArchVWall3, rndper);
	PlaceMiniSetRandom1x1(Tile::VArch,      Tile::VArch3,      rndper);
	PlaceMiniSetRandom1x1(Tile::HArch,      Tile::HArch3,      rndper);
	PlaceMiniSetRandom1x1(Tile::Floor,      Tile::Floor3,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWallVArch, Tile::HWallVArch3, rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar,      Tile::Pillar6,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar1,     Tile::Pillar7,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar2,     Tile::Pillar8,      rndper);
	// clang-format on
}

void CryptLeaking(int rndper)
{
	// clang-format off
	PlaceMiniSetRandom1x1(Tile::VWall,      Tile::VWall4,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWall,      Tile::HWall4,      rndper);
	PlaceMiniSetRandom1x1(Tile::Corner,     Tile::Corner4,     rndper);
	PlaceMiniSetRandom1x1(Tile::DWall,      Tile::DWall4,      rndper);
	PlaceMiniSetRandom1x1(Tile::DArch,      Tile::DArch4,      rndper);
	PlaceMiniSetRandom1x1(Tile::VWallEnd,   Tile::VWallEnd4,   rndper);
	PlaceMiniSetRandom1x1(Tile::HWallEnd,   Tile::HWallEnd4,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchEnd,   Tile::HArchEnd4,   rndper);
	PlaceMiniSetRandom1x1(Tile::VArchEnd,   Tile::VArchEnd4,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchVWall, Tile::HArchVWall4, rndper);
	PlaceMiniSetRandom1x1(Tile::VArch,      Tile::VArch4,      rndper);
	PlaceMiniSetRandom1x1(Tile::HArch,      Tile::HArch4,      rndper);
	PlaceMiniSetRandom1x1(Tile::Floor,      Tile::Floor4,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWallVArch, Tile::HWallVArch4, rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar,      Tile::Pillar9,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar1,     Tile::Pillar10,     rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar2,     Tile::Pillar11,     rndper);
	// clang-format on
}

void CryptSubstitions1(int rndper)
{
	PlaceMiniSetRandom1x1(Tile::VArch, Tile::VArch6, rndper);
	PlaceMiniSetRandom1x1(Tile::HArch, Tile::HArch6, rndper);
	PlaceMiniSetRandom1x1(Tile::VArch, Tile::VArch7, rndper);
	PlaceMiniSetRandom1x1(Tile::HArch, Tile::HArch7, rndper);
	PlaceMiniSetRandom1x1(Tile::VWall5, Tile::VWall8, rndper);
	PlaceMiniSetRandom1x1(Tile::VWall5, Tile::VWall9, rndper);
	PlaceMiniSetRandom1x1(Tile::VWall6, Tile::VWall10, rndper);
	PlaceMiniSetRandom1x1(Tile::VWall6, Tile::VWall11, rndper);
	PlaceMiniSetRandom1x1(Tile::VWall7, Tile::VWall12, rndper);
	PlaceMiniSetRandom1x1(Tile::VWall7, Tile::VWall13, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall5, Tile::HWall8, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall5, Tile::HWall9, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall5, Tile::HWall10, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall5, Tile::HWall11, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall5, Tile::HWall12, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall5, Tile::HWall13, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor7, Tile::Floor15, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor7, Tile::Floor16, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor6, Tile::Floor17, rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar, Tile::Pillar12, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor8, Tile::Floor18, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor8, Tile::Floor19, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor9, Tile::Floor20, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor10, Tile::Floor21, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor10, Tile::Floor22, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor10, Tile::Floor23, rndper);
}

void CryptSubstitions2(int rndper)
{
	PlaceMiniSetRandom(CryptPillar1, rndper);
	PlaceMiniSetRandom(CryptPillar2, rndper);
	PlaceMiniSetRandom(CryptPillar3, rndper);
	PlaceMiniSetRandom(CryptPillar4, rndper);
	PlaceMiniSetRandom(CryptPillar5, rndper);
	PlaceMiniSetRandom(CryptStar, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, Tile::Floor11, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, Tile::Floor12, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, Tile::Floor13, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, Tile::Floor14, rndper);
}

void CryptFloor(int rndper)
{
	PlaceMiniSetRandom1x1(Tile::Floor, Tile::Floor6, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, Tile::Floor7, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, Tile::Floor8, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, Tile::Floor9, rndper);
}

} // namespace

void InitCryptPieces()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 76) {
				dSpecial[i][j] = 1;
			} else if (dPiece[i][j] == 79) {
				dSpecial[i][j] = 2;
			}
		}
	}
}

void SetCryptRoom()
{
	Point position = SelectChamber();

	UberRow = 2 * position.x + 6;
	UberCol = 2 * position.y + 8;
	IsUberRoomOpened = false;
	IsUberLeverActivated = false;

	SetPiece = { position, UberRoomPattern.size };

	UberRoomPattern.place(position, true);
}

void SetCornerRoom()
{
	Point position = SelectChamber();

	SetPiece = { position, CornerstoneRoomPattern.size };

	CornerstoneRoomPattern.place(position, true);
}

void FixCryptDirtTiles()
{
	for (int j = 0; j < DMAXY - 1; j++) {
		for (int i = 0; i < DMAXX - 1; i++) {
			if (dungeon[i][j] == 19)
				dungeon[i][j] = 83;
			if (dungeon[i][j] == 21)
				dungeon[i][j] = 85;
			if (dungeon[i][j] == 23)
				dungeon[i][j] = 87;
			if (dungeon[i][j] == 24)
				dungeon[i][j] = 88;
			if (dungeon[i][j] == 18)
				dungeon[i][j] = 82;
		}
	}
}

bool PlaceCryptStairs(lvl_entry entry)
{
	bool success = true;
	std::optional<Point> position;

	// Place stairs up
	position = PlaceMiniSet(currlevel != 21 ? L5STAIRSUPHF : L5STAIRSTOWN, DMAXX * DMAXY, true);
	if (!position) {
		success = false;
	} else if (entry == ENTRY_MAIN || entry == ENTRY_TWARPDN) {
		ViewPosition = position->megaToWorld() + Displacement { 3, 5 };
	}

	// Place stairs down
	if (currlevel != 24) {
		position = PlaceMiniSet(L5STAIRSDOWN, DMAXX * DMAXY, true);
		if (!position)
			success = false;
		else if (entry == ENTRY_PREV)
			ViewPosition = position->megaToWorld() + Displacement { 3, 7 };
	}

	return success;
}

void CryptSubstitution()
{
	CryptStatues(10);
	PlaceMiniSetRandom1x1(Tile::VArch, Tile::VArch5, 95);
	PlaceMiniSetRandom1x1(Tile::HArch, Tile::HArch5, 95);
	PlaceMiniSetRandom(VWallSection, 100);
	PlaceMiniSetRandom(HWallSection, 100);
	PlaceMiniSetRandom(CryptFloorLave, 60);
	ApplyCryptShadowsPatterns();
	switch (currlevel) {
	case 21:
		CryptCracked(30);
		CryptBroken(15);
		CryptLeaking(5);
		ApplyCryptShadowsPatterns();
		CryptFloor(10);
		CryptSubstitions1(5);
		CryptSubstitions2(20);
		break;
	case 22:
		CryptFloor(10);
		CryptSubstitions1(10);
		CryptSubstitions2(20);
		CryptCracked(30);
		CryptBroken(20);
		CryptLeaking(10);
		ApplyCryptShadowsPatterns();
		break;
	case 23:
		CryptFloor(10);
		CryptSubstitions1(15);
		CryptSubstitions2(30);
		CryptCracked(30);
		CryptBroken(20);
		CryptLeaking(15);
		ApplyCryptShadowsPatterns();
		break;
	default:
		CryptFloor(10);
		CryptSubstitions1(20);
		CryptSubstitions2(30);
		CryptCracked(30);
		CryptBroken(20);
		CryptLeaking(20);
		ApplyCryptShadowsPatterns();
		break;
	}
}

} // namespace devilution
