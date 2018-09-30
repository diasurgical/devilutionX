#include "storm.h"

#define rBool { return TRUE; }
#define rPVoid { return NULL; }
#define rVoid { return; }
#define rInt { return 0; }

BOOL STORMAPI SNetCreateGame(const char *pszGameName, const char *pszGamePassword, const char *pszGameStatString, DWORD dwGameType, char *GameTemplateData, int GameTemplateSize, int playerCount, char *creatorName, char *a11, int *playerID) rBool;
BOOL STORMAPI SNetDestroy() rBool;
BOOL STORMAPI SNetEnumProviders(int (STORMAPI *callback)(DWORD, DWORD, DWORD, DWORD), int mincaps) rBool;

BOOL STORMAPI SNetEnumGames(int (STORMAPI *callback)(DWORD, DWORD, DWORD), int *hintnextcall) rBool;
BOOL STORMAPI SNetDropPlayer(int playerid, DWORD flags) rBool;
BOOL STORMAPI SNetGetGameInfo(int type, void *dst, size_t length, size_t *byteswritten) rBool;

BOOL STORMAPI SNetGetNumPlayers(int *firstplayerid, int *lastplayerid, int *activeplayers) rBool;

BOOL STORMAPI SNetGetPlayerCaps(char playerid, PCAPS playerCaps) rBool;
BOOL STORMAPI SNetGetPlayerName(int playerid, char *buffer, size_t buffersize) rBool;
//BOOL STORMAPI SNetGetProviderCaps(PCAPS providerCaps) rBool;
BOOL STORMAPI SNetGetTurnsInTransit(int *turns) rBool;
BOOL STORMAPI SNetInitializeDevice(int a1, int a2, int a3, int a4, int *a5) rBool;
//BOOL STORMAPI SNetInitializeProvider(DWORD providerName, client_info *gameClientInfo, user_info *userData, battle_info *bnCallbacks, module_info *moduleData) rBool;
BOOL STORMAPI SNetJoinGame(int id, char *gameName, char *gamePassword, char *playerName, char *userStats, int *playerid) rBool;
BOOL STORMAPI SNetLeaveGame(int type) rBool;
BOOL STORMAPI SNetPerformUpgrade(DWORD *upgradestatus) rBool;
BOOL STORMAPI SNetReceiveMessage(int *senderplayerid, char **data, int *databytes) rBool;
BOOL STORMAPI SNetReceiveTurns(int a1, int arraysize, char **arraydata, unsigned int *arraydatabytes, DWORD *arrayplayerstatus) rBool;
//HANDLE STORMAPI SNetRegisterEventHandler(int type, void (STORMAPI *sEvent)(PS_EVT)) rPVoid;

int STORMAPI SNetSelectGame(int a1, int a2, int a3, int a4, int a5, int *playerid) rInt;

BOOL STORMAPI SNetSendMessage(int playerID, void *data, size_t databytes) rBool;
BOOL STORMAPI SNetSendTurn(char *data, size_t databytes) rBool;

BOOL STORMAPI SNetSetGameMode(DWORD modeFlags, bool makePublic) rBool;

BOOL STORMAPI SNetEnumGamesEx(int a1, int a2, int (__fastcall *callback)(DWORD, DWORD, DWORD), int *hintnextcall) rBool;
BOOL STORMAPI SNetSendServerChatCommand(const char *command) rBool;

BOOL STORMAPI SNetDisconnectAll(DWORD flags) rBool;
BOOL STORMAPI SNetCreateLadderGame(const char *pszGameName, const char *pszGamePassword, const char *pszGameStatString, DWORD dwGameType, DWORD dwGameLadderType, DWORD dwGameModeFlags, char *GameTemplateData, int GameTemplateSize, int playerCount, char *creatorName, char *a11, int *playerID) rBool;
BOOL STORMAPI SNetReportGameResult(unsigned a1, int size, int *results, const char* headerInfo, const char* detailInfo) rBool;

