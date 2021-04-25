#pragma once

#define _(x) LanguageTranslate(x)
#define N_(x) (x)

void LanguageInitialize();
const char* LanguageTranslate(const char* key);

