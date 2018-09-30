//HEADER_GOES_HERE

#include "../types.h"

int pfile_cpp_init_value;

#ifndef NO_GLOBALS
char hero_names[MAX_CHARACTERS][PLR_NAME_LEN];
bool gbValidSaveFile; // idb
int save_prev_tc; // weak
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
	char Buffer[260]; // [esp+4h] [ebp-104h]

	if ( GetWindowsDirectory(Buffer, 0x104u)
	  && (pfile_check_available_space(Buffer), GetModuleFileName(ghInst, Buffer, 0x104u)) )
	{
		pfile_check_available_space(Buffer);
	}
	else
	{
		TermMsg("Unable to initialize save directory");
	}
}

void __fastcall pfile_check_available_space(char *pszDir)
{
	char *v1; // edi
	char *v2; // eax
	char v3; // cl
	BOOL v4; // esi
	DWORD TotalNumberOfClusters; // [esp+8h] [ebp-10h]
	DWORD NumberOfFreeClusters; // [esp+Ch] [ebp-Ch]
	DWORD BytesPerSector; // [esp+10h] [ebp-8h]
	DWORD SectorsPerCluster; // [esp+14h] [ebp-4h]

	v1 = pszDir;
	v2 = pszDir;
	while ( 1 )
	{
		v3 = *v2;
		if ( !*v2 )
			break;
		++v2;
		if ( v3 == '\\' )
		{
			*v2 = '\0';
			break;
		}
	}
	v4 = GetDiskFreeSpace(v1, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters);
	if ( !v4 )
		goto LABEL_12;
	if ( (signed __int64)(BytesPerSector * (unsigned __int64)SectorsPerCluster * NumberOfFreeClusters) < 0xA00000 )
		v4 = 0;
	if ( !v4 )
LABEL_12:
		DiskFreeDlg(v1);
}

void __cdecl pfile_write_hero()
{
	int v0; // eax
	int v1; // esi
	//int v2; // eax
	PkPlayerStruct pkplr; // [esp+4h] [ebp-4F4h]

	v0 = pfile_get_save_num_from_name(plr[myplr]._pName);
	v1 = v0;
	//_LOBYTE(v2) = pfile_open_archive(1, v0);
	if ( pfile_open_archive(1, v0) )
	{
		PackPlayer(&pkplr, myplr, gbMaxPlayers == 1);
		pfile_encode_hero(&pkplr);
		pfile_flush(gbMaxPlayers == 1, v1);
	}
}
// 679660: using guessed type char gbMaxPlayers;

int __fastcall pfile_get_save_num_from_name(char *name)
{
	char *v1; // ebx
	unsigned int v2; // esi

	v1 = name;
	v2 = 0;
	do
	{
		if ( !_strcmpi(hero_names[v2], v1) )
			break;
		++v2;
	}
	while ( v2 < MAX_CHARACTERS );
	return v2;
}

void __fastcall pfile_encode_hero(PkPlayerStruct *pPack)
{
	int v1; // ebx
	void *v2; // edi
	char password[16]; // [esp+Ch] [ebp-14h]
	void *v4; // [esp+1Ch] [ebp-4h]

	strcpy(password, "xrgyrkj1");
	*(_DWORD *)&password[9] = 0;
	*(_WORD *)&password[13] = 0;
	v4 = pPack;
	password[15] = 0;
	if ( (unsigned char)gbMaxPlayers > 1u )
		strcpy(password, "szqnlsk1");
	v1 = codec_get_encoded_len(1266);
	v2 = DiabloAllocPtr(v1);
	memcpy(v2, v4, 0x4F2u);
	codec_encode(v2, 1266, v1, password);
	mpqapi_write_file("hero", (char *)v2, v1);
	mem_free_dbg(v2);
}
// 679660: using guessed type char gbMaxPlayers;

