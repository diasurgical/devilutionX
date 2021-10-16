#include "utils/language.h"

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "options.h"
#include "utils/file_util.h"
#include "utils/paths.h"

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
std::map<const char *, const char *, CStringCmp> meta;

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

	// ko_KR, zh_CN, zh_TW
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
		meta[key] = val;

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
	return (SDL_RWread(rw, result.data(), sizeof(char), e->length) == e->length);
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

const char *LanguageMetadata(const char *key)
{
	auto it = meta.find(key);
	if (it == meta.end()) {
		return nullptr;
	}

	return it->second;
}

bool HasTranslation(const std::string &locale)
{
	std::string moPath = paths::LangPath() + locale + ".mo";
	if (FileExists(moPath.c_str()))
		return true;

	std::string gmoPath = paths::LangPath() + locale + ".gmo";
	return FileExists(gmoPath.c_str());
}

void LanguageInitialize()
{
	SDL_RWops *rw;

	auto path = paths::LangPath() + sgOptions.Language.szCode + ".mo";
	if ((rw = SDL_RWFromFile(path.c_str(), "rb")) == nullptr) {
		path = paths::LangPath() + sgOptions.Language.szCode + ".gmo";
		if ((rw = SDL_RWFromFile(path.c_str(), "rb")) == nullptr) {
			perror(path.c_str());
			return;
		}
	}
	// Read header and do sanity checks
	// FIXME: Endianness.
	MoHead head;
	if (SDL_RWread(rw, &head, sizeof(MoHead), 1) != 1) {
		return;
	}

	if (head.magic != MO_MAGIC) {
		return; // not a MO file
	}

	if (head.revision.major > 1 || head.revision.minor > 1) {
		return; // unsupported revision
	}

	// Read entries of source strings
	std::unique_ptr<MoEntry[]> src { new MoEntry[head.nbMappings] };
	if (SDL_RWseek(rw, head.srcOffset, RW_SEEK_SET) == -1)
		return;
	// FIXME: Endianness.
	if (SDL_RWread(rw, src.get(), sizeof(MoEntry), head.nbMappings) != head.nbMappings)
		return;

	// Read entries of target strings
	std::unique_ptr<MoEntry[]> dst { new MoEntry[head.nbMappings] };
	if (SDL_RWseek(rw, head.dstOffset, RW_SEEK_SET) == -1)
		return;
	// FIXME: Endianness.
	if (SDL_RWread(rw, dst.get(), sizeof(MoEntry), head.nbMappings) != head.nbMappings)
		return;

	std::vector<char> key;
	std::vector<char> value;

	// MO header
	if (!ReadEntry(rw, &src[0], key) && ReadEntry(rw, &dst[0], value))
		return;

	if (key[0] != '\0')
		return;

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
}
