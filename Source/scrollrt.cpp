#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

int light_table_index;
DWORD sgdwCursWdtOld;
DWORD sgdwCursX;
DWORD sgdwCursY;
BYTE *gpBufStart;
BYTE *gpBufEnd;
DWORD sgdwCursHgt;
DWORD level_cel_block;
DWORD sgdwCursXOld;
DWORD sgdwCursYOld;
char arch_draw_type;
int cel_transparency_active;
int level_piece_id;
DWORD sgdwCursWdt;
void (*DrawPlrProc)(int, int, int, int, int, BYTE *, int, int, int, int);
BYTE sgSaveBack[8192];
DWORD sgdwCursHgtOld;

/* data */

/* used in 1.00 debug */
char *szMonModeAssert[18] = {
	"standing",
	"walking (1)",
	"walking (2)",
	"walking (3)",
	"attacking",
	"getting hit",
	"dying",
	"attacking (special)",
	"fading in",
	"fading out",
	"attacking (ranged)",
	"standing (special)",
	"attacking (special ranged)",
	"delaying",
	"charging",
	"stoned",
	"healing",
	"talking"
};

char *szPlrModeAssert[12] = {
	"standing",
	"walking (1)",
	"walking (2)",
	"walking (3)",
	"attacking (melee)",
	"attacking (ranged)",
	"blocking",
	"getting hit",
	"dying",
	"casting a spell",
	"changing levels",
	"quitting"
};

void ClearCursor() // CODE_FIX: this was supposed to be in cursor.cpp
{
	sgdwCursWdt = 0;
	sgdwCursWdtOld = 0;
}

/**
 * @brief Remove the cursor from the backbuffer
 */
static void scrollrt_draw_cursor_back_buffer()
{
	int i;
	BYTE *src, *dst;

	if (sgdwCursWdt == 0) {
		return;
	}

	assert(gpBuffer);
	src = sgSaveBack;
	dst = &gpBuffer[SCREENXY(sgdwCursX, sgdwCursY)];

	for (i = sgdwCursHgt; i != 0; i--, src += sgdwCursWdt, dst += BUFFER_WIDTH) {
		memcpy(dst, src, sgdwCursWdt);
	}

	sgdwCursXOld = sgdwCursX;
	sgdwCursYOld = sgdwCursY;
	sgdwCursWdtOld = sgdwCursWdt;
	sgdwCursHgtOld = sgdwCursHgt;
	sgdwCursWdt = 0;
}

/**
 * @brief Draw the cursor on the backbuffer
 */
static void scrollrt_draw_cursor_item()
{
	int i, mx, my, col;
	BYTE *src, *dst;

	assert(! sgdwCursWdt);

	if (pcurs <= 0 || cursW == 0 || cursH == 0) {
		return;
	}

	if (sgbControllerActive && pcurs != CURSOR_TELEPORT && !invflag && (!chrflag || plr[myplr]._pStatPts <= 0)) {
		return;
	}

	mx = MouseX - 1;
	if (mx < 0) {
		mx = 0;
	} else if (mx > SCREEN_WIDTH - 1) {
		return;
	}
	my = MouseY - 1;
	if (my < 0) {
		my = 0;
	} else if (my > SCREEN_HEIGHT - 1) {
		return;
	}

	sgdwCursX = mx;
	sgdwCursWdt = sgdwCursX + cursW + 1;
	if (sgdwCursWdt > SCREEN_WIDTH - 1) {
		sgdwCursWdt = SCREEN_WIDTH - 1;
	}
	sgdwCursX &= ~3;
	sgdwCursWdt |= 3;
	sgdwCursWdt -= sgdwCursX;
	sgdwCursWdt++;

	sgdwCursY = my;
	sgdwCursHgt = sgdwCursY + cursH + 1;
	if (sgdwCursHgt > SCREEN_HEIGHT - 1) {
		sgdwCursHgt = SCREEN_HEIGHT - 1;
	}
	sgdwCursHgt -= sgdwCursY;
	sgdwCursHgt++;

	assert(sgdwCursWdt * sgdwCursHgt < sizeof sgSaveBack);
	assert(gpBuffer);
	dst = sgSaveBack;
	src = &gpBuffer[SCREENXY(sgdwCursX, sgdwCursY)];

	for (i = sgdwCursHgt; i != 0; i--, dst += sgdwCursWdt, src += BUFFER_WIDTH) {
		memcpy(dst, src, sgdwCursWdt);
	}

	mx++;
	my++;
	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (SCREEN_HEIGHT + SCREEN_Y) - cursW - 2];

	if (pcurs >= CURSOR_FIRSTITEM) {
		col = PAL16_YELLOW + 5;
		if (plr[myplr].HoldItem._iMagical != 0) {
			col = PAL16_BLUE + 5;
		}
		if (!plr[myplr].HoldItem._iStatFlag) {
			col = PAL16_RED + 5;
		}
		CelBlitOutline(col, mx + SCREEN_X, my + cursH + SCREEN_Y - 1, pCursCels, pcurs, cursW);
		if (col != PAL16_RED + 5) {
			CelClippedDrawSafe(mx + SCREEN_X, my + cursH + SCREEN_Y - 1, pCursCels, pcurs, cursW);
		} else {
			CelDrawLightRedSafe(mx + SCREEN_X, my + cursH + SCREEN_Y - 1, pCursCels, pcurs, cursW, 1);
		}
	} else {
		CelClippedDrawSafe(mx + SCREEN_X, my + cursH + SCREEN_Y - 1, pCursCels, pcurs, cursW);
	}
}

