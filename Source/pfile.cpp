#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"

DEVILUTION_BEGIN_NAMESPACE

#define PASSWORD_SINGLE "xrgyrkj1"
#define PASSWORD_MULTI "szqnlsk1"

static char hero_names[MAX_CHARACTERS][PLR_NAME_LEN];
BOOL gbValidSaveFile;

void pfile_init_save_directory()
{
	DWORD len;
	char Buffer[MAX_PATH];

	len = GetWindowsDirectory(Buffer, sizeof(Buffer));
	if (len) {
		pfile_check_available_space(Buffer);
		len = GetModuleFileName(ghInst, Buffer, sizeof(Buffer));
	}

	if (!len)
		app_fatal("Unable to initialize save directory");
	else
		pfile_check_available_space(Buffer);
}

void pfile_check_available_space(char *pszDir)
{
	char *s;
	BOOL hasSpace;
	DWORD TotalNumberOfClusters;
	DWORD NumberOfFreeClusters;
	DWORD BytesPerSector;
	DWORD SectorsPerCluster;

	s = pszDir;
	while (*s) {
		if (*s++ != '\\')
			continue;
		*s = '\0';
		break;
	}

	hasSpace = GetDiskFreeSpace(pszDir, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters);
	if (hasSpace) {
		// 10MB is the amount hardcoded in the error dialog
		if ((__int64)SectorsPerCluster * BytesPerSector * NumberOfFreeClusters < (__int64)(10 << 20))
			hasSpace = FALSE;
	}

	if (!hasSpace)
		DiskFreeDlg(pszDir);
}

void pfile_write_hero()
{
	DWORD save_num;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	if (pfile_open_archive(TRUE, save_num)) {
		PackPlayer(&pkplr, myplr, gbMaxPlayers == 1);
		pfile_encode_hero(&pkplr);
		pfile_flush(gbMaxPlayers == 1, save_num);
	}
}

DWORD pfile_get_save_num_from_name(const char *name)
{
	DWORD i;

	for (i = 0; i < MAX_CHARACTERS; i++) {
		if (!_strcmpi(hero_names[i], name))
			break;
	}

	return i;
}

void pfile_encode_hero(const PkPlayerStruct *pPack)
{
	BYTE *packed;
	DWORD packed_len;
	char password[16] = PASSWORD_SINGLE;

	if (gbMaxPlayers > 1)
		strcpy(password, PASSWORD_MULTI);

	packed_len = codec_get_encoded_len(sizeof(*pPack));
	packed = (BYTE *)DiabloAllocPtr(packed_len);
	memcpy(packed, pPack, sizeof(*pPack));
	codec_encode(packed, sizeof(*pPack), packed_len, password);
	mpqapi_write_file("hero", packed, packed_len);
	mem_free_dbg(packed);
}

BOOL pfile_open_archive(BOOL a1, DWORD save_num)
{
	char FileName[MAX_PATH];

	pfile_get_save_path(FileName, sizeof(FileName), save_num);
	if (OpenMPQ(FileName, FALSE, save_num))
		return TRUE;

	if (a1 && gbMaxPlayers > 1)
		mpqapi_store_default_time(save_num);
	return FALSE;
}

void pfile_get_save_path(char *pszBuf, DWORD dwBufSize, DWORD save_num)
{
	DWORD plen;
	char *s;
	char path[MAX_PATH];
	const char *fmt = "\\multi_%d.sv";

	if (gbMaxPlayers <= 1)
		fmt = "\\single_%d.sv";

	// BUGFIX: ignores dwBufSize and uses MAX_PATH instead
	plen = GetModuleFileName(ghInst, pszBuf, MAX_PATH);
	s = strrchr(pszBuf, '\\');
	if (s)
		*s = '\0';

	if (!plen)
		app_fatal("Unable to get save directory");

	sprintf(path, fmt, save_num);
	strcat(pszBuf, path);
}

void pfile_flush(BOOL is_single_player, DWORD save_num)
{
	char FileName[MAX_PATH];

	pfile_get_save_path(FileName, sizeof(FileName), save_num);
	mpqapi_flush_and_close(FileName, is_single_player, save_num);
}

