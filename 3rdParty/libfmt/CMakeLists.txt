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
  # branch: nxdk-v9.1.0
  FetchContent_Declare(libfmt
    URL https://github.com/diasurgical/fmt/archive/f69d48bf4049bbdfa74afca02fac3c81bb609dc8.tar.gz
    URL_HASH MD5=310c79c7a5cda3ad8f4e0d44d9fc1c57
  )
else()
  # master on 2022-03-12
  FetchContent_Declare(libfmt
    URL https://github.com/fmtlib/fmt/archive/7f882918eba6430a0509b5e8547de21611264c5c.tar.gz
    URL_HASH MD5=03fd08aeabf9021c6bf19fbd84d5e62e
  )
endif()
FetchContent_MakeAvailableExcludeFromAll(libfmt)
