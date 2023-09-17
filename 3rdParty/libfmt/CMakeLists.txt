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
  URL https://github.com/fmtlib/fmt/archive/a8a73da7e44e26d7c18d752976522eff7a21e0bf.tar.gz
  URL_HASH MD5=4ddd617a0921e919f1c2e6ab7c36c349
)
FetchContent_MakeAvailableExcludeFromAll(libfmt)

if(DEVILUTIONX_WINDOWS_NO_WCHAR)
  target_compile_definitions(fmt PUBLIC FMT_WINDOWS_NO_WCHAR)
endif()
