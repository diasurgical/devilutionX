/**
 * @file pfile.cpp
 *
 * Implementation of the save game encoding functionality.
 */
#include <string>

#include "all.h"
#include "paths.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"
#include "file_util.h"

DEVILUTION_BEGIN_NAMESPACE

#define PASSWORD_SPAWN_SINGLE "adslhfb1"
#define PASSWORD_SPAWN_MULTI "lshbkfg1"
#define PASSWORD_SINGLE "xrgyrkj1"
#define PASSWORD_MULTI "szqnlsk1"

namespace {

std::string GetSavePath(DWORD save_num)
{
	std::string path = GetPrefPath();
	const char *ext = ".sv";
	if (gbIsHellfire)
		ext = ".hsv";

	if (gbIsSpawn) {
		if (!gbIsMultiplayer) {
			path.append("spawn");
		} else {
			path.append("share_");
		}
	} else {
		if (!gbIsMultiplayer) {
			path.append("single_");
		} else {
			path.append("multi_");
		}
	}

	char save_num_str[21];
	snprintf(save_num_str, sizeof(save_num_str) / sizeof(char), "%d", save_num);
	path.append(save_num_str);
	path.append(ext);
	return path;
}

} // namespace

/** List of character names for the character selection screen. */
static char hero_names[MAX_CHARACTERS][PLR_NAME_LEN];
BOOL gbValidSaveFile;

static DWORD pfile_get_save_num_from_name(const char *name)
{
	DWORD i;

	for (i = 0; i < MAX_CHARACTERS; i++) {
		if (!strcasecmp(hero_names[i], name))
			break;
	}

	return i;
}

static BYTE *pfile_read_archive(HANDLE archive, const char *pszName, DWORD *pdwLen)
{
	DWORD nread;
	HANDLE file;
	BYTE *buf;

	if (!SFileOpenFileEx(archive, pszName, 0, &file))
		return NULL;

	*pdwLen = SFileGetFileSize(file, NULL);
	if (*pdwLen == 0)
		return NULL;

	buf = DiabloAllocPtr(*pdwLen);
	if (!SFileReadFile(file, buf, *pdwLen, &nread, NULL))
		return NULL;
	SFileCloseFile(file);

	{
		const char *password;
		DWORD nSize = 16;

		if (gbIsSpawn) {
			password = PASSWORD_SPAWN_SINGLE;
			if (gbIsMultiplayer)
				password = PASSWORD_SPAWN_MULTI;
		} else {
			password = PASSWORD_SINGLE;
			if (gbIsMultiplayer)
				password = PASSWORD_MULTI;
		}

		*pdwLen = codec_decode(buf, *pdwLen, password);
		if (*pdwLen == 0)
			return NULL;
	}
	return buf;
}

static BOOL pfile_read_hero(HANDLE archive, PkPlayerStruct *pPack)
{
	DWORD read;
	BYTE *buf;

	buf = pfile_read_archive(archive, "hero", &read);
	if (buf == NULL)
		return FALSE;

	BOOL ret = FALSE;
	if (read == sizeof(*pPack)) {
		memcpy(pPack, buf, sizeof(*pPack));
		ret = TRUE;
	}

	mem_free_dbg(buf);
	return ret;
}

static void pfile_encode_hero(const PkPlayerStruct *pPack)
{
	BYTE *packed;
	DWORD packed_len;
	const char *password;

	if (gbIsSpawn) {
		password = PASSWORD_SPAWN_SINGLE;
		if (gbIsMultiplayer)
			password = PASSWORD_SPAWN_MULTI;
	} else {
		password = PASSWORD_SINGLE;
		if (gbIsMultiplayer)
			password = PASSWORD_MULTI;
	}

	packed_len = codec_get_encoded_len(sizeof(*pPack));
	packed = (BYTE *)DiabloAllocPtr(packed_len);
	memcpy(packed, pPack, sizeof(*pPack));
	codec_encode(packed, sizeof(*pPack), packed_len, password);
	mpqapi_write_file("hero", packed, packed_len);
	mem_free_dbg(packed);
}

