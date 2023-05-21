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
  # branch: nxdk-v10.0.0
  FetchContent_Declare(libfmt
    URL https://github.com/diasurgical/fmt/archive/e421e854efcb21ee641349bab604d91b34af26b8.tar.gz
    URL_HASH MD5=ebaac86fca56d4ef4a162ef125433f6f
  )
else()
  FetchContent_Declare(libfmt
    URL https://github.com/fmtlib/fmt/archive/refs/tags/10.0.0.tar.gz
    URL_HASH MD5=fa629bc1178918b7af4b2ea6b6a271dc
  )
endif()
FetchContent_MakeAvailableExcludeFromAll(libfmt)
