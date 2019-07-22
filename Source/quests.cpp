#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

int qtopline;
BOOL questlog;
BYTE *pQLogCel;
QuestStruct quests[MAXQUESTS];
int qline;
int qlist[MAXQUESTS];
int numqlines;
int WaterDone;
int ReturnLvlY;
int ReturnLvlX;
int ReturnLvlT;
int ALLQUESTS;
int ReturnLvl;

QuestData questlist[MAXQUESTS] = {
	// clang-format off
	// _qdlvl, _qdmultlvl, _qlvlt,          _qdtype,   _qdrnd, _qslvl, _qflags, _qdmsg,         _qlstr
	{       5,         -1, DTYPE_NONE,      QTYPE_INFRA,  100,      0,       0, QUEST_INFRA5,   "The Magic Rock"           },
	{       9,         -1, DTYPE_NONE,      QTYPE_BLKM,   100,      0,       0, QUEST_MUSH8,    "Black Mushroom"           },
	{       4,         -1, DTYPE_NONE,      QTYPE_GARB,   100,      0,       0, QUEST_GARBUD1,  "Gharbad The Weak"         },
	{       8,         -1, DTYPE_NONE,      QTYPE_ZHAR,   100,      0,       0, QUEST_ZHAR1,    "Zhar the Mad"             },
	{      14,         -1, DTYPE_NONE,      QTYPE_VEIL,   100,      0,       0, QUEST_VEIL9,    "Lachdanan"                },
	{      15,         -1, DTYPE_NONE,      QTYPE_MOD,    100,      0,       1, QUEST_VILE3,    "Diablo"                   },
	{       2,          2, DTYPE_NONE,      QTYPE_BUTCH,  100,      0,       1, QUEST_BUTCH9,   "The Butcher"              },
	{       4,         -1, DTYPE_NONE,      QTYPE_BOL,    100,      0,       0, QUEST_BANNER2,  "Ogden's Sign"             },
	{       7,         -1, DTYPE_NONE,      QTYPE_BLIND,  100,      0,       0, QUEST_BLINDING, "Halls of the Blind"       },
	{       5,         -1, DTYPE_NONE,      QTYPE_BLOOD,  100,      0,       0, QUEST_BLOODY,   "Valor"                    },
	{      10,         -1, DTYPE_NONE,      QTYPE_ANVIL,  100,      0,       0, QUEST_ANVIL5,   "Anvil of Fury"            },
	{      13,         -1, DTYPE_NONE,      QTYPE_WARLRD, 100,      0,       0, QUEST_BLOODWAR, "Warlord of Blood"         },
	{       3,          3, DTYPE_CATHEDRAL, QTYPE_KING,   100,      1,       1, QUEST_KING2,    "The Curse of King Leoric" },
	{       2,         -1, DTYPE_CAVES,     QTYPE_PW,     100,      4,       0, QUEST_POISON3,  "Poisoned Water Supply"    },
	{       6,         -1, DTYPE_CATACOMBS, QTYPE_BONE,   100,      2,       0, QUEST_BONER,    "The Chamber of Bone"      },
	{      15,         15, DTYPE_CATHEDRAL, QTYPE_VB,     100,      5,       1, QUEST_VILE1,    "Archbishop Lazarus"       }
	// clang-format on
};
char questxoff[7] = { 0, -1, 0, -1, -2, -1, -2 };
char questyoff[7] = { 0, 0, -1, -1, -1, -2, -2 };
char *questtrigstr[5] = {
	"King Leoric's Tomb",
	"The Chamber of Bone",
	"Maze",
	"A Dark Passage",
	"Unholy Altar"
};
int QuestGroup1[3] = { QTYPE_BUTCH, QTYPE_BOL, QTYPE_GARB };
int QuestGroup2[3] = { QTYPE_BLIND, QTYPE_INFRA, QTYPE_BLOOD };
int QuestGroup3[3] = { QTYPE_BLKM, QTYPE_ZHAR, QTYPE_ANVIL };
int QuestGroup4[2] = { QTYPE_VEIL, QTYPE_WARLRD };