void DrawMissilePrivate(MissileStruct *m, int sx, int sy, BOOL pre)
{
	int mx, my, nCel, frames;
	BYTE *pCelBuff;

	if (m->_miPreFlag != pre || !m->_miDrawFlag)
		return;

	pCelBuff = m->_miAnimData;
	if (!pCelBuff) {
		// app_fatal("Draw Missile 2 type %d: NULL Cel Buffer", m->_mitype);
		return;
	}
	nCel = m->_miAnimFrame;
	frames = SDL_SwapLE32(*(DWORD *)pCelBuff);
	if (nCel < 1 || frames > 50 || nCel > frames) {
		// app_fatal("Draw Missile 2: frame %d of %d, missile type==%d", nCel, frames, m->_mitype);
		return;
	}
	mx = sx + m->_mixoff - m->_miAnimWidth2;
	my = sy + m->_miyoff;
	if (m->_miUniqTrans)
		Cl2DrawLightTbl(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, m->_miUniqTrans + 3);
	else if (m->_miLightFlag)
		Cl2DrawLight(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth);
	else
		Cl2Draw(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth);
}

/**
 * @brief Render a missile sprite
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param sx Backbuffer coordinate
 * @param sy Backbuffer coordinate
 * @param pre Is the sprite in the background
 */
void DrawMissile(int x, int y, int sx, int sy, BOOL pre)
{
	int i;
	MissileStruct *m;

	if (!(dFlags[x][y] & BFLAG_MISSILE))
		return;

	if (dMissile[x][y] != -1) {
		m = &missile[dMissile[x][y] - 1];
		DrawMissilePrivate(m, sx, sy, pre);
		return;
	}

	for (i = 0; i < nummissiles; i++) {
		assert(missileactive[i] < MAXMISSILES);
		m = &missile[missileactive[i]];
		if (m->_mix != x || m->_miy != y)
			continue;
		DrawMissilePrivate(m, sx, sy, pre);
	}
}

/**
 * @brief Render a monster sprite
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param mx Backbuffer coordinate
 * @param my Backbuffer coordinate
 */
static void DrawMonster(int x, int y, int mx, int my, int m)
{
	int nCel, frames;
	char trans;
	BYTE *pCelBuff;

	if ((DWORD)m >= MAXMONSTERS) {
		// app_fatal("Draw Monster: tried to draw illegal monster %d", m);
		return;
	}

	pCelBuff = monster[m]._mAnimData;
	if (!pCelBuff) {
		// app_fatal("Draw Monster \"%s\": NULL Cel Buffer", monster[m].mName);
		return;
	}

	nCel = monster[m]._mAnimFrame;
	frames = SDL_SwapLE32(*(DWORD *)pCelBuff);
	if (nCel < 1 || frames > 50 || nCel > frames) {
		/*
		const char *szMode = "unknown action";
		if(monster[m]._mmode <= 17)
			szMode = szMonModeAssert[monster[m]._mmode];
		app_fatal(
			"Draw Monster \"%s\" %s: facing %d, frame %d of %d",
			monster[m].mName,
			szMode,
			monster[m]._mdir,
			nCel,
			frames);
		*/
		return;
	}

	if (!(dFlags[x][y] & BFLAG_LIT)) {
		Cl2DrawLightTbl(mx, my, monster[m]._mAnimData, monster[m]._mAnimFrame, monster[m].MType->width, 1);
	} else {
		trans = 0;
		if (monster[m]._uniqtype)
			trans = monster[m]._uniqtrans + 4;
		if (monster[m]._mmode == MM_STONE)
			trans = 2;
		if (plr[myplr]._pInfraFlag && light_table_index > 8)
			trans = 1;
		if (trans)
			Cl2DrawLightTbl(mx, my, monster[m]._mAnimData, monster[m]._mAnimFrame, monster[m].MType->width, trans);
		else
			Cl2DrawLight(mx, my, monster[m]._mAnimData, monster[m]._mAnimFrame, monster[m].MType->width);
	}
}

/**
 * @brief Render a monster sprite
 * @param pnum Player id
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param px Backbuffer coordinate
 * @param py Backbuffer coordinate
 * @param pCelBuff sprite buffer
 * @param nCel frame
 * @param nWidth width
 */
