//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

PortalStruct portal[MAXPORTAL];
int portalindex;

int WarpDropX[MAXPORTAL] = { 57, 59, 61, 63 };
int WarpDropY[MAXPORTAL] = { 40, 40, 40, 40 };

void __cdecl InitPortals()
{
	int i;

	for (i = 0; i < MAXPORTAL; i++) {
		if (delta_portal_inited(i))
			portal[i].open = FALSE;
	}
}

void __fastcall SetPortalStats(int i, BOOL o, int x, int y, int lvl, int lvltype)
{
	portal[i].setlvl = FALSE;
	portal[i].x = x;
	portal[i].y = y;
	portal[i].open = o;
	portal[i].level = lvl;
	portal[i].ltype = lvltype;
}

void __fastcall AddWarpMissile(int i, int x, int y)
{
	int mi;

	missiledata[MIS_TOWN].mlSFX = -1;
	dMissile[x][y] = 0;
	mi = AddMissile(0, 0, x, y, 0, MIS_TOWN, 0, i, 0, 0);

	if (mi != -1) {
		SetMissDir(mi, 1);

		if (currlevel != 0)
			missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 15);

		missiledata[MIS_TOWN].mlSFX = LS_SENTINEL;
	}
}

void __cdecl SyncPortals()
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
			if (portal[i].level == lvl)
				AddWarpMissile(i, portal[i].x, portal[i].y);
		}
	}
}
// 5CF31D: using guessed type char setlevel;

void __fastcall AddInTownPortal(int i)
{
	AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
}

void __fastcall ActivatePortal(int i, int x, int y, int lvl, int lvltype, BOOL sp)
{
	portal[i].open = TRUE;

	if (lvl != 0) {
		portal[i].x = x;
		portal[i].y = y;
		portal[i].level = lvl;
		portal[i].ltype = lvltype;
		portal[i].setlvl = sp;
	}
}

void __fastcall DeactivatePortal(int i)
{
	portal[i].open = FALSE;
}

BOOL __fastcall PortalOnLevel(int i)
{
	if (portal[i].level == currlevel)
		return TRUE;
	else
		return currlevel == 0;
}

void __fastcall RemovePortalMissile(int id)
{
	int i;
	int mi;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		if (missile[mi]._mitype == MIS_TOWN && missile[mi]._misource == id) {
			dFlags[missile[mi]._mix][missile[mi]._miy] &= ~DFLAG_MISSILE;
			dMissile[missile[mi]._mix][missile[mi]._miy] = 0;

			if (portal[id].level)
				AddUnLight(missile[mi]._mlid);

			DeleteMissile(mi, i);
		}
	}
}

void __fastcall SetCurrentPortal(int p)
{
	portalindex = p;
}

void __cdecl GetPortalLevel()
{
	if (currlevel) {
		setlevel = 0;
		currlevel = 0;
		plr[myplr].plrlevel = 0;
		leveltype = 0;
	} else {
		if (portal[portalindex].setlvl) {
			setlevel = 1;
			setlvlnum = portal[portalindex].level;
			currlevel = portal[portalindex].level;
			plr[myplr].plrlevel = setlvlnum;
			leveltype = portal[portalindex].ltype;
		} else {
			setlevel = 0;
			currlevel = portal[portalindex].level;
			plr[myplr].plrlevel = currlevel;
			leveltype = portal[portalindex].ltype;
		}
		if (portalindex == myplr) {
			NetSendCmd(TRUE, CMD_DEACTIVATEPORTAL);
			DeactivatePortal(portalindex);
		}
	}
}
// 5CF31D: using guessed type char setlevel;

void __cdecl GetPortalLvlPos()
{
	if (currlevel == 0) {
		ViewX = WarpDropX[portalindex] + 1;
		ViewY = WarpDropY[portalindex] + 1;
	} else {
		ViewX = portal[portalindex].x;
		ViewY = portal[portalindex].y;

		if (portalindex != myplr) {
			ViewX++;
			ViewY++;
		}
	}
}

BOOL __fastcall PosOkPortal(int lvl, int x, int y)
{
	int i;

	for (i = 0; i < MAXPORTAL; i++) {
		if (portal[i].open && portal[i].level == lvl && ((portal[i].x == x && portal[i].y == y) || (portal[i].x == x - 1 && portal[i].y == y - 1)))
			return TRUE;
	}
	return FALSE;
}

DEVILUTION_END_NAMESPACE
