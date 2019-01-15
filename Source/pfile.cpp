//HEADER_GOES_HERE

#include "../types.h"

#define PASSWORD_SINGLE    "xrgyrkj1"
#define PASSWORD_MULTI     "szqnlsk1"

static int pfile_cpp_init_value;
static char hero_names[MAX_CHARACTERS][PLR_NAME_LEN];
#ifndef NO_GLOBALS
BOOL gbValidSaveFile; // idb
#endif

const int pfile_inf = 0x7F800000; // weak

struct pfile_cpp_init
{
	pfile_cpp_init()
	{
		pfile_cpp_init_value = pfile_inf;
	}
} _pfile_cpp_init;
// 47F1C0: using guessed type int pfile_inf;

void __cdecl pfile_init_save_directory()
{
	DWORD len;
	char Buffer[MAX_PATH];

	len = GetWindowsDirectory(Buffer, sizeof(Buffer));
	if ( len ) {
		pfile_check_available_space(Buffer);
		len = GetModuleFileName(ghInst, Buffer, sizeof(Buffer));
	}

	if ( !len )
		TermMsg("Unable to initialize save directory");
	else
		pfile_check_available_space(Buffer);
}

void __fastcall pfile_check_available_space(char *pszDir)
{
	char *s;
	BOOL hasSpace;
	DWORD TotalNumberOfClusters;
	DWORD NumberOfFreeClusters;
	DWORD BytesPerSector;
	DWORD SectorsPerCluster;

	s = pszDir;
	while ( *s ) {
		if ( *s++ != '\\' )
			continue;
		*s = '\0';
		break;
	}

	hasSpace = GetDiskFreeSpace(pszDir, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters);
	if ( hasSpace ) {
		// 10MB is the amount hardcoded in the error dialog
		if ( (__int64)SectorsPerCluster * BytesPerSector * NumberOfFreeClusters < (__int64)(10 << 20) )
			hasSpace = FALSE;
	}

	if ( !hasSpace )
		DiskFreeDlg(pszDir);
}

void __cdecl pfile_write_hero()
{
	unsigned int save_num;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	if (pfile_open_archive(TRUE, save_num)) {
		PackPlayer(&pkplr, myplr, gbMaxPlayers == 1);
		pfile_encode_hero(&pkplr);
		pfile_flush(gbMaxPlayers == 1, save_num);
	}
}

unsigned int __fastcall pfile_get_save_num_from_name(const char *name)
{
	unsigned int i;

	for ( i=0; i < MAX_CHARACTERS; i++ ) {
		if ( !_strcmpi(hero_names[i], name) )
			break;
	}

	return i;
}

void __fastcall pfile_encode_hero(const PkPlayerStruct *pPack)
{
	char *packed;
	DWORD packed_len;
	char password[16] = PASSWORD_SINGLE;

	if ( gbMaxPlayers > 1 )
		strcpy(password, PASSWORD_MULTI);

	packed_len = codec_get_encoded_len(sizeof(*pPack));
	packed = (char*)DiabloAllocPtr(packed_len);
	memcpy(packed, pPack, sizeof(*pPack));
	codec_encode(packed, sizeof(*pPack), packed_len, password);
	mpqapi_write_file("hero", packed, packed_len);
	mem_free_dbg(packed);
}

BOOL __fastcall pfile_open_archive(BOOL a1, unsigned int save_num)
{
#ifdef ANDROID
    char SaveDir[MAX_PATH] = "/sdcard/Diablo/"; // Full path to SAVDIR
    char FileName[MAX_PATH];


    pfile_get_save_path(FileName, sizeof(FileName), save_num);

    strcat(SaveDir,FileName+2);
    printf("FULL ITEM ? %s \n", SaveDir );
    strcpy(FileName, SaveDir);
    if ( mpqapi_open_archive(FileName, FALSE, save_num) )
        return TRUE;

    if ( a1 && gbMaxPlayers > 1 )
        mpqapi_update_multi_creation_time(save_num);
    return FALSE;





#else




	char FileName[MAX_PATH];

	pfile_get_save_path(FileName, sizeof(FileName), save_num);
	if ( mpqapi_open_archive(FileName, FALSE, save_num) )
		return TRUE;

	if ( a1 && gbMaxPlayers > 1 )
		mpqapi_update_multi_creation_time(save_num);
	return FALSE;

#endif

}

