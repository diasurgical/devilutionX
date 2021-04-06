/**
 * @file textdat.h
 *
 * Interface of all dialog texts.
 */
#ifndef __TEXTDAT_H__
#define __TEXTDAT_H__

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TextDataStruct {
	const char *txtstr;
	bool scrlltxt;
	_sfx_id sfxnr;
} TextDataStruct;

extern const TextDataStruct alltext[];

#ifdef __cplusplus
}
#endif

}

#endif /* __TEXTDAT_H__ */
