/**
 * @file inv.cpp
 *
 * Implementation of player inventory.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL invflag;
BYTE *pInvCels;
BOOL drawsbarflag;
int sgdwLastTime; // check name

/**
 * Maps from inventory slot to screen position. The inventory slots are
 * arranged as follows:
 *                          00 01
 *                          02 03   06
 *              07 08       19 20       13 14
 *              09 10       21 22       15 16
 *              11 12       23 24       17 18
 *                 04                   05
 *              25 26 27 28 29 30 31 32 33 34
 *              35 36 37 38 39 40 41 42 43 44
 *              45 46 47 48 49 50 51 52 53 54
 *              55 56 57 58 59 60 61 62 63 64
 * 65 66 67 68 69 70 71 72
 * @see graphics/inv/inventory.png
 */
const InvXY InvRect[] = {
	// clang-format off
	//  X,   Y
	{ 132,  31 }, // helmet
	{ 160,  31 }, // helmet
	{ 132,  59 }, // helmet
	{ 160,  59 }, // helmet
	{  45, 205 }, // left ring
	{ 247, 205 }, // right ring
	{ 204,  59 }, // amulet
	{  17, 104 }, // left hand
	{  46, 104 }, // left hand
	{  17, 132 }, // left hand
	{  46, 132 }, // left hand
	{  17, 160 }, // left hand
	{  46, 160 }, // left hand
	{ 247, 104 }, // right hand
	{ 276, 104 }, // right hand
	{ 247, 132 }, // right hand
	{ 276, 132 }, // right hand
	{ 247, 160 }, // right hand
	{ 276, 160 }, // right hand
	{ 132, 104 }, // chest
	{ 160, 104 }, // chest
	{ 132, 132 }, // chest
	{ 160, 132 }, // chest
	{ 132, 160 }, // chest
	{ 160, 160 }, // chest
	{  17, 250 }, // inv row 1
	{  46, 250 }, // inv row 1
	{  75, 250 }, // inv row 1
	{ 104, 250 }, // inv row 1
	{ 133, 250 }, // inv row 1
	{ 162, 250 }, // inv row 1
	{ 191, 250 }, // inv row 1
	{ 220, 250 }, // inv row 1
	{ 249, 250 }, // inv row 1
	{ 278, 250 }, // inv row 1
	{  17, 279 }, // inv row 2
	{  46, 279 }, // inv row 2
	{  75, 279 }, // inv row 2
	{ 104, 279 }, // inv row 2
	{ 133, 279 }, // inv row 2
	{ 162, 279 }, // inv row 2
	{ 191, 279 }, // inv row 2
	{ 220, 279 }, // inv row 2
	{ 249, 279 }, // inv row 2
	{ 278, 279 }, // inv row 2
	{  17, 308 }, // inv row 3
	{  46, 308 }, // inv row 3
	{  75, 308 }, // inv row 3
	{ 104, 308 }, // inv row 3
	{ 133, 308 }, // inv row 3
	{ 162, 308 }, // inv row 3
	{ 191, 308 }, // inv row 3
	{ 220, 308 }, // inv row 3
	{ 249, 308 }, // inv row 3
	{ 278, 308 }, // inv row 3
	{  17, 337 }, // inv row 4
	{  46, 337 }, // inv row 4
	{  75, 337 }, // inv row 4
	{ 104, 337 }, // inv row 4
	{ 133, 337 }, // inv row 4
	{ 162, 337 }, // inv row 4
	{ 191, 337 }, // inv row 4
	{ 220, 337 }, // inv row 4
	{ 249, 337 }, // inv row 4
	{ 278, 337 }, // inv row 4
	{ 205,  33 }, // belt
	{ 234,  33 }, // belt
	{ 263,  33 }, // belt
	{ 292,  33 }, // belt
	{ 321,  33 }, // belt
	{ 350,  33 }, // belt
	{ 379,  33 }, // belt
	{ 408,  33 }  // belt
	// clang-format on
};

/* data */
/** Specifies the starting inventory slots for placement of 2x2 items. */
int AP2x2Tbl[10] = { 8, 28, 6, 26, 4, 24, 2, 22, 0, 20 };

void FreeInvGFX()
{
	MemFreeDbg(pInvCels);
}

void InitInv()
{
	if (plr[myplr]._pClass == PC_WARRIOR) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv.CEL", NULL);
	} else if (plr[myplr]._pClass == PC_ROGUE) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_rog.CEL", NULL);
	} else if (plr[myplr]._pClass == PC_SORCERER) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_Sor.CEL", NULL);
	} else if (plr[myplr]._pClass == PC_MONK) {
		if (!gbIsSpawn)
			pInvCels = LoadFileInMem("Data\\Inv\\Inv_Sor.CEL", NULL);
		else
			pInvCels = LoadFileInMem("Data\\Inv\\Inv.CEL", NULL);
	} else if (plr[myplr]._pClass == PC_BARD) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_rog.CEL", NULL);
	} else if (plr[myplr]._pClass == PC_BARBARIAN) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv.CEL", NULL);
	}

	invflag = FALSE;
	drawsbarflag = FALSE;
}

void InvDrawSlotBack(int X, int Y, int W, int H)
{
	BYTE *dst;

	assert(gpBuffer);

	dst = &gpBuffer[X + BUFFER_WIDTH * Y];

	int wdt, hgt;
	BYTE pix;

	for (hgt = H; hgt; hgt--, dst -= BUFFER_WIDTH + W) {
		for (wdt = W; wdt; wdt--) {
			pix = *dst;
			if (pix >= PAL16_BLUE) {
				if (pix <= PAL16_BLUE + 15)
					pix -= PAL16_BLUE - PAL16_BEIGE;
				else if (pix >= PAL16_GRAY)
					pix -= PAL16_GRAY - PAL16_BEIGE;
			}
			*dst++ = pix;
		}
	}
}

/**
 * @brief Render the inventory panel to the back buffer
 */
