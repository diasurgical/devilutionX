/**
 * @file objects.cpp
 *
 * Implementation of object functionality, interaction, spawning, loading, etc.
 */
#include <algorithm>
#include <climits>
#include <cstdint>

#include "DiabloUI/ui_flags.hpp"
#include "automap.h"
#include "control.h"
#include "cursor.h"
#ifdef _DEBUG
#include "debug.h"
#endif
#include "drlg_l1.h"
#include "drlg_l4.h"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "error.h"
#include "init.h"
#include "inv.h"
#include "inv_iterators.hpp"
#include "lighting.h"
#include "minitext.h"
#include "missiles.h"
#include "monster.h"
#include "options.h"
#include "setmaps.h"
#include "stores.h"
#include "themes.h"
#include "towners.h"
#include "track.h"
#include "utils/language.h"
#include "utils/log.hpp"

namespace devilution {

Object Objects[MAXOBJECTS];
int AvailableObjects[MAXOBJECTS];
int ActiveObjects[MAXOBJECTS];
int ActiveObjectCount;
bool ApplyObjectLighting;
bool LoadingMapObjects;

namespace {

enum shrine_type : uint8_t {
	ShrineMysterious,
	ShrineHidden,
	ShrineGloomy,
	ShrineWeird,
	ShrineMagical,
	ShrineStone,
	ShrineReligious,
	ShrineEnchanted,
	ShrineThaumaturgic,
	ShrineFascinating,
	ShrineCryptic,
	ShrineMagicaL2,
	ShrineEldritch,
	ShrineEerie,
	ShrineDivine,
	ShrineHoly,
	ShrineSacred,
	ShrineSpiritual,
	ShrineSpooky,
	ShrineAbandoned,
	ShrineCreepy,
	ShrineQuiet,
	ShrineSecluded,
	ShrineOrnate,
	ShrineGlimmering,
	ShrineTainted,
	ShrineOily,
	ShrineGlowing,
	ShrineMendicant,
	ShrineSparkling,
	ShrineTown,
	ShrineShimmering,
	ShrineSolar,
	ShrineMurphys,
	NumberOfShrineTypes
};

int trapid;
int trapdir;
std::unique_ptr<byte[]> pObjCels[40];
object_graphic_id ObjFileList[40];
/** Specifies the number of active objects. */
int leverid;
int numobjfiles;

/** Tracks progress through the tome sequence that spawns Na-Krul (see OperateNakrulBook()) */
int NaKrulTomeSequence;

/** Specifies the X-coordinate delta between barrels. */
int bxadd[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
/** Specifies the Y-coordinate delta between barrels. */
int byadd[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
/** Maps from shrine_id to shrine name. */
const char *const ShrineNames[] = {
	// TRANSLATORS: Shrine Name Block
	N_("Mysterious"),
	N_("Hidden"),
	N_("Gloomy"),
	N_("Weird"),
	N_("Magical"),
	N_("Stone"),
	N_("Religious"),
	N_("Enchanted"),
	N_("Thaumaturgic"),
	N_("Fascinating"),
	N_("Cryptic"),
	N_("Magical"),
	N_("Eldritch"),
	N_("Eerie"),
	N_("Divine"),
	N_("Holy"),
	N_("Sacred"),
	N_("Spiritual"),
	N_("Spooky"),
	N_("Abandoned"),
	N_("Creepy"),
	N_("Quiet"),
	N_("Secluded"),
	N_("Ornate"),
	N_("Glimmering"),
	N_("Tainted"),
	N_("Oily"),
	N_("Glowing"),
	N_("Mendicant's"),
	N_("Sparkling"),
	N_("Town"),
	N_("Shimmering"),
	N_("Solar"),
	// TRANSLATORS: Shrine Name Block end
	N_("Murphy's"),
};
/** Specifies the minimum dungeon level on which each shrine will appear. */
char shrinemin[] = {
	1, // Mysterious
	1, // Hidden
	1, // Gloomy
	1, // Weird
	1, // Magical
	1, // Stone
	1, // Religious
	1, // Enchanted
	1, // Thaumaturgic
	1, // Fascinating
	1, // Cryptic
	1, // Magical
	1, // Eldritch
	1, // Eerie
	1, // Divine
	1, // Holy
	1, // Sacred
	1, // Spiritual
	1, // Spooky
	1, // Abandoned
	1, // Creepy
	1, // Quiet
	1, // Secluded
	1, // Ornate
	1, // Glimmering
	1, // Tainted
	1, // Oily
	1, // Glowing
	1, // Mendicant's
	1, // Sparkling
	1, // Town
	1, // Shimmering
	1, // Solar,
	1, // Murphy's
};

#define MAX_LVLS 24

/** Specifies the maximum dungeon level on which each shrine will appear. */
char shrinemax[] = {
	MAX_LVLS, // Mysterious
	MAX_LVLS, // Hidden
	MAX_LVLS, // Gloomy
	MAX_LVLS, // Weird
	MAX_LVLS, // Magical
	MAX_LVLS, // Stone
	MAX_LVLS, // Religious
	8,        // Enchanted
	MAX_LVLS, // Thaumaturgic
	MAX_LVLS, // Fascinating
	MAX_LVLS, // Cryptic
	MAX_LVLS, // Magical
	MAX_LVLS, // Eldritch
	MAX_LVLS, // Eerie
	MAX_LVLS, // Divine
	MAX_LVLS, // Holy
	MAX_LVLS, // Sacred
	MAX_LVLS, // Spiritual
	MAX_LVLS, // Spooky
	MAX_LVLS, // Abandoned
	MAX_LVLS, // Creepy
	MAX_LVLS, // Quiet
	MAX_LVLS, // Secluded
	MAX_LVLS, // Ornate
	MAX_LVLS, // Glimmering
	MAX_LVLS, // Tainted
	MAX_LVLS, // Oily
	MAX_LVLS, // Glowing
	MAX_LVLS, // Mendicant's
	MAX_LVLS, // Sparkling
	MAX_LVLS, // Town
	MAX_LVLS, // Shimmering
	MAX_LVLS, // Solar,
	MAX_LVLS, // Murphy's
};

/**
 * Specifies the game type for which each shrine may appear.
 * ShrineTypeAny - sp & mp
 * ShrineTypeSingle - sp only
 * ShrineTypeMulti - mp only
 */
enum shrine_gametype : uint8_t {
	ShrineTypeAny,
	ShrineTypeSingle,
	ShrineTypeMulti,
};

shrine_gametype shrineavail[] = {
	ShrineTypeAny,    // Mysterious
	ShrineTypeAny,    // Hidden
	ShrineTypeSingle, // Gloomy
	ShrineTypeSingle, // Weird
	ShrineTypeAny,    // Magical
	ShrineTypeAny,    // Stone
	ShrineTypeAny,    // Religious
	ShrineTypeAny,    // Enchanted
	ShrineTypeSingle, // Thaumaturgic
	ShrineTypeAny,    // Fascinating
	ShrineTypeAny,    // Cryptic
	ShrineTypeAny,    // Magical
	ShrineTypeAny,    // Eldritch
	ShrineTypeAny,    // Eerie
	ShrineTypeAny,    // Divine
	ShrineTypeAny,    // Holy
	ShrineTypeAny,    // Sacred
	ShrineTypeAny,    // Spiritual
	ShrineTypeMulti,  // Spooky
	ShrineTypeAny,    // Abandoned
	ShrineTypeAny,    // Creepy
	ShrineTypeAny,    // Quiet
	ShrineTypeAny,    // Secluded
	ShrineTypeAny,    // Ornate
	ShrineTypeAny,    // Glimmering
	ShrineTypeMulti,  // Tainted
	ShrineTypeAny,    // Oily
	ShrineTypeAny,    // Glowing
	ShrineTypeAny,    // Mendicant's
	ShrineTypeAny,    // Sparkling
	ShrineTypeAny,    // Town
	ShrineTypeAny,    // Shimmering
	ShrineTypeSingle, // Solar,
	ShrineTypeAny,    // Murphy's
};
/** Maps from book_id to book name. */
const char *const StoryBookName[] = {
	N_(/* TRANSLATORS: Book Title */ "The Great Conflict"),
	N_(/* TRANSLATORS: Book Title */ "The Wages of Sin are War"),
	N_(/* TRANSLATORS: Book Title */ "The Tale of the Horadrim"),
	N_(/* TRANSLATORS: Book Title */ "The Dark Exile"),
	N_(/* TRANSLATORS: Book Title */ "The Sin War"),
	N_(/* TRANSLATORS: Book Title */ "The Binding of the Three"),
	N_(/* TRANSLATORS: Book Title */ "The Realms Beyond"),
	N_(/* TRANSLATORS: Book Title */ "Tale of the Three"),
	N_(/* TRANSLATORS: Book Title */ "The Black King"),
	N_(/* TRANSLATORS: Book Title */ "Journal: The Ensorcellment"),
	N_(/* TRANSLATORS: Book Title */ "Journal: The Meeting"),
	N_(/* TRANSLATORS: Book Title */ "Journal: The Tirade"),
	N_(/* TRANSLATORS: Book Title */ "Journal: His Power Grows"),
	N_(/* TRANSLATORS: Book Title */ "Journal: NA-KRUL"),
	N_(/* TRANSLATORS: Book Title */ "Journal: The End"),
	N_(/* TRANSLATORS: Book Title */ "A Spellbook"),
};
/** Specifies the speech IDs of each dungeon type narrator book, for each player class. */
_speech_id StoryText[3][3] = {
	{ TEXT_BOOK11, TEXT_BOOK12, TEXT_BOOK13 },
	{ TEXT_BOOK21, TEXT_BOOK22, TEXT_BOOK23 },
	{ TEXT_BOOK31, TEXT_BOOK32, TEXT_BOOK33 }
};

bool RndLocOk(int xp, int yp)
{
	if (dMonster[xp][yp] != 0)
		return false;
	if (dPlayer[xp][yp] != 0)
		return false;
	if (IsObjectAtPosition({ xp, yp }))
		return false;
	if (TileContainsSetPiece({ xp, yp }))
		return false;
	if (nSolidTable[dPiece[xp][yp]])
		return false;
	return leveltype != DTYPE_CATHEDRAL || dPiece[xp][yp] <= 126 || dPiece[xp][yp] >= 144;
}

bool CanPlaceWallTrap(int xp, int yp)
{
	if (TileContainsSetPiece({ xp, yp }))
		return false;

	return nTrapTable[dPiece[xp][yp]];
}

void InitRndLocObj(int min, int max, _object_id objtype)
{
	int numobjs = GenerateRnd(max - min) + min;

	for (int i = 0; i < numobjs; i++) {
		while (true) {
			int xp = GenerateRnd(80) + 16;
			int yp = GenerateRnd(80) + 16;
			if (RndLocOk(xp - 1, yp - 1)
			    && RndLocOk(xp, yp - 1)
			    && RndLocOk(xp + 1, yp - 1)
			    && RndLocOk(xp - 1, yp)
			    && RndLocOk(xp, yp)
			    && RndLocOk(xp + 1, yp)
			    && RndLocOk(xp - 1, yp + 1)
			    && RndLocOk(xp, yp + 1)
			    && RndLocOk(xp + 1, yp + 1)) {
				AddObject(objtype, { xp, yp });
				break;
			}
		}
	}
}

void InitRndLocBigObj(int min, int max, _object_id objtype)
{
	int numobjs = GenerateRnd(max - min) + min;
	for (int i = 0; i < numobjs; i++) {
		while (true) {
			int xp = GenerateRnd(80) + 16;
			int yp = GenerateRnd(80) + 16;
			if (RndLocOk(xp - 1, yp - 2)
			    && RndLocOk(xp, yp - 2)
			    && RndLocOk(xp + 1, yp - 2)
			    && RndLocOk(xp - 1, yp - 1)
			    && RndLocOk(xp, yp - 1)
			    && RndLocOk(xp + 1, yp - 1)
			    && RndLocOk(xp - 1, yp)
			    && RndLocOk(xp, yp)
			    && RndLocOk(xp + 1, yp)
			    && RndLocOk(xp - 1, yp + 1)
			    && RndLocOk(xp, yp + 1)
			    && RndLocOk(xp + 1, yp + 1)) {
				AddObject(objtype, { xp, yp });
				break;
			}
		}
	}
}

void InitRndLocObj5x5(int min, int max, _object_id objtype)
{
	int numobjs = min + GenerateRnd(max - min);
	for (int i = 0; i < numobjs; i++) {
		int xp;
		int yp;
		int cnt = 0;
		bool exit = false;
		while (!exit) {
			exit = true;
			xp = GenerateRnd(80) + 16;
			yp = GenerateRnd(80) + 16;
			for (int n = -2; n <= 2; n++) {
				for (int m = -2; m <= 2; m++) {
					if (!RndLocOk(xp + m, yp + n))
						exit = false;
				}
			}
			if (!exit) {
				cnt++;
				if (cnt > 20000)
					return;
			}
		}
		AddObject(objtype, { xp, yp });
	}
}

void ClrAllObjects()
{
	memset(Objects, 0, sizeof(Objects));
	ActiveObjectCount = 0;
	for (int i = 0; i < MAXOBJECTS; i++) {
		AvailableObjects[i] = i;
	}
	memset(ActiveObjects, 0, sizeof(ActiveObjects));
	trapdir = 0;
	trapid = 1;
	leverid = 1;
}

void AddTortures()
{
	for (int oy = 0; oy < MAXDUNY; oy++) {
		for (int ox = 0; ox < MAXDUNX; ox++) {
			if (dPiece[ox][oy] == 367) {
				AddObject(OBJ_TORTURE1, { ox, oy + 1 });
				AddObject(OBJ_TORTURE3, { ox + 2, oy - 1 });
				AddObject(OBJ_TORTURE2, { ox, oy + 3 });
				AddObject(OBJ_TORTURE4, { ox + 4, oy - 1 });
				AddObject(OBJ_TORTURE5, { ox, oy + 5 });
				AddObject(OBJ_TNUDEM1, { ox + 1, oy + 3 });
				AddObject(OBJ_TNUDEM2, { ox + 4, oy + 5 });
				AddObject(OBJ_TNUDEM3, { ox + 2, oy });
				AddObject(OBJ_TNUDEM4, { ox + 3, oy + 2 });
				AddObject(OBJ_TNUDEW1, { ox + 2, oy + 4 });
				AddObject(OBJ_TNUDEW2, { ox + 2, oy + 1 });
				AddObject(OBJ_TNUDEW3, { ox + 4, oy + 2 });
			}
		}
	}
}

void AddCandles()
{
	int tx = Quests[Q_PWATER].position.x;
	int ty = Quests[Q_PWATER].position.y;
	AddObject(OBJ_STORYCANDLE, { tx - 2, ty + 1 });
	AddObject(OBJ_STORYCANDLE, { tx + 3, ty + 1 });
	AddObject(OBJ_STORYCANDLE, { tx - 1, ty + 2 });
	AddObject(OBJ_STORYCANDLE, { tx + 2, ty + 2 });
}

/**
 * @brief Attempts to spawn a book somewhere on the current floor which when activated will change a region of the map.
 *
 * This object acts like a lever and will cause a change to the map based on what quest is active. The exact effect is
 * determined by OperateBookLever().
 *
 * @param affectedArea The map region to be updated when this object is activated by the player.
 * @param msg The quest text to play when the player activates the book.
 */
void AddBookLever(Rectangle affectedArea, _speech_id msg)
{
	int cnt = 0;
	int xp;
	int yp;
	bool exit = false;
	while (!exit) {
		exit = true;
		xp = GenerateRnd(80) + 16;
		yp = GenerateRnd(80) + 16;
		for (int n = -2; n <= 2; n++) {
			for (int m = -2; m <= 2; m++) {
				if (!RndLocOk(xp + m, yp + n))
					exit = false;
			}
		}
		if (!exit) {
			cnt++;
			if (cnt > 20000)
				return;
		}
	}

	if (Quests[Q_BLIND].IsAvailable())
		AddObject(OBJ_BLINDBOOK, { xp, yp });
	if (Quests[Q_WARLORD].IsAvailable())
		AddObject(OBJ_STEELTOME, { xp, yp });
	if (Quests[Q_BLOOD].IsAvailable()) {
		xp = 2 * setpc_x + 25;
		yp = 2 * setpc_y + 40;
		AddObject(OBJ_BLOODBOOK, { xp, yp });
	}
	ObjectAtPosition({ xp, yp })->InitializeQuestBook(affectedArea, leverid, msg);
	leverid++;
}

void InitRndBarrels()
{
	/** number of groups of barrels to generate */
	int numobjs = GenerateRnd(5) + 3;
	for (int i = 0; i < numobjs; i++) {
		int xp;
		int yp;
		do {
			xp = GenerateRnd(80) + 16;
			yp = GenerateRnd(80) + 16;
		} while (!RndLocOk(xp, yp));
		_object_id o = (GenerateRnd(4) != 0) ? OBJ_BARREL : OBJ_BARRELEX;
		AddObject(o, { xp, yp });
		bool found = true;
		/** regulates chance to stop placing barrels in current group */
		int p = 0;
		/** number of barrels in current group */
		int c = 1;
		while (GenerateRnd(p) == 0 && found) {
			/** number of tries of placing next barrel in current group */
			int t = 0;
			found = false;
			while (true) {
				if (t >= 3)
					break;
				int dir = GenerateRnd(8);
				xp += bxadd[dir];
				yp += byadd[dir];
				found = RndLocOk(xp, yp);
				t++;
				if (found)
					break;
			}
			if (found) {
				o = (GenerateRnd(5) != 0) ? OBJ_BARREL : OBJ_BARRELEX;
				AddObject(o, { xp, yp });
				c++;
			}
			p = c / 2;
		}
	}
}

void AddCryptObjects(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < y2; j++) {
		for (int i = x1; i < x2; i++) {
			int pn = dPiece[i][j];
			if (pn == 77)
				AddObject(OBJ_L1LDOOR, { i, j });
			if (pn == 80)
				AddObject(OBJ_L1RDOOR, { i, j });
		}
	}
}

void AddL3Objs(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < y2; j++) {
		for (int i = x1; i < x2; i++) {
			int pn = dPiece[i][j];
			if (pn == 531)
				AddObject(OBJ_L3LDOOR, { i, j });
			if (pn == 534)
				AddObject(OBJ_L3RDOOR, { i, j });
		}
	}
}

void AddL2Torches()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			Point testPosition = { i, j };
			if (!TileContainsSetPiece(testPosition))
				continue;

			int pn = dPiece[i][j];
			if (pn == 1 && GenerateRnd(3) == 0) {
				AddObject(OBJ_TORCHL2, testPosition);
			}

			if (pn == 5 && GenerateRnd(3) == 0) {
				AddObject(OBJ_TORCHR2, testPosition);
			}

			if (pn == 37 && GenerateRnd(10) == 0 && !IsObjectAtPosition(testPosition + Direction::NorthWest)) {
				AddObject(OBJ_TORCHL, testPosition + Direction::NorthWest);
			}

			if (pn == 41 && GenerateRnd(10) == 0 && !IsObjectAtPosition(testPosition + Direction::NorthEast)) {
				AddObject(OBJ_TORCHR, testPosition + Direction::NorthEast);
			}
		}
	}
}

void AddObjTraps()
{
	int rndv;
	if (currlevel == 1)
		rndv = 10;
	if (currlevel >= 2)
		rndv = 15;
	if (currlevel >= 5)
		rndv = 20;
	if (currlevel >= 7)
		rndv = 25;
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			Object *triggerObject = ObjectAtPosition({ i, j }, false);
			if (triggerObject == nullptr || GenerateRnd(100) >= rndv)
				continue;

			if (!AllObjects[triggerObject->_otype].oTrapFlag)
				continue;

			Object *trapObject = nullptr;
			if (GenerateRnd(2) == 0) {
				int xp = i - 1;
				while (IsTileNotSolid({ xp, j }))
					xp--;

				if (!CanPlaceWallTrap(xp, j) || i - xp <= 1)
					continue;

				AddObject(OBJ_TRAPL, { xp, j });
				trapObject = ObjectAtPosition({ xp, j });
			} else {
				int yp = j - 1;
				while (IsTileNotSolid({ i, yp }))
					yp--;

				if (!CanPlaceWallTrap(i, yp) || j - yp <= 1)
					continue;

				AddObject(OBJ_TRAPR, { i, yp });
				trapObject = ObjectAtPosition({ i, yp });
			}

			if (trapObject != nullptr) {
				// nullptr check just in case we fail to find a valid location to place a trap in the chosen direction
				trapObject->_oVar1 = i;
				trapObject->_oVar2 = j;
				triggerObject->_oTrapFlag = true;
			}
		}
	}
}