static void DrawPlayer(int pnum, int x, int y, int px, int py, BYTE *pCelBuff, int nCel, int nWidth)
{
	int l, frames;

	if (dFlags[x][y] & BFLAG_LIT || plr[myplr]._pInfraFlag || !setlevel && !currlevel) {
		if (!pCelBuff) {
			// app_fatal("Drawing player %d \"%s\": NULL Cel Buffer", pnum, plr[pnum]._pName);
			return;
		}
		frames = SDL_SwapLE32(*(DWORD *)pCelBuff);
		if (nCel < 1 || frames > 50 || nCel > frames) {
			/*
			const char *szMode = "unknown action";
			if(plr[pnum]._pmode <= PM_QUIT)
				szMode = szPlrModeAssert[plr[pnum]._pmode];
			app_fatal(
				"Drawing player %d \"%s\" %s: facing %d, frame %d of %d",
				pnum,
				plr[pnum]._pName,
				szMode,
				plr[pnum]._pdir,
				nCel,
				frames);
			*/
			return;
		}
		if (pnum == pcursplr)
			Cl2DrawOutline(165, px, py, pCelBuff, nCel, nWidth);
		if (pnum == myplr) {
			Cl2Draw(px, py, pCelBuff, nCel, nWidth);
			if (plr[pnum].pManaShield)
				Cl2Draw(
				    px + plr[pnum]._pAnimWidth2 - misfiledata[MFILE_MANASHLD].mAnimWidth2[0],
				    py,
				    misfiledata[MFILE_MANASHLD].mAnimData[0],
				    1,
				    misfiledata[MFILE_MANASHLD].mAnimWidth[0]);
		} else if (!(dFlags[x][y] & BFLAG_LIT) || plr[myplr]._pInfraFlag && light_table_index > 8) {
			Cl2DrawLightTbl(px, py, pCelBuff, nCel, nWidth, 1);
			if (plr[pnum].pManaShield)
				Cl2DrawLightTbl(
				    px + plr[pnum]._pAnimWidth2 - misfiledata[MFILE_MANASHLD].mAnimWidth2[0],
				    py,
				    misfiledata[MFILE_MANASHLD].mAnimData[0],
				    1,
				    misfiledata[MFILE_MANASHLD].mAnimWidth[0],
				    1);
		} else {
			l = light_table_index;
			if (light_table_index < 5)
				light_table_index = 0;
			else
				light_table_index -= 5;
			Cl2DrawLight(px, py, pCelBuff, nCel, nWidth);
			if (plr[pnum].pManaShield)
				Cl2DrawLight(
				    px + plr[pnum]._pAnimWidth2 - misfiledata[MFILE_MANASHLD].mAnimWidth2[0],
				    py,
				    misfiledata[MFILE_MANASHLD].mAnimData[0],
				    1,
				    misfiledata[MFILE_MANASHLD].mAnimWidth[0]);
			light_table_index = l;
		}
	}
}

/**
 * @brief Render a monster sprite
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param sx Backbuffer coordinate
 * @param sy Backbuffer coordinate
 */
void DrawDeadPlayer(int x, int y, int sx, int sy)
{
	int i, px, py, nCel, frames;
	PlayerStruct *p;
	BYTE *pCelBuff;

	dFlags[x][y] &= ~BFLAG_DEAD_PLAYER;

	for (i = 0; i < MAX_PLRS; i++) {
		p = &plr[i];
		if (p->plractive && !p->_pHitPoints && p->plrlevel == (BYTE)currlevel && p->WorldX == x && p->WorldY == y) {
			pCelBuff = p->_pAnimData;
			if (!pCelBuff) {
				// app_fatal("Drawing dead player %d \"%s\": NULL Cel Buffer", i, p->_pName);
				break;
			}
			nCel = p->_pAnimFrame;
			frames = SDL_SwapLE32(*(DWORD *)pCelBuff);
			if (nCel < 1 || frames > 50 || nCel > frames) {
				// app_fatal("Drawing dead player %d \"%s\": facing %d, frame %d of %d", i, p->_pName, p->_pdir, nCel, frame);
				break;
			}
			dFlags[x][y] |= BFLAG_DEAD_PLAYER;
			px = sx + p->_pxoff - p->_pAnimWidth2;
			py = sy + p->_pyoff;
			DrawPlayer(i, x, y, px, py, p->_pAnimData, p->_pAnimFrame, p->_pAnimWidth);
		}
	}
}

/**
 * @brief Render an object sprite
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param ox Backbuffer coordinate
 * @param oy Backbuffer coordinate
 * @param pre Is the sprite in the background
 */
