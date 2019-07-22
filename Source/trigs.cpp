#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL townwarps[3];
BOOL trigflag;
int numtrigs;
TriggerStruct trigs[MAXTRIGGERS];
int TWarpFrom;

int TownDownList[11] = { 716, 715, 719, 720, 721, 723, 724, 725, 726, 727, -1 };
int TownWarp1List[13] = {
	1171,
	1172,
	1173,
	1174,
	1175,
	1176,
	1177,
	1178,
	1179,
	1181,
	1183,
	1185,
	-1
};
int L1UpList[12] = { 127, 129, 130, 131, 132, 133, 135, 137, 138, 139, 140, -1 };
int L1DownList[10] = { 106, 107, 108, 109, 110, 112, 114, 115, 118, -1 };
int L2UpList[3] = { 266, 267, -1 };
int L2DownList[5] = { 269, 270, 271, 272, -1 };
int L2TWarpUpList[3] = { 558, 559, -1 };
int L3UpList[15] = {
	170,
	171,
	172,
	173,
	174,
	175,
	176,
	177,
	178,
	179,
	180,
	181,
	182,
	183,
	-1
};
int L3DownList[9] = { 162, 163, 164, 165, 166, 167, 168, 169, -1 };
int L3TWarpUpList[14] = { 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, -1 };
int L4UpList[4] = { 82, 83, 90, -1 };
int L4DownList[6] = { 120, 130, 131, 132, 133, -1 };
int L4TWarpUpList[4] = { 421, 422, 429, -1 };
int L4PentaList[33] = {
	353,
	354,
	355,
	356,
	357,
	358,
	359,
	360,
	361,
	362,
	363,
	364,
	365,
	366,
	367,
	368,
	369,
	370,
	371,
	372,
	373,
	374,
	375,
	376,
	377,
	378,
	379,
	380,
	381,
	382,
	383,
	384,
	-1
};

void InitNoTriggers()
{
	numtrigs = 0;
	trigflag = 0;
}

void InitTownTriggers()
{
	int i;

	trigs[0]._tx = 25;
	trigs[0]._ty = 29;
	trigs[0]._tmsg = WM_DIABNEXTLVL;

	numtrigs = 1;

	if (gbMaxPlayers == MAX_PLRS) {
		for (i = 0; i < sizeof(townwarps) / sizeof(townwarps[0]); i++) {
			townwarps[i] = TRUE;
		}
		trigs[1]._tx = 49;
		trigs[1]._ty = 21;
		trigs[1]._tmsg = WM_DIABTOWNWARP;
		trigs[1]._tlvl = 5;
		trigs[2]._tx = 17;
		trigs[2]._ty = 69;
		trigs[2]._tmsg = WM_DIABTOWNWARP;
		trigs[2]._tlvl = 9;
		trigs[3]._tx = 41;
		trigs[3]._ty = 80;
		trigs[3]._tmsg = WM_DIABTOWNWARP;
		trigs[3]._tlvl = 13;
		numtrigs = 4;
	} else {
		for (i = 0; i < 3; i++) {
			townwarps[i] = FALSE;
		}
		if (plr[myplr].pTownWarps & 1) {
			trigs[1]._tx = 49;
			trigs[1]._ty = 21;
			trigs[1]._tmsg = WM_DIABTOWNWARP;
			trigs[1]._tlvl = 5;
			numtrigs = 2;
			townwarps[0] = TRUE;
		}
		if (plr[myplr].pTownWarps & 2) {
			townwarps[1] = TRUE;
			trigs[numtrigs]._tx = 17;
			trigs[numtrigs]._ty = 69;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 9;
			numtrigs++;
		}
		if (plr[myplr].pTownWarps & 4) {
			townwarps[2] = TRUE;
			trigs[numtrigs]._tx = 41;
			trigs[numtrigs]._ty = 80;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 13;
			numtrigs++;
		}
	}

	trigflag = FALSE;
}

void InitL1Triggers()
{
	int j, i;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 129) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}
			if (dPiece[i][j] == 115) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = 0;
}

