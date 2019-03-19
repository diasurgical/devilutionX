#include <SDL.h>
#include <SDL_mixer.h>
#include <Radon.hpp>
#include <smacker.h>

#include "devilution.h"
#include "stubs.h"
#include "DiabloUI/diabloui.h"
#include "dx.h"

namespace dvl {

DWORD nLastError = 0;

static std::string getIniPath()
{
	char path[DVL_MAX_PATH];
	int len = GetModuleFileNameA(ghInst, path, DVL_MAX_PATH);
	path[len - 1] = '/';
	strcat(path, "diablo.ini");

	return path;
}

static radon::File ini(getIniPath());
static Mix_Chunk *SFileChunk;

void TranslateFileName(char *dst, int dstLen, const char *src)
{
	for (int i = 0; i < dstLen; i++) {
		char c = *src++;
		dst[i] = c == '\\' ? '/' : c;
		if (!c) {
			break;
		}
	}
}

// BOOL SFileCloseArchive(HANDLE hArchive)
// {
// 	UNIMPLEMENTED();
// }

// BOOL SFileCloseFile(HANDLE hFile)
// {
// 	UNIMPLEMENTED();
// }

BOOL SFileDdaBeginEx(HANDLE hFile, DWORD flags, DWORD mask, unsigned __int32 lDistanceToMove,
    signed __int32 volume, signed int pan, int a7)
{
	DWORD bytestoread = SFileGetFileSize(hFile, 0);
	char *SFXbuffer = (char *)malloc(bytestoread);
	SFileReadFile(hFile, SFXbuffer, bytestoread, NULL, 0);

	SDL_RWops *rw = SDL_RWFromConstMem(SFXbuffer, bytestoread);
	SFileChunk = Mix_LoadWAV_RW(rw, 1);
	free(SFXbuffer);

	Mix_Volume(0, MIX_MAX_VOLUME - MIX_MAX_VOLUME * volume / VOLUME_MIN);
	int panned = 255 - 255 * abs(pan) / 10000;
	Mix_SetPanning(0, pan <= 0 ? 255 : panned, pan >= 0 ? 255 : panned);
	Mix_PlayChannel(0, SFileChunk, 0);

	return true;
}

BOOL SFileDdaDestroy()
{
	Mix_FreeChunk(SFileChunk);

	return true;
}

BOOL SFileDdaEnd(HANDLE hFile)
{
	Mix_HaltChannel(0);

	return true;
}

BOOL SFileDdaGetPos(HANDLE hFile, int *current, int *end)
{
	*current = 0;
	*end = 1;

	if (Mix_GetChunk(0) != SFileChunk || !Mix_Playing(0)) {
		*current = *end;
	}

	return true;
}

BOOL SFileDdaInitialize(HANDLE directsound)
{
	return true;
}

BOOL SFileDdaSetVolume(HANDLE hFile, signed int bigvolume, signed int volume)
{
	Mix_VolumeMusic(MIX_MAX_VOLUME - MIX_MAX_VOLUME * bigvolume / VOLUME_MIN);

	return true;
}

BOOL SFileGetFileArchive(HANDLE hFile, HANDLE *archive)
{
	UNIMPLEMENTED();
	return true;
}

// LONG SFileGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
// {
// 	UNIMPLEMENTED();
// }

// BOOL SFileOpenArchive(const char *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE
// *phMpq)
// {
// 	UNIMPLEMENTED();
// }

BOOL SFileOpenFile(const char *filename, HANDLE *phFile)
{
	//eprintf("%s: %s\n", __FUNCTION__, filename);

	BOOL result;

	result = patch_rt_mpq && SFileOpenFileEx((HANDLE)patch_rt_mpq, filename, 0, phFile);
	if (!result) {
		result = SFileOpenFileEx((HANDLE)diabdat_mpq, filename, 0, phFile);
	}

	if (!result || !*phFile) {
		eprintf("%s: Not found: %s\n", __FUNCTION__, filename);
	}
	return result;
}

// BOOL SFileOpenFileEx(HANDLE hMpq, const char *szFileName, DWORD dwSearchScope, HANDLE
// *phFile)
// {
// 	UNIMPLEMENTED();
// }

// BOOL SFileReadFile(HANDLE hFile, void *buffer, DWORD nNumberOfBytesToRead, DWORD *read,
// LONG lpDistanceToMoveHigh)
// {
// 	UNIMPLEMENTED();
// }

BOOL SBmpLoadImage(const char *pszFileName, PALETTEENTRY *pPalette, BYTE *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *dwHeight, DWORD *pdwBpp)
{
	HANDLE hFile;
	size_t size;
	PCXHeader pcxhdr;
	BYTE paldata[256][3];
	BYTE *dataPtr, *fileBuffer;
	BYTE byte;

	if (pdwWidth)
		*pdwWidth = 0;
	if (dwHeight)
		*dwHeight = 0;
	if (pdwBpp)
		*pdwBpp = 0;

	if (!pszFileName || !*pszFileName) {
		return false;
	}

	if (pBuffer && !dwBuffersize) {
		return false;
	}

	if (!pPalette && !pBuffer && !pdwWidth && !dwHeight) {
		return false;
	}

	if (!SFileOpenFile(pszFileName, &hFile)) {
		return false;
	}

	while (strchr(pszFileName, 92))
		pszFileName = strchr(pszFileName, 92) + 1;

	while (strchr(pszFileName + 1, 46))
		pszFileName = strchr(pszFileName, 46);

	// omit all types except PCX
	if (!pszFileName || _strcmpi(pszFileName, ".pcx")) {
		return false;
	}

	if (!SFileReadFile(hFile, &pcxhdr, 128, 0, 0)) {
		SFileCloseFile(hFile);
		return false;
	}

	int width = pcxhdr.xmax - pcxhdr.xmin + 1;
	int height = pcxhdr.ymax - pcxhdr.ymin + 1;

	if (pdwWidth)
		*pdwWidth = width;
	if (dwHeight)
		*dwHeight = height;
	if (pdwBpp)
		*pdwBpp = pcxhdr.bitsPerPixel;

	if (!pBuffer) {
		SFileSetFilePointer(hFile, 0, 0, 2);
		fileBuffer = NULL;
	} else {
		size = SFileGetFileSize(hFile, 0) - SFileSetFilePointer(hFile, 0, 0, 1);
		fileBuffer = (BYTE *)malloc(size);
	}

	if (fileBuffer) {
		SFileReadFile(hFile, fileBuffer, size, 0, 0);
		dataPtr = fileBuffer;

		for (int j = 0; j < height; j++) {
			for (int x = 0; x < width; dataPtr++) {
				byte = *dataPtr;
				if (byte < 0xC0) {
					*pBuffer = byte;
					pBuffer++;
					x++;
					continue;
				}
				dataPtr++;

				for (int i = 0; i < (byte & 0x3F); i++) {
					*pBuffer = *dataPtr;
					pBuffer++;
					x++;
				}
			}
		}

		free(fileBuffer);
	}

	if (pPalette && pcxhdr.bitsPerPixel == 8) {
		SFileSetFilePointer(hFile, -768, 0, 1);
		SFileReadFile(hFile, paldata, 768, 0, 0);
		for (int i = 0; i < 256; i++) {
			pPalette[i].peRed = paldata[i][0];
			pPalette[i].peGreen = paldata[i][1];
			pPalette[i].peBlue = paldata[i][2];
			pPalette[i].peFlags = 0;
		}
	}

	SFileCloseFile(hFile);

	return true;
}

// int SFileSetFilePointer(HANDLE, int, HANDLE, int)
// {
// 	UNIMPLEMENTED();
// }

HWND SDrawGetFrameWindow(HWND *sdraw_framewindow)
{
	DUMMY();
	return NULL;
}

// BOOL SDrawManualInitialize(HWND hWnd, LPDIRECTDRAW ddInterface, LPDIRECTDRAWSURFACE
// primarySurface, LPDIRECTDRAWSURFACE surface2, LPDIRECTDRAWSURFACE surface3, LPDIRECTDRAWSURFACE
// backSurface, LPDIRECTDRAWPALETTE ddPalette, HPALETTE hPalette)
//{
//	UNIMPLEMENTED();
//}

void *SMemAlloc(unsigned int amount, char *logfilename, int logline, char defaultValue)
{
	// fprintf(stderr, "%s: %d (%s:%d)\n", __FUNCTION__, amount, logfilename, logline);
	assert(amount != -1u);
	return malloc(amount);
}

BOOL SMemFree(void *location, char *logfilename, int logline, char defaultValue)
{
	// fprintf(stderr, "%s: (%s:%d)\n", __FUNCTION__, logfilename, logline);
	assert(location);
	free(location);
	return true;
}

void *SMemReAlloc(void *location, unsigned int amount, char *logfilename, int logline, char defaultValue)
{
	UNIMPLEMENTED();
}

bool getIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, int *dataSize = NULL)
{
	radon::Section *section = ini.getSection(sectionName);
	if (!section)
		return false;

	radon::Key *key = section->getKey(keyName);
	if (!key)
		return false;

	std::string value = key->getStringValue();
	if (dataSize)
		*dataSize = value.length();

	if (string)
		strncpy(string, value.c_str(), stringSize);

	return true;
}

