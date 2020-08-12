#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int trapid;
int trapdir;
BYTE *pObjCels[40];
char ObjFileList[40];
int objectactive[MAXOBJECTS];
int nobjects;
int leverid;
int objectavail[MAXOBJECTS];
ObjectStruct object[MAXOBJECTS];
BOOL InitObjFlag;
int numobjfiles;

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
char shrinemin[NUM_SHRINETYPE] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1
};
char shrinemax[NUM_SHRINETYPE] = {
	16, 16, 16, 16, 16, 16, 16, 8, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16
};
/** 0 - sp+mp, 1 - sp only, 2 - mp only */
BYTE shrineavail[NUM_SHRINETYPE] = {
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
	{ TEXT_BOOK11, TEXT_BOOK12, TEXT_BOOK13 },
	{ TEXT_BOOK21, TEXT_BOOK22, TEXT_BOOK23 },
	{ TEXT_BOOK31, TEXT_BOOK32, TEXT_BOOK33 }
};

void InitObjectGFX()
{
	BOOLEAN fileload[56];
	char filestr[32];
	int i, j;

	memset(fileload, FALSE, sizeof(fileload));

	for (i = 0; AllObjects[i].oload != -1; i++) {
		if (AllObjects[i].oload == 1
		    && (int)currlevel >= AllObjects[i].ominlvl
		    && (int)currlevel <= AllObjects[i].omaxlvl) {
			fileload[AllObjects[i].ofindex] = TRUE;
		}
		if (AllObjects[i].otheme != THEME_NONE) {
			for (j = 0; j < numthemes; j++) {
				if (themes[j].ttype == AllObjects[i].otheme)
					fileload[AllObjects[i].ofindex] = TRUE;
			}
		}

		if (AllObjects[i].oquest != -1) {
			if (QuestStatus(AllObjects[i].oquest))
				fileload[AllObjects[i].ofindex] = TRUE;
		}
	}

	for (i = 0; i < 56; i++) {
		if (fileload[i]) {
			ObjFileList[numobjfiles] = i;
			sprintf(filestr, "Objects\\%s.CEL", ObjMasterLoadList[i]);
			pObjCels[numobjfiles] = LoadFileInMem(filestr, NULL);
			numobjfiles++;
		}
	}
}

void FreeObjectGFX()
{
	int i;

	for (i = 0; i < numobjfiles; i++) {
		MemFreeDbg(pObjCels[i]);
	}
	numobjfiles = 0;
}

BOOL RndLocOk(int xp, int yp)
{
	if (dMonster[xp][yp])
		return FALSE;
	if (dPlayer[xp][yp])
		return FALSE;
	if (dObject[xp][yp])
		return FALSE;
	if (dFlags[xp][yp] & BFLAG_POPULATED)
		return FALSE;
	if (nSolidTable[dPiece[xp][yp]])
		return FALSE;
	if (leveltype != DTYPE_CATHEDRAL || dPiece[xp][yp] <= 126 || dPiece[xp][yp] >= 144)
		return TRUE;
	return FALSE;
}

