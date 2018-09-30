//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
void *tbuff;
#endif

void __fastcall LoadGame(BOOL firstflag)
{
	int v1; // esi
	int v2; // edi
	int v5; // ebx
	int v6; // eax
	int v7; // eax
	int v8; // ecx
	bool v9; // sf
	unsigned char v10; // of
	int *v11; // esi
	int *v12; // esi
	int i; // esi
	int *v14; // esi
	int *v15; // esi
	int j; // esi
	int *v17; // esi
	int *v18; // esi
	int k; // esi
	int l; // esi
	signed int v21; // esi
	int m; // esi
	int v23; // esi
	int *v24; // esi
	int *v25; // esi
	int n; // esi
	int *v27; // esi
	char *v29; // edi
	char *v30; // edi
	char *v31; // edi
	char *v32; // edi
	int (*v33)[112]; // ebx
	_DWORD *v34; // edi
	char *v35; // edi
	char *v36; // edi
	char *v37; // edi
	char *v38; // edi
	signed int v39; // ebx
	bool *v40; // edi
	char *v41; // edi
	int v42; // esi
	char dst[260]; // [esp+0h] [ebp-120h]
	int len; // [esp+104h] [ebp-1Ch]
	int v46; // [esp+108h] [ebp-18h]
	int v47; // [esp+10Ch] [ebp-14h]
	void *ptr; // [esp+110h] [ebp-10h]
	int v49; // [esp+114h] [ebp-Ch]
	int from_save; // [esp+118h] [ebp-8h]
	int quest_num; // [esp+11Ch] [ebp-4h]

	FreeGameMem();
	pfile_remove_temp_files();
	pfile_get_game_name(dst);
	ptr = pfile_read(dst, &len);
	tbuff = ptr;
	if ( ILoad_2() != 'RETL' )
		TermMsg("Invalid save file");
	setlevel = OLoad();
	setlvlnum = ILoad();
	currlevel = ILoad();
	leveltype = ILoad();
	v1 = ILoad();
	v2 = ILoad();
	invflag = OLoad();
	chrflag = OLoad();
	v5 = ILoad();
	v47 = ILoad();
	v49 = ILoad();
	v6 = ILoad();
	quest_num = 0;
	v46 = v6;
	do
	{
		*(int *)((char *)glSeedTbl + quest_num) = ILoad_2();
		v7 = ILoad();
		v8 = quest_num;
		quest_num += 4;
		v10 = __OFSUB__(quest_num, 68);
		v9 = quest_num - 68 < 0;
		*(int *)((char *)gnLevelTypeTbl + v8) = v7;
	}
	while ( v9 ^ v10 );
	LoadPlayer(myplr);
	quest_num = 0;
	do
		LoadQuest(quest_num++);
	while ( quest_num < MAXQUESTS );
	quest_num = 0;
	do
		LoadPortal(quest_num++);
	while ( quest_num < MAXPORTAL );
	LoadGameLevel(firstflag, 4);
	SyncInitPlr(myplr);
	SyncPlrAnim(myplr);
	ViewX = v1;
	numitems = v47;
	nummissiles = v49;
	ViewY = v2;
	nummonsters = v5;
	nobjects = v46;
	v11 = monstkills;
	do
	{
		*v11 = ILoad_2();
		++v11;
	}
	while ( (signed int)v11 < (signed int)&monstkills[MAXMONSTERS] );
	if ( leveltype )
	{
		v12 = monstactive;
		do
		{
			*v12 = ILoad();
			++v12;
		}
		while ( (signed int)v12 < (signed int)&monstactive[MAXMONSTERS] );
		for ( i = 0; i < nummonsters; ++i )
			LoadMonster(monstactive[i]);
		v14 = missileactive;
		do
		{
			*v14 = BLoad();
			++v14;
		}
		while ( (signed int)v14 < (signed int)&missileactive[MAXMISSILES] );
		v15 = missileavail;
		do
		{
			*v15 = BLoad();
			++v15;
		}
		while ( (signed int)v15 < (signed int)&missileavail[MAXMISSILES] );
		for ( j = 0; j < nummissiles; ++j )
			LoadMissile(missileactive[j]);
		v17 = objectactive;
		do
		{
			*v17 = BLoad();
			++v17;
		}
		while ( (signed int)v17 < (signed int)&objectactive[MAXOBJECTS] );
		v18 = objectavail;
		do
		{
			*v18 = BLoad();
			++v18;
		}
		while ( (signed int)v18 < (signed int)&objectavail[MAXOBJECTS] );
		for ( k = 0; k < nobjects; ++k )
			LoadObject(objectactive[k]);
		for ( l = 0; l < nobjects; ++l )
			SyncObjectAnim(objectactive[l]);
		numlights = ILoad();
		v21 = 0;
		do
			lightactive[v21++] = BLoad();
		while ( v21 < 32 );
		for ( m = 0; m < numlights; ++m )
			LoadLighting((unsigned char)lightactive[m]);
		visionid = ILoad();
		v23 = 0;
		numvision = ILoad();
		if ( numvision > 0 )
		{
			do
				LoadVision(v23++);
			while ( v23 < numvision );
		}
	}
	v24 = itemactive;
	do
	{
		*v24 = BLoad();
		++v24;
	}
	while ( (signed int)v24 < (signed int)&itemactive[MAXITEMS] );
	v25 = itemavail;
	do
	{
		*v25 = BLoad();
		++v25;
	}
	while ( (signed int)v25 < (signed int)&itemavail[MAXITEMS] );
	for ( n = 0; n < numitems; ++n )
		LoadItem(itemactive[n]);
	v27 = UniqueItemFlag;
	do
	{
		*v27 = OLoad();
		++v27;
	}
	while ( (signed int)v27 < (signed int)&UniqueItemFlag[128] );
	quest_num = 0;
	do
	{
		from_save = 112;
		v29 = (char *)dTransVal + quest_num;
		do
		{
			*v29 = BLoad();
			v29 += 112;
			--from_save;
		}
		while ( from_save );
		++quest_num;
	}
	while ( quest_num < 112 );
	quest_num = 0;
	do
	{
		from_save = 112;
		v30 = (char *)dFlags + quest_num;
		do
		{
			*v30 = BLoad();
			v30 += 112;
			--from_save;
		}
		while ( from_save );
		++quest_num;
	}
	while ( quest_num < 112 );
	quest_num = 0;
	do
	{
		from_save = 112;
		v31 = (char *)dPlayer + quest_num;
		do
		{
			*v31 = BLoad();
			v31 += 112;
			--from_save;
		}
		while ( from_save );
		++quest_num;
	}
	while ( quest_num < 112 );
	quest_num = 0;
	do
	{
		from_save = 112;
		v32 = (char *)dItem + quest_num;
		do
		{
			*v32 = BLoad();
			v32 += 112;
			--from_save;
		}
		while ( from_save );
		++quest_num;
	}
	while ( quest_num < 112 );
	if ( leveltype )
	{
		v33 = dMonster;
		do
		{
			v34 = (unsigned int *)v33;
			from_save = 112;
			do
			{
				*v34 = ILoad();
				v34 += 112;
				--from_save;
			}
			while ( from_save );
			v33 = (int (*)[112])((char *)v33 + 4);
		}
		while ( (signed int)v33 < (signed int)dMonster[1] );
		quest_num = 0;
		do
		{
			from_save = 112;
			v35 = (char *)dDead + quest_num;
			do
			{
				*v35 = BLoad();
				v35 += 112;
				--from_save;
			}
			while ( from_save );
			++quest_num;
		}
		while ( quest_num < 112 );
		quest_num = 0;
		do
		{
			from_save = 112;
			v36 = (char *)dObject + quest_num;
			do
			{
				*v36 = BLoad();
				v36 += 112;
				--from_save;
			}
			while ( from_save );
			++quest_num;
		}
		while ( quest_num < 112 );
		quest_num = 0;
		do
		{
			from_save = 112;
			v37 = (char *)dTransVal + quest_num;
			do
			{
				*v37 = BLoad();
				v37 += 112;
				--from_save;
			}
			while ( from_save );
			++quest_num;
		}
		while ( quest_num < 112 );
		quest_num = 0;
		do
		{
			from_save = 112;
			v38 = (char *)dTransVal2 + quest_num;
			do
			{
				*v38 = BLoad();
				v38 += 112;
				--from_save;
			}
			while ( from_save );
			++quest_num;
		}
		while ( quest_num < 112 );
		v39 = 0;
		do
		{
			v40 = (bool *)automapview + v39;
			from_save = 40;
			do
			{
				*v40 = OLoad();
				v40 += 40;
				--from_save;
			}
			while ( from_save );
			++v39;
		}
		while ( v39 < 40 );
		quest_num = 0;
		do
		{
			from_save = 112;
			v41 = (char *)dMissile + quest_num;
			do
			{
				*v41 = BLoad();
				v41 += 112;
				--from_save;
			}
			while ( from_save );
			++quest_num;
		}
		while ( quest_num < 112 );
	}
	numpremium = ILoad();
	premiumlevel = ILoad();
	v42 = 0;
	do
		LoadPremium(v42++);
	while ( v42 < 6 );
	automapflag = OLoad();
	AutoMapScale = ILoad();
	mem_free_dbg(ptr);
	AutomapZoomReset();
	ResyncQuests();
	if ( leveltype )
		ProcessLightList();
	RedoPlayerVision();
	ProcessVisionList();
	missiles_process_charge();
	ResetPal();
	SetCursor(CURSOR_HAND);
	gbProcessPlayers = 1;
}
// 5256A0: using guessed type int gbProcessPlayers;
// 5BB1ED: using guessed type char leveltype;
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;

