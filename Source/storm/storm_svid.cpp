#include "storm/storm_svid.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <SDL.h>
#include <smacker.h>

#ifndef NOSOUND
#include <Aulib/Stream.h>
#include <Aulib/ResamplerSpeex.h>

#include "utils/push_aulib_decoder.h"
#endif

#include "dx.h"
#include "options.h"
#include "palette.h"
#include "storm/storm_file_wrapper.h"
#include "storm/storm.h"
#include "utils/display.h"
#include "utils/log.hpp"
#include "utils/sdl_compat.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {
namespace {

#ifndef NOSOUND
std::optional<Aulib::Stream> SVidAudioStream;
PushAulibDecoder *SVidAudioDecoder;
std::uint8_t SVidAudioDepth;
#endif

unsigned long SVidWidth, SVidHeight;
double SVidFrameEnd;
double SVidFrameLength;
bool SVidLoop;
smk SVidSMK;
SDL_Color SVidPreviousPalette[256];
SDL_Palette *SVidPalette;
SDL_Surface *SVidSurface;

#ifndef DEVILUTIONX_STORM_FILE_WRAPPER_AVAILABLE
std::unique_ptr<uint8_t[]> SVidBuffer;
#endif

bool IsLandscapeFit(unsigned long srcW, unsigned long srcH, unsigned long dstW, unsigned long dstH)
{
	return srcW * dstH > dstW * srcH;
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

#ifndef NOSOUND
bool HaveAudio()
{
	return SVidAudioStream && SVidAudioStream->isPlaying();
}
#endif

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

bool SVidPlayBegin(const char *filename, int flags, HANDLE *video)
{
	if ((flags & 0x10000) != 0 || (flags & 0x20000000) != 0) {
		return false;
	}

	SVidLoop = false;
	if ((flags & 0x40000) != 0)
		SVidLoop = true;
	bool enableVideo = (flags & 0x100000) == 0;
	//0x8 // Non-interlaced
	//0x200, 0x800 // Upscale video
	//0x80000 // Center horizontally
	//0x800000 // Edge detection
	//0x200800 // Clear FB

	SFileOpenFile(filename, video);
#ifdef DEVILUTIONX_STORM_FILE_WRAPPER_AVAILABLE
	FILE *file = FILE_FromStormHandle(*video);
	SVidSMK = smk_open_filepointer(file, SMK_MODE_DISK);
#else
	int bytestoread = SFileGetFileSize(*video);
	SVidBuffer = std::unique_ptr<uint8_t[]> { new uint8_t[bytestoread] };
	SFileReadFileThreadSafe(*video, SVidBuffer.get(), bytestoread);
	SFileCloseFileThreadSafe(*video);
	*video = nullptr;
	SVidSMK = smk_open_memory(SVidBuffer.get(), bytestoread);
#endif
	if (SVidSMK == nullptr) {
		return false;
	}

#ifndef NOSOUND
	const bool enableAudio = (flags & 0x1000000) == 0;

	constexpr std::size_t MaxSmkChannels = 7;
	unsigned char channels[MaxSmkChannels];
	unsigned char depth[MaxSmkChannels];
	unsigned long rate[MaxSmkChannels]; // NOLINT(google-runtime-int): Match `smk_info_audio` signature.
	smk_info_audio(SVidSMK, nullptr, channels, depth, rate);
	LogVerbose(LogCategory::Audio, "SVid audio depth={} channels={} rate={}", depth[0], channels[0], rate[0]);

	if (enableAudio && depth[0] != 0) {
		sound_stop(); // Stop in-progress music and sound effects

		smk_enable_audio(SVidSMK, 0, enableAudio ? 1 : 0);
		SVidAudioDepth = depth[0];
		auto decoder = std::make_unique<PushAulibDecoder>(channels[0], rate[0]);
		SVidAudioDecoder = decoder.get();
		SVidAudioStream.emplace(/*rwops=*/nullptr, std::move(decoder),
		    std::make_unique<Aulib::ResamplerSpeex>(sgOptions.Audio.nResamplingQuality), /*closeRw=*/false);
		const float volume = static_cast<float>(sgOptions.Audio.nSoundVolume - VOLUME_MIN) / -VOLUME_MIN;
		SVidAudioStream->setVolume(volume);
		if (!SVidAudioStream->open()) {
			LogError(LogCategory::Audio, "Aulib::Stream::open (from SVidPlayBegin): {}", SDL_GetError());
			SVidAudioStream = std::nullopt;
			SVidAudioDecoder = nullptr;
		}
		if (!SVidAudioStream->play()) {
			LogError(LogCategory::Audio, "Aulib::Stream::play (from SVidPlayBegin): {}", SDL_GetError());
			SVidAudioStream = std::nullopt;
			SVidAudioDecoder = nullptr;
		}
	}
#endif

	unsigned long nFrames;
	smk_info_all(SVidSMK, nullptr, &nFrames, &SVidFrameLength);
	smk_info_video(SVidSMK, &SVidWidth, &SVidHeight, nullptr);

	smk_enable_video(SVidSMK, enableVideo ? 1 : 0);
	smk_first(SVidSMK); // Decode first frame

	smk_info_video(SVidSMK, &SVidWidth, &SVidHeight, nullptr);
#ifndef USE_SDL1
	if (renderer != nullptr) {
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
	return true;
}

bool SVidPlayContinue()
{
	if (smk_palette_updated(SVidSMK) != 0) {
		SDL_Color colors[256];
		const unsigned char *paletteData = smk_get_palette(SVidSMK);

		for (int i = 0; i < 256; i++) {
			colors[i].r = paletteData[i * 3 + 0];
			colors[i].g = paletteData[i * 3 + 1];
			colors[i].b = paletteData[i * 3 + 2];
#ifndef USE_SDL1
			colors[i].a = SDL_ALPHA_OPAQUE;
#endif

			orig_palette[i].r = paletteData[i * 3 + 0];
			orig_palette[i].g = paletteData[i * 3 + 1];
			orig_palette[i].b = paletteData[i * 3 + 2];
		}
		memcpy(logical_palette, orig_palette, sizeof(logical_palette));

		if (SDLC_SetSurfaceAndPaletteColors(SVidSurface, SVidPalette, colors, 0, 256) <= -1) {
			Log("{}", SDL_GetError());
			return false;
		}
	}

	if (SDL_GetTicks() * 1000 >= SVidFrameEnd) {
		return SVidLoadNextFrame(); // Skip video and audio if the system is to slow
	}

#ifndef NOSOUND
	if (HaveAudio()) {
		const auto len = smk_get_audio_size(SVidSMK, 0);
		const unsigned char *buf = smk_get_audio(SVidSMK, 0);
		if (SVidAudioDepth == 16) {
			SVidAudioDecoder->PushSamples(reinterpret_cast<const std::int16_t *>(buf), len / 2);
		} else {
			SVidAudioDecoder->PushSamples(reinterpret_cast<const std::uint8_t *>(buf), len);
		}
	}
#endif

	if (SDL_GetTicks() * 1000 >= SVidFrameEnd) {
		return SVidLoadNextFrame(); // Skip video if the system is to slow
	}

#ifndef USE_SDL1
	if (renderer != nullptr) {
		if (SDL_BlitSurface(SVidSurface, nullptr, GetOutputSurface(), nullptr) <= -1) {
			Log("{}", SDL_GetError());
			return false;
		}
	} else
#endif
	{
		SDL_Surface *outputSurface = GetOutputSurface();
#ifdef USE_SDL1
		const bool isIndexedOutputFormat = SDLBackport_IsPixelFormatIndexed(outputSurface->format);
#else
		const Uint32 wndFormat = SDL_GetWindowPixelFormat(ghMainWnd);
		const bool isIndexedOutputFormat = SDL_ISPIXELFORMAT_INDEXED(wndFormat);
#endif
		SDL_Rect outputRect;
		if (isIndexedOutputFormat) {
			// Cannot scale if the output format is indexed (8-bit palette).
			outputRect.w = static_cast<int>(SVidWidth);
			outputRect.h = static_cast<int>(SVidHeight);
		} else if (IsLandscapeFit(SVidWidth, SVidHeight, outputSurface->w, outputSurface->h)) {
			outputRect.w = outputSurface->w;
			outputRect.h = SVidHeight * outputSurface->w / SVidWidth;
		} else {
			outputRect.w = SVidWidth * outputSurface->h / SVidHeight;
			outputRect.h = outputSurface->h;
		}
		outputRect.x = (outputSurface->w - outputRect.w) / 2;
		outputRect.y = (outputSurface->h - outputRect.h) / 2;

		if (isIndexedOutputFormat
		    || outputSurface->w == static_cast<int>(SVidWidth)
		    || outputSurface->h == static_cast<int>(SVidHeight)) {
			if (SDL_BlitSurface(SVidSurface, nullptr, outputSurface, &outputRect) <= -1) {
				ErrSdl();
			}
		} else {
			// The source surface is always 8-bit, and the output surface is never 8-bit in this branch.
			// We must convert to the output format before calling SDL_BlitScaled.
#ifdef USE_SDL1
			SDLSurfaceUniquePtr converted { SDL_ConvertSurface(SVidSurface, ghMainWnd->format, 0) };
#else
			SDLSurfaceUniquePtr converted { SDL_ConvertSurfaceFormat(SVidSurface, wndFormat, 0) };
#endif
			if (SDL_BlitScaled(converted.get(), nullptr, outputSurface, &outputRect) <= -1) {
				Log("{}", SDL_GetError());
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
#ifndef NOSOUND
	if (HaveAudio()) {
		SVidAudioStream = std::nullopt;
		SVidAudioDecoder = nullptr;
	}
#endif

	if (SVidSMK != nullptr)
		smk_close(SVidSMK);

#ifndef DEVILUTIONX_STORM_FILE_WRAPPER_AVAILABLE
	SVidBuffer = nullptr;
#endif

	SDL_FreePalette(SVidPalette);
	SVidPalette = nullptr;

	SDL_FreeSurface(SVidSurface);
	SVidSurface = nullptr;

	memcpy(orig_palette, SVidPreviousPalette, sizeof(orig_palette));
#ifndef USE_SDL1
	if (renderer != nullptr) {
		SDL_DestroyTexture(texture);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, gnScreenWidth, gnScreenHeight);
		if (texture == nullptr) {
			ErrSdl();
		}
		if (renderer != nullptr && SDL_RenderSetLogicalSize(renderer, gnScreenWidth, gnScreenHeight) <= -1) {
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