BOOL pfile_create_player_description(char *dst, DWORD len)
{
	char desc[128];
	_uiheroinfo uihero;

	myplr = 0;
	pfile_read_player_from_save();
	game_2_ui_player(plr, &uihero, gbValidSaveFile);
	UiSetupPlayerInfo(gszHero, &uihero, GAME_ID);

	if (dst != NULL && len) {
		if (UiCreatePlayerDescription(&uihero, GAME_ID, desc) == 0)
			return FALSE;
		SStrCopy(dst, desc, len);
	}
	return TRUE;
}

BOOL pfile_create_save_file(const char *name_1, const char *name_2)
{
	int i;
	DWORD save_num;
	_uiheroinfo uihero;
	BOOL found = FALSE;

	if (pfile_get_save_num_from_name(name_2) == MAX_CHARACTERS) {
		for (i = 0; i != MAX_PLRS; i++) {
			if (!_strcmpi(name_1, plr[i]._pName)) {
				found = TRUE;
				break;
			}
		}
	}

	if (!found)
		return FALSE;
	save_num = pfile_get_save_num_from_name(name_1);
	if (save_num == MAX_CHARACTERS)
		return FALSE;

	SStrCopy(hero_names[save_num], name_2, PLR_NAME_LEN);
	SStrCopy(plr[i]._pName, name_2, PLR_NAME_LEN);
	if (!_strcmpi(gszHero, name_1))
		SStrCopy(gszHero, name_2, sizeof(gszHero));
	game_2_ui_player(plr, &uihero, gbValidSaveFile);
	UiSetupPlayerInfo(gszHero, &uihero, GAME_ID);
	pfile_write_hero();
	return TRUE;
}

void pfile_flush_W()
{
	pfile_flush(TRUE, pfile_get_save_num_from_name(plr[myplr]._pName));
}

void game_2_ui_player(const PlayerStruct *p, _uiheroinfo *heroinfo, BOOL bHasSaveFile)
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

unsigned char game_2_ui_class(const PlayerStruct *p)
{
	unsigned char uiclass;
	if (p->_pClass == PC_WARRIOR)
		uiclass = UI_WARRIOR;
	else if (p->_pClass == PC_ROGUE)
		uiclass = UI_ROGUE;
	else
		uiclass = UI_SORCERER;

	return uiclass;
}

BOOL __stdcall pfile_ui_set_hero_infos(BOOL(__stdcall *ui_add_hero_info)(_uiheroinfo *))
{
	DWORD i, save_num;
	char FileName[MAX_PATH];
	char NewFileName[MAX_PATH];
	BOOL showFixedMsg;

	memset(hero_names, 0, sizeof(hero_names));
	if (gbMaxPlayers > 1) {
		for (i = 0, save_num = 0; i < MAX_CHARACTERS && save_num < MAX_CHARACTERS; i++) {
			struct _OFSTRUCT ReOpenBuff;
			const char *s;

			GetSaveDirectory(FileName, sizeof(FileName), i);
			s = strrchr(FileName, '\\') + 1;
			if (s == (const char *)1)
				continue;
			if (OpenFile(FileName, &ReOpenBuff, OF_EXIST) == HFILE_ERROR)
				continue;
			if (!SRegLoadString("Diablo\\Converted", s, 0, NewFileName, sizeof(NewFileName))) {
				while (save_num < MAX_CHARACTERS) {
					pfile_get_save_path(NewFileName, sizeof(NewFileName), save_num++);
					if (OpenFile(NewFileName, &ReOpenBuff, OF_EXIST) == HFILE_ERROR) {
						if (CopyFile(FileName, NewFileName, TRUE)) {
							DWORD attrib;
							SRegSaveString("Diablo\\Converted", s, 0, NewFileName);
							attrib = GetFileAttributes(NewFileName);
							if (attrib != INVALID_FILE_ATTRIBUTES) {
								attrib &= ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
								SetFileAttributes(NewFileName, attrib);
							}
						}
						break;
					}
				}
			}
		}
	}

	showFixedMsg = TRUE;
	for (i = 0; i < MAX_CHARACTERS; i++) {
		PkPlayerStruct pkplr;
		HANDLE archive = pfile_open_save_archive(&showFixedMsg, i);
		if (archive) {
			if (pfile_read_hero(archive, &pkplr)) {
				_uiheroinfo uihero;
				strcpy(hero_names[i], pkplr.pName);
				UnPackPlayer(&pkplr, 0, FALSE);
				game_2_ui_player(plr, &uihero, pfile_archive_contains_game(archive, i));
				ui_add_hero_info(&uihero);
			}
			pfile_SFileCloseArchive(archive);
		}
	}

	return TRUE;
}