void InitRndLocObj(int min, int max, int objtype)
{
	int i, xp, yp, numobjs;

	numobjs = random_(139, max - min) + min;

	for (i = 0; i < numobjs; i++) {
		while (1) {
			xp = random_(139, 80) + 16;
			yp = random_(139, 80) + 16;
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

void InitRndLocBigObj(int min, int max, int objtype)
{
	int i, xp, yp, numobjs;

	numobjs = random_(140, max - min) + min;
	for (i = 0; i < numobjs; i++) {
		while (1) {
			xp = random_(140, 80) + 16;
			yp = random_(140, 80) + 16;
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

void InitRndLocObj5x5(int min, int max, int objtype)
{
	BOOL exit;
	int xp, yp, numobjs, i, cnt, m, n;

	numobjs = min + random_(139, max - min);
	for (i = 0; i < numobjs; i++) {
		cnt = 0;
		exit = FALSE;
		while (!exit) {
			exit = TRUE;
			xp = random_(139, 80) + 16;
			yp = random_(139, 80) + 16;
			for (n = -2; n <= 2; n++) {
				for (m = -2; m <= 2; m++) {
					if (!RndLocOk(xp + m, yp + n))
						exit = FALSE;
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
	int i;

	for (i = 0; i < MAXOBJECTS; i++) {
		object[i]._ox = 0;
		object[i]._oy = 0;
		object[i]._oAnimData = 0;
		object[i]._oAnimDelay = 0;
		object[i]._oAnimCnt = 0;
		object[i]._oAnimLen = 0;
		object[i]._oAnimFrame = 0;
		object[i]._oDelFlag = FALSE;
		object[i]._oVar1 = 0;
		object[i]._oVar2 = 0;
		object[i]._oVar3 = 0;
		object[i]._oVar4 = 0;
	}
	nobjects = 0;
	for (i = 0; i < MAXOBJECTS; i++) {
		objectavail[i] = i;
		objectactive[i] = 0;
	}
	trapdir = 0;
	trapid = 1;
	leverid = 1;
}

void AddTortures()
{
	int ox, oy;

	for (oy = 0; oy < MAXDUNY; oy++) {
		for (ox = 0; ox < MAXDUNX; ox++) {
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
	int tx, ty;

	tx = quests[Q_PWATER]._qtx;
	ty = quests[Q_PWATER]._qty;
	AddObject(OBJ_STORYCANDLE, tx - 2, ty + 1);
	AddObject(OBJ_STORYCANDLE, tx + 3, ty + 1);
	AddObject(OBJ_STORYCANDLE, tx - 1, ty + 2);
	AddObject(OBJ_STORYCANDLE, tx + 2, ty + 2);
}

void AddBookLever(int lx1, int ly1, int lx2, int ly2, int x1, int y1, int x2, int y2, int msg)
{
	DIABOOL exit;
	int xp, yp, ob, cnt, m, n;

	cnt = 0;
	exit = FALSE;
	while (!exit) {
		exit = TRUE;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (n = -2; n <= 2; n++) {
			for (m = -2; m <= 2; m++) {
				if (!RndLocOk(xp + m, yp + n))
					exit = FALSE;
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
	ob = dObject[xp][yp] - 1;
	SetObjMapRange(ob, x1, y1, x2, y2, leverid);
	SetBookMsg(ob, msg);
	leverid++;
	object[ob]._oVar6 = object[ob]._oAnimFrame + 1;
}

void InitRndBarrels()
{
	int numobjs; // number of groups of barrels to generate
	int xp, yp;
	_object_id o;
	BOOL found;
	int p; // regulates chance to stop placing barrels in current group
	int dir;
	int t; // number of tries of placing next barrel in current group
	int c; // number of barrels in current group
	int i;

	numobjs = random_(143, 5) + 3;
	for (i = 0; i < numobjs; i++) {
		do {
			xp = random_(143, 80) + 16;
			yp = random_(143, 80) + 16;
		} while (!RndLocOk(xp, yp));
		o = (random_(143, 4) != 0) ? OBJ_BARREL : OBJ_BARRELEX;
		AddObject(o, xp, yp);
		found = TRUE;
		p = 0;
		c = 1;
		while (random_(143, p) == 0 && found) {
			t = 0;
			found = FALSE;
			while (TRUE) {
				if (t >= 3)
					break;
				dir = random_(143, 8);
				xp += bxadd[dir];
				yp += byadd[dir];
				found = RndLocOk(xp, yp);
				t++;
				if (found)
					break;
			}
			if (found) {
				o = (random_(143, 5) != 0) ? OBJ_BARREL : OBJ_BARRELEX;
				AddObject(o, xp, yp);
				c++;
			}
			p = c >> 1;
		}
	}
}

void AddL1Objs(int x1, int y1, int x2, int y2)
{
	int i, j, pn;

	for (j = y1; j < y2; j++) {
		for (i = x1; i < x2; i++) {
			pn = dPiece[i][j];
			if (pn == 270)
				AddObject(OBJ_L1LIGHT, i, j);
			if (pn == 44 || pn == 51 || pn == 214)
				AddObject(OBJ_L1LDOOR, i, j);
			if (pn == 46 || pn == 56)
				AddObject(OBJ_L1RDOOR, i, j);
		}
	}
}

void AddL2Objs(int x1, int y1, int x2, int y2)
{
	int i, j, pn;

	for (j = y1; j < y2; j++) {
		for (i = x1; i < x2; i++) {
			pn = dPiece[i][j];
			if (pn == 13 || pn == 541)
				AddObject(OBJ_L2LDOOR, i, j);
			if (pn == 17 || pn == 542)
				AddObject(OBJ_L2RDOOR, i, j);
		}
	}
}

void AddL3Objs(int x1, int y1, int x2, int y2)
{
	int i, j, pn;

	for (j = y1; j < y2; j++) {
		for (i = x1; i < x2; i++) {
			pn = dPiece[i][j];
			if (pn == 531)
				AddObject(OBJ_L3LDOOR, i, j);
			if (pn == 534)
				AddObject(OBJ_L3RDOOR, i, j);
		}
	}
}

BOOL WallTrapLocOk(int xp, int yp)
{
	if (dFlags[xp][yp] & BFLAG_POPULATED)
		return FALSE;
	return TRUE;
}

void AddL2Torches()
{
	int i, j, pn;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (!WallTrapLocOk(i, j))
				continue;

			pn = dPiece[i][j];
			if (pn == 1 && random_(145, 3) == 0)
				AddObject(OBJ_TORCHL2, i, j);

			if (pn == 5 && random_(145, 3) == 0)
				AddObject(OBJ_TORCHR2, i, j);

			if (pn == 37 && random_(145, 10) == 0 && dObject[i - 1][j] == 0)
				AddObject(OBJ_TORCHL, i - 1, j);

			if (pn == 41 && random_(145, 10) == 0 && dObject[i][j - 1] == 0)
				AddObject(OBJ_TORCHR, i, j - 1);
		}
	}
}

BOOL TorchLocOK(int xp, int yp)
{
	if (dFlags[xp][yp] & BFLAG_POPULATED)
		return FALSE;

	if (nTrapTable[dPiece[xp][yp]] != FALSE)
		return TRUE;
	else
		return FALSE;
}

void AddObjTraps()
{
	char oi_trap, oi;
	int i, j;
	int xp, yp;
	int rndv;

	if (currlevel == 1)
		rndv = 10;
	if (currlevel >= 2)
		rndv = 15;
	if (currlevel >= 5)
		rndv = 20;
	if (currlevel >= 7)
		rndv = 25;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dObject[i][j] <= 0 || random_(144, 100) >= rndv)
				continue;

			oi = dObject[i][j] - 1;
			if (!AllObjects[object[oi]._otype].oTrapFlag)
				continue;

			if (random_(144, 2) == 0) {
				xp = i - 1;
				while (!nSolidTable[dPiece[xp][j]])
					xp--;

				if (!TorchLocOK(xp, j) || i - xp <= 1)
					continue;

				AddObject(OBJ_TRAPL, xp, j);
				oi_trap = dObject[xp][j] - 1;
				object[oi_trap]._oVar1 = i;
				object[oi_trap]._oVar2 = j;
				object[oi]._oTrapFlag = TRUE;
			} else {
				yp = j - 1;
				while (!nSolidTable[dPiece[i][yp]])
					yp--;

				if (!TorchLocOK(i, yp) || j - yp <= 1)
					continue;

				AddObject(OBJ_TRAPR, i, yp);
				oi_trap = dObject[i][yp] - 1;
				object[oi_trap]._oVar1 = i;
				object[oi_trap]._oVar2 = j;
				object[oi]._oTrapFlag = TRUE;
			}
		}
	}
}

void AddChestTraps()
{
	int i, j;
	char oi;

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dObject[i][j] > 0) {
				oi = dObject[i][j] - 1;
				if (object[oi]._otype >= OBJ_CHEST1 && object[oi]._otype <= OBJ_CHEST3 && !object[oi]._oTrapFlag && random_(0, 100) < 10) {
					object[oi]._otype += OBJ_BOOKCASER;
					object[oi]._oTrapFlag = TRUE;
					if (leveltype == DTYPE_CATACOMBS) {
						object[oi]._oVar4 = random_(0, 2);
					} else {
						object[oi]._oVar4 = random_(0, 3);
					}
				}
			}
		}
	}
}

void LoadMapObjects(BYTE *pMap, int startx, int starty, int x1, int y1, int w, int h, int leveridx)
{
	int rw, rh, i, j, oi;
	BYTE *lm;
	long mapoff;

	InitObjFlag = TRUE;

	lm = pMap + 2;
	rw = pMap[0];
	rh = *lm;
	mapoff = (rw * rh + 1) * 2;
	rw <<= 1;
	rh <<= 1;
	mapoff += rw * 2 * rh * 2;
	lm += mapoff;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				AddObject(ObjTypeConv[*lm], startx + 16 + i, starty + 16 + j);
				oi = ObjIndex(startx + 16 + i, starty + 16 + j);
				SetObjMapRange(oi, x1, y1, x1 + w, y1 + h, leveridx);
			}
			lm += 2;
		}
	}
	InitObjFlag = FALSE;
}

void LoadMapObjs(BYTE *pMap, int startx, int starty)
{
	int rw, rh;
	int i, j;
	BYTE *lm;
	long mapoff;

	InitObjFlag = TRUE;
	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	mapoff = (rw * rh + 1) * 2;
	rw <<= 1;
	rh <<= 1;
	mapoff += 2 * rw * rh * 2;
	lm += mapoff;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				AddObject(ObjTypeConv[*lm], startx + 16 + i, starty + 16 + j);
			}
			lm += 2;
		}
	}
	InitObjFlag = FALSE;
}

void AddDiabObjs()
{
	BYTE *lpSetPiece;

	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab1.DUN", NULL);
	LoadMapObjects(lpSetPiece, 2 * diabquad1x, 2 * diabquad1y, diabquad2x, diabquad2y, 11, 12, 1);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", NULL);
	LoadMapObjects(lpSetPiece, 2 * diabquad2x, 2 * diabquad2y, diabquad3x, diabquad3y, 11, 11, 2);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", NULL);
	LoadMapObjects(lpSetPiece, 2 * diabquad3x, 2 * diabquad3y, diabquad4x, diabquad4y, 9, 9, 3);
	mem_free_dbg(lpSetPiece);
}

void AddStoryBooks()
{
	int xp, yp, xx, yy;
	int cnt;
	BOOL done;

	cnt = 0;
	done = FALSE;
	while (!done) {
		done = TRUE;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (yy = -2; yy <= 2; yy++) {
			for (xx = -3; xx <= 3; xx++) {
				if (!RndLocOk(xx + xp, yy + yp))
					done = FALSE;
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
	int i, j, ii, jj;

	for (j = 0; j < 40; j++) {
		jj = 16 + j * 2;
		for (i = 0; i < 40; i++) {
			ii = 16 + i * 2;
			if (dungeon[i][j] != 1 && dungeon[i][j] != 2)
				continue;
			if (random_(0, freq) != 0)
				continue;
			if (!SkipThemeRoom(i, j))
				continue;
			if (dungeon[i][j] == 1 && dungeon[i + 1][j] == 6) {
				switch (random_(0, 3)) {
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
				switch (random_(0, 2)) {
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
	int xp, yp, xx, yy;
	int cnt;
	BOOL found;

	cnt = 0;
	found = FALSE;
	while (!found) {
		found = TRUE;
		xp = random_(139, 80) + 16;
		yp = random_(139, 80) + 16;
		for (yy = -3; yy <= 3; yy++) {
			for (xx = -2; xx <= 3; xx++) {
				if (!RndLocOk(xp + xx, yp + yy))
					found = FALSE;
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
	int sp_id;
	BYTE *mem;

	ClrAllObjects();
	if (currlevel == 16) {
		AddDiabObjs();
	} else {
		InitObjFlag = TRUE;
		GetRndSeed();
		if (currlevel == 9 && gbMaxPlayers == 1)
			AddSlainHero();
		if (currlevel == quests[Q_MUSHROOM]._qlevel && quests[Q_MUSHROOM]._qactive == QUEST_INIT)
			AddMushPatch();
		if (currlevel == 4)
			AddStoryBooks();
		if (currlevel == 8)
			AddStoryBooks();
		if (currlevel == 12)
			AddStoryBooks();
		if (leveltype == DTYPE_CATHEDRAL) {
			if (QuestStatus(Q_BUTCHER))
				AddTortures();
			if (QuestStatus(Q_PWATER))
				AddCandles();
			if (QuestStatus(Q_LTBANNER))
				AddObject(OBJ_SIGNCHEST, 2 * setpc_x + 26, 2 * setpc_y + 19);
			InitRndLocBigObj(10, 15, OBJ_SARC);
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
				if (plr[myplr]._pClass == PC_WARRIOR) {
					sp_id = TEXT_BLINDING;
				} else if (plr[myplr]._pClass == PC_ROGUE) {
					sp_id = TEXT_RBLINDING;
				} else if (plr[myplr]._pClass == PC_SORCERER) {
					sp_id = TEXT_MBLINDING;
				}
				quests[Q_BLIND]._qmsg = sp_id;
				AddBookLever(0, 0, MAXDUNX, MAXDUNY, setpc_x, setpc_y, setpc_w + setpc_x + 1, setpc_h + setpc_y + 1, sp_id);
				mem = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", NULL);
				LoadMapObjs(mem, 2 * setpc_x, 2 * setpc_y);
				mem_free_dbg(mem);
			}
			if (QuestStatus(Q_BLOOD)) {
				if (plr[myplr]._pClass == PC_WARRIOR) {
					sp_id = TEXT_BLOODY;
				} else if (plr[myplr]._pClass == PC_ROGUE) {
					sp_id = TEXT_RBLOODY;
				} else if (plr[myplr]._pClass == PC_SORCERER) {
					sp_id = TEXT_MBLOODY;
				}
				quests[Q_BLOOD]._qmsg = sp_id;
				AddBookLever(0, 0, MAXDUNX, MAXDUNY, setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7, sp_id);
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
				if (plr[myplr]._pClass == PC_WARRIOR) {
					sp_id = TEXT_BLOODWAR;
				} else if (plr[myplr]._pClass == PC_ROGUE) {
					sp_id = TEXT_RBLOODWAR;
				} else if (plr[myplr]._pClass == PC_SORCERER) {
					sp_id = TEXT_MBLOODWAR;
				}
				quests[Q_WARLORD]._qmsg = sp_id;
				AddBookLever(0, 0, MAXDUNX, MAXDUNY, setpc_x, setpc_y, setpc_x + setpc_w, setpc_y + setpc_h, sp_id);
				mem = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", NULL);
				LoadMapObjs(mem, 2 * setpc_x, 2 * setpc_y);
				mem_free_dbg(mem);
			}
			if (QuestStatus(Q_BETRAYER) && gbMaxPlayers == 1)
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
		InitObjFlag = FALSE;
	}
}

#ifndef SPAWN
void SetMapObjects(BYTE *pMap, int startx, int starty)
{
	int rw, rh;
	int i, j;
	BYTE *lm, *h;
	long mapoff;
	int fileload[56];
	char filestr[32];

	ClrAllObjects();
	for (i = 0; i < 56; i++)
		fileload[i] = FALSE;
	InitObjFlag = TRUE;

	for (i = 0; AllObjects[i].oload != -1; i++) {
		if (AllObjects[i].oload == 1 && leveltype == AllObjects[i].olvltype)
			fileload[AllObjects[i].ofindex] = TRUE;
	}

	lm = pMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	mapoff = (rw * rh + 1) * 2;
	rw <<= 1;
	rh <<= 1;
	mapoff += 2 * rw * rh * 2;
	lm += mapoff;
	h = lm;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				fileload[AllObjects[ObjTypeConv[*lm]].ofindex] = TRUE;
			}
			lm += 2;
		}
	}

	for (i = 0; i < 56; i++) {
		if (!fileload[i])
			continue;

		ObjFileList[numobjfiles] = i;
		sprintf(filestr, "Objects\\%s.CEL", ObjMasterLoadList[i]);
		pObjCels[numobjfiles] = LoadFileInMem(filestr, NULL);
		numobjfiles++;
	}

	lm = h;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm)
				AddObject(ObjTypeConv[*lm], startx + 16 + i, starty + 16 + j);
			lm += 2;
		}
	}
	InitObjFlag = FALSE;
}
#endif

void DeleteObject_(int oi, int i)
{
	int ox, oy;

	ox = object[oi]._ox;
	oy = object[oi]._oy;
	dObject[ox][oy] = 0;
	objectavail[-nobjects + MAXOBJECTS] = oi;
	nobjects--;
	if (nobjects > 0 && i != nobjects)
		objectactive[i] = objectactive[nobjects];
}

void SetupObject(int i, int x, int y, int ot)
{
	int ofi;
	int j;

	object[i]._otype = ot;
	ofi = AllObjects[ot].ofindex;
	object[i]._ox = x;
	object[i]._oy = y;
	j = 0;
	while (ObjFileList[j] != ofi) {
		j++;
	}
	object[i]._oAnimData = pObjCels[j];
	object[i]._oAnimFlag = AllObjects[ot].oAnimFlag;
	if (AllObjects[ot].oAnimFlag) {
		object[i]._oAnimDelay = AllObjects[ot].oAnimDelay;
		object[i]._oAnimCnt = random_(146, AllObjects[ot].oAnimDelay);
		object[i]._oAnimLen = AllObjects[ot].oAnimLen;
		object[i]._oAnimFrame = random_(146, AllObjects[ot].oAnimLen - 1) + 1;
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
	object[i]._oDelFlag = FALSE;
	object[i]._oBreak = AllObjects[ot].oBreak;
	object[i]._oSelFlag = AllObjects[ot].oSelFlag;
	object[i]._oPreFlag = FALSE;
	object[i]._oTrapFlag = FALSE;
	object[i]._oDoorFlag = FALSE;
}

void SetObjMapRange(int i, int x1, int y1, int x2, int y2, int v)
{
	object[i]._oVar1 = x1;
	object[i]._oVar2 = y1;
	object[i]._oVar3 = x2;
	object[i]._oVar4 = y2;
	object[i]._oVar8 = v;
}

void SetBookMsg(int i, int msg)
{
	object[i]._oVar7 = msg;
}

void AddL1Door(int i, int x, int y, int ot)
{
	object[i]._oDoorFlag = TRUE;
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
	if (!random_(147, 2))
		object[i]._oAnimFrame += 3;
	object[i]._oRndSeed = GetRndSeed();
	switch (t) {
	case OBJ_CHEST1:
	case OBJ_TCHEST1:
		if (setlevel) {
			object[i]._oVar1 = 1;
			break;
		}
		object[i]._oVar1 = random_(147, 2);
		break;
	case OBJ_TCHEST2:
	case OBJ_CHEST2:
		if (setlevel) {
			object[i]._oVar1 = 2;
			break;
		}
		object[i]._oVar1 = random_(147, 3);
		break;
	case OBJ_TCHEST3:
	case OBJ_CHEST3:
		if (setlevel) {
			object[i]._oVar1 = 3;
			break;
		}
		object[i]._oVar1 = random_(147, 4);
		break;
	}
	object[i]._oVar2 = random_(147, 8);
}

void AddL2Door(int i, int x, int y, int ot)
{
	object[i]._oDoorFlag = TRUE;
	if (ot == OBJ_L2LDOOR)
		ObjSetMicro(x, y, 538);
	else
		ObjSetMicro(x, y, 540);
	object[i]._oVar4 = 0;
}

void AddL3Door(int i, int x, int y, int ot)
{
	object[i]._oDoorFlag = TRUE;
	if (ot == OBJ_L3LDOOR)
		ObjSetMicro(x, y, 531);
	else
		ObjSetMicro(x, y, 534);
	object[i]._oVar4 = 0;
}

void AddSarc(int i)
{
	dObject[object[i]._ox][object[i]._oy - 1] = -(i + 1);
	object[i]._oVar1 = random_(153, 10);
	object[i]._oRndSeed = GetRndSeed();
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

void AddTrap(int i, int ot)
{
	int mt;

	mt = currlevel / 3 + 1;
	mt = random_(148, mt);
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
		DoLighting(object[i]._ox, object[i]._oy, r, -1);
		object[i]._oVar1 = -1;
	} else {
		object[i]._oVar1 = 0;
	}
}

void AddBarrel(int i, int t)
{
	object[i]._oVar1 = 0;
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oVar2 = (t == OBJ_BARRELEX) ? 0 : random_(149, 10);
	object[i]._oVar3 = random_(149, 3);

	if (object[i]._oVar2 >= 8)
		object[i]._oVar4 = PreSpawnSkeleton();
}

void AddShrine(int i)
{
	int val, j;
	BOOL slist[NUM_SHRINETYPE];

	object[i]._oPreFlag = TRUE;
	for (j = 0; j < NUM_SHRINETYPE; j++) {
		if (currlevel < shrinemin[j] || currlevel > shrinemax[j]) {
			slist[j] = 0;
		} else {
			slist[j] = 1;
		}
		if (gbMaxPlayers != 1 && shrineavail[j] == 1) {
			slist[j] = 0;
		}
		if (gbMaxPlayers == 1 && shrineavail[j] == 2) {
			slist[j] = 0;
		}
	}
	do {
		val = random_(150, NUM_SHRINETYPE);
	} while (!slist[val]);

	object[i]._oVar1 = val;
	if (random_(150, 2)) {
		object[i]._oAnimFrame = 12;
		object[i]._oAnimLen = 22;
	}
}

void AddBookcase(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oPreFlag = TRUE;
}

void AddBookstand(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddBloodFtn(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddPurifyingFountain(int i)
{
	int ox, oy;

	ox = object[i]._ox;
	oy = object[i]._oy;
	dObject[ox][oy - 1] = -1 - i;
	dObject[ox - 1][oy] = -1 - i;
	dObject[ox - 1][oy - 1] = -1 - i;
	object[i]._oRndSeed = GetRndSeed();
}

void AddArmorStand(int i)
{
	if (!armorFlag) {
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = 0;
	}

	object[i]._oRndSeed = GetRndSeed();
}

void AddGoatShrine(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddCauldron(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddMurkyFountain(int i)
{
	int ox, oy;

	ox = object[i]._ox;
	oy = object[i]._oy;
	dObject[ox][oy - 1] = -1 - i;
	dObject[ox - 1][oy] = -1 - i;
	dObject[ox - 1][oy - 1] = -1 - i;
	object[i]._oRndSeed = GetRndSeed();
}

void AddTearFountain(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddDecap(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oAnimFrame = random_(151, 8) + 1;
	object[i]._oPreFlag = TRUE;
}

void AddVilebook(int i)
{
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		object[i]._oAnimFrame = 4;
	}
}

void AddMagicCircle(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oPreFlag = TRUE;
	object[i]._oVar6 = 0;
	object[i]._oVar5 = 1;
}

void AddBrnCross(int i)
{
	object[i]._oRndSeed = GetRndSeed();
}

void AddPedistal(int i)
{
	object[i]._oVar1 = setpc_x;
	object[i]._oVar2 = setpc_y;
	object[i]._oVar3 = setpc_x + setpc_w;
	object[i]._oVar4 = setpc_y + setpc_h;
}

void AddStoryBook(int i)
{
	int bookframe;

	SetRndSeed(glSeedTbl[16]);
	bookframe = random_(0, 3);

	object[i]._oVar1 = bookframe;
	if (currlevel == 4)
		object[i]._oVar2 = StoryText[bookframe][0];
	if (currlevel == 8)
		object[i]._oVar2 = StoryText[bookframe][1];
	if (currlevel == 12)
		object[i]._oVar2 = StoryText[bookframe][2];
	object[i]._oVar3 = (currlevel >> 2) + 3 * bookframe - 1;
	object[i]._oAnimFrame = 5 - 2 * bookframe;
	object[i]._oVar4 = object[i]._oAnimFrame + 1;
}

void AddWeaponRack(int i)
{
	if (!weaponFlag) {
		object[i]._oAnimFlag = 2;
		object[i]._oSelFlag = 0;
	}
	object[i]._oRndSeed = GetRndSeed();
}

void AddTorturedBody(int i)
{
	object[i]._oRndSeed = GetRndSeed();
	object[i]._oAnimFrame = random_(0, 4) + 1;
	object[i]._oPreFlag = TRUE;
}

void GetRndObjLoc(int randarea, int *xx, int *yy)
{
	BOOL failed;
	int i, j, tries;

	if (randarea == 0)
		return;

	tries = 0;
	while (1) {
		tries++;
		if (tries > 1000 && randarea > 1)
			randarea--;
		*xx = random_(0, MAXDUNX);
		*yy = random_(0, MAXDUNY);
		failed = FALSE;
		for (i = 0; i < randarea && !failed; i++) {
			for (j = 0; j < randarea && !failed; j++) {
				failed = !RndLocOk(i + *xx, j + *yy);
			}
		}
		if (!failed)
			break;
	}
}

void AddMushPatch()
{
	int i;
	int y, x;

	if (nobjects < MAXOBJECTS) {
		i = objectavail[0];
		GetRndObjLoc(5, &x, &y);
		dObject[x + 1][y + 1] = -1 - i;
		dObject[x + 2][y + 1] = -1 - i;
		dObject[x + 1][y + 2] = -1 - i;
		AddObject(OBJ_MUSHPATCH, x + 2, y + 2);
	}
}

void AddSlainHero()
{
	int x, y;

	GetRndObjLoc(5, &x, &y);
	AddObject(OBJ_SLAINHERO, x + 2, y + 2);
}

void AddObject(int ot, int ox, int oy)
{
	int oi;

	if (nobjects >= MAXOBJECTS)
		return;

	oi = objectavail[0];
	objectavail[0] = objectavail[126 - nobjects];
	objectactive[nobjects] = oi;
	dObject[ox][oy] = oi + 1;
	SetupObject(oi, ox, oy, ot);
	switch (ot) {
	case OBJ_L1LIGHT:
		AddObjLight(oi, 5);
		break;
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
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		AddChest(oi, ot);
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
		AddTrap(oi, ot);
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
	}
	object[oi]._oAnimWidth2 = (object[oi]._oAnimWidth - 64) >> 1;
	nobjects++;
}

void Obj_Light(int i, int lr)
{
	int ox, oy, dx, dy, p, tr;
	BOOL turnon;

	if (object[i]._oVar1 != -1) {
		ox = object[i]._ox;
		oy = object[i]._oy;
		tr = lr + 10;
		turnon = FALSE;
		if (!lightflag) {
			for (p = 0; p < MAX_PLRS && !turnon; p++) {
				if (plr[p].plractive) {
					if (currlevel == plr[p].plrlevel) {
						dx = abs(plr[p]._px - ox);
						dy = abs(plr[p]._py - oy);
						if (dx < tr && dy < tr)
							turnon = TRUE;
					}
				}
			}
		}
		if (turnon) {
			if (!object[i]._oVar1)
				object[i]._olid = AddLight(ox, oy, lr);
			object[i]._oVar1 = 1;
		} else {
			if (object[i]._oVar1 == 1)
				AddUnLight(object[i]._olid);
			object[i]._oVar1 = 0;
		}
	}
}

void Obj_Circle(int i)
{
	int ox, oy, wx, wy;

	ox = object[i]._ox;
	oy = object[i]._oy;
	wx = plr[myplr]._px;
	wy = plr[myplr]._py;
	if (wx == ox && wy == oy) {
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
			if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE)
				quests[Q_BETRAYER]._qvar1 = 4;
			AddMissile(plr[myplr]._px, plr[myplr]._py, 35, 46, plr[myplr]._pdir, MIS_RNDTELEPORT, 0, myplr, 0, 0);
			track_repeat_walk(FALSE);
			sgbMouseDown = 0;
			ClrPlrPath(myplr);
			StartStand(myplr, 0);
		}
	} else {
		if (object[i]._otype == OBJ_MCIRCLE1)
			object[i]._oAnimFrame = 1;
		if (object[i]._otype == OBJ_MCIRCLE2)
			object[i]._oAnimFrame = 3;
		object[i]._oVar6 = 0;
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
	int dx, dy;
	BOOL dok;

	if (object[i]._oVar4 == 0) {
		object[i]._oSelFlag = 3;
		object[i]._oMissFlag = FALSE;
	} else {
		dx = object[i]._ox;
		dy = object[i]._oy;
		dok = !dMonster[dx][dy];
		dok = dok & !dItem[dx][dy];
		dok = dok & !dDead[dx][dy];
		dok = dok & !dPlayer[dx][dy];
		object[i]._oSelFlag = 2;
		object[i]._oVar4 = dok ? 1 : 2;
		object[i]._oMissFlag = TRUE;
	}
}

void Obj_Sarc(int i)
{
	if (object[i]._oAnimFrame == object[i]._oAnimLen)
		object[i]._oAnimFlag = 0;
}

void ActivateTrapLine(int ttype, int tid)
{
	int i, oi;

	for (i = 0; i < nobjects; i++) {
		oi = objectactive[i];
		if (object[oi]._otype == ttype && object[oi]._oVar1 == tid) {
			object[oi]._oVar4 = 1;
			object[oi]._oAnimFlag = 1;
			object[oi]._oAnimDelay = 1;
			object[oi]._olid = AddLight(object[oi]._ox, object[oi]._oy, 1);
		}
	}
}

void Obj_FlameTrap(int i)
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

void Obj_Trap(int i)
{
	int oti, dir;
	BOOLEAN otrig;
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
			if (object[oti]._oSelFlag == 0)
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
			object[oti]._oTrapFlag = FALSE;
		}
	}
}

void Obj_BCrossDamage(int i)
{
	int fire_resist;
	int damage[4] = { 6, 8, 10, 12 };

	if (plr[myplr]._pmode == PM_DEATH)
		return;

	fire_resist = plr[myplr]._pFireResist;
	if (fire_resist > 0)
		damage[leveltype - 1] -= fire_resist * damage[leveltype - 1] / 100;

	if (plr[myplr]._px != object[i]._ox || plr[myplr]._py != object[i]._oy - 1)
		return;

	plr[myplr]._pHitPoints -= damage[leveltype - 1];
	plr[myplr]._pHPBase -= damage[leveltype - 1];
	if (plr[myplr]._pHitPoints >> 6 <= 0) {
		SyncPlrKill(myplr, 0);
	} else {
		if (plr[myplr]._pClass == PC_WARRIOR) {
			PlaySfxLoc(PS_WARR68, plr[myplr]._px, plr[myplr]._py);
#ifndef SPAWN
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			PlaySfxLoc(PS_ROGUE68, plr[myplr]._px, plr[myplr]._py);
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			PlaySfxLoc(PS_MAGE68, plr[myplr]._px, plr[myplr]._py);
#endif
		}
	}
	drawhpflag = TRUE;
}

void ProcessObjects()
{
	int oi;
	int i;

	for (i = 0; i < nobjects; ++i) {
		oi = objectactive[i];
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
	i = 0;
	while (i < nobjects) {
		oi = objectactive[i];
		if (object[oi]._oDelFlag) {
			DeleteObject_(oi, i);
			i = 0;
		} else {
			i++;
		}
	}
}

void ObjSetMicro(int dx, int dy, int pn)
{
	WORD *v;
	MICROS *defs;
	int i;

	dPiece[dx][dy] = pn;
	pn--;
	defs = &dpiece_defs_map_2[dx][dy];
	if (leveltype != DTYPE_HELL) {
		v = (WORD *)pLevelPieces + 10 * pn;
		for (i = 0; i < 10; i++) {
			defs->mt[i] = SDL_SwapLE16(v[(i & 1) - (i & 0xE) + 8]);
		}
	} else {
		v = (WORD *)pLevelPieces + 16 * pn;
		for (i = 0; i < 16; i++) {
			defs->mt[i] = SDL_SwapLE16(v[(i & 1) - (i & 0xE) + 14]);
		}
	}
}

void objects_set_door_piece(int x, int y)
{
	int pn;
	long v1, v2;

	pn = dPiece[x][y] - 1;

	v1 = *((WORD *)pLevelPieces + 10 * pn + 8);
	v2 = *((WORD *)pLevelPieces + 10 * pn + 9);
	dpiece_defs_map_2[x][y].mt[0] = SDL_SwapLE16(v1);
	dpiece_defs_map_2[x][y].mt[1] = SDL_SwapLE16(v2);
}

void ObjSetMini(int x, int y, int v)
{
	int xx, yy;
	long v1, v2, v3, v4;
	WORD *MegaTiles;

	MegaTiles = (WORD *)&pMegaTiles[((WORD)v - 1) * 8];
	v1 = SDL_SwapLE16(*(MegaTiles + 0)) + 1;
	v2 = SDL_SwapLE16(*(MegaTiles + 1)) + 1;
	v3 = SDL_SwapLE16(*(MegaTiles + 2)) + 1;
	v4 = SDL_SwapLE16(*(MegaTiles + 3)) + 1;

	xx = 2 * x + 16;
	yy = 2 * y + 16;
	ObjSetMicro(xx, yy, v1);
	ObjSetMicro(xx + 1, yy, v2);
	ObjSetMicro(xx, yy + 1, v3);
	ObjSetMicro(xx + 1, yy + 1, v4);
}

void ObjL1Special(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (i = y1; i <= y2; ++i) {
		for (j = x1; j <= x2; ++j) {
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
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
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
			if (dPiece[i][j] == 13)
				dSpecial[i][j] = 5;
			if (dPiece[i][j] == 17)
				dSpecial[i][j] = 6;
		}
	}
	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
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
	int pn;

	pn = dPiece[dx][dy];
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
}

void RedoPlayerVision()
{
	int p;

	for (p = 0; p < MAX_PLRS; p++) {
		if (plr[p].plractive && currlevel == plr[p].plrlevel) {
			ChangeVisionXY(plr[p]._pvid, plr[p]._px, plr[p]._py);
		}
	}
}

void OperateL1RDoor(int pnum, int oi, BOOL sendflag)
{
	int xp, yp;

	if (object[oi]._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}

	xp = object[oi]._ox;
	yp = object[oi]._oy;
	if (object[oi]._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		ObjSetMicro(xp, yp, 395);
		dSpecial[xp][yp] = 8;
		objects_set_door_piece(xp, yp - 1);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		DoorSet(oi, xp - 1, yp);
		object[oi]._oVar4 = 1;
		object[oi]._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, xp, object[oi]._oy);
	if (((dDead[xp][yp] != 0 ? 0 : 1) & (dMonster[xp][yp] != 0 ? 0 : 1) & (dItem[xp][yp] != 0 ? 0 : 1)) != 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, oi);
		object[oi]._oVar4 = 0;
		object[oi]._oSelFlag = 3;
		ObjSetMicro(xp, yp, object[oi]._oVar1);
		if (object[oi]._oVar2 != 50) {
			ObjSetMicro(xp - 1, yp, object[oi]._oVar2);
		} else {
			if (dPiece[xp - 1][yp] == 396)
				ObjSetMicro(xp - 1, yp, 411);
			else
				ObjSetMicro(xp - 1, yp, 50);
		}
		object[oi]._oAnimFrame -= 2;
		object[oi]._oPreFlag = FALSE;
		RedoPlayerVision();
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL1LDoor(int pnum, int oi, BOOL sendflag)
{
	int xp, yp;

	if (object[oi]._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}

	xp = object[oi]._ox;
	yp = object[oi]._oy;
	if (object[oi]._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		if (object[oi]._oVar1 == 214)
			ObjSetMicro(xp, yp, 408);
		else
			ObjSetMicro(xp, yp, 393);
		dSpecial[xp][yp] = 7;
		objects_set_door_piece(xp - 1, yp);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		DoorSet(oi, xp, yp - 1);
		object[oi]._oVar4 = 1;
		object[oi]._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, xp, object[oi]._oy);
	if (((dDead[xp][yp] != 0 ? 0 : 1) & (dMonster[xp][yp] != 0 ? 0 : 1) & (dItem[xp][yp] != 0 ? 0 : 1)) != 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, oi);
		object[oi]._oVar4 = 0;
		object[oi]._oSelFlag = 3;
		ObjSetMicro(xp, yp, object[oi]._oVar1);
		if (object[oi]._oVar2 != 50) {
			ObjSetMicro(xp, yp - 1, object[oi]._oVar2);
		} else {
			if (dPiece[xp][yp - 1] == 396)
				ObjSetMicro(xp, yp - 1, 412);
			else
				ObjSetMicro(xp, yp - 1, 50);
		}
		object[oi]._oAnimFrame -= 2;
		object[oi]._oPreFlag = FALSE;
		RedoPlayerVision();
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL2RDoor(int pnum, int oi, BOOL sendflag)
{
	int xp, yp;

	if (object[oi]._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}
	xp = object[oi]._ox;
	yp = object[oi]._oy;
	if (object[oi]._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		ObjSetMicro(xp, yp, 17);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		object[oi]._oVar4 = 1;
		object[oi]._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, yp);
	if (((dDead[xp][yp] != 0 ? 0 : 1) & (dMonster[xp][yp] != 0 ? 0 : 1) & (dItem[xp][yp] != 0 ? 0 : 1)) != 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, oi);
		object[oi]._oVar4 = 0;
		object[oi]._oSelFlag = 3;
		ObjSetMicro(xp, yp, 540);
		object[oi]._oAnimFrame -= 2;
		object[oi]._oPreFlag = FALSE;
		RedoPlayerVision();
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL2LDoor(int pnum, int oi, BOOL sendflag)
{
	int xp, yp;

	if (object[oi]._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}
	xp = object[oi]._ox;
	yp = object[oi]._oy;
	if (object[oi]._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		ObjSetMicro(xp, yp, 13);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		object[oi]._oVar4 = 1;
		object[oi]._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, yp);
	if (((dDead[xp][yp] != 0 ? 0 : 1) & (dMonster[xp][yp] != 0 ? 0 : 1) & (dItem[xp][yp] != 0 ? 0 : 1)) != 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, oi);
		object[oi]._oVar4 = 0;
		object[oi]._oSelFlag = 3;
		ObjSetMicro(xp, yp, 538);
		object[oi]._oAnimFrame -= 2;
		object[oi]._oPreFlag = FALSE;
		RedoPlayerVision();
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL3RDoor(int pnum, int oi, BOOL sendflag)
{
	int xp, yp;

	if (object[oi]._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}

	xp = object[oi]._ox;
	yp = object[oi]._oy;
	if (object[oi]._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		ObjSetMicro(xp, yp, 541);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		object[oi]._oVar4 = 1;
		object[oi]._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, yp);
	if (((dDead[xp][yp] != 0 ? 0 : 1) & (dMonster[xp][yp] != 0 ? 0 : 1) & (dItem[xp][yp] != 0 ? 0 : 1)) != 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, oi);
		object[oi]._oVar4 = 0;
		object[oi]._oSelFlag = 3;
		ObjSetMicro(xp, yp, 534);
		object[oi]._oAnimFrame -= 2;
		object[oi]._oPreFlag = FALSE;
		RedoPlayerVision();
	} else {
		object[oi]._oVar4 = 2;
	}
}

void OperateL3LDoor(int pnum, int oi, BOOL sendflag)
{
	int xp, yp;

	if (object[oi]._oVar4 == 2) {
		if (!deltaload)
			PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, object[oi]._oy);
		return;
	}

	xp = object[oi]._ox;
	yp = object[oi]._oy;
	if (object[oi]._oVar4 == 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_OPENDOOR, oi);
		if (!deltaload)
			PlaySfxLoc(IS_DOOROPEN, object[oi]._ox, object[oi]._oy);
		ObjSetMicro(xp, yp, 538);
		object[oi]._oAnimFrame += 2;
		object[oi]._oPreFlag = TRUE;
		object[oi]._oVar4 = 1;
		object[oi]._oSelFlag = 2;
		RedoPlayerVision();
		return;
	}

	if (!deltaload)
		PlaySfxLoc(IS_DOORCLOS, object[oi]._ox, yp);
	if (((dDead[xp][yp] != 0 ? 0 : 1) & (dMonster[xp][yp] != 0 ? 0 : 1) & (dItem[xp][yp] != 0 ? 0 : 1)) != 0) {
		if (pnum == myplr && sendflag)
			NetSendCmdParam1(TRUE, CMD_CLOSEDOOR, oi);
		object[oi]._oVar4 = 0;
		object[oi]._oSelFlag = 3;
		ObjSetMicro(xp, yp, 531);
		object[oi]._oAnimFrame -= 2;
		object[oi]._oPreFlag = FALSE;
		RedoPlayerVision();
	} else {
		object[oi]._oVar4 = 2;
	}
}

void MonstCheckDoors(int m)
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

void ObjChangeMap(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			ObjSetMini(i, j, pdungeon[i][j]);
			dungeon[i][j] = pdungeon[i][j];
		}
	}
	if (leveltype == DTYPE_CATHEDRAL) {
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
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			ObjSetMini(i, j, pdungeon[i][j]);
			dungeon[i][j] = pdungeon[i][j];
		}
	}
	if (leveltype == DTYPE_CATHEDRAL) {
		ObjL1Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
	if (leveltype == DTYPE_CATACOMBS) {
		ObjL2Special(2 * x1 + 16, 2 * y1 + 16, 2 * x2 + 17, 2 * y2 + 17);
	}
}

void OperateL1Door(int pnum, int i, BOOL sendflag)
{
	int dpx, dpy;

	dpx = abs(object[i]._ox - plr[pnum]._px);
	dpy = abs(object[i]._oy - plr[pnum]._py);
	if (dpx == 1 && dpy <= 1 && object[i]._otype == OBJ_L1LDOOR)
		OperateL1LDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && object[i]._otype == OBJ_L1RDOOR)
		OperateL1RDoor(pnum, i, sendflag);
}

void OperateLever(int pnum, int i)
{
	int j, oi;
	BOOL mapflag;

	if (object[i]._oSelFlag != 0) {
		if (!deltaload)
			PlaySfxLoc(IS_LEVER, object[i]._ox, object[i]._oy);
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame++;
		mapflag = TRUE;
		if (currlevel == 16) {
			for (j = 0; j < nobjects; j++) {
				oi = objectactive[j];
				if (object[oi]._otype == OBJ_SWITCHSKL
				    && object[i]._oVar8 == object[oi]._oVar8
				    && object[oi]._oSelFlag != 0) {
					mapflag = FALSE;
				}
			}
		}
		if (mapflag)
			ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		if (pnum == myplr)
			NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
	}
}

void OperateBook(int pnum, int i)
{
	int j, oi;
	int dx, dy;
	int otype;
	BOOL do_add_missile, missile_added;

	if (object[i]._oSelFlag == 0)
		return;
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		do_add_missile = FALSE;
		missile_added = FALSE;
		for (j = 0; j < nobjects; j++) {
			oi = objectactive[j];
			otype = object[oi]._otype;
			if (otype == OBJ_MCIRCLE2 && object[oi]._oVar6 == 1) {
				dx = 27;
				dy = 29;
				object[oi]._oVar6 = 4;
				do_add_missile = TRUE;
			}
			if (otype == OBJ_MCIRCLE2 && object[oi]._oVar6 == 2) {
				dx = 43;
				dy = 29;
				object[oi]._oVar6 = 4;
				do_add_missile = TRUE;
			}
			if (do_add_missile) {
				object[dObject[35][36] - 1]._oVar5++;
				AddMissile(plr[pnum]._px, plr[pnum]._py, dx, dy, plr[pnum]._pdir, MIS_RNDTELEPORT, 0, pnum, 0, 0);
				missile_added = TRUE;
				do_add_missile = FALSE;
			}
		}
		if (!missile_added)
			return;
	}
	object[i]._oSelFlag = 0;
	object[i]._oAnimFrame++;
	if (!setlevel)
		return;

	if (setlvlnum == SL_BONECHAMB) {
		plr[myplr]._pMemSpells |= ((__int64)1 << (SPL_GUARDIAN - 1));
		if (plr[pnum]._pSplLvl[SPL_GUARDIAN] < 15)
			plr[myplr]._pSplLvl[SPL_GUARDIAN]++;
		quests[Q_SCHAMB]._qactive = QUEST_DONE;
		if (!deltaload)
			PlaySfxLoc(IS_QUESTDN, object[i]._ox, object[i]._oy);
		InitDiabloMsg(EMSG_BONECHAMB);
		AddMissile(
		    plr[myplr]._px,
		    plr[myplr]._py,
		    object[i]._ox - 2,
		    object[i]._oy - 4,
		    plr[myplr]._pdir,
		    MIS_GUARDIAN,
		    0,
		    myplr,
		    0,
		    0);
	}
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		ObjChangeMapResync(
		    object[i]._oVar1,
		    object[i]._oVar2,
		    object[i]._oVar3,
		    object[i]._oVar4);
		for (j = 0; j < nobjects; j++)
			SyncObjectAnim(objectactive[j]);
	}
}

void OperateBookLever(int pnum, int i)
{
	int x, y, tren;

	x = 2 * setpc_x + 16;
	y = 2 * setpc_y + 16;
	if (object[i]._oSelFlag != 0 && !qtextflag) {
		if (object[i]._otype == OBJ_BLINDBOOK && !quests[Q_BLIND]._qvar1) {
			quests[Q_BLIND]._qactive = QUEST_ACTIVE;
			quests[Q_BLIND]._qlog = 1;
			quests[Q_BLIND]._qvar1 = 1;
		}
		if (object[i]._otype == OBJ_BLOODBOOK && !quests[Q_BLOOD]._qvar1) {
			quests[Q_BLOOD]._qactive = QUEST_ACTIVE;
			quests[Q_BLOOD]._qlog = 1;
			quests[Q_BLOOD]._qvar1 = 1;
			SpawnQuestItem(IDI_BLDSTONE, 2 * setpc_x + 19, 2 * setpc_y + 26, 0, 1);
			SpawnQuestItem(IDI_BLDSTONE, 2 * setpc_x + 31, 2 * setpc_y + 26, 0, 1);
			SpawnQuestItem(IDI_BLDSTONE, 2 * setpc_x + 25, 2 * setpc_y + 33, 0, 1);
		}
		object[i]._otype = object[i]._otype;
		if (object[i]._otype == OBJ_STEELTOME && !quests[Q_WARLORD]._qvar1) {
			quests[Q_WARLORD]._qactive = QUEST_ACTIVE;
			quests[Q_WARLORD]._qlog = 1;
			quests[Q_WARLORD]._qvar1 = 1;
		}
		if (object[i]._oAnimFrame != object[i]._oVar6) {
			if (object[i]._otype != OBJ_BLOODBOOK)
				ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			if (object[i]._otype == OBJ_BLINDBOOK) {
				CreateItem(3, x + 5, y + 5);
				tren = TransVal;
				TransVal = 9;
				DRLG_MRectTrans(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
				TransVal = tren;
			}
		}
		object[i]._oAnimFrame = object[i]._oVar6;
		InitQTextMsg(object[i]._oVar7);
		if (pnum == myplr)
			NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
	}
}

void OperateSChambBk(int pnum, int i)
{
	int j, textdef;

	if (object[i]._oSelFlag != 0 && !qtextflag) {
		if (object[i]._oAnimFrame != object[i]._oVar6) {
			ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			for (j = 0; j < nobjects; j++)
				SyncObjectAnim(objectactive[j]);
		}
		object[i]._oAnimFrame = object[i]._oVar6;
		if (quests[Q_SCHAMB]._qactive == QUEST_INIT) {
			quests[Q_SCHAMB]._qactive = QUEST_ACTIVE;
			quests[Q_SCHAMB]._qlog = 1;
		}
		if (plr[myplr]._pClass == PC_WARRIOR) {
			textdef = TEXT_BONER;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			textdef = TEXT_RBONER;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			textdef = TEXT_MBONER;
		}
		quests[Q_SCHAMB]._qmsg = textdef;
		InitQTextMsg(textdef);
	}
}

void OperateChest(int pnum, int i, BOOL sendmsg)
{
	int j, mdir, mtype;

	if (object[i]._oSelFlag != 0) {
		if (!deltaload)
			PlaySfxLoc(IS_CHEST, object[i]._ox, object[i]._oy);
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame += 2;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			if (setlevel) {
				for (j = 0; j < object[i]._oVar1; j++) {
					CreateRndItem(object[i]._ox, object[i]._oy, TRUE, sendmsg, FALSE);
				}
			} else {
				for (j = 0; j < object[i]._oVar1; j++) {
					if (object[i]._oVar2)
						CreateRndItem(object[i]._ox, object[i]._oy, FALSE, sendmsg, FALSE);
					else
						CreateRndUseful(pnum, object[i]._ox, object[i]._oy, sendmsg);
				}
			}
			if (object[i]._oTrapFlag && object[i]._otype >= OBJ_TCHEST1 && object[i]._otype <= OBJ_TCHEST3) {
				mdir = GetDirection(object[i]._ox, object[i]._oy, plr[pnum]._px, plr[pnum]._py);
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
				}
				AddMissile(object[i]._ox, object[i]._oy, plr[pnum]._px, plr[pnum]._py, mdir, mtype, 1, -1, 0, 0);
				object[i]._oTrapFlag = FALSE;
			}
			if (pnum == myplr)
				NetSendCmdParam2(FALSE, CMD_PLROPOBJ, pnum, i);
			return;
		}
	}
}

void OperateMushPatch(int pnum, int i)
{
	int x, y;

	if (quests[Q_MUSHROOM]._qactive != QUEST_ACTIVE || quests[Q_MUSHROOM]._qvar1 < QS_TOMEGIVEN) {
		if (!deltaload && pnum == myplr) {
			if (plr[myplr]._pClass == PC_WARRIOR) {
				PlaySFX(PS_WARR13);
#ifndef SPAWN
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				PlaySFX(PS_ROGUE13);
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				PlaySFX(PS_MAGE13);
#endif
			}
		}
	} else {
		if (object[i]._oSelFlag != 0) {
			if (!deltaload)
				PlaySfxLoc(IS_CHEST, object[i]._ox, object[i]._oy);
			object[i]._oSelFlag = 0;
			object[i]._oAnimFrame++;
			if (!deltaload) {
				GetSuperItemLoc(object[i]._ox, object[i]._oy, &x, &y);
				SpawnQuestItem(IDI_MUSHROOM, x, y, 0, 0);
				quests[Q_MUSHROOM]._qvar1 = QS_MUSHSPAWNED;
			}
		}
	}
}

void OperateInnSignChest(int pnum, int i)
{
	int x, y;

	if (quests[Q_LTBANNER]._qvar1 != 2) {
		if (!deltaload && pnum == myplr) {
			if (plr[myplr]._pClass == PC_WARRIOR) {
				PlaySFX(PS_WARR24);
#ifndef SPAWN
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				PlaySFX(PS_ROGUE24);
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				PlaySFX(PS_MAGE24);
#endif
			}
		}
	} else {
		if (object[i]._oSelFlag != 0) {
			if (!deltaload)
				PlaySfxLoc(IS_CHEST, object[i]._ox, object[i]._oy);
			object[i]._oSelFlag = 0;
			object[i]._oAnimFrame += 2;
			if (!deltaload) {
				GetSuperItemLoc(object[i]._ox, object[i]._oy, &x, &y);
				SpawnQuestItem(IDI_BANNER, x, y, 0, 0);
			}
		}
	}
}

void OperateSlainHero(int pnum, int i, BOOL sendmsg)
{
	if (object[i]._oSelFlag != 0) {
		object[i]._oSelFlag = 0;
		if (!deltaload) {
			if (plr[pnum]._pClass == PC_WARRIOR) {
				CreateMagicArmor(object[i]._ox, object[i]._oy, ITYPE_HARMOR, ICURS_BREAST_PLATE, FALSE, TRUE);
#ifndef SPAWN
				PlaySfxLoc(PS_WARR9, plr[myplr]._px, plr[myplr]._py);
#endif
			} else if (plr[pnum]._pClass == PC_ROGUE) {
				CreateMagicWeapon(object[i]._ox, object[i]._oy, ITYPE_BOW, ICURS_LONG_WAR_BOW, FALSE, TRUE);
#ifndef SPAWN
				PlaySfxLoc(PS_ROGUE9, plr[myplr]._px, plr[myplr]._py);
#endif
			} else if (plr[pnum]._pClass == PC_SORCERER) {
				CreateSpellBook(object[i]._ox, object[i]._oy, SPL_LIGHTNING, FALSE, TRUE);
#ifndef SPAWN
				PlaySfxLoc(PS_MAGE9, plr[myplr]._px, plr[myplr]._py);
#endif
			}
			if (pnum == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
		}
	}
}

void OperateTrapLvr(int i)
{
	int frame, j, oi;

	frame = object[i]._oAnimFrame;
	j = 0;

	if (frame == 1) {
		object[i]._oAnimFrame = 2;
		for (; j < nobjects; j++) {
			oi = objectactive[j];
			if (object[oi]._otype == object[i]._oVar2 && object[oi]._oVar1 == object[i]._oVar1) {
				object[oi]._oVar2 = 1;
				object[oi]._oAnimFlag = 0;
			}
		}
		return;
	}

	object[i]._oAnimFrame = frame - 1;
	for (; j < nobjects; j++) {
		oi = objectactive[j];
		if (object[oi]._otype == object[i]._oVar2 && object[oi]._oVar1 == object[i]._oVar1) {
			object[oi]._oVar2 = 0;
			if (object[oi]._oVar4 != 0)
				object[oi]._oAnimFlag = 1;
		}
	}
}

void OperateSarc(int pnum, int i, BOOL sendmsg)
{
	if (object[i]._oSelFlag != 0) {
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
				CreateRndItem(object[i]._ox, object[i]._oy, FALSE, sendmsg, FALSE);
			if (object[i]._oVar1 >= 8)
				SpawnSkeleton(object[i]._oVar2, object[i]._ox, object[i]._oy);
			if (pnum == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
		}
	}
}

void OperateL2Door(int pnum, int i, BOOL sendflag)
{
	int dpx, dpy;

	dpx = abs(object[i]._ox - plr[pnum]._px);
	dpy = abs(object[i]._oy - plr[pnum]._py);
	if (dpx == 1 && dpy <= 1 && object[i]._otype == OBJ_L2LDOOR)
		OperateL2LDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && object[i]._otype == OBJ_L2RDOOR)
		OperateL2RDoor(pnum, i, sendflag);
}

void OperateL3Door(int pnum, int i, BOOL sendflag)
{
	int dpx, dpy;

	dpx = abs(object[i]._ox - plr[pnum]._px);
	dpy = abs(object[i]._oy - plr[pnum]._py);
	if (dpx == 1 && dpy <= 1 && object[i]._otype == OBJ_L3RDOOR)
		OperateL3RDoor(pnum, i, sendflag);
	if (dpx <= 1 && dpy == 1 && object[i]._otype == OBJ_L3LDOOR)
		OperateL3LDoor(pnum, i, sendflag);
}

void OperatePedistal(int pnum, int i)
{
	BYTE *mem;
	int iv;

	if (object[i]._oVar6 != 3) {
		if (PlrHasItem(pnum, IDI_BLDSTONE, &iv) != NULL) {
			RemoveInvItem(pnum, iv);
			object[i]._oAnimFrame++;
			object[i]._oVar6++;
		}
		if (object[i]._oVar6 == 1) {
			if (!deltaload)
				PlaySfxLoc(LS_PUDDLE, object[i]._ox, object[i]._oy);
			ObjChangeMap(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
		}
		if (object[i]._oVar6 == 2) {
			if (!deltaload)
				PlaySfxLoc(LS_PUDDLE, object[i]._ox, object[i]._oy);
			ObjChangeMap(setpc_x + 6, setpc_y + 3, setpc_x + setpc_w, setpc_y + 7);
		}
		if (object[i]._oVar6 == 3) {
			if (!deltaload)
				PlaySfxLoc(LS_BLODSTAR, object[i]._ox, object[i]._oy);
			ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			mem = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
			LoadMapObjs(mem, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(mem);
			CreateItem(7, 2 * setpc_x + 25, 2 * setpc_y + 19);
			object[i]._oSelFlag = 0;
		}
	}
}

void TryDisarm(int pnum, int i)
{
	int j, oi, oti, trapdisper;
	BOOL checkflag;

	if (pnum == myplr)
		SetCursor_(CURSOR_HAND);
	if (object[i]._oTrapFlag) {
		trapdisper = 2 * plr[pnum]._pDexterity - 5 * currlevel;
		if (random_(154, 100) <= trapdisper) {
			for (j = 0; j < nobjects; j++) {
				checkflag = FALSE;
				oi = objectactive[j];
				oti = object[oi]._otype;
				if (oti == OBJ_TRAPL)
					checkflag = TRUE;
				if (oti == OBJ_TRAPR)
					checkflag = TRUE;
				if (checkflag && dObject[object[oi]._oVar1][object[oi]._oVar2] - 1 == i) {
					object[oi]._oVar4 = 1;
					object[i]._oTrapFlag = FALSE;
				}
			}
			oti = object[i]._otype;
			if (oti >= OBJ_TCHEST1 && oti <= OBJ_TCHEST3)
				object[i]._oTrapFlag = FALSE;
		}
	}
}

int ItemMiscIdIdx(int imiscid)
{
	int i;

	i = 0;
	while (AllItemsList[i].iRnd == 0 || AllItemsList[i].iMiscId != imiscid) {
		i++;
	}

	return i;
}

void OperateShrine(int pnum, int i, int sType)
{
	int cnt;
	int r, j;
	DWORD lv, t;
	int xx, yy;
	int v1, v2, v3, v4;
	unsigned __int64 spell, spells;

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	assert((DWORD)i < MAXOBJECTS);

	if (object[i]._oSelFlag == 0)
		return;

	SetRndSeed(object[i]._oRndSeed);
	object[i]._oSelFlag = 0;

	if (!deltaload) {
		PlaySfxLoc(sType, object[i]._ox, object[i]._oy);
		object[i]._oAnimFlag = 1;
		object[i]._oAnimDelay = 1;
	} else {
		object[i]._oAnimFlag = 0;
		object[i]._oAnimFrame = object[i]._oAnimLen;
	}
	switch (object[i]._oVar1) {
	case SHRINE_MYSTERIOUS:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		ModifyPlrStr(pnum, -1);
		ModifyPlrMag(pnum, -1);
		ModifyPlrDex(pnum, -1);
		ModifyPlrVit(pnum, -1);
		switch (random_(0, 4)) {
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
		cnt = 0;
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		for (j = 0; j < NUM_INVLOC; j++) {
			if (plr[pnum].InvBody[j]._itype != ITYPE_NONE)
				cnt++;
		}
		if (cnt > 0) {
			for (j = 0; j < NUM_INVLOC; j++) {
				if (plr[pnum].InvBody[j]._itype != ITYPE_NONE
				    && plr[pnum].InvBody[j]._iMaxDur != DUR_INDESTRUCTIBLE
				    && plr[pnum].InvBody[j]._iMaxDur != 0) {
					plr[pnum].InvBody[j]._iDurability += 10;
					plr[pnum].InvBody[j]._iMaxDur += 10;
					if (plr[pnum].InvBody[j]._iDurability > plr[pnum].InvBody[j]._iMaxDur)
						plr[pnum].InvBody[j]._iDurability = plr[pnum].InvBody[j]._iMaxDur;
				}
			}
			while (TRUE) {
				cnt = 0;
				for (j = 0; j < NUM_INVLOC; j++) {
					if (plr[pnum].InvBody[j]._itype != ITYPE_NONE
					    && plr[pnum].InvBody[j]._iMaxDur != DUR_INDESTRUCTIBLE
					    && plr[pnum].InvBody[j]._iMaxDur != 0)
						cnt++;
				}
				if (cnt == 0)
					break;
				r = random_(0, NUM_INVLOC);
				if (plr[pnum].InvBody[r]._itype == ITYPE_NONE || plr[pnum].InvBody[r]._iMaxDur == DUR_INDESTRUCTIBLE || plr[pnum].InvBody[r]._iMaxDur == 0)
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
		break;
	case SHRINE_GLOOMY:
		if (deltaload)
			return;
		if (pnum != myplr)
			break;
		if (plr[pnum].InvBody[INVLOC_HEAD]._itype != ITYPE_NONE)
			plr[pnum].InvBody[INVLOC_HEAD]._iAC += 2;
		if (plr[pnum].InvBody[INVLOC_CHEST]._itype != ITYPE_NONE)
			plr[pnum].InvBody[INVLOC_CHEST]._iAC += 2;
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE) {
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
				plr[pnum].InvBody[INVLOC_HAND_LEFT]._iAC += 2;
			} else {
				plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam--;
				if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam < plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMinDam)
					plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam = plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMinDam;
			}
		}
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE) {
			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
				plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iAC += 2;
			} else {
				plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam--;
				if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam < plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMinDam)
					plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam = plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMinDam;
			}
		}
		for (j = 0; j < plr[pnum]._pNumInv; j++) {
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
			case ITYPE_LARMOR:
			case ITYPE_HELM:
			case ITYPE_MARMOR:
			case ITYPE_HARMOR:
				plr[pnum].InvList[j]._iAC += 2;
				break;
			}
		}
		InitDiabloMsg(EMSG_SHRINE_GLOOMY);
		break;
	case SHRINE_WEIRD:
		if (deltaload)
			return;
		if (pnum != myplr)
			break;

		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_SHIELD)
			plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMaxDam++;
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_SHIELD)
			plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iMaxDam++;
		for (j = 0; j < plr[pnum]._pNumInv; j++) {
			switch (plr[pnum].InvList[j]._itype) {
			case ITYPE_SWORD:
			case ITYPE_AXE:
			case ITYPE_BOW:
			case ITYPE_MACE:
			case ITYPE_STAFF:
				plr[pnum].InvList[j]._iMaxDam++;
				break;
			}
		}
		InitDiabloMsg(EMSG_SHRINE_WEIRD);
		break;
	case SHRINE_MAGICAL:

	case SHRINE_MAGICAL2:
		if (deltaload)
			return;
		AddMissile(
		    plr[pnum]._px,
		    plr[pnum]._py,
		    plr[pnum]._px,
		    plr[pnum]._py,
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
		if (pnum != myplr)
			break;

		for (j = 0; j < NUM_INVLOC; j++) {
			if (plr[pnum].InvBody[j]._itype == ITYPE_STAFF)
				plr[pnum].InvBody[j]._iCharges = plr[pnum].InvBody[j]._iMaxCharges;
		}
		for (j = 0; j < plr[pnum]._pNumInv; j++) {
			if (plr[pnum].InvList[j]._itype == ITYPE_STAFF)
				plr[pnum].InvList[j]._iCharges = plr[pnum].InvList[j]._iMaxCharges;
		}
		for (j = 0; j < MAXBELTITEMS; j++) {
			if (plr[pnum].SpdList[j]._itype == ITYPE_STAFF)
				plr[pnum].SpdList[j]._iCharges = plr[pnum].SpdList[j]._iMaxCharges; // belt items don't have charges?
		}
		InitDiabloMsg(EMSG_SHRINE_STONE);
		break;
	case SHRINE_RELIGIOUS:
		if (deltaload)
			return;
		if (pnum != myplr)
			break;

		for (j = 0; j < NUM_INVLOC; j++)
			plr[pnum].InvBody[j]._iDurability = plr[pnum].InvBody[j]._iMaxDur;
		for (j = 0; j < plr[pnum]._pNumInv; j++)
			plr[pnum].InvList[j]._iDurability = plr[pnum].InvList[j]._iMaxDur;
		for (j = 0; j < MAXBELTITEMS; j++)
			plr[pnum].SpdList[j]._iDurability = plr[pnum].SpdList[j]._iMaxDur; // belt items don't have durability?
		InitDiabloMsg(EMSG_SHRINE_RELIGIOUS);
		break;
	case SHRINE_ENCHANTED:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		cnt = 0;
		spell = 1;
		spells = plr[pnum]._pMemSpells;
		for (j = 0; j < MAX_SPELLS; j++) {
			if (spell & spells)
				cnt++;
			spell <<= 1;
		}
		if (cnt > 1) {
			spell = 1;
			for (j = 1; j <= MAX_SPELLS; j++) {
				if (plr[pnum]._pMemSpells & spell) {
					if (plr[pnum]._pSplLvl[j] < 15)
						plr[pnum]._pSplLvl[j]++;
				}
				spell <<= 1;
			}
			do {
				r = random_(0, 37);
			} while (!(plr[pnum]._pMemSpells & ((__int64)1 << r)));
			if (plr[pnum]._pSplLvl[r] >= 2)
				plr[pnum]._pSplLvl[r] -= 2;
			else
				plr[pnum]._pSplLvl[r] = 0;
		}
		InitDiabloMsg(EMSG_SHRINE_ENCHANTED);
		break;
	case SHRINE_THAUMATURGIC:
		for (j = 0; j < nobjects; j++) {
			v1 = objectactive[j];
			assert((DWORD)v1 < MAXOBJECTS);
			if ((object[v1]._otype == OBJ_CHEST1
			        || object[v1]._otype == OBJ_CHEST2
			        || object[v1]._otype == OBJ_CHEST3)
			    && object[v1]._oSelFlag == 0) {
				object[v1]._oRndSeed = GetRndSeed();
				object[v1]._oAnimFrame -= 2;
				object[v1]._oSelFlag = 1;
			}
		}
		if (deltaload)
			return;
		if (pnum == myplr)
			InitDiabloMsg(EMSG_SHRINE_THAUMATURGIC);
		break;
	case SHRINE_FASCINATING:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		plr[pnum]._pMemSpells |= (__int64)1 << (SPL_FIREBOLT - 1);
		if (plr[pnum]._pSplLvl[SPL_FIREBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_FIREBOLT]++;
		if (plr[pnum]._pSplLvl[SPL_FIREBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_FIREBOLT]++;
		t = plr[pnum]._pMaxManaBase / 10;
		v1 = plr[pnum]._pMana - plr[pnum]._pManaBase;
		v2 = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;

		plr[pnum]._pManaBase -= t;
		plr[pnum]._pMana -= t;
		plr[pnum]._pMaxMana -= t;
		plr[pnum]._pMaxManaBase -= t;
		if (plr[pnum]._pMana >> 6 <= 0) {
			plr[pnum]._pManaBase = 0;
			plr[pnum]._pMana = v1;
		}
		if (plr[pnum]._pMaxMana >> 6 <= 0) {
			plr[pnum]._pMaxManaBase = 0;
			plr[pnum]._pMaxMana = v2;
		}
		InitDiabloMsg(EMSG_SHRINE_FASCINATING);
		break;
	case SHRINE_CRYPTIC:
		if (deltaload)
			return;
		AddMissile(
		    plr[pnum]._px,
		    plr[pnum]._py,
		    plr[pnum]._px,
		    plr[pnum]._py,
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
		if (pnum != myplr)
			break;
		for (j = 0; j < plr[pnum]._pNumInv; j++) {
			if (plr[pnum].InvList[j]._itype == ITYPE_MISC) {
				if (plr[pnum].InvList[j]._iMiscId == IMISC_HEAL
				    || plr[pnum].InvList[j]._iMiscId == IMISC_MANA) {
					SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_REJUV));
					GetPlrHandSeed(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._iStatFlag = TRUE;
					plr[pnum].InvList[j] = plr[pnum].HoldItem;
				}
				if (plr[pnum].InvList[j]._iMiscId == IMISC_FULLHEAL
				    || plr[pnum].InvList[j]._iMiscId == IMISC_FULLMANA) {
					SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_FULLREJUV));
					GetPlrHandSeed(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._iStatFlag = TRUE;
					plr[pnum].InvList[j] = plr[pnum].HoldItem;
				}
			}
		}
		for (j = 0; j < MAXBELTITEMS; j++) {
			if (plr[pnum].SpdList[j]._itype == ITYPE_MISC) {
				if (plr[pnum].SpdList[j]._iMiscId == IMISC_HEAL
				    || plr[pnum].SpdList[j]._iMiscId == IMISC_MANA) {
					SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_REJUV));
					GetPlrHandSeed(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._iStatFlag = TRUE;
					plr[pnum].SpdList[j] = plr[pnum].HoldItem;
				}
				if (plr[pnum].SpdList[j]._iMiscId == IMISC_FULLHEAL
				    || plr[pnum].SpdList[j]._iMiscId == IMISC_FULLMANA) {
					SetPlrHandItem(&plr[pnum].HoldItem, ItemMiscIdIdx(IMISC_FULLREJUV));
					GetPlrHandSeed(&plr[pnum].HoldItem);
					plr[pnum].HoldItem._iStatFlag = TRUE;
					plr[pnum].SpdList[j] = plr[pnum].HoldItem;
				}
			}
		}
		InitDiabloMsg(EMSG_SHRINE_ELDRITCH);
		break;
	case SHRINE_EERIE:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		ModifyPlrMag(pnum, 2);
		CheckStats(pnum);
		InitDiabloMsg(EMSG_SHRINE_EERIE);
		break;
	case SHRINE_DIVINE:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		if (2 * currlevel < 7) {
			CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, ITYPE_MISC, IMISC_FULLMANA, FALSE, TRUE);
			CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, ITYPE_MISC, IMISC_FULLHEAL, FALSE, TRUE);
		} else {
			CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, ITYPE_MISC, IMISC_FULLREJUV, FALSE, TRUE);
			CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, ITYPE_MISC, IMISC_FULLREJUV, FALSE, TRUE);
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
		j = 0;
		do {
			xx = random_(159, MAXDUNX);
			yy = random_(159, MAXDUNY);
			j++;
			if (j > MAXDUNX * 112)
				break;
			lv = dPiece[xx][yy];
		} while (nSolidTable[lv] || dObject[xx][yy] || dMonster[xx][yy]);
		AddMissile(plr[pnum]._px, plr[pnum]._py, xx, yy, plr[pnum]._pdir, MIS_RNDTELEPORT, -1, pnum, 0, 2 * leveltype);
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
		t = plr[pnum]._pMaxManaBase / 10;
		v1 = plr[pnum]._pMana - plr[pnum]._pManaBase;
		v2 = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
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
		break;
	case SHRINE_SPIRITUAL:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		for (j = 0; j < NUM_INV_GRID_ELEM; j++) {
			if (!plr[pnum].InvGrid[j]) {
				r = 5 * leveltype + random_(160, 10 * leveltype);
				t = plr[pnum]._pNumInv; // check
				plr[pnum].InvList[t] = golditem;
				plr[pnum].InvList[t]._iSeed = GetRndSeed();
				plr[pnum]._pNumInv++;
				plr[pnum].InvGrid[j] = plr[pnum]._pNumInv;
				plr[pnum].InvList[t]._ivalue = r;
				plr[pnum]._pGold += r;
				SetGoldCurs(pnum, t);
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
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		ModifyPlrDex(pnum, 2);
		CheckStats(pnum);
		if (pnum == myplr)
			InitDiabloMsg(EMSG_SHRINE_ABANDONED);
		break;
	case SHRINE_CREEPY:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		ModifyPlrStr(pnum, 2);
		CheckStats(pnum);
		if (pnum == myplr)
			InitDiabloMsg(EMSG_SHRINE_CREEPY);
		break;
	case SHRINE_QUIET:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		ModifyPlrVit(pnum, 2);
		CheckStats(pnum);
		if (pnum == myplr)
			InitDiabloMsg(EMSG_SHRINE_QUIET);
		break;
	case SHRINE_SECLUDED:
		if (deltaload)
			return;
		if (pnum != myplr)
			break;

		for (yy = 0; yy < DMAXY; yy++) {
			for (xx = 0; xx < DMAXX; xx++) {
				automapview[xx][yy] = TRUE;
			}
		}
		InitDiabloMsg(EMSG_SHRINE_SECLUDED);
		break;
	case SHRINE_ORNATE:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		plr[pnum]._pMemSpells |= (__int64)1 << (SPL_HBOLT - 1);
		if (plr[pnum]._pSplLvl[SPL_HBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_HBOLT]++;
		if (plr[pnum]._pSplLvl[SPL_HBOLT] < 15)
			plr[pnum]._pSplLvl[SPL_HBOLT]++;
		t = plr[pnum]._pMaxManaBase / 10;
		v1 = plr[pnum]._pMana - plr[pnum]._pManaBase;
		v2 = plr[pnum]._pMaxMana - plr[pnum]._pMaxManaBase;
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
		break;
	case SHRINE_GLIMMERING:
		if (deltaload)
			return;
		if (pnum != myplr)
			return;
		for (j = 0; j < NUM_INVLOC; j++) {
			if (plr[pnum].InvBody[j]._iMagical && !plr[pnum].InvBody[j]._iIdentified)
				plr[pnum].InvBody[j]._iIdentified = TRUE;
		}
		for (j = 0; j < plr[pnum]._pNumInv; j++) {
			if (plr[pnum].InvList[j]._iMagical && !plr[pnum].InvList[j]._iIdentified)
				plr[pnum].InvList[j]._iIdentified = TRUE;
		}
		for (j = 0; j < 8; j++) {
			if (plr[pnum].SpdList[j]._iMagical && !plr[pnum].SpdList[j]._iIdentified)
				plr[pnum].SpdList[j]._iIdentified = TRUE; // belt items can't be magical?
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
			r = random_(155, 4);

			if (r != 0)
				v1 = -1;
			else
				v1 = 1;
			if (r != 1)
				v2 = -1;
			else
				v2 = 1;
			if (r == 2)
				v3 = 1;
			else
				v3 = -1;
			if (r == 3)
				v4 = 1;
			else
				v4 = -1;

			ModifyPlrStr(myplr, v1);
			ModifyPlrMag(myplr, v2);
			ModifyPlrDex(myplr, v3);
			ModifyPlrVit(myplr, v4);

			CheckStats(myplr);
		}
		break;
	}

	CalcPlrInv(pnum, TRUE);
	force_redraw = 255;

	if (pnum == myplr)
		NetSendCmdParam2(FALSE, CMD_PLROPOBJ, pnum, i);
}

void OperateSkelBook(int pnum, int i, BOOL sendmsg)
{
	if (object[i]._oSelFlag != 0) {
		if (!deltaload)
			PlaySfxLoc(IS_ISCROL, object[i]._ox, object[i]._oy);
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame += 2;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			if (random_(161, 5))
				CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, ITYPE_MISC, IMISC_SCROLL, sendmsg, FALSE);
			else
				CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, ITYPE_MISC, IMISC_BOOK, sendmsg, FALSE);
			if (pnum == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
		}
	}
}

