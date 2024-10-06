/**
 * @file pfile.cpp
 *
 * Implementation of the save game encoding functionality.
 */
#include "pfile.h"

#include <cstdint>
#include <string>
#include <string_view>

#include <ankerl/unordered_dense.h>
#include <fmt/core.h>

#include "codec.h"
#include "engine.h"
#include "engine/load_file.hpp"
#include "init.h"
#include "loadsave.h"
#include "menu.h"
#include "mpq/mpq_common.hpp"
#include "pack.h"
#include "playerdat.hpp"
#include "qol/stash.h"
#include "utils/endian.hpp"
#include "utils/file_util.h"
#include "utils/language.h"
#include "utils/parse_int.hpp"
#include "utils/paths.h"
#include "utils/stdcompat/filesystem.hpp"
#include "utils/str_cat.hpp"
#include "utils/str_split.hpp"
#include "utils/utf8.hpp"

#ifdef __DREAMCAST__
#include <dc/vmu_pkg.h>
#include <kos/fs.h>
#include <kos/fs_ramdisk.h>
#include <libgen.h>
#endif

#ifdef UNPACKED_SAVES
#include "utils/file_util.h"
#else
#include "mpq/mpq_reader.hpp"
#endif

namespace devilution {

#define PASSWORD_SPAWN_SINGLE "adslhfb1"
#define PASSWORD_SPAWN_MULTI "lshbkfg1"
#define PASSWORD_SINGLE "xrgyrkj1"
#define PASSWORD_MULTI "szqnlsk1"

bool gbValidSaveFile;

void listdir(const char *dir, int depth)
{
	file_t d = fs_open(dir, O_RDONLY | O_DIR);
	dirent_t *entry;
	printf("============ %s ============\n", dir);
	while (NULL != (entry = fs_readdir(d))) {
		char absolutePath[1024];
		strcpy(absolutePath, dir);
		strcat(absolutePath, "/");
		strcat(absolutePath, entry->name);
		bool isDir = entry->size == -1;
		printf("[%s]\t%.2f kB\t%s\n", isDir ? "DIR" : "FIL", entry->size / 1024.0, entry->name);
		if (isDir) {
			printf("absolutePath = %s, depth = %d\n", absolutePath, depth);
			listdir(absolutePath, depth + 1);
		}
	}
	fs_close(d);
	printf("============ %s ============\n\n\n", dir);
}
namespace {

/** List of character names for the character selection screen. */
char hero_names[MAX_CHARACTERS][PlayerNameLength];

std::string GetSavePath(uint32_t saveNum, std::string_view savePrefix = {})
{
	// shorter names to get around VMU filename size limits
	return StrCat(paths::PrefPath(), savePrefix,
	    gbIsSpawn
	        ? (gbIsMultiplayer ? "M" : "S")
	        : (gbIsMultiplayer ? "m" : "s"),
	    saveNum,
#ifdef UNPACKED_SAVES
#ifdef __DREAMCAST__
	    // flatten directory structure for easier fs_ramdisk_* usage
	    // for example, /ram/spawn_sv/hero would become /ram/spawn_sv_hero

	    gbIsHellfire ? "_hsv" DIRECTORY_SEPARATOR_STR : "_sv_"
#else
	    gbIsHellfire ? "_hsv" DIRECTORY_SEPARATOR_STR : "_sv" DIRECTORY_SEPARATOR_STR
#endif
#else
	    gbIsHellfire ? ".hsv" : ".sv"
#endif
	);
}

std::string GetStashSavePath()
{
	return StrCat(paths::PrefPath(),
	    gbIsSpawn ? "stash_spawn" : "stash",
#ifdef UNPACKED_SAVES
#ifdef __DREAMCAST__
	    // same as above
	    gbIsHellfire ? "_hsv" DIRECTORY_SEPARATOR_STR : "_sv_"
#else
	    gbIsHellfire ? "_hsv" DIRECTORY_SEPARATOR_STR : "_sv" DIRECTORY_SEPARATOR_STR
#endif
#else
	    gbIsHellfire ? ".hsv" : ".sv"
#endif
	);
}

bool GetSaveNames(uint8_t index, std::string_view prefix, char *out)
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