int STORMAPI SNetSendLeagueCommand(char *cmd, char *callback) rInt;
int STORMAPI SNetSendReplayPath(int a1, int a2, char *replayPath) rInt;
int STORMAPI SNetGetLeagueName(int leagueID) rInt;
BOOL STORMAPI SNetGetPlayerNames(char **names) rBool;
int STORMAPI SNetLeagueLogout(char *bnetName) rInt;
int STORMAPI SNetGetLeaguePlayerName(char *curPlayerLeageName, size_t nameSize) rInt;

HGDIOBJ STORMAPI SDlgDefDialogProc(HWND hDlg, signed int DlgType, HDC textLabel, HWND hWnd) rPVoid;

HANDLE STORMAPI SDlgDialogBoxIndirectParam(HMODULE hModule, LPCSTR lpName, HWND hWndParent, LPVOID lpParam, LPARAM lParam) rPVoid;

BOOL STORMAPI SDlgEndDialog(HWND hDlg, HANDLE nResult) rBool;

BOOL STORMAPI SDlgSetControlBitmaps(HWND parentwindow, int *id, int a3, char *buffer2, char *buffer, int flags, int mask) rBool;

BOOL STORMAPI SDlgBltToWindowE(HWND hWnd, HRGN a2, char *a3, int a4, void *buffer, RECT *rct, SIZE *size, int a8, int a9, DWORD rop) rBool;
BOOL STORMAPI SDlgSetBitmapE(HWND hWnd, int a2, char *src, int mask1, int flags, int a6, int a7, int width, int a9, int mask2) rBool;

int STORMAPI Ordinal224(int a1) rInt;

BOOL STORMAPI SFileCloseArchive(HANDLE hArchive) rBool;
BOOL STORMAPI SFileCloseFile(HANDLE hFile) rBool;

BOOL STORMAPI SFileDdaBeginEx(HANDLE directsound, DWORD flags, DWORD mask, unsigned __int32 lDistanceToMove, signed __int32 volume, signed int a6, int a7) rBool;
BOOL STORMAPI SFileDdaDestroy() rBool;
BOOL STORMAPI SFileDdaEnd(HANDLE directsound) rBool;
BOOL STORMAPI SFileDdaGetPos(HANDLE directsound, int a2, int a3) rBool;

BOOL STORMAPI SFileDdaInitialize(HANDLE directsound) rBool;
BOOL STORMAPI SFileDdaSetVolume(HANDLE directsound, signed int bigvolume, signed int volume) rBool;
BOOL STORMAPI SFileDestroy() rBool;

BOOL STORMAPI SFileGetFileArchive(HANDLE hFile, HANDLE archive) rBool;
LONG STORMAPI SFileGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh) rInt;
BOOL STORMAPI SFileOpenArchive(const char *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE *phMpq) rBool;
BOOL STORMAPI SFileOpenFile(const char *filename, HANDLE *phFile) rBool;
BOOL STORMAPI SFileOpenFileEx(HANDLE hMpq, const char *szFileName, DWORD dwSearchScope, HANDLE *phFile) rBool;
BOOL STORMAPI SFileReadFile(HANDLE hFile, void *buffer, DWORD nNumberOfBytesToRead, DWORD *read, LONG lpDistanceToMoveHigh) rBool;

void STORMAPI SFileSetLocale(LCID lcLocale) rVoid;

BOOL STORMAPI SFileSetIoErrorMode(int mode, BOOL (STORMAPI *callback)(char*,int,int) ) rBool;
BOOL STORMAPI SFileGetArchiveName(HANDLE hArchive, char *name, int length) rBool;
BOOL STORMAPI SFileGetFileName(HANDLE hFile, char *buffer, int length) rBool;

