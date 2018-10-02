#include "../types.h"
#include "stubs.h"

DWORD nLastError = 0;

BOOL STORMAPI SNetCreateGame(const char *pszGameName, const char *pszGamePassword, const char *pszGameStatString,
                             DWORD dwGameType, char *GameTemplateData, int GameTemplateSize, int playerCount,
                             char *creatorName, char *a11, int *playerID)
{
	DUMMY();
	return TRUE;
}

BOOL STORMAPI SNetDestroy()
{
	DUMMY();
	return TRUE;
}

BOOL STORMAPI SNetDropPlayer(int playerid, DWORD flags)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SNetGetGameInfo(int type, void *dst, size_t length, size_t *byteswritten)
{
	DUMMY();
	return TRUE;
}

BOOL STORMAPI SNetGetTurnsInTransit(int *turns)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SNetLeaveGame(int type)
{
	DUMMY();
	return TRUE;
}

BOOL STORMAPI SNetPerformUpgrade(DWORD *upgradestatus)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SNetReceiveTurns(int a1, int arraysize, char **arraydata, unsigned int *arraydatabytes,
                               DWORD *arrayplayerstatus)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SNetSendTurn(char *data, size_t databytes)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SNetSetGameMode(DWORD modeFlags, bool makePublic)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SNetSendServerChatCommand(const char *command)
{
	UNIMPLEMENTED();
}

// BOOL STORMAPI SFileCloseArchive(HANDLE hArchive)
// {
// 	UNIMPLEMENTED();
// }

// BOOL STORMAPI SFileCloseFile(HANDLE hFile)
// {
// 	UNIMPLEMENTED();
// }

BOOL STORMAPI SFileDdaBeginEx(HANDLE directsound, DWORD flags, DWORD mask, unsigned __int32 lDistanceToMove,
                              signed __int32 volume, signed int a6, int a7)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SFileDdaDestroy()
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SFileDdaEnd(HANDLE directsound)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SFileDdaGetPos(HANDLE directsound, int a2, int a3)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SFileDdaInitialize(HANDLE directsound)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SFileDdaSetVolume(HANDLE directsound, signed int bigvolume, signed int volume)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SFileGetFileArchive(HANDLE hFile, HANDLE archive)
{
	UNIMPLEMENTED();
}

// LONG STORMAPI SFileGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
// {
// 	UNIMPLEMENTED();
// }

// BOOL STORMAPI SFileOpenArchive(const char *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE
// *phMpq)
// {
// 	UNIMPLEMENTED();
// }

BOOL STORMAPI SFileOpenFile(const char *filename, HANDLE *phFile)
{
	BOOL result;
	//eprintf("%s: %s\n", __FUNCTION__, filename);

	result = SFileOpenFileEx((HANDLE)patch_rt_mpq, filename, 0, phFile);
	if (!result) {
		result = SFileOpenFileEx((HANDLE)diabdat_mpq, filename, 0, phFile);
	}

	if (!result || !*phFile) {
		eprintf("%s: Not found: %s\n", __FUNCTION__, filename);
	}
	return result;
}

// BOOL STORMAPI SFileOpenFileEx(HANDLE hMpq, const char *szFileName, DWORD dwSearchScope, HANDLE
// *phFile)
// {
// 	UNIMPLEMENTED();
// }

// BOOL STORMAPI SFileReadFile(HANDLE hFile, void *buffer, DWORD nNumberOfBytesToRead, DWORD *read,
// LONG lpDistanceToMoveHigh)
// {
// 	UNIMPLEMENTED();
// }

// int __stdcall SFileSetFilePointer(HANDLE, int, HANDLE, int)
// {
// 	UNIMPLEMENTED();
// }

HWND STORMAPI SDrawGetFrameWindow(HWND *sdraw_framewindow)
{
	DUMMY();
	return NULL;
}

// BOOL STORMAPI SDrawManualInitialize(HWND hWnd, LPDIRECTDRAW ddInterface, LPDIRECTDRAWSURFACE
// primarySurface, LPDIRECTDRAWSURFACE surface2, LPDIRECTDRAWSURFACE surface3, LPDIRECTDRAWSURFACE
// backSurface, LPDIRECTDRAWPALETTE ddPalette, HPALETTE hPalette)
//{
//	UNIMPLEMENTED();
//}

void *STORMAPI SMemAlloc(size_t amount, char *logfilename, int logline, char defaultValue)
{
	// fprintf(stderr, "%s: %d (%s:%d)\n", __FUNCTION__, amount, logfilename, logline);
	assert(amount != -1);
	return malloc(amount);
}

BOOL STORMAPI SMemFree(void *location, char *logfilename, int logline, char defaultValue)
{
	// fprintf(stderr, "%s: (%s:%d)\n", __FUNCTION__, logfilename, logline);
	assert(location);
	free(location);
	return TRUE;
}