char *GetSaveDirectory(char *dst, int dst_size, DWORD save_num)
{
	DWORD dirLen;
	char FileName[MAX_PATH];
	const char *savename;

	// BUGFIX: ignores dst_size and uses MAX_PATH instead
	if (gbMaxPlayers > 1) {
		savename = "\\dlinfo_%d.drv";
		dirLen = GetWindowsDirectory(dst, MAX_PATH);
	} else {
		char *s;
		savename = "\\single_%d.sv";
		dirLen = GetModuleFileName(ghInst, dst, MAX_PATH);
		s = strrchr(dst, '\\');
		if (s)
			*s = '\0';
	}

	if (!dirLen)
		app_fatal("Unable to get save directory");

	sprintf(FileName, savename, save_num);
	strcat(dst, FileName);
	return dst;
}

BOOL pfile_read_hero(HANDLE archive, PkPlayerStruct *pPack)
{
	HANDLE file;
	BOOL decoded;
	DWORD dwlen, nSize;
	BYTE *buf;

	if (!SFileOpenFileEx(archive, "hero", 0, &file)) {
		return FALSE;
	} else {
		BOOL ret = FALSE;
		char password[16] = PASSWORD_SINGLE;
		nSize = 16;

		if (gbMaxPlayers > 1)
			strcpy(password, PASSWORD_MULTI);

		dwlen = SFileGetFileSize(file, NULL);
		if (dwlen) {
			DWORD read;
			buf = DiabloAllocPtr(dwlen);
			if (SFileReadFile(file, buf, dwlen, &read, NULL)) {
				decoded = TRUE;
				read = codec_decode(buf, dwlen, password);
				if (!read && gbMaxPlayers > 1) {
					GetComputerName(password, &nSize);
					if (SFileSetFilePointer(file, 0, NULL, FILE_BEGIN) || !SFileReadFile(file, buf, dwlen, &read, NULL))
						decoded = FALSE;
					else
						read = codec_decode(buf, dwlen, password);
				}
				if (decoded && read == sizeof(*pPack)) {
					memcpy(pPack, buf, sizeof(*pPack));
					ret = TRUE;
				}
			}
			if (buf)
				mem_free_dbg(buf);
		}
		SFileCloseFile(file);
		return ret;
	}
}

/**
 * @param showFixedMsg Display a dialog if a save file was corrected (deprecated)
 */
HANDLE pfile_open_save_archive(BOOL *showFixedMsg, DWORD save_num)
{
	char SrcStr[MAX_PATH];
	HANDLE archive;

	pfile_get_save_path(SrcStr, sizeof(SrcStr), save_num);
	if (SFileOpenArchive(SrcStr, 0x7000, 0, &archive))
		return archive;
	return NULL;
}

void pfile_SFileCloseArchive(HANDLE hsArchive)
{
	SFileCloseArchive(hsArchive);
}

BOOL pfile_archive_contains_game(HANDLE hsArchive, DWORD save_num)
{
	HANDLE file;

	if (gbMaxPlayers != 1)
		return FALSE;

	if (!SFileOpenFileEx(hsArchive, "game", 0, &file))
		return FALSE;

	SFileCloseFile(file);
	return TRUE;
}

BOOL __stdcall pfile_ui_set_class_stats(unsigned int player_class_nr, _uidefaultstats *class_stats)
{
	int c;

	c = pfile_get_player_class(player_class_nr);
	class_stats->strength = StrengthTbl[c];
	class_stats->magic = MagicTbl[c];
	class_stats->dexterity = DexterityTbl[c];
	class_stats->vitality = VitalityTbl[c];
	return TRUE;
}

char pfile_get_player_class(unsigned int player_class_nr)
{
	char pc_class;

	if (player_class_nr == UI_WARRIOR)
		pc_class = PC_WARRIOR;
	else if (player_class_nr == UI_ROGUE)
		pc_class = PC_ROGUE;
	else
		pc_class = PC_SORCERER;
	return pc_class;
}