void __fastcall pfile_get_save_path(char *pszBuf, DWORD dwBufSize, unsigned int save_num)
{
	DWORD plen;
	char *s;
	char path[MAX_PATH];
	const char *fmt = "\\multi_%d.sv";

	if ( gbMaxPlayers <= 1 )
		fmt = "\\single_%d.sv";

	// BUGFIX: ignores dwBufSize and uses MAX_PATH instead
	plen = GetModuleFileName(ghInst, pszBuf, MAX_PATH);
	s = strrchr(pszBuf, '\\');
	if ( s )
		*s = '\0';

	if ( !plen )
		TermMsg("Unable to get save directory");

	sprintf(path, fmt, save_num);
	strcat(pszBuf, path);
	_strlwr(pszBuf);
}

void __fastcall pfile_flush(BOOL is_single_player, unsigned int save_num)
{
	char FileName[MAX_PATH];

	pfile_get_save_path(FileName, sizeof(FileName), save_num);
	mpqapi_flush_and_close(FileName, is_single_player, save_num);
}

bool __fastcall pfile_create_player_description(char *dst, int len)
{
	int v2; // edi
	char *v3; // ebx
	int v4; // eax
	char src[128]; // [esp+Ch] [ebp-ACh]
	_uiheroinfo hero_info; // [esp+8Ch] [ebp-2Ch]

	myplr = 0;
	v2 = len;
	v3 = dst;
	pfile_read_player_from_save();
	game_2_ui_player(plr, &hero_info, gbValidSaveFile);
	UiSetupPlayerInfo(chr_name_str, &hero_info, 'DRTL');
	if ( !v3 || !v2 )
		goto LABEL_5;
	v4 = UiCreatePlayerDescription(&hero_info, 'DRTL', src);
	if ( v4 )
	{
		SStrCopy(v3, src, v2);
LABEL_5:
		v4 = 1;
	}
	return v4;
}

int __fastcall pfile_create_save_file(char *name_1, char *name_2)
{
	char *v2; // edi
	char *v3; // ebp
	int v4; // esi
	int v5; // eax
	char *v7; // [esp+20h] [ebp-30h]
	_uiheroinfo heroinfo; // [esp+24h] [ebp-2Ch]

	v2 = name_2;
	v3 = name_1;
	if ( pfile_get_save_num_from_name(name_2) != MAX_CHARACTERS )
		return 0;
	v4 = 0;
	v7 = plr[0]._pName;
	while ( _strcmpi(v3, v7) )
	{
		v7 += 21720;
		++v4;
		if ( v7 == plr[4]._pName )
			return 0;
	}
	v5 = pfile_get_save_num_from_name(v3);
	if ( v5 == MAX_CHARACTERS )
		return 0;
	SStrCopy(hero_names[v5], v2, 32);
	SStrCopy(plr[v4]._pName, v2, 32);
	if ( !_strcmpi(chr_name_str, v3) )
		SStrCopy(chr_name_str, v2, 16);
	game_2_ui_player(plr, &heroinfo, gbValidSaveFile);
	UiSetupPlayerInfo(chr_name_str, &heroinfo, 'DRTL');
	pfile_write_hero();
	return 1;
}

void __cdecl pfile_flush_W()
{
	pfile_flush(TRUE, pfile_get_save_num_from_name(plr[myplr]._pName));
}

void __fastcall game_2_ui_player(const PlayerStruct *p, _uiheroinfo *heroinfo, BOOL bHasSaveFile)
{
	memset(heroinfo, 0, sizeof(*heroinfo));
	strncpy(heroinfo->name, p->_pName, sizeof(heroinfo->name) - 1);
	heroinfo->name[sizeof(heroinfo->name) - 1] = '\0';
	heroinfo->level = p->_pLevel;
	heroinfo->heroclass = game_2_ui_class(p);
	heroinfo->strength = p->_pStrength;
	heroinfo->magic = p->_pMagic;
	heroinfo->dexterity = p->_pDexterity;
	heroinfo->vitality = p->_pVitality;
	heroinfo->gold = p->_pGold;
	heroinfo->hassaved = bHasSaveFile;
	heroinfo->herorank = (unsigned char)p->pDiabloKillLevel;
	heroinfo->spawned = 0;
}