void OperateBookCase(int pnum, int i, BOOL sendmsg)
{
	if (object[i]._oSelFlag != 0) {
		if (!deltaload)
			PlaySfxLoc(IS_ISCROL, object[i]._ox, object[i]._oy);
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame -= 2;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, ITYPE_MISC, IMISC_BOOK, sendmsg, FALSE);
			if (QuestStatus(Q_ZHAR)
			    && monster[MAX_PLRS].mName == UniqMonst[UMT_ZHAR].mName
			    && monster[MAX_PLRS]._msquelch == UCHAR_MAX
			    && monster[MAX_PLRS]._mhitpoints) {
				monster[MAX_PLRS].mtalkmsg = TEXT_ZHAR2;
				M_StartStand(0, monster[MAX_PLRS]._mdir);
				monster[MAX_PLRS]._mgoal = MGOAL_SHOOT;
				monster[MAX_PLRS]._mmode = MM_TALK;
			}
			if (pnum == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
		}
	}
}

void OperateDecap(int pnum, int i, BOOL sendmsg)
{
	if (object[i]._oSelFlag != 0) {
		object[i]._oSelFlag = 0;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			CreateRndItem(object[i]._ox, object[i]._oy, FALSE, sendmsg, FALSE);
			if (pnum == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
		}
	}
}