static void DrawObject(int x, int y, int ox, int oy, BOOL pre)
{
	int sx, sy, xx, yy, nCel, frames;
	char bv;
	BYTE *pCelBuff;

	if (dObject[x][y] == 0 || light_table_index >= lightmax)
		return;

	if (dObject[x][y] > 0) {
		bv = dObject[x][y] - 1;
		if (object[bv]._oPreFlag != pre)
			return;
		sx = ox - object[bv]._oAnimWidth2;
		sy = oy;
	} else {
		bv = -(dObject[x][y] + 1);
		if (object[bv]._oPreFlag != pre)
			return;
		xx = object[bv]._ox - x;
		yy = object[bv]._oy - y;
		sx = (xx << 5) + ox - object[bv]._oAnimWidth2 - (yy << 5);
		sy = oy + (yy << 4) + (xx << 4);
	}

	assert((unsigned char)bv < MAXOBJECTS);

	pCelBuff = object[bv]._oAnimData;
	if (!pCelBuff) {
		// app_fatal("Draw Object type %d: NULL Cel Buffer", object[bv]._otype);
		return;
	}

	nCel = object[bv]._oAnimFrame;
	frames = SDL_SwapLE32(*(DWORD *)pCelBuff);
	if (nCel < 1 || frames > 50 || nCel > (int)frames) {
		// app_fatal("Draw Object: frame %d of %d, object type==%d", nCel, frames, object[bv]._otype);
		return;
	}

	if (bv == pcursobj)
		CelBlitOutline(194, sx, sy, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth);
	if (object[bv]._oLight) {
		CelClippedDrawLight(sx, sy, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth);
	} else {
		CelClippedDraw(sx, sy, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth);
	}
}

static void scrollrt_draw_dungeon(int sx, int sy, int dx, int dy, int eflag);

static void drawRow(int x, int y, int sx, int sy, int eflag)
{
	BYTE *dst;
	MICROS *pMap;

	level_piece_id = dPiece[x][y];
	light_table_index = dLight[x][y];

	dst = &gpBuffer[sx + sy * BUFFER_WIDTH];
	pMap = &dpiece_defs_map_2[x][y];
	cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
	for (int i = 0; i<MicroTileLen>> 1; i++) {
		arch_draw_type = i == 0 ? 1 : 0;
		level_cel_block = pMap->mt[2 * i];
		if (level_cel_block != 0) {
			drawUpperScreen(dst);
		}
		arch_draw_type = i == 0 ? 2 : 0;
		level_cel_block = pMap->mt[2 * i + 1];
		if (level_cel_block != 0) {
			drawUpperScreen(dst + 32);
		}
		dst -= BUFFER_WIDTH * 32;
	}

	scrollrt_draw_dungeon(x, y, sx, sy, eflag);
}

/**
 * This variant checks for of screen element on the lower screen
 * This function it self causes rendering issues since it will render on top of objects on the other side of walls
 * @brief Re render tile to workaround sorting issues with players walking east/west
 * @param y dPiece coordinate
 * @param x dPiece coordinate
 * @param sx Backbuffer coordinate
 * @param sy Backbuffer coordinate
 */
static void scrollrt_draw_e_flag(int x, int y, int sx, int sy)
{
	int lti_old, cta_old, lpi_old;

	lti_old = light_table_index;
	cta_old = cel_transparency_active;
	lpi_old = level_piece_id;

	drawRow(x, y, sx, sy, 0);

	light_table_index = lti_old;
	cel_transparency_active = cta_old;
	level_piece_id = lpi_old;
}

static void DrawItem(int x, int y, int sx, int sy, BOOL pre)
{
	char bItem = dItem[x][y];
	ItemStruct *pItem;

	if (bItem == 0)
		return;

	pItem = &item[bItem - 1];
	if (pItem->_iPostDraw == pre)
		return;

	assert((unsigned char)bItem <= MAXITEMS);
	int px = sx - pItem->_iAnimWidth2;
	if (bItem - 1 == pcursitem) {
		CelBlitOutline(181, px, sy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth);
	}
	CelClippedDrawLight(px, sy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth);
}

