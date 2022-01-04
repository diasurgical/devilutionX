#include "utils/language.h"

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "engine/assets.hpp"
#include "options.h"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/stdcompat/string_view.hpp"

using namespace devilution;
#define MO_MAGIC 0x950412de

namespace {

struct CStringCmp {
	bool operator()(const char *s1, const char *s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

std::vector<std::map<std::string, std::string, std::less<>>> translation = { {}, {} };

struct MoHead {
	uint32_t magic;
	struct {
		uint16_t major;
		uint16_t minor;
	} revision;

	uint32_t nbMappings;
	uint32_t srcOffset;
	uint32_t dstOffset;
};

struct MoEntry {
	uint32_t length;
	uint32_t offset;
};

char *StrTrimLeft(char *s)
{
	while (*s != '\0' && isblank(*s) != 0) {
		s++;
	}
	return s;
}

char *StrTrimRight(char *s)
{
	size_t length = strlen(s);

	while (length != 0) {
		length--;
		if (isblank(s[length]) != 0) {
			s[length] = '\0';
		} else {
			break;
		}
	}
	return s;
}

// English, Danish, Spanish, Italian, Swedish
int PluralForms = 2;
std::function<int(int n)> GetLocalPluralId = [](int n) -> int { return n != 1 ? 1 : 0; };

/**
 * Match plural=(n != 1);"
 */
void SetPluralForm(char *string)
{
	char *expression = strstr(string, "plural");
	if (expression == nullptr)
		return;

	expression = strstr(expression, "=");
	if (expression == nullptr)
		return;
	expression += 1;

	for (unsigned i = 0; i < strlen(expression); i++) {
		if (expression[i] == ';') {
			expression[i] = '\0';
			break;
		}
	}

	expression = StrTrimRight(expression);
	expression = StrTrimLeft(expression);

	// ko, zh_CN, zh_TW
	if (strcmp(expression, "0") == 0) {
		GetLocalPluralId = [](int /*n*/) -> int { return 0; };
		return;
	}

	// fr, pt_BR
	if (strcmp(expression, "(n > 1)") == 0) {
		GetLocalPluralId = [](int n) -> int { return n > 1 ? 1 : 0; };
		return;
	}

	// hr, ru
	if (strcmp(expression, "(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<12 || n%100>14) ? 1 : 2)") == 0) {
		GetLocalPluralId = [](int n) -> int {
			if (n % 10 == 1 && n % 100 != 11)
				return 0;
			if (n % 10 >= 2 && n % 10 <= 4 && (n % 100 < 12 || n % 100 > 14))
				return 1;
			return 2;
		};
		return;
	}

	// pl
	if (strcmp(expression, "(n==1 ? 0 : n%10>=2 && n%10<=4 && (n%100<12 || n%100>14) ? 1 : 2)") == 0) {
		GetLocalPluralId = [](int n) -> int {
			if (n == 1)
				return 0;
			if (n % 10 >= 2 && n % 10 <= 4 && (n % 100 < 12 || n % 100 > 14))
				return 1;
			return 2;
		};
		return;
	}

	// ro
	if (strcmp(expression, "(n==1 ? 0 : n==0 || (n!=1 && n%100>=1 && n%100<=19) ? 1 : 2)") == 0) {
		GetLocalPluralId = [](int n) -> int {
			if (n == 1)
				return 0;
			if (n == 0 || (n != 1 && n % 100 >= 1 && n % 100 <= 19))
				return 1;
			return 2;
		};
		return;
	}

	// cs
	if (strcmp(expression, "(n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2") == 0) {
		GetLocalPluralId = [](int n) -> int {
			if (n == 1)
				return 0;
			if (n >= 2 && n <= 4)
				return 1;
			return 2;
		};
		return;
	}

	// bg, da, de, es, it, sv
	// (n != 1)
}

/**
 * Parse "nplurals=2;"
 */
void ParsePluralForms(char *string)
{
	char *value = strstr(string, "nplurals");
	if (value == nullptr)
		return;

	value = strstr(value, "=");
	if (value == nullptr)
		return;

	value += 1;

	int nplurals = SDL_atoi(value);
	if (nplurals == 0)
		return;

	PluralForms = nplurals;

	SetPluralForm(value);
}

void ParseMetadata(char *ptr)
{
	char *delim;

	while ((ptr != nullptr) && ((delim = strstr(ptr, ":")) != nullptr)) {
		char *key = StrTrimLeft(ptr);
		char *val = StrTrimLeft(delim + 1);

		// null-terminate key
		*delim = '\0';

		// progress to next line (if any)
		if ((ptr = strstr(val, "\n")) != nullptr) {
			*ptr = '\0';
			ptr++;
		}

		val = StrTrimRight(val);

		if ((strcmp("Content-Type", key) == 0) && ((delim = strstr(val, "=")) != nullptr)) {
			if (strcasecmp(delim + 1, "utf-8") != 0) {
				Log("Translation is now UTF-8 encoded!");
			}
			continue;
		}

		// Match "Plural-Forms: nplurals=2; plural=(n != 1);"
		if (strcmp("Plural-Forms", key) == 0) {
			ParsePluralForms(val);
			continue;
		}
	}
}

bool ReadEntry(SDL_RWops *rw, MoEntry *e, std::vector<char> &result)
{
	if (SDL_RWseek(rw, e->offset, RW_SEEK_SET) == -1)
		return false;
	result.resize(e->length + 1);
	result.back() = '\0';
	return static_cast<uint32_t>(SDL_RWread(rw, result.data(), sizeof(char), e->length)) == e->length;
}

} // namespace

const std::string &LanguageParticularTranslate(const char *context, const char *message)
{
	constexpr const char *glue = "\004";

	std::string key = context;
	key += glue;
	key += message;

	auto it = translation[0].find(key);
	if (it == translation[0].end()) {
		it = translation[0].insert({ key, message }).first;
	}

	return it->second;
}

const std::string &LanguagePluralTranslate(const char *singular, const char *plural, int count)
{
	int n = GetLocalPluralId(count);

	auto it = translation[n].find(singular);
	if (it == translation[n].end()) {
		if (count != 1)
			it = translation[1].insert({ singular, plural }).first;
		else
			it = translation[0].insert({ singular, singular }).first;
	}

	return it->second;
}

const std::string &LanguageTranslate(const char *key)
{
	auto it = translation[0].find(key);
	if (it == translation[0].end()) {
		it = translation[0].insert({ key, key }).first;
	}

	return it->second;
}

bool HasTranslation(const std::string &locale)
{
	if (locale == "en") {
		// the base translation is en (really en_US). No translation file will be present for this code but we want
		//  the check to succeed to prevent further searches.
		return true;
	}

	constexpr std::array<const char *, 2> Extensions { ".mo", ".gmo" };
	return std::any_of(Extensions.cbegin(), Extensions.cend(), [locale](const std::string &extension) {
		SDL_RWops *rw = OpenAsset((locale + extension).c_str());
		if (rw != nullptr) {
			SDL_RWclose(rw);
			return true;
		}
		return false;
	});
}

bool IsSmallFontTall()
{
	string_view code = (*sgOptions.Language.code).substr(0, 2);
	return code == "zh" || code == "ja" || code == "ko";
}

void LanguageInitialize()
{
	translation = { {}, {} };

	const std::string lang(*sgOptions.Language.code);
	SDL_RWops *rw;

	// Translations normally come in ".gmo" files.
	// We also support ".mo" because that is what poedit generates
	// and what translators use to test their work.
	for (const char *ext : { ".mo", ".gmo" }) {
		if ((rw = OpenAsset((lang + ext).c_str())) != nullptr)
			break;
	}
	if (rw == nullptr)
		return;

	// Read header and do sanity checks
	// FIXME: Endianness.
	MoHead head;
	if (SDL_RWread(rw, &head, sizeof(MoHead), 1) != 1) {
		SDL_RWclose(rw);
		return;
	}

	if (head.magic != MO_MAGIC) {
		SDL_RWclose(rw);
		return; // not a MO file
	}

	if (head.revision.major > 1 || head.revision.minor > 1) {
		SDL_RWclose(rw);
		return; // unsupported revision
	}

	// Read entries of source strings
	std::unique_ptr<MoEntry[]> src { new MoEntry[head.nbMappings] };
	if (SDL_RWseek(rw, head.srcOffset, RW_SEEK_SET) == -1) {
		SDL_RWclose(rw);
		return;
	}
	// FIXME: Endianness.
	if (static_cast<uint32_t>(SDL_RWread(rw, src.get(), sizeof(MoEntry), head.nbMappings)) != head.nbMappings) {
		SDL_RWclose(rw);
		return;
	}

	// Read entries of target strings
	std::unique_ptr<MoEntry[]> dst { new MoEntry[head.nbMappings] };
	if (SDL_RWseek(rw, head.dstOffset, RW_SEEK_SET) == -1) {
		SDL_RWclose(rw);
		return;
	}
	// FIXME: Endianness.
	if (static_cast<uint32_t>(SDL_RWread(rw, dst.get(), sizeof(MoEntry), head.nbMappings)) != head.nbMappings) {
		SDL_RWclose(rw);
		return;
	}

	std::vector<char> key;
	std::vector<char> value;

	// MO header
	if (!ReadEntry(rw, &src[0], key) || !ReadEntry(rw, &dst[0], value)) {
		SDL_RWclose(rw);
		return;
	}

	if (key[0] != '\0') {
		SDL_RWclose(rw);
		return;
	}

	ParseMetadata(value.data());

	translation.resize(PluralForms);
	for (int i = 0; i < PluralForms; i++)
		translation[i] = {};

	// Read strings described by entries
	for (uint32_t i = 1; i < head.nbMappings; i++) {
		if (ReadEntry(rw, &src[i], key) && ReadEntry(rw, &dst[i], value)) {
			size_t offset = 0;
			for (int j = 0; j < PluralForms; j++) {
				const char *text = value.data() + offset;
				translation[j].emplace(key.data(), text);

				if (dst[i].length <= offset + strlen(value.data()))
					break;

				offset += strlen(text) + 1;
			}
		}
	}

	SDL_RWclose(rw);
}
