#include "locale.hpp"

#ifdef __ANDROID__
#include "SDL.h"
#include <jni.h>
#elif defined(__vita__)
#include <cstring>

#include <psp2/apputil.h>
#include <psp2/system_param.h>
#elif defined(__3DS__)
#include "platform/ctr/locale.hpp"
#elif defined(_WIN32)
// clang-format off
#include <windows.h>
#include <winnls.h>
// clang-format on

#include "utils/stdcompat/algorithm.hpp"
#else
#include <locale>
#endif

namespace devilution {

std::vector<std::string> GetLocales()
{
	std::vector<std::string> locales {};
#ifdef __ANDROID__
	JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();
	jobject activity = (jobject)SDL_AndroidGetActivity();
	jclass clazz(env->GetObjectClass(activity));
	jmethodID method_id = env->GetMethodID(clazz, "getLocale", "()Ljava/lang/String;");
	jstring jLocale = (jstring)env->CallObjectMethod(activity, method_id);
	const char *cLocale = env->GetStringUTFChars(jLocale, nullptr);
	locales.emplace_back(cLocale);
	env->ReleaseStringUTFChars(jLocale, cLocale);
	env->DeleteLocalRef(jLocale);
	env->DeleteLocalRef(activity);
	env->DeleteLocalRef(clazz);
#elif defined(__vita__)
	int32_t language = SCE_SYSTEM_PARAM_LANG_ENGLISH_US; // default to english
	const char *vita_locales[] = {
		"ja_JP",
		"en_US",
		"fr_FR",
		"es_ES",
		"de_DE",
		"it_IT",
		"nl_NL",
		"pt_PT",
		"ru_RU",
		"ko_KR",
		"zh_TW",
		"zh_CN",
		"fi_FI",
		"sv_SE",
		"da_DK",
		"no_NO",
		"pl_PL",
		"pt_BR",
		"en_GB",
		"tr_TR",
	};
	SceAppUtilInitParam initParam;
	SceAppUtilBootParam bootParam;
	memset(&initParam, 0, sizeof(SceAppUtilInitParam));
	memset(&bootParam, 0, sizeof(SceAppUtilBootParam));
	sceAppUtilInit(&initParam, &bootParam);
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &language);
	if (language < 0 || language > SCE_SYSTEM_PARAM_LANG_TURKISH)
		language = SCE_SYSTEM_PARAM_LANG_ENGLISH_US; // default to english
	locales.emplace_back(vita_locales[language]);
	sceAppUtilShutdown();
#elif defined(__3DS__)
	locales.push_back(n3ds::GetLocale());
#elif defined(_WIN32)
#if WINVER >= 0x0600
	auto wideCharToUtf8 = [](PWSTR wideString) {
		char utf8Buffer[12] {};
		// We only handle 5 character locales (lang2-region2), so don't bother reading past that. This does leave the
		//  resulting string unterminated but the buffer was zero initialised anyway.
		WideCharToMultiByte(CP_UTF8, 0, wideString, 5, utf8Buffer, sizeof(utf8Buffer), nullptr, nullptr);

		// GetUserDefaultLocaleName could return an ISO 639-2/T string (three letter language code) or even an
		//  arbitrary custom locale, however we only handle 639-1 (two letter language code) locale names when checking
		//  the fallback language.
		// if a region is included in the locale do a simple transformation to the expected POSIX style.
		std::replace(utf8Buffer, utf8Buffer + sizeof(utf8Buffer), '-', '_');
		return std::move(std::string(utf8Buffer));
	};
	WCHAR localeBuffer[LOCALE_NAME_MAX_LENGTH];

	if (GetUserDefaultLocaleName(localeBuffer, LOCALE_NAME_MAX_LENGTH) != 0) {
		// Found a user default locale convert from WIN32's default of UTF16 to UTF8 and add to the list
		locales.push_back(wideCharToUtf8(localeBuffer));
	}

	ULONG numberOfLanguages;
	ULONG bufferSize = LOCALE_NAME_MAX_LENGTH;
	GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numberOfLanguages, localeBuffer, &bufferSize);

	PWSTR bufferOffset = localeBuffer;
	for (unsigned i = 0; i < numberOfLanguages; i++) {
		// Found one (or more) user preferred UI language(s), add these to the list to check as well
		locales.push_back(wideCharToUtf8(bufferOffset));

		// GetUserPreferredUILanguages returns a null separated list of strings, need to increment past the null terminating
		//  the current string.
		bufferOffset += lstrlenW(localeBuffer) + 1;
	}
#else
	// Fallback method for older versions of windows, this is deprecated since Vista
	char localeBuffer[LOCALE_NAME_MAX_LENGTH];
	// Deliberately not using the unicode versions here as the information retrieved should be represented in ASCII/single
	//  byte UTF8 codepoints.
	if (GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, localeBuffer, LOCALE_NAME_MAX_LENGTH) != 0) {
		std::string locale { localeBuffer };
		if (GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, localeBuffer, LOCALE_NAME_MAX_LENGTH) != 0) {
			locale.append("_");
			locale.append(localeBuffer);
		}
		locales.push_back(std::move(locale));
	}
#endif
#else
	locales.emplace_back(std::locale("").name().substr(0, 5));
#endif
	return locales;
}

} // namespace devilution
