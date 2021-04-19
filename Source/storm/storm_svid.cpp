#include "storm/storm_svid.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <SDL.h>
#include <SDL_mixer.h>
#include <smacker.h>

#include "dx.h"
#include "options.h"
#include "palette.h"
#include "storm/storm.h"
#include "utils/display.h"
#include "utils/sdl_compat.h"

#if !SDL_VERSION_ATLEAST(2, 0, 4)
#include <queue>
#endif

namespace devilution {
namespace {

unsigned long SVidWidth, SVidHeight;
double SVidFrameEnd;
double SVidFrameLength;
char SVidAudioDepth;
double SVidVolume;
BYTE SVidLoop;
smk SVidSMK;
SDL_Color SVidPreviousPalette[256];
SDL_Palette *SVidPalette;
SDL_Surface *SVidSurface;
BYTE *SVidBuffer;

bool IsLandscapeFit(unsigned long src_w, unsigned long src_h, unsigned long dst_w, unsigned long dst_h)
{
	return src_w * dst_h > dst_w * src_h;
}

#ifdef USE_SDL1
// Whether we've changed the video mode temporarily for SVid.
// If true, we must restore it once the video has finished playing.
bool IsSVidVideoMode = false;

// Set the video mode close to the SVid resolution while preserving aspect ratio.
void TrySetVideoModeToSVidForSDL1()
{
	const SDL_Surface *display = SDL_GetVideoSurface();
	IsSVidVideoMode = (display->flags & (SDL_FULLSCREEN | SDL_NOFRAME)) != 0;
	if (!IsSVidVideoMode)
		return;

	int w;
	int h;
	if (IsLandscapeFit(SVidWidth, SVidHeight, display->w, display->h)) {
		w = SVidWidth;
		h = SVidWidth * display->h / display->w;
	} else {
		w = SVidHeight * display->w / display->h;
		h = SVidHeight;
	}

#ifdef SDL1_FORCE_SVID_VIDEO_MODE
	const bool video_mode_ok = true;
#else
	const bool video_mode_ok = SDL_VideoModeOK(
	    w, h, /*bpp=*/display->format->BitsPerPixel, display->flags);
#endif

	if (!video_mode_ok) {
		IsSVidVideoMode = false;

		// Get available fullscreen/hardware modes
		SDL_Rect **modes = SDL_ListModes(nullptr, display->flags);

		// Check is there are any modes available.
		if (modes == reinterpret_cast<SDL_Rect **>(0)
		    || modes == reinterpret_cast<SDL_Rect **>(-1)) {
			return;
		}

		// Search for a usable video mode
		bool found = false;
		for (int i = 0; modes[i]; i++) {
			if (modes[i]->w == w || modes[i]->h == h) {
				found = true;
				break;
			}
		}
		if (!found)
			return;
		IsSVidVideoMode = true;
	}

	SetVideoMode(w, h, display->format->BitsPerPixel, display->flags);
}
#endif

#if !SDL_VERSION_ATLEAST(2, 0, 4)
bool HaveAudio()
{
	return SDL_GetAudioStatus() != SDL_AUDIO_STOPPED;
}

struct AudioQueueItem {
	unsigned char *data;
	unsigned long len;
	const unsigned char *pos;
};

class AudioQueue {
public:
	static void Callback(void *userdata, Uint8 *out, int out_len)
	{
		static_cast<AudioQueue *>(userdata)->Dequeue(out, out_len);
	}

	void Subscribe(SDL_AudioSpec *spec)
	{
		spec->userdata = this;
		spec->callback = AudioQueue::Callback;
	}

	void Enqueue(const unsigned char *data, unsigned long len)
	{
#if SDL_VERSION_ATLEAST(2, 0, 4)
		SDL_LockAudioDevice(deviceId);
		EnqueueUnsafe(data, len);
		SDL_UnlockAudioDevice(deviceId);
#else
		SDL_LockAudio();
		EnqueueUnsafe(data, len);
		SDL_UnlockAudio();
#endif
	}