void InitQuests()
{
	int i, initiatedQuests;
	DWORD z;

	if (gbMaxPlayers == 1) {
		for (i = 0; i < MAXQUESTS; i++) {
			quests[i]._qactive = 0;
		}
	} else {
		for (i = 0; i < MAXQUESTS; i++) {
			if (!(questlist[i]._qflags & 1)) {
				quests[i]._qactive = 0;
			}
		}
	}

	initiatedQuests = 0;
	questlog = FALSE;
	ALLQUESTS = 1;
	WaterDone = 0;

	for (z = 0; z < MAXQUESTS; z++) {
		if (gbMaxPlayers <= 1 || questlist[z]._qflags & 1) {
			quests[z]._qtype = questlist[z]._qdtype;
			if (gbMaxPlayers > 1) {
				quests[z]._qlevel = questlist[z]._qdmultlvl;
				if (!delta_quest_inited(initiatedQuests)) {
					quests[z]._qactive = 1;
					quests[z]._qvar1 = 0;
					quests[z]._qlog = 0;
				}
				initiatedQuests++;
			} else {
				quests[z]._qactive = 1;
				quests[z]._qlevel = questlist[z]._qdlvl;
				quests[z]._qvar1 = 0;
				quests[z]._qlog = 0;
			}

			quests[z]._qtx = 0;
			quests[z]._qslvl = questlist[z]._qslvl;
			quests[z]._qidx = z;
			quests[z]._qlvltype = questlist[z]._qlvlt;
			quests[z]._qty = 0;
			quests[z]._qvar2 = 0;
			quests[z]._qmsg = questlist[z]._qdmsg;
		}
	}

	if (gbMaxPlayers == 1) {
		SetRndSeed(glSeedTbl[15]);
		if (random(0, 2))
			quests[QTYPE_PW]._qactive = 0;
		else
			quests[QTYPE_KING]._qactive = 0;

		quests[QuestGroup1[random(0, sizeof(QuestGroup1) / sizeof(int))]]._qactive = 0;
		quests[QuestGroup2[random(0, sizeof(QuestGroup2) / sizeof(int))]]._qactive = 0;
		quests[QuestGroup3[random(0, sizeof(QuestGroup3) / sizeof(int))]]._qactive = 0;
		quests[QuestGroup4[random(0, sizeof(QuestGroup4) / sizeof(int))]]._qactive = 0;
	}
#ifdef _DEBUG
	if (questdebug != -1)
		quests[questdebug]._qactive = 2;
#endif
	if (!quests[QTYPE_KING]._qactive)
		quests[QTYPE_KING]._qvar2 = 2;
	if (!quests[QTYPE_INFRA]._qactive)
		quests[QTYPE_INFRA]._qvar2 = 2;
	quests[QTYPE_BOL]._qvar1 = 1;
	if (gbMaxPlayers != 1)
		quests[QTYPE_VB]._qvar1 = 2;
}

