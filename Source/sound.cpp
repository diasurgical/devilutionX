//HEADER_GOES_HERE

#include "../types.h"

float sound_cpp_init_value;

#ifndef NO_GLOBALS
IDirectSoundBuffer *DSBs[8];
IDirectSound *sglpDS;
char gbSndInited;
int sglMusicVolume;
int sglSoundVolume;
HMODULE hDsound_dll; // idb
void *sgpMusicTrack;
IDirectSoundBuffer *sglpDSB;
#endif

const int sound_inf = 0x7F800000; // weak

/* data */

UCHAR gbMusicOn = 1; // weak
UCHAR gbSoundOn = 1; // weak
char gbDupSounds = 1; // weak
int sgnMusicTrack = 6;
char *sgszMusicTracks[6] =
{
  "Music\\DTowne.wav",
  "Music\\DLvlA.wav",
  "Music\\DLvlB.wav",
  "Music\\DLvlC.wav",
  "Music\\DLvlD.wav",
  "Music\\Dintro.wav"
};
char unk_volume[4][2] =
{
	{ 15, -16 },
	{ 15, -16 },
	{ 30, -31 },
	{ 30, -31 }
};

struct sound_cpp_init
{
	sound_cpp_init()
	{
		sound_cpp_init_value = sound_inf;
	}
} _sound_cpp_init;
// 47F24C: using guessed type int sound_inf;

void __fastcall snd_update(bool bStopAll)
{
	BOOL v1; // edi
	unsigned int v2; // esi
	IDirectSoundBuffer *v3; // eax
	unsigned long v4; // [esp+8h] [ebp-4h]

	v1 = bStopAll;
	v2 = 0;
	do
	{
		v3 = DSBs[v2];
		if ( v3 && (v1 || v3->GetStatus(&v4) || v4 != DSBSTATUS_PLAYING) ) // FIX_ME: double check
		{
			DSBs[v2]->Stop();
			DSBs[v2]->Release();
			DSBs[v2] = 0;
		}
		++v2;
	}
	while ( v2 < 8 );
}

void __fastcall snd_stop_snd(TSnd *pSnd)
{
	IDirectSoundBuffer *v1; // eax

	if ( pSnd )
	{
		v1 = pSnd->DSB;
		if ( v1 )
			v1->Stop();
	}
}

bool __fastcall snd_playing(TSnd *pSnd)
{
	IDirectSoundBuffer *v1; // eax
	bool result; // al
	unsigned long v3; // [esp+0h] [ebp-4h]

	v3 = (unsigned long)pSnd;
	if ( pSnd
	  && (v1 = pSnd->DSB) != 0
	  && !v1->GetStatus(&v3) )
	{
		result = v3 == DSBSTATUS_PLAYING;
	}
	else
	{
		result = 0;
	}
	return result;
}