	void Clear()
	{
		while (!queue_.empty())
			Pop();
	}

private:
	void EnqueueUnsafe(const unsigned char *data, unsigned long len)
	{
		AudioQueueItem item;
		item.data = new unsigned char[len];
		memcpy(item.data, data, len * sizeof(item.data[0]));
		item.len = len;
		item.pos = item.data;
		queue_.push(item);
	}

	void Dequeue(Uint8 *out, int out_len)
	{
		SDL_memset(out, 0, sizeof(out[0]) * out_len);
		AudioQueueItem *item;
		while ((item = Next()) != NULL) {
			if (static_cast<unsigned long>(out_len) <= item->len) {
				SDL_MixAudio(out, item->pos, out_len, SDL_MIX_MAXVOLUME);
				item->pos += out_len;
				item->len -= out_len;
				return;
			}

			SDL_MixAudio(out, item->pos, item->len, SDL_MIX_MAXVOLUME);
			out += item->len;
			out_len -= item->len;
			Pop();
		}
	}

	AudioQueueItem *Next()
	{
		while (!queue_.empty() && queue_.front().len == 0)
			Pop();
		if (queue_.empty())
			return NULL;
		return &queue_.front();
	}

	void Pop()
	{
		delete[] queue_.front().data;
		queue_.pop();
	}

	std::queue<AudioQueueItem> queue_;
};

AudioQueue *sVidAudioQueue = new AudioQueue();
#else // SDL_VERSION_ATLEAST(2, 0, 4)
SDL_AudioDeviceID deviceId;
bool HaveAudio()
{
	return deviceId != 0;
}
#endif

void SVidRestartMixer()
{
	if (Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 1024) < 0) {
		SDL_Log("%s", Mix_GetError());
	}
	Mix_AllocateChannels(25);
	Mix_ReserveChannels(1);
}

unsigned char *SVidApplyVolume(const unsigned char *raw, unsigned long rawLen)
{
	auto *scaled = (unsigned char *)malloc(rawLen);

	if (SVidAudioDepth == 16) {
		for (unsigned long i = 0; i < rawLen / 2; i++)
			((Sint16 *)scaled)[i] = ((Sint16 *)raw)[i] * SVidVolume;
	} else {
		for (unsigned long i = 0; i < rawLen; i++)
			scaled[i] = raw[i] * SVidVolume;
	}

	return (unsigned char *)scaled;
}

bool SVidLoadNextFrame()
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

} // namespace

void SVidPlayBegin(const char *filename, int flags, HANDLE *video)
{
	if (flags & 0x10000 || flags & 0x20000000) {
		return;
	}

	SVidLoop = false;
	if (flags & 0x40000)
		SVidLoop = true;
	bool enableVideo = !(flags & 0x100000);
	bool enableAudio = !(flags & 0x1000000);
	//0x8 // Non-interlaced
	//0x200, 0x800 // Upscale video
	//0x80000 // Center horizontally
	//0x800000 // Edge detection
	//0x200800 // Clear FB

	SFileOpenFile(filename, video);

	int bytestoread = SFileGetFileSize(*video, nullptr);
	SVidBuffer = DiabloAllocPtr(bytestoread);
	SFileReadFile(*video, SVidBuffer, bytestoread, nullptr, nullptr);

	SVidSMK = smk_open_memory(SVidBuffer, bytestoread);
	if (SVidSMK == nullptr) {
		return;
	}

	unsigned char channels[7], depth[7];
	unsigned long rate[7];
	smk_info_audio(SVidSMK, nullptr, channels, depth, rate);
	if (enableAudio && depth[0] != 0) {
		smk_enable_audio(SVidSMK, 0, enableAudio);
		SDL_AudioSpec audioFormat;
		SDL_zero(audioFormat);
		audioFormat.freq = rate[0];
		audioFormat.format = depth[0] == 16 ? AUDIO_S16SYS : AUDIO_U8;
		SVidAudioDepth = depth[0];
		audioFormat.channels = channels[0];

		SVidVolume = sgOptions.Audio.nSoundVolume - VOLUME_MIN;
		SVidVolume /= -VOLUME_MIN;

		Mix_CloseAudio();

#if SDL_VERSION_ATLEAST(2, 0, 4)
		deviceId = SDL_OpenAudioDevice(nullptr, 0, &audioFormat, nullptr, 0);
		if (deviceId == 0) {
			ErrSdl();
		}

		SDL_PauseAudioDevice(deviceId, 0); /* start audio playing. */
#else
		sVidAudioQueue->Subscribe(&audioFormat);
		if (SDL_OpenAudio(&audioFormat, NULL) != 0) {
			ErrSdl();
		}
		SDL_PauseAudio(0);
#endif
	}

	unsigned long nFrames;
	smk_info_all(SVidSMK, nullptr, &nFrames, &SVidFrameLength);
	smk_info_video(SVidSMK, &SVidWidth, &SVidHeight, nullptr);

	smk_enable_video(SVidSMK, enableVideo);
	smk_first(SVidSMK); // Decode first frame

	smk_info_video(SVidSMK, &SVidWidth, &SVidHeight, nullptr);
#ifndef USE_SDL1
	if (renderer) {
		SDL_DestroyTexture(texture);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SVidWidth, SVidHeight);
		if (texture == nullptr) {
			ErrSdl();
		}
		if (SDL_RenderSetLogicalSize(renderer, SVidWidth, SVidHeight) <= -1) {
			ErrSdl();
		}
	}
