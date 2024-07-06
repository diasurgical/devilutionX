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
FetchContent_Declare(libfmt
  URL https://github.com/fmtlib/fmt/archive/92227c77a4eed4c12f97f7916041ace4d5ca02d4.tar.gz
  URL_HASH MD5=a60bc215a81a4ba770a7c1936311612f
)
FetchContent_MakeAvailableExcludeFromAll(libfmt)

if(DEVILUTIONX_WINDOWS_NO_WCHAR)
  target_compile_definitions(fmt PUBLIC FMT_USE_WRITE_CONSOLE)
endif()
