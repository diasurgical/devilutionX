#include "stash.h"
#include "all.h"

namespace dvl {

char pcursstashitem = -1;
bool stash = false;
const InvXY stashGridStart = { 17, 76 }, stashGridOffsets = { INV_SLOT_SIZE_PX + 1, INV_SLOT_SIZE_PX + 1 };
const int stashRows = 10, stashCols = 10;
const int NUM_STASH_GRID_ELEM = stashRows * stashCols;

ItemStruct StashList[NUM_STASH_GRID_ELEM];
int _pNumStash;
char StashGrid[NUM_STASH_GRID_ELEM];

void InitStash()
{
	stash = false;
	if (!LoadStash()) {
		memset(StashGrid, 0, sizeof(StashGrid));
		for (int i = 0; i < NUM_STASH_GRID_ELEM; i++) {
			StashList[i]._itype = ITYPE_NONE;
		}
		_pNumStash = 0;
	}
}

InvXY getStashGridXY(int i)
{
	return {
		stashGridStart.X + (i % stashCols) * stashGridOffsets.X, stashGridStart.Y + (i / stashRows) * stashGridOffsets.Y
	};
}

std::string GetStashPath()
{
	return GetPrefPath() + "stash.lol";
}

int GetStashSlotFromMouse(int mx, int my)
{
	for (int r = 0; (DWORD)r < NUM_STASH_GRID_ELEM; r++) {
		// check which inventory rectangle the mouse is in, if any
		InvXY invrect = getStashGridXY(r);
		if (mx >= invrect.X && mx < invrect.X + (INV_SLOT_SIZE_PX + 1) && my >= invrect.Y - (INV_SLOT_SIZE_PX + 1) && my < invrect.Y) {
			return r;
		}
	}
	return -1;
}

char CheckStashHLight()
{
	if (!stash)
		return -1;
	int r, ii, nGold;
	ItemStruct *pi;
	char rv;

	r = GetStashSlotFromMouse(MouseX, MouseY);

	if (r == -1)
		return -1;

	rv = -1;
	infoclr = COL_WHITE;
	pi = NULL;
	ClearPanel();
	r = abs(StashGrid[r]);
	if (r == 0)
		return -1;
	ii = r - 1;
	rv = ii;
	pi = &StashList[ii];

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

bool SaveStash()
{
	FILE *pFile = fopen(GetStashPath().c_str(), "wb");

	if (pFile == NULL)
		return false;

	BYTE *tbuffold = tbuff;
	DWORD dwLen = (sizeof(ItemStruct) * NUM_STASH_GRID_ELEM) + sizeof(char) * NUM_STASH_GRID_ELEM + sizeof(int);
	BYTE *tmp = (BYTE *) malloc(dwLen);
	memset(tmp, 0, dwLen);
	tbuff = tmp;
	CopyInt(&_pNumStash, tbuff);
	SaveItems(StashList, NUM_STASH_GRID_ELEM);
	CopyBytes(StashGrid, NUM_STASH_GRID_ELEM, tbuff);
	fwrite(tmp, sizeof(char), dwLen, pFile);
	fclose(pFile);
	free(tmp);
	tbuff = tbuffold;
	return true;
}

bool LoadStash()
{
	FILE *pFile;
	unsigned long lSize;
	size_t result;

	pFile = fopen(GetStashPath().c_str(), "rb");
	if (pFile == NULL)
		return false;

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	BYTE *tbuffold = tbuff;
	BYTE *tmp = (BYTE *)malloc(lSize);
	if (tmp == NULL) {
		fclose(pFile);
		return false;
	}
	memset(tmp, 0, lSize);

	// copy the file into the buffer:
	result = fread(tmp, 1, lSize, pFile);
	if (result != lSize) {
		free(tmp);
		fclose(pFile);
		return false;
	}
	/* the whole file is now loaded in the memory buffer. */
	// terminate
	fclose(pFile);

	tbuff = tmp;
	CopyInt(tbuff, &_pNumStash);
	LoadItems(NUM_STASH_GRID_ELEM, StashList);
	CopyBytes(tbuff, NUM_STASH_GRID_ELEM, StashGrid);
	free(tmp);
	tbuff = tbuffold;
	return true;
}

void CheckStashPaste(int pnum, int mx, int my)
{
	int r, sx, sy;
	int i, j, xx, yy, ii;
	BOOL done;
	int il, cn, it, iv, ig, gt;

	mx += INV_SLOT_SIZE_PX / 2;
	my += INV_SLOT_SIZE_PX * 3 / 2;

	SetICursor(plr[pnum].HoldItem._iCurs + CURSOR_FIRSTITEM);
	i = mx + (icursW >> 1);
	j = my + (icursH >> 1);
	sx = icursW28;
	sy = icursH28;
	r = GetStashSlotFromMouse(mx, my);
	if (r == -1)
		return;

	done = TRUE;
	it = 0;
	ii = r;
	if (plr[pnum].HoldItem._itype == ITYPE_GOLD) {
		yy = 10 * (ii / 10);
		xx = ii % 10;
		if (StashGrid[xx + yy] != 0) {
			iv = StashGrid[xx + yy];
			if (iv > 0) {
				if (StashList[iv - 1]._itype != ITYPE_GOLD) {
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
			if (yy >= NUM_STASH_GRID_ELEM)
				done = FALSE;
			xx = (ii % 10) - ((sx - 1) >> 1);
			if (xx < 0)
				xx = 0;
			for (i = 0; i < sx && done; i++) {
				if (xx >= 10) {
					done = FALSE;
				} else {
					if (StashGrid[xx + yy] != 0) {
						iv = StashGrid[xx + yy];
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

	if (!done)
		return;

	if (pnum == myplr)
		PlaySFX(ItemInvSnds[ItemCAnimTbl[plr[pnum].HoldItem._iCurs]]);

	cn = CURSOR_HAND;
	if (plr[pnum].HoldItem._itype == ITYPE_GOLD && it == 0) {
		ii = r;
		yy = 10 * (ii / 10);
		xx = ii % 10;
		if (StashGrid[yy + xx] > 0) {
			il = StashGrid[yy + xx];
			il--;
			gt = StashList[il]._ivalue;
			ig = plr[pnum].HoldItem._ivalue + gt;
			if (ig <= GOLD_MAX_LIMIT) {
				StashList[il]._ivalue = ig;
				plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
				SetPlrHandGoldCurs(&StashList[il]);
			} else {
				ig = GOLD_MAX_LIMIT - gt;
				plr[pnum]._pGold += ig;
				plr[pnum].HoldItem._ivalue -= ig;
				StashList[il]._ivalue = GOLD_MAX_LIMIT;
				StashList[il]._iCurs = ICURS_GOLD_LARGE;
				// BUGFIX: incorrect values here are leftover from beta (fixed)
				cn = GetGoldCursor(plr[pnum].HoldItem._ivalue);
				cn += CURSOR_FIRSTITEM;
			}
		} else {
			il = _pNumStash;
			StashList[il] = plr[pnum].HoldItem;
			_pNumStash++;
			StashGrid[yy + xx] = _pNumStash;
			plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
			SetPlrHandGoldCurs(&StashList[il]);
		}
	} else {
		if (it == 0) {
			StashList[_pNumStash] = plr[pnum].HoldItem;
			_pNumStash++;
			it = _pNumStash;
		} else {
			il = it - 1;
			if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
				plr[pnum]._pGold += plr[pnum].HoldItem._ivalue;
			cn = SwapItem(&StashList[il], &plr[pnum].HoldItem);
			if (plr[pnum].HoldItem._itype == ITYPE_GOLD)
				plr[pnum]._pGold = CalculateGold(pnum);
			for (i = 0; i < NUM_STASH_GRID_ELEM; i++) {
				if (StashGrid[i] == it)
					StashGrid[i] = 0;
				if (StashGrid[i] == -it)
					StashGrid[i] = 0;
			}
		}
		ii = r;
		yy = 10 * (ii / 10 - ((sy - 1) >> 1));
		if (yy < 0)
			yy = 0;
		for (j = 0; j < sy; j++) {
			xx = (ii % 10 - ((sx - 1) >> 1));
			if (xx < 0)
				xx = 0;
			for (i = 0; i < sx; i++) {
				if (i != 0 || j != sy - 1)
					StashGrid[xx + yy] = -it;
				else
					StashGrid[xx + yy] = it;
				xx++;
			}
			yy += 10;
		}
	}

	CalcPlrInv(pnum, TRUE);
	if (pnum == myplr) {
		if (cn == CURSOR_HAND)
			SetCursorPos(MouseX + (cursW >> 1), MouseY + (cursH >> 1));
		SetCursor_(cn);
	}
	SaveStash();
}

void CheckStashCut(int pnum, int mx, int my)
{
	char ii;
	int r, iv, i, j, ig;

	if (plr[pnum]._pmode > PM_WALK3) {
		return;
	}

	if (dropGoldFlag) {
		dropGoldFlag = FALSE;
		dropGoldValue = 0;
	}

	r = GetStashSlotFromMouse(mx, my);
	if (r == -1)
		return;

	plr[pnum].HoldItem._itype = ITYPE_NONE;
	ig = r;
	ii = StashGrid[ig];
	if (ii != 0) {
		iv = ii;
		if (ii <= 0) {
			iv = -ii;
		}

		for (i = 0; i < NUM_STASH_GRID_ELEM; i++) {
			if (StashGrid[i] == iv || StashGrid[i] == -iv) {
				StashGrid[i] = 0;
			}
		}

		iv--;

		plr[pnum].HoldItem = StashList[iv];
		_pNumStash--;

		if (_pNumStash > 0 && _pNumStash != iv) {
			StashList[iv] = StashList[_pNumStash];

			for (j = 0; j < NUM_STASH_GRID_ELEM; j++) {
				if (StashGrid[j] == _pNumStash + 1) {
					StashGrid[j] = iv + 1;
				}
				if (StashGrid[j] == -(_pNumStash + 1)) {
					StashGrid[j] = -iv - 1;
				}
			}
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
	SaveStash();
}

void CheckStash()
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		CheckStashPaste(myplr, MouseX, MouseY);
	} else {
		CheckStashCut(myplr, MouseX, MouseY);
	}
}

void DrawStash()
{
	Uint8 *pStart, *pEnd;
	pStart = gpBufStart;
	pEnd = gpBufEnd;
	gpBufStart = &gpBuffer[SCREENXY(0, 220)];
	CelDraw(BORDER_LEFT, 351 + SCREEN_Y, pInvCels, 1, SPANEL_WIDTH);
	int offsx = 44;
	gpBufStart = &gpBuffer[SCREENXY(0, 89 + offsx)];
	gpBufEnd = &gpBuffer[SCREENXY(0, 178 + offsx)];
	CelDraw(BORDER_LEFT, 220 + SCREEN_Y + offsx, pInvCels, 1, SPANEL_WIDTH);
	offsx = -42;
	gpBufStart = &gpBuffer[SCREENXY(0, 80 + offsx)];
	gpBufEnd = &gpBuffer[SCREENXY(0, 178 + offsx)];
	CelDraw(BORDER_LEFT, 220 + SCREEN_Y + offsx, pInvCels, 1, SPANEL_WIDTH);
	gpBufStart = &gpBuffer[SCREENXY(0, 0)];
	gpBufEnd = &gpBuffer[SCREENXY(0, 40)];
	CelDraw(BORDER_LEFT, SCREEN_Y + 351, pInvCels, 1, SPANEL_WIDTH);
	gpBufStart = pStart;
	gpBufEnd = pEnd;
	for (int i = 0; i < NUM_STASH_GRID_ELEM; i++) {
		if (StashGrid[i] != 0) {
			InvXY xy = getStashGridXY(i);
			InvDrawSlotBack(xy.X + BORDER_LEFT, xy.Y + SCREEN_Y - 1, INV_SLOT_SIZE_PX, INV_SLOT_SIZE_PX);
		}
	}

	for (int j = 0; j < NUM_STASH_GRID_ELEM; j++) {
		if (StashGrid[j] > 0) {
			int ii = StashGrid[j] - 1;
			InvXY xy = getStashGridXY(j);
			int frame = StashList[ii]._iCurs + CURSOR_FIRSTITEM;
			int frame_width = InvItemWidth[frame];
			BYTE *icons = (frame <= 179 ? pCursCels : pCursCels2);
			frame = (frame <= 179 ? frame : frame - 179);
			int r = GetStashSlotFromMouse(MouseX, MouseY);
			if (pcurs < CURSOR_FIRSTITEM && r> 0 && (StashGrid[r] == StashGrid[j] || StashGrid[r] == -StashGrid[j])) {
				BYTE color = ICOL_WHITE;
				if (StashList[ii]._iMagical != ITEM_QUALITY_NORMAL)
					color = ICOL_BLUE;
				if (!StashList[ii]._iStatFlag)
					color = ICOL_RED;
				CelBlitOutline(color, xy.X + BORDER_LEFT, xy.Y + SCREEN_Y - 1, icons, frame, frame_width);
			}
			if (StashList[ii]._iStatFlag)
				CelClippedDraw(xy.X + BORDER_LEFT, xy.Y + SCREEN_Y - 1, icons, frame, frame_width);
			else
				CelDrawLightRed(xy.X + BORDER_LEFT, xy.Y + SCREEN_Y - 1, icons, frame, frame_width, 1);
		}
	}
}

} // namespace dvl
