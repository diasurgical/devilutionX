/*
Copyright (c) 2021 Cong

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _MSC_VER
#define _FSG_FUNC static __inline
#elif !defined __STDC_VERSION__ || __STDC_VERSION__ < 199901L
#define _FSG_FUNC static __inline__
#else
#define _FSG_FUNC static inline
#endif

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winerror.h>
#include <winreg.h>

	_FSG_FUNC
	void _fsg_query_reg_key(
		char *out, const HKEY key, const char *keypath, const char *valname)
	{
		HKEY pathkey;
		DWORD pathtype;
		DWORD pathlen;
		LONG res;

		if (ERROR_SUCCESS ==
			RegOpenKeyEx(key, keypath, 0, KEY_QUERY_VALUE, &pathkey))
		{
			if (ERROR_SUCCESS ==
					RegQueryValueEx(
						pathkey, valname, 0, &pathtype, NULL, &pathlen) &&
				pathtype == REG_SZ && pathlen != 0)
			{
				res = RegQueryValueEx(
					pathkey, valname, 0, NULL, (LPBYTE)out, &pathlen);
				if (res != ERROR_SUCCESS)
				{
					out[0] = '\0';
				}
			}
			RegCloseKey(pathkey);
		}
	}
#else

#include <pwd.h>
#include <unistd.h>

#endif

#if (defined _MSC_VER || defined __MINGW32__)
#define _FSG_PATH_MAX MAX_PATH
#elif defined __linux__
#include <limits.h>
#ifdef PATH_MAX
#define _FSG_PATH_MAX PATH_MAX
#endif
#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/param.h>
#if defined(BSD)
#include <limits.h>
#ifdef PATH_MAX
#define _FSG_PATH_MAX PATH_MAX
#endif
#endif
#endif

#ifndef _FSG_PATH_MAX
#define _FSG_PATH_MAX 4096
#endif

	_FSG_FUNC
	bool _fsg_dir_exists(const char *dir)
	{
		struct stat info;
		return stat(dir, &info) == 0 && (info.st_mode & S_IFDIR);
	}

	_FSG_FUNC
	void fsg_get_steam_game_path(char *out, const char *name)
	{
		out[0] = '\0';
#ifdef _MSC_VER
		char steam_path[_FSG_PATH_MAX];
		char buf[_FSG_PATH_MAX];
		_fsg_query_reg_key(
			steam_path, HKEY_CURRENT_USER, "Software\\Valve\\Steam",
			"SteamPath");
		if (strlen(steam_path) == 0)
		{
			_fsg_query_reg_key(
				steam_path, HKEY_LOCAL_MACHINE, "Software\\Valve\\Steam",
				"InstallPath");
			if (strlen(steam_path) == 0)
				return;
		}
		strcat(steam_path, "\\steamapps\\");

		// Check for game in common
		sprintf(out, "%scommon\\%s", steam_path, name);

		if (_fsg_dir_exists(out))
		{
			return;
		}
		out[0] = '\0';

		// Try reading library paths described by libraryfolders.vdf
		sprintf(buf, "%s\\libraryfolders.vdf", steam_path);
		FILE *f = fopen(buf, "r");
		if (f)
		{
			char line_buf[256];
			while (fgets(line_buf, 256, f))
			{
				// Look for a line with "path"		"<library path>"
				const char *path_p = strstr(line_buf, "\"path\"");
				if (path_p == NULL)
				{
					continue;
				}
				const char *value_start =
					strchr(path_p + strlen("\"path\"") + 1, '"');
				if (value_start == NULL)
				{
					continue;
				}
				value_start++;
				const char *value_end = strchr(value_start, '"');
				const char *value_p = value_start;
				char *out_p = out;
				while (value_p < value_end)
				{
					// Copy value to output, skipping double backslashes
					*out_p++ = *value_p++;
					if (*value_p == '\\' && value_p > value_start &&
						*(value_p - 1) == '\\')
					{
						out_p--;
					}
				}
				*out_p = '\0';
				strcat(out, "\\steamapps\\common\\");
				strcat(out, name);
				if (_fsg_dir_exists(out))
				{
					fclose(f);
					return;
				}
			}
			fclose(f);
			out[0] = '\0';
		}
#else
		// Look at $HOME/.local/share/Steam/steamapps/common/
		struct passwd *pw = getpwuid(getuid());
		const char *homedir = pw->pw_dir;
		sprintf(out, "%s/.local/share/Steam/steamapps/common/%s", homedir, name);
		if (_fsg_dir_exists(out))
		{
			return;
		}

		// Try reading library paths described by libraryfolders.vdf
		// TODO: steam installed at different location
		char buf[_FSG_PATH_MAX];
		const int ret = snprintf(
			buf, _FSG_PATH_MAX,
			"%s/.local/share/Steam/steamapps/libraryfolders.vdf", homedir);
		if (ret >= 0)
		{
			FILE *f = fopen(buf, "r");
			if (f)
			{
				char line_buf[256];
				while (fgets(line_buf, 256, f))
				{
					// Look for a line with "path"		"<library path>"
					const char *path_p = strstr(line_buf, "\"path\"");
					if (path_p == NULL)
					{
						continue;
					}
					const char *value_start =
						strchr(path_p + strlen("\"path\"") + 1, '"');
					if (value_start == NULL)
					{
						continue;
					}
					value_start++;
					const char *value_end = strchr(value_start, '"');
					const char *value_p = value_start;
					char *out_p = out;
					while (value_p < value_end)
					{
						// Copy value to output
						*out_p++ = *value_p++;
					}
					*out_p = '\0';
					strcat(out, "/steamapps/common/");
					strcat(out, name);
					if (_fsg_dir_exists(out))
					{
						fclose(f);
						return;
					}
				}
				fclose(f);
			}
		}
#endif

		out[0] = '\0';
	}

	_FSG_FUNC
	void fsg_get_gog_game_path(char *out, const char *app_id)
	{
		out[0] = '\0';
#ifdef _MSC_VER
		char buf[_FSG_PATH_MAX];
		sprintf(buf, "Software\\Wow6432Node\\GOG.com\\Games\\%s", app_id);
		_fsg_query_reg_key(out, HKEY_LOCAL_MACHINE, buf, "Path");
#endif
		(void)app_id;
	}

#ifdef __cplusplus
}
#endif