unsigned char __fastcall game_2_ui_class(const PlayerStruct *p)
{
	unsigned char uiclass;
	if ( p->_pClass == PC_WARRIOR )
		uiclass = UI_WARRIOR;
	else if ( p->_pClass == PC_ROGUE )
		uiclass = UI_ROGUE;
	else
		uiclass = UI_SORCERER;

	return uiclass;
}

BOOL __stdcall pfile_ui_set_hero_infos(BOOL (__stdcall *ui_add_hero_info)(_uiheroinfo *))
{
	char *v1; // esi
	//int v2; // eax
	int v3; // eax
	DWORD v4; // eax
	unsigned int v5; // esi
	void *v7; // eax
	void *v8; // edi
	//int v9; // eax
	bool v10; // al
	PkPlayerStruct pkplr; // [esp+Ch] [ebp-7BCh]
	struct _OFSTRUCT ReOpenBuff; // [esp+500h] [ebp-2C8h]
	char FileName[260]; // [esp+588h] [ebp-240h]
	char NewFileName[260]; // [esp+68Ch] [ebp-13Ch]
	_uiheroinfo hero_info; // [esp+790h] [ebp-38h]
	int unused; // [esp+7BCh] [ebp-Ch]
	LPCSTR lpSrcStr; // [esp+7C0h] [ebp-8h]
	int save_num; // [esp+7C4h] [ebp-4h]

	memset(hero_names, 0, sizeof(hero_names));
	if ( (unsigned char)gbMaxPlayers > 1u )
	{
		lpSrcStr = 0;
		save_num = 0;
		do
		{
			if ( (unsigned int)save_num >= MAX_CHARACTERS )
				break;
			GetSaveDirectory(FileName, 260, (int)lpSrcStr);
			v1 = strrchr(FileName, '\\') + 1;
			if ( v1 != (char *)1 && OpenFile(FileName, &ReOpenBuff, 0x4000u) != HFILE_ERROR )
			{
				if ( !SRegLoadString("Diablo\\Converted", (const char *)v1, 0, NewFileName, 260) )
				{
					while ( 1 )
					{
						v3 = save_num++;
						pfile_get_save_path(NewFileName, 260, v3);
						if ( OpenFile(NewFileName, &ReOpenBuff, 0x4000u) == HFILE_ERROR )
							break;
						if ( (unsigned int)save_num >= MAX_CHARACTERS )
							goto LABEL_13;
					}
					if ( CopyFile(FileName, NewFileName, 1) )
					{
						SRegSaveString("Diablo\\Converted", v1, 0, NewFileName);
						v4 = GetFileAttributes(NewFileName);
						if ( v4 != -1 )
						{
							_LOBYTE(v4) = v4 & 0xF9;
							SetFileAttributes(NewFileName, v4);
						}
					}
				}
			}
LABEL_13:
			++lpSrcStr;
		}
		while ( (unsigned int)lpSrcStr < MAX_CHARACTERS );
	}
	unused = 1;
	v5 = 0;
	do
	{
		v7 = pfile_open_save_archive(&unused, v5);
		v8 = v7;
		if ( v7 )
		{
			if ( pfile_read_hero(v7, &pkplr) )
			{
				strcpy(hero_names[v5], pkplr.pName);
				UnPackPlayer(&pkplr, 0, 0);
				v10 = pfile_archive_contains_game(v8);
				game_2_ui_player(plr, &hero_info, v10);
				ui_add_hero_info(&hero_info);
			}
			pfile_SFileCloseArchive(v8);
		}
		++v5;
	}
	while ( v5 < MAX_CHARACTERS );
	return 1;
}
// 679660: using guessed type char gbMaxPlayers;

