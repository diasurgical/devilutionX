//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int cursH; // weak
int icursH28; // idb
int cursW; // idb
int pcursmonst; // idb
int icursW28; // idb
void *pCursCels;
int icursH; // weak

// inv_item value
char pcursinvitem; // weak
int icursW; // weak
char pcursitem; // weak
char pcursobj; // weak
char pcursplr; // weak
int cursmx;
int cursmy;
int dword_4B8CCC; // weak
int pcurs; // idb
#endif

/* rdata */
const int InvItemWidth[180] =
{
  0,
  33,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  23,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56
};
const int InvItemHeight[180] =
{
  0,
  29,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  32,
  35,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  28,
  56,
  56,
  56,
  56,
  56,
  56,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  56,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84,
  84
};

void __cdecl InitCursor()
{
	pCursCels = LoadFileInMem("Data\\Inv\\Objcurs.CEL", 0);
	ClearCursor();
}

void __cdecl FreeCursor()
{
	void *v0; // ecx

	v0 = pCursCels;
	pCursCels = 0;
	mem_free_dbg(v0);
	ClearCursor();
}

void __fastcall SetICursor(int i)
{
	int v1; // ecx

	v1 = i;
	icursW = InvItemWidth[v1];
	icursH = InvItemHeight[v1];
	icursW28 = icursW / 28;
	icursH28 = icursH / 28;
}
// 4B8CB4: using guessed type int icursH;
// 4B8CBC: using guessed type int icursW;

void __fastcall SetCursor(int i)
{
	int v1; // eax

	v1 = InvItemWidth[i];
	pcurs = i;
	cursW = v1;
	cursH = InvItemHeight[i];
	SetICursor(i);
}
// 4B8C9C: using guessed type int cursH;

void __fastcall NewCursor(int i) {
	SetCursor(i);
}

void __cdecl InitLevelCursor()
{
	SetCursor(CURSOR_HAND);
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
	int v0; // ecx
	int v1; // eax
	int v2; // esi
	int v3; // edx
	int v4; // ebx
	int v5; // [esp+0h] [ebp-4h]

	v5 = 0;
	if ( nummissiles > 0 )
	{
		v0 = cursmx;
		v1 = cursmy;
		do
		{
			v2 = missileactive[v5];
			if ( missile[v2]._mitype == MIS_TOWN )
			{
				if ( (v3 = missile[v2]._mix, v4 = v3 - 1, v0 == v3 - 1) && v1 == missile[v2]._miy
				  || v0 == v3 && v1 == missile[v2]._miy - 1
				  || v0 == v4 && v1 == missile[v2]._miy - 1
				  || v0 == v3 - 2 && (v1 == missile[v2]._miy - 1 || v0 == v3 - 2 && v1 == missile[v2]._miy - 2)
				  || v0 == v4 && v1 == missile[v2]._miy - 2
				  || v0 == v3 && v1 == missile[v2]._miy )
				{
					trigflag[3] = 1;
					ClearPanel();
					strcpy(infostr, "Town Portal");
					sprintf(tempstr, "from %s", plr[missile[v2]._misource]._pName);
					AddPanelString(tempstr, 1);
					v0 = missile[v2]._mix;
					v1 = missile[v2]._miy;
					cursmx = missile[v2]._mix;
					cursmy = v1;
				}
			}
			++v5;
		}
		while ( v5 < nummissiles );
	}
}

