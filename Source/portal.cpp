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
PortalStruct Portals[MAXPORTAL];

namespace {

/** Current portal number (a portal array index). */
int portalindex;

/** X-coordinate of each players portal in town. */
int WarpDropX[MAXPORTAL] = { 57, 59, 61, 63 };
/** Y-coordinate of each players portal in town. */
int WarpDropY[MAXPORTAL] = { 40, 40, 40, 40 };

} // namespace

void InitPortals()
{
	for (int i = 0; i < MAXPORTAL; i++) {
		if (delta_portal_inited(i))
			Portals[i].open = false;
	}
}

void SetPortalStats(int i, bool o, int x, int y, int lvl, dungeon_type lvltype)
{
	Portals[i].open = o;
	Portals[i].position = { x, y };
	Portals[i].level = lvl;
	Portals[i].ltype = lvltype;
	Portals[i].setlvl = false;
}

void AddWarpMissile(int i, int x, int y)
{
	MissileData[MIS_TOWN].mlSFX = SFX_NONE;

	int mi = AddMissile({ 0, 0 }, { x, y }, DIR_S, MIS_TOWN, TARGET_MONSTERS, i, 0, 0);
	if (mi == -1)
		return;

	auto &missile = Missiles[mi];
	SetMissDir(missile, 1);

	if (currlevel != 0)
		missile._mlid = AddLight(missile.position.tile, 15);

	MissileData[MIS_TOWN].mlSFX = LS_SENTINEL;
}

void SyncPortals()
{
	for (int i = 0; i < MAXPORTAL; i++) {
		if (!Portals[i].open)
			continue;
		if (currlevel == 0)
			AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
		else {
			int lvl = currlevel;
			if (setlevel)
				lvl = setlvlnum;
			if (Portals[i].level == lvl && Portals[i].setlvl == setlevel)
				AddWarpMissile(i, Portals[i].position.x, Portals[i].position.y);
		}
	}
}

void AddInTownPortal(int i)
{
	AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
}

void ActivatePortal(int i, int x, int y, int lvl, dungeon_type lvltype, bool sp)
{
	Portals[i].open = true;

	if (lvl != 0) {
		Portals[i].position = { x, y };
		Portals[i].level = lvl;
		Portals[i].ltype = lvltype;
		Portals[i].setlvl = sp;
	}
}

void DeactivatePortal(int i)
{
	Portals[i].open = false;
}

bool PortalOnLevel(int i)
{
	if (Portals[i].level == currlevel)
		return true;

	return currlevel == 0;
}

void RemovePortalMissile(int id)
{
	for (int i = 0; i < ActiveMissileCount; i++) {
		int mi = ActiveMissiles[i];
		auto &missile = Missiles[mi];
		if (missile._mitype == MIS_TOWN && missile._misource == id) {
			dFlags[missile.position.tile.x][missile.position.tile.y] &= ~BFLAG_MISSILE;

			if (Portals[id].level != 0)
				AddUnLight(missile._mlid);

			DeleteMissile(i);
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
		Players[MyPlayerId].plrlevel = 0;
		leveltype = DTYPE_TOWN;
		return;
	}

	if (Portals[portalindex].setlvl) {
		setlevel = true;
		setlvlnum = (_setlevels)Portals[portalindex].level;
		currlevel = Portals[portalindex].level;
		Players[MyPlayerId].plrlevel = setlvlnum;
		leveltype = Portals[portalindex].ltype;
	} else {
		setlevel = false;
		currlevel = Portals[portalindex].level;
		Players[MyPlayerId].plrlevel = currlevel;
		leveltype = Portals[portalindex].ltype;
	}

	if (portalindex == MyPlayerId) {
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
		ViewX = Portals[portalindex].position.x;
		ViewY = Portals[portalindex].position.y;

		if (portalindex != MyPlayerId) {
			ViewX++;
			ViewY++;
		}
	}
}

bool PosOkPortal(int lvl, int x, int y)
{
	for (auto &portal : Portals) {
		if (portal.open && portal.level == lvl && ((portal.position.x == x && portal.position.y == y) || (portal.position.x == x - 1 && portal.position.y == y - 1)))
			return true;
	}
	return false;
}

} // namespace devilution