char __cdecl BLoad()
{
	char result; // al

	result = *(_BYTE *)tbuff;
	tbuff = (char *)tbuff + 1;
	return result;
}

int __cdecl ILoad()
{
	int v0; // eax
	int v1; // eax
	unsigned short v2; // dx
	int result; // eax

	v0 = *(unsigned char *)tbuff << 24;
	tbuff = (char *)tbuff + 1;
	v1 = (*(unsigned char *)tbuff << 16) | v0;
	_LOBYTE(v2) = 0;
	tbuff = (char *)tbuff + 1;
	_HIBYTE(v2) = *(_BYTE *)tbuff;
	tbuff = (char *)tbuff + 1;
	result = *(unsigned char *)tbuff | v2 | v1;
	tbuff = (char *)tbuff + 1;
	return result;
}

int __cdecl ILoad_2()
{
	int v0; // eax
	int v1; // eax
	unsigned short v2; // dx
	int result; // eax

	v0 = *(unsigned char *)tbuff << 24;
	tbuff = (char *)tbuff + 1;
	v1 = (*(unsigned char *)tbuff << 16) | v0;
	_LOBYTE(v2) = 0;
	tbuff = (char *)tbuff + 1;
	_HIBYTE(v2) = *(_BYTE *)tbuff;
	tbuff = (char *)tbuff + 1;
	result = *(unsigned char *)tbuff | v2 | v1;
	tbuff = (char *)tbuff + 1;
	return result;
}

