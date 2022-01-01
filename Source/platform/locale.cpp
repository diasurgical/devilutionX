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
#elif defined(__APPLE__) and defined(USE_COREFOUNDATION)
#include <CoreFoundation/CoreFoundation.h>
#else
#include <locale>
#endif

#include "utils/stdcompat/algorithm.hpp"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {
namespace {

std::string IetfToPosix(string_view langCode)
{
	/*
	 * Handle special case for simplified/traditional Chinese. IETF/BCP-47 specifies that only the script should be
	 *  used to discriminate languages when the region doesn't add additional value. For chinese scripts zh-Hans is
	 *  preferred over zh-Hans-CN (but platforms may include the region identifier anyway). POSIX locales don't use
	 *  script in the same way so we need to convert these back to lang_region.
	 */
	if (langCode.substr(0, 7) == "zh-Hans") {
		return "zh_CN";
	}
	if (langCode.substr(0, 7) == "zh-Hant") {
		return "zh_TW";
	}

	std::string posixLangCode { langCode };

	// if a region is included in the locale do a simple transformation to the expected POSIX style.
	std::replace(posixLangCode.begin(), posixLangCode.end(), '-', '_');

	return posixLangCode;
}

} // namespace

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
		// WideCharToMultiByte potentially leaves the buffer unterminated, default initialise here as a workaround
		char utf8Buffer[16] {};

		// Fetching up to 10 characters to allow for script tags
		WideCharToMultiByte(CP_UTF8, 0, wideString, 10, utf8Buffer, sizeof(utf8Buffer), nullptr, nullptr);

		// Windows NLS functions return IETF/BCP-47 locale codes (or potentially arbitrary custom locales) but devX
		//  uses posix format codes, return the expected format
		return std::move(IetfToPosix(utf8Buffer));
	};

	WCHAR localeBuffer[LOCALE_NAME_MAX_LENGTH];

	if (GetUserDefaultLocaleName(localeBuffer, sizeof(localeBuffer)) != 0) {
		// Found a user default locale convert from WIN32's default of UTF16 to UTF8 and add to the list
		locales.push_back(wideCharToUtf8(localeBuffer));
	}

	ULONG numberOfLanguages;
	ULONG bufferSize = sizeof(localeBuffer);
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
#elif defined(__APPLE__) and defined(USE_COREFOUNDATION)
	// Get the user's language list (in order of preference)
	CFArrayRef languages = CFLocaleCopyPreferredLanguages();
	CFIndex numLanguages = CFArrayGetCount(languages);

	for (CFIndex i = 0; i < numLanguages; i++) {
		auto language = static_cast<CFStringRef>(CFArrayGetValueAtIndex(languages, i));

		char buffer[16];

		if (CFStringGetCString(language, buffer, sizeof(buffer), kCFStringEncodingUTF8)) {
			// Convert to the posix format expected by callers
			locales.push_back(IetfToPosix(buffer));
		}
	}

	CFRelease(languages);
#else
	std::string locale = std::locale("").name();
	// strip off any encoding specifier, devX uses UTF8.
	locales.emplace_back(locale.substr(0, locale.find('.')));
#endif
	return locales;
}

} // namespace devilution
