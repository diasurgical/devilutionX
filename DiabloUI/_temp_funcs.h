void __fastcall artfont_SetArtFont(int nFont);
void __cdecl artfont_InitAllFonts();
void __cdecl artfont_FreeAllFonts();
void __fastcall artfont_FreeArtFont(FontStruct *pFont);
BOOL __cdecl artfont_LoadAllFonts();
void __fastcall artfont_LoadArtFont(FontStruct *pFont, const char *pszBinFile, const char *pszFileName);
int __cdecl artfont_GetFontMaxHeight();
int __cdecl artfont_GetFontDefWidth();
int __fastcall artfont_GetFontWidth(char *str);
void __cdecl j_artfont_cpp_init();
void __cdecl artfont_cpp_init();
int __fastcall artfont_GetFontBreak(char *str);
void __cdecl artfont_delete_operator(void *ptr);
void __fastcall artfont_PrintFontStr(char *str, DWORD **pSurface, int sx, int sy);

signed int bn_prof_100014E8();
//const char *UiProfileGetString();
//BOOL __stdcall UiProfileCallback(int a1, int a2, int a3, int a4, LPARAM a5, int a6, int a7, int a8, int (__stdcall *a9)(_DWORD, _DWORD, _DWORD, _DWORD));
HGDIOBJ __stdcall bn_prof_1000155F(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void UNKCALL bn_prof_100016DD(HWND arg);
void __fastcall bn_prof_100018CE(int a1, int a2);
int __fastcall bn_prof_10001938(HDC a1, _DWORD *a2, char *a3, int a4, int a5);
int __fastcall bn_prof_10001A10(HWND a1, HWND a2);
HINSTANCE __fastcall bn_prof_10001B0A(HWND a1, const CHAR *a2);
HWND UNKCALL bn_prof_10001C0E(HWND hWnd);
void __fastcall bn_prof_10001CB9(_DWORD *a1, int a2, void(__fastcall *a3)(_BYTE *, _DWORD, int), int a4);
BOOL UNKCALL bn_prof_10001CF3(HWND hWnd);
HFONT __fastcall bn_prof_10001D81(HWND hWnd, int a2, int a3);
void UNKCALL bn_prof_10001E34(void *arg);
void __fastcall bn_prof_10001E4C(char *a1, LPARAM lParam, HWND hDlg);
void __fastcall bn_prof_10001ED0(char *a1, _BYTE *a2, int a3);
void *bn_prof_10001F29();
BYTE *bn_prof_10001F84();
//int __stdcall UiProfileDraw(int, int, int, int, HGDIOBJ ho, int, int, int, int, int, int); // idb
BOOL bn_prof_100021C4();
void *bn_prof_10002247();
int j_bn_prof_10002282();
_DWORD *bn_prof_10002282();
void __cdecl bn_prof_10002298();         // idb
int UNKCALL bn_prof_100022A2(HWND hWnd); // idb
int UNKCALL bn_prof_10002353(HGDIOBJ h); // idb
HGDIOBJ bn_prof_100023D8();
_DWORD *__fastcall bn_prof_10002410(HDC hdc, _DWORD *a2);
signed int __fastcall bn_prof_10002456(int a1, const CHAR *a2, char a3, _DWORD *a4);
signed int bn_prof_100026B9();
signed int UNKCALL bn_prof_100026C4(_DWORD *arg);
void UNKCALL bn_prof_100026F0(_DWORD *arg);
int UNKCALL bn_prof_10002749(_DWORD *arg, _DWORD *location);
_DWORD *UNKCALL bn_prof_10002782(int *arg, int a2, int a3, char a4);
_DWORD *UNKCALL bn_prof_100027CE(_DWORD *arg);
void UNKCALL bn_prof_100027D8(_DWORD *arg);
_DWORD *UNKCALL bn_prof_1000280C(int *arg, _DWORD *a2, int a3, _DWORD *a4);
void UNKCALL bn_prof_1000287D(_DWORD *arg);
void UNKCALL bn_prof_10002890(_DWORD *arg);

void UNKCALL BNetGW_100028C2(_DWORD *arg);
void UNKCALL BNetGW_100029BF(_DWORD *arg, int a2);
void *UNKCALL BNetGW_10002A07(_DWORD *arg);
_DWORD *UNKCALL BNetGW_10002A84(_DWORD *arg, signed int a2);
signed int BNetGW_10002AE5();
int UNKCALL BNetGW_10002AF0(_DWORD *arg, char *a2);
_BYTE *UNKCALL BNetGW_10002B21(_DWORD *arg, signed int a2);
void UNKCALL BNetGW_10002B51(_DWORD *arg, signed int a2);
char *UNKCALL BNetGW_10002B78(void *arg, char *a2);
char *UNKCALL BNetGW_10002C23(_DWORD *arg);
int UNKCALL BNetGW_10002C51(_DWORD *arg);
int UNKCALL BNetGW_10002DBF(_DWORD *arg);
char *__stdcall BNetGW_10002DEB(char *a1, unsigned int a2);
char *__stdcall BNetGW_10002E0B(char *a1, unsigned int a2);

void __cdecl Connect_FreeConnectData();
BOOL __cdecl Connect_LoadGFXAndStuff();
BOOL __stdcall UiArtCallback(int game_type, unsigned int art_code, PALETTEENTRY *pPalette, BYTE *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp);
void __cdecl j_Connect_cpp_init();
void __cdecl Connect_cpp_init();
BOOL __stdcall UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5);
BOOL __stdcall UiSoundCallback(int a1, int type, int a3);
BOOL __stdcall UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, LPSTR lpBuffer, int cchBufferMax);
BOOL __stdcall UiDrawDescCallback(int game_type, COLORREF color, LPCSTR lpString, char *a4, int a5, UINT align, time_t a7, HDC *a8);
BOOL __stdcall UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, _DWORD *a6, _DWORD *a7);
int __fastcall Connect_GetRankFromLevel(char *str);
BOOL __fastcall Connect_DiffFromString(char *str, _gamedata *gamedata, int a3, int a4);
void __fastcall Connect_SetDiffString(_gamedata *gamedata, const char *str1, char *str2, char *str3, int size);
BOOL __fastcall Connect_GetHeroInfoConc(const char *a1, _uiheroinfo *pInfo);
void __fastcall Connect_MakeDescString(_uiheroinfo *a1, char *name, size_t size);
void __stdcall UiCreateGameCriteria(_uiheroinfo *pInfo, char *str);
BOOL __stdcall UiCreatePlayerDescription(_uiheroinfo *info, DWORD mode, char *desc);
void __stdcall UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, DWORD type);
void __fastcall Connect_CopyPlrDescStrings(char *str1, int size1, char *str2, int size2);