bool __cdecl OLoad()
{
	char v0; // cl
	bool result; // al

	v0 = *(_BYTE *)tbuff;
	tbuff = (char *)tbuff + 1;
	result = 1;
	if ( v0 != 1 )
		result = 0;
	return result;
}

void __fastcall LoadPlayer(int i)
{
	memcpy(&plr[i], tbuff, 0x54B0u);
	tbuff = (char *)tbuff + 21680;
}

void __fastcall LoadMonster(int i)
{
	int v1; // edi

	v1 = i;
	memcpy(&monster[i], tbuff, 0xD8u);
	tbuff = (char *)tbuff + 216;
	SyncMonsterAnim(v1);
}

void __fastcall LoadMissile(int i)
{
	memcpy(&missile[i], tbuff, 0xB0u);
	tbuff = (char *)tbuff + 176;
}

void __fastcall LoadObject(int i)
{
	memcpy(&object[i], tbuff, 0x78u);
	tbuff = (char *)tbuff + 120;
}

void __fastcall LoadItem(int i)
{
	int v1; // edi

	v1 = i;
	memcpy(&item[i], tbuff, 0x170u);
	tbuff = (char *)tbuff + 368;
	GetItemFrm(v1);
}

void __fastcall LoadPremium(int i)
{
	memcpy(&premiumitem[i], tbuff, 0x170u);
	tbuff = (char *)tbuff + 368;
}

