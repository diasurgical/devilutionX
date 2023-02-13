include(functions/FetchContent_MakeAvailableExcludeFromAll)

if(NOT WIN32 AND NOT APPLE AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
  # Enable POSIX extensions such as `readlink` and `ftruncate`.
  add_definitions(-D_POSIX_C_SOURCE=200809L)
endif()

if(DEVILUTIONX_STATIC_LIBFMT)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()
include(FetchContent)
if(NXDK)
  # branch: nxdk-2022-07-19
  FetchContent_Declare(libfmt
    URL https://github.com/diasurgical/fmt/archive/16d67608437f4fb9d293e561b4ddbccf3815a097.tar.gz
    URL_HASH MD5=7ef45573ef301c250717c60458aa7536
  )
else()
  FetchContent_Declare(libfmt
    URL https://github.com/fmtlib/fmt/archive/688a627d6c4d1fb6dfe070397b7cd4bf55a0024e.tar.gz
    URL_HASH MD5=55a7ac3c12ee83cc86b76275fd1e5f6b
  )
endif()
FetchContent_MakeAvailableExcludeFromAll(libfmt)