	*fmt::format_to(out, "{}{}{:02d}", prefix, suf, index) = '\0';
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

void RenameTempToPerm(SaveWriter &saveWriter)
{
	Log("RenameTempToPerm");
	char szTemp[MaxMpqPathSize];
	char szPerm[MaxMpqPathSize];

	uint32_t dwIndex = 0;
	while (GetTempSaveNames(dwIndex, szTemp)) {
		[[maybe_unused]] bool result = GetPermSaveNames(dwIndex, szPerm); // DO NOT PUT DIRECTLY INTO ASSERT!
		assert(result);
		dwIndex++;
		Log("GetPermSaveNames({}, \"{}\")", dwIndex, szTemp);
		if (saveWriter.HasFile(szTemp)) {
			Log("saveWriter.HasFile(\"{}\") = true", szTemp);
			if (saveWriter.HasFile(szPerm)) {
				Log("saveWriter.HasFile(\"{}\") = true => RemoveHashEntry", szPerm);
				saveWriter.RemoveHashEntry(szPerm);
			}
			Log("saveWriter.RenameFile(\"{}\", {})", szTemp, szPerm);
			saveWriter.RenameFile(szTemp, szPerm);
		}
	}
	assert(!GetPermSaveNames(dwIndex, szPerm));
}

bool ReadHero(SaveReader &archive, PlayerPack *pPack)
{
	size_t read;

	auto buf = ReadArchive(archive, "hero", &read);
	if (buf == nullptr) {
		Log("ReadArchive(archive, \"hero\", {}) = false", read);
		return false;
	}

	bool ret = false;
	if (read == sizeof(*pPack)) {
		memcpy(pPack, buf.get(), sizeof(*pPack));
		ret = true;
	}
	Log("{} == sizeof(*pPack) ({}) = {}", read, sizeof(*pPack), read == sizeof(*pPack));
	Log("Read player {}", pPack->pName);
	// Log("\tpHPBase = {}", pPack->pHPBase);

	listdir("/ram", 0);
	listdir("/vmu/a1", 0);
	return ret;
}

void EncodeHero(SaveWriter &saveWriter, const PlayerPack *pack)
{
	Log("EncodeHero");
	size_t packedLen = codec_get_encoded_len(sizeof(*pack));
	std::unique_ptr<std::byte[]> packed { new std::byte[packedLen] };

	Log("memcpy(packed.get(), pack, {})", sizeof(*pack));
	memcpy(packed.get(), pack, sizeof(*pack));
	codec_encode(packed.get(), sizeof(*pack), packedLen, pfile_get_password());
	Log("Saving player {}", pack->pName);
	// Log("\tpHPBase = {}", pack->pHPBase);
	bool result = saveWriter.WriteFile("hero", packed.get(), packedLen /* sizeof(*pack) */);
	Log("saveWriter.WriteFile(\"hero\", packed.get(), {}) = {}", packedLen, result);
}

SaveWriter GetSaveWriter(uint32_t saveNum)
{
	return SaveWriter(GetSavePath(saveNum));
}

SaveWriter GetStashWriter()
{
	return SaveWriter(GetStashSavePath());
}

#ifndef DISABLE_DEMOMODE
void CopySaveFile(uint32_t saveNum, std::string targetPath)
{
	const std::string savePath = GetSavePath(saveNum);
#if defined(UNPACKED_SAVES)
#ifdef DVL_NO_FILESYSTEM
#error "UNPACKED_SAVES requires either DISABLE_DEMOMODE or C++17 <filesystem>"
#endif
	CreateDir(targetPath.c_str());
	for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(savePath)) {
		CopyFileOverwrite(entry.path().string().c_str(), (targetPath + entry.path().filename().string()).c_str());
	}
#else
	CopyFileOverwrite(savePath.c_str(), targetPath.c_str());
#endif
}
#endif

void Game2UiPlayer(const Player &player, _uiheroinfo *heroinfo, bool bHasSaveFile)
{
	CopyUtf8(heroinfo->name, player._pName, sizeof(heroinfo->name));
	heroinfo->level = player.getCharacterLevel();
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

bool ArchiveContainsGame(SaveReader &hsArchive)
{
	if (gbIsMultiplayer)
		return false;

	auto gameData = ReadArchive(hsArchive, "game");
	if (gameData == nullptr)
		return false;

	uint32_t hdr = LoadLE32(gameData.get());

	return IsHeaderValid(hdr);
}

std::optional<SaveReader> CreateSaveReader(std::string &&path)
{
#ifdef UNPACKED_SAVES
#ifdef __DREAMCAST__
	Log("\tAttempting to load save file {}", path);
	// no notion of directories in ramdisk, so /ram/spawn_0_sv/ doesn't exist
	// instead, we check for /ram/spawn_0_sv_hero which was previously created
	std::string heroFile = path + "hero";
	if (!FileExists(heroFile)) {
		Log("\tFailed ):");
		return std::nullopt;
	}
	Log("\tFound save path {} (:", path);
	return SaveReader(std::move(path));
#else
	if (!FileExists(path))
		return std::nullopt;
	return SaveReader(std::move(path));
#endif
#else
	std::int32_t error;
	return MpqArchive::Open(path.c_str(), error);
#endif
}

#ifndef DISABLE_DEMOMODE
struct CompareInfo {
	std::unique_ptr<std::byte[]> &data;
	size_t currentPosition;
	size_t size;
	bool isTownLevel;
	bool dataExists;
};

struct CompareCounter {
	int reference;
	int actual;
	int max()
	{
		return std::max(reference, actual);
	}
	void checkIfDataExists(int count, CompareInfo &compareInfoReference, CompareInfo &compareInfoActual)
	{
		if (reference == count)
			compareInfoReference.dataExists = false;
		if (actual == count)
			compareInfoActual.dataExists = false;
	}
};

inline bool string_ends_with(std::string_view value, std::string_view suffix)
{
	if (suffix.size() > value.size())
		return false;
	return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

void CreateDetailDiffs(std::string_view prefix, std::string_view memoryMapFile, CompareInfo &compareInfoReference, CompareInfo &compareInfoActual, ankerl::unordered_dense::segmented_map<std::string, size_t> &foundDiffs)
{
	// Note: Detail diffs are currently only supported in unit tests
	std::string memoryMapFileAssetName = StrCat(paths::BasePath(), "/test/fixtures/memory_map/", memoryMapFile, ".txt");

	SDL_RWops *handle = SDL_RWFromFile(memoryMapFileAssetName.c_str(), "r");
	if (handle == nullptr) {
		app_fatal(StrCat("MemoryMapFile ", memoryMapFile, " is missing"));
		return;
	}

	size_t readBytes = static_cast<size_t>(SDL_RWsize(handle));
	std::unique_ptr<std::byte[]> memoryMapFileData { new std::byte[readBytes] };
#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_RWread(handle, memoryMapFileData.get(), readBytes, 1);
#else
	SDL_RWread(handle, memoryMapFileData.get(), static_cast<int>(readBytes), 1);
#endif
	SDL_RWclose(handle);

	const std::string_view buffer(reinterpret_cast<const char *>(memoryMapFileData.get()), readBytes);

	ankerl::unordered_dense::segmented_map<std::string, CompareCounter> counter;

	auto getCounter = [&](const std::string &counterAsString) {
		auto it = counter.find(counterAsString);
		if (it != counter.end())
			return it->second;
		const ParseIntResult<int> countFromMapFile = ParseInt<int>(counterAsString);
		if (!countFromMapFile.has_value())
			app_fatal(StrCat("Failed to parse ", counterAsString, " as int"));
		return CompareCounter { countFromMapFile.value(), countFromMapFile.value() };
	};
	auto addDiff = [&](const std::string &diffKey) {
		auto it = foundDiffs.find(diffKey);
		if (it == foundDiffs.end()) {
			foundDiffs.insert_or_assign(diffKey, 1);
		} else {
			foundDiffs.insert_or_assign(diffKey, it->second + 1);
		}
	};

	auto compareBytes = [&](size_t countBytes) {
		if (compareInfoReference.dataExists && compareInfoReference.currentPosition + countBytes > compareInfoReference.size)
			app_fatal(StrCat("Comparison failed. Not enough bytes in reference to compare. Location: ", prefix));
		if (compareInfoActual.dataExists && compareInfoActual.currentPosition + countBytes > compareInfoActual.size)
			app_fatal(StrCat("Comparison failed. Not enough bytes in actual to compare. Location: ", prefix));
		bool result = true;
		if (compareInfoReference.dataExists && compareInfoActual.dataExists)
			result = memcmp(compareInfoReference.data.get() + compareInfoReference.currentPosition, compareInfoActual.data.get() + compareInfoActual.currentPosition, countBytes) == 0;
		if (compareInfoReference.dataExists)
			compareInfoReference.currentPosition += countBytes;
		if (compareInfoActual.dataExists)
			compareInfoActual.currentPosition += countBytes;
		return result;
	};

	auto read32BitInt = [&](CompareInfo &compareInfo, bool useLE) {
		int32_t value = 0;
		if (!compareInfo.dataExists)
			return value;
		if (compareInfo.currentPosition + sizeof(value) > compareInfo.size)
			app_fatal("read32BitInt failed. Too less bytes to read.");
		memcpy(&value, compareInfo.data.get() + compareInfo.currentPosition, sizeof(value));
		if (useLE)
			value = SDL_SwapLE32(value);
		else
			value = SDL_SwapBE32(value);
		return value;
	};

	for (std::string_view line : SplitByChar(buffer, '\n')) {
		if (!line.empty() && line.back() == '\r')
			line.remove_suffix(1);
		if (line.empty())
			continue;
		const auto tokens = SplitByChar(line, ' ');
		auto it = tokens.begin();
		const auto end = tokens.end();
		if (it == end)
			continue;

		std::string_view command = *it;

		bool dataExistsReference = compareInfoReference.dataExists;
		bool dataExistsActual = compareInfoActual.dataExists;

		if (string_ends_with(command, "_HF")) {
			if (!gbIsHellfire)
				continue;
			command.remove_suffix(3);
		}
		if (string_ends_with(command, "_DA")) {
			if (gbIsHellfire)
				continue;
			command.remove_suffix(3);
		}
		if (string_ends_with(command, "_DL")) {
			if (compareInfoReference.isTownLevel && compareInfoActual.isTownLevel)
				continue;
			if (compareInfoReference.isTownLevel)
				compareInfoReference.dataExists = false;
			if (compareInfoActual.isTownLevel)
				compareInfoActual.dataExists = false;
			command.remove_suffix(3);
		}
		if (command == "R" || command == "LT" || command == "LC" || command == "LC_LE") {
			const auto bitsAsString = std::string(*++it);
			const auto comment = std::string(*++it);
			const ParseIntResult<size_t> parsedBytes = ParseInt<size_t>(bitsAsString);
			if (!parsedBytes.has_value())
				app_fatal(StrCat("Failed to parse ", bitsAsString, " as size_t"));
			const size_t bytes = static_cast<size_t>(parsedBytes.value() / 8);

			if (command == "LT") {
				int32_t valueReference = read32BitInt(compareInfoReference, false);
				int32_t valueActual = read32BitInt(compareInfoActual, false);
				assert(sizeof(valueReference) == bytes);
				compareInfoReference.isTownLevel = valueReference == 0;
				compareInfoActual.isTownLevel = valueActual == 0;
			}
			if (command == "LC" || command == "LC_LE") {
				int32_t valueReference = read32BitInt(compareInfoReference, command == "LC_LE");
				int32_t valueActual = read32BitInt(compareInfoActual, command == "LC_LE");
				assert(sizeof(valueReference) == bytes);
				counter.insert_or_assign(std::string(comment), CompareCounter { valueReference, valueActual });
			}

			if (!compareBytes(bytes)) {
				std::string diffKey = StrCat(prefix, ".", comment);
				addDiff(diffKey);
			}
		} else if (command == "M") {
			const auto countAsString = std::string(*++it);
			const auto bitsAsString = std::string(*++it);
			std::string_view comment = *++it;

			CompareCounter count = getCounter(countAsString);
			const ParseIntResult<size_t> parsedBytes = ParseInt<size_t>(bitsAsString);
			if (!parsedBytes.has_value())
				app_fatal(StrCat("Failed to parse ", bitsAsString, " as size_t"));
			const size_t bytes = static_cast<size_t>(parsedBytes.value() / 8);
			for (int i = 0; i < count.max(); i++) {
				count.checkIfDataExists(i, compareInfoReference, compareInfoActual);
				if (!compareBytes(bytes)) {
					std::string diffKey = StrCat(prefix, ".", comment);
					addDiff(diffKey);
				}
			}
		} else if (command == "C") {
			const auto countAsString = std::string(*++it);
			auto subMemoryMapFile = std::string(*++it);
			const auto comment = std::string(*++it);

			CompareCounter count = getCounter(countAsString);
			subMemoryMapFile.erase(std::remove(subMemoryMapFile.begin(), subMemoryMapFile.end(), '\r'), subMemoryMapFile.end());
			for (int i = 0; i < count.max(); i++) {
				count.checkIfDataExists(i, compareInfoReference, compareInfoActual);
				std::string subPrefix = StrCat(prefix, ".", comment);
				CreateDetailDiffs(subPrefix, subMemoryMapFile, compareInfoReference, compareInfoActual, foundDiffs);
			}
		}

		compareInfoReference.dataExists = dataExistsReference;
		compareInfoActual.dataExists = dataExistsActual;
	}
}

struct CompareTargets {
	std::string fileName;
	std::string memoryMapFileName;
	bool isTownLevel;
};

HeroCompareResult CompareSaves(const std::string &actualSavePath, const std::string &referenceSavePath, bool logDetails)
{
	std::vector<CompareTargets> possibleFileToCheck;
	possibleFileToCheck.push_back({ "hero", "hero", false });
	possibleFileToCheck.push_back({ "game", "game", false });
	possibleFileToCheck.push_back({ "additionalMissiles", "additionalMissiles", false });
	char szPerm[MaxMpqPathSize];
	for (int i = 0; GetPermSaveNames(i, szPerm); i++) {
		possibleFileToCheck.push_back({ std::string(szPerm), "level", i == 0 });
	}

	SaveReader actualArchive = *CreateSaveReader(std::string(actualSavePath));
	SaveReader referenceArchive = *CreateSaveReader(std::string(referenceSavePath));

	bool compareResult = true;
	std::string message;
	for (const auto &compareTarget : possibleFileToCheck) {
		size_t fileSizeActual = 0;
		auto fileDataActual = ReadArchive(actualArchive, compareTarget.fileName.c_str(), &fileSizeActual);
		size_t fileSizeReference = 0;
		auto fileDataReference = ReadArchive(referenceArchive, compareTarget.fileName.c_str(), &fileSizeReference);
		if (fileDataActual.get() == nullptr && fileDataReference.get() == nullptr) {
			continue;
		}
		if (fileSizeActual == fileSizeReference && memcmp(fileDataReference.get(), fileDataActual.get(), fileSizeActual) == 0)
			continue;
		compareResult = false;
		if (!message.empty())
			message.append("\n");
		if (fileSizeActual != fileSizeReference)
			StrAppend(message, "file \"", compareTarget.fileName, "\" is different size. Expected: ", fileSizeReference, " Actual: ", fileSizeActual);
		else
			StrAppend(message, "file \"", compareTarget.fileName, "\" has different content.");
		if (!logDetails)
			continue;
		ankerl::unordered_dense::segmented_map<std::string, size_t> foundDiffs;
		CompareInfo compareInfoReference = { fileDataReference, 0, fileSizeReference, compareTarget.isTownLevel, fileSizeReference != 0 };
		CompareInfo compareInfoActual = { fileDataActual, 0, fileSizeActual, compareTarget.isTownLevel, fileSizeActual != 0 };
		CreateDetailDiffs(compareTarget.fileName, compareTarget.memoryMapFileName, compareInfoReference, compareInfoActual, foundDiffs);
		if (compareInfoReference.currentPosition != fileSizeReference)
			app_fatal(StrCat("Comparison failed. Uncompared bytes in reference. File: ", compareTarget.fileName));
		if (compareInfoActual.currentPosition != fileSizeActual)
			app_fatal(StrCat("Comparison failed. Uncompared bytes in actual. File: ", compareTarget.fileName));
		for (const auto &[location, count] : foundDiffs) {
			StrAppend(message, "\nDiff found in ", location, " count: ", count);
		}
	}
	return { compareResult ? HeroCompareResult::Same : HeroCompareResult::Difference, message };
}
#endif // !DISABLE_DEMOMODE

void pfile_write_hero(SaveWriter &saveWriter, bool writeGameData)
{
	Log("pfile_write_hero with writeGameData = {}", writeGameData);
	if (writeGameData) {
		SaveGameData(saveWriter);
		RenameTempToPerm(saveWriter);
		Log("Game data saved");
	}
	PlayerPack pkplr;
	Player &myPlayer = *MyPlayer;

	PackPlayer(pkplr, myPlayer);
	Log("Player data packed");
	EncodeHero(saveWriter, &pkplr);
	Log("Player data saved");
	if (!gbVanilla) {
		SaveHotkeys(saveWriter, myPlayer);
		SaveHeroItems(saveWriter, myPlayer);
	}
}

void RemoveAllInvalidItems(Player &player)
{
	for (int i = 0; i < NUM_INVLOC; i++)
		RemoveInvalidItem(player.InvBody[i]);
	for (int i = 0; i < player._pNumInv; i++)
		RemoveInvalidItem(player.InvList[i]);
	for (int i = 0; i < MaxBeltItems; i++)
		RemoveInvalidItem(player.SpdList[i]);
	RemoveEmptyInventory(player);
}

} // namespace

#ifdef UNPACKED_SAVES
#ifdef __DREAMCAST__
std::unique_ptr<std::byte[]> SaveReader::ReadFile(const char *filename, std::size_t &fileSize, int32_t &error)
{
	Log("SaveReader::ReadFile(\"{}\", fileSize, error)", filename);
	error = 0;
	const std::string path = dir_ + filename;
	Log("path = \"{}\"", path);
	size_t size = 0;
	uint8 *contents;
	if (fs_load(path.c_str(), &contents) == -1) {
		error = 1;
		LogError("fs_load(\"{}\", &contents) = -1", path);
		app_fatal("SaveReader::ReadFile " + path + " KO");
		return nullptr;
	}
	vmu_pkg_t package;
	if (vmu_pkg_parse(contents, &package) < 0) {
		error = 1;
		free(contents);
		LogError("vmu_pkg_parse = -1");
		app_fatal("vmu_pkg_parse failed");
		return nullptr;
	}
	Log("Parsed package {} ({})", package.desc_short, package.desc_long);
	fileSize = package.data_len;
	std::unique_ptr<std::byte[]> result;
	result.reset(new std::byte[fileSize]);
	memcpy(result.get(), package.data, fileSize);
	// free(package.data);
	free(contents);
	return result;
}

/*
 * todo: add bzip compression to the inventory data (hitms)
std::byte* compressHeroItems(std::byte *data, size_t size)
{
    int bzBuffToBuffCompress( char*         dest,
                             unsigned int* destLen,
                             char*         source,
                             unsigned int  sourceLen,
                             int           blockSize100k,
                             int           verbosity,
                             int           workFactor );

    char *compressed = malloc(sizeof(std::byte) * size);
    size_t compressedLength;
    if(BZ_OK != bzBuffToBuffCompress(
            compressed,
            &compressedLength,
            data,
            size,

    )) {
        free(compressed);
    }
}*/

bool SaveWriter::WriteFile(const char *filename, const std::byte *data, size_t size)
{
	Log("SaveWriter::WriteFile(\"{}\", data[], {})", filename, size);
	const std::string path = dir_ + filename;
	Log("dir_ = {}", dir_);
	Log("path = {}", path);
	const char *baseName = basename(path.c_str());
	// vmu code
	if (dir_.starts_with("/vmu")) {
		vmu_pkg_t package;
		strcpy(package.app_id, "DevilutionX");
		strncpy(package.desc_short, filename, 20);
		strcpy(package.desc_long, "Diablo 1 save data");
		package.icon_cnt = 0;
		package.icon_anim_speed = 0;
		package.eyecatch_type = VMUPKG_EC_NONE;
		package.data_len = size;
		package.data = new uint8[size];
		memcpy(package.data, data, size);

		uint8 *contents;
		size_t packageSize;
		if (vmu_pkg_build(&package, &contents, &packageSize) < 0) {
			delete[] package.data;
			LogError("vmu_pkg_build failed");
			app_fatal("vmu_pkg_build failed");
			return false;
		}
		FILE *file = OpenFile(path.c_str(), "wb");
		if (file == nullptr) {
			delete[] package.data;
			free(contents);
			LogError("fopen(\"{}\", \"wb\") = nullptr", path);
			app_fatal("SaveReader::WriteFile KO");
			return false;
		}
		size_t written = std::fwrite(contents, sizeof(uint8), packageSize, file);
		if (written != packageSize) {
			delete[] package.data;
			free(contents);
			std::fclose(file);
			LogError("fwrite(data, {}, {}, file) = {} != -1", sizeof(uint8), packageSize, written);
			app_fatal("vmu fwrite call failed");
			return false;
		}
		if (std::fclose(file) != 0) {
			delete[] package.data;
			free(contents);
			LogError("fclose(file) = 0");
			app_fatal("fclose(file) = 0");
			return false;
		}
		delete[] package.data;
		free(contents);
		listdir("/vmu/a1", 0);
		return true;
	}

	// ramdisk code
	bool exists = FileExists(baseName);
	if (exists) {
		Log("{} exists, removing it", path);
		void *toFree;
		size_t ignore;
		int detach_result = fs_ramdisk_detach(baseName, &toFree, &ignore);
		free(toFree);
		Log("fs_ramdisk_detach result = {}", detach_result);
		if (detach_result == -1) {
			return false;
		}
	}
	Log("\tAllocating {} bytes for path {}", size, baseName);
	void *buffer = malloc(size);
	memcpy(buffer, data, size);
	Log("\tMallocation succeeded ? {}", buffer != NULL);
	int attach_result = fs_ramdisk_attach(baseName, buffer, size);
	Log("\tAttach result: {}", attach_result);
	Log("Current ramdisk contents:");
	listdir("/ram", 0);
	return attach_result != -1;
}
#else
std::unique_ptr<std::byte[]> SaveReader::ReadFile(const char *filename, std::size_t &fileSize, int32_t &error)
{
	std::unique_ptr<std::byte[]> result;
	error = 0;
	const std::string path = dir_ + filename;
	uintmax_t size;
	if (!GetFileSize(path.c_str(), &size)) {
		error = 1;
		return nullptr;
	}
	fileSize = size;
	FILE *file = OpenFile(path.c_str(), "rb");
	if (file == nullptr) {
		error = 1;
		return nullptr;
	}
	result.reset(new std::byte[size]);
	if (std::fread(result.get(), size, 1, file) != 1) {
		std::fclose(file);
		error = 1;
		return nullptr;
	}
	std::fclose(file);
	return result;
}

bool SaveWriter::WriteFile(const char *filename, const std::byte *data, size_t size)
{
	const std::string path = dir_ + filename;
	FILE *file = OpenFile(path.c_str(), "wb");
	if (file == nullptr) {
		return false;
	}
	if (std::fwrite(data, size, 1, file) != 1) {
		std::fclose(file);
		return false;
	}
	std::fclose(file);
	return true;
}
#endif // def __DREAMCAST__
void SaveWriter::RemoveHashEntries(bool (*fnGetName)(uint8_t, char *))
{
	char pszFileName[MaxMpqPathSize];

	for (uint8_t i = 0; fnGetName(i, pszFileName); i++) {
		Log("RemoveHashEntry(\"{}\")", pszFileName);
		RemoveHashEntry(pszFileName);
	}
}
#endif // def UNPACKED_SAVES

std::optional<SaveReader> OpenSaveArchive(uint32_t saveNum)
{
	return CreateSaveReader(GetSavePath(saveNum));
}

std::optional<SaveReader> OpenStashArchive()
{
	return CreateSaveReader(GetStashSavePath());
}

std::unique_ptr<std::byte[]> ReadArchive(SaveReader &archive, const char *pszName, size_t *pdwLen)
{
	int32_t error;
	std::size_t length;

	Log("ReadArchive(archive, \"{}\", {})", pszName, *pdwLen);
	Log("ReadArchive 0");
	std::unique_ptr<std::byte[]> result = archive.ReadFile(pszName, length, error);
	if (error != 0) {
		Log("ReadArchive 0 error = {}", error);
		app_fatal("ReadArchive 0 = " + error);
		return nullptr;
	}

	Log("ReadArchive 1, length = {}", length);
	std::size_t decodedLength = codec_decode(result.get(), length, pfile_get_password());
	if (decodedLength == 0) {
		Log("ReadArchive nullptr");
		app_fatal("decodedLength = 0");
		return nullptr;
	}
	if (strcmp(pszName, "hero") == 0) {
		PlayerPack pPack;
		memcpy(&pPack, result.get(), decodedLength);
		Log("ReadArchive player {}", pPack.pName);
		// Log("\tpHPBase = {}", pPack.pHPBase);
	}

	Log("ReadArchive 2");
	if (pdwLen != nullptr)
		*pdwLen = decodedLength;

	Log("ReadArchive 3 {}", decodedLength);
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
	SaveWriter saveWriter = GetSaveWriter(gSaveNumber);
	pfile_write_hero(saveWriter, writeGameData);
}

#ifndef DISABLE_DEMOMODE
void pfile_write_hero_demo(int demo)
{
	Log("pfile_write_hero_demo({})", demo);
	std::string savePath = GetSavePath(gSaveNumber, StrCat("demo_", demo, "_reference_"));
	CopySaveFile(gSaveNumber, savePath);
	auto saveWriter = SaveWriter(savePath.c_str());
	pfile_write_hero(saveWriter, true);
}

HeroCompareResult pfile_compare_hero_demo(int demo, bool logDetails)
{
	Log("pfile_compare_hero_demo({}, {})", demo, logDetails);
	std::string referenceSavePath = GetSavePath(gSaveNumber, StrCat("demo_", demo, "_reference_"));

	if (!FileExists(referenceSavePath.c_str()))
		return { HeroCompareResult::ReferenceNotFound, {} };

	std::string actualSavePath = GetSavePath(gSaveNumber, StrCat("demo_", demo, "_actual_"));
	{
		CopySaveFile(gSaveNumber, actualSavePath);
		SaveWriter saveWriter(actualSavePath.c_str());
		pfile_write_hero(saveWriter, true);
	}

	return CompareSaves(actualSavePath, referenceSavePath, logDetails);
}
#endif

void sfile_write_stash()
{
	if (!Stash.dirty)
		return;

	SaveWriter stashWriter = GetStashWriter();

	SaveStash(stashWriter);

	Stash.dirty = false;
}

bool pfile_ui_set_hero_infos(bool (*uiAddHeroInfo)(_uiheroinfo *))
{
	Log("pfile_ui_set_hero_infos");
	memset(hero_names, 0, sizeof(hero_names));

	for (uint32_t i = 0; i < MAX_CHARACTERS; i++) {
		std::optional<SaveReader> archive = OpenSaveArchive(i);
		if (archive) {
			PlayerPack pkplr;
			if (ReadHero(*archive, &pkplr)) {
				Log("ReadHero OK");
				Log("Player {}", pkplr.pName);
				// Log("Player {}, HP = {}", pkplr.pName, pkplr.pHPBase);
				_uiheroinfo uihero;
				uihero.saveNumber = i;
				strcpy(hero_names[i], pkplr.pName);
				Log("hero_names[{}] = {}", i, pkplr.pName);
				bool hasSaveGame = ArchiveContainsGame(*archive);
				if (hasSaveGame)
					pkplr.bIsHellfire = gbIsHellfireSaveGame ? 1 : 0;

				Player &player = Players[0];

				UnPackPlayer(pkplr, player);
				LoadHeroItems(player);
				RemoveAllInvalidItems(player);
				CalcPlrInv(player, false);

				Game2UiPlayer(player, &uihero, hasSaveGame);
				uiAddHeroInfo(&uihero);
			} else {
				Log("ReadHero(*archive, &pkplr) failed");
				app_fatal("ReadHero(*archive, &pkplr) failed");
			}
		}
	}

	Log("pfile_ui_set_hero_infos OK");
	return true;
}

void pfile_ui_set_class_stats(HeroClass playerClass, _uidefaultstats *classStats)
{
	const ClassAttributes &classAttributes = GetClassAttributes(playerClass);
	classStats->strength = classAttributes.baseStr;
	classStats->magic = classAttributes.baseMag;
	classStats->dexterity = classAttributes.baseDex;
	classStats->vitality = classAttributes.baseVit;
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
	Log("pfile_ui_save_create");
	PlayerPack pkplr;

	uint32_t saveNum = heroinfo->saveNumber;
	if (saveNum >= MAX_CHARACTERS)
		return false;
	heroinfo->saveNumber = saveNum;

	giNumberOfLevels = gbIsHellfire ? 25 : 17;

	Log("GetSaveWriter({})", saveNum);
	SaveWriter saveWriter = GetSaveWriter(saveNum);
	saveWriter.RemoveHashEntries(GetFileName);
	CopyUtf8(hero_names[saveNum], heroinfo->name, sizeof(hero_names[saveNum]));

	Player &player = Players[0];
	CreatePlayer(player, heroinfo->heroclass);
	CopyUtf8(player._pName, heroinfo->name, PlayerNameLength);
	PackPlayer(pkplr, player);
	EncodeHero(saveWriter, &pkplr);
	Game2UiPlayer(player, heroinfo, false);
	if (!gbVanilla) {
		SaveHotkeys(saveWriter, player);
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
	PlayerPack pkplr;
	{
		std::optional<SaveReader> archive = OpenSaveArchive(saveNum);
		if (!archive) {
			listdir("/ram", 0);
			listdir("/vmu/a1", 0);
			app_fatal(_("Unable to open archive"));
		}
		if (!ReadHero(*archive, &pkplr)) {
			listdir("/ram", 0);
			listdir("/vmu/a1", 0);
			app_fatal(_("Unable to load character"));
		}

		gbValidSaveFile = ArchiveContainsGame(*archive);
		if (gbValidSaveFile)
			pkplr.bIsHellfire = gbIsHellfireSaveGame ? 1 : 0;
	}

	UnPackPlayer(pkplr, player);
	LoadHeroItems(player);
	RemoveAllInvalidItems(player);
	CalcPlrInv(player, false);
	Log("pfile_read_player_from_save OK");
}

void pfile_save_level()
{
	SaveWriter saveWriter = GetSaveWriter(gSaveNumber);
	SaveLevel(saveWriter);
}

void pfile_convert_levels()
{
	SaveWriter saveWriter = GetSaveWriter(gSaveNumber);
	ConvertLevels(saveWriter);
}

void pfile_remove_temp_files()
{
	Log("pfile_remove_temp_files");
	if (gbIsMultiplayer)
		return;

	SaveWriter saveWriter = GetSaveWriter(gSaveNumber);
	saveWriter.RemoveHashEntries(GetTempSaveNames);
}

void pfile_update(bool forceSave)
{
	static Uint32 prevTick;

	if (!gbIsMultiplayer)
		return;

	Uint32 tick = SDL_GetTicks();
	// 600000 instead of 60000
	// 60000 ms is too frequent for the VMU, the game hangs too often and too long
	if (!forceSave && tick - prevTick <= 600000)
		return;

	Log("pfile_update({})", forceSave);
	prevTick = tick;
	pfile_write_hero();
	sfile_write_stash();
}

} // namespace devilution
