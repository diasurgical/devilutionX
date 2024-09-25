/**
 * @file capture.cpp
 *
 * Implementation of the screenshot function.
 */
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <SDL.h>
#include <expected.hpp>
#include <fmt/format.h>

#define DEVILUTIONX_SCREENSHOT_FORMAT_PCX 0
#define DEVILUTIONX_SCREENSHOT_FORMAT_PNG 1

#if DEVILUTIONX_SCREENSHOT_FORMAT == DEVILUTIONX_SCREENSHOT_FORMAT_PCX
#include "utils/surface_to_pcx.hpp"
#endif
#if DEVILUTIONX_SCREENSHOT_FORMAT == DEVILUTIONX_SCREENSHOT_FORMAT_PNG
#include "utils/surface_to_png.hpp"
#endif

#include "engine/backbuffer_state.hpp"
#include "engine/dx.h"
#include "engine/palette.h"
#include "plrmsg.h"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/str_cat.hpp"

namespace devilution {
namespace {

FILE *CaptureFile(std::string *dstPath)
{
	const char *ext =
#if DEVILUTIONX_SCREENSHOT_FORMAT == DEVILUTIONX_SCREENSHOT_FORMAT_PCX
	    ".pcx";
#elif DEVILUTIONX_SCREENSHOT_FORMAT == DEVILUTIONX_SCREENSHOT_FORMAT_PNG
	    ".png";
#endif
	const std::time_t tt = std::time(nullptr);
	const std::tm *tm = std::localtime(&tt);
	const std::string filename = tm != nullptr
	    ? fmt::format("{:04}-{:02}-{:02} {:02}-{:02}-{:02}",
	        tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec)
	    : "Screenshot";
	*dstPath = StrCat(paths::PrefPath(), filename, ext);
	int i = 0;
	while (FileExists(dstPath->c_str())) {
		i++;
		*dstPath = StrCat(paths::PrefPath(), filename, "-", i, ext);
	}
	return OpenFile(dstPath->c_str(), "wb");
}

} // namespace

std::string FileNameForChat(const std::string &fullPath)
{
	// Extracts just the file name from the full path
	auto pos = fullPath.find_last_of("\\/");
	if (pos == std::string::npos) {
		return fullPath;
	}
	return fullPath.substr(pos + 1);
}

void CaptureScreen()
{
	std::string fileName;

	FILE *outStream = CaptureFile(&fileName);
	if (outStream == nullptr) {
		auto errorMessage = fmt::format(fmt::runtime(_(/* TRANSLATORS: {fileName} is the file path where the screenshot was attempted to be saved. */ "Failed to open {} for writing: {}")), fileName, std::strerror(errno));
		LogError("{}", errorMessage);

		auto chatMessage = fmt::format(fmt::runtime(_(/* TRANSLATORS: {fileName} is the name of the file where the screenshot was attempted to be saved. */ "Failed to open {} for writing")), FileNameForChat(fileName));
		EventPlrMsg(chatMessage, UiFlags::ColorWhitegold);
		return;
	}

	const tl::expected<void, std::string> result =
#if DEVILUTIONX_SCREENSHOT_FORMAT == DEVILUTIONX_SCREENSHOT_FORMAT_PCX
	    WriteSurfaceToFilePcx(GlobalBackBuffer(), outStream);
#elif DEVILUTIONX_SCREENSHOT_FORMAT == DEVILUTIONX_SCREENSHOT_FORMAT_PNG
	    WriteSurfaceToFilePng(GlobalBackBuffer(), outStream);
#endif

	if (!result.has_value()) {
		auto errorMessage = fmt::format(fmt::runtime(_(/* TRANSLATORS: {fileName} is the file path where the screenshot was attempted to be saved. {result.error()} is the error message returned during the save attempt. */ "Failed to save screenshot {}: {}")), fileName, result.error());
		LogError("{}", errorMessage);

		auto chatMessage = fmt::format(fmt::runtime(_(/* TRANSLATORS: {fileName} is the name of the file where the screenshot was attempted to be saved. */ "Failed to save screenshot {}")), FileNameForChat(fileName));
		EventPlrMsg(chatMessage, UiFlags::ColorWhitegold);
		RemoveFile(fileName.c_str());
	} else {
		auto successMessage = fmt::format(fmt::runtime(_(/* TRANSLATORS: {fileName} is the file path where the screenshot was successfully saved. */ "Screenshot saved at {}")), fileName);
		Log("{}", successMessage);

		auto chatMessage = fmt::format(fmt::runtime(_(/* TRANSLATORS: {fileName} is the name of the file where the screenshot was successfully saved. */ "Screenshot saved as {}")), FileNameForChat(fileName));
		EventPlrMsg(chatMessage, UiFlags::ColorWhitegold);
	}
}

} // namespace devilution
