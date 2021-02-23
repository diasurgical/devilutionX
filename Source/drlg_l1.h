/**
 * @file drlg_l1.h
 *
 * Interface of the cathedral level generation algorithms.
 */
#ifndef __DRLG_L1_H__
#define __DRLG_L1_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern int UberRow;
extern int UberCol;
extern bool IsUberRoomOpened;
extern int UberLeverRow;
extern int UberLeverCol;
extern bool IsUberLeverActivated;
extern int UberDiabloMonsterIndex;

void DRLG_Init_Globals();
void LoadL1Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL1Dungeon(const char *sFileName, int vx, int vy);
void CreateL5Dungeon(DWORD rseed, int entry);
void drlg_l1_set_crypt_room(int rx1, int ry1);
void drlg_l1_set_corner_room(int rx1, int ry1);
void drlg_l1_crypt_pattern1(int rndper);
void drlg_l1_crypt_pattern2(int rndper);
void drlg_l1_crypt_pattern3(int rndper);
void drlg_l1_crypt_pattern4(int rndper);
void drlg_l1_crypt_pattern5(int rndper);
void drlg_l1_crypt_pattern6(int rndper);
void drlg_l1_crypt_pattern7(int rndper);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DRLG_L1_H__ */