char *__fastcall GetSaveDirectory(char *dst, int dst_size, unsigned int save_num)
{
	DWORD dirLen;
	char FileName[MAX_PATH];
	const char *savename;

	// BUGFIX: ignores dst_size and uses MAX_PATH instead
	if ( gbMaxPlayers > 1 ) {
		savename = "\\dlinfo_%d.drv";
		dirLen = GetWindowsDirectory(dst, MAX_PATH);
	} else {
		char *s;
		savename = "\\single_%d.sv";
		dirLen = GetModuleFileName(ghInst, dst, MAX_PATH);
		s = strrchr(dst, '\\');
		if ( s )
			*s = '\0';
	}

	if ( !dirLen )
		TermMsg("Unable to get save directory");

	sprintf(FileName, savename, save_num);
	strcat(dst, FileName);
	return _strlwr(dst);
}

BOOL __fastcall pfile_read_hero(void *archive, PkPlayerStruct *pPack)
{
	HANDLE file;

	if ( !SFileOpenFileEx(archive, "hero", 0, &file) )
		return FALSE;
	else {
		DWORD dwlen;
		BOOL ret = FALSE;
		char password[16] = PASSWORD_SINGLE;
		DWORD nSize = 16;

		if ( gbMaxPlayers > 1 )
			strcpy(password, PASSWORD_MULTI);

		dwlen = SFileGetFileSize(file, NULL);
		if ( dwlen ) {
			DWORD read;
			unsigned char *buf = DiabloAllocPtr(dwlen);
			if ( SFileReadFile(file, buf, dwlen, &read, NULL) ) {
				BOOL decoded = TRUE;
				read = codec_decode(buf, dwlen, password);
				if ( !read && gbMaxPlayers > 1 ) {
					GetComputerName(password, &nSize);
					if ( SFileSetFilePointer(file, 0, NULL, 0) || !SFileReadFile(file, buf, dwlen, &read, NULL) )
						decoded = FALSE;
					else
						read = codec_decode(buf, dwlen, password);
				}
				if ( decoded && read == sizeof(*pPack) ) {
					memcpy(pPack, buf, sizeof(*pPack));
					ret = TRUE;
				}
			}
			if ( buf )
				mem_free_dbg(buf);
		}
		SFileCloseFile(file);
		return ret;
	}
}

HANDLE __fastcall pfile_open_save_archive(int *unused, unsigned int save_num)
{
#ifdef ANDROID
    char SaveDir[MAX_PATH] = "/sdcard/Diablo/"; // Full path to SAVDIR
    char SrcStr[MAX_PATH];
    HANDLE archive;

    pfile_get_save_path(SrcStr, sizeof(SrcStr), save_num);

    strcat(SaveDir,SrcStr+2);
    printf("FULL ITEM ? %s \n", SaveDir );
    strcpy(SrcStr, SaveDir);
    if ( SFileOpenArchive(SrcStr, 0x7000, 0, &archive) )
        return archive;
    return NULL;

#else   // IF NOT ANDROID

    char SrcStr[MAX_PATH];
        HANDLE archive;

        pfile_get_save_path(SrcStr, sizeof(SrcStr), save_num);
        if (SFileOpenArchive(SrcStr, 0x7000, 0, &archive))
                return archive;
        return NULL;

#endif

}

void __fastcall pfile_SFileCloseArchive(HANDLE hsArchive)
{
	SFileCloseArchive(hsArchive);
}

BOOL __fastcall pfile_archive_contains_game(HANDLE hsArchive)
{
	HANDLE file;

	if ( gbMaxPlayers != 1 )
		return FALSE;

	if ( !SFileOpenFileEx(hsArchive, "game", 0, &file) )
		return FALSE;

	SFileCloseFile(file);
	return TRUE;
}

BOOL __stdcall pfile_ui_set_class_stats(int player_class_nr, _uidefaultstats *class_stats)
{
	int v2; // eax

	v2 = (char)pfile_get_player_class(player_class_nr);
	class_stats->strength = StrengthTbl[v2];
	class_stats->magic = MagicTbl[v2];
	class_stats->dexterity = DexterityTbl[v2];
	class_stats->vitality = VitalityTbl[v2];
	return 1;
}

