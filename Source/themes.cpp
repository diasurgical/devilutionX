/**
 * @file themes.cpp
 *
 * Implementation of the theme room placing algorithms.
 */
#include "themes.h"

#include "engine/random.hpp"
#include "items.h"
#include "monster.h"
#include "objects.h"
#include "path.h"
#include "quests.h"
#include "trigs.h"

namespace devilution {

int numthemes;
bool armorFlag;
bool weaponFlag;
bool treasureFlag;
bool mFountainFlag;
bool cauldronFlag;
bool tFountainFlag;
int zharlib;
int themex;
int themey;
int themeVar1;
ThemeStruct themes[MAXTHEMES];
bool pFountainFlag;
bool bFountainFlag;

/** Specifies the set of special theme IDs from which one will be selected at random. */
theme_id ThemeGood[4] = { THEME_GOATSHRINE, THEME_SHRINE, THEME_SKELROOM, THEME_LIBRARY };
/** Specifies a 5x5 area to fit theme objects. */
int trm5x[] = {
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2,
	-2, -1, 0, 1, 2
};
/** Specifies a 5x5 area to fit theme objects. */
int trm5y[] = {
	-2, -2, -2, -2, -2,
	-1, -1, -1, -1, -1,
	0, 0, 0, 0, 0,
	1, 1, 1, 1, 1,
	2, 2, 2, 2, 2
};
/** Specifies a 3x3 area to fit theme objects. */
int trm3x[] = {
	-1, 0, 1,
	-1, 0, 1,
	-1, 0, 1
};
/** Specifies a 3x3 area to fit theme objects. */
int trm3y[] = {
	-1, -1, -1,
	0, 0, 0,
	1, 1, 1
};

bool TFit_Shrine(int i)
{
	int xp = 0;
	int yp = 0;
	int found = 0;

	while (found == 0) {
		Point testPosition { xp, yp };
		if (dTransVal[xp][yp] == themes[i].ttval) {
			if (nTrapTable[dPiece[xp][yp - 1]]
			    && IsTileNotSolid(testPosition + Direction::NorthWest)
			    && IsTileNotSolid(testPosition + Direction::SouthEast)
			    && dTransVal[xp - 1][yp] == themes[i].ttval
			    && dTransVal[xp + 1][yp] == themes[i].ttval
			    && !IsObjectAtPosition(testPosition + Direction::North)
			    && !IsObjectAtPosition(testPosition + Direction::East)) {
				found = 1;
			}
			if (found == 0
			    && nTrapTable[dPiece[xp - 1][yp]]
			    && IsTileNotSolid(testPosition + Direction::NorthEast)
			    && IsTileNotSolid(testPosition + Direction::SouthWest)
			    && dTransVal[xp][yp - 1] == themes[i].ttval
			    && dTransVal[xp][yp + 1] == themes[i].ttval
			    && !IsObjectAtPosition(testPosition + Direction::North)
			    && !IsObjectAtPosition(testPosition + Direction::West)) {
				found = 2;
			}
		}
		if (found == 0) {
			xp++;
			if (xp == MAXDUNX) {
				xp = 0;
				yp++;
				if (yp == MAXDUNY)
					return false;
			}
		}
	}
	themex = xp;
	themey = yp;
	themeVar1 = found;
	return true;
}

bool TFit_Obj5(int t)
{
	int xp = 0;
	int yp = 0;
	int r = GenerateRnd(5) + 1;
	int rs = r;

	while (r > 0) {
		bool found = false;
		if (dTransVal[xp][yp] == themes[t].ttval && IsTileNotSolid({ xp, yp })) {
			found = true;
			for (int i = 0; found && i < 25; i++) {
				if (nSolidTable[dPiece[xp + trm5x[i]][yp + trm5y[i]]]) {
					found = false;
				}
				if (dTransVal[xp + trm5x[i]][yp + trm5y[i]] != themes[t].ttval) {
					found = false;
				}
			}
		}

		if (!found) {
			xp++;
			if (xp == MAXDUNX) {
				xp = 0;
				yp++;
				if (yp == MAXDUNY) {
					if (r == rs) {
						return false;
					}
					yp = 0;
				}
			}
			continue;
		}

		r--;
	}

	themex = xp;
	themey = yp;

	return true;
}

bool TFit_SkelRoom(int t)
{
	if (leveltype != DTYPE_CATHEDRAL && leveltype != DTYPE_CATACOMBS) {
		return false;
	}

	for (int i = 0; i < LevelMonsterTypeCount; i++) {
		if (IsSkel(LevelMonsterTypes[i].mtype)) {
			themeVar1 = i;
			return TFit_Obj5(t);
		}
	}

	return false;
}

bool TFit_GoatShrine(int t)
{
	for (int i = 0; i < LevelMonsterTypeCount; i++) {
		if (IsGoat(LevelMonsterTypes[i].mtype)) {
			themeVar1 = i;
			return TFit_Obj5(t);
		}
	}

	return false;
}

bool CheckThemeObj3(int xp, int yp, int t, int f)
{
	for (int i = 0; i < 9; i++) {
		Point testPosition = Point { xp, yp } + Displacement { trm3x[i], trm3y[i] };
		if (testPosition.x < 0 || testPosition.y < 0) {
			return false;
		}
		if (IsTileSolid(testPosition)) {
			return false;
		}
		if (dTransVal[testPosition.x][testPosition.y] != themes[t].ttval) {
			return false;
		}
		if (IsObjectAtPosition(testPosition)) {
			return false;
		}
		if (f != -1 && GenerateRnd(f) == 0) {
			return false;
		}
	}

	return true;
}

bool TFit_Obj3(int t)
{
	char objrnd[4] = { 4, 4, 3, 5 };

	for (int yp = 1; yp < MAXDUNY - 1; yp++) {
		for (int xp = 1; xp < MAXDUNX - 1; xp++) {
			if (CheckThemeObj3(xp, yp, t, objrnd[leveltype - 1])) {
				themex = xp;
				themey = yp;
				return true;
			}
		}
	}

	return false;
}

bool CheckThemeReqs(theme_id t)
{
	switch (t) {
	case THEME_SHRINE:
	case THEME_SKELROOM:
	case THEME_LIBRARY:
		if (leveltype == DTYPE_CAVES || leveltype == DTYPE_HELL) {
			return false;
		}
		break;
	case THEME_BLOODFOUNTAIN:
		if (!bFountainFlag) {
			return false;
		}
		break;
	case THEME_PURIFYINGFOUNTAIN:
		if (!pFountainFlag) {
			return false;
		}
		break;
	case THEME_ARMORSTAND:
		if (leveltype == DTYPE_CATHEDRAL) {
			return false;
		}
		break;
	case THEME_CAULDRON:
		if (leveltype != DTYPE_HELL || !cauldronFlag) {
			return false;
		}
		break;
	case THEME_MURKYFOUNTAIN:
		if (!mFountainFlag) {
			return false;
		}
		break;
	case THEME_TEARFOUNTAIN:
		if (!tFountainFlag) {
			return false;
		}
		break;
	case THEME_WEAPONRACK:
		if (leveltype == DTYPE_CATHEDRAL) {
			return false;
		}
		break;
	default:
		break;
	}

	return true;
}

static bool SpecialThemeFit(int i, theme_id t)
{
	bool rv;

	rv = CheckThemeReqs(t);
	switch (t) {
	case THEME_SHRINE:
	case THEME_LIBRARY:
		if (rv) {
			rv = TFit_Shrine(i);
		}
		break;
	case THEME_SKELROOM:
		if (rv) {
			rv = TFit_SkelRoom(i);
		}
		break;
	case THEME_BLOODFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			bFountainFlag = false;
		}
		break;
	case THEME_PURIFYINGFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			pFountainFlag = false;
		}
		break;
	case THEME_MURKYFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			mFountainFlag = false;
		}
		break;
	case THEME_TEARFOUNTAIN:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			tFountainFlag = false;
		}
		break;
	case THEME_CAULDRON:
		if (rv) {
			rv = TFit_Obj5(i);
		}
		if (rv) {
			cauldronFlag = false;
		}
		break;
	case THEME_GOATSHRINE:
		if (rv) {
			rv = TFit_GoatShrine(i);
		}
		break;
	case THEME_TORTURE:
	case THEME_DECAPITATED:
	case THEME_ARMORSTAND:
	case THEME_BRNCROSS:
	case THEME_WEAPONRACK:
		if (rv) {
			rv = TFit_Obj3(i);
		}
		break;
	case THEME_TREASURE:
		rv = treasureFlag;
		if (rv) {
			treasureFlag = false;
		}
		break;
	default:
		break;
	}

	return rv;
}