void InitL2Triggers()
{
	int i, j;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 267 && (i != quests[QTYPE_BONE]._qtx || j != quests[QTYPE_BONE]._qty)) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}

			if (dPiece[i][j] == 559) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				trigs[numtrigs]._tlvl = 0;
				numtrigs++;
			}

			if (dPiece[i][j] == 271) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = 0;
}

void InitL3Triggers()
{
	int i, j;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 171) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}

			if (dPiece[i][j] == 168) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}

			if (dPiece[i][j] == 549) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				numtrigs++;
			}
		}
	}
	trigflag = 0;
}

void InitL4Triggers()
{
	int i, j;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 83) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
				numtrigs++;
			}

			if (dPiece[i][j] == 422) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
				trigs[numtrigs]._tlvl = 0;
				numtrigs++;
			}

			if (dPiece[i][j] == 120) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 370 && quests[QTYPE_VB]._qactive == 3) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = 0;
}

void InitSKingTriggers()
{
	trigflag = 0;
	numtrigs = 1;
	trigs[0]._tx = 82;
	trigs[0]._ty = 42;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitSChambTriggers()
{
	trigflag = 0;
	numtrigs = 1;
	trigs[0]._tx = 70;
	trigs[0]._ty = 39;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitPWaterTriggers()
{
	trigflag = 0;
	numtrigs = 1;
	trigs[0]._tx = 30;
	trigs[0]._ty = 83;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitVPTriggers()
{
	trigflag = 0;
	numtrigs = 1;
	trigs[0]._tx = 35;
	trigs[0]._ty = 32;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

BOOL ForceTownTrig()
{
	int i, j, k, l;

	for (i = 0; TownDownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == TownDownList[i]) {
			strcpy(infostr, "Down to dungeon");
			cursmx = 25;
			cursmy = 29;
			return TRUE;
		}
	}

	if (townwarps[0]) {
		for (j = 0; TownWarp1List[j] != -1; j++) {
			if (dPiece[cursmx][cursmy] == TownWarp1List[j]) {
				strcpy(infostr, "Down to catacombs");
				cursmx = 49;
				cursmy = 21;
				return TRUE;
			}
		}
	}

	if (townwarps[1]) {
		for (k = 1199; k <= 1220; k++) {
			if (dPiece[cursmx][cursmy] == k) {
				strcpy(infostr, "Down to caves");
				cursmx = 17;
				cursmy = 69;
				return TRUE;
			}
		}
	}

	if (townwarps[2]) {
		for (l = 1240; l <= 1255; l++) {
			if (dPiece[cursmx][cursmy] == l) {
				strcpy(infostr, "Down to hell");
				cursmx = 41;
				cursmy = 80;
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL ForceL1Trig()
{
	int i, j;

	for (i = 0; L1UpList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L1UpList[i]) {
			if (currlevel > 1)
				sprintf(infostr, "Up to level %i", currlevel - 1);
			else
				strcpy(infostr, "Up to town");
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return TRUE;
				}
			}
		}
	}

	for (i = 0; L1DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L1DownList[i]) {
			sprintf(infostr, "Down to level %i", currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL ForceL2Trig()
{
	int i, j, dx, dy;

	for (i = 0; L2UpList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L2UpList[i]) {
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					dx = abs(trigs[j]._tx - cursmx);
					dy = abs(trigs[j]._ty - cursmy);
					if (dx < 4 && dy < 4) {
						sprintf(infostr, "Up to level %i", currlevel - 1);
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return TRUE;
					}
				}
			}
		}
	}

	for (i = 0; L2DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L2DownList[i]) {
			sprintf(infostr, "Down to level %i", currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return TRUE;
				}
			}
		}
	}

	if (currlevel == 5) {
		for (i = 0; L2TWarpUpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L2TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						dx = abs(trigs[j]._tx - cursmx);
						dy = abs(trigs[j]._ty - cursmy);
						if (dx < 4 && dy < 4) {
							strcpy(infostr, "Up to town");
							cursmx = trigs[j]._tx;
							cursmy = trigs[j]._ty;
							return TRUE;
						}
					}
				}
			}
		}
	}

	return FALSE;
}