void DrawInv()
{
	BOOL invtest[NUM_INV_GRID_ELEM];
	int frame, frame_width, color, screen_x, screen_y, i, j, ii;
	BYTE *pBuff;

	CelDraw(RIGHT_PANEL_X, 351 + SCREEN_Y, pInvCels, 1, SPANEL_WIDTH);

	if (plr[myplr].InvBody[INVLOC_HEAD]._itype != ITYPE_NONE) {
		InvDrawSlotBack(RIGHT_PANEL_X + 133, 59 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 2 * INV_SLOT_SIZE_PX);

		frame = plr[myplr].InvBody[INVLOC_HEAD]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_HEAD) {
			color = ICOL_WHITE;
			if (plr[myplr].InvBody[INVLOC_HEAD]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!plr[myplr].InvBody[INVLOC_HEAD]._iStatFlag) {
				color = ICOL_RED;
			}
			if (frame <= 179) {
				CelBlitOutline(color, RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		}

		if (plr[myplr].InvBody[INVLOC_HEAD]._iStatFlag) {
			if (frame <= 179) {
				CelClippedDraw(RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		} else {
			if (frame <= 179) {
				CelDrawLightRed(RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels, frame, frame_width, 1);
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
		}
	}

	if (plr[myplr].InvBody[INVLOC_RING_LEFT]._itype != ITYPE_NONE) {
		InvDrawSlotBack(RIGHT_PANEL_X + 48, 205 + SCREEN_Y, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);

		frame = plr[myplr].InvBody[INVLOC_RING_LEFT]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_RING_LEFT) {
			color = ICOL_WHITE;
			if (plr[myplr].InvBody[INVLOC_RING_LEFT]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!plr[myplr].InvBody[INVLOC_RING_LEFT]._iStatFlag) {
				color = ICOL_RED;
			}
			if (frame <= 179) {
				CelBlitOutline(color, RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		}

		if (plr[myplr].InvBody[INVLOC_RING_LEFT]._iStatFlag) {
			if (frame <= 179) {
				CelClippedDraw(RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		} else {
			if (frame <= 179) {
				CelDrawLightRed(RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels, frame, frame_width, 1);
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
		}
	}

	if (plr[myplr].InvBody[INVLOC_RING_RIGHT]._itype != ITYPE_NONE) {
		InvDrawSlotBack(RIGHT_PANEL_X + 249, 205 + SCREEN_Y, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);

		frame = plr[myplr].InvBody[INVLOC_RING_RIGHT]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_RING_RIGHT) {
			color = ICOL_WHITE;
			if (plr[myplr].InvBody[INVLOC_RING_RIGHT]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!plr[myplr].InvBody[INVLOC_RING_RIGHT]._iStatFlag) {
				color = ICOL_RED;
			}
			if (frame <= 179) {
				CelBlitOutline(color, RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		}

		if (plr[myplr].InvBody[INVLOC_RING_RIGHT]._iStatFlag) {
			if (frame <= 179) {
				CelClippedDraw(RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		} else {
			if (frame <= 179) {
				CelDrawLightRed(RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels, frame, frame_width, 1);
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
		}
	}

	if (plr[myplr].InvBody[INVLOC_AMULET]._itype != ITYPE_NONE) {
		InvDrawSlotBack(RIGHT_PANEL_X + 205, 60 + SCREEN_Y, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);

		frame = plr[myplr].InvBody[INVLOC_AMULET]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_AMULET) {
			color = ICOL_WHITE;
			if (plr[myplr].InvBody[INVLOC_AMULET]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!plr[myplr].InvBody[INVLOC_AMULET]._iStatFlag) {
				color = ICOL_RED;
			}
			if (frame <= 179) {
				CelBlitOutline(color, RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		}

		if (plr[myplr].InvBody[INVLOC_AMULET]._iStatFlag) {
			if (frame <= 179) {
				CelClippedDraw(RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		} else {
			if (frame <= 179) {
				CelDrawLightRed(RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels, frame, frame_width, 1);
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
		}
	}

	if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE) {
		InvDrawSlotBack(RIGHT_PANEL_X + 17, 160 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX);

		frame = plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];
		// calc item offsets for weapons smaller than 2x3 slots
		screen_x = frame_width == INV_SLOT_SIZE_PX ? (RIGHT_PANEL_X + 31) : (RIGHT_PANEL_X + 17);
		screen_y = InvItemHeight[frame] == (3 * INV_SLOT_SIZE_PX) ? (160 + SCREEN_Y) : (146 + SCREEN_Y);

		if (pcursinvitem == INVITEM_HAND_LEFT) {
			color = ICOL_WHITE;
			if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!plr[myplr].InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
				color = ICOL_RED;
			}
			if (frame <= 179) {
				CelBlitOutline(color, screen_x, screen_y, pCursCels, frame, frame_width);
			} else {
				CelBlitOutline(color, screen_x, screen_y, pCursCels2, frame - 179, frame_width);
			}
		}

		if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
			if (frame <= 179) {
				CelClippedDraw(screen_x, screen_y, pCursCels, frame, frame_width);
			} else {
				CelClippedDraw(screen_x, screen_y, pCursCels2, frame - 179, frame_width);
			}
		} else {
			if (frame <= 179) {
				CelDrawLightRed(screen_x, screen_y, pCursCels, frame, frame_width, 1);
			} else {
				CelDrawLightRed(screen_x, screen_y, pCursCels2, frame - 179, frame_width, 1);
			}
		}

		if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND) {
			if (plr[myplr]._pClass != PC_BARBARIAN
			    || plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_SWORD
			        && plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_MACE) {
				InvDrawSlotBack(RIGHT_PANEL_X + 248, 160 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX);
				light_table_index = 0;
				cel_transparency_active = TRUE;

				const int dst_x = RIGHT_PANEL_X + (frame_width == INV_SLOT_SIZE_PX ? 261 : 247);
				const int dst_y = SCREEN_Y + 160;
				if (frame <= 179) {
					CelClippedBlitLightTrans(dst_x, dst_y, pCursCels, frame, frame_width);
				} else {
					CelClippedBlitLightTrans(dst_x, dst_y, pCursCels2, frame - 179, frame_width);
				}

				cel_transparency_active = FALSE;
			}
		}
	}
	if (plr[myplr].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE) {
		InvDrawSlotBack(RIGHT_PANEL_X + 248, 160 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX);

		frame = plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];
		// calc item offsets for weapons smaller than 2x3 slots
		screen_x = frame_width == INV_SLOT_SIZE_PX ? (RIGHT_PANEL_X + 261) : (RIGHT_PANEL_X + 249);
		screen_y = InvItemHeight[frame] == 3 * INV_SLOT_SIZE_PX ? (160 + SCREEN_Y) : (146 + SCREEN_Y);

		if (pcursinvitem == INVITEM_HAND_RIGHT) {
			color = ICOL_WHITE;
			if (plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
				color = ICOL_RED;
			}
			if (frame <= 179) {
				CelBlitOutline(color, screen_x, screen_y, pCursCels, frame, frame_width);
			} else {
				CelBlitOutline(color, screen_x, screen_y, pCursCels2, frame - 179, frame_width);
			}
		}

		if (plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
			if (frame <= 179) {
				CelClippedDraw(screen_x, screen_y, pCursCels, frame, frame_width);
			} else {
				CelClippedDraw(screen_x, screen_y, pCursCels2, frame - 179, frame_width);
			}
		} else {
			if (frame <= 179) {
				CelDrawLightRed(screen_x, screen_y, pCursCels, frame, frame_width, 1);
			} else {
				CelDrawLightRed(screen_x, screen_y, pCursCels2, frame - 179, frame_width, 1);
			}
		}
	}

	if (plr[myplr].InvBody[INVLOC_CHEST]._itype != ITYPE_NONE) {
		InvDrawSlotBack(RIGHT_PANEL_X + 133, 160 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX);

		frame = plr[myplr].InvBody[INVLOC_CHEST]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == INVITEM_CHEST) {
			color = ICOL_WHITE;
			if (plr[myplr].InvBody[INVLOC_CHEST]._iMagical != ITEM_QUALITY_NORMAL) {
				color = ICOL_BLUE;
			}
			if (!plr[myplr].InvBody[INVLOC_CHEST]._iStatFlag) {
				color = ICOL_RED;
			}
			if (frame <= 179) {
				CelBlitOutline(color, RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		}

		if (plr[myplr].InvBody[INVLOC_CHEST]._iStatFlag) {
			if (frame <= 179) {
				CelClippedDraw(RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels, frame, frame_width);
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
		} else {
			if (frame <= 179) {
				CelDrawLightRed(RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels, frame, frame_width, 1);
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
		}
	}

	for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
		invtest[i] = FALSE;
		if (plr[myplr].InvGrid[i] != 0) {
			InvDrawSlotBack(
			    InvRect[i + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
			    InvRect[i + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
			    INV_SLOT_SIZE_PX,
			    INV_SLOT_SIZE_PX);
		}
	}

	for (j = 0; j < NUM_INV_GRID_ELEM; j++) {
		if (plr[myplr].InvGrid[j] > 0) // first slot of an item
		{
			ii = plr[myplr].InvGrid[j] - 1;

			invtest[j] = TRUE;

			frame = plr[myplr].InvList[ii]._iCurs + CURSOR_FIRSTITEM;
			frame_width = InvItemWidth[frame];
			if (pcursinvitem == ii + INVITEM_INV_FIRST) {
				color = ICOL_WHITE;
				if (plr[myplr].InvList[ii]._iMagical != ITEM_QUALITY_NORMAL) {
					color = ICOL_BLUE;
				}
				if (!plr[myplr].InvList[ii]._iStatFlag) {
					color = ICOL_RED;
				}
				if (frame <= 179) {
					CelBlitOutline(
					    color,
					    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
					    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
					    pCursCels, frame, frame_width);
				} else {
					CelBlitOutline(
					    color,
					    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
					    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
					    pCursCels2, frame - 179, frame_width);
				}
			}

			if (plr[myplr].InvList[ii]._iStatFlag) {
				if (frame <= 179) {
					CelClippedDraw(
					    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
					    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
					    pCursCels, frame, frame_width);
				} else {
					CelClippedDraw(
					    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
					    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
					    pCursCels2, frame - 179, frame_width);
				}
			} else {
				if (frame <= 179) {
					CelDrawLightRed(
					    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
					    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
					    pCursCels, frame, frame_width, 1);
				} else {
					CelDrawLightRed(
					    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
					    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
					    pCursCels2, frame - 179, frame_width, 1);
				}
			}
		}
	}
}

void DrawInvBelt(CelOutputBuffer out)
{
	int i, frame, frame_width, color;
	BYTE fi, ff;

	if (talkflag) {
		return;
	}

	DrawPanelBox(out, 205, 21, 232, 28, PANEL_X + 205, PANEL_Y + 5);

	for (i = 0; i < MAXBELTITEMS; i++) {
		if (plr[myplr].SpdList[i]._itype == ITYPE_NONE) {
			continue;
		}

		InvDrawSlotBack(InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);
		frame = plr[myplr].SpdList[i]._iCurs + CURSOR_FIRSTITEM;
		frame_width = InvItemWidth[frame];

		if (pcursinvitem == i + INVITEM_BELT_FIRST) {
			color = ICOL_WHITE;
			if (plr[myplr].SpdList[i]._iMagical)
				color = ICOL_BLUE;
			if (!plr[myplr].SpdList[i]._iStatFlag)
				color = ICOL_RED;
			if (!sgbControllerActive || invflag) {
				if (frame <= 179)
					CelBlitOutlineTo(out, color, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels, frame, frame_width);
				else
					CelBlitOutlineTo(out, color, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels2, frame - 179, frame_width);
			}
		}

		if (plr[myplr].SpdList[i]._iStatFlag) {
			if (frame <= 179)
				CelClippedDrawTo(out, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels, frame, frame_width);
			else
				CelClippedDrawTo(out, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels2, frame - 179, frame_width);
		} else {
			if (frame <= 179)
				CelDrawLightRedTo(out, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels, frame, frame_width, 1);
			else
				CelDrawLightRedTo(out, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels2, frame - 179, frame_width, 1);
		}

		if (AllItemsList[plr[myplr].SpdList[i].IDidx].iUsable
		    && plr[myplr].SpdList[i]._iStatFlag
		    && plr[myplr].SpdList[i]._itype != ITYPE_GOLD) {
			fi = i + 49;
			ff = fontframe[gbFontTransTbl[fi]];
			PrintChar(out, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X + INV_SLOT_SIZE_PX - fontkern[ff], InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, ff, 0);
		}
	}
}

/**
 * @brief Gets the size, in inventory cells, of the given item.
 * @param item The item whose size is to be determined.
 * @return The size, in inventory cells, of the item.
 */
InvXY GetInventorySize(const ItemStruct &item)
{
	int itemSizeIndex = item._iCurs + CURSOR_FIRSTITEM;

	return {
		InvItemWidth[itemSizeIndex] / INV_SLOT_SIZE_PX,
		InvItemHeight[itemSizeIndex] / INV_SLOT_SIZE_PX,
	};
}

/**
 * @brief Checks whether the given item can fit in a belt slot (i.e. the item's size in inventory cells is 1x1).
 * @param item The item to be checked.
 * @return 'True' in case the item can fit a belt slot and 'False' otherwise.
 */
bool FitsInBeltSlot(const ItemStruct &item)
{
	InvXY size = GetInventorySize(item);

	return size.X == 1 && size.Y == 1;
}

/**
 * @brief Checks whether the given item can be placed on the belt. Takes item size as well as characteristics into account. Items
 * that cannot be placed on the belt have to be placed in the inventory instead.
 * @param item The item to be checked.
 * @return 'True' in case the item can be placed on the belt and 'False' otherwise.
 */
bool CanBePlacedOnBelt(const ItemStruct &item)
{
	return FitsInBeltSlot(item)
	    && item._itype != ITYPE_GOLD
	    && item._iStatFlag
	    && AllItemsList[item.IDidx].iUsable;
}

/**
 * @brief Checks whether the given item can be placed on the specified player's belt. Returns 'True' when the item can be placed
 * on belt slots and the player has at least one empty slot in his belt.
 * @param playerNumber The player number on whose belt will be checked.
 * @param item The item to be checked.
 * @return 'True' in case the item can be placed on the player's belt and 'False' otherwise.
 */
bool CanBePlacedOnBelt(int playerNumber, const ItemStruct &item)
{
	if (!CanBePlacedOnBelt(item)) {
		return false;
	}

	for (int i = 0; i < MAXBELTITEMS; i++) {
		if (plr[playerNumber].SpdList[i]._itype == ITYPE_NONE) {
			return true;
		}
	}

	return false;
}

BOOL AutoPlace(int pnum, int ii, int sx, int sy, BOOL saveflag)
{
	int i, j, xx, yy;
	BOOL done;

	done = TRUE;
	yy = 10 * (ii / 10);
	if (yy < 0) {
		yy = 0;
	}
	for (j = 0; j < sy && done; j++) {
		if (yy >= NUM_INV_GRID_ELEM) {
			done = FALSE;
		}
		xx = ii % 10;
		if (xx < 0) {
			xx = 0;
		}
		for (i = 0; i < sx && done; i++) {
			if (xx >= 10) {
				done = FALSE;
			} else {
				done = plr[pnum].InvGrid[xx + yy] == 0;
			}
			xx++;
		}
		yy += 10;
	}
	if (done && saveflag) {
		plr[pnum].InvList[plr[pnum]._pNumInv] = plr[pnum].HoldItem;
		plr[pnum]._pNumInv++;
		yy = 10 * (ii / 10);
		if (yy < 0) {
			yy = 0;
		}
		for (j = 0; j < sy; j++) {
			xx = ii % 10;
			if (xx < 0) {
				xx = 0;
			}
			for (i = 0; i < sx; i++) {
				if (i != 0 || j != sy - 1) {
					plr[pnum].InvGrid[xx + yy] = -plr[pnum]._pNumInv;
				} else {
					plr[pnum].InvGrid[xx + yy] = plr[pnum]._pNumInv;
				}
				xx++;
			}
			yy += 10;
		}
		CalcPlrScrolls(pnum);
	}
	return done;
}

BOOL SpecialAutoPlace(int pnum, int ii, const ItemStruct &item)
{
	int i, j, xx, yy;
	BOOL done;

	done = TRUE;
	yy = 10 * (ii / 10);
	if (yy < 0) {
		yy = 0;
	}

	InvXY itemSize = GetInventorySize(item);
	for (j = 0; j < itemSize.Y && done; j++) {
		if (yy >= NUM_INV_GRID_ELEM) {
			done = FALSE;
		}
		xx = ii % 10;
		if (xx < 0) {
			xx = 0;
		}
		for (i = 0; i < itemSize.X && done; i++) {
			if (xx >= 10) {
				done = FALSE;
			} else {
				done = plr[pnum].InvGrid[xx + yy] == 0;
			}
			xx++;
		}
		yy += 10;
	}
	if (!done) {
		done = CanBePlacedOnBelt(pnum, item);
	}

	return done;
}

BOOL GoldAutoPlace(int pnum)
{
	bool done = false;

	for (int i = 0; i < plr[pnum]._pNumInv && !done; i++) {
		if (plr[pnum].InvList[i]._itype != ITYPE_GOLD)
			continue;
		if (plr[pnum].InvList[i]._ivalue >= MaxGold)
			continue;

		plr[pnum].InvList[i]._ivalue += plr[pnum].HoldItem._ivalue;
		if (plr[pnum].InvList[i]._ivalue > MaxGold) {
			plr[pnum].HoldItem._ivalue = plr[pnum].InvList[i]._ivalue - MaxGold;
			SetPlrHandGoldCurs(&plr[pnum].HoldItem);
			plr[pnum].InvList[i]._ivalue = MaxGold;
			if (gbIsHellfire)
				GetPlrHandSeed(&plr[pnum].HoldItem);
		} else {
			plr[pnum].HoldItem._ivalue = 0;
			done = true;
		}

		SetPlrHandGoldCurs(&plr[pnum].InvList[i]);
		plr[pnum]._pGold = CalculateGold(pnum);
	}

	for (int i = 39; i >= 0 && !done; i--) {
		int yy = 10 * (i / 10);
		int xx = i % 10;
		if (plr[pnum].InvGrid[xx + yy] == 0) {
			int ii = plr[pnum]._pNumInv;
			plr[pnum].InvList[ii] = plr[pnum].HoldItem;
			plr[pnum]._pNumInv = plr[pnum]._pNumInv + 1;
			plr[pnum].InvGrid[xx + yy] = plr[pnum]._pNumInv;
			GetPlrHandSeed(&plr[pnum].InvList[ii]);
			int gold = plr[pnum].HoldItem._ivalue;
			if (gold > MaxGold) {
				gold -= MaxGold;
				plr[pnum].HoldItem._ivalue = gold;
				GetPlrHandSeed(&plr[pnum].HoldItem);
				plr[pnum].InvList[ii]._ivalue = MaxGold;
			} else {
				plr[pnum].HoldItem._ivalue = 0;
				done = true;
				plr[pnum]._pGold = CalculateGold(pnum);
				SetCursor_(CURSOR_HAND);
			}
		}
	}

	return done;
}

BOOL WeaponAutoPlace(int pnum)
{
	if (plr[pnum]._pClass == PC_MONK)
		return FALSE;
	if (plr[pnum].HoldItem._iLoc != ILOC_TWOHAND
	    || (plr[pnum]._pClass == PC_BARBARIAN && (plr[pnum].HoldItem._itype == ITYPE_SWORD || plr[pnum].HoldItem._itype == ITYPE_MACE))) {
		if (plr[pnum]._pClass != PC_BARD) {
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON)
				return FALSE;
			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON)
				return FALSE;
		}

		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE) {
			NetSendCmdChItem(TRUE, INVLOC_HAND_LEFT);
			plr[pnum].InvBody[INVLOC_HAND_LEFT] = plr[pnum].HoldItem;
			return TRUE;
		}
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iLoc != ILOC_TWOHAND) {
			NetSendCmdChItem(TRUE, INVLOC_HAND_RIGHT);
			plr[pnum].InvBody[INVLOC_HAND_RIGHT] = plr[pnum].HoldItem;
			return TRUE;
		}
	} else if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE) {
		NetSendCmdChItem(TRUE, INVLOC_HAND_LEFT);
		plr[pnum].InvBody[INVLOC_HAND_LEFT] = plr[pnum].HoldItem;
		return TRUE;
	}

	return FALSE;
}

int SwapItem(ItemStruct *a, ItemStruct *b)
{
	ItemStruct h;

	h = *a;
	*a = *b;
	*b = h;

	return h._iCurs + CURSOR_FIRSTITEM;
}

void CheckInvPaste(int pnum, int mx, int my)
{
	int r, sx, sy;
	int i, j, xx, yy, ii;
	BOOL done, done2h;
	int il, cn, it, iv, ig, gt;
	ItemStruct tempitem;

	SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
	i = mx + (icursW >> 1);
	j = my + (icursH >> 1);
	sx = icursW28;
	sy = icursH28;
	done = FALSE;
	for (r = 0; (DWORD)r < NUM_XY_SLOTS && !done; r++) {
		int xo = RIGHT_PANEL;
		int yo = 0;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = PANEL_LEFT;
			yo = PANEL_TOP;
		}

		if (i >= InvRect[r].X + xo && i < InvRect[r].X + xo + INV_SLOT_SIZE_PX) {
			if (j >= InvRect[r].Y + yo - INV_SLOT_SIZE_PX - 1 && j < InvRect[r].Y + yo) {
				done = TRUE;
				r--;
			}
		}
		if (r == SLOTXY_CHEST_LAST) {
			if ((sx & 1) == 0)
				i -= 14;
			if ((sy & 1) == 0)
				j -= 14;
		}
		if (r == SLOTXY_INV_LAST && (sy & 1) == 0)
			j += 14;
	}
	if (!done)
		return;
	il = ILOC_UNEQUIPABLE;
	if (r >= SLOTXY_HEAD_FIRST && r <= SLOTXY_HEAD_LAST)
		il = ILOC_HELM;
	if (r >= SLOTXY_RING_LEFT && r <= SLOTXY_RING_RIGHT)
		il = ILOC_RING;
	if (r == SLOTXY_AMULET)
		il = ILOC_AMULET;
	if (r >= SLOTXY_HAND_LEFT_FIRST && r <= SLOTXY_HAND_RIGHT_LAST)
		il = ILOC_ONEHAND;
	if (r >= SLOTXY_CHEST_FIRST && r <= SLOTXY_CHEST_LAST)
		il = ILOC_ARMOR;
	if (r >= SLOTXY_BELT_FIRST && r <= SLOTXY_BELT_LAST)
		il = ILOC_BELT;
	done = FALSE;
	if (plr[pnum].HoldItem._iLoc == il)
		done = TRUE;
	if (il == ILOC_ONEHAND && plr[pnum].HoldItem._iLoc == ILOC_TWOHAND) {
		if (plr[pnum]._pClass == PC_BARBARIAN
		    && (plr[pnum].HoldItem._itype == ITYPE_SWORD || plr[pnum].HoldItem._itype == ITYPE_MACE))
			il = ILOC_ONEHAND;
		else
			il = ILOC_TWOHAND;
		done = TRUE;
	}
	if (plr[pnum].HoldItem._iLoc == ILOC_UNEQUIPABLE && il == ILOC_BELT) {
		if (sx == 1 && sy == 1) {
			done = TRUE;
			if (!AllItemsList[plr[pnum].HoldItem.IDidx].iUsable)
				done = FALSE;
			if (!plr[pnum].HoldItem._iStatFlag)
				done = FALSE;
			if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
				done = FALSE;
		}
	}

	if (il == ILOC_UNEQUIPABLE) {
		done = TRUE;
		it = 0;
		ii = r - SLOTXY_INV_FIRST;
		if (plr[pnum].HoldItem._itype == ITYPE_GOLD) {
			yy = 10 * (ii / 10);
			xx = ii % 10;
			if (plr[pnum].InvGrid[xx + yy] != 0) {
				iv = plr[pnum].InvGrid[xx + yy];
				if (iv > 0) {
					if (plr[pnum].InvList[iv - 1]._itype != ITYPE_GOLD) {
						it = iv;
					}
				} else {
					it = -iv;
				}
			}
		} else {
			yy = 10 * ((ii / 10) - ((sy - 1) >> 1));
			if (yy < 0)
				yy = 0;
			for (j = 0; j < sy && done; j++) {
				if (yy >= NUM_INV_GRID_ELEM)
					done = FALSE;
				xx = (ii % 10) - ((sx - 1) >> 1);
				if (xx < 0)
					xx = 0;
				for (i = 0; i < sx && done; i++) {
					if (xx >= 10) {
						done = FALSE;
					} else {
						if (plr[pnum].InvGrid[xx + yy] != 0) {
							iv = plr[pnum].InvGrid[xx + yy];
							if (iv < 0)
								iv = -iv;
							if (it != 0) {
								if (it != iv)
									done = FALSE;
							} else
								it = iv;
						}
					}
					xx++;
				}
				yy += 10;
			}
		}
	}

	if (!done)
		return;

	if (il != ILOC_UNEQUIPABLE && il != ILOC_BELT && !plr[pnum].HoldItem._iStatFlag) {
		done = FALSE;
		if (plr[pnum]._pClass == PC_WARRIOR)
			PlaySFX(PS_WARR13);
		else if (plr[pnum]._pClass == PC_ROGUE)
			PlaySFX(PS_ROGUE13);
		else if (plr[pnum]._pClass == PC_SORCERER)
			PlaySFX(PS_MAGE13);
		else if (plr[pnum]._pClass == PC_MONK)
			PlaySFX(PS_MONK13);
		else if (plr[pnum]._pClass == PC_BARD)
			PlaySFX(PS_ROGUE13);
		else if (plr[pnum]._pClass == PC_BARBARIAN)
			PlaySFX(PS_MAGE13);
	}

	if (!done)
		return;

	if (pnum == myplr)
		PlaySFX(ItemInvSnds[ItemCAnimTbl[plr[pnum].HoldItem._iCurs]]);

	cn = CURSOR_HAND;
	switch (il) {
	case ILOC_HELM:
		NetSendCmdChItem(FALSE, INVLOC_HEAD);
		if (plr[pnum].InvBody[INVLOC_HEAD]._itype == ITYPE_NONE)
			plr[pnum].InvBody[INVLOC_HEAD] = plr[pnum].HoldItem;
		else
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HEAD], &plr[pnum].HoldItem);
		break;
	case ILOC_RING:
		if (r == SLOTXY_RING_LEFT) {
			NetSendCmdChItem(FALSE, INVLOC_RING_LEFT);
			if (plr[pnum].InvBody[INVLOC_RING_LEFT]._itype == ITYPE_NONE)
				plr[pnum].InvBody[INVLOC_RING_LEFT] = plr[pnum].HoldItem;
			else
				cn = SwapItem(&plr[pnum].InvBody[INVLOC_RING_LEFT], &plr[pnum].HoldItem);
		} else {
			NetSendCmdChItem(FALSE, INVLOC_RING_RIGHT);
			if (plr[pnum].InvBody[INVLOC_RING_RIGHT]._itype == ITYPE_NONE)
				plr[pnum].InvBody[INVLOC_RING_RIGHT] = plr[pnum].HoldItem;
			else
				cn = SwapItem(&plr[pnum].InvBody[INVLOC_RING_RIGHT], &plr[pnum].HoldItem);
		}
		break;
	case ILOC_AMULET:
		NetSendCmdChItem(FALSE, INVLOC_AMULET);
		if (plr[pnum].InvBody[INVLOC_AMULET]._itype == ITYPE_NONE)
			plr[pnum].InvBody[INVLOC_AMULET] = plr[pnum].HoldItem;
		else
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_AMULET], &plr[pnum].HoldItem);
		break;
	case ILOC_ONEHAND:
		if (r <= SLOTXY_HAND_LEFT_LAST) {
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE) {
				if ((plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass != plr[pnum].HoldItem._iClass)
				    || (plr[pnum]._pClass == PC_BARD && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON && plr[pnum].HoldItem._iClass == ICLASS_WEAPON)) {
					NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
					plr[pnum].InvBody[INVLOC_HAND_LEFT] = plr[pnum].HoldItem;
				} else {
					NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
					cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
				}
				break;
			}
			if ((plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass != plr[pnum].HoldItem._iClass)
			    || (plr[pnum]._pClass == PC_BARD && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON && plr[pnum].HoldItem._iClass == ICLASS_WEAPON)) {
				NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
				cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].HoldItem);
				break;
			}

			NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
			break;
		}
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE) {
			if ((plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE || plr[pnum].InvBody[INVLOC_HAND_LEFT]._iLoc != ILOC_TWOHAND)
			    || (plr[pnum]._pClass == PC_BARBARIAN && (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE))) {
				if ((plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE || plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass != plr[pnum].HoldItem._iClass)
				    || (plr[pnum]._pClass == PC_BARD && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON && plr[pnum].HoldItem._iClass == ICLASS_WEAPON)) {
					NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
					plr[pnum].InvBody[INVLOC_HAND_RIGHT] = plr[pnum].HoldItem;
					break;
				}
				NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
				cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].HoldItem);
				break;
			}
			NetSendCmdDelItem(FALSE, INVLOC_HAND_LEFT);
			NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
			SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].InvBody[INVLOC_HAND_LEFT]);
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
			break;
		}

		if ((plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == plr[pnum].HoldItem._iClass)
		    && !(plr[pnum]._pClass == PC_BARD && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON && plr[pnum].HoldItem._iClass == ICLASS_WEAPON)) {
			NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].HoldItem);
			break;
		}
		NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
		cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
		break;
	case ILOC_TWOHAND:
		NetSendCmdDelItem(FALSE, INVLOC_HAND_RIGHT);
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE) {
			tempitem = plr[pnum].HoldItem;
			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD)
				plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HAND_RIGHT];
			else
				plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HAND_LEFT];
			if (pnum == myplr)
				SetCursor_(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			else
				SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			done2h = FALSE;
			for (i = 0; i < NUM_INV_GRID_ELEM && !done2h; i++)
				done2h = AutoPlace(pnum, i, icursW28, icursH28, TRUE);
			plr[pnum].HoldItem = tempitem;
			if (pnum == myplr)
				SetCursor_(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			else
				SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			if (!done2h)
				return;

			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD)
				plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			else
				plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
		}

		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE) {
			NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE)
				SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].InvBody[INVLOC_HAND_RIGHT]);
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].HoldItem);
		} else {
			NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
			plr[pnum].InvBody[INVLOC_HAND_LEFT] = plr[pnum].HoldItem;
		}
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iSpell != SPL_NULL && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
			plr[pnum]._pRSpell = plr[pnum].InvBody[INVLOC_HAND_LEFT]._iSpell;
			plr[pnum]._pRSplType = RSPLTYPE_CHARGES;
			force_redraw = 255;
		}
		break;
	case ILOC_ARMOR:
		NetSendCmdChItem(FALSE, INVLOC_CHEST);
		if (plr[pnum].InvBody[INVLOC_CHEST]._itype == ITYPE_NONE)
			plr[pnum].InvBody[INVLOC_CHEST] = plr[pnum].HoldItem;
		else
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_CHEST], &plr[pnum].HoldItem);
		break;
	case ILOC_UNEQUIPABLE:
		if (plr[pnum].HoldItem._itype == ITYPE_GOLD && it == 0) {
			ii = r - SLOTXY_INV_FIRST;
			yy = 10 * (ii / 10);
			xx = ii % 10;
			if (plr[pnum].InvGrid[yy + xx] > 0) {
				il = plr[pnum].InvGrid[yy + xx];
				il--;
				gt = plr[pnum].InvList[il]._ivalue;
				ig = plr[pnum].HoldItem._ivalue + gt;
				if (ig <= GOLD_MAX_LIMIT) {
					plr[pnum].InvList[il]._ivalue = ig;
					plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
					SetPlrHandGoldCurs(&plr[pnum].InvList[il]);
				} else {
					ig = GOLD_MAX_LIMIT - gt;
					plr[pnum]._pGold += ig;
					plr[pnum].HoldItem._ivalue -= ig;
					plr[pnum].InvList[il]._ivalue = GOLD_MAX_LIMIT;
					plr[pnum].InvList[il]._iCurs = ICURS_GOLD_LARGE;
					// BUGFIX: incorrect values here are leftover from beta (fixed)
					cn = GetGoldCursor(plr[pnum].HoldItem._ivalue);
					cn += CURSOR_FIRSTITEM;
				}
			} else {
				il = plr[pnum]._pNumInv;
				plr[pnum].InvList[il] = plr[pnum].HoldItem;
				plr[pnum]._pNumInv++;
				plr[pnum].InvGrid[yy + xx] = plr[pnum]._pNumInv;
				plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
				SetPlrHandGoldCurs(&plr[pnum].InvList[il]);
			}
		} else {
			if (it == 0) {
				plr[pnum].InvList[plr[pnum]._pNumInv] = plr[pnum].HoldItem;
				plr[pnum]._pNumInv++;
				it = plr[pnum]._pNumInv;
			} else {
				il = it - 1;
				if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
					plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
				cn = SwapItem(&plr[pnum].InvList[il], &plr[pnum].HoldItem);
				if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
					plr[pnum]._pGold = CalculateGold(pnum);
				for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
					if (plr[pnum].InvGrid[i] == it)
						plr[pnum].InvGrid[i] = 0;
					if (plr[pnum].InvGrid[i] == -it)
						plr[pnum].InvGrid[i] = 0;
				}
			}
			ii = r - SLOTXY_INV_FIRST;
			yy = 10 * (ii / 10 - ((sy - 1) >> 1));
			if (yy < 0)
				yy = 0;
			for (j = 0; j < sy; j++) {
				xx = (ii % 10 - ((sx - 1) >> 1));
				if (xx < 0)
					xx = 0;
				for (i = 0; i < sx; i++) {
					if (i != 0 || j != sy - 1)
						plr[pnum].InvGrid[xx + yy] = -it;
					else
						plr[pnum].InvGrid[xx + yy] = it;
					xx++;
				}
				yy += 10;
			}
		}
		break;
	case ILOC_BELT:
		ii = r - SLOTXY_BELT_FIRST;
		if (plr[pnum].HoldItem._itype == ITYPE_GOLD) {
			if (plr[pnum].SpdList[ii]._itype != ITYPE_NONE) {
				if (plr[pnum].SpdList[ii]._itype == ITYPE_GOLD) {
					i = plr[pnum].HoldItem._ivalue + plr[pnum].SpdList[ii]._ivalue;
					if (i <= GOLD_MAX_LIMIT) {
						plr[pnum].SpdList[ii]._ivalue = i;
						plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
						SetPlrHandGoldCurs(&plr[pnum].SpdList[ii]);
					} else {
						i = GOLD_MAX_LIMIT - plr[pnum].SpdList[ii]._ivalue;
						plr[pnum]._pGold += i;
						plr[pnum].HoldItem._ivalue -= i;
						plr[pnum].SpdList[ii]._ivalue = GOLD_MAX_LIMIT;
						plr[pnum].SpdList[ii]._iCurs = ICURS_GOLD_LARGE;

						// BUGFIX: incorrect values here are leftover from beta (fixed)
						cn = GetGoldCursor(plr[pnum].HoldItem._ivalue);
						cn += CURSOR_FIRSTITEM;
					}
				} else {
					plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
					cn = SwapItem(&plr[pnum].SpdList[ii], &plr[pnum].HoldItem);
				}
			} else {
				plr[pnum].SpdList[ii] = plr[pnum].HoldItem;
				plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
			}
		} else if (plr[pnum].SpdList[ii]._itype == ITYPE_NONE) {
			plr[pnum].SpdList[ii] = plr[pnum].HoldItem;
		} else {
			cn = SwapItem(&plr[pnum].SpdList[ii], &plr[pnum].HoldItem);
			if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
				plr[pnum]._pGold = CalculateGold(pnum);
		}
		drawsbarflag = TRUE;
		break;
	}
	CalcPlrInv(pnum, TRUE);
	if (pnum == myplr) {
		if (cn == CURSOR_HAND)
			SetCursorPos(MouseX + (cursW >> 1), MouseY + (cursH >> 1));
		SetCursor_(cn);
	}
}