void OperateArmorStand(int pnum, int i, BOOL sendmsg)
{
	BOOL uniqueRnd;

	if (object[i]._oSelFlag != 0) {
		object[i]._oSelFlag = 0;
		object[i]._oAnimFrame++;
		if (!deltaload) {
			SetRndSeed(object[i]._oRndSeed);
			uniqueRnd = random_(0, 2);
			if (currlevel <= 5) {
				CreateTypeItem(object[i]._ox, object[i]._oy, TRUE, ITYPE_LARMOR, IMISC_NONE, sendmsg, FALSE);
			} else if (currlevel >= 6 && currlevel <= 9) {
				CreateTypeItem(object[i]._ox, object[i]._oy, uniqueRnd, ITYPE_MARMOR, IMISC_NONE, sendmsg, FALSE);
			} else if (currlevel >= 10 && currlevel <= 12) {
				CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, ITYPE_HARMOR, IMISC_NONE, sendmsg, FALSE);
			} else if (currlevel >= 13 && currlevel <= 16) {
				CreateTypeItem(object[i]._ox, object[i]._oy, TRUE, ITYPE_HARMOR, IMISC_NONE, sendmsg, FALSE);
			}
			if (pnum == myplr)
				NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
			return;
		}
	}
}

int FindValidShrine(int i)
{
	BOOL done;
	int rv;

	while (1) {
		done = FALSE;
		while (!done) {
			rv = random_(0, NUM_SHRINETYPE);
			if (currlevel >= shrinemin[rv] && currlevel <= shrinemax[rv] && rv != 8)
				done = TRUE;
		}

		if (gbMaxPlayers != 1) {
			if (shrineavail[rv] != 1) {
				break;
			}
		} else {
			if (shrineavail[rv] != 2) {
				break;
			}
		}
	}

	return rv;
}