BOOL ForceL3Trig()
{
	int i, j, dx, dy;

	for (i = 0; L3UpList[i] != -1; ++i) {
		if (dPiece[cursmx][cursmy] == L3UpList[i]) {
			sprintf(infostr, "Up to level %i", currlevel - 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return TRUE;
				}
			}
		}
	}

	for (i = 0; L3DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L3DownList[i] || dPiece[cursmx + 1][cursmy] == L3DownList[i] || dPiece[cursmx + 2][cursmy] == L3DownList[i]) {
			sprintf(infostr, "Down to level %i", currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return TRUE;
				}
			}
		}
	}

	if (currlevel == 9) {
		for (i = 0; L3TWarpUpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L3TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						dx = abs(trigs[j]._tx - cursmx);
						dy = abs(trigs[j]._ty - cursmy);
						if (dx < 4 && dy < 4) {
							strcpy(infostr, "Up to town");
							cursmx = trigs[j]._tx;
							cursmy = trigs[j]._ty;
							return TRUE;
						}
					}
				}
			}
		}
	}

	return FALSE;
}

BOOL ForceL4Trig()
{
	int i, j, dx, dy;

	for (i = 0; L4UpList[i] != -1; ++i) {
		if (dPiece[cursmx][cursmy] == L4UpList[i]) {
			sprintf(infostr, "Up to level %i", currlevel - 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABPREVLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return TRUE;
				}
			}
		}
	}

	for (i = 0; L4DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L4DownList[i]) {
			sprintf(infostr, "Down to level %i", currlevel + 1);
			for (j = 0; j < numtrigs; j++) {
				if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
					cursmx = trigs[j]._tx;
					cursmy = trigs[j]._ty;
					return TRUE;
				}
			}
		}
	}

	if (currlevel == 13) {
		for (i = 0; L4TWarpUpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L4TWarpUpList[i]) {
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABTWARPUP) {
						dx = abs(trigs[j]._tx - cursmx);
						dy = abs(trigs[j]._ty - cursmy);
						if (dx < 4 && dy < 4) {
							strcpy(infostr, "Up to town");
							cursmx = trigs[j]._tx;
							cursmy = trigs[j]._ty;
							return TRUE;
						}
					}
				}
			}
		}
	}

	if (currlevel == 15) {
		for (i = 0; L4PentaList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L4PentaList[i]) {
				strcpy(infostr, "Down to Diablo");
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

void Freeupstairs()
{
	int i, yy, xx, tx, ty;

	for (i = 0; i < numtrigs; i++) {
		tx = trigs[i]._tx;
		ty = trigs[i]._ty;

		for (yy = 0; yy < MAXTRIGGERS; yy++) {
			for (xx = 0; xx < MAXTRIGGERS; xx++) {
				dFlags[tx - 2 + xx][ty - 2 + yy] |= BFLAG_POPULATED;
			}
		}
	}
}

BOOL ForceSKingTrig()
{
	int i;

	for (i = 0; L1UpList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L1UpList[i]) {
			sprintf(infostr, "Back to Level %i", quests[QTYPE_KING]._qlevel);
			cursmx = trigs[0]._tx;
			cursmy = trigs[0]._ty;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL ForceSChambTrig()
{
	int i;

	for (i = 0; L2DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L2DownList[i]) {
			sprintf(infostr, "Back to Level %i", quests[QTYPE_BONE]._qlevel);
			cursmx = trigs[0]._tx;
			cursmy = trigs[0]._ty;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL ForcePWaterTrig()
{
	int i;

	for (i = 0; L3DownList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L3DownList[i]) {
			sprintf(infostr, "Back to Level %i", quests[QTYPE_PW]._qlevel);
			cursmx = trigs[0]._tx;
			cursmy = trigs[0]._ty;

			return TRUE;
		}
	}

	return FALSE;
}

void CheckTrigForce()
{
	trigflag = FALSE;

	if (MouseY > PANEL_TOP - 1) {
		return;
	}

	if (!setlevel) {
		switch (leveltype) {
		case DTYPE_TOWN:
			trigflag = ForceTownTrig();
			break;
		case DTYPE_CATHEDRAL:
			trigflag = ForceL1Trig();
			break;
		case DTYPE_CATACOMBS:
			trigflag = ForceL2Trig();
			break;
		case DTYPE_CAVES:
			trigflag = ForceL3Trig();
			break;
		case DTYPE_HELL:
			trigflag = ForceL4Trig();
			break;
		}
		if (leveltype != DTYPE_TOWN && !trigflag) {
			trigflag = ForceQuests();
		}
	} else {
		switch (setlvlnum) {
		case SL_SKELKING:
			trigflag = ForceSKingTrig();
			break;
		case SL_BONECHAMB:
			trigflag = ForceSChambTrig();
			break;
		case SL_POISONWATER:
			trigflag = ForcePWaterTrig();
			break;
		}
	}

	if (trigflag) {
		ClearPanel();
	}
}

void CheckTriggers()
{
	int x, y, i;
	BOOL abort;
	char abortflag;

	if (plr[myplr]._pmode)
		return;

	for (i = 0; i < numtrigs; i++) {
		if (plr[myplr].WorldX != trigs[i]._tx || plr[myplr].WorldY != trigs[i]._ty) {
			continue;
		}

		switch (trigs[i]._tmsg) {
		case WM_DIABNEXTLVL:
			if (pcurs >= CURSOR_FIRSTITEM && DropItemBeforeTrig())
				return;
			StartNewLvl(myplr, trigs[i]._tmsg, currlevel + 1);
			break;
		case WM_DIABPREVLVL:
			if (pcurs >= CURSOR_FIRSTITEM && DropItemBeforeTrig())
				return;
			StartNewLvl(myplr, trigs[i]._tmsg, currlevel - 1);
			break;
		case WM_DIABRTNLVL:
			StartNewLvl(myplr, trigs[i]._tmsg, ReturnLvl);
			break;
		case WM_DIABTOWNWARP:
			if (gbMaxPlayers != 1) {
				abort = FALSE;

				if (trigs[i]._tlvl == 5 && plr[myplr]._pLevel < 8) {
					abort = TRUE;
					x = plr[myplr].WorldX;
					y = plr[myplr].WorldY + 1;
					abortflag = EMSG_REQUIRES_LVL_8;
				}

				if (trigs[i]._tlvl == 9 && plr[myplr]._pLevel < 13) {
					abort = TRUE;
					x = plr[myplr].WorldX + 1;
					y = plr[myplr].WorldY;
					abortflag = EMSG_REQUIRES_LVL_13;
				}

				if (trigs[i]._tlvl == 13 && plr[myplr]._pLevel < 17) {
					abort = TRUE;
					x = plr[myplr].WorldX;
					y = plr[myplr].WorldY + 1;
					abortflag = EMSG_REQUIRES_LVL_17;
				}

				if (abort) {
					if (plr[myplr]._pClass == PC_WARRIOR) {
						PlaySFX(PS_WARR43);
					} else if (plr[myplr]._pClass == PC_ROGUE) {
						PlaySFX(PS_ROGUE43);
					} else if (plr[myplr]._pClass == PC_SORCERER) {
						PlaySFX(PS_MAGE43);
					}

					InitDiabloMsg(abortflag);
					NetSendCmdLoc(TRUE, CMD_WALKXY, x, y);
					return;
				}
			}

			StartNewLvl(myplr, trigs[i]._tmsg, trigs[i]._tlvl);
			break;
		case WM_DIABTWARPUP:
			TWarpFrom = currlevel;
			StartNewLvl(myplr, trigs[i]._tmsg, 0);
			break;
		default:
			app_fatal("Unknown trigger msg");
			break;
		}
	}
}

DEVILUTION_END_NAMESPACE