bool CheckThemeRoom(int tv)
{
	for (int i = 0; i < numtrigs; i++) {
		if (dTransVal[trigs[i].position.x][trigs[i].position.y] == tv)
			return false;
	}

	int tarea = 0;
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (dTransVal[i][j] != tv)
				continue;
			if (TileContainsSetPiece({ i, j }))
				return false;

			tarea++;
		}
	}

	if (leveltype == DTYPE_CATHEDRAL && (tarea < 9 || tarea > 100))
		return false;

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (dTransVal[i][j] != tv || nSolidTable[dPiece[i][j]])
				continue;
			if (dTransVal[i - 1][j] != tv && IsTileNotSolid({ i - 1, j }))
				return false;
			if (dTransVal[i + 1][j] != tv && IsTileNotSolid({ i + 1, j }))
				return false;
			if (dTransVal[i][j - 1] != tv && IsTileNotSolid({ i, j - 1 }))
				return false;
			if (dTransVal[i][j + 1] != tv && IsTileNotSolid({ i, j + 1 }))
				return false;
		}
	}

	return true;
}

void InitThemes()
{
	zharlib = -1;
	numthemes = 0;
	armorFlag = true;
	bFountainFlag = true;
	cauldronFlag = true;
	mFountainFlag = true;
	pFountainFlag = true;
	tFountainFlag = true;
	treasureFlag = true;
	weaponFlag = true;

	if (currlevel == 16)
		return;

	if (leveltype == DTYPE_CATHEDRAL) {
		for (size_t i = 0; i < 256 && numthemes < MAXTHEMES; i++) {
			if (CheckThemeRoom(i)) {
				themes[numthemes].ttval = i;
				theme_id j = ThemeGood[GenerateRnd(4)];
				while (!SpecialThemeFit(numthemes, j)) {
					j = (theme_id)GenerateRnd(17);
				}
				themes[numthemes].ttype = j;
				numthemes++;
			}
		}
	}
	if (leveltype == DTYPE_CATACOMBS || leveltype == DTYPE_CAVES || leveltype == DTYPE_HELL) {
		for (int i = 0; i < themeCount; i++)
			themes[i].ttype = THEME_NONE;
		if (Quests[Q_ZHAR].IsAvailable()) {
			for (int j = 0; j < themeCount; j++) {
				themes[j].ttval = themeLoc[j].ttval;
				if (SpecialThemeFit(j, THEME_LIBRARY)) {
					themes[j].ttype = THEME_LIBRARY;
					zharlib = j;
					break;
				}
			}
		}
		for (int i = 0; i < themeCount; i++) {
			if (themes[i].ttype == THEME_NONE) {
				themes[i].ttval = themeLoc[i].ttval;
				theme_id j = ThemeGood[GenerateRnd(4)];
				while (!SpecialThemeFit(i, j)) {
					j = (theme_id)GenerateRnd(17);
				}
				themes[i].ttype = j;
			}
		}
		numthemes += themeCount;
	}
}

