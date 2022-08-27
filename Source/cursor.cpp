/**
 * @file cursor.cpp
 *
 * Implementation of cursor tracking functionality.
 */
#include "cursor.h"

#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "control.h"
#include "controls/plrctrls.h"
#include "doom.h"
#include "engine.h"
#include "engine/load_cel.hpp"
#include "engine/point.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/trn.hpp"
#include "hwcursor.hpp"
#include "inv.h"
#include "levels/trigs.h"
#include "missiles.h"
#include "options.h"
#include "qol/itemlabels.h"
#include "qol/stash.h"
#include "towners.h"
#include "track.h"
#include "utils/attributes.h"
#include "utils/language.h"
#include "utils/utf8.hpp"

namespace devilution {
namespace {
/** Cursor images CEL */
OptionalOwnedClxSpriteList pCursCels;
OptionalOwnedClxSpriteList pCursCels2;

/** Maps from objcurs.cel frame number to frame width. */
const uint16_t InvItemWidth1[] = {
	// clang-format off
	// Cursors
	33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 23,
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
const uint16_t InvItemWidth2[] = {
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	2 * 28, 2 * 28, 1 * 28, 1 * 28, 1 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28,
	2 * 28
	// clang-format on
};
constexpr uint16_t InvItems1Size = sizeof(InvItemWidth1) / sizeof(InvItemWidth1[0]);
constexpr uint16_t InvItems2Size = sizeof(InvItemWidth2) / sizeof(InvItemWidth2[0]);

/** Maps from objcurs.cel frame number to frame height. */
const uint16_t InvItemHeight1[InvItems1Size] = {
	// clang-format off
	// Cursors
	29, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35,
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
const uint16_t InvItemHeight2[InvItems2Size] = {
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28, 1 * 28,
	2 * 28, 2 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28,
	3 * 28
	// clang-format on
};

} // namespace

/** Current highlighted monster */
int pcursmonst = -1;

/** inv_item value */
int8_t pcursinvitem;
/** StashItem value */
uint16_t pcursstashitem;
/** Current highlighted item */
int8_t pcursitem;
/** Current highlighted object */
Object *ObjectUnderCursor;
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
	pCursCels = LoadCel("data\\inv\\objcurs.cel", InvItemWidth1);
	if (gbIsHellfire)
		pCursCels2 = LoadCel("data\\inv\\objcurs2.cel", InvItemWidth2);
	ClearCursor();
}

void FreeCursor()
{
	pCursCels = std::nullopt;
	pCursCels2 = std::nullopt;
	ClearCursor();
}

ClxSprite GetInvItemSprite(int cursId)
{
	if (cursId <= InvItems1Size)
		return (*pCursCels)[cursId - 1];
	return (*pCursCels2)[cursId - InvItems1Size - 1];
}

Size GetInvItemSize(int cursId)
{
	const int i = cursId - 1;
	if (i >= InvItems1Size)
		return { InvItemWidth2[i - InvItems1Size], InvItemHeight2[i - InvItems1Size] };
	return { InvItemWidth1[i], InvItemHeight1[i] };
}

void DrawItem(const Item &item, const Surface &out, Point position, ClxSprite clx)
{
	const bool usable = item._iStatFlag;
	if (usable) {
		ClxDraw(out, position, clx);
	} else {
		ClxDrawTRN(out, position, clx, GetInfravisionTRN());
	}
}

void ResetCursor()
{
	NewCursor(pcurs);
}

void NewCursor(const Item &item)
{
	if (item.isEmpty()) {
		NewCursor(CURSOR_HAND);
	} else {
		NewCursor(item._iCurs + CURSOR_FIRSTITEM);
	}
}

void NewCursor(int cursId)
{
	if (pcurs >= CURSOR_FIRSTITEM && cursId > CURSOR_HAND && cursId < CURSOR_HOURGLASS) {
		if (!TryDropItem()) {
			return;
		}
	}

	if (cursId < CURSOR_HOURGLASS && MyPlayer != nullptr) {
		MyPlayer->HoldItem.clear();
	}
	pcurs = cursId;

	if (IsHardwareCursorEnabled() && ControlDevice == ControlTypes::KeyboardAndMouse) {
		if (!ArtCursor && cursId == CURSOR_NONE)
			return;

		const CursorInfo newCursor = ArtCursor
		    ? CursorInfo::UserInterfaceCursor()
		    : CursorInfo::GameCursor(cursId);
		if (newCursor != GetCurrentCursorInfo())
			SetHardwareCursor(newCursor);
	}
}

void DrawSoftwareCursor(const Surface &out, Point position, int cursId)
{
	const ClxSprite sprite = GetInvItemSprite(cursId);
	if (!MyPlayer->HoldItem.isEmpty()) {
		const auto &heldItem = MyPlayer->HoldItem;
		ClxDrawOutline(out, GetOutlineColor(heldItem, true), position, sprite);
		DrawItem(heldItem, out, position, sprite);
	} else {
		ClxDraw(out, position, sprite);
	}
}

void InitLevelCursor()
{
	NewCursor(CURSOR_HAND);
	cursPosition = ViewPosition;
	pcurstemp = -1;
	pcursmonst = -1;
	ObjectUnderCursor = nullptr;
	pcursitem = -1;
	pcursstashitem = uint16_t(-1);
	pcursplr = -1;
	ClearCursor();
}

void CheckTown()
{
	for (auto &missile : Missiles) {
		if (missile._mitype == MIS_TOWN) {
			if (EntranceBoundaryContains(missile.position.tile, cursPosition)) {
				trigflag = true;
				ClearPanel();
				InfoString = _("Town Portal");
				AddPanelString(fmt::format(fmt::runtime(_("from {:s}")), Players[missile._misource]._pName));
				cursPosition = missile.position.tile;
			}
		}
	}
}

void CheckRportal()
{
	for (auto &missile : Missiles) {
		if (missile._mitype == MIS_RPORTAL) {
			if (EntranceBoundaryContains(missile.position.tile, cursPosition)) {
				trigflag = true;
				ClearPanel();
				InfoString = _("Portal to");
				AddPanelString(!setlevel ? _("The Unholy Altar") : _("level 15"));
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
		if (IsLeftPanelOpen()) {
			sx -= GetScreenWidth() / 4;
		} else if (IsRightPanelOpen()) {
			sx += GetScreenWidth() / 4;
		}
	}
	const Rectangle &mainPanel = GetMainPanel();
	if (mainPanel.contains(MousePosition) && track_isscrolling()) {
		sy = mainPanel.position.y - 1;
	}

	if (*sgOptions.Graphics.zoom) {
		sx /= 2;
		sy /= 2;
	}

	// Adjust by player offset and tile grid alignment
	int xo = 0;
	int yo = 0;
	CalcTileOffset(&xo, &yo);
	sx += xo;
	sy += yo;

	const Player &myPlayer = *MyPlayer;

	if (myPlayer.IsWalking()) {
		Displacement offset = GetOffsetForWalking(myPlayer.AnimInfo, myPlayer._pdir, true);
		sx -= offset.deltaX;
		sy -= offset.deltaY;

		// Predict the next frame when walking to avoid input jitter
		DisplacementOf<int16_t> offset2 = myPlayer.position.CalculateWalkingOffsetShifted8(myPlayer._pdir, myPlayer.AnimInfo);
		DisplacementOf<int16_t> velocity = myPlayer.position.GetWalkingVelocityShifted8(myPlayer._pdir, myPlayer.AnimInfo);
		int fx = offset2.deltaX / 256;
		int fy = offset2.deltaY / 256;
		fx -= (offset2.deltaX + velocity.deltaX) / 256;
		fy -= (offset2.deltaY + velocity.deltaY) / 256;

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

	if (*sgOptions.Graphics.zoom) {
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

	const Point currentTile { mx, my };

	// While holding the button down we should retain target (but potentially lose it if it dies, goes out of view, etc)
	if ((sgbMouseDown != CLICK_NONE || ControllerButtonHeld != ControllerButton_NONE) && IsNoneOf(LastMouseButtonAction, MouseActionType::None, MouseActionType::Attack, MouseActionType::Spell)) {
		InvalidateTargets();

		if (pcursmonst == -1 && ObjectUnderCursor == nullptr && pcursitem == -1 && pcursinvitem == -1 && pcursstashitem == uint16_t(-1) && pcursplr == -1) {
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
	ObjectUnderCursor = nullptr;
	pcursitem = -1;
	if (pcursinvitem != -1) {
		drawsbarflag = true;
	}
	pcursinvitem = -1;
	pcursstashitem = uint16_t(-1);
	pcursplr = -1;
	ShowUniqueItemInfoBox = false;
	panelflag = false;
	trigflag = false;

	if (myPlayer._pInvincible) {
		return;
	}
	if (!myPlayer.HoldItem.isEmpty() || spselflag) {
		cursPosition = { mx, my };
		return;
	}
	if (mainPanel.contains(MousePosition)) {
		CheckPanelInfo();
		return;
	}
	if (DoomFlag) {
		return;
	}
	if (invflag && GetRightPanel().contains(MousePosition)) {
		pcursinvitem = CheckInvHLight();
		return;
	}
	if (IsStashOpen && GetLeftPanel().contains(MousePosition)) {
		pcursstashitem = CheckStashHLight(MousePosition);
	}
	if (sbookflag && GetRightPanel().contains(MousePosition)) {
		return;
	}
	if (IsLeftPanelOpen() && GetLeftPanel().contains(MousePosition)) {
		return;
	}

	if (leveltype != DTYPE_TOWN) {
		if (pcurstemp != -1) {
			if (!flipflag && mx + 2 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 2][my + 1] != 0 && IsTileLit({ mx + 2, my + 1 })) {
				const uint16_t monsterId = abs(dMonster[mx + 2][my + 1]) - 1;
				if (monsterId == pcurstemp && Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 4) != 0) {
					cursPosition = Point { mx, my } + Displacement { 2, 1 };
					pcursmonst = monsterId;
				}
			}
			if (flipflag && mx + 1 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 1][my + 2] != 0 && IsTileLit({ mx + 1, my + 2 })) {
				const uint16_t monsterId = abs(dMonster[mx + 1][my + 2]) - 1;
				if (monsterId == pcurstemp && Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 4) != 0) {
					cursPosition = Point { mx, my } + Displacement { 1, 2 };
					pcursmonst = monsterId;
				}
			}
			if (mx + 2 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 2][my + 2] != 0 && IsTileLit({ mx + 2, my + 2 })) {
				const uint16_t monsterId = abs(dMonster[mx + 2][my + 2]) - 1;
				if (monsterId == pcurstemp && Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 4) != 0) {
					cursPosition = Point { mx, my } + Displacement { 2, 2 };
					pcursmonst = monsterId;
				}
			}
			if (mx + 1 < MAXDUNX && !flipflag && dMonster[mx + 1][my] != 0 && IsTileLit({ mx + 1, my })) {
				const uint16_t monsterId = abs(dMonster[mx + 1][my]) - 1;
				if (monsterId == pcurstemp && Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 2) != 0) {
					cursPosition = Point { mx, my } + Displacement { 1, 0 };
					pcursmonst = monsterId;
				}
			}
			if (my + 1 < MAXDUNY && flipflag && dMonster[mx][my + 1] != 0 && IsTileLit({ mx, my + 1 })) {
				const uint16_t monsterId = abs(dMonster[mx][my + 1]) - 1;
				if (monsterId == pcurstemp && Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 2) != 0) {
					cursPosition = Point { mx, my } + Displacement { 0, 1 };
					pcursmonst = monsterId;
				}
			}
			if (dMonster[mx][my] != 0 && IsTileLit({ mx, my })) {
				const uint16_t monsterId = abs(dMonster[mx][my]) - 1;
				if (monsterId == pcurstemp && Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 1) != 0) {
					cursPosition = { mx, my };
					pcursmonst = monsterId;
				}
			}
			if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 1][my + 1] != 0 && IsTileLit({ mx + 1, my + 1 })) {
				const uint16_t monsterId = abs(dMonster[mx + 1][my + 1]) - 1;
				if (monsterId == pcurstemp && Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 2) != 0) {
					cursPosition = Point { mx, my } + Displacement { 1, 1 };
					pcursmonst = monsterId;
				}
			}
			if (pcursmonst != -1 && (Monsters[pcursmonst].flags & MFLAG_HIDDEN) != 0) {
				pcursmonst = -1;
				cursPosition = { mx, my };
			}
			if (pcursmonst != -1 && Monsters[pcursmonst].isPlayerMinion()) {
				pcursmonst = -1;
			}
			if (pcursmonst != -1) {
				return;
			}
		}
		if (!flipflag && mx + 2 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 2][my + 1] != 0 && IsTileLit({ mx + 2, my + 1 })) {
			int monsterId = abs(dMonster[mx + 2][my + 1]) - 1;
			if (Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 4) != 0) {
				cursPosition = Point { mx, my } + Displacement { 2, 1 };
				pcursmonst = monsterId;
			}
		}
		if (flipflag && mx + 1 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 1][my + 2] != 0 && IsTileLit({ mx + 1, my + 2 })) {
			const uint16_t monsterId = abs(dMonster[mx + 1][my + 2]) - 1;
			if (Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 4) != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 2 };
				pcursmonst = monsterId;
			}
		}
		if (mx + 2 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 2][my + 2] != 0 && IsTileLit({ mx + 2, my + 2 })) {
			const uint16_t monsterId = abs(dMonster[mx + 2][my + 2]) - 1;
			if (Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 4) != 0) {
				cursPosition = Point { mx, my } + Displacement { 2, 2 };
				pcursmonst = monsterId;
			}
		}
		if (!flipflag && mx + 1 < MAXDUNX && dMonster[mx + 1][my] != 0 && IsTileLit({ mx + 1, my })) {
			const uint16_t monsterId = abs(dMonster[mx + 1][my]) - 1;
			if (Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 2) != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 0 };
				pcursmonst = monsterId;
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dMonster[mx][my + 1] != 0 && IsTileLit({ mx, my + 1 })) {
			const uint16_t monsterId = abs(dMonster[mx][my + 1]) - 1;
			if (Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 2) != 0) {
				cursPosition = Point { mx, my } + Displacement { 0, 1 };
				pcursmonst = monsterId;
			}
		}
		if (dMonster[mx][my] != 0 && IsTileLit({ mx, my })) {
			const uint16_t monsterId = abs(dMonster[mx][my]) - 1;
			if (Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 1) != 0) {
				cursPosition = { mx, my };
				pcursmonst = monsterId;
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 1][my + 1] != 0 && IsTileLit({ mx + 1, my + 1 })) {
			const uint16_t monsterId = abs(dMonster[mx + 1][my + 1]) - 1;
			if (Monsters[monsterId].hitPoints >> 6 > 0 && (Monsters[monsterId].data().selectionType & 2) != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 1 };
				pcursmonst = monsterId;
			}
		}
		if (pcursmonst != -1 && (Monsters[pcursmonst].flags & MFLAG_HIDDEN) != 0) {
			pcursmonst = -1;
			cursPosition = { mx, my };
		}
		if (pcursmonst != -1 && (Monsters[pcursmonst].isPlayerMinion() || IsAnyOf(pcurs, CURSOR_HEALOTHER, CURSOR_RESURRECT))) {
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
			const uint8_t playerId = abs(dPlayer[mx + 1][my]) - 1;
			Player &player = Players[playerId];
			if (&player != MyPlayer && player._pHitPoints != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 0 };
				pcursplr = static_cast<int8_t>(playerId);
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dPlayer[mx][my + 1] != 0) {
			const uint8_t playerId = abs(dPlayer[mx][my + 1]) - 1;
			Player &player = Players[playerId];
			if (&player != MyPlayer && player._pHitPoints != 0) {
				cursPosition = Point { mx, my } + Displacement { 0, 1 };
				pcursplr = static_cast<int8_t>(playerId);
			}
		}
		if (dPlayer[mx][my] != 0) {
			const uint8_t playerId = abs(dPlayer[mx][my]) - 1;
			if (playerId != MyPlayerId) {
				cursPosition = { mx, my };
				pcursplr = static_cast<int8_t>(playerId);
			}
		}
		if (TileContainsDeadPlayer({ mx, my })) {
			for (const Player &player : Players) {
				if (player.position.tile == Point { mx, my } && &player != MyPlayer) {
					cursPosition = { mx, my };
					pcursplr = static_cast<int8_t>(player.getId());
				}
			}
		}
		if (pcurs == CURSOR_RESURRECT) {
			for (int xx = -1; xx < 2; xx++) {
				for (int yy = -1; yy < 2; yy++) {
					if (TileContainsDeadPlayer({ mx + xx, my + yy })) {
						for (const Player &player : Players) {
							if (player.position.tile.x == mx + xx && player.position.tile.y == my + yy && &player != MyPlayer) {
								cursPosition = Point { mx, my } + Displacement { xx, yy };
								pcursplr = static_cast<int8_t>(player.getId());
							}
						}
					}
				}
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dPlayer[mx + 1][my + 1] != 0) {
			const uint8_t playerId = abs(dPlayer[mx + 1][my + 1]) - 1;
			const Player &player = Players[playerId];
			if (&player != MyPlayer && player._pHitPoints != 0) {
				cursPosition = Point { mx, my } + Displacement { 1, 1 };
				pcursplr = static_cast<int8_t>(playerId);
			}
		}
	}
	if (pcursmonst == -1 && pcursplr == -1) {
		// No monsters or players under the cursor, try find an object starting with the tile below the current tile (tall
		//  objects like doors)
		Point testPosition = currentTile + Direction::South;
		Object *object = FindObjectAtPosition(testPosition);

		if (object == nullptr || object->_oSelFlag < 2) {
			// Either no object or can't interact from the test position, try the current tile
			testPosition = currentTile;
			object = FindObjectAtPosition(testPosition);

			if (object == nullptr || IsNoneOf(object->_oSelFlag, 1, 3)) {
				// Still no object (that could be activated from this position), try the tile to the bottom left or right
				//  (whichever is closest to the cursor as determined when we set flipflag earlier)
				testPosition = currentTile + (flipflag ? Direction::SouthWest : Direction::SouthEast);
				object = FindObjectAtPosition(testPosition);

				if (object != nullptr && object->_oSelFlag < 2) {
					// Found an object but it's not in range, clear the pointer
					object = nullptr;
				}
			}
		}
		if (object != nullptr) {
			// found object that can be activated with the given cursor position
			cursPosition = testPosition;
			ObjectUnderCursor = object;
		}
	}
	if (pcursplr == -1 && ObjectUnderCursor == nullptr && pcursmonst == -1) {
		if (!flipflag && mx + 1 < MAXDUNX && dItem[mx + 1][my] > 0) {
			const uint8_t itemId = dItem[mx + 1][my] - 1;
			if (Items[itemId]._iSelFlag >= 2) {
				cursPosition = Point { mx, my } + Displacement { 1, 0 };
				pcursitem = static_cast<int8_t>(itemId);
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dItem[mx][my + 1] > 0) {
			const uint8_t itemId = dItem[mx][my + 1] - 1;
			if (Items[itemId]._iSelFlag >= 2) {
				cursPosition = Point { mx, my } + Displacement { 0, 1 };
				pcursitem = static_cast<int8_t>(itemId);
			}
		}
		if (dItem[mx][my] > 0) {
			const uint8_t itemId = dItem[mx][my] - 1;
			if (Items[itemId]._iSelFlag == 1 || Items[itemId]._iSelFlag == 3) {
				cursPosition = { mx, my };
				pcursitem = static_cast<int8_t>(itemId);
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dItem[mx + 1][my + 1] > 0) {
			const uint8_t itemId = dItem[mx + 1][my + 1] - 1;
			if (Items[itemId]._iSelFlag >= 2) {
				cursPosition = Point { mx, my } + Displacement { 1, 1 };
				pcursitem = static_cast<int8_t>(itemId);
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
		ObjectUnderCursor = nullptr;
		pcursmonst = -1;
		pcursitem = -1;
		cursPosition = { mx, my };
	}
	if (pcursmonst != -1 && leveltype != DTYPE_TOWN && Monsters[pcursmonst].isPlayerMinion()) {
		pcursmonst = -1;
	}
}

} // namespace devilution
