/**
 * @file loadsave.h
 *
 * Interface of save game functionality.
 */
#ifndef __LOADSAVE_H__
#define __LOADSAVE_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern bool gbIsHellfireSaveGame;
extern int giNumberOfLevels;

int RemapItemIdxFromDiablo(int i);
int RemapItemIdxToDiablo(int i);
bool IsHeaderValid(Uint32 magicNumber);
void LoadHotkeys(int pnum);
void LoadGame(int pnum, BOOL firstflag);
void SaveHotkeys(int pnum);
void SaveGame(int pnum);
void SaveLevel(int pnum);
void LoadLevel(int pnum);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __LOADSAVE_H__ */