void OperateGoatShrine(int pnum, int i, int sType)
{
	SetRndSeed(object[i]._oRndSeed);
	object[i]._oVar1 = FindValidShrine(i);
	OperateShrine(pnum, i, sType);
	object[i]._oAnimDelay = 2;
	force_redraw = 255;
}

void OperateCauldron(int pnum, int i, int sType)
{
	SetRndSeed(object[i]._oRndSeed);
	object[i]._oVar1 = FindValidShrine(i);
	OperateShrine(pnum, i, sType);
	object[i]._oAnimFrame = 3;
	object[i]._oAnimFlag = 0;
	force_redraw = 255;
}

BOOL OperateFountains(int pnum, int i)
{
	int prev, add, rnd, cnt;
	BOOL applied, done;

	applied = FALSE;
	SetRndSeed(object[i]._oRndSeed);
	switch (object[i]._otype) {
	case OBJ_BLOODFTN:
		if (deltaload)
			return FALSE;
		if (pnum != myplr)
			return FALSE;

		if (plr[pnum]._pHitPoints < plr[pnum]._pMaxHP) {
			PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
			plr[pnum]._pHitPoints += 64;
			plr[pnum]._pHPBase += 64;
			if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
				plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
				plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
			}
			applied = TRUE;
		} else
			PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
		break;
	case OBJ_PURIFYINGFTN:
		if (deltaload)
			return FALSE;
		if (pnum != myplr)
			return FALSE;

		if (plr[pnum]._pMana < plr[pnum]._pMaxMana) {
			PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);

			plr[pnum]._pMana += 64;
			plr[pnum]._pManaBase += 64;
			if (plr[pnum]._pMana > plr[pnum]._pMaxMana) {
				plr[pnum]._pMana = plr[pnum]._pMaxMana;
				plr[pnum]._pManaBase = plr[pnum]._pMaxManaBase;
			}

			applied = TRUE;
		} else
			PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
		break;
	case OBJ_MURKYFTN:
		if (object[i]._oSelFlag == 0)
			break;
		if (!deltaload)
			PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
		object[i]._oSelFlag = 0;
		if (deltaload)
			return FALSE;
		AddMissile(
		    plr[pnum]._px,
		    plr[pnum]._py,
		    plr[pnum]._px,
		    plr[pnum]._py,
		    plr[pnum]._pdir,
		    MIS_INFRA,
		    -1,
		    pnum,
		    0,
		    2 * leveltype);
		applied = TRUE;
		if (pnum == myplr)
			NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
		break;
	case OBJ_TEARFTN:
		if (object[i]._oSelFlag == 0)
			break;
		prev = -1;
		add = -1;
		done = FALSE;
		cnt = 0;
		if (!deltaload)
			PlaySfxLoc(LS_FOUNTAIN, object[i]._ox, object[i]._oy);
		object[i]._oSelFlag = 0;
		if (deltaload)
			return FALSE;
		if (pnum != myplr)
			return FALSE;
		while (!done) {
			rnd = random_(0, 4);
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
			if (cnt <= 1)
				continue;

			done = TRUE;
		}
		CheckStats(pnum);
		applied = TRUE;
		if (pnum == myplr)
			NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
		break;
	}
	force_redraw = 255;
	return applied;
}