BOOL __stdcall pfile_ui_save_create(_uiheroinfo *heroinfo)
{
	DWORD save_num;
	char cl;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(heroinfo->name);
	if (save_num == MAX_CHARACTERS) {
		for (save_num = 0; save_num < MAX_CHARACTERS; save_num++) {
			if (!hero_names[save_num][0])
				break;
		}
		if (save_num == MAX_CHARACTERS)
			return FALSE;
	}
	if (!pfile_open_archive(FALSE, save_num))
		return FALSE;
	mpqapi_remove_hash_entries(pfile_get_file_name);
	strncpy(hero_names[save_num], heroinfo->name, PLR_NAME_LEN);
	hero_names[save_num][PLR_NAME_LEN - 1] = '\0';
	cl = pfile_get_player_class(heroinfo->heroclass);
	CreatePlayer(0, cl);
	strncpy(plr[0]._pName, heroinfo->name, PLR_NAME_LEN);
	plr[0]._pName[PLR_NAME_LEN - 1] = '\0';
	PackPlayer(&pkplr, 0, TRUE);
	pfile_encode_hero(&pkplr);
	game_2_ui_player(&plr[0], heroinfo, FALSE);
	pfile_flush(TRUE, save_num);
	return TRUE;
}

BOOL __stdcall pfile_get_file_name(DWORD lvl, char *dst)
{
	const char *fmt;

	if (gbMaxPlayers > 1) {
		if (lvl)
			return FALSE;
		fmt = "hero";
	} else {
		if (lvl < 17)
			fmt = "perml%02d";
		else if (lvl < 34) {
			lvl -= 17;
			fmt = "perms%02d";
		} else if (lvl == 34)
			fmt = "game";
		else if (lvl == 35)
			fmt = "hero";
		else
			return FALSE;
	}
	sprintf(dst, fmt, lvl);
	return TRUE;
}

BOOL __stdcall pfile_delete_save(_uiheroinfo *hero_info)
{
	DWORD save_num;
	char FileName[MAX_PATH];

	save_num = pfile_get_save_num_from_name(hero_info->name);
	if (save_num < MAX_CHARACTERS) {
		hero_names[save_num][0] = '\0';
		pfile_get_save_path(FileName, sizeof(FileName), save_num);
		DeleteFile(FileName);
	}
	return TRUE;
}

void pfile_read_player_from_save()
{
	HANDLE archive;
	DWORD save_num;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(gszHero);
	archive = pfile_open_save_archive(NULL, save_num);
	if (archive == NULL)
		app_fatal("Unable to open archive");
	if (!pfile_read_hero(archive, &pkplr))
		app_fatal("Unable to load character");

	UnPackPlayer(&pkplr, myplr, FALSE);
	gbValidSaveFile = pfile_archive_contains_game(archive, save_num);
	pfile_SFileCloseArchive(archive);
}

void GetTempLevelNames(char *szTemp)
{
	// BUGFIX: function call has no purpose
	pfile_get_save_num_from_name(plr[myplr]._pName);
	if (setlevel)
		sprintf(szTemp, "temps%02d", setlvlnum);
	else
		sprintf(szTemp, "templ%02d", currlevel);
}

void GetPermLevelNames(char *szPerm)
{
	DWORD save_num;
	BOOL has_file;

	save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	GetTempLevelNames(szPerm);
	if (!pfile_open_archive(FALSE, save_num))
		app_fatal("Unable to read to save file archive");

	has_file = mpqapi_has_file(szPerm);
	pfile_flush(TRUE, save_num);
	if (!has_file) {
		if (setlevel)
			sprintf(szPerm, "perms%02d", setlvlnum);
		else
			sprintf(szPerm, "perml%02d", currlevel);
	}
}

void pfile_get_game_name(char *dst)
{
	// BUGFIX: function call with no purpose
	pfile_get_save_num_from_name(plr[myplr]._pName);
	strcpy(dst, "game");
}

void pfile_remove_temp_files()
{
	if (gbMaxPlayers <= 1) {
		DWORD save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
		if (!pfile_open_archive(FALSE, save_num))
			app_fatal("Unable to write to save file archive");
		mpqapi_remove_hash_entries(GetTempSaveNames);
		pfile_flush(TRUE, save_num);
	}
}

BOOL __stdcall GetTempSaveNames(DWORD dwIndex, char *szTemp)
{
	const char *fmt;

	if (dwIndex < 17)
		fmt = "templ%02d";
	else if (dwIndex < 34) {
		dwIndex -= 17;
		fmt = "temps%02d";
	} else
		return FALSE;

	sprintf(szTemp, fmt, dwIndex);
	return TRUE;
}