void CheckInvSwap(int pnum, BYTE bLoc, int idx, WORD wCI, int seed, BOOL bId)
{
	PlayerStruct *p;

	memset(&item[MAXITEMS], 0, sizeof(*item));
	RecreateItem(MAXITEMS, idx, wCI, seed, 0);

	p = &plr[pnum];
	p->HoldItem = item[MAXITEMS];

	if (bId) {
		p->HoldItem._iIdentified = TRUE;
	}

	if (bLoc < NUM_INVLOC) {
		p->InvBody[bLoc] = p->HoldItem;

		if (bLoc == INVLOC_HAND_LEFT && p->HoldItem._iLoc == ILOC_TWOHAND) {
			p->InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
		} else if (bLoc == INVLOC_HAND_RIGHT && p->HoldItem._iLoc == ILOC_TWOHAND) {
			p->InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
		}
	}

	CalcPlrInv(pnum, TRUE);
}

void CheckInvCut(int pnum, int mx, int my)
{
	int r;
	BOOL done;
	char ii;
	int iv, i, j, offs, ig;

	if (plr[pnum]._pmode > PM_WALK3) {
		return;
	}

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	done = FALSE;

	for (r = 0; (DWORD)r < NUM_XY_SLOTS && !done; r++) {
		int xo = RIGHT_PANEL;
		int yo = 0;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = PANEL_LEFT;
			yo = PANEL_TOP;
		}

		// check which inventory rectangle the mouse is in, if any
		if (mx >= InvRect[r].X + xo
		    && mx < InvRect[r].X + xo + (INV_SLOT_SIZE_PX + 1)
		    && my >= InvRect[r].Y + yo - (INV_SLOT_SIZE_PX + 1)
		    && my < InvRect[r].Y + yo) {
			done = TRUE;
			r--;
		}
	}

	if (!done) {
		// not on an inventory slot rectangle
		return;
	}

	plr[pnum].HoldItem._itype = ITYPE_NONE;

	if (
	    r >= SLOTXY_HEAD_FIRST
	    && r <= SLOTXY_HEAD_LAST
	    && plr[pnum].InvBody[INVLOC_HEAD]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_HEAD);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HEAD];
		plr[pnum].InvBody[INVLOC_HEAD]._itype = ITYPE_NONE;
	}

	if (
	    r == SLOTXY_RING_LEFT
	    && plr[pnum].InvBody[INVLOC_RING_LEFT]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_RING_LEFT);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_RING_LEFT];
		plr[pnum].InvBody[INVLOC_RING_LEFT]._itype = ITYPE_NONE;
	}

	if (
	    r == SLOTXY_RING_RIGHT
	    && plr[pnum].InvBody[INVLOC_RING_RIGHT]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_RING_RIGHT);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_RING_RIGHT];
		plr[pnum].InvBody[INVLOC_RING_RIGHT]._itype = ITYPE_NONE;
	}

	if (
	    r == SLOTXY_AMULET
	    && plr[pnum].InvBody[INVLOC_AMULET]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_AMULET);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_AMULET];
		plr[pnum].InvBody[INVLOC_AMULET]._itype = ITYPE_NONE;
	}

	if (
	    r >= SLOTXY_HAND_LEFT_FIRST
	    && r <= SLOTXY_HAND_LEFT_LAST
	    && plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_HAND_LEFT);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HAND_LEFT];
		plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
	}

	if (
	    r >= SLOTXY_HAND_RIGHT_FIRST
	    && r <= SLOTXY_HAND_RIGHT_LAST
	    && plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_HAND_RIGHT);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_HAND_RIGHT];
		plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
	}

	if (
	    r >= SLOTXY_CHEST_FIRST
	    && r <= SLOTXY_CHEST_LAST
	    && plr[pnum].InvBody[INVLOC_CHEST]._itype != ITYPE_NONE) {
		NetSendCmdDelItem(FALSE, INVLOC_CHEST);
		plr[pnum].HoldItem = plr[pnum].InvBody[INVLOC_CHEST];
		plr[pnum].InvBody[INVLOC_CHEST]._itype = ITYPE_NONE;
	}

	if (r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST) {
		ig = r - SLOTXY_INV_FIRST;
		ii = plr[pnum].InvGrid[ig];
		if (ii != 0) {
			iv = ii;
			if (ii <= 0) {
				iv = -ii;
			}

			for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
				if (plr[pnum].InvGrid[i] == iv || plr[pnum].InvGrid[i] == -iv) {
					plr[pnum].InvGrid[i] = 0;
				}
			}

			iv--;

			plr[pnum].HoldItem = plr[pnum].InvList[iv];
			plr[pnum]._pNumInv--;

			if (plr[pnum]._pNumInv > 0 && plr[pnum]._pNumInv != iv) {
				plr[pnum].InvList[iv] = plr[pnum].InvList[plr[pnum]._pNumInv];

				for (j = 0; j < NUM_INV_GRID_ELEM; j++) {
					if (plr[pnum].InvGrid[j] == plr[pnum]._pNumInv + 1) {
						plr[pnum].InvGrid[j] = iv + 1;
					}
					if (plr[pnum].InvGrid[j] == -(plr[pnum]._pNumInv + 1)) {
						plr[pnum].InvGrid[j] = -iv - 1;
					}
				}
			}
		}
	}

	if (r >= SLOTXY_BELT_FIRST) {
		offs = r - SLOTXY_BELT_FIRST;
		if (plr[pnum].SpdList[offs]._itype != ITYPE_NONE) {
			plr[pnum].HoldItem = plr[pnum].SpdList[offs];
			plr[pnum].SpdList[offs]._itype = ITYPE_NONE;
			drawsbarflag = TRUE;
		}
	}

	if (plr[pnum].HoldItem._itype != ITYPE_NONE) {
		if (plr[pnum].HoldItem._itype == ITYPE_GOLD) {
			plr[pnum]._pGold = CalculateGold(pnum);
		}

		CalcPlrInv(pnum, TRUE);
		CheckItemStats(pnum);

		if (pnum == myplr) {
			PlaySFX(IS_IGRAB);
			SetCursor_(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
			SetCursorPos(mx - (cursW >> 1), MouseY - (cursH >> 1));
		}
	}
}

