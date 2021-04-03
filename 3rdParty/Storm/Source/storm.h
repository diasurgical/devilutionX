#pragma once

#include <cstdint>
#include <limits>
#include <string>
namespace dvl {

// Note to self: Linker error => forgot a return value in cpp

// Storm API definition
#ifndef STORMAPI
#define STORMAPI
#endif

#ifdef __cplusplus
struct CCritSect {
	SDL_mutex *m_critsect;

	CCritSect()
	{
		m_critsect = SDL_CreateMutex();
		if (m_critsect == NULL) {
			ErrSdl();
		}
	}
	~CCritSect()
	{
		SDL_DestroyMutex(m_critsect);
	}
	void Enter()
	{
		if (SDL_LockMutex(m_critsect) < 0) {
			ErrSdl();
		}
	}
	void Leave()
	{
		if (SDL_UnlockMutex(m_critsect) < 0) {
			ErrSdl();
		}
	}
};
#endif

// Game states
#define GAMESTATE_PRIVATE 0x01
#define GAMESTATE_FULL    0x02
#define GAMESTATE_ACTIVE  0x04
#define GAMESTATE_STARTED 0x08
#define GAMESTATE_REPLAY  0x80

#define PS_CONNECTED 0x10000
#define PS_TURN_ARRIVED 0x20000
#define PS_ACTIVE 0x40000

#define LEAVE_ENDING 0x40000004
#define LEAVE_DROP 0x40000006

#if defined(__GNUC__) || defined(__cplusplus)
extern "C" {
#endif

BOOL STORMAPI SNetCreateGame(const char *pszGameName, const char *pszGamePassword, const char *pszGameStatString, DWORD dwGameType, char *GameTemplateData, int GameTemplateSize, int playerCount, const char *creatorName, const char *a11, int *playerID);
BOOL STORMAPI SNetDestroy();

/*  SNetDropPlayer @ 106
 *
 *  Drops a player from the current game.
 *
 *  playerid:     The player ID for the player to be dropped.
 *  flags:
 *
 *  Returns TRUE if the function was called successfully and FALSE otherwise.
 */
BOOL
    STORMAPI
    SNetDropPlayer(
        int playerid,
        DWORD flags);

/*  SNetGetGameInfo @ 107
 *
 *  Retrieves specific game information from Storm, such as name, password,
 *  stats, mode, game template, and players.
 *
 *  type:         The type of data to retrieve. See GAMEINFO_ flags.
 *  dst:          The destination buffer for the data.
 *  length:       The maximum size of the destination buffer.
 *
 *  Returns TRUE if the function was called successfully and FALSE otherwise.
 */
BOOL
    STORMAPI
    SNetGetGameInfo(
        int type,
        void *dst,
        unsigned int length);

/*  SNetGetTurnsInTransit @ 115
 *
 *  Retrieves the number of turns (buffers) that have been queued
 *  before sending them over the network.
 *
 *  turns: A pointer to an integer that will receive the value.
 *
 *  Returns TRUE if the function was called successfully and FALSE otherwise.
 */
BOOL
    STORMAPI
    SNetGetTurnsInTransit(
        DWORD *turns);

// Network provider structures
typedef struct _client_info {
	DWORD dwSize; // 60
	char *pszName;
	char *pszVersion;
	DWORD dwProduct;
	DWORD dwVerbyte;
	DWORD dwUnk5;
	DWORD dwMaxPlayers;
	DWORD dwUnk7;
	DWORD dwUnk8;
	DWORD dwUnk9;
	DWORD dwUnk10; // 0xFF
	char *pszCdKey;
	char *pszCdOwner;
	DWORD dwIsShareware;
	DWORD dwLangId;
} client_info;

typedef struct _user_info {
	DWORD dwSize; // 16
	char *pszPlayerName;
	char *pszUnknown;
	DWORD dwUnknown;
} user_info;

BOOL STORMAPI SNetJoinGame(int id, char *gameName, char *gamePassword, char *playerName, char *userStats, int *playerid);

/*  SNetLeaveGame @ 119
 *
 *  Notifies Storm that the player has left the game. Storm will
 *  notify all connected peers through the network provider.
 *
 *  type: The leave type. It doesn't appear to be important, no documentation available.
 *
 *  Returns TRUE if the function was called successfully and FALSE otherwise.
 */
BOOL
    STORMAPI
    SNetLeaveGame(
        int type);

BOOL STORMAPI SNetPerformUpgrade(DWORD *upgradestatus);
BOOL STORMAPI SNetReceiveMessage(int *senderplayerid, char **data, int *databytes);
BOOL STORMAPI SNetReceiveTurns(int a1, int arraysize, char **arraydata, DWORD *arraydatabytes, DWORD *arrayplayerstatus);

typedef void(STORMAPI *SEVTHANDLER)(struct _SNETEVENT *);

/*  SNetSendMessage @ 127
 *
 *  Sends a message to a player given their player ID. Network message
 *  is sent using class 01 and is retrieved by the other client using
 *  SNetReceiveMessage().
 *
 *  playerID:   The player index of the player to receive the data.
 *              Conversely, this field can be one of the following constants:
 *                  SNPLAYER_ALL      | Sends the message to all players, including oneself.
 *                  SNPLAYER_OTHERS   | Sends the message to all players, except for oneself.
 *  data:       A pointer to the data.
 *  databytes:  The amount of bytes that the data pointer contains.
 *
 *  Returns TRUE if the function was called successfully and FALSE otherwise.
 */
BOOL
    STORMAPI
    SNetSendMessage(
        int playerID,
        void *data,
        unsigned int databytes);

// Macro values to target specific players
#define SNPLAYER_ALL    -1
#define SNPLAYER_OTHERS -2

#define MPQ_FLAG_READ_ONLY 1
#define SFILE_OPEN_FROM_MPQ 0
#define SFILE_OPEN_LOCAL_FILE 0xFFFFFFFF

/*  SNetSendTurn @ 128
 *
 *  Sends a turn (data packet) to all players in the game. Network data
 *  is sent using class 02 and is retrieved by the other client using
 *  SNetReceiveTurns().
 *
 *  data:       A pointer to the data.
 *  databytes:  The amount of bytes that the data pointer contains.
 *
 *  Returns TRUE if the function was called successfully and FALSE otherwise.
 */
BOOL
    STORMAPI
    SNetSendTurn(
        char *data,
        unsigned int databytes);

BOOL STORMAPI SFileCloseArchive(HANDLE hArchive);
BOOL STORMAPI SFileCloseFile(HANDLE hFile);

LONG STORMAPI SFileGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
BOOL STORMAPI SFileOpenArchive(const char *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE *phMpq);

BOOL STORMAPI SFileOpenFile(const char *filename, HANDLE *phFile);
BOOL STORMAPI SFileOpenFileEx(HANDLE hMpq, const char *szFileName, DWORD dwSearchScope, HANDLE *phFile);

BOOL STORMAPI SFileReadFile(HANDLE hFile, void *buffer, DWORD nNumberOfBytesToRead, DWORD *read, LONG *lpDistanceToMoveHigh);

/*  SBmpLoadImage @ 323
 *
 *  Load an image from an available archive into a buffer.
 *
 *  pszFileName:  The name of the graphic in an active archive.
 *  pPalette:     An optional buffer that receives the image palette.
 *  pBuffer:      A buffer that receives the image data.
 *  dwBuffersize: The size of the specified image buffer.
 *  pdwWidth:     An optional variable that receives the image width.
 *  pdwHeight:    An optional variable that receives the image height.
 *  pdwBpp:       An optional variable that receives the image bits per pixel.
 *
 *  Returns TRUE if the image was supported and loaded correctly, FALSE otherwise.
 */
BOOL
    STORMAPI
    SBmpLoadImage(
        const char *pszFileName,
        SDL_Color *pPalette,
        BYTE *pBuffer,
        DWORD dwBuffersize,
        DWORD *pdwWidth,
        DWORD *pdwHeight,
        DWORD *pdwBpp);

/*  SMemAlloc @ 401
 *
 *  Allocates a block of memory. This block is different
 *  from the standard malloc by including a header containing
 *  information about the block.
 *
 *  amount:       The amount of memory to allocate, in bytes.
 *  logfilename:  The name of the file or object that this call belongs to.
 *  logline:      The line in the file or one of the SLOG_ macros.
 *  defaultValue: The default value of a byte in the allocated memory.
 *
 *  Returns a pointer to the allocated memory. This pointer does NOT include
 *  the additional storm header.
 */
void *
    STORMAPI
    SMemAlloc(
        unsigned int amount,
        const char *logfilename,
        int logline,
        int defaultValue);

/*  SMemFree @ 403
 *
 *  Frees a block of memory that was created using SMemAlloc,
 *  includes the log file and line for debugging purposes.
 *
 *  location:     The memory location to be freed.
 *  logfilename:  The name of the file or object that this call belongs to.
 *  logline:      The line in the file or one of the SLOG_ macros.
 *  defaultValue:
 *
 *  Returns TRUE if the call was successful and FALSE otherwise.
 */
BOOL
    STORMAPI
    SMemFree(
        void *location,
        const char *logfilename,
        int logline,
        char defaultValue);

bool getIniBool(const char *sectionName, const char *keyName, bool defaultValue = false);
bool getIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, const char *defaultString = "");
void setIniValue(const char *sectionName, const char *keyName, const char *value, int len = 0);
void SaveIni();
int getIniInt(const char *keyname, const char *valuename, int defaultValue);
void setIniInt(const char *keyname, const char *valuename, int value);

void SVidPlayBegin(const char *filename, int a2, int a3, int a4, int a5, int flags, HANDLE *video);
void SVidPlayEnd(HANDLE video);

/*  SErrGetLastError @ 463
 *
 *  Retrieves the last error that was specifically
 *  set for the Storm library.
 *
 *  Returns the last error set within the Storm library.
 */
DWORD
STORMAPI
SErrGetLastError();

/*  SErrSetLastError @ 465
 *
 *  Sets the last error for the Storm library and the Kernel32 library.
 *
 *  dwErrCode:  The error code that will be set.
 */
void
    STORMAPI
    SErrSetLastError(
        DWORD dwErrCode);

// Values for dwErrCode
#define STORM_ERROR_GAME_TERMINATED              0x85100069
#define STORM_ERROR_INVALID_PLAYER               0x8510006a
#define STORM_ERROR_NO_MESSAGES_WAITING          0x8510006b
#define STORM_ERROR_NOT_IN_GAME                  0x85100070
#define STORM_ERROR_REQUIRES_UPGRADE             0x85100077

/*  SStrCopy @ 501
 *
 *  Copies a string from src to dest (including NULL terminator)
 *  until the max_length is reached.
 *
 *  dest:         The destination array.
 *  src:          The source array.
 *  max_length:   The maximum length of dest.
 *
 */
void
    STORMAPI
    SStrCopy(
        char *dest,
        const char *src,
        int max_length);

BOOL SFileSetBasePath(const char *);
BOOL SVidPlayContinue(void);
BOOL SNetGetOwnerTurnsWaiting(DWORD *);
bool SNetUnregisterEventHandler(event_type, SEVTHANDLER);
bool SNetRegisterEventHandler(event_type, SEVTHANDLER);
BOOLEAN SNetSetBasePlayer(int);
int SNetInitializeProvider(unsigned long, struct _SNETPROGRAMDATA *, struct _SNETPLAYERDATA *, struct _SNETUIDATA *, struct _SNETVERSIONDATA *);
int SNetGetProviderCaps(struct _SNETCAPS *);
int SFileSetFilePointer(HANDLE, int, int*, int);
BOOL SFileEnableDirectAccess(BOOL enable);

#if defined(__GNUC__) || defined(__cplusplus)
}

// Additions to Storm API:

// Sets the file's 64-bit seek position.
inline std::uint64_t SFileSetFilePointer(HANDLE hFile, std::int64_t offset, int whence)
{
    int high = static_cast<std::uint64_t>(offset) >> 32;
    int low = static_cast<int>(offset);
    low = SFileSetFilePointer(hFile, low, &high, whence);
    return (static_cast<std::uint64_t>(high) << 32) | low;
}

// Returns the current 64-bit file seek position.
inline std::uint64_t SFileGetFilePointer(HANDLE hFile)
{
    // We use `SFileSetFilePointer` with offset 0 to get the current position
    // because there is no `SFileGetFilePointer`.
    return SFileSetFilePointer(hFile, 0, DVL_FILE_CURRENT);
}

#endif

} // namespace dvl