void setIniValue(const char *sectionName, const char *keyName, char *value, int len = 0)
{
	radon::Section *section = ini.getSection(sectionName);
	if (!section) {
		ini.addSection(sectionName);
		section = ini.getSection(sectionName);
	}

	std::string stringValue(value, len ?: strlen(value));

	radon::Key *key = section->getKey(keyName);
	if (!key) {
		section->addKey(radon::Key(keyName, stringValue));
	} else {
		key->setValue(stringValue);
	}

	ini.saveToFile();
}

BOOL SRegLoadData(const char *keyname, const char *valuename, int size, LPBYTE lpData, BYTE flags, LPDWORD lpcbData)
{
	return getIniValue(keyname, valuename, (char *)lpData, size, (int *)lpcbData);
}

BOOL SRegLoadString(const char *keyname, const char *valuename, BYTE flags, char *buffer, unsigned int buffersize)
{
	return getIniValue(keyname, valuename, buffer, buffersize);
}

BOOL SRegLoadValue(const char *keyname, const char *valuename, BYTE flags, int *value)
{
	char string[10];
	if (getIniValue(keyname, valuename, string, 10)) {
		*value = strtol(string, NULL, 10);
		return true;
	}

	return false;
}

BOOL SRegSaveData(const char *keyname, const char *valuename, int size, BYTE *lpData, DWORD cbData)
{
	setIniValue(keyname, valuename, (char *)lpData, cbData);

	return true;
}

