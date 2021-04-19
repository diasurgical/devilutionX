#include "utils/paths.h"

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#ifndef TTF_FONT_DIR
#define TTF_FONT_DIR ""
#endif

#ifndef TTF_FONT_NAME
#define TTF_FONT_NAME "CharisSILB.ttf"
#endif

namespace devilution {

namespace {

std::string *basePath = nullptr;
std::string *prefPath = nullptr;
std::string *configPath = nullptr;
std::string *ttfPath = nullptr;
std::string *ttfName = nullptr;

void AddTrailingSlash(std::string *path)
{
#ifdef _WIN32
	if (!path->empty() && path->back() != '\\')
		*path += '\\';
#else
	if (!path->empty() && path->back() != '/')
		*path += '/';
#endif
}

std::string *FromSDL(char *s)
{
	auto *result = new std::string(s != nullptr ? s : "");
	if (s != nullptr) {
		SDL_free(s);
	} else {
		SDL_Log("%s", SDL_GetError());
		SDL_ClearError();
	}
	return result;
}

} // namespace

const std::string &GetBasePath()
{
#ifdef __vita__
	if (basePath == NULL)
		basePath = new std::string(GetPrefPath());
#else
	if (basePath == nullptr)
		basePath = FromSDL(SDL_GetBasePath());
#endif
	return *basePath;
}

const std::string &GetPrefPath()
{
	if (prefPath == nullptr)
		prefPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
	return *prefPath;
}

const std::string &GetConfigPath()
{
	if (configPath == nullptr)
		configPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
	return *configPath;
}

const std::string &GetTtfPath()
{
	if (ttfPath == nullptr)
		ttfPath = new std::string(TTF_FONT_DIR);
	return *ttfPath;
}

const std::string &GetTtfName()
{
	if (ttfName == nullptr)
		ttfName = new std::string(TTF_FONT_NAME);
	return *ttfName;
}

void SetBasePath(const char *path)
{
	if (basePath == nullptr)
		basePath = new std::string;
	*basePath = path;
	AddTrailingSlash(basePath);
}

void SetPrefPath(const char *path)
{
	if (prefPath == nullptr)
		prefPath = new std::string;
	*prefPath = path;
	AddTrailingSlash(prefPath);
}

void SetConfigPath(const char *path)
{
	if (configPath == nullptr)
		configPath = new std::string;
	*configPath = path;
	AddTrailingSlash(configPath);
}

void SetTtfPath(const char *path)
{
	if (ttfPath == nullptr)
		ttfPath = new std::string;
	*ttfPath = path;
	AddTrailingSlash(ttfPath);
}

void SetTtfName(const char *path)
{
	if (ttfName == nullptr)
		ttfName = new std::string;
	*ttfName = path;
}

} // namespace devilution