void __fastcall LoadQuest(int i)
{
	memcpy(&quests[i], tbuff, 0x18u);
	tbuff = (char *)tbuff + 24;
	ReturnLvlX = ILoad();
	ReturnLvlY = ILoad();
	ReturnLvl = ILoad();
	ReturnLvlT = ILoad();
	DoomQuestState = ILoad();
}

void __fastcall LoadLighting(int i)
{
	memcpy(&LightList[i], tbuff, 0x34u);
	tbuff = (char *)tbuff + 52;
}

void __fastcall LoadVision(int i)
{
	memcpy(&VisionList[i], tbuff, 0x34u);
	tbuff = (char *)tbuff + 52;
}

void __fastcall LoadPortal(int i)
{
	memcpy(&portal[i], tbuff, 0x18u);
	tbuff = (char *)tbuff + 24;
}

void __cdecl SaveGame()
{
	int v0; // eax
	signed int v1; // ebx
	signed int v2; // esi
	int v3; // esi
	int v4; // esi
	int *v5; // esi
	int *v6; // esi
	int i; // esi
	int *v8; // esi
	int *v9; // esi
	int j; // esi
	int *v11; // esi
	int *v12; // esi
	int k; // esi
	signed int v14; // esi
	int l; // esi
	int m; // esi
	int *v17; // esi
	int *v18; // esi
	int n; // esi
	int *v20; // esi
	char *v21; // edi
	signed int v22; // ebx
	_BYTE *v23; // edi
	signed int v24; // ebx
	char *v25; // edi
	signed int v26; // ebx
	char *v27; // edi
	int (*v28)[112]; // ebx
	int *v29; // edi
	signed int v30; // ebx
	char *v31; // edi
	signed int v32; // ebx
	char *v33; // edi
	signed int v34; // ebx
	char *v35; // edi
	signed int v36; // ebx
	char *v37; // edi
	signed int v38; // ebx
	unsigned char *v39; // edi
	signed int v40; // ebx
	char *v41; // edi
	int v42; // esi
	void *v43; // esi
	int v44; // eax
	char v45[260]; // [esp+Ch] [ebp-10Ch]
	void *ptr; // [esp+110h] [ebp-8h]
	int v47; // [esp+114h] [ebp-4h]

	v0 = codec_get_encoded_len(262147); /* FILEBUFF */
	ptr = DiabloAllocPtr(v0);
	tbuff = ptr;
	ISave_2('RETL');
	OSave(setlevel);
	ISave((unsigned char)setlvlnum);
	ISave(currlevel);
	ISave((unsigned char)leveltype);
	ISave(ViewX);
	ISave(ViewY);
	OSave(invflag);
	OSave(chrflag);
	ISave(nummonsters);
	ISave(numitems);
	ISave(nummissiles);
	ISave(nobjects);
	v1 = 0;
	v2 = 0;
	do
	{
		ISave_2(glSeedTbl[v2]);
		ISave(gnLevelTypeTbl[v2]);
		++v2;
	}
	while ( v2 < NUMLEVELS );
	SavePlayer(myplr);
	v3 = 0;
	do
		SaveQuest(v3++);
	while ( v3 < MAXQUESTS );
	v4 = 0;
	do
		SavePortal(v4++);
	while ( v4 < MAXPORTAL );
	v5 = monstkills;
	do
	{
		ISave_2(*v5);
		++v5;
	}
	while ( (signed int)v5 < (signed int)&monstkills[MAXMONSTERS] );
	if ( leveltype )
	{
		v6 = monstactive;
		do
		{
			ISave(*v6);
			++v6;
		}
		while ( (signed int)v6 < (signed int)&monstactive[MAXMONSTERS] );
		for ( i = 0; i < nummonsters; ++i )
			SaveMonster(monstactive[i]);
		v8 = missileactive;
		do
		{
			BSave(*(_BYTE *)v8);
			++v8;
		}
		while ( (signed int)v8 < (signed int)&missileactive[MAXMISSILES] );
		v9 = missileavail;
		do
		{
			BSave(*(_BYTE *)v9);
			++v9;
		}
		while ( (signed int)v9 < (signed int)&missileavail[MAXMISSILES] );
		for ( j = 0; j < nummissiles; ++j )
			SaveMissile(missileactive[j]);
		v11 = objectactive;
		do
		{
			BSave(*(_BYTE *)v11);
			++v11;
		}
		while ( (signed int)v11 < (signed int)&objectactive[MAXOBJECTS] );
		v12 = objectavail;
		do
		{
			BSave(*(_BYTE *)v12);
			++v12;
		}
		while ( (signed int)v12 < (signed int)&objectavail[MAXOBJECTS] );
		for ( k = 0; k < nobjects; ++k )
			SaveObject(objectactive[k]);
		ISave(numlights);
		v14 = 0;
		do
			BSave(lightactive[v14++]);
		while ( v14 < 32 );
		for ( l = 0; l < numlights; ++l )
			SaveLighting((unsigned char)lightactive[l]);
		ISave(visionid);
		ISave(numvision);
		for ( m = 0; m < numvision; ++m )
			SaveVision(m);
	}
	v17 = itemactive;
	do
	{
		BSave(*(_BYTE *)v17);
		++v17;
	}
	while ( (signed int)v17 < (signed int)&itemactive[MAXITEMS] );
	v18 = itemavail;
	do
	{
		BSave(*(_BYTE *)v18);
		++v18;
	}
	while ( (signed int)v18 < (signed int)&itemavail[MAXITEMS] );
	for ( n = 0; n < numitems; ++n )
		SaveItem(itemactive[n]);
	v20 = UniqueItemFlag;
	do
	{
		OSave(*v20);
		++v20;
	}
	while ( (signed int)v20 < (signed int)&UniqueItemFlag[128] );
	do
	{
		v21 = (char *)dTransVal + v1;
		v47 = 112;
		do
		{
			BSave(*v21);
			v21 += 112;
			--v47;
		}
		while ( v47 );
		++v1;
	}
	while ( v1 < 112 );
	v22 = 0;
	do
	{
		v23 = (unsigned char *)dFlags + v22;
		v47 = 112;
		do
		{
			BSave(*v23 & 0xF8);
			v23 += 112;
			--v47;
		}
		while ( v47 );
		++v22;
	}
	while ( v22 < 112 );
	v24 = 0;
	do
	{
		v25 = (char *)dPlayer + v24;
		v47 = 112;
		do
		{
			BSave(*v25);
			v25 += 112;
			--v47;
		}
		while ( v47 );
		++v24;
	}
	while ( v24 < 112 );
	v26 = 0;
	do
	{
		v27 = (char *)dItem + v26;
		v47 = 112;
		do
		{
			BSave(*v27);
			v27 += 112;
			--v47;
		}
		while ( v47 );
		++v26;
	}
	while ( v26 < 112 );
	if ( leveltype )
	{
		v28 = dMonster;
		do
		{
			v29 = (int *)v28;
			v47 = 112;
			do
			{
				ISave(*v29);
				v29 += 112;
				--v47;
			}
			while ( v47 );
			v28 = (int (*)[112])((char *)v28 + 4);
		}
		while ( (signed int)v28 < (signed int)dMonster[1] );
		v30 = 0;
		do
		{
			v31 = (char *)dDead + v30;
			v47 = 112;
			do
			{
				BSave(*v31);
				v31 += 112;
				--v47;
			}
			while ( v47 );
			++v30;
		}
		while ( v30 < 112 );
		v32 = 0;
		do
		{
			v33 = (char *)dObject + v32;
			v47 = 112;
			do
			{
				BSave(*v33);
				v33 += 112;
				--v47;
			}
			while ( v47 );
			++v32;
		}
		while ( v32 < 112 );
		v34 = 0;
		do
		{
			v35 = (char *)dTransVal + v34;
			v47 = 112;
			do
			{
				BSave(*v35);
				v35 += 112;
				--v47;
			}
			while ( v47 );
			++v34;
		}
		while ( v34 < 112 );
		v36 = 0;
		do
		{
			v37 = (char *)dTransVal2 + v36;
			v47 = 112;
			do
			{
				BSave(*v37);
				v37 += 112;
				--v47;
			}
			while ( v47 );
			++v36;
		}
		while ( v36 < 112 );
		v38 = 0;
		do
		{
			v39 = (unsigned char *)automapview + v38;
			v47 = 40;
			do
			{
				OSave(*v39);
				v39 += 40;
				--v47;
			}
			while ( v47 );
			++v38;
		}
		while ( v38 < 40 );
		v40 = 0;
		do
		{
			v41 = (char *)dMissile + v40;
			v47 = 112;
			do
			{
				BSave(*v41);
				v41 += 112;
				--v47;
			}
			while ( v47 );
			++v40;
		}
		while ( v40 < 112 );
	}
	ISave(numpremium);
	ISave(premiumlevel);
	v42 = 0;
	do
		SavePremium(v42++);
	while ( v42 < 6 );
	OSave(automapflag);
	ISave(AutoMapScale);
	pfile_get_game_name(v45);
	v43 = ptr;
	v44 = codec_get_encoded_len((_BYTE *)tbuff - (_BYTE *)ptr);
	pfile_write_save_file(v45, v43, (_BYTE *)tbuff - (_BYTE *)v43, v44);
	mem_free_dbg(v43);
	*(_DWORD *)&gbValidSaveFile = 1;
	pfile_rename_temp_to_perm();
	pfile_write_hero();
}
// 5BB1ED: using guessed type char leveltype;
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;