BOOL __stdcall UiCopyProtError(int *pdwResult);
LRESULT __stdcall CopyProt_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __cdecl CopyProt_FreeCopyResrcs();
BOOL __fastcall CopyProt_LoadCopyStuff(HWND hWnd, int a2);
void __fastcall CopyProt_EndCopyDlg(HWND hWnd, int a2);
void __cdecl j_CopyProt_cpp_init();
void __cdecl CopyProt_cpp_init();

void __cdecl j_cr8game_cpp_init();
void __cdecl cr8game_cpp_init();
BOOL __fastcall cr8game_GetSnetCreaGame(HWND hWnd);
BOOL __stdcall UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6);
LRESULT __stdcall cr8game_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __cdecl cr8game_FreeCreaStuff();
BOOL __fastcall cr8game_LoadCreaGFX(HWND hWnd);
void __fastcall cr8game_FreeMainMem(HWND hWnd);
void __fastcall cr8game_AllocMainMem(HWND hWnd);
void __fastcall cr8game_DoAROP3Blit(HWND hWnd, int frame, int size);
void __fastcall cr8game_SendMessageF5(HWND hWnd);
void __fastcall cr8game_BlitCr8Dialog(HWND hWnd, int a2);
void __fastcall cr8game_SetWindowStr(HWND hWnd, int dlgitem, int a3);
int __fastcall cr8game_CheckValidGameName(char *name);
HFONT __fastcall cr8game_GetCr8Object(HWND hWnd);

void __fastcall CreaDung_SetDelSpin(int a1);
void __cdecl j_CreaDung_cpp_init();
void __cdecl CreaDung_cpp_init();
LRESULT __stdcall CreaDung_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall CreaDung_ParseDungProcs(HWND hWnd, int dlg);
void __fastcall CreaDung_FreeDungProcs(HWND hWnd);
void __fastcall CreaDung_LoadDungGFX(HWND hWnd);
void __fastcall CreaDung_PlaySndAndKill(HWND hWnd, int a2);
void __fastcall CreaDung_DoAllPlaySnd(HWND hWnd);
void __fastcall CreaDung_DoSnetCreaGame(HWND hWnd);
void __fastcall CreaDung_CheckDlgForSnd(HWND hWnd, int a2, int a3);
BOOL __fastcall CreaDung_SelDungDiff(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);

BOOL __stdcall UiGetDefaultStats(int pclass, _uidefaultstats *pStats);
void __cdecl j_CreaStat_cpp_init();
void __cdecl CreaStat_cpp_init();

void __cdecl j_credits_cpp_init();
void __cdecl credits_cpp_init();
BOOL __stdcall UiCreditsDialog(int a1);
LRESULT __stdcall credits_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall credits_FreeCreditResrc(HWND hWnd);
void __fastcall credits_LoadImgCreditTxt(HWND hWnd, LPARAM lParam);
void __fastcall credits_CalcPosROP3(HWND hWnd);
void __fastcall credits_PrintCredLines(HWND hWnd);
int __fastcall credits_GetCredLineBreak(char *str);
char *__fastcall credits_GetAdjustText(char *str, int len);

void __fastcall DiabEdit_DoPaintBMP(HWND hWnd);
void __cdecl j_DiabEdit_cpp_init();
void __cdecl DiabEdit_cpp_init();
void __cdecl DiabEdit_SetupWindow();
LRESULT __stdcall DiabEdit_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall DiabEdit_SendWndCommand(HWND hWnd, WORD a2);
void __fastcall DiabEdit_GetCursorProp(HWND hWnd);
void __fastcall DiabEdit_RestrictAndLimit(HWND hWnd, WPARAM wParam, LPARAM lParam);
void __fastcall DiabEdit_SetTextAndProp(HWND hWnd, WPARAM wParam, LPARAM lParam);
void __fastcall DiabEdit_SetRestrictString(HWND hWnd, LPARAM lParam);
void __fastcall DiabEdit_SetRestrictTimer(HWND hWnd);
void __fastcall DiabEdit_RemoveAllProps(HWND hWnd);

