//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

int cursH;      // weak
int icursH28;   // idb
int cursW;      // idb
int pcursmonst; // idb
int icursW28;   // idb
void *pCursCels;
int icursH; // weak

// inv_item value
char pcursinvitem; // weak
int icursW;        // weak
char pcursitem;    // weak
char pcursobj;     // weak
char pcursplr;     // weak
int cursmx;
int cursmy;
int dword_4B8CCC; // weak
int pcurs;        // idb

/* rdata */
const int InvItemWidth[180] = {
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
	2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28, 2 * 28
};

const int InvItemHeight[180] = {
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
	3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28, 3 * 28
};

void __cdecl InitCursor()
{
	/// ASSERT: assert(! pCursCels);
	pCursCels = LoadFileInMem("Data\\Inv\\Objcurs.CEL", 0);
	ClearCursor();
}

void __cdecl FreeCursor()
{
	void *p;

	p = pCursCels;
	pCursCels = NULL;
	mem_free_dbg(p);

	ClearCursor();
}

void __fastcall SetICursor(int i)
{
	icursW = InvItemWidth[i];
	icursH = InvItemHeight[i];
	icursW28 = icursW / 28;
	icursH28 = icursH / 28;
}
// 4B8CB4: using guessed type int icursH;
// 4B8CBC: using guessed type int icursW;

void __fastcall SetCursor_(int i)
{
	pcurs = i;
	cursW = InvItemWidth[i];
	cursH = InvItemHeight[i];
	SetICursor(i);
}
// 4B8C9C: using guessed type int cursH;

void __fastcall NewCursor(int i)
{
	SetCursor_(i);
}

void __cdecl InitLevelCursor()
{
	SetCursor_(CURSOR_HAND);
	cursmx = ViewX;
	cursmy = ViewY;
	dword_4B8CCC = -1;
	pcursmonst = -1;
	pcursobj = -1;
	pcursitem = -1;
	pcursplr = -1;
	ClearCursor();
}
// 4B8CC0: using guessed type char pcursitem;
// 4B8CC1: using guessed type char pcursobj;
// 4B8CC2: using guessed type char pcursplr;
// 4B8CCC: using guessed type int dword_4B8CCC;

void __cdecl CheckTown()
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
				trigflag[3] = 1;
				ClearPanel();
				strcpy(infostr, "Town Portal");
				sprintf(tempstr, "from %s", plr[missile[mx]._misource]._pName);
				AddPanelString(tempstr, 1);
				cursmx = missile[mx]._mix;
				cursmy = missile[mx]._miy;
			}
		}
	}
}

void __cdecl CheckRportal()
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
				trigflag[3] = 1;
				ClearPanel();
				strcpy(infostr, "Portal to");
				if (!setlevel)
					strcpy(tempstr, "The Unholy Altar");
				else
					strcpy(tempstr, "level 15");
				AddPanelString(tempstr, 1);
				cursmx = missile[mx]._mix;
				cursmy = missile[mx]._miy;
			}
		}
	}
}
// 5CF31D: using guessed type char setlevel;

