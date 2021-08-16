#General compilation options
set(ASAN OFF)
set(UBSAN OFF)
set(DEVILUTIONX_SYSTEM_LIBSODIUM OFF)
set(DEVILUTIONX_SYSTEM_LIBPNG OFF)
set(DISABLE_ZERO_TIER ON)

if(BINARY_RELEASE OR CMAKE_BUILD_TYPE STREQUAL "Release")
  # Workaroudn linker bug in CLang: https://github.com/android/ndk/issues/721
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto=full")
endif()

#additional compilation definitions
set(TTF_FONT_DIR \"\")

file(
  COPY "${DevilutionX_SOURCE_DIR}/Packaging/resources/CharisSILB.ttf"
  DESTINATION "${DevilutionX_SOURCE_DIR}/android-project/app/src/main/assets")