int __cdecl DiabloUI_GetSpawned();
void __stdcall UiOnPaint(int a1);
void __stdcall UiSetBackgroundBitmap(int a1, PALETTEENTRY *a2, int a3, int a4, int a5);
void __stdcall UiSetSpawned(BOOL bSpawned);
void __cdecl UiInitialize();
void __cdecl UiDestroy();
void __stdcall UiAppActivate(BOOL bActive);
BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
void __cdecl j_DiabloUI_cpp_init();
void __cdecl DiabloUI_cpp_init();

signed int DirLink_10005CFA();
BOOL __fastcall DirLink_10005D05(int a1, int a2, int a3, _DWORD *a4, int a5, int playerid);
int __stdcall DirLink_10005D63(HWND hWnd, UINT Msg, WPARAM wParam, unsigned int lParam);
int __fastcall DirLink_10005EB2(HWND hDlg, int a2);
int UNKCALL DirLink_10005F1F(HWND hDlg); // idb
int UNKCALL DirLink_10005F7B(HWND hWnd); // idb
int __fastcall DirLink_10006047(int a1, int a2);
void UNKCALL DirLink_10006073(void *arg);
HWND UNKCALL DirLink_100060D1(HWND arg);
int UNKCALL DirLink_10006141(void *arg);
int UNKCALL DirLink_100061E1(void *arg);
int UNKCALL DirLink_100062BF(void *arg, int a2, char *a3, char *a4);
signed int __stdcall DirLink_1000632B(int a1, char *a2, char *a3);
HWND __fastcall DirLink_10006359(HWND hWnd, int a2, int height);

BOOL __stdcall UiBetaDisclaimer(int a1);
LRESULT __stdcall disclaim_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall disclaim_DelDisclaimProcs(HWND hWnd);
void __fastcall disclaim_LoadDisclaimGFX(HWND hWnd);
void __fastcall disclaim_FadeFromDisclaim(HWND hWnd);
void __cdecl j_disclaim_cpp_init();
void __cdecl disclaim_cpp_init();

void __cdecl j_Doom_cpp_init();
void __cdecl Doom_cpp_init();
void __fastcall Doom_ParseWndProcs(HWND hWnd, int *msgtbl, int nFont, int a4);
void __fastcall Doom_GetSetWndText(HWND hWnd, int msg, int nFont, int a4);
void __fastcall Doom_PrintStrWithSpin(HWND hWnd, BOOL a2);
void __fastcall Doom_AllocAndSetBMP(HWND hWnd, int a2, int bmp_flags); /* check args, __stdcall? */
void __fastcall Doom_GetWindowROP3(HWND hWnd1, HWND hWnd2);
void __fastcall Doom_ParseWndProc2(HWND hWnd, int *msgtbl, int nFont, int a4);
void __fastcall Doom_GetSetWndTxt2(HWND hWnd, int msg, int nFont, int a4);
void __fastcall Doom_ParseWndProc3(HWND hWnd, int *msgtbl, int nFont);
void __fastcall Doom_GetSetWndTxt3(HWND hWnd, int msg, int nFont);
void __fastcall Doom_PrintStrWithSpn2(HWND hWnd, int justify_type);
void __fastcall Doom_ParseWndProc4(HWND hWnd, int *msgtbl, int nFont);
void __fastcall Doom_GetSetWndTxt4(HWND hWnd, int msg, int nFont);
void __fastcall Doom_ParseWndProc5(HWND hWnd, int *msgtbl, int nFont);
void __fastcall Doom_GetSetWndTxt5(HWND hWnd, int msg, int nFont);
void __fastcall Doom_PrintTextMsg403(HWND hWnd);
void __fastcall Doom_ParseWndProc6(HWND hWnd, int *msgtbl, int nFont);
void __fastcall Doom_GetSetWndTxt6(HWND hWnd, int msg, int nFont);
void __fastcall Doom_DeleteFreeProcs(HWND hWnd, int *msgtbl);

int __stdcall EntDial_10006C96(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam); // idb
HWND UNKCALL EntDial_10006D78(HWND hDlg);
HWND USERCALL EntDial_10006DB8(HWND hWnd, int a2);
int __fastcall EntDial_10006EA7(HWND hDlg, int a2);
void __fastcall EntDial_10006EE8(HWND hWnd, unsigned int a2, int a3);
int __fastcall EntDial_10006F16(HWND hDlg, int, int); // idb
signed int EntDial_10006F71();

LRESULT __stdcall EntName_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall EntName_DelEntNameMsgs(HWND hWnd);
void __fastcall EntName_LoadFocusChkName(HWND hWnd);
void __fastcall EntName_SetCharName(HWND hWnd, int a2);
void __fastcall EntName_GetMessageName(HWND hWnd, unsigned int a2, int a3);
void __cdecl j_EntName_cpp_init();
void __cdecl EntName_cpp_init();

