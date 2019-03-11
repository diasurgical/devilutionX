/*
	win32_wasapi: audio output for Windows wasapi exclusive mode audio

	copyright ?-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org

	based on win32.c
*/
#define _WIN32_WINNT 0x601
#define COBJMACROS 1
#include "out123_int.h"
#include <initguid.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <avrt.h>
#include "debug.h"

#ifdef _MSC_VER

/* When compiling C code with MSVC it is only possible to declare, but not
   define the WASAPI interface GUIDs using MS headers. So we define them 
   ourselves. */

#ifndef GUID_SECT
#define GUID_SECT
#endif

#define __DEFINE_GUID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const GUID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __DEFINE_IID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const IID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __DEFINE_CLSID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const CLSID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define MPG123_DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __DEFINE_CLSID(mpg123_CLSID_##className, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
#define MPG123_DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __DEFINE_IID(mpg123_IID_##interfaceName, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)

// "1CB9AD4C-DBFA-4c32-B178-C2F568A703B2"
MPG123_DEFINE_IID(IAudioClient, 1cb9ad4c, dbfa, 4c32, b1, 78, c2, f5, 68, a7, 03, b2);
// "A95664D2-9614-4F35-A746-DE8DB63617E6"
MPG123_DEFINE_IID(IMMDeviceEnumerator, a95664d2, 9614, 4f35, a7, 46, de, 8d, b6, 36, 17, e6);
// "BCDE0395-E52F-467C-8E3D-C4579291692E"
MPG123_DEFINE_CLSID(IMMDeviceEnumerator, bcde0395, e52f, 467c, 8e, 3d, c4, 57, 92, 91, 69, 2e);
// "F294ACFC-3146-4483-A7BF-ADDCA7C260E2"
MPG123_DEFINE_IID(IAudioRenderClient, f294acfc, 3146, 4483, a7, bf, ad, dc, a7, c2, 60, e2);
#else
#define mpg123_IID_IAudioClient IID_IAudioClient
#define mpg123_IID_IMMDeviceEnumerator IID_IMMDeviceEnumerator
#define mpg123_CLSID_IMMDeviceEnumerator CLSID_MMDeviceEnumerator
#define mpg123_IID_IAudioRenderClient IID_IAudioRenderClient
#endif

/* Push mode does not work right yet, noisy audio, probably something to do with timing and buffers */
#define WASAPI_EVENT_MODE 1
#ifdef WASAPI_EVENT_MODE
#define Init_Flag AUDCLNT_STREAMFLAGS_EVENTCALLBACK
#define MOD_STRING "Experimental Audio output for Windows (wasapi event mode)."
#define BUFFER_TIME 20000000.0
#else
#define Init_Flag 0
#define MOD_STRING "Experimental Audio output for Windows (wasapi push mode)."
#define BUFFER_TIME 640000000.0
#endif

static int init_win32(out123_handle* ao);
static void flush_win32(out123_handle *ao);
/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"win32_wasapi",						
	/* description */	MOD_STRING,
	/* revision */		"$Rev:$",						
	/* handle */		NULL,
	
	/* init_output */	init_win32,
};

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

/* todo: move into handle struct */
typedef struct _wasapi_state_struct {
  IMMDeviceEnumerator *pEnumerator;
  IMMDevice *pDevice;
  IAudioClient *pAudioClient;
  IAudioRenderClient *pRenderClient;
  BYTE *pData;
  UINT32 bufferFrameCount;
  REFERENCE_TIME hnsRequestedDuration;
  HANDLE hEvent;
  HANDLE hTask;
  size_t pData_off;
  DWORD taskIndex;
  char is_playing;
  DWORD framesize;
} wasapi_state_struct;