void HoldThemeRooms()
{
	if (currlevel == 16)
		return;

	if (leveltype != DTYPE_CATHEDRAL) {
		DRLG_HoldThemeRooms();
		return;
	}

	for (int i = 0; i < numthemes; i++) {
		int8_t v = themes[i].ttval;
		for (int y = 0; y < MAXDUNY; y++) {
			for (int x = 0; x < MAXDUNX; x++) {
				if (dTransVal[x][y] == v) {
					dFlags[x][y] |= DungeonFlag::Populated;
				}
			}
		}
	}
}

/**
 * PlaceThemeMonsts places theme monsters with the specified frequency.
 *
 * @param t theme number (index into themes array).
 * @param f frequency (1/f likelihood of adding monster).
 */
void PlaceThemeMonsts(int t, int f)
{
	int scattertypes[138];

	int numscattypes = 0;
	for (int i = 0; i < LevelMonsterTypeCount; i++) {
		if ((LevelMonsterTypes[i].mPlaceFlags & PLACE_SCATTER) != 0) {
			scattertypes[numscattypes] = i;
			numscattypes++;
		}
	}
	int mtype = scattertypes[GenerateRnd(numscattypes)];
	for (int yp = 0; yp < MAXDUNY; yp++) {
		for (int xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && IsTileNotSolid({ xp, yp }) && dItem[xp][yp] == 0 && !IsObjectAtPosition({ xp, yp })) {
				if (GenerateRnd(f) == 0) {
					AddMonster({ xp, yp }, static_cast<Direction>(GenerateRnd(8)), mtype, true);
				}
			}
		}
	}
}