void inv_update_rem_item(int pnum, BYTE iv)
{
	if (iv < NUM_INVLOC) {
		plr[pnum].InvBody[iv]._itype = ITYPE_NONE;
	}

	if (plr[pnum]._pmode != PM_DEATH) {
		CalcPlrInv(pnum, TRUE);
	} else {
		CalcPlrInv(pnum, FALSE);
	}
}

void RemoveInvItem(int pnum, int iv)
{
	int i, j;

	iv++;

	for (i = 0; i < NUM_INV_GRID_ELEM; i++) {
		if (plr[pnum].InvGrid[i] == iv || plr[pnum].InvGrid[i] == -iv) {
			plr[pnum].InvGrid[i] = 0;
		}
	}

	iv--;
	plr[pnum]._pNumInv--;

	if (plr[pnum]._pNumInv > 0 && plr[pnum]._pNumInv != iv) {
		plr[pnum].InvList[iv] = plr[pnum].InvList[plr[pnum]._pNumInv];

		for (j = 0; j < NUM_INV_GRID_ELEM; j++) {
			if (plr[pnum].InvGrid[j] == plr[pnum]._pNumInv + 1) {
				plr[pnum].InvGrid[j] = iv + 1;
			}
			if (plr[pnum].InvGrid[j] == -(plr[pnum]._pNumInv + 1)) {
				plr[pnum].InvGrid[j] = -(iv + 1);
			}
		}
	}

	CalcPlrScrolls(pnum);
}

