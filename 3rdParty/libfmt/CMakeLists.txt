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
FetchContent_Declare(libfmt
  URL https://github.com/fmtlib/fmt/archive/dbbd711f4635838290a10b1b3d8d378937a8741b.zip
  URL_HASH MD5=0d7ec2a3a3a7c401d0b230dff194c119
)
FetchContent_MakeAvailableExcludeFromAll(libfmt)
