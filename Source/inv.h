/**
 * @file inv.h
 *
 * Interface of player inventory.
 */
#pragma once

#include "items.h"
#include "player.h"

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef enum item_color {
	// clang-format off
	ICOL_WHITE = PAL16_YELLOW + 5,
	ICOL_BLUE  = PAL16_BLUE + 5,
	ICOL_RED   = PAL16_RED + 5,
	// clang-format on
} item_color;

typedef struct InvXY {
	Sint32 X;
	Sint32 Y;
} InvXY;

extern BOOL invflag;
extern BOOL drawsbarflag;
extern const InvXY InvRect[73];

void FreeInvGFX();
void InitInv();

/**
 * @brief Render the inventory panel to the given buffer.
 */
void DrawInv(CelOutputBuffer out);

void DrawInvBelt(CelOutputBuffer out);
bool AutoEquipEnabled(const PlayerStruct &player, const ItemStruct &item);
bool AutoEquip(int playerNumber, const ItemStruct &item, bool persistItem = true);
BOOL AutoPlace(int pnum, int ii, int sx, int sy, BOOL saveflag);
BOOL SpecialAutoPlace(int pnum, int ii, const ItemStruct &item);
BOOL GoldAutoPlace(int pnum);
void CheckInvSwap(int pnum, BYTE bLoc, int idx, WORD wCI, int seed, BOOL bId, uint32_t dwBuff);
void inv_update_rem_item(int pnum, BYTE iv);
void RemoveInvItem(int pnum, int iv);
void RemoveSpdBarItem(int pnum, int iv);
void CheckInvItem(bool isShiftHeld = false);
void CheckInvScrn(bool isShiftHeld);
void CheckItemStats(int pnum);
void InvGetItem(int pnum, int ii);
void AutoGetItem(int pnum, int ii);
int FindGetItem(int idx, WORD ci, int iseed);
void SyncGetItem(int x, int y, int idx, WORD ci, int iseed);
BOOL CanPut(int x, int y);
BOOL TryInvPut();
void DrawInvMsg(const char *msg);
int InvPutItem(int pnum, int x, int y);
int SyncPutItem(int pnum, int x, int y, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff, int to_hit, int max_dam, int min_str, int min_mag, int min_dex, int ac);
char CheckInvHLight();
void RemoveScroll(int pnum);
BOOL UseScroll();
void UseStaffCharge(int pnum);
BOOL UseStaff();
BOOL UseInvItem(int pnum, int cii);
void DoTelekinesis();
int CalculateGold(int pnum);
BOOL DropItemBeforeTrig();

/* data */

extern int AP2x2Tbl[10];

#ifdef __cplusplus
}
#endif

}
