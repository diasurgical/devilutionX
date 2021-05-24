/**
 * @file portal.cpp
 *
 * Implementation of functionality for handling town portals.
 */
#include "portal.h"

#include "lighting.h"
#include "misdat.h"
#include "missiles.h"
#include "multi.h"
#include "player.h"

namespace devilution {

/** In-game state of portals. */
PortalStruct portal[MAXPORTAL];
/** Current portal number (a portal array index). */
int portalindex;

/** X-coordinate of each players portal in town. */
int WarpDropX[MAXPORTAL] = { 57, 59, 61, 63 };
/** Y-coordinate of each players portal in town. */
int WarpDropY[MAXPORTAL] = { 40, 40, 40, 40 };

void InitPortals()
{
	int i;

	for (i = 0; i < MAXPORTAL; i++) {
		if (delta_portal_inited(i))
			portal[i].open = false;
	}
}

void SetPortalStats(int i, bool o, int x, int y, int lvl, dungeon_type lvltype)
{
	portal[i].open = o;
	portal[i].position = { x, y };
	portal[i].level = lvl;
	portal[i].ltype = lvltype;
	portal[i].setlvl = false;
}

void AddWarpMissile(int i, int x, int y)
{
	int mi;

	missiledata[MIS_TOWN].mlSFX = SFX_NONE;
	dMissile[x][y] = 0;
	mi = AddMissile(0, 0, x, y, 0, MIS_TOWN, TARGET_MONSTERS, i, 0, 0);

	if (mi != -1) {
		SetMissDir(mi, 1);

		if (currlevel != 0)
			missile[mi]._mlid = AddLight(missile[mi].position.tile, 15);

		missiledata[MIS_TOWN].mlSFX = LS_SENTINEL;
	}
}

void SyncPortals()
{
	int i;

	for (i = 0; i < MAXPORTAL; i++) {
		if (!portal[i].open)
			continue;
		if (currlevel == 0)
			AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
		else {
			int lvl = currlevel;
			if (setlevel)
				lvl = setlvlnum;
			if (portal[i].level == lvl && portal[i].setlvl == setlevel)
				AddWarpMissile(i, portal[i].position.x, portal[i].position.y);
		}
	}
}

void AddInTownPortal(int i)
{
	AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
}

void ActivatePortal(int i, int x, int y, int lvl, dungeon_type lvltype, bool sp)
{
	portal[i].open = true;

	if (lvl != 0) {
		portal[i].position = { x, y };
		portal[i].level = lvl;
		portal[i].ltype = lvltype;
		portal[i].setlvl = sp;
	}
}

void DeactivatePortal(int i)
{
	portal[i].open = false;
}

bool PortalOnLevel(int i)
{
	if (portal[i].level == currlevel)
		return true;

	return currlevel == 0;
}

void RemovePortalMissile(int id)
{
	int i;
	int mi;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		if (missile[mi]._mitype == MIS_TOWN && missile[mi]._misource == id) {
			dFlags[missile[mi].position.tile.x][missile[mi].position.tile.y] &= ~BFLAG_MISSILE;
			dMissile[missile[mi].position.tile.x][missile[mi].position.tile.y] = 0;

			if (portal[id].level != 0)
				AddUnLight(missile[mi]._mlid);

			DeleteMissile(mi, i);
		}
	}
}

void SetCurrentPortal(int p)
{
	portalindex = p;
}

void GetPortalLevel()
{
	if (currlevel != 0) {
		setlevel = false;
		currlevel = 0;
		plr[myplr].plrlevel = 0;
		leveltype = DTYPE_TOWN;
		return;
	}

	if (portal[portalindex].setlvl) {
		setlevel = true;
		setlvlnum = (_setlevels)portal[portalindex].level;
		currlevel = portal[portalindex].level;
		plr[myplr].plrlevel = setlvlnum;
		leveltype = portal[portalindex].ltype;
	} else {
		setlevel = false;
		currlevel = portal[portalindex].level;
		plr[myplr].plrlevel = currlevel;
		leveltype = portal[portalindex].ltype;
	}

	if (portalindex == myplr) {
		NetSendCmd(true, CMD_DEACTIVATEPORTAL);
		DeactivatePortal(portalindex);
	}
}

void GetPortalLvlPos()
{
	if (currlevel == 0) {
		ViewX = WarpDropX[portalindex] + 1;
		ViewY = WarpDropY[portalindex] + 1;
	} else {
		ViewX = portal[portalindex].position.x;
		ViewY = portal[portalindex].position.y;

		if (portalindex != myplr) {
			ViewX++;
			ViewY++;
		}
	}
}

bool PosOkPortal(int lvl, int x, int y)
{
	int i;

	for (i = 0; i < MAXPORTAL; i++) {
		if (portal[i].open && portal[i].level == lvl && ((portal[i].position.x == x && portal[i].position.y == y) || (portal[i].position.x == x - 1 && portal[i].position.y == y - 1)))
			return true;
	}
	return false;
}

} // namespace devilution
