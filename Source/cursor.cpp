/**
 * @file cursor.cpp
 *
 * Implementation of cursor tracking functionality.
 */
#include "cursor.h"

#include <fmt/format.h>

#include "control.h"
#include "doom.h"
#include "engine.h"
#include "engine/load_cel.hpp"
#include "engine/point.hpp"
#include "engine/render/cel_render.hpp"
#include "hwcursor.hpp"
#include "inv.h"
#include "missiles.h"
#include "qol/itemlabels.h"
#include "towners.h"
#include "track.h"
#include "trigs.h"
#include "utils/language.h"

namespace devilution {
namespace {
/** Cursor images CEL */
std::optional<CelSprite> pCursCels;
std::optional<CelSprite> pCursCels2;
constexpr int InvItems1Size = 180;

/** Maps from objcurs.cel frame number to frame width. */
const int InvItemWidth1[] = {
	// clang-format off
	// Cursors
	0, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 23,
	// Items
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
};
const int InvItemWidth2[] = {
	0,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	2 * 28, 2 * 28, 1 * 28, 1 * 28, 1 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28
	// clang-format on
};

/** Maps from objcurs.cel frame number to frame height. */
const int InvItemHeight1[] = {
	// clang-format off
	// Cursors
	0, 29, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35,
	// Items
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
};
const int InvItemHeight2[] = {
	0,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	2 * 28, 2 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28
	// clang-format on
};

} // namespace

/** Pixel size of the current cursor image */
Size cursSize;
/** Current highlighted monster */
int pcursmonst = -1;
/** Size of current cursor in inventory cells */
Size icursSize28;

/** inv_item value */
int8_t pcursinvitem;
/** Pixel size of the current cursor image */
Size icursSize;
/** Current highlighted item */
int8_t pcursitem;
/** Current highlighted object */
int8_t pcursobj;
/** Current highlighted player */
int8_t pcursplr;
/** Current highlighted tile position */
Point cursPosition;
/** Previously highlighted monster */
int pcurstemp;
/** Index of current cursor image */
int pcurs;

void InitCursor()
{
	assert(!pCursCels);
	pCursCels = LoadCel("Data\\Inv\\Objcurs.CEL", InvItemWidth1);
	if (gbIsHellfire)
		pCursCels2 = LoadCel("Data\\Inv\\Objcurs2.CEL", InvItemWidth2);
	ClearCursor();
}

void FreeCursor()
{
	pCursCels = std::nullopt;
	pCursCels2 = std::nullopt;
	ClearCursor();
}

const CelSprite &GetInvItemSprite(int i)
{
	return i < InvItems1Size ? *pCursCels : *pCursCels2;
}

int GetInvItemFrame(int i)
{
	return i < InvItems1Size ? i : i - (InvItems1Size - 1);
}

Size GetInvItemSize(int cursId)
{
	if (cursId >= InvItems1Size)
		return { InvItemWidth2[cursId - (InvItems1Size - 1)], InvItemHeight2[cursId - (InvItems1Size - 1)] };
	return { InvItemWidth1[cursId], InvItemHeight1[cursId] };
}

void SetICursor(int cursId)
{
	icursSize = GetInvItemSize(cursId);
	icursSize28 = icursSize / 28;
}

void NewCursor(int cursId)
{
	pcurs = cursId;
	cursSize = GetInvItemSize(cursId);
	SetICursor(cursId);
	if (IsHardwareCursorEnabled() && GetCurrentCursorInfo() != CursorInfo::GameCursor(cursId) && cursId != CURSOR_NONE) {
		SetHardwareCursor(CursorInfo::GameCursor(cursId));
	}
}

void CelDrawCursor(const Surface &out, Point position, int cursId)
{
	const auto &sprite = GetInvItemSprite(cursId);
	const int frame = GetInvItemFrame(cursId);
	if (IsItemSprite(cursId)) {
		const auto &heldItem = Players[MyPlayerId].HoldItem;
		CelBlitOutlineTo(out, GetOutlineColor(heldItem, true), position, sprite, frame, false);
		CelDrawItem(heldItem, out, position, sprite, frame);
	} else {
		CelClippedDrawTo(out, position, sprite, frame);
	}
}

void InitLevelCursor()
{
	NewCursor(CURSOR_HAND);
	cursPosition = ViewPosition;
	pcurstemp = -1;
	pcursmonst = -1;
	pcursobj = -1;
	pcursitem = -1;
	pcursplr = -1;
	ClearCursor();
}

void CheckTown()
{
	for (int i = 0; i < ActiveMissileCount; i++) {
		int mx = ActiveMissiles[i];
		auto &missile = Missiles[mx];
		if (missile._mitype == MIS_TOWN) {
			if (EntranceBoundaryContains(missile.position.tile, cursPosition)) {
				trigflag = true;
				ClearPanel();
				strcpy(infostr, _("Town Portal"));
				strcpy(tempstr, fmt::format(_("from {:s}"), Players[missile._misource]._pName).c_str());
				AddPanelString(tempstr);
				cursPosition = missile.position.tile;
			}
		}
	}
}

void CheckRportal()
{
	for (int i = 0; i < ActiveMissileCount; i++) {
		int mx = ActiveMissiles[i];
		auto &missile = Missiles[mx];
		if (missile._mitype == MIS_RPORTAL) {
			if (EntranceBoundaryContains(missile.position.tile, cursPosition)) {
				trigflag = true;
				ClearPanel();
				strcpy(infostr, _("Portal to"));
				if (!setlevel)
					strcpy(tempstr, _("The Unholy Altar"));
				else
					strcpy(tempstr, _("level 15"));
				AddPanelString(tempstr);
				cursPosition = missile.position.tile;
			}
		}
	}
}

void CheckCursMove()
{
	if (IsItemLabelHighlighted())
		return;

	int sx = MousePosition.x;
	int sy = MousePosition.y;

	if (CanPanelsCoverView()) {
		if (chrflag || QuestLogIsOpen) {
			sx -= gnScreenWidth / 4;
		} else if (invflag || sbookflag) {
			sx += gnScreenWidth / 4;
		}
	}
	if (sy > PANEL_TOP - 1 && MousePosition.x >= PANEL_LEFT && MousePosition.x < PANEL_LEFT + PANEL_WIDTH && track_isscrolling()) {
		sy = PANEL_TOP - 1;
	}

	if (!zoomflag) {
		sx /= 2;
		sy /= 2;
	}

	// Adjust by player offset and tile grid alignment
	int xo = 0;
	int yo = 0;
	CalcTileOffset(&xo, &yo);
	const auto &myPlayer = Players[MyPlayerId];
	Displacement offset = ScrollInfo.offset;
	if (myPlayer.IsWalking())
		offset = GetOffsetForWalking(myPlayer.AnimInfo, myPlayer._pdir, true);
	sx -= offset.deltaX - xo;
	sy -= offset.deltaY - yo;

	// Predict the next frame when walking to avoid input jitter
	int fx = myPlayer.position.offset2.deltaX / 256;
	int fy = myPlayer.position.offset2.deltaY / 256;
	fx -= (myPlayer.position.offset2.deltaX + myPlayer.position.velocity.deltaX) / 256;
	fy -= (myPlayer.position.offset2.deltaY + myPlayer.position.velocity.deltaY) / 256;
	if (ScrollInfo._sdir != ScrollDirection::None) {
		sx -= fx;
		sy -= fy;
	}

	// Convert to tile grid
	int mx = ViewPosition.x;
	int my = ViewPosition.y;

	int columns = 0;
	int rows = 0;
	TilesInView(&columns, &rows);
	int lrow = rows - RowsCoveredByPanel();

	// Center player tile on screen
	ShiftGrid(&mx, &my, -columns / 2, -lrow / 2);

	// Align grid
	if ((columns % 2) == 0 && (lrow % 2) == 0) {
		sy += TILE_HEIGHT / 2;
	} else if ((columns % 2) != 0 && (lrow % 2) != 0) {
		sx -= TILE_WIDTH / 2;
	} else if ((columns % 2) != 0 && (lrow % 2) == 0) {
		my++;
	}

	if (!zoomflag) {
		sy -= TILE_HEIGHT / 4;
	}

	int tx = sx / TILE_WIDTH;
	int ty = sy / TILE_HEIGHT;
	ShiftGrid(&mx, &my, tx, ty);

	// Shift position to match diamond grid aligment
	int px = sx % TILE_WIDTH;
	int py = sy % TILE_HEIGHT;

	// Shift position to match diamond grid aligment
	bool flipy = py < (px / 2);
	if (flipy) {
		my--;
	}
	bool flipx = py >= TILE_HEIGHT - (px / 2);
	if (flipx) {
		mx++;
	}

	mx = clamp(mx, 0, MAXDUNX - 1);
	my = clamp(my, 0, MAXDUNY - 1);

	// While holding the button down we should retain target (but potentially lose it if it dies, goes out of view, etc)
	if (sgbMouseDown != CLICK_NONE && IsNoneOf(LastMouseButtonAction, MouseActionType::None, MouseActionType::Attack, MouseActionType::Spell)) {
		if (pcursmonst != -1) {
			if (Monsters[pcursmonst]._mDelFlag || Monsters[pcursmonst]._mhitpoints >> 6 <= 0
			    || ((dFlags[Monsters[pcursmonst].position.tile.x][Monsters[pcursmonst].position.tile.y] & BFLAG_VISIBLE) == 0))
				pcursmonst = -1;
		} else if (pcursobj != -1) {
			if (Objects[pcursobj]._oSelFlag < 1)
				pcursobj = -1;
		} else if (pcursplr != -1) {
			auto &targetPlayer = Players[pcursplr];
			if (targetPlayer._pmode == PM_DEATH || targetPlayer._pmode == PM_QUIT || !targetPlayer.plractive
			    || currlevel != targetPlayer.plrlevel || targetPlayer._pHitPoints >> 6 <= 0
			    || ((dFlags[targetPlayer.position.tile.x][targetPlayer.position.tile.y] & BFLAG_VISIBLE) == 0))
				pcursplr = -1;
		}

		if (pcursmonst == -1 && pcursobj == -1 && pcursitem == -1 && pcursinvitem == -1 && pcursplr == -1) {
			cursPosition = { mx, my };
			CheckTrigForce();
			CheckTown();
			CheckRportal();
		}
		return;
	}

	bool flipflag = (flipy && flipx) || ((flipy || flipx) && px < TILE_WIDTH / 2);

	pcurstemp = pcursmonst;
	pcursmonst = -1;
	pcursobj = -1;
	pcursitem = -1;
	if (pcursinvitem != -1) {
		drawsbarflag = true;
	}
	pcursinvitem = -1;
	pcursplr = -1;
	ShowUniqueItemInfoBox = false;
	panelflag = false;
	trigflag = false;

	if (myPlayer._pInvincible) {
		return;
	}
	if (pcurs >= CURSOR_FIRSTITEM || spselflag) {
		cursPosition = { mx, my };
		return;
	}
	if (MainPanel.Contains(MousePosition)) {
		CheckPanelInfo();
		return;
	}
	if (DoomFlag) {
		return;
	}
	if (invflag && RightPanel.Contains(MousePosition)) {
		pcursinvitem = CheckInvHLight();
		return;
	}
	if (sbookflag && RightPanel.Contains(MousePosition)) {
		return;
	}
	if ((chrflag || QuestLogIsOpen) && LeftPanel.Contains(MousePosition)) {
		return;
	}

	if (leveltype != DTYPE_TOWN) {
		if (pcurstemp != -1) {
			if (!flipflag && mx + 2 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 2][my + 1] != 0 && (dFlags[mx + 2][my + 1] & BFLAG_LIT) != 0) {
				int mi = dMonster[mx + 2][my + 1] > 0 ? dMonster[mx + 2][my + 1] - 1 : -(dMonster[mx + 2][my + 1] + 1);
				if (mi == pcurstemp && Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 4) != 0) {
					/// BUGFIX: 'mx + 2' (fixed)
					/// BUGFIX: 'my + 1' (fixed)
					cursPosition = Point { mx, my } + Displacement { 2, 1 };
					pcursmonst = mi;
				}
			}
			if (flipflag && mx + 1 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 1][my + 2] != 0 && (dFlags[mx + 1][my + 2] & BFLAG_LIT) != 0) {
				int mi = dMonster[mx + 1][my + 2] > 0 ? dMonster[mx + 1][my + 2] - 1 : -(dMonster[mx + 1][my + 2] + 1);
				if (mi == pcurstemp && Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 4) != 0) {
					cursPosition = Point { mx, my } + Displacement { 1, 2 };
					pcursmonst = mi;
				}
			}
			if (mx + 2 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 2][my + 2] != 0 && (dFlags[mx + 2][my + 2] & BFLAG_LIT) != 0) {
				int mi = dMonster[mx + 2][my + 2] > 0 ? dMonster[mx + 2][my + 2] - 1 : -(dMonster[mx + 2][my + 2] + 1);
				if (mi == pcurstemp && Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 4) != 0) {
					cursPosition = Point { mx, my } + Displacement { 2, 2 };
					pcursmonst = mi;
				}
			}
			if (mx + 1 < MAXDUNX && !flipflag && dMonster[mx + 1][my] != 0 && (dFlags[mx + 1][my] & BFLAG_LIT) != 0) {
				int mi = dMonster[mx + 1][my] > 0 ? dMonster[mx + 1][my] - 1 : -(dMonster[mx + 1][my] + 1);
				if (mi == pcurstemp && Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 2) != 0) {
					cursPosition = Point { mx, my } + Displacement { 1, 0 };
					pcursmonst = mi;
				}
			}
			if (my + 1 < MAXDUNY && flipflag && dMonster[mx][my + 1] != 0 && (dFlags[mx][my + 1] & BFLAG_LIT) != 0) {
				int mi = dMonster[mx][my + 1] > 0 ? dMonster[mx][my + 1] - 1 : -(dMonster[mx][my + 1] + 1);
				if (mi == pcurstemp && Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 2) != 0) {
					cursPosition = Point { mx, my } + Displacement { 0, 1 };
					pcursmonst = mi;
				}
			}
			if (dMonster[mx][my] != 0 && (dFlags[mx][my] & BFLAG_LIT) != 0) {
				int mi = dMonster[mx][my] > 0 ? dMonster[mx][my] - 1 : -(dMonster[mx][my] + 1);
				if (mi == pcurstemp && Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 1) != 0) {
					cursPosition = { mx, my };
					pcursmonst = mi;
				}
			}
			if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 1][my + 1] != 0 && (dFlags[mx + 1][my + 1] & BFLAG_LIT) != 0) {
				int mi = dMonster[mx + 1][my + 1] > 0 ? dMonster[mx + 1][my + 1] - 1 : -(dMonster[mx + 1][my + 1] + 1);
				if (mi == pcurstemp && Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 2) != 0) {
					cursPosition = Point { mx, my } + Displacement { 1, 1 };
					pcursmonst = mi;
				}
			}
			if (pcursmonst != -1 && (Monsters[pcursmonst]._mFlags & MFLAG_HIDDEN) != 0) {
				pcursmonst = -1;
				cursPosition = { mx, my };
			}
			if (pcursmonst != -1 && (Monsters[pcursmonst]._mFlags & MFLAG_GOLEM) != 0 && (Monsters[pcursmonst]._mFlags & MFLAG_BERSERK) == 0) {
				pcursmonst = -1;
			}
			if (pcursmonst != -1) {
				return;
			}
		}
		if (!flipflag && mx + 2 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 2][my + 1] != 0 && (dFlags[mx + 2][my + 1] & BFLAG_LIT) != 0) {
			int mi = dMonster[mx + 2][my + 1] > 0 ? dMonster[mx + 2][my + 1] - 1 : -(dMonster[mx + 2][my + 1] + 1);
			if (Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 4) != 0) {
				cursPosition = Point { mx, my } + Displacement { 2, 1 };
				pcursmonst = mi;
			}
		}
		if (flipflag && mx + 1 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 1][my + 2] != 0 && (dFlags[mx + 1][my + 2] & BFLAG_LIT) != 0) {
			int mi = dMonster[mx + 1][my + 2] > 0 ? dMonster[mx + 1][my + 2] - 1 : -(dMonster[mx + 1][my + 2] + 1);
			if (Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 4) != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 2 };
				pcursmonst = mi;
			}
		}
		if (mx + 2 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 2][my + 2] != 0 && (dFlags[mx + 2][my + 2] & BFLAG_LIT) != 0) {
			int mi = dMonster[mx + 2][my + 2] > 0 ? dMonster[mx + 2][my + 2] - 1 : -(dMonster[mx + 2][my + 2] + 1);
			if (Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 4) != 0) {
				cursPosition = Point { mx, my } + Displacement { 2, 2 };
				pcursmonst = mi;
			}
		}
		if (!flipflag && mx + 1 < MAXDUNX && dMonster[mx + 1][my] != 0 && (dFlags[mx + 1][my] & BFLAG_LIT) != 0) {
			int mi = dMonster[mx + 1][my] > 0 ? dMonster[mx + 1][my] - 1 : -(dMonster[mx + 1][my] + 1);
			if (Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 2) != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 0 };
				pcursmonst = mi;
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dMonster[mx][my + 1] != 0 && (dFlags[mx][my + 1] & BFLAG_LIT) != 0) {
			int mi = dMonster[mx][my + 1] > 0 ? dMonster[mx][my + 1] - 1 : -(dMonster[mx][my + 1] + 1);
			if (Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 2) != 0) {
				cursPosition = Point { mx, my } + Displacement { 0, 1 };
				pcursmonst = mi;
			}
		}
		if (dMonster[mx][my] != 0 && (dFlags[mx][my] & BFLAG_LIT) != 0) {
			int mi = dMonster[mx][my] > 0 ? dMonster[mx][my] - 1 : -(dMonster[mx][my] + 1);
			if (Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 1) != 0) {
				cursPosition = { mx, my };
				pcursmonst = mi;
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 1][my + 1] != 0 && (dFlags[mx + 1][my + 1] & BFLAG_LIT) != 0) {
			int mi = dMonster[mx + 1][my + 1] > 0 ? dMonster[mx + 1][my + 1] - 1 : -(dMonster[mx + 1][my + 1] + 1);
			if (Monsters[mi]._mhitpoints >> 6 > 0 && (Monsters[mi].MData->mSelFlag & 2) != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 1 };
				pcursmonst = mi;
			}
		}
		if (pcursmonst != -1 && (Monsters[pcursmonst]._mFlags & MFLAG_HIDDEN) != 0) {
			pcursmonst = -1;
			cursPosition = { mx, my };
		}
		if (pcursmonst != -1 && (Monsters[pcursmonst]._mFlags & MFLAG_GOLEM) != 0 && (Monsters[pcursmonst]._mFlags & MFLAG_BERSERK) == 0) {
			pcursmonst = -1;
		}
	} else {
		if (!flipflag && mx + 1 < MAXDUNX && dMonster[mx + 1][my] > 0) {
			pcursmonst = dMonster[mx + 1][my] - 1;
			cursPosition = Point { mx, my } + Displacement { 1, 0 };
		}
		if (flipflag && my + 1 < MAXDUNY && dMonster[mx][my + 1] > 0) {
			pcursmonst = dMonster[mx][my + 1] - 1;
			cursPosition = Point { mx, my } + Displacement { 0, 1 };
		}
		if (dMonster[mx][my] > 0) {
			pcursmonst = dMonster[mx][my] - 1;
			cursPosition = { mx, my };
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 1][my + 1] > 0) {
			pcursmonst = dMonster[mx + 1][my + 1] - 1;
			cursPosition = Point { mx, my } + Displacement { 1, 1 };
		}
	}

	if (pcursmonst == -1) {
		if (!flipflag && mx + 1 < MAXDUNX && dPlayer[mx + 1][my] != 0) {
			int8_t bv = dPlayer[mx + 1][my] > 0 ? dPlayer[mx + 1][my] - 1 : -(dPlayer[mx + 1][my] + 1);
			if (bv != MyPlayerId && Players[bv]._pHitPoints != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 0 };
				pcursplr = bv;
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dPlayer[mx][my + 1] != 0) {
			int8_t bv = dPlayer[mx][my + 1] > 0 ? dPlayer[mx][my + 1] - 1 : -(dPlayer[mx][my + 1] + 1);
			if (bv != MyPlayerId && Players[bv]._pHitPoints != 0) {
				cursPosition = Point { mx, my } + Displacement { 0, 1 };
				pcursplr = bv;
			}
		}
		if (dPlayer[mx][my] != 0) {
			int8_t bv = dPlayer[mx][my] > 0 ? dPlayer[mx][my] - 1 : -(dPlayer[mx][my] + 1);
			if (bv != MyPlayerId) {
				cursPosition = { mx, my };
				pcursplr = bv;
			}
		}
		if ((dFlags[mx][my] & BFLAG_DEAD_PLAYER) != 0) {
			for (int i = 0; i < MAX_PLRS; i++) {
				if (Players[i].position.tile.x == mx && Players[i].position.tile.y == my && i != MyPlayerId) {
					cursPosition = { mx, my };
					pcursplr = i;
				}
			}
		}
		if (pcurs == CURSOR_RESURRECT) {
			for (int xx = -1; xx < 2; xx++) {
				for (int yy = -1; yy < 2; yy++) {
					if (mx + xx < MAXDUNX && my + yy < MAXDUNY && (dFlags[mx + xx][my + yy] & BFLAG_DEAD_PLAYER) != 0) {
						for (int i = 0; i < MAX_PLRS; i++) {
							if (Players[i].position.tile.x == mx + xx && Players[i].position.tile.y == my + yy && i != MyPlayerId) {
								cursPosition = Point { mx, my } + Displacement { xx, yy };
								pcursplr = i;
							}
						}
					}
				}
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dPlayer[mx + 1][my + 1] != 0) {
			int8_t bv = dPlayer[mx + 1][my + 1] > 0 ? dPlayer[mx + 1][my + 1] - 1 : -(dPlayer[mx + 1][my + 1] + 1);
			if (bv != MyPlayerId && Players[bv]._pHitPoints != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 1 };
				pcursplr = bv;
			}
		}
	}
	if (pcursmonst == -1 && pcursplr == -1) {
		if (!flipflag && mx + 1 < MAXDUNX && dObject[mx + 1][my] != 0) {
			int8_t bv = dObject[mx + 1][my] > 0 ? dObject[mx + 1][my] - 1 : -(dObject[mx + 1][my] + 1);
			if (Objects[bv]._oSelFlag >= 2) {
				cursPosition = Point { mx, my } + Displacement { 1, 0 };
				pcursobj = bv;
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dObject[mx][my + 1] != 0) {
			int8_t bv = dObject[mx][my + 1] > 0 ? dObject[mx][my + 1] - 1 : -(dObject[mx][my + 1] + 1);
			if (Objects[bv]._oSelFlag >= 2) {
				cursPosition = Point { mx, my } + Displacement { 0, 1 };
				pcursobj = bv;
			}
		}
		if (dObject[mx][my] != 0) {
			int8_t bv = dObject[mx][my] > 0 ? dObject[mx][my] - 1 : -(dObject[mx][my] + 1);
			if (Objects[bv]._oSelFlag == 1 || Objects[bv]._oSelFlag == 3) {
				cursPosition = { mx, my };
				pcursobj = bv;
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dObject[mx + 1][my + 1] != 0) {
			int8_t bv = dObject[mx + 1][my + 1] > 0 ? dObject[mx + 1][my + 1] - 1 : -(dObject[mx + 1][my + 1] + 1);
			if (Objects[bv]._oSelFlag >= 2) {
				cursPosition = Point { mx, my } + Displacement { 1, 1 };
				pcursobj = bv;
			}
		}
	}
	if (pcursplr == -1 && pcursobj == -1 && pcursmonst == -1) {
		if (!flipflag && mx + 1 < MAXDUNX && dItem[mx + 1][my] > 0) {
			int8_t bv = dItem[mx + 1][my] - 1;
			if (Items[bv]._iSelFlag >= 2) {
				cursPosition = Point { mx, my } + Displacement { 1, 0 };
				pcursitem = bv;
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dItem[mx][my + 1] > 0) {
			int8_t bv = dItem[mx][my + 1] - 1;
			if (Items[bv]._iSelFlag >= 2) {
				cursPosition = Point { mx, my } + Displacement { 0, 1 };
				pcursitem = bv;
			}
		}
		if (dItem[mx][my] > 0) {
			int8_t bv = dItem[mx][my] - 1;
			if (Items[bv]._iSelFlag == 1 || Items[bv]._iSelFlag == 3) {
				cursPosition = { mx, my };
				pcursitem = bv;
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dItem[mx + 1][my + 1] > 0) {
			int8_t bv = dItem[mx + 1][my + 1] - 1;
			if (Items[bv]._iSelFlag >= 2) {
				cursPosition = Point { mx, my } + Displacement { 1, 1 };
				pcursitem = bv;
			}
		}
		if (pcursitem == -1) {
			cursPosition = { mx, my };
			CheckTrigForce();
			CheckTown();
			CheckRportal();
		}
	}

	if (pcurs == CURSOR_IDENTIFY) {
		pcursobj = -1;
		pcursmonst = -1;
		pcursitem = -1;
		cursPosition = { mx, my };
	}
	if (pcursmonst != -1 && (Monsters[pcursmonst]._mFlags & MFLAG_GOLEM) != 0 && (Monsters[pcursmonst]._mFlags & MFLAG_BERSERK) == 0) {
		pcursmonst = -1;
	}
}

} // namespace devilution
