#pragma once
#ifndef NOSOUND

#include <memory>

#include <Aulib/Stream.h>

#ifdef DVL_AULIB_SUPPORTS_SDL_RESAMPLER
#include <Aulib/ResamplerSdl.h>
#else
#include <Aulib/ResamplerSpeex.h>
#endif

#include "options.h"

namespace devilution {

inline std::unique_ptr<Aulib::Resampler> CreateAulibResampler()
{
#ifdef DVL_AULIB_SUPPORTS_SDL_RESAMPLER
	return std::make_unique<Aulib::ResamplerSdl>();
#else
	return std::make_unique<Aulib::ResamplerSpeex>(*sgOptions.Audio.resamplingQuality);
#endif
}

} // namespace devilution

#endif // !NOSOUND