static void DrawMonsterHelper(int x, int y, int oy, int sx, int sy, int eflag)
{
	int mi, px, py;
	MonsterStruct *pMonster;

	if (!(dFlags[x][y] & BFLAG_LIT) && !plr[myplr]._pInfraFlag)
		return;

	mi = dMonster[x][y + oy];
	mi = mi > 0 ? mi - 1 : -(mi + 1);

	if (leveltype == DTYPE_TOWN) {
		px = sx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelBlitOutline(166, px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
		}
		assert(towner[mi]._tAnimData);
		CelClippedDraw(px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
		return;
	}

	if ((DWORD)mi >= MAXMONSTERS) {
		// app_fatal("Draw Monster: tried to draw illegal monster %d", mi);
	}

	pMonster = &monster[mi];
	if (pMonster->_mFlags & MFLAG_HIDDEN) {
		return;
	}

	if (pMonster->MType != NULL) {
		// app_fatal("Draw Monster \"%s\": uninitialized monster", pMonster->mName);
	}

	px = sx + pMonster->_mxoff - pMonster->MType->width2;
	py = sy + pMonster->_myoff;
	if (mi == pcursmonst) {
		Cl2DrawOutline(233, px, py, pMonster->_mAnimData, pMonster->_mAnimFrame, pMonster->MType->width);
	}
	DrawMonster(x, y, px, py, mi);
	if (eflag && !pMonster->_meflag) {
		scrollrt_draw_e_flag(x - 1, y + 1, sx - 64, sy);
	}
}

static void DrawPlayerHelper(int x, int y, int oy, int sx, int sy, int eflag)
{
	int p = dPlayer[x][y + oy];
	p = p > 0 ? p - 1 : -(p + 1);
	PlayerStruct *pPlayer = &plr[p];
	int px = sx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
	int py = sy + pPlayer->_pyoff;

	DrawPlayer(p, x, y + oy, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth);
	if (eflag && pPlayer->_peflag != 0) {
		if (pPlayer->_peflag == 2) {
			scrollrt_draw_e_flag(x - 2, y + 1, sx - 96, sy - 16);
		}
		scrollrt_draw_e_flag(x - 1, y + 1, sx - 64, sy);
	}
}

/**
 * @brief Render object sprites
 * @param sx dPiece coordinate
 * @param sy dPiece coordinate
 * @param dx Backbuffer coordinate
 * @param dy Backbuffer coordinate
 * @param eflag Should the sorting workaround be applied
 */
static void scrollrt_draw_dungeon(int sx, int sy, int dx, int dy, int eflag)
{
	int mi, px, py, nCel, nMon, negMon, frames;
	char bFlag, bDead, bObj, bItem, bPlr, bArch, bMap, negPlr, dd;
	DeadStruct *pDeadGuy;
	BYTE *pCelBuff;

	assert((DWORD)sx < MAXDUNX);
	assert((DWORD)sy < MAXDUNY);
	bFlag = dFlags[sx][sy];
	bDead = dDead[sx][sy];
	bArch = dArch[sx][sy];
	bMap = dTransVal[sx][sy];

	negMon = dMonster[sx][sy - 1];

	if (visiondebug && bFlag & BFLAG_LIT) {
		CelClippedDraw(dx, dy, pSquareCel, 1, 64);
	}

	if (MissilePreFlag) {
		DrawMissile(sx, sy, dx, dy, 1);
	}

	if (light_table_index < lightmax && bDead != 0) {
		pDeadGuy = &dead[(bDead & 0x1F) - 1];
		dd = (bDead >> 5) & 7;
		px = dx - pDeadGuy->_deadWidth2;
		pCelBuff = pDeadGuy->_deadData[dd];
		assert(pDeadGuy->_deadData[dd] != NULL);
		if (pCelBuff != NULL) {
			if (pDeadGuy->_deadtrans != 0) {
				Cl2DrawLightTbl(px, dy, pCelBuff, pDeadGuy->_deadFrame, pDeadGuy->_deadWidth, pDeadGuy->_deadtrans);
			} else {
				Cl2DrawLight(px, dy, pCelBuff, pDeadGuy->_deadFrame, pDeadGuy->_deadWidth);
			}
		}
	}
	DrawObject(sx, sy, dx, dy, 1);
	DrawItem(sx, sy, dx, dy, 1);
	if (bFlag & BFLAG_PLAYERLR) {
		assert((DWORD)(sy-1) < MAXDUNY);
		DrawPlayerHelper(sx, sy, -1, dx, dy, eflag);
	}
	if (bFlag & BFLAG_MONSTLR && negMon < 0) {
		DrawMonsterHelper(sx, sy, -1, dx, dy, eflag);
	}
	if (bFlag & BFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(sx, sy, dx, dy);
	}
	if (dPlayer[sx][sy] > 0) {
		DrawPlayerHelper(sx, sy, 0, dx, dy, eflag);
	}
	if (dMonster[sx][sy] > 0) {
		DrawMonsterHelper(sx, sy, 0, dx, dy, eflag);
	}
	DrawMissile(sx, sy, dx, dy, 0);
	DrawObject(sx, sy, dx, dy, 0);
	DrawItem(sx, sy, dx, dy, 0);

	if (bArch != 0) {
		cel_transparency_active = TransList[bMap];
		if (leveltype != DTYPE_TOWN) {
			CelClippedBlitLightTrans(&gpBuffer[dx + BUFFER_WIDTH * dy], pSpecialCels, bArch, 64);
		} else {
#if 0 // Special tree rendering, disabled in 1.09
			CelBlitFrame(&gpBuffer[dx + BUFFER_WIDTH * dy], pSpecialCels, bArch, 64);
#endif
		}
	}
}

/**
 * @brief Render a row of tile
 * @param x dPiece coordinate
 * @param y dPiece coordinate
 * @param sx Backbuffer coordinate
 * @param sy Backbuffer coordinate
 * @param chunks tile width of row
 * @param row current row being rendered
 */
static void scrollrt_draw(int x, int y, int sx, int sy, int chunks, int row)
{
	assert(gpBuffer);

	if (row & 1) {
		x -= 1;
		y += 1;
		sx -= 32;
		chunks += 1;
	}

	for (int j = 0; j < chunks; j++) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_piece_id = dPiece[x][y];
			if (level_piece_id != 0) {
				drawRow(x, y, sx, sy, 1);
			} else {
				world_draw_black_tile(sx, sy);
			}
		} else {
			world_draw_black_tile(sx, sy);
		}
		x++;
		y--;
		sx += 64;
	}
}

/**
 * @brief Configure render and process screen rows
 * @param x Center of view in dPiece coordinate
 * @param y Center of view in dPiece coordinate
 */
