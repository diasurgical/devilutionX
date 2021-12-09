include(functions/FetchContent_MakeAvailableExcludeFromAll)

if(DEVILUTIONX_STATIC_SDL_AUDIOLIB)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

# No need for the libsamplerate resampler:
set(USE_RESAMP_SRC OFF)

# No need for the SOX resampler:
set(USE_RESAMP_SOXR OFF)

# We do not need any of the audio formats except WAV:
set(USE_DEC_DRWAV ON)
set(USE_DEC_DRFLAC OFF)
set(USE_DEC_OPENMPT OFF)
set(USE_DEC_XMP OFF)
set(USE_DEC_MODPLUG OFF)
set(USE_DEC_MPG123 OFF)
set(USE_DEC_SNDFILE OFF)
set(USE_DEC_LIBVORBIS OFF)
set(USE_DEC_LIBOPUSFILE OFF)
set(USE_DEC_MUSEPACK OFF)
set(USE_DEC_FLUIDSYNTH OFF)
set(USE_DEC_BASSMIDI OFF)
set(USE_DEC_WILDMIDI OFF)
set(USE_DEC_ADLMIDI OFF)
set(USE_DEC_DRMP3 OFF)

include(FetchContent)
FetchContent_Declare(SDL_audiolib
  URL https://github.com/realnc/SDL_audiolib/archive/aa79660eba4467a44f9dcaecf26b0f0a000abfd7.tar.gz
  URL_HASH MD5=7c79bb0d97f8469bbe5339061e910095)
FetchContent_MakeAvailableExcludeFromAll(SDL_audiolib)
