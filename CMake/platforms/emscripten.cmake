set(BUILD_TESTING OFF)
set(DISABLE_ZERO_TIER ON)
set(DEVILUTIONX_SYSTEM_SDL_AUDIOLIB OFF)
set(DEVILUTIONX_SYSTEM_LIBSODIUM OFF)
set(DEVILUTIONX_SYSTEM_LIBFMT OFF)

# Emscripten ports do have a bzip2 but it fails to link with this error:
#   warning: _BZ2_bzDecompress may need to be added to EXPORTED_FUNCTIONS if it arrives from a system library
#   error: undefined symbol: BZ2_bzDecompressEnd (referenced by top-level compiled C/C++ code)
set(DEVILUTIONX_SYSTEM_BZIP2 OFF)
