#pragma once

#define _(x) LanguageTranslate(x)
#define _F(x) (x)

void LanguageInitialize();
const char* LanguageTranslate(const char* key);