void RemoveSpdBarItem(int pnum, int iv)
{
	plr[pnum].SpdList[iv]._itype = ITYPE_NONE;

	CalcPlrScrolls(pnum);
	force_redraw = 255;
}

void CheckInvItem()
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		CheckInvPaste(myplr, MouseX, MouseY);
	} else {
		CheckInvCut(myplr, MouseX, MouseY);
	}
}

/**
 * Check for interactions with belt
 */
void CheckInvScrn()
{
	if (MouseX > 190 + PANEL_LEFT && MouseX < 437 + PANEL_LEFT
	    && MouseY > PANEL_TOP && MouseY < 33 + PANEL_TOP) {
		CheckInvItem();
	}
}

void CheckItemStats(int pnum)
{
	PlayerStruct *p = &plr[pnum];

	p->HoldItem._iStatFlag = FALSE;

	if (p->_pStrength >= p->HoldItem._iMinStr
	    && p->_pMagic >= p->HoldItem._iMinMag
	    && p->_pDexterity >= p->HoldItem._iMinDex) {
		p->HoldItem._iStatFlag = TRUE;
	}
}

void CheckBookLevel(int pnum)
{
	int slvl;

	if (plr[pnum].HoldItem._iMiscId == IMISC_BOOK) {
		plr[pnum].HoldItem._iMinMag = spelldata[plr[pnum].HoldItem._iSpell].sMinInt;
		slvl = plr[pnum]._pSplLvl[plr[pnum].HoldItem._iSpell];
		while (slvl != 0) {
			plr[pnum].HoldItem._iMinMag += 20 * plr[pnum].HoldItem._iMinMag / 100;
			slvl--;
			if (plr[pnum].HoldItem._iMinMag + 20 * plr[pnum].HoldItem._iMinMag / 100 > 255) {
				plr[pnum].HoldItem._iMinMag = -1;
				slvl = 0;
			}
		}
	}
}

