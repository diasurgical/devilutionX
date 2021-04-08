/**
 * @file portal.h
 *
 * Interface of functionality for handling town portals.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PortalStruct {
	bool open;
	Sint32 x;
	Sint32 y;
	Sint32 level;
	dungeon_type ltype;
	bool setlvl;
} PortalStruct;

extern PortalStruct portal[MAXPORTAL];

void InitPortals();
void SetPortalStats(int i, BOOL o, int x, int y, int lvl, dungeon_type lvltype);
void AddWarpMissile(int i, int x, int y);
void SyncPortals();
void AddInTownPortal(int i);
void ActivatePortal(int i, int x, int y, int lvl, dungeon_type lvltype, BOOL sp);
void DeactivatePortal(int i);
BOOL PortalOnLevel(int i);
void RemovePortalMissile(int id);
void SetCurrentPortal(int p);
void GetPortalLevel();
void GetPortalLvlPos();
BOOL PosOkPortal(int lvl, int x, int y);

#ifdef __cplusplus
}
#endif

}
