#General compilation options
set(ASAN OFF)
set(UBSAN OFF)
set(DEVILUTIONX_SYSTEM_SDL2 OFF)
set(DEVILUTIONX_SYSTEM_LIBSODIUM OFF)
set(DEVILUTIONX_SYSTEM_LIBPNG OFF)
set(DEVILUTIONX_SYSTEM_BZIP2 OFF)
set(VIRTUAL_GAMEPAD ON)

if(BINARY_RELEASE OR CMAKE_BUILD_TYPE STREQUAL "Release")
  # Workaroudn linker bug in CLang: https://github.com/android/ndk/issues/721
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto=full")
endif()

set(DEVILUTIONX_ASSETS_OUTPUT_DIRECTORY "${DevilutionX_SOURCE_DIR}/android-project/app/src/main/assets")
set(BUILD_ASSETS_MPQ OFF)