void AddChestTraps()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) { // NOLINT(modernize-loop-convert)
			Object *chestObject = ObjectAtPosition({ i, j }, false);
			if (chestObject != nullptr && chestObject->IsUntrappedChest() && GenerateRnd(100) < 10) {
				switch (chestObject->_otype) {
				case OBJ_CHEST1:
					chestObject->_otype = OBJ_TCHEST1;
					break;
				case OBJ_CHEST2:
					chestObject->_otype = OBJ_TCHEST2;
					break;
				case OBJ_CHEST3:
					chestObject->_otype = OBJ_TCHEST3;
					break;
				default:
					break;
				}
				chestObject->_oTrapFlag = true;
				if (leveltype == DTYPE_CATACOMBS) {
					chestObject->_oVar4 = GenerateRnd(2);
				} else {
					chestObject->_oVar4 = GenerateRnd(gbIsHellfire ? 6 : 3);
				}
			}
		}
	}
}

void LoadMapObjects(const char *path, Point start, Rectangle mapRange, int leveridx)
{
	LoadingMapObjects = true;
	ApplyObjectLighting = true;

	auto dunData = LoadFileInMem<uint16_t>(path);

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	int layer2Offset = 2 + width * height;

	// The rest of the layers are at dPiece scale
	width *= 2;
	height *= 2;

	const uint16_t *objectLayer = &dunData[layer2Offset + width * height * 2];

	start += Displacement { 16, 16 };
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t objectId = SDL_SwapLE16(objectLayer[j * width + i]);
			if (objectId != 0) {
				Point mapPos = start + Displacement { i, j };
				AddObject(ObjTypeConv[objectId], mapPos);
				ObjectAtPosition(mapPos)->InitializeLoadedObject(mapRange, leveridx);
			}
		}
	}

	ApplyObjectLighting = false;
	LoadingMapObjects = false;
}

void LoadMapObjs(const char *path, Point start)
{
	LoadingMapObjects = true;
	ApplyObjectLighting = true;

	auto dunData = LoadFileInMem<uint16_t>(path);

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	int layer2Offset = 2 + width * height;

	// The rest of the layers are at dPiece scale
	width *= 2;
	height *= 2;

	const uint16_t *objectLayer = &dunData[layer2Offset + width * height * 2];

	start += Displacement { 16, 16 };
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t objectId = SDL_SwapLE16(objectLayer[j * width + i]);
			if (objectId != 0) {
				AddObject(ObjTypeConv[objectId], start + Displacement { i, j });
			}
		}
	}

	ApplyObjectLighting = false;
	LoadingMapObjects = false;
}

void AddDiabObjs()
{
	LoadMapObjects("Levels\\L4Data\\diab1.DUN", { 2 * diabquad1x, 2 * diabquad1y }, { { diabquad2x, diabquad2y }, { 11, 12 } }, 1);
	LoadMapObjects("Levels\\L4Data\\diab2a.DUN", { 2 * diabquad2x, 2 * diabquad2y }, { { diabquad3x, diabquad3y }, { 11, 11 } }, 2);
	LoadMapObjects("Levels\\L4Data\\diab3a.DUN", { 2 * diabquad3x, 2 * diabquad3y }, { { diabquad4x, diabquad4y }, { 9, 9 } }, 3);
}

void AddCryptObject(Object &object, int a2)
{
	if (a2 > 5) {
		auto &myPlayer = Players[MyPlayerId];
		switch (a2) {
		case 6:
			switch (myPlayer._pClass) {
			case HeroClass::Warrior:
			case HeroClass::Barbarian:
				object._oVar2 = TEXT_BOOKA;
				break;
			case HeroClass::Rogue:
				object._oVar2 = TEXT_RBOOKA;
				break;
			case HeroClass::Sorcerer:
				object._oVar2 = TEXT_MBOOKA;
				break;
			case HeroClass::Monk:
				object._oVar2 = TEXT_OBOOKA;
				break;
			case HeroClass::Bard:
				object._oVar2 = TEXT_BBOOKA;
				break;
			}
			break;
		case 7:
			switch (myPlayer._pClass) {
			case HeroClass::Warrior:
			case HeroClass::Barbarian:
				object._oVar2 = TEXT_BOOKB;
				break;
			case HeroClass::Rogue:
				object._oVar2 = TEXT_RBOOKB;
				break;
			case HeroClass::Sorcerer:
				object._oVar2 = TEXT_MBOOKB;
				break;
			case HeroClass::Monk:
				object._oVar2 = TEXT_OBOOKB;
				break;
			case HeroClass::Bard:
				object._oVar2 = TEXT_BBOOKB;
				break;
			}
			break;
		case 8:
			switch (myPlayer._pClass) {
			case HeroClass::Warrior:
			case HeroClass::Barbarian:
				object._oVar2 = TEXT_BOOKC;
				break;
			case HeroClass::Rogue:
				object._oVar2 = TEXT_RBOOKC;
				break;
			case HeroClass::Sorcerer:
				object._oVar2 = TEXT_MBOOKC;
				break;
			case HeroClass::Monk:
				object._oVar2 = TEXT_OBOOKC;
				break;
			case HeroClass::Bard:
				object._oVar2 = TEXT_BBOOKC;
				break;
			}
			break;
		}
		object._oVar3 = 15;
		object._oVar8 = a2;
	} else {
		object._oVar2 = a2 + TEXT_SKLJRN;
		object._oVar3 = a2 + 9;
		object._oVar8 = 0;
	}
	object._oVar1 = 1;
	object._oAnimFrame = 5 - 2 * object._oVar1;
	object._oVar4 = object._oAnimFrame + 1;
}

void SetupObject(Object &object, Point position, _object_id ot)
{
	const ObjectData &objectData = AllObjects[ot];
	object._otype = ot;
	object_graphic_id ofi = objectData.ofindex;
	object.position = position;

	const auto &found = std::find(std::begin(ObjFileList), std::end(ObjFileList), ofi);
	if (found == std::end(ObjFileList)) {
		LogCritical("Unable to find object_graphic_id {} in list of objects to load, level generation error.", ofi);
		return;
	}

	const int j = std::distance(std::begin(ObjFileList), found);

	object._oAnimData = pObjCels[j].get();
	object._oAnimFlag = objectData.oAnimFlag;
	if (object._oAnimFlag != 0) {
		object._oAnimDelay = objectData.oAnimDelay;
		object._oAnimCnt = GenerateRnd(object._oAnimDelay);
		object._oAnimLen = objectData.oAnimLen;
		object._oAnimFrame = GenerateRnd(object._oAnimLen - 1) + 1;
	} else {
		object._oAnimDelay = 1000;
		object._oAnimCnt = 0;
		object._oAnimLen = objectData.oAnimLen;
		object._oAnimFrame = objectData.oAnimDelay;
	}
	object._oAnimWidth = objectData.oAnimWidth;
	object._oSolidFlag = objectData.oSolidFlag;
	object._oMissFlag = objectData.oMissFlag;
	object._oLight = objectData.oLightFlag;
	object._oDelFlag = false;
	object._oBreak = objectData.oBreak;
	object._oSelFlag = objectData.oSelFlag;
	object._oPreFlag = false;
	object._oTrapFlag = false;
	object._oDoorFlag = false;
}

void AddCryptBook(_object_id ot, int v2, int ox, int oy)
{
	if (ActiveObjectCount >= MAXOBJECTS)
		return;

	int oi = AvailableObjects[0];
	AvailableObjects[0] = AvailableObjects[MAXOBJECTS - 1 - ActiveObjectCount];
	ActiveObjects[ActiveObjectCount] = oi;
	dObject[ox][oy] = oi + 1;
	Object &object = Objects[oi];
	SetupObject(object, { ox, oy }, ot);
	AddCryptObject(object, v2);
	ActiveObjectCount++;
}

void AddCryptStoryBook(int s)
{
	int cnt = 0;
	int xp;
	int yp;
	bool exit = false;
	while (!exit) {
		exit = true;
		xp = GenerateRnd(80) + 16;
		yp = GenerateRnd(80) + 16;
		for (int n = -2; n <= 2; n++) {
			for (int m = -3; m <= 3; m++) {
				if (!RndLocOk(xp + m, yp + n))
					exit = false;
			}
		}
		if (!exit) {
			cnt++;
			if (cnt > 20000)
				return;
		}
	}
	AddCryptBook(OBJ_STORYBOOK, s, xp, yp);
	AddObject(OBJ_STORYCANDLE, { xp - 2, yp + 1 });
	AddObject(OBJ_STORYCANDLE, { xp - 2, yp });
	AddObject(OBJ_STORYCANDLE, { xp - 1, yp - 1 });
	AddObject(OBJ_STORYCANDLE, { xp + 1, yp - 1 });
	AddObject(OBJ_STORYCANDLE, { xp + 2, yp });
	AddObject(OBJ_STORYCANDLE, { xp + 2, yp + 1 });
}

void AddNakrulBook(int a1, int a2, int a3)
{
	AddCryptBook(OBJ_STORYBOOK, a1, a2, a3);
}

void AddNakrulGate()
{
	AddNakrulLeaver();
	switch (GenerateRnd(6)) {
	case 0:
		AddNakrulBook(6, UberRow + 3, UberCol);
		AddNakrulBook(7, UberRow + 2, UberCol - 3);
		AddNakrulBook(8, UberRow + 2, UberCol + 2);
		break;
	case 1:
		AddNakrulBook(6, UberRow + 3, UberCol);
		AddNakrulBook(8, UberRow + 2, UberCol - 3);
		AddNakrulBook(7, UberRow + 2, UberCol + 2);
		break;
	case 2:
		AddNakrulBook(7, UberRow + 3, UberCol);
		AddNakrulBook(6, UberRow + 2, UberCol - 3);
		AddNakrulBook(8, UberRow + 2, UberCol + 2);
		break;
	case 3:
		AddNakrulBook(7, UberRow + 3, UberCol);
		AddNakrulBook(8, UberRow + 2, UberCol - 3);
		AddNakrulBook(6, UberRow + 2, UberCol + 2);
		break;
	case 4:
		AddNakrulBook(8, UberRow + 3, UberCol);
		AddNakrulBook(7, UberRow + 2, UberCol - 3);
		AddNakrulBook(6, UberRow + 2, UberCol + 2);
		break;
	case 5:
		AddNakrulBook(8, UberRow + 3, UberCol);
		AddNakrulBook(6, UberRow + 2, UberCol - 3);
		AddNakrulBook(7, UberRow + 2, UberCol + 2);
		break;
	}
}

void AddStoryBooks()
{
	int cnt = 0;
	int xp;
	int yp;
	bool done = false;
	while (!done) {
		done = true;
		xp = GenerateRnd(80) + 16;
		yp = GenerateRnd(80) + 16;
		for (int yy = -2; yy <= 2; yy++) {
			for (int xx = -3; xx <= 3; xx++) {
				if (!RndLocOk(xx + xp, yy + yp))
					done = false;
			}
		}
		if (!done) {
			cnt++;
			if (cnt > 20000)
				return;
		}
	}
	AddObject(OBJ_STORYBOOK, { xp, yp });
	AddObject(OBJ_STORYCANDLE, { xp - 2, yp + 1 });
	AddObject(OBJ_STORYCANDLE, { xp - 2, yp });
	AddObject(OBJ_STORYCANDLE, { xp - 1, yp - 1 });
	AddObject(OBJ_STORYCANDLE, { xp + 1, yp - 1 });
	AddObject(OBJ_STORYCANDLE, { xp + 2, yp });
	AddObject(OBJ_STORYCANDLE, { xp + 2, yp + 1 });
}

void AddHookedBodies(int freq)
{
	for (int j = 0; j < DMAXY; j++) {
		int jj = 16 + j * 2;
		for (int i = 0; i < DMAXX; i++) {
			int ii = 16 + i * 2;
			if (dungeon[i][j] != 1 && dungeon[i][j] != 2)
				continue;
			if (GenerateRnd(freq) != 0)
				continue;
			if (!SkipThemeRoom(i, j))
				continue;
			if (dungeon[i][j] == 1 && dungeon[i + 1][j] == 6) {
				switch (GenerateRnd(3)) {
				case 0:
					AddObject(OBJ_TORTURE1, { ii + 1, jj });
					break;
				case 1:
					AddObject(OBJ_TORTURE2, { ii + 1, jj });
					break;
				case 2:
					AddObject(OBJ_TORTURE5, { ii + 1, jj });
					break;
				}
				continue;
			}
			if (dungeon[i][j] == 2 && dungeon[i][j + 1] == 6) {
				switch (GenerateRnd(2)) {
				case 0:
					AddObject(OBJ_TORTURE3, { ii, jj });
					break;
				case 1:
					AddObject(OBJ_TORTURE4, { ii, jj });
					break;
				}
			}
		}
	}
}

void AddL4Goodies()
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

void AddLazStand()
{
	int cnt = 0;
	int xp;
	int yp;
	bool found = false;
	while (!found) {
		found = true;
		xp = GenerateRnd(80) + 16;
		yp = GenerateRnd(80) + 16;
		for (int yy = -3; yy <= 3; yy++) {
			for (int xx = -2; xx <= 3; xx++) {
				if (!RndLocOk(xp + xx, yp + yy))
					found = false;
			}
		}
		if (!found) {
			cnt++;
			if (cnt > 10000) {
				InitRndLocObj(1, 1, OBJ_LAZSTAND);
				return;
			}
		}
	}
	AddObject(OBJ_LAZSTAND, { xp, yp });
	AddObject(OBJ_TNUDEM2, { xp, yp + 2 });
	AddObject(OBJ_STORYCANDLE, { xp + 1, yp + 2 });
	AddObject(OBJ_TNUDEM3, { xp + 2, yp + 2 });
	AddObject(OBJ_TNUDEW1, { xp, yp - 2 });
	AddObject(OBJ_STORYCANDLE, { xp + 1, yp - 2 });
	AddObject(OBJ_TNUDEW2, { xp + 2, yp - 2 });
	AddObject(OBJ_STORYCANDLE, { xp - 1, yp - 1 });
	AddObject(OBJ_TNUDEW3, { xp - 1, yp });
	AddObject(OBJ_STORYCANDLE, { xp - 1, yp + 1 });
}

void DeleteObject(int oi, int i)
{
	int ox = Objects[oi].position.x;
	int oy = Objects[oi].position.y;
	dObject[ox][oy] = 0;
	AvailableObjects[-ActiveObjectCount + MAXOBJECTS] = oi;
	ActiveObjectCount--;
	if (pcursobj == oi) // Unselect object if this was highlighted by player
		pcursobj = -1;
	if (ActiveObjectCount > 0 && i != ActiveObjectCount)
		ActiveObjects[i] = ActiveObjects[ActiveObjectCount];
}

void AddChest(int i, int t)
{
	if (GenerateRnd(2) == 0)
		Objects[i]._oAnimFrame += 3;
	Objects[i]._oRndSeed = AdvanceRndSeed();
	switch (t) {
	case OBJ_CHEST1:
	case OBJ_TCHEST1:
		if (setlevel) {
			Objects[i]._oVar1 = 1;
			break;
		}
		Objects[i]._oVar1 = GenerateRnd(2);
		break;
	case OBJ_TCHEST2:
	case OBJ_CHEST2:
		if (setlevel) {
			Objects[i]._oVar1 = 2;
			break;
		}
		Objects[i]._oVar1 = GenerateRnd(3);
		break;
	case OBJ_TCHEST3:
	case OBJ_CHEST3:
		if (setlevel) {
			Objects[i]._oVar1 = 3;
			break;
		}
		Objects[i]._oVar1 = GenerateRnd(4);
		break;
	}
	Objects[i]._oVar2 = GenerateRnd(8);
}

void ObjSetMicro(Point position, int pn)
{
	dPiece[position.x][position.y] = pn;
	pn--;

	int blocks = leveltype != DTYPE_HELL ? 10 : 16;

	uint16_t *piece = &pLevelPieces[blocks * pn];
	MICROS &micros = dpiece_defs_map_2[position.x][position.y];

	for (int i = 0; i < blocks; i++) {
		micros.mt[i] = SDL_SwapLE16(piece[blocks - 2 + (i & 1) - (i & 0xE)]);
	}
}

void InitializeL1Door(Object &door)
{
	door.InitializeDoor();
	door._oVar1 = dPiece[door.position.x][door.position.y];
	if (door._otype == _object_id::OBJ_L1LDOOR) {
		door._oVar2 = dPiece[door.position.x][door.position.y - 1];
	} else { // _object_id::OBJ_L1RDOOR
		door._oVar2 = dPiece[door.position.x - 1][door.position.y];
	}
}

void InitializeMicroDoor(Object &door)
{
	door.InitializeDoor();
	int pieceNumber;
	switch (door._otype) {
	case _object_id::OBJ_L2LDOOR:
		pieceNumber = 538;
		break;
	case _object_id::OBJ_L2RDOOR:
		pieceNumber = 540;
		break;
	case _object_id::OBJ_L3LDOOR:
		pieceNumber = 531;
		break;
	case _object_id::OBJ_L3RDOOR:
		pieceNumber = 534;
		break;
	default:
		return; // unreachable
	}
	ObjSetMicro(door.position, pieceNumber);
}

void AddSarc(int i)
{
	dObject[Objects[i].position.x][Objects[i].position.y - 1] = -(i + 1);
	Objects[i]._oVar1 = GenerateRnd(10);
	Objects[i]._oRndSeed = AdvanceRndSeed();
	if (Objects[i]._oVar1 >= 8)
		Objects[i]._oVar2 = PreSpawnSkeleton();
}

void AddFlameTrap(int i)
{
	Objects[i]._oVar1 = trapid;
	Objects[i]._oVar2 = 0;
	Objects[i]._oVar3 = trapdir;
	Objects[i]._oVar4 = 0;
}

void AddFlameLvr(int i)
{
	Objects[i]._oVar1 = trapid;
	Objects[i]._oVar2 = MIS_FLAMEC;
}

void AddTrap(int i)
{
	int mt = currlevel / 3 + 1;
	if (currlevel > 16) {
		mt = (currlevel - 4) / 3 + 1;
	}
	if (currlevel > 20) {
		mt = (currlevel - 8) / 3 + 1;
	}
	mt = GenerateRnd(mt);
	if (mt == 0)
		Objects[i]._oVar3 = MIS_ARROW;
	if (mt == 1)
		Objects[i]._oVar3 = MIS_FIREBOLT;
	if (mt == 2)
		Objects[i]._oVar3 = MIS_LIGHTCTRL;
	Objects[i]._oVar4 = 0;
}

void AddObjectLight(int i, int r)
{
	if (ApplyObjectLighting) {
		DoLighting(Objects[i].position, r, -1);
		Objects[i]._oVar1 = -1;
	} else {
		Objects[i]._oVar1 = 0;
	}
}

void AddBarrel(int i, int t)
{
	Objects[i]._oVar1 = 0;
	Objects[i]._oRndSeed = AdvanceRndSeed();
	Objects[i]._oVar2 = (t == OBJ_BARRELEX) ? 0 : GenerateRnd(10);
	Objects[i]._oVar3 = GenerateRnd(3);

	if (Objects[i]._oVar2 >= 8)
		Objects[i]._oVar4 = PreSpawnSkeleton();
}

void AddShrine(int i)
{
	bool slist[NumberOfShrineTypes];

	Objects[i]._oPreFlag = true;

	int shrines = gbIsHellfire ? NumberOfShrineTypes : 26;

	for (int j = 0; j < shrines; j++) {
		slist[j] = currlevel >= shrinemin[j] && currlevel <= shrinemax[j];
		if (gbIsMultiplayer && shrineavail[j] == ShrineTypeSingle) {
			slist[j] = false;
		} else if (!gbIsMultiplayer && shrineavail[j] == ShrineTypeMulti) {
			slist[j] = false;
		}
	}

	int val;
	do {
		val = GenerateRnd(shrines);
	} while (!slist[val]);

	Objects[i]._oVar1 = val;
	if (GenerateRnd(2) != 0) {
		Objects[i]._oAnimFrame = 12;
		Objects[i]._oAnimLen = 22;
	}
}

void AddBookcase(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
	Objects[i]._oPreFlag = true;
}

