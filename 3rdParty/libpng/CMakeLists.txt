include(functions/FetchContent_MakeAvailableExcludeFromAll)

if(NOT DISABLE_LTO)
  # Force CMake to raise an error if INTERPROCEDURAL_OPTIMIZATION
  # is enabled and compiler does not support IPO
  set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
endif()

if(DEVILUTIONX_STATIC_LIBPNG)
  set(PNG_LIBRARY png_static)
  set(PNG_STATIC ON)
  set(PNG_SHARED OFF)
else()
  set(PNG_LIBRARY png)
  set(PNG_STATIC OFF)
  set(PNG_SHARED ON)
endif()
set(PNG_TESTS OFF)
set(PNG_EXECUTABLES OFF)
set(SKIP_INSTALL_ALL ON)

if(APPLE AND "${CMAKE_OSX_ARCHITECTURES}" STREQUAL "arm64")
  set(PNG_ARM_NEON "on" CACHE STRING "" FORCE)
endif()

include(FetchContent)
FetchContent_Declare(libpng
  URL https://github.com/diasurgical/libpng/archive/3fe9510f01748b8c706550974e23b09166e5a42d.tar.gz
  URL_HASH MD5=5c813665b54143a1c2a89dd38b598075
)
FetchContent_MakeAvailableExcludeFromAll(libpng)

target_include_directories(${PNG_LIBRARY} INTERFACE
  $<BUILD_INTERFACE:${libpng_SOURCE_DIR}>
  $<BUILD_INTERFACE:${libpng_BINARY_DIR}>)

add_library(PNG::PNG ALIAS ${PNG_LIBRARY})
