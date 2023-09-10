include(functions/FetchContent_MakeAvailableExcludeFromAll)

if(NOT WIN32 AND NOT APPLE AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
  # Enable POSIX extensions such as `readlink` and `ftruncate`.
  add_definitions(-D_POSIX_C_SOURCE=200809L)
endif()

# Disable fmt/os.h functionality.
# We do not use it and it is not supported on some systems.
set(FMT_OS OFF)

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
    URL https://github.com/fmtlib/fmt/archive/refs/tags/10.1.1.tar.gz
    URL_HASH MD5=0d41a16f1b3878d44e6fd7ff1f6cc45a
  )
endif()
FetchContent_MakeAvailableExcludeFromAll(libfmt)