void __cdecl CheckCursMove()
{
	int mos_x;     // esi MAPDST
	int mos_y;     // edi MAPDST
	int posy_31;   // ebx
	int mx;        // edi
	int my;        // esi
	BOOL v9;       // eax
	int d_monster; // eax MAPDST
	int mon_id;    // ecx MAPDST
	char d_player; // al MAPDST
	char plr_id;   // cl MAPDST
	char obj_id;   // cl MAPDST
	char d_obj;    // al MAPDST
	char d_item;   // al MAPDST
	int i;
	char item_id;         // al MAPDST
	int posx_63;          // [esp+Ch] [ebp-18h]
	signed int xx;        // [esp+14h] [ebp-10h]
	signed int yy;        // [esp+18h] [ebp-Ch]
	signed int some_bool; // [esp+20h] [ebp-4h]

	mos_x = MouseX;
	mos_y = MouseY;
	if (chrflag || questlog) {
		if (MouseX < 160)
			mos_x = 0;
		else
			mos_x = MouseX - 160;
	} else if (invflag || sbookflag) {
		if (MouseX > 320)
			mos_x = 0;
		else
			mos_x = MouseX + 160;
	}
	/* 	else
	{
		mos_x = MouseX;
		mos_y = MouseY;
	} */
	if (MouseY > 351 && track_isscrolling())
		mos_y = 351;
	if (!zoomflag) {
		mos_x >>= 1;
		mos_y >>= 1;
	}
	mos_x -= ScrollInfo._sxoff;
	mos_y -= ScrollInfo._syoff;
	if (ScrollInfo._sdir) {
		mos_x += ((plr[myplr]._pVar6 + plr[myplr]._pxvel) >> 8) - (plr[myplr]._pVar6 >> 8);
		mos_y += ((plr[myplr]._pVar7 + plr[myplr]._pyvel) >> 8) - (plr[myplr]._pVar7 >> 8);
	}

	if (mos_x < 0)
		mos_x = 0;
	if (mos_x >= 640)
		mos_x = 640;
	if (mos_y < 0)
		mos_y = 0;
	if (mos_y >= 480)
		mos_y = 480;

	posy_31 = mos_y & 0x1F;
	posx_63 = mos_x & 0x3F;
	mx = (mos_x >> 6) + (mos_y >> 5) + ViewX - (zoomflag != 0 ? 10 : 5);
	my = (mos_y >> 5) + ViewY - (mos_x >> 6);

	if (posy_31<posx_63>> 1)
		--my;

	v9 = posy_31 >= 32 - (posx_63 >> 1);
	if (v9)
		++mx;

	if (mx < 0)
		mx = 0;
	if (mx >= MAXDUNX)
		mx = MAXDUNX - 1;
	if (my < 0)
		my = 0;
	if (my >= MAXDUNY)
		my = MAXDUNY - 1;

	some_bool = 0;
	if (posy_31 >= posx_63 >> 1) {
		if (!v9)
			some_bool = 0;
		else if (posx_63 < 32)
			some_bool = 1;
		else
			some_bool = 0;
	} else if (!v9) {
		if (posx_63 < 32)
			some_bool = 1;
		else
			some_bool = 0;
	}

	dword_4B8CCC = pcursmonst;
	pcursmonst = -1;
	pcursobj = -1;
	pcursitem = -1;
	if (pcursinvitem != -1)
		drawsbarflag = 1;
	pcursinvitem = -1;
	pcursplr = -1;
	uitemflag = 0;
	panelflag = 0;
	trigflag[3] = 0;

	if (plr[myplr]._pInvincible)
		return;
	if (pcurs >= CURSOR_FIRSTITEM || spselflag) {
		cursmx = mx;
		cursmy = my;
		return;
	}
	if (MouseY > 352) {
		CheckPanelInfo();
		return;
	}
	if (doomflag)
		return;
	if (invflag && MouseX > 320) {
		pcursinvitem = CheckInvHLight();
		return;
	}
	if (sbookflag && MouseX > 320 || (chrflag || questlog) && MouseX < 320)
		return;

	if (!leveltype) {
		if (some_bool) {
			d_monster = dMonster[mx][my + 1];
			if (d_monster > 0) {
				cursmx = mx;
				cursmy = my + 1;
				pcursmonst = d_monster - 1;
			}
		} else {
			d_monster = dMonster[mx + 1][my];
			if (d_monster > 0) {
				cursmx = mx + 1;
				cursmy = my;
				pcursmonst = d_monster - 1;
			}
		}
		d_monster = dMonster[mx][my];
		if (d_monster > 0) {
			cursmx = mx;
			cursmy = my;
			pcursmonst = d_monster - 1;
		}
		d_monster = dMonster[mx + 1][my + 1];
		if (d_monster > 0) {
			cursmx = mx + 1;
			cursmy = my + 1;
			pcursmonst = d_monster - 1;
		}
		if (pcursmonst != -1 && !towner[pcursmonst]._tSelFlag) // check if issue is upstream
			pcursmonst = -1;
	} else {
		if (dword_4B8CCC != -1) {
			if (some_bool) {
				d_monster = dMonster[mx + 1][my + 2];
				if (d_monster) {
					if (dFlags[mx + 1][my + 2] & DFLAG_LIT) {
						mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
						if (mon_id == dword_4B8CCC
						    && monster[mon_id]._mhitpoints >> 6 > 0
						    && monster[mon_id].MData->mSelFlag & 4) {
							cursmx = mx + 1;
							cursmy = my + 2;
							pcursmonst = mon_id;
						}
					}
				}
			} else {
				d_monster = dMonster[mx + 2][my + 1];
				if (d_monster) {
					if (dFlags[mx + 2][my + 1] & DFLAG_LIT) {
						mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
						if (mon_id == dword_4B8CCC
						    && monster[mon_id]._mhitpoints >> 6 > 0
						    && monster[mon_id].MData->mSelFlag & 4) {
							cursmx = mx + 1;
							cursmy = my + 2;
							pcursmonst = mon_id;
						}
					}
				}
			}
			d_monster = dMonster[mx + 2][my + 2];
			if (d_monster && dFlags[mx + 2][my + 2] & DFLAG_LIT) {
				mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
				if (mon_id == dword_4B8CCC
				    && monster[mon_id]._mhitpoints >> 6 > 0
				    && monster[mon_id].MData->mSelFlag & 4) {
					cursmx = mx + 2;
					cursmy = my + 2;
					pcursmonst = mon_id;
				}
			}
			if (some_bool) {
				d_monster = dMonster[mx][my + 1];
				if (d_monster && dFlags[mx][my + 1] & DFLAG_LIT) {
					mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
					if (mon_id == dword_4B8CCC
					    && monster[mon_id]._mhitpoints >> 6 > 0
					    && monster[mon_id].MData->mSelFlag & 2) {
						cursmx = mx;
						cursmy = my + 1;
						pcursmonst = mon_id;
					}
				}
			} else {
				d_monster = dMonster[mx + 1][my];
				if (d_monster && dFlags[mx + 1][my] & DFLAG_LIT) {
					mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
					if (mon_id == dword_4B8CCC
					    && monster[mon_id]._mhitpoints >> 6 > 0
					    && monster[mon_id].MData->mSelFlag & 2) {
						cursmy = my;
						cursmx = mx + 1;
						pcursmonst = mon_id;
					}
				}
			}
			d_monster = dMonster[mx][my];
			if (d_monster && dFlags[mx][my] & DFLAG_LIT) {
				mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
				if (mon_id == dword_4B8CCC
				    && monster[mon_id]._mhitpoints >> 6 > 0
				    && monster[mon_id].MData->mSelFlag & 1) {
					cursmx = mx;
					cursmy = my;
					pcursmonst = mon_id;
				}
			}
			d_monster = dMonster[mx + 1][my + 1];
			if (d_monster && dFlags[mx + 1][my + 1] & DFLAG_LIT) {
				mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
				if (mon_id == dword_4B8CCC
				    && monster[mon_id]._mhitpoints >> 6 > 0
				    && monster[mon_id].MData->mSelFlag & 2) {
					cursmx = mx + 1;
					cursmy = my + 1;
					pcursmonst = mon_id;
				}
			}
			if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_HIDDEN) {
				cursmx = mx;
				cursmy = my;
				pcursmonst = -1;
			}
			if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_GOLEM)
				pcursmonst = -1;
			if (pcursmonst != -1)
				return;
		}
		if (some_bool) {
			d_monster = dMonster[mx + 1][my + 2];
			if (d_monster && dFlags[mx + 1][my + 2] & DFLAG_LIT) {
				mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
				if (monster[mon_id]._mhitpoints >> 6 > 0 && monster[mon_id].MData->mSelFlag & 4) {
					cursmx = mx + 1;
					cursmy = my + 2;
					pcursmonst = mon_id;
				}
			}
		} else {
			d_monster = dMonster[mx + 2][my + 1];
			if (d_monster && dFlags[mx + 2][my + 1] & DFLAG_LIT) {
				mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
				if (monster[mon_id]._mhitpoints >> 6 > 0 && monster[mon_id].MData->mSelFlag & 4) {
					cursmx = mx + 2;
					cursmy = my + 1;
					pcursmonst = mon_id;
				}
			}
		}
		d_monster = dMonster[mx + 2][my + 2];
		if (d_monster && dFlags[mx + 2][my + 2] & DFLAG_LIT) {
			mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
			if (monster[mon_id]._mhitpoints >> 6 > 0 && monster[mon_id].MData->mSelFlag & 4) {
				pcursmonst = mon_id;
				cursmx = mx + 2;
				cursmy = my + 2;
			}
		}
		if (some_bool) {
			d_monster = dMonster[mx][my + 1];
			if (d_monster && dFlags[mx][my + 1] & DFLAG_LIT) {
				mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
				if (monster[mon_id]._mhitpoints >> 6 > 0 && monster[mon_id].MData->mSelFlag & 2) {
					cursmx = mx;
					cursmy = my + 1;
					pcursmonst = mon_id;
				}
			}
		} else {
			d_monster = dMonster[mx + 1][my];
			if (d_monster && dFlags[mx + 1][my] & DFLAG_LIT) {
				mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
				if (monster[mon_id]._mhitpoints >> 6 > 0 && monster[mon_id].MData->mSelFlag & 2) {
					cursmy = my;
					cursmx = mx + 1;
					pcursmonst = mon_id;
				}
			}
		}
		d_monster = dMonster[mx][my];
		if (d_monster && dFlags[mx][my] & DFLAG_LIT) {
			mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
			if (monster[mon_id]._mhitpoints >> 6 > 0 && monster[mon_id].MData->mSelFlag & 1) {
				cursmx = mx;
				cursmy = my;
				pcursmonst = mon_id;
			}
		}
		d_monster = dMonster[mx + 1][my + 1];
		if (d_monster && dFlags[mx + 1][my + 1] & DFLAG_LIT) {
			mon_id = d_monster <= 0 ? -1 - d_monster : d_monster - 1;
			if (monster[mon_id]._mhitpoints >> 6 > 0 && monster[mon_id].MData->mSelFlag & 2) {
				pcursmonst = mon_id;
				cursmx = mx + 1;
				cursmy = my + 1;
			}
		}
		if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_HIDDEN) {
			cursmx = mx;
			cursmy = my;
			pcursmonst = -1;
		}
		if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_GOLEM)
			pcursmonst = -1;
	}
	if (pcursmonst == -1) {
		if (some_bool) {
			d_player = dPlayer[mx][my + 1];
			if (d_player) {
				plr_id = d_player <= 0 ? -1 - d_player : d_player - 1;
				if (plr_id != myplr && plr[plr_id]._pHitPoints) {
					cursmx = mx;
					cursmy = my + 1;
					pcursplr = plr_id;
				}
			}
		} else {
			d_player = dPlayer[mx + 1][my];
			if (d_player) {
				plr_id = d_player <= 0 ? -1 - d_player : d_player - 1;
				if (plr_id != myplr && plr[plr_id]._pHitPoints) {
					cursmy = my;
					cursmx = mx + 1;
					pcursplr = plr_id;
				}
			}
		}
		d_player = dPlayer[mx][my];
		if (d_player) {
			plr_id = d_player <= 0 ? -1 - d_player : d_player - 1;
			if (plr_id != myplr) {
				cursmx = mx;
				cursmy = my;
				pcursplr = plr_id;
			}
		}
		if (dFlags[mx][my] & DFLAG_DEAD_PLAYER) {
			for (i = 0; i < MAX_PLRS; i++) {
				if (plr[i].WorldX == mx && plr[i].WorldY == my && i != myplr) {
					cursmx = mx;
					cursmy = my;
					pcursplr = i;
				}
			}
		}
		if (pcurs == CURSOR_RESURRECT) {
			for (xx = -1; xx < 2; xx++) {
				for (yy = -1; yy < 2; yy++) {
					if (dFlags[mx + xx][my + yy] & DFLAG_DEAD_PLAYER) {
						for (i = 0; i < MAX_PLRS; i++) {
							//   plr[i].WorldX - mx = xx     plr[i].WorldY - yy = my
							if (plr[i].WorldX == mx + xx && plr[i].WorldY == my + yy && i != myplr) {
								cursmx = mx + xx;
								cursmy = my + yy;
								pcursplr = i;
							}
						}
					}
				}
			}
		}
		d_player = dPlayer[mx + 1][my + 1];
		if (d_player) {
			plr_id = d_player <= 0 ? -1 - d_player : d_player - 1;
			if (plr_id != myplr && plr[plr_id]._pHitPoints) {
				pcursplr = plr_id;
				cursmx = mx + 1;
				cursmy = my + 1;
			}
		}
	}
	if (pcursmonst == -1 && pcursplr == -1) /* (BYTE)pcursmonst */
	{
		if (some_bool) {
			d_obj = dObject[mx][my + 1];
			if (d_obj) {
				obj_id = d_obj <= 0 ? -1 - d_obj : d_obj - 1;
				if (object[obj_id]._oSelFlag >= 2) {
					cursmx = mx;
					cursmy = my + 1;
					pcursobj = obj_id;
				}
			}
		} else {
			d_obj = dObject[mx + 1][my];
			if (d_obj) {
				obj_id = d_obj <= 0 ? -1 - d_obj : d_obj - 1;
				if (object[obj_id]._oSelFlag >= 2) {
					cursmy = my;
					cursmx = mx + 1;
					pcursobj = obj_id;
				}
			}
		}
		d_obj = dObject[mx][my];
		if (d_obj) {
			obj_id = d_obj <= 0 ? -1 - d_obj : d_obj - 1;
			if (object[obj_id]._oSelFlag == 1 || object[obj_id]._oSelFlag == 3) {
				cursmx = mx;
				cursmy = my;
				pcursobj = obj_id;
			}
		}
		d_obj = dObject[mx + 1][my + 1];
		if (d_obj && (d_obj <= 0 ? (obj_id = -1 - d_obj) : (obj_id = d_obj - 1), object[obj_id]._oSelFlag >= 2)) {
			pcursobj = obj_id;
			cursmx = mx + 1;
			cursmy = my + 1;
		}
	}
	if (pcursmonst == -1 && pcursplr == -1 && pcursobj == -1) {
		if (some_bool) {
			d_item = dItem[mx][my + 1];
			if (d_item > 0) {
				item_id = d_item - 1;
				if (item[item_id]._iSelFlag >= 2) {
					cursmx = mx;
					cursmy = my + 1;
					pcursitem = item_id;
				}
			}
		} else {
			d_item = dItem[mx + 1][my];
			if (d_item > 0) {
				item_id = d_item - 1;
				if (item[item_id]._iSelFlag >= 2) {
					cursmy = my;
					cursmx = mx + 1;
					pcursitem = item_id;
				}
			}
		}
		d_item = dItem[mx][my];
		if (d_item > 0) {
			item_id = d_item - 1;
			if (item[item_id]._iSelFlag == 1 || item[item_id]._iSelFlag == 3) {
				cursmx = mx;
				cursmy = my;
				pcursitem = item_id;
			}
		}
		d_item = dItem[mx + 1][my + 1];
		if (d_item > 0) {
			item_id = d_item - 1;
			if (item[item_id]._iSelFlag >= 2) {
				pcursitem = item_id;
				cursmx = mx + 1;
				cursmy = my + 1;
			}
		}
	}
	if (pcursmonst == -1 && pcursplr == -1 && pcursobj == -1 && pcursitem == -1) {
		cursmx = mx;
		cursmy = my;
		CheckTrigForce();
		CheckTown();
		CheckRportal();
	}
	if (pcurs == CURSOR_IDENTIFY) {
		pcursobj = -1;
		pcursitem = -1;
		pcursmonst = -1;
		cursmx = mx;
		cursmy = my;
	}
	if (pcursmonst != -1 && monster[pcursmonst]._mFlags & MFLAG_GOLEM)
		pcursmonst = -1;
}
// 4B8968: using guessed type int sbookflag;
// 4B8B84: using guessed type int panelflag;
// 4B8C98: using guessed type int spselflag;
// 4B8CB8: using guessed type char pcursinvitem;
// 4B8CC0: using guessed type char pcursitem;
// 4B8CC1: using guessed type char pcursobj;
// 4B8CC2: using guessed type char pcursplr;
// 4B8CCC: using guessed type int dword_4B8CCC;
// 52569C: using guessed type int zoomflag;
// 52575C: using guessed type int doomflag;
// 69BD04: using guessed type int questlog;

DEVILUTION_END_NAMESPACE