BOOL SRegSaveString(const char *keyname, const char *valuename, BYTE flags, char *string)
{
	setIniValue(keyname, valuename, string);

	return true;
}

BOOL SRegSaveValue(const char *keyname, const char *valuename, BYTE flags, DWORD result)
{
	char str[10];
	sprintf(str, "%d", result);
	setIniValue(keyname, valuename, str);

	return true;
}

BOOL SVidInitialize(HANDLE video)
{
	DUMMY();
	return true;
}

BOOL SVidDestroy()
{
	DUMMY();
	return true;
}

double SVidFrameEnd;
double SVidFrameLength;
BYTE SVidLoop;
smk SVidSMK;
PALETTEENTRY SVidPreviousPalette[256];
SDL_Palette *SVidPalette;
SDL_Surface *SVidSurface;
BYTE *SVidBuffer;
SDL_AudioDeviceID deviceId;

BOOL SVidPlayBegin(char *filename, int a2, int a3, int a4, int a5, int flags, HANDLE *video)
{
	if (flags & 0x10000 || flags & 0x20000000) {
		return false;
	}

	SVidLoop = flags & 0x40000;
	bool enableVideo = !(flags & 0x100000);
	bool enableAudio = !(flags & 0x1000000);
	//0x8 // Non-interlaced
	//0x200, 0x800 // Upscale video
	//0x80000 // Center horizontally
	//0x800000 // Edge detection
	//0x200800 // Clear FB

	SFileOpenFile(filename, video);

	int bytestoread = SFileGetFileSize(*video, 0);
	SVidBuffer = DiabloAllocPtr(bytestoread);
	SFileReadFile(*video, SVidBuffer, bytestoread, NULL, 0);

	SVidSMK = smk_open_memory(SVidBuffer, bytestoread);
	if (SVidSMK == NULL) {
		return false;
	}

	deviceId = 0;
	unsigned char channels[7], depth[7];
	unsigned long rate[7];
	smk_info_audio(SVidSMK, NULL, channels, depth, rate);
	if (enableAudio && depth[0] != 0) {
		smk_enable_audio(SVidSMK, 0, enableAudio);
		SDL_AudioSpec audioFormat;
		SDL_zero(audioFormat);
		audioFormat.freq = rate[0];
		audioFormat.format = depth[0] == 16 ? AUDIO_S16 : AUDIO_U8;
		audioFormat.channels = channels[0];

		deviceId = SDL_OpenAudioDevice(NULL, 0, &audioFormat, NULL, 0);
		if (deviceId == 0) {
			SDL_Log("SDL_OpenAudioDevice: %s\n", SDL_GetError());
			return false;
		} else {
			SDL_PauseAudioDevice(deviceId, 0); /* start audio playing. */
		}
	}

	unsigned long width, height, nFrames;
	smk_info_all(SVidSMK, NULL, &nFrames, &SVidFrameLength);
	smk_info_video(SVidSMK, &width, &height, NULL);

	smk_enable_video(SVidSMK, enableVideo);
	smk_first(SVidSMK); // Decode first frame

	smk_info_video(SVidSMK, &width, &height, NULL);
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	SDL_RenderSetLogicalSize(renderer, width, height);
	memcpy(SVidPreviousPalette, orig_palette, 1024);

	// Copy frame to buffer
	SVidSurface = SDL_CreateRGBSurfaceWithFormatFrom(
	    (unsigned char *)smk_get_video(SVidSMK),
	    width,
	    height,
	    8,
	    width,
	    SDL_PIXELFORMAT_INDEX8);

	SVidPalette = SDL_AllocPalette(256);
	if (SDL_SetSurfacePalette(SVidSurface, SVidPalette) != 0) {
		SDL_Log("SDL_SetSurfacePalette: %s\n", SDL_GetError());
		return false;
	}

	SVidFrameEnd = SDL_GetTicks() * 1000 + SVidFrameLength;

	return true;
}