static BOOL pfile_open_archive(DWORD save_num)
{
	if (OpenMPQ(GetSavePath(save_num).c_str(), save_num))
		return TRUE;

	return FALSE;
}

static void pfile_flush(BOOL is_single_player, DWORD save_num)
{
	mpqapi_flush_and_close(GetSavePath(save_num).c_str(), is_single_player, save_num);
}

static HANDLE pfile_open_save_archive(DWORD save_num)
{
	HANDLE archive;

	if (SFileOpenArchive(GetSavePath(save_num).c_str(), 0x7000, FS_PC, &archive))
		return archive;
	return NULL;
}

static void pfile_SFileCloseArchive(HANDLE hsArchive)
{
	SFileCloseArchive(hsArchive);
}

void pfile_write_hero()
{
	DWORD save_num;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	if (pfile_open_archive(save_num)) {
		PackPlayer(&pkplr, myplr, !gbIsMultiplayer);
		pfile_encode_hero(&pkplr);
		pfile_flush(!gbIsMultiplayer, save_num);
	}
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
		if (UiCreatePlayerDescription(&uihero, GAME_ID, &desc) == 0)
			return FALSE;
		SStrCopy(dst, desc, len);
	}
	return TRUE;
}

BOOL pfile_rename_hero(const char *name_1, const char *name_2)
{
	int i;
	DWORD save_num;
	_uiheroinfo uihero;
	BOOL found = FALSE;

	if (pfile_get_save_num_from_name(name_2) == MAX_CHARACTERS) {
		for (i = 0; i != MAX_PLRS; i++) {
			if (!strcasecmp(name_1, plr[i]._pName)) {
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
	if (!strcasecmp(gszHero, name_1))
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
	heroinfo->heroclass = p->_pClass;
	heroinfo->strength = p->_pStrength;
	heroinfo->magic = p->_pMagic;
	heroinfo->dexterity = p->_pDexterity;
	heroinfo->vitality = p->_pVitality;
	heroinfo->gold = p->_pGold;
	heroinfo->hassaved = bHasSaveFile;
	heroinfo->herorank = p->pDiabloKillLevel;
	heroinfo->spawned = gbIsSpawn;
}

BOOL pfile_ui_set_hero_infos(BOOL (*ui_add_hero_info)(_uiheroinfo *))
{
	DWORD i;

	memset(hero_names, 0, sizeof(hero_names));

	for (i = 0; i < MAX_CHARACTERS; i++) {
		PkPlayerStruct pkplr;
		HANDLE archive = pfile_open_save_archive(i);
		if (archive) {
			if (pfile_read_hero(archive, &pkplr)) {
				_uiheroinfo uihero;
				strcpy(hero_names[i], pkplr.pName);
				bool hasSaveGame = pfile_archive_contains_game(archive, i);
				if (!hasSaveGame)
					gbIsHellfireSaveGame = pkplr.bIsHellfire;
				UnPackPlayer(&pkplr, 0, FALSE);
				game_2_ui_player(plr, &uihero, hasSaveGame);
				ui_add_hero_info(&uihero);
			}
			pfile_SFileCloseArchive(archive);
		}
	}

	return TRUE;
}

BOOL pfile_archive_contains_game(HANDLE hsArchive, DWORD save_num)
{
	if (gbIsMultiplayer)
		return FALSE;

	DWORD dwLen;
	BYTE *gameData = pfile_read_archive(hsArchive, "game", &dwLen);
	if (gameData == NULL)
		return FALSE;

	int hdr = (gameData[0] << 24) | (gameData[1] << 16) | (gameData[2] << 8) | gameData[3];
	mem_free_dbg(gameData);

	return IsHeaderValid(hdr);
}

void pfile_ui_set_class_stats(unsigned int player_class_nr, _uidefaultstats *class_stats)
{
	class_stats->strength = StrengthTbl[player_class_nr];
	class_stats->magic = MagicTbl[player_class_nr];
	class_stats->dexterity = DexterityTbl[player_class_nr];
	class_stats->vitality = VitalityTbl[player_class_nr];
}

BOOL pfile_ui_save_create(_uiheroinfo *heroinfo)
{
	DWORD save_num;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(heroinfo->name);
	if (save_num >= MAX_CHARACTERS) {
		for (save_num = 0; save_num < MAX_CHARACTERS; save_num++) {
			if (!hero_names[save_num][0])
				break;
		}
		if (save_num >= MAX_CHARACTERS)
			return FALSE;
	}
	if (!pfile_open_archive(save_num))
		return FALSE;
	mpqapi_remove_hash_entries(pfile_get_file_name);
	strncpy(hero_names[save_num], heroinfo->name, PLR_NAME_LEN);
	hero_names[save_num][PLR_NAME_LEN - 1] = '\0';
	CreatePlayer(0, heroinfo->heroclass);
	strncpy(plr[0]._pName, heroinfo->name, PLR_NAME_LEN);
	plr[0]._pName[PLR_NAME_LEN - 1] = '\0';
	PackPlayer(&pkplr, 0, TRUE);
	pfile_encode_hero(&pkplr);
	game_2_ui_player(&plr[0], heroinfo, FALSE);
	pfile_flush(TRUE, save_num);
	return TRUE;
}

BOOL pfile_get_file_name(DWORD lvl, char *dst)
{
	const char *fmt;

	if (gbIsMultiplayer) {
		if (lvl)
			return FALSE;
		fmt = "hero";
	} else {
		if (lvl < giNumberOfLevels)
			fmt = "perml%02d";
		else if (lvl < giNumberOfLevels * 2) {
			lvl -= giNumberOfLevels;
			fmt = "perms%02d";
		} else if (lvl == giNumberOfLevels * 2)
			fmt = "game";
		else if (lvl == giNumberOfLevels * 2 + 1)
			fmt = "hero";
		else
			return FALSE;
	}
	sprintf(dst, fmt, lvl);
	return TRUE;
}

BOOL pfile_delete_save(_uiheroinfo *hero_info)
{
	DWORD save_num;

	save_num = pfile_get_save_num_from_name(hero_info->name);
	if (save_num < MAX_CHARACTERS) {
		hero_names[save_num][0] = '\0';
		RemoveFile(GetSavePath(save_num).c_str());
	}
	return TRUE;
}

void pfile_read_player_from_save()
{
	HANDLE archive;
	DWORD save_num;
	PkPlayerStruct pkplr;

	save_num = pfile_get_save_num_from_name(gszHero);
	archive = pfile_open_save_archive(save_num);
	if (archive == NULL)
		app_fatal("Unable to open archive");
	if (!pfile_read_hero(archive, &pkplr))
		app_fatal("Unable to load character");

	gbValidSaveFile = pfile_archive_contains_game(archive, save_num);
	if (!gbValidSaveFile)
		gbIsHellfireSaveGame = pkplr.bIsHellfire;
	UnPackPlayer(&pkplr, myplr, FALSE);
	pfile_SFileCloseArchive(archive);
}

bool LevelFileExists()
{
	char szName[MAX_PATH];

	GetPermLevelNames(szName);

	DWORD save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	if (!pfile_open_archive(save_num))
		app_fatal("Unable to read to save file archive");

	bool has_file = mpqapi_has_file(szName);
	pfile_flush(TRUE, save_num);
	return has_file;
}

void GetTempLevelNames(char *szTemp)
{
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
	if (!pfile_open_archive(save_num))
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

static BOOL GetPermSaveNames(DWORD dwIndex, char *szPerm)
{
	const char *fmt;

	if (dwIndex < giNumberOfLevels)
		fmt = "perml%02d";
	else if (dwIndex < giNumberOfLevels * 2) {
		dwIndex -= giNumberOfLevels;
		fmt = "perms%02d";
	} else
		return FALSE;

	sprintf(szPerm, fmt, dwIndex);
	return TRUE;
}

static BOOL GetTempSaveNames(DWORD dwIndex, char *szTemp)
{
	const char *fmt;

	if (dwIndex < giNumberOfLevels)
		fmt = "templ%02d";
	else if (dwIndex < giNumberOfLevels * 2) {
		dwIndex -= giNumberOfLevels;
		fmt = "temps%02d";
	} else
		return FALSE;

	sprintf(szTemp, fmt, dwIndex);
	return TRUE;
}

void pfile_remove_temp_files()
{
	if (gbIsMultiplayer)
		return;

	DWORD save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	if (!pfile_open_archive(save_num))
		app_fatal("Unable to write to save file archive");
	mpqapi_remove_hash_entries(GetTempSaveNames);
	pfile_flush(TRUE, save_num);
}

void pfile_rename_temp_to_perm()
{
	DWORD dwChar, dwIndex;
	BOOL bResult;
	char szTemp[MAX_PATH];
	char szPerm[MAX_PATH];

	dwChar = pfile_get_save_num_from_name(plr[myplr]._pName);
	assert(dwChar < MAX_CHARACTERS);
	assert(!gbIsMultiplayer);
	if (!pfile_open_archive(dwChar))
		app_fatal("Unable to write to save file archive");

	dwIndex = 0;
	while (GetTempSaveNames(dwIndex, szTemp)) {
		bResult = GetPermSaveNames(dwIndex, szPerm);
		assert(bResult);
		dwIndex++;
		if (mpqapi_has_file(szTemp)) {
			if (mpqapi_has_file(szPerm))
				mpqapi_remove_hash_entry(szPerm);
			mpqapi_rename(szTemp, szPerm);
		}
	}
	assert(!GetPermSaveNames(dwIndex, szPerm));
	pfile_flush(TRUE, dwChar);
}

void pfile_write_save_file(const char *pszName, BYTE *pbData, DWORD dwLen, DWORD qwLen)
{
	DWORD save_num;

	save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	{
		const char *password;
		if (gbIsSpawn) {
			password = PASSWORD_SPAWN_SINGLE;
			if (gbIsMultiplayer)
				password = PASSWORD_SPAWN_MULTI;
		} else {
			password = PASSWORD_SINGLE;
			if (gbIsMultiplayer)
				password = PASSWORD_MULTI;
		}

		codec_encode(pbData, dwLen, qwLen, password);
	}
	if (!pfile_open_archive(save_num))
		app_fatal("Unable to write to save file archive");
	mpqapi_write_file(pszName, pbData, qwLen);
	pfile_flush(TRUE, save_num);
}

BYTE *pfile_read(const char *pszName, DWORD *pdwLen)
{
	DWORD save_num;
	HANDLE archive;
	BYTE *buf;

	save_num = pfile_get_save_num_from_name(plr[myplr]._pName);
	archive = pfile_open_save_archive(save_num);
	if (archive == NULL)
		app_fatal("Unable to open save file archive");

	buf = pfile_read_archive(archive, pszName, pdwLen);
	pfile_SFileCloseArchive(archive);
	if (buf == NULL)
		app_fatal("Invalid %s file", pszName);

	return buf;
}

void pfile_update(bool force_save)
{
	static Uint32 save_prev_tc;

	if (!gbIsMultiplayer)
		return;

	Uint32 tick = SDL_GetTicks();
	if (!force_save && tick - save_prev_tc <= 60000)
		return;

	save_prev_tc = tick;
	pfile_write_hero();
}

DEVILUTION_END_NAMESPACE
