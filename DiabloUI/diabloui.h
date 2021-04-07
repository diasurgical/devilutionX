//HEADER_GOES_HERE
#pragma once

namespace devilution {

typedef struct _uidefaultstats {
	Uint16 strength;
	Uint16 magic;
	Uint16 dexterity;
	Uint16 vitality;
} _uidefaultstats;

typedef struct _uiheroinfo {
	struct _uiheroinfo *next;
	char name[16];
	Uint16 level;
	plr_class heroclass;
	Uint8 herorank;
	Uint16 strength;
	Uint16 magic;
	Uint16 dexterity;
	Uint16 vitality;
	Sint32 gold;
	Sint32 hassaved;
	bool spawned;
} _uiheroinfo;

void UiDestroy();
void UiTitleDialog();
void UiSetSpawned(BOOL bSpawned);
void UiInitialize();
BOOL UiValidPlayerName(const char *name); /* check */
void UiSelHeroMultDialog(BOOL (*fninfo)(BOOL (*fninfofunc)(_uiheroinfo *)), BOOL (*fncreate)(_uiheroinfo *), BOOL (*fnremove)(_uiheroinfo *), void (*fnstats)(unsigned int, _uidefaultstats *), _selhero_selections *dlgresult, char (*name)[16]);
void UiSelHeroSingDialog(BOOL (*fninfo)(BOOL (*fninfofunc)(_uiheroinfo *)), BOOL (*fncreate)(_uiheroinfo *), BOOL (*fnremove)(_uiheroinfo *), void (*fnstats)(unsigned int, _uidefaultstats *), _selhero_selections *dlgresult, char (*name)[16], int *difficulty);
BOOL UiCreditsDialog();
BOOL UiSupportDialog();
BOOL UiMainMenuDialog(const char *name, int *pdwResult, void (*fnSound)(const char *file), int attractTimeOut);
BOOL UiProgressDialog(const char *msg, int enable, int (*fnfunc)(), int rate);
void UiProfileCallback();
void UiProfileDraw();
BOOL UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, DWORD *a6, DWORD *a7);
BOOL UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5);
BOOL UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, char *lpBuffer, int cchBufferMax);
BOOL UiSoundCallback(int a1, int type, int a3);
BOOL UiDrawDescCallback(int game_type, DWORD color, const char *lpString, char *a4, int a5, UINT align, time_t a7, HDC *a8);
BOOL UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6);
BOOL UiArtCallback(int game_type, unsigned int art_code, SDL_Color *pPalette, BYTE *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp);
int UiSelectGame(GameData *gameData, int *playerId);
int UiSelectProvider(GameData *gameData);
void UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, Uint32 type);

} // namespace devilution