BOOL STORMAPI SFileLoadFile(char *filename, void *buffer, int buffersize, int a4, int a5) rBool;
BOOL STORMAPI SFileUnloadFile(HANDLE hFile) rBool;
BOOL STORMAPI SFileLoadFileEx(void *hArchive, char *filename, int a3, int a4, int a5, DWORD searchScope, struct _OVERLAPPED *lpOverlapped) rBool;

BOOL STORMAPI SBltROP3(void *lpDstBuffer, void *lpSrcBuffer, int width, int height, int a5, int a6, int a7, DWORD rop) rBool;
BOOL STORMAPI SBltROP3Clipped(void *lpDstBuffer, RECT *lpDstRect, POINT *lpDstPt, int a4, void *lpSrcBuffer, RECT *lpSrcRect, POINT *lpSrcPt, int a8, int a9, DWORD rop) rBool;

BOOL STORMAPI SBmpDecodeImage(DWORD dwImgType, void *pSrcBuffer, DWORD dwSrcBuffersize, PALETTEENTRY *pPalette, void *pDstBuffer, DWORD dwDstBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp) rBool;

BOOL STORMAPI SBmpLoadImage(const char *pszFileName, PALETTEENTRY *pPalette, void *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *dwHeight, DWORD *pdwBpp) rBool;

BOOL   STORMAPI SBmpSaveImage(const char*, PALETTEENTRY*, void*, DWORD, DWORD, DWORD) rBool;
HANDLE STORMAPI SBmpAllocLoadImage(const char *fileName, PALETTEENTRY *palette, void **buffer, int *width, int *height, int unused6, int unused7, void *(STORMAPI *allocFunction)(DWORD)) rPVoid;

BOOL STORMAPI SCodeCompile(char *directives1, char *directives2, char *loopstring, unsigned int maxiterations, unsigned int flags, HANDLE handle) rBool;
BOOL STORMAPI SCodeDelete(HANDLE handle) rBool;

int  STORMAPI SCodeExecute(HANDLE handle, int a2) rInt;

BOOL STORMAPI SDrawAutoInitialize(HINSTANCE hInst, LPCSTR lpClassName, LPCSTR lpWindowName, WNDPROC pfnWndProc, int nMode, int nWidth, int nHeight, int nBits) rBool;
BOOL STORMAPI SDrawCaptureScreen(const char *source) rBool;

HWND STORMAPI SDrawGetFrameWindow(HWND *sdraw_framewindow) rPVoid;
BOOL STORMAPI SDrawGetObjects(LPDIRECTDRAW *ddInterface, LPDIRECTDRAWSURFACE *primarySurface, LPDIRECTDRAWSURFACE *surface2, LPDIRECTDRAWSURFACE *surface3, LPDIRECTDRAWSURFACE *backSurface, LPDIRECTDRAWPALETTE *ddPalette, HPALETTE *hPalette) rBool;
BOOL STORMAPI SDrawGetScreenSize(DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp) rBool;

BOOL STORMAPI SDrawLockSurface(int surfacenumber, RECT *lpDestRect, void **lplpSurface, int *lpPitch, int arg_unused) rBool;
BOOL STORMAPI SDrawManualInitialize(HWND hWnd, LPDIRECTDRAW ddInterface, LPDIRECTDRAWSURFACE primarySurface, LPDIRECTDRAWSURFACE surface2, LPDIRECTDRAWSURFACE surface3, LPDIRECTDRAWSURFACE backSurface, LPDIRECTDRAWPALETTE ddPalette, HPALETTE hPalette) rBool;

BOOL STORMAPI SDrawPostClose() rBool;
//BOOL STORMAPI SDrawRealizePalette() rBool;

BOOL STORMAPI SDrawUnlockSurface(int surfacenumber, void *lpSurface, int a3, RECT *lpRect) rBool;
BOOL STORMAPI SDrawUpdatePalette(unsigned int firstentry, unsigned int numentries, PALETTEENTRY *pPalEntries, int a4) rBool;

