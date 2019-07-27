#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

int light_table_index;
int PitchTbl[1024];
DWORD sgdwCursWdtOld;
DWORD sgdwCursX;
DWORD sgdwCursY;
BYTE *gpBufEnd;
DWORD sgdwCursHgt;
DWORD level_cel_block;
DWORD sgdwCursXOld;
DWORD sgdwCursYOld;
char arch_draw_type;
DDSURFACEDESC DDS_desc;
int cel_transparency_active;
int level_piece_id;
DWORD sgdwCursWdt;
void (*DrawPlrProc)(int, int, int, int, int, BYTE *, int, int, int, int);
BYTE sgSaveBack[8192];
int draw_monster_num;
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

void DrawMissile(int x, int y, int sx, int sy, int CelSkip, int CelCap, BOOL pre)
{
	int i, mx, my, nCel;
	MissileStruct *m;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

	if (dMissile[x][y] == -1) {
		for (i = 0; i < nummissiles; i++) {
			/// ASSERT: assert(missileactive[i] < MAXMISSILES);
			if (missileactive[i] >= MAXMISSILES)
				break;
			m = &missile[missileactive[i]];
			if (m->_mix == x && m->_miy == y && m->_miPreFlag == pre && m->_miDrawFlag) {
				pCelBuff = m->_miAnimData;
				if (!pCelBuff) {
					// app_fatal("Draw Missile type %d: NULL Cel Buffer", m->_mitype);
					return;
				}
				nCel = m->_miAnimFrame;
				pFrameTable = (DWORD *)pCelBuff;
				if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
					// app_fatal("Draw Missile: frame %d of %d, missile type==%d", nCel, pFrameTable[0], m->_mitype);
					return;
				}
				mx = sx + m->_mixoff - m->_miAnimWidth2;
				my = sy + m->_miyoff;
				if (m->_miUniqTrans)
					Cl2DecodeFrm3(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap, m->_miUniqTrans + 3);
				else if (m->_miLightFlag)
					Cl2DecodeLightTbl(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap);
				else
					Cl2DecodeFrm1(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap);
			}
		}
	} else {
		m = &missile[dMissile[x][y] - 1];
		if (m->_miPreFlag == pre && m->_miDrawFlag) {
			pCelBuff = m->_miAnimData;
			if (!pCelBuff) {
				// app_fatal("Draw Missile 2 type %d: NULL Cel Buffer", m->_mitype);
				return;
			}
			nCel = m->_miAnimFrame;
			pFrameTable = (DWORD *)pCelBuff;
			if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
				// app_fatal("Draw Missile 2: frame %d of %d, missile type==%d", nCel, pFrameTable[0], m->_mitype);
				return;
			}
			mx = sx + m->_mixoff - m->_miAnimWidth2;
			my = sy + m->_miyoff;
			if (m->_miUniqTrans)
				Cl2DecodeFrm3(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap, m->_miUniqTrans + 3);
			else if (m->_miLightFlag)
				Cl2DecodeLightTbl(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap);
			else
				Cl2DecodeFrm1(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap);
		}
	}
}

void DrawClippedMissile(int x, int y, int sx, int sy, int CelSkip, int CelCap, BOOL pre)
{
	int i, mx, my, nCel;
	MissileStruct *m;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

	if (dMissile[x][y] == -1) {
		for (i = 0; i < nummissiles; i++) {
			/// ASSERT: assert(missileactive[i] < MAXMISSILES);
			if (missileactive[i] >= MAXMISSILES)
				break;
			m = &missile[missileactive[i]];
			if (m->_mix == x && m->_miy == y && m->_miPreFlag == pre && m->_miDrawFlag) {
				pCelBuff = m->_miAnimData;
				if (!pCelBuff) {
					// app_fatal("Draw Missile type %d Clipped: NULL Cel Buffer", m->_mitype);
					return;
				}
				nCel = m->_miAnimFrame;
				pFrameTable = (DWORD *)pCelBuff;
				if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
					// app_fatal("Draw Clipped Missile: frame %d of %d, missile type==%d", nCel, pFrameTable[0], m->_mitype);
					return;
				}
				mx = sx + m->_mixoff - m->_miAnimWidth2;
				my = sy + m->_miyoff;
				if (m->_miUniqTrans)
					Cl2DecodeFrm5(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap, m->_miUniqTrans + 3);
				else if (m->_miLightFlag)
					Cl2DecodeFrm6(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap);
				else
					Cl2DecodeFrm4(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap);
			}
		}
	} else {
		m = &missile[dMissile[x][y] - 1];
		if (m->_miPreFlag == pre && m->_miDrawFlag) {
			pCelBuff = m->_miAnimData;
			if (!pCelBuff) {
				// app_fatal("Draw Missile 2 type %d Clipped: NULL Cel Buffer", m->_mitype);
				return;
			}
			nCel = m->_miAnimFrame;
			pFrameTable = (DWORD *)pCelBuff;
			if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
				// app_fatal("Draw Clipped Missile 2: frame %d of %d, missile type==%d", nCel, pFrameTable[0], m->_mitype);
				return;
			}
			mx = sx + m->_mixoff - m->_miAnimWidth2;
			my = sy + m->_miyoff;
			if (m->_miUniqTrans)
				Cl2DecodeFrm5(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap, m->_miUniqTrans + 3);
			else if (m->_miLightFlag)
				Cl2DecodeFrm6(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap);
			else
				Cl2DecodeFrm4(mx, my, m->_miAnimData, m->_miAnimFrame, m->_miAnimWidth, CelSkip, CelCap);
		}
	}
}

void DrawDeadPlayer(int x, int y, int sx, int sy, int CelSkip, int CelCap, BOOL clipped)
{
	int i, px, py, nCel;
	PlayerStruct *p;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

	if (clipped)
		DrawPlrProc = DrawClippedPlayer;
	else
		DrawPlrProc = DrawPlayer;

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
			pFrameTable = (DWORD *)pCelBuff;
			if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
				// app_fatal("Drawing dead player %d \"%s\": facing %d, frame %d of %d", i, p->_pName, p->_pdir, nCel, pFrameTable[0]);
				break;
			}
			dFlags[x][y] |= BFLAG_DEAD_PLAYER;
			px = sx + p->_pxoff - p->_pAnimWidth2;
			py = sy + p->_pyoff;
			DrawPlrProc(i, x, y, px, py, p->_pAnimData, p->_pAnimFrame, p->_pAnimWidth, CelSkip, CelCap);
		}
	}
}

void DrawPlayer(int pnum, int x, int y, int px, int py, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int l;
	DWORD *pFrameTable;

	if (dFlags[x][y] & BFLAG_LIT || plr[myplr]._pInfraFlag || !setlevel && !currlevel) {
		if (!pCelBuff) {
			// app_fatal("Drawing player %d \"%s\": NULL Cel Buffer", pnum, plr[pnum]._pName);
			return;
		}
		pFrameTable = (DWORD *)pCelBuff;
		if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
			/*
			const char *szMode = "unknown action";
			if(plr[pnum]._pmode <= 11)
				szMode = szPlrModeAssert[plr[pnum]._pmode];
			app_fatal(
				"Drawing player %d \"%s\" %s: facing %d, frame %d of %d",
				pnum,
				plr[pnum]._pName,
				szMode,
				plr[pnum]._pdir,
				nCel,
				pFrameTable[0]);
			*/
			return;
		}
		if (pnum == pcursplr)
			Cl2DecodeFrm2(165, px, py, pCelBuff, nCel, nWidth, CelSkip, CelCap);
		if (pnum == myplr) {
			Cl2DecodeFrm1(px, py, pCelBuff, nCel, nWidth, CelSkip, CelCap);
			if (plr[pnum].pManaShield)
				Cl2DecodeFrm1(
				    px + plr[pnum]._pAnimWidth2 - misfiledata[MFILE_MANASHLD].mAnimWidth2[0],
				    py,
				    misfiledata[MFILE_MANASHLD].mAnimData[0],
				    1,
				    misfiledata[MFILE_MANASHLD].mAnimWidth[0],
				    CelSkip,
				    CelCap);
		} else if (!(dFlags[x][y] & BFLAG_LIT) || plr[myplr]._pInfraFlag && light_table_index > 8) {
			Cl2DecodeFrm3(px, py, pCelBuff, nCel, nWidth, CelSkip, CelCap, 1);
			if (plr[pnum].pManaShield)
				Cl2DecodeFrm3(
				    px + plr[pnum]._pAnimWidth2 - misfiledata[MFILE_MANASHLD].mAnimWidth2[0],
				    py,
				    misfiledata[MFILE_MANASHLD].mAnimData[0],
				    1,
				    misfiledata[MFILE_MANASHLD].mAnimWidth[0],
				    CelSkip,
				    CelCap,
				    1);
		} else {
			l = light_table_index;
			if (light_table_index < 5)
				light_table_index = 0;
			else
				light_table_index -= 5;
			Cl2DecodeLightTbl(px, py, pCelBuff, nCel, nWidth, CelSkip, CelCap);
			if (plr[pnum].pManaShield)
				Cl2DecodeLightTbl(
				    px + plr[pnum]._pAnimWidth2 - misfiledata[MFILE_MANASHLD].mAnimWidth2[0],
				    py,
				    misfiledata[MFILE_MANASHLD].mAnimData[0],
				    1,
				    misfiledata[MFILE_MANASHLD].mAnimWidth[0],
				    CelSkip,
				    CelCap);
			light_table_index = l;
		}
	}
}