/* setup endpoints */
static int open_win32(out123_handle *ao){
  HRESULT hr = 0;
  wasapi_state_struct *state;

  debug1("%s",__FUNCTION__);
  if(!ao || ao->userptr) return -1; /* userptr should really be null */
  state = calloc(sizeof(*state),1);
  if(!state) return -1;
  state->hnsRequestedDuration = REFTIMES_PER_SEC;
  ao->userptr = (void *)state;

  CoInitialize(NULL);
  hr = CoCreateInstance(&mpg123_CLSID_IMMDeviceEnumerator,NULL,CLSCTX_ALL, &mpg123_IID_IMMDeviceEnumerator,(void**)&state->pEnumerator);
  debug("CoCreateInstance");
  EXIT_ON_ERROR(hr)

  hr = IMMDeviceEnumerator_GetDefaultAudioEndpoint(state->pEnumerator,eRender, eConsole, &state->pDevice);
  debug("IMMDeviceEnumerator_GetDefaultAudioEndpoint");
  EXIT_ON_ERROR(hr)

  hr = IMMDeviceActivator_Activate(state->pDevice,
                  &mpg123_IID_IAudioClient, CLSCTX_ALL,
                  NULL, (void**)&state->pAudioClient);
  debug("IMMDeviceActivator_Activate");
  EXIT_ON_ERROR(hr)

  return 0;
  Exit:
  debug2("%s failed with %lx", __FUNCTION__, hr);
  return 1;
}

/*
  typedef struct tWAVEFORMATEX {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    WORD cbSize;

  } WAVEFORMATEX;
*/

static int formats_generator(const out123_handle * const ao, const int waveformat, WAVEFORMATEX *const format){
  DWORD bytes_per_sample = 0;
  WORD tag = WAVE_FORMAT_PCM;
  debug1("%s",__FUNCTION__);
  int ret = waveformat;
  switch(waveformat){
    case MPG123_ENC_SIGNED_8:
      bytes_per_sample = 1;
      break;
    case MPG123_ENC_FLOAT_32:
      tag = WAVE_FORMAT_IEEE_FLOAT;
    case MPG123_ENC_SIGNED_32:
      bytes_per_sample = 4;
      break;
    case MPG123_ENC_SIGNED_16:
      bytes_per_sample = 2;
      break;
    case MPG123_ENC_SIGNED_24:
      bytes_per_sample = 3;
      break;
    default:
      debug1("uh oh unknown %d",waveformat);
      ret = 0;
      break;
  }
  format->wFormatTag = tag;
  format->nChannels = ao->channels;
  format->nSamplesPerSec = ao->rate;
  format->nAvgBytesPerSec = ao->channels * bytes_per_sample * ao->rate;
  format->nBlockAlign = ao->channels * bytes_per_sample;
  format->wBitsPerSample = bytes_per_sample * 8;
  format->cbSize = 0;
  return ret;
}

/* check supported formats */
static int get_formats_win32(out123_handle *ao){
  /* PLEASE check with write_init and write_win32 buffer size calculation in case it is able to support something other than 16bit */
  HRESULT hr;
  int ret = 0;
  debug1("%s",__FUNCTION__);

  if(!ao || !ao->userptr) return -1;
  wasapi_state_struct *state = (wasapi_state_struct *) ao->userptr;
  debug2("channels %d, rate %ld",ao->channels, ao->rate);

  WAVEFORMATEX wf;

   if(ao->format & MPG123_ENC_SIGNED_8){
      formats_generator(ao,MPG123_ENC_SIGNED_8,&wf);
      if((hr = IAudioClient_IsFormatSupported(state->pAudioClient,AUDCLNT_SHAREMODE_EXCLUSIVE, &wf, NULL)) == S_OK)
      ret |= MPG123_ENC_SIGNED_8;
      if(hr == AUDCLNT_E_UNSUPPORTED_FORMAT) debug1("MPG123_ENC_SIGNED_8 %ld not supported", ao->rate);
   }

   if(ao->format & MPG123_ENC_SIGNED_16){
      formats_generator(ao,MPG123_ENC_SIGNED_16,&wf);
      if((hr = IAudioClient_IsFormatSupported(state->pAudioClient,AUDCLNT_SHAREMODE_EXCLUSIVE, &wf, NULL)) == S_OK)
      ret |= MPG123_ENC_SIGNED_16;
      if(hr == AUDCLNT_E_UNSUPPORTED_FORMAT) debug1("MPG123_ENC_SIGNED_16 %ld not supported", ao->rate);
   }

   if(ao->format & MPG123_ENC_SIGNED_32){
      formats_generator(ao,MPG123_ENC_SIGNED_32,&wf);
      if((hr = IAudioClient_IsFormatSupported(state->pAudioClient,AUDCLNT_SHAREMODE_EXCLUSIVE, &wf, NULL)) == S_OK)
      ret |= MPG123_ENC_SIGNED_32;
      if(hr == AUDCLNT_E_UNSUPPORTED_FORMAT) debug1("MPG123_ENC_SIGNED_32 %ld not supported", ao->rate);
   }

   if(ao->format & MPG123_ENC_FLOAT_32){
      formats_generator(ao,MPG123_ENC_FLOAT_32,&wf);
      if((hr = IAudioClient_IsFormatSupported(state->pAudioClient,AUDCLNT_SHAREMODE_EXCLUSIVE, &wf, NULL)) == S_OK)
      ret |= MPG123_ENC_FLOAT_32;
      if(hr == AUDCLNT_E_UNSUPPORTED_FORMAT) debug1("MPG123_ENC_FLOAT_32 %ld not supported", ao->rate);
   }

   if(ao->format & MPG123_ENC_SIGNED_24){
      formats_generator(ao,MPG123_ENC_SIGNED_24,&wf);
      if((hr = IAudioClient_IsFormatSupported(state->pAudioClient,AUDCLNT_SHAREMODE_EXCLUSIVE, &wf, NULL)) == S_OK)
      ret |= MPG123_ENC_SIGNED_24;
      if(hr == AUDCLNT_E_UNSUPPORTED_FORMAT) debug1("MPG123_ENC_SIGNED_24 %ld not supported", ao->rate);
   }

  return ret; /* afaik only 16bit 44.1kHz/48kHz has been known to work */
}