void __fastcall BSave(char v)
{
	*(_BYTE *)tbuff = v;
	tbuff = (char *)tbuff + 1;
}

void __fastcall ISave(int v)
{
	*(_BYTE *)tbuff = _HIBYTE(v);
	tbuff = (char *)tbuff + 1;
	*(_BYTE *)tbuff = BYTE2(v);
	tbuff = (char *)tbuff + 1;
	*(_BYTE *)tbuff = BYTE1(v);
	tbuff = (char *)tbuff + 1;
	*(_BYTE *)tbuff = v;
	tbuff = (char *)tbuff + 1;
}

void __fastcall ISave_2(int v)
{
	*(_BYTE *)tbuff = _HIBYTE(v);
	tbuff = (char *)tbuff + 1;
	*(_BYTE *)tbuff = BYTE2(v);
	tbuff = (char *)tbuff + 1;
	*(_BYTE *)tbuff = BYTE1(v);
	tbuff = (char *)tbuff + 1;
	*(_BYTE *)tbuff = v;
	tbuff = (char *)tbuff + 1;
}

void __fastcall OSave(unsigned char v)
{
	if ( v )
		*(_BYTE *)tbuff = 1;
	else
		*(_BYTE *)tbuff = 0;
	tbuff = (char *)tbuff + 1;
}