void DrawClippedPlayer(int pnum, int x, int y, int px, int py, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int l;
	DWORD *pFrameTable;

	if (dFlags[x][y] & BFLAG_LIT || plr[myplr]._pInfraFlag) {
		if (!pCelBuff) {
			// app_fatal("Drawing player %d \"%s\" clipped: NULL Cel Buffer", pnum, plr[pnum]._pName);
			return;
		}
		pFrameTable = (DWORD *)pCelBuff;
		if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
			/*
			const char *szMode = "unknown action";
			if(plr[pnum]._pmode <= 11)
				szMode = szPlrModeAssert[plr[pnum]._pmode];
			app_fatal(
				"Drawing player %d \"%s\" %s clipped: facing %d, frame %d of %d",
				pnum,
				plr[pnum]._pName,
				szMode,
				plr[pnum]._pdir,
				nCel,
				pFrameTable[0]);
			*/
			return;
		}
		if (pnum == pcursplr)
			Cl2DecodeClrHL(165, px, py, pCelBuff, nCel, nWidth, CelSkip, CelCap);
		if (pnum == myplr) {
			Cl2DecodeFrm4(px, py, pCelBuff, nCel, nWidth, CelSkip, CelCap);
			if (plr[pnum].pManaShield)
				Cl2DecodeFrm4(
				    px + plr[pnum]._pAnimWidth2 - misfiledata[MFILE_MANASHLD].mAnimWidth2[0],
				    py,
				    misfiledata[MFILE_MANASHLD].mAnimData[0],
				    1,
				    misfiledata[MFILE_MANASHLD].mAnimWidth[0],
				    CelSkip,
				    CelCap);
		} else if (!(dFlags[x][y] & BFLAG_LIT) || plr[myplr]._pInfraFlag && light_table_index > 8) {
			Cl2DecodeFrm5(px, py, pCelBuff, nCel, nWidth, CelSkip, CelCap, 1);
			if (plr[pnum].pManaShield)
				Cl2DecodeFrm5(
				    px + plr[pnum]._pAnimWidth2 - misfiledata[MFILE_MANASHLD].mAnimWidth2[0],
				    py,
				    misfiledata[MFILE_MANASHLD].mAnimData[0],
				    1,
				    misfiledata[MFILE_MANASHLD].mAnimWidth[0],
				    CelSkip,
				    CelCap,
				    1);
		} else {
			l = light_table_index;
			if (light_table_index < 5)
				light_table_index = 0;
			else
				light_table_index -= 5;
			Cl2DecodeFrm6(px, py, pCelBuff, nCel, nWidth, CelSkip, CelCap);
			if (plr[pnum].pManaShield)
				Cl2DecodeFrm6(
				    px + plr[pnum]._pAnimWidth2 - misfiledata[MFILE_MANASHLD].mAnimWidth2[0],
				    py,
				    misfiledata[MFILE_MANASHLD].mAnimData[0],
				    1,
				    misfiledata[MFILE_MANASHLD].mAnimWidth[0],
				    CelSkip,
				    CelCap);
			light_table_index = l;
		}
	}
}

