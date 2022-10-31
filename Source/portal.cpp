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
Portal Portals[MAXPORTAL];

namespace {

/** Current portal number (a portal array index). */
size_t portalindex;

/** Coordinate of each players portal in town. */
Point WarpDrop[MAXPORTAL] = {
	{ 57, 40 },
	{ 59, 40 },
	{ 61, 40 },
	{ 63, 40 },
};

} // namespace

void InitPortals()
{
	for (auto &portal : Portals) {
		portal.open = false;
	}
}

void SetPortalStats(int i, bool o, int x, int y, int lvl, dungeon_type lvltype, bool isSetLevel)
{
	Portals[i].open = o;
	Portals[i].position = { x, y };
	Portals[i].level = lvl;
	Portals[i].ltype = lvltype;
	Portals[i].setlvl = isSetLevel;
}

void AddWarpMissile(int i, Point position)
{
	MissilesData[MIS_TOWN].mlSFX = SFX_NONE;

	auto *missile = AddMissile({ 0, 0 }, position, Direction::South, MIS_TOWN, TARGET_MONSTERS, i, 0, 0);
	if (missile != nullptr) {
		SetMissDir(*missile, 1);

		if (leveltype != DTYPE_TOWN)
			missile->_mlid = AddLight(missile->position.tile, 15);
	}

	MissilesData[MIS_TOWN].mlSFX = LS_SENTINEL;
}

void SyncPortals()
{
	for (int i = 0; i < MAXPORTAL; i++) {
		if (!Portals[i].open)
			continue;
		if (leveltype == DTYPE_TOWN)
			AddWarpMissile(i, WarpDrop[i]);
		else {
			int lvl = currlevel;
			if (setlevel)
				lvl = setlvlnum;
			if (Portals[i].level == lvl && Portals[i].setlvl == setlevel)
				AddWarpMissile(i, Portals[i].position);
		}
	}
}

void AddInTownPortal(int i)
{
	AddWarpMissile(i, WarpDrop[i]);
}

void ActivatePortal(int i, Point position, int lvl, dungeon_type dungeonType, bool isSetLevel)
{
	Portals[i].open = true;

	if (lvl != 0) {
		Portals[i].position = position;
		Portals[i].level = lvl;
		Portals[i].ltype = dungeonType;
		Portals[i].setlvl = isSetLevel;
	}
}

void DeactivatePortal(int i)
{
	Portals[i].open = false;
}

bool PortalOnLevel(size_t i)
{
	if (Portals[i].setlvl == setlevel && Portals[i].level == setlevel ? static_cast<int>(setlvlnum) : currlevel)
		return true;

	return leveltype == DTYPE_TOWN;
}

void RemovePortalMissile(int id)
{
	Missiles.remove_if([id](Missile &missile) {
		if (missile._mitype == MIS_TOWN && missile._misource == id) {
			dFlags[missile.position.tile.x][missile.position.tile.y] &= ~DungeonFlag::Missile;

			if (Portals[id].level != 0)
				AddUnLight(missile._mlid);

			return true;
		}
		return false;
	});
}

void SetCurrentPortal(size_t p)
{
	portalindex = p;
}

void GetPortalLevel()
{
	if (leveltype != DTYPE_TOWN) {
		setlevel = false;
		currlevel = 0;
		MyPlayer->setLevel(0);
		leveltype = DTYPE_TOWN;
		return;
	}

	if (Portals[portalindex].setlvl) {
		setlevel = true;
		setlvlnum = (_setlevels)Portals[portalindex].level;
		currlevel = Portals[portalindex].level;
		MyPlayer->setLevel(setlvlnum);
		leveltype = Portals[portalindex].ltype;
	} else {
		setlevel = false;
		currlevel = Portals[portalindex].level;
		MyPlayer->setLevel(currlevel);
		leveltype = Portals[portalindex].ltype;
	}

	if (portalindex == MyPlayerId) {
		NetSendCmd(true, CMD_DEACTIVATEPORTAL);
		DeactivatePortal(portalindex);
	}
}

void GetPortalLvlPos()
{
	if (leveltype == DTYPE_TOWN) {
		ViewPosition = WarpDrop[portalindex] + Displacement { 1, 1 };
	} else {
		ViewPosition = Portals[portalindex].position;

		if (portalindex != MyPlayerId) {
			ViewPosition.x++;
			ViewPosition.y++;
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