void CheckQuests()
{
	int i, rportx, rporty;

	if (QuestStatus(QTYPE_VB) && gbMaxPlayers != 1 && quests[QTYPE_VB]._qvar1 == 2) {
		AddObject(OBJ_ALTBOY, 2 * setpc_x + 20, 2 * setpc_y + 22);
		quests[QTYPE_VB]._qvar1 = 3;
		NetSendCmdQuest(TRUE, QTYPE_VB);
	}

	if (gbMaxPlayers != 1) {
		return;
	}

	if (currlevel == quests[QTYPE_VB]._qlevel
	    && !setlevel
	    && quests[QTYPE_VB]._qvar1 >= 2
	    && (quests[QTYPE_VB]._qactive == 2 || quests[QTYPE_VB]._qactive == 3)
	    && (quests[QTYPE_VB]._qvar2 == 0 || quests[QTYPE_VB]._qvar2 == 2)) {
		quests[QTYPE_VB]._qtx = 2 * quests[QTYPE_VB]._qtx + 16;
		quests[QTYPE_VB]._qty = 2 * quests[QTYPE_VB]._qty + 16;
		rportx = quests[QTYPE_VB]._qtx;
		rporty = quests[QTYPE_VB]._qty;
		AddMissile(rportx, rporty, rportx, rporty, 0, MIS_RPORTAL, 0, myplr, 0, 0);
		quests[QTYPE_VB]._qvar2 = 1;
		if (quests[QTYPE_VB]._qactive == 2) {
			quests[QTYPE_VB]._qvar1 = 3;
		}
	}

	if (quests[QTYPE_VB]._qactive == 3
	    && setlevel
	    && setlvlnum == SL_VILEBETRAYER
	    && quests[QTYPE_VB]._qvar2 == 4) {
		rportx = 35;
		rporty = 32;
		AddMissile(rportx, rporty, rportx, rporty, 0, MIS_RPORTAL, 0, myplr, 0, 0);
		quests[QTYPE_VB]._qvar2 = 3;
	}

	if (setlevel) {
		if (setlvlnum == quests[QTYPE_PW]._qslvl
		    && quests[QTYPE_PW]._qactive != 1
		    && leveltype == quests[QTYPE_PW]._qlvltype
		    && nummonsters == 4
		    && quests[QTYPE_PW]._qactive != 3) {
			quests[QTYPE_PW]._qactive = 3;
			PlaySfxLoc(IS_QUESTDN, plr[myplr].WorldX, plr[myplr].WorldY);
			LoadPalette("Levels\\L3Data\\L3pwater.pal");
			WaterDone = 32;
		}
		if (WaterDone > 0) {
			palette_update_quest_palette(WaterDone);
			WaterDone--;
		}
	} else if (plr[myplr]._pmode == PM_STAND) {
		for (i = 0; i < MAXQUESTS; i++) {
			if (currlevel == quests[i]._qlevel
			    && quests[i]._qslvl != 0
			    && quests[i]._qactive != 0
			    && plr[myplr].WorldX == quests[i]._qtx
			    && plr[myplr].WorldY == quests[i]._qty) {
				if (quests[i]._qlvltype != 255) {
					setlvltype = quests[i]._qlvltype;
				}
				StartNewLvl(myplr, WM_DIABSETLVL, quests[i]._qslvl);
			}
		}
	}
}

