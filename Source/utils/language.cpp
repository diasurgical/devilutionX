#include "utils/language.h"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "engine/assets.hpp"
#include "options.h"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/stdcompat/string_view.hpp"

#define MO_MAGIC 0x950412de

namespace {

using namespace devilution;

std::unique_ptr<char[]> translationKeys;
std::unique_ptr<char[]> translationValues;

using TranslationRef = uint32_t;

struct StringHash {
	size_t operator()(const char *str) const noexcept
	{
		return std::hash<string_view> {}(str);
	}
};

struct StringEq {
	bool operator()(const char *lhs, const char *rhs) const noexcept
	{
		return string_view(lhs) == string_view(rhs);
	}
};

std::vector<std::unordered_map<const char *, TranslationRef, StringHash, StringEq>> translation = { {}, {} };

constexpr uint32_t TranslationRefOffsetBits = 19;
constexpr uint32_t TranslationRefSizeBits = 32 - TranslationRefOffsetBits; // 13
constexpr uint32_t TranslationRefSizeMask = (1 << TranslationRefSizeBits) - 1;

TranslationRef EncodeTranslationRef(uint32_t offset, uint32_t size)
{
	return (offset << TranslationRefSizeBits) | size;
}

string_view GetTranslation(TranslationRef ref)
{
	return { &translationValues[ref >> TranslationRefSizeBits], ref & TranslationRefSizeMask };
}

} // namespace