static void DrawGame(int x, int y)
{
	int i, sx, sy, chunks, blocks;
	int wdt, nSrcOff, nDstOff;

	sx = (SCREEN_WIDTH % 64) / 2;
	sy = (VIEWPORT_HEIGHT % 32) / 2;

	if (zoomflag) {
		chunks = ceil(SCREEN_WIDTH / 64);
		blocks = ceil(VIEWPORT_HEIGHT / 32);

		gpBufStart = &gpBuffer[BUFFER_WIDTH * SCREEN_Y];
		gpBufEnd = &gpBuffer[BUFFER_WIDTH * (VIEWPORT_HEIGHT + SCREEN_Y)];
	} else {
		sy -= 32;

		chunks = ceil(SCREEN_WIDTH / 2 / 64) + 1; // TODO why +1?
		blocks = ceil(VIEWPORT_HEIGHT / 2 / 32);

		gpBufStart = &gpBuffer[(-17 + SCREEN_Y) * BUFFER_WIDTH];
		gpBufEnd = &gpBuffer[(160 + SCREEN_Y) * BUFFER_WIDTH];
	}

	sx += ScrollInfo._sxoff + SCREEN_X;
	sy += ScrollInfo._syoff + SCREEN_Y + 15;

	// Center screen
	x -= chunks;
	y--;

	// Keep evaulating untill MicroTiles can't affect screen
	blocks += ceil(MicroTileLen / 2);

	if (PANELS_COVER) {
		if (zoomflag) {
			if (chrflag || questlog) {
				x += 2;
				y -= 2;
				sx += 288;
				chunks -= 4;
			}
			if (invflag || sbookflag) {
				x += 2;
				y -= 2;
				sx -= 32;
				chunks -= 4;
			}
		}
	}

	switch (ScrollInfo._sdir) {
	case SDIR_NONE:
		break;
	case SDIR_N:
		sy -= 32;
		x--;
		y--;
		blocks++;
		break;
	case SDIR_NE:
		sy -= 32;
		x--;
		y--;
		chunks++;
		blocks++;
		break;
	case SDIR_E:
		chunks++;
		break;
	case SDIR_SE:
		chunks++;
		blocks++;
		break;
	case SDIR_S:
		blocks++;
		break;
	case SDIR_SW:
		sx -= 64;
		x--;
		y++;
		chunks++;
		blocks++;
		break;
	case SDIR_W:
		sx -= 64;
		x--;
		y++;
		chunks++;
		break;
	case SDIR_NW:
		sx -= 64;
		sy -= 32;
		x -= 2;
		chunks++;
		blocks++;
		break;
	}

	for (i = 0; i < (blocks << 1); i++) {
		scrollrt_draw(x, y, sx, sy, chunks, i);
		sy += 16;
		if (i & 1)
			y++;
		else
			x++;
	}
	gpBufStart = &gpBuffer[BUFFER_WIDTH * SCREEN_Y];
	gpBufEnd = &gpBuffer[BUFFER_WIDTH * (SCREEN_HEIGHT + SCREEN_Y)];

	if (zoomflag)
		return;

	nSrcOff = SCREENXY(32, VIEWPORT_HEIGHT / 2 - 17);
	nDstOff = SCREENXY(0, VIEWPORT_HEIGHT - 2);
	wdt = SCREEN_WIDTH / 2;
	if (PANELS_COVER) {
		if (chrflag || questlog) {
			nSrcOff = SCREENXY(112, VIEWPORT_HEIGHT / 2 - 17);
			nDstOff = SCREENXY(SPANEL_WIDTH, VIEWPORT_HEIGHT - 2);
			wdt = (SCREEN_WIDTH - SPANEL_WIDTH) / 2;
		} else if (invflag || sbookflag) {
			nSrcOff = SCREENXY(112, VIEWPORT_HEIGHT / 2 - 17);
			nDstOff = SCREENXY(0, VIEWPORT_HEIGHT - 2);
			wdt = (SCREEN_WIDTH - SPANEL_WIDTH) / 2;
		}
	}

	int hgt;
	BYTE *src, *dst1, *dst2;

	src = &gpBuffer[nSrcOff];
	dst1 = &gpBuffer[nDstOff];
	dst2 = &gpBuffer[nDstOff + BUFFER_WIDTH];

	for (hgt = VIEWPORT_HEIGHT / 2; hgt != 0; hgt--, src -= BUFFER_WIDTH + wdt, dst1 -= 2 * (BUFFER_WIDTH + wdt), dst2 -= 2 * (BUFFER_WIDTH + wdt)) {
		for (i = wdt; i != 0; i--) {
			*dst1++ = *src;
			*dst1++ = *src;
			*dst2++ = *src;
			*dst2++ = *src;
			src++;
		}
	}
}

/**
 * @brief Start rendering of screen, town variation
 * @param StartX Center of view in dPiece coordinate
 * @param StartY Center of view in dPiece coordinate
 */