void __fastcall SavePlayer(int i)
{
	memcpy(tbuff, &plr[i], 0x54B0u);
	tbuff = (char *)tbuff + 21680;
}

void __fastcall SaveMonster(int i)
{
	memcpy(tbuff, &monster[i], 0xD8u);
	tbuff = (char *)tbuff + 216;
}

void __fastcall SaveMissile(int i)
{
	memcpy(tbuff, &missile[i], 0xB0u);
	tbuff = (char *)tbuff + 176;
}

void __fastcall SaveObject(int i)
{
	memcpy(tbuff, &object[i], 0x78u);
	tbuff = (char *)tbuff + 120;
}

void __fastcall SaveItem(int i)
{
	memcpy(tbuff, &item[i], 0x170u);
	tbuff = (char *)tbuff + 368;
}

void __fastcall SavePremium(int i)
{
	memcpy(tbuff, &premiumitem[i], 0x170u);
	tbuff = (char *)tbuff + 368;
}

void __fastcall SaveQuest(int i)
{
	memcpy(tbuff, &quests[i], 0x18u);
	tbuff = (char *)tbuff + 24;
	ISave(ReturnLvlX);
	ISave(ReturnLvlY);
	ISave(ReturnLvl);
	ISave(ReturnLvlT);
	ISave(DoomQuestState);
}