void AddBookstand(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddBloodFtn(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddPurifyingFountain(int i)
{
	int ox = Objects[i].position.x;
	int oy = Objects[i].position.y;
	dObject[ox][oy - 1] = -(i + 1);
	dObject[ox - 1][oy] = -(i + 1);
	dObject[ox - 1][oy - 1] = -(i + 1);
	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddArmorStand(int i)
{
	if (!armorFlag) {
		Objects[i]._oAnimFlag = 2;
		Objects[i]._oSelFlag = 0;
	}

	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddGoatShrine(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddCauldron(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddMurkyFountain(int i)
{
	int ox = Objects[i].position.x;
	int oy = Objects[i].position.y;
	dObject[ox][oy - 1] = -(i + 1);
	dObject[ox - 1][oy] = -(i + 1);
	dObject[ox - 1][oy - 1] = -(i + 1);
	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddTearFountain(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddDecap(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
	Objects[i]._oAnimFrame = GenerateRnd(8) + 1;
	Objects[i]._oPreFlag = true;
}

void AddVilebook(int i)
{
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		Objects[i]._oAnimFrame = 4;
	}
}

void AddMagicCircle(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
	Objects[i]._oPreFlag = true;
	Objects[i]._oVar6 = 0;
	Objects[i]._oVar5 = 1;
}

void AddBrnCross(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddPedistal(int i)
{
	Objects[i]._oVar1 = setpc_x;
	Objects[i]._oVar2 = setpc_y;
	Objects[i]._oVar3 = setpc_x + setpc_w;
	Objects[i]._oVar4 = setpc_y + setpc_h;
	Objects[i]._oVar6 = 0;
}

void AddStoryBook(int i)
{
	SetRndSeed(glSeedTbl[16]);

	Objects[i]._oVar1 = GenerateRnd(3);
	if (currlevel == 4)
		Objects[i]._oVar2 = StoryText[Objects[i]._oVar1][0];
	else if (currlevel == 8)
		Objects[i]._oVar2 = StoryText[Objects[i]._oVar1][1];
	else if (currlevel == 12)
		Objects[i]._oVar2 = StoryText[Objects[i]._oVar1][2];
	Objects[i]._oVar3 = (currlevel / 4) + 3 * Objects[i]._oVar1 - 1;
	Objects[i]._oAnimFrame = 5 - 2 * Objects[i]._oVar1;
	Objects[i]._oVar4 = Objects[i]._oAnimFrame + 1;
}

void AddWeaponRack(int i)
{
	if (!weaponFlag) {
		Objects[i]._oAnimFlag = 2;
		Objects[i]._oSelFlag = 0;
	}
	Objects[i]._oRndSeed = AdvanceRndSeed();
}

void AddTorturedBody(int i)
{
	Objects[i]._oRndSeed = AdvanceRndSeed();
	Objects[i]._oAnimFrame = GenerateRnd(4) + 1;
	Objects[i]._oPreFlag = true;
}

void GetRndObjLoc(int randarea, int *xx, int *yy)
{
	if (randarea == 0)
		return;

	int tries = 0;
	while (true) {
		tries++;
		if (tries > 1000 && randarea > 1)
			randarea--;
		*xx = GenerateRnd(MAXDUNX);
		*yy = GenerateRnd(MAXDUNY);
		bool failed = false;
		for (int i = 0; i < randarea && !failed; i++) {
			for (int j = 0; j < randarea && !failed; j++) {
				failed = !RndLocOk(i + *xx, j + *yy);
			}
		}
		if (!failed)
			break;
	}
}

void AddMushPatch()
{
	int y;
	int x;

	if (ActiveObjectCount < MAXOBJECTS) {
		int i = AvailableObjects[0];
		GetRndObjLoc(5, &x, &y);
		dObject[x + 1][y + 1] = -(i + 1);
		dObject[x + 2][y + 1] = -(i + 1);
		dObject[x + 1][y + 2] = -(i + 1);
		AddObject(OBJ_MUSHPATCH, { x + 2, y + 2 });
	}
}

void UpdateObjectLight(int i, int lightRadius)
{
	if (Objects[i]._oVar1 == -1) {
		return;
	}

	bool turnon = false;
	int ox = Objects[i].position.x;
	int oy = Objects[i].position.y;
	int tr = lightRadius + 10;
	if (!DisableLighting) {
		for (int p = 0; p < MAX_PLRS && !turnon; p++) {
			if (Players[p].plractive) {
				if (currlevel == Players[p].plrlevel) {
					int dx = abs(Players[p].position.tile.x - ox);
					int dy = abs(Players[p].position.tile.y - oy);
					if (dx < tr && dy < tr)
						turnon = true;
				}
			}
		}
	}
	if (turnon) {
		if (Objects[i]._oVar1 == 0)
			Objects[i]._olid = AddLight(Objects[i].position, lightRadius);
		Objects[i]._oVar1 = 1;
	} else {
		if (Objects[i]._oVar1 == 1)
			AddUnLight(Objects[i]._olid);
		Objects[i]._oVar1 = 0;
	}
}

void UpdateCircle(int i)
{
	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer.position.tile != Objects[i].position) {
		if (Objects[i]._otype == OBJ_MCIRCLE1)
			Objects[i]._oAnimFrame = 1;
		if (Objects[i]._otype == OBJ_MCIRCLE2)
			Objects[i]._oAnimFrame = 3;
		Objects[i]._oVar6 = 0;
		return;
	}

	int ox = Objects[i].position.x;
	int oy = Objects[i].position.y;
	if (Objects[i]._otype == OBJ_MCIRCLE1)
		Objects[i]._oAnimFrame = 2;
	if (Objects[i]._otype == OBJ_MCIRCLE2)
		Objects[i]._oAnimFrame = 4;
	if (ox == 45 && oy == 47) {
		Objects[i]._oVar6 = 2;
	} else if (ox == 26 && oy == 46) {
		Objects[i]._oVar6 = 1;
	} else {
		Objects[i]._oVar6 = 0;
	}
	if (ox == 35 && oy == 36 && Objects[i]._oVar5 == 3) {
		Objects[i]._oVar6 = 4;
		ObjChangeMapResync(Objects[i]._oVar1, Objects[i]._oVar2, Objects[i]._oVar3, Objects[i]._oVar4);
		if (Quests[Q_BETRAYER]._qactive == QUEST_ACTIVE && Quests[Q_BETRAYER]._qvar1 <= 4) // BUGFIX stepping on the circle again will break the quest state (fixed)
			Quests[Q_BETRAYER]._qvar1 = 4;
		AddMissile(myPlayer.position.tile, { 35, 46 }, Direction::South, MIS_RNDTELEPORT, TARGET_BOTH, MyPlayerId, 0, 0);
		LastMouseButtonAction = MouseActionType::None;
		sgbMouseDown = CLICK_NONE;
		ClrPlrPath(myPlayer);
		StartStand(MyPlayerId, Direction::South);
	}
}

void ObjectStopAnim(int i)
{
	if (Objects[i]._oAnimFrame == Objects[i]._oAnimLen) {
		Objects[i]._oAnimCnt = 0;
		Objects[i]._oAnimDelay = 1000;
	}
}

void UpdateDoor(int i)
{
	if (Objects[i]._oVar4 == 0) {
		Objects[i]._oSelFlag = 3;
		Objects[i]._oMissFlag = false;
		return;
	}

	int dx = Objects[i].position.x;
	int dy = Objects[i].position.y;
	bool dok = dMonster[dx][dy] == 0;
	dok = dok && dItem[dx][dy] == 0;
	dok = dok && dCorpse[dx][dy] == 0;
	dok = dok && dPlayer[dx][dy] == 0;
	Objects[i]._oSelFlag = 2;
	Objects[i]._oVar4 = dok ? 1 : 2;
	Objects[i]._oMissFlag = true;
}

void UpdateSarcoffagus(int i)
{
	if (Objects[i]._oAnimFrame == Objects[i]._oAnimLen)
		Objects[i]._oAnimFlag = 0;
}

void ActivateTrapLine(int ttype, int tid)
{
	for (int i = 0; i < ActiveObjectCount; i++) {
		int oi = ActiveObjects[i];
		if (Objects[oi]._otype == ttype && Objects[oi]._oVar1 == tid) {
			Objects[oi]._oVar4 = 1;
			Objects[oi]._oAnimFlag = 1;
			Objects[oi]._oAnimDelay = 1;
			Objects[oi]._olid = AddLight(Objects[oi].position, 1);
		}
	}
}

void UpdateFlameTrap(int i)
{
	if (Objects[i]._oVar2 != 0) {
		if (Objects[i]._oVar4 != 0) {
			Objects[i]._oAnimFrame--;
			if (Objects[i]._oAnimFrame == 1) {
				Objects[i]._oVar4 = 0;
				AddUnLight(Objects[i]._olid);
			} else if (Objects[i]._oAnimFrame <= 4) {
				ChangeLightRadius(Objects[i]._olid, Objects[i]._oAnimFrame);
			}
		}
	} else if (Objects[i]._oVar4 == 0) {
		if (Objects[i]._oVar3 == 2) {
			int x = Objects[i].position.x - 2;
			int y = Objects[i].position.y;
			for (int j = 0; j < 5; j++) {
				if (dPlayer[x][y] != 0 || dMonster[x][y] != 0)
					Objects[i]._oVar4 = 1;
				x++;
			}
		} else {
			int x = Objects[i].position.x;
			int y = Objects[i].position.y - 2;
			for (int k = 0; k < 5; k++) {
				if (dPlayer[x][y] != 0 || dMonster[x][y] != 0)
					Objects[i]._oVar4 = 1;
				y++;
			}
		}
		if (Objects[i]._oVar4 != 0)
			ActivateTrapLine(Objects[i]._otype, Objects[i]._oVar1);
	} else {
		int damage[4] = { 6, 8, 10, 12 };

		int mindam = damage[leveltype - 1];
		int maxdam = mindam * 2;

		int x = Objects[i].position.x;
		int y = Objects[i].position.y;
		if (dMonster[x][y] > 0)
			MonsterTrapHit(dMonster[x][y] - 1, mindam / 2, maxdam / 2, 0, MIS_FIREWALLC, false);
		if (dPlayer[x][y] > 0) {
			bool unused;
			PlayerMHit(dPlayer[x][y] - 1, nullptr, 0, mindam, maxdam, MIS_FIREWALLC, false, 0, &unused);
		}

		if (Objects[i]._oAnimFrame == Objects[i]._oAnimLen)
			Objects[i]._oAnimFrame = 11;
		if (Objects[i]._oAnimFrame <= 5)
			ChangeLightRadius(Objects[i]._olid, Objects[i]._oAnimFrame);
	}
}

void UpdateBurningCrossDamage(int i)
{
	int damage[4] = { 6, 8, 10, 12 };

	auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pmode == PM_DEATH)
		return;

	int8_t fireResist = myPlayer._pFireResist;
	if (fireResist > 0)
		damage[leveltype - 1] -= fireResist * damage[leveltype - 1] / 100;

	if (myPlayer.position.tile.x != Objects[i].position.x || myPlayer.position.tile.y != Objects[i].position.y - 1)
		return;

	ApplyPlrDamage(MyPlayerId, 0, 0, damage[leveltype - 1]);
	if (myPlayer._pHitPoints >> 6 > 0) {
		myPlayer.Say(HeroSpeech::Argh);
	}
}

void ObjSetMini(Point position, int v)
{
	MegaTile mega = pMegaTiles[v - 1];

	Point megaOrigin = position * 2 + Displacement { 16, 16 };

	ObjSetMicro(megaOrigin, SDL_SwapLE16(mega.micro1) + 1);
	ObjSetMicro(megaOrigin + Direction::SouthEast, SDL_SwapLE16(mega.micro2) + 1);
	ObjSetMicro(megaOrigin + Direction::SouthWest, SDL_SwapLE16(mega.micro3) + 1);
	ObjSetMicro(megaOrigin + Direction::South, SDL_SwapLE16(mega.micro4) + 1);
}

void ObjL1Special(int x1, int y1, int x2, int y2)
{
	for (int i = y1; i <= y2; ++i) {
		for (int j = x1; j <= x2; ++j) {
			dSpecial[j][i] = 0;
			if (dPiece[j][i] == 12)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 11)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 71)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 253)
				dSpecial[j][i] = 3;
			if (dPiece[j][i] == 267)
				dSpecial[j][i] = 6;
			if (dPiece[j][i] == 259)
				dSpecial[j][i] = 5;
			if (dPiece[j][i] == 249)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 325)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 321)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 255)
				dSpecial[j][i] = 4;
			if (dPiece[j][i] == 211)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 344)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 341)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 331)
				dSpecial[j][i] = 2;
			if (dPiece[j][i] == 418)
				dSpecial[j][i] = 1;
			if (dPiece[j][i] == 421)
				dSpecial[j][i] = 2;
		}
	}
}

void ObjL2Special(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j <= y2; j++) {
		for (int i = x1; i <= x2; i++) {
			dSpecial[i][j] = 0;
			if (dPiece[i][j] == 541)
				dSpecial[i][j] = 5;
			if (dPiece[i][j] == 178)
				dSpecial[i][j] = 5;
			if (dPiece[i][j] == 551)
				dSpecial[i][j] = 5;
			if (dPiece[i][j] == 542)
				dSpecial[i][j] = 6;
			if (dPiece[i][j] == 553)
				dSpecial[i][j] = 6;
		}
	}
	for (int j = y1; j <= y2; j++) {
		for (int i = x1; i <= x2; i++) {
			if (dPiece[i][j] == 132) {
				dSpecial[i][j + 1] = 2;
				dSpecial[i][j + 2] = 1;
			}
			if (dPiece[i][j] == 135 || dPiece[i][j] == 139) {
				dSpecial[i + 1][j] = 3;
				dSpecial[i + 2][j] = 4;
			}
		}
	}
}

void SetDoorPiece(Point position)
{
	int pn = dPiece[position.x][position.y] - 1;

	uint16_t *piece = &pLevelPieces[10 * pn + 8];

	dpiece_defs_map_2[position.x][position.y].mt[0] = SDL_SwapLE16(piece[0]);
	dpiece_defs_map_2[position.x][position.y].mt[1] = SDL_SwapLE16(piece[1]);
}

void DoorSet(Point position, bool isLeftDoor)
{
	int pn = dPiece[position.x][position.y];
	if (currlevel < 17) {
		switch (pn) {
		case 43:
			ObjSetMicro(position, 392);
			break;
		case 45:
			ObjSetMicro(position, 394);
			break;
		case 50:
			ObjSetMicro(position, isLeftDoor ? 411 : 412);
			break;
		case 54:
			ObjSetMicro(position, 397);
			break;
		case 55:
			ObjSetMicro(position, 398);
			break;
		case 61:
			ObjSetMicro(position, 399);
			break;
		case 67:
			ObjSetMicro(position, 400);
			break;
		case 68:
			ObjSetMicro(position, 401);
			break;
		case 69:
			ObjSetMicro(position, 403);
			break;
		case 70:
			ObjSetMicro(position, 404);
			break;
		case 72:
			ObjSetMicro(position, 406);
			break;
		case 212:
			ObjSetMicro(position, 407);
			break;
		case 354:
			ObjSetMicro(position, 409);
			break;
		case 355:
			ObjSetMicro(position, 410);
			break;
		case 411:
		case 412:
			ObjSetMicro(position, 396);
			break;
		}
	} else {
		switch (pn) {
		case 75:
			ObjSetMicro(position, 204);
			break;
		case 79:
			ObjSetMicro(position, 208);
			break;
		case 86:
			ObjSetMicro(position, isLeftDoor ? 232 : 234);
			break;
		case 91:
			ObjSetMicro(position, 215);
			break;
		case 93:
			ObjSetMicro(position, 218);
			break;
		case 99:
			ObjSetMicro(position, 220);
			break;
		case 111:
			ObjSetMicro(position, 222);
			break;
		case 113:
			ObjSetMicro(position, 224);
			break;
		case 115:
			ObjSetMicro(position, 226);
			break;
		case 117:
			ObjSetMicro(position, 228);
			break;
		case 119:
			ObjSetMicro(position, 230);
			break;
		case 232:
		case 234:
			ObjSetMicro(position, 212);
			break;
		}
	}
}

/**
 * @brief Checks if an open door can be closed
 *
 * In order to be able to close a door the space where the closed door would be must be free of bodies, monsters, and items
 *
 * @param doorPos Map tile where the door is in its closed position
 * @return true if the door is free to be closed, false if anything is blocking it
 */
inline bool IsDoorClear(const Point &doorPosition)
{
	return dCorpse[doorPosition.x][doorPosition.y] == 0
	    && dMonster[doorPosition.x][doorPosition.y] == 0
	    && dItem[doorPosition.x][doorPosition.y] == 0;
}

void OperateL1RDoor(int pnum, int oi, bool sendflag)
{
	Object &door = Objects[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (currlevel < 21) {
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, door.position);
			ObjSetMicro(door.position, 395);
		} else {
			if (!deltaload)
				PlaySfxLoc(IS_CROPEN, door.position);
			ObjSetMicro(door.position, 209);
		}
		if (currlevel < 17) {
			dSpecial[door.position.x][door.position.y] = 8;
		} else {
			dSpecial[door.position.x][door.position.y] = 2;
		}
		SetDoorPiece(door.position + Direction::NorthEast);
		door._oAnimFrame += 2;
		door._oPreFlag = true;
		DoorSet(door.position + Direction::NorthWest, false);
		door._oVar4 = 1;
		door._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (currlevel < 21) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
	} else {
		if (!deltaload)
			PlaySfxLoc(IS_CRCLOS, door.position);
	}
	if (!deltaload && IsDoorClear(door.position)) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position, door._oVar1);

		// Restore the normal tile where the open door used to be
		auto openPosition = door.position + Direction::NorthWest;
		if (currlevel < 17) {
			if (door._oVar2 == 50 && dPiece[openPosition.x][openPosition.y] == 396)
				ObjSetMicro(openPosition, 411);
			else
				ObjSetMicro(openPosition, door._oVar2);
		} else {
			if (door._oVar2 == 86 && dPiece[openPosition.x][openPosition.y] == 210)
				ObjSetMicro(openPosition, 232);
			else
				ObjSetMicro(openPosition, door._oVar2);
		}

		dSpecial[door.position.x][door.position.y] = 0;
		door._oAnimFrame -= 2;
		door._oPreFlag = false;
		RedoPlayerVision();
	} else {
		door._oVar4 = 2;
	}
}

void OperateL1LDoor(int pnum, int oi, bool sendflag)
{
	Object &door = Objects[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (currlevel < 21) {
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, door.position);
			if (door._oVar1 == 214)
				ObjSetMicro(door.position, 408);
			else
				ObjSetMicro(door.position, 393);
		} else {
			if (!deltaload)
				PlaySfxLoc(IS_CROPEN, door.position);
			ObjSetMicro(door.position, 206);
		}
		if (currlevel < 17) {
			dSpecial[door.position.x][door.position.y] = 7;
		} else {
			dSpecial[door.position.x][door.position.y] = 1;
		}
		SetDoorPiece(door.position + Direction::NorthWest);
		door._oAnimFrame += 2;
		door._oPreFlag = true;
		DoorSet(door.position + Direction::NorthEast, true);
		door._oVar4 = 1;
		door._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (currlevel < 21) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
	} else {
		if (!deltaload)
			PlaySfxLoc(IS_CRCLOS, door.position);
	}
	if (IsDoorClear(door.position)) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position, door._oVar1);

		// Restore the normal tile where the open door used to be
		auto openPosition = door.position + Direction::NorthEast;
		if (currlevel < 17) {
			if (door._oVar2 == 50 && dPiece[openPosition.x][openPosition.y] == 396)
				ObjSetMicro(openPosition, 412);
			else
				ObjSetMicro(openPosition, door._oVar2);
		} else {
			if (door._oVar2 == 86 && dPiece[openPosition.x][openPosition.y] == 210)
				ObjSetMicro(openPosition, 234);
			else
				ObjSetMicro(openPosition, door._oVar2);
		}

		dSpecial[door.position.x][door.position.y] = 0;
		door._oAnimFrame -= 2;
		door._oPreFlag = false;
		RedoPlayerVision();
	} else {
		door._oVar4 = 2;
	}
}

void OperateL2RDoor(int pnum, int oi, bool sendflag)
{
	Object &door = Objects[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, door.position);
		ObjSetMicro(door.position, 17);
		dSpecial[door.position.x][door.position.y] = 6;
		door._oAnimFrame += 2;
		door._oPreFlag = true;
		door._oVar4 = 1;
		door._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, door.position);

	if (IsDoorClear(door.position)) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position, 540);
		dSpecial[door.position.x][door.position.y] = 0;
		door._oAnimFrame -= 2;
		door._oPreFlag = false;
		RedoPlayerVision();
	} else {
		door._oVar4 = 2;
	}
}

void OperateL2LDoor(int pnum, int oi, bool sendflag)
{
	Object &door = Objects[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, door.position);
		ObjSetMicro(door.position, 13);
		dSpecial[door.position.x][door.position.y] = 5;
		door._oAnimFrame += 2;
		door._oPreFlag = true;
		door._oVar4 = 1;
		door._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, door.position);

	if (IsDoorClear(door.position)) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position, 538);
		dSpecial[door.position.x][door.position.y] = 0;
		door._oAnimFrame -= 2;
		door._oPreFlag = false;
		RedoPlayerVision();
	} else {
		door._oVar4 = 2;
	}
}

void OperateL3RDoor(int pnum, int oi, bool sendflag)
{
	Object &door = Objects[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, door.position);
		ObjSetMicro(door.position, 541);
		door._oAnimFrame += 2;
		door._oPreFlag = true;
		door._oVar4 = 1;
		door._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, door.position);

	if (IsDoorClear(door.position)) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position, 534);
		door._oAnimFrame -= 2;
		door._oPreFlag = false;
		RedoPlayerVision();
	} else {
		door._oVar4 = 2;
	}
}