void OperateWeaponRack(int pnum, int i, BOOL sendmsg)
{
	int weaponType;

	if (object[i]._oSelFlag == 0)
		return;
	SetRndSeed(object[i]._oRndSeed);

	switch (random_(0, 4) + 1) {
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

	if (leveltype > 1)
		CreateTypeItem(object[i]._ox, object[i]._oy, TRUE, weaponType, IMISC_NONE, sendmsg, FALSE);
	else
		CreateTypeItem(object[i]._ox, object[i]._oy, FALSE, weaponType, IMISC_NONE, sendmsg, FALSE);
	if (pnum == myplr)
		NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
}

void OperateStoryBook(int pnum, int i)
{
	if (object[i]._oSelFlag != 0 && !deltaload && !qtextflag && pnum == myplr) {
		object[i]._oAnimFrame = object[i]._oVar4;
		PlaySfxLoc(IS_ISCROL, object[i]._ox, object[i]._oy);
		InitQTextMsg(object[i]._oVar2);
		NetSendCmdParam1(FALSE, CMD_OPERATEOBJ, i);
	}
}

void OperateLazStand(int pnum, int i)
{
	int xx, yy;

	if (object[i]._oSelFlag != 0 && !deltaload && !qtextflag && pnum == myplr) {
		object[i]._oAnimFrame++;
		object[i]._oSelFlag = 0;
		GetSuperItemLoc(object[i]._ox, object[i]._oy, &xx, &yy);
		SpawnQuestItem(IDI_LAZSTAFF, xx, yy, 0, 0);
	}
}

void OperateObject(int pnum, int i, BOOL TeleFlag)
{
	BOOL sendmsg;

	sendmsg = (pnum == myplr);
	switch (object[i]._otype) {
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
		if (TeleFlag) {
			if (object[i]._otype == OBJ_L1LDOOR)
				OperateL1LDoor(pnum, i, TRUE);
			if (object[i]._otype == OBJ_L1RDOOR)
				OperateL1RDoor(pnum, i, TRUE);
			break;
		}
		if (pnum == myplr)
			OperateL1Door(pnum, i, TRUE);
		break;
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
		if (TeleFlag) {
			if (object[i]._otype == OBJ_L2LDOOR)
				OperateL2LDoor(pnum, i, TRUE);
			if (object[i]._otype == OBJ_L2RDOOR)
				OperateL2RDoor(pnum, i, TRUE);
			break;
		}
		if (pnum == myplr)
			OperateL2Door(pnum, i, TRUE);
		break;
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (TeleFlag) {
			if (object[i]._otype == OBJ_L3LDOOR)
				OperateL3LDoor(pnum, i, TRUE);
			if (object[i]._otype == OBJ_L3RDOOR)
				OperateL3RDoor(pnum, i, TRUE);
			break;
		}
		if (pnum == myplr)
			OperateL3Door(pnum, i, TRUE);
		break;
	case OBJ_LEVER:
	case OBJ_SWITCHSKL:
		OperateLever(pnum, i);
		break;
	case OBJ_BOOK2L:
		OperateBook(pnum, i);
		break;
	case OBJ_BOOK2R:
		OperateSChambBk(pnum, i);
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
		OperateSlainHero(pnum, i, sendmsg);
		break;
	case OBJ_SIGNCHEST:
		OperateInnSignChest(pnum, i);
		break;
	}
}

void SyncOpL1Door(int pnum, int cmd, int i)
{
	BOOL do_sync;

	if (pnum == myplr)
		return;

	do_sync = FALSE;
	if (cmd == CMD_OPENDOOR && object[i]._oVar4 == 0) {
		do_sync = TRUE;
	}
	if (cmd == CMD_CLOSEDOOR && object[i]._oVar4 == 1)
		do_sync = TRUE;
	if (do_sync) {
		if (object[i]._otype == OBJ_L1LDOOR)
			OperateL1LDoor(-1, i, FALSE);
		if (object[i]._otype == OBJ_L1RDOOR)
			OperateL1RDoor(-1, i, FALSE);
	}
}

void SyncOpL2Door(int pnum, int cmd, int i)
{
	BOOL do_sync;

	if (pnum == myplr)
		return;

	do_sync = FALSE;
	if (cmd == CMD_OPENDOOR && object[i]._oVar4 == 0) {
		do_sync = TRUE;
	}
	if (cmd == CMD_CLOSEDOOR && object[i]._oVar4 == 1)
		do_sync = TRUE;
	if (do_sync) {
		if (object[i]._otype == OBJ_L2LDOOR)
			OperateL2LDoor(-1, i, FALSE);
		if (object[i]._otype == OBJ_L2RDOOR)
			OperateL2RDoor(-1, i, FALSE);
	}
}

void SyncOpL3Door(int pnum, int cmd, int i)
{
	BOOL do_sync;

	if (pnum == myplr)
		return;

	do_sync = FALSE;
	if (cmd == CMD_OPENDOOR && object[i]._oVar4 == 0) {
		do_sync = TRUE;
	}
	if (cmd == CMD_CLOSEDOOR && object[i]._oVar4 == 1)
		do_sync = TRUE;
	if (do_sync) {
		if (object[i]._otype == OBJ_L3LDOOR)
			OperateL2LDoor(-1, i, FALSE);
		if (object[i]._otype == OBJ_L3RDOOR)
			OperateL2RDoor(-1, i, FALSE);
	}
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
		OperateChest(pnum, i, FALSE);
		break;
	case OBJ_SARC:
		OperateSarc(pnum, i, FALSE);
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
		OperateSkelBook(pnum, i, FALSE);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		OperateBookCase(pnum, i, FALSE);
		break;
	case OBJ_DECAP:
		OperateDecap(pnum, i, FALSE);
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR:
		OperateArmorStand(pnum, i, FALSE);
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
		OperateWeaponRack(pnum, i, FALSE);
		break;
	case OBJ_MUSHPATCH:
		OperateMushPatch(pnum, i);
		break;
	case OBJ_SLAINHERO:
		OperateSlainHero(pnum, i, FALSE);
		break;
	case OBJ_SIGNCHEST:
		OperateInnSignChest(pnum, i);
		break;
	}
}