char __fastcall pfile_get_player_class(unsigned int player_class_nr)
{
	char pc_class;

	if ( player_class_nr == UI_WARRIOR )
		pc_class = PC_WARRIOR;
	else if ( player_class_nr == UI_ROGUE )
		pc_class = PC_ROGUE;
	else
		pc_class = PC_SORCERER;
	return pc_class;
}

BOOL __stdcall pfile_ui_save_create(_uiheroinfo *heroinfo)
{
	unsigned int v1; // edi
	//int v3; // eax
	char v5; // al
	PkPlayerStruct pkplr; // [esp+8h] [ebp-4F4h]

	v1 = pfile_get_save_num_from_name(heroinfo->name);
	if ( v1 == MAX_CHARACTERS )
	{
		v1 = 0;
		do
		{
			if ( !*hero_names[v1] )
				break;
			++v1;
		}
		while ( v1 < MAX_CHARACTERS );
		if ( v1 == MAX_CHARACTERS )
			return 0;
	}
	//_LOBYTE(v3) = pfile_open_archive(0, v1);
	if ( !pfile_open_archive(0, v1) )
		return 0;
	mpqapi_remove_hash_entries(pfile_get_file_name);
	strncpy(hero_names[v1], heroinfo->name, 0x20u);
	hero_names[v1][31] = 0;
	v5 = pfile_get_player_class((unsigned char)heroinfo->heroclass);
	CreatePlayer(0, v5);
	strncpy(plr[0]._pName, heroinfo->name, 0x20u);
	plr[0]._pName[31] = 0;
	PackPlayer(&pkplr, 0, 1);
	pfile_encode_hero(&pkplr);
	game_2_ui_player(plr, heroinfo, 0);
	pfile_flush(1, v1);
	return 1;
}

BOOL __stdcall pfile_get_file_name(DWORD lvl, char *dst)
{
	const char *fmt;

	if ( gbMaxPlayers > 1 ) {
		if ( lvl )
			return FALSE;
		fmt = "hero";
	} else {
		if ( lvl < 17 )
			fmt = "perml%02d";
		else if ( lvl < 34 ) {
			lvl -= 17;
			fmt = "perms%02d";
		} else if ( lvl == 34 )
			fmt = "game";
		else if ( lvl == 35 )
			fmt = "hero";
		else
			return FALSE;
	}
	sprintf(dst, fmt, lvl);
	return TRUE;
}

BOOL __stdcall pfile_delete_save(_uiheroinfo *hero_info)
{
	unsigned int v1; // eax
	char FileName[260]; // [esp+0h] [ebp-104h]

	v1 = pfile_get_save_num_from_name(hero_info->name);
	if ( v1 < MAX_CHARACTERS )
	{
		hero_names[v1][0] = 0;
		pfile_get_save_path(FileName, 260, v1);
		DeleteFile(FileName);
	}
	return 1;
}

void __cdecl pfile_read_player_from_save()
{
	int dwChar; // edi
	void *v1; // esi
	//int v2; // eax
	PkPlayerStruct pkplr; // [esp+8h] [ebp-4F4h]

	dwChar = pfile_get_save_num_from_name(chr_name_str);
	v1 = pfile_open_save_archive(0, dwChar);
	if ( !v1 )
		TermMsg("Unable to open archive");
	//_LOBYTE(v2) = pfile_read_hero(v1, &pkplr);
	if ( !pfile_read_hero(v1, &pkplr) )
		TermMsg("Unable to load character");
	UnPackPlayer(&pkplr, myplr, 0);
	gbValidSaveFile = pfile_archive_contains_game(v1);
	pfile_SFileCloseArchive(v1);
}

void __fastcall GetTempLevelNames(char *szTemp)
{
	char *v1; // esi

	v1 = szTemp;
	pfile_get_save_num_from_name(plr[myplr]._pName);
	if ( setlevel )
		sprintf(v1, "temps%02d", (unsigned char)setlvlnum);
	else
		sprintf(v1, "templ%02d", currlevel);
}
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;

