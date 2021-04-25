#include "options.h"
#include "utils/paths.h"
#include "utils/utf8.h"
#include <map>

using namespace devilution;
#define MO_MAGIC 0x950412de

namespace {

struct cstring_cmp {
	bool operator()(const char *s1, const char *s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

std::map<const char *, const char *, cstring_cmp> map;
std::map<const char *, const char *, cstring_cmp> meta;

struct mo_head {
	uint32_t magic;
	struct {
		uint16_t major;
		uint16_t minor;
	} revision;

	uint32_t nb_mappings;
	uint32_t src_offset;
	uint32_t dst_offset;
};

struct mo_entry {
	uint32_t length;
	uint32_t offset;
};

char *strtrim_left(char *s)
{
	while (*s && isblank(*s)) {
		s++;
	}
	return s;
}

char *strtrim_right(char *s)
{
	size_t length = strlen(s);

	while (length) {
		length--;
		if (isblank(s[length])) {
			s[length] = '\0';
		} else {
			break;
		}
	}
	return s;
}

bool parse_metadata(char *data)
{
	char *key, *delim, *val;
	char *ptr = data;
	bool utf8 = false;

	while (ptr && (delim = strstr(ptr, ":"))) {
		key = strtrim_left(ptr);
		val = strtrim_left(delim + 1);

		// null-terminate key
		*delim = '\0';

		// progress to next line (if any)
		if ((ptr = strstr(val, "\n"))) {
			*ptr = '\0';
			ptr++;
		}

		val = strtrim_right(val);
		meta[key] = val;

		// Match "Content-Type: text/plain; charset=UTF-8"
		if (!strcmp("Content-Type", key) && (delim = strstr(val, "="))) {
			utf8 = !strcasecmp(delim + 1, "utf-8");
		}
	}

	return utf8;
}

char *read_entry(FILE *fp, mo_entry *e)
{
	void *data;

	if (fseek(fp, e->offset, SEEK_SET)) {
		return nullptr;
	}

	if (!(data = calloc(e->length + 1, sizeof(char)))) {
		return nullptr;
	}

	if (fread(data, sizeof(char), e->length, fp) != e->length) {
		free(data);
		return nullptr;
	}

	return static_cast<char *>(data);
}
} // namespace

const char *LanguageTranslate(const char *key)
{
	auto it = map.find(key);
	if (it == map.end()) {
		return key;
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

void LanguageInitialize()
{
	mo_entry *src, *dst;
	mo_head head;
	FILE *fp;
	bool utf8;

	auto path = paths::LangPath() + "./" + sgOptions.Language.szCode + ".gmo";
	if (!(fp = fopen(path.c_str(), "rb"))) {
		path = paths::LangPath() + "./" + sgOptions.Language.szCode + ".mo";
		if (!(fp = fopen(path.c_str(), "rb"))) {
			perror(path.c_str());
			return;
		}
	}
	// Read header and do sanity checks
	if (fread(&head, sizeof(mo_head), 1, fp) != 1) {
		return;
	}

	if (head.magic != MO_MAGIC) {
		return; // not a MO file
	}

	if (head.revision.major > 1 || head.revision.minor > 1) {
		return; // unsupported revision
	}

	// Read entries of source strings
	src = new mo_entry[head.nb_mappings];
	if (fseek(fp, head.src_offset, SEEK_SET)) {
		delete[] src;
		return;
	}
	if (fread(src, sizeof(mo_entry), head.nb_mappings, fp) != head.nb_mappings) {
		delete[] src;
		return;
	}

	// Read entries of target strings
	dst = new mo_entry[head.nb_mappings];
	if (fseek(fp, head.dst_offset, SEEK_SET)) {
		delete[] dst;
		delete[] src;
		return;
	}

	if (fread(dst, sizeof(mo_entry), head.nb_mappings, fp) != head.nb_mappings) {
		delete[] dst;
		delete[] src;
		return;
	}

	// Read strings described by entries
	for (uint32_t i = 0; i < head.nb_mappings; i++) {
		char *key, *val;
		if ((key = read_entry(fp, src + i))) {
			if ((val = read_entry(fp, dst + i))) {
				if (!*key) {
					utf8 = parse_metadata(val);
				} else {
					if (utf8) {
						std::string latin1 = utf8_to_latin1(key);
						strcpy(key, latin1.c_str());

						latin1 = utf8_to_latin1(val);
						strcpy(val, latin1.c_str());
					}
					map[key] = val;
				}
			} else {
				free(key);
			}
		}
	}
	delete[] dst;
	delete[] src;
}