void __fastcall SaveLighting(int i)
{
	memcpy(tbuff, &LightList[i], 0x34u);
	tbuff = (char *)tbuff + 52;
}

void __fastcall SaveVision(int i)
{
	memcpy(tbuff, &VisionList[i], 0x34u);
	tbuff = (char *)tbuff + 52;
}

void __fastcall SavePortal(int i)
{
	memcpy(tbuff, &portal[i], 0x18u);
	tbuff = (char *)tbuff + 24;
}

void __cdecl SaveLevel()
{
	int v0; // eax
	int i; // esi
	int j; // esi
	void *v47; // esi
	int v48; // eax
	int v49; // eax
	char v50[260]; // [esp+0h] [ebp-10Ch]
	void *SaveBuff; // [esp+104h] [ebp-8h]

	if ( !currlevel )
		glSeedTbl[0] = GetRndSeed();
	v0 = codec_get_encoded_len(262147); /* FILEBUFF */
	SaveBuff = DiabloAllocPtr(v0);
	tbuff = SaveBuff;
	if ( leveltype )
	{
		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				BSave(dDead[j][i]); /* check */
			}
		}
	}
	ISave(nummonsters);
	ISave(numitems);
	ISave(nobjects);

	if ( leveltype )
	{
		for(i = 0; i < MAXMONSTERS; i++)
			ISave(monstactive[i]);

		for(i = 0; i < nummonsters; i++)
			SaveMonster(monstactive[i]);

		for(i = 0; i < MAXOBJECTS; i++)
			BSave(objectactive[i]);

		for(i = 0; i < MAXOBJECTS; i++)
			BSave(objectavail[i]);

		for(i = 0; i < nobjects; i++)
			SaveObject(objectactive[i]);
	}

	for(i = 0; i < MAXITEMS; i++)
		BSave(itemactive[i]);

	for(i = 0; i < MAXITEMS; i++)
		BSave(itemavail[i]);

	for(i = 0; i < numitems; i++)
		SaveItem(itemactive[i]);

	for(i = 0; i < 112; i++)
	{
		for(j = 0; j < 112; j++)
		{
			BSave(dFlags[j][i] & 0xF8);
		}
	}

	for(i = 0; i < 112; i++)
	{
		for(j = 0; j < 112; j++)
		{
			BSave(dItem[j][i]);
		}
	}

	if ( leveltype )
	{
		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				ISave(dMonster[j][i]);
			}
		}

		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				BSave(dObject[j][i]);
			}
		}

		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				BSave(dTransVal[j][i]);
			}
		}

		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				BSave(dTransVal2[j][i]);
			}
		}

		for(i = 0; i < 40; i++)
		{
			for(j = 0; j < 40; j++)
			{
				OSave(automapview[j][i]);
			}
		}

		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				BSave(dMissile[j][i]);
			}
		}
	}
	GetTempLevelNames(v50);
	v47 = SaveBuff;
	v48 = codec_get_encoded_len((_BYTE *)tbuff - (_BYTE *)SaveBuff);
	pfile_write_save_file(v50, v47, (_BYTE *)tbuff - (_BYTE *)v47, v48);
	mem_free_dbg(v47);
	v49 = myplr;
	if ( setlevel )
		plr[v49]._pSLvlVisited[(unsigned char)setlvlnum] = 1;
	else
		plr[v49]._pLvlVisited[currlevel] = 1;
}
// 5BB1ED: using guessed type char leveltype;
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;