void CheckQuestItem(int pnum)
{
	if (plr[pnum].HoldItem.IDidx == IDI_OPTAMULET && quests[Q_BLIND]._qactive == QUEST_ACTIVE)
		quests[Q_BLIND]._qactive = QUEST_DONE;
	if (plr[pnum].HoldItem.IDidx == IDI_MUSHROOM && quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qvar1 == QS_MUSHSPAWNED) {
		sfxdelay = 10;
		if (plr[pnum]._pClass == PC_WARRIOR) { // BUGFIX: Voice for this quest might be wrong in MP
			sfxdnum = PS_WARR95;
		} else if (plr[pnum]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE95;
		} else if (plr[pnum]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE95;
		} else if (plr[pnum]._pClass == PC_MONK) {
			sfxdnum = PS_MONK95;
		} else if (plr[pnum]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE95;
		} else if (plr[pnum]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR95;
		}
		quests[Q_MUSHROOM]._qvar1 = QS_MUSHPICKED;
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ANVIL && quests[Q_ANVIL]._qactive != QUEST_NOTAVAIL) {
		if (quests[Q_ANVIL]._qactive == QUEST_INIT) {
			quests[Q_ANVIL]._qactive = QUEST_ACTIVE;
			quests[Q_ANVIL]._qvar1 = 1;
		}
		if (quests[Q_ANVIL]._qlog == TRUE) {
			sfxdelay = 10;
			if (plr[myplr]._pClass == PC_WARRIOR) {
				sfxdnum = PS_WARR89;
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				sfxdnum = PS_ROGUE89;
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				sfxdnum = PS_MAGE89;
			} else if (plr[myplr]._pClass == PC_MONK) {
				sfxdnum = PS_MONK89;
			} else if (plr[myplr]._pClass == PC_BARD) {
				sfxdnum = PS_ROGUE89;
			} else if (plr[myplr]._pClass == PC_BARBARIAN) {
				sfxdnum = PS_WARR89;
			}
		}
	}
	if (plr[pnum].HoldItem.IDidx == IDI_GLDNELIX && quests[Q_VEIL]._qactive != QUEST_NOTAVAIL) {
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR88;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE88;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE88;
		} else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK88;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE88;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR88;
		}
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ROCK && quests[Q_ROCK]._qactive != QUEST_NOTAVAIL) {
		if (quests[Q_ROCK]._qactive == QUEST_INIT) {
			quests[Q_ROCK]._qactive = QUEST_ACTIVE;
			quests[Q_ROCK]._qvar1 = 1;
		}
		if (quests[Q_ROCK]._qlog == TRUE) {
			sfxdelay = 10;
			if (plr[myplr]._pClass == PC_WARRIOR) {
				sfxdnum = PS_WARR87;
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				sfxdnum = PS_ROGUE87;
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				sfxdnum = PS_MAGE87;
			} else if (plr[myplr]._pClass == PC_MONK) {
				sfxdnum = PS_MONK87;
			} else if (plr[myplr]._pClass == PC_BARD) {
				sfxdnum = PS_ROGUE87;
			} else if (plr[myplr]._pClass == PC_BARBARIAN) {
				sfxdnum = PS_WARR87;
			}
		}
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ARMOFVAL && quests[Q_BLOOD]._qactive == QUEST_ACTIVE) {
		quests[Q_BLOOD]._qactive = QUEST_DONE;
		sfxdelay = 20;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR91;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE91;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE91;
		} else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK91;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE91;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR91;
		}
	}
	if (plr[pnum].HoldItem.IDidx == IDI_MAPOFDOOM) {
		quests[Q_GRAVE]._qlog = FALSE;
		quests[Q_GRAVE]._qactive = QUEST_ACTIVE;
		quests[Q_GRAVE]._qvar1 = 1;
		sfxdelay = 10;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR79;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE79;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE79;
		} else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK79;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE79;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR79;
		}
	}
	if (plr[pnum].HoldItem.IDidx == IDI_NOTE1 || plr[pnum].HoldItem.IDidx == IDI_NOTE2 || plr[pnum].HoldItem.IDidx == IDI_NOTE3) {
		int mask, idx, item_num;
		int n1, n2, n3;
		ItemStruct tmp;
		mask = 0;
		idx = plr[pnum].HoldItem.IDidx;
		if (PlrHasItem(pnum, IDI_NOTE1, &n1) || idx == IDI_NOTE1)
			mask = 1;
		if (PlrHasItem(pnum, IDI_NOTE2, &n2) || idx == IDI_NOTE2)
			mask |= 2;
		if (PlrHasItem(pnum, IDI_NOTE3, &n3) || idx == IDI_NOTE3)
			mask |= 4;
		if (mask == 7) {
			sfxdelay = 10;
			if (plr[myplr]._pClass == PC_WARRIOR) {
				sfxdnum = PS_WARR46;
			} else if (plr[myplr]._pClass == PC_ROGUE) {
				sfxdnum = PS_ROGUE46;
			} else if (plr[myplr]._pClass == PC_SORCERER) {
				sfxdnum = PS_MAGE46;
			} else if (plr[myplr]._pClass == PC_MONK) {
				sfxdnum = PS_MONK46;
			} else if (plr[myplr]._pClass == PC_BARD) {
				sfxdnum = PS_ROGUE46;
			} else if (plr[myplr]._pClass == PC_BARBARIAN) {
				sfxdnum = PS_WARR46;
			}
			switch (idx) {
			case IDI_NOTE1:
				PlrHasItem(pnum, IDI_NOTE2, &n2);
				RemoveInvItem(pnum, n2);
				PlrHasItem(pnum, IDI_NOTE3, &n3);
				RemoveInvItem(pnum, n3);
				break;
			case IDI_NOTE2:
				PlrHasItem(pnum, IDI_NOTE1, &n1);
				RemoveInvItem(pnum, n1);
				PlrHasItem(pnum, IDI_NOTE3, &n3);
				RemoveInvItem(pnum, n3);
				break;
			case IDI_NOTE3:
				PlrHasItem(pnum, IDI_NOTE1, &n1);
				RemoveInvItem(pnum, n1);
				PlrHasItem(pnum, IDI_NOTE2, &n2);
				RemoveInvItem(pnum, n2);
				break;
			}
			item_num = itemactive[0];
			tmp = item[item_num];
			memset(&item[item_num], 0, sizeof(*item));
			GetItemAttrs(item_num, IDI_FULLNOTE, 16);
			SetupItem(item_num);
			plr[pnum].HoldItem = item[item_num];
			item[item_num] = tmp;
		}
	}
}

void CleanupItems(int ii)
{
	dItem[item[ii]._ix][item[ii]._iy] = 0;

	if (currlevel == 21 & item[ii]._ix == CornerStone.x && item[ii]._iy == CornerStone.y) {
		CornerStone.item.IDidx = -1;
		CornerStone.item._itype = ITYPE_MISC;
		CornerStone.item._iSelFlag = FALSE;
		CornerStone.item._ix = 0;
		CornerStone.item._iy = 0;
		CornerStone.item._iAnimFlag = FALSE;
		CornerStone.item._iIdentified = FALSE;
		CornerStone.item._iPostDraw = FALSE;
	}

	int i = 0;
	while (i < numitems) {
		if (itemactive[i] == ii) {
			DeleteItem(itemactive[i], i);
			i = 0;
			continue;
		}

		i++;
	}
}

void InvGetItem(int pnum, int ii)
{
	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	if (dItem[item[ii]._ix][item[ii]._iy] == 0)
		return;

	if (myplr == pnum && pcurs >= CURSOR_FIRSTITEM)
		NetSendCmdPItem(TRUE, CMD_SYNCPUTITEM, plr[myplr]._px, plr[myplr]._py);

	item[ii]._iCreateInfo &= ~CF_PREGEN;
	plr[pnum].HoldItem = item[ii];
	CheckQuestItem(pnum);
	CheckBookLevel(pnum);
	CheckItemStats(pnum);
	bool cursor_updated = false;
	if (gbIsHellfire && plr[pnum].HoldItem._itype == ITYPE_GOLD && GoldAutoPlace(pnum))
		cursor_updated = true;
	CleanupItems(ii);
	pcursitem = -1;
	if (!cursor_updated)
		SetCursor_(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
}

void AutoGetItem(int pnum, int ii)
{
	int i, idx;
	int w, h;
	BOOL done;

	if (pcurs != CURSOR_HAND) {
		return;
	}

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	if (ii != MAXITEMS) {
		if (dItem[item[ii]._ix][item[ii]._iy] == 0)
			return;
	}

	item[ii]._iCreateInfo &= ~CF_PREGEN;
	plr[pnum].HoldItem = item[ii]; /// BUGFIX: overwrites cursor item, allowing for belt dupe bug
	CheckQuestItem(pnum);
	CheckBookLevel(pnum);
	CheckItemStats(pnum);
	SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
	if (plr[pnum].HoldItem._itype == ITYPE_GOLD) {
		done = GoldAutoPlace(pnum);
		if (!done) {
			item[ii]._ivalue = plr[pnum].HoldItem._ivalue;
			SetPlrHandGoldCurs(&item[ii]);
		}
	} else {
		done = FALSE;
		if (((plr[pnum]._pgfxnum & 0xF) == ANIM_ID_UNARMED
		        || (plr[pnum]._pgfxnum & 0xF) == ANIM_ID_UNARMED_SHIELD
		        || plr[pnum]._pClass == PC_BARD && ((plr[pnum]._pgfxnum & 0xF) == ANIM_ID_MACE || (plr[pnum]._pgfxnum & 0xF) == ANIM_ID_SWORD))
		    && plr[pnum]._pmode <= PM_WALK3) {
			if (plr[pnum].HoldItem._iStatFlag) {
				if (plr[pnum].HoldItem._iClass == ICLASS_WEAPON) {
					done = WeaponAutoPlace(pnum);
					if (done)
						CalcPlrInv(pnum, TRUE);
				}
			}
		}
		if (!done) {
			w = icursW28;
			h = icursH28;
			if (w == 1 && h == 1) {
				idx = plr[pnum].HoldItem.IDidx;
				if (plr[pnum].HoldItem._iStatFlag && AllItemsList[idx].iUsable) {
					for (i = 0; i < MAXBELTITEMS && !done; i++) {
						if (plr[pnum].SpdList[i]._itype == ITYPE_NONE) {
							plr[pnum].SpdList[i] = plr[pnum].HoldItem;
							CalcPlrScrolls(pnum);
							drawsbarflag = TRUE;
							done = TRUE;
						}
					}
				}
				for (i = 30; i <= 39 && !done; i++) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
				for (i = 20; i <= 29 && !done; i++) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
				for (i = 10; i <= 19 && !done; i++) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
				for (i = 0; i <= 9 && !done; i++) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
			}
			if (w == 1 && h == 2) {
				for (i = 29; i >= 20 && !done; i--) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
				for (i = 9; i >= 0 && !done; i--) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
				for (i = 19; i >= 10 && !done; i--) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
			}
			if (w == 1 && h == 3) {
				for (i = 0; i < 20 && !done; i++) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
			}
			if (w == 2 && h == 2) {
				for (i = 0; i < 10 && !done; i++) {
					done = AutoPlace(pnum, AP2x2Tbl[i], w, h, TRUE);
				}
				for (i = 21; i < 29 && !done; i += 2) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
				for (i = 1; i < 9 && !done; i += 2) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
				for (i = 10; i < 19 && !done; i++) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
			}
			if (w == 2 && h == 3) {
				for (i = 0; i < 9 && !done; i++) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
				for (i = 10; i < 19 && !done; i++) {
					done = AutoPlace(pnum, i, w, h, TRUE);
				}
			}
		}
	}

	if (done) {
		CleanupItems(ii);
		return;
	}

	if (pnum == myplr) {
		if (plr[pnum]._pClass == PC_WARRIOR) {
			PlaySFX(random_(0, 3) + PS_WARR14);
		} else if (plr[pnum]._pClass == PC_ROGUE) {
			PlaySFX(random_(0, 3) + PS_ROGUE14);
		} else if (plr[pnum]._pClass == PC_SORCERER) {
			PlaySFX(random_(0, 3) + PS_MAGE14);
		} else if (plr[pnum]._pClass == PC_MONK) {
			PlaySFX(random_(0, 3) + PS_MONK14);
		} else if (plr[pnum]._pClass == PC_BARD) {
			PlaySFX(random_(0, 3) + PS_ROGUE14);
		} else if (plr[pnum]._pClass == PC_BARBARIAN) {
			PlaySFX(random_(0, 3) + PS_WARR14);
		}
	}
	plr[pnum].HoldItem = item[ii];
	RespawnItem(ii, TRUE);
	NetSendCmdPItem(TRUE, CMD_RESPAWNITEM, item[ii]._ix, item[ii]._iy);
	plr[pnum].HoldItem._itype = ITYPE_NONE;
}

int FindGetItem(int idx, WORD ci, int iseed)
{
	if (numitems <= 0)
		return -1;

	int ii;
	int i = 0;
	while (1) {
		ii = itemactive[i];
		if (item[ii].IDidx == idx && item[ii]._iSeed == iseed && item[ii]._iCreateInfo == ci)
			break;

		i++;

		if (i >= numitems)
			return -1;
	}

	return ii;
}

void SyncGetItem(int x, int y, int idx, WORD ci, int iseed)
{
	int i, ii;

	if (dItem[x][y]) {
		ii = dItem[x][y] - 1;
		if (item[ii].IDidx == idx
		    && item[ii]._iSeed == iseed
		    && item[ii]._iCreateInfo == ci) {
			FindGetItem(idx, ci, iseed);
		} else {
			ii = FindGetItem(idx, ci, iseed);
		}
	} else {
		ii = FindGetItem(idx, ci, iseed);
	}

	if (ii == -1)
		return;

	CleanupItems(ii);
	assert(FindGetItem(idx, ci, iseed) == -1);
}

BOOL CanPut(int x, int y)
{
	char oi, oi2;

	if (dItem[x][y])
		return FALSE;
	if (nSolidTable[dPiece[x][y]])
		return FALSE;

	if (dObject[x][y] != 0) {
		if (object[dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1)]._oSolidFlag)
			return FALSE;
	}

	oi = dObject[x + 1][y + 1];
	if (oi > 0 && object[oi - 1]._oSelFlag != 0) {
		return FALSE;
	}
	if (oi < 0 && object[-(oi + 1)]._oSelFlag != 0) {
		return FALSE;
	}

	oi = dObject[x + 1][y];
	if (oi > 0) {
		oi2 = dObject[x][y + 1];
		if (oi2 > 0 && object[oi - 1]._oSelFlag != 0 && object[oi2 - 1]._oSelFlag != 0)
			return FALSE;
	}

	if (currlevel == 0 && dMonster[x][y] != 0)
		return FALSE;
	if (currlevel == 0 && dMonster[x + 1][y + 1] != 0)
		return FALSE;

	return TRUE;
}