void *STORMAPI SMemReAlloc(void *location, size_t amount, char *logfilename, int logline, char defaultValue)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SRegLoadData(const char *keyname, const char *valuename, int size, LPBYTE lpData, BYTE flags,
                           LPDWORD lpcbData)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SRegLoadString(const char *keyname, const char *valuename, BYTE flags, char *buffer, size_t buffersize)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SRegLoadValue(const char *keyname, const char *valuename, BYTE flags, int *value)
{
	DUMMY_PRINT("key: %s value: %s", keyname, valuename);
	return FALSE;
}

BOOL STORMAPI SRegSaveData(const char *keyname, const char *valuename, int size, BYTE *lpData, DWORD cbData)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SRegSaveString(const char *keyname, const char *valuename, BYTE flags, char *string)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SRegSaveValue(const char *keyname, const char *valuename, BYTE flags, DWORD result)
{
	DUMMY_PRINT("key: %s value: %s", keyname, valuename);
	return TRUE;
}

// BOOL STORMAPI SVidDestroy()
//{
//	UNIMPLEMENTED();
//}
//
// BOOL STORMAPI SVidInitialize(HANDLE video)
//{
//	UNIMPLEMENTED();
//}
//
// BOOL STORMAPI SVidPlayBegin(char *filename, int arg4, int a3, int a4, int a5, int a6, HANDLE
// *video)
//{
//	UNIMPLEMENTED();
//}
//
// BOOL STORMAPI SVidPlayEnd(HANDLE video)
//{
//	UNIMPLEMENTED();
//}

BOOL STORMAPI SErrDisplayError(DWORD dwErrMsg, const char *logfilename, int logline, const char *message,
                               BOOL allowOption, int exitCode)
{
	UNIMPLEMENTED();
}

BOOL STORMAPI SErrGetErrorStr(DWORD dwErrCode, char *buffer, size_t bufferchars)
{
	UNIMPLEMENTED();
}

DWORD STORMAPI SErrGetLastError()
{
	return nLastError;
}

void STORMAPI SErrSetLastError(DWORD dwErrCode)
{
	nLastError = dwErrCode;
}

void STORMAPI SMemCopy(void *dest, const void *source, size_t size)
{
	UNIMPLEMENTED();
}

void STORMAPI SMemFill(void *location, size_t length, char fillWith)
{
	UNIMPLEMENTED();
}

void STORMAPI SMemZero(void *location, DWORD length)
{
	UNIMPLEMENTED();
}

int STORMAPI SMemCmp(void *location1, void *location2, DWORD size)
{
	UNIMPLEMENTED();
}

int STORMAPI SStrCopy(char *dest, const char *src, int max_length)
{
	UNIMPLEMENTED();
}

int STORMAPI SStrCmp(const char *string1, const char *string2, size_t size)
{
	UNIMPLEMENTED();
}

int STORMAPI SStrCmpI(const char *string1, const char *string2, size_t size)
{
	UNIMPLEMENTED();
}

// void __stdcall SDrawMessageBox(char *, char *, int)
//{
//	UNIMPLEMENTED();
//}
//
// void __cdecl SDrawDestroy(void)
//{
//	UNIMPLEMENTED();
//}
//
// bool __cdecl StormDestroy(void)
//{
//	UNIMPLEMENTED();
//}
//
// bool __stdcall SFileSetBasePath(char *)
//{
//	UNIMPLEMENTED();
//}

void __cdecl SDrawRealizePalette(void)
{
	DUMMY();
}

// bool __cdecl SVidPlayContinue(void)
//{
//	UNIMPLEMENTED();
//}

bool __stdcall SNetGetOwnerTurnsWaiting(int *)
{
	UNIMPLEMENTED();
}

void *__stdcall SNetUnregisterEventHandler(int, void(__stdcall *)(struct _SNETEVENT *))
{
	DUMMY();
	return (void *)-1;
}

void *__stdcall SNetRegisterEventHandler(int, void(__stdcall *)(struct _SNETEVENT *))
{
	UNIMPLEMENTED();
}

bool __stdcall SNetSetBasePlayer(int)
{
	DUMMY();
	return TRUE;
}

int __stdcall SNetInitializeProvider(unsigned long a1, struct _SNETPROGRAMDATA *client_info,
                                     struct _SNETPLAYERDATA *user_info, struct _SNETUIDATA *ui_info,
                                     struct _SNETVERSIONDATA *fileinfo)
{
	DUMMY();

	ui_info->selectnamecallback(0, 0, 0, 0, 0, NULL, 0, NULL, 0, NULL);

	return TRUE;
}

int __stdcall SNetGetProviderCaps(struct _SNETCAPS *)
{
	UNIMPLEMENTED();
}
