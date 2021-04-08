/**
 * @file textdat.h
 *
 * Interface of all dialog texts.
 */
#pragma once

namespace devilution {

typedef struct TextDataStruct {
	const char *txtstr;
	bool scrlltxt;
	_sfx_id sfxnr;
} TextDataStruct;

extern const TextDataStruct alltext[];

} // namespace devilution
