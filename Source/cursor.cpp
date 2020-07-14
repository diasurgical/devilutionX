/**
 * @file cursor.cpp
 *
 * Implementation of cursor tracking functionality.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int cursW;
int cursH;
int pcursmonst = -1;
int icursW28;
int icursH28;
BYTE *pCursCels;

/** inv_item value */
char pcursinvitem;
int icursW;
int icursH;
char pcursitem;
char pcursobj;
char pcursplr;
int cursmx;
int cursmy;
int pcurstemp;
int pcurs;

/* rdata */
/** Maps from objcurs.cel frame number to frame width. */
const int InvItemWidth[] = {
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
	// clang-format on
};

/** Maps from objcurs.cel frame number to frame height. */
const int InvItemHeight[] = {
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
	// clang-format on
};

void InitCursor()
{
	assert(!pCursCels);
	pCursCels = LoadFileInMem("Data\\Inv\\Objcurs.CEL", NULL);
	ClearCursor();
}

void FreeCursor()
{
	MemFreeDbg(pCursCels);
	ClearCursor();
}

void SetICursor(int i)
{
	icursW = InvItemWidth[i];
	icursH = InvItemHeight[i];
	icursW28 = icursW / 28;
	icursH28 = icursH / 28;
}

void SetCursor_(int i)
{
	pcurs = i;
	cursW = InvItemWidth[i];
	cursH = InvItemHeight[i];
	SetICursor(i);
}

void NewCursor(int i)
{
	SetCursor_(i);
}

void InitLevelCursor()
{
	SetCursor_(CURSOR_HAND);
	cursmx = ViewX;
	cursmy = ViewY;
	pcurstemp = -1;
	pcursmonst = -1;
	pcursobj = -1;
	pcursitem = -1;
	pcursplr = -1;
	ClearCursor();
}

void CheckTown()
{
	int i, mx;

	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_TOWN) {
			if (cursmx == missile[mx]._mix - 1 && cursmy == missile[mx]._miy
			    || cursmx == missile[mx]._mix && cursmy == missile[mx]._miy - 1
			    || cursmx == missile[mx]._mix - 1 && cursmy == missile[mx]._miy - 1
			    || cursmx == missile[mx]._mix - 2 && cursmy == missile[mx]._miy - 1
			    || cursmx == missile[mx]._mix - 2 && cursmy == missile[mx]._miy - 2
			    || cursmx == missile[mx]._mix - 1 && cursmy == missile[mx]._miy - 2
			    || cursmx == missile[mx]._mix && cursmy == missile[mx]._miy) {
				trigflag = TRUE;
				ClearPanel();
				strcpy(infostr, "Town Portal");
				sprintf(tempstr, "from %s", plr[missile[mx]._misource]._pName);
				AddPanelString(tempstr, TRUE);
				cursmx = missile[mx]._mix;
				cursmy = missile[mx]._miy;
			}
		}
	}
}

void CheckRportal()
{
	int i, mx;

	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_RPORTAL) {
			if (cursmx == missile[mx]._mix - 1 && cursmy == missile[mx]._miy
			    || cursmx == missile[mx]._mix && cursmy == missile[mx]._miy - 1
			    || cursmx == missile[mx]._mix - 1 && cursmy == missile[mx]._miy - 1
			    || cursmx == missile[mx]._mix - 2 && cursmy == missile[mx]._miy - 1
			    || cursmx == missile[mx]._mix - 2 && cursmy == missile[mx]._miy - 2
			    || cursmx == missile[mx]._mix - 1 && cursmy == missile[mx]._miy - 2
			    || cursmx == missile[mx]._mix && cursmy == missile[mx]._miy) {
				trigflag = TRUE;
				ClearPanel();
				strcpy(infostr, "Portal to");
				if (!setlevel)
					strcpy(tempstr, "The Unholy Altar");
				else
					strcpy(tempstr, "level 15");
				AddPanelString(tempstr, TRUE);
				cursmx = missile[mx]._mix;
				cursmy = missile[mx]._miy;
			}
		}
	}
}

