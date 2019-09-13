#include "paths.h"

#include <SDL.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

#ifndef TTF_FONT_DIR
#define TTF_FONT_DIR ""
#endif

#ifndef TTF_FONT_NAME
#define TTF_FONT_NAME "CharisSILB.ttf"
#endif

namespace dvl {

namespace {

std::string *basePath = NULL;
std::string *prefPath = NULL;
std::string *configPath = NULL;
std::string *ttfPath = NULL;
std::string *ttfName = NULL;

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
    std::string *result = new std::string(s != NULL ? s : "");
    if (s != NULL) {
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
	if (basePath == NULL)
		basePath = FromSDL(SDL_GetBasePath());
#endif
	return *basePath;
}

const std::string &GetPrefPath()
{
	if (prefPath == NULL)
		prefPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
	return *prefPath;
}

const std::string &GetConfigPath()
{
	if (configPath == NULL)
		configPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
	return *configPath;
}

const std::string &GetTtfPath()
{
	if (ttfPath == NULL)
		ttfPath = new std::string(TTF_FONT_DIR);
	return *ttfPath;
}

const std::string &GetTtfName()
{
	if (ttfName == NULL)
		ttfName = new std::string(TTF_FONT_NAME);
	return *ttfName;
}

void SetBasePath(const char *path)
{
#ifdef __ANDROID__
	basePath = new std::string("/sdcard/");
#endif
	if (basePath == NULL)
		basePath = new std::string;
	*basePath = path;
	AddTrailingSlash(basePath);
}

void SetPrefPath(const char *path)
{
	if (prefPath == NULL)
		prefPath = new std::string;
	*prefPath = path;
	AddTrailingSlash(prefPath);
}

void SetConfigPath(const char *path)
{
	if (configPath == NULL)
		configPath = new std::string;
	*configPath = path;
	AddTrailingSlash(configPath);
}

void SetTtfPath(const char *path)
{
	if (ttfPath == NULL)
		ttfPath = new std::string;
	*ttfPath = path;
	AddTrailingSlash(ttfPath);
}

void SetTtfName(const char *path)
{
	if (ttfName == NULL)
		ttfName = new std::string;
	*ttfName = path;
}

} // namespace dvl
