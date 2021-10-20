#General compilation options
set(ASAN OFF)
set(UBSAN OFF)
set(DEVILUTIONX_SYSTEM_LIBSODIUM OFF)
set(DEVILUTIONX_SYSTEM_LIBPNG OFF)
set(VIRTUAL_GAMEPAD ON)

if(BINARY_RELEASE OR CMAKE_BUILD_TYPE STREQUAL "Release")
  # Workaroudn linker bug in CLang: https://github.com/android/ndk/issues/721
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto=full")
endif()

file(GLOB VirtualGamepadArt "${DevilutionX_SOURCE_DIR}/Packaging/resources/assets/ui_art/*")
file(COPY ${VirtualGamepadArt} DESTINATION "${DevilutionX_SOURCE_DIR}/android-project/app/src/main/assets/ui_art")
