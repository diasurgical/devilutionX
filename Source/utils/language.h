#pragma once

#include <string>

#define _(x) LanguageTranslate(x).c_str()
#define N_(x) (x)

void LanguageInitialize();
const std::string &LanguageTranslate(const char* key);
const char *LanguageMetadata(const char *key);
