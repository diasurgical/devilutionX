include(functions/FetchContent_MakeAvailableExcludeFromAll)

if(NOT WIN32 AND NOT APPLE AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
  # Enable POSIX extensions such as `readlink` and `ftruncate`.
  add_definitions(-D_POSIX_C_SOURCE=200809L)
  # Defining _POSIX_C_SOURCE should be enough for the compiler not to define  __STRICT_ANSI__
  # but some still do, so we also have to explicitly undefine it.
  add_compile_options(-U__STRICT_ANSI__)
endif()

if(DEVILUTIONX_STATIC_LIBFMT)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()
include(FetchContent)
FetchContent_Declare(fmt
  URL https://github.com/fmtlib/fmt/archive/7bdf0628b1276379886c7f6dda2cef2b3b374f0b.zip
  URL_HASH MD5=70e0a0beda4bdac7debe60d380e1d802
)
FetchContent_MakeAvailableExcludeFromAll(fmt)
