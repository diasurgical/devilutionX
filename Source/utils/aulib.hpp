#pragma once
#ifndef NOSOUND

#include <memory>

#include <Aulib/Stream.h>

#ifdef DEVILUTIONX_RESAMPLER_SPEEX
#include <Aulib/ResamplerSpeex.h>
#endif

#ifdef DVL_AULIB_SUPPORTS_SDL_RESAMPLER
#include <Aulib/ResamplerSdl.h>
#endif

#include "options.h"

namespace devilution {

inline std::unique_ptr<Aulib::Resampler> CreateAulibResampler()
{
	switch (*sgOptions.Audio.resampler) {
#ifdef DEVILUTIONX_RESAMPLER_SPEEX
	case Resampler::Speex:
		return std::make_unique<Aulib::ResamplerSpeex>(*sgOptions.Audio.resamplingQuality);
#endif
#ifdef DVL_AULIB_SUPPORTS_SDL_RESAMPLER
	case Resampler::SDL:
		return std::make_unique<Aulib::ResamplerSdl>();
#endif
	}
	return nullptr;
}

} // namespace devilution

#endif // !NOSOUND
