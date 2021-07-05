/**
 * @file objects.cpp
 *
 * Implementation of object functionality, interaction, spawning, loading, etc.
 */
#include <algorithm>
#include <climits>
#include <cstdint>

#include "automap.h"
#include "control.h"
#include "cursor.h"
#include "drlg_l1.h"
#include "drlg_l4.h"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "error.h"
#include "init.h"
#include "lighting.h"
#include "minitext.h"
#include "missiles.h"
#include "options.h"
#include "setmaps.h"
#include "stores.h"
#include "themes.h"
#include "towners.h"
#include "track.h"
#include "utils/language.h"
#include "utils/log.hpp"

namespace devilution {

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
int objectactive[MAXOBJECTS];
/** Specifies the number of active objects. */
int nobjects;
int leverid;
int objectavail[MAXOBJECTS];
ObjectStruct object[MAXOBJECTS];
bool InitObjFlag;
bool LoadMapObjsFlag;
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
			if (QuestStatus(AllObjects[i].oquest))
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
	int i;

	for (i = 0; i < numobjfiles; i++) {
		pObjCels[i] = nullptr;
	}
	numobjfiles = 0;
}

bool RndLocOk(int xp, int yp)
{
	if (dMonster[xp][yp] != 0)
		return false;
	if (dPlayer[xp][yp] != 0)
		return false;
	if (dObject[xp][yp] != 0)
		return false;
	if ((dFlags[xp][yp] & BFLAG_POPULATED) != 0)
		return false;
	if (nSolidTable[dPiece[xp][yp]])
		return false;
	return leveltype != DTYPE_CATHEDRAL || dPiece[xp][yp] <= 126 || dPiece[xp][yp] >= 144;
}

static bool WallTrapLocOkK(int xp, int yp)
{
	if ((dFlags[xp][yp] & BFLAG_POPULATED) != 0)
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
				AddObject(objtype, xp, yp);
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
				AddObject(objtype, xp, yp);
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
		AddObject(objtype, xp, yp);
	}
}

void ClrAllObjects()
{
	memset(object, 0, sizeof(object));
	nobjects = 0;
	for (int i = 0; i < MAXOBJECTS; i++) {
		objectavail[i] = i;
	}
	memset(objectactive, 0, sizeof(objectactive));
	trapdir = 0;
	trapid = 1;
	leverid = 1;
}

void AddTortures()
{
	for (int oy = 0; oy < MAXDUNY; oy++) {
		for (int ox = 0; ox < MAXDUNX; ox++) {
			if (dPiece[ox][oy] == 367) {
				AddObject(OBJ_TORTURE1, ox, oy + 1);
				AddObject(OBJ_TORTURE3, ox + 2, oy - 1);
				AddObject(OBJ_TORTURE2, ox, oy + 3);
				AddObject(OBJ_TORTURE4, ox + 4, oy - 1);
				AddObject(OBJ_TORTURE5, ox, oy + 5);
				AddObject(OBJ_TNUDEM1, ox + 1, oy + 3);
				AddObject(OBJ_TNUDEM2, ox + 4, oy + 5);
				AddObject(OBJ_TNUDEM3, ox + 2, oy);
				AddObject(OBJ_TNUDEM4, ox + 3, oy + 2);
				AddObject(OBJ_TNUDEW1, ox + 2, oy + 4);
				AddObject(OBJ_TNUDEW2, ox + 2, oy + 1);
				AddObject(OBJ_TNUDEW3, ox + 4, oy + 2);
			}
		}
	}
}
void AddCandles()
{
	int tx = quests[Q_PWATER].position.x;
	int ty = quests[Q_PWATER].position.y;
	AddObject(OBJ_STORYCANDLE, tx - 2, ty + 1);
	AddObject(OBJ_STORYCANDLE, tx + 3, ty + 1);
	AddObject(OBJ_STORYCANDLE, tx - 1, ty + 2);
	AddObject(OBJ_STORYCANDLE, tx + 2, ty + 2);
}

void AddBookLever(int x1, int y1, int x2, int y2, _speech_id msg)
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

	if (QuestStatus(Q_BLIND))
		AddObject(OBJ_BLINDBOOK, xp, yp);
	if (QuestStatus(Q_WARLORD))
		AddObject(OBJ_STEELTOME, xp, yp);
	if (QuestStatus(Q_BLOOD)) {
		xp = 2 * setpc_x + 25;
		yp = 2 * setpc_y + 40;
		AddObject(OBJ_BLOODBOOK, xp, yp);
	}
	int ob = dObject[xp][yp] - 1;
	SetObjMapRange(ob, x1, y1, x2, y2, leverid);
	SetBookMsg(ob, msg);
	leverid++;
	object[ob]._oVar6 = object[ob]._oAnimFrame + 1;
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
		AddObject(o, xp, yp);
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
				AddObject(o, xp, yp);
				c++;
			}
			p = c / 2;
		}
	}
}

void AddL1Objs(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < y2; j++) {
		for (int i = x1; i < x2; i++) {
			int pn = dPiece[i][j];
			if (pn == 270)
				AddObject(OBJ_L1LIGHT, i, j);
			if (pn == 44 || pn == 51 || pn == 214)
				AddObject(OBJ_L1LDOOR, i, j);
			if (pn == 46 || pn == 56)
				AddObject(OBJ_L1RDOOR, i, j);
		}
	}
}

void add_crypt_objs(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < y2; j++) {
		for (int i = x1; i < x2; i++) {
			int pn = dPiece[i][j];
			if (pn == 77)
				AddObject(OBJ_L1LDOOR, i, j);
			if (pn == 80)
				AddObject(OBJ_L1RDOOR, i, j);
		}
	}
}

void AddL2Objs(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < y2; j++) {
		for (int i = x1; i < x2; i++) {
			int pn = dPiece[i][j];
			if (pn == 13 || pn == 541)
				AddObject(OBJ_L2LDOOR, i, j);
			if (pn == 17 || pn == 542)
				AddObject(OBJ_L2RDOOR, i, j);
		}
	}
}

void AddL3Objs(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j < y2; j++) {
		for (int i = x1; i < x2; i++) {
			int pn = dPiece[i][j];
			if (pn == 531)
				AddObject(OBJ_L3LDOOR, i, j);
			if (pn == 534)
				AddObject(OBJ_L3RDOOR, i, j);
		}
	}
}

bool TorchLocOK(int xp, int yp)
{
	return (dFlags[xp][yp] & BFLAG_POPULATED) == 0;
}

void AddL2Torches()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (!TorchLocOK(i, j))
				continue;

			int pn = dPiece[i][j];
			if (pn == 1 && GenerateRnd(3) == 0)
				AddObject(OBJ_TORCHL2, i, j);

			if (pn == 5 && GenerateRnd(3) == 0)
				AddObject(OBJ_TORCHR2, i, j);

			if (pn == 37 && GenerateRnd(10) == 0 && dObject[i - 1][j] == 0)
				AddObject(OBJ_TORCHL, i - 1, j);

			if (pn == 41 && GenerateRnd(10) == 0 && dObject[i][j - 1] == 0)
				AddObject(OBJ_TORCHR, i, j - 1);
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
			if (dObject[i][j] <= 0 || GenerateRnd(100) >= rndv)
				continue;

			int8_t oi = dObject[i][j] - 1;
			if (!AllObjects[object[oi]._otype].oTrapFlag)
				continue;

			if (GenerateRnd(2) == 0) {
				int xp = i - 1;
				while (!nSolidTable[dPiece[xp][j]]) // BUGFIX: check if xp >= 0
					xp--;

				if (!WallTrapLocOkK(xp, j) || i - xp <= 1)
					continue;

				AddObject(OBJ_TRAPL, xp, j);
				int8_t oiTrap = dObject[xp][j] - 1;
				object[oiTrap]._oVar1 = i;
				object[oiTrap]._oVar2 = j;
				object[oi]._oTrapFlag = true;
			} else {
				int yp = j - 1;
				while (!nSolidTable[dPiece[i][yp]]) // BUGFIX: check if yp >= 0
					yp--;

				if (!WallTrapLocOkK(i, yp) || j - yp <= 1)
					continue;

				AddObject(OBJ_TRAPR, i, yp);
				int8_t oiTrap = dObject[i][yp] - 1;
				object[oiTrap]._oVar1 = i;
				object[oiTrap]._oVar2 = j;
				object[oi]._oTrapFlag = true;
			}
		}
	}
}

void AddChestTraps()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) { // NOLINT(modernize-loop-convert)
			if (dObject[i][j] > 0) {
				int8_t oi = dObject[i][j] - 1;
				if (object[oi]._otype >= OBJ_CHEST1 && object[oi]._otype <= OBJ_CHEST3 && !object[oi]._oTrapFlag && GenerateRnd(100) < 10) {
					switch (object[oi]._otype) {
					case OBJ_CHEST1:
						object[oi]._otype = OBJ_TCHEST1;
						break;
					case OBJ_CHEST2:
						object[oi]._otype = OBJ_TCHEST2;
						break;
					case OBJ_CHEST3:
						object[oi]._otype = OBJ_TCHEST3;
						break;
					default:
						break;
					}
					object[oi]._oTrapFlag = true;
					if (leveltype == DTYPE_CATACOMBS) {
						object[oi]._oVar4 = GenerateRnd(2);
					} else {
						object[oi]._oVar4 = GenerateRnd(gbIsHellfire ? 6 : 3);
					}
				}
			}
		}
	}
}

void LoadMapObjects(const char *path, int startx, int starty, int x1, int y1, int w, int h, int leveridx)
{
	LoadMapObjsFlag = true;
	InitObjFlag = true;

	auto dunData = LoadFileInMem<uint16_t>(path);

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
				AddObject(ObjTypeConv[objectId], startx + 16 + i, starty + 16 + j);
				int oi = ObjIndex(startx + 16 + i, starty + 16 + j);
				SetObjMapRange(oi, x1, y1, x1 + w, y1 + h, leveridx);
			}
		}
	}

	InitObjFlag = false;
	LoadMapObjsFlag = false;
}

void LoadMapObjs(const char *path, int startx, int starty)
{
	LoadMapObjsFlag = true;
	InitObjFlag = true;

	auto dunData = LoadFileInMem<uint16_t>(path);

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
				AddObject(ObjTypeConv[objectId], startx + 16 + i, starty + 16 + j);
			}
		}
	}

	InitObjFlag = false;
	LoadMapObjsFlag = false;
}