void OperateL3LDoor(int pnum, int oi, bool sendflag)
{
	Object &door = Objects[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, door.position);
		ObjSetMicro(door.position, 538);
		door._oAnimFrame += 2;
		door._oPreFlag = true;
		door._oVar4 = 1;
		door._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, door.position);

	if (IsDoorClear(door.position)) {
		if (pnum == MyPlayerId && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position, 531);
		door._oAnimFrame -= 2;
		door._oPreFlag = false;
		RedoPlayerVision();
	} else {
		door._oVar4 = 2;
	}
}

void OperateL1Door(int pnum, int i, bool sendflag)
{
	int dpx = abs(Objects[i].position.x - Players[pnum].position.tile.x);
	int dpy = abs(Objects[i].position.y - Players[pnum].position.tile.y);
	if (dpx == 1 && dpy <= 1 && Objects[i]._otype == OBJ_L1LDOOR)
		OperateL1LDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && Objects[i]._otype == OBJ_L1RDOOR)
		OperateL1RDoor(pnum, i, sendflag);
}

bool AreAllLeversActivated(int leverId)
{
	for (int j = 0; j < ActiveObjectCount; j++) {
		int oi = ActiveObjects[j];
		if (Objects[oi]._otype == OBJ_SWITCHSKL
		    && Objects[oi]._oVar8 == leverId
		    && Objects[oi]._oSelFlag != 0) {
			return false;
		}
	}
	return true;
}

void OperateLever(int pnum, int i)
{
	Object &object = Objects[i];
	if (object._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_LEVER, object.position);
	object._oSelFlag = 0;
	object._oAnimFrame++;
	bool mapflag = true;
	if (currlevel == 16 && !AreAllLeversActivated(object._oVar8))
		mapflag = false;
	if (currlevel == 24) {
		OperateNakrulLever();
		IsUberLeverActivated = true;
		mapflag = false;
		Quests[Q_NAKRUL]._qactive = QUEST_DONE;
	}
	if (mapflag)
		ObjChangeMap(object._oVar1, object._oVar2, object._oVar3, object._oVar4);
	if (pnum == MyPlayerId)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateBook(int pnum, Object &book)
{
	if (book._oSelFlag == 0) {
		return;
	}

	auto &player = Players[pnum];

	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		bool missileAdded = false;
		for (int j = 0; j < ActiveObjectCount; j++) {
			Object &questObject = Objects[ActiveObjects[j]];

			Point target {};
			bool doAddMissile = false;

			if (questObject._otype == OBJ_MCIRCLE2 && questObject._oVar6 == 1) {
				target = { 27, 29 };
				doAddMissile = true;
			}
			if (questObject._otype == OBJ_MCIRCLE2 && questObject._oVar6 == 2) {
				target = { 43, 29 };
				doAddMissile = true;
			}

			if (doAddMissile) {
				questObject._oVar6 = 4;
				ObjectAtPosition({ 35, 36 })->_oVar5++;
				AddMissile(player.position.tile, target, Direction::South, MIS_RNDTELEPORT, TARGET_BOTH, pnum, 0, 0);
				missileAdded = true;
			}
		}
		if (!missileAdded) {
			return;
		}
	}

	book._oSelFlag = 0;
	book._oAnimFrame++;

	if (!setlevel) {
		return;
	}

	if (setlvlnum == SL_BONECHAMB) {
		player._pMemSpells |= GetSpellBitmask(SPL_GUARDIAN);
		if (player._pSplLvl[SPL_GUARDIAN] < MAX_SPELL_LEVEL)
			player._pSplLvl[SPL_GUARDIAN]++;
		Quests[Q_SCHAMB]._qactive = QUEST_DONE;
		if (!deltaload)
			PlaySfxLoc(IS_QUESTDN, book.position);
		InitDiabloMsg(EMSG_BONECHAMB);
		AddMissile(
		    player.position.tile,
		    book.position + Displacement { -2, -4 },
		    player._pdir,
		    MIS_GUARDIAN,
		    TARGET_MONSTERS,
		    pnum,
		    0,
		    0);
	}
	if (setlvlnum == SL_VILEBETRAYER) {
		ObjChangeMapResync(
		    book._oVar1,
		    book._oVar2,
		    book._oVar3,
		    book._oVar4);
		for (int j = 0; j < ActiveObjectCount; j++)
			SyncObjectAnim(Objects[ActiveObjects[j]]);
	}
}

void OperateBookLever(int pnum, int i)
{
	int x = 2 * setpc_x + 16;
	int y = 2 * setpc_y + 16;
	if (ActiveItemCount >= MAXITEMS) {
		return;
	}
	if (Objects[i]._oSelFlag != 0 && !qtextflag) {
		if (Objects[i]._otype == OBJ_BLINDBOOK && Quests[Q_BLIND]._qvar1 == 0) {
			Quests[Q_BLIND]._qactive = QUEST_ACTIVE;
			Quests[Q_BLIND]._qlog = true;
			Quests[Q_BLIND]._qvar1 = 1;
		}
		if (Objects[i]._otype == OBJ_BLOODBOOK && Quests[Q_BLOOD]._qvar1 == 0) {
			Quests[Q_BLOOD]._qactive = QUEST_ACTIVE;
			Quests[Q_BLOOD]._qlog = true;
			Quests[Q_BLOOD]._qvar1 = 1;
			SpawnQuestItem(IDI_BLDSTONE, { 2 * setpc_x + 25, 2 * setpc_y + 33 }, 0, 1);
		}
		if (Objects[i]._otype == OBJ_STEELTOME && Quests[Q_WARLORD]._qvar1 == 0) {
			Quests[Q_WARLORD]._qactive = QUEST_ACTIVE;
			Quests[Q_WARLORD]._qlog = true;
			Quests[Q_WARLORD]._qvar1 = 1;
		}
		if (Objects[i]._oAnimFrame != Objects[i]._oVar6) {
			if (Objects[i]._otype != OBJ_BLOODBOOK)
				ObjChangeMap(Objects[i]._oVar1, Objects[i]._oVar2, Objects[i]._oVar3, Objects[i]._oVar4);
			if (Objects[i]._otype == OBJ_BLINDBOOK) {
				SpawnUnique(UITEM_OPTAMULET, Point { x, y } + Displacement { 5, 5 });
				auto tren = TransVal;
				TransVal = 9;
				DRLG_MRectTrans(Objects[i]._oVar1, Objects[i]._oVar2, Objects[i]._oVar3, Objects[i]._oVar4);
				TransVal = tren;
			}
		}
		Objects[i]._oAnimFrame = Objects[i]._oVar6;
		InitQTextMsg(Objects[i].bookMessage);
		if (pnum == MyPlayerId)
			NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
	}
}

void OperateChamberOfBoneBook(Object &questBook)
{
	if (questBook._oSelFlag == 0 || qtextflag) {
		return;
	}

	if (questBook._oAnimFrame != questBook._oVar6) {
		ObjChangeMapResync(questBook._oVar1, questBook._oVar2, questBook._oVar3, questBook._oVar4);
		for (int j = 0; j < ActiveObjectCount; j++) {
			SyncObjectAnim(Objects[ActiveObjects[j]]);
		}
	}
	questBook._oAnimFrame = questBook._oVar6;
	if (Quests[Q_SCHAMB]._qactive == QUEST_INIT) {
		Quests[Q_SCHAMB]._qactive = QUEST_ACTIVE;
		Quests[Q_SCHAMB]._qlog = true;
	}

	_speech_id textdef;
	switch (Players[MyPlayerId]._pClass) {
	case HeroClass::Warrior:
		textdef = TEXT_BONER;
		break;
	case HeroClass::Rogue:
		textdef = TEXT_RBONER;
		break;
	case HeroClass::Sorcerer:
		textdef = TEXT_MBONER;
		break;
	case HeroClass::Monk:
		textdef = TEXT_HBONER;
		break;
	case HeroClass::Bard:
		textdef = TEXT_BBONER;
		break;
	case HeroClass::Barbarian:
		textdef = TEXT_BONER;
		break;
	}
	Quests[Q_SCHAMB]._qmsg = textdef;
	InitQTextMsg(textdef);
}

void OperateChest(int pnum, int i, bool sendmsg)
{
	if (Objects[i]._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_CHEST, Objects[i].position);
	Objects[i]._oSelFlag = 0;
	Objects[i]._oAnimFrame += 2;
	if (deltaload) {
		return;
	}
	SetRndSeed(Objects[i]._oRndSeed);
	if (setlevel) {
		for (int j = 0; j < Objects[i]._oVar1; j++) {
			CreateRndItem(Objects[i].position, true, sendmsg, false);
		}
	} else {
		for (int j = 0; j < Objects[i]._oVar1; j++) {
			if (Objects[i]._oVar2 != 0)
				CreateRndItem(Objects[i].position, false, sendmsg, false);
			else
				CreateRndUseful(Objects[i].position, sendmsg);
		}
	}
	if (Objects[i].IsTrappedChest()) {
		auto &player = Players[pnum];
		Direction mdir = GetDirection(Objects[i].position, player.position.tile);
		missile_id mtype;
		switch (Objects[i]._oVar4) {
		case 0:
			mtype = MIS_ARROW;
			break;
		case 1:
			mtype = MIS_FARROW;
			break;
		case 2:
			mtype = MIS_NOVA;
			break;
		case 3:
			mtype = MIS_FIRERING;
			break;
		case 4:
			mtype = MIS_STEALPOTS;
			break;
		case 5:
			mtype = MIS_MANATRAP;
			break;
		default:
			mtype = MIS_ARROW;
		}
		AddMissile(Objects[i].position, player.position.tile, mdir, mtype, TARGET_PLAYERS, -1, 0, 0);
		Objects[i]._oTrapFlag = false;
	}
	if (pnum == MyPlayerId)
		NetSendCmdParam2(false, CMD_PLROPOBJ, pnum, i);
}

void OperateMushroomPatch(int pnum, Object &questContainer)
{
	if (ActiveItemCount >= MAXITEMS) {
		return;
	}

	if (Quests[Q_MUSHROOM]._qactive != QUEST_ACTIVE) {
		if (!deltaload && pnum == MyPlayerId) {
			Players[pnum].Say(HeroSpeech::ICantUseThisYet);
		}
		return;
	}

	if (questContainer._oSelFlag == 0) {
		return;
	}

	questContainer._oSelFlag = 0;
	questContainer._oAnimFrame++;

	if (!deltaload) {
		PlaySfxLoc(IS_CHEST, questContainer.position);
		Point pos = GetSuperItemLoc(questContainer.position);
		SpawnQuestItem(IDI_MUSHROOM, pos, 0, 0);
		Quests[Q_MUSHROOM]._qvar1 = QS_MUSHSPAWNED;
	}
}

void OperateInnSignChest(int pnum, Object &questContainer)
{
	if (ActiveItemCount >= MAXITEMS) {
		return;
	}

	if (Quests[Q_LTBANNER]._qvar1 != 2) {
		if (!deltaload && pnum == MyPlayerId) {
			Players[pnum].Say(HeroSpeech::ICantOpenThisYet);
		}
		return;
	}

	if (questContainer._oSelFlag == 0) {
		return;
	}

	questContainer._oSelFlag = 0;
	questContainer._oAnimFrame += 2;

	if (!deltaload) {
		PlaySfxLoc(IS_CHEST, questContainer.position);
		Point pos = GetSuperItemLoc(questContainer.position);
		SpawnQuestItem(IDI_BANNER, pos, 0, 0);
	}
}

void OperateSlainHero(int pnum, int i)
{
	if (Objects[i]._oSelFlag == 0) {
		return;
	}
	Objects[i]._oSelFlag = 0;
	if (deltaload) {
		return;
	}

	auto &player = Players[pnum];

	if (player._pClass == HeroClass::Warrior) {
		CreateMagicArmor(Objects[i].position, ItemType::HeavyArmor, ICURS_BREAST_PLATE, false, true);
	} else if (player._pClass == HeroClass::Rogue) {
		CreateMagicWeapon(Objects[i].position, ItemType::Bow, ICURS_LONG_WAR_BOW, false, true);
	} else if (player._pClass == HeroClass::Sorcerer) {
		CreateSpellBook(Objects[i].position, SPL_LIGHTNING, false, true);
	} else if (player._pClass == HeroClass::Monk) {
		CreateMagicWeapon(Objects[i].position, ItemType::Staff, ICURS_WAR_STAFF, false, true);
	} else if (player._pClass == HeroClass::Bard) {
		CreateMagicWeapon(Objects[i].position, ItemType::Sword, ICURS_BASTARD_SWORD, false, true);
	} else if (player._pClass == HeroClass::Barbarian) {
		CreateMagicWeapon(Objects[i].position, ItemType::Axe, ICURS_BATTLE_AXE, false, true);
	}
	Players[MyPlayerId].Say(HeroSpeech::RestInPeaceMyFriend);
	if (pnum == MyPlayerId)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateTrapLever(Object &flameLever)
{
	if (!deltaload) {
		PlaySfxLoc(IS_LEVER, flameLever.position);
	}

	if (flameLever._oAnimFrame == 1) {
		flameLever._oAnimFrame = 2;
		for (int j = 0; j < ActiveObjectCount; j++) {
			Object &target = Objects[ActiveObjects[j]];
			if (target._otype == flameLever._oVar2 && target._oVar1 == flameLever._oVar1) {
				target._oVar2 = 1;
				target._oAnimFlag = 0;
			}
		}
		return;
	}

	flameLever._oAnimFrame--;
	for (int j = 0; j < ActiveObjectCount; j++) {
		Object &target = Objects[ActiveObjects[j]];
		if (target._otype == flameLever._oVar2 && target._oVar1 == flameLever._oVar1) {
			target._oVar2 = 0;
			if (target._oVar4 != 0) {
				target._oAnimFlag = 1;
			}
		}
	}
}

void OperateSarc(int pnum, int i, bool sendmsg)
{
	if (Objects[i]._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_SARC, Objects[i].position);
	Objects[i]._oSelFlag = 0;
	if (deltaload) {
		Objects[i]._oAnimFrame = Objects[i]._oAnimLen;
		return;
	}
	Objects[i]._oAnimFlag = 1;
	Objects[i]._oAnimDelay = 3;
	SetRndSeed(Objects[i]._oRndSeed);
	if (Objects[i]._oVar1 <= 2)
		CreateRndItem(Objects[i].position, false, sendmsg, false);
	if (Objects[i]._oVar1 >= 8)
		SpawnSkeleton(Objects[i]._oVar2, Objects[i].position);
	if (pnum == MyPlayerId)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateL2Door(int pnum, int i, bool sendflag)
{
	int dpx = abs(Objects[i].position.x - Players[pnum].position.tile.x);
	int dpy = abs(Objects[i].position.y - Players[pnum].position.tile.y);
	if (dpx == 1 && dpy <= 1 && Objects[i]._otype == OBJ_L2LDOOR)
		OperateL2LDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && Objects[i]._otype == OBJ_L2RDOOR)
		OperateL2RDoor(pnum, i, sendflag);
}

void OperateL3Door(int pnum, int i, bool sendflag)
{
	int dpx = abs(Objects[i].position.x - Players[pnum].position.tile.x);
	int dpy = abs(Objects[i].position.y - Players[pnum].position.tile.y);
	if (dpx == 1 && dpy <= 1 && Objects[i]._otype == OBJ_L3RDOOR)
		OperateL3RDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && Objects[i]._otype == OBJ_L3LDOOR)
		OperateL3LDoor(pnum, i, sendflag);
}