void __cdecl LoadLevel()
{
	int i; // esi
	int j; // esi
	char dst[260]; // [esp+Ch] [ebp-10Ch]
	int len; // [esp+110h] [ebp-8h]
	void *LoadBuff; // [esp+114h] [ebp-4h]

	GetPermLevelNames(dst);
	LoadBuff = pfile_read(dst, &len);
	tbuff = LoadBuff;

	if ( leveltype )
	{
		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				dDead[j][i] = BLoad(); /* check */
			}
		}

		SetDead();
	}

	nummonsters = ILoad();
	numitems = ILoad();
	nobjects = ILoad();

	if ( leveltype )
	{
		for(i = 0; i < MAXMONSTERS; i++)
			monstactive[i] = ILoad();

		for(i = 0; i < nummonsters; i++)
			LoadMonster(monstactive[i]);

		for(i = 0; i < MAXOBJECTS; i++)
			objectactive[i] = BLoad();

		for(i = 0; i < MAXOBJECTS; i++)
			objectavail[i] = BLoad();

		for(i = 0; i < nobjects; i++)
			LoadObject(objectactive[i]);

		for(i = 0; i < nobjects; i++)
			SyncObjectAnim(objectactive[i]);
	}

	for(i = 0; i < MAXITEMS; i++)
		itemactive[i] = BLoad();

	for(i = 0; i < MAXITEMS; i++)
		itemavail[i] = BLoad();

	for(i = 0; i < numitems; i++)
		LoadItem(itemactive[i]);

	for(i = 0; i < 112; i++)
	{
		for(j = 0; j < 112; j++)
		{
			dFlags[j][i] = BLoad();
		}
	}

	for(i = 0; i < 112; i++)
	{
		for(j = 0; j < 112; j++)
		{
			dItem[j][i] = BLoad();
		}
	}

	if ( leveltype )
	{
		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				dMonster[j][i] = ILoad();
			}
		}

		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				dObject[j][i] = BLoad();
			}
		}

		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				dTransVal[j][i] = BLoad();
			}
		}

		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				dTransVal2[j][i] = BLoad();
			}
		}

		for(i = 0; i < 40; i++)
		{
			for(j = 0; j < 40; j++)
			{
				automapview[j][i] = OLoad();
			}
		}

		for(i = 0; i < 112; i++)
		{
			for(j = 0; j < 112; j++)
			{
				dMissile[j][i] = 0;
			}
		}
	}

	AutomapZoomReset();
	ResyncQuests();
	SyncPortals();
	dolighting = 1;

	for(i = 0; i < 4; i++)
	{
		if ( plr[i].plractive && currlevel == plr[i].plrlevel )
			LightList[plr[i]._plid]._lunflag = 1;
	}

	mem_free_dbg(LoadBuff);
}
// 5BB1ED: using guessed type char leveltype;
// 642A18: using guessed type int dolighting;