void BreakCrux(int i)
{
	int j, oi;
	BOOL triggered;

	object[i]._oAnimFlag = 1;
	object[i]._oAnimFrame = 1;
	object[i]._oAnimDelay = 1;
	object[i]._oSolidFlag = TRUE;
	object[i]._oMissFlag = TRUE;
	object[i]._oBreak = -1;
	object[i]._oSelFlag = 0;
	triggered = TRUE;
	for (j = 0; j < nobjects; j++) {
		oi = objectactive[j];
		if (object[oi]._otype != OBJ_CRUX1 && object[oi]._otype != OBJ_CRUX2 && object[oi]._otype != OBJ_CRUX3)
			continue;
		if (object[i]._oVar8 != object[oi]._oVar8 || object[oi]._oBreak == -1)
			continue;
		triggered = FALSE;
	}
	if (!triggered)
		return;
	if (!deltaload)
		PlaySfxLoc(IS_LEVER, object[i]._ox, object[i]._oy);
	ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
}

void BreakBarrel(int pnum, int i, int dam, BOOL forcebreak, BOOL sendmsg)
{
	int oi;
	int xp, yp;

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

		PlaySfxLoc(IS_IBOW, object[i]._ox, object[i]._oy);
		return;
	}

	object[i]._oBreak = -1;
	object[i]._oVar1 = 0;
	object[i]._oAnimFlag = 1;
	object[i]._oAnimFrame = 1;
	object[i]._oAnimDelay = 1;
	object[i]._oSolidFlag = FALSE;
	object[i]._oMissFlag = TRUE;
	object[i]._oSelFlag = 0;
	object[i]._oPreFlag = TRUE;
	if (deltaload) {
		object[i]._oAnimCnt = 0;
		object[i]._oAnimFrame = object[i]._oAnimLen;
		object[i]._oAnimDelay = 1000;
		return;
	}

	if (object[i]._otype == OBJ_BARRELEX) {
		PlaySfxLoc(IS_BARLFIRE, object[i]._ox, object[i]._oy);
		for (yp = object[i]._oy - 1; yp <= object[i]._oy + 1; yp++) {
			for (xp = object[i]._ox - 1; xp <= object[i]._ox + 1; xp++) {
				if (dMonster[xp][yp] > 0)
					MonsterTrapHit(dMonster[xp][yp] - 1, 1, 4, 0, 1, FALSE);
				if (dPlayer[xp][yp] > 0)
					PlayerMHit(dPlayer[xp][yp] - 1, -1, 0, 8, 16, 1, FALSE, 0);
				if (dObject[xp][yp] > 0) {
					oi = dObject[xp][yp] - 1;
					if (object[oi]._otype == OBJ_BARRELEX && object[oi]._oBreak != -1)
						BreakBarrel(pnum, oi, dam, TRUE, sendmsg);
				}
			}
		}
	} else {
		PlaySfxLoc(IS_BARREL, object[i]._ox, object[i]._oy);
		SetRndSeed(object[i]._oRndSeed);
		if (object[i]._oVar2 <= 1) {
			if (!object[i]._oVar3)
				CreateRndUseful(pnum, object[i]._ox, object[i]._oy, sendmsg);
			else
				CreateRndItem(object[i]._ox, object[i]._oy, FALSE, sendmsg, FALSE);
		}
		if (object[i]._oVar2 >= 8)
			SpawnSkeleton(object[i]._oVar4, object[i]._ox, object[i]._oy);
	}
	if (pnum == myplr)
		NetSendCmdParam2(FALSE, CMD_BREAKOBJ, pnum, i);
}