void OperatePedistal(int pnum, int i)
{
	if (ActiveItemCount >= MAXITEMS) {
		return;
	}

	if (Objects[i]._oVar6 == 3 || !Players[pnum].TryRemoveInvItemById(IDI_BLDSTONE)) {
		return;
	}

	Objects[i]._oAnimFrame++;
	Objects[i]._oVar6++;
	if (Objects[i]._oVar6 == 1) {
		if (!deltaload)
			PlaySfxLoc(LS_PUDDLE, Objects[i].position);
		ObjChangeMap(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
		SpawnQuestItem(IDI_BLDSTONE, { 2 * setpc_x + 19, 2 * setpc_y + 26 }, 0, 1);
	}
	if (Objects[i]._oVar6 == 2) {
		if (!deltaload)
			PlaySfxLoc(LS_PUDDLE, Objects[i].position);
		ObjChangeMap(setpc_x + 6, setpc_y + 3, setpc_x + setpc_w, setpc_y + 7);
		SpawnQuestItem(IDI_BLDSTONE, { 2 * setpc_x + 31, 2 * setpc_y + 26 }, 0, 1);
	}
	if (Objects[i]._oVar6 == 3) {
		if (!deltaload)
			PlaySfxLoc(LS_BLODSTAR, Objects[i].position);
		ObjChangeMap(Objects[i]._oVar1, Objects[i]._oVar2, Objects[i]._oVar3, Objects[i]._oVar4);
		LoadMapObjs("Levels\\L2Data\\Blood2.DUN", { 2 * setpc_x, 2 * setpc_y });
		SpawnUnique(UITEM_ARMOFVAL, Point { setpc_x, setpc_y } * 2 + Displacement { 25, 19 });
		Objects[i]._oSelFlag = 0;
	}
}

bool OperateShrineMysterious(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	ModifyPlrStr(pnum, -1);
	ModifyPlrMag(pnum, -1);
	ModifyPlrDex(pnum, -1);
	ModifyPlrVit(pnum, -1);

	switch (static_cast<CharacterAttribute>(GenerateRnd(4))) {
	case CharacterAttribute::Strength:
		ModifyPlrStr(pnum, 6);
		break;
	case CharacterAttribute::Magic:
		ModifyPlrMag(pnum, 6);
		break;
	case CharacterAttribute::Dexterity:
		ModifyPlrDex(pnum, 6);
		break;
	case CharacterAttribute::Vitality:
		ModifyPlrVit(pnum, 6);
		break;
	}

	CheckStats(Players[pnum]);

	InitDiabloMsg(EMSG_SHRINE_MYSTERIOUS);

	return true;
}

bool OperateShrineHidden(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &player = Players[pnum];

	int cnt = 0;
	for (const auto &item : player.InvBody) {
		if (!item.isEmpty())
			cnt++;
	}
	if (cnt > 0) {
		for (auto &item : player.InvBody) {
			if (!item.isEmpty()
			    && item._iMaxDur != DUR_INDESTRUCTIBLE
			    && item._iMaxDur != 0) {
				item._iDurability += 10;
				item._iMaxDur += 10;
				if (item._iDurability > item._iMaxDur)
					item._iDurability = item._iMaxDur;
			}
		}
		while (true) {
			cnt = 0;
			for (auto &item : player.InvBody) {
				if (!item.isEmpty() && item._iMaxDur != DUR_INDESTRUCTIBLE && item._iMaxDur != 0) {
					cnt++;
				}
			}
			if (cnt == 0)
				break;
			int r = GenerateRnd(NUM_INVLOC);
			if (player.InvBody[r].isEmpty() || player.InvBody[r]._iMaxDur == DUR_INDESTRUCTIBLE || player.InvBody[r]._iMaxDur == 0)
				continue;

			player.InvBody[r]._iDurability -= 20;
			player.InvBody[r]._iMaxDur -= 20;
			if (player.InvBody[r]._iDurability <= 0)
				player.InvBody[r]._iDurability = 1;
			if (player.InvBody[r]._iMaxDur <= 0)
				player.InvBody[r]._iMaxDur = 1;
			break;
		}
	}

	InitDiabloMsg(EMSG_SHRINE_HIDDEN);

	return true;
}

bool OperateShrineGloomy(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return true;

	auto &player = Players[pnum];

	// Increment armor class by 2 and decrements max damage by 1.
	for (Item &item : PlayerItemsRange(player)) {
		switch (item._itype) {
		case ItemType::Sword:
		case ItemType::Axe:
		case ItemType::Bow:
		case ItemType::Mace:
		case ItemType::Staff:
			item._iMaxDam--;
			if (item._iMaxDam < item._iMinDam)
				item._iMaxDam = item._iMinDam;
			break;
		case ItemType::Shield:
		case ItemType::Helm:
		case ItemType::LightArmor:
		case ItemType::MediumArmor:
		case ItemType::HeavyArmor:
			item._iAC += 2;
			break;
		default:
			break;
		}
	}

	InitDiabloMsg(EMSG_SHRINE_GLOOMY);

	return true;
}

bool OperateShrineWeird(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return true;

	auto &player = Players[pnum];

	if (!player.InvBody[INVLOC_HAND_LEFT].isEmpty() && player.InvBody[INVLOC_HAND_LEFT]._itype != ItemType::Shield)
		player.InvBody[INVLOC_HAND_LEFT]._iMaxDam++;
	if (!player.InvBody[INVLOC_HAND_RIGHT].isEmpty() && player.InvBody[INVLOC_HAND_RIGHT]._itype != ItemType::Shield)
		player.InvBody[INVLOC_HAND_RIGHT]._iMaxDam++;

	for (Item &item : InventoryPlayerItemsRange { player }) {
		switch (item._itype) {
		case ItemType::Sword:
		case ItemType::Axe:
		case ItemType::Bow:
		case ItemType::Mace:
		case ItemType::Staff:
			item._iMaxDam++;
			break;
		default:
			break;
		}
	}

	InitDiabloMsg(EMSG_SHRINE_WEIRD);

	return true;
}

bool OperateShrineMagical(int pnum)
{
	if (deltaload)
		return false;

	auto &player = Players[pnum];

	AddMissile(
	    player.position.tile,
	    player.position.tile,
	    player._pdir,
	    MIS_MANASHIELD,
	    TARGET_PLAYERS,
	    pnum,
	    0,
	    2 * leveltype);

	if (pnum != MyPlayerId)
		return false;

	InitDiabloMsg(EMSG_SHRINE_MAGICAL);

	return true;
}

bool OperateShrineStone(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return true;

	for (Item &item : PlayerItemsRange { Players[pnum] }) {
		if (item._itype == ItemType::Staff)
			item._iCharges = item._iMaxCharges; // belt items don't have charges?
	}

	InitDiabloMsg(EMSG_SHRINE_STONE);

	return true;
}

bool OperateShrineReligious(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return true;

	for (Item &item : PlayerItemsRange { Players[pnum] }) {
		item._iDurability = item._iMaxDur; // belt items don't have durability?
	}

	InitDiabloMsg(EMSG_SHRINE_RELIGIOUS);

	return true;
}

bool OperateShrineEnchanted(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &player = Players[pnum];

	int cnt = 0;
	uint64_t spell = 1;
	int maxSpells = gbIsHellfire ? MAX_SPELLS : 37;
	uint64_t spells = player._pMemSpells;
	for (int j = 0; j < maxSpells; j++) {
		if ((spell & spells) != 0)
			cnt++;
		spell *= 2;
	}
	if (cnt > 1) {
		spell = 1;
		for (int j = SPL_FIREBOLT; j < maxSpells; j++) { // BUGFIX: < MAX_SPELLS, there is no spell with MAX_SPELLS index (fixed)
			if ((player._pMemSpells & spell) != 0) {
				if (player._pSplLvl[j] < MAX_SPELL_LEVEL)
					player._pSplLvl[j]++;
			}
			spell *= 2;
		}
		int r;
		do {
			r = GenerateRnd(maxSpells);
		} while ((player._pMemSpells & GetSpellBitmask(r + 1)) == 0);
		if (player._pSplLvl[r + 1] >= 2)
			player._pSplLvl[r + 1] -= 2;
		else
			player._pSplLvl[r + 1] = 0;
	}

	InitDiabloMsg(EMSG_SHRINE_ENCHANTED);

	return true;
}

bool OperateShrineThaumaturgic(int pnum)
{
	for (int j = 0; j < ActiveObjectCount; j++) {
		int v1 = ActiveObjects[j];
		assert(v1 >= 0 && v1 < MAXOBJECTS);
		if (Objects[v1].IsChest() && Objects[v1]._oSelFlag == 0) {
			Objects[v1]._oRndSeed = AdvanceRndSeed();
			Objects[v1]._oSelFlag = 1;
			Objects[v1]._oAnimFrame -= 2;
		}
	}

	if (deltaload)
		return false;

	if (pnum != MyPlayerId)
		return true;

	InitDiabloMsg(EMSG_SHRINE_THAUMATURGIC);

	return true;
}

bool OperateShrineFascinating(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &player = Players[pnum];

	player._pMemSpells |= GetSpellBitmask(SPL_FIREBOLT);

	if (player._pSplLvl[SPL_FIREBOLT] < MAX_SPELL_LEVEL)
		player._pSplLvl[SPL_FIREBOLT]++;
	if (player._pSplLvl[SPL_FIREBOLT] < MAX_SPELL_LEVEL)
		player._pSplLvl[SPL_FIREBOLT]++;

	DWORD t = player._pMaxManaBase / 10;
	int v1 = player._pMana - player._pManaBase;
	int v2 = player._pMaxMana - player._pMaxManaBase;
	player._pManaBase -= t;
	player._pMana -= t;
	player._pMaxMana -= t;
	player._pMaxManaBase -= t;
	if (player._pMana >> 6 <= 0) {
		player._pMana = v1;
		player._pManaBase = 0;
	}
	if (player._pMaxMana >> 6 <= 0) {
		player._pMaxMana = v2;
		player._pMaxManaBase = 0;
	}

	InitDiabloMsg(EMSG_SHRINE_FASCINATING);

	return true;
}

bool OperateShrineCryptic(int pnum)
{
	if (deltaload)
		return false;

	auto &player = Players[pnum];

	AddMissile(
	    player.position.tile,
	    player.position.tile,
	    player._pdir,
	    MIS_NOVA,
	    TARGET_PLAYERS,
	    pnum,
	    0,
	    2 * leveltype);

	if (pnum != MyPlayerId)
		return false;

	player._pMana = player._pMaxMana;
	player._pManaBase = player._pMaxManaBase;

	InitDiabloMsg(EMSG_SHRINE_CRYPTIC);

	return true;
}

bool OperateShrineEldritch(int pnum)
{
	/// BUGFIX: change `plr[pnum].HoldItem` to use a temporary buffer to prevent deleting item in hand
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return true;

	auto &player = Players[pnum];

	for (Item &item : InventoryAndBeltPlayerItemsRange { player }) {
		if (item._itype == ItemType::Misc) {
			if (item._iMiscId == IMISC_HEAL
			    || item._iMiscId == IMISC_MANA) {
				SetPlrHandItem(player.HoldItem, ItemMiscIdIdx(IMISC_REJUV));
				GetPlrHandSeed(&player.HoldItem);
				player.HoldItem._iStatFlag = true;
				item = player.HoldItem;
			}
			if (item._iMiscId == IMISC_FULLHEAL
			    || item._iMiscId == IMISC_FULLMANA) {
				SetPlrHandItem(player.HoldItem, ItemMiscIdIdx(IMISC_FULLREJUV));
				GetPlrHandSeed(&player.HoldItem);
				player.HoldItem._iStatFlag = true;
				item = player.HoldItem;
			}
		}
	}

	InitDiabloMsg(EMSG_SHRINE_ELDRITCH);

	return true;
}

bool OperateShrineEerie(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	ModifyPlrMag(pnum, 2);
	CheckStats(Players[pnum]);

	InitDiabloMsg(EMSG_SHRINE_EERIE);

	return true;
}

/**
 * @brief Fully restores HP and Mana of the active player and spawns a pair of potions
 *        in response to the player activating a Divine shrine
 * @param pnum The player that activated the shrine
 * @param spawnPosition The map tile where the potions will be spawned
 * @return false if the shrine was activated by another player in a multiplayer game and
 *               no changes were made by this instance, true otherwise.
 */
bool OperateShrineDivine(int pnum, Point spawnPosition)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &player = Players[pnum];

	if (currlevel < 4) {
		CreateTypeItem(spawnPosition, false, ItemType::Misc, IMISC_FULLMANA, false, true);
		CreateTypeItem(spawnPosition, false, ItemType::Misc, IMISC_FULLHEAL, false, true);
	} else {
		CreateTypeItem(spawnPosition, false, ItemType::Misc, IMISC_FULLREJUV, false, true);
		CreateTypeItem(spawnPosition, false, ItemType::Misc, IMISC_FULLREJUV, false, true);
	}

	player._pMana = player._pMaxMana;
	player._pManaBase = player._pMaxManaBase;
	player._pHitPoints = player._pMaxHP;
	player._pHPBase = player._pMaxHPBase;

	InitDiabloMsg(EMSG_SHRINE_DIVINE);

	return true;
}

bool OperateShrineHoly(int pnum)
{
	if (deltaload)
		return false;

	AddMissile(Players[pnum].position.tile, { 0, 0 }, Direction::South, MIS_RNDTELEPORT, TARGET_PLAYERS, pnum, 0, 2 * leveltype);

	if (pnum != MyPlayerId)
		return false;

	InitDiabloMsg(EMSG_SHRINE_HOLY);

	return true;
}

bool OperateShrineSacred(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &player = Players[pnum];

	player._pMemSpells |= GetSpellBitmask(SPL_CBOLT);

	if (player._pSplLvl[SPL_CBOLT] < MAX_SPELL_LEVEL)
		player._pSplLvl[SPL_CBOLT]++;
	if (player._pSplLvl[SPL_CBOLT] < MAX_SPELL_LEVEL)
		player._pSplLvl[SPL_CBOLT]++;

	uint32_t t = player._pMaxManaBase / 10;
	int v1 = player._pMana - player._pManaBase;
	int v2 = player._pMaxMana - player._pMaxManaBase;
	player._pManaBase -= t;
	player._pMana -= t;
	player._pMaxMana -= t;
	player._pMaxManaBase -= t;
	if (player._pMana >> 6 <= 0) {
		player._pMana = v1;
		player._pManaBase = 0;
	}
	if (player._pMaxMana >> 6 <= 0) {
		player._pMaxMana = v2;
		player._pMaxManaBase = 0;
	}

	InitDiabloMsg(EMSG_SHRINE_SACRED);

	return true;
}

bool OperateShrineSpiritual(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &player = Players[pnum];

	for (int8_t &gridItem : player.InvGrid) {
		if (gridItem == 0) {
			int r = 5 * leveltype + GenerateRnd(10 * leveltype);
			DWORD t = player._pNumInv; // check
			player.InvList[t] = golditem;
			player.InvList[t]._iSeed = AdvanceRndSeed();
			player._pNumInv++;
			gridItem = player._pNumInv;
			player.InvList[t]._ivalue = r;
			player._pGold += r;
			SetPlrHandGoldCurs(player.InvList[t]);
		}
	}

	InitDiabloMsg(EMSG_SHRINE_SPIRITUAL);

	return true;
}

bool OperateShrineSpooky(int pnum)
{
	if (deltaload)
		return false;

	if (pnum == MyPlayerId) {
		InitDiabloMsg(EMSG_SHRINE_SPOOKY1);
		return true;
	}

	auto &myPlayer = Players[MyPlayerId];

	myPlayer._pHitPoints = myPlayer._pMaxHP;
	myPlayer._pHPBase = myPlayer._pMaxHPBase;
	myPlayer._pMana = myPlayer._pMaxMana;
	myPlayer._pManaBase = myPlayer._pMaxManaBase;

	InitDiabloMsg(EMSG_SHRINE_SPOOKY2);

	return true;
}

bool OperateShrineAbandoned(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	ModifyPlrDex(pnum, 2);
	CheckStats(Players[pnum]);

	if (pnum != MyPlayerId)
		return true;

	InitDiabloMsg(EMSG_SHRINE_ABANDONED);

	return true;
}

bool OperateShrineCreepy(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	ModifyPlrStr(pnum, 2);
	CheckStats(Players[pnum]);

	if (pnum != MyPlayerId)
		return true;

	InitDiabloMsg(EMSG_SHRINE_CREEPY);

	return true;
}

bool OperateShrineQuiet(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	ModifyPlrVit(pnum, 2);
	CheckStats(Players[pnum]);

	if (pnum != MyPlayerId)
		return true;

	InitDiabloMsg(EMSG_SHRINE_QUIET);

	return true;
}

bool OperateShrineSecluded(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return true;

	for (int x = 0; x < DMAXX; x++)
		for (int y = 0; y < DMAXY; y++)
			UpdateAutomapExplorer({ x, y }, MAP_EXP_SHRINE);

	InitDiabloMsg(EMSG_SHRINE_SECLUDED);

	return true;
}

bool OperateShrineOrnate(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &player = Players[pnum];

	player._pMemSpells |= GetSpellBitmask(SPL_HBOLT);
	if (player._pSplLvl[SPL_HBOLT] < MAX_SPELL_LEVEL)
		player._pSplLvl[SPL_HBOLT]++;
	if (player._pSplLvl[SPL_HBOLT] < MAX_SPELL_LEVEL)
		player._pSplLvl[SPL_HBOLT]++;

	uint32_t t = player._pMaxManaBase / 10;
	int v1 = player._pMana - player._pManaBase;
	int v2 = player._pMaxMana - player._pMaxManaBase;
	player._pManaBase -= t;
	player._pMana -= t;
	player._pMaxMana -= t;
	player._pMaxManaBase -= t;
	if (player._pMana >> 6 <= 0) {
		player._pMana = v1;
		player._pManaBase = 0;
	}
	if (player._pMaxMana >> 6 <= 0) {
		player._pMaxMana = v2;
		player._pMaxManaBase = 0;
	}

	InitDiabloMsg(EMSG_SHRINE_ORNATE);

	return true;
}

bool OperateShrineGlimmering(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	for (Item &item : PlayerItemsRange { Players[pnum] }) {
		if (item._iMagical != ITEM_QUALITY_NORMAL && !item._iIdentified) {
			item._iIdentified = true; // belt items can't be magical?
		}
	}

	InitDiabloMsg(EMSG_SHRINE_GLIMMERING);

	return true;
}

bool OperateShrineTainted(int pnum)
{
	if (deltaload)
		return false;

	if (pnum == MyPlayerId) {
		InitDiabloMsg(EMSG_SHRINE_TAINTED1);
		return true;
	}

	int r = GenerateRnd(4);

	int v1 = r == 0 ? 1 : -1;
	int v2 = r == 1 ? 1 : -1;
	int v3 = r == 2 ? 1 : -1;
	int v4 = r == 3 ? 1 : -1;

	ModifyPlrStr(MyPlayerId, v1);
	ModifyPlrMag(MyPlayerId, v2);
	ModifyPlrDex(MyPlayerId, v3);
	ModifyPlrVit(MyPlayerId, v4);

	CheckStats(Players[MyPlayerId]);

	InitDiabloMsg(EMSG_SHRINE_TAINTED2);

	return true;
}

/**
 * @brief Oily shrines increase the players primary stat(s) by a total of two, but spawn a
 *        firewall near the shrine that will spread towards the player
 * @param pnum The player that activated the shrine
 * @param spawnPosition Start location for the firewall
 * @return false if the current player did not activate the shrine (i.e. it's a multiplayer
 *         game) and we bailed early to avoid doubling the effects, true otherwise.
 */
bool OperateShrineOily(int pnum, Point spawnPosition)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &myPlayer = Players[MyPlayerId];

	switch (myPlayer._pClass) {
	case HeroClass::Warrior:
		ModifyPlrStr(MyPlayerId, 2);
		break;
	case HeroClass::Rogue:
		ModifyPlrDex(MyPlayerId, 2);
		break;
	case HeroClass::Sorcerer:
		ModifyPlrMag(MyPlayerId, 2);
		break;
	case HeroClass::Barbarian:
		ModifyPlrVit(MyPlayerId, 2);
		break;
	case HeroClass::Monk:
		ModifyPlrStr(MyPlayerId, 1);
		ModifyPlrDex(MyPlayerId, 1);
		break;
	case HeroClass::Bard:
		ModifyPlrDex(MyPlayerId, 1);
		ModifyPlrMag(MyPlayerId, 1);
		break;
	}

	CheckStats(Players[pnum]);

	AddMissile(
	    spawnPosition,
	    myPlayer.position.tile,
	    myPlayer._pdir,
	    MIS_FIREWALL,
	    TARGET_PLAYERS,
	    -1,
	    2 * currlevel + 2,
	    0);

	InitDiabloMsg(EMSG_SHRINE_OILY);

	return true;
}

bool OperateShrineGlowing(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &myPlayer = Players[MyPlayerId];

	// Add 0-5 points to Magic (0.1% of the players XP)
	ModifyPlrMag(MyPlayerId, static_cast<int>(std::min<uint32_t>(myPlayer._pExperience / 1000, 5)));

	// Take 5% of the players experience to offset the bonus, unless they're very low level in which case take all their experience.
	if (myPlayer._pExperience > 5000)
		myPlayer._pExperience = static_cast<uint32_t>(myPlayer._pExperience * 0.95);
	else
		myPlayer._pExperience = 0;

	if (*sgOptions.Gameplay.experienceBar)
		force_redraw = 255;

	CheckStats(Players[pnum]);

	InitDiabloMsg(EMSG_SHRINE_GLOWING);

	return true;
}

bool OperateShrineMendicant(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &myPlayer = Players[MyPlayerId];

	int gold = myPlayer._pGold / 2;
	AddPlrExperience(MyPlayerId, myPlayer._pLevel, gold);
	TakePlrsMoney(gold);

	CheckStats(Players[pnum]);

	InitDiabloMsg(EMSG_SHRINE_MENDICANT);

	return true;
}

/**
 * @brief Grants experience to the player based on their current level while also triggering a magic trap
 * @param pnum The player that activated the shrine
 * @param spawnPosition The trap results in casting flash from this location targeting the player
 * @return false if the current player didn't activate the shrine (to avoid doubling the effect), true otherwise
 */
bool OperateShrineSparkling(int pnum, Point spawnPosition)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &myPlayer = Players[MyPlayerId];

	AddPlrExperience(MyPlayerId, myPlayer._pLevel, 1000 * currlevel);

	AddMissile(
	    spawnPosition,
	    myPlayer.position.tile,
	    myPlayer._pdir,
	    MIS_FLASH,
	    TARGET_PLAYERS,
	    -1,
	    3 * currlevel + 2,
	    0);

	CheckStats(Players[pnum]);

	InitDiabloMsg(EMSG_SHRINE_SPARKLING);

	return true;
}

/**
 * @brief Spawns a town portal near the active player
 * @param pnum The player that activated the shrine
 * @param spawnPosition The position of the shrine, the portal will be placed on the side closest to the player
 * @return false if the current player didn't activate the shrine (to avoid doubling the effect), true otherwise
 */
bool OperateShrineTown(int pnum, Point spawnPosition)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &myPlayer = Players[MyPlayerId];

	AddMissile(
	    spawnPosition,
	    myPlayer.position.tile,
	    myPlayer._pdir,
	    MIS_TOWN,
	    TARGET_PLAYERS,
	    pnum,
	    0,
	    0);

	InitDiabloMsg(EMSG_SHRINE_TOWN);

	return true;
}

bool OperateShrineShimmering(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &player = Players[pnum];

	player._pMana = player._pMaxMana;
	player._pManaBase = player._pMaxManaBase;

	InitDiabloMsg(EMSG_SHRINE_SHIMMERING);

	return true;
}

bool OperateShrineSolar(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	time_t tm = time(nullptr);
	int hour = localtime(&tm)->tm_hour;
	if (hour >= 20 || hour < 4) {
		InitDiabloMsg(EMSG_SHRINE_SOLAR4);
		ModifyPlrVit(MyPlayerId, 2);
	} else if (hour >= 18) {
		InitDiabloMsg(EMSG_SHRINE_SOLAR3);
		ModifyPlrMag(MyPlayerId, 2);
	} else if (hour >= 12) {
		InitDiabloMsg(EMSG_SHRINE_SOLAR2);
		ModifyPlrStr(MyPlayerId, 2);
	} else /* 4:00 to 11:59 */ {
		InitDiabloMsg(EMSG_SHRINE_SOLAR1);
		ModifyPlrDex(MyPlayerId, 2);
	}

	CheckStats(Players[pnum]);

	return true;
}

bool OperateShrineMurphys(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != MyPlayerId)
		return false;

	auto &myPlayer = Players[MyPlayerId];

	bool broke = false;
	for (auto &item : myPlayer.InvBody) {
		if (!item.isEmpty() && GenerateRnd(3) == 0) {
			if (item._iDurability != DUR_INDESTRUCTIBLE) {
				if (item._iDurability > 0) {
					item._iDurability /= 2;
					broke = true;
					break;
				}
			}
		}
	}
	if (!broke) {
		TakePlrsMoney(myPlayer._pGold / 3);
	}

	InitDiabloMsg(EMSG_SHRINE_MURPHYS);

	return true;
}