bool __fastcall pfile_open_archive(bool a1, int save_num)
{
	int v2; // esi
	BOOL v3; // edi
	//int v4; // eax
	char FileName[260]; // [esp+8h] [ebp-104h]

	v2 = save_num;
	v3 = a1;
	pfile_get_save_path(FileName, 260, save_num);
	//_LOBYTE(v4) = mpqapi_open_archive(FileName, 0, v2);
	if ( mpqapi_open_archive(FileName, 0, v2) )
		return 1;
	if ( v3 )
	{
		if ( (unsigned char)gbMaxPlayers > 1u )
			MI_Dummy(v2);
	}
	return 0;
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall pfile_get_save_path(char *pszBuf, int dwBufSize, int save_num)
{
	char *v3; // esi
	const char *v4; // ebx
	DWORD v5; // edi
	char *v6; // eax
	char v7[260]; // [esp+8h] [ebp-104h]

	v3 = pszBuf;
	v4 = "\\multi_%d.sv";
	if ( (unsigned char)gbMaxPlayers <= 1u )
		v4 = "\\single_%d.sv";
	v5 = GetModuleFileName(ghInst, pszBuf, 0x104u);
	v6 = strrchr(v3, '\\');
	if ( v6 )
		*v6 = 0;
	if ( !v5 )
		TermMsg("Unable to get save directory");
	sprintf(v7, v4, save_num);
	strcat(v3, v7);
	_strlwr(v3);
}
// 679660: using guessed type char gbMaxPlayers;

void __fastcall pfile_flush(bool is_single_player, int save_num)
{
	int v2; // esi
	bool v3; // di
	char FileName[260]; // [esp+8h] [ebp-104h]

	v2 = save_num;
	v3 = is_single_player;
	pfile_get_save_path(FileName, 260, save_num);
	mpqapi_flush_and_close(FileName, v3, v2);
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
	int v0; // eax

	v0 = pfile_get_save_num_from_name(plr[myplr]._pName);
	pfile_flush(1, v0);
}

void __fastcall game_2_ui_player(PlayerStruct *p, _uiheroinfo *heroinfo, bool bHasSaveFile)
{
	_uiheroinfo *v3; // esi
	PlayerStruct *v4; // edi
	char v5; // al

	v3 = heroinfo;
	v4 = p;
	memset(heroinfo, 0, 0x2Cu);
	strncpy(v3->name, v4->_pName, 0xFu);
	v3->name[15] = 0;
	v3->level = v4->_pLevel;
	v3->heroclass = game_2_ui_class(v4);
	v3->strength = v4->_pStrength;
	v3->magic = v4->_pMagic;
	v3->dexterity = v4->_pDexterity;
	v3->vitality = v4->_pVitality;
	v3->gold = v4->_pGold;
	v3->hassaved = bHasSaveFile;
	v5 = v4->pDiabloKillLevel;
	v3->spawned = 0;
	v3->herorank = v5;
}

char __fastcall game_2_ui_class(PlayerStruct *p)
{
	char result; // al

	result = p->_pClass;
	if ( result )
		result = (result != 1) + 1;
	return result;
}

bool __stdcall pfile_ui_set_hero_infos(void (__stdcall *ui_add_hero_info)(_uiheroinfo *))
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

	memset(hero_names, 0, 0x140u);
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
			if ( v1 != (char *)1 && OpenFile(FileName, &ReOpenBuff, 0x4000u) != -1 )
			{
				if ( !SRegLoadString("Diablo\\Converted", (const char *)v1, 0, NewFileName, 260) )
				{
					while ( 1 )
					{
						v3 = save_num++;
						pfile_get_save_path(NewFileName, 260, v3);
						if ( OpenFile(NewFileName, &ReOpenBuff, 0x4000u) == -1 )
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

char *__fastcall GetSaveDirectory(char *dst, int dst_size, int save_num)
{
	char *v3; // esi
	const char *v4; // ebx
	DWORD v5; // edi
	char *v6; // eax
	char path_buf[260]; // [esp+Ch] [ebp-104h]

	v3 = dst;
	if ( (unsigned char)gbMaxPlayers <= 1u )
	{
		v4 = "\\single_%d.sv";
		v5 = GetModuleFileName(ghInst, dst, 0x104u);
		v6 = strrchr(v3, '\\');
		if ( v6 )
			*v6 = '\0';
	}
	else
	{
		v4 = "\\dlinfo_%d.drv";
		v5 = GetWindowsDirectory(dst, 0x104u);
	}
	if ( !v5 )
		TermMsg("Unable to get save directory");
	sprintf(path_buf, v4, save_num);
	strcat(v3, path_buf);
	return _strlwr(v3);
}
// 679660: using guessed type char gbMaxPlayers;

bool __fastcall pfile_read_hero(void *archive, PkPlayerStruct *pPack)
{
	BOOL v2; // eax
	int dwSize; // eax
	int v4; // edi
	char *v5; // eax
	char *v6; // esi
	//int v7; // eax
	//int v8; // eax
	char password[16]; // [esp+4h] [ebp-24h]
	void *v11; // [esp+14h] [ebp-14h]
	DWORD nSize; // [esp+18h] [ebp-10h]
	int v13; // [esp+1Ch] [ebp-Ch]
	int dwBytes; // [esp+20h] [ebp-8h]
	void *file; // [esp+24h] [ebp-4h]

	v11 = pPack;
	v2 = SFileOpenFileEx(archive, "hero", 0, &file);
	if ( v2 )
	{
		strcpy(password, "xrgyrkj1");
		v13 = 0;
		*(_DWORD *)&password[9] = 0;
		*(_WORD *)&password[13] = 0;
		password[15] = 0;
		nSize = 16;
		if ( (unsigned char)gbMaxPlayers > 1u )
			strcpy(password, "szqnlsk1");
		dwSize = SFileGetFileSize((int *)file, 0);
		v4 = dwSize;
		if ( !dwSize )
			goto LABEL_15;
		v5 = (char *)DiabloAllocPtr(dwSize);
		v6 = v5;
		//_LOBYTE(v7) = SFileReadFile(file, v5, v4, (unsigned long *)&dwBytes, 0);
		if ( SFileReadFile(file, v5, v4, (unsigned long *)&dwBytes, 0) )
		{
			dwBytes = codec_decode(v6, v4, password);
			if ( dwBytes )
				goto LABEL_11;
			if ( (unsigned char)gbMaxPlayers > 1u )
			{
				GetComputerName(password, &nSize);
				if ( !SFileSetFilePointer(file, 0, 0, 0) )
				{
					//_LOBYTE(v8) = SFileReadFile(file, v6, v4, (unsigned long *)&dwBytes, 0);
					if ( SFileReadFile(file, v6, v4, (unsigned long *)&dwBytes, 0) )
					{
						dwBytes = codec_decode(v6, v4, password);
LABEL_11:
						if ( dwBytes == 1266 )
						{
							memcpy(v11, v6, 0x4F2u);
							v13 = 1;
						}
						goto LABEL_13;
					}
				}
			}
		}
LABEL_13:
		if ( v6 )
			mem_free_dbg(v6);
LABEL_15:
		SFileCloseFile(file);
		v2 = v13;
	}
	return v2;
}
// 679660: using guessed type char gbMaxPlayers;

void *__fastcall pfile_open_save_archive(int *unused, int save_num)
{
	//int v2; // eax
	char SrcStr[260]; // [esp+0h] [ebp-108h]
	void *archive; // [esp+104h] [ebp-4h]

	pfile_get_save_path(SrcStr, 260, save_num);
	//_LOBYTE(v2) = SFileOpenArchive(SrcStr, 0x7000, 0, &archive);
	return SFileOpenArchive(SrcStr, 0x7000, 0, &archive) != 0 ? archive : NULL;
}

void __fastcall pfile_SFileCloseArchive(void *hsArchive)
{
	SFileCloseArchive(hsArchive);
}

bool __fastcall pfile_archive_contains_game(void *hsArchive)
{
	//int v1; // eax
	void *file; // [esp+0h] [ebp-4h]

	file = hsArchive;
	if ( gbMaxPlayers != 1 )
		return 0;
	//_LOBYTE(v1) = SFileOpenFileEx(hsArchive, "game", 0, &file);
	if ( !SFileOpenFileEx(hsArchive, "game", 0, &file) )
		return 0;
	SFileCloseFile(file);
	return 1;
}
// 679660: using guessed type char gbMaxPlayers;

bool __stdcall pfile_ui_set_class_stats(int player_class_nr, _uidefaultstats *class_stats)
{
	int v2; // eax

	v2 = (char)pfile_get_player_class(player_class_nr);
	class_stats->strength = StrengthTbl[v2];
	class_stats->magic = MagicTbl[v2];
	class_stats->dexterity = DexterityTbl[v2];
	class_stats->vitality = VitalityTbl[v2];
	return 1;
}

int __fastcall pfile_get_player_class(int player_class_nr)
{
	int result; // eax

	if ( player_class_nr )
		_LOBYTE(result) = (player_class_nr != 1) + 1;
	else
		_LOBYTE(result) = 0;
	return result;
}

bool __stdcall pfile_ui_save_create(_uiheroinfo *heroinfo)
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

bool __stdcall pfile_get_file_name(int lvl, char *dst)
{
	int v2; // ecx
	bool v3; // zf
	const char *v4; // eax

	v2 = lvl;
	if ( (unsigned char)gbMaxPlayers > 1u )
	{
		v3 = lvl == 0;
		goto LABEL_10;
	}
	if ( (unsigned int)lvl < 0x11 )
	{
		v4 = "perml%02d";
LABEL_12:
		sprintf(dst, v4, v2);
		return 1;
	}
	if ( (unsigned int)lvl < 0x22 )
	{
		v2 = lvl - 17;
		v4 = "perms%02d";
		goto LABEL_12;
	}
	if ( lvl == 34 )
	{
		v4 = "game";
		goto LABEL_12;
	}
	v3 = lvl == 35;
LABEL_10:
	if ( v3 )
	{
		v4 = "hero";
		goto LABEL_12;
	}
	return 0;
}
// 679660: using guessed type char gbMaxPlayers;

bool __stdcall pfile_delete_save(_uiheroinfo *hero_info)
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
	*(_DWORD *)&gbValidSaveFile = pfile_archive_contains_game(v1);
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

bool __stdcall GetTempSaveNames(int dwIndex, char *szTemp)
{
	int v2; // eax
	const char *v3; // ecx

	v2 = dwIndex;
	if ( (unsigned int)dwIndex < 0x11 )
	{
		v3 = "templ%02d";
LABEL_5:
		sprintf(szTemp, v3, v2);
		return 1;
	}
	if ( (unsigned int)dwIndex < 0x22 )
	{
		v2 = dwIndex - 17;
		v3 = "temps%02d";
		goto LABEL_5;
	}
	return 0;
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

bool __stdcall GetPermSaveNames(int dwIndex, char *szPerm)
{
	int v2; // eax
	const char *v3; // ecx

	v2 = dwIndex;
	if ( (unsigned int)dwIndex < 0x11 )
	{
		v3 = "perml%02d";
LABEL_5:
		sprintf(szPerm, v3, v2);
		return 1;
	}
	if ( (unsigned int)dwIndex < 0x22 )
	{
		v2 = dwIndex - 17;
		v3 = "perms%02d";
		goto LABEL_5;
	}
	return 0;
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

void __fastcall pfile_strcpy(char *dst, char *src)
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

void __fastcall pfile_update(bool force_save)
{
	BOOL v1; // esi
	DWORD v2; // eax

	v1 = force_save;
	if ( gbMaxPlayers != 1 )
	{
		v2 = GetTickCount();
		if ( v1 || (signed int)(v2 - save_prev_tc) > 60000 )
		{
			save_prev_tc = v2;
			pfile_write_hero();
		}
	}
}
// 679660: using guessed type char gbMaxPlayers;
// 686428: using guessed type int save_prev_tc;