void __fastcall GetPermLevelNames(char *szPerm)
{
	char *v1; // esi
	int v2; // ebx
	//int v3; // eax
	//int v4; // eax
	int v5; // edi

	v1 = szPerm;
	v2 = pfile_get_save_num_from_name(plr[myplr]._pName);
	GetTempLevelNames(v1);
	//_LOBYTE(v3) = pfile_open_archive(0, v2);
	if ( !pfile_open_archive(0, v2) )
		TermMsg("Unable to read to save file archive");
	//_LOBYTE(v4) = mpqapi_has_file(v1);
	v5 = mpqapi_has_file(v1);
	pfile_flush(1, v2);
	if ( !v5 )
	{
		if ( setlevel )
			sprintf(v1, "perms%02d", (unsigned char)setlvlnum);
		else
			sprintf(v1, "perml%02d", currlevel);
	}
}
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;

void __fastcall pfile_get_game_name(char *dst)
{
	char *v1; // esi

	v1 = dst;
	pfile_get_save_num_from_name(plr[myplr]._pName);
	strcpy(v1, "game");
}

void __cdecl pfile_remove_temp_files()
{
	int v0; // eax
	int v1; // esi
	//int v2; // eax

	if ( (unsigned char)gbMaxPlayers <= 1u )
	{
		v0 = pfile_get_save_num_from_name(plr[myplr]._pName);
		v1 = v0;
		//_LOBYTE(v2) = pfile_open_archive(0, v0);
		if ( !pfile_open_archive(0, v0) )
			TermMsg("Unable to write to save file archive");
		mpqapi_remove_hash_entries(GetTempSaveNames);
		pfile_flush(1, v1);
	}
}
// 679660: using guessed type char gbMaxPlayers;

BOOL __stdcall GetTempSaveNames(DWORD dwIndex, char *szTemp)
{
	const char *fmt;

	if ( dwIndex < 17 )
		fmt = "templ%02d";
	else if ( dwIndex < 34 ) {
		dwIndex -= 17;
		fmt = "temps%02d";
	} else
		return FALSE;

	sprintf(szTemp, fmt, dwIndex);
	return TRUE;
}

void __cdecl pfile_rename_temp_to_perm()
{
	int v0; // eax
	int v1; // edi
	//int v2; // eax
	int v3; // esi
	//int v4; // eax
	//int v5; // eax
	//int v6; // eax
	char v7[260]; // [esp+8h] [ebp-208h]
	char v8[260]; // [esp+10Ch] [ebp-104h]

	v0 = pfile_get_save_num_from_name(plr[myplr]._pName);
	v1 = v0;
	//_LOBYTE(v2) = pfile_open_archive(0, v0);
	if ( !pfile_open_archive(0, v0) )
		TermMsg("Unable to write to save file archive");
	v3 = 0;
	while ( 1 )
	{
		//_LOBYTE(v6) = GetTempSaveNames(v3, v7);
		if ( !GetTempSaveNames(v3, v7) )
			break;
		GetPermSaveNames(v3++, v8);
		//_LOBYTE(v4) = mpqapi_has_file(v7);
		if ( mpqapi_has_file(v7) )
		{
			//_LOBYTE(v5) = mpqapi_has_file(v8);
			if ( mpqapi_has_file(v8) )
				mpqapi_remove_hash_entry(v8);
			mpqapi_rename(v7, v8);
		}
	}
	GetPermSaveNames(v3, v8);
	pfile_flush(1, v1);
}

BOOL __stdcall GetPermSaveNames(DWORD dwIndex, char *szPerm)
{
	const char *fmt;

	if ( dwIndex < 17 )
		fmt = "perml%02d";
	else if ( dwIndex < 34 ) {
		dwIndex -= 17;
		fmt = "perms%02d";
	} else
		return FALSE;

	sprintf(szPerm, fmt, dwIndex);
	return TRUE;
}