void OperateShrine(int pnum, int i, _sfx_id sType)
{
	if (dropGoldFlag) {
		CloseGoldDrop();
		dropGoldValue = 0;
	}

	assert(i >= 0 && i < MAXOBJECTS);

	if (Objects[i]._oSelFlag == 0)
		return;

	SetRndSeed(Objects[i]._oRndSeed);
	Objects[i]._oSelFlag = 0;

	if (!deltaload) {
		PlaySfxLoc(sType, Objects[i].position);
		Objects[i]._oAnimFlag = 1;
		Objects[i]._oAnimDelay = 1;
	} else {
		Objects[i]._oAnimFrame = Objects[i]._oAnimLen;
		Objects[i]._oAnimFlag = 0;
	}

	switch (Objects[i]._oVar1) {
	case ShrineMysterious:
		if (!OperateShrineMysterious(pnum))
			return;
		break;
	case ShrineHidden:
		if (!OperateShrineHidden(pnum))
			return;
		break;
	case ShrineGloomy:
		if (!OperateShrineGloomy(pnum))
			return;
		break;
	case ShrineWeird:
		if (!OperateShrineWeird(pnum))
			return;
		break;
	case ShrineMagical:
	case ShrineMagicaL2:
		if (!OperateShrineMagical(pnum))
			return;
		break;
	case ShrineStone:
		if (!OperateShrineStone(pnum))
			return;
		break;
	case ShrineReligious:
		if (!OperateShrineReligious(pnum))
			return;
		break;
	case ShrineEnchanted:
		if (!OperateShrineEnchanted(pnum))
			return;
		break;
	case ShrineThaumaturgic:
		if (!OperateShrineThaumaturgic(pnum))
			return;
		break;
	case ShrineFascinating:
		if (!OperateShrineFascinating(pnum))
			return;
		break;
	case ShrineCryptic:
		if (!OperateShrineCryptic(pnum))
			return;
		break;
	case ShrineEldritch:
		if (!OperateShrineEldritch(pnum))
			return;
		break;
	case ShrineEerie:
		if (!OperateShrineEerie(pnum))
			return;
		break;
	case ShrineDivine:
		if (!OperateShrineDivine(pnum, Objects[i].position))
			return;
		break;
	case ShrineHoly:
		if (!OperateShrineHoly(pnum))
			return;
		break;
	case ShrineSacred:
		if (!OperateShrineSacred(pnum))
			return;
		break;
	case ShrineSpiritual:
		if (!OperateShrineSpiritual(pnum))
			return;
		break;
	case ShrineSpooky:
		if (!OperateShrineSpooky(pnum))
			return;
		break;
	case ShrineAbandoned:
		if (!OperateShrineAbandoned(pnum))
			return;
		break;
	case ShrineCreepy:
		if (!OperateShrineCreepy(pnum))
			return;
		break;
	case ShrineQuiet:
		if (!OperateShrineQuiet(pnum))
			return;
		break;
	case ShrineSecluded:
		if (!OperateShrineSecluded(pnum))
			return;
		break;
	case ShrineOrnate:
		if (!OperateShrineOrnate(pnum))
			return;
		break;
	case ShrineGlimmering:
		if (!OperateShrineGlimmering(pnum))
			return;
		break;
	case ShrineTainted:
		if (!OperateShrineTainted(pnum))
			return;
		break;
	case ShrineOily:
		if (!OperateShrineOily(pnum, Objects[i].position))
			return;
		break;
	case ShrineGlowing:
		if (!OperateShrineGlowing(pnum))
			return;
		break;
	case ShrineMendicant:
		if (!OperateShrineMendicant(pnum))
			return;
		break;
	case ShrineSparkling:
		if (!OperateShrineSparkling(pnum, Objects[i].position))
			return;
		break;
	case ShrineTown:
		if (!OperateShrineTown(pnum, Objects[i].position))
			return;
		break;
	case ShrineShimmering:
		if (!OperateShrineShimmering(pnum))
			return;
		break;
	case ShrineSolar:
		if (!OperateShrineSolar(pnum))
			return;
		break;
	case ShrineMurphys:
		if (!OperateShrineMurphys(pnum))
			return;
		break;
	}

	CalcPlrInv(Players[pnum], true);
	force_redraw = 255;

	if (pnum == MyPlayerId)
		NetSendCmdParam2(false, CMD_PLROPOBJ, pnum, i);
}

void OperateSkelBook(int pnum, int i, bool sendmsg)
{
	if (Objects[i]._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_ISCROL, Objects[i].position);
	Objects[i]._oSelFlag = 0;
	Objects[i]._oAnimFrame += 2;
	if (deltaload) {
		return;
	}
	SetRndSeed(Objects[i]._oRndSeed);
	if (GenerateRnd(5) != 0)
		CreateTypeItem(Objects[i].position, false, ItemType::Misc, IMISC_SCROLL, sendmsg, false);
	else
		CreateTypeItem(Objects[i].position, false, ItemType::Misc, IMISC_BOOK, sendmsg, false);
	if (pnum == MyPlayerId)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateBookCase(int pnum, int i, bool sendmsg)
{
	if (Objects[i]._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_ISCROL, Objects[i].position);
	Objects[i]._oSelFlag = 0;
	Objects[i]._oAnimFrame -= 2;
	if (deltaload) {
		return;
	}
	SetRndSeed(Objects[i]._oRndSeed);
	CreateTypeItem(Objects[i].position, false, ItemType::Misc, IMISC_BOOK, sendmsg, false);

	if (Quests[Q_ZHAR].IsAvailable()) {
		auto &zhar = Monsters[MAX_PLRS];
		if (zhar._mmode == MonsterMode::Stand // prevents playing the "angry" message for the second time if zhar got aggroed by losing vision and talking again
		    && zhar._uniqtype - 1 == UMT_ZHAR
		    && zhar._msquelch == UINT8_MAX
		    && zhar._mhitpoints > 0) {
			zhar.mtalkmsg = TEXT_ZHAR2;
			M_StartStand(zhar, zhar._mdir); // BUGFIX: first parameter in call to M_StartStand should be MAX_PLRS, not 0. (fixed)
			zhar._mgoal = MGOAL_ATTACK2;
			zhar._mmode = MonsterMode::Talk;
		}
	}
	if (pnum == MyPlayerId)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateDecap(int pnum, int i, bool sendmsg)
{
	if (Objects[i]._oSelFlag == 0) {
		return;
	}
	Objects[i]._oSelFlag = 0;
	if (deltaload) {
		return;
	}
	SetRndSeed(Objects[i]._oRndSeed);
	CreateRndItem(Objects[i].position, false, sendmsg, false);
	if (pnum == MyPlayerId)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateArmorStand(int pnum, int i, bool sendmsg)
{
	if (Objects[i]._oSelFlag == 0) {
		return;
	}
	Objects[i]._oSelFlag = 0;
	Objects[i]._oAnimFrame++;
	if (deltaload) {
		return;
	}
	SetRndSeed(Objects[i]._oRndSeed);
	bool uniqueRnd = (GenerateRnd(2) != 0);
	if (currlevel <= 5) {
		CreateTypeItem(Objects[i].position, true, ItemType::LightArmor, IMISC_NONE, sendmsg, false);
	} else if (currlevel >= 6 && currlevel <= 9) {
		CreateTypeItem(Objects[i].position, uniqueRnd, ItemType::MediumArmor, IMISC_NONE, sendmsg, false);
	} else if (currlevel >= 10 && currlevel <= 12) {
		CreateTypeItem(Objects[i].position, false, ItemType::HeavyArmor, IMISC_NONE, sendmsg, false);
	} else if (currlevel >= 13 && currlevel <= 16) {
		CreateTypeItem(Objects[i].position, true, ItemType::HeavyArmor, IMISC_NONE, sendmsg, false);
	} else if (currlevel >= 17) {
		CreateTypeItem(Objects[i].position, true, ItemType::HeavyArmor, IMISC_NONE, sendmsg, false);
	}
	if (pnum == MyPlayerId)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

int FindValidShrine()
{
	bool done = false;
	int rv;
	do {
		rv = GenerateRnd(gbIsHellfire ? NumberOfShrineTypes : 26);
		if (currlevel >= shrinemin[rv] && currlevel <= shrinemax[rv] && rv != ShrineThaumaturgic) {
			done = true;
		}
		if (done) {
			if (gbIsMultiplayer) {
				if (shrineavail[rv] == ShrineTypeSingle) {
					done = false;
					continue;
				}
			}
			if (!gbIsMultiplayer) {
				if (shrineavail[rv] == ShrineTypeMulti) {
					done = false;
					continue;
				}
			}
			done = true;
		}
	} while (!done);
	return rv;
}

void OperateGoatShrine(int pnum, int i, _sfx_id sType)
{
	SetRndSeed(Objects[i]._oRndSeed);
	Objects[i]._oVar1 = FindValidShrine();
	OperateShrine(pnum, i, sType);
	Objects[i]._oAnimDelay = 2;
	force_redraw = 255;
}

void OperateCauldron(int pnum, int i, _sfx_id sType)
{
	SetRndSeed(Objects[i]._oRndSeed);
	Objects[i]._oVar1 = FindValidShrine();
	OperateShrine(pnum, i, sType);
	Objects[i]._oAnimFrame = 3;
	Objects[i]._oAnimFlag = 0;
	force_redraw = 255;
}

bool OperateFountains(int pnum, int i)
{
	auto &player = Players[pnum];
	bool applied = false;
	switch (Objects[i]._otype) {
	case OBJ_BLOODFTN:
		if (deltaload)
			return false;
		if (pnum != MyPlayerId)
			return false;

		if (player._pHitPoints < player._pMaxHP) {
			PlaySfxLoc(LS_FOUNTAIN, Objects[i].position);
			player._pHitPoints += 64;
			player._pHPBase += 64;
			if (player._pHitPoints > player._pMaxHP) {
				player._pHitPoints = player._pMaxHP;
				player._pHPBase = player._pMaxHPBase;
			}
			applied = true;
		} else
			PlaySfxLoc(LS_FOUNTAIN, Objects[i].position);
		break;
	case OBJ_PURIFYINGFTN:
		if (deltaload)
			return false;
		if (pnum != MyPlayerId)
			return false;

		if (player._pMana < player._pMaxMana) {
			PlaySfxLoc(LS_FOUNTAIN, Objects[i].position);

			player._pMana += 64;
			player._pManaBase += 64;
			if (player._pMana > player._pMaxMana) {
				player._pMana = player._pMaxMana;
				player._pManaBase = player._pMaxManaBase;
			}

			applied = true;
		} else
			PlaySfxLoc(LS_FOUNTAIN, Objects[i].position);
		break;
	case OBJ_MURKYFTN:
		if (Objects[i]._oSelFlag == 0)
			break;
		if (!deltaload)
			PlaySfxLoc(LS_FOUNTAIN, Objects[i].position);
		Objects[i]._oSelFlag = 0;
		if (deltaload)
			return false;
		AddMissile(
		    player.position.tile,
		    player.position.tile,
		    player._pdir,
		    MIS_INFRA,
		    TARGET_PLAYERS,
		    pnum,
		    0,
		    2 * leveltype);
		applied = true;
		if (pnum == MyPlayerId)
			NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
		break;
	case OBJ_TEARFTN: {
		if (Objects[i]._oSelFlag == 0)
			break;
		if (!deltaload)
			PlaySfxLoc(LS_FOUNTAIN, Objects[i].position);
		Objects[i]._oSelFlag = 0;
		if (deltaload)
			return false;
		if (pnum != MyPlayerId)
			return false;

		unsigned randomValue = (Objects[i]._oRndSeed >> 16) % 12;
		unsigned fromStat = randomValue / 3;
		unsigned toStat = randomValue % 3;
		if (toStat >= fromStat)
			toStat++;

		std::pair<unsigned, int> alterations[] = { { fromStat, -1 }, { toStat, 1 } };
		for (auto alteration : alterations) {
			switch (alteration.first) {
			case 0:
				ModifyPlrStr(pnum, alteration.second);
				break;
			case 1:
				ModifyPlrMag(pnum, alteration.second);
				break;
			case 2:
				ModifyPlrDex(pnum, alteration.second);
				break;
			case 3:
				ModifyPlrVit(pnum, alteration.second);
				break;
			}
		}

		CheckStats(player);
		applied = true;
		if (pnum == MyPlayerId)
			NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
	} break;
	default:
		break;
	}
	force_redraw = 255;
	return applied;
}

void OperateWeaponRack(int pnum, int i, bool sendmsg)
{
	if (Objects[i]._oSelFlag == 0)
		return;
	SetRndSeed(Objects[i]._oRndSeed);

	ItemType weaponType { PickRandomlyAmong({ ItemType::Sword, ItemType::Axe, ItemType::Bow, ItemType::Mace }) };

	Objects[i]._oSelFlag = 0;
	Objects[i]._oAnimFrame++;
	if (deltaload)
		return;

	CreateTypeItem(Objects[i].position, leveltype > 1, weaponType, IMISC_NONE, sendmsg, false);

	if (pnum == MyPlayerId)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

/**
 * @brief Checks whether the player is activating Na-Krul's spell tomes in the correct order
 *
 * Used as part of the final Diablo: Hellfire quest (from the hints provided to the player in the
 * reconstructed note). This function both updates the state of the variable that tracks progress
 * and also determines whether the spawn conditions are met (i.e. all tomes have been triggered
 * in the correct order).
 *
 * @param s the id of the spell tome
 * @return true if the player has activated all three tomes in the correct order, false otherwise
 */
bool OperateNakrulBook(int s)
{
	switch (s) {
	case 6:
		NaKrulTomeSequence = 1;
		break;
	case 7:
		if (NaKrulTomeSequence == 1) {
			NaKrulTomeSequence = 2;
		} else {
			NaKrulTomeSequence = 0;
		}
		break;
	case 8:
		if (NaKrulTomeSequence == 2)
			return true;
		NaKrulTomeSequence = 0;
		break;
	}
	return false;
}

void OperateStoryBook(int pnum, int i)
{
	if (Objects[i]._oSelFlag == 0 || deltaload || qtextflag || pnum != MyPlayerId) {
		return;
	}
	Objects[i]._oAnimFrame = Objects[i]._oVar4;
	PlaySfxLoc(IS_ISCROL, Objects[i].position);
	auto msg = static_cast<_speech_id>(Objects[i]._oVar2);
	if (Objects[i]._oVar8 != 0 && currlevel == 24) {
		if (!IsUberLeverActivated && Quests[Q_NAKRUL]._qactive != QUEST_DONE && OperateNakrulBook(Objects[i]._oVar8)) {
			NetSendCmd(false, CMD_NAKRUL);
			return;
		}
	} else if (currlevel >= 21) {
		Quests[Q_NAKRUL]._qactive = QUEST_ACTIVE;
		Quests[Q_NAKRUL]._qlog = true;
		Quests[Q_NAKRUL]._qmsg = msg;
	}
	InitQTextMsg(msg);
	NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateLazStand(int pnum, int i)
{
	if (ActiveItemCount >= MAXITEMS) {
		return;
	}

	if (Objects[i]._oSelFlag == 0 || deltaload || qtextflag || pnum != MyPlayerId) {
		return;
	}

	Objects[i]._oAnimFrame++;
	Objects[i]._oSelFlag = 0;
	Point pos = GetSuperItemLoc(Objects[i].position);
	SpawnQuestItem(IDI_LAZSTAFF, pos, 0, 0);
}

void SyncOpL1Door(int pnum, int cmd, int i)
{
	if (pnum == MyPlayerId)
		return;

	bool doSync = false;
	if (cmd == CMD_OPENDOOR && Objects[i]._oVar4 == 0)
		doSync = true;
	if (cmd == CMD_CLOSEDOOR && Objects[i]._oVar4 == 1)
		doSync = true;
	if (!doSync)
		return;

	if (Objects[i]._otype == OBJ_L1LDOOR)
		OperateL1LDoor(-1, i, false);
	if (Objects[i]._otype == OBJ_L1RDOOR)
		OperateL1RDoor(-1, i, false);
}

void SyncOpL2Door(int pnum, int cmd, int i)
{
	if (pnum == MyPlayerId)
		return;

	bool doSync = false;
	if (cmd == CMD_OPENDOOR && Objects[i]._oVar4 == 0)
		doSync = true;
	if (cmd == CMD_CLOSEDOOR && Objects[i]._oVar4 == 1)
		doSync = true;
	if (!doSync)
		return;

	if (Objects[i]._otype == OBJ_L2LDOOR)
		OperateL2LDoor(-1, i, false);
	if (Objects[i]._otype == OBJ_L2RDOOR)
		OperateL2RDoor(-1, i, false);
}

void SyncOpL3Door(int pnum, int cmd, int i)
{
	if (pnum == MyPlayerId)
		return;

	bool doSync = false;
	if (cmd == CMD_OPENDOOR && Objects[i]._oVar4 == 0)
		doSync = true;
	if (cmd == CMD_CLOSEDOOR && Objects[i]._oVar4 == 1)
		doSync = true;
	if (!doSync)
		return;

	if (Objects[i]._otype == OBJ_L3LDOOR)
		OperateL3LDoor(-1, i, false);
	if (Objects[i]._otype == OBJ_L3RDOOR)
		OperateL3RDoor(-1, i, false);
}

/**
 * @brief Checks if all active crux objects of the given type have been broken.
 *
 * Called by BreakCrux and SyncCrux to see if the linked map area needs to be updated. In practice I think this is
 * always true when called by BreakCrux as there *should* only be one instance of each crux with a given _oVar8 value?
 *
 * @param cruxType Discriminator/type (_oVar8 value) of the crux object which is currently changing state
 * @return true if all active cruxes of that type on the level are broken, false if at least one remains unbroken
 */
bool AreAllCruxesOfTypeBroken(int cruxType)
{
	for (int j = 0; j < ActiveObjectCount; j++) {
		const auto &testObject = Objects[ActiveObjects[j]];
		if (!testObject.IsCrux())
			continue; // Not a Crux object, keep searching
		if (cruxType != testObject._oVar8 || testObject._oBreak == -1)
			continue; // Found either a different crux or a previously broken crux, keep searching

		// Found an unbroken crux of this type
		return false;
	}
	return true;
}

void BreakCrux(Object &crux)
{
	crux._oAnimFlag = 1;
	crux._oAnimFrame = 1;
	crux._oAnimDelay = 1;
	crux._oSolidFlag = true;
	crux._oMissFlag = true;
	crux._oBreak = -1;
	crux._oSelFlag = 0;

	if (!AreAllCruxesOfTypeBroken(crux._oVar8))
		return;

	if (!deltaload)
		PlaySfxLoc(IS_LEVER, crux.position);
	ObjChangeMap(crux._oVar1, crux._oVar2, crux._oVar3, crux._oVar4);
}

void BreakBarrel(int pnum, Object &barrel, int dam, bool forcebreak, bool sendmsg)
{
	if (barrel._oSelFlag == 0)
		return;
	if (forcebreak) {
		barrel._oVar1 = 0;
	} else {
		barrel._oVar1 -= dam;
		if (pnum != MyPlayerId && barrel._oVar1 <= 0)
			barrel._oVar1 = 1;
	}
	if (barrel._oVar1 > 0) {
		if (deltaload)
			return;

		PlaySfxLoc(IS_IBOW, barrel.position);
		return;
	}

	barrel._oVar1 = 0;
	barrel._oAnimFlag = 1;
	barrel._oAnimFrame = 1;
	barrel._oAnimDelay = 1;
	barrel._oSolidFlag = false;
	barrel._oMissFlag = true;
	barrel._oBreak = -1;
	barrel._oSelFlag = 0;
	barrel._oPreFlag = true;
	if (deltaload) {
		barrel._oAnimFrame = barrel._oAnimLen;
		barrel._oAnimCnt = 0;
		barrel._oAnimDelay = 1000;
		return;
	}

	if (barrel._otype == OBJ_BARRELEX) {
		if (currlevel >= 21 && currlevel <= 24)
			PlaySfxLoc(IS_POPPOP3, barrel.position);
		else if (currlevel >= 17 && currlevel <= 20)
			PlaySfxLoc(IS_POPPOP8, barrel.position);
		else
			PlaySfxLoc(IS_BARLFIRE, barrel.position);
		for (int yp = barrel.position.y - 1; yp <= barrel.position.y + 1; yp++) {
			for (int xp = barrel.position.x - 1; xp <= barrel.position.x + 1; xp++) {
				if (dMonster[xp][yp] > 0) {
					MonsterTrapHit(dMonster[xp][yp] - 1, 1, 4, 0, MIS_FIREBOLT, false);
				}
				if (dPlayer[xp][yp] > 0) {
					bool unused;
					PlayerMHit(dPlayer[xp][yp] - 1, nullptr, 0, 8, 16, MIS_FIREBOLT, false, 0, &unused);
				}
				// don't really need to exclude large objects as explosive barrels are single tile objects, but using considerLargeObjects == false as this matches the old logic.
				Object *adjacentObject = ObjectAtPosition({ xp, yp }, false);
				if (adjacentObject != nullptr && adjacentObject->_otype == _object_id::OBJ_BARRELEX && !adjacentObject->IsBroken()) {
					BreakBarrel(pnum, *adjacentObject, dam, true, sendmsg);
				}
			}
		}
	} else {
		if (currlevel >= 21 && currlevel <= 24)
			PlaySfxLoc(IS_POPPOP2, barrel.position);
		else if (currlevel >= 17 && currlevel <= 20)
			PlaySfxLoc(IS_POPPOP5, barrel.position);
		else
			PlaySfxLoc(IS_BARREL, barrel.position);
		SetRndSeed(barrel._oRndSeed);
		if (barrel._oVar2 <= 1) {
			if (barrel._oVar3 == 0)
				CreateRndUseful(barrel.position, sendmsg);
			else
				CreateRndItem(barrel.position, false, sendmsg, false);
		}
		if (barrel._oVar2 >= 8)
			SpawnSkeleton(barrel._oVar4, barrel.position);
	}
	if (pnum == MyPlayerId) {
		NetSendCmdParam2(false, CMD_BREAKOBJ, pnum, static_cast<uint16_t>(barrel.GetId()));
	}
}

void SyncCrux(const Object &crux)
{
	if (AreAllCruxesOfTypeBroken(crux._oVar8))
		ObjChangeMap(crux._oVar1, crux._oVar2, crux._oVar3, crux._oVar4);
}

void SyncLever(const Object &lever)
{
	if (lever._oSelFlag != 0)
		return;

	if (currlevel == 16 && !AreAllLeversActivated(lever._oVar8))
		return;

	ObjChangeMap(lever._oVar1, lever._oVar2, lever._oVar3, lever._oVar4);
}

void SyncQSTLever(const Object &qstLever)
{
	if (qstLever._oAnimFrame == qstLever._oVar6) {
		ObjChangeMapResync(qstLever._oVar1, qstLever._oVar2, qstLever._oVar3, qstLever._oVar4);
		if (qstLever._otype == OBJ_BLINDBOOK) {
			auto tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(qstLever._oVar1, qstLever._oVar2, qstLever._oVar3, qstLever._oVar4);
			TransVal = tren;
		}
	}
}

void SyncPedestal(const Object &pedestal, Point origin, int width)
{
	if (pedestal._oVar6 == 1)
		ObjChangeMapResync(origin.x, origin.y + 3, origin.x + 2, origin.y + 7);
	if (pedestal._oVar6 == 2) {
		ObjChangeMapResync(origin.x, origin.y + 3, origin.x + 2, origin.y + 7);
		ObjChangeMapResync(origin.x + 6, origin.y + 3, origin.x + width, origin.y + 7);
	}
	if (pedestal._oVar6 == 3) {
		ObjChangeMapResync(pedestal._oVar1, pedestal._oVar2, pedestal._oVar3, pedestal._oVar4);
		LoadMapObjs("Levels\\L2Data\\Blood2.DUN", origin * 2);
	}
}

void SyncL1Doors(Object &door)
{
	if (door._oVar4 == 0) {
		door._oMissFlag = false;
		return;
	}

	door._oMissFlag = true;
	door._oSelFlag = 2;

	bool isLeftDoor = door._otype == _object_id::OBJ_L1LDOOR; // otherwise the door is type OBJ_L1RDOOR

	if (currlevel < 17) {
		if (isLeftDoor) {
			ObjSetMicro(door.position, door._oVar1 == 214 ? 408 : 393);
			dSpecial[door.position.x][door.position.y] = 7;
			SetDoorPiece(door.position + Direction::NorthWest);
			DoorSet(door.position + Direction::NorthEast, isLeftDoor);
		} else {
			ObjSetMicro(door.position, 395);
			dSpecial[door.position.x][door.position.y] = 8;
			SetDoorPiece(door.position + Direction::NorthEast);
			DoorSet(door.position + Direction::NorthWest, isLeftDoor);
		}
	} else {
		if (isLeftDoor) {
			ObjSetMicro(door.position, 206);
			dSpecial[door.position.x][door.position.y] = 1;
			SetDoorPiece(door.position + Direction::NorthWest);
			DoorSet(door.position + Direction::NorthEast, isLeftDoor);
		} else {
			ObjSetMicro(door.position, 209);
			dSpecial[door.position.x][door.position.y] = 2;
			SetDoorPiece(door.position + Direction::NorthEast);
			DoorSet(door.position + Direction::NorthWest, isLeftDoor);
		}
	}
}

void SyncL2Doors(Object &door)
{
	door._oMissFlag = door._oVar4 != 0;
	door._oSelFlag = 2;

	bool isLeftDoor = door._otype == _object_id::OBJ_L2LDOOR; // otherwise the door is type OBJ_L2RDOOR

	switch (door._oVar4) {
	case 0:
		ObjSetMicro(door.position, isLeftDoor ? 538 : 540);
		dSpecial[door.position.x][door.position.y] = 0;
		break;
	case 1:
	case 2:
		ObjSetMicro(door.position, isLeftDoor ? 13 : 17);
		dSpecial[door.position.x][door.position.y] = isLeftDoor ? 5 : 6;
		break;
	}
}

void SyncL3Doors(Object &door)
{
	door._oMissFlag = true;
	door._oSelFlag = 2;

	bool isLeftDoor = door._otype == _object_id::OBJ_L3LDOOR; // otherwise the door is type OBJ_L3RDOOR

	switch (door._oVar4) {
	case 0:
		ObjSetMicro(door.position, isLeftDoor ? 531 : 534);
		break;
	case 1:
	case 2:
		ObjSetMicro(door.position, isLeftDoor ? 538 : 541);
		break;
	}
}

} // namespace

unsigned int Object::GetId() const
{
	return abs(dObject[position.x][position.y]) - 1;
}

bool Object::IsDisabled() const
{
	if (!*sgOptions.Gameplay.disableCripplingShrines) {
		return false;
	}
	if (IsAnyOf(_otype, _object_id::OBJ_GOATSHRINE, _object_id::OBJ_CAULDRON)) {
		return true;
	}
	if (!IsShrine()) {
		return false;
	}
	return IsAnyOf(static_cast<shrine_type>(_oVar1), shrine_type::ShrineFascinating, shrine_type::ShrineOrnate, shrine_type::ShrineSacred);
}

Object *ObjectAtPosition(Point position, bool considerLargeObjects)
{
	if (!InDungeonBounds(position)) {
		return nullptr;
	}

	auto objectId = dObject[position.x][position.y];

	if (objectId > 0 || (considerLargeObjects && objectId != 0)) {
		return &Objects[abs(objectId) - 1];
	}

	// nothing at this position, return a nullptr
	return nullptr;
}

bool IsItemBlockingObjectAtPosition(Point position)
{
	Object *object = ObjectAtPosition(position);
	if (object != nullptr && object->_oSolidFlag) {
		// solid object
		return true;
	}

	object = ObjectAtPosition(position + Direction::South);
	if (object != nullptr && object->_oSelFlag != 0) {
		// An unopened container or breakable object exists which potentially overlaps this tile, the player might not be able to pick up an item dropped here.
		return true;
	}

	object = ObjectAtPosition(position + Direction::SouthEast, false);
	if (object != nullptr) {
		Object *otherDoor = ObjectAtPosition(position + Direction::SouthWest, false);
		if (otherDoor != nullptr && object->_oSelFlag != 0 && otherDoor->_oSelFlag != 0) {
			// Two interactive objects potentially overlap both sides of this tile, as above the player might not be able to pick up an item which is dropped here.
			return true;
		}
	}

	return false;
}

void InitObjectGFX()
{
	bool fileload[56] = {};

	int lvl = currlevel;
	if (currlevel >= 21 && currlevel <= 24)
		lvl -= 20;
	else if (currlevel >= 17 && currlevel <= 20)
		lvl -= 8;
	for (int i = 0; AllObjects[i].oload != -1; i++) {
		if (AllObjects[i].oload == 1
		    && lvl >= AllObjects[i].ominlvl
		    && lvl <= AllObjects[i].omaxlvl) {
			fileload[AllObjects[i].ofindex] = true;
		}
		if (AllObjects[i].otheme != THEME_NONE) {
			for (int j = 0; j < numthemes; j++) {
				if (themes[j].ttype == AllObjects[i].otheme)
					fileload[AllObjects[i].ofindex] = true;
			}
		}

		if (AllObjects[i].oquest != -1) {
			if (Quests[AllObjects[i].oquest].IsAvailable())
				fileload[AllObjects[i].ofindex] = true;
		}
	}

	for (int i = OFILE_L1BRAZ; i <= OFILE_LZSTAND; i++) {
		if (fileload[i]) {
			ObjFileList[numobjfiles] = static_cast<object_graphic_id>(i);
			char filestr[32];
			sprintf(filestr, "Objects\\%s.CEL", ObjMasterLoadList[i]);
			if (currlevel >= 17 && currlevel < 21)
				sprintf(filestr, "Objects\\%s.CEL", ObjHiveLoadList[i]);
			else if (currlevel >= 21)
				sprintf(filestr, "Objects\\%s.CEL", ObjCryptLoadList[i]);
			pObjCels[numobjfiles] = LoadFileInMem(filestr);
			numobjfiles++;
		}
	}
}

void FreeObjectGFX()
{
	for (int i = 0; i < numobjfiles; i++) {
		pObjCels[i] = nullptr;
	}
	numobjfiles = 0;
}

void AddL1Objs(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < y2; j++) {
		for (int i = x1; i < x2; i++) {
			int pn = dPiece[i][j];
			if (pn == 270)
				AddObject(OBJ_L1LIGHT, { i, j });
			if (pn == 44 || pn == 51 || pn == 214)
				AddObject(OBJ_L1LDOOR, { i, j });
			if (pn == 46 || pn == 56)
				AddObject(OBJ_L1RDOOR, { i, j });
		}
	}
}

void AddL2Objs(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < y2; j++) {
		for (int i = x1; i < x2; i++) {
			int pn = dPiece[i][j];
			if (pn == 13 || pn == 541)
				AddObject(OBJ_L2LDOOR, { i, j });
			if (pn == 17 || pn == 542)
				AddObject(OBJ_L2RDOOR, { i, j });
		}
	}
}

void AddSlainHero()
{
	int x;
	int y;

	GetRndObjLoc(5, &x, &y);
	AddObject(OBJ_SLAINHERO, { x + 2, y + 2 });
}

void InitObjects()
{
	ClrAllObjects();
	NaKrulTomeSequence = 0;
	if (currlevel == 16) {
		AddDiabObjs();
	} else {
		ApplyObjectLighting = true;
		AdvanceRndSeed();
		if (currlevel == 9 && !gbIsMultiplayer)
			AddSlainHero();
		if (currlevel == Quests[Q_MUSHROOM]._qlevel && Quests[Q_MUSHROOM]._qactive == QUEST_INIT)
			AddMushPatch();

		if (currlevel == 4 || currlevel == 8 || currlevel == 12)
			AddStoryBooks();
		if (currlevel == 21) {
			AddCryptStoryBook(1);
		} else if (currlevel == 22) {
			AddCryptStoryBook(2);
			AddCryptStoryBook(3);
		} else if (currlevel == 23) {
			AddCryptStoryBook(4);
			AddCryptStoryBook(5);
		}
		if (currlevel == 24) {
			AddNakrulGate();
		}
		if (leveltype == DTYPE_CATHEDRAL) {
			if (Quests[Q_BUTCHER].IsAvailable())
				AddTortures();
			if (Quests[Q_PWATER].IsAvailable())
				AddCandles();
			if (Quests[Q_LTBANNER].IsAvailable())
				AddObject(OBJ_SIGNCHEST, { 2 * setpc_x + 26, 2 * setpc_y + 19 });
			InitRndLocBigObj(10, 15, OBJ_SARC);
			if (currlevel >= 21)
				AddCryptObjects(0, 0, MAXDUNX, MAXDUNY);
			else
				AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
			InitRndBarrels();
		}
		if (leveltype == DTYPE_CATACOMBS) {
			if (Quests[Q_ROCK].IsAvailable())
				InitRndLocObj5x5(1, 1, OBJ_STAND);
			if (Quests[Q_SCHAMB].IsAvailable())
				InitRndLocObj5x5(1, 1, OBJ_BOOK2R);
			AddL2Objs(0, 0, MAXDUNX, MAXDUNY);
			AddL2Torches();
			if (Quests[Q_BLIND].IsAvailable()) {
				_speech_id spId;
				switch (Players[MyPlayerId]._pClass) {
				case HeroClass::Warrior:
					spId = TEXT_BLINDING;
					break;
				case HeroClass::Rogue:
					spId = TEXT_RBLINDING;
					break;
				case HeroClass::Sorcerer:
					spId = TEXT_MBLINDING;
					break;
				case HeroClass::Monk:
					spId = TEXT_HBLINDING;
					break;
				case HeroClass::Bard:
					spId = TEXT_BBLINDING;
					break;
				case HeroClass::Barbarian:
					spId = TEXT_BLINDING;
					break;
				}
				Quests[Q_BLIND]._qmsg = spId;
				AddBookLever({ { setpc_x, setpc_y }, { setpc_w + 1, setpc_h + 1 } }, spId);
				LoadMapObjs("Levels\\L2Data\\Blind2.DUN", { 2 * setpc_x, 2 * setpc_y });
			}
			if (Quests[Q_BLOOD].IsAvailable()) {
				_speech_id spId;
				switch (Players[MyPlayerId]._pClass) {
				case HeroClass::Warrior:
					spId = TEXT_BLOODY;
					break;
				case HeroClass::Rogue:
					spId = TEXT_RBLOODY;
					break;
				case HeroClass::Sorcerer:
					spId = TEXT_MBLOODY;
					break;
				case HeroClass::Monk:
					spId = TEXT_HBLOODY;
					break;
				case HeroClass::Bard:
					spId = TEXT_BBLOODY;
					break;
				case HeroClass::Barbarian:
					spId = TEXT_BLOODY;
					break;
				}
				Quests[Q_BLOOD]._qmsg = spId;
				AddBookLever({ { setpc_x, setpc_y + 3 }, { 2, 4 } }, spId);
				AddObject(OBJ_PEDISTAL, { 2 * setpc_x + 25, 2 * setpc_y + 32 });
			}
			InitRndBarrels();
		}
		if (leveltype == DTYPE_CAVES) {
			AddL3Objs(0, 0, MAXDUNX, MAXDUNY);
			InitRndBarrels();
		}
		if (leveltype == DTYPE_HELL) {
			if (Quests[Q_WARLORD].IsAvailable()) {
				_speech_id spId;
				switch (Players[MyPlayerId]._pClass) {
				case HeroClass::Warrior:
					spId = TEXT_BLOODWAR;
					break;
				case HeroClass::Rogue:
					spId = TEXT_RBLOODWAR;
					break;
				case HeroClass::Sorcerer:
					spId = TEXT_MBLOODWAR;
					break;
				case HeroClass::Monk:
					spId = TEXT_HBLOODWAR;
					break;
				case HeroClass::Bard:
					spId = TEXT_BBLOODWAR;
					break;
				case HeroClass::Barbarian:
					spId = TEXT_BLOODWAR;
					break;
				}
				Quests[Q_WARLORD]._qmsg = spId;
				AddBookLever({ { setpc_x, setpc_y }, { setpc_w, setpc_h } }, spId);
				LoadMapObjs("Levels\\L4Data\\Warlord.DUN", { 2 * setpc_x, 2 * setpc_y });
			}
			if (Quests[Q_BETRAYER].IsAvailable() && !gbIsMultiplayer)
				AddLazStand();
			InitRndBarrels();
			AddL4Goodies();
		}
		InitRndLocObj(5, 10, OBJ_CHEST1);
		InitRndLocObj(3, 6, OBJ_CHEST2);
		InitRndLocObj(1, 5, OBJ_CHEST3);
		if (leveltype != DTYPE_HELL)
			AddObjTraps();
		if (leveltype > DTYPE_CATHEDRAL)
			AddChestTraps();
		ApplyObjectLighting = false;
	}
}

void SetMapObjects(const uint16_t *dunData, int startx, int starty)
{
	bool filesLoaded[56];
	char filestr[32];

	ClrAllObjects();
	for (auto &fileLoaded : filesLoaded)
		fileLoaded = false;
	ApplyObjectLighting = true;

	for (int i = 0; AllObjects[i].oload != -1; i++) {
		if (AllObjects[i].oload == 1 && leveltype == AllObjects[i].olvltype)
			filesLoaded[AllObjects[i].ofindex] = true;
	}

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	int layer2Offset = 2 + width * height;

	// The rest of the layers are at dPiece scale
	width *= 2;
	height *= 2;

	const uint16_t *objectLayer = &dunData[layer2Offset + width * height * 2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t objectId = SDL_SwapLE16(objectLayer[j * width + i]);
			if (objectId != 0) {
				filesLoaded[AllObjects[ObjTypeConv[objectId]].ofindex] = true;
			}
		}
	}

	for (int i = OFILE_L1BRAZ; i <= OFILE_LZSTAND; i++) {
		if (!filesLoaded[i])
			continue;

		ObjFileList[numobjfiles] = (object_graphic_id)i;
		sprintf(filestr, "Objects\\%s.CEL", ObjMasterLoadList[i]);
		pObjCels[numobjfiles] = LoadFileInMem(filestr);
		numobjfiles++;
	}

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t objectId = SDL_SwapLE16(objectLayer[j * width + i]);
			if (objectId != 0) {
				AddObject(ObjTypeConv[objectId], { startx + 16 + i, starty + 16 + j });
			}
		}
	}

	ApplyObjectLighting = false;
}

