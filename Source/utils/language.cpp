#include "options.h"
#include "utils/paths.h"
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

char *read_entry(FILE *fp, mo_entry *e)
{
	void *data;

	if (fseek(fp, e->offset, SEEK_SET)) {
		return nullptr;
	}

	if (!(data = calloc(e->length, sizeof(char)))) {
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

void LanguageInitialize()
{
	auto path = GetLangPath() + "./" + sgOptions.Language.szCode + ".gmo";
	mo_entry *src, *dst;
	mo_head head;
	FILE *fp;

	if (!(fp = fopen(path.c_str(), "rb"))) {
		perror(path.c_str());
		return;
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
			if (*key && (val = read_entry(fp, dst + i))) {
				map[key] = val;
			} else {
				free(key);
			}
		}
	}
	delete[] dst;
	delete[] src;
}
