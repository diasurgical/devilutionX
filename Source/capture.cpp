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

#include "effects.h"
#include "engine/backbuffer_state.hpp"
#include "engine/dx.h"
#include "engine/palette.h"
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
	    ? fmt::format("Screenshot from {:04}-{:02}-{:02} {:02}-{:02}-{:02}",
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

void CaptureScreen()
{
	SDL_Color palette[256];
	std::string fileName;
	const uint32_t startTime = SDL_GetTicks();

	FILE *outStream = CaptureFile(&fileName);
	if (outStream == nullptr) {
		LogError("Failed to open {} for writing: {}", fileName, std::strerror(errno));
		return;
	}
	DrawAndBlit();

	auto tempPalette = system_palette;
	system_palette = orig_palette;
	palette_update();

	const tl::expected<void, std::string> result =
#if DEVILUTIONX_SCREENSHOT_FORMAT == DEVILUTIONX_SCREENSHOT_FORMAT_PCX
	    WriteSurfaceToFilePcx(GlobalBackBuffer(), outStream);
#elif DEVILUTIONX_SCREENSHOT_FORMAT == DEVILUTIONX_SCREENSHOT_FORMAT_PNG
	    WriteSurfaceToFilePng(GlobalBackBuffer(), outStream);
#endif

	if (!result.has_value()) {
		LogError("Failed to save screenshot at {}: ", fileName, result.error());
		RemoveFile(fileName.c_str());
	} else {
		Log("Screenshot saved at {}", fileName);
		PlaySFX(SfxID::MenuSelect);
	}

	system_palette = tempPalette;
	palette_update();
	RedrawEverything();
}

} // namespace devilution