void AddObject(_object_id objType, Point objPos)
{
	if (ActiveObjectCount >= MAXOBJECTS)
		return;

	int oi = AvailableObjects[0];
	AvailableObjects[0] = AvailableObjects[MAXOBJECTS - 1 - ActiveObjectCount];
	ActiveObjects[ActiveObjectCount] = oi;
	dObject[objPos.x][objPos.y] = oi + 1;
	Object &object = Objects[oi];
	SetupObject(object, objPos, objType);
	switch (objType) {
	case OBJ_L1LIGHT:
	case OBJ_SKFIRE:
	case OBJ_CANDLE1:
	case OBJ_CANDLE2:
	case OBJ_BOOKCANDLE:
		AddObjectLight(oi, 5);
		break;
	case OBJ_STORYCANDLE:
		AddObjectLight(oi, 3);
		break;
	case OBJ_TORCHL:
	case OBJ_TORCHR:
	case OBJ_TORCHL2:
	case OBJ_TORCHR2:
		AddObjectLight(oi, 8);
		break;
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		InitializeL1Door(object);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		// If a catacombs door happens to overlap an arch then clear the arch tile to prevent weird rendering
		dSpecial[object.position.x][object.position.y] = 0;
		// intentional fall-through
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		InitializeMicroDoor(object);
		break;
	case OBJ_BOOK2R:
		object.InitializeBook({ { setpc_x, setpc_y }, { setpc_w + 1, setpc_h + 1 } });
		break;
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
		AddChest(oi, objType);
		break;
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		AddChest(oi, objType);
		object._oTrapFlag = true;
		if (leveltype == DTYPE_CATACOMBS) {
			object._oVar4 = GenerateRnd(2);
		} else {
			object._oVar4 = GenerateRnd(3);
		}
		break;
	case OBJ_SARC:
		AddSarc(oi);
		break;
	case OBJ_FLAMEHOLE:
		AddFlameTrap(oi);
		break;
	case OBJ_FLAMELVR:
		AddFlameLvr(oi);
		break;
	case OBJ_WATER:
		object._oAnimFrame = 1;
		break;
	case OBJ_TRAPL:
	case OBJ_TRAPR:
		AddTrap(oi);
		break;
	case OBJ_BARREL:
	case OBJ_BARRELEX:
		AddBarrel(oi, objType);
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		AddShrine(oi);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		AddBookcase(oi);
		break;
	case OBJ_SKELBOOK:
	case OBJ_BOOKSTAND:
		AddBookstand(oi);
		break;
	case OBJ_BLOODFTN:
		AddBloodFtn(oi);
		break;
	case OBJ_DECAP:
		AddDecap(oi);
		break;
	case OBJ_PURIFYINGFTN:
		AddPurifyingFountain(oi);
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		AddArmorStand(oi);
		break;
	case OBJ_GOATSHRINE:
		AddGoatShrine(oi);
		break;
	case OBJ_CAULDRON:
		AddCauldron(oi);
		break;
	case OBJ_MURKYFTN:
		AddMurkyFountain(oi);
		break;
	case OBJ_TEARFTN:
		AddTearFountain(oi);
		break;
	case OBJ_BOOK2L:
		AddVilebook(oi);
		break;
	case OBJ_MCIRCLE1:
	case OBJ_MCIRCLE2:
		AddMagicCircle(oi);
		break;
	case OBJ_STORYBOOK:
		AddStoryBook(oi);
		break;
	case OBJ_BCROSS:
	case OBJ_TBCROSS:
		AddBrnCross(oi);
		AddObjectLight(oi, 5);
		break;
	case OBJ_PEDISTAL:
		AddPedistal(oi);
		break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK:
		AddWeaponRack(oi);
		break;
	case OBJ_TNUDEM2:
		AddTorturedBody(oi);
		break;
	default:
		break;
	}
	ActiveObjectCount++;
}

void OperateTrap(Object &trap)
{
	if (trap._oVar4 != 0)
		return;

	Object &trigger = *ObjectAtPosition({ trap._oVar1, trap._oVar2 });
	switch (trigger._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (trigger._oVar4 == 0)
			return;
		break;
	case OBJ_LEVER:
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
	case OBJ_SWITCHSKL:
	case OBJ_SARC:
		if (trigger._oSelFlag != 0)
			return;
		break;
	default:
		return;
	}

	trap._oVar4 = 1;
	Point target = trigger.position;
	for (int y = target.y - 1; y <= trigger.position.y + 1; y++) {
		for (int x = trigger.position.x - 1; x <= trigger.position.x + 1; x++) {
			if (dPlayer[x][y] != 0) {
				target.x = x;
				target.y = y;
			}
		}
	}
	if (!deltaload) {
		Direction dir = GetDirection(trap.position, target);
		AddMissile(trap.position, target, dir, static_cast<missile_id>(trap._oVar3), TARGET_PLAYERS, -1, 0, 0);
		PlaySfxLoc(IS_TRAP, trigger.position);
	}
	trigger._oTrapFlag = false;
}