BOOL SVidLoadNextFrame()
{
	SVidFrameEnd += SVidFrameLength;

	if (smk_next(SVidSMK) == SMK_DONE) {
		if (!SVidLoop) {
			return false;
		}

		smk_first(SVidSMK);
	}

	return true;
}

BOOL SVidPlayContinue(void)
{
	if (smk_palette_updated(SVidSMK)) {
		SDL_Color colors[256];
		const unsigned char *palette_data = smk_get_palette(SVidSMK);

		for (int i = 0; i < 256; i++) {
			colors[i].r = palette_data[i * 3 + 0];
			colors[i].g = palette_data[i * 3 + 1];
			colors[i].b = palette_data[i * 3 + 2];
			colors[i].a = SDL_ALPHA_OPAQUE;

			orig_palette[i].peFlags = 0;
			orig_palette[i].peRed = palette_data[i * 3 + 0];
			orig_palette[i].peGreen = palette_data[i * 3 + 1];
			orig_palette[i].peBlue = palette_data[i * 3 + 2];
		}
		memcpy(logical_palette, orig_palette, 1024);

		if (SDL_SetPaletteColors(SVidPalette, colors, 0, 256) != 0) {
			SDL_Log("SDL_SetPaletteColors: %s\n", SDL_GetError());
			return false;
		}
	}

	if (SDL_GetTicks() * 1000 >= SVidFrameEnd) {
		return SVidLoadNextFrame(); // Skip video and audio if the system is to slow
	}

	if (deviceId && SDL_QueueAudio(deviceId, smk_get_audio(SVidSMK, 0), smk_get_audio_size(SVidSMK, 0)) == -1) {
		SDL_Log("SDL_QueueAudio: %s\n", SDL_GetError());
		return false;
	}

	if (SDL_GetTicks() * 1000 >= SVidFrameEnd) {
		return SVidLoadNextFrame(); // Skip video if the system is to slow
	}

	SDL_Rect pal_surface_offset = { 64, 160, 0, 0 };
	if (SDL_BlitSurface(SVidSurface, NULL, pal_surface, &pal_surface_offset) != 0) {
		SDL_Log("SDL_BlitSurface: %s\n", SDL_GetError());
		return false;
	}

	SetFadeLevel(256); // present frame

	double now = SDL_GetTicks() * 1000;
	if (now < SVidFrameEnd) {
		SDL_Delay((SVidFrameEnd - now)/1000); // wait with next frame if the system is to fast
	}

	return SVidLoadNextFrame();
}