#else
	TrySetVideoModeToSVidForSDL1();
#endif
	std::memcpy(SVidPreviousPalette, orig_palette, sizeof(SVidPreviousPalette));

	// Copy frame to buffer
	SVidSurface = SDL_CreateRGBSurfaceWithFormatFrom(
	    (unsigned char *)smk_get_video(SVidSMK),
	    SVidWidth,
	    SVidHeight,
	    8,
	    SVidWidth,
	    SDL_PIXELFORMAT_INDEX8);
	if (SVidSurface == nullptr) {
		ErrSdl();
	}

	SVidPalette = SDL_AllocPalette(256);
	if (SVidPalette == nullptr) {
		ErrSdl();
	}
	if (SDLC_SetSurfaceColors(SVidSurface, SVidPalette) <= -1) {
		ErrSdl();
	}

	SVidFrameEnd = SDL_GetTicks() * 1000 + SVidFrameLength;
	SDL_FillRect(GetOutputSurface(), nullptr, 0x000000);
}

bool SVidPlayContinue()
{
	if (smk_palette_updated(SVidSMK)) {
		SDL_Color colors[256];
		const unsigned char *palette_data = smk_get_palette(SVidSMK);

		for (int i = 0; i < 256; i++) {
			colors[i].r = palette_data[i * 3 + 0];
			colors[i].g = palette_data[i * 3 + 1];
			colors[i].b = palette_data[i * 3 + 2];
#ifndef USE_SDL1
			colors[i].a = SDL_ALPHA_OPAQUE;
#endif

			orig_palette[i].r = palette_data[i * 3 + 0];
			orig_palette[i].g = palette_data[i * 3 + 1];
			orig_palette[i].b = palette_data[i * 3 + 2];
		}
		memcpy(logical_palette, orig_palette, sizeof(logical_palette));

		if (SDLC_SetSurfaceAndPaletteColors(SVidSurface, SVidPalette, colors, 0, 256) <= -1) {
			SDL_Log("%s", SDL_GetError());
			return false;
		}
	}

	if (SDL_GetTicks() * 1000 >= SVidFrameEnd) {
		return SVidLoadNextFrame(); // Skip video and audio if the system is to slow
	}

	if (HaveAudio()) {
		unsigned long len = smk_get_audio_size(SVidSMK, 0);
		unsigned char *audio = SVidApplyVolume(smk_get_audio(SVidSMK, 0), len);
#if SDL_VERSION_ATLEAST(2, 0, 4)
		if (SDL_QueueAudio(deviceId, audio, len) <= -1) {
			SDL_Log("%s", SDL_GetError());
			return false;
		}
#else
		sVidAudioQueue->Enqueue(audio, len);
#endif
		free(audio);
	}

	if (SDL_GetTicks() * 1000 >= SVidFrameEnd) {
		return SVidLoadNextFrame(); // Skip video if the system is to slow
	}

#ifndef USE_SDL1
	if (renderer) {
		if (SDL_BlitSurface(SVidSurface, nullptr, GetOutputSurface(), nullptr) <= -1) {
			SDL_Log("%s", SDL_GetError());
			return false;
		}
	} else
#endif
	{
		SDL_Surface *output_surface = GetOutputSurface();
#ifdef USE_SDL1
		const bool is_indexed_output_format = SDLBackport_IsPixelFormatIndexed(output_surface->format);
#else
		const Uint32 wnd_format = SDL_GetWindowPixelFormat(ghMainWnd);
		const bool is_indexed_output_format = SDL_ISPIXELFORMAT_INDEXED(wnd_format);
#endif
		SDL_Rect output_rect;
		if (is_indexed_output_format) {
			// Cannot scale if the output format is indexed (8-bit palette).
			output_rect.w = static_cast<int>(SVidWidth);
			output_rect.h = static_cast<int>(SVidHeight);
		} else if (IsLandscapeFit(SVidWidth, SVidHeight, output_surface->w, output_surface->h)) {
			output_rect.w = output_surface->w;
			output_rect.h = SVidHeight * output_surface->w / SVidWidth;
		} else {
			output_rect.w = SVidWidth * output_surface->h / SVidHeight;
			output_rect.h = output_surface->h;
		}
		output_rect.x = (output_surface->w - output_rect.w) / 2;
		output_rect.y = (output_surface->h - output_rect.h) / 2;

		if (is_indexed_output_format
		    || output_surface->w == static_cast<int>(SVidWidth)
		    || output_surface->h == static_cast<int>(SVidHeight)) {
			if (SDL_BlitSurface(SVidSurface, nullptr, output_surface, &output_rect) <= -1) {
				ErrSdl();
			}
		} else {
			// The source surface is always 8-bit, and the output surface is never 8-bit in this branch.
			// We must convert to the output format before calling SDL_BlitScaled.
#ifdef USE_SDL1
			SDLSurfaceUniquePtr converted { SDL_ConvertSurface(SVidSurface, ghMainWnd->format, 0) };
#else
			SDLSurfaceUniquePtr converted { SDL_ConvertSurfaceFormat(SVidSurface, wnd_format, 0) };
#endif
			if (SDL_BlitScaled(converted.get(), nullptr, output_surface, &output_rect) <= -1) {
				SDL_Log("%s", SDL_GetError());
				return false;
			}
		}
	}

	RenderPresent();

	double now = SDL_GetTicks() * 1000;
	if (now < SVidFrameEnd) {
		SDL_Delay((SVidFrameEnd - now) / 1000); // wait with next frame if the system is too fast
	}

	return SVidLoadNextFrame();
}