void __cdecl CheckRportal()
{
	int v0; // ecx
	int v1; // eax
	int v2; // esi
	int v3; // edx
	int v4; // ebx
	int v5; // [esp+0h] [ebp-4h]

	v5 = 0;
	if ( nummissiles > 0 )
	{
		v0 = cursmx;
		v1 = cursmy;
		do
		{
			v2 = missileactive[v5];
			if ( missile[v2]._mitype == MIS_RPORTAL )
			{
				if ( (v3 = missile[v2]._mix, v4 = v3 - 1, v0 == v3 - 1) && v1 == missile[v2]._miy
				  || v0 == v3 && v1 == missile[v2]._miy - 1
				  || v0 == v4 && v1 == missile[v2]._miy - 1
				  || v0 == v3 - 2 && (v1 == missile[v2]._miy - 1 || v0 == v3 - 2 && v1 == missile[v2]._miy - 2)
				  || v0 == v4 && v1 == missile[v2]._miy - 2
				  || v0 == v3 && v1 == missile[v2]._miy )
				{
					trigflag[3] = 1;
					ClearPanel();
					strcpy(infostr, "Portal to");
					if ( setlevel )
						strcpy(tempstr, "level 15");
					else
						strcpy(tempstr, "The Unholy Altar");
					AddPanelString(tempstr, 1);
					v0 = missile[v2]._mix;
					v1 = missile[v2]._miy;
					cursmx = missile[v2]._mix;
					cursmy = v1;
				}
			}
			++v5;
		}
		while ( v5 < nummissiles );
	}
}
// 5CF31D: using guessed type char setlevel;