BOOL STORMAPI SEvtDispatch(DWORD dwMessageID, DWORD dwFlags, int type, PS_EVT pEvent) rBool;

BOOL STORMAPI SGdiDeleteObject(HANDLE handle) rBool;

BOOL STORMAPI SGdiExtTextOut(int a1, int a2, int a3, int a4, unsigned int a8, signed int a6, signed int a7, const char *string, unsigned int arg20) rBool;
BOOL STORMAPI SGdiImportFont(HGDIOBJ handle, int windowsfont) rBool;

BOOL STORMAPI SGdiSelectObject(int handle) rBool;
BOOL STORMAPI SGdiSetPitch(int pitch) rBool;

BOOL STORMAPI Ordinal393(char *string, int, int) rBool;

void* STORMAPI SMemAlloc(size_t amount, char *logfilename, int logline, char defaultValue) rPVoid;

BOOL STORMAPI SMemFree(void *location, char *logfilename, int logline, char defaultValue) rBool;

void* STORMAPI SMemReAlloc(void *location, size_t amount, char *logfilename, int logline, char defaultValue) rPVoid;

BOOL STORMAPI SRegLoadData(const char *keyname, const char *valuename, int size, LPBYTE lpData, BYTE flags, LPDWORD lpcbData) rBool;
BOOL STORMAPI SRegLoadString(const char *keyname, const char *valuename, BYTE flags, char *buffer, size_t buffersize) rBool;
BOOL STORMAPI SRegLoadValue(const char *keyname, const char *valuename, BYTE flags, int *value) rBool;
BOOL STORMAPI SRegSaveData(const char *keyname, const char *valuename, int size, BYTE *lpData, DWORD cbData) rBool;
BOOL STORMAPI SRegSaveString(const char *keyname, const char *valuename, BYTE flags, char *string) rBool;
BOOL STORMAPI SRegSaveValue(const char *keyname, const char *valuename, BYTE flags, DWORD result) rBool;

BOOL STORMAPI SRegDeleteValue(const char *keyname, const char *valuename, BYTE flags) rBool;

BOOL STORMAPI STransBlt(void *lpSurface, int x, int y, int width, HANDLE hTrans) rBool;
BOOL STORMAPI STransBltUsingMask(void *lpSurface, void *lpSource, int pitch, int width, HANDLE hTrans) rBool;

BOOL STORMAPI STransDelete(HANDLE hTrans) rBool;

BOOL STORMAPI STransDuplicate(HANDLE hTransSource, HANDLE hTransDest) rBool;
BOOL STORMAPI STransIntersectDirtyArray(HANDLE hTrans, char * dirtyarraymask, unsigned flags, HANDLE * phTransResult) rBool;
BOOL STORMAPI STransInvertMask(HANDLE hTrans, HANDLE * phTransResult) rBool;

BOOL STORMAPI STransSetDirtyArrayInfo(int width, int height, int depth, int bits) rBool;

BOOL STORMAPI STransPointInMask(HANDLE hTrans, int x, int y) rBool;
BOOL STORMAPI STransCombineMasks(HANDLE hTransA, HANDLE hTransB, int left, int top, int flags, HANDLE * phTransResult) rBool;

BOOL STORMAPI STransCreateE(void *pBuffer, int width, int height, int bpp, int a5, int bufferSize, HANDLE *phTransOut) rBool;

BOOL STORMAPI SVidDestroy() rBool;
BOOL STORMAPI SVidGetSize(HANDLE video, int width, int height, int zero) rBool;
BOOL STORMAPI SVidInitialize(HANDLE video) rBool;
BOOL STORMAPI SVidPlayBegin(char *filename, int arg4, int a3, int a4, int a5, int a6, HANDLE* video) rBool;

BOOL STORMAPI SVidPlayContinueSingle(HANDLE video, int a2, int a3) rBool;
BOOL STORMAPI SVidPlayEnd(HANDLE video) rBool;