BOOL ForceQuests()
{
	int i, j, qx, qy, ql;

	if (gbMaxPlayers != 1) {
		return FALSE;
	}

	for (i = 0; i < MAXQUESTS; i++) {

		if (i != QTYPE_VB && currlevel == quests[i]._qlevel && quests[i]._qslvl != 0) {
			ql = quests[quests[i]._qidx]._qslvl - 1;
			qx = quests[i]._qtx;
			qy = quests[i]._qty;

			for (j = 0; j < 7; j++) {
				if (qx + questxoff[j] == cursmx && qy + questyoff[j] == cursmy) {
					sprintf(infostr, "To %s", questtrigstr[ql]);
					cursmx = qx;
					cursmy = qy;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL QuestStatus(int i)
{
	BOOL result;

	if (setlevel
	    || currlevel != quests[i]._qlevel
	    || !quests[i]._qactive
	    || (result = 1, gbMaxPlayers != 1) && !(questlist[i]._qflags & 1)) {
		result = FALSE;
	}
	return result;
}

void CheckQuestKill(int m, BOOL sendmsg)
{
	int i, j;

	if (monster[m].MType->mtype == MT_SKING) {
		quests[QTYPE_KING]._qactive = 3;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR82;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE82;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE82;
		}
		if (sendmsg)
			NetSendCmdQuest(TRUE, QTYPE_KING);

	} else if (monster[m].MType->mtype == MT_CLEAVER) {
		quests[QTYPE_BUTCH]._qactive = 3;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR80;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE80;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE80;
		}
		if (sendmsg)
			NetSendCmdQuest(TRUE, QTYPE_BUTCH);
	} else if (monster[m].mName == UniqMonst[UMT_GARBUD].mName) { //"Gharbad the Weak"
		quests[QTYPE_GARB]._qactive = 3;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR61;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE61;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE61;
		}
	} else if (monster[m].mName == UniqMonst[UMT_ZHAR].mName) { //"Zhar the Mad"
		quests[QTYPE_ZHAR]._qactive = 3;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR62;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE62;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE62;
		}
	} else if (monster[m].mName == UniqMonst[UMT_LAZURUS].mName && gbMaxPlayers != 1) { //"Arch-Bishop Lazarus"
		quests[QTYPE_VB]._qactive = 3;
		quests[QTYPE_VB]._qvar1 = 7;
		sfxdelay = 30;
		quests[QTYPE_MOD]._qactive = 2;

		for (j = 0; j < 112; j++) {
			for (i = 0; i < 112; i++) {
				if (dPiece[i][j] == 370) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = 1026;
					numtrigs++;
				}
			}
		}
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR83;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE83;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE83;
		}
		if (sendmsg) {
			NetSendCmdQuest(TRUE, QTYPE_VB);
			NetSendCmdQuest(TRUE, QTYPE_MOD);
		}
	} else if (monster[m].mName == UniqMonst[UMT_LAZURUS].mName && gbMaxPlayers == 1) { //"Arch-Bishop Lazarus"
		quests[QTYPE_VB]._qactive = 3;
		sfxdelay = 30;
		InitVPTriggers();
		quests[QTYPE_VB]._qvar1 = 7;
		quests[QTYPE_VB]._qvar2 = 4;
		quests[QTYPE_MOD]._qactive = 2;
		AddMissile(35, 32, 35, 32, 0, MIS_RPORTAL, 0, myplr, 0, 0);
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR83;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE83;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE83;
		}
	} else if (monster[m].mName == UniqMonst[UMT_WARLORD].mName) { //"Warlord of Blood"
		quests[QTYPE_WARLRD]._qactive = 3;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR94;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE94;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE94;
		}
	}
}

void DrawButcher()
{
	int x, y;

	x = 2 * setpc_x + 16;
	y = 2 * setpc_y + 16;
	DRLG_RectTrans(x + 3, y + 3, x + 10, y + 10);
}

void DrawSkelKing(int q, int x, int y)
{
	quests[q]._qtx = 2 * x + 28;
	quests[q]._qty = 2 * y + 23;
}