void __cdecl CheckCursMove()
{
	int v0; // esi
	signed int v1; // edi
	int v2; // esi
	int v3; // edi
	int v4; // edx
	int v5; // ebx
	int v6; // edi
	int v7; // eax
	int v8; // esi
	BOOL v9; // eax
	int v10; // ecx
	int v11; // edx
	int v12; // ecx
	int v13; // ebx
	int v14; // ebx
	int v15; // eax
	bool v16; // zf
	int v17; // ecx
	int v18; // eax
	int v19; // ecx
	int v20; // eax
	int v21; // ecx
	int v22; // eax
	int v23; // eax
	int v24; // ecx
	int v25; // eax
	int v26; // ecx
	int v27; // ebx
	int v28; // edx
	int v29; // eax
	int v30; // ecx
	int v31; // eax
	int v32; // eax
	int v33; // eax
	int v34; // ecx
	int v35; // eax
	int v36; // ecx
	int v37; // eax
	int v38; // eax
	int v39; // ecx
	int v40; // eax
	int v41; // ecx
	signed int v42; // eax
	int v43; // ecx
	int v44; // eax
	int v45; // eax
	int v46; // eax
	int v47; // eax
	char v48; // al
	char v49; // cl
	char v50; // al
	char v51; // al
	char v52; // cl
	int v53; // ecx
	int *v54; // eax
	int v55; // edx
	int *v56; // ecx
	char v57; // al
	char v58; // cl
	signed int v59; // edx
	char v60; // al
	char v61; // cl
	char v62; // al
	char v63; // al
	char v64; // cl
	char v65; // al
	char v66; // al
	char v67; // cl
	char v68; // al
	char v69; // al
	char v70; // al
	char v71; // al
	char v72; // al
	char v73; // cl
	char v74; // al
	char v75; // al
	int v76; // [esp+Ch] [ebp-18h]
	char *v77; // [esp+Ch] [ebp-18h]
	int v78; // [esp+10h] [ebp-14h]
	signed int v79; // [esp+14h] [ebp-10h]
	signed int v80; // [esp+18h] [ebp-Ch]
	int v81; // [esp+1Ch] [ebp-8h]
	int v82; // [esp+1Ch] [ebp-8h]
	signed int v83; // [esp+20h] [ebp-4h]

	v0 = MouseX;
	v1 = MouseY;
	if ( chrflag || questlog )
	{
		if ( MouseX >= 160 )
		{
			v0 = MouseX - 160;
			goto LABEL_10;
		}
		goto LABEL_9;
	}
	if ( invflag || sbookflag )
	{
		if ( MouseX <= 320 )
		{
			v0 = MouseX + 160;
			goto LABEL_10;
		}
LABEL_9:
		v0 = 0;
	}
LABEL_10:
	if ( MouseY > 351 && track_isscrolling() )
		v1 = 351;
	if ( !zoomflag )
	{
		v0 >>= 1;
		v1 >>= 1;
	}
	v2 = v0 - ScrollInfo._sxoff;
	v3 = v1 - ScrollInfo._syoff;
	if ( ScrollInfo._sdir )
	{
		v2 += ((plr[myplr]._pVar6 + plr[myplr]._pxvel) >> 8) - (plr[myplr]._pVar6 >> 8);
		v3 += ((plr[myplr]._pVar7 + plr[myplr]._pyvel) >> 8) - (plr[myplr]._pVar7 >> 8);
	}
	if ( v2 < 0 )
		v2 = 0;
	if ( v2 >= 640 )
		v2 = 640;
	if ( v3 < 0 )
		v3 = 0;
	if ( v3 >= 480 )
		v3 = 480;
	v4 = v3 >> 5;
	v5 = v3 & 0x1F;
	v76 = v2 & 0x3F;
	v6 = (v2 >> 6) + (v3 >> 5) + ViewX - (zoomflag != 0 ? 10 : 5);
	v7 = v76 >> 1;
	v8 = v4 + ViewY - (v2 >> 6);
	if ( v5 < v76 >> 1 )
		--v8;
	v9 = v5 >= 32 - v7;
	if ( v9 )
		++v6;
	if ( v6 < 0 )
		v6 = 0;
	if ( v6 >= 112 )
		v6 = 111;
	if ( v8 < 0 )
		v8 = 0;
	if ( v8 >= 112 )
		v8 = 111;
	if ( v5 >= v76 >> 1 )
	{
		if ( !v9 )
			goto LABEL_49;
		goto LABEL_48;
	}
	if ( !v9 )
	{
LABEL_48:
		if ( v76 < 32 )
			goto LABEL_39;
LABEL_49:
		v83 = 0;
		goto LABEL_40;
	}
LABEL_39:
	v83 = 1;
LABEL_40:
	v10 = pcursmonst;
	pcursobj = -1;
	pcursitem = -1;
	v11 = -1;
	dword_4B8CCC = pcursmonst;
	pcursmonst = -1;
	if ( pcursinvitem != -1 )
		drawsbarflag = 1;
	pcursinvitem = -1;
	pcursplr = -1;
	v16 = plr[myplr]._pInvincible == 0;
	uitemflag = 0;
	panelflag = 0;
	trigflag[3] = 0;
	if ( !v16 )
		return;
	if ( pcurs >= CURSOR_FIRSTITEM || spselflag )
	{
		cursmx = v6;
		cursmy = v8;
		return;
	}
	if ( MouseY > 352 )
	{
		CheckPanelInfo();
		return;
	}
	if ( doomflag )
		return;
	if ( invflag && MouseX > 320 )
	{
		pcursinvitem = CheckInvHLight();
		return;
	}
	if ( sbookflag && MouseX > 320 || (chrflag || questlog) && MouseX < 320 )
		return;
	if ( leveltype == DTYPE_TOWN )
	{
		if ( v83 )
		{
			v27 = 112 * v6;
			v78 = 112 * v6;
			v43 = 112 * v6 + v8;
			v45 = dMonster[0][v43 + 1];
			if ( v45 <= 0 )
				goto LABEL_200;
			v11 = v45 - 1;
			cursmx = v6;
			cursmy = v8 + 1;
		}
		else
		{
			v27 = 112 * v6;
			v78 = 112 * v6;
			v43 = 112 * v6 + v8;
			v44 = dMonster[1][v43];
			if ( v44 <= 0 )
				goto LABEL_200;
			v11 = v44 - 1;
			cursmx = v6 + 1;
			cursmy = v8;
		}
		pcursmonst = v11;
LABEL_200:
		v46 = dMonster[0][v43];
		if ( v46 > 0 )
		{
			v11 = v46 - 1;
			cursmx = v6;
			pcursmonst = v46 - 1;
			cursmy = v8;
		}
		v47 = dMonster[1][v43 + 1];
		if ( v47 > 0 )
		{
			v11 = v47 - 1;
			cursmx = v6 + 1;
			pcursmonst = v47 - 1;
			cursmy = v8 + 1;
		}
		if ( !towner[v11]._tSelFlag )
LABEL_205:
			pcursmonst = -1;
LABEL_206:
		if ( pcursmonst != -1 )
		{
LABEL_305:
			v59 = pcursmonst;
			goto LABEL_306;
		}
LABEL_207:
		if ( v83 )
		{
			v50 = dPlayer[0][v27 + 1 + v8];
			if ( v50 )
			{
				v49 = v50 <= 0 ? -1 - v50 : v50 - 1;
				if ( v49 != myplr && plr[v49]._pHitPoints )
				{
					cursmx = v6;
					cursmy = v8 + 1;
					goto LABEL_222;
				}
			}
		}
		else
		{
			v48 = dPlayer[1][v27 + v8];
			if ( v48 )
			{
				v49 = v48 <= 0 ? -1 - v48 : v48 - 1;
				if ( v49 != myplr && plr[v49]._pHitPoints )
				{
					cursmy = v8;
					cursmx = v6 + 1;
LABEL_222:
					pcursplr = v49;
					goto LABEL_223;
				}
			}
		}
LABEL_223:
		v51 = dPlayer[0][v27 + v8];
		if ( v51 )
		{
			v52 = v51 <= 0 ? -1 - v51 : v51 - 1;
			if ( v52 != myplr )
			{
				cursmx = v6;
				cursmy = v8;
				pcursplr = v52;
			}
		}
		if ( dFlags[0][v27 + v8] & DFLAG_DEAD_PLAYER )
		{
			v53 = 0;
			v54 = &plr[0].WorldY;
			do
			{
				if ( *(v54 - 1) == v6 && *v54 == v8 && v53 != myplr )
				{
					cursmx = v6;
					cursmy = v8;
					pcursplr = v53;
				}
				v54 += 5430;
				++v53;
			}
			while ( (signed int)v54 < (signed int)&plr[4].WorldY );
		}
		if ( pcurs == CURSOR_RESURRECT )
		{
			v79 = -1;
			v77 = &dFlags[-1][v27 + v8]; /* v77 = &nBlockTable[v27 + 1944 + v8]; check */
			do
			{
				v80 = -1;
				v55 = v8 - 1;
				do
				{
					if ( v77[v80] & DFLAG_DEAD_PLAYER )
					{
						v82 = 0;
						v56 = &plr[0].WorldY;
						do
						{
							if ( *(v56 - 1) == v6 + v79 && *v56 == v55 && v82 != myplr )
							{
								cursmx = v6 + v79;
								cursmy = v55;
								pcursplr = v82;
							}
							++v82;
							v56 += 5430;
						}
						while ( (signed int)v56 < (signed int)&plr[4].WorldY );
					}
					++v80;
					++v55;
				}
				while ( v80 < 2 );
				++v79;
				v77 += 112;
			}
			while ( v79 < 2 );
			v27 = v78;
		}
		v57 = dPlayer[1][v27 + 1 + v8];
		if ( v57 )
		{
			v58 = v57 <= 0 ? -1 - v57 : v57 - 1;
			if ( v58 != myplr && plr[v58]._pHitPoints )
			{
				pcursplr = v58;
				cursmx = v6 + 1;
				cursmy = v8 + 1;
			}
		}
		v59 = pcursmonst;
		if ( pcursmonst != -1 )
		{
LABEL_285:
			if ( pcursplr == -1 )
				goto LABEL_286;
LABEL_306:
			if ( pcurs == CURSOR_IDENTIFY )
			{
				pcursobj = -1;
				v59 = -1;
				pcursitem = -1;
				pcursmonst = -1;
				cursmx = v6;
				cursmy = v8;
			}
			if ( v59 != -1 )
			{
				if ( monster[v59]._mFlags & 0x20 )
					pcursmonst = -1;
			}
			return;
		}
		if ( pcursplr != pcursmonst ) /* check in future */
			goto LABEL_306;
		if ( v83 )
		{
			v62 = dObject[0][v27 + 1 + v8];
			if ( !v62 )
				goto LABEL_272;
			v61 = v62 <= 0 ? -1 - v62 : v62 - 1;
			if ( object[v61]._oSelFlag < 2 )
				goto LABEL_272;
			cursmx = v6;
			cursmy = v8 + 1;
		}
		else
		{
			v60 = dObject[1][v27 + v8];
			if ( !v60 )
				goto LABEL_272;
			v61 = v60 <= 0 ? -1 - v60 : v60 - 1;
			if ( object[v61]._oSelFlag < 2 )
				goto LABEL_272;
			cursmy = v8;
			cursmx = v6 + 1;
		}
		pcursobj = v61;
LABEL_272:
		v63 = dObject[0][v27 + v8];
		if ( v63 )
		{
			v64 = v63 <= 0 ? -1 - v63 : v63 - 1;
			v65 = object[v64]._oSelFlag;
			if ( v65 == 1 || v65 == 3 )
			{
				cursmx = v6;
				cursmy = v8;
				pcursobj = v64;
			}
		}
		v66 = dObject[1][v27 + 1 + v8];
		if ( !v66 || (v66 <= 0 ? (v67 = -1 - v66) : (v67 = v66 - 1), object[v67]._oSelFlag < 2) )
		{
LABEL_286:
			if ( pcursobj != -1 || pcursmonst != -1 )
				goto LABEL_306;
			if ( v83 )
			{
				v70 = dItem[0][v27 + 1 + v8];
				if ( v70 <= 0 )
					goto LABEL_296;
				v69 = v70 - 1;
				if ( item[v69]._iSelFlag < 2 )
					goto LABEL_296;
				cursmx = v6;
				cursmy = v8 + 1;
			}
			else
			{
				v68 = dItem[1][v27 + v8];
				if ( v68 <= 0 )
					goto LABEL_296;
				v69 = v68 - 1;
				if ( item[v69]._iSelFlag < 2 )
					goto LABEL_296;
				cursmy = v8;
				cursmx = v6 + 1;
			}
			pcursitem = v69;
LABEL_296:
			v71 = dItem[0][v27 + v8];
			if ( v71 > 0 )
			{
				v72 = v71 - 1;
				v73 = item[v72]._iSelFlag;
				if ( v73 == 1 || v73 == 3 )
				{
					cursmx = v6;
					cursmy = v8;
					pcursitem = v72;
				}
			}
			v74 = dItem[1][v27 + 1 + v8];
			if ( v74 > 0 )
			{
				v75 = v74 - 1;
				if ( item[v75]._iSelFlag >= 2 )
				{
					pcursitem = v75;
					cursmx = v6 + 1;
					cursmy = v8 + 1;
				}
			}
			if ( pcursitem != -1 )
				goto LABEL_306;
			cursmx = v6;
			cursmy = v8;
			CheckTrigForce();
			CheckTown();
			CheckRportal();
			goto LABEL_305;
		}
		pcursobj = v67;
		cursmx = v6 + 1;
		cursmy = v8 + 1;
		goto LABEL_285;
	}
	if ( v10 == -1 )
		goto LABEL_128;
	v12 = 112 * v6 + v8;
	v81 = 112 * v6 + v8;
	v13 = 112 * v6 + v8;
	if ( v83 )
	{
		v14 = v13;
		v15 = dMonster[1][v14 + 2];
		if ( !v15 )
			goto LABEL_74;
		v16 = (dFlags[1][v12 + 2] & DFLAG_LIT) == 0;
	}
	else
	{
		v14 = v13;
		v15 = dMonster[2][v14 + 1];
		if ( !v15 )
			goto LABEL_74;
		v16 = (dFlags[2][v12 + 1] & DFLAG_LIT) == 0;
	}
	if ( !v16 )
	{
		v17 = v15 <= 0 ? -1 - v15 : v15 - 1;
		if ( v17 == dword_4B8CCC
		  && (signed int)(monster[v17]._mhitpoints & 0xFFFFFFC0) > 0
		  && monster[v17].MData->mSelFlag & 4 )
		{
			v11 = v17;
			cursmx = v6 + 1;
			cursmy = v8 + 2;
			pcursmonst = v17;
		}
	}
LABEL_74:
	v18 = dMonster[2][v14 + 2];
	if ( v18 && dFlags[2][v81 + 2] & DFLAG_LIT )
	{
		v19 = v18 <= 0 ? -1 - v18 : v18 - 1;
		if ( v19 == dword_4B8CCC
		  && (signed int)(monster[v19]._mhitpoints & 0xFFFFFFC0) > 0
		  && monster[v19].MData->mSelFlag & 4 )
		{
			v11 = v19;
			cursmx = v6 + 2;
			cursmy = v8 + 2;
			pcursmonst = v19;
		}
	}
	if ( v83 )
	{
		v22 = dMonster[0][v14 + 1];
		if ( v22 && dFlags[0][v81 + 1] & DFLAG_LIT )
		{
			v21 = v22 <= 0 ? -1 - v22 : v22 - 1;
			if ( v21 == dword_4B8CCC
			  && (signed int)(monster[v21]._mhitpoints & 0xFFFFFFC0) > 0
			  && monster[v21].MData->mSelFlag & 2 )
			{
				cursmx = v6;
				cursmy = v8 + 1;
				goto LABEL_102;
			}
		}
	}
	else
	{
		v20 = dMonster[1][v14];
		if ( v20 && dFlags[1][v81] & DFLAG_LIT )
		{
			v21 = v20 <= 0 ? -1 - v20 : v20 - 1;
			if ( v21 == dword_4B8CCC
			  && (signed int)(monster[v21]._mhitpoints & 0xFFFFFFC0) > 0
			  && monster[v21].MData->mSelFlag & 2 )
			{
				cursmy = v8;
				cursmx = v6 + 1;
LABEL_102:
				v11 = v21;
				pcursmonst = v21;
				goto LABEL_103;
			}
		}
	}
LABEL_103:
	v23 = dMonster[0][v14];
	if ( v23 && dFlags[0][v81] & DFLAG_LIT )
	{
		v24 = v23 <= 0 ? -1 - v23 : v23 - 1;
		if ( v24 == dword_4B8CCC
		  && (signed int)(monster[v24]._mhitpoints & 0xFFFFFFC0) > 0
		  && monster[v24].MData->mSelFlag & 1 )
		{
			v11 = v24;
			cursmx = v6;
			cursmy = v8;
			pcursmonst = v24;
		}
	}
	v25 = dMonster[1][v14 + 1];
	if ( v25 && dFlags[1][v81 + 1] & DFLAG_LIT )
	{
		v26 = v25 <= 0 ? -1 - v25 : v25 - 1;
		if ( v26 == dword_4B8CCC
		  && (signed int)(monster[v26]._mhitpoints & 0xFFFFFFC0) > 0
		  && monster[v26].MData->mSelFlag & 2 )
		{
			v11 = v26;
			cursmx = v6 + 1;
			cursmy = v8 + 1;
			pcursmonst = v26;
		}
	}
	if ( v11 == -1 )
		goto LABEL_128;
	if ( monster[v11]._mFlags & 1 )
	{
		v11 = -1;
		cursmx = v6;
		pcursmonst = -1;
		cursmy = v8;
	}
	if ( v11 == -1 )
		goto LABEL_128;
	if ( monster[v11]._mFlags & 0x20 )
	{
		v11 = -1;
		pcursmonst = -1;
	}
	if ( v11 == -1 )
	{
LABEL_128:
		v27 = 112 * v6;
		v78 = 112 * v6;
		if ( v83 )
		{
			v28 = v27 + v8;
			v32 = dMonster[1][v28 + 2];
			if ( v32 && dFlags[1][v27 + 2 + v8] & DFLAG_LIT )
			{
				v30 = v32 <= 0 ? -1 - v32 : v32 - 1;
				if ( (signed int)(monster[v30]._mhitpoints & 0xFFFFFFC0) > 0 && monster[v30].MData->mSelFlag & 4 )
				{
					cursmx = v6 + 1;
					v31 = v8 + 2;
					goto LABEL_145;
				}
			}
		}
		else
		{
			v28 = v27 + v8;
			v29 = dMonster[2][v28 + 1];
			if ( v29 && dFlags[2][v27 + 1 + v8] & DFLAG_LIT )
			{
				v30 = v29 <= 0 ? -1 - v29 : v29 - 1;
				if ( (signed int)(monster[v30]._mhitpoints & 0xFFFFFFC0) > 0 && monster[v30].MData->mSelFlag & 4 )
				{
					cursmx = v6 + 2;
					v31 = v8 + 1;
LABEL_145:
					cursmy = v31;
					pcursmonst = v30;
					goto LABEL_146;
				}
			}
		}
LABEL_146:
		v33 = dMonster[2][v28 + 2];
		if ( v33 && dFlags[2][v27 + 2 + v8] & DFLAG_LIT )
		{
			v34 = v33 <= 0 ? -1 - v33 : v33 - 1;
			if ( (signed int)(monster[v34]._mhitpoints & 0xFFFFFFC0) > 0 && monster[v34].MData->mSelFlag & 4 )
			{
				pcursmonst = v34;
				cursmx = v6 + 2;
				cursmy = v8 + 2;
			}
		}
		if ( v83 )
		{
			v37 = dMonster[0][v28 + 1];
			if ( v37 && dFlags[0][v27 + 1 + v8] & DFLAG_LIT )
			{
				v36 = v37 <= 0 ? -1 - v37 : v37 - 1;
				if ( (signed int)(monster[v36]._mhitpoints & 0xFFFFFFC0) > 0 && monster[v36].MData->mSelFlag & 2 )
				{
					cursmx = v6;
					cursmy = v8 + 1;
					goto LABEL_171;
				}
			}
		}
		else
		{
			v35 = dMonster[1][v28];
			if ( v35 && dFlags[1][v27 + v8] & DFLAG_LIT )
			{
				v36 = v35 <= 0 ? -1 - v35 : v35 - 1;
				if ( (signed int)(monster[v36]._mhitpoints & 0xFFFFFFC0) > 0 && monster[v36].MData->mSelFlag & 2 )
				{
					cursmy = v8;
					cursmx = v6 + 1;
LABEL_171:
					pcursmonst = v36;
					goto LABEL_172;
				}
			}
		}
LABEL_172:
		v38 = dMonster[0][v28];
		if ( v38 && dFlags[0][v27 + v8] & DFLAG_LIT )
		{
			v39 = v38 <= 0 ? -1 - v38 : v38 - 1;
			if ( (signed int)(monster[v39]._mhitpoints & 0xFFFFFFC0) > 0 && monster[v39].MData->mSelFlag & 1 )
			{
				cursmx = v6;
				cursmy = v8;
				pcursmonst = v39;
			}
		}
		v40 = dMonster[1][v28 + 1];
		if ( v40 && dFlags[1][v27 + 1 + v8] & DFLAG_LIT )
		{
			v41 = v40 <= 0 ? -1 - v40 : v40 - 1;
			if ( (signed int)(monster[v41]._mhitpoints & 0xFFFFFFC0) > 0 && monster[v41].MData->mSelFlag & 2 )
			{
				pcursmonst = v41;
				cursmx = v6 + 1;
				cursmy = v8 + 1;
			}
		}
		v42 = pcursmonst;
		if ( pcursmonst == -1 )
			goto LABEL_207;
		if ( monster[pcursmonst]._mFlags & 1 )
		{
			v42 = -1;
			cursmx = v6;
			pcursmonst = -1;
			cursmy = v8;
		}
		if ( v42 == -1 )
			goto LABEL_207;
		if ( monster[v42]._mFlags & 0x20 )
			goto LABEL_205;
		goto LABEL_206;
	}
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
// 5BB1ED: using guessed type char leveltype;
// 69BD04: using guessed type int questlog;