void DrawView(int StartX, int StartY)
{
	if (zoomflag) {
		DrawGame(StartX, StartY);
	} else {
		DrawZoom(StartX, StartY);
	}
	if (automapflag) {
		DrawAutomap();
	}
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
	} else if (plr[myplr]._pStatPts != 0 && !spselflag) {
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

void DrawGame(int x, int y)
{
	int i, sx, sy, chunks, blocks;

	scr_pix_width = SCREEN_WIDTH;
	scr_pix_height = VIEWPORT_HEIGHT;
	dword_5C2FF8 = SCREEN_WIDTH / 64;
	dword_5C2FFC = VIEWPORT_HEIGHT / 32;

	sx = ScrollInfo._sxoff + 64;
	sy = ScrollInfo._syoff + 175;
	x -= 10;
	y--;
	chunks = 10;
	blocks = 8;

	if (chrflag || questlog) {
		x += 2;
		y -= 2;
		sx += 288;
		chunks = 6;
	}
	if (invflag || sbookflag) {
		x += 2;
		y -= 2;
		sx -= 32;
		chunks = 6;
	}

	switch (ScrollInfo._sdir) {
	case SDIR_NONE:
		break;
	case SDIR_NE:
		chunks++;
	case SDIR_N:
		sy -= 32;
		x--;
		y--;
		blocks++;
		break;
	case SDIR_SE:
		blocks++;
	case SDIR_E:
		chunks++;
		break;
	case SDIR_S:
		blocks++;
		break;
	case SDIR_SW:
		blocks++;
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

	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[0 + SCREEN_Y]];
	for (i = 0; i < 4; i++) {
		scrollrt_draw_upper(x, y, sx, sy, chunks, i, 0);
		y++;
		sx -= 32;
		sy += 16;
		scrollrt_draw_upper(x, y, sx, sy, chunks, i, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[VIEWPORT_HEIGHT + SCREEN_Y]];
	for (i = 0; i < blocks; i++) {
		scrollrt_draw_lower(x, y, sx, sy, chunks, 0);
		y++;
		sx -= 32;
		sy += 16;
		scrollrt_draw_lower(x, y, sx, sy, chunks, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	arch_draw_type = 0;
	for (i = 0; i < 4; i++) {
		scrollrt_draw_lower_2(x, y, sx, sy, chunks, i, 0);
		y++;
		sx -= 32;
		sy += 16;
		scrollrt_draw_lower_2(x, y, sx, sy, chunks, i, 1);
		x++;
		sx += 32;
		sy += 16;
	}
}

void scrollrt_draw_lower(int x, int y, int sx, int sy, int chunks, int eflag)
{
	int i, j;
	BYTE *dst;
	MICROS *pMap;

	/// ASSERT: assert(gpBuffer);

	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];

	if (eflag) {
		if ((DWORD)y < MAXDUNY && (DWORD)x < MAXDUNX) {
			level_piece_id = dPiece[x][y];
			light_table_index = dLight[x][y];
			if (level_piece_id != 0) {
				dst = &gpBuffer[sx + 32 + PitchTbl[sy]];
				cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				arch_draw_type = 2;
				level_cel_block = pMap->mt[1];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				arch_draw_type = 0;
				dst -= BUFFER_WIDTH * 32;
				level_cel_block = pMap->mt[3];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				dst -= BUFFER_WIDTH * 32;
				level_cel_block = pMap->mt[5];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				dst -= BUFFER_WIDTH * 32;
				level_cel_block = pMap->mt[7];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				dst -= BUFFER_WIDTH * 32;
				level_cel_block = pMap->mt[9];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				dst -= BUFFER_WIDTH * 32;
				level_cel_block = pMap->mt[11];
				if (level_cel_block != 0 && leveltype == DTYPE_HELL) {
					drawLowerScreen(dst);
				}
				scrollrt_draw_clipped_dungeon(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 0);
			} else {
				world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
			}
		}
		x++;
		y--;
		sx += 64;
		pMap++;
		chunks--;
	}

	j = chunks;
	while (j != 0) {
		j--;
		if (y < 0 || x >= MAXDUNX) {
			break;
		}
		if (y < MAXDUNY && x >= 0) {
			level_piece_id = dPiece[x][y];
			light_table_index = dLight[x][y];
			if (level_piece_id == 0) {
				world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
			} else {
				dst = &gpBuffer[sx + PitchTbl[sy]];
				cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				arch_draw_type = 1;
				level_cel_block = pMap->mt[0];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				arch_draw_type = 2;
				level_cel_block = pMap->mt[1];
				if (level_cel_block != 0) {
					drawLowerScreen(dst + 32);
				}
				arch_draw_type = 0;
				for (i = 2; i < MicroTileLen; i += 2) {
					dst -= BUFFER_WIDTH * 32;
					level_cel_block = pMap->mt[i];
					if (level_cel_block != 0) {
						drawLowerScreen(dst);
					}
					level_cel_block = pMap->mt[i + 1];
					if (level_cel_block != 0) {
						drawLowerScreen(dst + 32);
					}
				}
				scrollrt_draw_clipped_dungeon(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 1);
			}
		}
		x++;
		y--;
		sx += 64;
		pMap++;
	}

	if (eflag && (DWORD)y < MAXDUNY && (DWORD)x < MAXDUNX) {
		level_piece_id = dPiece[x][y];
		light_table_index = dLight[x][y];
		if (level_piece_id == 0) {
			world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
		} else {
			dst = &gpBuffer[sx + PitchTbl[sy]];
			cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
			arch_draw_type = 1;
			level_cel_block = pMap->mt[0];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			arch_draw_type = 0;
			dst -= BUFFER_WIDTH * 32;
			level_cel_block = pMap->mt[2];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			dst -= BUFFER_WIDTH * 32;
			level_cel_block = pMap->mt[4];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			dst -= BUFFER_WIDTH * 32;
			level_cel_block = pMap->mt[6];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			dst -= BUFFER_WIDTH * 32;
			level_cel_block = pMap->mt[8];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			dst -= BUFFER_WIDTH * 32;
			level_cel_block = pMap->mt[10];
			if (level_cel_block != 0 && leveltype == DTYPE_HELL) {
				drawLowerScreen(dst);
			}
			scrollrt_draw_clipped_dungeon(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 0);
		}
	}
}

void scrollrt_draw_clipped_dungeon(BYTE *pBuff, int sx, int sy, int dx, int dy, int eflag)
{
	int px, py, nCel, nMon, negMon, p;
	char bFlag, bDead, bObj, bItem, bPlr, bArch, bMap, negPlr, dd;
	DeadStruct *pDeadGuy;
	ItemStruct *pItem;
	PlayerStruct *pPlayer;
	MonsterStruct *pMonster;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

	/// ASSERT: assert((DWORD)sx < MAXDUNX);
	/// ASSERT: assert((DWORD)sy < MAXDUNY);
	bFlag = dFlags[sx][sy];
	bDead = dDead[sx][sy];
	bObj = dObject[sx][sy];
	bItem = dItem[sx][sy];
	bPlr = dPlayer[sx][sy];
	bArch = dArch[sx][sy];
	bMap = dTransVal[sx][sy];
	nMon = dMonster[sx][sy];

	/// ASSERT: assert((DWORD)(sy-1) < MAXDUNY);
	negPlr = dPlayer[sx][sy - 1];
	negMon = dMonster[sx][sy - 1];

	if (visiondebug && bFlag & BFLAG_LIT) {
		Cel2DecodeHdrOnly(pBuff, pSquareCel, 1, 64, 0, 8);
	}
	if (MissilePreFlag && bFlag & BFLAG_MISSILE) {
		DrawClippedMissile(sx, sy, dx, dy, 0, 8, 1);
	}
	if (light_table_index < lightmax) {
		if (bDead != 0) {
			pDeadGuy = &dead[(bDead & 0x1F) - 1];
			dd = (bDead >> 5) & 7;
			px = dx - pDeadGuy->_deadWidth2;
			pCelBuff = pDeadGuy->_deadData[dd];
			/// ASSERT: assert(pDeadGuy->_deadData[dd] != NULL);
			if (pCelBuff != NULL) {
				pFrameTable = (DWORD *)pDeadGuy->_deadData[dd];
				nCel = pDeadGuy->_deadFrame;
				if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
					if (pDeadGuy->_deadtrans != 0) {
						Cl2DecodeFrm5(px, dy, pCelBuff, nCel, pDeadGuy->_deadWidth, 0, 8, pDeadGuy->_deadtrans);
					} else {
						Cl2DecodeFrm6(px, dy, pCelBuff, pDeadGuy->_deadFrame, pDeadGuy->_deadWidth, 0, 8);
					}
				} else {
					// app_fatal("Clipped dead sub: frame %d of %d, deadnum==%d", nCel, pFrameTable[0], (bDead & 0x1F) - 1);
				}
			}
		}
		if (bObj != 0) {
			DrawClippedObject(sx, sy, dx, dy, 1, 0, 8);
		}
	}
	if (bItem != 0) {
		pItem = &item[bItem - 1];
		if (!pItem->_iPostDraw) {
			/// ASSERT: assert((unsigned char)bItem <= MAXITEMS);
			if ((BYTE)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if (pCelBuff != NULL) {
					pFrameTable = (DWORD *)pCelBuff;
					nCel = pItem->_iAnimFrame;
					if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if (bItem - 1 == pcursitem) {
							CelDrawHdrClrHL(181, px, dy, pCelBuff, nCel, pItem->_iAnimWidth, 0, 8);
						}
						Cel2DecodeHdrLight(px, dy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth, 0, 8);
					} else {
						/*
						app_fatal(
							"Draw Clipped \"%s\" Item: frame %d of %d, item type==%d",
							pItem->_iIName,
							nCel,
							pFrameTable[0],
							pItem->_itype);
						*/
					}
				} else {
					// app_fatal("Draw Item \"%s\" Clipped 1: NULL Cel Buffer", pItem->_iIName);
				}
			}
		}
	}
	if (bFlag & BFLAG_PLAYERLR) {
		p = -(negPlr + 1);
		if ((DWORD)p < MAX_PLRS) {
			pPlayer = &plr[p];
			px = dx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
			py = dy + pPlayer->_pyoff;
			DrawClippedPlayer(p, sx, sy - 1, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth, 0, 8);
			if (eflag && pPlayer->_peflag != 0) {
				if (pPlayer->_peflag == 2) {
					scrollrt_draw_clipped_e_flag(pBuff - (BUFFER_WIDTH * 16 + 96), sx - 2, sy + 1, dx - 96, dy - 16);
				}
				scrollrt_draw_clipped_e_flag(pBuff - 64, sx - 1, sy + 1, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player clipped: tried to draw illegal player %d", p);
		}
	}
	if (bFlag & BFLAG_MONSTLR && (bFlag & BFLAG_LIT || plr[myplr]._pInfraFlag) && negMon < 0) {
		draw_monster_num = -(negMon + 1);
		if ((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if (!(pMonster->_mFlags & 1)) {
				if (pMonster->MType != NULL) {
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
					py = dy + pMonster->_myoff;
					if (draw_monster_num == pcursmonst) {
						Cl2DecodeClrHL(233, px, py, pMonster->_mAnimData, pMonster->_mAnimFrame, pMonster->MType->width, 0, 8);
					}
					DrawClippedMonster(sx, sy, px, py, draw_monster_num, 0, 8);
					if (eflag && pMonster->_meflag) {
						scrollrt_draw_clipped_e_flag(pBuff - 64, sx - 1, sy + 1, dx - 64, dy);
					}
				} else {
					// app_fatal("Draw Monster \"%s\" Clipped: uninitialized monster", pMonster->mName);
				}
			}
		} else {
			// app_fatal("Draw Monster Clipped: tried to draw illegal monster %d", draw_monster_num);
		}
	}
	if (bFlag & BFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(sx, sy, dx, dy, 0, 8, 1);
	}
	if (bPlr > 0) {
		p = bPlr - 1;
		if ((DWORD)p < MAX_PLRS) {
			pPlayer = &plr[p];
			px = dx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
			py = dy + pPlayer->_pyoff;
			DrawClippedPlayer(p, sx, sy, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth, 0, 8);
			if (eflag && pPlayer->_peflag != 0) {
				if (pPlayer->_peflag == 2) {
					scrollrt_draw_clipped_e_flag(pBuff - (BUFFER_WIDTH * 16 + 96), sx - 2, sy + 1, dx - 96, dy - 16);
				}
				scrollrt_draw_clipped_e_flag(pBuff - 64, sx - 1, sy + 1, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player clipped: tried to draw illegal player %d", p);
		}
	}
	if (nMon > 0 && (bFlag & BFLAG_LIT || plr[myplr]._pInfraFlag)) {
		draw_monster_num = nMon - 1;
		if ((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if (!(pMonster->_mFlags & 1)) {
				if (pMonster->MType != NULL) {
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
					py = dy + pMonster->_myoff;
					if (draw_monster_num == pcursmonst) {
						Cl2DecodeClrHL(233, px, py, pMonster->_mAnimData, pMonster->_mAnimFrame, pMonster->MType->width, 0, 8);
					}
					DrawClippedMonster(sx, sy, px, py, draw_monster_num, 0, 8);
					if (eflag && pMonster->_meflag) {
						scrollrt_draw_clipped_e_flag(pBuff - 64, sx - 1, sy + 1, dx - 64, dy);
					}
				} else {
					// app_fatal("Draw Monster \"%s\" Clipped: uninitialized monster", pMonster->mName);
				}
			}
		} else {
			// app_fatal("Draw Monster Clipped: tried to draw illegal monster %d", draw_monster_num);
		}
	}
	if (bFlag & BFLAG_MISSILE) {
		DrawClippedMissile(sx, sy, dx, dy, 0, 8, 0);
	}
	if (bObj != 0 && light_table_index < lightmax) {
		DrawClippedObject(sx, sy, dx, dy, 0, 0, 8);
	}
	if (bItem != 0) {
		pItem = &item[bItem - 1];
		if (pItem->_iPostDraw) {
			/// ASSERT: assert((unsigned char)bItem <= MAXITEMS);
			if ((BYTE)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if (pCelBuff != NULL) {
					pFrameTable = (DWORD *)pCelBuff;
					nCel = pItem->_iAnimFrame;
					if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if (bItem - 1 == pcursitem) {
							CelDrawHdrClrHL(181, px, dy, pCelBuff, nCel, pItem->_iAnimWidth, 0, 8);
						}
						Cel2DecodeHdrLight(px, dy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth, 0, 8);
					} else {
						/*
						app_fatal(
							"Draw Clipped \"%s\" Item 2: frame %d of %d, item type==%d",
							pItem->_iIName,
							nCel,
							pFrameTable[0],
							pItem->_itype);
						*/
					}
				} else {
					// app_fatal("Draw Item \"%s\" Clipped 2: NULL Cel Buffer", pItem->_iIName);
				}
			}
		}
	}
	if (bArch != 0) {
		cel_transparency_active = TransList[bMap];
		Cel2DecodeLightTrans(pBuff, pSpecialCels, bArch, 64, 0, 8);
	}
}

void DrawClippedMonster(int x, int y, int mx, int my, int m, int CelSkip, int CelCap)
{
	int nCel;
	char trans;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

	if ((DWORD)m >= MAXMONSTERS) {
		// app_fatal("Draw Monster Clipped: tried to draw illegal monster %d", m);
		return;
	}

	pCelBuff = monster[m]._mAnimData;
	if (!pCelBuff) {
		// app_fatal("Draw Monster \"%s\" Clipped: NULL Cel Buffer", monster[m].mName);
		return;
	}

	nCel = monster[m]._mAnimFrame;
	pFrameTable = (DWORD *)pCelBuff;
	if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
		/*
		const char *szMode = "unknown action";
		if(monster[m]._mmode <= 17)
			szMode = szMonModeAssert[monster[m]._mmode];
		app_fatal(
			"Draw Monster \"%s\" %s Clipped: facing %d, frame %d of %d",
			monster[m].mName,
			szMode,
			monster[m]._mdir,
			nCel,
			pFrameTable[0]);
		*/
		return;
	}

	if (!(dFlags[x][y] & BFLAG_LIT)) {
		Cl2DecodeFrm5(mx, my, monster[m]._mAnimData, monster[m]._mAnimFrame, monster[m].MType->width, CelSkip, CelCap, 1);
	} else {
		trans = 0;
		if (monster[m]._uniqtype)
			trans = monster[m]._uniqtrans + 4;
		if (monster[m]._mmode == MM_STONE)
			trans = 2;
		if (plr[myplr]._pInfraFlag && light_table_index > 8)
			trans = 1;
		if (trans)
			Cl2DecodeFrm5(mx, my, monster[m]._mAnimData, monster[m]._mAnimFrame, monster[m].MType->width, CelSkip, CelCap, trans);
		else
			Cl2DecodeFrm6(mx, my, monster[m]._mAnimData, monster[m]._mAnimFrame, monster[m].MType->width, CelSkip, CelCap);
	}
}

void DrawClippedObject(int x, int y, int ox, int oy, BOOL pre, int CelSkip, int CelCap)
{
	int sx, sy, xx, yy, nCel;
	char bv;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

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
		CelSkip = 0;
		CelCap = 8;
	}

	/// ASSERT: assert((unsigned char)bv < MAXOBJECTS);
	if ((BYTE)bv >= MAXOBJECTS)
		return;

	pCelBuff = object[bv]._oAnimData;
	if (!pCelBuff) {
		// app_fatal("Draw Object type %d Clipped: NULL Cel Buffer", object[bv]._otype);
		return;
	}

	nCel = object[bv]._oAnimFrame;
	pFrameTable = (DWORD *)pCelBuff;
	if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
		// app_fatal("Draw Clipped Object: frame %d of %d, object type==%d", nCel, pFrameTable[0], object[bv]._otype);
		return;
	}

	if (bv == pcursobj)
		CelDrawHdrClrHL(194, sx, sy, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, CelSkip, CelCap);
	if (object[bv]._oLight)
		Cel2DecodeHdrLight(sx, sy, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, CelSkip, CelCap);
	else
		Cel2DrawHdrOnly(sx, sy, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, CelSkip, CelCap);
}

void scrollrt_draw_clipped_e_flag(BYTE *pBuff, int x, int y, int sx, int sy)
{
	int i, lti_old, cta_old, lpi_old;
	BYTE *dst;
	MICROS *pMap;

	lti_old = light_table_index;
	cta_old = cel_transparency_active;
	lpi_old = level_piece_id;

	level_piece_id = dPiece[x][y];
	light_table_index = dLight[x][y];
	cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];

	dst = pBuff;
	arch_draw_type = 1;
	level_cel_block = pMap->mt[0];
	if (level_cel_block != 0) {
		drawLowerScreen(dst);
	}
	arch_draw_type = 2;
	level_cel_block = pMap->mt[1];
	if (level_cel_block != 0) {
		drawLowerScreen(dst + 32);
	}

	dst = pBuff;
	arch_draw_type = 0;
	for (i = 2; i < MicroTileLen; i += 2) {
		dst -= BUFFER_WIDTH * 32;
		level_cel_block = pMap->mt[i];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[i + 1];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
	}

	scrollrt_draw_clipped_dungeon(pBuff, x, y, sx, sy, 0);

	light_table_index = lti_old;
	cel_transparency_active = cta_old;
	level_piece_id = lpi_old;
}

void scrollrt_draw_lower_2(int x, int y, int sx, int sy, int chunks, int skipChunks, int eflag)
{
	int i, j, CelSkip;
	BYTE *dst;
	MICROS *pMap;

	/// ASSERT: assert(gpBuffer);

	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
	CelSkip = 2 * skipChunks + 2;

	if (eflag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_piece_id = dPiece[x][y];
			light_table_index = dLight[x][y];
			if (level_piece_id != 0) {
				dst = &gpBuffer[sx - (BUFFER_WIDTH * 32 - 32) + PitchTbl[sy]];
				cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				for (i = 0; i < (MicroTileLen >> 1) - 1; i++) {
					if (skipChunks <= i) {
						level_cel_block = pMap->mt[2 * i + 3];
						if (level_cel_block != 0) {
							drawLowerScreen(dst);
						}
					}
					dst -= BUFFER_WIDTH * 32;
				}
				if (CelSkip < 8) {
					scrollrt_draw_clipped_dungeon_2(&gpBuffer[sx + PitchTbl[sy] - BUFFER_WIDTH * 16 * CelSkip], x, y, skipChunks, CelSkip, sx, sy, 0);
				}
			}
		}
		x++;
		y--;
		sx += 64;
		chunks--;
		pMap++;
	}

	j = chunks;
	while (j != 0) {
		j--;
		if (x >= MAXDUNX || y < 0) {
			break;
		}
		if (y < MAXDUNY && x >= 0) {
			level_piece_id = dPiece[x][y];
			light_table_index = dLight[x][y];
			if (level_piece_id != 0) {
				dst = &gpBuffer[sx - BUFFER_WIDTH * 32 + PitchTbl[sy]];
				cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				i = 0;
				while (i < (MicroTileLen >> 1) - 1) {
					if (skipChunks <= i) {
						level_cel_block = pMap->mt[2 * i + 2];
						if (level_cel_block != 0) {
							drawLowerScreen(dst);
						}
						level_cel_block = pMap->mt[2 * i + 3];
						if (level_cel_block != 0) {
							drawLowerScreen(dst + 32);
						}
					}
					i++;
					dst -= BUFFER_WIDTH * 32;
				}
				if (CelSkip < 8) {
					scrollrt_draw_clipped_dungeon_2(&gpBuffer[sx + PitchTbl[sy] - BUFFER_WIDTH * 32 * (skipChunks + 1)], x, y, skipChunks, CelSkip, sx, sy, 1);
				}
			}
		}
		x++;
		y--;
		sx += 64;
		pMap++;
	}

	if (eflag) {
		if ((DWORD)y < MAXDUNY && (DWORD)x < MAXDUNX) {
			level_piece_id = dPiece[x][y];
			light_table_index = dLight[x][y];
			if (level_piece_id != 0) {
				dst = &gpBuffer[sx - BUFFER_WIDTH * 32 + PitchTbl[sy]];
				cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				for (i = 0; i < (MicroTileLen >> 1) - 1; i++) {
					if (skipChunks <= i) {
						level_cel_block = pMap->mt[2 * i + 2];
						if (level_cel_block != 0) {
							drawLowerScreen(dst);
						}
					}
					dst -= BUFFER_WIDTH * 32;
				}
				if (CelSkip < 8) {
					scrollrt_draw_clipped_dungeon_2(&gpBuffer[sx + PitchTbl[sy] - BUFFER_WIDTH * 16 * CelSkip], x, y, skipChunks, CelSkip, sx, sy, 0);
				}
			}
		}
	}
}

void scrollrt_draw_clipped_dungeon_2(BYTE *pBuff, int sx, int sy, int skipChunks, int CelSkip, int dx, int dy, int eflag)
{
	int px, py, nCel, nMon, negMon, p;
	char bFlag, bDead, bObj, bItem, bPlr, bArch, bMap, negPlr, dd;
	DeadStruct *pDeadGuy;
	ItemStruct *pItem;
	PlayerStruct *pPlayer;
	MonsterStruct *pMonster;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

	/// ASSERT: assert((DWORD)sx < MAXDUNX);
	/// ASSERT: assert((DWORD)sy < MAXDUNY);
	bFlag = dFlags[sx][sy];
	bDead = dDead[sx][sy];
	bObj = dObject[sx][sy];
	bItem = dItem[sx][sy];
	bPlr = dPlayer[sx][sy];
	bArch = dArch[sx][sy];
	bMap = dTransVal[sx][sy];
	nMon = dMonster[sx][sy];

	/// ASSERT: assert((DWORD)(sy-1) < MAXDUNY);
	negPlr = dPlayer[sx][sy - 1];
	negMon = dMonster[sx][sy - 1];

	if (visiondebug && bFlag & BFLAG_LIT) {
		Cel2DecodeHdrOnly(pBuff, pSquareCel, 1, 64, CelSkip, 8);
	}
	if (MissilePreFlag && bFlag & BFLAG_MISSILE) {
		DrawClippedMissile(sx, sy, dx, dy, CelSkip, 8, 1);
	}
	if (light_table_index < lightmax) {
		if (bDead != 0) {
			pDeadGuy = &dead[(bDead & 0x1F) - 1];
			dd = (bDead >> 5) & 7;
			px = dx - pDeadGuy->_deadWidth2;
			pCelBuff = pDeadGuy->_deadData[dd];
			/// ASSERT: assert(pDeadGuy->_deadData[dd] != NULL);
			if (pCelBuff != NULL) {
				pFrameTable = (DWORD *)pDeadGuy->_deadData[dd];
				nCel = pDeadGuy->_deadFrame;
				if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
					if (pDeadGuy->_deadtrans != 0) {
						Cl2DecodeFrm5(px, dy, pCelBuff, nCel, pDeadGuy->_deadWidth, CelSkip, 8, pDeadGuy->_deadtrans);
					} else {
						Cl2DecodeFrm6(px, dy, pCelBuff, pDeadGuy->_deadFrame, pDeadGuy->_deadWidth, CelSkip, 8);
					}
				} else {
					// app_fatal("Clipped dead sub2: frame %d of %d, deadnum==%d", nCel, pFrameTable[0], (bDead & 0x1F) - 1);
				}
			}
		}
		if (bObj != 0) {
			DrawClippedObject(sx, sy, dx, dy, 1, CelSkip, 8);
		}
	}
	if (bItem != 0) {
		pItem = &item[bItem - 1];
		if (!pItem->_iPostDraw) {
			/// ASSERT: assert((unsigned char)bItem <= MAXITEMS);
			if ((BYTE)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if (pCelBuff != NULL) {
					pFrameTable = (DWORD *)pCelBuff;
					nCel = pItem->_iAnimFrame;
					if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if (bItem - 1 == pcursitem) {
							CelDrawHdrClrHL(181, px, dy, pCelBuff, nCel, pItem->_iAnimWidth, CelSkip, 8);
						}
						Cel2DecodeHdrLight(px, dy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth, CelSkip, 8);
					} else {
						/*
						app_fatal(
							"Draw Clipped \"%s\" Item 3: frame %d of %d, item type==%d",
							pItem->_iIName,
							nCel,
							pFrameTable[0],
							pItem->_itype);
						*/
					}
				} else {
					// app_fatal("Draw Item \"%s\" Clipped 3: NULL Cel Buffer", pItem->_iIName);
				}
			}
		}
	}
	if (bFlag & BFLAG_PLAYERLR) {
		p = -(negPlr + 1);
		if ((DWORD)p < MAX_PLRS) {
			pPlayer = &plr[p];
			px = dx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
			py = dy + pPlayer->_pyoff;
			DrawClippedPlayer(p, sx, sy - 1, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth, CelSkip, 8);
			if (eflag && pPlayer->_peflag != 0) {
				if (pPlayer->_peflag == 2) {
					scrollrt_draw_clipped_e_flag_2(pBuff - (BUFFER_WIDTH * 16 + 96), sx - 2, sy + 1, skipChunks, CelSkip, dx - 96, dy - 16);
				}
				scrollrt_draw_clipped_e_flag_2(pBuff - 64, sx - 1, sy + 1, skipChunks, CelSkip, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player clipped: tried to draw illegal player %d", p);
		}
	}
	if (bFlag & BFLAG_MONSTLR && (bFlag & BFLAG_LIT || plr[myplr]._pInfraFlag) && negMon < 0) {
		draw_monster_num = -(negMon + 1);
		if ((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if (!(pMonster->_mFlags & 1)) {
				if (pMonster->MType != NULL) {
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
					py = dy + pMonster->_myoff;
					if (draw_monster_num == pcursmonst) {
						Cl2DecodeClrHL(233, px, py, pMonster->_mAnimData, pMonster->_mAnimFrame, pMonster->MType->width, CelSkip, 8);
					}
					DrawClippedMonster(sx, sy, px, py, draw_monster_num, CelSkip, 8);
					if (eflag && !pMonster->_meflag) {
						scrollrt_draw_clipped_e_flag_2(pBuff - 64, sx - 1, sy + 1, skipChunks, CelSkip, dx - 64, dy);
					}
				} else {
					// app_fatal("Draw Monster \"%s\" Clipped: uninitialized monster", pMonster->mName);
				}
			}
		} else {
			// app_fatal("Draw Monster Clipped: tried to draw illegal monster %d", draw_monster_num);
		}
	}
	if (bFlag & BFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(sx, sy, dx, dy, CelSkip, 8, 1);
	}
	if (bPlr > 0) {
		p = bPlr - 1;
		if ((DWORD)p < MAX_PLRS) {
			pPlayer = &plr[p];
			px = dx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
			py = dy + pPlayer->_pyoff;
			DrawClippedPlayer(p, sx, sy, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth, CelSkip, 8);
			if (eflag && pPlayer->_peflag != 0) {
				if (pPlayer->_peflag == 2) {
					scrollrt_draw_clipped_e_flag_2(pBuff - (BUFFER_WIDTH * 16 + 96), sx - 2, sy + 1, skipChunks, CelSkip, dx - 96, dy - 16);
				}
				scrollrt_draw_clipped_e_flag_2(pBuff - 64, sx - 1, sy + 1, skipChunks, CelSkip, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player clipped: tried to draw illegal player %d", p);
		}
	}
	if (nMon > 0 && (bFlag & BFLAG_LIT || plr[myplr]._pInfraFlag)) {
		draw_monster_num = nMon - 1;
		if ((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if (!(pMonster->_mFlags & 1)) {
				if (pMonster->MType != NULL) {
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
					py = dy + pMonster->_myoff;
					if (draw_monster_num == pcursmonst) {
						Cl2DecodeClrHL(233, px, py, pMonster->_mAnimData, pMonster->_mAnimFrame, pMonster->MType->width, CelSkip, 8);
					}
					DrawClippedMonster(sx, sy, px, py, draw_monster_num, CelSkip, 8);
					if (eflag && !pMonster->_meflag) {
						scrollrt_draw_clipped_e_flag_2(pBuff - 64, sx - 1, sy + 1, skipChunks, CelSkip, dx - 64, dy);
					}
				} else {
					// app_fatal("Draw Monster \"%s\" Clipped: uninitialized monster", pMonster->mName);
				}
			}
		} else {
			// app_fatal("Draw Monster Clipped: tried to draw illegal monster %d", draw_monster_num);
		}
	}
	if (bFlag & BFLAG_MISSILE) {
		DrawClippedMissile(sx, sy, dx, dy, CelSkip, 8, 0);
	}
	if (bObj != 0 && light_table_index < lightmax) {
		DrawClippedObject(sx, sy, dx, dy, 0, CelSkip, 8);
	}
	if (bItem != 0) {
		pItem = &item[bItem - 1];
		if (pItem->_iPostDraw) {
			/// ASSERT: assert((unsigned char)bItem <= MAXITEMS);
			if ((BYTE)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if (pCelBuff != NULL) {
					pFrameTable = (DWORD *)pCelBuff;
					nCel = pItem->_iAnimFrame;
					if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if (bItem - 1 == pcursitem) {
							CelDrawHdrClrHL(181, px, dy, pCelBuff, nCel, pItem->_iAnimWidth, CelSkip, 8);
						}
						Cel2DecodeHdrLight(px, dy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth, CelSkip, 8);
					} else {
						/*
						app_fatal(
							"Draw Clipped \"%s\" Item 4: frame %d of %d, item type==%d",
							pItem->_iIName,
							nCel,
							pFrameTable[0],
							pItem->_itype);
						*/
					}
				} else {
					// app_fatal("Draw Item \"%s\" Clipped 4: NULL Cel Buffer", pItem->_iIName);
				}
			}
		}
	}
	if (bArch != 0) {
		cel_transparency_active = TransList[bMap];
		Cel2DecodeLightTrans(pBuff, pSpecialCels, bArch, 64, CelSkip, 8);
	}
}

void scrollrt_draw_clipped_e_flag_2(BYTE *pBuff, int x, int y, int skipChunks, int CelSkip, int sx, int sy)
{
	int lti_old, cta_old, lpi_old;
	BYTE *dst;
	MICROS *pMap;

	lti_old = light_table_index;
	cta_old = cel_transparency_active;
	lpi_old = level_piece_id;

	level_piece_id = dPiece[x][y];
	light_table_index = dLight[x][y];
	dst = &pBuff[BUFFER_WIDTH * 32 * skipChunks];
	cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];

	switch (skipChunks) {
	case 0:
		level_cel_block = pMap->mt[2];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[3];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
	case 1:
		dst -= BUFFER_WIDTH * 32;
		level_cel_block = pMap->mt[4];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[5];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
	case 2:
		dst -= BUFFER_WIDTH * 32;
		level_cel_block = pMap->mt[6];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[7];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
	case 3:
		dst -= BUFFER_WIDTH * 32;
		level_cel_block = pMap->mt[8];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[9];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
		break;
	}

	if (CelSkip < 8) {
		scrollrt_draw_clipped_dungeon_2(pBuff, x, y, skipChunks, CelSkip, sx, sy, 0);
	}

	light_table_index = lti_old;
	cel_transparency_active = cta_old;
	level_piece_id = lpi_old;
}

void scrollrt_draw_upper(int x, int y, int sx, int sy, int chunks, int capChunks, int eflag)
{
	int i, j, CelCap;
	BYTE *dst;
	MICROS *pMap;

	/// ASSERT: assert(gpBuffer);

	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];
	CelCap = 2 * capChunks + 2;
	if (CelCap > 8) {
		CelCap = 8;
	}

	if (eflag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_piece_id = dPiece[x][y];
			light_table_index = dLight[x][y];
			if (level_piece_id != 0) {
				dst = &gpBuffer[sx + 32 + PitchTbl[sy]];
				cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				if (capChunks >= 0) {
					level_cel_block = pMap->mt[1];
					if (level_cel_block != 0) {
						arch_draw_type = 2;
						drawUpperScreen(dst);
						arch_draw_type = 0;
					}
				}
				dst -= BUFFER_WIDTH * 32;
				if (capChunks >= 1) {
					level_cel_block = pMap->mt[3];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
				}
				dst -= BUFFER_WIDTH * 32;
				if (capChunks >= 2) {
					level_cel_block = pMap->mt[5];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
				}
				dst -= BUFFER_WIDTH * 32;
				if (capChunks >= 3) {
					level_cel_block = pMap->mt[7];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
				}
				scrollrt_draw_dungeon(&gpBuffer[sx + PitchTbl[sy]], x, y, capChunks, CelCap, sx, sy, 0);
			} else {
				world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
			}
		}
		x++;
		y--;
		sx += 64;
		chunks--;
		pMap++;
	}

	for (j = 0; j < chunks; j++) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_piece_id = dPiece[x][y];
			light_table_index = dLight[x][y];
			if (level_piece_id != 0) {
				dst = &gpBuffer[sx + PitchTbl[sy]];
				cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				arch_draw_type = 1;
				level_cel_block = pMap->mt[0];
				if (level_cel_block != 0) {
					drawUpperScreen(dst);
				}
				arch_draw_type = 2;
				level_cel_block = pMap->mt[1];
				if (level_cel_block != 0) {
					drawUpperScreen(dst + 32);
				}
				arch_draw_type = 0;
				for (i = 1; i < (MicroTileLen >> 1) - 1; i++) {
					dst -= BUFFER_WIDTH * 32;
					if (capChunks >= i) {
						level_cel_block = pMap->mt[2 * i];
						if (level_cel_block != 0) {
							drawUpperScreen(dst);
						}
						level_cel_block = pMap->mt[2 * i + 1];
						if (level_cel_block != 0) {
							drawUpperScreen(dst + 32);
						}
					}
				}
				scrollrt_draw_dungeon(&gpBuffer[sx + PitchTbl[sy]], x, y, capChunks, CelCap, sx, sy, 1);
			} else {
				world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
			}
		}
		x++;
		y--;
		sx += 64;
		pMap++;
	}

	if (eflag) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_piece_id = dPiece[x][y];
			light_table_index = dLight[x][y];
			if (level_piece_id != 0) {
				dst = &gpBuffer[sx + PitchTbl[sy]];
				cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				arch_draw_type = 1;
				if (capChunks >= 0) {
					level_cel_block = pMap->mt[0];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
				}
				arch_draw_type = 0;
				dst -= BUFFER_WIDTH * 32;
				if (capChunks >= 1) {
					level_cel_block = pMap->mt[2];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
				}
				dst -= BUFFER_WIDTH * 32;
				if (capChunks >= 2) {
					level_cel_block = pMap->mt[4];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
				}
				dst -= BUFFER_WIDTH * 32;
				if (capChunks >= 3) {
					level_cel_block = pMap->mt[6];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
				}
				scrollrt_draw_dungeon(&gpBuffer[sx + PitchTbl[sy]], x, y, capChunks, CelCap, sx, sy, 0);
			} else {
				world_draw_black_tile(&gpBuffer[sx + PitchTbl[sy]]);
			}
		}
	}
}