void __fastcall pfile_write_save_file(char *pszName, void *pbData, int dwLen, int qwLen)
{
	void *v4; // ebx
	int v5; // eax
	//int v6; // eax
	char file_name[260]; // [esp+Ch] [ebp-118h]
	char password[16]; // [esp+110h] [ebp-14h]
	int v9; // [esp+120h] [ebp-4h]

	v4 = pbData;
	pfile_strcpy(file_name, pszName);
	v5 = pfile_get_save_num_from_name(plr[myplr]._pName);
	strcpy(password, "xrgyrkj1");
	v9 = v5;
	*(_DWORD *)&password[9] = 0;
	*(_WORD *)&password[13] = 0;
	password[15] = 0;
	if ( (unsigned char)gbMaxPlayers > 1u )
		strcpy(password, "szqnlsk1");
	codec_encode(v4, dwLen, qwLen, password);
	//_LOBYTE(v6) = pfile_open_archive(0, v9);
	if ( !pfile_open_archive(0, v9) )
		TermMsg("Unable to write to save file archive");
	mpqapi_write_file(file_name, (char *)v4, qwLen);
	pfile_flush(1, v9);
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall pfile_strcpy(char *dst, const char *src)
{
	strcpy(dst, src);
}

char *__fastcall pfile_read(char *pszName, int *pdwLen)
{
	int *v2; // ebx
	int v3; // eax
	void *v4; // edi
	//int v5; // eax
	int v6; // eax
	void *v7; // eax
	//int v8; // eax
	char *v9; // esi
	int v10; // eax
	//int v11; // eax
	char v13[260]; // [esp+Ch] [ebp-124h]
	char password[16]; // [esp+110h] [ebp-20h]
	void *src_dst; // [esp+120h] [ebp-10h]
	int nread; // [esp+124h] [ebp-Ch]
	DWORD nSize; // [esp+128h] [ebp-8h]
	void *file; // [esp+12Ch] [ebp-4h]

	v2 = pdwLen;
	pfile_strcpy(v13, pszName);
	v3 = pfile_get_save_num_from_name(plr[myplr]._pName);
	v4 = pfile_open_save_archive(0, v3);
	if ( !v4 )
		TermMsg("Unable to open save file archive");
	//_LOBYTE(v5) = SFileOpenFileEx(v4, v13, 0, &file);
	if ( !SFileOpenFileEx(v4, v13, 0, &file) )
		TermMsg("Unable to open save file");
	v6 = SFileGetFileSize((int *)file, 0);
	*v2 = v6;
	if ( !v6 )
		TermMsg("Invalid save file");
	v7 = DiabloAllocPtr(*v2);
	src_dst = v7;
	//_LOBYTE(v8) = SFileReadFile(file, (char *)v7, *v2, (unsigned long *)&nread, 0);
	if ( !SFileReadFile(file, (char *)v7, *v2, (unsigned long *)&nread, 0) )
		TermMsg("Unable to read save file");
	SFileCloseFile(file);
	pfile_SFileCloseArchive(v4);
	strcpy(password, "xrgyrkj1");
	nSize = 16;
	*(_DWORD *)&password[9] = 0;
	*(_WORD *)&password[13] = 0;
	password[15] = 0;
	if ( (unsigned char)gbMaxPlayers > 1u )
		strcpy(password, "szqnlsk1");
	v9 = (char *)src_dst;
	v10 = codec_decode(src_dst, *v2, password);
	*v2 = v10;
	if ( !v10 )
	{
		if ( (unsigned char)gbMaxPlayers > 1u )
		{
			GetComputerName(password, &nSize);
			if ( SFileSetFilePointer(file, 0, 0, 0) )
				TermMsg("Unable to read save file");
			//_LOBYTE(v11) = SFileReadFile(file, v9, *v2, (unsigned long *)&nread, 0);
			if ( !SFileReadFile(file, v9, *v2, (unsigned long *)&nread, 0) )
				TermMsg("Unable to read save file");
			*v2 = codec_decode(v9, *v2, password);
		}
		if ( !*v2 )
			TermMsg("Invalid save file");
	}
	return v9;
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall pfile_update(BOOL force_save)
{
	// BUGFIX: these tick values should be treated as unsigned to handle overflows correctly
	static int save_prev_tc;

	if (gbMaxPlayers != 1) {
		int tick = GetTickCount();
		if(force_save || tick - save_prev_tc > 60000) {
			save_prev_tc = tick;
			pfile_write_hero();
		}
	}
}