BOOL TryInvPut()
{
	int dir;

	if (numitems >= MAXITEMS)
		return FALSE;

	dir = GetDirection(plr[myplr]._px, plr[myplr]._py, cursmx, cursmy);
	if (CanPut(plr[myplr]._px + offset_x[dir], plr[myplr]._py + offset_y[dir])) {
		return TRUE;
	}

	dir = (dir - 1) & 7;
	if (CanPut(plr[myplr]._px + offset_x[dir], plr[myplr]._py + offset_y[dir])) {
		return TRUE;
	}

	dir = (dir + 2) & 7;
	if (CanPut(plr[myplr]._px + offset_x[dir], plr[myplr]._py + offset_y[dir])) {
		return TRUE;
	}

	return CanPut(plr[myplr]._px, plr[myplr]._py);
}

void DrawInvMsg(const char *msg)
{
	DWORD dwTicks;

	dwTicks = SDL_GetTicks();
	if (dwTicks - sgdwLastTime >= 5000) {
		sgdwLastTime = dwTicks;
		ErrorPlrMsg(msg);
	}
}

int InvPutItem(int pnum, int x, int y)
{
	BOOL done;
	int d;
	int i, j, l;
	int xx, yy;
	int xp, yp;

	if (numitems >= MAXITEMS)
		return -1;

	d = GetDirection(plr[pnum]._px, plr[pnum]._py, x, y);
	xx = x - plr[pnum]._px;
	yy = y - plr[pnum]._py;
	if (abs(xx) > 1 || abs(yy) > 1) {
		x = plr[pnum]._px + offset_x[d];
		y = plr[pnum]._py + offset_y[d];
	}
	if (!CanPut(x, y)) {
		d = (d - 1) & 7;
		x = plr[pnum]._px + offset_x[d];
		y = plr[pnum]._py + offset_y[d];
		if (!CanPut(x, y)) {
			d = (d + 2) & 7;
			x = plr[pnum]._px + offset_x[d];
			y = plr[pnum]._py + offset_y[d];
			if (!CanPut(x, y)) {
				done = FALSE;
				for (l = 1; l < 50 && !done; l++) {
					for (j = -l; j <= l && !done; j++) {
						yp = j + plr[pnum]._py;
						for (i = -l; i <= l && !done; i++) {
							xp = i + plr[pnum]._px;
							if (CanPut(xp, yp)) {
								done = TRUE;
								x = xp;
								y = yp;
							}
						}
					}
				}
				if (!done)
					return -1;
			}
		}
	}

	if (currlevel == 0) {
		yp = cursmy;
		xp = cursmx;
		if (plr[pnum].HoldItem._iCurs == ICURS_RUNE_BOMB && xp >= 79 && xp <= 82 && yp >= 61 && yp <= 64) {
			NetSendCmdLocParam2(0, CMD_OPENHIVE, plr[pnum]._px, plr[pnum]._py, xx, yy);
			quests[Q_FARMER]._qactive = 3;
			if (gbIsMultiplayer) {
				NetSendCmdQuest(TRUE, Q_FARMER);
				return -1;
			}
			return -1;
		}
		if (plr[pnum].HoldItem.IDidx == IDI_MAPOFDOOM && xp >= 35 && xp <= 38 && yp >= 20 && yp <= 24) {
			NetSendCmd(FALSE, CMD_OPENCRYPT);
			quests[Q_GRAVE]._qactive = 3;
			if (gbIsMultiplayer) {
				NetSendCmdQuest(TRUE, Q_GRAVE);
			}
			return -1;
		}
	}

	assert(CanPut(x, y));

	int ii = AllocateItem();

	dItem[x][y] = ii + 1;
	item[ii] = plr[pnum].HoldItem;
	item[ii]._ix = x;
	item[ii]._iy = y;
	RespawnItem(ii, TRUE);

	if (currlevel == 21 && x == CornerStone.x && y == CornerStone.y) {
		CornerStone.item = item[ii];
		InitQTextMsg(296);
		quests[Q_CORNSTN]._qlog = FALSE;
		quests[Q_CORNSTN]._qactive = QUEST_DONE;
	}

	NewCursor(CURSOR_HAND);
	return ii;
}

int SyncPutItem(int pnum, int x, int y, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff, int to_hit, int max_dam, int min_str, int min_mag, int min_dex, int ac)
{
	BOOL done;
	int d;
	int i, j, l;
	int xx, yy;
	int xp, yp;

	if (numitems >= MAXITEMS)
		return -1;

	d = GetDirection(plr[pnum]._px, plr[pnum]._py, x, y);
	xx = x - plr[pnum]._px;
	yy = y - plr[pnum]._py;
	if (abs(xx) > 1 || abs(yy) > 1) {
		x = plr[pnum]._px + offset_x[d];
		y = plr[pnum]._py + offset_y[d];
	}
	if (!CanPut(x, y)) {
		d = (d - 1) & 7;
		x = plr[pnum]._px + offset_x[d];
		y = plr[pnum]._py + offset_y[d];
		if (!CanPut(x, y)) {
			d = (d + 2) & 7;
			x = plr[pnum]._px + offset_x[d];
			y = plr[pnum]._py + offset_y[d];
			if (!CanPut(x, y)) {
				done = FALSE;
				for (l = 1; l < 50 && !done; l++) {
					for (j = -l; j <= l && !done; j++) {
						yp = j + plr[pnum]._py;
						for (i = -l; i <= l && !done; i++) {
							xp = i + plr[pnum]._px;
							if (CanPut(xp, yp)) {
								done = TRUE;
								x = xp;
								y = yp;
							}
						}
					}
				}
				if (!done)
					return -1;
			}
		}
	}

	CanPut(x, y);

	int ii = AllocateItem();

	dItem[x][y] = ii + 1;

	if (idx == IDI_EAR) {
		RecreateEar(ii, icreateinfo, iseed, Id, dur, mdur, ch, mch, ivalue, ibuff);
	} else {
		RecreateItem(ii, idx, icreateinfo, iseed, ivalue);
		if (Id)
			item[ii]._iIdentified = TRUE;
		item[ii]._iDurability = dur;
		item[ii]._iMaxDur = mdur;
		item[ii]._iCharges = ch;
		item[ii]._iMaxCharges = mch;
		item[ii]._iPLToHit = to_hit;
		item[ii]._iMaxDam = max_dam;
		item[ii]._iMinStr = min_str;
		item[ii]._iMinMag = min_mag;
		item[ii]._iMinDex = min_dex;
		item[ii]._iAC = ac;
	}

	item[ii]._ix = x;
	item[ii]._iy = y;
	RespawnItem(ii, TRUE);

	if (currlevel == 21 && x == CornerStone.x && y == CornerStone.y) {
		CornerStone.item = item[ii];
		InitQTextMsg(296);
		quests[Q_CORNSTN]._qlog = 0;
		quests[Q_CORNSTN]._qactive = 3;
	}
	return ii;
}