BOOL SVidPlayEnd(HANDLE video)
{
	if (deviceId) {
		SDL_ClearQueuedAudio(deviceId);
		SDL_CloseAudioDevice(deviceId);
		deviceId = 0;
	}

	if (SVidSMK)
		smk_close(SVidSMK);

	if (SVidBuffer) {
		mem_free_dbg(SVidBuffer);
		SVidBuffer = NULL;
	}

	SDL_FreePalette(SVidPalette);
	SDL_FreeSurface(SVidSurface);

	SFileCloseFile(video);
	video = NULL;

	memcpy(orig_palette, SVidPreviousPalette, 1024);
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	return true;
}

BOOL SErrDisplayError(DWORD dwErrMsg, const char *logfilename, int logline, const char *message,
    BOOL allowOption, int exitCode)
{
	UNIMPLEMENTED();
}

BOOL SErrGetErrorStr(DWORD dwErrCode, char *buffer, unsigned int bufferchars)
{
	DUMMY();
	return false;
}

DWORD SErrGetLastError()
{
	return nLastError;
}

void SErrSetLastError(DWORD dwErrCode)
{
	nLastError = dwErrCode;
}

void SMemCopy(void *dest, const void *source, unsigned int size)
{
	UNIMPLEMENTED();
}

void SMemFill(void *location, unsigned int length, char fillWith)
{
	UNIMPLEMENTED();
}

void SMemZero(void *location, DWORD length)
{
	UNIMPLEMENTED();
}

int SMemCmp(void *location1, void *location2, DWORD size)
{
	UNIMPLEMENTED();
}

int SStrCopy(char *dest, const char *src, int max_length)
{
	strncpy(dest, src, max_length);
	return strlen(dest);
}

int SStrCmp(const char *string1, const char *string2, unsigned int size)
{
	UNIMPLEMENTED();
}

int SStrCmpI(const char *string1, const char *string2, unsigned int size)
{
	UNIMPLEMENTED();
}

void SDrawMessageBox(char *Text, char *Title, int Flags)
{
	MessageBoxA(NULL, Text, Title, Flags);
}

// void SDrawDestroy(void)
//{
//	UNIMPLEMENTED();
//}

BOOLEAN StormDestroy(void)
{
	DUMMY();
	return true;
}

BOOLEAN SFileSetBasePath(char *)
{
	DUMMY();
	return true;
}

void SDrawRealizePalette(void)
{
	DUMMY();
}

BOOL SFileEnableDirectAccess(BOOL enable)
{
	DUMMY();
	return true;
}

}