void AddDiabObjs()
{
	LoadMapObjects("Levels\\L4Data\\diab1.DUN", 2 * diabquad1x, 2 * diabquad1y, diabquad2x, diabquad2y, 11, 12, 1);
	LoadMapObjects("Levels\\L4Data\\diab2a.DUN", 2 * diabquad2x, 2 * diabquad2y, diabquad3x, diabquad3y, 11, 11, 2);
	LoadMapObjects("Levels\\L4Data\\diab3a.DUN", 2 * diabquad3x, 2 * diabquad3y, diabquad4x, diabquad4y, 9, 9, 3);
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
	AddObject(OBJ_STORYCANDLE, xp - 2, yp + 1);
	AddObject(OBJ_STORYCANDLE, xp - 2, yp);
	AddObject(OBJ_STORYCANDLE, xp - 1, yp - 1);
	AddObject(OBJ_STORYCANDLE, xp + 1, yp - 1);
	AddObject(OBJ_STORYCANDLE, xp + 2, yp);
	AddObject(OBJ_STORYCANDLE, xp + 2, yp + 1);
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

void AddNakrulBook(int a1, int a2, int a3)
{
	AddCryptBook(OBJ_STORYBOOK, a1, a2, a3);
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
	AddObject(OBJ_STORYBOOK, xp, yp);
	AddObject(OBJ_STORYCANDLE, xp - 2, yp + 1);
	AddObject(OBJ_STORYCANDLE, xp - 2, yp);
	AddObject(OBJ_STORYCANDLE, xp - 1, yp - 1);
	AddObject(OBJ_STORYCANDLE, xp + 1, yp - 1);
	AddObject(OBJ_STORYCANDLE, xp + 2, yp);
	AddObject(OBJ_STORYCANDLE, xp + 2, yp + 1);
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
					AddObject(OBJ_TORTURE1, ii + 1, jj);
					break;
				case 1:
					AddObject(OBJ_TORTURE2, ii + 1, jj);
					break;
				case 2:
					AddObject(OBJ_TORTURE5, ii + 1, jj);
					break;
				}
				continue;
			}
			if (dungeon[i][j] == 2 && dungeon[i][j + 1] == 6) {
				switch (GenerateRnd(2)) {
				case 0:
					AddObject(OBJ_TORTURE3, ii, jj);
					break;
				case 1:
					AddObject(OBJ_TORTURE4, ii, jj);
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
	AddObject(OBJ_LAZSTAND, xp, yp);
	AddObject(OBJ_TNUDEM2, xp, yp + 2);
	AddObject(OBJ_STORYCANDLE, xp + 1, yp + 2);
	AddObject(OBJ_TNUDEM3, xp + 2, yp + 2);
	AddObject(OBJ_TNUDEW1, xp, yp - 2);
	AddObject(OBJ_STORYCANDLE, xp + 1, yp - 2);
	AddObject(OBJ_TNUDEW2, xp + 2, yp - 2);
	AddObject(OBJ_STORYCANDLE, xp - 1, yp - 1);
	AddObject(OBJ_TNUDEW3, xp - 1, yp);
	AddObject(OBJ_STORYCANDLE, xp - 1, yp + 1);
}

void InitObjects()
{
	ClrAllObjects();
	NaKrulTomeSequence = 0;
	if (currlevel == 16) {
		AddDiabObjs();
	} else {
		InitObjFlag = true;
		AdvanceRndSeed();
		if (currlevel == 9 && !gbIsMultiplayer)
			AddSlainHero();
		if (currlevel == quests[Q_MUSHROOM]._qlevel && quests[Q_MUSHROOM]._qactive == QUEST_INIT)
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
			if (QuestStatus(Q_BUTCHER))
				AddTortures();
			if (QuestStatus(Q_PWATER))
				AddCandles();
			if (QuestStatus(Q_LTBANNER))
				AddObject(OBJ_SIGNCHEST, 2 * setpc_x + 26, 2 * setpc_y + 19);
			InitRndLocBigObj(10, 15, OBJ_SARC);
			if (currlevel >= 21)
				add_crypt_objs(0, 0, MAXDUNX, MAXDUNY);
			else
				AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
			InitRndBarrels();
		}
		if (leveltype == DTYPE_CATACOMBS) {
			if (QuestStatus(Q_ROCK))
				InitRndLocObj5x5(1, 1, OBJ_STAND);
			if (QuestStatus(Q_SCHAMB))
				InitRndLocObj5x5(1, 1, OBJ_BOOK2R);
			AddL2Objs(0, 0, MAXDUNX, MAXDUNY);
			AddL2Torches();
			if (QuestStatus(Q_BLIND)) {
				_speech_id spId;
				switch (plr[myplr]._pClass) {
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
				quests[Q_BLIND]._qmsg = spId;
				AddBookLever(setpc_x, setpc_y, setpc_w + setpc_x + 1, setpc_h + setpc_y + 1, spId);
				LoadMapObjs("Levels\\L2Data\\Blind2.DUN", 2 * setpc_x, 2 * setpc_y);
			}
			if (QuestStatus(Q_BLOOD)) {
				_speech_id spId;
				switch (plr[myplr]._pClass) {
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
				quests[Q_BLOOD]._qmsg = spId;
				AddBookLever(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7, spId);
				AddObject(OBJ_PEDISTAL, 2 * setpc_x + 25, 2 * setpc_y + 32);
			}
			InitRndBarrels();
		}
		if (leveltype == DTYPE_CAVES) {
			AddL3Objs(0, 0, MAXDUNX, MAXDUNY);
			InitRndBarrels();
		}
		if (leveltype == DTYPE_HELL) {
			if (QuestStatus(Q_WARLORD)) {
				_speech_id spId;
				switch (plr[myplr]._pClass) {
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
				quests[Q_WARLORD]._qmsg = spId;
				AddBookLever(setpc_x, setpc_y, setpc_x + setpc_w, setpc_y + setpc_h, spId);
				LoadMapObjs("Levels\\L4Data\\Warlord.DUN", 2 * setpc_x, 2 * setpc_y);
			}
			if (QuestStatus(Q_BETRAYER) && !gbIsMultiplayer)
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
		InitObjFlag = false;
	}
}

void SetMapObjects(const uint16_t *dunData, int startx, int starty)
{
	bool filesLoaded[56];
	char filestr[32];

	ClrAllObjects();
	for (auto &fileLoaded : filesLoaded)
		fileLoaded = false;
	InitObjFlag = true;

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
				AddObject(ObjTypeConv[objectId], startx + 16 + i, starty + 16 + j);
			}
		}
	}

	InitObjFlag = false;
}

void DeleteObject(int oi, int i)
{
	int ox = object[oi].position.x;
	int oy = object[oi].position.y;
	dObject[ox][oy] = 0;
	objectavail[-nobjects + MAXOBJECTS] = oi;
	nobjects--;
	if (nobjects > 0 && i != nobjects)
		objectactive[i] = objectactive[nobjects];
}

void SetupObject(int i, int x, int y, _object_id ot)
{
	object[i]._otype = ot;
	object_graphic_id ofi = AllObjects[ot].ofindex;
	object[i].position = { x, y };

	const auto &found = std::find(std::begin(ObjFileList), std::end(ObjFileList), ofi);
	if (found == std::end(ObjFileList)) {
		LogCritical("Unable to find object_graphic_id {} in list of objects to load, level generation error.", ofi);
		return;
	}

	const int j = std::distance(std::begin(ObjFileList), found);

	object[i]._oAnimData = pObjCels[j].get();
	object[i]._oAnimFlag = AllObjects[ot].oAnimFlag;
	if (AllObjects[ot].oAnimFlag != 0) {
		object[i]._oAnimDelay = AllObjects[ot].oAnimDelay;
		object[i]._oAnimCnt = GenerateRnd(AllObjects[ot].oAnimDelay);
		object[i]._oAnimLen = AllObjects[ot].oAnimLen;
		object[i]._oAnimFrame = GenerateRnd(AllObjects[ot].oAnimLen - 1) + 1;
	} else {
		object[i]._oAnimDelay = 1000;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimLen = AllObjects[ot].oAnimLen;
		object[i]._oAnimFrame = AllObjects[ot].oAnimDelay;
	}
	object[i]._oAnimWidth = AllObjects[ot].oAnimWidth;
	object[i]._oSolidFlag = AllObjects[ot].oSolidFlag;
	object[i]._oMissFlag = AllObjects[ot].oMissFlag;
	object[i]._oLight = AllObjects[ot].oLightFlag;
	object[i]._oDelFlag = false;
	object[i]._oBreak = AllObjects[ot].oBreak;
	object[i]._oSelFlag = AllObjects[ot].oSelFlag;
	object[i]._oPreFlag = false;
	object[i]._oTrapFlag = false;
	object[i]._oDoorFlag = false;
}

void SetObjMapRange(int i, int x1, int y1, int x2, int y2, int v)
{
	object[i]._oVar1 = x1;
	object[i]._oVar2 = y1;
	object[i]._oVar3 = x2;
	object[i]._oVar4 = y2;
	object[i]._oVar8 = v;
}

void SetBookMsg(int i, _speech_id msg)
{
	object[i]._oVar7 = msg;
}

void AddL1Door(int i, int x, int y, int ot)
{
	object[i]._oDoorFlag = true;
	if (ot == 1) {
		object[i]._oVar1 = dPiece[x][y];
		object[i]._oVar2 = dPiece[x][y - 1];
	} else {
		object[i]._oVar1 = dPiece[x][y];
		object[i]._oVar2 = dPiece[x - 1][y];
	}
	object[i]._oVar4 = 0;
}

void AddSCambBook(int i)
{
	object[i]._oVar1 = setpc_x;
	object[i]._oVar2 = setpc_y;
	object[i]._oVar3 = setpc_w + setpc_x + 1;
	object[i]._oVar4 = setpc_h + setpc_y + 1;
	object[i]._oVar6 = object[i]._oAnimFrame + 1;
}

void AddChest(int i, int t)
{
	if (GenerateRnd(2) == 0)
		object[i]._oAnimFrame += 3;
	object[i]._oRndSeed = AdvanceRndSeed();
	switch (t) {
	case OBJ_CHEST1:
	case OBJ_TCHEST1:
		if (setlevel) {
			object[i]._oVar1 = 1;
			break;
		}
		object[i]._oVar1 = GenerateRnd(2);
		break;
	case OBJ_TCHEST2:
	case OBJ_CHEST2:
		if (setlevel) {
			object[i]._oVar1 = 2;
			break;
		}
		object[i]._oVar1 = GenerateRnd(3);
		break;
	case OBJ_TCHEST3:
	case OBJ_CHEST3:
		if (setlevel) {
			object[i]._oVar1 = 3;
			break;
		}
		object[i]._oVar1 = GenerateRnd(4);
		break;
	}
	object[i]._oVar2 = GenerateRnd(8);
}

void AddL2Door(int i, int x, int y, int ot)
{
	object[i]._oDoorFlag = true;
	if (ot == OBJ_L2LDOOR)
		ObjSetMicro(x, y, 538);
	else
		ObjSetMicro(x, y, 540);
	dSpecial[x][y] = 0;
	object[i]._oVar4 = 0;
}

void AddL3Door(int i, int x, int y, int ot)
{
	object[i]._oDoorFlag = true;
	if (ot == OBJ_L3LDOOR)
		ObjSetMicro(x, y, 531);
	else
		ObjSetMicro(x, y, 534);
	object[i]._oVar4 = 0;
}

void AddSarc(int i)
{
	dObject[object[i].position.x][object[i].position.y - 1] = -(i + 1);
	object[i]._oVar1 = GenerateRnd(10);
	object[i]._oRndSeed = AdvanceRndSeed();
	if (object[i]._oVar1 >= 8)
		object[i]._oVar2 = PreSpawnSkeleton();
}

void AddFlameTrap(int i)
{
	object[i]._oVar1 = trapid;
	object[i]._oVar2 = 0;
	object[i]._oVar3 = trapdir;
	object[i]._oVar4 = 0;
}

void AddFlameLvr(int i)
{
	object[i]._oVar1 = trapid;
	object[i]._oVar2 = MIS_FLAMEC;
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
		object[i]._oVar3 = MIS_ARROW;
	if (mt == 1)
		object[i]._oVar3 = MIS_FIREBOLT;
	if (mt == 2)
		object[i]._oVar3 = MIS_LIGHTCTRL;
	object[i]._oVar4 = 0;
}

void AddObjLight(int i, int r)
{
	if (InitObjFlag) {
		DoLighting(object[i].position, r, -1);
		object[i]._oVar1 = -1;
	} else {
		object[i]._oVar1 = 0;
	}
}

void AddBarrel(int i, int t)
{
	object[i]._oVar1 = 0;
	object[i]._oRndSeed = AdvanceRndSeed();
	object[i]._oVar2 = (t == OBJ_BARRELEX) ? 0 : GenerateRnd(10);
	object[i]._oVar3 = GenerateRnd(3);

	if (object[i]._oVar2 >= 8)
		object[i]._oVar4 = PreSpawnSkeleton();
}

void AddShrine(int i)
{
	bool slist[NumberOfShrineTypes];

	object[i]._oPreFlag = true;

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

	object[i]._oVar1 = val;
	if (GenerateRnd(2) != 0) {
		object[i]._oAnimFrame = 12;
		object[i]._oAnimLen = 22;
	}
}

void AddBookcase(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
	object[i]._oPreFlag = true;
}

void AddBookstand(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddBloodFtn(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddPurifyingFountain(int i)
{
	int ox = object[i].position.x;
	int oy = object[i].position.y;
	dObject[ox][oy - 1] = -(i + 1);
	dObject[ox - 1][oy] = -(i + 1);
	dObject[ox - 1][oy - 1] = -(i + 1);
	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddArmorStand(int i)
{
	if (!armorFlag) {
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = 0;
	}

	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddGoatShrine(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddCauldron(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddMurkyFountain(int i)
{
	int ox = object[i].position.x;
	int oy = object[i].position.y;
	dObject[ox][oy - 1] = -(i + 1);
	dObject[ox - 1][oy] = -(i + 1);
	dObject[ox - 1][oy - 1] = -(i + 1);
	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddTearFountain(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddDecap(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
	object[i]._oAnimFrame = GenerateRnd(8) + 1;
	object[i]._oPreFlag = true;
}

void AddVilebook(int i)
{
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		object[i]._oAnimFrame = 4;
	}
}

void AddMagicCircle(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
	object[i]._oPreFlag = true;
	object[i]._oVar6 = 0;
	object[i]._oVar5 = 1;
}

void AddBrnCross(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddPedistal(int i)
{
	object[i]._oVar1 = setpc_x;
	object[i]._oVar2 = setpc_y;
	object[i]._oVar3 = setpc_x + setpc_w;
	object[i]._oVar4 = setpc_y + setpc_h;
	object[i]._oVar6 = 0;
}

void AddStoryBook(int i)
{
	SetRndSeed(glSeedTbl[16]);

	object[i]._oVar1 = GenerateRnd(3);
	if (currlevel == 4)
		object[i]._oVar2 = StoryText[object[i]._oVar1][0];
	else if (currlevel == 8)
		object[i]._oVar2 = StoryText[object[i]._oVar1][1];
	else if (currlevel == 12)
		object[i]._oVar2 = StoryText[object[i]._oVar1][2];
	object[i]._oVar3 = (currlevel / 4) + 3 * object[i]._oVar1 - 1;
	object[i]._oAnimFrame = 5 - 2 * object[i]._oVar1;
	object[i]._oVar4 = object[i]._oAnimFrame + 1;
}

void AddWeaponRack(int i)
{
	if (!weaponFlag) {
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = 0;
	}
	object[i]._oRndSeed = AdvanceRndSeed();
}

void AddTorturedBody(int i)
{
	object[i]._oRndSeed = AdvanceRndSeed();
	object[i]._oAnimFrame = GenerateRnd(4) + 1;
	object[i]._oPreFlag = true;
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

	if (nobjects < MAXOBJECTS) {
		int i = objectavail[0];
		GetRndObjLoc(5, &x, &y);
		dObject[x + 1][y + 1] = -(i + 1);
		dObject[x + 2][y + 1] = -(i + 1);
		dObject[x + 1][y + 2] = -(i + 1);
		AddObject(OBJ_MUSHPATCH, x + 2, y + 2);
	}
}

void AddSlainHero()
{
	int x;
	int y;

	GetRndObjLoc(5, &x, &y);
	AddObject(OBJ_SLAINHERO, x + 2, y + 2);
}

void AddCryptBook(_object_id ot, int v2, int ox, int oy)
{
	if (nobjects >= MAXOBJECTS)
		return;

	int oi = objectavail[0];
	objectavail[0] = objectavail[MAXOBJECTS - 1 - nobjects];
	objectactive[nobjects] = oi;
	dObject[ox][oy] = oi + 1;
	SetupObject(oi, ox, oy, ot);
	AddCryptObject(oi, v2);
	nobjects++;
}

void AddCryptObject(int i, int a2)
{
	if (a2 > 5) {
		switch (a2) {
		case 6:
			switch (plr[myplr]._pClass) {
			case HeroClass::Warrior:
			case HeroClass::Barbarian:
				object[i]._oVar2 = TEXT_BOOKA;
				break;
			case HeroClass::Rogue:
				object[i]._oVar2 = TEXT_RBOOKA;
				break;
			case HeroClass::Sorcerer:
				object[i]._oVar2 = TEXT_MBOOKA;
				break;
			case HeroClass::Monk:
				object[i]._oVar2 = TEXT_OBOOKA;
				break;
			case HeroClass::Bard:
				object[i]._oVar2 = TEXT_BBOOKA;
				break;
			}
			break;
		case 7:
			switch (plr[myplr]._pClass) {
			case HeroClass::Warrior:
			case HeroClass::Barbarian:
				object[i]._oVar2 = TEXT_BOOKB;
				break;
			case HeroClass::Rogue:
				object[i]._oVar2 = TEXT_RBOOKB;
				break;
			case HeroClass::Sorcerer:
				object[i]._oVar2 = TEXT_MBOOKB;
				break;
			case HeroClass::Monk:
				object[i]._oVar2 = TEXT_OBOOKB;
				break;
			case HeroClass::Bard:
				object[i]._oVar2 = TEXT_BBOOKB;
				break;
			}
			break;
		case 8:
			switch (plr[myplr]._pClass) {
			case HeroClass::Warrior:
			case HeroClass::Barbarian:
				object[i]._oVar2 = TEXT_BOOKC;
				break;
			case HeroClass::Rogue:
				object[i]._oVar2 = TEXT_RBOOKC;
				break;
			case HeroClass::Sorcerer:
				object[i]._oVar2 = TEXT_MBOOKC;
				break;
			case HeroClass::Monk:
				object[i]._oVar2 = TEXT_OBOOKC;
				break;
			case HeroClass::Bard:
				object[i]._oVar2 = TEXT_BBOOKC;
				break;
			}
			break;
		}
		object[i]._oVar3 = 15;
		object[i]._oVar8 = a2;
	} else {
		object[i]._oVar2 = a2 + TEXT_SKLJRN;
		object[i]._oVar3 = a2 + 9;
		object[i]._oVar8 = 0;
	}
	object[i]._oVar1 = 1;
	object[i]._oAnimFrame = 5 - 2 * object[i]._oVar1;
	object[i]._oVar4 = object[i]._oAnimFrame + 1;
}

void AddObject(_object_id ot, int ox, int oy)
{
	if (nobjects >= MAXOBJECTS)
		return;

	int oi = objectavail[0];
	objectavail[0] = objectavail[MAXOBJECTS - 1 - nobjects];
	objectactive[nobjects] = oi;
	dObject[ox][oy] = oi + 1;
	SetupObject(oi, ox, oy, ot);
	switch (ot) {
	case OBJ_L1LIGHT:
	case OBJ_SKFIRE:
	case OBJ_CANDLE1:
	case OBJ_CANDLE2:
	case OBJ_BOOKCANDLE:
		AddObjLight(oi, 5);
		break;
	case OBJ_STORYCANDLE:
		AddObjLight(oi, 3);
		break;
	case OBJ_TORCHL:
	case OBJ_TORCHR:
	case OBJ_TORCHL2:
	case OBJ_TORCHR2:
		AddObjLight(oi, 8);
		break;
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		AddL1Door(oi, ox, oy, ot);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		AddL2Door(oi, ox, oy, ot);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		AddL3Door(oi, ox, oy, ot);
		break;
	case OBJ_BOOK2R:
		AddSCambBook(oi);
		break;
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
		AddChest(oi, ot);
		break;
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		AddChest(oi, ot);
		object[oi]._oTrapFlag = true;
		if (leveltype == DTYPE_CATACOMBS) {
			object[oi]._oVar4 = GenerateRnd(2);
		} else {
			object[oi]._oVar4 = GenerateRnd(3);
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
		object[oi]._oAnimFrame = 1;
		break;
	case OBJ_TRAPL:
	case OBJ_TRAPR:
		AddTrap(oi);
		break;
	case OBJ_BARREL:
	case OBJ_BARRELEX:
		AddBarrel(oi, ot);
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
		AddObjLight(oi, 5);
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
	nobjects++;
}

void Obj_Light(int i, int lr)
{
	if (object[i]._oVar1 == -1) {
		return;
	}

	bool turnon = false;
	int ox = object[i].position.x;
	int oy = object[i].position.y;
	int tr = lr + 10;
	if (!lightflag) {
		for (int p = 0; p < MAX_PLRS && !turnon; p++) {
			if (plr[p].plractive) {
				if (currlevel == plr[p].plrlevel) {
					int dx = abs(plr[p].position.tile.x - ox);
					int dy = abs(plr[p].position.tile.y - oy);
					if (dx < tr && dy < tr)
						turnon = true;
				}
			}
		}
	}
	if (turnon) {
		if (object[i]._oVar1 == 0)
			object[i]._olid = AddLight(object[i].position, lr);
		object[i]._oVar1 = 1;
	} else {
		if (object[i]._oVar1 == 1)
			AddUnLight(object[i]._olid);
		object[i]._oVar1 = 0;
	}
}

void Obj_Circle(int i)
{
	if (plr[myplr].position.tile != object[i].position) {
		if (object[i]._otype == OBJ_MCIRCLE1)
			object[i]._oAnimFrame = 1;
		if (object[i]._otype == OBJ_MCIRCLE2)
			object[i]._oAnimFrame = 3;
		object[i]._oVar6 = 0;
		return;
	}

	int ox = object[i].position.x;
	int oy = object[i].position.y;
	if (object[i]._otype == OBJ_MCIRCLE1)
		object[i]._oAnimFrame = 2;
	if (object[i]._otype == OBJ_MCIRCLE2)
		object[i]._oAnimFrame = 4;
	if (ox == 45 && oy == 47) {
		object[i]._oVar6 = 2;
	} else if (ox == 26 && oy == 46) {
		object[i]._oVar6 = 1;
	} else {
		object[i]._oVar6 = 0;
	}
	if (ox == 35 && oy == 36 && object[i]._oVar5 == 3) {
		object[i]._oVar6 = 4;
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE && quests[Q_BETRAYER]._qvar1 <= 4) // BUGFIX stepping on the circle again will break the quest state (fixed)
			quests[Q_BETRAYER]._qvar1 = 4;
		AddMissile(plr[myplr].position.tile, { 35, 46 }, plr[myplr]._pdir, MIS_RNDTELEPORT, TARGET_MONSTERS, myplr, 0, 0);
		track_repeat_walk(false);
		sgbMouseDown = CLICK_NONE;
		ClrPlrPath(plr[myplr]);
		StartStand(myplr, DIR_S);
	}
}

void Obj_StopAnim(int i)
{
	if (object[i]._oAnimFrame == object[i]._oAnimLen) {
		object[i]._oAnimCnt = 0;
		object[i]._oAnimDelay = 1000;
	}
}

void Obj_Door(int i)
{
	if (object[i]._oVar4 == 0) {
		object[i]._oSelFlag = 3;
		object[i]._oMissFlag = false;
		return;
	}

	int dx = object[i].position.x;
	int dy = object[i].position.y;
	bool dok = dMonster[dx][dy] == 0;
	dok = dok && dItem[dx][dy] == 0;
	dok = dok && dDead[dx][dy] == 0;
	dok = dok && dPlayer[dx][dy] == 0;
	object[i]._oSelFlag = 2;
	object[i]._oVar4 = dok ? 1 : 2;
	object[i]._oMissFlag = true;
}

void Obj_Sarc(int i)
{
	if (object[i]._oAnimFrame == object[i]._oAnimLen)
		object[i]._oAnimFlag = 0;
}

void ActivateTrapLine(int ttype, int tid)
{
	for (int i = 0; i < nobjects; i++) {
		int oi = objectactive[i];
		if (object[oi]._otype == ttype && object[oi]._oVar1 == tid) {
			object[oi]._oVar4 = 1;
			object[oi]._oAnimFlag = 1;
			object[oi]._oAnimDelay = 1;
			object[oi]._olid = AddLight(object[oi].position, 1);
		}
	}
}

void Obj_FlameTrap(int i)
{
	if (object[i]._oVar2 != 0) {
		if (object[i]._oVar4 != 0) {
			object[i]._oAnimFrame--;
			if (object[i]._oAnimFrame == 1) {
				object[i]._oVar4 = 0;
				AddUnLight(object[i]._olid);
			} else if (object[i]._oAnimFrame <= 4) {
				ChangeLightRadius(object[i]._olid, object[i]._oAnimFrame);
			}
		}
	} else if (object[i]._oVar4 == 0) {
		if (object[i]._oVar3 == 2) {
			int x = object[i].position.x - 2;
			int y = object[i].position.y;
			for (int j = 0; j < 5; j++) {
				if (dPlayer[x][y] != 0 || dMonster[x][y] != 0)
					object[i]._oVar4 = 1;
				x++;
			}
		} else {
			int x = object[i].position.x;
			int y = object[i].position.y - 2;
			for (int k = 0; k < 5; k++) {
				if (dPlayer[x][y] != 0 || dMonster[x][y] != 0)
					object[i]._oVar4 = 1;
				y++;
			}
		}
		if (object[i]._oVar4 != 0)
			ActivateTrapLine(object[i]._otype, object[i]._oVar1);
	} else {
		int damage[4] = { 6, 8, 10, 12 };

		int mindam = damage[leveltype - 1];
		int maxdam = mindam * 2;

		int x = object[i].position.x;
		int y = object[i].position.y;
		if (dMonster[x][y] > 0)
			MonsterTrapHit(dMonster[x][y] - 1, mindam / 2, maxdam / 2, 0, MIS_FIREWALLC, false);
		if (dPlayer[x][y] > 0) {
			bool unused;
			PlayerMHit(dPlayer[x][y] - 1, -1, 0, mindam, maxdam, MIS_FIREWALLC, false, 0, &unused);
		}

		if (object[i]._oAnimFrame == object[i]._oAnimLen)
			object[i]._oAnimFrame = 11;
		if (object[i]._oAnimFrame <= 5)
			ChangeLightRadius(object[i]._olid, object[i]._oAnimFrame);
	}
}

void Obj_Trap(int i)
{
	if (object[i]._oVar4 != 0)
		return;

	int oti = dObject[object[i]._oVar1][object[i]._oVar2] - 1;
	switch (object[oti]._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (object[oti]._oVar4 == 0)
			return;
		break;
	case OBJ_LEVER:
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
	case OBJ_SWITCHSKL:
	case OBJ_SARC:
		if (object[oti]._oSelFlag != 0)
			return;
		break;
	default:
		return;
	}

	object[i]._oVar4 = 1;
	Point target = object[oti].position;
	for (int y = target.y - 1; y <= object[oti].position.y + 1; y++) {
		for (int x = object[oti].position.x - 1; x <= object[oti].position.x + 1; x++) {
			if (dPlayer[x][y] != 0) {
				target.x = x;
				target.y = y;
			}
		}
	}
	if (!deltaload) {
		Direction dir = GetDirection(object[i].position, target);
		AddMissile(object[i].position, target, dir, object[i]._oVar3, TARGET_PLAYERS, -1, 0, 0);
		PlaySfxLoc(IS_TRAP, object[oti].position);
	}
	object[oti]._oTrapFlag = false;
}

void Obj_BCrossDamage(int i)
{
	int damage[4] = { 6, 8, 10, 12 };

	if (plr[myplr]._pmode == PM_DEATH)
		return;

	int8_t fireResist = plr[myplr]._pFireResist;
	if (fireResist > 0)
		damage[leveltype - 1] -= fireResist * damage[leveltype - 1] / 100;

	if (plr[myplr].position.tile.x != object[i].position.x || plr[myplr].position.tile.y != object[i].position.y - 1)
		return;

	ApplyPlrDamage(myplr, 0, 0, damage[leveltype - 1]);
	if (plr[myplr]._pHitPoints >> 6 > 0) {
		plr[myplr].Say(HeroSpeech::Argh);
	}
}

void ProcessObjects()
{
	for (int i = 0; i < nobjects; ++i) {
		int oi = objectactive[i];
		switch (object[oi]._otype) {
		case OBJ_L1LIGHT:
			Obj_Light(oi, 10);
			break;
		case OBJ_SKFIRE:
		case OBJ_CANDLE2:
		case OBJ_BOOKCANDLE:
			Obj_Light(oi, 5);
			break;
		case OBJ_STORYCANDLE:
			Obj_Light(oi, 3);
			break;
		case OBJ_CRUX1:
		case OBJ_CRUX2:
		case OBJ_CRUX3:
		case OBJ_BARREL:
		case OBJ_BARRELEX:
		case OBJ_SHRINEL:
		case OBJ_SHRINER:
			Obj_StopAnim(oi);
			break;
		case OBJ_L1LDOOR:
		case OBJ_L1RDOOR:
		case OBJ_L2LDOOR:
		case OBJ_L2RDOOR:
		case OBJ_L3LDOOR:
		case OBJ_L3RDOOR:
			Obj_Door(oi);
			break;
		case OBJ_TORCHL:
		case OBJ_TORCHR:
		case OBJ_TORCHL2:
		case OBJ_TORCHR2:
			Obj_Light(oi, 8);
			break;
		case OBJ_SARC:
			Obj_Sarc(oi);
			break;
		case OBJ_FLAMEHOLE:
			Obj_FlameTrap(oi);
			break;
		case OBJ_TRAPL:
		case OBJ_TRAPR:
			Obj_Trap(oi);
			break;
		case OBJ_MCIRCLE1:
		case OBJ_MCIRCLE2:
			Obj_Circle(oi);
			break;
		case OBJ_BCROSS:
		case OBJ_TBCROSS:
			Obj_Light(oi, 10);
			Obj_BCrossDamage(oi);
			break;
		default:
			break;
		}
		if (object[oi]._oAnimFlag == 0)
			continue;

		object[oi]._oAnimCnt++;

		if (object[oi]._oAnimCnt < object[oi]._oAnimDelay)
			continue;

		object[oi]._oAnimCnt = 0;
		object[oi]._oAnimFrame++;
		if (object[oi]._oAnimFrame > object[oi]._oAnimLen)
			object[oi]._oAnimFrame = 1;
	}

	for (int i = 0; i < nobjects;) {
		int oi = objectactive[i];
		if (object[oi]._oDelFlag) {
			DeleteObject(oi, i);
		} else {
			i++;
		}
	}
}

void ObjSetMicro(int dx, int dy, int pn)
{
	dPiece[dx][dy] = pn;
	pn--;

	int blocks = leveltype != DTYPE_HELL ? 10 : 16;

	uint16_t *piece = &pLevelPieces[blocks * pn];
	MICROS &micros = dpiece_defs_map_2[dx][dy];

	for (int i = 0; i < blocks; i++) {
		micros.mt[i] = SDL_SwapLE16(piece[blocks - 2 + (i & 1) - (i & 0xE)]);
	}
}

void objects_set_door_piece(int x, int y)
{
	int pn = dPiece[x][y] - 1;

	uint16_t *piece = &pLevelPieces[10 * pn + 8];

	dpiece_defs_map_2[x][y].mt[0] = SDL_SwapLE16(piece[0]);
	dpiece_defs_map_2[x][y].mt[1] = SDL_SwapLE16(piece[1]);
}

void ObjSetMini(int x, int y, int v)
{
	MegaTile mega = pMegaTiles[v - 1];

	int xx = 2 * x + 16;
	int yy = 2 * y + 16;

	ObjSetMicro(xx + 0, yy + 0, SDL_SwapLE16(mega.micro1) + 1);
	ObjSetMicro(xx + 1, yy + 0, SDL_SwapLE16(mega.micro2) + 1);
	ObjSetMicro(xx + 0, yy + 1, SDL_SwapLE16(mega.micro3) + 1);
	ObjSetMicro(xx + 1, yy + 1, SDL_SwapLE16(mega.micro4) + 1);
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

void DoorSet(int oi, int dx, int dy)
{
	int pn = dPiece[dx][dy];
	if (currlevel < 17) {
		if (pn == 43)
			ObjSetMicro(dx, dy, 392);
		if (pn == 45)
			ObjSetMicro(dx, dy, 394);
		if (pn == 50 && object[oi]._otype == OBJ_L1LDOOR)
			ObjSetMicro(dx, dy, 411);
		if (pn == 50 && object[oi]._otype == OBJ_L1RDOOR)
			ObjSetMicro(dx, dy, 412);
		if (pn == 54)
			ObjSetMicro(dx, dy, 397);
		if (pn == 55)
			ObjSetMicro(dx, dy, 398);
		if (pn == 61)
			ObjSetMicro(dx, dy, 399);
		if (pn == 67)
			ObjSetMicro(dx, dy, 400);
		if (pn == 68)
			ObjSetMicro(dx, dy, 401);
		if (pn == 69)
			ObjSetMicro(dx, dy, 403);
		if (pn == 70)
			ObjSetMicro(dx, dy, 404);
		if (pn == 72)
			ObjSetMicro(dx, dy, 406);
		if (pn == 212)
			ObjSetMicro(dx, dy, 407);
		if (pn == 354)
			ObjSetMicro(dx, dy, 409);
		if (pn == 355)
			ObjSetMicro(dx, dy, 410);
		if (pn == 411)
			ObjSetMicro(dx, dy, 396);
		if (pn == 412)
			ObjSetMicro(dx, dy, 396);
	} else {
		if (pn == 75)
			ObjSetMicro(dx, dy, 204);
		if (pn == 79)
			ObjSetMicro(dx, dy, 208);
		if (pn == 86 && object[oi]._otype == OBJ_L1LDOOR) {
			ObjSetMicro(dx, dy, 232);
		}
		if (pn == 86 && object[oi]._otype == OBJ_L1RDOOR) {
			ObjSetMicro(dx, dy, 234);
		}
		if (pn == 91)
			ObjSetMicro(dx, dy, 215);
		if (pn == 93)
			ObjSetMicro(dx, dy, 218);
		if (pn == 99)
			ObjSetMicro(dx, dy, 220);
		if (pn == 111)
			ObjSetMicro(dx, dy, 222);
		if (pn == 113)
			ObjSetMicro(dx, dy, 224);
		if (pn == 115)
			ObjSetMicro(dx, dy, 226);
		if (pn == 117)
			ObjSetMicro(dx, dy, 228);
		if (pn == 119)
			ObjSetMicro(dx, dy, 230);
		if (pn == 232)
			ObjSetMicro(dx, dy, 212);
		if (pn == 234)
			ObjSetMicro(dx, dy, 212);
	}
}

void RedoPlayerVision()
{
	for (auto &player : plr) {
		if (player.plractive && currlevel == player.plrlevel) {
			ChangeVisionXY(player._pvid, player.position.tile);
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
static inline bool IsDoorClear(const Point &doorPosition)
{
	return dDead[doorPosition.x][doorPosition.y] == 0
	    && dMonster[doorPosition.x][doorPosition.y] == 0
	    && dItem[doorPosition.x][doorPosition.y] == 0;
}

void OperateL1RDoor(int pnum, int oi, bool sendflag)
{
	ObjectStruct &door = object[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (currlevel < 21) {
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, door.position);
			ObjSetMicro(door.position.x, door.position.y, 395);
		} else {
			if (!deltaload)
				PlaySfxLoc(IS_CROPEN, door.position);
			ObjSetMicro(door.position.x, door.position.y, 209);
		}
		if (currlevel < 17) {
			dSpecial[door.position.x][door.position.y] = 8;
		} else {
			dSpecial[door.position.x][door.position.y] = 2;
		}
		objects_set_door_piece(door.position.x, door.position.y - 1);
		door._oAnimFrame += 2;
		door._oPreFlag = true;
		DoorSet(oi, door.position.x - 1, door.position.y);
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
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position.x, door.position.y, door._oVar1);
		if (currlevel < 17) {
			if (door._oVar2 != 50) {
				ObjSetMicro(door.position.x - 1, door.position.y, door._oVar2);
			} else {
				if (dPiece[door.position.x - 1][door.position.y] == 396)
					ObjSetMicro(door.position.x - 1, door.position.y, 411);
				else
					ObjSetMicro(door.position.x - 1, door.position.y, 50);
			}
		} else {
			if (door._oVar2 != 86) {
				ObjSetMicro(door.position.x - 1, door.position.y, door._oVar2);
			} else {
				if (dPiece[door.position.x - 1][door.position.y] == 210)
					ObjSetMicro(door.position.x - 1, door.position.y, 232);
				else
					ObjSetMicro(door.position.x - 1, door.position.y, 86);
			}
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
	ObjectStruct &door = object[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (currlevel < 21) {
			if (!deltaload)
				PlaySfxLoc(IS_DOOROPEN, door.position);
			if (door._oVar1 == 214)
				ObjSetMicro(door.position.x, door.position.y, 408);
			else
				ObjSetMicro(door.position.x, door.position.y, 393);
		} else {
			if (!deltaload)
				PlaySfxLoc(IS_CROPEN, door.position);
			ObjSetMicro(door.position.x, door.position.y, 206);
		}
		if (currlevel < 17) {
			dSpecial[door.position.x][door.position.y] = 7;
		} else {
			dSpecial[door.position.x][door.position.y] = 1;
		}
		objects_set_door_piece(door.position.x - 1, door.position.y);
		door._oAnimFrame += 2;
		door._oPreFlag = true;
		DoorSet(oi, door.position.x, door.position.y - 1);
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
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position.x, door.position.y, door._oVar1);
		if (currlevel < 17) {
			if (door._oVar2 != 50) {
				ObjSetMicro(door.position.x, door.position.y - 1, door._oVar2);
			} else {
				if (dPiece[door.position.x][door.position.y - 1] == 396)
					ObjSetMicro(door.position.x, door.position.y - 1, 412);
				else
					ObjSetMicro(door.position.x, door.position.y - 1, 50);
			}
		} else {
			if (door._oVar2 != 86) {
				ObjSetMicro(door.position.x, door.position.y - 1, door._oVar2);
			} else {
				if (dPiece[door.position.x][door.position.y - 1] == 210)
					ObjSetMicro(door.position.x, door.position.y - 1, 234);
				else
					ObjSetMicro(door.position.x, door.position.y - 1, 86);
			}
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
	ObjectStruct &door = object[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, door.position);
		ObjSetMicro(door.position.x, door.position.y, 17);
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
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position.x, door.position.y, 540);
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
	ObjectStruct &door = object[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, door.position);
		ObjSetMicro(door.position.x, door.position.y, 13);
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
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position.x, door.position.y, 538);
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
	ObjectStruct &door = object[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, door.position);
		ObjSetMicro(door.position.x, door.position.y, 541);
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
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position.x, door.position.y, 534);
		door._oAnimFrame -= 2;
		door._oPreFlag = false;
		RedoPlayerVision();
	} else {
		door._oVar4 = 2;
	}
}

void OperateL3LDoor(int pnum, int oi, bool sendflag)
{
	ObjectStruct &door = object[oi];

	if (door._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, door.position);
		return;
	}

	if (door._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, door.position);
		ObjSetMicro(door.position.x, door.position.y, 538);
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
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(true, CMD_CLOSEDOOR, oi);
		door._oVar4 = 0;
		door._oSelFlag = 3;
		ObjSetMicro(door.position.x, door.position.y, 531);
		door._oAnimFrame -= 2;
		door._oPreFlag = false;
		RedoPlayerVision();
	} else {
		door._oVar4 = 2;
	}
}

void MonstCheckDoors(int m)
{
	int mx = monster[m].position.tile.x;
	int my = monster[m].position.tile.y;
	if (dObject[mx - 1][my - 1] != 0
	    || dObject[mx][my - 1] != 0
	    || dObject[mx + 1][my - 1] != 0
	    || dObject[mx - 1][my] != 0
	    || dObject[mx + 1][my] != 0
	    || dObject[mx - 1][my + 1] != 0
	    || dObject[mx][my + 1] != 0
	    || dObject[mx + 1][my + 1] != 0) {
		for (int i = 0; i < nobjects; i++) {
			int oi = objectactive[i];
			if ((object[oi]._otype == OBJ_L1LDOOR || object[oi]._otype == OBJ_L1RDOOR) && object[oi]._oVar4 == 0) {
				int dpx = abs(object[oi].position.x - mx);
				int dpy = abs(object[oi].position.y - my);
				if (dpx == 1 && dpy <= 1 && object[oi]._otype == OBJ_L1LDOOR)
					OperateL1LDoor(myplr, oi, true);
				if (dpx <= 1 && dpy == 1 && object[oi]._otype == OBJ_L1RDOOR)
					OperateL1RDoor(myplr, oi, true);
			}
			if ((object[oi]._otype == OBJ_L2LDOOR || object[oi]._otype == OBJ_L2RDOOR) && object[oi]._oVar4 == 0) {
				int dpx = abs(object[oi].position.x - mx);
				int dpy = abs(object[oi].position.y - my);
				if (dpx == 1 && dpy <= 1 && object[oi]._otype == OBJ_L2LDOOR)
					OperateL2LDoor(myplr, oi, true);
				if (dpx <= 1 && dpy == 1 && object[oi]._otype == OBJ_L2RDOOR)
					OperateL2RDoor(myplr, oi, true);
			}
			if ((object[oi]._otype == OBJ_L3LDOOR || object[oi]._otype == OBJ_L3RDOOR) && object[oi]._oVar4 == 0) {
				int dpx = abs(object[oi].position.x - mx);
				int dpy = abs(object[oi].position.y - my);
				if (dpx == 1 && dpy <= 1 && object[oi]._otype == OBJ_L3RDOOR)
					OperateL3RDoor(myplr, oi, true);
				if (dpx <= 1 && dpy == 1 && object[oi]._otype == OBJ_L3LDOOR)
					OperateL3LDoor(myplr, oi, true);
			}
		}
	}
}

void ObjChangeMap(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j <= y2; j++) {
		for (int i = x1; i <= x2; i++) {
			ObjSetMini(i, j, pdungeon[i][j]);
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
			ObjSetMini(i, j, pdungeon[i][j]);
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

void OperateL1Door(int pnum, int i, bool sendflag)
{
	int dpx = abs(object[i].position.x - plr[pnum].position.tile.x);
	int dpy = abs(object[i].position.y - plr[pnum].position.tile.y);
	if (dpx == 1 && dpy <= 1 && object[i]._otype == OBJ_L1LDOOR)
		OperateL1LDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && object[i]._otype == OBJ_L1RDOOR)
		OperateL1RDoor(pnum, i, sendflag);
}

void OperateLever(int pnum, int i)
{
	if (object[i]._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_LEVER, object[i].position);
	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame++;
	bool mapflag = true;
	if (currlevel == 16) {
		for (int j = 0; j < nobjects; j++) {
			int oi = objectactive[j];
			if (object[oi]._otype == OBJ_SWITCHSKL
			    && object[i]._oVar8 == object[oi]._oVar8
			    && object[oi]._oSelFlag != 0) {
				mapflag = false;
			}
		}
	}
	if (currlevel == 24) {
		OperateNakrulLever();
		IsUberLeverActivated = true;
		mapflag = false;
		quests[Q_NAKRUL]._qactive = QUEST_DONE;
	}
	if (mapflag)
		ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
	if (pnum == myplr)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateBook(int pnum, int i)
{
	int dx;
	int dy;

	if (object[i]._oSelFlag == 0)
		return;
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		bool doAddMissile = false;
		bool missileAdded = false;
		for (int j = 0; j < nobjects; j++) {
			int oi = objectactive[j];
			int otype = object[oi]._otype;
			if (otype == OBJ_MCIRCLE2 && object[oi]._oVar6 == 1) {
				dx = 27;
				dy = 29;
				object[oi]._oVar6 = 4;
				doAddMissile = true;
			}
			if (otype == OBJ_MCIRCLE2 && object[oi]._oVar6 == 2) {
				dx = 43;
				dy = 29;
				object[oi]._oVar6 = 4;
				doAddMissile = true;
			}
			if (doAddMissile) {
				object[dObject[35][36] - 1]._oVar5++;
				AddMissile(plr[pnum].position.tile, { dx, dy }, plr[pnum]._pdir, MIS_RNDTELEPORT, TARGET_MONSTERS, pnum, 0, 0);
				missileAdded = true;
				doAddMissile = false;
			}
		}
		if (!missileAdded)
			return;
	}
	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame++;
	if (!setlevel)
		return;

	if (setlvlnum == SL_BONECHAMB) {
		plr[pnum]._pMemSpells |= GetSpellBitmask(SPL_GUARDIAN);
		if (plr[pnum]._pSplLvl[SPL_GUARDIAN] < MAX_SPELL_LEVEL)
			plr[pnum]._pSplLvl[SPL_GUARDIAN]++;
		quests[Q_SCHAMB]._qactive = QUEST_DONE;
		if (!deltaload)
			PlaySfxLoc(IS_QUESTDN, object[i].position);
		InitDiabloMsg(EMSG_BONECHAMB);
		AddMissile(
		    plr[pnum].position.tile,
		    object[i].position + Displacement { -2, -4 },
		    plr[pnum]._pdir,
		    MIS_GUARDIAN,
		    TARGET_MONSTERS,
		    pnum,
		    0,
		    0);
	}
	if (setlvlnum == SL_VILEBETRAYER) {
		ObjChangeMapResync(
		    object[i]._oVar1,
		    object[i]._oVar2,
		    object[i]._oVar3,
		    object[i]._oVar4);
		for (int j = 0; j < nobjects; j++)
			SyncObjectAnim(objectactive[j]);
	}
}

void OperateBookLever(int pnum, int i)
{
	int x = 2 * setpc_x + 16;
	int y = 2 * setpc_y + 16;
	if (numitems >= MAXITEMS) {
		return;
	}
	if (object[i]._oSelFlag != 0 && !qtextflag) {
		if (object[i]._otype == OBJ_BLINDBOOK && quests[Q_BLIND]._qvar1 == 0) {
			quests[Q_BLIND]._qactive = QUEST_ACTIVE;
			quests[Q_BLIND]._qlog = true;
			quests[Q_BLIND]._qvar1 = 1;
		}
		if (object[i]._otype == OBJ_BLOODBOOK && quests[Q_BLOOD]._qvar1 == 0) {
			quests[Q_BLOOD]._qactive = QUEST_ACTIVE;
			quests[Q_BLOOD]._qlog = true;
			quests[Q_BLOOD]._qvar1 = 1;
			SpawnQuestItem(IDI_BLDSTONE, { 2 * setpc_x + 25, 2 * setpc_y + 33 }, 0, 1);
		}
		object[i]._otype = object[i]._otype;
		if (object[i]._otype == OBJ_STEELTOME && quests[Q_WARLORD]._qvar1 == 0) {
			quests[Q_WARLORD]._qactive = QUEST_ACTIVE;
			quests[Q_WARLORD]._qlog = true;
			quests[Q_WARLORD]._qvar1 = 1;
		}
		if (object[i]._oAnimFrame != object[i]._oVar6) {
			if (object[i]._otype != OBJ_BLOODBOOK)
				ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			if (object[i]._otype == OBJ_BLINDBOOK) {
				SpawnUnique(UITEM_OPTAMULET, Point { x, y } + Displacement { 5, 5 });
				int8_t tren = TransVal;
				TransVal = 9;
				DRLG_MRectTrans(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
				TransVal = tren;
			}
		}
		object[i]._oAnimFrame = object[i]._oVar6;
		InitQTextMsg(object[i]._oVar7);
		if (pnum == myplr)
			NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
	}
}

void OperateSChambBk(int i)
{
	if (object[i]._oSelFlag == 0 || qtextflag) {
		return;
	}

	if (object[i]._oAnimFrame != object[i]._oVar6) {
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		for (int j = 0; j < nobjects; j++)
			SyncObjectAnim(objectactive[j]);
	}
	object[i]._oAnimFrame = object[i]._oVar6;
	if (quests[Q_SCHAMB]._qactive == QUEST_INIT) {
		quests[Q_SCHAMB]._qactive = QUEST_ACTIVE;
		quests[Q_SCHAMB]._qlog = true;
	}

	_speech_id textdef;
	switch (plr[myplr]._pClass) {
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
	quests[Q_SCHAMB]._qmsg = textdef;
	InitQTextMsg(textdef);
}

void OperateChest(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_CHEST, object[i].position);
	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame += 2;
	if (deltaload) {
		return;
	}
	SetRndSeed(object[i]._oRndSeed);
	if (setlevel) {
		for (int j = 0; j < object[i]._oVar1; j++) {
			CreateRndItem(object[i].position, true, sendmsg, false);
		}
	} else {
		for (int j = 0; j < object[i]._oVar1; j++) {
			if (object[i]._oVar2 != 0)
				CreateRndItem(object[i].position, false, sendmsg, false);
			else
				CreateRndUseful(object[i].position, sendmsg);
		}
	}
	if (object[i]._oTrapFlag && object[i]._otype >= OBJ_TCHEST1 && object[i]._otype <= OBJ_TCHEST3) {
		Direction mdir = GetDirection(object[i].position, plr[pnum].position.tile);
		int mtype;
		switch (object[i]._oVar4) {
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
		AddMissile(object[i].position, plr[pnum].position.tile, mdir, mtype, TARGET_PLAYERS, -1, 0, 0);
		object[i]._oTrapFlag = false;
	}
	if (pnum == myplr)
		NetSendCmdParam2(false, CMD_PLROPOBJ, pnum, i);
}

void OperateMushPatch(int pnum, int i)
{
	if (numitems >= MAXITEMS) {
		return;
	}

	if (quests[Q_MUSHROOM]._qactive != QUEST_ACTIVE) {
		if (!deltaload && pnum == myplr) {
			plr[myplr].Say(HeroSpeech::ICantUseThisYet);
		}
		return;
	}

	if (object[i]._oSelFlag != 0) {
		if (!deltaload)
			PlaySfxLoc(IS_CHEST, object[i].position);
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame++;
		if (!deltaload) {
			Point pos = GetSuperItemLoc(object[i].position);
			SpawnQuestItem(IDI_MUSHROOM, pos, 0, 0);
			quests[Q_MUSHROOM]._qvar1 = QS_MUSHSPAWNED;
		}
	}
}

void OperateInnSignChest(int pnum, int i)
{
	if (numitems >= MAXITEMS) {
		return;
	}

	if (quests[Q_LTBANNER]._qvar1 != 2) {
		if (!deltaload && pnum == myplr) {
			plr[myplr].Say(HeroSpeech::ICantOpenThisYet);
		}
		return;
	}

	if (object[i]._oSelFlag == 0) {
		return;
	}
	if (!deltaload)
		PlaySfxLoc(IS_CHEST, object[i].position);
	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame += 2;
	if (!deltaload) {
		Point pos = GetSuperItemLoc(object[i].position);
		SpawnQuestItem(IDI_BANNER, pos, 0, 0);
	}
}

void OperateSlainHero(int pnum, int i)
{
	if (object[i]._oSelFlag == 0) {
		return;
	}
	object[i]._oSelFlag = 0;
	if (deltaload) {
		return;
	}
	if (plr[pnum]._pClass == HeroClass::Warrior) {
		CreateMagicArmor(object[i].position, ITYPE_HARMOR, ICURS_BREAST_PLATE, false, true);
	} else if (plr[pnum]._pClass == HeroClass::Rogue) {
		CreateMagicWeapon(object[i].position, ITYPE_BOW, ICURS_LONG_WAR_BOW, false, true);
	} else if (plr[pnum]._pClass == HeroClass::Sorcerer) {
		CreateSpellBook(object[i].position, SPL_LIGHTNING, false, true);
	} else if (plr[pnum]._pClass == HeroClass::Monk) {
		CreateMagicWeapon(object[i].position, ITYPE_STAFF, ICURS_WAR_STAFF, false, true);
	} else if (plr[pnum]._pClass == HeroClass::Bard) {
		CreateMagicWeapon(object[i].position, ITYPE_SWORD, ICURS_BASTARD_SWORD, false, true);
	} else if (plr[pnum]._pClass == HeroClass::Barbarian) {
		CreateMagicWeapon(object[i].position, ITYPE_AXE, ICURS_BATTLE_AXE, false, true);
	}
	plr[myplr].Say(HeroSpeech::RestInPeaceMyFriend);
	if (pnum == myplr)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateTrapLvr(int i)
{
	if (!deltaload)
		PlaySfxLoc(IS_LEVER, object[i].position);

	if (object[i]._oAnimFrame == 1) {
		object[i]._oAnimFrame = 2;
		for (int j = 0; j < nobjects; j++) {
			int oi = objectactive[j];
			if (object[oi]._otype == object[i]._oVar2 && object[oi]._oVar1 == object[i]._oVar1) {
				object[oi]._oVar2 = 1;
				object[oi]._oAnimFlag = 0;
			}
		}
		return;
	}

	object[i]._oAnimFrame--;
	for (int j = 0; j < nobjects; j++) {
		int oi = objectactive[j];
		if (object[oi]._otype == object[i]._oVar2 && object[oi]._oVar1 == object[i]._oVar1) {
			object[oi]._oVar2 = 0;
			if (object[oi]._oVar4 != 0)
				object[oi]._oAnimFlag = 1;
		}
	}
}

void OperateSarc(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_SARC, object[i].position);
	object[i]._oSelFlag = 0;
	if (deltaload) {
		object[i]._oAnimFrame = object[i]._oAnimLen;
		return;
	}
	object[i]._oAnimFlag = 1;
	object[i]._oAnimDelay = 3;
	SetRndSeed(object[i]._oRndSeed);
	if (object[i]._oVar1 <= 2)
		CreateRndItem(object[i].position, false, sendmsg, false);
	if (object[i]._oVar1 >= 8)
		SpawnSkeleton(object[i]._oVar2, object[i].position);
	if (pnum == myplr)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateL2Door(int pnum, int i, bool sendflag)
{
	int dpx = abs(object[i].position.x - plr[pnum].position.tile.x);
	int dpy = abs(object[i].position.y - plr[pnum].position.tile.y);
	if (dpx == 1 && dpy <= 1 && object[i]._otype == OBJ_L2LDOOR)
		OperateL2LDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && object[i]._otype == OBJ_L2RDOOR)
		OperateL2RDoor(pnum, i, sendflag);
}

void OperateL3Door(int pnum, int i, bool sendflag)
{
	int dpx = abs(object[i].position.x - plr[pnum].position.tile.x);
	int dpy = abs(object[i].position.y - plr[pnum].position.tile.y);
	if (dpx == 1 && dpy <= 1 && object[i]._otype == OBJ_L3RDOOR)
		OperateL3RDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && object[i]._otype == OBJ_L3LDOOR)
		OperateL3LDoor(pnum, i, sendflag);
}

void OperatePedistal(int pnum, int i)
{
	if (numitems >= MAXITEMS) {
		return;
	}

	if (object[i]._oVar6 == 3 || !plr[pnum].TryRemoveInvItemById(IDI_BLDSTONE)) {
		return;
	}

	object[i]._oAnimFrame++;
	object[i]._oVar6++;
	if (object[i]._oVar6 == 1) {
		if (!deltaload)
			PlaySfxLoc(LS_PUDDLE, object[i].position);
		ObjChangeMap(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
		SpawnQuestItem(IDI_BLDSTONE, { 2 * setpc_x + 19, 2 * setpc_y + 26 }, 0, 1);
	}
	if (object[i]._oVar6 == 2) {
		if (!deltaload)
			PlaySfxLoc(LS_PUDDLE, object[i].position);
		ObjChangeMap(setpc_x + 6, setpc_y + 3, setpc_x + setpc_w, setpc_y + 7);
		SpawnQuestItem(IDI_BLDSTONE, { 2 * setpc_x + 31, 2 * setpc_y + 26 }, 0, 1);
	}
	if (object[i]._oVar6 == 3) {
		if (!deltaload)
			PlaySfxLoc(LS_BLODSTAR, object[i].position);
		ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		LoadMapObjs("Levels\\L2Data\\Blood2.DUN", 2 * setpc_x, 2 * setpc_y);
		SpawnUnique(UITEM_ARMOFVAL, Point { setpc_x, setpc_y } * 2 + Displacement { 25, 19 });
		object[i]._oSelFlag = 0;
	}
}

void TryDisarm(int pnum, int i)
{
	if (pnum == myplr)
		NewCursor(CURSOR_HAND);
	if (!object[i]._oTrapFlag) {
		return;
	}
	int trapdisper = 2 * plr[pnum]._pDexterity - 5 * currlevel;
	if (GenerateRnd(100) > trapdisper) {
		return;
	}
	for (int j = 0; j < nobjects; j++) {
		bool checkflag = false;
		int oi = objectactive[j];
		int oti = object[oi]._otype;
		if (oti == OBJ_TRAPL)
			checkflag = true;
		if (oti == OBJ_TRAPR)
			checkflag = true;
		if (checkflag && dObject[object[oi]._oVar1][object[oi]._oVar2] - 1 == i) {
			object[oi]._oVar4 = 1;
			object[i]._oTrapFlag = false;
		}
	}
	int oti = object[i]._otype;
	if (oti >= OBJ_TCHEST1 && oti <= OBJ_TCHEST3)
		object[i]._oTrapFlag = false;
}

int ItemMiscIdIdx(item_misc_id imiscid)
{
	int i = IDI_GOLD;
	while (AllItemsList[i].iRnd == IDROP_NEVER || AllItemsList[i].iMiscId != imiscid) {
		i++;
	}

	return i;
}

bool OperateShrineMysterious(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
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

	CheckStats(plr[pnum]);

	InitDiabloMsg(EMSG_SHRINE_MYSTERIOUS);

	return true;
}

bool OperateShrineHidden(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	int cnt = 0;
	for (const auto &item : plr[pnum].InvBody) {
		if (!item.isEmpty())
			cnt++;
	}
	if (cnt > 0) {
		for (auto &item : plr[pnum].InvBody) {
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
			for (auto &item : plr[pnum].InvBody) {
				if (!item.isEmpty() && item._iMaxDur != DUR_INDESTRUCTIBLE && item._iMaxDur != 0) {
					cnt++;
				}
			}
			if (cnt == 0)
				break;
			int r = GenerateRnd(NUM_INVLOC);
			if (plr[pnum].InvBody[r].isEmpty() || plr[pnum].InvBody[r]._iMaxDur == DUR_INDESTRUCTIBLE || plr[pnum].InvBody[r]._iMaxDur == 0)
				continue;

			plr[pnum].InvBody[r]._iDurability -= 20;
			plr[pnum].InvBody[r]._iMaxDur -= 20;
			if (plr[pnum].InvBody[r]._iDurability <= 0)
				plr[pnum].InvBody[r]._iDurability = 1;
			if (plr[pnum].InvBody[r]._iMaxDur <= 0)
				plr[pnum].InvBody[r]._iMaxDur = 1;
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
	if (pnum != myplr)
		return true;

	if (!plr[pnum].InvBody[INVLOC_HEAD].isEmpty())
		plr[pnum].InvBody[INVLOC_HEAD]._iAC += 2;
	if (!plr[pnum].InvBody[INVLOC_CHEST].isEmpty())
		plr[pnum].InvBody[INVLOC_CHEST]._iAC += 2;
	if (!plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty()) {
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
			plr[pnum].InvBody[INVLOC_HAND_LEFT]._iAC += 2;
		} else {
			plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam--;
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam < plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMinDam)
				plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam = plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMinDam;
		}
	}
	if (!plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty()) {
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
			plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iAC += 2;
		} else {
			plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam--;
			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam < plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMinDam)
				plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam = plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMinDam;
		}
	}

	for (int j = 0; j < plr[pnum]._pNumInv; j++) {
		switch (plr[pnum].InvList[j]._itype) {
		case ITYPE_SWORD:
		case ITYPE_AXE:
		case ITYPE_BOW:
		case ITYPE_MACE:
		case ITYPE_STAFF:
			plr[pnum].InvList[j]._iMaxDam--;
			if (plr[pnum].InvList[j]._iMaxDam < plr[pnum].InvList[j]._iMinDam)
				plr[pnum].InvList[j]._iMaxDam = plr[pnum].InvList[j]._iMinDam;
			break;
		case ITYPE_SHIELD:
		case ITYPE_HELM:
		case ITYPE_LARMOR:
		case ITYPE_MARMOR:
		case ITYPE_HARMOR:
			plr[pnum].InvList[j]._iAC += 2;
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
	if (pnum != myplr)
		return true;

	if (!plr[pnum].InvBody[INVLOC_HAND_LEFT].isEmpty() && plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_SHIELD)
		plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam++;
	if (!plr[pnum].InvBody[INVLOC_HAND_RIGHT].isEmpty() && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_SHIELD)
		plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam++;

	for (int j = 0; j < plr[pnum]._pNumInv; j++) {
		switch (plr[pnum].InvList[j]._itype) {
		case ITYPE_SWORD:
		case ITYPE_AXE:
		case ITYPE_BOW:
		case ITYPE_MACE:
		case ITYPE_STAFF:
			plr[pnum].InvList[j]._iMaxDam++;
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

	AddMissile(
	    plr[pnum].position.tile,
	    plr[pnum].position.tile,
	    plr[pnum]._pdir,
	    MIS_MANASHIELD,
	    -1,
	    pnum,
	    0,
	    2 * leveltype);

	if (pnum != myplr)
		return false;

	InitDiabloMsg(EMSG_SHRINE_MAGICAL);

	return true;
}

bool OperateShrineStone(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return true;

	for (auto &item : plr[pnum].InvBody) {
		if (item._itype == ITYPE_STAFF)
			item._iCharges = item._iMaxCharges;
	}
	for (int j = 0; j < plr[pnum]._pNumInv; j++) {
		if (plr[pnum].InvList[j]._itype == ITYPE_STAFF)
			plr[pnum].InvList[j]._iCharges = plr[pnum].InvList[j]._iMaxCharges;
	}
	for (auto &item : plr[pnum].SpdList) {
		if (item._itype == ITYPE_STAFF)
			item._iCharges = item._iMaxCharges; // belt items don't have charges?
	}

	InitDiabloMsg(EMSG_SHRINE_STONE);

	return true;
}

bool OperateShrineReligious(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return true;

	for (auto &item : plr[pnum].InvBody)
		item._iDurability = item._iMaxDur;
	for (int j = 0; j < plr[pnum]._pNumInv; j++)
		plr[pnum].InvList[j]._iDurability = plr[pnum].InvList[j]._iMaxDur;
	for (auto &item : plr[pnum].SpdList)
		item._iDurability = item._iMaxDur; // belt items don't have durability?

	InitDiabloMsg(EMSG_SHRINE_RELIGIOUS);

	return true;
}

bool OperateShrineEnchanted(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	int cnt = 0;
	uint64_t spell = 1;
	int maxSpells = gbIsHellfire ? MAX_SPELLS : 37;
	uint64_t spells = plr[pnum]._pMemSpells;
	for (int j = 0; j < maxSpells; j++) {
		if ((spell & spells) != 0)
			cnt++;
		spell *= 2;
	}
	if (cnt > 1) {
		spell = 1;
		for (int j = SPL_FIREBOLT; j < maxSpells; j++) { // BUGFIX: < MAX_SPELLS, there is no spell with MAX_SPELLS index (fixed)
			if ((plr[pnum]._pMemSpells & spell) != 0) {
				if (plr[pnum]._pSplLvl[j] < MAX_SPELL_LEVEL)
					plr[pnum]._pSplLvl[j]++;
			}
			spell *= 2;
		}
		int r;
		do {
			r = GenerateRnd(maxSpells);
		} while ((plr[pnum]._pMemSpells & GetSpellBitmask(r + 1)) == 0);
		if (plr[pnum]._pSplLvl[r + 1] >= 2)
			plr[pnum]._pSplLvl[r + 1] -= 2;
		else
			plr[pnum]._pSplLvl[r + 1] = 0;
	}

	InitDiabloMsg(EMSG_SHRINE_ENCHANTED);

	return true;
}

bool OperateShrineThaumaturgic(int pnum)
{
	for (int j = 0; j < nobjects; j++) {
		int v1 = objectactive[j];
		assert((DWORD)v1 < MAXOBJECTS);
		if (IsAnyOf(object[v1]._otype, OBJ_CHEST1, OBJ_CHEST2, OBJ_CHEST3, OBJ_TCHEST1, OBJ_TCHEST2, OBJ_TCHEST3) && object[v1]._oSelFlag == 0) {
			object[v1]._oRndSeed = AdvanceRndSeed();
			object[v1]._oSelFlag = 1;
			object[v1]._oAnimFrame -= 2;
		}
	}

	if (deltaload)
		return false;

	if (pnum != myplr)
		return true;

	InitDiabloMsg(EMSG_SHRINE_THAUMATURGIC);

	return true;
}

bool OperateShrineFascinating(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	plr[pnum]._pMemSpells |= GetSpellBitmask(SPL_FIREBOLT);

	if (plr[pnum]._pSplLvl[SPL_FIREBOLT] < MAX_SPELL_LEVEL)
		plr[pnum]._pSplLvl[SPL_FIREBOLT]++;
	if (plr[pnum]._pSplLvl[SPL_FIREBOLT] < MAX_SPELL_LEVEL)
		plr[pnum]._pSplLvl[SPL_FIREBOLT]++;

	DWORD t = plr[pnum]._pMaxManaBase / 10;
	int v1 = plr[pnum]._pMana - plr[pnum]._pManaBase;
	int v2 = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
	plr[pnum]._pManaBase -= t;
	plr[pnum]._pMana -= t;
	plr[pnum]._pMaxMana -= t;
	plr[pnum]._pMaxManaBase -= t;
	if (plr[pnum]._pMana >> 6 <= 0) {
		plr[pnum]._pMana = v1;
		plr[pnum]._pManaBase = 0;
	}
	if (plr[pnum]._pMaxMana >> 6 <= 0) {
		plr[pnum]._pMaxMana = v2;
		plr[pnum]._pMaxManaBase = 0;
	}

	InitDiabloMsg(EMSG_SHRINE_FASCINATING);

	return true;
}

bool OperateShrineCryptic(int pnum)
{
	if (deltaload)
		return false;

	AddMissile(
	    plr[pnum].position.tile,
	    plr[pnum].position.tile,
	    plr[pnum]._pdir,
	    MIS_NOVA,
	    -1,
	    pnum,
	    0,
	    2 * leveltype);

	if (pnum != myplr)
		return false;

	plr[pnum]._pMana = plr[pnum]._pMaxMana;
	plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;

	InitDiabloMsg(EMSG_SHRINE_CRYPTIC);

	return true;
}

bool OperateShrineEldritch(int pnum)
{
	/// BUGFIX: change `plr[pnum].HoldItem` to use a temporary buffer to prevent deleting item in hand
	if (deltaload)
		return false;
	if (pnum != myplr)
		return true;

	for (int j = 0; j < plr[pnum]._pNumInv; j++) {
		if (plr[pnum].InvList[j]._itype == ITYPE_MISC) {
			if (plr[pnum].InvList[j]._iMiscId == IMISC_HEAL
			    || plr[pnum].InvList[j]._iMiscId == IMISC_MANA) {
				SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_REJUV));
				GetPlrHandSeed(&plr[pnum].HoldItem);
				plr[pnum].HoldItem._iStatFlag = true;
				plr[pnum].InvList[j] = plr[pnum].HoldItem;
			}
			if (plr[pnum].InvList[j]._iMiscId == IMISC_FULLHEAL
			    || plr[pnum].InvList[j]._iMiscId == IMISC_FULLMANA) {
				SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_FULLREJUV));
				GetPlrHandSeed(&plr[pnum].HoldItem);
				plr[pnum].HoldItem._iStatFlag = true;
				plr[pnum].InvList[j] = plr[pnum].HoldItem;
			}
		}
	}
	for (auto &item : plr[pnum].SpdList) {
		if (item._itype == ITYPE_MISC) {
			if (item._iMiscId == IMISC_HEAL
			    || item._iMiscId == IMISC_MANA) {
				SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_REJUV));
				GetPlrHandSeed(&plr[pnum].HoldItem);
				plr[pnum].HoldItem._iStatFlag = true;
				item = plr[pnum].HoldItem;
			}
			if (item._iMiscId == IMISC_FULLHEAL
			    || item._iMiscId == IMISC_FULLMANA) {
				SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_FULLREJUV));
				GetPlrHandSeed(&plr[pnum].HoldItem);
				plr[pnum].HoldItem._iStatFlag = true;
				item = plr[pnum].HoldItem;
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
	if (pnum != myplr)
		return false;

	ModifyPlrMag(pnum, 2);
	CheckStats(plr[pnum]);

	InitDiabloMsg(EMSG_SHRINE_EERIE);

	return true;
}

bool OperateShrineDivine(int pnum, int x, int y)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	if (currlevel < 4) {
		CreateTypeItem({ x, y }, false, ITYPE_MISC, IMISC_FULLMANA, false, true);
		CreateTypeItem({ x, y }, false, ITYPE_MISC, IMISC_FULLHEAL, false, true);
	} else {
		CreateTypeItem({ x, y }, false, ITYPE_MISC, IMISC_FULLREJUV, false, true);
		CreateTypeItem({ x, y }, false, ITYPE_MISC, IMISC_FULLREJUV, false, true);
	}

	plr[pnum]._pMana = plr[pnum]._pMaxMana;
	plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
	plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
	plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;

	InitDiabloMsg(EMSG_SHRINE_DIVINE);

	return true;
}

bool OperateShrineHoly(int pnum)
{
	if (deltaload)
		return false;

	int j = 0;
	int xx;
	int yy;
	uint32_t lv;
	do {
		xx = GenerateRnd(MAXDUNX);
		yy = GenerateRnd(MAXDUNY);
		lv = dPiece[xx][yy];
		j++;
		if (j > MAXDUNX * MAXDUNY)
			break;
	} while (nSolidTable[lv] || dObject[xx][yy] != 0 || dMonster[xx][yy] != 0);

	AddMissile(plr[pnum].position.tile, { xx, yy }, plr[pnum]._pdir, MIS_RNDTELEPORT, -1, pnum, 0, 2 * leveltype);

	if (pnum != myplr)
		return false;

	InitDiabloMsg(EMSG_SHRINE_HOLY);

	return true;
}

bool OperateShrineSacred(int pnum)
{
	if (deltaload || pnum != myplr)
		return false;

	plr[pnum]._pMemSpells |= GetSpellBitmask(SPL_CBOLT);

	if (plr[pnum]._pSplLvl[SPL_CBOLT] < MAX_SPELL_LEVEL)
		plr[pnum]._pSplLvl[SPL_CBOLT]++;
	if (plr[pnum]._pSplLvl[SPL_CBOLT] < MAX_SPELL_LEVEL)
		plr[pnum]._pSplLvl[SPL_CBOLT]++;

	uint32_t t = plr[pnum]._pMaxManaBase / 10;
	int v1 = plr[pnum]._pMana - plr[pnum]._pManaBase;
	int v2 = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
	plr[pnum]._pManaBase -= t;
	plr[pnum]._pMana -= t;
	plr[pnum]._pMaxMana -= t;
	plr[pnum]._pMaxManaBase -= t;
	if (plr[pnum]._pMana >> 6 <= 0) {
		plr[pnum]._pMana = v1;
		plr[pnum]._pManaBase = 0;
	}
	if (plr[pnum]._pMaxMana >> 6 <= 0) {
		plr[pnum]._pMaxMana = v2;
		plr[pnum]._pMaxManaBase = 0;
	}

	InitDiabloMsg(EMSG_SHRINE_SACRED);

	return true;
}

bool OperateShrineSpiritual(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	for (int8_t &gridItem : plr[pnum].InvGrid) {
		if (gridItem == 0) {
			int r = 5 * leveltype + GenerateRnd(10 * leveltype);
			DWORD t = plr[pnum]._pNumInv; // check
			plr[pnum].InvList[t] = golditem;
			plr[pnum].InvList[t]._iSeed = AdvanceRndSeed();
			plr[pnum]._pNumInv++;
			gridItem = plr[pnum]._pNumInv;
			plr[pnum].InvList[t]._ivalue = r;
			plr[pnum]._pGold += r;
			SetPlrHandGoldCurs(&plr[pnum].InvList[t]);
		}
	}

	InitDiabloMsg(EMSG_SHRINE_SPIRITUAL);

	return true;
}

bool OperateShrineSpooky(int pnum)
{
	if (deltaload)
		return false;

	if (pnum == myplr) {
		InitDiabloMsg(EMSG_SHRINE_SPOOKY1);
		return true;
	}

	plr[myplr]._pHitPoints = plr[myplr]._pMaxHP;
	plr[myplr]._pHPBase = plr[myplr]._pMaxHPBase;
	plr[myplr]._pMana = plr[myplr]._pMaxMana;
	plr[myplr]._pManaBase = plr[myplr]._pMaxManaBase;

	InitDiabloMsg(EMSG_SHRINE_SPOOKY2);

	return true;
}

bool OperateShrineAbandoned(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	ModifyPlrDex(pnum, 2);
	CheckStats(plr[pnum]);

	if (pnum != myplr)
		return true;

	InitDiabloMsg(EMSG_SHRINE_ABANDONED);

	return true;
}

bool OperateShrineCreepy(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	ModifyPlrStr(pnum, 2);
	CheckStats(plr[pnum]);

	if (pnum != myplr)
		return true;

	InitDiabloMsg(EMSG_SHRINE_CREEPY);

	return true;
}

bool OperateShrineQuiet(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	ModifyPlrVit(pnum, 2);
	CheckStats(plr[pnum]);

	if (pnum != myplr)
		return true;

	InitDiabloMsg(EMSG_SHRINE_QUIET);

	return true;
}

bool OperateShrineSecluded(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return true;

	std::fill(&AutomapView[0][0], &AutomapView[DMAXX - 1][DMAXX - 1], true);

	InitDiabloMsg(EMSG_SHRINE_SECLUDED);

	return true;
}

bool OperateShrineOrnate(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	plr[pnum]._pMemSpells |= GetSpellBitmask(SPL_HBOLT);
	if (plr[pnum]._pSplLvl[SPL_HBOLT] < MAX_SPELL_LEVEL)
		plr[pnum]._pSplLvl[SPL_HBOLT]++;
	if (plr[pnum]._pSplLvl[SPL_HBOLT] < MAX_SPELL_LEVEL)
		plr[pnum]._pSplLvl[SPL_HBOLT]++;

	uint32_t t = plr[pnum]._pMaxManaBase / 10;
	int v1 = plr[pnum]._pMana - plr[pnum]._pManaBase;
	int v2 = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
	plr[pnum]._pManaBase -= t;
	plr[pnum]._pMana -= t;
	plr[pnum]._pMaxMana -= t;
	plr[pnum]._pMaxManaBase -= t;
	if (plr[pnum]._pMana >> 6 <= 0) {
		plr[pnum]._pMana = v1;
		plr[pnum]._pManaBase = 0;
	}
	if (plr[pnum]._pMaxMana >> 6 <= 0) {
		plr[pnum]._pMaxMana = v2;
		plr[pnum]._pMaxManaBase = 0;
	}

	InitDiabloMsg(EMSG_SHRINE_ORNATE);

	return true;
}

bool OperateShrineGlimmering(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	for (auto &item : plr[pnum].InvBody) {
		if (item._iMagical != ITEM_QUALITY_NORMAL && !item._iIdentified)
			item._iIdentified = true;
	}
	for (int j = 0; j < plr[pnum]._pNumInv; j++) {
		if (plr[pnum].InvList[j]._iMagical != ITEM_QUALITY_NORMAL && !plr[pnum].InvList[j]._iIdentified)
			plr[pnum].InvList[j]._iIdentified = true;
	}
	for (auto &item : plr[pnum].SpdList) {
		if (item._iMagical != ITEM_QUALITY_NORMAL && !item._iIdentified)
			item._iIdentified = true; // belt items can't be magical?
	}

	InitDiabloMsg(EMSG_SHRINE_GLIMMERING);

	return true;
}

bool OperateShrineTainted(int pnum)
{
	if (deltaload)
		return false;

	if (pnum == myplr) {
		InitDiabloMsg(EMSG_SHRINE_TAINTED1);
		return true;
	}

	int r = GenerateRnd(4);

	int v1 = r == 0 ? 1 : -1;
	int v2 = r == 1 ? 1 : -1;
	int v3 = r == 2 ? 1 : -1;
	int v4 = r == 3 ? 1 : -1;

	ModifyPlrStr(myplr, v1);
	ModifyPlrMag(myplr, v2);
	ModifyPlrDex(myplr, v3);
	ModifyPlrVit(myplr, v4);

	CheckStats(plr[myplr]);

	InitDiabloMsg(EMSG_SHRINE_TAINTED2);

	return true;
}

bool OperateShrineOily(int pnum, int x, int y)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	switch (plr[myplr]._pClass) {
	case HeroClass::Warrior:
		ModifyPlrStr(myplr, 2);
		break;
	case HeroClass::Rogue:
		ModifyPlrDex(myplr, 2);
		break;
	case HeroClass::Sorcerer:
		ModifyPlrMag(myplr, 2);
		break;
	case HeroClass::Barbarian:
		ModifyPlrVit(myplr, 2);
		break;
	case HeroClass::Monk:
		ModifyPlrStr(myplr, 1);
		ModifyPlrDex(myplr, 1);
		break;
	case HeroClass::Bard:
		ModifyPlrDex(myplr, 1);
		ModifyPlrMag(myplr, 1);
		break;
	}

	CheckStats(plr[pnum]);

	AddMissile(
	    { x, y },
	    plr[myplr].position.tile,
	    plr[myplr]._pdir,
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
	if (pnum != myplr)
		return false;

	int playerXP = plr[myplr]._pExperience;
	int magicGain = playerXP / 1000;
	int xpLoss = 0;
	if (playerXP > 5000) {
		magicGain = 5;
		xpLoss = static_cast<int>(playerXP * 0.95);
	}
	ModifyPlrMag(myplr, magicGain);
	plr[myplr]._pExperience = xpLoss;

	if (sgOptions.Gameplay.bExperienceBar) {
		force_redraw = 255;
	}

	CheckStats(plr[pnum]);

	InitDiabloMsg(EMSG_SHRINE_GLOWING);

	return true;
}

bool OperateShrineMendicant(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	int gold = plr[myplr]._pGold / 2;
	AddPlrExperience(myplr, plr[myplr]._pLevel, gold);
	TakePlrsMoney(gold);

	CheckStats(plr[pnum]);

	InitDiabloMsg(EMSG_SHRINE_MENDICANT);

	return true;
}

bool OperateShrineSparkling(int pnum, int x, int y)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	AddPlrExperience(myplr, plr[myplr]._pLevel, 1000 * currlevel);

	AddMissile(
	    { x, y },
	    plr[myplr].position.tile,
	    plr[myplr]._pdir,
	    MIS_FLASH,
	    TARGET_PLAYERS,
	    -1,
	    3 * currlevel + 2,
	    0);

	CheckStats(plr[pnum]);

	InitDiabloMsg(EMSG_SHRINE_SPARKLING);

	return true;
}

bool OperateShrineTown(int pnum, int x, int y)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	AddMissile(
	    { x, y },
	    plr[myplr].position.tile,
	    plr[myplr]._pdir,
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
	if (pnum != myplr)
		return false;

	plr[pnum]._pMana = plr[pnum]._pMaxMana;
	plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;

	InitDiabloMsg(EMSG_SHRINE_SHIMMERING);

	return true;
}

bool OperateShrineSolar(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	time_t tm = time(nullptr);
	int hour = localtime(&tm)->tm_hour;
	if (hour >= 20 || hour < 4) {
		InitDiabloMsg(EMSG_SHRINE_SOLAR4);
		ModifyPlrVit(myplr, 2);
	} else if (hour >= 18) {
		InitDiabloMsg(EMSG_SHRINE_SOLAR3);
		ModifyPlrMag(myplr, 2);
	} else if (hour >= 12) {
		InitDiabloMsg(EMSG_SHRINE_SOLAR2);
		ModifyPlrStr(myplr, 2);
	} else /* 4:00 to 11:59 */ {
		InitDiabloMsg(EMSG_SHRINE_SOLAR1);
		ModifyPlrDex(myplr, 2);
	}

	CheckStats(plr[pnum]);

	return true;
}

bool OperateShrineMurphys(int pnum)
{
	if (deltaload)
		return false;
	if (pnum != myplr)
		return false;

	bool broke = false;
	for (auto &item : plr[myplr].InvBody) {
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
		TakePlrsMoney(plr[myplr]._pGold / 3);
	}

	InitDiabloMsg(EMSG_SHRINE_MURPHYS);

	return true;
}

void OperateShrine(int pnum, int i, _sfx_id sType)
{
	if (dropGoldFlag) {
		dropGoldFlag = false;
		dropGoldValue = 0;
	}

	assert((DWORD)i < MAXOBJECTS);

	if (object[i]._oSelFlag == 0)
		return;

	SetRndSeed(object[i]._oRndSeed);
	object[i]._oSelFlag = 0;

	if (!deltaload) {
		PlaySfxLoc(sType, object[i].position);
		object[i]._oAnimFlag = 1;
		object[i]._oAnimDelay = 1;
	} else {
		object[i]._oAnimFrame = object[i]._oAnimLen;
		object[i]._oAnimFlag = 0;
	}

	switch (object[i]._oVar1) {
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
		if (!OperateShrineDivine(pnum, object[i].position.x, object[i].position.y))
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
		if (!OperateShrineOily(pnum, object[i].position.x, object[i].position.y))
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
		if (!OperateShrineSparkling(pnum, object[i].position.x, object[i].position.y))
			return;
		break;
	case ShrineTown:
		if (!OperateShrineTown(pnum, object[i].position.x, object[i].position.y))
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

	CalcPlrInv(pnum, true);
	force_redraw = 255;

	if (pnum == myplr)
		NetSendCmdParam2(false, CMD_PLROPOBJ, pnum, i);
}

void OperateSkelBook(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_ISCROL, object[i].position);
	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame += 2;
	if (deltaload) {
		return;
	}
	SetRndSeed(object[i]._oRndSeed);
	if (GenerateRnd(5) != 0)
		CreateTypeItem(object[i].position, false, ITYPE_MISC, IMISC_SCROLL, sendmsg, false);
	else
		CreateTypeItem(object[i].position, false, ITYPE_MISC, IMISC_BOOK, sendmsg, false);
	if (pnum == myplr)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateBookCase(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag == 0) {
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_ISCROL, object[i].position);
	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame -= 2;
	if (deltaload) {
		return;
	}
	SetRndSeed(object[i]._oRndSeed);
	CreateTypeItem(object[i].position, false, ITYPE_MISC, IMISC_BOOK, sendmsg, false);
	if (QuestStatus(Q_ZHAR)
	    && monster[MAX_PLRS]._mmode == MM_STAND // prevents playing the "angry" message for the second time if zhar got aggroed by losing vision and talking again
	    && monster[MAX_PLRS]._uniqtype - 1 == UMT_ZHAR
	    && monster[MAX_PLRS]._msquelch == UINT8_MAX
	    && monster[MAX_PLRS]._mhitpoints > 0) {
		monster[MAX_PLRS].mtalkmsg = TEXT_ZHAR2;
		M_StartStand(0, monster[MAX_PLRS]._mdir); // BUGFIX: first parameter in call to M_StartStand should be MAX_PLRS, not 0.
		monster[MAX_PLRS]._mgoal = MGOAL_ATTACK2;
		monster[MAX_PLRS]._mmode = MM_TALK;
	}
	if (pnum == myplr)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateDecap(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag == 0) {
		return;
	}
	object[i]._oSelFlag = 0;
	if (deltaload) {
		return;
	}
	SetRndSeed(object[i]._oRndSeed);
	CreateRndItem(object[i].position, false, sendmsg, false);
	if (pnum == myplr)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateArmorStand(int pnum, int i, bool sendmsg)
{
	if (object[i]._oSelFlag == 0) {
		return;
	}
	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame++;
	if (deltaload) {
		return;
	}
	SetRndSeed(object[i]._oRndSeed);
	bool uniqueRnd = (GenerateRnd(2) != 0);
	if (currlevel <= 5) {
		CreateTypeItem(object[i].position, true, ITYPE_LARMOR, IMISC_NONE, sendmsg, false);
	} else if (currlevel >= 6 && currlevel <= 9) {
		CreateTypeItem(object[i].position, uniqueRnd, ITYPE_MARMOR, IMISC_NONE, sendmsg, false);
	} else if (currlevel >= 10 && currlevel <= 12) {
		CreateTypeItem(object[i].position, false, ITYPE_HARMOR, IMISC_NONE, sendmsg, false);
	} else if (currlevel >= 13 && currlevel <= 16) {
		CreateTypeItem(object[i].position, true, ITYPE_HARMOR, IMISC_NONE, sendmsg, false);
	} else if (currlevel >= 17) {
		CreateTypeItem(object[i].position, true, ITYPE_HARMOR, IMISC_NONE, sendmsg, false);
	}
	if (pnum == myplr)
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
	SetRndSeed(object[i]._oRndSeed);
	object[i]._oVar1 = FindValidShrine();
	OperateShrine(pnum, i, sType);
	object[i]._oAnimDelay = 2;
	force_redraw = 255;
}

void OperateCauldron(int pnum, int i, _sfx_id sType)
{
	SetRndSeed(object[i]._oRndSeed);
	object[i]._oVar1 = FindValidShrine();
	OperateShrine(pnum, i, sType);
	object[i]._oAnimFrame = 3;
	object[i]._oAnimFlag = 0;
	force_redraw = 255;
}

bool OperateFountains(int pnum, int i)
{
	bool applied = false;
	SetRndSeed(object[i]._oRndSeed);
	switch (object[i]._otype) {
	case OBJ_BLOODFTN:
		if (deltaload)
			return false;
		if (pnum != myplr)
			return false;

		if (plr[pnum]._pHitPoints < plr[pnum]._pMaxHP) {
			PlaySfxLoc(LS_FOUNTAIN, object[i].position);
			plr[pnum]._pHitPoints += 64;
			plr[pnum]._pHPBase += 64;
			if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
				plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
				plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			}
			applied = true;
		} else
			PlaySfxLoc(LS_FOUNTAIN, object[i].position);
		break;
	case OBJ_PURIFYINGFTN:
		if (deltaload)
			return false;
		if (pnum != myplr)
			return false;

		if (plr[pnum]._pMana < plr[pnum]._pMaxMana) {
			PlaySfxLoc(LS_FOUNTAIN, object[i].position);

			plr[pnum]._pMana += 64;
			plr[pnum]._pManaBase += 64;
			if (plr[pnum]._pMana > plr[pnum]._pMaxMana) {
				plr[pnum]._pMana = plr[pnum]._pMaxMana;
				plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
			}

			applied = true;
		} else
			PlaySfxLoc(LS_FOUNTAIN, object[i].position);
		break;
	case OBJ_MURKYFTN:
		if (object[i]._oSelFlag == 0)
			break;
		if (!deltaload)
			PlaySfxLoc(LS_FOUNTAIN, object[i].position);
		object[i]._oSelFlag = 0;
		if (deltaload)
			return false;
		AddMissile(
		    plr[pnum].position.tile,
		    plr[pnum].position.tile,
		    plr[pnum]._pdir,
		    MIS_INFRA,
		    -1,
		    pnum,
		    0,
		    2 * leveltype);
		applied = true;
		if (pnum == myplr)
			NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
		break;
	case OBJ_TEARFTN: {
		if (object[i]._oSelFlag == 0)
			break;
		if (!deltaload)
			PlaySfxLoc(LS_FOUNTAIN, object[i].position);
		object[i]._oSelFlag = 0;
		if (deltaload)
			return false;
		if (pnum != myplr)
			return false;
		int prev = -1;
		int add = -1;
		int cnt = 0;
		while (true) {
			int rnd = GenerateRnd(4);
			if (rnd != prev) {
				switch (rnd) {
				case 0:
					ModifyPlrStr(pnum, add);
					break;
				case 1:
					ModifyPlrMag(pnum, add);
					break;
				case 2:
					ModifyPlrDex(pnum, add);
					break;
				case 3:
					ModifyPlrVit(pnum, add);
					break;
				}
				prev = rnd;
				add = 1;
				cnt++;
			}
			if (cnt > 1)
				break;
		}
		CheckStats(plr[pnum]);
		applied = true;
		if (pnum == myplr)
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
	int weaponType;

	if (object[i]._oSelFlag == 0)
		return;
	SetRndSeed(object[i]._oRndSeed);

	switch (GenerateRnd(4) + ITYPE_SWORD) {
	case ITYPE_SWORD:
		weaponType = ITYPE_SWORD;
		break;
	case ITYPE_AXE:
		weaponType = ITYPE_AXE;
		break;
	case ITYPE_BOW:
		weaponType = ITYPE_BOW;
		break;
	case ITYPE_MACE:
		weaponType = ITYPE_MACE;
		break;
	}

	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame++;
	if (deltaload)
		return;

	CreateTypeItem(object[i].position, leveltype > 1, weaponType, IMISC_NONE, sendmsg, false);

	if (pnum == myplr)
		NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateStoryBook(int pnum, int i)
{
	if (object[i]._oSelFlag == 0 || deltaload || qtextflag || pnum != myplr) {
		return;
	}
	object[i]._oAnimFrame = object[i]._oVar4;
	PlaySfxLoc(IS_ISCROL, object[i].position);
	auto msg = static_cast<_speech_id>(object[i]._oVar2);
	if (object[i]._oVar8 != 0 && currlevel == 24) {
		if (!IsUberLeverActivated && quests[Q_NAKRUL]._qactive != QUEST_DONE && OperateNakrulBook(object[i]._oVar8)) {
			NetSendCmd(false, CMD_NAKRUL);
			return;
		}
	} else if (currlevel >= 21) {
		quests[Q_NAKRUL]._qactive = QUEST_ACTIVE;
		quests[Q_NAKRUL]._qlog = true;
		quests[Q_NAKRUL]._qmsg = msg;
	}
	InitQTextMsg(msg);
	NetSendCmdParam1(false, CMD_OPERATEOBJ, i);
}

void OperateLazStand(int pnum, int i)
{
	if (numitems >= MAXITEMS) {
		return;
	}

	if (object[i]._oSelFlag == 0 || deltaload || qtextflag || pnum != myplr) {
		return;
	}

	object[i]._oAnimFrame++;
	object[i]._oSelFlag = 0;
	Point pos = GetSuperItemLoc(object[i].position);
	SpawnQuestItem(IDI_LAZSTAFF, pos, 0, 0);
}

bool objectIsDisabled(int i)
{
	if (!sgOptions.Gameplay.bDisableCripplingShrines)
		return false;
	if ((object[i]._otype == OBJ_GOATSHRINE) || (object[i]._otype == OBJ_CAULDRON))
		return true;
	if ((object[i]._otype != OBJ_SHRINEL) && (object[i]._otype != OBJ_SHRINER))
		return false;
	if ((object[i]._oVar1 == ShrineFascinating)
	    || (object[i]._oVar1 == ShrineOrnate)
	    || (object[i]._oVar1 == ShrineSacred))
		return true;
	return false;
}

void OperateObject(int pnum, int i, bool teleFlag)
{
	bool sendmsg = pnum == myplr;
	switch (object[i]._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		if (teleFlag) {
			if (object[i]._otype == OBJ_L1LDOOR)
				OperateL1LDoor(pnum, i, true);
			if (object[i]._otype == OBJ_L1RDOOR)
				OperateL1RDoor(pnum, i, true);
			break;
		}
		if (pnum == myplr)
			OperateL1Door(pnum, i, true);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		if (teleFlag) {
			if (object[i]._otype == OBJ_L2LDOOR)
				OperateL2LDoor(pnum, i, true);
			if (object[i]._otype == OBJ_L2RDOOR)
				OperateL2RDoor(pnum, i, true);
			break;
		}
		if (pnum == myplr)
			OperateL2Door(pnum, i, true);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (teleFlag) {
			if (object[i]._otype == OBJ_L3LDOOR)
				OperateL3LDoor(pnum, i, true);
			if (object[i]._otype == OBJ_L3RDOOR)
				OperateL3RDoor(pnum, i, true);
			break;
		}
		if (pnum == myplr)
			OperateL3Door(pnum, i, true);
		break;
	case OBJ_LEVER:
	case OBJ_SWITCHSKL:
		OperateLever(pnum, i);
		break;
	case OBJ_BOOK2L:
		OperateBook(pnum, i);
		break;
	case OBJ_BOOK2R:
		OperateSChambBk(i);
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
		OperateTrapLvr(i);
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
		OperateMushPatch(pnum, i);
		break;
	case OBJ_LAZSTAND:
		OperateLazStand(pnum, i);
		break;
	case OBJ_SLAINHERO:
		OperateSlainHero(pnum, i);
		break;
	case OBJ_SIGNCHEST:
		OperateInnSignChest(pnum, i);
		break;
	default:
		break;
	}
}

void SyncOpL1Door(int pnum, int cmd, int i)
{
	if (pnum == myplr)
		return;

	bool doSync = false;
	if (cmd == CMD_OPENDOOR && object[i]._oVar4 == 0)
		doSync = true;
	if (cmd == CMD_CLOSEDOOR && object[i]._oVar4 == 1)
		doSync = true;
	if (!doSync)
		return;

	if (object[i]._otype == OBJ_L1LDOOR)
		OperateL1LDoor(-1, i, false);
	if (object[i]._otype == OBJ_L1RDOOR)
		OperateL1RDoor(-1, i, false);
}

void SyncOpL2Door(int pnum, int cmd, int i)
{
	if (pnum == myplr)
		return;

	bool doSync = false;
	if (cmd == CMD_OPENDOOR && object[i]._oVar4 == 0)
		doSync = true;
	if (cmd == CMD_CLOSEDOOR && object[i]._oVar4 == 1)
		doSync = true;
	if (!doSync)
		return;

	if (object[i]._otype == OBJ_L2LDOOR)
		OperateL2LDoor(-1, i, false);
	if (object[i]._otype == OBJ_L2RDOOR)
		OperateL2RDoor(-1, i, false);
}

void SyncOpL3Door(int pnum, int cmd, int i)
{
	if (pnum == myplr)
		return;

	bool doSync = false;
	if (cmd == CMD_OPENDOOR && object[i]._oVar4 == 0)
		doSync = true;
	if (cmd == CMD_CLOSEDOOR && object[i]._oVar4 == 1)
		doSync = true;
	if (!doSync)
		return;

	if (object[i]._otype == OBJ_L3LDOOR)
		OperateL3LDoor(-1, i, false);
	if (object[i]._otype == OBJ_L3RDOOR)
		OperateL3RDoor(-1, i, false);
}

void SyncOpObject(int pnum, int cmd, int i)
{
	switch (object[i]._otype) {
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
		OperateMushPatch(pnum, i);
		break;
	case OBJ_SLAINHERO:
		OperateSlainHero(pnum, i);
		break;
	case OBJ_SIGNCHEST:
		OperateInnSignChest(pnum, i);
		break;
	default:
		break;
	}
}

void BreakCrux(int i)
{
	object[i]._oAnimFlag = 1;
	object[i]._oAnimFrame = 1;
	object[i]._oAnimDelay = 1;
	object[i]._oSolidFlag = true;
	object[i]._oMissFlag = true;
	object[i]._oBreak = -1;
	object[i]._oSelFlag = 0;
	bool triggered = true;
	for (int j = 0; j < nobjects; j++) {
		int oi = objectactive[j];
		if (object[oi]._otype != OBJ_CRUX1 && object[oi]._otype != OBJ_CRUX2 && object[oi]._otype != OBJ_CRUX3)
			continue;
		if (object[i]._oVar8 != object[oi]._oVar8 || object[oi]._oBreak == -1)
			continue;
		triggered = false;
	}
	if (!triggered)
		return;
	if (!deltaload)
		PlaySfxLoc(IS_LEVER, object[i].position);
	ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
}

void BreakBarrel(int pnum, int i, int dam, bool forcebreak, bool sendmsg)
{
	if (object[i]._oSelFlag == 0)
		return;
	if (forcebreak) {
		object[i]._oVar1 = 0;
	} else {
		object[i]._oVar1 -= dam;
		if (pnum != myplr && object[i]._oVar1 <= 0)
			object[i]._oVar1 = 1;
	}
	if (object[i]._oVar1 > 0) {
		if (deltaload)
			return;

		PlaySfxLoc(IS_IBOW, object[i].position);
		return;
	}

	object[i]._oVar1 = 0;
	object[i]._oAnimFlag = 1;
	object[i]._oAnimFrame = 1;
	object[i]._oAnimDelay = 1;
	object[i]._oSolidFlag = false;
	object[i]._oMissFlag = true;
	object[i]._oBreak = -1;
	object[i]._oSelFlag = 0;
	object[i]._oPreFlag = true;
	if (deltaload) {
		object[i]._oAnimFrame = object[i]._oAnimLen;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimDelay = 1000;
		return;
	}

	if (object[i]._otype == OBJ_BARRELEX) {
		if (currlevel >= 21 && currlevel <= 24)
			PlaySfxLoc(IS_POPPOP3, object[i].position);
		else if (currlevel >= 17 && currlevel <= 20)
			PlaySfxLoc(IS_POPPOP8, object[i].position);
		else
			PlaySfxLoc(IS_BARLFIRE, object[i].position);
		for (int yp = object[i].position.y - 1; yp <= object[i].position.y + 1; yp++) {
			for (int xp = object[i].position.x - 1; xp <= object[i].position.x + 1; xp++) {
				if (dMonster[xp][yp] > 0)
					MonsterTrapHit(dMonster[xp][yp] - 1, 1, 4, 0, MIS_FIREBOLT, false);
				bool unused;
				if (dPlayer[xp][yp] > 0)
					PlayerMHit(dPlayer[xp][yp] - 1, -1, 0, 8, 16, MIS_FIREBOLT, false, 0, &unused);
				if (dObject[xp][yp] > 0) {
					int oi = dObject[xp][yp] - 1;
					if (object[oi]._otype == OBJ_BARRELEX && object[oi]._oBreak != -1)
						BreakBarrel(pnum, oi, dam, true, sendmsg);
				}
			}
		}
	} else {
		if (currlevel >= 21 && currlevel <= 24)
			PlaySfxLoc(IS_POPPOP2, object[i].position);
		else if (currlevel >= 17 && currlevel <= 20)
			PlaySfxLoc(IS_POPPOP5, object[i].position);
		else
			PlaySfxLoc(IS_BARREL, object[i].position);
		SetRndSeed(object[i]._oRndSeed);
		if (object[i]._oVar2 <= 1) {
			if (object[i]._oVar3 == 0)
				CreateRndUseful(object[i].position, sendmsg);
			else
				CreateRndItem(object[i].position, false, sendmsg, false);
		}
		if (object[i]._oVar2 >= 8)
			SpawnSkeleton(object[i]._oVar4, object[i].position);
	}
	if (pnum == myplr)
		NetSendCmdParam2(false, CMD_BREAKOBJ, pnum, i);
}

void BreakObject(int pnum, int oi)
{
	int objdam = 10;
	if (pnum != -1) {
		int mind = plr[pnum]._pIMinDam;
		int maxd = plr[pnum]._pIMaxDam;
		objdam = GenerateRnd(maxd - mind + 1) + mind;
		objdam += plr[pnum]._pDamageMod + plr[pnum]._pIBonusDamMod + objdam * plr[pnum]._pIBonusDam / 100;
	}

	switch (object[oi]._otype) {
	case OBJ_CRUX1:
	case OBJ_CRUX2:
	case OBJ_CRUX3:
		BreakCrux(oi);
		break;
	case OBJ_BARREL:
	case OBJ_BARRELEX:
		BreakBarrel(pnum, oi, objdam, false, true);
		break;
	default:
		break;
	}
}

void SyncBreakObj(int pnum, int oi)
{
	if (object[oi]._otype >= OBJ_BARREL && object[oi]._otype <= OBJ_BARRELEX)
		BreakBarrel(pnum, oi, 0, true, false);
}

void SyncL1Doors(int i)
{
	if (object[i]._oVar4 == 0) {
		object[i]._oMissFlag = false;
		return;
	}

	object[i]._oMissFlag = true;

	int x = object[i].position.x;
	int y = object[i].position.y;
	object[i]._oSelFlag = 2;
	if (currlevel < 17) {
		if (object[i]._otype == OBJ_L1LDOOR) {
			if (object[i]._oVar1 == 214)
				ObjSetMicro(x, y, 408);
			else
				ObjSetMicro(x, y, 393);
			dSpecial[x][y] = 7;
			objects_set_door_piece(x - 1, y);
			y--;
		} else {
			ObjSetMicro(x, y, 395);
			dSpecial[x][y] = 8;
			objects_set_door_piece(x, y - 1);
			x--;
		}
	} else {
		if (object[i]._otype == OBJ_L1LDOOR) {
			ObjSetMicro(x, y, 206);
			dSpecial[x][y] = 1;
			objects_set_door_piece(x - 1, y);
			y--;
		} else {
			ObjSetMicro(x, y, 209);
			dSpecial[x][y] = 2;
			objects_set_door_piece(x, y - 1);
			x--;
		}
	}
	DoorSet(i, x, y);
}

void SyncCrux(int i)
{
	bool found = true;
	for (int j = 0; j < nobjects; j++) {
		int oi = objectactive[j];
		int type = object[oi]._otype;
		if (IsNoneOf(type, OBJ_CRUX1, OBJ_CRUX2, OBJ_CRUX3))
			continue;
		if (object[i]._oVar8 != object[oi]._oVar8 || object[oi]._oBreak == -1)
			continue;
		found = false;
	}
	if (found)
		ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
}

void SyncLever(int i)
{
	if (object[i]._oSelFlag != 0)
		return;

	ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
}

void SyncQSTLever(int i)
{
	if (object[i]._oAnimFrame == object[i]._oVar6) {
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		if (object[i]._otype == OBJ_BLINDBOOK) {
			int8_t tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			TransVal = tren;
		}
	}
}

void SyncPedistal(int i)
{
	if (object[i]._oVar6 == 1)
		ObjChangeMapResync(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
	if (object[i]._oVar6 == 2) {
		ObjChangeMapResync(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
		ObjChangeMapResync(setpc_x + 6, setpc_y + 3, setpc_x + setpc_w, setpc_y + 7);
	}
	if (object[i]._oVar6 == 3) {
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		LoadMapObjs("Levels\\L2Data\\Blood2.DUN", 2 * setpc_x, 2 * setpc_y);
	}
}

void SyncL2Doors(int i)
{
	object[i]._oMissFlag = object[i]._oVar4 != 0;
	int x = object[i].position.x;
	int y = object[i].position.y;
	object[i]._oSelFlag = 2;
	if (object[i]._otype == OBJ_L2LDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(x, y, 538);
		dSpecial[x][y] = 0;
	} else if (object[i]._otype == OBJ_L2LDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(x, y, 13);
		dSpecial[x][y] = 5;
	} else if (object[i]._otype == OBJ_L2RDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(x, y, 540);
		dSpecial[x][y] = 0;
	} else if (object[i]._otype == OBJ_L2RDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(x, y, 17);
		dSpecial[x][y] = 6;
	}
}

void SyncL3Doors(int i)
{
	object[i]._oMissFlag = true;
	int x = object[i].position.x;
	int y = object[i].position.y;
	object[i]._oSelFlag = 2;
	if (object[i]._otype == OBJ_L3LDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(x, y, 531);
	} else if (object[i]._otype == OBJ_L3LDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(x, y, 538);
	} else if (object[i]._otype == OBJ_L3RDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(x, y, 534);
	} else if (object[i]._otype == OBJ_L3RDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(x, y, 541);
	}
}

void SyncObjectAnim(int o)
{
	object_graphic_id index = AllObjects[object[o]._otype].ofindex;

	const auto &found = std::find(std::begin(ObjFileList), std::end(ObjFileList), index);
	if (found == std::end(ObjFileList)) {
		LogCritical("Unable to find object_graphic_id {} in list of objects to load, level generation error.", index);
		return;
	}

	const int i = std::distance(std::begin(ObjFileList), found);

	object[o]._oAnimData = pObjCels[i].get();
	switch (object[o]._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		SyncL1Doors(o);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		SyncL2Doors(o);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		SyncL3Doors(o);
		break;
	case OBJ_CRUX1:
	case OBJ_CRUX2:
	case OBJ_CRUX3:
		SyncCrux(o);
		break;
	case OBJ_LEVER:
	case OBJ_BOOK2L:
	case OBJ_SWITCHSKL:
		SyncLever(o);
		break;
	case OBJ_BOOK2R:
	case OBJ_BLINDBOOK:
	case OBJ_STEELTOME:
		SyncQSTLever(o);
		break;
	case OBJ_PEDISTAL:
		SyncPedistal(o);
		break;
	default:
		break;
	}
}

void GetObjectStr(int i)
{
	switch (object[i]._otype) {
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
		if (object[i]._oVar4 == 1)
			strcpy(infostr, _("Open Door"));
		if (object[i]._oVar4 == 0)
			strcpy(infostr, _("Closed Door"));
		if (object[i]._oVar4 == 2)
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
			strcpy(infostr, _("Pod"));               //Then a barrel is called a pod
		else if (currlevel >= 21 && currlevel <= 24) // for crypt levels
			strcpy(infostr, _("Urn"));               //Then a barrel is called an urn
		else
			strcpy(infostr, _("Barrel"));
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		strcpy(tempstr, fmt::format(_(/* TRANSLATORS: {:s} will be a name from the Shrine block above */ "{:s} Shrine"), _(ShrineNames[object[i]._oVar1])).c_str());
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
		strcpy(infostr, _(StoryBookName[object[i]._oVar3]));
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
	if (plr[myplr]._pClass == HeroClass::Rogue) {
		if (object[i]._oTrapFlag) {
			strcpy(tempstr, fmt::format(_(/* TRANSLATORS: {:s} will either be a chest or a door */ "Trapped {:s}"), infostr).c_str());
			strcpy(infostr, tempstr);
			infoclr = UIS_RED;
		}
	}
	if (objectIsDisabled(i)) {
		strcpy(tempstr, fmt::format(_(/* TRANSLATORS: If user enabled diablo.ini setting "Disable Crippling Shrines" is set to 1; also used for Na-Kruls leaver */ "{:s} (disabled)"), infostr).c_str());
		strcpy(infostr, tempstr);
		infoclr = UIS_RED;
	}
}

void OperateNakrulLever()
{
	if (currlevel == 24) {
		PlaySfxLoc(IS_CROPEN, { UberRow, UberCol });
		//the part below is the same as SyncNakrulRoom
		dPiece[UberRow][UberCol] = 298;
		dPiece[UberRow][UberCol - 1] = 301;
		dPiece[UberRow][UberCol - 2] = 300;
		dPiece[UberRow][UberCol + 1] = 299;
		SetDungeonMicros();
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
	AddObject(OBJ_LEVER, UberRow + 3, UberCol - 1);
}

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

} // namespace devilution