void DrawWarLord(int x, int y)
{
	int rw, rh;
	int i, j;
	BYTE *sp, *setp;
	int v;

	setp = LoadFileInMem("Levels\\L4Data\\Warlord2.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_w = rw;
	setpc_h = rh;
	setpc_x = x;
	setpc_y = y;
	for (j = y; j < y + rh; j++) {
		for (i = x; i < x + rw; i++) {
			if (*sp != 0) {
				v = *sp;
			} else {
				v = 6;
			}
			dungeon[i][j] = v;
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DrawSChamber(int q, int x, int y)
{
	int i, j;
	int rw, rh;
	int xx, yy;
	BYTE *sp, *setp;
	int v;

	setp = LoadFileInMem("Levels\\L2Data\\Bonestr1.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_w = rw;
	setpc_h = rh;
	setpc_x = x;
	setpc_y = y;
	for (j = y; j < y + rh; j++) {
		for (i = x; i < x + rw; i++) {
			if (*sp != 0) {
				v = *sp;
			} else {
				v = 3;
			}
			dungeon[i][j] = v;
			sp += 2;
		}
	}
	xx = 2 * x + 22;
	yy = 2 * y + 23;
	quests[q]._qtx = xx;
	quests[q]._qty = yy;
	mem_free_dbg(setp);
}

void DrawLTBanner(int x, int y)
{
	int rw, rh;
	int i, j;
	BYTE *sp, *setp;

	setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_w = rw;
	setpc_h = rh;
	setpc_x = x;
	setpc_y = y;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				pdungeon[x + i][y + j] = *sp;
			}
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DrawBlind(int x, int y)
{
	int rw, rh;
	int i, j;
	BYTE *sp, *setp;

	setp = LoadFileInMem("Levels\\L2Data\\Blind1.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_x = x;
	setpc_y = y;
	setpc_w = rw;
	setpc_h = rh;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				pdungeon[x + i][y + j] = *sp;
			}
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DrawBlood(int x, int y)
{
	int rw, rh;
	int i, j;
	BYTE *sp, *setp;

	setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
	rw = *setp;
	sp = setp + 2;
	rh = *sp;
	sp += 2;
	setpc_x = x;
	setpc_y = y;
	setpc_w = rw;
	setpc_h = rh;
	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*sp != 0) {
				dungeon[x + i][y + j] = *sp;
			}
			sp += 2;
		}
	}
	mem_free_dbg(setp);
}

void DRLG_CheckQuests(int x, int y)
{
	int i;

	for (i = 0; i < MAXQUESTS; i++) {
		if (QuestStatus(i)) {
			switch (quests[i]._qtype) {
			case QTYPE_BUTCH:
				DrawButcher();
				break;
			case QTYPE_BOL:
				DrawLTBanner(x, y);
				break;
			case QTYPE_BLIND:
				DrawBlind(x, y);
				break;
			case QTYPE_BLOOD:
				DrawBlood(x, y);
				break;
			case QTYPE_WARLRD:
				DrawWarLord(x, y);
				break;
			case QTYPE_KING:
				DrawSkelKing(i, x, y);
				break;
			case QTYPE_BONE:
				DrawSChamber(i, x, y);
				break;
			}
		}
	}
}

void SetReturnLvlPos()
{
	switch (setlvlnum) {
	case SL_SKELKING:
		ReturnLvlX = quests[QTYPE_KING]._qtx + 1;
		ReturnLvlY = quests[QTYPE_KING]._qty;
		ReturnLvlT = DTYPE_CATHEDRAL;
		ReturnLvl = quests[QTYPE_KING]._qlevel;
		break;
	case SL_BONECHAMB:
		ReturnLvlX = quests[QTYPE_BONE]._qtx + 1;
		ReturnLvlY = quests[QTYPE_BONE]._qty;
		ReturnLvlT = DTYPE_CATACOMBS;
		ReturnLvl = quests[QTYPE_BONE]._qlevel;
		break;
	case SL_POISONWATER:
		ReturnLvlX = quests[QTYPE_PW]._qtx;
		ReturnLvlY = quests[QTYPE_PW]._qty + 1;
		ReturnLvlT = DTYPE_CATHEDRAL;
		ReturnLvl = quests[QTYPE_PW]._qlevel;
		break;
	case SL_VILEBETRAYER:
		ReturnLvlX = quests[QTYPE_VB]._qtx + 1;
		ReturnLvlY = quests[QTYPE_VB]._qty - 1;
		ReturnLvlT = DTYPE_HELL;
		ReturnLvl = quests[QTYPE_VB]._qlevel;
		break;
	}
}

void GetReturnLvlPos()
{
	if (quests[QTYPE_VB]._qactive == 3)
		quests[QTYPE_VB]._qvar2 = 2;
	ViewX = ReturnLvlX;
	ViewY = ReturnLvlY;
	currlevel = ReturnLvl;
	leveltype = ReturnLvlT;
}

void ResyncMPQuests()
{
	if (quests[QTYPE_KING]._qactive == 1
	    && currlevel >= quests[QTYPE_KING]._qlevel - 1
	    && currlevel <= quests[QTYPE_KING]._qlevel + 1) {
		quests[QTYPE_KING]._qactive = 2;
		NetSendCmdQuest(TRUE, QTYPE_KING);
	}
	if (quests[QTYPE_BUTCH]._qactive == 1
	    && currlevel >= quests[QTYPE_BUTCH]._qlevel - 1
	    && currlevel <= quests[QTYPE_BUTCH]._qlevel + 1) {
		quests[QTYPE_BUTCH]._qactive = 2;
		NetSendCmdQuest(TRUE, QTYPE_BUTCH);
	}
	if (quests[QTYPE_VB]._qactive == 1 && currlevel == quests[QTYPE_VB]._qlevel - 1) {
		quests[QTYPE_VB]._qactive = 2;
		NetSendCmdQuest(TRUE, QTYPE_VB);
	}
	if (QuestStatus(QTYPE_VB))
		AddObject(OBJ_ALTBOY, 2 * setpc_x + 20, 2 * setpc_y + 22);
}

void ResyncQuests()
{
	int i, tren, x, y;

	if (setlevel && setlvlnum == quests[QTYPE_PW]._qslvl && quests[QTYPE_PW]._qactive != 1 && leveltype == quests[QTYPE_PW]._qlvltype) {

		if (quests[QTYPE_PW]._qactive == 3)
			LoadPalette("Levels\\L3Data\\L3pwater.pal");
		else
			LoadPalette("Levels\\L3Data\\L3pfoul.pal");

		for (i = 0; i <= 32; i++)
			palette_update_quest_palette(i);
	}

	if (QuestStatus(QTYPE_BOL)) {
		if (quests[QTYPE_BOL]._qvar1 == 1)
			ObjChangeMapResync(
			    setpc_w + setpc_x - 2,
			    setpc_h + setpc_y - 2,
			    setpc_w + setpc_x + 1,
			    setpc_h + setpc_y + 1);
		if (quests[QTYPE_BOL]._qvar1 == 2) {
			ObjChangeMapResync(
			    setpc_w + setpc_x - 2,
			    setpc_h + setpc_y - 2,
			    setpc_w + setpc_x + 1,
			    setpc_h + setpc_y + 1);
			ObjChangeMapResync(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 2, (setpc_h >> 1) + setpc_y - 2);
			for (i = 0; i < nobjects; i++)
				SyncObjectAnim(objectactive[i]);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 4, setpc_y + (setpc_h >> 1));
			TransVal = tren;
		}
		if (quests[QTYPE_BOL]._qvar1 == 3) {
			x = setpc_x;
			y = setpc_y;
			ObjChangeMapResync(x, y, x + setpc_w + 1, y + setpc_h + 1);
			for (i = 0; i < nobjects; i++)
				SyncObjectAnim(objectactive[i]);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 4, setpc_y + (setpc_h >> 1));
			TransVal = tren;
		}
	}
	if (currlevel == quests[QTYPE_BLKM]._qlevel) {
		if (quests[QTYPE_BLKM]._qactive == 1) {
			if (!quests[QTYPE_BLKM]._qvar1) {
				SpawnQuestItem(IDI_FUNGALTM, 0, 0, 5, 1);
				quests[QTYPE_BLKM]._qvar1 = QS_TOMESPAWNED;
			}
		} else if (quests[QTYPE_BLKM]._qactive == 2) {
			if (quests[QTYPE_BLKM]._qvar1 >= QS_MUSHGIVEN) {
				Qtalklist[TOWN_WITCH]._qblkm = -1;
				Qtalklist[TOWN_HEALER]._qblkm = QUEST_MUSH3;
			} else if (quests[QTYPE_BLKM]._qvar1 >= QS_BRAINGIVEN) {
				Qtalklist[TOWN_HEALER]._qblkm = -1;
			}
		}
	}
	if (currlevel == quests[QTYPE_VEIL]._qlevel + 1 && quests[QTYPE_VEIL]._qactive == 2 && !quests[QTYPE_VEIL]._qvar1) {
		quests[QTYPE_VEIL]._qvar1 = 1;
		SpawnQuestItem(IDI_GLDNELIX, 0, 0, 5, 1);
	}
	if (setlevel && setlvlnum == 5) {
		if (quests[QTYPE_VB]._qvar1 >= 4)
			ObjChangeMapResync(1, 11, 20, 18);
		if (quests[QTYPE_VB]._qvar1 >= 6)
			ObjChangeMapResync(1, 18, 20, 24);
		if (quests[QTYPE_VB]._qvar1 >= 7)
			InitVPTriggers();
		for (i = 0; i < nobjects; i++)
			SyncObjectAnim(objectactive[i]);
	}
	if (currlevel == quests[QTYPE_VB]._qlevel
	    && !setlevel
	    && (quests[QTYPE_VB]._qvar2 == 1 || quests[QTYPE_VB]._qvar2 >= 3)
	    && (quests[QTYPE_VB]._qactive == 2 || quests[QTYPE_VB]._qactive == 3)) {
		quests[QTYPE_VB]._qvar2 = 2;
	}
}

void PrintQLString(int x, int y, BOOL cjustflag, char *str, int col)
{
	int len, width, off, i, k, s;
	BYTE c;

	s = SStringY[y];
	off = x + PitchTbl[SStringY[y] + 204] + 96;
	len = strlen(str);
	k = 0;
	if (cjustflag) {
		width = 0;
		for (i = 0; i < len; i++)
			width += fontkern[fontframe[gbFontTransTbl[(BYTE)str[i]]]] + 1;
		if (width < 257)
			k = (257 - width) >> 1;
		off += k;
	}
	if (qline == y) {
		CelDecodeOnly(cjustflag ? x + k + 76 : x + 76, s + 205, pSPentSpn2Cels, ALLQUESTS, 12);
	}
	for (i = 0; i < len; i++) {
		c = fontframe[gbFontTransTbl[(BYTE)str[i]]];
		k += fontkern[c] + 1;
		if (c && k <= 257) {
			CPrintString(off, c, col);
		}
		off += fontkern[c] + 1;
	}
	if (qline == y) {
		CelDecodeOnly(cjustflag ? x + k + 100 : 340 - x, s + 205, pSPentSpn2Cels, ALLQUESTS, 12);
	}
}

void DrawQuestLog()
{
	int y, i;

	PrintQLString(0, 2, TRUE, "Quest Log", 3);
	CelDecodeOnly(64, 511, pQLogCel, 1, 320);
	y = qtopline;
	for (i = 0; i < numqlines; i++) {
		PrintQLString(0, y, TRUE, questlist[qlist[i]]._qlstr, 0);
		y += 2;
	}
	PrintQLString(0, 22, TRUE, "Close Quest Log", 0);
	ALLQUESTS = (ALLQUESTS & 7) + 1;
}

void StartQuestlog()
{
	DWORD i;

	numqlines = 0;
	for (i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == 2 && quests[i]._qlog) {
			qlist[numqlines] = i;
			numqlines++;
		}
	}
	if (numqlines > 5) {
		qtopline = 5 - (numqlines >> 1);
	} else {
		qtopline = 8;
	}
	qline = 22;
	if (numqlines != 0)
		qline = qtopline;
	questlog = TRUE;
	ALLQUESTS = 1;
}