void pfile_rename_temp_to_perm()
{
	DWORD save_num, i;
	char TempName[MAX_PATH];
	char PermName[MAX_PATH];

	save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	if (!pfile_open_archive(FALSE, save_num))
		app_fatal("Unable to write to save file archive");

	i = 0;
	while (GetTempSaveNames(i, TempName)) {
		GetPermSaveNames(i, PermName);
		i++;
		if (mpqapi_has_file(TempName)) {
			if (mpqapi_has_file(PermName))
				mpqapi_remove_hash_entry(PermName);
			mpqapi_rename(TempName, PermName);
		}
	}
	GetPermSaveNames(i, PermName);
	pfile_flush(TRUE, save_num);
}

BOOL __stdcall GetPermSaveNames(DWORD dwIndex, char *szPerm)
{
	const char *fmt;

	if (dwIndex < 17)
		fmt = "perml%02d";
	else if (dwIndex < 34) {
		dwIndex -= 17;
		fmt = "perms%02d";
	} else
		return FALSE;

	sprintf(szPerm, fmt, dwIndex);
	return TRUE;
}

void pfile_write_save_file(const char *pszName, BYTE *pbData, DWORD dwLen, DWORD qwLen)
{
	DWORD save_num;
	char FileName[MAX_PATH];

	pfile_strcpy(FileName, pszName);
	save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	{
		char password[16] = PASSWORD_SINGLE;
		if (gbMaxPlayers > 1)
			strcpy(password, PASSWORD_MULTI);

		codec_encode(pbData, dwLen, qwLen, password);
	}
	if (!pfile_open_archive(FALSE, save_num))
		app_fatal("Unable to write so save file archive");
	mpqapi_write_file(FileName, pbData, qwLen);
	pfile_flush(TRUE, save_num);
}

void pfile_strcpy(char *dst, const char *src)
{
	strcpy(dst, src);
}

BYTE *pfile_read(const char *pszName, DWORD *pdwLen)
{
	DWORD save_num, nread;
	char FileName[MAX_PATH];
	HANDLE archive, save;
	BYTE *buf;

	pfile_strcpy(FileName, pszName);
	save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	archive = pfile_open_save_archive(NULL, save_num);
	if (archive == NULL)
		app_fatal("Unable to open save file archive");

	if (!SFileOpenFileEx(archive, FileName, 0, &save))
		app_fatal("Unable to open save file");

	*pdwLen = SFileGetFileSize(save, NULL);
	if (*pdwLen == 0)
		app_fatal("Invalid save file");

	buf = (BYTE *)DiabloAllocPtr(*pdwLen);
	if (!SFileReadFile(save, buf, *pdwLen, &nread, NULL))
		app_fatal("Unable to read save file");
	SFileCloseFile(save);
	pfile_SFileCloseArchive(archive);

	{
		char password[16] = PASSWORD_SINGLE;
		DWORD nSize = 16;

		if (gbMaxPlayers > 1)
			strcpy(password, PASSWORD_MULTI);

		*pdwLen = codec_decode(buf, *pdwLen, password);
		if (*pdwLen == 0) {
			// BUGFIFX: *pdwLen has already been overwritten with zero and the savefile has been closed
			// there is no way this can work correctly
			if (gbMaxPlayers > 1) {
				GetComputerName(password, &nSize);
				if (SFileSetFilePointer(save, 0, NULL, FILE_BEGIN))
					app_fatal("Unable to read save file");

				if (!SFileReadFile(save, buf, *pdwLen, &nread, NULL))
					app_fatal("Unable to read save file");
				*pdwLen = codec_decode(buf, *pdwLen, password);
			}
			if (*pdwLen == 0)
				app_fatal("Invalid save file");
		}
	}
	return buf;
}

void pfile_update(BOOL force_save)
{
	// BUGFIX: these tick values should be treated as unsigned to handle overflows correctly
	static int save_prev_tc;

	if (gbMaxPlayers != 1) {
		int tick = GetTickCount();
		if (force_save || tick - save_prev_tc > 60000) {
			save_prev_tc = tick;
			pfile_write_hero();
		}
	}
}

DEVILUTION_END_NAMESPACE