void CheckCursMove()
{
	int i, sx, sy, fx, fy, mx, my, tx, ty, px, py, xx, yy, mi, columns, rows, xo, yo;
	char bv;
	BOOL flipflag, flipx, flipy;

	sx = MouseX;
	sy = MouseY;

	if (PANELS_COVER) {
		if (chrflag || questlog) {
			if (sx >= SCREEN_WIDTH / 2) { /// BUGFIX: (sx >= SCREEN_WIDTH / 2) (fixed)
				sx -= SCREEN_WIDTH / 4;
			} else {
				sx = 0;
			}
		} else if (invflag || sbookflag) {
			if (sx <= SCREEN_WIDTH / 2) {
				sx += SCREEN_WIDTH / 4;
			} else {
				sx = 0;
			}
		}
	}
	if (sy > PANEL_TOP - 1 && MouseX >= PANEL_LEFT && MouseX < PANEL_LEFT + PANEL_WIDTH && track_isscrolling()) {
		sy = PANEL_TOP - 1;
	}

	if (!zoomflag) {
		sx >>= 1;
		sy >>= 1;
	}

	// Adjust by player offset and tile grid alignment
	CalcTileOffset(&xo, &yo);
	sx -= ScrollInfo._sxoff - xo;
	sy -= ScrollInfo._syoff - yo;

	// Predict the next frame when walking to avoid input jitter
	fx = plr[myplr]._pVar6 / 256;
	fy = plr[myplr]._pVar7 / 256;
	fx -= (plr[myplr]._pVar6 + plr[myplr]._pxvel) / 256;
	fy -= (plr[myplr]._pVar7 + plr[myplr]._pyvel) / 256;
	if (ScrollInfo._sdir != SDIR_NONE) {
		sx -= fx;
		sy -= fy;
	}

	// Convert to tile grid
	mx = ViewX;
	my = ViewY;
	tx = sx / TILE_WIDTH;
	ty = sy / TILE_HEIGHT;
	ShiftGrid(&mx, &my, tx, ty);

	// Center player tile on screen
	TilesInView(&columns, &rows);
	ShiftGrid(&mx, &my, -columns / 2, -(rows - RowsCoveredByPanel()) / 4);
	if ((columns % 2) != 0) {
		my++;
	}

	// Shift position to match diamond grid aligment
	px = sx % TILE_WIDTH;
	py = sy % TILE_HEIGHT;

	// Shift position to match diamond grid aligment
	flipy = py < (px >> 1);
	if (flipy) {
		my--;
	}
	flipx = py >= TILE_HEIGHT - (px >> 1);
	if (flipx) {
		mx++;
	}

	if (mx < 0) {
		mx = 0;
	}
	if (mx >= MAXDUNX) {
		mx = MAXDUNX - 1;
	}
	if (my < 0) {
		my = 0;
	}
	if (my >= MAXDUNY) {
		my = MAXDUNY - 1;
	}

	flipflag = flipy && flipx || (flipy || flipx) && px < TILE_WIDTH / 2;

	pcurstemp = pcursmonst;
	pcursmonst = -1;
	pcursobj = -1;
	pcursitem = -1;
	if (pcursinvitem != -1) {
		drawsbarflag = TRUE;
	}
	pcursinvitem = -1;
	pcursplr = -1;
	uitemflag = FALSE;
	panelflag = FALSE;
	trigflag = FALSE;

	if (plr[myplr]._pInvincible) {
		return;
	}
	if (pcurs >= CURSOR_FIRSTITEM || spselflag) {
		cursmx = mx;
		cursmy = my;
		return;
	}
	if (MouseY > PANEL_TOP && MouseX >= PANEL_LEFT && MouseX <= PANEL_LEFT + PANEL_WIDTH) {
		CheckPanelInfo();
		return;
	}
	if (doomflag) {
		return;
	}
	if (invflag && MouseX > RIGHT_PANEL && MouseY <= SPANEL_HEIGHT) {
		pcursinvitem = CheckInvHLight();
		return;
	}
	if (sbookflag && MouseX > RIGHT_PANEL && MouseY <= SPANEL_HEIGHT) {
		return;
	}
	if ((chrflag || questlog) && MouseX < SPANEL_WIDTH && MouseY <= SPANEL_HEIGHT) {
		return;
	}

	if (leveltype != DTYPE_TOWN) {
		if (pcurstemp != -1) {
			if (!flipflag && mx + 2 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 2][my + 1] != 0 && dFlags[mx + 2][my + 1] & BFLAG_LIT) {
				mi = dMonster[mx + 2][my + 1] > 0 ? dMonster[mx + 2][my + 1] - 1 : -(dMonster[mx + 2][my + 1] + 1);
				if (mi == pcurstemp && monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 4) {
					cursmx = mx + 2; /// BUGFIX: 'mx + 2' (fixed)
					cursmy = my + 1; /// BUGFIX: 'my + 1' (fixed)
					pcursmonst = mi;
				}
			}
			if (flipflag && mx + 1 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 1][my + 2] != 0 && dFlags[mx + 1][my + 2] & BFLAG_LIT) {
				mi = dMonster[mx + 1][my + 2] > 0 ? dMonster[mx + 1][my + 2] - 1 : -(dMonster[mx + 1][my + 2] + 1);
				if (mi == pcurstemp && monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 4) {
					cursmx = mx + 1;
					cursmy = my + 2;
					pcursmonst = mi;
				}
			}
			if (mx + 2 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 2][my + 2] != 0 && dFlags[mx + 2][my + 2] & BFLAG_LIT) {
				mi = dMonster[mx + 2][my + 2] > 0 ? dMonster[mx + 2][my + 2] - 1 : -(dMonster[mx + 2][my + 2] + 1);
				if (mi == pcurstemp && monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 4) {
					cursmx = mx + 2;
					cursmy = my + 2;
					pcursmonst = mi;
				}
			}
			if (mx + 1 < MAXDUNX && !flipflag && dMonster[mx + 1][my] != 0 && dFlags[mx + 1][my] & BFLAG_LIT) {
				mi = dMonster[mx + 1][my] > 0 ? dMonster[mx + 1][my] - 1 : -(dMonster[mx + 1][my] + 1);
				if (mi == pcurstemp && monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 2) {
					cursmx = mx + 1;
					cursmy = my;
					pcursmonst = mi;
				}
			}
			if (my + 1 < MAXDUNY && flipflag && dMonster[mx][my + 1] != 0 && dFlags[mx][my + 1] & BFLAG_LIT) {
				mi = dMonster[mx][my + 1] > 0 ? dMonster[mx][my + 1] - 1 : -(dMonster[mx][my + 1] + 1);
				if (mi == pcurstemp && monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 2) {
					cursmx = mx;
					cursmy = my + 1;
					pcursmonst = mi;
				}
			}
			if (dMonster[mx][my] != 0 && dFlags[mx][my] & BFLAG_LIT) {
				mi = dMonster[mx][my] > 0 ? dMonster[mx][my] - 1 : -(dMonster[mx][my] + 1);
				if (mi == pcurstemp && monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 1) {
					cursmx = mx;
					cursmy = my;
					pcursmonst = mi;
				}
			}
			if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 1][my + 1] != 0 && dFlags[mx + 1][my + 1] & BFLAG_LIT) {
				mi = dMonster[mx + 1][my + 1] > 0 ? dMonster[mx + 1][my + 1] - 1 : -(dMonster[mx + 1][my + 1] + 1);
				if (mi == pcurstemp && monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 2) {
					cursmx = mx + 1;
					cursmy = my + 1;
					pcursmonst = mi;
				}
			}
			if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_HIDDEN) {
				pcursmonst = -1;
				cursmx = mx;
				cursmy = my;
			}
			if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_GOLEM) {
				pcursmonst = -1;
			}
			if (pcursmonst != -1) {
				return;
			}
		}
		if (!flipflag && mx + 2 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 2][my + 1] != 0 && dFlags[mx + 2][my + 1] & BFLAG_LIT) {
			mi = dMonster[mx + 2][my + 1] > 0 ? dMonster[mx + 2][my + 1] - 1 : -(dMonster[mx + 2][my + 1] + 1);
			if (monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 4) {
				cursmx = mx + 2;
				cursmy = my + 1;
				pcursmonst = mi;
			}
		}
		if (flipflag && mx + 1 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 1][my + 2] != 0 && dFlags[mx + 1][my + 2] & BFLAG_LIT) {
			mi = dMonster[mx + 1][my + 2] > 0 ? dMonster[mx + 1][my + 2] - 1 : -(dMonster[mx + 1][my + 2] + 1);
			if (monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 4) {
				cursmx = mx + 1;
				cursmy = my + 2;
				pcursmonst = mi;
			}
		}
		if (mx + 2 < MAXDUNX && my + 2 < MAXDUNY && dMonster[mx + 2][my + 2] != 0 && dFlags[mx + 2][my + 2] & BFLAG_LIT) {
			mi = dMonster[mx + 2][my + 2] > 0 ? dMonster[mx + 2][my + 2] - 1 : -(dMonster[mx + 2][my + 2] + 1);
			if (monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 4) {
				cursmx = mx + 2;
				cursmy = my + 2;
				pcursmonst = mi;
			}
		}
		if (!flipflag && mx + 1 < MAXDUNX && dMonster[mx + 1][my] != 0 && dFlags[mx + 1][my] & BFLAG_LIT) {
			mi = dMonster[mx + 1][my] > 0 ? dMonster[mx + 1][my] - 1 : -(dMonster[mx + 1][my] + 1);
			if (monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 2) {
				cursmx = mx + 1;
				cursmy = my;
				pcursmonst = mi;
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dMonster[mx][my + 1] != 0 && dFlags[mx][my + 1] & BFLAG_LIT) {
			mi = dMonster[mx][my + 1] > 0 ? dMonster[mx][my + 1] - 1 : -(dMonster[mx][my + 1] + 1);
			if (monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 2) {
				cursmx = mx;
				cursmy = my + 1;
				pcursmonst = mi;
			}
		}
		if (dMonster[mx][my] != 0 && dFlags[mx][my] & BFLAG_LIT) {
			mi = dMonster[mx][my] > 0 ? dMonster[mx][my] - 1 : -(dMonster[mx][my] + 1);
			if (monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 1) {
				cursmx = mx;
				cursmy = my;
				pcursmonst = mi;
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 1][my + 1] != 0 && dFlags[mx + 1][my + 1] & BFLAG_LIT) {
			mi = dMonster[mx + 1][my + 1] > 0 ? dMonster[mx + 1][my + 1] - 1 : -(dMonster[mx + 1][my + 1] + 1);
			if (monster[mi]._mhitpoints >> 6 > 0 && monster[mi].MData->mSelFlag & 2) {
				cursmx = mx + 1;
				cursmy = my + 1;
				pcursmonst = mi;
			}
		}
		if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_HIDDEN) {
			pcursmonst = -1;
			cursmx = mx;
			cursmy = my;
		}
		if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_GOLEM) {
			pcursmonst = -1;
		}
	} else {
		if (!flipflag && mx + 1 < MAXDUNX && dMonster[mx + 1][my] > 0) {
			pcursmonst = dMonster[mx + 1][my] - 1;
			cursmx = mx + 1;
			cursmy = my;
		}
		if (flipflag && my + 1 < MAXDUNY && dMonster[mx][my + 1] > 0) {
			pcursmonst = dMonster[mx][my + 1] - 1;
			cursmx = mx;
			cursmy = my + 1;
		}
		if (dMonster[mx][my] > 0) {
			pcursmonst = dMonster[mx][my] - 1;
			cursmx = mx;
			cursmy = my;
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dMonster[mx + 1][my + 1] > 0) {
			pcursmonst = dMonster[mx + 1][my + 1] - 1;
			cursmx = mx + 1;
			cursmy = my + 1;
		}
		if (pcursmonst != -1 && !towner[pcursmonst]._tSelFlag) {
			pcursmonst = -1;
		}
	}

	if (pcursmonst == -1) {
		if (!flipflag && mx + 1 < MAXDUNX && dPlayer[mx + 1][my] != 0) {
			bv = dPlayer[mx + 1][my] > 0 ? dPlayer[mx + 1][my] - 1 : -(dPlayer[mx + 1][my] + 1);
			if (bv != myplr && plr[bv]._pHitPoints != 0) {
				cursmx = mx + 1;
				cursmy = my;
				pcursplr = bv;
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dPlayer[mx][my + 1] != 0) {
			bv = dPlayer[mx][my + 1] > 0 ? dPlayer[mx][my + 1] - 1 : -(dPlayer[mx][my + 1] + 1);
			if (bv != myplr && plr[bv]._pHitPoints != 0) {
				cursmx = mx;
				cursmy = my + 1;
				pcursplr = bv;
			}
		}
		if (dPlayer[mx][my] != 0) {
			bv = dPlayer[mx][my] > 0 ? dPlayer[mx][my] - 1 : -(dPlayer[mx][my] + 1);
			if (bv != myplr) {
				cursmx = mx;
				cursmy = my;
				pcursplr = bv;
			}
		}
		if (dFlags[mx][my] & BFLAG_DEAD_PLAYER) {
			for (i = 0; i < MAX_PLRS; i++) {
				if (plr[i]._px == mx && plr[i]._py == my && i != myplr) {
					cursmx = mx;
					cursmy = my;
					pcursplr = i;
				}
			}
		}
		if (pcurs == CURSOR_RESURRECT) {
			for (xx = -1; xx < 2; xx++) {
				for (yy = -1; yy < 2; yy++) {
					if (mx + xx < MAXDUNX && my + yy < MAXDUNY && dFlags[mx + xx][my + yy] & BFLAG_DEAD_PLAYER) {
						for (i = 0; i < MAX_PLRS; i++) {
							if (plr[i]._px == mx + xx && plr[i]._py == my + yy && i != myplr) {
								cursmx = mx + xx;
								cursmy = my + yy;
								pcursplr = i;
							}
						}
					}
				}
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dPlayer[mx + 1][my + 1] != 0) {
			bv = dPlayer[mx + 1][my + 1] > 0 ? dPlayer[mx + 1][my + 1] - 1 : -(dPlayer[mx + 1][my + 1] + 1);
			if (bv != myplr && plr[bv]._pHitPoints != 0) {
				cursmx = mx + 1;
				cursmy = my + 1;
				pcursplr = bv;
			}
		}
	}
	if (pcursmonst == -1 && pcursplr == -1) {
		if (!flipflag && mx + 1 < MAXDUNX && dObject[mx + 1][my] != 0) {
			bv = dObject[mx + 1][my] > 0 ? dObject[mx + 1][my] - 1 : -(dObject[mx + 1][my] + 1);
			if (object[bv]._oSelFlag >= 2) {
				cursmx = mx + 1;
				cursmy = my;
				pcursobj = bv;
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dObject[mx][my + 1] != 0) {
			bv = dObject[mx][my + 1] > 0 ? dObject[mx][my + 1] - 1 : -(dObject[mx][my + 1] + 1);
			if (object[bv]._oSelFlag >= 2) {
				cursmx = mx;
				cursmy = my + 1;
				pcursobj = bv;
			}
		}
		if (dObject[mx][my] != 0) {
			bv = dObject[mx][my] > 0 ? dObject[mx][my] - 1 : -(dObject[mx][my] + 1);
			if (object[bv]._oSelFlag == 1 || object[bv]._oSelFlag == 3) {
				cursmx = mx;
				cursmy = my;
				pcursobj = bv;
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dObject[mx + 1][my + 1] != 0) {
			bv = dObject[mx + 1][my + 1] > 0 ? dObject[mx + 1][my + 1] - 1 : -(dObject[mx + 1][my + 1] + 1);
			if (object[bv]._oSelFlag >= 2) {
				cursmx = mx + 1;
				cursmy = my + 1;
				pcursobj = bv;
			}
		}
	}
	if (pcursplr == -1 && pcursobj == -1 && pcursmonst == -1) {
		if (!flipflag && mx + 1 < MAXDUNX && dItem[mx + 1][my] > 0) {
			bv = dItem[mx + 1][my] - 1;
			if (item[bv]._iSelFlag >= 2) {
				cursmx = mx + 1;
				cursmy = my;
				pcursitem = bv;
			}
		}
		if (flipflag && my + 1 < MAXDUNY && dItem[mx][my + 1] > 0) {
			bv = dItem[mx][my + 1] - 1;
			if (item[bv]._iSelFlag >= 2) {
				cursmx = mx;
				cursmy = my + 1;
				pcursitem = bv;
			}
		}
		if (dItem[mx][my] > 0) {
			bv = dItem[mx][my] - 1;
			if (item[bv]._iSelFlag == 1 || item[bv]._iSelFlag == 3) {
				cursmx = mx;
				cursmy = my;
				pcursitem = bv;
			}
		}
		if (mx + 1 < MAXDUNX && my + 1 < MAXDUNY && dItem[mx + 1][my + 1] > 0) {
			bv = dItem[mx + 1][my + 1] - 1;
			if (item[bv]._iSelFlag >= 2) {
				cursmx = mx + 1;
				cursmy = my + 1;
				pcursitem = bv;
			}
		}
		if (pcursitem == -1) {
			cursmx = mx;
			cursmy = my;
			CheckTrigForce();
			CheckTown();
			CheckRportal();
		}
	}

	if (pcurs == CURSOR_IDENTIFY) {
		pcursobj = -1;
		pcursmonst = -1;
		pcursitem = -1;
		cursmx = mx;
		cursmy = my;
	}
	if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_GOLEM) {
		pcursmonst = -1;
	}
}

DEVILUTION_END_NAMESPACE