void __fastcall snd_play_snd(TSnd *pSnd, int lVolume, int lPan)
{

		
	TSnd *v3; // edi
	int v4; // ebp
	IDirectSoundBuffer *v5; // esi
	//int v6; // eax
	int v7; // ebp
	int v8; // eax
	//int v9; // eax
	DWORD v10; // [esp+30h] [ebp-4h]

	v3 = pSnd;
	v4 = lVolume;
	if ( pSnd )
	{
		if ( gbSoundOn )
		{
			v5 = pSnd->DSB;
			if ( v5 )
			{
				v10 = GetTickCount();
				if ( v10 - v3->start_tc >= 0x50 )
				{
					//_LOBYTE(v6) = snd_playing(v3);
					if ( !snd_playing(v3) || (v5 = sound_dup_channel(v3->DSB)) != 0 )
					{
						v7 = sglSoundVolume + v4;
						if ( v7 >= -1600 )
						{
							if ( v7 > 0 )
								v7 = 0;
						}
						else
						{
							v7 = -1600;
						}
						v5->SetVolume(v7);
						v5->SetPan(lPan);
						v8 = v5->Play(0, 0, 0);
						if ( v8 == DSERR_BUFFERLOST )
						{
							//_LOBYTE(v9) = sound_file_reload(v3, v5);
							if ( sound_file_reload(v3, v5) )
								v5->Play(0, 0, 0);
						}
						else if ( v8 )
						{
							DSErrMsg(v8, 261, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
						}
						v3->start_tc = v10;
					}
				}
				else
				{
					GetTickCount();
				}
			}
		}
	}
}
// 4A22D5: using guessed type char gbSoundOn;

IDirectSoundBuffer *__fastcall sound_dup_channel(IDirectSoundBuffer *DSB)
{
	IDirectSoundBuffer *result; // eax
	IDirectSoundBuffer **v2; // esi

	result = 0;
	if ( gbDupSounds )
	{
		while ( DSBs[(_DWORD)result] )
		{
			result = (IDirectSoundBuffer *)((char *)result + 1); // result++
			if ( (unsigned int)result >= 8 )
				return 0;
		}
		v2 = &DSBs[(_DWORD)result];
		if ( sglpDS->DuplicateSoundBuffer(DSB, &DSBs[(_DWORD)result]) )
		{
			return 0;
		}
		result = *v2;
	}
	return result;
}
// 4A22D6: using guessed type char gbDupSounds;

bool __fastcall sound_file_reload(TSnd *sound_file, IDirectSoundBuffer *DSB)
{
	IDirectSoundBuffer *v2; // edi
	TSnd *v3; // esi
	char *v5; // ecx
	void *aptr2; // [esp+8h] [ebp-18h]
	unsigned long asize2; // [esp+Ch] [ebp-14h]
	bool v8; // [esp+10h] [ebp-10h]
	void *aptr1; // [esp+14h] [ebp-Ch]
	unsigned long asize1; // [esp+18h] [ebp-8h]
	void *a1; // [esp+1Ch] [ebp-4h]

	v2 = DSB;
	v3 = sound_file;
	if ( DSB->Restore() )
		return 0;
	v5 = v3->sound_path;
	v8 = 0;
	WOpenFile(v5, &a1, 0);
	WSetFilePointer(a1, v3->offset, 0, 0);
	if ( !v2->Lock(0, v3->len, &aptr1, &asize1, &aptr2, &asize2, 0) )
	{
		WReadFile(a1, (char *)aptr1, asize1);
		if ( !v2->Unlock(aptr1, asize1, aptr2, asize2) )
			v8 = 1;
	}
	WCloseFile(a1);
	return v8;
}

TSnd *__fastcall sound_file_load(char *path)
{
//	int v1; // esi
	char *v2; // edi
	TSnd *v4; // esi
	int v5; // eax
	int v6; // eax
//	int v7; // [esp-4h] [ebp-24h]
	void *aptr2; // [esp+8h] [ebp-18h]
	unsigned long asize2; // [esp+Ch] [ebp-14h]
	void *a1; // [esp+10h] [ebp-10h]
	void *ptr; // [esp+14h] [ebp-Ch]
	void *aptr1; // [esp+18h] [ebp-8h]
	unsigned long asize1; // [esp+1Ch] [ebp-4h]

	v2 = path;
	if ( !sglpDS )
		return 0;
//	v7 = v1;
	WOpenFile(path, &a1, 0);
	v4 = (TSnd *)DiabloAllocPtr(40);
	memset(v4, 0, 0x28u);
	v4->sound_path = v2;
	v4->start_tc = GetTickCount() - 81;
	ptr = LoadWaveFile(a1, &v4->fmt, &v4->len);
	if ( !ptr )
		TermMsg("Invalid sound format on file %s", v4->sound_path);
	sound_CreateSoundBuffer(v4);
	v5 = v4->DSB->Lock(0, v4->len, &aptr1, &asize1, &aptr2, &asize2, 0); //v7);
	if ( v5 )
		DSErrMsg(v5, 318, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
	memcpy(aptr1, (char *)ptr + v4->offset, asize1);
	v6 = v4->DSB->Unlock(aptr1, asize1, aptr2, asize2);
	if ( v6 )
		DSErrMsg(v6, 325, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
	mem_free_dbg(ptr);
	WCloseFile(a1);
	return v4;
}
// 456F07: could not find valid save-restore pair for esi

void __fastcall sound_CreateSoundBuffer(TSnd *sound_file)
{
	TSnd *v1; // esi
	int v2; // eax
	DSBUFFERDESC v3; // [esp+4h] [ebp-14h]

	v1 = sound_file;
	memset(&v3, 0, sizeof(DSBUFFERDESC));
	v3.dwBufferBytes = v1->len;
	v3.lpwfxFormat = &v1->fmt;
	v3.dwSize = sizeof(DSBUFFERDESC);
	v3.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_STATIC;
	v2 = sglpDS->CreateSoundBuffer(&v3, &v1->DSB, NULL);
	if ( v2 )
		DSErrMsg(v2, 282, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
}

void __fastcall sound_file_cleanup(TSnd *sound_file)
{
	TSnd *v1; // esi
	IDirectSoundBuffer *v2; // eax

	v1 = sound_file;
	if ( sound_file )
	{
		v2 = sound_file->DSB;
		if ( v2 )
		{
			sound_file->DSB->Stop();
			v1->DSB->Release();
			v1->DSB = 0;
		}
		mem_free_dbg(v1);
	}
}

void __fastcall snd_init(HWND hWnd)
{
	sound_load_volume("Sound Volume", &sglSoundVolume);
	gbSoundOn = sglSoundVolume > -1600;
	sound_load_volume("Music Volume", &sglMusicVolume);
	gbMusicOn = sglMusicVolume > -1600;
	if ( sound_DirectSoundCreate(NULL, &sglpDS, NULL) )
		sglpDS = 0;
	if ( sglpDS && !sglpDS->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE) )
		sound_create_primary_buffer(0);
	SVidInitialize(sglpDS);
	SFileDdaInitialize(sglpDS);
	gbSndInited = sglpDS != 0;
}
// 4A22D4: using guessed type char gbMusicOn;
// 4A22D5: using guessed type char gbSoundOn;

void __fastcall sound_load_volume(char *value_name, int *value)
{
	int *v2; // esi
	//int v3; // eax
	int v4; // ecx
	int valuea; // [esp+8h] [ebp-4h]

	v2 = value;
	valuea = *value;
	//_LOBYTE(v3) = SRegLoadValue("Diablo", value_name, 0, &valuea);
	if ( SRegLoadValue("Diablo", value_name, 0, &valuea) )
		v4 = valuea;
	else
		v4 = 0;
	*v2 = v4;
	if ( v4 >= -1600 )
	{
		if ( v4 > 0 )
			*v2 = 0;
	}
	else
	{
		*v2 = -1600;
	}
	*v2 -= *v2 % 100;
}

void __fastcall sound_create_primary_buffer(int music_track)
{
	int v1; // eax
	int v2; // eax
	DSCAPS v3; // [esp+4h] [ebp-8Ch]
	DSBUFFERDESC v4; // [esp+64h] [ebp-2Ch]
	int a1; // [esp+78h] [ebp-18h]
	WAVEFORMATEX a2; // [esp+7Ch] [ebp-14h]

	a1 = music_track;
	if ( !music_track )
	{
		memset(&v4, 0, sizeof(DSBUFFERDESC));
		v4.dwSize = sizeof(DSBUFFERDESC);
		v4.dwFlags = DSBCAPS_PRIMARYBUFFER;
		v1 = sglpDS->CreateSoundBuffer(&v4, &sglpDSB, NULL);
		if ( v1 )
			DSErrMsg(v1, 375, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
	}
	if ( sglpDSB )
	{
		v3.dwSize = 96;
		v2 = sglpDS->GetCaps(&v3);
		if ( v2 )
			DSErrMsg(v2, 383, "C:\\Src\\Diablo\\Source\\SOUND.CPP");
		if ( !a1 || !LoadWaveFormat((HANDLE)a1, &a2) )
		{
			memset(&a2, 0, 0x12u);
			a2.wFormatTag = WAVE_FORMAT_PCM;
			a2.nSamplesPerSec = 22050;
			a2.wBitsPerSample = 16;
			a2.cbSize = 0;
		}
		a2.nChannels = 2;
		a2.nBlockAlign = 2 * a2.wBitsPerSample / 8; /// BUGFIX: should be `a2.nChannels * a2.wBitsPerSample / 8`
		a2.nAvgBytesPerSec = a2.nSamplesPerSec * a2.nBlockAlign;
		sglpDSB->SetFormat(&a2);
	}
}
// 69F100: using guessed type int sglpDSB;

int __fastcall sound_DirectSoundCreate(GUID *guid, IDirectSound **DS, int always_null)
{
	IDirectSound **v3; // ebp
	int v4; // eax
	FARPROC v5; // ebx
	int v6; // eax
	GUID *v8; // [esp+10h] [ebp-4h]

	v3 = DS;
	v8 = guid;
	if ( !hDsound_dll )
	{
		hDsound_dll = LoadLibrary("dsound.dll");
		if ( !hDsound_dll )
		{
			v4 = GetLastError();
			ErrDlg(IDD_DIALOG5, v4, "C:\\Src\\Diablo\\Source\\SOUND.CPP", 422);
		}
	}
	v5 = GetProcAddress(hDsound_dll, "DirectSoundCreate");
	if ( !v5 )
	{
		v6 = GetLastError();
		ErrDlg(IDD_DIALOG5, v6, "C:\\Src\\Diablo\\Source\\SOUND.CPP", 427);
	}
	return ((int (__stdcall *)(GUID *, IDirectSound **, int))v5)(v8, v3, always_null);
}

void __cdecl sound_cleanup()
{
	snd_update(1);
	SVidDestroy();
	SFileDdaDestroy();
	if ( sglpDS )
	{
		sglpDS->Release();
		sglpDS = 0;
	}
	if ( gbSndInited )
	{
		gbSndInited = 0;
		sound_store_volume("Sound Volume", sglSoundVolume);
		sound_store_volume("Music Volume", sglMusicVolume);
	}
}

void __fastcall sound_store_volume(char *key, int value)
{
	SRegSaveValue("Diablo", key, 0, value);
}

void __cdecl music_stop()
{
	if ( sgpMusicTrack )
	{
		SFileDdaEnd(sgpMusicTrack);
		SFileCloseFile(sgpMusicTrack);
		sgpMusicTrack = 0;
		sgnMusicTrack = 6;
	}
}

void __fastcall music_start(int nTrack)
{
	//int v1; // esi
	//int v2; // eax
	//int v3; // edi

	//v1 = nTrack;
	music_stop();
	if ( sglpDS && gbMusicOn )
	{
		//_LOBYTE(v2) = SFileOpenFile(sgszMusicTracks[v1], &sgpMusicTrack);
		//v3 = v2;
		sound_create_primary_buffer((int)sgpMusicTrack);
		if ( SFileOpenFile(sgszMusicTracks[nTrack], &sgpMusicTrack) )
		{
			SFileDdaBeginEx(sgpMusicTrack, 0x40000, 0x40000, 0, sglMusicVolume, 0, 0);
			sgnMusicTrack = nTrack;
		}
		else
		{
			sgpMusicTrack = 0;
		}
	}
}
// 4A22D4: using guessed type char gbMusicOn;

void __fastcall sound_disable_music(bool disable)
{
	if ( disable )
	{
		music_stop();
	}
	else if ( sgnMusicTrack != 6 )
	{
		music_start(sgnMusicTrack);
	}
}

int __fastcall sound_get_or_set_music_volume(int volume)
{
	if ( volume != 1 )
	{
		sglMusicVolume = volume;
		if ( sgpMusicTrack )
			SFileDdaSetVolume(sgpMusicTrack, volume, 0);
	}
	return sglMusicVolume;
}

int __fastcall sound_get_or_set_sound_volume(int volume)
{
	int result; // eax

	result = volume;
	if ( volume == 1 )
		return sglSoundVolume;
	sglSoundVolume = volume;
	return result;
}
