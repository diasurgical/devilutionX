include(functions/FetchContent_MakeAvailableExcludeFromAll)

if(DEVILUTIONX_STATIC_SDL_AUDIOLIB)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

# Will use our `fmt::fmt` target if it exists.
set(WITH_SYSTEM_FMTLIB ON)

# No need for the libsamplerate resampler:
set(USE_RESAMP_SRC OFF)

# No need for the SOX resampler:
set(USE_RESAMP_SOXR OFF)

# Exceptions are only used for fatal errors which we can't handle anyway:
set(DISABLE_EXCEPTIONS ON)

# We do not need any of the audio formats except WAV and mp3:
set(USE_DEC_DRWAV ON)
set(USE_DEC_DRFLAC OFF)
set(USE_DEC_OPENMPT OFF)
set(USE_DEC_XMP OFF)
set(USE_DEC_MODPLUG OFF)
set(USE_DEC_MPG123 OFF)
set(USE_DEC_SNDFILE OFF)
set(USE_DEC_LIBVORBIS OFF)
set(USE_DEC_LIBOPUSFILE OFF)
set(USE_DEC_FLAC OFF)
set(USE_DEC_MUSEPACK OFF)
set(USE_DEC_FLUIDSYNTH OFF)
set(USE_DEC_BASSMIDI OFF)
set(USE_DEC_WILDMIDI OFF)
set(USE_DEC_ADLMIDI OFF)
set(USE_DEC_DRMP3 ON)

include(FetchContent)
FetchContent_Declare(SDL_audiolib
  URL https://github.com/realnc/SDL_audiolib/archive/cc1bb6af8d4cf5e200259072bde1edd1c8c5137e.tar.gz
  URL_HASH MD5=0e8174264ac9c6b314c6b2d9a5f72efd)
FetchContent_MakeAvailableExcludeFromAll(SDL_audiolib)

add_library(SDL_audiolib::SDL_audiolib ALIAS SDL_audiolib)