/* setup with agreed on format, for now only MPG123_ENC_SIGNED_16 */
static int write_init(out123_handle *ao){
  HRESULT hr;
  double offset = 0.5;

  debug1("%s",__FUNCTION__);
  if(!ao || !ao->userptr) return -1;
  wasapi_state_struct *state = (wasapi_state_struct *) ao->userptr;

  WAVEFORMATEX s16;
  formats_generator(ao,ao->format,&s16);
  state->framesize = s16.nBlockAlign;
  debug1("block size %ld", state->framesize);
  /* cargo cult code */
  hr = IAudioClient_GetDevicePeriod(state->pAudioClient,NULL, &state->hnsRequestedDuration);
  debug("IAudioClient_GetDevicePeriod OK");
  reinit:
  hr = IAudioClient_Initialize(state->pAudioClient,
                       AUDCLNT_SHAREMODE_EXCLUSIVE,
                       Init_Flag,
                       state->hnsRequestedDuration,
                       state->hnsRequestedDuration,
                       &s16,
                       NULL);
  debug("IAudioClient_Initialize OK");
  /* something about buffer sizes on Win7, fixme might loop forever */
  if (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED){
    if (offset > 10.0) goto Exit; /* is 10 enough to break out of the loop?*/
    debug("AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED");
    IAudioClient_GetBufferSize(state->pAudioClient,&state->bufferFrameCount);
    /* double buffered */
	state->hnsRequestedDuration = (REFERENCE_TIME)((BUFFER_TIME / s16.nSamplesPerSec * state->bufferFrameCount) + offset);
    offset += 0.5;
	IAudioClient_Release(state->pAudioClient);
	state->pAudioClient = NULL;
	hr = IMMDeviceActivator_Activate(state->pDevice,
                  &mpg123_IID_IAudioClient, CLSCTX_ALL,
                  NULL, (void**)&state->pAudioClient);
    debug("IMMDeviceActivator_Activate");
    goto reinit;
  }
  EXIT_ON_ERROR(hr)
  EXIT_ON_ERROR(hr)
  hr = IAudioClient_GetService(state->pAudioClient,
                        &mpg123_IID_IAudioRenderClient,
                        (void**)&state->pRenderClient);
  debug("IAudioClient_GetService OK");
  EXIT_ON_ERROR(hr)
#ifdef WASAPI_EVENT_MODE
  state->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  debug("CreateEvent OK");
  if(!state->hEvent) goto Exit;
  hr = IAudioClient_SetEventHandle(state->pAudioClient,state->hEvent);
  EXIT_ON_ERROR(hr);
#endif
  hr = IAudioClient_GetBufferSize(state->pAudioClient,&state->bufferFrameCount);
  debug("IAudioClient_GetBufferSize OK");
  EXIT_ON_ERROR(hr)
  return 0;
Exit:
  debug2("%s failed with %lx", __FUNCTION__, hr);
  return 1;
}