void BreakObject(int pnum, int oi)
{
	int objdam, mind, maxd;

	if (pnum != -1) {
		mind = plr[pnum]._pIMinDam;
		maxd = random_(163, plr[pnum]._pIMaxDam - mind + 1);
		objdam = maxd + mind;
		objdam += plr[pnum]._pDamageMod + plr[pnum]._pIBonusDamMod + objdam * plr[pnum]._pIBonusDam / 100;
	} else {
		objdam = 10;
	}
	switch (object[oi]._otype) {
	case OBJ_CRUX1:
	case OBJ_CRUX2:
	case OBJ_CRUX3:
		BreakCrux(oi);
		break;
	case OBJ_BARREL:
	case OBJ_BARRELEX:
		BreakBarrel(pnum, oi, objdam, FALSE, TRUE);
		break;
	}
}

void SyncBreakObj(int pnum, int oi)
{
	if (object[oi]._otype >= OBJ_BARREL && object[oi]._otype <= OBJ_BARRELEX)
		BreakBarrel(pnum, oi, 0, TRUE, FALSE);
}

void SyncL1Doors(int i)
{
	int x, y;

	if (object[i]._oVar4 == 0) {
		object[i]._oMissFlag = FALSE;
		return;
	}

	x = object[i]._ox;
	y = object[i]._oy;
	object[i]._oMissFlag = TRUE;
	object[i]._oSelFlag = 2;
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
	DoorSet(i, x, y);
}

void SyncCrux(int i)
{
	BOOL found;
	int j, oi, type;

	found = TRUE;
	for (j = 0; j < nobjects; j++) {
		oi = objectactive[j];
		type = object[oi]._otype;
		if (type != OBJ_CRUX1 && type != OBJ_CRUX2 && type != OBJ_CRUX3)
			continue;
		if (object[i]._oVar8 != object[oi]._oVar8 || object[oi]._oBreak == -1)
			continue;
		found = FALSE;
	}
	if (found)
		ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
}

void SyncLever(int i)
{
	if (object[i]._oSelFlag == 0)
		ObjChangeMap(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
}

void SyncQSTLever(int i)
{
	int tren;

	if (object[i]._oAnimFrame == object[i]._oVar6) {
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		if (object[i]._otype == OBJ_BLINDBOOK) {
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
			TransVal = tren;
		}
	}
}

void SyncPedistal(int i)
{
	BYTE *setp;

	if (object[i]._oVar6 == 1)
		ObjChangeMapResync(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
	if (object[i]._oVar6 == 2) {
		ObjChangeMapResync(setpc_x, setpc_y + 3, setpc_x + 2, setpc_y + 7);
		ObjChangeMapResync(setpc_x + 6, setpc_y + 3, setpc_x + setpc_w, setpc_y + 7);
	}
	if (object[i]._oVar6 == 3) {
		ObjChangeMapResync(object[i]._oVar1, object[i]._oVar2, object[i]._oVar3, object[i]._oVar4);
		setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
		LoadMapObjs(setp, 2 * setpc_x, 2 * setpc_y);
		mem_free_dbg(setp);
	}
}

void SyncL2Doors(int i)
{
	int x, y;

	if (object[i]._oVar4 == 0)
		object[i]._oMissFlag = FALSE;
	else
		object[i]._oMissFlag = TRUE;
	x = object[i]._ox;
	y = object[i]._oy;
	object[i]._oSelFlag = 2;
	if (object[i]._otype == OBJ_L2LDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(x, y, 538);
	} else if (object[i]._otype == OBJ_L2LDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(x, y, 13);
	} else if (object[i]._otype == OBJ_L2RDOOR && object[i]._oVar4 == 0) {
		ObjSetMicro(x, y, 540);
	} else if (object[i]._otype == OBJ_L2RDOOR && (object[i]._oVar4 == 1 || object[i]._oVar4 == 2)) {
		ObjSetMicro(x, y, 17);
	}
}

void SyncL3Doors(int i)
{
	int x, y;

	object[i]._oMissFlag = TRUE;
	x = object[i]._ox;
	y = object[i]._oy;
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
	int file;
	int i;
	int ofindex;

	file = ObjFileList[0];
	ofindex = AllObjects[object[o]._otype].ofindex;
	i = 0;
	while (file != ofindex) {
		file = ObjFileList[i + 1];
		i++;
	}
	object[o]._oAnimData = pObjCels[i];
	switch (object[o]._otype) {
	case OBJ_BOOK2R:
	case OBJ_BLINDBOOK:
	case OBJ_STEELTOME:
		SyncQSTLever(o);
		break;
	case OBJ_L1LIGHT:
		break;
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
	case OBJ_LEVER:
	case OBJ_BOOK2L:
	case OBJ_SWITCHSKL:
		SyncLever(o);
		break;
	case OBJ_CRUX1:
	case OBJ_CRUX2:
	case OBJ_CRUX3:
		SyncCrux(o);
		break;
	case OBJ_PEDISTAL:
		SyncPedistal(o);
		break;
	}
}

void GetObjectStr(int i)
{
	switch (object[i]._otype) {
	case OBJ_LEVER:
	case OBJ_FLAMELVR:
		strcpy(infostr, "Lever");
		break;
	case OBJ_CHEST1:
	case OBJ_TCHEST1:
		strcpy(infostr, "Small Chest");
		break;
	case OBJ_L1LDOOR:
	case OBJ_L1RDOOR:
	case OBJ_L2LDOOR:
	case OBJ_L2RDOOR:
	case OBJ_L3LDOOR:
	case OBJ_L3RDOOR:
		if (object[i]._oVar4 == 1)
			strcpy(infostr, "Open Door");
		if (object[i]._oVar4 == 0)
			strcpy(infostr, "Closed Door");
		if (object[i]._oVar4 == 2)
			strcpy(infostr, "Blocked Door");
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
	case OBJ_SKELBOOK:
		strcpy(infostr, "Skeleton Tome");
		break;
	case OBJ_SHRINEL:
	case OBJ_SHRINER:
		sprintf(tempstr, "%s Shrine", shrinestrs[object[i]._oVar1]);
		strcpy(infostr, tempstr);
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
	case OBJ_STEELTOME:
		strcpy(infostr, "Steel Tome");
		break;
	case OBJ_STORYBOOK:
		strcpy(infostr, StoryBookName[object[i]._oVar3]);
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
	}
	if (plr[myplr]._pClass == PC_ROGUE) {
		if (object[i]._oTrapFlag) {
			sprintf(tempstr, "Trapped %s", infostr);
			strcpy(infostr, tempstr);
			infoclr = COL_RED;
		}
	}
}

DEVILUTION_END_NAMESPACE
