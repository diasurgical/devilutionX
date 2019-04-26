#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

int light_table_index; // weak
int PitchTbl[1024];
DWORD sgdwCursWdtOld;    // idb
DWORD sgdwCursX;         // idb
DWORD sgdwCursY;         // idb
unsigned char *gpBufEnd; // weak
DWORD sgdwCursHgt;
DWORD level_cel_block; // weak
DWORD sgdwCursXOld;    // idb
DWORD sgdwCursYOld;    // idb
char arch_draw_type;   // weak
DDSURFACEDESC DDS_desc;
int cel_transparency_active; // weak
int level_piece_id;          // weak
DWORD sgdwCursWdt;
void(*DrawPlrProc)(int, int, int, int, int, BYTE *, int, int, int, int);
BYTE sgSaveBack[8192];
int draw_monster_num; // weak
DWORD sgdwCursHgtOld; // idb

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

	dFlags[x][y] &= ~DFLAG_DEAD_PLAYER;

	for (i = 0; i < MAX_PLRS; i++) {
		p = &plr[i];
		if (p->plractive && !p->_pHitPoints && p->plrlevel == (unsigned char)currlevel && p->WorldX == x && p->WorldY == y) {
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
			dFlags[x][y] |= DFLAG_DEAD_PLAYER;
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

	if (dFlags[x][y] & DFLAG_LIT || plr[myplr]._pInfraFlag || !setlevel && !currlevel) {
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
		} else if (!(dFlags[x][y] & DFLAG_LIT) || plr[myplr]._pInfraFlag && light_table_index > 8) {
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
// 4B8CC2: using guessed type char pcursplr;
// 5CF31D: using guessed type char setlevel;
// 69BEF8: using guessed type int light_table_index;

void DrawClippedPlayer(int pnum, int x, int y, int px, int py, BYTE *pCelBuff, int nCel, int nWidth, int CelSkip, int CelCap)
{
	int l;
	DWORD *pFrameTable;

	if (dFlags[x][y] & DFLAG_LIT || plr[myplr]._pInfraFlag) {
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
		} else if (!(dFlags[x][y] & DFLAG_LIT) || plr[myplr]._pInfraFlag && light_table_index > 8) {
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
// 4B8CC2: using guessed type char pcursplr;
// 69BEF8: using guessed type int light_table_index;

void DrawView(int StartX, int StartY)
{
	if(zoomflag) {
		DrawGame(StartX, StartY);
	} else {
		DrawZoom(StartX, StartY);
	}
	if(automapflag) {
		DrawAutomap();
	}
	if(invflag) {
		DrawInv();
	} else if(sbookflag) {
		DrawSpellBook();
	}

	DrawDurIcon();

	if(chrflag) {
		DrawChr();
	} else if(questlog) {
		DrawQuestLog();
	} else if(plr[myplr]._pStatPts != 0 && !spselflag) {
		DrawLevelUpIcon();
	}
	if(uitemflag) {
		DrawUniqueInfo();
	}
	if(qtextflag) {
		DrawQText();
	}
	if(spselflag) {
		DrawSpellList();
	}
	if(dropGoldFlag) {
		DrawGoldSplit(dropGoldValue);
	}
	if(helpflag) {
		DrawHelp();
	}
	if(msgflag) {
		DrawDiabloMsg();
	}
	if(deathflag) {
		RedBack();
	} else if(PauseMode != 0) {
		gmenu_draw_pause();
	}

	DrawPlrMsg();
	gmenu_draw();
	doom_draw();
	DrawInfoBox();
	DrawLifeFlask();
	DrawManaFlask();
}
// 4B8968: using guessed type int sbookflag;
// 4B8C98: using guessed type int spselflag;
// 52569C: using guessed type int zoomflag;
// 525740: using guessed type int PauseMode;
// 52B9F1: using guessed type char msgflag;
// 646D00: using guessed type char qtextflag;

void DrawGame(int x, int y)
{
	int i, sx, sy, chunks, blocks;

	scr_pix_width = 640;
	scr_pix_height = 352;
	dword_5C2FF8 = 10;
	dword_5C2FFC = 11;

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
	gpBufEnd = &gpBuffer[PitchTbl[160]];
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
	gpBufEnd = &gpBuffer[PitchTbl[512]];
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
// 4B8968: using guessed type int sbookflag;
// 5C2FF8: using guessed type int dword_5C2FF8;
// 5C2FFC: using guessed type int dword_5C2FFC;
// 5C3000: using guessed type int scr_pix_width;
// 5C3004: using guessed type int scr_pix_height;
// 69CF0C: using guessed type int gpBufEnd;
// 69CF20: using guessed type char arch_draw_type;

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
				cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				arch_draw_type = 2;
				level_cel_block = pMap->mt[1];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				arch_draw_type = 0;
				dst -= 768 * 32;
				level_cel_block = pMap->mt[3];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				dst -= 768 * 32;
				level_cel_block = pMap->mt[5];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				dst -= 768 * 32;
				level_cel_block = pMap->mt[7];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				dst -= 768 * 32;
				level_cel_block = pMap->mt[9];
				if (level_cel_block != 0) {
					drawLowerScreen(dst);
				}
				dst -= 768 * 32;
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
				cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
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
					dst -= 768 * 32;
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
			cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
			arch_draw_type = 1;
			level_cel_block = pMap->mt[0];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			arch_draw_type = 0;
			dst -= 768 * 32;
			level_cel_block = pMap->mt[2];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			dst -= 768 * 32;
			level_cel_block = pMap->mt[4];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			dst -= 768 * 32;
			level_cel_block = pMap->mt[6];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			dst -= 768 * 32;
			level_cel_block = pMap->mt[8];
			if (level_cel_block != 0) {
				drawLowerScreen(dst);
			}
			dst -= 768 * 32;
			level_cel_block = pMap->mt[10];
			if (level_cel_block != 0 && leveltype == DTYPE_HELL) {
				drawLowerScreen(dst);
			}
			scrollrt_draw_clipped_dungeon(&gpBuffer[sx + PitchTbl[sy]], x, y, sx, sy, 0);
		}
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF20: using guessed type char arch_draw_type;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

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

	int xyoff;

	/// ASSERT: assert((DWORD)sx < MAXDUNX);
	/// ASSERT: assert((DWORD)sy < MAXDUNY);
	xyoff = 112 * sx + sy;
	bFlag = dFlags[0][xyoff];
	bDead = dDead[0][xyoff];
	bObj = dObject[0][xyoff];
	bItem = dItem[0][xyoff];
	bPlr = dPlayer[0][xyoff];
	bArch = dArch[0][xyoff];
	bMap = dTransVal[0][xyoff];
	nMon = dMonster[0][xyoff];

	/// ASSERT: assert((DWORD)(sy-1) < MAXDUNY);
	negPlr = dPlayer[0][xyoff - 1];
	negMon = dMonster[0][xyoff - 1];

	if(visiondebug && bFlag & DFLAG_LIT) {
		Cel2DecodeHdrOnly(pBuff, (BYTE *)pSquareCel, 1, 64, 0, 8);
	}
	if(MissilePreFlag && bFlag & DFLAG_MISSILE) {
		DrawClippedMissile(sx, sy, dx, dy, 0, 8, 1);
	}
	if(light_table_index < lightmax) {
		if(bDead != 0) {
			pDeadGuy = &dead[(bDead & 0x1F) - 1];
			dd = (bDead >> 5) & 7;
			pCelBuff = pDeadGuy->_deadData[dd];
			px = dx - pDeadGuy->_deadWidth2;
			/// ASSERT: assert(pDeadGuy->_deadData[dd] != NULL);
			if(pCelBuff != NULL) {
				pFrameTable = (DWORD *)pCelBuff;
				nCel = pDeadGuy->_deadFrame;
				if(nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
					if(pDeadGuy->_deadtrans != 0) {
						Cl2DecodeFrm5(px, dy, pCelBuff, nCel, pDeadGuy->_deadWidth, 0, 8, pDeadGuy->_deadtrans);
					} else {
						Cl2DecodeFrm6(px, dy, pCelBuff, nCel, pDeadGuy->_deadWidth, 0, 8);
					}
				} else {
					// app_fatal("Clipped dead sub: frame %d of %d, deadnum==%d", nCel, pFrameTable[0], (bDead & 0x1F) - 1);
				}
			}
		}
		if(bObj != 0) {
			DrawClippedObject(sx, sy, dx, dy, 1, 0, 8);
		}
	}
	if(bItem != 0) {
		pItem = &item[bItem - 1];
		if(!pItem->_iPostDraw) {
			/// ASSERT: assert((unsigned char)bItem <= MAXITEMS);
			if((unsigned char)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if(pCelBuff != NULL) {
					pFrameTable = (DWORD *)pCelBuff;
					nCel = pItem->_iAnimFrame;
					if(nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if(bItem - 1 == pcursitem) {
							CelDrawHdrClrHL(181, px, dy, pCelBuff, pItem->_iAnimFrame, pItem->_iAnimWidth, 0, 8);
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
	if(bFlag & DFLAG_PLAYER) {
		p = -(negPlr + 1);
		if((DWORD)p < MAX_PLRS) {
			pPlayer = &plr[p];
			px = dx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
			py = dy + pPlayer->_pyoff;
			DrawClippedPlayer(p, sx, sy - 1, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth, 0, 8);
			if(eflag && pPlayer->_peflag != 0) {
				if(pPlayer->_peflag == 2) {
					scrollrt_draw_clipped_e_flag(pBuff - (768 * 16 + 96), sx - 2, sy + 1, dx - 96, dy - 16);
				}
				scrollrt_draw_clipped_e_flag(pBuff - 64, sx - 1, sy + 1, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player clipped: tried to draw illegal player %d", p);
		}
	}
	if(bFlag & DFLAG_MONSTER && (bFlag & DFLAG_LIT || plr[myplr]._pInfraFlag) && negMon < 0) {
		draw_monster_num = -(negMon + 1);
		if((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if(!(pMonster->_mFlags & 1)) {
				if(pMonster->MType != NULL) {
					py = dy + pMonster->_myoff;
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
					if(draw_monster_num == pcursmonst) {
						Cl2DecodeClrHL(233, px, py, pMonster->_mAnimData, pMonster->_mAnimFrame, pMonster->MType->width, 0, 8);
					}
					DrawClippedMonster(sx, sy, px, py, draw_monster_num, 0, 8);
					if(eflag && pMonster->_meflag) {
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
	if(bFlag & DFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(sx, sy, dx, dy, 0, 8, 1);
	}
	if(bPlr > 0) {
		p = bPlr - 1;
		if((DWORD)p < MAX_PLRS) {
			pPlayer = &plr[p];
			px = dx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
			py = dy + pPlayer->_pyoff;
			DrawClippedPlayer(p, sx, sy, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth, 0, 8);
			if(eflag && pPlayer->_peflag != 0) {
				if(pPlayer->_peflag == 2) {
					scrollrt_draw_clipped_e_flag(pBuff - (768 * 16 + 96), sx - 2, sy + 1, dx - 96, dy - 16);
				}
				scrollrt_draw_clipped_e_flag(pBuff - 64, sx - 1, sy + 1, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player clipped: tried to draw illegal player %d", p);
		}
	}
	if(nMon > 0 && (bFlag & DFLAG_LIT || plr[myplr]._pInfraFlag)) {
		draw_monster_num = nMon - 1;
		if((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if(!(pMonster->_mFlags & 1)) {
				if(pMonster->MType != NULL) {
					py = dy + pMonster->_myoff;
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
					if(draw_monster_num == pcursmonst) {
						Cl2DecodeClrHL(233, px, py, pMonster->_mAnimData, pMonster->_mAnimFrame, pMonster->MType->width, 0, 8);
					}
					DrawClippedMonster(sx, sy, px, py, draw_monster_num, 0, 8);
					if(eflag && pMonster->_meflag) {
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
	if(bFlag & DFLAG_MISSILE) {
		DrawClippedMissile(sx, sy, dx, dy, 0, 8, 0);
	}
	if(bObj != 0 && light_table_index < lightmax) {
		DrawClippedObject(sx, sy, dx, dy, 0, 0, 8);
	}
	if(bItem != 0) {
		pItem = &item[bItem - 1];
		if(pItem->_iPostDraw) {
			/// ASSERT: assert((unsigned char)bItem <= MAXITEMS);
			if((unsigned char)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if(pCelBuff != NULL) {
					pFrameTable = (DWORD *)pCelBuff;
					nCel = pItem->_iAnimFrame;
					if(nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if(bItem - 1 == pcursitem) {
							CelDrawHdrClrHL(181, px, dy, pCelBuff, pItem->_iAnimFrame, pItem->_iAnimWidth, 0, 8);
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
	if(bArch != 0) {
		cel_transparency_active = (unsigned char)TransList[bMap];
		Cel2DecodeLightTrans(pBuff, (BYTE *)level_special_cel, bArch, 64, 0, 8);
	}
}
// 4B8CC0: using guessed type char pcursitem;
// 525720: using guessed type int visiondebug;
// 642A14: using guessed type char lightmax;
// 64CCD4: using guessed type int MissilePreFlag;
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;
// 69EFA4: using guessed type int draw_monster_num;

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

	if (!(dFlags[x][y] & DFLAG_LIT)) {
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
// 69BEF8: using guessed type int light_table_index;

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
	if ((unsigned char)bv >= MAXOBJECTS)
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
// 4B8CC1: using guessed type char pcursobj;

void scrollrt_draw_clipped_e_flag(BYTE *pBuff, int x, int y, int a4, int a5)
{
	int i, lti_old, cta_old, lpi_old;
	BYTE *dst;
	MICROS *pMap;

	lti_old = light_table_index;
	cta_old = cel_transparency_active;
	lpi_old = level_piece_id;

	level_piece_id = dPiece[x][y];
	light_table_index = dLight[x][y];
	cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
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
		dst -= 768 * 32;
		level_cel_block = pMap->mt[i];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[i + 1];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
	}

	scrollrt_draw_clipped_dungeon(pBuff, x, y, a4, a5, 0);

	light_table_index = lti_old;
	cel_transparency_active = cta_old;
	level_piece_id = lpi_old;
}
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF20: using guessed type char arch_draw_type;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

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
				dst = &gpBuffer[sx - (768 * 32 - 32) + PitchTbl[sy]];
				cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				for (i = 0; i < (MicroTileLen >> 1) - 1; i++) {
					if (skipChunks <= i) {
						level_cel_block = pMap->mt[2 * i + 3];
						if (level_cel_block != 0) {
							drawLowerScreen(dst);
						}
					}
					dst -= 768 * 32;
				}
				if (CelSkip < 8) {
					scrollrt_draw_clipped_dungeon_2((char *)&gpBuffer[sx + PitchTbl[sy] - 768 * 16 * CelSkip], x, y, skipChunks, CelSkip, sx, sy, 0);
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
				dst = &gpBuffer[sx - 768 * 32 + PitchTbl[sy]];
				cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
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
					dst -= 768 * 32;
				}
				if (CelSkip < 8) {
					scrollrt_draw_clipped_dungeon_2((char *)&gpBuffer[sx + PitchTbl[sy] - 768 * 32 * (skipChunks + 1)], x, y, skipChunks, CelSkip, sx, sy, 1);
				}
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
		if (level_piece_id != 0) {
			dst = &gpBuffer[sx - 768 * 32 + PitchTbl[sy]];
			cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
			for (i = 0; i < (MicroTileLen >> 1) - 1; i++) {
				if (skipChunks <= i) {
					level_cel_block = pMap->mt[2 * i + 2];
					if (level_cel_block != 0) {
						drawLowerScreen(dst);
					}
				}
				dst -= 768 * 32;
			}
			if (CelSkip < 8) {
				scrollrt_draw_clipped_dungeon_2((char *)&gpBuffer[sx + PitchTbl[sy] - 768 * 16 * CelSkip], x, y, skipChunks, CelSkip, sx, sy, 0);
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

void scrollrt_draw_clipped_dungeon_2(char *buffer, int x, int y, int skipChunks, int CelSkip, int sx, int sy, int eflag)
{
	int v8;             // eax
	char v9;            // bl
	char v10;           // cl
	char v11;           // dl
	int *v12;           // eax
	int v13;            // edi
	DeadStruct *v14;    // eax
	int *v15;           // esi
	int v16;            // ecx
	int v17;            // edx
	char v18;           // bl
	ItemStruct *v19;    // esi
	char *v20;          // eax
	signed int v21;     // ebx
	int v22;            // ebx
	unsigned int v23;   // ecx
	PlayerStruct *v24;  // esi
	int v25;            // esi
	int v26;            // eax
	MonsterStruct *v27; // esi
	CMonster *v28;      // ecx
	int v29;            // ebx
	int v30;            // edi
	unsigned int v31;   // ecx
	PlayerStruct *v32;  // esi
	int v33;            // esi
	int v34;            // eax
	MonsterStruct *v35; // esi
	CMonster *v36;      // ecx
	int v37;            // ebx
	int v38;            // edi
	ItemStruct *v39;    // esi
	char *v40;          // eax
	int v41;            // ecx
	int v42;            // edi
	int v43;            // [esp+Ch] [ebp-18h]
	int v44;            // [esp+10h] [ebp-14h]
	char *dst_buf;      // [esp+14h] [ebp-10h]
	int a1;             // [esp+18h] [ebp-Ch]
	char v47;           // [esp+1Dh] [ebp-7h]
	char v48;           // [esp+1Eh] [ebp-6h]
	char v49;           // [esp+1Fh] [ebp-5h]
	char v50;           // [esp+20h] [ebp-4h]
	char v51;           // [esp+21h] [ebp-3h]
	char v52;           // [esp+22h] [ebp-2h]
	char v53;           // [esp+23h] [ebp-1h]

	a1 = x;
	dst_buf = buffer;
	v8 = 112 * x + y;
	v9 = dDead[0][v8];
	v53 = dFlags[0][v8];
	v50 = dObject[0][v8];
	v52 = dItem[0][v8];
	v10 = dPlayer[0][v8 - 1];
	v51 = dPlayer[0][v8];
	v49 = dArch[0][v8];
	v11 = dTransVal[0][v8];
	v12 = (int *)((char *)dMonster + 4 * v8);
	v47 = v11;
	v48 = v10;
	v43 = *v12;
	v44 = *(v12 - 1);
	if (visiondebug && v53 & DFLAG_LIT)
		Cel2DecodeHdrOnly((BYTE *)dst_buf, (BYTE *)pSquareCel, 1, 64, CelSkip, 8);
	if (MissilePreFlag && v53 & DFLAG_MISSILE) {
		v13 = sx;
		DrawClippedMissile(a1, y, sx, sy, CelSkip, 8, TRUE);
	} else {
		v13 = sx;
	}
	if (light_table_index < lightmax) {
		if (v9) {
			v14 = &dead[(v9 & 0x1F) - 1];
			v15 = (int *)v14->_deadData[(v9 >> 5) & 7];
			v16 = v13 - v14->_deadWidth2;
			if (v15) {
				v17 = v14->_deadFrame;
				if (v17 >= 1 && (unsigned int)*v15 <= 0x32 && v17 <= *v15) {
					v18 = v14->_deadtrans;
					if (v18)
						Cl2DecodeFrm5(v16, sy, (BYTE *)v15, v17, v14->_deadWidth, CelSkip, 8, v18);
					else
						Cl2DecodeFrm6(v16, sy, (BYTE *)v15, v17, v14->_deadWidth, CelSkip, 8);
				}
			}
		}
		if (v50)
			DrawClippedObject(a1, y, v13, sy, 1, CelSkip, 8);
	}
	if (v52) {
		v19 = &item[v52 - 1];
		if (!v19->_iPostDraw && (unsigned char)v52 <= MAXITEMS) {
			v20 = (char *)v19->_iAnimData;
			if (v20) {
				v21 = v19->_iAnimFrame;
				if (v21 >= 1 && *(_DWORD *)v20 <= 0x32u && v21 <= *(_DWORD *)v20) {
					v22 = v13 - v19->_iAnimWidth2;
					if (v52 - 1 == pcursitem)
						CelDrawHdrClrHL(ICOL_BLUE, v22, sy, (BYTE *)v20, v19->_iAnimFrame, v19->_iAnimWidth, CelSkip, 8);
					Cel2DecodeHdrLight(v22, sy, v19->_iAnimData, v19->_iAnimFrame, v19->_iAnimWidth, CelSkip, 8);
				}
			}
		}
	}
	if (v53 & DFLAG_PLAYER) {
		v23 = -1 - v48;
		if (v23 < 4) {
			v24 = &plr[v23];
			DrawClippedPlayer(
			    v23,
			    a1,
			    y - 1,
			    v13 + v24->_pxoff - v24->_pAnimWidth2,
			    sy + v24->_pyoff,
			    v24->_pAnimData,
			    v24->_pAnimFrame,
			    v24->_pAnimWidth,
			    CelSkip,
			    8);
			if (eflag) {
				v25 = v24->_peflag;
				if (v25) {
					if (v25 == 2)
						scrollrt_draw_clipped_e_flag_2((BYTE *)dst_buf - 12384, a1 - 2, y + 1, skipChunks, CelSkip, v13 - 96, sy - 16);
					scrollrt_draw_clipped_e_flag_2((BYTE *)dst_buf - 64, a1 - 1, y + 1, skipChunks, CelSkip, v13 - 64, sy);
				}
			}
		}
	}
	if (v53 & DFLAG_MONSTER && (v53 & DFLAG_LIT || plr[myplr]._pInfraFlag) && v44 < 0) {
		v26 = -1 - v44;
		draw_monster_num = -1 - v44;
		if ((unsigned int)(-1 - v44) < MAXMONSTERS) {
			v27 = &monster[v26];
			if (!(v27->_mFlags & MFLAG_HIDDEN)) {
				v28 = v27->MType;
				if (v28) {
					v29 = sy + v27->_myoff;
					v30 = sx + v27->_mxoff - v28->width2;
					if (v26 == pcursmonst) {
						Cl2DecodeClrHL(233, v30, v29, v27->_mAnimData, v27->_mAnimFrame, v28->width, CelSkip, 8);
						v26 = draw_monster_num;
					}
					DrawClippedMonster(a1, y, v30, v29, v26, CelSkip, 8);
					if (eflag && !v27->_meflag)
						scrollrt_draw_clipped_e_flag_2((BYTE *)dst_buf - 64, a1 - 1, y + 1, skipChunks, CelSkip, sx - 64, sy);
					v13 = sx;
				}
			}
		}
	}
	if (v53 & DFLAG_DEAD_PLAYER)
		DrawDeadPlayer(a1, y, v13, sy, CelSkip, 8, 1);
	if (v51 > 0) {
		v31 = v51 - 1;
		if (v31 < 4) {
			v32 = &plr[v31];
			DrawClippedPlayer(
			    v31,
			    a1,
			    y,
			    v13 + v32->_pxoff - v32->_pAnimWidth2,
			    sy + v32->_pyoff,
			    v32->_pAnimData,
			    v32->_pAnimFrame,
			    v32->_pAnimWidth,
			    CelSkip,
			    8);
			if (eflag) {
				v33 = v32->_peflag;
				if (v33) {
					if (v33 == 2)
						scrollrt_draw_clipped_e_flag_2((BYTE *)dst_buf - 12384, a1 - 2, y + 1, skipChunks, CelSkip, v13 - 96, sy - 16);
					scrollrt_draw_clipped_e_flag_2((BYTE *)dst_buf - 64, a1 - 1, y + 1, skipChunks, CelSkip, v13 - 64, sy);
				}
			}
		}
	}
	if (v43 > 0 && (v53 & DFLAG_LIT || plr[myplr]._pInfraFlag)) {
		v34 = v43 - 1;
		draw_monster_num = v43 - 1;
		if ((unsigned int)(v43 - 1) < MAXMONSTERS) {
			v35 = &monster[v34];
			if (!(v35->_mFlags & MFLAG_HIDDEN)) {
				v36 = v35->MType;
				if (v36) {
					v37 = sy + v35->_myoff;
					v38 = sx + v35->_mxoff - v36->width2;
					if (v34 == pcursmonst) {
						Cl2DecodeClrHL(233, v38, v37, v35->_mAnimData, v35->_mAnimFrame, v36->width, CelSkip, 8);
						v34 = draw_monster_num;
					}
					DrawClippedMonster(a1, y, v38, v37, v34, CelSkip, 8);
					if (eflag && !v35->_meflag)
						scrollrt_draw_clipped_e_flag_2((BYTE *)dst_buf - 64, a1 - 1, y + 1, skipChunks, CelSkip, sx - 64, sy);
					v13 = sx;
				}
			}
		}
	}
	if (v53 & DFLAG_MISSILE)
		DrawClippedMissile(a1, y, v13, sy, CelSkip, 8, FALSE);
	if (v50 && light_table_index < lightmax)
		DrawClippedObject(a1, y, v13, sy, 0, CelSkip, 8);
	if (v52) {
		v39 = &item[v52 - 1];
		if (v39->_iPostDraw) {
			if ((unsigned char)v52 <= MAXITEMS) {
				v40 = (char *)v39->_iAnimData;
				if (v40) {
					v41 = v39->_iAnimFrame;
					if (v41 >= 1 && *(_DWORD *)v40 <= 0x32u && v41 <= *(_DWORD *)v40) {
						v42 = v13 - v39->_iAnimWidth2;
						if (v52 - 1 == pcursitem)
							CelDrawHdrClrHL(ICOL_BLUE, v42, sy, (BYTE *)v40, v41, v39->_iAnimWidth, CelSkip, 8);
						Cel2DecodeHdrLight(
						    v42,
						    sy,
						    v39->_iAnimData,
						    v39->_iAnimFrame,
						    v39->_iAnimWidth,
						    CelSkip,
						    8);
					}
				}
			}
		}
	}
	if (v49) {
		cel_transparency_active = (unsigned char)TransList[v47];
		Cel2DecodeLightTrans((BYTE *)dst_buf, (BYTE *)level_special_cel, v49, 64, CelSkip, 8);
	}
}
// 4B8CC0: using guessed type char pcursitem;
// 525720: using guessed type int visiondebug;
// 642A14: using guessed type char lightmax;
// 64CCD4: using guessed type int MissilePreFlag;
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;
// 69EFA4: using guessed type int draw_monster_num;

void scrollrt_draw_clipped_e_flag_2(BYTE *pBuff, int x, int y, int skipChunks, signed int CelSkip, int sx, int sy)
{
	int lti_old, cta_old, lpi_old;
	BYTE *dst;
	MICROS *pMap;

	lti_old = light_table_index;
	cta_old = cel_transparency_active;
	lpi_old = level_piece_id;

	level_piece_id = dPiece[x][y];
	light_table_index = dLight[x][y];
	dst = &pBuff[768 * 32 * skipChunks];
	cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
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
		dst -= 768 * 32;
		level_cel_block = pMap->mt[4];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[5];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
	case 2:
		dst -= 768 * 32;
		level_cel_block = pMap->mt[6];
		if (level_cel_block != 0) {
			drawLowerScreen(dst);
		}
		level_cel_block = pMap->mt[7];
		if (level_cel_block != 0) {
			drawLowerScreen(dst + 32);
		}
	case 3:
		dst -= 768 * 32;
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
		scrollrt_draw_clipped_dungeon_2((char *)pBuff, x, y, skipChunks, CelSkip, sx, sy, 0);
	}

	light_table_index = lti_old;
	cel_transparency_active = cta_old;
	level_piece_id = lpi_old;
}
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

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
				cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
				if (capChunks >= 0) {
					level_cel_block = pMap->mt[1];
					if (level_cel_block != 0) {
						arch_draw_type = 2;
						drawUpperScreen(dst);
						arch_draw_type = 0;
					}
				}
				dst -= 768 * 32;
				if (capChunks >= 1) {
					level_cel_block = pMap->mt[3];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
				}
				dst -= 768 * 32;
				if (capChunks >= 2) {
					level_cel_block = pMap->mt[5];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
				}
				dst -= 768 * 32;
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
				cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
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
					dst -= 768 * 32;
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

	if (eflag && y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
		level_piece_id = dPiece[x][y];
		light_table_index = dLight[x][y];
		if (level_piece_id != 0) {
			dst = &gpBuffer[sx + PitchTbl[sy]];
			cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
			arch_draw_type = 1;
			if (capChunks >= 0) {
				level_cel_block = pMap->mt[0];
				if (level_cel_block != 0) {
					drawUpperScreen(dst);
				}
			}
			arch_draw_type = 0;
			dst -= 768 * 32;
			if (capChunks >= 1) {
				level_cel_block = pMap->mt[2];
				if (level_cel_block != 0) {
					drawUpperScreen(dst);
				}
			}
			dst -= 768 * 32;
			if (capChunks >= 2) {
				level_cel_block = pMap->mt[4];
				if (level_cel_block != 0) {
					drawUpperScreen(dst);
				}
			}
			dst -= 768 * 32;
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
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF20: using guessed type char arch_draw_type;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

void scrollrt_draw_dungeon(BYTE *pBuff, int sx, int sy, int capChunks, int CelCap, int dx, int dy, int eflag)
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

	if (visiondebug && (bFlag & 0x40)) {
		CelDecodeHdrOnly(pBuff, (BYTE *)pSquareCel, 1, 64, 0, CelCap);
	}
	if (MissilePreFlag && (bFlag & 1)) {
		DrawMissile(sx, sy, dx, dy, 0, CelCap, 1);
	}
	if (light_table_index < lightmax) {
		if (bDead != 0) {
			pDeadGuy = &dead[(bDead & 0x1F) - 1];
			dd = (unsigned char)(bDead >> 5) & 7;
			px = dx - pDeadGuy->_deadWidth2;
			pCelBuff = pDeadGuy->_deadData[dd];
			/// ASSERT: assert(pDeadGuy->_deadData[dd] != NULL);
			if (pCelBuff != NULL) {
				nCel = pDeadGuy->_deadFrame;
				pFrameTable = (DWORD *)pCelBuff;
				if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
					if (pDeadGuy->_deadtrans != 0) {
						Cl2DecodeFrm3(px, dy, pDeadGuy->_deadData[dd], pDeadGuy->_deadFrame, pDeadGuy->_deadWidth, 0, CelCap, pDeadGuy->_deadtrans);
					} else {
						Cl2DecodeLightTbl(px, dy, pDeadGuy->_deadData[dd], pDeadGuy->_deadFrame, pDeadGuy->_deadWidth, 0, CelCap);
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
			if ((unsigned char)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if (pCelBuff != NULL) {
					nCel = pItem->_iAnimFrame;
					pFrameTable = (DWORD *)pCelBuff;
					if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if (bItem - 1 == pcursitem) {
							CelDecodeClr(181, px, dy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth, 0, CelCap);
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
	if (bFlag & 0x20) {
		p = -(negPlr + 1);
		if ((DWORD)p < MAX_PLRS) {
			pPlayer = &plr[p];
			px = dx + pPlayer->_pxoff - pPlayer->_pAnimWidth2;
			py = dy + pPlayer->_pyoff;
			DrawPlayer(p, sx, sy - 1, px, py, pPlayer->_pAnimData, pPlayer->_pAnimFrame, pPlayer->_pAnimWidth, 0, CelCap);
			if (eflag && pPlayer->_peflag != 0) {
				if (pPlayer->_peflag == 2) {
					scrollrt_draw_e_flag(pBuff - (768 * 16 + 96), sx - 2, sy + 1, capChunks, CelCap, dx - 96, dy - 16);
				}
				scrollrt_draw_e_flag(pBuff - 64, sx - 1, sy + 1, capChunks, CelCap, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player: tried to draw illegal player %d", p);
		}
	}
	if ((bFlag & 0x10) && ((bFlag & 0x40) || plr[myplr]._pInfraFlag) && negMon < 0) {
		draw_monster_num = -(negMon + 1);
		if ((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if (!(pMonster->_mFlags & 1)) {
				if (pMonster->MType != NULL) {
					py = dy + pMonster->_myoff;
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
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
	if (bFlag & 4) {
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
					scrollrt_draw_e_flag(pBuff - (768 * 16 + 96), sx - 2, sy + 1, capChunks, CelCap, dx - 96, dy - 16);
				}
				scrollrt_draw_e_flag(pBuff - 64, sx - 1, sy + 1, capChunks, CelCap, dx - 64, dy);
			}
		} else {
			// app_fatal("draw player: tried to draw illegal player %d", p);
		}
	}
	if (nMon > 0 && ((bFlag & 0x40) || plr[myplr]._pInfraFlag)) {
		draw_monster_num = nMon - 1;
		if ((DWORD)draw_monster_num < MAXMONSTERS) {
			pMonster = &monster[draw_monster_num];
			if (!(pMonster->_mFlags & 1)) {
				if (pMonster->MType != NULL) {
					py = dy + pMonster->_myoff;
					px = dx + pMonster->_mxoff - pMonster->MType->width2;
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
	if (bFlag & 1) {
		DrawMissile(sx, sy, dx, dy, 0, CelCap, 0);
	}
	if (bObj != 0 && light_table_index < lightmax) {
		DrawObject(sx, sy, dx, dy, 0, 0, CelCap);
	}
	if (bItem != 0) {
		pItem = &item[bItem - 1];
		if (pItem->_iPostDraw) {
			/// ASSERT: assert((unsigned char)bItem <= MAXITEMS);
			if ((unsigned char)bItem <= MAXITEMS) {
				pCelBuff = pItem->_iAnimData;
				if (pCelBuff != NULL) {
					nCel = pItem->_iAnimFrame;
					pFrameTable = (DWORD *)pCelBuff;
					if (nCel >= 1 && pFrameTable[0] <= 50 && nCel <= (int)pFrameTable[0]) {
						px = dx - pItem->_iAnimWidth2;
						if (bItem - 1 == pcursitem) {
							CelDecodeClr(181, px, dy, pItem->_iAnimData, pItem->_iAnimFrame, pItem->_iAnimWidth, 0, CelCap);
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
		cel_transparency_active = (unsigned char)TransList[bMap];
		CelDecodeHdrLightTrans(pBuff, (BYTE *)level_special_cel, bArch, 64, 0, CelCap);
	}
}
// 4B8CC0: using guessed type char pcursitem;
// 525720: using guessed type int visiondebug;
// 642A14: using guessed type char lightmax;
// 64CCD4: using guessed type int MissilePreFlag;
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;
// 69EFA4: using guessed type int draw_monster_num;

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

	if (!(dFlags[x][y] & DFLAG_LIT)) {
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
// 69BEF8: using guessed type int light_table_index;

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
	if ((unsigned char)bv >= MAXOBJECTS)
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
// 4B8CC1: using guessed type char pcursobj;

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
	cel_transparency_active = (unsigned char)(nTransTable[level_piece_id] & TransList[dTransVal[x][y]]);
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
		dst -= 768 * 32;
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
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF20: using guessed type char arch_draw_type;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

void DrawZoom(int x, int y)
{
	int i, sx, sy, chunks, blocks;
	int wdt, nSrcOff, nDstOff;

	scr_pix_width = 384;
	scr_pix_height = 192;
	dword_5C2FF8 = 6;
	dword_5C2FFC = 6;

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
	gpBufEnd = &gpBuffer[PitchTbl[143]];
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
	gpBufEnd = &gpBuffer[PitchTbl[320]];
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
		wdt = 160;
	} else if (invflag || sbookflag) {
		nSrcOff = SCREENXY(112, 159);
		nDstOff = SCREENXY(0, 350);
		wdt = 160;
	} else {
		nSrcOff = SCREENXY(32, 159);
		nDstOff = SCREENXY(0, 350);
		wdt = 320;
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
		add		ebx, 768
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
		mov		eax, 768
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
	dst2 = &gpBuffer[nDstOff + 768];

	for (hgt = 176; hgt != 0; hgt--, src -= 768 + wdt, dst1 -= 2 * (768 + wdt), dst2 -= 2 * (768 + wdt)) {
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
// 4B8968: using guessed type int sbookflag;
// 5C2FF8: using guessed type int dword_5C2FF8;
// 5C2FFC: using guessed type int dword_5C2FFC;
// 5C3000: using guessed type int scr_pix_width;
// 5C3004: using guessed type int scr_pix_height;
// 69CF0C: using guessed type int gpBufEnd;
// 69CF20: using guessed type char arch_draw_type;

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
		hgt = 480;
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
// 52571C: using guessed type int drawpanflag;

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

	for (i = sgdwCursHgt; i != 0; i--, src += sgdwCursWdt, dst += 768) {
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

	for (i = sgdwCursHgt; i != 0; i--, dst += sgdwCursWdt, src += 768) {
		memcpy(dst, src, sgdwCursWdt);
	}

	mx++;
	my++;
	gpBufEnd = &gpBuffer[PitchTbl[640] - cursW - 2];

	if (pcurs >= CURSOR_FIRSTITEM) {
		col = PAL16_YELLOW + 5;
		if (plr[myplr].HoldItem._iMagical != 0) {
			col = PAL16_BLUE + 5;
		}
		if (!plr[myplr].HoldItem._iStatFlag) {
			col = PAL16_RED + 5;
		}
		CelDrawHdrClrHL(col, mx + 64, my + cursH + 160 - 1, (BYTE *)pCursCels, pcurs, cursW, 0, 8);
		if (col != PAL16_RED + 5) {
			Cel2DrawHdrOnly(mx + 64, my + cursH + 160 - 1, (BYTE *)pCursCels, pcurs, cursW, 0, 8);
		} else {
			Cel2DrawHdrLightRed(mx + 64, my + cursH + 160 - 1, (BYTE *)pCursCels, pcurs, cursW, 0, 8, 1);
		}
	} else {
		Cel2DrawHdrOnly(mx + 64, my + cursH + 160 - 1, (BYTE *)pCursCels, pcurs, cursW, 0, 8);
	}
}
// 4B8C9C: using guessed type int cursH;
// 69CF0C: using guessed type int gpBufEnd;

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
		ysize = 480;
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
				ysize = 480;
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

	/// ASSERT: assert(ysize >= 0 && ysize <= 480);

	if (ysize > 0) {
		DoBlitScreen(0, 0, 640, ysize);
	}
	if (ysize < 480) {
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
// 634980: using guessed type int gbActive;
// 679660: using guessed type char gbMaxPlayers;

#ifdef _DEBUG
void DrawFPS()
{
	DWORD v0;        // eax
	int v1;          // esi
	char String[12]; // [esp+8h] [ebp-10h]
	HDC hdc;         // [esp+14h] [ebp-4h]

	if (frameflag && gbActive) {
		++frameend;
		v0 = GetTickCount();
		v1 = v0 - framestart;
		if (v0 - framestart >= 1000) {
			framestart = v0;
			framerate = 1000 * frameend / v1;
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
		SrcRect.left = dwX + 64;
		SrcRect.top = dwY + 160;
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

	if(!gbRunGame) {
		return;
	}

	if(drawpanflag == 255) {
		drawhpflag = TRUE;
		drawmanaflag = TRUE;
		drawbtnflag = TRUE;
		drawsbarflag = TRUE;
		ddsdesc = FALSE;
		ctrlPan = TRUE;
		hgt = 480;
	} else if(drawpanflag == 1) {
		ddsdesc = TRUE;
		ctrlPan = FALSE;
		hgt = 352;
	} else {
		return;
	}

	drawpanflag = 0;

	lock_buf(0);
	if(leveltype != DTYPE_TOWN) {
		DrawView(ViewX, ViewY);
	} else {
		T_DrawView(ViewX, ViewY);
	}
	if(ctrlPan) {
		ClearCtrlPan();
	}
	if(drawhpflag) {
		UpdateLifeFlask();
	}
	if(drawmanaflag) {
		UpdateManaFlask();
	}
	if(drawbtnflag) {
		DrawCtrlPan();
	}
	if(drawsbarflag) {
		DrawInvBelt();
	}
	if(talkflag) {
		DrawTalkPan();
		hgt = 480;
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
// 4B8960: using guessed type int talkflag;
// 52571C: using guessed type int drawpanflag;

DEVILUTION_END_NAMESPACE
