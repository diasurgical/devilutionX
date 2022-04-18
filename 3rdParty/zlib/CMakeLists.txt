include(functions/FetchContent_MakeAvailableExcludeFromAll)

include(FetchContent)
FetchContent_Declare(zlib
    URL https://www.zlib.net/zlib-1.2.12.tar.gz
    URL_HASH MD5=5fc414a9726be31427b440b434d05f78
)
FetchContent_MakeAvailableExcludeFromAll(zlib)

if(DEVILUTIONX_STATIC_ZLIB)
  set(_lib_type STATIC)
else()
  set(_lib_type SHARED)
endif()
add_library(ZLIB ${_lib_type}
  ${zlib_SOURCE_DIR}/crc32.h
  ${zlib_SOURCE_DIR}/gzguts.h
  ${zlib_SOURCE_DIR}/inffixed.h
  ${zlib_SOURCE_DIR}/inftrees.h
  ${zlib_BINARY_DIR}/zconf.h
  ${zlib_SOURCE_DIR}/zutil.h
  ${zlib_SOURCE_DIR}/deflate.h
  ${zlib_SOURCE_DIR}/inffast.h
  ${zlib_SOURCE_DIR}/inflate.h
  ${zlib_SOURCE_DIR}/trees.h
  ${zlib_SOURCE_DIR}/zlib.h
  ${zlib_SOURCE_DIR}/adler32.c
  ${zlib_SOURCE_DIR}/deflate.c
  ${zlib_SOURCE_DIR}/gzread.c
  ${zlib_SOURCE_DIR}/inffast.c
  ${zlib_SOURCE_DIR}/trees.c
  ${zlib_SOURCE_DIR}/compress.c
  ${zlib_SOURCE_DIR}/gzclose.c
  ${zlib_SOURCE_DIR}/gzwrite.c
  ${zlib_SOURCE_DIR}/inflate.c
  ${zlib_SOURCE_DIR}/uncompr.c
  ${zlib_SOURCE_DIR}/crc32.c
  ${zlib_SOURCE_DIR}/gzlib.c
  ${zlib_SOURCE_DIR}/infback.c
  ${zlib_SOURCE_DIR}/inftrees.c
  ${zlib_SOURCE_DIR}/zutil.c
)
target_include_directories(ZLIB PUBLIC ${zlib_SOURCE_DIR})
target_include_directories(ZLIB PUBLIC ${zlib_BINARY_DIR})

add_library(ZLIB::ZLIB ALIAS ZLIB)

set(ZLIB_LIBRARY ZLIB::ZLIB PARENT_SCOPE)
set(ZLIB_INCLUDE_DIR HINTS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} PARENT_SCOPE)
