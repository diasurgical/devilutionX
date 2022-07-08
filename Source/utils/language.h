#pragma once

#include <string>

#include "utils/stdcompat/string_view.hpp"

#define _(x) LanguageTranslate(x)
#define ngettext(x, y, z) LanguagePluralTranslate(x, y, z)
#define pgettext(context, x) LanguageParticularTranslate(context, x)
#define N_(x) (x)
#define P_(context, x) (x)

bool HasTranslation(const std::string &locale);
void LanguageInitialize();

/**
 * @brief Returns the translation for the given key.
 *
 * @return guaranteed to be null-terminated.
 */
devilution::string_view LanguageTranslate(const char *key);
inline devilution::string_view LanguageTranslate(const std::string &key)
{
	return LanguageTranslate(key.c_str());
}

/**
 * @brief Returns a singular or plural translation for the given keys and count.
 *
 * @return guaranteed to be null-terminated if `plural` is.
 */
devilution::string_view LanguagePluralTranslate(const char *singular, devilution::string_view plural, int count);

/**
 * @brief Returns the translation for the given key and context identifier.
 *
 * @return guaranteed to be null-terminated.
 */
devilution::string_view LanguageParticularTranslate(devilution::string_view context, devilution::string_view message);

// Chinese and Japanese, and Korean small font is 16px instead of a 12px one for readability.
bool IsSmallFontTall();