void DrawView(int StartX, int StartY)
{
	DrawGame(StartX, StartY);
	if (automapflag) {
		DrawAutomap();
	}
	if (stextflag && !qtextflag)
		DrawSText();
	if (invflag) {
		DrawInv();
	} else if (sbookflag) {
		DrawSpellBook();
	}

	DrawDurIcon();

	if (chrflag) {
		DrawChr();
	} else if (questlog) {
		DrawQuestLog();
	}
	if (!chrflag && plr[myplr]._pStatPts != 0 && !spselflag
		&& (!questlog || SCREEN_HEIGHT >= SPANEL_HEIGHT + PANEL_HEIGHT + 74 || SCREEN_WIDTH >= 4 * SPANEL_WIDTH)) {
		DrawLevelUpIcon();
	}
	if (uitemflag) {
		DrawUniqueInfo();
	}
	if (qtextflag) {
		DrawQText();
	}
	if (spselflag) {
		DrawSpellList();
	}
	if (dropGoldFlag) {
		DrawGoldSplit(dropGoldValue);
	}
	if (helpflag) {
		DrawHelp();
	}
	if (msgflag) {
		DrawDiabloMsg();
	}
	if (deathflag) {
		RedBack();
	} else if (PauseMode != 0) {
		gmenu_draw_pause();
	}

	DrawPlrMsg();
	gmenu_draw();
	doom_draw();
	DrawInfoBox();
	DrawLifeFlask();
	DrawManaFlask();
}

/**
 * @brief Render the whole screen black
 */
void ClearScreenBuffer()
{
	lock_buf(3);

	assert(gpBuffer);

	int i;
	BYTE *dst;

	dst = &gpBuffer[SCREENXY(0, 0)];

	for (i = 0; i < SCREEN_HEIGHT; i++, dst += BUFFER_WIDTH) {
		memset(dst, 0, SCREEN_WIDTH);
	}

	unlock_buf(3);
}

#ifdef _DEBUG
/**
 * @brief Scroll the screen when mouse is close to the edge
 */
void ScrollView()
{
	BOOL scroll;

	if (pcurs >= CURSOR_FIRSTITEM)
		return;

	scroll = FALSE;

	if (MouseX < 20) {
		if (dmaxy - 1 <= ViewY || dminx >= ViewX) {
			if (dmaxy - 1 > ViewY) {
				ViewY++;
				scroll = TRUE;
			}
			if (dminx < ViewX) {
				ViewX--;
				scroll = TRUE;
			}
		} else {
			ViewY++;
			ViewX--;
			scroll = TRUE;
		}
	}
	if (MouseX > SCREEN_WIDTH - 20) {
		if (dmaxx - 1 <= ViewX || dminy >= ViewY) {
			if (dmaxx - 1 > ViewX) {
				ViewX++;
				scroll = TRUE;
			}
			if (dminy < ViewY) {
				ViewY--;
				scroll = TRUE;
			}
		} else {
			ViewY--;
			ViewX++;
			scroll = TRUE;
		}
	}
	if (MouseY < 20) {
		if (dminy >= ViewY || dminx >= ViewX) {
			if (dminy < ViewY) {
				ViewY--;
				scroll = TRUE;
			}
			if (dminx < ViewX) {
				ViewX--;
				scroll = TRUE;
			}
		} else {
			ViewX--;
			ViewY--;
			scroll = TRUE;
		}
	}
	if (MouseY > SCREEN_HEIGHT - 20) {
		if (dmaxy - 1 <= ViewY || dmaxx - 1 <= ViewX) {
			if (dmaxy - 1 > ViewY) {
				ViewY++;
				scroll = TRUE;
			}
			if (dmaxx - 1 > ViewX) {
				ViewX++;
				scroll = TRUE;
			}
		} else {
			ViewX++;
			ViewY++;
			scroll = TRUE;
		}
	}

	if (scroll)
		ScrollInfo._sdir = SDIR_NONE;
}
#endif

/**
 * @brief Initialize the FPS meter
 */
void EnableFrameCount()
{
	frameflag = frameflag == 0;
	framestart = GetTickCount();
}

/**
 * @brief Display the current average FPS over 1 sec
 */
static void DrawFPS()
{
	DWORD tc, frames;
	char String[12];
	HDC hdc;

	if (frameflag && gbActive && pPanelText) {
		frameend++;
		tc = GetTickCount();
		frames = tc - framestart;
		if (tc - framestart >= 1000) {
			framestart = tc;
			framerate = 1000 * frameend / frames;
			frameend = 0;
		}
		wsprintf(String, "%d FPS", framerate);
		PrintGameStr(8, 65, String, COL_RED);
	}
}

/**
 * @brief Update part of the screen from the backbuffer
 * @param dwX Backbuffer coordinate
 * @param dwY Backbuffer coordinate
 * @param dwWdt Backbuffer coordinate
 * @param dwHgt Backbuffer coordinate
 */
static void DoBlitScreen(DWORD dwX, DWORD dwY, DWORD dwWdt, DWORD dwHgt)
{
	RECT SrcRect;

	SrcRect.left = dwX + SCREEN_X;
	SrcRect.top = dwY + SCREEN_Y;
	SrcRect.right = SrcRect.left + dwWdt - 1;
	SrcRect.bottom = SrcRect.top + dwHgt - 1;

	BltFast(dwX, dwY, &SrcRect);
}

