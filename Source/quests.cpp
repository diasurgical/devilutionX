#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

int qtopline;
BOOL questlog;
BYTE *pQLogCel;
QuestStruct quests[MAXQUESTS];
int qline;
int qlist[MAXQUESTS];
int numqlines;
int WaterDone;
int ReturnLvlX;
int ReturnLvlY;
int ReturnLvlT;
int ReturnLvl;

QuestData questlist[MAXQUESTS] = {
	// clang-format off
	// _qdlvl, _qdmultlvl, _qlvlt,          _qdtype,   _qdrnd, _qslvl, _qflags, _qdmsg,         _qlstr
	{       5,         -1, DTYPE_NONE,      Q_ROCK,  100,      0,       0, TEXT_INFRA5,   "The Magic Rock"           },
	{       9,         -1, DTYPE_NONE,      Q_MUSHROOM,   100,      0,       0, TEXT_MUSH8,    "Black Mushroom"           },
	{       4,         -1, DTYPE_NONE,      Q_GARBUD,   100,      0,       0, TEXT_GARBUD1,  "Gharbad The Weak"         },
	{       8,         -1, DTYPE_NONE,      Q_ZHAR,   100,      0,       0, TEXT_ZHAR1,    "Zhar the Mad"             },
	{      14,         -1, DTYPE_NONE,      Q_VEIL,   100,      0,       0, TEXT_VEIL9,    "Lachdanan"                },
	{      15,         -1, DTYPE_NONE,      Q_DIABLO,    100,      0,       1, TEXT_VILE3,    "Diablo"                   },
	{       2,          2, DTYPE_NONE,      Q_BUTCHER,  100,      0,       1, TEXT_BUTCH9,   "The Butcher"              },
	{       4,         -1, DTYPE_NONE,      Q_LTBANNER,    100,      0,       0, TEXT_BANNER2,  "Ogden's Sign"             },
	{       7,         -1, DTYPE_NONE,      Q_BLIND,  100,      0,       0, TEXT_BLINDING, "Halls of the Blind"       },
	{       5,         -1, DTYPE_NONE,      Q_BLOOD,  100,      0,       0, TEXT_BLOODY,   "Valor"                    },
	{      10,         -1, DTYPE_NONE,      Q_ANVIL,  100,      0,       0, TEXT_ANVIL5,   "Anvil of Fury"            },
	{      13,         -1, DTYPE_NONE,      Q_WARLORD, 100,      0,       0, TEXT_BLOODWAR, "Warlord of Blood"         },
	{       3,          3, DTYPE_CATHEDRAL, Q_SKELKING,   100,      1,       1, TEXT_KING2,    "The Curse of King Leoric" },
	{       2,         -1, DTYPE_CAVES,     Q_PWATER,     100,      4,       0, TEXT_POISON3,  "Poisoned Water Supply"    },
	{       6,         -1, DTYPE_CATACOMBS, Q_SCHAMB,   100,      2,       0, TEXT_BONER,    "The Chamber of Bone"      },
	{      15,         15, DTYPE_CATHEDRAL, Q_BETRAYER,     100,      5,       1, TEXT_VILE1,    "Archbishop Lazarus"       },
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
int QuestGroup1[3] = { Q_BUTCHER, Q_LTBANNER, Q_GARBUD };
int QuestGroup2[3] = { Q_BLIND, Q_ROCK, Q_BLOOD };
int QuestGroup3[3] = { Q_MUSHROOM, Q_ZHAR, Q_ANVIL };
int QuestGroup4[2] = { Q_VEIL, Q_WARLORD };

void InitQuests()
{
	int i, initiatedQuests;
	DWORD z;

	if (gbMaxPlayers == 1) {
		for (i = 0; i < MAXQUESTS; i++) {
			quests[i]._qactive = QUEST_NOTAVAIL;
		}
	} else {
		for (i = 0; i < MAXQUESTS; i++) {
			if (!(questlist[i]._qflags & 1)) {
				quests[i]._qactive = QUEST_NOTAVAIL;
			}
		}
	}

	questlog = FALSE;
	PentSpn2Frame = 1;
	WaterDone = 0;
	initiatedQuests = 0;

	for (z = 0; z < MAXQUESTS; z++) {
		if (gbMaxPlayers <= 1 || questlist[z]._qflags & 1) {
			quests[z]._qtype = questlist[z]._qdtype;
			if (gbMaxPlayers > 1) {
				quests[z]._qlevel = questlist[z]._qdmultlvl;
				if (!delta_quest_inited(initiatedQuests)) {
					quests[z]._qactive = QUEST_INIT;
					quests[z]._qvar1 = 0;
					quests[z]._qlog = 0;
				}
				initiatedQuests++;
			} else {
				quests[z]._qactive = QUEST_INIT;
				quests[z]._qlevel = questlist[z]._qdlvl;
				quests[z]._qvar1 = 0;
				quests[z]._qlog = 0;
			}

			quests[z]._qslvl = questlist[z]._qslvl;
			quests[z]._qtx = 0;
			quests[z]._qty = 0;
			quests[z]._qidx = z;
			quests[z]._qlvltype = questlist[z]._qlvlt;
			quests[z]._qvar2 = 0;
			quests[z]._qmsg = questlist[z]._qdmsg;
		}
	}

	if (gbMaxPlayers == 1) {
		SetRndSeed(glSeedTbl[15]);
		if (random_(0, 2))
			quests[Q_PWATER]._qactive = QUEST_NOTAVAIL;
		else
			quests[Q_SKELKING]._qactive = QUEST_NOTAVAIL;

		quests[QuestGroup1[random_(0, sizeof(QuestGroup1) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup2[random_(0, sizeof(QuestGroup2) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup3[random_(0, sizeof(QuestGroup3) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
		quests[QuestGroup4[random_(0, sizeof(QuestGroup4) / sizeof(int))]]._qactive = QUEST_NOTAVAIL;
	}
#ifdef _DEBUG
	if (questdebug != -1)
		quests[questdebug]._qactive = QUEST_ACTIVE;
#endif

#ifdef SPAWN
	for (z = 0; z < MAXQUESTS; z++) {
		quests[z]._qactive = QUEST_NOTAVAIL;
	}
#endif

	if (!quests[Q_SKELKING]._qactive)
		quests[Q_SKELKING]._qvar2 = 2;
	if (!quests[Q_ROCK]._qactive)
		quests[Q_ROCK]._qvar2 = 2;
	quests[Q_LTBANNER]._qvar1 = 1;
	if (gbMaxPlayers != 1)
		quests[Q_BETRAYER]._qvar1 = 2;
}

void CheckQuests()
{
#ifndef SPAWN
	int i, rportx, rporty;

	if (QuestStatus(Q_BETRAYER) && gbMaxPlayers != 1 && quests[Q_BETRAYER]._qvar1 == 2) {
		AddObject(OBJ_ALTBOY, 2 * setpc_x + 20, 2 * setpc_y + 22);
		quests[Q_BETRAYER]._qvar1 = 3;
		NetSendCmdQuest(TRUE, Q_BETRAYER);
	}

	if (gbMaxPlayers != 1) {
		return;
	}

	if (currlevel == quests[Q_BETRAYER]._qlevel
	    && !setlevel
	    && quests[Q_BETRAYER]._qvar1 >= 2
	    && (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE || quests[Q_BETRAYER]._qactive == QUEST_DONE)
	    && (quests[Q_BETRAYER]._qvar2 == 0 || quests[Q_BETRAYER]._qvar2 == 2)) {
		quests[Q_BETRAYER]._qtx = 2 * quests[Q_BETRAYER]._qtx + 16;
		quests[Q_BETRAYER]._qty = 2 * quests[Q_BETRAYER]._qty + 16;
		rportx = quests[Q_BETRAYER]._qtx;
		rporty = quests[Q_BETRAYER]._qty;
		AddMissile(rportx, rporty, rportx, rporty, 0, MIS_RPORTAL, 0, myplr, 0, 0);
		quests[Q_BETRAYER]._qvar2 = 1;
		if (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE) {
			quests[Q_BETRAYER]._qvar1 = 3;
		}
	}

	if (quests[Q_BETRAYER]._qactive == QUEST_DONE
	    && setlevel
	    && setlvlnum == SL_VILEBETRAYER
	    && quests[Q_BETRAYER]._qvar2 == 4) {
		rportx = 35;
		rporty = 32;
		AddMissile(rportx, rporty, rportx, rporty, 0, MIS_RPORTAL, 0, myplr, 0, 0);
		quests[Q_BETRAYER]._qvar2 = 3;
	}

	if (setlevel) {
		if (setlvlnum == quests[Q_PWATER]._qslvl
		    && quests[Q_PWATER]._qactive != QUEST_INIT
		    && leveltype == quests[Q_PWATER]._qlvltype
		    && nummonsters == 4
		    && quests[Q_PWATER]._qactive != QUEST_DONE) {
			quests[Q_PWATER]._qactive = QUEST_DONE;
			PlaySfxLoc(IS_QUESTDN, plr[myplr]._px, plr[myplr]._py);
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
			    && quests[i]._qactive != QUEST_NOTAVAIL
			    && plr[myplr]._px == quests[i]._qtx
			    && plr[myplr]._py == quests[i]._qty) {
				if (quests[i]._qlvltype != DTYPE_NONE) {
					setlvltype = quests[i]._qlvltype;
				}
				StartNewLvl(myplr, WM_DIABSETLVL, quests[i]._qslvl);
			}
		}
	}
#endif
}

BOOL ForceQuests()
{
#ifndef SPAWN
	int i, j, qx, qy, ql;

	if (gbMaxPlayers != 1) {
		return FALSE;
	}

	for (i = 0; i < MAXQUESTS; i++) {

		if (i != Q_BETRAYER && currlevel == quests[i]._qlevel && quests[i]._qslvl != 0) {
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
#endif

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
#ifndef SPAWN
	int i, j;

	if (monster[m].MType->mtype == MT_SKING) {
		quests[Q_SKELKING]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR82;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE82;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE82;
		}
		if (sendmsg)
			NetSendCmdQuest(TRUE, Q_SKELKING);

	} else if (monster[m].MType->mtype == MT_CLEAVER) {
		quests[Q_BUTCHER]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR80;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE80;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE80;
		}
		if (sendmsg)
			NetSendCmdQuest(TRUE, Q_BUTCHER);
	} else if (monster[m].mName == UniqMonst[UMT_GARBUD].mName) { //"Gharbad the Weak"
		quests[Q_GARBUD]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR61;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE61;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE61;
		}
	} else if (monster[m].mName == UniqMonst[UMT_ZHAR].mName) { //"Zhar the Mad"
		quests[Q_ZHAR]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR62;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE62;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE62;
		}
	} else if (monster[m].mName == UniqMonst[UMT_LAZURUS].mName && gbMaxPlayers != 1) { //"Arch-Bishop Lazarus"
		quests[Q_BETRAYER]._qactive = QUEST_DONE;
		quests[Q_BETRAYER]._qvar1 = 7;
		sfxdelay = 30;
		quests[Q_DIABLO]._qactive = QUEST_ACTIVE;

		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] == 370) {
					trigs[numtrigs]._tx = i;
					trigs[numtrigs]._ty = j;
					trigs[numtrigs]._tmsg = WM_DIABNEXTLVL;
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
			NetSendCmdQuest(TRUE, Q_BETRAYER);
			NetSendCmdQuest(TRUE, Q_DIABLO);
		}
	} else if (monster[m].mName == UniqMonst[UMT_LAZURUS].mName && gbMaxPlayers == 1) { //"Arch-Bishop Lazarus"
		quests[Q_BETRAYER]._qactive = QUEST_DONE;
		sfxdelay = 30;
		InitVPTriggers();
		quests[Q_BETRAYER]._qvar1 = 7;
		quests[Q_BETRAYER]._qvar2 = 4;
		quests[Q_DIABLO]._qactive = QUEST_ACTIVE;
		AddMissile(35, 32, 35, 32, 0, MIS_RPORTAL, 0, myplr, 0, 0);
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR83;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE83;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE83;
		}
	} else if (monster[m].mName == UniqMonst[UMT_WARLORD].mName) { //"Warlord of Blood"
		quests[Q_WARLORD]._qactive = QUEST_DONE;
		sfxdelay = 30;
		if (plr[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR94;
		} else if (plr[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE94;
		} else if (plr[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE94;
		}
	}
#endif
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
			case Q_BUTCHER:
				DrawButcher();
				break;
			case Q_LTBANNER:
				DrawLTBanner(x, y);
				break;
			case Q_BLIND:
				DrawBlind(x, y);
				break;
			case Q_BLOOD:
				DrawBlood(x, y);
				break;
			case Q_WARLORD:
				DrawWarLord(x, y);
				break;
			case Q_SKELKING:
				DrawSkelKing(i, x, y);
				break;
			case Q_SCHAMB:
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
		ReturnLvlX = quests[Q_SKELKING]._qtx + 1;
		ReturnLvlY = quests[Q_SKELKING]._qty;
		ReturnLvl = quests[Q_SKELKING]._qlevel;
		ReturnLvlT = DTYPE_CATHEDRAL;
		break;
	case SL_BONECHAMB:
		ReturnLvlX = quests[Q_SCHAMB]._qtx + 1;
		ReturnLvlY = quests[Q_SCHAMB]._qty;
		ReturnLvl = quests[Q_SCHAMB]._qlevel;
		ReturnLvlT = DTYPE_CATACOMBS;
		break;
	case SL_POISONWATER:
		ReturnLvlX = quests[Q_PWATER]._qtx;
		ReturnLvlY = quests[Q_PWATER]._qty + 1;
		ReturnLvl = quests[Q_PWATER]._qlevel;
		ReturnLvlT = DTYPE_CATHEDRAL;
		break;
	case SL_VILEBETRAYER:
		ReturnLvlX = quests[Q_BETRAYER]._qtx + 1;
		ReturnLvlY = quests[Q_BETRAYER]._qty - 1;
		ReturnLvl = quests[Q_BETRAYER]._qlevel;
		ReturnLvlT = DTYPE_HELL;
		break;
	}
}

void GetReturnLvlPos()
{
	if (quests[Q_BETRAYER]._qactive == QUEST_DONE)
		quests[Q_BETRAYER]._qvar2 = 2;
	ViewX = ReturnLvlX;
	ViewY = ReturnLvlY;
	currlevel = ReturnLvl;
	leveltype = ReturnLvlT;
}

void ResyncMPQuests()
{
#ifndef SPAWN
	if (quests[Q_SKELKING]._qactive == QUEST_INIT
	    && currlevel >= quests[Q_SKELKING]._qlevel - 1
	    && currlevel <= quests[Q_SKELKING]._qlevel + 1) {
		quests[Q_SKELKING]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(TRUE, Q_SKELKING);
	}
	if (quests[Q_BUTCHER]._qactive == QUEST_INIT
	    && currlevel >= quests[Q_BUTCHER]._qlevel - 1
	    && currlevel <= quests[Q_BUTCHER]._qlevel + 1) {
		quests[Q_BUTCHER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(TRUE, Q_BUTCHER);
	}
	if (quests[Q_BETRAYER]._qactive == QUEST_INIT && currlevel == quests[Q_BETRAYER]._qlevel - 1) {
		quests[Q_BETRAYER]._qactive = QUEST_ACTIVE;
		NetSendCmdQuest(TRUE, Q_BETRAYER);
	}
	if (QuestStatus(Q_BETRAYER))
		AddObject(OBJ_ALTBOY, 2 * setpc_x + 20, 2 * setpc_y + 22);
#endif
}

void ResyncQuests()
{
#ifndef SPAWN
	int i, tren, x, y;

	if (setlevel && setlvlnum == quests[Q_PWATER]._qslvl && quests[Q_PWATER]._qactive != QUEST_INIT && leveltype == quests[Q_PWATER]._qlvltype) {

		if (quests[Q_PWATER]._qactive == QUEST_DONE)
			LoadPalette("Levels\\L3Data\\L3pwater.pal");
		else
			LoadPalette("Levels\\L3Data\\L3pfoul.pal");

		for (i = 0; i <= 32; i++)
			palette_update_quest_palette(i);
	}

	if (QuestStatus(Q_LTBANNER)) {
		if (quests[Q_LTBANNER]._qvar1 == 1)
			ObjChangeMapResync(
			    setpc_w + setpc_x - 2,
			    setpc_h + setpc_y - 2,
			    setpc_w + setpc_x + 1,
			    setpc_h + setpc_y + 1);
		if (quests[Q_LTBANNER]._qvar1 == 2) {
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
		if (quests[Q_LTBANNER]._qvar1 == 3) {
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
	if (currlevel == quests[Q_MUSHROOM]._qlevel) {
		if (quests[Q_MUSHROOM]._qactive == QUEST_INIT && !quests[Q_MUSHROOM]._qvar1) {
			SpawnQuestItem(IDI_FUNGALTM, 0, 0, 5, 1);
			quests[Q_MUSHROOM]._qvar1 = QS_TOMESPAWNED;
		} else {
			if (quests[Q_MUSHROOM]._qactive == QUEST_ACTIVE) {
				if (quests[Q_MUSHROOM]._qvar1 >= QS_MUSHGIVEN) {
					Qtalklist[TOWN_WITCH]._qblkm = -1;
					Qtalklist[TOWN_HEALER]._qblkm = TEXT_MUSH3;
				} else if (quests[Q_MUSHROOM]._qvar1 >= QS_BRAINGIVEN) {
					Qtalklist[TOWN_HEALER]._qblkm = -1;
				}
			}
		}
	}
	if (currlevel == quests[Q_VEIL]._qlevel + 1 && quests[Q_VEIL]._qactive == QUEST_ACTIVE && !quests[Q_VEIL]._qvar1) {
		quests[Q_VEIL]._qvar1 = 1;
		SpawnQuestItem(IDI_GLDNELIX, 0, 0, 5, 1);
	}
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		if (quests[Q_BETRAYER]._qvar1 >= 4)
			ObjChangeMapResync(1, 11, 20, 18);
		if (quests[Q_BETRAYER]._qvar1 >= 6)
			ObjChangeMapResync(1, 18, 20, 24);
		if (quests[Q_BETRAYER]._qvar1 >= 7)
			InitVPTriggers();
		for (i = 0; i < nobjects; i++)
			SyncObjectAnim(objectactive[i]);
	}
	if (currlevel == quests[Q_BETRAYER]._qlevel
	    && !setlevel
	    && (quests[Q_BETRAYER]._qvar2 == 1 || quests[Q_BETRAYER]._qvar2 >= 3)
	    && (quests[Q_BETRAYER]._qactive == QUEST_ACTIVE || quests[Q_BETRAYER]._qactive == QUEST_DONE)) {
		quests[Q_BETRAYER]._qvar2 = 2;
	}
#endif
}

void PrintQLString(int x, int y, BOOL cjustflag, char *str, int col)
{
	int len, width, i, k, sx, sy;
	BYTE c;

	sx = x + (32 + SCREEN_X);
	sy = y * 12 + (44 + SCREEN_Y);
	len = strlen(str);
	k = 0;
	if (cjustflag) {
		width = 0;
		for (i = 0; i < len; i++)
			width += fontkern[fontframe[gbFontTransTbl[(BYTE)str[i]]]] + 1;
		if (width < 257)
			k = (257 - width) >> 1;
		sx += k;
	}
	if (qline == y) {
		CelDraw(cjustflag ? x + k + (12 + SCREEN_X) : x + (12 + SCREEN_X), sy + 1, pSPentSpn2Cels, PentSpn2Frame, 12);
	}
	for (i = 0; i < len; i++) {
		c = fontframe[gbFontTransTbl[(BYTE)str[i]]];
		k += fontkern[c] + 1;
		if (c && k <= 257) {
			PrintChar(sx, sy, c, col);
		}
		sx += fontkern[c] + 1;
	}
	if (qline == y) {
		CelDraw(cjustflag ? x + k + (36 + SCREEN_X) : (276 + SCREEN_X) - x, sy + 1, pSPentSpn2Cels, PentSpn2Frame, 12);
	}
}

void DrawQuestLog()
{
	int y, i;

	PrintQLString(0, 2, TRUE, "Quest Log", 3);
	CelDraw(SCREEN_X, SCREEN_Y + 351, pQLogCel, 1, SPANEL_WIDTH);
	y = qtopline;
	for (i = 0; i < numqlines; i++) {
		PrintQLString(0, y, TRUE, questlist[qlist[i]]._qlstr, 0);
		y += 2;
	}
	PrintQLString(0, 22, TRUE, "Close Quest Log", 0);
	PentSpn2Spin();
}

void StartQuestlog()
{
	DWORD i;

	numqlines = 0;
	for (i = 0; i < MAXQUESTS; i++) {
		if (quests[i]._qactive == QUEST_ACTIVE && quests[i]._qlog) {
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
	PentSpn2Frame = 1;
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
#ifndef SPAWN
	if (quests[q]._qactive != QUEST_DONE) {
		if (s > quests[q]._qactive)
			quests[q]._qactive = s;
		quests[q]._qlog |= l;
		if (v1 > quests[q]._qvar1)
			quests[q]._qvar1 = v1;
	}
#endif
}

DEVILUTION_END_NAMESPACE
