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
#ifdef HELLFIRE
	} else if (plr[myplr]._pClass == PC_MONK) {
		if (gbIsSpawn)
			pInvCels = LoadFileInMem("Data\\Inv\\Inv_Sor.CEL", NULL);
		else
			pInvCels = LoadFileInMem("Data\\Inv\\Inv.CEL", NULL);
	} else if (plr[myplr]._pClass == PC_BARD) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv_rog.CEL", NULL);
	} else if (plr[myplr]._pClass == PC_BARBARIAN) {
		pInvCels = LoadFileInMem("Data\\Inv\\Inv.CEL", NULL);
#endif
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
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelBlitOutline(color, RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		}

		if (plr[myplr].InvBody[INVLOC_HEAD]._iStatFlag) {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
				CelClippedDraw(RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		} else {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelDrawLightRed(RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels, frame, frame_width, 1);
#ifdef HELLFIRE
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 133, 59 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
#endif
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
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelBlitOutline(color, RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		}

		if (plr[myplr].InvBody[INVLOC_RING_LEFT]._iStatFlag) {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelClippedDraw(RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		} else {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelDrawLightRed(RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels, frame, frame_width, 1);
#ifdef HELLFIRE
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 48, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
#endif
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
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelBlitOutline(color, RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		}

		if (plr[myplr].InvBody[INVLOC_RING_RIGHT]._iStatFlag) {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelClippedDraw(RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		} else {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelDrawLightRed(RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels, frame, frame_width, 1);
#ifdef HELLFIRE
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 249, 205 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
#endif
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
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelBlitOutline(color, RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		}

		if (plr[myplr].InvBody[INVLOC_AMULET]._iStatFlag) {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelClippedDraw(RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		} else {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelDrawLightRed(RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels, frame, frame_width, 1);
#ifdef HELLFIRE
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 205, 60 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
#endif
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
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelBlitOutline(color, screen_x, screen_y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelBlitOutline(color, screen_x, screen_y, pCursCels2, frame - 179, frame_width);
			}
#endif
		}

		if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelClippedDraw(screen_x, screen_y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelClippedDraw(screen_x, screen_y, pCursCels2, frame - 179, frame_width);
			}
#endif
		} else {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelDrawLightRed(screen_x, screen_y, pCursCels, frame, frame_width, 1);
#ifdef HELLFIRE
			} else {
				CelDrawLightRed(screen_x, screen_y, pCursCels2, frame - 179, frame_width, 1);
			}
#endif
		}

		if (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND) {
#ifdef HELLFIRE
			if (plr[myplr]._pClass != PC_BARBARIAN
			    || plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_SWORD
			        && plr[myplr].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_MACE) {
#endif
				InvDrawSlotBack(RIGHT_PANEL_X + 248, 160 + SCREEN_Y, 2 * INV_SLOT_SIZE_PX, 3 * INV_SLOT_SIZE_PX);
				light_table_index = 0;
				cel_transparency_active = TRUE;

				pBuff = frame_width == INV_SLOT_SIZE_PX
					? &gpBuffer[SCREENXY(RIGHT_PANEL_X + 197, SCREEN_Y)]
					: &gpBuffer[SCREENXY(RIGHT_PANEL_X + 183, SCREEN_Y)];
#ifdef HELLFIRE
				if (frame <= 179) {
#endif
					CelClippedBlitLightTrans(pBuff, pCursCels, frame, frame_width);
#ifdef HELLFIRE
				} else {
					CelClippedBlitLightTrans(pBuff, pCursCels2, frame - 179, frame_width);
				}
#endif

				cel_transparency_active = FALSE;
#ifdef HELLFIRE
			}
#endif
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
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelBlitOutline(color, screen_x, screen_y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelBlitOutline(color, screen_x, screen_y, pCursCels2, frame - 179, frame_width);
			}
#endif
		}

		if (plr[myplr].InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelClippedDraw(screen_x, screen_y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelClippedDraw(screen_x, screen_y, pCursCels2, frame - 179, frame_width);
			}
#endif
		} else {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelDrawLightRed(screen_x, screen_y, pCursCels, frame, frame_width, 1);
#ifdef HELLFIRE
			} else {
				CelDrawLightRed(screen_x, screen_y, pCursCels2, frame - 179, frame_width, 1);
			}
#endif
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
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelBlitOutline(color, RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelBlitOutline(color, RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		}

		if (plr[myplr].InvBody[INVLOC_CHEST]._iStatFlag) {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelClippedDraw(RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			} else {
				CelClippedDraw(RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels2, frame - 179, frame_width);
			}
#endif
		} else {
#ifdef HELLFIRE
			if (frame <= 179) {
#endif
			CelDrawLightRed(RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels, frame, frame_width, 1);
#ifdef HELLFIRE
			} else {
				CelDrawLightRed(RIGHT_PANEL_X + 133, 160 + SCREEN_Y, pCursCels2, frame - 179, frame_width, 1);
			}
#endif
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
#ifdef HELLFIRE
				if (frame <= 179) {
#endif
				CelBlitOutline(
				    color,
				    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
				    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
				    pCursCels, frame, frame_width);
#ifdef HELLFIRE
				} else {
					CelBlitOutline(
					    color,
					    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
					    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
					    pCursCels2, frame - 179, frame_width);
				}
#endif
			}

			if (plr[myplr].InvList[ii]._iStatFlag) {
#ifdef HELLFIRE
				if (frame <= 179) {
#endif
				CelClippedDraw(
				    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
				    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
				    pCursCels, frame, frame_width);
#ifdef HELLFIRE
				} else {
					CelClippedDraw(
					    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
					    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
					    pCursCels2, frame - 179, frame_width);
				}
#endif
			} else {
#ifdef HELLFIRE
				if (frame <= 179) {
#endif
				CelDrawLightRed(
				    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
				    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
				    pCursCels, frame, frame_width, 1);
#ifdef HELLFIRE
				} else {
					CelDrawLightRed(
					    InvRect[j + SLOTXY_INV_FIRST].X + RIGHT_PANEL_X,
					    InvRect[j + SLOTXY_INV_FIRST].Y + SCREEN_Y - 1,
					    pCursCels2, frame - 179, frame_width, 1);
				}
#endif
			}
		}
	}
}

void DrawInvBelt()
{
	int i, frame, frame_width, color;
	BYTE fi, ff;

	if (talkflag) {
		return;
	}

	DrawPanelBox(205, 21, 232, 28, PANEL_X + 205, PANEL_Y + 5);

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
#ifdef HELLFIRE
			if (frame <= 179)
#endif
			CelBlitOutline(color, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			else
				CelBlitOutline(color, InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels2, frame - 179, frame_width);
#endif
			}
		}

		if (plr[myplr].SpdList[i]._iStatFlag) {
#ifdef HELLFIRE
			if (frame <= 179)
#endif
			CelClippedDraw(InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels, frame, frame_width);
#ifdef HELLFIRE
			else
				CelClippedDraw(InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels2, frame - 179, frame_width);
#endif
		} else {
#ifdef HELLFIRE
			if (frame <= 179)
#endif
			CelDrawLightRed(InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels, frame, frame_width, 1);
#ifdef HELLFIRE
			else
				CelDrawLightRed(InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X, InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, pCursCels2, frame - 179, frame_width, 1);
#endif
		}

		if (AllItemsList[plr[myplr].SpdList[i].IDidx].iUsable
		    && plr[myplr].SpdList[i]._iStatFlag
		    && plr[myplr].SpdList[i]._itype != ITYPE_GOLD) {
			fi = i + 49;
			ff = fontframe[gbFontTransTbl[fi]];
			PrintChar(InvRect[i + SLOTXY_BELT_FIRST].X + PANEL_X + INV_SLOT_SIZE_PX - fontkern[ff], InvRect[i + SLOTXY_BELT_FIRST].Y + PANEL_Y - 1, ff, 0);
		}
	}
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

BOOL SpecialAutoPlace(int pnum, int ii, int sx, int sy, BOOL saveflag)
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
	if (!done) {
		if (sx > 1 || sy > 1) {
			done = FALSE;
		} else {
			for (i = 0; i < MAXBELTITEMS; i++) {
				if (plr[pnum].SpdList[i]._itype == ITYPE_NONE) {
					done = TRUE;
					break;
				}
			}
		}
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

#ifndef HELLFIRE
BOOL GoldAutoPlace(int pnum)
{
	BOOL done;
	int i, ii;
	int xx, yy;

	done = FALSE;
	for (i = 0; i < plr[pnum]._pNumInv && !done; i++) {
		if (plr[pnum].InvList[i]._itype == ITYPE_GOLD) {
			if (plr[pnum].HoldItem._ivalue + plr[pnum].InvList[i]._ivalue <= GOLD_MAX_LIMIT) {
				plr[pnum].InvList[i]._ivalue = plr[pnum].HoldItem._ivalue + plr[pnum].InvList[i]._ivalue;
				if (plr[pnum].InvList[i]._ivalue >= GOLD_MEDIUM_LIMIT)
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_LARGE;
				else if (plr[pnum].InvList[i]._ivalue <= GOLD_SMALL_LIMIT)
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_SMALL;
				else
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_MEDIUM;
				plr[pnum]._pGold = CalculateGold(pnum);
				done = TRUE;
			}
		}
	}
	if (done)
		return done;

	for (i = 0; i < plr[pnum]._pNumInv && !done; i++) {
		if (plr[pnum].InvList[i]._itype == ITYPE_GOLD && plr[pnum].InvList[i]._ivalue < GOLD_MAX_LIMIT) {
			if (plr[pnum].HoldItem._ivalue + plr[pnum].InvList[i]._ivalue <= GOLD_MAX_LIMIT) {
				plr[pnum].InvList[i]._ivalue = plr[pnum].HoldItem._ivalue + plr[pnum].InvList[i]._ivalue;
				if (plr[pnum].InvList[i]._ivalue >= GOLD_MEDIUM_LIMIT)
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_LARGE;
				else if (plr[pnum].InvList[i]._ivalue <= GOLD_SMALL_LIMIT)
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_SMALL;
				else
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_MEDIUM;
				plr[pnum]._pGold = CalculateGold(pnum);
				done = TRUE;
			}
		}
	}
	if (done)
		return done;

	for (i = 39; i >= 0 && !done; i--) {
		yy = 10 * (i / 10);
		xx = i % 10;
		if (!plr[pnum].InvGrid[xx + yy]) {
			ii = plr[pnum]._pNumInv;
			plr[pnum].InvList[ii] = plr[pnum].HoldItem;
			plr[pnum]._pNumInv = plr[pnum]._pNumInv + 1;
			plr[pnum].InvGrid[xx + yy] = plr[pnum]._pNumInv;
			if (plr[pnum].HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
				plr[pnum].InvList[ii]._iCurs = ICURS_GOLD_LARGE;
			else if (plr[pnum].HoldItem._ivalue <= GOLD_SMALL_LIMIT)
				plr[pnum].InvList[ii]._iCurs = ICURS_GOLD_SMALL;
			else
				plr[pnum].InvList[ii]._iCurs = ICURS_GOLD_MEDIUM;
			plr[pnum]._pGold = CalculateGold(pnum);
			done = TRUE;
		}
	}

	return done;
}
#else
BOOL GoldAutoPlace(int pnum)
{
	int ii;
	int xx, yy;
	int max_gold, gold;
	BOOL done;

	done = FALSE;
	for (int i = 0; i < plr[pnum]._pNumInv && !done; i++) {
		if (plr[pnum].InvList[i]._itype == ITYPE_GOLD) {
			gold = plr[pnum].InvList[i]._ivalue + plr[pnum].HoldItem._ivalue;
			if (gold <= MaxGold) {
				plr[pnum].InvList[i]._ivalue = gold;
				if (gold >= GOLD_MEDIUM_LIMIT)
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_LARGE;
				else if (gold <= GOLD_SMALL_LIMIT)
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_SMALL;
				else
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_MEDIUM;
				plr[pnum]._pGold = CalculateGold(pnum);
				done = TRUE;
				plr[pnum].HoldItem._ivalue = 0;
			} else {
				max_gold = MaxGold;
				if (plr[pnum].InvList[i]._ivalue < max_gold) {
					gold = max_gold - plr[pnum].InvList[i]._ivalue;
					plr[pnum].InvList[i]._ivalue = max_gold;
					plr[pnum].InvList[i]._iCurs = ICURS_GOLD_LARGE;
					plr[pnum].HoldItem._ivalue -= gold;
					if (plr[pnum].HoldItem._ivalue < 0) {
						plr[pnum].HoldItem._ivalue = 0;
						done = TRUE;
					}
					GetPlrHandSeed(&plr[pnum].HoldItem);
					control_set_gold_curs(pnum);
					plr[pnum]._pGold = CalculateGold(pnum);
				}
			}
		}
	}
	if (!done)
		for (int i = 39; i >= 0 && !done; i--) {
			yy = 10 * (i / 10);
			xx = i % 10;
			if (!plr[pnum].InvGrid[xx + yy]) {
				ii = plr[pnum]._pNumInv;
				plr[pnum].InvList[ii] = plr[pnum].HoldItem;
				plr[pnum]._pNumInv = plr[pnum]._pNumInv + 1;
				plr[pnum].InvGrid[xx + yy] = plr[pnum]._pNumInv;
				if (plr[pnum].HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
					plr[pnum].InvList[ii]._iCurs = ICURS_GOLD_LARGE;
				else if (plr[pnum].HoldItem._ivalue <= GOLD_SMALL_LIMIT)
					plr[pnum].InvList[ii]._iCurs = ICURS_GOLD_SMALL;
				else
					plr[pnum].InvList[ii]._iCurs = ICURS_GOLD_MEDIUM;

				gold = plr[pnum].HoldItem._ivalue;
				if (gold > MaxGold) {
					gold -= MaxGold;
					plr[pnum].HoldItem._ivalue = gold;
					GetPlrHandSeed(&plr[pnum].HoldItem);
					plr[pnum].InvList[ii]._ivalue = MaxGold;
				} else {
					plr[pnum].HoldItem._ivalue = 0;
					done = TRUE;
					plr[pnum]._pGold = CalculateGold(pnum);
					SetCursor_(CURSOR_HAND);
				}
			}
		}
	return done;
}
#endif

BOOL WeaponAutoPlace(int pnum)
{
#ifdef HELLFIRE
	if (plr[pnum]._pClass == PC_MONK)
		return FALSE;
#endif
	if (plr[pnum].HoldItem._iLoc != ILOC_TWOHAND
#ifdef HELLFIRE
	    || (plr[pnum]._pClass == PC_BARBARIAN && (plr[pnum].HoldItem._itype == ITYPE_SWORD || plr[pnum].HoldItem._itype == ITYPE_MACE))
#endif
	) {
#ifdef HELLFIRE
		if (plr[pnum]._pClass != PC_BARD)
#endif
		{
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
			if ((sy & 1) == 0) {
				j -= 14;
			}
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
				if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass != plr[pnum].HoldItem._iClass) {
					NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
					plr[pnum].InvBody[INVLOC_HAND_LEFT] = plr[pnum].HoldItem;
				} else {
					NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
					cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
				}
				break;
			}
			if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE || plr[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass != plr[pnum].HoldItem._iClass) {
				NetSendCmdChItem(FALSE, INVLOC_HAND_LEFT);
				cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_LEFT], &plr[pnum].HoldItem);
				break;
			}

			NetSendCmdChItem(FALSE, INVLOC_HAND_RIGHT);
			cn = SwapItem(&plr[pnum].InvBody[INVLOC_HAND_RIGHT], &plr[pnum].HoldItem);
			break;
		}
		if (plr[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE) {
			if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE || plr[pnum].InvBody[INVLOC_HAND_LEFT]._iLoc != ILOC_TWOHAND) {
				if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE || plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass != plr[pnum].HoldItem._iClass) {
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

		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == plr[pnum].HoldItem._iClass) {
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
		if (plr[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iSpell != 0 && plr[pnum].InvBody[INVLOC_HAND_LEFT]._iCharges > 0) {
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
					if (ig >= GOLD_MEDIUM_LIMIT)
						plr[pnum].InvList[il]._iCurs = ICURS_GOLD_LARGE;
					else if (ig <= GOLD_SMALL_LIMIT)
						plr[pnum].InvList[il]._iCurs = ICURS_GOLD_SMALL;
					else
						plr[pnum].InvList[il]._iCurs = ICURS_GOLD_MEDIUM;
				} else {
					ig = GOLD_MAX_LIMIT - gt;
					plr[pnum]._pGold += ig;
					plr[pnum].HoldItem._ivalue -= ig;
					plr[pnum].InvList[il]._ivalue = GOLD_MAX_LIMIT;
					plr[pnum].InvList[il]._iCurs = ICURS_GOLD_LARGE;
					// BUGFIX: incorrect values here are leftover from beta
					if (plr[pnum].HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
						cn = 18;
					else if (plr[pnum].HoldItem._ivalue <= GOLD_SMALL_LIMIT)
						cn = 16;
					else
						cn = 17;
				}
			} else {
				il = plr[pnum]._pNumInv;
				plr[pnum].InvList[il] = plr[pnum].HoldItem;
				plr[pnum]._pNumInv++;
				plr[pnum].InvGrid[yy + xx] = plr[pnum]._pNumInv;
				plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
				if (plr[pnum].HoldItem._ivalue <= GOLD_MAX_LIMIT) {
					if (plr[pnum].HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
						plr[pnum].InvList[il]._iCurs = ICURS_GOLD_LARGE;
					else if (plr[pnum].HoldItem._ivalue <= GOLD_SMALL_LIMIT)
						plr[pnum].InvList[il]._iCurs = ICURS_GOLD_SMALL;
					else
						plr[pnum].InvList[il]._iCurs = ICURS_GOLD_MEDIUM;
				}
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
						plr[pnum].SpdList[ii]._ivalue += plr[pnum].HoldItem._ivalue;
						plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
						if (i >= GOLD_MEDIUM_LIMIT)
							plr[pnum].SpdList[ii]._iCurs = ICURS_GOLD_LARGE;
						else if (i <= GOLD_SMALL_LIMIT)
							plr[pnum].SpdList[ii]._iCurs = ICURS_GOLD_SMALL;
						else
							plr[pnum].SpdList[ii]._iCurs = ICURS_GOLD_MEDIUM;
					} else {
						i = GOLD_MAX_LIMIT - plr[pnum].SpdList[ii]._ivalue;
						plr[pnum]._pGold += i;
						plr[pnum].HoldItem._ivalue -= i;
						plr[pnum].SpdList[ii]._ivalue = GOLD_MAX_LIMIT;
						plr[pnum].SpdList[ii]._iCurs = ICURS_GOLD_LARGE;

						// BUGFIX: incorrect values here are leftover from beta
						if (plr[pnum].HoldItem._ivalue >= GOLD_MEDIUM_LIMIT)
							cn = 18;
						else if (plr[pnum].HoldItem._ivalue <= GOLD_SMALL_LIMIT)
							cn = 16;
						else
							cn = 17;
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

	if (plr[pnum]._pRSplType == RSPLTYPE_SCROLL) {
		if (plr[pnum]._pRSpell != SPL_INVALID) {
			// BUGFIX: Cast the literal `1` to `unsigned __int64` to make that bitshift 64bit
			// this causes the last 4 skills to not reset correctly after use
			if (!(
			        plr[pnum]._pScrlSpells
			        & (1 << (plr[pnum]._pRSpell - 1)))) {
				plr[pnum]._pRSpell = SPL_INVALID;
			}

			force_redraw = 255;
		}
	}
}

#ifdef HELLFIRE
/**
 * @brief This destroyes all items except gold
 */
BOOL inv_diablo_to_hellfire(int pnum)
{
	ItemStruct tmp;
	ItemStruct *item;
	int i, old_item_cnt, new_item_index;

	if (plr[pnum]._pgfxnum != 0) {
		plr[pnum]._pgfxnum = 0;
		plr[pnum]._pGFXLoad = 0;
		SetPlrAnims(pnum);
	}
	for (i = 0, item = plr[pnum].InvBody; i < NUM_INVLOC; i++, item++) {
		item->_itype = ITYPE_NONE;
	}
	old_item_cnt = plr[pnum]._pNumInv;
	memset(plr[pnum].InvGrid, 0, sizeof(plr[pnum].InvGrid));
	plr[pnum]._pNumInv = 0;
	for (i = 0; i < old_item_cnt; i++) {
		item = &plr[pnum].InvList[i];
		if (item->_itype == ITYPE_GOLD) {
			new_item_index = plr[pnum]._pNumInv;
			// BUGFIX: new_item_index may be greater or equal to NUM_INV_GRID_ELEM
			tmp = *item;
			item->_itype = ITYPE_NONE;
			plr[pnum].InvList[new_item_index] = tmp;
			plr[pnum]._pNumInv++;
			plr[pnum].InvGrid[i] = plr[pnum]._pNumInv;
		} else {
			item->_itype = ITYPE_NONE;
		}
	};
	for (i = 0, item = plr[pnum].SpdList; i < MAXBELTITEMS; i++, item++) {
		item->_itype = ITYPE_NONE;
	}
	CalcPlrItemVals(pnum, FALSE);
	return FALSE;
}
#endif

void RemoveSpdBarItem(int pnum, int iv)
{
	plr[pnum].SpdList[iv]._itype = ITYPE_NONE;

	CalcPlrScrolls(pnum);

	if (plr[pnum]._pRSplType == RSPLTYPE_SCROLL) {
		if (plr[pnum]._pRSpell != SPL_INVALID) {
			// BUGFIX: Cast the literal `1` to `unsigned __int64` to make that bitshift 64bit
			// this causes the last 4 skills to not reset correctly after use
			if (!(
			        plr[pnum]._pScrlSpells
			        & (1 << (plr[pnum]._pRSpell - 1)))) {
				plr[pnum]._pRSpell = SPL_INVALID;
			}
		}
	}
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
	if (plr[pnum].HoldItem.IDidx == IDI_OPTAMULET)
		quests[Q_BLIND]._qactive = QUEST_DONE;
	if (plr[pnum].HoldItem.IDidx == IDI_MUSHROOM && quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE && quests[Q_MUSHROOM]._qvar1 == QS_MUSHSPAWNED) {
		sfxdelay = 10;
		if (plr[pnum]._pClass == PC_WARRIOR) { // BUGFIX: Voice for this quest might be wrong in MP
			sfxdnum = PS_WARR95;
		} else if (plr[pnum]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE95;
		} else if (plr[pnum]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE95;
		}
#ifdef HELLFIRE
		else if (plr[pnum]._pClass == PC_MONK) {
			sfxdnum = PS_MONK95;
		} else if (plr[pnum]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE95;
		} else if (plr[pnum]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR95;
		}
#endif
		quests[Q_MUSHROOM]._qvar1 = QS_MUSHPICKED;
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ANVIL) {
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
			}
#ifdef HELLFIRE
			else if (plr[myplr]._pClass == PC_MONK) {
				sfxdnum = PS_MONK89;
			} else if (plr[myplr]._pClass == PC_BARD) {
				sfxdnum = PS_ROGUE89;
			} else if (plr[myplr]._pClass == PC_BARBARIAN) {
				sfxdnum = PS_WARR89;
			}
#endif
		}
	}
	if (plr[pnum].HoldItem.IDidx == IDI_GLDNELIX) {
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR88;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE88;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE88;
		}
#ifdef HELLFIRE
		else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK88;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE88;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR88;
		}
#endif
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ROCK) {
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
			}
#ifdef HELLFIRE
			else if (plr[myplr]._pClass == PC_MONK) {
				sfxdnum = PS_MONK87;
			} else if (plr[myplr]._pClass == PC_BARD) {
				sfxdnum = PS_ROGUE87;
			} else if (plr[myplr]._pClass == PC_BARBARIAN) {
				sfxdnum = PS_WARR87;
			}
#endif
		}
	}
	if (plr[pnum].HoldItem.IDidx == IDI_ARMOFVAL) {
		quests[Q_BLOOD]._qactive = QUEST_DONE;
		sfxdelay = 20;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR91;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE91;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE91;
#ifdef HELLFIRE
		} else if (plr[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK91;
		} else if (plr[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE91;
		} else if (plr[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR91;
#endif
		}
	}
#ifdef HELLFIRE
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
			GetItemAttrs(item_num, IDI_FULLNOTE, 16);
			SetupItem(item_num);
			plr[pnum].HoldItem = item[item_num];
			item[item_num] = tmp;
		}
	}
#endif
}

void InvGetItem(int pnum, int ii)
{
	int i;
#ifdef HELLFIRE
	BOOL cursor_updated;
#endif

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	if (dItem[item[ii]._ix][item[ii]._iy] != 0) {
		if (myplr == pnum && pcurs >= CURSOR_FIRSTITEM)
			NetSendCmdPItem(TRUE, CMD_SYNCPUTITEM, plr[myplr]._px, plr[myplr]._py);
#ifdef HELLFIRE
		if (item[ii]._iUid != 0)
#endif
			item[ii]._iCreateInfo &= ~CF_PREGEN;
		plr[pnum].HoldItem = item[ii];
		CheckQuestItem(pnum);
		CheckBookLevel(pnum);
		CheckItemStats(pnum);
#ifdef HELLFIRE
		cursor_updated = FALSE;
		if (plr[pnum].HoldItem._itype == ITYPE_GOLD && GoldAutoPlace(pnum))
			cursor_updated = TRUE;
#endif
		dItem[item[ii]._ix][item[ii]._iy] = 0;
#ifdef HELLFIRE
		if (currlevel == 21 && item[ii]._ix == CornerStone.x && item[ii]._iy == CornerStone.y) {
			CornerStone.item.IDidx = -1;
			CornerStone.item._itype = ITYPE_MISC;
			CornerStone.item._iSelFlag = FALSE;
			CornerStone.item._ix = 0;
			CornerStone.item._iy = 0;
			CornerStone.item._iAnimFlag = FALSE;
			CornerStone.item._iIdentified = FALSE;
			CornerStone.item._iPostDraw = FALSE;
		}
#endif
		i = 0;
		while (i < numitems) {
			if (itemactive[i] == ii) {
				DeleteItem(itemactive[i], i);
				i = 0;
			} else {
				i++;
			}
		}
		pcursitem = -1;
#ifdef HELLFIRE
		if (!cursor_updated)
#endif
			SetCursor_(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
	}
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

#ifdef HELLFIRE
	if (item[ii]._iUid != 0)
#endif
		item[ii]._iCreateInfo &= ~CF_PREGEN;
	plr[pnum].HoldItem = item[ii]; /// BUGFIX: overwrites cursor item, allowing for belt dupe bug
	CheckQuestItem(pnum);
	CheckBookLevel(pnum);
	CheckItemStats(pnum);
	SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
	if (plr[pnum].HoldItem._itype == ITYPE_GOLD) {
		done = GoldAutoPlace(pnum);
#ifdef HELLFIRE
		if (!done)
			item[ii]._ivalue = plr[pnum].HoldItem._ivalue;
#endif
	} else {
		done = FALSE;
		if (((plr[pnum]._pgfxnum & 0xF) == ANIM_ID_UNARMED || (plr[pnum]._pgfxnum & 0xF) == ANIM_ID_UNARMED_SHIELD
#ifdef HELLFIRE
		        || plr[pnum]._pClass == PC_BARD && ((plr[pnum]._pgfxnum & 0xF) == ANIM_ID_MACE || (plr[pnum]._pgfxnum & 0xF) == ANIM_ID_SWORD)
#endif
		            )
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
		dItem[item[ii]._ix][item[ii]._iy] = 0;
#ifdef HELLFIRE
		if (currlevel == 21 && item[ii]._ix == CornerStone.x && item[ii]._iy == CornerStone.y) {
			CornerStone.item.IDidx = -1;
			CornerStone.item._itype = ITYPE_MISC;
			CornerStone.item._iSelFlag = FALSE;
			CornerStone.item._ix = 0;
			CornerStone.item._iy = 0;
			CornerStone.item._iAnimFlag = FALSE;
			CornerStone.item._iIdentified = FALSE;
			CornerStone.item._iPostDraw = FALSE;
		}
#endif
		i = 0;
		while (i < numitems) {
			if (itemactive[i] == ii) {
				DeleteItem(itemactive[i], i);
				i = 0;
			} else {
				i++;
			}
		}
	} else {
		if (pnum == myplr) {
			if (plr[pnum]._pClass == PC_WARRIOR) {
				PlaySFX(random_(0, 3) + PS_WARR14);
			} else if (plr[pnum]._pClass == PC_ROGUE) {
				PlaySFX(random_(0, 3) + PS_ROGUE14);
			} else if (plr[pnum]._pClass == PC_SORCERER) {
				PlaySFX(random_(0, 3) + PS_MAGE14);
#ifdef HELLFIRE
			} else if (plr[pnum]._pClass == PC_MONK) {
				PlaySFX(random_(0, 3) + PS_MONK14);
			} else if (plr[pnum]._pClass == PC_BARD) {
				PlaySFX(random_(0, 3) + PS_ROGUE14);
			} else if (plr[pnum]._pClass == PC_BARBARIAN) {
				PlaySFX(random_(0, 3) + PS_WARR14);
#endif
			}
		}
		plr[pnum].HoldItem = item[ii];
		RespawnItem(ii, TRUE);
		NetSendCmdPItem(TRUE, CMD_RESPAWNITEM, item[ii]._ix, item[ii]._iy);
		plr[pnum].HoldItem._itype = ITYPE_NONE;
#ifdef HELLFIRE
		NewCursor(CURSOR_HAND);
#endif
	}
}

int FindGetItem(int idx, WORD ci, int iseed)
{
	int i, ii;

	i = 0;
	if (numitems <= 0)
		return -1;

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

	if (ii != -1) {
		dItem[item[ii]._ix][item[ii]._iy] = 0;
#ifdef HELLFIRE
		if (currlevel == 21 && item[ii]._ix == CornerStone.x && item[ii]._iy == CornerStone.y) {
			CornerStone.item.IDidx = -1;
			CornerStone.item._itype = ITYPE_MISC;
			CornerStone.item._iSelFlag = FALSE;
			CornerStone.item._ix = 0;
			CornerStone.item._iy = 0;
			CornerStone.item._iAnimFlag = FALSE;
			CornerStone.item._iIdentified = FALSE;
			CornerStone.item._iPostDraw = FALSE;
		}
#endif
		i = 0;
		while (i < numitems) {
			if (itemactive[i] == ii) {
				DeleteItem(itemactive[i], i);
				FindGetItem(idx, ci, iseed);
#ifndef HELLFIRE
				/// ASSERT: assert(FindGetItem(idx,ci,iseed) == -1);
				FindGetItem(idx, ci, iseed); /* todo: replace with above */
#endif
				i = 0;
			} else {
				i++;
			}
		}
		/// ASSERT: assert(FindGetItem(idx, ci, iseed) == -1);
		FindGetItem(idx, ci, iseed); /* todo: replace with above */
	}
}

BOOL CanPut(int x, int y)
{
	char oi, oi2;

	if (dItem[x][y])
		return FALSE;
	if (nSolidTable[dPiece[x][y]])
		return FALSE;

	if (dObject[x][y] != 0) {
		if (object[dObject[x][y] > 0 ? dObject[x][y] - 1 : -1 - dObject[x][y]]._oSolidFlag)
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
	int d, ii;
	int i, j, l;
	int xx, yy;
	int xp, yp;

	if (numitems >= MAXITEMS)
		return -1;

	if (FindGetItem(plr[pnum].HoldItem.IDidx, plr[pnum].HoldItem._iCreateInfo, plr[pnum].HoldItem._iSeed) != -1) {
		DrawInvMsg("A duplicate item has been detected.  Destroying duplicate...");
		SyncGetItem(x, y, plr[pnum].HoldItem.IDidx, plr[pnum].HoldItem._iCreateInfo, plr[pnum].HoldItem._iSeed);
	}

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

#ifdef HELLFIRE
	if (currlevel == 0) {
		yp = cursmy;
		xp = cursmx;
		if (plr[pnum].HoldItem._iCurs == ICURS_RUNE_BOMB && xp >= 79 && xp <= 82 && yp >= 61 && yp <= 64) {
			NetSendCmdLocParam2(0, CMD_OPENHIVE, plr[pnum]._px, plr[pnum]._py, xx, yy);
			quests[Q_FARMER]._qactive = 3;
			if (gbMaxPlayers != 1) {
				NetSendCmdQuest(TRUE, Q_FARMER);
				return -1;
			}
			return -1;
		}
		if (plr[pnum].HoldItem.IDidx == IDI_MAPOFDOOM && xp >= 35 && xp <= 38 && yp >= 20 && yp <= 24) {
			NetSendCmd(FALSE, CMD_OPENCRYPT);
			quests[Q_GRAVE]._qactive = 3;
			if (gbMaxPlayers != 1) {
				NetSendCmdQuest(TRUE, Q_GRAVE);
			}
			return -1;
		}
	}
#endif
	CanPut(x, y); //if (!CanPut(x, y)) {
	//	assertion_failed(__LINE__, __FILE__, "CanPut(x,y)");
	//}

	ii = itemavail[0];
	dItem[x][y] = ii + 1;
	itemavail[0] = itemavail[MAXITEMS - (numitems + 1)];
	itemactive[numitems] = ii;
	item[ii] = plr[pnum].HoldItem;
	item[ii]._ix = x;
	item[ii]._iy = y;
	RespawnItem(ii, TRUE);
	numitems++;
#ifdef HELLFIRE
	if (currlevel == 21 && x == CornerStone.x && y == CornerStone.y) {
		CornerStone.item = item[ii];
		InitQTextMsg(296);
		quests[Q_CORNSTN]._qlog = FALSE;
		quests[Q_CORNSTN]._qactive = QUEST_DONE;
	}
#endif
	NewCursor(CURSOR_HAND);
	return ii;
}

int SyncPutItem(int pnum, int x, int y, int idx, WORD icreateinfo, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, DWORD ibuff
#ifdef HELLFIRE
    ,
    int to_hit, int max_dam, int min_str, int min_mag, int min_dex, int ac
#endif
)
{
	BOOL done;
	int d, ii;
	int i, j, l;
	int xx, yy;
	int xp, yp;

	if (numitems >= MAXITEMS)
		return -1;

	if (FindGetItem(idx, icreateinfo, iseed) != -1) {
		DrawInvMsg("A duplicate item has been detected from another player.");
		SyncGetItem(x, y, idx, icreateinfo, iseed);
	}

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

	ii = itemavail[0];
	dItem[x][y] = ii + 1;
	itemavail[0] = itemavail[MAXITEMS - (numitems + 1)];
	itemactive[numitems] = ii;

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
#ifdef HELLFIRE
		item[ii]._iPLToHit = to_hit;
		item[ii]._iMaxDam = max_dam;
		item[ii]._iMinStr = min_str;
		item[ii]._iMinMag = min_mag;
		item[ii]._iMinDex = min_dex;
		item[ii]._iAC = ac;
#endif
	}

	item[ii]._ix = x;
	item[ii]._iy = y;
	RespawnItem(ii, TRUE);
	numitems++;
#ifdef HELLFIRE
	if (currlevel == 21 && x == CornerStone.x && y == CornerStone.y) {
		CornerStone.item = item[ii];
		InitQTextMsg(296);
		quests[Q_CORNSTN]._qlog = 0;
		quests[Q_CORNSTN]._qactive = 3;
	}
#endif
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
#ifdef HELLFIRE
		if (pi->_itype == ITYPE_NONE || pi->_iLoc != ILOC_TWOHAND
		    || (p->_pClass == PC_BARBARIAN && (p->InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || p->InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE))) {
#else
		if (pi->_itype == ITYPE_NONE || pi->_iLoc != ILOC_TWOHAND) {
#endif
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
#ifndef HELLFIRE
		    && plr[pnum].InvList[i]._iSpell == plr[pnum]._pRSpell) {
#else
		    && plr[pnum].InvList[i]._iSpell == plr[pnum]._pSpell) {
#endif
			RemoveInvItem(pnum, i);
			CalcPlrScrolls(pnum);
			return;
		}
	}
	for (i = 0; i < MAXBELTITEMS; i++) {
		if (plr[pnum].SpdList[i]._itype != ITYPE_NONE
		    && (plr[pnum].SpdList[i]._iMiscId == IMISC_SCROLL || plr[pnum].SpdList[i]._iMiscId == IMISC_SCROLLT)
#ifndef HELLFIRE
		    && plr[pnum].SpdList[i]._iSpell == plr[pnum]._pRSpell) {
#else
		    && plr[pnum].SpdList[i]._iSpell == plr[pnum]._pSpell) {
#endif
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
#ifdef HELLFIRE
	           || plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_UNIQUE // BUGFIX: myplr->pnum
#endif
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
#ifdef HELLFIRE
		    && (plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_STAFF || plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_UNIQUE)
#else
		    && plr[myplr].InvBody[INVLOC_HAND_LEFT]._iMiscId == IMISC_STAFF
#endif
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
	if (cii <= INVITEM_HAND_RIGHT)
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
		}
#ifdef HELLFIRE
		else if (plr[pnum]._pClass == PC_MONK) {
			sfxdnum = PS_MONK95;
		} else if (plr[pnum]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE95;
		} else if (plr[pnum]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR95;
		}
#endif
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
#ifdef HELLFIRE
		} else if (plr[pnum]._pClass == PC_MONK) {
			sfxdnum = PS_MONK29;
		} else if (plr[pnum]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE29;
		} else if (plr[pnum]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR29;
#endif
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
#ifdef HELLFIRE
		} else if (plr[pnum]._pClass == PC_MONK) {
			PlaySFX(PS_MONK13);
		} else if (plr[pnum]._pClass == PC_BARD) {
			PlaySFX(PS_ROGUE13);
		} else if (plr[pnum]._pClass == PC_BARBARIAN) {
			PlaySFX(PS_WARR13);
#endif
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

#ifdef HELLFIRE
	if (Item->_iMiscId > IMISC_RUNEFIRST && Item->_iMiscId < IMISC_RUNELAST && currlevel == 0) {
		return TRUE;
	}
#endif

	idata = ItemCAnimTbl[Item->_iCurs];
	if (Item->_iMiscId == IMISC_BOOK)
		PlaySFX(IS_RBOOK);
	else if (pnum == myplr)
		PlaySFX(ItemInvSnds[idata]);

	UseItem(pnum, Item->_iMiscId, Item->_iSpell);

	if (speedlist) {
#ifdef HELLFIRE
		if (plr[pnum].SpdList[c]._iMiscId == IMISC_NOTE) {
			InitQTextMsg(322);
			invflag = FALSE;
			return TRUE;
		}
#endif
		RemoveSpdBarItem(pnum, c);
		return TRUE;
	} else {
		if (plr[pnum].InvList[c]._iMiscId == IMISC_MAPOFDOOM)
			return TRUE;
#ifdef HELLFIRE
		if (plr[pnum].InvList[c]._iMiscId == IMISC_NOTE) {
			InitQTextMsg(322);
			invflag = FALSE;
			return TRUE;
		}
#endif
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