namespace {

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

void SwapLE(MoHead &head)
{
	head.magic = SDL_SwapLE32(head.magic);
	head.revision.major = SDL_SwapLE16(head.revision.major);
	head.revision.minor = SDL_SwapLE16(head.revision.minor);
	head.nbMappings = SDL_SwapLE32(head.nbMappings);
	head.srcOffset = SDL_SwapLE32(head.srcOffset);
	head.dstOffset = SDL_SwapLE32(head.dstOffset);
}

struct MoEntry {
	uint32_t length;
	uint32_t offset;
};

void SwapLE(MoEntry &entry)
{
	entry.length = SDL_SwapLE32(entry.length);
	entry.offset = SDL_SwapLE32(entry.offset);
}

string_view TrimLeft(string_view str)
{
	str.remove_prefix(std::min(str.find_first_not_of(" \t"), str.size()));
	return str;
}

string_view TrimRight(string_view str)
{
	str.remove_suffix(str.size() - (str.find_last_not_of(" \t") + 1));
	return str;
}

// English, Danish, Spanish, Italian, Swedish
unsigned PluralForms = 2;
std::function<int(int n)> GetLocalPluralId = [](int n) -> int { return n != 1 ? 1 : 0; };

/**
 * Match plural=(n != 1);"
 */
void SetPluralForm(string_view expression)
{
	const string_view key = "plural=";
	const string_view::size_type keyPos = expression.find(key);
	if (keyPos == string_view::npos)
		return;
	expression.remove_prefix(keyPos + key.size());

	const string_view::size_type semicolonPos = expression.find(';');
	if (semicolonPos != string_view::npos) {
		expression.remove_suffix(expression.size() - semicolonPos);
	}

	expression = TrimLeft(TrimRight(expression));

	// ko, zh_CN, zh_TW
	if (expression == "0") {
		GetLocalPluralId = [](int /*n*/) -> int { return 0; };
		return;
	}

	// en, bg, da, de, es, it, sv
	if (expression == "(n != 1)") {
		GetLocalPluralId = [](int n) -> int { return n != 1 ? 1 : 0; };
		return;
	}

	// fr, pt_BR
	if (expression == "(n > 1)") {
		GetLocalPluralId = [](int n) -> int { return n > 1 ? 1 : 0; };
		return;
	}

	// hr, ru
	if (expression == "(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<12 || n%100>14) ? 1 : 2)") {
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
	if (expression == "(n==1 ? 0 : n%10>=2 && n%10<=4 && (n%100<12 || n%100>14) ? 1 : 2)") {
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
	if (expression == "(n==1 ? 0 : n==0 || (n!=1 && n%100>=1 && n%100<=19) ? 1 : 2)") {
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
	if (expression == "(n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2") {
		GetLocalPluralId = [](int n) -> int {
			if (n == 1)
				return 0;
			if (n >= 2 && n <= 4)
				return 1;
			return 2;
		};
		return;
	}

	LogError("Unknown plural expression: '{}'", expression);
}

/**
 * Parse "nplurals=2;"
 */
void ParsePluralForms(string_view string)
{
	const string_view pluralsKey = "nplurals";
	const string_view::size_type pluralsPos = string.find(pluralsKey);
	if (pluralsPos == string_view::npos)
		return;
	string.remove_prefix(pluralsPos + pluralsKey.size());

	const string_view::size_type eqPos = string.find('=');
	if (eqPos == string_view::npos)
		return;

	string_view value = string.substr(eqPos + 1);
	if (value.empty() || value[0] < '0')
		return;

	const unsigned nplurals = value[0] - '0';
	if (nplurals == 0)
		return;

	PluralForms = nplurals;

	SetPluralForm(value);
}

void ParseMetadata(string_view metadata)
{
	string_view::size_type delim;

	while (!metadata.empty() && ((delim = metadata.find(':')) != string_view::npos)) {
		const string_view key = TrimLeft(string_view(metadata.data(), delim));
		string_view val = TrimLeft(string_view(metadata.data() + delim + 1, metadata.size() - delim - 1));

		if ((delim = val.find('\n')) != string_view::npos) {
			val = string_view(val.data(), delim);
			metadata.remove_prefix(val.data() - metadata.data() + val.size() + 1);
		} else {
			metadata.remove_prefix(metadata.size());
		}

		// Match "Plural-Forms: nplurals=2; plural=(n != 1);"
		if (key == "Plural-Forms") {
			ParsePluralForms(val);
			break;
		}
	}
}

bool ReadEntry(SDL_RWops *rw, const MoEntry &e, char *result)
{
	if (SDL_RWseek(rw, e.offset, RW_SEEK_SET) == -1)
		return false;
	result[e.length] = '\0';
	return static_cast<uint32_t>(SDL_RWread(rw, result, sizeof(char), e.length)) == e.length;
}

} // namespace

string_view LanguageParticularTranslate(string_view context, string_view message)
{
	constexpr const char Glue = '\004';

	std::string key = std::string(context);
	key.reserve(key.size() + 1 + message.size());
	key += Glue;
	AppendStrView(key, message);

	auto it = translation[0].find(key.c_str());
	if (it == translation[0].end()) {
		return message;
	}

	return GetTranslation(it->second);
}

string_view LanguagePluralTranslate(const char *singular, string_view plural, int count)
{
	int n = GetLocalPluralId(count);

	auto it = translation[n].find(singular);
	if (it == translation[n].end()) {
		if (count != 1)
			return plural;
		return singular;
	}

	return GetTranslation(it->second);
}

string_view LanguageTranslate(const char *key)
{
	auto it = translation[0].find(key);
	if (it == translation[0].end()) {
		return key;
	}

	return GetTranslation(it->second);
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
	translationKeys = nullptr;
	translationValues = nullptr;

	if (IsSmallFontTall() && !font_mpq) {
		UiErrorOkDialog(
		    "Missing fonts.mpq",
		    StrCat("fonts.mpq is required for locale \"",
		        *sgOptions.Language.code,
		        "\"\n\n"
		        "Please download fonts.mpq from:\n"
		        "github.com/diasurgical/\ndevilutionx-assets/releases"));
		sgOptions.Language.code = "en";
	}

	const std::string lang(*sgOptions.Language.code);
	SDL_RWops *rw;

	// Translations normally come in ".gmo" files.
	// We also support ".mo" because that is what poedit generates
	// and what translators use to test their work.
	for (const char *ext : { ".mo", ".gmo" }) {
		if ((rw = OpenAsset((lang + ext).c_str())) != nullptr) {
			break;
		}
	}
	if (rw == nullptr) {
		SetPluralForm("plural=(n != 1);"); // Reset to English plural form
		return;
	}

	// Read header and do sanity checks
	MoHead head;
	if (SDL_RWread(rw, &head, sizeof(MoHead), 1) != 1) {
		SDL_RWclose(rw);
		return;
	}
	SwapLE(head);

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
	if (static_cast<uint32_t>(SDL_RWread(rw, src.get(), sizeof(MoEntry), head.nbMappings)) != head.nbMappings) {
		SDL_RWclose(rw);
		return;
	}
	for (size_t i = 0; i < head.nbMappings; ++i) {
		SwapLE(src[i]);
	}

	// Read entries of target strings
	std::unique_ptr<MoEntry[]> dst { new MoEntry[head.nbMappings] };
	if (SDL_RWseek(rw, head.dstOffset, RW_SEEK_SET) == -1) {
		SDL_RWclose(rw);
		return;
	}
	if (static_cast<uint32_t>(SDL_RWread(rw, dst.get(), sizeof(MoEntry), head.nbMappings)) != head.nbMappings) {
		SDL_RWclose(rw);
		return;
	}
	for (size_t i = 0; i < head.nbMappings; ++i) {
		SwapLE(dst[i]);
	}

	// MO header
	if (src[0].length != 0) {
		SDL_RWclose(rw);
		return;
	}
	{
		auto headerValue = std::unique_ptr<char[]> { new char[dst[0].length + 1] };
		if (!ReadEntry(rw, dst[0], &headerValue[0])) {
			SDL_RWclose(rw);
			return;
		}
		ParseMetadata(&headerValue[0]);
	}

	translation.resize(PluralForms);
	for (unsigned i = 0; i < PluralForms; i++)
		translation[i] = {};

	// Read strings described by entries
	size_t keysSize = 0;
	size_t valuesSize = 0;
	for (uint32_t i = 1; i < head.nbMappings; i++) {
		keysSize += src[i].length + 1;
		valuesSize += dst[i].length + 1;
	}
	translationKeys = std::unique_ptr<char[]> { new char[keysSize] };
	translationValues = std::unique_ptr<char[]> { new char[valuesSize] };

	char *keyPtr = &translationKeys[0];
	char *valuePtr = &translationValues[0];
	for (uint32_t i = 1; i < head.nbMappings; i++) {
		if (ReadEntry(rw, src[i], keyPtr) && ReadEntry(rw, dst[i], valuePtr)) {
			// Plural keys also have a plural form but it does not participate in lookup.
			// Plural values are \0-terminated.
			string_view value { valuePtr, dst[i].length + 1 };
			for (size_t j = 0; j < PluralForms && !value.empty(); j++) {
				const size_t formValueEnd = value.find('\0');
				translation[j].emplace(keyPtr, EncodeTranslationRef(value.data() - &translationValues[0], formValueEnd));
				value.remove_prefix(formValueEnd + 1);
			}

			keyPtr += src[i].length + 1;
			valuePtr += dst[i].length + 1;
		}
	}

	SDL_RWclose(rw);
}