char CheckInvHLight()
{
	int r, ii, nGold;
	ItemStruct *pi;
	PlayerStruct *p;
	char rv;

	for (r = 0; (DWORD)r < NUM_XY_SLOTS; r++) {
		int xo = RIGHT_PANEL;
		int yo = 0;
		if (r >= SLOTXY_BELT_FIRST) {
			xo = PANEL_LEFT;
			yo = PANEL_TOP;
		}

		if (MouseX >= InvRect[r].X + xo
		    && MouseX < InvRect[r].X + xo + (INV_SLOT_SIZE_PX + 1)
		    && MouseY >= InvRect[r].Y + yo - (INV_SLOT_SIZE_PX + 1)
		    && MouseY < InvRect[r].Y + yo) {
			break;
		}
	}

	if ((DWORD)r >= NUM_XY_SLOTS)
		return -1;

	rv = -1;
	infoclr = COL_WHITE;
	pi = NULL;
	p = &plr[myplr];
	ClearPanel();
	if (r >= SLOTXY_HEAD_FIRST && r <= SLOTXY_HEAD_LAST) {
		rv = INVLOC_HEAD;
		pi = &p->InvBody[rv];
	} else if (r == SLOTXY_RING_LEFT) {
		rv = INVLOC_RING_LEFT;
		pi = &p->InvBody[rv];
	} else if (r == SLOTXY_RING_RIGHT) {
		rv = INVLOC_RING_RIGHT;
		pi = &p->InvBody[rv];
	} else if (r == SLOTXY_AMULET) {
		rv = INVLOC_AMULET;
		pi = &p->InvBody[rv];
	} else if (r >= SLOTXY_HAND_LEFT_FIRST && r <= SLOTXY_HAND_LEFT_LAST) {
		rv = INVLOC_HAND_LEFT;
		pi = &p->InvBody[rv];
	} else if (r >= SLOTXY_HAND_RIGHT_FIRST && r <= SLOTXY_HAND_RIGHT_LAST) {
		pi = &p->InvBody[INVLOC_HAND_LEFT];
		if (pi->_itype == ITYPE_NONE || pi->_iLoc != ILOC_TWOHAND
		    || (p->_pClass == PC_BARBARIAN && (p->InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || p->InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE))) {
			rv = INVLOC_HAND_RIGHT;
			pi = &p->InvBody[rv];
		} else {
			rv = INVLOC_HAND_LEFT;
		}
	} else if (r >= SLOTXY_CHEST_FIRST && r <= SLOTXY_CHEST_LAST) {
		rv = INVLOC_CHEST;
		pi = &p->InvBody[rv];
	} else if (r >= SLOTXY_INV_FIRST && r <= SLOTXY_INV_LAST) {
		r = abs(p->InvGrid[r - SLOTXY_INV_FIRST]);
		if (r == 0)
			return -1;
		ii = r - 1;
		rv = ii + INVITEM_INV_FIRST;
		pi = &p->InvList[ii];
	} else if (r >= SLOTXY_BELT_FIRST) {
		r -= SLOTXY_BELT_FIRST;
		drawsbarflag = TRUE;
		pi = &p->SpdList[r];
		if (pi->_itype == ITYPE_NONE)
			return -1;
		rv = r + INVITEM_BELT_FIRST;
	}

	if (pi->_itype == ITYPE_NONE)
		return -1;

	if (pi->_itype == ITYPE_GOLD) {
		nGold = pi->_ivalue;
		sprintf(infostr, "%i gold %s", nGold, get_pieces_str(nGold));
	} else {
		if (pi->_iMagical == ITEM_QUALITY_MAGIC) {
			infoclr = COL_BLUE;
		} else if (pi->_iMagical == ITEM_QUALITY_UNIQUE) {
			infoclr = COL_GOLD;
		}
		strcpy(infostr, pi->_iName);
		if (pi->_iIdentified) {
			strcpy(infostr, pi->_iIName);
			PrintItemDetails(pi);
		} else {
			PrintItemDur(pi);
		}
	}

	return rv;
}

void RemoveScroll(int pnum)
{
	int i;

	for (i = 0; i < plr[pnum]._pNumInv; i++) {
		if (plr[pnum].InvList[i]._itype != ITYPE_NONE
		    && (plr[pnum].InvList[i]._iMiscId == IMISC_SCROLL || plr[pnum].InvList[i]._iMiscId == IMISC_SCROLLT)
		    && plr[pnum].InvList[i]._iSpell == plr[pnum]._pRSpell) {
			RemoveInvItem(pnum, i);
			CalcPlrScrolls(pnum);
			return;
		}
	}
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (plr[pnum].SpdList[i]._itype != ITYPE_NONE
		    && (plr[pnum].SpdList[i]._iMiscId == IMISC_SCROLL || plr[pnum].SpdList[i]._iMiscId == IMISC_SCROLLT)
		    && plr[pnum].SpdList[i]._iSpell == plr[pnum]._pSpell) {
			RemoveSpdBarItem(pnum, i);
			CalcPlrScrolls(pnum);
			return;
		}
	}
}

BOOL UseScroll()
{
	int i;

	if (pcurs != CURSOR_HAND)
		return FALSE;
	if (leveltype == DTYPE_TOWN && !spelldata[plr[myplr]._pRSpell].sTownSpell)
		return FALSE;

	for (i = 0; i < plr[myplr]._pNumInv; i++) {
		if (plr[myplr].InvList[i]._itype != ITYPE_NONE
		    && (plr[myplr].InvList[i]._iMiscId == IMISC_SCROLL || plr[myplr].InvList[i]._iMiscId == IMISC_SCROLLT)
		    && plr[myplr].InvList[i]._iSpell == plr[myplr]._pRSpell) {
			return TRUE;
		}
	}
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (plr[myplr].SpdList[i]._itype != ITYPE_NONE
		    && (plr[myplr].SpdList[i]._iMiscId == IMISC_SCROLL || plr[myplr].SpdList[i]._iMiscId == IMISC_SCROLLT)
		    && plr[myplr].SpdList[i]._iSpell == plr[myplr]._pRSpell) {
			return TRUE;
		}
	}

	return FALSE;
}

void UseStaffCharge(int pnum)
{
	if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE
	    && (plr[pnum].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_STAFF
	        || plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_UNIQUE // BUGFIX: myplr->pnum
	        )
	    && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iSpell == plr[pnum]._pRSpell
	    && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
		plr[pnum].InvBody[INVLOC_HAND_LEFT]._iCharges--;
		CalcPlrStaff(pnum);
	}
}

BOOL UseStaff()
{
	if (pcurs == CURSOR_HAND) {
		if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE
		    && (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_STAFF || plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_UNIQUE)
		    && plr[myplr].InvBody[INVLOC_HAND_LEFT]._iSpell == plr[myplr]._pRSpell
		    && plr[myplr].InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
			return TRUE;
		}
	}

	return FALSE;
}

void StartGoldDrop()
{
	initialDropGoldIndex = pcursinvitem;
	if (pcursinvitem <= INVITEM_INV_LAST)
		initialDropGoldValue = plr[myplr].InvList[pcursinvitem - INVITEM_INV_FIRST]._ivalue;
	else
		initialDropGoldValue = plr[myplr].SpdList[pcursinvitem - INVITEM_BELT_FIRST]._ivalue;
	dropGoldFlag = TRUE;
	dropGoldValue = 0;
	if (talkflag)
		control_reset_talk();
}

BOOL UseInvItem(int pnum, int cii)
{
	int c, idata;
	ItemStruct *Item;
	BOOL speedlist;

	if (plr[pnum]._pInvincible && plr[pnum]._pHitPoints == 0 && pnum == myplr)
		return TRUE;
	if (pcurs != CURSOR_HAND)
		return TRUE;
	if (stextflag != STORE_NONE)
		return TRUE;
	if (cii < INVITEM_INV_FIRST)
		return FALSE;

	if (cii <= INVITEM_INV_LAST) {
		c = cii - INVITEM_INV_FIRST;
		Item = &plr[pnum].InvList[c];
		speedlist = FALSE;
	} else {
		if (talkflag)
			return TRUE;
		c = cii - INVITEM_BELT_FIRST;
		Item = &plr[pnum].SpdList[c];
		speedlist = TRUE;
	}

	switch (Item->IDidx) {
	case IDI_MUSHROOM:
		sfxdelay = 10;
		if (plr[pnum]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR95;
		} else if (plr[pnum]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE95;
		} else if (plr[pnum]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE95;
		} else if (plr[pnum]._pClass == PC_MONK) {
			sfxdnum = PS_MONK95;
		} else if (plr[pnum]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE95;
		} else if (plr[pnum]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR95;
		}
		return TRUE;
	case IDI_FUNGALTM:
		PlaySFX(IS_IBOOK);
		sfxdelay = 10;
		if (plr[pnum]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR29;
		} else if (plr[pnum]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE29;
		} else if (plr[pnum]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE29;
		} else if (plr[pnum]._pClass == PC_MONK) {
			sfxdnum = PS_MONK29;
		} else if (plr[pnum]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE29;
		} else if (plr[pnum]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR29;
		}
		return TRUE;
	}

	if (!AllItemsList[Item->IDidx].iUsable)
		return FALSE;

	if (!Item->_iStatFlag) {
		if (plr[pnum]._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR13);
		} else if (plr[pnum]._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE13);
		} else if (plr[pnum]._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE13);
		} else if (plr[pnum]._pClass == PC_MONK) {
			PlaySFX(PS_MONK13);
		} else if (plr[pnum]._pClass == PC_BARD) {
			PlaySFX(PS_ROGUE13);
		} else if (plr[pnum]._pClass == PC_BARBARIAN) {
			PlaySFX(PS_WARR13);
		}
		return TRUE;
	}

	if (Item->_iMiscId == IMISC_NONE && Item->_itype == ITYPE_GOLD) {
		StartGoldDrop();
		return TRUE;
	}

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	if (Item->_iMiscId == IMISC_SCROLL && currlevel == 0 && !spelldata[Item->_iSpell].sTownSpell) {
		return TRUE;
	}

	if (Item->_iMiscId == IMISC_SCROLLT && currlevel == 0 && !spelldata[Item->_iSpell].sTownSpell) {
		return TRUE;
	}

	if (Item->_iMiscId > IMISC_RUNEFIRST && Item->_iMiscId < IMISC_RUNELAST && currlevel == 0) {
		return TRUE;
	}

	idata = ItemCAnimTbl[Item->_iCurs];
	if (Item->_iMiscId == IMISC_BOOK)
		PlaySFX(IS_RBOOK);
	else if (pnum == myplr)
		PlaySFX(ItemInvSnds[idata]);

	UseItem(pnum, Item->_iMiscId, Item->_iSpell);

	if (speedlist) {
		if (plr[pnum].SpdList[c]._iMiscId == IMISC_NOTE) {
			InitQTextMsg(322);
			invflag = FALSE;
			return TRUE;
		}
		RemoveSpdBarItem(pnum, c);
		return TRUE;
	} else {
		if (plr[pnum].InvList[c]._iMiscId == IMISC_MAPOFDOOM)
			return TRUE;
		if (plr[pnum].InvList[c]._iMiscId == IMISC_NOTE) {
			InitQTextMsg(322);
			invflag = FALSE;
			return TRUE;
		}
		RemoveInvItem(pnum, c);
	}
	return TRUE;
}

void DoTelekinesis()
{
	if (pcursobj != -1)
		NetSendCmdParam1(TRUE, CMD_OPOBJT, pcursobj);
	if (pcursitem != -1)
		NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, myplr, myplr, pcursitem);
	if (pcursmonst != -1 && !M_Talker(pcursmonst) && monster[pcursmonst].mtalkmsg == 0)
		NetSendCmdParam1(TRUE, CMD_KNOCKBACK, pcursmonst);
	NewCursor(CURSOR_HAND);
}

int CalculateGold(int pnum)
{
	int i, gold;

	gold = 0;
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (plr[pnum].SpdList[i]._itype == ITYPE_GOLD) {
			gold += plr[pnum].SpdList[i]._ivalue;
			force_redraw = 255;
		}
	}
	for (i = 0; i < plr[pnum]._pNumInv; i++) {
		if (plr[pnum].InvList[i]._itype == ITYPE_GOLD)
			gold += plr[pnum].InvList[i]._ivalue;
	}

	return gold;
}

BOOL DropItemBeforeTrig()
{
	if (TryInvPut()) {
		NetSendCmdPItem(TRUE, CMD_PUTITEM, cursmx, cursmy);
		NewCursor(CURSOR_HAND);
		return TRUE;
	}

	return FALSE;
}

DEVILUTION_END_NAMESPACE