/**
 * @brief Check render pipline and blit indivudal screen parts
 * @param dwHgt Section of screen to update from top to bottom
 * @param draw_desc Render info box
 * @param draw_hp Render halth bar
 * @param draw_mana Render mana bar
 * @param draw_sbar Render belt
 * @param draw_btn Render panel buttons
 */
static void DrawMain(int dwHgt, BOOL draw_desc, BOOL draw_hp, BOOL draw_mana, BOOL draw_sbar, BOOL draw_btn)
{
	int ysize;
	DWORD dwTicks;
	BOOL retry;

	ysize = dwHgt;

	if (!gbActive) {
		return;
	}

	assert(ysize >= 0 && ysize <= SCREEN_HEIGHT);

	if (ysize > 0) {
		DoBlitScreen(0, 0, SCREEN_WIDTH, ysize);
	}
	if (ysize < SCREEN_HEIGHT) {
		if (draw_sbar) {
			DoBlitScreen(PANEL_LEFT + 204, PANEL_TOP + 5, 232, 28);
		}
		if (draw_desc) {
			DoBlitScreen(PANEL_LEFT + 176, PANEL_TOP + 46, 288, 60);
		}
		if (draw_mana) {
			DoBlitScreen(PANEL_LEFT + 460, PANEL_TOP, 88, 72);
			DoBlitScreen(PANEL_LEFT + 564, PANEL_TOP + 64, 56, 56);
		}
		if (draw_hp) {
			DoBlitScreen(PANEL_LEFT + 96, PANEL_TOP, 88, 72);
		}
		if (draw_btn) {
			DoBlitScreen(PANEL_LEFT + 8, PANEL_TOP + 5, 72, 119);
			DoBlitScreen(PANEL_LEFT + 556, PANEL_TOP + 5, 72, 48);
			if (gbMaxPlayers > 1) {
				DoBlitScreen(PANEL_LEFT + 84, PANEL_TOP + 91, 36, 32);
				DoBlitScreen(PANEL_LEFT + 524, PANEL_TOP + 91, 36, 32);
			}
		}
		if (sgdwCursWdtOld != 0) {
			DoBlitScreen(sgdwCursXOld, sgdwCursYOld, sgdwCursWdtOld, sgdwCursHgtOld);
		}
		if (sgdwCursWdt != 0) {
			DoBlitScreen(sgdwCursX, sgdwCursY, sgdwCursWdt, sgdwCursHgt);
		}
	}
}

void scrollrt_draw_game_screen(BOOL draw_cursor)
{
	int hgt;

	if (force_redraw == 255) {
		force_redraw = 0;
		hgt = SCREEN_HEIGHT;
	} else {
		hgt = 0;
	}

	if (draw_cursor) {
		lock_buf(0);
		scrollrt_draw_cursor_item();
		unlock_buf(0);
	}

	DrawMain(hgt, 0, 0, 0, 0, 0);

	if (draw_cursor) {
		lock_buf(0);
		scrollrt_draw_cursor_back_buffer();
		unlock_buf(0);
	}
}

/**
 * @brief Render the game
 */
void DrawAndBlit()
{
	int hgt;
	BOOL ddsdesc, ctrlPan;

	if (!gbRunGame) {
		return;
	}

	if (SCREEN_WIDTH > PANEL_WIDTH || SCREEN_HEIGHT > VIEWPORT_HEIGHT + PANEL_HEIGHT || force_redraw == 255) {
		drawhpflag = TRUE;
		drawmanaflag = TRUE;
		drawbtnflag = TRUE;
		drawsbarflag = TRUE;
		ddsdesc = FALSE;
		ctrlPan = TRUE;
		hgt = SCREEN_HEIGHT;
	} else {
		ddsdesc = TRUE;
		ctrlPan = FALSE;
		hgt = VIEWPORT_HEIGHT;
	}

	force_redraw = 0;

	lock_buf(0);
	DrawView(ViewX, ViewY);
	if (ctrlPan) {
		ClearCtrlPan();
	}
	if (drawhpflag) {
		UpdateLifeFlask();
	}
	if (drawmanaflag) {
		UpdateManaFlask();
	}
	if (drawbtnflag) {
		DrawCtrlPan();
	}
	if (drawsbarflag) {
		DrawInvBelt();
	}
	if (talkflag) {
		DrawTalkPan();
		hgt = SCREEN_HEIGHT;
	}
	scrollrt_draw_cursor_item();

	DrawFPS();

	unlock_buf(0);

	DrawMain(hgt, ddsdesc, drawhpflag, drawmanaflag, drawsbarflag, drawbtnflag);

	lock_buf(0);
	scrollrt_draw_cursor_back_buffer();
	unlock_buf(0);

	drawhpflag = FALSE;
	drawmanaflag = FALSE;
	drawbtnflag = FALSE;
	drawsbarflag = FALSE;
}

DEVILUTION_END_NAMESPACE
