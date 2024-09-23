#pragma once

#include <string>
#include <string_view>

#define _(x) LanguageTranslate(x)
#define ngettext(x, y, z) LanguagePluralTranslate(x, y, z)
#define pgettext(context, x) LanguageParticularTranslate(context, x)
#define N_(x) (x)
#define P_(context, x) (x)

extern std::string forceLocale;

std::string_view GetLanguageCode();

bool HasTranslation(const std::string &locale);
void LanguageInitialize();

/**
 * @brief Returns the translation for the given key.
 *
 * @return guaranteed to be null-terminated.
 */
std::string_view LanguageTranslate(const char *key);
inline std::string_view LanguageTranslate(const std::string &key)
{
	return LanguageTranslate(key.c_str());
}

/**
 * @brief Returns a singular or plural translation for the given keys and count.
 *
 * @return guaranteed to be null-terminated if `plural` is.
 */
std::string_view LanguagePluralTranslate(const char *singular, std::string_view plural, int count);

/**
 * @brief Returns the translation for the given key and context identifier.
 *
 * @return guaranteed to be null-terminated.
 */
std::string_view LanguageParticularTranslate(std::string_view context, std::string_view message);

// Chinese and Japanese, and Korean small font is 16px instead of a 12px one for readability.
inline bool IsSmallFontTall()
{
	const std::string_view code = GetLanguageCode().substr(0, 2);
	return code == "zh" || code == "ja" || code == "ko";
}
