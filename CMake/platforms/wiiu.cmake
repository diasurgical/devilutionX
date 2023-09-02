#General compilation options
set(ASAN OFF)
set(UBSAN OFF)
set(BUILD_TESTING OFF)
set(BUILD_ASSETS_MPQ OFF)
set(DEVILUTIONX_SYSTEM_LIBSODIUM OFF)
set(DEVILUTIONX_SYSTEM_LIBFMT OFF)
set(DEVILUTIONX_SYSTEM_SDL_IMAGE OFF)
set(DEVILUTIONX_STATIC_LIBSODIUM ON)
set(DEVILUTIONX_STATIC_LIBFMT ON)
set(DISABLE_ZERO_TIER ON)
set(LIBMPQ_FILE_BUFFER_SIZE 32768)
set(PREFILL_PLAYER_NAME ON)
set(DEVILUTIONX_GAMEPAD_TYPE Nintendo)
set(NOEXIT ON)
set(NONET ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/threads-stub")

list(APPEND DEVILUTIONX_PLATFORM_COMPILE_DEFINITIONS __WIIU__)

# The wiiU build handles the stripping in a custom way.
set(DEVILUTIONX_DISABLE_STRIP ON)

# The SDL2 version in the pacman repos is currently several versions behind.
# The Wii U build is using a custom version for now.
# TODO: remove this once this version is upstreamed to the pacman repos
set(DEVILUTIONX_SYSTEM_SDL2 OFF)
