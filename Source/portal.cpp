//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
PortalStruct portal[MAXPORTAL];
int portalindex;
#endif

int WarpDropX[MAXPORTAL] = { 57, 59, 61, 63 };
int WarpDropY[MAXPORTAL] = { 40, 40, 40, 40 };

void __cdecl InitPortals()
{
	int i; // edi

	for(i = 0; i < MAXPORTAL; i++)
	{
		if(delta_portal_inited(i))
			portal[i].open = 0;
	}
}

void __fastcall SetPortalStats(int i, int o, int x, int y, int lvl, int lvltype)
{
	portal[i].x = x;
	portal[i].setlvl = 0;
	portal[i].y = y;
	portal[i].open = o;
	portal[i].level = lvl;
	portal[i].ltype = lvltype;
}

void __fastcall AddWarpMissile(int i, int x, int y)
{
	int mi; // eax

	missiledata[MIS_TOWN].mlSFX = -1;
	dMissile[x][y] = 0;
	mi = AddMissile(0, 0, x, y, 0, MIS_TOWN, 0, i, 0, 0);

	if ( mi != -1 )
	{
		SetMissDir(mi, 1);

		if ( currlevel )
			missile[mi]._mlid = AddLight(missile[mi]._mix, missile[mi]._miy, 15);

		missiledata[MIS_TOWN].mlSFX = LS_SENTINEL;
	}
}

void __cdecl SyncPortals()
{
	int v0; // edi
	int *v1; // esi
	int v2; // eax

	v0 = 0;
	v1 = &portal[0].level;
	do
	{
		if ( *(v1 - 3) )
		{
			if ( currlevel )
			{
				v2 = currlevel;
				if ( setlevel )
					v2 = (unsigned char)setlvlnum;
				if ( *v1 == v2 )
					AddWarpMissile(v0, *(v1 - 2), *(v1 - 1));
			}
			else
			{
				AddWarpMissile(v0, WarpDropX[v0], WarpDropY[v0]);
			}
		}
		v1 += 6;
		++v0;
	}
	while ( (signed int)v1 < (signed int)&portal[MAXPORTAL].level );
}
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;
// 69BD04: using guessed type int questlog;

void __fastcall AddInTownPortal(int i)
{
	AddWarpMissile(i, WarpDropX[i], WarpDropY[i]);
}

void __fastcall ActivatePortal(int i, int x, int y, int lvl, int lvltype, int sp)
{
	portal[i].open = 1;

	if ( lvl )
	{
		portal[i].level = lvl;
		portal[i].x = x;
		portal[i].ltype = lvltype;
		portal[i].y = y;
		portal[i].setlvl = sp;
	}
}

void __fastcall DeactivatePortal(int i)
{
	portal[i].open = 0;
}

bool __fastcall PortalOnLevel(int i)
{
	if ( portal[i].level == currlevel )
		return 1;
	else
		return currlevel == 0;
}

void __fastcall RemovePortalMissile(int id)
{
	int i; // esi
	int mi; // eax

	for ( i = 0; i < nummissiles; ++i )
	{
		mi = missileactive[i];
		if ( missile[mi]._mitype == MIS_TOWN && missile[mi]._misource == id )
		{
			dFlags[missile[mi]._mix][missile[mi]._miy] &= ~DFLAG_MISSILE;
			dMissile[missile[mi]._mix][missile[mi]._miy] = 0;

			if ( portal[id].level )
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
	if ( currlevel )
	{
		setlevel = 0;
		currlevel = 0;
		leveltype = 0;
		plr[myplr].plrlevel = 0;
	}
	else
	{
		if ( portal[portalindex].setlvl )
		{
			setlevel = 1;
			setlvlnum = portal[portalindex].level;
		}
		else
		{
			setlevel = 0;
		}

		currlevel = portal[portalindex].level;
		leveltype = portal[portalindex].ltype;
		plr[myplr].plrlevel = portal[portalindex].level;

		if ( portalindex == myplr )
		{
			NetSendCmd(1, CMD_DEACTIVATEPORTAL);
			DeactivatePortal(portalindex);
		}
	}
}
// 5BB1ED: using guessed type char leveltype;
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;

void __cdecl GetPortalLvlPos()
{
	if ( currlevel )
	{
		ViewX = portal[portalindex].x;
		ViewY = portal[portalindex].y;

		if ( portalindex != myplr )
		{
			ViewX++;
			ViewY++;
		}
	}
	else
	{
		ViewX = WarpDropX[portalindex] + 1;
		ViewY = WarpDropY[portalindex] + 1;
	}
}

BOOL __fastcall PosOkPortal(int lvl, int x, int y)
{
	int *v3; // eax

	v3 = &portal[0].x;
	while ( !*(v3 - 1) || v3[2] != lvl || (*v3 != x || v3[1] != y) && (*v3 != x - 1 || v3[1] != y - 1) )
	{
		v3 += 6;
		if ( (signed int)v3 >= (signed int)&portal[MAXPORTAL].x )
			return 0;
	}
	return 1;
}
// 69BCFC: using guessed type int END_portalstruct;