/* Set play mode if unset, also raise thread priority */
static HRESULT play_init(out123_handle *ao){
  HRESULT hr = S_OK;
  if(!ao || !ao->userptr) return -1;
  wasapi_state_struct *state = (wasapi_state_struct *) ao->userptr;
  if(!state->is_playing){
    debug1("%s",__FUNCTION__);
    state->hTask = AvSetMmThreadCharacteristicsW(L"Pro Audio", &state->taskIndex);
    hr = IAudioClient_Start(state->pAudioClient);
    state->is_playing = 1;
    debug("IAudioClient_Start");
    EXIT_ON_ERROR(hr)
    }
  return hr;
Exit:
  debug2("%s failed with %lx", __FUNCTION__, hr);
  return hr;
}

/* copy audio into IAudioRenderClient provided buffer */
static int write_win32(out123_handle *ao, unsigned char *buf, int len){
  HRESULT hr;
  size_t to_copy = 0;
  debug1("%s",__FUNCTION__);
  if(!ao || !ao->userptr) return -1;
  wasapi_state_struct *state = (wasapi_state_struct *) ao->userptr;
  if(!len) return 0;
  if(!state->pRenderClient) write_init(ao);
  size_t frames_in = len/state->framesize; /* Frames in buf, is framesize even correct? */
  debug("mode entered");
#ifdef WASAPI_EVENT_MODE
  /* Event mode WASAPI */
  DWORD retval = -1;
  int flag = 0; /* Silence flag */
  feed_again:
  if(!state->pData){
    /* Acquire buffer */
    hr = IAudioRenderClient_GetBuffer(state->pRenderClient,state->bufferFrameCount, &state->pData);
    debug("IAudioRenderClient_GetBuffer");
    EXIT_ON_ERROR(hr)
  }
  if(frames_in){ /* Did we get half a frame?? non-zero len smaller than framesize? */
    /* We must put in exactly the amount of frames specified by IAudioRenderClient_GetBuffer */
    while(state->pData_off < state->bufferFrameCount){
      to_copy = state->bufferFrameCount - state->pData_off;
      debug3("pData_off %I64d, bufferFrameCount %d, to_copy %I64d", state->pData_off, state->bufferFrameCount, to_copy);
      if(to_copy > frames_in){
        /* buf can fit in provided buffer space */
        debug1("all buffers copied, %I64d", frames_in);
        memcpy(state->pData+state->pData_off*state->framesize,buf,state->framesize*(frames_in));
        state->pData_off += frames_in;
        frames_in = 0;
        break;
      } else {
        /* buf too big, needs spliting */
        debug1("partial buffers %I64d", to_copy);
        memcpy(state->pData+state->pData_off*state->framesize,buf,state->framesize*(to_copy));
        state->pData_off += to_copy;
        buf+=(to_copy*state->framesize);
        frames_in -= to_copy;
      }
    }
  } else {
    /* In case we ever get half a frame, is it possible? */
    flag = AUDCLNT_BUFFERFLAGS_SILENT;
  }
  debug2("Copied %I64d, left %I64d", state->pData_off, frames_in);
  if(state->pData_off == state->bufferFrameCount) {
    /* Tell IAudioRenderClient that buffer is filled and released */
    hr = IAudioRenderClient_ReleaseBuffer(state->pRenderClient,state->pData_off, flag);
    state->pData_off = 0;
    state->pData = NULL;
    debug("IAudioRenderClient_ReleaseBuffer");
    EXIT_ON_ERROR(hr)
    if(!state->is_playing){
      hr = play_init(ao);
      EXIT_ON_ERROR(hr)
    }
    /* wait for next pull event */
    retval = WaitForSingleObject(state->hEvent, 2000);
    if (retval != WAIT_OBJECT_0){
      /* Event handle timed out after a 2-second wait, something went very wrong */
      IAudioClient_Stop(state->pAudioClient);
      hr = ERROR_TIMEOUT;
      goto Exit;
    }
  }
  if(frames_in > 0)
    goto feed_again;
#else /* PUSH mode code */
    UINT32 numFramesAvailable, numFramesPadding;
    debug1("block size %ld", state->framesize);
feed_again:
    /* How much buffer do we get to use? */
    hr = IAudioClient_GetBufferSize(state->pAudioClient,&state->bufferFrameCount);
    debug("IAudioRenderClient_GetBuffer");
    EXIT_ON_ERROR(hr)
    hr = IAudioClient_GetCurrentPadding(state->pAudioClient,&numFramesPadding);
    debug("IAudioClient_GetCurrentPadding");
    EXIT_ON_ERROR(hr)
    /* How much buffer is writable at the moment? */
    numFramesAvailable = state->bufferFrameCount - numFramesPadding;
    debug3("numFramesAvailable %d, bufferFrameCount %d, numFramesPadding %d", numFramesAvailable, state->bufferFrameCount, numFramesPadding);
    if(numFramesAvailable > frames_in){
      /* can fit all frames now */
      state->pData_off = 0;
      to_copy = frames_in;
    } else {
      /* copy whatever that fits in the buffer */
      state->pData_off = frames_in - numFramesAvailable;
      to_copy = numFramesAvailable;
    }
    /* Acquire buffer */
    hr = IAudioRenderClient_GetBuffer(state->pRenderClient,to_copy,&state->pData);
    debug("IAudioRenderClient_GetBuffer");
    EXIT_ON_ERROR(hr)
    memcpy(state->pData,buf+state->pData_off * state->framesize,to_copy*state->framesize);
    /* Release buffer */
    hr = IAudioRenderClient_ReleaseBuffer(state->pRenderClient,to_copy, 0);
    debug("IAudioRenderClient_ReleaseBuffer");
    EXIT_ON_ERROR(hr)
    if(!state->is_playing){
      hr = play_init(ao);
      EXIT_ON_ERROR(hr)
    }
    frames_in -= to_copy;
    /* Wait sometime for buffer to empty? */
    DWORD sleeptime = (DWORD)(state->hnsRequestedDuration/REFTIMES_PER_MILLISEC/ao->rate);
    debug1("Sleeping %ld msec", sleeptime);
    Sleep(sleeptime);
    if (frames_in)
      goto feed_again;
#endif
  return len;
  Exit:
  debug2("%s failed with %lx", __FUNCTION__, hr);
  return -1;
}

