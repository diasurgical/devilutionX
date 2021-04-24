#pragma once

#define _(x) LanguageTranslate(x)

void LanguageInitialize();
const char* LanguageTranslate(const char* key);