void __fastcall Fade_ApplyPaletteRange(int range1, int range2);
void __fastcall Fade_UpdatePaletteRange(int range);
BOOL __cdecl Fade_CheckRange5();
void __cdecl Fade_Range5SetZero();
void __fastcall Fade_NoInputAndArt(HWND hWnd, BOOL bShowCurs);
void __fastcall Fade_SetInputWindow(HWND hWnd);
void __fastcall Fade_SetFadeTimer(int nTime);
void __stdcall Fade_TimerFunctionDlg(int a1, int a2, int a3, int a4);
void __cdecl j_Fade_cpp_init();
void __cdecl Fade_cpp_init();

void __fastcall Focus_CheckPlayMove(LPARAM lParam);
int __cdecl Focus_GetSpinWidthOrZero();
void __fastcall Focus_BlitSpinner(HWND hWnd1, HWND hWnd2);
void __fastcall Focus_CenterSpinFromSide(HWND hWnd);
void __fastcall Focus_GetAndBlitSpin(HWND hWnd, LPARAM lParam);
BOOL __fastcall Focus_DoBlitSpinIncFrame(HWND hWnd1, HWND hWnd2);
void __cdecl Focus_DeleteSpinners();
void __cdecl Focus_ResetSpinToZero();
void __cdecl j_Focus_cpp_init();
void __cdecl Focus_cpp_init();
void __fastcall Focus_LoadSpinner(const char *pszFileName);
void __fastcall Focus_SetFocusTimer(HWND hWnd, const char *pszFileName);
void __stdcall Focus_SetFocusAndBlit(int hWnd, int a2, int a3, int a4);
void __fastcall Focus_KillFocusTimer(HWND hWnd);

void __cdecl local_InitUiPalette();
void __cdecl local_DelUiPalette();
tagPALETTEENTRY *__fastcall local_GetArtPalEntry(int entry);
void __fastcall local_ClearPalette(PALETTEENTRY *pPal);
void __cdecl local_ClearSurface();
BOOL __fastcall local_LoadArtImage(const char *pszFileName, BYTE **pBuffer, DWORD *pdwSize);
BOOL __fastcall local_LoadArtWithPal(HWND hWnd, int a2, char *src, int mask, int flags, const char *pszFileName, BYTE **pBuffer, DWORD *pdwSize, BOOL a9);
void __fastcall local_AdjustRectSize(tagRECT *pRect, int a2, int a3);
BOOL __fastcall local_SetStaticBmp(HWND hWnd, int nIDDlgItem, BYTE *pBuffer, DWORD *pdwSize);
void __cdecl j_local_cpp_init();
void __cdecl local_cpp_init();
BOOL __fastcall local_SetButtonBmp(HWND hWnd, int flags, int a7, void *pBuffer, DWORD *pdwSize);
void __fastcall local_FitButtonDlg(HWND hWnd, int *a2, void *pBuffer, DWORD *pdwSize);
void __fastcall local_SetWhiteText(HDC hdc);
BOOL __fastcall local_GetBottomRect(HWND hWnd1, HWND hWnd2, int width, int height);
void __fastcall local_DlgDoPaint(HWND hWnd);
void __fastcall local_DoUiWndProc(HWND hWnd, DWORD *pdwMsgTbl);
LRESULT __stdcall local_PostUiWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void __fastcall local_DoUiWndProc2(HWND hWnd, DWORD *pdwMsgTbl);
LRESULT __stdcall local_PostUiWndProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL __fastcall local_DisableKeyWaitMouse(HWND hWnd);
DWORD *__cdecl local_AllocWndLongData();
void __fastcall local_FreeMemPtr(void **p);
void __fastcall local_SetWndLongStr(int WndLongData, const char *pszStr);
void __cdecl local_LoadArtCursor();
void __cdecl local_InitArtCursor();
void __cdecl local_FreeArtCursor();
void __cdecl local_SetCursorArt();
void __cdecl local_SetCursorDefault();
void __fastcall local_SetDiabloCursor(HWND hWnd);

void __cdecl j_MainMenu_cpp_init();
void __cdecl MainMenu_cpp_init();
BOOL __stdcall UiMainMenuDialog(char *name, int *pdwResult, void(__stdcall *fnSound)(char *file), int a4);
LRESULT __stdcall MainMenu_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall MainMenu_KillAndFreeMenu(HWND hWnd);
void __fastcall MainMenu_SetMenuTimer(HWND hWnd);
void __fastcall MainMenu_LoadMenuGFX(HWND hWnd);
void __fastcall MainMenu_DoOptions(HWND hWnd, int option, int PlaySelect);
BOOL __cdecl MainMenu_CheckEnoughMemory();
void __fastcall MainMenu_CheckWParamFocus(HWND hWnd, WPARAM wParam);