/**
 * Theme_Barrel initializes the barrel theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Barrel(int t)
{
	char barrnd[4] = { 2, 6, 4, 8 };
	char monstrnd[4] = { 5, 7, 3, 9 };

	for (int yp = 0; yp < MAXDUNY; yp++) {
		for (int xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && IsTileNotSolid({ xp, yp })) {
				if (GenerateRnd(barrnd[leveltype - 1]) == 0) {
					_object_id r = OBJ_BARREL;
					if (GenerateRnd(barrnd[leveltype - 1]) != 0) {
						r = OBJ_BARRELEX;
					}
					AddObject(r, { xp, yp });
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_Shrine initializes the shrine theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Shrine(int t)
{
	char monstrnd[4] = { 6, 6, 3, 9 };

	TFit_Shrine(t);
	if (themeVar1 == 1) {
		AddObject(OBJ_CANDLE2, { themex - 1, themey });
		AddObject(OBJ_SHRINER, { themex, themey });
		AddObject(OBJ_CANDLE2, { themex + 1, themey });
	} else {
		AddObject(OBJ_CANDLE2, { themex, themey - 1 });
		AddObject(OBJ_SHRINEL, { themex, themey });
		AddObject(OBJ_CANDLE2, { themex, themey + 1 });
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_MonstPit initializes the monster pit theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_MonstPit(int t)
{
	uint8_t monstrnd[4] = { 6, 7, 3, 9 };

	int r = GenerateRnd(100) + 1;
	int ixp = 0;
	int iyp = 0;
	while (r > 0) {
		if (dTransVal[ixp][iyp] == themes[t].ttval && IsTileNotSolid({ ixp, iyp })) {
			--r;
		}
		if (r <= 0)
			continue;
		ixp++;
		if (ixp == MAXDUNX) {
			ixp = 0;
			iyp++;
			if (iyp == MAXDUNY) {
				iyp = 0;
			}
		}
	}
	CreateRndItem({ ixp, iyp }, true, false, true);
	ItemNoFlippy();
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_SkelRoom initializes the skeleton room theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_SkelRoom(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_SkelRoom(t);

	int xp = themex;
	int yp = themey;

	AddObject(OBJ_SKFIRE, { xp, yp });

	if (GenerateRnd(monstrnd[leveltype - 1]) != 0) {
		int i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp - 1, yp - 1 });
	} else {
		AddObject(OBJ_BANNERL, { xp - 1, yp - 1 });
	}

	{
		int i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp, yp - 1 });
	}

	if (GenerateRnd(monstrnd[leveltype - 1]) != 0) {
		int i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp + 1, yp - 1 });
	} else {
		AddObject(OBJ_BANNERR, { xp + 1, yp - 1 });
	}
	if (GenerateRnd(monstrnd[leveltype - 1]) != 0) {
		int i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp - 1, yp });
	} else {
		AddObject(OBJ_BANNERM, { xp - 1, yp });
	}
	if (GenerateRnd(monstrnd[leveltype - 1]) != 0) {
		int i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp + 1, yp });
	} else {
		AddObject(OBJ_BANNERM, { xp + 1, yp });
	}
	if (GenerateRnd(monstrnd[leveltype - 1]) != 0) {
		int i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp - 1, yp + 1 });
	} else {
		AddObject(OBJ_BANNERR, { xp - 1, yp + 1 });
	}

	{
		int i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp, yp + 1 });
	}

	if (GenerateRnd(monstrnd[leveltype - 1]) != 0) {
		int i = PreSpawnSkeleton();
		SpawnSkeleton(i, { xp + 1, yp + 1 });
	} else {
		AddObject(OBJ_BANNERL, { xp + 1, yp + 1 });
	}

	if (!IsObjectAtPosition({ xp, yp - 3 })) {
		AddObject(OBJ_SKELBOOK, { xp, yp - 2 });
	}
	if (!IsObjectAtPosition({ xp, yp + 3 })) {
		AddObject(OBJ_SKELBOOK, { xp, yp + 2 });
	}
}

/**
 * Theme_Treasure initializes the treasure theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Treasure(int t)
{
	int8_t treasrnd[4] = { 4, 9, 7, 10 };
	int8_t monstrnd[4] = { 6, 8, 3, 7 };

	AdvanceRndSeed();
	for (int yp = 0; yp < MAXDUNY; yp++) {
		for (int xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && IsTileNotSolid({ xp, yp })) {
				int8_t treasureType = treasrnd[leveltype - 1];
				int rv = GenerateRnd(treasureType);
				// BUGFIX: the `2*` in `2*GenerateRnd(treasrnd...) == 0` has no effect, should probably be `GenerateRnd(2*treasrnd...) == 0`
				if ((2 * GenerateRnd(treasureType)) == 0) {
					CreateTypeItem({ xp, yp }, false, ItemType::Gold, IMISC_NONE, false, true);
					ItemNoFlippy();
				}
				if (rv == 0) {
					CreateRndItem({ xp, yp }, false, false, true);
					ItemNoFlippy();
				}
				if (rv >= treasureType - 2 && leveltype != DTYPE_CATHEDRAL) {
					Item &item = Items[ActiveItems[ActiveItemCount - 1]];
					if (item.IDidx == IDI_GOLD) {
						item._ivalue = std::max(item._ivalue / 2, 1);
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_Library initializes the library theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Library(int t)
{
	char librnd[4] = { 1, 2, 2, 5 };
	char monstrnd[4] = { 5, 7, 3, 9 };

	TFit_Shrine(t);

	if (themeVar1 == 1) {
		AddObject(OBJ_BOOKCANDLE, { themex - 1, themey });
		AddObject(OBJ_BOOKCASER, { themex, themey });
		AddObject(OBJ_BOOKCANDLE, { themex + 1, themey });
	} else {
		AddObject(OBJ_BOOKCANDLE, { themex, themey - 1 });
		AddObject(OBJ_BOOKCASEL, { themex, themey });
		AddObject(OBJ_BOOKCANDLE, { themex, themey + 1 });
	}

	for (int yp = 1; yp < MAXDUNY - 1; yp++) {
		for (int xp = 1; xp < MAXDUNX - 1; xp++) {
			if (CheckThemeObj3(xp, yp, t, -1) && dMonster[xp][yp] == 0 && GenerateRnd(librnd[leveltype - 1]) == 0) {
				AddObject(OBJ_BOOKSTAND, { xp, yp });
				if (GenerateRnd(2 * librnd[leveltype - 1]) != 0) {
					Object *bookstand = ObjectAtPosition({ xp, yp });
					if (bookstand != nullptr) {
						bookstand->_oSelFlag = 0;
						bookstand->_oAnimFrame += 2;
					}
				}
			}
		}
	}

	if (Quests[Q_ZHAR].IsAvailable()) {
		if (t == zharlib) {
			return;
		}
		PlaceThemeMonsts(t, monstrnd[leveltype]); /// BUGFIX: `leveltype - 1`
	} else {
		PlaceThemeMonsts(t, monstrnd[leveltype]); /// BUGFIX: `leveltype - 1`
	}
}

/**
 * Theme_Torture initializes the torture theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Torture(int t)
{
	char tortrnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 8, 3, 9 };

	for (int yp = 1; yp < MAXDUNY - 1; yp++) {
		for (int xp = 1; xp < MAXDUNX - 1; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && IsTileNotSolid({ xp, yp })) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (GenerateRnd(tortrnd[leveltype - 1]) == 0) {
						AddObject(OBJ_TNUDEM2, { xp, yp });
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_BloodFountain initializes the blood fountain theme.
 * @param t Theme number (index into themes array).
 */
