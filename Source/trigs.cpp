/**
 * @file trigs.cpp
 *
 * Implementation of functionality for triggering events when the player enters an area.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

BOOL townwarps[3];
BOOL trigflag;
int numtrigs;
TriggerStruct trigs[MAXTRIGGERS];
int TWarpFrom;

/** Specifies the dungeon piece IDs which constitute stairways leading down to the cathedral from town. */
int TownDownList[] = { 716, 715, 719, 720, 721, 723, 724, 725, 726, 727, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down to the catacombs from town. */
int TownWarp1List[] = { 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1181, 1183, 1185, -1 };
int TownCryptList[] = { 1331, 1332, 1333, 1334, 1335, 1336, 1337, 1338, -1 };
int TownHiveList[] = { 1307, 1308, 1309, 1310, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up from the cathedral. */
int L1UpList[] = { 127, 129, 130, 131, 132, 133, 135, 137, 138, 139, 140, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down from the cathedral. */
int L1DownList[] = { 106, 107, 108, 109, 110, 112, 114, 115, 118, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up from the catacombs. */
int L2UpList[] = { 266, 267, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down from the catacombs. */
int L2DownList[] = { 269, 270, 271, 272, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up to town from the catacombs. */
int L2TWarpUpList[] = { 558, 559, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up from the caves. */
int L3UpList[] = { 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down from the caves. */
int L3DownList[] = { 162, 163, 164, 165, 166, 167, 168, 169, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up to town from the caves. */
int L3TWarpUpList[] = { 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up from hell. */
int L4UpList[] = { 82, 83, 90, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down from hell. */
int L4DownList[] = { 120, 130, 131, 132, 133, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading up to town from hell. */
int L4TWarpUpList[] = { 421, 422, 429, -1 };
/** Specifies the dungeon piece IDs which constitute stairways leading down to Diablo from hell. */
int L4PentaList[] = { 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, -1 };
int L5TWarpUpList[] = { 172, 173, 174, 175, 176, 177, 178, 179, 184, -1 };
int L5UpList[] = { 149, 150, 151, 152, 153, 154, 155, 157, 158, 159, -1 };
int L5DownList[] = { 125, 126, 129, 131, 132, 135, 136, 140, 142, -1 };
int L6TWarpUpList[] = { 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, -1 };
int L6UpList[] = { 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, -1 };
int L6DownList[] = { 57, 58, 59, 60, 61, 62, 63, 64, -1 };

void InitNoTriggers()
{
	numtrigs = 0;
	trigflag = FALSE;
}

void InitTownTriggers()
{
	int i;

	numtrigs = 0;

	trigs[numtrigs]._tx = 25;
	trigs[numtrigs]._ty = 29;
	trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
	numtrigs++;

	bool isMultiplayer = gbMaxPlayers != 1;

	for (i = 0; i < sizeof(townwarps) / sizeof(townwarps[0]); i++) {
		townwarps[i] = isMultiplayer && !gbIsSpawn;
	}
	if (!gbIsSpawn) {
		if (isMultiplayer || plr[myplr].pTownWarps & 1 || (gbIsHellfire && plr[myplr]._pLevel >= 10)) {
			townwarps[0] = TRUE;
			trigs[numtrigs]._tx = 49;
			trigs[numtrigs]._ty = 21;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 5;
			numtrigs++;
		}
		if (isMultiplayer || plr[myplr].pTownWarps & 2 || (gbIsHellfire && plr[myplr]._pLevel >= 15)) {
			townwarps[1] = TRUE;
			trigs[numtrigs]._tx = 17;
			trigs[numtrigs]._ty = 69;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 9;
			numtrigs++;
		}
		if (isMultiplayer || plr[myplr].pTownWarps & 4 || (gbIsHellfire && plr[myplr]._pLevel >= 20)) {
			townwarps[2] = TRUE;
			trigs[numtrigs]._tx = 41;
			trigs[numtrigs]._ty = 80;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 13;
			numtrigs++;
		}
	}
	if (gbIsHellfire) {
		trigs[numtrigs]._tx = 80;
		trigs[numtrigs]._ty = 62;
		trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
		trigs[numtrigs]._tlvl = 17;
		numtrigs++;
		if (isMultiplayer || quests[Q_GRAVE]._qactive == 3) {
			trigs[numtrigs]._tx = 36;
			trigs[numtrigs]._ty = 24;
			trigs[numtrigs]._tmsg = WM_DIABTOWNWARP;
			trigs[numtrigs]._tlvl = 21;
			numtrigs++;
		}
	}

	trigflag = FALSE;
}

void InitL1Triggers()
{
	int i, j;

	numtrigs = 0;
	if (currlevel < 17) {
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
	} else {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] == 184) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
					trigs[numtrigs]._tlvl = 0;
					numtrigs++;
				}
				if (dPiece[i][j] == 158) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}
				if (dPiece[i][j] == 126) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}
			}
		}
	}
	trigflag = FALSE;
}

void InitL2Triggers()
{
	int i, j;

	numtrigs = 0;
	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 267 && (i != quests[Q_SCHAMB]._qtx || j != quests[Q_SCHAMB]._qty)) {
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
	trigflag = FALSE;
}

void InitL3Triggers()
{
	int i, j;

	if (currlevel < 17) {
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
	} else {
		numtrigs = 0;
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] == 66) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABPREVLVL;
					numtrigs++;
				}

				if (dPiece[i][j] == 63) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
					numtrigs++;
				}

				if (dPiece[i][j] == 80) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABTWARPUP;
					numtrigs++;
				}
			}
		}
	}
	trigflag = FALSE;
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
			if (dPiece[i][j] == 370 && quests[Q_BETRAYER]._qactive == QUEST_DONE) {
				trigs[numtrigs]._tx = i;
				trigs[numtrigs]._ty = j;
				trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
				numtrigs++;
			}
		}
	}
	trigflag = FALSE;
}

void InitSKingTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._tx = 82;
	trigs[0]._ty = 42;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitSChambTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._tx = 70;
	trigs[0]._ty = 39;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitPWaterTriggers()
{
	trigflag = FALSE;
	numtrigs = 1;
	trigs[0]._tx = 30;
	trigs[0]._ty = 83;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void InitVPTriggers()
{
	trigflag = FALSE;
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

	if (gbIsHellfire) {
		for (i = 0; TownCryptList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == TownCryptList[i]) {
				strcpy(infostr, "Down to Crypt");
				cursmx = 36;
				cursmy = 24;
				return TRUE;
			}
		}
		for (i = 0; TownHiveList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == TownHiveList[i]) {
				strcpy(infostr, "Down to Hive");
				cursmx = 80;
				cursmy = 62;
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL ForceL1Trig()
{
	int i, j;
	int dx, dy;

	if (currlevel < 17) {
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
	} else {
		for (i = 0; L5UpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L5UpList[i]) {
				sprintf(infostr, "Up to Crypt level %i", currlevel - 21);
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABPREVLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return TRUE;
					}
				}
			}
		}
		if (dPiece[cursmx][cursmy] == 317) {
			strcpy(infostr, "Cornerstone of the World");
			return TRUE;
		}
		for (i = 0; L5DownList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L5DownList[i]) {
				sprintf(infostr, "Down to Crypt level %i", currlevel - 19);
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABNEXTLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return TRUE;
					}
				}
			}
		}
		if (currlevel == 21) {
			for (i = 0; L5TWarpUpList[i] != -1; i++) {
				if (dPiece[cursmx][cursmy] == L5TWarpUpList[i]) {
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

	if (currlevel < 17) {
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
			if (dPiece[cursmx][cursmy] == L3DownList[i]
			    || dPiece[cursmx + 1][cursmy] == L3DownList[i]
			    || dPiece[cursmx + 2][cursmy] == L3DownList[i]) {
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
	} else {
		for (i = 0; L6UpList[i] != -1; ++i) {
			if (dPiece[cursmx][cursmy] == L6UpList[i]) {
				sprintf(infostr, "Up to Nest level %i", currlevel - 17);
				for (j = 0; j < numtrigs; j++) {
					if (trigs[j]._tmsg == WM_DIABPREVLVL) {
						cursmx = trigs[j]._tx;
						cursmy = trigs[j]._ty;
						return TRUE;
					}
				}
			}
		}
		for (i = 0; L6DownList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L6DownList[i]
			    || dPiece[cursmx + 1][cursmy] == L6DownList[i]
			    || dPiece[cursmx + 2][cursmy] == L6DownList[i]) {
				sprintf(infostr, "Down to level %i", currlevel - 15);
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
	if (currlevel == 17) {
		for (i = 0; L6TWarpUpList[i] != -1; i++) {
			if (dPiece[cursmx][cursmy] == L6TWarpUpList[i]) {
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
	int i, tx, ty, xx, yy;

	for (i = 0; i < numtrigs; i++) {
		tx = trigs[i]._tx;
		ty = trigs[i]._ty;

		for (yy = -2; yy <= 2; yy++) {
			for (xx = -2; xx <= 2; xx++) {
				dFlags[tx + xx][ty + yy] |= BFLAG_POPULATED;
			}
		}
	}
}

BOOL ForceSKingTrig()
{
	int i;

	for (i = 0; L1UpList[i] != -1; i++) {
		if (dPiece[cursmx][cursmy] == L1UpList[i]) {
			sprintf(infostr, "Back to Level %i", quests[Q_SKELKING]._qlevel);
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
			sprintf(infostr, "Back to Level %i", quests[Q_SCHAMB]._qlevel);
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
			sprintf(infostr, "Back to Level %i", quests[Q_PWATER]._qlevel);
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

	if (!sgbControllerActive && MouseY > PANEL_TOP - 1) {
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

	if (plr[myplr]._pmode != PM_STAND)
		return;

	for (i = 0; i < numtrigs; i++) {
		if (plr[myplr]._px != trigs[i]._tx || plr[myplr]._py != trigs[i]._ty) {
			continue;
		}

		switch (trigs[i]._tmsg) {
		case WM_DIABNEXTLVL:
			if (gbIsSpawn && currlevel >= 2) {
				NetSendCmdLoc(TRUE, CMD_WALKXY, plr[myplr]._px, plr[myplr]._py + 1);
				PlaySFX(PS_WARR18);
				InitDiabloMsg(EMSG_NOT_IN_SHAREWARE);
			} else {
				if (pcurs >= CURSOR_FIRSTITEM && DropItemBeforeTrig())
					return;
				StartNewLvl(myplr, trigs[i]._tmsg, currlevel + 1);
			}
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
					x = plr[myplr]._px;
					y = plr[myplr]._py + 1;
					abortflag = EMSG_REQUIRES_LVL_8;
				}

				if (trigs[i]._tlvl == 9 && plr[myplr]._pLevel < 13) {
					abort = TRUE;
					x = plr[myplr]._px + 1;
					y = plr[myplr]._py;
					abortflag = EMSG_REQUIRES_LVL_13;
				}

				if (trigs[i]._tlvl == 13 && plr[myplr]._pLevel < 17) {
					abort = TRUE;
					x = plr[myplr]._px;
					y = plr[myplr]._py + 1;
					abortflag = EMSG_REQUIRES_LVL_17;
				}

				if (abort) {
					if (plr[myplr]._pClass == PC_WARRIOR) {
						PlaySFX(PS_WARR43);
					} else if (plr[myplr]._pClass == PC_ROGUE) {
						PlaySFX(PS_ROGUE43);
					} else if (plr[myplr]._pClass == PC_SORCERER) {
						PlaySFX(PS_MAGE43);
					} else if (plr[myplr]._pClass == PC_MONK) {
						PlaySFX(PS_MONK43);
					} else if (plr[myplr]._pClass == PC_BARD) {
						PlaySFX(PS_ROGUE43);
					} else if (plr[myplr]._pClass == PC_BARBARIAN) {
						PlaySFX(PS_WARR43);
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