void scrollrt_draw_dungeon(BYTE *pBuff, int sx, int sy, int capChunks, int CelCap, int dx, int dy, int eflag)
{
	int px, py, nCel, nMon, negMon, p, tx, ty;
	char bFlag, bDead, bObj, bItem, bPlr, bArch, bMap, negPlr, dd;
	DeadStruct *pDeadGuy;
	ItemStruct *pItem;
	PlayerStruct *pPlayer;
	MonsterStruct *pMonster;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

	/// ASSERT: assert((DWORD)sx < MAXDUNX);
	/// ASSERT: assert((DWORD)sy < MAXDUNY);
	bFlag = dFlags[sx][sy];
	bDead = dDead[sx][sy];
	bObj = dObject[sx][sy];
	bItem = dItem[sx][sy];
	bPlr = dPlayer[sx][sy];
	bArch = dArch[sx][sy];
	bMap = dTransVal[sx][sy];
	nMon = dMonster[sx][sy];

	/// ASSERT: assert((DWORD)(sy-1) < MAXDUNY);
	negPlr = dPlayer[sx][sy - 1];
	negMon = dMonster[sx][sy - 1];

	if (visiondebug && bFlag & BFLAG_LIT) {
		CelDecodeHdrOnly(pBuff, pSquareCel, 1, 64, 0, CelCap);
	}
	tx = dx - 96;
	ty = dy - 16;

	if (MissilePreFlag && bFlag & BFLAG_MISSILE) {
		DrawMissile(sx, sy, dx, dy, 0, CelCap, 1);
	}

	if (light_table_index < lightmax) {
		if (bDead != 0) {
			pDeadGuy = &dead[(bDead & 0x1F) - 1];
			dd = (bDead >> 5) & 7;
			px = dx - pDeadGuy->_deadWidth2;
			pCelBuff = pDeadGuy->_deadData[dd];
			/// ASSERT: assert(pDeadGuy->_deadData[dd] != NULL);
			if (pCelBuff != NULL) {
				pFrameTable = (DWORD *)pDeadGuy->_deadData[dd];
				nCel = pDeadGuy->_deadFrame;
				if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
					if (pDeadGuy->_deadtrans != 0) {
						Cl2DecodeFrm3(px, dy, pCelBuff, nCel, pDeadGuy->_deadWidth, 0, CelCap, pDeadGuy->_deadtrans);
					} else {
						Cl2DecodeLightTbl(px, dy, pCelBuff, pDeadGuy->_deadFrame, pDeadGuy->_deadWidth, 0, CelCap);
					}
				} else {
					// app_fatal("Unclipped dead: frame %d of %d, deadnum==%d", nCel, pFrameTable[0], (bDead & 0x1F) - 1);
				}
			}
		}
		if (bObj != 0) {
			DrawObject(sx, sy, dx, dy, 1, 0, CelCap);
		}
	}
	if (bItem != 0) {
		pItem = &item[bItem - 1];
		if (!pItem->_iPostDraw) {
			/// ASSERT: assert((unsigned char)bItem <= MAXITEMS);
			if ((BYTE)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if (pCelBuff != NULL) {
					pFrameTable = (DWORD *)pCelBuff;
					nCel = pItem->_iAnimFrame;
					if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if (bItem - 1 == pcursitem) {
							CelDecodeClr(181, px, dy, pCelBuff, nCel, pItem->_iAnimWidth, 0, CelCap);
						}
						CelDecodeHdrLightOnly(px, dy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth, 0, CelCap);
					} else {
						// app_fatal("Draw \"%s\" Item 1: frame %d of %d, item type==%d", pItem->_iIName, nCel, pFrameTable[0], pItem->_itype);
					}
				} else {
					// app_fatal("Draw Item \"%s\" 1: NULL Cel Buffer", pItem->_iIName);
				}
			}
		}
	}
	if (bFlag & BFLAG_PLAYERLR) {
		p = -(negPlr + 1);
		if ((DWORD)p < MAX_PLRS) {
			pPlayer = &plr[p];
			px = dx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
			py = dy + pPlayer->_pyoff;
			DrawPlayer(p, sx, sy - 1, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth, 0, CelCap);
			if (eflag && pPlayer->_peflag != 0) {
				if (pPlayer->_peflag == 2) {
					scrollrt_draw_e_flag(pBuff - (BUFFER_WIDTH * 16 + 96), sx - 2, sy + 1, capChunks, CelCap, tx, ty);
				}
				scrollrt_draw_e_flag(pBuff - 64, sx - 1, sy + 1, capChunks, CelCap, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player: tried to draw illegal player %d", p);
		}
	}
	if (bFlag & BFLAG_MONSTLR && (bFlag & BFLAG_LIT || plr[myplr]._pInfraFlag) && negMon < 0) {
		draw_monster_num = -(negMon + 1);
		if ((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if (!(pMonster->_mFlags & 1)) {
				if (pMonster->MType != NULL) {
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
					py = dy + pMonster->_myoff;
					if (draw_monster_num == pcursmonst) {
						Cl2DecodeFrm2(233, px, py, pMonster->_mAnimData, pMonster->_mAnimFrame, pMonster->MType->width, 0, CelCap);
					}
					DrawMonster(sx, sy, px, py, draw_monster_num, 0, CelCap);
					if (eflag && !pMonster->_meflag) {
						scrollrt_draw_e_flag(pBuff - 64, sx - 1, sy + 1, capChunks, CelCap, dx - 64, dy);
					}
				} else {
					// app_fatal("Draw Monster \"%s\": uninitialized monster", pMonster->mName);
				}
			}
		} else {
			// app_fatal("Draw Monster: tried to draw illegal monster %d", draw_monster_num);
		}
	}
	if (bFlag & BFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(sx, sy, dx, dy, 0, CelCap, 0);
	}
	if (bPlr > 0) {
		p = bPlr - 1;
		if ((DWORD)p < MAX_PLRS) {
			pPlayer = &plr[p];
			px = dx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
			py = dy + pPlayer->_pyoff;
			DrawPlayer(p, sx, sy, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth, 0, CelCap);
			if (eflag && pPlayer->_peflag != 0) {
				if (pPlayer->_peflag == 2) {
					scrollrt_draw_e_flag(pBuff - (BUFFER_WIDTH * 16 + 96), sx - 2, sy + 1, capChunks, CelCap, dx - 96, dy - 16);
				}
				scrollrt_draw_e_flag(pBuff - 64, sx - 1, sy + 1, capChunks, CelCap, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player: tried to draw illegal player %d", p);
		}
	}
	if (nMon > 0 && (bFlag & BFLAG_LIT || plr[myplr]._pInfraFlag)) {
		draw_monster_num = nMon - 1;
		if ((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if (!(pMonster->_mFlags & 1)) {
				if (pMonster->MType != NULL) {
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
					py = dy + pMonster->_myoff;
					if (draw_monster_num == pcursmonst) {
						Cl2DecodeFrm2(233, px, py, pMonster->_mAnimData, pMonster->_mAnimFrame, pMonster->MType->width, 0, CelCap);
					}
					DrawMonster(sx, sy, px, py, draw_monster_num, 0, CelCap);
					if (eflag && !pMonster->_meflag) {
						scrollrt_draw_e_flag(pBuff - 64, sx - 1, sy + 1, capChunks, CelCap, dx - 64, dy);
					}
				} else {
					// app_fatal("Draw Monster \"%s\": uninitialized monster", pMonster->mName);
				}
			}
		} else {
			// app_fatal("Draw Monster: tried to draw illegal monster %d", draw_monster_num);
		}
	}
	if (bFlag & BFLAG_MISSILE) {
		DrawMissile(sx, sy, dx, dy, 0, CelCap, 0);
	}
	if (bObj != 0 && light_table_index < lightmax) {
		DrawObject(sx, sy, dx, dy, 0, 0, CelCap);
	}
	if (bItem != 0) {
		pItem = &item[bItem - 1];
		if (pItem->_iPostDraw) {
			/// ASSERT: assert((unsigned char)bItem <= MAXITEMS);
			if ((BYTE)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if (pCelBuff != NULL) {
					pFrameTable = (DWORD *)pCelBuff;
					nCel = pItem->_iAnimFrame;
					if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if (bItem - 1 == pcursitem) {
							CelDecodeClr(181, px, dy, pCelBuff, nCel, pItem->_iAnimWidth, 0, CelCap);
						}
						CelDecodeHdrLightOnly(px, dy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth, 0, CelCap);
					} else {
						// app_fatal("Draw \"%s\" Item 2: frame %d of %d, item type==%d", pItem->_iIName, nCel, pFrameTable[0], pItem->_itype);
					}
				} else {
					// app_fatal("Draw Item \"%s\" 2: NULL Cel Buffer", pItem->_iIName);
				}
			}
		}
	}
	if (bArch != 0) {
		cel_transparency_active = TransList[bMap];
		CelDecodeHdrLightTrans(pBuff, pSpecialCels, bArch, 64, 0, CelCap);
	}
}

void DrawMonster(int x, int y, int mx, int my, int m, int CelSkip, int CelCap)
{
	int nCel;
	char trans;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

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
	pFrameTable = (DWORD *)pCelBuff;
	if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
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
			pFrameTable[0]);
		*/
		return;
	}

	if (!(dFlags[x][y] & BFLAG_LIT)) {
		Cl2DecodeFrm3(mx, my, monster[m]._mAnimData, monster[m]._mAnimFrame, monster[m].MType->width, CelSkip, CelCap, 1);
	} else {
		trans = 0;
		if (monster[m]._uniqtype)
			trans = monster[m]._uniqtrans + 4;
		if (monster[m]._mmode == MM_STONE)
			trans = 2;
		if (plr[myplr]._pInfraFlag && light_table_index > 8)
			trans = 1;
		if (trans)
			Cl2DecodeFrm3(mx, my, monster[m]._mAnimData, monster[m]._mAnimFrame, monster[m].MType->width, CelSkip, CelCap, trans);
		else
			Cl2DecodeLightTbl(mx, my, monster[m]._mAnimData, monster[m]._mAnimFrame, monster[m].MType->width, CelSkip, CelCap);
	}
}

void DrawObject(int x, int y, int ox, int oy, BOOL pre, int CelSkip, int CelCap)
{
	int sx, sy, xx, yy, nCel;
	char bv;
	BYTE *pCelBuff;
	DWORD *pFrameTable;

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
		CelSkip = 0;
		CelCap = 8;
	}

	/// ASSERT: assert((unsigned char)bv < MAXOBJECTS);
	if ((BYTE)bv >= MAXOBJECTS)
		return;

	pCelBuff = object[bv]._oAnimData;
	if (!pCelBuff) {
		// app_fatal("Draw Object type %d: NULL Cel Buffer", object[bv]._otype);
		return;
	}

	nCel = object[bv]._oAnimFrame;
	pFrameTable = (DWORD *)pCelBuff;
	if (nCel < 1 || pFrameTable[0] > 50 || nCel > (int)pFrameTable[0]) {
		// app_fatal("Draw Object: frame %d of %d, object type==%d", nCel, pFrameTable[0], object[bv]._otype);
		return;
	}

	if (bv == pcursobj)
		CelDecodeClr(194, sx, sy, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, CelSkip, CelCap);
	if (object[bv]._oLight) {
		CelDecodeHdrLightOnly(sx, sy, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, CelSkip, CelCap);
	} else {
		/// ASSERT: assert(object[bv]._oAnimData);
		if (object[bv]._oAnimData) // BUGFIX: _oAnimData was already checked, this is redundant
			CelDrawHdrOnly(sx, sy, object[bv]._oAnimData, object[bv]._oAnimFrame, object[bv]._oAnimWidth, CelSkip, CelCap);
	}
}

void scrollrt_draw_e_flag(BYTE *pBuff, int x, int y, int capChunks, int CelCap, int sx, int sy)
{
	int i, lti_old, cta_old, lpi_old;
	BYTE *dst;
	MICROS *pMap;

	lti_old = light_table_index;
	cta_old = cel_transparency_active;
	lpi_old = level_piece_id;

	level_piece_id = dPiece[x][y];
	light_table_index = dLight[x][y];
	dst = pBuff;
	cel_transparency_active = (BYTE)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
	pMap = &dpiece_defs_map_1[IsometricCoord(x, y)];

	arch_draw_type = 1;
	level_cel_block = pMap->mt[0];
	if (level_cel_block != 0) {
		drawUpperScreen(dst);
	}
	arch_draw_type = 2;
	level_cel_block = pMap->mt[1];
	if (level_cel_block != 0) {
		drawUpperScreen(dst + 32);
	}

	arch_draw_type = 0;
	for (i = 1; i < (MicroTileLen >> 1) - 1; i++) {
		dst -= BUFFER_WIDTH * 32;
		if (capChunks >= i) {
			level_cel_block = pMap->mt[2 * i];
			if (level_cel_block != 0) {
				drawUpperScreen(dst);
			}
			level_cel_block = pMap->mt[2 * i + 1];
			if (level_cel_block != 0) {
				drawUpperScreen(dst + 32);
			}
		}
	}

	scrollrt_draw_dungeon(pBuff, x, y, capChunks, CelCap, sx, sy, 0);

	light_table_index = lti_old;
	cel_transparency_active = cta_old;
	level_piece_id = lpi_old;
}

void DrawZoom(int x, int y)
{
	int i, sx, sy, chunks, blocks;
	int wdt, nSrcOff, nDstOff;

	scr_pix_width = ZOOM_WIDTH;
	scr_pix_height = 192;
	dword_5C2FF8 = ZOOM_WIDTH / 64;
	dword_5C2FFC = 192 / 32;

	sx = ScrollInfo._sxoff + 64;
	sy = ScrollInfo._syoff + 143;
	x -= 6;
	y--;
	chunks = 6;
	blocks = 3;

	switch (ScrollInfo._sdir) {
	case SDIR_NONE:
		break;
	case SDIR_NE:
		chunks++;
	case SDIR_N:
		sy -= 32;
		x--;
		y--;
		blocks++;
		break;
	case SDIR_SE:
		blocks++;
	case SDIR_E:
		chunks++;
		break;
	case SDIR_S:
		blocks++;
		break;
	case SDIR_SW:
		blocks++;
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

	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[-17 + SCREEN_Y]];
	for (i = 0; i < 4; i++) {
		scrollrt_draw_upper(x, y, sx, sy, chunks, i, 0);
		y++;
		sx -= 32;
		sy += 16;
		scrollrt_draw_upper(x, y, sx, sy, chunks, i, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	/// ASSERT: assert(gpBuffer);
	gpBufEnd = &gpBuffer[PitchTbl[160 + SCREEN_Y]];
	for (i = 0; i < blocks; i++) {
		scrollrt_draw_lower(x, y, sx, sy, chunks, 0);
		y++;
		sx -= 32;
		sy += 16;
		scrollrt_draw_lower(x, y, sx, sy, chunks, 1);
		x++;
		sx += 32;
		sy += 16;
	}
	arch_draw_type = 0;
	for (i = 0; i < 4; i++) {
		scrollrt_draw_lower_2(x, y, sx, sy, chunks, i, 0);
		y++;
		sx -= 32;
		sy += 16;
		scrollrt_draw_lower_2(x, y, sx, sy, chunks, i, 1);
		x++;
		sx += 32;
		sy += 16;
	}

	if (chrflag || questlog) {
		nSrcOff = SCREENXY(112, 159);
		nDstOff = SCREENXY(320, 350);
		wdt = (SCREEN_WIDTH - 320) / 2;
	} else if (invflag || sbookflag) {
		nSrcOff = SCREENXY(112, 159);
		nDstOff = SCREENXY(0, 350);
		wdt = (SCREEN_WIDTH - 320) / 2;
	} else {
		nSrcOff = SCREENXY(32, 159);
		nDstOff = SCREENXY(0, 350);
		wdt = SCREEN_WIDTH / 2;
	}

	/// ASSERT: assert(gpBuffer);

#ifdef USE_ASM
	__asm {
		mov		esi, gpBuffer
		mov		edx, nDstOff
		mov		edi, esi
		mov		ecx, nSrcOff
		add		edi, edx
		add		esi, ecx
		mov		ebx, edi
		add		ebx, BUFFER_WIDTH
		mov		edx, 176
	label1:
		mov		ecx, wdt
	label2:
		mov		al, [esi]
		inc		esi
		mov		ah, al
		mov		[edi], ax
		mov		[ebx], ax
		add		edi, 2
		add		ebx, 2
		dec		ecx
		jnz		label2
		mov		eax, BUFFER_WIDTH
		add		eax, wdt
		sub		esi, eax
		add		eax, eax
		sub		ebx, eax
		sub		edi, eax
		dec		edx
		jnz		label1
	}
#else
	int hgt;
	BYTE *src, *dst1, *dst2;

	src = &gpBuffer[nSrcOff];
	dst1 = &gpBuffer[nDstOff];
	dst2 = &gpBuffer[nDstOff + BUFFER_WIDTH];

	for (hgt = 176; hgt != 0; hgt--, src -= BUFFER_WIDTH + wdt, dst1 -= 2 * (BUFFER_WIDTH + wdt), dst2 -= 2 * (BUFFER_WIDTH + wdt)) {
		for (i = wdt; i != 0; i--) {
			*dst1++ = *src;
			*dst1++ = *src;
			*dst2++ = *src;
			*dst2++ = *src;
			src++;
		}
	}
#endif
}

void ClearScreenBuffer()
{
	lock_buf(3);

	/// ASSERT: assert(gpBuffer);

#ifdef USE_ASM
	__asm {
		mov		edi, gpBuffer
		add		edi, SCREENXY(0, 0)
		mov		edx, SCREEN_HEIGHT
		xor		eax, eax
	zeroline:
		mov		ecx, SCREEN_WIDTH / 4
		rep stosd
		add		edi, BUFFER_WIDTH - SCREEN_WIDTH
		dec		edx
		jnz		zeroline
	}
#else
	int i;
	BYTE *dst;

	dst = &gpBuffer[SCREENXY(0, 0)];

	for (i = 0; i < SCREEN_HEIGHT; i++, dst += BUFFER_WIDTH) {
		memset(dst, 0, SCREEN_WIDTH);
	}
#endif

	unlock_buf(3);
}

#ifdef _DEBUG
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

void EnableFrameCount()
{
	frameflag = frameflag == 0;
	framestart = GetTickCount();
}
#endif

void scrollrt_draw_game_screen(BOOL draw_cursor)
{
	int hgt;

	if (drawpanflag == 255) {
		drawpanflag = 0;
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

void scrollrt_draw_cursor_back_buffer()
{
	int i;
	BYTE *src, *dst;

	if (sgdwCursWdt == 0) {
		return;
	}

	/// ASSERT: assert(gpBuffer);
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

void scrollrt_draw_cursor_item()
{
	int i, mx, my, col;
	BYTE *src, *dst;

	/// ASSERT: assert(! sgdwCursWdt);

	if (pcurs <= 0 || cursW == 0 || cursH == 0) {
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

	/// ASSERT: assert(sgdwCursWdt * sgdwCursHgt < sizeof sgSaveBack);
	/// ASSERT: assert(gpBuffer);
	dst = sgSaveBack;
	src = &gpBuffer[SCREENXY(sgdwCursX, sgdwCursY)];

	for (i = sgdwCursHgt; i != 0; i--, dst += sgdwCursWdt, src += BUFFER_WIDTH) {
		memcpy(dst, src, sgdwCursWdt);
	}

	mx++;
	my++;
	gpBufEnd = &gpBuffer[PitchTbl[SCREEN_HEIGHT + SCREEN_Y] - cursW - 2];

	if (pcurs >= CURSOR_FIRSTITEM) {
		col = PAL16_YELLOW + 5;
		if (plr[myplr].HoldItem._iMagical != 0) {
			col = PAL16_BLUE + 5;
		}
		if (!plr[myplr].HoldItem._iStatFlag) {
			col = PAL16_RED + 5;
		}
		CelDrawHdrClrHL(col, mx + SCREEN_X, my + cursH + SCREEN_Y - 1, pCursCels, pcurs, cursW, 0, 8);
		if (col != PAL16_RED + 5) {
			Cel2DrawHdrOnly(mx + SCREEN_X, my + cursH + SCREEN_Y - 1, pCursCels, pcurs, cursW, 0, 8);
		} else {
			Cel2DrawHdrLightRed(mx + SCREEN_X, my + cursH + SCREEN_Y - 1, pCursCels, pcurs, cursW, 0, 8, 1);
		}
	} else {
		Cel2DrawHdrOnly(mx + SCREEN_X, my + cursH + SCREEN_Y - 1, pCursCels, pcurs, cursW, 0, 8);
	}
}

void DrawMain(int dwHgt, BOOL draw_desc, BOOL draw_hp, BOOL draw_mana, BOOL draw_sbar, BOOL draw_btn)
{
	int ysize;
	DWORD dwTicks;
	BOOL retry;
	HRESULT hDDVal;

	ysize = dwHgt;

	if (!gbActive || lpDDSPrimary == NULL) {
		return;
	}

#ifdef __cplusplus
	if (lpDDSPrimary->IsLost() == DDERR_SURFACELOST) {
		if (lpDDSPrimary->Restore() != DD_OK) {
			return;
		}
#else
	if (lpDDSPrimary->lpVtbl->IsLost(lpDDSPrimary) == DDERR_SURFACELOST) {
		if (lpDDSPrimary->lpVtbl->Restore(lpDDSPrimary) != DD_OK) {
			return;
		}
#endif
		ResetPal();
		ysize = SCREEN_HEIGHT;
	}

	if (lpDDSBackBuf == NULL) {
		retry = TRUE;
		dwTicks = GetTickCount();
		while (1) {
			DDS_desc.dwSize = sizeof(DDS_desc);
#ifdef __cplusplus
			hDDVal = lpDDSPrimary->Lock(NULL, &DDS_desc, DDLOCK_WRITEONLY | DDLOCK_WAIT, NULL);
#else
			hDDVal = lpDDSPrimary->lpVtbl->Lock(lpDDSPrimary, NULL, &DDS_desc, DDLOCK_WRITEONLY | DDLOCK_WAIT, NULL);
#endif
			if (hDDVal == DD_OK) {
				break;
			}
			if (dwTicks - GetTickCount() > 5000) {
				break;
			}
			Sleep(1);
			if (hDDVal == DDERR_SURFACELOST) {
				return;
			}
			if (hDDVal != DDERR_WASSTILLDRAWING && hDDVal != DDERR_SURFACEBUSY) {
				if (!retry || hDDVal != DDERR_GENERIC) {
					break;
				}
				retry = FALSE;
				dx_reinit();
				ysize = SCREEN_HEIGHT;
				dwTicks = GetTickCount();
			}
		}
		if (hDDVal == DDERR_SURFACELOST
		    || hDDVal == DDERR_WASSTILLDRAWING
		    || hDDVal == DDERR_SURFACEBUSY) {
			return;
		}
		if (hDDVal != DD_OK) {
			DDErrMsg(hDDVal, 3707, "C:\\Src\\Diablo\\Source\\SCROLLRT.CPP");
		}
	}

	/// ASSERT: assert(ysize >= 0 && ysize <= 480); // SCREEN_HEIGHT

	if (ysize > 0) {
		DoBlitScreen(0, 0, SCREEN_WIDTH, ysize);
	}
	if (ysize < SCREEN_HEIGHT) {
		if (draw_sbar) {
			DoBlitScreen(204, 357, 232, 28);
		}
		if (draw_desc) {
			DoBlitScreen(176, 398, 288, 60);
		}
		if (draw_mana) {
			DoBlitScreen(460, 352, 88, 72);
			DoBlitScreen(564, 416, 56, 56);
		}
		if (draw_hp) {
			DoBlitScreen(96, 352, 88, 72);
		}
		if (draw_btn) {
			DoBlitScreen(8, 357, 72, 119);
			DoBlitScreen(556, 357, 72, 48);
			if (gbMaxPlayers > 1) {
				DoBlitScreen(84, 443, 36, 32);
				DoBlitScreen(524, 443, 36, 32);
			}
		}
		if (sgdwCursWdtOld != 0) {
			DoBlitScreen(sgdwCursXOld, sgdwCursYOld, sgdwCursWdtOld, sgdwCursHgtOld);
		}
		if (sgdwCursWdt != 0) {
			DoBlitScreen(sgdwCursX, sgdwCursY, sgdwCursWdt, sgdwCursHgt);
		}
	}

	if (lpDDSBackBuf == NULL) {
#ifdef __cplusplus
		hDDVal = lpDDSPrimary->Unlock(NULL);
#else
		hDDVal = lpDDSPrimary->lpVtbl->Unlock(lpDDSPrimary, NULL);
#endif
		if (hDDVal != DDERR_SURFACELOST && hDDVal != DD_OK) {
			DDErrMsg(hDDVal, 3779, "C:\\Src\\Diablo\\Source\\SCROLLRT.CPP");
		}
	}

#ifdef _DEBUG
	DrawFPS();
#endif
}

#ifdef _DEBUG
void DrawFPS()
{
	DWORD tc, frames;
	char String[12];
	HDC hdc;

	if (frameflag && gbActive) {
		frameend++;
		tc = GetTickCount();
		frames = tc - framestart;
		if (tc - framestart >= 1000) {
			framestart = tc;
			framerate = 1000 * frameend / frames;
			frameend = 0;
		}
		if (framerate > 99)
			framerate = 99;
		wsprintf(String, "%2d", framerate);
#ifdef __cplusplus
		if (!lpDDSPrimary->GetDC(&hdc)) {
			TextOut(hdc, 0, 400, String, strlen(String));
			lpDDSPrimary->ReleaseDC(hdc);
		}
#else
		if (!lpDDSPrimary->lpVtbl->GetDC(lpDDSPrimary, &hdc)) {
			TextOut(hdc, 0, 400, String, strlen(String));
			lpDDSPrimary->lpVtbl->ReleaseDC(lpDDSPrimary, hdc);
		}
#endif
	}
}
#endif

void DoBlitScreen(DWORD dwX, DWORD dwY, DWORD dwWdt, DWORD dwHgt)
{
	int nSrcOff, nDstOff, nSrcWdt, nDstWdt;
	DWORD dwTicks;
	HRESULT hDDVal;
	RECT SrcRect;

	/// ASSERT: assert(! (dwX & 3));
	/// ASSERT: assert(! (dwWdt & 3));

	if (lpDDSBackBuf != NULL) {
		SrcRect.left = dwX + SCREEN_X;
		SrcRect.top = dwY + SCREEN_Y;
		SrcRect.right = SrcRect.left + dwWdt - 1;
		SrcRect.bottom = SrcRect.top + dwHgt - 1;
		/// ASSERT: assert(! gpBuffer);
		dwTicks = GetTickCount();
		while (1) {
#ifdef __cplusplus
			hDDVal = lpDDSPrimary->BltFast(dwX, dwY, lpDDSBackBuf, &SrcRect, DDBLTFAST_WAIT);
#else
			hDDVal = lpDDSPrimary->lpVtbl->BltFast(lpDDSPrimary, dwX, dwY, lpDDSBackBuf, &SrcRect, DDBLTFAST_WAIT);
#endif
			if (hDDVal == DD_OK) {
				break;
			}
			if (dwTicks - GetTickCount() > 5000) {
				break;
			}
			Sleep(1);
			if (hDDVal == DDERR_SURFACELOST) {
				return;
			}
			if (hDDVal != DDERR_WASSTILLDRAWING && hDDVal != DDERR_SURFACEBUSY) {
				break;
			}
		}
		if (hDDVal != DDERR_SURFACELOST
		    && hDDVal != DDERR_WASSTILLDRAWING
		    && hDDVal != DDERR_SURFACEBUSY
		    && hDDVal != DD_OK) {
			DDErrMsg(hDDVal, 3596, "C:\\Src\\Diablo\\Source\\SCROLLRT.CPP");
		}
	} else {
		nSrcOff = SCREENXY(dwX, dwY);
		nDstOff = dwX * (SCREEN_BPP / 8) + dwY * DDS_desc.lPitch;
		nSrcWdt = BUFFER_WIDTH - dwWdt;
		nDstWdt = DDS_desc.lPitch - dwWdt * (SCREEN_BPP / 8);
		dwWdt >>= 2;

		lock_buf(6);

		/// ASSERT: assert(gpBuffer);

#if defined(USE_ASM) && !defined(RGBMODE)
		__asm {
			mov		esi, gpBuffer
			mov		edi, DDS_desc.lpSurface
			add		esi, nSrcOff
			add		edi, nDstOff
			mov		eax, nSrcWdt
			mov		ebx, nDstWdt
			mov		edx, dwHgt
		blitline:
			mov		ecx, dwWdt
			rep movsd
			add		esi, eax
			add		edi, ebx
			dec		edx
			jnz		blitline
		}
#else
		int wdt, hgt;
		BYTE *src, *dst;

		src = &gpBuffer[nSrcOff];
		dst = (BYTE *)DDS_desc.lpSurface + nDstOff;

		for (hgt = 0; hgt < dwHgt; hgt++, src += nSrcWdt, dst += nDstWdt) {
			for (wdt = 0; wdt < 4 * dwWdt; wdt++) {
#ifndef RGBMODE
				*dst++ = *src++;
#else
				PALETTEENTRY pal = system_palette[*src++];
				dst[0] = pal.peBlue;
				dst[1] = pal.peGreen;
				dst[2] = pal.peRed;
				dst += 4;
#endif
			}
		}
#endif

		unlock_buf(6);
	}
}

void DrawAndBlit()
{
	int hgt;
	BOOL ddsdesc, ctrlPan;

	if (!gbRunGame) {
		return;
	}

	if (drawpanflag == 255) {
		drawhpflag = TRUE;
		drawmanaflag = TRUE;
		drawbtnflag = TRUE;
		drawsbarflag = TRUE;
		ddsdesc = FALSE;
		ctrlPan = TRUE;
		hgt = SCREEN_HEIGHT;
	} else if (drawpanflag == 1) {
		ddsdesc = TRUE;
		ctrlPan = FALSE;
		hgt = VIEWPORT_HEIGHT;
	} else {
		return;
	}

	drawpanflag = 0;

	lock_buf(0);
	if (leveltype != DTYPE_TOWN) {
		DrawView(ViewX, ViewY);
	} else {
		T_DrawView(ViewX, ViewY);
	}
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