static void flush_win32(out123_handle *ao){
  /* Wait for the last buffer to play before stopping. */
  debug1("%s",__FUNCTION__);
  if(!ao || !ao->userptr) return;
  wasapi_state_struct *state = (wasapi_state_struct *) ao->userptr;
  HRESULT hr;
  if(!state->pAudioClient) return;
  state->pData = NULL;
  hr = IAudioClient_Stop(state->pAudioClient);
  EXIT_ON_ERROR(hr)
  IAudioClient_Reset(state->pAudioClient);
  EXIT_ON_ERROR(hr)
  return;
  Exit:
  debug2("%s IAudioClient_Stop with %lx", __FUNCTION__, hr);
}

static int close_win32(out123_handle *ao)
{
  debug1("%s",__FUNCTION__);
  if(!ao || !ao->userptr) return -1;
  wasapi_state_struct *state = (wasapi_state_struct *) ao->userptr;
#ifdef WASAPI_EVENT_MODE
  if(state->pData){
    /* Play all in buffer before closing */
    debug("Flushing remaining buffers");
    IAudioRenderClient_ReleaseBuffer(state->pRenderClient,state->bufferFrameCount, 0);
    WaitForSingleObject(state->hEvent, 2000);
    state->pData = NULL;
  }
#endif
  if(state->pAudioClient) IAudioClient_Stop(state->pAudioClient);
  if(state->pRenderClient) IAudioRenderClient_Release(state->pRenderClient);  
  if(state->pAudioClient) IAudioClient_Release(state->pAudioClient);
  if(state->hTask) AvRevertMmThreadCharacteristics(state->hTask);
  if(state->pEnumerator) IMMDeviceEnumerator_Release(state->pEnumerator);
  if(state->pDevice) IMMDevice_Release(state->pDevice);
  CoUninitialize();
  free(state);
  ao->userptr = NULL;
  return 0;
}

static int init_win32(out123_handle* ao){
    debug1("%s",__FUNCTION__);
	if(!ao) return -1;

	/* Set callbacks */
	ao->open = open_win32;
	ao->flush = flush_win32;
	ao->write = write_win32;
	ao->get_formats = get_formats_win32;
	ao->close = close_win32;
    ao->userptr = NULL;

	/* Success */
	return 0;
}