void QuestlogUp()
{
	if (numqlines) {
		if (qline == qtopline) {
			qline = 22;
		} else if (qline == 22) {
			qline = qtopline + 2 * numqlines - 2;
		} else {
			qline -= 2;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

void QuestlogDown()
{
	if (numqlines) {
		if (qline == 22) {
			qline = qtopline;
		} else if (qline == qtopline + 2 * numqlines - 2) {
			qline = 22;
		} else {
			qline += 2;
		}
		PlaySFX(IS_TITLEMOV);
	}
}

void QuestlogEnter()
{
	PlaySFX(IS_TITLSLCT);
	if (numqlines && qline != 22)
		InitQTextMsg(quests[qlist[(qline - qtopline) >> 1]]._qmsg);
	questlog = FALSE;
}

void QuestlogESC()
{
	int y, i;

	y = (MouseY - 32) / 12;
	if (numqlines) {
		for (i = 0; i < numqlines; i++) {
			if (y == qtopline + 2 * i) {
				qline = y;
				QuestlogEnter();
			}
		}
	}
	if (y == 22) {
		qline = 22;
		QuestlogEnter();
	}
}

void SetMultiQuest(int q, int s, int l, int v1)
{
	if (quests[q]._qactive != 3) {
		if (s > quests[q]._qactive)
			quests[q]._qactive = s;
		quests[q]._qlog |= l;
		if (v1 > quests[q]._qvar1)
			quests[q]._qvar1 = v1;
	}
}

DEVILUTION_END_NAMESPACE