void Theme_BloodFountain(int t)
{
	char monstrnd[4] = { 6, 8, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_BLOODFTN, { themex, themey });
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_Decap initializes the decapitated theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Decap(int t)
{
	char decaprnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 8, 3, 9 };

	for (int yp = 1; yp < MAXDUNY - 1; yp++) {
		for (int xp = 1; xp < MAXDUNX - 1; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && IsTileNotSolid({ xp, yp })) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (GenerateRnd(decaprnd[leveltype - 1]) == 0) {
						AddObject(OBJ_DECAP, { xp, yp });
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_PurifyingFountain initializes the purifying fountain theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_PurifyingFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_PURIFYINGFTN, { themex, themey });
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_ArmorStand initializes the armor stand theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_ArmorStand(int t)
{
	char armorrnd[4] = { 6, 8, 3, 8 };
	char monstrnd[4] = { 6, 7, 3, 9 };

	if (armorFlag) {
		TFit_Obj3(t);
		AddObject(OBJ_ARMORSTAND, { themex, themey });
	}
	for (int yp = 0; yp < MAXDUNY; yp++) {
		for (int xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && IsTileNotSolid({ xp, yp })) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (GenerateRnd(armorrnd[leveltype - 1]) == 0) {
						AddObject(OBJ_ARMORSTANDN, { xp, yp });
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
	armorFlag = false;
}

/**
 * Theme_GoatShrine initializes the goat shrine theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_GoatShrine(int t)
{
	TFit_GoatShrine(t);
	AddObject(OBJ_GOATSHRINE, { themex, themey });
	for (int yy = themey - 1; yy <= themey + 1; yy++) {
		for (int xx = themex - 1; xx <= themex + 1; xx++) {
			if (dTransVal[xx][yy] == themes[t].ttval && IsTileNotSolid({ xx, yy }) && (xx != themex || yy != themey)) {
				AddMonster({ xx, yy }, Direction::SouthWest, themeVar1, true);
			}
		}
	}
}

/**
 * Theme_Cauldron initializes the cauldron theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_Cauldron(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_CAULDRON, { themex, themey });
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_MurkyFountain initializes the murky fountain theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_MurkyFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_MURKYFTN, { themex, themey });
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_TearFountain initializes the tear fountain theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_TearFountain(int t)
{
	char monstrnd[4] = { 6, 7, 3, 9 };

	TFit_Obj5(t);
	AddObject(OBJ_TEARFTN, { themex, themey });
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_BrnCross initializes the burning cross theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_BrnCross(int t)
{
	char monstrnd[4] = { 6, 8, 3, 9 };
	char bcrossrnd[4] = { 5, 7, 3, 8 };

	for (int yp = 0; yp < MAXDUNY; yp++) {
		for (int xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && IsTileNotSolid({ xp, yp })) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (GenerateRnd(bcrossrnd[leveltype - 1]) == 0) {
						AddObject(OBJ_TBCROSS, { xp, yp });
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
}

/**
 * Theme_WeaponRack initializes the weapon rack theme.
 *
 * @param t theme number (index into themes array).
 */
void Theme_WeaponRack(int t)
{
	char weaponrnd[4] = { 6, 8, 5, 8 };
	char monstrnd[4] = { 6, 7, 3, 9 };

	if (weaponFlag) {
		TFit_Obj3(t);
		AddObject(OBJ_WEAPONRACK, { themex, themey });
	}
	for (int yp = 0; yp < MAXDUNY; yp++) {
		for (int xp = 0; xp < MAXDUNX; xp++) {
			if (dTransVal[xp][yp] == themes[t].ttval && IsTileNotSolid({ xp, yp })) {
				if (CheckThemeObj3(xp, yp, t, -1)) {
					if (GenerateRnd(weaponrnd[leveltype - 1]) == 0) {
						AddObject(OBJ_WEAPONRACKN, { xp, yp });
					}
				}
			}
		}
	}
	PlaceThemeMonsts(t, monstrnd[leveltype - 1]);
	weaponFlag = false;
}

/**
 * UpdateL4Trans sets each value of the transparency map to 1.
 */
void UpdateL4Trans()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) { // NOLINT(modernize-loop-convert)
			if (dTransVal[i][j] != 0) {
				dTransVal[i][j] = 1;
			}
		}
	}
}

void CreateThemeRooms()
{
	if (currlevel == 16) {
		return;
	}
	ApplyObjectLighting = true;
	for (int i = 0; i < numthemes; i++) {
		themex = 0;
		themey = 0;
		switch (themes[i].ttype) {
		case THEME_BARREL:
			Theme_Barrel(i);
			break;
		case THEME_SHRINE:
			Theme_Shrine(i);
			break;
		case THEME_MONSTPIT:
			Theme_MonstPit(i);
			break;
		case THEME_SKELROOM:
			Theme_SkelRoom(i);
			break;
		case THEME_TREASURE:
			Theme_Treasure(i);
			break;
		case THEME_LIBRARY:
			Theme_Library(i);
			break;
		case THEME_TORTURE:
			Theme_Torture(i);
			break;
		case THEME_BLOODFOUNTAIN:
			Theme_BloodFountain(i);
			break;
		case THEME_DECAPITATED:
			Theme_Decap(i);
			break;
		case THEME_PURIFYINGFOUNTAIN:
			Theme_PurifyingFountain(i);
			break;
		case THEME_ARMORSTAND:
			Theme_ArmorStand(i);
			break;
		case THEME_GOATSHRINE:
			Theme_GoatShrine(i);
			break;
		case THEME_CAULDRON:
			Theme_Cauldron(i);
			break;
		case THEME_MURKYFOUNTAIN:
			Theme_MurkyFountain(i);
			break;
		case THEME_TEARFOUNTAIN:
			Theme_TearFountain(i);
			break;
		case THEME_BRNCROSS:
			Theme_BrnCross(i);
			break;
		case THEME_WEAPONRACK:
			Theme_WeaponRack(i);
			break;
		case THEME_NONE:
			app_fatal("Unknown theme type: %i", themes[i].ttype);
		}
	}
	ApplyObjectLighting = false;
	if (leveltype == DTYPE_HELL && themeCount > 0) {
		UpdateL4Trans();
	}
}

} // namespace devilution