void ProcessObjects()
{
	for (int i = 0; i < ActiveObjectCount; ++i) {
		int oi = ActiveObjects[i];
		switch (Objects[oi]._otype) {
		case OBJ_L1LIGHT:
			UpdateObjectLight(oi, 10);
			break;
		case OBJ_SKFIRE:
		case OBJ_CANDLE2:
		case OBJ_BOOKCANDLE:
			UpdateObjectLight(oi, 5);
			break;
		case OBJ_STORYCANDLE:
			UpdateObjectLight(oi, 3);
			break;
		case OBJ_CRUX1:
		case OBJ_CRUX2:
		case OBJ_CRUX3:
		case OBJ_BARREL:
		case OBJ_BARRELEX:
		case OBJ_SHRINEL:
		case OBJ_SHRINER:
			ObjectStopAnim(oi);
			break;
		case OBJ_L1LDOOR:
		case OBJ_L1RDOOR:
		case OBJ_L2LDOOR:
		case OBJ_L2RDOOR:
		case OBJ_L3LDOOR:
		case OBJ_L3RDOOR:
			UpdateDoor(oi);
			break;
		case OBJ_TORCHL:
		case OBJ_TORCHR:
		case OBJ_TORCHL2:
		case OBJ_TORCHR2:
			UpdateObjectLight(oi, 8);
			break;
		case OBJ_SARC:
			UpdateSarcoffagus(oi);
			break;
		case OBJ_FLAMEHOLE:
			UpdateFlameTrap(oi);
			break;
		case OBJ_TRAPL:
		case OBJ_TRAPR:
			OperateTrap(Objects[oi]);
			break;
		case OBJ_MCIRCLE1:
		case OBJ_MCIRCLE2:
			UpdateCircle(oi);
			break;
		case OBJ_BCROSS:
		case OBJ_TBCROSS:
			UpdateObjectLight(oi, 10);
			UpdateBurningCrossDamage(oi);
			break;
		default:
			break;
		}
		if (Objects[oi]._oAnimFlag == 0)
			continue;

		Objects[oi]._oAnimCnt++;

		if (Objects[oi]._oAnimCnt < Objects[oi]._oAnimDelay)
			continue;

		Objects[oi]._oAnimCnt = 0;
		Objects[oi]._oAnimFrame++;
		if (Objects[oi]._oAnimFrame > Objects[oi]._oAnimLen)
			Objects[oi]._oAnimFrame = 1;
	}

	for (int i = 0; i < ActiveObjectCount;) {
		int oi = ActiveObjects[i];
		if (Objects[oi]._oDelFlag) {
			DeleteObject(oi, i);
		} else {
			i++;
		}
	}
}

void RedoPlayerVision()
{
	for (auto &player : Players) {
		if (player.plractive && currlevel == player.plrlevel) {
			ChangeVisionXY(player._pvid, player.position.tile);
		}
	}
}

void MonstCheckDoors(Monster &monster)
{
	int mx = monster.position.tile.x;
	int my = monster.position.tile.y;
	if (dObject[mx - 1][my - 1] != 0
	    || dObject[mx][my - 1] != 0
	    || dObject[mx + 1][my - 1] != 0
	    || dObject[mx - 1][my] != 0
	    || dObject[mx + 1][my] != 0
	    || dObject[mx - 1][my + 1] != 0
	    || dObject[mx][my + 1] != 0
	    || dObject[mx + 1][my + 1] != 0) {
		for (int i = 0; i < ActiveObjectCount; i++) {
			int oi = ActiveObjects[i];
			if ((Objects[oi]._otype == OBJ_L1LDOOR || Objects[oi]._otype == OBJ_L1RDOOR) && Objects[oi]._oVar4 == 0) {
				int dpx = abs(Objects[oi].position.x - mx);
				int dpy = abs(Objects[oi].position.y - my);
				if (dpx == 1 && dpy <= 1 && Objects[oi]._otype == OBJ_L1LDOOR)
					OperateL1LDoor(MyPlayerId, oi, true);
				if (dpx <= 1 && dpy == 1 && Objects[oi]._otype == OBJ_L1RDOOR)
					OperateL1RDoor(MyPlayerId, oi, true);
			}
			if ((Objects[oi]._otype == OBJ_L2LDOOR || Objects[oi]._otype == OBJ_L2RDOOR) && Objects[oi]._oVar4 == 0) {
				int dpx = abs(Objects[oi].position.x - mx);
				int dpy = abs(Objects[oi].position.y - my);
				if (dpx == 1 && dpy <= 1 && Objects[oi]._otype == OBJ_L2LDOOR)
					OperateL2LDoor(MyPlayerId, oi, true);
				if (dpx <= 1 && dpy == 1 && Objects[oi]._otype == OBJ_L2RDOOR)
					OperateL2RDoor(MyPlayerId, oi, true);
			}
			if ((Objects[oi]._otype == OBJ_L3LDOOR || Objects[oi]._otype == OBJ_L3RDOOR) && Objects[oi]._oVar4 == 0) {
				int dpx = abs(Objects[oi].position.x - mx);
				int dpy = abs(Objects[oi].position.y - my);
				if (dpx == 1 && dpy <= 1 && Objects[oi]._otype == OBJ_L3RDOOR)
					OperateL3RDoor(MyPlayerId, oi, true);
				if (dpx <= 1 && dpy == 1 && Objects[oi]._otype == OBJ_L3LDOOR)
					OperateL3LDoor(MyPlayerId, oi, true);
			}
		}
	}
}

void ObjChangeMap(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j <= y2; j++) {
		for (int i = x1; i <= x2; i++) {
			ObjSetMini({ i, j }, pdungeon[i][j]);
			dungeon[i][j] = pdungeon[i][j];
		}
	}
	if (leveltype == DTYPE_CATHEDRAL && currlevel < 17) {
		ObjL1Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
		AddL1Objs(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
	if (leveltype == DTYPE_CATACOMBS) {
		ObjL2Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
		AddL2Objs(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
}

void ObjChangeMapResync(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j <= y2; j++) {
		for (int i = x1; i <= x2; i++) {
			ObjSetMini({ i, j }, pdungeon[i][j]);
			dungeon[i][j] = pdungeon[i][j];
		}
	}
	if (leveltype == DTYPE_CATHEDRAL && currlevel < 17) {
		ObjL1Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
	if (leveltype == DTYPE_CATACOMBS) {
		ObjL2Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
}

void TryDisarm(int pnum, int i)
{
	if (pnum == MyPlayerId)
		NewCursor(CURSOR_HAND);
	if (!Objects[i]._oTrapFlag) {
		return;
	}
	int trapdisper = 2 * Players[pnum]._pDexterity - 5 * currlevel;
	if (GenerateRnd(100) > trapdisper) {
		return;
	}
	for (int j = 0; j < ActiveObjectCount; j++) {
		Object &trap = Objects[ActiveObjects[j]];
		if (trap.IsTrap() && dObject[trap._oVar1][trap._oVar2] - 1 == i) {
			trap._oVar4 = 1;
			Objects[i]._oTrapFlag = false;
		}
	}
	if (Objects[i].IsTrappedChest()) {
		Objects[i]._oTrapFlag = false;
	}
}

int ItemMiscIdIdx(item_misc_id imiscid)
{
	int i = IDI_GOLD;
	while (AllItemsList[i].iRnd == IDROP_NEVER || AllItemsList[i].iMiscId != imiscid) {
		i++;
	}

	return i;
}

void OperateObject(int pnum, int i, bool teleFlag)
{
	bool sendmsg = pnum == MyPlayerId;
	switch (Objects[i]._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		if (teleFlag) {
			if (Objects[i]._otype == OBJ_L1LDOOR)
				OperateL1LDoor(pnum, i, true);
			if (Objects[i]._otype == OBJ_L1RDOOR)
				OperateL1RDoor(pnum, i, true);
			break;
		}
		if (pnum == MyPlayerId)
			OperateL1Door(pnum, i, true);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		if (teleFlag) {
			if (Objects[i]._otype == OBJ_L2LDOOR)
				OperateL2LDoor(pnum, i, true);
			if (Objects[i]._otype == OBJ_L2RDOOR)
				OperateL2RDoor(pnum, i, true);
			break;
		}
		if (pnum == MyPlayerId)
			OperateL2Door(pnum, i, true);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (teleFlag) {
			if (Objects[i]._otype == OBJ_L3LDOOR)
				OperateL3LDoor(pnum, i, true);
			if (Objects[i]._otype == OBJ_L3RDOOR)
				OperateL3RDoor(pnum, i, true);
			break;
		}
		if (pnum == MyPlayerId)
			OperateL3Door(pnum, i, true);
		break;
	case OBJ_LEVER:
	case OBJ_SWITCHSKL:
		OperateLever(pnum, i);
		break;
	case OBJ_BOOK2L:
		OperateBook(pnum, Objects[i]);
		break;
	case OBJ_BOOK2R:
		OperateChamberOfBoneBook(Objects[i]);
		break;
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		OperateChest(pnum, i, sendmsg);
		break;
	case OBJ_SARC:
		OperateSarc(pnum, i, sendmsg);
		break;
	case OBJ_FLAMELVR:
		OperateTrapLever(Objects[i]);
		break;
	case OBJ_BLINDBOOK:
	case OBJ_BLOODBOOK:
	case OBJ_STEELTOME:
		OperateBookLever(pnum, i);
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		OperateShrine(pnum, i, IS_MAGIC);
		break;
	case OBJ_SKELBOOK:
	case OBJ_BOOKSTAND:
		OperateSkelBook(pnum, i, sendmsg);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		OperateBookCase(pnum, i, sendmsg);
		break;
	case OBJ_DECAP:
		OperateDecap(pnum, i, sendmsg);
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		OperateArmorStand(pnum, i, sendmsg);
		break;
	case OBJ_GOATSHRINE:
		OperateGoatShrine(pnum, i, LS_GSHRINE);
		break;
	case OBJ_CAULDRON:
		OperateCauldron(pnum, i, LS_CALDRON);
		break;
	case OBJ_BLOODFTN:
	case OBJ_PURIFYINGFTN:
	case OBJ_MURKYFTN:
	case OBJ_TEARFTN:
		OperateFountains(pnum, i);
		break;
	case OBJ_STORYBOOK:
		OperateStoryBook(pnum, i);
		break;
	case OBJ_PEDISTAL:
		OperatePedistal(pnum, i);
		break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK:
		OperateWeaponRack(pnum, i, sendmsg);
		break;
	case OBJ_MUSHPATCH:
		OperateMushroomPatch(pnum, Objects[i]);
		break;
	case OBJ_LAZSTAND:
		OperateLazStand(pnum, i);
		break;
	case OBJ_SLAINHERO:
		OperateSlainHero(pnum, i);
		break;
	case OBJ_SIGNCHEST:
		OperateInnSignChest(pnum, Objects[i]);
		break;
	default:
		break;
	}
}

void SyncOpObject(int pnum, int cmd, int i)
{
	switch (Objects[i]._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		SyncOpL1Door(pnum, cmd, i);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		SyncOpL2Door(pnum, cmd, i);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		SyncOpL3Door(pnum, cmd, i);
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
		OperateChest(pnum, i, false);
		break;
	case OBJ_SARC:
		OperateSarc(pnum, i, false);
		break;
	case OBJ_BLINDBOOK:
	case OBJ_BLOODBOOK:
	case OBJ_STEELTOME:
		OperateBookLever(pnum, i);
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		OperateShrine(pnum, i, IS_MAGIC);
		break;
	case OBJ_SKELBOOK:
	case OBJ_BOOKSTAND:
		OperateSkelBook(pnum, i, false);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		OperateBookCase(pnum, i, false);
		break;
	case OBJ_DECAP:
		OperateDecap(pnum, i, false);
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		OperateArmorStand(pnum, i, false);
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
	case OBJ_PEDISTAL:
		OperatePedistal(pnum, i);
		break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK:
		OperateWeaponRack(pnum, i, false);
		break;
	case OBJ_MUSHPATCH:
		OperateMushroomPatch(pnum, Objects[i]);
		break;
	case OBJ_SLAINHERO:
		OperateSlainHero(pnum, i);
		break;
	case OBJ_SIGNCHEST:
		OperateInnSignChest(pnum, Objects[i]);
		break;
	default:
		break;
	}
}

void BreakObject(int pnum, Object &object)
{
	int objdam = 10;
	if (pnum != -1) {
		auto &player = Players[pnum];
		int mind = player._pIMinDam;
		int maxd = player._pIMaxDam;
		objdam = GenerateRnd(maxd - mind + 1) + mind;
		objdam += player._pDamageMod + player._pIBonusDamMod + objdam * player._pIBonusDam / 100;
	}

	if (object.IsBarrel()) {
		BreakBarrel(pnum, object, objdam, false, true);
	} else if (object.IsCrux()) {
		BreakCrux(object);
	}
}

void SyncBreakObj(int pnum, Object &object)
{
	if (object.IsBarrel()) {
		BreakBarrel(pnum, object, 0, true, false);
	}
}

void SyncObjectAnim(Object &object)
{
	object_graphic_id index = AllObjects[object._otype].ofindex;

	const auto &found = std::find(std::begin(ObjFileList), std::end(ObjFileList), index);
	if (found == std::end(ObjFileList)) {
		LogCritical("Unable to find object_graphic_id {} in list of objects to load, level generation error.", index);
		return;
	}

	const int i = std::distance(std::begin(ObjFileList), found);

	object._oAnimData = pObjCels[i].get();
	switch (object._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		SyncL1Doors(object);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		SyncL2Doors(object);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		SyncL3Doors(object);
		break;
	case OBJ_CRUX1:
	case OBJ_CRUX2:
	case OBJ_CRUX3:
		SyncCrux(object);
		break;
	case OBJ_LEVER:
	case OBJ_BOOK2L:
	case OBJ_SWITCHSKL:
		SyncLever(object);
		break;
	case OBJ_BOOK2R:
	case OBJ_BLINDBOOK:
	case OBJ_STEELTOME:
		SyncQSTLever(object);
		break;
	case OBJ_PEDISTAL:
		SyncPedestal(object, { setpc_x, setpc_y }, setpc_w);
		break;
	default:
		break;
	}
}

void GetObjectStr(const Object &object)
{
	switch (object._otype) {
	case OBJ_CRUX1:
	case OBJ_CRUX2:
	case OBJ_CRUX3:
		strcpy(infostr, _("Crucified Skeleton"));
		break;
	case OBJ_LEVER:
	case OBJ_FLAMELVR:
		strcpy(infostr, _("Lever"));
		break;
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (object._oVar4 == 1)
			strcpy(infostr, _("Open Door"));
		if (object._oVar4 == 0)
			strcpy(infostr, _("Closed Door"));
		if (object._oVar4 == 2)
			strcpy(infostr, _("Blocked Door"));
		break;
	case OBJ_BOOK2L:
		if (setlevel) {
			if (setlvlnum == SL_BONECHAMB) {
				strcpy(infostr, _("Ancient Tome"));
			} else if (setlvlnum == SL_VILEBETRAYER) {
				strcpy(infostr, _("Book of Vileness"));
			}
		}
		break;
	case OBJ_SWITCHSKL:
		strcpy(infostr, _("Skull Lever"));
		break;
	case OBJ_BOOK2R:
		strcpy(infostr, _("Mythical Book"));
		break;
	case OBJ_CHEST1:
	case OBJ_TCHEST1:
		strcpy(infostr, _("Small Chest"));
		break;
	case OBJ_CHEST2:
	case OBJ_TCHEST2:
		strcpy(infostr, _("Chest"));
		break;
	case OBJ_CHEST3:
	case OBJ_TCHEST3:
	case OBJ_SIGNCHEST:
		strcpy(infostr, _("Large Chest"));
		break;
	case OBJ_SARC:
		strcpy(infostr, _("Sarcophagus"));
		break;
	case OBJ_BOOKSHELF:
		strcpy(infostr, _("Bookshelf"));
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		strcpy(infostr, _("Bookcase"));
		break;
	case OBJ_BARREL:
	case OBJ_BARRELEX:
		if (currlevel >= 17 && currlevel <= 20)      // for hive levels
			strcpy(infostr, _("Pod"));               // Then a barrel is called a pod
		else if (currlevel >= 21 && currlevel <= 24) // for crypt levels
			strcpy(infostr, _("Urn"));               // Then a barrel is called an urn
		else
			strcpy(infostr, _("Barrel"));
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		strcpy(tempstr, fmt::format(_(/* TRANSLATORS: {:s} will be a name from the Shrine block above */ "{:s} Shrine"), _(ShrineNames[object._oVar1])).c_str());
		strcpy(infostr, tempstr);
		break;
	case OBJ_SKELBOOK:
		strcpy(infostr, _("Skeleton Tome"));
		break;
	case OBJ_BOOKSTAND:
		strcpy(infostr, _("Library Book"));
		break;
	case OBJ_BLOODFTN:
		strcpy(infostr, _("Blood Fountain"));
		break;
	case OBJ_DECAP:
		strcpy(infostr, _("Decapitated Body"));
		break;
	case OBJ_BLINDBOOK:
		strcpy(infostr, _("Book of the Blind"));
		break;
	case OBJ_BLOODBOOK:
		strcpy(infostr, _("Book of Blood"));
		break;
	case OBJ_PURIFYINGFTN:
		strcpy(infostr, _("Purifying Spring"));
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		strcpy(infostr, _("Armor"));
		break;
	case OBJ_WARWEAP:
		strcpy(infostr, _("Weapon Rack"));
		break;
	case OBJ_GOATSHRINE:
		strcpy(infostr, _("Goat Shrine"));
		break;
	case OBJ_CAULDRON:
		strcpy(infostr, _("Cauldron"));
		break;
	case OBJ_MURKYFTN:
		strcpy(infostr, _("Murky Pool"));
		break;
	case OBJ_TEARFTN:
		strcpy(infostr, _("Fountain of Tears"));
		break;
	case OBJ_STEELTOME:
		strcpy(infostr, _("Steel Tome"));
		break;
	case OBJ_PEDISTAL:
		strcpy(infostr, _("Pedestal of Blood"));
		break;
	case OBJ_STORYBOOK:
		strcpy(infostr, _(StoryBookName[object._oVar3]));
		break;
	case OBJ_WEAPONRACK:
		strcpy(infostr, _("Weapon Rack"));
		break;
	case OBJ_MUSHPATCH:
		strcpy(infostr, _("Mushroom Patch"));
		break;
	case OBJ_LAZSTAND:
		strcpy(infostr, _("Vile Stand"));
		break;
	case OBJ_SLAINHERO:
		strcpy(infostr, _("Slain Hero"));
		break;
	default:
		break;
	}
	if (Players[MyPlayerId]._pClass == HeroClass::Rogue) {
		if (object._oTrapFlag) {
			strcpy(tempstr, fmt::format(_(/* TRANSLATORS: {:s} will either be a chest or a door */ "Trapped {:s}"), infostr).c_str());
			strcpy(infostr, tempstr);
			InfoColor = UiFlags::ColorRed;
		}
	}
	if (object.IsDisabled()) {
		strcpy(tempstr, fmt::format(_(/* TRANSLATORS: If user enabled diablo.ini setting "Disable Crippling Shrines" is set to 1; also used for Na-Kruls leaver */ "{:s} (disabled)"), infostr).c_str());
		strcpy(infostr, tempstr);
		InfoColor = UiFlags::ColorRed;
	}
}

void OperateNakrulLever()
{
	if (currlevel == 24) {
		PlaySfxLoc(IS_CROPEN, { UberRow, UberCol });
		SyncNakrulRoom();
	}
}

void SyncNakrulRoom()
{
	dPiece[UberRow][UberCol] = 298;
	dPiece[UberRow][UberCol - 1] = 301;
	dPiece[UberRow][UberCol - 2] = 300;
	dPiece[UberRow][UberCol + 1] = 299;

	SetDungeonMicros();
}

void AddNakrulLeaver()
{
	while (true) {
		int xp = GenerateRnd(80) + 16;
		int yp = GenerateRnd(80) + 16;
		if (RndLocOk(xp - 1, yp - 1)
		    && RndLocOk(xp, yp - 1)
		    && RndLocOk(xp + 1, yp - 1)
		    && RndLocOk(xp - 1, yp)
		    && RndLocOk(xp, yp)
		    && RndLocOk(xp + 1, yp)
		    && RndLocOk(xp - 1, yp + 1)
		    && RndLocOk(xp, yp + 1)
		    && RndLocOk(xp + 1, yp + 1)) {
			break;
		}
	}
	AddObject(OBJ_LEVER, { UberRow + 3, UberCol - 1 });
}

} // namespace devilution
