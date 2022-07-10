/**
 * @file pfile.cpp
 *
 * Implementation of the save game encoding functionality.
 */
#include "pfile.h"

#include <string>

#include <fmt/compile.h>

#include "codec.h"
#include "engine.h"
#include "init.h"
#include "loadsave.h"
#include "menu.h"
#include "mpq/mpq_reader.hpp"
#include "pack.h"
#include "qol/stash.h"
#include "utils/endian.hpp"
#include "utils/file_util.h"
#include "utils/language.h"
#include "utils/paths.h"
#include "utils/stdcompat/string_view.hpp"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

namespace devilution {

#define PASSWORD_SPAWN_SINGLE "adslhfb1"
#define PASSWORD_SPAWN_MULTI "lshbkfg1"
#define PASSWORD_SINGLE "xrgyrkj1"
#define PASSWORD_MULTI "szqnlsk1"

bool gbValidSaveFile;

namespace {

/** List of character names for the character selection screen. */
char hero_names[MAX_CHARACTERS][PlayerNameLength];

std::string GetSavePath(uint32_t saveNum, string_view savePrefix = {})
{
	return StrCat(paths::PrefPath(), savePrefix,
	    gbIsSpawn
	        ? (gbIsMultiplayer ? "share_" : "spawn_")
	        : (gbIsMultiplayer ? "multi_" : "single_"),
	    saveNum, gbIsHellfire ? ".hsv" : ".sv");
}

std::string GetStashSavePath()
{
	return StrCat(paths::PrefPath(),
	    gbIsSpawn ? "stash_spawn" : "stash",
	    gbIsHellfire ? ".hsv" : ".sv");
}

bool GetSaveNames(uint8_t index, string_view prefix, char *out)
{
	char suf;
	if (index < giNumberOfLevels)
		suf = 'l';
	else if (index < giNumberOfLevels * 2) {
		index -= giNumberOfLevels;
		suf = 's';
	} else {
		return false;
	}

	*fmt::format_to(out, FMT_COMPILE("{}{}{:02d}"), prefix, suf, index) = '\0';
	return true;
}

bool GetPermSaveNames(uint8_t dwIndex, char *szPerm)
{
	return GetSaveNames(dwIndex, "perm", szPerm);
}

bool GetTempSaveNames(uint8_t dwIndex, char *szTemp)
{
	return GetSaveNames(dwIndex, "temp", szTemp);
}

void RenameTempToPerm(MpqWriter &saveWriter)
{
	char szTemp[MAX_PATH];
	char szPerm[MAX_PATH];

	uint32_t dwIndex = 0;
	while (GetTempSaveNames(dwIndex, szTemp)) {
		[[maybe_unused]] bool result = GetPermSaveNames(dwIndex, szPerm); // DO NOT PUT DIRECTLY INTO ASSERT!
		assert(result);
		dwIndex++;
		if (saveWriter.HasFile(szTemp)) {
			if (saveWriter.HasFile(szPerm))
				saveWriter.RemoveHashEntry(szPerm);
			saveWriter.RenameFile(szTemp, szPerm);
		}
	}
	assert(!GetPermSaveNames(dwIndex, szPerm));
}

bool ReadHero(MpqArchive &archive, PlayerPack *pPack)
{
	size_t read;

	auto buf = ReadArchive(archive, "hero", &read);
	if (buf == nullptr)
		return false;

	bool ret = false;
	if (read == sizeof(*pPack)) {
		memcpy(pPack, buf.get(), sizeof(*pPack));
		ret = true;
	}

	return ret;
}

void EncodeHero(MpqWriter &saveWriter, const PlayerPack *pack)
{
	size_t packedLen = codec_get_encoded_len(sizeof(*pack));
	std::unique_ptr<byte[]> packed { new byte[packedLen] };

	memcpy(packed.get(), pack, sizeof(*pack));
	codec_encode(packed.get(), sizeof(*pack), packedLen, pfile_get_password());
	saveWriter.WriteFile("hero", packed.get(), packedLen);
}

MpqWriter GetSaveWriter(uint32_t saveNum)
{
	return MpqWriter(GetSavePath(saveNum).c_str());
}

MpqWriter GetStashWriter()
{
	return MpqWriter(GetStashSavePath().c_str());
}

void Game2UiPlayer(const Player &player, _uiheroinfo *heroinfo, bool bHasSaveFile)
{
	CopyUtf8(heroinfo->name, player._pName, sizeof(heroinfo->name));
	heroinfo->level = player._pLevel;
	heroinfo->heroclass = player._pClass;
	heroinfo->strength = player._pStrength;
	heroinfo->magic = player._pMagic;
	heroinfo->dexterity = player._pDexterity;
	heroinfo->vitality = player._pVitality;
	heroinfo->hassaved = bHasSaveFile;
	heroinfo->herorank = player.pDiabloKillLevel;
	heroinfo->spawned = gbIsSpawn;
}

bool GetFileName(uint8_t lvl, char *dst)
{
	if (gbIsMultiplayer) {
		if (lvl != 0)
			return false;
		memcpy(dst, "hero", 5);
		return true;
	}
	if (GetPermSaveNames(lvl, dst)) {
		return true;
	}
	if (lvl == giNumberOfLevels * 2) {
		memcpy(dst, "game", 5);
		return true;
	}
	if (lvl == giNumberOfLevels * 2 + 1) {
		memcpy(dst, "hero", 5);
		return true;
	}
	return false;
}

bool ArchiveContainsGame(MpqArchive &hsArchive)
{
	if (gbIsMultiplayer)
		return false;

	auto gameData = ReadArchive(hsArchive, "game");
	if (gameData == nullptr)
		return false;

	uint32_t hdr = LoadLE32(gameData.get());

	return IsHeaderValid(hdr);
}

bool CompareSaves(const std::string &actualSavePath, const std::string &referenceSavePath)
{
	std::vector<std::string> possibleFileNamesToCheck;
	possibleFileNamesToCheck.emplace_back("hero");
	possibleFileNamesToCheck.emplace_back("game");
	possibleFileNamesToCheck.emplace_back("additionalMissiles");
	char szPerm[MAX_PATH];
	for (int i = 0; GetPermSaveNames(i, szPerm); i++) {
		possibleFileNamesToCheck.emplace_back(szPerm);
	}

	std::int32_t error;
	auto actualArchive = *MpqArchive::Open(actualSavePath.c_str(), error);
	auto referenceArchive = *MpqArchive::Open(referenceSavePath.c_str(), error);

	bool compareResult = true;
	for (const auto &fileName : possibleFileNamesToCheck) {
		size_t fileSizeActual;
		auto fileDataActual = ReadArchive(actualArchive, fileName.c_str(), &fileSizeActual);
		size_t fileSizeReference;
		auto fileDataReference = ReadArchive(referenceArchive, fileName.c_str(), &fileSizeReference);
		if (fileDataActual.get() == nullptr && fileDataReference.get() == nullptr) {
			continue;
		}
		if (fileSizeActual != fileSizeReference) {
			compareResult = false;
			break;
		}
		if (memcmp(fileDataReference.get(), fileDataActual.get(), fileSizeActual) != 0) {
			compareResult = false;
			break;
		}
	}
	return compareResult;
}

void pfile_write_hero(MpqWriter &saveWriter, bool writeGameData)
{
	if (writeGameData) {
		SaveGameData(saveWriter);
		RenameTempToPerm(saveWriter);
	}
	PlayerPack pkplr;
	Player &myPlayer = *MyPlayer;

	PackPlayer(&pkplr, myPlayer, !gbIsMultiplayer, false);
	EncodeHero(saveWriter, &pkplr);
	if (!gbVanilla) {
		SaveHotkeys(saveWriter);
		SaveHeroItems(saveWriter, myPlayer);
	}
}

} // namespace

std::optional<MpqArchive> OpenSaveArchive(uint32_t saveNum)
{
	std::int32_t error;
	return MpqArchive::Open(GetSavePath(saveNum).c_str(), error);
}

std::optional<MpqArchive> OpenStashArchive()
{
	std::int32_t error;
	return MpqArchive::Open(GetStashSavePath().c_str(), error);
}

std::unique_ptr<byte[]> ReadArchive(MpqArchive &archive, const char *pszName, size_t *pdwLen)
{
	int32_t error;
	std::size_t length;

	std::unique_ptr<byte[]> result = archive.ReadFile(pszName, length, error);
	if (error != 0)
		return nullptr;

	std::size_t decodedLength = codec_decode(result.get(), length, pfile_get_password());
	if (decodedLength == 0)
		return nullptr;

	if (pdwLen != nullptr)
		*pdwLen = decodedLength;

	return result;
}

const char *pfile_get_password()
{
	if (gbIsSpawn)
		return gbIsMultiplayer ? PASSWORD_SPAWN_MULTI : PASSWORD_SPAWN_SINGLE;
	return gbIsMultiplayer ? PASSWORD_MULTI : PASSWORD_SINGLE;
}

void pfile_write_hero(bool writeGameData)
{
	MpqWriter saveWriter = GetSaveWriter(gSaveNumber);
	pfile_write_hero(saveWriter, writeGameData);
}

void pfile_write_hero_demo(int demo)
{
	std::string savePath = GetSavePath(gSaveNumber, StrCat("demo_", demo, "_reference_"));
	auto saveWriter = MpqWriter(savePath.c_str());
	pfile_write_hero(saveWriter, true);
}

HeroCompareResult pfile_compare_hero_demo(int demo)
{
	std::string referenceSavePath = GetSavePath(gSaveNumber, StrCat("demo_", demo, "_reference_"));

	if (!FileExists(referenceSavePath.c_str()))
		return HeroCompareResult::ReferenceNotFound;

	std::string actualSavePath = GetSavePath(gSaveNumber, StrCat("demo_", demo, "_actual_"));
	{
		MpqWriter saveWriter(actualSavePath.c_str());
		pfile_write_hero(saveWriter, true);
	}

	bool compareResult = CompareSaves(actualSavePath, referenceSavePath);
	return compareResult ? HeroCompareResult::Same : HeroCompareResult::Difference;
}

void sfile_write_stash()
{
	if (!Stash.dirty)
		return;

	MpqWriter stashWriter = GetStashWriter();

	SaveStash(stashWriter);

	Stash.dirty = false;
}

bool pfile_ui_set_hero_infos(bool (*uiAddHeroInfo)(_uiheroinfo *))
{
	memset(hero_names, 0, sizeof(hero_names));

	for (uint32_t i = 0; i < MAX_CHARACTERS; i++) {
		std::optional<MpqArchive> archive = OpenSaveArchive(i);
		if (archive) {
			PlayerPack pkplr;
			if (ReadHero(*archive, &pkplr)) {
				_uiheroinfo uihero;
				uihero.saveNumber = i;
				strcpy(hero_names[i], pkplr.pName);
				bool hasSaveGame = ArchiveContainsGame(*archive);
				if (hasSaveGame)
					pkplr.bIsHellfire = gbIsHellfireSaveGame ? 1 : 0;

				Player &player = Players[0];

				player = {};

				if (UnPackPlayer(&pkplr, player, false)) {
					LoadHeroItems(player);
					RemoveEmptyInventory(player);
					CalcPlrInv(player, false);

					Game2UiPlayer(player, &uihero, hasSaveGame);
					uiAddHeroInfo(&uihero);
				}
			}
		}
	}

	return true;
}

void pfile_ui_set_class_stats(unsigned int playerClass, _uidefaultstats *classStats)
{
	classStats->strength = StrengthTbl[playerClass];
	classStats->magic = MagicTbl[playerClass];
	classStats->dexterity = DexterityTbl[playerClass];
	classStats->vitality = VitalityTbl[playerClass];
}

uint32_t pfile_ui_get_first_unused_save_num()
{
	uint32_t saveNum;
	for (saveNum = 0; saveNum < MAX_CHARACTERS; saveNum++) {
		if (hero_names[saveNum][0] == '\0')
			break;
	}
	return saveNum;
}

bool pfile_ui_save_create(_uiheroinfo *heroinfo)
{
	PlayerPack pkplr;

	uint32_t saveNum = heroinfo->saveNumber;
	if (saveNum >= MAX_CHARACTERS)
		return false;
	heroinfo->saveNumber = saveNum;

	giNumberOfLevels = gbIsHellfire ? 25 : 17;

	MpqWriter saveWriter = GetSaveWriter(saveNum);
	saveWriter.RemoveHashEntries(GetFileName);
	CopyUtf8(hero_names[saveNum], heroinfo->name, sizeof(hero_names[saveNum]));

	Player &player = Players[0];
	CreatePlayer(player, heroinfo->heroclass);
	CopyUtf8(player._pName, heroinfo->name, PlayerNameLength);
	PackPlayer(&pkplr, player, true, false);
	EncodeHero(saveWriter, &pkplr);
	Game2UiPlayer(player, heroinfo, false);
	if (!gbVanilla) {
		SaveHotkeys(saveWriter);
		SaveHeroItems(saveWriter, player);
	}

	return true;
}

bool pfile_delete_save(_uiheroinfo *heroInfo)
{
	uint32_t saveNum = heroInfo->saveNumber;
	if (saveNum < MAX_CHARACTERS) {
		hero_names[saveNum][0] = '\0';
		RemoveFile(GetSavePath(saveNum).c_str());
	}
	return true;
}

void pfile_read_player_from_save(uint32_t saveNum, Player &player)
{
	player = {};

	PlayerPack pkplr;
	{
		std::optional<MpqArchive> archive = OpenSaveArchive(saveNum);
		if (!archive)
			app_fatal(_("Unable to open archive"));
		if (!ReadHero(*archive, &pkplr))
			app_fatal(_("Unable to load character"));

		gbValidSaveFile = ArchiveContainsGame(*archive);
		if (gbValidSaveFile)
			pkplr.bIsHellfire = gbIsHellfireSaveGame ? 1 : 0;
	}

	if (!UnPackPlayer(&pkplr, player, false)) {
		return;
	}

	LoadHeroItems(player);
	RemoveEmptyInventory(player);
	CalcPlrInv(player, false);
}

void pfile_save_level()
{
	MpqWriter saveWriter = GetSaveWriter(gSaveNumber);
	SaveLevel(saveWriter);
}

void pfile_convert_levels()
{
	MpqWriter saveWriter = GetSaveWriter(gSaveNumber);
	ConvertLevels(saveWriter);
}

void pfile_remove_temp_files()
{
	if (gbIsMultiplayer)
		return;

	MpqWriter saveWriter = GetSaveWriter(gSaveNumber);
	saveWriter.RemoveHashEntries(GetTempSaveNames);
}

void pfile_update(bool forceSave)
{
	static Uint32 prevTick;

	if (!gbIsMultiplayer)
		return;

	Uint32 tick = SDL_GetTicks();
	if (!forceSave && tick - prevTick <= 60000)
		return;

	prevTick = tick;
	pfile_write_hero();
	sfile_write_stash();
}

} // namespace devilution