BOOL STORMAPI SErrDisplayError(DWORD dwErrMsg, const char *logfilename, int logline, const char *message, BOOL allowOption, int exitCode) rBool;
BOOL STORMAPI SErrGetErrorStr(DWORD dwErrCode, char *buffer, size_t bufferchars) rBool;
DWORD STORMAPI SErrGetLastError() rInt;

void STORMAPI SErrSetLastError(DWORD dwErrCode) rVoid;

void STORMAPI SErrSuppressErrors(BOOL suppressErrors) rVoid;

void STORMAPI SMemCopy(void *dest, const void *source, size_t size) rVoid;
void STORMAPI SMemFill(void *location, size_t length, char fillWith) rVoid;

void STORMAPI SMemZero(void *location, DWORD length) rVoid;
int  STORMAPI SMemCmp(void *location1, void *location2, DWORD size) rInt;

int   STORMAPI SStrCopy(char *dest, const char *src, int max_length) rInt;
DWORD STORMAPI SStrHash(const char *string, DWORD flags, DWORD Seed) rInt;
int   STORMAPI SStrNCat(char *dest, const char *src, DWORD max_length) rInt;

int STORMAPI SStrLen(const char* string) rInt;

int STORMAPI SStrCmp(const char *string1, const char *string2, size_t size) rInt;
int STORMAPI SStrCmpI(const char *string1, const char *string2, size_t size) rInt;
char* STORMAPI SStrUpper(char* string) rPVoid;

void STORMAPI SRgn523(HANDLE hRgn, RECT *pRect, int a3, int a4) rVoid;
void STORMAPI SRgnCreateRegion(HANDLE *hRgn, int a2) rVoid;
void STORMAPI SRgnDeleteRegion(HANDLE hRgn) rVoid;

void STORMAPI SRgn529i(int handle, int a2, int a3) rVoid;

BOOL SErrDisplayErrorFmt(DWORD dwErrMsg, const char *logfilename, int logline, BOOL allowOption, int exitCode, const char *format, ...) rBool;

void STORMAPI SErrCatchUnhandledExceptions() rVoid;

char* STORMAPI SStrChr(const char *string, char c) rPVoid;
char* STORMAPI SStrChrR(const char *string, char c) rPVoid;

size_t SStrVPrintf(char *dest, size_t size, const char *format, ...) rInt;

int STORMAPI SBigDel(void *buffer) rInt;

int STORMAPI SBigFromBinary(void *buffer, const void *str, size_t size) rInt;

int STORMAPI SBigNew(void **buffer) rInt;

int STORMAPI SBigPowMod(void *buffer1, void *buffer2, int a3, int a4) rInt;

int STORMAPI SBigToBinaryBuffer(void *buffer, int length, int a3, int a4) rInt;
//

void __stdcall SDrawMessageBox(char *,char *,int) rVoid;
void __cdecl SDrawDestroy(void) rVoid;
bool __cdecl StormDestroy(void) rBool;
bool __stdcall SFileSetBasePath(char *) rBool;
void __cdecl SDrawRealizePalette(void) rVoid;
bool __cdecl SVidPlayContinue(void) rBool;
bool __stdcall SNetGetOwnerTurnsWaiting(int *) rBool;
void * __stdcall SNetUnregisterEventHandler(int,void (__stdcall*)(struct _SNETEVENT *)) rPVoid;
void * __stdcall SNetRegisterEventHandler(int,void (__stdcall*)(struct _SNETEVENT *)) rPVoid;
bool __stdcall SNetSetBasePlayer(int) rBool;
int __stdcall SNetInitializeProvider(unsigned long,struct _SNETPROGRAMDATA *,struct _SNETPLAYERDATA *,struct _SNETUIDATA *,struct _SNETVERSIONDATA *) rInt;
int __stdcall SNetGetProviderCaps(struct _SNETCAPS *) rInt;
int __stdcall SFileSetFilePointer(HANDLE,int,HANDLE,int) rInt;
