#pragma once

#include <string>

#define _(x) LanguageTranslate(x).c_str()
#define ngettext(x, y, z) LanguagePluralTranslate(x, y, z).c_str()
#define pgettext(context, x) LanguageParticularTranslate(context, x).c_str()
#define N_(x) (x)
#define P_(context, x) (x)

bool HasTranslation(const std::string &locale);
void LanguageInitialize();
const std::string &LanguageParticularTranslate(const char *context, const char *message);
const std::string &LanguagePluralTranslate(const char *singular, const char *plural, int count);
const std::string &LanguageTranslate(const char *key);
const char *LanguageMetadata(const char *key);