void SVidPlayEnd(HANDLE video)
{
	if (HaveAudio()) {
#if SDL_VERSION_ATLEAST(2, 0, 4)
		SDL_ClearQueuedAudio(deviceId);
		SDL_CloseAudioDevice(deviceId);
		deviceId = 0;
#else
		SDL_CloseAudio();
		sVidAudioQueue->Clear();
#endif
		SVidRestartMixer();
	}

	if (SVidSMK)
		smk_close(SVidSMK);

	if (SVidBuffer) {
		mem_free_dbg(SVidBuffer);
		SVidBuffer = nullptr;
	}

	SDL_FreePalette(SVidPalette);
	SVidPalette = nullptr;

	SDL_FreeSurface(SVidSurface);
	SVidSurface = nullptr;

	SFileCloseFile(video);
	video = nullptr;

	memcpy(orig_palette, SVidPreviousPalette, sizeof(orig_palette));
#ifndef USE_SDL1
	if (renderer) {
		SDL_DestroyTexture(texture);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, gnScreenWidth, gnScreenHeight);
		if (texture == nullptr) {
			ErrSdl();
		}
		if (renderer && SDL_RenderSetLogicalSize(renderer, gnScreenWidth, gnScreenHeight) <= -1) {
			ErrSdl();
		}
	}
#else
	if (IsSVidVideoMode) {
		SetVideoModeToPrimary(IsFullScreen(), gnScreenWidth, gnScreenHeight);
		IsSVidVideoMode = false;
	}
#endif
}

} // namespace devilution