int Modem_1000855D();
HWND __fastcall Modem_10008563(HWND hDlg, const char *edx0, int a2);
int __stdcall Modem_100085D8(int, char *, char *); // idb
BOOL Modem_10008606();
char *Modem_1000863D();
signed int Modem_10008648();
int Modem_10008653();
int Modem_10008659();
int UNKCALL Modem_1000865F(char *); // idb
BOOL __fastcall Modem_10008680(int a1, int a2, int a3, _DWORD *a4, int a5, int playerid);
int __stdcall Modem_100086DE(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // idb
void **UNKCALL Modem_1000879E(HWND hDlg);
BOOL UNKCALL Modem_100087DB(HWND hWnd);
int Modem_10008888();
int UNKCALL Modem_100088DB(HWND hWnd);         // idb
int UNKCALL Modem_1000893D(HWND hWnd);         // idb
int __fastcall Modem_10008A38(HWND hWnd, int); // idb
void __cdecl Modem_10008B42(char *a1);
int UNKCALL Modem_10008BB7(HWND hWnd); // idb
int UNKCALL Modem_10008BFE(HWND hWnd); // idb

int __stdcall ModmStat_10008C62(char *, int, int, int, int); // idb
int UNKCALL ModmStat_10008C87(void *arg);
int __stdcall ModmStat_10008CA0(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // idb
int UNKCALL ModmStat_10008DB3(HWND hDlg);                                           // idb
BOOL UNKCALL ModmStat_10008DE4(HWND hWnd);
int __fastcall ModmStat_10008E89(int a1, int a2);
void UNKCALL ModmStat_10008EBF(HWND hDlg);
signed int ModmStat_10008F26();

BOOL __fastcall OkCancel_DrawString(HWND hWnd, char *str);
void __cdecl j_OkCancel_cpp_init();
void __cdecl OkCancel_cpp_init();
LRESULT __stdcall OkCancel_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall OkCancel_FreeDlgBmp(HWND hWnd);
BOOL __fastcall OkCancel_LoadOkCancGFX(HWND hWnd, DWORD *lParam);
void __fastcall OkCancel_PlaySndEndDlg(HWND hWnd, int a2);
void __fastcall OkCancel_DoOkDialog(HWND hWnd, char *str, int a3);
void __stdcall UiMessageBoxCallback(HWND hWnd, char *lpText, LPCSTR lpCaption, UINT uType);

signed int Progress_10009480();
//BOOL __stdcall UiProgressDialog(int a1, int a2, BOOL a3, int (*a4)(void), int a5);
int __stdcall Progress_100094F4(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // idb
void *Progress_100095EC();
BOOL __fastcall Progress_10009675(HWND hWnd, const CHAR *edx0);
BOOL __fastcall Progress_10009805(HWND hWnd, int a2);
void Progress_100098B0();
void UNKCALL Progress_100098C5(HWND hWnd);
BOOL UNKCALL Progress_1000991C(HWND hWnd);

void __cdecl j_Sbar_cpp_init();
void __cdecl Sbar_cpp_init();
BOOL __fastcall Sbar_CheckIfNextHero(HWND hWnd);
int __fastcall Sbar_NumScrollLines(HWND hWnd, int width, int height);
void __fastcall Sbar_DrawScrollBar(HWND hWnd, int nIDDlgItem, int width, int height);
void __fastcall Sbar_LoadScrBarGFX(HWND hWnd, int nIDDlgItem);
void __cdecl j_Sbar_cpp_init2();
void __cdecl Sbar_cpp_init2();
void __fastcall Sbar_FreeScrollBar(HWND hWnd, int nIDDlgItem);

LRESULT __stdcall SelClass_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall SelClass_FreeClassMsgTbl(HWND hWnd);
void __fastcall SelClass_LoadClassFocus(HWND hWnd);
void __fastcall SelClass_SetDefaultStats(HWND hWnd, int a2);
void __fastcall SelClass_CheckClassSpawn(HWND hWnd, int a2);
void __cdecl j_SelClass_cpp_init();
void __cdecl SelClass_cpp_init();

void *SelConn_1000A082();
signed int SelConn_1000A09B();
int __stdcall SelConn_1000A0A6(HWND hWnd, UINT Msg, WPARAM wParam, unsigned int lParam);
HWND __fastcall SelConn_1000A226(HWND hDlg, int nIDDlgItem);
HWND UNKCALL SelConn_1000A3E2(HWND hDlg);
int SelConn_1000A3FF();
void UNKCALL SelConn_1000A43A(HWND hDlg);
BOOL __fastcall SelConn_1000A4B9(_DWORD *a1);
BOOL UNKCALL SelConn_1000A4CD(void *location);
HWND UNKCALL SelConn_1000A4E4(HWND hWnd, char *a2, int a3);
signed int __stdcall SelConn_1000A5F3(int a1, char *a2, char *a3, int a4);
int __fastcall SelConn_1000A670(HWND a1, const char *a2);
void UNKCALL SelConn_1000A6EC(HWND hDlg);
LRESULT __stdcall SelConn_1000A73E(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
HWND UNKCALL SelConn_1000A866(HWND hWnd);
HWND UNKCALL SelConn_1000A8D7(HWND hWnd);
HWND UNKCALL SelConn_1000A948(HWND hWnd);
int UNKCALL SelConn_1000A9F3(HWND hWnd); // idb
_DWORD *__fastcall SelConn_1000AA28(int a1);
HWND UNKCALL SelConn_1000AA3B(HWND hWnd);
HWND UNKCALL SelConn_1000AAEB(HWND hWnd);
HWND UNKCALL SelConn_1000AB83(HWND hWnd);
int __fastcall SelConn_1000AC07(int a1, int a2);
int UNKCALL SelConn_1000AC30(HWND arg);
int UNKCALL SelConn_1000AC9E(HWND hWnd); // idb
int UNKCALL SelConn_1000ADA8(HWND hWnd); // idb
BOOL UNKCALL SelConn_1000ADD0(HWND hWnd);
int __fastcall SelConn_1000AE19(int a1, UINT a2);
HWND __fastcall SelConn_1000AE59(HWND hWnd, int a2, int height);
//signed int __stdcall UiSelectProvider(int a1, int a2, int a3, int a4, char *a5, int *a6);

int UNKCALL SelDial_1000B011(char *arg);
signed int SelDial_1000B0C4();
int __stdcall SelDial_1000B0CF(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // idb
HWND __fastcall SelDial_1000B1FB(HWND hWnd, int a2);
HWND UNKCALL SelDial_1000B29A(HWND hDlg);
int __fastcall SelDial_1000B2D8(int a1, int a2);
HWND UNKCALL SelDial_1000B354(HWND hDlg);
HWND UNKCALL SelDial_1000B3D8(HWND hDlg);
HWND UNKCALL SelDial_1000B44C(HWND hDlg);
HWND USERCALL SelDial_1000B483(HWND hWnd, int a2);
int SelDial_1000B5D9();
int __fastcall SelDial_1000B614(HWND hWnd, int, int); // idb

void UNKCALL SelGame_1000B66A(void *arg);
int SelGame_1000B671();
void UNKCALL SelGame_1000B677(void *arg);
int SelGame_1000B67E();
//int __stdcall UiSelectGame(int, int, void *, int, int, int); // idb
signed int SelGame_1000B795();

_uiheroinfo *__cdecl SelHero_GetCurrentHeroInfo();
int __cdecl SelHero_GetNumHeroesLeft();
void __fastcall SelHero_SetHeroDifficulty(int diff);
char *__cdecl SelHero_GetHeroNameStr();
_uiheroinfo *__cdecl SelHero_AllocHeroInfo();
int __cdecl SelHero_GetHeroIsGood();
int __fastcall SelHero_SetClassStats(int heroclass, _uidefaultstats *pStats);
void __cdecl j_SelHero_cpp_init();
void __cdecl SelHero_cpp_init();
void __fastcall SelHero_SetStaticBMP(HWND hWnd, int adjust_size);
void __fastcall SelHero_PrintHeroInfo(HWND hWnd, _uiheroinfo *pInfo);
void __fastcall SelHero_SetStringWithMsg(HWND hWnd, const char *str);
BOOL __fastcall SelHero_IsNameReserved(char *name);
void __fastcall SelHero_SetLastNamePos(char *name);
BOOL __fastcall SelHero_NameHasChar(char *name, char *illegalchrs);
BOOL __fastcall UiValidPlayerName(char *name);
BOOL __stdcall UiSelHeroMultDialog(BOOL(__stdcall *fninfo)(BOOL(__stdcall *fninfofunc)(_uiheroinfo *)), BOOL(__stdcall *fncreate)(_uiheroinfo *), BOOL(__stdcall *fnremove)(_uiheroinfo *), BOOL(__stdcall *fnstats)(int, _uidefaultstats *), int *dlgresult, int *a6, char *name);
LRESULT __stdcall SelHero_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall SelHero_DoStuffWithStrings(HWND hWnd);
_uiheroinfo *__fastcall SelHero_GetNextHeroFromStr(_uiheroinfo *pInfo, char *name);
void __fastcall SelHero_FreeSomeMemory(void *ptr);
_uiheroinfo *__fastcall SelHero_GetHeroSlotFromName(_uiheroinfo *pInfo, const char *name);
void __fastcall SelHero_DoHeroSelList(HWND hWnd);
void __fastcall SelHero_DoHeroSelClass(HWND hWnd);
void __fastcall SelHero_DoEnterName(HWND hWnd);
BOOL __fastcall SelHero_CreateHero(HWND hWnd, char *name);
void __fastcall SelHero_DoSelLoad(HWND hWnd);
void __fastcall SelHero_DoSelDiff(HWND hWnd);
void __fastcall SelHero_DeleteAndFree(HWND hWnd);
void __fastcall SelHero_FreeAllHeroes(_uiheroinfo *pInfo);
void __fastcall SelHero_DoHeroEndFade(HWND hWnd, int a2);
void __fastcall SelHero_LoadHeroGFX(HWND hWnd);
void __fastcall SelHero_SelectHeroRegion(HWND hWnd);
BOOL __stdcall SelHero_GetHeroInfo(_uiheroinfo *pInfo);
BOOL __stdcall UiSelHeroSingDialog(BOOL(__stdcall *fninfo)(BOOL(__stdcall *fninfofunc)(_uiheroinfo *)), BOOL(__stdcall *fncreate)(_uiheroinfo *), BOOL(__stdcall *fnremove)(_uiheroinfo *), BOOL(__stdcall *fnstats)(int, _uidefaultstats *), int *dlgresult, char *name, int *difficulty);

void *SelIPX_1000C610();
signed int SelIPX_1000C629();
BOOL __fastcall SelIPX_1000C634(int a1, int a2, int a3, _DWORD *a4, int a5, int playerid);
int __stdcall SelIPX_1000C692(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // idb
LONG __fastcall SelIPX_1000C818(HWND hDlg, int nIDDlgItem);
HWND UNKCALL SelIPX_1000C982(HWND hDlg);
int SelIPX_1000C99F();
const char *UNKCALL SelIPX_1000C9DA(HWND hDlg);
void __fastcall SelIPX_1000CA64(_DWORD *a1);
_DWORD **__fastcall SelIPX_1000CA71(_DWORD *a1);
BOOL UNKCALL SelIPX_1000CAC1(void *location);
void *__stdcall SelIPX_1000CAD5(int a1, char *a2, char *a3);
_DWORD *__fastcall SelIPX_1000CB50(_DWORD *a1, _DWORD *a2);
_DWORD *__fastcall SelIPX_1000CB73(_DWORD *a1, int a2);
int __fastcall SelIPX_1000CB83(HWND a1, const char *a2);
int UNKCALL SelIPX_1000CC41(HWND hDlg); // idb
BOOL __fastcall SelIPX_1000CCC5(_DWORD *a1);
HWND UNKCALL SelIPX_1000CCD9(HWND hWnd);
HWND UNKCALL SelIPX_1000CD4A(HWND hWnd);
void UNKCALL SelIPX_1000CEE6(HWND hDlg);
LRESULT __stdcall SelIPX_1000CF38(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
HWND UNKCALL SelIPX_1000D070(HWND hWnd);
HWND UNKCALL SelIPX_1000D0E1(HWND hWnd);
int UNKCALL SelIPX_1000D18C(HWND hWnd); // idb
_DWORD *__fastcall SelIPX_1000D1C1(int a1);
HWND UNKCALL SelIPX_1000D1D4(HWND hWnd);
HWND UNKCALL SelIPX_1000D284(HWND hWnd);
HWND UNKCALL SelIPX_1000D31C(HWND hWnd);
int __fastcall SelIPX_1000D3A0(int a1, int a2);
HWND USERCALL SelIPX_1000D3C5(HWND hDlg, int a2);
BOOL __fastcall SelIPX_1000D4CA(HWND hDlg, int a2);
char *UNKCALL SelIPX_1000D520(char *arg);
const char *__fastcall SelIPX_1000D58D(const char *a1, const char *a2);
int __fastcall SelIPX_1000D5B0(int a1, int a2);
HWND __fastcall SelIPX_1000D696(HWND hDlg, int a2, int height);

void __cdecl j_SelList_cpp_init();
void __cdecl SelList_cpp_init();
LRESULT __stdcall SelList_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall SelList_DeleteFreeProcs(HWND hWnd);
void __fastcall SelList_GetHeroStats(HWND hWnd, int nIDDlgItem);
void __fastcall SelList_CountHeroList(HWND hWnd);
int __fastcall SelList_GetNextHeroLong(HWND hWnd);
void __fastcall SelList_LoadFocus16(HWND hWnd);
void __fastcall SelList_KillFocus16(HWND hWnd);
void __fastcall SelList_ShowListWindow(HWND hWnd);
void __fastcall SelList_SetHeroDlgLong(HWND hWnd, _uiheroinfo *pInfo);
void __fastcall SelList_DoListOldProc(HWND hWnd);
LRESULT __stdcall SelList_OldListWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall SelList_ShiftHeroDlgItems(HWND hWnd);
void __fastcall SelList_ShiftHeroDlgItm2(HWND hWnd);
void __fastcall SelList_HeroesWithBigDialogs(HWND hWnd);
_uiheroinfo *__fastcall SelList_GetHeroFromNum(int heronum);
void __fastcall SelList_HeroesWithHugeDlg(HWND hWnd);
void __fastcall SelList_HeroDlgWithSound(HWND hWnd);
void __fastcall SelList_HeroDlgWithSnd2(HWND hWnd);
void __fastcall SelList_ChooseDlgFromSize(HWND hWnd, int width, int height);

LRESULT __stdcall SelLoad_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall SelLoad_DeleteProcsAndSpin(HWND hWnd);
void __fastcall SelLoad_LoadFocusAndMsg(HWND hWnd);
void __fastcall SelLoad_SelectSndLoad(HWND hWnd, int a2);
void __cdecl j_SelLoad_cpp_init();
void __cdecl SelLoad_cpp_init();

signed int SelModem_1000E42A();
int __fastcall SelModem_1000E435(void *a1, int a2, int a3, char *a4, char *a5);
char *__stdcall SelModem_1000E497(int a1, char *a2, char *a3);
void *SelModem_1000E4EC();
_DWORD *__fastcall SelModem_1000E500(int a1, _DWORD *a2);
signed int UNKCALL SelModem_1000E505(void *arg);
signed int SelModem_1000E51E();
BOOL __fastcall SelModem_1000E553(_DWORD *a1);
BOOL UNKCALL SelModem_1000E567(void *location);
int __fastcall SelModem_1000E57B(int a1, int a2);
signed int SelModem_1000E5CC();
int __stdcall SelModem_1000E63E(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam); // idb
void UNKCALL SelModem_1000E783(HWND hDlg);
HWND UNKCALL SelModem_1000E7E9(HWND hDlg);
int UNKCALL SelModem_1000E80E(HWND hWnd); // idb
HWND UNKCALL SelModem_1000E843(HWND hWnd);
int __fastcall SelModem_1000E932(HWND a1, const char *a2);
void UNKCALL SelModem_1000E9B2(HWND hDlg);
LRESULT __stdcall SelModem_1000EA04(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
HWND UNKCALL SelModem_1000EB2C(HWND hWnd);
HWND UNKCALL SelModem_1000EB9D(HWND hWnd);
HWND UNKCALL SelModem_1000EC0E(HWND hWnd);
_DWORD *__fastcall SelModem_1000EC9F(int a1);
HWND UNKCALL SelModem_1000ECB2(HWND hWnd);
HWND UNKCALL SelModem_1000ED3B(HWND hWnd);
HWND UNKCALL SelModem_1000EDBC(HWND hWnd);
int __fastcall SelModem_1000EE29(int a1, int a2);
HWND __fastcall SelModem_1000EE78(HWND hWnd, int a2, int height);

void *SelRegn_1000EF42();
_uiheroinfo *__fastcall SelRegn_SetNextHero(_uiheroinfo *pNext, _uiheroinfo *pCurrent);
signed int SelRegn_1000EF60();
int __stdcall SelRegn_1000EF6B(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // idb
HWND __fastcall SelRegn_1000F0D7(HWND hDlg, int nIDDlgItem);
HWND UNKCALL SelRegn_1000F109(HWND hDlg);
int SelRegn_1000F126();
void UNKCALL SelRegn_1000F161(HWND hDlg);
BOOL __fastcall SelRegn_1000F1D4(_DWORD *a1);
BOOL UNKCALL SelRegn_1000F1E8(void *location);
HWND UNKCALL SelRegn_1000F1FC(HWND hWnd);
signed int SelRegn_1000F2ED();
int __fastcall SelRegn_1000F346(HWND a1, const char *a2);
void UNKCALL SelRegn_1000F3C2(HWND hDlg);
LRESULT __stdcall SelRegn_1000F414(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
HWND UNKCALL SelRegn_1000F53C(HWND hWnd);
HWND UNKCALL SelRegn_1000F5AD(HWND hWnd);
HWND UNKCALL SelRegn_1000F61E(HWND hWnd);
int UNKCALL SelRegn_1000F6C9(HWND hWnd); // idb
_DWORD *__fastcall SelRegn_1000F6FE(int a1);
HWND UNKCALL SelRegn_1000F711(HWND hWnd);
HWND UNKCALL SelRegn_1000F7C1(HWND hWnd);
HWND UNKCALL SelRegn_1000F859(HWND hWnd);
signed int UNKCALL SelRegn_1000F8DD(void *arg);
signed int SelRegn_1000F8F6();
HWND __fastcall SelRegn_1000F929(HWND hWnd, int a2, int height);
//signed int __stdcall UiSelectRegion(_DWORD *a1);

int __fastcall SelYesNo_YesNoDialog(HWND hWnd, char *dialogstr, char *hero, int nofocus); /* void */
LRESULT __stdcall SelYesNo_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall SelYesNo_RemoveYNDialog(HWND hWnd);
void __fastcall SelYesNo_LoadSelYN_GFX(HWND hWnd);
void __fastcall SelYesNo_DoSelectYesNo(HWND hWnd, int option);
int __fastcall SelYesNo_SelOkDialog(HWND hWnd, char *dialogstr, char *hero, int nofocus); /* void */
int __fastcall SelYesNo_SpawnErrDialog(HWND hWnd, int string_rsrc, int is_popup);         /* void */
void __cdecl j_SelYesNo_cpp_init();
void __cdecl SelYesNo_cpp_init();

void __fastcall Title_BlitTitleBuffer(HWND hWnd);
void __cdecl Title_DeletePhTrans();
void __fastcall Title_FreeTransMem(HWND hWnd);
void __fastcall Title_SetTitleBMP(HWND hWnd);
void __fastcall Title_LoadTitleImage(HWND hWnd, const char *pszFileName);
void __fastcall Title_LoadImgSetTimer(HWND hWnd, const char *pszFileName);
void __stdcall Title_BlitTitleBufFnc(int hWnd, int a2, int a3, int a4);
void __cdecl j_Title_cpp_init();
void __cdecl Title_cpp_init();
void __fastcall Title_KillTitleTimer(HWND hWnd);
BOOL __stdcall UiTitleDialog(int a1);
LRESULT __stdcall Title_MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void __fastcall Title_KillTimerAndFree(HWND hWnd);
void __fastcall Title_LoadAllTitleImgs(HWND hWnd, int time);
void __fastcall Title_KillAndFadeDlg(HWND hWnd);

void __fastcall TitleSnd_SetSoundFunction(void(__stdcall *func)(char *file));
void __cdecl TitleSnd_InitSoundFunc();
void __cdecl TitleSnd_PlayMoveSound();
void __cdecl TitleSnd_PlaySelectSound();
void __cdecl j_TitleSnd_cpp_init();
void __cdecl TitleSnd_cpp_init();
